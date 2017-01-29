// swad_search.c: edition of courses

#ifndef _SWAD_SCH
#define _SWAD_SCH
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Sch_MAX_LENGTH_STRING_TO_FIND	255
#define Sch_MAX_WORDS_IN_SEARCH		 10
#define Sch_MAX_LENGTH_SEARCH_WORD	255
#define Sch_MAX_LENGTH_SEARCH_QUERY	(Sch_MAX_WORDS_IN_SEARCH*Sch_MAX_LENGTH_SEARCH_WORD)

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Sch_NUM_WHAT_TO_SEARCH	13
typedef enum
  {
   Sch_SEARCH_UNKNOWN			=  0,
   Sch_SEARCH_ALL			=  1,
   Sch_SEARCH_INSTITS			=  2,
   Sch_SEARCH_CENTRES			=  3,
   Sch_SEARCH_DEGREES			=  4,
   Sch_SEARCH_COURSES			=  5,
   Sch_SEARCH_USERS			=  6,
   Sch_SEARCH_TEACHERS			=  7,
   Sch_SEARCH_STUDENTS			=  8,
   Sch_SEARCH_GUESTS			=  9,
   Sch_SEARCH_OPEN_DOCUMENTS		= 10,
   Sch_SEARCH_DOCUM_IN_MY_COURSES	= 11,
   Sch_SEARCH_MY_DOCUMENTS		= 12,
  } Sch_WhatToSearch_t;
#define Sch_WHAT_TO_SEARCH_DEFAULT Sch_SEARCH_ALL

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Sch_ReqSysSearch (void);
void Sch_ReqCtySearch (void);
void Sch_ReqInsSearch (void);
void Sch_ReqCtrSearch (void);
void Sch_ReqDegSearch (void);
void Sch_ReqCrsSearch (void);

void Sch_PutFormToSearchInPageTopHeading (void);

void Sch_PutInputStringToSearch (const char *IdInputText);
void Sch_PutMagnifyingGlassButton (const char *Icon);
void Sch_GetParamWhatToSearch (void);
void Sch_GetParamsSearch (void);
void Sch_SysSearch (void);
void Sch_CtySearch (void);
void Sch_InsSearch (void);
void Sch_CtrSearch (void);
void Sch_DegSearch (void);
void Sch_CrsSearch (void);

bool Sch_BuildSearchQuery (char SearchQuery[Sch_MAX_LENGTH_SEARCH_QUERY + 1],
                           const char *FieldName,
                           const char *CharSet,const char *Collate);

#endif
