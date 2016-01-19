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
#define Soc_MAX_SHARERS_FAVERS_SHOWN	 10	// Maximum number of users shown who have share/fav a social note

#define Soc_MAX_BYTES_SUMMARY	       1000
#define Soc_MAX_CHARS_IN_POST	       1000

// Number of recent publishings got and shown the first time, before refreshing
#define Soc_MAX_NEW_PUBS_TO_GET_AND_SHOW	10000	// Unlimited
#define Soc_MAX_REC_PUBS_TO_GET_AND_SHOW	20	// Recent publishings to show (first time)
#define Soc_MAX_OLD_PUBS_TO_GET_AND_SHOW	20	// IMPORTANT: If you change this number,
							// set also this constant to the new value
							// in JavaScript function readOldTimelineData

#define Soc_MAX_LENGTH_ID	(32+Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+10+1)

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

typedef enum
  {
   Soc_TOP_MESSAGE_NONE,
   Soc_TOP_MESSAGE_SHARED,
   Soc_TOP_MESSAGE_UNSHARED,
   Soc_TOP_MESSAGE_FAV,
   Soc_TOP_MESSAGE_UNFAV,
   Soc_TOP_MESSAGE_COMMENTED,
  } Soc_TopMessage_t;

static const Act_Action_t Soc_DefaultActions[Soc_NUM_NOTE_TYPES] =
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

static const char *Soc_Icons[Soc_NUM_NOTE_TYPES] =
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
   Soc_TopMessage_t TopMessage;	// Used to show feedback on the action made
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
   unsigned NumFavs;	// Number of times (users) this note has been favourited
  };

struct SocialComment
  {
   long ComCod;
   long UsrCod;
   long NotCod;		// Note code
   time_t DateTimeUTC;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
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

static void Soc_GetAndShowNewTimeline (Soc_TimelineUsrOrGbl_t TimelineUsrOrGbl);
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
static void Soc_InsertNewPubsInTimeline (const char *Query);
static void Soc_ShowOldPubsInTimeline (const char *Query);

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub);

static void Soc_PutLinkToViewNewPublishings (void);
static void Soc_PutLinkToViewOldPublishings (void);

static void Soc_WriteSocialNote (const struct SocialNote *SocNot,
                                 Soc_TopMessage_t TopMessage,
                                 long UsrCod,
                                 bool Highlight,
                                 bool ShowNoteAlone);
static void Soc_WriteTopPublisher (Soc_TopMessage_t TopMessage,long UsrCod);
static void Soc_WriteAuthorNote (struct UsrData *UsrDat);
static void Soc_WriteDateTime (time_t TimeUTC);
static void Soc_GetAndWriteSocialPost (long PstCod);
static void Soc_PutFormGoToAction (const struct SocialNote *SocNot);
static void Soc_GetNoteSummary (const struct SocialNote *SocNot,
                                char *SummaryStr,unsigned MaxChars);
static void Soc_PublishSocialNoteInTimeline (struct SocialPublishing *SocPub);

static void Soc_PutFormToWriteNewPost (void);
static void Soc_PutTextarea (const char *Placeholder);

static long Soc_ReceiveSocialPost (void);

static void Soc_PutIconToToggleCommentSocialNote (const char UniqueId[Soc_MAX_LENGTH_ID],
                                                  bool PutText);
static void Soc_PutHiddenFormToWriteNewCommentToSocialNote (long NotCod,
                                                            const char IdNewComment[Soc_MAX_LENGTH_ID]);
static unsigned long Soc_GetNumCommentsInSocialNote (long NotCod);
static void Soc_WriteCommentsInSocialNote (const struct SocialNote *SocNot,
                                           const char IdNewComment[Soc_MAX_LENGTH_ID]);
static void Soc_WriteSocialComment (struct SocialComment *SocCom,
                                    bool ShowCommentAlone);
static void Soc_WriteAuthorComment (struct UsrData *UsrDat);
static void Soc_PutFormToRemoveComment (long ComCod);

static void Soc_PutDisabledIconShare (unsigned NumShared);
static void Soc_PutDisabledIconFav (unsigned NumFavs);

static void Soc_PutFormToShareSocialNote (long NotCod);
static void Soc_PutFormToFavSocialNote (long NotCod);

static void Soc_PutFormToUnshareSocialNote (long NotCod);
static void Soc_PutFormToUnfavSocialNote (long NotCod);

static void Soc_PutFormToRemoveSocialPublishing (long NotCod);

static void Soc_PutHiddenParamNotCod (long NotCod);
static void Soc_PutHiddenParamComCod (long ComCod);
static long Soc_GetParamNotCod (void);
static long Soc_GetParamComCod (void);

static long Soc_ReceiveComment (void);

static long Soc_ShareSocialNote (void);
static long Soc_FavSocialNote (void);

static long Soc_UnshareSocialNote (void);
static void Soc_UnshareASocialPublishingFromDB (struct SocialNote *SocNot);
static long Soc_UnfavSocialNote (void);

static void Soc_RequestRemovalSocialNote (void);
static void Soc_RemoveSocialNote (void);
static void Soc_RemoveASocialNoteFromDB (struct SocialNote *SocNot);

static void Soc_RequestRemovalSocialComment (void);
static void Soc_RemoveSocialComment (void);
static void Soc_RemoveASocialCommentFromDB (struct SocialComment *SocCom);

static bool Soc_CheckIfNoteIsPublishedInTimelineByUsr (long NotCod,long UsrCod);
static bool Soc_CheckIfNoteIsFavouritedByUsr (long NotCod,long UsrCod);

static void Soc_UpdateNumTimesANoteHasBeenShared (struct SocialNote *SocNot);
static void Soc_UpdateNumTimesANoteHasBeenFav (struct SocialNote *SocNot);

static void Soc_ShowUsrsWhoHaveSharedSocialNote (const struct SocialNote *SocNot);
static void Soc_ShowUsrsWhoHaveMarkedSocialNoteAsFav (const struct SocialNote *SocNot);
static void Soc_ShowSharersOrFavers (unsigned NumUsrs,const char *Query);

static void Soc_GetDataOfSocialNoteByCod (struct SocialNote *SocNot);
static void Soc_GetDataOfSocialCommentByCod (struct SocialComment *SocCom);

static void Soc_GetDataOfSocialPublishingFromRow (MYSQL_ROW row,struct SocialPublishing *SocPub);
static void Soc_GetDataOfSocialNoteFromRow (MYSQL_ROW row,struct SocialNote *SocNot);
static Soc_PubType_t Soc_GetPubTypeFromStr (const char *Str);
static Soc_NoteType_t Soc_GetNoteTypeFromStr (const char *Str);
static void Soc_GetDataOfSocialCommentFromRow (MYSQL_ROW row,struct SocialComment *SocCom);

static void Soc_ResetSocialNote (struct SocialNote *SocNot);
static void Soc_ResetSocialComment (struct SocialComment *SocCom);

static void Soc_SetUniqueId (char UniqueId[Soc_MAX_LENGTH_ID]);

static void Soc_ClearTimelineThisSession (void);
static void Soc_AddNotesJustRetrievedToTimelineThisSession (void);

