// swad_follow.c: user's followers and followed

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_follow.h"
#include "swad_follow_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Fol_NUM_COLUMNS_FOLLOW 3

#define Fol_FOLLOW_SECTION_ID	"follow_section"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Fol_PutIconsWhoToFollow (__attribute__((unused)) void *Args);
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
static void Fol_PutIconToFollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);
static void Fol_PutIconToUnfollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);

static void Fol_RequestFollowUsrs (Act_Action_t NextAction);
static void Fol_RequestUnfollowUsrs (Act_Action_t NextAction);
static void Fol_PutHiddenParSelectedUsrsCods (void *SelectedUsrs);
static void Fol_GetFollowedFromSelectedUsrs (unsigned *NumFollowed,
                                             unsigned *NumNotFollowed);

static void Fol_FollowUsr (const struct UsrData *UsrDat);
static void Fol_UnfollowUsr (const struct UsrData *UsrDat);

/*****************************************************************************/
/********************** Put link to show users to follow **********************/
/*****************************************************************************/

void Fol_PutLinkWhoToFollow (void)
  {
   extern const char *Txt_Who_to_follow;

   Lay_PutContextualLinkIconText (ActSeeSocPrf,NULL,
                                  NULL,NULL,
				  "user-plus.svg",
				  Txt_Who_to_follow);
  }

/*****************************************************************************/
/****************** Show several users to follow on main zone ****************/
/*****************************************************************************/

#define Fol_MAX_USRS_TO_FOLLOW_MAIN_ZONE (Fol_NUM_COLUMNS_FOLLOW * 3)

void Fol_SuggestUsrsToFollowMainZone (void)
  {
   extern const char *Hlp_START_Profiles_who_to_follow;
   extern const char *Txt_Who_to_follow;
   extern const char *Txt_No_user_to_whom_you_can_follow_Try_again_later;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData UsrDat;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Prf_PutLinkMyPublicProfile ();		// My public profile
      Prf_PutLinkRequestAnotherUserProfile ();	// Request another user's profile
   Mnu_ContextMenuEnd ();

   /***** Get users *****/
   if ((NumUsrs = Fol_DB_GetUsrsToFollow (Fol_MAX_USRS_TO_FOLLOW_MAIN_ZONE,
                                          Fol_SUGGEST_ANY_USER,
                                          &mysql_res)))
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin ("560px",Txt_Who_to_follow,
                         Fol_PutIconsWhoToFollow,NULL,
                         Hlp_START_Profiles_who_to_follow,Box_NOT_CLOSABLE,2);

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
	       HTM_TR_Begin (NULL);
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	       Fol_ShowFollowedOrFollower (&UsrDat);
	    if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		NumUsr == NumUsrs - 1)
	       HTM_TR_End ();
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

      /***** End table and box *****/
      Box_BoxTableEnd ();
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
   if ((NumUsrs = Fol_DB_GetUsrsToFollow (Fol_MAX_USRS_TO_FOLLOW_RIGHT_COLUMN,
                                          Fol_SUGGEST_ONLY_USERS_WITH_PHOTO,
                                          &mysql_res)))
     {
      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"CONNECTED\"");

	 /***** Title with link to suggest more users to follow *****/
	 Frm_BeginForm (ActSeeSocPrf);
	    HTM_BUTTON_SUBMIT_Begin (Txt_Who_to_follow,"BT_LINK CONNECTED_TXT",NULL);
	       HTM_Txt (Txt_Who_to_follow);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

	 /***** Begin table *****/
	 HTM_TABLE_Begin (NULL);

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
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
		  Fol_WriteRowUsrToFollowOnRightColumn (&UsrDat);
	      }

	    /***** Free memory used for user's data *****/
	    Usr_UsrDataDestructor (&UsrDat);

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End container *****/
      HTM_DIV_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Put contextual icons in "who to follow" ******************/
/*****************************************************************************/

static void Fol_PutIconsWhoToFollow (__attribute__((unused)) void *Args)
  {
   /***** Put icon to update who to follow *****/
   Fol_PutIconToUpdateWhoToFollow ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_FOLLOW);
  }

