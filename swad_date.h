// swad_date.h: dates

#ifndef _SWAD_DAT
#define _SWAD_DAT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include <stdio.h>		// For FILE *
#include <time.h>

#include "swad_constant.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Dat_SECONDS_IN_ONE_MONTH (30UL * 24UL * 60UL * 60UL)

#define Dat_MAX_BYTES_TIME_ZONE 256

#define Dat_MAX_BYTES_TIME (128 - 1)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Dat_LENGTH_YYYYMMDD (4 + 2 + 2)
struct Dat_Date
  {
   unsigned Day;
   unsigned Month;
   unsigned Year;
   unsigned Week;
   char YYYYMMDD[Dat_LENGTH_YYYYMMDD + 1];
  };
struct Dat_Time
  {
   unsigned Hour;
   unsigned Minute;
   unsigned Second;
  };
struct Dat_Hour
  {
   unsigned Hour;
   unsigned Minute;
  };
struct Dat_DateTime
  {
   struct Dat_Date Date;
   struct Dat_Time Time;
   char YYYYMMDDHHMMSS[4 + 2 + 2 + 2 + 2 + 2 + 1];
  };

#define Dat_NUM_TIME_STATUS 3
typedef enum
  {
   Dat_PAST    = 0,
   Dat_PRESENT = 1,
   Dat_FUTURE  = 2,
  } Dat_TimeStatus_t;

#define Dat_NUM_START_END_TIME 2
typedef enum
  {
   Dat_STR_TIME = 0,
   Dat_END_TIME = 1,
  } Dat_StartEndTime_t;

#define Dat_NUM_FORM_SECONDS 2
typedef enum
  {
   Dat_FORM_SECONDS_OFF,
   Dat_FORM_SECONDS_ON,
  } Dat_FormSeconds;

#define Dat_NUM_SET_HMS 3
typedef enum
  {
   Dat_HMS_DO_NOT_SET = 0,
   Dat_HMS_TO_000000  = 1,
   Dat_HMS_TO_235959  = 2,
  } Dat_SetHMS;

/***** Date format *****/
#define Dat_NUM_OPTIONS_FORMAT 3
typedef enum
  {
   Dat_FORMAT_YYYY_MM_DD	= 0,	// ISO 8601, default
   Dat_FORMAT_DD_MONTH_YYYY	= 1,
   Dat_FORMAT_MONTH_DD_YYYY	= 2,
  } Dat_Format_t;	// Do not change these numbers because they are used in database
#define Dat_FORMAT_DEFAULT Dat_FORMAT_YYYY_MM_DD

#define Dat_NUM_SEPARATORS 3
typedef enum
  {
   Dat_SEPARATOR_NONE,	// No separator
   Dat_SEPARATOR_COMMA,	// Comma + space
   Dat_SEPARATOR_BREAK,	// Line break
  } Dat_Separator_t;

typedef unsigned Dat_WhatToWrite_t;
#define Dat_WRITE_TODAY			((Dat_WhatToWrite_t) (1 << 5))
#define Dat_WRITE_DATE_ON_SAME_DAY	((Dat_WhatToWrite_t) (1 << 4))
#define Dat_WRITE_WEEK_DAY		((Dat_WhatToWrite_t) (1 << 3))
#define Dat_WRITE_HOUR			((Dat_WhatToWrite_t) (1 << 2))
#define Dat_WRITE_MINUTE		((Dat_WhatToWrite_t) (1 << 1))
#define Dat_WRITE_SECOND		((Dat_WhatToWrite_t) (1 << 0))

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Dat_SetStartExecutionTimeval (void);
void Dat_SetStartExecutionTimeUTC (void);
time_t Dat_GetStartExecutionTimeUTC (void);
struct Dat_Date *Dat_GetCurrentDate (void);
unsigned Dat_GetCurrentDay (void);
unsigned Dat_GetCurrentMonth (void);
unsigned Dat_GetCurrentYear (void);
long Dat_GetTimeGenerationInMicroseconds (void);
long Dat_GetTimeSendInMicroseconds (void);
void Dat_ComputeTimeToGeneratePage (void);
void Dat_ComputeTimeToSendPage (void);
void Dat_WriteTimeToGenerateAndSendPage (void);
void Dat_WriteTime (char Str[Dat_MAX_BYTES_TIME],long TimeInMicroseconds);

void Dat_ResetDate (struct Dat_Date *Date);
void Dat_ResetHour (struct Dat_Hour *Hour);

void Dat_PutBoxToSelectDateFormat (void);

void Dat_PutSpanDateFormat (Dat_Format_t Format);
void Dat_PutScriptDateFormat (Dat_Format_t Format);

void Dat_ChangeDateFormat (void);
Dat_Format_t Dat_GetDateFormatFromStr (const char *Str);

void Dat_GetAndConvertCurrentDateTime (void);

time_t Dat_GetUNIXTimeFromStr (const char *Str);
Err_SuccessOrError_t Dat_GetDateFromYYYYMMDD (struct Dat_Date *Date,
					      const char *YYYYMMDD);

void Dat_ShowClientLocalTime (void);

struct tm *Dat_GetLocalTimeFromClock (const time_t *timep);
void Dat_ConvDateToDateStr (const struct Dat_Date *Date,char StrDate[Cns_MAX_BYTES_DATE + 1]);

time_t Dat_GetRangeTimeUTC (Dat_StartEndTime_t StartEndTime);
struct Dat_Date *Dat_GetRangeDate (Dat_StartEndTime_t StartEndTime);

void Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME]);
void Dat_PutFormStartEndClientLocalDateTimes (const time_t TimeUTC[Dat_NUM_START_END_TIME],
                                              Dat_FormSeconds FormSeconds,
					      const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME]);

void Dat_WriteFormClientLocalDateTimeFromTimeUTC (const char *Id,
                                                  Dat_StartEndTime_t StartEndTime,
                                                  time_t TimeUTC,
                                                  unsigned FirstYear,
                                                  unsigned LastYear,
                                                  Dat_FormSeconds FormSeconds,
                                                  Dat_SetHMS SetHMS,
                                                  HTM_Attributes_t Attributes);
time_t Dat_GetTimeUTCFromForm (Dat_StartEndTime_t StartEndTime);

void Dat_PutParBrowserTZDiff (void);
void Dat_GetBrowserTimeZone (char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE + 1]);

void Dat_WriteFormDate (unsigned FirstYear,unsigned LastYear,
	                const char *Id,
		        struct Dat_Date *DateSelected,
                        HTM_Attributes_t Attributes);
void Dat_GetDateFromForm (const char *ParNameDay,const char *ParNameMonth,const char *ParNameYear,
                          unsigned *Day,unsigned *Month,unsigned *Year);

void Dat_SetDatesToPastNow (void);
void Dat_SetDatesToRecWeeks (void);
void Dat_WriteParsIniEndDates (void);
void Dat_GetIniEndDatesFromForm (void);

void Dat_WriteRFC822DateFromTM (FILE *File,struct tm *tm);

void Dat_GetDateAfter   (struct Dat_Date *Date,struct Dat_Date *SubsequentDate);
void Dat_GetDateBefore  (struct Dat_Date *Date,struct Dat_Date *PrecedingDate );
void Dat_GetWeekBefore  (struct Dat_Date *Date,struct Dat_Date *PrecedingDate );
void Dat_GetMonthBefore (struct Dat_Date *Date,struct Dat_Date *PrecedingDate );
void Dat_GetYearBefore  (struct Dat_Date *Date,struct Dat_Date *PrecedingDate );
unsigned Dat_GetNumDaysBetweenDates   (struct Dat_Date *DateStr,
                                       struct Dat_Date *DateEnd);
unsigned Dat_GetNumWeeksBetweenDates  (struct Dat_Date *DateStr,
                                       struct Dat_Date *DateEnd);
unsigned Dat_GetNumMonthsBetweenDates (struct Dat_Date *DateStr,
                                       struct Dat_Date *DateEnd);
unsigned Dat_GetNumYearsBetweenDates  (struct Dat_Date *DateStr,
                                       struct Dat_Date *DateEnd);
unsigned Dat_GetNumDaysInYear (unsigned Year);
unsigned Dat_GetNumDaysFebruary (unsigned Year);
unsigned Dat_GetNumWeeksInYear (unsigned Year);
unsigned Dat_GetDayOfWeek (unsigned Year,unsigned Month,unsigned Day);
unsigned Dat_GetDayOfYear (struct Dat_Date *Date);
void Dat_CalculateWeekOfYear (struct Dat_Date *Date);
void Dat_AssignDate (struct Dat_Date *DateDst,struct Dat_Date *DateSrc);

void Dat_WriteScriptMonths (void);

void Dat_WriteHoursMinutesSecondsFromSeconds (time_t TotalSeconds);
void Dat_WriteHoursMinutesSeconds (struct Dat_Time *Time);

void Dat_WriteLocalDateHMSFromUTC (const char *Id,time_t TimeUTC,
				   Dat_Format_t DateFormat,Dat_Separator_t Separator,
				   Dat_WhatToWrite_t Write);

//-------------------------------- Figures ------------------------------------
void Dat_GetAndShowNumUsrsPerDateFormat (Hie_Level_t HieLvl);

#endif
