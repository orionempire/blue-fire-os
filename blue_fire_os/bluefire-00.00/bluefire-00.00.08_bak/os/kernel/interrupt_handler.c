/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.05
 *	Author: David Davidson
 *	Name: interrupt_handler.c
 *	Created: Jan 30, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include "../../../bluefire-00.00.08_bak/os/include/common_include.h"

//indexed by IRQ
isr_t interrupt_handlers[256] = { 0 };


/**************************************************************************
* This gets called from interrupt_top.asm and calls the installed function
* pointer to the proper handler.
***************************************************************************/
void interrupt_handler_middle(irq_context_t *context) {
	// Check if this interrupt involved the slave. This was set in os/interrupt_controller/reprogram_PIC_8259
	if (context->IRQ >= 0x7) {
		//Signal a EOI (end of interrupt) by sending a reset signal to the PICs
		outport08(SLAVE_8259_COMMAND_PORT, END_OF_INTERRUPT);
	}
	// we always send one to the master
	outport08(MASTER_8259_COMMAND_PORT, END_OF_INTERRUPT);

	if (interrupt_handlers[context->IRQ] != NULL) {
		isr_t handler = interrupt_handlers[context->IRQ];
		handler(context);
	} else {
    	kprintf("Un-handled interrupt: ->%x\n",context->IRQ);
    	PANIC("Un-handled interrupt");
    }

}

/**************************************************************************
* The PIC will no longer respond to specified IRQ wire
***************************************************************************/
void disable_IRQ(u08int irq) {
	static u08int mask;

	if (irq > NUMBER_ALLOCATED_IRQ) {
		return;
	}

	if (irq < 8) {		// Master
		mask = inport08(MASTER_8259_REGISTER_PORT);
		mask |= 1 << irq;
		outport08(MASTER_8259_REGISTER_PORT, mask);
	} else {			// Slave
		mask = inport08(SLAVE_8259_REGISTER_PORT);
		mask |= 1 << (irq - 8);
		outport08(SLAVE_8259_REGISTER_PORT, mask);
	}
}

/**************************************************************************
* The PIC will now respond to specified IRQ wire
***************************************************************************/
void enable_IRQ(u08int irq) {
	static u08int mask;

	if (irq > NUMBER_ALLOCATED_IRQ) {
		return;
	}

	if (irq < 8) {	// Master
		mask = inport08(MASTER_8259_REGISTER_PORT);
		mask &= ~(1 << irq);
		outport08(MASTER_8259_REGISTER_PORT, mask);
	} else {		// Slave
		mask = inport08(SLAVE_8259_REGISTER_PORT);
		mask &= ~(1 << (irq - 8));
		outport08(SLAVE_8259_REGISTER_PORT, mask);
	}
}

/**************************************************************************
* Install the function that will be called.
***************************************************************************/
void register_interrupt_handler(u08int irq, isr_t handler) {
	u32int flags;

	if ( irq > NUMBER_ALLOCATED_IRQ ) {
			return;
	}

	if (interrupt_handlers[irq] != NULL) {
			PANIC("Multiple IRQ's on the same line not yet implemented.")
	}

	disable_and_save_interrupts(flags);

    interrupt_handlers[irq] = handler;
    enable_IRQ(irq);

	restore_interrupts(flags);
}

/**************************************************************************
* Install the function that will be called.
***************************************************************************/
void unregister_interrupt_handler(u08int irq) {
	u32int flags;

	if ( irq > NUMBER_ALLOCATED_IRQ ) {
			return;
	}

	disable_and_save_interrupts(flags);

    interrupt_handlers[irq] = NULL;
    disable_IRQ(irq);

	restore_interrupts(flags);
}