/*****************************************************************************/
/********************* Put icon to update who to follow **********************/
/*****************************************************************************/

static void Fol_PutIconToUpdateWhoToFollow (void)
  {
   extern const char *Txt_Update;

   Frm_BeginForm (ActSeeSocPrf);
      HTM_BUTTON_Animated_Begin (Txt_Update,"BT_LINK",NULL);
	 Ico_PutCalculateIcon (Txt_Update);
      HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** Get number of followed **************************/
/*****************************************************************************/

void Fol_FlushCacheFollow (void)
  {
   Gbl.Cache.Follow.UsrCod = -1L;
   Gbl.Cache.Follow.NumFollowing =
   Gbl.Cache.Follow.NumFollowers = 0;
  }

void Fol_GetNumFollow (long UsrCod,
                       unsigned *NumFollowing,unsigned *NumFollowers)
  {
   /***** 1. Fast check: trivial cases *****/
   if (UsrCod <= 0)
     {
      *NumFollowing = *NumFollowers = 0;
      return;
     }

   /***** 2. Fast check: Is number of following already calculated? *****/
   if (UsrCod == Gbl.Cache.Follow.UsrCod)
     {
      *NumFollowing = Gbl.Cache.Follow.NumFollowing;
      *NumFollowers = Gbl.Cache.Follow.NumFollowers;
      return;
     }

   /***** 3. Slow check: Get number of following/followers from database *****/
   Gbl.Cache.Follow.UsrCod = UsrCod;
   *NumFollowing = Gbl.Cache.Follow.NumFollowing = Fol_DB_GetNumFollowing (UsrCod);
   *NumFollowers = Gbl.Cache.Follow.NumFollowers = Fol_DB_GetNumFollowers (UsrCod);
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
   extern const char *Txt_Follow;

   /***** Begin section *****/
   HTM_SECTION_Begin (Fol_FOLLOW_SECTION_ID);

      /***** Followed users *****/
      HTM_DIV_Begin ("id=\"following_side\"");
	 HTM_DIV_Begin ("class=\"FOLLOW_SIDE\"");

	    /* User follows me? */
	    HTM_DIV_Begin ("id=\"follows_me\" class=\"DAT_LIGHT\"");
	       if (UsrFollowsMe)
		  HTM_Txt (Txt_FOLLOWS_YOU);
	    HTM_DIV_End ();

	    /* Number of followed */
	    Fol_ShowNumberOfFollowingOrFollowers (UsrDat,
						  NumFollowing,
						  ActSeeFlg,Txt_Following);

	 /* End following side */
	 HTM_DIV_End ();
      HTM_DIV_End ();

      /***** Followers *****/
      HTM_DIV_Begin ("id=\"followers_side\"");
	 HTM_DIV_Begin ("class=\"FOLLOW_SIDE\"");

	    /* Number of followers */
	    Fol_ShowNumberOfFollowingOrFollowers (UsrDat,
						  NumFollowers,
						  ActSeeFlr,Txt_Followers);

	    /* I follow user? */
	    HTM_DIV_Begin ("id=\"follow_usr\"");
	       if (Gbl.Usrs.Me.Logged &&	// Logged
		   !Usr_ItsMe (UsrDat->UsrCod))	// Not me!
		 {
		  Frm_BeginForm (IFollowUsr ? ActUnfUsr :
					      ActFolUsr);
		     Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
		     HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,
				      IFollowUsr ? "user-check.svg" :
						   "user-plus.svg",
				      IFollowUsr ? Txt_Following_unfollow :
						   Txt_Follow,
				      "ICO_HIGHLIGHT ICO40x40");
		  Frm_EndForm ();
		 }
	    HTM_DIV_End ();

	 /* End followers side */
	 HTM_DIV_End ();
      HTM_DIV_End ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/**************** Show following and followers of a user *********************/
/*****************************************************************************/

static void Fol_ShowNumberOfFollowingOrFollowers (const struct UsrData *UsrDat,
                                                  unsigned NumUsrs,
                                                  Act_Action_t Action,
                                                  const char *Title)
  {
   extern const char *The_ClassFormOutBox[The_NUM_THEMES];
   extern const char *The_ClassFormOutBoxBold[The_NUM_THEMES];
   extern const char *The_ClassFormLinkOutBox[The_NUM_THEMES];
   extern const char *The_ClassFormLinkOutBoxBold[The_NUM_THEMES];

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"FOLLOW_BOX\"");

      /***** Number *****/
      if (NumUsrs)
	{
	 /* Form to list users */
	 Frm_BeginFormAnchor (Action,Fol_FOLLOW_SECTION_ID);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	    HTM_BUTTON_SUBMIT_Begin (Title,
				     (Gbl.Action.Act == Action) ? "BT_LINK FOLLOW_NUM_B" :
								  "BT_LINK FOLLOW_NUM",
				     NULL);
	}
      else
	 HTM_SPAN_Begin ("class=\"%s\"",(Gbl.Action.Act == Action) ? "FOLLOW_NUM_B" :
								     "FOLLOW_NUM");
      HTM_Unsigned (NumUsrs);
      if (NumUsrs)
	{
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
	}
      else
	 HTM_SPAN_End ();

      /***** Text *****/
      HTM_DIV_Begin ("class=\"%s\"",
		     (Gbl.Action.Act == Action) ? The_ClassFormOutBoxBold[Gbl.Prefs.Theme] :
						  The_ClassFormOutBox    [Gbl.Prefs.Theme]);
	 if (NumUsrs)
	   {
	    /* Form to list users */
	    Frm_BeginFormAnchor (Action,Fol_FOLLOW_SECTION_ID);
	       Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	       HTM_BUTTON_SUBMIT_Begin (Title,
					(Gbl.Action.Act == Action) ? The_ClassFormLinkOutBoxBold[Gbl.Prefs.Theme] :
								     The_ClassFormLinkOutBox    [Gbl.Prefs.Theme],
					NULL);
	   }
	 HTM_Txt (Title);
	 if (NumUsrs)
	   {
	       HTM_BUTTON_End ();
	    Frm_EndForm ();
	   }
      HTM_DIV_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************************** List followed users ***************************/
/*****************************************************************************/

void Fol_ListFollowing (void)
  {
   /***** Get user to view user he/she follows *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	 Fol_ListFollowingUsr (&Gbl.Usrs.Other.UsrDat);
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else				// If user not specified, view my profile
      Fol_ListFollowingUsr (&Gbl.Usrs.Me.UsrDat);
  }

static void Fol_ListFollowingUsr (struct UsrData *UsrDat)
  {
   extern const char *Txt_Following;
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData FollowingUsrDat;

   /***** Show user's profile *****/
   if (Prf_ShowUserProfile (UsrDat))
     {
      /***** Get list of following *****/
      NumUsrs = Fol_DB_GetListFollowing (UsrDat->UsrCod,&mysql_res);

      if (NumUsrs)
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&FollowingUsrDat);

         /***** Begin box and table *****/
	 Box_BoxTableBegin ("560px",Txt_Following,
	                    NULL,NULL,
	                    NULL,Box_NOT_CLOSABLE,2);

	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /***** Get user's code *****/
	       FollowingUsrDat.UsrCod = DB_GetNextCode (mysql_res);

	       /***** Show user *****/
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
		  HTM_TR_Begin (NULL);
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FollowingUsrDat,
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
		  Fol_ShowFollowedOrFollower (&FollowingUsrDat);
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		   NumUsr == NumUsrs - 1)
		  HTM_TR_End ();
	      }

         /***** End table and box *****/
	 Box_BoxTableEnd ();

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&FollowingUsrDat);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/******************************* List followers ******************************/
/*****************************************************************************/

