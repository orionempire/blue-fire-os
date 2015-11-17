/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: exception_handler.c
 *	Created: Jan 30, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Page fault panic flag
s32int pf_panic;



// From: <INTEL Software Developer's Manual 2011>
//
// Identifier   Description
//
// 0            Divide error
// 1            Debug exceptions
// 2            Non-maskable interrupt
// 3            Breakpoint (INT 3 instruction)
// 4            Overflow (INTO instruction)
// 5            BOUND Range Exceeded (BOUND instruction)
// 6            Invalid opcode
// 7            Device Not Available (No Math Co-processor)
// 8            Double fault
// 9            CoProcessor Segment Overrun (reserved)
// 10 (0xA)     Invalid TSS
// 11 (0xB)     Segment not present
// 12 (0xC)     Stack Segment Fault
// 13 (0xD)     General protection
// 14 (0xE)     Page fault
// 15 (0xF)     (reserved)
// 16 (0x10)	Floating-Point Error (Math Fault)
// 17 (0x11)	Alignment Check
// 18 (0x12)	Machine Check
// 19 (0x13)	SIMD Floating-Point Exception
// 20-31        (reserved)
// 32-255       Available for external interrupts via INTR pin
/**************************************************************************
* Exceptions are hard coded as they don't change.
* Note: For a Interrupt control will return to the next instruction.
* 	for exceptions control will return to the original instruction so
* 	if it can't be fixed then a panic is the only option.
* 	For Ex. If the panic was removed then
* 	a1 = 1/0
* 	a2 = 2/0
* 	would never get to a2 but just keep triggering exceptions on a1.
***************************************************************************/
void exception_handler_middle(exception_context_t *context) {
	u32int cr2;

	switch (context->EXC)	{
		case 0x00:
			kprintf("\nException 00: DIVISION BY ZERO\n");
		break;

		case 0x01:
			kprintf("\nException 01: DEBUG EXCEPTION DETECTED\n");
		break;

		case 0x02:
			kprintf("\nException 02: NON MASKABLE INTERRUPT\n");
		break;

		case 0x03:
			kprintf("\nException 03: BREAKPOINT INSTRUCTION DETECTED\n");
			dump_registers(context);
			//kprintf(KERN_WARNING "\nBreakpoint from task [%s] (%i)\n", get_pname(), get_pid());
			PANIC("Breakpoint not yet implemented");
			// After a breakpoint we can restore execution.
			return;
		break;

		case 0x04:
			kprintf("\nException 04: INTO DETECTED OVERFLOW\n");
		break;

		case 0x05:
			kprintf("\nException 05: BOUND RANGE EXCEEDED\n");
		break;

		case 0x06:
			kprintf("\nException 06: INVALID OPCODE\n");
		break;

		case 0x07:
			kprintf("\nException 07: PROCESSOR EXTENSION NOT AVAILABLE\n");
		break;

		case 0x08:
			kprintf("\nException 08: DOUBLE FAULT DETECTED\n");
		break;

		case 0x09:
			kprintf("\nException 09: PROCESSOR EXTENSION PROTECTION FAULT\n");
		break;

		case 0x0A:
			kprintf("\nException 0A: INVALID TASK STATE SEGMENT\n");
		break;

		case 0x0B:
			kprintf("\nException 0B: SEGMENT NOT PRESENT\n");
		break;

		case 0x0C:
			kprintf("\nException 0C: STACK FAULT\n");
		break;

		case 0x0D:
			if ((context->eflags & EFLAGS_VM) == EFLAGS_VM) {
				PANIC("v86_mode not supported by this OS!");
				return;
			} else {
				kprintf("\nException 0D: GENERAL PROTECTION FAULT\n");
			}
		break;

		case 0x0E:
			__asm__ __volatile__ ("movl %%cr2, %0" : "=r"(cr2) : );

			if ((context->err_code & P_PRESENT) != P_PRESENT) {
				if (!page_fault_handler(context->err_code, cr2)) {
					return;
				} else {
					kprintf("\nException 0E: PAGE FAULT (Out of memory)!\n");
				}
			} else {
				kprintf("\nException 0E: PAGE FAULT  (page level protection violation)!\n");
			}
		break;

		case 0x0F:
			kprintf("\nException 0F: UNKNOWN EXCEPTION\n");
		break;

		default:
			kprintf("\nException #%X: UNEXPECTED EXCEPTION !!!\n", (u32int)context->EXC);
		break;
	}

	// If control arrives here then it was a exception that could not be handled.
	dump_registers(context);
	PANIC("End of default_exception() reached, this is not done in this nation.");
}
