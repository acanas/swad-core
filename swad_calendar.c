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

static void Cal_DrawMonth (unsigned RealYear,unsigned RealMonth,
                           bool DrawingCalendar,bool PutLinkToEvents,bool PrintView);

/*****************************************************************************/
/***************************** Draw current month ****************************/
/*****************************************************************************/

void Cal_DrawCurrentMonth (void)
  {
   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrderType = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays ();
     }

   /***** Create list of dates of exam announcements *****/
   Exa_CreateListOfExamAnnouncements ();

   /***** Draw the month *****/
   Cal_DrawMonth (Gbl.Now.Date.Year,Gbl.Now.Date.Month,false,true,false);

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
   static unsigned StartingMonth[1+12] =	// Calendar starts one row before current month
     {
      0,	// Not used
      9,	// January   --> September
      9,	// February  --> September
      9,	// Mars      --> September
      9,	// April     --> September
      1,	// May       --> January
      1,	// June      --> January
      1,	// July      --> January
      1,	// Agoust    --> January
      5,	// September --> May
      5,	// October   --> May
      5,	// November  --> May
      5,	// December  --> May
     };
   unsigned Row,Col;
   unsigned Month = StartingMonth[Gbl.Now.Date.Month];
   unsigned Year = (Month < Gbl.Now.Date.Month) ? Gbl.Now.Date.Year :
	                                          Gbl.Now.Date.Year - 1;
   bool PrintView = (Gbl.CurrentAct == ActPrnCal);

   /***** Get list of holidays *****/
   if (!Gbl.Hlds.LstIsRead)
     {
      Gbl.Hlds.SelectedOrderType = Hld_ORDER_BY_START_DATE;
      Hld_GetListHolidays ();
     }

   /***** Start of table and title *****/
   if (!PrintView)
     {
      /* Link to print view */
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Act_PutContextualLink (ActPrnCal,NULL,"print",Txt_Print);
      fprintf (Gbl.F.Out,"</div>");
     }
   Lay_StartRoundFrameTable10 (NULL,0,NULL);
   Lay_WriteHeaderClassPhoto (1,PrintView,false,
			      Gbl.CurrentIns.Ins.InsCod,
			      Gbl.CurrentDeg.Deg.DegCod,
			      Gbl.CurrentCrs.Crs.CrsCod);

   /***** Create list of calls for examination *****/
   Exa_CreateListOfExamAnnouncements ();

   /***** Draw several months *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td style=\"text-align:center;\">"
	              "<table style=\"margin:0 auto; border-spacing:5px;\">");
   for (Row = 0;
	Row < 4;
	Row++)
     {
      fprintf (Gbl.F.Out,"<tr>");
      for (Col = 0;
	   Col < 4;
	   Col++)
	{
	 fprintf (Gbl.F.Out,"<td style=\"width:120px;"
	                    " text-align:center; vertical-align:top;\">");
	 Cal_DrawMonth (Year,Month,true,!PrintView,(Gbl.CurrentAct == ActPrnCal));
	 fprintf (Gbl.F.Out,"</td>");
	 if (++Month == 13)
	   {
	    Month = 1;
	    Year++;
	   }
	}
      fprintf (Gbl.F.Out,"</tr>");
     }
   fprintf (Gbl.F.Out,"</table>"
	              "</td>"
	              "</tr>");

   /***** Free list of dates of exam announcements *****/
   Exa_FreeListExamAnnouncements ();

   /***** End frame *****/
   Lay_EndRoundFrameTable10 (Lay_NO_BUTTON,NULL);
  }

/*****************************************************************************/
/******************************** Draw a month *******************************/
/*****************************************************************************/

static void Cal_DrawMonth (unsigned RealYear,unsigned RealMonth,
                           bool DrawingCalendar,bool PutLinkToEvents,bool PrintView)
  {
   extern const unsigned Dat_NumDaysMonth[1+12];
   extern const char *Txt_Show_calendar;
   extern const char *Txt_DAYS_CAPS[7];
   extern const char *Txt_MONTHS_CAPS[12];
   extern const char *Txt_Exam_of_X;
   char StrExamOfX[512+Crs_MAX_LENGTH_COURSE_FULL_NAME];
   unsigned Week;
   unsigned DayOfWeek; /* 0, 1, 2, 3, 4, 5 ó 6 */
   unsigned DayOfMonth;
   unsigned NumDaysInMonth;
   unsigned Year = RealYear;
   unsigned Month = RealMonth;
   char YYYYMMDD[4+2+2+1];
   unsigned NumHld;
   char *ClassForDay;	// Class of day depending on type of day
   char *TextForDay;	// Text associated to a day, for example the name of the holiday
   unsigned NumExamAnnouncement;	// Number of exam announcement
   int ResultOfCmpStartDate;
   bool ContinueSearching;
   bool PutLinkToCalendar = !DrawingCalendar && Gbl.CurrentCrs.Crs.CrsCod > 0;
   bool ThisDayHasEvent = false;
   bool IsToday;

   /***** Compute number of day of month for the first box *****/
   /* The initial day of month can be -5, -4, -3, -2, -1, 0, or 1
      If it's -5 then write 6 boxes of the previous month.
      If it's -4 then write 5 boxes of the previous month.
      If it's -3 then write 4 boxes of the previous month.
      If it's -2 then write 3 boxes of the previous month.
      If it's -1 then write 2 boxes of the previous month.
      If it's  0 then write 1 box   of the previous month.
      If it's  1 then write 0 boxes of the previous month. */

   if ((DayOfWeek = Dat_GetDayOfWeek (Year,Month,1)) == 0)
      DayOfMonth = 1;
   else
     {
      if (Month <= 1)
	{
	 Month = 12;
	 Year--;
	}
      else
	 Month--;
      NumDaysInMonth = (Month == 2) ? Dat_GetNumDaysFebruary (Year) :
	                              Dat_NumDaysMonth[Month];
      DayOfMonth = NumDaysInMonth - DayOfWeek + 1;
     }

   /***** Start of month *****/
   fprintf (Gbl.F.Out,"<div class=\"MONTH_CONTAINER\">");

   /***** Month name *****/
   fprintf (Gbl.F.Out,"<div class=\"MONTH\">");
   if (PutLinkToCalendar)
     {
      Act_FormStart (ActSeeCal);
      Act_LinkFormSubmit (Txt_Show_calendar,"MONTH");
     }
   fprintf (Gbl.F.Out,"%s %u",
	    Txt_MONTHS_CAPS[RealMonth-1],RealYear);
   if (PutLinkToCalendar)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Month head: first letter for each day of week *****/
   fprintf (Gbl.F.Out,"<table class=\"MONTH_TABLE_DAYS\">"
                      "<tr>");
   for (DayOfWeek = 0;
	DayOfWeek < 7;
	DayOfWeek++)
      fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:center;\">"
	                 "%c"
	                 "</td>",
               (DayOfWeek == 6) ? "DAY_NO_WRK_HEAD" :
        	                  "DAY_WRK_HEAD",
               Txt_DAYS_CAPS[DayOfWeek][0]);
   fprintf (Gbl.F.Out,"</tr>");

   /***** Draw every week of the month *****/
   for (Week = 0;
	Week < 6;
	Week++)
     {
      fprintf (Gbl.F.Out,"<tr>");

      /***** Draw every day of the week *****/
      for (DayOfWeek = 0;
	   DayOfWeek < 7;
	   DayOfWeek++)
	{
         /***** Set class for day being drawn *****/
         ClassForDay = (Month == RealMonth) ? "DAY_WRK" :
                                              "DAY_WRK_LIGHT";
	 TextForDay = NULL;

         /* Check if day is a holiday or a school day */
         sprintf (YYYYMMDD,"%04u%02u%02u",Year,Month,DayOfMonth);
         for (NumHld = 0, ContinueSearching = true;
              NumHld < Gbl.Hlds.Num && ContinueSearching;
              NumHld++)
            if (Gbl.Hlds.Lst[NumHld].PlcCod <= 0 ||
        	Gbl.Hlds.Lst[NumHld].PlcCod == Gbl.CurrentCtr.Ctr.PlcCod)
              {
               ResultOfCmpStartDate = strcmp (Gbl.Hlds.Lst[NumHld].StartDate.YYYYMMDD,YYYYMMDD);
               if (ResultOfCmpStartDate > 0)	// List is ordered by start date. If start date is greater than date being drawn, don't continue searching

                  ContinueSearching = false;
               else	// ResultOfCmpStartDate <= 0 <==> start date <= date being drawn
                  switch (Gbl.Hlds.Lst[NumHld].HldTyp)
                    {
                     case Hld_HOLIDAY:
                        if (ResultOfCmpStartDate == 0)	// If start date == date being drawn
                          {
                           ClassForDay = (Month == RealMonth) ? "DAY_HLD" :
                        	                                "DAY_HLD_LIGHT";
                           TextForDay = Gbl.Hlds.Lst[NumHld].Name;
                           ContinueSearching = false;
                          }
                        break;
                     case Hld_NON_SCHOOL_PERIOD:
                        if (strcmp (Gbl.Hlds.Lst[NumHld].EndDate.YYYYMMDD,YYYYMMDD) >= 0)	// If start date <= date being drawn <= end date
                          {
                           ClassForDay = (Month == RealMonth) ? "DAY_NO_WORK" :
                        	                                "DAY_NO_WORK_LIGHT";
                           TextForDay = Gbl.Hlds.Lst[NumHld].Name;
                          }
                        break;
                    }
              }

         /* Day being drawn is sunday? */
	 if (DayOfWeek == 6) // All the sundays are holidays
	    ClassForDay = (Month == RealMonth) ? "DAY_HLD" :
		                                 "DAY_HLD_LIGHT";

         /* Date being drawn is today? */
	 IsToday = (Gbl.CurrentAct != ActPrnCal && Month == RealMonth &&
                    Year       == Gbl.Now.Date.Year &&
                    Month      == Gbl.Now.Date.Month &&
                    DayOfMonth == Gbl.Now.Date.Day);

         /* Check if day has an exam announcement */
         ThisDayHasEvent = false;
	 if (!DrawingCalendar || Month == RealMonth)	// If drawing calendar and the month is not the real one, don't draw exam announcements
	    for (NumExamAnnouncement = 0;
		 NumExamAnnouncement < Gbl.LstExamAnnouncements.NumExamAnnounc;
		 NumExamAnnouncement++)
               if (Year       == Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Year &&
                   Month      == Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Month &&
                   DayOfMonth == Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Day)
                 {
		  ThisDayHasEvent = true;
		  if (PutLinkToEvents)
                    {
                     sprintf (StrExamOfX,Txt_Exam_of_X,Gbl.CurrentCrs.Crs.FullName);
   	             sprintf (Gbl.Title,"%s: %02u/%02u/%04u",
                              StrExamOfX,
                              Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Day,
                              Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Month,
                              Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Year);
                    }
                  break;
                 }

         /***** Write the box with the day *****/
	 fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:center;\">",
                  IsToday ? (ThisDayHasEvent ? "TODAY_EVENT" :
                	                       "TODAY") :
                            (ThisDayHasEvent ? "DAY_EVENT" :
                        	               "DAY"  ));

         /* If day has an exam announcement */
	 if (PutLinkToEvents && ThisDayHasEvent)
           {
            Act_FormStart (ActSeeExaAnn);
            fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                               "<tr>"
                               "<td class=\"%s\" style=\"text-align:center;\">",
                     ClassForDay);
            Act_LinkFormSubmit (Gbl.Title,ClassForDay);
           }
         else
           {
            fprintf (Gbl.F.Out,"<div class=\"%s\"",ClassForDay);
            if (!PrintView && TextForDay)
	       fprintf (Gbl.F.Out," title=\"%s\"",TextForDay);
	    fprintf (Gbl.F.Out,">");
           }

	 /* Write the day of month */
	 fprintf (Gbl.F.Out,"%u",DayOfMonth);

         /* If day has an exam announcement */
	 if (PutLinkToEvents && ThisDayHasEvent)
	   {
            fprintf (Gbl.F.Out,"</a>"
        	               "</td>"
        	               "</tr>"
        	               "</table>");
	    Act_FormEnd ();
	   }
         else
            fprintf (Gbl.F.Out,"</div>");

	 fprintf (Gbl.F.Out,"</td>");

         /***** Set the next day *****/
	 NumDaysInMonth = (Month == 2) ? Dat_GetNumDaysFebruary (Year) :
	                                 Dat_NumDaysMonth[Month];
	 if (++DayOfMonth > NumDaysInMonth)
	   {
	    if (++Month > 12)
	      {
	       Year++;
	       Month = 1;
	      }
	    DayOfMonth = 1;
	   }
	}
      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** End of month *****/
   fprintf (Gbl.F.Out,"</table>"
		      "</div>");
  }
