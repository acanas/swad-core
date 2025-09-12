// swad_MFU_database.h: Most Frequently Used actions, operations with database

#ifndef _SWAD_MFU_DB
#define _SWAD_MFU_DB
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

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void MFU_DB_UpdateScoreForCurrentAction (long ActCod,double Score);
void MFU_DB_UpdateScoreForOtherActions (long ActCod);

unsigned MFU_DB_GetMFUActionsOrderByScore (MYSQL_RES **mysql_res);
unsigned MFU_DB_GetMFUActionsOrderByLastClick (MYSQL_RES **mysql_res);
Exi_Exist_t MFU_DB_GetScoreForCurrentAction (MYSQL_RES **mysql_res,long ActCod);

#endif
