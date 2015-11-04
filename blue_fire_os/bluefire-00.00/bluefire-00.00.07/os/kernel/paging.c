/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: paging.c
 *	Created: Jan 23, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// declared in assembly/start.asm
extern u32int var_system_memory_amount;
// defined in kernel.ld
extern u32int KERNEL_END_CODE;

// End of kernel virtual space
u32int *VIRTUAL_KERNEL_END;
// Free-frames stack is placed just above kernel memory
u32int *free_frames = (u32int *)&KERNEL_END_CODE;

/**************************************************************************
*	Free frame stack. Basically one word of memory is recored to represent
*	every physical group (page) of memory not used. Later on a more efficient
*	bitset algorithm will be used but for now things are kept simple as there
*	arn't to many kernel resources available for debugging.
**************************************************************************/
// ---------- Free frames stack operators ----------
u32int pop_frame() {
	u32int ret;
	u32int flags;

	disable_and_save_interrupts(flags);

	if (*free_frames != NULL) {
		ret = *free_frames;
		*free_frames = NULL;
		free_frames++;

		restore_interrupts(flags);
		return(ret);
	}

	// Out of memory
	restore_interrupts(flags);
	return NULL;
}

void push_frame(u32int p_addr) {
	u32int flags;

	disable_and_save_interrupts(flags);

	// Push the frame into free frames stack
	if ((u32int)free_frames > ((u32int)&KERNEL_END_CODE)) {
		free_frames--;
		*free_frames=p_addr;
	}

	restore_interrupts(flags);
}

/**************************************************************************
*	---------- Mapping operators ----------
*	Associates a Virtual address with a physical address
**************************************************************************/
// ---------- Initialization routine ----------
// Creates a list of all frames of physical memory available for use and stores
// the list at the virtual address starting above the end of the kernel.
// For our OS the list is all frames above 16 MB (below is reserved for BIOS,DMA,
// and Kernel) so for example the first available frame is 0x1000 and that is
// recorded at 0xC0015000
void init_free_frames() {
	u32int phys_addr;

	// -> 0xC0015000 - 0xC0030FFC : 0x(1000-8000)
	phys_addr = P_ADDR_16MB;		//0x1000 (0x1000 X Page size(0x1000) = 16 MB)
	VIRTUAL_KERNEL_END = free_frames;	//(KERNEL_TOP, dynamic) 0xC0015000 in the current example
	while (phys_addr < ADDR_TO_PAGE(var_system_memory_amount)) {
		*(VIRTUAL_KERNEL_END++) = phys_addr++;
	}

	// Last frame is NULL => out of physical memory.
	// Kernel virtual address space ends here:
	*VIRTUAL_KERNEL_END=NULL;
}

// ---------- Actual map routine ----------
s32int map_page(u32int vir_addr, u32int phys_addr, u16int attribs) {
	// Perform a page mapping for the current address space
	u32int *PTE;
	u32int i;
	u32int flags;

	disable_and_save_interrupts(flags);

	// Round virtual & physical address to the page boundary
	vir_addr = PAGE_ALIGN(vir_addr);
	phys_addr = PAGE_ALIGN(phys_addr);

	// Get only valid attributes
	attribs &= (PAGE_SIZE-1);


	// If the page directory entry is NULL must be created
	if (*VIRT_TO_PDE_ADDR(vir_addr) == NULL) {
		// Create a new page table
		PTE = (u32int *)(pop_frame() * PAGE_SIZE);
		if (PTE == NULL) {
			// Out of memory
			restore_interrupts(flags);
			return(FALSE);
		}

		// Set the PDE as present, user level, read-write
		*VIRT_TO_PDE_ADDR(vir_addr) = (u32int)PTE | P_PRESENT | P_USER | P_WRITABLE;

		// Invalidate the self-mapping page
		invlpg((u32int)VIRT_TO_PTE_ADDR(vir_addr));

		// NULL every PTE entry
		for (i=PAGE_DIR_ALIGN(vir_addr); i<PAGE_DIR_ALIGN_UP(vir_addr); i+=PAGE_SIZE) {
			*VIRT_TO_PTE_ADDR(i) = NULL;
		}
	}

	// Store the physical address into the page table entry
	*VIRT_TO_PTE_ADDR(vir_addr) = (u32int)phys_addr | attribs;

	// Invalidate the page in the TLB cache
	invlpg(vir_addr);

	restore_interrupts(flags);

	return(TRUE);
}

