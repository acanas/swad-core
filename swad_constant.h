// swad_constant.h: common constants used in several modules

#ifndef _SWAD_CNS
#define _SWAD_CNS
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_string.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Cns_MAX_CHARS_WWW	(256 - 1)		// 255: max. number of chars of a URL
#define Cns_MAX_BYTES_WWW	Cns_MAX_CHARS_WWW	// 255

#define Cns_MAX_CHARS_IP	(3 + 1 + 3 + 1 + 3 + 1 + 3)	// 15: max. number of chars of an IP address
				// Example: 255.255.255.255
				//           3+1+3+1+3+1+3
				//          123456789012345
#define Cns_MAX_BYTES_IP	Cns_MAX_CHARS_IP	// 15

#define Cns_MAX_CHARS_DATE	(16 - 1)	// 15
#define Cns_MAX_BYTES_DATE	((Cns_MAX_CHARS_DATE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 255

#define Cns_MAX_CHARS_EMAIL_ADDRESS	(256 - 1)			// 255
#define Cns_MAX_BYTES_EMAIL_ADDRESS	Cns_MAX_CHARS_EMAIL_ADDRESS	// 255

#define Cns_MAX_BYTES_USR_LOGIN	Cns_MAX_BYTES_EMAIL_ADDRESS		// 255

#define Cns_MAX_CHARS_SUBJECT	(256 - 1)	// 255
#define Cns_MAX_BYTES_SUBJECT	((Cns_MAX_CHARS_SUBJECT + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 4095

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

#define Cns_MAX_DECIMAL_DIGITS_UCHAR		(((sizeof (unsigned char     ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DECIMAL_DIGITS_USHORT		(((sizeof (unsigned short    ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DECIMAL_DIGITS_UINT		(((sizeof (unsigned int      ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DECIMAL_DIGITS_ULONG		(((sizeof (unsigned long     ) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DECIMAL_DIGITS_ULONGLONG	(((sizeof (unsigned long long) * LOG2_x_2_11) >> 8) + 1)
#define Cns_MAX_DECIMAL_DIGITS_UINT128		(((sizeof (unsigned __int128 ) * LOG2_x_2_11) >> 8) + 1)

#define Cns_MAX_DECIMAL_DIGITS_CHAR		(1 + Cns_MAX_DECIMAL_DIGITS_UCHAR    )
#define Cns_MAX_DECIMAL_DIGITS_SHORT		(1 + Cns_MAX_DECIMAL_DIGITS_USHORT   )
#define Cns_MAX_DECIMAL_DIGITS_INT		(1 + Cns_MAX_DECIMAL_DIGITS_UINT     )
#define Cns_MAX_DECIMAL_DIGITS_LONG		(1 + Cns_MAX_DECIMAL_DIGITS_ULONG    )
#define Cns_MAX_DECIMAL_DIGITS_LONGLONG		(1 + Cns_MAX_DECIMAL_DIGITS_ULONGLONG)
#define Cns_MAX_DECIMAL_DIGITS_INT128		(1 + Cns_MAX_DECIMAL_DIGITS_UINT128  )

/* Hierarchy names */
#define Cns_HIERARCHY_MAX_CHARS_SHRT_NAME	(32 - 1)	// 31
#define Cns_HIERARCHY_MAX_BYTES_SHRT_NAME	((Cns_HIERARCHY_MAX_CHARS_SHRT_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 511

#define Cns_HIERARCHY_MAX_CHARS_FULL_NAME	(128 - 1)	// 127
#define Cns_HIERARCHY_MAX_BYTES_FULL_NAME	((Cns_HIERARCHY_MAX_CHARS_FULL_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Cns_QUIET,
   Cns_VERBOSE,
  } Cns_QuietOrVerbose_t;

// Related with names of centers, degrees, courses,...
typedef enum
  {
   Cns_SHRT_NAME,
   Cns_FULL_NAME,
  } Cns_ShrtOrFullName_t;

#define Cns_NUM_DISABLED_ENABLED 2
typedef enum
  {
   Cns_DISABLED,
   Cns_ENABLED,
  } Cns_DisabledOrEnabled_t;

#define Cns_NUM_HIDDEN_VISIBLE 2
typedef enum
  {
   Cns_HIDDEN,
   Cns_VISIBLE,
  } Cns_HiddenOrVisible_t;

#endif
