// swad_mail_database.h: everything related to email, operations with database

#ifndef _SWAD_MAI_DB
#define _SWAD_MAI_DB
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

#include "swad_constant.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//------------------------------ User email -----------------------------------
void Mai_DB_UpdateEmail (long UsrCod,const char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
void Mai_DB_ConfirmEmail (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

unsigned Mai_DB_GetMyEmails (MYSQL_RES **mysql_res,long UsrCod);
Exi_Exist_t Mai_DB_GetEmailFromUsrCod (MYSQL_RES **mysql_res,long UsrCod);
long Mai_DB_GetUsrCodFromEmail (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
char Mai_DB_CheckIfEmailIsConfirmed (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
Exi_Exist_t Mai_DB_CheckIfEmailExistsConfirmed (const char *Email);
bool Mai_DB_CheckIfEmailBelongToAnotherUsr (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

void Mai_DB_RemoveNotConfirmedEmailForOtherUsrs (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
void Mai_DB_RemoveEmail (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
void Mai_DB_RemoveUsrEmails (long UsrCod);

//---------------------------- Pending emails ---------------------------------
void Mai_DB_InsertPendingEmail (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1],
                                const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1]);

Exi_Exist_t Mai_DB_GetPendingEmail (MYSQL_RES **mysql_res,
                                    const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1]);

void Mai_DB_RemovePendingEmailForOtherUsrs (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
void Mai_DB_RemovePendingEmail (const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1]);
void Mai_DB_RemoveUsrPendingEmails (long UsrCod);
void Mai_DB_RemoveExpiredPendingEmails (void);

//----------------------------- Mail domains ----------------------------------
void Mai_DB_CreateTmpTables (void);
void Mai_DB_CreateMailDomain (const struct Mail *Mai);
void Mai_DB_UpdateMailDomainName (long MaiCod,
                                  const char *FldName,const char *NewMaiName);

unsigned Mai_DB_GetMailDomains (MYSQL_RES **mysql_res,Mai_DomainsOrder_t SelectedOrder);
Exi_Exist_t Mai_DB_GetMailDomainDataByCod (MYSQL_RES **mysql_res,long MaiCod);
Exi_Exist_t Mai_DB_CheckIfMailDomainNameExists (const char *FldName,const char *Name,long MaiCod);
bool Mai_DB_CheckIfMailDomainIsAllowedForNotif (const char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

void Mai_DB_RemoveMailDomain (long MaiCod);
void Mai_DB_RemoveTmpTables (void);

#endif
