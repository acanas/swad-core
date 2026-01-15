// swad_group_database.h: types of groups and groups operations with database

#ifndef _SWAD_GRP_DB
#define _SWAD_GRP_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_exist.h"
#include "swad_group.h"
#include "swad_parameter.h"

/*****************************************************************************/
/**************************** Public prototypes ******************************/
/*****************************************************************************/

void Grp_DB_LockTables (void);

long Grp_DB_CreateGroupType (const struct GroupType *GrpTyp);
void Grp_DB_CreateGroup (long GrpTypCod,const struct Group *Grp);

void Grp_DB_GetGrpTypTitle (long GrpTypCod,char *Title,size_t TitleSize);
Exi_Exist_t Grp_DB_GetGroupTypeDataFromGrpTypCod (MYSQL_RES **mysql_res,long GrpTypCod);
Exi_Exist_t Grp_DB_GetSingleMultiple (MYSQL_RES **mysql_res,long GrpTypCod);
Exi_Exist_t Grp_DB_GetFileZones (MYSQL_RES **mysql_res,long GrpCod);
Exi_Exist_t Grp_DB_GetGroupDataByCod (MYSQL_RES **mysql_res,long GrpCod);

Exi_Exist_t Grp_DB_CheckIfGrpExists (long GrpCod);
Exi_Exist_t Grp_DB_CheckIfGrpExistsInCrs (long GrpCod,long HieCod);

Exi_Exist_t Grp_DB_CheckIfGrpTypNameExistsInCurrentCrs (const char *GrpTypName,long GrpTypCod);
Exi_Exist_t Grp_DB_CheckIfGrpNameExistsForGrpTyp (long GrpTypCod,const char *GrpName,long GrpCod);

unsigned Grp_DB_CountNumUsrsInGrp (Rol_Role_t Role,long GrpCod);
unsigned Grp_DB_CountNumUsrsInNoGrpsOfType (Rol_Role_t Role,long GrpTypCod);
long Grp_DB_GetRamdomStdFromGrp (long GrpCod);
unsigned Grp_DB_GetUsrsFromGrpExceptMe (MYSQL_RES **mysql_res,long GrpCod);
unsigned Grp_DB_GetTchsFromGrpExceptMe (MYSQL_RES **mysql_res,long GrpCod);

Usr_Belong_t Grp_DB_CheckIfIBelongToGrpsOfType (long GrpTypCod);
Usr_Belong_t Grp_DB_CheckIfIBelongToGrp (long GrpCod);
Usr_Share_t Grp_DB_CheckIfUsrSharesAnyOfMyGrpsInCurrentCrs (long UsrCod);

unsigned Grp_DB_GetGrpTypesWithGrpsInCrs (MYSQL_RES **mysql_res,long HieCod);
unsigned Grp_DB_GetAllGrpTypesInCrs (MYSQL_RES **mysql_res,long HieCod);

unsigned Grp_DB_GetGrpTypesInCurrentCrsToBeOpened (MYSQL_RES **mysql_res);

unsigned Grp_DB_CountNumGrpsInCurrentCrs (void);
unsigned Grp_DB_GetGrpsInCrs (MYSQL_RES **mysql_res,long HieCod);
unsigned Grp_DB_GetGrpsOfType (MYSQL_RES **mysql_res,long GrpTypCod);

unsigned Grp_DB_GetLstCodGrpsInAllCrssUsrBelongs (MYSQL_RES **mysql_res,long UsrCod);
unsigned Grp_DB_GetLstCodGrpsOfAnyTypeInCurrentCrsUsrBelongs (MYSQL_RES **mysql_res,
							      long UsrCod,
							      Grp_ClosedOpenGrps_t ClosedOpenGroups);
unsigned Grp_DB_GetLstCodGrpsOfATypeInCurrentCrsUsrBelongs (MYSQL_RES **mysql_res,
							    long UsrCod,long GrpTypCod);
unsigned Grp_DB_GetLstCodGrpsWithFileZonesInCurrentCrsIBelong (MYSQL_RES **mysql_res);
unsigned Grp_DB_GetNamesGrpsUsrBelongsTo (MYSQL_RES **mysql_res,
                                          long UsrCod,long GrpTypCod);

long Grp_DB_GetGrpTypeFromGrp (long GrpCod);

Exi_Exist_t Grp_DB_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                           long Cod,long GrpCod);
Exi_Exist_t Grp_DB_CheckIfAssociatedToGrps (Grp_WhichIsAssociatedToGrp_t WhichIsAssociatedToGrp,
					    long Cod);

void Grp_DB_ChangeOptionalMandatory (const struct GroupType *GrpTyp);
void Grp_DB_ChangeSingleMultiple (const struct GroupType *GrpTyp);
void Grp_DB_ChangeOpeningTime (const struct GroupType *GrpTyp);
void Grp_DB_ClearMustBeOpened (long GrpTypCod);
void Grp_DB_OpenGrpsOfType (long GrpTypCod);

void Grp_DB_OpenGrp (long GrpCod);
void Grp_DB_CloseGrp (long GrpCod);
void Grp_DB_EnableFileZonesGrp (long GrpCod);
void Grp_DB_DisableFileZonesGrp (long GrpCod);
void Grp_DB_ChangeGrpTypOfGrp (long GrpCod,long NewGrpTypCod);
void Grp_DB_ChangeRoomOfGrp (long GrpCod,long NewRooCod);
void Grp_DB_ResetRoomInGrps (long RooCod);
void Grp_DB_ChangeMaxStdsOfGrp (long GrpCod,unsigned NewMaxStds);

bool Grp_DB_CheckIfAvailableGrpTyp (long GrpTypCod);

void Grp_DB_RenameGrpTyp (const struct GroupType *GrpTyp);
void Grp_DB_RenameGrp (long GrpCod,
                       const char NewNameGrp[Grp_MAX_BYTES_GROUP_NAME + 1]);

void Grp_DB_AddUsrToGrp (long UsrCod,long GrpCod);

void Grp_DB_RemoveUsrFromGrp (long UsrCod,long GrpCod);
void Grp_DB_RemUsrFromAllGrpsInCrs (long UsrCod,long HieCod);
void Grp_DB_RemUsrFromAllGrps (long UsrCod);

void Grp_DB_RemoveUsrsFromGrpsOfCrs (long HieCod);
void Grp_DB_RemoveUsrsFromGrpsOfType (long GrpTypCod);
void Grp_DB_RemoveUsrsFromGrp (long GrpCod);

void Grp_DB_RemoveGrpTypesInCrs (long HieCod);
void Grp_DB_RemoveGrpType (long GrpTypCod);

void Grp_DB_RemoveGrpsInCrs (long HieCod);
void Grp_DB_RemoveGrpsOfType (long GrpTypCod);
void Grp_DB_RemoveGrp (long GrpCod);

#endif
