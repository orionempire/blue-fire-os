/******************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: string.h
 *	Created: Oct 28, 2015
 *	Last Update:
 *	Purpose:
 *  Usage:
******************************************************************************/
#ifndef STRING_H_
#define STRING_H_

static __inline__ s08int *strcpy08(s08int *dest, const s08int *org) {
	s08int *temp = dest;

	// Continue until '\0' is copied //
	for (; (*dest = *org); ++dest, ++org);

	return temp;
}
// Copy no more than N characters from a source string to a	destination.
static __inline__ s08int *strncpy08(s08int *dest, const s08int *org, size_t n){
	s08int *d = dest;
	const s08int *s = org;

	if (n != 0) {
		do {
			if ((*d++ = *s++) == 0) {
				while (--n)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return( dest );
}

/******************************************************************************
 *			--------- PUBLIC FUNCTION DECLARATIONS ----------
******************************************************************************/
u32int strlen(const s08int *str);
u32int strnlen(const s08int *s, u32int count);
#endif /* STRING_H_ */
