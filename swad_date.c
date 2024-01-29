// swad_date.c: dates

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE		// For vasprintf
#include <stdio.h>		// For vasprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions
#include <time.h>		// For time functions (mktime...)

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char *Dat_TimeStatusClass[Dat_NUM_TIME_STATUS][HidVis_NUM_HIDDEN_VISIBLE] =
  {
   [Dat_PAST   ][HidVis_HIDDEN ] = "DATE_RED_LIGHT",
   [Dat_PAST   ][HidVis_VISIBLE] = "DATE_RED",

   [Dat_PRESENT][HidVis_HIDDEN ] = "DATE_GREEN_LIGHT",
   [Dat_PRESENT][HidVis_VISIBLE] = "DATE_GREEN",

   [Dat_FUTURE ][HidVis_HIDDEN ] = "DATE_BLUE_LIGHT",
   [Dat_FUTURE ][HidVis_VISIBLE] = "DATE_BLUE",
  };

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

static const unsigned Dat_NumDaysMonth[1 + 12] =
  {
   [ 0] =  0,
   [ 1] = 31,	// January
   [ 2] = 28,	// February
   [ 3] = 31,	// March
   [ 4] = 30,	// April
   [ 5] = 31,	// May
   [ 6] = 30,	// June
   [ 7] = 31,	// July
   [ 8] = 31,	// Agoust
   [ 9] = 30,	// September
   [10] = 31,	// October
   [11] = 30,	// November
   [12] = 31,	// December
  };

static struct
  {
   struct timeval tvStart;
   struct timeval tvPageCreated;
   time_t StartExecutionTimeUTC;
   long TimeGenerationInMicroseconds;
   long TimeSendInMicroseconds;
   struct Dat_DateTime Now;
   struct
     {
      struct Dat_DateTime DateTime[Dat_NUM_START_END_TIME];	// TODO: Remove in future versions?
      time_t TimeUTC[Dat_NUM_START_END_TIME];
     } Range;
  } Dat_Time =
  {
   .TimeGenerationInMicroseconds = 0L,
   .TimeSendInMicroseconds       = 0L,
  };

static const char *Dat_ParName[Dat_NUM_START_END_TIME] =
  {
   [Dat_STR_TIME] = "Start",
   [Dat_END_TIME] = "End",
  };

static const char *Dat_ParTimeUTCName[Dat_NUM_START_END_TIME] =
  {
   [Dat_STR_TIME] = "StartTimeUTC",
   [Dat_END_TIME] = "EndTimeUTC",
  };

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Dat_PutIconsDateFormat (__attribute__((unused)) void *Args);
static unsigned Dat_GetParDateFormat (void);

/*****************************************************************************/
/*********************** Set/get start execution time ************************/
/*****************************************************************************/

void Dat_SetStartExecutionTimeval (void)
  {
   struct timezone tz;

   gettimeofday (&Dat_Time.tvStart, &tz);
  }

void Dat_SetStartExecutionTimeUTC (void)
  {
   Dat_Time.StartExecutionTimeUTC = time (NULL);
  }

time_t Dat_GetStartExecutionTimeUTC (void)
  {
   return Dat_Time.StartExecutionTimeUTC;
  }

struct Dat_Date *Dat_GetCurrentDate (void)
  {
   return &Dat_Time.Now.Date;
  }

unsigned Dat_GetCurrentDay (void)
  {
   return Dat_Time.Now.Date.Day;
  }

unsigned Dat_GetCurrentMonth (void)
  {
   return Dat_Time.Now.Date.Month;
  }

unsigned Dat_GetCurrentYear (void)
  {
   return Dat_Time.Now.Date.Year;
  }

/*****************************************************************************/
/************** Get time to generate/send page in microseconds ***************/
/*****************************************************************************/

long Dat_GetTimeGenerationInMicroseconds (void)
  {
   return Dat_Time.TimeGenerationInMicroseconds;
  }

long Dat_GetTimeSendInMicroseconds (void)
  {
   return Dat_Time.TimeSendInMicroseconds;
  }

/*****************************************************************************/
/**************** Compute the time used to generate the page *****************/
/*****************************************************************************/

void Dat_ComputeTimeToGeneratePage (void)
  {
   struct timezone tz;

   if (gettimeofday (&Dat_Time.tvPageCreated, &tz))
      // Error in gettimeofday
      Dat_Time.TimeGenerationInMicroseconds = 0L;
   else
      Dat_Time.TimeGenerationInMicroseconds = (long) ((Dat_Time.tvPageCreated.tv_sec  - Dat_Time.tvStart.tv_sec) * 1000000L +
						       Dat_Time.tvPageCreated.tv_usec - Dat_Time.tvStart.tv_usec);
  }

/*****************************************************************************/
/****************** Compute the time used to send the page *******************/
/*****************************************************************************/

void Dat_ComputeTimeToSendPage (void)
  {
   struct timeval tvPageSent;
   struct timezone tz;

   if (gettimeofday (&tvPageSent, &tz))
      // Error in gettimeofday
      Dat_Time.TimeSendInMicroseconds = 0;
   else
     {
      if (tvPageSent.tv_usec < Dat_Time.tvPageCreated.tv_usec)
	{
	 tvPageSent.tv_sec--;
	 tvPageSent.tv_usec += 1000000;
	}
      Dat_Time.TimeSendInMicroseconds = (tvPageSent.tv_sec  - Dat_Time.tvPageCreated.tv_sec) * 1000000L +
                                         tvPageSent.tv_usec - Dat_Time.tvPageCreated.tv_usec;
     }
  }

/*****************************************************************************/
/************** Write the time to generate and send the page *****************/
/*****************************************************************************/

void Dat_WriteTimeToGenerateAndSendPage (void)
  {
   extern const char *Txt_PAGE1_Page_generated_in;
   extern const char *Txt_PAGE2_and_sent_in;
   char StrTimeGenerationInMicroseconds[Dat_MAX_BYTES_TIME + 1];
   char StrTimeSendInMicroseconds[Dat_MAX_BYTES_TIME + 1];

   Dat_WriteTime (StrTimeGenerationInMicroseconds,Dat_GetTimeGenerationInMicroseconds ());
   Dat_WriteTime (StrTimeSendInMicroseconds      ,Dat_GetTimeSendInMicroseconds ());
   HTM_TxtF ("%s %s %s %s",
             Txt_PAGE1_Page_generated_in,StrTimeGenerationInMicroseconds,
             Txt_PAGE2_and_sent_in,StrTimeSendInMicroseconds);
  }

/*****************************************************************************/
/********* Write time (given in microseconds) depending on amount ************/
/*****************************************************************************/

void Dat_WriteTime (char Str[Dat_MAX_BYTES_TIME],long TimeInMicroseconds)
  {
   if (TimeInMicroseconds < 1000L)
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%ld &micro;s",TimeInMicroseconds);
   else if (TimeInMicroseconds < 1000000L)
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%ld ms",TimeInMicroseconds / 1000);
   else if (TimeInMicroseconds < (60 * 1000000L))
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%.1f s",
                (double) TimeInMicroseconds / 1E6);
   else
      snprintf (Str,Dat_MAX_BYTES_TIME + 1,"%ld min, %ld s",
                TimeInMicroseconds / (60 * 1000000L),
                (TimeInMicroseconds / 1000000L) % 60);
  }

/*****************************************************************************/
/******************************** Reset date *********************************/
/*****************************************************************************/

void Dat_ResetDate (struct Dat_Date *Date)
  {
   Date->Day = 0;
   Date->Month = 0;
   Date->Year = 0;
   Date->Week = 0;
   Date->YYYYMMDD[0] = '\0';
  }

void Dat_ResetHour (struct Dat_Hour *Hour)
  {
   Hour->Hour = 0;
   Hour->Minute = 0;
  }

