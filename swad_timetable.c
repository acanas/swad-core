// swad_timetable.c: timetables

/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For malloc, calloc, free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_setting.h"
#include "swad_timetable.h"
#include "swad_timetable_database.h"

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

#define Tmt_MAX_BYTES_STR_CLASS_TYPE		256
#define Tmt_MAX_BYTES_STR_DURATION		 32	// "hh:mm h"

struct Tmt_Cell
  {
   unsigned NumColumns;
   struct Tmt_Column Columns[Tmt_MAX_COLUMNS_PER_CELL];
  };

/*****************************************************************************/
/************************** Private global variables *************************/
/*****************************************************************************/

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
static void Tmt_GetParsTimeTable (struct Tmt_Timetable *Timetable);
static void Tmt_PutContextualIcons (void *Timetable);
static void Tmt_PutFormToSelectWhichGroupsToShow (const struct Tmt_Timetable *Timetable);

static void Tmt_PutIconToViewCrsTT (__attribute__((unused)) void *Args);
static void Tmt_PutIconToViewMyTT (__attribute__((unused)) void *Args);

static void Tmt_WriteCrsTimeTableIntoDB (const struct Tmt_Timetable *Timetable,
                                         long CrsCod);
static void Tmt_WriteUsrTimeTableIntoDB (const struct Tmt_Timetable *Timetable,
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
static void Tmt_DrawHourCell (unsigned Hour,unsigned Min,const char *Align);
static unsigned Tmt_CalculateColsToDrawInCell (const struct Tmt_Timetable *Timetable,
					       bool TopCall,
                                               unsigned Weekday,unsigned Interval);
static void Tmt_DrawCellAlignTimeTable (void);
static void Tmt_TimeTableDrawCell (const struct Tmt_Timetable *Timetable,
				   const struct Tmt_WhichCell *WhichCell,unsigned ColSpan,
                                   long CrsCod,long GrpCod,
                                   Tmt_IntervalType_t IntervalType,Tmt_ClassType_t ClassType,
                                   unsigned DurationNumIntervals,const char *Info);
static void Tmt_TimeTableDrawCellView (const struct Tmt_Timetable *Timetable,
                                       long CrsCod,long GrpCod,
                                       Tmt_ClassType_t ClassType,
                                       unsigned DurationNumIntervals,
                                       const char *Info);
static void Tmt_TimeTableDrawCellEdit (const struct Tmt_Timetable *Timetable,
				       const struct Tmt_WhichCell *WhichCell,
                                       long GrpCod,
                                       Tmt_IntervalType_t IntervalType,Tmt_ClassType_t ClassType,
                                       unsigned DurationNumIntervals,const char *Info);

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
            Err_NotEnoughMemoryExit ();
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

   HTM_DIV_Begin ("class=\"CLASSPHOTO_TITLE CLASSPHOTO_%s CM\"",
                  The_GetSuffix ());

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

static void Tmt_GetParsTimeTable (struct Tmt_Timetable *Timetable)
  {
   extern const char *Tmt_DB_ClassType[Tmt_NUM_CLASS_TYPES];
   char StrClassType[Tmt_MAX_BYTES_STR_CLASS_TYPE + 1];
   char StrDuration[Tmt_MAX_BYTES_STR_DURATION + 1];
   unsigned Hours;
   unsigned Minutes;

   /***** Get day (0: monday, 1: tuesday,..., 6: sunday *****/
   Timetable->WhichCell.Weekday = (unsigned)
				  Par_GetParUnsignedLong ("TTDay",
							  0,
							  Tmt_DAYS_PER_WEEK - 1,
							  0);

   /***** Get hour *****/
   Timetable->WhichCell.Interval = (unsigned)
				   Par_GetParUnsignedLong ("TTInt",
							   0,
							   Timetable->Config.IntervalsPerDay - 1,
							   0);

   /***** Get number of column *****/
   Timetable->WhichCell.Column = (unsigned)
				 Par_GetParUnsignedLong ("TTCol",
							 0,
							 Tmt_MAX_COLUMNS_PER_CELL - 1,
							 0);

   /***** Get class type *****/
   Par_GetParText ("TTTyp",StrClassType,Tmt_MAX_BYTES_STR_CLASS_TYPE);
   for (Timetable->ClassType  = (Tmt_ClassType_t) 0;
	Timetable->ClassType <= (Tmt_ClassType_t) (Tmt_NUM_CLASS_TYPES - 1);
	Timetable->ClassType++)
      if (!strcmp (StrClassType,Tmt_DB_ClassType[Timetable->ClassType]))
         break;
   if (Timetable->ClassType > (Tmt_ClassType_t) (Tmt_NUM_CLASS_TYPES - 1))
      Err_ShowErrorAndExit ("Type of timetable cell is missing.");

   /***** Get class duration *****/
   Par_GetParText ("TTDur",StrDuration,Tmt_MAX_BYTES_STR_DURATION);
   if (sscanf (StrDuration,"%u:%u",&Hours,&Minutes) != 2)
      Err_ShowErrorAndExit ("Duration is missing.");
   Timetable->DurationIntervals = Hours * Timetable->Config.IntervalsPerHour +
	                          Minutes / Timetable->Config.Range.MinutesPerInterval;

   /***** Get group code *****/
   Timetable->GrpCod = ParCod_GetPar (ParCod_Grp);

   /***** Get info *****/
   Par_GetParText ("TTInf",Timetable->Info,Tmt_MAX_BYTES_INFO);
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
   Vie_ViewType_t ViewType = (Gbl.Action.Act == ActPrnCrsTT ||
	                      Gbl.Action.Act == ActPrnMyTT) ? Vie_PRINT :
	                				      Vie_VIEW;
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
	 Err_WrongActionExit ();
     }

   Timetable.ContextualIcons.PutIconEditCrsTT = (Timetable.Type == Tmt_COURSE_TIMETABLE &&
	                                         ViewType == Vie_VIEW &&
                                                 Gbl.Usrs.Me.Role.Logged >= Rol_TCH);
   Timetable.ContextualIcons.PutIconEditOfficeHours = (Timetable.Type == Tmt_MY_TIMETABLE &&
	                                               ViewType == Vie_VIEW &&
                                                       (Gbl.Usrs.Me.Role.Available & (1 << Rol_TCH |
                                                		                      1 << Rol_NET)));
   Timetable.ContextualIcons.PutIconPrint = (ViewType == Vie_VIEW);

   /***** Get whether to show only my groups or all groups *****/
   Grp_GetParWhichGroups ();

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

   /***** Begin time table drawing *****/
   if (Timetable.Type == Tmt_COURSE_TIMETABLE)
      Lay_WriteHeaderClassPhoto (ViewType,false);

   switch (ViewType)
     {
      case Vie_VIEW:
	 /***** Setting selector *****/
	 Set_BeginSettingsHead ();

	 /* Select whether show only my groups or all groups */
	 if ( Timetable.Type == Tmt_MY_TIMETABLE ||
	     (Timetable.Type == Tmt_COURSE_TIMETABLE &&
	      Gbl.Crs.Grps.NumGrps))
	    Tmt_PutFormToSelectWhichGroupsToShow (&Timetable);

	 /* Show form to change first day of week */
	 WhichGroups = Grp_GetParWhichGroups ();
	 Cal_ShowFormToSelFirstDayOfWeek (ActChgTT1stDay[Timetable.Type],
					  Grp_PutParWhichGroups,&WhichGroups);

	 Set_EndSettingsHead ();
	 break;
      case Vie_PRINT:
	 /***** Show whether only my groups or all groups are selected *****/
	 Tmt_ShowTimeTableGrpsSelected ();
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
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
      WhichGroups = Grp_GetParWhichGroups ();

      if (((struct Tmt_Timetable *) Timetable)->ContextualIcons.PutIconEditCrsTT)
	 Ico_PutContextualIconToEdit (ActEdiCrsTT,NULL,
				      Grp_PutParWhichGroups,&WhichGroups);

      if (((struct Tmt_Timetable *) Timetable)->ContextualIcons.PutIconEditOfficeHours)
	 Ico_PutContextualIconToEdit (ActEdiTut,NULL,
				      NULL,NULL);

      if (((struct Tmt_Timetable *) Timetable)->ContextualIcons.PutIconPrint)
	 Ico_PutContextualIconToPrint (((struct Tmt_Timetable *) Timetable)->Type == Tmt_COURSE_TIMETABLE ? ActPrnCrsTT :
										                            ActPrnMyTT,
				       Grp_PutParWhichGroups,&WhichGroups);
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
   Ico_PutContextualIconToView (ActSeeCrsTT,NULL,
				NULL,NULL);
  }

/*****************************************************************************/
/************************ Put icon to view my timetable **********************/
/*****************************************************************************/

static void Tmt_PutIconToViewMyTT (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToView (ActSeeMyTT,NULL,
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
      Timetable->Config.Range.Hours.End	         = Tmt_END_HOUR;		// Day  ends  at this hour
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
      Tmt_GetParsTimeTable (Timetable);

      /* Modify timetable in memory */
      Tmt_ModifTimeTable (Timetable);

      /* Write a new timetable in database */
      switch (Timetable->Type)
        {
         case Tmt_COURSE_TIMETABLE:
            Tmt_WriteCrsTimeTableIntoDB (Timetable,Gbl.Hierarchy.Node[Hie_CRS].HieCod);
	    break;
         case Tmt_TUTORING_TIMETABLE:
            Tmt_WriteUsrTimeTableIntoDB (Timetable,UsrCod);
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
   Tmt_DB_RemoveCrsTimeTable (CrsCod);

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
	       Tmt_DB_InsertHourInCrsTimeTable (CrsCod,
	                                        &Tmt_TimeTable[Weekday][Interval].Columns[Column],
                                                Weekday,Hour,Min,
                                                Timetable->Config.SecondsPerInterval);
  }

/*****************************************************************************/
/********************* Write tutor timetable into database *******************/
/*****************************************************************************/

static void Tmt_WriteUsrTimeTableIntoDB (const struct Tmt_Timetable *Timetable,
                                         long UsrCod)
  {
   unsigned Weekday;
   unsigned Interval;
   unsigned Hour;
   unsigned Min;
   unsigned Column;

   /***** Remove former timetable *****/
   Tmt_DB_RemoveUsrTimeTable (UsrCod);

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
   	       Tmt_DB_InsertHourInUsrTimeTable (UsrCod,
	                                        &Tmt_TimeTable[Weekday][Interval].Columns[Column],
                                                Weekday,Hour,Min,
                                                Timetable->Config.SecondsPerInterval);
  }

/*****************************************************************************/
/********** Create an internal table with timetable from database ************/
/*****************************************************************************/

static void Tmt_FillTimeTableFromDB (struct Tmt_Timetable *Timetable,
                                     long UsrCod)
  {
   extern const char *Tmt_DB_ClassType[Tmt_NUM_CLASS_TYPES];
   extern const char *Txt_Incomplete_timetable_for_lack_of_space;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows = 0;	// Initialized to avoid warning
   unsigned NumRow;
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
   NumRows = Tmt_DB_GetTimeTable (&mysql_res,Timetable->Type,UsrCod);

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
	    Err_ShowErrorAndExit ("Wrong start time in timetable.");

	 /* Duration formatted as seconds (row[2]) */
	 if (sscanf (row[2],"%u",&DurationSeconds) != 1)
	    Err_ShowErrorAndExit ("Wrong duration in timetable.");
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
	    Err_ShowErrorAndExit ("Wrong day of week in timetable.");
	 if (Weekday >= Tmt_DAYS_PER_WEEK)
	    Err_ShowErrorAndExit ("Wrong day of week in timetable.");

	 /* StartTime formatted as seconds (row[1])
	    --> StartTime in number of intervals */
	 if (sscanf (row[1],"%u",&StartTimeSeconds) != 1)
	    Err_ShowErrorAndExit ("Wrong start time in timetable.");
	 Interval = StartTimeSeconds /
	            Timetable->Config.SecondsPerInterval;
	 if (Interval < Timetable->Config.IntervalsBeforeStartHour)
	    Err_ShowErrorAndExit ("Wrong start time in timetable.");
	 Interval -= Timetable->Config.IntervalsBeforeStartHour;

	 /* Duration formatted as seconds (row[2])
	    --> Duration in number of intervals */
	 if (sscanf (row[2],"%u",&DurationSeconds) != 1)
	    Err_ShowErrorAndExit ("Wrong duration in timetable.");
	 DurationNumIntervals = DurationSeconds /
	                        Timetable->Config.SecondsPerInterval;

	 /* Type of class (row[4]) */
	 switch (Timetable->Type)
	   {
	    case Tmt_COURSE_TIMETABLE:
	    case Tmt_MY_TIMETABLE:
	       for (ClassType  = Tmt_LECTURE, Found = false;
		    ClassType <= Tmt_TUTORING;
		    ClassType++)
		  if (!strcmp (row[4],Tmt_DB_ClassType[ClassType]))
		    {
		     Found = true;
		     break;
		    }
	       if (!Found)
		  Err_ShowErrorAndExit ("Wrong type of class in timetable.");
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
	       /* Check if there's place for all rows of this class */
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
								  Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
   if (Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].IntervalType == Tmt_FIRST_INTERVAL)
     {
      /***** Free this cell *****/
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].GrpCod            = -1L;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].IntervalType      = Tmt_FREE_INTERVAL;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].ClassType         = Tmt_FREE;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].DurationIntervals = 0;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].Info[0]          = '\0';
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].NumColumns--;
     }

   if (Timetable->ClassType != Tmt_FREE &&
       Timetable->DurationIntervals > 0 &&
       Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].NumColumns < Tmt_MAX_COLUMNS_PER_CELL)
     {
      /***** Change this cell *****/
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].NumColumns++;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].GrpCod            = Timetable->GrpCod;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].IntervalType      = Tmt_FIRST_INTERVAL;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].ClassType         = Timetable->ClassType;
      Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].DurationIntervals = Timetable->DurationIntervals;
      Str_Copy (Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].Info,
                Timetable->Info,
                sizeof (Tmt_TimeTable[Timetable->WhichCell.Weekday][Timetable->WhichCell.Interval].Columns[Timetable->WhichCell.Column].Info) - 1);
     }
  }

