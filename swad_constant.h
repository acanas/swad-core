// swad_constant.h: common constants used in several modules

#ifndef _SWAD_CNS
#define _SWAD_CNS
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*****************************************************************************/
/***************************** Public constants *****************************/
/*****************************************************************************/
/*
   Some special characters, like a chinese character,
   are received from a form in a format like this:
   %26%2335753%3B --> %26 %23 3 5 7 5 3 %3B --> &#35753;
		       ^   ^             ^
		       |   |             |
	      SpecialChar SpecialChar SpecialChar
   Here one chinese character is converted
   to 2 special chars + 5 normal chars + 1 special char,
   and finally is stored as the following 8 bytes: &#35753;

   The maximum UTF-8 code is 1114111 or 0x10FFFF.
   So, when read from form, a character may be read temporarily as %26%231114111%3B (16 bytes)
   Then it may be transformed to &#1114111; (10 bytes)
   So, each char from a form may be transformed finally into a sequence of 1 to 10 bytes,
   but temporarily it may need 16 bytes
*/
#define Cns_MAX_BYTES_PER_CHAR	16	// Maximum number of bytes of a char.
					// Do not change (or change carefully) because it is used to compute size of database fields

#define Cns_MAX_CHARS_IP	(3 + 1 + 3 + 1 + 3 + 1 + 3)	// 15: max. number of chars of an IP address
				// Example: 255.255.255.255
				//           3+1+3+1+3+1+3
				//          123456789012345
#define Cns_MAX_BYTES_IP	Cns_MAX_CHARS_IP	// 15

#define Cns_MAX_CHARS_DATE	(16 - 1)	// 15
#define Cns_MAX_BYTES_DATE	((Cns_MAX_CHARS_DATE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 255

#define Cns_MAX_CHARS_EMAIL_ADDRESS	(256 - 1)			// 255
#define Cns_MAX_BYTES_EMAIL_ADDRESS	Cns_MAX_CHARS_EMAIL_ADDRESS	// 255

#define Cns_MAX_BYTES_USR_LOGIN	Cns_MAX_BYTES_EMAIL_ADDRESS		// 255

#define Cns_MAX_CHARS_SUBJECT	(256 - 1)	// 255
#define Cns_MAX_BYTES_SUBJECT	((Cns_MAX_CHARS_SUBJECT + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 4095

#define Cns_MAX_BYTES_TEXT	( 64 * 1024 - 1)	// Used for medium texts
#define Cns_MAX_BYTES_LONG_TEXT	(256 * 1024 - 1)	// Used for big contents

#define Cns_BYTES_SESSION_ID	Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64

/*
The maximum number of decimal digits d of an integer of b bits
(signed or unsigned) matches the number of digits of the number 2^b.

In the case of signed numbers, an extra character must be added for the sign.
The number of decimal digits of a number X can be calculated as log_10(X), rounded up.

Therefore, log_10(2^b) = b * log_10(2) = b * 0.301029995663981

If s is the size in bytes of a certain type of integer,
given by the sizeof operator, its size b in bits will be b = (s * 8).

The maximum number of decimal digits will be (s * 8) * 0.301029995663981, rounded up.
Rounding up will consist of truncating (converting to an integer), and adding 1.

Unfortunately, the use of floating point may cause problems
when evaluating the expressions as constants. It's necessary to modify them,
for example by multiplying by 2 ^ 11 and dividing by 2 ^ 8,
so that all calculations should be performed by the preprocessor with integers.
*/

#define LOG2_x_2_11 616	// log10(2) * 2^11

#define Cns_MAX_DIGITS_UCHAR		(((sizeof (unsigned char     ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DIGITS_USHORT		(((sizeof (unsigned short    ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DIGITS_UINT		(((sizeof (unsigned int      ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DIGITS_ULONG		(((sizeof (unsigned long     ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DIGITS_ULONGLONG	(((sizeof (unsigned long long) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DIGITS_UINT128		(((sizeof (unsigned __int128 ) * LOG2_x_2_11) >> 8) + 1)

#define Cns_MAX_DIGITS_CHAR		(1 + Cns_MAX_DIGITS_UCHAR    )
#define Cns_MAX_DIGITS_SHORT		(1 + Cns_MAX_DIGITS_USHORT   )
#define Cns_MAX_DIGITS_INT		(1 + Cns_MAX_DIGITS_UINT     )
#define Cns_MAX_DIGITS_LONG		(1 + Cns_MAX_DIGITS_ULONG    )
#define Cns_MAX_DIGITS_LONGLONG		(1 + Cns_MAX_DIGITS_ULONGLONG)
#define Cns_MAX_DIGITS_INT128		(1 + Cns_MAX_DIGITS_UINT128  )

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Cns_QUIET,
   Cns_VERBOSE,
  } Cns_Verbose_t;

#endif
