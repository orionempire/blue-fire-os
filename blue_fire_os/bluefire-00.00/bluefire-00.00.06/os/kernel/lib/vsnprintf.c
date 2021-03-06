/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: vsnprintf.c
 *	Created: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

#define do_div(n,base) ({ \
	u32int __upper, __low, __high, __mod, __base; \
		__base = (base); \
		asm("":"=a" (__low), "=d" (__high):"A" (n)); \
		__upper = __high; \
		if (__high) { \
			__upper = __high % (__base); \
			__high = __high / (__base); \
		} \
		asm("divl %2":"=a" (__low), "=d" (__mod):"rm" (__base), "0" (__low), "1" (__upper)); \
		asm("":"=A" (n):"a" (__low),"d" (__high)); \
		__mod; \
	})

//Converts character string to integer
static s32int skip_atoi(const s08int **s) {
	s32int i=0;

	while (isdigit(**s)) {
		i = i*10 + *((*s)++) - '0';
	}
	return i;
}


#define ZEROPAD	1		//pad with zero
#define SIGN	2		//unsigned/signed long
#define PLUS	4		//precede with a plus
#define SPACE	8		// space if plus
#define LEFT	16		// left justified
#define SPECIAL	32		// start hexadecimal numbers with 0x
#define LARGE	64		// use capital letters for hexadecimal numbers

/**************************************************************************
* convert a number to its string of ascii characters.
* buff 		- the buffer being written to is expected to be advanced by amt of written characters
* end 		- end of buffer so that we can check for overflows.
* num 		- the number to write
* base 		- the base to use for conversion, 16 for hex, 10 for decimal, 8 for octal
* size 		- minimum width of complete number
* precision - how many decimal places after the decimal point
* type 		- flag field
**************************************************************************/
static s08int * number(s08int * buf, s08int * end, s64int num, s32int base, s32int size, s32int precision, s32int type) {
	s08int c,sign,tmp[66];
	const s08int *digits;
	const s08int small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	const s08int large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	s32int i;

	//Whether to use Large digits for hex variables based on passed flag
	digits = (type & LARGE) ? large_digits : small_digits;
	//
	if (type & LEFT) { type &= ~ZEROPAD; }
	//there are only 36 usable symbols 0-9 and A-Z so we can't handle above that
	if (base < 2 || base > 36) { return 0; }

	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;

	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}

	//If we want to proceed hexadecimal with "0x"
	if (type & SPECIAL) {
		if (base == 16) {
			size -= 2;
		} else if (base == 8) {
			size--;
		}
	}

	i = 0;
	if (num == 0) {
		tmp[i++]='0';
	} else {
		while (num != 0) {
			tmp[i++] = digits[do_div(num,base)];
		}
	}

	if (i > precision) { precision = i; }

	size -= precision;

	if (!(type&(ZEROPAD+LEFT))) {
		while(size-->0) {
			if (buf <= end) { *buf = ' '; }
			++buf;
		}
	}
	if (sign) {
		if (buf <= end) { *buf = sign; }
		++buf;
	}
	if (type & SPECIAL) {
		if (base==8) {
			if (buf <= end) { *buf = '0'; }
			++buf;
		} else if (base==16) {
			if (buf <= end) { *buf = '0'; }
			++buf;
			if (buf <= end) { *buf = digits[33]; }//X or x depending on LARGE
			++buf;
		}
	}
	if (!(type & LEFT)) {
		while (size-- > 0) {
			if (buf <= end) { *buf = c; }
			++buf;
		}
	}
	while (i < precision--) {
		if (buf <= end) { *buf = '0'; }
		++buf;
	}
	while (i-- > 0) {
		if (buf <= end){ *buf = tmp[i]; }
		++buf;
	}
	while (size-- > 0) {
		if (buf <= end){ *buf = ' '; }
		++buf;
	}
	//return the new index into str
	return buf;
}