/*****************************************************************************/
/***** Show social activity (timeline) including all the users I follow ******/
/*****************************************************************************/

void Soc_ShowTimelineGbl (void)
  {
   Soc_ShowTimelineGblHighlightingNot (-1L);
  }

static void Soc_ShowTimelineGblHighlightingNot (long NotCod)
  {
   extern const char *Txt_Public_activity;
   extern const char *Txt_You_dont_follow_any_user;
   char Query[1024];

   /***** Show warning if I do not follow anyone *****/
   if (!Fol_GetNumFollowing (Gbl.Usrs.Me.UsrDat.UsrCod))
      Lay_ShowAlert (Lay_INFO,Txt_You_dont_follow_any_user);

   /***** Build query to get timeline *****/
   Soc_BuildQueryToGetTimeline (Soc_TIMELINE_GBL,
                                Soc_GET_RECENT_TIMELINE,
                                Query);

   /***** Show timeline *****/
   Soc_ShowTimeline (Query,Txt_Public_activity,NotCod);

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
   extern const char *Txt_Public_activity_OF_A_USER;
   char Query[1024];

   /***** Build query to show timeline with publishings of a unique user *****/
   Soc_BuildQueryToGetTimeline (Soc_TIMELINE_USR,
                                Soc_GET_RECENT_TIMELINE,
                                Query);

   /***** Show timeline *****/
   sprintf (Gbl.Title,Txt_Public_activity_OF_A_USER,Gbl.Usrs.Other.UsrDat.FirstName);
   Soc_ShowTimeline (Query,Gbl.Title,NotCod);

   /***** Drop temporary tables *****/
   Soc_DropTemporaryTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/********** Refresh new publishings in social timeline via AJAX **************/
/*****************************************************************************/

void Soc_RefreshNewTimelineGbl (void)
  {
   // Send, before the HTML, the refresh time
   fprintf (Gbl.F.Out,"%lu|",
            Cfg_TIME_TO_REFRESH_SOCIAL_TIMELINE);

   Soc_GetAndShowNewTimeline (Soc_TIMELINE_GBL);
  }

/*****************************************************************************/
/****************** Get and show new publishings in timeline *****************/
/*****************************************************************************/

static void Soc_GetAndShowNewTimeline (Soc_TimelineUsrOrGbl_t TimelineUsrOrGbl)
  {
   char Query[1024];

   /***** Build query to get timeline *****/
   Soc_BuildQueryToGetTimeline (TimelineUsrOrGbl,
                                Soc_GET_ONLY_NEW_PUBS,
                                Query);

   /***** Show new timeline *****/
   Soc_InsertNewPubsInTimeline (Query);

   /***** Drop temporary tables *****/
   Soc_DropTemporaryTablesUsedToQueryTimeline ();

   /***** All the output is made, so don't write anymore *****/
   Gbl.Layout.DivsEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }

/*****************************************************************************/
/************ View new publishings in social timeline via AJAX ***************/
/*****************************************************************************/

void Soc_RefreshOldTimelineGbl (void)
  {
   Soc_GetAndShowOldTimeline (Soc_TIMELINE_GBL);
  }

void Soc_RefreshOldTimelineUsr (void)
  {
   /***** If user exists, show old publishings *****/
   // User's code is already taken from nickname in Par_GetMainParameters ()
   if (Usr_ChkIfUsrCodExists (Gbl.Usrs.Other.UsrDat.UsrCod))        // Existing user
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
/************************ Build query to get timeline ************************/
/*****************************************************************************/
// Query must have space for at least 1024 chars

static void Soc_BuildQueryToGetTimeline (Soc_TimelineUsrOrGbl_t TimelineUsrOrGbl,
                                         Soc_WhatToGetFromTimeline_t WhatToGetFromTimeline,
                                         char *Query)
  {
   char SubQueryPublishers[128];
   char SubQueryRangeBottom[128];
   char SubQueryRangeTop[128];
   char SubQueryAlreadyExists[256];
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
	 sprintf (SubQueryPublishers,"PublisherCod='%ld'",
	          Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case Soc_TIMELINE_GBL:	// Show the timeline of the users I follow
	 sprintf (Query,"CREATE TEMPORARY TABLE publishers "
		        "(UsrCod INT NOT NULL,UNIQUE INDEX(UsrCod)) ENGINE=MEMORY"
		        " SELECT '%ld' AS UsrCod"
		        " UNION"
		        " SELECT FollowedCod AS UsrCod"
		        " FROM usr_follow WHERE FollowerCod='%ld'",
	          Gbl.Usrs.Me.UsrDat.UsrCod,
	          Gbl.Usrs.Me.UsrDat.UsrCod);
	 if (mysql_query (&Gbl.mysql,Query))
	    DB_ExitOnMySQLError ("can not create temporary table");
	 sprintf (SubQueryPublishers,"social_pubs.PublisherCod=publishers.UsrCod");
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
	       strcpy (SubQueryAlreadyExists," AND NotCod NOT IN"
					     " (SELECT NotCod FROM not_codes)");
	       break;
            case Soc_GET_ONLY_OLD_PUBS:
	       strcpy (SubQueryAlreadyExists," AND NotCod NOT IN"
					     " (SELECT NotCod FROM current_timeline)");
	       break;
           }
	 break;
      case Soc_TIMELINE_GBL:	// Show the timeline of the users I follow
	 switch (WhatToGetFromTimeline)
           {
            case Soc_GET_ONLY_NEW_PUBS:
            case Soc_GET_RECENT_TIMELINE:
	       strcpy (SubQueryAlreadyExists," AND social_pubs.NotCod NOT IN"
					     " (SELECT NotCod FROM not_codes)");
	       break;
            case Soc_GET_ONLY_OLD_PUBS:
	       strcpy (SubQueryAlreadyExists," AND social_pubs.NotCod NOT IN"
					     " (SELECT NotCod FROM current_timeline)");
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
      of every set of publishings corresponding to the same note:
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
	 sprintf (SubQueryRangeBottom,
		  TimelineUsrOrGbl == Soc_TIMELINE_USR ? "PubCod>'%ld' AND " :
							 "social_pubs.PubCod>'%ld' AND ",
		  RangePubsToGet.Bottom);
      else
	 SubQueryRangeBottom[0] = '\0';
      if (RangePubsToGet.Top > 0)
	 sprintf (SubQueryRangeTop,
		  TimelineUsrOrGbl == Soc_TIMELINE_USR ? "PubCod<'%ld' AND " :
							 "social_pubs.PubCod<'%ld' AND ",
		  RangePubsToGet.Top);
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
	 case Soc_TIMELINE_GBL:	// Show the timeline of the users I follow
	    sprintf (Query,"SELECT PubCod,NotCod FROM social_pubs,publishers"
			   " WHERE %s%s%s%s"
			   " ORDER BY social_pubs.PubCod DESC LIMIT 1",
		     SubQueryRangeBottom,SubQueryRangeTop,
		     SubQueryPublishers,
		     SubQueryAlreadyExists);
	    break;
	}
      if (DB_QuerySELECT (Query,&mysql_res,"can not get publishing") == 1)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of social publishing (row[0]) */
	 PubCod = Str_ConvertStrCodToLongCod (row[0]);
	 sprintf (Query,"INSERT INTO pub_codes SET PubCod='%ld'",PubCod);
	 DB_QueryINSERT (Query,"can not store publishing code");
	 RangePubsToGet.Top = PubCod;	// Narrow the range for the next iteration

	 /* Get social note code (row[1]) */
	 NotCod = Str_ConvertStrCodToLongCod (row[1]);
	 sprintf (Query,"INSERT INTO not_codes SET NotCod='%ld'",NotCod);
	 DB_QueryINSERT (Query,"can not store note code");
	 sprintf (Query,"INSERT INTO current_timeline SET NotCod='%ld'",NotCod);
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
	          " SET LastPubCod=(SELECT MAX(PubCod) FROM social_pubs)"
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPub;
   struct SocialPublishing SocPub;
   struct SocialNote SocNot;

   /***** Get publishings from database *****/
   NumPubsGot = DB_QuerySELECT (Query,&mysql_res,"can not get timeline");

   /***** Start frame *****/
   Lay_StartRoundFrame (Soc_WIDTH_TIMELINE,Title);

   /***** Form to write a new post *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0 ||				// Global timeline
       Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      Soc_PutFormToWriteNewPost ();

   /***** New publishings refreshed dynamically via AJAX *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod <= 0)				// Global timeline
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
      Soc_GetDataOfSocialNoteByCod (&SocNot);

      /* Write social note */
      Soc_WriteSocialNote (&SocNot,
                           SocPub.TopMessage,SocPub.PublisherCod,
			   SocNot.NotCod == NotCodToHighlight,
			   false);
     }
   fprintf (Gbl.F.Out,"</ul>");

   /***** Store first publishing code into session *****/
   Soc_UpdateFirstPubCodIntoSession (SocPub.PubCod);

   if (NumPubsGot == Soc_MAX_REC_PUBS_TO_GET_AND_SHOW)
     {
      /***** Link to view old publishings via AJAX *****/
      Soc_PutLinkToViewOldPublishings ();

      /***** Hidden list where insert old publishings via AJAX *****/
      fprintf (Gbl.F.Out,"<ul id=\"old_timeline_list\"></ul>");
     }

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
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
      Soc_GetDataOfSocialNoteByCod (&SocNot);

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
      Soc_GetDataOfSocialNoteByCod (&SocNot);

      /* Write social note */
      Soc_WriteSocialNote (&SocNot,
                           SocPub.TopMessage,SocPub.PublisherCod,
                           false,false);
     }

   /***** Store first publishing code into session *****/
   Soc_UpdateFirstPubCodIntoSession (SocPub.PubCod);

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
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteSocialNote (const struct SocialNote *SocNot,
                                 Soc_TopMessage_t TopMessage,
                                 long UsrCod,
                                 bool Highlight,	// Highlight social note
                                 bool ShowNoteAlone)	// Social note is shown alone, not in a list
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData UsrDat;
   bool IAmTheAuthor = false;
   bool IAmAPublisherOfThisSocNot = false;
   bool IAmAFavouriterOfThisSocNot = false;
   struct Institution Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char ForumName[512];
   char SummaryStr[Cns_MAX_BYTES_TEXT+1];
   unsigned NumComments;
   char IdNewComment[Soc_MAX_LENGTH_ID];

   /***** Start frame ****/
   if (ShowNoteAlone)
     {
      Lay_StartRoundFrame (Soc_WIDTH_TIMELINE,NULL);
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
      Lay_ShowAlert (Lay_ERROR,"Error in social note.");
   else
     {
      /***** Initialize location in hierarchy *****/
      Ins.InsCod = -1L;
      Ctr.CtrCod = -1L;
      Deg.DegCod = -1L;
      Crs.CrsCod = -1L;

      /***** Write sharer/commenter if distinct to author *****/
      Soc_WriteTopPublisher (TopMessage,UsrCod);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get author data *****/
      UsrDat.UsrCod = SocNot->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
      if (Gbl.Usrs.Me.Logged)
	{
	 IAmTheAuthor = (UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
	 IAmAPublisherOfThisSocNot = Soc_CheckIfNoteIsPublishedInTimelineByUsr (SocNot->NotCod,
										Gbl.Usrs.Me.UsrDat.UsrCod);
	 IAmAFavouriterOfThisSocNot = Soc_CheckIfNoteIsFavouritedByUsr (SocNot->NotCod,
									Gbl.Usrs.Me.UsrDat.UsrCod);
	}

      /***** Left: write author's photo *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_LEFT_PHOTO\">");
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO45x60",Pho_ZOOM,true);	// Use unique id
      fprintf (Gbl.F.Out,"</div>");

      /***** Right: author's name, time, summary and buttons *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_CONTAINER\">");

      /* Write author's full name and nickname */
      Soc_WriteAuthorNote (&UsrDat);

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

      /* Create unique id for new comment */
      Soc_SetUniqueId (IdNewComment);

      /* Get number of comments in this social note */
      NumComments = Soc_GetNumCommentsInSocialNote (SocNot->NotCod);

      /* Put icon to add a comment */
      if (!NumComments &&
	  !SocNot->Unavailable)	// Unavailable social notes can not be commented
         Soc_PutIconToToggleCommentSocialNote (IdNewComment,false);

      /* Put icons to share/unshare */
      if (IAmTheAuthor)				// I am the author
	 Soc_PutDisabledIconShare (SocNot->NumShared);
      else if (IAmAPublisherOfThisSocNot)	// I am a sharer of this social note,
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

      /* Put icon to mark this social note as favourite */
      if (IAmTheAuthor)				// I am the author
	 Soc_PutDisabledIconFav (SocNot->NumFavs);
      else if (IAmAFavouriterOfThisSocNot)	// I have favourited this social note
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

      /* Put icon to remove this social note */
      if (IAmTheAuthor)
	 Soc_PutFormToRemoveSocialPublishing (SocNot->NotCod);

      /* Show comments */
      if (NumComments)
	 Soc_WriteCommentsInSocialNote (SocNot,IdNewComment);

      /* Put hidden form to write a new comment */
      Soc_PutHiddenFormToWriteNewCommentToSocialNote (SocNot->NotCod,IdNewComment);

      /* End of right part */
      fprintf (Gbl.F.Out,"</div>");

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** End list item *****/
   fprintf (Gbl.F.Out,"</li>");

   /***** End frame ****/
   if (ShowNoteAlone)
     {
      fprintf (Gbl.F.Out,"</ul>");
      Lay_EndRoundFrame ();
     }
  }

/*****************************************************************************/
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteTopPublisher (Soc_TopMessage_t TopMessage,long UsrCod)
  {
   extern const char *Txt_View_public_profile;
   extern const char *Txt_SOCIAL_USER_has_shared;
   extern const char *Txt_SOCIAL_USER_has_stopped_sharing;
   extern const char *Txt_SOCIAL_USER_has_marked_as_favourite;
   extern const char *Txt_SOCIAL_USER_has_unmarked_as_favourite;
   extern const char *Txt_SOCIAL_USER_has_commented;
   struct UsrData UsrDat;

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
	 Act_FormStartUnique (ActSeePubPrf);
	 Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
	 Act_LinkFormSubmitUnique (Txt_View_public_profile,"SOCIAL_TOP_PUBLISHER");
	 Str_LimitLengthHTMLStr (UsrDat.FullName,40);
	 fprintf (Gbl.F.Out,"%s</a>",UsrDat.FullName);
	 Act_FormEnd ();

	 /***** Show action made *****/
	 switch (TopMessage)
	   {
	    case Soc_TOP_MESSAGE_NONE:	// Not applicable
	       break;
	    case Soc_TOP_MESSAGE_SHARED:
	       fprintf (Gbl.F.Out," %s",Txt_SOCIAL_USER_has_shared);
	       break;
	    case Soc_TOP_MESSAGE_UNSHARED:
	       fprintf (Gbl.F.Out," %s",Txt_SOCIAL_USER_has_stopped_sharing);
	       break;
	    case Soc_TOP_MESSAGE_FAV:
	       fprintf (Gbl.F.Out," %s",Txt_SOCIAL_USER_has_marked_as_favourite);
	       break;
	    case Soc_TOP_MESSAGE_UNFAV:
	       fprintf (Gbl.F.Out," %s",Txt_SOCIAL_USER_has_unmarked_as_favourite);
	       break;
	    case Soc_TOP_MESSAGE_COMMENTED:
	       fprintf (Gbl.F.Out," %s",Txt_SOCIAL_USER_has_commented);
	       break;
	   }

	 fprintf (Gbl.F.Out,"</div>");
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
  }

/*****************************************************************************/
/************ Write name and nickname of autor of a social note **************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteAuthorNote (struct UsrData *UsrDat)
  {
   extern const char *Txt_View_public_profile;

   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_RIGHT_AUTHOR\">");

   /***** Show user's name inside form to go to user's public profile *****/
   Act_FormStartUnique (ActSeePubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (Txt_View_public_profile,"DAT_N_BOLD");
   Str_LimitLengthHTMLStr (UsrDat->FullName,16);
   fprintf (Gbl.F.Out,"%s</a>",UsrDat->FullName);
   Act_FormEnd ();

   /***** Show user's nickname inside form to go to user's public profile *****/
   Act_FormStartUnique (ActSeePubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (Txt_View_public_profile,"DAT_LIGHT");
   fprintf (Gbl.F.Out," @%s</a>",UsrDat->Nickname);
   Act_FormEnd ();

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/**************** Write the date of creation of a social note ****************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

static void Soc_WriteDateTime (time_t TimeUTC)
  {
   extern const char *Txt_Today;
   char IdDateTime[Soc_MAX_LENGTH_ID];

   /***** Create unique Id *****/
   Soc_SetUniqueId (IdDateTime);

   /***** Container where the date-time is written *****/
   fprintf (Gbl.F.Out,"<div id=\"%s\" class=\"SOCIAL_RIGHT_TIME DAT_LIGHT\""
	              " style=\"display:inline-block;\"></div>",
            IdDateTime);

   /***** Script to write date and time in browser local time *****/
   // This must be out of the div where the output is written
   // because it will be evaluated in a loop in JavaScript
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateHMFromUTC('%s',%ld,'&nbsp;','%s');"
                      "</script>",
            IdDateTime,(long) TimeUTC,Txt_Today);
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
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormGoToAction (const struct SocialNote *SocNot)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_SOCIAL_NOTE[Soc_NUM_NOTE_TYPES];
   extern const char *Txt_not_available;
   char Class[64];

   if (SocNot->Unavailable ||	// File/notice... pointer by this social note is unavailable
       Gbl.Form.Inside)		// Inside another form
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
	    Act_FormStartUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentIns.Ins.InsCod)	// Not the current institution
	       Ins_PutParamInsCod (SocNot->HieCod);		// Go to another institution
	    break;
	 case Soc_NOTE_CTR_DOC_PUB_FILE:
	 case Soc_NOTE_CTR_SHA_PUB_FILE:
	    Act_FormStartUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCtr.Ctr.CtrCod)	// Not the current centre
	       Ctr_PutParamCtrCod (SocNot->HieCod);		// Go to another centre
	    break;
	 case Soc_NOTE_DEG_DOC_PUB_FILE:
	 case Soc_NOTE_DEG_SHA_PUB_FILE:
	    Act_FormStartUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentDeg.Deg.DegCod)	// Not the current degree
	       Deg_PutParamDegCod (SocNot->HieCod);		// Go to another degree
	    break;
	 case Soc_NOTE_CRS_DOC_PUB_FILE:
	 case Soc_NOTE_CRS_SHA_PUB_FILE:
	    Act_FormStartUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_EXAM_ANNOUNCEMENT:
	    Act_FormStartUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Not_PutHiddenParamNotCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_SOCIAL_POST:	// Not applicable
	    return;
	 case Soc_NOTE_FORUM_POST:
	    Act_FormStartUnique (For_ActionsSeeFor[Gbl.Forum.ForumType]);
	    For_PutAllHiddenParamsForum ();
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case Soc_NOTE_NOTICE:
	    Act_FormStartUnique (Soc_DefaultActions[SocNot->NoteType]);
	    Not_PutHiddenParamNotCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 default:			// Not applicable
	    return;
	}

      /***** Link and end form *****/
      sprintf (Class,"%s ICON_HIGHLIGHT",The_ClassFormBold[Gbl.Prefs.Theme]);
      Act_LinkFormSubmitUnique (Txt_SOCIAL_NOTE[SocNot->NoteType],Class);
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
// Return the code of the new note just created

