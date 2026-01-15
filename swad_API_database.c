

// swad_API.c: SWAD web API provided to external plugins

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_API_database.h"
#include "swad_config.h"
#include "swad_database.h"

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

Exi_Exist_t API_DB_CheckIfAPIKeyExists (char APIKey[API_BYTES_KEY + 1])
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

Exi_Exist_t API_DB_GetDataFromAPIKey (MYSQL_RES **mysql_res,
				      char APIKey[API_BYTES_KEY + 1])
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not data from API key",
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
