// swad_date.c: dates

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

#include "swad_config.h"
#include "swad_date.h"
#include "swad_global.h"
#include "swad_parameter.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

const unsigned Dat_NumDaysMonth[1+12] =
  {
    0,
   31,	//  1: January
   28,	//  2: February
   31,	//  3: Mars
   30,	//  4: April
   31,	//  5: May
   30,	//  6: June
   31,	//  7: July
   31,	//  8: Agoust
   30,	//  9: September
   31,	// 10: October
   30,	// 11: November
   31,	// 12: December
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Get current time ******************************/
/*****************************************************************************/

void Dat_GetTimeStartExecution (void)
  {
   Gbl.TimeStartExecution = time (NULL);
  }

/*****************************************************************************/
/********************** Get and convert current time *************************/
/*****************************************************************************/

void Dat_GetAndConvertCurrentDateTime (void)
  {
   /***** Convert current local time to a struct tblock *****/
   Dat_GetLocalTimeFromClock (&Gbl.TimeStartExecution);

   Gbl.Now.Date.Year   = Gbl.tblock->tm_year + 1900;
   Gbl.Now.Date.Month  = Gbl.tblock->tm_mon + 1;
   Gbl.Now.Date.Day    = Gbl.tblock->tm_mday;
   Gbl.Now.Time.Hour   = Gbl.tblock->tm_hour;
   Gbl.Now.Time.Minute = Gbl.tblock->tm_min;
   Gbl.Now.Time.Second = Gbl.tblock->tm_sec;

   /***** Initialize current date in format YYYYMMDD *****/
   sprintf (Gbl.Now.Date.YYYYMMDD,"%04u%02u%02u",
            Gbl.Now.Date.Year,Gbl.Now.Date.Month,Gbl.Now.Date.Day);

   /***** Initialize current time in format YYYYMMDDHHMMSS *****/
   sprintf (Gbl.Now.YYYYMMDDHHMMSS,"%04u%02u%02u%02u%02u%02u",
            Gbl.Now.Date.Year,Gbl.Now.Date.Month,Gbl.Now.Date.Day,
            Gbl.Now.Time.Hour,Gbl.Now.Time.Minute,Gbl.Now.Time.Second);

   /***** Compute what day was yesterday *****/
   Dat_GetDateBefore (&Gbl.Now.Date,&Gbl.Yesterday);
  }

/*****************************************************************************/
/************ Get UNIX time (seconds since 1970 UTC) from string *************/
/*****************************************************************************/

time_t Dat_GetUNIXTimeFromStr (const char *Str)
  {
   time_t Time;

   if (sscanf (Str,"%ld",&Time) != 1)
      Time = (time_t) 0;

   return Time;
  }

/*****************************************************************************/
/*********** Get a struct Date from a string in YYYYMMDD format **************/
/*****************************************************************************/

bool Dat_GetDateFromYYYYMMDD (struct Date *Date,const char *YYYYMMDD)
  {
   if (sscanf (YYYYMMDD,"%04u%02u%02u",&(Date->Year),&(Date->Month),&(Date->Day)) == 3)
     {
      strncpy (Date->YYYYMMDD,YYYYMMDD,4+2+2);
      Date->YYYYMMDD[4+2+2] = '\0';
      return true;
     }
   else
     {
      Date->Year = Date->Month = Date->Day = 0;
      Date->YYYYMMDD[0] = '\0';
      return false;
     }
  }

/*****************************************************************************/
/******* Get a struct DateTime from a string in YYYYMMDDHHMMSS format ********/
/*****************************************************************************/

bool Dat_GetDateTimeFromYYYYMMDDHHMMSS (struct DateTime *DateTime,const char *YYYYMMDDHHMMSS)
  {
   if (sscanf (YYYYMMDDHHMMSS,"%04u%02u%02u%02u%02u%02u",
               &(DateTime->Date.Year),&(DateTime->Date.Month),&(DateTime->Date.Day),
               &(DateTime->Time.Hour),&(DateTime->Time.Minute),&(DateTime->Time.Second)) == 6)
     {
      strncpy (DateTime->Date.YYYYMMDD,YYYYMMDDHHMMSS,4+2+2);
      DateTime->YYYYMMDDHHMMSS[4+2+2] = '\0';

      strncpy (DateTime->YYYYMMDDHHMMSS,YYYYMMDDHHMMSS,4+2+2+2+2+2);
      DateTime->YYYYMMDDHHMMSS[4+2+2+2+2+2] = '\0';

      return true;
     }
   else
     {
      DateTime->Date.Year = DateTime->Date.Month = DateTime->Date.Day = 0;
      DateTime->Date.YYYYMMDD[0] = '\0';

      DateTime->Time.Hour = DateTime->Time.Minute = DateTime->Time.Second = 0;
      DateTime->YYYYMMDDHHMMSS[0] = '\0';

      return false;
     }
  }

/*****************************************************************************/
/******************** Write div for client local time ************************/
/*****************************************************************************/

void Dat_ShowClientLocalTime (void)
  {
   extern const char *The_ClassCurrentTime[The_NUM_THEMES];

   fprintf (Gbl.F.Out,"<div id=\"hm\" class=\"%s\""
	              " style=\"padding-top:10px;\">"
                      "</div>",
            The_ClassCurrentTime[Gbl.Prefs.Theme]);
  }

/*****************************************************************************/
/***************** Compute local time, adjusting day of week *****************/
/*****************************************************************************/

void Dat_GetLocalTimeFromClock (const time_t *clock)
  {
   if ((Gbl.tblock = localtime (clock)) != NULL)
     {
      // Convert from sunday, monday, tuesday... to monday, tuesday, wednesday...
      if (Gbl.tblock->tm_wday == 0)	// If sunday
	 Gbl.tblock->tm_wday = 6;
      else				// If no sunday
	 Gbl.tblock->tm_wday--;
     }
  }

/*****************************************************************************/
/********* Convert a struct with Day, Month and Year to a date string ********/
/*****************************************************************************/

void Dat_ConvDateToDateStr (struct Date *Date,char *DateStr)
  {
   if (Date->Day   == 0 ||
       Date->Month == 0 ||
       Date->Year  == 0)
      DateStr[0] = '\0';
   else
      sprintf (DateStr,"%02u/%02u/%04u",Date->Day,Date->Month,Date->Year);
  }

/*****************************************************************************/
/*************** Show forms to enter initial and ending dates ****************/
/*****************************************************************************/

void Dat_WriteFormIniEndDates (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Start_date;
   extern const char *Txt_End_date;
   extern const char *Txt_Yesterday;
   extern const char *Txt_Today;

   /***** Start date *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s:"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Start_date);
   Dat_WriteFormDate (Cfg_LOG_START_YEAR,Gbl.Now.Date.Year,"Start",
                      &(Gbl.DateRange.DateIni),
                      false,false);

   /***** "Yesterday" and "Today" buttons *****/
   fprintf (Gbl.F.Out,"</td>"
                      "<td rowspan=\"2\" class=\"LEFT_MIDDLE\">"
	              "<input type=\"button\" name=\"Yesterday\" value=\"%s\""
                      " onclick=\"setDateTo(this,%u,%u,%u)\" />"
	              "<input type=\"button\" name=\"Today\" value=\"%s\""
                      " onclick=\"setDateTo(this,%u,%u,%u)\" />"
                      "</td>"
                      "</tr>",
            Txt_Yesterday,
	    Gbl.Yesterday.Day,
	    Gbl.Yesterday.Month,
	    Gbl.Yesterday.Year - Cfg_LOG_START_YEAR + 1,
            Txt_Today,
            Gbl.Now.Date.Day,
            Gbl.Now.Date.Month,
            Gbl.Now.Date.Year - Cfg_LOG_START_YEAR + 1);

   /***** End date *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s:"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_End_date);
   Dat_WriteFormDate (Cfg_LOG_START_YEAR,Gbl.Now.Date.Year,"End",
                      &(Gbl.DateRange.DateEnd),
                      false,false);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
  }


/*****************************************************************************/
/************************* Show a form to enter a date ***********************/
/*****************************************************************************/

void Dat_WriteFormClientLocalDateTime (const char *Id,
                                       time_t TimeUTC,
                                       unsigned FirstYear,unsigned LastYear,
                                       bool SubmitFormOnChange,bool Disabled)
  {
   extern const char *Txt_MONTHS_SMALL[12];
   unsigned Day;
   unsigned Month;
   unsigned Year;
   unsigned Hour;
   unsigned Minute;
   unsigned Second;

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table>"
	              "<tr>");

   /***** Year *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<select id=\"%sYear\" name=\"%sYear\""
                      " onchange=\""
                      "adjustDateForm('%s');"
                      "setUTCFromLocalDateTimeForm('%s');",
	    Id,Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"0\">-</option>");
   for (Year = FirstYear;
	Year <= LastYear;
	Year++)
      fprintf (Gbl.F.Out,"<option value=\"%u\">%u</option>",
               Year,Year);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Month *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<select id=\"%sMonth\" name=\"%sMonth\""
                      " onchange=\""
                      "adjustDateForm('%s');"
                      "setUTCFromLocalDateTimeForm('%s');",
	    Id,Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"0\">-</option>");
   for (Month = 1;
	Month <= 12;
	Month++)
      fprintf (Gbl.F.Out,"<option value=\"%u\">%s</option>",
               Month,Txt_MONTHS_SMALL[Month-1]);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Day *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
	              "<select id=\"%sDay\" name=\"%sDay\""
	              " onchange=\"setUTCFromLocalDateTimeForm('%s');",
            Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,">"
	              "<option value=\"0\">-</option>");
   for (Day = 1;
	Day <= 31;
	Day++)
      fprintf (Gbl.F.Out,"<option value=\"%u\">%u</option>",
               Day,Day);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Hour *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">,&nbsp;"
                      "<select id=\"%sHour\" name=\"%sHour\""
                      " onchange=\"setUTCFromLocalDateTimeForm('%s');",
            Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,">");
   for (Hour = 0;
	Hour <= 23;
	Hour++)
      fprintf (Gbl.F.Out,"<option value=\"%u\">%02u h</option>",
               Hour,Hour);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Minute *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<select id=\"%sMinute\" name=\"%sMinute\""
                      " onchange=\"setUTCFromLocalDateTimeForm('%s');",
	    Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,">");
   for (Minute = 0;
	Minute <= 59;
	Minute++)
      fprintf (Gbl.F.Out,"<option value=\"%u\">%02u &#39;</option>",
               Minute,Minute);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** Second *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<select id=\"%sSecond\" name=\"%sSecond\""
                      " onchange=\"setUTCFromLocalDateTimeForm('%s');",
	    Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,">");
   for (Second = 0;
	Second <= 59;
	Second++)
      fprintf (Gbl.F.Out,"<option value=\"%u\">%02u &quot;</option>",
               Second,Second);
   fprintf (Gbl.F.Out,"</select>"
	              "</td>");

   /***** End table *****/
   fprintf (Gbl.F.Out,"</tr>"
	              "</table>");

   /***** Hidden field with UTC time (seconds since 1970) used to send time *****/
   fprintf (Gbl.F.Out,"<input type=\"hidden\" id=\"%sTimeUTC\" name=\"%sTimeUTC\" value=\"%ld\" />",
            Id,Id,(long) TimeUTC);

   /***** Script to set selectors to local date and time from UTC time *****/
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
		      "setLocalDateTimeFormFromUTC('%s',%ld);"
		      "adjustDateForm('%s');"
		      "</script>",
	    Id,(long) TimeUTC,Id);
  }

