// swad_timetable.c: timetables

/*
    SWAD (Shared Workspace At a Distance),
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

#define TT_START_HOUR				  6	// Day starts at 6:00
#define TT_HOURS_PER_DAY			 16	// From 6:00 to 22:00
#define TT_DAYS					  7	// Seven days per week
#define TT_MAX_COLUMNS_PER_CELL			  3	// Maximum number of items (i.e. classes) in a timetable cell (1, 2, 3 or 4)
#define TT_NUM_MINICOLUMNS_PER_DAY		  6	// Least common multiple of 1,2,3,...,TT_MAX_COLUMNS_PER_CELL
#define TT_PERCENT_WIDTH_OF_A_MINICOLUMN	  2	// Width (%) of each minicolumn
#define TT_PERCENT_WIDTH_OF_A_DAY		(TT_PERCENT_WIDTH_OF_A_MINICOLUMN*TT_NUM_MINICOLUMNS_PER_DAY)	// Width (%) of each day
#define TT_PERCENT_WIDTH_OF_ALL_DAYS		(TT_PERCENT_WIDTH_OF_A_DAY*TT_DAYS)				// Width (%) of all days
#define TT_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN	  1	// Width (%) of left and right columns (frame)
#define TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN 	 ((100-TT_PERCENT_WIDTH_OF_ALL_DAYS-TT_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN*2)/2)	// Width (%) of the separation columns

#define TT_MAX_BYTES_STR_CLASS_TYPE		256
#define TT_MAX_BYTES_STR_DURATION		((2+1+2+1+1)*Str_LENGTH_STR_HEX)	// "hh:mm h"
#define TT_MAX_LENGTH_PLACE			 32

/*****************************************************************************/
/******************************* Internal types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

char TimeTableCharsDays[TT_DAYS] =	// TODO: Change from Spanish to numbers, also in database
 {
  'L',	// Monday
  'M',	// Tuesday
  'X',	// Wednesday
  'J',	// Thursday
  'V',	// Friday
  'S',	// Saturday
  'D',	// Sunday
 };

char *TimeTableStrsClassTypeDB[TT_NUM_CLASS_TYPES] =	// TODO: Change from Spanish to English, also in database
  {
   "libre",
   "teoria",
   "practicas",
   "tutorias",
  };

struct TimeTableColumn
  {
   long CrsCod;		// Course code (-1 if no course selected)
   long GrpCod;		// Group code (-1 if no group selected)
   TT_HourType_t HourType;
   TT_ClassType_t ClassType;
   unsigned Duration;
   char Place[TT_MAX_BYTES_PLACE+1];
   char Group[TT_MAX_BYTES_GROUP+1];
  };
struct
  {
   unsigned NumColumns;
   struct TimeTableColumn Columns[TT_MAX_COLUMNS_PER_CELL];
  } TimeTable[TT_DAYS][TT_HOURS_PER_DAY*2];
bool TimeTableHoursChecked[TT_HOURS_PER_DAY*2];

/*****************************************************************************/
/***************************** Internal prototypes **************************/
/*****************************************************************************/

static void TT_ShowTimeTableGrpsSelected (void);
static void TT_GetParamsTimeTable (void);
static void TT_PutContextualIcons (void);
static void TT_ShowSelectorWhichGrps (Act_Action_t Action);

static void TT_PutIconToViewCrsTT (void);
static void TT_PutIconToViewMyTT (void);

static void TT_WriteCrsTimeTableIntoDB (long CrsCod);
static void TT_WriteTutTimeTableIntoDB (long UsrCod);
static void TT_CreatTimeTableFromDB (long UsrCod);
static void TT_ModifTimeTable (void);
static void TT_DrawTimeTable (void);
static void TT_TimeTableDrawAdjustRow (void);
static void TT_TimeTableDrawDaysCells (void);
static unsigned TT_TimeTableCalculateColsToDraw (unsigned Day,unsigned Hour);
static void TT_DrawCellAlignTimeTable (void);
static void TT_TimeTableDrawCell (unsigned Day,unsigned Hour,unsigned Column,unsigned ColSpan,
                                  long CrsCod,TT_HourType_t HourType,TT_ClassType_t ClassType,unsigned Duration,char *Group,long GrpCod,char *Place);

/*****************************************************************************/
/*********** Show whether only my groups or all groups are shown *************/
/*****************************************************************************/

static void TT_ShowTimeTableGrpsSelected (void)
  {
   extern const char *Txt_Groups_OF_A_USER;
   extern const char *Txt_All_groups;

   fprintf (Gbl.F.Out,"<div class=\"TIT_CLASSPHOTO CENTER_MIDDLE\">");
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
   char UnsignedStr[10+1];
   char LongStr[1+10+1];
   char StrClassType[TT_MAX_BYTES_STR_CLASS_TYPE+1];
   char StrDuration[TT_MAX_BYTES_STR_DURATION+1];
   unsigned Hours,Minutes;

   /***** Get day (0: monday, 1: tuesday,..., 4: friday *****/
   Par_GetParToText ("ModTTDay",UnsignedStr,2);
   if (sscanf (UnsignedStr,"%u",&Gbl.TimeTable.Day) != 1)
      Lay_ShowErrorAndExit ("Day is missing.");

   /***** Get hour *****/
   Par_GetParToText ("ModTTHour",UnsignedStr,2);
   if (sscanf (UnsignedStr,"%u",&Gbl.TimeTable.Hour) != 1)
      Lay_ShowErrorAndExit ("Hour is missing.");

   /***** Get number of column *****/
   Par_GetParToText ("ModTTCol",UnsignedStr,2);
   if (sscanf (UnsignedStr,"%u",&Gbl.TimeTable.Column) != 1)
      Lay_ShowErrorAndExit ("Column is missing.");

   /***** Get class type *****/
   Par_GetParToText ("ModTTCellType",StrClassType,TT_MAX_BYTES_STR_CLASS_TYPE);
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
   Gbl.TimeTable.Duration = Hours * 2 + Minutes / 30;

   /***** Get group *****/
   Par_GetParToText ("ModHorGrp",Gbl.TimeTable.Group,TT_MAX_BYTES_GROUP);

   /***** Get group code *****/
   Par_GetParToText ("ModTTGrpCod",LongStr,1+10);
   if (LongStr[0])
     {
      if (sscanf (LongStr,"%ld",&Gbl.TimeTable.GrpCod) != 1)
         Lay_ShowErrorAndExit ("Wrong code of group.");
     }
   else
      Gbl.TimeTable.GrpCod = -1;

   /***** Get place *****/
   Par_GetParToText ("ModHorLugar",Gbl.TimeTable.Place,TT_MAX_BYTES_PLACE);
  }

