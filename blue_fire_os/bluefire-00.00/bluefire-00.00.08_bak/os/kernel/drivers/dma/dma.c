/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: dma.c
 *	Created: Feb 14, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include "../../../../../bluefire-00.00.08_bak/os/include/common_include.h"

// ---------- DMA memory allocator ----------
// DMA free frames stack
u32int *dma_free_frames;
s32int dma_pointer;

void initialize_DMA(){

	s32int i;
	u32int addr;

	dma_pointer = 0;
	dma_free_frames = (u32int *)kmalloc((DMA_MEMORY_DIMINSION / PAGE_SIZE) * sizeof(u32int));
	for (i=0, addr=(PHYSICAL_DMA_MEMORY_START / PAGE_SIZE); i < (DMA_MEMORY_DIMINSION / PAGE_SIZE); i++, addr++){
			dma_free_frames[i]=addr;
	}
}
