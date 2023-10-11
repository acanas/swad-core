// swad_department_database.h: departments operations with database

#ifndef _SWAD_DPT_DB
#define _SWAD_DPT_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_constant.h"
#include "swad_department.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Dpt_DB_CreateDepartment (const struct Dpt_Department *Dpt);

unsigned Dpt_DB_GetListDepartments (MYSQL_RES **mysql_res,
                                    long InsCod,Dpt_Order_t SelectedOrder);
unsigned Dpt_DB_GetDepartmentDataByCod (MYSQL_RES **mysql_res,long DptCod);
bool Dpt_DB_CheckIfDepartmentNameExists (const char *FldName,const char *Name,long Cod,
				         __attribute__((unused)) long PrtCod,
				         __attribute__((unused)) unsigned Year);
unsigned Dpt_DB_GetNumDepartmentsInInstitution (long InsCod);
unsigned Dpt_DB_GetNumTchsCurrentInsInDepartment (long DptCod);

void Dpt_DB_UpdateDptIns (long DptCod,long NewInsCod);
void Dpt_DB_UpdateDptName (long DptCod,
			   const char *FldName,const char *NewDptName);
void Dpt_DB_UpdateDptWWW (long DptCod,char NewWWW[Cns_MAX_BYTES_WWW + 1]);

void Dpt_DB_RemoveDepartment (long DptCod);

#endif
