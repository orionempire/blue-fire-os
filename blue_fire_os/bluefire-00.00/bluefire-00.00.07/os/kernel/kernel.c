/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: kernel.c
 *	Created: Feb 14, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

void error(const char *msg) {
	disable_interrupts();
	//beep();
	kset_color(LIGHT_RED);
	kprintf("\n\rERROR: %s", msg);
	kset_color(DEFAULT_COLOR);
	kprintf("\n\rTask [%s] (%i) killed!", get_pname(), get_pid());

	sched_leave_critical_region();

	if (get_pid()) {
		auto_kill();
	} else {
		halt();
	}
}

// CPU hard stop
void halt() {
	__asm__ __volatile__ ("cli\n hlt");
}

