// swad_social.c: social networking (timeline)

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

#include <linux/limits.h>	// For PATH_MAX
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions
#include <sys/types.h>		// For time_t

#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_image.h"
#include "swad_layout.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_social.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Soc_WIDTH_TIMELINE		"560px"
#define Soc_MAX_SHARERS_FAVERS_SHOWN	7	// Maximum number of users shown who have share/fav a social note

#define Soc_MAX_CHARS_IN_POST	1000

// Number of recent publishings got and shown the first time, before refreshing
#define Soc_MAX_NEW_PUBS_TO_GET_AND_SHOW	10000	// Unlimited
#define Soc_MAX_REC_PUBS_TO_GET_AND_SHOW	20	// Recent publishings to show (first time)
#define Soc_MAX_OLD_PUBS_TO_GET_AND_SHOW	20	// IMPORTANT: If you change this number,
							// set also this constant to the new value
							// in JavaScript function readOldTimelineData

typedef enum
  {
   Soc_TIMELINE_USR,	// Show the timeline of a user
   Soc_TIMELINE_GBL,	// Show the timeline of the users follwed by me
  } Soc_TimelineUsrOrGbl_t;

typedef enum
  {
   Soc_GET_ONLY_NEW_PUBS,	// New publishings are retrieved via AJAX
				// automatically from time to time
   Soc_GET_RECENT_TIMELINE,	// Recent timeline is shown when user clicks on action menu,...
				// or after editing timeline
   Soc_GET_ONLY_OLD_PUBS,	// Old publishings are retrieved via AJAX
				// when user clicks on link at bottom of timeline
  } Soc_WhatToGetFromTimeline_t;

// Social images will be saved with:
// - maximum width of Soc_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of Soc_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Soc_IMAGE_SAVED_MAX_WIDTH	768
#define Soc_IMAGE_SAVED_MAX_HEIGHT	512
#define Soc_IMAGE_SAVED_QUALITY		 50	// 1 to 100
// in social posts, the quality is low in order to speed up the loading of images

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

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
   unsigned NumFavs;	// Number of times (users) this note has been favourited
  };

struct SocialComment
  {
   long PubCod;
   long UsrCod;
   long NotCod;		// Note code
   time_t DateTimeUTC;
   unsigned NumFavs;	// Number of times (users) this comment has been favourited
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Image Image;
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

static void Soc_ShowTimelineGblHighlightingNot (long NotCod);
static void Soc_ShowTimelineUsrHighlightingNot (long NotCod);

static void Soc_GetAndShowOldTimeline (Soc_TimelineUsrOrGbl_t TimelineUsrOrGbl);

static void Soc_BuildQueryToGetTimeline (Soc_TimelineUsrOrGbl_t TimelineUsrOrGbl,
                                         Soc_WhatToGetFromTimeline_t WhatToGetFromTimeline,
                                         char *Query);
static long Soc_GetPubCodFromSession (const char *FieldName);
static void Soc_UpdateLastPubCodIntoSession (void);
static void Soc_UpdateFirstPubCodIntoSession (long FirstPubCod);
static void Soc_DropTemporaryTablesUsedToQueryTimeline (void);

static void Soc_ShowTimeline (const char *Query,const char *Title,
                              long NotCodToHighlight);
static void Soc_PutIconsTimeline (void);

static void Soc_FormStart (Act_Action_t ActionGbl,Act_Action_t ActionUsr);

static void Soc_PutFormWhichUsrs (void);
static void Soc_PutParamWhichUsrs (void);
static void Soc_GetParamsWhichUsrs (void);

static void Soc_ShowWarningYouDontFollowAnyUser (void);

static void Soc_InsertNewPubsInTimeline (const char *Query);
static void Soc_ShowOldPubsInTimeline (const char *Query);

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub);

static void Soc_PutLinkToViewNewPublishings (void);
static void Soc_PutLinkToViewOldPublishings (void);

static void Soc_WriteSocialNote (const struct SocialNote *SocNot,
                                 Soc_TopMessage_t TopMessage,long UsrCod,
                                 bool Highlight,
                                 bool ShowNoteAlone);
static void Soc_WriteTopMessage (Soc_TopMessage_t TopMessage,long UsrCod);
static void Soc_WriteAuthorNote (const struct UsrData *UsrDat);
static void Soc_WriteDateTime (time_t TimeUTC);
static void Soc_GetAndWriteSocialPost (long PstCod);
static void Soc_PutFormGoToAction (const struct SocialNote *SocNot);
static void Soc_GetNoteSummary (const struct SocialNote *SocNot,
                                char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1]);
static void Soc_PublishSocialNoteInTimeline (struct SocialPublishing *SocPub);

static void Soc_PutFormToWriteNewPost (void);
static void Soc_PutTextarea (const char *Placeholder,
                             const char *ClassTextArea,const char *ClassImgTit);

static long Soc_ReceiveSocialPost (void);

static void Soc_PutIconToToggleCommentSocialNote (const char UniqueId[Act_MAX_BYTES_ID]);
static void Soc_PutIconCommentDisabled (void);
static void Soc_PutHiddenFormToWriteNewCommentToSocialNote (long NotCod,
                                                            const char IdNewComment[Act_MAX_BYTES_ID]);
static unsigned long Soc_GetNumCommentsInSocialNote (long NotCod);
static void Soc_WriteCommentsInSocialNote (const struct SocialNote *SocNot);
static void Soc_WriteSocialComment (struct SocialComment *SocCom,
                                    Soc_TopMessage_t TopMessage,long UsrCod,
                                    bool ShowCommentAlone);
static void Soc_WriteAuthorComment (struct UsrData *UsrDat);

static void Soc_PutFormToRemoveComment (long PubCod);
static void Soc_PutFormToFavSocialComment (long PubCod);

static void Soc_PutDisabledIconShare (unsigned NumShared);
static void Soc_PutDisabledIconFav (unsigned NumFavs);

static void Soc_PutFormToShareSocialNote (long NotCod);
static void Soc_PutFormToFavSocialNote (long NotCod);

static void Soc_PutFormToUnshareSocialNote (long NotCod);
static void Soc_PutFormToUnfavSocialNote (long NotCod);
static void Soc_PutFormToUnfavSocialComment (long PubCod);

static void Soc_PutFormToRemoveSocialPublishing (long NotCod);

static void Soc_PutHiddenParamNotCod (long NotCod);
static long Soc_GetParamNotCod (void);
static long Soc_GetParamPubCod (void);

static long Soc_ReceiveComment (void);

static long Soc_ShareSocialNote (void);
static long Soc_FavSocialNote (void);
static long Soc_FavSocialComment (void);
static void Soc_CreateNotifToAuthor (long AuthorCod,long PubCod,
                                     Ntf_NotifyEvent_t NotifyEvent);

static long Soc_UnshareSocialNote (void);
static long Soc_UnfavSocialNote (void);
static long Soc_UnfavSocialComment (void);

static void Soc_RequestRemovalSocialNote (void);
static void Soc_PutParamsRemoveSocialNote (void);
static void Soc_RemoveSocialNote (void);
static void Soc_RemoveImgFileFromSocialPost (long PstCod);
static void Soc_RemoveASocialNoteFromDB (struct SocialNote *SocNot);

static long Soc_GetNotCodOfSocialPublishing (long PubCod);
static long Soc_GetPubCodOfOriginalSocialNote (long NotCod);

static void Soc_RequestRemovalSocialComment (void);
static void Soc_PutParamsRemoveSocialCommment (void);
static void Soc_RemoveSocialComment (void);
static void Soc_RemoveImgFileFromSocialComment (long PubCod);
static void Soc_RemoveASocialCommentFromDB (struct SocialComment *SocCom);

static bool Soc_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod);
static bool Soc_CheckIfNoteIsFavedByUsr (long NotCod,long UsrCod);
static bool Soc_CheckIfCommIsFavedByUsr (long PubCod,long UsrCod);

static unsigned Soc_UpdateNumTimesANoteHasBeenShared (struct SocialNote *SocNot);
static unsigned Soc_GetNumTimesANoteHasBeenFav (struct SocialNote *SocNot);
static unsigned Soc_GetNumTimesACommHasBeenFav (struct SocialComment *SocCom);

static void Soc_ShowUsrsWhoHaveSharedSocialNote (const struct SocialNote *SocNot);
static void Soc_ShowUsrsWhoHaveMarkedSocialNoteAsFav (const struct SocialNote *SocNot);
static void Soc_ShowUsrsWhoHaveMarkedSocialCommAsFav (const struct SocialComment *SocCom);
static void Soc_ShowSharersOrFavers (unsigned NumUsrs,const char *Query);

static void Soc_GetDataOfSocialNotByCod (struct SocialNote *SocNot);
static void Soc_GetDataOfSocialComByCod (struct SocialComment *SocCom);

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub);
static void Soc_GetDataOfSocialNoteFromRow (MYSQL_ROW row,struct SocialNote *SocNot);
static Soc_PubType_t Soc_GetPubTypeFromStr (const char *Str);
static Soc_NoteType_t Soc_GetNoteTypeFromStr (const char *Str);
static void Soc_GetDataOfSocialCommentFromRow (MYSQL_ROW row,struct SocialComment *SocCom);

static void Soc_ResetSocialNote (struct SocialNote *SocNot);
static void Soc_ResetSocialComment (struct SocialComment *SocCom);

static void Soc_ClearTimelineThisSession (void);
static void Soc_AddNotesJustRetrievedToTimelineThisSession (void);

static void Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (long PubCod,const char *Txt);

/*****************************************************************************/
/***** Show social activity (timeline) including all the users I follow ******/
/*****************************************************************************/

void Soc_ShowTimelineGbl1 (void)
  {
   /***** Mark all my notifications about timeline as seen *****/
   Soc_MarkMyNotifAsSeen ();

   /***** Get which users *****/
   Soc_GetParamsWhichUsrs ();
  }

void Soc_ShowTimelineGbl2 (void)
  {
   long PubCod;
   struct SocialNote SocNot;
   struct UsrData UsrDat;
   Ntf_NotifyEvent_t NotifyEvent;
   const Soc_TopMessage_t TopMessages[Ntf_NUM_NOTIFY_EVENTS] =
     {
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_UNKNOWN

      /* Course tab */
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_DOCUMENT_FILE
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_TEACHERS_FILE
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_SHARED_FILE

      /* Assessment tab */
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_ASSIGNMENT
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_EXAM_ANNOUNCEMENT
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_MARKS_FILE

      /* Users tab */
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_ENROLMENT_STD
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_ENROLMENT_TCH
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_ENROLMENT_REQUEST

      /* Social tab */
      Soc_TOP_MESSAGE_COMMENTED,	// Ntf_EVENT_TIMELINE_COMMENT
      Soc_TOP_MESSAGE_FAVED,		// Ntf_EVENT_TIMELINE_FAV
      Soc_TOP_MESSAGE_SHARED,		// Ntf_EVENT_TIMELINE_SHARE
      Soc_TOP_MESSAGE_MENTIONED,	// Ntf_EVENT_TIMELINE_MENTION
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_FOLLOWER
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_FORUM_POST_COURSE
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_FORUM_REPLY

      /* Messages tab */
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_NOTICE
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_MESSAGE

      /* Statistics tab */

      /* Profile tab */

      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_SURVEY		// TODO: Move to assessment tab (also necessary in database) !!!!!!!!!
      Soc_TOP_MESSAGE_NONE,		// Ntf_EVENT_ENROLMENT_NET	// TODO: Move to users tab (also necessary in database) !!!!!!!!!
     };

   /***** Initialize social note code to -1 ==> no highlighted note *****/
   SocNot.NotCod = -1L;

   /***** Get parameter with the code of a social publishing *****/
   // This parameter is optional. It can be provided by a notification.
   // If > 0 ==> the social note is shown highlighted above the timeline
   PubCod = Soc_GetParamPubCod ();
   if (PubCod > 0)
      /***** Get code of social note from database *****/
      SocNot.NotCod = Soc_GetNotCodOfSocialPublishing (PubCod);

   if (SocNot.NotCod > 0)
     {
      /* Get who did the action (publishing, commenting, faving, sharing, mentioning) */
      Usr_GetParamOtherUsrCodEncrypted (&UsrDat);

      /* Get what he/she did */
      NotifyEvent = Ntf_GetParamNotifyEvent ();

      /***** Show the social note highlighted *****/
      Soc_GetDataOfSocialNotByCod (&SocNot);
      Soc_WriteSocialNote (&SocNot,
			   TopMessages[NotifyEvent],UsrDat.UsrCod,
			   true,true);
     }

   /***** Show timeline with possible highlighted note *****/
   Soc_ShowTimelineGblHighlightingNot (SocNot.NotCod);
  }

static void Soc_ShowTimelineGblHighlightingNot (long NotCod)
  {
   extern const char *Txt_Timeline;
   char Query[1024];

   /***** Build query to get timeline *****/
   Soc_BuildQueryToGetTimeline (Soc_TIMELINE_GBL,
                                Soc_GET_RECENT_TIMELINE,
                                Query);

   /***** Show timeline *****/
   Soc_ShowTimeline (Query,Txt_Timeline,NotCod);

   /***** Drop temporary tables *****/
   Soc_DropTemporaryTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/*********** Show social activity (timeline) of a selected user **************/
/*****************************************************************************/

void Soc_ShowTimelineUsr (void)
  {
   Soc_ShowTimelineUsrHighlightingNot (-1L);
  }

static void Soc_ShowTimelineUsrHighlightingNot (long NotCod)
  {
   extern const char *Txt_Timeline_OF_A_USER;
   char Query[1024];

   /***** Build query to show timeline with publishings of a unique user *****/
   Soc_BuildQueryToGetTimeline (Soc_TIMELINE_USR,
                                Soc_GET_RECENT_TIMELINE,
                                Query);

   /***** Show timeline *****/
   sprintf (Gbl.Title,Txt_Timeline_OF_A_USER,Gbl.Usrs.Other.UsrDat.FirstName);
   Soc_ShowTimeline (Query,Gbl.Title,NotCod);

   /***** Drop temporary tables *****/
   Soc_DropTemporaryTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/********** Refresh new publishings in social timeline via AJAX **************/
/*****************************************************************************/

void Soc_RefreshNewTimelineGbl (void)
  {
   char Query[1024];

   if (Gbl.Session.IsOpen)	// If session has been closed, do not write anything
     {
      /***** Send, before the HTML, the refresh time *****/
      fprintf (Gbl.F.Out,"%lu|",
	       Cfg_TIME_TO_REFRESH_SOCIAL_TIMELINE);

      /***** Get which users *****/
      Soc_GetParamsWhichUsrs ();

      /***** Build query to get timeline *****/
      Soc_BuildQueryToGetTimeline (Soc_TIMELINE_GBL,
				   Soc_GET_ONLY_NEW_PUBS,
				   Query);

      /***** Show new timeline *****/
      Soc_InsertNewPubsInTimeline (Query);

      /***** Drop temporary tables *****/
      Soc_DropTemporaryTablesUsedToQueryTimeline ();
     }

   /***** All the output is made, so don't write anymore *****/
   Gbl.Layout.DivsEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }

/*****************************************************************************/
/************ View old publishings in social timeline via AJAX ***************/
/*****************************************************************************/

void Soc_RefreshOldTimelineGbl (void)
  {
   /***** Get which users *****/
   Soc_GetParamsWhichUsrs ();

   /***** Show old publishings *****/
   Soc_GetAndShowOldTimeline (Soc_TIMELINE_GBL);
  }

void Soc_RefreshOldTimelineUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())	// Existing user
      /***** If user exists, show old publishings *****/
      Soc_GetAndShowOldTimeline (Soc_TIMELINE_USR);
  }

/*****************************************************************************/
/**************** Get and show old publishings in timeline *******************/
/*****************************************************************************/

static void Soc_GetAndShowOldTimeline (Soc_TimelineUsrOrGbl_t TimelineUsrOrGbl)
  {
   char Query[1024];

   /***** Build query to get timeline *****/
   Soc_BuildQueryToGetTimeline (TimelineUsrOrGbl,
                                Soc_GET_ONLY_OLD_PUBS,
                                Query);

   /***** Show old timeline *****/
   Soc_ShowOldPubsInTimeline (Query);

   /***** Drop temporary tables *****/
   Soc_DropTemporaryTablesUsedToQueryTimeline ();

   /***** All the output is made, so don't write anymore *****/
   Gbl.Layout.DivsEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }

/*****************************************************************************/
/************ Mark all my notifications about timeline as seen ***************/
/*****************************************************************************/
// Must be executed as a priori function

