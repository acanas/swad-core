// swad_profile.c: user's public profile

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

#include <linux/stddef.h>	// For NULL

#include "swad_config.h"
#include "swad_database.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_nickname.h"
#include "swad_parameter.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_role.h"
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

struct UsrFigures
  {
   time_t FirstClickTimeUTC;	//   0 ==> unknown first click time of user never logged
   int NumDays;			//  -1 ==> not applicable
   long NumClicks;		// -1L ==> unknown number of clicks
   long NumFileViews;		// -1L ==> unknown number of file views
   long NumForPst;		// -1L ==> unknown number of forum posts
   long NumMsgSnt;		// -1L ==> unknown number of messages sent
  };

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

static void Prf_RequestUserProfileWithDefaultNickname (const char *DefaultNickname);

static void Prf_GetUsrDatAndShowUserProfile (void);
static void Prf_ShowDetailsUserProfile (const struct UsrData *UsrDat);

static void Prf_GetUsrFigures (long UsrCod,struct UsrFigures *UsrFigures);

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
static void Prf_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures);
static bool Prf_CheckIfUsrFiguresExists (long UsrCod);

static void Prf_GetAndShowRankingFigure (const char *FieldName);

/*****************************************************************************/
/************************** Get public profile URL ***************************/
/*****************************************************************************/

char *Prf_GetURLPublicProfile (char *URL,const char *NicknameWithoutArroba)
  {
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];

   /***** Build URL using nickname *****/
   sprintf (URL,"%s/%s?usr=@%s",
	    Cfg_HTTPS_URL_SWAD_CGI,
	    Txt_STR_LANG_ID[Gbl.Prefs.Language],
	    NicknameWithoutArroba);

   return URL;
  }

/*****************************************************************************/
/************************** Request a user's profile *************************/
/*****************************************************************************/

void Prf_RequestUserProfile (void)
  {
   /* By default, the nickname is filled with my nickname
      If no user logged ==> the nickname is empty */
   Prf_RequestUserProfileWithDefaultNickname (Gbl.Usrs.Me.UsrDat.Nickname);
  }

/*****************************************************************************/
/*************** Request a user's profile with nickname filled ***************/
/*****************************************************************************/

static void Prf_RequestUserProfileWithDefaultNickname (const char *DefaultNickname)
  {
   extern const char *Txt_View_public_profile;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Nickname;
   extern const char *Txt_Continue;

   /***** Start form *****/
   Act_FormStart (ActSeePubPrf);

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,Txt_View_public_profile);

   /***** Form to request user's @nickname *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
                      "%s:&nbsp;"
                      "</label>"
                      "<input type=\"text\" name=\"usr\""
                      " size=\"20\" maxlength=\"%u\" value=\"@%s\" />",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Nickname,
            Nck_MAX_BYTES_NICKNAME_WITH_ARROBA,
            DefaultNickname);

   /***** Send button and end frame *****/
   Lay_EndRoundFrameWithButton (Lay_CONFIRM_BUTTON,Txt_Continue);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/**************** Get user's code and show a user's profile ******************/
/*****************************************************************************/

void Prf_GetUsrCodAndShowUserProfile (void)
  {
   /***** Try to get user *****/
   // User's code may be already taken from nickname in Par_GetMainParameters ()
   if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0)
      // If user is not set, try to get it from user code
      Usr_GetParamOtherUsrCodEncrypted ();

   /***** Show user's profile *****/
   Prf_GetUsrDatAndShowUserProfile ();
  }

/*****************************************************************************/
/*************************** Show a user's profile ***************************/
/*****************************************************************************/
// If error, Nickname is used to fill the form to request another nickname

static void Prf_GetUsrDatAndShowUserProfile (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   bool Error;

   /***** Check if user exists and get his data *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // Existing user
      /***** Show public profile *****/
      Error = !Prf_ShowUserProfile ();
   else
      Error = true;

   if (Error)
     {
      /***** Show error message *****/
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

      /***** Request nickname again *****/
      Prf_RequestUserProfileWithDefaultNickname ("");
     }
  }

/*****************************************************************************/
/*************************** Show a user's profile ***************************/
/*****************************************************************************/
// Return false on error

