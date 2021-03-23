// swad_timetable.c: timetables

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For malloc, calloc, free
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_timetable.h"

/*****************************************************************************/
/*************************** External constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define Tmt_DAYS_PER_WEEK			  7	// Seven days per week

#define Tmt_MINUTES_PER_HOUR		         60	// Number of minutes in 1 hour
#define Tmt_SECONDS_PER_MINUTE		         60	// Number of seconds in 1 minute
#define Tmt_SECONDS_PER_HOUR 	(Tmt_SECONDS_PER_MINUTE * Tmt_MINUTES_PER_HOUR)	// Number of seconds in 1 hour

#define Tmt_START_HOUR			  	  6	// Day starts at this hour
#define Tmt_END_HOUR				 24	// Day  ends  at this hour

#define Tmt_MIN_MINUTES_PER_INTERVAL		  5
#define Tmt_MAX_MINUTES_PER_INTERVAL		 30

#define Tmt_MAX_COLUMNS_PER_CELL			  3	// Maximum number of items (i.e. classes) in a timetable cell (1, 2, 3 or 4)
#define Tmt_NUM_MINICOLUMNS_PER_DAY		  6	// Least common multiple of 1,2,3,...,Tmt_MAX_COLUMNS_PER_CELL

#define Tmt_PERCENT_WIDTH_OF_A_MINICOLUMN	  2	// Width (%) of each minicolumn
#define Tmt_PERCENT_WIDTH_OF_A_DAY		(Tmt_PERCENT_WIDTH_OF_A_MINICOLUMN * Tmt_NUM_MINICOLUMNS_PER_DAY)	// Width (%) of each day
#define Tmt_PERCENT_WIDTH_OF_ALL_DAYS		(Tmt_PERCENT_WIDTH_OF_A_DAY * Tmt_DAYS_PER_WEEK)			// Width (%) of all days
#define Tmt_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN	  1								// Width (%) of left and right columns
#define Tmt_PERCENT_WIDTH_OF_AN_HOUR_COLUMN 	 ((100 - Tmt_PERCENT_WIDTH_OF_ALL_DAYS - Tmt_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN * 2) / 2)	// Width (%) of the separation columns

#define Tmt_MAX_BYTES_STR_CLASS_TYPE		256
#define Tmt_MAX_BYTES_STR_DURATION		 32	// "hh:mm h"

struct Tmt_Column
  {
   long CrsCod;		// Course code (-1 if no course selected)
   long GrpCod;		// Group code (-1 if no group selected)
   Tmt_IntervalType_t IntervalType;
   Tmt_ClassType_t ClassType;
   unsigned DurationIntervals;
   char Info[Tmt_MAX_BYTES_INFO + 1];
  };

struct Tmt_Cell
  {
   unsigned NumColumns;
   struct Tmt_Column Columns[Tmt_MAX_COLUMNS_PER_CELL];
  };

/*****************************************************************************/
/************************** Private global variables *************************/
/*****************************************************************************/

static const char *Tmt_ClassTypeDB[Tmt_NUM_CLASS_TYPES] =
  {
   [Tmt_FREE     ] = "free",
   [Tmt_LECTURE  ] = "lecture",
   [Tmt_PRACTICAL] = "practical",
   [Tmt_TUTORING ] = "tutoring",
  };

struct Tmt_Cell *Tmt_TimeTable[Tmt_DAYS_PER_WEEK];

/* Possible resolutions of the timetable in minutes */
#define Tmt_NUM_RESOLUTIONS 3
static const unsigned Tmt_MinutesPerInterval[Tmt_NUM_RESOLUTIONS] =
  {
    5,	//  5 minutes
   15,	// 15 minutes	// Use 10 or 15 minutes (15 looks better), never both together
   30,	// 30 minutes
  };

/*****************************************************************************/
/***************************** Private prototypes ***************************/
/*****************************************************************************/

static void Tmt_TimeTableConfigureIntervalsAndAllocateTimeTable (struct Tmt_Timetable *Timetable);
static void Tmt_FreeTimeTable (void);

static void Tmt_ShowTimeTableGrpsSelected (void);
static void Tmt_GetParamsTimeTable (struct Tmt_Timetable *Timetable);
static void Tmt_PutContextualIcons (void *Timetable);
static void Tmt_PutFormToSelectWhichGroupsToShow (const struct Tmt_Timetable *Timetable);

static void Tmt_PutIconToViewCrsTT (__attribute__((unused)) void *Args);
static void Tmt_PutIconToViewMyTT (__attribute__((unused)) void *Args);

static void Tmt_WriteCrsTimeTableIntoDB (const struct Tmt_Timetable *Timetable,
                                         long CrsCod);
static void Tmt_WriteTutTimeTableIntoDB (const struct Tmt_Timetable *Timetable,
                                         long UsrCod);
static void Tmt_FillTimeTableFromDB (struct Tmt_Timetable *Timetable,
                                     long UsrCod);
static void Tmt_CalculateRangeCell (unsigned StartTimeSeconds,
                                    unsigned EndTimeSeconds,
                                    struct Tmt_Range *Range);
static unsigned Tmt_CalculateMinutesPerInterval (unsigned Seconds);

static void Tmt_ModifTimeTable (struct Tmt_Timetable *Timetable);
static void Tmt_DrawTimeTable (const struct Tmt_Timetable *Timetable);
static void Tmt_TimeTableDrawAdjustRow (void);
static void Tmt_TimeTableDrawDaysCells (void);
static void Tmt_TimeTableDrawHourCell (unsigned Hour,unsigned Min,const char *Align);
static unsigned Tmt_CalculateColsToDrawInCell (const struct Tmt_Timetable *Timetable,
					       bool TopCall,
                                               unsigned Weekday,unsigned Interval);
static void Tmt_DrawCellAlignTimeTable (void);
static void Tmt_TimeTableDrawCell (const struct Tmt_Timetable *Timetable,
				   unsigned Weekday,unsigned Interval,unsigned Column,unsigned ColSpan,
                                   long CrsCod,Tmt_IntervalType_t IntervalType,Tmt_ClassType_t ClassType,
                                   unsigned DurationNumIntervals,long GrpCod,const char *Info);

/*****************************************************************************/
/******************** Create internal timetable in memory ********************/
/*****************************************************************************/

static void Tmt_TimeTableConfigureIntervalsAndAllocateTimeTable (struct Tmt_Timetable *Timetable)
  {
   unsigned Weekday;

   if (Timetable->Config.Range.Hours.End >
       Timetable->Config.Range.Hours.Start)
     {
      /***** Configuration of timetable depending on hours and resolution *****/
      Timetable->Config.HoursPerDay              = Timetable->Config.Range.Hours.End -
						   Timetable->Config.Range.Hours.Start;	// From start hour to end hour
      Timetable->Config.SecondsPerInterval       = Timetable->Config.Range.MinutesPerInterval *
						   Tmt_SECONDS_PER_MINUTE;
      Timetable->Config.IntervalsPerHour         = Tmt_MINUTES_PER_HOUR /
						   Timetable->Config.Range.MinutesPerInterval;
      Timetable->Config.IntervalsPerDay          = Timetable->Config.IntervalsPerHour *
						   Timetable->Config.HoursPerDay;
      Timetable->Config.IntervalsBeforeStartHour = Timetable->Config.IntervalsPerHour *
						   Timetable->Config.Range.Hours.Start;

      /***** Allocate memory for timetable *****/
      for (Weekday = 0;
	   Weekday < Tmt_DAYS_PER_WEEK;
	   Weekday++)
	 if ((Tmt_TimeTable[Weekday] = malloc (Timetable->Config.IntervalsPerDay *
					      sizeof (*Tmt_TimeTable[Weekday]))) == NULL)
            Lay_NotEnoughMemoryExit ();
     }
   else
     {
      /***** Table is empty *****/
      Timetable->Config.HoursPerDay              = 0;
      Timetable->Config.SecondsPerInterval       = 0;
      Timetable->Config.IntervalsPerHour         = 0;
      Timetable->Config.IntervalsPerDay          = 0;
      Timetable->Config.IntervalsBeforeStartHour = 0;

      /***** Clear timetable in order to not try to free it *****/
      for (Weekday = 0;
	   Weekday < Tmt_DAYS_PER_WEEK;
	   Weekday++)
         Tmt_TimeTable[Weekday] = NULL;
     }
  }

/*****************************************************************************/
/******************** Destroy internal timetable in memory *******************/
/*****************************************************************************/

static void Tmt_FreeTimeTable (void)
  {
   unsigned Weekday;

   /***** Free memory for timetable *****/
   for (Weekday = 0;
	Weekday < Tmt_DAYS_PER_WEEK;
	Weekday++)
      if (Tmt_TimeTable[Weekday])
	{
         free (Tmt_TimeTable[Weekday]);
         Tmt_TimeTable[Weekday] = NULL;
	}
  }

/*****************************************************************************/
/*********** Show whether only my groups or all groups are shown *************/
/*****************************************************************************/