void delete_page(u32int addr) {
	// Unmap the page and destroy the physical frame where the address is mapped
	u32int temp;

	// Align address to the page boundary
	addr = PAGE_ALIGN(addr);

	if (*VIRT_TO_PDE_ADDR(addr) == NULL) return;
	if (*VIRT_TO_PTE_ADDR(addr) == NULL) return;

	u32int flags;
	disable_and_save_interrupts(flags);


	// Push the physical frame into the free frames stack
	if ((virtual_to_physical_address(addr) > PHYSICAL_DMA_MEMORY_END) && (virtual_to_physical_address(addr) < var_system_memory_amount))
		push_frame(virtual_to_physical_address(addr)/PAGE_SIZE);

	// Unmap the page
	*VIRT_TO_PTE_ADDR(addr) = NULL;

	// Invalidate the page in the TLB cache
	invlpg(addr);

	// Check if it is possible to deallocate the frame
	// of the page table used to map the address
	// So let's examine all entries in the page table
	// where the address is mapped.
	for( temp = PAGE_DIR_ALIGN(addr); temp < PAGE_DIR_ALIGN_UP(addr); temp += PAGE_SIZE ) {
		if (*VIRT_TO_PTE_ADDR(temp) != NULL){
			restore_interrupts(flags);
			return;
		}
	}

	// No PTEs found... deallocate the page table!
	push_frame(*VIRT_TO_PDE_ADDR(addr)/PAGE_SIZE);
	*VIRT_TO_PDE_ADDR(addr) = NULL;

	// Invalidate the self-mapping page
	invlpg((u32int)VIRT_TO_PTE_ADDR(addr));

	restore_interrupts(flags);
}

// Spinlock for mutual exclusion of temporary pages.
DECLARE_SPINLOCK( mem_temp_lock );

void *get_temp_page() {
	u08int *p = (u08int *)VIRTUAL_KERNEL_TMP_MEMORY_START;
	size_t frame;
	u32int flags;

	spin_lock_irqsave( &mem_temp_lock, flags );

	while( TRUE ) {
		if( *VIRT_TO_PDE_ADDR((size_t)p) ) {
			if( *VIRT_TO_PTE_ADDR((size_t)p) ) {
				p += PAGE_SIZE;
				if( p >= (u08int *)VIRTUAL_KERNEL_TMP_MEMORY_END ) {
					// Out of temporary memory!
					spin_unlock_irqrestore( &mem_temp_lock, flags );
					return( NULL );
				}
				continue;
			}
		}
		// OK! A free temporary page has been found!
		// Now map this page to a free frame and return the
		// virtual address of this page, or NULL if there is
		// no free frame.
		frame = pop_frame();
		if ( frame==NULL ) {
			p = NULL;
		} else if( !map_page((u32int)p, frame, P_PRESENT | P_WRITABLE) ){
			p = NULL;
		}
		spin_unlock_irqrestore( &mem_temp_lock, flags );
		return( (void *)p );
	}
}
/*
// ---------- Generic operators ----------
u32int virtual_to_physical_address(u32int vir_addr) {
	// Returns the physical address associated with the virtual address
	if (*VIRT_TO_PDE_ADDR(vir_addr) == NULL) return(NULL);
	return ((*VIRT_TO_PTE_ADDR(vir_addr) & -PAGE_SIZE) + (vir_addr % PAGE_SIZE));
}
*/
/**************************************************************************
*	---------- Page fault handler ----------
*	This function is hardcoded in the IDT and called on a page fault exception.
*	returns - A error code in case of failure.
**************************************************************************/
s32int page_fault_handler(u32int err_code, u32int cr2) {
	u32int phys_addr;

	phys_addr = pop_frame() * PAGE_SIZE;

	kprintf("\n allocated 0x%x -> 0x%x\n",cr2,phys_addr);

	// If out of memory return with a marked page fault panic
	if (phys_addr == NULL) 	{
		kset_color(LIGHT_RED);
		kprintf("\n\rPage fault handler panic: Out of memory!!!");
		kset_color(DEFAULT_COLOR);
		return TRUE;
	}

	// Get only valid err_code
	err_code &= (P_PRESENT | P_WRITABLE | P_USER);

	// Map page with correct attributes
	if (cr2 >= VIRTUAL_KERNEL_START) {
		if (!(map_page(cr2, phys_addr, P_PRESENT | P_WRITABLE))) {
			// Out of memory
			kset_color(LIGHT_RED);
			kprintf("\n\rPage fault handler panic: Out of memory!!!");
			kset_color(DEFAULT_COLOR);
			return TRUE;
		}
	} else {
		if (!(map_page(cr2, phys_addr, P_PRESENT | P_WRITABLE | P_USER))) {
			// Out of memory
			kset_color(LIGHT_RED);
			kprintf("\n\rPage fault handler panic: Out of memory!!!");
			kset_color(DEFAULT_COLOR);
			return TRUE;
		}
	}
	// Initialize the new page
	memset08((void *)PAGE_ALIGN(cr2), 0, PAGE_SIZE);

	return FALSE;
}