/*****************************************************************************/
/********************* Draw timetable using internal table *******************/
/*****************************************************************************/

static void Tmt_DrawTimeTable (const struct Tmt_Timetable *Timetable)
  {
   unsigned DayColumn;	// Column from left (0) to right (6)
   struct Tmt_WhichCell WhichCell;
   unsigned Min;
   unsigned ColumnsToDraw;
   unsigned ColumnsToDrawIncludingExtraColumn;
   unsigned ContinuousFreeMinicolumns;

   /***** Begin table *****/
   HTM_TABLE_Begin ("TT");

      /***** Top row used for column adjustement *****/
      Tmt_TimeTableDrawAdjustRow ();

      /***** Row with day names *****/
      HTM_TR_Begin (NULL);
         Tmt_DrawHourCell (Timetable->Config.Range.Hours.Start,0,"RM");
	 Tmt_DrawCellAlignTimeTable ();
	 Tmt_TimeTableDrawDaysCells ();
	 Tmt_DrawCellAlignTimeTable ();
         Tmt_DrawHourCell (Timetable->Config.Range.Hours.Start,0,"LM");
      HTM_TR_End ();

      /***** Get list of groups types and groups in this course *****/
      if (Gbl.Action.Act == ActEdiCrsTT ||
	  Gbl.Action.Act == ActChgCrsTT)
	 Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

      /***** Write the table row by row *****/
      for (WhichCell.Interval = 0, Min = Timetable->Config.Range.MinutesPerInterval;
	   WhichCell.Interval < Timetable->Config.IntervalsPerDay;
	   WhichCell.Interval++,
	   Min = (Min + Timetable->Config.Range.MinutesPerInterval) %
		 Tmt_SECONDS_PER_MINUTE)
	{
	 HTM_TR_Begin (NULL);

	    /* Left hour:minutes cell */
	    if (WhichCell.Interval % 2)
	       Tmt_DrawHourCell (Timetable->Config.Range.Hours.Start +
				 (WhichCell.Interval + 2) / Timetable->Config.IntervalsPerHour,
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
	       WhichCell.Weekday = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;

	       /* Check how many colums are needed.
		  For each item (class) in this hour from left to right,
		  we must check the maximum of columns */
	       ColumnsToDraw = Tmt_CalculateColsToDrawInCell (Timetable,
							      true,	// Top call, non recursive
							      WhichCell.Weekday,WhichCell.Interval);
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
	       for (WhichCell.Column = 0, ContinuousFreeMinicolumns = 0;
		    WhichCell.Column < ColumnsToDrawIncludingExtraColumn;
		    WhichCell.Column++)
		  if (Tmt_TimeTable[WhichCell.Weekday][WhichCell.Interval].Columns[WhichCell.Column].IntervalType == Tmt_FREE_INTERVAL)
		     ContinuousFreeMinicolumns += Tmt_NUM_MINICOLUMNS_PER_DAY /
						  ColumnsToDrawIncludingExtraColumn;
		  else
		    {
		     if (ContinuousFreeMinicolumns)
		       {
			WhichCell.Column--;
			Tmt_TimeTableDrawCell (Timetable,
					       &WhichCell,ContinuousFreeMinicolumns,
					       -1L,-1L,Tmt_FREE_INTERVAL,Tmt_FREE,0,NULL);
			WhichCell.Column++;
			ContinuousFreeMinicolumns = 0;
		       }
		     Tmt_TimeTableDrawCell (Timetable,
					    &WhichCell,
					    Tmt_NUM_MINICOLUMNS_PER_DAY /
					    ColumnsToDrawIncludingExtraColumn,
					    Tmt_TimeTable[WhichCell.Weekday][WhichCell.Interval].Columns[WhichCell.Column].CrsCod,
					    Tmt_TimeTable[WhichCell.Weekday][WhichCell.Interval].Columns[WhichCell.Column].GrpCod,
					    Tmt_TimeTable[WhichCell.Weekday][WhichCell.Interval].Columns[WhichCell.Column].IntervalType,
					    Tmt_TimeTable[WhichCell.Weekday][WhichCell.Interval].Columns[WhichCell.Column].ClassType,
					    Tmt_TimeTable[WhichCell.Weekday][WhichCell.Interval].Columns[WhichCell.Column].DurationIntervals,
					    Tmt_TimeTable[WhichCell.Weekday][WhichCell.Interval].Columns[WhichCell.Column].Info);
		    }
	       if (ContinuousFreeMinicolumns)
		 {
		  WhichCell.Column--;
		  Tmt_TimeTableDrawCell (Timetable,
					 &WhichCell,ContinuousFreeMinicolumns,
					 -1L,-1L,Tmt_FREE_INTERVAL,Tmt_FREE,0,NULL);
		  WhichCell.Column++;
		 }
	      }

	    /* Empty column used to adjust height */
	    Tmt_DrawCellAlignTimeTable ();

	    /* Right hour:minutes cell */
	    if (WhichCell.Interval % 2)
	       Tmt_DrawHourCell (Timetable->Config.Range.Hours.Start +
				 (WhichCell.Interval + 2) / Timetable->Config.IntervalsPerHour,
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
   extern const char *Txt_DAYS[7];
   unsigned DayColumn;
   unsigned Weekday;

   for (DayColumn = 0;
	DayColumn < Tmt_DAYS_PER_WEEK;
	DayColumn++)
     {
      Weekday = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;
      HTM_TD_Begin ("colspan=\"%u\" class=\"Tmt_DAY %s%s CM\" style=\"width:%u%%;\"",
		    Tmt_NUM_MINICOLUMNS_PER_DAY,
		    Weekday == 6 ? "Tmt_SUNDAY_" :	// Sunday drawn in red
				   "Tmt_DAY_",		// Monday to Saturday
		    The_GetSuffix (),
		    Tmt_PERCENT_WIDTH_OF_A_DAY);
	 HTM_Txt (Txt_DAYS[Weekday]);
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/****************** Draw cells with day names in a time table ****************/
/*****************************************************************************/

static void Tmt_DrawHourCell (unsigned Hour,unsigned Min,const char *Align)
  {
   HTM_TD_Begin ("rowspan=\"2\" class=\"Tmt_HOUR %s_%s %s\"",
		 Min ? "Tmt_HOURXX" :
		       "Tmt_HOUR00",
		 The_GetSuffix (),
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
         Err_NotEnoughMemoryExit ();

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
				   const struct Tmt_WhichCell *WhichCell,unsigned ColSpan,
                                   long CrsCod,long GrpCod,
                                   Tmt_IntervalType_t IntervalType,Tmt_ClassType_t ClassType,
                                   unsigned DurationNumIntervals,const char *Info)
  {
   extern const char *Txt_All_groups;
   static const char *TimeTableClasses[Tmt_NUM_CLASS_TYPES] =
     {
      [Tmt_FREE     ] = "Tmt_FREE",	// free hour
      [Tmt_LECTURE  ] = "Tmt_LECT",	// lecture class
      [Tmt_PRACTICAL] = "Tmt_PRAC",	// practical class
      [Tmt_TUTORING ] = "Tmt_TUTO",	// tutoring/office hour
     };
   unsigned RowSpan = 0;
   char *RowSpanStr;
   char *ColSpanStr;
   char *ClassStr;

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

   /***** Cell start *****/
   /* Create rowspan, colspan and class strings */
   if (RowSpan > 1)
     {
      if (asprintf (&RowSpanStr,"rowspan=\"%u\" ",RowSpan) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&RowSpanStr,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }
   if (ColSpan > 1)
     {
      if (asprintf (&ColSpanStr,"colspan=\"%u\" ",ColSpan) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&ColSpanStr,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }
   switch (ClassType)
     {
      case Tmt_FREE:
	 if (asprintf (&ClassStr,"Tmt_CELL Tmt_FREE%u_%s",
		       WhichCell->Interval % 4,
		       The_GetSuffix ()) < 0)
	    Err_NotEnoughMemoryExit ();
         break;
      default:
	 if (asprintf (&ClassStr,"Tmt_CELL %s",
		       TimeTableClasses[ClassType]) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
     }

   /* Begin cell */
   HTM_TD_Begin ("%s%sclass=\"%s\"",RowSpanStr,ColSpanStr,ClassStr);

   /* Free allocated memory for rowspan, colspan and class strings */
   free (RowSpanStr);
   free (ColSpanStr);
   free (ClassStr);

      /***** Draw cell depending on type of view *****/
      switch (Timetable->View)
	{
	 case Tmt_CRS_VIEW:	// View course timetable
	 case Tmt_TUT_VIEW:	// View tutoring timetable
	    if (IntervalType != Tmt_FREE_INTERVAL) // If cell is not empty...
	       Tmt_TimeTableDrawCellView (Timetable,
                                          CrsCod,GrpCod,
                                          ClassType,
                                          DurationNumIntervals,
                                          Info);
	    break;
	 case Tmt_CRS_EDIT:	// Edit course timetable
	 case Tmt_TUT_EDIT:	// Edit tutoring timetable
	    Tmt_TimeTableDrawCellEdit (Timetable,WhichCell,
                                       GrpCod,
                                       IntervalType,ClassType,
                                       DurationNumIntervals,
                                       Info);
	    break;
	}

   /***** End cell *****/
   HTM_TD_End ();
  }

static void Tmt_TimeTableDrawCellView (const struct Tmt_Timetable *Timetable,
                                       long CrsCod,long GrpCod,
                                       Tmt_ClassType_t ClassType,
                                       unsigned DurationNumIntervals,
                                       const char *Info)
  {
   extern const char *Txt_unknown_removed_course;
   extern const char *Txt_TIMETABLE_CLASS_TYPES[Tmt_NUM_CLASS_TYPES];
   struct Hie_Node Crs;
   struct GroupData GrpDat;

   /***** Begin cell *****/
   HTM_DIV_Begin ("class=\"Tmt_TXT\"");

      /***** Course name *****/
      if (Timetable->Type == Tmt_MY_TIMETABLE &&
	  (ClassType == Tmt_LECTURE ||
	   ClassType == Tmt_PRACTICAL))
	{
	 Crs.HieCod = CrsCod;
	 Crs_GetCourseDataByCod (&Crs);
	 HTM_Txt (Crs.ShrtName[0] ? Crs.ShrtName :
				    Txt_unknown_removed_course);
	 HTM_BR ();
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
	 GrpDat.GrpCod = GrpCod;
         Grp_GetGroupDataByCod (&GrpDat);
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

static void Tmt_TimeTableDrawCellEdit (const struct Tmt_Timetable *Timetable,
				       const struct Tmt_WhichCell *WhichCell,
                                       long GrpCod,
                                       Tmt_IntervalType_t IntervalType,Tmt_ClassType_t ClassType,
                                       unsigned DurationNumIntervals,const char *Info)
  {
   extern const char *Par_CodeStr[];
   extern const char *Tmt_DB_ClassType[Tmt_NUM_CLASS_TYPES];
   extern const char *Txt_TIMETABLE_CLASS_TYPES[Tmt_NUM_CLASS_TYPES];
   extern const char *Txt_Group;
   extern const char *Txt_All_groups;
   extern const char *Txt_Info;
   static const Act_Action_t NextAction[Tmt_NUM_VIEW_EDIT] =
     {
      [Tmt_CRS_VIEW] = ActUnk,		// course view
      [Tmt_TUT_VIEW] = ActUnk,		// tutorials view
      [Tmt_CRS_EDIT] = ActChgCrsTT,	// course edit
      [Tmt_TUT_EDIT] = ActChgTut,	// tutorials edit
     };
   char *CellStr;	// Unique string for this cell used in labels
   Tmt_ClassType_t CT;
   unsigned i;
   unsigned Dur;
   unsigned MaxDuration;
   char *TTDur;
   unsigned NumGrpTyp;
   unsigned NumGrp;
   struct GroupType *GrpTyp;
   struct Group *Grp;
   char *Room;

   /***** Form to modify this cell *****/
   Frm_BeginForm (NextAction[Timetable->View]);

      /***** Put hidden parameters *****/
      Par_PutParUnsigned (NULL,"TTDay",WhichCell->Weekday );
      Par_PutParUnsigned (NULL,"TTInt",WhichCell->Interval);
      Par_PutParUnsigned (NULL,"TTCol",WhichCell->Column  );

      /***** Class type *****/
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			"name=\"TTTyp\" class=\"Tmt_TYP INPUT_%s\"",
			The_GetSuffix ());
	 for (CT  = (Tmt_ClassType_t) 0;
	      CT <= (Tmt_ClassType_t) (Tmt_NUM_CLASS_TYPES - 1);
	      CT++)
	    if ((CT == Tmt_FREE) ||
		((Timetable->View == Tmt_CRS_EDIT) && (CT == Tmt_LECTURE || CT == Tmt_PRACTICAL)) ||
		((Timetable->View == Tmt_TUT_EDIT) && (CT == Tmt_TUTORING)))
	       HTM_OPTION (HTM_Type_STRING,Tmt_DB_ClassType[CT],
			   CT == ClassType ? HTM_OPTION_SELECTED :
	                		     HTM_OPTION_UNSELECTED,
			   HTM_OPTION_ENABLED,
			   "%s",Txt_TIMETABLE_CLASS_TYPES[CT]);
      HTM_SELECT_End ();

      if (IntervalType == Tmt_FREE_INTERVAL)
	{
	 for (i = WhichCell->Interval + 1;
	      i < Timetable->Config.IntervalsPerDay;
	      i++)
	   if (Tmt_TimeTable[WhichCell->Weekday][i].NumColumns == Tmt_MAX_COLUMNS_PER_CELL)
	       break;
	 MaxDuration = i - WhichCell->Interval;
	 Dur = (MaxDuration >= Timetable->Config.IntervalsPerHour) ? Timetable->Config.IntervalsPerHour :	// MaxDuration >= 1h ==> Dur = 1h
								     MaxDuration;				// MaxDuration  < 1h ==> Dur = MaxDuration
	 if (asprintf (&TTDur,"%u:%02u",
		       (Dur / Timetable->Config.IntervalsPerHour),	// Hours
		       (Dur % Timetable->Config.IntervalsPerHour) *
		       Timetable->Config.Range.MinutesPerInterval) < 0)	// Minutes
	    Err_NotEnoughMemoryExit ();
	 Par_PutParString (NULL,"TTDur",TTDur);
	 free (TTDur);
	}
      else
	{
	 /***** Class duration *****/
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			   "name=\"TTDur\" class=\"Tmt_DUR INPUT_%s\"",
			   The_GetSuffix ());
	    for (i = WhichCell->Interval +
		     Tmt_TimeTable[WhichCell->Weekday][WhichCell->Interval].Columns[WhichCell->Column].DurationIntervals;
		 i < Timetable->Config.IntervalsPerDay;
		 i++)
	       if (Tmt_TimeTable[WhichCell->Weekday][i].NumColumns == Tmt_MAX_COLUMNS_PER_CELL)
		  break;
	    MaxDuration = i - WhichCell->Interval;
	    if (Tmt_TimeTable[WhichCell->Weekday][WhichCell->Interval].Columns[WhichCell->Column].DurationIntervals > MaxDuration)
	       MaxDuration = Tmt_TimeTable[WhichCell->Weekday][WhichCell->Interval].Columns[WhichCell->Column].DurationIntervals;
	    for (Dur  = 0;
		 Dur <= MaxDuration;
		 Dur++)
	      {
	       if (asprintf (&TTDur,"%u:%02u",
			     (Dur / Timetable->Config.IntervalsPerHour),	// Hours
			     (Dur % Timetable->Config.IntervalsPerHour) *
			     Timetable->Config.Range.MinutesPerInterval) < 0)	// Minutes
		  Err_NotEnoughMemoryExit ();
	       HTM_OPTION (HTM_Type_STRING,TTDur,
			   Dur == DurationNumIntervals ? HTM_OPTION_SELECTED :
	                				 HTM_OPTION_UNSELECTED,
			   HTM_OPTION_ENABLED,
			   "%s",TTDur);
	       free (TTDur);
	      }
	 HTM_SELECT_End ();

	 /***** Create unique string for this cell used in labels *****/
	 if (asprintf (&CellStr,"%02u%02u%02u",
		       WhichCell->Weekday,
		       WhichCell->Interval,
		       WhichCell->Column) < 0)
	    Err_NotEnoughMemoryExit ();

	 if (Timetable->View == Tmt_CRS_EDIT)
	   {
	    /***** Group *****/
	    HTM_BR ();
	    HTM_LABEL_Begin ("for=\"TTGrp%s\"",CellStr);
	       HTM_Txt (Txt_Group);
	    HTM_LABEL_End ();
	    HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			      "id=\"TTGrp%s\" name=\"%s\""
			      " class=\"Tmt_GRP INPUT_%s\"",
			      CellStr,Par_CodeStr[ParCod_Grp],
			      The_GetSuffix ());
	       HTM_OPTION (HTM_Type_STRING,"-1",
	                   GrpCod <= 0 ? HTM_OPTION_SELECTED :
	                		 HTM_OPTION_UNSELECTED,
	                   HTM_OPTION_ENABLED,
			   "%s",Txt_All_groups);
	       for (NumGrpTyp = 0;
		    NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
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
			   Err_NotEnoughMemoryExit ();
		       }
		     else
		       {
			if (asprintf (&Room,"%s","") < 0)
			   Err_NotEnoughMemoryExit ();
		       }
		     HTM_OPTION (HTM_Type_LONG,&Grp->GrpCod,
				 GrpCod == Grp->GrpCod ? HTM_OPTION_SELECTED :
	                				 HTM_OPTION_UNSELECTED,
				 HTM_OPTION_ENABLED,
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
			    "id=\"TTInf%s\" size=\"1\""
			    " class=\"Tmt_INF INPUT_%s\"",
			    CellStr,The_GetSuffix ());
	   }
	 else // TimeTableView == Tmt_TUT_EDIT
	   {
	    /***** Info *****/
	    HTM_BR ();
	    HTM_LABEL_Begin ("for=\"TTInf%s\" class=\"DAT_SMALL_%s\"",
			     CellStr,
			     The_GetSuffix ());
	       HTM_Txt (Txt_Info);
	    HTM_LABEL_End ();
	    HTM_INPUT_TEXT ("TTInf",Tmt_MAX_CHARS_INFO,Info,
			    HTM_SUBMIT_ON_CHANGE,
			    "id=\"TTInf%s\" size=\"12\""
			    " class=\"Tmt_INF INPUT_%s\"",
			    CellStr,The_GetSuffix ());
	   }

	 /***** Free allocated unique string for this cell used in labels *****/
	 free (CellStr);
	}

   /***** End form *****/
   Frm_EndForm ();
  }
