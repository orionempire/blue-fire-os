/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: dma.c
 *	Created: Oct 22, 2015
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// ---------- DMA memory allocator ----------
// DMA free frames stack
u32int *dma_free_frames;
s32int dma_pointer;

void initialize_DMA(){

	s32int i;
	u32int addr;

	dma_pointer = 0;

	dma_free_frames = (u32int *)kmalloc(((DMA_MEMORY_DIMINSION / PAGE_SIZE) * sizeof(u32int)), GFP_KERNEL);

	dbg_brk()
	for (i=0, addr=(PHYSICAL_DMA_MEMORY_START / PAGE_SIZE); i < (DMA_MEMORY_DIMINSION / PAGE_SIZE); i++, addr++){
			dma_free_frames[i]=addr;
	}
}
