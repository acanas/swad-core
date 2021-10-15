// swad_session_database.c: sessions operations with database

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

// #include <mysql/mysql.h>	// To access MySQL databases
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For sprintf
// #include <string.h>		// For string functions

#include "swad_config.h"
// #include "swad_connected_database.h"
#include "swad_database.h"
// #include "swad_error.h"
#include "swad_global.h"
#include "swad_pagination.h"
// #include "swad_parameter.h"
// #include "swad_session_database.h"
// #include "swad_timeline_database.h"

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

/*****************************************************************************/
/******************** Insert new session in the database *********************/
/*****************************************************************************/

void Ses_DB_InsertSession (void)
  {
   if (Gbl.Search.WhatToSearch == Sch_SEARCH_UNKNOWN)
      Gbl.Search.WhatToSearch = Sch_WHAT_TO_SEARCH_DEFAULT;

   DB_QueryINSERT ("can not create session",
		   "INSERT INTO ses_sessions"
	           " (SessionId,UsrCod,Password,Role,"
                     "CtyCod,InsCod,CtrCod,DegCod,CrsCod,LastTime,LastRefresh,WhatToSearch)"
                   " VALUES"
                   " ('%s',%ld,'%s',%u,"
                     "%ld,%ld,%ld,%ld,%ld,NOW(),NOW(),%u)",
		   Gbl.Session.Id,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Usrs.Me.UsrDat.Password,
		   (unsigned) Gbl.Usrs.Me.Role.Logged,
		   Gbl.Hierarchy.Cty.CtyCod,
		   Gbl.Hierarchy.Ins.InsCod,
		   Gbl.Hierarchy.Ctr.CtrCod,
		   Gbl.Hierarchy.Deg.DegCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Search.WhatToSearch);
  }

/*****************************************************************************/
/***************** Modify data of session in the database ********************/
/*****************************************************************************/