/******************************************************************************
*	Sets up everything we need for paging
******************************************************************************/
void initialize_paging() {
	//Set by the linker using kernel.ld
	extern size_t KERNEL_TEXT, KERNEL_END_TEXT;
	// Declared in assembly/exit.asm
	//extern void __task_exit_point;
	extern size_t __task_exit_point;

	u32int addr;

	// Initialize free frames stack
	init_free_frames();

	// Every process assumes it has the first 3GB of memory to its self. Until now
	// lower memory was identity mapped so 0x1000(V) = 0x1000(P) while the page directory that resided in it
	// was self mapped and addressable as either 0x1000(V) or 0xFFFFF000(V) both mapping to 0x1000(P). Now Lower
	// memory will be un-mapped and only the self mapped will work.
	// -> 0xFFFFF000 : 0x0
	*VIRT_TO_PDE_ADDR(0x0) = NULL;

	// The kernel already has it's paging entries entered by start.asm. Two things are needed..
	// - Mark every frame containing kernel code read only
	// - Mark the one page containing the function declared in exit.asm (and linked in to the kernel code)
	//		as user accessible, as all exiting processes will end up there. (it is aligned to a page/frame so
	//		by definition one entry need be marked.)
	for( addr = (size_t)&KERNEL_TEXT; addr < (size_t)&KERNEL_END_TEXT; addr += PAGE_SIZE )
		if( *VIRT_TO_PDE_ADDR(addr) )
			*VIRT_TO_PTE_ADDR( addr ) &= ~P_WRITABLE;

	// Make only the exit point address available for users. (entire PDE but only one frame)
	*VIRT_TO_PDE_ADDR( (size_t)&__task_exit_point ) |= P_USER;
	*VIRT_TO_PTE_ADDR( (size_t)&__task_exit_point ) |= P_USER;

	// Map physical memory into the kernel address space
	// Map the physical addresses of the first 16 MB of memory to Virtual addresses 0xE0000000 to 0xE1000000
	// V(0xE0000000, 0xE1000000)->P(0x00000000, 0x1000000)
	for(addr = 0; addr < LOWER_MEMORY_SIZE ; addr+=PAGE_SIZE ){
		map_page(VIRTUAL_LOWER_MEMORY_START+addr , addr, P_PRESENT | P_WRITABLE );
	}

	// Initialize temporary memory area.
	for( addr = VIRTUAL_KERNEL_TMP_MEMORY_START; addr < VIRTUAL_KERNEL_TMP_MEMORY_END; addr += PAGE_SIZE ){
		if( *VIRT_TO_PDE_ADDR(addr) ) {
			*VIRT_TO_PTE_ADDR(addr) = NULL;
		}
	}

	// Lower memory was un-identity mapped and re-mapped to 0xE0000000, however right now V(0x1000)->P(0x1000)
	// because the old TLB entries are still in the CPU, reloading cr3 empties this "cache". The other
	// Other choice is to invlpg a address manually.
	reload_CR3();
}

// ---------- Debug functions ----------
// Show all the dirty pages
void dump_dirty_pages() {
	u32int vir_addr;
	u32int display=1;

	// Print all the dirty pages
	kprintf("\nDirty pages:\n");
	for (vir_addr = 0; vir_addr < VIRTUAL_PAGE_TABLE_START; vir_addr += PAGE_SIZE) {
		if (*VIRT_TO_PDE_ADDR(vir_addr) != NULL) {
			if ((*VIRT_TO_PTE_ADDR(vir_addr) & P_DIRTY) == P_DIRTY) {
				if (!(++display % 24)){
					// No keyboard yet so can't pause
					dbg_pause(1);
				}
				kprintf("\nvir_addr = %X\tpage_entry = %X", vir_addr, *(VIRT_TO_PTE_ADDR(vir_addr)));
			}
		}
	}
	kprintf("\n");
}
void dump_free_frames() {
	u32int *f = free_frames;
	u32int display=1;

	kprintf("\nFree frames list: (KERNEL_TOP=%X)\n", (u32int)&KERNEL_END_CODE);
	for(;;) 	{
		if (*f == NULL) break;
		if (!(++display % 24)){
			// No keyboard yet so can't pause
			dbg_pause(1);
		}
		kprintf("\nframe #%X &frame=%X", *f, (u32int)f);
		f++;
	}
	kprintf("\n");
}
