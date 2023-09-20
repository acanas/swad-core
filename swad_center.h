// swad_center.h: centers

#ifndef _SWAD_CTR
#define _SWAD_CTR
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

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_degree.h"
#include "swad_figure_cache.h"
#include "swad_map.h"
#include "swad_role_type.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Ctr_MAX_CENTERS_PER_USR	 10 // Used in list of my centers

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void);

void Ctr_DrawCenterLogoAndNameWithLink (struct Hie_Node *Ctr,Act_Action_t Action,
                                        const char *ClassLogo);

void Ctr_ShowCtrsOfCurrentIns (void);
void Ctr_EditCenters (void);

void Ctr_GetBasicListOfCenters (long InsCod);
void Ctr_GetFullListOfCenters (long InsCod,Hie_Order_t SelectedOrder);
bool Ctr_GetCenterDataByCod (struct Hie_Node *Ctr);
void Ctr_GetCoordByCod (long CtrCod,struct Map_Coordinates *Coord);
void Ctr_WriteSelectorOfCenter (void);
void Ctr_RemoveCenter (void);
void Ctr_ChangeCtrPlc (void);
void Ctr_RenameCenterShort (void);
void Ctr_RenameCenterFull (void);
void Ctr_RenameCenter (struct Hie_Node *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName);
void Ctr_ChangeCtrWWW (void);
void Ctr_ChangeCtrStatus (void);
void Ctr_ContEditAfterChgCtr (void);

void Ctr_ReceiveFormReqCtr (void);
void Ctr_ReceiveFormNewCtr (void);

void Ctr_FlushCacheNumCtrsInCty (void);
unsigned Ctr_GetNumCtrsInCty (long CtyCod);

void Ctr_FlushCacheNumCtrsInIns (void);
unsigned Ctr_GetNumCtrsInIns (long InsCod);

unsigned Ctr_GetCachedNumCtrsWithMapInSys (void);
unsigned Ctr_GetCachedNumCtrsWithMapInCty (long CtyCod);
unsigned Ctr_GetCachedNumCtrsWithMapInIns (long InsCod);

unsigned Ctr_GetCachedNumCtrsWithDegs (void);
unsigned Ctr_GetCachedNumCtrsWithCrss (void);
unsigned Ctr_GetCachedNumCtrsWithUsrs (Rol_Role_t Role);

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs);

void Ctr_GetMyCenters (void);
void Ctr_FreeMyCenters (void);
bool Ctr_CheckIfIBelongToCtr (long CtrCod);

void Ctr_FlushCacheNumUsrsWhoClaimToBelongToCtr (void);
unsigned Ctr_GetNumUsrsWhoClaimToBelongToCtr (struct Hie_Node *Ctr);
unsigned Ctr_GetCachedNumUsrsWhoClaimToBelongToCtr (struct Hie_Node *Ctr);

#endif
