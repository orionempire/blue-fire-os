/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.10
 *	Author: David Davidson
 *	Name: dma.c
 *	Created: Feb 14, 2012
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

void initialize_DMA(){

	s32int i;
	u32int addr;

	dma_pointer = 0;
	dma_free_frames = (u32int *)kmalloc((DMA_MEMORY_DIMINSION / PAGE_SIZE) * sizeof(u32int));
	for (i=0, addr=(PHYSICAL_DMA_MEMORY_START / PAGE_SIZE); i < (DMA_MEMORY_DIMINSION / PAGE_SIZE); i++, addr++){
			dma_free_frames[i]=addr;
	}
}
