// swad_link_type.h: definition of types for course links

#ifndef _SWAD_LNK_TYP
#define _SWAD_LNK_TYP
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#define Lnk_NUM_FIELDS 2
typedef enum
  {
   Lnk_TITLE		= 0,
   Lnk_DESCRIPTION	= 1,
  } Lnk_Field_t;

#define Lnk_MAX_CHARS_FIELD	(128 - 1)	// 127
#define Lnk_MAX_BYTES_FIELD	((Lnk_MAX_CHARS_FIELD + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Lnk_Link
  {
   char Fields[Lnk_NUM_FIELDS][Lnk_MAX_BYTES_FIELD + 1];
   char WWW[WWW_MAX_BYTES_WWW + 1];
  };

#endif
