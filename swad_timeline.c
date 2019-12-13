// swad_timeline.c: social timeline

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions
#include <sys/types.h>		// For time_t

#include "swad_announcement.h"
#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_exam.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_layout.h"
#include "swad_media.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_setting.h"
#include "swad_timeline.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_NUM_VISIBLE_COMMENTS	3	// Maximum number of comments visible before expanding them

#define TL_DEF_USRS_SHOWN	5	// Default maximum number of users shown who have share/fav a note
#define TL_MAX_USRS_SHOWN	1000	// Top     maximum number of users shown who have share/fav a note

#define TL_MAX_CHARS_IN_POST	1000

#define TL_ICON_ELLIPSIS	"ellipsis-h.svg"
#define TL_ICON_FAV		"heart.svg"
#define TL_ICON_FAVED		"heart-red.svg"
#define TL_ICON_SHARE		"share-alt.svg"
#define TL_ICON_SHARED		"share-alt-green.svg"

typedef enum
  {
   TL_TIMELINE_USR,	// Show the timeline of a user
   TL_TIMELINE_GBL,	// Show the timeline of the users follwed by me
  } TL_TimelineUsrOrGbl_t;

#define TL_NUM_WHAT_TO_GET_FROM_TIMELINE 3
typedef enum
  {
   TL_GET_ONLY_NEW_PUBS,	// New publications are retrieved via AJAX
				// automatically from time to time
   TL_GET_RECENT_TIMELINE,	// Recent timeline is shown when user clicks on action menu,...
				// or after editing timeline
   TL_GET_ONLY_OLD_PUBS,	// Old publications are retrieved via AJAX
				// when user clicks on link at bottom of timeline
  } TL_WhatToGetFromTimeline_t;

struct PostContent
  {
   char Txt[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Media Media;
  };

/*
   Timeline images will be saved with:
   · maximum width of TL_IMAGE_SAVED_MAX_HEIGHT
   · maximum height of TL_IMAGE_SAVED_MAX_HEIGHT
   · maintaining the original aspect ratio (aspect ratio recommended: 3:2)
*/
#define TL_IMAGE_SAVED_MAX_WIDTH	768
#define TL_IMAGE_SAVED_MAX_HEIGHT	512
#define TL_IMAGE_SAVED_QUALITY		 75	// 1 to 100
// in timeline posts, the quality should not be high in order to speed up the loading of images

/*
   The timeline is a set of publications.
   A publication can be:
   · an original note
   · a shared note
   · a comment to a note

     _____tl_pubs_____                                       _tl_comments_
    |                 |                                     |             |
    |  Publication n  |------------------------------------>|  Comment p  |
    |    (comment)    |                                +----| (to note m) |
    |_________________|          ____tl_notes_____     |    |_____________|
    |                 |         |                 |    |    |             |
    | Publication n-1 |-------->|      Note m     |<---+    |             |
    | (original note) |         |    (tl. post)   |         |     ...     |
    |_________________|         |_________________|         |_____________|
    |                 |         |                 |         |             |
    |                 |         |     Note m-1    |         |  Comment 1  |
    |        ...      |         |  (public file)  |    +----| (to note 2) |
    |_________________|         |_________________|    |    |_____________|
    |                 |         |                 |    |
    |  Publication 2  |         |      Note 2     |<---+
    |  (shared note)  |----+    | (exam announc.) |
    |_________________|    |    |_________________|
    |                 |    |    |                 |
    |  Publication 1  |    +--->|      Note 1     |
    | (original note) |-------->|    (tl. post)   |
    |_________________|         |_________________|

   A note can be:
   · a timeline post
   · a public file
   · an exam announcement
   · a notice
   · a forum post
   written by an author in a date-time.
*/

struct TL_Note
  {
   long NotCod;
   TL_NoteType_t NoteType;	// Timeline post, public file, exam announcement, notice, forum post...
   long UsrCod;			// Publisher
   long HieCod;			// Hierarchy code (institution/centre/degree/course)
   long Cod;			// Code of file, forum post, notice, timeline post...
   bool Unavailable;		// File, forum post, notice,... unavailable (removed)
   time_t DateTimeUTC;		// Date-time of publication in UTC time
   unsigned NumShared;		// Number of times (users) this note has been shared
   unsigned NumFavs;		// Number of times (users) this note has been favourited
  };

/* A note can have comments attached to it.
 __________________
|@author           |
|       Note       |
|__________________|
    |@author       |
    |  Comment  1  |
    |______________|
    |@author       |
    |  Comment  2  |
    |______________|
    |              |
    |      ...     |
    |______________|
    |@author       |
    |  Comment  n  |
    |______________|

*/
struct TL_Comment
  {
   long PubCod;
   long UsrCod;			// Publisher
   long NotCod;			// Note code to which this comment belongs
   time_t DateTimeUTC;		// Date-time of publication in UTC time
   unsigned NumFavs;		// Number of times (users) this comment has been favourited
   struct PostContent Content;
  };

typedef enum
  {
   TL_SHOW_A_FEW_USRS,	// Show a few first favers/sharers
   TL_SHOW_ALL_USRS,	// Show all favers/sharers
  } TL_HowMany_t;

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

static void TL_ShowTimelineGblHighlightingNot (long NotCod);
static void TL_ShowTimelineUsrHighlightingNot (long NotCod);

static void TL_GetAndShowOldTimeline (TL_TimelineUsrOrGbl_t TimelineUsrOrGbl);

static void TL_BuildQueryToGetTimeline (char **Query,
                                        TL_TimelineUsrOrGbl_t TimelineUsrOrGbl,
                                        TL_WhatToGetFromTimeline_t WhatToGetFromTimeline);
static long TL_GetPubCodFromSession (const char *FieldName);
static void TL_UpdateLastPubCodIntoSession (void);
static void TL_UpdateFirstPubCodIntoSession (long FirstPubCod);
static void TL_DropTemporaryTablesUsedToQueryTimeline (void);

static void TL_ShowTimeline (char *Query,
                             const char *Title,long NotCodToHighlight);
static void TL_PutIconsTimeline (void);

static void TL_FormStart (Act_Action_t ActionGbl,Act_Action_t ActionUsr);
static void TL_FormFavSha (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
			   const char *ParamCod,
			   const char *Icon,const char *Title);

static void TL_PutFormWho (void);
static void TL_GetParamWho (void);
static Usr_Who_t TL_GetWhoFromDB (void);
static void TL_SaveWhichUsersInDB (void);

static void TL_ShowWarningYouDontFollowAnyUser (void);

static void TL_InsertNewPubsInTimeline (char *Query);
static void TL_ShowOldPubsInTimeline (char *Query);

static void TL_GetDataOfPublicationFromRow (MYSQL_ROW row,struct TL_Publication *SocPub);

static void TL_PutLinkToViewNewPublications (void);
static void TL_PutLinkToViewOldPublications (void);

static void TL_WriteNote (const struct TL_Note *SocNot,
                          TL_TopMessage_t TopMessage,long UsrCod,
                          bool Highlight,
                          bool ShowNoteAlone);
static void TL_WriteTopMessage (TL_TopMessage_t TopMessage,long UsrCod);
static void TL_WriteAuthorNote (const struct UsrData *UsrDat);
static void TL_WriteDateTime (time_t TimeUTC);
static void TL_GetAndWritePost (long PstCod);
static void TL_PutFormGoToAction (const struct TL_Note *SocNot);
static void TL_GetNoteSummary (const struct TL_Note *SocNot,
                               char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1]);
static void TL_PublishNoteInTimeline (struct TL_Publication *SocPub);

static void TL_PutFormToWriteNewPost (void);
static void TL_PutTextarea (const char *Placeholder,const char *ClassTextArea);

static long TL_ReceivePost (void);

static void TL_PutIconToToggleCommentNote (const char UniqueId[Frm_MAX_BYTES_ID + 1]);
static void TL_PutIconCommentDisabled (void);
static void TL_PutHiddenFormToWriteNewCommentToNote (long NotCod,
                                                     const char IdNewComment[Frm_MAX_BYTES_ID + 1]);
static unsigned long TL_GetNumCommentsInNote (long NotCod);
static void TL_WriteCommentsInNote (const struct TL_Note *SocNot,
				    unsigned NumComments);
static void TL_FormToShowHiddenComments (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
			                 long NotCod,
					 char IdComments[Frm_MAX_BYTES_ID + 1],
					 unsigned NumInitialComments);
static unsigned TL_WriteHiddenComments (long NotCod,
					char IdComments[Frm_MAX_BYTES_ID + 1],
					unsigned NumInitialCommentsToGet);
static void TL_WriteOneCommentInList (MYSQL_RES *mysql_res);
static void TL_LinkToShowOnlyLatestComments (const char IdComments[Frm_MAX_BYTES_ID + 1]);
static void TL_LinkToShowPreviousComments (const char IdComments[Frm_MAX_BYTES_ID + 1],
				           unsigned NumInitialComments);
static void TL_PutIconToToggleComments (const char *UniqueId,
                                        const char *Icon,const char *Text);
static void TL_WriteComment (struct TL_Comment *SocCom,
                             TL_TopMessage_t TopMessage,long UsrCod,
                             bool ShowCommentAlone);
static void TL_WriteAuthorComment (struct UsrData *UsrDat);

static void TL_PutFormToRemoveComment (long PubCod);

static void TL_PutDisabledIconShare (unsigned NumShared);
static void TL_PutDisabledIconFav (unsigned NumFavs);

static void TL_PutFormToSeeAllSharersNote (const struct TL_Note *SocNot,
                                           TL_HowMany_t HowMany);
static void TL_PutFormToShaNote (const struct TL_Note *SocNot);
static void TL_PutFormToUnsNote (const struct TL_Note *SocNot);

static void TL_PutFormToSeeAllFaversNote (const struct TL_Note *SocNot,
                                          TL_HowMany_t HowMany);
static void TL_PutFormToFavNote (const struct TL_Note *SocNot);
static void TL_PutFormToUnfNote (const struct TL_Note *SocNot);

static void TL_PutFormToSeeAllFaversComment (const struct TL_Comment *SocCom,
                                             TL_HowMany_t HowMany);
static void TL_PutFormToFavComment (const struct TL_Comment *SocCom);
static void TL_PutFormToUnfComment (const struct TL_Comment *SocCom);

static void TL_PutFormToRemovePublication (long NotCod);

static void TL_PutHiddenParamNotCod (long NotCod);
static long TL_GetParamNotCod (void);
static long TL_GetParamPubCod (void);

static long TL_ReceiveComment (void);

static void TL_PutFormToShaUnsNote (const struct TL_Note *SocNot,
                                    TL_HowMany_t HowMany);
static void TL_ShaNote (struct TL_Note *SocNot);
static void TL_UnsNote (struct TL_Note *SocNot);

static void TL_PutFormToFavUnfNote (const struct TL_Note *SocNot,
                                    TL_HowMany_t HowMany);
static void TL_FavNote (struct TL_Note *SocNot);
static void TL_UnfNote (struct TL_Note *SocNot);

static void TL_PutFormToFavUnfComment (const struct TL_Comment *SocCom,
                                       TL_HowMany_t HowMany);
static void TL_FavComment (struct TL_Comment *SocCom);
static void TL_UnfComment (struct TL_Comment *SocCom);

static void TL_CreateNotifToAuthor (long AuthorCod,long PubCod,
                                    Ntf_NotifyEvent_t NotifyEvent);

static void TL_RequestRemovalNote (void);
static void TL_PutParamsRemoveNote (void);
static void TL_RemoveNote (void);
static void TL_RemoveNoteMediaAndDBEntries (struct TL_Note *SocNot);

static long TL_GetNotCodOfPublication (long PubCod);
static long TL_GetPubCodOfOriginalNote (long NotCod);

static void TL_RequestRemovalComment (void);
static void TL_PutParamsRemoveCommment (void);
static void TL_RemoveComment (void);
static void TL_RemoveCommentMediaAndDBEntries (long PubCod);

static bool TL_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod);
static bool TL_CheckIfNoteIsFavedByUsr (long NotCod,long UsrCod);
static bool TL_CheckIfCommIsFavedByUsr (long PubCod,long UsrCod);

static void TL_UpdateNumTimesANoteHasBeenShared (struct TL_Note *SocNot);
static void TL_GetNumTimesANoteHasBeenFav (struct TL_Note *SocNot);
static void TL_GetNumTimesACommHasBeenFav (struct TL_Comment *SocCom);

static void TL_ShowUsrsWhoHaveSharedNote (const struct TL_Note *SocNot,
					  TL_HowMany_t HowMany);
static void TL_ShowUsrsWhoHaveMarkedNoteAsFav (const struct TL_Note *SocNot,
					       TL_HowMany_t HowMany);
static void TL_ShowUsrsWhoHaveMarkedCommAsFav (const struct TL_Comment *SocCom,
					       TL_HowMany_t HowMany);
static void TL_ShowNumSharersOrFavers (unsigned NumUsrs);
static void TL_ShowSharersOrFavers (MYSQL_RES **mysql_res,
				    unsigned NumUsrs,unsigned NumFirstUsrs);

static void TL_GetDataOfNoteByCod (struct TL_Note *SocNot);
static void TL_GetDataOfCommByCod (struct TL_Comment *SocCom);

static void TL_GetDataOfPublicationFromRow (MYSQL_ROW row,struct TL_Publication *SocPub);
static void TL_GetDataOfNoteFromRow (MYSQL_ROW row,struct TL_Note *SocNot);
static TL_PubType_t TL_GetPubTypeFromStr (const char *Str);
static TL_NoteType_t TL_GetNoteTypeFromStr (const char *Str);
static void TL_GetDataOfCommentFromRow (MYSQL_ROW row,struct TL_Comment *SocCom);

static void TL_ResetNote (struct TL_Note *SocNot);
static void TL_ResetComment (struct TL_Comment *SocCom);

static void TL_ClearTimelineThisSession (void);
static void TL_AddNotesJustRetrievedToTimelineThisSession (void);

static void Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (long PubCod,const char *Txt);

/*****************************************************************************/
/************** Show timeline including all the users I follow ***************/
/*****************************************************************************/

void TL_ShowTimelineGbl1 (void)
  {
   /***** Mark all my notifications about timeline as seen *****/
   TL_MarkMyNotifAsSeen ();

   /***** Get which users *****/
   TL_GetParamWho ();

   /***** Save which users in database *****/
   if (Gbl.Action.Act == ActSeeSocTmlGbl)	// Only in action to see global timeline
      TL_SaveWhichUsersInDB ();
  }

void TL_ShowTimelineGbl2 (void)
  {
   long PubCod;
   struct TL_Note SocNot;
   struct UsrData UsrDat;
   Ntf_NotifyEvent_t NotifyEvent;
   static const TL_TopMessage_t TopMessages[Ntf_NUM_NOTIFY_EVENTS] =
     {
      [Ntf_EVENT_UNKNOWN          ] = TL_TOP_MESSAGE_NONE,
      /* Start tab */
      [Ntf_EVENT_TIMELINE_COMMENT ] = TL_TOP_MESSAGE_COMMENTED,
      [Ntf_EVENT_TIMELINE_FAV     ] = TL_TOP_MESSAGE_FAVED,
      [Ntf_EVENT_TIMELINE_SHARE   ] = TL_TOP_MESSAGE_SHARED,
      [Ntf_EVENT_TIMELINE_MENTION ] = TL_TOP_MESSAGE_MENTIONED,
      [Ntf_EVENT_FOLLOWER         ] = TL_TOP_MESSAGE_NONE,
      /* System tab */
      /* Country tab */
      /* Institution tab */
      /* Centre tab */
      /* Degree tab */
      /* Course tab */
      /* Assessment tab */
      [Ntf_EVENT_ASSIGNMENT       ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_SURVEY           ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_EXAM_ANNOUNCEMENT] = TL_TOP_MESSAGE_NONE,
      /* Files tab */
      [Ntf_EVENT_DOCUMENT_FILE    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_TEACHERS_FILE    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_SHARED_FILE      ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_MARKS_FILE       ] = TL_TOP_MESSAGE_NONE,
      /* Users tab */
      [Ntf_EVENT_ENROLMENT_STD    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_NET    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_TCH    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_REQUEST] = TL_TOP_MESSAGE_NONE,
      /* Messages tab */
      [Ntf_EVENT_NOTICE           ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_FORUM_POST_COURSE] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_FORUM_REPLY      ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_MESSAGE          ] = TL_TOP_MESSAGE_NONE,
      /* Analytics tab */
      /* Profile tab */
     };

   /***** Initialize note code to -1 ==> no highlighted note *****/
   SocNot.NotCod = -1L;

   /***** Get parameter with the code of a publication *****/
   // This parameter is optional. It can be provided by a notification.
   // If > 0 ==> the note is shown highlighted above the timeline
   PubCod = TL_GetParamPubCod ();
   if (PubCod > 0)
      /***** Get code of note from database *****/
      SocNot.NotCod = TL_GetNotCodOfPublication (PubCod);

   if (SocNot.NotCod > 0)
     {
      /* Get who did the action (publication, commenting, faving, sharing, mentioning) */
      Usr_GetParamOtherUsrCodEncrypted (&UsrDat);

      /* Get what he/she did */
      NotifyEvent = Ntf_GetParamNotifyEvent ();

      /***** Show the note highlighted *****/
      TL_GetDataOfNoteByCod (&SocNot);
      TL_WriteNote (&SocNot,
		    TopMessages[NotifyEvent],UsrDat.UsrCod,
		    true,true);
     }

   /***** Show timeline with possible highlighted note *****/
   TL_ShowTimelineGblHighlightingNot (SocNot.NotCod);
  }

static void TL_ShowTimelineGblHighlightingNot (long NotCod)
  {
   extern const char *Txt_Timeline;
   char *Query = NULL;

   /***** Build query to get timeline *****/
   TL_BuildQueryToGetTimeline (&Query,
	                       TL_TIMELINE_GBL,
                               TL_GET_RECENT_TIMELINE);

   /***** Show timeline *****/
   TL_ShowTimeline (Query,Txt_Timeline,NotCod);

   /***** Drop temporary tables *****/
   TL_DropTemporaryTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/********************* Show timeline of a selected user **********************/
/*****************************************************************************/

void TL_ShowTimelineUsr (void)
  {
   TL_ShowTimelineUsrHighlightingNot (-1L);
  }

static void TL_ShowTimelineUsrHighlightingNot (long NotCod)
  {
   extern const char *Txt_Timeline_OF_A_USER;
   char *Query = NULL;

   /***** Build query to show timeline with publications of a unique user *****/
   TL_BuildQueryToGetTimeline (&Query,
	                       TL_TIMELINE_USR,
                               TL_GET_RECENT_TIMELINE);

   /***** Show timeline *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_Timeline_OF_A_USER,
	     Gbl.Usrs.Other.UsrDat.FirstName);
   TL_ShowTimeline (Query,Gbl.Title,NotCod);

   /***** Drop temporary tables *****/
   TL_DropTemporaryTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/************** Refresh new publications in timeline via AJAX ****************/
/*****************************************************************************/

void TL_RefreshNewTimelineGbl (void)
  {
   char *Query = NULL;

   if (Gbl.Session.IsOpen)	// If session has been closed, do not write anything
     {
      /***** Get which users *****/
      TL_GetParamWho ();

      /***** Build query to get timeline *****/
      TL_BuildQueryToGetTimeline (&Query,
	                          TL_TIMELINE_GBL,
				  TL_GET_ONLY_NEW_PUBS);

      /***** Show new timeline *****/
      TL_InsertNewPubsInTimeline (Query);

      /***** Drop temporary tables *****/
      TL_DropTemporaryTablesUsedToQueryTimeline ();
     }
  }

/*****************************************************************************/
/**************** View old publications in timeline via AJAX *****************/
/*****************************************************************************/

void TL_RefreshOldTimelineGbl (void)
  {
   /***** Get which users *****/
   TL_GetParamWho ();

   /***** Show old publications *****/
   TL_GetAndShowOldTimeline (TL_TIMELINE_GBL);
  }

void TL_RefreshOldTimelineUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())	// Existing user
      /***** If user exists, show old publications *****/
      TL_GetAndShowOldTimeline (TL_TIMELINE_USR);
  }

/*****************************************************************************/
/**************** Get and show old publications in timeline ******************/
/*****************************************************************************/