bool Prf_ShowUserProfile (void)
  {
   /***** Check if I can see the public profile *****/
   if (Pri_ShowIsAllowed (Gbl.Usrs.Other.UsrDat.ProfileVisibility,Gbl.Usrs.Other.UsrDat.UsrCod))
     {
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	{
	 /* Get user's role in current course */
	 Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB = Rol_GetRoleInCrs (Gbl.CurrentCrs.Crs.CrsCod,Gbl.Usrs.Other.UsrDat.UsrCod);

	 /* Get if user has accepted enrollment in current course */
	 Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrBelongsToCrs (Gbl.Usrs.Other.UsrDat.UsrCod,
	                                                              Gbl.CurrentCrs.Crs.CrsCod,
	                                                              true);
	}

      /***** Common record *****/
      Rec_ShowSharedUsrRecord (Rec_RECORD_PUBLIC,&Gbl.Usrs.Other.UsrDat);

      /***** Show details of user's profile *****/
      Prf_ShowDetailsUserProfile (&Gbl.Usrs.Other.UsrDat);

      /***** Show following and followers *****/
      Fol_ShowFollowingAndFollowers (&Gbl.Usrs.Other.UsrDat);

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
   extern const char *Txt_The_visibility_of_your_public_profile_has_changed;
   char Query[128];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.ProfileVisibility = Pri_GetParamVisibility ();

   /***** Store public/private photo in database *****/
   sprintf (Query,"UPDATE usr_data SET ProfileVisibility='%s'"
	          " WHERE UsrCod='%ld'",
            Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.ProfileVisibility],
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your preference about public profile visibility");

   /***** Show alert *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_The_visibility_of_your_public_profile_has_changed);

   /***** Show form again *****/
   Pri_EditMyPrivacy ();
  }

/*****************************************************************************/
/********************** Show details of user's profile ***********************/
/*****************************************************************************/

static void Prf_ShowDetailsUserProfile (const struct UsrData *UsrDat)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_teachers_ABBREVIATION;
   extern const char *Txt_students_ABBREVIATION;
   extern const char *Txt_course;
   extern const char *Txt_courses;
   extern const char *Txt_Files;
   extern const char *Txt_file;
   extern const char *Txt_files;
   extern const char *Txt_public_FILES;
   extern const char *Txt_From_TIME;
   extern const char *Txt_day;
   extern const char *Txt_days;
   extern const char *Txt_Calculate;
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
   unsigned NumStds;
   unsigned NumTchs;
   unsigned NumFiles;
   unsigned NumPublicFiles;
   // char StrTimeGenerationInMicroseconds[64];

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<div id=\"prf_fig_container\">"
	              "<ul id=\"prf_fig_list\" class=\"DAT\">");

   /***** Number of courses in which the user is teacher or student *****/
   if ((NumCrssUsrIsTeacher = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Rol_TEACHER)))
     {
      NumTchs = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_TEACHER,Rol_TEACHER);
      NumStds = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_TEACHER,Rol_STUDENT);
      fprintf (Gbl.F.Out,"<li>"
                         "<img src=\"%s/tch64x64.gif\""
                         " alt=\"%s\" title=\"%s\""
	                 " class=\"PRF_ICON\" />"
			 "</td>"
			 "<td class=\"PRF_FIG DAT\">"
			 "%u&nbsp;%s&nbsp;(%u&nbsp;%s&nbsp;+&nbsp;%u&nbsp;%s)"
			 "</li>",
               Gbl.Prefs.IconsURL,
	       Txt_ROLES_SINGUL_Abc[Rol_TEACHER][UsrDat->Sex],
	       Txt_ROLES_SINGUL_Abc[Rol_TEACHER][UsrDat->Sex],
	       NumCrssUsrIsTeacher,
	       (NumCrssUsrIsTeacher == 1) ? Txt_course :
		                            Txt_courses,
	       NumTchs,
	       Txt_teachers_ABBREVIATION,
	       NumStds,
	       Txt_students_ABBREVIATION);
     }
   if ((NumCrssUsrIsStudent = Usr_GetNumCrssOfUsrWithARole (UsrDat->UsrCod,Rol_STUDENT)))
     {
      NumTchs = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_STUDENT,Rol_TEACHER);
      NumStds = Usr_GetNumUsrsInCrssOfAUsr (UsrDat->UsrCod,Rol_STUDENT,Rol_STUDENT);
      fprintf (Gbl.F.Out,"<li>"
                         "<img src=\"%s/std64x64.gif\""
                         " alt=\"%s\" title=\"%s\""
	                 " class=\"PRF_ICON\" />"
			 "%u&nbsp;%s&nbsp;(%u&nbsp;%s&nbsp;+&nbsp;%u&nbsp;%s)"
			 "</li>",
               Gbl.Prefs.IconsURL,
	       Txt_ROLES_SINGUL_Abc[Rol_STUDENT][UsrDat->Sex],
	       Txt_ROLES_SINGUL_Abc[Rol_STUDENT][UsrDat->Sex],
	       NumCrssUsrIsStudent,
	       (NumCrssUsrIsStudent == 1) ? Txt_course :
	                                    Txt_courses,
	       NumTchs,
	       Txt_teachers_ABBREVIATION,
	       NumStds,
	       Txt_students_ABBREVIATION);
     }

   /***** Number of files currently published *****/
   if ((NumFiles = Brw_GetNumFilesUsr (UsrDat->UsrCod)))
      NumPublicFiles = Brw_GetNumPublicFilesUsr (UsrDat->UsrCod);
   else
      NumPublicFiles = 0;
   fprintf (Gbl.F.Out,"<li>"
                      "<img src=\"%s/file64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
	              " class=\"PRF_ICON\" />"
		      "%u&nbsp;%s&nbsp;(%u&nbsp;%s)"
		      "</li>",
            Gbl.Prefs.IconsURL,
	    Txt_Files,
	    Txt_Files,
	    NumFiles,
	    (NumFiles == 1) ? Txt_file :
		              Txt_files,
	    NumPublicFiles,Txt_public_FILES);

   /***** Get figures *****/
   Prf_GetUsrFigures (UsrDat->UsrCod,&UsrFigures);

   /* First click time */
   fprintf (Gbl.F.Out,"<li>"
                      "<img src=\"%s/clock64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
	              " class=\"PRF_ICON\" />",
            Gbl.Prefs.IconsURL,
	    Txt_From_TIME,
	    Txt_From_TIME);
   if (UsrFigures.FirstClickTimeUTC)
     {
      fprintf (Gbl.F.Out,"<span id=\"first_click_time\"></span>");
      if (UsrFigures.NumDays > 0)
	 fprintf (Gbl.F.Out,"&nbsp;(%d&nbsp;%s)",
		  UsrFigures.NumDays,
		  (UsrFigures.NumDays == 1) ? Txt_day :
					      Txt_days);
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                         "writeLocalDateTimeFromUTC('first_click_time',%ld,'&nbsp;');"
                         "</script>",
               (long) UsrFigures.FirstClickTimeUTC);
     }
   else	// First click time is unknown or user never logged
     {
      /***** Button to fetch and store first click time *****/
      Act_FormStart (ActCal1stClkTim);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormBold[Gbl.Prefs.Theme]);
      Lay_PutCalculateIconWithText (Txt_Calculate,Txt_Calculate);
      Act_FormEnd ();
     }
   fprintf (Gbl.F.Out,"</li>");

   UsrIsBannedFromRanking = Usr_CheckIfUsrBanned (UsrDat->UsrCod);
   if (!UsrIsBannedFromRanking)
     {
      /* Number of clicks */
      fprintf (Gbl.F.Out,"<li>"
                         "<img src=\"%s/click64x64.gif\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"PRF_ICON\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Clicks,
	       Txt_Clicks);

      if (UsrFigures.NumClicks >= 0)
	{
	 fprintf (Gbl.F.Out,"%ld&nbsp;%s&nbsp;",
		  UsrFigures.NumClicks,Txt_clicks);
	 Prf_ShowRanking (Prf_GetRankingFigure (UsrDat->UsrCod,"NumClicks"),
			  Prf_GetNumUsrsWithFigure ("NumClicks"));
	 if (UsrFigures.NumDays > 0)
	   {
	    fprintf (Gbl.F.Out,"&nbsp;(");
	    Str_WriteFloatNum ((float) UsrFigures.NumClicks /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"&nbsp;/&nbsp;%s&nbsp;",Txt_day);
	    Prf_ShowRanking (Prf_GetRankingNumClicksPerDay (UsrDat->UsrCod),
			     Prf_GetNumUsrsWithNumClicksPerDay ());
	    fprintf (Gbl.F.Out,")");
	   }
	}
      else	// Number of clicks is unknown
	{
	 /***** Button to fetch and store number of clicks *****/
	 Act_FormStart (ActCalNumClk);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormBold[Gbl.Prefs.Theme]);
	 Lay_PutCalculateIconWithText (Txt_Calculate,Txt_Calculate);
	 Act_FormEnd ();
	}
      fprintf (Gbl.F.Out,"</li>");

      /***** Number of file views *****/
      fprintf (Gbl.F.Out,"<li>"
                         "<img src=\"%s/download64x64.gif\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"PRF_ICON\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Downloads,
	       Txt_Downloads);
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
	    Str_WriteFloatNum ((float) UsrFigures.NumFileViews /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"&nbsp;/&nbsp;%s)",Txt_day);
	   }
	}
      else	// Number of file views is unknown
	{
	 /***** Button to fetch and store number of file views *****/
	 Act_FormStart (ActCalNumFilVie);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormBold[Gbl.Prefs.Theme]);
	 Lay_PutCalculateIconWithText (Txt_Calculate,Txt_Calculate);
	 Act_FormEnd ();
	}
      fprintf (Gbl.F.Out,"</li>");

      /***** Number of posts in forums *****/
      fprintf (Gbl.F.Out,"<li>"
                         "<img src=\"%s/forum64x64.gif\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"PRF_ICON\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Forums,
	       Txt_Forums);
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
	    Str_WriteFloatNum ((float) UsrFigures.NumForPst /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"&nbsp;/&nbsp;%s)",Txt_day);
	   }
	}
      else	// Number of forum posts is unknown
	{
	 /***** Button to fetch and store number of forum posts *****/
	 Act_FormStart (ActCalNumForPst);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormBold[Gbl.Prefs.Theme]);
	 Lay_PutCalculateIconWithText (Txt_Calculate,Txt_Calculate);
	 Act_FormEnd ();
	}
      fprintf (Gbl.F.Out,"</li>");

      /***** Number of messages sent *****/
      fprintf (Gbl.F.Out,"<li>"
                         "<img src=\"%s/msg64x64.gif\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"PRF_ICON\" />",
	       Gbl.Prefs.IconsURL,
	       Txt_Messages,
	       Txt_Messages);
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
	    Str_WriteFloatNum ((float) UsrFigures.NumMsgSnt /
			       (float) UsrFigures.NumDays);
	    fprintf (Gbl.F.Out,"&nbsp;/&nbsp;%s)",Txt_day);
	   }
	}
      else	// Number of clicks is unknown
	{
	 /***** Button to fetch and store number of messages sent *****/
	 Act_FormStart (ActCalNumMsgSnt);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	 Act_LinkFormSubmitAnimated (Txt_Calculate,The_ClassFormBold[Gbl.Prefs.Theme]);
	 Lay_PutCalculateIconWithText (Txt_Calculate,Txt_Calculate);
	 Act_FormEnd ();
	}
      fprintf (Gbl.F.Out,"</li>");
     }

   /***** End of table *****/
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
  }

