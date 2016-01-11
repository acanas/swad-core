// swad_social.c: social networking (timeline)

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

#include <linux/limits.h>	// For PATH_MAX
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions
#include <sys/types.h>		// For time_t

#include "swad_constant.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_layout.h"
#include "swad_notice.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_social.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Soc_WIDTH_TIMELINE	    "560px"
#define Soc_MAX_NUM_SHARERS_SHOWN	 10		// Maximum number of users shown who have share a social note
#define Soc_MAX_BYTES_SUMMARY	 	100

// Number of recent publishings got and shown the first time, before refreshing
#define Soc_MAX_RECENT_PUBS_TO_SHOW	  50					// Publishings to show
#define Soc_MAX_RECENT_PUBS_TO_GET	  (Soc_MAX_RECENT_PUBS_TO_SHOW+1)	// Publishings to get

// Number of old publishings got and shown when I want to see old publishings
#define Soc_MAX_OLD_PUBS_TO_GET_AND_SHOW  10	// If you change this number, set also this constant to the new value in JavaScript

typedef enum
  {
   Soc_GET_RECENT_TIMELINE,
   Soc_GET_ONLY_NEW_PUBS,
   Soc_GET_ONLY_OLD_PUBS,
  } Soc_WhatToGetFromTimeline_t;

static const Act_Action_t Soc_DefaultActions[Soc_NUM_SOCIAL_NOTES] =
  {
   ActUnk,		// Soc_NOTE_UNKNOWN

   /* Institution tab */
   ActReqDatSeeDocIns,	// Soc_NOTE_INS_DOC_PUB_FILE
   ActReqDatShaIns,	// Soc_NOTE_INS_SHA_PUB_FILE

   /* Centre tab */
   ActReqDatSeeDocCtr,	// Soc_NOTE_CTR_DOC_PUB_FILE
   ActReqDatShaCtr,	// Soc_NOTE_CTR_SHA_PUB_FILE

   /* Degree tab */
   ActReqDatSeeDocDeg,	// Soc_NOTE_DEG_DOC_PUB_FILE
   ActReqDatShaDeg,	// Soc_NOTE_DEG_SHA_PUB_FILE

   /* Course tab */
   ActReqDatSeeDocCrs,	// Soc_NOTE_CRS_DOC_PUB_FILE
   ActReqDatShaCrs,	// Soc_NOTE_CRS_SHA_PUB_FILE

   /* Assessment tab */
   ActSeeExaAnn,	// Soc_NOTE_EXAM_ANNOUNCEMENT

   /* Users tab */

   /* Social tab */
   ActUnk,		// Soc_NOTE_SOCIAL_POST (action not used)
   ActSeeFor,		// Soc_NOTE_FORUM_POST

   /* Messages tab */
   ActShoNot,		// Soc_NOTE_NOTICE

   /* Statistics tab */

   /* Profile tab */

  };

static const char *Soc_Icons[Soc_NUM_SOCIAL_NOTES] =
  {
   NULL,		// Soc_NOTE_UNKNOWN

   /* Institution tab */
   "file64x64.gif",	// Soc_NOTE_INS_DOC_PUB_FILE
   "file64x64.gif",	// Soc_NOTE_INS_SHA_PUB_FILE

   /* Centre tab */
   "file64x64.gif",	// Soc_NOTE_CTR_DOC_PUB_FILE
   "file64x64.gif",	// Soc_NOTE_CTR_SHA_PUB_FILE

   /* Degree tab */
   "file64x64.gif",	// Soc_NOTE_DEG_DOC_PUB_FILE
   "file64x64.gif",	// Soc_NOTE_DEG_SHA_PUB_FILE

   /* Course tab */
   "file64x64.gif",	// Soc_NOTE_CRS_DOC_PUB_FILE
   "file64x64.gif",	// Soc_NOTE_CRS_SHA_PUB_FILE

   /* Assessment tab */
   "announce16x16.gif",	// Soc_NOTE_EXAM_ANNOUNCEMENT

   /* Users tab */

   /* Social tab */
   NULL,		// Soc_NOTE_SOCIAL_POST (icon not used)
   "forum64x64.gif",	// Soc_NOTE_FORUM_POST

   /* Messages tab */
   "note16x16.gif",	// Soc_NOTE_NOTICE

   /* Statistics tab */

   /* Profile tab */

  };

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

struct SocialPublishing
  {
   long PubCod;
   long NotCod;
   long PublisherCod;	// Sharer or writer of a comment
   Soc_PubType_t PubType;
   time_t DateTimeUTC;
  };

struct SocialNote
  {
   long NotCod;
   Soc_NoteType_t NoteType;
   long UsrCod;
   long HieCod;		// Hierarchy code (institution/centre/degree/course)
   long Cod;		// Code of file, forum post, notice,...
   bool Unavailable;	// File, forum post, notice,... unavailable (removed)
   time_t DateTimeUTC;
   unsigned NumShared;	// Number of times (users) this note has been shared
  };

struct SocialComment
  {
   long ComCod;
   long UsrCod;
   long NotCod;		// Note code
   time_t DateTimeUTC;
   char Content[Cns_MAX_BYTES_LONG_TEXT];
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

static void Soc_BuildQueryToGetTimelineGbl (Soc_WhatToGetFromTimeline_t WhatToGetFromTimeline,
                                            char *Query);
static long Soc_GetPubCodFromSession (const char *FieldName);
static void Soc_UpdateLastPubCodIntoSession (void);
static void Soc_UpdateFirstPubCodIntoSession (long FirstPubCod);
static void Soc_DropTemporaryTableWithPubCods (void);

static void Soc_ShowTimeline (const char *Query,const char *Title);
static void Soc_ShowNewPubsInTimeline (const char *Query);
static void Soc_ShowOldPubsInTimeline (const char *Query);

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub);

static void Soc_PutLinkToViewNewPublishings (void);
static void Soc_PutLinkToViewOldPublishings (void);

static void Soc_WriteSocialNote (const struct SocialPublishing *SocPub,
                                 const struct SocialNote *SocNot,
                                 bool ShowNoteAlone,
                                 bool ViewTopLine);
static void Soc_WriteDateTime (time_t TimeUTC);
static void Soc_GetAndWriteSocialPost (long PstCod);
static void Soc_PutFormGoToAction (const struct SocialNote *SocNot);
static void Soc_GetNoteSummary (const struct SocialNote *SocNot,
                                char *SummaryStr,unsigned MaxChars);
static void Soc_PublishSocialNoteInTimeline (struct SocialPublishing *SocPub);

static void Soc_PutHiddenFormToWriteNewPost (void);
static void Soc_ReceiveSocialPost (void);

static void Soc_PutIconToToggleCommentSocialNote (long PubCod,bool PutText,bool ShowNoteAlone);
static void Soc_PutHiddenFormToWriteNewCommentToSocialNote (long PubCod,bool ShowNoteAlone);
static unsigned long Soc_GetNumCommentsInSocialNote (long NotCod);
static void Soc_WriteCommentsInSocialNote (long PubCod,long NotCod,
                                           unsigned long NumComments,bool ShowNoteAlone);
static void Soc_WriteSocialComment (struct SocialComment *SocCom,
                                    bool ShowCommentAlone);
static void Soc_PutFormToRemoveComment (long ComCod);
static void Soc_PutDisabledIconShare (unsigned NumShared);
static void Soc_PutFormToShareSocialNote (long NotCod);
static void Soc_PutFormToUnshareSocialPublishing (long PubCod);
static void Soc_PutFormToRemoveSocialPublishing (long PubCod);

static void Soc_PutHiddenParamNotCod (long NotCod);
static void Soc_PutHiddenParamPubCod (long PubCod);
static void Soc_PutHiddenParamComCod (long ComCod);
static long Soc_GetParamNotCod (void);
static long Soc_GetParamPubCod (void);
static long Soc_GetParamComCod (void);

static void Soc_ReceiveComment (void);
static bool Soc_CheckIfICanCommentNote (long NotCod);

static void Soc_ShareSocialNote (void);
static void Soc_UnshareSocialPublishing (void);
static void Soc_UnshareASocialPublishingFromDB (struct SocialNote *SocNot);

static void Soc_RequestRemovalSocialNote (void);
static void Soc_RemoveSocialNote (void);
static void Soc_RemoveASocialNoteFromDB (struct SocialNote *SocNot);

static void Soc_RequestRemovalSocialComment (void);
static void Soc_RemoveSocialComment (void);
static void Soc_RemoveASocialCommentFromDB (struct SocialComment *SocCom);

static bool Soc_CheckIfNoteIsPublishedInTimelineByUsr (long NotCod,long UsrCod);
static void Soc_UpdateNumTimesANoteHasBeenShared (struct SocialNote *SocNot);
static void Soc_ShowUsrsWhoHaveSharedSocialNote (const struct SocialNote *SocNot);

static void Soc_GetDataOfSocialPublishingByCod (struct SocialPublishing *SocPub);
static void Soc_GetDataOfSocialNoteByCod (struct SocialNote *SocNot);
static void Soc_GetDataOfSocialCommentByCod (struct SocialComment *SocCom);

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub);
static void Soc_GetDataOfSocialNoteFromRow (MYSQL_ROW row,struct SocialNote *SocNot);
static Soc_NoteType_t Soc_GetNoteTypeFromStr (const char *Str);
static void Soc_GetDataOfSocialCommentFromRow (MYSQL_ROW row,struct SocialComment *SocCom);

static void Soc_ResetSocialPublishing (struct SocialPublishing *SocPub);
static void Soc_ResetSocialNote (struct SocialNote *SocNot);
static void Soc_ResetSocialComment (struct SocialComment *SocCom);

/*****************************************************************************/
/*********** Show social activity (timeline) of a selected user **************/
/*****************************************************************************/

void Soc_ShowTimelineUsr (void)
  {
   extern const char *Txt_Public_activity_OF_A_USER;
   char Query[512];

   /***** Build query to show timeline with publishing of a unique user *****/
   sprintf (Query,"SELECT PubCod,NotCod,PublisherCod,UNIX_TIMESTAMP(TimePublish)"
                  " FROM social_timeline"
                  " WHERE PublisherCod='%ld'"
                  " ORDER BY PubCod DESC LIMIT %u",
            Gbl.Usrs.Other.UsrDat.UsrCod,
            Soc_MAX_RECENT_PUBS_TO_GET);

   /***** Show timeline *****/
   sprintf (Gbl.Title,Txt_Public_activity_OF_A_USER,Gbl.Usrs.Other.UsrDat.FirstName);
   Soc_ShowTimeline (Query,Gbl.Title);
  }

/*****************************************************************************/
/***** Show social activity (timeline) including all the users I follow ******/
/*****************************************************************************/

