/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: ext2.h
 *	Created: Feb 27, 2012
 *	Last Update: Feb 27, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef EXT2_H_
#define EXT2_H_

//Size of a sector on floppy
#define SIZE_SECTOR 	512

// ext2 magic number
// Before 0.2b it was 0xEF51 which we dont support.
#define EXT2_MAGIC_NUMBER 0xEF53 //Current magic number

// Defined Reserved Inodes
#define EXT2_BAD_INO 			0x01 //	Bad blocks inode
#define EXT2_ROOT_INO 			0x02 // Root directory inode
#define EXT2_ACL_IDX_INO 		0x03 // ACL index inode (deprecated)
#define EXT2_ACL_DATA_INO 		0x04 // ACL data inode (deprecated)
#define EXT2_BOOT_LOADER INO	0x05 // Boot loader inode
#define EXT2_UNDEL_DIR_INO 		0x06 // Undelete directory inode

// The layout of a disk looks like
// Name							(Length??)
// Boot Record
// super block
// block group descriptor table
// block bitmap
// inode bitmap
// inode table
// Data blocks
// repeats for backup....

// SUPERBLOCK
// The superblock contains all the information about the configuration of the filesystem.
// The information in the superblock contains fields such as the total number of inodes and blocks in the filesystem and how many are free,
// how many inodes and blocks are in each block group, when the filesystem was mounted (and if it was cleanly unmounted), when it was modified, what version of the filesystem it is and which OS created it.
// The primary copy of the superblock is stored at an offset of 1024 bytes from the start of the device, and it is essential to mounting the filesystem.
// Since it is so important, backup copies of the superblock are stored in block groups throughout the filesystem.

// What byte the super block resides on the disk.
// Must be hard coded because the disk calculations will be based on values not yet loaded.
#define START_SUPER_BLOCK 		1024
// Size of the super block on disk in ext2 blocks.
#define DIMENSION_SUPER_BLOCK 	1024

struct super_block{
	u32int s_inodes_count;		//The total number of inodes, both used and free, in the file system.
	u32int s_blocks_count;		//The total number of blocks in the system including all used, free and reserved.
	u32int s_r_blocks_count;	//The total number of blocks reserved for the usage of the super user.
	u32int s_free_blocks_count;	//The total number of free blocks, including the number of reserved blocks
	u32int s_free_inodes_count;	//The total number of free inodes. This is a sum of all free inodes of all the block groups.
	u32int s_first_data_block;	//The first data block, (id of the block containing the superblock structure).
	u32int s_log_block_size;	//The block size is computed using this 32bit value as the number of bits to shift left the value 1024 (block size = 1024 << s_log_block_size;)
	u32int s_log_frag_size;		//The fragment size is computed using this 32bit value as the number of bits to shift left the value 1024
	u32int s_blocks_per_group;	//The total number of blocks per group.
	u32int s_frags_per_group;	//The total number of fragments per group
	u32int s_inodes_per_group;	//The total number of inodes per group
	u32int s_mtime;				//The last time the file system was mounted.
	u32int s_wtime;				//The time of the last write access to the file system.
	u16int s_mnt_count;			//How many times the file system was mounted since the last time it was fully verified.
	u16int s_max_mnt_count;		//The maximum number of times that the file system may be mounted before a full check is performed.
	u16int s_magic;				//16bit value identifying the file system as Ext2, currently fixed to EXT2_SUPER_MAGIC (0xEF53).
	u16int s_state;				//File system state. When FS is mounted, set to EXT2_ERROR_FS. After unmounted, set to EXT2_VALID_FS. EXT2_ERROR_FS on next mount shows dirty unmount
	u16int s_errors;			//What the file system driver should do when an error is detected. (Continue, remount read-only, or panic)
	u16int s_minor_rev_level;	//the minor revision level within its revision level.
	u32int s_lastcheck;			//Time of the last file system check.
	u32int s_checkinterval;		//Maximum time interval allowed between file system checks.
	u32int s_creator_os;		//Identifier of the os that created the file system.
	u32int s_rev_level;			//Revision level
	u16int s_def_resuid;		//The default user id for reserved blocks.
	u16int s_def_resgid;		//The default group id for reserved blocks.
	u32int s_first_ino;			//Index to the first inode usable for standard files
	u16int s_inode_size;		//The size of the inode structure.
	u16int s_block_group_nr;	//the block group number hosting this superblock structure. Used to rebuild the FS from any superblock backup.
	u32int s_feature_compact;	//Bitmask of compatible features. File system implementation is free to support them or not without risk of damaging the meta-data.
	u32int s_feature_incompact;	//Bitmask of incompatible features. File system implementation should refuse to mount the file system if any of the indicated feature is unsupported.
	u32int s_feature_ro_compact;//Bitmask of "read-only" features. The file system implementation should mount as read-only if any of the indicated feature is unsupported.
	u08int s_uuid[16];			//Volume id. This should, as much as possible, be unique for each file system formatted.
	u16int s_volume_name;		//Volume name, mostly unused.
	u08int s_last_mounted[8];	//Directory path where the file system was last mounted.
	u32int s_algo_bitmap;		//Used by compression algorithms to determine the compression method(s) used.
	u08int s_reserved[886];		//Reserved for the operating system

};

