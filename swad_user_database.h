// swad_user_database.h: users, operations with database

#ifndef _SWAD_USR_DB
#define _SWAD_USR_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases
#include <sys/types.h>		// For time_t

#include "swad_cryptography.h"
#include "swad_search.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

//------------------------------- User's data ---------------------------------
void Usr_DB_UpdateMyOffice (void);
void Usr_DB_UpdateMyOfficePhone (void);

Exi_Exist_t Usr_DB_ChkIfUsrCodExists (long UsrCod);
void Usr_DB_GetEncryptedUsrCodFromUsrCod (long UsrCod,
					  char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);
long Usr_DB_GetUsrCodFromEncryptedUsrCod (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);
long Usr_DB_GetUsrCodFromNick (const char *NickWithoutArr);
long Usr_DB_GetUsrCodFromNickPwd (const char *NickWithoutArr,const char *Password);
long Usr_DB_GetUsrCodFromEmail (const char *Email);
long Usr_DB_GetUsrCodFromEmailPwd (const char *Email,const char *Password);
long Usr_DB_GetUsrCodFromID (const char *ID);
long Usr_DB_GetUsrCodFromIDPwd (const char *ID,const char *Password);
Exi_Exist_t Usr_DB_GetUsrDataFromUsrCod (MYSQL_RES **mysql_res,long UsrCod,
                                         Usr_GetPrefs_t GetPrefs);
Exi_Exist_t Usr_DB_GetSomeUsrDataFromUsrCod (MYSQL_RES **mysql_res,long UsrCod);
void Usr_DB_GetUsrName (long UsrCod,char *Title,size_t TitleSize);
Exi_Exist_t Usr_DB_FindStrInUsrsNames (const char *Str);
void Usr_DB_BuildQueryToGetUsrsLst (Hie_Level_t HieLvl,Rol_Role_t Role,
				    char **Query);
void Usr_DB_BuildQueryToGetUsrsLstCrs (char **Query,Rol_Role_t Role);
void Usr_DB_BuildQueryToGetAdmsLst (Hie_Level_t HieLvl,char **Query);
void Usr_DB_BuildQueryToGetGstsLst (Hie_Level_t HieLvl,char **Query);
void Usr_DB_BuildQueryToSearchListUsrs (Hie_Level_t HieLvl,Rol_Role_t Role,
					char **Query);
void Usr_DB_BuildQueryToGetUnorderedStdsCodesInDeg (long DegCod,char **Query);
unsigned Usr_DB_GetNumUsrsWhoChoseAnOption (Hie_Level_t HieLvl,const char *SubQuery);
unsigned Usr_DB_GetOldUsrs (MYSQL_RES **mysql_res,time_t SecondsWithoutAccess);
void Usr_DB_CreateTmpTableAndSearchCandidateUsrs (const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1]);
void Usr_DB_DropTmpTableWithCandidateUsrs (void);

void Usr_DB_RemoveUsrData (long UsrCod);

//------------------------------ User's last data -----------------------------
void Usr_DB_InsertMyLastData (void);
void Usr_DB_UpdateMyLastData (void);
void Usr_DB_UpdateMyLastWhatToSearch (Sch_WhatToSearch_t WhatToSearch);

Exi_Exist_t Usr_DB_CheckMyLastData (void);
Exi_Exist_t Usr_DB_GetMyLastData (MYSQL_RES **mysql_res);

void Usr_DB_RemoveUsrLastData (long UsrCod);

//------------------------------ Birthdays today ------------------------------
void Usr_DB_MarkMyBirthdayAsCongratulated (void);
bool Usr_DB_CheckIfMyBirthdayHasNotBeenCongratulated (void);
void Usr_DB_DeleteOldBirthdays (void);

//----------------------------- User clipboard --------------------------------
void Usr_DB_CopyToClipboard (long OthUsrCod);
unsigned Usr_DB_GetNumUsrsInMyClipboard (void);
void Usr_DB_BuildQueryToGetUsrsInMyClipboard (char **Query);
void Usr_DB_RemoveMyClipboard (void);
void Usr_DB_RemoveExpiredClipboards (void);


#endif
