// swad_session.c: sessions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdio.h>		// For sprintf
#include <string.h>		// For string functions

#include "swad_connected.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_social.h"

/*****************************************************************************/
/**************************** Internal constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void Ses_RemoveSessionFromDB (void);

static bool Ses_CheckIfHiddenParIsAlreadyInDB (Act_Action_t Action,const char *ParamName);

/*****************************************************************************/
/************************** Get number of open sessions **********************/
/*****************************************************************************/

void Ses_GetNumSessions (void)
  {
   char Query[128];

   /***** Get the number of open sessions from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM sessions");
   Gbl.Session.NumSessions = (unsigned) DB_QueryCOUNT (Query,"can not get the number of open sessions");

   Gbl.Usrs.Connected.TimeToRefreshInMs = (unsigned long) (Gbl.Session.NumSessions/Cfg_TIMES_PER_SECOND_REFRESH_CONNECTED) * 1000UL;
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
   strcpy (Gbl.Session.Id,Gbl.UniqueNameEncrypted);

   /***** Check that session is not open *****/
   if (Ses_CheckIfSessionExists (Gbl.Session.Id))
      Lay_ShowErrorAndExit ("Can not create session.");

   /***** Add session to database *****/
   Ses_InsertSessionInDB ();

   /***** Update time and course in connected list *****/
   Con_UpdateMeInConnectedList ();

   /***** Update number of open sessions in order to show them properly *****/
   Ses_GetNumSessions ();
  }

/*****************************************************************************/
/*********** Check if the session already exists in the database *************/
/*****************************************************************************/
// Return true if session exists
// Return false if session does not exist or error

bool Ses_CheckIfSessionExists (const char *IdSes)
  {
   char Query[512];

   /***** Get if session already exists in database *****/
   sprintf (Query,"SELECT COUNT(*) FROM sessions WHERE SessionId='%s'",
            IdSes);
   return (DB_QueryCOUNT (Query,"can not check if a session already existed") != 0);
  }

/*****************************************************************************/
/************************** Close current session ****************************/
/*****************************************************************************/

void Ses_CloseSession (void)
  {
   if (Gbl.Usrs.Me.Logged)
     {
      /***** Remove session from database *****/
      Ses_RemoveSessionFromDB ();
      Gbl.Session.IsOpen = false;
      // Gbl.Session.HasBeenDisconnected = true;
      Gbl.Session.Id[0] = '\0';

      /***** If there are no more sessions for current user ==> remove user from connected list *****/
      Con_RemoveOldConnected ();

      Ses_RemoveHiddenParFromExpiredSessions ();

      /***** Now, user is not logged in *****/
      Gbl.Usrs.Me.LoggedRoleBeforeCloseSession = Gbl.Usrs.Me.LoggedRole;
      Gbl.Usrs.Me.Logged = false;
      Gbl.Usrs.Me.IBelongToCurrentIns = false;
      Gbl.Usrs.Me.IBelongToCurrentCtr = false;
      Gbl.Usrs.Me.IBelongToCurrentDeg = false;
      Gbl.Usrs.Me.IBelongToCurrentCrs = false;
      Gbl.Usrs.Me.LoggedRole = Rol_UNKNOWN;	// Don't uncomment this line. Don't change the role to unknown. Keep user's role in order to log the access
      Gbl.Usrs.Me.MyCrss.Filled = false;
      Gbl.Usrs.Me.MyCrss.Num = 0;

      /***** Update number of open sessions in order to show them properly *****/
      Ses_GetNumSessions ();
     }
  }

/*****************************************************************************/
/******************** Insert new session in the database *********************/
/*****************************************************************************/

void Ses_InsertSessionInDB (void)
  {
   char Query[1024];

   /***** Insert session in the database *****/
   sprintf (Query,"INSERT INTO sessions (SessionId,UsrCod,Password,Role,"
                  "CtyCod,InsCod,CtrCod,DegCod,CrsCod,LastTime,LastRefresh,WhatToSearch)"
                  " VALUES ('%s','%ld','%s','%u',"
                  "'%ld','%ld','%ld','%ld','%ld',NOW(),NOW(),'%u')",
            Gbl.Session.Id,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Gbl.Usrs.Me.UsrDat.Password,
            (unsigned) Gbl.Usrs.Me.LoggedRole,
            Gbl.CurrentCty.Cty.CtyCod,
            Gbl.CurrentIns.Ins.InsCod,
            Gbl.CurrentCtr.Ctr.CtrCod,
            Gbl.CurrentDeg.Deg.DegCod,
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Search.WhatToSearch);
   DB_QueryINSERT (Query,"can not create session");
  }

/*****************************************************************************/
/***************** Modify data of session in the database ********************/
/*****************************************************************************/

