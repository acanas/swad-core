// swad_center.h: centers

#ifndef _SWAD_CTR
#define _SWAD_CTR
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

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_degree.h"
#include "swad_figure_cache.h"
#include "swad_map.h"
#include "swad_name.h"
#include "swad_role_type.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void);

void Ctr_DrawCenterLogoAndNameWithLink (struct Hie_Node *Ctr,Act_Action_t Action,
                                        const char *IconClass);

void Ctr_ShowCtrsOfCurrentIns (void);
void Ctr_EditCenters (void);

void Ctr_GetBasicListOfCentersInIns (long HieCod);
Err_SuccessOrError_t Ctr_GetCenterDataByCod (struct Hie_Node *Node);
void Ctr_GetCoordByCod (long HieCod,struct Map_Coordinates *Coord);
void Ctr_WriteSelectorOfCenter (void);
void Ctr_RemoveCenter (void);
void Ctr_ChangeCtrPlc (void);
void Ctr_RenameCenterShrt (void);
void Ctr_RenameCenterFull (void);
void Ctr_RenameCenter (struct Hie_Node *Ctr,Nam_ShrtOrFullName_t ShrtOrFull);
void Ctr_ChangeCtrWWW (void);
void Ctr_ChangeCtrStatus (void);
void Ctr_ContEditAfterChgCtr (void);

void Ctr_ReceiveReqCtr (void);
void Ctr_ReceiveNewCtr (void);

unsigned Ctr_GetCachedNumCtrsWithMapInSys (void);
unsigned Ctr_GetCachedNumCtrsWithMapInCty (long HieCod);
unsigned Ctr_GetCachedNumCtrsWithMapInIns (long HieCod);

unsigned Ctr_GetCachedNumCtrsWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role);

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs);

#endif
