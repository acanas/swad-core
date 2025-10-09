// swad_institution.h: institutions

#ifndef _SWAD_INS
#define _SWAD_INS
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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

#include "swad_center.h"
#include "swad_constant.h"
#include "swad_figure_cache.h"
#include "swad_name.h"
#include "swad_role_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void);

void Ins_DrawInstitutionLogoWithLink (struct Hie_Node *Ins,
				      Pho_ShowPhotos_t ShowPhotos,
				      const char *IconClass);
void Ins_DrawInstitLogoAndNameWithLink (struct Hie_Node *Ins,Act_Action_t Action,
                                        const char *IconClass);

void Ins_ShowInssOfCurrentCty (void);
void Ins_EditInstitutions (void);
void Ins_GetBasicListOfInstitutions (long CtyCod);
void Ins_GetFullListOfInstitutions (long CtyCod);

void Ins_WriteInstitutionNameAndCty (long HieCod);
Err_SuccessOrError_t Ins_GetInstitDataByCod (struct Hie_Node *Node);
void Ins_FlushCacheFullNameAndCtyOfInstitution (void);
void Ins_GetShrtNameAndCtyOfInstitution (struct Hie_Node *Ins,
                                         char CtyName[Nam_MAX_BYTES_FULL_NAME + 1]);

void Ins_WriteSelectorOfInstitution (void);

void Ins_RemoveInstitution (void);
void Ins_RenameInsShort (void);
void Ins_RenameInsFull (void);
void Ins_RenameInstitution (struct Hie_Node *Ins,Nam_ShrtOrFullName_t ShrtOrFull);
void Ins_ChangeInsWWW (void);
void Ins_ChangeInsStatus (void);
void Ins_ContEditAfterChgIns (void);

void Ins_ReceiveReqIns (void);
void Ins_ReceiveNewIns (void);

unsigned Ins_GetCachedNumInssWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role);

void Ins_ListInssFound (MYSQL_RES **mysql_res,unsigned NumInss);

void Ins_PutParInsCod (void *InsCod);

//-------------------------------- Figures ------------------------------------
void Ins_GetAndShowInstitutionsStats (Hie_Level_t HieLvl);

#endif
