/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: interrupt.h
 *	Created: Jan 30, 2012
 *	Last Update: Jan 30, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef INTERRUPT_H_
#define INTERRUPT_H_

// 8259 Programmable Interrupt Controller ports
#define MASTER_8259_COMMAND_PORT	0x20
#define SLAVE_8259_COMMAND_PORT		0xA0
// The full name of the Register is Interrupt Mask Register
#define MASTER_8259_REGISTER_PORT	0x21
#define SLAVE_8259_REGISTER_PORT	0xA1
//#define EOI							0x20
#define END_OF_INTERRUPT			0x20


// Public Function declarations
void reprogram_PIC_8259();

#endif /* INTERRUPT_H_ */
