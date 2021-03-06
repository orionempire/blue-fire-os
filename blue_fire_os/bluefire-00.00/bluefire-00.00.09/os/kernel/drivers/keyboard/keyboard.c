/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.08
 *	Author: David Davidson
 *	Name: keyboard.c
 *	Created: Feb 22, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

//US keyboard keymap :: regular keys.
static u16int regular_keymap[128] = {
  0x0000,0x011B,0x0231,0x0332,0x0433,0x0534,0x0635,0x0736,0x0837,0x0938,0x0A39,0x0B30,0x0C2D,0x0D3D,0x0E08,0x0F09,
  0x1071,0x1177,0x1265,0x1372,0x1474,0x1579,0x1675,0x1769,0x186F,0x1970,0x1A5B,0x1B5D,0x000D,0x1D00,0x1E61,0x1F73,
  0x2064,0x2166,0x2267,0x2368,0x246A,0x256B,0x266C,0x273B,0x2827,0x2960,0x2A00,0x2B5C,0x2C7A,0x2D78,0x2E63,0x2F76,
  0x3062,0x316E,0x326D,0x332C,0x342E,0x352F,0x3600,0x372A,0x3800,0x3920,0x3A00,0x3B00,0x3C00,0x3D00,0x3E00,0x3F00,
  0x4000,0x4100,0x4200,0x4300,0x4400,0x4500,0x4600,0x4700,0x4800,0x4900,0x4A2D,0x4B00,0x4C00,0x4D00,0x4E2B,0x4F00,
  0x5000,0x5100,0x5200,0x5300,0x5400,0x5500,0x5600,0x8500,0x8600,0x0000,0x0000,0x5B00,0x5C00,0x5D00
};

// US keyboard keymap :: "with SHIFT" keys.
static u16int with_shift_keymap[128] = {
  0x0000,0x011B,0x0221,0x0340,0x0423,0x0524,0x0625,0x075E,0x0826,0x092A,0x0A28,0x0B29,0x0C5F,0x0D2B,0x0E08,0x0F00,
  0x1051,0x1157,0x1245,0x1352,0x1454,0x1559,0x1655,0x1749,0x184F,0x1950,0x1A7B,0x1B7D,0x000D,0x1D00,0x1E41,0x1F53,
  0x2044,0x2146,0x2247,0x2348,0x244A,0x254B,0x264C,0x273A,0x2822,0x297E,0x2A00,0x2B7C,0x2C5A,0x2D58,0x2E43,0x2F56,
  0x3042,0x314E,0x324D,0x333C,0x343E,0x353F,0x3600,0x372A,0x3800,0x3920,0x3A00,0x5400,0x5500,0x5600,0x5700,0x5800,
  0x5900,0x5A00,0x5B00,0x5C00,0x5D00,0x4500,0x4600,0x4700,0x4800,0x4900,0x4A2D,0x4B00,0x4C00,0x4D00,0x4E2B,0x4F00,
  0x5000,0x5100,0x5200,0x5300,0x5400,0x5500,0x5600,0x8700,0x8800,0x0000,0x0000,0x5B00,0x5C00,0x5D00
};

// US keyboard keymap :: "with ALT" keys.
static u16int with_alt_keymap[128] = {
  0x0000,0x0100,0x7800,0x7900,0x7A00,0x7B00,0x7C00,0x7D00,0x7E00,0x7F00,0x8000,0x8100,0x8200,0x8300,0x0E00,0xA500,
  0x1000,0x1100,0x1200,0x1300,0x1400,0x1500,0x1600,0x1700,0x1800,0x1900,0x1A00,0x1B00,0x1C00,0x1D00,0x1E00,0x1F00,
  0x2000,0x2100,0x2200,0x2300,0x2400,0x2500,0x2600,0x2700,0x2800,0x2900,0x2A00,0x2B00,0x2C00,0x2D00,0x2E00,0x2F00,
  0x3000,0x3100,0x3200,0x3300,0x3400,0x3500,0x3600,0x3700,0x3800,0x3900,0x3A00,0x6800,0x6900,0x6A00,0x6B00,0x6C00,
  0x6D00,0x6E00,0x6F00,0x7000,0x7100,0x4500,0x4600,0x9700,0x9800,0x9900,0x4A00,0x9B00,0x9C00,0x9D00,0x4E00,0x9F00,
  0xA000,0xA100,0xA200,0xA300,0x5400,0x5500,0x5600,0x8B00,0x8C00
};

