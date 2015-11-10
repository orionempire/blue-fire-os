/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: kernel_map.h
 *	Created: Jan 23, 2012
 *	Last Update: Jan 23, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef KERNEL_MAP_H_
#define KERNEL_MAP_H_

/**************************************************************************
 * 	Virtual address memory definitions
**************************************************************************/
// DMA memory (DON'T TOUCH)
#define PHYSICAL_DMA_MEMORY_START			0x3000
#define PHYSICAL_DMA_MEMORY_END				0x01000000

// NOTE: Must be PAGE_SIZE*1024 aligned!!!
#define VIRTUAL_KERNEL_START				0xC0000000

// Kernel heap
#define VIRTUAL_KERNEL_HEAP_START			0xD0000000
#define VIRTUAL_KERNEL_HEAP_END				0xE0000000

// the first 16MB of physical memory are mapped here
#define VIRTUAL_ACTUAL_MEMORY_START			0xE0000000
#define VIRTUAL_ACTUAL_MEMORY_END			0xE1000000

// Temporary memory
#define VIRTUAL_KERNEL_TEMP_MEMORY_START	0xF0000000
#define VIRTUAL_KERNEL_TEMP_MEMORY_END		0xFFC00000

// The page tables are self mapped here. The starting address of Virtual Memory
// represented by the last (1024) entry in the PDE
#define VIRTUAL_PAGE_TABLE_MAP				0xFFC00000
//											(0xFFC00000	+ (0xFFC00000 		      / 1024 = 0x3FF000)) = 0xFFFFF000 (maps to 0x1000)
#define VIRTUAL_PAGE_DIRECTORY_MAP			(0xFFC00000 + (VIRTUAL_PAGE_TABLE_MAP / 0x400))

#endif /* KERNEL_MAP_H_ */