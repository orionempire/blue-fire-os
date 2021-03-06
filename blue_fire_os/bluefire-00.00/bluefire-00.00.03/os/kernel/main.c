/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.03
 *	Author: David Davidson
 *	Name: main.c
 *	Created: Dec 7, 2011
 *	Last Update: Dec 7, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Declared in assembly/test.s
extern void infinity();

void k_main(int system_memory_amount) {
	init_video();

	int var = 0;
	volatile char *video=(volatile char*)0xB8000;
	const char *string = "Hello from kernel land!";

	//Clear screen
	for (var = 0; var < 4000; var++){
		*video=0;
		video++;
	}

	// Print message in windows 95 nostalgic white on blue. Can't handle spaces.
	video=(volatile char*)0xB8000;
	while(*string!=0)
	{
		*video=*string;
		string++;
		video++;
		*video=0x1F;
		video++;
	}

	infinity();
}
