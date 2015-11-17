/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: spinlock.h
 *	Created: Oct 26, 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef SPINLOCK_H
#define SPINLOCK_H

// Simple spinlock semaphore structure.
typedef struct spinlock {
	volatile unsigned long lock;
} spinlock_t;

// The spinlock unlocked value.
#define SPINLOCK_UNLOCKED	{ 1 }
// The spinlock locked value.
#define SPINLOCK_LOCKED		{ 0

// Declare a simple spinlock unlocked.
#define DECLARE_SPINLOCK( name ) 	spinlock_t name = SPINLOCK_UNLOCKED

//! Check if a spinlock is locked.
#define spin_is_locked( s ) \
	( *(volatile signed char *)(&((s)->lock))<=0 )

//! Initialize a spinlock.
#define spin_lock_init( x ) \
	do { *(x) = (spinlock_t)SPINLOCK_UNLOCKED; } while( 0 )

// Lock the semaphore.
static __inline__ __attribute__((__always_inline__)) void spin_lock( spinlock_t *lock ) {
	__asm__ __volatile__(
		"1: lock ; decb %0\n"
		"jge 3f\n"
		"2:\n"
		"cmpb $0, %0\n"
		"rep; nop\n"
		"jle 2b\n"
		"jmp 1b\n"
		"3:\n"
		: "=m"(lock->lock) : : "memory");
}

// Unlock the semaphore.
static __inline__ __attribute__((__always_inline__)) void spin_unlock( spinlock_t *lock ) {
	__asm__ __volatile__(
		"movb $1, %0"
		: "=m"(lock->lock) : : "memory");
}

//! Disable irq and lock the semaphore.
#define spin_lock_irqsave(lock, flags) \
	do { disable_and_save_interrupts(flags); spin_lock(lock); } while( 0 )

//! Unlock the semaphore and restore eflags.
#define spin_unlock_irqrestore(lock, flags) \
	do { spin_unlock(lock); restore_interrupts(flags); } while( 0 )

#endif
