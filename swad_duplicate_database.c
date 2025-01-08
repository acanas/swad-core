// swad_duplicate_database.c: duplicate users operations with database

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
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************* Get list of possible duplicate users ********************/
/*****************************************************************************/

unsigned Dup_DB_GetListDuplicateUsrs (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get possibly duplicate users",
		   "SELECT UsrCod,"					// row[0]
			  "COUNT(*) AS N,"				// row[1]
			  "UNIX_TIMESTAMP(MIN(InformTime)) AS T"	// row[2]
		    " FROM usr_duplicated"
		" GROUP BY UsrCod"
		" ORDER BY N DESC,"
			  "T DESC");
  }

/*****************************************************************************/
/***************** Get list of users similar to a given one ******************/
/*****************************************************************************/

unsigned Dup_DB_GetUsrsSimilarTo (MYSQL_RES **mysql_res,const struct Usr_Data *UsrDat)
  {
   if (UsrDat->Surname1[0] &&
       UsrDat->FrstName[0])	// Name and surname 1 not empty
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not get similar users",
		      "SELECT DISTINCT "
		             "UsrCod"
		       " FROM usr_ids"
		      " WHERE UsrID IN"
			    " (SELECT UsrID"
			       " FROM usr_ids"
			      " WHERE UsrCod=%ld)"
		      " UNION DISTINCT"
		     " SELECT UsrCod"
		       " FROM usr_data"
		      " WHERE Surname1='%s'"
			" AND Surname2='%s'"
			" AND FirstName='%s'",
		      UsrDat->UsrCod,
		      UsrDat->Surname1,
		      UsrDat->Surname2,
		      UsrDat->FrstName);

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get similar users",
		   "SELECT DISTINCT "
		          "UsrCod"
		    " FROM usr_ids"
		   " WHERE UsrID IN"
			 " (SELECT UsrID"
			    " FROM usr_ids"
			   " WHERE UsrCod=%ld)",
		   UsrDat->UsrCod);
  }

/*****************************************************************************/
/********** Check if a user is in list of possible duplicate users ***********/
/*****************************************************************************/

bool Dup_DB_CheckIfUsrIsDup (long UsrCod)
  {
   return
   DB_QueryEXISTS ("can not check if user is in list of possible duplicate users",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_duplicated"
		    " WHERE UsrCod=%ld)",
		   UsrCod);
  }

/*****************************************************************************/
/******************* Insert possible duplicate into database *****************/
/*****************************************************************************/

void Dup_DB_AddUsrToDuplicated (long UsrCod)
  {
   DB_QueryREPLACE ("can not report duplicate",
		    "REPLACE INTO usr_duplicated"
		    " (UsrCod,InformerCod,InformTime)"
		    " VALUES"
		    " (%ld,%ld,NOW())",
		    UsrCod,
		    Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******* Remove user from list of possible duplicate users in database *******/
/*****************************************************************************/

void Dup_DB_RemoveUsrFromDuplicated (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from possible duplicates",
		   "DELETE FROM usr_duplicated"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
