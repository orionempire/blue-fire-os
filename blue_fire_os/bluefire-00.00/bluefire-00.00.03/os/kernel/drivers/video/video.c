/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: video.c
 *	Created: Dec 14, 2011
 *	Last Update: Dec 14, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Video global variables
u16int	cur_pos=0;		// cursor position
u08int	cur_color;		// current text color
u16int	crt_port;
u08int	crt_width;
u08int	crt_height;

void init_video()
{
	crt_port=0x3D4;
	crt_width=80;
	crt_height=25;
	cur_color=DEFAULT_COLOR;
	cur_pos=0;

	// Clear the Screen
	//clrscr();
}
