/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: mem.h
 *	Created: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef MEM_H_
#define MEM_H_

// Public Function declarations
void memset8(void *dest_ptr, u08int val, u32int count);
void memset16(void *dest_ptr, u16int val, u32int count);
void memset32(void *dest_ptr, u32int val, u32int count);
void memcpy8(void *dest_ptr, const void *org_ptr, u32int count);
void memcpy32(void *dest_ptr, const void *org_ptr, u32int count);

#endif /* MEM_H_ */
