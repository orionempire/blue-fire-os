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
#define disable_and_save_interrupts( flags ) \
	__asm__ __volatile__ ("pushfl ; popl %0 ; cli" : "=g"(flags) : : "memory")

// Save but don't modify interrupt state.
#define save_interrupts( flags ) \
	__asm__ __volatile__ ("pushfl ; popl %0" : "=g"(flags) : : "memory")

// Turn on interrupts if they were on before.
#define restore_interrupts( flags ) \
	__asm__ __volatile__ ("pushl %0 ; popfl" : : "g"(flags) : "memory", "cc")

// Enable all interrupts.
#define enable_interrupts()	__asm__ __volatile__ ("sti" : : : "memory");

// Disable all interrupts.
#define disable_interrupts()	__asm__ __volatile__ ("cli" : : : "memory");

// Halt the CPU until an interrupt occurs.
#define idle() \
	__asm__ __volatile__ ("hlt" : : : "memory");

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


#endif /* I386_H_ */