void Soc_ShowTimelineGbl (void)
  {
   extern const char *Txt_Public_activity;
   extern const char *Txt_You_dont_follow_any_user;
   char Query[512];

   /***** Show warning if I do not follow anyone *****/
   if (!Fol_GetNumFollowing (Gbl.Usrs.Me.UsrDat.UsrCod))
      Lay_ShowAlert (Lay_INFO,Txt_You_dont_follow_any_user);

   /***** Build query to get timeline *****/
   Soc_BuildQueryToGetTimelineGbl (Soc_GET_RECENT_TIMELINE,Query);

   /***** Show timeline *****/
   Soc_ShowTimeline (Query,Txt_Public_activity);

   /***** Drop temporary table with publishing codes *****/
   Soc_DropTemporaryTableWithPubCods ();
  }

/*****************************************************************************/
/******* Get and show recent timeline including all the users I follow *******/
/*****************************************************************************/

void Soc_GetAndShowNewTimelineGbl (void)
  {
   char Query[512];

   /***** Build query to get timeline *****/
   Soc_BuildQueryToGetTimelineGbl (Soc_GET_ONLY_NEW_PUBS,Query);

   /***** Show new timeline *****/
   Soc_ShowNewPubsInTimeline (Query);

   /***** Drop temporary table with publishing codes *****/
   Soc_DropTemporaryTableWithPubCods ();
  }

/*****************************************************************************/
/******** Get and show old timeline including all the users I follow *********/
/*****************************************************************************/

void Soc_GetAndShowOldTimelineGbl (void)
  {
   char Query[512];

   /***** Build query to get timeline *****/
   Soc_BuildQueryToGetTimelineGbl (Soc_GET_ONLY_OLD_PUBS,Query);

   /***** Show old timeline *****/
   Soc_ShowOldPubsInTimeline (Query);

   /***** Drop temporary table with publishing codes *****/
   Soc_DropTemporaryTableWithPubCods ();
  }

/*****************************************************************************/
/************************ Build query to get timeline ************************/
/*****************************************************************************/
// Query must have space for at least 512 chars

static void Soc_BuildQueryToGetTimelineGbl (Soc_WhatToGetFromTimeline_t WhatToGetFromTimeline,
                                            char *Query)
  {
   char SubQueryRangePubs[64];
   char SubQueryLimit[64];
   long LastPubCod;
   long FirstPubCod;

   /****** Build subqueries *****/
   SubQueryRangePubs[0] = '\0';
   SubQueryLimit[0] = '\0';

   switch (WhatToGetFromTimeline)
     {
      case Soc_GET_RECENT_TIMELINE:
	 // Get some limited recent publihings
	 sprintf (SubQueryLimit," LIMIT %u",Soc_MAX_RECENT_PUBS_TO_GET);
	 break;
      case Soc_GET_ONLY_NEW_PUBS:
	 // Get the publishings (without limit) newer than LastPubCod
	 LastPubCod = Soc_GetPubCodFromSession ("LastPubCod");
	 if (LastPubCod > 0)
	    sprintf (SubQueryRangePubs,"PubCod>'%ld' AND ",LastPubCod);
         break;
      case Soc_GET_ONLY_OLD_PUBS:
	 // Get some limited publishings older than FirstPubCod
	 FirstPubCod = Soc_GetPubCodFromSession ("FirstPubCod");
	 if (FirstPubCod > 0)
	    sprintf (SubQueryRangePubs,"PubCod<'%ld' AND ",FirstPubCod);
	 sprintf (SubQueryLimit," LIMIT %u",Soc_MAX_OLD_PUBS_TO_GET_AND_SHOW);
         break;
     }

   /***** Remove temporary table with publishing codes *****/
   sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS pub_cods");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");

   /***** Create temporary table with publishing codes *****/
   sprintf (Query,"CREATE TEMPORARY TABLE pub_cods"
	          " (PubCod BIGINT NOT NULL,UNIQUE INDEX(PubCod)) ENGINE=MEMORY"
		  " SELECT MIN(PubCod) AS PubCod"
		  " FROM social_timeline"
		  " WHERE %sPublisherCod IN"
		  " (SELECT '%ld'"
		  " UNION"
		  " SELECT FollowedCod FROM usr_follow WHERE FollowerCod='%ld')"
		  " GROUP BY NotCod"
		  " ORDER BY PubCod DESC%s",
            SubQueryRangePubs,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    SubQueryLimit);
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create temporary table");

   /***** Update last publishing code into session for next refresh *****/
   // Do this inmediately after getting the publishings codes...
   // ...in order to not lose publishings
   Soc_UpdateLastPubCodIntoSession ();

   /***** Build query to show timeline including the users I am following *****/
   sprintf (Query,"SELECT PubCod,NotCod,PublisherCod,UNIX_TIMESTAMP(TimePublish)"
		  " FROM social_timeline WHERE PubCod IN "
		  "(SELECT PubCod FROM pub_cods)"
		  " ORDER BY PubCod DESC");
  }

/*****************************************************************************/
/********* Get last/first social publishing code stored in session ***********/
/*****************************************************************************/
// FieldName can be:
// "LastPubCod"
// "FirstPubCod"

static long Soc_GetPubCodFromSession (const char *FieldName)
  {
   char Query[128+Ses_LENGTH_SESSION_ID];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;

   /***** Get last publishing code from database *****/
   sprintf (Query,"SELECT %s FROM sessions WHERE SessionId='%s'",
            FieldName,Gbl.Session.Id);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get publishing code from session") != 1)
      Lay_ShowErrorAndExit ("Error when getting publishing code from session.");

   /***** Get last publishing code *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&PubCod) != 1)
      PubCod = 0;

   return PubCod;
  }

/*****************************************************************************/
/*********************** Update last publishing code *************************/
/*****************************************************************************/

static void Soc_UpdateLastPubCodIntoSession (void)
  {
   char Query[128+Ses_LENGTH_SESSION_ID];

   /***** Update last publishing code *****/
   sprintf (Query,"UPDATE sessions"
	          " SET LastPubCod=(SELECT MAX(PubCod) FROM social_timeline)"
	          " WHERE SessionId='%s'",
	    Gbl.Session.Id);
   DB_QueryUPDATE (Query,"can not update last publishing code into session");
  }

/*****************************************************************************/
/*********************** Update first publishing code ************************/
/*****************************************************************************/

static void Soc_UpdateFirstPubCodIntoSession (long FirstPubCod)
  {
   char Query[128+Ses_LENGTH_SESSION_ID];

   /***** Update last publishing code *****/
   sprintf (Query,"UPDATE sessions SET FirstPubCod='%ld' WHERE SessionId='%s'",
	    FirstPubCod,Gbl.Session.Id);
   DB_QueryUPDATE (Query,"can not update first publishing code into session");
  }

/*****************************************************************************/
/**************** Drop temporary table with publishing codes *****************/
/*****************************************************************************/

static void Soc_DropTemporaryTableWithPubCods (void)
  {
   char Query[128];

   sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS pub_cods");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");
  }

/*****************************************************************************/
/*********************** Show social activity (timeline) *********************/
/*****************************************************************************/

