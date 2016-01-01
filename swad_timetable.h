// swad_timetable.h: timetables

#ifndef _SWAD_TT
#define _SWAD_TT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type

/*****************************************************************************/
/************************* Public constants and types ************************/
/*****************************************************************************/

#define TT_MAX_BYTES_PLACE	127
#define TT_MAX_BYTES_GROUP	255

#define TT_NUM_TIMETABLE_TYPES 3
typedef enum
  {
   TT_COURSE_TIMETABLE = 0,
   TT_MY_TIMETABLE     = 1,
   TT_TUTOR_TIMETABLE  = 2,
  } TT_TimeTableType_t;

typedef enum
  {
   TT_FREE_HOUR,
   TT_FIRST_HOUR,
   TT_NEXT_HOUR,
  } TT_HourType_t;

#define TT_NUM_CLASS_TYPES 4
typedef enum
  {
   TT_NO_CLASS     = 0,
   TT_THEORY_CLASS = 1,
   TT_PRACT_CLASS  = 2,
   TT_TUTOR_CLASS  = 3,
  } TT_ClassType_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TT_ShowClassTimeTable (void);
void TT_EditCrsTimeTable (void);
void TT_ShowMyTutTimeTable (void);
void TT_ShowTimeTable (TT_TimeTableType_t TimeTableType,long UsrCod);

#endif
