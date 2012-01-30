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

// Master page directory
u32int K_PDBR[1024];

// ---------- Free frames stack operators ----------
u32int pop_frame() {
	u32int ret;
	u32int flags;

	disable_interrupts(flags);

	if (*free_frames != NULL) {
		ret = *free_frames;
		*free_frames = NULL;
		free_frames++;

		enable_interrupts(flags);
		return(ret);
	}

	// Out of memory
	enable_interrupts(flags);
	return NULL;
}

void push_frame(u32int p_addr) {
	u32int flags;

	disable_interrupts(flags);

	// Push the frame into free frames stack
	if ((u32int)free_frames > ((u32int)&KERNEL_TOP)) {
		free_frames--;
		*free_frames=p_addr;
	}

	enable_interrupts(flags);
}

// ---------- Mapping operators ----------
s32int map_page(u32int vir_addr, u32int phys_addr, u16int attribs) {
	// Perform a page mapping for the current address space
	u32int *PTE;
	u32int i;
	u32int flags;

	disable_interrupts(flags);

	// Round virtual & physical address to the page boundary
	vir_addr = PAGE_ALIGN(vir_addr);
	phys_addr = PAGE_ALIGN(phys_addr);

	// Get only valid attribs
	attribs &= (PAGE_SIZE-1);


	// If the page directory entry is NULL must be created
	if (*ADDR_TO_PDE(vir_addr) == NULL) {
		// Create a new page table
		PTE = (u32int *)(pop_frame() * PAGE_SIZE);
		if (PTE == NULL) {
			// Out of memory
			enable_interrupts(flags);
			return(FALSE);
		}

		// Set the PDE as present, user level, read-write
		*ADDR_TO_PDE(vir_addr) = (u32int)PTE | P_PRESENT | P_USER | P_WRITE;

		// Invalidate the self-mapping page
		invlpg((u32int)ADDR_TO_PTE(vir_addr));

		// NULL every PTE entry
		for (i=PAGE_DIR_ALIGN(vir_addr); i<PAGE_DIR_ALIGN_UP(vir_addr); i+=PAGE_SIZE) {
			*ADDR_TO_PTE(i) = NULL;
		}

		// Update master page directory
		if (vir_addr >= VIRTUAL_KERNEL_START) {
			K_PDBR[vir_addr/(PAGE_SIZE*1024)] = *ADDR_TO_PDE(vir_addr);
		}
	}

	// Store the physical address into the page table entry
	*ADDR_TO_PTE(vir_addr) = (u32int)phys_addr | attribs;

	// Invalidate the page in the TLB cache
	invlpg(vir_addr);

	enable_interrupts(flags);

	return(TRUE);
}
// ---------- Initialization routines ----------
void init_free_frames() {
	u32int p_addr;

	// First physical 16MB are reserved for kernel, BIOS & DMA
	// so let's start with free memory area at 16MB
	// -> 0xC0015000 - 0xC0031000 : 0x(1000-8000)
	p_addr = P_ADDR_16MB;		//0x1000
	K_VIR_END = free_frames;	//(KERNEL_TOP, dynamic) 0xC0015000 in the current example
	while (p_addr < ADDR_TO_PAGE(var_system_memory_amount)) {
		*(K_VIR_END++) = p_addr++;
	}

	// Last frame is NULL => out of physical memory.
	// Kernel virtual address space ends here:
	*K_VIR_END=NULL;
}

/**************************************************************************
*	Sets up everything we need for paging
**************************************************************************/
void initialize_paging() {
	// TODO flush the tlb?
	u32int addr;

	// Initialize free frames stack
	init_free_frames();

	// Unmap first 4MB identical-map pages
	// for now on the self mapped page directory entry is used
	// so what was once 0x1000 is now 0xFFFFF000
	// -> 0xFFFFF000 : 0x0
	*ADDR_TO_PDE(0) = NULL;

	// Map physical memory into the kernel address space
	// Map the physical addresses of the first 16 MB of memory to Virtual addresses 0xEF000000 to 0xF0000000
	for (addr=VIRTUAL_ACTUAL_MEMORY_START; addr<VIRTUAL_ACTUAL_MEMORY_END; addr+=PAGE_SIZE) {
		map_page(addr, addr-VIRTUAL_ACTUAL_MEMORY_START, P_PRESENT | P_WRITE);
	}

	// Initialize master page directory
	// The master page directory is a manually maintained record of the kernel's page directory
	// Processes will us it when they fork as a base of their page tables.
	for (addr=0; addr<1024; addr++) {
		K_PDBR[addr] = ((u32int *)VIRTUAL_PAGE_DIRECTORY_MAP)[addr];
	}
}

// ---------- Debug functions ----------
// Show all the dirty pages
void dump_dirty_pages() {
	u32int vir_addr;
	u32int display=1;

	// Print all the dirty pages
	kprintf("\nDirty pages:\n");
	for (vir_addr = 0; vir_addr<VIRTUAL_PAGE_TABLE_MAP; vir_addr += PAGE_SIZE) {
		if (*ADDR_TO_PDE(vir_addr) != NULL) {
			if ((*ADDR_TO_PTE(vir_addr) & P_DIRTY) == P_DIRTY) {
				if (!(++display % 24)){
					//if (getchar() == CTRL_C) { // no keyboard yet
					//	printf("\n\r");
					//	return;
					//}
				}
				kprintf("\nvir_addr = %X\tpage_entry = %X", vir_addr, *(ADDR_TO_PTE(vir_addr)));
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
			//if (getchar() == CTRL_C) { // no keyboard yet
			//	printf("\n\r");
			//	return;
			//}
		}
		kprintf("\nframe #%X &frame=%X", *f, (u32int)f);
		f++;
	}
	kprintf("\n");
}