static void Soc_ShowTimeline (const char *Query,const char *Title)
  {
   extern const char *Txt_No_public_activity;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPubsToShow;
   unsigned long NumPub;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;

   /***** Get timeline from database *****/
   NumPubsGot = DB_QuerySELECT (Query,&mysql_res,"can not get timeline");

   /***** List my timeline *****/
   if (NumPubsGot)	// Publishings found in timeline
     {
      /***** Start frame *****/
      Lay_StartRoundFrame (Soc_WIDTH_TIMELINE,Title);

      /***** Form to write a new post *****/
      if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0 ||				// Global timeline
	  Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
         Soc_PutHiddenFormToWriteNewPost ();

      /***** Link to view new publishings via AJAX *****/
      Soc_PutLinkToViewNewPublishings ();

      /***** Hidden list where insert new publishings via AJAX *****/
      fprintf (Gbl.F.Out,"<ul id=\"new_timeline_list\"></ul>");

      /***** List recent publishings in timeline *****/
      fprintf (Gbl.F.Out,"<ul id=\"timeline_list\" class=\"LIST_LEFT\">");
      NumPubsToShow = (NumPubsGot <= Soc_MAX_RECENT_PUBS_TO_SHOW) ? NumPubsGot :
	                                                            Soc_MAX_RECENT_PUBS_TO_SHOW;
      for (NumPub = 0;
	   NumPub < NumPubsToShow;
	   NumPub++)
	{
         /* Get data of social publishing */
         row = mysql_fetch_row (mysql_res);
         Soc_GetDataOfSocialPublishingFromRow (row,&SocPub);

	 /* Get data of social note */
	 SocNot.NotCod = SocPub.NotCod;
	 Soc_GetDataOfSocialNoteByCod (&SocNot);

	 /* Write social note */
	 Soc_WriteSocialNote (&SocPub,&SocNot,false,true);
        }
      fprintf (Gbl.F.Out,"</ul>");

      /***** Store first publishing code into session *****/
      Soc_UpdateFirstPubCodIntoSession (SocPub.PubCod);

      if (NumPubsGot > Soc_MAX_RECENT_PUBS_TO_SHOW)
	{
         /***** Link to view old publishings via AJAX *****/
         Soc_PutLinkToViewOldPublishings ();

         /***** Hidden list where insert old publishings via AJAX *****/
         fprintf (Gbl.F.Out,"<ul id=\"old_timeline_list\"></ul>");
	}

      /***** End frame *****/
      Lay_EndRoundFrame ();
     }
   else	// No publishing found in timeline
      Lay_ShowAlert (Lay_INFO,Txt_No_public_activity);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Show new social activity (new publishings in timeline) ***********/
/*****************************************************************************/
// The publishings are inserted as list elements of a hidden list

static void Soc_ShowNewPubsInTimeline (const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPub;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;

   /***** Get new publishings timeline from database *****/
   NumPubsGot = DB_QuerySELECT (Query,&mysql_res,"can not get timeline");

   /***** List new publishings timeline *****/
   for (NumPub = 0;
	NumPub < NumPubsGot;
	NumPub++)
     {
      /* Get data of social publishing */
      row = mysql_fetch_row (mysql_res);
      Soc_GetDataOfSocialPublishingFromRow (row,&SocPub);

      /* Get data of social note */
      SocNot.NotCod = SocPub.NotCod;
      Soc_GetDataOfSocialNoteByCod (&SocNot);

      /* Write social note */
      Soc_WriteSocialNote (&SocPub,&SocNot,false,true);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Show old social activity (old publishings in timeline) ***********/
/*****************************************************************************/
// The publishings are inserted as list elements of a hidden list

static void Soc_ShowOldPubsInTimeline (const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPub;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;

   /***** Get old publishings timeline from database *****/
   NumPubsGot = DB_QuerySELECT (Query,&mysql_res,"can not get timeline");

   if (NumPubsGot)
     {
      /***** List old publishings in timeline *****/
      for (NumPub = 0;
	   NumPub < NumPubsGot;
	   NumPub++)
	{
	 /* Get data of social publishing */
	 row = mysql_fetch_row (mysql_res);
	 Soc_GetDataOfSocialPublishingFromRow (row,&SocPub);

	 /* Get data of social note */
	 SocNot.NotCod = SocPub.NotCod;
	 Soc_GetDataOfSocialNoteByCod (&SocNot);

	 /* Write social note */
	 Soc_WriteSocialNote (&SocPub,&SocNot,false,true);
	}

      /***** Store first publishing code into session *****/
      Soc_UpdateFirstPubCodIntoSession (SocPub.PubCod);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Put link to view new publishings in timeline **************/
/*****************************************************************************/

static void Soc_PutLinkToViewNewPublishings (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_See_new_activity;

   /***** Link to view (show hidden) new publishings *****/
   // div is hidden. When new posts arrive to the client via AJAX, div is shown
   fprintf (Gbl.F.Out,"<div id=\"view_new_posts_container\""
	              " class=\"SOCIAL_PUB VERY_LIGHT_BLUE\""
	              " style=\"display:none;\">"
                      "<a href=\"\" class=\"%s\""
                      " onclick=\"moveNewTimelineToTimeline();return false;\" />"
                      "%s (<span id=\"view_new_posts_count\">0</span>)"
	              "</a>"
	              "</div>",
	    The_ClassFormBold[Gbl.Prefs.Theme],
	    Txt_See_new_activity);
  }

/*****************************************************************************/
/***************** Put link to view old publishings in timeline **************/
/*****************************************************************************/

static void Soc_PutLinkToViewOldPublishings (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_See_more;

   /***** Link to view old publishings *****/
   fprintf (Gbl.F.Out,"<div id=\"view_old_posts_container\""
	              " class=\"SOCIAL_PUB VERY_LIGHT_BLUE\">"
                      "<a href=\"\" class=\"%s\""
                      " onclick=\"refreshOldTimeline();return false;\" />"
                      "%s"
	              "</a>"
	              "</div>",
	    The_ClassFormBold[Gbl.Prefs.Theme],
	    Txt_See_more);
  }

/*****************************************************************************/
/***************************** Write social note *****************************/
/*****************************************************************************/

static void Soc_WriteSocialNote (const struct SocialPublishing *SocPub,
                                 const struct SocialNote *SocNot,
                                 bool ShowNoteAlone,	// Social note is shown alone, not in a list
                                 bool ViewTopLine)	// Separate with a top line from previous social note
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData UsrDat;
   bool IAmTheAuthor = false;
   bool IAmAPublisherOfThisSocNot = false;
   struct Institution Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char ForumName[512];
   char SummaryStr[Cns_MAX_BYTES_TEXT+1];
   unsigned NumComments;

   if (ShowNoteAlone)
     {
      Lay_StartRoundFrame (Soc_WIDTH_TIMELINE,NULL);
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");
     }

   /***** Start list item *****/
   fprintf (Gbl.F.Out,"<li");
   if (!ShowNoteAlone && ViewTopLine)
      fprintf (Gbl.F.Out," class=\"SOCIAL_PUB\"");
   fprintf (Gbl.F.Out,">");

   if (SocPub->PubCod       <= 0 ||
       SocPub->NotCod       <= 0 ||
       SocPub->PublisherCod <= 0 ||
       SocNot->NoteType     == Soc_NOTE_UNKNOWN ||
       SocNot->UsrCod       <= 0)
      Lay_ShowAlert (Lay_ERROR,"Error in social note.");
   else
     {
      /***** Initialize location in hierarchy *****/
      Ins.InsCod = -1L;
      Ctr.CtrCod = -1L;
      Deg.DegCod = -1L;
      Crs.CrsCod = -1L;

      /***** Get author data *****/
      Usr_UsrDataConstructor (&UsrDat);
      UsrDat.UsrCod = SocNot->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      if (Gbl.Usrs.Me.Logged)
	{
	 IAmTheAuthor = (UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
	 IAmAPublisherOfThisSocNot = Soc_CheckIfNoteIsPublishedInTimelineByUsr (SocNot->NotCod,
										Gbl.Usrs.Me.UsrDat.UsrCod);
	}

      /***** Left: write author's photo *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_LEFT_PHOTO\">");
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO45x60",Pho_ZOOM);
      fprintf (Gbl.F.Out,"</div>");

      /***** Right: author's name, time, summary and buttons *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_CONTAINER\">");

      /* Write author's full name and nickname */
      Str_LimitLengthHTMLStr (UsrDat.FullName,16);
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_AUTHOR\">"
			 "<span class=\"DAT_N_BOLD\">%s</span>"
			 "<span class=\"DAT_LIGHT\"> @%s</span>"
			 "</div>",
	       UsrDat.FullName,UsrDat.Nickname);

      /* Write date and time */
      Soc_WriteDateTime (SocNot->DateTimeUTC);

      /* Write content of the note */
      if (SocNot->NoteType == Soc_NOTE_SOCIAL_POST)
	{
	 /* Write post content */
	 fprintf (Gbl.F.Out,"<div class=\"DAT\">");
	 Soc_GetAndWriteSocialPost (SocNot->Cod);
	 fprintf (Gbl.F.Out,"</div>");
	}
      else
	{
	 /* Get location in hierarchy */
	 if (!SocNot->Unavailable)
	    switch (SocNot->NoteType)
	      {
	       case Soc_NOTE_INS_DOC_PUB_FILE:
	       case Soc_NOTE_INS_SHA_PUB_FILE:
		  /* Get institution data */
		  Ins.InsCod = SocNot->HieCod;
		  Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
		  break;
	       case Soc_NOTE_CTR_DOC_PUB_FILE:
	       case Soc_NOTE_CTR_SHA_PUB_FILE:
		  /* Get centre data */
		  Ctr.CtrCod = SocNot->HieCod;
		  Ctr_GetDataOfCentreByCod (&Ctr);
		  break;
	       case Soc_NOTE_DEG_DOC_PUB_FILE:
	       case Soc_NOTE_DEG_SHA_PUB_FILE:
		  /* Get degree data */
		  Deg.DegCod = SocNot->HieCod;
		  Deg_GetDataOfDegreeByCod (&Deg);
		  break;
	       case Soc_NOTE_CRS_DOC_PUB_FILE:
	       case Soc_NOTE_CRS_SHA_PUB_FILE:
	       case Soc_NOTE_EXAM_ANNOUNCEMENT:
	       case Soc_NOTE_NOTICE:
		  /* Get course data */
		  Crs.CrsCod = SocNot->HieCod;
		  Crs_GetDataOfCourseByCod (&Crs);
		  break;
	       case Soc_NOTE_FORUM_POST:
		  /* Get forum type of the post */
		  Gbl.Forum.ForumType = For_GetForumTypeOfAPost (SocNot->Cod);
		  For_SetForumName (Gbl.Forum.ForumType,
				    &Ins,
				    &Ctr,
				    &Deg,
				    &Crs,
				    ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
		  Gbl.Forum.Ins.InsCod = Ins.InsCod;
		  Gbl.Forum.Ctr.CtrCod = Ctr.CtrCod;
		  Gbl.Forum.Deg.DegCod = Deg.DegCod;
		  Gbl.Forum.Crs.CrsCod = Crs.CrsCod;
		  break;
	       default:
		  break;
	      }

	 /* Write note type */
	 fprintf (Gbl.F.Out,"<div>");
	 Soc_PutFormGoToAction (SocNot);
	 fprintf (Gbl.F.Out,"</div>");

	 /* Write location in hierarchy */
	 if (!SocNot->Unavailable)
	    switch (SocNot->NoteType)
	      {
	       case Soc_NOTE_INS_DOC_PUB_FILE:
	       case Soc_NOTE_INS_SHA_PUB_FILE:
		  /* Write location (institution) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Institution,Ins.ShortName);
		  break;
	       case Soc_NOTE_CTR_DOC_PUB_FILE:
	       case Soc_NOTE_CTR_SHA_PUB_FILE:
		  /* Write location (centre) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Centre,Ctr.ShortName);
		  break;
	       case Soc_NOTE_DEG_DOC_PUB_FILE:
	       case Soc_NOTE_DEG_SHA_PUB_FILE:
		  /* Write location (degree) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Degree,Deg.ShortName);
		  break;
	       case Soc_NOTE_CRS_DOC_PUB_FILE:
	       case Soc_NOTE_CRS_SHA_PUB_FILE:
	       case Soc_NOTE_EXAM_ANNOUNCEMENT:
	       case Soc_NOTE_NOTICE:
		  /* Write location (course) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Course,Crs.ShortName);
		  break;
	       case Soc_NOTE_FORUM_POST:
		  /* Write forum name */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Forum,ForumName);
		  break;
	       default:
		  break;
	      }

	 /* Write note summary */
	 Soc_GetNoteSummary (SocNot,SummaryStr,Soc_MAX_BYTES_SUMMARY);
	 fprintf (Gbl.F.Out,"<div class=\"DAT\">%s</div>",SummaryStr);
	}

      /* Get number of comments in this social note */
      NumComments = Soc_GetNumCommentsInSocialNote (SocNot->NotCod);

      /* Put icon to add a comment */
      if (!NumComments)
         Soc_PutIconToToggleCommentSocialNote (SocPub->PubCod,false,ShowNoteAlone);

      /* Put icons to share/unshare */
      if (IAmTheAuthor)			// I am the author
	 Soc_PutDisabledIconShare (SocNot->NumShared);
      else if (IAmAPublisherOfThisSocNot)	// I am a publisher of this social note,
					   // but not the author ==> I have shared this social note
	 /* Put icon to unshare this publishing */
	 Soc_PutFormToUnshareSocialPublishing (SocPub->PubCod);
      else					// I am not the author and I am not a publisher
	{
	 if (SocNot->Unavailable)		// Unavailable social notes can not be shared
	    Soc_PutDisabledIconShare (SocNot->NumShared);
	 else
	    /* Put icon to share this publishing */
	    Soc_PutFormToShareSocialNote (SocNot->NotCod);
	}

      /* Show who have shared this social note */
      Soc_ShowUsrsWhoHaveSharedSocialNote (SocNot);

      /* Put icon to remove this publishing */
      if (IAmTheAuthor && !ShowNoteAlone)
	 Soc_PutFormToRemoveSocialPublishing (SocPub->PubCod);

      /* Show comments */
      Soc_WriteCommentsInSocialNote (SocPub->PubCod,SocNot->NotCod,
                                     NumComments,ShowNoteAlone);

      /* End of right part */
      fprintf (Gbl.F.Out,"</div>");

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** End list item *****/
   fprintf (Gbl.F.Out,"</li>");

   if (ShowNoteAlone)
     {
      fprintf (Gbl.F.Out,"</ul>");
      Lay_EndRoundFrame ();
     }
  }

/*****************************************************************************/
/**************** Write the date of creation of a social note ****************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

static void Soc_WriteDateTime (time_t TimeUTC)
  {
   extern const char *Txt_Today;
   static unsigned UniqueId = 0;
   char Id[32+Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+10+1];

   UniqueId++;

   /***** Create Id. The id must be unique in timeline,
          but the timeline is updated via AJAX.
          So, Id uses:
          - a name for this execution (Gbl.UniqueNameEncrypted)
          - an extension for the current element (UniqueId) *****/
   sprintf (Id,"date_%s_%u",Gbl.UniqueNameEncrypted,UniqueId);

   /***** Container where the date-time is written *****/
   fprintf (Gbl.F.Out,"<div id=\"%s\" class=\"SOCIAL_RIGHT_TIME DAT_LIGHT\""
	              " style=\"display:inline-block;\"></div>",
            Id);

   /***** Script to write date and time in browser local time *****/
   // This must be out of the div where the output is written
   // because it will be evaluated in a loop in JavaScript
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateTimeFromUTC('%s',%ld,'&nbsp;','%s');"
                      "</script>",
            Id,(long) TimeUTC,Txt_Today);
  }

/*****************************************************************************/
/***************** Get from database and write public post *******************/
/*****************************************************************************/

static void Soc_GetAndWriteSocialPost (long PstCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Get social post from database *****/
   sprintf (Query,"SELECT Content FROM social_posts WHERE PstCod='%ld'",
            PstCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the content of a social post");

   /***** Result should have a unique row *****/
   if (NumRows == 1)
     {
      /***** Get number of rows *****/
      row = mysql_fetch_row (mysql_res);

      /****** Get content (row[0]) *****/
      strncpy (Content,row[0],Cns_MAX_BYTES_LONG_TEXT);
      Content[Cns_MAX_BYTES_LONG_TEXT] = '\0';
     }
   else
      Content[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Write content *****/
   Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
  }

/*****************************************************************************/
/********* Put form to go to an action depending on the social note **********/
/*****************************************************************************/

static void Soc_PutFormGoToAction (const struct SocialNote *SocNot)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_SOCIAL_NOTE[Soc_NUM_SOCIAL_NOTES];
   extern const char *Txt_not_available;
   char Class[64];

   if (SocNot->Unavailable ||	// File/notice... pointer by this social note is unavailable
       Gbl.InsideForm)		// Inside another form
     {
      /***** Do not put form *****/
      fprintf (Gbl.F.Out,"<span class=\"DAT_LIGHT\">%s",
               Txt_SOCIAL_NOTE[SocNot->NoteType]);
      if (SocNot->Unavailable)
         fprintf (Gbl.F.Out," (%s)",Txt_not_available);
      fprintf (Gbl.F.Out,"</span>");
     }
   else			// Not inside another form
     {
      /***** Parameters depending on the type of note *****/
      switch (SocNot->NoteType)
	{
	 case Soc_NOTE_INS_DOC_PUB_FILE:
	 case Soc_NOTE_INS_SHA_PUB_FILE:
	    Act_FormStart (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentIns.Ins.InsCod)	// Not the current institution
	       Ins_PutParamInsCod (SocNot->HieCod);		// Go to another institution
	    break;
	 case Soc_NOTE_CTR_DOC_PUB_FILE:
	 case Soc_NOTE_CTR_SHA_PUB_FILE:
	    Act_FormStart (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCtr.Ctr.CtrCod)	// Not the current centre
	       Ctr_PutParamCtrCod (SocNot->HieCod);		// Go to another centre
	    break;
	 case Soc_NOTE_DEG_DOC_PUB_FILE:
	 case Soc_NOTE_DEG_SHA_PUB_FILE:
	    Act_FormStart (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentDeg.Deg.DegCod)	// Not the current degree
	       Deg_PutParamDegCod (SocNot->HieCod);		// Go to another degree
	    break;
	 case Soc_NOTE_CRS_DOC_PUB_FILE:
	 case Soc_NOTE_CRS_SHA_PUB_FILE:
	    Act_FormStart (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_EXAM_ANNOUNCEMENT:
	    Act_FormStart (Soc_DefaultActions[SocNot->NoteType]);
	    Not_PutHiddenParamNotCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_SOCIAL_POST:	// Not applicable
	    return;
	 case Soc_NOTE_FORUM_POST:
	    Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
	    For_PutAllHiddenParamsForum ();
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_NOTICE:
	    Act_FormStart (Soc_DefaultActions[SocNot->NoteType]);
	    Not_PutHiddenParamNotCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 default:			// Not applicable
	    return;
	}

      /***** Link and end form *****/
      sprintf (Class,"%s ICON_HIGHLIGHT",The_ClassFormBold[Gbl.Prefs.Theme]);
      Act_LinkFormSubmit (Txt_SOCIAL_NOTE[SocNot->NoteType],
			  Class);
      fprintf (Gbl.F.Out,"<img src=\"%s/%s\""
	                 " alt=\"%s\" title=\"%s\""
	                 " class=\"ICON20x20\" />"
	                 "&nbsp;%s"
	                 "</a>",
            Gbl.Prefs.IconsURL,Soc_Icons[SocNot->NoteType],
            Txt_SOCIAL_NOTE[SocNot->NoteType],
            Txt_SOCIAL_NOTE[SocNot->NoteType],
            Txt_SOCIAL_NOTE[SocNot->NoteType]);
      Act_FormEnd ();
     }
  }

/*****************************************************************************/
/******************* Get social note summary and content *********************/
/*****************************************************************************/

static void Soc_GetNoteSummary (const struct SocialNote *SocNot,
                                char *SummaryStr,unsigned MaxChars)
  {
   SummaryStr[0] = '\0';

   switch (SocNot->NoteType)
     {
      case Soc_NOTE_UNKNOWN:
          break;
      case Soc_NOTE_INS_DOC_PUB_FILE:
      case Soc_NOTE_INS_SHA_PUB_FILE:
      case Soc_NOTE_CTR_DOC_PUB_FILE:
      case Soc_NOTE_CTR_SHA_PUB_FILE:
      case Soc_NOTE_DEG_DOC_PUB_FILE:
      case Soc_NOTE_DEG_SHA_PUB_FILE:
      case Soc_NOTE_CRS_DOC_PUB_FILE:
      case Soc_NOTE_CRS_SHA_PUB_FILE:
	 Brw_GetSummaryAndContentOrSharedFile (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
      case Soc_NOTE_EXAM_ANNOUNCEMENT:
         Exa_GetSummaryAndContentExamAnnouncement (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
      case Soc_NOTE_SOCIAL_POST:
	 // Not applicable
         break;
      case Soc_NOTE_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
      case Soc_NOTE_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,SocNot->Cod,MaxChars,false);
         break;
     }
  }

/*****************************************************************************/
/************** Store and publish a social note into database ****************/
/*****************************************************************************/

void Soc_StoreAndPublishSocialNote (Soc_NoteType_t NoteType,long Cod)
  {
   char Query[256];
   long HieCod;	// Hierarchy code (institution/centre/degree/course)
   struct SocialPublishing SocPub;

   switch (NoteType)
     {
      case Soc_NOTE_INS_DOC_PUB_FILE:
      case Soc_NOTE_INS_SHA_PUB_FILE:
	 HieCod = Gbl.CurrentIns.Ins.InsCod;
	 break;
      case Soc_NOTE_CTR_DOC_PUB_FILE:
      case Soc_NOTE_CTR_SHA_PUB_FILE:
	 HieCod = Gbl.CurrentCtr.Ctr.CtrCod;
	 break;
      case Soc_NOTE_DEG_DOC_PUB_FILE:
      case Soc_NOTE_DEG_SHA_PUB_FILE:
	 HieCod = Gbl.CurrentDeg.Deg.DegCod;
	 break;
      case Soc_NOTE_CRS_DOC_PUB_FILE:
      case Soc_NOTE_CRS_SHA_PUB_FILE:
      case Soc_NOTE_EXAM_ANNOUNCEMENT:
      case Soc_NOTE_NOTICE:
	 HieCod = Gbl.CurrentCrs.Crs.CrsCod;
	 break;
      default:
	 HieCod = -1L;
         break;
     }

   /***** Store social note *****/
   sprintf (Query,"INSERT INTO social_notes"
	          " (NoteType,UsrCod,HieCod,Cod,Unavailable,TimeNote)"
                  " VALUES ('%u','%ld','%ld','%ld','N',NOW())",
            (unsigned) NoteType,Gbl.Usrs.Me.UsrDat.UsrCod,HieCod,Cod);
   SocPub.NotCod = DB_QueryINSERTandReturnCode (Query,"can not create new social note");

   /***** Publish social note in timeline *****/
   SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Soc_PublishSocialNoteInTimeline (&SocPub);
  }

/*****************************************************************************/
/********************** Mark a social note as unavailable ********************/
/*****************************************************************************/

void Soc_MarkSocialNoteAsUnavailableUsingNotCod (long NotCod)
  {
   char Query[256];

   /***** Mark the social note as unavailable *****/
   sprintf (Query,"UPDATE social_notes SET Unavailable='Y'"
		  " WHERE NotCod='%ld'",
	    NotCod);
   DB_QueryUPDATE (Query,"can not mark social note as unavailable");
  }

void Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (Soc_NoteType_t NoteType,long Cod)
  {
   char Query[256];

   /***** Mark the social note as unavailable *****/
   sprintf (Query,"UPDATE social_notes SET Unavailable='Y'"
		  " WHERE NoteType='%u' AND Cod='%ld'",
	    (unsigned) NoteType,Cod);
   DB_QueryUPDATE (Query,"can not mark social note as unavailable");
  }

/*****************************************************************************/
/*********** Mark possible notifications of one file as removed **************/
/*****************************************************************************/

void Soc_MarkSocialNoteOneFileAsRemoved (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;
   Soc_NoteType_t NoteType;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOCUM_INS:
      case Brw_ADMI_SHARE_INS:
      case Brw_ADMI_DOCUM_CTR:
      case Brw_ADMI_SHARE_CTR:
      case Brw_ADMI_DOCUM_DEG:
      case Brw_ADMI_SHARE_DEG:
      case Brw_ADMI_DOCUM_CRS:
      case Brw_ADMI_SHARE_CRS:
         /***** Get file code *****/
	 FilCod = Brw_GetFilCodByPath (Path,true);	// Only if file is public
	 if (FilCod > 0)
	   {
	    /***** Mark possible social note as unavailable *****/
	    switch (FileBrowser)
	      {
	       case Brw_ADMI_DOCUM_INS:
		  NoteType = Soc_NOTE_INS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHARE_INS:
		  NoteType = Soc_NOTE_INS_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOCUM_CTR:
		  NoteType = Soc_NOTE_CTR_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHARE_CTR:
		  NoteType = Soc_NOTE_CTR_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOCUM_DEG:
		  NoteType = Soc_NOTE_DEG_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHARE_DEG:
		  NoteType = Soc_NOTE_DEG_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOCUM_CRS:
		  NoteType = Soc_NOTE_CRS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHARE_CRS:
		  NoteType = Soc_NOTE_CRS_SHA_PUB_FILE;
		  break;
	       default:
		  return;
	      }
	    Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (NoteType,FilCod);
	   }
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/** Mark possible social notes involving children of a folder as unavailable */
/*****************************************************************************/

void Soc_MarkSocialNotesChildrenOfFolderAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long Cod = Brw_GetCodForFiles ();
   char Query[512];
   Soc_NoteType_t NoteType;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOCUM_INS:
      case Brw_ADMI_SHARE_INS:
      case Brw_ADMI_DOCUM_CTR:
      case Brw_ADMI_SHARE_CTR:
      case Brw_ADMI_DOCUM_DEG:
      case Brw_ADMI_SHARE_DEG:
      case Brw_ADMI_DOCUM_CRS:
      case Brw_ADMI_SHARE_CRS:
	 /***** Mark possible social note as unavailable *****/
	 switch (FileBrowser)
	   {
	    case Brw_ADMI_DOCUM_INS:
	       NoteType = Soc_NOTE_INS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHARE_INS:
	       NoteType = Soc_NOTE_INS_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOCUM_CTR:
	       NoteType = Soc_NOTE_CTR_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHARE_CTR:
	       NoteType = Soc_NOTE_CTR_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOCUM_DEG:
	       NoteType = Soc_NOTE_DEG_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHARE_DEG:
	       NoteType = Soc_NOTE_DEG_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOCUM_CRS:
	       NoteType = Soc_NOTE_CRS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHARE_CRS:
	       NoteType = Soc_NOTE_CRS_SHA_PUB_FILE;
	       break;
	    default:
	       return;
	   }
         sprintf (Query,"UPDATE social_notes SET Unavailable='Y'"
		        " WHERE NoteType='%u' AND Cod IN"
	                " (SELECT FilCod FROM files"
			" WHERE FileBrowser='%u' AND Cod='%ld'"
			" AND Path LIKE '%s/%%' AND Public='Y')",	// Only public files
	          (unsigned) NoteType,
	          (unsigned) FileBrowser,Cod,
	          Path);
         DB_QueryUPDATE (Query,"can not mark social notes as unavailable");
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/***************** Put contextual link to write a new post *******************/
/*****************************************************************************/
// SocPub->PubCod is set

static void Soc_PublishSocialNoteInTimeline (struct SocialPublishing *SocPub)
  {
   char Query[256];

   /***** Publish social note in timeline *****/
   sprintf (Query,"INSERT INTO social_timeline"
	          " (NotCod,PublisherCod,TimePublish)"
                  " VALUES"
                  " ('%ld','%ld',NOW())",
            SocPub->NotCod,SocPub->PublisherCod);
   SocPub->PubCod = DB_QueryINSERTandReturnCode (Query,"can not publish social note");
  }

/*****************************************************************************/
/****************** Form to write a new social publishing ********************/
/*****************************************************************************/

static void Soc_PutHiddenFormToWriteNewPost (void)
  {
   extern const char *Txt_Post;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];
   char FullName[(Usr_MAX_BYTES_NAME+1)*3];

   /***** Start list *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">"
                      "<li>");

   /***** Left: write author's photo *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_LEFT_PHOTO\">");
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
						     NULL,
		     "PHOTO45x60",Pho_ZOOM);
   fprintf (Gbl.F.Out,"</div>");

   /***** Right: author's name, time, summary and buttons *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_CONTAINER\">");

   /* Write author's full name and nickname */
   strcpy (FullName,Gbl.Usrs.Me.UsrDat.FullName);
   Str_LimitLengthHTMLStr (FullName,16);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_AUTHOR\">"
		      "<span class=\"DAT_N_BOLD\">%s</span>"
		      "<span class=\"DAT_LIGHT\"> @%s</span>"
		      "</div>",
	    FullName,Gbl.Usrs.Me.UsrDat.Nickname);

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_FORM_COMMENT\">");

   /* Start form to write the post */
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartAnchor (ActRcvSocPstUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActRcvSocPstGbl);

   /* Content of new post */
   fprintf (Gbl.F.Out,"<textarea name=\"Content\" cols=\"45\" rows=\"3\">"
		      "</textarea>");

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_CREATE\">"
		      "%s"
		      "</button>",
	    Txt_Post);

   /***** End form *****/
   Act_FormEnd ();

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");

   /***** End list *****/
   fprintf (Gbl.F.Out,"</li>"
	              "</ul>");
  }

/*****************************************************************************/
/******************* Receive and store a new public post *********************/
/*****************************************************************************/

void Soc_ReceiveSocialPostGbl (void)
  {
   /***** Receive and store social post *****/
   Soc_ReceiveSocialPost ();

   /***** Write updated timeline after publishing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_ReceiveSocialPostUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Receive and store social post *****/
   Soc_ReceiveSocialPost ();

   /***** Write updated timeline after publishing (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_ReceiveSocialPost (void)
  {
   extern const char *Txt_SOCIAL_PUBLISHING_Published;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   char Query[128+Cns_MAX_BYTES_LONG_TEXT];
   long PstCod;

   /***** Get and store new post *****/
   /* Get the content of the post */
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   if (Content[0])
     {
      /***** Publish *****/
      /* Insert post content in the database */
      sprintf (Query,"INSERT INTO social_posts (Content) VALUES ('%s')",
	       Content);
      PstCod = DB_QueryINSERTandReturnCode (Query,"can not create post");

      /* Insert post in social notes */
      Soc_StoreAndPublishSocialNote (Soc_NOTE_SOCIAL_POST,PstCod);

      /***** Message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_SOCIAL_PUBLISHING_Published);
     }
  }

/*****************************************************************************/
/****** Put an icon to toggle on/off the form to comment a social note *******/
/*****************************************************************************/

static void Soc_PutIconToToggleCommentSocialNote (long PubCod,bool PutText,bool ShowNoteAlone)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Comment;

   /***** Link to toggle on/off the form to comment a social note *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_COMMENT ICON_HIGHLIGHT\">"
                      "<a href=\"\"");
   if (PutText)
      fprintf (Gbl.F.Out," class=\"%s\"",The_ClassForm[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out," onclick=\"toggleDisplay('div_comment_%ld');return false;\" />"
		      "<img src=\"%s/write64x64.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />",
	    ShowNoteAlone ? 0 :
		            PubCod,
	    Gbl.Prefs.IconsURL,
	    Txt_Comment,
	    Txt_Comment);
   if (PutText)
      fprintf (Gbl.F.Out,"&nbsp;%s",Txt_Comment);
   fprintf (Gbl.F.Out,"</a>"
		      "</div>");
  }

/*****************************************************************************/
/******************* Form to comment a social publishing *********************/
/*****************************************************************************/

static void Soc_PutHiddenFormToWriteNewCommentToSocialNote (long PubCod,bool ShowNoteAlone)
  {
   extern const char *Txt_Send_comment;

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div id=\"div_comment_%ld\""
		      " class=\"SOCIAL_FORM_COMMENT\""
		      " style=\"display:none;\">",
	    ShowNoteAlone ? 0 :
		            PubCod);

   /***** Start form to write the post *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartAnchor (ActRcvSocComUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActRcvSocComGbl);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<textarea name=\"Comment\" cols=\"45\" rows=\"3\">"
		      "</textarea>");

   /***** Send button *****/
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_CREATE\">"
		      "%s"
		      "</button>",
	    Txt_Send_comment);

   /***** End form *****/
   Act_FormEnd ();

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Get number of comments in a social note ******************/
/*****************************************************************************/

static unsigned long Soc_GetNumCommentsInSocialNote (long NotCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM social_comments"
	          " WHERE NotCod='%ld'",
	    NotCod);
   return DB_QueryCOUNT (Query,"can not get number of comments in a social note");
  }

/*****************************************************************************/
/******************* Form to comment a social publishing *********************/
/*****************************************************************************/

static void Soc_WriteCommentsInSocialNote (long PubCod,long NotCod,
                                           unsigned long NumComments,bool ShowNoteAlone)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumCom;
   struct SocialComment SocCom;

   if (NumComments)
     {
      /***** Get comments of this social note from database *****/
      sprintf (Query,"SELECT social_comments.ComCod,social_comments.UsrCod,"
		     "social_comments.NotCod,"
		     "UNIX_TIMESTAMP(social_comments.TimeComment),"
		     "social_comments_content.Content"
		     " FROM social_comments,social_comments_content"
		     " WHERE social_comments.NotCod='%ld'"
		     " AND social_comments.ComCod=social_comments_content.ComCod"
		     " ORDER BY social_comments.ComCod",
	       NotCod);
      NumComments = DB_QuerySELECT (Query,&mysql_res,"can not get social comments");

      /***** List comments *****/
      if (NumComments)	// Comments to this social note found
	{
	 /***** Start list *****/
	 fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT SOCIAL_COMMENTS\">");

	 /***** List comments one by one *****/
	 for (NumCom = 0;
	      NumCom < NumComments;
	      NumCom++)
	   {
	    /* Get data of social comment */
	    row = mysql_fetch_row (mysql_res);
	    Soc_GetDataOfSocialCommentFromRow (row,&SocCom);

	    /* Write social comment */
	    Soc_WriteSocialComment (&SocCom,false);
	   }

	 /* Put icon to add a comment */
	 fprintf (Gbl.F.Out,"<li class=\"SOCIAL_COMMENT\">");
	 Soc_PutIconToToggleCommentSocialNote (PubCod,true,ShowNoteAlone);
	 fprintf (Gbl.F.Out,"</li>");

	 /***** End list *****/
	 fprintf (Gbl.F.Out,"</ul>");
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** Put hidden form to write a new comment *****/
   Soc_PutHiddenFormToWriteNewCommentToSocialNote (PubCod,ShowNoteAlone);
  }

/*****************************************************************************/
/**************************** Write social comment ***************************/
/*****************************************************************************/

static void Soc_WriteSocialComment (struct SocialComment *SocCom,
                                    bool ShowCommentAlone)	// Social comment is shown alone, not in a list
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData UsrDat;
   bool IAmTheAuthor;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];

   if (ShowCommentAlone)
     {
      Lay_StartRoundFrame (Soc_WIDTH_TIMELINE,NULL);
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_LEFT_PHOTO\">"
                         "</div>"
                         "<div class=\"SOCIAL_RIGHT_CONTAINER\">"
                         "<ul class=\"LIST_LEFT\">");
     }

   /***** Start list item *****/
   fprintf (Gbl.F.Out,"<li");
   if (!ShowCommentAlone)
      fprintf (Gbl.F.Out," class=\"SOCIAL_COMMENT\"");
   fprintf (Gbl.F.Out,">");

   if (SocCom->ComCod <= 0 ||
       SocCom->NotCod <= 0 ||
       SocCom->UsrCod <= 0)
      Lay_ShowAlert (Lay_ERROR,"Error in social comment.");
   else
     {
      /***** Get author's data *****/
      Usr_UsrDataConstructor (&UsrDat);
      UsrDat.UsrCod = SocCom->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      IAmTheAuthor = (Gbl.Usrs.Me.Logged &&
	              UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);

      /***** Left: write author's photo *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_PHOTO\">");
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO30x40",Pho_ZOOM);
      fprintf (Gbl.F.Out,"</div>");

      /***** Right: author's name, time, summary and buttons *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_RIGHT_CONTAINER\">");

      /* Write author's full name and nickname */
      Str_LimitLengthHTMLStr (UsrDat.FullName,12);
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_RIGHT_AUTHOR\">"
			 "<span class=\"DAT_BOLD\">%s</span>"
			 "<span class=\"DAT_LIGHT\"> @%s</span>"
			 "</div>",
	       UsrDat.FullName,UsrDat.Nickname);

      /* Write date and time */
      Soc_WriteDateTime (SocCom->DateTimeUTC);

      /* Write content of the social comment */
      fprintf (Gbl.F.Out,"<div class=\"DAT\">");
      Msg_WriteMsgContent (SocCom->Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
      fprintf (Gbl.F.Out,"</div>");

      /* Put icon to remove this social comment */
      if (IAmTheAuthor && !ShowCommentAlone)
	 Soc_PutFormToRemoveComment (SocCom->ComCod);

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** End list item *****/
   fprintf (Gbl.F.Out,"</li>");

   if (ShowCommentAlone)
     {
      fprintf (Gbl.F.Out,"</ul>"
                         "</div>");
      Lay_EndRoundFrame ();
     }
  }

/*****************************************************************************/
/********************** Form to remove social comment ************************/
/*****************************************************************************/

static void Soc_PutFormToRemoveComment (long ComCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartAnchor (ActReqRemSocComUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActReqRemSocComGbl);
   Soc_PutHiddenParamComCod (ComCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_REMOVE ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/remove-on64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*********************** Put disabled icon to share **************************/
/*****************************************************************************/

static void Soc_PutDisabledIconShare (unsigned NumShared)
  {
   extern const char *Txt_SOCIAL_PUBLISHING_Shared_by_X_USERS;
   extern const char *Txt_SOCIAL_PUBLISHING_Not_shared_by_anyone;

   if (NumShared)
      sprintf (Gbl.Title,Txt_SOCIAL_PUBLISHING_Shared_by_X_USERS,NumShared);
   else
      strcpy (Gbl.Title,Txt_SOCIAL_PUBLISHING_Not_shared_by_anyone);

   /***** Disabled icon to share *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE_DISABLED\">"
		      "<img src=\"%s/share64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Gbl.Title,Gbl.Title);
  }

/*****************************************************************************/
/************************* Form to share social note *************************/
/*****************************************************************************/

static void Soc_PutFormToShareSocialNote (long NotCod)
  {
   extern const char *Txt_Share;

   /***** Form to share social note *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartAnchor (ActShaSocNotUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActShaSocNotGbl);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/share64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Share,
	    Txt_Share);
   Act_FormEnd ();
  }

/*****************************************************************************/
/************ Form to unshare (stop sharing) social publishing ***************/
/*****************************************************************************/

static void Soc_PutFormToUnshareSocialPublishing (long PubCod)
  {
   extern const char *Txt_Unshare;

   /***** Form to share social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartAnchor (ActUnsSocPubUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActUnsSocPubGbl);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/shared64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Unshare,Txt_Unshare);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Form to remove social publishing ***********************/
/*****************************************************************************/

static void Soc_PutFormToRemoveSocialPublishing (long PubCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartAnchor (ActReqRemSocPubUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStart (ActReqRemSocPubGbl);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_REMOVE ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/remove-on64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
   Act_FormEnd ();
  }

/*****************************************************************************/
/************** Put parameter with the code of a social note *****************/
/*****************************************************************************/

static void Soc_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong ("NotCod",NotCod);
  }

/*****************************************************************************/
/*********** Put parameter with the code of a social publishing **************/
/*****************************************************************************/

static void Soc_PutHiddenParamPubCod (long PubCod)
  {
   Par_PutHiddenParamLong ("PubCod",PubCod);
  }

/*****************************************************************************/
/************* Put parameter with the code of a social comment ***************/
/*****************************************************************************/

static void Soc_PutHiddenParamComCod (long ComCod)
  {
   Par_PutHiddenParamLong ("ComCod",ComCod);
  }

/*****************************************************************************/
/************** Get parameter with the code of a social note *****************/
/*****************************************************************************/

static long Soc_GetParamNotCod (void)
  {
   char LongStr[1+10+1];	// String that holds the social note code
   long NotCod;

   /* Get social note code */
   Par_GetParToText ("NotCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&NotCod) != 1)
      Lay_ShowErrorAndExit ("Wrong code of social note.");

   return NotCod;
  }

/*****************************************************************************/
/*********** Get parameter with the code of a social publishing **************/
/*****************************************************************************/

static long Soc_GetParamPubCod (void)
  {
   char LongStr[1+10+1];	// String that holds the social publishing code
   long PubCod;

   /* Get social punlishing code */
   Par_GetParToText ("PubCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&PubCod) != 1)
      Lay_ShowErrorAndExit ("Wrong code of social publishing.");

   return PubCod;
  }

/*****************************************************************************/
/************* Get parameter with the code of a social comment ***************/
/*****************************************************************************/

static long Soc_GetParamComCod (void)
  {
   char LongStr[1+10+1];	// String that holds the social comment code
   long ComCod;

   /* Get social comment code */
   Par_GetParToText ("ComCod",LongStr,1+10);
   if (sscanf (LongStr,"%ld",&ComCod) != 1)
      Lay_ShowErrorAndExit ("Wrong code of social comment.");

   return ComCod;
  }

/*****************************************************************************/
/*************************** Comment a social note ***************************/
/*****************************************************************************/

void Soc_ReceiveCommentGbl (void)
  {
   /***** Receive comment in a social note *****/
   Soc_ReceiveComment ();

   /***** Write updated timeline after commenting (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_ReceiveCommentUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Receive comment in a social note *****/
   Soc_ReceiveComment ();

   /***** Write updated timeline after commenting (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_ReceiveComment (void)
  {
   extern const char *Txt_SOCIAL_PUBLISHING_Published;
   extern const char *Txt_The_original_post_no_longer_exists;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   char Query[128+Cns_MAX_BYTES_LONG_TEXT];
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   long ComCod;

   /***** Get data of social publishing *****/
   SocPub.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   /***** Get data of social note *****/
   SocNot.NotCod = SocPub.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      /***** Get the content of the comment *****/
      Par_GetParAndChangeFormat ("Comment",Content,Cns_MAX_BYTES_LONG_TEXT,
				 Str_TO_RIGOROUS_HTML,true);

      if (Content[0])
	{
	 /***** Check if I can comment *****/
	 if (Soc_CheckIfICanCommentNote (SocNot.NotCod))
	   {
	    /***** Publish *****/
	    /* Insert comment in the database */
	    sprintf (Query,"INSERT INTO social_comments (NotCod,UsrCod,TimeComment)"
			   " VALUES ('%ld','%ld',NOW())",
		     SocNot.NotCod,Gbl.Usrs.Me.UsrDat.UsrCod);
	    ComCod = DB_QueryINSERTandReturnCode (Query,"can not create comment");

	    /* Insert comment content in the database */
	    sprintf (Query,"INSERT INTO social_comments_content (ComCod,Content)"
			   " VALUES ('%ld','%s')",
		     ComCod,Content);
	    DB_QueryINSERT (Query,"can not store comment content");

	    /***** Message of success *****/
	    Lay_ShowAlert (Lay_SUCCESS,Txt_SOCIAL_PUBLISHING_Published);

	    /***** Show the social note just commented *****/
	    Soc_WriteSocialNote (&SocPub,&SocNot,true,false);
	   }
	 else
	    Lay_ShowErrorAndExit ("You can not comment this note.");
	}
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/******************* Check if I can comment a social note ********************/
/*****************************************************************************/
// I can comment a social note <=>
// <=> if I can view the note <=>
// <=> if I am a publisher (author or sharer)
//     or I follow at least one publisher (author or sharer)

static bool Soc_CheckIfICanCommentNote (long NotCod)
  {
   // char Query[256];

   /***** Check if I am a publisher of this note
          or I follow any of the publishers of this note *****/
   /*
   sprintf (Query,"SELECT COUNT(*) FROM social_timeline"
	          " WHERE NotCod='%ld' AND PublisherCod IN"
	          " (SELECT '%ld'"
		  " UNION"
		  " SELECT FollowedCod FROM usr_follow WHERE FollowerCod='%ld')",
	    NotCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if I can comment a social note") != 0);
   */
   // TODO: Remove this function if not used in future
   return (NotCod > 0);	// Anyone can comment
  }

/*****************************************************************************/
/**************************** Share a social note ****************************/
/*****************************************************************************/

void Soc_ShareSocialNoteGbl (void)
  {
   /***** Share social note *****/
   Soc_ShareSocialNote ();

   /***** Write updated timeline after sharing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_ShareSocialNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Share social note *****/
   Soc_ShareSocialNote ();

   /***** Write updated timeline after sharing (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_ShareSocialNote (void)
  {
   extern const char *Txt_SOCIAL_PUBLISHING_Shared;
   extern const char *Txt_The_original_post_no_longer_exists;
   struct SocialNote SocNot;
   struct SocialPublishing SocPub;
   bool IAmTheAuthor;
   bool IAmAPublisherOfThisSocNot;
   bool ICanShare;

   /***** Get the code of the social note to share *****/
   SocNot.NotCod = Soc_GetParamNotCod ();

   /***** Get data of social note *****/
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      IAmTheAuthor = (SocNot.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmTheAuthor)
	 IAmAPublisherOfThisSocNot = true;
      else
	 IAmAPublisherOfThisSocNot = Soc_CheckIfNoteIsPublishedInTimelineByUsr (SocNot.NotCod,
										Gbl.Usrs.Me.UsrDat.UsrCod);
      ICanShare = (Gbl.Usrs.Me.Logged &&
		   !IAmTheAuthor &&		// I am not the author
		   !IAmAPublisherOfThisSocNot);	// I have not shared the note
      if (ICanShare)
	{
	 /***** Share (publish social note in timeline) *****/
	 SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 SocPub.NotCod       = SocNot.NotCod;
	 Soc_PublishSocialNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	 /* Update number of times this social note is shared */
	 Soc_UpdateNumTimesANoteHasBeenShared (&SocNot);

	 /***** Message of success *****/
	 Lay_ShowAlert (Lay_SUCCESS,Txt_SOCIAL_PUBLISHING_Shared);

	 /***** Show the social note just shared *****/
	 Soc_WriteSocialNote (&SocPub,&SocNot,true,false);
	}
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/************** Unshare a previously shared social publishing ****************/
/*****************************************************************************/

void Soc_UnshareSocialPubGbl (void)
  {
   /***** Unshare a previously shared social publishing *****/
   Soc_UnshareSocialPublishing ();

   /***** Write updated timeline after unsharing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_UnshareSocialPubUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /*****  Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Unshare a previously shared social publishing *****/
   Soc_UnshareSocialPublishing ();

   /***** Write updated timeline after unsharing (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_UnshareSocialPublishing (void)
  {
   extern const char *Txt_SOCIAL_PUBLISHING_Unshared;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   bool IAmTheAuthor;
   bool IAmAPublisherOfThisSocNot;
   bool ICanUnshare;

   /***** Get data of social publishing *****/
   SocPub.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   /***** Get data of social note *****/
   SocNot.NotCod = SocPub.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   IAmTheAuthor = (SocNot.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
   if (IAmTheAuthor)
      IAmAPublisherOfThisSocNot = true;
   else
      IAmAPublisherOfThisSocNot = Soc_CheckIfNoteIsPublishedInTimelineByUsr (SocNot.NotCod,
									     Gbl.Usrs.Me.UsrDat.UsrCod);
   ICanUnshare = (Gbl.Usrs.Me.Logged &&
                  !IAmTheAuthor &&		// I am not the author
	          IAmAPublisherOfThisSocNot);	// I have shared the note
   if (ICanUnshare)
     {
      /***** Delete social publishing from database *****/
      Soc_UnshareASocialPublishingFromDB (&SocNot);

      /***** Update number of times this social note is shared *****/
      Soc_UpdateNumTimesANoteHasBeenShared (&SocNot);

      /***** Message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_SOCIAL_PUBLISHING_Unshared);

      /***** Show the social note corresponding
             to the publishing just unshared *****/
      Soc_WriteSocialNote (&SocPub,&SocNot,true,false);
     }
  }

/*****************************************************************************/
/**************** Unshare a social publishing from database ******************/
/*****************************************************************************/

static void Soc_UnshareASocialPublishingFromDB (struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Remove social publishing *****/
   sprintf (Query,"DELETE FROM social_timeline"
                  " USING social_timeline,social_notes"
	          " WHERE social_timeline.NotCod='%ld'"
	          " AND social_timeline.PublisherCod='%ld'"	// I have share this note
                  " AND social_timeline.NotCod=social_notes.NotCod"
                  " AND social_notes.UsrCod<>'%ld'",		// I am not the author
	    SocNot->NotCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not remove a social publishing");
  }

/*****************************************************************************/
/******************* Request the removal of a social note ********************/
/*****************************************************************************/

void Soc_RequestRemSocialNoteGbl (void)
  {
   /***** Request the removal of social note *****/
   Soc_RequestRemovalSocialNote ();

   /***** Write timeline again (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_RequestRemSocialNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Request the removal of social note *****/
   Soc_RequestRemovalSocialNote ();

   /***** Write timeline again (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_RequestRemovalSocialNote (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   bool ICanRemove;

   /***** Get data of social publishing *****/
   SocPub.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   /***** Get data of social note *****/
   SocNot.NotCod = SocPub.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocPub.PublisherCod == Gbl.Usrs.Me.UsrDat.UsrCod &&	// I have published this note
                 SocNot.UsrCod       == Gbl.Usrs.Me.UsrDat.UsrCod);	// I am the author of this note
   if (ICanRemove)
     {
      if (Soc_CheckIfNoteIsPublishedInTimelineByUsr (SocNot.NotCod,SocNot.UsrCod))
	{
	 /***** Show warning and social note *****/
	 /* Warning message */
	 Lay_ShowAlert (Lay_WARNING,Txt_Do_you_really_want_to_remove_the_following_comment);

	 /* Show social note */
	 Soc_WriteSocialNote (&SocPub,&SocNot,true,false);

	 /***** Form to ask for confirmation to remove this social post *****/
	 /* Start form */
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	   {
	    Act_FormStartAnchor (ActRemSocPubUsr,"timeline");
	    Usr_PutParamOtherUsrCodEncrypted ();
	   }
	 else
	    Act_FormStart (ActRemSocPubGbl);
	 Soc_PutHiddenParamPubCod (SocPub.PubCod);

	 /* End form */
	 Lay_PutRemoveButton (Txt_Remove);
	 Act_FormEnd ();
	}
     }
  }

/*****************************************************************************/
/*************************** Remove a social note ****************************/
/*****************************************************************************/

void Soc_RemoveSocialNoteGbl (void)
  {
   /***** Remove a social note *****/
   Soc_RemoveSocialNote ();

   /***** Write updated timeline after removing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_RemoveSocialNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Remove a social note *****/
   Soc_RemoveSocialNote ();

   /***** Write updated timeline after removing (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_RemoveSocialNote (void)
  {
   extern const char *Txt_SOCIAL_PUBLISHING_Removed;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   bool ICanRemove;

   /***** Get data of social publishing *****/
   SocPub.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialPublishingByCod (&SocPub);

   /***** Get data of social note *****/
   SocNot.NotCod = SocPub.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocPub.PublisherCod == Gbl.Usrs.Me.UsrDat.UsrCod &&	// I have published this note
                 SocNot.UsrCod       == Gbl.Usrs.Me.UsrDat.UsrCod);	// I am the author of this note
   if (ICanRemove)
     {
      /***** Delete social publishing from database *****/
      Soc_RemoveASocialNoteFromDB (&SocNot);

      /***** Message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_SOCIAL_PUBLISHING_Removed);
     }
  }

/*****************************************************************************/
/******************* Remove a social note from database **********************/
/*****************************************************************************/

static void Soc_RemoveASocialNoteFromDB (struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Remove all the social publishings of this note *****/
   sprintf (Query,"DELETE FROM social_timeline WHERE NotCod='%ld'",
	    SocNot->NotCod);
   DB_QueryDELETE (Query,"can not remove a social publishing");

   /***** Remove social note *****/
   sprintf (Query,"DELETE FROM social_notes"
	          " WHERE NotCod='%ld'"
	          " AND UsrCod='%ld'",		// Extra check: I am the author
	    SocNot->NotCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not remove a social note");

   if (SocNot->NoteType == Soc_NOTE_SOCIAL_POST)
     {
      /***** Remove social post *****/
      sprintf (Query,"DELETE FROM social_posts"
	             " WHERE PstCod='%ld'",
	       SocNot->Cod);
      DB_QueryDELETE (Query,"can not remove a social post");
     }

   /***** Remove content of the comments of this social note *****/
   sprintf (Query,"DELETE FROM social_comments_content"
	          " USING social_comments,social_comments_content"
	          " WHERE social_comments.NotCod='%ld'"
	          " AND social_comments.ComCod=social_comments_content.ComCod",
	    SocNot->NotCod);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Remove comments of this social note *****/
   sprintf (Query,"DELETE FROM social_comments"
	          " WHERE NotCod='%ld'",
	    SocNot->NotCod);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Reset social note *****/
   Soc_ResetSocialNote (SocNot);
  }

/*****************************************************************************/
/************* Request the removal of a comment in a social note *************/
/*****************************************************************************/

void Soc_RequestRemSocialComGbl (void)
  {
   /***** Request the removal of comment in social note *****/
   Soc_RequestRemovalSocialComment ();

   /***** Write timeline again (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_RequestRemSocialComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Request the removal of comment in social note *****/
   Soc_RequestRemovalSocialComment ();

   /***** Write timeline again (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_RequestRemovalSocialComment (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct SocialComment SocCom;
   bool ICanRemove;

   /***** Get the code of the social comment to remove *****/
   SocCom.ComCod = Soc_GetParamComCod ();

   /***** Get data of social comment *****/
   Soc_GetDataOfSocialCommentByCod (&SocCom);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocCom.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
   if (ICanRemove)
     {
      /***** Show warning and social comment *****/
      /* Warning message */
      Lay_ShowAlert (Lay_WARNING,Txt_Do_you_really_want_to_remove_the_following_comment);

      /* Show social comment */
      Soc_WriteSocialComment (&SocCom,true);

      /***** Form to ask for confirmation to remove this social comment *****/
      /* Start form */
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	{
	 Act_FormStartAnchor (ActRemSocComUsr,"timeline");
	 Usr_PutParamOtherUsrCodEncrypted ();
	}
      else
	 Act_FormStart (ActRemSocComGbl);
      Soc_PutHiddenParamComCod (SocCom.ComCod);

      /* End form */
      Lay_PutRemoveButton (Txt_Remove);
      Act_FormEnd ();
     }
  }

/*****************************************************************************/
/************************** Remove a social comment **************************/
/*****************************************************************************/

void Soc_RemoveSocialComGbl (void)
  {
   /***** Remove a social comment *****/
   Soc_RemoveSocialComment ();

   /***** Write updated timeline after removing (global) *****/
   Soc_ShowTimelineGbl ();
  }

void Soc_RemoveSocialComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Remove a social comment *****/
   Soc_RemoveSocialComment ();

   /***** Write updated timeline after removing (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static void Soc_RemoveSocialComment (void)
  {
   extern const char *Txt_SOCIAL_PUBLISHING_Removed;
   struct SocialComment SocCom;
   struct SocialNote SocNot;
   bool ICanRemove;

   /***** Get the code of the social comment to remove *****/
   SocCom.ComCod = Soc_GetParamComCod ();

   /***** Get data of social comment *****/
   Soc_GetDataOfSocialCommentByCod (&SocCom);

   /***** Get data of social note *****/
   SocNot.NotCod = SocCom.NotCod;
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocCom.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
   if (ICanRemove)
     {
      /***** Delete social comment from database *****/
      Soc_RemoveASocialCommentFromDB (&SocCom);

      /***** Message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_SOCIAL_PUBLISHING_Removed);
     }
  }

/*****************************************************************************/
/****************** Remove a social comment from database ********************/
/*****************************************************************************/

static void Soc_RemoveASocialCommentFromDB (struct SocialComment *SocCom)
  {
   char Query[128];

   /***** Remove content of this social comment *****/
   sprintf (Query,"DELETE FROM social_comments_content"
	          " WHERE ComCod='%ld'",
	    SocCom->ComCod);
   DB_QueryDELETE (Query,"can not remove a social comment");

   /***** Remove this social comment *****/
   sprintf (Query,"DELETE FROM social_comments"
	          " WHERE ComCod='%ld'"
	          " AND UsrCod='%ld'",	// Extra check: I am the author
	    SocCom->ComCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not remove a social comment");

   /***** Reset social comment *****/
   Soc_ResetSocialComment (SocCom);
  }

/*****************************************************************************/
/********** Remove all the social content of a user from database ************/
/*****************************************************************************/

void Soc_RemoveUsrSocialContent (long UsrCod)
  {
   char Query[512];

   /***** Remove social comments *****/
   /* Remove content of all the comments in all the social notes of the user */
   sprintf (Query,"DELETE FROM social_comments_content"
	          " USING social_comments,social_comments_content"
	          " WHERE social_comments.NotCod IN"
		  " (SELECT NotCod FROM social_notes WHERE UsrCod='%ld')"
	          " AND social_comments.ComCod=social_comments_content.ComCod",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social comments");

   /* Remove all the comments in all the social notes of the user */
   sprintf (Query,"DELETE FROM social_comments"
	          " WHERE NotCod IN"
		  " (SELECT NotCod FROM social_notes WHERE UsrCod='%ld')",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social comments");

   /* Remove content of all the comments of the user in any social note */
   sprintf (Query,"DELETE FROM social_comments_content"
	          " USING social_comments,social_comments_content"
	          " WHERE social_comments.UsrCod='%ld'"
	          " AND social_comments.ComCod=social_comments_content.ComCod",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social comments");

   /* Remove all the comments of the user in any social note */
   sprintf (Query,"DELETE FROM social_comments"
	          " WHERE UsrCod='%ld'",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Remove all the social posts of the user *****/
   sprintf (Query,"DELETE FROM social_posts"
		  " WHERE PstCod IN"
		  " (SELECT Cod FROM social_notes"
	          " WHERE UsrCod='%ld' AND NoteType='%u')",
	    UsrCod,(unsigned) Soc_NOTE_SOCIAL_POST);
   DB_QueryDELETE (Query,"can not remove social posts");

   /***** Remove all the social publishings of the user *****/
   sprintf (Query,"DELETE FROM social_timeline"
	          " WHERE PublisherCod='%ld'",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social publishings");

   /***** Remove all the social publishings authored by the user *****/
   sprintf (Query,"DELETE FROM social_timeline"
                  " USING social_notes,social_timeline"
	          " WHERE social_notes.UsrCod='%ld'"
                  " AND social_notes.NotCod=social_timeline.NotCod",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social publishings");

   /***** Remove all the social notes of the user *****/
   sprintf (Query,"DELETE FROM social_notes"
	          " WHERE UsrCod='%ld'",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social notes");
  }

/*****************************************************************************/
/**************** Check if a user has published a social note ****************/
/*****************************************************************************/

static bool Soc_CheckIfNoteIsPublishedInTimelineByUsr (long NotCod,long UsrCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM social_timeline"
	          " WHERE NotCod='%ld' AND PublisherCod='%ld'",
	    NotCod,UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user has published a social note") != 0);
  }

/*****************************************************************************/
/******** Get number of times a note code has been shared in timeline ********/
/*****************************************************************************/

static void Soc_UpdateNumTimesANoteHasBeenShared (struct SocialNote *SocNot)
  {
   char Query[128];

   /***** Get number of times (users) this note has been shared *****/
   sprintf (Query,"SELECT COUNT(*) FROM social_timeline"
	          " WHERE NotCod='%ld' AND PublisherCod<>'%ld'",
	    SocNot->NotCod,
	    SocNot->UsrCod);	// The author
   SocNot->NumShared = (unsigned) DB_QueryCOUNT (Query,"can not get number of times a note has been shared");
  }

/*****************************************************************************/
/**************** Show users who have shared this social note ****************/
/*****************************************************************************/

static void Soc_ShowUsrsWhoHaveSharedSocialNote (const struct SocialNote *SocNot)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   unsigned NumUsr;
   unsigned NumUsrsShown = 0;
   struct UsrData UsrDat;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];

   /* Show number of users who have shared this social note */
   fprintf (Gbl.F.Out,"<span class=\"SOCIAL_NUM_SHARES\"> %u</span>",
            SocNot->NumShared);

   if (SocNot->NumShared)
     {
      /***** Get list of publishers from database (only the first) *****/
      sprintf (Query,"SELECT PublisherCod"
		     " FROM social_timeline"
		     " WHERE NotCod='%ld' AND PublisherCod<>'%ld'"
		     " ORDER BY PubCod LIMIT %u",
	       SocNot->NotCod,SocNot->UsrCod,
	       Soc_MAX_NUM_SHARERS_SHOWN);
      NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get data of social publishing");
      if (NumUsrs)
	{
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

	    /***** Get user's data and show user's photo *****/
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
	      {
               fprintf (Gbl.F.Out,"<div class=\"SOCIAL_SHARER\">");
	       ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
	       Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
	                                             NULL,
	                         "PHOTO18x24",Pho_ZOOM);
               fprintf (Gbl.F.Out,"</div>");

               NumUsrsShown++;
              }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}

      if (SocNot->NumShared > NumUsrsShown)
	 fprintf (Gbl.F.Out,"<div class=\"SOCIAL_SHARER\">"
	                    "<img src=\"%s/ellipsis32x32.gif\""
			    " alt=\"%u\" title=\"%u\""
			    " class=\"ICON20x20\" />"
			    "</div>",
		  Gbl.Prefs.IconsURL,
		  SocNot->NumShared - NumUsrsShown,
		  SocNot->NumShared - NumUsrsShown);
     }
  }

/*****************************************************************************/
/********* Get data of social publishing in timeline using its code **********/
/*****************************************************************************/

static void Soc_GetDataOfSocialPublishingByCod (struct SocialPublishing *SocPub)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (SocPub->PubCod > 0)
     {
      /***** Get data of social publishing from database *****/
      sprintf (Query,"SELECT PubCod,NotCod,PublisherCod,UNIX_TIMESTAMP(TimePublish)"
		     " FROM social_timeline"
		     " WHERE PubCod='%ld'",
	       SocPub->PubCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social publishing"))
	{
	 /***** Get social note *****/
	 row = mysql_fetch_row (mysql_res);
	 Soc_GetDataOfSocialPublishingFromRow (row,SocPub);
	}
      else
	 /***** Reset fields of social publishing *****/
	 Soc_ResetSocialPublishing (SocPub);
     }
   else
      /***** Reset fields of social publishing *****/
      Soc_ResetSocialPublishing (SocPub);
  }

/*****************************************************************************/
/**************** Get data of social note using its code *********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialNoteByCod (struct SocialNote *SocNot)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (SocNot->NotCod > 0)
     {
      /***** Get data of social note from database *****/
      sprintf (Query,"SELECT NotCod,NoteType,UsrCod,HieCod,Cod,Unavailable,UNIX_TIMESTAMP(TimeNote)"
		     " FROM social_notes"
		     " WHERE NotCod='%ld'",
	       SocNot->NotCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social note"))
	{
	 /***** Get data of social note *****/
	 row = mysql_fetch_row (mysql_res);
	 Soc_GetDataOfSocialNoteFromRow (row,SocNot);

	 /***** Get number of times this social note has been shared *****/
	 Soc_UpdateNumTimesANoteHasBeenShared (SocNot);
	}
      else
	 /***** Reset fields of social note *****/
	 Soc_ResetSocialNote (SocNot);
     }
   else
      /***** Reset fields of social note *****/
      Soc_ResetSocialNote (SocNot);
  }

/*****************************************************************************/
/*************** Get data of social comment using its code *******************/
/*****************************************************************************/

static void Soc_GetDataOfSocialCommentByCod (struct SocialComment *SocCom)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (SocCom->ComCod > 0)
     {
      /***** Get data of social comment from database *****/
      sprintf (Query,"SELECT social_comments.ComCod,social_comments.UsrCod,"
		     "social_comments.NotCod,"
		     "UNIX_TIMESTAMP(social_comments.TimeComment),"
		     "social_comments_content.Content"
		     " FROM social_comments,social_comments_content"
		     " WHERE social_comments.ComCod='%ld'"
		     " AND social_comments.ComCod=social_comments_content.ComCod",
	       SocCom->ComCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social comment"))
	{
	 /***** Get data of social comment *****/
	 row = mysql_fetch_row (mysql_res);
	 Soc_GetDataOfSocialCommentFromRow (row,SocCom);
	}
      else
	 /***** Reset fields of social comment *****/
	 Soc_ResetSocialComment (SocCom);
     }
   else
      /***** Reset fields of social comment *****/
      Soc_ResetSocialComment (SocCom);
  }

/*****************************************************************************/
/************** Get data of social publishing using its code *****************/
/*****************************************************************************/

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub)
  {
   /* Get code of social publishing (row[0]) */
   SocPub->PubCod       = Str_ConvertStrCodToLongCod (row[0]);

   /* Get social note code (row[1]) */
   SocPub->NotCod       = Str_ConvertStrCodToLongCod (row[1]);

   /* Get publisher's code (row[2]) */
   SocPub->PublisherCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get time of the note (row[3]) */
   SocPub->DateTimeUTC  = Dat_GetUNIXTimeFromStr (row[3]);
  }

/*****************************************************************************/
/******************** Get data of social note from row ***********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialNoteFromRow (MYSQL_ROW row,struct SocialNote *SocNot)
  {
   /* Get social code (row[0]) */
   SocNot->NotCod      = Str_ConvertStrCodToLongCod (row[0]);

   /* Get note type (row[1]) */
   SocNot->NoteType    = Soc_GetNoteTypeFromStr ((const char *) row[1]);

   /* Get (from) user code (row[2]) */
   SocNot->UsrCod      = Str_ConvertStrCodToLongCod (row[2]);

   /* Get hierarchy code (row[3]) */
   SocNot->HieCod      = Str_ConvertStrCodToLongCod (row[3]);

   /* Get file/post... code (row[4]) */
   SocNot->Cod         = Str_ConvertStrCodToLongCod (row[4]);

   /* File/post... unavailable (row[5]) */
   SocNot->Unavailable = (Str_ConvertToUpperLetter (row[5][0]) == 'Y');

   /* Get time of the note (row[6]) */
   SocNot->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[6]);
  }

/*****************************************************************************/
/****** Get social note type from string number coming from database *********/
/*****************************************************************************/

static Soc_NoteType_t Soc_GetNoteTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Soc_NUM_SOCIAL_NOTES)
         return (Soc_NoteType_t) UnsignedNum;

   return Soc_NOTE_UNKNOWN;
  }

/*****************************************************************************/
/****************** Get data of social comment from row **********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialCommentFromRow (MYSQL_ROW row,struct SocialComment *SocCom)
  {
   /* Get code of social comment (row[0]) */
   SocCom->ComCod      = Str_ConvertStrCodToLongCod (row[0]);

   /* Get (from) user code (row[1]) */
   SocCom->UsrCod      = Str_ConvertStrCodToLongCod (row[1]);

   /* Get code of social note (row[2]) */
   SocCom->NotCod      = Str_ConvertStrCodToLongCod (row[2]);

   /* Get time of the note (row[3]) */
   SocCom->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[3]);

   /* Get content (row[4]) */
   strncpy (SocCom->Content,row[4],Cns_MAX_BYTES_LONG_TEXT);
   SocCom->Content[Cns_MAX_BYTES_LONG_TEXT] = '\0';
  }

/*****************************************************************************/
/******************** Reset fields of social publishing **********************/
/*****************************************************************************/

static void Soc_ResetSocialPublishing (struct SocialPublishing *SocPub)
  {
   SocPub->PubCod       = -1L;
   SocPub->NotCod       = -1L;
   SocPub->PublisherCod = -1L;
   SocPub->DateTimeUTC  = (time_t) 0;
  }

/*****************************************************************************/
/*********************** Reset fields of social note *************************/
/*****************************************************************************/

static void Soc_ResetSocialNote (struct SocialNote *SocNot)
  {
   SocNot->NotCod      = -1L;
   SocNot->NoteType    = Soc_NOTE_UNKNOWN;
   SocNot->UsrCod      = -1L;
   SocNot->HieCod      = -1L;
   SocNot->Cod         = -1L;
   SocNot->Unavailable = false;
   SocNot->DateTimeUTC = (time_t) 0;
   SocNot->NumShared   = 0;
  }

/*****************************************************************************/
/********************** Reset fields of social comment ***********************/
/*****************************************************************************/

static void Soc_ResetSocialComment (struct SocialComment *SocCom)
  {
   SocCom->ComCod      = -1L;
   SocCom->UsrCod      = -1L;
   SocCom->NotCod      = -1L;
   SocCom->DateTimeUTC = (time_t) 0;
   SocCom->Content[0]  = '\0';
  }
