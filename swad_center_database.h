// swad_center_database.h: centers operations with database

#ifndef _SWAD_CTR_DB
#define _SWAD_CTR_DB
/*
    SWAD (Shared Workspace At a Distance),
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
Exi_Exist_t Ctr_DB_GetCenterDataByCod (MYSQL_RES **mysql_res,long HieCod);
Exi_Exist_t Ctr_DB_GetCoordByCod (MYSQL_RES **mysql_res,long HieCod);
long Ctr_DB_GetInsCodOfCenterByCod (long HieCod);
void Ctr_DB_GetCtrShrtName (long HieCod,char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1]);
unsigned Ctr_DB_GetPhotoAttribution (MYSQL_RES **mysql_res,long CtrCod);
Exi_Exist_t Ctr_DB_CheckIfCtrNameExistsInIns (const char *FldName,const char *Name,
					      long Cod,long PrtCod,
					      __attribute__((unused)) unsigned Year);

unsigned Ctr_DB_SearchCtrs (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery);

unsigned Ctr_DB_GetNumCtrsInSys (__attribute__((unused)) long HieCod);
unsigned Ctr_DB_GetNumCtrsInCty (long HieCod);
unsigned Ctr_DB_GetNumCtrsInIns (long HieCod);
unsigned Ctr_DB_GetNumCtrsInPlc (long PlcCod);
unsigned Ctr_DB_GetNumCtrsWithMap (void);
unsigned Ctr_DB_GetNumCtrsWithMapInCty (long HieCod);
unsigned Ctr_DB_GetNumCtrsWithMapInIns (long HieCod);
unsigned Ctr_DB_GetNumCtrsWithDegs (Hie_Level_t HieLvl,long HieCod);
unsigned Ctr_DB_GetNumCtrsWithCrss (Hie_Level_t HieLvl,long HieCod);
unsigned Ctr_DB_GetNumCtrsWithUsrs (Hie_Level_t HieLvl,long HieCod,Rol_Role_t Role);

void Ctr_DB_UpdateCtrIns (long HieCod,long NewInsCod);
void Ctr_DB_UpdateCtrPlc (long HieCod,long NewPlcCod);
void Ctr_DB_UpdateCtrName (long HieCod,
			   const char *FldName,const char *NewCtrName);
void Ctr_DB_UpdateCtrWWW (long HieCod,const char NewWWW[WWW_MAX_BYTES_WWW + 1]);
void Ctr_DB_UpdateCtrPhotoAttribution (long HieCod,const char NewPhotoAttribution[Med_MAX_BYTES_ATTRIBUTION + 1]);
void Ctr_DB_UpdateCtrCoordinate (long HieCod,
				 const char *CoordField,double NewCoord);
void Ctr_DB_UpdateCtrStatus (long HieCod,Hie_Status_t NewStatus);


Exi_Exist_t Ctr_DB_CheckIfMapExistsInIns (long HieCod);
void Ctr_DB_GetAvgCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom);
void Ctr_DB_GetAvgCoordAndZoomInCurrentIns (struct Map_Coordinates *Coord,unsigned *Zoom);
unsigned Ctr_DB_GetCtrsWithCoords (MYSQL_RES **mysql_res);
unsigned Ctr_DB_GetCtrsWithCoordsInCurrentIns (MYSQL_RES **mysql_res);

unsigned Ctr_DB_GetMyCtrs (MYSQL_RES **mysql_res,long PrtCod);
Usr_Belong_t Ctr_DB_CheckIfUsrBelongsToCtr (long UsrCod,long HieCod,
					    bool CountOnlyAcceptedCourses);
unsigned Ctr_DB_GetNumUsrsWhoClaimToBelongToCtr (long HieCod);

void Ctr_DB_RemoveCenter (long HieCod);

#endif
