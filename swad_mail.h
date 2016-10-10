// swad_mail.h: all the stuff related to e-mail

#ifndef _SWAD_MAI
#define _SWAD_MAI
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Mai_MAX_LENGTH_MAIL_DOMAIN	255
#define Mai_MAX_LENGTH_MAIL_INFO	255

typedef enum
  {
   Mai_ORDER_BY_DOMAIN = 0,
   Mai_ORDER_BY_INFO   = 1,
   Mai_ORDER_BY_USERS  = 2,
  } Mai_MailDomainsOrderType_t;

#define Mai_DEFAULT_ORDER_TYPE Mai_ORDER_BY_USERS

struct Mail
  {
   long MaiCod;
   char Domain[Mai_MAX_LENGTH_MAIL_DOMAIN+1];
   char Info[Mai_MAX_LENGTH_MAIL_INFO+1];
   unsigned NumUsrs;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Mai_SeeMailDomains (void);
void Mai_EditMailDomains (void);
void Mai_FreeListMailDomains (void);
bool Mai_CheckIfMailDomainIsAllowedForNotifications (const char *MailDomain);
void Mai_GetDataOfMailDomainByCod (struct Mail *Plc);
long Mai_GetParamMaiCod (void);
void Mai_RemoveMailDomain (void);
void Mai_RenameMailDomainShort (void);
void Mai_RenameMailDomainFull (void);
void Mai_RecFormNewMailDomain (void);

void Mai_ListEMails (void); // Creates an e-mail message to students

bool Mai_CheckIfEmailIsValid (const char *Email);
bool Mai_GetEmailFromUsrCod (struct UsrData *UsrDat);
long Mai_GetUsrCodFromEmail (const char *Email);

void Mai_PutLinkToChangeOtherUsrEmail (void);
void Mai_ShowFormOthEmail (void);
void Mai_ShowFormChangeUsrEmail (const struct UsrData *UsrDat,bool ItsMe);
void Mai_RemoveMyUsrEmail (void);
void Mai_RemoveOtherUsrEmail (void);
void May_NewMyUsrEmail (void);
void Mai_NewOtherUsrEmail (void);
bool Mai_UpdateEmailInDB (const struct UsrData *UsrDat,const char *NewEmail);

void Mai_PutButtonToCheckEmailAddress (void);
bool Mai_SendMailMsgToConfirmEmail (void);
void Mai_ConfirmEmail (void);

void Mai_CreateFileNameMail (void);
void Mai_WriteWelcomeNoteEMail (struct UsrData *UsrDat);
void Mai_WriteFootNoteEMail (Txt_Language_t Language);

bool Mai_ICanSeeEmail (struct UsrData *UsrDat);

#endif
