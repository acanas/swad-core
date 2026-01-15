// swad_log.h: access log stored in database

#ifndef _SWAD_LOG
#define _SWAD_LOG
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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Log_SECONDS_IN_RECENT_LOG ((time_t) (Cfg_DAYS_IN_RECENT_LOG * 24UL * 60UL * 60UL))	// Remove entries in recent log oldest than this time

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Log_NOT_SEARCH,
   Log_SEARCH,
  } Log_Search_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Log_SetLogSearch (Log_Search_t Search);

void Log_LogAccess (const char *Comments);

void Log_PutLinkToLastClicks (void);
void Log_ShowLastClicks (void);
void Log_GetAndShowLastClicks (void);

#endif
