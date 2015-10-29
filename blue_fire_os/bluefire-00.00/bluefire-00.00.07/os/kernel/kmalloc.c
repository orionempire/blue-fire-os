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

extern task_t	*current_task;
// ---------- Memory operators for kernel ----------
void *kmalloc(u32int size, u32int mflag) {
	// This is a best-fit like allocator.
	memory_block_t *p, *p_best=NULL;
	u32int flags;

	if (!size) return NULL; // Short Circuit zero size requests

	// Round the size up to sizeof(mem_block_t); the
	// sizeof(mem_block_t) is the unit of allocation.
	size = ( size & -(sizeof(memory_block_t)) ) + sizeof(memory_block_t);
	p = (memory_block_t *)VIRTUAL_KERNEL_HEAP_START;

	disable_and_save_interrupts(flags);

	while(p < (memory_block_t *)VIRTUAL_KERNEL_HEAP_END) {
		// Find a free block //
		if (p->flags == KMALLOC_FREE) {
			// This block is free... let's proceed //
			if (p->size == size) {
				// Best fit block found! You're lucky! //
				p_best = p;
				break;
			}
		}
		// Examine the next memory block //
		p = (void *)p+p->size+sizeof(memory_block_t);
	}

	// Have you found a free block?! //
	if (p_best == NULL) {
		restore_interrupts(flags);
		return NULL;
	}

	// If the block does not cover perfectly the size,
	// split the hole in two blocks
	if( p_best->size != size ){
		p = (void *)p_best + sizeof(memory_block_t) + size;
		p->magic = KMALLOC_MAGIC;
		p->flags = KMALLOC_FREE;
		p->size = p_best->size - size - sizeof(memory_block_t);
		p->owner = NULL;

		p_best->size = size;
	}

	// Allocate the block.
	p_best->magic = KMALLOC_MAGIC;
	p_best->flags = KMALLOC_ALLOC;
	if( mflag == GFP_KERNEL ) {
		p_best->owner = current_task->pid;
	} else {
		// kmalloc() during interrupt! the owner
		// is ambiguous.
		p_best->owner = NULL;
	}

	// Return the first-fit pointer.
	p_best = (void *)p_best + sizeof(memory_block_t);

	restore_interrupts(flags);

	return( (void *)p_best );

	/*
	// If you do, allocate it! //
	p_best->magic = KMALLOC_MAGIC;
	p_best->flags = KMALLOC_ALLOC;
	p_best->owner = get_pid();
	p_best->owner = NULL;

	if (p_best->size == size) {
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
	*/
}

static void deallocate(memory_block_t *ptr){
	size_t start = PAGE_ALIGN_UP( (size_t)((void *)ptr + sizeof(memory_block_t)) );
	size_t end = PAGE_ALIGN( (size_t)((void *)ptr + sizeof(memory_block_t) + ptr->size) );

	while( start < end ) {
		if( *VIRT_TO_PDE_ADDR(start) != NULL ) {
			delete_page( start );
			start += PAGE_SIZE;
		} else {
			start = PAGE_DIR_ALIGN_UP( start );
		}
	}
}

void kfree(void *ptr) {
	memory_block_t *p, *p2;
	u32int flags;

	if( ptr == NULL ) return;

	// Point to the header.
	p = (void *)ptr - sizeof(memory_block_t);

	if( p->magic != KMALLOC_MAGIC ) return;
	if( p->flags != KMALLOC_ALLOC ) return;

	disable_and_save_interrupts( flags );

	// Free the block.
	p->flags = KMALLOC_FREE;
	p->owner = NULL;

	// Try to combine the block wih the next one.
	p2 = (void *)p + p->size + sizeof(memory_block_t);

	if( p2 < (memory_block_t *)VIRTUAL_KERNEL_HEAP_END ) {
		if( p2->flags == KMALLOC_FREE ) {
			// Let's merge the two blocks!
			p->size += p2->size + sizeof(memory_block_t);
			p2->magic = NULL;
		}
	}

	// Try to combine the block with the previous one.
	if( p != (memory_block_t *)VIRTUAL_KERNEL_HEAP_START ) {
		// Find the previous block.
		p2 = (void *)VIRTUAL_KERNEL_HEAP_START;
		for(;;) {
			if( ((void *)p2 + p2->size + sizeof(memory_block_t)) == p ) {
				// Block found!
				// Check if it's a free block.
				if( p2->flags == KMALLOC_FREE ) {
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

	// Release the physical space occupied by the block.
	deallocate( p );

	restore_interrupts( flags );
}

s32int mem_sizeof(void *ptr) {
	memory_block_t *p = ((memory_block_t *)ptr-1);
	u32int flags;
	s32int __ret;

	disable_and_save_interrupts( flags );

	if ( (p->magic == KMALLOC_MAGIC) && (p->flags == KMALLOC_ALLOC) ) {
		__ret =  p->size;
	} else {
		__ret = NULL;
	}

	restore_interrupts( flags );

	return( __ret );
}
void *kmemalign(size_t alignment, size_t size, s32int flags) {
	memory_block_t *p, *p2;

	// Cannot allocate memory with a size of zero.
	if( !size ) {
		return( NULL );
	}

	// Allocate the pointer.
	p = kmalloc(size + alignment + 2 * sizeof(memory_block_t), flags);
	if( p == NULL ) {
		return( NULL );
	}

	// An alignment of zero is intended as a simple kmalloc(size).
	if ( alignment == 0 ) {
		return( p );
	}

	// Check if the pointer is correctly aligned.
	if( (size_t)p % alignment ) {
		// Align the pointer p to the boundary.
		u32int flags;

		disable_and_save_interrupts( flags );

		// Allocate the new block.
		p2 = p + 2;
		p2 = (memory_block_t *)ALIGN_UP((size_t)p2, alignment)  - 1;
		p2->magic = KMALLOC_MAGIC;
		p2->flags = (p-1)->flags;
		p2->owner = (p-1)->owner;
		p2->size = mem_sizeof(p) - ((size_t)(p2+1) - (size_t)p);

		// Free the unused space.
		(p-1)->size = (size_t)p2 - (size_t)p;

		restore_interrupts( flags );

		kfree( p );

		return( p2 + 1 );
	}

	// The pointer p is already aligned to the boundary.
	return( p );
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
