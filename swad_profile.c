// swad_profile.c: user's public profile

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_follow_database.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_message.h"
#include "swad_network.h"
#include "swad_nickname.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_role.h"
#include "swad_role_type.h"
#include "swad_setting.h"
#include "swad_theme.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prf_ShowTimeSinceFirstClick (const struct UsrData *UsrDat,
                                         const struct UsrFigures *UsrFigures);
static void Prf_ShowNumCrssWithRole (const struct UsrData *UsrDat,
				     Rol_Role_t Role);
static void Prf_ShowNumFilesCurrentlyPublished (const struct UsrData *UsrDat);
static void Prf_ShowNumClicks (const struct UsrData *UsrDat,
                               const struct UsrFigures *UsrFigures);
static void Prf_ShowNumFileViews (const struct UsrData *UsrDat,
                                  const struct UsrFigures *UsrFigures);
static void Prf_ShowNumSocialPublications (const struct UsrData *UsrDat,
                                           const struct UsrFigures *UsrFigures);
static void Prf_ShowNumForumPosts (const struct UsrData *UsrDat,
                                   const struct UsrFigures *UsrFigures);
static void Prf_ShowNumMessagesSent (const struct UsrData *UsrDat,
                                     const struct UsrFigures *UsrFigures);
static void Prf_BeginListItem (const char *Title,const char *Icon);
static void Prf_EndListItem (void);
static void Prf_PutLinkCalculateFigures (const char *EncryptedUsrCod);

static unsigned Prf_DB_GetRankingFigure (long UsrCod,const char *FieldName);
static unsigned Prf_DB_GetNumUsrsWithFigure (const char *FieldName);
static unsigned Prf_DB_GetRankingNumClicksPerDay (long UsrCod);
static unsigned Prf_DB_GetNumUsrsWithNumClicksPerDay (void);
static void Prf_ShowRanking (unsigned Rank,unsigned NumUsrs);

static void Prf_GetFirstClickFromLogAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumClicksAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumSocialPubsAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumFileViewsAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumForumPostsAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumMessagesSentAndStoreAsUsrFigure (long UsrCod);

static void Prf_ResetUsrFigures (struct UsrFigures *UsrFigures);
static void Prf_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures,
                                  bool CreatingMyOwnAccount);
static bool Prf_DB_CheckIfUsrFiguresExists (long UsrCod);

static void Prf_GetAndShowRankingFigure (const char *FieldName);
static void Prf_ShowUsrInRanking (struct UsrData *UsrDat,unsigned Rank,bool ItsMe);

/*****************************************************************************/
/************* Suggest who to follow or request user's profile ***************/
/*****************************************************************************/

void Prf_SeeSocialProfiles (void)
  {
   if (Gbl.Usrs.Me.Logged)
      Fol_SuggestUsrsToFollowMainZone ();
   else
      Prf_RequestUserProfile ();
  }

/*****************************************************************************/
/************************** Get public profile URL ***************************/
/*****************************************************************************/

char *Prf_GetURLPublicProfile (char URL[Cns_MAX_BYTES_WWW + 1],
                               const char *NickWithoutArr)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   /***** Build URL using nickname *****/
   snprintf (URL,Cns_MAX_BYTES_WWW + 1,"%s/%s?usr=@%s",
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
				  Usr_PutParamMyUsrCodEncrypted,Gbl.Usrs.Me.UsrDat.EnUsrCod,
				  "user-circle.svg",
				  Txt_My_public_profile);
  }

/*****************************************************************************/
/***************** Put link to request another user's profile ****************/
/*****************************************************************************/

void Prf_PutLinkRequestAnotherUserProfile (void)
  {
   extern const char *Txt_Another_user_s_profile;

   Lay_PutContextualLinkIconText (ActReqOthPubPrf,NULL,
                                  NULL,NULL,
				  "user-circle.svg",
				  Txt_Another_user_s_profile);
  }

/*****************************************************************************/
/************************** Request a user's profile *************************/
/*****************************************************************************/

