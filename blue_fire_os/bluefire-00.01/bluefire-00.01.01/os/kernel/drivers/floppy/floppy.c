/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.10
 *	Author: David Davidson
 *	Name: floppy.c
 *	Created: Feb 27, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Motor on flag
volatile s32int fdc_motor = FALSE;

// FDC timeout
volatile s32int fdc_timeout = 0;

// Floppy motor kill countdown
volatile s32int fdc_motor_countdown = 0;


// FDC operation finish
volatile s32int fdc_done = FALSE;

// Disk change flag
volatile s32int fdc_change = FALSE;

// FDC status (result output)
volatile u08int fdc_status[7] = { 0 };

// Current head position
volatile u08int fdc_track = 0xFF;

// Status register 0
volatile u08int status_register_0 = 0;

// Floppy types known from the system
static floppy_struct floppy_type[] = {
{ 2880, 18, 2, 80, 0x54, 0x1B, 0x00, "H1440" },	// 1.44MB
{ 3360, 21, 2, 80, 0x0C, 0x1C, 0x00, "H1680" }	// 1.68MB
};

// Current floppy geometry
u08int fdc_geometry = 0;

// Floppy r/w buffer
u08int *fdc_buffer;

// ---------- Floppy functions ----------
void fdc_sendbyte(u08int b) {
// Send a byte to the FDC controller
	s32int msr;
	s32int tmo;

	for (tmo=0; tmo<128; tmo++)	{
		msr = inport08(FDC_MSR);
		if ((msr & 0xC0) == 0x80) {
			outport08(FDC_DATA, b);
			return;
		}
		inport08(0x80); //delay
	}
}

int fdc_getbyte() {
// Get a byte from the FDC controller
	s32int msr;
	s32int tmo;

	for (tmo=0; tmo<128; tmo++)	{
		msr = inport08(FDC_MSR);
		if ((msr & 0xD0) == 0xD0) {
			return(inport08(FDC_DATA));
		}
		inport08(0x80); //delay
	}
	return(-1); // timeout
}

// Turns the floppy motor on
void fdc_motor_on() {
	if (!fdc_motor)	{
		outport08(FDC_DOR, 0x1C);
		delay(FDC_TIME_MOTOR_SPINUP);
		fdc_motor = TRUE;
	}
	fdc_motor_countdown = -1;
}

// Starts the FDC motor kill count down
void fdc_motor_off() {
	if (fdc_motor && (fdc_motor_countdown==-1)) {
		fdc_motor_countdown =  ((FDC_TIME_MOTOR_OFF / 1000 ) * HZ);
	}
}

s32int fdc_wait(s32int sensei) {
	s32int i;
	fdc_timeout = HZ; // timeout = 2 sec

	// Wait for IRQ or timeout
	while((!fdc_done) && (fdc_timeout)) {
		enable_interrupts();
		idle();
	}

	// Read command result bytes
	i = 0;
	while ( (i < 7) && (inport08(FDC_MSR) & MSR_BUSY)) {
		fdc_status[i++] = fdc_getbyte();
	}

	// Send a "sense interrupt status" command
	if (sensei) {
		fdc_sendbyte(FLOPPY_COMMAND_SENSEI);
		status_register_0 = fdc_getbyte();
		fdc_track = fdc_getbyte();
	}

	// Timed out?
	if (!fdc_done) {
		// Check for disk change
		if (inport08(FDC_DIR) & 0x80) {
			fdc_change = TRUE;
		}
		return(FALSE);
	} else {
		fdc_done = FALSE;
		return(TRUE);
	}
}

// Recalibrate the drive
void fdc_recalibrate() {
	// Turn the motor on
	fdc_motor_on();

	// Send re-calibrate command
	fdc_sendbyte(FLOPPY_COMMAND_RECAL);
	fdc_sendbyte(0);

	// Wait until re-calibrate command is finished
	fdc_wait(TRUE);

	// Turn the motor off
	fdc_motor_off();
}

// Seek the track
s32int fdc_seek(s32int track) {
	// If already there return
	if (fdc_track == track)
		return(TRUE);
	// Turn the motor on
	fdc_motor_on();
	// Send seek command
	fdc_sendbyte(FLOPPY_COMMAND_SEEK);
	fdc_sendbyte(0);
	fdc_sendbyte(track);
	// Wait until seek is finished
	if (!fdc_wait(TRUE)) {
		// Timeout!
		fdc_motor_off();
		return(FALSE);
	}

	// Let the head settle for 15msec
	delay(15);

	// Turn off the motor
	fdc_motor_off();

	// Check if seek worked
	if ((status_register_0 != 0x20) || (fdc_track != track))
		return(FALSE);
	else
		return(TRUE);
}

void fdc_reset() {
	// Stop the motor and disable IRQ
	outport08(FDC_DOR, 0x00);
	// Program data rate (500K/s)
	outport08(FDC_DSR, 0x00);

	// re-enable floppy IRQ
	outport08(FDC_DOR, 0x0C);
	// Wait for fdc
	fdc_done = TRUE;
	fdc_wait(TRUE);

	// Specify drive timings //
	fdc_sendbyte(FLOPPY_COMMAND_SPECIFY);
	fdc_sendbyte(0xDF); // SRT = 3ms, HUT = 240ms
	fdc_sendbyte(0x02); // HLT = 4ms, ND = 0 (DMA mode selected)

	// Clear disk change flag
	fdc_seek(1);

	fdc_recalibrate();
	fdc_change = FALSE;
}

