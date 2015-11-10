/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: kmalloc.h
 *	Created: Feb 6, 2012
 *	Last Update: Feb 6, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef KMALLOC_H_
#define KMALLOC_H_

#define KMALLOC_MAGIC		0xDEADBEEF
#define KMALLOC_FREE		0
#define KMALLOC_ALLOC		1

// Structures
typedef struct MEMORY_BLOCK {
	u32int	magic;
	u32int	flags;
	u32int	size;
	u32int	owner;
} memory_block_t;

// Public Function declarations
void *kmalloc(u32int size);
void kfree(void *ptr);
void kmalloc_initialize();
void dump_memory_map(void);

#endif /* KMALLOC_H_ */
