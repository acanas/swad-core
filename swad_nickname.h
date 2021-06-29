// swad_nickname.h: Users' nicknames

#ifndef _SWAD_NCK
#define _SWAD_NCK
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

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

#define Nck_MIN_CHARS_NICK_WITHOUT_ARROBA	 3
#define Nck_MIN_BYTES_NICK_WITHOUT_ARROBA	Nck_MIN_CHARS_NICK_WITHOUT_ARROBA

#define Nck_MAX_CHARS_NICK_WITHOUT_ARROBA	16
#define Nck_MAX_BYTES_NICK_WITHOUT_ARROBA	Nck_MAX_CHARS_NICK_WITHOUT_ARROBA

#define Nck_MAX_BYTES_NICK_FROM_FORM	(128 - 1)	// For variables that store characters typed in a form

#define Nck_MAX_BYTES_LIST_NICKS		((Nck_MAX_BYTES_NICK_FROM_FORM + 2) * Cfg_MAX_USRS_IN_LIST)

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

bool Nck_CheckIfNickWithArrIsValid (const char *NickWithArr);

void Nck_DB_GetNicknameFromUsrCod (long UsrCod,
                                   char Nickname[Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1]);
long Nck_GetUsrCodFromNickname (const char *Nickname);

void Nck_ShowFormChangeMyNickname (bool IMustFillNickname);
void Nck_ShowFormChangeOtherUsrNickname (void);

void Nck_RemoveMyNick (void);
void Nck_RemoveOtherUsrNick (void);
void Nck_DB_RemoveUsrNicknames (long UsrCod);

void Nck_UpdateMyNick (void);
void Nck_UpdateOtherUsrNick (void);

void Nck_DB_UpdateNick (long UsrCod,const char *NewNickname);

#endif
