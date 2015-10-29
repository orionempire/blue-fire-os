/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: atomic.h
 *	Created: Oct 28, 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef ATOMIC_H_
#define ATOMIC_H_

/**************************************************************************
* ---------- Atomic operators ----------
* Uses the lock opcode
* Older CPUs would lock the memory bus now they lock the CPU cache.
* Structure precludes casting to int.
* volatile key word ensures compiler does not move around.
* **************************************************************************/
// The atomic variable structure.
typedef struct atomic {
	volatile s32int counter;
} atomic_t;

// Initialize an atomic variable.
#define atomic_init(i)		{ (i) }

// Set the atomic value of v to i (guaranteed only for 24 bits)
#define atomic_set(v, i)	(((v)->counter) = (i))

// Read the atomic value of v (guaranteed only for 24 bits)
#define atomic_read(v)		((v)->counter)

// Perform an atomic increment
static __inline__ void atomic_inc(atomic_t *v) {
	__asm__ __volatile__ (	"lock; incl %0" : "=m"(v->counter) : "m"(v->counter));
}

// Perform an atomic decrement
static __inline__ void atomic_dec(atomic_t *v) {
	__asm__ __volatile__ (	"lock; decl %0" : "=m"(v->counter) : "m"(v->counter));
}

/******************************************************************************
 *			--------- PUBLIC FUNCTION DECLARATIONS ----------
******************************************************************************/

#endif	/* ATOMIC_H_ */
