// swad_timetable.c: timetables

/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For fprintf, etc.
#include <string.h>		// For string functions

#include "swad_calendar.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_text.h"
#include "swad_timetable.h"

/*****************************************************************************/
/*************************** External constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Internal constants ******************************/
/*****************************************************************************/

#define TT_DAYS_PER_WEEK			  7	// Seven days per week

#define TT_MINUTES_PER_HOUR		         60	// Number of minutes in 1 hour
#define TT_SECONDS_PER_MINUTE		         60	// Number of seconds in 1 minute
/*
#define TT_MIN_START_HOUR		 0			// Day starts at this hour
#define TT_MAX_END_HOUR			24			// Day  ends  at this hour
#define TT_MIN_MINUTES_PER_INTERVAL	 5
#define TT_MAX_HOURS_PER_DAY		(TT_MAX_END_HOUR - TT_MIN_START_HOUR)
#define TT_MAX_INTERVALS_PER_HOUR	(TT_MINUTES_PER_HOUR / TT_MIN_MINUTES_PER_INTERVAL)
#define TT_MAX_INTERVALS_PER_DAY	(TT_MAX_INTERVALS_PER_HOUR * TT_MAX_HOURS_PER_DAY)
*/
#define TT_MAX_COLUMNS_PER_CELL			  3	// Maximum number of items (i.e. classes) in a timetable cell (1, 2, 3 or 4)
#define TT_NUM_MINICOLUMNS_PER_DAY		  6	// Least common multiple of 1,2,3,...,TT_MAX_COLUMNS_PER_CELL

#define TT_PERCENT_WIDTH_OF_A_MINICOLUMN	  2	// Width (%) of each minicolumn
#define TT_PERCENT_WIDTH_OF_A_DAY		(TT_PERCENT_WIDTH_OF_A_MINICOLUMN * TT_NUM_MINICOLUMNS_PER_DAY)	// Width (%) of each day
#define TT_PERCENT_WIDTH_OF_ALL_DAYS		(TT_PERCENT_WIDTH_OF_A_DAY * TT_DAYS_PER_WEEK)			// Width (%) of all days
#define TT_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN	  1								// Width (%) of left and right columns (frame)
#define TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN 	 ((100 - TT_PERCENT_WIDTH_OF_ALL_DAYS - TT_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN * 2) / 2)	// Width (%) of the separation columns

#define TT_MAX_BYTES_STR_CLASS_TYPE		256
#define TT_MAX_BYTES_STR_DURATION		 32	// "hh:mm h"

/*****************************************************************************/
/******************************* Internal types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

char *TimeTableStrsClassTypeDB[TT_NUM_CLASS_TYPES] =
  {
   "free",
   "lecture",
   "practical",
   "tutoring",
  };

struct TimeTableColumn
  {
   long CrsCod;		// Course code (-1 if no course selected)
   long GrpCod;		// Group code (-1 if no group selected)
   TT_IntervalType_t IntervalType;
   TT_ClassType_t ClassType;
   unsigned DurationNumIntervals;
   char Place[TT_MAX_BYTES_PLACE + 1];
   char Group[Grp_MAX_BYTES_GROUP_NAME + 1];
  };

struct TimeTableCell
  {
   unsigned NumColumns;
   struct TimeTableColumn Columns[TT_MAX_COLUMNS_PER_CELL];
  };

struct TimeTableCell *TT_TimeTable[TT_DAYS_PER_WEEK];

/*****************************************************************************/
/***************************** Internal prototypes **************************/
/*****************************************************************************/

static void TT_TimeTableConstructor (void);
static void TT_TimeTableDestructor (void);

static void TT_ShowTimeTableGrpsSelected (void);
static void TT_GetParamsTimeTable (void);
static void TT_PutContextualIcons (void);
static void TT_PutFormToSelectWhichGroupsToShow (void);

static void TT_PutIconToViewCrsTT (void);
static void TT_PutIconToViewMyTT (void);

static void TT_WriteCrsTimeTableIntoDB (long CrsCod);
static void TT_WriteTutTimeTableIntoDB (long UsrCod);
static void TT_FillTimeTableFromDB (long UsrCod);
static void TT_ModifTimeTable (void);
static void TT_DrawTimeTable (void);
static void TT_TimeTableDrawAdjustRow (void);
static void TT_TimeTableDrawDaysCells (void);
static void TT_TimeTableDrawHourCell (unsigned Hour,unsigned Min,const char *Align);
static unsigned TT_CalculateColsToDrawInCell (bool TopCall,
                                              unsigned Weekday,unsigned Interval);
static void TT_DrawCellAlignTimeTable (void);
static void TT_TimeTableDrawCell (unsigned Weekday,unsigned Interval,unsigned Column,unsigned ColSpan,
                                  long CrsCod,TT_IntervalType_t IntervalType,TT_ClassType_t ClassType,
                                  unsigned DurationNumIntervals,char *Group,long GrpCod,char *Place);

/*****************************************************************************/
/******************** Create internal timetable in memory ********************/
/*****************************************************************************/