static void Tmt_ShowTimeTableGrpsSelected (void)
  {
   extern const char *Txt_Groups_OF_A_USER;
   extern const char *Txt_All_groups;

   HTM_DIV_Begin ("class=\"CLASSPHOTO_TITLE CM\"");
   switch (Gbl.Crs.Grps.WhichGrps)
     {
      case Grp_MY_GROUPS:
        HTM_TxtF (Txt_Groups_OF_A_USER,Gbl.Usrs.Me.UsrDat.FullName);
        break;
      case Grp_ALL_GROUPS:
        HTM_Txt (Txt_All_groups);
        break;
     }
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Get paramaters for timetable editing *******************/
/*****************************************************************************/

static void Tmt_GetParamsTimeTable (struct Tmt_Timetable *Timetable)
  {
   char StrClassType[Tmt_MAX_BYTES_STR_CLASS_TYPE + 1];
   char StrDuration[Tmt_MAX_BYTES_STR_DURATION + 1];
   unsigned Hours;
   unsigned Minutes;

   /***** Get day (0: monday, 1: tuesday,..., 6: sunday *****/
   Timetable->Weekday = (unsigned)
			Par_GetParToUnsignedLong ("TTDay",
						  0,
						  Tmt_DAYS_PER_WEEK - 1,
						  0);

   /***** Get hour *****/
   Timetable->Interval = (unsigned)
			 Par_GetParToUnsignedLong ("TTInt",
						   0,
						   Timetable->Config.IntervalsPerDay - 1,
						   0);

   /***** Get number of column *****/
   Timetable->Column = (unsigned)
		       Par_GetParToUnsignedLong ("TTCol",
						 0,
						 Tmt_MAX_COLUMNS_PER_CELL - 1,
						 0);

   /***** Get class type *****/
   Par_GetParToText ("TTTyp",StrClassType,Tmt_MAX_BYTES_STR_CLASS_TYPE);
   for (Timetable->ClassType  = (Tmt_ClassType_t) 0;
	Timetable->ClassType <= (Tmt_ClassType_t) (Tmt_NUM_CLASS_TYPES - 1);
	Timetable->ClassType++)
      if (!strcmp (StrClassType,Tmt_ClassTypeDB[Timetable->ClassType]))
         break;
   if (Timetable->ClassType > (Tmt_ClassType_t) (Tmt_NUM_CLASS_TYPES - 1))
      Lay_ShowErrorAndExit ("Type of timetable cell is missing.");

   /***** Get class duration *****/
   Par_GetParToText ("TTDur",StrDuration,Tmt_MAX_BYTES_STR_DURATION);
   if (sscanf (StrDuration,"%u:%u",&Hours,&Minutes) != 2)
      Lay_ShowErrorAndExit ("Duration is missing.");
   Timetable->DurationIntervals = Hours * Timetable->Config.IntervalsPerHour +
	                          Minutes / Timetable->Config.Range.MinutesPerInterval;

   /***** Get group code *****/
   Timetable->GrpCod = Par_GetParToLong ("TTGrp");

   /***** Get info *****/
   Par_GetParToText ("TTInf",Timetable->Info,Tmt_MAX_BYTES_INFO);
  }

/*****************************************************************************/
/**************************** Show class timetable ***************************/
/*****************************************************************************/

void Tmt_ShowClassTimeTable (void)
  {
   extern const char *Hlp_COURSE_Timetable;
   extern const char *Hlp_PROFILE_Timetable;
   extern const char *Txt_TIMETABLE_TYPES[Tmt_NUM_TIMETABLE_TYPES];
   const char *Help[Tmt_NUM_TIMETABLE_TYPES] =
     {
      [Tmt_COURSE_TIMETABLE  ] = Hlp_COURSE_Timetable,
      [Tmt_MY_TIMETABLE      ] = Hlp_PROFILE_Timetable,
      [Tmt_TUTORING_TIMETABLE] = NULL,
     };
   static const Act_Action_t ActChgTT1stDay[Tmt_NUM_TIMETABLE_TYPES] =
     {
      [Tmt_COURSE_TIMETABLE  ] = ActChgCrsTT1stDay,
      [Tmt_MY_TIMETABLE      ] = ActChgMyTT1stDay,
      [Tmt_TUTORING_TIMETABLE] = ActUnk,
     };
   struct Tmt_Timetable Timetable;
   bool PrintView = (Gbl.Action.Act == ActPrnCrsTT ||
	             Gbl.Action.Act == ActPrnMyTT);
   Grp_WhichGroups_t WhichGroups;

   /***** Initializations *****/
   switch (Gbl.Action.Act)
     {
      case ActSeeCrsTT:
      case ActPrnCrsTT:
      case ActChgCrsTT1stDay:
         Timetable.Type = Tmt_COURSE_TIMETABLE;
	 break;
      case ActSeeMyTT:
      case ActPrnMyTT:
      case ActChgMyTT1stDay:
         Timetable.Type = Tmt_MY_TIMETABLE;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong action.");
     }

   Timetable.ContextualIcons.PutIconEditCrsTT = (Timetable.Type == Tmt_COURSE_TIMETABLE &&
	                                         !PrintView &&
                                                 Gbl.Usrs.Me.Role.Logged >= Rol_TCH);
   Timetable.ContextualIcons.PutIconEditOfficeHours = (Timetable.Type == Tmt_MY_TIMETABLE &&
	                                               !PrintView &&
                                                       (Gbl.Usrs.Me.Role.Available & (1 << Rol_TCH)));
   Timetable.ContextualIcons.PutIconPrint = !PrintView;

   /***** Get whether to show only my groups or all groups *****/
   Grp_GetParamWhichGroups ();

   /***** Begin box *****/
   if (Timetable.ContextualIcons.PutIconEditCrsTT ||
       Timetable.ContextualIcons.PutIconEditOfficeHours ||
       Timetable.ContextualIcons.PutIconPrint)
      Box_BoxBegin ("100%",Txt_TIMETABLE_TYPES[Timetable.Type],
		    Tmt_PutContextualIcons,&Timetable,
		    Help[Timetable.Type],Box_NOT_CLOSABLE);
   else
      Box_BoxBegin ("100%",Txt_TIMETABLE_TYPES[Timetable.Type],
		    NULL,NULL,
		    Help[Timetable.Type],Box_NOT_CLOSABLE);

   /***** Start time table drawing *****/
   if (Timetable.Type == Tmt_COURSE_TIMETABLE)
      Lay_WriteHeaderClassPhoto (PrintView,false,
				 Gbl.Hierarchy.Ins.InsCod,Gbl.Hierarchy.Deg.DegCod,Gbl.Hierarchy.Crs.CrsCod);

   if (PrintView)
      /***** Show whether only my groups or all groups are selected *****/
      Tmt_ShowTimeTableGrpsSelected ();
   else
     {
      /***** Setting selector *****/
      Set_BeginSettingsHead ();

      /* Select whether show only my groups or all groups */
      if ( Timetable.Type == Tmt_MY_TIMETABLE ||
          (Timetable.Type == Tmt_COURSE_TIMETABLE &&
           Gbl.Crs.Grps.NumGrps))
         Tmt_PutFormToSelectWhichGroupsToShow (&Timetable);

      /* Show form to change first day of week */
      WhichGroups = Grp_GetParamWhichGroups ();
      Cal_ShowFormToSelFirstDayOfWeek (ActChgTT1stDay[Timetable.Type],
                                       Grp_PutParamWhichGroups,&WhichGroups);

      Set_EndSettingsHead ();
     }

   /***** Show the time table *****/
   Tmt_ShowTimeTable (&Timetable,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Put contextual icons above the time table *****************/
/*****************************************************************************/

static void Tmt_PutContextualIcons (void *Timetable)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Timetable)
     {
      WhichGroups = Grp_GetParamWhichGroups ();

      if (((struct Tmt_Timetable *) Timetable)->ContextualIcons.PutIconEditCrsTT)
	 Ico_PutContextualIconToEdit (ActEdiCrsTT,NULL,
				      Grp_PutParamWhichGroups,&WhichGroups);

      if (((struct Tmt_Timetable *) Timetable)->ContextualIcons.PutIconEditOfficeHours)
	 Ico_PutContextualIconToEdit (ActEdiTut,NULL,
				      NULL,NULL);

      if (((struct Tmt_Timetable *) Timetable)->ContextualIcons.PutIconPrint)
	 Ico_PutContextualIconToPrint (((struct Tmt_Timetable *) Timetable)->Type == Tmt_COURSE_TIMETABLE ? ActPrnCrsTT :
										                          ActPrnMyTT,
				       Grp_PutParamWhichGroups,&WhichGroups);
     }
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void Tmt_PutFormToSelectWhichGroupsToShow (const struct Tmt_Timetable *Timetable)
  {
   static const Act_Action_t ActSeeTT[Tmt_NUM_TIMETABLE_TYPES] =
     {
      [Tmt_COURSE_TIMETABLE  ] = ActSeeCrsTT,
      [Tmt_MY_TIMETABLE      ] = ActSeeMyTT,
      [Tmt_TUTORING_TIMETABLE] = ActUnk,
     };

   Grp_ShowFormToSelWhichGrps (ActSeeTT[Timetable->Type],
                               NULL,NULL);
  }

/*****************************************************************************/
/********************** Show course timetable for edition ********************/
/*****************************************************************************/

void Tmt_EditCrsTimeTable (void)
  {
   extern const char *Hlp_COURSE_Timetable;
   extern const char *Txt_TIMETABLE_TYPES[Tmt_NUM_TIMETABLE_TYPES];
   struct Tmt_Timetable Timetable;

   /***** Editable time table *****/
   Timetable.Type = Tmt_COURSE_TIMETABLE;
   Box_BoxBegin ("100%",Txt_TIMETABLE_TYPES[Timetable.Type],
                 Tmt_PutIconToViewCrsTT,NULL,
                 Hlp_COURSE_Timetable,Box_NOT_CLOSABLE);
   Tmt_ShowTimeTable (&Timetable,Gbl.Usrs.Me.UsrDat.UsrCod);
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Show tutor timetable for edition **********************/
/*****************************************************************************/

void Tmt_EditMyTutTimeTable (void)
  {
   extern const char *Hlp_PROFILE_Timetable;
   extern const char *Txt_TIMETABLE_TYPES[Tmt_NUM_TIMETABLE_TYPES];
   struct Tmt_Timetable Timetable;

   /***** Time table *****/
   Timetable.Type = Tmt_TUTORING_TIMETABLE;
   Box_BoxBegin ("100%",Txt_TIMETABLE_TYPES[Timetable.Type],
                 Tmt_PutIconToViewMyTT,NULL,
                 Hlp_PROFILE_Timetable,Box_NOT_CLOSABLE);
   Tmt_ShowTimeTable (&Timetable,Gbl.Usrs.Me.UsrDat.UsrCod);
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Put icon to view course timetable ********************/
/*****************************************************************************/

static void Tmt_PutIconToViewCrsTT (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToView (ActSeeCrsTT,
				NULL,NULL);
  }

/*****************************************************************************/
/************************ Put icon to view my timetable **********************/
/*****************************************************************************/

static void Tmt_PutIconToViewMyTT (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToView (ActSeeMyTT,
				NULL,NULL);
  }

/*****************************************************************************/
/*********** Show course timetable or tutor timetable of a teacher ***********/
/*****************************************************************************/

void Tmt_ShowTimeTable (struct Tmt_Timetable *Timetable,long UsrCod)
  {
   extern const char *Txt_The_timetable_is_empty;

   /***** Set type of view depending on current action *****/
   Timetable->View = Tmt_CRS_VIEW;
   switch (Gbl.Action.Act)
     {
      case ActSeeCrsTT:		case ActPrnCrsTT:	case ActChgCrsTT1stDay:
      case ActSeeMyTT:		case ActPrnMyTT:	case ActChgMyTT1stDay:
	 Timetable->View = Tmt_CRS_VIEW;
	 break;
      case ActSeeRecOneTch:	case ActSeeRecSevTch:
	 Timetable->View = Tmt_TUT_VIEW;
	 break;
      case ActEdiCrsTT:		case ActChgCrsTT:
	 Timetable->View = Tmt_CRS_EDIT;
	 break;
      case ActEdiTut:		case ActChgTut:
	 Timetable->View = Tmt_TUT_EDIT;
	 break;
     }

   /***** If editing ==> configure and allocate timetable *****/
   if (Timetable->View == Tmt_CRS_EDIT ||
       Timetable->View == Tmt_TUT_EDIT)
     {
      Timetable->Config.Range.Hours.Start        = Tmt_START_HOUR;		// Day starts at this hour
      Timetable->Config.Range.Hours.End	         = Tmt_END_HOUR;			// Day  ends  at this hour
      Timetable->Config.Range.MinutesPerInterval = Tmt_MinutesPerInterval[0];	// The smallest interval
      Tmt_TimeTableConfigureIntervalsAndAllocateTimeTable (Timetable);
     }
   /* If viewing (not editing) ==>
      configure and allocate memory when table is read from database */

   /***** Fill internal timetable with the timetable from database *****/
   Tmt_FillTimeTableFromDB (Timetable,UsrCod);

   /***** If timetable must be modified... *****/
   if (Gbl.Action.Act == ActChgCrsTT ||
       Gbl.Action.Act == ActChgTut)
     {
      /* Get parameters for time table editing */
      Tmt_GetParamsTimeTable (Timetable);

      /* Modify timetable in memory */
      Tmt_ModifTimeTable (Timetable);

      /* Write a new timetable in database */
      switch (Timetable->Type)
        {
         case Tmt_COURSE_TIMETABLE:
            Tmt_WriteCrsTimeTableIntoDB (Timetable,Gbl.Hierarchy.Crs.CrsCod);
	    break;
         case Tmt_TUTORING_TIMETABLE:
            Tmt_WriteTutTimeTableIntoDB (Timetable,UsrCod);
            break;
         default:
            break;
        }

      /* Get a new table from database */
      Tmt_FillTimeTableFromDB (Timetable,UsrCod);
     }

   /***** Draw timetable *****/
   if (Timetable->Config.HoursPerDay)
      Tmt_DrawTimeTable (Timetable);
   else
      Ale_ShowAlert (Ale_INFO,Txt_The_timetable_is_empty);

   /***** Free internal timetable in memory *****/
   Tmt_FreeTimeTable ();
  }

/*****************************************************************************/
/******************* Write course timetable into database ********************/
/*****************************************************************************/

static void Tmt_WriteCrsTimeTableIntoDB (const struct Tmt_Timetable *Timetable,
                                         long CrsCod)
  {
   unsigned Weekday;
   unsigned Interval;
   unsigned Hour;
   unsigned Min;
   unsigned Column;

   /***** Remove former timetable *****/
   DB_QueryDELETE ("can not remove former timetable",
		   "DELETE FROM tmt_courses"
		   " WHERE CrsCod=%ld",
		   CrsCod);

   /***** Go across the timetable inserting classes into database *****/
   for (Weekday = 0;
	Weekday < Tmt_DAYS_PER_WEEK;
	Weekday++)
      for (Interval = 0, Hour = Timetable->Config.Range.Hours.Start, Min = 0;
	   Interval < Timetable->Config.IntervalsPerDay;
	   Interval++,
	   Hour += (Min + Timetable->Config.Range.MinutesPerInterval) / Tmt_SECONDS_PER_MINUTE,
	   Min   = (Min + Timetable->Config.Range.MinutesPerInterval) % Tmt_SECONDS_PER_MINUTE)
         for (Column = 0;
              Column < Tmt_MAX_COLUMNS_PER_CELL;
              Column++)
	    if (Tmt_TimeTable[Weekday][Interval].Columns[Column].IntervalType == Tmt_FIRST_INTERVAL &&
                Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals)
               DB_QueryINSERT ("can not create course timetable",
        		       "INSERT INTO tmt_courses"
        	               " (CrsCod,GrpCod,Weekday,StartTime,Duration,"
        	               "ClassType,Info)"
                               " VALUES"
                               " (%ld,%ld,%u,'%02u:%02u:00',SEC_TO_TIME(%u),"
                               "'%s','%s')",
			       CrsCod,
			       Tmt_TimeTable[Weekday][Interval].Columns[Column].GrpCod,
			       Weekday,
			       Hour,Min,
			       Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals *
			       Timetable->Config.SecondsPerInterval,
			       Tmt_ClassTypeDB[Tmt_TimeTable[Weekday][Interval].Columns[Column].ClassType],
			       Tmt_TimeTable[Weekday][Interval].Columns[Column].Info);
  }

/*****************************************************************************/
/********************* Write tutor timetable into database *******************/
/*****************************************************************************/

static void Tmt_WriteTutTimeTableIntoDB (const struct Tmt_Timetable *Timetable,
                                         long UsrCod)
  {
   unsigned Weekday;
   unsigned Interval;
   unsigned Hour;
   unsigned Min;
   unsigned Column;

   /***** Remove former timetable *****/
   DB_QueryDELETE ("can not remove former timetable",
		   "DELETE FROM tmt_tutoring"
		   " WHERE UsrCod=%ld",
		   UsrCod);

   /***** Loop over timetable *****/
   for (Weekday = 0;
	Weekday < Tmt_DAYS_PER_WEEK;
	Weekday++)
      for (Interval = 0, Hour = Timetable->Config.Range.Hours.Start, Min = 0;
	   Interval < Timetable->Config.IntervalsPerDay;
	   Interval++,
	   Hour += (Min + Timetable->Config.Range.MinutesPerInterval) / Tmt_SECONDS_PER_MINUTE,
	   Min   = (Min + Timetable->Config.Range.MinutesPerInterval) % Tmt_SECONDS_PER_MINUTE)
	 for (Column = 0;
              Column < Tmt_MAX_COLUMNS_PER_CELL;
              Column++)
	    if (Tmt_TimeTable[Weekday][Interval].Columns[Column].IntervalType == Tmt_FIRST_INTERVAL &&
                Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals)
               DB_QueryINSERT ("can not create office timetable",
        		       "INSERT INTO tmt_tutoring"
        	               " (UsrCod,Weekday,StartTime,Duration,Info)"
                               " VALUES"
                               " (%ld,%u,'%02u:%02u:00',SEC_TO_TIME(%u),'%s')",
			       UsrCod,
			       Weekday,
			       Hour,Min,
			       Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals *
			       Timetable->Config.SecondsPerInterval,
			       Tmt_TimeTable[Weekday][Interval].Columns[Column].Info);
  }

/*****************************************************************************/
/********** Create an internal table with timetable from database ************/
/*****************************************************************************/

static void Tmt_FillTimeTableFromDB (struct Tmt_Timetable *Timetable,
                                     long UsrCod)
  {
   extern const char *Txt_Incomplete_timetable_for_lack_of_space;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned long NumRow;
   unsigned Weekday;
   unsigned Interval;
   unsigned i;	// To iterate through intervals
   unsigned DurationNumIntervals;
   unsigned Column;
   unsigned StartTimeSeconds;
   unsigned DurationSeconds;
   unsigned EndTimeSeconds;
   struct Tmt_Range RangeCell;
   unsigned FirstFreeColumn;
   Tmt_ClassType_t ClassType = Tmt_FREE;	// Initialized to avoid warning
   bool TimeTableIsIncomplete = false;
   bool TimeTableHasSpaceForThisClass;
   bool Found;

   /***** Get timetable from database *****/
   switch (Timetable->Type)
     {
      case Tmt_MY_TIMETABLE:
         switch (Gbl.Crs.Grps.WhichGrps)
           {
            case Grp_MY_GROUPS:
               NumRows = DB_QuerySELECT (&mysql_res,"can not get timetable",
					 "SELECT tmt_courses.Weekday,"
					        "TIME_TO_SEC(tmt_courses.StartTime) AS S,"
					        "TIME_TO_SEC(tmt_courses.Duration) AS D,"
					        "tmt_courses.Info,"
					        "tmt_courses.ClassType,"
					        "tmt_courses.GrpCod,"
					        "tmt_courses.CrsCod"
					  " FROM tmt_courses,"
					        "crs_users"
					 " WHERE crs_users.UsrCod=%ld"
					   " AND tmt_courses.GrpCod=-1"
					   " AND tmt_courses.CrsCod=crs_users.CrsCod"
					 " UNION DISTINCT "
					 "SELECT tmt_courses.Weekday,"
					        "TIME_TO_SEC(tmt_courses.StartTime) AS S,"
					        "TIME_TO_SEC(tmt_courses.Duration) AS D,"
					        "tmt_courses.Info,"
					        "tmt_courses.ClassType,"
					        "tmt_courses.GrpCod,"
					        "tmt_courses.CrsCod"
					  " FROM tmt_courses,"
					        "crs_grp_usr"
					 " WHERE crs_grp_usr.UsrCod=%ld"
					   " AND tmt_courses.GrpCod=crs_grp_usr.GrpCod"
					 " UNION "
					 "SELECT Weekday,"
					        "TIME_TO_SEC(StartTime) AS S,"
					        "TIME_TO_SEC(Duration) AS D,"
					        "Info,"
					        "'tutoring' AS ClassType,"
					        "-1 AS GrpCod,"
					        "-1 AS CrsCod"
					  " FROM tmt_tutoring"
					 " WHERE UsrCod=%ld"
					 " ORDER BY Weekday,S,ClassType,"
					 "GrpCod,Info,D DESC,CrsCod",
					 UsrCod,UsrCod,UsrCod);
               break;
            case Grp_ALL_GROUPS:
               NumRows = DB_QuerySELECT (&mysql_res,"can not get timetable",
					 "SELECT tmt_courses.Weekday,"				// row[0]
					        "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
					        "TIME_TO_SEC(tmt_courses.Duration) AS D,"	// row[2]
					        "tmt_courses.Info,"				// row[3]
					        "tmt_courses.ClassType,"			// row[4]
					        "tmt_courses.GrpCod,"				// row[5]
					        "tmt_courses.CrsCod"				// row[6]
					  " FROM tmt_courses,"
					        "crs_users"
					 " WHERE crs_users.UsrCod=%ld"
					   " AND tmt_courses.CrsCod=crs_users.CrsCod"
					 " UNION "
					 "SELECT Weekday,"					// row[0]
					        "TIME_TO_SEC(StartTime) AS S,"
					        "TIME_TO_SEC(Duration) AS D,"
					        "Info,"
					        "'tutoring' AS ClassType,"
					        "-1 AS GrpCod,"
					        "-1 AS CrsCod"
					  " FROM tmt_tutoring"
					 " WHERE UsrCod=%ld"
					 " ORDER BY Weekday,"
					           "S,"
					           "ClassType,"
					           "GrpCod,"
					           "Info,"
					           "D DESC,"
					           "CrsCod",
					 UsrCod,UsrCod);
               break;
           }
	 break;
      case Tmt_COURSE_TIMETABLE:
         if (Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS ||
             Gbl.Action.Act == ActEdiCrsTT ||
             Gbl.Action.Act == ActChgCrsTT)	// If we are editing, all groups are shown
            NumRows = DB_QuerySELECT (&mysql_res,"can not get timetable",
				      "SELECT Weekday,"				// row[0]
					     "TIME_TO_SEC(StartTime) AS S,"	// row[1]
					     "TIME_TO_SEC(Duration) AS D,"	// row[2]
					     "Info,"				// row[3]
					     "ClassType,"			// row[4]
					     "GrpCod"				// row[5]
				       " FROM tmt_courses"
				      " WHERE CrsCod=%ld"
				      " ORDER BY Weekday,"
				                "S,"
				                "ClassType,"
				                "GrpCod,"
				                "Info,"
				                "D DESC",
				      Gbl.Hierarchy.Crs.CrsCod);
         else
            NumRows = DB_QuerySELECT (&mysql_res,"can not get timetable",
				      "SELECT tmt_courses.Weekday,"			// row[0]
					     "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
					     "TIME_TO_SEC(tmt_courses.Duration) AS D,"	// row[2]
					     "tmt_courses.Info,"			// row[3]
					     "tmt_courses.ClassType,"			// row[4]
					     "tmt_courses.GrpCod"			// row[5]
				       " FROM tmt_courses,"
				             "crs_users"
				      " WHERE tmt_courses.CrsCod=%ld"
				        " AND tmt_courses.GrpCod=-1"
				        " AND crs_users.UsrCod=%ld"
				        " AND tmt_courses.CrsCod=crs_users.CrsCod"
				      " UNION DISTINCT "
				      "SELECT tmt_courses.Weekday,"			// row[0]
					     "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
					     "TIME_TO_SEC(tmt_courses.Duration) AS D,"	// row[2]
					     "tmt_courses.Info,"			// row[3]
					     "tmt_courses.ClassType,"			// row[4]
					     "tmt_courses.GrpCod"			// row[5]
				       " FROM tmt_courses,crs_grp_usr"
				      " WHERE tmt_courses.CrsCod=%ld"
				        " AND crs_grp_usr.UsrCod=%ld"
				        " AND tmt_courses.GrpCod=crs_grp_usr.GrpCod"
				      " ORDER BY Weekday,"
				                "S,"
				                "ClassType,"
				                "GrpCod,"
				                "Info,"
				                "D DESC",
				      Gbl.Hierarchy.Crs.CrsCod,UsrCod,
				      Gbl.Hierarchy.Crs.CrsCod,UsrCod);
	 break;
      case Tmt_TUTORING_TIMETABLE:
         NumRows = DB_QuerySELECT (&mysql_res,"can not get timetable",
				   "SELECT Weekday,"				// row[0]
					  "TIME_TO_SEC(StartTime) AS S,"	// row[1]
					  "TIME_TO_SEC(Duration) AS D,"		// row[2]
					  "Info"				// row[3]
				    " FROM tmt_tutoring"
				   " WHERE UsrCod=%ld"
				   " ORDER BY Weekday,"
				             "S,"
				             "Info,"
				             "D DESC",
				   UsrCod);
         break;
     }

   /***** If viewing (not editing) ==>
          calculate range of hours and resolution *****/
   if (Timetable->View == Tmt_CRS_VIEW ||
       Timetable->View == Tmt_TUT_VIEW)
     {
      /* Initialize hours and resolution for timetable */
      Timetable->Config.Range.Hours.Start        = Tmt_END_HOUR;		// Initialized to maximum hour
      Timetable->Config.Range.Hours.End          = Tmt_START_HOUR;		// Initialized to minimum hour
      Timetable->Config.Range.MinutesPerInterval = Tmt_MinutesPerInterval[Tmt_NUM_RESOLUTIONS - 1];	// The longest interval

      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* StartTime formatted as seconds (row[1]) */
	 if (sscanf (row[1],"%u",&StartTimeSeconds) != 1)
	    Lay_ShowErrorAndExit ("Wrong start time in timetable.");

	 /* Duration formatted as seconds (row[2]) */
	 if (sscanf (row[2],"%u",&DurationSeconds) != 1)
	    Lay_ShowErrorAndExit ("Wrong duration in timetable.");
	 EndTimeSeconds = StartTimeSeconds + DurationSeconds;

	 /* Compute hours and resolution */
	 Tmt_CalculateRangeCell (StartTimeSeconds,EndTimeSeconds,&RangeCell);
	 if (RangeCell.Hours.Start < Timetable->Config.Range.Hours.Start)
            Timetable->Config.Range.Hours.Start = RangeCell.Hours.Start;
	 if (RangeCell.Hours.End > Timetable->Config.Range.Hours.End)
            Timetable->Config.Range.Hours.End = RangeCell.Hours.End;
	 if (RangeCell.MinutesPerInterval < Timetable->Config.Range.MinutesPerInterval)
            Timetable->Config.Range.MinutesPerInterval = RangeCell.MinutesPerInterval;

	}
      mysql_data_seek (mysql_res,0);

      /***** Configure and allocate timetable *****/
      Tmt_TimeTableConfigureIntervalsAndAllocateTimeTable (Timetable);
     }

   /***** Build the table by filling it from database *****/
   if (Timetable->Config.HoursPerDay)
     {
      /***** Initialize timetable to all free *****/
      for (Weekday = 0;
	   Weekday < Tmt_DAYS_PER_WEEK;
	   Weekday++)
	 for (Interval = 0;
	      Interval < Timetable->Config.IntervalsPerDay;
	      Interval++)
	   {
	    Tmt_TimeTable[Weekday][Interval].NumColumns = 0;
	    for (Column = 0;
		 Column < Tmt_MAX_COLUMNS_PER_CELL;
		 Column++)
	      {
	       Tmt_TimeTable[Weekday][Interval].Columns[Column].CrsCod            = -1L;
	       Tmt_TimeTable[Weekday][Interval].Columns[Column].GrpCod            = -1L;
	       Tmt_TimeTable[Weekday][Interval].Columns[Column].IntervalType      = Tmt_FREE_INTERVAL;
	       Tmt_TimeTable[Weekday][Interval].Columns[Column].ClassType         = Tmt_FREE;
	       Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals = 0;
	       Tmt_TimeTable[Weekday][Interval].Columns[Column].Info[0]           = '\0';
	      }
	   }

      /***** Fill data from database *****/
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Day of week (row[0]) */
	 if (sscanf (row[0],"%u",&Weekday) != 1)
	    Lay_ShowErrorAndExit ("Wrong day of week in timetable.");
	 if (Weekday >= Tmt_DAYS_PER_WEEK)
	    Lay_ShowErrorAndExit ("Wrong day of week in timetable.");

	 /* StartTime formatted as seconds (row[1])
	    --> StartTime in number of intervals */
	 if (sscanf (row[1],"%u",&StartTimeSeconds) != 1)
	    Lay_ShowErrorAndExit ("Wrong start time in timetable.");
	 Interval = StartTimeSeconds /
	            Timetable->Config.SecondsPerInterval;
	 if (Interval < Timetable->Config.IntervalsBeforeStartHour)
	    Lay_ShowErrorAndExit ("Wrong start time in timetable.");
	 Interval -= Timetable->Config.IntervalsBeforeStartHour;

	 /* Duration formatted as seconds (row[2])
	    --> Duration in number of intervals */
	 if (sscanf (row[2],"%u",&DurationSeconds) != 1)
	    Lay_ShowErrorAndExit ("Wrong duration in timetable.");
	 DurationNumIntervals = DurationSeconds /
	                        Timetable->Config.SecondsPerInterval;

	 /* Type of class (row[4]) */
	 switch (Timetable->Type)
	   {
	    case Tmt_COURSE_TIMETABLE:
	    case Tmt_MY_TIMETABLE:
	       for (ClassType = Tmt_LECTURE, Found = false;
		    ClassType <= Tmt_TUTORING;
		    ClassType++)
		  if (!strcmp (row[4],Tmt_ClassTypeDB[ClassType]))
		    {
		     Found = true;
		     break;
		    }
	       if (!Found)
		  Lay_ShowErrorAndExit ("Wrong type of class in timetable.");
	       break;
	    case Tmt_TUTORING_TIMETABLE:
	       ClassType = Tmt_TUTORING;
	       break;
	   }

	 /* Cell has been read without errors */
	 if (Tmt_TimeTable[Weekday][Interval].NumColumns < Tmt_MAX_COLUMNS_PER_CELL)
	    // If there's place for another column in this cell...
	   {
	    /* Find the first free column for this day-hour */
	    FirstFreeColumn = Tmt_MAX_COLUMNS_PER_CELL;
	    for (Column = 0;
		 Column < Tmt_MAX_COLUMNS_PER_CELL;
		 Column++)
	       if (Tmt_TimeTable[Weekday][Interval].Columns[Column].IntervalType == Tmt_FREE_INTERVAL)
		 {
		  FirstFreeColumn = Column;
		  break;
		 }

	    if (FirstFreeColumn < Tmt_MAX_COLUMNS_PER_CELL)
	       // If there's place for another column in this cell
	      {
	       /* Check if there's place for all the rows of this class */
	       TimeTableHasSpaceForThisClass = true;
	       for (i = Interval + 1;
		    i < Interval + DurationNumIntervals &&
		    i < Timetable->Config.IntervalsPerDay;
		    i++)
		  if (Tmt_TimeTable[Weekday][i].Columns[FirstFreeColumn].IntervalType != Tmt_FREE_INTERVAL)
		   {
		     TimeTableIsIncomplete = true;
		     TimeTableHasSpaceForThisClass = false;
		     break;
		    }
	       if (TimeTableHasSpaceForThisClass)
		 {
		  Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].ClassType         = ClassType;
		  Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].DurationIntervals = DurationNumIntervals;
		  Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].IntervalType      = Tmt_FIRST_INTERVAL;
		  for (i = Interval + 1;
		       i < Interval + DurationNumIntervals &&
		       i < Timetable->Config.IntervalsPerDay;
		       i++)
		    {
		     Tmt_TimeTable[Weekday][i].Columns[FirstFreeColumn].IntervalType = Tmt_NEXT_INTERVAL;
		     Tmt_TimeTable[Weekday][i].NumColumns++;
		    }

		  /* Course (row[6]) and info (row[3])*/
		  switch (Timetable->Type)
		    {
		     case Tmt_MY_TIMETABLE:
		     case Tmt_COURSE_TIMETABLE:
			/* Group code (row[5]) */
			if (Timetable->Type == Tmt_MY_TIMETABLE ||
			    Timetable->Type == Tmt_COURSE_TIMETABLE)
			   if (sscanf (row[5],"%ld",&Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].GrpCod) != 1)
			      Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].GrpCod = -1;

			/* Course code (row[6]) */
			Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].CrsCod =
			   (Timetable->Type == Tmt_MY_TIMETABLE ? Str_ConvertStrCodToLongCod (row[6]) :
								    Gbl.Hierarchy.Crs.CrsCod);
			/* falls through */
			/* no break */
		     case Tmt_TUTORING_TIMETABLE:
			Str_Copy (Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].Info,
				  row[3],
				  sizeof (Tmt_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].Info) - 1);
			break;
		    }

		  /* Increment number of items in this cell */
		  Tmt_TimeTable[Weekday][Interval].NumColumns++;
		 }
	      }
	    else
	       TimeTableIsIncomplete = true;
	   }
	 else
	    TimeTableIsIncomplete = true;
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (TimeTableIsIncomplete)
      Ale_ShowAlert (Ale_INFO,Txt_Incomplete_timetable_for_lack_of_space);
  }

