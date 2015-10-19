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


/**************************************************************************
* Memory manipulation procedures. Inline assembly used, it's less portable, but faster
**************************************************************************/
// sets all of destination specified to val 8 bits at a time
// so memset08(X,0xFF,4) would write 0xFFFFFFFF at X
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

#endif /* MEM_H_ */