void Fol_ListFollowers (void)
  {
   /***** Get user to view user he/she follows *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	 Fol_ListFollowersUsr (&Gbl.Usrs.Other.UsrDat);
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else				// If user not specified, view my profile
      Fol_ListFollowersUsr (&Gbl.Usrs.Me.UsrDat);
  }

static void Fol_ListFollowersUsr (struct UsrData *UsrDat)
  {
   extern const char *Txt_Followers;
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct UsrData FollowerUsrDat;

   /***** Show user's profile *****/
   if (Prf_ShowUserProfile (UsrDat))
     {
      /***** Get list of followers *****/
      if ((NumUsrs = Fol_DB_GetListFollowers (UsrDat->UsrCod,&mysql_res)))
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&FollowerUsrDat);

         /***** Begin box and table *****/
	 Box_BoxTableBegin ("560px",Txt_Followers,
	                    NULL,NULL,
	                    NULL,Box_NOT_CLOSABLE,2);

	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /***** Get user's code *****/
	       FollowerUsrDat.UsrCod = DB_GetNextCode (mysql_res);

	       /***** Show user *****/
	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == 0)
		  HTM_TR_Begin (NULL);

	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FollowerUsrDat,
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
		  Fol_ShowFollowedOrFollower (&FollowerUsrDat);

	       if ((NumUsr % Fol_NUM_COLUMNS_FOLLOW) == (Fol_NUM_COLUMNS_FOLLOW-1) ||
		   NumUsr == NumUsrs - 1)
		  HTM_TR_End ();
	      }

         /***** End table and box *****/
	 Box_BoxTableEnd ();

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&FollowerUsrDat);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** If it's me, mark possible notification as seen *****/
      if (Usr_ItsMe (UsrDat->UsrCod))
	 Ntf_DB_MarkNotifsAsSeen (Ntf_EVENT_FOLLOWER);
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************* Show followed or follower *************************/
/*****************************************************************************/

