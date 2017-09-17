// swad_follow.c: user's followers and followed

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include "swad_box.h"
#include "swad_database.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_notification.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Fol_NUM_COLUMNS_FOLLOW 3

#define Fol_FOLLOW_SECTION_ID	"follow_section"

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

static unsigned Fol_GetUsrsWhoToFollow (unsigned MaxUsrsToShow,
                                        bool OnlyUsrsWithPhotos,
                                        MYSQL_RES **mysql_res);

static void Fol_PutIconsWhoToFollow (void);
static void Fol_PutIconToUpdateWhoToFollow (void);

static void Fol_ShowNumberOfFollowingOrFollowers (const struct UsrData *UsrDat,
                                                  unsigned NumUsrs,
                                                  Act_Action_t Action,
                                                  const char *Title);

static void Fol_ListFollowingUsr (struct UsrData *UsrDat);
static void Fol_ListFollowersUsr (struct UsrData *UsrDat);

static void Fol_ShowFollowedOrFollower (struct UsrData *UsrDat);
static void Fol_WriteRowUsrToFollowOnRightColumn (struct UsrData *UsrDat);
static void Fol_PutInactiveIconToFollowUnfollow (void);
static void Fol_PutIconToFollow (struct UsrData *UsrDat);
static void Fol_PutIconToUnfollow (struct UsrData *UsrDat);

/*****************************************************************************/
/********************** Put link to show users to follow **********************/
/*****************************************************************************/

void Fol_PutLinkWhoToFollow (void)
  {
   extern const char *Txt_Who_to_follow;

   Lay_PutContextualLink (ActSeeSocPrf,NULL,NULL,
                          "follow64x64.png",
                          Txt_Who_to_follow,Txt_Who_to_follow,
                          NULL);
  }

/*****************************************************************************/
/****************** Show several users to follow on main zone ****************/
/*****************************************************************************/

#define Fol_MAX_USRS_TO_FOLLOW_MAIN_ZONE (Fol_NUM_COLUMNS_FOLLOW * 3)

