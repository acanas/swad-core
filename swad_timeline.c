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
#include "swad_timeline_publication.h"
#include "swad_timeline_share.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

#define TL_MAX_CHARS_IN_POST	1000	// Maximum number of characters in a post

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

static void TL_ShowTimeline (struct TL_Timeline *Timeline,
                             const char *Title,long NotCodToHighlight);
static void TL_PutIconsTimeline (__attribute__((unused)) void *Args);

static void TL_PutFormWho (struct TL_Timeline *Timeline);
static Usr_Who_t TL_GetWhoFromDB (void);
static void TL_SetGlobalWho (Usr_Who_t Who);

static void TL_SaveWhoInDB (struct TL_Timeline *Timeline);

static void TL_ShowWarningYouDontFollowAnyUser (void);

static void TL_PutFormToWriteNewPost (struct TL_Timeline *Timeline);

static long TL_ReceivePost (void);

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
   Timeline->UsrOrGbl    = TL_TIMELINE_GBL;
   Timeline->Who         = TL_DEFAULT_WHO;
   Timeline->WhatToGet   = TL_GET_RECENT_TIMELINE;
   Timeline->Pubs.Top    =
   Timeline->Pubs.Bottom = NULL,
   Timeline->NotCod      = -1L;
   Timeline->PubCod      = -1L;
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
   PubCod = TL_Pub_GetParamPubCod ();

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

   /***** Get list of pubications to show in timeline *****/
   Timeline->UsrOrGbl  = TL_TIMELINE_GBL;
   Timeline->WhatToGet = TL_GET_RECENT_TIMELINE;
   TL_Pub_GetListPubsToShowInTimeline (Timeline);

   /***** Show timeline *****/
   TL_ShowTimeline (Timeline,Txt_Timeline,NotCod);

   /***** Free chained list of publications *****/
   TL_Pub_FreeListPubs (Timeline);
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

   /***** Get list of pubications to show in timeline *****/
   Timeline->UsrOrGbl  = TL_TIMELINE_USR;
   Timeline->WhatToGet = TL_GET_RECENT_TIMELINE;
   TL_Pub_GetListPubsToShowInTimeline (Timeline);

   /***** Show timeline *****/
   TL_ShowTimeline (Timeline,
                    Str_BuildStringStr (Txt_Timeline_OF_A_USER,
					Gbl.Usrs.Other.UsrDat.FirstName),
		    NotCod);
   Str_FreeString ();

   /***** Free chained list of publications *****/
   TL_Pub_FreeListPubs (Timeline);
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

      /***** Get list of pubications to show in timeline *****/
      Timeline.UsrOrGbl  = TL_TIMELINE_GBL;
      Timeline.WhatToGet = TL_GET_ONLY_NEW_PUBS;
      TL_Pub_GetListPubsToShowInTimeline (&Timeline);

      /***** Show new timeline *****/
      TL_Pub_InsertNewPubsInTimeline (&Timeline);

      /***** Free chained list of publications *****/
      TL_Pub_FreeListPubs (&Timeline);
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
   /***** Get list of pubications to show in timeline *****/
   TL_Pub_GetListPubsToShowInTimeline (Timeline);

   /***** Show old timeline *****/
   TL_Pub_ShowOldPubsInTimeline (Timeline);

   /***** Free chained list of publications *****/
   TL_Pub_FreeListPubs (Timeline);
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
   struct TL_Pub_Publication *Pub;
   struct TL_Not_Note Not;
   unsigned NumPubs;
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
      TL_Pub_PutLinkToViewNewPublications ();

      /* Hidden list where insert just received (not visible) publications via AJAX */
      HTM_UL_Begin ("id=\"just_now_timeline_list\" class=\"TL_LIST\"");
      HTM_UL_End ();

      /* Hidden list where insert new (not visible) publications via AJAX */
      HTM_UL_Begin ("id=\"new_timeline_list\" class=\"TL_LIST\"");
      HTM_UL_End ();
     }

   /***** List recent publications in timeline *****/
   HTM_UL_Begin ("id=\"timeline_list\" class=\"TL_LIST\"");
   for (Pub = Timeline->Pubs.Top, NumPubs = 0;
	Pub;
	Pub = Pub->Next, NumPubs++)
     {
      /* Get data of note */
      Not.NotCod = Pub->NotCod;
      TL_Not_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_Not_WriteNote (Timeline,&Not,
                        Pub->TopMessage,
                        Pub->PublisherCod,
		        Not.NotCod == NotCodToHighlight ? TL_HIGHLIGHT :
			                                  TL_DONT_HIGHLIGHT,
		        TL_DONT_SHOW_ALONE);
     }
   HTM_UL_End ();

   /***** If the number of publications shown is the maximum,
          probably there will be more, so show link to get more *****/
   if (NumPubs == TL_Pub_MAX_REC_PUBS_TO_GET_AND_SHOW)
     {
      /* Link to view old publications via AJAX */
      TL_Pub_PutLinkToViewOldPublications ();

      /* Hidden list where insert old publications via AJAX */
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
   struct TL_Pub_Publication Pub;

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
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /* Remove all favs for all comments in all the notes of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments_fav"
	           " USING tl_notes,tl_pubs,tl_comments_fav"
	           " WHERE tl_notes.UsrCod=%ld"	// Author of the note
	           " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments_fav.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

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
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /* Remove all the comments from any user in any note of the user */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_pubs"
	           " USING tl_notes,tl_pubs"
	           " WHERE tl_notes.UsrCod=%ld"
		   " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

   /* Remove content of all the comments of the user in any note */
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments"
	           " USING tl_pubs,tl_comments"
	           " WHERE tl_pubs.PublisherCod=%ld"
	           " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);

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
