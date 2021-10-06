// swad_log_database.c: access log, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_log.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/*********************** Log access in historical log ************************/
/*****************************************************************************/

long Log_DB_LogAccessInHistoricalLog (long ActCod,Rol_Role_t RoleToStore)
  {
   return
   DB_QueryINSERTandReturnCode ("can not log access",
				"INSERT INTO log"
				" (ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
				  "UsrCod,Role,ClickTime,"
				  "TimeToGenerate,TimeToSend,IP)"
				" VALUES"
				" (%ld,%ld,%ld,%ld,%ld,%ld,"
				  "%ld,%u,NOW(),"
				  "%ld,%ld,'%s')",
				ActCod,
				Gbl.Hierarchy.Cty.CtyCod,
				Gbl.Hierarchy.Ins.InsCod,
				Gbl.Hierarchy.Ctr.CtrCod,
				Gbl.Hierarchy.Deg.DegCod,
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				(unsigned) RoleToStore,
				Gbl.TimeGenerationInMicroseconds,
				Gbl.TimeSendInMicroseconds,
				Gbl.IP);
  }

/*****************************************************************************/
/************************* Log access in recent log **************************/
/*****************************************************************************/

void Log_DB_LogAccessInRecentLog (long LogCod,long ActCod,Rol_Role_t RoleToStore)
  {
   DB_QueryINSERT ("can not log access (recent)",
		   "INSERT INTO log_recent"
	           " (LogCod,ActCod,CtyCod,InsCod,CtrCod,DegCod,CrsCod,"
	             "UsrCod,Role,ClickTime,"
	             "TimeToGenerate,TimeToSend,IP)"
                   " VALUES"
                   " (%ld,%ld,%ld,%ld,%ld,%ld,%ld,"
                     "%ld,%u,NOW(),"
                     "%ld,%ld,'%s')",
		   LogCod,
		   ActCod,
		   Gbl.Hierarchy.Cty.CtyCod,
		   Gbl.Hierarchy.Ins.InsCod,
		   Gbl.Hierarchy.Ctr.CtrCod,
		   Gbl.Hierarchy.Deg.DegCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) RoleToStore,
		   Gbl.TimeGenerationInMicroseconds,
		   Gbl.TimeSendInMicroseconds,
		   Gbl.IP);
   }

/*****************************************************************************/
/*********************************** Log comments ****************************/
/*****************************************************************************/

void Log_DB_LogComments (long LogCod,const char *CommentsDB)
  {
   DB_QueryINSERT ("can not log access (comments)",
		   "INSERT INTO log_comments"
		   " (LogCod,Comments)"
		   " VALUES"
		   " (%ld,'%s')",
		   LogCod,
		   CommentsDB);
  }

/*****************************************************************************/
/****************************** Log search string ****************************/
/*****************************************************************************/

void Log_DB_LogSearchString (long LogCod)
  {
   DB_QueryINSERT ("can not log access (search)",
		   "INSERT INTO log_search"
		   " (LogCod,SearchStr)"
		   " VALUES"
		   " (%ld,'%s')",
		   LogCod,
		   Gbl.Search.Str);
  }

/*****************************************************************************/
/***************** Log web service plugin and API function *******************/
/*****************************************************************************/

void Log_DB_LogAPI (long LogCod)
  {
   DB_QueryINSERT ("can not log access (API)",
		   "INSERT INTO log_api"
		   " (LogCod,PlgCod,FunCod)"
		   " VALUES"
		   " (%ld,%ld,%u)",
		   LogCod,
		   Gbl.WebService.PlgCod,
		   (unsigned) Gbl.WebService.Function);
  }

/*****************************************************************************/
/********************************* Log banner ********************************/
/*****************************************************************************/

void Log_DB_LogBanner (long LogCod,long BanCodClicked)
  {
   DB_QueryINSERT ("can not log banner clicked",
		   "INSERT INTO log_banners"
		   " (LogCod,BanCod)"
		   " VALUES"
		   " (%ld,%ld)",
		   LogCod,
		   BanCodClicked);
  }

/*****************************************************************************/
/****************************** Get last clicks ******************************/
/*****************************************************************************/

unsigned Log_DB_GetLastClicks (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get last clicks",
		   "SELECT LogCod,"			// row[0]
			  "ActCod,"			// row[1]
			  "UNIX_TIMESTAMP()-"
			  "UNIX_TIMESTAMP(ClickTime),"	// row[2]
			  "Role,"			// row[3]
			  "CtyCod,"			// row[4]
			  "InsCod,"			// row[5]
			  "CtrCod,"			// row[6]
			  "DegCod"			// row[7]
		    " FROM log_recent"
		   " ORDER BY LogCod DESC"
		   " LIMIT 20");
  }

/*****************************************************************************/
/*** Get first click of a user from log table and store in user's figures ****/
/*****************************************************************************/

unsigned Log_DB_GetUsrFirstClick (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's first click",
		   "SELECT UNIX_TIMESTAMP((SELECT MIN(ClickTime)"
					   " FROM log"
					  " WHERE UsrCod=%ld))",
		   UsrCod);
  }

/*****************************************************************************/
/********************** Get number of clicks of a user ***********************/
/*****************************************************************************/

unsigned Log_DB_GetUsrNumClicks (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of clicks",
		  "SELECT COUNT(*)"
		   " FROM log"
		  " WHERE UsrCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/************ Sometimes, we delete old entries in recent log table ***********/
/*****************************************************************************/

void Log_DB_RemoveOldEntriesRecentLog (void)
  {
   /***** Remove all expired clipboards *****/
   DB_QueryDELETE ("can not remove old entries from recent log",
		   "DELETE LOW_PRIORITY FROM log_recent"
                   " WHERE ClickTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Log_SECONDS_IN_RECENT_LOG);
  }
