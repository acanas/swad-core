// swad_ID_database.h: Users' IDs operations with database

#ifndef _SWAD_ID_DB
#define _SWAD_ID_DB
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
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ID_DB_InsertANewUsrID (long UsrCod,
		            const char ID[ID_MAX_BYTES_USR_ID + 1],
		            ID_Confirmed_t Confirmed);
void ID_DB_ConfirmUsrID (long UsrCod,const char ID[ID_MAX_BYTES_USR_ID + 1]);

unsigned ID_DB_GetIDsFromUsrCod (MYSQL_RES **mysql_res,long UsrCod);
bool ID_DB_CheckIfConfirmed (long UsrCod,const char ID[ID_MAX_BYTES_USR_ID + 1]);
Exi_Exist_t ID_DB_FindStrInUsrsIDs (const char *Str);
unsigned ID_DB_GetUsrCodsFromUsrID (MYSQL_RES **mysql_res,
                                    const struct Usr_Data *UsrDat,
                                    const char *EncryptedPassword,
                                    bool OnlyConfirmedIDs);

void ID_DB_RemoveUsrID (long UsrCod,const char ID[ID_MAX_BYTES_USR_ID + 1]);
void ID_DB_RemoveUsrIDs (long UsrCod);

#endif
