// swad_hierarchy.h: hierarchy (system, institution, center, degree, course)

#ifndef _SWAD_HIE
#define _SWAD_HIE
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_center.h"
#include "swad_country.h"
#include "swad_course.h"
#include "swad_degree.h"
#include "swad_institution.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Hie_SeePending (void);

void Hie_WriteMenuHierarchy (void);
void Hie_WriteHierarchyInBreadcrumb (void);
void Hie_WriteBigNameCtyInsCtrDegCrs (void);

void Hie_SetHierarchyFromUsrLastHierarchy (void);
void Hie_InitHierarchy (void);
void Hie_ResetHierarchy (void);

void Hie_GetAndWriteInsCtrDegAdminBy (long UsrCod,unsigned ColSpan);

bool Hie_CheckIfICanEdit (void);

void Hie_WriteStatusCell (Hie_Status_t Status,
			  const char *Class,const char *BgColor,
			  const char *Txt[Hie_NUM_STATUS_TXT]);
void Hie_WriteStatusCellEditable (bool ICanEdit,Hie_Status_t Status,
                                  Act_Action_t NextAction,long HieCod,
                                  const char *Txt[Hie_NUM_STATUS_TXT]);
Hie_Status_t Hie_GetParStatus (void);
void Hie_PutParOtherHieCod (void *HieCod);

Hie_Order_t Hie_GetParHieOrder (void);

void Hie_FreeList (Hie_Level_t Level);

void Hie_ResetMyHierarchy (void);
void Hie_FreeMyHierarchy (void);
void Hie_GetMyHierarchy (Hie_Level_t Level);
bool Hie_CheckIfIBelongTo (Hie_Level_t Level,long HieCod);

void Hie_FlushCacheUsrBelongsTo (Hie_Level_t Level);
bool Hie_CheckIfUsrBelongsTo (Hie_Level_t Level,long UsrCod,long HieCod,
                              bool CountOnlyAcceptedCourses);

//-------------------------------- Figures ------------------------------------
void Hie_GetAndShowHierarchyStats (void);

void Hie_FlushCachedNumNodesInHieLvl (Hie_Level_t LevelChildren,
		      		      Hie_Level_t LevelParent);
unsigned Hie_GetCachedNumNodesInHieLvl (Hie_Level_t LevelChildren,
		      		        Hie_Level_t LevelParent,long HieCod);
unsigned Hie_GetNumNodesInHieLvl (Hie_Level_t LevelChildren,
		      	          Hie_Level_t LevelParent,long HieCod);

unsigned Hie_GetCachedNumNodesInHieLvlWith (Hie_Level_t LevelChildren,
					    Hie_Level_t LevelParent,
					    Hie_Level_t HavingNodesOfLevel);

void Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_Level_t Level);
unsigned Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_Level_t Level,
						 struct Hie_Node *Node);
unsigned Hie_GetNumUsrsWhoClaimToBelongTo (Hie_Level_t Level,
					   struct Hie_Node *Node);

//--------------------------- My hierarchy ------------------------------------
void Hie_PutIconToSelectMyHierarchyInBreadcrumb (void);
void Hie_PutIconToSelectMyHierarchy (__attribute__((unused)) void *Args);

void Hie_SelOneNodeFromMyHierarchy (void);

#endif
