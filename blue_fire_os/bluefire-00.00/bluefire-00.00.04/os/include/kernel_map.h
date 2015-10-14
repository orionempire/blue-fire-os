/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: kernel_map.h
 *	Created: Jan 23, 2012
 *	Last Update: Jan 23, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef KERNEL_MAP_H_
#define KERNEL_MAP_H_

/**************************************************************************
 * 	Virtual address memory definitions
**************************************************************************/
// Lower Memory is the first 16 MB
#define LOWER_MEMORY_SIZE	0x1000000

// NOTE: Must be PAGE_SIZE*1024 aligned!!!
#define VIRTUAL_KERNEL_START		0xC0000000

// the first 16MB of physical memory are mapped here
#define VIRTUAL_LOWER_MEMORY_START	0xE0000000
#define VIRTUAL_LOWER_MEMORY_END	VIRTUAL_LOWER_MEMORY_START + LOWER_MEMORY_SIZE

// the page tables are self mapped here.
#define VIRTUAL_PAGE_TABLE_MAP		0xFFC00000
#define VIRTUAL_PAGE_DIRECTORY_MAP	0xFFFFF000

#endif /* KERNEL_MAP_H_ */
