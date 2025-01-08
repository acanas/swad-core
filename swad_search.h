// swad_search.c: search for courses and teachers

#ifndef _SWAD_SCH
#define _SWAD_SCH
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_icon.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Sch_MAX_CHARS_STRING_TO_FIND	(128 - 1)	// 127
#define Sch_MAX_BYTES_STRING_TO_FIND	((Sch_MAX_CHARS_STRING_TO_FIND + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Sch_MAX_WORDS_IN_SEARCH		 10

#define Sch_MAX_CHARS_SEARCH_WORD	(128 - 1)	// 127
#define Sch_MAX_BYTES_SEARCH_WORD	((Sch_MAX_CHARS_SEARCH_WORD - 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Sch_MAX_BYTES_SEARCH_QUERY	(Sch_MAX_WORDS_IN_SEARCH * (128 + Sch_MAX_BYTES_SEARCH_WORD))

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Sch_NUM_WHAT_TO_SEARCH	14
typedef enum
  {
   Sch_SEARCH_UNKNOWN			=  0,
   Sch_SEARCH_ALL			=  1,
   Sch_SEARCH_COUNTRIES			=  2,
   Sch_SEARCH_INSTITS			=  3,
   Sch_SEARCH_CENTERS			=  4,
   Sch_SEARCH_DEGREES			=  5,
   Sch_SEARCH_COURSES			=  6,
   Sch_SEARCH_USERS			=  7,
   Sch_SEARCH_TEACHERS			=  8,
   Sch_SEARCH_STUDENTS			=  9,
   Sch_SEARCH_GUESTS			= 10,
   Sch_SEARCH_OPEN_DOCUMENTS		= 11,
   Sch_SEARCH_DOCUM_IN_MY_COURSES	= 12,
   Sch_SEARCH_MY_DOCUMENTS		= 13,
  } Sch_WhatToSearch_t;
#define Sch_WHAT_TO_SEARCH_DEFAULT Sch_SEARCH_ALL

struct Sch_Search
  {
   Sch_WhatToSearch_t WhatToSearch;
   char Str[Sch_MAX_BYTES_STRING_TO_FIND + 1];
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

struct Sch_Search *Sch_GetSearch (void);

void Sch_ReqSysSearch (void);

void Sch_PutFormToSearchInPageTopHeading (void);

void Sch_PutInputStringToSearch (const char *IdInputText);
void Sch_PutMagnifyingGlassButton (Ico_Color_t Color);
void Sch_GetParWhatToSearch (void);
void Sch_GetParsSearch (void);
void Sch_SysSearch (void);

bool Sch_BuildSearchQuery (char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                           const struct Sch_Search *Search,
                           const char *FldName,
                           const char *CharSet,const char *Collate);

void Sch_PutLinkToSearchCoursesPars (__attribute__((unused)) void *Args);

#endif
