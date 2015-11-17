/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: dma.h
 *	Created: Feb 14, 2012
 *	Last Update: Feb 14, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef DMA_H_
#define DMA_H_

#define DMA_MEMORY_DIMINSION (PHYSICAL_DMA_MEMORY_END-PHYSICAL_DMA_MEMORY_START)

// Public Function declarations
void initialize_DMA();
u32int dma_pop_frame();
void dma_xfer(unsigned channel, u32int physaddr, s32int length, s32int read);

#endif /* DMA_H_ */
