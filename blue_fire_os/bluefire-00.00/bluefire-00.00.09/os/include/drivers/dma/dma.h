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

#endif /* DMA_H_ */