void lba2chs(s32int lba, s32int *track, s32int *head, s32int *sector) {
	*track = lba / (floppy_type[fdc_geometry].spt * floppy_type[fdc_geometry].heads);
	*head = (lba / floppy_type[fdc_geometry].spt) % floppy_type[fdc_geometry].heads;
	*sector = (lba % floppy_type[fdc_geometry].spt) + 1;
}

s32int fdc_rw(s32int logical_block, u08int *buffer, s32int do_read) {

	s32int track, head, sector, tries;

	// Translate the logical address into CHS address
	lba2chs(logical_block, &track, &head, &sector);

	// Spin up the motor
	fdc_motor_on();

	// Write operation => copy data from buffer into fdc_buffer
	if (!do_read) {
		memcpy08(fdc_buffer, buffer, FDC_SECTOR_SIZE);
	}

	for(tries=0; tries<3; tries++) {
		// Check for disk changed
		if (inport08(FDC_DIR) & 0x80) {
			fdc_change = TRUE;
			fdc_seek(1);
			fdc_recalibrate();
			fdc_motor_off();
			return(FALSE);
		}

		// Move the head to the right track
		if (!fdc_seek(track)) {
			fdc_motor_off();
			return(FALSE);
		}

		// Program data rate
		outport08(FDC_CCR, floppy_type[fdc_geometry].rate);

		// Send command
		if (do_read) {
			// Read operation
			dma_xfer(2, (u32int)fdc_buffer, FDC_SECTOR_SIZE, FALSE);
			fdc_sendbyte(FLOPPY_COMMAND_READ);
		} else {
			// Write operation //
			dma_xfer(2, (u32int)fdc_buffer, FDC_SECTOR_SIZE, TRUE);
			fdc_sendbyte(FLOPPY_COMMAND_WRITE);
		}
		fdc_sendbyte(head << 2);
		fdc_sendbyte(track);
		fdc_sendbyte(head);
		fdc_sendbyte(sector);
		fdc_sendbyte(2); // 512 bytes/sector
		fdc_sendbyte(floppy_type[fdc_geometry].spt);
		fdc_sendbyte(floppy_type[fdc_geometry].rwgap);
		fdc_sendbyte(0xFF); // DTL = unused

		// Wait for command completion
		if (!fdc_wait(FALSE)) {
			return(FALSE); // Timed out!
		}

		if ((fdc_status[0] & 0xC0) == 0) {
			break; // It worked
		}

		// Try again
		fdc_recalibrate();
	}

	// Stop the motor
	fdc_motor_off();

	if (do_read)
		// Read operation => copy data from fdc_buffer into buffer
		memcpy08(buffer, fdc_buffer, FDC_SECTOR_SIZE);

	// Return if the R/W operation terminates successfully
	return(tries != 3);
}

// Read <count> block
s32int fdc_read(s32int block, u08int *buffer, u32int count) {
	register s32int i;

	for( i = 0; i < count; i++) {
		if (!(fdc_rw(block, buffer + FDC_SECTOR_SIZE * i, TRUE))) {
			return(FALSE);
		}
	}
	// Read operation OK!
	return(TRUE);
}

// Write <count> block
s32int fdc_write(s32int block, u08int *buffer, u32int count) {

	register s32int i;

	for(i = 0; i < count; i++) {
		if (!(fdc_rw(block, buffer + FDC_SECTOR_SIZE * i, FALSE))) {
			return(FALSE);
		}
	}
	// Write operation OK!
	return(TRUE);
}
// Floppy disk interrupt handler
void floppy_handler(irq_context_t *context) {
	fdc_done = TRUE;
	enable_IRQ(context->IRQ);
}


// Called from clock_thread() at every clock tick
void floppy_thread() {

	if (fdc_timeout > 0) {
		fdc_timeout--;
	}

	if (fdc_motor_countdown > 0) {
		fdc_motor_countdown--;
	} else if (fdc_motor && !fdc_motor_countdown) {
		outport08(FDC_DOR, 0x0C);
		fdc_motor = FALSE;
	}
}



// Initialize the floppy driver
s32int initialize_floppy() {
	s32int v;

	//TODO DOWN( &fdc_mutex );
	// Install the interrupt handler routine
	register_interrupt_handler( FLOPPY_IRQ, (void *)floppy_handler );

	// Reset the controller
	fdc_reset();

	// Get floppy controller version
	fdc_sendbyte(FLOPPY_COMMAND_VERSION);
	v = fdc_getbyte();

	if (v==0xFF) {
		kprintf("%s(): No floppy controller [%#04x].\n", __FUNCTION__, v );
		//TODO UP( &fdc_mutex );
		return( FALSE );
	}

	if (v == 0x90) {
		kprintf("Enhanced");
	} else {
		kprintf("%s(): unknown controller [%#04x].\n", __FUNCTION__, v );
	}
	//TODO DOWN( &fdc_mutex );

	// Create the floppy buffer for low-level read/write
	// operations.
	fdc_buffer = PHYSICAL( dma_phys_alloc(FDC_BUFFER_SIZE) );
	if( fdc_buffer == NULL ) {
		//TODO UP( &fdc_mutex );

		kprintf("%s(): out of dma memory!\n", __FUNCTION__ );

		return( FALSE );
	}

	//TODO UP( &fdc_mutex );

	//kprintf("%s(): floppy buffer at [%p]->[%p]\n",__FUNCTION__,fdc_buffer, (size_t)fdc_buffer + PAGE_SIZE );

	return( TRUE );
}
