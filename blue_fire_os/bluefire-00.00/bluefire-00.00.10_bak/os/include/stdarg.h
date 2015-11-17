/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: stdarg.h
 *	Created: Jan 12, 2012
 *	Last Update: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#ifndef STDARG_H_
#define STDARG_H_

typedef char *va_list;

//(X + K-1)/ K * K rounds X up to a whole multiple of K - so if
//sizeof(TYPE) == 6 and sizeof(int) == 4, this becomes 8.
#define va_rounded_size(type) \
(((sizeof (type) + sizeof (s32int) - 1) / sizeof (s32int)) * sizeof (s32int))

#define va_start(ap, v) \
((void) (ap = (va_list) &v + va_rounded_size (v)))

#define va_arg(ap, type) \
(ap += va_rounded_size (type), *((type *)(ap - va_rounded_size (type))))

#define va_end(ap) ((void) (ap = 0))

#endif /* STDARG_H_ */
