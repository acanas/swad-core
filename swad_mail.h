// swad_mail.h: everything related to email

#ifndef _SWAD_MAI
#define _SWAD_MAI
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_mail_type.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Mai_SeeMailDomains (void);
void Mai_EditMailDomains (void);

Usr_Can_t Mai_CheckIfUsrCanReceiveEmailNotif (const struct Usr_Data *UsrDat);
void Mai_WriteWarningEmailNotifications (void);

void Mai_GetMailDomainDataByCod (struct Mail *Plc);
void Mai_RemoveMailDom (void);
void Mai_RenameMailDomShrt (void);
void Mai_RenameMailDomFull (void);
void Mai_ContEditAfterChgMai (void);
void Mai_ReceiveNewMailDom (void);

void Mai_ReqUsrsToListEmails (void);
void Mai_GetSelUsrsAndListEmails (void);

Err_SuccessOrError_t Mai_CheckIfEmailIsValid (const char *Email);
void Mai_GetEmailFromUsrCod (struct Usr_Data *UsrDat);

void Mai_PutFormToGetNewEmail (const char *NewEmail);

void Mai_ShowFormChangeMyEmail (void);
void Mai_ShowFormChangeOtherUsrEmail (void);

void Mai_RemoveMyUsrEmail (void);
void Mai_RemoveOtherUsrEmail (void);

void May_NewMyUsrEmail (void);
void Mai_ChangeOtherUsrEmail (void);
Err_SuccessOrError_t Mai_UpdateEmailInDB (const struct Usr_Data *UsrDat,
					  const char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

void Mai_SendMailMsgToConfirmEmail (void);
void Mai_ConfirmEmail (void);

void Mai_CreateFileNameMail (char FileNameMail[PATH_MAX + 1],FILE **FileMail);
void Mai_WriteWelcomeNoteEMail (FILE *FileMail,const struct Usr_Data *UsrDat,
                                Lan_Language_t ToUsrLanguage);
void Mai_WriteFootNoteEMail (FILE *FileMail,Lan_Language_t Language);

Usr_Can_t Mai_ICanSeeOtherUsrEmail (const struct Usr_Data *UsrDat);

int Mai_SendMailMsg (const char FileNameMail[PATH_MAX + 1],
                     const char *Subject,
                     const char ToEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

#endif
