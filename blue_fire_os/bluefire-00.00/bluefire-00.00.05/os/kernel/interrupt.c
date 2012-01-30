/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: interrupt.c
 *	Created: Jan 30, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

void reprogram_PIC_8259() {
	// Yet another stunning feature of the x86 architecture is that on startup there is a IRQ conflict
	// Between the CPU and the Programmable Interrupt Controller (PIC). The PIC is using IRQ numbers 0-15 to
	// signal various hardware interrupts.At the same time by default the CPU uses 0-31 to signal when
	// it has a interrupt that needs attention. Luckily the PIC's are easy to reprogram, you just reset them.
	// As part of the reset routine you need to specify what IRQ you want that specific chip to start at.
	// You send it a reset command and then 4 ICWS specifying what features and configurations you want.


	// ICW1 (interrupt control word)
	// Write the reset command (0x11) to PIC1's command port (0x20).
	outport08(MASTER_8259_COMMAND_PORT, 0x11);
	// Write the reset command (0x11) to PIC2's command port (0xA0).
	outport08(SLAVE_8259_COMMAND_PORT, 0x11);

	// ICW2
	// Write PIC1's first isr number 0x20(32) to PIC1's data port (0x21).
	outport08(MASTER_8259_REGISTER_PORT, 0x20);
	// Write PIC2's first isr number 0x28(40) to PIC2's data port (0xA1).
	outport08(SLAVE_8259_REGISTER_PORT, 0x28);

	// ICW3
	// Tell PIC1 to signal PIC2 using its 4th interrupt line.
	outport08(MASTER_8259_REGISTER_PORT, 0x04); // IRQ2 cascade to slave
	outport08(SLAVE_8259_REGISTER_PORT, 0x02); // cascade on IRQ2

	// ICW4
	// Set PIC1 to 8086/88 (MCS-80/85) mode
	outport08(MASTER_8259_REGISTER_PORT, 0x01);
	// Set PIC2 to 8086/88 (MCS-80/85) mode
	outport08(SLAVE_8259_REGISTER_PORT, 0x01);

	// Disable all interrupts for now
	outport08(MASTER_8259_REGISTER_PORT, 0xFF);
	outport08(SLAVE_8259_REGISTER_PORT, 0xFF);
}
