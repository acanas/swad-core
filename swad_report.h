// swad_report.h: report on my use of the platform

#ifndef _SWAD_REP
#define _SWAD_REP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_constant.h"
#include "swad_profile.h"

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

struct Rep_CurrentTimeUTC
  {
   char StrDate[3 * (Cns_MAX_DIGITS_INT + 1)];	// Example: 2016-10-02
   char StrTime[3 * (Cns_MAX_DIGITS_INT + 1)];	// Example: 19:03:49
   unsigned Date;		// Example: 20161002
   unsigned Time;		// Example: 190349
  };

struct Rep_Hits
  {
   unsigned long Num;
   unsigned long Max;
  };

struct Rep_Report
  {
   struct Prf_UsrFigures UsrFigures;
   struct tm tm_FirstClickTime;
   struct tm tm_CurrentTime;
   struct Rep_CurrentTimeUTC CurrentTimeUTC;
   struct Rep_Hits Hits;
   unsigned long MaxHitsPerYear;
   char FilenameReport[NAME_MAX + 1];
   char Permalink[WWW_MAX_BYTES_WWW + 1];
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Rep_ReqMyUsageReport (void);
void Rep_ShowMyUsageReport (void);
void Rep_PrintMyUsageReport (void);

void Rep_RemoveUsrUsageReports (long UsrCod);

#endif
