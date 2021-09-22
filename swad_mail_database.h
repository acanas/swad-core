// swad_mail_database.h: all the stuff related to email, operations with database

#ifndef _SWAD_MAI_DB
#define _SWAD_MAI_DB
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

#include "swad_constant.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Mai_DB_CreateTmpTables (void);
void Mai_DB_CreateMailDomain (const struct Mail *Mai);
void Mai_DB_UpdateMailDomainName (long MaiCod,
                                  const char *FieldName,const char *NewMaiName);

unsigned Mai_DB_GetMailDomains (MYSQL_RES **mysql_res);
unsigned Mai_DB_GetDataOfMailDomainByCod (MYSQL_RES **mysql_res,long MaiCod);
bool Mai_DB_CheckIfMailDomainNameExists (const char *FieldName,const char *Name,long MaiCod);
bool Mai_DB_CheckIfMailDomainIsAllowedForNotif (const char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

void Mai_DB_RemoveMailDomain (long MaiCod);
void Mai_DB_RemoveTmpTables (void);

#endif
