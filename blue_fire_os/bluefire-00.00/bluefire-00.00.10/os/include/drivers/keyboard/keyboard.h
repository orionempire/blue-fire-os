/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.08
 *	Author: David Davidson
 *	Name: keyboard.h
 *	Created: Nov 10 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef KEYBOARD_H_
#define KEYBOARD_H_

// --- Keyboard registers ---------------------------------------------

// Keyboard base address port.
#define KEYBOARD_PORT			0x60
// Keyboard acknowledge register.
#define KEYBOARD_ACKNOWLEDGE	0x61
// Keyboard status register.
#define KEYBOARD_STATUS			0x64
// Keyboard LED register.
#define KEYBOARD_LED_CODE		0xED

// --- Keyboard register values ---------------------------------------

// The keyboard controller is busy.
#define KEYBOARD_BUSY			0x02
// Command to set the typematic delay and rate.
#define KEYBOARD_SET_TYPEMATIC	0xF3

// --- Key codes ------------------------------------------------------ //
#define DEL_SCAN				0x53		// DEL scan code.
#define CTRL_C					0x2E03		// CTRL+C ASCII code.
#define CTRL_X					0x2D18		// CTRL+X ASCII code.
#define CTRL_D					0x2004		// CTRL+X ASCII code.
#define ALT_F1					0x6800		// ALT+F1 ASCII code.
#define ALT_F2					0x6900		// ALT+F2 ASCII code.
#define ALT_F3					0x6A00		// ALT+F3 ASCII code.
#define ALT_F4					0x6B00		// ALT+F4 ASCII code.
#define ALT_F5					0x6C00		// ALT+F5 ASCII code.
#define ALT_F6					0x6D00		// ALT+F6 ASCII code.
#define ALT_F7					0x6E00		// ALT+F7 ASCII code.
#define ALT_F8					0x6F00		// ALT+F8 ASCII code.
#define ALT_F9					0x7000		// ALT+F9 ASCII code.
#define ALT_F10					0x7100		// ALT+F10 ASCII code.
#define ALT_F11					0x8B00		// ALT+F11 ASCII code.
#define ALT_F12					0x8C00		// ALT+F12 ASCII code.
#define KEYB_UP					0x4800		// Up.
#define KEYB_DOWN				0x5000		// Down.
#define KEYB_LEFT				0x4b00		// Left.
#define KEYB_RIGHT				0x4d00		// Right.
#define KEYB_PAGE_UP			0x4900		// Page up.
#define KEYB_PAGE_DOWN			0x5100		// Page down.
#define KEYB_HOME				0x4700		// Home.
#define KEYB_END				0x4f00		// End.

/******************************************************************************
 *			--------- PUBLIC FUNCTION DECLARATIONS ----------
******************************************************************************/
void initialize_keyboard();
void keyboard_handler(irq_context_t *context);
void update_leds();
void keyboard_wait_controller();
s32int kgetchar();

#endif /* KEYBOARD_H_ */
