// swad_group_database.h: types of groups and groups operations with database

#ifndef _SWAD_GRP_DB
#define _SWAD_GRP_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include "swad_group.h"
// #include "swad_info.h"
// #include "swad_room.h"
// #include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Grp_DB_LockTables (void);
void Grp_DB_UnlockTables (void);

long Grp_DB_CreateGroupType (const struct GroupType *GrpTyp);
void Grp_DB_CreateGroup (const struct Grp_Groups *Grps);

unsigned Grp_DB_GetDataOfGroupTypeByCod (MYSQL_RES **mysql_res,long GrpTypCod);
unsigned Grp_DB_GetMultipleEnrolmentOfAGroupType (MYSQL_RES **mysql_res,long GrpTypCod);
unsigned Grp_DB_GetDataOfGroupByCod (MYSQL_RES **mysql_res,long GrpCod);

bool Grp_DB_CheckIfGrpExists (long GrpCod);
bool Grp_DB_CheckIfGrpBelongsToCrs (long GrpCod,long CrsCod);

bool Grp_DB_CheckIfGrpTypNameExistsInCurrentCrs (const char *GrpTypName,long GrpTypCod);
bool Grp_DB_CheckIfGrpNameExistsForGrpTyp (long GrpTypCod,const char *GrpName,long GrpCod);

unsigned Grp_DB_CountNumUsrsInGrp (Rol_Role_t Role,long GrpCod);
unsigned Grp_DB_CountNumUsrsInNoGrpsOfType (Rol_Role_t Role,long GrpTypCod);

bool Grp_DB_CheckIfIBelongToGrpsOfType (long GrpTypCod);
bool Grp_DB_CheckIfIBelongToGrp (long GrpCod);

unsigned Grp_DB_GetGrpTypesWithGrpsInCurrentCrs (MYSQL_RES **mysql_res);
unsigned Grp_DB_GetAllGrpTypesInCurrentCrs (MYSQL_RES **mysql_res);

unsigned Grp_DB_GetGrpTypesInCurrentCrsToBeOpened (MYSQL_RES **mysql_res);

unsigned Grp_DB_CountNumGrpsInCurrentCrs (void);
unsigned Grp_DB_CountNumGrpsInThisCrsOfType (long GrpTypCod);
unsigned Grp_DB_GetGrpsOfType (MYSQL_RES **mysql_res,long GrpTypCod);

long Grp_DB_GetGrpTypeFromGrp (long GrpCod);

bool Grp_DB_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                    long Cod,long GrpCod);
bool Grp_DB_CheckIfAssociatedToGrps (const char *Table,const char *Field,long Cod);

void Grp_DB_OpenGrpsOfType (long GrpTypCod);
void Grp_DB_ClearMustBeOpened (long GrpTypCod);

void Grp_DB_RenameGrpTyp (long GrpTypCod,
                          const char NewNameGrpTyp[Grp_MAX_BYTES_GROUP_TYPE_NAME + 1]);
void Grp_DB_RenameGrp (long GrpCod,
                       const char NewNameGrp[Grp_MAX_BYTES_GROUP_NAME + 1]);

void Grp_DB_AddUsrToGrp (long UsrCod,long GrpCod);

void Grp_DB_RemoveUsrFromGrp (long UsrCod,long GrpCod);
void Grp_DB_RemUsrFromAllGrpsInCrs (long UsrCod,long CrsCod);
void Grp_DB_RemUsrFromAllGrps (long UsrCod);

void Grp_DB_RemoveUsrsInGrpsOfCrs (long CrsCod);
void Grp_DB_RemoveGrpsInCrs (long CrsCod);
void Grp_DB_RemoveGrpTypesInCrs (long CrsCod);

#endif