void Prf_RequestUserProfile (void)
  {
   extern const char *Hlp_START_Profiles_view_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Nickname;
   extern const char *Txt_Continue;
   char NickWithArr[1 + Nck_MAX_BYTES_NICK_WITHOUT_ARROBA + 1];

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
      Box_BoxBegin (NULL,Txt_Another_user_s_profile,
		    NULL,NULL,
		    Hlp_START_Profiles_view_public_profile,Box_NOT_CLOSABLE);

	 /***** Form to request user's @nickname *****/
	 /* By default, the nickname is filled with my nickname
	    If no user logged ==> the nickname is empty */
	 HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	    HTM_TxtColonNBSP (Txt_Nickname);

	    snprintf (NickWithArr,sizeof (NickWithArr),"@%s",
		      Gbl.Usrs.Me.UsrDat.Nickname);
	    HTM_INPUT_TEXT ("usr",Nck_MAX_BYTES_NICK_FROM_FORM,NickWithArr,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"18\"");
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
   bool ItsMe;
   bool ProfileShown = false;

   /***** Get user's data *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0)
      Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Show profile and timeline *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
      /* Show profile */
      if (Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat))
	{
	 ProfileShown = true;

	 if (Gbl.Usrs.Me.Logged)	// Timeline visible only by logged users
	   {
	    /* Reset timeline context */
            Tml_ResetTimeline (&Timeline);

	    /* Show timeline */
	    HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);
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
      Prf_RequestUserProfile ();
     }

   /***** If it's not me, mark possible notification as seen *****/
   ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
   if (!ItsMe)	// Not me
      Ntf_MarkNotifAsSeen (Ntf_EVENT_FOLLOWER,
                           Gbl.Usrs.Other.UsrDat.UsrCod,-1L,
			   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************************** Show a user's profile ***************************/
/*****************************************************************************/
// Return false on error

bool Prf_ShowUserProfile (struct UsrData *UsrDat)
  {
   unsigned NumFollowing;
   unsigned NumFollowers;
   bool UsrFollowsMe;
   bool IFollowUsr;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   /***** Check if I can see the public profile *****/
   if (Pri_ShowingIsAllowed (UsrDat->BaPrfVisibility,UsrDat))
     {
      if (Gbl.Usrs.Me.Logged)
	{
	 /***** Contextual menu *****/
	 Mnu_ContextMenuBegin ();
	    if (ItsMe)
	       Prf_PutLinkRequestAnotherUserProfile ();	// Request another user's profile
	    else	// Not me
	       Prf_PutLinkMyPublicProfile ();		// My public profile
	    Fol_PutLinkWhoToFollow ();			// Users to follow
	 Mnu_ContextMenuEnd ();
	}

      /***** Shared record card *****/
      if (!ItsMe &&				// If not me...
	  Gbl.Hierarchy.Level == HieLvl_CRS)	// ...and a course is selected
	{
	 /* Get user's role in current course */
	 UsrDat->Roles.InCurrentCrs = Rol_GetRoleUsrInCrs (UsrDat->UsrCod,
	                                                   Gbl.Hierarchy.Crs.CrsCod);

	 /* Get if user has accepted enrolment in current course */
	 UsrDat->Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (UsrDat);
	}
      Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_PUBLIC,UsrDat,NULL);

      /***** Extended profile *****/
      if (Pri_ShowingIsAllowed (UsrDat->ExPrfVisibility,UsrDat))
        {
         /***** Show details of user's profile *****/
         Prf_ShowDetailsUserProfile (UsrDat);

	 /***** Count following and followers *****/
	 Fol_GetNumFollow (UsrDat->UsrCod,&NumFollowing,&NumFollowers);
	 UsrFollowsMe = false;
	 if (NumFollowing)
	    UsrFollowsMe = Fol_DB_CheckUsrIsFollowerOf (UsrDat->UsrCod,
						        Gbl.Usrs.Me.UsrDat.UsrCod);
	 IFollowUsr   = false;
	 if (NumFollowers)
	    IFollowUsr   = Fol_DB_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
						        UsrDat->UsrCod);

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
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.BaPrfVisibility = Pri_GetParamVisibility ("VisBasPrf",
	                                                        Pri_BASIC_PROFILE_ALLOWED_VIS);

   /***** Store public/private photo in database *****/
   DB_QueryUPDATE ("can not update your setting"
		   " about public profile visibility",
		   "UPDATE usr_data"
		     " SET BaPrfVisibility='%s'"
		   " WHERE UsrCod=%ld",
                   Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.BaPrfVisibility],
                   Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show form again *****/
   Set_EditSettings ();
  }

void Prf_ChangeExtendedProfileVis (void)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.ExPrfVisibility = Pri_GetParamVisibility ("VisExtPrf",
	                                                        Pri_EXTENDED_PROFILE_ALLOWED_VIS);

   /***** Store public/private photo in database *****/
   DB_QueryUPDATE ("can not update your setting"
		   " about public profile visibility",
		   "UPDATE usr_data"
		     " SET ExPrfVisibility='%s'"
		   " WHERE UsrCod=%ld",
                   Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.ExPrfVisibility],
                   Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show form again *****/
   Set_EditSettings ();
  }

/*****************************************************************************/
/********************** Show details of user's profile ***********************/
/*****************************************************************************/

