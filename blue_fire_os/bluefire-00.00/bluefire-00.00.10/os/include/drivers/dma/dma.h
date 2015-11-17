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

#define DMA_MEMORY_DIMINSION ( PHYSICAL_DMA_MEMORY_END - PHYSICAL_DMA_MEMORY_START )

// The DMA-memory frame is marked as free (available for dma).
#define DMA_FREE_FRAME		0

// The DMA-memory frame is marked as busy (already in use for dma)
#define DMA_BUSY_FRAME		1

// The DMA-memory frame is marked as reserve (never available for dma
//		(like the space already used by bios)).
#define DMA_RESERVED_FRAME	2

#define DMA_PHYSICAL_TO_IDX(addr) 	(addr / PAGE_SIZE)
/******************************************************************************
 *			--------- PUBLIC FUNCTION DECLARATIONS ----------
******************************************************************************/
void initialize_DMA();
void *dma_phys_alloc( size_t len );
void dma_xfer(unsigned channel, u32int physaddr, s32int length, s32int read);

#endif /* DMA_H_ */
