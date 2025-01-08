// swad_report_database.c: report on my use of the platform, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_report.h"
#include "swad_report_database.h"

/*****************************************************************************/
/************** Insert a new user's usage report into database ***************/
/*****************************************************************************/

void Rep_DB_CreateNewReport (long UsrCod,const struct Rep_Report *Report,
                             const char UniqueNameEncrypted[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   /***** Insert a new user's usage report into database *****/
   DB_QueryINSERT ("can not create new user's usage report",
		   "INSERT INTO usr_reports"
	           " (UsrCod,ReportTimeUTC,"
	             "UniqueDirL,UniqueDirR,Filename,Permalink)"
                   " VALUES"
                   " (%ld,'%04d-%02d-%02d %02d:%02d:%02d',"
                     "'%c%c','%s','%s','%s')",
		   UsrCod,
		   1900 + Report->tm_CurrentTime.tm_year,	// year
		   1 +  Report->tm_CurrentTime.tm_mon,		// month
		   Report->tm_CurrentTime.tm_mday,		// day of the month
		   Report->tm_CurrentTime.tm_hour,		// hours
		   Report->tm_CurrentTime.tm_min,		// minutes
		   Report->tm_CurrentTime.tm_sec,		// seconds
		   UniqueNameEncrypted[0],		//  2  leftmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
		   UniqueNameEncrypted[1],
		   &UniqueNameEncrypted[2],		// 41 rightmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
		   Report->FilenameReport,
		   Report->Permalink);
  }

/*****************************************************************************/
/********************** Get directories for the reports **********************/
/*****************************************************************************/

unsigned Rep_DB_GetUsrReportsFiles (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's usage reports",
		   "SELECT UniqueDirL,"	// row[0]
			  "UniqueDirR"	// row[1]
		    " FROM usr_reports"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/********** Remove all user's usage reports of a user from database **********/
/*****************************************************************************/

void Rep_DB_RemoveUsrReports (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's usage reports",
		   "DELETE FROM usr_reports"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
