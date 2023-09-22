// swad_log_database.c: access log, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_parameter.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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
				Gbl.Hierarchy.Node[Hie_CTY].HieCod,
				Gbl.Hierarchy.Node[Hie_INS].HieCod,
				Gbl.Hierarchy.Node[Hie_CTR].HieCod,
				Gbl.Hierarchy.Node[Hie_DEG].HieCod,
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				(unsigned) RoleToStore,
				Dat_GetTimeGenerationInMicroseconds (),
				Dat_GetTimeSendInMicroseconds (),
				Par_GetIP ());
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
		   Gbl.Hierarchy.Node[Hie_CTY].HieCod,
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   Gbl.Hierarchy.Node[Hie_CTR].HieCod,
		   Gbl.Hierarchy.Node[Hie_DEG].HieCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) RoleToStore,
		   Dat_GetTimeGenerationInMicroseconds (),
		   Dat_GetTimeSendInMicroseconds (),
		   Par_GetIP ());
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

void Log_DB_LogSearchString (long LogCod,const struct Sch_Search *Search)
  {
   DB_QueryINSERT ("can not log access (search)",
		   "INSERT INTO log_search"
		   " (LogCod,SearchStr)"
		   " VALUES"
		   " (%ld,'%s')",
		   LogCod,Search->Str);
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
/********************** Get my clicks grouped by action **********************/
/*****************************************************************************/

unsigned Log_DB_GetMyClicksGroupedByAction (MYSQL_RES **mysql_res,
                                            time_t FirstClickTimeUTC,
                                            unsigned MaxActions)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get clicks",
		   "SELECT SQL_NO_CACHE ActCod,"	// row[0]
				       "COUNT(*) AS N"	// row[1]
		    " FROM log"
		   " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
		     " AND UsrCod=%ld"
		" GROUP BY ActCod"
		" ORDER BY N DESC"
		   " LIMIT %u",
		   (long) FirstClickTimeUTC,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   MaxActions);
  }

/*****************************************************************************/
/************ Get my maximum number of hits per course-year-role ************/
/*****************************************************************************/

unsigned Log_DB_GetMyMaxHitsPerYear (MYSQL_RES **mysql_res,
                                     time_t FirstClickTimeUTC)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get last question index",
		   "SELECT MAX(N) FROM ("
		   // Clicks without course selected --------------------------
	           "SELECT -1 AS CrsCod,"
	                  "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"
	                  "%u AS Role,"
	                  "COUNT(*) AS N"
	            " FROM log"
	           " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
	             " AND UsrCod=%ld"
	             " AND CrsCod<=0"
	        " GROUP BY Year"
		   // ---------------------------------------------------------
	           " UNION "
		   // Clicks as student, non-editing teacher or teacher in courses
	           "SELECT CrsCod,"
	                  "YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"
	                  "Role,"
	                  "COUNT(*) AS N"
	            " FROM log"
	           " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
	             " AND UsrCod=%ld"
	             " AND Role>=%u"	// Student
	             " AND Role<=%u"	// Teacher
	             " AND CrsCod>0"
	        " GROUP BY CrsCod,"
	                  "Year,"
	                  "Role"
		   // ---------------------------------------------------------
	           ") AS hits_per_crs_year",
		   (unsigned) Rol_UNK,
		   (long) FirstClickTimeUTC,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (long) FirstClickTimeUTC,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Rol_STD,
		   (unsigned) Rol_TCH);
  }

/*****************************************************************************/
/************ Get my courses with number of hits per course-role ************/
/*****************************************************************************/

unsigned Log_DB_GetMyCrssAndHitsPerCrs (MYSQL_RES **mysql_res,Rol_Role_t Role)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get courses of a user",
		   "SELECT my_courses.CrsCod,"		// row[0]
			  "COUNT(*) AS N"			// row[1]
		    " FROM (SELECT CrsCod"
			    " FROM crs_users"
			   " WHERE UsrCod=%ld"
			     " AND Role=%u) AS my_courses"	// It's imperative to use a derived table to not block crs_usr!
		    " LEFT JOIN log"
		      " ON my_courses.CrsCod=log.CrsCod"
		   " WHERE log.UsrCod=%ld"
		     " AND log.Role=%u"
		" GROUP BY my_courses.CrsCod"
		" ORDER BY N DESC,"
			  "my_courses.CrsCod DESC",
		   Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) Role,
		   Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) Role);
  }

/*****************************************************************************/
/************************** Get my historic courses **************************/
/*****************************************************************************/

unsigned Log_DB_GetMyHistoricCrss (MYSQL_RES **mysql_res,
                                   Rol_Role_t Role,unsigned MinClicksCrs)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get courses of a user",
		   "SELECT CrsCod,"		// row[0]
			  "COUNT(*) AS N"	// row[1]
		    " FROM log"
		   " WHERE UsrCod=%ld"
		     " AND Role=%u"
		     " AND CrsCod>0"
	        " GROUP BY CrsCod"
		  " HAVING N>%u"
		   " ORDER BY N DESC",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Role,
		   MinClicksCrs);
  }

/*****************************************************************************/
/********************** Write my hits grouped by years ***********************/
/*****************************************************************************/

unsigned Log_DB_GetMyHitsPerYear (MYSQL_RES **mysql_res,
                                  bool AnyCourse,long CrsCod,Rol_Role_t Role,
                                  time_t FirstClickTimeUTC)
  {
   char SubQueryCrs[128];
   char SubQueryRol[128];

   if (AnyCourse)
      SubQueryCrs[0] = '\0';
   else
      sprintf (SubQueryCrs," AND CrsCod=%ld",CrsCod);

   if (Role == Rol_UNK)	// Here Rol_UNK means any role
      SubQueryRol[0] = '\0';
   else
      sprintf (SubQueryRol," AND Role=%u",(unsigned) Role);

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get clicks",
		   "SELECT SQL_NO_CACHE YEAR(CONVERT_TZ(ClickTime,@@session.time_zone,'UTC')) AS Year,"	// row[0]
				       "COUNT(*)"							// row[1]
		    " FROM log"
		   " WHERE ClickTime>=FROM_UNIXTIME(%ld)"
		     " AND UsrCod=%ld"
		       "%s"
		       "%s"
	        " GROUP BY Year"
		" ORDER BY Year DESC",
		   (long) FirstClickTimeUTC,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SubQueryCrs,
		   SubQueryRol);
  }

/*****************************************************************************/
/********************* Get the comments of a hit from log ********************/
/*****************************************************************************/

void Log_DB_GetLogComments (long LogCod,char Comments[Cns_MAX_BYTES_TEXT + 1])
  {
   DB_QuerySELECTString (Comments,Cns_MAX_BYTES_TEXT,
                         "can not get log comments",
			 "SELECT Comments"
			  " FROM log_comments"
			 " WHERE LogCod=%ld",
			 LogCod);
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