static void Fol_ShowFollowedOrFollower (struct UsrData *UsrDat)
  {
   extern const char *Txt_Another_user_s_profile;
   static const char *ClassPhoto[Pho_NUM_SHAPES] =
     {
      [Pho_SHAPE_CIRCLE   ] = "PHOTOC60x80",
      [Pho_SHAPE_ELLIPSE  ] = "PHOTOE60x80",
      [Pho_SHAPE_OVAL     ] = "PHOTOO60x80",
      [Pho_SHAPE_RECTANGLE] = "PHOTOR60x80",
     };
   bool Visible = Pri_ShowingIsAllowed (UsrDat->BaPrfVisibility,UsrDat);

   /***** Show user's photo *****/
   HTM_TD_Begin ("class=\"FOLLOW_PHOTO\"");
      if (Visible)
	 Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                            ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM,
	                            false);
   HTM_TD_End ();

   /***** Show user's name and icon to follow/unfollow *****/
   HTM_TD_Begin ("class=\"FOLLOW_USR\"");

      if (Visible)
	{
	 /* Put form to go to public profile */
	 Frm_BeginForm (ActSeeOthPubPrf);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	    HTM_DIV_Begin ("class=\"FOLLOW_USR_NAME\"");	// Limited width
	       HTM_BUTTON_SUBMIT_Begin (Txt_Another_user_s_profile,"BT_LINK LT DAT",NULL);
		  Usr_WriteFirstNameBRSurnames (UsrDat);
	       HTM_BUTTON_End ();
	    HTM_DIV_End ();
	 Frm_EndForm ();
	}

      if (!Gbl.Usrs.Me.Logged ||	// Not logged
	  Usr_ItsMe (UsrDat->UsrCod))	// It's me
	 /* Inactive icon to follow/unfollow */
	 Fol_PutInactiveIconToFollowUnfollow ();
      else				// It's not me
	{
	 /* Put form to follow / unfollow */
	 if (Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                  UsrDat->UsrCod))	// I follow user
	    /* Form to unfollow */
	    Fol_PutIconToUnfollow (UsrDat->EnUsrCod);
	 else if (Visible)	// I do not follow this user and I can follow
	    /* Form to follow */
	    Fol_PutIconToFollow (UsrDat->EnUsrCod);
	}

   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Write the name of a connected user ********************/
