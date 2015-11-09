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
	// Should one day become a bitset.
	//0x4000 frames
	dma_free_frames = (u32int *)kmalloc(DMA_PHYSICAL_TO_IDX(PHYSICAL_DMA_MEMORY_END) * sizeof(u32int), GFP_KERNEL);

	//dma_free_frames[0x0..0x1000] -> (0x00000000, 0x01000000)
	for( i = 0; i < DMA_PHYSICAL_TO_IDX(PHYSICAL_DMA_MEMORY_END); i++ ) {
		dma_free_frames[ i ] = DMA_FREE_FRAME;
	}

	// Automatically reserve the first 4k (0x0 - 0x2FFF) as it is used by the IVT and original page table
	//dma_free_frames[0x0..0x2] -> (0x00000000,0x0002FFF)
	for( i = 0; i < (DMA_PHYSICAL_TO_IDX(PHYSICAL_DMA_MEMORY_START) - 1 ); i++ ){
		dma_free_frames[ i ] = DMA_RESERVED_FRAME;
	}

	// Set the video buffer memory busy.
	//dma_free_frames[0x0A0..0x0BF] -> (0x000A0000,0x000BFFF)
	for( i = DMA_PHYSICAL_TO_IDX(PHYSICAL_VIDEO_BUFFFER_START); i < (DMA_PHYSICAL_TO_IDX(PHYSICAL_VIDEO_BUFFFER_END) - 1); i++ ) {
		dma_free_frames[ i ] = DMA_RESERVED_FRAME;
	}

	//dma_free_frames[0x0C0..0x0FF] -> (0x000C0000,0x000FFFFF)
	// Set the BIOS ROM memory busy.
	for( i = DMA_PHYSICAL_TO_IDX(PHYSICAL_BIOS_ROM_START); i < (DMA_PHYSICAL_TO_IDX(PHYSICAL_BIOS_ROM_END) - 1 ); i++ ) {
		dma_free_frames[ i ] = DMA_RESERVED_FRAME;
	}

	//Amount of frames that the kernel takes up. KERNEL_END_CODE is set in the linker script (kernel.ld)
	u32int kernel_code_frame_count = (((u32int)&KERNEL_END_CODE - VIRTUAL_KERNEL_START) / PAGE_SIZE);

	//Mark the kernel physical space taken. <will change as code grows>
	//dma_free_frames[0x100..<0x12b>] -> (0x00100000,<c002b000>)
	for( i = 0; i <= (kernel_code_frame_count - 1); i++) {
		dma_free_frames[ (DMA_PHYSICAL_TO_IDX(PHYSICAL_KERNEL_START) + i) ] = DMA_RESERVED_FRAME;
	}
	//dbg((DMA_PHYSICAL_TO_IDX(PHYSICAL_KERNEL_START) + i)); dbg_brk()
}
