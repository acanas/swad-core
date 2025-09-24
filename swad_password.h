// swad_password.h: Users' passwords

#ifndef _SWAD_PWD
#define _SWAD_PWD
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
/************************* Public types and constants ************************/
/*****************************************************************************/

#define Pwd_MIN_CHARS_PLAIN_PASSWORD	  8
#define Pwd_MIN_BYTES_PLAIN_PASSWORD	Pwd_MIN_CHARS_PLAIN_PASSWORD

#define Pwd_MAX_CHARS_PLAIN_PASSWORD	(256 - 1)
#define Pwd_MAX_BYTES_PLAIN_PASSWORD	Pwd_MAX_CHARS_PLAIN_PASSWORD

#define Pwd_BYTES_ENCRYPTED_PASSWORD	Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Pwd_GetParUsrPwdLogin (void);

Err_SuccessOrError_t Pwd_CheckCurrentPassword (void);
bool Pwd_CheckPendingPassword (void);
void Pwd_AssignMyPendingPasswordToMyCurrentPassword (void);

void Pwd_UpdateMyPwd (void);
void Pwd_UpdateOtherUsrPwd (void);

void Pwd_PutLinkToSendNewPasswd (void);
void Pwd_ShowFormSendNewPwd (void);
void Pwd_ChkIdLoginAndSendNewPwd (void);
int Pwd_SendNewPasswordByEmail (char NewRandomPlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1]);
void Pwd_SetMyPendingPassword (char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1]);

bool Pwd_SlowCheckIfPasswordIsGood (const char PlainPassword[Pwd_MAX_BYTES_PLAIN_PASSWORD + 1],
                                    const char EncryptedPassword[Pwd_BYTES_ENCRYPTED_PASSWORD + 1],
                                    long UsrCod);
bool Pwd_FastCheckIfPasswordSeemsGood (const char *PlainPassword);

void Pwd_ShowFormChgMyPwd (void);
void Pwd_PutFormToGetNewPasswordOnce (void);
void Pwd_PutFormToGetNewPasswordTwice (void);
void Pwd_ShowFormChgOtherUsrPwd (void);

void Pwd_AskForConfirmationOnDangerousAction (void);
bool Pwd_GetConfirmationOnDangerousAction (void);

#endif
