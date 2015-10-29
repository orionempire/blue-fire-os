/******************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: queue.h
 *	Created: Oct 28, 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
******************************************************************************/
#ifndef QUEUE_H_
#define QUEUE_H_

/******************************************************************************
 *				--------- STANDARD QUEUES ----------
******************************************************************************/
// Doubly linked queue structure.
typedef struct queue{
	void 	*value;			// value of the element.
	struct 	queue 	*next;	// pointer to the next element.
	struct 	queue 	*prev;	// pointer to the previous element.
} queue_t;

//! Declare a doubley linked circular queue.
#define DECLARE_QUEUE( name ) queue_t *name = NULL

//! Delete the current entry into the queue.
static __inline__ void __queue_del_entry( queue_t **q, queue_t *entry ) {
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;

	if( entry==(*q) ) {
		(*q) = (*q)->next;
		if( entry == (*q) ) *q = NULL;
	}

	kfree( entry );
}

//! Delete the current entry into the queue safe against empty queues.
static __inline__ int queue_del_entry( queue_t **q, queue_t *entry ) {
	if( *q == NULL )
		return( -1 );

	__queue_del_entry( q, entry );

	return( 0 );
}

//! Delete the head of the queue.
static __inline__ s32int queue_del_head( queue_t **q ) {
	return( queue_del_entry(q, *q) );
}

//! Move an element from a queue to another queue.
static __inline__ s32int queue_move_entry( queue_t **to, queue_t **from, queue_t *entry ) {
	if( *from == NULL )
		return( -1 );

	// Unlink the entry.
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;

	// Update the head of the source queue.
	if( entry == (*from) ) {
		(*from) = (*from)->next;
		if( entry == (*from) )
			*from = NULL;
	}
	// Put the entry into the destination queue.
	if ( *to == NULL ) {
		entry->next = entry->prev = entry;
		*to = entry;
	} else {
		entry->prev = (*to)->prev;
		entry->next = (*to);
		(*to)->prev->next = entry;
		(*to)->prev = entry;
	}

	return( 0 );
}

// Get the value of the entry.
static __inline__ void *queue_get_entry( queue_t *entry ) {
	return( (entry != NULL) ? entry->value : NULL );
}

// brief Get the number of elements in the queue.
static __inline__ s32int count_queue(queue_t **q) {
	queue_t *head = *q, *tmp = *q;
	register s32int count = 0;

	if( head != NULL ) {
		while( (tmp != head) || !(count) ){
			tmp = tmp->next;
			prefetch_cpu_cache( tmp->next );
			count++;
		}
	}

	return( count );
}

#define queue_for_each_safe( pos, n, c, head ) \
	if( (head) != NULL ) \
		for( pos = (head), n = pos->next, \
			c = count_queue( &(head) ); \
			c--; \
			pos = n, n = pos->next )

#define queue_for_each( pos, c, head ) \
	if( (head) != NULL ) \
		for( pos = (head), c = 0, prefetch_cpu_cache(pos->next); \
			(pos != (head)) || !(c); \
			pos = pos->next, prefetch_cpu_cache(pos->next), ++c )

static __inline__ s32int add_queue( queue_t **q, void *v ) {
	queue_t *p;

	p = (queue_t *)kmalloc( sizeof(queue_t), GFP_ATOMIC );
	if (p == NULL)
		return( -ENOMEM );

	p->value = v;

	if ( *q==NULL ) {
		p->next = p->prev = p;
		*q = p;
	} else {
		p->prev = (*q)->prev;
		p->next = (*q);
		(*q)->prev->next = p;
		(*q)->prev = p;
	}
	return( 0 );
}
static __inline__ s32int rem_queue( queue_t **q, void *v ){
	queue_t *t, *_tmp;
	int n;

	// Search (safe) the element 'v'.
	queue_for_each_safe( t, _tmp, n, *q ) {
		if( t->value==v ) {
			// Element found.
			__queue_del_entry( q, t );
			return( 0 );
		}
	}
	return( -1 );
}

/******************************************************************************
 *			--------- PUBLIC FUNCTION DECLARATIONS ----------
******************************************************************************/


#endif /* QUEUE_H_ */
