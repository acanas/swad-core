// swad_session.c: sessions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_connected_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_file_database.h"
#include "swad_global.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_session_database.h"
#include "swad_timeline_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ses_RemoveSessionFromDB (void);

/*****************************************************************************/
/************************** Get number of open sessions **********************/
/*****************************************************************************/

void Ses_GetNumSessions (void)
  {
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
   struct Sch_Search *Search = Sch_GetSearch ();

   /***** Create a unique name for the session *****/
   Str_Copy (Gbl.Session.Id,Cry_GetUniqueNameEncrypted (),sizeof (Gbl.Session.Id) - 1);

   /***** Check that session is not open *****/
   if (Ses_DB_CheckIfSessionExists (Gbl.Session.Id))
      Err_ShowErrorAndExit ("Can not create session.");

   /***** Add session to database *****/
   if (Search->WhatToSearch == Sch_SEARCH_UNKNOWN)
      Search->WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;
   Ses_DB_InsertSession (Search->WhatToSearch);

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
      Fil_DB_RemovePublicDirsCache ();

      /***** Remove session from database *****/
      Ses_RemoveSessionFromDB ();
      Gbl.Session.IsOpen = false;
      // Gbl.Session.HasBeenDisconnected = true;
      Gbl.Session.Id[0] = '\0';

      /***** If there are no more sessions for current user ==> remove user from connected list *****/
      Con_DB_RemoveOldConnected ();

      /***** Remove unused data associated to expired sessions *****/
      Ses_DB_RemoveParsFromExpiredSessions ();
      Fil_DB_RemovePublicDirsFromExpiredSessions ();

      /***** Now, user is not logged in *****/
      Gbl.Usrs.Me.Role.LoggedBeforeCloseSession = Gbl.Usrs.Me.Role.Logged;
      Gbl.Usrs.Me.Logged = false;
      Gbl.Usrs.Me.Role.Logged = Rol_UNK;	// TODO: Keep user's role in order to log the access?
      Hie_FreeMyHierarchy ();
      // Gbl.Usrs.Me.Hierarchy[Hie_CRS].Filled = false;
      // Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num = 0;

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
   struct Sch_Search *Search;

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
                 instit. code (row[4]),
                 center code (row[5]),
                 degree code (row[6]),
             and course code (row[7]) *****/
      Gbl.Hierarchy.Node[Hie_CTY].HieCod = Str_ConvertStrCodToLongCod (row[3]);
      Gbl.Hierarchy.Node[Hie_INS].HieCod = Str_ConvertStrCodToLongCod (row[4]);
      Gbl.Hierarchy.Node[Hie_CTR].HieCod = Str_ConvertStrCodToLongCod (row[5]);
      Gbl.Hierarchy.Node[Hie_DEG].HieCod = Str_ConvertStrCodToLongCod (row[6]);
      Gbl.Hierarchy.Node[Hie_CRS].HieCod = Str_ConvertStrCodToLongCod (row[7]);

      /***** Get last search *****/
      if (Gbl.Action.Act != ActLogOut)	// When closing session, last search will not be needed
	{
         Search = Sch_GetSearch ();

	 /* Get what to search (row[8]) */
	 Search->WhatToSearch = Sch_SEARCH_UNKNOWN;
	 if (sscanf (row[8],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Sch_NUM_WHAT_TO_SEARCH)
	       Search->WhatToSearch = (Sch_WhatToSearch_t) UnsignedNum;
	 if (Search->WhatToSearch == Sch_SEARCH_UNKNOWN)
	    Search->WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;

	 /* Get search string (row[9]) */
	 Str_Copy (Search->Str,row[9],sizeof (Search->Str) - 1);
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

void Ses_InsertParInDB (const char *ParName,const char *ParValue)
  {
   /***** Before of inserting the first session parameter passed to the next action,
	  delete all the parameters coming from the previous action *****/
   Ses_DB_RemovePar ();

   /***** For a unique session-parameter,
          don't insert a parameter more than one time *****/
   if (ParName)
      if (ParName[0])
         if (!Ses_DB_CheckIfParIsAlreadyStored (ParName))
	   {
	    /***** Insert session parameter in the database *****/
	    Ses_DB_InsertPar (ParName,ParValue);
	    Gbl.Session.ParsInsertedIntoDB = true;
	   }
  }