/*****************************************************************************/
/** Calculate range of a cell (start hour, end hour, minutes per interval) ***/
/*****************************************************************************/

static void Tmt_CalculateRangeCell (unsigned StartTimeSeconds,
                                    unsigned EndTimeSeconds,
                                    struct Tmt_Range *Range)
  {
   unsigned TimeMinutes;
   unsigned MinutesPerIntervalForEndTime;

   /***** Compute minimum hour *****/
   // Example: if Seconds == 42300 (time == 11:45:00) =>
   //          TimeMinutes = 42300/60 = 705 =>
   //          Hour = 705/60 = 11
   TimeMinutes = StartTimeSeconds / Tmt_SECONDS_PER_MINUTE;
   Range->Hours.Start = TimeMinutes / Tmt_MINUTES_PER_HOUR;

   /***** Compute maximum hour *****/
   // Example: if Seconds == 42300 (time == 11:45:00) =>
   //          TimeMinutes = 42300/60 = 705 =>
   //          Hour = 705/60 = 11
   // 	       705 % 60 = 45 ==> Hour = Hour+1 = 12
   TimeMinutes = EndTimeSeconds / Tmt_SECONDS_PER_MINUTE;
   Range->Hours.End = TimeMinutes / Tmt_MINUTES_PER_HOUR;
   if (TimeMinutes % Tmt_MINUTES_PER_HOUR)
      Range->Hours.End++;

   /***** Compute resolution (longest interval necessary for this cell) *****/
   Range->MinutesPerInterval = Tmt_CalculateMinutesPerInterval (StartTimeSeconds);
   if (Range->MinutesPerInterval > Tmt_MinutesPerInterval[0])	// If not already the shortest
     {
      MinutesPerIntervalForEndTime = Tmt_CalculateMinutesPerInterval (EndTimeSeconds);
      if (MinutesPerIntervalForEndTime < Range->MinutesPerInterval)
	 Range->MinutesPerInterval = MinutesPerIntervalForEndTime;
     }
  }