/*****************************************************************************/

static void Fol_WriteRowUsrToFollowOnRightColumn (struct UsrData *UsrDat)
  {
   extern const char *Txt_Another_user_s_profile;
   static const char *ClassPhoto[Pho_NUM_SHAPES] =
     {
      [Pho_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [Pho_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [Pho_SHAPE_OVAL     ] = "PHOTOO21x28",
      [Pho_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   bool Visible = Pri_ShowingIsAllowed (UsrDat->BaPrfVisibility,UsrDat);

   /***** Show user's photo *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"CON_PHOTO COLOR%u\"",Gbl.RowEvenOdd);
	 if (Visible)
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM,
	                               false);
      HTM_TD_End ();

      /***** User's name *****/
      HTM_TD_Begin ("class=\"CON_NAME_FOLLOW COLOR%u\"",Gbl.RowEvenOdd);
	 if (Visible)
	   {
	    /* Put form to go to public profile */
	    Frm_BeginForm (ActSeeOthPubPrf);
	       Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	       HTM_DIV_Begin ("class=\"CON_NAME_FOLLOW\"");	// Limited width
		  HTM_BUTTON_SUBMIT_Begin (Txt_Another_user_s_profile,
		                           "BT_LINK CON_NAME_FOLLOW CON_CRS",NULL);
		     Usr_WriteFirstNameBRSurnames (UsrDat);
		  HTM_BUTTON_End ();
	       HTM_DIV_End ();
	    Frm_EndForm ();
	   }
      HTM_TD_End ();

      /***** Icon to follow *****/
      HTM_TD_Begin ("class=\"CON_ICON_FOLLOW RM COLOR%u\"",Gbl.RowEvenOdd);
      if (!Gbl.Usrs.Me.Logged ||	// Not logged
	  Usr_ItsMe (UsrDat->UsrCod))	// It's me
	 /* Inactive icon to follow/unfollow */
	 Fol_PutInactiveIconToFollowUnfollow ();
      else				// It's not me
	{
	 /* Put form to follow / unfollow */
	 if (Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
	                                  UsrDat->UsrCod))	// I follow user
	    /* Form to unfollow */
	    Fol_PutIconToUnfollow (UsrDat->EnUsrCod);
	 else if (Visible)	// I do not follow this user and I can follow
	    /* Form to follow */
	    Fol_PutIconToFollow (UsrDat->EnUsrCod);
	}
      HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*********************** Put icon to unfollow another user *********************/
/*****************************************************************************/

static void Fol_PutInactiveIconToFollowUnfollow (void)
  {
   /***** Inactive icon to follow/unfollow *****/
   HTM_DIV_Begin ("class=\"FOLLOW_USR_ICO ICO_HIDDEN\"");
      Ico_PutIcon ("user.svg","","ICO16x16");
   HTM_DIV_End ();
   }

/*****************************************************************************/
/*********************** Put icon to follow another user *********************/
/*****************************************************************************/

static void Fol_PutIconToFollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   extern const char *Txt_Follow;

   /***** Form to unfollow *****/
   Frm_BeginForm (ActFolUsr);
      Usr_PutParamUsrCodEncrypted (EncryptedUsrCod);
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"user-plus.svg",
		       Txt_Follow,"FOLLOW_USR_ICO ICO_HIGHLIGHT ICO16x16");
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************** Put icon to unfollow another user ********************/
/*****************************************************************************/

static void Fol_PutIconToUnfollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   extern const char *Txt_Unfollow;

   /* Form to follow */
   Frm_BeginForm (ActUnfUsr);
      Usr_PutParamUsrCodEncrypted (EncryptedUsrCod);
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"user-check.svg",
		       Txt_Unfollow,"FOLLOW_USR_ICO ICO_HIGHLIGHT ICO16x16");
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** Follow another user ***************************/
/*****************************************************************************/

void Fol_FollowUsr1 (void)
  {
   /***** Get user to be followed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      // Follow only if I do not follow him/her
      if (!Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
				        Gbl.Usrs.Other.UsrDat.UsrCod))
	 Fol_FollowUsr (&Gbl.Usrs.Other.UsrDat);

      Ale_CreateAlert (Ale_SUCCESS,NULL,"");	// Txt not used
     }
   else
      Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
  }

void Fol_FollowUsr2 (void)
  {
   if (Ale_GetTypeOfLastAlert () == Ale_SUCCESS)
     {
      /***** Show user's profile again *****/
      if (!Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat))
	 /* 1) I had permission to follow the user and I've just follow him/her
	    2) User restricted permission, so now I can not view his/her profile
	    3) Now I can not view his/her profile ==> show users I follow */
	 Fol_ListFollowingUsr (&Gbl.Usrs.Me.UsrDat);		// List users I follow
     }
   else
      Ale_ShowAlerts (NULL);
  }

