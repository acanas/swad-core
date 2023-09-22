// swad_follow.c: user's followers and followed

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For asprintf
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
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
#include "swad_parameter.h"
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

static void Fol_ShowNumberOfFollowingOrFollowers (const struct Usr_Data *UsrDat,
                                                  unsigned NumUsrs,
                                                  Act_Action_t Action,
                                                  const char *Title);

static void Fol_ListFollowingUsr (struct Usr_Data *UsrDat);
static void Fol_ListFollowersUsr (struct Usr_Data *UsrDat);

static void Fol_ShowFollowedOrFollower (struct Usr_Data *UsrDat);
static void Fol_WriteRowUsrToFollowOnRightColumn (struct Usr_Data *UsrDat);
static void Fol_PutInactiveIconToFollowUnfollow (void);
static void Fol_PutIconToFollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);
static void Fol_PutIconToUnfollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1]);

static void Fol_RequestFollowUsrs (Act_Action_t NextAction);
static void Fol_RequestUnfollowUsrs (Act_Action_t NextAction);
static void Fol_PutParSelectedUsrsCods (void *SelectedUsrs);
static void Fol_GetFollowedFromSelectedUsrs (unsigned *NumFollowed,
                                             unsigned *NumNotFollowed);

static void Fol_FollowUsr (const struct Usr_Data *UsrDat);
static void Fol_UnfollowUsr (const struct Usr_Data *UsrDat);

/*****************************************************************************/
/********************** Put link to show users to follow **********************/
/*****************************************************************************/

void Fol_PutLinkWhoToFollow (void)
  {
   extern const char *Txt_Who_to_follow;

   Lay_PutContextualLinkIconText (ActSeeSocPrf,NULL,
                                  NULL,NULL,
				  "user-plus.svg",Ico_BLACK,
				  Txt_Who_to_follow,NULL);
  }

/*****************************************************************************/
/****************** Show several users to follow on main zone ****************/
/*****************************************************************************/

#define Fol_MAX_USRS_TO_FOLLOW_MAIN_ZONE (Fol_NUM_COLUMNS_FOLLOW * 3)

