// swad_session_database.h: sessions operations with database

#ifndef _SWAD_SES_DB
#define _SWAD_SES_DB
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

#include "swad_pagination.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/
//---------------------------------- Sessions ---------------------------------
void Ses_DB_InsertSession (void);
void Ses_DB_UpdateSession (void);
void Ses_DB_SaveLastSearchIntoSession (void);
void Ses_DB_SaveLastPageMsgIntoSession (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage);
void Ses_DB_UpdateSessionLastRefresh (void);

bool Ses_DB_CheckIfSessionExists (const char *IdSes);
unsigned Ses_DB_GetSessionData (MYSQL_RES **mysql_res);
unsigned Ses_DB_GetSomeSessionData (MYSQL_RES **mysql_res,const char *SessionId);
unsigned Ses_DB_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate);

void Ses_DB_RemoveExpiredSessions (void);
void Ses_DB_RemoveUsrSessions (long UsrCod);
void Ses_SB_RemoveCurrentSession (void);

//---------------------------- Session parameters -----------------------------
void Ses_DB_InsertParam (const char *ParamName,const char *ParamValue);

bool Ses_DB_CheckIfParamIsAlreadyStored (const char *ParamName);
void Ses_DB_GetParam (const char *ParamName,char *ParamValue,size_t StrSize);

void Ses_DB_RemoveParam (void);
void Ses_DB_RemoveParamsFromExpiredSessions (void);

#endif
