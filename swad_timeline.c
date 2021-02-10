// swad_timeline.c: social timeline

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions
#include <sys/types.h>		// For time_t

#include "swad_database.h"
#include "swad_exam_announcement.h"
#include "swad_figure.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_message.h"
#include "swad_notice.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_setting.h"
#include "swad_timeline.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_share.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

// Number of recent publishings got and shown the first time, before refreshing
// Don't use big numbers because dynamic memory will be allocated to stored publications
#define TL_MAX_REC_PUBS_TO_GET_AND_SHOW	   10	// Recent publishings to show (first time)
#define TL_MAX_NEW_PUBS_TO_GET_AND_SHOW	  100	// New publishings retrieved
#define TL_MAX_OLD_PUBS_TO_GET_AND_SHOW	   20	// Old publishings are retrieved in packs of this size

#define TL_NUM_VISIBLE_COMMENTS	3	// Maximum number of comments visible before expanding them

#define TL_MAX_CHARS_IN_POST	1000	// Maximum number of characters in a post

#define TL_MAX_BYTES_SUBQUERY (128 - 1)

/*
   Timeline images will be saved with:
   · maximum width  of TL_IMAGE_SAVED_MAX_HEIGHT
   · maximum height of TL_IMAGE_SAVED_MAX_HEIGHT
   · maintaining the original aspect ratio (aspect ratio recommended: 3:2)
*/
#define TL_IMAGE_SAVED_MAX_WIDTH	768
#define TL_IMAGE_SAVED_MAX_HEIGHT	768
#define TL_IMAGE_SAVED_QUALITY		 90	// 1 to 100
// in timeline posts, the quality should not be high in order to speed up the loading of images

/*
mysql> SHOW TABLES LIKE 'tl_%';
+-----------------------+
| Tables_in_swad (tl_%) |
+-----------------------+
| tl_comments           |
| tl_comments_fav       |
| tl_notes              |
| tl_notes_fav          |
| tl_posts              |
| tl_pubs               |
| tl_timelines          |
+-----------------------+
7 rows in set (0.00 sec)

   The timeline is a set of publications.
   A publication can be:
   · an original note    (22783, 83% of 27396)
   · a shared note       (  750,  3% of 27396)
   · a comment to a note ( 3863, 14% of 27396)

    *Numbers are got from swad.ugr.es on february 2020

   ____tl_pubs____             _tl_comments_
  |               |           |             |
  | Publication p |---------->|  Comment c  |-----+
  |   (comment)   |           | (to note 2) |     |
  |_______________|           |_____________|     |
  |               |           |             |     |
  ·      ...      ·           ·     ...     ·     |
  ·      ...      ·           ·     ...     ·     |
  |_______________|           |_____________|     |
  |               |           |             |     |
  |Publication i+4|---------->|  Comment 1  |---+ |
  |   (comment)   |           | (to note n) |   | |
  |_______________|           |_____________|   | |
  |               |                (3863)       | |
  |Publication i+3|--                           | |
  |(original note)|  \                          | |
  |_______________|   \       ___tl_notes____   | |     _exam_announcements_
  |               |    \     |               |  | |    |                    |
  |Publication i+2|--   ---->|     Note n    |<-+ |    | Exam announcement  | (5571)
  |(original note)|  \       |(exam announc.)|-(2639)->|____________________|
  |_______________|   \      |_______________|  12%     ____files____
  |               |    \     |               |    |    |             |
  |Publication i+1|--   ---->|    Note n-1   |-(64)--->| Public file | (1473406)
  |(original note)|  \       | (public file) |  <1%    |_____________|
  |_______________|   \      |_______________|    |     _notices_
  |               |    \     |               |    |    |         |
  | Publication i |--   ---->|    Note n-2   |-(16693)>| Notice  | (14793)
  |(original note)|  \       |    (notice)   |  73%    |_________|
  |_______________|   \      |_______________|    |     __tl_posts___
  |               |    \     |               |    |    |             |
  ·      ...      ·     ---->|    Note n-3   |-(3119)->|    Post s   |
  ·      ...      ·          |   (tl. post)  |  14%    |             |
  |_______________|          |_______________|    |    |_____________|
  |               |          |               |    |    |             |
  | Publication 3 |          ·      ...      ·    |    ·     ...     · (3119)
  | (shared note) |---       ·      ...      ·    |    ·     ...     ·
  |_______________|   \      |_______________|    |    |_____________|
  |               |    \     |               |    |    |             |
  | Publication 2 |     ---->|     Note 2    |<---+    |    Post 1   |
  |(original note)|--------->|   (tl. post)  |-------->|             |
  |_______________|          |_______________|         |_____________|
  |               |          |               |          _forum_post_
  | Publication 1 |--------->|     Note 1    |         |            |
  |(original note)|          | (forum post)  |-(268)-->| Forum post | (66226)
  |_______________|          |_______________|   1%    |____________|
       (27396)                    (22783)

   A note can be:
   · a timeline post      ( 3119, 14% of 22783)
   · a public file        (   64, <1% of 22783)
   · an exam announcement ( 2639, 12% of 22783)
   · a notice             (16693, 73% of 22783)
   · a forum post         (  268,  1% of 22783)
   written by an author in a date-time.

   A note can have comments attached to it.
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
struct TL_SubQueries
  {
   char *TablePublishers;
   char Publishers[TL_MAX_BYTES_SUBQUERY + 1];
   char RangeBottom[TL_MAX_BYTES_SUBQUERY + 1];
   char RangeTop[TL_MAX_BYTES_SUBQUERY + 1];
   char AlreadyExists[TL_MAX_BYTES_SUBQUERY + 1];
  };

struct TL_RangePubsToGet
  {
   long Top;
   long Bottom;
  };

typedef enum
  {
   TL_DONT_HIGHLIGHT_NOTE,
   TL_HIGHLIGHT_NOTE,
  } TL_HighlightNote_t;

typedef enum
  {
   TL_DONT_SHOW_NOTE_ALONE,
   TL_SHOW_NOTE_ALONE,
  } TL_ShowNoteAlone_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

Usr_Who_t TL_GlobalWho;

#define TL_DEFAULT_WHO	Usr_WHO_FOLLOWED

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TL_InitTimelineGbl (struct TL_Timeline *Timeline);
static void TL_ShowHighlightedNote (struct TL_Timeline *Timeline,
                                    struct TL_Note *Not);
static void TL_ShowNoteAndTimelineGbl (struct TL_Timeline *Timeline);

static void TL_ShowTimelineGblHighlightingNot (struct TL_Timeline *Timeline,
	                                       long NotCod);
static void TL_ShowTimelineUsrHighlightingNot (struct TL_Timeline *Timeline,
                                               long NotCod);

static void TL_GetAndShowOldTimeline (struct TL_Timeline *Timeline);

static void TL_GetListPubsToShowInTimeline (struct TL_Timeline *Timeline);
static unsigned TL_GetMaxPubsToGet (const struct TL_Timeline *Timeline);
static long TL_GetPubCodFromSession (const char *FieldName);
static void TL_UpdateFirstLastPubCodesIntoSession (const struct TL_Timeline *Timeline);
static void TL_CreateTmpTableCurrentTimeline (const struct TL_Timeline *Timeline);
static void TL_CreateTmpTablePublishers (void);
static void TL_DropTmpTablesUsedToQueryTimeline (void);
static void TL_CreateSubQueryPublishers (const struct TL_Timeline *Timeline,
                                         struct TL_SubQueries *SubQueries);
static void TL_CreateSubQueryAlreadyExists (const struct TL_Timeline *Timeline,
                                            struct TL_SubQueries *SubQueries);
static void TL_CreateSubQueryRangeBottom (const struct TL_RangePubsToGet *RangePubsToGet,
                                          struct TL_SubQueries *SubQueries);
static void TL_CreateSubQueryRangeTop (const struct TL_RangePubsToGet *RangePubsToGet,
                                       struct TL_SubQueries *SubQueries);
static void TL_AllocateListPubs (struct TL_Timeline *Timeline,
                                 unsigned MaxPubsToGet);
static void TL_FreeListPubs (const struct TL_Timeline *Timeline);
static void TL_SelectTheMostRecentPub (const struct TL_SubQueries *SubQueries,
				       struct TL_Publication *Pub);
static void TL_InsertNoteInJustRetrievedNotes (long NotCod);
static void TL_InsertNoteInVisibleTimeline (long NotCod);

static void TL_ShowTimeline (struct TL_Timeline *Timeline,
                             const char *Title,long NotCodToHighlight);
static void TL_PutIconsTimeline (__attribute__((unused)) void *Args);

static void TL_FormStart (const struct TL_Timeline *Timeline,
	                  Act_Action_t ActionGbl,
                          Act_Action_t ActionUsr);

static void TL_PutFormWho (struct TL_Timeline *Timeline);
static Usr_Who_t TL_GetWhoFromDB (void);
static void Set_GlobalWho (Usr_Who_t Who);

static void TL_SaveWhoInDB (struct TL_Timeline *Timeline);

static void TL_ShowWarningYouDontFollowAnyUser (void);

static void TL_InsertNewPubsInTimeline (struct TL_Timeline *Timeline);
static void TL_ShowOldPubsInTimeline (struct TL_Timeline *Timeline);

static void TL_PutLinkToViewNewPublications (void);
static void TL_PutLinkToViewOldPublications (void);

static void TL_WriteNote (struct TL_Timeline *Timeline,
	                  const struct TL_Note *Not,
                          TL_TopMessage_t TopMessage,
                          long PublisherCod,	// Who did the action (publication, commenting, faving, sharing, mentioning)
                          TL_HighlightNote_t HighlightNote,	// Highlight note
                          TL_ShowNoteAlone_t ShowNoteAlone);	// Note is shown alone, not in a list
static void TL_WriteTopMessage (TL_TopMessage_t TopMessage,long PublisherCod);
static void TL_WriteAuthorNote (const struct UsrData *UsrDat);
static void TL_WriteDateTime (time_t TimeUTC);
static void TL_GetAndWritePost (long PstCod);
static void TL_PutFormGoToAction (const struct TL_Note *Not,
                                  const struct For_Forums *Forums);
static void TL_GetNoteSummary (const struct TL_Note *Not,
                               char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1]);
static void TL_StoreAndPublishNoteInternal (TL_NoteType_t NoteType,long Cod,struct TL_Publication *Pub);

static void TL_PutFormToWriteNewPost (struct TL_Timeline *Timeline);
static void TL_PutTextarea (const char *Placeholder,const char *ClassTextArea);

static long TL_ReceivePost (void);

static void TL_PutIconToToggleCommentNote (const char UniqueId[Frm_MAX_BYTES_ID + 1]);
static void TL_PutIconCommentDisabled (void);
static void TL_PutHiddenFormToWriteNewCommentToNote (const struct TL_Timeline *Timeline,
	                                             long NotCod,
                                                     const char IdNewComment[Frm_MAX_BYTES_ID + 1]);
static unsigned long TL_GetNumCommentsInNote (long NotCod);
static void TL_WriteCommentsInNote (struct TL_Timeline *Timeline,
				    const struct TL_Note *Not,
				    unsigned NumComments);
static void TL_FormToShowHiddenComments (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
			                 long NotCod,
					 char IdComments[Frm_MAX_BYTES_ID + 1],
					 unsigned NumInitialComments);
static unsigned TL_WriteHiddenComments (struct TL_Timeline *Timeline,
                                        long NotCod,
				        char IdComments[Frm_MAX_BYTES_ID + 1],
					unsigned NumInitialCommentsToGet);
static void TL_WriteOneCommentInList (struct TL_Timeline *Timeline,
                                      MYSQL_RES *mysql_res);
static void TL_LinkToShowOnlyLatestComments (const char IdComments[Frm_MAX_BYTES_ID + 1]);
static void TL_LinkToShowPreviousComments (const char IdComments[Frm_MAX_BYTES_ID + 1],
				           unsigned NumInitialComments);
static void TL_PutIconToToggleComments (const char *UniqueId,
                                        const char *Icon,const char *Text);
static void TL_WriteComment (struct TL_Timeline *Timeline,
	                     struct TL_Comment *Com,
                             TL_TopMessage_t TopMessage,long UsrCod,
                             bool ShowCommentAlone);	// Comment is shown alone, not in a list
static void TL_WriteAuthorComment (struct UsrData *UsrDat);

static void TL_PutFormToRemoveComment (const struct TL_Timeline *Timeline,
	                               long PubCod);

static void TL_PutFormToRemovePublication (const struct TL_Timeline *Timeline,
                                           long NotCod);

static void TL_PutHiddenParamNotCod (long NotCod);

static long TL_ReceiveComment (void);

static void TL_RequestRemovalNote (struct TL_Timeline *Timeline);
static void TL_PutParamsRemoveNote (void *Timeline);
static void TL_RemoveNote (void);
static void TL_RemoveNoteMediaAndDBEntries (struct TL_Note *Not);

static long TL_GetNotCodFromPubCod (long PubCod);

static void TL_RequestRemovalComment (struct TL_Timeline *Timeline);
static void TL_PutParamsRemoveComment (void *Timeline);
static void TL_RemoveComment (void);
static void TL_RemoveCommentMediaAndDBEntries (long PubCod);

static void TL_GetDataOfPublicationFromNextRow (MYSQL_RES *mysql_res,
                                                struct TL_Publication *Pub);
static void TL_GetDataOfNoteFromRow (MYSQL_ROW row,struct TL_Note *Not);
static TL_PubType_t TL_GetPubTypeFromStr (const char *Str);
static TL_NoteType_t TL_GetNoteTypeFromStr (const char *Str);
static void TL_GetDataOfCommentFromRow (MYSQL_ROW row,struct TL_Comment *Com);

static void TL_ResetPublication (struct TL_Publication *Pub);
static void TL_ResetNote (struct TL_Note *Not);
static void TL_ResetComment (struct TL_Comment *Com);

static void TL_ClearTimelineThisSession (void);
static void TL_AddNotesJustRetrievedToTimelineThisSession (void);

/*****************************************************************************/
/************************ Initialize global timeline *************************/
/*****************************************************************************/