long Soc_StoreAndPublishSocialNote (Soc_NoteType_t NoteType,long Cod)
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
   SocPub.PubType      = Soc_PUB_ORIGINAL_NOTE;
   Soc_PublishSocialNoteInTimeline (&SocPub);

   return SocPub.NotCod;
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
   sprintf (Query,"INSERT INTO social_pubs"
	          " (NotCod,PublisherCod,PubType,TimePublish)"
                  " VALUES"
                  " ('%ld','%ld','%u',NOW())",
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
		     "PHOTO45x60",Pho_ZOOM,false);
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

   /***** Textarea and button *****/
   Soc_PutTextarea (Txt_New_SOCIAL_post);

   /***** End form *****/
   Act_FormEnd ();

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");

   /***** End list *****/
   fprintf (Gbl.F.Out,"</li>"
	              "</ul>");
  }

/*****************************************************************************/
/*** Put textarea and button inside a form to submit a new post or comment ***/
/*****************************************************************************/

static void Soc_PutTextarea (const char *Placeholder)
  {
   extern const char *Txt_Post;
   char IdButton[Soc_MAX_LENGTH_ID];

   /***** Set unique id for the button *****/
   Soc_SetUniqueId (IdButton);

   /***** Textarea to write the content *****/
   fprintf (Gbl.F.Out,"<textarea name=\"Content\" rows=\"1\" maxlength=\"%u\""
                      " placeholder=\"%s&hellip;\""
	              " class=\"SOCIAL_TEXTAREA\""
	              " onfocus=\"expandTextarea(this,'%s','5');\""
	              " onblur=\"contractTextarea(this,'%s','1');\">"
		      "</textarea>",
            Soc_MAX_CHARS_IN_POST,
            Placeholder,
            IdButton,IdButton);

   /***** Help on editor and submit button *****/
   fprintf (Gbl.F.Out,"<div id=\"%s\" style=\"display:none;\">",
            IdButton);
   Lay_HelpPlainEditor ();
   fprintf (Gbl.F.Out,"<button type=\"submit\""
	              " class=\"BT_SUBMIT_INLINE BT_CREATE\">"
		      "%s"
		      "</button>"
		      "</div>",
	    Txt_Post);
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
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Receive and store social post *****/
   NotCod = Soc_ReceiveSocialPost ();

   /***** Write updated timeline after publishing (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

// Returns the code of the social note just created
static long Soc_ReceiveSocialPost (void)
  {
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   char Query[128+Cns_MAX_BYTES_LONG_TEXT];
   long PstCod;
   long NotCod;

   /***** Get the content of the new post *****/
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
      NotCod = Soc_StoreAndPublishSocialNote (Soc_NOTE_SOCIAL_POST,PstCod);
     }
   else
      NotCod = -1L;

   return NotCod;
  }

