// swad_profile.c: user's public profile

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_network.h"
#include "swad_nickname.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_preference.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_role.h"
#include "swad_role_type.h"
#include "swad_social.h"
#include "swad_text.h"
#include "swad_theme.h"
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

static void Prf_PutLinkToUpdateAction (Act_Action_t Action,const char *EncryptedUsrCod);

static unsigned long Prf_GetRankingFigure (long UsrCod,const char *FieldName);
static unsigned long Prf_GetNumUsrsWithFigure (const char *FieldName);
static unsigned long Prf_GetRankingNumClicksPerDay (long UsrCod);
static unsigned long Prf_GetNumUsrsWithNumClicksPerDay (void);
static void Prf_ShowRanking (unsigned long Rank,unsigned long NumUsrs);

static void Prf_GetFirstClickFromLogAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumClicksAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumFileViewsAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumForPstAndStoreAsUsrFigure (long UsrCod);
static void Prf_GetNumMsgSntAndStoreAsUsrFigure (long UsrCod);

static void Prf_ResetUsrFigures (struct UsrFigures *UsrFigures);
static void Prf_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures,
                                  bool CreatingMyOwnAccount);
static bool Prf_CheckIfUsrFiguresExists (long UsrCod);

static void Prf_GetAndShowRankingFigure (const char *FieldName);

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
                               const char *NicknameWithoutArroba)
  {
   extern const char *Txt_STR_LANG_ID[1 + Txt_NUM_LANGUAGES];

   /***** Build URL using nickname *****/
   snprintf (URL,Cns_MAX_BYTES_WWW + 1,
	     "%s/%s?usr=@%s",
	     Cfg_URL_SWAD_CGI,
	     Txt_STR_LANG_ID[Gbl.Prefs.Language],
	     NicknameWithoutArroba);

   return URL;
  }

/*****************************************************************************/
/******************** Put link to view my public profile *********************/
/*****************************************************************************/

void Prf_PutLinkMyPublicProfile (void)
  {
   extern const char *Txt_My_public_profile;

   Lay_PutContextualLink (ActSeeOthPubPrf,NULL,Usr_PutParamMyUsrCodEncrypted,
                          "usr64x64.png",
                          Txt_My_public_profile,Txt_My_public_profile,
		          NULL);
  }

/*****************************************************************************/
/***************** Put link to request another user's profile ****************/
/*****************************************************************************/

void Prf_PutLinkRequestAnotherUserProfile (void)
  {
   extern const char *Txt_Another_user_s_profile;

   Lay_PutContextualLink (ActReqOthPubPrf,NULL,NULL,
                          "usr64x64.png",
                          Txt_Another_user_s_profile,Txt_Another_user_s_profile,
		          NULL);
  }

/*****************************************************************************/
/************************** Request a user's profile *************************/
/*****************************************************************************/

void Prf_RequestUserProfile (void)
  {
   extern const char *Hlp_SOCIAL_Profiles_view_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Nickname;
   extern const char *Txt_Continue;

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Put link to show my public profile and users to follow *****/
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Prf_PutLinkMyPublicProfile ();
      Fol_PutLinkWhoToFollow ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Start form *****/
   Act_StartForm (ActSeeOthPubPrf);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Another_user_s_profile,NULL,
                 Hlp_SOCIAL_Profiles_view_public_profile,Box_NOT_CLOSABLE);

   /***** Form to request user's @nickname *****/
   /* By default, the nickname is filled with my nickname
      If no user logged ==> the nickname is empty */
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
                      "%s:&nbsp;"
                      "<input type=\"text\" name=\"usr\""
                      " size=\"18\" maxlength=\"%u\" value=\"@%s\" />"
                      "</label>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Nickname,
            Nck_MAX_BYTES_NICKNAME_FROM_FORM,
            Gbl.Usrs.Me.UsrDat.Nickname);

   /***** Send button and end box *****/
   Box_EndBoxWithButton (Btn_CONFIRM_BUTTON,Txt_Continue);

   /***** End form *****/
   Act_EndForm ();
  }

/*****************************************************************************/
/******************** Get user and show a user's profile *********************/
/*****************************************************************************/
// Gbl.Usrs.Other.UsrDat.UsrCod may be already taken. If not ==> try to get it