void Prf_ShowDetailsUserProfile (const struct UsrData *UsrDat)
  {
   bool UsrIsBannedFromRanking;
   struct UsrFigures UsrFigures;
   Rol_Role_t Role;

   /***** Get figures *****/
   Prf_GetUsrFigures (UsrDat->UsrCod,&UsrFigures);

   /***** Left list *****/
   /* Begin left list */
   HTM_DIV_Begin ("class=\"PRF_FIG_LEFT_CONT\"");
      HTM_UL_Begin ("class=\"PRF_FIG_UL DAT_NOBR_N\"");

	 /* Time since first click */
	 Prf_ShowTimeSinceFirstClick (UsrDat,&UsrFigures);

	 /* Number of courses in which the user has a role */
	 for (Role = Rol_TCH;
	      Role >= Rol_STD;
	      Role--)
	    Prf_ShowNumCrssWithRole (UsrDat,Role);

	 /* Number of files currently published */
	 Prf_ShowNumFilesCurrentlyPublished (UsrDat);

      /* End left list */
      HTM_UL_End ();
   HTM_DIV_End ();

   /***** Right list *****/
   HTM_DIV_Begin ("class=\"PRF_FIG_RIGHT_CONT\"");

      UsrIsBannedFromRanking = Usr_DB_CheckIfUsrBanned (UsrDat->UsrCod);
      if (!UsrIsBannedFromRanking)
	{
	 /* Begin right list */
	 HTM_UL_Begin ("class=\"PRF_FIG_UL DAT_NOBR_N\"");

	    /* Number of clicks */
	    Prf_ShowNumClicks (UsrDat,&UsrFigures);

	    /* Number of file views */
	    Prf_ShowNumFileViews (UsrDat,&UsrFigures);

	    /* Number of social publications */
	    Prf_ShowNumSocialPublications (UsrDat,&UsrFigures);

	    /* Number of posts in forums */
	    Prf_ShowNumForumPosts (UsrDat,&UsrFigures);

	    /* Number of messages sent */
	    Prf_ShowNumMessagesSent (UsrDat,&UsrFigures);

	 /* End right list */
	 HTM_UL_End ();
	}

   HTM_DIV_End ();
  }

/*****************************************************************************/
/************** Show time since first click in user's profile ****************/
/*****************************************************************************/

static void Prf_ShowTimeSinceFirstClick (const struct UsrData *UsrDat,
                                         const struct UsrFigures *UsrFigures)
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
				    true,true,false,0x6);
     }
   else	// First click time is unknown or user never logged
      /***** Button to fetch and store user's figures *****/
      Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/*** Show number of courses in which the user has a role in user's profile ***/
/*****************************************************************************/

static void Prf_ShowNumCrssWithRole (const struct UsrData *UsrDat,
				     Rol_Role_t Role)
  {
   extern const char *Rol_Icons[Rol_NUM_ROLES];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_courses_ABBREVIATION;
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   unsigned NumCrss;

   /***** Number of courses in which the user has a given role *****/
   NumCrss = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Role);

   Prf_BeginListItem (Txt_ROLES_SINGUL_Abc[Role][UsrDat->Sex],Rol_Icons[Role]);

      HTM_TxtF ("%u&nbsp;%s",NumCrss,Txt_courses_ABBREVIATION);

      if (NumCrss)
	 HTM_TxtF ("&nbsp;(%u&nbsp;%s/%u&nbsp;%s)",
		   Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Role,
					       (1 << Rol_NET) |
					       (1 << Rol_TCH)),
		   Txt_teachers_ABBREVIATION,
		   Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Role,
					       (1 << Rol_STD)),
		   Txt_students_ABBREVIATION);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/******** Show number of files currently published in user's profile *********/
/*****************************************************************************/

static void Prf_ShowNumFilesCurrentlyPublished (const struct UsrData *UsrDat)
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