/*****************************************************************************/
/**************************** Show class timetable ***************************/
/*****************************************************************************/

void TT_ShowClassTimeTable (void)
  {
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];
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
   Gbl.TimeTable.ContextualIcons.PutIconPrint = !PrintView;
   Gbl.TimeTable.ContextualIcons.PutIconEditCrsTT = (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE &&
	                                             !PrintView &&
                                                     Gbl.Usrs.Me.LoggedRole >= Rol_TEACHER);
   Gbl.TimeTable.ContextualIcons.PutIconEditOfficeHours = (Gbl.TimeTable.Type == TT_MY_TIMETABLE &&
	                                                   !PrintView &&
                                                           (Gbl.Usrs.Me.AvailableRoles & (1 << Rol_TEACHER)));

   /***** Get whether to show only my groups or all groups *****/
   Grp_GetParamWhichGrps ();

   /***** Start frame *****/
   Lay_StartRoundFrame ("100%",Txt_TIMETABLE_TYPES[Gbl.TimeTable.Type],
                        (Gbl.TimeTable.ContextualIcons.PutIconEditCrsTT ||
                         Gbl.TimeTable.ContextualIcons.PutIconEditOfficeHours ||
                         Gbl.TimeTable.ContextualIcons.PutIconPrint) ? TT_PutContextualIcons :
                                                                       NULL,
                        NULL);

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
      TT_ShowSelectorWhichGrps (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE ? ActSeeCrsTT :
	                                                                    ActSeeMyTT);

      /***** Show form to change first day of week *****/
      Cal_ShowFormToSelFirstDayOfWeek (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE ? ActChgCrsTT1stDay :
	                                                                           ActChgMyTT1stDay,
	                               "ICON25x25");
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
      Lay_PutContextualLink (ActEdiCrsTT,Grp_PutParamWhichGrps,
			     "edit64x64.png",
			     Txt_Edit,NULL,
		             NULL);

   if (Gbl.TimeTable.ContextualIcons.PutIconEditOfficeHours)
      Lay_PutContextualLink (ActEdiTut,NULL,
                             "edit64x64.png",
			     Txt_Edit_office_hours,NULL,
		             NULL);

   if (Gbl.TimeTable.ContextualIcons.PutIconPrint)
      Lay_PutContextualLink (Gbl.TimeTable.Type == TT_COURSE_TIMETABLE ? ActPrnCrsTT :
								         ActPrnMyTT,
			     Grp_PutParamWhichGrps,
			     "print64x64.png",
			     Txt_Print,NULL,
		             NULL);
  }

/*****************************************************************************/
/************* Select whether show only my groups or all groups **************/
/*****************************************************************************/

static void TT_ShowSelectorWhichGrps (Act_Action_t Action)
  {
   Act_FormStart (Action);
   Grp_ShowSelectorWhichGrps ();
   Act_FormEnd ();
  }

/*****************************************************************************/
/********************** Show course timetable for edition ********************/
/*****************************************************************************/

void TT_EditCrsTimeTable (void)
  {
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];

   /***** Editable time table *****/
   Gbl.TimeTable.Type = TT_COURSE_TIMETABLE;
   Lay_StartRoundFrame ("100%",Txt_TIMETABLE_TYPES[Gbl.TimeTable.Type],
                        TT_PutIconToViewCrsTT,NULL);
   TT_ShowTimeTable (Gbl.Usrs.Me.UsrDat.UsrCod);
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/********************* Show tutor timetable for edition **********************/
/*****************************************************************************/

void TT_ShowMyTutTimeTable (void)
  {
   extern const char *Txt_TIMETABLE_TYPES[TT_NUM_TIMETABLE_TYPES];

   /***** Time table *****/
   Gbl.TimeTable.Type = TT_TUTOR_TIMETABLE;
   Lay_StartRoundFrame ("100%",Txt_TIMETABLE_TYPES[Gbl.TimeTable.Type],
                        TT_PutIconToViewMyTT,NULL);
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
   Lay_PutContextualLink (ActSeeCrsTT,NULL,
                          "clock64x64.gif",
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
   Lay_PutContextualLink (ActSeeMyTT,NULL,
                          "clock64x64.gif",
                          Txt_Show_timetable,NULL,
		          NULL);
  }

/*****************************************************************************/
/*********** Show course timetable or tutor timetable of a teacher ***********/
/*****************************************************************************/

void TT_ShowTimeTable (long UsrCod)
  {
   /***** Create an internal table with the timetable from database *****/
   TT_CreatTimeTableFromDB (UsrCod);

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
         case TT_TUTOR_TIMETABLE:
            TT_WriteTutTimeTableIntoDB (UsrCod);
            break;
         default:
            break;
        }

      /* Get a new table from database */
      TT_CreatTimeTableFromDB (UsrCod);
     }

   /***** Draw timetable *****/
   TT_DrawTimeTable ();
  }

