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
//task_t	*current_task = NULL;

//declared in assembly/exit.asm
extern size_t __task_exit_point;
//extern void __task_exit_point;


// Declared in sched.c
extern queue_t *ready_queue;
extern task_t	*current_task;
extern task_t *idle_task;
extern queue_t *zombie_queue;

// Last used pid.
s32int last_pid = 0;

// A spinlock used to generate new pids.
DECLARE_SPINLOCK( pid_lock );
// Create a new pid. return The new pid created.

//This routine generate a pid using an atomic increment.
__inline__ s32int new_pid() {
	s32int ret;

	spin_lock( &pid_lock );
	ret = ++last_pid;
	spin_unlock( &pid_lock );

	return( ret );
}

s32int get_pid() {
	// Return the current task pid if there is a curr_task
	if (!current_task) return NULL;
	return(current_task->pid);
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
	memset08( t->pdbr, 0, VIRT_TO_PDE_IDX(VIRTUAL_KERNEL_START) * sizeof(u32int) );

	if( current_task ) {
		for( i = VIRT_TO_PDE_IDX(VIRTUAL_KERNEL_START); i < 1023; i++ )
			t->pdbr[ i ] = current_task->pdbr[ i ];
	} else {
		for( i = VIRT_TO_PDE_IDX(VIRTUAL_KERNEL_START); i < 1023; i++ )
			t->pdbr[ i ] = ((u32int *)VIRTUAL_PAGE_DIRECTORY_START)[ i ];
	}
	// Map page directory into itself.
	t->pdbr[ 1023 ] =  virtual_to_physical_address((size_t)(t->pdbr)) | P_PRESENT | P_WRITABLE;

	// Get the pdbr counter of the father
	// (we've just made the update!)
	if( current_task ) {
		t->pdbr_update_counter = current_task->pdbr_update_counter;
	}
}

//TODO
static __inline__ u32int *task_setup_stack( int argc, char **argv, size_t stack_start ) {
	u32int *stack = (u32int *)( ALIGN_DOWN(stack_start, sizeof(u32int)) - sizeof(u32int) );
	u32int *user_argv;
	int i;

	// Copy external parameters strings.
	for( i = 0; i < argc; i++ ) {
		stack = (u32int *)((size_t)stack - strlen(argv[i]) - 1);
		strcpy08( (char *)stack, argv[i] );
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
task_t *create_process(void *routine, s32int argc, s08int **argv, s08int *pname, s32int privilege) {
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
	//memset32( (void *)(new_task->pl0_stack), 0, STACK_SIZE );
	memset08( (void *)(new_task->pl0_stack), 0, STACK_SIZE );

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
	new_task->tss.cr3 = virtual_to_physical_address( (size_t)(new_task->pdbr) );

	// Temporary switch to the new address space.
	if( current_task != NULL )
		task_switch_mmu( current_task, new_task );
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

	// Restore the old address space.
	if( current_task != NULL ) { task_switch_mmu( new_task, current_task ); }

	// Setup the IO port mapping.
	new_task->tss.io_map_addr = sizeof(tss_t);
	memset32( new_task->tss.io_map, 0xffffffff, IO_MAP_SIZE );

	// Setup general registers.
	if ( privilege == KERNEL_PRIVILEGE ) {
		new_task->tss.ds = new_task->tss.es =
		new_task->tss.fs = new_task->tss.gs = KERNEL_DATA;
	} else {
		new_task->tss.ds = new_task->tss.es =
		new_task->tss.fs = new_task->tss.gs = USER_DATA | 3;
	}

	// Setup the eflags register.
	new_task->tss.eflags = EFLAGS_IOPL0 | EFLAGS_IF | 0x02;

	// Setup starting address (program counter).
	new_task->tss.cs = (privilege == KERNEL_PRIVILEGE) ? KERNEL_CODE : USER_CODE | 3;
	new_task->tss.eip = (size_t)routine;

	// Store the process name.
	// The last character must be ever '\0' (end of string).
	strncpy08( new_task->name, pname, sizeof(new_task->name) - 2 );


	// Set the current working directory.
	new_task->cwd = 0;

	// Set the process credentials.
	new_task->pid = new_pid();
	if( privilege == KERNEL_PRIVILEGE ) {
		new_task->uid = new_task->euid =
		new_task->suid = new_task->fsuid = 0;
		new_task->gid = new_task->egid =
		new_task->sgid = new_task->fsgid = 0;
	} else {
		new_task->uid = new_task->euid =
		new_task->suid = new_task->fsuid = 1;
		new_task->gid = new_task->egid =
		new_task->sgid = new_task->fsgid = 1;
	}

	// Set the parent.
	new_task->father = current_task;

	// Set the console.
	if( current_task != NULL ) {
		new_task->console = current_task->console;
	}

	// Setup the priority.
	new_task->priority = new_task->counter = HIGH_PRIORITY;

	// Insert the task into the ready queue.
	rem_queue( &zombie_queue, new_task );
	add_queue( &ready_queue, new_task );
	new_task->state = TASK_READY;

	sched_leave_critical_region();




	// This is a little trick... Because we exit
	// from a very long critical region we call
	// the scheduler to enforce a new task selection.
	if( current_task != NULL )
		schedule();

	return( new_task );
}

/**************************************************************************
* ---------- Task's routines ----------
**************************************************************************/
/**************************************************************************
* ---------- Task's routines ----------
**************************************************************************/
// Simply do nothing...
void do_idle() {
	while(TRUE) {
		enable_interrupts();
		idle();
	}
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
	current_task = create_process( NULL, 0, NULL, "init", KERNEL_PRIVILEGE );

	// Set the console.
	current_task->console = 1;

	// After the init thread is out it will become the idle task.
	idle_task = current_task;

	// Load task register.
	__asm__ __volatile__ ("ltr %0" : : "a" (current_task->tss_sel));
}
