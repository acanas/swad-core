// swad_follow.c: user's followers and followed

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For sprintf
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_notification.h"
#include "swad_profile.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Fol_NUM_COLUMNS_FOLLOW 3

#define Fol_MAX_USRS_TO_FOLLOW_SUGGESTED (Fol_NUM_COLUMNS_FOLLOW * 3)

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

static void Fol_ShowNumberOfFollowingOrFollowers (const struct UsrData *UsrDat,
                                                  unsigned NumUsrs,
                                                  Act_Action_t Action,
                                                  const char *Title);
static void Fol_ShowFollowedOrFollower (const struct UsrData *UsrDat);

/*****************************************************************************/
/********************** Put link to show users to follow **********************/
/*****************************************************************************/

void Fol_PutLinkWhoToFollow (void)
  {
   extern const char *Txt_Who_to_follow;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Lay_PutContextualLink (ActWhoFol,NULL,
                          "follow64x64.png",
                          Txt_Who_to_follow,Txt_Who_to_follow);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/***************************** Show users to follow **************************/
/*****************************************************************************/

void Fol_SuggestWhoToFollow (void)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   extern const char *Txt_No_user_to_whom_you_can_follow_Try_again_later;
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** First try: build query to get users to follow *****/
   sprintf (Query,"SELECT UsrCod FROM"
                  " ("
		  // Users with privacy
                  // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
                  // who are followed from my followed
                  " SELECT DISTINCT usr_follow.FollowedCod AS UsrCod"
                  " FROM usr_follow,"
                  " (SELECT FollowedCod FROM usr_follow"
                  " WHERE FollowerCod='%ld') AS my_followed,"
                  " usr_data"
                  " WHERE usr_follow.FollowerCod=my_followed.FollowedCod"
                  " AND usr_follow.FollowedCod<>'%ld'"
                  " AND usr_follow.FollowedCod=usr_data.UsrCod"
                  " AND usr_data.ProfileVisibility IN ('%s','%s')"

                  " UNION"

		  // Users with privacy Pri_VISIBILITY_COURSE
                  // who share any course with me
                  " SELECT DISTINCT crs_usr.UsrCod"
                  " FROM crs_usr,"
                  " (SELECT CrsCod FROM crs_usr"
                  " WHERE UsrCod='%ld') AS my_crs,"
                  " usr_data"
                  " WHERE crs_usr.CrsCod=my_crs.CrsCod"
                  " AND crs_usr.UsrCod<>'%ld'"
                  " AND crs_usr.UsrCod=usr_data.UsrCod"
                  " AND usr_data.ProfileVisibility='%s'"

                  " UNION"

		  // Users with privacy Pri_VISIBILITY_USER
                  // who share any course with me with another role
                  " SELECT DISTINCT crs_usr.UsrCod"
                  " FROM crs_usr,"
                  " (SELECT CrsCod,Role FROM crs_usr"
                  " WHERE UsrCod='%ld') AS my_crs_role,"
                  " usr_data"
                  " WHERE crs_usr.CrsCod=my_crs_role.CrsCod"
                  " AND crs_usr.Role<>my_crs_role.Role"
                  " AND crs_usr.UsrCod=usr_data.UsrCod"
                  " AND usr_data.ProfileVisibility='%s'"

                  ") AS UsrsToFollow"

		  // Not select my followed
                  " WHERE UsrCod NOT IN"
                  " (SELECT FollowedCod FROM usr_follow"
                  " WHERE FollowerCod='%ld')"

		  // Get only Fol_MAX_USRS_TO_FOLLOW_SUGGESTED users
                  " ORDER BY RAND() LIMIT %u",
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
   Pri_VisibilityDB[Pri_VISIBILITY_WORLD],
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Pri_VisibilityDB[Pri_VISIBILITY_USER],
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Fol_MAX_USRS_TO_FOLLOW_SUGGESTED);

   // Lay_ShowAlert (Lay_INFO,Query);

   /***** Try to get users *****/
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get followed users");

   if (!NumUsrs)
     {
      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Second try: build query to get users to follow *****/
      sprintf (Query,// Users with privacy
		     // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
		     "SELECT UsrCod FROM usr_data"
		     " WHERE UsrCod<>'%ld'"
		     " AND ProfileVisibility IN ('%s','%s')"

		     // Not select my followed
		     " AND UsrCod NOT IN"
		     " (SELECT FollowedCod FROM usr_follow"
		     " WHERE FollowerCod='%ld')"

		     // Get only Fol_MAX_USRS_TO_FOLLOW_SUGGESTED users
		     " ORDER BY RAND() LIMIT %u",
      Gbl.Usrs.Me.UsrDat.UsrCod,
      Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
      Pri_VisibilityDB[Pri_VISIBILITY_WORLD],
      Gbl.Usrs.Me.UsrDat.UsrCod,
      Fol_MAX_USRS_TO_FOLLOW_SUGGESTED);

      // Lay_ShowAlert (Lay_INFO,Query);

      /***** Get users *****/
      NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get followed users");
     }

   if (NumUsrs)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start listing *****/
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\""
			 " style=\"margin:12px auto;\">");

      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 /***** Get user *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get user's code (row[0]) */
	 UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /***** Show user *****/
	 if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
	    fprintf (Gbl.F.Out,"<tr>");
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
	    Fol_ShowFollowedOrFollower (&UsrDat);
	 if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
	     NumUsr == NumUsrs - 1)
	    fprintf (Gbl.F.Out,"</tr>");
	}

      /***** End listing *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_No_user_to_whom_you_can_follow_Try_again_later);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

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
/*************************** Get number of followed **************************/
/*****************************************************************************/