static void TL_GetAndShowOldTimeline (TL_TimelineUsrOrGbl_t TimelineUsrOrGbl)
  {
   char *Query = NULL;

   /***** Build query to get timeline *****/
   TL_BuildQueryToGetTimeline (&Query,
	                       TimelineUsrOrGbl,
                               TL_GET_ONLY_OLD_PUBS);

   /***** Show old timeline *****/
   TL_ShowOldPubsInTimeline (Query);

   /***** Drop temporary tables *****/
   TL_DropTemporaryTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/************ Mark all my notifications about timeline as seen ***************/
/*****************************************************************************/
// Must be executed as a priori function

void TL_MarkMyNotifAsSeen (void)
  {
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_COMMENT,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_FAV    ,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_SHARE  ,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ntf_MarkNotifAsSeen (Ntf_EVENT_TIMELINE_MENTION,-1L,-1L,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************ Build query to get timeline ************************/
/*****************************************************************************/

#define TL_MAX_BYTES_SUBQUERY_ALREADY_EXISTS (256 - 1)

static void TL_BuildQueryToGetTimeline (char **Query,
                                        TL_TimelineUsrOrGbl_t TimelineUsrOrGbl,
                                        TL_WhatToGetFromTimeline_t WhatToGetFromTimeline)
  {
   char SubQueryPublishers[128];
   char SubQueryRangeBottom[128];
   char SubQueryRangeTop[128];
   char SubQueryAlreadyExists[TL_MAX_BYTES_SUBQUERY_ALREADY_EXISTS + 1];
   struct
     {
      long Top;
      long Bottom;
     } RangePubsToGet;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPubs;
   unsigned NumPub;
   long PubCod;
   long NotCod;
   static const unsigned MaxPubsToGet[TL_NUM_WHAT_TO_GET_FROM_TIMELINE] =
     {
      [TL_GET_ONLY_NEW_PUBS  ] = TL_MAX_NEW_PUBS_TO_GET_AND_SHOW,
      [TL_GET_RECENT_TIMELINE] = TL_MAX_REC_PUBS_TO_GET_AND_SHOW,
      [TL_GET_ONLY_OLD_PUBS  ] = TL_MAX_OLD_PUBS_TO_GET_AND_SHOW,
     };

   /***** Clear timeline for this session in database *****/
   if (WhatToGetFromTimeline == TL_GET_RECENT_TIMELINE)
      TL_ClearTimelineThisSession ();

   /***** Drop temporary tables *****/
   TL_DropTemporaryTablesUsedToQueryTimeline ();

   /***** Create temporary table with publication codes *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_pub_codes "
	     "(PubCod BIGINT NOT NULL,UNIQUE INDEX(PubCod)) ENGINE=MEMORY");

   /***** Create temporary table with notes got in this execution *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_not_codes "
	     "(NotCod BIGINT NOT NULL,INDEX(NotCod)) ENGINE=MEMORY");

   /***** Create temporary table with notes already present in timeline for this session *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_current_timeline "
	     "(NotCod BIGINT NOT NULL,INDEX(NotCod)) ENGINE=MEMORY"
	     " SELECT NotCod FROM tl_timelines WHERE SessionId='%s'",
	     Gbl.Session.Id);

   /***** Create temporary table and subquery with potential publishers *****/
   switch (TimelineUsrOrGbl)
     {
      case TL_TIMELINE_USR:	// Show the timeline of a user
	 sprintf (SubQueryPublishers,"PublisherCod=%ld AND ",
	          Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case TL_TIMELINE_GBL:	// Show the global timeline
	 switch (Gbl.Timeline.Who)
	   {
	    case Usr_WHO_ME:	// Show my timeline
	       sprintf (SubQueryPublishers,"PublisherCod=%ld AND ",
	                Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
	    case Usr_WHO_FOLLOWED:	// Show the timeline of the users I follow
	       DB_Query ("can not create temporary table",
		         "CREATE TEMPORARY TABLE tl_publishers "
			 "(UsrCod INT NOT NULL,UNIQUE INDEX(UsrCod)) ENGINE=MEMORY"
			 " SELECT %ld AS UsrCod"
			 " UNION"
			 " SELECT FollowedCod AS UsrCod"
			 " FROM usr_follow WHERE FollowerCod=%ld",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 Gbl.Usrs.Me.UsrDat.UsrCod);

	       sprintf (SubQueryPublishers,
			"tl_pubs.PublisherCod=tl_publishers.UsrCod AND ");
	       break;
	    case Usr_WHO_ALL:	// Show the timeline of all users
	       SubQueryPublishers[0] = '\0';
	       break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong parameter which users.");
	       break;
	   }
	 break;
     }

   /***** Create subquery to get only notes not present in timeline *****/
   switch (TimelineUsrOrGbl)
     {
      case TL_TIMELINE_USR:	// Show the timeline of a user
	 switch (WhatToGetFromTimeline)
           {
            case TL_GET_ONLY_NEW_PUBS:
            case TL_GET_RECENT_TIMELINE:
	       Str_Copy (SubQueryAlreadyExists,
	                 " NotCod NOT IN"
			 " (SELECT NotCod FROM tl_not_codes)",
			 TL_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
            case TL_GET_ONLY_OLD_PUBS:
	       Str_Copy (SubQueryAlreadyExists,
	                 " NotCod NOT IN"
			 " (SELECT NotCod FROM tl_current_timeline)",
			 TL_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
           }
	 break;
      case TL_TIMELINE_GBL:	// Show the timeline of the users I follow
	 switch (WhatToGetFromTimeline)
           {
            case TL_GET_ONLY_NEW_PUBS:
            case TL_GET_RECENT_TIMELINE:
	       Str_Copy (SubQueryAlreadyExists,
	                 " tl_pubs.NotCod NOT IN"
			 " (SELECT NotCod FROM tl_not_codes)",
			 TL_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
            case TL_GET_ONLY_OLD_PUBS:
	       Str_Copy (SubQueryAlreadyExists,
	                 " tl_pubs.NotCod NOT IN"
			 " (SELECT NotCod FROM tl_current_timeline)",
			 TL_MAX_BYTES_SUBQUERY_ALREADY_EXISTS);
	       break;
           }
	 break;
     }

   /***** Get the publications in timeline *****/
   /* Initialize range of pubs:

              tl_pubs
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
      case TL_GET_ONLY_NEW_PUBS:	 // Get the publications (without limit) newer than LastPubCod
	 /* This query is made via AJAX automatically from time to time */
	 RangePubsToGet.Bottom = TL_GetPubCodFromSession ("LastPubCod");
	 break;
      case TL_GET_RECENT_TIMELINE:	 // Get some limited recent publications
	 /* This is the first query to get initial timeline shown
	    ==> no notes yet in current timeline table */
	 break;
      case TL_GET_ONLY_OLD_PUBS:	 // Get some limited publications older than FirstPubCod
	 /* This query is made via AJAX
	    when I click in link to get old publications */
	 RangePubsToGet.Top    = TL_GetPubCodFromSession ("FirstPubCod");
	 break;
     }

   /*
      With the current approach, we select one by one
      the publications and notes in a loop. In each iteration,
      we get the more recent publication (original, shared or commment)
      of every set of publications corresponding to the same note,
      checking that the note is not already retrieved.
      After getting a publication, its note code is stored
      in order to not get it again.

      As an alternative, we tried to get the maximum PubCod,
      i.e more recent publication (original, shared or commment),
      of every set of publications corresponding to the same note:
      "SELECT MAX(PubCod) AS NewestPubCod FROM tl_pubs ...
      " GROUP BY NotCod ORDER BY NewestPubCod DESC LIMIT ..."
      but this query is slow (several seconds) with a big table.
   */

   for (NumPub = 0;
	NumPub < MaxPubsToGet[WhatToGetFromTimeline];
	NumPub++)
     {
      /* Create subqueries with range of publications to get from tl_pubs */
      if (RangePubsToGet.Bottom > 0)
	 switch (TimelineUsrOrGbl)
	   {
	    case TL_TIMELINE_USR:	// Show the timeline of a user
	       sprintf (SubQueryRangeBottom,"PubCod>%ld AND ",
		        RangePubsToGet.Bottom);
	       break;
	    case TL_TIMELINE_GBL:	// Show the global timeline
	       switch (Gbl.Timeline.Who)
		 {
		  case Usr_WHO_ME:	// Show my timeline
		  case Usr_WHO_ALL:	// Show the timeline of all users
		     sprintf (SubQueryRangeBottom,"PubCod>%ld AND ",
			      RangePubsToGet.Bottom);
		     break;
		  case Usr_WHO_FOLLOWED:// Show the timeline of the users I follow
		     sprintf (SubQueryRangeBottom,"tl_pubs.PubCod>%ld AND ",
			      RangePubsToGet.Bottom);
		     break;
		  default:
		     Lay_ShowErrorAndExit ("Wrong parameter which users.");
		     break;
		 }
	       break;
	   }
      else
	 SubQueryRangeBottom[0] = '\0';

      if (RangePubsToGet.Top > 0)
	 switch (TimelineUsrOrGbl)
	   {
	    case TL_TIMELINE_USR:	// Show the timeline of a user
	       sprintf (SubQueryRangeTop,"PubCod<%ld AND ",
		        RangePubsToGet.Top);
	       break;
	    case TL_TIMELINE_GBL:	// Show the global timeline
	       switch (Gbl.Timeline.Who)
		 {
		  case Usr_WHO_ME:	// Show my timeline
		  case Usr_WHO_ALL:	// Show the timeline of all users
		     sprintf (SubQueryRangeTop,"PubCod<%ld AND ",
			      RangePubsToGet.Top);
		     break;
		  case Usr_WHO_FOLLOWED:// Show the timeline of the users I follow
		     sprintf (SubQueryRangeTop,"tl_pubs.PubCod<%ld AND ",
			      RangePubsToGet.Top);
		     break;
		  default:
		     Lay_ShowErrorAndExit ("Wrong parameter which users.");
		     break;
		 }
	       break;
	   }
      else
	 SubQueryRangeTop[0] = '\0';

      /* Select the most recent publication from tl_pubs */
      NumPubs = 0;	// Initialized to avoid warning
      switch (TimelineUsrOrGbl)
	{
	 case TL_TIMELINE_USR:	// Show the timeline of a user
	    NumPubs =
	    (unsigned) DB_QuerySELECT (&mysql_res,"can not get publication",
				       "SELECT PubCod,NotCod"
				       " FROM tl_pubs"
				       " WHERE %s%s%s%s"
				       " ORDER BY PubCod DESC LIMIT 1",
				       SubQueryRangeBottom,SubQueryRangeTop,
				       SubQueryPublishers,
				       SubQueryAlreadyExists);
	    break;
	 case TL_TIMELINE_GBL:	// Show the global timeline
	    switch (Gbl.Timeline.Who)
	      {
	       case Usr_WHO_ME:		// Show my timeline
		  NumPubs =
		  (unsigned) DB_QuerySELECT (&mysql_res,"can not get publication",
					     "SELECT PubCod,NotCod"
					     " FROM tl_pubs"
					     " WHERE %s%s%s%s"
					     " ORDER BY PubCod DESC LIMIT 1",
					     SubQueryRangeBottom,SubQueryRangeTop,
					     SubQueryPublishers,
					     SubQueryAlreadyExists);
		  break;
	       case Usr_WHO_FOLLOWED:	// Show the timeline of the users I follow
		  NumPubs =
		  (unsigned) DB_QuerySELECT (&mysql_res,"can not get publication",
				             "SELECT PubCod,NotCod"
				             " FROM tl_pubs,tl_publishers"
					     " WHERE %s%s%s%s"
					     " ORDER BY tl_pubs.PubCod DESC LIMIT 1",
					     SubQueryRangeBottom,SubQueryRangeTop,
					     SubQueryPublishers,
					     SubQueryAlreadyExists);
		  break;
	       case Usr_WHO_ALL:	// Show the timeline of all users
		  NumPubs =
		  (unsigned) DB_QuerySELECT (&mysql_res,"can not get publication",
				             "SELECT PubCod,NotCod"
				             " FROM tl_pubs"
					     " WHERE %s%s%s"
					     " ORDER BY PubCod DESC LIMIT 1",
					     SubQueryRangeBottom,SubQueryRangeTop,
					     SubQueryAlreadyExists);
		  break;
	       default:
		  Lay_ShowErrorAndExit ("Wrong parameter which users.");
		  break;
	      }
	    break;
	}

      if (NumPubs == 1)
	{
	 /* Get code of publication */
	 row = mysql_fetch_row (mysql_res);
	 PubCod = Str_ConvertStrCodToLongCod (row[0]);
	}
      else
        {
	 row = NULL;
	 PubCod = -1L;
        }

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      if (PubCod > 0)
	{
	 DB_QueryINSERT ("can not store publication code",
			 "INSERT INTO tl_pub_codes SET PubCod=%ld",
			 PubCod);
	 RangePubsToGet.Top = PubCod;	// Narrow the range for the next iteration

	 /* Get note code (row[1]) */
	 if (row)
	   {
	    NotCod = Str_ConvertStrCodToLongCod (row[1]);
	    DB_QueryINSERT ("can not store note code",
			    "INSERT INTO tl_not_codes SET NotCod=%ld",
			    NotCod);
	    DB_QueryINSERT ("can not store note code",
			    "INSERT INTO tl_current_timeline SET NotCod=%ld",
			    NotCod);
	   }
	}
      else	// Nothing got ==> abort loop
         break;	// Last publication
     }

   /***** Update last publication code into session for next refresh *****/
   // Do this inmediately after getting the publications codes...
   // ...in order to not lose publications
   TL_UpdateLastPubCodIntoSession ();

   /***** Add notes just retrieved to current timeline for this session *****/
   TL_AddNotesJustRetrievedToTimelineThisSession ();

   /***** Build query to show timeline including the users I am following *****/
   DB_BuildQuery (Query,
	          "SELECT PubCod,"			// row[0]
	                 "NotCod,"			// row[1]
	                 "PublisherCod,"		// row[2]
	                 "PubType,"			// row[3]
	                 "UNIX_TIMESTAMP(TimePublish)"	// row[4]
		  " FROM tl_pubs"
		  " WHERE PubCod IN "
		  "(SELECT PubCod"
		  " FROM tl_pub_codes)"
		  " ORDER BY PubCod DESC");
  }

/*****************************************************************************/
/************* Get last/first publication code stored in session *************/
/*****************************************************************************/
// FieldName can be:
// "LastPubCod"
// "FirstPubCod"

static long TL_GetPubCodFromSession (const char *FieldName)
  {
   extern const char *Txt_The_session_has_expired;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;

   /***** Get last publication code from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get publication code from session",
		       "SELECT %s FROM sessions"
		       " WHERE SessionId='%s'",
		       FieldName,Gbl.Session.Id) != 1)
      Lay_ShowErrorAndExit (Txt_The_session_has_expired);

   /***** Get last publication code *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&PubCod) != 1)
      PubCod = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PubCod;
  }

/*****************************************************************************/
/*********************** Update last publication code ************************/
/*****************************************************************************/

static void TL_UpdateLastPubCodIntoSession (void)
  {
   /***** Update last publication code *****/
   DB_QueryUPDATE ("can not update last publication code into session",
		   "UPDATE sessions"
	           " SET LastPubCod="
	           "(SELECT IFNULL(MAX(PubCod),0) FROM tl_pubs)"
	           " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/*********************** Update first publication code ***********************/
/*****************************************************************************/

static void TL_UpdateFirstPubCodIntoSession (long FirstPubCod)
  {
   /***** Update last publication code *****/
   DB_QueryUPDATE ("can not update first publication code into session",
		   "UPDATE sessions SET FirstPubCod=%ld WHERE SessionId='%s'",
		   FirstPubCod,Gbl.Session.Id);
  }

/*****************************************************************************/
/*************** Drop temporary tables used to query timeline ****************/
/*****************************************************************************/

static void TL_DropTemporaryTablesUsedToQueryTimeline (void)
  {
   DB_Query ("can not remove temporary tables",
	     "DROP TEMPORARY TABLE IF EXISTS "
	     "tl_pub_codes,"
	     "tl_not_codes,"
	     "tl_publishers,"
	     "tl_current_timeline");
  }

/*****************************************************************************/
/******************************* Show timeline *******************************/
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
static void TL_ShowTimeline (char *Query,
                             const char *Title,long NotCodToHighlight)
  {
   extern const char *Hlp_START_Timeline;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPub;
   struct TL_Publication SocPub;
   struct TL_Note SocNot;
   bool GlobalTimeline = (Gbl.Usrs.Other.UsrDat.UsrCod <= 0);
   bool ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Get publications from database *****/
   NumPubsGot = DB_QuerySELECT (&mysql_res,"can not get timeline",
				"%s",
				Query);
   /***** Begin box *****/
   Box_BoxBegin (NULL,Title,TL_PutIconsTimeline,
                 Hlp_START_Timeline,Box_NOT_CLOSABLE);

   /***** Put form to select users whom public activity is displayed *****/
   if (GlobalTimeline)
      TL_PutFormWho ();

   /***** Form to write a new post *****/
   if (GlobalTimeline || ItsMe)
      TL_PutFormToWriteNewPost ();

   /***** New publications refreshed dynamically via AJAX *****/
   if (GlobalTimeline)
     {
      /* Link to view new publications via AJAX */
      TL_PutLinkToViewNewPublications ();

      /* Hidden list where insert just received (not visible) publications via AJAX */
      HTM_UL_Begin ("id=\"just_now_timeline_list\" class=\"TL_LIST\"");
      HTM_UL_End ();

      /* Hidden list where insert new (not visible) publications via AJAX */
      HTM_UL_Begin ("id=\"new_timeline_list\" class=\"TL_LIST\"");
      HTM_UL_End ();
     }

   /***** List recent publications in timeline *****/
   HTM_UL_Begin ("id=\"timeline_list\" class=\"TL_LIST\"");

   for (NumPub = 0, SocPub.PubCod = 0;
	NumPub < NumPubsGot;
	NumPub++)
     {
      /* Get data of publication */
      row = mysql_fetch_row (mysql_res);
      TL_GetDataOfPublicationFromRow (row,&SocPub);

      /* Get data of note */
      SocNot.NotCod = SocPub.NotCod;
      TL_GetDataOfNoteByCod (&SocNot);

      /* Write note */
      TL_WriteNote (&SocNot,
                    SocPub.TopMessage,SocPub.PublisherCod,
		    SocNot.NotCod == NotCodToHighlight,
		    false);
     }
   HTM_UL_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Store first publication code into session *****/
   TL_UpdateFirstPubCodIntoSession (SocPub.PubCod);

   if (NumPubsGot == TL_MAX_REC_PUBS_TO_GET_AND_SHOW)
     {
      /***** Link to view old publications via AJAX *****/
      TL_PutLinkToViewOldPublications ();

      /***** Hidden list where insert old publications via AJAX *****/
      HTM_UL_Begin ("id=\"old_timeline_list\" class=\"TL_LIST\"");
      HTM_UL_End ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Put contextual icons in timeline **********************/
/*****************************************************************************/

static void TL_PutIconsTimeline (void)
  {
   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_TIMELINE;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/***************** Start a form in global or user timeline *******************/
/*****************************************************************************/

static void TL_FormStart (Act_Action_t ActionGbl,Act_Action_t ActionUsr)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Frm_StartFormAnchor (ActionUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted ();
     }
   else
     {
      Frm_StartForm (ActionGbl);
      Usr_PutHiddenParamWho (Gbl.Timeline.Who);
     }
  }

/*****************************************************************************/
/******* Form to fav/unfav or share/unshare in global or user timeline *******/
/*****************************************************************************/

static void TL_FormFavSha (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
			   const char *ParamCod,
			   const char *Icon,const char *Title)
  {
   char *OnSubmit;

   /*
   +---------------------------------------------------------------------------+
   | div which content will be updated (parent of parent of form)              |
   | +---------------------+ +-------+ +-------------------------------------+ |
   | | div (parent of form)| | div   | | div for users                       | |
   | | +-----------------+ | | for   | | +------+ +------+ +------+ +------+ | |
   | | |    this form    | | | num.  | | |      | |      | |      | | form | | |
   | | | +-------------+ | | | of    | | | user | | user | | user | |  to  | | |
   | | | |   fav icon  | | | | users | | |   1  | |   2  | |   3  | | show | | |
   | | | +-------------+ | | |       | | |      | |      | |      | |  all | | |
   | | +-----------------+ | |       | | +------+ +------+ +------+ +------+ | |
   | +---------------------+ +-------+ +-------------------------------------+ |
   +---------------------------------------------------------------------------+
   */

   /***** Form and icon to mark note as favourite *****/
   /* Form with icon */
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (asprintf (&OnSubmit,"updateDivFaversSharers(this,"
			      "'act=%ld&ses=%s&%s&OtherUsrCod=%s');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (ActionUsr),
		    Gbl.Session.Id,
		    ParamCod,
		    Gbl.Usrs.Other.UsrDat.EncryptedUsrCod) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,"timeline",OnSubmit);
     }
   else
     {
      if (asprintf (&OnSubmit,"updateDivFaversSharers(this,"
			      "'act=%ld&ses=%s&%s');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (ActionGbl),
		    Gbl.Session.Id,
		    ParamCod) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,NULL,OnSubmit);
     }
   Ico_PutIconLink (Icon,Title);
   Frm_EndForm ();

   /* Free allocated memory */
   free (OnSubmit);
  }

/*****************************************************************************/
/******** Show form to select users whom public activity is displayed ********/
/*****************************************************************************/

static void TL_PutFormWho (void)
  {
   Usr_Who_t Who;
   unsigned Mask = 1 << Usr_WHO_ME       |
	           1 << Usr_WHO_FOLLOWED |
		   1 << Usr_WHO_ALL;

   /***** Setting selector for which users *****/
   Set_StartSettingsHead ();
   Set_StartOneSettingSelector ();
   for (Who  = (Usr_Who_t) 0;
	Who <= (Usr_Who_t) (Usr_NUM_WHO - 1);
	Who++)
      if (Mask & (1 << Who))
	{
	 HTM_DIV_Begin ("class=\"%s\"",
			Who == Gbl.Timeline.Who ? "PREF_ON" :
						  "PREF_OFF");
	 Frm_StartForm (ActSeeSocTmlGbl);
	 Par_PutHiddenParamUnsigned (NULL,"Who",(unsigned) Who);
	 Usr_PutWhoIcon (Who);
	 Frm_EndForm ();
	 HTM_DIV_End ();
	}
   Set_EndOneSettingSelector ();
   Set_EndSettingsHead ();

   /***** Show warning if I do not follow anyone *****/
   if (Gbl.Timeline.Who == Usr_WHO_FOLLOWED)
      TL_ShowWarningYouDontFollowAnyUser ();
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

static void TL_GetParamWho (void)
  {
   /***** Get which users I want to see *****/
   Gbl.Timeline.Who = Usr_GetHiddenParamWho ();

   /***** If parameter Who is not present, get it from database *****/
   if (Gbl.Timeline.Who == Usr_WHO_UNKNOWN)
      Gbl.Timeline.Who = TL_GetWhoFromDB ();

   /***** If parameter Who is unknown, set it to default *****/
   if (Gbl.Timeline.Who == Usr_WHO_UNKNOWN)
      Gbl.Timeline.Who = TL_DEFAULT_WHO;
  }

/*****************************************************************************/
/********** Get user's last data from database giving a user's code **********/
/*****************************************************************************/

static Usr_Who_t TL_GetWhoFromDB (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   Usr_Who_t Who = Usr_WHO_UNKNOWN;

   /***** Get which users from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get timeline users from user's last data",
		       "SELECT TimelineUsrs"		   // row[0]
		       " FROM usr_last WHERE UsrCod=%ld",
		       Gbl.Usrs.Me.UsrDat.UsrCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get who */
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
         if (UnsignedNum < Usr_NUM_WHO)
            Who = (Usr_Who_t) UnsignedNum;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Who;
  }

/*****************************************************************************/
/********************** Save which users into database ***********************/
/*****************************************************************************/

static void TL_SaveWhichUsersInDB (void)
  {
   if (Gbl.Usrs.Me.Logged)
     {
      if (Gbl.Timeline.Who == Usr_WHO_UNKNOWN)
	 Gbl.Timeline.Who = TL_DEFAULT_WHO;

      /***** Update which users in database *****/
      // Who is stored in usr_last for next time I log in
      DB_QueryUPDATE ("can not update timeline users in user's last data",
		      "UPDATE usr_last SET TimelineUsrs=%u"
		      " WHERE UsrCod=%ld",
		      (unsigned) Gbl.Timeline.Who,
		      Gbl.Usrs.Me.UsrDat.UsrCod);
     }
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

static void TL_ShowWarningYouDontFollowAnyUser (void)
  {
   extern const char *Txt_You_dont_follow_any_user;
   unsigned NumFollowing;
   unsigned NumFollowers;

   /***** Check if I follow someone *****/
   Fol_GetNumFollow (Gbl.Usrs.Me.UsrDat.UsrCod,&NumFollowing,&NumFollowers);
   if (!NumFollowing)
     {
      /***** Show warning if I do not follow anyone *****/
      Ale_ShowAlert (Ale_WARNING,Txt_You_dont_follow_any_user);

      /***** Contextual menu *****/
      Mnu_ContextMenuBegin ();
      Fol_PutLinkWhoToFollow ();	// Users to follow
      Mnu_ContextMenuEnd ();
     }
  }

/*****************************************************************************/
/******************* Show new publications in timeline ***********************/
/*****************************************************************************/
// The publications are inserted as list elements of a hidden list

static void TL_InsertNewPubsInTimeline (char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPub;
   struct TL_Publication SocPub;
   struct TL_Note SocNot;

   /***** Get new publications timeline from database *****/
   NumPubsGot = DB_QuerySELECT (&mysql_res,"can not get timeline",
				"%s",
				Query);

   /***** List new publications timeline *****/
   for (NumPub = 0;
	NumPub < NumPubsGot;
	NumPub++)
     {
      /* Get data of publication */
      row = mysql_fetch_row (mysql_res);
      TL_GetDataOfPublicationFromRow (row,&SocPub);

      /* Get data of note */
      SocNot.NotCod = SocPub.NotCod;
      TL_GetDataOfNoteByCod (&SocNot);

      /* Write note */
      TL_WriteNote (&SocNot,
                    SocPub.TopMessage,SocPub.PublisherCod,
                    false,false);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Show old publications in timeline *********************/
/*****************************************************************************/
// The publications are inserted as list elements of a hidden list

static void TL_ShowOldPubsInTimeline (char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumPubsGot;
   unsigned long NumPub;
   struct TL_Publication SocPub;
   struct TL_Note SocNot;

   /***** Get old publications timeline from database *****/
   NumPubsGot = DB_QuerySELECT (&mysql_res,"can not get timeline",
				"%s",
				Query);

   /***** List old publications in timeline *****/
   for (NumPub = 0, SocPub.PubCod = 0;
	NumPub < NumPubsGot;
	NumPub++)
     {
      /* Get data of publication */
      row = mysql_fetch_row (mysql_res);
      TL_GetDataOfPublicationFromRow (row,&SocPub);

      /* Get data of note */
      SocNot.NotCod = SocPub.NotCod;
      TL_GetDataOfNoteByCod (&SocNot);

      /* Write note */
      TL_WriteNote (&SocNot,
                    SocPub.TopMessage,SocPub.PublisherCod,
                    false,false);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Store first publication code into session *****/
   TL_UpdateFirstPubCodIntoSession (SocPub.PubCod);
  }

/*****************************************************************************/
/***************** Put link to view new publications in timeline *************/
/*****************************************************************************/

static void TL_PutLinkToViewNewPublications (void)
  {
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_See_new_activity;

   /***** Link to view (show hidden) new publications *****/
   // div is hidden. When new posts arrive to the client via AJAX, div is shown
   HTM_DIV_Begin ("id=\"view_new_posts_container\""
		  " class=\"TL_WIDTH TL_SEP VERY_LIGHT_BLUE\""
		  " style=\"display:none;\"");
   HTM_A_Begin ("href=\"\" class=\"%s\""
                " onclick=\"moveNewTimelineToTimeline();return false;\"",
	        The_ClassFormInBoxBold[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s (",Txt_See_new_activity);
   HTM_SPAN_Begin ("id=\"view_new_posts_count\"");
   HTM_Unsigned (0);
   HTM_SPAN_End ();
   HTM_Txt (")");
   HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Put link to view old publications in timeline *************/
/*****************************************************************************/

static void TL_PutLinkToViewOldPublications (void)
  {
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_See_more;

   /***** Animated link to view old publications *****/
   HTM_DIV_Begin ("id=\"view_old_posts_container\""
	          " class=\"TL_WIDTH TL_SEP VERY_LIGHT_BLUE\"");
   HTM_A_Begin ("href=\"\" class=\"%s\" onclick=\""
   		"document.getElementById('get_old_timeline').style.display='none';"	// Icon to be hidden on click
		"document.getElementById('getting_old_timeline').style.display='';"	// Icon to be shown on click
                "refreshOldTimeline();"
		"return false;\"",
	        The_ClassFormInBoxBold[Gbl.Prefs.Theme]);
   HTM_IMG (Cfg_URL_ICON_PUBLIC,"recycle16x16.gif","Txt_See_more",
	    "class=\"ICO20x20\" id=\"get_old_timeline\"");
   HTM_IMG (Cfg_URL_ICON_PUBLIC,"working16x16.gif",Txt_See_more,
	    "class=\"ICO20x20\" style=\"display:none;\" id=\"getting_old_timeline\"");	// Animated icon hidden
   HTM_IMG (Cfg_URL_ICON_PUBLIC,"recycle16x16.gif","Txt_See_more",
	    "class=\"ICO20x20\" style=\"display:none;\" id=\"get_old_timeline\"");
   HTM_TxtF ("&nbsp;%s",Txt_See_more);
   HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************************** Write note *********************************/
/*****************************************************************************/

static void TL_WriteNote (const struct TL_Note *SocNot,
                          TL_TopMessage_t TopMessage,long UsrCod,
                          bool Highlight,	// Highlight note
                          bool ShowNoteAlone)	// Note is shown alone, not in a list
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData UsrDat;
   bool IAmTheAuthor;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];
   unsigned NumComments;
   char IdNewComment[Frm_MAX_BYTES_ID + 1];
   static unsigned NumDiv = 0;	// Used to create unique div id for fav and shared

   NumDiv++;

   /***** Begin box ****/
   if (ShowNoteAlone)
     {
      Box_BoxBegin (NULL,NULL,NULL,
                    NULL,Box_CLOSABLE);
      HTM_UL_Begin ("class=\"TL_LIST\"");
     }

   /***** Start list item *****/
   HTM_LI_Begin ("class=\"%s\"",
		 ShowNoteAlone ? (Highlight ? "TL_WIDTH TL_NEW_PUB" :
					      "TL_WIDTH") :
				 (Highlight ? "TL_WIDTH TL_SEP TL_NEW_PUB" :
					      "TL_WIDTH TL_SEP"));

   if (SocNot->NotCod   <= 0 ||
       SocNot->NoteType == TL_NOTE_UNKNOWN ||
       SocNot->UsrCod   <= 0)
      Ale_ShowAlert (Ale_ERROR,"Error in note.");
   else
     {
      /***** Initialize location in hierarchy *****/
      Ins.InsCod = -1L;
      Ctr.CtrCod = -1L;
      Deg.DegCod = -1L;
      Crs.CrsCod = -1L;

      /***** Write sharer/commenter if distinct to author *****/
      TL_WriteTopMessage (TopMessage,UsrCod);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get author data *****/
      UsrDat.UsrCod = SocNot->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
      IAmTheAuthor = Usr_ItsMe (UsrDat.UsrCod);

      /***** Left: write author's photo *****/
      HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO45x60",Pho_ZOOM,true);	// Use unique id
      HTM_DIV_End ();

      /***** Right: author's name, time, summary and buttons *****/
      /* Begin right container */
      HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");

      /* Write author's full name and date-time */
      TL_WriteAuthorNote (&UsrDat);
      TL_WriteDateTime (SocNot->DateTimeUTC);

      /* Write content of the note */
      if (SocNot->NoteType == TL_NOTE_POST)
	 /* Write post content */
	 TL_GetAndWritePost (SocNot->Cod);
      else
	{
	 /* Get location in hierarchy */
	 if (!SocNot->Unavailable)
	    switch (SocNot->NoteType)
	      {
	       case TL_NOTE_INS_DOC_PUB_FILE:
	       case TL_NOTE_INS_SHA_PUB_FILE:
		  /* Get institution data */
		  Ins.InsCod = SocNot->HieCod;
		  Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_BASIC_DATA);
		  break;
	       case TL_NOTE_CTR_DOC_PUB_FILE:
	       case TL_NOTE_CTR_SHA_PUB_FILE:
		  /* Get centre data */
		  Ctr.CtrCod = SocNot->HieCod;
		  Ctr_GetDataOfCentreByCod (&Ctr);
		  break;
	       case TL_NOTE_DEG_DOC_PUB_FILE:
	       case TL_NOTE_DEG_SHA_PUB_FILE:
		  /* Get degree data */
		  Deg.DegCod = SocNot->HieCod;
		  Deg_GetDataOfDegreeByCod (&Deg);
		  break;
	       case TL_NOTE_CRS_DOC_PUB_FILE:
	       case TL_NOTE_CRS_SHA_PUB_FILE:
	       case TL_NOTE_EXAM_ANNOUNCEMENT:
	       case TL_NOTE_NOTICE:
		  /* Get course data */
		  Crs.CrsCod = SocNot->HieCod;
		  Crs_GetDataOfCourseByCod (&Crs);
		  break;
	       case TL_NOTE_FORUM_POST:
		  /* Get forum type of the post */
		  For_GetForumTypeAndLocationOfAPost (SocNot->Cod,&Gbl.Forum.ForumSelected);
		  For_SetForumName (&Gbl.Forum.ForumSelected,
		                    ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
		  break;
	       default:
		  break;
	      }

	 /* Write note type */
	 TL_PutFormGoToAction (SocNot);

	 /* Write location in hierarchy */
	 if (!SocNot->Unavailable)
	    switch (SocNot->NoteType)
	      {
	       case TL_NOTE_INS_DOC_PUB_FILE:
	       case TL_NOTE_INS_SHA_PUB_FILE:
		  /* Write location (institution) in hierarchy */
		  HTM_DIV_Begin ("class=\"TL_LOC\"");
		  HTM_TxtF ("%s:&nbsp;%s",Txt_Institution,Ins.ShrtName);
		  HTM_DIV_End ();
		  break;
	       case TL_NOTE_CTR_DOC_PUB_FILE:
	       case TL_NOTE_CTR_SHA_PUB_FILE:
		  /* Write location (centre) in hierarchy */
		  HTM_DIV_Begin ("class=\"TL_LOC\"");
		  HTM_TxtF ("%s:&nbsp;%s",Txt_Centre,Ctr.ShrtName);
		  HTM_DIV_End ();
		  break;
	       case TL_NOTE_DEG_DOC_PUB_FILE:
	       case TL_NOTE_DEG_SHA_PUB_FILE:
		  /* Write location (degree) in hierarchy */
		  HTM_DIV_Begin ("class=\"TL_LOC\"");
		  HTM_TxtF ("%s:&nbsp;%s",Txt_Degree,Deg.ShrtName);
		  HTM_DIV_End ();
		  break;
	       case TL_NOTE_CRS_DOC_PUB_FILE:
	       case TL_NOTE_CRS_SHA_PUB_FILE:
	       case TL_NOTE_EXAM_ANNOUNCEMENT:
	       case TL_NOTE_NOTICE:
		  /* Write location (course) in hierarchy */
		  HTM_DIV_Begin ("class=\"TL_LOC\"");
		  HTM_TxtF ("%s:&nbsp;%s",Txt_Course,Crs.ShrtName);
		  HTM_DIV_End ();
		  break;
	       case TL_NOTE_FORUM_POST:
		  /* Write forum name */
		  HTM_DIV_Begin ("class=\"TL_LOC\"");
		  HTM_TxtF ("%s:&nbsp;%s",Txt_Forum,ForumName);
		  HTM_DIV_End ();
		  break;
	       default:
		  break;
	      }

	 /* Write note summary */
	 TL_GetNoteSummary (SocNot,SummaryStr);
	 HTM_DIV_Begin ("class=\"TL_TXT\"");
	 HTM_Txt (SummaryStr);
	 HTM_DIV_End ();
	}

      /* End right container */
      HTM_DIV_End ();

      /***** Buttons and comments *****/
      /* Create unique id for new comment */
      Frm_SetUniqueId (IdNewComment);

      /* Get number of comments in this note */
      NumComments = TL_GetNumCommentsInNote (SocNot->NotCod);

      /* Put icon to add a comment */
      HTM_DIV_Begin ("class=\"TL_BOTTOM_LEFT\"");
      if (SocNot->Unavailable)		// Unavailable notes can not be commented
	 TL_PutIconCommentDisabled ();
      else
         TL_PutIconToToggleCommentNote (IdNewComment);
      HTM_DIV_End ();

      /* Start container for buttons and comments */
      HTM_DIV_Begin ("class=\"TL_BOTTOM_RIGHT TL_RIGHT_WIDTH\"");

      /* Start foot container */
      HTM_DIV_Begin ("class=\"TL_FOOT TL_RIGHT_WIDTH\"");

      /* Foot column 1: Fav zone */
      HTM_DIV_Begin ("id=\"fav_not_%s_%u\" class=\"TL_FAV_NOT TL_FAV_NOT_WIDTH\"",
	             Gbl.UniqueNameEncrypted,NumDiv);
      TL_PutFormToFavUnfNote (SocNot,TL_SHOW_A_FEW_USRS);
      HTM_DIV_End ();

      /* Foot column 2: Share zone */
      HTM_DIV_Begin ("id=\"sha_not_%s_%u\" class=\"TL_SHA_NOT TL_SHA_NOT_WIDTH\"",
	             Gbl.UniqueNameEncrypted,NumDiv);
      TL_PutFormToShaUnsNote (SocNot,TL_SHOW_A_FEW_USRS);
      HTM_DIV_End ();

      /* Foot column 3: Icon to remove this note */
      HTM_DIV_Begin ("class=\"TL_REM\"");
      if (IAmTheAuthor)
	 TL_PutFormToRemovePublication (SocNot->NotCod);
      HTM_DIV_End ();

      /* End foot container */
      HTM_DIV_End ();

      /* Comments */
      if (NumComments)
	 TL_WriteCommentsInNote (SocNot,NumComments);

      /* End container for buttons and comments */
      HTM_DIV_End ();

      /* Put hidden form to write a new comment */
      TL_PutHiddenFormToWriteNewCommentToNote (SocNot->NotCod,IdNewComment);

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** End list item *****/
   HTM_LI_End ();

   /***** End box ****/
   if (ShowNoteAlone)
     {
      HTM_UL_End ();
      Box_BoxEnd ();
     }
  }

/*****************************************************************************/
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/

static void TL_WriteTopMessage (TL_TopMessage_t TopMessage,long UsrCod)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *Txt_TIMELINE_NOTE_TOP_MESSAGES[TL_NUM_TOP_MESSAGES];
   struct UsrData UsrDat;
   bool ItsMe = Usr_ItsMe (UsrCod);

   if (TopMessage != TL_TOP_MESSAGE_NONE)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Get user's data *****/
      UsrDat.UsrCod = UsrCod;
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))	// Really we only need EncryptedUsrCod and FullName
	{
	 HTM_DIV_Begin ("class=\"TL_TOP_CONT TL_TOP_PUBLISHER TL_WIDTH\"");

	 /***** Show user's name inside form to go to user's public profile *****/
	 Frm_StartFormUnique (ActSeeOthPubPrf);
	 Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
	 HTM_BUTTON_SUBMIT_Begin (ItsMe ? Txt_My_public_profile :
					  Txt_Another_user_s_profile,
				  "BT_LINK TL_TOP_PUBLISHER",NULL);
	 HTM_Txt (UsrDat.FullName);
	 HTM_BUTTON_End ();
	 Frm_EndForm ();

	 /***** Show action made *****/
         HTM_TxtF (" %s:",Txt_TIMELINE_NOTE_TOP_MESSAGES[TopMessage]);

         HTM_DIV_End ();
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
  }

/*****************************************************************************/
/*************** Write name and nickname of author of a note *****************/
/*****************************************************************************/

static void TL_WriteAuthorNote (const struct UsrData *UsrDat)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   /***** Show user's name inside form to go to user's public profile *****/
   Frm_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   HTM_BUTTON_SUBMIT_Begin (ItsMe ? Txt_My_public_profile :
				    Txt_Another_user_s_profile,
		            "BT_LINK TL_RIGHT_AUTHOR TL_RIGHT_AUTHOR_WIDTH DAT_N_BOLD",
			    NULL);
   HTM_Txt (UsrDat->FullName);
   HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Write the date of creation of a note ********************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

static void TL_WriteDateTime (time_t TimeUTC)
  {
   char IdDateTime[Frm_MAX_BYTES_ID + 1];

   /***** Create unique Id *****/
   Frm_SetUniqueId (IdDateTime);

   /***** Container where the date-time is written *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"TL_RIGHT_TIME DAT_LIGHT\"",IdDateTime);
   HTM_DIV_End ();

   /***** Script to write date and time in browser local time *****/
   // This must be out of the div where the output is written
   // because it will be evaluated in a loop in JavaScript
   Dat_WriteLocalDateHMSFromUTC (IdDateTime,TimeUTC,
		                 Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				 true,true,false,0x6);
  }

/*****************************************************************************/
/***************** Get from database and write public post *******************/
/*****************************************************************************/

static void TL_GetAndWritePost (long PstCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   struct PostContent Content;

   /***** Initialize image *****/
   Med_MediaConstructor (&Content.Media);

   /***** Get post from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the content"
					" of a post",
			     "SELECT Txt,"		// row[0]
			            "MedCod"		// row[1]
			     " FROM tl_posts"
			     " WHERE PstCod=%ld",
			     PstCod);

   /***** Result should have a unique row *****/
   if (NumRows == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /****** Get content (row[0]) *****/
      Str_Copy (Content.Txt,row[0],
                Cns_MAX_BYTES_LONG_TEXT);

      /***** Get media (row[1]) *****/
      Content.Media.MedCod = Str_ConvertStrCodToLongCod (row[1]);
      Med_GetMediaDataByCod (&Content.Media);
     }
   else
      Content.Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Write content *****/
   if (Content.Txt[0])
     {
      HTM_DIV_Begin ("class=\"TL_TXT\"");
      Msg_WriteMsgContent (Content.Txt,Cns_MAX_BYTES_LONG_TEXT,true,false);
      HTM_DIV_End ();
     }

   /***** Show image *****/
   Med_ShowMedia (&Content.Media,"TL_PST_MED_CONT TL_RIGHT_WIDTH",
	                         "TL_PST_MED TL_RIGHT_WIDTH");

   /***** Free image *****/
   Med_MediaDestructor (&Content.Media);
  }

/*****************************************************************************/
/************* Put form to go to an action depending on the note *************/
/*****************************************************************************/

static void TL_PutFormGoToAction (const struct TL_Note *SocNot)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_TIMELINE_NOTE[TL_NUM_NOTE_TYPES];
   extern const char *Txt_not_available;
   char *Class;
   char *Anchor = NULL;
   static const Act_Action_t TL_DefaultActions[TL_NUM_NOTE_TYPES] =
     {
      [TL_NOTE_UNKNOWN          ] = ActUnk,
      /* Start tab */
      [TL_NOTE_POST             ] = ActUnk,	// action not used
      /* Institution tab */
      [TL_NOTE_INS_DOC_PUB_FILE ] = ActReqDatSeeDocIns,
      [TL_NOTE_INS_SHA_PUB_FILE ] = ActReqDatShaIns,
      /* Centre tab */
      [TL_NOTE_CTR_DOC_PUB_FILE ] = ActReqDatSeeDocCtr,
      [TL_NOTE_CTR_SHA_PUB_FILE ] = ActReqDatShaCtr,
      /* Degree tab */
      [TL_NOTE_DEG_DOC_PUB_FILE ] = ActReqDatSeeDocDeg,
      [TL_NOTE_DEG_SHA_PUB_FILE ] = ActReqDatShaDeg,
      /* Course tab */
      [TL_NOTE_CRS_DOC_PUB_FILE ] = ActReqDatSeeDocCrs,
      [TL_NOTE_CRS_SHA_PUB_FILE ] = ActReqDatShaCrs,
      /* Assessment tab */
      [TL_NOTE_EXAM_ANNOUNCEMENT] = ActSeeOneExaAnn,
      /* Users tab */
      /* Messages tab */
      [TL_NOTE_NOTICE           ] = ActSeeOneNot,
      [TL_NOTE_FORUM_POST       ] = ActSeeFor,
      /* Analytics tab */
      /* Profile tab */
     };
   static const char *TL_Icons[TL_NUM_NOTE_TYPES] =
     {
      [TL_NOTE_UNKNOWN          ] = NULL,
      /* Start tab */
      [TL_NOTE_POST             ] = NULL,	// icon not used
      /* Institution tab */
      [TL_NOTE_INS_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_INS_SHA_PUB_FILE ] = "file.svg",
      /* Centre tab */
      [TL_NOTE_CTR_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_CTR_SHA_PUB_FILE ] = "file.svg",
      /* Degree tab */
      [TL_NOTE_DEG_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_DEG_SHA_PUB_FILE ] = "file.svg",
      /* Course tab */
      [TL_NOTE_CRS_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_CRS_SHA_PUB_FILE ] = "file.svg",
      /* Assessment tab */
      [TL_NOTE_EXAM_ANNOUNCEMENT] = "bullhorn.svg",
      /* Users tab */
      /* Messages tab */
      [TL_NOTE_NOTICE           ] = "sticky-note.svg",
      [TL_NOTE_FORUM_POST       ] = "comments.svg",
      /* Analytics tab */
      /* Profile tab */
     };

   if (SocNot->Unavailable ||	// File/notice... pointed by this note is unavailable
       Gbl.Form.Inside)		// Inside another form
     {
      /***** Do not put form *****/
      HTM_DIV_Begin ("class=\"TL_FORM_OFF\"");
      HTM_Txt (Txt_TIMELINE_NOTE[SocNot->NoteType]);
      if (SocNot->Unavailable)
         HTM_TxtF ("&nbsp;(%s)",Txt_not_available);
      HTM_DIV_End ();
     }
   else			// Not inside another form
     {
      HTM_DIV_Begin ("class=\"TL_FORM\"");

      /***** Start form with parameters depending on the type of note *****/
      switch (SocNot->NoteType)
	{
	 case TL_NOTE_INS_DOC_PUB_FILE:
	 case TL_NOTE_INS_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.Hierarchy.Ins.InsCod)	// Not the current institution
	       Ins_PutParamInsCod (SocNot->HieCod);		// Go to another institution
	    break;
	 case TL_NOTE_CTR_DOC_PUB_FILE:
	 case TL_NOTE_CTR_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.Hierarchy.Ctr.CtrCod)	// Not the current centre
	       Ctr_PutParamCtrCod (SocNot->HieCod);		// Go to another centre
	    break;
	 case TL_NOTE_DEG_DOC_PUB_FILE:
	 case TL_NOTE_DEG_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.Hierarchy.Deg.DegCod)	// Not the current degree
	       Deg_PutParamDegCod (SocNot->HieCod);		// Go to another degree
	    break;
	 case TL_NOTE_CRS_DOC_PUB_FILE:
	 case TL_NOTE_CRS_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[SocNot->NoteType]);
	    Brw_PutHiddenParamFilCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_EXAM_ANNOUNCEMENT:
            Frm_SetAnchorStr (SocNot->Cod,&Anchor);
	    Frm_StartFormUniqueAnchor (TL_DefaultActions[SocNot->NoteType],
		                       Anchor);	// Locate on this specific exam
            Frm_FreeAnchorStr (Anchor);
	    Exa_PutHiddenParamExaCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_POST:	// Not applicable
	    return;
	 case TL_NOTE_FORUM_POST:
	    Frm_StartFormUnique (For_ActionsSeeFor[Gbl.Forum.ForumSelected.Type]);
	    For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                         1,	// Page of posts   = first
                                         Gbl.Forum.ForumSet,
					 Gbl.Forum.ThreadsOrder,
					 Gbl.Forum.ForumSelected.Location,
					 Gbl.Forum.ForumSelected.ThrCod,
					 -1L);
	    if (SocNot->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_NOTICE:
            Frm_SetAnchorStr (SocNot->Cod,&Anchor);
	    Frm_StartFormUniqueAnchor (TL_DefaultActions[SocNot->NoteType],
				       Anchor);
            Frm_FreeAnchorStr (Anchor);
	    Not_PutHiddenParamNotCod (SocNot->Cod);
	    if (SocNot->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (SocNot->HieCod);		// Go to another course
	    break;
	 default:			// Not applicable
	    return;
	}

      /***** Icon and link to go to action *****/
      if (asprintf (&Class,"BT_LINK %s ICO_HIGHLIGHT",
		    The_ClassFormInBoxBold[Gbl.Prefs.Theme]) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_BUTTON_SUBMIT_Begin (Txt_TIMELINE_NOTE[SocNot->NoteType],Class,NULL);
      Ico_PutIcon (TL_Icons[SocNot->NoteType],Txt_TIMELINE_NOTE[SocNot->NoteType],"CONTEXT_ICO_x16");
      HTM_TxtF ("&nbsp;%s",Txt_TIMELINE_NOTE[SocNot->NoteType]);
      HTM_BUTTON_End ();
      free (Class);

      /***** End form *****/
      Frm_EndForm ();

      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/********************** Get note summary and content *************************/
/*****************************************************************************/

static void TL_GetNoteSummary (const struct TL_Note *SocNot,
                               char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1])
  {
   SummaryStr[0] = '\0';

   switch (SocNot->NoteType)
     {
      case TL_NOTE_UNKNOWN:
          break;
      case TL_NOTE_INS_DOC_PUB_FILE:
      case TL_NOTE_INS_SHA_PUB_FILE:
      case TL_NOTE_CTR_DOC_PUB_FILE:
      case TL_NOTE_CTR_SHA_PUB_FILE:
      case TL_NOTE_DEG_DOC_PUB_FILE:
      case TL_NOTE_DEG_SHA_PUB_FILE:
      case TL_NOTE_CRS_DOC_PUB_FILE:
      case TL_NOTE_CRS_SHA_PUB_FILE:
	 Brw_GetSummaryAndContentOfFile (SummaryStr,NULL,SocNot->Cod,false);
         break;
      case TL_NOTE_EXAM_ANNOUNCEMENT:
         Exa_GetSummaryAndContentExamAnnouncement (SummaryStr,NULL,SocNot->Cod,false);
         break;
      case TL_NOTE_POST:
	 // Not applicable
         break;
      case TL_NOTE_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,SocNot->Cod,false);
         break;
      case TL_NOTE_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,SocNot->Cod,false);
         break;
     }
  }

/*****************************************************************************/
/***************** Store and publish a note into database ********************/
/*****************************************************************************/
// Return the code of the new note just created

void TL_StoreAndPublishNote (TL_NoteType_t NoteType,long Cod,struct TL_Publication *SocPub)
  {
   long HieCod;	// Hierarchy code (institution/centre/degree/course)

   switch (NoteType)
     {
      case TL_NOTE_INS_DOC_PUB_FILE:
      case TL_NOTE_INS_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Ins.InsCod;
	 break;
      case TL_NOTE_CTR_DOC_PUB_FILE:
      case TL_NOTE_CTR_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Ctr.CtrCod;
	 break;
      case TL_NOTE_DEG_DOC_PUB_FILE:
      case TL_NOTE_DEG_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Deg.DegCod;
	 break;
      case TL_NOTE_CRS_DOC_PUB_FILE:
      case TL_NOTE_CRS_SHA_PUB_FILE:
      case TL_NOTE_EXAM_ANNOUNCEMENT:
      case TL_NOTE_NOTICE:
	 HieCod = Gbl.Hierarchy.Crs.CrsCod;
	 break;
      default:
	 HieCod = -1L;
         break;
     }

   /***** Store note *****/
   SocPub->NotCod =
   DB_QueryINSERTandReturnCode ("can not create new note",
				"INSERT INTO tl_notes"
				" (NoteType,Cod,UsrCod,HieCod,Unavailable,TimeNote)"
				" VALUES"
				" (%u,%ld,%ld,%ld,'N',NOW())",
				(unsigned) NoteType,
				Cod,Gbl.Usrs.Me.UsrDat.UsrCod,HieCod);

   /***** Publish note in timeline *****/
   SocPub->PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   SocPub->PubType      = TL_PUB_ORIGINAL_NOTE;
   TL_PublishNoteInTimeline (SocPub);
  }

/*****************************************************************************/
/************************* Mark a note as unavailable ************************/
/*****************************************************************************/

void TL_MarkNoteAsUnavailableUsingNotCod (long NotCod)
  {
   /***** Mark the note as unavailable *****/
   DB_QueryUPDATE ("can not mark note as unavailable",
		   "UPDATE tl_notes SET Unavailable='Y'"
		   " WHERE NotCod=%ld",
		   NotCod);
  }

void TL_MarkNoteAsUnavailableUsingNoteTypeAndCod (TL_NoteType_t NoteType,long Cod)
  {
   /***** Mark the note as unavailable *****/
   DB_QueryUPDATE ("can not mark note as unavailable",
		   "UPDATE tl_notes SET Unavailable='Y'"
		   " WHERE NoteType=%u AND Cod=%ld",
		   (unsigned) NoteType,Cod);
  }

/*****************************************************************************/
/****************** Mark notes of one file as unavailable ********************/
/*****************************************************************************/

void TL_MarkNoteOneFileAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;
   TL_NoteType_t NoteType;

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
	    /***** Mark possible note as unavailable *****/
	    switch (FileBrowser)
	      {
	       case Brw_ADMI_DOC_INS:
		  NoteType = TL_NOTE_INS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_INS:
		  NoteType = TL_NOTE_INS_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_CTR:
		  NoteType = TL_NOTE_CTR_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_CTR:
		  NoteType = TL_NOTE_CTR_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_DEG:
		  NoteType = TL_NOTE_DEG_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_DEG:
		  NoteType = TL_NOTE_DEG_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_CRS:
		  NoteType = TL_NOTE_CRS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_CRS:
		  NoteType = TL_NOTE_CRS_SHA_PUB_FILE;
		  break;
	       default:
		  return;
	      }
	    TL_MarkNoteAsUnavailableUsingNoteTypeAndCod (NoteType,FilCod);
	   }
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/***** Mark possible notes involving children of a folder as unavailable *****/
/*****************************************************************************/

void TL_MarkNotesChildrenOfFolderAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long Cod = Brw_GetCodForFiles ();
   TL_NoteType_t NoteType;

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
	 /***** Mark possible note as unavailable *****/
	 switch (FileBrowser)
	   {
	    case Brw_ADMI_DOC_INS:
	       NoteType = TL_NOTE_INS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_INS:
	       NoteType = TL_NOTE_INS_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_CTR:
	       NoteType = TL_NOTE_CTR_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_CTR:
	       NoteType = TL_NOTE_CTR_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_DEG:
	       NoteType = TL_NOTE_DEG_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_DEG:
	       NoteType = TL_NOTE_DEG_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_CRS:
	       NoteType = TL_NOTE_CRS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_CRS:
	       NoteType = TL_NOTE_CRS_SHA_PUB_FILE;
	       break;
	    default:
	       return;
	   }
         DB_QueryUPDATE ("can not mark notes as unavailable",
			 "UPDATE tl_notes SET Unavailable='Y'"
		         " WHERE NoteType=%u AND Cod IN"
	                 " (SELECT FilCod FROM files"
			 " WHERE FileBrowser=%u AND Cod=%ld"
			 " AND Path LIKE '%s/%%' AND Public='Y')",	// Only public files
			 (unsigned) NoteType,
			 (unsigned) FileBrowser,Cod,
			 Path);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/************************* Publish note in timeline **************************/
/*****************************************************************************/
// SocPub->PubCod is set by the function

static void TL_PublishNoteInTimeline (struct TL_Publication *SocPub)
  {
   /***** Publish note in timeline *****/
   SocPub->PubCod =
   DB_QueryINSERTandReturnCode ("can not publish note",
				"INSERT INTO tl_pubs"
				" (NotCod,PublisherCod,PubType,TimePublish)"
				" VALUES"
				" (%ld,%ld,%u,NOW())",
				SocPub->NotCod,
				SocPub->PublisherCod,
				(unsigned) SocPub->PubType);

   /***** Increment number of publications in user's figures *****/
   Prf_IncrementNumSocPubUsr (SocPub->PublisherCod);
  }

/*****************************************************************************/
/********************** Form to write a new publication **********************/
/*****************************************************************************/

static void TL_PutFormToWriteNewPost (void)
  {
   extern const char *Txt_New_TIMELINE_post;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   /***** Start list *****/
   HTM_UL_Begin ("class=\"TL_LIST\"");
   HTM_LI_Begin ("class=\"TL_WIDTH\"");

   /***** Left: write author's photo (my photo) *****/
   HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
						     NULL,
		     "PHOTO45x60",Pho_ZOOM,false);
   HTM_DIV_End ();

   /***** Right: author's name, time, textarea *****/
   HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");

   /* Author name */
   TL_WriteAuthorNote (&Gbl.Usrs.Me.UsrDat);

   /* Form to write the post */
   HTM_DIV_Begin ("class=\"TL_FORM_NEW_PST TL_RIGHT_WIDTH\"");
   TL_FormStart (ActRcvSocPstGbl,ActRcvSocPstUsr);
   TL_PutTextarea (Txt_New_TIMELINE_post,"TL_PST_TEXTAREA TL_RIGHT_WIDTH");
   Frm_EndForm ();
   HTM_DIV_End ();

   HTM_DIV_End ();

   /***** End list *****/
   HTM_LI_End ();
   HTM_UL_End ();
  }

/*****************************************************************************/
/*** Put textarea and button inside a form to submit a new post or comment ***/
/*****************************************************************************/

static void TL_PutTextarea (const char *Placeholder,const char *ClassTextArea)
  {
   extern const char *Txt_Post;
   char IdDivImgButton[Frm_MAX_BYTES_ID + 1];

   /***** Set unique id for the hidden div *****/
   Frm_SetUniqueId (IdDivImgButton);

   /***** Textarea to write the content *****/
   HTM_TEXTAREA_Begin ("name=\"Txt\" rows=\"1\" maxlength=\"%u\""
                       " placeholder=\"%s&hellip;\" class=\"%s\""
	               " onfocus=\"expandTextarea(this,'%s','6');\"",
		       TL_MAX_CHARS_IN_POST,
		       Placeholder,ClassTextArea,
		       IdDivImgButton);
   HTM_TEXTAREA_End ();

   /***** Start concealable div *****/
   HTM_DIV_Begin ("id=\"%s\" style=\"display:none;\"",IdDivImgButton);

   /***** Help on editor *****/
   Lay_HelpPlainEditor ();

   /***** Attached image (optional) *****/
   Med_PutMediaUploader (-1,"TL_MED_INPUT_WIDTH");

   /***** Submit button *****/
   HTM_BUTTON_SUBMIT_Begin (NULL,"BT_SUBMIT_INLINE BT_CREATE",NULL);
   HTM_Txt (Txt_Post);
   HTM_BUTTON_End ();

   /***** End hidden div *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************* Receive and store a new public post *********************/
/*****************************************************************************/

void TL_ReceivePostUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Receive and store post, and
          write updated timeline after publication (user) *****/
   NotCod = TL_ReceivePost ();
   TL_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_ReceivePostGbl (void)
  {
   long NotCod;

   /***** Receive and store post *****/
   NotCod = TL_ReceivePost ();

   /***** Write updated timeline after publication (global) *****/
   TL_ShowTimelineGblHighlightingNot (NotCod);
  }

// Returns the code of the note just created
static long TL_ReceivePost (void)
  {
   struct PostContent Content;
   long PstCod;
   struct TL_Publication SocPub;

   /***** Get the content of the new post *****/
   Par_GetParAndChangeFormat ("Txt",Content.Txt,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /***** Initialize image *****/
   Med_MediaConstructor (&Content.Media);

   /***** Get attached image (action, file and title) *****/
   Content.Media.Width   = TL_IMAGE_SAVED_MAX_WIDTH;
   Content.Media.Height  = TL_IMAGE_SAVED_MAX_HEIGHT;
   Content.Media.Quality = TL_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (-1,&Content.Media,NULL,NULL);
   Ale_ShowAlerts (NULL);

   if (Content.Txt[0] ||		// Text not empty
      Content.Media.Status == Med_PROCESSED)	// A media is attached
     {
      /***** Store media in filesystem and database *****/
      Med_RemoveKeepOrStoreMedia (-1L,&Content.Media);

      /***** Publish *****/
      /* Insert post content in the database */
      PstCod =
      DB_QueryINSERTandReturnCode ("can not create post",
				   "INSERT INTO tl_posts"
				   " (Txt,MedCod)"
				   " VALUES"
				   " ('%s',%ld)",
				   Content.Txt,
				   Content.Media.MedCod);

      /* Insert post in notes */
      TL_StoreAndPublishNote (TL_NOTE_POST,PstCod,&SocPub);

      /***** Analyze content and store notifications about mentions *****/
      Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (SocPub.PubCod,Content.Txt);
     }
   else	// Text and image are empty
      SocPub.NotCod = -1L;

   /***** Free image *****/
   Med_MediaDestructor (&Content.Media);

   return SocPub.NotCod;
  }

/*****************************************************************************/
/********* Put an icon to toggle on/off the form to comment a note ***********/
/*****************************************************************************/

static void TL_PutIconToToggleCommentNote (const char UniqueId[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_Comment;

   /***** Link to toggle on/off the form to comment a note *****/
   HTM_DIV_Begin ("id=\"%s_ico\" class=\"TL_ICO_COM_OFF\"",UniqueId);
   HTM_A_Begin ("href=\"\" onclick=\"toggleNewComment ('%s');return false;\"",
                UniqueId);
   Ico_PutIcon ("edit.svg",Txt_Comment,"CONTEXT_ICO_16x16");
   HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off the form to comment a note **********/
/*****************************************************************************/

static void TL_PutIconCommentDisabled (void)
  {
   extern const char *Txt_Comment;

   /***** Disabled icon to comment a note *****/
   HTM_DIV_Begin ("class=\"TL_ICO_COM_OFF TL_ICO_DISABLED\"");
   Ico_PutIcon ("edit.svg",Txt_Comment,"ICO16x16");
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Form to comment a publication ************************/
/*****************************************************************************/

static void TL_PutHiddenFormToWriteNewCommentToNote (long NotCod,
                                                     const char IdNewComment[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_New_TIMELINE_comment;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];

   /***** Start container *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"TL_FORM_NEW_COM TL_RIGHT_WIDTH\""
		  " style=\"display:none;\"",
	          IdNewComment);

   /***** Left: write author's photo (my photo) *****/
   HTM_DIV_Begin ("class=\"TL_COM_PHOTO\"");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Me.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,ShowPhoto ? PhotoURL :
					             NULL,
		     "PHOTO30x40",Pho_ZOOM,true);	// Use unique id
   HTM_DIV_End ();

   /***** Right: form to write the comment *****/
   /* Start right container */
   HTM_DIV_Begin ("class=\"TL_COM_CONT TL_COMM_WIDTH\"");

   /* Begin form to write the post */
   TL_FormStart (ActRcvSocComGbl,ActRcvSocComUsr);
   TL_PutHiddenParamNotCod (NotCod);

   /* Textarea and button */
   TL_PutTextarea (Txt_New_TIMELINE_comment,
	           "TL_COM_TEXTAREA TL_COMM_WIDTH");

   /* End form */
   Frm_EndForm ();

   /* End right container */
   HTM_DIV_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Get number of comments in a note **********************/
/*****************************************************************************/

static unsigned long TL_GetNumCommentsInNote (long NotCod)
  {
   return DB_QueryCOUNT ("can not get number of comments in a note",
			 "SELECT COUNT(*) FROM tl_pubs"
			 " WHERE NotCod=%ld AND PubType=%u",
			 NotCod,(unsigned) TL_PUB_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*********************** Write comments in a note ****************************/
/*****************************************************************************/

static void TL_WriteCommentsInNote (const struct TL_Note *SocNot,
				    unsigned NumComments)
  {
   MYSQL_RES *mysql_res;
   unsigned NumInitialComments;
   unsigned NumFinalCommentsToGet;
   unsigned NumFinalCommentsGot;
   unsigned NumCom;
   char IdComments[Frm_MAX_BYTES_ID + 1];

   /***** Compute how many initial comments will be hidden
          and how many final comments will be visible *****/
   // Never hide only one comment
   // So, the number of comments initially hidden must be 0 or >= 2
   if (NumComments <= TL_NUM_VISIBLE_COMMENTS + 1)
     {
      NumInitialComments    = 0;
      NumFinalCommentsToGet = NumComments;
     }
   else
     {
      NumInitialComments    = NumComments - TL_NUM_VISIBLE_COMMENTS;
      NumFinalCommentsToGet = TL_NUM_VISIBLE_COMMENTS;
     }

   /***** Get last comments of this note from database *****/
   NumFinalCommentsGot = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get comments",
			      "SELECT * FROM "
			      "("
			      "SELECT tl_pubs.PubCod,"		// row[0]
				     "tl_pubs.PublisherCod,"	// row[1]
				     "tl_pubs.NotCod,"		// row[2]
				     "UNIX_TIMESTAMP("
				     "tl_pubs.TimePublish),"	// row[3]
				     "tl_comments.Txt,"		// row[4]
				     "tl_comments.MedCod"	// row[5]
			      " FROM tl_pubs,tl_comments"
			      " WHERE tl_pubs.NotCod=%ld"
			      " AND tl_pubs.PubType=%u"
			      " AND tl_pubs.PubCod=tl_comments.PubCod"
			      " ORDER BY tl_pubs.PubCod DESC LIMIT %u"
			      ") AS comments"
			      " ORDER BY PubCod",
			      SocNot->NotCod,(unsigned) TL_PUB_COMMENT_TO_NOTE,
			      NumFinalCommentsToGet);

   /*
      Before clicking "See prev..."    -->    After clicking "See prev..."
    _________________________________       _________________________________
   |           div con_<id>          |     |           div con_<id>          |
   |            (hidden)             |     |            (visible)            |
   |  _____________________________  |     |  _____________________________  |
   | |    v See only the latest    | |     | |    v See only the latest    | |
   | |_____________________________| |     | |_____________________________| |
   |_________________________________|     |_________________________________|
    _________________________________       _________________________________
   |            div <id>             |     |        div <id> updated         |
   |          which content          |     |  _____________________________  |
   |    will be updated via AJAX     |     | |         ul com_<id>         | |
   |   (parent of parent of form)    |     | |  _________________________  | |
   |                                 |     | | |     li (comment 1)      | | |
   |                                 |     | | |_________________________| | |
   |                                 |     | | |           ...           | | |
   |                                 |     | | |_________________________| | |
   |                                 |     | | |     li (comment n)      | | |
   |                                 | --> | | |_________________________| | |
   |                                 |     | |_____________________________| |
   |  _____________________________  |     |  _____________________________  |
   | |        div exp_<id>         | |     | |         div exp_<id>        | |
   | |  _________________________  | |     | |          (hidden)           | |
   | | |          form           | | |     | |                             | |
   | | |  _____________________  | | |     | |    _____________________    | |
   | | | | ^ See prev.comments | | | |     | |   | ^ See prev.comments |   | |
   | | | |_____________________| | | |     | |   |_____________________|   | |
   | | |_________________________| | |     | |                             | |
   | |_____________________________| |     | |_____________________________| |
   |_________________________________|     |_________________________________|
    _________________________________       _________________________________
   |           ul com_<id>           |     |           ul com_<id>           |
   |    _________________________    |     |    _________________________    |
   |   |     li (comment 1)      |   |     |   |     li (comment 1)      |   |
   |   |_________________________|   |     |   |_________________________|   |
   |   |           ...           |   |     |   |           ...           |   |
   |   |_________________________|   |     |   |_________________________|   |
   |   |     li (comment n)      |   |     |   |     li (comment n)      |   |
   |   |_________________________|   |     |   |_________________________|   |
   |_________________________________|     |_________________________________|
   */
   /***** Link to show initial hidden comments *****/
   if (NumInitialComments)
     {
      /***** Create unique id for list of hidden comments *****/
      Frm_SetUniqueId (IdComments);

      /***** Link (initially hidden) to show only the latest comments *****/
      TL_LinkToShowOnlyLatestComments (IdComments);

      /***** Div which content will be updated via AJAX *****/
      HTM_DIV_Begin ("id=\"%s\" class=\"TL_RIGHT_WIDTH\"",IdComments);
      TL_FormToShowHiddenComments (ActShoHidSocComGbl,ActShoHidSocComUsr,
				   SocNot->NotCod,
				   IdComments,
				   NumInitialComments);
      HTM_DIV_End ();
     }

   /***** List final visible comments *****/
   if (NumFinalCommentsGot)
     {
      HTM_UL_Begin ("class=\"TL_LIST\"");
      for (NumCom = 0;
	   NumCom < NumFinalCommentsGot;
	   NumCom++)
	 TL_WriteOneCommentInList (mysql_res);
      HTM_UL_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Form to show hidden coments in global or user timeline ***********/
/*****************************************************************************/

static void TL_FormToShowHiddenComments (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
			                 long NotCod,
					 char IdComments[Frm_MAX_BYTES_ID + 1],
					 unsigned NumInitialComments)
  {
   extern const char *The_ClassFormLinkInBox[The_NUM_THEMES];
   extern const char *Txt_See_the_previous_X_COMMENTS;
   char *OnSubmit;

   HTM_DIV_Begin ("id=\"exp_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\"",
		  IdComments);

   /***** Form and icon-text to show hidden comments *****/
   /* Start form */
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      if (asprintf (&OnSubmit,"toggleComments('%s');"
	                      "updateDivHiddenComments(this,"
			      "'act=%ld&ses=%s&NotCod=%ld&IdComments=%s&NumHidCom=%u&OtherUsrCod=%s');"
			      " return false;",	// return false is necessary to not submit form
		    IdComments,
		    Act_GetActCod (ActionUsr),
		    Gbl.Session.Id,
		    NotCod,
		    IdComments,
		    NumInitialComments,
		    Gbl.Usrs.Other.UsrDat.EncryptedUsrCod) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,"timeline",OnSubmit);
     }
   else
     {
      if (asprintf (&OnSubmit,"toggleComments('%s');"
	                      "updateDivHiddenComments(this,"
			      "'act=%ld&ses=%s&NotCod=%ld&IdComments=%s&NumHidCom=%u');"
			      " return false;",	// return false is necessary to not submit form
		    IdComments,
		    Act_GetActCod (ActionGbl),
		    Gbl.Session.Id,
		    NotCod,
		    IdComments,
		    NumInitialComments) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,NULL,OnSubmit);
     }

   /* Put icon and text with link to show the first hidden comments */
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_See_the_previous_X_COMMENTS,
	     NumInitialComments);
   HTM_BUTTON_SUBMIT_Begin (NULL,The_ClassFormLinkInBox[Gbl.Prefs.Theme],NULL);
   Ico_PutIconTextLink ("angle-up.svg",Gbl.Title);
   HTM_BUTTON_End ();

   /* End form */
   Frm_EndForm ();

   /* Free allocated memory */
   free (OnSubmit);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Write hidden comments via AJAX ***********************/
/*****************************************************************************/

void TL_ShowHiddenCommentsUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show hidden comments *****/
   TL_ShowHiddenCommentsGbl ();
  }

void TL_ShowHiddenCommentsGbl (void)
  {
   long NotCod;
   char IdComments[Frm_MAX_BYTES_ID + 1];
   unsigned NumInitialCommentsToGet;
   unsigned NumInitialCommentsGot;

   /***** Get parameters *****/
   /* Get note code */
   NotCod = TL_GetParamNotCod ();

   /* Get identifier */
   Par_GetParToText ("IdComments",IdComments,Frm_MAX_BYTES_ID);

   /* Get number of comments to get */
   NumInitialCommentsToGet = (unsigned) Par_GetParToLong ("NumHidCom");

   /***** Write HTML inside DIV with hidden comments *****/
   NumInitialCommentsGot = TL_WriteHiddenComments (NotCod,IdComments,NumInitialCommentsToGet);

   /***** Link to show the first comments *****/
   TL_LinkToShowPreviousComments (IdComments,NumInitialCommentsGot);
  }

/*****************************************************************************/
/**************************** Write hidden comments **************************/
/*****************************************************************************/
// Returns the number of comments got

static unsigned TL_WriteHiddenComments (long NotCod,
				        char IdComments[Frm_MAX_BYTES_ID + 1],
					unsigned NumInitialCommentsToGet)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumInitialCommentsGot;
   unsigned long NumCom;

   /***** Get comments of this note from database *****/
   NumInitialCommentsGot = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get comments",
		   "SELECT tl_pubs.PubCod,"		// row[0]
			  "tl_pubs.PublisherCod,"	// row[1]
			  "tl_pubs.NotCod,"		// row[2]
			  "UNIX_TIMESTAMP("
			  "tl_pubs.TimePublish),"	// row[3]
			  "tl_comments.Txt,"		// row[4]
			  "tl_comments.MedCod"		// row[5]
		   " FROM tl_pubs,tl_comments"
		   " WHERE tl_pubs.NotCod=%ld"
		   " AND tl_pubs.PubType=%u"
		   " AND tl_pubs.PubCod=tl_comments.PubCod"
		   " ORDER BY tl_pubs.PubCod"
		   " LIMIT %lu",
		   NotCod,(unsigned) TL_PUB_COMMENT_TO_NOTE,
		   NumInitialCommentsToGet);

   /***** List with comments *****/
   HTM_UL_Begin ("id=\"com_%s\" class=\"TL_LIST\"",IdComments);
   for (NumCom = 0;
	NumCom < NumInitialCommentsGot;
	NumCom++)
      TL_WriteOneCommentInList (mysql_res);
   HTM_UL_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumInitialCommentsGot;
  }

/*****************************************************************************/
/************************* Write a comment in list ***************************/
/*****************************************************************************/

static void TL_WriteOneCommentInList (MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   struct TL_Comment SocCom;

   /***** Initialize image *****/
   Med_MediaConstructor (&SocCom.Content.Media);

   /***** Get data of comment *****/
   row = mysql_fetch_row (mysql_res);
   TL_GetDataOfCommentFromRow (row,&SocCom);

   /***** Write comment *****/
   TL_WriteComment (&SocCom,
		    TL_TOP_MESSAGE_NONE,-1L,
		    false);	// Not alone

   /***** Free image *****/
   Med_MediaDestructor (&SocCom.Content.Media);
  }

/*****************************************************************************/
/****************** Link to show only the latest comments ********************/
/*****************************************************************************/

static void TL_LinkToShowOnlyLatestComments (const char IdComments[Frm_MAX_BYTES_ID + 1])
  {
   extern const char *Txt_See_only_the_latest_COMMENTS;

   /***** Icon and text to show only the latest comments ****/
   HTM_DIV_Begin ("id=\"con_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\""
		  " style=\"display:none;\"",	// Hidden
		  IdComments);
   TL_PutIconToToggleComments (IdComments,"angle-down.svg",
			       Txt_See_only_the_latest_COMMENTS);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Link to show the first comments ***********************/
/*****************************************************************************/

static void TL_LinkToShowPreviousComments (const char IdComments[Frm_MAX_BYTES_ID + 1],
				           unsigned NumInitialComments)
  {
   extern const char *Txt_See_the_previous_X_COMMENTS;

   /***** Build text to show *****/
   snprintf (Gbl.Title,sizeof (Gbl.Title),
	     Txt_See_the_previous_X_COMMENTS,
	     NumInitialComments);

   /***** Icon and text to show only the latest comments ****/
   HTM_DIV_Begin ("id=\"exp_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\""
	          " style=\"display:none;\"",	// Hidden
		  IdComments);
   TL_PutIconToToggleComments (IdComments,"angle-up.svg",Gbl.Title);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off comments in a publication ***********/
/*****************************************************************************/

static void TL_PutIconToToggleComments (const char *UniqueId,
                                        const char *Icon,const char *Text)
  {
   extern const char *The_ClassFormLinkInBox[The_NUM_THEMES];
   char *OnClick;

   if (asprintf (&OnClick,"toggleComments('%s')",UniqueId) < 0)
      Lay_NotEnoughMemoryExit ();

   /***** Link to toggle on/off some divs *****/
   HTM_BUTTON_BUTTON_Begin (Text,The_ClassFormLinkInBox[Gbl.Prefs.Theme],OnClick);
   Ico_PutIconTextLink (Icon,Text);
   HTM_BUTTON_End ();

   free (OnClick);
  }

/*****************************************************************************/
/******************************** Write comment ******************************/
/*****************************************************************************/

static void TL_WriteComment (struct TL_Comment *SocCom,
                             TL_TopMessage_t TopMessage,long UsrCod,
                             bool ShowCommentAlone)	// Comment is shown alone, not in a list
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData UsrDat;
   bool IAmTheAuthor;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];
   static unsigned NumDiv = 0;	// Used to create unique div id for fav

   NumDiv++;

   if (ShowCommentAlone)
     {
      Box_BoxBegin (NULL,NULL,NULL,
                    NULL,Box_NOT_CLOSABLE);

      /***** Write sharer/commenter if distinct to author *****/
      TL_WriteTopMessage (TopMessage,UsrCod);

      HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");
      HTM_UL_Begin ("class=\"LIST_LEFT\"");
     }

   /***** Start list item *****/
   if (ShowCommentAlone)
      HTM_LI_Begin (NULL);
   else
      HTM_LI_Begin ("class=\"TL_COM\"");

   if (SocCom->PubCod <= 0 ||
       SocCom->NotCod <= 0 ||
       SocCom->UsrCod <= 0)
      Ale_ShowAlert (Ale_ERROR,"Error in comment.");
   else
     {
      /***** Get author's data *****/
      Usr_UsrDataConstructor (&UsrDat);
      UsrDat.UsrCod = SocCom->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
      IAmTheAuthor = Usr_ItsMe (UsrDat.UsrCod);

      /***** Left: write author's photo *****/
      HTM_DIV_Begin ("class=\"TL_COM_PHOTO\"");
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
					    NULL,
			"PHOTO30x40",Pho_ZOOM,true);	// Use unique id
      HTM_DIV_End ();

      /***** Right: author's name, time, content, image and buttons *****/
      HTM_DIV_Begin ("class=\"TL_COM_CONT TL_COMM_WIDTH\"");

      /* Write author's full name and nickname */
      TL_WriteAuthorComment (&UsrDat);

      /* Write date and time */
      TL_WriteDateTime (SocCom->DateTimeUTC);

      /* Write content of the comment */
      if (SocCom->Content.Txt[0])
	{
	 HTM_DIV_Begin ("class=\"TL_TXT\"");
	 Msg_WriteMsgContent (SocCom->Content.Txt,Cns_MAX_BYTES_LONG_TEXT,true,false);
	 HTM_DIV_End ();
	}

      /* Show image */
      Med_ShowMedia (&SocCom->Content.Media,"TL_COM_MED_CONT TL_COMM_WIDTH",
	                                    "TL_COM_MED TL_COMM_WIDTH");

      /* Start foot container */
      HTM_DIV_Begin ("class=\"TL_FOOT TL_COMM_WIDTH\"");

      /* Fav zone */
      HTM_DIV_Begin ("id=\"fav_com_%s_%u\" class=\"TL_FAV_COM TL_FAV_WIDTH\"",
	             Gbl.UniqueNameEncrypted,NumDiv);
      TL_PutFormToFavUnfComment (SocCom,TL_SHOW_A_FEW_USRS);
      HTM_DIV_End ();

      /* Put icon to remove this comment */
      HTM_DIV_Begin ("class=\"TL_REM\"");
      if (IAmTheAuthor && !ShowCommentAlone)
	 TL_PutFormToRemoveComment (SocCom->PubCod);
      HTM_DIV_End ();

      /* End foot container */
      HTM_DIV_End ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** End list item *****/
   HTM_LI_End ();

   if (ShowCommentAlone)
     {
      HTM_UL_End ();
      HTM_DIV_End ();
      Box_BoxEnd ();
     }
  }

/*****************************************************************************/
/********* Write name and nickname of author of a comment to a note **********/
/*****************************************************************************/

static void TL_WriteAuthorComment (struct UsrData *UsrDat)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   /***** Show user's name inside form to go to user's public profile *****/
   Frm_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   HTM_BUTTON_SUBMIT_Begin (ItsMe ? Txt_My_public_profile :
			            Txt_Another_user_s_profile,
	                    "BT_LINK TL_COM_AUTHOR TL_COMM_AUTHOR_WIDTH DAT_BOLD",NULL);
   HTM_Txt (UsrDat->FullName);
   HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************* Form to remove comment ****************************/
/*****************************************************************************/

static void TL_PutFormToRemoveComment (long PubCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   TL_FormStart (ActReqRemSocComGbl,ActReqRemSocComUsr);
   TL_PutHiddenParamPubCod (PubCod);
   Ico_PutIconLink ("trash.svg",Txt_Remove);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********************** Put disabled icon to share **************************/
/*****************************************************************************/

static void TL_PutDisabledIconShare (unsigned NumShared)
  {
   extern const char *Txt_TIMELINE_NOTE_Shared_by_X_USERS;
   extern const char *Txt_TIMELINE_NOTE_Not_shared_by_anyone;

   if (NumShared)
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Txt_TIMELINE_NOTE_Shared_by_X_USERS,
		NumShared);
   else
      Str_Copy (Gbl.Title,Txt_TIMELINE_NOTE_Not_shared_by_anyone,
                Lay_MAX_BYTES_TITLE);

   /***** Disabled icon to share *****/
   Ico_PutDivIcon ("TL_ICO_DISABLED",
		   TL_ICON_SHARE,Gbl.Title);
  }

/*****************************************************************************/
/****************** Put disabled icon to mark as favourite *******************/
/*****************************************************************************/

static void TL_PutDisabledIconFav (unsigned NumFavs)
  {
   extern const char *Txt_TIMELINE_NOTE_Favourited_by_X_USERS;
   extern const char *Txt_TIMELINE_NOTE_Not_favourited_by_anyone;

   if (NumFavs)
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        Txt_TIMELINE_NOTE_Favourited_by_X_USERS,
		NumFavs);
   else
      Str_Copy (Gbl.Title,Txt_TIMELINE_NOTE_Not_favourited_by_anyone,
                Lay_MAX_BYTES_TITLE);

   /***** Disabled icon to mark as favourite *****/
   Ico_PutDivIcon ("TL_ICO_DISABLED",
		   TL_ICON_FAV,Gbl.Title);
  }

/*****************************************************************************/
/*********************** Form to share/unshare note **************************/
/*****************************************************************************/

static void TL_PutFormToSeeAllSharersNote (const struct TL_Note *SocNot,
                                           TL_HowMany_t HowMany)
  {
   extern const char *Txt_View_all_USERS;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   switch (HowMany)
     {
      case TL_SHOW_A_FEW_USRS:
	 /***** Form and icon to mark note as favourite *****/
	 sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
	 TL_FormFavSha (ActAllShaSocNotGbl,ActAllShaSocNotUsr,ParamCod,
			TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
      case TL_SHOW_ALL_USRS:
	 Ico_PutIconOff (TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
     }
  }

static void TL_PutFormToShaNote (const struct TL_Note *SocNot)
  {
   extern const char *Txt_Share;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to mark note as favourite *****/
   sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
   TL_FormFavSha (ActShaSocNotGbl,ActShaSocNotUsr,ParamCod,
	          TL_ICON_SHARE,Txt_Share);
  }

static void TL_PutFormToUnsNote (const struct TL_Note *SocNot)
  {
   extern const char *Txt_TIMELINE_NOTE_Shared;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to mark note as favourite *****/
   sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
   TL_FormFavSha (ActUnsSocNotGbl,ActUnsSocNotUsr,ParamCod,
	          TL_ICON_SHARED,Txt_TIMELINE_NOTE_Shared);
  }

/*****************************************************************************/
/************************** Form to fav/unfav note ***************************/
/*****************************************************************************/

static void TL_PutFormToSeeAllFaversNote (const struct TL_Note *SocNot,
                                          TL_HowMany_t HowMany)
  {
   extern const char *Txt_View_all_USERS;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   switch (HowMany)
     {
      case TL_SHOW_A_FEW_USRS:
	 /***** Form and icon to mark note as favourite *****/
	 sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
	 TL_FormFavSha (ActAllFavSocNotGbl,ActAllFavSocNotUsr,ParamCod,
			TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
      case TL_SHOW_ALL_USRS:
         Ico_PutIconOff (TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
     }
  }

static void TL_PutFormToFavNote (const struct TL_Note *SocNot)
  {
   extern const char *Txt_Mark_as_favourite;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to mark note as favourite *****/
   sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
   TL_FormFavSha (ActFavSocNotGbl,ActFavSocNotUsr,ParamCod,
	          TL_ICON_FAV,Txt_Mark_as_favourite);
  }

static void TL_PutFormToUnfNote (const struct TL_Note *SocNot)
  {
   extern const char *Txt_TIMELINE_NOTE_Favourite;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to unfav (remove mark as favourite) note *****/
   sprintf (ParamCod,"NotCod=%ld",SocNot->NotCod);
   TL_FormFavSha (ActUnfSocNotGbl,ActUnfSocNotUsr,ParamCod,
	          TL_ICON_FAVED,Txt_TIMELINE_NOTE_Favourite);
  }

/*****************************************************************************/
/************************** Form to fav/unfav comment ************************/
/*****************************************************************************/

static void TL_PutFormToSeeAllFaversComment (const struct TL_Comment *SocCom,
                                             TL_HowMany_t HowMany)
  {
   extern const char *Txt_View_all_USERS;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   switch (HowMany)
     {
      case TL_SHOW_A_FEW_USRS:
	 /***** Form and icon to mark comment as favourite *****/
	 sprintf (ParamCod,"PubCod=%ld",SocCom->PubCod);
	 TL_FormFavSha (ActAllFavSocComGbl,ActAllFavSocComUsr,ParamCod,
			TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
      case TL_SHOW_ALL_USRS:
         Ico_PutIconOff (TL_ICON_ELLIPSIS,Txt_View_all_USERS);
         break;
     }
  }

static void TL_PutFormToFavComment (const struct TL_Comment *SocCom)
  {
   extern const char *Txt_Mark_as_favourite;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to mark comment as favourite *****/
   sprintf (ParamCod,"PubCod=%ld",SocCom->PubCod);
   TL_FormFavSha (ActFavSocComGbl,ActFavSocComUsr,ParamCod,
	          TL_ICON_FAV,Txt_Mark_as_favourite);
  }

static void TL_PutFormToUnfComment (const struct TL_Comment *SocCom)
  {
   extern const char *Txt_TIMELINE_NOTE_Favourite;
   char ParamCod[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Form and icon to unfav (remove mark as favourite) comment *****/
   sprintf (ParamCod,"PubCod=%ld",SocCom->PubCod);
   TL_FormFavSha (ActUnfSocComGbl,ActUnfSocComUsr,ParamCod,
	          TL_ICON_FAVED,Txt_TIMELINE_NOTE_Favourite);
  }

/*****************************************************************************/
/************************ Form to remove publication *************************/
/*****************************************************************************/

static void TL_PutFormToRemovePublication (long NotCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   TL_FormStart (ActReqRemSocPubGbl,ActReqRemSocPubUsr);
   TL_PutHiddenParamNotCod (NotCod);
   Ico_PutIconLink ("trash.svg",Txt_Remove);
   Frm_EndForm ();
  }

/*****************************************************************************/
/****************** Put parameter with the code of a note ********************/
/*****************************************************************************/

static void TL_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong (NULL,"NotCod",NotCod);
  }

/*****************************************************************************/
/*************** Put parameter with the code of a publication ****************/
/*****************************************************************************/

void TL_PutHiddenParamPubCod (long PubCod)
  {
   Par_PutHiddenParamLong (NULL,"PubCod",PubCod);
  }

/*****************************************************************************/
/****************** Get parameter with the code of a note ********************/
/*****************************************************************************/

static long TL_GetParamNotCod (void)
  {
   /***** Get note code *****/
   return Par_GetParToLong ("NotCod");
  }

/*****************************************************************************/
/**************** Get parameter with the code of a publication ***************/
/*****************************************************************************/

static long TL_GetParamPubCod (void)
  {
   /***** Get comment code *****/
   return Par_GetParToLong ("PubCod");
  }

/*****************************************************************************/
/******************************* Comment a note ******************************/
/*****************************************************************************/

void TL_ReceiveCommentUsr (void)
  {
   long NotCod;

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Receive comment in a note
          and write updated timeline after commenting (user) *****/
   NotCod = TL_ReceiveComment ();
   TL_ShowTimelineUsrHighlightingNot (NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_ReceiveCommentGbl (void)
  {
   long NotCod;

   /***** Receive comment in a note *****/
   NotCod = TL_ReceiveComment ();

   /***** Write updated timeline after commenting (global) *****/
   TL_ShowTimelineGblHighlightingNot (NotCod);
  }

static long TL_ReceiveComment (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   struct PostContent Content;
   struct TL_Note SocNot;
   struct TL_Publication SocPub;

   /***** Get data of note *****/
   SocNot.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      /***** Get the content of the comment *****/
      Par_GetParAndChangeFormat ("Txt",Content.Txt,Cns_MAX_BYTES_LONG_TEXT,
				 Str_TO_RIGOROUS_HTML,true);

      /***** Initialize image *****/
      Med_MediaConstructor (&Content.Media);

      /***** Get attached image (action, file and title) *****/
      Content.Media.Width   = TL_IMAGE_SAVED_MAX_WIDTH;
      Content.Media.Height  = TL_IMAGE_SAVED_MAX_HEIGHT;
      Content.Media.Quality = TL_IMAGE_SAVED_QUALITY;
      Med_GetMediaFromForm (-1,&Content.Media,NULL,NULL);
      Ale_ShowAlerts (NULL);

      if (Content.Txt[0] ||			// Text not empty
	 Content.Media.Status == Med_PROCESSED)	// A media is attached
	{
	 /***** Store media in filesystem and database *****/
	 Med_RemoveKeepOrStoreMedia (-1L,&Content.Media);

	 /***** Publish *****/
	 /* Insert into publications */
	 SocPub.NotCod       = SocNot.NotCod;
	 SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 SocPub.PubType      = TL_PUB_COMMENT_TO_NOTE;
	 TL_PublishNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	 /* Insert comment content in the database */
	 DB_QueryINSERT ("can not store comment content",
			 "INSERT INTO tl_comments"
	                 " (PubCod,Txt,MedCod)"
			 " VALUES"
			 " (%ld,'%s',%ld)",
			 SocPub.PubCod,
			 Content.Txt,
			 Content.Media.MedCod);

	 /***** Store notifications about the new comment *****/
	 Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TIMELINE_COMMENT,SocPub.PubCod);

	 /***** Analyze content and store notifications about mentions *****/
	 Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (SocPub.PubCod,Content.Txt);
	}

      /***** Free image *****/
      Med_MediaDestructor (&Content.Media);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

   return SocNot.NotCod;
  }

/*****************************************************************************/
/******************************** Share a note *******************************/
/*****************************************************************************/

void TL_ShowAllSharersNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all sharers *****/
   TL_ShowAllSharersNoteGbl ();
  }

void TL_ShowAllSharersNoteGbl (void)
  {
   struct TL_Note SocNot;

   /***** Get data of note *****/
   SocNot.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&SocNot);

   /***** Write HTML inside DIV with form to share/unshare *****/
   TL_PutFormToShaUnsNote (&SocNot,TL_SHOW_ALL_USRS);
  }

void TL_ShaNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Share note *****/
   TL_ShaNoteGbl ();
  }

void TL_ShaNoteGbl (void)
  {
   struct TL_Note SocNot;

   /***** Share note *****/
   TL_ShaNote (&SocNot);

   /***** Write HTML inside DIV with form to unshare *****/
   TL_PutFormToShaUnsNote (&SocNot,TL_SHOW_A_FEW_USRS);
  }

static void TL_PutFormToShaUnsNote (const struct TL_Note *SocNot,
                                    TL_HowMany_t HowMany)
  {
   bool IAmTheAuthor;
   bool IAmASharerOfThisSocNot;

   /***** Put form to share/unshare this note *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   IAmTheAuthor = Usr_ItsMe (SocNot->UsrCod);
   if (SocNot->Unavailable ||		// Unavailable notes can not be shared
       IAmTheAuthor)			// I am the author
      /* Put disabled icon */
      TL_PutDisabledIconShare (SocNot->NumShared);
   else					// Available and I am not the author
     {
      /* Put icon to share/unshare */
      IAmASharerOfThisSocNot = TL_CheckIfNoteIsSharedByUsr (SocNot->NotCod,
							    Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmASharerOfThisSocNot)	// I have shared this note
	 TL_PutFormToUnsNote (SocNot);
      else				// I have not shared this note
	 TL_PutFormToShaNote (SocNot);
     }
   HTM_DIV_End ();

   /***** Show who have shared this note *****/
   TL_ShowUsrsWhoHaveSharedNote (SocNot,HowMany);
  }

static void TL_ShaNote (struct TL_Note *SocNot)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   struct TL_Publication SocPub;
   bool ItsMe;
   long OriginalPubCod;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
         if (!TL_CheckIfNoteIsSharedByUsr (SocNot->NotCod,
					    Gbl.Usrs.Me.UsrDat.UsrCod))	// Not yet shared by me
	   {
	    /***** Share (publish note in timeline) *****/
	    SocPub.NotCod       = SocNot->NotCod;
	    SocPub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	    SocPub.PubType      = TL_PUB_SHARED_NOTE;
	    TL_PublishNoteInTimeline (&SocPub);	// Set SocPub.PubCod

	    /* Update number of times this note is shared */
	    TL_UpdateNumTimesANoteHasBeenShared (SocNot);

	    /**** Create notification about shared post
		  for the author of the post ***/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       TL_CreateNotifToAuthor (SocNot->UsrCod,OriginalPubCod,Ntf_EVENT_TIMELINE_SHARE);
	   }
     }
  }

/*****************************************************************************/
/********************** Mark/unmark a note as favourite **********************/
/*****************************************************************************/

void TL_ShowAllFaversNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   TL_ShowAllFaversNoteGbl ();
  }

void TL_ShowAllFaversNoteGbl (void)
  {
   struct TL_Note SocNot;

   /***** Get data of note *****/
   SocNot.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&SocNot);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TL_PutFormToFavUnfNote (&SocNot,TL_SHOW_ALL_USRS);
  }

void TL_FavNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark note as favourite *****/
   TL_FavNoteGbl ();
  }

void TL_FavNoteGbl (void)
  {
   struct TL_Note SocNot;

   /***** Mark note as favourite *****/
   TL_FavNote (&SocNot);

   /***** Write HTML inside DIV with form to unfav *****/
   TL_PutFormToFavUnfNote (&SocNot,TL_SHOW_A_FEW_USRS);
  }

void TL_UnfNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a note previously marked as favourite *****/
   TL_UnfNoteGbl ();
  }

void TL_UnfNoteGbl (void)
  {
   struct TL_Note SocNot;

   /***** Stop marking as favourite a previously favourited note *****/
   TL_UnfNote (&SocNot);

   /***** Write HTML inside DIV with form to fav *****/
   TL_PutFormToFavUnfNote (&SocNot,TL_SHOW_A_FEW_USRS);
  }

static void TL_PutFormToFavUnfNote (const struct TL_Note *SocNot,
                                    TL_HowMany_t HowMany)
  {
   bool IAmTheAuthor;
   bool IAmAFaverOfThisSocNot;

   /***** Put form to fav/unfav this note *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   IAmTheAuthor = Usr_ItsMe (SocNot->UsrCod);
   if (SocNot->Unavailable ||		// Unavailable notes can not be favourited
       IAmTheAuthor)			// I am the author
      /* Put disabled icon */
      TL_PutDisabledIconFav (SocNot->NumFavs);
   else					// Available and I am not the author
     {
      /* Put icon to fav/unfav */
      IAmAFaverOfThisSocNot = TL_CheckIfNoteIsFavedByUsr (SocNot->NotCod,
							  Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmAFaverOfThisSocNot)	// I have favourited this note
	 TL_PutFormToUnfNote (SocNot);
      else				// I am not a faver of this note
	 TL_PutFormToFavNote (SocNot);
     }
   HTM_DIV_End ();

   /***** Show who have marked this note as favourite *****/
   TL_ShowUsrsWhoHaveMarkedNoteAsFav (SocNot,HowMany);
  }

static void TL_FavNote (struct TL_Note *SocNot)
  {
   bool ItsMe;
   long OriginalPubCod;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (!TL_CheckIfNoteIsFavedByUsr (SocNot->NotCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod))	// I have not yet favourited the note
	   {
	    /***** Mark as favourite in database *****/
	    DB_QueryINSERT ("can not favourite note",
			    "INSERT IGNORE INTO tl_notes_fav"
			    " (NotCod,UsrCod,TimeFav)"
			    " VALUES"
			    " (%ld,%ld,NOW())",
			    SocNot->NotCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /***** Update number of times this note is favourited *****/
	    TL_GetNumTimesANoteHasBeenFav (SocNot);

	    /***** Create notification about favourite post
		   for the author of the post *****/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       TL_CreateNotifToAuthor (SocNot->UsrCod,OriginalPubCod,Ntf_EVENT_TIMELINE_FAV);
	   }
     }
  }

static void TL_UnfNote (struct TL_Note *SocNot)
  {
   long OriginalPubCod;
   bool ItsMe;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (SocNot->NumFavs &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (TL_CheckIfNoteIsFavedByUsr (SocNot->NotCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the note
	   {
	    /***** Delete the mark as favourite from database *****/
	    DB_QueryDELETE ("can not unfavourite note",
			    "DELETE FROM tl_notes_fav"
			    " WHERE NotCod=%ld AND UsrCod=%ld",
			    SocNot->NotCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /***** Update number of times this note is favourited *****/
	    TL_GetNumTimesANoteHasBeenFav (SocNot);

            /***** Mark possible notifications on this note as removed *****/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,OriginalPubCod);
	   }
     }
  }

/*****************************************************************************/
/********************* Mark/unmark a comment as favourite ************************/
/*****************************************************************************/

void TL_ShowAllFaversComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show all favers *****/
   TL_ShowAllFaversComGbl ();
  }

void TL_ShowAllFaversComGbl (void)
  {
   struct TL_Comment SocCom;

   /***** Get data of comment *****/
   Med_MediaConstructor (&SocCom.Content.Media);
   SocCom.PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (&SocCom);
   Med_MediaDestructor (&SocCom.Content.Media);

   /***** Write HTML inside DIV with form to fav/unfav *****/
   TL_PutFormToFavUnfComment (&SocCom,TL_SHOW_ALL_USRS);
  }

void TL_FavCommentUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Mark comment as favourite *****/
   TL_FavCommentGbl ();
  }

void TL_FavCommentGbl (void)
  {
   struct TL_Comment SocCom;

   /***** Mark comment as favourite *****/
   TL_FavComment (&SocCom);

   /***** Write HTML inside DIV with form to unfav *****/
   TL_PutFormToFavUnfComment (&SocCom,TL_SHOW_A_FEW_USRS);
  }

void TL_UnfCommentUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unfav a comment previously marked as favourite *****/
   TL_UnfCommentGbl ();
  }

void TL_UnfCommentGbl (void)
  {
   struct TL_Comment SocCom;

   /***** Stop marking as favourite a previously favourited comment *****/
   TL_UnfComment (&SocCom);

   /***** Write HTML inside DIV with form to fav *****/
   TL_PutFormToFavUnfComment (&SocCom,TL_SHOW_A_FEW_USRS);
  }

static void TL_PutFormToFavUnfComment (const struct TL_Comment *SocCom,
                                       TL_HowMany_t HowMany)
  {
   bool IAmTheAuthor;
   bool IAmAFaverOfThisSocCom;

   /***** Put form to fav/unfav this comment *****/
   HTM_DIV_Begin ("class=\"TL_ICO\"");
   IAmTheAuthor = Usr_ItsMe (SocCom->UsrCod);
   if (IAmTheAuthor)			// I am the author
      /* Put disabled icon */
      TL_PutDisabledIconFav (SocCom->NumFavs);
   else				// I am not the author
     {
      /* Put icon to mark this comment as favourite */
      IAmAFaverOfThisSocCom = TL_CheckIfCommIsFavedByUsr (SocCom->PubCod,
							  Gbl.Usrs.Me.UsrDat.UsrCod);
      if (IAmAFaverOfThisSocCom)	// I have favourited this comment
	 /* Put icon to unfav this publication and list of users */
	 TL_PutFormToUnfComment (SocCom);
      else				// I am not a favouriter
	 /* Put icon to fav this publication and list of users */
	 TL_PutFormToFavComment (SocCom);
     }
   HTM_DIV_End ();

   /***** Show who have marked this comment as favourite *****/
   TL_ShowUsrsWhoHaveMarkedCommAsFav (SocCom,HowMany);
  }

static void TL_FavComment (struct TL_Comment *SocCom)
  {
   bool IAmTheAuthor;

   /***** Initialize image *****/
   Med_MediaConstructor (&SocCom->Content.Media);

   /***** Get data of comment *****/
   SocCom->PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (SocCom);

   if (SocCom->PubCod > 0)
     {
      IAmTheAuthor = Usr_ItsMe (SocCom->UsrCod);
      if (!IAmTheAuthor)	// I am not the author
	 if (!TL_CheckIfCommIsFavedByUsr (SocCom->PubCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod)) // I have not yet favourited the comment
	   {
	    /***** Mark as favourite in database *****/
	    DB_QueryINSERT ("can not favourite comment",
			    "INSERT IGNORE INTO tl_comments_fav"
			    " (PubCod,UsrCod,TimeFav)"
			    " VALUES"
			    " (%ld,%ld,NOW())",
			    SocCom->PubCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /* Update number of times this comment is favourited */
	    TL_GetNumTimesACommHasBeenFav (SocCom);

	    /**** Create notification about favourite post
		  for the author of the post ***/
	    TL_CreateNotifToAuthor (SocCom->UsrCod,SocCom->PubCod,Ntf_EVENT_TIMELINE_FAV);
	   }
     }

   /***** Free image *****/
   Med_MediaDestructor (&SocCom->Content.Media);
  }

static void TL_UnfComment (struct TL_Comment *SocCom)
  {
   bool IAmTheAuthor;

   /***** Initialize image *****/
   Med_MediaConstructor (&SocCom->Content.Media);

   /***** Get data of comment *****/
   SocCom->PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (SocCom);

   if (SocCom->PubCod > 0)
     {
      IAmTheAuthor = Usr_ItsMe (SocCom->UsrCod);
      if (SocCom->NumFavs &&
	  !IAmTheAuthor)	// I am not the author
	 if (TL_CheckIfCommIsFavedByUsr (SocCom->PubCod,
					  Gbl.Usrs.Me.UsrDat.UsrCod))	// I have favourited the comment
	   {
	    /***** Delete the mark as favourite from database *****/
	    DB_QueryDELETE ("can not unfavourite comment",
			    "DELETE FROM tl_comments_fav"
			    " WHERE PubCod=%ld AND UsrCod=%ld",
			    SocCom->PubCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod);

	    /***** Update number of times this comment is favourited *****/
	    TL_GetNumTimesACommHasBeenFav (SocCom);

            /***** Mark possible notifications on this comment as removed *****/
            Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV,SocCom->PubCod);
	   }
     }

   /***** Free image *****/
   Med_MediaDestructor (&SocCom->Content.Media);
  }

/*****************************************************************************/
/*********** Create a notification for the author of a post/comment **********/
/*****************************************************************************/

static void TL_CreateNotifToAuthor (long AuthorCod,long PubCod,
                                    Ntf_NotifyEvent_t NotifyEvent)
  {
   struct UsrData UsrDat;
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   UsrDat.UsrCod = AuthorCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
     {
      /***** This fav must be notified by email? *****/
      CreateNotif = (UsrDat.NtfEvents.CreateNotif & (1 << NotifyEvent));
      NotifyByEmail = CreateNotif &&
		      (UsrDat.NtfEvents.SendEmail & (1 << NotifyEvent));

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
/******************** Unshare a previously shared note ***********************/
/*****************************************************************************/

void TL_UnsNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Unshare note *****/
   TL_UnsNoteGbl ();
  }

void TL_UnsNoteGbl (void)
  {
   struct TL_Note SocNot;

   /***** Unshare note *****/
   TL_UnsNote (&SocNot);

   /***** Write HTML inside DIV with form to share *****/
   TL_PutFormToShaUnsNote (&SocNot,TL_SHOW_A_FEW_USRS);
  }

static void TL_UnsNote (struct TL_Note *SocNot)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   long OriginalPubCod;
   bool ItsMe;

   /***** Get data of note *****/
   SocNot->NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (SocNot);

   if (SocNot->NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot->UsrCod);
      if (SocNot->NumShared &&
	  Gbl.Usrs.Me.Logged && !ItsMe)	// I am not the author
	 if (TL_CheckIfNoteIsSharedByUsr (SocNot->NotCod,
					   Gbl.Usrs.Me.UsrDat.UsrCod))	// I am a sharer
	   {
	    /***** Delete publication from database *****/
	    DB_QueryDELETE ("can not remove a publication",
			    "DELETE FROM tl_pubs"
	                    " WHERE NotCod=%ld"
	                    " AND PublisherCod=%ld"
	                    " AND PubType=%u",
			    SocNot->NotCod,
			    Gbl.Usrs.Me.UsrDat.UsrCod,
			    (unsigned) TL_PUB_SHARED_NOTE);

	    /***** Update number of times this note is shared *****/
	    TL_UpdateNumTimesANoteHasBeenShared (SocNot);

            /***** Mark possible notifications on this note as removed *****/
	    OriginalPubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
	    if (OriginalPubCod > 0)
	       Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE,OriginalPubCod);
	   }
     }
  }

/*****************************************************************************/
/*********************** Request the removal of a note ***********************/
/*****************************************************************************/

void TL_RequestRemNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Request the removal of note *****/
   TL_RequestRemovalNote ();

   /***** Write timeline again (user) *****/
   TL_ShowTimelineUsr ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RequestRemNoteGbl (void)
  {
   /***** Request the removal of note *****/
   TL_RequestRemovalNote ();

   /***** Write timeline again (global) *****/
   TL_ShowTimelineGbl2 ();
  }

static void TL_RequestRemovalNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   extern const char *Txt_Remove;
   struct TL_Note SocNot;
   bool ItsMe;

   /***** Get data of note *****/
   SocNot.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (ItsMe)	// I am the author of this note
	{
	 /***** Show question and button to remove note *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_post);

	 /* Show note */
	 TL_WriteNote (&SocNot,
		       TL_TOP_MESSAGE_NONE,-1L,
		       false,true);

	 /* End alert */
	 Gbl.Timeline.NotCod = SocNot.NotCod;	// Note to be removed
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	    Ale_ShowAlertAndButton2 (ActRemSocPubUsr,"timeline",NULL,
	                             TL_PutParamsRemoveNote,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlertAndButton2 (ActRemSocPubGbl,NULL,NULL,
	                             TL_PutParamsRemoveNote,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/********************* Put parameters to remove a note ***********************/
/*****************************************************************************/

static void TL_PutParamsRemoveNote (void)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted ();
   else
      Usr_PutHiddenParamWho (Gbl.Timeline.Who);
   TL_PutHiddenParamNotCod (Gbl.Timeline.NotCod);
  }

/*****************************************************************************/
/******************************* Remove a note *******************************/
/*****************************************************************************/

void TL_RemoveNoteUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Remove a note *****/
   TL_RemoveNote ();

   /***** Write updated timeline after removing (user) *****/
   TL_ShowTimelineUsr ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RemoveNoteGbl (void)
  {
   /***** Remove a note *****/
   TL_RemoveNote ();

   /***** Write updated timeline after removing (global) *****/
   TL_ShowTimelineGbl2 ();
  }

static void TL_RemoveNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_TIMELINE_Post_removed;
   struct TL_Note SocNot;
   bool ItsMe;

   /***** Get data of note *****/
   SocNot.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&SocNot);

   if (SocNot.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (SocNot.UsrCod);
      if (ItsMe)	// I am the author of this note
	{
	 /***** Delete note from database *****/
	 TL_RemoveNoteMediaAndDBEntries (&SocNot);

	 /***** Reset note *****/
	 TL_ResetNote (&SocNot);

	 /***** Message of success *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_TIMELINE_Post_removed);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/*********************** Remove a note from database *************************/
/*****************************************************************************/

static void TL_RemoveNoteMediaAndDBEntries (struct TL_Note *SocNot)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;
   unsigned long NumComments;
   unsigned long NumCom;
   long MedCod;

   /***** Remove comments associated to this note *****/
   /* Get comments of this note */
   NumComments = DB_QuerySELECT (&mysql_res,"can not get comments",
				 "SELECT PubCod"
				 " FROM tl_pubs"
				 " WHERE NotCod=%ld AND PubType=%u",
				 SocNot->NotCod,
				 (unsigned) TL_PUB_COMMENT_TO_NOTE);

   /* For each comment... */
   for (NumCom = 0;
	NumCom < NumComments;
	NumCom++)
     {
      /* Get code of comment **/
      row = mysql_fetch_row (mysql_res);
      PubCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Remove media associated to comment
	 and delete comment from database */
      TL_RemoveCommentMediaAndDBEntries (PubCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove media associated to post *****/
   if (SocNot->NoteType == TL_NOTE_POST)
     {
      /* Remove media associated to a post from database */
      if (DB_QuerySELECT (&mysql_res,"can not get media",
				 "SELECT MedCod"	// row[0]
				 " FROM tl_posts"
				 " WHERE PstCod=%ld",
				 SocNot->Cod) == 1)   // Result should have a unique row
        {
	 /* Get media code */
	 row = mysql_fetch_row (mysql_res);
	 MedCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Remove media */
	 Med_RemoveMedia (MedCod);
        }

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** Mark possible notifications on the publications
          of this note as removed *****/
   /* Mark notifications of the original note as removed */
   PubCod = TL_GetPubCodOfOriginalNote (SocNot->NotCod);
   if (PubCod > 0)
     {
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV    ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE  ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_MENTION,PubCod);
     }

   /***** Remove favs for this note *****/
   DB_QueryDELETE ("can not remove favs for note",
		   "DELETE FROM tl_notes_fav"
		   " WHERE NotCod=%ld",
		   SocNot->NotCod);

   /***** Remove all the publications of this note *****/
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tl_pubs"
		   " WHERE NotCod=%ld",
		   SocNot->NotCod);

   /***** Remove note *****/
   DB_QueryDELETE ("can not remove a note",
		   "DELETE FROM tl_notes"
	           " WHERE NotCod=%ld"
	           " AND UsrCod=%ld",		// Extra check: I am the author
		   SocNot->NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);

   if (SocNot->NoteType == TL_NOTE_POST)
      /***** Remove post *****/
      DB_QueryDELETE ("can not remove a post",
		      "DELETE FROM tl_posts"
		      " WHERE PstCod=%ld",
		      SocNot->Cod);
  }

/*****************************************************************************/
/*********************** Get code of note of a publication *******************/
/*****************************************************************************/

static long TL_GetNotCodOfPublication (long PubCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long NotCod = -1L;

   /***** Get code of note from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get code of note",
		       "SELECT NotCod FROM tl_pubs"
		       " WHERE PubCod=%ld",
		       PubCod) == 1)   // Result should have a unique row
     {
      /* Get code of note */
      row = mysql_fetch_row (mysql_res);
      NotCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NotCod;
  }

/*****************************************************************************/
/*************** Get code of publication of the original note ****************/
/*****************************************************************************/

static long TL_GetPubCodOfOriginalNote (long NotCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long OriginalPubCod = -1L;

   /***** Get code of publication of the original note *****/
   if (DB_QuerySELECT (&mysql_res,"can not get code of publication",
		       "SELECT PubCod FROM tl_pubs"
		       " WHERE NotCod=%ld AND PubType=%u",
		       NotCod,(unsigned) TL_PUB_ORIGINAL_NOTE) == 1)   // Result should have a unique row
     {
      /* Get code of publication (row[0]) */
      row = mysql_fetch_row (mysql_res);
      OriginalPubCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return OriginalPubCod;
  }

/*****************************************************************************/
/**************** Request the removal of a comment in a note *****************/
/*****************************************************************************/

void TL_RequestRemComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Request the removal of comment in note *****/
   TL_RequestRemovalComment ();

   /***** Write timeline again (user) *****/
   TL_ShowTimelineUsr ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RequestRemComGbl (void)
  {
   /***** Request the removal of comment in note *****/
   TL_RequestRemovalComment ();

   /***** Write timeline again (global) *****/
   TL_ShowTimelineGbl2 ();
  }

static void TL_RequestRemovalComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct TL_Comment SocCom;
   bool ItsMe;

   /***** Initialize image *****/
   Med_MediaConstructor (&SocCom.Content.Media);

   /***** Get data of comment *****/
   SocCom.PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (&SocCom);

   if (SocCom.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (SocCom.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Show question and button to remove comment *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_comment);

	 /* Show comment */
	 TL_WriteComment (&SocCom,
			  TL_TOP_MESSAGE_NONE,-1L,
			  true);	// Alone

	 /* End alert */
	 Gbl.Timeline.PubCod = SocCom.PubCod;	// Publication to be removed
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	    Ale_ShowAlertAndButton2 (ActRemSocComUsr,"timeline",NULL,
	                             TL_PutParamsRemoveCommment,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlertAndButton2 (ActRemSocComGbl,NULL,NULL,
	                             TL_PutParamsRemoveCommment,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Med_MediaDestructor (&SocCom.Content.Media);
  }

/*****************************************************************************/
/******************** Put parameters to remove a comment *********************/
/*****************************************************************************/

static void TL_PutParamsRemoveCommment (void)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted ();
   else
      Usr_PutHiddenParamWho (Gbl.Timeline.Who);
   TL_PutHiddenParamPubCod (Gbl.Timeline.PubCod);
  }

/*****************************************************************************/
/***************************** Remove a comment ******************************/
/*****************************************************************************/

void TL_RemoveComUsr (void)
  {
   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Remove a comment *****/
   TL_RemoveComment ();

   /***** Write updated timeline after removing (user) *****/
   TL_ShowTimelineUsr ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RemoveComGbl (void)
  {
   /***** Remove a comment *****/
   TL_RemoveComment ();

   /***** Write updated timeline after removing (global) *****/
   TL_ShowTimelineGbl2 ();
  }

static void TL_RemoveComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Comment_removed;
   struct TL_Comment SocCom;
   bool ItsMe;

   /***** Initialize image *****/
   Med_MediaConstructor (&SocCom.Content.Media);

   /***** Get data of comment *****/
   SocCom.PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (&SocCom);

   if (SocCom.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (SocCom.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Remove media associated to comment
	        and delete comment from database *****/
	 TL_RemoveCommentMediaAndDBEntries (SocCom.PubCod);

	 /***** Reset fields of comment *****/
	 TL_ResetComment (&SocCom);

	 /***** Message of success *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Comment_removed);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Med_MediaDestructor (&SocCom.Content.Media);
  }

/*****************************************************************************/
/*************** Remove comment media and database entries *******************/
/*****************************************************************************/

static void TL_RemoveCommentMediaAndDBEntries (long PubCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long MedCod;;

   /***** Remove media associated to comment *****/
   if (DB_QuerySELECT (&mysql_res,"can not get media",
		       "SELECT MedCod"	// row[0]
		       " FROM tl_comments"
		       " WHERE PubCod=%ld",
		       PubCod) == 1)   // Result should have a unique row
     {
      /* Get media code */
      row = mysql_fetch_row (mysql_res);
      MedCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Remove media */
      Med_RemoveMedia (MedCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Mark possible notifications on this comment as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_COMMENT,PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV    ,PubCod);
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_MENTION,PubCod);

   /***** Remove favs for this comment *****/
   DB_QueryDELETE ("can not remove favs for comment",
		   "DELETE FROM tl_comments_fav"
		   " WHERE PubCod=%ld",
		   PubCod);

   /***** Remove content of this comment *****/
   DB_QueryDELETE ("can not remove a comment",
		   "DELETE FROM tl_comments"
		   " WHERE PubCod=%ld",
		   PubCod);

   /***** Remove this comment *****/
   DB_QueryDELETE ("can not remove a comment",
		   "DELETE FROM tl_pubs"
	           " WHERE PubCod=%ld"
	           " AND PublisherCod=%ld"	// Extra check: I am the author
	           " AND PubType=%u",		// Extra check: it's a comment
		   PubCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) TL_PUB_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/************* Remove all the content of a user from database ****************/
/*****************************************************************************/

void TL_RemoveUsrContent (long UsrCod)
  {
   /***** Remove favs for comments *****/
   /* Remove all favs made by this user in any comment */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_comments_fav"
		   " WHERE UsrCod=%ld",
		   UsrCod);

   /* Remove all favs for all comments of this user */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_comments_fav"
	           " USING tl_pubs,tl_comments_fav"
	           " WHERE tl_pubs.PublisherCod=%ld"	// Author of the comment
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments_fav.PubCod",
		   UsrCod,(unsigned) TL_PUB_COMMENT_TO_NOTE);

   /* Remove all favs for all comments in all the notes of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments_fav"
	           " USING tl_notes,tl_pubs,tl_comments_fav"
	           " WHERE tl_notes.UsrCod=%ld"	// Author of the note
	           " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments_fav.PubCod",
		   UsrCod,(unsigned) TL_PUB_COMMENT_TO_NOTE);

   /***** Remove favs for notes *****/
   /* Remove all favs made by this user in any note */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_notes_fav"
		   " WHERE UsrCod=%ld",
		   UsrCod);

   /* Remove all favs for all notes of this user */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_notes_fav"
	           " USING tl_notes,tl_notes_fav"
	           " WHERE tl_notes.UsrCod=%ld"	// Author of the note
	           " AND tl_notes.NotCod=tl_notes_fav.NotCod",
		   UsrCod);

   /***** Remove comments *****/
   /* Remove content of all the comments in all the notes of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments"
	           " USING tl_notes,tl_pubs,tl_comments"
	           " WHERE tl_notes.UsrCod=%ld"
		   " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments.PubCod",
		   UsrCod,(unsigned) TL_PUB_COMMENT_TO_NOTE);

   /* Remove all the comments from any user in any note of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_pubs"
	           " USING tl_notes,tl_pubs"
	           " WHERE tl_notes.UsrCod=%ld"
		   " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u",
		   UsrCod,(unsigned) TL_PUB_COMMENT_TO_NOTE);

   /* Remove content of all the comments of the user in any note */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments"
	           " USING tl_pubs,tl_comments"
	           " WHERE tl_pubs.PublisherCod=%ld"
	           " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments.PubCod",
		   UsrCod,(unsigned) TL_PUB_COMMENT_TO_NOTE);

   /***** Remove all the posts of the user *****/
   DB_QueryDELETE ("can not remove posts",
		   "DELETE FROM tl_posts"
		   " WHERE PstCod IN"
		   " (SELECT Cod FROM tl_notes"
	           " WHERE UsrCod=%ld AND NoteType=%u)",
		   UsrCod,(unsigned) TL_NOTE_POST);

   /***** Remove all the publications of any user authored by the user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tl_pubs"
                   " USING tl_notes,tl_pubs"
	           " WHERE tl_notes.UsrCod=%ld"
                   " AND tl_notes.NotCod=tl_pubs.NotCod",
		   UsrCod);

   /***** Remove all the publications of the user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tl_pubs"
		   " WHERE PublisherCod=%ld",
		   UsrCod);

   /***** Remove all the notes of the user *****/
   DB_QueryDELETE ("can not remove notes",
		   "DELETE FROM tl_notes"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/****************** Check if a user has published a note *********************/
/*****************************************************************************/

static bool TL_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user has shared a note",
			  "SELECT COUNT(*) FROM tl_pubs"
			  " WHERE NotCod=%ld"
			  " AND PublisherCod=%ld"
			  " AND PubType=%u",
			  NotCod,
			  UsrCod,
			  (unsigned) TL_PUB_SHARED_NOTE) != 0);
  }

/*****************************************************************************/
/****************** Check if a user has favourited a note ********************/
/*****************************************************************************/

static bool TL_CheckIfNoteIsFavedByUsr (long NotCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user"
			  " has favourited a note",
			  "SELECT COUNT(*) FROM tl_notes_fav"
			  " WHERE NotCod=%ld AND UsrCod=%ld",
			  NotCod,UsrCod) != 0);
  }

/*****************************************************************************/
/**************** Check if a user has favourited a comment *******************/
/*****************************************************************************/

static bool TL_CheckIfCommIsFavedByUsr (long PubCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user"
			  " has favourited a comment",
			  "SELECT COUNT(*) FROM tl_comments_fav"
			  " WHERE PubCod=%ld AND UsrCod=%ld",
			  PubCod,UsrCod) != 0);
  }

/*****************************************************************************/
/********** Get number of times a note has been shared in timeline ***********/
/*****************************************************************************/

static void TL_UpdateNumTimesANoteHasBeenShared (struct TL_Note *SocNot)
  {
   /***** Get number of times (users) this note has been shared *****/
   SocNot->NumShared =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a note has been shared",
			     "SELECT COUNT(*) FROM tl_pubs"
			     " WHERE NotCod=%ld"
			     " AND PublisherCod<>%ld"
			     " AND PubType=%u",
			     SocNot->NotCod,
			     SocNot->UsrCod,	// The author
			     (unsigned) TL_PUB_SHARED_NOTE);
  }

/*****************************************************************************/
/*************** Get number of times a note has been favourited **************/
/*****************************************************************************/

static void TL_GetNumTimesANoteHasBeenFav (struct TL_Note *SocNot)
  {
   /***** Get number of times (users) this note has been favourited *****/
   SocNot->NumFavs =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a note has been favourited",
			     "SELECT COUNT(*) FROM tl_notes_fav"
			     " WHERE NotCod=%ld"
			     " AND UsrCod<>%ld",	// Extra check
			     SocNot->NotCod,
			     SocNot->UsrCod);		// The author
  }

/*****************************************************************************/
/************ Get number of times a comment has been favourited **************/
/*****************************************************************************/

static void TL_GetNumTimesACommHasBeenFav (struct TL_Comment *SocCom)
  {
   /***** Get number of times (users) this comment has been favourited *****/
   SocCom->NumFavs =
   (unsigned) DB_QueryCOUNT ("can not get number of times"
			     " a comment has been favourited",
			     "SELECT COUNT(*) FROM tl_comments_fav"
			     " WHERE PubCod=%ld"
			     " AND UsrCod<>%ld",	// Extra check
			     SocCom->PubCod,
			     SocCom->UsrCod);		// The author
  }

/*****************************************************************************/
/******************* Show users who have shared this note ********************/
/*****************************************************************************/

static void TL_ShowUsrsWhoHaveSharedNote (const struct TL_Note *SocNot,
					  TL_HowMany_t HowMany)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs = 0;

   /***** Get users who have shared this note *****/
   if (SocNot->NumShared)
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT PublisherCod FROM tl_pubs"
				 " WHERE NotCod=%ld"
				 " AND PublisherCod<>%ld"
				 " AND PubType=%u"
				 " ORDER BY PubCod LIMIT %u",
				 SocNot->NotCod,
				 SocNot->UsrCod,
				 (unsigned) TL_PUB_SHARED_NOTE,
				 HowMany == TL_SHOW_A_FEW_USRS ? TL_DEF_USRS_SHOWN :
				                                 TL_MAX_USRS_SHOWN);

   /***** Show users *****/
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_ShowNumSharersOrFavers (SocNot->NumShared);
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_ShowSharersOrFavers (&mysql_res,SocNot->NumShared,NumFirstUsrs);
   if (NumFirstUsrs < SocNot->NumShared)
      TL_PutFormToSeeAllSharersNote (SocNot,HowMany);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (SocNot->NumShared)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show users who have marked this note as favourite **************/
/*****************************************************************************/

static void TL_ShowUsrsWhoHaveMarkedNoteAsFav (const struct TL_Note *SocNot,
					       TL_HowMany_t HowMany)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs = 0;

   /***** Get users who have marked this note as favourite *****/
   if (SocNot->NumFavs)
     {
      /***** Get list of users from database *****/
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT UsrCod FROM tl_notes_fav"
				 " WHERE NotCod=%ld"
				 " AND UsrCod<>%ld"	// Extra check
				 " ORDER BY FavCod LIMIT %u",
				 SocNot->NotCod,
				 SocNot->UsrCod,
				 HowMany == TL_SHOW_A_FEW_USRS ? TL_DEF_USRS_SHOWN :
				                                 TL_MAX_USRS_SHOWN);
     }

   /***** Show users *****/
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_ShowNumSharersOrFavers (SocNot->NumFavs);
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_ShowSharersOrFavers (&mysql_res,SocNot->NumFavs,NumFirstUsrs);
   if (NumFirstUsrs < SocNot->NumFavs)		// Not all are shown
      TL_PutFormToSeeAllFaversNote (SocNot,HowMany);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (SocNot->NumFavs)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Show users who have marked this note as favourite **************/
/*****************************************************************************/

static void TL_ShowUsrsWhoHaveMarkedCommAsFav (const struct TL_Comment *SocCom,
					       TL_HowMany_t HowMany)
  {
   MYSQL_RES *mysql_res;
   unsigned NumFirstUsrs = 0;

   /***** Get users who have marked this comment as favourite *****/
   if (SocCom->NumFavs)
      /***** Get list of users from database *****/
      NumFirstUsrs =
      (unsigned) DB_QuerySELECT (&mysql_res,"can not get users",
				 "SELECT UsrCod FROM tl_comments_fav"
				 " WHERE PubCod=%ld"
				 " AND UsrCod<>%ld"	// Extra check
				 " ORDER BY FavCod LIMIT %u",
				 SocCom->PubCod,
				 SocCom->UsrCod,
				 HowMany == TL_SHOW_A_FEW_USRS ? TL_DEF_USRS_SHOWN :
				                                 TL_MAX_USRS_SHOWN);

   /***** Show users *****/
   HTM_DIV_Begin ("class=\"TL_NUM_USRS\"");
   TL_ShowNumSharersOrFavers (SocCom->NumFavs);
   HTM_DIV_End ();

   HTM_DIV_Begin ("class=\"TL_USRS\"");
   TL_ShowSharersOrFavers (&mysql_res,SocCom->NumFavs,NumFirstUsrs);
   if (NumFirstUsrs < SocCom->NumFavs)
      TL_PutFormToSeeAllFaversComment (SocCom,HowMany);
   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   if (SocCom->NumFavs)
      DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show sharers or favouriters ************************/
/*****************************************************************************/

static void TL_ShowNumSharersOrFavers (unsigned NumUsrs)
  {
   /***** Show number of users who have marked this note as favourite *****/
   HTM_TxtF ("&nbsp;%u",NumUsrs);
  }

static void TL_ShowSharersOrFavers (MYSQL_RES **mysql_res,
				    unsigned NumUsrs,unsigned NumFirstUsrs)
  {
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned NumUsrsShown = 0;
   struct UsrData UsrDat;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   if (NumUsrs)
     {
      /***** A list of users has been got from database *****/
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
	    row = mysql_fetch_row (*mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	    /***** Get user's data and show user's photo *****/
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
	      {
               HTM_DIV_Begin ("class=\"TL_SHARER\"");
	       ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
	       Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
	                                             NULL,
	                         "PHOTO12x16",Pho_ZOOM,true);	// Use unique id
               HTM_DIV_End ();

               NumUsrsShown++;
              }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}
     }
  }

/*****************************************************************************/
/******************** Get data of note using its code ************************/
/*****************************************************************************/

static void TL_GetDataOfNoteByCod (struct TL_Note *SocNot)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (SocNot->NotCod > 0)
     {
      /***** Get data of note from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of note",
			  "SELECT NotCod,"			// row[0]
				 "NoteType,"			// row[1]
				 "Cod,"				// row[2]
				 "UsrCod,"			// row[3]
				 "HieCod,"			// row[4]
				 "Unavailable,"			// row[5]
				 "UNIX_TIMESTAMP(TimeNote)"	// row[6]
			  " FROM tl_notes"
			  " WHERE NotCod=%ld",
			  SocNot->NotCod))
	{
	 /***** Get data of note *****/
	 row = mysql_fetch_row (mysql_res);
	 TL_GetDataOfNoteFromRow (row,SocNot);
	}
      else
	 /***** Reset fields of note *****/
	 TL_ResetNote (SocNot);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Reset fields of note *****/
      TL_ResetNote (SocNot);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/

static void TL_GetDataOfCommByCod (struct TL_Comment *SocCom)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (SocCom->PubCod > 0)
     {
      /***** Get data of comment from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of comment",
			  "SELECT tl_pubs.PubCod,"			// row[0]
				 "tl_pubs.PublisherCod,"		// row[1]
				 "tl_pubs.NotCod,"			// row[2]
				 "UNIX_TIMESTAMP(tl_pubs.TimePublish),"	// row[3]
				 "tl_comments.Txt,"			// row[4]
				 "tl_comments.MedCod"			// row[5]
			  " FROM tl_pubs,tl_comments"
			  " WHERE tl_pubs.PubCod=%ld"
			  " AND tl_pubs.PubType=%u"
			  " AND tl_pubs.PubCod=tl_comments.PubCod",
			  SocCom->PubCod,(unsigned) TL_PUB_COMMENT_TO_NOTE))
	{
	 /***** Get data of comment *****/
	 row = mysql_fetch_row (mysql_res);
	 TL_GetDataOfCommentFromRow (row,SocCom);
	}
      else
	 /***** Reset fields of comment *****/
	 TL_ResetComment (SocCom);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Reset fields of comment *****/
      TL_ResetComment (SocCom);
  }

/*****************************************************************************/
/***************** Get data of publication using its code ********************/
/*****************************************************************************/

static void TL_GetDataOfPublicationFromRow (MYSQL_ROW row,struct TL_Publication *SocPub)
  {
   static const TL_TopMessage_t TopMessages[TL_NUM_PUB_TYPES] =
     {
      [TL_PUB_UNKNOWN        ] = TL_TOP_MESSAGE_NONE,
      [TL_PUB_ORIGINAL_NOTE  ] = TL_TOP_MESSAGE_NONE,
      [TL_PUB_SHARED_NOTE    ] = TL_TOP_MESSAGE_SHARED,
      [TL_PUB_COMMENT_TO_NOTE] = TL_TOP_MESSAGE_COMMENTED,
     };

   /***** Get code of publication (row[0]) *****/
   SocPub->PubCod       = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get note code (row[1]) *****/
   SocPub->NotCod       = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get publisher's code (row[2]) *****/
   SocPub->PublisherCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get type of publication (row[3]) *****/
   SocPub->PubType      = TL_GetPubTypeFromStr ((const char *) row[3]);
   SocPub->TopMessage   = TopMessages[SocPub->PubType];

   /***** Get time of the note (row[4]) *****/
   SocPub->DateTimeUTC  = Dat_GetUNIXTimeFromStr (row[4]);
  }

/*****************************************************************************/
/************************ Get data of note from row **************************/
/*****************************************************************************/

static void TL_GetDataOfNoteFromRow (MYSQL_ROW row,struct TL_Note *SocNot)
  {
   /***** Get code (row[0]) *****/
   SocNot->NotCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get note type (row[1]) *****/
   SocNot->NoteType    = TL_GetNoteTypeFromStr ((const char *) row[1]);

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

   /***** Get number of times this note has been shared *****/
   TL_UpdateNumTimesANoteHasBeenShared (SocNot);

   /***** Get number of times this note has been favourited *****/
   TL_GetNumTimesANoteHasBeenFav (SocNot);
  }

/*****************************************************************************/
/******* Get publication type from string number coming from database ********/
/*****************************************************************************/

static TL_PubType_t TL_GetPubTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < TL_NUM_PUB_TYPES)
         return (TL_PubType_t) UnsignedNum;

   return TL_PUB_UNKNOWN;
  }

/*****************************************************************************/
/********* Get note type from string number coming from database *************/
/*****************************************************************************/

static TL_NoteType_t TL_GetNoteTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < TL_NUM_NOTE_TYPES)
         return (TL_NoteType_t) UnsignedNum;

   return TL_NOTE_UNKNOWN;
  }

/*****************************************************************************/
/********************** Get data of comment from row *************************/
/*****************************************************************************/

static void TL_GetDataOfCommentFromRow (MYSQL_ROW row,struct TL_Comment *SocCom)
  {
   /*
   row[0]: PubCod
   row[1]: PublisherCod
   row[2]: NotCod
   row[3]: TimePublish
   row[4]: Txt
   row[5]: MedCod
    */
   /***** Get code of comment (row[0]) *****/
   SocCom->PubCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get (from) user code (row[1]) *****/
   SocCom->UsrCod      = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get code of note (row[2]) *****/
   SocCom->NotCod      = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get time of the note (row[3]) *****/
   SocCom->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[3]);

   /***** Get text content (row[4]) *****/
   Str_Copy (SocCom->Content.Txt,row[4],
             Cns_MAX_BYTES_LONG_TEXT);

   /***** Get number of times this comment has been favourited *****/
   TL_GetNumTimesACommHasBeenFav (SocCom);

   /***** Get media content (row[5]) *****/
   SocCom->Content.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
   Med_GetMediaDataByCod (&SocCom->Content.Media);
  }

/*****************************************************************************/
/*************************** Reset fields of note ****************************/
/*****************************************************************************/

static void TL_ResetNote (struct TL_Note *SocNot)
  {
   SocNot->NotCod      = -1L;
   SocNot->NoteType    = TL_NOTE_UNKNOWN;
   SocNot->UsrCod      = -1L;
   SocNot->HieCod      = -1L;
   SocNot->Cod         = -1L;
   SocNot->Unavailable = false;
   SocNot->DateTimeUTC = (time_t) 0;
   SocNot->NumShared   = 0;
  }

