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

void k_main() {
	init_video();

	infinity();
	while(1);
}