/*****************************************************************************/
/******************* Write course timetable into database ********************/
/*****************************************************************************/

static void TT_WriteCrsTimeTableIntoDB (long CrsCod)
  {
   char Query[1024];
   unsigned Hour,Day,Column;

   /***** Remove former timetable *****/
   sprintf (Query,"DELETE FROM timetable_crs WHERE CrsCod='%ld'",CrsCod);
   DB_QueryDELETE (Query,"can not remove former timetable");

   /***** Go across the timetable inserting classes into database *****/
   for (Day = 0;
	Day < TT_DAYS;
	Day++)
      for (Hour = 0;
	   Hour < TT_HOURS_PER_DAY * 2;
	   Hour++)
         for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
	    if (TimeTable[Day][Hour].Columns[Column].HourType == TT_FIRST_HOUR &&
                TimeTable[Day][Hour].Columns[Column].Duration > 0)
              {
               sprintf (Query,"INSERT INTO timetable_crs (CrsCod,GrpCod,Day,Hour,Duration,ClassType,Place,GroupName)"
                              " VALUES ('%ld','%ld','%c','%u','%d','%s','%s','%s')",
                        CrsCod,
			TimeTable[Day][Hour].Columns[Column].GrpCod,
			TimeTableCharsDays[Day],
			Hour,
                        TimeTable[Day][Hour].Columns[Column].Duration,
                        TimeTableStrsClassTypeDB[TimeTable[Day][Hour].Columns[Column].ClassType],
                        TimeTable[Day][Hour].Columns[Column].Place,
		        TimeTable[Day][Hour].Columns[Column].Group);
               DB_QueryINSERT (Query,"can not create course timetable");
              }
  }

/*****************************************************************************/
/********************* Write tutor timetable into database *******************/
/*****************************************************************************/

static void TT_WriteTutTimeTableIntoDB (long UsrCod)
  {
   char Query[1024];
   unsigned Hour,Day,Column;

   /***** Remove former timetable *****/
   sprintf (Query,"DELETE FROM timetable_tut WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove former timetable");

   /***** Loop over timetable *****/
   for (Day = 0;
	Day < TT_DAYS;
	Day++)
      for (Hour = 0;
	   Hour < TT_HOURS_PER_DAY * 2;
	   Hour++)
         for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
	    if (TimeTable[Day][Hour].Columns[Column].HourType == TT_FIRST_HOUR &&
                TimeTable[Day][Hour].Columns[Column].Duration > 0)
              {
               sprintf (Query,"INSERT INTO timetable_tut (UsrCod,Day,Hour,Duration,Place)"
                              " VALUES ('%ld','%c','%u','%d','%s')",
                        UsrCod,TimeTableCharsDays[Day],Hour,
                        TimeTable[Day][Hour].Columns[Column].Duration,
			TimeTable[Day][Hour].Columns[Column].Place);
               DB_QueryINSERT (Query,"can not create office timetable");
              }
  }

/*****************************************************************************/
/********** Create an internal table with timetable from database ************/
/*****************************************************************************/

