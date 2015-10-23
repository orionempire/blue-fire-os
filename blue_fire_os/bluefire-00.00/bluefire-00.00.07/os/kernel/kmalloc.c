/******************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: kmalloc.c
 *	Created: Oct 22, 2015
 *	Purpose:
 *  Usage:
******************************************************************************/
#include <common_include.h>

// ---------- Memory operators for kernel ----------
void *kmalloc(u32int size) {
	// This is a best-fit like allocator.
	memory_block_t *p, *p_best=NULL;

	if (!size) return NULL; // Short Circuit zero size requests
	u32int flags;
	disable_and_save_interrupts(flags);

	// Round the size up to sizeof(mem_block_t); the
	// sizeof(mem_block_t) is the unit of allocation.
	if (size%sizeof(memory_block_t))
		size += (sizeof(memory_block_t) - size%sizeof(memory_block_t));

	// Explore the heap for the best-fit hole
	p=(memory_block_t *)VIRTUAL_KERNEL_HEAP_START;
	while(p < (memory_block_t *)VIRTUAL_KERNEL_HEAP_END)
	{
		// Find a free block //
		if (p->flags == KMALLOC_FREE)
		{
			// This block is free... let's proceed //
			if (p->size == size)
			{
				// Best fit block found! You're lucky! //
				p_best = p;
				break;
			}
			if (p->size > size)
			{
				// The hole is bigger than size... 	//
				// ...check if it's the best-fit hole!	//
				if (p_best==NULL)
					p_best = p;
				else
					if (p_best->size > p->size)
						p_best = p;
			}
		}
		// Examine the next memory block //
		p = (void *)p+p->size+sizeof(memory_block_t);
	}

	// Have you found a free block?! //
	if (p_best == NULL)
	{
		restore_interrupts(flags);
		return NULL;
	}

	// If you do, allocate it! //
	p_best->magic = KMALLOC_MAGIC;
	p_best->flags = KMALLOC_ALLOC;
	p_best->owner = get_pid();
	p_best->owner = NULL;

	if (p_best->size == size)
	{
		p_best = (void *)p_best + sizeof(memory_block_t);
		restore_interrupts(flags);
		// The block cover the size perfectly //
		return ((void *)p_best);
	}

	// Otherwise split the hole in two blocks //
	p = (void *)p_best + sizeof(memory_block_t) + size;
	p->magic = KMALLOC_MAGIC;
	p->flags = KMALLOC_FREE;
	p->size = p_best->size - size - sizeof(memory_block_t);
	p->owner = NULL;

	p_best->size = size;
	p_best = (void *)p_best + sizeof(memory_block_t);

	restore_interrupts(flags);
	// Return the best-fit pointer
	return ((void *)p_best);
}


void kmalloc_initialize() {

	// Initialize the memory allocator
	memory_block_t *p;

	// Create the master block spanning the whole available size.
	p = (memory_block_t *)VIRTUAL_KERNEL_HEAP_START;
	p->magic = KMALLOC_MAGIC;
	p->flags = KMALLOC_FREE;
	p->size = VIRTUAL_KERNEL_HEAP_END - VIRTUAL_KERNEL_HEAP_START - sizeof(memory_block_t);		//(0x0FFFFFF0)
	p->owner = 0;

}

void dump_memory_map(void) {

	memory_block_t *p = (memory_block_t *)VIRTUAL_KERNEL_HEAP_START;

	for( ; ; ) {
		kprintf("\np        = %#010x "
				"\np->magic = %#010X"
				"\np->flags = %#010x"
				"\np->size  = %u "
				"\np->owner = %i\n",
				(u32int)p + sizeof(memory_block_t),
				p->magic,
				p->flags,
				p->size,
				p->owner);

		p = (void *)p + p->size + sizeof(memory_block_t);

		if (p >= (memory_block_t *)VIRTUAL_KERNEL_HEAP_END) break;

		// No keyboard yet so can't pause
		dbg_pause(1);
	}
}
