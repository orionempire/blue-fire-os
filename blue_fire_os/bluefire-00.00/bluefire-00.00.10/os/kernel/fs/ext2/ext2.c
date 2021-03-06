/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: ext2.c
 *	Created: Feb 27, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

//The current path
s08int path[256];
//mounted file system sentinel
s32int mount_sentinel = 0;

// --- Pointers to file system descriptors loaded off the disk ---
// pointer to the superblock
struct super_block 		*superblock = 0;
// pointer to the group descriptor table
struct group_descriptor *group_descriptor_table = 0;
//block read off disk that holds the inode we want
struct i_node 			*current_inode_block = 0;

struct i_node 			new_inode;


// --- Values calculated from the SuperBlock
u32int formated_block_size;
u32int fragment_size;
u32int number_of_groups;
u32int inodes_per_indirect;
u16int inodes_per_block;
u16int sectors_per_block;


// --- Variables representing a open file object ---
// A list of pointers in LBA to all the blocks needed by our current file object.
u32int *pointer_directory;
// Number of pointers loaded in to pointer_directory
u32int number_blocks_loaded = 0;

// --- Variables representing a open directory object ---
// Parsed directory
struct directory_entry parsed_directory[MAX_LOADED_DIRS];
// Amount of entries in parsed directory
u32int parsed_directory_size;
u32int block_to_LBA(u32int block_number) {
		return block_number * sectors_per_block;
}

u32int block_to_bytes(u32int block_count) {
		return block_count * formated_block_size;
}

s32int is_dir(struct i_node* current_inode){
	if ((current_inode->i_mode & MODE_MASK) == MODE_DIRECTORY) {
		return TRUE;
	}
	return FALSE;
}

s32int is_fast_symbolic_link(struct i_node* current_inode){
	if ((current_inode->i_mode & MODE_MASK) == MODE_SYMBOLIC_LINK) {
		return TRUE;
	}
	return FALSE;
}

struct group_descriptor * get_group_descriptor(u32int group_index){

	if(group_index > (number_of_groups - 1)) {
		kprintf ("\n\rERROR - Invalid group descriptor number");
		return NULL;
	}
	return( &group_descriptor_table[group_index] );
}

// ---Get the actual portions off the disk---
u32int inode_to_block(u32int i_node_number) {

	struct group_descriptor* temp_group_descriptor;

	if (!i_node_number || i_node_number > superblock->s_inodes_count) {
		kprintf("\n\rERROR - Invalid inode number ->%i",i_node_number);
		return 0;
	}


	//Inodes are indexed from 1 not 0
	i_node_number--;

	temp_group_descriptor = get_group_descriptor( i_node_number / superblock->s_inodes_per_group);	//(1 / 184)

	if (!temp_group_descriptor) {
		kprintf ("\nERROR - Could not find group descriptor ");
		return 0;
	}

	i_node_number %= superblock->s_inodes_per_group;

	return (temp_group_descriptor->bg_inode_table + ( i_node_number / inodes_per_block));
}


s32int read_inode_block(u32int i_node_number){

	u32int sentinal = FALSE;
	u32int inode_block = inode_to_block(i_node_number);

	//make sure that the requested inode exists.
	if (!inode_block) {
		kset_color(LIGHT_RED);
		kprintf("\n\rERROR - Requested INode block %i does not exist...\n\r" , inode_block);
		return NULL;
	}

	if(!current_inode_block) {
		current_inode_block = kmalloc(formated_block_size, GFP_KERNEL);
	}

	memset08(current_inode_block, 0, formated_block_size);
	sentinal = fdc_read( block_to_LBA(inode_block), (u08int *)(current_inode_block), block_to_LBA(1));

	if( sentinal == FALSE){
		kset_color(LIGHT_RED);
		kprintf("\n\rFatal Error loading requested inode block\n\r");
		return NULL;
	}
	return TRUE;
}

// D -> i_node_number = 2
struct i_node *get_inode(u32int i_node_number) {
	u32int sentinal = FALSE;


	//make sure that the requested inode exists.
	if ((!i_node_number) || (i_node_number > superblock->s_inodes_count)) {
		kprintf("\nERROR - Requested inode %i does not exist..." , i_node_number);
		return NULL;
	}

	//load the proper inode block int o the global variable current_inode_block
	sentinal = read_inode_block(i_node_number);

	if( sentinal == FALSE){
		kset_color(LIGHT_RED);
		kprintf("\n\rFatal Error reading inode\n\r");
		halt();
	}

	//Inodes are indexed from 1 not 0
	memcpy08(&new_inode, &current_inode_block[i_node_number - 1], sizeof(struct i_node));

	kfree(current_inode_block);

	return &new_inode;

}

// Variables needed for directory usage.
s08int** ext2_file_name = NULL;

