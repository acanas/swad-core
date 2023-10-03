// swad_center_database.h: centers operations with database

#ifndef _SWAD_CTR_DB
#define _SWAD_CTR_DB
/*
    SWAD (Shared Workspace At a Distance),
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

#include "swad_center.h"
#include "swad_constant.h"
#include "swad_media.h"
#include "swad_search.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

long Ctr_DB_CreateCenter (const struct Hie_Node *Ctr,Hie_Status_t Status);

unsigned Ctr_DB_GetListOfCtrsInCurrentIns (MYSQL_RES **mysql_res);
unsigned Ctr_DB_GetListOfCtrsFull (MYSQL_RES **mysql_res,long InsCod);
unsigned Ctr_DB_GetListOfCtrsFullWithNumUsrs (MYSQL_RES **mysql_res,
                                   long InsCod,Hie_Order_t SelectedOrder);
unsigned Ctr_DB_GetCtrsWithPendingDegs (MYSQL_RES **mysql_res);
unsigned Ctr_DB_GetCenterDataByCod (MYSQL_RES **mysql_res,long CtrCod);
unsigned Ctr_DB_GetCoordByCod (MYSQL_RES **mysql_res,long CtrCod);
long Ctr_DB_GetInsCodOfCenterByCod (long CtrCod);
void Ctr_DB_GetShortNameOfCenterByCod (long CtrCod,char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1]);
unsigned Ctr_DB_GetPhotoAttribution (MYSQL_RES **mysql_res,long CtrCod);
bool Ctr_DB_CheckIfCtrNameExistsInIns (const char *FldName,const char *Name,
				       long CtrCod,long InsCod);

unsigned Ctr_DB_SearchCtrs (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery);

unsigned Ctr_DB_GetNumCtrsInSys (__attribute__((unused)) long SysCod);
unsigned Ctr_DB_GetNumCtrsInCty (long CtyCod);
unsigned Ctr_DB_GetNumCtrsInIns (long InsCod);
unsigned Ctr_DB_GetNumCtrsInPlc (long PlcCod);
unsigned Ctr_DB_GetNumCtrsWithMap (void);
unsigned Ctr_DB_GetNumCtrsWithMapInCty (long CtyCod);
unsigned Ctr_DB_GetNumCtrsWithMapInIns (long InsCod);
unsigned Ctr_DB_GetNumCtrsWithDegs (Hie_Level_t Level,long Cod);
unsigned Ctr_DB_GetNumCtrsWithCrss (Hie_Level_t Level,long Cod);
unsigned Ctr_DB_GetNumCtrsWithUsrs (Rol_Role_t Role,
                                    Hie_Level_t Level,long Cod);

void Ctr_DB_UpdateCtrIns (long CtrCod,long NewInsCod);
void Ctr_DB_UpdateCtrPlc (long CtrCod,long NewPlcCod);
void Ctr_DB_UpdateCtrName (long CtrCod,const char *FldName,const char *NewCtrName);
void Ctr_DB_UpdateCtrWWW (long CtrCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1]);
void Ctr_DB_UpdateCtrPhotoAttribution (long CtrCod,const char NewPhotoAttribution[Med_MAX_BYTES_ATTRIBUTION + 1]);
void Ctr_DB_UpdateCtrCoordinate (long CtrCod,
				 const char *CoordField,double NewCoord);
void Ctr_DB_UpdateCtrStatus (long CtrCod,Hie_Status_t NewStatus);


bool Ctr_DB_CheckIfMapIsAvailableInIns (long InsCod);
void Ctr_DB_GetAvgCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom);
void Ctr_DB_GetAvgCoordAndZoomInCurrentIns (struct Map_Coordinates *Coord,unsigned *Zoom);
unsigned Ctr_DB_GetCtrsWithCoords (MYSQL_RES **mysql_res);
unsigned Ctr_DB_GetCtrsWithCoordsInCurrentIns (MYSQL_RES **mysql_res);

unsigned Ctr_DB_GetCtrsFromUsr (MYSQL_RES **mysql_res,long UsrCod,long HieCod);
bool Ctr_DB_CheckIfUsrBelongsToCtr (long UsrCod,long HieCod,
				    bool CountOnlyAcceptedCourses);
unsigned Ctr_DB_GetNumUsrsWhoClaimToBelongToCtr (long CtrCod);

void Ctr_DB_RemoveCenter (long CtrCod);

#endif
