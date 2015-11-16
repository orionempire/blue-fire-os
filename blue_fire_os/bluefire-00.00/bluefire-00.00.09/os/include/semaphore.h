/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: semaphore.h
 *	Created: Oct 22, 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

// Spinlock semaphore structure.
typedef struct semaphore {
	atomic_t count;
	s32int sleepers;
	queue_t *waitq;
} semaphore_t;

// Initialize a spinlock semaphore.
static __inline__ void initialize_MUTEX( semaphore_t *sem ){
	atomic_set( &(sem->count), 1 );
	sem->sleepers = 0;
	sem->waitq = NULL;
}

#endif // SEMAPHORE_H_
