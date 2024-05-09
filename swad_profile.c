// swad_profile.c: user's public profile

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_follow_database.h"
#include "swad_form.h"
#include "swad_forum_database.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_log_database.h"
#include "swad_message.h"
#include "swad_message_database.h"
#include "swad_network.h"
#include "swad_nickname.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_profile_database.h"
#include "swad_role.h"
#include "swad_role_type.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_theme.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_user.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prf_ShowTimeSinceFirstClick (const struct Usr_Data *UsrDat,
                                         const struct Prf_UsrFigures *UsrFigures);
static void Prf_ShowNumCrssWithRole (const struct Usr_Data *UsrDat,
				     Rol_Role_t Role);
static void Prf_ShowNumFilesCurrentlyPublished (const struct Usr_Data *UsrDat);
static void Prf_ShowNumClicks (const struct Usr_Data *UsrDat,
                               const struct Prf_UsrFigures *UsrFigures);
static void Prf_ShowNumFileViews (const struct Usr_Data *UsrDat,
                                  const struct Prf_UsrFigures *UsrFigures);
static void Prf_ShowNumTimelinePubs (const struct Usr_Data *UsrDat,
                                     const struct Prf_UsrFigures *UsrFigures);
static void Prf_ShowNumForumPosts (const struct Usr_Data *UsrDat,
                                   const struct Prf_UsrFigures *UsrFigures);
static void Prf_ShowNumMessagesSent (const struct Usr_Data *UsrDat,
                                     const struct Prf_UsrFigures *UsrFigures);
static void Prf_BeginListItem (const char *Title,const char *Icon);
static void Prf_EndListItem (void);
static void Prf_PutLinkCalculateFigures (const char *EncryptedUsrCod);

static void Prf_ShowRanking (unsigned Rank,unsigned NumUsrs);

static void Prf_GetFirstClickFromLogAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumClicksAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumTimelinePubsAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumFileViewsAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumForumPostsAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumMessagesSentAndStoreAsUsrFigure (long UsrCod);

static void Prf_ResetUsrFigures (struct Prf_UsrFigures *UsrFigures);

static void Prf_GetAndShowRankingFigure (const char *FldName);
static void Prf_ShowUsrInRanking (struct Usr_Data *UsrDat,unsigned Rank,
                                  Usr_MeOrOther_t MeOrOther);

/*****************************************************************************/
/************* Suggest who to follow or request user's profile ***************/
/*****************************************************************************/

void Prf_SeeSocialProfiles (void)
  {
   if (Gbl.Usrs.Me.Logged)
      Fol_SuggestUsrsToFollowOnMainZone ();
   else
      Prf_ReqUserProfile ();
  }

/*****************************************************************************/
/************************** Get public profile URL ***************************/
/*****************************************************************************/

char *Prf_GetURLPublicProfile (char URL[WWW_MAX_BYTES_WWW + 1],
                               const char *NickWithoutArr)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   /***** Build URL using nickname *****/
   snprintf (URL,WWW_MAX_BYTES_WWW + 1,"%s/%s?usr=@%s",
	     Cfg_URL_SWAD_CGI,Lan_STR_LANG_ID[Gbl.Prefs.Language],
	     NickWithoutArr);

   return URL;
  }

/*****************************************************************************/
/******************** Put link to view my public profile *********************/
/*****************************************************************************/

void Prf_PutLinkMyPublicProfile (void)
  {
   extern const char *Txt_My_public_profile;

   Lay_PutContextualLinkIconText (ActSeeOthPubPrf,NULL,
				  Usr_PutParMyUsrCodEncrypted,Gbl.Usrs.Me.UsrDat.EnUsrCod,
				  "user-circle.svg",Ico_BLACK,
				  Txt_My_public_profile,NULL);
  }

/*****************************************************************************/
/***************** Put link to request another user's profile ****************/
/*****************************************************************************/

void Prf_PutLinkReqAnotherUsrProfile (void)
  {
   extern const char *Txt_Another_user_s_profile;

   Lay_PutContextualLinkIconText (ActReqOthPubPrf,NULL,
                                  NULL,NULL,
				  "user-circle.svg",Ico_BLACK,
				  Txt_Another_user_s_profile,NULL);
  }

/*****************************************************************************/
/************************** Request a user's profile *************************/
/*****************************************************************************/

void Prf_ReqUserProfile (void)
  {
   extern const char *Hlp_START_Profiles_view_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *Txt_Nickname;
   extern const char *Txt_Continue;
   char NickWithArr[Nck_MAX_BYTES_NICK_WITH_ARROBA + 1];

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Contextual menu *****/
      Mnu_ContextMenuBegin ();
	 Prf_PutLinkMyPublicProfile ();	// My public profile
	 Fol_PutLinkWhoToFollow ();	// Users to follow
      Mnu_ContextMenuEnd ();
     }

   /***** Begin form *****/
   Frm_BeginForm (ActSeeOthPubPrf);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Another_user_s_profile,NULL,NULL,
		    Hlp_START_Profiles_view_public_profile,Box_NOT_CLOSABLE);

	 /***** Form to request user's @nickname *****/
	 /* By default, the nickname is filled with my nickname
	    If no user logged ==> the nickname is empty */
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtColonNBSP (Txt_Nickname);

	    snprintf (NickWithArr,sizeof (NickWithArr),"@%s",
		      Gbl.Usrs.Me.UsrDat.Nickname);
	    HTM_INPUT_TEXT ("usr",sizeof (NickWithArr) - 1,NickWithArr,
			    HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"16\" class=\"INPUT_%s\" required=\"required\"",
			    The_GetSuffix ());
	 HTM_LABEL_End ();

      /***** Send button and end box *****/
      Box_BoxWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Continue);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Get user and show a user's profile *********************/
