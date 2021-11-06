// swad_user_database.h: users, operations with database

#ifndef _SWAD_USR_DB
#define _SWAD_USR_DB
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases
#include <stdbool.h>		// For boolean type
#include <sys/types.h>		// For time_t

// #include "swad_action.h"
// #include "swad_constant.h"
// #include "swad_country.h"
#include "swad_cryptography.h"
// #include "swad_date.h"
// #include "swad_degree.h"
// #include "swad_icon.h"
// #include "swad_layout.h"
// #include "swad_menu.h"
// #include "swad_nickname.h"
// #include "swad_password.h"
// #include "swad_privacy_visibility_type.h"
// #include "swad_role_type.h"
// #include "swad_scope.h"
// #include "swad_search.h"
// #include "swad_string.h"
// #include "swad_theme.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Usr_DB_UpdateMyOffice (void);
void Usr_DB_UpdateMyOfficePhone (void);
void Usr_DB_UpdateMyLastWhatToSearch (void);

bool Usr_DB_ChkIfUsrCodExists (long UsrCod);
long Usr_DB_GetUsrCodFromEncryptedUsrCod (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);
bool Usr_DB_FindStrInUsrsNames (const char *Str);
void Usr_DB_BuildQueryToGetGstsLst (HieLvl_Level_t Scope,char **Query);
void Usr_DB_BuildQueryToGetUnorderedStdsCodesInDeg (long DegCod,char **Query);
unsigned Usr_DB_GetNumUsrsWhoChoseAnOption (const char *SubQuery);
unsigned Usr_DB_GetOldUsrs (MYSQL_RES **mysql_res,time_t SecondsWithoutAccess);

void Usr_DB_RemoveUsrLastData (long UsrCod);
void Usr_DB_RemoveUsrData (long UsrCod);

//------------------------------ Birthdays today ------------------------------
void Usr_DB_MarkMyBirthdayAsCongratulated (void);
bool Usr_DB_CheckIfMyBirthdayHasNotBeenCongratulated (void);
void Usr_DB_DeleteOldBirthdays (void);

#endif
