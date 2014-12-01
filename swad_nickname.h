// swad_nickname.h: Users' nicknames

#ifndef _SWAD_NCK
#define _SWAD_NCK
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#define Nck_MIN_LENGTH_NICKNAME_WITHOUT_ARROBA	  3
#define Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA	 16
#define Nck_MAX_BYTES_NICKNAME_WITH_ARROBA      127	// Used for variables that store the characters typed in a form

#define Nck_MAX_BYTES_LIST_NICKS		((Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+2)*Cfg_MAX_USRS_IN_LIST)

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

bool Nck_CheckIfNickWithArrobaIsValid (const char *NicknameWithArroba);

bool Nck_GetNicknameFromUsrCod (long UsrCod,char *Nickname);
long Nck_GetUsrCodFromNickname (const char *Nickname);
void Nck_ShowFormChangeUsrNickname (void);

void Nck_RemoveNick (void);
void Nck_UpdateNick (void);
void Nck_UpdateMyNick (const char *NewNickname);

#endif