/*****************************************************************************/
/****** Put an icon to toggle on/off the form to comment a social note *******/
/*****************************************************************************/

static void Soc_PutIconToToggleCommentSocialNote (const char UniqueId[Soc_MAX_LENGTH_ID],
                                                  bool PutText)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Comment;

   /***** Link to toggle on/off the form to comment a social note *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_COMMENT ICON_HIGHLIGHT\">"
                      "<a href=\"\"");
   if (PutText)
      fprintf (Gbl.F.Out," class=\"%s\"",The_ClassForm[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out," onclick=\"toggleDisplay('%s');return false;\" />"
		      "<img src=\"%s/write64x64.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />",
	    UniqueId,
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
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutHiddenFormToWriteNewCommentToSocialNote (long NotCod,
                                                            const char IdNewComment[Soc_MAX_LENGTH_ID])
  {
   extern const char *Txt_New_SOCIAL_comment;

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div id=\"%s\""
		      " class=\"SOCIAL_FORM_COMMENT\""
		      " style=\"display:none;\">",
	    IdNewComment);

   /***** Start form to write the post *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartUniqueAnchor (ActRcvSocComUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStartUnique (ActRcvSocComGbl);
   Soc_PutHiddenParamNotCod (NotCod);

   /***** Textarea and button *****/
   Soc_PutTextarea (Txt_New_SOCIAL_comment);

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

   sprintf (Query,"SELECT COUNT(*) FROM social_pubs"
	          " WHERE NotCod='%ld' AND PubType='%u'",
	    NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   return DB_QueryCOUNT (Query,"can not get number of comments in a social note");
  }