unsigned Fol_GetNumFollowing (long UsrCod)
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

unsigned Fol_GetNumFollowers (long UsrCod)
  {
   char Query[128];

   /***** Check if a user is a follower of another user *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_follow WHERE FollowedCod='%ld'",
            UsrCod);
   return DB_QueryCOUNT (Query,"can not get number of followers");
  }

/*****************************************************************************/
/**************** Show following and followers of a user *********************/
/*****************************************************************************/

void Fol_ShowFollowingAndFollowers (const struct UsrData *UsrDat,
                                    unsigned NumFollowing,unsigned NumFollowers,
                                    bool UsrFollowsMe,bool IFollowUsr)
  {
   extern const char *Txt_FOLLOWS_YOU;
   extern const char *Txt_Following;
   extern const char *Txt_Followers;
   extern const char *Txt_Following_unfollow;
   extern const char *Txt_Unfollow;
   extern const char *Txt_Follow;
   bool ItsMe = (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"follow_section\">");

   /***** Followed users *****/
   fprintf (Gbl.F.Out,"<div id=\"following_side\">"
                      "<div class=\"FOLLOW_SIDE\">");

   /* User follows me? */
   fprintf (Gbl.F.Out,"<div id=\"follows_me\" class=\"DAT_LIGHT\">");
   if (UsrFollowsMe)
      fprintf (Gbl.F.Out,"%s",Txt_FOLLOWS_YOU);
   fprintf (Gbl.F.Out,"</div>");

   /* Number of followed */
   Fol_ShowNumberOfFollowingOrFollowers (UsrDat,
                                         NumFollowing,
                                         ActSeeFlg,Txt_Following);

   /* End following side */
   fprintf (Gbl.F.Out,"</div>"
                      "</div>");

   /***** Followers *****/
   fprintf (Gbl.F.Out,"<div id=\"followers_side\">"
                      "<div class=\"FOLLOW_SIDE\">");

   /* Number of followers */
   Fol_ShowNumberOfFollowingOrFollowers (UsrDat,
                                         NumFollowers,
                                         ActSeeFlr,Txt_Followers);

   /* I follow user? */
   fprintf (Gbl.F.Out,"<div id=\"follow_usr\">");
   if (Gbl.Usrs.Me.Logged && !ItsMe)	// It's another logged user
     {
      if (IFollowUsr)	// I follow this user
	{
	 Act_FormStart (ActUnfUsr);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_Following_unfollow,"REC_DAT_BOLD");
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
			    " style=\"display:inline;\" >"
			    "<img src=\"%s/following64x64.png\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICON40x40\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Unfollow,Txt_Following_unfollow);
	 Act_FormEnd ();
	}
      else		// I do not follow this user
	{
	 Act_FormStart (ActFolUsr);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_Follow,"REC_DAT_BOLD");
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\""
			    " style=\"display:inline;\" >"
			    "<img src=\"%s/follow64x64.png\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICON40x40\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Follow,Txt_Follow);
	 Act_FormEnd ();
	}
     }
   fprintf (Gbl.F.Out,"</div>");

   /* End followers side */
   fprintf (Gbl.F.Out,"</div>"
	              "</div>");

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

/*****************************************************************************/
/**************** Show following and followers of a user *********************/
/*****************************************************************************/

