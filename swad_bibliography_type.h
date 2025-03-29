// swad_bibliography_type.h: definition of types for course bibliography

#ifndef _SWAD_BIB_TYP
#define _SWAD_BIB_TYP
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_constant.h"
#include "swad_www.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Bib_NUM_FIELDS 6
typedef enum
  {
   Bib_AUTHORS		= 0,
   Bib_TITLE		= 1,
   Bib_SOURCE		= 2,
   Bib_PUBLISHER	= 3,
   Bib_DATE		= 4,
   Bib_ID		= 5,
  } Bib_Field_t;

#define Bib_MAX_CHARS_FIELD	(128 - 1)	// 127
#define Bib_MAX_BYTES_FIELD	((Bib_MAX_CHARS_FIELD + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Bib_BibliographicReference
  {
   char Fields[Bib_NUM_FIELDS][Bib_MAX_BYTES_FIELD + 1];
   char URL[WWW_MAX_BYTES_WWW + 1];		// URL
  };

#endif
