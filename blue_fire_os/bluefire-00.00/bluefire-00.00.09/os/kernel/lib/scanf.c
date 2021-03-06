/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.00
 *	Author: David Davidson
 *	Name: scanf.c
 *	Created: Feb 23, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

s32int k_getchar(void) {
	s32int ch;

	// Get a valid char & print it on the screen //
	while (!((ch = kgetchar()) & 0xFF));
	return(ch);
}

s32int scanf(const char *format, ...){
	u32int *args = (u32int *)&format;

	while(*format) {
		switch (*format) {
			// Skip spaces
			case ' ':
			case '\f':
			case '\t':
			case '\v':
			case '\n':
			case '\r':
			case '\b':
				++format;
			break;

			case '%':
				format++;
				switch(*format) {
					case 's': {
						s08int *str;
						s32int width;
						s08int ch;

						// Get a string
						for(width=0, str=(s08int *)(*++args);;) {
							ch = k_getchar();

							if (ch == '\r') {
								kputchar('\n');
								kputchar(ch);
								break;
							}
							if (ch == '\t') continue;
							if (ch == 0x1B) continue; // ESC
							if (ch == '\b') {
								if (width) {
									str--; width--;
									kputchar(ch);
									kputchar(' ');
									kputchar(ch);
								}
							} else if (width < STRING_MAX_LENGTH) {
								kputchar(ch);
								*str++=ch;
								width++;
							}
						}
						kputchar(ch);
						*str='\0';
					}
					break;

					case 'i': {
						// Get a integer
						s32int *i=(int *)(*++args);
						s32int width, sign=1;
						s08int ch;

						for(width=0, *i=0;;) {
							ch = k_getchar();
							if (ch == 13) break;

							if ((!width) && (ch=='-')) {
								width++;
								sign = -1;
								kputchar('-');
							}

							if ((!width) && (ch=='+')) {
								width++;
								sign = 1;
								kputchar('+');
							}

							if (ch == '\b') {
								if (width) {
									*i = *i / 10;
									width--;
									kputchar(ch);
									kputchar(' ');
									kputchar(ch);
								}
							}

							if ((ch < '0') || (ch > '9')) {
								continue;
							} else {
								kputchar(ch);
								width++;
								*i = (*i) * 10 + (ch - '0');
							}
						}
						*i = (*i) * sign;
					}
					break;

					default:
					return(-1);
				}
			break;

			default:
				return(-1);
		}
		format++;
	}
	return(0);
}