/*****************************************************************************/
/***************************** Unfollow another user *************************/
/*****************************************************************************/

void Fol_UnfollowUsr1 (void)
  {
   /***** Get user to be unfollowed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      // Unfollow only if I follow him/her
      if (Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
                                       Gbl.Usrs.Other.UsrDat.UsrCod))
	 Fol_UnfollowUsr (&Gbl.Usrs.Other.UsrDat);

      Ale_CreateAlert (Ale_SUCCESS,NULL,"");	// Txt not used
     }
   else
      Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
  }

void Fol_UnfollowUsr2 (void)
  {
   /***** Get user to be unfollowed *****/
   if (Ale_GetTypeOfLastAlert () == Ale_SUCCESS)
     {
      /***** Show user's profile again *****/
      if (!Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat))	// I can not view user's profile
	 /* 1) I followed a user when I had permission
	    2) User restricted permission, so now I can not view his/her profile
	    3) Now I can not view his/her profile ==> show users I follow */
	 Fol_ListFollowingUsr (&Gbl.Usrs.Me.UsrDat);		// List users I follow
     }
   else
      Ale_ShowAlerts (NULL);
  }

/*****************************************************************************/
/***************** Request follow/unfollow several users *********************/
/*****************************************************************************/

void Fol_RequestFollowStds (void)
  {
   Fol_RequestFollowUsrs (ActFolSevStd);
  }

void Fol_RequestFollowTchs (void)
  {
   Fol_RequestFollowUsrs (ActFolSevTch);
  }