/*****************************************************************************/
/************** Put icons to select the first day of the week ****************/
/*****************************************************************************/

void Dat_PutBoxToSelectDateFormat (void)
  {
   extern const char *Hlp_PROFILE_Settings_dates;
   extern const char *Txt_Dates;
   Dat_Format_t Format;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Dates,
                 Dat_PutIconsDateFormat,NULL,
                 Hlp_PROFILE_Settings_dates,Box_NOT_CLOSABLE);

      /***** Form with list of options *****/
      Frm_BeginForm (ActChgDatFmt);

         /***** Begin list *****/
	 HTM_UL_Begin ("class=\"LIST_LEFT\"");

	    for (Format  = (Dat_Format_t) 0;
		 Format <= (Dat_Format_t) (Dat_NUM_OPTIONS_FORMAT - 1);
		 Format++)
	      {
	       if (Format == Gbl.Prefs.DateFormat)
		  HTM_LI_Begin ("class=\"DAT_STRONG_%s BG_HIGHLIGHT\"",
		                The_GetSuffix ());
	       else
		  HTM_LI_Begin ("class=\"DAT_%s\"",
		                The_GetSuffix ());
	       HTM_LABEL_Begin (NULL);
		  HTM_INPUT_RADIO ("DateFormat",HTM_SUBMIT_ON_CLICK,
				   " value=\"%u\"%s",
				   (unsigned) Format,
				   Format == Gbl.Prefs.DateFormat ? " checked=\"checked\"" :
								    "");
		  Dat_PutSpanDateFormat (Format);
		  Dat_PutScriptDateFormat (Format);
	       HTM_LABEL_End ();
	       HTM_LI_End ();
	      }

	 /***** End list *****/
	 HTM_UL_End ();

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Put contextual icons in date-format setting *****************/
/*****************************************************************************/

static void Dat_PutIconsDateFormat (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_DATE_FORMAT);
  }

/*****************************************************************************/
/******* Put script to write current date-time in a given date format ********/
/*****************************************************************************/

void Dat_PutSpanDateFormat (Dat_Format_t Format)
  {
   HTM_SPAN_Begin ("id=\"date_format_%u\"",(unsigned) Format);
   HTM_SPAN_End ();
  }

void Dat_PutScriptDateFormat (Dat_Format_t Format)
  {
   char *Id;

   if (asprintf (&Id,"date_format_%u",(unsigned) Format) < 0)
      Err_NotEnoughMemoryExit ();
   Dat_WriteLocalDateHMSFromUTC (Id,Dat_GetStartExecutionTimeUTC (),
				 Format,Dat_SEPARATOR_NONE,
				 false,true,false,0x0);
   free (Id);
  }

/*****************************************************************************/
/***************************** Change date format ****************************/
/*****************************************************************************/

void Dat_ChangeDateFormat (void)
  {
   /***** Get param with date format *****/
   Gbl.Prefs.DateFormat = Dat_GetParDateFormat ();

   /***** Store date format in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutDateFormat (Gbl.Prefs.DateFormat);

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/********************** Get parameter with date format ***********************/
/*****************************************************************************/

static Dat_Format_t Dat_GetParDateFormat (void)
  {
   return (Dat_Format_t) Par_GetParUnsignedLong ("DateFormat",
                                                 0L,
                                                 (unsigned long) (Dat_NUM_OPTIONS_FORMAT - 1),
                                                 (unsigned long) Dat_FORMAT_DEFAULT);
  }

/*****************************************************************************/
/*********************** Get date format from string *************************/
/*****************************************************************************/

Dat_Format_t Dat_GetDateFormatFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Dat_NUM_OPTIONS_FORMAT)
         return (Dat_Format_t) UnsignedNum;

   return Dat_FORMAT_DEFAULT;
  }

/*****************************************************************************/
/********************** Get and convert current time *************************/
/*****************************************************************************/

void Dat_GetAndConvertCurrentDateTime (void)
  {
   struct tm *tm_ptr;
   time_t t = Dat_GetStartExecutionTimeUTC ();

   /***** Convert current local time to a struct tblock *****/
   tm_ptr = Dat_GetLocalTimeFromClock (&t);

   Dat_Time.Now.Date.Year   = tm_ptr->tm_year + 1900;
   Dat_Time.Now.Date.Month  = tm_ptr->tm_mon  + 1;
   Dat_Time.Now.Date.Day    = tm_ptr->tm_mday;
   Dat_Time.Now.Time.Hour   = tm_ptr->tm_hour;
   Dat_Time.Now.Time.Minute = tm_ptr->tm_min;
   Dat_Time.Now.Time.Second = tm_ptr->tm_sec;

   /***** Initialize current date in format YYYYMMDD *****/
   snprintf (Dat_Time.Now.Date.YYYYMMDD,sizeof (Dat_Time.Now.Date.YYYYMMDD),
	     "%04u%02u%02u",
             Dat_Time.Now.Date.Year,Dat_Time.Now.Date.Month,Dat_Time.Now.Date.Day);

   /***** Initialize current time in format YYYYMMDDHHMMSS *****/
   snprintf (Dat_Time.Now.YYYYMMDDHHMMSS,sizeof (Dat_Time.Now.YYYYMMDDHHMMSS),
	     "%04u%02u%02u%02u%02u%02u",
             Dat_Time.Now.Date.Year,Dat_Time.Now.Date.Month ,Dat_Time.Now.Date.Day,
             Dat_Time.Now.Time.Hour,Dat_Time.Now.Time.Minute,Dat_Time.Now.Time.Second);
  }

/*****************************************************************************/
/************ Get UNIX time (seconds since 1970 UTC) from string *************/
/*****************************************************************************/

time_t Dat_GetUNIXTimeFromStr (const char *Str)
  {
   time_t Time = (time_t) 0;

   if (Str)
      if (Str[0])
         if (sscanf (Str,"%ld",&Time) != 1)
	    Time = (time_t) 0;

   return Time;
  }

/*****************************************************************************/
/*********** Get a struct Date from a string in YYYYMMDD format **************/
/*****************************************************************************/

bool Dat_GetDateFromYYYYMMDD (struct Dat_Date *Date,const char *YYYYMMDD)
  {
   if (YYYYMMDD)
      if (YYYYMMDD[0])
	 if (sscanf (YYYYMMDD,"%04u%02u%02u",&(Date->Year),&(Date->Month),&(Date->Day)) == 3)
	   {
	    Str_Copy (Date->YYYYMMDD,YYYYMMDD,sizeof (Date->YYYYMMDD) - 1);
	    return true;
	   }

   Date->Year = Date->Month = Date->Day = 0;
   Date->YYYYMMDD[0] = '\0';
   return false;
  }

/*****************************************************************************/
/******************** Write div for client local time ************************/
/*****************************************************************************/

