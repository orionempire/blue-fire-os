/******************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: common.h
 *	Created: Dec 14, 2011
 *	Purpose:
 *  Usage:
******************************************************************************/

#ifndef COMMON_H_
#define COMMON_H_

#define KERNEL_NAME	"BLUE FIRE (IA-32)"
#define KERNEL_VERSION	"00.00.06"

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

// TODO FIX ?
#define CHAR_MIN	(-128)
#define CHAR_MAX	127
#define UCHAR_MAX	255

#define SHRT_MAX	((short)(~0U>>1))
#define SHRT_MIN	(-SHRT_MAX - 1)
#define USHRT_MAX	((short)(~0U>>1))

#define INT_MAX         ((int)(~0U>>1))
#define INT_MIN         (-INT_MAX - 1)
#define UINT_MAX        (~0U)

#define LONG_MAX        ((long)(~0UL>>1))
#define LONG_MIN        (-LONG_MAX - 1)
#define ULONG_MAX       (~0UL)

//! This macro returns the minimum value between \a a and \a b.
#define MIN(a, b)	((a) < (b) ? (a) : (b))
//! This macro returns the maximum value between \a a and \a b.
#define MAX(a, b)	((a) > (b) ? (a) : (b))
//! This macro returns the absolute value of \a a.
#define ABS(a)		((a) < 0 ? -(a) : (a))
//! This macro truncates \a addr to the \a align boundary.
#define TRUNC(addr, align)	((addr) & ~((align) - 1))
//! This macro rounds down \a addr to the \a align boundary.
#define ALIGN_DOWN(addr, align)	TRUNC(addr, align)
//! This macro rounds up \a addr to the \a align boundary.
#define ALIGN_UP(addr, align)	( ((addr) + (align) - 1) & (~((align) - 1)) )

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
