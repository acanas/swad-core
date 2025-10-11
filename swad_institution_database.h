// swad_institution_database.h: institutions operations with database

#ifndef _SWAD_INS_DB
#define _SWAD_INS_DB
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

#include "swad_constant.h"
#include "swad_hierarchy_type.h"
#include "swad_map.h"
#include "swad_role_type.h"
#include "swad_search.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Ins_DB_CreateInstitution (const struct Hie_Node *Ins,Hie_Status_t Status);
void Ins_DB_UpdateInsCty (long HieCods[Hie_NUM_LEVELS]);
void Ins_DB_UpdateInsStatus (long HieCod,Hie_Status_t Status);
void Ins_DB_UpdateInsName (long HieCod,const char *FldName,const char *NewName);
void Ins_DB_UpdateInsWWW (long HieCod,const char NewWWW[WWW_MAX_BYTES_WWW + 1]);

Exi_Exist_t Ins_DB_GetInsDataByCod (MYSQL_RES **mysql_res,long HieCod);
void Ins_DB_GetInsShrtName (long HieCod,char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1]);
Exi_Exist_t Ins_DB_GetInsShrtNameAndCty (MYSQL_RES **mysql_res,long HieCod);
Exi_Exist_t Ins_DB_CheckIfInsNameExistsInCty (const char *FldName,
					      const char *Name,
					      long Cod,
					      long PrtCod,
					      __attribute__((unused)) unsigned Year);

unsigned Ins_DB_GetAllInsWithPendingCtr (MYSQL_RES **mysql_res);
unsigned Ins_DB_GetInsWithPendingCtrsAdminByMe (MYSQL_RES **mysql_res);

unsigned Ins_DB_GetInssInCtyOrderedByShrtName (MYSQL_RES **mysql_res,long HieCod);
unsigned Ins_DB_GetInssInCtyOrderedByFullName (MYSQL_RES **mysql_res,long HieCod);
unsigned Ins_DB_GetFullListOfInssInCty (MYSQL_RES **mysql_res,long HieCod);

unsigned Ins_DB_GetInssOrderedByNumCtrs (MYSQL_RES **mysql_res,Hie_Level_t HieLvl);
unsigned Ins_DB_GetInssOrderedByNumDegs (MYSQL_RES **mysql_res,Hie_Level_t HieLvl);
unsigned Ins_DB_GetInssOrderedByNumCrss (MYSQL_RES **mysql_res,Hie_Level_t HieLvl);
unsigned Ins_DB_GetInssOrderedByNumUsrsInCrss (MYSQL_RES **mysql_res,
					       Hie_Level_t HieLvl);
unsigned Ins_DB_GetInssOrderedByNumUsrsWhoClaimToBelongToThem (MYSQL_RES **mysql_res,
							       Hie_Level_t HieLvl);

unsigned Ins_DB_SearchInss (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery);

unsigned Ins_DB_GetNumInssInSys (__attribute__((unused)) long HieCod);
unsigned Ins_DB_GetNumInssInCty (long HieCod);

unsigned Ins_DB_GetNumInssWithCtrs (Hie_Level_t HieLvl,long HieCod);
unsigned Ins_DB_GetNumInssWithDegs (Hie_Level_t HieLvl,long HieCod);
unsigned Ins_DB_GetNumInssWithCrss (Hie_Level_t HieLvl,long HieCod);
unsigned Ins_DB_GetNumInnsWithUsrs (Hie_Level_t HieLvl,long HieCod,Rol_Role_t Role);

unsigned Ins_DB_GetMyInss (MYSQL_RES **mysql_res,long HieCod);
Usr_Belong_t Ins_DB_CheckIfUsrBelongsToIns (long UsrCod,long HieCod,
					    Hie_DB_CountOnlyAcceptedCrss_t CountOnlyAcceptedCourses);
unsigned Ins_DB_GetNumUsrsWhoClaimToBelongToIns (long HieCod);

void Ins_DB_RemoveInstitution (long HieCod);

#endif
