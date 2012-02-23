/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: mem.h
 *	Created: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef MEM_H_
#define MEM_H_

// GDT constants
#define GDT_TABLE_DIMENSION		8192	// number of entries in the GDT
#define GDT_ENTRY_DIMENSION		8		// size of each entry

// Segment Constants
#define PRESENT			0x80
#define CODE			0x18
#define DATA			0x10
#define EXP_DOWN		0x04
#define CONFORMING		0x04
#define READABLE		0x02
#define WRITEABLE		0x02

#define CODE_SEG		(CODE | PRESENT | READABLE)
#define DATA_SEG		(DATA | PRESENT | WRITEABLE)
#define STACK_SEG		DATA_SEG
#define TSS_SEG			(PRESENT | 0x09)
#define REAL_SEG		(CODE | PRESENT | READABLE)

#define DPL_0			0x00
#define DPL_1			0x20
#define DPL_2			0x40
#define DPL_3			0x60

#define BUSY_FLAG		0x02

#define ATTR_32BIT      0x40
#define ATTR_GRANUL     0x80

#define KERNEL_CODE		0x08
#define KERNEL_DATA		0x10
#define KERNEL_STACK	0x10

#define USER_CODE		0x18
#define USER_DATA		0x20
#define USER_STACK		0x20


// GDT structure
typedef struct gdt_entry {
	u16int limit;
	u16int base0_15;
	u08int base16_23;
	u08int attribs0_7;  // P(1bit) | DPL(2bit) | S(1bit) | Type(4bit)
	u08int attribs8_15; // G(1bit) | D/B(1bit) | 0(1bit) | AVL(1bit) | limit16_19(4bit)
	u08int base24_31;
// P=Present, DPL=Descriptor Privilege Level, S=Descriptor type (0=system; 1=code or data),
// Type=Segment type, G=Granularity, D/B=Default operation size(0=16bit; 1=32bit segment),
// AVL=Available for use by system software.
} gdt_entry_t;

typedef struct gdt_register {
	u16int limit;
	u08int base[sizeof(u32int)];
} gdt_reg;

// Update segment registers with kernel selectors
static __inline__ void k_update_segment_regs() {
	__asm__ __volatile__ ("ljmp %0, $update_sel" : : "i" (KERNEL_CODE));

	__asm__ __volatile__ ("update_sel:");

	__asm__ __volatile__ (
		"mov %0, %%ds\n"
		"mov %0, %%es\n"
		"mov %0, %%ss\n" : : "r" (KERNEL_DATA));

	__asm__ __volatile__ ("nop");
	__asm__ __volatile__ (
		"mov %0, %%fs\n"
		"mov %0, %%gs\n" : : "r" (KERNEL_DATA));
}

/**************************************************************************
* Memory manipulation procedures. Inline assembly used, it's less portable, but faster
**************************************************************************/
// sets all of destination specified to val 8 bits at a time
// so memset8(X,0xFF,4) would write 0xFFFFFFFF at X
static __inline__ void memset08(void *dest_ptr, u08int val, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep stosb": :"D"(dest_ptr), "c"(count), "a"(val));
	__asm__("": : :"%edi", "%ecx");
}

// sets all of destination specified to val 16 bits at a time
// so memset16(X,0xFF,4) would write 0x00FF00FF 0x00FF00FF at X
static __inline__ void memset16(void *dest_ptr, u16int val, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep stosw": :"D"(dest_ptr), "c"(count), "a"(val));
	__asm__("": : :"%edi", "%ecx");
}

// sets all of destination specified to val 32 bits at a time
// so memset32(X,0xFF,4) would write 0x000000FF 0x000000FF 0x000000FF 0x000000FF at X
static __inline__ void memset32(void *dest_ptr, u32int val, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep stosl": :"D"(dest_ptr), "c"(count), "a"(val));
	__asm__("": : :"%edi", "%ecx");
}

// Copies count bytes from org_ptr to dest_ptr
static __inline__ void memcpy08(void *dest_ptr, const void *org_ptr, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep movsb": :"D"(dest_ptr), "S"(org_ptr), "c"(count));
	__asm__("": : :"%ecx", "%edi", "%esi");
}

// Copies count u32int from org_ptr to dest_ptr //
static __inline__ void memcpy32(void *dest_ptr, const void *org_ptr, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep movsl": :"D"(dest_ptr), "S"(org_ptr), "c"(count));
	__asm__("": : :"%ecx", "%edi", "%esi");
}

// Public Function declarations
void initialize_GDT();
u16int setup_GDT_entry(u16int limit, u32int base, u08int attribs0_7, u08int attribs8_15);
void remove_GDT_entry(u16int sel);

#endif /* MEM_H_ */
