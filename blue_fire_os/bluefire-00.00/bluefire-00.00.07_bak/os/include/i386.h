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

// Record whether interrupts where off and then disable them regardless.
#define disable_and_save_interrupts( flags ) \
	__asm__ __volatile__ ("pushfl ; popl %0 ; cli" : "=g"(flags) : : "memory")

// Record whether interrupts were turned off
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

/**************************************************************************
* ---------- Atomic operators ----------
* Uses the lock opcode
* Older CPUs would lock the memory bus now they lock the CPU cache.
* Structure precludes casting to int.
* volatile key word ensures compiler does not move around.
* **************************************************************************/
typedef struct {
	volatile s32int counter;
} atomic_t;

// Initialize an atomic variable.
#define atomic_init(i)		{ (i) }

// Set the atomic value of v to i
#define atomic_set(v, i)	(((v)->counter) = (i))
// Read the atomic value of v
#define atomic_read(v)		((v)->counter)

// Perform an atomic increment
static __inline__ void atomic_inc(atomic_t *v) {
	__asm__ __volatile__ (	"lock; incl %0" : "=m"(v->counter) : "m"(v->counter));
}

// Perform an atomic decrement
static __inline__ void atomic_dec(atomic_t *v) {
	__asm__ __volatile__ (	"lock; decl %0" : "=m"(v->counter) : "m"(v->counter));
}

#endif /* I386_H_ */
