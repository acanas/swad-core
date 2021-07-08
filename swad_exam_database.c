// swad_exam_database.c: exams operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

//#define _GNU_SOURCE 		// For asprintf
//#include <stdio.h>		// For asprintf
//#include <stdlib.h>		// For system, getenv, etc.
//#include <string.h>		// For string functions

//#include "swad_action.h"
#include "swad_database.h"
//#include "swad_error.h"
#include "swad_exam_database.h"
#include "swad_exam_log.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******** Check if the current session id is the same as the last one ********/
/*****************************************************************************/

bool ExaLog_DB_CheckIfSessionIsTheSameAsTheLast (long PrnCod)
  {
   /***** Check if the current session id
          is the same as the last one stored in database *****/
   return (DB_QueryCOUNT ("can not check session",
			  "SELECT COUNT(*)"
			   " FROM exa_log_sessions"
			  " WHERE LogCod="
				 "(SELECT MAX(LogCod)"
				   " FROM exa_log_sessions"
				  " WHERE PrnCod=%ld)"
			    " AND SessionId='%s'",
			  PrnCod,
			  Gbl.Session.Id) != 0);
  }

/*****************************************************************************/
/******** Check if the current user agent is the same as the last one ********/
/*****************************************************************************/

bool ExaLog_DB_CheckIfUserAgentIsTheSameAsTheLast (long PrnCod,const char *UserAgentDB)
  {
   /***** Get if the current user agent
          is the same as the last stored in database *****/
   return (DB_QueryCOUNT ("can not check user agent",
			  "SELECT COUNT(*)"
			   " FROM exa_log_user_agents"
			  " WHERE LogCod="
				 "(SELECT MAX(LogCod)"
				   " FROM exa_log_user_agents"
				  " WHERE PrnCod=%ld)"
			    " AND UserAgent='%s'",
			  PrnCod,
			  UserAgentDB) != 0);
  }

/*****************************************************************************/
/******************************** Log access *********************************/
/*****************************************************************************/

void ExaLog_DB_LogAccess (long LogCod,long PrnCod,ExaLog_Action_t Action)
  {
   /* Log access in exam log.
      Redundant data (also present in log table) are stored for speed */
   DB_QueryINSERT ("can not log exam access",
		   "INSERT INTO exa_log "
		   "(LogCod,PrnCod,ActCod,QstInd,CanAnswer,ClickTime,IP)"
		   " VALUES "
		   "(%ld,%ld,%ld,%d,'%c',NOW(),'%s')",
		   LogCod,
		   PrnCod,
		   (unsigned) Action,
		   ExaLog_GetQstInd (),
		   ExaLog_GetIfCanAnswer () ? 'Y' :
					      'N',
		   // NOW()   	   Redundant, for speed
		   Gbl.IP);	// Redundant, for speed
  }

/*****************************************************************************/
/*************************** Log session in database *************************/
/*****************************************************************************/

void ExaLog_DB_LogSession (long LogCod,long PrnCod)
  {
   DB_QueryINSERT ("can not log session",
		   "INSERT INTO exa_log_sessions "
		   "(LogCod,PrnCod,SessionId)"
		   " VALUES "
		   "(%ld,%ld,'%s')",
		   LogCod,
		   PrnCod,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/************************* Log user agent in database ************************/
/*****************************************************************************/

void ExaLog_DB_LogUserAgent (long LogCod,long PrnCod,const char *UserAgentDB)
  {
   DB_QueryINSERT ("can not log user agent",
		   "INSERT INTO exa_log_user_agents "
		   "(LogCod,PrnCod,UserAgent)"
		   " VALUES "
		   "(%ld,%ld,'%s')",
		   LogCod,
		   PrnCod,
		   UserAgentDB);
  }
