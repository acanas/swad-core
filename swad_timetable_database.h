// swad_timetable_database.h: timetables, operations with database

#ifndef _SWAD_TMT_DB
#define _SWAD_TMT_DB
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_timetable.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Tmt_DB_InsertHourInCrsTimeTable (long HieCod,
                                      const struct Tmt_Column *Column,
                                      unsigned Weekday,unsigned Hour,unsigned Min,
                                      unsigned SecondsPerInterval);
void Tmt_DB_InsertHourInUsrTimeTable (long UsrCod,
                                      const struct Tmt_Column *Column,
                                      unsigned Weekday,unsigned Hour,unsigned Min,
                                      unsigned SecondsPerInterval);

unsigned Tmt_DB_GetTimeTable (MYSQL_RES **mysql_res,
                              Tmt_TimeTableType_t Type,long UsrCod);

void Tmt_DB_OrphanAllGrpsOfATypeInCrsTimeTable (long GrpTypCod);
void Tmt_DB_OrphanGrpInCrsTimeTable (long GrpCod);

void Tmt_DB_RemoveCrsTimeTable (long HieCod);
void Tmt_DB_RemoveUsrTimeTable (long UsrCod);

#endif