void Soc_MarkMyNotifAsSeen (void)
  {
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_COMMENT,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_FAV    ,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_SHARE  ,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_MENTION,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************ Build query to get timeline ************************/
/*****************************************************************************/
// Query must have space for at least 1024 chars

#define Soc_MAX_BYTES_SUBQUERY_ALREADY_EXISTS (256 - 1)

static void Soc_BuildQueryToGetTimeline (Soc_TimelineUsrOrGbl_t TimelineUsrOrGbl,
                                         Soc_WhatToGetFromTimeline_t WhatToGetFromTimeline,
                                         char *Query)
  {
   char SubQueryPublishers[128];
   char SubQueryRangeBottom[128];
   char SubQueryRangeTop[128];
   char SubQueryAlreadyExists[Soc_MAX_BYTES_SUBQUERY_ALREADY_EXISTS + 1];
   struct
     {
      long Top;
      long Bottom;
     } RangePubsToGet;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPub;
   long PubCod;
   long NotCod;
   const unsigned MaxPubsToGet[3] =
     {
      Soc_MAX_NEW_PUBS_TO_GET_AND_SHOW,	// Soc_GET_ONLY_NEW_PUBS
      Soc_MAX_REC_PUBS_TO_GET_AND_SHOW,	// Soc_GET_RECENT_TIMELINE
      Soc_MAX_OLD_PUBS_TO_GET_AND_SHOW,	// Soc_GET_ONLY_OLD_PUBS
     };

   /***** Clear social timeline for this session in database *****/
   if (WhatToGetFromTimeline == Soc_GET_RECENT_TIMELINE)
      Soc_ClearTimelineThisSession ();

   /***** Drop temporary tables *****/
   Soc_DropTemporaryTablesUsedToQueryTimeline ();

   /***** Create temporary table with publishing codes *****/
   sprintf (Query,"CREATE TEMPORARY TABLE pub_codes "
	          "(PubCod BIGINT NOT NULL,UNIQUE INDEX(PubCod)) ENGINE=MEMORY");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create temporary table");

   /***** Create temporary table with notes got in this execution *****/
   sprintf (Query,"CREATE TEMPORARY TABLE not_codes "
	          "(NotCod BIGINT NOT NULL,INDEX(NotCod)) ENGINE=MEMORY");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create temporary table");

   /***** Create temporary table with notes already present in timeline for this session *****/
   sprintf (Query,"CREATE TEMPORARY TABLE current_timeline "
		  "(NotCod BIGINT NOT NULL,INDEX(NotCod)) ENGINE=MEMORY"
		  " SELECT NotCod FROM social_timelines WHERE SessionId='%s'",
	    Gbl.Session.Id);
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create temporary table");

   /***** Create temporary table and subquery with potential publishers *****/
   switch (TimelineUsrOrGbl)
     {
      case Soc_TIMELINE_USR:	// Show the timeline of a user
	 sprintf (SubQueryPublishers,"PublisherCod=%ld AND ",
	          Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case Soc_TIMELINE_GBL:	// Show the global timeline
	 switch (Gbl.Social.WhichUsrs)
	   {
	    case Soc_FOLLOWED:	// Show the timeline of the users I follow
	       sprintf (Query,"CREATE TEMPORARY TABLE publishers "
			      "(UsrCod INT NOT NULL,UNIQUE INDEX(UsrCod)) ENGINE=MEMORY"
			      " SELECT %ld AS UsrCod"
			      " UNION"
			      " SELECT FollowedCod AS UsrCod"
			      " FROM usr_follow WHERE FollowerCod=%ld",
			Gbl.Usrs.Me.UsrDat.UsrCod,
			Gbl.Usrs.Me.UsrDat.UsrCod);
	       if (mysql_query (&Gbl.mysql,Query))
		  DB_ExitOnMySQLError ("can not create temporary table");
	       sprintf (SubQueryPublishers,"social_pubs.PublisherCod=publishers.UsrCod AND ");
	       break;
	    case Soc_ALL_USRS:	// Show the timeline of all users
	       SubQueryPublishers[0] = '\0';
	       break;
	   }
	 break;
     }

   /***** Create subquery to get only notes not present in timeline *****/
   switch (TimelineUsrOrGbl)
     {
      case Soc_TIMELINE_USR:	// Show the timeline of a user
	 switch (WhatToGetFromTimeline)
           {
            case Soc_GET_ONLY_NEW_PUBS:
            case Soc_GET_RECENT_TIMELINE:
	       Str_Copy (SubQueryAlreadyExists,
	                 " NotCod NOT IN"
			 " (SELECT NotCod FROM not_codes)",
			 Soc_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
            case Soc_GET_ONLY_OLD_PUBS:
	       Str_Copy (SubQueryAlreadyExists,
	                 " NotCod NOT IN"
			 " (SELECT NotCod FROM current_timeline)",
			 Soc_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
           }
	 break;
      case Soc_TIMELINE_GBL:	// Show the timeline of the users I follow
	 switch (WhatToGetFromTimeline)
           {
            case Soc_GET_ONLY_NEW_PUBS:
            case Soc_GET_RECENT_TIMELINE:
	       Str_Copy (SubQueryAlreadyExists,
	                 " social_pubs.NotCod NOT IN"
			 " (SELECT NotCod FROM not_codes)",
			 Soc_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
            case Soc_GET_ONLY_OLD_PUBS:
	       Str_Copy (SubQueryAlreadyExists,
	                 " social_pubs.NotCod NOT IN"
			 " (SELECT NotCod FROM current_timeline)",
			 Soc_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
           }
	 break;
     }

   /***** Get the publishings in timeline *****/
   /* Initialize range of pubs:

            social_pubs
               _____
              |_____|11
              |_____|10
             _|_____| 9 <-- RangePubsToGet.Top
     Get    / |_____| 8
    pubs   |  |_____| 7
    from  <   |_____| 6
    this   |  |_____| 5
   range    \_|_____| 4
              |_____| 3 <-- RangePubsToGet.Bottom
              |_____| 2
              |_____| 1
                      0
   */
   RangePubsToGet.Top    = 0;	// +Infinite
   RangePubsToGet.Bottom = 0;	// -Infinite
   switch (WhatToGetFromTimeline)
     {
      case Soc_GET_ONLY_NEW_PUBS:	 // Get the publishings (without limit) newer than LastPubCod
	 /* This query is made via AJAX automatically from time to time */
	 RangePubsToGet.Bottom = Soc_GetPubCodFromSession ("LastPubCod");
	 break;
      case Soc_GET_RECENT_TIMELINE:	 // Get some limited recent publishings
	 /* This is the first query to get initial timeline shown
	    ==> no notes yet in current timeline table */
	 break;
      case Soc_GET_ONLY_OLD_PUBS:	 // Get some limited publishings older than FirstPubCod
	 /* This query is made via AJAX
	    when I click in link to get old publishings */
	 RangePubsToGet.Top    = Soc_GetPubCodFromSession ("FirstPubCod");
	 break;
     }

   /*
      With the current approach, we select one by one
      the publishings and notes in a loop. In each iteration,
      we get the more recent publishing (original, shared or commment)
      of every set of publishings corresponding to the same note,
      checking that the note is not already retrieved.
      After getting a publishing, its note code is stored
      in order to not get it again.

      As an alternative, we tried to get the maximum PubCod,
      i.e more recent publishing (original, shared or commment),
      of every set of publishings corresponding to the same note:
      "SELECT MAX(PubCod) AS NewestPubCod FROM social_pubs ...
      " GROUP BY NotCod ORDER BY NewestPubCod DESC LIMIT ..."
      but this query is slow (several seconds) with a big table.
    */
   for (NumPub = 0;
	NumPub < MaxPubsToGet[WhatToGetFromTimeline];
	NumPub++)
     {
      /* Create subqueries with range of publishings to get from social_pubs */
      if (RangePubsToGet.Bottom > 0)
	 switch (TimelineUsrOrGbl)
	   {
	    case Soc_TIMELINE_USR:	// Show the timeline of a user
	       sprintf (SubQueryRangeBottom,"PubCod>%ld AND ",RangePubsToGet.Bottom);
	       break;
	    case Soc_TIMELINE_GBL:	// Show the global timeline
	       switch (Gbl.Social.WhichUsrs)
		 {
		  case Soc_FOLLOWED:	// Show the timeline of the users I follow
		     sprintf (SubQueryRangeBottom,"social_pubs.PubCod>%ld AND ",RangePubsToGet.Bottom);
		     break;
		  case Soc_ALL_USRS:	// Show the timeline of all users
		     sprintf (SubQueryRangeBottom,"PubCod>%ld AND ",RangePubsToGet.Bottom);
		     break;
		 }
	       break;
	   }
      else
	 SubQueryRangeBottom[0] = '\0';

      if (RangePubsToGet.Top > 0)
	 switch (TimelineUsrOrGbl)
	   {
	    case Soc_TIMELINE_USR:	// Show the timeline of a user
	       sprintf (SubQueryRangeTop,"PubCod<%ld AND ",RangePubsToGet.Top);
	       break;
	    case Soc_TIMELINE_GBL:	// Show the global timeline
	       switch (Gbl.Social.WhichUsrs)
		 {
		  case Soc_FOLLOWED:	// Show the timeline of the users I follow
		     sprintf (SubQueryRangeTop,"social_pubs.PubCod<%ld AND ",RangePubsToGet.Top);
		     break;
		  case Soc_ALL_USRS:	// Show the timeline of all users
		     sprintf (SubQueryRangeTop,"PubCod<%ld AND ",RangePubsToGet.Top);
		     break;
		 }
	       break;
	   }
      else
	 SubQueryRangeTop[0] = '\0';

      /* Select the most recent publishing from social_pubs */
      switch (TimelineUsrOrGbl)
	{
	 case Soc_TIMELINE_USR:	// Show the timeline of a user
	    sprintf (Query,"SELECT PubCod,NotCod FROM social_pubs"
			   " WHERE %s%s%s%s"
			   " ORDER BY PubCod DESC LIMIT 1",
		     SubQueryRangeBottom,SubQueryRangeTop,
		     SubQueryPublishers,
		     SubQueryAlreadyExists);
	    break;
	 case Soc_TIMELINE_GBL:	// Show the global timeline
	    switch (Gbl.Social.WhichUsrs)
	      {
	       case Soc_FOLLOWED:	// Show the timeline of the users I follow
		  sprintf (Query,"SELECT PubCod,NotCod FROM social_pubs,publishers"
				 " WHERE %s%s%s%s"
				 " ORDER BY social_pubs.PubCod DESC LIMIT 1",
			   SubQueryRangeBottom,SubQueryRangeTop,
			   SubQueryPublishers,
			   SubQueryAlreadyExists);
		  break;
	       case Soc_ALL_USRS:	// Show the timeline of all users
		  sprintf (Query,"SELECT PubCod,NotCod FROM social_pubs"
				 " WHERE %s%s%s"
				 " ORDER BY PubCod DESC LIMIT 1",
			   SubQueryRangeBottom,SubQueryRangeTop,
			   SubQueryAlreadyExists);
		  break;
	      }
	    break;
	}
      if (DB_QuerySELECT (Query,&mysql_res,"can not get publishing") == 1)
	{
	 /* Get code of social publishing */
	 row = mysql_fetch_row (mysql_res);
	 PubCod = Str_ConvertStrCodToLongCod (row[0]);
	}
      else
	 PubCod = -1L;

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      if (PubCod > 0)
	{
	 sprintf (Query,"INSERT INTO pub_codes SET PubCod=%ld",PubCod);
	 DB_QueryINSERT (Query,"can not store publishing code");
	 RangePubsToGet.Top = PubCod;	// Narrow the range for the next iteration

	 /* Get social note code (row[1]) */
	 NotCod = Str_ConvertStrCodToLongCod (row[1]);
	 sprintf (Query,"INSERT INTO not_codes SET NotCod=%ld",NotCod);
	 DB_QueryINSERT (Query,"can not store note code");
	 sprintf (Query,"INSERT INTO current_timeline SET NotCod=%ld",NotCod);
	 DB_QueryINSERT (Query,"can not store note code");
	}
      else	// Nothing got ==> abort loop
         break;	// Last publishing
     }

   /***** Update last publishing code into session for next refresh *****/
   // Do this inmediately after getting the publishings codes...
   // ...in order to not lose publishings
   Soc_UpdateLastPubCodIntoSession ();

   /***** Add notes just retrieved to current timeline for this session *****/
   Soc_AddNotesJustRetrievedToTimelineThisSession ();

   /***** Build query to show timeline including the users I am following *****/
   sprintf (Query,"SELECT PubCod,NotCod,PublisherCod,PubType,UNIX_TIMESTAMP(TimePublish)"
		  " FROM social_pubs WHERE PubCod IN "
		  "(SELECT PubCod FROM pub_codes)"
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
   char Query[128 + Ses_BYTES_SESSION_ID];
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

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PubCod;
  }

/*****************************************************************************/
/*********************** Update last publishing code *************************/
/*****************************************************************************/

static void Soc_UpdateLastPubCodIntoSession (void)
  {
   char Query[256 + Ses_BYTES_SESSION_ID];

   /***** Update last publishing code *****/
   sprintf (Query,"UPDATE sessions"
	          " SET LastPubCod=(SELECT IFNULL(MAX(PubCod),0) FROM social_pubs)"
	          " WHERE SessionId='%s'",
	    Gbl.Session.Id);
   DB_QueryUPDATE (Query,"can not update last publishing code into session");
  }

/*****************************************************************************/
/*********************** Update first publishing code ************************/
/*****************************************************************************/

static void Soc_UpdateFirstPubCodIntoSession (long FirstPubCod)
  {
   char Query[128 + Ses_BYTES_SESSION_ID];

   /***** Update last publishing code *****/
   sprintf (Query,"UPDATE sessions SET FirstPubCod=%ld WHERE SessionId='%s'",
	    FirstPubCod,Gbl.Session.Id);
   DB_QueryUPDATE (Query,"can not update first publishing code into session");
  }

/*****************************************************************************/
/*************** Drop temporary tables used to query timeline ****************/
/*****************************************************************************/

static void Soc_DropTemporaryTablesUsedToQueryTimeline (void)
  {
   char Query[128];

   sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS"
	          " pub_codes,not_codes,publishers,current_timeline");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");
  }

/*****************************************************************************/
/*********************** Show social activity (timeline) *********************/
/*****************************************************************************/
/*             _____
            / |_____| just_now_timeline_list (Posts retrieved automatically
           |  |_____|                         via AJAX from time to time.
           |  |_____|                         They are transferred inmediately
           |     |                            to new_timeline_list.)
  Hidden  <    __v__
           |  |_____| new_timeline_list (Posts retrieved but hidden.
           |  |_____|                    When user clicks to view them,
           |  |_____|                    they are transferred
            \ |_____|                    to visible timeline_list.)
                 |
               __v__
            / |_____| timeline_list (Posts visible on page)
           |  |_____|
  Visible  |  |_____|
    on    <   |_____|
   page    |  |_____|
           |  |_____|
            \ |_____|
                 ^
               __|__
            / |_____| old_timeline_list (Posts just retrieved via AJAX
           |  |_____|                    when user clicks "see more".
           |  |_____|                    They are transferred inmediately
  Hidden  <   |_____|                    to timeline_list.)
           |  |_____|
           |  |_____|
            \ |_____|
*/
static void Soc_ShowTimeline (const char *Query,const char *Title,
                              long NotCodToHighlight)
  {
   extern const char *Hlp_SOCIAL_Timeline;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPub;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   bool GlobalTimeline = (Gbl.Usrs.Other.UsrDat.UsrCod <= 0);
   bool ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Get publishings from database *****/
   NumPubsGot = DB_QuerySELECT (Query,&mysql_res,"can not get timeline");

   /***** Start box *****/
   Box_StartBox (Soc_WIDTH_TIMELINE,Title,Soc_PutIconsTimeline,
                 Hlp_SOCIAL_Timeline,Box_NOT_CLOSABLE);

   /***** Put form to select users whom public activity is displayed *****/
   if (GlobalTimeline)
      Soc_PutFormWhichUsrs ();

   /***** Form to write a new post *****/
   if (GlobalTimeline || ItsMe)
      Soc_PutFormToWriteNewPost ();

   /***** New publishings refreshed dynamically via AJAX *****/
   if (GlobalTimeline)
     {
      /* Link to view new publishings via AJAX */
      Soc_PutLinkToViewNewPublishings ();

      /* Hidden list where insert just received (not visible) publishings via AJAX */
      fprintf (Gbl.F.Out,"<ul id=\"just_now_timeline_list\"></ul>");

      /* Hidden list where insert new (not visible) publishings via AJAX */
      fprintf (Gbl.F.Out,"<ul id=\"new_timeline_list\"></ul>");
     }

   /***** List recent publishings in timeline *****/
   fprintf (Gbl.F.Out,"<ul id=\"timeline_list\" class=\"LIST_LEFT\">");

   for (NumPub = 0;
	NumPub < NumPubsGot;
	NumPub++)
     {
      /* Get data of social publishing */
      row = mysql_fetch_row (mysql_res);
      Soc_GetDataOfSocialPublishingFromRow (row,&SocPub);

      /* Get data of social note */
      SocNot.NotCod = SocPub.NotCod;
      Soc_GetDataOfSocialNotByCod (&SocNot);

      /* Write social note */
      Soc_WriteSocialNote (&SocNot,
                           SocPub.TopMessage,SocPub.PublisherCod,
			   SocNot.NotCod == NotCodToHighlight,
			   false);
     }
   fprintf (Gbl.F.Out,"</ul>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Store first publishing code into session *****/
   Soc_UpdateFirstPubCodIntoSession (SocPub.PubCod);

   if (NumPubsGot == Soc_MAX_REC_PUBS_TO_GET_AND_SHOW)
     {
      /***** Link to view old publishings via AJAX *****/
      Soc_PutLinkToViewOldPublishings ();

      /***** Hidden list where insert old publishings via AJAX *****/
      fprintf (Gbl.F.Out,"<ul id=\"old_timeline_list\"></ul>");
     }

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/********************* Put contextual icons in timeline **********************/
/*****************************************************************************/

static void Soc_PutIconsTimeline (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_SOCIAL_ACTIVITY;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/***************** Start a form in global or user timeline *******************/
/*****************************************************************************/

static void Soc_FormStart (Act_Action_t ActionGbl,Act_Action_t ActionUsr)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_StartFormAnchor (ActionUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
     {
      Act_StartForm (ActionGbl);
      Soc_PutParamWhichUsrs ();
     }
  }

/*****************************************************************************/
/******** Put form to select users whom public activity is displayed *********/
/*****************************************************************************/

static void Soc_PutFormWhichUsrs (void)
  {
   extern const char *Txt_TIMELINE_WHICH_USERS[Soc_NUM_WHICH_USRS];
   Soc_WhichUsrs_t WhichUsrs;

   /***** Form to select which users I want to see in timeline:
          - only the users I follow
          - all users *****/
   Act_StartForm (ActSeeSocTmlGbl);
   fprintf (Gbl.F.Out,"<div class=\"SEL_BELOW_TITLE\">"
	              "<ul>");

   for (WhichUsrs = (Soc_WhichUsrs_t) 0;
	WhichUsrs < Soc_NUM_WHICH_USRS;
	WhichUsrs++)
     {
      fprintf (Gbl.F.Out,"<li>"
                         "<label>"
                         "<input type=\"radio\" name=\"WhichUsrs\""
                         " value=\"%u\"",
               (unsigned) WhichUsrs);
      if (WhichUsrs == Gbl.Social.WhichUsrs)
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"document.getElementById('%s').submit();\" />"
	                 "%s"
                         "</label>"
                         "</li>",
               Gbl.Form.Id,Txt_TIMELINE_WHICH_USERS[WhichUsrs]);
     }
   fprintf (Gbl.F.Out,"</ul>"
	              "</div>");
   Act_EndForm ();

   /***** Show warning if I do not follow anyone *****/
   if (Gbl.Social.WhichUsrs == Soc_FOLLOWED)
      Soc_ShowWarningYouDontFollowAnyUser ();
  }

/*****************************************************************************/
/***** Put hidden parameter with which users to view in global timeline ******/
/*****************************************************************************/

static void Soc_PutParamWhichUsrs (void)
  {
   Par_PutHiddenParamUnsigned ("WhichUsrs",Gbl.Social.WhichUsrs);
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

static void Soc_GetParamsWhichUsrs (void)
  {
   /***** Get which users I want to see *****/
   Gbl.Social.WhichUsrs = (Soc_WhichUsrs_t)
	                   Par_GetParToUnsignedLong ("WhichUsrs",
                                                     0,
                                                     Soc_NUM_WHICH_USRS - 1,
                                                     (unsigned long) Soc_DEFAULT_WHICH_USRS);
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

static void Soc_ShowWarningYouDontFollowAnyUser (void)
  {
   extern const char *Txt_You_dont_follow_any_user;

   /***** Check if I follow someone *****/
   if (!Fol_GetNumFollowing (Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      /***** Show warning if I do not follow anyone *****/
      Ale_ShowAlert (Ale_WARNING,Txt_You_dont_follow_any_user);

      /***** Put link to show users to follow *****/
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Fol_PutLinkWhoToFollow ();
      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/********** Show new social activity (new publishings in timeline) ***********/
/*****************************************************************************/
// The publishings are inserted as list elements of a hidden list

static void Soc_InsertNewPubsInTimeline (const char *Query)
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
      Soc_GetDataOfSocialNotByCod (&SocNot);

      /* Write social note */
      Soc_WriteSocialNote (&SocNot,
                           SocPub.TopMessage,SocPub.PublisherCod,
                           false,false);
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
      Soc_GetDataOfSocialNotByCod (&SocNot);

      /* Write social note */
      Soc_WriteSocialNote (&SocNot,
                           SocPub.TopMessage,SocPub.PublisherCod,
                           false,false);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Store first publishing code into session *****/
   Soc_UpdateFirstPubCodIntoSession (SocPub.PubCod);
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

   /***** Animated link to view old publishings *****/
   fprintf (Gbl.F.Out,"<div id=\"view_old_posts_container\""
	              " class=\"SOCIAL_PUB VERY_LIGHT_BLUE\">"
                      "<a href=\"\" class=\"%s\" onclick=\""
   		      "document.getElementById('get_old_timeline').style.display='none';"	// Icon to be hidden on click
		      "document.getElementById('getting_old_timeline').style.display='';"	// Icon to be shown on click
                      "refreshOldTimeline();"
		      "return false;\">"
	              "<img id=\"get_old_timeline\""
	              " src=\"%s/recycle16x16.gif\" alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "<img id=\"getting_old_timeline\""
		      " src=\"%s/working16x16.gif\" alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" style=\"display:none;\" />"				// Animated icon hidden
		      "&nbsp;%s"
	              "</a>"
	              "</div>",
	    The_ClassFormBold[Gbl.Prefs.Theme],
	    Gbl.Prefs.IconsURL,Txt_See_more,Txt_See_more,
	    Gbl.Prefs.IconsURL,Txt_See_more,Txt_See_more,
	    Txt_See_more);
  }

/*****************************************************************************/
/***************************** Write social note *****************************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteSocialNote (const struct SocialNote *SocNot,
                                 Soc_TopMessage_t TopMessage,long UsrCod,
                                 bool Highlight,	// Highlight social note
                                 bool ShowNoteAlone)	// Social note is shown alone, not in a list
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData UsrDat;
   bool ItsMe;
   bool IAmTheAuthor = false;
   bool IAmASharerOfThisSocNot = false;
   bool IAmAFaverOfThisSocNot = false;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];
   unsigned NumComments;
   char IdNewComment[Act_MAX_BYTES_ID];

   /***** Start box ****/
   if (ShowNoteAlone)
     {
      Box_StartBox (Soc_WIDTH_TIMELINE,NULL,NULL,
                    NULL,Box_CLOSABLE);
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");
     }

   /***** Start list item *****/
   fprintf (Gbl.F.Out,"<li");
   if (!ShowNoteAlone || Highlight)
     {
      fprintf (Gbl.F.Out," class=\"");
      if (!ShowNoteAlone)
	 fprintf (Gbl.F.Out," SOCIAL_PUB");
      if (Highlight)
	 fprintf (Gbl.F.Out," SOCIAL_NEW_PUB");
      fprintf (Gbl.F.Out,"\"");
     }
   fprintf (Gbl.F.Out,">");

   if (SocNot->NotCod   <= 0 ||
       SocNot->NoteType == Soc_NOTE_UNKNOWN ||
       SocNot->UsrCod   <= 0)
      Ale_ShowAlert (Ale_ERROR,"Error in social note.");
   else
     {
      /***** Initialize location in hierarchy *****/
      Ins.InsCod = -1L;
      Ctr.CtrCod = -1L;
      Deg.DegCod = -1L;
      Crs.CrsCod = -1L;

      /***** Write sharer/commenter if distinct to author *****/
      Soc_WriteTopMessage (TopMessage,UsrCod);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get author data *****/
      UsrDat.UsrCod = SocNot->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      if (Gbl.Usrs.Me.Logged)
	{
	 ItsMe = Usr_ItsMe (UsrDat.UsrCod);
	 IAmTheAuthor = ItsMe;
	 if (!IAmTheAuthor)
	   {
	    IAmASharerOfThisSocNot = Soc_CheckIfNoteIsSharedByUsr (SocNot->NotCod,
								   Gbl.Usrs.Me.UsrDat.UsrCod);
	    IAmAFaverOfThisSocNot  = Soc_CheckIfNoteIsFavedByUsr  (SocNot->NotCod,
								   Gbl.Usrs.Me.UsrDat.UsrCod);
	   }
	}

      /***** Left: write author's photo *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_NOTE_LEFT_PHOTO\">");
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO42x56",Pho_ZOOM,true);	// Use unique id
      fprintf (Gbl.F.Out,"</div>");

      /***** Right: author's name, time, summary and buttons *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_NOTE_RIGHT_CONTAINER\">");

      /* Write author's full name and nickname */
      Soc_WriteAuthorNote (&UsrDat);

      /* Write date and time */
      Soc_WriteDateTime (SocNot->DateTimeUTC);

      /* Write content of the note */
      if (SocNot->NoteType == Soc_NOTE_SOCIAL_POST)
	 /* Write post content */
	 Soc_GetAndWriteSocialPost (SocNot->Cod);
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
		  For_GetForumTypeAndLocationOfAPost (SocNot->Cod,&Gbl.Forum.ForumSelected);
		  For_SetForumName (&Gbl.Forum.ForumSelected,
		                    ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
		  break;
	       default:
		  break;
	      }

	 /* Write note type */
	 Soc_PutFormGoToAction (SocNot);

	 /* Write location in hierarchy */
	 if (!SocNot->Unavailable)
	    switch (SocNot->NoteType)
	      {
	       case Soc_NOTE_INS_DOC_PUB_FILE:
	       case Soc_NOTE_INS_SHA_PUB_FILE:
		  /* Write location (institution) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Institution,Ins.ShrtName);
		  break;
	       case Soc_NOTE_CTR_DOC_PUB_FILE:
	       case Soc_NOTE_CTR_SHA_PUB_FILE:
		  /* Write location (centre) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Centre,Ctr.ShrtName);
		  break;
	       case Soc_NOTE_DEG_DOC_PUB_FILE:
	       case Soc_NOTE_DEG_SHA_PUB_FILE:
		  /* Write location (degree) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Degree,Deg.ShrtName);
		  break;
	       case Soc_NOTE_CRS_DOC_PUB_FILE:
	       case Soc_NOTE_CRS_SHA_PUB_FILE:
	       case Soc_NOTE_EXAM_ANNOUNCEMENT:
	       case Soc_NOTE_NOTICE:
		  /* Write location (course) in hierarchy */
		  fprintf (Gbl.F.Out,"<div class=\"DAT\">%s: %s</div>",
			   Txt_Course,Crs.ShrtName);
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
	 Soc_GetNoteSummary (SocNot,SummaryStr);
	 fprintf (Gbl.F.Out,"<div class=\"DAT\">%s</div>",SummaryStr);
	}

      /* End of right part */
      fprintf (Gbl.F.Out,"</div>");

      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_BOTTOM_LEFT\">");

      /* Create unique id for new comment */
      Act_SetUniqueId (IdNewComment);

      /* Get number of comments in this social note */
      NumComments = Soc_GetNumCommentsInSocialNote (SocNot->NotCod);

      /* Put icon to add a comment */
      // if (NumComments || SocNot->Unavailable)	// Unavailable social notes can not be commented
      if (SocNot->Unavailable)	// Unavailable social notes can not be commented
	 Soc_PutIconCommentDisabled ();
      else
         Soc_PutIconToToggleCommentSocialNote (IdNewComment);

      fprintf (Gbl.F.Out,"</div>");

      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_BOTTOM_RIGHT\">"
	                 "<div class=\"SOCIAL_ICOS_FAV_SHA_REM\">");

      /* Put icon to mark this social note as favourite */
      if (IAmTheAuthor)				// I am the author
	 Soc_PutDisabledIconFav (SocNot->NumFavs);
      else if (IAmAFaverOfThisSocNot)	// I have favourited this social note
	 /* Put icon to unfav this publishing */
	 Soc_PutFormToUnfavSocialNote (SocNot->NotCod);
      else					// I am not the author and I am not a sharer
	{
	 if (SocNot->Unavailable)		// Unavailable social notes can not be favourited
	    Soc_PutDisabledIconFav (SocNot->NumFavs);
	 else
	    /* Put icon to share this publishing */
	    Soc_PutFormToFavSocialNote (SocNot->NotCod);
	}

      /* Show who have marked this social note as favourite */
      Soc_ShowUsrsWhoHaveMarkedSocialNoteAsFav (SocNot);

      /* Put icons to share/unshare */
      if (IAmTheAuthor)				// I am the author
	 Soc_PutDisabledIconShare (SocNot->NumShared);
      else if (IAmASharerOfThisSocNot)	// I am a sharer of this social note,
					// but not the author ==> I have shared this social note
	 /* Put icon to unshare this publishing */
	 Soc_PutFormToUnshareSocialNote (SocNot->NotCod);
      else					// I am not the author and I am not a sharer
	{
	 if (SocNot->Unavailable)		// Unavailable social notes can not be shared
	    Soc_PutDisabledIconShare (SocNot->NumShared);
	 else
	    /* Put icon to share this publishing */
	    Soc_PutFormToShareSocialNote (SocNot->NotCod);
	}

      /* Show who have shared this social note */
      Soc_ShowUsrsWhoHaveSharedSocialNote (SocNot);

      /* Put icon to remove this social note */
      if (IAmTheAuthor)
	 Soc_PutFormToRemoveSocialPublishing (SocNot->NotCod);

      /* End of icon bar */
      fprintf (Gbl.F.Out,"</div>");

      /* Show comments */
      if (NumComments)
	 Soc_WriteCommentsInSocialNote (SocNot);

      /* End of bottom right */
      fprintf (Gbl.F.Out,"</div>");

      /* Put hidden form to write a new comment */
      Soc_PutHiddenFormToWriteNewCommentToSocialNote (SocNot->NotCod,IdNewComment);

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** End list item *****/
   fprintf (Gbl.F.Out,"</li>");

   /***** End box ****/
   if (ShowNoteAlone)
     {
      fprintf (Gbl.F.Out,"</ul>");
      Box_EndBox ();
     }
  }

/*****************************************************************************/
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteTopMessage (Soc_TopMessage_t TopMessage,long UsrCod)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *Txt_SOCIAL_NOTE_TOP_MESSAGES[Soc_NUM_TOP_MESSAGES];
   struct UsrData UsrDat;
   bool ItsMe = Usr_ItsMe (UsrCod);

   if (TopMessage != Soc_TOP_MESSAGE_NONE)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get user's data *****/
      UsrDat.UsrCod = UsrCod;
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))	// Really we only need EncryptedUsrCod and FullName
	{
	 fprintf (Gbl.F.Out,"<div class=\"SOCIAL_TOP_CONTAINER SOCIAL_TOP_PUBLISHER\">");

	 /***** Show user's name inside form to go to user's public profile *****/
	 Act_StartFormUnique (ActSeeOthPubPrf);
	 Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
	 Act_LinkFormSubmitUnique (ItsMe ? Txt_My_public_profile :
					   Txt_Another_user_s_profile,
				   "SOCIAL_TOP_PUBLISHER");
	 fprintf (Gbl.F.Out,"%s</a>",UsrDat.FullName);
	 Act_EndForm ();

	 /***** Show action made *****/
         fprintf (Gbl.F.Out," %s:</div>",
                  Txt_SOCIAL_NOTE_TOP_MESSAGES[TopMessage]);
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
  }

/*****************************************************************************/
/************ Write name and nickname of author of a social note *************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteAuthorNote (const struct UsrData *UsrDat)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_AUTHOR\">");

   /***** Show user's name inside form to go to user's public profile *****/
   Act_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (ItsMe ? Txt_My_public_profile :
				     Txt_Another_user_s_profile,
			     "DAT_N_BOLD");
   fprintf (Gbl.F.Out,"%s</a>",UsrDat->FullName);
   Act_EndForm ();

   /***** Show user's nickname inside form to go to user's public profile *****/
   Act_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (ItsMe ? Txt_My_public_profile :
				     Txt_Another_user_s_profile,
			     "DAT_LIGHT");
   fprintf (Gbl.F.Out," @%s</a>",UsrDat->Nickname);
   Act_EndForm ();

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/**************** Write the date of creation of a social note ****************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

static void Soc_WriteDateTime (time_t TimeUTC)
  {
   extern const char *Txt_Today;
   char IdDateTime[Act_MAX_BYTES_ID];

   /***** Create unique Id *****/
   Act_SetUniqueId (IdDateTime);

   /***** Container where the date-time is written *****/
   fprintf (Gbl.F.Out,"<div id=\"%s\" class=\"SOCIAL_RIGHT_TIME DAT_LIGHT\">"
	              "</div>",
            IdDateTime);

   /***** Script to write date and time in browser local time *****/
   // This must be out of the div where the output is written
   // because it will be evaluated in a loop in JavaScript
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
		      "writeLocalDateHMSFromUTC('%s',%ld,"
		      "%u,',&nbsp;','%s',true,false,0x6);"
                      "</script>",
            IdDateTime,(long) TimeUTC,
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
  }

/*****************************************************************************/
/***************** Get from database and write public post *******************/
/*****************************************************************************/

static void Soc_GetAndWriteSocialPost (long PstCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Image Image;

   /***** Initialize image *****/
   Img_ImageConstructor (&Image);

   /***** Get social post from database *****/
   sprintf (Query,"SELECT Content,ImageName,ImageTitle,ImageURL"
	          " FROM social_posts WHERE PstCod=%ld",
            PstCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the content of a social post");

   /***** Result should have a unique row *****/
   if (NumRows == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /****** Get content (row[0]) *****/
      Str_Copy (Content,row[0],
                Cns_MAX_BYTES_LONG_TEXT);

      /****** Get image name (row[1]), title (row[2]) and URL (row[3]) *****/
      Img_GetImageNameTitleAndURLFromRow (row[1],row[2],row[3],&Image);
     }
   else
      Content[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Write content *****/
   if (Content[0])
     {
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_TXT\">");
      Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Show image *****/
   Img_ShowImage (&Image,"SOCIAL_POST_IMG_CONTAINER","SOCIAL_POST_IMG");

   /***** Free image *****/
   Img_ImageDestructor (&Image);
  }

/*****************************************************************************/
/********* Put form to go to an action depending on the social note **********/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormGoToAction (const struct SocialNote *SocNot)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_SOCIAL_NOTE[Soc_NUM_NOTE_TYPES];
   extern const char *Txt_not_available;
   char Class[64];
   const Act_Action_t Soc_DefaultActions[Soc_NUM_NOTE_TYPES] =
     {
      ActUnk,			// Soc_NOTE_UNKNOWN

      /* Institution tab */
      ActReqDatSeeDocIns,	// Soc_NOTE_INS_DOC_PUB_FILE
      ActReqDatShaIns,		// Soc_NOTE_INS_SHA_PUB_FILE

      /* Centre tab */
      ActReqDatSeeDocCtr,	// Soc_NOTE_CTR_DOC_PUB_FILE
      ActReqDatShaCtr,		// Soc_NOTE_CTR_SHA_PUB_FILE

      /* Degree tab */
      ActReqDatSeeDocDeg,	// Soc_NOTE_DEG_DOC_PUB_FILE
      ActReqDatShaDeg,		// Soc_NOTE_DEG_SHA_PUB_FILE

      /* Course tab */
      ActReqDatSeeDocCrs,	// Soc_NOTE_CRS_DOC_PUB_FILE
      ActReqDatShaCrs,		// Soc_NOTE_CRS_SHA_PUB_FILE

      /* Assessment tab */
      ActSeeOneExaAnn,		// Soc_NOTE_EXAM_ANNOUNCEMENT

      /* Users tab */

      /* Social tab */
      ActUnk,			// Soc_NOTE_SOCIAL_POST (action not used)
      ActSeeFor,		// Soc_NOTE_FORUM_POST

      /* Messages tab */
      ActSeeOneNot,		// Soc_NOTE_NOTICE

      /* Statistics tab */

      /* Profile tab */

     };
   const char *Soc_Icons[Soc_NUM_NOTE_TYPES] =
     {
      NULL,			// Soc_NOTE_UNKNOWN

      /* Institution tab */
      "file64x64.gif",		// Soc_NOTE_INS_DOC_PUB_FILE
      "file64x64.gif",		// Soc_NOTE_INS_SHA_PUB_FILE

      /* Centre tab */
      "file64x64.gif",		// Soc_NOTE_CTR_DOC_PUB_FILE
      "file64x64.gif",		// Soc_NOTE_CTR_SHA_PUB_FILE

      /* Degree tab */
      "file64x64.gif",		// Soc_NOTE_DEG_DOC_PUB_FILE
      "file64x64.gif",		// Soc_NOTE_DEG_SHA_PUB_FILE

      /* Course tab */
      "file64x64.gif",		// Soc_NOTE_CRS_DOC_PUB_FILE
      "file64x64.gif",		// Soc_NOTE_CRS_SHA_PUB_FILE

      /* Assessment tab */
      "announce64x64.gif",	// Soc_NOTE_EXAM_ANNOUNCEMENT

      /* Users tab */

      /* Social tab */
      NULL,			// Soc_NOTE_SOCIAL_POST (icon not used)
      "forum64x64.gif",		// Soc_NOTE_FORUM_POST

      /* Messages tab */
      "notice64x64.png",	// Soc_NOTE_NOTICE

      /* Statistics tab */

      /* Profile tab */

     };

   if (SocNot->Unavailable ||	// File/notice... pointed by this social note is unavailable
       Gbl.Form.Inside)		// Inside another form
     {
      /***** Do not put form *****/
      fprintf (Gbl.F.Out,"<div class=\"DAT_LIGHT\">%s",
               Txt_SOCIAL_NOTE[SocNot->NoteType]);
      if (SocNot->Unavailable)
         fprintf (Gbl.F.Out," (%s)",Txt_not_available);
      fprintf (Gbl.F.Out,"</div>");
     }
   else			// Not inside another form
     {
      fprintf (Gbl.F.Out,"<div>");

      /***** Parameters depending on the type of note *****/
      switch (SocNot->NoteType)
	{
	 case Soc_NOTE_INS_DOC_PUB_FILE:
	 case Soc_NOTE_INS_SHA_PUB_FILE:
	    Act_StartFormUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentIns.Ins.InsCod)	// Not the current institution
	       Ins_PutParamInsCod (SocNot->HieCod);		// Go to another institution
	    break;
	 case Soc_NOTE_CTR_DOC_PUB_FILE:
	 case Soc_NOTE_CTR_SHA_PUB_FILE:
	    Act_StartFormUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCtr.Ctr.CtrCod)	// Not the current centre
	       Ctr_PutParamCtrCod (SocNot->HieCod);		// Go to another centre
	    break;
	 case Soc_NOTE_DEG_DOC_PUB_FILE:
	 case Soc_NOTE_DEG_SHA_PUB_FILE:
	    Act_StartFormUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentDeg.Deg.DegCod)	// Not the current degree
	       Deg_PutParamDegCod (SocNot->HieCod);		// Go to another degree
	    break;
	 case Soc_NOTE_CRS_DOC_PUB_FILE:
	 case Soc_NOTE_CRS_SHA_PUB_FILE:
	    Act_StartFormUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_EXAM_ANNOUNCEMENT:
	    Act_StartFormUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Exa_PutHiddenParamExaCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_SOCIAL_POST:	// Not applicable
	    return;
	 case Soc_NOTE_FORUM_POST:
	    Act_StartFormUnique (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type]);
	    For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                         1,	// Page of posts   = first
                                         Gbl.Forum.ForumSet,
					 Gbl.Forum.ThreadsOrder,
					 Gbl.Forum.ForumSelected.Location,
					 Gbl.Forum.ForumSelected.ThrCod,
					 -1L);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_NOTICE:
	    Act_StartFormUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Not_PutHiddenParamNotCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 default:			// Not applicable
	    return;
	}

      /***** Link and end form *****/
      sprintf (Class,"%s ICO_HIGHLIGHT",The_ClassFormBold[Gbl.Prefs.Theme]);
      Act_LinkFormSubmitUnique (Txt_SOCIAL_NOTE[SocNot->NoteType],Class);
      fprintf (Gbl.F.Out,"<img src=\"%s/%s\""
	                 " alt=\"%s\" title=\"%s\""
	                 " class=\"ICO20x20\" />"
	                 "&nbsp;%s"
	                 "</a>",
            Gbl.Prefs.IconsURL,Soc_Icons[SocNot->NoteType],
            Txt_SOCIAL_NOTE[SocNot->NoteType],
            Txt_SOCIAL_NOTE[SocNot->NoteType],
            Txt_SOCIAL_NOTE[SocNot->NoteType]);
      Act_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
  }

/*****************************************************************************/
/******************* Get social note summary and content *********************/
/*****************************************************************************/

static void Soc_GetNoteSummary (const struct SocialNote *SocNot,
                                char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1])
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
	 Brw_GetSummaryAndContentOfFile (SummaryStr,NULL,SocNot->Cod,false);
         break;
      case Soc_NOTE_EXAM_ANNOUNCEMENT:
         Exa_GetSummaryAndContentExamAnnouncement (SummaryStr,NULL,SocNot->Cod,false);
         break;
      case Soc_NOTE_SOCIAL_POST:
	 // Not applicable
         break;
      case Soc_NOTE_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,SocNot->Cod,false);
         break;
      case Soc_NOTE_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,SocNot->Cod,false);
         break;
     }
  }

