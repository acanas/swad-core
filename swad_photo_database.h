// swad_photo_database.h: Users' photos management, operations with database

#ifndef _SWAD_PHO_DB
#define _SWAD_PHO_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_cryptography.h"
#include "swad_photo.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Pho_DB_ClearPhotoName (long UsrCod);
void Pho_DB_UpdatePhotoName (long UsrCod,
                             const char UniqueNameEncrypted[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);

//-------------------------- Clicks without photo -----------------------------
void Pho_DB_InitMyClicksWithoutPhoto (void);
void Pho_DB_IncrMyClicksWithoutPhoto (void);

unsigned Pho_DB_GetMyClicksWithoutPhoto (MYSQL_RES **mysql_res);

void Pho_DB_RemoveUsrFromTableClicksWithoutPhoto (long UsrCod);

//------------------------ Statistics about degrees ---------------------------
void Pho_DB_UpdateDegStats (long DegCod,Usr_Sex_t Sex,
			    unsigned NumStds,unsigned NumStdsWithPhoto,
			    long TimeToComputeAvgPhotoInMicroseconds);

unsigned Pho_DB_QueryDegrees (MYSQL_RES **mysql_res,
                              Pho_HowOrderDegrees_t HowOrderDegrees);
long Pho_DB_GetADegWithStdsNotInTableOfComputedDegs (void);
long Pho_DB_GetDegWithAvgPhotoLeastRecentlyUpdated (void);
unsigned Pho_DB_GetMaxStdsPerDegree (MYSQL_RES **mysql_res);
unsigned Pho_DB_GetNumStdsInDegree (MYSQL_RES **mysql_res,long DegCod,Usr_Sex_t Sex);
unsigned Pho_DB_GetTimeAvgPhotoWasComputed (MYSQL_RES **mysql_res,long DegCod);
unsigned Pho_DB_GetTimeToComputeAvgPhoto (MYSQL_RES **mysql_res,long DegCod);

void Pho_DB_RemoveObsoleteStatDegrees (void);

#endif