static void TT_CreatTimeTableFromDB (long UsrCod)
  {
   extern const char *Txt_Incomplete_timetable_for_lack_of_space;
   char Query[4096];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   unsigned Hour,Day,Column,FirstFreeColumn,Duration,H;
   long GrpCod;
   TT_ClassType_t ClassType = TT_NO_CLASS;	// Initialized to avoid warning
   bool TimeTableIsIncomplete = false;
   bool TimeTableHasSpaceForThisClass;
   bool Found;

   /***** Initialize timetable to all free *****/
   for (Day = 0;
	Day < TT_DAYS;
	Day++)
      for (Hour = 0;
	   Hour < TT_HOURS_PER_DAY * 2;
	   Hour++)
        {
         TimeTable[Day][Hour].NumColumns = 0;
         for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
	   {
	    TimeTable[Day][Hour].Columns[Column].CrsCod    = -1L;
	    TimeTable[Day][Hour].Columns[Column].GrpCod    = -1L;
	    TimeTable[Day][Hour].Columns[Column].HourType  = TT_FREE_HOUR;
	    TimeTable[Day][Hour].Columns[Column].ClassType = TT_NO_CLASS;
	    TimeTable[Day][Hour].Columns[Column].Duration  = 0;
	    TimeTable[Day][Hour].Columns[Column].Group[0]  = '\0';
	    TimeTable[Day][Hour].Columns[Column].Place[0]  = '\0';
	   }
        }

   /***** Get timetable from database *****/
   switch (Gbl.TimeTable.Type)
     {
      case TT_MY_TIMETABLE:
         switch (Gbl.CurrentCrs.Grps.WhichGrps)
           {
            case Grp_ONLY_MY_GROUPS:
               sprintf (Query,"SELECT timetable_crs.Day,timetable_crs.Hour,timetable_crs.Duration,timetable_crs.Place,"
                              "timetable_crs.ClassType,timetable_crs.GroupName,timetable_crs.GrpCod,timetable_crs.CrsCod"
                              " FROM timetable_crs,crs_usr"
                              " WHERE crs_usr.UsrCod='%ld' AND timetable_crs.GrpCod='-1' AND timetable_crs.CrsCod=crs_usr.CrsCod"
                              " UNION DISTINCT "
                              "SELECT timetable_crs.Day,timetable_crs.Hour,timetable_crs.Duration,timetable_crs.Place,"
                              "timetable_crs.ClassType,timetable_crs.GroupName,timetable_crs.GrpCod,timetable_crs.CrsCod"
                              " FROM timetable_crs,crs_grp_usr"
                              " WHERE crs_grp_usr.UsrCod='%ld' AND timetable_crs.GrpCod=crs_grp_usr.GrpCod"
                              " UNION "
                              "SELECT Day,Hour,Duration,Place,"
                              "'tutorias' AS ClassType,'' AS GroupName,'-1' AS GrpCod,'-1' AS CrsCod"
                              " FROM timetable_tut"
                              " WHERE UsrCod='%ld'"
                              " ORDER BY Day,Hour,ClassType,GroupName,GrpCod,Place,Duration DESC,CrsCod",
                        UsrCod,UsrCod,UsrCod);
               break;
            case Grp_ALL_GROUPS:
               sprintf (Query,"SELECT timetable_crs.Day,timetable_crs.Hour,timetable_crs.Duration,timetable_crs.Place,"
                              "timetable_crs.ClassType,timetable_crs.GroupName,timetable_crs.GrpCod,timetable_crs.CrsCod"
                              " FROM timetable_crs,crs_usr"
                              " WHERE crs_usr.UsrCod='%ld' AND timetable_crs.CrsCod=crs_usr.CrsCod"
                              " UNION "
                              "SELECT Day,Hour,Duration,Place,"
                              "'tutorias' AS ClassType,'' AS GroupName,'-1' AS GrpCod,'-1' AS CrsCod"
                              " FROM timetable_tut"
                              " WHERE UsrCod='%ld'"
                              " ORDER BY Day,Hour,ClassType,"
                              "GroupName,GrpCod,Place,Duration DESC,CrsCod",
                        UsrCod,UsrCod);
               break;
           }
	 break;
      case TT_COURSE_TIMETABLE:
         if (Gbl.CurrentCrs.Grps.WhichGrps == Grp_ALL_GROUPS ||
             Gbl.Action.Act == ActEdiCrsTT ||
             Gbl.Action.Act == ActChgCrsTT)	// If we are editing, all groups are shown
            sprintf (Query,"SELECT Day,Hour,Duration,Place,ClassType,GroupName,GrpCod"
        	           " FROM timetable_crs"
                           " WHERE CrsCod='%ld'"
                           " ORDER BY Day,Hour,ClassType,GroupName,GrpCod,Place,Duration DESC",
                     Gbl.CurrentCrs.Crs.CrsCod);
         else
            sprintf (Query,"SELECT timetable_crs.Day,timetable_crs.Hour,timetable_crs.Duration,timetable_crs.Place,timetable_crs.ClassType,timetable_crs.GroupName,timetable_crs.GrpCod"
                           " FROM timetable_crs,crs_usr"
                           " WHERE timetable_crs.CrsCod='%ld' AND timetable_crs.GrpCod='-1' AND crs_usr.UsrCod='%ld' AND timetable_crs.CrsCod=crs_usr.CrsCod"
                           " UNION DISTINCT "
                           "SELECT timetable_crs.Day,timetable_crs.Hour,timetable_crs.Duration,timetable_crs.Place,"
                           "timetable_crs.ClassType,timetable_crs.GroupName,timetable_crs.GrpCod"
			   " FROM timetable_crs,crs_grp_usr"
                           " WHERE timetable_crs.CrsCod='%ld' AND crs_grp_usr.UsrCod='%ld' AND timetable_crs.GrpCod=crs_grp_usr.GrpCod"
                           " ORDER BY Day,Hour,ClassType,GroupName,GrpCod,Place,Duration DESC",
                     Gbl.CurrentCrs.Crs.CrsCod,UsrCod,
                     Gbl.CurrentCrs.Crs.CrsCod,UsrCod);
	 break;
      case TT_TUTOR_TIMETABLE:
         sprintf (Query,"SELECT Day,Hour,Duration,Place"
                        " FROM timetable_tut"
                        " WHERE UsrCod='%ld'"
                        " ORDER BY Day,Hour,Place,Duration DESC",
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
      for (Day = 0;
	   Day < TT_DAYS;
	   Day++)
	 if (Str_ConvertToUpperLetter (row[0][0]) == TimeTableCharsDays[Day])
	    break;
      if (Day == TT_DAYS)
	 Lay_ShowErrorAndExit ("Wrong day of week in timetable.");

      /* Hour (row[1]) */
      if (sscanf (row[1],"%u",&Hour) != 1)
	 Lay_ShowErrorAndExit ("Wrong hour in timetable.");

      /* Duration (row[2]) */
      if (sscanf (row[2],"%u",&Duration) != 1)
	 Lay_ShowErrorAndExit ("Wrong duration in timetable.");

      /* Type of class */
      switch (Gbl.TimeTable.Type)
        {
         case TT_COURSE_TIMETABLE:
         case TT_MY_TIMETABLE:
            for (ClassType = TT_THEORY_CLASS, Found = false;
        	 ClassType <= TT_TUTOR_CLASS;
        	 ClassType++)
  	       if (!strcmp (row[4],TimeTableStrsClassTypeDB[ClassType]))
  		 {
  		  Found = true;
                  break;
                 }
            if (!Found)
	       Lay_ShowErrorAndExit ("Wrong type of class in timetable.");
            break;
         case TT_TUTOR_TIMETABLE:
            ClassType = TT_TUTOR_CLASS;
            break;
        }

      /* Cell has been read without errors. */
      if (TimeTable[Day][Hour].NumColumns < TT_MAX_COLUMNS_PER_CELL)	// If there's place for another column in this cell...
        {
         /* Find the first free column for this day-hour */
         FirstFreeColumn = TT_MAX_COLUMNS_PER_CELL;
         for (Column = 0;
              Column < TT_MAX_COLUMNS_PER_CELL;
              Column++)
            if (TimeTable[Day][Hour].Columns[Column].HourType == TT_FREE_HOUR)
              {
               FirstFreeColumn = Column;
               break;
              }
         if (FirstFreeColumn < TT_MAX_COLUMNS_PER_CELL)	// If there's place for another column in this cell
           {
            /* Check if there's place for all the rows of this class */
            TimeTableHasSpaceForThisClass = true;
            for (H = Hour + 1;
        	 H < Hour + Duration && H < TT_HOURS_PER_DAY * 2;
        	 H++)
               if (TimeTable[Day][H].Columns[FirstFreeColumn].HourType != TT_FREE_HOUR)
                {
                  TimeTableIsIncomplete = true;
                  TimeTableHasSpaceForThisClass = false;
                  break;
                 }
            if (TimeTableHasSpaceForThisClass)
              {
               TimeTable[Day][Hour].Columns[FirstFreeColumn].ClassType = ClassType;
               TimeTable[Day][Hour].Columns[FirstFreeColumn].Duration  = Duration;
               TimeTable[Day][Hour].Columns[FirstFreeColumn].HourType  = TT_FIRST_HOUR;
               for (H = Hour + 1;
        	    H < Hour + Duration && H < TT_HOURS_PER_DAY * 2;
        	    H++)
                 {
	          TimeTable[Day][H].Columns[FirstFreeColumn].HourType = TT_NEXT_HOUR;
	          TimeTable[Day][H].NumColumns++;
                 }

               /* Group and place */
               switch (Gbl.TimeTable.Type)
                 {
                  case TT_MY_TIMETABLE:
                  case TT_COURSE_TIMETABLE:
                     TimeTable[Day][Hour].Columns[FirstFreeColumn].CrsCod =
                        (Gbl.TimeTable.Type == TT_MY_TIMETABLE ? Str_ConvertStrCodToLongCod (row[7]) :
                                                                 Gbl.CurrentCrs.Crs.CrsCod);
                     strcpy (TimeTable[Day][Hour].Columns[FirstFreeColumn].Group,row[5]);
                     TimeTable[Day][Hour].Columns[FirstFreeColumn].GrpCod = GrpCod;
                     // no break;
                  case TT_TUTOR_TIMETABLE:
                     strcpy (TimeTable[Day][Hour].Columns[FirstFreeColumn].Place,row[3]);
                     break;
                 }

               /* Increment number of items in this cell */
               TimeTable[Day][Hour].NumColumns++;
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
   if (TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].HourType == TT_FIRST_HOUR)
     {
      /***** Free this cell *****/
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].GrpCod    = -1;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].HourType  = TT_FREE_HOUR;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].ClassType = TT_NO_CLASS;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].Duration  = 0;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].Group[0]  = '\0';
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].Place[0]  = '\0';
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].NumColumns--;
     }

   if (Gbl.TimeTable.ClassType != TT_NO_CLASS && Gbl.TimeTable.Duration > 0 &&
       TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].NumColumns < TT_MAX_COLUMNS_PER_CELL)
     {
      /***** Change this cell *****/
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].NumColumns++;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].GrpCod    = Gbl.TimeTable.GrpCod;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].HourType  = TT_FIRST_HOUR;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].ClassType = Gbl.TimeTable.ClassType;
      TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].Duration  = Gbl.TimeTable.Duration;
      strcpy (TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].Group,Gbl.TimeTable.Group);
      strcpy (TimeTable[Gbl.TimeTable.Day][Gbl.TimeTable.Hour].Columns[Gbl.TimeTable.Column].Place,Gbl.TimeTable.Place);
     }
  }