static void TT_TimeTableConstructor (void)
  {
   unsigned Weekday;

   /***** Configure timetable *****/
   Gbl.TimeTable.Config.StartHour =  6;			// Day starts at this hour
   Gbl.TimeTable.Config.EndHour	  = 24;			// Day  ends  at this hour

   Gbl.TimeTable.Config.MinutesPerInterval = 15;	// Number of minutes per interval

   Gbl.TimeTable.Config.HoursPerDay              = Gbl.TimeTable.Config.EndHour -
	                                           Gbl.TimeTable.Config.StartHour;	// From start hour to end hour
   Gbl.TimeTable.Config.SecondsPerInterval       = Gbl.TimeTable.Config.MinutesPerInterval *
	                                           TT_SECONDS_PER_MINUTE;
   Gbl.TimeTable.Config.IntervalsPerHour         = TT_MINUTES_PER_HOUR /
	                                           Gbl.TimeTable.Config.MinutesPerInterval;
   Gbl.TimeTable.Config.IntervalsPerDay          = Gbl.TimeTable.Config.IntervalsPerHour *
	                                           Gbl.TimeTable.Config.HoursPerDay;
   Gbl.TimeTable.Config.IntervalsBeforeStartHour = Gbl.TimeTable.Config.IntervalsPerHour *
	                                           Gbl.TimeTable.Config.StartHour;

   /***** Allocate memory for timetable *****/
   for (Weekday = 0;
	Weekday < TT_DAYS_PER_WEEK;
	Weekday++)
      if ((TT_TimeTable[Weekday] = (struct TimeTableCell *)
	                           malloc (Gbl.TimeTable.Config.IntervalsPerDay *
	                                   sizeof (struct TimeTableCell))) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for timetable.");
  }

/*****************************************************************************/
/******************** Destroy internal timetable in memory *******************/
/*****************************************************************************/

static void TT_TimeTableDestructor (void)
  {
   unsigned Weekday;

   /***** Free memory for timetable *****/
   for (Weekday = 0;
	Weekday < TT_DAYS_PER_WEEK;
	Weekday++)
      free ((void *) TT_TimeTable[Weekday]);
  }

/*****************************************************************************/
/*********** Show whether only my groups or all groups are shown *************/
/*****************************************************************************/

static void TT_ShowTimeTableGrpsSelected (void)
  {
   extern const char *Txt_Groups_OF_A_USER;
   extern const char *Txt_All_groups;

   fprintf (Gbl.F.Out,"<div class=\"CLASSPHOTO_TITLE CENTER_MIDDLE\">");
   switch (Gbl.CurrentCrs.Grps.WhichGrps)
     {
      case Grp_ONLY_MY_GROUPS:
        fprintf (Gbl.F.Out,Txt_Groups_OF_A_USER,
                 Gbl.Usrs.Me.UsrDat.FullName);
        break;
      case Grp_ALL_GROUPS:
        fprintf (Gbl.F.Out,"%s",Txt_All_groups);
        break;
     }
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************** Get paramaters for timetable editing *******************/
/*****************************************************************************/

static void TT_GetParamsTimeTable (void)
  {
   char StrClassType[TT_MAX_BYTES_STR_CLASS_TYPE + 1];
   char StrDuration[TT_MAX_BYTES_STR_DURATION + 1];
   unsigned Hours;
   unsigned Minutes;

   /***** Get day (0: monday, 1: tuesday,..., 6: sunday *****/
   Gbl.TimeTable.Weekday = (unsigned)
	                   Par_GetParToUnsignedLong ("ModTTDay",
                                                     0,
                                                     TT_DAYS_PER_WEEK - 1,
                                                     0);

   /***** Get hour *****/
   Gbl.TimeTable.Interval = (unsigned)
	                    Par_GetParToUnsignedLong ("ModTTHour",
                                                      0,
                                                      Gbl.TimeTable.Config.IntervalsPerDay - 1,
                                                      0);

   /***** Get number of column *****/
   Gbl.TimeTable.Column = (unsigned)
	                  Par_GetParToUnsignedLong ("ModTTCol",
                                                    0,
                                                    TT_MAX_COLUMNS_PER_CELL - 1,
                                                    0);

   /***** Get class type *****/
   Par_GetParToText ("ModTTClassType",StrClassType,TT_MAX_BYTES_STR_CLASS_TYPE);
   for (Gbl.TimeTable.ClassType = (TT_ClassType_t) 0;
	Gbl.TimeTable.ClassType < (TT_ClassType_t) TT_NUM_CLASS_TYPES;
	Gbl.TimeTable.ClassType++)
      if (!strcmp (StrClassType,TimeTableStrsClassTypeDB[Gbl.TimeTable.ClassType]))
         break;
   if (Gbl.TimeTable.ClassType == (TT_ClassType_t) TT_NUM_CLASS_TYPES)
      Lay_ShowErrorAndExit ("Type of timetable cell is missing.");

   /***** Get class duration *****/
   Par_GetParToText ("ModTTDur",StrDuration,TT_MAX_BYTES_STR_DURATION);
   if (sscanf (StrDuration,"%u:%u",&Hours,&Minutes) != 2)
      Lay_ShowErrorAndExit ("Duration is missing.");
   Gbl.TimeTable.DurationNumIntervals = Hours * Gbl.TimeTable.Config.IntervalsPerHour +
	                                Minutes / Gbl.TimeTable.Config.MinutesPerInterval;

   /***** Get group code *****/
   Gbl.TimeTable.GrpCod = Par_GetParToLong ("ModTTGrpCod");

   /***** Get place *****/
   Par_GetParToText ("ModTTPlace",Gbl.TimeTable.Place,TT_MAX_BYTES_PLACE);
  }

/*****************************************************************************/
/**************************** Show class timetable ***************************/
/*****************************************************************************/

void TT_ShowClassTimeTable (void)
  {
   extern const char *Hlp_COURSE_Timetable;
   extern const char *Hlp_PROFILE_Timetable;
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];
   const char *Help[TT_NUM_TIMETABLE_TYPES] =
     {
      Hlp_COURSE_Timetable,	// TT_COURSE_TIMETABLE
      Hlp_PROFILE_Timetable,	// TT_MY_TIMETABLE
      NULL,			// TT_TUTORING_TIMETABLE
     };
   Act_Action_t ActChgTT1stDay[TT_NUM_TIMETABLE_TYPES] =
     {
      ActChgCrsTT1stDay,// TT_COURSE_TIMETABLE
      ActChgMyTT1stDay,	// TT_MY_TIMETABLE
      ActUnk,		// TT_TUTORING_TIMETABLE
     };
   bool PrintView = (Gbl.Action.Act == ActPrnCrsTT ||
	             Gbl.Action.Act == ActPrnMyTT);;

   /***** Initializations *****/
   switch (Gbl.Action.Act)
     {
      case ActSeeCrsTT:
      case ActPrnCrsTT:
      case ActChgCrsTT1stDay:
         Gbl.TimeTable.Type = TT_COURSE_TIMETABLE;
	 break;
      case ActSeeMyTT:
      case ActPrnMyTT:
      case ActChgMyTT1stDay:
         Gbl.TimeTable.Type = TT_MY_TIMETABLE;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong action.");
     }

   Gbl.TimeTable.ContextualIcons.PutIconEditCrsTT = (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE &&
	                                             !PrintView &&
                                                     Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER);
   Gbl.TimeTable.ContextualIcons.PutIconEditOfficeHours = (Gbl.TimeTable.Type == TT_MY_TIMETABLE &&
	                                                   !PrintView &&
                                                           (Gbl.Usrs.Me.AvailableRoles & (1 << Rol_TEACHER)));
   Gbl.TimeTable.ContextualIcons.PutIconPrint = !PrintView;

   /***** Get whether to show only my groups or all groups *****/
   Grp_GetParamWhichGrps ();

   /***** Start frame *****/
   Lay_StartRoundFrame ("100%",Txt_TIMETABLE_TYPES[Gbl.TimeTable.Type],
                        (Gbl.TimeTable.ContextualIcons.PutIconEditCrsTT ||
                         Gbl.TimeTable.ContextualIcons.PutIconEditOfficeHours ||
                         Gbl.TimeTable.ContextualIcons.PutIconPrint) ? TT_PutContextualIcons :
                                                                       NULL,
                        Help[Gbl.TimeTable.Type]);

   /***** Start time table drawing *****/
   if (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE)
      Lay_WriteHeaderClassPhoto (PrintView,false,
				 Gbl.CurrentIns.Ins.InsCod,Gbl.CurrentDeg.Deg.DegCod,Gbl.CurrentCrs.Crs.CrsCod);

   if (PrintView)
      /***** Show whether only my groups or all groups are selected *****/
      TT_ShowTimeTableGrpsSelected ();
   else
     {
      /***** Select whether show only my groups or all groups *****/
      if ( Gbl.TimeTable.Type == TT_MY_TIMETABLE ||
          (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE &&
           Gbl.CurrentCrs.Grps.NumGrps))
         TT_PutFormToSelectWhichGroupsToShow ();

      /***** Show form to change first day of week *****/
      Cal_ShowFormToSelFirstDayOfWeek (ActChgTT1stDay[Gbl.TimeTable.Type],
                                       Grp_PutParamWhichGrps,
	                               "ICO25x25");
     }

   /***** Show the time table *****/
   TT_ShowTimeTable (Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/***************** Put contextual icons above the time table *****************/
/*****************************************************************************/

static void TT_PutContextualIcons (void)
  {
   extern const char *Txt_Edit;
   extern const char *Txt_Edit_office_hours;
   extern const char *Txt_Print;

   if (Gbl.TimeTable.ContextualIcons.PutIconEditCrsTT)
      Lay_PutContextualLink (ActEdiCrsTT,NULL,Grp_PutParamWhichGrps,
			     "edit64x64.png",
			     Txt_Edit,NULL,
		             NULL);

   if (Gbl.TimeTable.ContextualIcons.PutIconEditOfficeHours)
      Lay_PutContextualLink (ActEdiTut,NULL,NULL,
                             "edit64x64.png",
			     Txt_Edit_office_hours,NULL,
		             NULL);

   if (Gbl.TimeTable.ContextualIcons.PutIconPrint)
      Lay_PutContextualLink (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE ? ActPrnCrsTT :
								         ActPrnMyTT,
			     NULL,Grp_PutParamWhichGrps,
			     "print64x64.png",
			     Txt_Print,NULL,
		             NULL);
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void TT_PutFormToSelectWhichGroupsToShow (void)
  {
   Act_Action_t ActSeeTT[TT_NUM_TIMETABLE_TYPES] =
     {
      ActSeeCrsTT,	// TT_COURSE_TIMETABLE
      ActSeeMyTT,	// TT_MY_TIMETABLE
      ActUnk,		// TT_TUTORING_TIMETABLE
     };

   Grp_ShowFormToSelWhichGrps (ActSeeTT[Gbl.TimeTable.Type],NULL);
  }

/*****************************************************************************/
/********************** Show course timetable for edition ********************/
/*****************************************************************************/

void TT_EditCrsTimeTable (void)
  {
   extern const char *Hlp_COURSE_Timetable;
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];

   /***** Editable time table *****/
   Gbl.TimeTable.Type = TT_COURSE_TIMETABLE;
   Lay_StartRoundFrame ("100%",Txt_TIMETABLE_TYPES[Gbl.TimeTable.Type],
                        TT_PutIconToViewCrsTT,Hlp_COURSE_Timetable);
   TT_ShowTimeTable (Gbl.Usrs.Me.UsrDat.UsrCod);
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************* Show tutor timetable for edition **********************/
/*****************************************************************************/

void TT_EditMyTutTimeTable (void)
  {
   extern const char *Hlp_PROFILE_Timetable;
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];

   /***** Time table *****/
   Gbl.TimeTable.Type = TT_TUTORING_TIMETABLE;
   Lay_StartRoundFrame ("100%",Txt_TIMETABLE_TYPES[Gbl.TimeTable.Type],
                        TT_PutIconToViewMyTT,Hlp_PROFILE_Timetable);
   TT_ShowTimeTable (Gbl.Usrs.Me.UsrDat.UsrCod);
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************** Put icon to view course timetable ********************/
/*****************************************************************************/

static void TT_PutIconToViewCrsTT (void)
  {
   extern const char *Txt_Show_timetable;

   /***** Link (form) to see course timetable *****/
   Lay_PutContextualLink (ActSeeCrsTT,NULL,NULL,
                          "eye-on64x64.png",
                          Txt_Show_timetable,NULL,
		          NULL);
  }

/*****************************************************************************/
/************************ Put icon to view my timetable **********************/
/*****************************************************************************/

static void TT_PutIconToViewMyTT (void)
  {
   extern const char *Txt_Show_timetable;

   /***** Link (form) to see my timetable *****/
   Lay_PutContextualLink (ActSeeMyTT,NULL,NULL,
                          "eye-on64x64.png",
                          Txt_Show_timetable,NULL,
		          NULL);
  }

/*****************************************************************************/
/*********** Show course timetable or tutor timetable of a teacher ***********/
/*****************************************************************************/

void TT_ShowTimeTable (long UsrCod)
  {
   /***** Create internal timetable in memory *****/
   TT_TimeTableConstructor ();

   /***** Fill internal timetable with the timetable from database *****/
   TT_FillTimeTableFromDB (UsrCod);

   /***** If timetable must be modified... *****/
   if (Gbl.Action.Act == ActChgCrsTT ||
       Gbl.Action.Act == ActChgTut)
     {
      /* Get parameters for time table editing */
      TT_GetParamsTimeTable ();

      /* Modify timetable in memory */
      TT_ModifTimeTable ();

      /* Write a new timetable in database */
      switch (Gbl.TimeTable.Type)
        {
         case TT_COURSE_TIMETABLE:
            TT_WriteCrsTimeTableIntoDB (Gbl.CurrentCrs.Crs.CrsCod);
	    break;
         case TT_TUTORING_TIMETABLE:
            TT_WriteTutTimeTableIntoDB (UsrCod);
            break;
         default:
            break;
        }

      /* Get a new table from database */
      TT_FillTimeTableFromDB (UsrCod);
     }

   /***** Draw timetable *****/
   TT_DrawTimeTable ();

   /***** Free internal timetable in memory *****/
   TT_TimeTableDestructor ();
  }

/*****************************************************************************/
/******************* Write course timetable into database ********************/
/*****************************************************************************/

static void TT_WriteCrsTimeTableIntoDB (long CrsCod)
  {
   char Query[512 +
              TT_MAX_BYTES_PLACE +
              Grp_MAX_BYTES_GROUP_NAME];
   unsigned Weekday;
   unsigned Interval;
   unsigned Hour;
   unsigned Min;
   unsigned Column;

   /***** Remove former timetable *****/
   sprintf (Query,"DELETE FROM timetable_crs WHERE CrsCod=%ld",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove former timetable");

   /***** Go across the timetable inserting classes into database *****/
   for (Weekday = 0;
	Weekday < TT_DAYS_PER_WEEK;
	Weekday++)
      for (Interval = 0, Hour = Gbl.TimeTable.Config.StartHour, Min = 0;
	   Interval < Gbl.TimeTable.Config.IntervalsPerDay;
	   Interval++,
	   Hour += (Min + Gbl.TimeTable.Config.MinutesPerInterval) / TT_SECONDS_PER_MINUTE,
	   Min   = (Min + Gbl.TimeTable.Config.MinutesPerInterval) % TT_SECONDS_PER_MINUTE)
         for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
	    if (TT_TimeTable[Weekday][Interval].Columns[Column].IntervalType == TT_FIRST_INTERVAL &&
                TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals)
              {
               sprintf (Query,"INSERT INTO timetable_crs"
        	              " (CrsCod,GrpCod,Weekday,StartTime,Duration,"
        	              "ClassType,Place,GroupName)"
                              " VALUES"
                              " (%ld,%ld,%u,'%02u:%02u:00',SEC_TO_TIME(%u),"
                              "'%s','%s','%s')",
                        CrsCod,
			TT_TimeTable[Weekday][Interval].Columns[Column].GrpCod,
			Weekday,
			Hour,Min,
			TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals *
			Gbl.TimeTable.Config.SecondsPerInterval,
                        TimeTableStrsClassTypeDB[TT_TimeTable[Weekday][Interval].Columns[Column].ClassType],
                        TT_TimeTable[Weekday][Interval].Columns[Column].Place,
		        TT_TimeTable[Weekday][Interval].Columns[Column].Group);
               DB_QueryINSERT (Query,"can not create course timetable");
              }
  }

/*****************************************************************************/
/********************* Write tutor timetable into database *******************/
/*****************************************************************************/

static void TT_WriteTutTimeTableIntoDB (long UsrCod)
  {
   char Query[512 +
              TT_MAX_BYTES_PLACE];
   unsigned Weekday;
   unsigned Interval;
   unsigned Hour;
   unsigned Min;
   unsigned Column;

   /***** Remove former timetable *****/
   sprintf (Query,"DELETE FROM timetable_tut WHERE UsrCod=%ld",
            UsrCod);
   DB_QueryDELETE (Query,"can not remove former timetable");

   /***** Loop over timetable *****/
   for (Weekday = 0;
	Weekday < TT_DAYS_PER_WEEK;
	Weekday++)
      for (Interval = 0, Hour = Gbl.TimeTable.Config.StartHour, Min = 0;
	   Interval < Gbl.TimeTable.Config.IntervalsPerDay;
	   Interval++,
	   Hour += (Min + Gbl.TimeTable.Config.MinutesPerInterval) / TT_SECONDS_PER_MINUTE,
	   Min   = (Min + Gbl.TimeTable.Config.MinutesPerInterval) % TT_SECONDS_PER_MINUTE)
	 for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
	    if (TT_TimeTable[Weekday][Interval].Columns[Column].IntervalType == TT_FIRST_INTERVAL &&
                TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals)
              {
               sprintf (Query,"INSERT INTO timetable_tut"
        	              " (UsrCod,Weekday,StartTime,Duration,Place)"
                              " VALUES"
                              " (%ld,%u,'%02u:%02u:00',SEC_TO_TIME(%u),'%s')",
                        UsrCod,
			Weekday,
			Hour,Min,
			TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals *
			Gbl.TimeTable.Config.SecondsPerInterval,
			TT_TimeTable[Weekday][Interval].Columns[Column].Place);
               DB_QueryINSERT (Query,"can not create office timetable");
              }
  }

/*****************************************************************************/
/********** Create an internal table with timetable from database ************/
/*****************************************************************************/

static void TT_FillTimeTableFromDB (long UsrCod)
  {
   extern const char *Txt_Incomplete_timetable_for_lack_of_space;
   char Query[4096];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   unsigned Weekday;
   unsigned Interval;
   unsigned I;
   unsigned DurationNumIntervals;
   unsigned Column;
   unsigned Seconds;
   unsigned FirstFreeColumn;
   long GrpCod;
   TT_ClassType_t ClassType = TT_FREE;	// Initialized to avoid warning
   bool TimeTableIsIncomplete = false;
   bool TimeTableHasSpaceForThisClass;
   bool Found;

   /***** Initialize timetable to all free *****/
   for (Weekday = 0;
	Weekday < TT_DAYS_PER_WEEK;
	Weekday++)
      for (Interval = 0;
	   Interval < Gbl.TimeTable.Config.IntervalsPerDay;
	   Interval++)
        {
         TT_TimeTable[Weekday][Interval].NumColumns = 0;
         for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
	   {
	    TT_TimeTable[Weekday][Interval].Columns[Column].CrsCod    = -1L;
	    TT_TimeTable[Weekday][Interval].Columns[Column].GrpCod    = -1L;
	    TT_TimeTable[Weekday][Interval].Columns[Column].IntervalType  = TT_FREE_INTERVAL;
	    TT_TimeTable[Weekday][Interval].Columns[Column].ClassType = TT_FREE;
	    TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals = 0;
	    TT_TimeTable[Weekday][Interval].Columns[Column].Group[0]  = '\0';
	    TT_TimeTable[Weekday][Interval].Columns[Column].Place[0]  = '\0';
	   }
        }

   /***** Get timetable from database *****/
   switch (Gbl.TimeTable.Type)
     {
      case TT_MY_TIMETABLE:
         switch (Gbl.CurrentCrs.Grps.WhichGrps)
           {
            case Grp_ONLY_MY_GROUPS:
               sprintf (Query,"SELECT "
                              "timetable_crs.Weekday,"
        	              "TIME_TO_SEC(timetable_crs.StartTime) AS S,"
        	              "TIME_TO_SEC(timetable_crs.Duration) AS D,"
        	              "timetable_crs.Place,"
                              "timetable_crs.ClassType,"
                              "timetable_crs.GroupName,"
                              "timetable_crs.GrpCod,"
                              "timetable_crs.CrsCod"
                              " FROM timetable_crs,crs_usr"
                              " WHERE crs_usr.UsrCod=%ld"
                              " AND timetable_crs.GrpCod=-1"
                              " AND timetable_crs.CrsCod=crs_usr.CrsCod"
                              " UNION DISTINCT "
                              "SELECT "
                              "timetable_crs.Weekday,"
                              "TIME_TO_SEC(timetable_crs.StartTime) AS S,"
                              "TIME_TO_SEC(timetable_crs.Duration) AS D,"
                              "timetable_crs.Place,"
                              "timetable_crs.ClassType,"
                              "timetable_crs.GroupName,"
                              "timetable_crs.GrpCod,"
                              "timetable_crs.CrsCod"
                              " FROM timetable_crs,crs_grp_usr"
                              " WHERE crs_grp_usr.UsrCod=%ld"
                              " AND timetable_crs.GrpCod=crs_grp_usr.GrpCod"
                              " UNION "
                              "SELECT "
                              "Weekday,"
                              "TIME_TO_SEC(StartTime) AS S,"
                              "TIME_TO_SEC(Duration) AS D,"
                              "Place,"
                              "'tutoring' AS ClassType,"
                              "'' AS GroupName,"
                              "-1 AS GrpCod,"
                              "-1 AS CrsCod"
                              " FROM timetable_tut"
                              " WHERE UsrCod=%ld"
                              " ORDER BY Weekday,S,ClassType,"
                              "GroupName,GrpCod,Place,D DESC,CrsCod",
                        UsrCod,UsrCod,UsrCod);
               break;
            case Grp_ALL_GROUPS:
               sprintf (Query,"SELECT "
        	              "timetable_crs.Weekday,"
        	              "TIME_TO_SEC(timetable_crs.StartTime) AS S,"
        	              "TIME_TO_SEC(timetable_crs.Duration) AS D,"
        	              "timetable_crs.Place,"
                              "timetable_crs.ClassType,"
                              "timetable_crs.GroupName,"
                              "timetable_crs.GrpCod,"
                              "timetable_crs.CrsCod"
                              " FROM timetable_crs,crs_usr"
                              " WHERE crs_usr.UsrCod=%ld"
                              " AND timetable_crs.CrsCod=crs_usr.CrsCod"
                              " UNION "
                              "SELECT "
                              "Weekday,"
                              "TIME_TO_SEC(StartTime) AS S,"
                              "TIME_TO_SEC(Duration) AS D,"
                              "Place,"
                              "'tutoring' AS ClassType,"
                              "'' AS GroupName,"
                              "-1 AS GrpCod,"
                              "-1 AS CrsCod"
                              " FROM timetable_tut"
                              " WHERE UsrCod=%ld"
                              " ORDER BY Weekday,S,ClassType,"
                              "GroupName,GrpCod,Place,D DESC,CrsCod",
                        UsrCod,UsrCod);
               break;
           }
	 break;
      case TT_COURSE_TIMETABLE:
         if (Gbl.CurrentCrs.Grps.WhichGrps == Grp_ALL_GROUPS ||
             Gbl.Action.Act == ActEdiCrsTT ||
             Gbl.Action.Act == ActChgCrsTT)	// If we are editing, all groups are shown
            sprintf (Query,"SELECT "
        	           "Weekday,"
        	           "TIME_TO_SEC(StartTime) AS S,"
        	           "TIME_TO_SEC(Duration) AS D,"
        	           "Place,"
        	           "ClassType,"
        	           "GroupName,"
        	           "GrpCod"
        	           " FROM timetable_crs"
                           " WHERE CrsCod=%ld"
                           " ORDER BY Weekday,S,ClassType,"
                           "GroupName,GrpCod,Place,D DESC",
                     Gbl.CurrentCrs.Crs.CrsCod);
         else
            sprintf (Query,"SELECT "
        	           "timetable_crs.Weekday,"
        	           "TIME_TO_SEC(timetable_crs.StartTime) AS S,"
        	           "TIME_TO_SEC(timetable_crs.Duration) AS D,"
        	           "timetable_crs.Place,"
        	           "timetable_crs.ClassType,"
        	           "timetable_crs.GroupName,"
        	           "timetable_crs.GrpCod"
                           " FROM timetable_crs,crs_usr"
                           " WHERE timetable_crs.CrsCod=%ld"
                           " AND timetable_crs.GrpCod=-1 AND crs_usr.UsrCod=%ld"
                           " AND timetable_crs.CrsCod=crs_usr.CrsCod"
                           " UNION DISTINCT "
                           "SELECT timetable_crs.Weekday,"
                           "TIME_TO_SEC(timetable_crs.StartTime) AS S,"
                           "TIME_TO_SEC(timetable_crs.Duration) AS D,"
                           "timetable_crs.Place,"
                           "timetable_crs.ClassType,"
                           "timetable_crs.GroupName,"
                           "timetable_crs.GrpCod"
			   " FROM timetable_crs,crs_grp_usr"
                           " WHERE timetable_crs.CrsCod=%ld"
                           " AND crs_grp_usr.UsrCod=%ld"
                           " AND timetable_crs.GrpCod=crs_grp_usr.GrpCod"
                           " ORDER BY Weekday,S,ClassType,"
                           "GroupName,GrpCod,Place,D DESC",
                     Gbl.CurrentCrs.Crs.CrsCod,UsrCod,
                     Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
	 break;
      case TT_TUTORING_TIMETABLE:
         sprintf (Query,"SELECT "
                        "Weekday,"
                        "TIME_TO_SEC(StartTime) AS S,"
                        "TIME_TO_SEC(Duration) AS D,"
                        "Place"
                        " FROM timetable_tut"
                        " WHERE UsrCod=%ld"
                        " ORDER BY Weekday,S,Place,D DESC",
                  UsrCod);
         break;
     }
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get timetable");

   /***** Build the table depending on the number of rows of the timetable *****/
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      if (Gbl.TimeTable.Type == TT_MY_TIMETABLE ||
          Gbl.TimeTable.Type == TT_COURSE_TIMETABLE)
         /* Group code */
         if (sscanf (row[6],"%ld",&GrpCod) != 1)
            GrpCod = -1;

      /* Day of week (row[0]) */
      if (sscanf (row[0],"%u",&Weekday) != 1)
	 Lay_ShowErrorAndExit ("Wrong day of week in timetable.");
      if (Weekday >= TT_DAYS_PER_WEEK)
	 Lay_ShowErrorAndExit ("Wrong day of week in timetable.");

      /* StartTime formatted as seconds (row[1])
         --> StartTime in number of intervals */
      if (sscanf (row[1],"%u",&Seconds) != 1)
	 Lay_ShowErrorAndExit ("Wrong start time in timetable.");
      Interval = Seconds / Gbl.TimeTable.Config.SecondsPerInterval;
      if (Interval < Gbl.TimeTable.Config.IntervalsBeforeStartHour)
	 Lay_ShowErrorAndExit ("Wrong start time in timetable.");
      Interval -= Gbl.TimeTable.Config.IntervalsBeforeStartHour;

      /* Duration formatted as seconds (row[2])
         --> Duration in number of intervals */
      if (sscanf (row[2],"%u",&Seconds) != 1)
	 Lay_ShowErrorAndExit ("Wrong duration in timetable.");
      DurationNumIntervals = Seconds / Gbl.TimeTable.Config.SecondsPerInterval;

      /* Type of class (row[4]) */
      switch (Gbl.TimeTable.Type)
        {
         case TT_COURSE_TIMETABLE:
         case TT_MY_TIMETABLE:
            for (ClassType = TT_LECTURE, Found = false;
        	 ClassType <= TT_TUTORING;
        	 ClassType++)
  	       if (!strcmp (row[4],TimeTableStrsClassTypeDB[ClassType]))
  		 {
  		  Found = true;
                  break;
                 }
            if (!Found)
	       Lay_ShowErrorAndExit ("Wrong type of class in timetable.");
            break;
         case TT_TUTORING_TIMETABLE:
            ClassType = TT_TUTORING;
            break;
        }

      /* Cell has been read without errors */
      if (TT_TimeTable[Weekday][Interval].NumColumns < TT_MAX_COLUMNS_PER_CELL)
	 // If there's place for another column in this cell...
        {
         /* Find the first free column for this day-hour */
         FirstFreeColumn = TT_MAX_COLUMNS_PER_CELL;
         for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
            if (TT_TimeTable[Weekday][Interval].Columns[Column].IntervalType == TT_FREE_INTERVAL)
              {
               FirstFreeColumn = Column;
               break;
              }

         if (FirstFreeColumn < TT_MAX_COLUMNS_PER_CELL)
            // If there's place for another column in this cell
           {
            /* Check if there's place for all the rows of this class */
            TimeTableHasSpaceForThisClass = true;
            for (I = Interval + 1;
        	 I < Interval + DurationNumIntervals &&
        	 I < Gbl.TimeTable.Config.IntervalsPerDay;
        	 I++)
               if (TT_TimeTable[Weekday][I].Columns[FirstFreeColumn].IntervalType != TT_FREE_INTERVAL)
                {
                  TimeTableIsIncomplete = true;
                  TimeTableHasSpaceForThisClass = false;
                  break;
                 }
            if (TimeTableHasSpaceForThisClass)
              {
               TT_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].ClassType = ClassType;
               TT_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].DurationNumIntervals = DurationNumIntervals;
               TT_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].IntervalType = TT_FIRST_INTERVAL;
               for (I = Interval + 1;
        	    I < Interval + DurationNumIntervals &&
        	    I < Gbl.TimeTable.Config.IntervalsPerDay;
        	    I++)
                 {
	          TT_TimeTable[Weekday][I].Columns[FirstFreeColumn].IntervalType = TT_NEXT_INTERVAL;
	          TT_TimeTable[Weekday][I].NumColumns++;
                 }

               /* Course (row[7]), group (row[5]) and place (row[3])*/
               switch (Gbl.TimeTable.Type)
                 {
                  case TT_MY_TIMETABLE:
                  case TT_COURSE_TIMETABLE:
                     TT_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].CrsCod =
                        (Gbl.TimeTable.Type == TT_MY_TIMETABLE ? Str_ConvertStrCodToLongCod (row[7]) :
                                                                 Gbl.CurrentCrs.Crs.CrsCod);
                     Str_Copy (TT_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].Group,
                               row[5],
                               Grp_MAX_BYTES_GROUP_NAME);
                     TT_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].GrpCod = GrpCod;
                     // no break;
                  case TT_TUTORING_TIMETABLE:
                     Str_Copy (TT_TimeTable[Weekday][Interval].Columns[FirstFreeColumn].Place,
                               row[3],
                               TT_MAX_BYTES_PLACE);
                     break;
                 }

               /* Increment number of items in this cell */
               TT_TimeTable[Weekday][Interval].NumColumns++;
              }
           }
         else
            TimeTableIsIncomplete = true;
        }
      else
         TimeTableIsIncomplete = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (TimeTableIsIncomplete)
      Lay_ShowAlert (Lay_INFO,Txt_Incomplete_timetable_for_lack_of_space);
  }