void Dat_ShowClientLocalTime (void)
  {
   extern const char *Txt_Show_calendar;
   extern const char *Txt_Show_agenda;

   /***** Draw the current date and time *****/
   /* Begin container */
   HTM_DIV_Begin ("id=\"current_date\"");

      /* Month with link to calendar */
      HTM_DIV_Begin ("id=\"current_month\"");
	 Frm_BeginForm (ActSeeCal);
	    HTM_BUTTON_Submit_Begin (Txt_Show_calendar,
	                             "class=\"BT_LINK CURRENT_MONTH\"");
	       HTM_SPAN_Begin ("id=\"current_month_txt\"");
	       // JavaScript will write HTML here
	       HTM_SPAN_End ();
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_DIV_End ();

      /* Day with link to agenda (if I am logged) */
      HTM_DIV_Begin ("id=\"current_day\"");
	 if (Gbl.Usrs.Me.Logged)
	   {
	    Frm_BeginForm (ActSeeMyAgd);
	       HTM_BUTTON_Submit_Begin (Txt_Show_agenda,
	                                "class=\"BT_LINK CURRENT_DAY\"");
	   }
	 HTM_SPAN_Begin ("id=\"current_day_txt\"");
	 // JavaScript will write HTML here
	 HTM_SPAN_End ();
	 if (Gbl.Usrs.Me.Logged)
	   {
	       HTM_BUTTON_End ();
	    Frm_EndForm ();
	   }
      HTM_DIV_End ();

      /* Time */
      HTM_DIV_Begin ("id=\"current_time\"");	// JavaScript will write HTML here
      HTM_DIV_End ();

   /* End container */
   HTM_DIV_End ();

   /* Write script to draw the month */
   HTM_SCRIPT_Begin (NULL,NULL);
      HTM_TxtF ("secondsSince1970UTC = %ld;\n"
		"writeLocalClock();\n",
		(long) Dat_GetStartExecutionTimeUTC ());
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/***************** Compute local time, adjusting day of week *****************/
/*****************************************************************************/

struct tm *Dat_GetLocalTimeFromClock (const time_t *timep)
  {
   struct tm *tm_ptr;

   if ((tm_ptr = localtime (timep)) == NULL)
      Err_ShowErrorAndExit ("Can not get local time from clock.");

   /***** Convert from sunday, monday, tuesday...
	  to monday, tuesday, wednesday... *****/
   if (tm_ptr->tm_wday == 0)	// sunday
      tm_ptr->tm_wday = 6;
   else if (tm_ptr->tm_wday >= 1 &&
	    tm_ptr->tm_wday <= 6)	// monday to saturday
      tm_ptr->tm_wday--;
   else				// error!
      tm_ptr->tm_wday = 0;

   return tm_ptr;
  }

/*****************************************************************************/
/********* Convert a struct with Day, Month and Year to a date string ********/
/*****************************************************************************/

void Dat_ConvDateToDateStr (const struct Dat_Date *Date,char StrDate[Cns_MAX_BYTES_DATE + 1])
  {
   extern const char *Txt_MONTHS_SMALL_SHORT[12];

   if (Date->Day   == 0 ||
       Date->Month == 0 ||
       Date->Year  == 0)
      StrDate[0] = '\0';
   else
      switch (Gbl.Prefs.DateFormat)
        {
	 case Dat_FORMAT_YYYY_MM_DD:
	    snprintf (StrDate,Cns_MAX_BYTES_DATE + 1,"%04u-%02u-%02u",
		      Date->Year,
		      Date->Month,
		      Date->Day);
	    break;
	 case Dat_FORMAT_DD_MONTH_YYYY:
	    snprintf (StrDate,Cns_MAX_BYTES_DATE + 1,"%u&nbsp;%s&nbsp;%04u",
		      Date->Day,
		      Txt_MONTHS_SMALL_SHORT[Date->Month - 1],
		      Date->Year);
	    break;
	 case Dat_FORMAT_MONTH_DD_YYYY:
	    snprintf (StrDate,Cns_MAX_BYTES_DATE + 1,"%s&nbsp;%u,&nbsp;%04u",
		      Txt_MONTHS_SMALL_SHORT[Date->Month - 1],
		      Date->Day,
		      Date->Year);
	    break;
        }
  }

/*****************************************************************************/
/**************************** Get start/end date *****************************/
/*****************************************************************************/

time_t Dat_GetRangeTimeUTC (Dat_StartEndTime_t StartEndTime)
  {
   return Dat_Time.Range.TimeUTC[StartEndTime];
  }

struct Dat_Date *Dat_GetRangeDate (Dat_StartEndTime_t StartEndTime)
  {
   return &Dat_Time.Range.DateTime[StartEndTime].Date;
  }

/*****************************************************************************/
/*************** Show forms to enter initial and ending dates ****************/
/*****************************************************************************/

void Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME])
  {
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Yesterday;
   extern const char *Txt_Today;
   unsigned CurrentYear = Dat_GetCurrentYear ();

   /***** Start date-time *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("REC_C1_BOT RT","",Txt_START_END_TIME[Dat_STR_TIME]);

      /* Data (date-time) */
      HTM_TD_Begin ("class=\"REC_C2_BOT LT\"");
	 Dat_WriteFormClientLocalDateTimeFromTimeUTC ("Start",
						      Dat_STR_TIME,
						      Dat_Time.Range.TimeUTC[Dat_STR_TIME],
						      Cfg_LOG_START_YEAR,
						      CurrentYear,
						      Dat_FORM_SECONDS_ON,
						      SetHMS[Dat_STR_TIME],
						      HTM_DONT_SUBMIT_ON_CHANGE);

	 /* "Yesterday" and "Today" buttons */
	 HTM_DIV_Begin ("class=\"DATE_FORM_TODAY\"");
	    // HTM_SP ();
	    HTM_INPUT_BUTTON ("Yesterday",Txt_Yesterday,
			      "onclick=\"setDateToYesterday('Start','End');\"");
	    HTM_INPUT_BUTTON ("Today",Txt_Today,
			      "onclick=\"setDateToToday('Start','End');\"");
	 HTM_DIV_End ();
      HTM_TD_End ();

   HTM_TR_End ();

   /***** End date-time *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("REC_C1_BOT RT","",Txt_START_END_TIME[Dat_END_TIME]);

      /* Data (date-time) */
      HTM_TD_Begin ("class=\"REC_C2_BOT LT\"");
	 Dat_WriteFormClientLocalDateTimeFromTimeUTC ("End",
						      Dat_END_TIME,
						      Dat_Time.Range.TimeUTC[Dat_END_TIME],
						      Cfg_LOG_START_YEAR,
						      CurrentYear,
						      Dat_FORM_SECONDS_ON,
						      SetHMS[Dat_END_TIME],
						      HTM_DONT_SUBMIT_ON_CHANGE);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show forms to enter initial and ending date-times *************/
/*****************************************************************************/

void Dat_PutFormStartEndClientLocalDateTimes (const time_t TimeUTC[Dat_NUM_START_END_TIME],
                                              Dat_FormSeconds FormSeconds,
					      const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME])
  {
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   Dat_StartEndTime_t StartEndTime;
   const char *Id[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = "Start",
      [Dat_END_TIME] = "End",
     };

   for (StartEndTime  = Dat_STR_TIME;
	StartEndTime <= Dat_END_TIME;
	StartEndTime++)
     {
      /***** Date-time *****/
      HTM_TR_Begin (NULL);

	 /* Label */
	 Frm_LabelColumn ("RM","",Txt_START_END_TIME[StartEndTime]);

	 /* Data */
	 HTM_TD_Begin ("class=\"LM\"");
	    Dat_WriteFormClientLocalDateTimeFromTimeUTC (Id[StartEndTime],
							 StartEndTime,
							 TimeUTC[StartEndTime],
							 Cfg_LOG_START_YEAR,
							 Dat_GetCurrentYear () + 1,
							 FormSeconds,
							 SetHMS[StartEndTime],	// Set hour, minute and second?
							 HTM_DONT_SUBMIT_ON_CHANGE);
	 HTM_TD_End ();

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/************************* Show a form to enter a date ***********************/
/*****************************************************************************/

void Dat_WriteFormClientLocalDateTimeFromTimeUTC (const char *Id,
                                                  Dat_StartEndTime_t StartEndTime,
                                                  time_t TimeUTC,
                                                  unsigned FirstYear,
                                                  unsigned LastYear,
                                                  Dat_FormSeconds FormSeconds,
                                                  Dat_SetHMS SetHMS,
                                                  HTM_SubmitOnChange_t SubmitOnChange)
  {
   extern const char *Txt_MONTHS_SMALL[12];
   unsigned Day;
   unsigned Month;
   unsigned Year;
   unsigned Hour;
   unsigned Minute;
   unsigned Second;
   static unsigned MinutesIInterval[Dat_NUM_FORM_SECONDS] =
     {
      [Dat_FORM_SECONDS_OFF] = 5,
      [Dat_FORM_SECONDS_ON ] = 1,
     };
   static const char *Format[Dat_NUM_SET_HMS] =
     {
      [Dat_HMS_DO_NOT_SET] = NULL,
      [Dat_HMS_TO_000000 ] = "setHMSTo000000('%s');",	// Set HH:MM:SS form selectors to 00:00:00
      [Dat_HMS_TO_235959 ] = "setHMSTo235959('%s');",	// Set HH:MM:SS form selectors to 23:59:59
     };
   char *FuncsYearMonth;
   char *FuncDayHMS;
   char *IdTimeUTC;

   /***** Build strings for onchange functions *****/
   if (asprintf (&FuncsYearMonth,"adjustDateForm('%s');"
				 "setUTCFromLocalDateTimeForm('%s');",
		 Id,Id) < 0)
      Err_NotEnoughMemoryExit ();
   if (asprintf (&FuncDayHMS,"setUTCFromLocalDateTimeForm('%s');",
		 Id) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Year, month, day *****/
   HTM_DIV_Begin ("class=\"DATE_FORM_YMD\"");

      /* Year */
      HTM_SELECT_Begin (SubmitOnChange,FuncsYearMonth,
			"id=\"%sYear\" name=\"%sYear\""
			" class=\"INPUT_%s\"",
			Id,Dat_ParName[StartEndTime],
			The_GetSuffix ());
	 for (Year  = FirstYear;
	      Year <= LastYear;
	      Year++)
	    HTM_OPTION (HTM_Type_UNSIGNED,&Year,
			HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%u",Year);
      HTM_SELECT_End ();

      /* Month */
      HTM_SELECT_Begin (SubmitOnChange,FuncsYearMonth,
			"id=\"%sMonth\" name=\"%sMonth\""
			" class=\"DATE_FORM_MONTH INPUT_%s\"",
			Id,Dat_ParName[StartEndTime],
			The_GetSuffix ());
	 for (Month = 1;
	      Month <= 12;
	      Month++)
	    HTM_OPTION (HTM_Type_UNSIGNED,&Month,
			HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%s",Txt_MONTHS_SMALL[Month - 1]);
      HTM_SELECT_End ();

      /* Day */
      HTM_SELECT_Begin (SubmitOnChange,FuncDayHMS,
			"id=\"%sDay\" name=\"%sDay\""
			" class=\"INPUT_%s\"",
			Id,Dat_ParName[StartEndTime],
			The_GetSuffix ());
	 for (Day  = 1;
	      Day <= 31;
	      Day++)
	    HTM_OPTION (HTM_Type_UNSIGNED,&Day,
			HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%u",Day);
      HTM_SELECT_End ();

   HTM_DIV_End ();

   /***** Hour, minute, second *****/
   HTM_DIV_Begin ("class=\"DATE_FORM_HMS\"");

	 /* Hour */
	 HTM_SELECT_Begin (SubmitOnChange,FuncDayHMS,
			   "id=\"%sHour\" name=\"%sHour\""
			   " class=\"INPUT_%s\"",
			   Id,Dat_ParName[StartEndTime],
			   The_GetSuffix ());
	    for (Hour  = 0;
		 Hour <= 23;
		 Hour++)
	       HTM_OPTION (HTM_Type_UNSIGNED,&Hour,
			   HTM_OPTION_UNSELECTED,
			   HTM_OPTION_ENABLED,
			   "%02u h",Hour);
	 HTM_SELECT_End ();

	 /* Minute */
	 HTM_SELECT_Begin (SubmitOnChange,FuncDayHMS,
			   "id=\"%sMinute\" name=\"%sMinute\""
			   " class=\"INPUT_%s\"",
			   Id,Dat_ParName[StartEndTime],
			   The_GetSuffix ());
	    for (Minute = 0;
		 Minute < 60;
		 Minute += MinutesIInterval[FormSeconds])
	       HTM_OPTION (HTM_Type_UNSIGNED,&Minute,
			   HTM_OPTION_UNSELECTED,
			   HTM_OPTION_ENABLED,
			   "%02u &prime;",Minute);
	 HTM_SELECT_End ();

	 /* Second */
	 if (FormSeconds == Dat_FORM_SECONDS_ON)
	   {
	    HTM_SELECT_Begin (SubmitOnChange,FuncDayHMS,
			      "id=\"%sSecond\" name=\"%sSecond\""
			      " class=\"INPUT_%s\"",
			      Id,Dat_ParName[StartEndTime],
			      The_GetSuffix ());
	       for (Second  = 0;
		    Second <= 59;
		    Second++)
		  HTM_OPTION (HTM_Type_UNSIGNED,&Second,
			      HTM_OPTION_UNSELECTED,
			      HTM_OPTION_ENABLED,
			      "%02u &Prime;",Second);
	    HTM_SELECT_End ();
	   }

   HTM_DIV_End ();

   /***** Free strings for onchange functions *****/
   free (FuncsYearMonth);
   free (FuncDayHMS);

   /***** Hidden field with UTC time (seconds since 1970) used to send time *****/
   if (asprintf (&IdTimeUTC,"%sTimeUTC",Id) < 0)
      Err_NotEnoughMemoryExit ();
   Par_PutParLong (IdTimeUTC,Dat_ParTimeUTCName[StartEndTime],(long) TimeUTC);
   free (IdTimeUTC);

   /***** Script to set selectors to local date and time from UTC time *****/
   HTM_SCRIPT_Begin (NULL,NULL);
      HTM_TxtF ("setLocalDateTimeFormFromUTC('%s',%ld);\n"	// Set date-time form from UTC time
		"adjustDateForm('%s');\n"			// Adjust date-time form
		"setUTCFromLocalDateTimeForm('%s');\n",		// Adjust UTC time from date-time form
		Id,(long) TimeUTC,
		Id,
		Id);
      if (Format[SetHMS])
	 HTM_TxtF (Format[SetHMS],Id);	// Hidden TimeUTC is also adjusted
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/***************** Get an hour-minute time from a form ***********************/
/*****************************************************************************/

time_t Dat_GetTimeUTCFromForm (Dat_StartEndTime_t StartEndTime)
  {
   return Par_GetParLong (Dat_ParTimeUTCName[StartEndTime]);
  }

/*****************************************************************************/
/**************** Put a hidden param with time difference ********************/
/**************** between UTC time and client local time, ********************/
/**************** in minutes                              ********************/
/*****************************************************************************/

void Dat_PutParBrowserTZDiff (void)
  {
   Par_PutParString ("BrowserTZName","BrowserTZName","");
   Par_PutParLong ("BrowserTZDiff","BrowserTZDiff",0);
   HTM_SCRIPT_Begin (NULL,NULL);
      HTM_TxtF ("setTZname('BrowserTZName');"
		"setTZ('BrowserTZDiff');");
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/*************************** Get browser time zone ***************************/
/*****************************************************************************/
// MySQL CONVERT_TZ function may fail around changes related to Daylight Saving Time
// when a fixed amount (for example +01:00) is used as destination time zone,
// because this amount may be different before and after the DST change

void Dat_GetBrowserTimeZone (char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool TZNameIsUsable = false;
   char IntStr[Cns_MAX_DECIMAL_DIGITS_INT + 1];
   int ClientUTCMinusLocal;	// Time difference between UTC time and client local time, in minutes

   /***** 1. Get client time zone name *****/
   // We get client time zone using JavaScript jstz script, available in:
   // - http://pellepim.bitbucket.org/jstz/
   // - https://bitbucket.org/pellepim/jstimezonedetect/
   // The return value is an IANA zone info key (aka the Olson time zone database).
   // https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
   // For example, if browser is in Madrid(Spain) timezone, "Europe/Berlin" will be returned.
   Par_GetParText ("BrowserTZName",BrowserTimeZone,Dat_MAX_BYTES_TIME_ZONE);

   /* Check if client time zone is usable with CONVERT_TZ */
   if (BrowserTimeZone[0])
     {
      /* Try to convert a date from server time zone to browser time zone */
      if (DB_QuerySELECT (&mysql_res,"can not check if time zone name"
				     " is usable",
			  "SELECT CONVERT_TZ(NOW(),@@session.time_zone,'%s')",	// row[0]
                          BrowserTimeZone))
	{
         row = mysql_fetch_row (mysql_res);
         if (row[0] != NULL)
            TZNameIsUsable = true;
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }

   if (!TZNameIsUsable)
     {
      /***** 2. Get client time zone difference *****/
      // We get client TZ difference using JavaScript getTimezoneOffset() method
      // getTimezoneOffset() returns UTC-time - browser-local-time, in minutes.
      // For example, if browser time zone is GMT+2, -120 will be returned.
      Par_GetParText ("BrowserTZDiff",IntStr,Cns_MAX_DECIMAL_DIGITS_INT);
      if (sscanf (IntStr,"%d",&ClientUTCMinusLocal) != 1)
	 ClientUTCMinusLocal = 0;

      /* Convert from minutes to +-hh:mm */
      // BrowserTimeZone must have space for strings in +hh:mm format (6 chars + \0)
      if (ClientUTCMinusLocal > 0)
	 snprintf (BrowserTimeZone,Dat_MAX_BYTES_TIME_ZONE + 1,"-%02u:%02u",
		   (unsigned) ClientUTCMinusLocal / 60,
		   (unsigned) ClientUTCMinusLocal % 60);
      else	// ClientUTCMinusLocal <= 0
	 snprintf (BrowserTimeZone,Dat_MAX_BYTES_TIME_ZONE + 1,"+%02u:%02u",
		   (unsigned) (-ClientUTCMinusLocal) / 60,
		   (unsigned) (-ClientUTCMinusLocal) % 60);
     }
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
		        struct Dat_Date *DateSelected,
                        HTM_SubmitOnChange_t SubmitOnChange,bool Disabled)
  {
   extern const char *Txt_MONTHS_SMALL[12];
   char *FuncOnChange;
   unsigned Year;
   unsigned Month;
   unsigned Day;
   unsigned NumDaysSelectedMonth;


   if (asprintf (&FuncOnChange,"adjustDateForm('%s');",Id) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Year *****/
   // HTM_TD_Begin ("class=\"CM\"");
      HTM_SELECT_Begin (SubmitOnChange,FuncOnChange,
			"id=\"%sYear\" name=\"%sYear\""
			" class=\"INPUT_%s\"%s",
			Id,Id,
			The_GetSuffix (),
			Disabled ? " disabled=\"disabled\"" :
				   "");
	 HTM_OPTION (HTM_Type_STRING,"0",
		     HTM_OPTION_UNSELECTED,
		     HTM_OPTION_ENABLED,
		     "-");
	 for (Year  = FirstYear;
	      Year <= LastYear;
	      Year++)
	    HTM_OPTION (HTM_Type_UNSIGNED,&Year,
			Year == DateSelected->Year ? HTM_OPTION_SELECTED :
						     HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%u",Year);
      HTM_SELECT_End ();
   // HTM_TD_End ();

   /***** Month *****/
   // HTM_TD_Begin ("class=\"CM\"");
      HTM_SELECT_Begin (SubmitOnChange,FuncOnChange,
			"id=\"%sMonth\" name=\"%sMonth\""
			" class=\"INPUT_%s\"%s",
			Id,Id,
			The_GetSuffix (),
			Disabled ? " disabled=\"disabled\"" :
				   "");
	 HTM_OPTION (HTM_Type_STRING,"0",
		     HTM_OPTION_UNSELECTED,
		     HTM_OPTION_ENABLED,
		     "-");
	 for (Month  =  1;
	      Month <= 12;
	      Month++)
	    HTM_OPTION (HTM_Type_UNSIGNED,&Month,
			Month == DateSelected->Month ? HTM_OPTION_SELECTED :
						       HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%s",Txt_MONTHS_SMALL[Month - 1]);
      HTM_SELECT_End ();
   // HTM_TD_End ();

   free (FuncOnChange);

   /***** Day *****/
   // HTM_TD_Begin ("class=\"CM\"");
      HTM_SELECT_Begin (SubmitOnChange,NULL,
			"id=\"%sDay\" name=\"%sDay\""
			" class=\"INPUT_%s\"%s",
			Id,Id,
			The_GetSuffix (),
			Disabled ? " disabled=\"disabled\"" :
				   "");
	    HTM_OPTION (HTM_Type_STRING,"0",
			HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"-");
	    NumDaysSelectedMonth = (DateSelected->Month == 0) ? 31 :
								((DateSelected->Month == 2) ? Dat_GetNumDaysFebruary (DateSelected->Year) :
											      Dat_NumDaysMonth[DateSelected->Month]);
	    for (Day  = 1;
		 Day <= NumDaysSelectedMonth;
		 Day++)
	       HTM_OPTION (HTM_Type_UNSIGNED,&Day,
			   Day == DateSelected->Day ? HTM_OPTION_SELECTED :
						      HTM_OPTION_UNSELECTED,
			   HTM_OPTION_ENABLED,
			   "%u",Day);
      HTM_SELECT_End ();
   // HTM_TD_End ();
  }

/*****************************************************************************/
/*************************** Get a date from a form **************************/
/*****************************************************************************/

void Dat_GetDateFromForm (const char *ParNameDay,const char *ParNameMonth,const char *ParNameYear,
                          unsigned *Day,unsigned *Month,unsigned *Year)
  {
   /**** Get day ****/
   *Day   = (unsigned) Par_GetParUnsignedLong (ParNameDay  ,1,31,0);

   /**** Get month ****/
   *Month = (unsigned) Par_GetParUnsignedLong (ParNameMonth,1,12,0);

   /**** Get year ****/
   *Year  = (unsigned) Par_GetParUnsignedLong (ParNameYear ,0,UINT_MAX,0);
  }

/*****************************************************************************/
/******* Set initial date to distant past and end date to current date *******/
/*****************************************************************************/

void Dat_SetIniEndDatesToPastAndNow (void)
  {
   Dat_Time.Range.TimeUTC[Dat_STR_TIME] = (time_t) 0;
   Dat_Time.Range.TimeUTC[Dat_END_TIME] = Dat_GetStartExecutionTimeUTC ();
  }

/*****************************************************************************/
/************ Set end date to current date                        ************/
/************ and set initial date to end date minus several days ************/
/*****************************************************************************/

void Dat_SetIniEndDatesToRecentWeeks (void)
  {
   Dat_Time.Range.TimeUTC[Dat_END_TIME] = Dat_GetStartExecutionTimeUTC ();
   Dat_Time.Range.TimeUTC[Dat_STR_TIME] = Dat_Time.Range.TimeUTC[Dat_END_TIME] -
	                                  ((Cfg_DAYS_IN_RECENT_LOG - 1) * 24 * 60 * 60);
  }

/*****************************************************************************/
/******** Write parameters of initial and final dates *****/
/*****************************************************************************/

void Dat_WriteParsIniEndDates (void)
  {
   Par_PutParUnsigned (NULL,Dat_ParTimeUTCName[Dat_STR_TIME],Dat_Time.Range.TimeUTC[Dat_STR_TIME]);
   Par_PutParUnsigned (NULL,Dat_ParTimeUTCName[Dat_END_TIME],Dat_Time.Range.TimeUTC[Dat_END_TIME]);
  }

/*****************************************************************************/
/************************** Get initial and end dates ************************/
/*****************************************************************************/

void Dat_GetIniEndDatesFromForm (void)
  {
   struct tm tm;
   struct tm *tm_ptr;

   /***** Get initial date *****/
   Dat_Time.Range.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   if (Dat_Time.Range.TimeUTC[Dat_STR_TIME])
      /* Convert time UTC to a local date */
      tm_ptr = Dat_GetLocalTimeFromClock (&Dat_Time.Range.TimeUTC[Dat_STR_TIME]);
   else	// Dat_Time.Range.TimeUTC[Dat_STR_TIME] == 0 ==> initial date not specified
     {
      tm.tm_year  = Cfg_LOG_START_YEAR - 1900;
      tm.tm_mon   =  0;	// January
      tm.tm_mday  =  1;
      tm.tm_hour  =  0;
      tm.tm_min   =  0;
      tm.tm_sec   =  0;
      tm.tm_isdst = -1;	// a negative value means that mktime() should
			// (use timezone information and system databases to)
			// attempt to determine whether DST
			// is in effect at the specified time.
      if ((Dat_Time.Range.TimeUTC[Dat_STR_TIME] = mktime (&tm)) < 0)
	 Dat_Time.Range.TimeUTC[Dat_STR_TIME] = (time_t) 0;
      tm_ptr = &tm;
     }

   Dat_Time.Range.DateTime[Dat_STR_TIME].Date.Year   = tm_ptr->tm_year + 1900;
   Dat_Time.Range.DateTime[Dat_STR_TIME].Date.Month  = tm_ptr->tm_mon  + 1;
   Dat_Time.Range.DateTime[Dat_STR_TIME].Date.Day    = tm_ptr->tm_mday;
   Dat_Time.Range.DateTime[Dat_STR_TIME].Time.Hour   = tm_ptr->tm_hour;
   Dat_Time.Range.DateTime[Dat_STR_TIME].Time.Minute = tm_ptr->tm_min;
   Dat_Time.Range.DateTime[Dat_STR_TIME].Time.Second = tm_ptr->tm_sec;

   /***** Get end date *****/
   Dat_Time.Range.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);
   if (Dat_Time.Range.TimeUTC[Dat_END_TIME] == 0)	// Dat_Time.Range.TimeUTC[Dat_END_TIME] == 0 ==> end date not specified
      Dat_Time.Range.TimeUTC[Dat_END_TIME] = Dat_GetStartExecutionTimeUTC ();

   /* Convert current time UTC to a local date */
   tm_ptr = Dat_GetLocalTimeFromClock (&Dat_Time.Range.TimeUTC[Dat_END_TIME]);

   Dat_Time.Range.DateTime[Dat_END_TIME].Date.Year   = tm_ptr->tm_year + 1900;
   Dat_Time.Range.DateTime[Dat_END_TIME].Date.Month  = tm_ptr->tm_mon  + 1;
   Dat_Time.Range.DateTime[Dat_END_TIME].Date.Day    = tm_ptr->tm_mday;
   Dat_Time.Range.DateTime[Dat_END_TIME].Time.Hour   = tm_ptr->tm_hour;
   Dat_Time.Range.DateTime[Dat_END_TIME].Time.Minute = tm_ptr->tm_min;
   Dat_Time.Range.DateTime[Dat_END_TIME].Time.Second = tm_ptr->tm_sec;
  }

/*****************************************************************************/
/****************** Write a UTC date into RFC 822 format *********************/
/*****************************************************************************/
// tm must hold a UTC date

void Dat_WriteRFC822DateFromTM (FILE *File,struct tm *tm_ptr)
  {
   static const char *StrDayOfWeek[7] =
     {
      [0] = "Sun",
      [1] = "Mon",
      [2] = "Tue",
      [3] = "Wed",
      [4] = "Thu",
      [5] = "Fri",
      [6] = "Sat",
     };
   static const char *StrMonth[12] =
     {
      [ 0] = "Jan",
      [ 1] = "Feb",
      [ 2] = "Mar",
      [ 3] = "Apr",
      [ 4] = "May",
      [ 5] = "Jun",
      [ 6] = "Jul",
      [ 7] = "Aug",
      [ 8] = "Sep",
      [ 9] = "Oct",
      [10] = "Nov",
      [11] = "Dec",
      };

   fprintf (File,"%s, %d %s %d %02d:%02d:%02d UT",
            StrDayOfWeek[tm_ptr->tm_wday],
            tm_ptr->tm_mday,
            StrMonth[tm_ptr->tm_mon],
            tm_ptr->tm_year + 1900,
            tm_ptr->tm_hour,
            tm_ptr->tm_min,
            tm_ptr->tm_sec);
  }

/*****************************************************************************/
/************** Compute the subsequent date of a given date ******************/
/*****************************************************************************/

void Dat_GetDateAfter (struct Dat_Date *Date,struct Dat_Date *SubsequentDate)
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
/************** Compute the preceding date of a given date *******************/
/*****************************************************************************/

void Dat_GetDateBefore (struct Dat_Date *Date,struct Dat_Date *PrecedingDate)
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
/**************** Compute the week before to a given week ********************/
/*****************************************************************************/

void Dat_GetWeekBefore (struct Dat_Date *Date,struct Dat_Date *PrecedingDate)
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

void Dat_GetMonthBefore (struct Dat_Date *Date,struct Dat_Date *PrecedingDate)
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
/**************** Compute the year before to a given year ********************/
/*****************************************************************************/

void Dat_GetYearBefore (struct Dat_Date *Date,struct Dat_Date *PrecedingDate)
  {
   PrecedingDate->Year = Date->Year - 1;
  }

/*****************************************************************************/
/************** Compute the number of days beteen two dates ******************/
/*****************************************************************************/
// If the dates are the same, return 1
// If the old date is the day before the new data, return 2
// ...

unsigned Dat_GetNumDaysBetweenDates (struct Dat_Date *DateStr,
                                     struct Dat_Date *DateEnd)
  {
   int DiffDays;
   unsigned Year;

   /***** If initial year is less than end year, return 0
          (actually the difference in days should be negative) *****/
   if (DateStr->Year > DateEnd->Year)
      return 0;

   /***** Initial year is less or equal than end year ==> compute difference in days *****/
   DiffDays = (int) Dat_GetDayOfYear (DateEnd) - (int) Dat_GetDayOfYear (DateStr) + 1;
   for (Year = DateStr->Year;
	Year < DateEnd->Year;
	Year++)
      DiffDays += (int) Dat_GetNumDaysInYear (Year);
   return (DiffDays > 0) ? (unsigned) DiffDays :
	                   0;
  }

/*****************************************************************************/
/*************** Compute the number of weeks between two dates ***************/
/*****************************************************************************/
// If the two dates are in the same week, return 1

unsigned Dat_GetNumWeeksBetweenDates (struct Dat_Date *DateStr,
                                      struct Dat_Date *DateEnd)
  {
   int DiffWeeks;
   unsigned Year;

   /***** If initial year is lower than the ending year, return 0
          (actually the difference should be negative) *****/
   if (DateStr->Year > DateEnd->Year)
      return 0;

   /***** Initial year is lower or equal to ending year ==>
          compute difference in weeks *****/
   DiffWeeks = (int) DateEnd->Week - (int) DateStr->Week + 1;
   for (Year = DateStr->Year;
	Year < DateEnd->Year;
	Year++)
      DiffWeeks += (int) Dat_GetNumWeeksInYear (Year);
   return (DiffWeeks > 0) ? (unsigned) DiffWeeks :
	                    0;
  }

/*****************************************************************************/
/************* Compute the number of months between two dates ****************/
/*****************************************************************************/
// If the two dates are in the same month, return 1

unsigned Dat_GetNumMonthsBetweenDates (struct Dat_Date *DateStr,
                                       struct Dat_Date *DateEnd)
  {
   int DiffMonths;

   /***** Compute the difference in months *****/
   DiffMonths = ((int) DateEnd->Year  - (int) DateStr->Year) * 12 +
	         (int) DateEnd->Month - (int) DateStr->Month + 1;
   return (DiffMonths > 0) ? (unsigned) DiffMonths :
	                     0;
  }

/*****************************************************************************/
/************** Compute the number of years between two dates ****************/
/*****************************************************************************/
// If the two dates are in the same year, return 1

unsigned Dat_GetNumYearsBetweenDates (struct Dat_Date *DateStr,
                                      struct Dat_Date *DateEnd)
  {
   int DiffYears;

   /***** Compute the difference in years *****/
   DiffYears = (int) DateEnd->Year - (int) DateStr->Year + 1;
   return (DiffYears > 0) ? (unsigned) DiffYears :
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
   struct tm tm;
   struct tm *tm_ptr;
   time_t t;

   /***** Create clock in UNIX time from date *****/
   tm.tm_year  = Year  - 1900;
   tm.tm_mon   = Month - 1;
   tm.tm_mday  = Day;
   tm.tm_hour  = 0;
   tm.tm_min   = 0;
   tm.tm_sec   = 0;
   tm.tm_isdst = -1;	// a negative value means that mktime() should
			// (use timezone information and system databases to)
			// attempt to determine whether DST
			// is in effect at the specified time.
   if ((t = mktime (&tm)) < 0)
      return 0;

   /***** Compute local time, adjusting day of week *****/
   tm_ptr = Dat_GetLocalTimeFromClock (&t);

   return (unsigned) tm_ptr->tm_wday;
  }

/* Alternative:

   if (Month <= 2)
     {
      Month += 12;
      Year--;
     }
   return (
	   (
            (
	     Day +
	     Month*2 + (Month*3 + 3) / 5 +
	     Year + Year/4 - Year/100 + Year/400 +
	     2
	    ) % 7
	   ) + 5
	  ) % 7;

Code generated by gcc for the alternative:

0000000000001810 <Dat_GetDayOfWeek>:
    1810:	83 fe 02             	cmp    $0x2,%esi
    1813:	77 06                	ja     181b <Dat_GetDayOfWeek+0xb>
    1815:	83 c6 0c             	add    $0xc,%esi
    1818:	83 ef 01             	sub    $0x1,%edi
    181b:	8d 44 17 02          	lea    0x2(%rdi,%rdx,1),%eax
    181f:	ba 1f 85 eb 51       	mov    $0x51eb851f,%edx
    1824:	8d 0c 70             	lea    (%rax,%rsi,2),%ecx
    1827:	89 f8                	mov    %edi,%eax
    1829:	c1 e8 02             	shr    $0x2,%eax
    182c:	01 c1                	add    %eax,%ecx
    182e:	89 f8                	mov    %edi,%eax
    1830:	bf 25 49 92 24       	mov    $0x24924925,%edi
    1835:	f7 e2                	mul    %edx
    1837:	89 d0                	mov    %edx,%eax
    1839:	c1 ea 05             	shr    $0x5,%edx
    183c:	c1 e8 07             	shr    $0x7,%eax
    183f:	01 c1                	add    %eax,%ecx
    1841:	8d 44 76 03          	lea    0x3(%rsi,%rsi,2),%eax
    1845:	29 d1                	sub    %edx,%ecx
    1847:	ba cd cc cc cc       	mov    $0xcccccccd,%edx
    184c:	f7 e2                	mul    %edx
    184e:	c1 ea 02             	shr    $0x2,%edx
    1851:	01 d1                	add    %edx,%ecx
    1853:	89 c8                	mov    %ecx,%eax
    1855:	89 ce                	mov    %ecx,%esi
    1857:	f7 e7                	mul    %edi
    1859:	29 d6                	sub    %edx,%esi
    185b:	d1 ee                	shr    %esi
    185d:	01 f2                	add    %esi,%edx
    185f:	c1 ea 02             	shr    $0x2,%edx
    1862:	8d 04 d5 00 00 00 00 	lea    0x0(,%rdx,8),%eax
    1869:	29 d0                	sub    %edx,%eax
    186b:	29 c1                	sub    %eax,%ecx
    186d:	83 c1 05             	add    $0x5,%ecx
    1870:	89 c8                	mov    %ecx,%eax
    1872:	f7 e7                	mul    %edi
    1874:	89 c8                	mov    %ecx,%eax
    1876:	29 d0                	sub    %edx,%eax
    1878:	d1 e8                	shr    %eax
    187a:	01 d0                	add    %edx,%eax
    187c:	c1 e8 02             	shr    $0x2,%eax
    187f:	8d 14 c5 00 00 00 00 	lea    0x0(,%rax,8),%edx
    1886:	29 c2                	sub    %eax,%edx
    1888:	29 d1                	sub    %edx,%ecx
    188a:	89 c8                	mov    %ecx,%eax
    188c:	c3                   	retq
    188d:	0f 1f 00             	nopl   (%rax)

Understanding the code generated by gcc:

0000000000001810 <Dat_GetDayOfWeek>:
    1810:	83 fe 02             	cmp    $2,Month
    1813:	77 06                	ja     continue

    1815:	83 c6 0c             	add    $12,Month
    1818:	83 ef 01             	sub    $1,Year
continue:
    181b:	8d 44 17 02          	lea    2(Year,Day,1),Suma1	// SumAux = Day + Year + 2

    181f:	ba 1f 85 eb 51       	mov    $0x51eb851f,%edx		// 1374389535

    1824:	8d 0c 70             	lea    (SumAux,Month,2),n	// n = Day + Year + 2 + Month*2
    1827:	89 f8                	mov    Year,%eax
    1829:	c1 e8 02             	shr    $2,%eax			// Year / 4
    182c:	01 c1                	add    %eax,n			// n = Day + Year + 2 + Month*2 + Year/4
    182e:	89 f8                	mov    Year,%eax
    1830:	bf 25 49 92 24       	mov    $0x24924925,%edi		// 613566757
    1835:	f7 e2                	mul    %edx			// Year * 1374389535

    1837:	89 d0                	mov    %edx,%eax		// (Year * 1374389535) / 2^32 = (Year * 2^32 * 2^5 / 100) / 2^32 = (Year * 2^5) / 100
    1839:	c1 ea 05             	shr    $5,%edx			// (Year * 1374389535) / 2^32 / 2^5 = Year / 100
    183c:	c1 e8 07             	shr    $7,%eax			// (Year * 1374389535) / 2^32 / 2^7 = Year / 400
    183f:	01 c1                	add    %eax,n			// n = Day + Year + 2 + Month*2 + Year/4 + Year/400
    1841:	8d 44 76 03          	lea    3(Month,Month,2),%eax	// Month*3 + 3
    1845:	29 d1                	sub    %edx,n			// n = Day + Year + 2 + Month*2 + Year/4 + Year/400 - Year/100

    1847:	ba cd cc cc cc       	mov    $0xcccccccd,%edx
    184c:	f7 e2                	mul    %edx			// (Month*3 + 3) * 3435973837
    184e:	c1 ea 02             	shr    $2,%edx			// (Month*3 + 3) * 3435973837 / 2^32 / 2^2 = (Month*3 + 3) / 5
    1851:	01 d1                	add    %edx,n			// n = Day + Year + 2 + Month*2 + Year/4 + Year/400 - Year/100 + (Month*3 + 3) / 5

    1853:	89 c8                	mov    n,%eax
    1855:	89 ce                	mov    n,%esi

									// Algorithm for remainder: https://doc.lagout.org/security/Hackers%20Delight.pdf, page 209

    1857:	f7 e7                	mul    %edi			// edx = q = M*n/2**32
    1859:	29 d6                	sub    %edx,%esi		// esi = t = n - q
    185b:	d1 ee                	shr    %esi			// esi = t = (n - q)/2
    185d:	01 f2                	add    %esi,%edx		// edx = t = (n - q)/2 + q = (n + q)/2
    185f:	c1 ea 02             	shr    $2,%edx			// edx = q = (n + q)/8 = (n+Mn/2**32)/8 = floor(n/7)
    1862:	8d 04 d5 00 00 00 00 	lea    0(,%rdx,8),%eax		// eax = q*8
    1869:	29 d0                	sub    %edx,%eax		// eax = q*8-q = q*7
    186b:	29 c1                	sub    %eax,%ecx		// ecx = r = n - q*7

    186d:	83 c1 05             	add    $5,Mod			// Mod += 5
    1870:	89 c8                	mov    Mod,%eax

    1872:	f7 e7                	mul    %edi
    1874:	89 c8                	mov    Mod,%eax
    1876:	29 d0                	sub    %edx,%eax
    1878:	d1 e8                	shr    %eax
    187a:	01 d0                	add    %edx,%eax
    187c:	c1 e8 02             	shr    $2,%eax
    187f:	8d 14 c5 00 00 00 00 	lea    0(,%rax,8),%edx
    1886:	29 c2                	sub    %eax,%edx
    1888:	29 d1                	sub    %edx,Mod			// Mod % 7

    188a:	89 c8                	mov    Mod,%eax
    188c:	c3                   	retq
    188d:	0f 1f 00             	nopl   (%rax)

 */

/*****************************************************************************/
/***************** Compute the day of year (from 1 to 366) *******************/
/*****************************************************************************/

unsigned Dat_GetDayOfYear (struct Dat_Date *Date)
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

void Dat_CalculateWeekOfYear (struct Dat_Date *Date)
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
   (3) Thursday                 | January,   3 of Year + 1                   |
   (4) Friday                   | January,   4 of Year + 1                   |
   (5) Saturday                 | January,   5 of Year + 1                   |
   (6) Sunday                   | December, 31 of Year                       |
   */
   int DayThatFirstWeekStarts[7] = {1,0,-1,-2,4,3,2};
   unsigned DayThatLastWeekEnds[7] = {30,29,28,34,33,32,31};
   unsigned January1DayOfWeek   = Dat_GetDayOfWeek (Date->Year, 1, 1); // From 0 to 6
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

void Dat_AssignDate (struct Dat_Date *DateDst,struct Dat_Date *DateSrc)
  {
   DateDst->Year  = DateSrc->Year;
   DateDst->Month = DateSrc->Month;
   DateDst->Day   = DateSrc->Day;
   DateDst->Week  = DateSrc->Week;
   Str_Copy (DateDst->YYYYMMDD,DateSrc->YYYYMMDD,sizeof (DateDst->YYYYMMDD) - 1);
  }

/*****************************************************************************/
/****** Write script to automatically update clocks of connected users *******/
/*****************************************************************************/

void Dat_WriteScriptMonths (void)
  {
   extern const char *Txt_MONTHS_SMALL[12];
   extern const char *Txt_MONTHS_SMALL_SHORT[12];
   unsigned NumMonth;

   /***** Month names (full) *****/
   HTM_Txt ("\tvar Months = [");
   for (NumMonth = 0;
	NumMonth < 12;
	NumMonth++)
     {
      if (NumMonth)
	 HTM_Comma ();
      HTM_TxtF ("'%s'",Txt_MONTHS_SMALL[NumMonth]);
     }
   HTM_Txt ("];\n");

   /***** Month names (short) *****/
   HTM_Txt ("\tvar MonthsShort = [");
   for (NumMonth = 0;
	NumMonth < 12;
	NumMonth++)
     {
      if (NumMonth)
	 HTM_Comma ();
      HTM_TxtF ("'%s'",Txt_MONTHS_SMALL_SHORT[NumMonth]);
     }
   HTM_Txt ("];\n");
  }

/*****************************************************************************/
/********* Write time difference in seconds as hours:minutes:seconds *********/
/*****************************************************************************/

void Dat_WriteHoursMinutesSecondsFromSeconds (time_t Seconds)
  {
   time_t Hours   = Seconds / (60 * 60);
   time_t Minutes = (Seconds / 60) % 60;

   Seconds %= 60;
   if (Hours)
      HTM_TxtF ("%ld:%02ld&prime;%02ld&Prime;",(long) Hours,(long) Minutes,(long) Seconds);
   else if (Minutes)
      HTM_TxtF ("%ld&prime;%02ld&Prime;",(long) Minutes,(long) Seconds);
   else
      HTM_TxtF ("%ld&Prime;",(long) Seconds);
  }

/*****************************************************************************/
/******************* Write time as hours:minutes:seconds *********************/
/*****************************************************************************/

void Dat_WriteHoursMinutesSeconds (struct Dat_Time *Time)
  {
   if (Time->Hour)
      HTM_TxtF ("%u:%02u&prime;%02u&Prime;",Time->Hour,Time->Minute,Time->Second);
   else if (Time->Minute)
      HTM_TxtF ("%u&prime;%02u&Prime;",Time->Minute,Time->Second);
   else
      HTM_TxtF ("%u&Prime;",Time->Second);
  }

/*****************************************************************************/
/**** Write call to JavaScript function to write local date from UTC time ****/
/*****************************************************************************/

void Dat_WriteLocalDateHMSFromUTC (const char *Id,time_t TimeUTC,
				   Dat_Format_t DateFormat,Dat_Separator_t Separator,
				   bool WriteToday,bool WriteDateOnSameDay,
				   bool WriteWeekDay,unsigned WriteHMS)
  {
   static const char *SeparatorStr[Dat_NUM_SEPARATORS] =
     {
      [Dat_SEPARATOR_NONE ] = "",
      [Dat_SEPARATOR_COMMA] = ", ",
      [Dat_SEPARATOR_BREAK] = "<br />",
     };

   HTM_SCRIPT_Begin (NULL,NULL);
      HTM_TxtF ("writeLocalDateHMSFromUTC('%s',%ld,%u,'%s',%u,%s,%s,%s,0x%x);",
		Id,(long) TimeUTC,(unsigned) DateFormat,SeparatorStr[Separator],
		(unsigned) Gbl.Prefs.Language,
		WriteToday         ? "true" :
				     "false",
		WriteDateOnSameDay ? "true" :
				     "false",
		WriteWeekDay       ? "true" :
				     "false",
		WriteHMS);
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/******** Get and show number of users who have chosen a date format *********/
/*****************************************************************************/

void Dat_GetAndShowNumUsrsPerDateFormat (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_dates;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Format;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   unsigned Format;
   char *SubQuery;
   unsigned NumUsrs[Dat_NUM_OPTIONS_FORMAT];
   unsigned NumUsrsTotal = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_DATE_FORMAT],NULL,NULL,
                      Hlp_ANALYTICS_Figures_dates,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Format          ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** For each format... *****/
      for (Format  = (Dat_Format_t) 0;
	   Format <= (Dat_Format_t) (Dat_NUM_OPTIONS_FORMAT - 1);
	   Format++)
	{
	 /* Get number of users who have chosen this date format from database */
	 if (asprintf (&SubQuery,"usr_data.DateFormat=%u",
		       (unsigned) Format) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[Format] = Usr_DB_GetNumUsrsWhoChoseAnOption (SubQuery);
	 free (SubQuery);

	 /* Update total number of users */
	 NumUsrsTotal += NumUsrs[Format];
	}

      /***** Write number of users who have chosen each date format *****/
      for (Format  = (Dat_Format_t) 0;
	   Format <= (Dat_Format_t) (Dat_NUM_OPTIONS_FORMAT - 1);
	   Format++)
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM DAT_STRONG_%s\"",
	                  The_GetSuffix ());
	       Dat_PutSpanDateFormat (Format);
	       Dat_PutScriptDateFormat (Format);
	    HTM_TD_End ();

	    HTM_TD_Unsigned (NumUsrs[Format]);
	    HTM_TD_Percentage (NumUsrs[Format],NumUsrsTotal);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
