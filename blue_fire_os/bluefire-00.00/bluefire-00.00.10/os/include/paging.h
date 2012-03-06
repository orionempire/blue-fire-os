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
// The page where 16MB starts is hard coded as it is used a lot.
// ADDR_TO_PAGE(1000000) -> 0x1000
#define P_ADDR_16MB		0x1000

/**************************************************************************
*	Page look up routines
*	To find a page the kernel, the kernel must get values from two tables.
*	The first table is called the "Page Directory", each entry in it is called
*	a "Page Directory Entry (PDE)".
*	The Second table is called the "Page Table" which each entry called
*	a "Page Table Entry (PTE)".
*	To find to take a Virtual address and find find what physical address to read...
*	First,
*		the PDE is stored at 0x1000(P) which is addressable as 0xFFFFF000(V)
*	Next
*		the 4,294,967,295 bytes of addressable space is divided into 1,048,576 parts
*		and the proper entry is read from the PDE.
*		So for example... (0 = the first entry)  (C0000000 = the 3072 entry)
*	Next
*		the entry stored in that PDE is used as a physical address that represents the PTE
*		which is a block of 1024 4 bytes entries. That entry is the upper 20 bits of the physical
*		address while the lower 12 comes from the last bits of the virtual address.
*
*	The astute observer might note that this has turned one expensive memory operation in to 3.
*	The answer is the Translation Lookaside Buffer (TLB). In reality the TLB usually has a
*	99% hit rate but the downside is that it must be maintained.
**************************************************************************/
// Divides the addressable space into 4096 parts and returns which part (addr) is in.
#define ADDR_TO_PDE(addr)	(u32int *)(VIRTUAL_PAGE_DIRECTORY_MAP + (((u32int) (addr) / (1024 * 1024))&(~0x3)))
// Divides the 1,048,576 memory portion as calculated above in to 1024 parts and returns which part (addr) is in.
#define ADDR_TO_PTE(addr)	(u32int *)(VIRTUAL_PAGE_TABLE_MAP + ((((u32int) (addr) / 1024))&(~0x3)))

/**************************************************************************
*	Return calculated values based on a virtual address
**************************************************************************/
// Rounds an address up to a page boundary
#define PAGE_ALIGN_UP(addr)		(((addr) + PAGE_SIZE - 1) & -PAGE_SIZE)
// Rounds an address down to a page boundary
#define PAGE_ALIGN(addr)		((addr) & -PAGE_SIZE)
// Rounds an address up to a page directory boundary
#define PAGE_DIR_ALIGN_UP(addr)	(((addr) & -(PAGE_SIZE*1024)) + PAGE_SIZE*1024)
// Rounds an address down to a page directory boundary
#define PAGE_DIR_ALIGN(addr)	((addr) & -(PAGE_SIZE*1024))


// returns which page (not the address) a Virtual address is in
#define ADDR_TO_PAGE(addr)		(addr / PAGE_SIZE)
// Converts from page to address
#define PAGE_TO_ADDR(p_addr)	(p_addr * PAGE_SIZE)
// Get current page directory base address
#define GET_PDBR()				( (*ADDR_TO_PDE(VIRTUAL_PAGE_TABLE_MAP)) & (-PAGE_SIZE) )


//Access to an address into the mapped physical memory area.
#define PHYSICAL(addr)		((addr) + VIRTUAL_ACTUAL_MEMORY_START)
/**************************************************************************
* Paging Procedures
**************************************************************************/
// Invalidate a single page entry
static __inline__ void invlpg(u32int page_addr) {
        __asm__ __volatile__ ("invlpg %0" : : "m" (page_addr));
}

// Invalidate all the TLB entries
static __inline__ void reload_CR3() {
	__asm__ __volatile__ ("movl %%cr3,%%eax ; movl %%eax,%%cr3" : : : "memory");
}


// Public Function declarations
void initialize_paging();
u32int virtual_to_physical_address(u32int vir_addr);
s32int map_page(u32int vir_addr, u32int phys_addr, u16int attribs);
void delete_page(u32int addr);
s32int page_fault_handler(u32int err_code, u32int cr2);
void dump_free_frames();
void dump_dirty_pages();
u32int pop_frame();
void push_frame(u32int p_addr);
void check_free_frames_integrity();

#endif /* PAGING_H_ */
