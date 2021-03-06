/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: ctype.h
 *	Created: Jan 12, 2012
 *	Last Update: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef CTYPE_H_
#define CTYPE_H_
/*
 * NOTE! This ctype does not handle EOF like the standard C
 * library is required to.
 */

#define _U	0x01	// upper.
#define _L	0x02	// lower.
#define _D	0x04	// digit.
#define _C	0x08	// cntrl.
#define _P	0x10	// punct.
#define _S	0x20	// white space (space/lf/tab).
#define _X	0x40	// hex digit.
#define _SP	0x80	// hard space (0x20).

// defined in lib/ctype.c
extern u08int _ctype[];

#define __ismask(x) (_ctype[(s32int)(u08int)(x)])

//! Return a value != 0 only if c is an alphanumeric character.
#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
//! Return a value != 0 only if c is an alphabetic character.
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
//! Return a value != 0 only if c is a control character.
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
//! Return a value != 0 only if c is a digit character.
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
//! Return a value != 0 only if c is a printable (not a space)
//! character.
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
//! Return a value != 0 only if c is a lower case character.
#define islower(c)	((__ismask(c)&(_L)) != 0)
//! Return a value != 0 only if c is a printable character.
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
//! Return a value != 0 only if c is a punctuation-mark.
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
//! Return a value != 0 only if c is an empty space
//! like TAB, CR, LF...
#define isspace(c)	((__ismask(c)&(_S)) != 0)
//! Return a value != 0 only if c is an upper case character.
#define isupper(c)	((__ismask(c)&(_U)) != 0)
//! Return a value != 0 only if c is a hexadecimal digit.
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)
//! Return a value != 0 only if c is an ASCII character <= 127.
#define isascii(c) (((u08int)(c))<=0x7f)
//! Return only the ASCII lower 7 bits of the c character.
#define toascii(c) (((u08int)(c))&0x7f)

//Convert a character into the lower case.
//c The character to convert.
//return The lower case of c.
static inline u08int __tolower(u08int c) {
	if (isupper(c)){ c -= ('A'-'a'); }
	return c;
}

// Convert a character into the upper case.
// c The character to convert.
// return The upper case of c.
static inline u08int __toupper(u08int c) {
	if (islower(c)){ c -= ('a'-'A'); }
	return c;
}

// Macro equivalent to __tolower(c).
#define tolower(c) __tolower(c)

// Macro equivalent to __toupper(c).
#define toupper(c) __toupper(c)

#endif /* CTYPE_H_ */
