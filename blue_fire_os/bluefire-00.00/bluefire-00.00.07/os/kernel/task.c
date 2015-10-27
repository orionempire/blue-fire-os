/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: task.c
 *	Created: Oct 22, 2015
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Current running task
task_t	*curr_task = NULL;

//declared in assembly/exit.asm
extern void __task_exit_point;

int get_pid() {
	// Return the current task pid if there is a curr_task
	if (!curr_task) return NULL;
	return(curr_task->pid);
}

//TODO
static void new_vspace( task_t *t ) {
	int i;

	// Create the virtual space of the task.
	t->pdbr = (u32int *)get_temp_page();
	if( t->pdbr == NULL ) {
		kprintf("%s:Out of virtual memory!!!", __FUNCTION__);
		return;
	}

	// Initialize PDBR.
	memset( t->pdbr, 0, VIRT_TO_PDE_IDX(VIRTUAL_KERNEL_START) * sizeof(u32int) );

	if( curr_task ) {
		for( i = VIRT_TO_PDE_IDX(VIRTUAL_KERNEL_START); i < 1023; i++ )
			t->pdbr[ i ] = curr_task->pdbr[ i ];
	} else {
		for( i = VIRT_TO_PDE_IDX(VIRTUAL_KERNEL_START); i < 1023; i++ )
			t->pdbr[ i ] = ((u32int *)VIRTUAL_PAGE_DIRECTORY_START)[ i ];
	}
	// Map page directory into itself.
	t->pdbr[ 1023 ] =  vir_to_phys((size_t)(t->pdbr)) | P_PRESENT | P_WRITABLE;

	// Get the pdbr counter of the father
	// (we've just made the update!)
	if( curr_task ) {
		t->pdbr_update_counter = curr_task->pdbr_update_counter;
	}
}

//TODO
static inline u32int *task_setup_stack( int argc, char **argv, size_t stack_start ) {
	u32int *stack = (u32int *)( ALIGN_DOWN(stack_start, sizeof(u32int)) - sizeof(u32int) );
	u32int *user_argv;
	int i;

	// Copy external parameters strings.
	for( i = 0; i < argc; i++ ) {
		stack = (u32int *)((size_t)stack - strlen(argv[i]) - 1);
		strcpy( (char *)stack, argv[i] );
		argv[i] = (char *)stack;
	}

	// Round down the stack pointer to the stack boundaries.
	stack = (u32int *)( ALIGN_DOWN((size_t)stack, sizeof(u32int)) );

	// Copy parameter string pointers.
	*(--stack) = NULL;
	for( i = 0; i < argc; i++ ) {
		*(--stack) = (u32int)(argv[argc-i-1]);
	}

	// Set the process argv pointer.
	user_argv = stack;
	*(--stack) = (u32int)user_argv;
	// Copy number of parameters value.
	*(--stack) = argc;

	// Copy the exit point address.
	*(--stack) = (size_t)(&__task_exit_point);

	// Insert a NULL into the stack.
	*(stack - 1) = NULL;

	// Return the initial stack pointer.
	return( stack );
}

//TODO
task_t *create_process(void *routine, int argc, char **argv, char *pname, int privilege) {
	task_t *new_task;

	sched_enter_critical_region();

	new_task = kmemalign( PAGE_SIZE, sizeof(task_t), GFP_KERNEL );

	if( new_task == NULL ) {
		// Out of virtual memory!!!
		kprintf("%s:Out of virtual memory!!! Cannot create task [%s].", __FUNCTION__, pname);
		sched_leave_critical_region();
		return( NULL );
	}

	// Add the task into the zombie queue, so if the current task
	// is killed the space allocated until now can be freed by the
	// kpager daemon.
	add_queue( &zombie_queue, new_task );
	new_task->state = TASK_NEW;

	// Create the pl0-stack.
	new_task->pl0_stack = (size_t)kmalloc( STACK_SIZE, GFP_KERNEL );
	if( new_task->pl0_stack == NULL ) {
		// Out of virtual memory!!!
		kprintf("%s:Out of virtual memory!!! Cannot create p10 stack [%s].", __FUNCTION__, pname);
		kfree( new_task );

		sched_leave_critical_region();
		return( NULL );
	}
	// Null the stack to enforce the page mapping
	memset( (void *)(new_task->pl0_stack), 0, STACK_SIZE );
	// Setup the privileged stack.
	new_task->tss.ss0 = KERNEL_STACK;
	new_task->tss.esp0 = ALIGN_DOWN( (size_t)(new_task->pl0_stack) + STACK_SIZE - sizeof(u32int), sizeof(u32int) );

	// Setup the TSS.
	new_task->tss_sel = setup_GDT_entry(sizeof(tss_IO_t), (size_t)&(new_task->tss), TSS_SEG, 0 );
	if( new_task->tss_sel == NULL ) {
		kprintf("%s:Out of virtual memory!!! Cannot create GDT entry [%s].", __FUNCTION__, pname);
		kfree( new_task );
		kfree( (void *)(new_task->pl0_stack) );

		sched_leave_critical_region();
		return( NULL );
	}

	// Set the privilege level.
	new_task->privilege = privilege;

	// Set the task type.
	new_task->flags.type = PROCESS_TYPE;

	// Create the virtual space of the task.
	new_vspace( new_task );
	if ( new_task->pdbr == NULL ) {
		// Out of virtual memory!!!
		kprintf("%s:Out of virtual memory!!! Cannot create page table [%s].", __FUNCTION__, pname);

		kfree( new_task );
		kfree( (void *)(new_task->pl0_stack) );
		remove_GDT_entry( new_task->tss_sel );

		sched_leave_critical_region();
		return( NULL );
	}

	// Setup the task page directory address.
	new_task->tss.cr3 = vir_to_phys( (size_t)(new_task->pdbr) );

	// Temporary switch to the new address space.
	if( curr_task != NULL )
		task_switch_mmu( curr_task, new_task );
	else
		switch_mmu( new_task->tss.cr3 );


	// Create the task stack.
	new_task->tss.ss = (privilege == KERNEL_PRIVILEGE) ? KERNEL_STACK : USER_STACK | 3;
	if ( privilege == KERNEL_PRIVILEGE ) {
		new_task->tss.esp = (size_t) task_setup_stack( argc, argv, new_task->tss.esp0 + sizeof(u32int) );
	} else {
		new_task->tss.esp = (size_t) task_setup_stack( argc, argv, VIRTUAL_TASK_HEAP_START );
	}

	// Initialize the user heap.
	umalloc_init( new_task, VIRTUAL_TASK_HEAP_START, VIRTUAL_TASK_HEAP_SIZE );

}

/**************************************************************************
* ---------- Scheduling routines ----------
**************************************************************************/
atomic_t sched_enabled;

// Disable the scheduling for the other tasks
void sched_enter_critical_region() {
	atomic_set(&sched_enabled, FALSE);
}

// Re-enable the scheduling for the other tasks
void sched_leave_critical_region() {
	atomic_set(&sched_enabled, TRUE);
}

void initialize_multitasking() {
	// Create the "init" task.
	curr_task = create_process( NULL, 0, NULL, "init", KERNEL_PRIVILEGE );
}
