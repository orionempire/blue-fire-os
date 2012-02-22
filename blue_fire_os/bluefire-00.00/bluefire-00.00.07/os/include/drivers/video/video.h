/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: video.h
 *	Created: Dec 14, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/

#ifndef VIDEO_H_
#define VIDEO_H_

// The video CRT port.
#define VIDEO_CRT_PORT		0x03D4
// Set the first byte to ASCII space 0x20 (32); then move it to the second byte and make the first
// byte the attribute
#define BLANK	(0x20 | (0x0F << 8))

// The physical address of the video memory buffer.
#define VIDEO_MEM_ADDRESS	PHYSICAL(0xB8000)

// Public Function declarations
void initialize_video();
void video_clrscr( console_t *console );
void video_set_color( console_t *console, u08int attrib );
void video_place_char(u08int x_pos, u08int y_pos, u08int char_to_write, console_t *console);
void video_put_char( u08int c, console_t *console );

#endif /* VIDEO_H_ */