s32int open_directory(struct i_node* current_i_node) {
	// Pointer to actual data block of directory
	u32int* current_directory = 0;

	u32int sentinal = FALSE;
	u32int* current_location_pointer;
	u32int current_location;
	s32int x;

	u32int final_location;
	struct directory_entry *tem_dir;

	if (!(is_dir(current_i_node) || is_fast_symbolic_link(current_i_node))){
		return FALSE;
	}

	if (ext2_file_name != NULL) {
		kfree(ext2_file_name);
	}

	//if (!(open_file(current_i_node , current_i_node->i_mode & MODE_MASK))) {
	//	return FALSE;
	//}

	if(current_directory) { kfree(current_directory); }
	current_directory = (u32int *)kmalloc(current_i_node->i_size, GFP_KERNEL);
	current_location_pointer = current_directory;

	for( x = 0; x < number_blocks_loaded; x++ ) {
		sentinal = fdc_read(block_to_LBA( pointer_directory[x]),(u08int *)current_location_pointer, block_to_LBA(1));

		if( sentinal == FALSE){
			kset_color(LIGHT_RED);
			kprintf("\n\rFatal Error - reading directory block\n\r");
			halt();
		}
		current_location_pointer += formated_block_size;
	}

	current_location = (u32int)current_directory;
	final_location = ((u32int)current_directory + current_i_node->i_size);
	x = 0;

	while (current_location < final_location) {
		if(x > MAX_LOADED_DIRS) {
			kprintf("\nFatal Error - to many directory entries");
			halt();
		}
		tem_dir = (struct directory_entry*)current_location;
		parsed_directory[x].inode = tem_dir->inode;
		parsed_directory[x].name_length = tem_dir->name_length;
		parsed_directory[x].file_type = tem_dir->file_type;

		memcpy08(parsed_directory[x].name, tem_dir->name, tem_dir->name_length);
		x++;
		current_location += tem_dir->record_length;
	}

	parsed_directory_size = x;

	return TRUE;
}

void load_root_directory() {
	u32int sentinal = FALSE;

	sentinal = open_directory(get_inode(EXT2_ROOT_INO));

	if( sentinal == FALSE){
		kset_color(LIGHT_RED);
		kprintf("\n\rFatal Error Loading root directory\n\r");
		halt();
	}
}


void load_group_descriptor() {
	u32int sentinal = FALSE;

	if (group_descriptor_table) { kfree(group_descriptor_table); }
	group_descriptor_table = kmalloc(block_to_bytes(DIMENSION_GROUP_DESCRIPTOR), GFP_KERNEL);
	memset08(group_descriptor_table, 0, block_to_bytes(DIMENSION_GROUP_DESCRIPTOR));

	sentinal = fdc_read(block_to_LBA(START_GROUP_DESCRIPTER),(u08int *)group_descriptor_table, block_to_LBA(DIMENSION_GROUP_DESCRIPTOR));

	if( sentinal == FALSE){
		kset_color(LIGHT_RED);
		kprintf("\nFatal Error mounting file system.");
		halt();
	}


}

// ---Load the sections of the disk---
void load_super_block() {
	u32int sentinal = FALSE;

	if (superblock) {
		kfree(superblock);
	}

	superblock = kmalloc(DIMENSION_SUPER_BLOCK, GFP_KERNEL);
	memset08(superblock, 0, DIMENSION_SUPER_BLOCK);

	sentinal = fdc_read((START_SUPER_BLOCK/FDC_SECTOR_SIZE),(u08int *)superblock,(DIMENSION_SUPER_BLOCK / FDC_SECTOR_SIZE));

	if( sentinal == FALSE){
		kset_color(LIGHT_RED);
		kprintf("\nFatal Error mounting file system\n");
		halt();
	}

	//Sanity check by looking at magic number
	if (superblock->s_magic != EXT2_MAGIC_NUMBER) {
		kset_color(LIGHT_RED);
		kprintf("\nFatal Error verifying file system\n");
		halt();
	}
}

// Return the current path
s08int *pwd() {
	return(path);
}

//mount the floppy's ext2 file system
void mount_floppy() {

	if (mount_sentinel) {
		kprintf("\nERROR - File system already mounted!");
		return;
	}

	kprintf("\nInitializing FileSystem...");
	// Initialize the path
	path[0]='\0';

	load_super_block();

	kprintf("\nLoaded Super Block...");

	//Calculate the derived numbers
	formated_block_size = (1024 << (superblock->s_log_block_size));	//Size of block. Decided during the formating of the disk.
	fragment_size = 1024 << superblock->s_log_frag_size;			//
	number_of_groups = (superblock->s_inodes_count / superblock->s_inodes_per_group);	//
	inodes_per_block = formated_block_size / superblock->s_inode_size;	//
	sectors_per_block = formated_block_size / FDC_SECTOR_SIZE;
	inodes_per_indirect = formated_block_size / sizeof(u32int);

	load_group_descriptor();
	kprintf("\nLoaded Group Descriptor...\n");

	//load_root_directory();
	//kprintf("\nLoaded Root Directory...");
}

