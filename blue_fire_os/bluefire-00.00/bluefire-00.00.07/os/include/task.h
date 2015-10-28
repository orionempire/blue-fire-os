/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.07
 *	Author: David Davidson
 *	Name: task.h
 *	Created: Feb 8, 2012
 *	Last Update: Feb 8, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef TASK_H_
#define TASK_H_

// Stack size in byte for every kernel-mode task
// Use at least 2 pages .
#define STACK_SIZE		0x4000


// --------------------------------TASK PRIVILEGE--------------------------- //
#define KERNEL_PRIVILEGE	0 	// Max privilege (kernel).
#define USER_PRIVILEGE		3 	// Min privilege (user).

// --------------------------------TASK STATE------------------------------- //
#define TASK_NEW		0 		// The task is going to be created.
#define TASK_READY		1 		// The task is ready for the CPU.
#define TASK_WAIT		2 		// The task is waiting for I/O.
#define TASK_ZOMBIE		4 		// The task is dying.

// --------------------------------TASK STATE------------------------------- //
#define THREAD_TYPE		0x00 //!< A kernel thread type.
#define PROCESS_TYPE		0x01 //!< A task type.

//! TSS without I/O bit map.
typedef struct tss
{
	uint32_t	link;
	uint32_t	esp0;
	uint16_t	ss0, __ss0h;
	uint32_t	esp1;
	uint16_t	ss1, __ss1h;
	uint32_t	esp2;
	uint16_t	ss2, __ss2h;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax, ecx, edx, ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint16_t	es, __esh;
	uint16_t	cs, __csh;
	uint16_t	ss, __ssh;
	uint16_t	ds, __dsh;
	uint16_t	fs, __fsh;
	uint16_t	gs, __gsh;
	uint16_t	ldtr, __ldtrh;
	uint16_t	trace, io_map_addr;
} tss_t;


// Size in double-words of the I/O bit map.
#define IO_MAP_SIZE	2048

// Task state segment with I/O bit map
typedef struct TSS_IO {
	u32int	link,
			esp0,
			ss0,
			esp1,
			ss1,
			esp2,
			ss2,
			cr3,
			eip,
			eflags,
			eax,
			ecx,
			edx,
			ebx,
			esp,
			ebp,
			esi,
			edi,
			es,
			cs,
			ss,
			ds,
			fs,
			gs,
			ldtr;
	u16int	trace,
			io_map_addr;
	u08int	io_map[8192];	// I/O redirection bit map
} tss_IO_t;

//! The flags structure for a task.
typedef struct task_flags
{
	unsigned long	type		: 1;
	unsigned long	uninterruptible	: 1;
	unsigned long	__res		: 30;
} __attribute__ ((packed)) task_flags_t;

// Task structure
typedef struct TASK
{
	// Must be the first to allow the TSS to be page-aligned.
	tss_IO_t	tss;
	// The selector the tss.
	u16int	tss_sel;

	// The page directory of the task.
	u32int	*pdbr;

	// A counter to report if is necessary to update the page directory with the global kernel page directory.
	// In this way we preserve coherency of the kernel memory area between all tasks.
	unsigned long	pdbr_update_counter;

	// Process credentials.
	s32int		pid;
	u16int		uid, euid, suid, fsuid;
	u16int		gid, egid, sgid, fsgid;

	// A pointer to the privileged stack of the task.
	size_t		pl0_stack;
	// A pointer to the user level stack.
	size_t		stack;

	// The start of the task heap.
	size_t		heap_start;
	// The size of the task heap.
	size_t		heap_size;
	// A semaphore for the heap mutual exclusion .
	semaphore_t	heap_sem;

	// The state of the task.
	u08int		state;
	// A pointer to the task structure of the father.
	struct task	*father;
	// The console where task runs.
	s32int		console;
	// The task flags.
	task_flags_t	flags;
	// The privilege level of the task.
	s32int		privilege;
	// The initial priority.
	s32int		priority;
	// The dynamic priority.
	s32int		counter;
	// The name of the task.
	s08int		name[ 256 ];
	// The current working directory of the task.
	int		cwd;
} task_t;

// Switch into a different task virtual address space.
static __inline__ void task_switch_mmu( task_t *prev, task_t *next ) {
	u32int cr3_prev = vir_to_phys( (size_t)(prev->pdbr) );
	u32int cr3_next = vir_to_phys( (size_t)(next->pdbr) );

	if( cr3_prev != cr3_next ) {
		if( next->pdbr_update_counter != prev->pdbr_update_counter ) {
			u32int flags;
			disable_and_save_interrupts( flags );

			//TODO
			// Update the page directory for kernel address space.
			// This must be done to preserve coherency between
			// page directories of each task.
			memcpy08( PDE_OFFSET(next->pdbr, VIRTUAL_KERNEL_START), PDE_OFFSET(prev->pdbr, VIRTUAL_KERNEL_START),
				( PDE_INDEX(VIRTUAL_PAGE_DIRECTORY_START) - PDE_INDEX(VIRTUAL_KERNEL_START) ) * sizeof(u32int) );

			// Update the pdbr counter.
			next->pdbr_update_counter = prev->pdbr_update_counter;

			restore_interrupts( flags );

		}
		// Switch into the new virtual space.
		switch_mmu( cr3_next );
	}
}
// Public Function declarations
s32int get_pid();
void initialize_multitasking();
//s08int *get_pname();
void sched_enter_critical_region();
void sched_leave_critical_region();
//void do_idle();
//task_t *create_kthread(void *address, s08int *pname);
//task_t *create_process(void *address, void *buffer, s08int *pname);
task_t *create_process(void *routine, s32int argc, s08int **argv, s08int *pname, s32int privilege);
//void auto_kill();
//void scheduler();
//void ps();

#endif /* TASK_H_ */
