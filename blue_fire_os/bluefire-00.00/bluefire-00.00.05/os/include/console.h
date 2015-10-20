/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: console.h
 *	Created: Jan 11, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef CONSOLE_H_
#define CONSOLE_H_

// Console colors							//
#define BLACK 			0x00 // Black color.
#define BLUE			0x01 // Blue color.
#define GREEN			0x02 // Green color.
#define CYAN			0x03 // Cyan color.
#define RED	 			0x04 // Red color.
#define MAGENTA			0x05 // Magenta color.
#define BROWN	 		0x06 // Brown color.
#define LIGHT_GREY		0x07 // Light grey color.
#define GREY			0x08 // Grey color.
#define LIGHT_BLUE		0x09 // Light blue color.
#define LIGHT_GREEN		0x0A // Light green color.
#define LIGHT_CYAN		0x0B // Light cyan color.
#define LIGHT_RED		0x0C // Light red color.
#define LIGHT_MAGENTA	0x0D // Light magenta color.
#define YELLOW			0x0E // Yellow color.
#define WHITE			0x0F // White color.

// The default color for the text of the console.
#define DEFAULT_COLOR 		0x07

// Number of virtual consoles (must be at least 1).
#define K_TOT_VIR_CONS		2

// Virtual console structure.
typedef struct console
{
	// --- VIDEO ---						//
	u16int 	*vid_buffer;	//Video buffer.
	u08int 	cursor_x;		//Current x position.
	u08int 	cursor_y;		//Current y position.
	u08int	cur_color;		//Current text color.
}console_t;

// Public Function declarations
void initialize_boot_console();
void initialize_main_console();
console_t *get_console_addr(s32int c);
void kset_color(u08int attrib);
s32int kprintf(const s08int *fmt, ...);
s32int kputchar( s32int c );
s32int kplacechar( u08int x_pos, u08int y_pos, s32int c );

#endif /* CONSOLE_H_ */