/*****************************************************************************/
/********************* Draw timetable using internal table *******************/
/*****************************************************************************/

static void TT_DrawTimeTable (void)
  {
   bool Editing = false;
   unsigned DayColumn;	// Column from left (0) to right (6)
   unsigned Day;	// Day of week
   unsigned Hour;
   unsigned H;
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
	              "<td rowspan=\"2\" class=\"TT_TXT RIGHT_MIDDLE\""
	              " style=\"width:%u%%; height:40px;\">"
	              "%u:00"
	              "</td>",
            TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN,TT_START_HOUR);
   TT_DrawCellAlignTimeTable ();
   TT_TimeTableDrawDaysCells ();
   TT_DrawCellAlignTimeTable ();
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"TT_TXT LEFT_MIDDLE\""
	              " style=\"width:%u%%; height:40px;\">"
	              "%u:00"
	              "</td>"
	              "</tr>",
            TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN,TT_START_HOUR);

   /***** Get list of groups types and groups in this course *****/
   if (Gbl.Action.Act == ActEdiCrsTT ||
       Gbl.Action.Act == ActChgCrsTT)
      Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   /***** Write the table row by row *****/
   for (Hour = 0;
	Hour < TT_HOURS_PER_DAY * 2;
	Hour++)
     {
      fprintf (Gbl.F.Out,"<tr>");

      /* Hour */
      if (Hour % 2)
         fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"TT_TXT RIGHT_MIDDLE\""
                            " style=\"width:%u%%; height:40px;\">"
                            "%2u:00"
                            "</td>",
                  TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN,TT_START_HOUR + (Hour+1)/2);

      /* Empty column used to adjust height */
      TT_DrawCellAlignTimeTable ();

      /* Row for this hour */
      for (DayColumn = 0;
	   DayColumn < TT_DAYS;
	   DayColumn++)
        {
	 /* Day == 0 ==> monday,
	            ...
	    Day == 6 ==> sunday */
	 Day = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;

         /* Check how many colums are needed.
            For each item (class) in this hour from left to right, we must check the maximum of columns */
         for (H = 0;
              H < TT_HOURS_PER_DAY * 2;
              H++)
            TimeTableHoursChecked[H] = false;
         ColumnsToDraw = TT_TimeTableCalculateColsToDraw (Day,Hour);
         if (!Editing && ColumnsToDraw == 0)
            ColumnsToDraw = 1;
         ColumnsToDrawIncludingExtraColumn = ColumnsToDraw;
         if (Editing && ColumnsToDraw < TT_MAX_COLUMNS_PER_CELL)
            ColumnsToDrawIncludingExtraColumn++;

        /* Draw cells */
         for (Column = 0, ContinuousFreeMinicolumns = 0;
              Column < ColumnsToDrawIncludingExtraColumn;
              Column++)
            if (TimeTable[Day][Hour].Columns[Column].HourType == TT_FREE_HOUR)
               ContinuousFreeMinicolumns += TT_NUM_MINICOLUMNS_PER_DAY / ColumnsToDrawIncludingExtraColumn;
            else
              {
               if (ContinuousFreeMinicolumns)
                 {
                  TT_TimeTableDrawCell (Day,Hour,Column-1,ContinuousFreeMinicolumns,
                                        -1L,TT_FREE_HOUR,TT_NO_CLASS,0,NULL,-1,NULL);
                  ContinuousFreeMinicolumns = 0;
                 }
               TT_TimeTableDrawCell (Day,Hour,Column,TT_NUM_MINICOLUMNS_PER_DAY/ColumnsToDrawIncludingExtraColumn,
	                             TimeTable[Day][Hour].Columns[Column].CrsCod,
				     TimeTable[Day][Hour].Columns[Column].HourType,
	                             TimeTable[Day][Hour].Columns[Column].ClassType,
                                     TimeTable[Day][Hour].Columns[Column].Duration,
	                             TimeTable[Day][Hour].Columns[Column].Group,
	                             TimeTable[Day][Hour].Columns[Column].GrpCod,
                                     TimeTable[Day][Hour].Columns[Column].Place);
              }
         if (ContinuousFreeMinicolumns)
            TT_TimeTableDrawCell (Day,Hour,Column-1,ContinuousFreeMinicolumns,
                                  -1L,TT_FREE_HOUR,TT_NO_CLASS,0,NULL,-1L,NULL);
        }

      /* Empty column used to adjust height */
      TT_DrawCellAlignTimeTable ();

      /* Hour */
      if (Hour % 2)
         fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"TT_TXT LEFT_MIDDLE\""
                            " style=\"width:%u%%; height:40px;\">"
                            "%2u:00"
                            "</td>",
                  TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN,TT_START_HOUR + (Hour+1)/2);

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
   unsigned Day;
   unsigned Minicolumn;

   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TT_TXT LEFT_MIDDLE\" style=\"width:%u%%;\">"
                      "&nbsp;"
                      "</td>",
            TT_PERCENT_WIDTH_OF_AN_HOUR_COLUMN);
   TT_DrawCellAlignTimeTable ();
   for (Day = 0;
	Day < TT_DAYS;
	Day++)
      for (Minicolumn = 0;
	   Minicolumn < TT_NUM_MINICOLUMNS_PER_DAY;
	   Minicolumn++)
         fprintf (Gbl.F.Out,"<td class=\"TT_TXT LEFT_MIDDLE\""
                            " style=\"width:%u%%;\">"
			    "&nbsp;"
                            "</td>",
                  TT_PERCENT_WIDTH_OF_A_MINICOLUMN);
   TT_DrawCellAlignTimeTable ();
   fprintf (Gbl.F.Out,"<td class=\"TT_TXT LEFT_MIDDLE\""
	              " style=\"width:%u%%;\">"
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
   unsigned Day;

   for (DayColumn = 0;
	DayColumn < TT_DAYS;
	DayColumn++)
     {
      Day = (DayColumn + Gbl.Prefs.FirstDayOfWeek) % 7;
      fprintf (Gbl.F.Out,"<td colspan=\"%u\" class=\"%s CENTER_MIDDLE\""
	                 " style=\"width:%u%%;\">"
	                 "%s"
	                 "</td>",
               TT_NUM_MINICOLUMNS_PER_DAY,
               Day == 6 ? "TT_SUNDAY" :	// Sunday drawn in red
        	          "TT_DAY",	// Monday to Saturday
               TT_PERCENT_WIDTH_OF_A_DAY,
               Txt_DAYS_CAPS[Day]);
     }
  }