void Prf_GetUsrDatAndShowUserProfile (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool ItsMe;
   bool ProfileShown = false;

   /***** Get user's data *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0)
      Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Show profile and timeline *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
      /* Show profile */
      if (Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat))
	{
	 ProfileShown = true;

	 if (Gbl.Usrs.Me.Logged)	// Timeline visible only by logged users
	   {
	    /* Show timeline */
	    Lay_StartSection (Soc_TIMELINE_SECTION_ID);
   	    Soc_ShowTimelineUsr ();
	    Lay_EndSection ();
	   }
	}

   /***** If profile could not be shown... *****/
   if (!ProfileShown)
     {
      /* Show error message */
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

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
   if (Pri_ShowingIsAllowed (UsrDat->ProfileVisibility,UsrDat))
     {
      /***** Contextual links *****/
      if (Gbl.Usrs.Me.Logged)
	{
	 fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

	 if (ItsMe)
	    /* Put link to show another user's profile */
	    Prf_PutLinkRequestAnotherUserProfile ();
	 else	// Not me
	    /* Put link to show my public profile */
	    Prf_PutLinkMyPublicProfile ();

	 /* Put link to show users to follow */
	 Fol_PutLinkWhoToFollow ();

	 fprintf (Gbl.F.Out,"</div>");
	}

      /***** Shared record card *****/
      if (!ItsMe &&				// If not me...
	  Gbl.CurrentCrs.Crs.CrsCod > 0)	// ...and a course is selected
	{
	 /* Get user's role in current course */
	 UsrDat->Roles.InCurrentCrs.Role = Rol_GetRoleUsrInCrs (UsrDat->UsrCod,
	                                                        Gbl.CurrentCrs.Crs.CrsCod);
	 UsrDat->Roles.InCurrentCrs.Valid = true;

	 /* Get if user has accepted enrolment in current course */
	 UsrDat->Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (UsrDat);
	}
      Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_PUBLIC,UsrDat,NULL);

      /***** Show details of user's profile *****/
      Prf_ShowDetailsUserProfile (UsrDat);

      /***** Count following and followers *****/
      NumFollowing = Fol_GetNumFollowing (UsrDat->UsrCod);
      NumFollowers = Fol_GetNumFollowers (UsrDat->UsrCod);
      UsrFollowsMe = false;
      if (NumFollowing)
         UsrFollowsMe = Fol_CheckUsrIsFollowerOf (UsrDat->UsrCod,
                                                  Gbl.Usrs.Me.UsrDat.UsrCod);
      IFollowUsr   = false;
      if (NumFollowers)
         IFollowUsr   = Fol_CheckUsrIsFollowerOf (Gbl.Usrs.Me.UsrDat.UsrCod,
                                                  UsrDat->UsrCod);

      /***** Show following and followers *****/
      Fol_ShowFollowingAndFollowers (UsrDat,
                                     NumFollowing,NumFollowers,
                                     UsrFollowsMe,IFollowUsr);

      return true;
     }
   return false;
  }

/*****************************************************************************/
/******************** Change my public profile visibility ********************/
/*****************************************************************************/

void Prf_ChangeProfileVisibility (void)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.ProfileVisibility = Pri_GetParamVisibility ("VisPrf");

   /***** Store public/private photo in database *****/
   DB_QueryUPDATE ("can not update your preference"
		   " about public profile visibility",
		   "UPDATE usr_data SET ProfileVisibility='%s'"
		   " WHERE UsrCod=%ld",
                   Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.ProfileVisibility],
                   Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show form again *****/
   Pre_EditPrefs ();
  }

/*****************************************************************************/
/********************** Show details of user's profile ***********************/
/*****************************************************************************/