/*****************************************************************************/
// Gbl.Usrs.Other.UsrDat.UsrCod may be already taken. If not ==> try to get it

void Prf_GetUsrDatAndShowUserProfile (void)
  {
   struct Tml_Timeline Timeline;
   bool ProfileShown = false;

   /***** Get user's data *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0)
      Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   /***** Show profile and timeline *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_GET_ROLE_IN_CRS))
      /* Show profile */
      if (Prf_ShowUsrProfile (&Gbl.Usrs.Other.UsrDat))
	{
	 ProfileShown = true;

	 if (Gbl.Usrs.Me.Logged)	// Timeline visible only by logged users
	   {
	    /* Reset timeline context */
            Tml_ResetTimeline (&Timeline);

	    /* Show timeline */
	    HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);
	       Tml_ShowTimelineUsr (&Timeline);
	    HTM_SECTION_End ();
	   }
	}

   /***** If profile could not be shown... *****/
   if (!ProfileShown)
     {
      /* Show error message */
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

      /* Request a user's profile */
      Prf_ReqUserProfile ();
     }

   /***** If it's not me, mark possible notification as seen *****/
   if (Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_OTHER)	// Not me
      Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_FOLLOWER,
                                      Gbl.Usrs.Other.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************************** Show a user's profile ***************************/
/*****************************************************************************/
// Return false on error

bool Prf_ShowUsrProfile (struct Usr_Data *UsrDat)
  {
   static void (*PutLinkToUsrProfile[Usr_NUM_ME_OR_OTHER]) (void) =
     {
      [Usr_ME   ] = Prf_PutLinkReqAnotherUsrProfile,	// Request another user's profile
      [Usr_OTHER] = Prf_PutLinkMyPublicProfile		// My public profile
     };
   unsigned NumFollowing;
   unsigned NumFollowers;
   bool UsrFollowsMe;
   bool IFollowUsr;
   Usr_MeOrOther_t MeOrOther = Usr_ItsMe (UsrDat->UsrCod);

   /***** Check if I can see the public profile *****/
   if (Pri_CheckIfICanView (UsrDat->BaPrfVisibility,UsrDat) == Usr_CAN)
     {
      if (Gbl.Usrs.Me.Logged)
	{
	 /***** Contextual menu *****/
	 Mnu_ContextMenuBegin ();
	    PutLinkToUsrProfile[MeOrOther] ();	// Public profile
	    Fol_PutLinkWhoToFollow ();		// Users to follow
	 Mnu_ContextMenuEnd ();
	}

      /***** Shared record card *****/
      if (MeOrOther == Usr_OTHER &&		// If not me...
	  Gbl.Hierarchy.Level == Hie_CRS)	// ...and a course is selected
	{
	 /* Get user's role in current course */
	 UsrDat->Roles.InCurrentCrs = Rol_GetRoleUsrInCrs (UsrDat->UsrCod,
	                                                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);

	 /* Get if user has accepted enrolment in current course */
	 UsrDat->Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (UsrDat);
	}
      Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_PUBLIC,UsrDat,NULL);

      /***** Extended profile *****/
      if (Pri_CheckIfICanView (UsrDat->ExPrfVisibility,UsrDat) == Usr_CAN)
        {
         /***** Show details of user's profile *****/
         Prf_ShowDetailsUserProfile (UsrDat);

	 /***** Count following and followers *****/
	 Fol_GetNumFollow (UsrDat->UsrCod,&NumFollowing,&NumFollowers);
	 if (NumFollowing)
	    UsrFollowsMe = Fol_DB_CheckUsrIsFollowerOf (UsrDat->UsrCod,
						        Gbl.Usrs.Me.UsrDat.UsrCod);
	 else
	    UsrFollowsMe = false;
	 if (NumFollowers)
	    IFollowUsr   = Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
						        UsrDat->UsrCod);
	 else
	    IFollowUsr   = false;

	 /***** Show following and followers *****/
	 Fol_ShowFollowingAndFollowers (UsrDat,
					NumFollowing,NumFollowers,
					UsrFollowsMe,IFollowUsr);
        }

      return true;
     }
   return false;
  }

/*****************************************************************************/
/******************** Change my public profile visibility ********************/
/*****************************************************************************/

void Prf_ChangeBasicProfileVis (void)
  {
   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.BaPrfVisibility = Pri_GetParVisibility ("VisBasPrf",
	                                                      Pri_BASIC_PROFILE_ALLOWED_VIS);

   /***** Store public/private basic profile visibility in database *****/
   Set_DB_UpdateMySettingsAboutBasicProfile ();

   /***** Show form again *****/
   Set_EditSettings ();
  }

void Prf_ChangeExtendedProfileVis (void)
  {
   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.ExPrfVisibility = Pri_GetParVisibility ("VisExtPrf",
	                                                      Pri_EXTENDED_PROFILE_ALLOWED_VIS);

   /***** Store public/private extended profile visibility in database *****/
   Set_DB_UpdateMySettingsAboutExtendedProfile ();

   /***** Show form again *****/
   Set_EditSettings ();
  }

