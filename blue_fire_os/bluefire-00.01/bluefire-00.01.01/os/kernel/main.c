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

// Declared in sched.c
extern queue_t *ready_queue;

// Declared in sched.c
extern task_t  *current_task;	// Current running task

// Function Declarations
void print_ok();
void print_failed();


/******************************************************************************
 *	---------  ----------
 *	Control arrives here from assembly/start.asm
******************************************************************************/
void k_main() {

	// Paging must be initialized first so that video can use its proper address
	initialize_paging();

	initialize_video();
	// Now debug functions can be used.

	initialize_boot_console();

	// Print the welcome banner
	kset_color(DEFAULT_COLOR);
	kprintf("Welcome To ");
	kset_color(LIGHT_BLUE);
	kprintf("Blue Fire OS ");
	kset_color(DEFAULT_COLOR);
	kprintf("(%s) version %s.\n", KERNEL_NAME, KERNEL_VERSION);
	kprintf("Kernel is running at virtual address: %#010x\n", (u32int)&_start);
	kprintf("Total System memory is: %d MB\n\n", (var_system_memory_amount /(1024 * 1024)) );

	// There are now enough resources available to print to the screen.
	kprintf("Paging and Video initialized successfully...");
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

	// Initialize memory manager.
	kprintf("Initializing memory manager...");
	kmalloc_initialize();
	print_ok();

	kprintf("Initializing Direct Memory Access...");
	initialize_DMA();
	print_ok();

	// Initialize multitasking
	kprintf("Initializing multitasking...");
	initialize_multitasking();
	print_ok();

	// Initialize the clock, will start scheduling
	kprintf("Initializing system clock...");
	initialize_clock();
	print_ok();

	// Initialize the keyboard
	kprintf("Initializing keyboard..");
	initialize_keyboard();
	print_ok();

	/// Initialize the floppy drive
	kprintf("Initializing ");
	kset_color( YELLOW );
	initialize_floppy();
	kset_color( DEFAULT_COLOR );
	kprintf(" floppy controller...");
	print_ok();

	// Create the shell task
	create_thread(&shell, 0, NULL, "BlueShell");

	// Well done!!!
	rem_queue(&ready_queue, current_task);
	do_idle();

	// We must never reach this point.
	PANIC("End of k_main reached.");
}

/******************************************************************************
* Prints [ OK ] in green.
******************************************************************************/
void print_ok() {
	kset_color( DEFAULT_COLOR );
	kprintf( "\r\t\t\t\t\t\t[ " );
	kset_color( GREEN );
	kprintf( "OK" );
	kset_color( DEFAULT_COLOR );
	kprintf( " ]\n" );
}

/**************************************************************************
* Prints [ Failed ] in red.
***************************************************************************/
void  print_failed() {
	kset_color( DEFAULT_COLOR );
	kprintf( "\r\t\t\t\t\t\t[ " );
	kset_color( LIGHT_RED );
	kprintf( "Failed" );
	kset_color( DEFAULT_COLOR );
	kprintf( " ]\n" );
}

