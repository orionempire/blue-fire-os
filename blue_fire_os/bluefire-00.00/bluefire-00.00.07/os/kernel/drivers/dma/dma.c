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
	// defined in kernel.ld
	extern u32int KERNEL_END_CODE;

	dma_pointer = 0;

	// Initialize the DMA free frames vector.
	dma_free_frames = (u32int *)kmalloc(((DMA_MEMORY_DIMINSION / PAGE_SIZE) * sizeof(u32int)), GFP_KERNEL); //0x3ff4

	//dma_free_frames[0x0..0xFFD]
	for( i = 0; i < DMA_MEMORY_DIMINSION / PAGE_SIZE; i++ ) { //0x0 - 0xFFD
		dma_free_frames[ i ] = DMA_FREE_FRAME;
	}

	// Set the video buffer memory busy.
	//dma_free_frames[0x09D..0x0BC] -> (0x000A0000,0x000C0000)
	for( i = DMA_FRAME_ADDR(PHYSICAL_VIDEO_BUFFFER_START) / PAGE_SIZE; i < DMA_FRAME_ADDR(PHYSICAL_VIDEO_BUFFFER_END) / PAGE_SIZE; i++ ) {//0x9D - 0xBC
		dma_free_frames[ i ] = DMA_BUSY_FRAME;
	}

	//dma_free_frames[0x0BD..0x0FC] -> (0x000C0000,0x00100000)
	// Set the BIOS ROM memory busy.
	for( i = DMA_FRAME_ADDR(PHYSICAL_BIOS_ROM_START) / PAGE_SIZE; i < (DMA_FRAME_ADDR(PHYSICAL_BIOS_ROM_END) / PAGE_SIZE); i++ ) { //0xBD - 0xFC
		dma_free_frames[ i ] = DMA_BUSY_FRAME;
	}


	//Amount of frames that the kernel takes up. KERNEL_END_CODE is set in the linker script (kernel.ld)
	u32int kernel_code_frame_count = (((u32int)&KERNEL_END_CODE - VIRTUAL_KERNEL_START) / PAGE_SIZE);

	//Mark the kernel physical space taken.
	//dma_free_frames[0x0FC..0x129] -> (0x00100000,0x00128000) //TODO - will change as code grows
	for( i = 0; i <= kernel_code_frame_count; i++) {//0xFD - 0x129
		dma_free_frames[ (DMA_FRAME_ADDR(PHYSICAL_KERNEL_START) / PAGE_SIZE) + i ] = DMA_BUSY_FRAME;
	}
}