static void TL_InitTimelineGbl (struct TL_Timeline *Timeline)
  {
   /***** Reset timeline context *****/
   TL_ResetTimeline (Timeline);

   /***** Mark all my notifications about timeline as seen *****/
   TL_MarkMyNotifAsSeen ();

   /***** Get which users *****/
   Timeline->Who = TL_GetGlobalWho ();
  }

/*****************************************************************************/
/*************************** Reset timeline context **************************/
/*****************************************************************************/

void TL_ResetTimeline (struct TL_Timeline *Timeline)
  {
   Timeline->UsrOrGbl  = TL_TIMELINE_GBL;
   Timeline->Who       = TL_DEFAULT_WHO;
   Timeline->WhatToGet = TL_GET_RECENT_TIMELINE;
   Timeline->Pubs.Num  = 0,
   Timeline->Pubs.Lst  = NULL,
   Timeline->NotCod    = -1L;
   Timeline->PubCod    = -1L;
  }

/*****************************************************************************/
/**************** Show highlighted note and global timeline ******************/
/*****************************************************************************/

void TL_ShowTimelineGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Save which users in database *****/
   TL_SaveWhoInDB (&Timeline);

   /***** Show highlighted note and global timeline *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

/*****************************************************************************/
/**************** Show highlighted note and global timeline ******************/
/*****************************************************************************/

static void TL_ShowNoteAndTimelineGbl (struct TL_Timeline *Timeline)
  {
   long PubCod;
   struct TL_Note Not;

   /***** Initialize note code to -1 ==> no highlighted note *****/
   Not.NotCod = -1L;

   /***** Get parameter with the code of a publication *****/
   // This parameter is optional. It can be provided by a notification.
   // If > 0 ==> the note is shown highlighted above the timeline
   PubCod = TL_GetParamPubCod ();

   /***** If a note should be highlighted ==> get code of note from database *****/
   if (PubCod > 0)
      Not.NotCod = TL_GetNotCodFromPubCod (PubCod);

   /***** If a note should be highlighted ==> show it above the timeline *****/
   if (Not.NotCod > 0)
      /***** Show the note highlighted above the timeline *****/
      TL_ShowHighlightedNote (Timeline,&Not);

   /***** Show timeline with possible highlighted note *****/
   TL_ShowTimelineGblHighlightingNot (Timeline,Not.NotCod);
  }

/*****************************************************************************/
/****************** Show highlighted note above timeline *********************/
/*****************************************************************************/

static void TL_ShowHighlightedNote (struct TL_Timeline *Timeline,
                                    struct TL_Note *Not)
  {
   struct UsrData PublisherDat;
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

   /***** Get other parameters *****/
   /* Get the publisher who did the action
      (publishing, commenting, faving, sharing, mentioning) */
   Usr_GetParamOtherUsrCodEncrypted (&PublisherDat);

   /* Get what he/she did */
   NotifyEvent = Ntf_GetParamNotifyEvent ();

   /***** Show the note highlighted *****/
   TL_GetDataOfNoteByCod (Not);
   TL_WriteNote (Timeline,Not,
		 TopMessages[NotifyEvent],PublisherDat.UsrCod,
		 TL_HIGHLIGHT_NOTE,
		 TL_SHOW_NOTE_ALONE);
  }

/*****************************************************************************/
/******************* Show global timeline highlighting a note ****************/
/*****************************************************************************/

static void TL_ShowTimelineGblHighlightingNot (struct TL_Timeline *Timeline,
	                                       long NotCod)
  {
   extern const char *Txt_Timeline;

   /***** Get list of pubications/notes to show in timeline *****/
   Timeline->UsrOrGbl  = TL_TIMELINE_GBL;
   Timeline->WhatToGet = TL_GET_RECENT_TIMELINE;
   TL_GetListPubsToShowInTimeline (Timeline);

   /***** Show timeline *****/
   TL_ShowTimeline (Timeline,Txt_Timeline,NotCod);

   /***** Free memory used for publications *****/
   TL_FreeListPubs (Timeline);
  }

/*****************************************************************************/
/********************* Show timeline of a selected user **********************/
/*****************************************************************************/

void TL_ShowTimelineUsr (struct TL_Timeline *Timeline)
  {
   TL_ShowTimelineUsrHighlightingNot (Timeline,-1L);
  }

/*****************************************************************************/
/************ Show timeline of a selected user highlighting a note ***********/
/*****************************************************************************/

static void TL_ShowTimelineUsrHighlightingNot (struct TL_Timeline *Timeline,
                                               long NotCod)
  {
   extern const char *Txt_Timeline_OF_A_USER;

   /***** Get list of pubications/notes to show in timeline *****/
   Timeline->UsrOrGbl  = TL_TIMELINE_USR;
   Timeline->WhatToGet = TL_GET_RECENT_TIMELINE;
   TL_GetListPubsToShowInTimeline (Timeline);

   /***** Show timeline *****/
   TL_ShowTimeline (Timeline,
                    Str_BuildStringStr (Txt_Timeline_OF_A_USER,
					Gbl.Usrs.Other.UsrDat.FirstName),
		    NotCod);
   Str_FreeString ();

   /***** Free memory used for publications *****/
   TL_FreeListPubs (Timeline);
  }

/*****************************************************************************/
/************** Refresh new publications in timeline via AJAX ****************/
/*****************************************************************************/

void TL_RefreshNewTimelineGbl (void)
  {
   struct TL_Timeline Timeline;

   if (Gbl.Session.IsOpen)	// If session has been closed, do not write anything
     {
      /***** Reset timeline context *****/
      TL_ResetTimeline (&Timeline);

      /***** Get which users *****/
      Timeline.Who = TL_GetGlobalWho ();

      /***** Get list of pubications/notes to show in timeline *****/
      Timeline.UsrOrGbl  = TL_TIMELINE_GBL;
      Timeline.WhatToGet = TL_GET_ONLY_NEW_PUBS;
      TL_GetListPubsToShowInTimeline (&Timeline);

      /***** Show new timeline *****/
      TL_InsertNewPubsInTimeline (&Timeline);

      /***** Free memory used for publications *****/
      TL_FreeListPubs (&Timeline);
     }
  }

/*****************************************************************************/
/**************** View old publications in timeline via AJAX *****************/
/*****************************************************************************/

void TL_RefreshOldTimelineGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get which users *****/
   Timeline.Who = TL_GetGlobalWho ();

   /***** Show old publications *****/
   Timeline.UsrOrGbl  = TL_TIMELINE_GBL;
   Timeline.WhatToGet = TL_GET_ONLY_OLD_PUBS;
   TL_GetAndShowOldTimeline (&Timeline);
  }

void TL_RefreshOldTimelineUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Get user whom profile is displayed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())	// Existing user
     {
      /***** Reset timeline context *****/
      TL_ResetTimeline (&Timeline);

      /***** If user exists, show old publications *****/
      Timeline.UsrOrGbl  = TL_TIMELINE_USR;
      Timeline.WhatToGet = TL_GET_ONLY_OLD_PUBS;
      TL_GetAndShowOldTimeline (&Timeline);
     }
  }

/*****************************************************************************/
/**************** Get and show old publications in timeline ******************/
/*****************************************************************************/

