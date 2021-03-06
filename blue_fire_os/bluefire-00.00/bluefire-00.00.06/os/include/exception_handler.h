/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.05
 *	Author: David Davidson
 *	Name: exception_handler.h
 *	Created: Jan 30, 2012
 *	Last Update: Jan 30, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef EXCEPTION_HANDLER_H_
#define EXCEPTION_HANDLER_H_

// Context pushed by assembly/exception_top.asm
typedef struct exception_context {
	// General purpose register.
	u32int	edi, esi, ebp, esp, ebx, edx, ecx, eax;
	// Segment registers.
	u32int	ds, es, fs, gs;
	// The exception number and error code, pushed by CPU.
	u32int	EXC, err_code;
	// Instruction pointer, Code segment register, and EFLAGS register pushed by cpu
	u32int	eip, cs, eflags;
} exception_context_t;

// helper functions

/*
static __inline__ void dump_stack( exc_context_t *c ) {
	u32int *esp = (u32int *)curr_task->pl0_stack;
	s32int i = 0;

	while (esp < (uint32_t *)c) {
		if ((*esp >= K_VIR_START) && (*esp < K_HEAP_START)) {
			printk(KERN_WARNING "[ %p ] = %p\n", esp, *esp);
			if (++i >= 12)
				break;
		}
		esp++;
	}
}*/


static __inline__ void dump_registers( exception_context_t *c ) {
	//register u16int ss, fs, gs;
	//register u32int cr0, cr2, cr3, cr4;
	u16int ss, fs, gs;
	u32int cr0, cr2, cr3, cr4;

	//dump_stack(c);
	//PANIC("stackdump not yet implemented");
	dbg("stackdump not yet implemented");

	// Save stack segment register.
	__asm__ __volatile__ ("movw %%ss, %0" : "=&r"(ss) : );

	// Save %fs and %gs registers.
	__asm__ __volatile__ ("movw %%fs, %0" : "=m"(fs) :);
	__asm__ __volatile__ ("movw %%gs, %0" : "=m"(gs) :);

	// Save control registers.
	__asm__ __volatile__ ("mov %%cr0, %0" : "=&r"(cr0) : );
	__asm__ __volatile__ ("mov %%cr2, %0" : "=&r"(cr2) : );
	__asm__ __volatile__ ("mov %%cr3, %0" : "=&r"(cr3) : );
	__asm__ __volatile__ ("mov %%cr2, %0" : "=&r"(cr4) : );

	// Dump registers.
	kprintf("\neax = %#010x  ds = %#010x  cr0 = %#010x  esp    = %#010x", c->eax, (u16int)(c->ds), cr0, c);
	kprintf("\nebx = %#010x  es = %#010x  cr2 = %#010x  ebp    = %#010x", c->ebx, (u16int)(c->es), cr2, c->ebp);
	kprintf("\necx = %#010x  fs = %#010x  cr3 = %#010x  eip    = %#010x", c->ecx, (u16int)(fs), cr3, c->eip);
	kprintf("\nedx = %#010x  gs = %#010x  cr4 = %#010x  eflags = %#010x", c->edx, (u16int)(gs), cr4, c->eflags);
	kprintf("\nesi = %#010x  ss = %#010x  exc = %#010x", c->esi, (u16int)(ss), c->EXC);
	kprintf("\nedi = %#010x  cs = %#010x  err = %#010x", c->edi, (u16int)(c->cs), c->err_code);
}

// Public Function declarations
void exception_handler_middle(exception_context_t *context);

#endif /* EXCEPTION_HANDLER_H_ */