void Fol_SuggestUsrsToFollowOnMainZone (void)
  {
   extern const char *Hlp_START_Profiles_who_to_follow;
   extern const char *Txt_Who_to_follow;
   extern const char *Txt_No_user_to_whom_you_can_follow_Try_again_later;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data UsrDat;

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

void Fol_SuggestUsrsToFollowOnRightColumn (void)
  {
   extern const char *Txt_No_user_to_whom_you_can_follow_Try_again_later;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data UsrDat;

   /***** Get users *****/
   if ((NumUsrs = Fol_DB_GetUsrsToFollow (Fol_MAX_USRS_TO_FOLLOW_RIGHT_COLUMN,
                                          Fol_SUGGEST_ONLY_USERS_WITH_PHOTO,
                                          &mysql_res)))
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

	 /***** Begin table *****/
	 HTM_TABLE_Begin (NULL);

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

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Link to suggest more users to follow *****/
      Lay_PutContextualLinkOnlyIcon (ActSeeSocPrf,NULL,
				     NULL,NULL,
				     "ellipsis-h.svg",Ico_BLACK);
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
   Ico_PutContextualIconToUpdate (ActSeeSocPrf,NULL,
                                  NULL,NULL);
  }

/*****************************************************************************/
/*************************** Get number of followed **************************/
/*****************************************************************************/

void Fol_FlushCacheFollow (void)
  {
   Gbl.Cache.Follow.Valid = false;
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
   if (Gbl.Cache.Follow.Valid &&
       UsrCod == Gbl.Cache.Follow.UsrCod)
     {
      *NumFollowing = Gbl.Cache.Follow.NumFollowing;
      *NumFollowers = Gbl.Cache.Follow.NumFollowers;
      return;
     }

   /***** 3. Slow check: Get number of following/followers from database *****/
   Gbl.Cache.Follow.UsrCod = UsrCod;
   *NumFollowing = Gbl.Cache.Follow.NumFollowing = Fol_DB_GetNumFollowing (UsrCod);
   *NumFollowers = Gbl.Cache.Follow.NumFollowers = Fol_DB_GetNumFollowers (UsrCod);
   Gbl.Cache.Follow.Valid = true;
  }

/*****************************************************************************/
/**************** Show following and followers of a user *********************/
/*****************************************************************************/

void Fol_ShowFollowingAndFollowers (const struct Usr_Data *UsrDat,
                                    unsigned NumFollowing,unsigned NumFollowers,
                                    bool UsrFollowsMe,bool IFollowUsr)
  {
   extern const char *Txt_FOLLOWS_YOU;
   extern const char *Txt_Following;
   extern const char *Txt_Followers;

   /***** Begin section *****/
   HTM_SECTION_Begin (Fol_FOLLOW_SECTION_ID);

      /***** Followed users *****/
      HTM_DIV_Begin ("id=\"following_side\"");
	 HTM_DIV_Begin ("class=\"FOLLOW_SIDE\"");

	    /* User follows me? */
	    HTM_DIV_Begin ("id=\"follows_me\" class=\"DAT_LIGHT_%s\"",
	                   The_GetSuffix ());
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
	       if (Gbl.Usrs.Me.Logged &&			// Logged
		   Usr_ItsMe (UsrDat->UsrCod) == Usr_OTHER)	// Not me!
		 {
		  Frm_BeginForm (IFollowUsr ? ActUnfUsr :
					      ActFolUsr);
		     Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
		     HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,
				      IFollowUsr ? "user-check.svg" :
						   "user-plus.svg",
				      Act_GetActionText (IFollowUsr ? ActUnfUsr :
						                      ActFolUsr),
				      "class=\"ICO_HIGHLIGHT ICO40x40\"");
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

static void Fol_ShowNumberOfFollowingOrFollowers (const struct Usr_Data *UsrDat,
                                                  unsigned NumUsrs,
                                                  Act_Action_t Action,
                                                  const char *Title)
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"FOLLOW_BOX %s\"",
                  (Gbl.Action.Act == Action) ? "FOLLOW_NUM_B" :
					       "FOLLOW_NUM");

      /***** Number *****/
      if (NumUsrs)
	{
	 /* Form to list users */
	 Frm_BeginFormAnchor (Action,Fol_FOLLOW_SECTION_ID);
	    Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	    HTM_BUTTON_Submit_Begin (Title,"class=\"BT_LINK\"");
	}
      HTM_Unsigned (NumUsrs);
      if (NumUsrs)
	{
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
	}

      /***** Text *****/
      HTM_DIV_Begin ("class=\"FORM_OUT_%s%s\"",
                     The_GetSuffix (),
		     (Gbl.Action.Act == Action) ? " BOLD" :
						  "");
	 if (NumUsrs)
	   {
	    /* Form to list users */
	    Frm_BeginFormAnchor (Action,Fol_FOLLOW_SECTION_ID);
	       Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	       HTM_BUTTON_Submit_Begin (Title,"class=\"BT_LINK\"");
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
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

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

static void Fol_ListFollowingUsr (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Following;
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data FollowingUsrDat;

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
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

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

static void Fol_ListFollowersUsr (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Followers;
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data FollowerUsrDat;

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
      if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
	 Ntf_DB_MarkNotifsAsSeen (Ntf_EVENT_FOLLOWER);
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************* Show followed or follower *************************/
/*****************************************************************************/

static void Fol_ShowFollowedOrFollower (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Another_user_s_profile;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC60x80",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE60x80",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO60x80",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR60x80",
     };
   bool Visible = Pri_ShowingIsAllowed (UsrDat->BaPrfVisibility,UsrDat);

   /***** Show user's photo *****/
   HTM_TD_Begin ("class=\"FOLLOW_PHOTO\"");
      if (Visible)
	 Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                            ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
   HTM_TD_End ();

   /***** Show user's name and icon to follow/unfollow *****/
   HTM_TD_Begin ("class=\"FOLLOW_USR\"");

      if (Visible)
	{
	 /* Put form to go to public profile */
	 Frm_BeginForm (ActSeeOthPubPrf);
	    Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	    HTM_DIV_Begin ("class=\"LT FOLLOW_USR_NAME DAT_%s\"",	// Limited width
	                   The_GetSuffix ());
	       HTM_BUTTON_Submit_Begin (Txt_Another_user_s_profile,
	                                "class=\"BT_LINK LT\"");
		  Usr_WriteFirstNameBRSurnames (UsrDat);
	       HTM_BUTTON_End ();
	    HTM_DIV_End ();
	 Frm_EndForm ();
	}

      if (!Gbl.Usrs.Me.Logged ||		// Not logged
	  Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)	// It's me
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

static void Fol_WriteRowUsrToFollowOnRightColumn (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Another_user_s_profile;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   bool Visible = Pri_ShowingIsAllowed (UsrDat->BaPrfVisibility,UsrDat);

   /***** Show user's photo *****/
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"CON_PHOTO %s\"",The_GetColorRows ());
	 if (Visible)
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
      HTM_TD_End ();

      /***** User's name *****/
      HTM_TD_Begin ("class=\"CON_NAME_FOLLOW %s\"",
                    The_GetColorRows ());
	 if (Visible)
	   {
	    /* Put form to go to public profile */
	    Frm_BeginForm (ActSeeOthPubPrf);
	       Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	       HTM_DIV_Begin ("class=\"CON_NAME_FOLLOW CON_CRS LT\"");	// Limited width
		  HTM_BUTTON_Submit_Begin (Txt_Another_user_s_profile,
		                           "class=\"LT BT_LINK\"");
		     Usr_WriteFirstNameBRSurnames (UsrDat);
		  HTM_BUTTON_End ();
	       HTM_DIV_End ();
	    Frm_EndForm ();
	   }
      HTM_TD_End ();

      /***** Icon to follow *****/
      HTM_TD_Begin ("class=\"CON_ICON_FOLLOW RM %s\"",
                    The_GetColorRows ());
	 if (!Gbl.Usrs.Me.Logged ||			// Not logged
	     Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)	// It's me
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

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/************* Put inactive icon to follow/unfollow another user *************/
/*****************************************************************************/

static void Fol_PutInactiveIconToFollowUnfollow (void)
  {
   /***** Inactive icon to follow/unfollow *****/
   HTM_DIV_Begin ("class=\"FOLLOW_USR_ICO ICO_HIDDEN\"");
      Ico_PutIcon ("user.svg",Ico_BLACK,"","ICO16x16");
   HTM_DIV_End ();
   }

/*****************************************************************************/
/*********************** Put icon to follow another user *********************/
/*****************************************************************************/

static void Fol_PutIconToFollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   Frm_BeginForm (ActFolUsr);
      Usr_PutParUsrCodEncrypted (EncryptedUsrCod);
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"user-plus.svg",
                       Act_GetActionText (ActFolUsr),
                       "class=\"FOLLOW_USR_ICO ICO16x16 ICO_%s_%s ICO_HIGHLIGHT\"",
                       Ico_GetPreffix (Ico_BLACK),The_GetSuffix ());
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************** Put icon to unfollow another user ********************/
/*****************************************************************************/

static void Fol_PutIconToUnfollow (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   Frm_BeginForm (ActUnfUsr);
      Usr_PutParUsrCodEncrypted (EncryptedUsrCod);
      HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"user-check.svg",
		       Act_GetActionText (ActUnfUsr),
		       "class=\"FOLLOW_USR_ICO ICO_HIGHLIGHT ICO16x16\"");
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** Follow another user ***************************/
/*****************************************************************************/

void Fol_FollowUsr1 (void)
  {
   /***** Get user to be followed *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
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
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
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

void Fol_ReqFollowStds (void)
  {
   Fol_RequestFollowUsrs (ActFolSevStd);
  }

void Fol_ReqFollowTchs (void)
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
				 Fol_PutParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Follow,
				 Ale_QUESTION,Txt_Do_you_want_to_follow_the_selected_user_whom_you_do_not_follow_yet);
      else
         Ale_ShowAlertAndButton (NextAction,NULL,NULL,
				 Fol_PutParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Follow,
				 Ale_QUESTION,Txt_Do_you_want_to_follow_the_X_selected_users_whom_you_do_not_follow_yet,
				 NumNotFollowed);
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

void Fol_ReqUnfollowStds (void)
  {
   Fol_RequestUnfollowUsrs (ActUnfSevStd);
  }

void Fol_ReqUnfollowTchs (void)
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
				 Fol_PutParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Unfollow,
				 Ale_QUESTION,Txt_Do_you_want_to_stop_following_the_selected_user_whom_you_follow);
      else
         Ale_ShowAlertAndButton (NextAction,NULL,NULL,
				 Fol_PutParSelectedUsrsCods,&Gbl.Usrs.Selected,
				 Btn_CREATE_BUTTON,Txt_Unfollow,
				 Ale_QUESTION,Txt_Do_you_want_to_stop_following_the_X_selected_users_whom_you_follow,
				 NumFollowed);
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

static void Fol_PutParSelectedUsrsCods (void *SelectedUsrs)
  {
   if (SelectedUsrs)
      Usr_PutParSelectedUsrsCods ((struct Usr_SelectedUsrs *) SelectedUsrs);
  }

/*****************************************************************************/
/**** Go through the list getting the number of followed and not followed ****/
/*****************************************************************************/

static void Fol_GetFollowedFromSelectedUsrs (unsigned *NumFollowed,
                                             unsigned *NumNotFollowed)
  {
   extern const char *Txt_Selected_users_X_Followed_Y_Not_followed_Z;
   struct Usr_Data UsrDat;
   const char *Ptr;
   unsigned NumUsrs = 0;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Check users to know if I follow them *****/
   *NumFollowed = 0;
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
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
   struct Usr_Data UsrDat;
   unsigned NumFollowed = 0;

   /***** Get list of selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Check users to know if I follow them *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
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
   struct Usr_Data UsrDat;
   unsigned NumUnfollowed = 0;

   /***** Get list of selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Check users to know if I follow them *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
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

static void Fol_FollowUsr (const struct Usr_Data *UsrDat)
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
				    Gbl.Hierarchy.Node[Hie_INS].HieCod,
				    Gbl.Hierarchy.Node[Hie_CTR].HieCod,
				    Gbl.Hierarchy.Node[Hie_DEG].HieCod,
				    Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************************* Unfollow user *******************************/
/*****************************************************************************/

static void Fol_UnfollowUsr (const struct Usr_Data *UsrDat)
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

/*****************************************************************************/
/************** Get and show number of following and followers ***************/
/*****************************************************************************/

void Fol_GetAndShowFollowStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_followed_followers;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Users;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_Followed;
   extern const char *Txt_Followers;
   extern const char *Txt_FollowPerFollow[2];
   unsigned Fol;
   unsigned NumUsrsTotal;
   unsigned NumUsrs;
   double Average;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_FOLLOW],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_followed_followers,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Users           ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Get total number of users *****/
      NumUsrsTotal = Usr_GetTotalNumberOfUsers ();

      /***** Get total number of following/followers from database *****/
      for (Fol = 0;
	   Fol < 2;
	   Fol++)
	{
	 NumUsrs = Fol_DB_GetNumFollowinFollowers (Fol);

	 /***** Write number of followed / followers *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       HTM_Txt (Fol == 0 ? Txt_Followed :
				   Txt_Followers);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (NumUsrs);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Percentage (NumUsrsTotal ? (double) NumUsrs * 100.0 /
					      (double) NumUsrsTotal :
					      0.0);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write number of followed/followers per follower/followed *****/
      for (Fol = 0;
	   Fol < 2;
	   Fol++)
	{
	 Average = Fol_DB_GetNumFollowedPerFollower (Fol);

	 /***** Write number of followed per follower *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_FollowPerFollow[Fol]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Double2Decimals (Average);
	    HTM_TD_End ();

	    HTM_TD_Empty (1);

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
