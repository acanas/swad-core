// swad_center_database.h: centers operations with database

#ifndef _SWAD_CTR_DB
#define _SWAD_CTR_DB
/*
    SWAD (Shared Workspace At a Distance),
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
#include "swad_center.h"
#include "swad_constant.h"
// #include "swad_degree.h"
// #include "swad_map.h"
// #include "swad_role_type.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

long Ctr_DB_CreateCenter (const struct Ctr_Center *Ctr,Ctr_Status_t Status);

unsigned Ctr_DB_GetListOfCtrsInCurrentIns (MYSQL_RES **mysql_res);
unsigned Ctr_DB_GetListOfCtrsFull (MYSQL_RES **mysql_res,long InsCod);
unsigned Ctr_DB_GetListOfCtrsFullWithNumUsrs (MYSQL_RES **mysql_res,
                                   long InsCod,Ctr_Order_t SelectedOrder);
unsigned Ctr_DB_GetCtrsWithPendingDegs (MYSQL_RES **mysql_res);
unsigned Ctr_DB_GetDataOfCenterByCod (MYSQL_RES **mysql_res,long CtrCod);
long Ctr_DB_GetInsCodOfCenterByCod (long CtrCod);
void Ctr_DB_GetShortNameOfCenterByCod (long CtrCod,char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1]);
bool Ctr_DB_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,
				       long CtrCod,long InsCod);
unsigned Ctr_DB_GetNumCtrsInCty (long CtyCod);
unsigned Ctr_DB_GetNumCtrsInIns (long InsCod);
unsigned Ctr_DB_GetNumCtrsInPlc (long PlcCod);
unsigned Ctr_DB_GetNumCtrsWithMap (void);
unsigned Ctr_DB_GetNumCtrsWithMapInCty (long CtyCod);
unsigned Ctr_DB_GetNumCtrsWithMapInIns (long InsCod);
unsigned Ctr_DB_GetNumCtrsWithDegs (const char *SubQuery);
unsigned Ctr_DB_GetNumCtrsWithCrss (const char *SubQuery);
unsigned Ctr_DB_GetNumCtrsWithUsrs (Rol_Role_t Role,const char *SubQuery);

void Ctr_DB_UpdateCtrPlc (long CtrCod,long NewPlcCod);
void Ctr_DB_UpdateCtrName (long CtrCod,const char *FieldName,const char *NewCtrName);
void Ctr_DB_UpdateCtrWWW (long CtrCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1]);
void Ctr_DB_UpdateCtrStatus (long CtrCod,Ctr_Status_t Status);

void Ctr_DB_RemoveCenter (long CtrCod);

#endif