void Prf_ShowDetailsUserProfile (const struct UsrData *UsrDat)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_courses_ABBREVIATION;
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   extern const char *Txt_Files_uploaded;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_public_FILES;
   extern const char *Txt_TIME_Since;
   extern const char *Txt_day;
   extern const char *Txt_days;
   extern const char *Txt_Today;
   extern const char *Txt_Clicks;
   extern const char *Txt_clicks;
   extern const char *Txt_Downloads;
   extern const char *Txt_download;
   extern const char *Txt_downloads;
   extern const char *Txt_Forums;
   extern const char *Txt_post;
   extern const char *Txt_posts;
   extern const char *Txt_Messages;
   extern const char *Txt_message;
   extern const char *Txt_messages;
   bool UsrIsBannedFromRanking;
   struct UsrFigures UsrFigures;
   unsigned NumCrssUsrIsTeacher;
   unsigned NumCrssUsrIsStudent;
   unsigned NumFiles;
   unsigned NumPublicFiles;
   char IdFirstClickTime[Act_MAX_BYTES_ID + 1];

   /***** Start left list *****/
   fprintf (Gbl.F.Out,"<div class=\"PRF_FIG_LEFT_CONTAINER\">"
	              "<ul class=\"PRF_FIG_UL DAT_NOBR_N\">");

   /***** Number of courses in which the user is teacher *****/
   NumCrssUsrIsTeacher = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Rol_TCH);
   fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	              " style=\"background-image:url('%s/tch64x64.gif');\">"
		      "%u&nbsp;%s",
	    Txt_ROLES_SINGUL_Abc[Rol_TCH][UsrDat->Sex],
	    Gbl.Prefs.IconsURL,
	    NumCrssUsrIsTeacher,
	    Txt_courses_ABBREVIATION);
   if (NumCrssUsrIsTeacher)
      fprintf (Gbl.F.Out,"&nbsp;(%u&nbsp;%s/%u&nbsp;%s)",
	       Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_TCH,Rol_TCH),
	       Txt_teachers_ABBREVIATION,
	       Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_TCH,Rol_STD),
	       Txt_students_ABBREVIATION);
   fprintf (Gbl.F.Out,"</li>");

   /***** Number of courses in which the user is student *****/
   NumCrssUsrIsStudent = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Rol_STD);
   fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	              " style=\"background-image:url('%s/std64x64.gif');\">"
		      "%u&nbsp;%s",
	    Txt_ROLES_SINGUL_Abc[Rol_STD][UsrDat->Sex],
	    Gbl.Prefs.IconsURL,
	    NumCrssUsrIsStudent,
	    Txt_courses_ABBREVIATION);
   if (NumCrssUsrIsStudent)
      fprintf (Gbl.F.Out,"&nbsp;(%u&nbsp;%s/%u&nbsp;%s)",
	       Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_STD,Rol_TCH),
	       Txt_teachers_ABBREVIATION,
	       Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_STD,Rol_STD),
	       Txt_students_ABBREVIATION);
   fprintf (Gbl.F.Out,"</li>");

   /***** Number of files currently published *****/
   if ((NumFiles = Brw_GetNumFilesUsr (UsrDat->UsrCod)))
      NumPublicFiles = Brw_GetNumPublicFilesUsr (UsrDat->UsrCod);
   else
      NumPublicFiles = 0;
   fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	              " style=\"background-image:url('%s/file64x64.gif');\">"
		      "%u&nbsp;%s&nbsp;(%u&nbsp;%s)"
		      "</li>",
	    Txt_Files_uploaded,
            Gbl.Prefs.IconsURL,
	    NumFiles,
	    (NumFiles == 1) ? Txt_file :
		              Txt_files,
	    NumPublicFiles,Txt_public_FILES);

   /***** Get figures *****/
   Prf_GetUsrFigures (UsrDat->UsrCod,&UsrFigures);

   /* Time since first click */
   fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	              " style=\"background-image:url('%s/clock64x64.gif');\">",
	    Txt_TIME_Since,
            Gbl.Prefs.IconsURL);
   if (UsrFigures.FirstClickTimeUTC)
     {
      /* Create unique id for new comment */
      Act_SetUniqueId (IdFirstClickTime);

      fprintf (Gbl.F.Out,"<span id=\"%s\"></span>",IdFirstClickTime);
      if (UsrFigures.NumDays > 0)
	 fprintf (Gbl.F.Out,"&nbsp;(%d&nbsp;%s)",
		  UsrFigures.NumDays,
		  (UsrFigures.NumDays == 1) ? Txt_day :
					      Txt_days);
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
			 "writeLocalDateHMSFromUTC('%s',%ld,"
			 "%u,',&nbsp;','%s',true,false,0x6);"
			 "</script>",
	       IdFirstClickTime,(long) UsrFigures.FirstClickTimeUTC,
	       (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
     }
   else	// First click time is unknown or user never logged
      /***** Button to fetch and store first click time *****/
      Prf_PutLinkToUpdateAction (ActCal1stClkTim,UsrDat->EncryptedUsrCod);
   fprintf (Gbl.F.Out,"</li>");

   /***** End left list *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");

   /***** Start right list *****/
   fprintf (Gbl.F.Out,"<div class=\"PRF_FIG_RIGHT_CONTAINER\">"
	              "<ul class=\"PRF_FIG_UL DAT_NOBR_N\">");

   UsrIsBannedFromRanking = Usr_CheckIfUsrBanned (UsrDat->UsrCod);
   if (!UsrIsBannedFromRanking)
     {
      /* Number of clicks */
      fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	                 " style=\"background-image:url('%s/click64x64.gif');\">",
	       Txt_Clicks,
	       Gbl.Prefs.IconsURL);

      if (UsrFigures.NumClicks >= 0)
	{
	 fprintf (Gbl.F.Out,"%ld&nbsp;%s&nbsp;",
		  UsrFigures.NumClicks,Txt_clicks);
	 Prf_ShowRanking (Prf_GetRankingFigure (UsrDat->UsrCod,"NumClicks"),
			  Prf_GetNumUsrsWithFigure ("NumClicks"));
	 if (UsrFigures.NumDays > 0)
	   {
	    fprintf (Gbl.F.Out,"&nbsp;(");
	    Str_WriteFloatNum (Gbl.F.Out,
	                       (float) UsrFigures.NumClicks /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"/%s&nbsp;",Txt_day);
	    Prf_ShowRanking (Prf_GetRankingNumClicksPerDay (UsrDat->UsrCod),
			     Prf_GetNumUsrsWithNumClicksPerDay ());
	    fprintf (Gbl.F.Out,")");
	   }
	}
      else	// Number of clicks is unknown
	 /***** Button to fetch and store number of clicks *****/
         Prf_PutLinkToUpdateAction (ActCalNumClk,UsrDat->EncryptedUsrCod);
      fprintf (Gbl.F.Out,"</li>");

      /***** Number of file views *****/
      fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	                 " style=\"background-image:url('%s/download64x64.png');\">",
	       Txt_Downloads,
	       Gbl.Prefs.IconsURL);
      if (UsrFigures.NumFileViews >= 0)
	{
	 fprintf (Gbl.F.Out,"%ld&nbsp;%s&nbsp;",
		  UsrFigures.NumFileViews,
		  (UsrFigures.NumFileViews == 1) ? Txt_download :
						   Txt_downloads);
	 Prf_ShowRanking (Prf_GetRankingFigure (UsrDat->UsrCod,"NumFileViews"),
			  Prf_GetNumUsrsWithFigure ("NumFileViews"));
	 if (UsrFigures.NumDays > 0)
	   {
	    fprintf (Gbl.F.Out,"&nbsp;(");
	    Str_WriteFloatNum (Gbl.F.Out,
	                       (float) UsrFigures.NumFileViews /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"/%s)",Txt_day);
	   }
	}
      else	// Number of file views is unknown
	 /***** Button to fetch and store number of file views *****/
         Prf_PutLinkToUpdateAction (ActCalNumFilVie,UsrDat->EncryptedUsrCod);
      fprintf (Gbl.F.Out,"</li>");

      /***** Number of posts in forums *****/
      fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	                 " style=\"background-image:url('%s/forum64x64.gif');\">",
	       Txt_Forums,
	       Gbl.Prefs.IconsURL);
      if (UsrFigures.NumForPst >= 0)
	{
	 fprintf (Gbl.F.Out,"%ld&nbsp;%s&nbsp;",
		  UsrFigures.NumForPst,
		  (UsrFigures.NumForPst == 1) ? Txt_post :
						Txt_posts);
	 Prf_ShowRanking (Prf_GetRankingFigure (UsrDat->UsrCod,"NumForPst"),
			  Prf_GetNumUsrsWithFigure ("NumForPst"));
	 if (UsrFigures.NumDays > 0)
	   {
	    fprintf (Gbl.F.Out,"&nbsp;(");
	    Str_WriteFloatNum (Gbl.F.Out,
	                       (float) UsrFigures.NumForPst /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"/%s)",Txt_day);
	   }
	}
      else	// Number of forum posts is unknown
	 /***** Button to fetch and store number of forum posts *****/
         Prf_PutLinkToUpdateAction (ActCalNumForPst,UsrDat->EncryptedUsrCod);

      fprintf (Gbl.F.Out,"</li>");

      /***** Number of messages sent *****/
      fprintf (Gbl.F.Out,"<li title=\"%s\" class=\"PRF_FIG_LI\""
	                 " style=\"background-image:url('%s/msg64x64.gif');\">",
	       Txt_Messages,
	       Gbl.Prefs.IconsURL);
      if (UsrFigures.NumMsgSnt >= 0)
	{
	 fprintf (Gbl.F.Out,"%ld&nbsp;%s&nbsp;",
		  UsrFigures.NumMsgSnt,
		  (UsrFigures.NumMsgSnt == 1) ? Txt_message :
						Txt_messages);
	 Prf_ShowRanking (Prf_GetRankingFigure (UsrDat->UsrCod,"NumMsgSnt"),
			  Prf_GetNumUsrsWithFigure ("NumMsgSnt"));
	 if (UsrFigures.NumDays > 0)
	   {
	    fprintf (Gbl.F.Out,"&nbsp;(");
	    Str_WriteFloatNum (Gbl.F.Out,
	                       (float) UsrFigures.NumMsgSnt /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"/%s)",Txt_day);
	   }
	}
      else	// Number of messages sent is unknown
	 /***** Button to fetch and store number of messages sent *****/
         Prf_PutLinkToUpdateAction (ActCalNumMsgSnt,UsrDat->EncryptedUsrCod);
      fprintf (Gbl.F.Out,"</li>");
     }

   /***** End right list *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
  }

/*****************************************************************************/
/******** Put contextual link with animated icon to update an action *********/
/*****************************************************************************/

