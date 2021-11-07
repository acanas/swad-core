

// swad_API.c: SWAD web API provided to external plugins

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

// #include <dirent.h>		// For scandir, etc.
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
// #include <string.h>
// #include <stdsoap2.h>
// #include <sys/stat.h>		// For lstat

// #include "soap/soapH.h"		// gSOAP header
// #include "soap/swad.nsmap"	// Namespaces map used

// #include "swad_account.h"
#include "swad_API_database.h"
// #include "swad_attendance_database.h"
// #include "swad_browser.h"
// #include "swad_browser_database.h"
#include "swad_config.h"
#include "swad_database.h"
// #include "swad_error.h"
// #include "swad_forum.h"
// #include "swad_global.h"
// #include "swad_group_database.h"
// #include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_ID.h"
// #include "swad_match.h"
// #include "swad_nickname_database.h"
// #include "swad_notice.h"
// #include "swad_notification.h"
// #include "swad_password.h"
// #include "swad_plugin_database.h"
// #include "swad_question_database.h"
// #include "swad_role.h"
// #include "swad_room_database.h"
// #include "swad_search.h"
// #include "swad_session_database.h"
// #include "swad_test_config.h"
// #include "swad_test_visibility.h"
// #include "swad_user.h"
// #include "swad_user_database.h"
// #include "swad_xml.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

// extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Insert API key *******************************/
/*****************************************************************************/

void API_DB_CreateAPIKey (char APIKey[API_BYTES_KEY + 1],long UsrCod,long PlgCod)
  {
   DB_QueryINSERT ("can not insert new key",
		   "INSERT INTO api_keys"
	           " (WSKey,UsrCod,PlgCod,LastTime)"
                   " VALUES"
                   " ('%s',%ld,%ld,NOW())",
		   APIKey,
		   UsrCod,
		   PlgCod);
  }

/*****************************************************************************/
/********************** Check API key exists in database *********************/
/*****************************************************************************/

bool API_DB_CheckIfAPIKeyExists (char APIKey[API_BYTES_KEY + 1])
  {
   return
   DB_QueryEXISTS ("can not get existence of key",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM api_keys"
		    " WHERE WSKey='%s')",
		   APIKey);
  }

/*****************************************************************************/
/************************** Get data from an API key *************************/
/*****************************************************************************/

unsigned API_DB_GetDataFromAPIKey (MYSQL_RES **mysql_res,char APIKey[API_BYTES_KEY + 1])
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not data from API key",
		   "SELECT UsrCod,"	// row[0]
			  "PlgCod"	// row[1]
		    " FROM api_keys"
		   " WHERE WSKey='%s'",
		   APIKey);
  }

/*****************************************************************************/
/***************************** Remove old API keys ***************************/
/*****************************************************************************/

void API_DB_RemoveOldAPIKeys (void)
  {
   /***** Remove expired API keys *****/
   /* A session expire when last click (LastTime) is too old */
   DB_QueryDELETE ("can not remove old API keys",
		   "DELETE LOW_PRIORITY FROM api_keys"
	           " WHERE LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_API_KEY);
  }