/*****************************************************************************/
/*********************** Modify a class in timetable *************************/
/*****************************************************************************/

static void TT_ModifTimeTable (void)
  {
   if (TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].IntervalType == TT_FIRST_INTERVAL)
     {
      /***** Free this cell *****/
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].GrpCod    = -1;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].IntervalType  = TT_FREE_INTERVAL;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].ClassType = TT_FREE;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].DurationNumIntervals = 0;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].Group[0]  = '\0';
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].Place[0]  = '\0';
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].NumColumns--;
     }

   if (Gbl.TimeTable.ClassType != TT_FREE &&
       Gbl.TimeTable.DurationNumIntervals > 0 &&
       TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].NumColumns < TT_MAX_COLUMNS_PER_CELL)
     {
      /***** Change this cell *****/
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].NumColumns++;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].GrpCod    = Gbl.TimeTable.GrpCod;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].IntervalType  = TT_FIRST_INTERVAL;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].ClassType = Gbl.TimeTable.ClassType;
      TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].DurationNumIntervals = Gbl.TimeTable.DurationNumIntervals;
      Str_Copy (TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].Group,
                Gbl.TimeTable.Group,
                Grp_MAX_BYTES_GROUP_NAME);
      Str_Copy (TT_TimeTable[Gbl.TimeTable.Weekday][Gbl.TimeTable.Interval].Columns[Gbl.TimeTable.Column].Place,
                Gbl.TimeTable.Place,
                TT_MAX_BYTES_PLACE);
     }
  }

