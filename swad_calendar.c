// swad_calendar.c: Draw month and calendar

/*
    SWAD (Shared Workspace At a Distance),
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

static void Cal_ShowFormToSelFirstDayOfWeek (Act_Action_t Action,const char *ClassIcon);

static unsigned Cal_GetParamFirstDayOfWeek (void);

/*****************************************************************************/
/************** Put icons to select the first day of the week ****************/
/*****************************************************************************/

void Cal_PutIconsToSelectFirstDayOfWeek (void)
  {
   extern const char *Txt_Calendar;

   Lay_StartRoundFrameTable (NULL,0,Txt_Calendar);
   Cal_ShowFormToSelFirstDayOfWeek (ActChg1stDay,"ICON32x32");
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/************** Show form to select the first day of the week ****************/
/*****************************************************************************/

void Cal_ShowIntegratedFormToSelFirstDayOfWeek (Act_Action_t Action)
  {
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\" style=\"margin:0 auto;\">");
   Cal_ShowFormToSelFirstDayOfWeek (Action,"ICON20x20");
   fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/******** Show form in calendar to select the first day of the week **********/
/*****************************************************************************/

static void Cal_ShowFormToSelFirstDayOfWeek (Act_Action_t Action,const char *ClassIcon)
  {
   extern const char *Txt_First_day_of_the_week;
   extern const char *Txt_DAYS_SMALL[7];
   unsigned FirstDayOfWeek;

   fprintf (Gbl.F.Out,"<tr>");
   for (FirstDayOfWeek = 0;	// Monday
	FirstDayOfWeek <= 6;	// Sunday
	FirstDayOfWeek++)
      if (Cal_DayIsValidAsFirstDayOfWeek[FirstDayOfWeek])
	{
	 fprintf (Gbl.F.Out,"<td class=\"%s\">",
		  FirstDayOfWeek == Gbl.Prefs.FirstDayOfWeek ? "PREF_ON" :
							       "PREF_OFF");
	 Act_FormStart (Action);
	 Par_PutHiddenParamUnsigned ("FirstDayOfWeek",FirstDayOfWeek);
	 fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/first-day-of-week-%u-64x64.png\""
			    " alt=\"%s\" title=\"%s: %s\" class=\"%s\""
			    " style=\"margin:0 auto;\" />",
		  Gbl.Prefs.IconsURL,
		  FirstDayOfWeek,
		  Txt_DAYS_SMALL[FirstDayOfWeek],
		  Txt_First_day_of_the_week,Txt_DAYS_SMALL[FirstDayOfWeek],
		  ClassIcon);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>");
        }
   fprintf (Gbl.F.Out,"</tr>");
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
   char Params[256+256+Ses_LENGTH_SESSION_ID+256];

   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrderType = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays ();
     }

   /***** Create list of dates of exam announcements *****/
   Exa_CreateListOfExamAnnouncements ();

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
	    Cfg_HTTPS_URL_SWAD_CGI,
	    Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   Act_SetParamsForm (Params,ActSeeCal,true);
   fprintf (Gbl.F.Out,"'%s',",Params);
   Act_SetParamsForm (Params,ActSeeExaAnn,true);
   fprintf (Gbl.F.Out,"'%s');"
	              "</script>",Params);

   /***** Free list of dates of exam announcements *****/
   Exa_FreeListExamAnnouncements ();
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
   extern const char *Txt_Print;
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];
   char Params[256+256+Ses_LENGTH_SESSION_ID+256];
   bool PrintView = (Gbl.CurrentAct == ActPrnCal);

   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrderType = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays ();
     }

   /***** Create list of calls for examination *****/
   Exa_CreateListOfExamAnnouncements ();

   /***** Start of table and title *****/
   if (!PrintView)
     {
      /* Link to print view */
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Act_PutContextualLink (ActPrnCal,NULL,"print64x64.png",Txt_Print);
      fprintf (Gbl.F.Out,"</div>");
     }
   Lay_StartRoundFrameTable (NULL,0,NULL);
   Lay_WriteHeaderClassPhoto (1,PrintView,false,
			      Gbl.CurrentIns.Ins.InsCod,
			      Gbl.CurrentDeg.Deg.DegCod,
			      Gbl.CurrentCrs.Crs.CrsCod);

   /***** Draw several months *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"CENTER_TOP\">");

   /* Show form to change first day of week */
   if (!PrintView)
      Cal_ShowIntegratedFormToSelFirstDayOfWeek (ActChgCal1stDay);

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
	    (Gbl.CurrentAct == ActPrnCal) ? "true" :
		                            "false",
	    Cfg_HTTPS_URL_SWAD_CGI,
	    Txt_STR_LANG_ID[Gbl.Prefs.Language]);
   Act_SetParamsForm (Params,ActSeeCal,true);
   fprintf (Gbl.F.Out,"'%s',",
            Params);
   Act_SetParamsForm (Params,ActSeeExaAnn,true);
   fprintf (Gbl.F.Out,"'%s');"
	              "</script>",
	    Params);

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Free list of dates of exam announcements *****/
   Exa_FreeListExamAnnouncements ();

   /***** End frame *****/
   Lay_EndRoundFrameTable ();
  }
