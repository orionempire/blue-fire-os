/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.05
 *	Author: David Davidson
 *	Name: interrupt_controller.h
 *	Created: Jan 30, 2012
 *	Last Update: Jan 30, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef INTERRUPT_H_
#define INTERRUPT_H_


#define TIMER_IRQ		0
#define KEYBOARD_IRQ	1
#define FLOPPY_IRQ		6
#define SPURIOUS_IRQ	7
#define SPURIOUS_IRQ_B	15

// 8259 Programmable Interrupt Controller ports
#define MASTER_8259_COMMAND_PORT	0x20
#define SLAVE_8259_COMMAND_PORT		0xA0
// The full name of the Register is Interrupt Mask Register
#define MASTER_8259_REGISTER_PORT	0x21
#define SLAVE_8259_REGISTER_PORT	0xA1
#define END_OF_INTERRUPT			0x20



/**************************************************************************
*	---------- IDT constants ----------
**************************************************************************/
// number of entries in the IDT
#define IDT_DIMENSION 				256
// size of each IDT entry in bytes
#define IDT_ENTRY_DIMIMENSION		8

// IDT entry structure
typedef struct idt_entry {
	u16int offset0_15;		// offset bits 0..15
	u16int selector;		// a code segment selector in GDT or LDT
	u08int reserved;		// unused, must be 0
	u08int attribs;			// attributes and type (defined below)
	u16int offset16_31;		// offset bits 16..31
} idt_entry_t;

// attributes and type (PRESENT defined in mem.h)
// 0x5	80386 32 bit Task gate
#define TASK_GATE		(PRESENT | 0x05) // Task Gate
// 0xE 	80386 32-bit interrupt gate
#define INTERRUPT_GATE	(PRESENT | 0x0E) // 32-bit Interrupt Gate
// 0xF 	80386 32-bit trap gate
#define TRAP_GATE		(PRESENT | 0x0F) // 32-bit Trap Gate
// Trap gate with lower permissions
#define USER_TRAP_GATE	(TRAP_GATE | DPL_3)

typedef struct idt_reg {
	u16int limit;					// Defines the length of the IDT in bytes (minimum value is 0x100)
	u08int base[sizeof(u32int)];	// This 32 bits are the physical address where the IDT starts (INT 0)
} idt_reg_t;

// Public Function declarations
void reprogram_PIC_8259();
void initialize_IDT();
void set_IDT_gate(u08int num, u32int offset, u16int selector, u08int attributes);

#endif /* INTERRUPT_H_ */
