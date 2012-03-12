/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.05
 *	Author: David Davidson
 *	Name: main.c
 *	Created: Dec 7, 2011
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// declared in assembly/start.asm
extern u32int var_system_memory_amount, _start;

// Function Declarations
void print_ok();
void print_failed();

/**************************************************************************
*
***************************************************************************/

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
	kprintf("Total System memory is: %d MB\n\n", (var_system_memory_amount /(1024 * 1024)) );

	kprintf("Initializing paging... ");
	initialize_paging();
	print_ok();


	// Reprogram the Programmable Interrupt Controller 8259
	kprintf("Reprogramming PIC 8259...");
	reprogram_PIC_8259();
	print_ok();

	// Install the GDT
	kprintf("Installing kernel GDT...");
	initialize_GDT();
	print_ok();



	// Install the IDT
	kprintf("Installing kernel IDT...");
	initialize_IDT();
	print_ok();


	dbg_brk();
	// We must never reach this point.
	PANIC("End of k_main reached.");
}

/**************************************************************************
* Prints [ OK ] in green.
***************************************************************************/
void print_ok() {
	kset_color( DEFAULT_COLOR );
	kprintf( "\r\t\t\t\t\t\t[ " );
	kset_color( GREEN );
	kprintf( "OK" );
	kset_color( DEFAULT_COLOR );
	kprintf( " ]\n" );
}

/**************************************************************************
* Prints [ Failed ] in green.
***************************************************************************/
void  print_failed() {
	kset_color( DEFAULT_COLOR );
	kprintf( "\r\t\t\t\t\t\t[ " );
	kset_color( LIGHT_RED );
	kprintf( "Failed" );
	kset_color( DEFAULT_COLOR );
	kprintf( " ]\n" );
}