/*****************************************************************************/
/*********************** Calculate minutes per interval **********************/
/*****************************************************************************/
// Example: if Seconds == 42300 (time == 11:45:00) => Minutes = 45 => Resolution = 15

static unsigned Tmt_CalculateMinutesPerInterval (unsigned Seconds)
  {
   unsigned Minutes;
   unsigned MinutesPerInterval;
   unsigned Resolution;

   /***** Compute minutes part (45) of a time (11:45:00) from seconds (42300) *****/
   Minutes = (Seconds / Tmt_SECONDS_PER_MINUTE) % Tmt_MINUTES_PER_HOUR;

   /***** Compute minutes per interval *****/
   MinutesPerInterval = Tmt_MinutesPerInterval[0];	// Default: the shortest interval
   for (Resolution = Tmt_NUM_RESOLUTIONS - 1;		// From the longest interval...
	Resolution > 0;
	Resolution--)					// ...to shorter intervals
      if (Minutes % Tmt_MinutesPerInterval[Resolution] == 0)
	{
	 MinutesPerInterval = Tmt_MinutesPerInterval[Resolution];
	 break;
	}

   return MinutesPerInterval;
  }

/*****************************************************************************/
/*********************** Modify a class in timetable *************************/
/*****************************************************************************/

static void Tmt_ModifTimeTable (struct Tmt_Timetable *Timetable)
  {
   if (Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].IntervalType == Tmt_FIRST_INTERVAL)
     {
      /***** Free this cell *****/
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].GrpCod            = -1L;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].IntervalType      = Tmt_FREE_INTERVAL;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].ClassType         = Tmt_FREE;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].DurationIntervals = 0;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].Info[0]          = '\0';
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].NumColumns--;
     }

   if (Timetable->ClassType != Tmt_FREE &&
       Timetable->DurationIntervals > 0 &&
       Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].NumColumns < Tmt_MAX_COLUMNS_PER_CELL)
     {
      /***** Change this cell *****/
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].NumColumns++;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].GrpCod            = Timetable->GrpCod;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].IntervalType      = Tmt_FIRST_INTERVAL;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].ClassType         = Timetable->ClassType;
      Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].DurationIntervals = Timetable->DurationIntervals;
      Str_Copy (Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].Info,
                Timetable->Info,
                sizeof (Tmt_TimeTable[Timetable->Weekday][Timetable->Interval].Columns[Timetable->Column].Info) - 1);
     }
  }

