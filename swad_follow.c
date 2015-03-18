// swad_follow.c: user's followers and followed

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <stdio.h>		// For sprintf

#include "swad_bool.h"
#include "swad_database.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_profile.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/*************** Check if a user is a follower of another user ***************/
/*****************************************************************************/

bool Fol_CheckUsrIsFollowerOf (long FollowerCod,long FollowedCod)
  {
   char Query[256];

   if (FollowerCod == FollowedCod)
      return false;

   /***** Check if a user is a follower of another user *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_follow"
	          " WHERE FollowerCod='%ld' AND FollowedCod='%ld'",
            FollowerCod,FollowedCod);
   return (DB_QueryCOUNT (Query,"can not get if a user is a follower of another one") != 0);
  }


/*****************************************************************************/
/***************************** Follow another user ***************************/
/*****************************************************************************/

void Fol_FollowUsr (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   bool Error;

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (!Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
                                     Gbl.Usrs.Other.UsrDat.UsrCod))
	{
	 /***** Follow user in database *****/
	 sprintf (Query,"REPLACE INTO usr_follow"
	                " (FollowerCod,FollowedCod,FollowTime)"
			" VALUES ('%ld','%ld',NOW())",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
                  Gbl.Usrs.Other.UsrDat.UsrCod);
	 DB_QueryREPLACE (Query,"can not follow user");
        }

      /***** Show user's profile again *****/
      Error = !Prf_ShowUserProfile ();
     }
   else
      Error = true;

   if (Error)
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/***************************** Unfollow another user *************************/
/*****************************************************************************/

void Fol_UnfollowUsr (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   bool Error;

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
                                    Gbl.Usrs.Other.UsrDat.UsrCod))
	{
	 /***** Follow user in database *****/
	 sprintf (Query,"DELETE FROM usr_follow"
	                " WHERE FollowerCod='%ld' AND FollowedCod='%ld'",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
                  Gbl.Usrs.Other.UsrDat.UsrCod);
	 DB_QueryREPLACE (Query,"can not unfollow user");
        }

      /***** Show user's profile again *****/
      Error = !Prf_ShowUserProfile ();
     }
   else
      Error = true;

   if (Error)
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }
