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

// The atomic variable structure.
typedef struct atomic {
	volatile s32int counter;
} atomic_t;

// Set the atomic value of v to i (guaranteed only for 24 bits)
#define atomic_set(v, i)	(((v)->counter) = (i))


#endif	//ATOMIC_H_
