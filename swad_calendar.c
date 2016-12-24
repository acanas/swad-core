// swad_calendar.c: Draw month and calendar

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <string.h>		// For string functions

#include "swad_exam.h"
#include "swad_calendar.h"
#include "swad_database.h"
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

static void Cal_PutIconToPrintCalendar (void);

/*****************************************************************************/
/************** Put icons to select the first day of the week ****************/
/*****************************************************************************/

void Cal_PutIconsToSelectFirstDayOfWeek (void)
  {
   extern const char *Hlp_PROFILE_Preferences_calendar;
   extern const char *Txt_Calendar;

   Lay_StartRoundFrame (NULL,Txt_Calendar,
                        Cal_PutIconsFirstDayOfWeek,
                        Hlp_PROFILE_Preferences_calendar);
   Cal_ShowFormToSelFirstDayOfWeek (ActChg1stDay,NULL,"ICO40x40B");
   Lay_EndRoundFrame ();
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
	 Act_FormStart (Action);
	 Par_PutHiddenParamUnsigned ("FirstDayOfWeek",FirstDayOfWeek);
	 if (FuncParams)	// Extra parameters depending on the action
	    FuncParams ();
	 fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/first-day-of-week-%u-64x64.png\""
			    " alt=\"%s\" title=\"%s: %s\" class=\"%s\" />",
		  Gbl.Prefs.IconsURL,
		  FirstDayOfWeek,
		  Txt_DAYS_SMALL[FirstDayOfWeek],
		  Txt_First_day_of_the_week,Txt_DAYS_SMALL[FirstDayOfWeek],
		  ClassIcon);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</div>");
        }
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************* Change first day of week **************************/
/*****************************************************************************/

void Cal_ChangeFirstDayOfWeek (void)
  {
   char Query[512];

   /***** Get param with icon set *****/
   Gbl.Prefs.FirstDayOfWeek = Cal_GetParamFirstDayOfWeek ();

   /***** Store icon set in database *****/
   if (Gbl.Usrs.Me.Logged)
     {
      sprintf (Query,"UPDATE usr_data SET FirstDayOfWeek='%u'"
	             " WHERE UsrCod='%ld'",
               Gbl.Prefs.FirstDayOfWeek,
               Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryUPDATE (Query,"can not update your preference about first day of week");
     }

   /***** Set preferences from current IP *****/
   Pre_SetPrefsFromIP ();
  }

/*****************************************************************************/
/*********************** Get parameter with icon set *************************/
/*****************************************************************************/

static unsigned Cal_GetParamFirstDayOfWeek (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;
   unsigned FirstDayOfWeek = Cal_FIRST_DAY_OF_WEEK_DEFAULT;

   Par_GetParToText ("FirstDayOfWeek",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (Cal_DayIsValidAsFirstDayOfWeek[UnsignedNum])
	 FirstDayOfWeek = UnsignedNum;

   return FirstDayOfWeek;
  }

/*****************************************************************************/
/***************************** Draw current month ****************************/
/*****************************************************************************/

void Cal_DrawCurrentMonth (void)
  {
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char ParamsStr[256+256+Ses_LENGTH_SESSION_ID+256];

   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrderType = Hld_ORDER_BY_START_DATE;
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
	    Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   Act_SetParamsForm (ParamsStr,ActSeeCal,true);
   fprintf (Gbl.F.Out,"'%s',",ParamsStr);
   Act_SetParamsForm (ParamsStr,ActSeeDatExaAnn,true);
   fprintf (Gbl.F.Out,"'%s');"
	              "</script>",ParamsStr);
  }

/*****************************************************************************/
/************************ Draw an academic calendar **************************/
/*****************************************************************************/
/* Current     Starting
    month       month
      1    ->    10
      2    ->    10
      3    ->    10

      4    ->     1
      5    ->     1
      6    ->     1

      7    ->     4
      8    ->     4
      9    ->     4

     10    ->     7
     11    ->     7
     12    ->     7
*/
void Cal_DrawCalendar (void)
  {
   extern const char *Hlp_ASSESSMENT_Calendar;
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char ParamsStr[256+256+Ses_LENGTH_SESSION_ID+256];
   bool PrintView = (Gbl.Action.Act == ActPrnCal);

   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrderType = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays ();
     }

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,NULL,
                        PrintView ? NULL :
	                            Cal_PutIconToPrintCalendar,
	                PrintView ? NULL :
	                            Hlp_ASSESSMENT_Calendar);
   Lay_WriteHeaderClassPhoto (PrintView,false,
			      Gbl.CurrentIns.Ins.InsCod,
			      Gbl.CurrentDeg.Deg.DegCod,
			      Gbl.CurrentCrs.Crs.CrsCod);

   /***** Draw several months *****/
   /* Show form to change first day of week */
   if (!PrintView)
      Cal_ShowFormToSelFirstDayOfWeek (ActChgCal1stDay,NULL,"ICO25x25");

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
	    (Gbl.Action.Act == ActPrnCal) ? "true" :
		                            "false",
	    Cfg_URL_SWAD_CGI,
	    Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   Act_SetParamsForm (ParamsStr,ActSeeCal,true);
   fprintf (Gbl.F.Out,"'%s',",
            ParamsStr);
   Act_SetParamsForm (ParamsStr,ActSeeDatExaAnn,true);
   fprintf (Gbl.F.Out,"'%s');"
	              "</script>",
	    ParamsStr);

   /***** End frame *****/
   Lay_EndRoundFrame ();
  }

/*****************************************************************************/
/************************ Put icon to print calendar *************************/
/*****************************************************************************/

static void Cal_PutIconToPrintCalendar (void)
  {
   extern const char *Txt_Print;

   Lay_PutContextualLink (ActPrnCal,NULL,
                          "print64x64.png",
                          Txt_Print,NULL,
                          NULL);
  }
