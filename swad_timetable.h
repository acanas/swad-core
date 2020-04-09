// swad_timetable.h: timetables

#ifndef _SWAD_TT
#define _SWAD_TT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#define TT_MAX_CHARS_INFO	(128 - 1)	// 127
#define TT_MAX_BYTES_INFO	((TT_MAX_CHARS_INFO + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

typedef enum
  {
   TT_CRS_VIEW,
   TT_TUT_VIEW,
   TT_CRS_EDIT,
   TT_TUT_EDIT,
  } TT_TimeTableView_t;

#define TT_NUM_TIMETABLE_TYPES 3
typedef enum
  {
   TT_COURSE_TIMETABLE   = 0,
   TT_MY_TIMETABLE       = 1,
   TT_TUTORING_TIMETABLE = 2,
  } TT_TimeTableType_t;

typedef enum
  {
   TT_FREE_INTERVAL,
   TT_FIRST_INTERVAL,
   TT_NEXT_INTERVAL,
  } TT_IntervalType_t;

#define TT_NUM_CLASS_TYPES 4
typedef enum
  {
   TT_FREE      = 0,
   TT_LECTURE   = 1,
   TT_PRACTICAL = 2,
   TT_TUTORING  = 3,
  } TT_ClassType_t;

struct TT_Range
  {
   struct
     {
      unsigned Start;		// Class or table starts at this hour
      unsigned End;		// Class or table ends at this hour
     } Hours;
   unsigned MinutesPerInterval;	// 5, 15 or 30 minutes
  };

struct TT_Timetable
  {
   struct
     {
      struct TT_Range Range;
      unsigned HoursPerDay;		// From start hour to end hour
      unsigned SecondsPerInterval;
      unsigned IntervalsPerHour;
      unsigned IntervalsPerDay;
      unsigned IntervalsBeforeStartHour;
     } Config;
   TT_TimeTableType_t Type;
   TT_TimeTableView_t View;
   unsigned Weekday;
   unsigned Interval;
   unsigned Column;
   TT_ClassType_t ClassType;
   unsigned DurationIntervals;
   char Info[TT_MAX_BYTES_INFO + 1];
   long GrpCod;		// Group code (-1 if no group selected)
   struct
     {
      bool PutIconEditCrsTT;
      bool PutIconEditOfficeHours;
      bool PutIconPrint;
     } ContextualIcons;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TT_ShowClassTimeTable (void);
void TT_EditCrsTimeTable (void);
void TT_EditMyTutTimeTable (void);
void TT_ShowTimeTable (long UsrCod);

#endif