// What ext2 block the Group descriptor on the disk.
//#define START_GROUP_DESCRIPTER		2048
#define START_GROUP_DESCRIPTER		2
//Size of the Group Descriptor on disk in ext2 blocks.
//#define DIMENSION_GROUP_DESCRIPTOR 	1024
#define DIMENSION_GROUP_DESCRIPTOR 	1
// BLOCK GROUP DESCRIPTOR
// The block group descriptor table is an array of block group descriptor, used to define parameters of all the block groups. It provides the location of the inode bitmap and inode table, block bitmap, number of free blocks and inodes, and some other useful information.
// The block group descriptor table starts on the first block following the superblock. This would be the third block on a 1KiB block file system, or the second block for 2KiB and larger block file systems.
// Shadow copies of the block group descriptor table are also stored with every copy of the superblock.
// Depending on how many block groups are defined, this table can require multiple blocks of storage.
// Always refer to the superblock in case of doubt.
struct group_descriptor{
	u32int bg_block_bitmap;		//Id of the first block of the "block bitmap" for the group represented.
	u32int bg_inode_bitmap;		//Id of the first block of the "inode bitmap" for the group represented.
	u32int bg_inode_table;		//Id of the first block of the "inode table" for the group represented.
	u16int bg_free_blocks_count;//The total number of free blocks for the represented group.
	u16int bg_free_inodes_count;//The total number of free inodes for the represented group.
	u16int bg_used_dirs_count;	//The number of inodes allocated to directories for the represented group.
	u16int bg_pad;				//Value used for padding the structure on a 32bit boundary.
	u32int bg_reserved[3];		//Reserved space for future revisions.
};

// Block Bitmap
// On small file systems, the "Block Bitmap" is normally located at the first block, or second block if a superblock backup is present, of each block group.
// Its official location can be determined by reading the "bg_block_bitmap" in its associated group descriptor.
// Each bit represent the current state of a block within that block group, where 1 means "used" and 0 "free/available".
// The first block of this block group is represented by bit 0 of byte 0, the second by bit 1 of byte 0. The 8th block is represented by bit 7 (most significant bit) of byte 0 while the 9th block is represented by bit 0 (least significant bit) of byte 1.

// Inode Bitmap
// The "Inode Bitmap" works in a similar way as the "Block Bitmap", difference being in each bit representing an inode in the "Inode Table" rather than a block.
// There is one inode bitmap per group and its location may be determined by reading the "bg_inode_bitmap" in its associated group descriptor.
// When the inode table is created, all the reserved inodes are marked as used. In revision 0 this is the first 11 inodes.

// Inode table
// The inode table is used to keep track of every directory, regular file, symbolic link, or special file; their location, size, type and access rights are all stored in inodes.
// There is no filename stored in the inode itself, names are contained in directory files only.
// There is one inode table per block group and it can be located by reading the bg_inode_table in its associated group descriptor.
// There are s_inodes_per_group inodes per table.
// Each inode contain the information about a single physical file on the system.
// A file can be a directory, a socket, a buffer, character or block device, symbolic link or a regular file.
// So an inode can be seen as a block of information related to an entity, describing its location on disk, its size and its owner.

// INODE
// Each inode contain the information about a single physical file on the system.
// A file can be a directory, a socket, a buffer, character or block device, symbolic link or a regular file.
// So an inode can be seen as a block of information related to an entity, describing its location on disk, its size and its owner.