static void TL_GetAndShowOldTimeline (struct TL_Timeline *Timeline)
  {
   /***** Get list of pubications/notes to show in timeline *****/
   TL_GetListPubsToShowInTimeline (Timeline);

   /***** Show old timeline *****/
   TL_ShowOldPubsInTimeline (Timeline);

   /***** Free memory used for publications *****/
   TL_FreeListPubs (Timeline);
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
/************ Get list of pubications/notes to show in timeline **************/
/*****************************************************************************/

static void TL_GetListPubsToShowInTimeline (struct TL_Timeline *Timeline)
  {
   struct TL_SubQueries SubQueries;
   struct TL_RangePubsToGet RangePubsToGet;
   unsigned MaxPubsToGet = TL_GetMaxPubsToGet (Timeline);
   unsigned NumPub;

   /***** Clear timeline for this session in database *****/
   if (Timeline->WhatToGet == TL_GET_RECENT_TIMELINE)
      TL_ClearTimelineThisSession ();

   /***** Create temporary table with notes in current timeline *****/
   TL_CreateTmpTableCurrentTimeline (Timeline);

   /***** Create temporary table and subquery with potential publishers *****/
   TL_CreateSubQueryPublishers (Timeline,&SubQueries);

   /***** Create subquery to get only notes not present in timeline *****/
   TL_CreateSubQueryAlreadyExists (Timeline,&SubQueries);

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
   switch (Timeline->WhatToGet)
     {
      case TL_GET_ONLY_NEW_PUBS:	 // Get the publications (without limit) newer than LastPubCod
	 /* This query is made via AJAX automatically from time to time */
         RangePubsToGet.Top    = 0;	// +Infinite
	 RangePubsToGet.Bottom = TL_GetPubCodFromSession ("LastPubCod");
	 break;
      case TL_GET_ONLY_OLD_PUBS:	 // Get some limited publications older than FirstPubCod
	 /* This query is made via AJAX
	    when I click in link to get old publications */
	 RangePubsToGet.Top    = TL_GetPubCodFromSession ("FirstPubCod");
         RangePubsToGet.Bottom = 0;	// -Infinite
	 break;
      case TL_GET_RECENT_TIMELINE:	 // Get some limited recent publications
      default:
	 /* This is the first query to get initial timeline shown
	    ==> no notes yet in current timeline table */
         RangePubsToGet.Top    = 0;	// +Infinite
         RangePubsToGet.Bottom = 0;	// -Infinite
	 break;
     }
   /* Create subquery with bottom range of publications to get from tl_pubs.
      Bottom publication code remains unchanged in all iterations of the next loop. */
   TL_CreateSubQueryRangeBottom (&RangePubsToGet,&SubQueries);

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
   /* Allocate memory to store publications */
   TL_AllocateListPubs (Timeline,MaxPubsToGet);

   for (NumPub = 0;
	NumPub < MaxPubsToGet;
	NumPub++)
     {
      /* Create subquery with top range of publications to get from tl_pubs
         In each iteration of this loop, top publication code is changed to a lower value */
      TL_CreateSubQueryRangeTop (&RangePubsToGet,&SubQueries);

      /* Select the most recent publication from tl_pubs */
      TL_SelectTheMostRecentPub (&SubQueries,&Timeline->Pubs.Lst[NumPub]);
      if (Timeline->Pubs.Lst[NumPub].PubCod <= 0)	// Nothing got ==> abort loop
         break;	// Last publication

      /* Insert note in temporary tables with just retrieved notes.
	 These tables will be used to not get notes already shown */
      TL_InsertNoteInJustRetrievedNotes (Timeline->Pubs.Lst[NumPub].NotCod);
      if (Timeline->WhatToGet == TL_GET_ONLY_OLD_PUBS)
	 TL_InsertNoteInVisibleTimeline (Timeline->Pubs.Lst[NumPub].NotCod);

      RangePubsToGet.Top = Timeline->Pubs.Lst[NumPub].PubCod;	// Narrow the range for the next iteration
     }
   Timeline->Pubs.Num = NumPub;	// Number of publications actually got

   /***** Update first (oldest) and last (more recent) publication codes
          into session for next refresh *****/
   TL_UpdateFirstLastPubCodesIntoSession (Timeline);

   /***** Add notes just retrieved to current timeline for this session *****/
   TL_AddNotesJustRetrievedToTimelineThisSession ();

   /***** Drop temporary tables *****/
   TL_DropTmpTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/********* Get maximum number of publications to get from database ***********/
/*****************************************************************************/

static unsigned TL_GetMaxPubsToGet (const struct TL_Timeline *Timeline)
  {
   static const unsigned MaxPubsToGet[TL_NUM_WHAT_TO_GET] =
     {
      [TL_GET_RECENT_TIMELINE] = TL_MAX_REC_PUBS_TO_GET_AND_SHOW,
      [TL_GET_ONLY_NEW_PUBS  ] = TL_MAX_NEW_PUBS_TO_GET_AND_SHOW,
      [TL_GET_ONLY_OLD_PUBS  ] = TL_MAX_OLD_PUBS_TO_GET_AND_SHOW,
     };

   return MaxPubsToGet[Timeline->WhatToGet];
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
/************* Update first (oldest) and last (more recent)    ***************/
/************* publication codes into session for next refresh ***************/
/*****************************************************************************/

static void TL_UpdateFirstLastPubCodesIntoSession (const struct TL_Timeline *Timeline)
  {
   long FirstPubCod;

   switch (Timeline->WhatToGet)
     {
      case TL_GET_ONLY_NEW_PUBS:
	 DB_QueryUPDATE ("can not update first/last publication codes into session",
			 "UPDATE sessions"
			 " SET LastPubCod="
			      "(SELECT IFNULL(MAX(PubCod),0)"
			      " FROM tl_pubs)"	// The most recent publication
			 " WHERE SessionId='%s'",
			 Gbl.Session.Id);
	 break;
      case TL_GET_ONLY_OLD_PUBS:
	 // The oldest publication code retrieved and shown
	 FirstPubCod = Timeline->Pubs.Num ? Timeline->Pubs.Lst[Timeline->Pubs.Num - 1].PubCod :
			                    0;

	 DB_QueryUPDATE ("can not update first/last publication codes into session",
			 "UPDATE sessions"
			 " SET FirstPubCod=%ld"
			 " WHERE SessionId='%s'",
			 FirstPubCod,
			 Gbl.Session.Id);
	 break;
      case TL_GET_RECENT_TIMELINE:
	 // The oldest publication code retrieved and shown
	 FirstPubCod = Timeline->Pubs.Num ? Timeline->Pubs.Lst[Timeline->Pubs.Num - 1].PubCod :
			                    0;

	 DB_QueryUPDATE ("can not update first/last publication codes into session",
			 "UPDATE sessions"
			 " SET FirstPubCod=%ld,"
			      "LastPubCod="
			      "(SELECT IFNULL(MAX(PubCod),0)"
			      " FROM tl_pubs)"	// The most recent publication
			 " WHERE SessionId='%s'",
			 FirstPubCod,
			 Gbl.Session.Id);
	 break;
     }
  }

/*****************************************************************************/
/************* Create temporary tables used to query timeline ****************/
/*****************************************************************************/

static void TL_CreateTmpTableCurrentTimeline (const struct TL_Timeline *Timeline)
  {
   /***** Create temporary table with notes just retrieved *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_tmp_just_retrieved_notes "
	     "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
	     " ENGINE=MEMORY");

   if (Timeline->WhatToGet == TL_GET_ONLY_OLD_PUBS)
      /***** Create temporary table with all notes visible in timeline *****/
      DB_Query ("can not create temporary table",
		"CREATE TEMPORARY TABLE tl_tmp_visible_timeline "
		"(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
		" ENGINE=MEMORY"
		" SELECT NotCod FROM tl_timelines WHERE SessionId='%s'",
		Gbl.Session.Id);
  }

static void TL_CreateTmpTablePublishers (void)
  {
   /***** Create temporary table with me and the users I follow *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_tmp_publishers "
	     "(UsrCod INT NOT NULL,"
	     "UNIQUE INDEX(UsrCod))"
	     " ENGINE=MEMORY"
	     " SELECT %ld AS UsrCod"		// Me
	     " UNION"
	     " SELECT FollowedCod AS UsrCod"	// Users I follow
	     " FROM usr_follow"
	     " WHERE FollowerCod=%ld",
	     Gbl.Usrs.Me.UsrDat.UsrCod,
	     Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************** Drop temporary tables used to query timeline ****************/
/*****************************************************************************/

static void TL_DropTmpTablesUsedToQueryTimeline (void)
  {
   DB_Query ("can not remove temporary tables",
	     "DROP TEMPORARY TABLE IF EXISTS "
             "tl_tmp_just_retrieved_notes,"
             "tl_tmp_visible_timeline,"
	     "tl_tmp_publishers");
  }

/*****************************************************************************/
/******* Create temporary table and subquery with potential publishers *******/
/*****************************************************************************/

static void TL_CreateSubQueryPublishers (const struct TL_Timeline *Timeline,
                                         struct TL_SubQueries *SubQueries)
  {
   /***** Create temporary table and subquery with potential publishers *****/
   switch (Timeline->UsrOrGbl)
     {
      case TL_TIMELINE_USR:		// Show the timeline of a user
	 SubQueries->TablePublishers = "";
	 sprintf (SubQueries->Publishers,"tl_pubs.PublisherCod=%ld AND ",
	          Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case TL_TIMELINE_GBL:		// Show the global timeline
	 switch (Timeline->Who)
	   {
	    case Usr_WHO_ME:		// Show my timeline
	       SubQueries->TablePublishers = "";
	       snprintf (SubQueries->Publishers,sizeof (SubQueries->Publishers),
	                 "tl_pubs.PublisherCod=%ld AND ",
	                 Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
	    case Usr_WHO_FOLLOWED:	// Show the timeline of the users I follow
	       TL_CreateTmpTablePublishers ();
	       SubQueries->TablePublishers = ",tl_tmp_publishers";
	       Str_Copy (SubQueries->Publishers,
			 "tl_pubs.PublisherCod=tl_tmp_publishers.UsrCod AND ",
			 TL_MAX_BYTES_SUBQUERY);
	       break;
	    case Usr_WHO_ALL:		// Show the timeline of all users
	       SubQueries->TablePublishers = "";
	       SubQueries->Publishers[0] = '\0';
	       break;
	    default:
	       Lay_WrongWhoExit ();
	       break;
	   }
	 break;
     }
  }

/*****************************************************************************/
/********* Create subquery to get only notes not present in timeline *********/
/*****************************************************************************/

static void TL_CreateSubQueryAlreadyExists (const struct TL_Timeline *Timeline,
                                            struct TL_SubQueries *SubQueries)
  {
   switch (Timeline->WhatToGet)
     {
      case TL_GET_RECENT_TIMELINE:
      case TL_GET_ONLY_NEW_PUBS:
	 Str_Copy (SubQueries->AlreadyExists,
		   " tl_pubs.NotCod NOT IN"
		   " (SELECT NotCod FROM tl_tmp_just_retrieved_notes)",	// Avoid notes just retrieved
		   TL_MAX_BYTES_SUBQUERY);
         break;
      case TL_GET_ONLY_OLD_PUBS:
	 Str_Copy (SubQueries->AlreadyExists,
		   " tl_pubs.NotCod NOT IN"
		   " (SELECT NotCod FROM tl_tmp_visible_timeline)",	// Avoid notes already shown
		   TL_MAX_BYTES_SUBQUERY);
	 break;
     }
  }

/*****************************************************************************/
/***** Create subqueries with range of publications to get from tl_pubs ******/
/*****************************************************************************/

static void TL_CreateSubQueryRangeBottom (const struct TL_RangePubsToGet *RangePubsToGet,
                                          struct TL_SubQueries *SubQueries)
  {
   if (RangePubsToGet->Bottom > 0)
      sprintf (SubQueries->RangeBottom,"tl_pubs.PubCod>%ld AND ",
	       RangePubsToGet->Bottom);
   else
      SubQueries->RangeBottom[0] = '\0';
  }

static void TL_CreateSubQueryRangeTop (const struct TL_RangePubsToGet *RangePubsToGet,
                                       struct TL_SubQueries *SubQueries)
  {
   if (RangePubsToGet->Top > 0)
      sprintf (SubQueries->RangeTop,"tl_pubs.PubCod<%ld AND ",
	       RangePubsToGet->Top);
   else
      SubQueries->RangeTop[0] = '\0';
  }

/*****************************************************************************/
/******** Allocate/free list to store publications/notes in timeline *********/
/*****************************************************************************/

static void TL_AllocateListPubs (struct TL_Timeline *Timeline,
                                 unsigned MaxPubsToGet)
  {
   /***** Reset number of publications in list *****/
   Timeline->Pubs.Num = 0;

   /***** Allocate memory to store publications *****/
   if ((Timeline->Pubs.Lst = (struct TL_Publication *) malloc (MaxPubsToGet *
                                                       sizeof (struct TL_Publication))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for list of publications.");
  }

static void TL_FreeListPubs (const struct TL_Timeline *Timeline)
  {
   /***** Free memory used for publications *****/
   free (Timeline->Pubs.Lst);
  }

/*****************************************************************************/
/************** Select the most recent publication from tl_pubs **************/
/*****************************************************************************/

static void TL_SelectTheMostRecentPub (const struct TL_SubQueries *SubQueries,
				       struct TL_Publication *Pub)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPubs = 0;	// Initialized to avoid warning

   NumPubs =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get publication",
			      "SELECT tl_pubs.PubCod,"		// row[0]
			             "tl_pubs.NotCod,"		// row[1]
			             "tl_pubs.PublisherCod,"	// row[2]
			             "tl_pubs.PubType"		// row[3]
			      " FROM tl_pubs%s"
			      " WHERE %s%s%s%s"
			      " ORDER BY tl_pubs.PubCod DESC LIMIT 1",
			      SubQueries->TablePublishers,
			      SubQueries->RangeBottom,
			      SubQueries->RangeTop,
			      SubQueries->Publishers,
			      SubQueries->AlreadyExists);

   if (NumPubs == 1)
      /* Get data of publication */
      TL_GetDataOfPublicationFromNextRow (mysql_res,Pub);
   else
      /* Reset data of publication */
      TL_ResetPublication (Pub);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**** Insert note in temporary tables used to not get notes already shown ****/
/*****************************************************************************/

static void TL_InsertNoteInJustRetrievedNotes (long NotCod)
  {
   /* Insert note in temporary table with just retrieved notes.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tl_tmp_just_retrieved_notes"
		   " SET NotCod=%ld",
		   NotCod);
  }

static void TL_InsertNoteInVisibleTimeline (long NotCod)
  {
   /* Insert note in temporary table with visible timeline.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tl_tmp_visible_timeline"
		   " SET NotCod=%ld",
		   NotCod);
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
static void TL_ShowTimeline (struct TL_Timeline *Timeline,
                             const char *Title,long NotCodToHighlight)
  {
   extern const char *Hlp_START_Timeline;
   unsigned long NumPub;
   struct TL_Note Not;
   bool GlobalTimeline = (Gbl.Usrs.Other.UsrDat.UsrCod <= 0);
   bool ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Title,
                 TL_PutIconsTimeline,NULL,
                 Hlp_START_Timeline,Box_NOT_CLOSABLE);

   /***** Put form to select users whom public activity is displayed *****/
   if (GlobalTimeline)
      TL_PutFormWho (Timeline);

   /***** Form to write a new post *****/
   if (GlobalTimeline || ItsMe)
      TL_PutFormToWriteNewPost (Timeline);

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
   for (NumPub = 0;
	NumPub < Timeline->Pubs.Num;
	NumPub++)
     {
      /* Get data of note */
      Not.NotCod = Timeline->Pubs.Lst[NumPub].NotCod;
      TL_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_WriteNote (Timeline,&Not,
                    Timeline->Pubs.Lst[NumPub].TopMessage,
                    Timeline->Pubs.Lst[NumPub].PublisherCod,
		    Not.NotCod == NotCodToHighlight ? TL_HIGHLIGHT_NOTE :
			                                 TL_DONT_HIGHLIGHT_NOTE,
		    TL_DONT_SHOW_NOTE_ALONE);
     }
   HTM_UL_End ();

   if (Timeline->Pubs.Num == TL_MAX_REC_PUBS_TO_GET_AND_SHOW)
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

static void TL_PutIconsTimeline (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_TIMELINE);
  }

/*****************************************************************************/
/***************** Start a form in global or user timeline *******************/
/*****************************************************************************/

static void TL_FormStart (const struct TL_Timeline *Timeline,
	                  Act_Action_t ActionGbl,
                          Act_Action_t ActionUsr)
  {
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Frm_StartFormAnchor (ActionUsr,"timeline");
      Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
     }
   else
     {
      Frm_StartForm (ActionGbl);
      Usr_PutHiddenParamWho (Timeline->Who);
     }
  }

/*****************************************************************************/
/******** Show form to select users whom public activity is displayed ********/
/*****************************************************************************/

static void TL_PutFormWho (struct TL_Timeline *Timeline)
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
			Who == Timeline->Who ? "PREF_ON" :
					       "PREF_OFF");
	 Frm_StartForm (ActSeeTmlGbl);
	 Par_PutHiddenParamUnsigned (NULL,"Who",(unsigned) Who);
	 Usr_PutWhoIcon (Who);
	 Frm_EndForm ();
	 HTM_DIV_End ();
	}
   Set_EndOneSettingSelector ();
   Set_EndSettingsHead ();

   /***** Show warning if I do not follow anyone *****/
   if (Timeline->Who == Usr_WHO_FOLLOWED)
      TL_ShowWarningYouDontFollowAnyUser ();
  }

/*****************************************************************************/
/********* Get parameter with which users to view in global timeline *********/
/*****************************************************************************/

void TL_GetParamWho (void)
  {
   Usr_Who_t Who;

   /***** Get which users I want to see *****/
   Who = Usr_GetHiddenParamWho ();

   /***** If parameter Who is not present, get it from database *****/
   if (Who == Usr_WHO_UNKNOWN)
      Who = TL_GetWhoFromDB ();

   /***** If parameter Who is unknown, set it to default *****/
   if (Who == Usr_WHO_UNKNOWN)
      Who = TL_DEFAULT_WHO;

   /***** Set global variable *****/
   Set_GlobalWho (Who);
  }

/*****************************************************************************/
/********* Get which users to view in global timeline from database **********/
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
/******** Save which users to view in global timeline into database **********/
/*****************************************************************************/

static void TL_SaveWhoInDB (struct TL_Timeline *Timeline)
  {
   if (Gbl.Usrs.Me.Logged)
     {
      if (Timeline->Who == Usr_WHO_UNKNOWN)
	 Timeline->Who = TL_DEFAULT_WHO;

      /***** Update which users in database *****/
      // Who is stored in usr_last for next time I log in
      DB_QueryUPDATE ("can not update timeline users in user's last data",
		      "UPDATE usr_last SET TimelineUsrs=%u"
		      " WHERE UsrCod=%ld",
		      (unsigned) Timeline->Who,
		      Gbl.Usrs.Me.UsrDat.UsrCod);
     }
  }

/*****************************************************************************/
/**** Set/get global variable with which users to view in global timeline ****/
/*****************************************************************************/

static void Set_GlobalWho (Usr_Who_t Who)
  {
   TL_GlobalWho = Who;
  }

Usr_Who_t TL_GetGlobalWho (void)
  {
   return TL_GlobalWho;
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

static void TL_InsertNewPubsInTimeline (struct TL_Timeline *Timeline)
  {
   unsigned long NumPub;
   struct TL_Note Not;

   /***** List new publications timeline *****/
   for (NumPub = 0;
	NumPub < Timeline->Pubs.Num;
	NumPub++)
     {
      /* Get data of note */
      Not.NotCod = Timeline->Pubs.Lst[NumPub].NotCod;
      TL_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_WriteNote (Timeline,&Not,
                    Timeline->Pubs.Lst[NumPub].TopMessage,
                    Timeline->Pubs.Lst[NumPub].PublisherCod,
                    TL_DONT_HIGHLIGHT_NOTE,
                    TL_DONT_SHOW_NOTE_ALONE);
     }
  }

/*****************************************************************************/
/********************* Show old publications in timeline *********************/
/*****************************************************************************/
// The publications are inserted as list elements of a hidden list

static void TL_ShowOldPubsInTimeline (struct TL_Timeline *Timeline)
  {
   unsigned long NumPub;
   struct TL_Note Not;

   /***** List old publications in timeline *****/
   for (NumPub = 0;
	NumPub < Timeline->Pubs.Num;
	NumPub++)
     {
      /* Get data of note */
      Not.NotCod = Timeline->Pubs.Lst[NumPub].NotCod;
      TL_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_WriteNote (Timeline,&Not,
                    Timeline->Pubs.Lst[NumPub].TopMessage,
                    Timeline->Pubs.Lst[NumPub].PublisherCod,
                    TL_DONT_HIGHLIGHT_NOTE,
                    TL_DONT_SHOW_NOTE_ALONE);
     }
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

static void TL_WriteNote (struct TL_Timeline *Timeline,
	                  const struct TL_Note *Not,
                          TL_TopMessage_t TopMessage,
                          long PublisherCod,	// Who did the action (publication, commenting, faving, sharing, mentioning)
                          TL_HighlightNote_t HighlightNote,	// Highlight note
                          TL_ShowNoteAlone_t ShowNoteAlone)	// Note is shown alone, not in a list
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   struct UsrData AuthorDat;
   bool IAmTheAuthor;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];
   unsigned NumComments;
   char IdNewComment[Frm_MAX_BYTES_ID + 1];
   static unsigned NumDiv = 0;	// Used to create unique div id for fav and shared

   NumDiv++;

   /***** Begin box ****/
   if (ShowNoteAlone == TL_SHOW_NOTE_ALONE)
     {
      Box_BoxBegin (NULL,NULL,
                    NULL,NULL,
                    NULL,Box_CLOSABLE);
      HTM_UL_Begin ("class=\"TL_LIST\"");
     }

   /***** Start list item *****/
   HTM_LI_Begin ("class=\"%s\"",
		 ShowNoteAlone == TL_SHOW_NOTE_ALONE ?
		    (HighlightNote == TL_HIGHLIGHT_NOTE ? "TL_WIDTH TL_NEW_PUB" :
					                  "TL_WIDTH") :
		    (HighlightNote == TL_HIGHLIGHT_NOTE ? "TL_WIDTH TL_SEP TL_NEW_PUB" :
					                  "TL_WIDTH TL_SEP"));

   if (Not->NotCod   <= 0 ||
       Not->NoteType == TL_NOTE_UNKNOWN ||
       Not->UsrCod   <= 0)
      Ale_ShowAlert (Ale_ERROR,"Error in note.");
   else
     {
      /***** Initialize location in hierarchy *****/
      Ins.InsCod = -1L;
      Ctr.CtrCod = -1L;
      Deg.DegCod = -1L;
      Crs.CrsCod = -1L;

      /***** Write sharer/commenter if distinct to author *****/
      TL_WriteTopMessage (TopMessage,PublisherCod);

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&AuthorDat);

      /***** Get author data *****/
      AuthorDat.UsrCod = Not->UsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&AuthorDat,Usr_DONT_GET_PREFS);
      IAmTheAuthor = Usr_ItsMe (AuthorDat.UsrCod);

      /***** Left: write author's photo *****/
      HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&AuthorDat,PhotoURL);
      Pho_ShowUsrPhoto (&AuthorDat,ShowPhoto ? PhotoURL :
					       NULL,
			"PHOTO45x60",Pho_ZOOM,true);	// Use unique id
      HTM_DIV_End ();

      /***** Right: author's name, time, summary and buttons *****/
      /* Begin right container */
      HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");

      /* Write author's full name and date-time */
      TL_WriteAuthorNote (&AuthorDat);
      TL_WriteDateTime (Not->DateTimeUTC);

      /* Write content of the note */
      if (Not->NoteType == TL_NOTE_POST)
	 /* Write post content */
	 TL_GetAndWritePost (Not->Cod);
      else
	{
	 /* Reset forums */
         For_ResetForums (&Forums);

	 /* Get location in hierarchy */
	 if (!Not->Unavailable)
	    switch (Not->NoteType)
	      {
	       case TL_NOTE_INS_DOC_PUB_FILE:
	       case TL_NOTE_INS_SHA_PUB_FILE:
		  /* Get institution data */
		  Ins.InsCod = Not->HieCod;
		  Ins_GetDataOfInstitutionByCod (&Ins);
		  break;
	       case TL_NOTE_CTR_DOC_PUB_FILE:
	       case TL_NOTE_CTR_SHA_PUB_FILE:
		  /* Get centre data */
		  Ctr.CtrCod = Not->HieCod;
		  Ctr_GetDataOfCentreByCod (&Ctr);
		  break;
	       case TL_NOTE_DEG_DOC_PUB_FILE:
	       case TL_NOTE_DEG_SHA_PUB_FILE:
		  /* Get degree data */
		  Deg.DegCod = Not->HieCod;
		  Deg_GetDataOfDegreeByCod (&Deg);
		  break;
	       case TL_NOTE_CRS_DOC_PUB_FILE:
	       case TL_NOTE_CRS_SHA_PUB_FILE:
	       case TL_NOTE_EXAM_ANNOUNCEMENT:
	       case TL_NOTE_NOTICE:
		  /* Get course data */
		  Crs.CrsCod = Not->HieCod;
		  Crs_GetDataOfCourseByCod (&Crs);
		  break;
	       case TL_NOTE_FORUM_POST:
		  /* Get forum type of the post */
		  For_GetForumTypeAndLocationOfAPost (Not->Cod,&Forums.Forum);
		  For_SetForumName (&Forums.Forum,ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
		  break;
	       default:
		  break;
	      }

	 /* Write note type */
	 TL_PutFormGoToAction (Not,&Forums);

	 /* Write location in hierarchy */
	 if (!Not->Unavailable)
	    switch (Not->NoteType)
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
	 TL_GetNoteSummary (Not,SummaryStr);
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
      NumComments = TL_GetNumCommentsInNote (Not->NotCod);

      /* Put icon to add a comment */
      HTM_DIV_Begin ("class=\"TL_BOTTOM_LEFT\"");
      if (Not->Unavailable)		// Unavailable notes can not be commented
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
      TL_Fav_PutFormToFavUnfNote (Not,TL_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /* Foot column 2: Share zone */
      HTM_DIV_Begin ("id=\"sha_not_%s_%u\" class=\"TL_SHA_NOT TL_SHA_NOT_WIDTH\"",
	             Gbl.UniqueNameEncrypted,NumDiv);
      TL_Sha_PutFormToShaUnsNote (Not,TL_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /* Foot column 3: Icon to remove this note */
      HTM_DIV_Begin ("class=\"TL_REM\"");
      if (IAmTheAuthor)
	 TL_PutFormToRemovePublication (Timeline,
	                                Not->NotCod);
      HTM_DIV_End ();

      /* End foot container */
      HTM_DIV_End ();

      /* Comments */
      if (NumComments)
	 TL_WriteCommentsInNote (Timeline,Not,NumComments);

      /* End container for buttons and comments */
      HTM_DIV_End ();

      /* Put hidden form to write a new comment */
      TL_PutHiddenFormToWriteNewCommentToNote (Timeline,
                                               Not->NotCod,IdNewComment);

      /***** Free memory used for author's data *****/
      Usr_UsrDataDestructor (&AuthorDat);
     }

   /***** End list item *****/
   HTM_LI_End ();

   /***** End box ****/
   if (ShowNoteAlone == TL_SHOW_NOTE_ALONE)
     {
      HTM_UL_End ();
      Box_BoxEnd ();
     }
  }

/*****************************************************************************/
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/

static void TL_WriteTopMessage (TL_TopMessage_t TopMessage,long PublisherCod)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *Txt_TIMELINE_NOTE_TOP_MESSAGES[TL_NUM_TOP_MESSAGES];
   struct UsrData PublisherDat;
   bool ItsMe = Usr_ItsMe (PublisherCod);

   if (TopMessage != TL_TOP_MESSAGE_NONE)
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&PublisherDat);

      /***** Get user's data *****/
      PublisherDat.UsrCod = PublisherCod;
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherDat,Usr_DONT_GET_PREFS))	// Really we only need EncryptedUsrCod and FullName
	{
	 HTM_DIV_Begin ("class=\"TL_TOP_CONT TL_TOP_PUBLISHER TL_WIDTH\"");

	 /***** Show user's name inside form to go to user's public profile *****/
	 Frm_StartFormUnique (ActSeeOthPubPrf);
	 Usr_PutParamUsrCodEncrypted (PublisherDat.EncryptedUsrCod);
	 HTM_BUTTON_SUBMIT_Begin (ItsMe ? Txt_My_public_profile :
					  Txt_Another_user_s_profile,
				  "BT_LINK TL_TOP_PUBLISHER",NULL);
	 HTM_Txt (PublisherDat.FullName);
	 HTM_BUTTON_End ();
	 Frm_EndForm ();

	 /***** Show action made *****/
         HTM_TxtF (" %s:",Txt_TIMELINE_NOTE_TOP_MESSAGES[TopMessage]);

         HTM_DIV_End ();
	}

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&PublisherDat);
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
   struct TL_PostContent Content;

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