static void Prf_PutLinkToUpdateAction (Act_Action_t Action,const char *EncryptedUsrCod)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Calculate;

   Act_StartForm (Action);
   Usr_PutParamUsrCodEncrypted (EncryptedUsrCod);
   Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassForm[Gbl.Prefs.Theme],
                               NULL);
   Ico_PutCalculateIconWithText (Txt_Calculate,Txt_Calculate);
   Act_EndForm ();
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
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get user's figures",
					"SELECT UNIX_TIMESTAMP(FirstClickTime),"
				        "DATEDIFF(NOW(),FirstClickTime)+1,"
				        "NumClicks,NumFileViews,NumForPst,NumMsgSnt"
				        " FROM usr_figures WHERE UsrCod=%ld",
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
      if (sscanf (row[2],"%ld",&UsrFigures->NumClicks) != 1)
	 UsrFigures->NumClicks = -1L;

      /* Get number of file views (row[3]) */
      if (sscanf (row[3],"%ld",&UsrFigures->NumFileViews) != 1)
	 UsrFigures->NumFileViews = -1L;

      /* Get number of forum posts (row[4]) */
      if (sscanf (row[4],"%ld",&UsrFigures->NumForPst) != 1)
	 UsrFigures->NumForPst = -1L;

      /* Get number of messages sent (row[5]) */
      if (sscanf (row[5],"%ld",&UsrFigures->NumMsgSnt) != 1)
	 UsrFigures->NumMsgSnt = -1L;
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

