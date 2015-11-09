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

#define DMA_MEMORY_DIMINSION (PHYSICAL_DMA_MEMORY_END - PHYSICAL_DMA_MEMORY_START)

// The DMA-memory frame is marked as free.
#define DMA_FREE_FRAME	0

// The DMA-memory frame is marked as busy.
#define DMA_BUSY_FRAME	1



// Normalize the physical address to the dma base address.
#define DMA_FRAME_ADDR( i ) \
	(((i) < PHYSICAL_DMA_MEMORY_START) ? 0 : ((size_t)(i) - PHYSICAL_DMA_MEMORY_START))

// Public Function declarations
void initialize_DMA();

#endif /* DMA_H_ */