static void TL_PutFormGoToAction (const struct TL_Note *Not,
                                  const struct For_Forums *Forums)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_TIMELINE_NOTE[TL_NUM_NOTE_TYPES];
   extern const char *Txt_not_available;
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

   if (Not->Unavailable ||	// File/notice... pointed by this note is unavailable
       Gbl.Form.Inside)		// Inside another form
     {
      /***** Do not put form *****/
      HTM_DIV_Begin ("class=\"TL_FORM_OFF\"");
      HTM_Txt (Txt_TIMELINE_NOTE[Not->NoteType]);
      if (Not->Unavailable)
         HTM_TxtF ("&nbsp;(%s)",Txt_not_available);
      HTM_DIV_End ();
     }
   else			// Not inside another form
     {
      HTM_DIV_Begin ("class=\"TL_FORM\"");

      /***** Begin form with parameters depending on the type of note *****/
      switch (Not->NoteType)
	{
	 case TL_NOTE_INS_DOC_PUB_FILE:
	 case TL_NOTE_INS_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Ins.InsCod)	// Not the current institution
	       Ins_PutParamInsCod (Not->HieCod);		// Go to another institution
	    break;
	 case TL_NOTE_CTR_DOC_PUB_FILE:
	 case TL_NOTE_CTR_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Ctr.CtrCod)	// Not the current centre
	       Ctr_PutParamCtrCod (Not->HieCod);		// Go to another centre
	    break;
	 case TL_NOTE_DEG_DOC_PUB_FILE:
	 case TL_NOTE_DEG_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Deg.DegCod)	// Not the current degree
	       Deg_PutParamDegCod (Not->HieCod);		// Go to another degree
	    break;
	 case TL_NOTE_CRS_DOC_PUB_FILE:
	 case TL_NOTE_CRS_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_EXAM_ANNOUNCEMENT:
            Frm_SetAnchorStr (Not->Cod,&Anchor);
	    Frm_StartFormUniqueAnchor (TL_DefaultActions[Not->NoteType],
		                       Anchor);	// Locate on this specific exam
            Frm_FreeAnchorStr (Anchor);
	    ExaAnn_PutHiddenParamExaCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_POST:	// Not applicable
	    return;
	 case TL_NOTE_FORUM_POST:
	    Frm_StartFormUnique (For_ActionsSeeFor[Forums->Forum.Type]);
	    For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                         1,	// Page of posts   = first
                                         Forums->ForumSet,
					 Forums->ThreadsOrder,
					 Forums->Forum.Location,
					 Forums->Thread.Selected,
					 -1L);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_NOTICE:
            Frm_SetAnchorStr (Not->Cod,&Anchor);
	    Frm_StartFormUniqueAnchor (TL_DefaultActions[Not->NoteType],
				       Anchor);
            Frm_FreeAnchorStr (Anchor);
	    Not_PutHiddenParamNotCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 default:			// Not applicable
	    return;
	}

      /***** Icon and link to go to action *****/
      HTM_BUTTON_SUBMIT_Begin (Txt_TIMELINE_NOTE[Not->NoteType],
			       Str_BuildStringStr ("BT_LINK %s ICO_HIGHLIGHT",
						   The_ClassFormInBoxBold[Gbl.Prefs.Theme]),
			       NULL);
      Ico_PutIcon (TL_Icons[Not->NoteType],Txt_TIMELINE_NOTE[Not->NoteType],"CONTEXT_ICO_x16");
      HTM_TxtF ("&nbsp;%s",Txt_TIMELINE_NOTE[Not->NoteType]);
      HTM_BUTTON_End ();
      Str_FreeString ();

      /***** End form *****/
      Frm_EndForm ();

      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/********************** Get note summary and content *************************/
