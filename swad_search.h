// swad_search.c: edition of courses

#ifndef _SWAD_SCH
#define _SWAD_SCH
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#define Sch_MAX_LENGTH_STRING_TO_FIND 255

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Sch_NUM_WHAT_TO_SEARCH	11
typedef enum
  {
   Sch_SEARCH_ALL			=  0,
   Sch_SEARCH_INSTITUTIONS		=  1,
   Sch_SEARCH_CENTRES			=  2,
   Sch_SEARCH_DEGREES			=  3,
   Sch_SEARCH_COURSES			=  4,
   Sch_SEARCH_TEACHERS			=  5,
   Sch_SEARCH_STUDENTS			=  6,
   Sch_SEARCH_GUESTS			=  7,
   Sch_SEARCH_OPEN_DOCUMENTS		=  8,
   Sch_SEARCH_DOCUM_IN_MY_COURSES	=  9,
   Sch_SEARCH_MY_DOCUMENTS		= 10,
  } Sch_WhatToSearch_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Sch_ReqSysSearch (void);
void Sch_ReqCtySearch (void);
void Sch_ReqInsSearch (void);
void Sch_ReqCtrSearch (void);
void Sch_ReqDegSearch (void);
void Sch_ReqCrsSearch (void);
void Sch_PutFormToSearch (const char *Icon,const char *IdInputText);
void Sch_GetParamWhatToSearch (void);
void Sch_GetParamsSearch (void);
void Sch_SysSearch (void);
void Sch_CtySearch (void);
void Sch_InsSearch (void);
void Sch_CtrSearch (void);
void Sch_DegSearch (void);
void Sch_CrsSearch (void);

#endif
