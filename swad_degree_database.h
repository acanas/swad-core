// swad_degree_database.h: degrees operations with database

#ifndef _SWAD_DEG_DB
#define _SWAD_DEG_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

// #include "swad_action.h"
// #include "swad_constant.h"
#include "swad_degree_type.h"
// #include "swad_hierarchy_level.h"
// #include "swad_role_type.h"
// #include "swad_string.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Deg_DB_CreateDegreeType (const char DegTypName[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1]);
void Deg_DB_CreateDegree (struct Deg_Degree *Deg,unsigned Status);

unsigned Deg_DB_GetDegreeTypes (MYSQL_RES **mysql_res,
                                HieLvl_Level_t Scope,DegTyp_Order_t Order);
void Deg_DB_GetDegTypeNameByCod (struct DegreeType *DegTyp);
unsigned Deg_DB_GetDataOfDegreeByCod (MYSQL_RES **mysql_res,long DegCod);
long Deg_DB_GetCtrCodOfDegreeByCod (long DegCod);
void Deg_DB_GetShortNameOfDegreeByCod (struct Deg_Degree *Deg);
bool Deg_DB_CheckIfDegreeTypeNameExists (const char *DegTypName,long DegTypCod);
unsigned Deg_DB_GetNumDegsOfType (long DegTypCod);
unsigned Deg_DB_GetDegsOfType (MYSQL_RES **mysql_res,long DegTypCod);
unsigned Deg_DB_GetDegsOfCurrentCtr (MYSQL_RES **mysql_res);
unsigned Deg_DB_GetDegsWithPendingCrss (MYSQL_RES **mysql_res);

void Deg_DB_UpdateDegTypName (long DegTypCod,const char NewNameDegTyp[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1]);
void Deg_DB_UpdateDegCtr (long DegCod,long CtrCod);

void Deg_DB_RemoveDegTyp (long DegTypCod);

#endif
