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
extern u32int KERNEL_TOP;

// End of kernel virtual space
u32int *K_VIR_END;
// Free-frames stack is placed just above kernel memory
u32int *free_frames = (u32int *)&KERNEL_TOP;

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
	if ((u32int)free_frames > ((u32int)&KERNEL_TOP)) {
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
	K_VIR_END = free_frames;	//(KERNEL_TOP, dynamic) 0xC0015000 in the current example
	while (phys_addr < ADDR_TO_PAGE(var_system_memory_amount)) {
		*(K_VIR_END++) = phys_addr++;
	}

	// Last frame is NULL => out of physical memory.
	// Kernel virtual address space ends here:
	*K_VIR_END=NULL;
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


/**************************************************************************
*	Sets up everything we need for paging
**************************************************************************/
void initialize_paging() {

	u32int addr;

	// Initialize free frames stack
	init_free_frames();

	// Every process assumes it has the first 3GB of memory to its self. Until now
	// lower memory was identity mapped so 0x1000(V) = 0x1000(P) while the page directory that resided in it
	// was self mapped and addressable as either 0x1000(V) or 0xFFFFF000(V) both mapping to 0x1000(P). Now Lower
	// memory will be un-mapped and only the self mapped will work.
	// -> 0xFFFFF000 : 0x0
	*VIRT_TO_PDE_ADDR(0x0) = NULL;

	// Map physical memory into the kernel address space
	// Map the physical addresses of the first 16 MB of memory to Virtual addresses 0xE0000000 to 0xE1000000
	// V(0xE0000000, 0xE1000000)->P(0x00000000, 0x1000000)
	for(addr = 0; addr < LOWER_MEMORY_SIZE ; addr+=PAGE_SIZE ){
		map_page(VIRTUAL_LOWER_MEMORY_START+addr , addr, P_PRESENT | P_WRITABLE );
	}

	// Lower memory was un-identity mapped and re-mapped to 0xE0000000, however right now V(0x1000)->P(0x1000)
	// because the old TLB entries are still in the CPU, reloading cr3 empties this "cache". The other
	// Other choice is to invlpg a address manually.
	reload_CR3();

	// Note for this early example video no longer works after this point (it is identity mapped in console.c)
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

	kprintf("\nFree frames list: (KERNEL_TOP=%X)\n", (u32int)&KERNEL_TOP);
	for(;;) 	{
		if (*f == NULL) break;
		if (!(++display % 24)){
			// No keyboard yet so can't pause
		}
		kprintf("\nframe #%X &frame=%X", *f, (u32int)f);
		f++;
	}
	kprintf("\n");
}
