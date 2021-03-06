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

// declared in assembly/start.asm
extern u32int var_system_memory_amount, _start;

/**************************************************************************
* Control arrives here from assembly/start.asm
***************************************************************************/
void k_main() {

	initialize_video();

	initialize_boot_console();

	// Print the welcome banner
	kset_color(DEFAULT_COLOR);
	kprintf("Welcome To ");
	kset_color(LIGHT_BLUE);
	kprintf("Blue Fire OS.\n");
	kset_color(DEFAULT_COLOR);
	kprintf("Kernel is running at virtual address: %#010x\n", (u32int)&_start);
	kprintf("Total System memory is: %d MB\n", (var_system_memory_amount /(1024 * 1024)) );

	dump_dirty_pages();

	initialize_paging();

	initialize_main_console();

	dbg_brk();
	// We must never reach this point.
	PANIC("End of k_main reached.");
}