// US keyboard keymap :: "with CTRL" keys.
static u16int with_control_keymap[128] = {
  0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x9400,
  0x1011,0x1117,0x1205,0x1312,0x1414,0x1519,0x1615,0x1709,0x180F,0x1910,0x0000,0x0000,0x1C0A,0x1D00,0x1E01,0x1F13,
  0x2004,0x2106,0x2207,0x2308,0x240A,0x250B,0x260C,0x0000,0x0000,0x0000,0x2A00,0x0000,0x2C1A,0x2D18,0x2E03,0x2F16,
  0x3002,0x310E,0x320D,0x0000,0x0000,0x9500,0x3600,0x9600,0x3800,0x3920,0x3A00,0x5E00,0x5F00,0x6000,0x6100,0x6200,
  0x6300,0x6400,0x6500,0x6600,0x6700,0x4500,0x4600,0x7700,0x8D00,0x8400,0x8E00,0x7300,0x8F00,0x7400,0x9000,0x7500,
  0x9100,0x7600,0x9200,0x9300,0x5400,0x5500,0x5600,0x8900,0x8A00
};

// The keypad on the side is handled as a one off map.
static u08int keypad_char[] = {'7','8','9','-','4','5','6','+','1','2','3','0','.'};

// Shift key flag.
static u08int shift_flag = 0;
// Control key flag.
static u08int control_flag = 0;
// Alt key flag.
static u08int alt_flag = 0;

/******************************************************************************
 *	---------  ----------
******************************************************************************/
void keyboard_wait_controller() {
	s32int retries = 500000;

	while (((inport08(KEYBOARD_STATUS) & KEYBOARD_BUSY) == KEYBOARD_BUSY) && retries!=0) {
			retries--;
	}
}

/******************************************************************************
 *	---------  ----------
******************************************************************************/
void update_leds() {
	s32int leds;
	console_t *curr_cons = get_console_addr( get_curr_console() );

	leds = (curr_cons->scroll_lock) | (curr_cons->num_lock << 1) | (curr_cons->caps_lock << 2);

	u32int flags;
	disable_and_save_interrupts(flags);

	keyboard_wait_controller();
	outport08(KEYBOARD_PORT, KEYBOARD_LED_CODE);
	keyboard_wait_controller();
	outport08(KEYBOARD_PORT, leds);


	restore_interrupts(flags);

}

/******************************************************************************
 *	---------  ----------
******************************************************************************/
u16int scan_key() {
	static s32int code, val;

	u32int flags;
	disable_and_save_interrupts(flags);

	code = inport08(KEYBOARD_PORT);				// Get scan code
	val =  inport08(KEYBOARD_ACKNOWLEDGE);		// Get keyboard acknowledge
	outport08(KEYBOARD_ACKNOWLEDGE, val | 0x80);// Disable bit 7
	outport08(KEYBOARD_ACKNOWLEDGE, val);		// Send that back

	restore_interrupts(flags);
	return( code );
}

