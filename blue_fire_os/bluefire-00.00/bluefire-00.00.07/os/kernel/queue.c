/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: queue.c
 *	Created: Feb 14, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Return the next element in the queue
void *pick_queue(queue_t **q) {
	if (q==NULL) {
		return(NULL);
	}

	if (*q==NULL) {
		return(NULL);
	}

	*q=(*q)->next;

	return((*q)->value);
}

// Add an element to the queue
void add_queue(queue_t **q, void *v) {
	queue_t *new;

	new = (queue_t *)kmalloc(sizeof(queue_t));

	if (new == NULL) {
		error("Out of virtual memory!!!");
	}

	new->value = v;

	if (*q==NULL) {
		new->next = new;
		*q=new;
	} else {
		new->next = (*q)->next;
		(*q)->next = new;
	}
}

// Remove the element v from the queue if it is present.
s32int rem_queue(queue_t **q, void *v){
	queue_t *t, *t2;

	if (q==NULL) {
		return(FALSE);
	}

	if (*q==NULL){
		return(FALSE);
	}

	// Search for the element 'v'
	t = *q;
	do {
		t2 = t;
		t = t->next;

		if (t->value == v) {
			// Element found!
			t2->next = t->next;

			if (t == *q) {
				// Deleting current element
				if (t == t->next) {
					*q = NULL;
				} else {
					*q = (*q)->next;
				}
			};
			kfree(t);
			return(TRUE);
		}
	} while (t != *q);

	return(FALSE);
}