static void Fol_ShowNumberOfFollowingOrFollowers (const struct UsrData *UsrDat,
                                                  unsigned NumUsrs,
                                                  Act_Action_t Action,
                                                  const char *Title)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *The_ClassFormBold[The_NUM_THEMES];

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"FOLLOW_BOX\">");

   /***** Number *****/
   if (NumUsrs)
     {
      /* Form to list users */
      Act_FormStartAnchor (Action,"follow_section");
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Title,
                          (Gbl.Action.Act == Action) ? "FOLLOW_NUM_B" :
        	                                       "FOLLOW_NUM");
     }
   else
      fprintf (Gbl.F.Out,"<span class=\"%s\">",
	       (Gbl.Action.Act == Action) ? "FOLLOW_NUM_B" :
					    "FOLLOW_NUM");
   fprintf (Gbl.F.Out,"%u",NumUsrs);
   if (NumUsrs)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   else
      fprintf (Gbl.F.Out,"</span>");

   /***** Text *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">",
            (Gbl.Action.Act == Action) ? The_ClassFormBold[Gbl.Prefs.Theme] :
        	                         The_ClassForm[Gbl.Prefs.Theme]);
   if (NumUsrs)
     {
      /* Form to list users */
      Act_FormStartAnchor (Action,"follow_section");
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Title,
                          (Gbl.Action.Act == Action) ? The_ClassFormBold[Gbl.Prefs.Theme] :
        	                                       The_ClassForm[Gbl.Prefs.Theme]);
     }
   fprintf (Gbl.F.Out,"%s",Title);
   if (NumUsrs)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/***************************** List followed users ***************************/
/*****************************************************************************/

void Fol_ListFollowing (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
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
	    fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\""
		               " style=\"margin:12px auto;\">");

	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /***** Get user *****/
	       row = mysql_fetch_row (mysql_res);

	       /* Get user's code (row[0]) */
	       UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	       /***** Show user *****/
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
		  fprintf (Gbl.F.Out,"<tr>");
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
		  Fol_ShowFollowedOrFollower (&UsrDat);
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		   NumUsr == NumUsrs - 1)
		  fprintf (Gbl.F.Out,"</tr>");
	      }

	    /***** End listing *****/
	    fprintf (Gbl.F.Out,"</table>");

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
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Get user to view user he/she follows *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();
   if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0)	// If user not specified, view my profile
      Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;

   /***** Check if user exists and get his data *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
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
	    fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\""
		               " style=\"margin:12px auto;\">");

	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /***** Get user and number of clicks *****/
	       row = mysql_fetch_row (mysql_res);

	       /* Get user's code (row[0]) */
	       UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	       /***** Show user *****/
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
		  fprintf (Gbl.F.Out,"<tr>");
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
	          Fol_ShowFollowedOrFollower (&UsrDat);
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		   NumUsr == NumUsrs - 1)
		  fprintf (Gbl.F.Out,"</tr>");
	      }

	    /***** End listing *****/
	    fprintf (Gbl.F.Out,"</table>");

	    /***** Free memory used for user's data *****/
	    Usr_UsrDataDestructor (&UsrDat);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

	 /***** If it's me, mark possible notification as seen *****/
	 if (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)
	    Ntf_MarkNotifAsSeen (Ntf_EVENT_FOLLOWER,
	                         -1L,-1L,
				 Gbl.Usrs.Me.UsrDat.UsrCod);
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************* Show followed or follower *************************/
/*****************************************************************************/