void debug_dump_descriptortable() {
	int i;
	struct group_descriptor  *group_descriptor;

	for(i = 0; i < number_of_groups; i++){
		group_descriptor  = &group_descriptor_table[i];
		kprintf("\n----Printing Entry %i of %i---\n\r", i + 1 ,number_of_groups);
		kprintf("\nId of first block of Block bitmap :\t\t\t%i",group_descriptor->bg_block_bitmap);		//8
		kprintf("\nId of first block of Inode bitmap :\t\t\t%i",group_descriptor->bg_inode_bitmap);		//9
		kprintf("\nId of first block of Inode Table :\t\t\t%i",group_descriptor->bg_inode_table);			//10
		kprintf("\nNumber of free blocks in this group :\t\t\t%i",group_descriptor->bg_free_blocks_count);	//1219
		kprintf("\nNumber of free inodes in this group :\t\t\t%i",group_descriptor->bg_free_inodes_count);	//165
		kprintf("\nNumber inodes allocated to directories:\t\t\t%i",group_descriptor->bg_used_dirs_count);	//5
	}
	kprintf("\n");
}

void debug_dump_superblock() {

	kprintf("\n----Loaded Entries---");
	kprintf("\nTotal number of inodes:\t\t\t%i", superblock->s_inodes_count);								//56
	kprintf("\nTotal number of block:\t\t\t%i", superblock->s_blocks_count);								//400
	kprintf("\nBlocks reserved for the super user:\t%i", superblock->s_r_blocks_count );					//20
	kprintf("\nTotal number of free blocks:\t%i", superblock->s_free_blocks_count );						//373
	kprintf("\nTotal number of free inodes:\t%i ",superblock->s_free_inodes_count );						//45
	kprintf("\nId of block containing the superblock structure :\t%i ",superblock->s_first_data_block );	//1
	kprintf("\nBlock Size:\t\t\t\t%i", formated_block_size);												//1024 (1024 Shifted left 0 spaces)
	kprintf("\nFragment Size:\t\t\t\t%i", fragment_size);													//1024 (1024 Shifted left 0 spaces)
	kprintf("\nNumber of blocks per group:\t%i ",superblock->s_blocks_per_group);							//8192
	kprintf("\nNumber of fragments per group:\t%i ",superblock->s_frags_per_group );						//8192
	kprintf("\nInodes per group:\t\t\t%i", superblock->s_inodes_per_group);									//56
	kprintf("\nLast recorded mount time:\t%i", superblock->s_mtime);										//0 (varies)
	kprintf("\nLast recorded write time:\t%i", superblock->s_wtime);										//1320245118 (varies)
	kprintf("\nTimes file system mounted since last check:\t%i", superblock->s_mnt_count);					//1 (varies)
	kprintf("\nMaximum times file system may be mounted before check:\t%i", superblock->s_max_mnt_count);	//32 (varies)
	kprintf("\nMagic number\t%X", superblock->s_magic);														//0xEF53
	// s_state = 1 (), s_errors = 1, s_minor_rev_level = 0, s_lastcheck = (varies), s_checkinterval= (varies)
	// s_creator_os = 0 (), s_rev_level = 1, s_def_resuid = 0, s_def_resgid = 0;
	kprintf("\nIndex to the first inode usable for standard files:\t%i", superblock->s_first_ino);			//11
	kprintf("\nSize of Inode structure:\t%i", superblock->s_inode_size);									//128
	// s_block_group_nr = 0;,s_feature_compact = 00111000 (56), s_feature_incompact = 00000010 (2), s_feature_ro_compact = 00000001 (1)
	// s_uuid[16] = (varies), s_volume_name = 0,  s_last_mounted[8] = 0, s_algo_bitmap= 0;
	kprintf("\n----Calculated Entries---");
	kprintf("\nNumber of groups:\t\t\t\t%i", number_of_groups);												//1
	kprintf("\nInodes Per Group:\t\t\t\t%i", inodes_per_block);												//8
	kprintf("\nSectors per block:\t\t\t\t%i", sectors_per_block);											//2
	kprintf("\nNumber of INode records in a indirect block %X:\t\t\t\t%i", inodes_per_indirect);			//0x100
	kprintf("\n");

}

void debug_dump_current_directory(){
	u32int x;

	kprintf("\n name\tinode\tname_length\ttype");
	for( x = 0; x < parsed_directory_size; x++){
		kprintf ("\n%s",parsed_directory[x].name );
		kprintf ("\t%X",parsed_directory[x].inode );
		kprintf ("\t%i",parsed_directory[x].name_length );
		kprintf ("\t%X",parsed_directory[x].file_type );
		kprintf ("\n");
	}
}

//kprintf ("\n\rDebug - inode_block -> %X",inode_block );

/*
int z =0;
kprintf("\n\r");
for(z = 1; z < 50; z++){
	kprintf("%X " , *((u08int *)current_inode_block+(z-1)));
	if((z % 7) == 0){
		kprintf("\n\r");
	}
}
*/


