// swad_session.h: sessions

#ifndef _SWAD_SES
#define _SWAD_SES
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

#include "swad_action.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ses_GetNumSessions (void);
void Ses_CreateSession (void);
bool Ses_CheckIfSessionExists (const char *IdSes);
void Ses_CloseSession (void);
void Ses_InsertSessionInDB (void);
void Ses_UpdateSessionDataInDB (void);
void Ses_UpdateSessionLastRefreshInDB (void);
void Ses_RemoveExpiredSessions (void);
bool Ses_GetSessionData (void);

void Ses_InsertHiddenParInDB (const char *ParamName,const char *ParamValue);
void Ses_RemoveHiddenParFromThisSession (void);
void Ses_RemoveHiddenParFromExpiredSessions (void);
void Ses_GetHiddenParFromDB (const char *ParamName,char *ParamValue,
                             size_t MaxBytes);

bool Ses_GetPublicDirFromCache (const char *FullPathMediaPriv,
                                char TmpPubDir[PATH_MAX + 1]);
void Ses_AddPublicDirToCache (const char *FullPathMediaPriv,
                              const char TmpPubDir[PATH_MAX + 1]);
void Ses_RemovePublicDirsCache (void);
void Ses_RemovePublicDirsFromExpiredSessions (void);

#endif
