// swad_photo_database.c: Users' photos management, operations with database

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

// #define _GNU_SOURCE 		// For asprintf
// #include <linux/limits.h>	// For PATH_MAX
// #include <math.h>		// For log10, floor, ceil, modf, sqrt...
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For system, getenv, etc.
// #include <string.h>		// For string functions
// #include <sys/wait.h>	// For the macro WEXITSTATUS
// #include <unistd.h>		// For unlink

// #include "swad_action.h"
// #include "swad_box.h"
// #include "swad_browser.h"
// #include "swad_config.h"
#include "swad_database.h"
// #include "swad_enrolment.h"
// #include "swad_error.h"
// #include "swad_file.h"
// #include "swad_follow.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_logo.h"
// #include "swad_parameter.h"
// #include "swad_photo.h"
#include "swad_photo_database.h"
// #include "swad_privacy.h"
// #include "swad_setting.h"
// #include "swad_statistic.h"
// #include "swad_theme.h"
// #include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

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
/****************** Clear photo name of an user in database ******************/
/*****************************************************************************/

void Pho_DB_ClearPhotoName (long UsrCod)
  {
   DB_QueryUPDATE ("can not clear the name of a user's photo",
		   "UPDATE usr_data"
		     " SET Photo=''"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/***************** Update photo name of an user in database ******************/
/*****************************************************************************/

void Pho_DB_UpdatePhotoName (long UsrCod,
                             const char UniqueNameEncrypted[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   DB_QueryUPDATE ("can not update the name of a user's photo",
		   "UPDATE usr_data"
		     " SET Photo='%s'"
		   " WHERE UsrCod=%ld",
                   UniqueNameEncrypted,
                   UsrCod);
  }

/*****************************************************************************/
/*************** Initialize my number of clicks without photo ****************/
/*****************************************************************************/

void Pho_DB_InitMyClicksWithoutPhoto (void)
  {
   DB_QueryINSERT ("can not create number of clicks without photo",
		   "INSERT INTO usr_clicks_without_photo"
		   " (UsrCod,NumClicks)"
		   " VALUES"
		   " (%ld,1)",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************** Increment my number of clicks without photo *****************/
/*****************************************************************************/

void Pho_DB_IncrMyClicksWithoutPhoto (void)
  {
   DB_QueryUPDATE ("can not update number of clicks without photo",
		   "UPDATE usr_clicks_without_photo"
		     " SET NumClicks=NumClicks+1"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************ Get number of clicks without photo from database ***************/
/*****************************************************************************/

unsigned Pho_DB_GetMyClicksWithoutPhoto (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get number of clicks without photo",
		   "SELECT NumClicks"		// row[0]
		    " FROM usr_clicks_without_photo"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******** Remove user from table with number of clicks without photo *********/
/*****************************************************************************/

void Pho_DB_RemoveUsrFromTableClicksWithoutPhoto (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from the list of users without photo",
		   "DELETE FROM usr_clicks_without_photo"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*********************** Update statistics of a degree ***********************/
/*****************************************************************************/

void Pho_DB_UpdateDegStats (long DegCod,Usr_Sex_t Sex,
			    unsigned NumStds,unsigned NumStdsWithPhoto,
			    long TimeToComputeAvgPhotoInMicroseconds)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];

   DB_QueryREPLACE ("can not save stats of a degree",
		    "REPLACE INTO sta_degrees"
		    " (DegCod,Sex,NumStds,NumStdsWithPhoto,"
		    "TimeAvgPhoto,TimeToComputeAvgPhoto)"
		    " VALUES"
		    " (%ld,'%s',%u,%u,NOW(),%ld)",
	            DegCod,
	            Usr_StringsSexDB[Sex],
	            NumStds,
	            NumStdsWithPhoto,
		    TimeToComputeAvgPhotoInMicroseconds);
  }

/*****************************************************************************/
/****** Build a query to get the degrees ordered by different criteria *******/
/*****************************************************************************/

unsigned Pho_DB_QueryDegrees (MYSQL_RES **mysql_res,
                              Pho_HowOrderDegrees_t HowOrderDegrees)
  {
   switch (HowOrderDegrees)
     {
      case Pho_NUMBER_OF_STUDENTS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get degrees",
		         "SELECT deg_degrees.DegCod"
			  " FROM deg_degrees,"
			        "sta_degrees"
		         " WHERE sta_degrees.Sex='all'"
			   " AND sta_degrees.NumStds>0"
			   " AND deg_degrees.DegCod=sta_degrees.DegCod"
		         " ORDER BY sta_degrees.NumStds DESC,"
				   "sta_degrees.NumStdsWithPhoto DESC,"
				   "deg_degrees.ShortName");
      case Pho_NUMBER_OF_PHOTOS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get degrees",
		         "SELECT deg_degrees.DegCod"
			  " FROM deg_degrees,"
			        "sta_degrees"
		         " WHERE sta_degrees.Sex='all'"
			   " AND sta_degrees.NumStds>0"
			   " AND deg_degrees.DegCod=sta_degrees.DegCod"
		         " ORDER BY sta_degrees.NumStdsWithPhoto DESC,"
				   "sta_degrees.NumStds DESC,"
				   "deg_degrees.ShortName");
      case Pho_PERCENT:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get degrees",
		         "SELECT deg_degrees.DegCod"
			  " FROM deg_degrees,"
			        "sta_degrees"
		         " WHERE sta_degrees.Sex='all'"
			   " AND sta_degrees.NumStds>0"
			   " AND deg_degrees.DegCod=sta_degrees.DegCod"
		         " ORDER BY sta_degrees.NumStdsWithPhoto/"
				   "sta_degrees.NumStds DESC,"
				   "deg_degrees.ShortName");
      case Pho_DEGREE_NAME:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get degrees",
		         "SELECT deg_degrees.DegCod"
			  " FROM deg_degrees,"
			        "sta_degrees"
		         " WHERE sta_degrees.Sex='all'"
			   " AND sta_degrees.NumStds>0"
			   " AND deg_degrees.DegCod=sta_degrees.DegCod"
		         " ORDER BY deg_degrees.ShortName");
     }

   return 0;
  }

/*****************************************************************************/
/* Delete all the degrees in sta_degrees table not present in degrees table **/
/*****************************************************************************/

void Pho_DB_RemoveObsoleteStatDegrees (void)
  {
   DB_QueryDELETE ("can not remove old degrees from stats",
		   "DELETE FROM sta_degrees"
		   " WHERE DegCod NOT IN"
		         " (SELECT DegCod"
		            " FROM deg_degrees)");
  }