void Fol_SuggestUsrsToFollowMainZone (void)
  {
   extern const char *Hlp_SOCIAL_Profiles_who_to_follow;
   extern const char *Txt_Who_to_follow;
   extern const char *Txt_No_user_to_whom_you_can_follow_Try_again_later;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Put links to request my public profile and another user's profile *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Prf_PutLinkMyPublicProfile ();
   Prf_PutLinkRequestAnotherUserProfile ();
   fprintf (Gbl.F.Out,"</div>");

   /***** Get users *****/
   if ((NumUsrs = Fol_GetUsrsWhoToFollow (Fol_MAX_USRS_TO_FOLLOW_MAIN_ZONE,
                                          false,
                                          &mysql_res)))
     {
      /***** Start box and table *****/
      Box_StartBoxTable ("560px",Txt_Who_to_follow,Fol_PutIconsWhoToFollow,
                         Hlp_SOCIAL_Profiles_who_to_follow,Box_NOT_CLOSABLE,2);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List users *****/
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

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End table and box *****/
      Box_EndBoxTable ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_No_user_to_whom_you_can_follow_Try_again_later);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** Show several users to follow on right column ***************/
/*****************************************************************************/

#define Fol_MAX_USRS_TO_FOLLOW_RIGHT_COLUMN 3

void Fol_SuggestUsrsToFollowMainZoneOnRightColumn (void)
  {
   extern const char *Txt_Who_to_follow;
   extern const char *Txt_No_user_to_whom_you_can_follow_Try_again_later;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Get users *****/
   if ((NumUsrs = Fol_GetUsrsWhoToFollow (Fol_MAX_USRS_TO_FOLLOW_RIGHT_COLUMN,
                                          true,
                                          &mysql_res)))
     {
      /***** Start container *****/
      fprintf (Gbl.F.Out,"<div class=\"CONNECTED\">");

      /***** Title with link to suggest more users to follow *****/
      Act_FormStart (ActSeeSocPrf);
      Act_LinkFormSubmit (Txt_Who_to_follow,"CONNECTED_TXT",NULL);
      fprintf (Gbl.F.Out,"%s</a>",Txt_Who_to_follow);
      Act_FormEnd ();

      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table>");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** List users *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 /***** Get user *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get user's code (row[0]) */
	 UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /***** Show user *****/
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
	    Fol_WriteRowUsrToFollowOnRightColumn (&UsrDat);
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");

      /***** End container *****/
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Get users to follow *****************************/
/*****************************************************************************/

static unsigned Fol_GetUsrsWhoToFollow (unsigned MaxUsrsToShow,
                                        bool OnlyUsrsWithPhotos,
                                        MYSQL_RES **mysql_res)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   char Query[4096];
   char SubQuery1[256];
   char SubQuery2[256];
   char SubQuery3[256];
   char SubQuery4[256];

   /***** Build subqueries related to photos *****/
   if (OnlyUsrsWithPhotos)
     {
      // Photo visibility should be >= profile visibility in every subquery
      sprintf (SubQuery1,
               " AND usr_data.PhotoVisibility IN ('%s','%s')"
               " AND usr_data.Photo<>''",
	       Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
	       Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
      sprintf (SubQuery2,
               " AND usr_data.PhotoVisibility IN ('%s','%s','%s')"
               " AND usr_data.Photo<>''",
	       Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
	       Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
	       Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
      sprintf (SubQuery3,
               " AND usr_data.PhotoVisibility IN ('%s','%s','%s','%s')"
               " AND usr_data.Photo<>''",
	       Pri_VisibilityDB[Pri_VISIBILITY_USER  ],
	       Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
	       Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
	       Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
      sprintf (SubQuery4,
               " AND usr_data.PhotoVisibility IN ('%s','%s')"
               " AND usr_data.Photo<>''",
	       Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
               Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
     }
   else
     {
      SubQuery1[0] = '\0';
      SubQuery2[0] = '\0';
      SubQuery3[0] = '\0';
      SubQuery4[0] = '\0';
     }

   /***** Build query to get users to follow *****/
   // Get only users with surname 1 and first name
   sprintf (Query,"SELECT DISTINCT UsrCod FROM"
                  " ("
		  /***** Likely known users *****/
                  "(SELECT DISTINCT UsrCod FROM"
                  " ("
		  // Users followed by my followed whose privacy is
                  // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
                  "("
                  "SELECT DISTINCT usr_follow.FollowedCod AS UsrCod"
                  " FROM usr_follow,"
                  "(SELECT FollowedCod FROM usr_follow"
                  " WHERE FollowerCod=%ld) AS my_followed,"
                  " usr_data"
                  " WHERE usr_follow.FollowerCod=my_followed.FollowedCod"
                  " AND usr_follow.FollowedCod<>%ld"
                  " AND usr_follow.FollowedCod=usr_data.UsrCod"
                  " AND usr_data.ProfileVisibility IN ('%s','%s')"
		  " AND usr_data.Surname1<>''"	// Surname 1 not empty
		  " AND usr_data.FirstName<>''"	// First name not empty
                  "%s"				// SubQuery1
                  ")"
                  " UNION "
		  // Users who share any course with me
		  // and whose privacy is Pri_VISIBILITY_COURSE,
                  // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
                  "("
                  "SELECT DISTINCT crs_usr.UsrCod"
                  " FROM crs_usr,"
                  "(SELECT CrsCod FROM crs_usr"
                  " WHERE UsrCod=%ld) AS my_crs,"
                  " usr_data"
                  " WHERE crs_usr.CrsCod=my_crs.CrsCod"
                  " AND crs_usr.UsrCod<>%ld"
                  " AND crs_usr.UsrCod=usr_data.UsrCod"
                  " AND usr_data.ProfileVisibility IN ('%s','%s','%s')"
		  " AND usr_data.Surname1<>''"	// Surname 1 not empty
		  " AND usr_data.FirstName<>''"	// First name not empty
                  "%s"				// SubQuery2
                  ")"
                  " UNION "
		  // Users who share any course with me with another role
		  // and whose privacy is Pri_VISIBILITY_USER
                  "("
                  "SELECT DISTINCT crs_usr.UsrCod"
                  " FROM crs_usr,"
                  "(SELECT CrsCod,Role FROM crs_usr"
                  " WHERE UsrCod=%ld) AS my_crs_role,"
                  " usr_data"
                  " WHERE crs_usr.CrsCod=my_crs_role.CrsCod"
                  " AND crs_usr.Role<>my_crs_role.Role"
                  " AND crs_usr.UsrCod=usr_data.UsrCod"
                  " AND usr_data.ProfileVisibility='%s'"
		  " AND usr_data.Surname1<>''"	// Surname 1 not empty
		  " AND usr_data.FirstName<>''"	// First name not empty
                  "%s"				// SubQuery3
                  ")"
                  ") AS LikelyKnownUsrsToFollow"
		  // Do not select my followed
                  " WHERE UsrCod NOT IN"
                  " (SELECT FollowedCod FROM usr_follow"
                  " WHERE FollowerCod=%ld)"
		  // Get only MaxUsrsToShow * 2 users
		  " ORDER BY RAND() LIMIT %u"
                  ")"
                  " UNION "
                  "("
		  /***** Likely unknown users *****/
		  // Add some likely unknown random users with privacy
                  // Pri_VISIBILITY_SYSTEM or Pri_VISIBILITY_WORLD
		  "SELECT UsrCod FROM usr_data"
		  " WHERE UsrCod<>%ld"
		  " AND ProfileVisibility IN ('%s','%s')"
		  " AND Surname1<>''"		// Surname 1 not empty
		  " AND FirstName<>''"		// First name not empty
                  "%s"				// SubQuery4
		  // Do not select my followed
		  " AND UsrCod NOT IN"
		  " (SELECT FollowedCod FROM usr_follow"
		  " WHERE FollowerCod=%ld)"
		  // Get only MaxUsrsToShow users
		  " ORDER BY RAND() LIMIT %u"
		  ")"
                  ") AS UsrsToFollow"
		  // Get only MaxUsrsToShow users
                  " ORDER BY RAND() LIMIT %u",

   Gbl.Usrs.Me.UsrDat.UsrCod,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
   Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
   SubQuery1,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
   Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
   SubQuery2,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   Pri_VisibilityDB[Pri_VISIBILITY_USER  ],
   SubQuery3,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   MaxUsrsToShow * 2,		// 2/3 likely known users

   Gbl.Usrs.Me.UsrDat.UsrCod,
   Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
   Pri_VisibilityDB[Pri_VISIBILITY_WORLD ],
   SubQuery4,
   Gbl.Usrs.Me.UsrDat.UsrCod,
   MaxUsrsToShow,		// 1/3 likely unknown users

   MaxUsrsToShow);

   return DB_QuerySELECT (Query,mysql_res,"can not get users to follow");
  }

/*****************************************************************************/
/****************** Put contextual icons in "who to follow" ******************/
/*****************************************************************************/

static void Fol_PutIconsWhoToFollow (void)
  {
   /***** Put icon to update who to follow *****/
   Fol_PutIconToUpdateWhoToFollow ();

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_FOLLOW;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************* Put icon to update who to follow **********************/
/*****************************************************************************/

static void Fol_PutIconToUpdateWhoToFollow (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Update;

   Act_FormStart (ActSeeSocPrf);
   Act_LinkFormSubmitAnimated (Txt_Update,The_ClassFormBold[Gbl.Prefs.Theme],
                               NULL);
   Ico_PutCalculateIcon (Txt_Update);
   Act_FormEnd ();
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
	          " WHERE FollowerCod=%ld AND FollowedCod=%ld",
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
   sprintf (Query,"SELECT COUNT(*) FROM usr_follow WHERE FollowerCod=%ld",
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
   sprintf (Query,"SELECT COUNT(*) FROM usr_follow WHERE FollowedCod=%ld",
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
   Lay_StartSection (Fol_FOLLOW_SECTION_ID);

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
	 Act_LinkFormSubmit (Txt_Following_unfollow,"REC_DAT_BOLD",NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICO_HIGHLIGHT\""
			    " style=\"display:inline;\" >"
			    "<img src=\"%s/following64x64.png\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICO40x40\" />"
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
	 Act_LinkFormSubmit (Txt_Follow,"REC_DAT_BOLD",NULL);
	 fprintf (Gbl.F.Out,"<div class=\"ICO_HIGHLIGHT\""
			    " style=\"display:inline;\" >"
			    "<img src=\"%s/follow64x64.png\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICO40x40\" />"
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
   Lay_EndSection ();
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
      Act_FormStartAnchor (Action,Fol_FOLLOW_SECTION_ID);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Title,
                          (Gbl.Action.Act == Action) ? "FOLLOW_NUM_B" :
        	                                       "FOLLOW_NUM",NULL);
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
      Act_FormStartAnchor (Action,Fol_FOLLOW_SECTION_ID);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Title,
                          (Gbl.Action.Act == Action) ? The_ClassFormBold[Gbl.Prefs.Theme] :
        	                                       The_ClassForm[Gbl.Prefs.Theme],NULL);
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

   /***** Get user to view user he/she follows *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
	 Fol_ListFollowingUsr (&Gbl.Usrs.Other.UsrDat);
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else				// If user not specified, view my profile
      Fol_ListFollowingUsr (&Gbl.Usrs.Me.UsrDat);
  }

static void Fol_ListFollowingUsr (struct UsrData *UsrDat)
  {
   extern const char *Txt_Following;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData FollowingUsrDat;

   /***** Show user's profile *****/
   if (Prf_ShowUserProfile (UsrDat))
     {
      /***** Check if a user is a follower of another user *****/
      sprintf (Query,"SELECT FollowedCod FROM usr_follow"
		     " WHERE FollowerCod=%ld ORDER BY FollowTime DESC",
	       UsrDat->UsrCod);
      NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get followed users");
      if (NumUsrs)
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&FollowingUsrDat);

         /***** Start box and table *****/
	 Box_StartBoxTable ("560px",Txt_Following,NULL,
	                    NULL,Box_NOT_CLOSABLE,2);

	 for (NumUsr = 0;
	      NumUsr < NumUsrs;
	      NumUsr++)
	   {
	    /***** Get user *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code (row[0]) */
	    FollowingUsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	    /***** Show user *****/
	    if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
	       fprintf (Gbl.F.Out,"<tr>");
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FollowingUsrDat))
	       Fol_ShowFollowedOrFollower (&FollowingUsrDat);
	    if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		NumUsr == NumUsrs - 1)
	       fprintf (Gbl.F.Out,"</tr>");
	   }

         /***** End table and box *****/
	 Box_EndBoxTable ();

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&FollowingUsrDat);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/******************************* List followers ******************************/
/*****************************************************************************/

void Fol_ListFollowers (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user to view user he/she follows *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
	 Fol_ListFollowersUsr (&Gbl.Usrs.Other.UsrDat);
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else				// If user not specified, view my profile
      Fol_ListFollowersUsr (&Gbl.Usrs.Me.UsrDat);
  }

static void Fol_ListFollowersUsr (struct UsrData *UsrDat)
  {
   extern const char *Txt_Followers;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData FollowerUsrDat;

   /***** Show user's profile *****/
   if (Prf_ShowUserProfile (UsrDat))
     {
      /***** Check if a user is a follower of another user *****/
      sprintf (Query,"SELECT FollowerCod FROM usr_follow"
		     " WHERE FollowedCod=%ld ORDER BY FollowTime DESC",
	       UsrDat->UsrCod);
      NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get followers");
      if (NumUsrs)
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&FollowerUsrDat);

         /***** Start box and table *****/
	 Box_StartBoxTable ("560px",Txt_Followers,NULL,
	                    NULL,Box_NOT_CLOSABLE,2);

	 for (NumUsr = 0;
	      NumUsr < NumUsrs;
	      NumUsr++)
	   {
	    /***** Get user and number of clicks *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code (row[0]) */
	    FollowerUsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	    /***** Show user *****/
	    if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
	       fprintf (Gbl.F.Out,"<tr>");
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FollowerUsrDat))
	       Fol_ShowFollowedOrFollower (&FollowerUsrDat);
	    if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		NumUsr == NumUsrs - 1)
	       fprintf (Gbl.F.Out,"</tr>");
	   }

         /***** End table and box *****/
	 Box_EndBoxTable ();

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&FollowerUsrDat);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** If it's me, mark possible notification as seen *****/
      if (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)
	 Ntf_MarkNotifAsSeen (Ntf_EVENT_FOLLOWER,
			      -1L,-1L,
			      Gbl.Usrs.Me.UsrDat.UsrCod);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************* Show followed or follower *************************/
/*****************************************************************************/

static void Fol_ShowFollowedOrFollower (struct UsrData *UsrDat)
  {
   extern const char *Txt_Another_user_s_profile;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool Visible = Pri_ShowingIsAllowed (UsrDat->ProfileVisibility,UsrDat);

   /***** Show user's photo *****/
   fprintf (Gbl.F.Out,"<td class=\"FOLLOW_PHOTO\">");
   if (Visible)
     {
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					   NULL,
			"PHOTO60x80",Pho_ZOOM,false);
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Show user's name and icon to follow/unfollow *****/
   fprintf (Gbl.F.Out,"<td class=\"FOLLOW_USR\">");
   if (Visible)
     {
      /* Put form to go to public profile */
      Act_FormStart (ActSeeOthPubPrf);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      fprintf (Gbl.F.Out,"<div class=\"FOLLOW_USR_NAME\">");	// Limited width
      Act_LinkFormSubmit (Txt_Another_user_s_profile,"DAT",NULL);
      Usr_WriteFirstNameBRSurnames (UsrDat);
      fprintf (Gbl.F.Out,"</a>"
	                 "</div>");
      Act_FormEnd ();
     }

   if (!Gbl.Usrs.Me.Logged ||				// Not logged
       Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod)	// It's me
      /* Inactive icon to follow/unfollow */
      Fol_PutInactiveIconToFollowUnfollow ();
   else
     {
      /* Put form to follow / unfollow */
      if (Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDat->UsrCod))	// I follow user
	 /* Form to unfollow */
	 Fol_PutIconToUnfollow (UsrDat);
      else if (Visible)	// I do not follow this user and I can follow
	 /* Form to follow */
	 Fol_PutIconToFollow (UsrDat);
     }
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/********************* Write the name of a connected user ********************/
/*****************************************************************************/

static void Fol_WriteRowUsrToFollowOnRightColumn (struct UsrData *UsrDat)
  {
   extern const char *Txt_Another_user_s_profile;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool Visible = Pri_ShowingIsAllowed (UsrDat->ProfileVisibility,UsrDat);

   /***** Show user's photo *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"LEFT_MIDDLE COLOR%u\""
	              " style=\"width:22px;\">",
	    Gbl.RowEvenOdd);
   if (Visible)
     {
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO21x28",Pho_ZOOM,false);
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** User's name *****/
   fprintf (Gbl.F.Out,"<td class=\"CON_USR_NARROW CON_CRS COLOR%u\">",
	    Gbl.RowEvenOdd);
   if (Visible)
     {
      /* Put form to go to public profile */
      Act_FormStart (ActSeeOthPubPrf);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      fprintf (Gbl.F.Out,"<div class=\"CON_NAME_NARROW\">");	// Limited width
      Act_LinkFormSubmit (Txt_Another_user_s_profile,"CON_CRS",NULL);
      Usr_WriteFirstNameBRSurnames (UsrDat);
      fprintf (Gbl.F.Out,"</a>"
	                 "</div>");
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</td>");

   /***** Icon to follow *****/
   fprintf (Gbl.F.Out,"<td class=\"RIGHT_MIDDLE COLOR%u\""
	              " style=\"width:48px;\">",
            Gbl.RowEvenOdd);
   if (!Gbl.Usrs.Me.Logged ||				// Not logged
       Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod)	// It's me
      /* Inactive icon to follow/unfollow */
      Fol_PutInactiveIconToFollowUnfollow ();
   else
     {
      /* Put form to follow / unfollow */
      if (Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDat->UsrCod))	// I follow user
	 /* Form to unfollow */
	 Fol_PutIconToUnfollow (UsrDat);
      else if (Visible)	// I do not follow this user and I can follow
	 /* Form to follow */
	 Fol_PutIconToFollow (UsrDat);
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*********************** Put icon to unfollow another user *********************/
/*****************************************************************************/

static void Fol_PutInactiveIconToFollowUnfollow (void)
  {
   /***** Inactive icon to follow/unfollow *****/
   fprintf (Gbl.F.Out,"<div class=\"FOLLOW_USR_ICO ICO_HIDDEN\">"
		      "<img src=\"%s/usr64x64.png\""
		      " alt=\"\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL);
   }

/*****************************************************************************/
/*********************** Put icon to unfollow another user *********************/
/*****************************************************************************/

static void Fol_PutIconToFollow (struct UsrData *UsrDat)
  {
   extern const char *Txt_Follow;

   /***** Form to unfollow *****/
   Act_FormStart (ActFolUsr);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmit (Txt_Follow,NULL,NULL);
   fprintf (Gbl.F.Out,"<div class=\"FOLLOW_USR_ICO ICO_HIGHLIGHT\">"
		      "<img src=\"%s/follow64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>"
		      "</a>",
	    Gbl.Prefs.IconsURL,
	    Txt_Follow,Txt_Follow);
   Act_FormEnd ();
  }

/*****************************************************************************/
/********************** Put icon to unfollow another user ********************/
/*****************************************************************************/

static void Fol_PutIconToUnfollow (struct UsrData *UsrDat)
  {
   extern const char *Txt_Unfollow;

   /* Form to follow */
   Act_FormStart (ActUnfUsr);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmit (Txt_Unfollow,NULL,NULL);
   fprintf (Gbl.F.Out,"<div class=\"FOLLOW_USR_ICO ICO_HIGHLIGHT\">"
		      "<img src=\"%s/following64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>"
		      "</a>",
	    Gbl.Prefs.IconsURL,
	    Txt_Unfollow,Txt_Unfollow);
   Act_FormEnd ();
  }

/*****************************************************************************/
/***************************** Follow another user ***************************/
/*****************************************************************************/

void Fol_FollowUsr1 (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Get user to be followed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      // Follow only if I can view his/her public profile
      if (Pri_ShowingIsAllowed (Gbl.Usrs.Other.UsrDat.ProfileVisibility,&Gbl.Usrs.Other.UsrDat))
         // Follow only if I do not follow him/her
	 if (!Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
					Gbl.Usrs.Other.UsrDat.UsrCod))
	   {
	    /***** Follow user in database *****/
	    sprintf (Query,"REPLACE INTO usr_follow"
			   " (FollowerCod,FollowedCod,FollowTime)"
			   " VALUES"
			   " (%ld,%ld,NOW())",
		     Gbl.Usrs.Me.UsrDat.UsrCod,
		     Gbl.Usrs.Other.UsrDat.UsrCod);
	    DB_QueryREPLACE (Query,"can not follow user");

	    /***** This follow must be notified by email? *****/
            CreateNotif = (Gbl.Usrs.Other.UsrDat.Prefs.NotifNtfEvents & (1 << Ntf_EVENT_FOLLOWER));
            NotifyByEmail = CreateNotif &&
        	            (Gbl.Usrs.Other.UsrDat.Prefs.EmailNtfEvents & (1 << Ntf_EVENT_FOLLOWER));

            /***** Create notification for this followed.
                   If this followed wants to receive notifications by email, activate the sending of a notification *****/
            if (CreateNotif)
               Ntf_StoreNotifyEventToOneUser (Ntf_EVENT_FOLLOWER,&Gbl.Usrs.Other.UsrDat,Gbl.Usrs.Me.UsrDat.UsrCod,
                                              (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
                                        	                              0));
	   }
      Gbl.Alert.Type = Ale_SUCCESS;
     }
   else
     {
      Gbl.Alert.Type = Ale_WARNING;
      sprintf (Gbl.Alert.Txt,"%s",Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

void Fol_FollowUsr2 (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   if (Gbl.Alert.Type == Ale_SUCCESS)
      /***** Show user's profile again *****/
      if (!Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat))
	{
	 Gbl.Alert.Type = Ale_WARNING;
	 sprintf (Gbl.Alert.Txt,"%s",Txt_User_not_found_or_you_do_not_have_permission_);
	}

   if (Gbl.Alert.Type != Ale_SUCCESS)
      Ale_ShowPendingAlert ();
  }

/*****************************************************************************/
/***************************** Unfollow another user *************************/
/*****************************************************************************/

void Fol_UnfollowUsr1 (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];

   /***** Get user to be unfollowed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      // Unfollow only if I follow him/her
      if (Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
                                    Gbl.Usrs.Other.UsrDat.UsrCod))
	{
	 /***** Unfollow user in database *****/
	 sprintf (Query,"DELETE FROM usr_follow"
	                " WHERE FollowerCod=%ld AND FollowedCod=%ld",
		  Gbl.Usrs.Me.UsrDat.UsrCod,
                  Gbl.Usrs.Other.UsrDat.UsrCod);
	 DB_QueryREPLACE (Query,"can not unfollow user");
        }
      Gbl.Alert.Type = Ale_SUCCESS;
     }
   else
     {
      Gbl.Alert.Type = Ale_WARNING;
      sprintf (Gbl.Alert.Txt,"%s",Txt_User_not_found_or_you_do_not_have_permission_);
     }
  }

void Fol_UnfollowUsr2 (void)
  {
   /***** Get user to be unfollowed *****/
   if (Gbl.Alert.Type == Ale_SUCCESS)
     {
      /***** Show user's profile again *****/
      if (!Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat))	// I can not view user's profile
	 /* 1) I followed a user when I had permission
	    2) User restricted permission, so now I can not view his/her profile
	    3) Now I can not view his/her profile ==> show users I follow */
	 Fol_ListFollowingUsr (&Gbl.Usrs.Me.UsrDat);		// List users I follow
     }
   else
      Ale_ShowPendingAlert ();
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
                        " WHERE institutions.CtyCod=%ld"
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
                        " WHERE centres.InsCod=%ld"
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
                        " WHERE degrees.CtrCod=%ld"
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
                        " WHERE courses.DegCod=%ld"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_follow.FollowedCod"
	                " GROUP BY usr_follow.FollowedCod"
			" ORDER BY N DESC,usr_follow.FollowedCod LIMIT 100",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT usr_follow.FollowedCod,COUNT(DISTINCT usr_follow.FollowerCod) AS N"
                        " FROM crs_usr,usr_follow"
                        " WHERE crs_usr.CrsCod=%ld"
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

void Fol_GetNotifFollower (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                           char **ContentStr)
  {
   SummaryStr[0] = '\0';	// Return nothing on error

   if ((*ContentStr = (char *) malloc (1)))
      *ContentStr[0] = '\0';
  }

/*****************************************************************************/
/*********************** Remove user from user follow ************************/
/*****************************************************************************/

void Fol_RemoveUsrFromUsrFollow (long UsrCod)
  {
   char Query[128];

   sprintf (Query,"DELETE FROM usr_follow"
	          " WHERE FollowerCod=%ld OR FollowedCod=%ld",
	    UsrCod,UsrCod);
   DB_QueryDELETE (Query,"can not remove user from followers and followed");
  }
