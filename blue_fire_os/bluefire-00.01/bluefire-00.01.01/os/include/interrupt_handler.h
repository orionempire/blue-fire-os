/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.05
 *	Author: David Davidson
 *	Name: interrupt_handler.h
 *	Created: Jan 30, 2012
 *	Last Update: Jan 30, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_


/******************************************************************************
 *				--------- EFLAGS  ----------
******************************************************************************/
// Interrupt enable flag in EFLAGS register.
#define EFLAGS_IF		0x200
// I/O privilege level 3 (minimum privilege).
// Everyone can perform I/O operation.
#define EFLAGS_IOPL3	0x3000
// I/O privilege level 2.
#define EFLAGS_IOPL2	0x2000
// I/O privilege level 1.
#define EFLAGS_IOPL1	0x1000
// I/O privilege level 0 (maximum privilege).
// Only the kernel can perform I/O operation.
#define EFLAGS_IOPL0	0x0000

// Context after an interrupt.
typedef struct irq_context {

	// General purpose register.
	u32int	edi, esi, ebp, esp, ebx, edx, ecx, eax;
	// Segment registers.
	u32int	ds, es, fs, gs;
	// The interrupt number.
	u32int	IRQ;
	// Instruction pointer, Code segment register, and EFLAGS register pushed by cpu on intial interrupt
	u32int	eip, cs, eflags;
} irq_context_t;

/**************************************************************************
* Because we are calling assembly coded functions we need to use a void function pointer.
* We are basically telling the compiler trust us blindly we know what we are doing.
* We are then taking the responsibility on ourselves to make sure that every time a
* function handler is called it has the above data structure as a argument.
**************************************************************************/
typedef void (*isr_t)(irq_context_t*);

// Total number of IRQ.
#define NUMBER_ALLOCATED_IRQ		16


// Public Function declarations
void register_interrupt_handler(u08int irq, isr_t handler);
void enable_IRQ(u08int irq);
void disable_IRQ(u08int irq);
void interrupt_handler_middle(irq_context_t *context);

#endif /* INTERRUPT_HANDLER_H_ */