/*****************************************************************************/
/********************** Show details of user's profile ***********************/
/*****************************************************************************/

void Prf_ShowDetailsUserProfile (const struct Usr_Data *UsrDat)
  {
   bool UsrIsBannedFromRanking;
   struct Prf_UsrFigures UsrFigures;
   Rol_Role_t Role;

   /***** Get figures *****/
   Prf_GetUsrFigures (UsrDat->UsrCod,&UsrFigures);

   /***** Container *****/
   HTM_DIV_Begin ("class=\"PRF_FIG_CONT\"");

      /***** Left list *****/
      /* Begin left list */
      HTM_DIV_Begin ("class=\"PRF_FIG_LEFT_RIGHT\"");
	 HTM_UL_Begin ("class=\"PRF_FIG_UL DAT_STRONG_%s NOWRAP\"",
		       The_GetSuffix ());

	    /* Time since first click */
	    Prf_ShowTimeSinceFirstClick (UsrDat,&UsrFigures);

	    /* Number of courses in which the user has a role */
	    for (Role  = Rol_TCH;
		 Role >= Rol_STD;
		 Role--)
	       Prf_ShowNumCrssWithRole (UsrDat,Role);

	    /* Number of files currently published */
	    Prf_ShowNumFilesCurrentlyPublished (UsrDat);

	 /* End left list */
	 HTM_UL_End ();
      HTM_DIV_End ();

      /***** Right list *****/
      HTM_DIV_Begin ("class=\"PRF_FIG_LEFT_RIGHT\"");

	 UsrIsBannedFromRanking = Prf_DB_CheckIfUsrBanned (UsrDat->UsrCod);
	 if (!UsrIsBannedFromRanking)
	   {
	    /* Begin right list */
	    HTM_UL_Begin ("class=\"PRF_FIG_UL DAT_STRONG_%s NOWRAP\"",
			  The_GetSuffix ());

	       /* Number of clicks */
	       Prf_ShowNumClicks (UsrDat,&UsrFigures);

	       /* Number of file views */
	       Prf_ShowNumFileViews (UsrDat,&UsrFigures);

	       /* Number of timeline publications */
	       Prf_ShowNumTimelinePubs (UsrDat,&UsrFigures);

	       /* Number of posts in forums */
	       Prf_ShowNumForumPosts (UsrDat,&UsrFigures);

	       /* Number of messages sent */
	       Prf_ShowNumMessagesSent (UsrDat,&UsrFigures);

	    /* End right list */
	    HTM_UL_End ();
	   }

      HTM_DIV_End ();

   HTM_DIV_End ();
  }

/*****************************************************************************/
/************** Show time since first click in user's profile ****************/
/*****************************************************************************/

static void Prf_ShowTimeSinceFirstClick (const struct Usr_Data *UsrDat,
                                         const struct Prf_UsrFigures *UsrFigures)
  {
   extern const char *Txt_TIME_Since;
   extern const char *Txt_year;
   extern const char *Txt_years;
   extern const char *Txt_month;
   extern const char *Txt_months;
   extern const char *Txt_day;
   extern const char *Txt_days;
   char IdFirstClickTime[Frm_MAX_BYTES_ID + 1];
   int NumYears;
   int NumMonths;

   /***** Time since first click *****/
   Prf_BeginListItem (Txt_TIME_Since,"clock.svg");

   if (UsrFigures->FirstClickTimeUTC)
     {
      /* Create unique id */
      Frm_SetUniqueId (IdFirstClickTime);

      HTM_SPAN_Begin ("id=\"%s\"",IdFirstClickTime);
      HTM_SPAN_End ();
      if (UsrFigures->NumDays > 0)
        {
	 HTM_TxtF ("&nbsp;%s","(");
	 NumYears = UsrFigures->NumDays / 365;
	 if (NumYears)
	    HTM_TxtF ("%d&nbsp;%s",
		      NumYears,
		      (NumYears == 1) ? Txt_year :
				        Txt_years);
	 else		// Less than one year
	   {
	    NumMonths = UsrFigures->NumDays / 30;
	    if (NumMonths)
	       HTM_TxtF ("%d&nbsp;%s",
			 NumMonths,
			 (NumMonths == 1) ? Txt_month :
					    Txt_months);
	    else	// Less than one month
	       HTM_TxtF ("%d&nbsp;%s",
			 UsrFigures->NumDays,
			 (UsrFigures->NumDays == 1) ? Txt_day :
						      Txt_days);
	   }
	 HTM_Txt (")");
        }
      Dat_WriteLocalDateHMSFromUTC (IdFirstClickTime,UsrFigures->FirstClickTimeUTC,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    Dat_WRITE_TODAY |
				    Dat_WRITE_DATE_ON_SAME_DAY |
				    Dat_WRITE_HOUR |
				    Dat_WRITE_MINUTE);
     }
   else	// First click time is unknown or user never logged
      /***** Button to fetch and store user's figures *****/
      Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/*** Show number of courses in which the user has a role in user's profile ***/
/*****************************************************************************/

static void Prf_ShowNumCrssWithRole (const struct Usr_Data *UsrDat,
				     Rol_Role_t Role)
  {
   extern const char *Rol_Icons[Rol_NUM_ROLES];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_courses_ABBREVIATION;
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   unsigned NumCrss;

   /***** Number of courses in which the user has a given role *****/
   NumCrss = Enr_DB_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Role);

   Prf_BeginListItem (Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex],Rol_Icons[Role]);

      HTM_TxtF ("%u&nbsp;%s",NumCrss,Txt_courses_ABBREVIATION);

      if (NumCrss)
	 HTM_TxtF ("&nbsp;(%u&nbsp;%s/%u&nbsp;%s)",
		   Enr_DB_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Role,
					       (1 << Rol_NET) |
					       (1 << Rol_TCH)),
		   Txt_teachers_ABBREVIATION,
		   Enr_DB_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Role,
					       (1 << Rol_STD)),
		   Txt_students_ABBREVIATION);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/******** Show number of files currently published in user's profile *********/
