// swad_calendar.c: Draw month and calendar

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_preference.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

const bool Cal_DayIsValidAsFirstDayOfWeek[7] =
  {
   true,	// 0: monday
   false,	// 1: tuesday
   false,	// 2: wednesday
   false,	// 3: thursday
   false,	// 4: friday
   false,	// 5: saturday
   true,	// 6: sunday
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cal_PutIconsFirstDayOfWeek (void);

static unsigned Cal_GetParamFirstDayOfWeek (void);

static void Cal_DrawCalendar (Act_Action_t ActionSeeCalendar,
                              Act_Action_t ActionChangeCalendar1stDay,
                              void (*FunctionToDrawContextualIcons) (void),
                              bool PrintView);
static void Cal_PutIconToPrintCalendarSys (void);
static void Cal_PutIconToPrintCalendarCty (void);
static void Cal_PutIconToPrintCalendarIns (void);
static void Cal_PutIconToPrintCalendarCtr (void);
static void Cal_PutIconToPrintCalendarDeg (void);
static void Cal_PutIconToPrintCalendarCrs (void);

/*****************************************************************************/
/************** Put icons to select the first day of the week ****************/
/*****************************************************************************/

void Cal_PutIconsToSelectFirstDayOfWeek (void)
  {
   extern const char *Hlp_PROFILE_Preferences_calendar;
   extern const char *Txt_Calendar;

   Box_StartBox (NULL,Txt_Calendar,Cal_PutIconsFirstDayOfWeek,
                 Hlp_PROFILE_Preferences_calendar,Box_NOT_CLOSABLE);
   Cal_ShowFormToSelFirstDayOfWeek (ActChg1stDay,NULL,"ICOx25");
   Box_EndBox ();
  }

/*****************************************************************************/
/*********** Put contextual icons in first-day-of-week preference ************/
/*****************************************************************************/

static void Cal_PutIconsFirstDayOfWeek (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_FIRST_DAY_OF_WEEK;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/************** Show form to select the first day of the week ****************/
/*****************************************************************************/

void Cal_ShowFormToSelFirstDayOfWeek (Act_Action_t Action,void (*FuncParams) (),
                                      const char *ClassIcon)
  {
   extern const char *Txt_First_day_of_the_week;
   extern const char *Txt_DAYS_SMALL[7];
   unsigned FirstDayOfWeek;

   fprintf (Gbl.F.Out,"<div class=\"PREF_CONTAINER\">");
   for (FirstDayOfWeek = 0;	// Monday
	FirstDayOfWeek <= 6;	// Sunday
	FirstDayOfWeek++)
      if (Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
	{
         fprintf (Gbl.F.Out,"<div class=\"%s\">",
		  FirstDayOfWeek == Gbl.Prefs.FirstDayOfWeek ? "PREF_ON" :
							       "PREF_OFF");
	 Frm_StartForm (Action);
	 Par_PutHiddenParamUnsigned ("FirstDayOfWeek",FirstDayOfWeek);
	 if (FuncParams)	// Extra parameters depending on the action
	    FuncParams ();
	 fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/first-day-of-week-%u.png\""
			    " alt=\"%s\" title=\"%s: %s\" class=\"%s\" />",
		  Gbl.Prefs.URLIcons,
		  FirstDayOfWeek,
		  Txt_DAYS_SMALL[FirstDayOfWeek],
		  Txt_First_day_of_the_week,Txt_DAYS_SMALL[FirstDayOfWeek],
		  ClassIcon);
	 Frm_EndForm ();
	 fprintf (Gbl.F.Out,"</div>");
        }
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************* Change first day of week **************************/
/*****************************************************************************/

void Cal_ChangeFirstDayOfWeek (void)
  {
   /***** Get param with icon set *****/
   Gbl.Prefs.FirstDayOfWeek = Cal_GetParamFirstDayOfWeek ();

   /***** Store icon first day of week database *****/
   if (Gbl.Usrs.Me.Logged)
      DB_QueryUPDATE ("can not update your preference about first day of week",
		      "UPDATE usr_data SET FirstDayOfWeek=%u"
		      " WHERE UsrCod=%ld",
                      Gbl.Prefs.FirstDayOfWeek,
                      Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/*********************** Get parameter with icon set *************************/
/*****************************************************************************/

static unsigned Cal_GetParamFirstDayOfWeek (void)
  {
   unsigned FirstDayOfWeek;

   FirstDayOfWeek = (unsigned)
	            Par_GetParToUnsignedLong ("FirstDayOfWeek",
                                              0,
                                              6,
                                              Cal_FIRST_DAY_OF_WEEK_DEFAULT);
   if (!Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
      FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;

   return FirstDayOfWeek;
  }

/*****************************************************************************/
/******************** Get first day of week from string **********************/
/*****************************************************************************/

unsigned Cal_GetFirstDayOfWeekFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (Cal_DayIsValidAsFirstDayOfWeek[UnsignedNum])
         return (Dat_Format_t) UnsignedNum;

   return Cal_FIRST_DAY_OF_WEEK_DEFAULT;
  }

/*****************************************************************************/
/***************************** Draw current month ****************************/
/*****************************************************************************/

void Cal_DrawCurrentMonth (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char ParamsStr[Frm_MAX_BYTES_PARAMS_STR];

   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrder = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays ();
     }

   /***** Draw the month in JavaScript *****/
   /* JavaScript will write HTML here */
   fprintf (Gbl.F.Out,"<div id=\"CurrentMonth\">"
	              "</div>");

   /* Write script to draw the month */
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "	Gbl_HTMLContent = '';"
	              "	DrawCurrentMonth ('CurrentMonth',%u,%ld,%ld,'%s/%s',",
	    Gbl.Prefs.FirstDayOfWeek,
	    (long) Gbl.StartExecutionTimeUTC,
	    Gbl.CurrentCtr.Ctr.PlcCod,
	    Cfg_URL_SWAD_CGI,
	    Lan_STR_LANG_ID[Gbl.Prefs.Language]);
   Frm_SetParamsForm (ParamsStr,Cal_GetActionToSeeCalendar (),true);
   fprintf (Gbl.F.Out,"'%s',",ParamsStr);
   Frm_SetParamsForm (ParamsStr,ActSeeDatExaAnn,true);
   fprintf (Gbl.F.Out,"'%s');"
	              "</script>",ParamsStr);
  }

/*****************************************************************************/
/********** Get action to see calendar depending on current scope ************/
/*****************************************************************************/

Act_Action_t Cal_GetActionToSeeCalendar ()
  {
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
      return ActSeeCalCrs;

   if (Gbl.CurrentDeg.Deg.DegCod > 0)	// Degree selected
      return ActSeeCalDeg;

   if (Gbl.CurrentCtr.Ctr.CtrCod > 0)	// Centre selected
      return ActSeeCalCtr;

   if (Gbl.CurrentIns.Ins.InsCod > 0)	// Institution selected
      return ActSeeCalIns;

   if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected
      return ActSeeCalCty;

   return ActSeeCalSys;
  }

/*****************************************************************************/
/************************ Draw an academic calendar **************************/
/*****************************************************************************/

void Cal_DrawCalendarSys (void)
  {
   Cal_DrawCalendar (ActSeeCalSys,ActChgCalSys1stDay,Cal_PutIconToPrintCalendarSys,false);
  }

void Cal_DrawCalendarCty (void)
  {
   Cal_DrawCalendar (ActSeeCalCty,ActChgCalCty1stDay,Cal_PutIconToPrintCalendarCty,false);
  }

void Cal_DrawCalendarIns (void)
  {
   Cal_DrawCalendar (ActSeeCalIns,ActChgCalIns1stDay,Cal_PutIconToPrintCalendarIns,false);
  }

void Cal_DrawCalendarCtr (void)
  {
   Cal_DrawCalendar (ActSeeCalCtr,ActChgCalCtr1stDay,Cal_PutIconToPrintCalendarCtr,false);
  }

void Cal_DrawCalendarDeg (void)
  {
   Cal_DrawCalendar (ActSeeCalDeg,ActChgCalDeg1stDay,Cal_PutIconToPrintCalendarDeg,false);
  }

void Cal_DrawCalendarCrs (void)
  {
   Cal_DrawCalendar (ActSeeCalCrs,ActChgCalCrs1stDay,Cal_PutIconToPrintCalendarCrs,false);
  }

void Cal_PrintCalendar (void)
  {
   Cal_DrawCalendar (ActUnk,ActUnk,NULL,true);
  }

static void Cal_DrawCalendar (Act_Action_t ActionSeeCalendar,
                              Act_Action_t ActionChangeCalendar1stDay,
                              void (*FunctionToDrawContextualIcons) (void),
                              bool PrintView)
  {
   extern const char *Hlp_Calendar;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char ParamsStr[Frm_MAX_BYTES_PARAMS_STR];

   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrder = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays ();
     }

   /***** Start box *****/
   Box_StartBox (NULL,NULL,FunctionToDrawContextualIcons,
	         PrintView ? NULL :
	                     Hlp_Calendar,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (PrintView,false,
			      Gbl.CurrentIns.Ins.InsCod,
			      Gbl.CurrentDeg.Deg.DegCod,
			      Gbl.CurrentCrs.Crs.CrsCod);

   /***** Draw several months *****/
   /* Show form to change first day of week */
   if (!PrintView)
      Cal_ShowFormToSelFirstDayOfWeek (ActionChangeCalendar1stDay,NULL,"ICOx25");

   /* JavaScript will write HTML here */
   fprintf (Gbl.F.Out,"<div id=\"calendar\">"
	              "</div>");

   /* Write script to draw the month */
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "	Gbl_HTMLContent = '';"
	              "	Cal_DrawCalendar('calendar',%u,%ld,%ld,%s,'%s/%s',",
	    Gbl.Prefs.FirstDayOfWeek,
	    (long) Gbl.StartExecutionTimeUTC,
	    Gbl.CurrentCtr.Ctr.PlcCod,
	    PrintView ? "true" :
		        "false",
	    Cfg_URL_SWAD_CGI,
	    Lan_STR_LANG_ID[Gbl.Prefs.Language]);
   Frm_SetParamsForm (ParamsStr,ActionSeeCalendar,true);
   fprintf (Gbl.F.Out,"'%s',",
            ParamsStr);
   Frm_SetParamsForm (ParamsStr,ActSeeDatExaAnn,true);
   fprintf (Gbl.F.Out,"'%s');"
	              "</script>",
	    ParamsStr);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/************************ Put icon to print calendar *************************/
/*****************************************************************************/

static void Cal_PutIconToPrintCalendarSys (void)
  {
   Ico_PutContextualIconToPrint (ActPrnCalSys,NULL);
  }

static void Cal_PutIconToPrintCalendarCty (void)
  {
   Ico_PutContextualIconToPrint (ActPrnCalCty,NULL);
  }

static void Cal_PutIconToPrintCalendarIns (void)
  {
   extern const char *Txt_Holidays;

   /***** Print calendar *****/
   Ico_PutContextualIconToPrint (ActPrnCalIns,NULL);

   /***** View holidays *****/
   if (Gbl.Hlds.Num ||					// There are holidays
       Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)	// Institution admin or system admin
      Lay_PutContextualLink (ActSeeHld,NULL,NULL,
			     "holiday.png",
			     Txt_Holidays,NULL,
			     NULL);
  }

static void Cal_PutIconToPrintCalendarCtr (void)
  {
   Ico_PutContextualIconToPrint (ActPrnCalCtr,NULL);
  }

static void Cal_PutIconToPrintCalendarDeg (void)
  {
   Ico_PutContextualIconToPrint (ActPrnCalDeg,NULL);
  }

static void Cal_PutIconToPrintCalendarCrs (void)
  {
   Ico_PutContextualIconToPrint (ActPrnCalCrs,NULL);
  }