/*****************************************************************************/
/************** Store and publish a social note into database ****************/
/*****************************************************************************/
// Return the code of the new note just created

void Soc_StoreAndPublishSocialNote (Soc_NoteType_t NoteType,long Cod,struct SocialPublishing *SocPub)
  {
   char Query[256];
   long HieCod;	// Hierarchy code (institution/centre/degree/course)

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
	          " (NoteType,Cod,UsrCod,HieCod,Unavailable,TimeNote)"
                  " VALUES"
                  " (%u,%ld,%ld,%ld,'N',NOW())",
            (unsigned) NoteType,Cod,Gbl.Usrs.Me.UsrDat.UsrCod,HieCod);
   SocPub->NotCod = DB_QueryINSERTandReturnCode (Query,"can not create new social note");

   /***** Publish social note in timeline *****/
   SocPub->PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   SocPub->PubType      = Soc_PUB_ORIGINAL_NOTE;
   Soc_PublishSocialNoteInTimeline (SocPub);
  }

/*****************************************************************************/
/********************** Mark a social note as unavailable ********************/
/*****************************************************************************/

void Soc_MarkSocialNoteAsUnavailableUsingNotCod (long NotCod)
  {
   char Query[256];

   /***** Mark the social note as unavailable *****/
   sprintf (Query,"UPDATE social_notes SET Unavailable='Y'"
		  " WHERE NotCod=%ld",
	    NotCod);
   DB_QueryUPDATE (Query,"can not mark social note as unavailable");
  }

