/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.05
 *	Author: David Davidson
 *	Name: interrupt_controller.c
 *	Created: Jan 30, 2012
 *	Purpose: Routines used to adjust the PIC_8259 and to handle entries in the IDT.
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Exception stubs
// Declared in os/assembly/exception_top.s
extern u32int exc_00, exc_01, exc_02, exc_03, exc_04, exc_05, exc_06, exc_07;
extern u32int exc_08, exc_09, exc_0A, exc_0B, exc_0C, exc_0D, exc_0E, exc_0F;
extern u32int exc_unhandled;

// Interrupt request stubs
// Declared in os/assembly/interrupt_top.s
extern u32int irq_00, irq_01, irq_02, irq_03, irq_04, irq_05, irq_06, irq_07;
extern u32int irq_08, irq_09, irq_0A, irq_0B, irq_0C, irq_0D, irq_0E, irq_0F;
extern u32int irq_unhandled;

// The Interrupt Descriptor Table (IDT)
// The IDT register will point here
volatile idt_entry_t idt[IDT_DIMENSION];
idt_reg_t idt_ptr;

// If this is set to 1 a kernel message will print out.
s32int spurious_interrupt_show = 0;

void reprogram_PIC_8259() {
	// Yet another stunning feature of the x86 architecture is that on startup there is a IRQ conflict
	// Between the CPU and the Programmable Interrupt Controller (PIC). The PIC is using IRQ numbers 0-15 to
	// signal various hardware interrupts. At the same time by default the CPU uses 0-31 to signal when
	// it has a interrupt (called a exception) that needs attention. Luckily the PIC's are easy to reprogram,
	// you just reset them. As part of the reset routine you need to specify what IRQ you want that specific
	// chip to start at. You send it a reset command and then 4 ICWS specifying what features and configurations you want.

	// ICW1 (interrupt control word)
	// Write the reset command (0x11) to PIC1's command port (0x20).
	outport08(MASTER_8259_COMMAND_PORT, 0x11);
	// Write the reset command (0x11) to PIC2's command port (0xA0).
	outport08(SLAVE_8259_COMMAND_PORT, 0x11);

	// ICW2
	// Write PIC1's first isr number 0x20(32) to PIC1's data port (0x21).
	// This means when a keyboard key is pressed, IRQ line 1 lights which calls the 32 entry in the
	// IDT table (ISR 32)
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

void set_IDT_gate(u08int num, u32int offset, u16int selector, u08int attributes) {
	u32int flags;
	disable_and_save_interrupts(flags);

	idt[num].offset0_15 = offset & 0xFFFF;
	idt[num].selector = selector;
	idt[num].reserved = 0;	//By INTEL definition
	idt[num].attribs = attributes;
	idt[num].offset16_31 = (offset >> 16);

	restore_interrupts(flags);
}

// The interrupt controller generates random interrupts at times on line 7 and 15
// turning them off wont help, so for now they get blocked and logged if wanted.
void spurious_interrupt(irq_context_t *context) {
	if(spurious_interrupt_show) {
		kprintf("\nSpurious interrupt: ->%x\n",context->IRQ);
	}
}

void fake_keyboard(irq_context_t *context) {
	static s32int code, val;
	u32int flags;
	disable_and_save_interrupts(flags);

	// If the scan is not acknowledged no further interrupts will be generated
	code = inport08(0x60);	// Get scan code
	val =  inport08(0x61);	// Get keyboard acknowledge
	outport08(0x61, val | 0x80); // Disable bit 7
	outport08(0x61, val);	// Send that back

	kprintf("\nKeyboard interrupt %x caught, scan code %x: \n", context->IRQ,code);
	enable_IRQ(context->IRQ);
}

void initialize_IDT() {
	u32int i;

	for(i=0x00; i <= 0x1F; i++) {
		set_IDT_gate(i, (u32int)&exc_unhandled, KERNEL_CODE, INTERRUPT_GATE);
	}

	// Setup the IDT entries for exceptions.
	// Install Division by 0 handler
	set_IDT_gate( 0x00, (u32int)&exc_00, KERNEL_CODE, TRAP_GATE);
	// Install Debug trap handler
	set_IDT_gate( 0x01, (u32int) &exc_01, KERNEL_CODE, TRAP_GATE);
	// Install Non-Maskable Interrupt
	set_IDT_gate( 0x02, (u32int)&exc_02, KERNEL_CODE, TRAP_GATE); //from INTERRUPT_GATE
	// Install Breakpoint handler
	set_IDT_gate( 0x03, (u32int)&exc_03, KERNEL_CODE, USER_TRAP_GATE);
	// Install Overflow handler
	set_IDT_gate( 0x04, (u32int)&exc_04, KERNEL_CODE, USER_TRAP_GATE);
	// Install Bound range handler
	set_IDT_gate( 0x05, (u32int)&exc_05, KERNEL_CODE, USER_TRAP_GATE);
	// Install Invalid opcode handler
	set_IDT_gate( 0x06 , (u32int)&exc_06, KERNEL_CODE, TRAP_GATE);
	// Install no math co-processor handler
	set_IDT_gate( 0x07 , (u32int) &exc_07, KERNEL_CODE, TRAP_GATE);
	// Install Double fault handler
	set_IDT_gate( 0x08 , (u32int)&exc_08, KERNEL_CODE, TRAP_GATE); //from TASK_GATE
	// Install 80x87 overflow handler
	set_IDT_gate( 0x09, (u32int)&exc_09, KERNEL_CODE, TRAP_GATE);
	// Install Invalid TSS handler
	set_IDT_gate( 0x0A, (u32int)&exc_0A, KERNEL_CODE, TRAP_GATE);
	// Install Segment not present handler
	set_IDT_gate( 0x0B, (u32int)&exc_0B, KERNEL_CODE, TRAP_GATE);
	// Install Stack fault handler
	set_IDT_gate( 0x0C, (u32int)&exc_0C, KERNEL_CODE, TRAP_GATE);
	// Install General protection fault handler
	set_IDT_gate( 0x0D, (u32int)&exc_0D, KERNEL_CODE, TRAP_GATE);
	// Install Page fault handler
	set_IDT_gate( 0x0E, (u32int)&exc_0E, KERNEL_CODE, TRAP_GATE);	//from INTERRUPT_GATE
	// Install Unknown exception handler
	set_IDT_gate( 0x0F, (u32int)&exc_0F, KERNEL_CODE, TRAP_GATE);	//from INTERRUPT_GATE
	// Interrupts 0x10 through 0x1f Remain pointing to exc_unhandled.

	// Install default IRQ handlers 0x20..0xFF
	for(i=0x20;i<=0xFF;i++)
		set_IDT_gate(i, (u32int)&irq_unhandled, KERNEL_CODE, INTERRUPT_GATE);

	// Setup the IDT entries for IRQs.
	set_IDT_gate( 0x20, (u32int)&irq_00, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x21, (u32int)&irq_01, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x22, (u32int)&irq_02, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x23, (u32int)&irq_03, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x24, (u32int)&irq_04, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x25, (u32int)&irq_05, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x26, (u32int)&irq_06, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x27, (u32int)&irq_07, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x28, (u32int)&irq_08, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x29, (u32int)&irq_09, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x2A, (u32int)&irq_0A, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x2B, (u32int)&irq_0B, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x2C, (u32int)&irq_0C, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x2D, (u32int)&irq_0D, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x2E, (u32int)&irq_0E, KERNEL_CODE, INTERRUPT_GATE);
	set_IDT_gate( 0x2F, (u32int)&irq_0F, KERNEL_CODE, INTERRUPT_GATE);

	//TODO system calls

	// Set up the IDT pointer.
	idt_ptr.limit = (IDT_DIMENSION * IDT_ENTRY_DIMIMENSION - 1);
	*(u32int *)idt_ptr.base = ((u32int)&idt);

	// Load info into IDTR register.
	__asm__ __volatile__ ("lidtl (%0)" : : "r"((u32int)&idt_ptr));

	// install spurious interrupt
	register_interrupt_handler(SPURIOUS_IRQ, &spurious_interrupt);
	register_interrupt_handler(SPURIOUS_IRQ_B, &spurious_interrupt);
	register_interrupt_handler(KEYBOARD_IRQ, &fake_keyboard);

	enable_interrupts();
}