/**************************************************************************
* takes interpolates a stream of trailing values into string of characters
* containing specifiers in the format of %[flags][width][.precision][length]specifier
* so 		"Hi there number %3d", 6
* becomes 	"Hi there number 006"
* buf 	- output to pass formated string into.
* size 	- size of buf.
* fmt 	- input string specifying format
* args 	- the place holder to what field we are up to
* we ignore a lot of non standard combinations like %p with precision
**************************************************************************/
s32int vsnprintf(s08int *buf, u32int size, const s08int *fmt, va_list args) {
	s32int len;
	u64int num;
	s32int i, base;
	s08int *str, *end, c;
	const s08int *s;

	s32int flags;			//holds the options processed by [flags] as seen below
	s32int field_width;		//holds the options processed by [width] as seen below
	s32int precision;		//holds the options processed by [.precision] as seen below
	s32int qualifier;		//holds the options processed by [length] as seen below

	//string starts out as pointer to the memory location of buffer but, we will advance the pointer
	//as we write to buffer so that we keep track of where we are.
	str = buf;
	end = buf + size - 1;

	//Check for a negative size value.
	if (end < buf - 1) {
		end = ((void *) -1);
		size = end - buf + 1;
	}

	//The format of a specifier is %[flags][width][.precision][length]specifier
	for (; *fmt ; ++fmt) {
		//First print out all characters until we reach the first specifier (starts with %)
		if (*fmt != '%') {
			if (str <= end){ *str = *fmt; }
			++str;
			continue;
		}

		// Process [flags]
		flags = 0;
		repeat:
			++fmt;		// skips the first '%'
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
			}

		// Process [width], minimum field width
		field_width = -1;
		//if there is a integer here it is the width of the field.
		if (isdigit(*fmt)) {
			field_width = skip_atoi(&fmt);

		} else if (*fmt == '*') {
			//if there is a * in this spot instead of a integer
			//the width is specified as an additional integer value
			//argument preceding the argument that has to be formatted.
			++fmt;
			//get the next argument of the format string as the width
			field_width = va_arg(args, s32int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		// process [.precision], the same algorithm as [width] but after a [.]
		precision = -1;
		if (*fmt == '.') {
			++fmt;	//skip the decimal place
			if (isdigit(*fmt)) {
				precision = skip_atoi(&fmt);
			} else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, s32int);
			}
			if (precision < 0){ precision = 0; }
		}

		//Process [length] into the variable qualifier
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt =='Z') {
			qualifier = *fmt;
			++fmt;
			if (qualifier == 'l' && *fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		//default base
		base = 10;

		//process specifier
		switch (*fmt) {
			//c is for character
			case 'c':
				//process the left side of this specifier
				if (!(flags & LEFT)) {
					while (--field_width > 0) {
						if (str <= end){ *str = ' '; }
						++str;
					}
				}
				//get the character

				c = (u08int) va_arg(args, s32int);
				if (str <= end) { *str = c; }
				++str;
				//process the right side of this specifier
				while (--field_width > 0) {
					if (str <= end){ *str = ' '; }
					++str;
				}
				continue;
			//s is for a string, a series of characters
			case 's':
				s = va_arg(args, s08int *);
				if (!s) { s = "<NULL>"; }

				//By default all characters are printed, if precision is specified string is
				//truncated to the first "precision" characters and len is such set.
				//debug -> should this be width?
				len = strnlen(s, precision);

				//process the left side of this specifier
				if (!(flags & LEFT)) {
					while (len < field_width--) {
						if (str <= end){ *str = ' '; }
						++str;
					}
				}
				//stings are easy, just move the characters to the buffer.
				for (i = 0; i < len; ++i) {
					if (str <= end){ *str = *s; }
					++str; ++s;
				}
				//process the right side of this specifier
				while (len < field_width--) {
					if (str <= end){ *str = ' '; }
					++str;
				}
				continue;
			//p is for pointer address, for our implementation same as %d with a default field with.
			case 'p':
				//Set to size of a address for our architecture, unless they are overridden by user.
				if (field_width == -1) {
					field_width = 2*sizeof(void *);
					flags |= ZEROPAD;
				}
				//a hexadecimal number gets passed, convert it to ascii
				str = number(str, end, (u32int) va_arg(args, void *), 16, field_width, precision, flags);
				continue;

			//n stores the amount of characters printed by this call of the function
			//parameter must be a pointer I.E. &test, may cause a triple fault if not used properly in kernel mode
			case 'n':
				//in this case qualifier specifies how many bytes the field written to is.
				if (qualifier == 'l') {
					s32int * ip = va_arg(args, s32int *);
					*ip = (str - buf);
				} else if (qualifier == 'Z') {	// by posix definition z is size_t we will just use u32int to keep things working
					u32int * ip = va_arg(args, u32int *);
					*ip = (str - buf);
				} else {
					s32int * ip = va_arg(args, s32int *);
					*ip = (str - buf);
				}
				continue;

			//A % followed by another %  character will write % to stdout
			//Could be printed by default but technically that is a compiler warning
			case '%':
				if (str <= end){ *str = '%'; }
				++str;
				continue;

			//any thing else is a number, set up the flags and break to qualifier then process by number below.
			case 'o':
				base = 8;
				break;

			case 'X':
				flags |= LARGE;
				base = 16;
				break;
			case 'x':
				base = 16;
				break;

			case 'd':
			case 'i':
				flags |= SIGN;
				break;
			case 'u':
				break;
			//if a unknown specifier is used just print it by default.
			default:
				if (str <= end){ *str = '%'; }
				++str;
				if (*fmt) {
					if (str <= end){ *str = *fmt; }
					++str;
				} else {
					--fmt;
				}
				continue;
		}

		//if we got here it is a number, decide how many bytes we are going to quaff and pass to number.
		if (qualifier == 'L') {
			num = va_arg(args, s64int);
		} else if (qualifier == 'l') {
			num = va_arg(args, u32int);
			if (flags & SIGN){ num = (s32int) num; }
		} else if (qualifier == 'Z') {
			num = va_arg(args, u32int);
		} else if (qualifier == 'h') {
			num = (u16int) va_arg(args, s32int);
			if (flags & SIGN){ num = (s16int) num; }
		} else {
			num = va_arg(args, u32int);
			if (flags & SIGN){ num = (s32int) num; }
		}

		//pass it to number
		str = number(str, end, num, base, field_width, precision, flags);
	}
	if (str <= end) {
		*str = '\0';
	} else if (size > 0) {
		// don't write out a null byte if the buf size is zero
		*end = '\0';
	}
	return str-buf;
}
