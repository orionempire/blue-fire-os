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
// Lower Memory is the first 16 MB
#define LOWER_MEMORY_SIZE				0x1000000

// DMA memory (DON'T TOUCH)
#define PHYSICAL_DMA_MEMORY_START		0x3000
#define PHYSICAL_DMA_MEMORY_END			0x01000000

//TODO
// The address where the user-privilege task stack is placed.
// Stack grows up from beggingin of kernel (0xC0000000 --> 0x0)
#define VIRTUAL_TASK_STACK_START		0xC0000000

// The start of the heap for a user-mode task. (1 GB)
#define VIRTUAL_TASK_HEAP_START			0x40000000

// The size of the heap for a user-mode task.
#define VIRTUAL_TASK_HEAP_SIZE			0x40000000

// NOTE: Must be PAGE_SIZE*1024 aligned!!!
#define VIRTUAL_KERNEL_START			0xC0000000

// Kernel heap
#define VIRTUAL_KERNEL_HEAP_START		0xD0000000
#define VIRTUAL_KERNEL_HEAP_END			0xE0000000

// the first 16MB of physical memory are mapped here
#define VIRTUAL_LOWER_MEMORY_START		0xE0000000
#define VIRTUAL_LOWER_MEMORY_END		0xE1000000	`// (VIRTUAL_LOWER_MEMORY_START + LOWER_MEMORY_SIZE)

// Temporary memory area start address.
#define VIRTUAL_KERNEL_TMP_MEMORY_START	0xE1000000	// (VIRTUAL_LOWER_MEMORY_START + LOWER_MEMORY_SIZE)
#define VIRTUAL_KERNEL_TMP_MEMORY_END	0xFFC00000	// VIRTUAL_PAGE_TABLE_START

// the page tables are self mapped here.
#define VIRTUAL_PAGE_TABLE_START		0xFFC00000
#define VIRTUAL_PAGE_DIRECTORY_START	0xFFFFF000

#endif /* KERNEL_MAP_H_ */
