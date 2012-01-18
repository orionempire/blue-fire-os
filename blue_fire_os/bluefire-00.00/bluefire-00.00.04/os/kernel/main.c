/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: main.c
 *	Created: Dec 7, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Declared in assembly/test.s
// extern void infinity();

// function is called by address. static __inline__ calls might move the function down breaking things.
void k_main(u32int system_memory_amount) {

	initialize_video();

	initialize_boot_console();

	// Print the welcome banner
	kset_color(DEFAULT_COLOR);
	kprintf("Welcome To ");
	kset_color(LIGHT_BLUE);
	kprintf("Blue Fire OS.\n");
	kset_color(DEFAULT_COLOR);
	kprintf("Kernel is running at virtual address: %#010x\n", (u32int)&k_main );
	kprintf("Total System memory is: %d MB\n", (system_memory_amount /(1024 * 1024)) );

	dbg_brk();
}
