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

#define Fol_NUM_COLUMNS_FOLLOW 3

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

static unsigned Fol_GetNumFollowing (long UsrCod);
static unsigned Fol_GetNumFollowers (long UsrCod);
static void Fol_ShowFollowedOrFollowed (const struct UsrData *UsrDat);

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
/**************** Show following and followers of a user *********************/
/*****************************************************************************/

void Fol_ShowFollowingAndFollowers (const struct UsrData *UsrDat)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Following;
   extern const char *Txt_Followers;
   unsigned Following = Fol_GetNumFollowing (UsrDat->UsrCod);
   unsigned Followers = Fol_GetNumFollowers (UsrDat->UsrCod);

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_4\" style=\"margin:0 auto;\">"
	              "<tr>");

   /***** Followed users *****/
   fprintf (Gbl.F.Out,"<td style=\"min-width:100px;"
	              " text-align:center; vertical-align:top;\">"
		      "<div class=\"FOLLOW\">");
   if (Following)
     {
      /* Form to list followed users */
      Act_FormStart (ActSeeFlg);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Txt_Following,"FOLLOW");
     }
   fprintf (Gbl.F.Out,"%u",Following);
   if (Following)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</div>"
                      "<div class=\"%s\">"
                      "%s</div>"
		      "</td>",
	    The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Following);

   /***** Followers *****/
   fprintf (Gbl.F.Out,"<td style=\"min-width:100px;"
	              " text-align:center; vertical-align:top;\">"
		      "<div class=\"FOLLOW\">");
   if (Followers)
     {
      /* Form to list followers */
      Act_FormStart (ActSeeFlr);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Txt_Followers,"FOLLOW");
     }
   fprintf (Gbl.F.Out,"%u",Followers);
   if (Followers)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</div>"
                      "<div class=\"%s\">"
                      "%s</div>"
		      "</td>",
	    The_ClassFormul[Gbl.Prefs.Theme],
            Txt_Followers);

   /***** End table *****/
   fprintf (Gbl.F.Out,"</tr>"
	              "</table>");
  }

/*****************************************************************************/
/*************************** Get number of followed **************************/
/*****************************************************************************/

