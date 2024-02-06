// swad_account_database.h: user's account operations with database

#ifndef _SWAD_ACC_DB
#define _SWAD_ACC_DB
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_ID.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

unsigned Acc_DB_GetUsrsWithID (MYSQL_RES **mysql_res,
                               const char ID[ID_MAX_BYTES_USR_ID + 1]);

bool Acc_DB_CheckIfNicknameAlreadyExists (const char NewNickWithoutArr[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1]);
bool Acc_DB_CheckIfEmailAlreadyExists (const char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

long Acc_DB_CreateNewUsr (const struct Usr_Data *UsrDat);
void Acc_DB_UpdateUsrData (const struct Usr_Data *UsrDat);
void Acc_DB_UpdateMyInstitutionCenterDepartment (void);

#endif