static unsigned long Prf_GetRankingFigure (long UsrCod,const char *FieldName)
  {
   /***** Select number of rows with figure
          greater than the figure of this user *****/
   return DB_QueryCOUNT ("can not get ranking using a figure",
			 "SELECT COUNT(*)+1 FROM usr_figures"
			 " WHERE UsrCod<>%ld"	// Really not necessary here
			 " AND %s>"
			 "(SELECT %s FROM usr_figures WHERE UsrCod=%ld)",
			 UsrCod,FieldName,FieldName,UsrCod);
  }

/*****************************************************************************/
/********************* Get number of users with a figure *********************/
/*****************************************************************************/

static unsigned long Prf_GetNumUsrsWithFigure (const char *FieldName)
  {
   /***** Select number of rows with values already calculated *****/
   return DB_QueryCOUNT ("can not get number of users with a figure",
			 "SELECT COUNT(*) FROM usr_figures WHERE %s>=0",
			 FieldName);
  }

/*****************************************************************************/
/****** Get ranking of a user according to the number of clicks per day ******/
/*****************************************************************************/

static unsigned long Prf_GetRankingNumClicksPerDay (long UsrCod)
  {
   /***** Select number of rows with number of clicks per day
          greater than the clicks per day of this user *****/
   return DB_QueryCOUNT ("can not get ranking using number of clicks per day",
			 "SELECT COUNT(*)+1 FROM"
			 " (SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
			 " AS NumClicksPerDay"
			 " FROM usr_figures"
			 " WHERE UsrCod<>%ld"	// Necessary because the following comparison is not exact in floating point
			 " AND NumClicks>0"
			 " AND FirstClickTime>FROM_UNIXTIME(0))"
			 " AS TableNumClicksPerDay"
			 " WHERE NumClicksPerDay>"
			 "(SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
			 " FROM usr_figures"
			 " WHERE UsrCod=%ld"
			 " AND NumClicks>0"
			 " AND FirstClickTime>FROM_UNIXTIME(0))",
			 UsrCod,UsrCod);
  }

/*****************************************************************************/
/************** Get number of users with number of clicks per day ************/
/*****************************************************************************/

static unsigned long Prf_GetNumUsrsWithNumClicksPerDay (void)
  {
   /***** Select number of rows with values already calculated *****/
   return DB_QueryCOUNT ("can not get number of users"
			 " with number of clicks per day",
			 "SELECT COUNT(*) FROM usr_figures"
			 " WHERE NumClicks>0"
			 " AND FirstClickTime>FROM_UNIXTIME(0)");
  }

/*****************************************************************************/
/************************* Show position in ranking **************************/
/*****************************************************************************/