/*****************************************************************************/
/**** Calculate recursively number of columns to draw for a day and hour *****/
/*****************************************************************************/

static unsigned TT_TimeTableCalculateColsToDraw (unsigned Day,unsigned Hour)
  {
   unsigned ColumnsToDraw;
   unsigned Column;
   unsigned H;
   unsigned FirstHour;
   unsigned Cols;

   ColumnsToDraw = TimeTable[Day][Hour].NumColumns;

   if (!TimeTableHoursChecked[Hour])
     {
      TimeTableHoursChecked[Hour] = true;
      for (Column = 0;
	   Column < TT_MAX_COLUMNS_PER_CELL;
	   Column++)
        {
         switch (TimeTable[Day][Hour].Columns[Column].HourType)
           {
            case TT_FREE_HOUR:
               break;
            case TT_FIRST_HOUR:
               /* Check from first hour (this one) to last hour searching maximum number of columns */
               for (H = Hour + 1;
        	    H < Hour + TimeTable[Day][Hour].Columns[Column].Duration;
        	    H++)
                  if (!TimeTableHoursChecked[H])
                    {
                     Cols = TT_TimeTableCalculateColsToDraw (Day,H);
                     if (Cols > ColumnsToDraw)
                        ColumnsToDraw = Cols;
                    }
               break;
            case TT_NEXT_HOUR:
               /* Find first hour for this item (class) */
               for (FirstHour = Hour;
        	    TimeTable[Day][FirstHour].Columns[Column].HourType == TT_NEXT_HOUR;
        	    FirstHour--);
                 /* Check from first hour to last hour searching maximum number of columns */
               for (H = FirstHour;
        	    H < FirstHour + TimeTable[Day][FirstHour].Columns[Column].Duration;
        	    H++)
                  if (!TimeTableHoursChecked[H])
                    {
                     Cols = TT_TimeTableCalculateColsToDraw (Day,H);
                     if (Cols > ColumnsToDraw)
                        ColumnsToDraw = Cols;
                    }
               break;
           }
        }
     }
   return ColumnsToDraw;
  }

