// swad_photo_database.c: Users' photos management, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_photo_database.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

const char Pho_DB_ShowPhotos[Pho_NUM_PHOTOS] =
  {
   [Pho_PHOTOS_UNKNOWN	] = 'Y',
   [Pho_PHOTOS_DONT_SHOW] = 'N',
   [Pho_PHOTOS_SHOW	] = 'Y',
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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

Exi_Exist_t Pho_DB_GetMyClicksWithoutPhoto (MYSQL_RES **mysql_res)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get number of clicks without photo",
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

void Pho_DB_UpdateDegStats (long HieCod,Usr_Sex_t Sex,
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
	            HieCod,
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
/******** Get degree with students not in table of computed degrees **********/
/*****************************************************************************/

long Pho_DB_GetADegWithStdsNotInTableOfComputedDegs (void)
  {
   return DB_QuerySELECTCode ("can not get degree",
			      "SELECT DISTINCT "
			             "deg_degrees.DegCod"
			       " FROM deg_degrees,"
				     "crs_courses,"
				     "crs_users"
			      " WHERE deg_degrees.DegCod=crs_courses.DegCod"
			        " AND crs_courses.CrsCod=crs_users.CrsCod"
			        " AND crs_users.Role=%u"
			        " AND deg_degrees.DegCod NOT IN"
				    " (SELECT DISTINCT "
				             "DegCod"
				       " FROM sta_degrees)"
			      " LIMIT 1",
			     (unsigned) Rol_STD);
  }
/*****************************************************************************/
/********* Get the least recently updated degree that has students ***********/
/*****************************************************************************/

long Pho_DB_GetDegWithAvgPhotoLeastRecentlyUpdated (void)
  {
   return DB_QuerySELECTCode ("can not get degree",
			      "SELECT sta_degrees.DegCod"
			       " FROM sta_degrees,"
				     "crs_courses,"
				     "crs_users"
			      " WHERE sta_degrees.TimeAvgPhoto<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)"
			        " AND sta_degrees.DegCod=crs_courses.DegCod"
			        " AND crs_courses.CrsCod=crs_users.CrsCod"
			        " AND crs_users.Role=%u"
			   " ORDER BY sta_degrees.TimeAvgPhoto"
			      " LIMIT 1",
			      Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO,
			      (unsigned) Rol_STD);
  }

/*****************************************************************************/
/********* Get maximum number of students in a degree from database **********/
/*****************************************************************************/

Exi_Exist_t Pho_DB_GetMaxStdsPerDegree (MYSQL_RES **mysql_res)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get maximum number of students in a degree",
			 "SELECT MAX(NumStds),"			// row[0]
				"MAX(NumStdsWithPhoto),"		// row[1]
				"MAX(NumStdsWithPhoto/NumStds)"	// row[2]
			  " FROM sta_degrees"
			 " WHERE Sex='all'"
			   " AND NumStds>0");
  }

/*****************************************************************************/
/*** Get number of students and number of students with photo in a degree ****/
/*****************************************************************************/

Exi_Exist_t Pho_DB_GetNumStdsInDegree (MYSQL_RES **mysql_res,long HieCod,Usr_Sex_t Sex)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];

   return
   DB_QuerySELECTunique (mysql_res,"can not get the number of students in a degree",
			 "SELECT NumStds,"		// row[0]
				"NumStdsWithPhoto"	// row[1]
			  " FROM sta_degrees"
			 " WHERE DegCod=%ld"
			   " AND Sex='%s'",
			 HieCod,
			 Usr_StringsSexDB[Sex]);
  }

/*****************************************************************************/
/********* Get last time an average photo was computed from database *********/
/*****************************************************************************/

Exi_Exist_t Pho_DB_GetTimeAvgPhotoWasComputed (MYSQL_RES **mysql_res,long HieCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get last time"
				  " an average photo was computed",
			 "SELECT COALESCE(MIN(UNIX_TIMESTAMP(TimeAvgPhoto)),0)"	// row[0]
			  " FROM sta_degrees"
			 " WHERE DegCod=%ld",
			 HieCod);
  }

/*****************************************************************************/
/********************* Get time to compute average photo *********************/
/*****************************************************************************/

unsigned Pho_DB_GetTimeToComputeAvgPhoto (MYSQL_RES **mysql_res,long HieCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get time to compute average photo",
		   "SELECT TimeToComputeAvgPhoto"	// row[0]
		    " FROM sta_degrees"
		   " WHERE DegCod=%ld",
		   HieCod);
  }

/*****************************************************************************/
/*** Delete all degrees in sta_degrees table not present in degrees table ****/
/*****************************************************************************/

void Pho_DB_RemoveObsoleteStatDegrees (void)
  {
   DB_QueryDELETE ("can not remove old degrees from stats",
		   "DELETE FROM sta_degrees"
		   " WHERE DegCod NOT IN"
		         " (SELECT DegCod"
		            " FROM deg_degrees)");
  }