void Ses_UpdateSessionDataInDB (void)
  {
   char Query[1024];

   /***** Update session in database *****/
   sprintf (Query,"UPDATE sessions SET UsrCod='%ld',Password='%s',Role='%u',"
                  "CtyCod='%ld',InsCod='%ld',CtrCod='%ld',DegCod='%ld',CrsCod='%ld',"
                  "LastTime=NOW(),LastRefresh=NOW()"
                  " WHERE SessionId='%s'",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Gbl.Usrs.Me.UsrDat.Password,
            (unsigned) Gbl.Usrs.Me.LoggedRole,
            Gbl.CurrentCty.Cty.CtyCod,
            Gbl.CurrentIns.Ins.InsCod,
            Gbl.CurrentCtr.Ctr.CtrCod,
            Gbl.CurrentDeg.Deg.DegCod,
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Session.Id);
   DB_QueryUPDATE (Query,"can not update session");
  }

/*****************************************************************************/
/******************** Modify session last refresh in database ****************/
/*****************************************************************************/

void Ses_UpdateSessionLastRefreshInDB (void)
  {
   char Query[512];

   /***** Update session in database *****/
   sprintf (Query,"UPDATE sessions SET LastRefresh=NOW()"
	          " WHERE SessionId='%s'",
	    Gbl.Session.Id);
   DB_QueryUPDATE (Query,"can not update session");
  }

/*****************************************************************************/
/********************** Remove session from the database *********************/
/*****************************************************************************/

static void Ses_RemoveSessionFromDB (void)
  {
   char Query[512];

   /***** Remove current session *****/
   sprintf (Query,"DELETE FROM sessions WHERE SessionId='%s'",
            Gbl.Session.Id);
   DB_QueryDELETE (Query,"can not remove a session");

   /***** Clear old unused social timelines in database *****/
   // This is necessary to prevent the table growing and growing
   Soc_ClearOldTimelinesDB ();
  }

/*****************************************************************************/
/*************************** Remove expired sessions *************************/
/*****************************************************************************/

void Ses_RemoveExpiredSessions (void)
  {
   char Query[1024];

   /***** Remove expired sessions *****/
   /* A session expire
      when last click (LastTime) is too old,
      or (when there was at least one refresh (navigator supports AJAX)
          and last refresh is too old (browser probably was closed)) */
   sprintf (Query,"DELETE LOW_PRIORITY FROM sessions WHERE"
                  " LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')"
                  " OR "
                  "(LastRefresh>LastTime+INTERVAL 1 SECOND"
                  " AND"
                  " LastRefresh<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu'))",
            Cfg_TIME_TO_CLOSE_SESSION_FROM_LAST_CLICK,
            Cfg_TIME_TO_CLOSE_SESSION_FROM_LAST_REFRESH);
   DB_QueryDELETE (Query,"can not remove expired sessions");
  }

/*****************************************************************************/
/******* Get the data (user code and password) of an initiated session *******/
/*****************************************************************************/

bool Ses_GetSessionData (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   bool Result = false;

   /***** Query data of session from database *****/
   sprintf (Query,"SELECT UsrCod,Password,Role,"
	          "CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
	          "WhatToSearch,SearchString"
	          " FROM sessions WHERE SessionId='%s'",
	    Gbl.Session.Id);

   /***** Check if the session existed in the database *****/
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of session"))
     {
      row = mysql_fetch_row (mysql_res);

      /***** Get user code (row[0]) *****/
      Gbl.Session.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /***** Get password (row[1]) *****/
      strcpy (Gbl.Usrs.Me.LoginEncryptedPassword,row[1]);

      /***** Get logged user type (row[2]) *****/
      if (sscanf (row[2],"%u",&Gbl.Usrs.Me.RoleFromSession) != 1)
         Gbl.Usrs.Me.RoleFromSession = Rol_UNKNOWN;

      /***** Get country code (row[3]) *****/
      Gbl.CurrentCty.Cty.CtyCod = Str_ConvertStrCodToLongCod (row[3]);

      /***** Get institution code (row[4]) *****/
      Gbl.CurrentIns.Ins.InsCod = Str_ConvertStrCodToLongCod (row[4]);

      /***** Get centre code (row[5]) *****/
      Gbl.CurrentCtr.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[5]);

      /***** Get degree code (row[6]) *****/
      Gbl.CurrentDeg.Deg.DegCod = Str_ConvertStrCodToLongCod (row[6]);

      /***** Get course code (row[7]) *****/
      Gbl.CurrentCrs.Crs.CrsCod = Str_ConvertStrCodToLongCod (row[7]);

      /***** Get last search *****/
      if (Gbl.Action.Act != ActLogOut)	// When closing session, last search will not be needed
	{
	 /* Get what to search (row[8]) */
	 Gbl.Search.WhatToSearch = Sch_SEARCH_ALL;
	 if (sscanf (row[8],"%u",&UnsignedNum) == 1)
	    if (UnsignedNum < Sch_NUM_WHAT_TO_SEARCH)
	       Gbl.Search.WhatToSearch = (Sch_WhatToSearch_t) UnsignedNum;

	 /* Get search string (row[9]) */
	 strncpy (Gbl.Search.Str,row[9],Sch_MAX_LENGTH_STRING_TO_FIND);
	 Gbl.Search.Str[Sch_MAX_LENGTH_STRING_TO_FIND] = '\0';
	}

      Result = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Result;
  }

