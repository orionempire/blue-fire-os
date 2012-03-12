/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: string.c
 *	Created: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>


/**************************************************************************
* Calculate the length of a non-fixed length string
**************************************************************************/
 u32int strlen(const s08int *str) {
	const s08int *s;
	if (str == 0) return(0);
	for (s = str; *s; ++s);
	return(s - str);
}

/**************************************************************************
* Determine the length of a fixed-size string
* The strnlen function returns the number of characters in the string pointed to by s,
* not including the terminating '\0' character, but at most count.
* In doing this, strnlen looks only at the first count characters at s and never beyond s+count.
**************************************************************************/
u32int strnlen(const s08int *s, u32int count) {
	const s08int *sc;
	if( !s ) return( 0 );
	for ( sc = s; ( count-- ) && ( *sc != '\0' ); ++sc );
	return( sc - s );
}
