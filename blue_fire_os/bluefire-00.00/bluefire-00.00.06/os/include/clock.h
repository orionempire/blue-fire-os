/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: clock.h
 *	Created: Feb 2, 2012
 *	Last Update: Feb 2, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef CLOCK_H_
#define CLOCK_H_

// Clock Constants

#define LATCH_COUNT		0x00	// ...to copy chip count
#define TIMER0			0x40	// I/O port for timer channel 0
#define TIMER2			0x42	// I/O port for timer channel 2
#define TIMER_MODE		0x43	// I/O port for timer mode control
#define SQUARE_WAVE		0x36
#define TIMER_FREQ  	1193182L // Clock frequency for timer in PC

#define HZ		200	// Clock Frequency (User set-able, default=200Hz)

#define TIMER_COUNT (unsigned) (TIMER_FREQ/HZ) // Value to initialize timer


// Public Function declarations
void delay(u32int millisec);
void initialize_clock();
void stop_clock();

#endif /* CLOCK_H_ */
