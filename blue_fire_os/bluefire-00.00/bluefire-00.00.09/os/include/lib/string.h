/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: string.h
 *	Created: Jan 12, 2012
 *	Last Update: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef STRING_H_
#define STRING_H_


#define STRING_MAX_LENGTH	256
// Public Function declarations
u32int strlen(const s08int *str);
u32int strnlen(const s08int *s, u32int count);
s32int strcmp(const s08int *s1, const s08int *s2);
void strtoupper(s08int *s);
s32int strncmp(const s08int *s1, const s08int *s2, size_t n);
#endif /* STRING_H_ */
