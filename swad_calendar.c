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
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Draw current month ****************************/
/*****************************************************************************/

void Cal_DrawCurrentMonth (void)
  {
   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
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
	              "	DrawCurrentMonth ('CurrentMonth',%ld,%ld,'%s/%s',",
	    (long) Gbl.StartExecutionTimeUTC,
	    Gbl.CurrentCtr.Ctr.PlcCod,
	    Cfg_HTTPS_URL_SWAD_CGI,Txt_STR_LANG_ID[Gbl.Prefs.Language]);
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
      1    ->     9
      2    ->     9
      3    ->     9
      4    ->     9

      5    ->     1
      6    ->     1
      7    ->     1
      8    ->     1

      9    ->     5
     10    ->     5
     11    ->     5
     12    ->     5
*/
void Cal_DrawCalendar (void)
  {
   extern const char *Txt_Print;
   bool PrintView = (Gbl.CurrentAct == ActPrnCal);

   extern const char *Txt_STR_LANG_ID[Txt_NUM_LANGUAGES];
   char Params[256+256+Ses_LENGTH_SESSION_ID+256];

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
      Act_PutContextualLink (ActPrnCal,NULL,"print",Txt_Print);
      fprintf (Gbl.F.Out,"</div>");
     }
   Lay_StartRoundFrameTable (NULL,0,NULL);
   Lay_WriteHeaderClassPhoto (1,PrintView,false,
			      Gbl.CurrentIns.Ins.InsCod,
			      Gbl.CurrentDeg.Deg.DegCod,
			      Gbl.CurrentCrs.Crs.CrsCod);

   /***** Draw several months *****/
   /* JavaScript will write HTML here */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"CENTER_TOP\">"
	              "<div id=\"calendar\">"
	              "</div>");

   /* Write script to draw the month */
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "	Gbl_HTMLContent = '';"
	              "	Cal_DrawCalendar('calendar',%ld,%ld,%s,'%s/%s',",
	    (long) Gbl.StartExecutionTimeUTC,
	    Gbl.CurrentCtr.Ctr.PlcCod,
	    (Gbl.CurrentAct == ActPrnCal) ? "true" :
		                            "false",
	    Cfg_HTTPS_URL_SWAD_CGI,Txt_STR_LANG_ID[Gbl.Prefs.Language]);
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
