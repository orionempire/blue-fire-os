/******************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: sched.c
 *	Created: Oct 28, 2015
 *	Purpose:
 *  Usage:
******************************************************************************/
#include <common_include.h>


// Ready-queue: tasks are ready for the CPU.
DECLARE_QUEUE( ready_queue );

// Wait-queue: tasks are waiting for I/O.
DECLARE_QUEUE( wait_queue );

// Zombie-queue: tasks are dying.
DECLARE_QUEUE( zombie_queue );

 // Current running task.
task_t	*curr_task = NULL;

// IDLE task structure.
task_t *idle_task = NULL;

int sched_is_disabled( void ) {
	return( spin_is_locked(&sched_enabled) );
}

//! Update all task queue.
static __inline__ void do_refresh_queues()
{
	queue_t *t, *t2;
	task_t *p;
	int n;

	// Look into the ready queue for the tasks that need to
	// be waked-up and to be killed.
	queue_for_each_safe( t, t2, n, ready_queue ) {
		p = queue_get_entry( t );
		if( p->state == TASK_WAIT ) {
			queue_move_entry( &wait_queue, &ready_queue, t );
		} else if( p->state == TASK_ZOMBIE ) {
			queue_move_entry( &zombie_queue, &ready_queue, t );
			//wakeup_task( kpagd );
			PANIC("NO kpadb yet");
		}
	}

	// Look into the wait queue for the tasks that need to
	// be waked-up and to be killed.
	queue_for_each_safe( t, t2, n, wait_queue ) {
		p = queue_get_entry( t );
		if( p->state == TASK_READY ) {
			queue_move_entry( &ready_queue, &wait_queue, t );
			p->counter = (HIGH_PRIORITY << 1) + p->priority;
		} else if( p->state == TASK_ZOMBIE ) {
			queue_move_entry( &zombie_queue, &wait_queue, t );
			//wakeup_task( kpagd );
			PANIC("NO kpadb yet");
		}
	}

	// Refresh done.
	atomic_set( &refresh_queues_flag, 0 );
}

//! \brief The dispatcher.
/*!
 *	This routine supplies to change the context of the tasks.
 */
static __inline__ void switch_to( task_t *prev, task_t *next )
{
	// Perform the context switch only if it is necessary.
	if( prev != next ) {
		// Update the page directory for kernel address
		// space only if the update counter is different
		// between the prev and next task. Because prev
		// was the running task it is the most recently
		// updated task. So we don't have to check if the
		// counter is 'less than' the next task. Better to
		// check only if they are different...
		if( next->pdbr_update_counter != prev->pdbr_update_counter ) {
			// Update the page directory for kernel address space
			// only if it is necessary.
			// This must be done to preserve coherency between
			// page directories of each task.
			if( next->tss.cr3 != prev->tss.cr3 ) {
				// Ok... we have to update now!
				memcpy08( PDE_OFFSET(next->pdbr, VIRTUAL_KERNEL_START),
					PDE_OFFSET(prev->pdbr, VIRTUAL_KERNEL_START),
					( PDE_INDEX(PAGE_DIR_MAP) - PDE_INDEX(VIRTUAL_KERNEL_START) ) * sizeof(uint32_t) );
			}
			// Update the pdbr counter.
			next->pdbr_update_counter = prev->pdbr_update_counter;
		}
		// Perform the hardware task switch.
		jmp_to_tss( next->tss_sel );
	}
}


void schedule( void ) {
	s32int min_counter = INT_MAX;
	task_t *prev, *next;
	queue_t *entry;
	s32int n;
	uint32_t flags;

	if( sched_is_disabled() ){
		// We can't switch to another process!
		// Come back to the current task...
		return;
	}

	disable_and_save_interrupts( flags );

	// Update all the task queue if necessary.
	if( atomic_read(&refresh_queues_flag) ){
		do_refresh_queues();
	}

	// Save the previous task.
	prev = curr_task;

	// Set the idle task as the default candidate to run.
	curr_task = idle_task;

	// The scheduler's body.
	prev->counter = prev->priority;
	queue_for_each( entry, n, ready_queue ) {
		next = queue_get_entry( entry );
		if( next->counter < min_counter ) {
			curr_task = next;
			min_counter = curr_task->counter;
		}
		if( next->counter > (HIGH_PRIORITY << 1) ) {
			(next->counter)--;
		}
	}

	// Switch from the previous to the next process.
	switch_to( prev, curr_task );

	restore_interrupts( flags );
}