/*****************************************************************************/
/***************** Get an hour-minute time from a form ***********************/
/*****************************************************************************/

time_t Dat_GetTimeUTCFromForm (const char *ParamName)
  {
   char LongStr[1+10+1];

   /**** Get time ****/
   Par_GetParToText (ParamName,LongStr,1+10);
   return Dat_GetUNIXTimeFromStr (LongStr);
  }

/*****************************************************************************/
/************************* Show a form to enter a date ***********************/
/*****************************************************************************/
/*
See the following pages:
http://javascript.internet.com/forms/date-selection-form.html
http://www.java-scripts.net/javascripts/Calendar-Popup.phtml
http://webmonkey.wired.com/webmonkey/98/04/index3a_page10.html?tw=programming
http://www.jsmadeeasy.com/javascripts/Calendars/Select-A-Mo://tech.irt.org/articles/js068/calendar.htm
http://javascript.internet.com/calendars/select-a-month.html
http://javascript.internet.com/forms/country.html
http://javascript.internet.com/forms/category-selection.html
See also http://www.ashleyit.com/rs/jsrs/select/php/select.php
*/
void Dat_WriteFormDate (unsigned FirstYear,unsigned LastYear,
	                const char *Id,
		        struct Date *DateSelected,
                        bool SubmitFormOnChange,bool Disabled)
  {
   extern const char *Txt_MONTHS_SMALL[12];
   unsigned Day;
   unsigned Month;
   unsigned Year;
   unsigned NumDaysSelectedMonth;

   /***** Day *****/
   fprintf (Gbl.F.Out,"<table>"
	              "<tr>"
	              "<td class=\"CENTER_MIDDLE\">"
	              "<select id=\"%sDay\" name=\"%sDay\"",
            Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.FormId);
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"0\">-</option>");
   NumDaysSelectedMonth = (DateSelected->Month == 0) ? 31 :
	                                               ((DateSelected->Month == 2) ? Dat_GetNumDaysFebruary (DateSelected->Year) :
	                                        	                             Dat_NumDaysMonth[DateSelected->Month]);
   for (Day = 1;
	Day <= NumDaysSelectedMonth;
	Day++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",Day);
      if (Day == DateSelected->Day)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",Day);
     }

   /***** Month *****/
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "<td class=\"CENTER_MIDDLE\">"
                      "<select id=\"%sMonth\" name=\"%sMonth\""
                      " onchange=\"adjustDateForm('%s');",
	    Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"0\">-</option>");
   for (Month = 1;
	Month <= 12;
	Month++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",Month);
      if (Month == DateSelected->Month)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_MONTHS_SMALL[Month-1]);
     }

   /***** Year *****/
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "<td class=\"CENTER_MIDDLE\">"
                      "<select id=\"%sYear\" name=\"%sYear\""
                      " onchange=\"adjustDateForm('%s');",
	    Id,Id,Id);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out,"document.getElementById('%s').submit();",
               Gbl.FormId);
   fprintf (Gbl.F.Out,"\"");
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,"><option value=\"0\">-</option>");
   for (Year = FirstYear;
	Year <= LastYear;
	Year++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",Year);
      if (Year == DateSelected->Year)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%u</option>",Year);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "</tr>"
	              "</table>");
  }

