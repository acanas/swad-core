// swad_FAQ_type.h: definition of types for Frequently Asked Questions

#ifndef _SWAD_FAQ_TYP
#define _SWAD_FAQ_TYP
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
#include "swad_hidden_visible.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define FAQ_MAX_CHARS_QUESTION	(128 - 1)	// 127
#define FAQ_MAX_BYTES_QUESTION	((FAQ_MAX_CHARS_QUESTION + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

struct FAQ_QaA
  {
   char Question[FAQ_MAX_BYTES_QUESTION + 1];
   char Answer[Cns_MAX_BYTES_TEXT + 1];
  };

#endif