/*****************************************************************************/
/********************* Draw timetable using internal table *******************/
/*****************************************************************************/

static void TT_DrawTimeTable (void)
  {
   bool Editing = false;
   unsigned DayColumn;	// Column from left (0) to right (6)
   unsigned Weekday;	// Day of week
   unsigned Interval;
   unsigned Min;
   unsigned Column;
   unsigned ColumnsToDraw;
   unsigned ColumnsToDrawIncludingExtraColumn;
   unsigned ContinuousFreeMinicolumns;

   switch (Gbl.Action.Act)
     {
      case ActSeeCrsTT:		case ActPrnCrsTT:	case ActChgCrsTT1stDay:
      case ActSeeMyTT:		case ActPrnMyTT:	case ActChgMyTT1stDay:
      case ActSeeRecOneTch:	case ActSeeRecSevTch:
         Editing = false;
         break;
      case ActEdiCrsTT:		case ActChgCrsTT:
      case ActEdiTut:		case ActChgTut:
	 // If editing and there's place for more columns, a potential new column is added at the end of each day
         Editing = true;
         break;
     }

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<table id=\"timetable\">");

   /***** Top row used for column adjustement *****/
   TT_TimeTableDrawAdjustRow ();

   /***** Row with day names *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td rowspan=\"2\" class=\"TT_HOUR_BIG RIGHT_MIDDLE\""
	              " style=\"width:%u%%;\">"
	              "%02u"
	              "</td>",
            TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN,
            Gbl.TimeTable.Config.StartHour);
   TT_DrawCellAlignTimeTable ();
   TT_TimeTableDrawDaysCells ();
   TT_DrawCellAlignTimeTable ();
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"TT_HOUR_BIG LEFT_MIDDLE\""
	              " style=\"width:%u%%;\">"
	              "%02u"
	              "</td>"
	              "</tr>",
            TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN,
            Gbl.TimeTable.Config.StartHour);

   /***** Get list of groups types and groups in this course *****/
   if (Gbl.Action.Act == ActEdiCrsTT ||
       Gbl.Action.Act == ActChgCrsTT)
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Write the table row by row *****/
   for (Interval = 0, Min = Gbl.TimeTable.Config.MinutesPerInterval;
	Interval < Gbl.TimeTable.Config.IntervalsPerDay;
	Interval++,
	Min = (Min + Gbl.TimeTable.Config.MinutesPerInterval) %
	      TT_SECONDS_PER_MINUTE)
     {
      fprintf (Gbl.F.Out,"<tr>");

      /* Left hour:minutes cell */
      if (Interval % 2)
	 TT_TimeTableDrawHourCell (Gbl.TimeTable.Config.StartHour +
	                           (Interval + 2) / Gbl.TimeTable.Config.IntervalsPerHour,
	                           Min,
	                           "RIGHT_MIDDLE");

      /* Empty column used to adjust height */
      TT_DrawCellAlignTimeTable ();

      /* Row for this hour */
      for (DayColumn = 0;
	   DayColumn < TT_DAYS_PER_WEEK;
	   DayColumn++)
        {
	 /* Weekday == 0 ==> monday,
	            ...
	    Weekday == 6 ==> sunday */
	 Weekday = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;

         /* Check how many colums are needed.
            For each item (class) in this hour from left to right,
            we must check the maximum of columns */
         ColumnsToDraw = TT_CalculateColsToDrawInCell (true,	// Top call, non recursive
                                                       Weekday,Interval);
         if (!Editing && ColumnsToDraw == 0)
            ColumnsToDraw = 1;
         ColumnsToDrawIncludingExtraColumn = ColumnsToDraw;
         if (Editing && ColumnsToDraw < TT_MAX_COLUMNS_PER_CELL)
            ColumnsToDrawIncludingExtraColumn++;

         /* Draw cells */
         for (Column = 0, ContinuousFreeMinicolumns = 0;
              Column < ColumnsToDrawIncludingExtraColumn;
              Column++)
            if (TT_TimeTable[Weekday][Interval].Columns[Column].IntervalType == TT_FREE_INTERVAL)
               ContinuousFreeMinicolumns += TT_NUM_MINICOLUMNS_PER_DAY /
                                            ColumnsToDrawIncludingExtraColumn;
            else
              {
               if (ContinuousFreeMinicolumns)
                 {
                  TT_TimeTableDrawCell (Weekday,Interval,Column - 1,ContinuousFreeMinicolumns,
                                        -1L,TT_FREE_INTERVAL,TT_FREE,0,NULL,-1,NULL);
                  ContinuousFreeMinicolumns = 0;
                 }
               TT_TimeTableDrawCell (Weekday,Interval,Column,
                                     TT_NUM_MINICOLUMNS_PER_DAY /
                                     ColumnsToDrawIncludingExtraColumn,
	                             TT_TimeTable[Weekday][Interval].Columns[Column].CrsCod,
				     TT_TimeTable[Weekday][Interval].Columns[Column].IntervalType,
	                             TT_TimeTable[Weekday][Interval].Columns[Column].ClassType,
                                     TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals,
	                             TT_TimeTable[Weekday][Interval].Columns[Column].Group,
	                             TT_TimeTable[Weekday][Interval].Columns[Column].GrpCod,
                                     TT_TimeTable[Weekday][Interval].Columns[Column].Place);
              }
         if (ContinuousFreeMinicolumns)
            TT_TimeTableDrawCell (Weekday,Interval,Column - 1,ContinuousFreeMinicolumns,
                                  -1L,TT_FREE_INTERVAL,TT_FREE,0,NULL,-1L,NULL);
        }

      /* Empty column used to adjust height */
      TT_DrawCellAlignTimeTable ();

      /* Right hour:minutes cell */
      if (Interval % 2)
	 TT_TimeTableDrawHourCell (Gbl.TimeTable.Config.StartHour +
	                           (Interval + 2) / Gbl.TimeTable.Config.IntervalsPerHour,
	                           Min,
	                           "LEFT_MIDDLE");

      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** Free list of groups types and groups in this course *****/
   if (Gbl.Action.Act == ActEdiCrsTT ||
       Gbl.Action.Act == ActChgCrsTT)
      Grp_FreeListGrpTypesAndGrps ();

   /***** Row with day names *****/
   fprintf (Gbl.F.Out,"<tr>");
   TT_DrawCellAlignTimeTable ();
   TT_TimeTableDrawDaysCells ();
   TT_DrawCellAlignTimeTable ();
   fprintf (Gbl.F.Out,"</tr>");

   /***** Bottom row used for column adjustement *****/
   TT_TimeTableDrawAdjustRow ();

   /***** End of the table *****/
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/********** Draw a row used for column adjustement in a time table ***********/
/*****************************************************************************/

static void TT_TimeTableDrawAdjustRow (void)
  {
   unsigned Weekday;
   unsigned Minicolumn;

   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TT_TXT LEFT_MIDDLE\" style=\"width:%u%%;\">"
                      "&nbsp;"
                      "</td>",
            TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN);
   TT_DrawCellAlignTimeTable ();
   for (Weekday = 0;
	Weekday < TT_DAYS_PER_WEEK;
	Weekday++)
      for (Minicolumn = 0;
	   Minicolumn < TT_NUM_MINICOLUMNS_PER_DAY;
	   Minicolumn++)
         fprintf (Gbl.F.Out,"<td class=\"TT_TXT LEFT_MIDDLE\""
                            " style=\"width:%u%%;\">"
			    "&nbsp;"
                            "</td>",
                  TT_PERCENT_WIDTH_OF_A_MINICOLUMN);
   TT_DrawCellAlignTimeTable ();
   fprintf (Gbl.F.Out,"<td class=\"TT_TXT LEFT_MIDDLE\" style=\"width:%u%%;\">"
                      "&nbsp;"
	              "</td>"
	              "</tr>",
            TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN);
  }

