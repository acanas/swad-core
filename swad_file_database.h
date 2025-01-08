// swad_file_database.h: files, operations with database

#ifndef _SWAD_FIL_DB
#define _SWAD_FIL_DB
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

#include <linux/limits.h>	// For PATH_MAX

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//------------------------------- File caches ---------------------------------
void Fil_DB_AddPublicDirToCache (const char *FullPathPriv,
                                 const char TmpPubDir[PATH_MAX + 1]);
void Fil_DB_GetPublicDirFromCache (const char *FullPathPriv,
                                   char TmpPubDir[PATH_MAX + 1]);
void Fil_DB_RemovePublicDirFromCache (const char *FullPathPriv);
void Fil_DB_RemovePublicDirsCache (void);
void Fil_DB_RemovePublicDirsFromExpiredSessions (void);

#endif
