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

/**************************************************************************
* Memory manipulation procedures. Inline assembly used, it's less portable, but faster
**************************************************************************/
// sets all of destination specified to val 8 bits at a time
// so memset8(X,0xFF,4) would write 0xFFFFFFFF at X
void memset8(void *dest_ptr, u08int val, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep stosb": :"D"(dest_ptr), "c"(count), "a"(val));
	__asm__("": : :"%edi", "%ecx");
}

// sets all of destination specified to val 16 bits at a time
// so memset16(X,0xFF,4) would write 0x00FF00FF 0x00FF00FF at X
void memset16(void *dest_ptr, u16int val, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep stosw": :"D"(dest_ptr), "c"(count), "a"(val));
	__asm__("": : :"%edi", "%ecx");
}

// sets all of destination specified to val 32 bits at a time
// so memset32(X,0xFF,4) would write 0x000000FF 0x000000FF 0x000000FF 0x000000FF at X
void memset32(void *dest_ptr, u32int val, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep stosl": :"D"(dest_ptr), "c"(count), "a"(val));
	__asm__("": : :"%edi", "%ecx");
}

// Copies count bytes from org_ptr to dest_ptr
void memcpy8(void *dest_ptr, const void *org_ptr, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep movsb": :"D"(dest_ptr), "S"(org_ptr), "c"(count));
	__asm__("": : :"%ecx", "%edi", "%esi");
}

// Copies count u32int from org_ptr to dest_ptr //
void memcpy32(void *dest_ptr, const void *org_ptr, u32int count) {
	__asm__ __volatile__ (	"cld\n"
				"rep movsl": :"D"(dest_ptr), "S"(org_ptr), "c"(count));
	__asm__("": : :"%ecx", "%edi", "%esi");
}