void Ses_DB_UpdateSession (void)
  {
   DB_QueryUPDATE ("can not update session",
		   "UPDATE ses_sessions"
		     " SET UsrCod=%ld,"
		          "Password='%s',"
		          "Role=%u,"
                          "CtyCod=%ld,"
                          "InsCod=%ld,"
                          "CtrCod=%ld,"
                          "DegCod=%ld,"
                          "CrsCod=%ld,"
                          "LastTime=NOW(),"
                          "LastRefresh=NOW()"
                   " WHERE SessionId='%s'",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Usrs.Me.UsrDat.Password,
		   (unsigned) Gbl.Usrs.Me.Role.Logged,
		   Gbl.Hierarchy.Cty.CtyCod,
		   Gbl.Hierarchy.Ins.InsCod,
		   Gbl.Hierarchy.Ctr.CtrCod,
		   Gbl.Hierarchy.Deg.DegCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/********************** Save last search into session ************************/
/*****************************************************************************/

void Ses_DB_SaveLastSearchIntoSession (void)
  {
   DB_QueryUPDATE ("can not update last search in session",
		   "UPDATE ses_sessions"
		     " SET WhatToSearch=%u,"
			  "SearchStr='%s'"
		   " WHERE SessionId='%s'",
		   (unsigned) Gbl.Search.WhatToSearch,
		   Gbl.Search.Str,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/********* Save last page of received/sent messages into session *************/
/*****************************************************************************/

void Ses_DB_SaveLastPageMsgIntoSession (Pag_WhatPaginate_t WhatPaginate,unsigned NumPage)
  {
   /***** Save last page of received/sent messages *****/
   DB_QueryUPDATE ("can not update last page of messages",
		   "UPDATE ses_sessions"
		     " SET %s=%u"
		   " WHERE SessionId='%s'",
                   WhatPaginate == Pag_MESSAGES_RECEIVED ? "LastPageMsgRcv" :
        	                                           "LastPageMsgSnt",
                   NumPage,Gbl.Session.Id);
  }

/*****************************************************************************/
/******************** Update session last refresh in database ****************/
/*****************************************************************************/

void Ses_DB_UpdateSessionLastRefresh (void)
  {
   DB_QueryUPDATE ("can not update session",
		   "UPDATE ses_sessions"
		     " SET LastRefresh=NOW()"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/*********** Check if the session already exists in the database *************/
/*****************************************************************************/
// Return true if session exists
// Return false if session does not exist or error

bool Ses_DB_CheckIfSessionExists (const char *IdSes)
  {
   /***** Get if session already exists in database *****/
   return (DB_QueryCOUNT ("can not check if a session already existed",
			  "SELECT COUNT(*)"
			   " FROM ses_sessions"
			  " WHERE SessionId='%s'",
			  IdSes) != 0);
  }

/*****************************************************************************/
/******* Get the data (user code and password) of an initiated session *******/
/*****************************************************************************/

unsigned Ses_DB_GetSessionData (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of session",
		   "SELECT UsrCod,"		// row[0]
			  "Password,"		// row[1]
			  "Role,"		// row[2]
			  "CtyCod,"		// row[3]
			  "InsCod,"		// row[4]
			  "CtrCod,"		// row[5]
			  "DegCod,"		// row[6]
			  "CrsCod,"		// row[7]
			  "WhatToSearch,"	// row[8]
			  "SearchStr"		// row[9]
		    " FROM ses_sessions"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/********* Get last page of received/sent messages stored in session *********/
/*****************************************************************************/

unsigned Ses_DB_GetLastPageMsgFromSession (Pag_WhatPaginate_t WhatPaginate)
  {
   static const char *Field[Pag_NUM_WHAT_PAGINATE] =
     {
      [Pag_MESSAGES_RECEIVED] = "LastPageMsgRcv",
      [Pag_MESSAGES_SENT    ] = "LastPageMsgSnt",
     };

   return DB_QuerySELECTUnsigned ("can not get last page of messages",
				     "SELECT %s"
				      " FROM ses_sessions"
				     " WHERE SessionId='%s'",
				     Field[WhatPaginate],
				     Gbl.Session.Id);
  }

/*****************************************************************************/
/*************************** Remove expired sessions *************************/
/*****************************************************************************/

void Ses_DB_RemoveExpiredSessions (void)
  {
   /***** Remove expired sessions *****/
   /* A session expire
      when last click (LastTime) is too old,
      or (when there was at least one refresh (navigator supports AJAX)
          and last refresh is too old (browser probably was closed)) */
   DB_QueryDELETE ("can not remove expired sessions",
		   "DELETE LOW_PRIORITY FROM ses_sessions"
		   " WHERE LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)"
                      " OR "
                         "(LastRefresh>LastTime+INTERVAL 1 SECOND"
                         " AND"
                         " LastRefresh<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu))",
                   Cfg_TIME_TO_CLOSE_SESSION_FROM_LAST_CLICK,
                   Cfg_TIME_TO_CLOSE_SESSION_FROM_LAST_REFRESH);
  }

/*****************************************************************************/
/******************* Remove all sessions of a given user *********************/
/*****************************************************************************/

void Ses_DB_RemoveUsrSessions (long UsrCod)
  {
   DB_QueryDELETE ("can not remove sessions of a user",
		   "DELETE FROM ses_sessions"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************************** Remove current session ***************************/
/*****************************************************************************/

void Ses_SB_RemoveCurrentSession (void)
  {
   /***** Remove current session *****/
   DB_QueryDELETE ("can not remove a session",
		   "DELETE FROM ses_sessions"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/******************* Insert session parameter in the database ****************/
/*****************************************************************************/

void Ses_DB_InsertParam (const char *ParamName,const char *ParamValue)
  {
   DB_QueryINSERT ("can not create session parameter",
		   "INSERT INTO ses_params"
		   " (SessionId,ParamName,ParamValue)"
		   " VALUES"
		   " ('%s','%s','%s')",
		   Gbl.Session.Id,
		   ParamName,
		   ParamValue ? ParamValue :
				"");
  }