static void Fol_RequestFollowUsrs (Act_Action_t NextAction)
  {
   extern const char *Txt_Follow;
   extern const char *Txt_Do_you_want_to_follow_the_selected_user_whom_you_do_not_follow_yet;
   extern const char *Txt_Do_you_want_to_follow_the_X_selected_users_whom_you_do_not_follow_yet;
   unsigned NumFollowed;
   unsigned NumNotFollowed;

   // List of selected users is already got

   /***** Go through list of selected users
          getting the number of followed and not followed ****/
   Fol_GetFollowedFromSelectedUsrs (&NumFollowed,&NumNotFollowed);

   /***** Show question to confirm ****/
   if (NumNotFollowed)
     {
      if (NumNotFollowed == 1)
         Ale_ShowAlertAndButton (NextAction,NULL,NULL,
				 Fol_PutHiddenParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Follow,
				 Ale_QUESTION,Txt_Do_you_want_to_follow_the_selected_user_whom_you_do_not_follow_yet);
      else
         Ale_ShowAlertAndButton (NextAction,NULL,NULL,
				 Fol_PutHiddenParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Follow,
				 Ale_QUESTION,Txt_Do_you_want_to_follow_the_X_selected_users_whom_you_do_not_follow_yet,
				 NumNotFollowed);
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

void Fol_RequestUnfollowStds (void)
  {
   Fol_RequestUnfollowUsrs (ActUnfSevStd);
  }

void Fol_RequestUnfollowTchs (void)
  {
   Fol_RequestUnfollowUsrs (ActUnfSevTch);
  }

static void Fol_RequestUnfollowUsrs (Act_Action_t NextAction)
  {
   extern const char *Txt_Do_you_want_to_stop_following_the_selected_user_whom_you_follow;
   extern const char *Txt_Do_you_want_to_stop_following_the_X_selected_users_whom_you_follow;
   extern const char *Txt_Unfollow;
   unsigned NumFollowed;
   unsigned NumNotFollowed;

   // List of selected users is already got

   /***** Go through list of selected users
          getting the number of followed and not followed ****/
   Fol_GetFollowedFromSelectedUsrs (&NumFollowed,&NumNotFollowed);

   /***** Show question to confirm ****/
   if (NumFollowed)
     {
      if (NumFollowed == 1)
         Ale_ShowAlertAndButton (NextAction,NULL,NULL,
				 Fol_PutHiddenParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Unfollow,
				 Ale_QUESTION,Txt_Do_you_want_to_stop_following_the_selected_user_whom_you_follow);
      else
         Ale_ShowAlertAndButton (NextAction,NULL,NULL,
				 Fol_PutHiddenParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Unfollow,
				 Ale_QUESTION,Txt_Do_you_want_to_stop_following_the_X_selected_users_whom_you_follow,
				 NumFollowed);
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

static void Fol_PutHiddenParSelectedUsrsCods (void *SelectedUsrs)
  {
   if (SelectedUsrs)
      Usr_PutHiddenParSelectedUsrsCods ((struct SelectedUsrs *) SelectedUsrs);
  }

/*****************************************************************************/
/**** Go through the list getting the number of followed and not followed ****/
/*****************************************************************************/

static void Fol_GetFollowedFromSelectedUsrs (unsigned *NumFollowed,
                                             unsigned *NumNotFollowed)
  {
   extern const char *Txt_Selected_users_X_Followed_Y_Not_followed_Z;
   struct UsrData UsrDat;
   const char *Ptr;
   unsigned NumUsrs = 0;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Check users to know if I follow them *****/
   *NumFollowed = 0;
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EnUsrCod,
                                         Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Gbl.Usrs.Me.UsrDat.UsrCod != UsrDat.UsrCod)		// Skip me
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CURRENT_CRS))	// Get from the database the data of the student
	    if (Enr_CheckIfUsrBelongsToCurrentCrs (&UsrDat))
	      {
	       /* Check if I follow this user, and update number of users */
	       if (Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
						UsrDat.UsrCod))	// I follow user
		  (*NumFollowed)++;
	       NumUsrs++;
	      }
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Show alert ****/
   *NumNotFollowed = NumUsrs - *NumFollowed;
   Ale_ShowAlert (Ale_INFO,Txt_Selected_users_X_Followed_Y_Not_followed_Z,
	          NumUsrs,*NumFollowed,*NumNotFollowed);
  }

/*****************************************************************************/
/********************** Follow/unfollow several users ************************/
/*****************************************************************************/

void Fol_FollowUsrs ()
  {
   extern const char *Txt_You_have_followed_one_user;
   extern const char *Txt_You_have_followed_X_users;
   const char *Ptr;
   struct UsrData UsrDat;
   unsigned NumFollowed = 0;

   /***** Get list of selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Check users to know if I follow them *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EnUsrCod,
                                         Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Gbl.Usrs.Me.UsrDat.UsrCod != UsrDat.UsrCod)		// Skip me
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get user's data from database
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	    if (Enr_CheckIfUsrBelongsToCurrentCrs (&UsrDat))
	       /* If I don't follow this user ==> follow him/her */
	       if (!Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
					         UsrDat.UsrCod))
		 {
		  Fol_FollowUsr (&UsrDat);
		  NumFollowed++;
		 }
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Show alert *****/
   if (NumFollowed == 1)
      Ale_ShowAlert (Ale_SUCCESS,Txt_You_have_followed_one_user);
   else
      Ale_ShowAlert (Ale_SUCCESS,Txt_You_have_followed_X_users,
	             NumFollowed);
  }

