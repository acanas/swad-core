// swad_statistic_database.h: statistics, operations with database

#ifndef _SWAD_STA_DB
#define _SWAD_STA_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_statistic.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Sta_DB_GetHits (MYSQL_RES **mysql_res,
                         const struct Sta_Stats *Stats,
                         const char *LogTable,
                         const char BrowserTimeZone[Dat_MAX_BYTES_TIME_ZONE + 1],
                         unsigned NumUsrsInList,
			 const long *LstSelectedUsrCods);

#endif