static unsigned Fol_GetNumFollowing (long UsrCod)
  {
   char Query[128];

   /***** Check if a user is a follower of another user *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_follow WHERE FollowerCod='%ld'",
            UsrCod);
   return DB_QueryCOUNT (Query,"can not get number of followed");
  }

/*****************************************************************************/
/************************** Get number of followers **************************/
/*****************************************************************************/

static unsigned Fol_GetNumFollowers (long UsrCod)
  {
   char Query[128];

   /***** Check if a user is a follower of another user *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_follow WHERE FollowedCod='%ld'",
            UsrCod);
   return DB_QueryCOUNT (Query,"can not get number of followers");
  }

/*****************************************************************************/
/***************************** List followed users ***************************/
/*****************************************************************************/

void Fol_ListFollowing (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   extern const char *Txt_Following;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Get user to view user he/she follows *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /***** Show user's profile *****/
      if (Prf_ShowUserProfile ())
	{
	 /***** Check if a user is a follower of another user *****/
	 sprintf (Query,"SELECT FollowedCod FROM usr_follow"
			" WHERE FollowerCod='%ld' ORDER BY FollowTime DESC",
		  Gbl.Usrs.Other.UsrDat.UsrCod);
	 NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get followed users");
	 if (NumUsrs)
	   {
	    /***** Initialize structure with user's data *****/
	    Usr_UsrDataConstructor (&UsrDat);

	    /***** Start listing *****/
	    Lay_StartRoundFrameTable10 (NULL,2,Txt_Following);

	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /***** Get user and number of clicks *****/
	       row = mysql_fetch_row (mysql_res);

	       /* Get user's code (row[0]) */
	       UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	       Usr_GetAllUsrDataFromUsrCod (&UsrDat);

	       /***** Show user *****/
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
		  fprintf (Gbl.F.Out,"<tr>");
	       Fol_ShowFollowedOrFollowed (&UsrDat);
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		   NumUsr == NumUsrs - 1)
		  fprintf (Gbl.F.Out,"</tr>");
	      }

	    /***** End listing *****/
	    Lay_EndRoundFrameTable10 ();

	    /***** Free memory used for user's data *****/
	    Usr_UsrDataDestructor (&UsrDat);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/******************************* List followers ******************************/
/*****************************************************************************/

void Fol_ListFollowers (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   extern const char *Txt_Followers;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Get user to view user he/she follows *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /***** Show user's profile *****/
      if (Prf_ShowUserProfile ())
	{
	 /***** Check if a user is a follower of another user *****/
	 sprintf (Query,"SELECT FollowerCod FROM usr_follow"
			" WHERE FollowedCod='%ld' ORDER BY FollowTime DESC",
		  Gbl.Usrs.Other.UsrDat.UsrCod);
	 NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get followers");
	 if (NumUsrs)
	   {
	    /***** Initialize structure with user's data *****/
	    Usr_UsrDataConstructor (&UsrDat);

	    /***** Start listing *****/
	    Lay_StartRoundFrameTable10 (NULL,2,Txt_Followers);

	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /***** Get user and number of clicks *****/
	       row = mysql_fetch_row (mysql_res);

	       /* Get user's code (row[0]) */
	       UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	       Usr_GetAllUsrDataFromUsrCod (&UsrDat);

	       /***** Show user *****/
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
		  fprintf (Gbl.F.Out,"<tr>");
	       Fol_ShowFollowedOrFollowed (&UsrDat);
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		   NumUsr == NumUsrs - 1)
		  fprintf (Gbl.F.Out,"</tr>");
	      }

	    /***** End listing *****/
	    Lay_EndRoundFrameTable10 ();

	    /***** Free memory used for user's data *****/
	    Usr_UsrDataDestructor (&UsrDat);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************** Show user's photo and nickname in ranking list ***************/
/*****************************************************************************/

static void Fol_ShowFollowedOrFollowed (const struct UsrData *UsrDat)
  {
   extern const char *Txt_View_public_profile;
   extern const char *Txt_Unfollow;
   extern const char *Txt_Follow;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   bool Visible = Pri_ShowIsAllowed (UsrDat->ProfileVisibility,UsrDat->UsrCod);

   /***** Put form to follow / unfollow *****/
   fprintf (Gbl.F.Out,"<td style=\"width:50px; height:50px;"
	              " text-align:right;\">");
   if (Visible &&
       Gbl.Usrs.Me.Logged &&
       Gbl.Usrs.Me.UsrDat.UsrCod != UsrDat->UsrCod)
     {
      if (Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDat->UsrCod))
	{
	 Act_FormStart (ActUnfUsr);
	 Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_Unfollow,NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\">"
			    "<img src=\"%s/unfollow16x16.gif\""
			    " style=\"width:16px; height:16px; padding:0 2px;\" alt=\"%s\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Unfollow);
	 Act_FormEnd ();
	}
      else
	{
	 Act_FormStart (ActFolUsr);
	 Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_Follow,NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\">"
			    "<img src=\"%s/follow16x16.gif\""
			    " style=\"width:16px; height:16px; padding:0 2px;\" alt=\"%s\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Follow);
	 Act_FormEnd ();
	}
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Check if I can see the public profile *****/
   fprintf (Gbl.F.Out,"<td style=\"width:40px; height:50px;"
	              " text-align:center;\">");
   if (Visible)
     {
      /***** User's photo *****/
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					   NULL,
			"PHOTO36x48",Pho_ZOOM);
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Put form to go to public profile *****/
   fprintf (Gbl.F.Out,"<td style=\"min-width:150px; height:50px;"
	              " text-align:left;\">");
   if (Visible &&
       UsrDat->Nickname[0])
     {
      Act_FormStart (ActSeePubPrf);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Txt_View_public_profile,"DAT");
      Usr_RestrictLengthAndWriteName (UsrDat,20);
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/***************************** Follow another user ***************************/
/*****************************************************************************/

void Fol_FollowUsr (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   bool Error;

   /***** Get user to be followed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      // Follow only if I can view his/her public profile
      if (Pri_ShowIsAllowed (Gbl.Usrs.Other.UsrDat.ProfileVisibility,Gbl.Usrs.Other.UsrDat.UsrCod))
         // Follow only if I do not follow him/her
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

   /***** Get user to be unfollowed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      // Unfollow only if I follow him/her
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
