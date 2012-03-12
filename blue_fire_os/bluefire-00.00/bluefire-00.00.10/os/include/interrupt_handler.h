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