static void Prf_ShowRanking (unsigned long Rank,unsigned long NumUsrs)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_of_PART_OF_A_TOTAL;

   /***** Part of a total and end container *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     "#%lu %s %lu",
	     Rank,Txt_of_PART_OF_A_TOTAL,NumUsrs);

   /***** Rank in form to go to ranking *****/
   Act_StartForm (ActSeeUseGbl);
   Sco_PutParamScope ("ScopeSta",Sco_SCOPE_SYS);
   Par_PutHiddenParamUnsigned ("FigureType",(unsigned) Sta_USERS_RANKING);
   Act_LinkFormSubmit (Gbl.Title,The_ClassForm[Gbl.Prefs.Theme],NULL);
   fprintf (Gbl.F.Out,"#%lu</a>",Rank);
   Act_EndForm ();
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
   if (UsrFigures->NumFileViews < 0)
     {
      /***** Get number of file views and store as user's figure *****/
      Prf_GetNumFileViewsAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
   if (UsrFigures->NumForPst < 0)
     {
      /***** Get number of forum posts and store as user's figure *****/
      Prf_GetNumForPstAndStoreAsUsrFigure (UsrCod);
      UsrFiguresModified = true;
     }
    if (UsrFigures->NumMsgSnt < 0)
     {
      /***** Get number of messages sent and store as user's figure *****/
      Prf_GetNumMsgSntAndStoreAsUsrFigure (UsrCod);
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
			  "SELECT UNIX_TIMESTAMP("
			  "(SELECT MIN(ClickTime) FROM log_full"
			  " WHERE UsrCod=%ld)"
			  ")",
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
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures"
			 " SET FirstClickTime=FROM_UNIXTIME(%ld)"
			 " WHERE UsrCod=%ld",
		         (long) UsrFigures.FirstClickTimeUTC,UsrCod);
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
      UsrFigures.NumClicks =
      (long) DB_QueryCOUNT ("can not get number of clicks",
			    "SELECT COUNT(*) FROM log_full WHERE UsrCod=%ld",
			    UsrCod);

      /***** Update number of clicks in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures SET NumClicks=%ld"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumClicks,UsrCod);
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
      UsrFigures.NumFileViews = Brw_GetNumFileViewsUsr (UsrCod);

      /***** Update number of file views in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures SET NumFileViews=%ld"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumFileViews,UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
   }

/*****************************************************************************/
/**** Get number of forum posts sent by a user and store in user's figures ***/
/*****************************************************************************/

static void Prf_GetNumForPstAndStoreAsUsrFigure (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of forum posts from database *****/
      UsrFigures.NumForPst = For_GetNumPostsUsr (UsrCod);

      /***** Update number of forum posts in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures SET NumForPst=%ld"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumForPst,UsrCod);
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures,false);
     }
   }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

static void Prf_GetNumMsgSntAndStoreAsUsrFigure (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of messages sent from database *****/
      UsrFigures.NumMsgSnt = Msg_GetNumMsgsSentByUsr (UsrCod);

      /***** Update number of messages sent in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	 DB_QueryUPDATE ("can not update user's figures",
			 "UPDATE usr_figures SET NumMsgSnt=%ld"
			 " WHERE UsrCod=%ld",
		         UsrFigures.NumMsgSnt,UsrCod);
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
   UsrFigures->NumClicks    = -1L;	// unknown number of clicks
   UsrFigures->NumFileViews = -1L;	// unknown number of file views
   UsrFigures->NumForPst    = -1L;	// unknown number of forum posts
   UsrFigures->NumMsgSnt    = -1L;	// unknown number of messages sent
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
      Str_Copy (SubQueryFirstClickTime,"NOW()",
                Prf_MAX_BYTES_SUBQUERY_FIRST_CLICK_TIME);
   else
      sprintf (SubQueryFirstClickTime,"FROM_UNIXTIME(%ld)",
	       (long) UsrFigures->FirstClickTimeUTC);	//   0 ==> unknown first click time or user never logged

   /***** Create user's figures *****/
   DB_QueryINSERT ("can not create user's figures",
		   "INSERT INTO usr_figures"
		   " (UsrCod,FirstClickTime,NumClicks,NumFileViews,NumForPst,NumMsgSnt)"
		   " VALUES"
		   " (%ld,%s,%ld,%ld,%ld,%ld)",
		   UsrCod,
		   SubQueryFirstClickTime,
		   UsrFigures->NumClicks,	// -1L ==> unknown number of clicks
		   UsrFigures->NumFileViews,	// -1L ==> unknown number of file views
		   UsrFigures->NumForPst,	// -1L ==> unknown number of forum posts
		   UsrFigures->NumMsgSnt);	// -1L ==> unknown number of messages sent
  }

/*****************************************************************************/
/**************************** Remove user's figures **************************/
/*****************************************************************************/

void Prf_RemoveUsrFigures (long UsrCod)
  {
   /***** Remove user's figures *****/
   DB_QueryDELETE ("can not delete user's figures",
		   "DELETE FROM usr_figures WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*** Check if it exists an entry for this user in table of user's figures ****/
/*****************************************************************************/

static bool Prf_CheckIfUsrFiguresExists (long UsrCod)
  {
   return (DB_QueryCOUNT ("can not get user's first click",
			  "SELECT COUNT(*) FROM usr_figures WHERE UsrCod=%ld",
			  UsrCod) != 0);
  }

/*****************************************************************************/
/*************** Increment number of clicks made by a user *******************/
/*****************************************************************************/

void Prf_IncrementNumClicksUsr (long UsrCod)
  {
   /***** Increment number of clicks *****/
   // If NumClicks < 0 ==> not yet calculated, so do nothing
   DB_QueryINSERT ("can not increment user's clicks",
		   "UPDATE IGNORE usr_figures SET NumClicks=NumClicks+1"
		   " WHERE UsrCod=%ld AND NumClicks>=0",
	           UsrCod);
  }

/*****************************************************************************/
/************** Increment number of file views sent by a user ****************/
/*****************************************************************************/

void Prf_IncrementNumFileViewsUsr (long UsrCod)
  {
   /***** Increment number of file views *****/
   // If NumFileViews < 0 ==> not yet calculated, so do nothing
   DB_QueryINSERT ("can not increment user's file views",
		   "UPDATE IGNORE usr_figures SET NumFileViews=NumFileViews+1"
		   " WHERE UsrCod=%ld AND NumFileViews>=0",
	           UsrCod);
  }

/*****************************************************************************/
/************* Increment number of forum posts sent by a user ****************/
/*****************************************************************************/

void Prf_IncrementNumForPstUsr (long UsrCod)
  {
   /***** Increment number of forum posts *****/
   // If NumForPst < 0 ==> not yet calculated, so do nothing
   DB_QueryINSERT ("can not increment user's forum posts",
		   "UPDATE IGNORE usr_figures SET NumForPst=NumForPst+1"
		   " WHERE UsrCod=%ld AND NumForPst>=0",
	           UsrCod);
  }

/*****************************************************************************/
/*************** Increment number of messages sent by a user *****************/
/*****************************************************************************/

void Prf_IncrementNumMsgSntUsr (long UsrCod)
  {
   /***** Increment number of messages sent *****/
   // If NumMsgSnt < 0 ==> not yet calculated, so do nothing
   DB_QueryINSERT ("can not increment user's messages sent",
		   "UPDATE IGNORE usr_figures SET NumMsgSnt=NumMsgSnt+1"
		   " WHERE UsrCod=%ld AND NumMsgSnt>=0",
	           UsrCod);
  }

/*****************************************************************************/
/******** Get and show ranking of users attending to number of clicks ********/
/*****************************************************************************/

void Prf_GetAndShowRankingClicks (void)
  {
   Prf_GetAndShowRankingFigure ("NumClicks");
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
      case Sco_SCOPE_SYS:
	 NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT UsrCod,%s"
				    " FROM usr_figures"
				    " WHERE %s>=0"
				    " AND UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY %s DESC,UsrCod LIMIT 100",
				    FieldName,
				    FieldName,FieldName);
         break;
      case Sco_SCOPE_CTY:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
				    " FROM institutions,centres,degrees,courses,crs_usr,usr_figures"
				    " WHERE institutions.CtyCod=%ld"
				    " AND institutions.InsCod=centres.InsCod"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.%s>=0"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
				    FieldName,
				    Gbl.CurrentCty.Cty.CtyCod,
				    FieldName,FieldName);
         break;
      case Sco_SCOPE_INS:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
				    " FROM centres,degrees,courses,crs_usr,usr_figures"
				    " WHERE centres.InsCod=%ld"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.%s>=0"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
				    FieldName,
				    Gbl.CurrentIns.Ins.InsCod,
				    FieldName,FieldName);
         break;
      case Sco_SCOPE_CTR:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
				    " FROM degrees,courses,crs_usr,usr_figures"
				    " WHERE degrees.CtrCod=%ld"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.%s>=0"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
				    FieldName,
				    Gbl.CurrentCtr.Ctr.CtrCod,
				    FieldName,FieldName);
         break;
      case Sco_SCOPE_DEG:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
				    " FROM courses,crs_usr,usr_figures"
				    " WHERE courses.DegCod=%ld"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.%s>=0"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
				    FieldName,
				    Gbl.CurrentDeg.Deg.DegCod,
				    FieldName,FieldName);
         break;
      case Sco_SCOPE_CRS:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
				    " FROM crs_usr,usr_figures"
				    " WHERE crs_usr.CrsCod=%ld"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.%s>=0"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
				    FieldName,
				    Gbl.CurrentCrs.Crs.CrsCod,
				    FieldName,FieldName);
         break;
      default:
         Lay_WrongScopeExit ();
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
   long FigureHigh = LONG_MAX;
   long Figure;

   if (NumUsrs)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      fprintf (Gbl.F.Out,"<table>");

      for (NumUsr = 1, Rank = 1, Gbl.RowEvenOdd = 0;
	   NumUsr <= NumUsrs;
	   NumUsr++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
	{
	 /***** Get user and number of clicks *****/
	 row = mysql_fetch_row (*mysql_res);

	 /* Get user's code (row[0]) */
	 UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	 Usr_GetAllUsrDataFromUsrCod (&UsrDat);

	 /* Get figure (row[1]) */
	 if (sscanf (row[1],"%ld",&Figure) != 1)
	    Lay_ShowErrorAndExit ("Error reading user's figure.");

	 if (Figure < FigureHigh)
	   {
	    Rank = NumUsr;
	    FigureHigh = Figure;
	   }

	 /***** Show row *****/
	 fprintf (Gbl.F.Out,"<tr>");
         Prf_ShowUsrInRanking (&UsrDat,Rank);
	 fprintf (Gbl.F.Out,"<td class=\"RIGHT_MIDDLE COLOR%u\""
	                    " style=\"height:50px;\">"
	                    "%ld"
	                    "</td>"
			    "</tr>",
		  Gbl.RowEvenOdd,
		  Figure);
	}

      fprintf (Gbl.F.Out,"</table>");

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
   float NumClicksPerDayHigh = (float) LONG_MAX;
   float NumClicksPerDay;

   /***** Get ranking from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT UsrCod,"
				    "NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1) AS NumClicksPerDay"
				    " FROM usr_figures"
				    " WHERE NumClicks>0"
				    " AND FirstClickTime>FROM_UNIXTIME(0)"
				    " AND UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY NumClicksPerDay DESC,UsrCod LIMIT 100");
         break;
      case Sco_SCOPE_CTY:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,"
				    "usr_figures.NumClicks/(DATEDIFF(NOW(),"
				    "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
				    " FROM institutions,centres,degrees,courses,crs_usr,usr_figures"
				    " WHERE institutions.CtyCod=%ld"
				    " AND institutions.InsCod=centres.InsCod"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.NumClicks>0"
				    " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
				    Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,"
				    "usr_figures.NumClicks/(DATEDIFF(NOW(),"
				    "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
				    " FROM centres,degrees,courses,crs_usr,usr_figures"
				    " WHERE centres.InsCod=%ld"
				    " AND centres.CtrCod=degrees.CtrCod"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.NumClicks>0"
				    " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
				    Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,"
				    "usr_figures.NumClicks/(DATEDIFF(NOW(),"
				    "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
				    " FROM degrees,courses,crs_usr,usr_figures"
				    " WHERE degrees.CtrCod=%ld"
				    " AND degrees.DegCod=courses.DegCod"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.NumClicks>0"
				    " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
				    Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,"
				    "usr_figures.NumClicks/(DATEDIFF(NOW(),"
				    "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
				    " FROM courses,crs_usr,usr_figures"
				    " WHERE courses.DegCod=%ld"
				    " AND courses.CrsCod=crs_usr.CrsCod"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.NumClicks>0"
				    " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
				    Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         NumUsrs =
         (unsigned) DB_QuerySELECT (&mysql_res,"can not get ranking",
				    "SELECT DISTINCTROW usr_figures.UsrCod,"
				    "usr_figures.NumClicks/(DATEDIFF(NOW(),"
				    "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
				    " FROM crs_usr,usr_figures"
				    " WHERE crs_usr.CrsCod=%ld"
				    " AND crs_usr.UsrCod=usr_figures.UsrCod"
				    " AND usr_figures.NumClicks>0"
				    " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
				    " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
				    " ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
				    Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
         Lay_WrongScopeExit ();
         break;
     }

   if (NumUsrs)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      fprintf (Gbl.F.Out,"<table>");

      for (NumUsr = 1, Rank = 1, Gbl.RowEvenOdd = 0;
	   NumUsr <= NumUsrs;
	   NumUsr++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
	{
	 /***** Get user and number of clicks *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get user's code (row[0]) */
	 UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	 Usr_GetAllUsrDataFromUsrCod (&UsrDat);

	 /* Get number of clicks per day (row[1]) */
	 NumClicksPerDay = Str_GetFloatNumFromStr (row[1]);
	 if (NumClicksPerDay < NumClicksPerDayHigh)
	   {
	    Rank = NumUsr;
	    NumClicksPerDayHigh = NumClicksPerDay;
	   }

	 /***** Show row *****/
	 fprintf (Gbl.F.Out,"<tr>");
	 Prf_ShowUsrInRanking (&UsrDat,Rank);
	 fprintf (Gbl.F.Out,"<td class=\"RIGHT_MIDDLE COLOR%u\""
	                    " style=\"height:50px;\">",
	          Gbl.RowEvenOdd);
	 Str_WriteFloatNum (Gbl.F.Out,NumClicksPerDay);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	}

      fprintf (Gbl.F.Out,"</table>");

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Show user's photo and nickname in ranking list ***************/
/*****************************************************************************/

