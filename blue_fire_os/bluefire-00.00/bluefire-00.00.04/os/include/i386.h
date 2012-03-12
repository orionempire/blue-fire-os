/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: i386.h
 *	Created: Jan 11, 2012
 *	Purpose: Describes the only supported architecture, 386 class 32 bit.
 *  Usage:
***************************************************************************/
#ifndef I386_H_
#define I386_H_

// Turn off interrupts: save flags and disable IRQs.
#define disable_interrupts(flags) \
	__asm__ __volatile__ ("pushfl ; popl %0 ; cli" : "=g"(flags) : : "memory")

// Turn on interrupts: restore flags.
#define enable_interrupts(flags)	restore_flags(flags)

#define restore_flags( flags ) \
	__asm__ __volatile__ ("pushl %0 ; popfl" : : "g"(flags) : "memory", "cc")

// Halt the CPU until an interrupt occurs.
#define idle() \
	__asm__ __volatile__ ("hlt" : : : "memory");

/**************************************************************************
* Hardware Input / Output functions
**************************************************************************/
// Input from I/O port
static __inline__ u08int inport8(u16int port) {
		u08int val;
        __asm__ __volatile__ ("inb %%dx, %%al" : "=a" (val) : "d" (port));
        return val;
}

// Output to I/O port
static __inline__ void outport8(u16int port, u08int val) {
        __asm__ __volatile__ ("outb %%al, %%dx" : : "d" (port), "a" (val));
}


#endif /* I386_H_ */