void Fol_UnfollowUsrs (void)
  {
   extern const char *Txt_You_have_stopped_following_one_user;
   extern const char *Txt_You_have_stopped_following_X_users;
   const char *Ptr;
   struct UsrData UsrDat;
   unsigned NumUnfollowed = 0;

   /***** Get list of selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Check users to know if I follow them *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EnUsrCod,
                                         Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);
      if (Gbl.Usrs.Me.UsrDat.UsrCod != UsrDat.UsrCod)		// Skip me
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get user's data from database
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	    if (Enr_CheckIfUsrBelongsToCurrentCrs (&UsrDat))
	       /* If I follow this user ==> unfollow him/her */
	       if (Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
					        UsrDat.UsrCod))
		 {
		  Fol_UnfollowUsr (&UsrDat);
		  NumUnfollowed++;
		 }
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Show alert *****/
   if (NumUnfollowed == 1)
      Ale_ShowAlert (Ale_SUCCESS,Txt_You_have_stopped_following_one_user);
   else
      Ale_ShowAlert (Ale_SUCCESS,Txt_You_have_stopped_following_X_users,
	             NumUnfollowed);
  }

/*****************************************************************************/
/******************************** Follow user ********************************/
/*****************************************************************************/

static void Fol_FollowUsr (const struct UsrData *UsrDat)
  {
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Avoid wrong cases *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod <= 0 ||
       UsrDat->UsrCod <= 0            ||
       Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod)	// Skip me
      return;

   /***** Follow user in database *****/
   Fol_DB_FollowUsr (UsrDat->UsrCod);

   /***** Flush cache *****/
   Fol_FlushCacheFollow ();

   /***** This follow must be notified by email? *****/
   CreateNotif = (UsrDat->NtfEvents.CreateNotif & (1 << Ntf_EVENT_FOLLOWER));
   NotifyByEmail = CreateNotif &&
		   (UsrDat->NtfEvents.SendEmail & (1 << Ntf_EVENT_FOLLOWER));

   /***** Create notification for this followed.
	  If this followed wants to receive notifications by email,
	  activate the sending of a notification *****/
   if (CreateNotif)
      Ntf_DB_StoreNotifyEventToUsr (Ntf_EVENT_FOLLOWER,UsrDat->UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod,
				    (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
								    0),
				    Gbl.Hierarchy.Ins.InsCod,
				    Gbl.Hierarchy.Ctr.CtrCod,
				    Gbl.Hierarchy.Deg.DegCod,
				    Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******************************* Unfollow user *******************************/
/*****************************************************************************/

static void Fol_UnfollowUsr (const struct UsrData *UsrDat)
  {
   /***** Avoid wrong cases *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod <= 0 ||
       UsrDat->UsrCod <= 0            ||
       Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod)	// Skip me
      return;

   /***** Unfollow user in database *****/
   Fol_DB_UnfollowUsr (UsrDat->UsrCod);

   /***** Flush cache *****/
   Fol_FlushCacheFollow ();
  }

/*****************************************************************************/
/****** Get and show ranking of users attending to number of followers *******/
/*****************************************************************************/

void Fol_GetAndShowRankingFollowers (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;

   /***** Get ranking from database *****/
   NumUsrs = Fol_DB_GetRankingFollowers (&mysql_res);

   /***** Show ranking *****/
   Prf_ShowRankingFigure (&mysql_res,NumUsrs);
  }

/*****************************************************************************/
/********************* Get notification of a new follower ********************/
/*****************************************************************************/

void Fol_GetNotifFollower (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                           char **ContentStr)
  {
   SummaryStr[0] = '\0';	// Return nothing on error

   if ((*ContentStr = malloc (1)))
      *ContentStr[0] = '\0';
  }

/*****************************************************************************/
/*********************** Remove user from user follow ************************/
/*****************************************************************************/

void Fol_RemoveUsrFromUsrFollow (long UsrCod)
  {
   /***** Remove user from followers and followed *****/
   Fol_DB_RemoveUsrFromUsrFollow (UsrCod);

   /***** Flush cache *****/
   Fol_FlushCacheFollow ();
  }
