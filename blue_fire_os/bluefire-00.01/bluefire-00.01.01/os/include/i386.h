/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: i386.h
 *	Created: Jan 11, 2012
 *	Purpose: Describes the only supported architecture, 386 class 32 bit.
 *  Usage:
***************************************************************************/
#ifndef I386_H_
#define I386_H_

// Turn off interrupts: save flags and disable IRQs.
#define disable_and_save_interrupts( flags ) \
	__asm__ __volatile__ ("pushfl ; popl %0 ; cli" : "=g"(flags) : : "memory")

// Save but don't modify interrupt state.
#define save_interrupts( flags ) \
	__asm__ __volatile__ ("pushfl ; popl %0" : "=g"(flags) : : "memory")

// Enable on interrupts only if they were on before.
#define restore_interrupts( flags ) \
	__asm__ __volatile__ ("pushl %0 ; popfl" : : "g"(flags) : "memory", "cc")

// Disable interrupts regardless of previous state.
#define disable_interrupts() __asm__ __volatile__ ("cli" : : : "memory");

// Disable interrupts regardless of previous state.
#define enable_interrupts()	__asm__ __volatile__ ("sti" : : : "memory");

// Halt the CPU until an interrupt occurs.
#define idle()	__asm__ __volatile__ ("hlt" : : : "memory");

/**************************************************************************
* Hardware Input / Output functions
**************************************************************************/
// Input from I/O port
static __inline__ u08int inport08(u16int port) {
		u08int val;
        __asm__ __volatile__ ("inb %%dx, %%al" : "=a" (val) : "d" (port));
        return val;
}

// Output to I/O port
static __inline__ void outport08(u16int port, u08int val) {
        __asm__ __volatile__ ("outb %%al, %%dx" : : "d" (port), "a" (val));
}

/******************************************************************************
 *	Prefetch data into cache. Because it is didatic this os is hard coded to
 *	Bochs which defaults to the Pentium IV hence the prefetchnta instruction.
 *	other wise replace with a null operation ...
 *	static __inline__ void prefetch_cpu_cache( const void *p ) { ; }
******************************************************************************/
static __inline__ void prefetch_cpu_cache( const void *p ) {
	__asm__ __volatile__ ("prefetchnta (%0)" : : "r"(p));
}

//TODO
static __inline__ void jmp_to_tss(u16int tss_sel) {
	static struct {
	unsigned eip : 32; // 32 bit
	unsigned cs  : 16; // 16 bit
	} __attribute__ ((packed)) tss_link = {0, 0};

	// Set the TSS link						//
	tss_link.cs = tss_sel;

	// Jump to the task						//
	__asm__ __volatile__ ("ljmp *(%0)" : : "m" (tss_link));
}

#endif /* I386_H_ */
