/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: common.h
 *	Created: Dec 14, 2011
 *	Last Update: Dec 14, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#define KERNEL_NAME	"BLUE FIRE (IA-32)"
#define KERNEL_VERSION	"00.00.00"

#define TRUE	1
#define FALSE	0

#define NULL	0

//type definitions that lend to future portability
typedef unsigned char  		u08int;			//also byte
typedef          char  		s08int;
typedef unsigned short 		u16int;			//also word
typedef          short 		s16int;
typedef unsigned int   		u32int;			//also dword
typedef          int   		s32int;			//also bool
typedef unsigned long long  u64int;
typedef          long long  s64int;

typedef u32int 				size_t;

#endif /* COMMON_H_ */