/*****************************************************************************/
/****************** Draw cells with day names in a time table ****************/
/*****************************************************************************/

static void TT_TimeTableDrawDaysCells (void)
  {
   extern const char *Txt_DAYS_CAPS[7];
   unsigned DayColumn;
   unsigned Weekday;

   for (DayColumn = 0;
	DayColumn < TT_DAYS_PER_WEEK;
	DayColumn++)
     {
      Weekday = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;
      fprintf (Gbl.F.Out,"<td colspan=\"%u\""
	                 " class=\"%s CENTER_MIDDLE\""
	                 " style=\"width:%u%%;\">"
	                 "%s"
	                 "</td>",
               TT_NUM_MINICOLUMNS_PER_DAY,
               Weekday == 6 ? "TT_SUNDAY" :	// Sunday drawn in red
        	              "TT_DAY",	// Monday to Saturday
               TT_PERCENT_WIDTH_OF_A_DAY,
               Txt_DAYS_CAPS[Weekday]);
     }
  }

/*****************************************************************************/
/****************** Draw cells with day names in a time table ****************/
/*****************************************************************************/

static void TT_TimeTableDrawHourCell (unsigned Hour,unsigned Min,const char *Align)
  {
   fprintf (Gbl.F.Out,"<td rowspan=\"2\""
		      " class=\"TT_HOUR %s %s\""
		      " style=\"width:%u%%;\">",
	    Min ? "TT_HOUR_SMALL" :
		  "TT_HOUR_BIG",
            Align,
	    TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN);
   fprintf (Gbl.F.Out,"%02u",Hour);
   if (Min)
      fprintf (Gbl.F.Out,":%02u",Min);
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/**** Calculate recursively number of columns to draw for a day and hour *****/
/*****************************************************************************/

static unsigned TT_CalculateColsToDrawInCell (bool TopCall,
                                              unsigned Weekday,unsigned Interval)
  {
   unsigned ColumnsToDraw;
   unsigned Column;
   unsigned I;
   unsigned FirstHour;
   unsigned Cols;
   static bool *TT_IntervalsChecked;

   if (TopCall)	// Top call, non recursive call
     {
      /****** Allocate space to store list of intervals already checked
              and initialize to false by using calloc *****/
      if ((TT_IntervalsChecked = (bool *) calloc (Gbl.TimeTable.Config.IntervalsPerDay,
                                                  sizeof (bool))) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for timetable.");
     }

   ColumnsToDraw = TT_TimeTable[Weekday][Interval].NumColumns;

   if (!TT_IntervalsChecked[Interval])
     {
      TT_IntervalsChecked[Interval] = true;
      for (Column = 0;
	   Column < TT_MAX_COLUMNS_PER_CELL;
	   Column++)
        {
         switch (TT_TimeTable[Weekday][Interval].Columns[Column].IntervalType)
           {
            case TT_FREE_INTERVAL:
               break;
            case TT_FIRST_INTERVAL:
               /* Check from first hour (this one) to last hour searching maximum number of columns */
               for (I = Interval + 1;
        	    I < Interval + TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals;
        	    I++)
                  if (!TT_IntervalsChecked[I])
                    {
                     Cols = TT_CalculateColsToDrawInCell (false,	// Recursive call
                                                          Weekday,I);
                     if (Cols > ColumnsToDraw)
                        ColumnsToDraw = Cols;
                    }
               break;
            case TT_NEXT_INTERVAL:
               /* Find first hour for this item (class) */
               for (FirstHour = Interval;
        	    TT_TimeTable[Weekday][FirstHour].Columns[Column].IntervalType == TT_NEXT_INTERVAL;
        	    FirstHour--);

               /* Check from first hour to last hour searching maximum number of columns */
               for (I = FirstHour;
        	    I < FirstHour + TT_TimeTable[Weekday][FirstHour].Columns[Column].DurationNumIntervals;
        	    I++)
                  if (!TT_IntervalsChecked[I])
                    {
                     Cols = TT_CalculateColsToDrawInCell (false,	// Recursive call
                                                          Weekday,I);
                     if (Cols > ColumnsToDraw)
                        ColumnsToDraw = Cols;
                    }
               break;
           }
        }
     }

   if (TopCall)	// Top call, non recursive call
      /****** Free space used by list of intervals already checked *****/
      free ((void *) TT_IntervalsChecked);

   return ColumnsToDraw;
  }

/*****************************************************************************/
/******************** Write empty cell for alignment *************************/
/*****************************************************************************/

static void TT_DrawCellAlignTimeTable (void)
  {
   fprintf (Gbl.F.Out,"<td class=\"TT_ALIGN\""
	              " style=\"width:%u%%;\">"
	              "</td>",
            TT_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN);
  }

/*****************************************************************************/
/*************************** Write a timetable cell **************************/
/*****************************************************************************/

static void TT_TimeTableDrawCell (unsigned Weekday,unsigned Interval,unsigned Column,unsigned ColSpan,
                                  long CrsCod,TT_IntervalType_t IntervalType,TT_ClassType_t ClassType,
                                  unsigned DurationNumIntervals,char *Group,long GrpCod,char *Place)
  {
   extern const char *Txt_unknown_removed_course;
   extern const char *Txt_TIMETABLE_CLASS_TYPES[TT_NUM_CLASS_TYPES];
   extern const char *Txt_Group;
   extern const char *Txt_All_groups;
   extern const char *Txt_Classroom;
   extern const char *Txt_Place;
   static const char *TimeTableClasses[TT_NUM_CLASS_TYPES] =
     {
      "TT_FREE",	// Free hour
      "TT_THEO",	// Theoretical class
      "TT_PRAC",	// Practical class
      "TT_TUTO"		// Tutorials
     };
   enum
     {
      TT_CRS_SHOW,
      TT_CRS_EDIT,
      TT_TUT_SHOW,
      TT_TUT_EDIT,
     } TimeTableView = TT_CRS_SHOW;
   struct GroupData GrpDat;
   unsigned NumGrpTyp;
   unsigned NumGrp;
   unsigned I;
   unsigned Dur;
   unsigned MaxDuration;
   unsigned RowSpan = 0;
   TT_ClassType_t CT;
   struct Course Crs;

   /***** Compute row span and background color depending on hour type *****/
   switch (IntervalType)
     {
      case TT_FREE_INTERVAL:	// Free cell written
	 RowSpan = 1;
	 break;
      case TT_FIRST_INTERVAL:	// Normal cell written
	 RowSpan = DurationNumIntervals;
	 break;
      case TT_NEXT_INTERVAL:	// Nothing written
	 break;
     }

   /***** If there's nothing to do... *****/
   if (RowSpan == 0)
      return;

   /***** Set type of view depending on current action *****/
   switch (Gbl.Action.Act)
     {
      case ActSeeCrsTT:		case ActPrnCrsTT:	case ActChgCrsTT1stDay:
      case ActSeeMyTT:		case ActPrnMyTT:	case ActChgMyTT1stDay:
	 TimeTableView = TT_CRS_SHOW;
	 break;
      case ActEdiCrsTT:		case ActChgCrsTT:
	 TimeTableView = TT_CRS_EDIT;
	 break;
      case ActSeeRecOneTch:	case ActSeeRecSevTch:
	 TimeTableView = TT_TUT_SHOW;
	 break;
      case ActEdiTut:		case ActChgTut:
	 TimeTableView = TT_TUT_EDIT;
	 break;
     }

   /***** If group code > 0, a group is selected ==> get group type and name *****/
   if (IntervalType == TT_FIRST_INTERVAL &&
       (TimeTableView == TT_CRS_SHOW ||
        TimeTableView == TT_CRS_EDIT) &&
       GrpCod > 0)
     {
      /* Get group type and name */
      GrpDat.GrpCod = GrpCod;
      Grp_GetDataOfGroupByCod (&GrpDat);
     }

   /***** Cell start *****/
   fprintf (Gbl.F.Out,"<td rowspan=\"%u\" colspan=\"%u\" class=\"%s",
            RowSpan,ColSpan,TimeTableClasses[ClassType]);
   if (ClassType == TT_FREE)
      fprintf (Gbl.F.Out,"%u",Interval % 4);
   fprintf (Gbl.F.Out," CENTER_MIDDLE DAT_SMALL\">");

   /***** Form to modify this cell *****/
   if (TimeTableView == TT_CRS_EDIT)
      Act_FormStart (ActChgCrsTT);
   else if (TimeTableView == TT_TUT_EDIT)
      Act_FormStart (ActChgTut);

   /***** Draw cell depending on type of view *****/
   switch (TimeTableView)
     {
      case TT_CRS_SHOW:	// View course timetable
      case TT_TUT_SHOW:	// View tutoring timetable
	 if (IntervalType != TT_FREE_INTERVAL) // If cell is not empty...
	   {
	    /***** Start cell *****/
	    fprintf (Gbl.F.Out,"<div class=\"TT_CELL TT_TXT\">");

	    /***** Course name *****/
	    if (Gbl.TimeTable.Type == TT_MY_TIMETABLE)
              {
               Crs.CrsCod = CrsCod;
               Crs_GetDataOfCourseByCod (&Crs);
               if (ClassType == TT_LECTURE ||
                   ClassType == TT_PRACTICAL)
		  fprintf (Gbl.F.Out,"%s<br />",
		           Crs.ShrtName[0] ? Crs.ShrtName :
			                     Txt_unknown_removed_course);
              }

	    /***** Type of class and duration *****/
	    fprintf (Gbl.F.Out,"%s (%u:%02u h)",
		     Txt_TIMETABLE_CLASS_TYPES[ClassType],
	             (DurationNumIntervals / Gbl.TimeTable.Config.IntervalsPerHour),	// Hours
	             (DurationNumIntervals % Gbl.TimeTable.Config.IntervalsPerHour) *
	             Gbl.TimeTable.Config.MinutesPerInterval);				// Minutes

	    /***** Group *****/
	    if (TimeTableView == TT_CRS_SHOW)
	      {
               if (GrpCod <= 0)
                 {
	          if (Group[0])
		     fprintf (Gbl.F.Out,"<br />%s",Group);
                 }
               else
		  fprintf (Gbl.F.Out,"<br />%s %s",
                           GrpDat.GrpTypName,GrpDat.GrpName);
	      }

	    /***** Place *****/
	    if (Place[0])
	       fprintf (Gbl.F.Out,"<br />%s",Place);

	    /***** End cell *****/
            fprintf (Gbl.F.Out,"</div>");
	   }
	 break;
      case TT_CRS_EDIT:
      case TT_TUT_EDIT:
         Par_PutHiddenParamUnsigned ("ModTTDay",Weekday);
         Par_PutHiddenParamUnsigned ("ModTTHour",Interval);
         Par_PutHiddenParamUnsigned ("ModTTCol",Column);

	 /***** Class type *****/
	 fprintf (Gbl.F.Out,"<select name=\"ModTTClassType\" style=\"width:68px;\""
	                    " onchange=\"document.getElementById('%s').submit();\">",
	          Gbl.Form.Id);
	 for (CT = (TT_ClassType_t) 0;
	      CT < (TT_ClassType_t) TT_NUM_CLASS_TYPES;
	      CT++)
	    if ((CT == TT_FREE) ||
		((TimeTableView == TT_CRS_EDIT) && (CT == TT_LECTURE || CT == TT_PRACTICAL)) ||
		((TimeTableView == TT_TUT_EDIT) && (CT == TT_TUTORING)))
	      {
	       fprintf (Gbl.F.Out,"<option");
	       if (CT == ClassType)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
	       fprintf (Gbl.F.Out," value=\"%s\">%s</option>",
		        TimeTableStrsClassTypeDB[CT],
		        Txt_TIMETABLE_CLASS_TYPES[CT]);
	      }
	 fprintf (Gbl.F.Out,"</select>");

	 if (IntervalType == TT_FREE_INTERVAL)
	   {
	    fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"ModTTDur\" value=\"");
            for (I = Interval + 1;
        	 I < Gbl.TimeTable.Config.IntervalsPerDay;
        	 I++)
              if (TT_TimeTable[Weekday][I].NumColumns == TT_MAX_COLUMNS_PER_CELL)
                  break;
            MaxDuration = I - Interval;
	    Dur = (MaxDuration >= Gbl.TimeTable.Config.IntervalsPerHour) ? Gbl.TimeTable.Config.IntervalsPerHour :	// MaxDuration >= 1h ==> Dur = 1h
	                                                                   MaxDuration;			// MaxDuration  < 1h ==> Dur = MaxDuration
	    fprintf (Gbl.F.Out,"%u:%02u h\" />",
		     (Dur / Gbl.TimeTable.Config.IntervalsPerHour),	// Hours
		     (Dur % Gbl.TimeTable.Config.IntervalsPerHour) *
		     Gbl.TimeTable.Config.MinutesPerInterval);		// Minutes
	   }
	 else
	   {
	    /***** Class duration *****/
	    fprintf (Gbl.F.Out,"<select name=\"ModTTDur\" style=\"width:57px;\""
		               " onchange=\"document.getElementById('%s').submit();\">",
		     Gbl.Form.Id);
            for (I = Interval + TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals;
        	 I < Gbl.TimeTable.Config.IntervalsPerDay;
        	 I++)
               if (TT_TimeTable[Weekday][I].NumColumns == TT_MAX_COLUMNS_PER_CELL)
                  break;
            MaxDuration = I - Interval;
            if (TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals > MaxDuration)
               MaxDuration = TT_TimeTable[Weekday][Interval].Columns[Column].DurationNumIntervals;
	    for (Dur = 0;
		 Dur <= MaxDuration;
		 Dur++)
	      {
	       fprintf (Gbl.F.Out,"<option");
	       if (Dur == DurationNumIntervals)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
	       fprintf (Gbl.F.Out,">%u:%02u h</option>",
		        (Dur / Gbl.TimeTable.Config.IntervalsPerHour),	// Hours
		        (Dur % Gbl.TimeTable.Config.IntervalsPerHour) *
		        Gbl.TimeTable.Config.MinutesPerInterval);	// Minutes
	      }
	    fprintf (Gbl.F.Out,"</select>");

	    if (TimeTableView == TT_CRS_EDIT)
	      {
	       /***** Group *****/
	       fprintf (Gbl.F.Out,"<br />"
		                  "<label>"
		                  "%s"
	                          "<select name=\"ModTTGrpCod\""
	                          " style=\"width:110px;\""
		                  " onchange=\"document.getElementById('%s').submit();\">",
		        Txt_Group,Gbl.Form.Id);
               fprintf (Gbl.F.Out,"<option value=\"-1\"");
	       if (GrpCod <= 0)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
               fprintf (Gbl.F.Out,">%s</option>",Txt_All_groups);
               for (NumGrpTyp = 0;
        	    NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
        	    NumGrpTyp++)
                  for (NumGrp = 0;
                       NumGrp < Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps;
                       NumGrp++)
                    {
	             fprintf (Gbl.F.Out,"<option value=\"%ld\"",
	        	      Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrp].GrpCod);
	             if (GrpCod == Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrp].GrpCod)
		        fprintf (Gbl.F.Out," selected=\"selected\"");
	             fprintf (Gbl.F.Out,">%s %s</option>",
	        	      Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,
                              Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrp].GrpName);
                    }
	       fprintf (Gbl.F.Out,"</select>"
		                  "</label>");

	       /***** Class room *****/
	       fprintf (Gbl.F.Out,"<br />"
		                  "<label>"
		                  "%s"
	                          "<input type=\"text\" name=\"ModTTPlace\""
	                          " size=\"1\" maxlength=\"%u\" value=\"%s\""
		                  " onchange=\"document.getElementById('%s').submit();\" />"
		                  "</label>",
		        Txt_Classroom,TT_MAX_CHARS_PLACE,Place,Gbl.Form.Id);
	      }
	    else // TimeTableView == TT_TUT_EDIT
	      {
	       /***** Place *****/
	       fprintf (Gbl.F.Out,"<br />"
		                  "<label class=\"DAT_SMALL\">"
		                  "%s"
                                  "<input type=\"text\" name=\"ModTTPlace\""
                                  " size=\"12\" maxlength=\"%u\" value=\"%s\""
		                  " onchange=\"document.getElementById('%s').submit();\" />"
		                  "</label>",
		        Txt_Place,TT_MAX_CHARS_PLACE,Place,Gbl.Form.Id);
	      }
	   }
	 break;
     }

   /***** End of form *****/
   if (TimeTableView == TT_CRS_EDIT ||
       TimeTableView == TT_TUT_EDIT)
      Act_FormEnd ();

   /***** End of cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }
