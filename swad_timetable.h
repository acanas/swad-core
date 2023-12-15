// swad_timetable.h: timetables

#ifndef _SWAD_TMT
#define _SWAD_TMT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type

/*****************************************************************************/
/************************* Public constants and types ************************/
/*****************************************************************************/

#define Tmt_MAX_CHARS_INFO	(128 - 1)	// 127
#define Tmt_MAX_BYTES_INFO	((Tmt_MAX_CHARS_INFO + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Tmt_NUM_TIMETABLE_TYPES 3
typedef enum
  {
   Tmt_COURSE_TIMETABLE   = 0,
   Tmt_MY_TIMETABLE       = 1,
   Tmt_TUTORING_TIMETABLE = 2,
  } Tmt_TimeTableType_t;

typedef enum
  {
   Tmt_FREE_INTERVAL,
   Tmt_FIRST_INTERVAL,
   Tmt_NEXT_INTERVAL,
  } Tmt_IntervalType_t;

#define Tmt_NUM_CLASS_TYPES 4
typedef enum
  {
   Tmt_FREE      = 0,
   Tmt_LECTURE   = 1,
   Tmt_PRACTICAL = 2,
   Tmt_TUTORING  = 3,
  } Tmt_ClassType_t;

struct Tmt_Range
  {
   struct
     {
      unsigned Start;		// Class or table starts at this hour
      unsigned End;		// Class or table ends at this hour
     } Hours;
   unsigned MinutesPerInterval;	// 5, 15 or 30 minutes
  };

struct Tmt_WhichCell
  {
   unsigned Weekday;
   unsigned Interval;
   unsigned Column;
  };

struct Tmt_Timetable
  {
   struct
     {
      struct Tmt_Range Range;
      unsigned HoursPerDay;		// From start hour to end hour
      unsigned SecondsPerInterval;
      unsigned IntervalsPerHour;
      unsigned IntervalsPerDay;
      unsigned IntervalsBeforeStartHour;
     } Config;
   Tmt_TimeTableType_t Type;
   Vie_ViewType_t View;
   struct Tmt_WhichCell WhichCell;
   Tmt_ClassType_t ClassType;
   unsigned DurationIntervals;
   char Info[Tmt_MAX_BYTES_INFO + 1];
   long GrpCod;		// Group code (-1 if no group selected)
   struct
     {
      bool PutIconEditCrsTT;
      bool PutIconEditOfficeHours;
      bool PutIconPrint;
     } ContextualIcons;
  };

struct Tmt_Column
  {
   long CrsCod;		// Course code (-1 if no course selected)
   long GrpCod;		// Group code (-1 if no group selected)
   Tmt_IntervalType_t IntervalType;
   Tmt_ClassType_t ClassType;
   unsigned DurationIntervals;
   char Info[Tmt_MAX_BYTES_INFO + 1];
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Tmt_ShowClassTimeTable (void);
void Tmt_EditCrsTimeTable (void);
void Tmt_EditMyTutTimeTable (void);
void Tmt_ShowTimeTable (struct Tmt_Timetable *Timetable,long UsrCod);

#endif