/*****************************************************************************/
/******************* Form to comment a social publishing *********************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteCommentsInSocialNote (const struct SocialNote *SocNot,
                                           const char IdNewComment[Soc_MAX_LENGTH_ID])
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumComments;
   unsigned long NumCom;
   struct SocialComment SocCom;

   /***** Get comments of this social note from database *****/
   sprintf (Query,"SELECT social_pubs.PubCod,social_pubs.PublisherCod,"
		  "social_pubs.NotCod,"
		  "UNIX_TIMESTAMP(social_pubs.TimePublish),"
		  "social_comments.Content"
		  " FROM social_pubs,social_comments"
		  " WHERE social_pubs.NotCod='%ld'"
                  " AND social_pubs.PubType='%u'"
		  " AND social_pubs.PubCod=social_comments.ComCod"
		  " ORDER BY social_pubs.PubCod",
	    SocNot->NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
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
      if (!SocNot->Unavailable)	// Unavailable social notes can not be commented
	{
	 fprintf (Gbl.F.Out,"<li class=\"SOCIAL_COMMENT\">");
	 Soc_PutIconToToggleCommentSocialNote (IdNewComment,true);
	 fprintf (Gbl.F.Out,"</li>");
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
			"PHOTO30x40",Pho_ZOOM,true);	// Use unique id
      fprintf (Gbl.F.Out,"</div>");

      /***** Right: author's name, time, summary and buttons *****/
      fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_RIGHT_CONTAINER\">");

      /* Write author's full name and nickname */
      Soc_WriteAuthorComment (&UsrDat);

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
/****** Write name and nickname of autor of a comment to a social note *******/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_WriteAuthorComment (struct UsrData *UsrDat)
  {
   extern const char *Txt_View_public_profile;

   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_COMMENT_RIGHT_AUTHOR\">");

   /***** Show user's name inside form to go to user's public profile *****/
   Act_FormStartUnique (ActSeePubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (Txt_View_public_profile,"DAT_BOLD");
   Str_LimitLengthHTMLStr (UsrDat->FullName,12);
   fprintf (Gbl.F.Out,"%s</a>",UsrDat->FullName);
   Act_FormEnd ();

   /***** Show user's nickname inside form to go to user's public profile *****/
   Act_FormStartUnique (ActSeePubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Act_LinkFormSubmitUnique (Txt_View_public_profile,"DAT_LIGHT");
   fprintf (Gbl.F.Out," @%s</a>",UsrDat->Nickname);
   Act_FormEnd ();

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Form to remove social comment ************************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToRemoveComment (long ComCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartUniqueAnchor (ActReqRemSocComUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStartUnique (ActReqRemSocComGbl);
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
   extern const char *Txt_SOCIAL_NOTE_Shared_by_X_USERS;
   extern const char *Txt_SOCIAL_NOTE_Not_shared_by_anyone;

   if (NumShared)
      sprintf (Gbl.Title,Txt_SOCIAL_NOTE_Shared_by_X_USERS,NumShared);
   else
      strcpy (Gbl.Title,Txt_SOCIAL_NOTE_Not_shared_by_anyone);

   /***** Disabled icon to share *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE_FAV_DISABLED\">"
		      "<img src=\"%s/share64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
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
      strcpy (Gbl.Title,Txt_SOCIAL_NOTE_Not_favourited_by_anyone);

   /***** Disabled icon to mark as favourite *****/
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE_FAV_DISABLED\">"
		      "<img src=\"%s/fav64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
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
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartUniqueAnchor (ActShaSocNotUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStartUnique (ActShaSocNotGbl);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE_FAV ICON_HIGHLIGHT\">"
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
/******************* Form to mark a social note as favourite *****************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToFavSocialNote (long NotCod)
  {
   extern const char *Txt_Mark_as_favourite;

   /***** Form to mark social note as favourite *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartUniqueAnchor (ActFavSocNotUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStartUnique (ActFavSocNotGbl);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE_FAV ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/fav64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Mark_as_favourite,Txt_Mark_as_favourite);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************** Form to unshare (stop sharing) social note ******************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToUnshareSocialNote (long NotCod)
  {
   extern const char *Txt_Unshare;

   /***** Form to share social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartUniqueAnchor (ActUnsSocNotUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStartUnique (ActUnsSocNotGbl);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE_FAV ICON_HIGHLIGHT\">"
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
/*********** Form to unfav (remove mark as favourite) social note ************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToUnfavSocialNote (long NotCod)
  {
   extern const char *Txt_Favourite;

   /***** Form to share social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartUniqueAnchor (ActUnfSocNotUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStartUnique (ActUnfSocNotGbl);
   Soc_PutHiddenParamNotCod (NotCod);
   fprintf (Gbl.F.Out,"<div class=\"SOCIAL_ICON_SHARE_FAV ICON_HIGHLIGHT\">"
		      "<input type=\"image\""
		      " src=\"%s/faved64x64.png\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"ICON20x20\" />"
		      "</div>",
	    Gbl.Prefs.IconsURL,
	    Txt_Favourite,Txt_Favourite);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Form to remove social publishing ***********************/
/*****************************************************************************/
// All forms in this function and nested functions must have unique identifiers

static void Soc_PutFormToRemoveSocialPublishing (long NotCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove social publishing *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Act_FormStartUniqueAnchor (ActReqRemSocPubUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
      Act_FormStartUnique (ActReqRemSocPubGbl);
   Soc_PutHiddenParamNotCod (NotCod);
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
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Receive comment in a social note *****/
   NotCod = Soc_ReceiveComment ();

   /***** Write updated timeline after commenting (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static long Soc_ReceiveComment (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   char Query[128+Cns_MAX_BYTES_LONG_TEXT];
   struct SocialNote SocNot;
   struct SocialPublishing SocPub;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      /***** Get the content of the comment *****/
      Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
				 Str_TO_RIGOROUS_HTML,true);

      if (Content[0])
	{
	 /***** Publish *****/
	 /* Insert into publishings */
	 SocPub.NotCod       = SocNot.NotCod;
	 SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 SocPub.PubType      = Soc_PUB_COMMENT_TO_NOTE;
	 Soc_PublishSocialNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	 /* Insert comment content in the database */
	 sprintf (Query,"INSERT INTO social_comments (ComCod,Content)"
			" VALUES ('%ld','%s')",
		  SocPub.PubCod,
		  Content);
	 DB_QueryINSERT (Query,"can not store comment content");

	 /***** Show the social note just commented *****/
	 Soc_WriteSocialNote (&SocNot,
	                      Soc_TOP_MESSAGE_COMMENTED,Gbl.Usrs.Me.UsrDat.UsrCod,
	                      true,true);
	}
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_The_original_post_no_longer_exists);

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
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Share social note *****/
   NotCod = Soc_ShareSocialNote ();

   /***** Write updated timeline after sharing (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static long Soc_ShareSocialNote (void)
  {
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
	 SocPub.NotCod       = SocNot.NotCod;
	 SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 SocPub.PubType      = Soc_PUB_SHARED_NOTE;
	 Soc_PublishSocialNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	 /* Update number of times this social note is shared */
	 Soc_UpdateNumTimesANoteHasBeenShared (&SocNot);
	}
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_The_original_post_no_longer_exists);

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
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Mark social note as favourite *****/
   NotCod = Soc_FavSocialNote ();

   /***** Write updated timeline after marking as favourite (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static long Soc_FavSocialNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   struct SocialNote SocNot;
   bool IAmTheAuthor;
   bool IAmAFavouriterOfThisSocNot;
   bool ICanFav;
   char Query[256];

   /***** Get the code of the social note to mark as favourite *****/
   SocNot.NotCod = Soc_GetParamNotCod ();

   /***** Get data of social note *****/
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      IAmTheAuthor = (SocNot.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
      IAmAFavouriterOfThisSocNot = Soc_CheckIfNoteIsFavouritedByUsr (SocNot.NotCod,
								     Gbl.Usrs.Me.UsrDat.UsrCod);
      ICanFav = (Gbl.Usrs.Me.Logged &&
		 !IAmTheAuthor &&		// I am not the author
		 !IAmAFavouriterOfThisSocNot);	// I have not favourited the note
      if (ICanFav)
	{
	 /***** Mark as favourite in database *****/
	 sprintf (Query,"INSERT IGNORE INTO social_notes_fav"
			" (NotCod,UsrCod) VALUES ('%ld','%ld')",
		  SocNot.NotCod,
		  Gbl.Usrs.Me.UsrDat.UsrCod);
	 DB_QueryINSERT (Query,"can not favourite social note");

	 /* Update number of times this social note is favourited */
	 Soc_UpdateNumTimesANoteHasBeenFav (&SocNot);

	 /***** Show the social note corresponding
		to the publishing just fav *****/
	 Soc_WriteSocialNote (&SocNot,
	                      Soc_TOP_MESSAGE_FAV,Gbl.Usrs.Me.UsrDat.UsrCod,
	                      true,true);
	}
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_The_original_post_no_longer_exists);

   return SocNot.NotCod;
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
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Unshare a previously shared social note *****/
   NotCod = Soc_UnshareSocialNote ();

   /***** Write updated timeline after unsharing (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static long Soc_UnshareSocialNote (void)
  {
   struct SocialNote SocNot;
   bool IAmTheAuthor;
   bool IAmAPublisherOfThisSocNot;
   bool ICanUnshare;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
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

      /***** Show the social note corresponding
             to the publishing just unshared *****/
      Soc_WriteSocialNote (&SocNot,
	                   Soc_TOP_MESSAGE_UNSHARED,Gbl.Usrs.Me.UsrDat.UsrCod,
                           true,true);
     }

   return SocNot.NotCod;
  }

/*****************************************************************************/
/**************** Unshare a social publishing from database ******************/
/*****************************************************************************/

static void Soc_UnshareASocialPublishingFromDB (struct SocialNote *SocNot)
  {
   char Query[128];

   /***** Remove social publishing *****/
   sprintf (Query,"DELETE FROM social_pubs"
	          " WHERE NotCod='%ld'"
	          " AND PublisherCod='%ld'"	// I have share this note
                  " AND PubType='%u'",		// It's a shared note
	    SocNot->NotCod,
	    Gbl.Usrs.Me.UsrDat.UsrCod,
	    (unsigned) Soc_PUB_SHARED_NOTE);
   DB_QueryDELETE (Query,"can not remove a social publishing");
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
   Prf_ShowUserProfile ();

   /***** Start section *****/
   fprintf (Gbl.F.Out,"<section id=\"timeline\">");

   /***** Unshare a previously shared social note *****/
   NotCod = Soc_UnfavSocialNote ();

   /***** Write updated timeline after unfav (user) *****/
   Soc_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   fprintf (Gbl.F.Out,"</section>");
  }

static long Soc_UnfavSocialNote (void)
  {
   struct SocialNote SocNot;
   bool IAmTheAuthor;
   bool IAmAFavouriterOfThisSocNot;
   bool ICanUnfav;
   char Query[256];

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   IAmTheAuthor = (SocNot.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
   IAmAFavouriterOfThisSocNot = Soc_CheckIfNoteIsFavouritedByUsr (SocNot.NotCod,
								  Gbl.Usrs.Me.UsrDat.UsrCod);
   ICanUnfav = (Gbl.Usrs.Me.Logged &&
                !IAmTheAuthor &&		// I am not the author
	        IAmAFavouriterOfThisSocNot);	// I have favourited the note
   if (ICanUnfav)
     {
      /***** Delete the mark as favourite from database *****/
      sprintf (Query,"DELETE FROM social_notes_fav"
	             " WHERE NotCod='%ld' AND UsrCod='%ld'",
	       SocNot.NotCod,
	       Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryDELETE (Query,"can not unfavourite social note");

      /***** Update number of times this social note is favourited *****/
      Soc_UpdateNumTimesANoteHasBeenFav (&SocNot);

      /***** Show the social note corresponding
             to the publishing just unfavourited *****/
      Soc_WriteSocialNote (&SocNot,
	                   Soc_TOP_MESSAGE_UNFAV,Gbl.Usrs.Me.UsrDat.UsrCod,
                           true,true);
     }

   return SocNot.NotCod;
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
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   extern const char *Txt_Remove;
   struct SocialNote SocNot;
   bool ICanRemove;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocNot.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);	// I am the author of this note
   if (ICanRemove)
     {
      if (Soc_CheckIfNoteIsPublishedInTimelineByUsr (SocNot.NotCod,SocNot.UsrCod))
	{
	 /***** Show warning and social note *****/
	 /* Warning message */
	 Lay_ShowAlert (Lay_WARNING,Txt_Do_you_really_want_to_remove_the_following_post);

	 /* Show social note */
	 Soc_WriteSocialNote (&SocNot,
	                      Soc_TOP_MESSAGE_NONE,-1L,
	                      false,true);

	 /***** Form to ask for confirmation to remove this social post *****/
	 /* Start form */
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	   {
	    Act_FormStartAnchor (ActRemSocPubUsr,"timeline");
	    Usr_PutParamOtherUsrCodEncrypted ();
	   }
	 else
	    Act_FormStart (ActRemSocPubGbl);
	 Soc_PutHiddenParamNotCod (SocNot.NotCod);

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
   extern const char *Txt_Post_removed;
   struct SocialNote SocNot;
   bool ICanRemove;

   /***** Get data of social note *****/
   SocNot.NotCod = Soc_GetParamNotCod ();
   Soc_GetDataOfSocialNoteByCod (&SocNot);

   ICanRemove = (Gbl.Usrs.Me.Logged &&
                 SocNot.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);	// I am the author of this note
   if (ICanRemove)
     {
      /***** Delete social publishing from database *****/
      Soc_RemoveASocialNoteFromDB (&SocNot);

      /***** Message of success *****/
      Lay_ShowAlert (Lay_SUCCESS,Txt_Post_removed);
     }
  }

/*****************************************************************************/
/******************* Remove a social note from database **********************/
/*****************************************************************************/

static void Soc_RemoveASocialNoteFromDB (struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Remove content of the comments of this social note *****/
   sprintf (Query,"DELETE FROM social_comments"
	          " USING social_pubs,social_comments"
	          " WHERE social_pubs.NotCod='%ld'"
                  " AND social_pubs.PubType='%u'"
	          " AND social_pubs.PubCod=social_comments.ComCod",
	    SocNot->NotCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Remove all the social publishings of this note *****/
   sprintf (Query,"DELETE FROM social_pubs WHERE NotCod='%ld'",
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
   extern const char *Txt_Comment_removed;
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
      Lay_ShowAlert (Lay_SUCCESS,Txt_Comment_removed);
     }
  }

/*****************************************************************************/
/****************** Remove a social comment from database ********************/
/*****************************************************************************/

static void Soc_RemoveASocialCommentFromDB (struct SocialComment *SocCom)
  {
   char Query[128];

   /***** Remove content of this social comment *****/
   sprintf (Query,"DELETE FROM social_comments"
	          " WHERE ComCod='%ld'",
	    SocCom->ComCod);
   DB_QueryDELETE (Query,"can not remove a social comment");

   /***** Remove this social comment *****/
   sprintf (Query,"DELETE FROM social_pubs"
	          " WHERE PubCod='%ld'"
	          " AND PublisherCod='%ld'"	// Extra check: I am the author
	          " AND PubType='%u'",		// Extra check: it's a comment
	    SocCom->ComCod,
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

   /***** Remove social comments *****/
   /* Remove content of all the comments in all the social notes of the user */
   sprintf (Query,"DELETE FROM social_comments"
	          " USING social_pubs,social_comments"
	          " WHERE social_pubs.NotCod IN"
		  " (SELECT NotCod FROM social_notes WHERE UsrCod='%ld')"
                  " AND social_pubs.PubType='%u'"
	          " AND social_pubs.PubCod=social_comments.ComCod",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /* Remove all the comments from any user in any social note of the user */
   sprintf (Query,"DELETE FROM social_pubs"
	          " WHERE NotCod IN"
		  " (SELECT NotCod FROM social_notes WHERE UsrCod='%ld')"
                  " AND social_pubs.PubType='%u'",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /* Remove content of all the comments of the user in any social note */
   sprintf (Query,"DELETE FROM social_comments"
	          " USING social_pubs,social_comments"
	          " WHERE social_pubs.PublisherCod='%ld'"
	          " AND social_pubs.PubType='%u'"
	          " AND social_pubs.PubCod=social_comments.ComCod",
	    UsrCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
   DB_QueryDELETE (Query,"can not remove social comments");

   /***** Remove all the social posts of the user *****/
   sprintf (Query,"DELETE FROM social_posts"
		  " WHERE PstCod IN"
		  " (SELECT Cod FROM social_notes"
	          " WHERE UsrCod='%ld' AND NoteType='%u')",
	    UsrCod,(unsigned) Soc_NOTE_SOCIAL_POST);
   DB_QueryDELETE (Query,"can not remove social posts");

   /***** Remove all the social publishings of any user authored by the user *****/
   sprintf (Query,"DELETE FROM social_pubs"
                  " USING social_notes,social_pubs"
	          " WHERE social_notes.UsrCod='%ld'"
                  " AND social_notes.NotCod=social_pubs.NotCod",
	    UsrCod);
   DB_QueryDELETE (Query,"can not remove social publishings");

   /***** Remove all the social publishings of the user *****/
   sprintf (Query,"DELETE FROM social_pubs"
	          " WHERE PublisherCod='%ld'",
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
   char Query[256];

   sprintf (Query,"SELECT COUNT(*) FROM social_pubs"
	          " WHERE NotCod='%ld'"
	          " AND PublisherCod='%ld'"
                  " AND PubType IN ('%u','%u')",
	    NotCod,UsrCod,
	    (unsigned) Soc_PUB_ORIGINAL_NOTE,
	    (unsigned) Soc_PUB_SHARED_NOTE);
   return (DB_QueryCOUNT (Query,"can not check if a user has published a social note") != 0);
  }

/*****************************************************************************/
/*************** Check if a user has favourited a social note ****************/
/*****************************************************************************/

static bool Soc_CheckIfNoteIsFavouritedByUsr (long NotCod,long UsrCod)
  {
   char Query[256];

   sprintf (Query,"SELECT COUNT(*) FROM social_notes_fav"
	          " WHERE NotCod='%ld' AND UsrCod='%ld'",
	    NotCod,UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user has favourited a social note") != 0);
  }

/*****************************************************************************/
/******** Get number of times a note code has been shared in timeline ********/
/*****************************************************************************/

static void Soc_UpdateNumTimesANoteHasBeenShared (struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Get number of times (users) this note has been shared *****/
   sprintf (Query,"SELECT COUNT(*) FROM social_pubs"
	          " WHERE NotCod='%ld'"
	          " AND PublisherCod<>'%ld'"
		  " AND PubType='%u'",
	    SocNot->NotCod,
	    SocNot->UsrCod,	// The author
	    (unsigned) Soc_PUB_SHARED_NOTE);
   SocNot->NumShared = (unsigned) DB_QueryCOUNT (Query,"can not get number of times a note has been shared");
  }

/*****************************************************************************/
/************ Get number of times a note code has been favourited ************/
/*****************************************************************************/

static void Soc_UpdateNumTimesANoteHasBeenFav (struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Get number of times (users) this note has been favourited *****/
   sprintf (Query,"SELECT COUNT(*) FROM social_notes_fav"
	          " WHERE NotCod='%ld'"
	          " AND UsrCod<>'%ld'",	// Extra check
	    SocNot->NotCod,
	    SocNot->UsrCod);	// The author
   SocNot->NumFavs = (unsigned) DB_QueryCOUNT (Query,"can not get number of times a note has been favourited");
  }

/*****************************************************************************/
/**************** Show users who have shared this social note ****************/
/*****************************************************************************/

static void Soc_ShowUsrsWhoHaveSharedSocialNote (const struct SocialNote *SocNot)
  {
   char Query[256];

   /***** Get users who have shared this note *****/
   sprintf (Query,"SELECT PublisherCod"
		  " FROM social_pubs"
		  " WHERE NotCod='%ld'"
		  " AND PublisherCod<>'%ld'"
		  " AND PubType='%u'"
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

   /***** Get users who have mark this note as favourite *****/
   sprintf (Query,"SELECT UsrCod"
		  " FROM social_notes_fav"
		  " WHERE NotCod='%ld'"
		  " AND UsrCod<>'%ld'"	// Extra check
		  " ORDER BY TimeFav LIMIT %u",
	    SocNot->NotCod,
	    SocNot->UsrCod,
	    Soc_MAX_SHARERS_FAVERS_SHOWN);
   Soc_ShowSharersOrFavers (SocNot->NumFavs,Query);
  }

/*****************************************************************************/
/********* Show users who have marked this social note as favourite **********/
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
   char PhotoURL[PATH_MAX+1];

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
	       ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
	       Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
	                                             NULL,
	                         "PHOTO18x24",Pho_ZOOM,true);	// Use unique id
               fprintf (Gbl.F.Out,"</div>");

               NumUsrsShown++;
              }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}

      if (NumUsrs > NumUsrsShown)
	 fprintf (Gbl.F.Out,"<div class=\"SOCIAL_SHARER\">"
	                    "<img src=\"%s/ellipsis32x32.gif\""
			    " alt=\"%u\" title=\"%u\""
			    " class=\"ICON20x20\" />"
			    "</div>",
		  Gbl.Prefs.IconsURL,
		  NumUsrs - NumUsrsShown,
		  NumUsrs - NumUsrsShown);
     }
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
      sprintf (Query,"SELECT NotCod,NoteType,Cod,UsrCod,HieCod,Unavailable,UNIX_TIMESTAMP(TimeNote)"
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

	 /***** Get number of times this social note has been favourited *****/
	 Soc_UpdateNumTimesANoteHasBeenFav (SocNot);
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
      sprintf (Query,"SELECT social_pubs.PubCod,social_pubs.PublisherCod,"
		     "social_pubs.NotCod,"
		     "UNIX_TIMESTAMP(social_pubs.TimePublish),"
		     "social_comments.Content"
		     " FROM social_pubs,social_comments"
		     " WHERE social_pubs.PubCod='%ld'"
                     " AND social_pubs.PubType='%u'"
		     " AND social_pubs.PubCod=social_comments.ComCod",
	       SocCom->ComCod,(unsigned) Soc_PUB_COMMENT_TO_NOTE);
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

   /* Get type of publishing (row[3]) */
   SocPub->PubType      = Soc_GetPubTypeFromStr ((const char *) row[3]);
   switch (SocPub->PubType)
     {
      case Soc_PUB_UNKNOWN:
      case Soc_PUB_ORIGINAL_NOTE:
	 SocPub->TopMessage = Soc_TOP_MESSAGE_NONE;
	 break;
      case Soc_PUB_SHARED_NOTE:
	 SocPub->TopMessage = Soc_TOP_MESSAGE_SHARED;
	 break;
      case Soc_PUB_COMMENT_TO_NOTE:
	 SocPub->TopMessage = Soc_TOP_MESSAGE_COMMENTED;
	 break;
     }

   /* Get time of the note (row[4]) */
   SocPub->DateTimeUTC  = Dat_GetUNIXTimeFromStr (row[4]);
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

   /* Get file/post... code (row[2]) */
   SocNot->Cod         = Str_ConvertStrCodToLongCod (row[2]);

   /* Get (from) user code (row[3]) */
   SocNot->UsrCod      = Str_ConvertStrCodToLongCod (row[3]);

   /* Get hierarchy code (row[4]) */
   SocNot->HieCod      = Str_ConvertStrCodToLongCod (row[4]);

   /* File/post... unavailable (row[5]) */
   SocNot->Unavailable = (Str_ConvertToUpperLetter (row[5][0]) == 'Y');

   /* Get time of the note (row[6]) */
   SocNot->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[6]);
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

/*****************************************************************************/
/***************************** Get unique Id *********************************/
/*****************************************************************************/

static void Soc_SetUniqueId (char UniqueId[Soc_MAX_LENGTH_ID])
  {
   static unsigned CountForThisExecution = 0;

   /***** Create Id. The id must be unique in timeline,
          but the timeline is updated via AJAX.
          So, Id uses:
          - a name for this execution (Gbl.UniqueNameEncrypted)
          - a number for each element in this execution (CountForThisExecution) *****/
   sprintf (UniqueId,"id_%s_%u",
            Gbl.UniqueNameEncrypted,
            ++CountForThisExecution);
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
   char Query[128+Ses_LENGTH_SESSION_ID];

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
   char Query[256+Ses_LENGTH_SESSION_ID];

   sprintf (Query,"INSERT IGNORE INTO social_timelines (SessionId,NotCod)"
	          " SELECT DISTINCTROW '%s',NotCod FROM not_codes",
            Gbl.Session.Id);
   DB_QueryREPLACE (Query,"can not insert social notes in timeline");
  }
