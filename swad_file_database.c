// swad_file_database.c: files, operations with database

/*
    SWAD (Shared Workspace At a Distance),
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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_file_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/********* Add public directory used to link private path to cache ***********/
/*****************************************************************************/

void Fil_DB_AddPublicDirToCache (const char *FullPathPriv,
                                 const char TmpPubDir[PATH_MAX + 1])
  {
   DB_QueryINSERT ("can not cache file",
		   "INSERT INTO brw_caches"
		   " (SessionId,PrivPath,TmpPubDir)"
		   " VALUES"
		   " ('%s','%s','%s')",
		   Gbl.Session.Id,
		   FullPathPriv,
		   TmpPubDir);
  }

/*****************************************************************************/
/******** Get public directory used to link private path from cache **********/
/*****************************************************************************/

void Fil_DB_GetPublicDirFromCache (const char *FullPathPriv,
                                   char TmpPubDir[PATH_MAX + 1])
  {
   DB_QuerySELECTString (TmpPubDir,PATH_MAX,"can not get check if file is cached",
			 "SELECT TmpPubDir"
			  " FROM brw_caches"
			 " WHERE SessionId='%s'"
			   " AND PrivPath='%s'",
			 Gbl.Session.Id,
			 FullPathPriv);
  }

/*****************************************************************************/
/******** Remove public directory used to link private path to cache *********/
/*****************************************************************************/

void Fil_DB_RemovePublicDirFromCache (const char *FullPathPriv)
  {
   DB_QueryDELETE ("can not remove cached file",
		   "DELETE FROM brw_caches"
		   " WHERE SessionId='%s'"
		     " AND PrivPath='%s'",
		   Gbl.Session.Id,
		   FullPathPriv);
  }

/*****************************************************************************/
/****** Remove public directories used to link private paths from cache ******/
/*****************************************************************************/

void Fil_DB_RemovePublicDirsCache (void)
  {
   DB_QueryDELETE ("can not cache file",
		   "DELETE FROM brw_caches"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/****** Remove public directories used to link private paths from cache ******/
/****** (from expired sessions)                                         ******/
/*****************************************************************************/

void Fil_DB_RemovePublicDirsFromExpiredSessions (void)
  {
   DB_QueryDELETE ("can not remove public directories in expired sessions",
		   "DELETE FROM brw_caches"
                   " WHERE SessionId NOT IN"
                         " (SELECT SessionId"
                          " FROM ses_sessions)");
  }
