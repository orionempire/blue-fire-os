/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: queue.h
 *	Created: Feb 14, 2012
 *	Last Update: Feb 14, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef QUEUE_H_
#define QUEUE_H_

// Queue structure //
typedef struct QUEUE {
	void *value;
	struct QUEUE *next;
} queue_t;

// Public Function declarations
void *pick_queue(queue_t **q);
void add_queue(queue_t **q, void *v);
int rem_queue(queue_t **q, void *v);

#endif /* QUEUE_H_ */