/*****************************************************************************/

static void Prf_ShowNumFilesCurrentlyPublished (const struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Files_uploaded;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_public_FILES;
   unsigned NumFiles;
   unsigned NumPublicFiles;

   /***** Number of files currently published *****/
   if ((NumFiles = Brw_DB_GetNumFilesUsr (UsrDat->UsrCod)))
      NumPublicFiles = Brw_DB_GetNumPublicFilesUsr (UsrDat->UsrCod);
   else
      NumPublicFiles = 0;

   Prf_BeginListItem (Txt_Files_uploaded,"file.svg");

      HTM_TxtF ("%u&nbsp;%s",NumFiles,(NumFiles == 1) ? Txt_file :
						        Txt_files);
      HTM_NBSP ();
      HTM_Txt ("(");
      HTM_TxtF ("%u&nbsp;%s",NumPublicFiles,Txt_public_FILES);
      HTM_Txt (")");

   Prf_EndListItem ();
  }

/*****************************************************************************/
/****************** Show number of clicks in user's profile ******************/
/*****************************************************************************/

static void Prf_ShowNumClicks (const struct Usr_Data *UsrDat,
                               const struct Prf_UsrFigures *UsrFigures)
  {
   extern const char *Txt_Clicks;
   extern const char *Txt_clicks;
   extern const char *Txt_day;

   /***** Number of clicks *****/
   Prf_BeginListItem (Txt_Clicks,"mouse-pointer.svg");

      if (UsrFigures->NumClicks >= 0)
	{
	 HTM_Long (UsrFigures->NumClicks);
	 HTM_TxtF ("&nbsp;%s&nbsp;",Txt_clicks);
	 Prf_ShowRanking (Prf_DB_GetUsrRankingFigure (UsrDat->UsrCod,"NumClicks"),
			  Prf_DB_GetNumUsrsWithFigure ("NumClicks"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumClicks /
				 (double) UsrFigures->NumDays);
	    HTM_TxtF ("/%s&nbsp;",Txt_day);
	    Prf_ShowRanking (Prf_DB_GetRankingNumClicksPerDay (UsrDat->UsrCod),
			     Prf_DB_GetNumUsrsWithNumClicksPerDay ());
	    HTM_Txt (")");
	   }
	}
      else	// Number of clicks is unknown
	 /***** Button to fetch and store user's figures *****/
	 Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/*************** Show number of file views in user's profile *****************/
/*****************************************************************************/

static void Prf_ShowNumFileViews (const struct Usr_Data *UsrDat,
                                  const struct Prf_UsrFigures *UsrFigures)
  {
   extern const char *Txt_Downloads;
   extern const char *Txt_download;
   extern const char *Txt_downloads;
   extern const char *Txt_day;

   /***** Number of file views *****/
   Prf_BeginListItem (Txt_Downloads,"download.svg");

      if (UsrFigures->NumFileViews >= 0)
	{
	 HTM_Long (UsrFigures->NumFileViews);
	 HTM_TxtF ("&nbsp;%s&nbsp;",(UsrFigures->NumFileViews == 1) ? Txt_download :
								      Txt_downloads);
	 Prf_ShowRanking (Prf_DB_GetUsrRankingFigure (UsrDat->UsrCod,"NumFileViews"),
			  Prf_DB_GetNumUsrsWithFigure ("NumFileViews"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumFileViews /
				 (double) UsrFigures->NumDays);
	    HTM_TxtF ("/%s)",Txt_day);
	   }
	}
      else	// Number of file views is unknown
	 /***** Button to fetch and store user's figures *****/
	 Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/********* Show number of timeline publications in user's profile ************/
/*****************************************************************************/

static void Prf_ShowNumTimelinePubs (const struct Usr_Data *UsrDat,
                                     const struct Prf_UsrFigures *UsrFigures)
  {
   extern const char *Txt_Timeline;
   extern const char *Txt_TIMELINE_post;
   extern const char *Txt_TIMELINE_posts;
   extern const char *Txt_day;

   /***** Number of timeline publications *****/
   Prf_BeginListItem (Txt_Timeline,"comment-dots.svg");

      if (UsrFigures->NumTimelinePubs >= 0)
	{
	 HTM_Int (UsrFigures->NumTimelinePubs);
	 HTM_TxtF ("&nbsp;%s&nbsp;",UsrFigures->NumTimelinePubs == 1 ? Txt_TIMELINE_post :
								 Txt_TIMELINE_posts);
	 Prf_ShowRanking (Prf_DB_GetUsrRankingFigure (UsrDat->UsrCod,"NumSocPub"),
			  Prf_DB_GetNumUsrsWithFigure ("NumSocPub"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumTimelinePubs /
				 (double) UsrFigures->NumDays);
	    HTM_TxtF ("/%s)",Txt_day);
	   }
	}
      else	// Number of timeline publications is unknown
	 /***** Button to fetch and store user's figures *****/
	 Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/********** Show number of timeline publications in user's profile ***********/
/*****************************************************************************/

static void Prf_ShowNumForumPosts (const struct Usr_Data *UsrDat,
                                   const struct Prf_UsrFigures *UsrFigures)
  {
   extern const char *Txt_Forums;
   extern const char *Txt_FORUM_post;
   extern const char *Txt_FORUM_posts;
   extern const char *Txt_day;

   /***** Number of posts in forums *****/
   Prf_BeginListItem (Txt_Forums,"comments.svg");

      if (UsrFigures->NumForumPosts >= 0)
	{
	 HTM_Long (UsrFigures->NumForumPosts);
	 HTM_TxtF ("&nbsp;%s&nbsp;",UsrFigures->NumForumPosts == 1 ? Txt_FORUM_post :
								 Txt_FORUM_posts);
	 Prf_ShowRanking (Prf_DB_GetUsrRankingFigure (UsrDat->UsrCod,"NumForPst"),
			  Prf_DB_GetNumUsrsWithFigure ("NumForPst"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumForumPosts /
				 (double) UsrFigures->NumDays);
	    HTM_TxtF ("/%s)",Txt_day);
	   }
	}
      else	// Number of forum posts is unknown
	 /***** Button to fetch and store user's figures *****/
	 Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/************** Show number of messages sent in user's profile ***************/
/*****************************************************************************/

static void Prf_ShowNumMessagesSent (const struct Usr_Data *UsrDat,
                                     const struct Prf_UsrFigures *UsrFigures)
  {
   extern const char *Txt_Messages;
   extern const char *Txt_message;
   extern const char *Txt_messages;
   extern const char *Txt_day;

   /***** Number of messages sent *****/
   Prf_BeginListItem (Txt_Messages,"envelope.svg");

      if (UsrFigures->NumMessagesSent >= 0)
	{
	 HTM_Long (UsrFigures->NumMessagesSent);
	 HTM_TxtF ("&nbsp;%s&nbsp;",UsrFigures->NumMessagesSent == 1 ? Txt_message :
								 Txt_messages);
	 Prf_ShowRanking (Prf_DB_GetUsrRankingFigure (UsrDat->UsrCod,"NumMsgSnt"),
			  Prf_DB_GetNumUsrsWithFigure ("NumMsgSnt"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumMessagesSent /
				 (double) UsrFigures->NumDays);
	    HTM_TxtF ("/%s)",Txt_day);
	   }
	}
      else	// Number of messages sent is unknown
	 /***** Button to fetch and store user's figures *****/
	 Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/****************** Begin/end list item in user's profile ********************/
/*****************************************************************************/

static void Prf_BeginListItem (const char *Title,const char *Icon)
  {
   HTM_LI_Begin ("title=\"%s\" class=\"PRF_FIG_LI\"",Title);
      HTM_DIV_Begin ("class=\"PRF_FIG_ICO\"");
	 Ico_PutIcon (Icon,Ico_BLACK,Title,"CONTEXT_ICOx16");
      HTM_DIV_End ();
  }

static void Prf_EndListItem (void)
  {
   HTM_LI_End ();
  }

/*****************************************************************************/
/******** Put contextual link with animated icon to update an action *********/
/*****************************************************************************/

static void Prf_PutLinkCalculateFigures (const char *EncryptedUsrCod)
  {
   extern const char *Txt_Calculate;

   Frm_BeginForm (ActCalFig);
      Usr_PutParUsrCodEncrypted (EncryptedUsrCod);

      /***** Put icon to refresh *****/
      HTM_BUTTON_Submit_Begin (Txt_Calculate,
			       "class=\"BT_LINK FORM_OUT_%s BOLD\"",
			       The_GetSuffix ());
	 Ico_PutIconTextUpdate (Txt_Calculate);
      HTM_BUTTON_End ();

   Frm_EndForm ();
  }

/*****************************************************************************/
/********************** Select values on user's figures **********************/
/*****************************************************************************/

void Prf_GetUsrFigures (long UsrCod,struct Prf_UsrFigures *UsrFigures)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get user's figures from database *****/
   if (Prf_DB_GetUsrFigures (&mysql_res,UsrCod))
     {
      /***** Get user's figures *****/
      row = mysql_fetch_row (mysql_res);

      /* Get first click (row[0] holds first click time UTC) */
      UsrFigures->FirstClickTimeUTC = Dat_GetUNIXTimeFromStr (row[0]);

      /* Get number of days since first click (row[1]) */
      if (UsrFigures->FirstClickTimeUTC)
	{
	 if (sscanf (row[1],"%d",&UsrFigures->NumDays) != 1)
	    UsrFigures->NumDays = -1;
	}
      else
	 UsrFigures->NumDays = -1;

      /* Get number of clicks (row[2]) */
      if (sscanf (row[2],"%d",&UsrFigures->NumClicks) != 1)
	 UsrFigures->NumClicks = -1;

      /* Get number of timeline publications (row[3]) */
      if (sscanf (row[3],"%d",&UsrFigures->NumTimelinePubs) != 1)
	 UsrFigures->NumTimelinePubs = -1;

      /* Get number of file views (row[4]) */
      if (sscanf (row[4],"%d",&UsrFigures->NumFileViews) != 1)
	 UsrFigures->NumFileViews = -1;

      /* Get number of forum posts (row[5]) */
      if (sscanf (row[5],"%d",&UsrFigures->NumForumPosts) != 1)
	 UsrFigures->NumForumPosts = -1;

      /* Get number of messages sent (row[6]) */
      if (sscanf (row[6],"%d",&UsrFigures->NumMessagesSent) != 1)
	 UsrFigures->NumMessagesSent = -1;
     }
   else
      /***** Return special user's figures indicating "not present" *****/
      Prf_ResetUsrFigures (UsrFigures);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Show position in ranking **************************/
/*****************************************************************************/

static void Prf_ShowRanking (unsigned Rank,unsigned NumUsrs)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   char *Title;

   /***** Rank in form to go to ranking *****/
   Frm_BeginForm (ActSeeUseGbl);
      Sco_PutParScope ("ScopeSta",Hie_SYS);
      Par_PutParUnsigned (NULL,"FigureType",(unsigned) Fig_USERS_RANKING);
      if (asprintf (&Title,"#%u %s %u",
		    Rank,Txt_of_PART_OF_A_TOTAL,NumUsrs) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_BUTTON_Submit_Begin (Title,
                               "class=\"BT_LINK FORM_OUT_%s\"",
                               The_GetSuffix ());
      free (Title);
	 HTM_TxtF ("#%u",Rank);
      HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/********** Calculate user's figures and show user's profile again ***********/
/*****************************************************************************/

void Prf_CalculateFigures (void)
  {
   struct Prf_UsrFigures UsrFigures;

   /***** Get user's code *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get first click time from log and store as user's figure *****/
   Prf_GetUsrFigures (Gbl.Usrs.Other.UsrDat.UsrCod,&UsrFigures);
   Prf_GetAndStoreAllUsrFigures (Gbl.Usrs.Other.UsrDat.UsrCod,&UsrFigures);

   /***** Show user's profile again *****/
   Prf_GetUsrDatAndShowUserProfile ();
  }

/*****************************************************************************/
/*** Calculate and store all figures in user's profile not yet calculated ****/
/*****************************************************************************/
// Return true if any figure has been calculated and stored

bool Prf_GetAndStoreAllUsrFigures (long UsrCod,struct Prf_UsrFigures *UsrFigures)
  {
   bool UsrFiguresModified = false;

   if (!UsrFigures->FirstClickTimeUTC)
     {
      /***** Get first click time from log and store as user's figure *****/
      Prf_GetFirstClickFromLogAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
   if (UsrFigures->NumClicks < 0)
     {
      /***** Get number of clicks and store as user's figure *****/
      Prf_GetNumClicksAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
   if (UsrFigures->NumTimelinePubs < 0)
     {
      /***** Get number of timeline publications and store as user's figure *****/
      Prf_GetNumTimelinePubsAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
   if (UsrFigures->NumFileViews < 0)
     {
      /***** Get number of file views and store as user's figure *****/
      Prf_GetNumFileViewsAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
   if (UsrFigures->NumForumPosts < 0)
     {
      /***** Get number of forum posts and store as user's figure *****/
      Prf_GetNumForumPostsAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
    if (UsrFigures->NumMessagesSent < 0)
     {
      /***** Get number of messages sent and store as user's figure *****/
      Prf_GetNumMessagesSentAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }

   return UsrFiguresModified;
  }

/*****************************************************************************/
/*** Get first click of a user from log table and store in user's figures ****/
/*****************************************************************************/

static void Prf_GetFirstClickFromLogAndStoreAsUsrFigure (long UsrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Prf_UsrFigures UsrFigures;

   if (Usr_DB_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get first click from log table *****/
      if (Log_DB_GetUsrFirstClick (&mysql_res,UsrCod))
	{
	 /* Get first click */
	 row = mysql_fetch_row (mysql_res);

	 /* Get first click (row[0] holds the start date-time UTC) */
	 if (row[0])	// It is NULL when user never logged
	    UsrFigures.FirstClickTimeUTC = Dat_GetUNIXTimeFromStr (row[0]);
	}
      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Update first click time in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 Prf_DB_UpdateFirstClickTimeUsr (UsrCod,UsrFigures.FirstClickTimeUTC);
      else			// User entry does not exist
	 Prf_DB_CreateUsrFigures (UsrCod,&UsrFigures,Usr_OTHER);
     }
  }

/*****************************************************************************/
/* Get number of clicks of a user from log table and store in user's figures */
/*****************************************************************************/

static void Prf_GetNumClicksAndStoreAsUsrFigure (long UsrCod)
  {
   struct Prf_UsrFigures UsrFigures;

   if (Usr_DB_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of clicks from database *****/
      UsrFigures.NumClicks = (int) Log_DB_GetUsrNumClicks (UsrCod);

      /***** Update number of clicks in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 Prf_DB_UpdateNumClicksUsr (UsrCod,UsrFigures.NumClicks);
      else			// User entry does not exist
	 Prf_DB_CreateUsrFigures (UsrCod,&UsrFigures,Usr_OTHER);
     }
  }

/*****************************************************************************/
/**** Get number of timeline pubs. from a user and store in user's figures ***/
/*****************************************************************************/

static void Prf_GetNumTimelinePubsAndStoreAsUsrFigure (long UsrCod)
  {
   struct Prf_UsrFigures UsrFigures;

   if (Usr_DB_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of forum posts from database *****/
      UsrFigures.NumTimelinePubs = (int) Tml_DB_GetNumPubsUsr (UsrCod);

      /***** Update number of timeline publications in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 Prf_DB_UpdateNumTimelinePubsUsr (UsrCod,UsrFigures.NumTimelinePubs);
      else			// User entry does not exist
	 Prf_DB_CreateUsrFigures (UsrCod,&UsrFigures,Usr_OTHER);
     }
  }

/*****************************************************************************/
/**** Get number of file views sent by a user and store in user's figures ****/
/*****************************************************************************/

static void Prf_GetNumFileViewsAndStoreAsUsrFigure (long UsrCod)
  {
   struct Prf_UsrFigures UsrFigures;

   if (Usr_DB_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of file views from database *****/
      UsrFigures.NumFileViews = (long) Brw_DB_GetNumFileViewsUsr (UsrCod);

      /***** Update number of file views in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 Prf_DB_UpdateNumFileViewsUsr (UsrCod,UsrFigures.NumFileViews);
      else			// User entry does not exist
	 Prf_DB_CreateUsrFigures (UsrCod,&UsrFigures,Usr_OTHER);
     }
  }

/*****************************************************************************/
/**** Get number of forum posts sent by a user and store in user's figures ***/
/*****************************************************************************/

static void Prf_GetNumForumPostsAndStoreAsUsrFigure (long UsrCod)
  {
   struct Prf_UsrFigures UsrFigures;

   if (Usr_DB_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of forum posts from database *****/
      UsrFigures.NumForumPosts = For_DB_GetNumPostsUsr (UsrCod);

      /***** Update number of forum posts in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 Prf_DB_UpdateNumForumPostsUsr (UsrCod,UsrFigures.NumForumPosts);
      else			// User entry does not exist
	 Prf_DB_CreateUsrFigures (UsrCod,&UsrFigures,Usr_OTHER);
     }
  }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

static void Prf_GetNumMessagesSentAndStoreAsUsrFigure (long UsrCod)
  {
   struct Prf_UsrFigures UsrFigures;

   if (Usr_DB_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of messages sent from database *****/
      UsrFigures.NumMessagesSent = (int) Msg_DB_GetNumMsgsSentByUsr (UsrCod);

      /***** Update number of messages sent in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 Prf_DB_UpdateNumMessagesSentUsr (UsrCod,UsrFigures.NumMessagesSent);
      else			// User entry does not exist
	 Prf_DB_CreateUsrFigures (UsrCod,&UsrFigures,Usr_OTHER);
     }
  }

/*****************************************************************************/
/******************* Create user's figures for a new user ********************/
/*****************************************************************************/

void Prf_CreateNewUsrFigures (long UsrCod,Usr_MeOrOther_t MeOrOther)
  {
   struct Prf_UsrFigures UsrFigures;

   /***** Reset user's figures *****/
   Prf_ResetUsrFigures (&UsrFigures);
   UsrFigures.NumClicks       = 0;	// set number of clicks to 0
   UsrFigures.NumTimelinePubs = 0;	// set number of timeline publications to 0
   UsrFigures.NumFileViews    = 0;	// set number of file views to 0
   UsrFigures.NumForumPosts   = 0;	// set number of forum posts to 0
   UsrFigures.NumMessagesSent = 0;	// set number of messages sent to 0

   /***** Create user's figures *****/
   Prf_DB_CreateUsrFigures (UsrCod,&UsrFigures,MeOrOther);
  }

/*****************************************************************************/
/********************** Reset values of user's figures ***********************/
/*****************************************************************************/

static void Prf_ResetUsrFigures (struct Prf_UsrFigures *UsrFigures)
  {
   UsrFigures->FirstClickTimeUTC = (time_t) 0;	// unknown first click time or user never logged
   UsrFigures->NumDays         = -1;	// not applicable
   UsrFigures->NumClicks       = -1;	// unknown number of clicks
   UsrFigures->NumTimelinePubs = -1;	// unknown number of timeline publications
   UsrFigures->NumFileViews    = -1;	// unknown number of file views
   UsrFigures->NumForumPosts   = -1;	// unknown number of forum posts
   UsrFigures->NumMessagesSent = -1;	// unknown number of messages sent
  }

/*****************************************************************************/
/******** Get and show ranking of users attending to number of clicks ********/
/*****************************************************************************/

void Prf_GetAndShowRankingClicks (void)
  {
   Prf_GetAndShowRankingFigure ("NumClicks");
  }

void Prf_GetAndShowRankingTimelinePubs (void)
  {
   Prf_GetAndShowRankingFigure ("NumSocPub");
  }

void Prf_GetAndShowRankingFileViews (void)
  {
   Prf_GetAndShowRankingFigure ("NumFileViews");
  }

void Prf_GetAndShowRankingForPsts (void)
  {
   Prf_GetAndShowRankingFigure ("NumForPst");
  }

void Prf_GetAndShowRankingMsgsSnt (void)
  {
   Prf_GetAndShowRankingFigure ("NumMsgSnt");
  }

static void Prf_GetAndShowRankingFigure (const char *FldName)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;

   /***** Get ranking from database *****/
   NumUsrs = Prf_DB_GetRankingFigure (&mysql_res,FldName);

   Prf_ShowRankingFigure (&mysql_res,NumUsrs);
  }

void Prf_ShowRankingFigure (MYSQL_RES **mysql_res,unsigned NumUsrs)
  {
   static const char *Class[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = "DAT_SMALL_STRONG",
      [Usr_OTHER] = "DAT_SMALL"
     };
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned Rank;
   struct Usr_Data UsrDat;
   Usr_MeOrOther_t MeOrOther;
   long FigureHigh = LONG_MAX;
   long Figure;

   if (NumUsrs)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      HTM_TABLE_Begin (NULL);

	 for (NumUsr  = 1, Rank = 1, The_ResetRowColor ();
	      NumUsr <= NumUsrs;
	      NumUsr++, The_ChangeRowColor ())
	   {
	    /***** Get user and number of clicks *****/
	    row = mysql_fetch_row (*mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	    Usr_GetAllUsrDataFromUsrCod (&UsrDat,
					 Usr_DONT_GET_PREFS,
					 Usr_DONT_GET_ROLE_IN_CRS);

	    /* Get figure (row[1]) */
	    if (sscanf (row[1],"%ld",&Figure) != 1)
	       Err_ShowErrorAndExit ("Error reading user's figure.");

	    if (Figure < FigureHigh)
	      {
	       Rank = NumUsr;
	       FigureHigh = Figure;
	      }

	    /***** Show row *****/
	    HTM_TR_Begin (NULL);
	       MeOrOther = Usr_ItsMe (UsrDat.UsrCod);
	       Prf_ShowUsrInRanking (&UsrDat,Rank,MeOrOther);
	       HTM_TD_Begin ("class=\"RM %s_%s %s\"",
			     Class[MeOrOther],
		             The_GetSuffix (),
			     The_GetColorRows ());
		  HTM_Long (Figure);
	       HTM_TD_End ();
	    HTM_TR_End ();
	   }

      HTM_TABLE_End ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/**** Get and show ranking of users attending to number of clicks per day ****/
/*****************************************************************************/

void Prf_GetAndShowRankingClicksPerDay (void)
  {
   static const char *Class[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = "DAT_SMALL_STRONG",
      [Usr_OTHER] = "DAT_SMALL"
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs = 0;	// Initialized to avoid warning
   unsigned NumUsr;
   unsigned Rank;
   struct Usr_Data UsrDat;
   Usr_MeOrOther_t MeOrOther;
   double NumClicksPerDayHigh = (double) LONG_MAX;
   double NumClicksPerDay;

   /***** Get ranking from database *****/
   if ((NumUsrs = Prf_DB_GetRankingClicksPerDay (&mysql_res)))
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      HTM_TABLE_Begin (NULL);

	 for (NumUsr  = 1, Rank = 1, The_ResetRowColor ();
	      NumUsr <= NumUsrs;
	      NumUsr++, The_ChangeRowColor ())
	   {
	    /***** Get user and number of clicks *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	    Usr_GetAllUsrDataFromUsrCod (&UsrDat,
					 Usr_DONT_GET_PREFS,
					 Usr_DONT_GET_ROLE_IN_CRS);

	    /* Get average number of clicks per day (row[1]) */
	    NumClicksPerDay = Str_GetDoubleFromStr (row[1]);
	    if (NumClicksPerDay < NumClicksPerDayHigh)
	      {
	       Rank = NumUsr;
	       NumClicksPerDayHigh = NumClicksPerDay;
	      }

	    /***** Show row *****/
	    HTM_TR_Begin (NULL);
	       MeOrOther = Usr_ItsMe (UsrDat.UsrCod);
	       Prf_ShowUsrInRanking (&UsrDat,Rank,MeOrOther);
	       HTM_TD_Begin ("class=\"RM %s_%s %s\"",Class[MeOrOther],
			     The_GetSuffix (),
			     The_GetColorRows ());
		  HTM_DoubleFewDigits (NumClicksPerDay);
	       HTM_TD_End ();
	    HTM_TR_End ();
	   }

      HTM_TABLE_End ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Show user's photo and nickname in ranking list ***************/
/*****************************************************************************/

static void Prf_ShowUsrInRanking (struct Usr_Data *UsrDat,unsigned Rank,
                                  Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Another_user_s_profile;
   static const char *Class[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = "DAT_SMALL_STRONG",
      [Usr_OTHER] = "DAT_SMALL"
     };
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC30x40",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE30x40",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO30x40",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR30x40",
     };
   Usr_Can_t ICanView = Pri_CheckIfICanView (UsrDat->BaPrfVisibility,UsrDat);

   HTM_TD_Begin ("class=\"RM %s_%s %s\"",
		 Class[MeOrOther],
		 The_GetSuffix (),
                 The_GetColorRows ());
      HTM_TxtF ("#%u",Rank);
   HTM_TD_End ();

   /***** Check if I can see the public profile *****/
   HTM_TD_Begin ("class=\"RANK_PHOTO %s\"",The_GetColorRows ());
      if (ICanView == Usr_CAN)
	 /***** User's photo *****/
	 Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                            ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
   HTM_TD_End ();

   /***** Put form to go to public profile *****/
   HTM_TD_Begin ("class=\"RANK_USR %s_%s %s\"",	// Limited width
                 Class[MeOrOther],
	         The_GetSuffix (),
                 The_GetColorRows ());
      if (ICanView == Usr_CAN)
	{
	 Frm_BeginForm (ActSeeOthPubPrf);
	    Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	    HTM_BUTTON_Submit_Begin (Txt_Another_user_s_profile,
				     "class=\"LM BT_LINK\"");
	       Usr_WriteFirstNameBRSurnames (UsrDat);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
	}
   HTM_TD_End ();
  }