/*****************************************************************************/

static void TL_GetNoteSummary (const struct TL_Note *Not,
                               char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1])
  {
   SummaryStr[0] = '\0';

   switch (Not->NoteType)
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
	 Brw_GetSummaryAndContentOfFile (SummaryStr,NULL,Not->Cod,false);
         break;
      case TL_NOTE_EXAM_ANNOUNCEMENT:
         ExaAnn_GetSummaryAndContentExamAnn (SummaryStr,NULL,Not->Cod,false);
         break;
      case TL_NOTE_POST:
	 // Not applicable
         break;
      case TL_NOTE_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,Not->Cod,false);
         break;
      case TL_NOTE_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,Not->Cod,false);
         break;
     }
  }

/*****************************************************************************/
/***************** Store and publish a note into database ********************/
/*****************************************************************************/

void TL_StoreAndPublishNote (TL_NoteType_t NoteType,long Cod)
  {
   struct TL_Publication Pub;

   TL_StoreAndPublishNoteInternal (NoteType,Cod,&Pub);
  }

static void TL_StoreAndPublishNoteInternal (TL_NoteType_t NoteType,long Cod,struct TL_Publication *Pub)
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
   Pub->NotCod =
   DB_QueryINSERTandReturnCode ("can not create new note",
				"INSERT INTO tl_notes"
				" (NoteType,Cod,UsrCod,HieCod,Unavailable,TimeNote)"
				" VALUES"
				" (%u,%ld,%ld,%ld,'N',NOW())",
				(unsigned) NoteType,
				Cod,Gbl.Usrs.Me.UsrDat.UsrCod,HieCod);

   /***** Publish note in timeline *****/
   Pub->PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Pub->PubType      = TL_PUB_ORIGINAL_NOTE;
   TL_PublishNoteInTimeline (Pub);
  }