void Soc_MarkSocialNoteAsUnavailableUsingNoteTypeAndCod (Soc_NoteType_t NoteType,long Cod)
  {
   char Query[256];

   /***** Mark the social note as unavailable *****/
   sprintf (Query,"UPDATE social_notes SET Unavailable='Y'"
		  " WHERE NoteType=%u AND Cod=%ld",
	    (unsigned) NoteType,Cod);
   DB_QueryUPDATE (Query,"can not mark social note as unavailable");
  }

/*****************************************************************************/
/************** Mark social notes of one file as unavailable *****************/
/*****************************************************************************/

void Soc_MarkSocialNoteOneFileAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;
   Soc_NoteType_t NoteType;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_SHR_CRS:
         /***** Get file code *****/
	 FilCod = Brw_GetFilCodByPath (Path,true);	// Only if file is public
	 if (FilCod > 0)
	   {
	    /***** Mark possible social note as unavailable *****/
	    switch (FileBrowser)
	      {
	       case Brw_ADMI_DOC_INS:
		  NoteType = Soc_NOTE_INS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_INS:
		  NoteType = Soc_NOTE_INS_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_CTR:
		  NoteType = Soc_NOTE_CTR_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_CTR:
		  NoteType = Soc_NOTE_CTR_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_DEG:
		  NoteType = Soc_NOTE_DEG_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_DEG:
		  NoteType = Soc_NOTE_DEG_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_CRS:
		  NoteType = Soc_NOTE_CRS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_CRS:
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
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_SHR_CRS:
	 /***** Mark possible social note as unavailable *****/
	 switch (FileBrowser)
	   {
	    case Brw_ADMI_DOC_INS:
	       NoteType = Soc_NOTE_INS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_INS:
	       NoteType = Soc_NOTE_INS_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_CTR:
	       NoteType = Soc_NOTE_CTR_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_CTR:
	       NoteType = Soc_NOTE_CTR_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_DEG:
	       NoteType = Soc_NOTE_DEG_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_DEG:
	       NoteType = Soc_NOTE_DEG_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_CRS:
	       NoteType = Soc_NOTE_CRS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_CRS:
	       NoteType = Soc_NOTE_CRS_SHA_PUB_FILE;
	       break;
	    default:
	       return;
	   }
         sprintf (Query,"UPDATE social_notes SET Unavailable='Y'"
		        " WHERE NoteType=%u AND Cod IN"
	                " (SELECT FilCod FROM files"
			" WHERE FileBrowser=%u AND Cod=%ld"
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
   sprintf (Query,"INSERT INTO social_pubs"
	          " (NotCod,PublisherCod,PubType,TimePublish)"
                  " VALUES"
                  " (%ld,%ld,%u,NOW())",
            SocPub->NotCod,
            SocPub->PublisherCod,
            (unsigned) SocPub->PubType);
   SocPub->PubCod = DB_QueryINSERTandReturnCode (Query,"can not publish social note");
  }

/*****************************************************************************/
/****************** Form to write a new social publishing ********************/
/*****************************************************************************/

static void Soc_PutFormToWriteNewPost (void)
  {
   extern const char *Txt_New_SOCIAL_post;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   /***** Start list *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">"
                      "<li>");

   /***** Left: write author's photo (my photo) *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_NOTE_LEFT_PHOTO\">");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
						     NULL,
		     "PHOTO42x56",Pho_ZOOM,false);
   fprintf (Gbl.F.Out,"</div>");

   /***** Right: author's name, time, summary and buttons *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_NOTE_RIGHT_CONTAINER\">");

   /* Write author's full name and nickname */
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_AUTHOR\">"
		      "<span class=\"DAT_N_BOLD\">%s</span>"
		      "<span class=\"DAT_LIGHT\"> @%s</span>"
		      "</div>",
	    Gbl.Usrs.Me.UsrDat.FullName,Gbl.Usrs.Me.UsrDat.Nickname);

   /***** Form to write the post *****/
   /* Start container */
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_FORM_NEW_POST\">");

   /* Start form to write the post */
   Soc_FormStart (ActRcvSocPstGbl,ActRcvSocPstUsr);

   /* Textarea and button */
   Soc_PutTextarea (Txt_New_SOCIAL_post,
                    "SOCIAL_TEXTAREA_POST","SOCIAL_POST_IMG_TIT_URL");

   /* End form */
   Act_EndForm ();

   /* End container */
   fprintf (Gbl.F.Out,"</div>");

   /***** End list *****/
   fprintf (Gbl.F.Out,"</li>"
	              "</ul>");
  }

/*****************************************************************************/
/*** Put textarea and button inside a form to submit a new post or comment ***/
/*****************************************************************************/