/*****************************************************************************/
/*************** Show a form to enter a hour-minute time *********************/
/*****************************************************************************/

void Dat_WriteFormHourMinute (const char *NameSelectHour,const char *NameSelectMinute,
		              struct Time *TimeSelected,
                              bool SubmitFormOnChange,bool Disabled)
  {
   unsigned Hour;
   unsigned Minute;

   /***** Hour *****/
   fprintf (Gbl.F.Out,"<table>"
                      "<tr>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<select name=\"%s\"",
            NameSelectHour);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.FormId);
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,">");
   for (Hour = 0;
	Hour <= 23;
	Hour++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",Hour);
      if (Hour == TimeSelected->Hour)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%02u h</option>",Hour);
     }

   /***** Minute *****/
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<select name=\"%s\"",
	    NameSelectMinute);
   if (SubmitFormOnChange)
      fprintf (Gbl.F.Out," onchange=\"document.getElementById('%s').submit();\"",
               Gbl.FormId);
   if (Disabled)
      fprintf (Gbl.F.Out," disabled=\"disabled\"");
   fprintf (Gbl.F.Out,">");
   for (Minute = 0;
	Minute <= 59;
	Minute++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",Minute);
      if (Minute == TimeSelected->Minute)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%02u &#39;</option>",Minute);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
	              "</tr>"
	              "</table>");
  }

/*****************************************************************************/
/*************************** Get a date from a form **************************/
/*****************************************************************************/

void Dat_GetDateFromForm (const char *ParamNameDay,const char *ParamNameMonth,const char *ParamNameYear,
                          unsigned *Day,unsigned *Month,unsigned *Year)
  {
   char UnsignedStr[10+1];

   /**** Get day ****/
   Par_GetParToText (ParamNameDay,UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",Day) != 1)
      *Day = 0;

   /**** Get month ****/
   Par_GetParToText (ParamNameMonth,UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",Month) != 1)
      *Month = 0;

   /**** Get year ****/
   Par_GetParToText (ParamNameYear,UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",Year) != 1)
      *Year = 0;
  }

/*****************************************************************************/
/***************** Get an hour-minute time from a form ***********************/
/*****************************************************************************/

void Dat_GetHourMinuteFromForm (const char *ParamNameHour,const char *ParamNameMinute,
                                unsigned *Hour,unsigned *Minute)
  {
   char UnsignedStr[10+1];

   /**** Get hour ****/
   Par_GetParToText (ParamNameHour,UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",Hour) != 1)
      *Hour = 0;

   /**** Get minute ****/
   Par_GetParToText (ParamNameMinute,UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",Minute) != 1)
      *Minute = 0;
  }

/*****************************************************************************/
/************************** Get initial and end dates ************************/
/*****************************************************************************/

void Dat_GetIniEndDatesFromForm (void)
  {
   /***** Get initial date *****/
   Dat_GetDateFromForm ("StartDay","StartMonth","StartYear",
                        &Gbl.DateRange.DateIni.Day,
                        &Gbl.DateRange.DateIni.Month,
                        &Gbl.DateRange.DateIni.Year);
   if (Gbl.DateRange.DateIni.Day   == 0 ||
       Gbl.DateRange.DateIni.Month == 0 ||
       Gbl.DateRange.DateIni.Year  == 0)
     {
      Gbl.DateRange.DateIni.Day   = Cfg_LOG_START_DAY;
      Gbl.DateRange.DateIni.Month = Cfg_LOG_START_MONTH;
      Gbl.DateRange.DateIni.Year  = Cfg_LOG_START_YEAR;
     }

   /***** Get end date *****/
   Dat_GetDateFromForm ("EndDay","EndMonth","EndYear",
                        &Gbl.DateRange.DateEnd.Day,
                        &Gbl.DateRange.DateEnd.Month,
                        &Gbl.DateRange.DateEnd.Year);
   if (Gbl.DateRange.DateEnd.Day   == 0 ||
       Gbl.DateRange.DateEnd.Month == 0 ||
       Gbl.DateRange.DateEnd.Year  == 0)
     {
      Gbl.DateRange.DateEnd.Day   = Gbl.Now.Date.Day;
      Gbl.DateRange.DateEnd.Month = Gbl.Now.Date.Month;
      Gbl.DateRange.DateEnd.Year  = Gbl.Now.Date.Year;
     }
  }

/*****************************************************************************/
/******************************** Write a date *******************************/
/*****************************************************************************/
// DateTime must be in YYYYMMDD[...] format

void Dat_WriteDate (const char *YYYYMMDD)
  {
   extern const char *Txt_Today;

   if (strncmp (YYYYMMDD,Gbl.Now.Date.YYYYMMDD,8))	// If date is not today
      fprintf (Gbl.F.Out,"%c%c/%c%c/%c%c",
               YYYYMMDD[6],YYYYMMDD[7],
               YYYYMMDD[4],YYYYMMDD[5],
               YYYYMMDD[2],YYYYMMDD[3]);
   else						// If date is today
      fprintf (Gbl.F.Out,"%s",Txt_Today);
  }

/*****************************************************************************/
/************************* Write hour and minute *****************************/
/*****************************************************************************/
// DateTime must be in HHMM[SS] format

void Dat_WriteHourMinute (const char *HHMM)
  {
   fprintf (Gbl.F.Out,"%c%c:%c%c",
            HHMM[0],HHMM[1],
            HHMM[2],HHMM[3]);
  }

/*****************************************************************************/
/****************** Write a UTC date into RFC 822 format *********************/
/*****************************************************************************/
// tm must hold a UTC date

void Dat_WriteRFC822DateFromTM (FILE *File,struct tm *tm)
  {
   const char *StrDayOfWeek[7] =
     {
      "Sun",
      "Mon",
      "Tue",
      "Wed",
      "Thu",
      "Fri",
      "Sat",
     };
   const char *StrMonth[12] =
     {
      "Jan",
      "Feb",
      "Mar",
      "Apr",
      "May",
      "Jun",
      "Jul",
      "Aug",
      "Sep",
      "Oct",
      "Nov",
      "Dec",
      };

   fprintf (File,"%s, %d %s %d %02d:%02d:%02d UT",
            StrDayOfWeek[tm->tm_wday],
            tm->tm_mday,
            StrMonth[tm->tm_mon],
            tm->tm_year + 1900,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec);
  }

/*****************************************************************************/
/************** Compute the preceding date of a given date *******************/
/*****************************************************************************/

void Dat_GetDateBefore (struct Date *Date,struct Date *PrecedingDate)
  {
   if (Date->Day == 1)
     {
      if (Date->Month == 1)
        {
         PrecedingDate->Year  = Date->Year - 1;
         PrecedingDate->Month = 12;
         PrecedingDate->Day   = 31;
        }
      else
        {
         PrecedingDate->Year  = Date->Year;
         PrecedingDate->Month = Date->Month - 1;
         PrecedingDate->Day   = (PrecedingDate->Month == 2) ? Dat_GetNumDaysFebruary (PrecedingDate->Year) :
                                                              Dat_NumDaysMonth[PrecedingDate->Month];
        }
     }
   else
     {
      PrecedingDate->Year  = Date->Year;
      PrecedingDate->Month = Date->Month;
      PrecedingDate->Day   = Date->Day - 1;
     }
  }

/*****************************************************************************/
/************** Compute the preceding date of a given date *******************/
/*****************************************************************************/

void Dat_GetDateAfter (struct Date *Date,struct Date *SubsequentDate)
  {
   unsigned NumDaysInMonth = (Date->Month == 2) ? Dat_GetNumDaysFebruary (Date->Year) :
	                                          Dat_NumDaysMonth[Date->Month];

   if (Date->Day == NumDaysInMonth)
     {
      if (Date->Month == 12)
        {
         SubsequentDate->Year  = Date->Year + 1;
         SubsequentDate->Month = 1;
         SubsequentDate->Day   = 1;
        }
      else
        {
         SubsequentDate->Year  = Date->Year;
         SubsequentDate->Month = Date->Month + 1;
         SubsequentDate->Day   = 1;
        }
     }
   else
     {
      SubsequentDate->Year  = Date->Year;
      SubsequentDate->Month = Date->Month;
      SubsequentDate->Day   = Date->Day + 1;
     }
  }

/*****************************************************************************/
/**************** Compute the week before to a given week ********************/
/*****************************************************************************/

void Dat_GetWeekBefore (struct Date *Date,struct Date *PrecedingDate)
  {
   if (Date->Week == 1)
     {
      PrecedingDate->Year = Date->Year - 1;
      PrecedingDate->Week = Dat_GetNumWeeksInYear (PrecedingDate->Year);
     }
   else
     {
      PrecedingDate->Year = Date->Year;
      PrecedingDate->Week = Date->Week - 1;
     }
  }

/*****************************************************************************/
/*************** Compute the month before to a given month *******************/
/*****************************************************************************/

void Dat_GetMonthBefore (struct Date *Date,struct Date *PrecedingDate)
  {
   if (Date->Month == 1)
     {
      PrecedingDate->Year  = Date->Year - 1;
      PrecedingDate->Month = 12;
     }
   else
     {
      PrecedingDate->Year  = Date->Year;
      PrecedingDate->Month = Date->Month - 1;
     }
  }

/*****************************************************************************/
/************** Compute the number of days beteen two dates ******************/
/*****************************************************************************/
// If the dates are the same, return 1
// If the old date is the day before the new data, return 2
// ...

unsigned Dat_GetNumDaysBetweenDates (struct Date *DateIni,struct Date *DateEnd)
  {
   int DiffDays;
   unsigned Year;

   /***** If initial year is less than end year, return 0
          (actually the difference in days should be negative) *****/
   if (DateIni->Year > DateEnd->Year)
      return 0;

   /***** Initial year is less or equal than end year ==> compute difference in days *****/
   DiffDays = (int) Dat_GetDayOfYear (DateEnd) - (int) Dat_GetDayOfYear (DateIni) + 1;
   for (Year = DateIni->Year;
	Year < DateEnd->Year;
	Year++)
      DiffDays += (int) Dat_GetNumDaysInYear (Year);
   return (DiffDays >= 0) ? (unsigned) DiffDays :
	                    0;
  }

/*****************************************************************************/
/*************** Compute the number of weeks between two dates ***************/
/*****************************************************************************/
// If the two dates are in the same week, return 1

unsigned Dat_GetNumWeeksBetweenDates (struct Date *DateIni,struct Date *DateEnd)
  {
   int DiffWeeks;
   unsigned Year;

   /***** If initial year is lower than the ending year, return 0
          (actually the difference should be negative) *****/
   if (DateIni->Year > DateEnd->Year)
      return 0;

   /***** Initial year is lower or equal to ending year ==> compute difference in weeks *****/
   DiffWeeks = (int) DateEnd->Week - (int) DateIni->Week + 1;
   for (Year = DateIni->Year;
	Year < DateEnd->Year;
	Year++)
      DiffWeeks += (int) Dat_GetNumWeeksInYear (Year);
   return (DiffWeeks >= 0) ? (unsigned) DiffWeeks :
	                     0;
  }

/*****************************************************************************/
/************* Compute the number of months between two dates ****************/
/*****************************************************************************/
// If the two dates are in the same month, return 1

unsigned Dat_GetNumMonthsBetweenDates (struct Date *DateIni,struct Date *DateEnd)
  {
   int DiffMonths;

   /***** Compute the difference in months *****/
   DiffMonths = ((int) DateEnd->Year - (int) DateIni->Year) * 12 +
	         (int) DateEnd->Month - (int) DateIni->Month + 1;
   return (DiffMonths >= 0) ? (unsigned) DiffMonths :
	                      0;
  }

/*****************************************************************************/
/*************** Compute the number of days in a given year ******************/
/*****************************************************************************/

#define NUM_DAYS_YEAR_EXCEPT_FEBRUARY (365 - 28)

unsigned Dat_GetNumDaysInYear (unsigned Year)
  {
   return NUM_DAYS_YEAR_EXCEPT_FEBRUARY + Dat_GetNumDaysFebruary (Year);
  }

/*****************************************************************************/
/****************** Return the number of days of february ********************/
/*****************************************************************************/

unsigned Dat_GetNumDaysFebruary (unsigned Year)
  {
   return (Dat_GetIfLeapYear (Year) ? 29 :
	                              28);
  }

/*****************************************************************************/
/************************* Return true if year is leap ***********************/
/*****************************************************************************/

bool Dat_GetIfLeapYear (unsigned Year)
  {
   return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
  }

/*****************************************************************************/
/******* Compute the number of weeks starting in monday of a given year ******/
/*****************************************************************************/
// A week starting on monday is considered belonging to a year only if it has most of its days (4 to 7) belonging to that year
// A year may have 52 (usually) or 53 of those weeks

unsigned Dat_GetNumWeeksInYear (unsigned Year)
  {
   unsigned December31DayOfWeek = Dat_GetDayOfWeek (Year,12,31); // From 0 to 6

   if (December31DayOfWeek == 3)
      return 53;
   if (December31DayOfWeek == 4)
      if (Dat_GetIfLeapYear (Year))
         return 53;
   return 52;
  }

/*****************************************************************************/
/***************** Compute day of the week from a given date *****************/
/*****************************************************************************/
// Return 0 for monday, 1 for tuesday,... 6 for sunday

unsigned Dat_GetDayOfWeek (unsigned Year,unsigned Month,unsigned Day)
  {
   if (Month <= 2)
     {
      Month += 12;
      Year--;
     }
   return (((Day+(Month*2)+(((Month+1)*3)/5)+Year+(Year/4-Year/100+Year/400)+2) % 7) + 5) % 7;
  }

/*****************************************************************************/
/***************** Compute the day of year (from 1 to 366) *******************/
/*****************************************************************************/

unsigned Dat_GetDayOfYear (struct Date *Date)
  {
   unsigned Month;
   unsigned DayYear;

   /* Compute day of year */
   for (Month = 1, DayYear = 0;
	Month < Date->Month;
	Month++)
      DayYear += (Month == 2) ? Dat_GetNumDaysFebruary (Date->Year) :
	                        Dat_NumDaysMonth[Month];
   DayYear += Date->Day;

   return DayYear;
  }

/*****************************************************************************/
/******** Compute Week (and possibly updated Year) from a given date *********/
/*****************************************************************************/
/* The weeks always are counted from monday to sunday.
   01/01/2006 was sunday => it is counted in the week 52 of 2005, which goes from the 26/12/2005 (monday) until the 01/01/2006 (sunday).
   Week 1 of 2006 goes from the 02/01/2006 (monday) until the 08/01/2006 (sunday) */

void Dat_CalculateWeekOfYear (struct Date *Date)
  {
   /*
   If January 1 of Year is...   | ...then first week of Year starts the day  |
   -----------------------------+--------------------------------------------+
   (0) Monday                   | January,   1 of Year                       |
   (1) Tuesday                  | December, 31 of Year-1                     |
   (2) Wednesday                | December, 30 of Year-1                     |
   (3) Thursday                 | December, 29 of Year-1                     |
   (4) Friday                   | January,   4 of Year                       |
   (5) Saturday                 | January,   5 of Year                       |
   (6) Sunday                   | January,   6 of Year                       |
   If December 31 of Year is... | ...then last week of Year ends the day     |
   -----------------------------+--------------------------------------------+
   (0) Monday                   | December, 30 of Year                       |
   (1) Tuesday                  | December, 29 of Year                       |
   (2) Wednesday                | December, 28 of Year                       |
   (3) Thursday                 | January,   3 of Year+1                     |
   (4) Friday                   | January,   4 of Year+1                     |
   (5) Saturday                 | January,   5 of Year+1                     |
   (6) Sunday                   | December, 31 of Year                       |
   */
   int DayThatFirstWeekStarts[7] = {1,0,-1,-2,4,3,2};
   unsigned DayThatLastWeekEnds[7] = {30,29,28,34,33,32,31};
   unsigned January1DayOfWeek = Dat_GetDayOfWeek (Date->Year,1,1); // From 0 to 6
   unsigned December31DayOfWeek = Dat_GetDayOfWeek (Date->Year,12,31); // From 0 to 6

   if (Date->Month == 1 && (int) Date->Day < DayThatFirstWeekStarts[January1DayOfWeek])	// Week is the last week of the year before this
     {
      Date->Year--;
      Date->Week = Dat_GetNumWeeksInYear (Date->Year);
     }
   else if (Date->Month == 12 && Date->Day > DayThatLastWeekEnds[December31DayOfWeek])	// Week is the first week of the year after this
     {
      Date->Year++;
      Date->Week = 1;
     }
   else	// Week corresponds to this year
      Date->Week = ((int) Dat_GetDayOfYear (Date) - DayThatFirstWeekStarts[January1DayOfWeek]) / 7 + 1;
  }

/*****************************************************************************/
/******************* Assign the values of a date to another ******************/
/*****************************************************************************/

void Dat_AssignDate (struct Date *DateDst,struct Date *DateSrc)
  {
   DateDst->Year  = DateSrc->Year;
   DateDst->Month = DateSrc->Month;
   DateDst->Day   = DateSrc->Day;
   DateDst->Week  = DateSrc->Week;
  }

/*****************************************************************************/
/****************************** Compare dates ********************************/
/*****************************************************************************/
// Return <0 if Date1<Date2
// Return >0 if Date1>Date2
// Return  0 if Date1==Date2

int Dat_CompareDates (struct Date *Date1,struct Date *Date2)
  {
   if (Date1->Year  < Date2->Year ) return -1;
   if (Date1->Year  > Date2->Year ) return 1;

   if (Date1->Month < Date2->Month) return -1;
   if (Date1->Month > Date2->Month) return 1;

   if (Date1->Day   < Date2->Day  ) return -1;
   if (Date1->Day   > Date2->Day  ) return 1;

   return 0;
  }

/*****************************************************************************/
/************************** Compare datetimes ********************************/
/*****************************************************************************/
// Return <0 if DateTime1<DateTime2
// Return >0 if DateTime1>DateTime2
// Return  0 if DateTime1==DateTime2

int Dat_CompareDateTimes (struct DateTime *DateTime1,struct DateTime *DateTime2)
  {
   if (DateTime1->Date.Year   < DateTime2->Date.Year  ) return -1;
   if (DateTime1->Date.Year   > DateTime2->Date.Year  ) return 1;

   if (DateTime1->Date.Month  < DateTime2->Date.Month ) return -1;
   if (DateTime1->Date.Month  > DateTime2->Date.Month ) return 1;

   if (DateTime1->Date.Day    < DateTime2->Date.Day   ) return -1;
   if (DateTime1->Date.Day    > DateTime2->Date.Day   ) return 1;

   if (DateTime1->Time.Hour   < DateTime2->Time.Hour  ) return -1;
   if (DateTime1->Time.Hour   > DateTime2->Time.Hour  ) return 1;

   if (DateTime1->Time.Minute < DateTime2->Time.Minute) return -1;
   if (DateTime1->Time.Minute > DateTime2->Time.Minute) return 1;

   if (DateTime1->Time.Second < DateTime2->Time.Second) return -1;
   if (DateTime1->Time.Second > DateTime2->Time.Second) return 1;

   return 0;
  }