/*****************************************************************************/
/************************* Mark a note as unavailable ************************/
/*****************************************************************************/

void TL_MarkNoteAsUnavailable (TL_NoteType_t NoteType,long Cod)
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
	    TL_MarkNoteAsUnavailable (NoteType,FilCod);
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
// Pub->PubCod is set by the function

void TL_PublishNoteInTimeline (struct TL_Publication *Pub)
  {
   /***** Publish note in timeline *****/
   Pub->PubCod =
   DB_QueryINSERTandReturnCode ("can not publish note",
				"INSERT INTO tl_pubs"
				" (NotCod,PublisherCod,PubType,TimePublish)"
				" VALUES"
				" (%ld,%ld,%u,NOW())",
				Pub->NotCod,
				Pub->PublisherCod,
				(unsigned) Pub->PubType);

   /***** Increment number of publications in user's figures *****/
   Prf_IncrementNumPubsUsr (Pub->PublisherCod);
  }

/*****************************************************************************/
/********************** Form to write a new publication **********************/
/*****************************************************************************/

static void TL_PutFormToWriteNewPost (struct TL_Timeline *Timeline)
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
   TL_FormStart (Timeline,ActRcvTL_PstGbl,ActRcvTL_PstUsr);
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
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Receive and store post, and
          write updated timeline after publication (user) *****/
   NotCod = TL_ReceivePost ();
   TL_ShowTimelineUsrHighlightingNot (&Timeline,NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_ReceivePostGbl (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Receive and store post *****/
   NotCod = TL_ReceivePost ();

   /***** Write updated timeline after publication (global) *****/
   TL_ShowTimelineGblHighlightingNot (&Timeline,NotCod);
  }

// Returns the code of the note just created
static long TL_ReceivePost (void)
  {
   struct TL_PostContent Content;
   long PstCod;
   struct TL_Publication Pub;

   /***** Get the content of the new post *****/
   Par_GetParAndChangeFormat ("Txt",Content.Txt,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_RIGOROUS_HTML,true);

   /***** Initialize image *****/
   Med_MediaConstructor (&Content.Media);

   /***** Get attached image (action, file and title) *****/
   Content.Media.Width   = TL_IMAGE_SAVED_MAX_WIDTH;
   Content.Media.Height  = TL_IMAGE_SAVED_MAX_HEIGHT;
   Content.Media.Quality = TL_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (-1L,-1L,-1,&Content.Media,NULL,NULL);
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
      TL_StoreAndPublishNoteInternal (TL_NOTE_POST,PstCod,&Pub);

      /***** Analyze content and store notifications about mentions *****/
      Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (Pub.PubCod,Content.Txt);
     }
   else	// Text and image are empty
      Pub.NotCod = -1L;

   /***** Free image *****/
   Med_MediaDestructor (&Content.Media);

   return Pub.NotCod;
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
   Ico_PutIcon ("comment-regular.svg",Txt_Comment,"CONTEXT_ICO_16x16");
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

static void TL_PutHiddenFormToWriteNewCommentToNote (const struct TL_Timeline *Timeline,
	                                             long NotCod,
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
   TL_FormStart (Timeline,ActRcvTL_ComGbl,ActRcvTL_ComUsr);
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

static void TL_WriteCommentsInNote (struct TL_Timeline *Timeline,
				    const struct TL_Note *Not,
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
			      Not->NotCod,(unsigned) TL_PUB_COMMENT_TO_NOTE,
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
      TL_FormToShowHiddenComments (ActShoHidTL_ComGbl,ActShoHidTL_ComUsr,
				   Not->NotCod,
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
	 TL_WriteOneCommentInList (Timeline,mysql_res);
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
   /* Begin form */
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
   HTM_BUTTON_SUBMIT_Begin (NULL,The_ClassFormLinkInBox[Gbl.Prefs.Theme],NULL);
   Ico_PutIconTextLink ("angle-up.svg",
			Str_BuildStringLong (Txt_See_the_previous_X_COMMENTS,
					     (long) NumInitialComments));
   Str_FreeString ();
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
   struct TL_Timeline Timeline;
   long NotCod;
   char IdComments[Frm_MAX_BYTES_ID + 1];
   unsigned NumInitialCommentsToGet;
   unsigned NumInitialCommentsGot;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get parameters *****/
   /* Get note code */
   NotCod = TL_GetParamNotCod ();

   /* Get identifier */
   Par_GetParToText ("IdComments",IdComments,Frm_MAX_BYTES_ID);

   /* Get number of comments to get */
   NumInitialCommentsToGet = (unsigned) Par_GetParToLong ("NumHidCom");

   /***** Write HTML inside DIV with hidden comments *****/
   NumInitialCommentsGot = TL_WriteHiddenComments (&Timeline,
                                                   NotCod,IdComments,NumInitialCommentsToGet);

   /***** Link to show the first comments *****/
   TL_LinkToShowPreviousComments (IdComments,NumInitialCommentsGot);
  }

/*****************************************************************************/
/**************************** Write hidden comments **************************/
/*****************************************************************************/
// Returns the number of comments got

static unsigned TL_WriteHiddenComments (struct TL_Timeline *Timeline,
                                        long NotCod,
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
      TL_WriteOneCommentInList (Timeline,mysql_res);
   HTM_UL_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumInitialCommentsGot;
  }

/*****************************************************************************/
/************************* Write a comment in list ***************************/
/*****************************************************************************/

static void TL_WriteOneCommentInList (struct TL_Timeline *Timeline,
                                      MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;
   struct TL_Comment Com;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   row = mysql_fetch_row (mysql_res);
   TL_GetDataOfCommentFromRow (row,&Com);

   /***** Write comment *****/
   TL_WriteComment (Timeline,&Com,
		    TL_TOP_MESSAGE_NONE,-1L,
		    false);	// Not alone

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
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

   /***** Icon and text to show only the latest comments ****/
   HTM_DIV_Begin ("id=\"exp_%s\" class=\"TL_EXPAND_COM TL_RIGHT_WIDTH\""
	          " style=\"display:none;\"",	// Hidden
		  IdComments);
   TL_PutIconToToggleComments (IdComments,"angle-up.svg",
			       Str_BuildStringLong (Txt_See_the_previous_X_COMMENTS,
						    (long) NumInitialComments));
   Str_FreeString ();
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

static void TL_WriteComment (struct TL_Timeline *Timeline,
	                     struct TL_Comment *Com,
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
      Box_BoxBegin (NULL,NULL,
                    NULL,NULL,
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

   if (Com->PubCod <= 0 ||
       Com->NotCod <= 0 ||
       Com->UsrCod <= 0)
      Ale_ShowAlert (Ale_ERROR,"Error in comment.");
   else
     {
      /***** Get author's data *****/
      Usr_UsrDataConstructor (&UsrDat);
      UsrDat.UsrCod = Com->UsrCod;
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
      TL_WriteDateTime (Com->DateTimeUTC);

      /* Write content of the comment */
      if (Com->Content.Txt[0])
	{
	 HTM_DIV_Begin ("class=\"TL_TXT\"");
	 Msg_WriteMsgContent (Com->Content.Txt,Cns_MAX_BYTES_LONG_TEXT,true,false);
	 HTM_DIV_End ();
	}

      /* Show image */
      Med_ShowMedia (&Com->Content.Media,"TL_COM_MED_CONT TL_COMM_WIDTH",
	                                    "TL_COM_MED TL_COMM_WIDTH");

      /* Start foot container */
      HTM_DIV_Begin ("class=\"TL_FOOT TL_COMM_WIDTH\"");

      /* Fav zone */
      HTM_DIV_Begin ("id=\"fav_com_%s_%u\" class=\"TL_FAV_COM TL_FAV_WIDTH\"",
	             Gbl.UniqueNameEncrypted,NumDiv);
      TL_Fav_PutFormToFavUnfComment (Com,TL_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /* Put icon to remove this comment */
      HTM_DIV_Begin ("class=\"TL_REM\"");
      if (IAmTheAuthor && !ShowCommentAlone)
	 TL_PutFormToRemoveComment (Timeline,Com->PubCod);
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

static void TL_PutFormToRemoveComment (const struct TL_Timeline *Timeline,
	                               long PubCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   TL_FormStart (Timeline,ActReqRemTL_ComGbl,ActReqRemTL_ComUsr);
   TL_PutHiddenParamPubCod (PubCod);
   Ico_PutIconLink ("trash.svg",Txt_Remove);
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************ Form to remove publication *************************/
/*****************************************************************************/

static void TL_PutFormToRemovePublication (const struct TL_Timeline *Timeline,
                                           long NotCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   TL_FormStart (Timeline,ActReqRemTL_PubGbl,ActReqRemTL_PubUsr);
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

long TL_GetParamNotCod (void)
  {
   /***** Get note code *****/
   return Par_GetParToLong ("NotCod");
  }

/*****************************************************************************/
/**************** Get parameter with the code of a publication ***************/
/*****************************************************************************/

long TL_GetParamPubCod (void)
  {
   /***** Get comment code *****/
   return Par_GetParToLong ("PubCod");
  }

/*****************************************************************************/
/******************************* Comment a note ******************************/
/*****************************************************************************/

void TL_ReceiveCommentUsr (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Receive comment in a note
          and write updated timeline after commenting (user) *****/
   NotCod = TL_ReceiveComment ();
   TL_ShowTimelineUsrHighlightingNot (&Timeline,NotCod);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_ReceiveCommentGbl (void)
  {
   struct TL_Timeline Timeline;
   long NotCod;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Receive comment in a note *****/
   NotCod = TL_ReceiveComment ();

   /***** Write updated timeline after commenting (global) *****/
   TL_ShowTimelineGblHighlightingNot (&Timeline,NotCod);
  }

static long TL_ReceiveComment (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   struct TL_PostContent Content;
   struct TL_Note Not;
   struct TL_Publication Pub;

   /***** Get data of note *****/
   Not.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&Not);

   if (Not.NotCod > 0)
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
      Med_GetMediaFromForm (-1L,-1L,-1,&Content.Media,NULL,NULL);
      Ale_ShowAlerts (NULL);

      if (Content.Txt[0] ||			// Text not empty
	 Content.Media.Status == Med_PROCESSED)	// A media is attached
	{
	 /***** Store media in filesystem and database *****/
	 Med_RemoveKeepOrStoreMedia (-1L,&Content.Media);

	 /***** Publish *****/
	 /* Insert into publications */
	 Pub.NotCod       = Not.NotCod;
	 Pub.PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
	 Pub.PubType      = TL_PUB_COMMENT_TO_NOTE;
	 TL_PublishNoteInTimeline (&Pub);	// Set Pub.PubCod

	 /* Insert comment content in the database */
	 DB_QueryINSERT ("can not store comment content",
			 "INSERT INTO tl_comments"
	                 " (PubCod,Txt,MedCod)"
			 " VALUES"
			 " (%ld,'%s',%ld)",
			 Pub.PubCod,
			 Content.Txt,
			 Content.Media.MedCod);

	 /***** Store notifications about the new comment *****/
	 Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_TIMELINE_COMMENT,Pub.PubCod);

	 /***** Analyze content and store notifications about mentions *****/
	 Str_AnalyzeTxtAndStoreNotifyEventToMentionedUsrs (Pub.PubCod,Content.Txt);
	}

      /***** Free image *****/
      Med_MediaDestructor (&Content.Media);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);

   return Not.NotCod;
  }

/*****************************************************************************/
/*********** Create a notification for the author of a post/comment **********/
/*****************************************************************************/

void TL_CreateNotifToAuthor (long AuthorCod,long PubCod,
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
									0),
					Gbl.Hierarchy.Ins.InsCod,
					Gbl.Hierarchy.Ctr.CtrCod,
					Gbl.Hierarchy.Deg.DegCod,
					Gbl.Hierarchy.Crs.CrsCod);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*********************** Request the removal of a note ***********************/
/*****************************************************************************/

void TL_RequestRemNoteUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Request the removal of note *****/
   TL_RequestRemovalNote (&Timeline);

   /***** Write timeline again (user) *****/
   TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RequestRemNoteGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Request the removal of note *****/
   TL_RequestRemovalNote (&Timeline);

   /***** Write timeline again (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_RequestRemovalNote (struct TL_Timeline *Timeline)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   extern const char *Txt_Remove;
   struct TL_Note Not;
   bool ItsMe;

   /***** Get data of note *****/
   Not.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&Not);

   if (Not.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (Not.UsrCod);
      if (ItsMe)	// I am the author of this note
	{
	 /***** Show question and button to remove note *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_post);

	 /* Show note */
	 TL_WriteNote (Timeline,&Not,
		       TL_TOP_MESSAGE_NONE,-1L,
		       TL_DONT_HIGHLIGHT_NOTE,
		       TL_SHOW_NOTE_ALONE);

	 /* End alert */
	 Timeline->NotCod = Not.NotCod;	// Note to be removed
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	    Ale_ShowAlertAndButton2 (ActRemTL_PubUsr,"timeline",NULL,
	                             TL_PutParamsRemoveNote,Timeline,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlertAndButton2 (ActRemTL_PubGbl,NULL,NULL,
	                             TL_PutParamsRemoveNote,Timeline,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/********************* Put parameters to remove a note ***********************/
/*****************************************************************************/

static void TL_PutParamsRemoveNote (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
      else
	 Usr_PutHiddenParamWho (((struct TL_Timeline *) Timeline)->Who);
      TL_PutHiddenParamNotCod (((struct TL_Timeline *) Timeline)->NotCod);
     }
  }

/*****************************************************************************/
/******************************* Remove a note *******************************/
/*****************************************************************************/

void TL_RemoveNoteUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Remove a note *****/
   TL_RemoveNote ();

   /***** Write updated timeline after removing (user) *****/
   TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RemoveNoteGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Remove a note *****/
   TL_RemoveNote ();

   /***** Write updated timeline after removing (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_RemoveNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_TIMELINE_Post_removed;
   struct TL_Note Not;
   bool ItsMe;

   /***** Get data of note *****/
   Not.NotCod = TL_GetParamNotCod ();
   TL_GetDataOfNoteByCod (&Not);

   if (Not.NotCod > 0)
     {
      ItsMe = Usr_ItsMe (Not.UsrCod);
      if (ItsMe)	// I am the author of this note
	{
	 /***** Delete note from database *****/
	 TL_RemoveNoteMediaAndDBEntries (&Not);

	 /***** Reset note *****/
	 TL_ResetNote (&Not);

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

static void TL_RemoveNoteMediaAndDBEntries (struct TL_Note *Not)
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
				 Not->NotCod,
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
   if (Not->NoteType == TL_NOTE_POST)
     {
      /* Remove media associated to a post from database */
      if (DB_QuerySELECT (&mysql_res,"can not get media",
				 "SELECT MedCod"	// row[0]
				 " FROM tl_posts"
				 " WHERE PstCod=%ld",
				 Not->Cod) == 1)   // Result should have a unique row
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
   PubCod = TL_GetPubCodOfOriginalNote (Not->NotCod);
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
		   Not->NotCod);

   /***** Remove all the publications of this note *****/
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tl_pubs"
		   " WHERE NotCod=%ld",
		   Not->NotCod);

   /***** Remove note *****/
   DB_QueryDELETE ("can not remove a note",
		   "DELETE FROM tl_notes"
	           " WHERE NotCod=%ld"
	           " AND UsrCod=%ld",		// Extra check: I am the author
		   Not->NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);

   if (Not->NoteType == TL_NOTE_POST)
      /***** Remove post *****/
      DB_QueryDELETE ("can not remove a post",
		      "DELETE FROM tl_posts"
		      " WHERE PstCod=%ld",
		      Not->Cod);
  }

/*****************************************************************************/
/*********************** Get code of note of a publication *******************/
/*****************************************************************************/

static long TL_GetNotCodFromPubCod (long PubCod)
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

long TL_GetPubCodOfOriginalNote (long NotCod)
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
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Request the removal of comment in note *****/
   TL_RequestRemovalComment (&Timeline);

   /***** Write timeline again (user) *****/
   TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RequestRemComGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Request the removal of comment in note *****/
   TL_RequestRemovalComment (&Timeline);

   /***** Write timeline again (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_RequestRemovalComment (struct TL_Timeline *Timeline)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_comment;
   extern const char *Txt_Remove;
   struct TL_Comment Com;
   bool ItsMe;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (&Com);

   if (Com.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (Com.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Show question and button to remove comment *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_following_comment);

	 /* Show comment */
	 TL_WriteComment (Timeline,&Com,
			  TL_TOP_MESSAGE_NONE,-1L,
			  true);	// Alone

	 /* End alert */
	 Timeline->PubCod = Com.PubCod;	// Publication to be removed
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	    Ale_ShowAlertAndButton2 (ActRemTL_ComUsr,"timeline",NULL,
	                             TL_PutParamsRemoveComment,Timeline,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlertAndButton2 (ActRemTL_ComGbl,NULL,NULL,
	                             TL_PutParamsRemoveComment,Timeline,
				     Btn_REMOVE_BUTTON,Txt_Remove);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/******************** Put parameters to remove a comment *********************/
/*****************************************************************************/

static void TL_PutParamsRemoveComment (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
      else
	 Usr_PutHiddenParamWho (((struct TL_Timeline *) Timeline)->Who);
      TL_PutHiddenParamPubCod (((struct TL_Timeline *) Timeline)->PubCod);
     }
  }

/*****************************************************************************/
/***************************** Remove a comment ******************************/
/*****************************************************************************/

void TL_RemoveComUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Remove a comment *****/
   TL_RemoveComment ();

   /***** Write updated timeline after removing (user) *****/
   TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_RemoveComGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Remove a comment *****/
   TL_RemoveComment ();

   /***** Write updated timeline after removing (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_RemoveComment (void)
  {
   extern const char *Txt_The_comment_no_longer_exists;
   extern const char *Txt_Comment_removed;
   struct TL_Comment Com;
   bool ItsMe;

   /***** Initialize image *****/
   Med_MediaConstructor (&Com.Content.Media);

   /***** Get data of comment *****/
   Com.PubCod = TL_GetParamPubCod ();
   TL_GetDataOfCommByCod (&Com);

   if (Com.PubCod > 0)
     {
      ItsMe = Usr_ItsMe (Com.UsrCod);
      if (ItsMe)	// I am the author of this comment
	{
	 /***** Remove media associated to comment
	        and delete comment from database *****/
	 TL_RemoveCommentMediaAndDBEntries (Com.PubCod);

	 /***** Reset fields of comment *****/
	 TL_ResetComment (&Com);

	 /***** Message of success *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Comment_removed);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_comment_no_longer_exists);

   /***** Free image *****/
   Med_MediaDestructor (&Com.Content.Media);
  }

/*****************************************************************************/
/*************** Remove comment media and database entries *******************/
/*****************************************************************************/

static void TL_RemoveCommentMediaAndDBEntries (long PubCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long MedCod;

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
/************************ Show sharers or favouriters ************************/
/*****************************************************************************/

void TL_ShowNumSharersOrFavers (unsigned NumUsrs)
  {
   /***** Show number of sharers or favers
          (users who have shared or marked this note as favourite) *****/
   HTM_TxtF ("&nbsp;%u",NumUsrs);
  }

void TL_ShowSharersOrFavers (MYSQL_RES **mysql_res,
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
/********************* Form to show all favers/sharers ***********************/
/*****************************************************************************/

void TL_PutFormToSeeAllFaversSharers (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		                      const char *ParamFormat,long ParamCod,
                                      TL_HowManyUsrs_t HowManyUsrs)
  {
   extern const char *Txt_View_all_USERS;

   switch (HowManyUsrs)
     {
      case TL_SHOW_FEW_USRS:
	 /***** Form and icon to mark note as favourite *****/
	 TL_FormFavSha (ActionGbl,ActionUsr,
	                ParamFormat,ParamCod,
			TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
      case TL_SHOW_ALL_USRS:
         Ico_PutIconOff (TL_ICON_ELLIPSIS,Txt_View_all_USERS);
	 break;
     }
  }

/*****************************************************************************/
/******* Form to fav/unfav or share/unshare in global or user timeline *******/
/*****************************************************************************/

void TL_FormFavSha (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		    const char *ParamFormat,long ParamCod,
		    const char *Icon,const char *Title)
  {
   char *OnSubmit;
   char ParamStr[7 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Create parameter string *****/
   sprintf (ParamStr,ParamFormat,ParamCod);

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
		    ParamStr,
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
		    ParamStr) < 0)
	 Lay_NotEnoughMemoryExit ();
      Frm_StartFormUniqueAnchorOnSubmit (ActUnk,NULL,OnSubmit);
     }
   Ico_PutIconLink (Icon,Title);
   Frm_EndForm ();

   /* Free allocated memory */
   free (OnSubmit);
  }

/*****************************************************************************/
/******************** Get data of note using its code ************************/
/*****************************************************************************/

void TL_GetDataOfNoteByCod (struct TL_Note *Not)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (Not->NotCod > 0)
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
			  Not->NotCod))
	{
	 /***** Get data of note *****/
	 row = mysql_fetch_row (mysql_res);
	 TL_GetDataOfNoteFromRow (row,Not);
	}
      else
	 /***** Reset fields of note *****/
	 TL_ResetNote (Not);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Reset fields of note *****/
      TL_ResetNote (Not);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/

void TL_GetDataOfCommByCod (struct TL_Comment *Com)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (Com->PubCod > 0)
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
			  Com->PubCod,(unsigned) TL_PUB_COMMENT_TO_NOTE))
	{
	 /***** Get data of comment *****/
	 row = mysql_fetch_row (mysql_res);
	 TL_GetDataOfCommentFromRow (row,Com);
	}
      else
	 /***** Reset fields of comment *****/
	 TL_ResetComment (Com);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Reset fields of comment *****/
      TL_ResetComment (Com);
  }

/*****************************************************************************/
/***************** Get data of publication using its code ********************/
/*****************************************************************************/

static void TL_GetDataOfPublicationFromNextRow (MYSQL_RES *mysql_res,
                                                struct TL_Publication *Pub)
  {
   static const TL_TopMessage_t TopMessages[TL_NUM_PUB_TYPES] =
     {
      [TL_PUB_UNKNOWN        ] = TL_TOP_MESSAGE_NONE,
      [TL_PUB_ORIGINAL_NOTE  ] = TL_TOP_MESSAGE_NONE,
      [TL_PUB_SHARED_NOTE    ] = TL_TOP_MESSAGE_SHARED,
      [TL_PUB_COMMENT_TO_NOTE] = TL_TOP_MESSAGE_COMMENTED,
     };
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]: PubCod
   row[1]: NotCod
   row[2]: PublisherCod
   row[3]: PubType
   */

   /***** Get code of publication (row[0]) *****/
   Pub->PubCod       = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get note code (row[1]) *****/
   Pub->NotCod       = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get publisher's code (row[2]) *****/
   Pub->PublisherCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get type of publication (row[3]) *****/
   Pub->PubType      = TL_GetPubTypeFromStr ((const char *) row[3]);
   Pub->TopMessage   = TopMessages[Pub->PubType];
  }

/*****************************************************************************/
/************************ Get data of note from row **************************/
/*****************************************************************************/

static void TL_GetDataOfNoteFromRow (MYSQL_ROW row,struct TL_Note *Not)
  {
   /*
   row[0]: NotCod
   row[1]: NoteType
   row[2]: Cod
   row[3]: UsrCod
   row[4]: HieCod
   row[5]: Unavailable
   row[5]: UNIX_TIMESTAMP(TimeNote)
   */
   /***** Get code (row[0]) *****/
   Not->NotCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get note type (row[1]) *****/
   Not->NoteType    = TL_GetNoteTypeFromStr ((const char *) row[1]);

   /***** Get file/post... code (row[2]) *****/
   Not->Cod         = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get (from) user code (row[3]) *****/
   Not->UsrCod      = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get hierarchy code (row[4]) *****/
   Not->HieCod      = Str_ConvertStrCodToLongCod (row[4]);

   /***** File/post... unavailable (row[5]) *****/
   Not->Unavailable = (row[5][0] == 'Y');

   /***** Get time of the note (row[6]) *****/
   Not->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[6]);

   /***** Get number of times this note has been shared *****/
   TL_Sha_UpdateNumTimesANoteHasBeenShared (Not);

   /***** Get number of times this note has been favourited *****/
   TL_Fav_GetNumTimesANoteHasBeenFav (Not);
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

static void TL_GetDataOfCommentFromRow (MYSQL_ROW row,struct TL_Comment *Com)
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
   Com->PubCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get (from) user code (row[1]) *****/
   Com->UsrCod      = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get code of note (row[2]) *****/
   Com->NotCod      = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get time of the note (row[3]) *****/
   Com->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[3]);

   /***** Get text content (row[4]) *****/
   Str_Copy (Com->Content.Txt,row[4],
             Cns_MAX_BYTES_LONG_TEXT);

   /***** Get number of times this comment has been favourited *****/
   TL_Fav_GetNumTimesACommHasBeenFav (Com);

   /***** Get media content (row[5]) *****/
   Com->Content.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
   Med_GetMediaDataByCod (&Com->Content.Media);
  }

/*****************************************************************************/
/************************ Reset fields of publication ************************/
/*****************************************************************************/

static void TL_ResetPublication (struct TL_Publication *Pub)
  {
   Pub->PubCod       = -1L;
   Pub->NotCod       = -1L;
   Pub->PublisherCod = -1L;
   Pub->PubType      = TL_PUB_UNKNOWN;
   Pub->TopMessage   = TL_TOP_MESSAGE_NONE;
  }

/*****************************************************************************/
/*************************** Reset fields of note ****************************/
/*****************************************************************************/

static void TL_ResetNote (struct TL_Note *Not)
  {
   Not->NotCod      = -1L;
   Not->NoteType    = TL_NOTE_UNKNOWN;
   Not->UsrCod      = -1L;
   Not->HieCod      = -1L;
   Not->Cod         = -1L;
   Not->Unavailable = false;
   Not->DateTimeUTC = (time_t) 0;
   Not->NumShared   = 0;
  }

/*****************************************************************************/
/************************** Reset fields of comment **************************/
/*****************************************************************************/

static void TL_ResetComment (struct TL_Comment *Com)
  {
   Com->PubCod      = -1L;
   Com->UsrCod      = -1L;
   Com->NotCod      = -1L;
   Com->DateTimeUTC = (time_t) 0;
   Com->Content.Txt[0]  = '\0';
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
   /* tl_timelines contains the distinct notes in timeline of each open session:
mysql> SELECT SessionId,COUNT(*) FROM tl_timelines GROUP BY SessionId;
+---------------------------------------------+----------+
| SessionId                                   | COUNT(*) |
+---------------------------------------------+----------+
| u-X-R3gKki7eKMXrNCP8bGhwOAZuVngRy7FNGZFMKzI |       52 | --> 52 distinct notes
| u1CoqL1YWl3_hR4wk4bI7vhnc-uRcCmIDyKYAgBB6kk |       10 |
| u8xqamzkorHfY4BvYRMXjNhzHvQyigZUZemO0YiMn48 |       10 |
| u_n2V_L3KrFjnd4SqZk0gxMFwZHRuWZ8_EIVTU9sdpI |       10 |
| V6pGe1kGGS_uO5i__waqXKnuDkPYaDZHNAYr-Zv-GJQ |        2 |
| vqDRz-iiM8v10Dl8ThwqIqmDRIklz8szJaqflwXZucs |       10 |
| w11juqKPx6lg-f_pL2ZBYqlagU1mEepSvvk9L3gDGac |       10 | --> 10 distinct notes
| wLg4e8KQljCcVuFWIkJjNeti89kAiwOZ3iyXdzm_eDk |       10 |
| wnU85YrwJHhZGWIZhd7LQfQTPrclIWHfMF3DcB-Rcgw |        4 |
| wRzRJFnHfzW61fZYnvMIaMRlkuWUeEyqXVQ6JeWA32k |       11 |
+---------------------------------------------+----------+
10 rows in set (0,01 sec)
   */
   DB_QueryINSERT ("can not insert notes in timeline",
		   "INSERT IGNORE INTO tl_timelines"
	           " (SessionId,NotCod)"
	           " SELECT '%s',NotCod FROM tl_tmp_just_retrieved_notes",
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
   struct TL_Publication Pub;
   struct TL_Note Not;
   struct TL_PostContent Content;
   size_t Length;
   bool ContentCopied = false;

   /***** Return nothing on error *****/
   Pub.PubType = TL_PUB_UNKNOWN;
   SummaryStr[0] = '\0';	// Return nothing on error
   Content.Txt[0] = '\0';

   /***** Get summary and content from post from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get data of publication",
		       "SELECT PubCod,"			// row[0]
			      "NotCod,"			// row[1]
			      "PublisherCod,"		// row[2]
			      "PubType"			// row[3]
		       " FROM tl_pubs WHERE PubCod=%ld",
		       PubCod) == 1)   // Result should have a unique row
      /* Get data of publication from row */
      TL_GetDataOfPublicationFromNextRow (mysql_res,&Pub);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get summary and content *****/
   switch (Pub.PubType)
     {
      case TL_PUB_UNKNOWN:
	 break;
      case TL_PUB_ORIGINAL_NOTE:
      case TL_PUB_SHARED_NOTE:
	 /* Get data of note */
	 Not.NotCod = Pub.NotCod;
	 TL_GetDataOfNoteByCod (&Not);

	 if (Not.NoteType == TL_NOTE_POST)
	   {
	    /***** Get content of post from database *****/
	    if (DB_QuerySELECT (&mysql_res,"can not get the content of a post",
			        "SELECT Txt"	// row[0]
			        " FROM tl_posts"
				" WHERE PstCod=%ld",
				Not.Cod) == 1)   // Result should have a unique row
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
	    TL_GetNoteSummary (&Not,SummaryStr);
	 break;
      case TL_PUB_COMMENT_TO_NOTE:
	 /***** Get content of post from database *****/
	 if (DB_QuerySELECT (&mysql_res,"can not get the content"
				        " of a comment to a note",
			     "SELECT Txt"	// row[0]
			     " FROM tl_comments"
			     " WHERE PubCod=%ld",
			     Pub.PubCod) == 1)   // Result should have a unique row
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
