/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: paging.h
 *	Created: Jan 23, 2012
 *	Last Update: Jan 23, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef PAGING_H_
#define PAGING_H_

// Paging Constants
#define PAGE_SIZE	4096U	//A unsigned c literal decimal (bytes)

#define P_PRESENT			0x01
#define P_WRITABLE			0x02
#define P_USER				0x04
#define P_WRITE_THROUGH		0x08
#define P_CHACHE_DISABLED	0x10
#define P_ACCESSED			0x20
#define P_DIRTY				0x40	// PTE only. Set by MMU
#define P_PAGE_SIZE			0x80	// PDE only. 0 for 4KB
#define P_GLOBAL			0x100	// PTE only.

/**************************************************************************
*	Page directory calculators
**************************************************************************/
// Returns which index entry of the Page Directory Table represents a virtual address
// Every index entry represents 0x400000 (4MB) of memory.
#define VIRT_TO_PDE_IDX(addr)	( addr / 0x400000)
// Returns the address of the page directory entry representing the specified address.
// The page directory table was permanently mapped to 0xFFFFF000 by start.asm
#define VIRT_TO_PDE_ADDR(addr)	(u32int *)(0xFFFFF000 + ((VIRT_TO_PDE_IDX(addr) * 0x4)&(~0x3)))
/**************************************************************************
*	Page table calculators
**************************************************************************/
// Returns which record of a arbitrary page table a virtual address is represented in (ie array index).
#define VIRT_TO_PTE_IDX(addr) 	((addr % 0x400000) / 0x1000)
// returns the starting address of the page table
#define VIRT_TO_PT_ADDR(addr)	(0xFFC00000 + (VIRT_TO_PDE_IDX(addr) * 0x1000))
// VIRT_TO_PTE_ADDR(addr) = &VIRT_TO_PT_ADDR(addr)[VIRT_TO_PTE_IDX(addr)]
#define VIRT_TO_PTE_ADDR(addr)	(u32int *)(VIRT_TO_PT_ADDR(addr) + ((VIRT_TO_PTE_IDX(addr) * 0x4)&(~0x3)))

/**************************************************************************
*	Return calculated values based on a virtual address
**************************************************************************/
// Rounds an address up to a page boundary (4K)
#define PAGE_ALIGN_UP(addr)	(((addr) + PAGE_SIZE - 1) & -PAGE_SIZE)
// Rounds an address down to a page boundary
#define PAGE_ALIGN(addr)	((addr) & -PAGE_SIZE)
// Rounds an address up to a page directory boundary (4MB)
#define PAGE_DIR_ALIGN_UP(addr)	(((addr) & -(PAGE_SIZE*1024)) + PAGE_SIZE*1024)
// Rounds an address down to a page directory boundary
#define PAGE_DIR_ALIGN(addr)	((addr) & -(PAGE_SIZE*1024))

// returns which page (not the address) a Virtual address is in
#define ADDR_TO_PAGE(addr)	(addr / PAGE_SIZE)
// The page where 16MB starts is hard coded as it is used a lot.
// ADDR_TO_PAGE(1000000) -> 0x1000
#define P_ADDR_16MB		0x1000

//! Access to an address into the mapped physical memory area.
#define PHYSICAL(addr)		((addr) + VIRTUAL_LOWER_MEMORY_START)

//! Convert a virtual address of the low memory area (DMA-memory) into
//! the equivalent physical address.
#define VIRTUAL(addr)		((addr) - VIRTUAL_LOWER_MEMORY_START)


// TODO
//! Determine the page size.
#define PAGE_SHIFT	12

//! Determine the memory area covered by a page directory entry.
#define PAGE_DIR_SHIFT	22
//! Get the page directory index of the current address.
#define PDE_INDEX( addr ) \
	( ((addr) >> PAGE_DIR_SHIFT) & 1023 )

//! Get the page table index of the current address.
#define PTE_INDEX( addr ) \
	( ((addr) >> PAGE_SHIFT) & 1023 )

//! Get the offset of a page directory entry.
#define PDE_OFFSET( pd, addr ) \
	( (u32int *)(pd) + PDE_INDEX( addr ) )

//! Get the offset of a page table entry.
#define PTE_OFFSET( pt, addr ) \
	( (u32int *)(pt) + PTE_INDEX( addr ) )
// \TODO

// Paging Procedures
// Because this OS is didactic, global pages are not supported or accounted for.
// clear tlb of one address
static __inline__ void invlpg(u32int page_addr) {
        __asm__ __volatile__ ("invlpg %0" : : "m" (page_addr));
}
// clear tlb of all address
static __inline__ void reload_CR3() {
	__asm__ __volatile__ ("movl %%cr3,%%eax ; movl %%eax,%%cr3" : : : "memory");
}

// Translate a virtual address into the physical address.
// vir_addr The virtual address to be translated.
static __inline__ size_t virtual_to_physical_address(size_t vir_addr) {
	if (*VIRT_TO_PDE_ADDR(vir_addr) == NULL)
		return(NULL);
	return( (*VIRT_TO_PTE_ADDR(vir_addr) & -PAGE_SIZE) + (vir_addr % PAGE_SIZE) );
}

// Switch into a new virtual address space.
// pdbr is the physical address of the page directory!
static __inline__ void switch_mmu( u32int pdbr ) {
	// Switch to the new virtual space.
	__asm__ __volatile__ ( "mov %0, %%cr3" : : "r"(pdbr) );
}

/******************************************************************************
 *			--------- PUBLIC FUNCTION DECLARATIONS ----------
******************************************************************************/
s32int page_fault_handler(u32int err_code, u32int cr2);
void initialize_paging();
void dump_free_frames();
void dump_dirty_pages();
void delete_page(u32int addr);
void *get_temp_page();

#endif /* PAGING_H_ */
