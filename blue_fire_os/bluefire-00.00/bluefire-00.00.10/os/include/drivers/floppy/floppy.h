/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.10
 *	Author: David Davidson
 *	Name: floppy.h
 *	Created: Feb 27, 2012
 *	Last Update: Feb 27, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef FLOPPY_H_
#define FLOPPY_H_


// Drive geometry for 1.44MB floppies
#define FDC_SECTORS	18
#define FDC_HEADS	2
#define FDC_TRACKS	80
#define FDC_SECTOR_SIZE	512

// FDC motor spin up (msec)
#define FDC_TIME_MOTOR_SPINUP	500

// FDC timeout to turn the motor off (msec)
#define FDC_TIME_MOTOR_OFF	3000

// I/O ports
#define FDC_DOR		0x3F2 		// Digital Output Register
#define FDC_MSR		0x3F4 		// Main Status Register (read)
#define FDC_DSR		0x3F4 		// Data Rate Select Register (write)
#define FDC_DATA	0x3F5 		// Data Register
#define FDC_DIR		0x3F7 		// Digital Input Register
#define FDC_CCR		0x3F7 		// Configuration Control Register

// Commands
#define FLOPPY_COMMAND_SPECIFY	0x03
#define FLOPPY_COMMAND_WRITE	0xC5
#define FLOPPY_COMMAND_READ		0xE6
#define FLOPPY_COMMAND_RECAL	0x07
#define FLOPPY_COMMAND_SENSEI	0x08
#define FLOPPY_COMMAND_FORMAT	0x4D
#define FLOPPY_COMMAND_SEEK		0x0F
#define FLOPPY_COMMAND_VERSION	0x10

// Register flags
#define MSR_BUSY	0x10

// Floppy geometry structures
typedef struct floppy_struct {
	u32int 	size, 	// # of sectors total
			spt,	// sectors per track
			heads,	// # of heads
			tracks;	// # of tracks
	u08int	fmtgap,	// gap3 while formatting
			rwgap,	// gap3 while reading/writing
			rate;	// data rate
	s08int	*name;	// format name
} floppy_struct;

#define FDC_BUFFER_SIZE \
	( PAGE_ALIGN_UP(floppy_type[ fdc_geometry ].spt * FDC_SECTOR_SIZE) )

// Public Function declarations
s32int initialize_floppy();
void floppy_thread();
void floppy_handler(irq_context_t *context);
s32int fdc_read(s32int block, u08int *buffer, u32int count);
s32int fdc_write(s32int block, u08int *buffer, u32int count);

#endif /* FLOPPY_H_ */
