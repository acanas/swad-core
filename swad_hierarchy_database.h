// swad_hierarchy_database.h: hierarchy (system, institution, center, degree, course), operations with database

#ifndef _SWAD_HIE_DB
#define _SWAD_HIE_DB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Hie_DB_NUM_COUNT_ONLY_ACCEPTED_CRSS 2
typedef enum
  {
   Hie_DB_ANY_COURSE,
   Hie_DB_ONLY_ACCEPTED_COURSES,
  } Hie_DB_CountOnlyAcceptedCrss_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Hie_DB_BuildSubquery (char SubQuery[128],Hie_Level_t HieLvl,long HieCod);
unsigned Hie_DB_GetInsCtrDegAdminBy (MYSQL_RES **mysql_res,long UsrCod);

#endif
