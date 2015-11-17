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

// Used to store hardware definition of DMA channels
typedef struct DmaChannel {
	u08int page;     // Page register
	u08int offset;   // Offset register
	u08int length;   // Length register
} DmaChannel;

// Definition of DMA channels
static DmaChannel dmainfo[] =
{
	{ 0x87, 0x00, 0x01 },
	{ 0x83, 0x02, 0x03 },
	{ 0x81, 0x04, 0x05 },
	{ 0x82, 0x06, 0x07 }
};

/**************************************************************************
 * This sets up a DMA transfer between a device and memory.  Pass the DMA 8bit
 * channel number (0..3), the physical address of the buffer and transfer
 * length.  If 'read' is TRUE, then transfer will be from memory to device,
 * else from the device to memory.
 ***************************************************************************/
void dma_xfer(unsigned channel, u32int physaddr, s32int length, s32int read) {
	s32int page, offset;

	if (channel > 3) return;

	u32int flags;
	disable_and_save_interrupts(flags);

	// calculate dma page and offset
	page = physaddr >> 16;
	offset = physaddr & 0xFFFF;
	length -= 1;  // with dma, if you want k bytes, you ask for k-1


	// set the mask bit for the channel
	outport08(0x0A, channel | 0x04);

	// clear flipflop
	outport08(0x0C, 0x00);

	// set DMA mode (write+single+r/w)
	outport08(0x0B, (read ? 0x48 : 0x44) + channel);

	// set DMA page
	outport08(dmainfo[channel].page, page);

	// set DMA offset
	outport08(dmainfo[channel].offset, offset & 0xFF);  // low byte
	outport08(dmainfo[channel].offset, offset >> 8);    // high byte

	// set DMA length
	outport08(dmainfo[channel].length, length & 0xFF);  // low byte
	outport08(dmainfo[channel].length, length >> 8);    // high byte

	// clear DMA mask bit
	outport08(0x0A, channel);

	restore_interrupts(flags);
}

u32int dma_pop_frame() {
	u32int ret;

	u32int flags;
	disable_and_save_interrupts(flags);

	if (dma_pointer < (DMA_MEMORY_DIMINSION / PAGE_SIZE)) {
		ret = dma_free_frames[dma_pointer];
		dma_pointer++;
		restore_interrupts(flags);
		return(ret);
	}

	// Out of DMA memory
	restore_interrupts(flags);
	return (NULL);

}
/******************************************************************************
 *	--------- DMA FRAME STACK INIALIZATION  ----------
 *  <TODO> Convert to bit set.
******************************************************************************/
void initialize_DMA(){

	s32int i;
	// defined in kernel.ld
	extern u32int KERNEL_END_CODE;

	dma_pointer = 0;

	// Initialize the DMA free frames vector.
	// Should one day become a bitset.
	//0x4000 frames
	dma_free_frames = (u32int *)kmalloc(DMA_PHYSICAL_TO_IDX(PHYSICAL_DMA_MEMORY_END) * sizeof(u32int), GFP_KERNEL);

	//dma_free_frames[0x0..0xFFF] -> (0x00000000, 0x00FFFFFF)
	for( i = 0; i < (DMA_PHYSICAL_TO_IDX(PHYSICAL_DMA_MEMORY_END) - 1); i++ ) {
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
}