static void Fol_ShowFollowedOrFollower (const struct UsrData *UsrDat)
  {
   extern const char *Txt_View_public_profile;
   extern const char *Txt_Following_unfollow;
   extern const char *Txt_Unfollow;
   extern const char *Txt_Follow;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   bool Visible = Pri_ShowIsAllowed (UsrDat->ProfileVisibility,UsrDat->UsrCod);

   /***** Put form to follow / unfollow *****/
   fprintf (Gbl.F.Out,"<td class=\"RIGHT_MIDDLE\""
	              " style=\"width:25px; height:62px;\">");
   if (Visible &&
       Gbl.Usrs.Me.Logged &&
       Gbl.Usrs.Me.UsrDat.UsrCod != UsrDat->UsrCod)
     {
      if (Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDat->UsrCod))	// I follow user
	{
	 Act_FormStart (ActUnfUsr);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_Following_unfollow,NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\">"
			    "<img src=\"%s/following64x64.png\""
			    " alt=\"%s\" title=\"%s\""
	                    " class=\"ICON20x20\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Unfollow,Txt_Following_unfollow);
	 Act_FormEnd ();
	}
      else	// I do not follow this user
	{
	 Act_FormStart (ActFolUsr);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmit (Txt_Follow,NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICON_HIGHLIGHT\">"
			    "<img src=\"%s/follow64x64.png\""
			    " alt=\"%s\" title=\"%s\""
	                    " class=\"ICON20x20\" />"
			    "</div>"
			    "</a>",
		  Gbl.Prefs.IconsURL,
		  Txt_Follow,Txt_Follow);
	 Act_FormEnd ();
	}
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Check if I can see the public profile *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\""
	              " style=\"width:50px; height:62px;\">");
   if (Visible)
     {
      /***** User's photo *****/
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					   NULL,
			"PHOTO45x60",Pho_ZOOM,false);
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Put form to go to public profile *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\""
	              " style=\"min-width:87px; height:62px;\">");
   if (Visible &&
       UsrDat->Nickname[0])
     {
      Act_FormStart (ActSeePubPrf);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Txt_View_public_profile,"DAT");
      Usr_RestrictLengthAndWriteName (UsrDat,8);
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
   bool CreateNotif;
   bool NotifyByEmail;
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

	    /***** This follow must be notified by e-mail? *****/
            CreateNotif = (Gbl.Usrs.Other.UsrDat.Prefs.NotifNtfEvents & (1 << Ntf_EVENT_FOLLOWER));
            NotifyByEmail = CreateNotif &&
        	            (Gbl.Usrs.Other.UsrDat.Prefs.EmailNtfEvents & (1 << Ntf_EVENT_FOLLOWER));

            /***** Create notification for this followed.
                   If this followed wants to receive notifications by e-mail, activate the sending of a notification *****/
            if (CreateNotif)
               Ntf_StoreNotifyEventToOneUser (Ntf_EVENT_FOLLOWER,&Gbl.Usrs.Other.UsrDat,Gbl.Usrs.Me.UsrDat.UsrCod,
                                              (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
                                        	                              0));
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

/*****************************************************************************/
/****** Get and show ranking of users attending to number of followers *******/
/*****************************************************************************/

void Fol_GetAndShowRankingFollowers (void)
  {
   char Query[512];

   /***** Get ranking from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT FollowedCod,COUNT(FollowerCod) AS N"
	                " FROM usr_follow"
	                " GROUP BY FollowedCod"
			" ORDER BY N DESC,FollowedCod LIMIT 100");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT usr_follow.FollowedCod,COUNT(DISTINCT usr_follow.FollowerCod) AS N"
                        " FROM institutions,centres,degrees,courses,crs_usr,usr_follow"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_follow.FollowedCod"
	                " GROUP BY usr_follow.FollowedCod"
			" ORDER BY N DESC,usr_follow.FollowedCod LIMIT 100",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT usr_follow.FollowedCod,COUNT(DISTINCT usr_follow.FollowerCod) AS N"
                        " FROM centres,degrees,courses,crs_usr,usr_follow"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_follow.FollowedCod"
	                " GROUP BY usr_follow.FollowedCod"
			" ORDER BY N DESC,usr_follow.FollowedCod LIMIT 100",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT usr_follow.FollowedCod,COUNT(DISTINCT usr_follow.FollowerCod) AS N"
                        " FROM degrees,courses,crs_usr,usr_follow"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_follow.FollowedCod"
	                " GROUP BY usr_follow.FollowedCod"
			" ORDER BY N DESC,usr_follow.FollowedCod LIMIT 100",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT usr_follow.FollowedCod,COUNT(DISTINCT usr_follow.FollowerCod) AS N"
                        " FROM courses,crs_usr,usr_follow"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_follow.FollowedCod"
	                " GROUP BY usr_follow.FollowedCod"
			" ORDER BY N DESC,usr_follow.FollowedCod LIMIT 100",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT usr_follow.FollowedCod,COUNT(DISTINCT usr_follow.FollowerCod) AS N"
                        " FROM crs_usr,usr_follow"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.UsrCod=usr_follow.FollowedCod"
	                " GROUP BY usr_follow.FollowedCod"
			" ORDER BY N DESC,usr_follow.FollowedCod LIMIT 100",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
         Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }
   Prf_ShowRankingFigure (Query);
  }

/*****************************************************************************/
/********************* Get notification of a new follower ********************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Fol_GetNotifFollower (char *SummaryStr,char **ContentStr)
  {
   SummaryStr[0] = '\0';

   if ((*ContentStr = (char *) malloc (1)))
      strcpy (*ContentStr,"");
  }

/*****************************************************************************/
/*********************** Remove user from user follow ************************/
/*****************************************************************************/

void Fol_RemoveUsrFromUsrFollow (long UsrCod)
  {
   char Query[128];

   sprintf (Query,"DELETE FROM usr_follow"
	          " WHERE FollowerCod='%ld' OR FollowedCod='%ld'",
	    UsrCod,UsrCod);
   DB_QueryDELETE (Query,"can not remove user from followers and followed");
  }