/*****************************************************************************/
/********************** Select values on user's figures **********************/
/*****************************************************************************/

static void Prf_GetUsrFigures (long UsrCod,struct UsrFigures *UsrFigures)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;

   /***** Get user's figures from database *****/
   sprintf (Query,"SELECT UNIX_TIMESTAMP(FirstClickTime),"
	          "DATEDIFF(NOW(),FirstClickTime)+1,"
	          "NumClicks,NumFileViews,NumForPst,NumMsgSnt"
	          " FROM usr_figures WHERE UsrCod='%ld'",
	    UsrCod);
   if ((NumRows = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get user's figures")))
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
   char Query[256];

   /***** Select number of rows with figure
          greater than the figure of this user *****/
   sprintf (Query,"SELECT COUNT(*)+1 FROM usr_figures"
	          " WHERE UsrCod<>'%ld'"	// Really not necessary here
                  " AND %s>"
	          "(SELECT %s FROM usr_figures WHERE UsrCod='%ld')",
	    UsrCod,FieldName,FieldName,UsrCod);
   return DB_QueryCOUNT (Query,"can not get ranking using a figure");
  }

/*****************************************************************************/
/********************* Get number of users with a figure *********************/
/*****************************************************************************/

static unsigned long Prf_GetNumUsrsWithFigure (const char *FieldName)
  {
   char Query[128];

   /***** Select number of rows with values already calculated *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_figures WHERE %s>='0'",
            FieldName);
   return DB_QueryCOUNT (Query,"can not get number of users with a figure");
  }

/*****************************************************************************/
/****** Get ranking of a user according to the number of clicks per day ******/
/*****************************************************************************/

static unsigned long Prf_GetRankingNumClicksPerDay (long UsrCod)
  {
   char Query[512];

   /***** Select number of rows with number of clicks per day
          greater than the clicks per day of this user *****/
   sprintf (Query,"SELECT COUNT(*)+1 FROM"
                  " (SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
                  " AS NumClicksPerDay"
                  " FROM usr_figures"
                  " WHERE UsrCod<>'%ld'"	// Necessary because the following comparison is not exact in floating point
                  " AND NumClicks>='0' AND FirstClickTime>FROM_UNIXTIME('0'))"
                  " AS TableNumClicksPerDay"
                  " WHERE NumClicksPerDay>"
                  "(SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
                  " FROM usr_figures"
                  " WHERE UsrCod='%ld'"
                  " AND NumClicks>='0' AND FirstClickTime>FROM_UNIXTIME('0'))",
	    UsrCod,UsrCod);
   return DB_QueryCOUNT (Query,"can not get ranking using number of clicks per day");
  }

/*****************************************************************************/
/************** Get number of users with number of clicks per day ************/
/*****************************************************************************/

static unsigned long Prf_GetNumUsrsWithNumClicksPerDay (void)
  {
   char Query[128];

   /***** Select number of rows with values already calculated *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_figures"
	          " WHERE NumClicks>='0' AND FirstClickTime>FROM_UNIXTIME('0')");
   return DB_QueryCOUNT (Query,"can not get number of users with number of clicks per day");
  }

/*****************************************************************************/
/************************* Show position in ranking **************************/
/*****************************************************************************/

static void Prf_ShowRanking (unsigned long Rank,unsigned long NumUsrs)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_of_PART_OF_A_TOTAL;

   /***** Part of a total and end container *****/
   sprintf (Gbl.Title,"#%lu %s %lu",
	    Rank,Txt_of_PART_OF_A_TOTAL,NumUsrs);

   /***** Rank in form to go to ranking *****/
   Act_FormStart (ActSeeUseGbl);
   Sco_PutParamScope (Sco_SCOPE_SYS);
   Par_PutHiddenParamUnsigned ("FigureType",(unsigned) Sta_USERS_RANKING);
   Act_LinkFormSubmit (Gbl.Title,The_ClassForm[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"#%lu</a>",Rank);
   Act_FormEnd ();
  }

/*****************************************************************************/
/********* Calculate first click time and show user's profile again **********/
/*****************************************************************************/

void Prf_CalculateFirstClickTime (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get first click time from log and store as user's figure *****/
   Prf_GetFirstClickFromLogAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Prf_GetUsrDatAndShowUserProfile ();
  }

/*****************************************************************************/
/*** Get first click of a user from log table and store in user's figures ****/
/*****************************************************************************/

static void Prf_GetFirstClickFromLogAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get first click from log table *****/
      sprintf (Query,"SELECT UNIX_TIMESTAMP("
	             "(SELECT MIN(ClickTime) FROM log_full WHERE UsrCod='%ld')"
	             ")",
	       UsrCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get user's first click"))
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
	{
	 sprintf (Query,"UPDATE usr_figures SET FirstClickTime=FROM_UNIXTIME('%ld')"
			" WHERE UsrCod='%ld'",
		  (long) UsrFigures.FirstClickTimeUTC,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures);
     }
  }

/*****************************************************************************/
/********* Calculate number of clicks and show user's profile again **********/
/*****************************************************************************/

void Prf_CalculateNumClicks (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get number of clicks and store as user's figure *****/
   Prf_GetNumClicksAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Prf_GetUsrDatAndShowUserProfile ();
  }

/*****************************************************************************/
/* Get number of clicks of a user from log table and store in user's figures */
/*****************************************************************************/

static void Prf_GetNumClicksAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of clicks from database *****/
      sprintf (Query,"SELECT COUNT(*) FROM log_full WHERE UsrCod='%ld'",
	       UsrCod);
      UsrFigures.NumClicks = (long) DB_QueryCOUNT (Query,"can not get number of clicks");

      /***** Update number of clicks in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET NumClicks='%ld'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.NumClicks,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures);
     }
   }

/*****************************************************************************/
/******* Calculate number of file views and show user's profile again *******/
/*****************************************************************************/

void Prf_CalculateNumFileViews (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get number of file views and store as user's figure *****/
   Prf_GetNumFileViewsAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Prf_GetUsrDatAndShowUserProfile ();
  }

/*****************************************************************************/
/**** Get number of file views sent by a user and store in user's figures ****/
/*****************************************************************************/

static void Prf_GetNumFileViewsAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of file views from database *****/
      UsrFigures.NumFileViews = Brw_GetNumFileViewsUsr (UsrCod);

      /***** Update number of file views in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET NumFileViews='%ld'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.NumFileViews,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures);
     }
   }

/*****************************************************************************/
/******* Calculate number of forum posts and show user's profile again *******/
/*****************************************************************************/

void Prf_CalculateNumForPst (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get number of forum posts and store as user's figure *****/
   Prf_GetNumForPstAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Prf_GetUsrDatAndShowUserProfile ();
  }

/*****************************************************************************/
/**** Get number of forum posts sent by a user and store in user's figures ***/
/*****************************************************************************/

static void Prf_GetNumForPstAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of forum posts from database *****/
      UsrFigures.NumForPst = For_GetNumPostsUsr (UsrCod);

      /***** Update number of forum posts in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET NumForPst='%ld'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.NumForPst,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures);
     }
   }

/*****************************************************************************/
/****** Calculate number of messages sent and show user's profile again ******/
/*****************************************************************************/

void Prf_CalculateNumMsgSnt (void)
  {
   /***** Get user's code *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get number of messages sent and store as user's figure *****/
   Prf_GetNumMsgSntAndStoreAsUsrFigure (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Show user's profile again *****/
   Prf_GetUsrDatAndShowUserProfile ();
  }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

static void Prf_GetNumMsgSntAndStoreAsUsrFigure (long UsrCod)
  {
   char Query[256];
   struct UsrFigures UsrFigures;

   if (Usr_ChkIfUsrCodExists (UsrCod))
     {
      /***** Reset user's figures *****/
      Prf_ResetUsrFigures (&UsrFigures);

      /***** Get number of messages sent from database *****/
      UsrFigures.NumMsgSnt = Msg_GetNumMsgsSentByUsr (UsrCod);

      /***** Update number of messages sent in user's figures *****/
      if (Prf_CheckIfUsrFiguresExists (UsrCod))
	{
	 sprintf (Query,"UPDATE usr_figures SET NumMsgSnt='%ld'"
			" WHERE UsrCod='%ld'",
		  UsrFigures.NumMsgSnt,UsrCod);
	 DB_QueryUPDATE (Query,"can not update user's figures");
	}
      else			// User entry does not exist
	 Prf_CreateUsrFigures (UsrCod,&UsrFigures);
     }
  }

/*****************************************************************************/
/******************* Create user's figures for a new user ********************/
/*****************************************************************************/

void Prf_CreateNewUsrFigures (long UsrCod)
  {
   struct UsrFigures UsrFigures;

   /***** Reset user's figures *****/
   Prf_ResetUsrFigures (&UsrFigures);

   /***** Create user's figures *****/
   Prf_CreateUsrFigures (UsrCod,&UsrFigures);
  }

/*****************************************************************************/
/********************** Reset values of user's figures ***********************/
/*****************************************************************************/

static void Prf_ResetUsrFigures (struct UsrFigures *UsrFigures)
  {
   UsrFigures->FirstClickTimeUTC = (time_t) 0;	// unknown first click time or user never logged
   UsrFigures->NumDays      = -1;		// not applicable
   UsrFigures->NumClicks    = -1L;		// unknown number of clicks
   UsrFigures->NumFileViews = -1L;		// unknown number of file views
   UsrFigures->NumForPst    = -1L;		// unknown number of forum posts
   UsrFigures->NumMsgSnt    = -1L;		// unknown number of messages sent
  }

/*****************************************************************************/
/***** Get number of messages sent by a user and store in user's figures *****/
/*****************************************************************************/

static void Prf_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures)
  {
   char Query[512];

   /***** Create user's figures *****/
   sprintf (Query,"INSERT INTO usr_figures"
	          "(UsrCod,FirstClickTime,NumClicks,NumFileViews,NumForPst,NumMsgSnt)"
		  " VALUES ('%ld',FROM_UNIXTIME('%ld'),'%ld','%ld','%ld','%ld')",
	    UsrCod,
	    (long) UsrFigures->FirstClickTimeUTC,	//   0 ==> unknown first click time or user never logged
	    UsrFigures->NumClicks,			// -1L ==> unknown number of clicks
	    UsrFigures->NumFileViews,			// -1L ==> unknown number of file views
	    UsrFigures->NumForPst,			// -1L ==> unknown number of forum posts
	    UsrFigures->NumMsgSnt);			// -1L ==> unknown number of messages sent
   DB_QueryINSERT (Query,"can not create user's figures");
  }

/*****************************************************************************/
/**************************** Remove user's figures **************************/
/*****************************************************************************/

void Prf_RemoveUsrFigures (long UsrCod)
  {
   char Query[128];

   /***** Remove user's figures *****/
   sprintf (Query,"DELETE FROM usr_figures WHERE UsrCod='%ld'",
	    UsrCod);
   DB_QueryDELETE (Query,"can not delete user's figures");
  }

/*****************************************************************************/
/*** Check if it exists an entry for this user in table of user's figures ****/
/*****************************************************************************/

static bool Prf_CheckIfUsrFiguresExists (long UsrCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM usr_figures WHERE UsrCod='%ld'",
	    UsrCod);
   return (DB_QueryCOUNT (Query,"can not get user's first click") != 0);
  }

/*****************************************************************************/
/*************** Increment number of clicks made by a user *******************/
/*****************************************************************************/

void Prf_IncrementNumClicksUsr (long UsrCod)
  {
   char Query[256];

   /***** Increment number of clicks *****/
   // If NumClicks < 0 ==> not yet calculated, so do nothing
   sprintf (Query,"UPDATE IGNORE usr_figures SET NumClicks=NumClicks+1"
	          " WHERE UsrCod='%ld' AND NumClicks>=0",
	    UsrCod);
   DB_QueryINSERT (Query,"can not increment user's clicks");
  }

/*****************************************************************************/
/************** Increment number of file views sent by a user ****************/
/*****************************************************************************/

void Prf_IncrementNumFileViewsUsr (long UsrCod)
  {
   char Query[256];

   /***** Increment number of file views *****/
   // If NumFileViews < 0 ==> not yet calculated, so do nothing
   sprintf (Query,"UPDATE IGNORE usr_figures SET NumFileViews=NumFileViews+1"
	          " WHERE UsrCod='%ld' AND NumFileViews>=0",
	    UsrCod);
   DB_QueryINSERT (Query,"can not increment user's file views");
  }

/*****************************************************************************/
/************* Increment number of forum posts sent by a user ****************/
/*****************************************************************************/

void Prf_IncrementNumForPstUsr (long UsrCod)
  {
   char Query[256];

   /***** Increment number of forum posts *****/
   // If NumForPst < 0 ==> not yet calculated, so do nothing
   sprintf (Query,"UPDATE IGNORE usr_figures SET NumForPst=NumForPst+1"
	          " WHERE UsrCod='%ld' AND NumForPst>=0",
	    UsrCod);
   DB_QueryINSERT (Query,"can not increment user's forum posts");
  }

/*****************************************************************************/
/*************** Increment number of messages sent by a user *****************/
/*****************************************************************************/

void Prf_IncrementNumMsgSntUsr (long UsrCod)
  {
   char Query[256];

   /***** Increment number of messages sent *****/
   // If NumMsgSnt < 0 ==> not yet calculated, so do nothing
   sprintf (Query,"UPDATE IGNORE usr_figures SET NumMsgSnt=NumMsgSnt+1"
	          " WHERE UsrCod='%ld' AND NumMsgSnt>=0",
	    UsrCod);
   DB_QueryINSERT (Query,"can not increment user's messages sent");
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
   char Query[1024];

   /***** Get ranking from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT UsrCod,%s"
	                " FROM usr_figures"
			" WHERE %s>='0'"
			" AND UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY %s DESC,UsrCod LIMIT 100",
		  FieldName,
		  FieldName,FieldName);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
                        " FROM institutions,centres,degrees,courses,crs_usr,usr_figures"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.%s>='0'"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
		  FieldName,
                  Gbl.CurrentCty.Cty.CtyCod,
                  FieldName,FieldName);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
                        " FROM centres,degrees,courses,crs_usr,usr_figures"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.%s>='0'"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
		  FieldName,
                  Gbl.CurrentIns.Ins.InsCod,
                  FieldName,FieldName);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
                        " FROM degrees,courses,crs_usr,usr_figures"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.%s>='0'"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
		  FieldName,
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  FieldName,FieldName);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
                        " FROM courses,crs_usr,usr_figures"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.%s>='0'"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
		  FieldName,
                  Gbl.CurrentDeg.Deg.DegCod,
                  FieldName,FieldName);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,usr_figures.%s"
                        " FROM crs_usr,usr_figures"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.%s>='0'"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY usr_figures.%s DESC,usr_figures.UsrCod LIMIT 100",
		  FieldName,
                  Gbl.CurrentCrs.Crs.CrsCod,
                  FieldName,FieldName);
         break;
      default:
         Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }
   Prf_ShowRankingFigure (Query);
  }

void Prf_ShowRankingFigure (const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   unsigned Rank;
   struct UsrData UsrDat;
   long FigureHigh = LONG_MAX;
   long Figure;

   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get ranking");
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
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**** Get and show ranking of users attending to number of clicks per day ****/
/*****************************************************************************/

void Prf_GetAndShowRankingClicksPerDay (void)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   unsigned Rank;
   struct UsrData UsrDat;
   float NumClicksPerDayHigh = (float) LONG_MAX;
   float NumClicksPerDay;

   /***** Get ranking from database *****/
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 sprintf (Query,"SELECT UsrCod,"
	                "NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1) AS NumClicksPerDay"
	                " FROM usr_figures"
			" WHERE FirstClickTime>0"
			" AND UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY NumClicksPerDay DESC,UsrCod LIMIT 100");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,"
                        "usr_figures.NumClicks/(DATEDIFF(NOW(),usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
                        " FROM institutions,centres,degrees,courses,crs_usr,usr_figures"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.FirstClickTime>0"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,"
                        "usr_figures.NumClicks/(DATEDIFF(NOW(),usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
                        " FROM centres,degrees,courses,crs_usr,usr_figures"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.FirstClickTime>0"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,"
                        "usr_figures.NumClicks/(DATEDIFF(NOW(),usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
                        " FROM degrees,courses,crs_usr,usr_figures"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.FirstClickTime>0"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,"
                        "usr_figures.NumClicks/(DATEDIFF(NOW(),usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
                        " FROM courses,crs_usr,usr_figures"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.FirstClickTime>0"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT DISTINCTROW usr_figures.UsrCod,"
                        "usr_figures.NumClicks/(DATEDIFF(NOW(),usr_figures.FirstClickTime)+1) AS NumClicksPerDay"
                        " FROM crs_usr,usr_figures"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.UsrCod=usr_figures.UsrCod"
			" AND usr_figures.FirstClickTime>0"
			" AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			" ORDER BY NumClicksPerDay DESC,usr_figures.UsrCod LIMIT 100",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
         Lay_ShowErrorAndExit ("Wrong scope.");
         break;
     }
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get ranking");
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
	 Str_WriteFloatNum (NumClicksPerDay);
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

void Prf_ShowUsrInRanking (const struct UsrData *UsrDat,unsigned Rank)
  {
   extern const char *Txt_View_public_profile;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   bool Visible = Pri_ShowIsAllowed (UsrDat->ProfileVisibility,UsrDat->UsrCod);

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
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					   NULL,
			"PHOTO24x32",Pho_ZOOM);
     }

   fprintf (Gbl.F.Out,"</td>"
		      "<td class=\"COLOR%u\" style=\"height:50px;\">",
            Gbl.RowEvenOdd);

   /***** Put form to go to public profile *****/
   if (Visible)
     {
      Act_FormStart (ActSeePubPrf);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
      Act_LinkFormSubmit (Txt_View_public_profile,"DAT_SMALL");
      Usr_RestrictLengthAndWriteName (UsrDat,8);
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }

   fprintf (Gbl.F.Out,"</td>");
  }
