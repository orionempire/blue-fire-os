/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: umalloc.h
 *	Created: Oct 22, 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef UMALLOC_H_
#define UMALLOC_H_

#define UMALLOC_MAGIC		0xDEADBEEF
#define UMALLOC_FREE		0
#define UMALLOC_ALLOC		1

//! A memory block structure.
typedef struct UMEM_BLOCK {
	u32int	magic;
	u32int	flags;
	u32int	size;
	u32int	owner;
} umem_block_t;
#endif /* UMALLOC_H_ */
