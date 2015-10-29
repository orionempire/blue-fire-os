/******************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: kmalloc.h
 *	Created: Oct 22, 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
******************************************************************************/
#ifndef KMALLOC_H_
#define KMALLOC_H_

#define KMALLOC_MAGIC		0xDEADBEEF
#define KMALLOC_FREE		0
#define KMALLOC_ALLOC		1

// Allocate normal kernel ram.
#define GFP_KERNEL			1
// Use inside interrupt
#define GFP_ATOMIC			2

// Structures
typedef struct MEMORY_BLOCK {
	u32int	magic;
	u32int	flags;
	u32int	size;
	u32int	owner;
} memory_block_t;

// Public Function declarations
void *kmalloc(u32int size, u32int mflag);
void kfree(void *ptr);
void kmalloc_initialize();
void dump_memory_map(void);
void *kmemalign(size_t alignment, size_t size, s32int flags);

#endif /* KMALLOC_H_ */