static void Soc_PutTextarea (const char *Placeholder,
                             const char *ClassTextArea,const char *ClassImgTit)
  {
   extern const char *Txt_Post;
   char IdDivImgButton[Act_MAX_BYTES_ID];

   /***** Set unique id for the hidden div *****/
   Act_SetUniqueId (IdDivImgButton);

   /***** Textarea to write the content *****/
   fprintf (Gbl.F.Out,"<textarea name=\"Content\" rows=\"1\" maxlength=\"%u\""
                      " placeholder=\"%s&hellip;\""
	              " class=\"%s\""
	              " onfocus=\"expandTextarea(this,'%s','5');\">"
		      "</textarea>",
            Soc_MAX_CHARS_IN_POST,
            Placeholder,ClassTextArea,
            IdDivImgButton);

   /***** Start concealable div *****/
   fprintf (Gbl.F.Out,"<div id=\"%s\" style=\"display:none;\">",
            IdDivImgButton);

   /***** Help on editor *****/
   Lay_HelpPlainEditor ();

   /***** Attached image (optional) *****/
   Img_PutImageUploader (-1,ClassImgTit);

   /***** Submit button *****/
   fprintf (Gbl.F.Out,"<button type=\"submit\""
	              " class=\"BT_SUBMIT_INLINE BT_CREATE\">"
		      "%s"
		      "</button>",
	    Txt_Post);

   /***** End hidden div *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******************* Receive and store a new public post *********************/
/*****************************************************************************/

void Soc_ReceiveSocialPostGbl (void)
  {
   long NotCod;

   /***** Receive and store social post *****/
   NotCod = Soc_ReceiveSocialPost ();

   /***** Write updated timeline after publishing (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_ReceiveSocialPostUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Receive and store social post *****/
   NotCod = Soc_ReceiveSocialPost ();

   /***** Write updated timeline after publishing (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

// Returns the code of the social note just created
static long Soc_ReceiveSocialPost (void)
  {
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Image Image;
   char *Query;
   long PstCod;
   struct SocialPublishing SocPub;

   /***** Get the content of the new post *****/
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /***** Initialize image *****/
   Img_ImageConstructor (&Image);

   /***** Get attached image (action, file and title) *****/
   Image.Width   = Soc_IMAGE_SAVED_MAX_WIDTH;
   Image.Height  = Soc_IMAGE_SAVED_MAX_HEIGHT;
   Image.Quality = Soc_IMAGE_SAVED_QUALITY;
   Img_GetImageFromForm (-1,&Image,NULL);

   if (Content[0] ||	// Text not empty
       Image.Name[0])	// An image is attached
     {
      /***** Allocate space for query *****/
      if ((Query = (char *) malloc (256 +
			            strlen (Content) +
			            Img_BYTES_NAME +
			            Img_MAX_BYTES_TITLE +
                                    Cns_MAX_BYTES_WWW)) == NULL)
	 Lay_ShowErrorAndExit ("Not enough memory to store database query.");

      /***** Check if image is received and processed *****/
      if (Image.Action == Img_ACTION_NEW_IMAGE &&	// Upload new image
	  Image.Status == Img_FILE_PROCESSED)	// The new image received has been processed
	 /* Move processed image to definitive directory */
	 Img_MoveImageToDefinitiveDirectory (&Image);

      /***** Publish *****/
      /* Insert post content in the database */
      sprintf (Query,"INSERT INTO social_posts"
	             " (Content,ImageName,ImageTitle,ImageURL)"
	             " VALUES"
	             " ('%s','%s','%s','%s')",
	       Content,
	       Image.Name,
	       (Image.Name[0] &&	// Save image title only if image attached
		Image.Title) ? Image.Title : "",
	       (Image.Name[0] &&	// Save image URL   only if image attached
		Image.URL  ) ? Image.URL   : "");
      PstCod = DB_QueryINSERTandReturnCode (Query,"can not create post");

      /* Insert post in social notes */
      Soc_StoreAndPublishSocialNote (Soc_NOTE_SOCIAL_POST,PstCod,&SocPub);

      /***** Free space used for query *****/
      free ((void *) Query);

      /***** Analyze content and store notifications about mentions *****/
      Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (SocPub.PubCod,Content);
     }
   else	// Text and image are empty
      SocPub.NotCod = -1L;

   /***** Free image *****/
   Img_ImageDestructor (&Image);

   return SocPub.NotCod;
  }

/*****************************************************************************/
/****** Put an icon to toggle on/off the form to comment a social note *******/
/*****************************************************************************/

static void Soc_PutIconToToggleCommentSocialNote (const char UniqueId[Act_MAX_BYTES_ID])
  {
   extern const char *Txt_Comment;

   /***** Link to toggle on/off the form to comment a social note *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_COMMENT ICO_HIGHLIGHT\">"
                      "<a href=\"\""
                      " onclick=\"toggleDisplay('%s');return false;\" />"
                      "<img src=\"%s/write64x64.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "</a>"
                      "</div>",
            UniqueId,
            Gbl.Prefs.IconsURL,
            Txt_Comment,Txt_Comment);
  }

/*****************************************************************************/
/****** Put an icon to toggle on/off the form to comment a social note *******/
/*****************************************************************************/

static void Soc_PutIconCommentDisabled (void)
  {
   extern const char *Txt_Comment;

   /***** Disabled icon to comment a social note *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_COMMENT_DISABLED\">"
 		      "<img src=\"%s/write64x64.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Comment,Txt_Comment);
  }

/*****************************************************************************/
/******************* Form to comment a social publishing *********************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutHiddenFormToWriteNewCommentToSocialNote (long NotCod,
                                                            const char IdNewComment[Act_MAX_BYTES_ID])
  {
   extern const char *Txt_New_SOCIAL_comment;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div id=\"%s\""
		      " class=\"SOCIAL_FORM_NEW_COMMENT\""
		      " style=\"display:none;\">",
	    IdNewComment);

   /***** Left: write author's photo (my photo) *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_PHOTO\">");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
					             NULL,
		     "PHOTO30x40",Pho_ZOOM,true);	// Use unique id
   fprintf (Gbl.F.Out,"</div>");

   /***** Right: form to write the comment *****/
   /* Start right container */
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_RIGHT_CONTAINER\">");

   /* Start form to write the post */
   Soc_FormStart (ActRcvSocComGbl,ActRcvSocComUsr);
   Soc_PutHiddenParamNotCod (NotCod);

   /* Textarea and button */
   Soc_PutTextarea (Txt_New_SOCIAL_comment,
                    "SOCIAL_TEXTAREA_COMMENT","SOCIAL_COMMENT_IMG_TIT_URL");

   /* End form */
   Act_EndForm ();

   /* End right container */
   fprintf (Gbl.F.Out,"</div>");

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Get number of comments in a social note ******************/
/*****************************************************************************/

static unsigned long Soc_GetNumCommentsInSocialNote (long NotCod)
  {
   char Query[128];

   sprintf (Query,"SELECT COUNT(*) FROM social_pubs"
	          " WHERE NotCod=%ld AND PubType=%u",
	    NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   return DB_QueryCOUNT (Query,"can not get number of comments in a social note");
  }

/*****************************************************************************/
/******************* Form to comment a social publishing *********************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteCommentsInSocialNote (const struct SocialNote *SocNot)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumComments;
   unsigned long NumCom;
   struct SocialComment SocCom;

   /***** Get comments of this social note from database *****/
   sprintf (Query,"SELECT social_pubs.PubCod,social_pubs.PublisherCod,"
		  "social_pubs.NotCod,"
		  "UNIX_TIMESTAMP(social_pubs.TimePublish),"
		  "social_comments.Content,"
		  "social_comments.ImageName,"
		  "social_comments.ImageTitle,"
		  "social_comments.ImageURL"
		  " FROM social_pubs,social_comments"
		  " WHERE social_pubs.NotCod=%ld"
                  " AND social_pubs.PubType=%u"
		  " AND social_pubs.PubCod=social_comments.PubCod"
		  " ORDER BY social_pubs.PubCod",
	    SocNot->NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   NumComments = DB_QuerySELECT (Query,&mysql_res,"can not get social comments");

   /***** List comments *****/
   if (NumComments)	// Comments to this social note found
     {
      /***** Start list *****/
      fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");

      /***** List comments one by one *****/
      for (NumCom = 0;
	   NumCom < NumComments;
	   NumCom++)
	{
	 /* Initialize image */
	 Img_ImageConstructor (&SocCom.Image);

	 /* Get data of social comment */
	 row = mysql_fetch_row (mysql_res);
	 Soc_GetDataOfSocialCommentFromRow (row,&SocCom);

	 /* Write social comment */
	 Soc_WriteSocialComment (&SocCom,
	                         Soc_TOP_MESSAGE_NONE,-1L,
	                         false);

	 /* Free image */
	 Img_ImageDestructor (&SocCom.Image);
	}

      /***** End list *****/
      fprintf (Gbl.F.Out,"</ul>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** Write social comment ***************************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteSocialComment (struct SocialComment *SocCom,
                                    Soc_TopMessage_t TopMessage,long UsrCod,
                                    bool ShowCommentAlone)	// Social comment is shown alone, not in a list
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData UsrDat;
   bool ItsMe;
   bool IAmTheAuthor;
   bool IAmAFaverOfThisSocCom = false;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];

   if (ShowCommentAlone)
     {
      Box_StartBox (Soc_WIDTH_TIMELINE,NULL,NULL,
                    NULL,Box_NOT_CLOSABLE);

      /***** Write sharer/commenter if distinct to author *****/
      Soc_WriteTopMessage (TopMessage,UsrCod);

      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_NOTE_LEFT_PHOTO\">"
                         "</div>"
                         "<div class=\"SOCIAL_NOTE_RIGHT_CONTAINER\">"
                         "<ul class=\"LIST_LEFT\">");
     }

   /***** Start list item *****/
   fprintf (Gbl.F.Out,"<li");
   if (!ShowCommentAlone)
      fprintf (Gbl.F.Out," class=\"SOCIAL_COMMENT\"");
   fprintf (Gbl.F.Out,">");

   if (SocCom->PubCod <= 0 ||
       SocCom->NotCod <= 0 ||
       SocCom->UsrCod <= 0)
      Ale_ShowAlert (Ale_ERROR,"Error in social comment.");
   else
     {
      /***** Get author's data *****/
      Usr_UsrDataConstructor (&UsrDat);
      UsrDat.UsrCod = SocCom->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      ItsMe = Usr_ItsMe (UsrDat.UsrCod);
      IAmTheAuthor = ItsMe;
      if (!IAmTheAuthor)
	 IAmAFaverOfThisSocCom = Soc_CheckIfCommIsFavedByUsr (SocCom->PubCod,
							      Gbl.Usrs.Me.UsrDat.UsrCod);

      /***** Left: write author's photo *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_PHOTO\">");
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO30x40",Pho_ZOOM,true);	// Use unique id
      fprintf (Gbl.F.Out,"</div>");

      /***** Right: author's name, time, content, image and buttons *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_RIGHT_CONTAINER\">");

      /* Write author's full name and nickname */
      Soc_WriteAuthorComment (&UsrDat);

      /* Write date and time */
      Soc_WriteDateTime (SocCom->DateTimeUTC);

      /* Write content of the social comment */
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_TXT\">");
      Msg_WriteMsgContent (SocCom->Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
      fprintf (Gbl.F.Out,"</div>");

      /* Show image */
      Img_ShowImage (&SocCom->Image,"SOCIAL_COMMENT_IMG_CONTAINER","SOCIAL_COMMENT_IMG");

      /* Put icon to mark this social comment as favourite */
      if (IAmTheAuthor)				// I am the author
	 Soc_PutDisabledIconFav (SocCom->NumFavs);
      else if (IAmAFaverOfThisSocCom)	// I have favourited this social note
	 /* Put icon to unfav this publishing */
	 Soc_PutFormToUnfavSocialComment (SocCom->PubCod);
      else					// I am not the author and I am not a favouriter
         /* Put icon to mark this social comment as favourite */
	 Soc_PutFormToFavSocialComment (SocCom->PubCod);

      /* Show who have marked this social comment as favourite */
      Soc_ShowUsrsWhoHaveMarkedSocialCommAsFav (SocCom);

      /* Put icon to remove this social comment */
      if (IAmTheAuthor && !ShowCommentAlone)
	 Soc_PutFormToRemoveComment (SocCom->PubCod);

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** End list item *****/
   fprintf (Gbl.F.Out,"</li>");

   if (ShowCommentAlone)
     {
      fprintf (Gbl.F.Out,"</ul>"
                         "</div>");	// SOCIAL_NOTE_RIGHT_CONTAINER
      Box_EndBox ();
     }
  }

/*****************************************************************************/
/****** Write name and nickname of author of a comment to a social note ******/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteAuthorComment (struct UsrData *UsrDat)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_RIGHT_AUTHOR\">");

   /***** Show user's name inside form to go to user's public profile *****/
   Act_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (ItsMe ? Txt_My_public_profile :
				     Txt_Another_user_s_profile,
			     "DAT_BOLD");
   fprintf (Gbl.F.Out,"%s</a>",UsrDat->FullName);
   Act_EndForm ();

   /***** Show user's nickname inside form to go to user's public profile *****/
   Act_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (ItsMe ? Txt_My_public_profile :
				     Txt_Another_user_s_profile,
			     "DAT_LIGHT");
   fprintf (Gbl.F.Out," @%s</a>",UsrDat->Nickname);
   Act_EndForm ();

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Form to remove social comment ************************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToRemoveComment (long PubCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social publishing *****/
   Soc_FormStart (ActReqRemSocComGbl,ActReqRemSocComUsr);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_REMOVE ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/remove-on64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
   Act_EndForm ();
  }

/*****************************************************************************/
/***************** Form to mark a social comment as favourite ****************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToFavSocialComment (long PubCod)
  {
   extern const char *Txt_Mark_as_favourite;

   /***** Form to mark social comment as favourite *****/
   Soc_FormStart (ActFavSocComGbl,ActFavSocComUsr);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_FAV ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/fav64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Mark_as_favourite,Txt_Mark_as_favourite);
   Act_EndForm ();
  }

/*****************************************************************************/
/*********************** Put disabled icon to share **************************/
/*****************************************************************************/

static void Soc_PutDisabledIconShare (unsigned NumShared)
  {
   extern const char *Txt_SOCIAL_NOTE_Shared_by_X_USERS;
   extern const char *Txt_SOCIAL_NOTE_Not_shared_by_anyone;

   if (NumShared)
      sprintf (Gbl.Title,Txt_SOCIAL_NOTE_Shared_by_X_USERS,NumShared);
   else
      Str_Copy (Gbl.Title,Txt_SOCIAL_NOTE_Not_shared_by_anyone,
                Lay_MAX_BYTES_TITLE);

   /***** Disabled icon to share *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_SHARE_DISABLED\">"
		      "<img src=\"%s/share64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Gbl.Title,Gbl.Title);
  }

/*****************************************************************************/
/****************** Put disabled icon to mark as favourite *******************/
/*****************************************************************************/

static void Soc_PutDisabledIconFav (unsigned NumFavs)
  {
   extern const char *Txt_SOCIAL_NOTE_Favourited_by_X_USERS;
   extern const char *Txt_SOCIAL_NOTE_Not_favourited_by_anyone;

   if (NumFavs)
      sprintf (Gbl.Title,Txt_SOCIAL_NOTE_Favourited_by_X_USERS,NumFavs);
   else
      Str_Copy (Gbl.Title,Txt_SOCIAL_NOTE_Not_favourited_by_anyone,
                Lay_MAX_BYTES_TITLE);

   /***** Disabled icon to mark as favourite *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_FAV_DISABLED\">"
		      "<img src=\"%s/fav64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Gbl.Title,Gbl.Title);
  }

/*****************************************************************************/
/************************* Form to share social note *************************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToShareSocialNote (long NotCod)
  {
   extern const char *Txt_Share;

   /***** Form to share social note *****/
   Soc_FormStart (ActShaSocNotGbl,ActShaSocNotUsr);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_SHARE ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/share64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Share,Txt_Share);
   Act_EndForm ();
  }

/*****************************************************************************/
/******************* Form to mark a social note as favourite *****************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToFavSocialNote (long NotCod)
  {
   extern const char *Txt_Mark_as_favourite;

   /***** Form to mark social note as favourite *****/
   Soc_FormStart (ActFavSocNotGbl,ActFavSocNotUsr);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_FAV ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/fav64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Mark_as_favourite,Txt_Mark_as_favourite);
   Act_EndForm ();
  }

/*****************************************************************************/
/*************** Form to unshare (stop sharing) social note ******************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToUnshareSocialNote (long NotCod)
  {
   extern const char *Txt_SOCIAL_NOTE_Shared;

   /***** Form to share social publishing *****/
   Soc_FormStart (ActUnsSocNotGbl,ActUnsSocNotUsr);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_SHARE ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/shared64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_SOCIAL_NOTE_Shared,Txt_SOCIAL_NOTE_Shared);
   Act_EndForm ();
  }

/*****************************************************************************/
/*********** Form to unfav (remove mark as favourite) social note ************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToUnfavSocialNote (long NotCod)
  {
   extern const char *Txt_SOCIAL_NOTE_Favourite;

   /***** Form to unfav social note *****/
   Soc_FormStart (ActUnfSocNotGbl,ActUnfSocNotUsr);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_FAV ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/faved64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_SOCIAL_NOTE_Favourite,Txt_SOCIAL_NOTE_Favourite);
   Act_EndForm ();
  }

/*****************************************************************************/
/********* Form to unfav (remove mark as favourite) social comment ***********/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToUnfavSocialComment (long PubCod)
  {
   extern const char *Txt_SOCIAL_NOTE_Favourite;

   /***** Form to unfav social comment *****/
   Soc_FormStart (ActUnfSocComGbl,ActUnfSocComUsr);
   Soc_PutHiddenParamPubCod (PubCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_FAV ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/faved64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_SOCIAL_NOTE_Favourite,Txt_SOCIAL_NOTE_Favourite);
   Act_EndForm ();
  }

/*****************************************************************************/
/******************** Form to remove social publishing ***********************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToRemoveSocialPublishing (long NotCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social publishing *****/
   Soc_FormStart (ActReqRemSocPubGbl,ActReqRemSocPubUsr);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICO_REMOVE ICO_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/remove-on64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICO20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Remove,
	    Txt_Remove);
   Act_EndForm ();
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

void Soc_PutHiddenParamPubCod (long PubCod)
  {
   Par_PutHiddenParamLong ("PubCod",PubCod);
  }

/*****************************************************************************/
/************** Get parameter with the code of a social note *****************/
/*****************************************************************************/

static long Soc_GetParamNotCod (void)
  {
   /***** Get social note code *****/
   return Par_GetParToLong ("NotCod");
  }

/*****************************************************************************/
/************ Get parameter with the code of a social publishing *************/
/*****************************************************************************/

static long Soc_GetParamPubCod (void)
  {
   /***** Get social comment code *****/
   return Par_GetParToLong ("PubCod");
  }

/*****************************************************************************/
/*************************** Comment a social note ***************************/
/*****************************************************************************/

void Soc_ReceiveCommentGbl (void)
  {
   long NotCod;

   /***** Receive comment in a social note *****/
   NotCod = Soc_ReceiveComment ();

   /***** Write updated timeline after commenting (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_ReceiveCommentUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Receive comment in a social note *****/
   NotCod = Soc_ReceiveComment ();

   /***** Write updated timeline after commenting (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

static long Soc_ReceiveComment (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Image Image;
   char *Query;
   struct SocialNote SocNot;
   struct SocialPublishing SocPub;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNotByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      /***** Get the content of the comment *****/
      Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
				 Str_TO_RIGOROUS_HTML,true);

      /***** Initialize image *****/
      Img_ImageConstructor (&Image);

      /***** Get attached image (action, file and title) *****/
      Image.Width   = Soc_IMAGE_SAVED_MAX_WIDTH;
      Image.Height  = Soc_IMAGE_SAVED_MAX_HEIGHT;
      Image.Quality = Soc_IMAGE_SAVED_QUALITY;
      Img_GetImageFromForm (-1,&Image,NULL);

      if (Content[0] ||		// Text not empty
	  Image.Name[0])	// An image is attached
	{
	 /***** Allocate space for query *****/
	 if ((Query = (char *) malloc (256 +
			               strlen (Content) +
			               Img_BYTES_NAME +
			               Img_MAX_BYTES_TITLE +
			               Cns_MAX_BYTES_WWW)) == NULL)
	    Lay_ShowErrorAndExit ("Not enough memory to store database query.");

	 /***** Check if image is received and processed *****/
	 if (Image.Action == Img_ACTION_NEW_IMAGE &&	// Upload new image
	     Image.Status == Img_FILE_PROCESSED)	// The new image received has been processed
	    /* Move processed image to definitive directory */
	    Img_MoveImageToDefinitiveDirectory (&Image);

	 /***** Publish *****/
	 /* Insert into publishings */
	 SocPub.NotCod       = SocNot.NotCod;
	 SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 SocPub.PubType      = Soc_PUB_COMMENT_TO_NOTE;
	 Soc_PublishSocialNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	 /* Insert comment content in the database */
	 sprintf (Query,"INSERT INTO social_comments"
	                " (PubCod,Content,ImageName,ImageTitle,ImageURL)"
			" VALUES"
			" (%ld,'%s','%s','%s','%s')",
		  SocPub.PubCod,
		  Content,
		  Image.Name,
		  (Image.Name[0] &&	// Save image title only if image attached
		   Image.Title) ? Image.Title : "",
		  (Image.Name[0] &&	// Save image URL   only if image attached
		   Image.URL  ) ? Image.URL   : "");
	 DB_QueryINSERT (Query,"can not store comment content");

	 /***** Free space used for query *****/
	 free ((void *) Query);

	 /***** Store notifications about the new comment *****/
	 Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TIMELINE_COMMENT,SocPub.PubCod);

	 /***** Analyze content and store notifications about mentions *****/
	 Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (SocPub.PubCod,Content);

	 /***** Show the social note just commented *****/
	 Soc_WriteSocialNote (&SocNot,
	                      Soc_TOP_MESSAGE_COMMENTED,Gbl.Usrs.Me.UsrDat.UsrCod,
	                      true,true);
	}

      /***** Free image *****/
      Img_ImageDestructor (&Image);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

   return SocNot.NotCod;
  }

/*****************************************************************************/
/**************************** Share a social note ****************************/
/*****************************************************************************/

void Soc_ShareSocialNoteGbl (void)
  {
   long NotCod;

   /***** Share social note *****/
   NotCod = Soc_ShareSocialNote ();

   /***** Write updated timeline after sharing (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_ShareSocialNoteUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Share social note *****/
   NotCod = Soc_ShareSocialNote ();

   /***** Write updated timeline after sharing (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

static long Soc_ShareSocialNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   struct SocialNote SocNot;
   struct SocialPublishing SocPub;
   bool ItsMe;
   long OriginalPubCod;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNotByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
         if (!Soc_CheckIfNoteIsSharedByUsr (SocNot.NotCod,
					    Gbl.Usrs.Me.UsrDat.UsrCod))	// Not yet shared by me
	   {
	    /***** Share (publish social note in timeline) *****/
	    SocPub.NotCod       = SocNot.NotCod;
	    SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	    SocPub.PubType      = Soc_PUB_SHARED_NOTE;
	    Soc_PublishSocialNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	    /* Update number of times this social note is shared */
	    SocNot.NumShared = Soc_UpdateNumTimesANoteHasBeenShared (&SocNot);

	    /**** Create notification about shared post
		  for the author of the post ***/
	    OriginalPubCod = Soc_GetPubCodOfOriginalSocialNote (SocNot.NotCod);
	    if (OriginalPubCod > 0)
	       Soc_CreateNotifToAuthor (SocNot.UsrCod,OriginalPubCod,Ntf_EVENT_TIMELINE_SHARE);
	   }
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

   return SocNot.NotCod;
  }

/*****************************************************************************/
/********************** Mark a social note as favourite **********************/
/*****************************************************************************/

void Soc_FavSocialNoteGbl (void)
  {
   long NotCod;

   /***** Mark social note as favourite *****/
   NotCod = Soc_FavSocialNote ();

   /***** Write updated timeline after marking as favourite (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_FavSocialNoteUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Mark social note as favourite *****/
   NotCod = Soc_FavSocialNote ();

   /***** Write updated timeline after marking as favourite (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

static long Soc_FavSocialNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   char Query[256];
   struct SocialNote SocNot;
   bool ItsMe;
   long OriginalPubCod;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNotByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (!Soc_CheckIfNoteIsFavedByUsr (SocNot.NotCod,
					   Gbl.Usrs.Me.UsrDat.UsrCod))	// I have not yet favourited the note
	   {
	    /***** Mark as favourite in database *****/
	    sprintf (Query,"INSERT IGNORE INTO social_notes_fav"
			   " (NotCod,UsrCod,TimeFav)"
			   " VALUES"
			   " (%ld,%ld,NOW())",
		     SocNot.NotCod,
		     Gbl.Usrs.Me.UsrDat.UsrCod);
	    DB_QueryINSERT (Query,"can not favourite social note");

	    /* Update number of times this social note is favourited */
	    SocNot.NumFavs = Soc_GetNumTimesANoteHasBeenFav (&SocNot);

	    /**** Create notification about favourite post
		  for the author of the post ***/
	    OriginalPubCod = Soc_GetPubCodOfOriginalSocialNote (SocNot.NotCod);
	    if (OriginalPubCod > 0)
	       Soc_CreateNotifToAuthor (SocNot.UsrCod,OriginalPubCod,Ntf_EVENT_TIMELINE_FAV);

	    /***** Show the social note just favourited *****/
	    Soc_WriteSocialNote (&SocNot,
				 Soc_TOP_MESSAGE_FAVED,Gbl.Usrs.Me.UsrDat.UsrCod,
				 true,true);
	   }
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

   return SocNot.NotCod;
  }

/*****************************************************************************/
/********************* Mark a social comment as favourite ********************/
/*****************************************************************************/

