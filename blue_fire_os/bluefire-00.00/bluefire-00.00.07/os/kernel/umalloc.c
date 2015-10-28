/******************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: umalloc.c
 *	Created: Oct 22, 2015
 *	Purpose:
 *  Usage:
******************************************************************************/
#include <common_include.h>

void umalloc_init(task_t *t, size_t heap_start, size_t heap_size)
{
	umem_block_t *p;

	if( t == NULL )
		return;

	if( t->flags.type == THREAD_TYPE ){
		kprintf("%s(): threads share the father's heap.\n", __FUNCTION__);
		return;
	}

	// Setup the heap in the task structure.
	t->heap_start = heap_start;
	t->heap_size = heap_size;
	initialize_MUTEX( &(t->heap_sem) );

	// Create only one hudge block.
	p = (umem_block_t *)heap_start;
	p->magic = UMALLOC_MAGIC;
	p->flags = UMALLOC_FREE;
	p->size = heap_size - sizeof(umem_block_t);
	p->owner = NULL;
}
