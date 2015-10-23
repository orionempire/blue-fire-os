/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: clock.c
 *	Created: Feb 2, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include "../../../bluefire-00.00.07_bak/os/include/common_include.h"

// Real time ticks
volatile u32int ticks = 0;

void stop_clock() {
	u32int flags;
	disable_and_save_interrupts(flags);

	// Reset the clock for rebooting
	outport08(TIMER_MODE, SQUARE_WAVE);	// set timer on
	outport08(TIMER0, 0);				// timer low byte
	outport08(TIMER0, 0);				// timer high byte

	restore_interrupts(flags);
}


// Delay some milliseconds
void delay(u32int millisec) {
	u32int flags;
	u32int count, elapsed;

	save_interrupts(flags);

	// Start timer //
	elapsed = 0;
	while (elapsed < millisec) {
		// Poll ticks in memory
		count = ticks;

		// Auto-sleep
		enable_interrupts();
		idle();

		// Update elapsed time
		elapsed += (ticks - count)*1000/HZ;
	}

	restore_interrupts(flags);
}

void clock_handler(irq_context_t *context) {
	// Update ticks
	ticks++;

	// Put here every periodical task...
	//floppy_thread();

	// Call the scheduler.
	scheduler();

	enable_IRQ(context->IRQ);
}


//Must be called after multitasking is enabled as clock_handler(..) calls scheduler(..)
void initialize_clock() {
	u32int flags;
	disable_and_save_interrupts(flags);

	// Initialize channel 0 of the 8253A timer
	//Turn the timer on.
	outport08(TIMER_MODE, SQUARE_WAVE);
	//Set the 8253s low byte
	outport08(TIMER0, TIMER_COUNT & 0xFF);
	//Set the 8253s high byte
	outport08(TIMER0, TIMER_COUNT >> 8);

	register_interrupt_handler(TIMER_IRQ, &clock_handler);

	restore_interrupts(flags);

}
