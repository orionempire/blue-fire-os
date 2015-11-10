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

// Task Constants
// Stack size in byte for every tasks
#define STACK_SIZE	0x8000

// Task states
#define READY		1
#define WAIT		2
#define ZOMBIE		3

// Task types
#define KTHREAD_T	0
#define PROCESS_T	1

// TSS (no I/O bit map)
typedef struct TSS {
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
} tss_t;

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

// Task structure
typedef struct TASK
{
	tss_IO_t	tss;		// TSS must go first so that it is page aligned
	u16int		tss_sel;
	u32int		pid;		// Unique identification number
	u32int		pl0_stack;	// Pointer to the privileged stack of the task.
	u08int		state;		// Running, Wait on IO, Sleeping, etc...
	u08int		priority;
	u08int		type;
	s08int		*name;
} task_t;

static __inline__ void jmp_to_tss(u16int tss_sel) {
	static struct {
		unsigned eip : 32; // 32 bit
		unsigned cs  : 16; // 16 bit
	} tss_link;

	// Jump to the selected tss
	tss_link.eip = 0;
	tss_link.cs = tss_sel;
	__asm__ __volatile__ ("ljmp *(%0)" : : "m" (tss_link));
}


// Public Function declarations
s32int get_pid();
void initialize_multitasking();
s08int *get_pname();
void sched_enter_critical_region();
void sched_leave_critical_region();
void do_idle();
task_t *create_kthread(void *address, s08int *pname);
task_t *create_process(void *address, void *buffer, s08int *pname);
void auto_kill();
void scheduler();
void ps();

#endif /* TASK_H_ */
