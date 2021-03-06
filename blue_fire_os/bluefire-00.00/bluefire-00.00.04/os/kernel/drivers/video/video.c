/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: video.c
 *	Created: Dec 14, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Default Video width (columns).
u16int	crt_width = 80;
// Default Video height (rows).
u16int	crt_height = 25;

/**************************************************************************
* Refresh cursor register on the video card to the position recorded for the current console.
* x_pos - Row
* y_pos - Column
**************************************************************************/
void video_move_cursor( u08int x_pos, u08int y_pos ) {
	// Registers 14-15 tell the 6845 where to put the
	// cursor. The screen is 80 characters wide, memory is linear.
	u16int cursor_location = ((y_pos * crt_width) + x_pos);
	outport08(VIDEO_CRT_PORT, 0xE);                  	// Tell the VGA controller we are setting the Cursor Location High byte
	outport08(VIDEO_CRT_PORT + 1, cursor_location >> 8); // Set it
	outport08(VIDEO_CRT_PORT, 0xF);                  	// Tell the VGA controller we are setting the Cursor Location Low byte
	outport08(VIDEO_CRT_PORT + 1, cursor_location);      // Set It
}

/**************************************************************************
* Set the current color of the selected console.
* console 	- The address of the selected console.
* attrib 	- The current color code as per console.h.
**************************************************************************/
void video_set_color( console_t *console, u08int attrib ) {
	console->cur_color = attrib;
}

/**************************************************************************
* Write blanks to the entire buffer of the selected console.
* console - The address of the selected console.
**************************************************************************/
void video_clrscr( console_t *console ) {
	u32int flags;

	disable_and_save_interrupts(flags);

	memset16((u16int *)(console->vid_buffer ), BLANK, crt_width*crt_height*2);
	console->cursor_x = 0;
	console->cursor_y = 0;

	// Update the cursor position only for current console.
	if( console==get_console_addr(0) )
		video_move_cursor( console->cursor_x, console->cursor_y);

	restore_interrupts(flags);
}

/**************************************************************************
* scrolls the text on the screen up by one line.
* console - The address of the selected console.
**************************************************************************/
void video_scroll_console(console_t *console) {
	u32int flags;

	disable_and_save_interrupts(flags);

    // Row 25 is the end, this means we need to scroll up
    if((console->cursor_y) >= crt_height)
    {
        //move the last line up
        s32int i;
        for (i = 0*crt_width; i < ((crt_height - 1) * crt_width); i++) {
        	(console->vid_buffer)[i] = (console->vid_buffer)[i+crt_width];
        }

        // The cursor should now be on the last line.
        (console->cursor_y) = (crt_height - 1);

        //blank the new line
        for (i =  0; i < crt_width; i++) {
        	//(console->vid_buffer)[i] = blank;
        	video_place_char(i, console->cursor_y, ' ', console);
        }

    }
    restore_interrupts(flags);
}

/**************************************************************************
* Write ASCII character to a specified position in the selected console.
* x_pos - Row
* y_pos - Column
* char_to_write - ASCII value of character to write
* console - The address of the selected console.
* **************************************************************************/
void video_place_char(u08int x_pos, u08int y_pos, u08int char_to_write, console_t *console) {
	// TODO - Check for x or y out of bounds.
	//convert the current X & Y to a linear value.
	int current_location = ((y_pos * crt_width) + x_pos);

	(console->vid_buffer)[current_location] = (((console->cur_color) << 8) | char_to_write);
}

/**************************************************************************
* Put a character to the next space in the specified console.
// console 	- The address of the selected console.
// c 		- The ASCII code of the character to print.
 * **************************************************************************/
void video_put_char( u08int c, console_t *console) {

	u32int flags;

	disable_and_save_interrupts(flags);

	switch (c)
	{
		case '\n':		//New line
			(console->cursor_y) += 1;	//Move down a line
			(console->cursor_x) = 0; 	//And back to the start of the line
			break;
		case '\r':		// Handle carriage return
			(console->cursor_x) = 0; 	//And back to the start of the line
			break;
		case '\b':		//handle a backspaced request.
			if(console->cursor_x == 0){	// If we are at the beginning of a line
				if(console->cursor_y == 0){	//first check whether we are at the top left of screen
					return;
				}
				//move the cursor to the end of the last line
				(console->cursor_y) -= 1;
				console->cursor_x = crt_width;
			}else{
				//If we are in the middle of a line just move the cursor back
				(console->cursor_x) += 1;
				// and "erase" what is there by over writing it with a space.
				video_place_char(console->cursor_x, console->cursor_y, ' ', console);
			}
			break;
		case '\t':
			// Handle a tab by moving the cursor 8 spaces and then truncating it back to a division of 8
			// a simple example is to snap 13 to 4 spaces we take 13 (1101) and & it with (4-1)->'s complement -> (~3) 1100
			// so 1101 & 1100 = 1100 (12)
			console->cursor_x = (console->cursor_x + 8) & ~(8-1);
			break;
		case '\a': // CTRL+G => system beep! //
			//Implement a system beep
			return;
			break;
		default: 		{
			video_place_char(console->cursor_x, console->cursor_y, c, console);
			(console->cursor_x) += 1;
			break;
		}
	}

	// Check if we need to insert a new line because we have reached the end
	// of the screen.
	if (console->cursor_x >= crt_width)	{
		(console->cursor_x) = 0;
		(console->cursor_y) += 1;
	}
	// Check if cursor is at the bottom of the screen.
	// Scroll the screen if needed.
	video_scroll_console(console);

	// Update the cursor position only for current console.
	if( console==get_console_addr(0) ) {
		video_move_cursor(console->cursor_x, console->cursor_y);
	}


	restore_interrupts(flags);
}

void initialize_video() {
	// Set the size of the screen.
	crt_width = 80;
	crt_height = 25;
}
