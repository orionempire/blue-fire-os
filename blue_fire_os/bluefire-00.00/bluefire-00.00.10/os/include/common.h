/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.08
 *	Author: David Davidson
 *	Name: common.h
 *	Created: Dec 14, 2011
 *	Purpose: Structures likely to be used all around the kernel.
 *  Usage:
***************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#define KERNEL_NAME	"BLUE FIRE (IA-32)"
#define KERNEL_VERSION	"00.00.10"

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

#define PANIC(msg)  kprintf("\nKERNEL PANIC (%s) \n\tin %s at line %d",msg,__FILE__,__LINE__); asm volatile("cli");for(;;);
#define ASSERT(itm) if(!itm){ PANIC("ASSERTION FAILED") }
//Debug shortcuts
#define dbg(itm) kprintf("\n at line %d in %s "#itm" is -> 0x%x\n",__LINE__,__FILE__,itm);
// Mark and halt this line of code execution here.
#define dbg_brk() kprintf("\nHalted in ->%s at line %d",__FILE__,__LINE__); while(1);
// Chew the cpu for about imt seconds.
#define dbg_pause(itm) u32int dbg_pause_var = itm * 10000000; while(dbg_pause_var) { --dbg_pause_var; };
//
#define dbg_cnt_set(itm) static u32int tmp = itm;
#define dbg_cnt_go(itm) kprintf("\n%s(%d)( X %d) "#itm" is -> 0x%x",__FILE__,__LINE__,tmp,itm);tmp--; if(tmp <= 0){ dbg_brk();};

#endif /* COMMON_H_ */