// Values referenced by i_node.i_mode
//The mask used to read INode type
#define MODE_MASK 			0xF000 // format mask
// The flags that Inodes use to indicate what type of object they point to.
// Note, separate from the values that directory entries use.
#define MODE_FIFO 			0x1000 // FIFO (named pipe)
#define MODE_CHAR_DEVICE 	0x2000 // Character device
#define MODE_DIRECTORY 		0x4000 // Directory
#define MODE_BLOCK_DEVICE	0x6000 // Block device
#define MODE_FILE 			0x8000 // Regular file
#define MODE_SYMBOLIC_LINK 	0xA000 // Symbolic link
#define MODE_SOCKET 		0xC000 // Socket

struct i_node{
	u16int i_mode;				//Value used to indicate the format of the described file and the access rights.
	u16int i_uid;				//User id associated with the file.
	u32int i_size;				//The size of the file in bytes. In revision 1 a upper 32-bit is located in the i_dir_acl.
	u32int i_atime;				//The number of seconds since January 1st 1970 of the last time this inode was accessed.
	u32int i_ctime;				//The number of seconds since January 1st 1970, of when the inode was created.
	u32int i_mtime;				//The number of seconds since January 1st 1970, of the last time this inode was modified.
	u32int i_dtime;				//The number of seconds since January 1st 1970, of when the inode was deleted.
	u16int i_gid;				//Group ID having access to this file.
	u16int i_links_count;		//How many times this particular inode is linked (referred to). Every hardlink increases by 1
	u32int i_blocks;			//The total number of 512-bytes blocks reserved to contain the data of this inode, regardless of used.
	u32int i_flags;				//How the ext2 implementation should behave when accessing the data for this inode.
	u32int i_osd1;				//OS dependent value.
	u32int i_block[15];			// --See Below--
	u32int i_generation;		//Indicate the file version (used by NFS).
	u32int i_file_acl;			//The block number containing the extended attributes.
	u32int i_dir_acl;			//Upper portion 64bit file size (lower in i_size).
	u32int i_faddr;				//The location of the file fragment.
	//Linux specific OSD (Operating system Dependent)
	u08int l_i_frag;			//Fragment number.
	u08int l_i_fsize;			//Fragment size.
	u16int reserverd1;			//Reserved for future use.
	u16int l_i_uid_high;		//High 16bit of user id.
	u16int l_i_gid_high;		//High 16bit of group id.
	u16int reserverd2;			//Reserved for future use.

	//i_block ->
	//15 x 32bit block numbers pointing to the blocks containing the data for this inode. The first 12 blocks are direct blocks.
	//The 13th entry in this array is the block number of the first indirect block;
	//which is a block containing an array of block ID containing the data.
	//Therefore, the 13th block of the file will be the first block ID contained in the indirect block.
	//With a 1KiB block size, blocks 13 to 268 of the file data are contained in this indirect block.
	//
	//The 14th entry in this array is the block number of the first doubly-indirect block;
	//which is a block containing an array of indirect block IDs, with each of those indirect blocks containing an array of blocks containing the data.
	//In a 1KiB block size, there would be 256 indirect blocks per doubly-indirect block, with 256 direct blocks per indirect block for a total of 65536 blocks per doubly-indirect block.
	//
	//The 15th entry in this array is the block number of the triply-indirect block;
	//which is a block containing an array of doubly-indrect block IDs, with each of those doubly-indrect block containing an array of indrect block, and each of those indirect block containing an array of direct block.
	//In a 1KiB file system, this would be a total of 16777216 blocks per triply-indirect block.
	//A value of 0 in this array effectively terminates it with no further block being defined. All the remaining entries of the array should still be set to 0.


};


#define EXT2_FT_UNKNOWN         0
#define EXT2_FT_REG_FILE        1
#define EXT2_FT_DIR             2
#define EXT2_FT_CHRDEV          3
#define EXT2_FT_BLKDEV          4
#define EXT2_FT_FIFO            5
#define EXT2_FT_SOCK            6
#define EXT2_FT_SYMLINK         7

//maximum entries a directory can hold.
#define MAX_LOADED_DIRS 16

struct directory_entry {
	u32int inode; 			// Entry number of the file, 0 unused
	u16int record_length; 	//
	u08int name_length; 	//
	u08int file_type; 		//
	s08int name[256]; 		//
};

struct i_node_tab {
	u32int i_node_n; 		// Inode number
	struct i_node inode; 	// Actual inode
	u16int ref;				//
} ;
// Public Function declarations
s08int *pwd();
void mount_floppy();
//void ls();
s32int open_directory(struct i_node* current_i_node);
void debug_dump_superblock();
void debug_dump_descriptortable();
void debug_dump_current_directory();

#endif /* EXT2_H_ */
