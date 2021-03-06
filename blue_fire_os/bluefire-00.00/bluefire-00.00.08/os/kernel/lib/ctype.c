/**************************************************************************
 *	bluefire-os
 *	Version: 00.00.04
 *	Author: David Davidson
 *	Name: ctype.c
 *	Created: Jan 12, 2012
 *	Purpose:
 *  Usage:
***************************************************************************/
#include <common_include.h>

// _U upper.
// _L lower.
// _D digit.
// _C cntrl.
// _P punct.
// _S white space (space/lf/tab).
// _X hex digit.
// _SP hard space (0x20).

// Character types table.
u08int _ctype[] = {
		_C,		//0 - Null character
		_C,		//1 - Start of Header
		_C,		//2 - Start of Text
		_C,		//3 - End of Text
		_C,		//4 - End of Transmission
		_C,		//5 - Enquiry
		_C,		//6 - Acknowledgment
		_C,		//7 - Bell
		_C,		//8 - Backspace
		_C|_S,	//9 - Horizontal Tab
		_C|_S,	//10 - Line feed
		_C|_S,	//11 - Vertical Tab
		_C|_S,	//12 - Form feed
		_C|_S,	//13 - Carriage return
		_C,		//14 - Shift Out
		_C,		//15 - Shift In
		_C,		//16 - Data Link Escape
		_C,		//17 - Device Control 1 (oft. XON)
		_C,		//18 - Device Control 2
		_C,		//19 - Device Control 3 (oft. XOFF)
		_C,		//20 - Device Control 4
		_C,		//21 - Negative Acknowledgment
		_C,		//22 - Synchronous Idle
		_C,		//23 - End of Transaction. Block
		_C,		//24 - Cancel
		_C,		//25 - End of Medium
		_C,		//26 - Substitute
		_C,		//27 - Escape
		_C,		//28 - File Separator
		_C,		//29 - Group Separator
		_C,		//30 - Record Separator
		_C,		//31 - Unit Separator
		_S|_SP,	//32 - Space character, denotes the space between words, as per the space-bar.
		_P,		//33 - !
		_P,		//34 - "
		_P,		//35 - #
		_P,		//36 - $
		_P,		//37 - %
		_P,		//38 - &
		_P,		//39 - '
		_P,		//40 - (
		_P,		//41 - )
		_P,		//42 - *
		_P,		//43 - +
		_P,		//44 - ,
		_P,		//45 - -
		_P,		//46 - .
		_P,		//47 - /
		_D,		//48 - 0
		_D,		//49 - 1
		_D,		//50 - 2
		_D,		//51 - 3
		_D,		//52 - 4
		_D,		//53 - 5
		_D,		//54 - 6
		_D,		//55 - 7
		_D,		//56 - 8
		_D,		//57 - 9
		_P,		//58 - :
		_P,		//59 - ;
		_P,		//60 - <
		_P,		//61 - =
		_P,		//62 - >
		_P,		//63 - ?
		_P,		//64 - @
		_U|_X,	//65 - A
		_U|_X,  //66 - B
		_U|_X,	//67 - C
		_U|_X,	//68 - D
		_U|_X,	//69 - E
		_U|_X,	//70 - F
		_U,		//71 - G
		_U,		//72 - H
		_U,		//73 - I
		_U,		//74 - J
		_U,		//75 - K
		_U,		//76 - L
		_U,		//77 - M
		_U,		//78 - N
		_U,		//79 - O
		_U,		//80 - P
		_U,		//81 - Q
		_U,		//82 - R
		_U,		//83 - S
		_U,		//84 - T
		_U,		//85 - U
		_U,		//86 - V
		_U,		//87 - W
		_U,		//88 - X
		_U,		//89 - Y
		_U,		//90 - Z
		_P,		//91 - [
		_P,		/*92 - \*/
		_P,		//93 - ]
		_P,		//94 - ^
		_P,		//95 - _
		_P,		//96 - `
		_L|_X,	//97 - a
		_L|_X,	//98 - b
		_L|_X,	//99 - c
		_L|_X,	//100 - d
		_L|_X,	//101 - e
		_L|_X,	//102 - f
		_L,		//103 - g
		_L,		//104 - h
		_L,		//105 - i
		_L,		//106 - j
		_L,		//107 - k
		_L,		//108 - l
		_L,		//109 - m
		_L,		//110 - n
		_L,		//111 - o
		_L,		//112 - p
		_L,		//113 - q
		_L,		//114 - r
		_L,		//115 - s
		_L,		//116 - t
		_L,		//117 - u
		_L,		//118 - v
		_L,		//119 - w
		_L,		//120 - x
		_L,		//121 - y
		_L,		//122 - z
		_P,		//123 - {
		_P,		//124 - |
		_P,		//125 - }
		_P,		//126 - ~
		_C,		//127 -  DEL    (delete)
		/*The extended ASCII codes (character code 128-255)
		There are several different variations of the 8-bit ASCII table.
		The table below is according to ISO 8859-1, also called ISO Latin-1.*/
		/* We don't classify graphical characters 128 - 159 */
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 128-143 */
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 144-159 */
		_S|_SP,	//160 - Non-breaking space
		_P,		//161 - � Inverted exclamation mark
		_P,		//162 - � Cent sign
		_P,		//163 - � Pound sign
		_P,		//164 - � Currency sign
		_P,		//165 - � Yen sign
		_P,		//166 - � Pipe, Broken vertical bar
		_P,		//167 - � Section sign
		_P,		//168 - � Spacing diaeresis - umlaut
		_P,		//169 - � Copyright sign
		_P,		//170 - � Feminine ordinal indicator
		_P,		//171 -	� Left double angle quotes
		_P,		//172 -	� Not sign
		_P,		//173 -	� Soft hyphen
		_P,		//174 -	� Registered trade mark sign
		_P,		//175 -	� Spacing macron - overline
		_P,		//176 -	� Degree sign
		_P,		//177 -	� Plus-or-minus sign
		_P,		//178 -	� Superscript two - squared
		_P,		//179 -	� Superscript three - cubed
		_P,		//180 -	� Acute accent - spacing acute
		_P,		//181 -	� Micro sign
		_P,		//182 -	� Pilcrow sign - paragraph sign
		_P,		//183 -	� Middle dot - Georgian comma
		_P,		//184 -	� Spacing cedilla
		_P,		//185 -	� Superscript one
		_P,		//186 -	� Masculine ordinal indicator
		_P,		//187 -	� Right double angle quotes
		_P,		//188 -	� Fraction one quarter
		_P,		//189 -	� Fraction one half
		_P,		//190 -	� Fraction three quarters
		_P,		//191 -	� Inverted question mark
		_U,		//192 -	� Latin capital letter A with grave
		_U,		//193 -	� Latin capital letter A with acute
		_U,		//194 -	� Latin capital letter A with circumflex
		_U,		//195 -	� Latin capital letter A with tilde
		_U,		//196 -	� Latin capital letter A with diaeresis
		_U,		//197 -	� Latin capital letter A with ring above
		_U,		//198 -	� Latin capital letter AE
		_U,		//199 -	� Latin capital letter C with cedilla
		_U,		//200 -	� Latin capital letter E with grave
		_U,		//201 -	� Latin capital letter E with acute
		_U,		//202 -	� Latin capital letter E with circumflex
		_U,		//203 -	� Latin capital letter E with diaeresis
		_U,		//204 -	� Latin capital letter I with grave
		_U,		//205 -	� Latin capital letter I with acute
		_U,		//206 -	� Latin capital letter I with circumflex
		_U,		//207 -	� Latin capital letter I with diaeresis
		_U,		//208 -	� Latin capital letter ETH
		_U,		//209 -	� Latin capital letter N with tilde
		_U,		//210 -	� Latin capital letter O with grave
		_U,		//211 -	� Latin capital letter O with acute
		_U,		//212 -	� Latin capital letter O with circumflex
		_U,		//213 -	� Latin capital letter O with tilde
		_U,		//214 -	� Latin capital letter O with diaeresis
		_U,		//215 -	� Multiplication sign
		_U,		//216 -	� Latin capital letter O with slash
		_U,		//217 -	� Latin capital letter U with grave
		_U,		//218 -	� Latin capital letter U with acute
		_U,		//219 -	� Latin capital letter U with circumflex
		_U,		//220 -	� Latin capital letter U with diaeresis
		_U,		//221 -	� Latin capital letter Y with acute
		_U,		//222 -	� Latin capital letter THORN
		_L,		//223 -	� Latin small letter sharp s - ess-zed
		_L,		//224 -	� Latin small letter a with grave
		_L,		//225 -	� Latin small letter a with acute
		_L,		//226 -	� Latin small letter a with circumflex
		_L,		//227 -	� Latin small letter a with tilde
		_L,		//228 -	� Latin small letter a with diaeresis
		_L,		//229 -	� Latin small letter a with ring above
		_L,		//230 -	� Latin small letter ae
		_L,		//231 -	� Latin small letter c with cedilla
		_L,		//232 -	� Latin small letter e with grave
		_L,		//233 -	� Latin small letter e with acute
		_L,		//234 -	� Latin small letter e with circumflex
		_L,		//235 -	� Latin small letter e with diaeresis
		_L,		//236 -	� Latin small letter i with grave
		_L,		//237 -	� Latin small letter i with acute
		_L,		//238 -	� Latin small letter i with circumflex
		_L,		//239 -	� Latin small letter i with diaeresis
		_L,		//240 -	� Latin small letter eth
		_L,		//241 -	� Latin small letter n with tilde
		_L,		//242 -	� Latin small letter o with grave
		_L,		//243 -	� Latin small letter o with acute
		_L,		//244 -	� Latin small letter o with circumflex
		_L,		//245 -	� Latin small letter o with tilde
		_L,		//246 -	� Latin small letter o with diaeresis
		_P,		//247 -	� Division sign
		_L,		//248 -	� Latin small letter o with slash
		_L,		//249 -	� Latin small letter u with grave
		_L,		//250 -	� Latin small letter u with acute
		_L,		//251 -	� Latin small letter u with circumflex
		_L,		//252 -	� Latin small letter u with diaeresis
		_L,		//253 -	� Latin small letter y with acute
		_L,		//254 -	� Latin small letter thorn
		_L,		//255 -	� Latin small letter y with diaeresis
};
