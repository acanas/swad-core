// swad_constant.h: common constants used in several modules

#ifndef _SWAD_CNS
#define _SWAD_CNS
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#define Cns_MAX_CHARS_WWW	(256 - 1)	// Max. number of chars of a URL
#define Cns_MAX_BYTES_WWW	Cns_MAX_CHARS_WWW

#define Cns_MAX_CHARS_IP	(16 - 1)	// Max. number of chars of an IP address
#define Cns_MAX_BYTES_IP	Cns_MAX_CHARS_IP

#define Cns_MAX_CHARS_DATE	(4 + 1 + 2 + 1 + 2)
#define Cns_MAX_BYTES_DATE	Cns_MAX_CHARS_DATE

#define Cns_MAX_CHARS_SUBJECT	(256 - 1)
#define Cns_MAX_BYTES_SUBJECT	(Cns_MAX_CHARS_SUBJECT * Str_MAX_BYTES_PER_CHAR)

#define Cns_MAX_CHARS_SUMMARY_STRING	(256 - 1)
#define Cns_MAX_BYTES_SUMMARY_STRING	(Cns_MAX_CHARS_SUMMARY_STRING * Str_MAX_BYTES_PER_CHAR)

#define Cns_MAX_BYTES_TEXT	( 64 * 1024 - 1)	// Used for medium texts
#define Cns_MAX_BYTES_LONG_TEXT	(256 * 1024 - 1)	// Used for big contents

#define Cns_MAX_CHARS_STRING	(128 - 1)
#define Cns_MAX_BYTES_STRING	(Cns_MAX_CHARS_STRING * Str_MAX_BYTES_PER_CHAR)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Cns_QUIET,
   Cns_VERBOSE,
  } Cns_QuietOrVerbose_t;

// Related with names of centres, degrees, courses,...
typedef enum
  {
   Cns_SHRT_NAME,
   Cns_FULL_NAME,
  } Cns_ShrtOrFullName_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

#endif
