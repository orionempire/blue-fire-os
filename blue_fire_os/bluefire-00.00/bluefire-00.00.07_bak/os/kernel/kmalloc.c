/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: kmalloc.c
 *	Created: Feb 6, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include "../../../bluefire-00.00.07_bak/os/include/common_include.h"


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

void kfree(void *ptr) {
//TODO get rid of the nested function!
	void deallocate(memory_block_t *ptr) {
		// Try to release physical frames occupied by the freed block
		u32int start = PAGE_ALIGN_UP((u32int)((void *)ptr + sizeof(memory_block_t)));
		u32int end = PAGE_ALIGN((u32int)((void *)ptr + sizeof(memory_block_t) + ptr->size));

		while(start < end)
		{
			if (*ADDR_TO_PDE(start) != NULL)
			{
				delete_page(start);
				start += PAGE_SIZE;
			}
			else
				start = PAGE_DIR_ALIGN_UP(start);
		}
	}
	memory_block_t *p, *p2;

	if (ptr == NULL) return;

	u32int flags;
	disable_and_save_interrupts(flags);

	// Point to the header
	p = (void *)ptr - sizeof(memory_block_t);

	if (p->magic != KMALLOC_MAGIC) return; // Is the process crazy?
	if (p->flags != KMALLOC_ALLOC) return; // Crazy again?

	// Free the block
	p->flags = KMALLOC_FREE;
	p->owner = 0;

	// Try to combine the block with the next one...
	p2 = (void *)p + p->size + sizeof(memory_block_t);

	if (p2 < (memory_block_t *)VIRTUAL_KERNEL_HEAP_END) {
		if (p2->flags == KMALLOC_FREE) {
			// Let's merge the two blocks!
			p->size += p2->size + sizeof(memory_block_t);
			p2->magic = NULL;
		}
	}

	// Try to combine the block with the previous one...
	if (p != (memory_block_t *)VIRTUAL_KERNEL_HEAP_START) {
		// Find the previous block
		p2 = (void *)VIRTUAL_KERNEL_HEAP_START;
		for(;;) {
			if (((void *)p2 + p2->size + sizeof(memory_block_t)) == p) {
				// Block found! Check if it's a free block
				if (p2->flags == KMALLOC_FREE) {
					// Let's merge the two blocks!
					p2->size += (p->size + sizeof(memory_block_t));
					p->magic = NULL;
					p = p2;
				}
				break;
			}
			p2 = (void *)p2 + p2->size + sizeof(memory_block_t);
		}
	}
	deallocate(p);

	restore_interrupts(flags);
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

		// Wait for a key... //
		//if( kgetchar() == CTRL_C ) break;
		dbg_pause(1);
	}
}