void Prf_ShowUsrInRanking (struct UsrData *UsrDat,unsigned Rank)
  {
   extern const char *Txt_Another_user_s_profile;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool Visible = Pri_ShowingIsAllowed (UsrDat->ProfileVisibility,UsrDat);

   fprintf (Gbl.F.Out,"<td class=\"RANK RIGHT_MIDDLE COLOR%u\""
	              " style=\"height:50px;\">"
		      "#%u"
		      "</td>"
                      "<td class=\"COLOR%u\""
                      " style=\"width:35px; height:50px;\">",
	    Gbl.RowEvenOdd,
	    Rank,
            Gbl.RowEvenOdd);

   /***** Check if I can see the public profile *****/
   if (Visible)
     {
      /***** User's photo *****/
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					   NULL,
			"PHOTO30x40",Pho_ZOOM,false);
     }

   fprintf (Gbl.F.Out,"</td>"
		      "<td class=\"COLOR%u\" style=\"height:50px;\">",
            Gbl.RowEvenOdd);

   /***** Put form to go to public profile *****/
   if (Visible)
     {
      Act_StartForm (ActSeeOthPubPrf);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      fprintf (Gbl.F.Out,"<div class=\"RANK_USR\">");	// Limited width
      Act_LinkFormSubmit (Txt_Another_user_s_profile,"DAT_SMALL",NULL);
      Usr_WriteFirstNameBRSurnames (UsrDat);
      fprintf (Gbl.F.Out,"</a>"
	                 "</div>");
      Act_EndForm ();
     }

   fprintf (Gbl.F.Out,"</td>");
  }