/******************************************************************************
 *	---------  ----------
 *	Keyboard interrupt handler routine. It is invoked every time a key is
 *	pressed or released on the keyboard.
******************************************************************************/
void keyboard_handler(irq_context_t *context) {

	console_t *curr_cons;
	u16int code;
	u16int keypressed;

	keypressed = scan_key();

	// Get the current console structure.
	curr_cons = get_console_addr( get_curr_console() );

	// If CTRL+ALT+DEL is pressed Reboot the system.
	if (control_flag == TRUE && alt_flag == TRUE && keypressed == DEL_SCAN) {
			reboot();
	}

	if (alt_flag == TRUE ) {
		code = with_alt_keymap[keypressed];
	} else if ( control_flag == TRUE) {
		code = with_control_keymap[keypressed];
	} else {
		if ((curr_cons->caps_lock == FALSE) && (shift_flag == FALSE)) {
			code = regular_keymap[keypressed];
		} else if ((curr_cons->caps_lock == FALSE) && (shift_flag == TRUE)) {
			code = with_shift_keymap[keypressed];
		} else if ((curr_cons->caps_lock == TRUE) && (shift_flag == FALSE))	{
			code = regular_keymap[keypressed];
			if ((( code & 0xFF) >= 'a') && (( code & 0xFF) <= 'z')) {
				code -= ('a'-'A');
			}
		} else {
			code = with_shift_keymap[keypressed];
			if ((( code & 0xFF ) >='A' ) && ((code & 0xFF) <= 'Z')){
				code += ('a'-'A');
			}
		}


		if (( curr_cons->num_lock != shift_flag) && ( keypressed >= 71) && (keypressed <= 83) ) {
			code = keypad_char[(keypressed - 71)];
		}
	}

	// Print the char only if it's not released (bit 8 set).
	switch (keypressed)	{
		case 42:			// Left Shift pressed
			shift_flag = TRUE;
		break;

		case (42+128):		// Left Shift Released
			shift_flag = FALSE;
		break;

		case 54:			// Right Shift pressed
			shift_flag = TRUE;
		break;

		case (54+128):		// Right Shift Released
			shift_flag = FALSE;
		break;

		case 56:			// ALT pressed
			alt_flag = TRUE;
		break;

		case (56+128):		// ALT Released
			alt_flag = FALSE;
		break;

		case 29:			// CTRL pressed
			control_flag = TRUE;
		break;

		case (29+128):		// CTRL Released
			control_flag = FALSE;
		break;

		case 58:	// CAPS LOCK
			curr_cons->caps_lock ^= TRUE;
			update_leds();
		break;

		case 69:	// NUM LOCK
			curr_cons->num_lock ^= TRUE;
			update_leds();
		break;

		case 70:	// SCROLL LOCK
			curr_cons->scroll_lock ^= TRUE;
			update_leds();
		break;

		default:
		// Update keyboard buffer. This is an interrupt
		// handler so we are already in mutual exclusion!!!
		if ( !(keypressed & 0x80 )) {
			// A key has been released => update the buffer
			if ( (curr_cons->keyb_buf_count) < KEYBOARD_BUFFER_DIMENSION ) {
				(curr_cons->keyb_buf_count)++;
				if ( ++(curr_cons->keyb_buf_write) >= KEYBOARD_BUFFER_DIMENSION ){
					curr_cons->keyb_buf_write = 0;
				}
				curr_cons->keyb_buffer[curr_cons->keyb_buf_write] = code;
			} else{
				// TODO
				// Beep when buffer full
				// beep();
				PANIC("Speaker not yet implemented")
			}
		}
		break;
	}
	enable_IRQ(context->IRQ);

}

/******************************************************************************
 *	---------  ----------
 *	Wait for key pressed and return the selected character
******************************************************************************/
s32int kgetchar() {
	u16int key;
	u32int flags;
	disable_and_save_interrupts(flags);

	console_t *curr_cons = get_console_addr( get_curr_console() );

	// If the keyboard buffer is empty enable interrupts and wait...
	if (curr_cons->keyb_buf_count == 0){
		enable_interrupts();
	}

	while ( curr_cons->keyb_buf_count == 0) {
		idle();
	}

	disable_interrupts();

	// Update keyboard buffer in mutual exclusion
	curr_cons->keyb_buf_count--;

	// wrap around the buffer
	if (++(curr_cons->keyb_buf_read) >= KEYBOARD_BUFFER_DIMENSION) {
		curr_cons->keyb_buf_read = 0;
	}

	key = curr_cons->keyb_buffer[curr_cons->keyb_buf_read];

	restore_interrupts(flags);

	return key;
}
/******************************************************************************
 *	---------  ----------
******************************************************************************/
void initialize_keyboard() {

	update_leds();

	register_interrupt_handler(KEYBOARD_IRQ, &keyboard_handler);
}
