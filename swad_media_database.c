// swad_media_database.c: processing of image/video operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_media.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Med_DB_StrTypes[Med_NUM_TYPES] =
  {
   [Med_TYPE_NONE] = "none",
   [Med_JPG      ] = "jpg",
   [Med_GIF      ] = "gif",
   [Med_MP4      ] = "mp4",
   [Med_WEBM     ] = "webm",
   [Med_OGG      ] = "ogg",
   [Med_YOUTUBE  ] = "youtube",
   [Med_EMBED    ] = "embed",
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static const char *Med_DB_GetStringType (Med_Type_t Type);

/*****************************************************************************/
/**** Get media name, title and URL from a query result and copy to struct ***/
/*****************************************************************************/

Exi_Exist_t Med_DB_GetMediaDataByCod (MYSQL_RES **mysql_res,long MedCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get media",
			 "SELECT Type,"	// row[0]
				"Name,"	// row[1]
				"URL,"	// row[2]
				"Title"	// row[3]
			  " FROM med_media"
			 " WHERE MedCod=%ld",
			 MedCod);
  }

/*****************************************************************************/
/************************ Store media into database **************************/
/*****************************************************************************/

long Med_DB_StoreMedia (const struct Med_Media *Media)
  {
   /***** Insert media into database *****/
   return DB_QueryINSERTandReturnCode ("can not create media",
				       "INSERT INTO med_media"
				       " (Type,Name,URL,Title)"
				       " VALUES"
				       " ('%s','%s','%s','%s')",
				       Med_DB_GetStringType (Media->Type),
				       Media->Name,
				       Media->URL   ? Media->URL   :
						      "",
				       Media->Title ? Media->Title :
						      "");
  }

/*****************************************************************************/
/************************* Remove media from database ************************/
/*****************************************************************************/

void Med_DB_RemoveMedia (long MedCod)
  {
   /***** Remove entry for this media from database *****/
   DB_QueryDELETE ("can not remove media",
		   "DELETE FROM med_media"
		   " WHERE MedCod=%ld",
		   MedCod);
  }

/*****************************************************************************/
/************************ Get media type from string *************************/
/*****************************************************************************/

Med_Type_t Med_DB_GetTypeFromStr (const char *Str)
  {
   Med_Type_t Type;

   for (Type  = (Med_Type_t) 0;
        Type <= (Med_Type_t) (Med_NUM_TYPES - 1);
        Type++)
      if (!strcasecmp (Str,Med_DB_StrTypes[Type]))
         return Type;

   return Med_TYPE_NONE;
  }

/*****************************************************************************/
/*************** Get string media type in database from type *****************/
/*****************************************************************************/

static const char *Med_DB_GetStringType (Med_Type_t Type)
  {
   /***** Check if type is out of valid range *****/
   if (Type > (Med_Type_t) (Med_NUM_TYPES - 1))
      return Med_DB_StrTypes[Med_TYPE_NONE];

   /***** Get string from type *****/
   return Med_DB_StrTypes[Type];
  }
