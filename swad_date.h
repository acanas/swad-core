// swad_date.h: dates

#ifndef _SWAD_DAT
#define _SWAD_DAT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For FILE *
#include <time.h>

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Dat_SECONDS_IN_ONE_MONTH (30UL*24UL*60UL*60UL)

#define Dat_MAX_BYTES_TIME_ZONE 256

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Date
  {
   unsigned Day;
   unsigned Month;
   unsigned Year;
   unsigned Week;
   char YYYYMMDD[4+2+2+1];
  };
struct Time
  {
   unsigned Hour;
   unsigned Minute;
   unsigned Second;
  };
struct Hour
  {
   unsigned Hour;
   unsigned Minute;
  };
struct DateTime
  {
   struct Date Date;
   struct Time Time;
   char YYYYMMDDHHMMSS[4+2+2+2+2+2+1];
  };

#define Dat_NUM_TIME_STATUS 3
typedef enum
  {
   Dat_PAST    = 0,
   Dat_PRESENT = 1,
   Dat_FUTURE  = 2,
  } Dat_TimeStatus_t;

typedef enum
  {
   Dat_FORM_SECONDS_OFF,
   Dat_FORM_SECONDS_ON,
  } Dat_FormSeconds;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Dat_GetStartExecutionTimeUTC (void);
void Dat_GetAndConvertCurrentDateTime (void);

time_t Dat_GetUNIXTimeFromStr (const char *Str);
bool Dat_GetDateFromYYYYMMDD (struct Date *Date,const char *YYYYMMDDString);

void Dat_ShowClientLocalTime (void);

void Dat_GetLocalTimeFromClock (const time_t *clock);
void Dat_ConvDateToDateStr (struct Date *Date,char *DateStr);

void Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (void);
void Dat_PutFormStartEndClientLocalDateTimes (time_t TimeUTC[2],
                                              Dat_FormSeconds FormSeconds);

void Dat_WriteFormClientLocalDateTimeFromTimeUTC (const char *Id,
                                                  const char *ParamName,
                                                  time_t TimeUTC,
                                                  unsigned FirstYear,
                                                  unsigned LastYear,
                                                  Dat_FormSeconds FormSeconds,
                                                  bool SubmitFormOnChange);
time_t Dat_GetTimeUTCFromForm (const char *ParamName);

void Dat_PutHiddenParBrowserTZDiff (void);
void Dat_GetBrowserTimeZone (char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE+1]);

void Dat_WriteFormDate (unsigned FirstYear,unsigned LastYear,
	                const char *Id,
		        struct Date *DateSelected,
                        bool SubmitFormOnChange,bool Disabled);
void Dat_GetDateFromForm (const char *ParamNameDay,const char *ParamNameMonth,const char *ParamNameYear,
                          unsigned *Day,unsigned *Month,unsigned *Year);

void Dat_GetIniEndDatesFromForm (void);

void Dat_WriteRFC822DateFromTM (FILE *File,struct tm *tm);

void Dat_GetDateBefore (struct Date *Date,struct Date *PrecedingDate);
void Dat_GetDateAfter (struct Date *Date,struct Date *SubsequentDate);
void Dat_GetWeekBefore (struct Date *Date,struct Date *PrecedingDate);
void Dat_GetMonthBefore (struct Date *Date,struct Date *PrecedingDate);
unsigned Dat_GetNumDaysBetweenDates (struct Date *DateIni,struct Date *DateEnd);
unsigned Dat_GetNumWeeksBetweenDates (struct Date *DateIni,struct Date *DateEnd);
unsigned Dat_GetNumMonthsBetweenDates (struct Date *DateIni,struct Date *DateEnd);
unsigned Dat_GetNumDaysInYear (unsigned Year);
unsigned Dat_GetNumDaysFebruary (unsigned Year);
bool Dat_GetIfLeapYear (unsigned Year);
unsigned Dat_GetNumWeeksInYear (unsigned Year);
unsigned Dat_GetDayOfWeek (unsigned Year,unsigned Month,unsigned Day);
unsigned Dat_GetDayOfYear (struct Date *Date);
void Dat_CalculateWeekOfYear (struct Date *Date);
void Dat_AssignDate (struct Date *DateDst,struct Date *DateSrc);

void Dat_WriteScriptMonths (void);

#endif