/*****************************************************************************/
/************************** Reset fields of comment **************************/
/*****************************************************************************/

static void TL_ResetComment (struct TL_Comment *SocCom)
  {
   SocCom->PubCod      = -1L;
   SocCom->UsrCod      = -1L;
   SocCom->NotCod      = -1L;
   SocCom->DateTimeUTC = (time_t) 0;
   SocCom->Content.Txt[0]  = '\0';
  }

/*****************************************************************************/
/******************* Clear unused old timelines in database ******************/
/*****************************************************************************/

void TL_ClearOldTimelinesDB (void)
  {
   /***** Remove timelines for expired sessions *****/
   DB_QueryDELETE ("can not remove old timelines",
		   "DELETE LOW_PRIORITY FROM tl_timelines"
                   " WHERE SessionId NOT IN (SELECT SessionId FROM sessions)");
  }

/*****************************************************************************/
/**************** Clear timeline for this session in database ****************/
/*****************************************************************************/

static void TL_ClearTimelineThisSession (void)
  {
   /***** Remove timeline for this session *****/
   DB_QueryDELETE ("can not remove timeline",
		   "DELETE FROM tl_timelines"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/****** Add just retrieved notes to current timeline for this session ********/
/*****************************************************************************/

static void TL_AddNotesJustRetrievedToTimelineThisSession (void)
  {
   DB_QueryINSERT ("can not insert notes in timeline",
		   "INSERT IGNORE INTO tl_timelines"
	           " (SessionId,NotCod)"
	           " SELECT DISTINCTROW '%s',NotCod FROM tl_not_codes",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/***************** Get notification of a new publication *********************/
/*****************************************************************************/

void TL_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                             char **ContentStr,
                             long PubCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct TL_Publication SocPub;
   struct TL_Note SocNot;
   struct PostContent Content;
   size_t Length;
   bool ContentCopied = false;

   /***** Return nothing on error *****/
   SocPub.PubType = TL_PUB_UNKNOWN;
   SummaryStr[0] = '\0';	// Return nothing on error
   Content.Txt[0] = '\0';

   /***** Get summary and content from post from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get data of publication",
		       "SELECT PubCod,"				// row[0]
			      "NotCod,"				// row[1]
			      "PublisherCod,"			// row[2]
			      "PubType,"			// row[3]
			      "UNIX_TIMESTAMP(TimePublish)"	// row[4]
		       " FROM tl_pubs WHERE PubCod=%ld",
		       PubCod) == 1)   // Result should have a unique row
     {
      /* Get data of publication */
      row = mysql_fetch_row (mysql_res);
      TL_GetDataOfPublicationFromRow (row,&SocPub);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get summary and content *****/
   switch (SocPub.PubType)
     {
      case TL_PUB_UNKNOWN:
	 break;
      case TL_PUB_ORIGINAL_NOTE:
      case TL_PUB_SHARED_NOTE:
	 /* Get data of note */
	 SocNot.NotCod = SocPub.NotCod;
	 TL_GetDataOfNoteByCod (&SocNot);

	 if (SocNot.NoteType == TL_NOTE_POST)
	   {
	    /***** Get content of post from database *****/
	    if (DB_QuerySELECT (&mysql_res,"can not get the content of a post",
			        "SELECT Txt"	// row[0]
			        " FROM tl_posts"
				" WHERE PstCod=%ld",
				SocNot.Cod) == 1)   // Result should have a unique row
	      {
	       /***** Get row *****/
	       row = mysql_fetch_row (mysql_res);

	       /****** Get content (row[0]) *****/
	       Str_Copy (Content.Txt,row[0],
	                 Cns_MAX_BYTES_LONG_TEXT);
	      }

	    /***** Free structure that stores the query result *****/
            DB_FreeMySQLResult (&mysql_res);

	    /***** Copy content string *****/
	    if (GetContent)
	      {
	       Length = strlen (Content.Txt);
	       if ((*ContentStr = (char *) malloc (Length + 1)) != NULL)
		 {
		  Str_Copy (*ContentStr,Content.Txt,
		            Length);
		  ContentCopied = true;
		 }
	      }

	    /***** Copy summary string *****/
	    Str_LimitLengthHTMLStr (Content.Txt,Ntf_MAX_CHARS_SUMMARY);
	    Str_Copy (SummaryStr,Content.Txt,
	              Ntf_MAX_BYTES_SUMMARY);
	   }
	 else
	    TL_GetNoteSummary (&SocNot,SummaryStr);
	 break;
      case TL_PUB_COMMENT_TO_NOTE:
	 /***** Get content of post from database *****/
	 if (DB_QuerySELECT (&mysql_res,"can not get the content"
				        " of a comment to a note",
			     "SELECT Txt"	// row[0]
			     " FROM tl_comments"
			     " WHERE PubCod=%ld",
			     SocPub.PubCod) == 1)   // Result should have a unique row
	   {
	    /***** Get row *****/
	    row = mysql_fetch_row (mysql_res);

	    /****** Get content (row[0]) *****/
	    Str_Copy (Content.Txt,row[0],
	              Cns_MAX_BYTES_LONG_TEXT);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

	 /***** Copy content string *****/
	 if (GetContent)
	   {
	    Length = strlen (Content.Txt);
	    if ((*ContentStr = (char *) malloc (Length + 1)) != NULL)
	      {
	       Str_Copy (*ContentStr,Content.Txt,
	                 Length);
	       ContentCopied = true;
	      }
	   }

	 /***** Copy summary string *****/
	 Str_LimitLengthHTMLStr (Content.Txt,Ntf_MAX_CHARS_SUMMARY);
	 Str_Copy (SummaryStr,Content.Txt,
	           Ntf_MAX_BYTES_SUMMARY);
	 break;
     }

   /***** Create empty content string if nothing copied *****/
   if (GetContent && !ContentCopied)
      if ((*ContentStr = (char *) malloc (1)) != NULL)
         (*ContentStr)[0] = '\0';
  }

/*****************************************************************************/
/*** Create a notification about mention for any nickname in a publication ***/
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
	    UsrDat.Nickname[Nickname.Length] = '\0';

	    if ((UsrDat.UsrCod = Nck_GetUsrCodFromNickname (UsrDat.Nickname)) > 0)
	      {
	       ItsMe = Usr_ItsMe (UsrDat.UsrCod);
	       if (!ItsMe)	// Not me
		 {
		  /* Get user's data */
		  Usr_GetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);

		  /* Create notification for the mentioned user *****/
		  CreateNotif = (UsrDat.NtfEvents.CreateNotif & (1 << Ntf_EVENT_TIMELINE_MENTION));
		  if (CreateNotif)
		    {
		     NotifyByEmail = (UsrDat.NtfEvents.SendEmail & (1 << Ntf_EVENT_TIMELINE_MENTION));
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

/*****************************************************************************/
/****************** Get number of publications from a user *******************/
/*****************************************************************************/

unsigned long TL_GetNumPubsUsr (long UsrCod)
  {
   /***** Get number of posts from a user from database *****/
   return DB_QueryCOUNT ("can not get number of publications from a user",
			 "SELECT COUNT(*) FROM tl_pubs"
			 " WHERE PublisherCod=%ld",
			 UsrCod);
  }
