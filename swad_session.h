// swad_session.h: sessions

#ifndef _SWAD_SES
#define _SWAD_SES
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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

#include "swad_action.h"
#include "swad_pagination.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ses_GetNumSessions (void);
void Ses_CreateSession (void);
void Ses_CloseSession (void);

bool Ses_GetSessionData (void);

void Ses_InsertParamInDB (const char *ParamName,const char *ParamValue);
void Ses_RemoveParamFromThisSession (void);
void Ses_RemoveParamsFromExpiredSessions (void);
void Ses_GetParamFromDB (const char *ParamName,char *ParamValue,size_t StrSize);

bool Brw_GetPublicDirFromCache (const char *FullPathMediaPriv,
                                char TmpPubDir[PATH_MAX + 1]);
void Brw_DB_GetPublicDirFromCache (const char *FullPathMediaPriv,
                                   char TmpPubDir[PATH_MAX + 1]);
void Brw_AddPublicDirToCache (const char *FullPathMediaPriv,
                              const char TmpPubDir[PATH_MAX + 1]);
void Brw_DB_AddPublicDirToCache (const char *FullPathMediaPriv,
                                 const char TmpPubDir[PATH_MAX + 1]);
void Brw_DB_RemovePublicDirsCache (void);
void Brw_DB_RemovePublicDirsFromExpiredSessions (void);

#endif
