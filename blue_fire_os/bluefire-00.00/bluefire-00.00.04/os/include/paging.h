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
#define PAGE_SIZE	4096U	//A unsigned c literal decimal

#define P_PRESENT	0x01
#define P_WRITE		0x02
#define P_USER		0x04
#define P_ACCESSED	0x20
#define P_DIRTY		0x40

// Addresses in pages
#define P_ADDR_16MB		0x1000

// Rounds an address up to a page boundary
#define PAGE_ALIGN_UP(addr)	(((addr) + PAGE_SIZE - 1) & -PAGE_SIZE)
// Rounds an address down to a page boundary
#define PAGE_ALIGN(addr)	((addr) & -PAGE_SIZE)
// Rounds an address up to a page directory boundary
#define PAGE_DIR_ALIGN_UP(addr)	(((addr) & -(PAGE_SIZE*1024)) + PAGE_SIZE*1024)
// Rounds an address down to a page directory boundary
#define PAGE_DIR_ALIGN(addr)	((addr) & -(PAGE_SIZE*1024))

// Converts from address to page
#define ADDR_TO_PAGE(addr)	(addr / PAGE_SIZE)
// Divides the addressable space into 4096 parts and returns which part (addr) is in.
#define ADDR_TO_PDE(addr)	(u32int *)(VIRTUAL_PAGE_DIRECTORY_MAP + (((u32int) (addr) / (1024 * 1024))&(~0x3)))
// Divides the 1,048,576 memory portion as calculated above in to 1024 parts and returns which part (addr) is in.
#define ADDR_TO_PTE(addr)	(u32int *)(VIRTUAL_PAGE_TABLE_MAP + ((((u32int) (addr) / 1024))&(~0x3)))

// Paging Procedures
static __inline__ void invlpg(u32int page_addr) {
        __asm__ __volatile__ ("invlpg %0" : : "m" (page_addr));
}

static __inline__ void reload_CR3() {
	__asm__ __volatile__ ("movl %%cr3,%%eax ; movl %%eax,%%cr3" : : : "memory");
}


// Public Function declarations
void initialize_paging();
void dump_free_frames();
void dump_dirty_pages();

#endif /* PAGING_H_ */
