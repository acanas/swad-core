// swad_ID_database.c: Users' IDs operations with database

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

#include <stdlib.h>		// For free

#include "swad_database.h"
#include "swad_error.h"
#include "swad_ID.h"
#include "swad_ID_database.h"

/*****************************************************************************/
/*************************** Create new user's ID ****************************/
/*****************************************************************************/

void ID_DB_InsertANewUsrID (long UsrCod,
		            const char ID[ID_MAX_BYTES_USR_ID + 1],
		            ID_Confirmed_t Confirmed)
  {
   static char YN[ID_NUM_CONFIRMED] =
     {
      [ID_NOT_CONFIRMED] = 'N',
      [ID_CONFIRMED    ] = 'Y',
     };

   DB_QueryINSERT ("can not create user's ID",
		   "INSERT INTO usr_ids"
		   " (UsrCod,UsrID,CreatTime,Confirmed)"
		   " VALUES"
		   " (%ld,'%s',NOW(),'%c')",
		   UsrCod,
		   ID,
		   YN[Confirmed]);
  }

/*****************************************************************************/
/*********************** Set a user's ID as confirmed ************************/
/*****************************************************************************/

void ID_DB_ConfirmUsrID (long UsrCod,const char ID[ID_MAX_BYTES_USR_ID + 1])
  {
   DB_QueryUPDATE ("can not confirm a user's ID",
		   "UPDATE usr_ids"
		     " SET Confirmed='Y'"
		   " WHERE UsrCod=%ld"
		     " AND UsrID='%s'"
		     " AND Confirmed<>'Y'",
                   UsrCod,
                   ID);
  }

/*****************************************************************************/
/********************** Get list of IDs of a user ****************************/
/*****************************************************************************/

unsigned ID_DB_GetIDsFromUsrCod (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's IDs",
		   "SELECT UsrID,"	// row[0]
			  "Confirmed"	// row[1]
		    " FROM usr_ids"
		   " WHERE UsrCod=%ld"
		" ORDER BY Confirmed DESC,"
			  "UsrID",
		   UsrCod);
  }

/*****************************************************************************/
/************************ Check if an ID is confirmed ************************/
/*****************************************************************************/

bool ID_DB_CheckIfConfirmed (long UsrCod,const char ID[ID_MAX_BYTES_USR_ID + 1])
  {
   return
   DB_QueryEXISTS ("can not check if ID is confirmed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_ids"
		    " WHERE UsrCod=%ld"
		      " AND UsrID='%s'"
		      " AND Confirmed='Y')",
		   UsrCod,
		   ID) == Exi_EXISTS;
  }

/*****************************************************************************/
/********************** Check if a string is a user's ID *********************/
/*****************************************************************************/

Exi_Exist_t ID_DB_FindStrInUsrsIDs (const char *Str)
  {
   return
   DB_QueryEXISTS ("can not check if a string matches any user's ID",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_ids"
		    " WHERE UsrID='%s')",
		   Str);
  }

/*****************************************************************************/
/***************** Get list of user codes from user's IDs ********************/
/*****************************************************************************/
// Returns the number of users with any of these IDs

unsigned ID_DB_GetUsrCodsFromUsrID (MYSQL_RES **mysql_res,
                                    const struct Usr_Data *UsrDat,
                                    const char *EncryptedPassword,	// If NULL or empty ==> do not check password
                                    ID_OnlyConfirmed_t OnlyConfirmedIDs)
  {
   static const char *SubqueryConfirmedIDs[ID_NUM_ONLY_CONFIRMED] =
     {
      [ID_ANY           ] = "",
      [ID_ONLY_CONFIRMED] = " AND usr_ids.Confirmed='Y'",
     };
   char *SubQueryAllUsrs = NULL;
   char SubQueryOneUsr[1 + ID_MAX_BYTES_USR_ID + 1 + 1];
   size_t MaxLength;
   unsigned NumID;
   unsigned NumUsrs;
   bool CheckPassword = false;

   if (EncryptedPassword)
      if (EncryptedPassword[0])
	 CheckPassword = true;

   /***** Allocate memory for subquery string *****/
   MaxLength = 512 + UsrDat->IDs.Num * (1 + ID_MAX_BYTES_USR_ID + 1) - 1;
   if ((SubQueryAllUsrs = malloc (MaxLength + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   SubQueryAllUsrs[0] = '\0';

   /***** Get user's code(s) from database *****/
   for (NumID = 0;
	NumID < UsrDat->IDs.Num;
	NumID++)
     {
      if (NumID)
	 Str_Concat (SubQueryAllUsrs,",",MaxLength);
      sprintf (SubQueryOneUsr,"'%s'",UsrDat->IDs.List[NumID].ID);

      Str_Concat (SubQueryAllUsrs,SubQueryOneUsr,MaxLength);
     }

   if (CheckPassword)
     {
      // Get user's code if I have written the correct password
      // or if password in database is empty (new user)
      NumUsrs = (unsigned)
      DB_QuerySELECT (mysql_res,"can not get user's codes",
		      "SELECT DISTINCT "
		             "usr_ids.UsrCod"
		       " FROM usr_ids,"
			     "usr_data"
		      " WHERE usr_ids.UsrID IN (%s)"
			  "%s"
			" AND usr_ids.UsrCod=usr_data.UsrCod"
			" AND (usr_data.Password='%s'"
			  " OR usr_data.Password='')",
		      SubQueryAllUsrs,
		      SubqueryConfirmedIDs[OnlyConfirmedIDs],
		      EncryptedPassword);
     }
   else
      NumUsrs = (unsigned)
      DB_QuerySELECT (mysql_res,"can not get user's codes",
		      "SELECT DISTINCT "
		             "UsrCod"
		       " FROM usr_ids"
		      " WHERE UsrID IN (%s)"
			  "%s",
		      SubQueryAllUsrs,
		      SubqueryConfirmedIDs[OnlyConfirmedIDs]);

   /***** Free memory for subquery string *****/
   free (SubQueryAllUsrs);

   return NumUsrs;
  }

/*****************************************************************************/
/**************** Remove one of my user's IDs from database ******************/
/*****************************************************************************/

void ID_DB_RemoveUsrID (long UsrCod,const char ID[ID_MAX_BYTES_USR_ID + 1])
  {
   DB_QueryREPLACE ("can not remove a user's ID",
		    "DELETE FROM usr_ids"
		    " WHERE UsrCod=%ld"
		      " AND UsrID='%s'",
                    UsrCod,
                    ID);
  }

/*****************************************************************************/
/****************************** Remove user's IDs ****************************/
/*****************************************************************************/

void ID_DB_RemoveUsrIDs (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's IDs",
		   "DELETE FROM usr_ids"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
