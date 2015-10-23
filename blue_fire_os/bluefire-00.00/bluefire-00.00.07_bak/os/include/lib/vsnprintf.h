/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: vsnprintf.h
 *	Created: Jan 12, 2012
 *	Last Update: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef VSNPRINTF_H_
#define VSNPRINTF_H_

// Public Function declarations
s32int vsnprintf(s08int *buf, u32int size, const s08int *fmt, va_list args);

#endif /* VSNPRINTF_H_ */
