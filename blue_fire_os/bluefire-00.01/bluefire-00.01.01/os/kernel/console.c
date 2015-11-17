/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: console.c
 *	Created: Jan 11, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

static console_t vir_cons[K_TOT_VIR_CONS+1];

// Index of the current console.
static s32int current_console_id = 0;

/**************************************************************************
* initialize_paging re-mapped lower memory from starting at 0x0 to starting
* at 0xE0000000 so V(0xE00B8000) -> P(0x000B8000)
**************************************************************************/
void initialize_boot_console() {
	vir_cons[0].vid_buffer = (u16int *)0xE00B8000;
	vir_cons[0].cursor_x = 0;
	vir_cons[0].cursor_y = 0;
	vir_cons[0].cur_color = DEFAULT_COLOR;

	// Clear the screen.
	video_clrscr( (console_t *)(&vir_cons[0]) );
}

void kclrscr() {
	video_clrscr( (console_t *)(&vir_cons[0]) );
}
/**************************************************************************
* Put a character on the current console.
* c - The ASCII value of the character.
**************************************************************************/
s32int kputchar( s32int c ) {
	u32int flags;

	disable_and_save_interrupts(flags);
	// For  now just print to the video buffer.
	video_put_char(c, (console_t *)(&vir_cons[0]));

	restore_interrupts(flags);

	return( 0 );

}

/**************************************************************************
* Put a character on the current console.
* c - The ASCII value of the character.
**************************************************************************/
s32int kplacechar( u08int x_pos, u08int y_pos, s32int c ) {
	u32int flags;

	disable_and_save_interrupts(flags);
	// For  now just print to the video buffer.
	video_place_char(x_pos, y_pos, c,(console_t *)(&vir_cons[0]));

	restore_interrupts(flags);

	return( 0 );

}

/**************************************************************************
* Set the text color for the current console.
* attrib - The color value.
**************************************************************************/
void kset_color(u08int attrib) {
	video_set_color( (console_t *)(&vir_cons[0]), attrib );
}

/**************************************************************************
* Transforms a argument list in to characters on the screen.
* In the function
* 			var=2; kprintf(   "Hi %d \n -", var)
* the string starting with the quotation mark, is literally (in HEX ASCII)
* 			  	42 48 69 20 25 64 20 5C 6E 20 55 42
* the compiler interprets the quotation marks and translates "\n" to '\n'.
* 				48 69 20 25 64 20 0A 20 2D
* the function vsnprintf translates the specifier into "2"
* 				48 69 20 32 20 0A 20 2D
* the function kputchar will put the characters in the proper place
* 				Hi 2 <moves cursor to next line>
* 				-
**************************************************************************/
s32int kprintf(const s08int *fmt, ...) {
	s08int kprintf_buf[1024];
	va_list args;
	u32int i;

	va_start( args, fmt );
	vsnprintf( kprintf_buf, sizeof(kprintf_buf), fmt, args );
	va_end( args );

	// Writing to the video buffer must be mutual exclusive. For now interrupts are
	// simply disabled. In the future semaphores will be used.
	u32int flags;
	disable_and_save_interrupts(flags);

	for( i = 0; i < 1024; i++ )	{
		if( !kprintf_buf[i] ) break;
		kputchar( kprintf_buf[i] );
	}

	restore_interrupts(flags);
	return( i );
}

/**************************************************************************
* Get and return the specified console structure.
* returns NULL if console c does not exists.
**************************************************************************/
console_t *get_console_addr(s32int c) {
	if ( (c >= 0) && ( c <= K_TOT_VIR_CONS) ) {
		return( (console_t *)(&vir_cons[c]) );
	}

	return((console_t *)NULL);
}

s32int get_curr_console() {
	return(current_console_id);
}

