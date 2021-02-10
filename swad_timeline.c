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
#include "swad_timeline_note.h"
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

#define TL_MAX_CHARS_IN_POST	1000	// Maximum number of characters in a post

#define TL_MAX_BYTES_SUBQUERY (128 - 1)

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

static void TL_ShowTimeline (struct TL_Timeline *Timeline,
                             const char *Title,long NotCodToHighlight);
static void TL_PutIconsTimeline (__attribute__((unused)) void *Args);

static void TL_PutFormWho (struct TL_Timeline *Timeline);
static Usr_Who_t TL_GetWhoFromDB (void);
static void TL_SetGlobalWho (Usr_Who_t Who);

static void TL_SaveWhoInDB (struct TL_Timeline *Timeline);

static void TL_ShowWarningYouDontFollowAnyUser (void);

static void TL_InsertNewPubsInTimeline (struct TL_Timeline *Timeline);
static void TL_ShowOldPubsInTimeline (struct TL_Timeline *Timeline);

static void TL_PutLinkToViewNewPublications (void);
static void TL_PutLinkToViewOldPublications (void);

static void TL_PutFormToWriteNewPost (struct TL_Timeline *Timeline);

static long TL_ReceivePost (void);

static long TL_Pub_GetNotCodFromPubCod (long PubCod);

static void TL_GetDataOfPublicationFromNextRow (MYSQL_RES *mysql_res,
                                                struct TL_Publication *Pub);
static TL_PubType_t TL_GetPubTypeFromStr (const char *Str);

static void TL_Pub_ResetPublication (struct TL_Publication *Pub);

static void TL_ClearTimelineThisSession (void);

/*****************************************************************************/
/************************ Initialize global timeline *************************/
/*****************************************************************************/

void TL_InitTimelineGbl (struct TL_Timeline *Timeline)
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

void TL_ShowNoteAndTimelineGbl (struct TL_Timeline *Timeline)
  {
   long PubCod;
   struct TL_Not_Note Not;

   /***** Initialize note code to -1 ==> no highlighted note *****/
   Not.NotCod = -1L;

   /***** Get parameter with the code of a publication *****/
   // This parameter is optional. It can be provided by a notification.
   // If > 0 ==> the note is shown highlighted above the timeline
   PubCod = TL_GetParamPubCod ();

   /***** If a note should be highlighted ==> get code of note from database *****/
   if (PubCod > 0)
      Not.NotCod = TL_Pub_GetNotCodFromPubCod (PubCod);

   /***** If a note should be highlighted ==> show it above the timeline *****/
   if (Not.NotCod > 0)
      /***** Show the note highlighted above the timeline *****/
      TL_Not_ShowHighlightedNote (Timeline,&Not);

   /***** Show timeline with possible highlighted note *****/
   TL_ShowTimelineGblHighlightingNot (Timeline,Not.NotCod);
  }

/*****************************************************************************/
/******************* Show global timeline highlighting a note ****************/
/*****************************************************************************/

