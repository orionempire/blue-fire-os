/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: video.h
 *	Created: Dec 14, 2011
 *	Last Update: Dec 14, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/

#ifndef VIDEO_H_
#define VIDEO_H_

// Video Constants
#define BLACK	 		0x00
#define BLUE			0x01
#define GREEN			0x02
#define CYAN			0x03
#define RED	 			0x04
#define MAGENTA			0x05
#define BROWN	 		0x06
#define DEFAULT_COLOR 	0x07
#define LIGHT_GREY		0x07
#define GREY			0x08
#define LIGHT_BLUE		0x09
#define LIGHT_GREEN		0x0A
#define LIGHT_CYAN		0x0B
#define LIGHT_RED		0x0C
#define LIGHT_MAGENTA	0x0D
#define YELLOW			0x0E
#define WHITE			0x0F

#define BLANK	(0x20 | (DEFAULT_COLOR << 8))

#define VIDEO_MEM_ADDRESS	PHYSICAL(0xB8000)

// Public Function declarations
void init_video();

#endif /* VIDEO_H_ */
