// swad_nickname_database.h: Users' nicknames operations with database

#ifndef _SWAD_NCK_DB
#define _SWAD_NCK_DB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_nickname.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Nck_DB_UpdateNick (long UsrCod,const char *NewNickname);

long Nck_DB_GetUsrCodFromNickname (const char *NickWithoutArr);
void Nck_DB_GetNicknameFromUsrCod (long UsrCod,
                                   char NickWithoutArr[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1]);
unsigned Nck_DB_GetUsrNicknames (MYSQL_RES **mysql_res,long UsrCod);
bool Nck_DB_CheckIfNickMatchesAnyNick (const char *NickWithoutArr);
bool Nck_DB_CheckIfNickMatchesAnyUsrNick (long UsrCod,const char *NickWithoutArr);
bool Nck_DB_CheckIfNickMatchesAnyOtherUsrsNicks (long UsrCod,const char *NickWithoutArr);

void Nck_DB_RemoveNickname (long UsrCod,const char *Nickname);
void Nck_DB_RemoveUsrNicknames (long UsrCod);

#endif
