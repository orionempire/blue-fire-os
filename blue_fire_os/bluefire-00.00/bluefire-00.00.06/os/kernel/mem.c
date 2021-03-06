 /**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: mem.c
 *	Created: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// Global Descriptor Table (GDT) (volatile ?)
// The GDT register will point here
volatile gdt_entry_t gdt[GDT_TABLE_DIMENSION];
// GDT pointer
gdt_reg gdt_ptr;
// Busy (used) entries in GDT
u08int gdt_busy_entry[GDT_TABLE_DIMENSION];


u16int setup_GDT_entry(u16int limit, u32int base, u08int attribs0_7, u08int attribs8_15){
	u32int flags;

	register u16int i;

	for (i=0; (gdt_busy_entry[i]); i++) {
		if (i == GDT_TABLE_DIMENSION) {
			//error("Out of GDT entries!!!"); // will be a handled error later
			PANIC("Out of GDT entries!!!"); 	// but a panic for now
		}
	}

	disable_and_save_interrupts(flags);

	gdt[i].limit = limit;
	gdt[i].base0_15 = base & 0xFFFF;
	gdt[i].base16_23 = (base >> 16) & 0xFF;
	gdt[i].base24_31 = (base >> 24) & 0xFF;
	gdt[i].attribs0_7 = attribs0_7;
	gdt[i].attribs8_15 = attribs8_15;

	gdt_busy_entry[i] = 1;

	restore_interrupts(flags);

	// Return the selector //
	return(i * GDT_ENTRY_DIMENSION);
}


void remove_GDT_entry(u16int sel){
	u32int flags;

	disable_and_save_interrupts(flags);

	gdt[sel/GDT_ENTRY_DIMENSION].limit = 0;
	gdt[sel/GDT_ENTRY_DIMENSION].base0_15 = 0;
	gdt[sel/GDT_ENTRY_DIMENSION].base16_23 = 0;
	gdt[sel/GDT_ENTRY_DIMENSION].base24_31 = 0;
	gdt[sel/GDT_ENTRY_DIMENSION].attribs0_7 = 0;
	gdt[sel/GDT_ENTRY_DIMENSION].attribs8_15 = 0;

	gdt_busy_entry[sel/GDT_ENTRY_DIMENSION] = 0;

	restore_interrupts(flags);
}

void initialize_GDT() {
	register u16int i;

	// Reset the GDT
	for (i=0; i < GDT_TABLE_DIMENSION; i+=GDT_ENTRY_DIMENSION) {
		remove_GDT_entry(i);
	}

	// ------------- Set the GDT Reserved entries ------------------
	// Dummy descriptor//
	setup_GDT_entry(0, 0, 0, 0);
	// KERNEL_CODE descriptor 0x08
	setup_GDT_entry(0xFFFF, 0, CODE_SEG, (ATTR_GRANUL | ATTR_32BIT | 0xF));
	// KERNEL_DATA descriptor 0x10
	setup_GDT_entry(0xFFFF, 0, DATA_SEG, (ATTR_GRANUL | ATTR_32BIT | 0xF));
	// USER_CODE descriptor 0x18
	setup_GDT_entry(0xFFFF, 0, (CODE_SEG | DPL_3), (ATTR_GRANUL | ATTR_32BIT | 0xF));
	// USER_DATA descriptor 0x20
	setup_GDT_entry(0xFFFF, 0, (DATA_SEG | DPL_3), (ATTR_GRANUL | ATTR_32BIT | 0xF));


	// --------- End of reserved entries in the GDT ---------------

	// Set up the GDT pointer
	//Total GDT ((8192 * 8) - 1 = 0xFFFF)
	gdt_ptr.limit = ((GDT_TABLE_DIMENSION * GDT_ENTRY_DIMENSION) - 1);
	* (u32int *)gdt_ptr.base = ((u32int)&gdt);

	// Load info into GDTR
	__asm__ __volatile__ ("lgdtl (%0)" : : "r"((u32int)&gdt_ptr));

	// Update segment registers //
	k_update_segment_regs();
}