void TL_ShowTimelineGblHighlightingNot (struct TL_Timeline *Timeline,
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

void TL_ShowTimelineUsrHighlightingNot (struct TL_Timeline *Timeline,
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
      TL_Not_InsertNoteInJustRetrievedNotes (Timeline->Pubs.Lst[NumPub].NotCod);
      if (Timeline->WhatToGet == TL_GET_ONLY_OLD_PUBS)
	 TL_Not_InsertNoteInVisibleTimeline (Timeline->Pubs.Lst[NumPub].NotCod);

      RangePubsToGet.Top = Timeline->Pubs.Lst[NumPub].PubCod;	// Narrow the range for the next iteration
     }
   Timeline->Pubs.Num = NumPub;	// Number of publications actually got

   /***** Update first (oldest) and last (more recent) publication codes
          into session for next refresh *****/
   TL_UpdateFirstLastPubCodesIntoSession (Timeline);

   /***** Add notes just retrieved to current timeline for this session *****/
   TL_Not_AddNotesJustRetrievedToTimelineThisSession ();

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
      TL_Pub_ResetPublication (Pub);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
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
   struct TL_Not_Note Not;
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
      TL_Not_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_Not_WriteNote (Timeline,&Not,
                        Timeline->Pubs.Lst[NumPub].TopMessage,
                        Timeline->Pubs.Lst[NumPub].PublisherCod,
		        Not.NotCod == NotCodToHighlight ? TL_HIGHLIGHT :
			                                  TL_DONT_HIGHLIGHT,
		        TL_DONT_SHOW_ALONE);
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

void TL_FormStart (const struct TL_Timeline *Timeline,
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
   TL_SetGlobalWho (Who);
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

static void TL_SetGlobalWho (Usr_Who_t Who)
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
   struct TL_Not_Note Not;

   /***** List new publications timeline *****/
   for (NumPub = 0;
	NumPub < Timeline->Pubs.Num;
	NumPub++)
     {
      /* Get data of note */
      Not.NotCod = Timeline->Pubs.Lst[NumPub].NotCod;
      TL_Not_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_Not_WriteNote (Timeline,&Not,
                    Timeline->Pubs.Lst[NumPub].TopMessage,
                    Timeline->Pubs.Lst[NumPub].PublisherCod,
                    TL_DONT_HIGHLIGHT,
                    TL_DONT_SHOW_ALONE);
     }
  }

/*****************************************************************************/
/********************* Show old publications in timeline *********************/
/*****************************************************************************/
// The publications are inserted as list elements of a hidden list

static void TL_ShowOldPubsInTimeline (struct TL_Timeline *Timeline)
  {
   unsigned long NumPub;
   struct TL_Not_Note Not;

   /***** List old publications in timeline *****/
   for (NumPub = 0;
	NumPub < Timeline->Pubs.Num;
	NumPub++)
     {
      /* Get data of note */
      Not.NotCod = Timeline->Pubs.Lst[NumPub].NotCod;
      TL_Not_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_Not_WriteNote (Timeline,&Not,
                    Timeline->Pubs.Lst[NumPub].TopMessage,
                    Timeline->Pubs.Lst[NumPub].PublisherCod,
                    TL_DONT_HIGHLIGHT,
                    TL_DONT_SHOW_ALONE);
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
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/

void TL_WriteTopMessage (TL_TopMessage_t TopMessage,long PublisherCod)
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
/******************* Write the date of creation of a note ********************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

void TL_WriteDateTime (time_t TimeUTC)
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

void TL_GetAndWritePost (long PstCod)
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
/********************* Publish note/comment in timeline **********************/
/*****************************************************************************/
// Pub->PubCod is set by the function

void TL_PublishPubInTimeline (struct TL_Publication *Pub)
  {
   /***** Publish note in timeline *****/
   Pub->PubCod =
   DB_QueryINSERTandReturnCode ("can not publish note/comment",
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
   TL_Not_WriteAuthorNote (&Gbl.Usrs.Me.UsrDat);

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

void TL_PutTextarea (const char *Placeholder,const char *ClassTextArea)
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
      TL_Not_StoreAndPublishNoteInternal (TL_NOTE_POST,PstCod,&Pub);

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
/*************** Put parameter with the code of a publication ****************/
/*****************************************************************************/

void TL_PutHiddenParamPubCod (long PubCod)
  {
   Par_PutHiddenParamLong (NULL,"PubCod",PubCod);
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
/*********************** Get code of note of a publication *******************/
/*****************************************************************************/

static long TL_Pub_GetNotCodFromPubCod (long PubCod)
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

long TL_Not_GetPubCodOfOriginalNote (long NotCod)
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
/************************ Reset fields of publication ************************/
/*****************************************************************************/

static void TL_Pub_ResetPublication (struct TL_Publication *Pub)
  {
   Pub->PubCod       = -1L;
   Pub->NotCod       = -1L;
   Pub->PublisherCod = -1L;
   Pub->PubType      = TL_PUB_UNKNOWN;
   Pub->TopMessage   = TL_TOP_MESSAGE_NONE;
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
/***************** Get notification of a new publication *********************/
/*****************************************************************************/

void TL_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                             char **ContentStr,
                             long PubCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct TL_Publication Pub;
   struct TL_Not_Note Not;
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
	 TL_Not_GetDataOfNoteByCod (&Not);

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
	    TL_Not_GetNoteSummary (&Not,SummaryStr);
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