/*****************************************************************************/
/******************* Insert hidden parameter in the database *****************/
/*****************************************************************************/

void Ses_InsertHiddenParInDB (Act_Action_t Action,const char *ParamName,const char *ParamValue)
  {
   char Query[512+Cns_MAX_BYTES_TEXT];

   /***** Before of inserting the first hidden parameter passed to the next action,
	  delete all the parameters coming from the previous action *****/
   if (!Gbl.HiddenParamsInsertedIntoDB)
      Ses_RemoveHiddenParFromThisSession ();

   /***** For a unique session-action-parameter, don't insert a parameter more than one time *****/
   if (!Ses_CheckIfHiddenParIsAlreadyInDB (Action,ParamName))
     {
      /***** Insert parameter in the database *****/
      sprintf (Query,"INSERT INTO hidden_params (SessionId,Action,ParamName,ParamValue)"
		     " VALUES ('%s','%d','%s','%s')",
	       Gbl.Session.Id,(int) Action,ParamName,ParamValue);
      DB_QueryINSERT (Query,"can not create hidden parameter");
      Gbl.HiddenParamsInsertedIntoDB = true;
     }
  }

/*****************************************************************************/
/************ Remove hidden parameters of a session from database ************/
/*****************************************************************************/

void Ses_RemoveHiddenParFromThisSession (void)
  {
   char Query[512];

   if (Gbl.Session.IsOpen)
     {
      /***** Remove hidden parameters of this session *****/
      sprintf (Query,"DELETE FROM hidden_params WHERE SessionId='%s'",
               Gbl.Session.Id);
      DB_QueryDELETE (Query,"can not remove hidden parameters of current session");
     }
  }

/*****************************************************************************/
/********* Remove expired hidden parameters (from expired sessions) **********/
/*****************************************************************************/

void Ses_RemoveHiddenParFromExpiredSessions (void)
  {
   char Query[512];

   /***** Remove hidden parameters from expired sessions *****/
   sprintf (Query,"DELETE FROM hidden_params"
                  " WHERE SessionId NOT IN (SELECT SessionId FROM sessions)");
   DB_QueryDELETE (Query,"can not remove hidden parameters of expired sessions");
  }

/*****************************************************************************/
/*************** Check if a hidden parameter existed in database *************/
/*****************************************************************************/
// Return true if the parameter already existed in database

static bool Ses_CheckIfHiddenParIsAlreadyInDB (Act_Action_t Action,const char *ParamName)
  {
   char Query[512];

   /***** Get a hidden parameter from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM hidden_params"
                  " WHERE SessionId='%s' AND Action='%d' AND ParamName='%s'",
            Gbl.Session.Id,(int) Action,ParamName);
   return (DB_QueryCOUNT (Query,"can not check if a hidden parameter is already in database") != 0);
  }

/*****************************************************************************/
/***************** Get hidden parameter from the database ********************/
/*****************************************************************************/
// Return true if the parameter is too big

unsigned Ses_GetHiddenParFromDB (Act_Action_t Action,const char *ParamName,char *ParamValue,size_t MaxBytes)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool ParameterIsTooBig = false;
   unsigned NumTimes = 0;
   const char *Ptr;

   ParamValue[0] = '\0';
   if (Gbl.Session.IsOpen)	// If the session is open, get parameter from DB
     {
      /***** Get a hidden parameter from database *****/
      sprintf (Query,"SELECT ParamValue FROM hidden_params"
                     " WHERE SessionId='%s' AND Action='%d' AND ParamName='%s'",
               Gbl.Session.Id,(int) Action,ParamName);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get a hidden parameter");

      /***** Check if the parameter is found in database *****/
      if (NumRows)
        {
         /***** Get the value del parameter *****/
         row = mysql_fetch_row (mysql_res);
         strncpy (ParamValue,row[0],MaxBytes);
         ParamValue[MaxBytes] = '\0';
         ParameterIsTooBig = (strlen (row[0]) > MaxBytes);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   if (ParameterIsTooBig)
     {
      sprintf (Gbl.Message,"Hidden parameter <strong>%s</strong> too large,"
                           " it exceed the maximum allowed size (%lu bytes).",
               ParamName,(unsigned long) MaxBytes);
      Lay_ShowErrorAndExit (Gbl.Message);
     }

   /***** Count number of values of the parameter *****/
   Ptr = ParamValue;
   while (*Ptr)
      if (Par_GetNextStrUntilSeparParamMult (&Ptr,NULL,MaxBytes)) // Characters found?
         NumTimes++;

   return NumTimes;
  }