void Soc_FavSocialCommentGbl (void)
  {
   long NotCod;

   /***** Mark social comment as favourite *****/
   NotCod = Soc_FavSocialComment ();

   /***** Write updated timeline after marking as favourite (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_FavSocialCommentUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Mark social comment as favourite *****/
   NotCod = Soc_FavSocialComment ();

   /***** Write updated timeline after marking as favourite (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

static long Soc_FavSocialComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   struct SocialComment SocCom;
   bool ItsMe;
   char Query[256];

   /***** Initialize image *****/
   Img_ImageConstructor (&SocCom.Image);

   /***** Get data of social comment *****/
   SocCom.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialComByCod (&SocCom);

   if (SocCom.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (SocCom.UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (!Soc_CheckIfCommIsFavedByUsr (SocCom.PubCod,
					   Gbl.Usrs.Me.UsrDat.UsrCod)) // I have not yet favourited the comment
	   {
	    /***** Mark as favourite in database *****/
	    sprintf (Query,"INSERT IGNORE INTO social_comments_fav"
			   " (PubCod,UsrCod,TimeFav)"
			   " VALUES"
			   " (%ld,%ld,NOW())",
		     SocCom.PubCod,
		     Gbl.Usrs.Me.UsrDat.UsrCod);
	    DB_QueryINSERT (Query,"can not favourite social comment");

	    /* Update number of times this social comment is favourited */
	    SocCom.NumFavs = Soc_GetNumTimesACommHasBeenFav (&SocCom);

	    /**** Create notification about favourite post
		  for the author of the post ***/
	    Soc_CreateNotifToAuthor (SocCom.UsrCod,SocCom.PubCod,Ntf_EVENT_TIMELINE_FAV);

	    /***** Show the social comment just favourited *****/
	    Soc_WriteSocialComment (&SocCom,
				    Soc_TOP_MESSAGE_FAVED,Gbl.Usrs.Me.UsrDat.UsrCod,
				    true);
	   }
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Img_ImageDestructor (&SocCom.Image);

   return SocCom.NotCod;
  }

/*****************************************************************************/
/*********** Create a notification for the author of a post/comment **********/
/*****************************************************************************/

static void Soc_CreateNotifToAuthor (long AuthorCod,long PubCod,
                                     Ntf_NotifyEvent_t NotifyEvent)
  {
   struct UsrData UsrDat;
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   UsrDat.UsrCod = AuthorCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
     {
      /***** This fav must be notified by email? *****/
      CreateNotif = (UsrDat.Prefs.NotifNtfEvents & (1 << NotifyEvent));
      NotifyByEmail = CreateNotif &&
		      (UsrDat.Prefs.EmailNtfEvents & (1 << NotifyEvent));

      /***** Create notification for the author of the post.
	     If this author wants to receive notifications by email,
	     activate the sending of a notification *****/
      if (CreateNotif)
	 Ntf_StoreNotifyEventToOneUser (NotifyEvent,&UsrDat,PubCod,
					(Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
									0));
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/***************** Unshare a previously shared social note *******************/
/*****************************************************************************/

void Soc_UnshareSocialNoteGbl (void)
  {
   long NotCod;

   /***** Unshare a previously shared social note *****/
   NotCod = Soc_UnshareSocialNote ();

   /***** Write updated timeline after unsharing (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_UnshareSocialNoteUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Unshare a previously shared social note *****/
   NotCod = Soc_UnshareSocialNote ();

   /***** Write updated timeline after unsharing (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

static long Soc_UnshareSocialNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   char Query[256];
   struct SocialNote SocNot;
   long OriginalPubCod;
   bool ItsMe;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNotByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (SocNot.NumShared &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (Soc_CheckIfNoteIsSharedByUsr (SocNot.NotCod,
					   Gbl.Usrs.Me.UsrDat.UsrCod))	// I am a sharer
	   {
	    /***** Delete social publishing from database *****/
	    sprintf (Query,"DELETE FROM social_pubs"
	                   " WHERE NotCod=%ld"
	                   " AND PublisherCod=%ld"
	                   " AND PubType=%u",
	             SocNot.NotCod,
	             Gbl.Usrs.Me.UsrDat.UsrCod,
	             (unsigned) Soc_PUB_SHARED_NOTE);
	    DB_QueryDELETE (Query,"can not remove a social publishing");

	    /***** Update number of times this social note is shared *****/
	    SocNot.NumShared = Soc_UpdateNumTimesANoteHasBeenShared (&SocNot);

            /***** Mark possible notifications on this social note as removed *****/
	    OriginalPubCod = Soc_GetPubCodOfOriginalSocialNote (SocNot.NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE,OriginalPubCod);

	    /***** Show the social note corresponding
		   to the publishing just unshared *****/
	    Soc_WriteSocialNote (&SocNot,
				 Soc_TOP_MESSAGE_UNSHARED,Gbl.Usrs.Me.UsrDat.UsrCod,
				 true,true);
	   }
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

   return SocNot.NotCod;
  }

/*****************************************************************************/
/******* Stop marking as favourite a previously favourited social note *******/
/*****************************************************************************/

void Soc_UnfavSocialNoteGbl (void)
  {
   long NotCod;

   /***** Stop marking as favourite a previously favourited social note *****/
   NotCod = Soc_UnfavSocialNote ();

   /***** Write updated timeline after unfav (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_UnfavSocialNoteUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Unfav a social note previously marked as favourite *****/
   NotCod = Soc_UnfavSocialNote ();

   /***** Write updated timeline after unfav (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

static long Soc_UnfavSocialNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   struct SocialNote SocNot;
   char Query[256];
   long OriginalPubCod;
   bool ItsMe;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNotByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (SocNot.NumFavs &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (Soc_CheckIfNoteIsFavedByUsr (SocNot.NotCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the note
	   {
	    /***** Delete the mark as favourite from database *****/
	    sprintf (Query,"DELETE FROM social_notes_fav"
			   " WHERE NotCod=%ld AND UsrCod=%ld",
		     SocNot.NotCod,
		     Gbl.Usrs.Me.UsrDat.UsrCod);
	    DB_QueryDELETE (Query,"can not unfavourite social note");

	    /***** Update number of times this social note is favourited *****/
	    SocNot.NumFavs = Soc_GetNumTimesANoteHasBeenFav (&SocNot);

            /***** Mark possible notifications on this social note as removed *****/
	    OriginalPubCod = Soc_GetPubCodOfOriginalSocialNote (SocNot.NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,OriginalPubCod);

	    /***** Show the social note just unfavourited *****/
	    Soc_WriteSocialNote (&SocNot,
				 Soc_TOP_MESSAGE_UNFAVED,Gbl.Usrs.Me.UsrDat.UsrCod,
				 true,true);
	   }
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

   return SocNot.NotCod;
  }

/*****************************************************************************/
/***** Stop marking as favourite a previously favourited social comment *****/
/*****************************************************************************/

void Soc_UnfavSocialCommentGbl (void)
  {
   long NotCod;

   /***** Stop marking as favourite a previously favourited social comment *****/
   NotCod = Soc_UnfavSocialComment ();

   /***** Write updated timeline after unfav (global) *****/
   Soc_ShowTimelineGblHighlightingNot (NotCod);
  }

void Soc_UnfavSocialCommentUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Unfav a social comment previously marked as favourite *****/
   NotCod = Soc_UnfavSocialComment ();

   /***** Write updated timeline after unfav (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   Lay_EndSection ();
  }

static long Soc_UnfavSocialComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   struct SocialComment SocCom;
   bool ItsMe;
   char Query[256];

   /***** Initialize image *****/
   Img_ImageConstructor (&SocCom.Image);

   /***** Get data of social comment *****/
   SocCom.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialComByCod (&SocCom);

   if (SocCom.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (SocCom.UsrCod);
      if (SocCom.NumFavs &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (Soc_CheckIfCommIsFavedByUsr (SocCom.PubCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the comment
	   {
	    /***** Delete the mark as favourite from database *****/
	    sprintf (Query,"DELETE FROM social_comments_fav"
			   " WHERE PubCod=%ld AND UsrCod=%ld",
		     SocCom.PubCod,
		     Gbl.Usrs.Me.UsrDat.UsrCod);
	    DB_QueryDELETE (Query,"can not unfavourite social comment");

	    /***** Update number of times this social comment is favourited *****/
	    SocCom.NumFavs = Soc_GetNumTimesACommHasBeenFav (&SocCom);

            /***** Mark possible notifications on this social comment as removed *****/
            Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,SocCom.PubCod);

	    /***** Show the social comment just unfavourited *****/
	    Soc_WriteSocialComment (&SocCom,
				    Soc_TOP_MESSAGE_UNFAVED,Gbl.Usrs.Me.UsrDat.UsrCod,
				    true);
	   }
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Img_ImageDestructor (&SocCom.Image);

   return SocCom.NotCod;
  }

/*****************************************************************************/
/******************* Request the removal of a social note ********************/
/*****************************************************************************/

void Soc_RequestRemSocialNoteGbl (void)
  {
   /***** Request the removal of social note *****/
   Soc_RequestRemovalSocialNote ();

   /***** Write timeline again (global) *****/
   Soc_ShowTimelineGbl2 ();
  }

void Soc_RequestRemSocialNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Request the removal of social note *****/
   Soc_RequestRemovalSocialNote ();

   /***** Write timeline again (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   Lay_EndSection ();
  }

static void Soc_RequestRemovalSocialNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   extern const char *Txt_Remove;
   struct SocialNote SocNot;
   bool ItsMe;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNotByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (ItsMe)	// I am the author of this note
	{
	 /***** Show question and button to remove social note *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_post);

	 /* Show social note */
	 Soc_WriteSocialNote (&SocNot,
			      Soc_TOP_MESSAGE_NONE,-1L,
			      false,true);

	 /* End alert */
	 Gbl.Social.NotCod = SocNot.NotCod;	// Social note to be removed
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	    Ale_ShowAlertAndButton2 (ActRemSocPubUsr,"timeline",NULL,
	                             Soc_PutParamsRemoveSocialNote,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlertAndButton2 (ActRemSocPubGbl,NULL,NULL,
	                             Soc_PutParamsRemoveSocialNote,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/****************** Put parameters to remove a social note *******************/
/*****************************************************************************/

static void Soc_PutParamsRemoveSocialNote (void)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted ();
   else
      Soc_PutParamWhichUsrs ();
   Soc_PutHiddenParamNotCod (Gbl.Social.NotCod);
  }

/*****************************************************************************/
/*************************** Remove a social note ****************************/
/*****************************************************************************/

void Soc_RemoveSocialNoteGbl (void)
  {
   /***** Remove a social note *****/
   Soc_RemoveSocialNote ();

   /***** Write updated timeline after removing (global) *****/
   Soc_ShowTimelineGbl2 ();
  }

void Soc_RemoveSocialNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Remove a social note *****/
   Soc_RemoveSocialNote ();

   /***** Write updated timeline after removing (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   Lay_EndSection ();
  }

static void Soc_RemoveSocialNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_Post_removed;
   struct SocialNote SocNot;
   bool ItsMe;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNotByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (ItsMe)	// I am the author of this note
	{
	 if (SocNot.NoteType == Soc_NOTE_SOCIAL_POST)
            /***** Remove image file associated to social post *****/
            Soc_RemoveImgFileFromSocialPost (SocNot.Cod);

	 /***** Delete social note from database *****/
	 Soc_RemoveASocialNoteFromDB (&SocNot);

	 /***** Message of success *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Post_removed);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/************** Remove one file associated to a social post ******************/
/*****************************************************************************/

static void Soc_RemoveImgFileFromSocialPost (long PstCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get name of image associated to a social post from database *****/
   sprintf (Query,"SELECT ImageName FROM social_posts WHERE PstCod=%ld",
	    PstCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get image"))
     {
      /***** Get image name (row[0]) *****/
      row = mysql_fetch_row (mysql_res);

      /***** Remove image file *****/
      Img_RemoveImageFile (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Remove a social note from database **********************/
/*****************************************************************************/

static void Soc_RemoveASocialNoteFromDB (struct SocialNote *SocNot)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;
   unsigned long NumComments;
   unsigned long NumCom;

   /***** Mark possible notifications on the publishings
          of this social note as removed *****/
   /* Mark notifications of the original social note as removed */
   PubCod = Soc_GetPubCodOfOriginalSocialNote (SocNot->NotCod);
   if (PubCod > 0)
     {
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_COMMENT,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV    ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE  ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_MENTION,PubCod);
     }

   /* Get comments of this social note */
   sprintf (Query,"SELECT PubCod FROM social_pubs"
	          " WHERE NotCod=%ld AND PubType=%u",
	    SocNot->NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   NumComments = DB_QuerySELECT (Query,&mysql_res,"can not get social comments");

   /* For each comment... */
   for (NumCom = 0;
	NumCom < NumComments;
	NumCom++)
     {
      /* Get code of social comment **/
      row = mysql_fetch_row (mysql_res);
      PubCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Mark notifications as removed */
      if (PubCod > 0)
	{
	 Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_COMMENT,PubCod);
	 Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV    ,PubCod);
	 Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_MENTION,PubCod);
	}
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove favs *****/
   /* Remove favs for all comments in this note */
   sprintf (Query,"DELETE FROM social_comments_fav"
                  " USING social_pubs,social_comments_fav"
	          " WHERE social_pubs.NotCod=%ld"
                  " AND social_pubs.PubType=%u"
	          " AND social_pubs.PubCod=social_comments_fav.PubCod",
	    SocNot->NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove favs for social note");

   /* Remove favs for this note */
   sprintf (Query,"DELETE FROM social_notes_fav WHERE NotCod=%ld",
	    SocNot->NotCod);
   DB_QueryDELETE (Query,"can not remove favs for social note");

   /***** Remove content of the comments of this social note *****/
   sprintf (Query,"DELETE FROM social_comments"
	          " USING social_pubs,social_comments"
	          " WHERE social_pubs.NotCod=%ld"
                  " AND social_pubs.PubType=%u"
	          " AND social_pubs.PubCod=social_comments.PubCod",
	    SocNot->NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Remove all the social publishings of this note *****/
   sprintf (Query,"DELETE FROM social_pubs WHERE NotCod=%ld",
	    SocNot->NotCod);
   DB_QueryDELETE (Query,"can not remove a social publishing");

   /***** Remove social note *****/
   sprintf (Query,"DELETE FROM social_notes"
	          " WHERE NotCod=%ld"
	          " AND UsrCod=%ld",		// Extra check: I am the author
	    SocNot->NotCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not remove a social note");

   if (SocNot->NoteType == Soc_NOTE_SOCIAL_POST)
     {
      /***** Remove social post *****/
      sprintf (Query,"DELETE FROM social_posts"
	             " WHERE PstCod=%ld",
	       SocNot->Cod);
      DB_QueryDELETE (Query,"can not remove a social post");
     }

   /***** Reset social note *****/
   Soc_ResetSocialNote (SocNot);
  }

/*****************************************************************************/
/******************* Get code of social note of a publishing *****************/
/*****************************************************************************/

static long Soc_GetNotCodOfSocialPublishing (long PubCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long NotCod = -1L;

   /***** Get code of social note from database *****/
   sprintf (Query,"SELECT NotCod FROM social_pubs WHERE PubCod=%ld",
	    PubCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get code of social note") == 1)   // Result should have a unique row
     {
      /* Get code of social note */
      row = mysql_fetch_row (mysql_res);
      NotCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NotCod;
  }

/*****************************************************************************/
/************ Get code of social publishing of the original note *************/
/*****************************************************************************/

static long Soc_GetPubCodOfOriginalSocialNote (long NotCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long OriginalPubCod = -1L;

   /***** Get code of social publishing of the original note *****/
   sprintf (Query,"SELECT PubCod FROM social_pubs"
		  " WHERE NotCod=%ld AND PubType=%u",
	    NotCod,(unsigned) Soc_PUB_ORIGINAL_NOTE);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get code of social publishing") == 1)   // Result should have a unique row
     {
      /* Get code of social publishing (row[0]) */
      row = mysql_fetch_row (mysql_res);
      OriginalPubCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return OriginalPubCod;
  }

/*****************************************************************************/
/************* Request the removal of a comment in a social note *************/
/*****************************************************************************/

void Soc_RequestRemSocialComGbl (void)
  {
   /***** Request the removal of comment in social note *****/
   Soc_RequestRemovalSocialComment ();

   /***** Write timeline again (global) *****/
   Soc_ShowTimelineGbl2 ();
  }

void Soc_RequestRemSocialComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Request the removal of comment in social note *****/
   Soc_RequestRemovalSocialComment ();

   /***** Write timeline again (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   Lay_EndSection ();
  }

static void Soc_RequestRemovalSocialComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct SocialComment SocCom;
   bool ItsMe;

   /***** Initialize image *****/
   Img_ImageConstructor (&SocCom.Image);

   /***** Get data of social comment *****/
   SocCom.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialComByCod (&SocCom);

   if (SocCom.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (SocCom.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Show question and button to remove social comment *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_comment);

	 /* Show social comment */
	 Soc_WriteSocialComment (&SocCom,
				 Soc_TOP_MESSAGE_NONE,-1L,
				 true);

	 /* End alert */
	 Gbl.Social.PubCod = SocCom.PubCod;	// Social publishing to be removed
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	    Ale_ShowAlertAndButton2 (ActRemSocComUsr,"timeline",NULL,
	                             Soc_PutParamsRemoveSocialCommment,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlertAndButton2 (ActRemSocComGbl,NULL,NULL,
	                             Soc_PutParamsRemoveSocialCommment,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Img_ImageDestructor (&SocCom.Image);
  }

/*****************************************************************************/
/***************** Put parameters to remove a social comment *****************/
/*****************************************************************************/

static void Soc_PutParamsRemoveSocialCommment (void)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted ();
   else
      Soc_PutParamWhichUsrs ();
   Soc_PutHiddenParamPubCod (Gbl.Social.PubCod);
  }

/*****************************************************************************/
/************************** Remove a social comment **************************/
/*****************************************************************************/

void Soc_RemoveSocialComGbl (void)
  {
   /***** Remove a social comment *****/
   Soc_RemoveSocialComment ();

   /***** Write updated timeline after removing (global) *****/
   Soc_ShowTimelineGbl2 ();
  }

void Soc_RemoveSocialComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   Lay_StartSection (Soc_TIMELINE_SECTION_ID);

   /***** Remove a social comment *****/
   Soc_RemoveSocialComment ();

   /***** Write updated timeline after removing (user) *****/
   Soc_ShowTimelineUsr ();

   /***** End section *****/
   Lay_EndSection ();
  }

static void Soc_RemoveSocialComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Comment_removed;
   struct SocialComment SocCom;
   bool ItsMe;

   /***** Initialize image *****/
   Img_ImageConstructor (&SocCom.Image);

   /***** Get data of social comment *****/
   SocCom.PubCod = Soc_GetParamPubCod ();
   Soc_GetDataOfSocialComByCod (&SocCom);

   if (SocCom.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (SocCom.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Remove image file associated to social post *****/
	 Soc_RemoveImgFileFromSocialComment (SocCom.PubCod);

	 /***** Delete social comment from database *****/
	 Soc_RemoveASocialCommentFromDB (&SocCom);

	 /***** Message of success *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Comment_removed);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Img_ImageDestructor (&SocCom.Image);
  }

/*****************************************************************************/
/************* Remove one file associated to a social comment ****************/
/*****************************************************************************/

static void Soc_RemoveImgFileFromSocialComment (long PubCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get name of image associated to a social post from database *****/
   sprintf (Query,"SELECT ImageName FROM social_comments WHERE PubCod=%ld",
	    PubCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get image"))
     {
      /***** Get image name (row[0]) *****/
      row = mysql_fetch_row (mysql_res);

      /***** Remove image file *****/
      Img_RemoveImageFile (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Remove a social comment from database ********************/
/*****************************************************************************/

static void Soc_RemoveASocialCommentFromDB (struct SocialComment *SocCom)
  {
   char Query[128];

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_COMMENT,SocCom->PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV    ,SocCom->PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_MENTION,SocCom->PubCod);

   /***** Remove favs for this comment *****/
   sprintf (Query,"DELETE FROM social_comments_fav WHERE PubCod=%ld",
	    SocCom->PubCod);
   DB_QueryDELETE (Query,"can not remove favs for social comment");

   /***** Remove content of this social comment *****/
   sprintf (Query,"DELETE FROM social_comments WHERE PubCod=%ld",
	    SocCom->PubCod);
   DB_QueryDELETE (Query,"can not remove a social comment");

   /***** Remove this social comment *****/
   sprintf (Query,"DELETE FROM social_pubs"
	          " WHERE PubCod=%ld"
	          " AND PublisherCod=%ld"	// Extra check: I am the author
	          " AND PubType=%u",		// Extra check: it's a comment
	    SocCom->PubCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    (unsigned) Soc_PUB_COMMENT_TO_NOTE);
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

   /***** Remove favs for comments *****/
   /* Remove all favs made by this user in any social comment */
   sprintf (Query,"DELETE FROM social_comments_fav WHERE UsrCod=%ld",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove favs");

   /* Remove all favs for all comments of this user */
   sprintf (Query,"DELETE FROM social_comments_fav"
	          " USING social_pubs,social_comments_fav"
	          " WHERE social_pubs.PublisherCod=%ld"	// Author of the comment
                  " AND social_pubs.PubType=%u"
	          " AND social_pubs.PubCod=social_comments_fav.PubCod",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove favs");

   /* Remove all favs for all comments in all the social notes of the user */
   sprintf (Query,"DELETE FROM social_comments_fav"
	          " USING social_notes,social_pubs,social_comments_fav"
	          " WHERE social_notes.UsrCod=%ld"	// Author of the note
	          " AND social_notes.NotCod=social_pubs.NotCod"
                  " AND social_pubs.PubType=%u"
	          " AND social_pubs.PubCod=social_comments_fav.PubCod",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Remove favs for notes *****/
   /* Remove all favs made by this user in any social note */
   sprintf (Query,"DELETE FROM social_notes_fav WHERE UsrCod=%ld",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove favs");

   /* Remove all favs for all notes of this user */
   sprintf (Query,"DELETE FROM social_notes_fav"
	          " USING social_notes,social_notes_fav"
	          " WHERE social_notes.UsrCod=%ld"	// Author of the note
	          " AND social_notes.NotCod=social_notes_fav.NotCod",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove favs");

   /***** Remove social comments *****/
   /* Remove content of all the comments in all the social notes of the user */
   sprintf (Query,"DELETE FROM social_comments"
	          " USING social_notes,social_pubs,social_comments"
	          " WHERE social_notes.UsrCod=%ld"
		  " AND social_notes.NotCod=social_pubs.NotCod"
                  " AND social_pubs.PubType=%u"
	          " AND social_pubs.PubCod=social_comments.PubCod",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /* Remove all the comments from any user in any social note of the user */
   sprintf (Query,"DELETE FROM social_pubs"
	          " USING social_notes,social_pubs"
	          " WHERE social_notes.UsrCod=%ld"
		  " AND social_notes.NotCod=social_pubs.NotCod"
                  " AND social_pubs.PubType=%u",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /* Remove content of all the comments of the user in any social note */
   sprintf (Query,"DELETE FROM social_comments"
	          " USING social_pubs,social_comments"
	          " WHERE social_pubs.PublisherCod=%ld"
	          " AND social_pubs.PubType=%u"
	          " AND social_pubs.PubCod=social_comments.PubCod",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Remove all the social posts of the user *****/
   sprintf (Query,"DELETE FROM social_posts"
		  " WHERE PstCod IN"
		  " (SELECT Cod FROM social_notes"
	          " WHERE UsrCod=%ld AND NoteType=%u)",
	    UsrCod,(unsigned) Soc_NOTE_SOCIAL_POST);
   DB_QueryDELETE (Query,"can not remove social posts");

   /***** Remove all the social publishings of any user authored by the user *****/
   sprintf (Query,"DELETE FROM social_pubs"
                  " USING social_notes,social_pubs"
	          " WHERE social_notes.UsrCod=%ld"
                  " AND social_notes.NotCod=social_pubs.NotCod",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social publishings");

   /***** Remove all the social publishings of the user *****/
   sprintf (Query,"DELETE FROM social_pubs WHERE PublisherCod=%ld",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social publishings");

   /***** Remove all the social notes of the user *****/
   sprintf (Query,"DELETE FROM social_notes WHERE UsrCod=%ld",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social notes");
  }

/*****************************************************************************/
/**************** Check if a user has published a social note ****************/
/*****************************************************************************/

static bool Soc_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod)
  {
   char Query[256];

   sprintf (Query,"SELECT COUNT(*) FROM social_pubs"
	          " WHERE NotCod=%ld AND PublisherCod=%ld AND PubType=%u",
	    NotCod,UsrCod,(unsigned) Soc_PUB_SHARED_NOTE);
   return (DB_QueryCOUNT (Query,"can not check if a user has shared a social note") != 0);
  }

/*****************************************************************************/
/*************** Check if a user has favourited a social note ****************/
/*****************************************************************************/

static bool Soc_CheckIfNoteIsFavedByUsr (long NotCod,long UsrCod)
  {
   char Query[256];

   sprintf (Query,"SELECT COUNT(*) FROM social_notes_fav"
	          " WHERE NotCod=%ld AND UsrCod=%ld",
	    NotCod,UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user has favourited a social note") != 0);
  }

/*****************************************************************************/
/************* Check if a user has favourited a social comment ***************/
/*****************************************************************************/

static bool Soc_CheckIfCommIsFavedByUsr (long PubCod,long UsrCod)
  {
   char Query[256];

   sprintf (Query,"SELECT COUNT(*) FROM social_comments_fav"
	          " WHERE PubCod=%ld AND UsrCod=%ld",
	    PubCod,UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user has favourited a social comment") != 0);
  }

/*****************************************************************************/
/******* Get number of times a social note has been shared in timeline *******/
/*****************************************************************************/

static unsigned Soc_UpdateNumTimesANoteHasBeenShared (struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Get number of times (users) this note has been shared *****/
   sprintf (Query,"SELECT COUNT(*) FROM social_pubs"
	          " WHERE NotCod=%ld"
	          " AND PublisherCod<>%ld"
		  " AND PubType=%u",
	    SocNot->NotCod,
	    SocNot->UsrCod,	// The author
	    (unsigned) Soc_PUB_SHARED_NOTE);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of times a note has been shared");
  }

/*****************************************************************************/
/*********** Get number of times a social note has been favourited ***********/
/*****************************************************************************/

static unsigned Soc_GetNumTimesANoteHasBeenFav (struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Get number of times (users) this note has been favourited *****/
   sprintf (Query,"SELECT COUNT(*) FROM social_notes_fav"
	          " WHERE NotCod=%ld"
	          " AND UsrCod<>%ld",	// Extra check
	    SocNot->NotCod,
	    SocNot->UsrCod);	// The author
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of times a note has been favourited");
  }

/*****************************************************************************/
/********* Get number of times a social comment has been favourited **********/
/*****************************************************************************/

static unsigned Soc_GetNumTimesACommHasBeenFav (struct SocialComment *SocCom)
  {
   char Query[256];

   /***** Get number of times (users) this comment has been favourited *****/
   sprintf (Query,"SELECT COUNT(*) FROM social_comments_fav"
	          " WHERE PubCod=%ld"
	          " AND UsrCod<>%ld",	// Extra check
	    SocCom->PubCod,
	    SocCom->UsrCod);	// The author
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of times a comment has been favourited");
  }

/*****************************************************************************/
/**************** Show users who have shared this social note ****************/
/*****************************************************************************/

static void Soc_ShowUsrsWhoHaveSharedSocialNote (const struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Get users who have shared this note *****/
   sprintf (Query,"SELECT PublisherCod FROM social_pubs"
		  " WHERE NotCod=%ld"
		  " AND PublisherCod<>%ld"
		  " AND PubType=%u"
		  " ORDER BY PubCod LIMIT %u",
	    SocNot->NotCod,
	    SocNot->UsrCod,
	    (unsigned) Soc_PUB_SHARED_NOTE,
	    Soc_MAX_SHARERS_FAVERS_SHOWN);
   Soc_ShowSharersOrFavers (SocNot->NumShared,Query);
  }

/*****************************************************************************/
/********* Show users who have marked this social note as favourite **********/
/*****************************************************************************/

static void Soc_ShowUsrsWhoHaveMarkedSocialNoteAsFav (const struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Get users who have marked this note as favourite *****/
   sprintf (Query,"SELECT UsrCod FROM social_notes_fav"
		  " WHERE NotCod=%ld"
		  " AND UsrCod<>%ld"	// Extra check
		  " ORDER BY FavCod LIMIT %u",
	    SocNot->NotCod,
	    SocNot->UsrCod,
	    Soc_MAX_SHARERS_FAVERS_SHOWN);
   Soc_ShowSharersOrFavers (SocNot->NumFavs,Query);
  }

/*****************************************************************************/
/********* Show users who have marked this social note as favourite **********/
/*****************************************************************************/

static void Soc_ShowUsrsWhoHaveMarkedSocialCommAsFav (const struct SocialComment *SocCom)
  {
   char Query[256];

   /***** Get users who have marked this comment as favourite *****/
   sprintf (Query,"SELECT UsrCod FROM social_comments_fav"
		  " WHERE PubCod=%ld"
		  " AND UsrCod<>%ld"	// Extra check
		  " ORDER BY FavCod LIMIT %u",
	    SocCom->PubCod,
	    SocCom->UsrCod,
	    Soc_MAX_SHARERS_FAVERS_SHOWN);
   Soc_ShowSharersOrFavers (SocCom->NumFavs,Query);
  }

/*****************************************************************************/
/************************ Show sharers or favouriters ************************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_ShowSharersOrFavers (unsigned NumUsrs,const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumFirstUsrs;
   unsigned NumUsr;
   unsigned NumUsrsShown = 0;
   struct UsrData UsrDat;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   /***** Show number of users who have marked this social note as favourite *****/
   fprintf (Gbl.F.Out,"<span class=\"SOCIAL_NUM_SHARES_FAVS\"> %u</span>",
            NumUsrs);

   if (NumUsrs)
     {
      /***** Get list of users from database *****/
      NumFirstUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get users");
      if (NumFirstUsrs)
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** List users *****/
	 for (NumUsr = 0;
	      NumUsr < NumFirstUsrs;
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
	       ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
	       Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
	                                             NULL,
	                         "PHOTO15x20",Pho_ZOOM,true);	// Use unique id
               fprintf (Gbl.F.Out,"</div>");

               NumUsrsShown++;
              }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      if (NumUsrs > NumUsrsShown)
	 fprintf (Gbl.F.Out,"<div class=\"SOCIAL_SHARER\">"
	                    "<img src=\"%s/ellipsis32x32.gif\""
			    " alt=\"%u\" title=\"%u\""
			    " class=\"ICO20x20\" />"
			    "</div>",
		  Gbl.Prefs.IconsURL,
		  NumUsrs - NumUsrsShown,
		  NumUsrs - NumUsrsShown);
     }
  }

/*****************************************************************************/
/**************** Get data of social note using its code *********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialNotByCod (struct SocialNote *SocNot)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (SocNot->NotCod > 0)
     {
      /***** Get data of social note from database *****/
      sprintf (Query,"SELECT NotCod,NoteType,Cod,UsrCod,HieCod,Unavailable,UNIX_TIMESTAMP(TimeNote)"
		     " FROM social_notes"
		     " WHERE NotCod=%ld",
	       SocNot->NotCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social note"))
	{
	 /***** Get data of social note *****/
	 row = mysql_fetch_row (mysql_res);
	 Soc_GetDataOfSocialNoteFromRow (row,SocNot);
	}
      else
	 /***** Reset fields of social note *****/
	 Soc_ResetSocialNote (SocNot);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Reset fields of social note *****/
      Soc_ResetSocialNote (SocNot);
  }

/*****************************************************************************/
/*************** Get data of social comment using its code *******************/
/*****************************************************************************/

static void Soc_GetDataOfSocialComByCod (struct SocialComment *SocCom)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (SocCom->PubCod > 0)
     {
      /***** Get data of social comment from database *****/
      sprintf (Query,"SELECT social_pubs.PubCod,social_pubs.PublisherCod,"
		     "social_pubs.NotCod,"
		     "UNIX_TIMESTAMP(social_pubs.TimePublish),"
		     "social_comments.Content,"
		     "social_comments.ImageName,"
		     "social_comments.ImageTitle,"
		     "social_comments.ImageURL"
		     " FROM social_pubs,social_comments"
		     " WHERE social_pubs.PubCod=%ld"
                     " AND social_pubs.PubType=%u"
		     " AND social_pubs.PubCod=social_comments.PubCod",
	       SocCom->PubCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social comment"))
	{
	 /***** Get data of social comment *****/
	 row = mysql_fetch_row (mysql_res);
	 Soc_GetDataOfSocialCommentFromRow (row,SocCom);
	}
      else
	 /***** Reset fields of social comment *****/
	 Soc_ResetSocialComment (SocCom);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
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
   const Soc_TopMessage_t TopMessages[Soc_NUM_PUB_TYPES] =
     {
      Soc_TOP_MESSAGE_NONE,		// Soc_PUB_UNKNOWN
      Soc_TOP_MESSAGE_NONE,		// Soc_PUB_ORIGINAL_NOTE
      Soc_TOP_MESSAGE_SHARED,		// Soc_PUB_SHARED_NOTE
      Soc_TOP_MESSAGE_COMMENTED,	// Soc_PUB_COMMENT_TO_NOTE
     };

   /***** Get code of social publishing (row[0]) *****/
   SocPub->PubCod       = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get social note code (row[1]) *****/
   SocPub->NotCod       = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get publisher's code (row[2]) *****/
   SocPub->PublisherCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get type of publishing (row[3]) *****/
   SocPub->PubType      = Soc_GetPubTypeFromStr ((const char *) row[3]);
   SocPub->TopMessage   = TopMessages[SocPub->PubType];

   /***** Get time of the note (row[4]) *****/
   SocPub->DateTimeUTC  = Dat_GetUNIXTimeFromStr (row[4]);
  }

/*****************************************************************************/
/******************** Get data of social note from row ***********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialNoteFromRow (MYSQL_ROW row,struct SocialNote *SocNot)
  {
   /***** Get social code (row[0]) *****/
   SocNot->NotCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get note type (row[1]) *****/
   SocNot->NoteType    = Soc_GetNoteTypeFromStr ((const char *) row[1]);

   /***** Get file/post... code (row[2]) *****/
   SocNot->Cod         = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get (from) user code (row[3]) *****/
   SocNot->UsrCod      = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get hierarchy code (row[4]) *****/
   SocNot->HieCod      = Str_ConvertStrCodToLongCod (row[4]);

   /***** File/post... unavailable (row[5]) *****/
   SocNot->Unavailable = (row[5][0] == 'Y');

   /***** Get time of the note (row[6]) *****/
   SocNot->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[6]);

   /***** Get number of times this social note has been shared *****/
   SocNot->NumShared   = Soc_UpdateNumTimesANoteHasBeenShared (SocNot);

   /***** Get number of times this social note has been favourited *****/
   SocNot->NumFavs     = Soc_GetNumTimesANoteHasBeenFav (SocNot);
  }

/*****************************************************************************/
/**** Get social publishing type from string number coming from database *****/
/*****************************************************************************/

static Soc_PubType_t Soc_GetPubTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Soc_NUM_PUB_TYPES)
         return (Soc_PubType_t) UnsignedNum;

   return Soc_PUB_UNKNOWN;
  }

/*****************************************************************************/
/****** Get social note type from string number coming from database *********/
/*****************************************************************************/

static Soc_NoteType_t Soc_GetNoteTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Soc_NUM_NOTE_TYPES)
         return (Soc_NoteType_t) UnsignedNum;

   return Soc_NOTE_UNKNOWN;
  }

/*****************************************************************************/
/****************** Get data of social comment from row **********************/
/*****************************************************************************/

static void Soc_GetDataOfSocialCommentFromRow (MYSQL_ROW row,struct SocialComment *SocCom)
  {
   /*
   row[0]: PubCod
   row[1]: PublisherCod
   row[2]: NotCod
   row[3]: TimePublish
   row[4]: Content
   row[5]: ImageName
   row[6]: ImageTitle
   row[7]: ImageURL
    */
   /***** Get code of social comment (row[0]) *****/
   SocCom->PubCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get (from) user code (row[1]) *****/
   SocCom->UsrCod      = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get code of social note (row[2]) *****/
   SocCom->NotCod      = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get time of the note (row[3]) *****/
   SocCom->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[3]);

   /***** Get content (row[4]) *****/
   Str_Copy (SocCom->Content,row[4],
             Cns_MAX_BYTES_LONG_TEXT);

   /***** Get number of times this comment has been favourited *****/
   SocCom->NumFavs     = Soc_GetNumTimesACommHasBeenFav (SocCom);

   /****** Get image name (row[5]), title (row[6]) and URL (row[7]) *****/
   Img_GetImageNameTitleAndURLFromRow (row[5],row[6],row[7],&SocCom->Image);
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
   SocCom->PubCod      = -1L;
   SocCom->UsrCod      = -1L;
   SocCom->NotCod      = -1L;
   SocCom->DateTimeUTC = (time_t) 0;
   SocCom->Content[0]  = '\0';
  }

/*****************************************************************************/
/**************** Clear unused old social timelines in database **************/
/*****************************************************************************/

void Soc_ClearOldTimelinesDB (void)
  {
   char Query[256];

   /***** Remove social timelines for expired sessions *****/
   sprintf (Query,"DELETE LOW_PRIORITY FROM social_timelines"
                  " WHERE SessionId NOT IN (SELECT SessionId FROM sessions)");
   DB_QueryDELETE (Query,"can not remove old social timelines");
  }

/*****************************************************************************/
/************* Clear social timeline for this session in database ************/
/*****************************************************************************/

static void Soc_ClearTimelineThisSession (void)
  {
   char Query[128 + Ses_BYTES_SESSION_ID];

   /***** Remove social timeline for this session *****/
   sprintf (Query,"DELETE FROM social_timelines WHERE SessionId='%s'",
            Gbl.Session.Id);
   DB_QueryDELETE (Query,"can not remove social timeline");
  }

/*****************************************************************************/
/****** Add just retrieved notes to current timeline for this session ********/
/*****************************************************************************/

static void Soc_AddNotesJustRetrievedToTimelineThisSession (void)
  {
   char Query[256 + Ses_BYTES_SESSION_ID];

   sprintf (Query,"INSERT IGNORE INTO social_timelines"
	          " (SessionId,NotCod)"
	          " SELECT DISTINCTROW '%s',NotCod FROM not_codes",
            Gbl.Session.Id);
   DB_QueryINSERT (Query,"can not insert social notes in timeline");
  }

/*****************************************************************************/
/******************* Get notification of a new social post *******************/
/*****************************************************************************/

void Soc_GetNotifSocialPublishing (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                   char **ContentStr,
                                   long PubCod,bool GetContent)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   size_t Length;
   bool ContentCopied = false;

   /***** Return nothing on error *****/
   SocPub.PubType = Soc_PUB_UNKNOWN;
   SummaryStr[0] = '\0';	// Return nothing on error
   Content[0] = '\0';

   /***** Get summary and content from social post from database *****/
   sprintf (Query,"SELECT PubCod,NotCod,PublisherCod,PubType,UNIX_TIMESTAMP(TimePublish)"
		  " FROM social_pubs WHERE PubCod=%ld",
            PubCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get data of social publishing") == 1)   // Result should have a unique row
     {
      /* Get data of social publishing */
      row = mysql_fetch_row (mysql_res);
      Soc_GetDataOfSocialPublishingFromRow (row,&SocPub);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get summary and content *****/
   switch (SocPub.PubType)
     {
      case Soc_PUB_UNKNOWN:
	 break;
      case Soc_PUB_ORIGINAL_NOTE:
      case Soc_PUB_SHARED_NOTE:
	 /* Get data of social note */
	 SocNot.NotCod = SocPub.NotCod;
	 Soc_GetDataOfSocialNotByCod (&SocNot);

	 if (SocNot.NoteType == Soc_NOTE_SOCIAL_POST)
	   {
	    /***** Get content of social post from database *****/
	    // TODO: What happens if content is empty and an image is attached?
	    sprintf (Query,"SELECT Content FROM social_posts"
			   " WHERE PstCod=%ld",
		     SocNot.Cod);
	    if (DB_QuerySELECT (Query,&mysql_res,"can not get the content of a social post") == 1)   // Result should have a unique row
	      {
	       /***** Get row *****/
	       row = mysql_fetch_row (mysql_res);

	       /****** Get content (row[0]) *****/
	       Str_Copy (Content,row[0],
	                 Cns_MAX_BYTES_LONG_TEXT);
	      }

	    /***** Free structure that stores the query result *****/
            DB_FreeMySQLResult (&mysql_res);

	    /***** Copy content string *****/
	    if (GetContent)
	      {
	       Length = strlen (Content);
	       if ((*ContentStr = (char *) malloc (Length + 1)) != NULL)
		 {
		  Str_Copy (*ContentStr,Content,
		            Length);
		  ContentCopied = true;
		 }
	      }

	    /***** Copy summary string *****/
	    Str_LimitLengthHTMLStr (Content,Ntf_MAX_BYTES_SUMMARY);
	    Str_Copy (SummaryStr,Content,
	              Ntf_MAX_BYTES_SUMMARY);
	   }
	 else
	    Soc_GetNoteSummary (&SocNot,SummaryStr);
	 break;
      case Soc_PUB_COMMENT_TO_NOTE:
	 /***** Get content of social post from database *****/
	 // TODO: What happens if content is empty and an image is attached?
	 sprintf (Query,"SELECT Content FROM social_comments"
			" WHERE PubCod=%ld",
		  SocPub.PubCod);
	 if (DB_QuerySELECT (Query,&mysql_res,"can not get the content of a comment to a social note") == 1)   // Result should have a unique row
	   {
	    /***** Get row *****/
	    row = mysql_fetch_row (mysql_res);

	    /****** Get content (row[0]) *****/
	    Str_Copy (Content,row[0],
	              Cns_MAX_BYTES_LONG_TEXT);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

	 /***** Copy content string *****/
	 if (GetContent)
	   {
	    Length = strlen (Content);
	    if ((*ContentStr = (char *) malloc (Length + 1)) != NULL)
	      {
	       Str_Copy (*ContentStr,Content,
	                 Length);
	       ContentCopied = true;
	      }
	   }

	 /***** Copy summary string *****/
	 Str_LimitLengthHTMLStr (Content,Ntf_MAX_BYTES_SUMMARY);
	 Str_Copy (SummaryStr,Content,
	           Ntf_MAX_BYTES_SUMMARY);
	 break;
     }

   /***** Create empty content string if nothing copied *****/
   if (GetContent && !ContentCopied)
      if ((*ContentStr = (char *) malloc (1)) != NULL)
         (*ContentStr)[0] = '\0';
  }

/*****************************************************************************/
/*** Create a notification about mention for any nickname in a publishing ****/
/*****************************************************************************/
/*
 Example: "The user @rms says..."
                     ^ ^
         PtrStart ___| |___ PtrEnd
                 Length = 3
*/
static void Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (long PubCod,const char *Txt)
  {
   const char *Ptr;
   bool IsNickname;
   struct
     {
      const char *PtrStart;
      const char *PtrEnd;
      size_t Length;		// Length of the nickname
     } Nickname;
   struct UsrData UsrDat;
   bool ItsMe;
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Find nicknames and create notifications *****/
   for (Ptr = Txt;
	*Ptr;)
      /* Check if the next char is the start of a nickname */
      if ((int) *Ptr == (int) '@')
	{
	 /* Find nickname end */
	 Ptr++;	// Points to first character after @
         Nickname.PtrStart = Ptr;

	 /* A nick can have digits, letters and '_'  */
	 for (;
	      *Ptr;
	      Ptr++)
	    if (!((*Ptr >= 'a' && *Ptr <= 'z') ||
		  (*Ptr >= 'A' && *Ptr <= 'Z') ||
		  (*Ptr >= '0' && *Ptr <= '9') ||
		  (*Ptr == '_')))
	       break;

	 /* Calculate length of this nickname */
	 Nickname.PtrEnd = Ptr - 1;
         Nickname.Length = (size_t) (Ptr - Nickname.PtrStart);

	 /* A nick (without arroba) must have a number of characters
            Nck_MIN_BYTES_NICKNAME_WITHOUT_ARROBA <= Length <= Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA */
	 IsNickname = (Nickname.Length >= Nck_MIN_BYTES_NICKNAME_WITHOUT_ARROBA &&
	               Nickname.Length <= Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA);

	 if (IsNickname)
	   {
	    /* Copy nickname */
	    strncpy (UsrDat.Nickname,Nickname.PtrStart,Nickname.Length);

	    if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrDat.Nickname)) > 0)
	      {
	       ItsMe = Usr_ItsMe (UsrDat.UsrCod);
	       if (!ItsMe)	// Not me
		 {
		  /* Get user's data */
		  Usr_GetAllUsrDataFromUsrCod (&UsrDat);

		  /* Create notification for the mentioned user *****/
		  CreateNotif = (UsrDat.Prefs.NotifNtfEvents & (1 << Ntf_EVENT_TIMELINE_MENTION));
		  if (CreateNotif)
		    {
		     NotifyByEmail = (UsrDat.Prefs.EmailNtfEvents & (1 << Ntf_EVENT_TIMELINE_MENTION));
		     Ntf_StoreNotifyEventToOneUser (Ntf_EVENT_TIMELINE_MENTION,&UsrDat,PubCod,
						    (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
										    0));
		    }
		 }
	      }
	   }
	}
      /* The next char is not the start of a nickname */
      else	// Character != '@'
         Ptr++;

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }
