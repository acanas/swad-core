// swad_session.c: sessions

/*
    SWAD (Shared Workspace At a Distance),
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
/************************************ Headers ********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases
#include <stddef.h>		// For NULL
#include <stdio.h>		// For sprintf
#include <string.h>		// For string functions

#include "swad_connected_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_session_database.h"
#include "swad_timeline_database.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ses_RemoveSessionFromDB (void);

static bool Ses_CheckIfParamIsAlreadyInDB (const char *ParamName);

static void Brw_DB_DeletePublicDirFromCache (const char *FullPathMediaPriv);

/*****************************************************************************/
/************************** Get number of open sessions **********************/
/*****************************************************************************/

void Ses_GetNumSessions (void)
  {
   /***** Get the number of open sessions from database *****/
   Gbl.Session.NumSessions = (unsigned) DB_GetNumRowsTable ("ses_sessions");

   Gbl.Usrs.Connected.TimeToRefreshInMs = (unsigned long) (Gbl.Session.NumSessions/
	                                                   Cfg_TIMES_PER_SECOND_REFRESH_CONNECTED) * 1000UL;
   if (Gbl.Usrs.Connected.TimeToRefreshInMs < Con_MIN_TIME_TO_REFRESH_CONNECTED_IN_MS)
      Gbl.Usrs.Connected.TimeToRefreshInMs = Con_MIN_TIME_TO_REFRESH_CONNECTED_IN_MS;
   else if (Gbl.Usrs.Connected.TimeToRefreshInMs > Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS)
      Gbl.Usrs.Connected.TimeToRefreshInMs = Con_MAX_TIME_TO_REFRESH_CONNECTED_IN_MS;
  }

/*****************************************************************************/
/*************************** Create a new session ****************************/
/*****************************************************************************/

void Ses_CreateSession (void)
  {
   /***** Create a unique name for the session *****/
   Str_Copy (Gbl.Session.Id,Gbl.UniqueNameEncrypted,sizeof (Gbl.Session.Id) - 1);

   /***** Check that session is not open *****/
   if (Ses_DB_CheckIfSessionExists (Gbl.Session.Id))
      Err_ShowErrorAndExit ("Can not create session.");

   /***** Add session to database *****/
   Ses_DB_InsertSession ();

   /***** Update time and course in connected list *****/
   Con_DB_UpdateMeInConnectedList ();

   /***** Update number of open sessions in order to show them properly *****/
   Ses_GetNumSessions ();
  }

/*****************************************************************************/
/************************** Close current session ****************************/
/*****************************************************************************/

void Ses_CloseSession (void)
  {
   if (Gbl.Usrs.Me.Logged)
     {
      /***** Remove links to private files from cache *****/
      Brw_DB_RemovePublicDirsCache ();

      /***** Remove session from database *****/
      Ses_RemoveSessionFromDB ();
      Gbl.Session.IsOpen = false;
      // Gbl.Session.HasBeenDisconnected = true;
      Gbl.Session.Id[0] = '\0';

      /***** If there are no more sessions for current user ==> remove user from connected list *****/
      Con_DB_RemoveOldConnected ();

      /***** Remove unused data associated to expired sessions *****/
      Ses_RemoveParamsFromExpiredSessions ();
      Brw_DB_RemovePublicDirsFromExpiredSessions ();

      /***** Now, user is not logged in *****/
      Gbl.Usrs.Me.Role.LoggedBeforeCloseSession = Gbl.Usrs.Me.Role.Logged;
      Gbl.Usrs.Me.Logged = false;
      Gbl.Usrs.Me.IBelongToCurrentIns = false;
      Gbl.Usrs.Me.IBelongToCurrentCtr = false;
      Gbl.Usrs.Me.IBelongToCurrentDeg = false;
      Gbl.Usrs.Me.IBelongToCurrentCrs = false;
      Gbl.Usrs.Me.Role.Logged = Rol_UNK;	// Don't uncomment this line. Don't change the role to unknown. Keep user's role in order to log the access
      Gbl.Usrs.Me.MyCrss.Filled = false;
      Gbl.Usrs.Me.MyCrss.Num = 0;

      /***** Update number of open sessions in order to show them properly *****/
      Ses_GetNumSessions ();
     }
  }

/*****************************************************************************/
/********************** Remove session from the database *********************/
/*****************************************************************************/

static void Ses_RemoveSessionFromDB (void)
  {
   /***** Remove current session *****/
   Ses_SB_RemoveCurrentSession ();

   /***** Clear old unused social timelines in database *****/
   // This is necessary to prevent the table growing and growing
   Tml_DB_ClearOldTimelinesNotesFromDB ();
  }

/*****************************************************************************/
/******* Get the data (user code and password) of an initiated session *******/
/*****************************************************************************/

bool Ses_GetSessionData (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   bool Result = false;

   /***** Check if the session existed in the database *****/
   if (Ses_DB_GetSessionData (&mysql_res))
     {
      row = mysql_fetch_row (mysql_res);

      /***** Get user code (row[0]) *****/
      Gbl.Session.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /***** Get password (row[1]) *****/
      Str_Copy (Gbl.Usrs.Me.LoginEncryptedPassword,row[1],
                sizeof (Gbl.Usrs.Me.LoginEncryptedPassword) - 1);

      /***** Get logged user type (row[2]) *****/
      if (sscanf (row[2],"%u",&Gbl.Usrs.Me.Role.FromSession) != 1)
         Gbl.Usrs.Me.Role.FromSession = Rol_UNK;

      /***** Get country code (row[3]),
                 institution code (row[4]),
                 center code (row[5]),
                 degree code (row[6]),
             and course code (row[7]) *****/
      Gbl.Hierarchy.Cty.CtyCod = Str_ConvertStrCodToLongCod (row[3]);
      Gbl.Hierarchy.Ins.InsCod = Str_ConvertStrCodToLongCod (row[4]);
      Gbl.Hierarchy.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[5]);
      Gbl.Hierarchy.Deg.DegCod = Str_ConvertStrCodToLongCod (row[6]);
      Gbl.Hierarchy.Crs.CrsCod = Str_ConvertStrCodToLongCod (row[7]);

      /***** Get last search *****/
      if (Gbl.Action.Act != ActLogOut)	// When closing session, last search will not be needed
	{
	 /* Get what to search (row[8]) */
	 Gbl.Search.WhatToSearch = Sch_SEARCH_UNKNOWN;
	 if (sscanf (row[8],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Sch_NUM_WHAT_TO_SEARCH)
	       Gbl.Search.WhatToSearch = (Sch_WhatToSearch_t) UnsignedNum;
	 if (Gbl.Search.WhatToSearch == Sch_SEARCH_UNKNOWN)
	    Gbl.Search.WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;

	 /* Get search string (row[9]) */
	 Str_Copy (Gbl.Search.Str,row[9],sizeof (Gbl.Search.Str) - 1);
	}

      Result = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Result;
  }

/*****************************************************************************/
/******************* Insert session parameter in the database ****************/
/*****************************************************************************/

void Ses_InsertParamInDB (const char *ParamName,const char *ParamValue)
  {
   /***** Before of inserting the first session parameter passed to the next action,
	  delete all the parameters coming from the previous action *****/
   Ses_RemoveParamFromThisSession ();

   /***** For a unique session-parameter,
          don't insert a parameter more than one time *****/
   if (ParamName)
      if (ParamName[0])
         if (!Ses_CheckIfParamIsAlreadyInDB (ParamName))
	   {
	    /***** Insert session parameter in the database *****/
	    Ses_DB_InsertParam (ParamName,ParamValue);
	    Gbl.Session.ParamsInsertedIntoDB = true;
	   }
  }

/*****************************************************************************/
/************ Remove session parameters of a session from database ***********/
/*****************************************************************************/

void Ses_RemoveParamFromThisSession (void)
  {
   if (Gbl.Session.IsOpen &&			// There is an open session
       !Gbl.Session.ParamsInsertedIntoDB)		// No params just inserted
      /***** Remove session parameters of this session *****/
      DB_QueryDELETE ("can not remove session parameters of current session",
		      "DELETE FROM ses_params"
		      " WHERE SessionId='%s'",
		      Gbl.Session.Id);
  }

/*****************************************************************************/
/********* Remove expired hidden parameters (from expired sessions) **********/
/*****************************************************************************/

void Ses_RemoveParamsFromExpiredSessions (void)
  {
   /***** Remove session parameters from expired sessions *****/
   DB_QueryDELETE ("can not remove session parameters of expired sessions",
		   "DELETE FROM ses_params"
                   " WHERE SessionId NOT IN"
                         " (SELECT SessionId"
                            " FROM ses_sessions)");
  }

/*****************************************************************************/
/************** Check if a session parameter existed in database *************/
/*****************************************************************************/
// Return true if the parameter already existed in database

static bool Ses_CheckIfParamIsAlreadyInDB (const char *ParamName)
  {
   return (DB_QueryCOUNT ("can not check if a session parameter"
			  " is already in database",
			  "SELECT COUNT(*)"
			   " FROM ses_params"
			  " WHERE SessionId='%s'"
			    " AND ParamName='%s'",
			  Gbl.Session.Id,
			  ParamName) != 0);
  }

/*****************************************************************************/
/***************** Get session parameter from the database *******************/
/*****************************************************************************/
// StrSize is the size of the parameter value, not including the ending '\0'

void Ses_GetParamFromDB (const char *ParamName,char *ParamValue,size_t StrSize)
  {
   ParamValue[0] = '\0';
   if (Gbl.Session.IsOpen)	// If the session is open, get parameter from DB
      /***** Get a session parameter from database *****/
      DB_QuerySELECTString (ParamValue,StrSize,"can not get a session parameter",
			    "SELECT ParamValue"	// row[0]
			     " FROM ses_params"
			    " WHERE SessionId='%s'"
			      " AND ParamName='%s'",
			    Gbl.Session.Id,
			    ParamName);
  }

/*****************************************************************************/
/******** Get public directory used to link private path from cache **********/
/*****************************************************************************/

bool Brw_GetPublicDirFromCache (const char *FullPathMediaPriv,
                                char TmpPubDir[PATH_MAX + 1])
  {
   bool Cached;
   bool TmpPubDirExists;

   /***** Reset temporary directory *****/
   TmpPubDir[0] = '\0';

   if (Gbl.Session.IsOpen)
     {
      /***** Get temporary directory from cache *****/
      Brw_DB_GetPublicDirFromCache (FullPathMediaPriv,TmpPubDir);
      Cached = (TmpPubDir[0] != '\0');

      /***** Check if temporary public directory exists *****/
      if (Cached)
	{
	 /* If not exists (it could be deleted if its lifetime has expired)
	    ==> remove from cache */
         TmpPubDirExists = Fil_CheckIfPathExists (TmpPubDir);
         if (!TmpPubDirExists)
            Brw_DB_DeletePublicDirFromCache (FullPathMediaPriv);
         return TmpPubDirExists;
	}
     }

   return false;
  }

/*****************************************************************************/
/******** Get public directory used to link private path from cache **********/
/*****************************************************************************/

void Brw_DB_GetPublicDirFromCache (const char *FullPathMediaPriv,
                                   char TmpPubDir[PATH_MAX + 1])
  {
   DB_QuerySELECTString (TmpPubDir,PATH_MAX,"can not get check if file is cached",
			 "SELECT TmpPubDir"
			  " FROM brw_caches"
			 " WHERE SessionId='%s'"
			   " AND PrivPath='%s'",
			 Gbl.Session.Id,
			 FullPathMediaPriv);
  }

/*****************************************************************************/
/********* Add public directory used to link private path to cache ***********/
/*****************************************************************************/

static void Brw_DB_DeletePublicDirFromCache (const char *FullPathMediaPriv)
  {
   /***** Delete possible entry *****/
   if (Gbl.Session.IsOpen)
      DB_QueryDELETE ("can not remove cached file",
		      "DELETE FROM brw_caches"
		      " WHERE SessionId='%s'"
		        " AND PrivPath='%s'",
		      Gbl.Session.Id,FullPathMediaPriv);
  }

/*****************************************************************************/
/********* Add public directory used to link private path to cache ***********/
/*****************************************************************************/

void Brw_AddPublicDirToCache (const char *FullPathMediaPriv,
                              const char TmpPubDir[PATH_MAX + 1])
  {
   if (Gbl.Session.IsOpen)
     {
      /* Delete possible old entry */
      Brw_DB_DeletePublicDirFromCache (FullPathMediaPriv);

      /* Insert new entry */
      Brw_DB_AddPublicDirToCache (FullPathMediaPriv,TmpPubDir);
     }
  }

/*****************************************************************************/
/********* Add public directory used to link private path to cache ***********/
/*****************************************************************************/

void Brw_DB_AddPublicDirToCache (const char *FullPathMediaPriv,
                                 const char TmpPubDir[PATH_MAX + 1])
  {
   /* Insert new entry */
   DB_QueryINSERT ("can not cache file",
		   "INSERT INTO brw_caches"
		   " (SessionId,PrivPath,TmpPubDir)"
		   " VALUES"
		   " ('%s','%s','%s')",
		   Gbl.Session.Id,
		   FullPathMediaPriv,
		   TmpPubDir);
  }

/*****************************************************************************/
/****** Remove public directories used to link private paths from cache ******/
/*****************************************************************************/

void Brw_DB_RemovePublicDirsCache (void)
  {
   /***** Insert into cache *****/
   if (Gbl.Session.IsOpen)
      DB_QueryDELETE ("can not cache file",
		      "DELETE FROM brw_caches"
		      " WHERE SessionId='%s'",
		      Gbl.Session.Id);
  }

/*****************************************************************************/
/****** Remove public directories used to link private paths from cache ******/
/****** (from expired sessions)                                         ******/
/*****************************************************************************/

void Brw_DB_RemovePublicDirsFromExpiredSessions (void)
  {
   /***** Remove public directories in expired sessions *****/
   DB_QueryDELETE ("can not remove public directories in expired sessions",
		   "DELETE FROM brw_caches"
                   " WHERE SessionId NOT IN"
                         " (SELECT SessionId"
                          " FROM ses_sessions)");
  }
