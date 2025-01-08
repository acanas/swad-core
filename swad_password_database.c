// swad_password_database.c: Users' passwords, operations with database

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_password_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************** Update my current pending password in database ***************/
/*****************************************************************************/

void Pwd_DB_UpdateMyPendingPassword (void)
  {
   DB_QueryREPLACE ("can not create pending password",
		    "REPLACE INTO usr_pending_passwd"
		    " (UsrCod,PendingPassword,DateAndTime)"
		    " VALUES"
		    " (%ld,'%s',NOW())",
                    Gbl.Usrs.Me.UsrDat.UsrCod,
                    Gbl.Usrs.Me.PendingPassword);
  }

/*****************************************************************************/
/******************* Update my current password in database ******************/
/*****************************************************************************/

void Pwd_DB_AssignMyPendingPasswordToMyCurrentPassword (void)
  {
   DB_QueryUPDATE ("can not update your password",
		   "UPDATE usr_data"
		     " SET Password='%s'"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.PendingPassword,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************* Get pending password from database **********************/
/*****************************************************************************/

void Pwd_DB_GetPendingPassword (void)
  {
   DB_QuerySELECTString (Gbl.Usrs.Me.PendingPassword,
                         sizeof (Gbl.Usrs.Me.PendingPassword) - 1,
                         "can not get pending password",
		         "SELECT PendingPassword"	// row[0]
		          " FROM usr_pending_passwd"
		         " WHERE UsrCod=%ld",
		         Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Get the number of users who use yet a password ***************/
/*****************************************************************************/

unsigned Pwd_DB_GetNumOtherUsrsWhoUseThisPassword (const char *EncryptedPassword,long UsrCod)
  {
   unsigned NumUsrs;
   char *SubQuery;

   /***** Build subquery *****/
   if (UsrCod > 0)
     {
      if (asprintf (&SubQuery," AND UsrCod<>%ld",UsrCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
      SubQuery = "";

   /***** Get number of other users who use a password from database *****/
   NumUsrs = (unsigned)
   DB_QueryCOUNT ("can not check if a password is trivial",
		  "SELECT COUNT(*)"
		   " FROM usr_data"
		  " WHERE Password='%s'"
		     "%s",
		  EncryptedPassword,
		  SubQuery);

   /***** Free subquery *****/
   if (UsrCod > 0)
      free (SubQuery);

   return NumUsrs;
  }

/*****************************************************************************/
/***************** Remove my pending password from database ******************/
/*****************************************************************************/

void Pwd_DB_RemoveMyPendingPassword (void)
  {
   DB_QueryDELETE ("can not remove pending password",
		   "DELETE FROM usr_pending_passwd"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************* Remove expired pending passwords from database ****************/
/*****************************************************************************/

void Pwd_DB_RemoveExpiredPendingPassword (void)
  {
   DB_QueryDELETE ("can not remove expired pending passwords",
		   "DELETE LOW_PRIORITY FROM usr_pending_passwd"
		   " WHERE DateAndTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_OLD_PENDING_PASSWORDS);
  }