/*****************************************************************************/
/********************* Draw timetable using internal table *******************/
/*****************************************************************************/

static void Tmt_DrawTimeTable (const struct Tmt_Timetable *Timetable)
  {
   unsigned DayColumn;	// Column from left (0) to right (6)
   unsigned Weekday;	// Day of week
   unsigned Interval;
   unsigned Min;
   unsigned Column;
   unsigned ColumnsToDraw;
   unsigned ColumnsToDrawIncludingExtraColumn;
   unsigned ContinuousFreeMinicolumns;

   /***** Begin table *****/
   HTM_TABLE_Begin ("TT");

   /***** Top row used for column adjustement *****/
   Tmt_TimeTableDrawAdjustRow ();

   /***** Row with day names *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("rowspan=\"2\" class=\"Tmt_HOUR_BIG RM\" style=\"width:%u%%;\"",
		 Tmt_PERCENT_WIDTH_OF_AN_HOUR_COLUMN);
   HTM_TxtF ("%02u:00",Timetable->Config.Range.Hours.Start);
   HTM_TD_End ();

   Tmt_DrawCellAlignTimeTable ();
   Tmt_TimeTableDrawDaysCells ();
   Tmt_DrawCellAlignTimeTable ();

   HTM_TD_Begin ("rowspan=\"2\" class=\"Tmt_HOUR_BIG LM\" style=\"width:%u%%;\"",
		 Tmt_PERCENT_WIDTH_OF_AN_HOUR_COLUMN);
   HTM_TxtF ("%02u:00",Timetable->Config.Range.Hours.Start);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Get list of groups types and groups in this course *****/
   if (Gbl.Action.Act == ActEdiCrsTT ||
       Gbl.Action.Act == ActChgCrsTT)
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Write the table row by row *****/
   for (Interval = 0, Min = Timetable->Config.Range.MinutesPerInterval;
	Interval < Timetable->Config.IntervalsPerDay;
	Interval++,
	Min = (Min + Timetable->Config.Range.MinutesPerInterval) %
	      Tmt_SECONDS_PER_MINUTE)
     {
      HTM_TR_Begin (NULL);

      /* Left hour:minutes cell */
      if (Interval % 2)
	 Tmt_TimeTableDrawHourCell (Timetable->Config.Range.Hours.Start +
	                           (Interval + 2) / Timetable->Config.IntervalsPerHour,
	                           Min,
	                           "RM");

      /* Empty column used to adjust height */
      Tmt_DrawCellAlignTimeTable ();

      /* Row for this hour */
      for (DayColumn = 0;
	   DayColumn < Tmt_DAYS_PER_WEEK;
	   DayColumn++)
        {
	 /* Weekday == 0 ==> monday,
	            ...
	    Weekday == 6 ==> sunday */
	 Weekday = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;

         /* Check how many colums are needed.
            For each item (class) in this hour from left to right,
            we must check the maximum of columns */
         ColumnsToDraw = Tmt_CalculateColsToDrawInCell (Timetable,
                                                       true,	// Top call, non recursive
                                                       Weekday,Interval);
         if (ColumnsToDraw == 0 &&
             (Timetable->View == Tmt_CRS_VIEW ||
              Timetable->View == Tmt_TUT_VIEW))
            ColumnsToDraw = 1;
	 // If editing and there's place for more columns,
         // a potential new column is added at the end of each day
         ColumnsToDrawIncludingExtraColumn = ColumnsToDraw;
         if (ColumnsToDraw < Tmt_MAX_COLUMNS_PER_CELL &&
             (Timetable->View == Tmt_CRS_EDIT ||
              Timetable->View == Tmt_TUT_EDIT))
            ColumnsToDrawIncludingExtraColumn++;

         /* Draw cells */
         for (Column = 0, ContinuousFreeMinicolumns = 0;
              Column < ColumnsToDrawIncludingExtraColumn;
              Column++)
            if (Tmt_TimeTable[Weekday][Interval].Columns[Column].IntervalType == Tmt_FREE_INTERVAL)
               ContinuousFreeMinicolumns += Tmt_NUM_MINICOLUMNS_PER_DAY /
                                            ColumnsToDrawIncludingExtraColumn;
            else
              {
               if (ContinuousFreeMinicolumns)
                 {
                  Tmt_TimeTableDrawCell (Timetable,
                                        Weekday,Interval,Column - 1,ContinuousFreeMinicolumns,
                                        -1L,Tmt_FREE_INTERVAL,Tmt_FREE,0,-1L,NULL);
                  ContinuousFreeMinicolumns = 0;
                 }
               Tmt_TimeTableDrawCell (Timetable,
                                     Weekday,Interval,Column,
                                     Tmt_NUM_MINICOLUMNS_PER_DAY /
                                     ColumnsToDrawIncludingExtraColumn,
	                             Tmt_TimeTable[Weekday][Interval].Columns[Column].CrsCod,
				     Tmt_TimeTable[Weekday][Interval].Columns[Column].IntervalType,
	                             Tmt_TimeTable[Weekday][Interval].Columns[Column].ClassType,
                                     Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals,
	                             Tmt_TimeTable[Weekday][Interval].Columns[Column].GrpCod,
                                     Tmt_TimeTable[Weekday][Interval].Columns[Column].Info);
              }
         if (ContinuousFreeMinicolumns)
            Tmt_TimeTableDrawCell (Timetable,
                                  Weekday,Interval,Column - 1,ContinuousFreeMinicolumns,
                                  -1L,Tmt_FREE_INTERVAL,Tmt_FREE,0,-1L,NULL);
        }

      /* Empty column used to adjust height */
      Tmt_DrawCellAlignTimeTable ();

      /* Right hour:minutes cell */
      if (Interval % 2)
	 Tmt_TimeTableDrawHourCell (Timetable->Config.Range.Hours.Start +
	                           (Interval + 2) / Timetable->Config.IntervalsPerHour,
	                           Min,
	                           "LM");

      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   if (Gbl.Action.Act == ActEdiCrsTT ||
       Gbl.Action.Act == ActChgCrsTT)
      Grp_FreeListGrpTypesAndGrps ();

   /***** Row with day names *****/
   HTM_TR_Begin (NULL);
   Tmt_DrawCellAlignTimeTable ();
   Tmt_TimeTableDrawDaysCells ();
   Tmt_DrawCellAlignTimeTable ();
   HTM_TR_End ();

   /***** Bottom row used for column adjustement *****/
   Tmt_TimeTableDrawAdjustRow ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Draw a row used for column adjustement in a time table ***********/
/*****************************************************************************/

static void Tmt_TimeTableDrawAdjustRow (void)
  {
   unsigned Weekday;
   unsigned Minicolumn;

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"Tmt_HOURCOL\"");
   HTM_TD_End ();

   Tmt_DrawCellAlignTimeTable ();
   for (Weekday = 0;
	Weekday < Tmt_DAYS_PER_WEEK;
	Weekday++)
      for (Minicolumn = 0;
	   Minicolumn < Tmt_NUM_MINICOLUMNS_PER_DAY;
	   Minicolumn++)
	{
         HTM_TD_Begin ("class=\"Tmt_MINICOL\"");
         HTM_TD_End ();
	}
   Tmt_DrawCellAlignTimeTable ();

   HTM_TD_Begin ("class=\"Tmt_HOURCOL\"");
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Draw cells with day names in a time table ****************/
/*****************************************************************************/

static void Tmt_TimeTableDrawDaysCells (void)
  {
   extern const char *Txt_DAYS_CAPS[7];
   unsigned DayColumn;
   unsigned Weekday;

   for (DayColumn = 0;
	DayColumn < Tmt_DAYS_PER_WEEK;
	DayColumn++)
     {
      Weekday = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;
      HTM_TD_Begin ("colspan=\"%u\" class=\"%s CM\" style=\"width:%u%%;\"",
		    Tmt_NUM_MINICOLUMNS_PER_DAY,
		    Weekday == 6 ? "Tmt_SUNDAY" :	// Sunday drawn in red
				   "Tmt_DAY",	// Monday to Saturday
		    Tmt_PERCENT_WIDTH_OF_A_DAY);
      HTM_Txt (Txt_DAYS_CAPS[Weekday]);
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/****************** Draw cells with day names in a time table ****************/
/*****************************************************************************/

static void Tmt_TimeTableDrawHourCell (unsigned Hour,unsigned Min,const char *Align)
  {
   HTM_TD_Begin ("rowspan=\"2\" class=\"Tmt_HOUR %s %s\"",
		 Min ? "Tmt_HOUR_SMALL" :
		       "Tmt_HOUR_BIG",
		 Align);
   HTM_TxtF ("%02u:%02u",Hour,Min);
   HTM_TD_End ();
  }

/*****************************************************************************/
/**** Calculate recursively number of columns to draw for a day and hour *****/
/*****************************************************************************/

static unsigned Tmt_CalculateColsToDrawInCell (const struct Tmt_Timetable *Timetable,
					       bool TopCall,
                                               unsigned Weekday,unsigned Interval)
  {
   unsigned ColumnsToDraw;
   unsigned Column;
   unsigned i;
   unsigned FirstHour;
   unsigned Cols;
   static bool *Tmt_IntervalsChecked;

   if (TopCall)	// Top call, non recursive call
      /****** Allocate space to store list of intervals already checked
              and initialize to false by using calloc *****/
      if ((Tmt_IntervalsChecked = calloc (Timetable->Config.IntervalsPerDay,
                                         sizeof (*Tmt_IntervalsChecked))) == NULL)
         Lay_NotEnoughMemoryExit ();

   ColumnsToDraw = Tmt_TimeTable[Weekday][Interval].NumColumns;

   if (!Tmt_IntervalsChecked[Interval])
     {
      Tmt_IntervalsChecked[Interval] = true;
      for (Column = 0;
	   Column < Tmt_MAX_COLUMNS_PER_CELL;
	   Column++)
         switch (Tmt_TimeTable[Weekday][Interval].Columns[Column].IntervalType)
           {
            case Tmt_FREE_INTERVAL:
               break;
            case Tmt_FIRST_INTERVAL:
               /* Check from first hour (this one) to last hour
                  searching maximum number of columns */
               for (i = Interval + 1;
        	    i < Interval + Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals;
        	    i++)
                  if (!Tmt_IntervalsChecked[i])
                    {
                     Cols = Tmt_CalculateColsToDrawInCell (Timetable,
                                                          false,	// Recursive call
                                                          Weekday,i);
                     if (Cols > ColumnsToDraw)
                        ColumnsToDraw = Cols;
                    }
               break;
            case Tmt_NEXT_INTERVAL:
               /* Find first hour for this item (class) */
               for (FirstHour = Interval;
        	    Tmt_TimeTable[Weekday][FirstHour].Columns[Column].IntervalType == Tmt_NEXT_INTERVAL;
        	    FirstHour--);

               /* Check from first hour to last hour
                  searching maximum number of columns */
               for (i = FirstHour;
        	    i < FirstHour + Tmt_TimeTable[Weekday][FirstHour].Columns[Column].DurationIntervals;
        	    i++)
                  if (!Tmt_IntervalsChecked[i])
                    {
                     Cols = Tmt_CalculateColsToDrawInCell (Timetable,
                                                          false,	// Recursive call
                                                          Weekday,i);
                     if (Cols > ColumnsToDraw)
                        ColumnsToDraw = Cols;
                    }
               break;
           }
     }

   if (TopCall)	// Top call, non recursive call
      /****** Free space used by list of intervals already checked *****/
      free (Tmt_IntervalsChecked);

   return ColumnsToDraw;
  }

/*****************************************************************************/
/******************** Write empty cell for alignment *************************/
/*****************************************************************************/

static void Tmt_DrawCellAlignTimeTable (void)
  {
   HTM_TD_Begin ("class=\"Tmt_ALIGN\"");
   HTM_TD_End ();
  }

/*****************************************************************************/
/*************************** Write a timetable cell **************************/
/*****************************************************************************/

static void Tmt_TimeTableDrawCell (const struct Tmt_Timetable *Timetable,
				   unsigned Weekday,unsigned Interval,unsigned Column,unsigned ColSpan,
                                   long CrsCod,Tmt_IntervalType_t IntervalType,Tmt_ClassType_t ClassType,
                                   unsigned DurationNumIntervals,long GrpCod,const char *Info)
  {
   extern const char *Txt_unknown_removed_course;
   extern const char *Txt_TIMETABLE_CLASS_TYPES[Tmt_NUM_CLASS_TYPES];
   extern const char *Txt_Group;
   extern const char *Txt_All_groups;
   extern const char *Txt_Info;
   static const char *TimeTableClasses[Tmt_NUM_CLASS_TYPES] =
     {
      [Tmt_FREE     ] = "Tmt_FREE",	// free hour
      [Tmt_LECTURE  ] = "Tmt_LECT",	// lecture class
      [Tmt_PRACTICAL] = "Tmt_PRAC",	// practical class
      [Tmt_TUTORING ] = "Tmt_TUTO",	// tutoring/office hour
     };
   char *CellStr;	// Unique string for this cell used in labels
   struct GroupData GrpDat;
   unsigned NumGrpTyp;
   unsigned NumGrp;
   unsigned i;
   unsigned Dur;
   unsigned MaxDuration;
   char *TTDur;
   unsigned RowSpan = 0;
   char *RowSpanStr;
   char *ColSpanStr;
   char *ClassStr;
   Tmt_ClassType_t CT;
   struct Crs_Course Crs;
   struct GroupType *GrpTyp;
   struct Group *Grp;
   char *Room;

   /***** Compute row span and background color depending on hour type *****/
   switch (IntervalType)
     {
      case Tmt_FREE_INTERVAL:	// Free cell written
	 RowSpan = 1;
	 break;
      case Tmt_FIRST_INTERVAL:	// Normal cell written
	 RowSpan = DurationNumIntervals;
	 break;
      case Tmt_NEXT_INTERVAL:	// Nothing written
	 break;
     }

   /***** If there's nothing to do... *****/
   if (RowSpan == 0)
      return;

   /***** If group code > 0, a group is selected ==> get group data *****/
   if (IntervalType == Tmt_FIRST_INTERVAL &&
       (Timetable->View == Tmt_CRS_VIEW ||
        Timetable->View == Tmt_CRS_EDIT) &&
       GrpCod > 0)
     {
      /* Get group data */
      GrpDat.GrpCod = GrpCod;
      Grp_GetDataOfGroupByCod (&GrpDat);
     }

   /***** Cell start *****/
   /* Create rowspan, colspan and class strings */
   if (RowSpan > 1)
     {
      if (asprintf (&RowSpanStr,"rowspan=\"%u\" ",RowSpan) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&RowSpanStr,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   if (ColSpan > 1)
     {
      if (asprintf (&ColSpanStr,"colspan=\"%u\" ",ColSpan) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&ColSpanStr,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   if (ClassType == Tmt_FREE)
     {
      if (asprintf (&ClassStr,"%s%u",TimeTableClasses[ClassType],Interval % 4) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&ClassStr,"%s CM DAT_SMALL",TimeTableClasses[ClassType]) < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /* Begin cell */
   HTM_TD_Begin ("%s%sclass=\"%s\"",RowSpanStr,ColSpanStr,ClassStr);

   /* Free allocated memory for rowspan, colspan and class strings */
   free (RowSpanStr);
   free (ColSpanStr);
   free (ClassStr);

   /***** Form to modify this cell *****/
   if (Timetable->View == Tmt_CRS_EDIT)
      Frm_BeginForm (ActChgCrsTT);
   else if (Timetable->View == Tmt_TUT_EDIT)
      Frm_BeginForm (ActChgTut);

   /***** Draw cell depending on type of view *****/
   switch (Timetable->View)
     {
      case Tmt_CRS_VIEW:	// View course timetable
      case Tmt_TUT_VIEW:	// View tutoring timetable
	 if (IntervalType != Tmt_FREE_INTERVAL) // If cell is not empty...
	   {
	    /***** Begin cell *****/
	    HTM_DIV_Begin ("class=\"Tmt_CELL Tmt_TXT\"");

	    /***** Course name *****/
	    if (Timetable->Type == Tmt_MY_TIMETABLE)
              {
               Crs.CrsCod = CrsCod;
               Crs_GetDataOfCourseByCod (&Crs);
               if (ClassType == Tmt_LECTURE ||
                   ClassType == Tmt_PRACTICAL)
        	 {
		  HTM_Txt (Crs.ShrtName[0] ? Crs.ShrtName :
			                     Txt_unknown_removed_course);
		  HTM_BR ();
        	 }
              }

	    /***** Type of class and duration *****/
	    HTM_TxtF ("%s (%u:%02u)",
		      Txt_TIMETABLE_CLASS_TYPES[ClassType],
	              (DurationNumIntervals / Timetable->Config.IntervalsPerHour),	// Hours
	              (DurationNumIntervals % Timetable->Config.IntervalsPerHour) *
	              Timetable->Config.Range.MinutesPerInterval);			// Minutes

	    /***** Group *****/
	    if (Timetable->View == Tmt_CRS_VIEW &&
		GrpCod > 0)
	      {
	       HTM_BR ();
	       HTM_Txt (GrpDat.GrpTypName);
	       HTM_BR ();
	       HTM_Txt (GrpDat.GrpName);
	       if (GrpDat.Room.RooCod > 0)
		 {
		  HTM_BR ();
		  HTM_TxtF ("(%s)",GrpDat.Room.ShrtName);
		 }
	      }

	    /***** Info *****/
	    if (Info)
	       if (Info[0])
		 {
		  HTM_BR ();
	          HTM_Txt (Info);
		 }

	    /***** End cell *****/
            HTM_DIV_End ();
	   }
	 break;
      case Tmt_CRS_EDIT:
      case Tmt_TUT_EDIT:
	 /***** Create unique string for this cell used in labels *****/
	 if (asprintf (&CellStr,"%02u%02u%02u",
		       Weekday,Interval,Column) < 0)
	    Lay_NotEnoughMemoryExit ();

	 /***** Put hidden parameters *****/
         Par_PutHiddenParamUnsigned (NULL,"TTDay",Weekday);
         Par_PutHiddenParamUnsigned (NULL,"TTInt",Interval);
         Par_PutHiddenParamUnsigned (NULL,"TTCol",Column);

	 /***** Class type *****/
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "name=\"TTTyp\" class=\"Tmt_TYP\"");
	 for (CT  = (Tmt_ClassType_t) 0;
	      CT <= (Tmt_ClassType_t) (Tmt_NUM_CLASS_TYPES - 1);
	      CT++)
	    if ((CT == Tmt_FREE) ||
		((Timetable->View == Tmt_CRS_EDIT) && (CT == Tmt_LECTURE || CT == Tmt_PRACTICAL)) ||
		((Timetable->View == Tmt_TUT_EDIT) && (CT == Tmt_TUTORING)))
	       HTM_OPTION (HTM_Type_STRING,Tmt_ClassTypeDB[CT],
			   CT == ClassType,false,
			   "%s",Txt_TIMETABLE_CLASS_TYPES[CT]);
	 HTM_SELECT_End ();

	 if (IntervalType == Tmt_FREE_INTERVAL)
	   {
            for (i = Interval + 1;
        	 i < Timetable->Config.IntervalsPerDay;
        	 i++)
              if (Tmt_TimeTable[Weekday][i].NumColumns == Tmt_MAX_COLUMNS_PER_CELL)
                  break;
            MaxDuration = i - Interval;
	    Dur = (MaxDuration >= Timetable->Config.IntervalsPerHour) ? Timetable->Config.IntervalsPerHour :	// MaxDuration >= 1h ==> Dur = 1h
	                                                                MaxDuration;				// MaxDuration  < 1h ==> Dur = MaxDuration
	    if (asprintf (&TTDur,"%u:%02u",
		          (Dur / Timetable->Config.IntervalsPerHour),		// Hours
		          (Dur % Timetable->Config.IntervalsPerHour) *
		          Timetable->Config.Range.MinutesPerInterval) < 0)	// Minutes
	       Lay_NotEnoughMemoryExit ();
	    Par_PutHiddenParamString (NULL,"TTDur",TTDur);
	    free (TTDur);
	   }
	 else
	   {
	    /***** Class duration *****/
	    HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			      "name=\"TTDur\" class=\"Tmt_DUR\"");
            for (i = Interval + Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals;
        	 i < Timetable->Config.IntervalsPerDay;
        	 i++)
               if (Tmt_TimeTable[Weekday][i].NumColumns == Tmt_MAX_COLUMNS_PER_CELL)
                  break;
            MaxDuration = i - Interval;
            if (Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals > MaxDuration)
               MaxDuration = Tmt_TimeTable[Weekday][Interval].Columns[Column].DurationIntervals;
	    for (Dur = 0;
		 Dur <= MaxDuration;
		 Dur++)
	      {
	       if (asprintf (&TTDur,"%u:%02u",
			     (Dur / Timetable->Config.IntervalsPerHour),	// Hours
			     (Dur % Timetable->Config.IntervalsPerHour) *
			     Timetable->Config.Range.MinutesPerInterval) < 0)	// Minutes
		  Lay_NotEnoughMemoryExit ();
	       HTM_OPTION (HTM_Type_STRING,TTDur,
			   Dur == DurationNumIntervals,false,
			   "%s",TTDur);
	       free (TTDur);
	      }
	    HTM_SELECT_End ();

	    if (Timetable->View == Tmt_CRS_EDIT)
	      {
	       /***** Group *****/
	       HTM_BR ();
	       HTM_LABEL_Begin ("for=\"TTGrp%s\"",CellStr);
	       HTM_Txt (Txt_Group);
	       HTM_LABEL_End ();
	       HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
				 "id=\"TTGrp%s\" name=\"TTGrp\""
				 " class=\"Tmt_GRP\"",
			         CellStr);
	       HTM_OPTION (HTM_Type_STRING,"-1",GrpCod <= 0,false,
			   "%s",Txt_All_groups);
               for (NumGrpTyp = 0;
        	    NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
        	    NumGrpTyp++)
                 {
                  GrpTyp = &Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp];

                  for (NumGrp = 0;
                       NumGrp < GrpTyp->NumGrps;
                       NumGrp++)
                    {
                     Grp = &GrpTyp->LstGrps[NumGrp];
		     if (Grp->Room.RooCod > 0)
		       {
			if (asprintf (&Room," (%s)",Grp->Room.ShrtName) < 0)
			   Lay_NotEnoughMemoryExit ();
		       }
		     else
		       {
			if (asprintf (&Room,"%s","") < 0)
			   Lay_NotEnoughMemoryExit ();
		       }
		     HTM_OPTION (HTM_Type_LONG,&Grp->GrpCod,
				 GrpCod == Grp->GrpCod,false,
				 "%s %s%s",
				 GrpTyp->GrpTypName,Grp->GrpName,Room);
	             free (Room);
                    }
                 }
	       HTM_SELECT_End ();

	       /***** Info *****/
	       HTM_BR ();
	       HTM_LABEL_Begin ("for=\"TTInf%s\"",CellStr);
	       HTM_Txt (Txt_Info);
	       HTM_LABEL_End ();
	       HTM_INPUT_TEXT ("TTInf",Tmt_MAX_CHARS_INFO,Info ? Info :
			                                        "",
			       HTM_SUBMIT_ON_CHANGE,
	                       "id=\"TTInf%s\" size=\"1\" class=\"Tmt_INF\"",
			       CellStr);
	      }
	    else // TimeTableView == Tmt_TUT_EDIT
	      {
	       /***** Info *****/
	       HTM_BR ();
	       HTM_LABEL_Begin ("for=\"TTInf%s\" class=\"DAT_SMALL\"",CellStr);
	       HTM_Txt (Txt_Info);
	       HTM_LABEL_End ();
	       HTM_INPUT_TEXT ("TTInf",Tmt_MAX_CHARS_INFO,Info,
	                       HTM_SUBMIT_ON_CHANGE,
	                       "id=\"TTInf%s\" size=\"12\" class=\"Tmt_INF\"",
			       CellStr);
	      }
	   }

	 /***** Free allocated unique string for this cell used in labels *****/
	 free (CellStr);

	 break;
     }

   /***** End form *****/
   if (Timetable->View == Tmt_CRS_EDIT ||
       Timetable->View == Tmt_TUT_EDIT)
      Frm_EndForm ();

   /***** End cell *****/
   HTM_TD_End ();
  }