static void Prf_ShowNumClicks (const struct UsrData *UsrDat,
                               const struct UsrFigures *UsrFigures)
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
	 Prf_ShowRanking (Prf_DB_GetRankingFigure (UsrDat->UsrCod,"NumClicks"),
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

static void Prf_ShowNumFileViews (const struct UsrData *UsrDat,
                                  const struct UsrFigures *UsrFigures)
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
	 Prf_ShowRanking (Prf_DB_GetRankingFigure (UsrDat->UsrCod,"NumFileViews"),
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
/*********** Show number of social publications in user's profile ************/
/*****************************************************************************/

static void Prf_ShowNumSocialPublications (const struct UsrData *UsrDat,
                                           const struct UsrFigures *UsrFigures)
  {
   extern const char *Txt_Timeline;
   extern const char *Txt_TIMELINE_post;
   extern const char *Txt_TIMELINE_posts;
   extern const char *Txt_day;

   /***** Number of social publications *****/
   Prf_BeginListItem (Txt_Timeline,"comment-dots.svg");

      if (UsrFigures->NumSocPub >= 0)
	{
	 HTM_Int (UsrFigures->NumSocPub);
	 HTM_TxtF ("&nbsp;%s&nbsp;",UsrFigures->NumSocPub == 1 ? Txt_TIMELINE_post :
								 Txt_TIMELINE_posts);
	 Prf_ShowRanking (Prf_DB_GetRankingFigure (UsrDat->UsrCod,"NumSocPub"),
			  Prf_DB_GetNumUsrsWithFigure ("NumSocPub"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumSocPub /
				 (double) UsrFigures->NumDays);
	    HTM_TxtF ("/%s)",Txt_day);
	   }
	}
      else	// Number of social publications is unknown
	 /***** Button to fetch and store user's figures *****/
	 Prf_PutLinkCalculateFigures (UsrDat->EnUsrCod);

   Prf_EndListItem ();
  }

/*****************************************************************************/
/*********** Show number of social publications in user's profile ************/
/*****************************************************************************/

static void Prf_ShowNumForumPosts (const struct UsrData *UsrDat,
                                   const struct UsrFigures *UsrFigures)
  {
   extern const char *Txt_Forums;
   extern const char *Txt_FORUM_post;
   extern const char *Txt_FORUM_posts;
   extern const char *Txt_day;

   /***** Number of posts in forums *****/
   Prf_BeginListItem (Txt_Forums,"comments.svg");

      if (UsrFigures->NumForPst >= 0)
	{
	 HTM_Long (UsrFigures->NumForPst);
	 HTM_TxtF ("&nbsp;%s&nbsp;",UsrFigures->NumForPst == 1 ? Txt_FORUM_post :
								 Txt_FORUM_posts);
	 Prf_ShowRanking (Prf_DB_GetRankingFigure (UsrDat->UsrCod,"NumForPst"),
			  Prf_DB_GetNumUsrsWithFigure ("NumForPst"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumForPst /
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

static void Prf_ShowNumMessagesSent (const struct UsrData *UsrDat,
                                     const struct UsrFigures *UsrFigures)
  {
   extern const char *Txt_Messages;
   extern const char *Txt_message;
   extern const char *Txt_messages;
   extern const char *Txt_day;

   /***** Number of messages sent *****/
   Prf_BeginListItem (Txt_Messages,"envelope.svg");

      if (UsrFigures->NumMsgSnt >= 0)
	{
	 HTM_Long (UsrFigures->NumMsgSnt);
	 HTM_TxtF ("&nbsp;%s&nbsp;",UsrFigures->NumMsgSnt == 1 ? Txt_message :
								 Txt_messages);
	 Prf_ShowRanking (Prf_DB_GetRankingFigure (UsrDat->UsrCod,"NumMsgSnt"),
			  Prf_DB_GetNumUsrsWithFigure ("NumMsgSnt"));
	 if (UsrFigures->NumDays > 0)
	   {
	    HTM_TxtF ("&nbsp;%s","(");
	    HTM_DoubleFewDigits ((double) UsrFigures->NumMsgSnt /
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
   HTM_LI_Begin ("title=\"%s\" class=\"PRF_FIG_LI\""
		 " style=\"background-image:url('%s/%s');\"",
		 Title,
		 Cfg_URL_ICON_PUBLIC,Icon);
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
   extern const char *The_ClassFormLinkOutBoxBold[The_NUM_THEMES];
   extern const char *Txt_Calculate;

   Frm_BeginForm (ActCalFig);
   Usr_PutParamUsrCodEncrypted (EncryptedUsrCod);
      HTM_BUTTON_Animated_Begin (Txt_Calculate,
				 The_ClassFormLinkOutBoxBold[Gbl.Prefs.Theme],
				 NULL);
	 Ico_PutCalculateIconWithText (Txt_Calculate);
      HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************** Select values on user's figures **********************/
/*****************************************************************************/

void Prf_GetUsrFigures (long UsrCod,struct UsrFigures *UsrFigures)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Get user's figures from database *****/
   NumRows = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get user's figures",
		   "SELECT UNIX_TIMESTAMP(FirstClickTime),"	// row[0]
			  "DATEDIFF(NOW(),FirstClickTime)+1,"	// row[1]
			  "NumClicks,"				// row[2]
			  "NumSocPub,"				// row[3]
			  "NumFileViews,"			// row[4]
			  "NumForPst,"				// row[5]
			  "NumMsgSnt"				// row[6]
		    " FROM usr_figures"
		   " WHERE UsrCod=%ld",
		   UsrCod);
   if (NumRows)
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

      /* Get number of social publications (row[3]) */
      if (sscanf (row[3],"%d",&UsrFigures->NumSocPub) != 1)
	 UsrFigures->NumSocPub = -1;

      /* Get number of file views (row[4]) */
      if (sscanf (row[4],"%d",&UsrFigures->NumFileViews) != 1)
	 UsrFigures->NumFileViews = -1;

      /* Get number of forum posts (row[5]) */
      if (sscanf (row[5],"%d",&UsrFigures->NumForPst) != 1)
	 UsrFigures->NumForPst = -1;

      /* Get number of messages sent (row[6]) */
      if (sscanf (row[6],"%d",&UsrFigures->NumMsgSnt) != 1)
	 UsrFigures->NumMsgSnt = -1;
     }
   else
      /***** Return special user's figures indicating "not present" *****/
      Prf_ResetUsrFigures (UsrFigures);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Get ranking of a user according to the number of clicks **********/
/*****************************************************************************/

static unsigned Prf_DB_GetRankingFigure (long UsrCod,const char *FieldName)
  {
   /***** Select number of rows with figure
          greater than the figure of this user *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get ranking using a figure",
		  "SELECT COUNT(*)+1"
		   " FROM usr_figures"
		  " WHERE UsrCod<>%ld"	// Really not necessary here
		    " AND %s>(SELECT %s"
			      " FROM usr_figures"
			     " WHERE UsrCod=%ld)",
		  UsrCod,
		  FieldName,
		  FieldName,
		  UsrCod);
  }

/*****************************************************************************/
/********************* Get number of users with a figure *********************/
/*****************************************************************************/

static unsigned Prf_DB_GetNumUsrsWithFigure (const char *FieldName)
  {
   /***** Select number of rows with values already calculated *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users with a figure",
		  "SELECT COUNT(*)"
		   " FROM usr_figures"
		  " WHERE %s>=0",
		  FieldName);
  }

/*****************************************************************************/
/****** Get ranking of a user according to the number of clicks per day ******/
/*****************************************************************************/

static unsigned Prf_DB_GetRankingNumClicksPerDay (long UsrCod)
  {
   /***** Select number of rows with number of clicks per day
          greater than the clicks per day of this user *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get ranking using number of clicks per day",
		  "SELECT COUNT(*)+1"
		   " FROM (SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1) AS NumClicksPerDay"
			   " FROM usr_figures"
			  " WHERE UsrCod<>%ld"	// Necessary because the following comparison is not exact in floating point
			    " AND NumClicks>0"
			    " AND FirstClickTime>FROM_UNIXTIME(0)) AS TableNumClicksPerDay"
		  " WHERE NumClicksPerDay>"
		         "(SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
			   " FROM usr_figures"
			  " WHERE UsrCod=%ld"
			    " AND NumClicks>0"
			    " AND FirstClickTime>FROM_UNIXTIME(0))",
		  UsrCod,
		  UsrCod);
  }

/*****************************************************************************/
/************** Get number of users with number of clicks per day ************/
/*****************************************************************************/

static unsigned Prf_DB_GetNumUsrsWithNumClicksPerDay (void)
  {
   /***** Select number of rows with values already calculated *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users with number of clicks per day",
		  "SELECT COUNT(*)"
		   " FROM usr_figures"
		  " WHERE NumClicks>0"
		    " AND FirstClickTime>FROM_UNIXTIME(0)");
  }

/*****************************************************************************/
/************************* Show position in ranking **************************/
/*****************************************************************************/

static void Prf_ShowRanking (unsigned Rank,unsigned NumUsrs)
  {
   extern const char *The_ClassFormLinkOutBox[The_NUM_THEMES];
   extern const char *Txt_of_PART_OF_A_TOTAL;
   char *Title;

   /***** Rank in form to go to ranking *****/
   Frm_BeginForm (ActSeeUseGbl);
   Sco_PutParamScope ("ScopeSta",HieLvl_SYS);
   Par_PutHiddenParamUnsigned (NULL,"FigureType",(unsigned) Fig_USERS_RANKING);
      if (asprintf (&Title,"#%u %s %u",
		    Rank,Txt_of_PART_OF_A_TOTAL,NumUsrs) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_BUTTON_SUBMIT_Begin (Title,The_ClassFormLinkOutBox[Gbl.Prefs.Theme],NULL);
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
   struct UsrFigures UsrFigures;

   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

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

bool Prf_GetAndStoreAllUsrFigures (long UsrCod,struct UsrFigures *UsrFigures)
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
   if (UsrFigures->NumSocPub < 0)
     {
      /***** Get number of social publications and store as user's figure *****/
      Prf_GetNumSocialPubsAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
   if (UsrFigures->NumFileViews < 0)
     {
      /***** Get number of file views and store as user's figure *****/
      Prf_GetNumFileViewsAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
   if (UsrFigures->NumForPst < 0)
     {
      /***** Get number of forum posts and store as user's figure *****/
      Prf_GetNumForumPostsAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
    if (UsrFigures->NumMsgSnt < 0)
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
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get first click from log table *****/
      if (DB_QuerySELECT (&mysql_res,"can not get user's first click",
			  "SELECT UNIX_TIMESTAMP((SELECT MIN(ClickTime)"
					          " FROM log"
						 " WHERE UsrCod=%ld))",
			  UsrCod))
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
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures"
			   " SET FirstClickTime=FROM_UNIXTIME(%ld)"
			 " WHERE UsrCod=%ld",
		         (long) UsrFigures.FirstClickTimeUTC,
		         UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
  }

/*****************************************************************************/
/* Get number of clicks of a user from log table and store in user's figures */
/*****************************************************************************/

static void Prf_GetNumClicksAndStoreAsUsrFigure (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of clicks from database *****/
      UsrFigures.NumClicks = (int)
      DB_QueryCOUNT ("can not get number of clicks",
		     "SELECT COUNT(*)"
		      " FROM log"
		     " WHERE UsrCod=%ld",
		     UsrCod);

      /***** Update number of clicks in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures"
			   " SET NumClicks=%ld"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumClicks,
		         UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
  }

/*****************************************************************************/
/* Get number of social publications from a user and store in user's figures */
/*****************************************************************************/

static void Prf_GetNumSocialPubsAndStoreAsUsrFigure (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of forum posts from database *****/
      UsrFigures.NumSocPub = (int) Tml_DB_GetNumPubsUsr (UsrCod);

      /***** Update number of forum posts in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures"
			   " SET NumSocPub=%d"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumSocPub,
		         UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
  }

/*****************************************************************************/
/**** Get number of file views sent by a user and store in user's figures ****/
/*****************************************************************************/

static void Prf_GetNumFileViewsAndStoreAsUsrFigure (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of file views from database *****/
      UsrFigures.NumFileViews = (long) Brw_DB_GetNumFileViewsUsr (UsrCod);

      /***** Update number of file views in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures"
			   " SET NumFileViews=%d"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumFileViews,
		         UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
  }

/*****************************************************************************/
/**** Get number of forum posts sent by a user and store in user's figures ***/
/*****************************************************************************/

static void Prf_GetNumForumPostsAndStoreAsUsrFigure (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of forum posts from database *****/
      UsrFigures.NumForPst = For_DB_GetNumPostsUsr (UsrCod);

      /***** Update number of forum posts in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures"
			   " SET NumForPst=%d"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumForPst,
		         UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
  }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

static void Prf_GetNumMessagesSentAndStoreAsUsrFigure (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of messages sent from database *****/
      UsrFigures.NumMsgSnt = (int) Msg_DB_GetNumMsgsSentByUsr (UsrCod);

      /***** Update number of messages sent in user's figures *****/
      if (Prf_DB_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures"
			   " SET NumMsgSnt=%d"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumMsgSnt,
		         UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
  }

/*****************************************************************************/
/******************* Create user's figures for a new user ********************/
/*****************************************************************************/

void Prf_CreateNewUsrFigures (long UsrCod,bool CreatingMyOwnAccount)
  {
   struct UsrFigures UsrFigures;

   /***** Reset user's figures *****/
   Prf_ResetUsrFigures (&UsrFigures);
   UsrFigures.NumClicks    = 0;	// set number of clicks to 0
   UsrFigures.NumSocPub    = 0;	// set number of social publications to 0
   UsrFigures.NumFileViews = 0;	// set number of file views to 0
   UsrFigures.NumForPst    = 0;	// set number of forum posts to 0
   UsrFigures.NumMsgSnt    = 0;	// set number of messages sent to 0

   /***** Create user's figures *****/
   Prf_CreateUsrFigures (UsrCod,&UsrFigures,CreatingMyOwnAccount);
  }

/*****************************************************************************/
/********************** Reset values of user's figures ***********************/
/*****************************************************************************/

static void Prf_ResetUsrFigures (struct UsrFigures *UsrFigures)
  {
   UsrFigures->FirstClickTimeUTC = (time_t) 0;	// unknown first click time or user never logged
   UsrFigures->NumDays      = -1;	// not applicable
   UsrFigures->NumClicks    = -1;	// unknown number of clicks
   UsrFigures->NumSocPub    = -1;	// unknown number of social publications
   UsrFigures->NumFileViews = -1;	// unknown number of file views
   UsrFigures->NumForPst    = -1;	// unknown number of forum posts
   UsrFigures->NumMsgSnt    = -1;	// unknown number of messages sent
  }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

#define Prf_MAX_BYTES_SUBQUERY_FIRST_CLICK_TIME (64 - 1)

static void Prf_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures,
                                  bool CreatingMyOwnAccount)
  {
   char SubQueryFirstClickTime[Prf_MAX_BYTES_SUBQUERY_FIRST_CLICK_TIME + 1];

   if (CreatingMyOwnAccount)
      // This is the first click
      Str_Copy (SubQueryFirstClickTime,"NOW()",sizeof (SubQueryFirstClickTime) - 1);
   else
      sprintf (SubQueryFirstClickTime,"FROM_UNIXTIME(%ld)",
	       (long) UsrFigures->FirstClickTimeUTC);	//   0 ==> unknown first click time or user never logged

   /***** Create user's figures *****/
   DB_QueryINSERT ("can not create user's figures",
		   "INSERT INTO usr_figures"
		   " (UsrCod,FirstClickTime,"
		     "NumClicks,NumSocPub,NumFileViews,NumForPst,NumMsgSnt)"
		   " VALUES"
		   " (%ld,%s,"
		     "%d,%d,%d,%d,%d)",
		   UsrCod,
		   SubQueryFirstClickTime,
		   UsrFigures->NumClicks,	// -1 ==> unknown number of clicks
		   UsrFigures->NumSocPub,	// -1 ==> unknown number of social publications
		   UsrFigures->NumFileViews,	// -1 ==> unknown number of file views
		   UsrFigures->NumForPst,	// -1 ==> unknown number of forum posts
		   UsrFigures->NumMsgSnt);	// -1 ==> unknown number of messages sent
  }

/*****************************************************************************/
/**************************** Remove user's figures **************************/
/*****************************************************************************/

void Prf_DB_RemoveUsrFigures (long UsrCod)
  {
   DB_QueryDELETE ("can not delete user's figures",
		   "DELETE FROM usr_figures"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*** Check if it exists an entry for this user in table of user's figures ****/
/*****************************************************************************/

static bool Prf_DB_CheckIfUsrFiguresExists (long UsrCod)
  {
   return (DB_QueryCOUNT ("can not get user's first click",
			  "SELECT COUNT(*)"
			   " FROM usr_figures"
			  " WHERE UsrCod=%ld",
			  UsrCod) != 0);
  }

/*****************************************************************************/
/*************** Increment number of clicks made by a user *******************/
/*****************************************************************************/

void Prf_DB_IncrementNumClicksUsr (long UsrCod)
  {
   /***** Increment number of clicks *****/
   // If NumClicks < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's clicks",
		   "UPDATE IGNORE usr_figures"
		     " SET NumClicks=NumClicks+1"
		   " WHERE UsrCod=%ld"
		     " AND NumClicks>=0",
	           UsrCod);
  }

/*****************************************************************************/
/********* Increment number of social publications sent by a user ************/
/*****************************************************************************/

void Prf_DB_IncrementNumPubsUsr (long UsrCod)
  {
   /***** Increment number of social publications *****/
   // If NumSocPub < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's social publications",
		   "UPDATE IGNORE usr_figures"
		     " SET NumSocPub=NumSocPub+1"
		   " WHERE UsrCod=%ld"
		     " AND NumSocPub>=0",
	           UsrCod);
  }

/*****************************************************************************/
/************** Increment number of file views sent by a user ****************/
/*****************************************************************************/

void Prf_DB_IncrementNumFileViewsUsr (long UsrCod)
  {
   /***** Increment number of file views *****/
   // If NumFileViews < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's file views",
		   "UPDATE IGNORE usr_figures"
		     " SET NumFileViews=NumFileViews+1"
		   " WHERE UsrCod=%ld"
		     " AND NumFileViews>=0",
	           UsrCod);
  }

/*****************************************************************************/
/************* Increment number of forum posts sent by a user ****************/
/*****************************************************************************/

void Prf_DB_IncrementNumForPstUsr (long UsrCod)
  {
   /***** Increment number of forum posts *****/
   // If NumForPst < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's forum posts",
		   "UPDATE IGNORE usr_figures"
		     " SET NumForPst=NumForPst+1"
		   " WHERE UsrCod=%ld"
		     " AND NumForPst>=0",
	           UsrCod);
  }

/*****************************************************************************/
/*************** Increment number of messages sent by a user *****************/
/*****************************************************************************/

void Prf_DB_IncrementNumMsgSntUsr (long UsrCod)
  {
   /***** Increment number of messages sent *****/
   // If NumMsgSnt < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's messages sent",
		   "UPDATE IGNORE usr_figures"
		     " SET NumMsgSnt=NumMsgSnt+1"
		   " WHERE UsrCod=%ld"
		     " AND NumMsgSnt>=0",
	           UsrCod);
  }

/*****************************************************************************/
/******** Get and show ranking of users attending to number of clicks ********/
/*****************************************************************************/

void Prf_GetAndShowRankingClicks (void)
  {
   Prf_GetAndShowRankingFigure ("NumClicks");
  }

void Prf_GetAndShowRankingSocPub (void)
  {
   Prf_GetAndShowRankingFigure ("NumSocPub");
  }

void Prf_GetAndShowRankingFileViews (void)
  {
   Prf_GetAndShowRankingFigure ("NumFileViews");
  }

void Prf_GetAndShowRankingForPst (void)
  {
   Prf_GetAndShowRankingFigure ("NumForPst");
  }

void Prf_GetAndShowRankingMsgSnt (void)
  {
   Prf_GetAndShowRankingFigure ("NumMsgSnt");
  }

static void Prf_GetAndShowRankingFigure (const char *FieldName)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs = 0;	// Initialized to avoid warning

   /***** Get ranking from database *****/
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 NumUsrs = (unsigned)
	 DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT UsrCod,"		// row[0]
			        "%s"			// row[1]
			  " FROM usr_figures"
			 " WHERE %s>0"
			   " AND UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY %s DESC,"
			           "UsrCod"
			 " LIMIT 100",
			 FieldName,
			 FieldName,
			 FieldName);
         break;
      case HieLvl_CTY:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Cty.CtyCod,
			 FieldName,
			 FieldName);
         break;
      case HieLvl_INS:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Ins.InsCod,
			 FieldName,
			 FieldName);
         break;
      case HieLvl_CTR:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Ctr.CtrCod,
			 FieldName,FieldName);
         break;
      case HieLvl_DEG:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Deg.DegCod,
			 FieldName,FieldName);
         break;
      case HieLvl_CRS:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM crs_users,"
			        "usr_figures"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Crs.CrsCod,
			 FieldName,FieldName);
         break;
      default:
         Err_WrongScopeExit ();
         break;
     }

   Prf_ShowRankingFigure (&mysql_res,NumUsrs);
  }

void Prf_ShowRankingFigure (MYSQL_RES **mysql_res,unsigned NumUsrs)
  {
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned Rank;
   struct UsrData UsrDat;
   bool ItsMe;
   long FigureHigh = LONG_MAX;
   long Figure;

   if (NumUsrs)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      HTM_TABLE_Begin (NULL);

	 for (NumUsr = 1, Rank = 1, Gbl.RowEvenOdd = 0;
	      NumUsr <= NumUsrs;
	      NumUsr++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
	   {
	    /***** Get user and number of clicks *****/
	    row = mysql_fetch_row (*mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	    Usr_GetAllUsrDataFromUsrCod (&UsrDat,
					 Usr_DONT_GET_PREFS,
					 Usr_DONT_GET_ROLE_IN_CURRENT_CRS);
	    ItsMe = (UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);

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
	       Prf_ShowUsrInRanking (&UsrDat,Rank,ItsMe);
	       HTM_TD_Begin ("class=\"RM %s COLOR%u\"",
			     ItsMe ? "DAT_SMALL_N" :
				     "DAT_SMALL",
			     Gbl.RowEvenOdd);
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs = 0;	// Initialized to avoid warning
   unsigned NumUsr;
   unsigned Rank;
   struct UsrData UsrDat;
   bool ItsMe;
   double NumClicksPerDayHigh = (double) LONG_MAX;
   double NumClicksPerDay;

   /***** Get ranking from database *****/
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 NumUsrs = (unsigned)
	 DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT UsrCod,"						// row[0]
			        "NumClicks/(DATEDIFF(NOW(),"
			                   "FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM usr_figures"
			 " WHERE NumClicks>0"
			   " AND FirstClickTime>FROM_UNIXTIME(0)"
			   " AND UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
			           "UsrCod"
			 " LIMIT 100");
         break;
      case HieLvl_CTY:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Cty.CtyCod);
         break;
      case HieLvl_INS:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Ins.InsCod);
         break;
      case HieLvl_CTR:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case HieLvl_DEG:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Deg.DegCod);
         break;
      case HieLvl_CRS:
         NumUsrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM crs_users,"
			        "usr_figures"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
         Err_WrongScopeExit ();
         break;
     }

   if (NumUsrs)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      HTM_TABLE_Begin (NULL);

	 for (NumUsr = 1, Rank = 1, Gbl.RowEvenOdd = 0;
	      NumUsr <= NumUsrs;
	      NumUsr++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
	   {
	    /***** Get user and number of clicks *****/
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	    Usr_GetAllUsrDataFromUsrCod (&UsrDat,
					 Usr_DONT_GET_PREFS,
					 Usr_DONT_GET_ROLE_IN_CURRENT_CRS);
	    ItsMe = (UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);

	    /* Get average number of clicks per day (row[1]) */
	    NumClicksPerDay = Str_GetDoubleFromStr (row[1]);
	    if (NumClicksPerDay < NumClicksPerDayHigh)
	      {
	       Rank = NumUsr;
	       NumClicksPerDayHigh = NumClicksPerDay;
	      }

	    /***** Show row *****/
	    HTM_TR_Begin (NULL);
	       Prf_ShowUsrInRanking (&UsrDat,Rank,ItsMe);
	       HTM_TD_Begin ("class=\"RM %s COLOR%u\"",
			     ItsMe ? "DAT_SMALL_N" :
				     "DAT_SMALL",
			     Gbl.RowEvenOdd);
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

static void Prf_ShowUsrInRanking (struct UsrData *UsrDat,unsigned Rank,bool ItsMe)
  {
   extern const char *Txt_Another_user_s_profile;
   bool Visible = Pri_ShowingIsAllowed (UsrDat->BaPrfVisibility,UsrDat);

   HTM_TD_Begin ("class=\"RM %s COLOR%u\"",
		 ItsMe ? "DAT_SMALL_N" :
		         "DAT_SMALL",
                 Gbl.RowEvenOdd);
      HTM_TxtF ("#%u",Rank);
   HTM_TD_End ();

   /***** Check if I can see the public profile *****/
   HTM_TD_Begin ("class=\"RANK_PHOTO COLOR%u\"",Gbl.RowEvenOdd);
      if (Visible)
	 /***** User's photo *****/
	 Pho_ShowUsrPhotoIfAllowed (UsrDat,"PHOTO30x40",Pho_ZOOM,false);
   HTM_TD_End ();

   /***** Put form to go to public profile *****/
   HTM_TD_Begin ("class=\"COLOR%u\"",Gbl.RowEvenOdd);
      if (Visible)
	{
	 Frm_BeginForm (ActSeeOthPubPrf);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	    HTM_BUTTON_SUBMIT_Begin (Txt_Another_user_s_profile,
				     ItsMe ? "BT_LINK RANK_USR DAT_SMALL_N" :
					     "BT_LINK RANK_USR DAT_SMALL",
				     NULL);	// Limited width
	       Usr_WriteFirstNameBRSurnames (UsrDat);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
	}
   HTM_TD_End ();
  }