/*****************************************************************************/
/******************** Write empty cell for alignment *************************/
/*****************************************************************************/

static void TT_DrawCellAlignTimeTable (void)
  {
   fprintf (Gbl.F.Out,"<td class=\"TT_TXT LEFT_MIDDLE\" style=\"width:%u%%;\">"
                      "&nbsp;"
	              "</td>",
            TT_PERCENT_WIDTH_OF_A_SEPARATION_COLUMN);
  }

/*****************************************************************************/
/*************************** Write a timetable cell **************************/
/*****************************************************************************/

static void TT_TimeTableDrawCell (unsigned Day,unsigned Hour,unsigned Column,unsigned ColSpan,
                                  long CrsCod,TT_HourType_t HourType,TT_ClassType_t ClassType,unsigned Duration,char *Group,long GrpCod,char *Place)
  {
   extern const char *Txt_unknown_removed_course;
   extern const char *Txt_TIMETABLE_CLASS_TYPES[TT_NUM_CLASS_TYPES];
   extern const char *Txt_Group;
   extern const char *Txt_All_groups;
   extern const char *Txt_Classroom;
   extern const char *Txt_Place;
   static const char *TimeTableClasses[TT_NUM_CLASS_TYPES] =
     {
      "FREE",	// Free hour
      "THEO",	// Theoretical class
      "PRAC",	// Practical class
      "TUTO"	// Tutorials
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
   unsigned H;
   unsigned Dur;
   unsigned MaxDuration;
   unsigned RowSpan = 0;
   TT_ClassType_t CT;
   struct Course Crs;

   /***** Compute row span and background color depending on hour type *****/
   switch (HourType)
     {
      case TT_FREE_HOUR:	// Free cell written
	 RowSpan = 1;
	 break;
      case TT_FIRST_HOUR:	// Normal cell written
	 RowSpan = Duration;
	 break;
      case TT_NEXT_HOUR:	// Nothing written
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
   if (HourType == TT_FIRST_HOUR &&
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
   if (ClassType == TT_NO_CLASS)
      fprintf (Gbl.F.Out,"%u",Hour % 4);
   fprintf (Gbl.F.Out," CENTER_MIDDLE\">");

   /***** Form to modify this cell *****/
   if (TimeTableView == TT_CRS_EDIT)
      Act_FormStart (ActChgCrsTT);
   else if (TimeTableView == TT_TUT_EDIT)
      Act_FormStart (ActChgTut);

   /***** Draw cell depending on type of view *****/
   switch (TimeTableView)
     {
      case TT_CRS_SHOW:
      case TT_TUT_SHOW:
	 if (HourType != TT_FREE_HOUR) // If cell is not empty...
	   {
	    fprintf (Gbl.F.Out,"<span class=\"TT_TXT\">");
	    if (Gbl.TimeTable.Type == TT_MY_TIMETABLE)
              {
               Crs.CrsCod = CrsCod;
               Crs_GetDataOfCourseByCod (&Crs);
               if (ClassType == TT_THEORY_CLASS ||
                   ClassType == TT_PRACT_CLASS)
		  fprintf (Gbl.F.Out,"%s<br />",
		           Crs.ShrtName[0] ? Crs.ShrtName :
			                     Txt_unknown_removed_course);
              }
	    fprintf (Gbl.F.Out,"%s (%dh%s)",
		     Txt_TIMETABLE_CLASS_TYPES[ClassType],
	             Duration / 2,
	             Duration % 2 ? "30'" :
	        	            "");
	    if (TimeTableView == TT_CRS_SHOW)
	      {
               if (GrpCod == -1)
                 {
	          if (Group[0])
		     fprintf (Gbl.F.Out,"<br />%s",Group);
                 }
               else
                 {
                  Str_LimitLengthHTMLStr (GrpDat.GrpTypName,12);
                  Str_LimitLengthHTMLStr (GrpDat.GrpName,12);
		  fprintf (Gbl.F.Out,"<br />%s %s",
                           GrpDat.GrpTypName,GrpDat.GrpName);
                 }
	       if (Place[0])
		  fprintf (Gbl.F.Out,"<br />%s",Place);
	      }
	    else	// TimeTableView == TT_TUT_SHOW
	       if (Place[0])
		  fprintf (Gbl.F.Out,"<br />%s",Place);
            fprintf (Gbl.F.Out,"</span>");
	   }
	 break;
      case TT_CRS_EDIT:
      case TT_TUT_EDIT:
         Par_PutHiddenParamUnsigned ("ModTTDay",Day);
         Par_PutHiddenParamUnsigned ("ModTTHour",Hour);
         Par_PutHiddenParamUnsigned ("ModTTCol",Column);

	 fprintf (Gbl.F.Out,"<span class=\"DAT_SMALL\">");

	 /***** Class type *****/
	 fprintf (Gbl.F.Out,"<select name=\"ModTTCellType\" style=\"width:68px;\""
	                    " onchange=\"document.getElementById('%s').submit();\">",
	          Gbl.Form.Id);
	 for (CT = (TT_ClassType_t) 0;
	      CT < (TT_ClassType_t) TT_NUM_CLASS_TYPES;
	      CT++)
	    if ((CT == TT_NO_CLASS) ||
		((TimeTableView == TT_CRS_EDIT) && (CT == TT_THEORY_CLASS || CT == TT_PRACT_CLASS)) ||
		((TimeTableView == TT_TUT_EDIT) && (CT == TT_TUTOR_CLASS)))
	      {
	       fprintf (Gbl.F.Out,"<option");
	       if (CT == ClassType)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
	       fprintf (Gbl.F.Out," value=\"%s\">%s</option>",
		        TimeTableStrsClassTypeDB[CT],
		        Txt_TIMETABLE_CLASS_TYPES[CT]);
	      }
	 fprintf (Gbl.F.Out,"</select>");
	 if (HourType == TT_FREE_HOUR)
	   {
	    fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"ModTTDur\" value=\"");
            for (H = Hour + 1;
        	 H < TT_HOURS_PER_DAY * 2;
        	 H++)
              if (TimeTable[Day][H].NumColumns == TT_MAX_COLUMNS_PER_CELL)
                  break;
            MaxDuration = H - Hour;
	    if (MaxDuration > 1)
	       fprintf (Gbl.F.Out,"1:00");
	    else
	       fprintf (Gbl.F.Out,"0:30");
	    fprintf (Gbl.F.Out," h\" />");
            Par_PutHiddenParamString ("ModHorGrp","");
            Par_PutHiddenParamString ("ModHorLugar","");
	   }
	 else
	   {
	    /***** Class duration *****/
	    fprintf (Gbl.F.Out,"<select name=\"ModTTDur\" style=\"width:57px;\""
		               " onchange=\"document.getElementById('%s').submit();\">",
		     Gbl.Form.Id);
            for (H = Hour + TimeTable[Day][Hour].Columns[Column].Duration;
        	 H < TT_HOURS_PER_DAY * 2;
        	 H++)
               if (TimeTable[Day][H].NumColumns == TT_MAX_COLUMNS_PER_CELL)
                  break;
            MaxDuration = H - Hour;
            if (TimeTable[Day][Hour].Columns[Column].Duration > MaxDuration)
               MaxDuration = TimeTable[Day][Hour].Columns[Column].Duration;
	    for (Dur = 0;
		 Dur <= MaxDuration;
		 Dur++)
	      {
	       fprintf (Gbl.F.Out,"<option");
	       if (Dur == Duration)
		  fprintf (Gbl.F.Out," selected=\"selected\"");
	       fprintf (Gbl.F.Out,">%d:%02d h</option>",
		        Dur / 2,
		        Dur % 2 ? 30 :
		                  0);
	      }
	    fprintf (Gbl.F.Out,"</select>");

	    if (TimeTableView == TT_CRS_EDIT)
	      {
	       /***** Group *****/
	       fprintf (Gbl.F.Out,"<br />%s"
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
                     Str_LimitLengthHTMLStr (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,8);
                     Str_LimitLengthHTMLStr (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrp].GrpName,8);
	             fprintf (Gbl.F.Out,">%s %s</option>",
	        	      Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypName,
                              Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].LstGrps[NumGrp].GrpName);
                    }
	       fprintf (Gbl.F.Out,"</select>");

	       /***** Class room *****/
	       fprintf (Gbl.F.Out,"<br />%s"
	                          "<input type=\"text\" name=\"ModHorLugar\""
	                          " size=\"1\" maxlength=\"%u\" value=\"%s\""
		                  " onchange=\"document.getElementById('%s').submit();\" />",
		        Txt_Classroom,TT_MAX_LENGTH_PLACE,Place,Gbl.Form.Id);
	      }
	    else // TimeTableView == TT_TUT_EDIT
	      {
               Par_PutHiddenParamString ("ModHorGrp","");
	       /***** Place *****/
	       fprintf (Gbl.F.Out,"<br />%s"
                                  "<input type=\"text\" name=\"ModHorLugar\""
                                  " size=\"12\" maxlength=\"%u\" value=\"%s\""
		                  " onchange=\"document.getElementById('%s').submit();\" />",
		        Txt_Place,TT_MAX_LENGTH_PLACE,Place,Gbl.Form.Id);
	      }
	   }
         fprintf (Gbl.F.Out,"</span>");
	 break;
     }

   /***** End of form *****/
   if (TimeTableView == TT_CRS_EDIT ||
       TimeTableView == TT_TUT_EDIT)
      Act_FormEnd ();

   /***** End of cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }
