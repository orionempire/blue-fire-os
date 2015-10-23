/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: task.c
 *	Created: Feb 8, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include "../../../bluefire-00.00.07_bak/os/include/common_include.h"

// Current running task
task_t	*curr_task = NULL;

// IDLE task structure
task_t *idle_task = NULL;

// Ready-queue: tasks are ready to the CPU
queue_t *ready_queue = NULL;
// Wait-queue: tasks are waiting for I/O
queue_t *wait_queue = NULL;
// Zombie-queue: tasks are dying!
queue_t *zombie_queue = NULL;

// Last used pid
atomic_t last_pid;

/**************************************************************************
* ---------- Process info ----------
**************************************************************************/
// Return the current task name if there is a curr_task
s08int *get_pname() {
	if (!curr_task) {
		return NULL;
	}

	return(curr_task->name);
}
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
* ---------- PID operators ----------
**************************************************************************/
int new_pid() {
	// Create a new pid
	atomic_inc(&last_pid);
	return(atomic_read(&last_pid));
}

int get_pid() {
	// Return the current task pid if there is a curr_task
	if (!curr_task) return NULL;
	return(curr_task->pid);
}

// Create a new process
task_t *create_process(void *address, void *buffer, s08int *pname) {
	//defined in os/kernel/paging.c
	extern u32int K_PDBR[PAGE_SIZE/sizeof(u32int)];
	task_t *new_task;
	u08int *pl0_stack;
	u32int *PDBR;
	u32int cr3, PDBR_frame, i;

	sched_enter_critical_region();

	// --- Create the task structure ---
	new_task = kmalloc(sizeof(task_t));
	if (new_task == NULL) {
		// Out of virtual memory!!! //
		kset_color(LIGHT_RED);
		kprintf("\n\rOut of virtual memory!!! Cannot create task [%s].", pname);
		kset_color(DEFAULT_COLOR);

		sched_leave_critical_region();
		return(NULL);
	}
	memset08(new_task, 0, sizeof(task_t));

	// --- Create the pl0-stack --- //
	pl0_stack = kmalloc(STACK_SIZE);
	if (pl0_stack == NULL) {
		// Out of virtual memory!!!
		kset_color(LIGHT_RED);
		kprintf("\n\rOut of virtual memory!!! Cannot create task [%s].", pname);
		kset_color(DEFAULT_COLOR);
		// Free the previous allocated space
		kfree((void *)new_task);

		sched_leave_critical_region();
		return(NULL);
	}
	// Null the stack to enforce the page mapping
	memset08(pl0_stack, 0, STACK_SIZE);

	// Setup the pl0-stack
	new_task->tss.ss0 = KERNEL_STACK;
	new_task->tss.esp0 = new_task->pl0_stack = (u32int)(pl0_stack + STACK_SIZE);

	// --- Setup the TSS ---
	new_task->tss_sel = setup_GDT_entry(sizeof(tss_IO_t), (u32int)&(new_task->tss), TSS_SEG, 0);

	// --- Create the virtual space of the task ---
	PDBR_frame = (pop_frame()*PAGE_SIZE);
	if (PDBR_frame == NULL)	{
		// Out of memory!!! Free the previous allocated memory. //
		kset_color(LIGHT_RED);
		kprintf("\n\rOut of physical memory!!! Cannot create task [%s].", pname);
		kset_color(DEFAULT_COLOR);
		// Free the previous allocated space //
		kfree((void *)(pl0_stack));
		kfree((void *)new_task);

		sched_leave_critical_region();
		return(NULL);
	}

	// Get the frame... now we can intialize the task page directory
	if (!(map_page(VIRTUAL_KERNEL_TEMP_MEMORY_START, PDBR_frame, P_PRESENT | P_WRITE)))	{
		// Out of memory!!! Free the previous allocated memory.
		kset_color(LIGHT_RED);
		kprintf("\n\rOut of physical memory!!! Cannot create task [%s].", pname);
		kset_color(DEFAULT_COLOR);
		// Free the previous allocated space //
		kfree((void *)(pl0_stack));
		kfree((void *)new_task);
		push_frame(PDBR_frame / PAGE_SIZE);

		sched_leave_critical_region();
		return(NULL);
	}

	// Initialize PDBR
	PDBR = (u32int *)VIRTUAL_KERNEL_TEMP_MEMORY_START;
	memset08(PDBR, 0, PAGE_SIZE);
	for (i = VIRTUAL_KERNEL_START/(PAGE_SIZE*1024); i < 1024; i++) {
		PDBR[i] = K_PDBR[i];
	}

	// Map page directory into itself
	PDBR[1023] = PDBR_frame | P_PRESENT | P_WRITE;

	// Temporary switch to the new address space
	__asm__ __volatile__ ("movl %%cr3, %0" : "=r"(cr3) : );
	__asm__ __volatile__ ("movl %0, %%cr3" : : "r"(PDBR_frame));

	// --- Create the user space ---

	// Create the task stack
	#define TASK_STACK_START 0x80000000
	memset08((void *)TASK_STACK_START, 0, STACK_SIZE);
	new_task->tss.ss = KERNEL_STACK;
	new_task->tss.esp = (u32int)(TASK_STACK_START + STACK_SIZE);

	// Map the user code and data space
	// ("address" is the virtual task space, "buffer" is the kernel space where the task-code is located)
	#define TASK_SPACE_DIM	512
	if (address != buffer) {
		memcpy08(address, buffer, TASK_SPACE_DIM);
	}

	// Restore the old address space //
	__asm__ __volatile__ ("movl %0, %%cr3" : : "r"(cr3));

	// Setup the task PDBR
	new_task->tss.cr3 = PDBR_frame;

	// Setup the IO port mapping
	new_task->tss.io_map_addr = sizeof(tss_t);

	// Setup general registers
	new_task->tss.ds = new_task->tss.es = KERNEL_DATA;
	new_task->tss.fs = new_task->tss.gs = KERNEL_DATA;
	new_task->tss.eflags = EFLAGS_IF | 0x02;

	// Initialize general purpose registers
	new_task->tss.eax = new_task->tss.ebx = 0;
	new_task->tss.ecx = new_task->tss.edx = 0;
	new_task->tss.esi = new_task->tss.edi = 0;

	// Initialize LDTR (Local Descriptor Table Register)
	// No LDTs for now...
	new_task->tss.ldtr = 0;

	// Initialize debug trap //
	// If set to 1 the processor generate a debug exception when a task switch to this task occurs...
	new_task->tss.trace = 0;

	// Setup starting address
	new_task->tss.cs = KERNEL_CODE;
	new_task->tss.eip = (u32int)address;

	// --- Get a pid ---
	new_task->pid = new_pid();

	// --- Store the name ---
	new_task->name = pname;

	// --- Set the type ---
	new_task->type = PROCESS_T;

	// --- Insert the task into the ready queue ---
	new_task->state = READY;
	add_queue(&ready_queue, new_task);

	sched_leave_critical_region();

	return (new_task);
}

// Create a new kernel thread
task_t *create_kthread(void *address, s08int *pname) {

	task_t *new_task;
	u08int *pl0_stack;

	sched_enter_critical_region();

	// --- Create the task structure ---
	new_task = kmalloc(sizeof(task_t));
	if (new_task == NULL) {
		// Out of virtual memory!!!
		kset_color(LIGHT_RED);
		kprintf("\n\rOut of virtual memory!!! Cannot create task [%s].", pname);
		kset_color(DEFAULT_COLOR);

		sched_leave_critical_region();
		return(NULL);
	}
	memset08(new_task, 0, sizeof(task_t));
	// --- Create the pl0-stack ---
	pl0_stack = kmalloc(STACK_SIZE);
	if (pl0_stack == NULL) {
		// Out of virtual memory!!!
		kset_color(LIGHT_RED);
		kprintf("\n\rOut of virtual memory!!! Cannot create task [%s].", pname);
		kset_color(DEFAULT_COLOR);
		// Free the previous allocated space
		kfree((void *)new_task);

		sched_leave_critical_region();
		return(NULL);
	}
	// Null the stack to enforce the page mapping
	memset08(pl0_stack, 0, STACK_SIZE);

	// Setup the pl0-stack
	new_task->tss.ss0 = new_task->tss.ss = KERNEL_STACK;
	new_task->tss.esp0 = new_task->tss.esp = new_task->pl0_stack = (u32int)(pl0_stack+STACK_SIZE);

	// --- Setup the TSS ---
	new_task->tss_sel = setup_GDT_entry(sizeof(tss_IO_t), (u32int)&(new_task->tss), TSS_SEG, 0);

	// Setup the task PDBR => get the kernel PDBR
	new_task->tss.cr3 = GET_PDBR();


	// Setup the IO port mapping
	new_task->tss.io_map_addr = sizeof(tss_t);

	// Setup general registers
	new_task->tss.ds = new_task->tss.es = KERNEL_DATA;
	new_task->tss.fs = new_task->tss.gs = KERNEL_DATA;
	new_task->tss.eflags = EFLAGS_IF | 0x02;

	// Initialize general purpose registers
	new_task->tss.eax = new_task->tss.ebx = new_task->tss.ecx =
	new_task->tss.edx =	new_task->tss.esi = new_task->tss.edi = 0;

	// Initialize debug trap
	// If set to 1 the processor generates a debug exception when a task switch to this task occurs...
	new_task->tss.trace = 0;

	// Setup starting address
	new_task->tss.cs = KERNEL_CODE;
	new_task->tss.eip = (u32int)address;

	// --- Get a pid ---
	new_task->pid = new_pid();

	// --- Store the name --- //
	new_task->name = pname;

	// --- Set the type --- //
	new_task->type = KTHREAD_T;

	// --- Insert the task into the ready queue --- //
	new_task->state = READY;

	add_queue(&ready_queue, new_task);

	sched_leave_critical_region();

	return (new_task);
}
/**************************************************************************
* ---------- Task's routines ----------
**************************************************************************/
// Kill the current running task
// NOTE: if a kthread kills itself it becomes an idle task
void auto_kill() {
	// Kill the task only if it is not a kthread
	if (curr_task->type!=KTHREAD_T)
	{
		sched_enter_critical_region();

		// Move the task from the ready queue to the zombie queue //
		rem_queue(&ready_queue, curr_task);
		add_queue(&zombie_queue, curr_task);
		curr_task->state = ZOMBIE;

		sched_leave_critical_region();
	}

	// Waiting for the dispatcher... //
	do_idle();
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

// Simple round robin scheduler
//static __inline__ void scheduler() {
void scheduler() {
	//defined in os/kernel/paging.c
	extern u32int K_PDBR[1024];

	u32int flags;
	disable_and_save_interrupts(flags);

	task_t	*prev_task;

	u32int addr;

	// Can we switch to another process?!
	if ( atomic_read(&sched_enabled) ) {
		// Remember about previous task
		prev_task = curr_task;

		// Select a new task
		curr_task = pick_queue(&ready_queue);

		if (curr_task == NULL) {
			curr_task = idle_task;
		}

		if( prev_task != curr_task ){

			// Update shared virtual address space of the new selected task
			if( !(map_page(VIRTUAL_KERNEL_TEMP_MEMORY_START, curr_task->tss.cr3, P_PRESENT | P_WRITE)) ) {
				// If there's no free pages return to the previous task
				curr_task = prev_task;
				restore_interrupts(flags);
				return;
			}

			for ( addr = VIRTUAL_KERNEL_START; addr < VIRTUAL_PAGE_TABLE_MAP; addr += (PAGE_SIZE * 1024)) {
				((u32int *)VIRTUAL_KERNEL_TEMP_MEMORY_START)[(addr / (PAGE_SIZE * 1024))] = K_PDBR[ (addr / (PAGE_SIZE * 1024))];

			}

			// Invalidate all the TLB entries
			reload_CR3();
			// Perform the task switch
			jmp_to_tss(curr_task->tss_sel);

		}
	}
	restore_interrupts(flags);
}


// ---------- Kernel tasks & threads ----------
// This special process provides to free the kernel space used by zombie tasks
void kpager() {
	task_t *t;
	u32int addr, PDBR;

	while(TRUE)	{
		t = pick_queue(&zombie_queue);
		if (t == NULL) {
			// No task in the zombie queue => IDLE!
			enable_interrupts();
			idle();
		} else {
			sched_enter_critical_region();

			PDBR = t->tss.cr3;

			// Temporary switch to the task address space
			__asm__ __volatile__ ("movl %0, %%cr3" : : "r"(t->tss.cr3));

			// Free the user address space
			for(addr=0; addr<VIRTUAL_KERNEL_START; addr+=PAGE_SIZE) {
				if (*ADDR_TO_PDE(addr) != NULL) {
					delete_page(addr);
				} else {
					addr = PAGE_DIR_ALIGN_UP(addr);
				}
			}

			// Free the pl0-stack
			kfree((void *)((u32int)(t->pl0_stack) - STACK_SIZE));

			// Destroy the TSS
			remove_GDT_entry(t->tss_sel);

			// The task is dead
			rem_queue(&zombie_queue, t);

			// Free the task structure
			kfree((void *)t);

			// Restore the kpager address space
			__asm__ __volatile__ ("movl %0, %%cr3" : : "r"(curr_task->tss.cr3));

			// Free the task page directory
			push_frame( PDBR / PAGE_SIZE);

			sched_leave_critical_region();
		}
	}
}
void initialize_multitasking() {
	// Initialize the multitasking management
	// Initialize pid counter
	atomic_set(&last_pid, -1);

	// Initialize scheduler critical region flag
	atomic_set(&sched_enabled, TRUE);

	// Create kernel task Process Control Block
	curr_task = (task_t *)kmalloc(sizeof(task_t));

	memset08(curr_task, 0, sizeof(task_t));

	// Setup the kernel TSS
	curr_task->tss_sel = setup_GDT_entry(sizeof(tss_IO_t), (u32int)&(curr_task->tss), TSS_SEG, 0);

	// Setup the kernel address space
	curr_task->tss.cr3 = GET_PDBR();

	// Setup the kernel registers
	curr_task->tss.ss0 = KERNEL_STACK;
	curr_task->tss.eflags = 0x02;
	curr_task->tss.ldtr = 0;
	curr_task->tss.trace = 0;
	curr_task->tss.io_map_addr = sizeof(tss_t);

	// Get a pid
	curr_task->pid = new_pid();

	// Store the name
	curr_task->name = "init";
	// Insert the current task into the ready queue
	add_queue(&ready_queue, curr_task);

	curr_task->state = READY;

	// Load task register
	__asm__ __volatile__ ("ltr %0" : : "r" ((u16int)curr_task->tss_sel));
	// Initialize the IDLE task
	// The IDLE task is a special task,
	// it's always ready but never present in the ready queue.
	idle_task = create_kthread(&do_idle, "idle");
	rem_queue(&ready_queue, idle_task);

	create_kthread(&kpager, "kpager");
}

// Print the state of every process from the ready, wait, and zombie queues.
void ps() {
	task_t *p, *ps;

	if ((ps = p = curr_task)==NULL) return;
	// Header.
	kset_color( WHITE );
	kprintf("PID\tSTATE\tCOMMAND\n");
	kset_color( DEFAULT_COLOR );

	sched_enter_critical_region();

	// Ready processes
	while(TRUE)	{
		// Print process informations
		kprintf("%d\tReady\t%s\n",p->pid,p->name);
		// Get the next task in the ready queue
		p = pick_queue(&ready_queue);
		if (p == curr_task) break;
	}

	if ((ps = p = pick_queue(&wait_queue)) != NULL) {
		// Wait processes
		while(TRUE)	{
			// Print process informations
			kprintf("%d\tWait\t%s\n",p->pid,p->name);
			// Get the next task in the ready queue
			p = pick_queue(&wait_queue);
			if (p == ps) break;
		}
	}

	// Zombie processes
	if ((ps = p = pick_queue(&zombie_queue)) != NULL) {
		while(TRUE)	{
			// Print process informations
			kprintf("%d\tZombie\t%s\n",p->pid,p->name);
			// Get the next task in the ready queue
			// Get the next task in the ready queue //
			p = pick_queue(&zombie_queue);
			if (p==ps) break;
		}
	}
	sched_leave_critical_region();
}
