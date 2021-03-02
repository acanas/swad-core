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
  |_______________|   \       ___tl_notes____   | |    exam_announcements
  |               |    \     |               |  | |    |               |
  |Publication i+2|--   ---->|     Note n    |<-+ |    | Exam announc. | (5571)
  |(original note)|  \       |(exam announc.)|-(2639)->|_______________|
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf

#include "swad_figure.h"
#include "swad_global.h"
#include "swad_message.h"
#include "swad_notification.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_note.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"
#include "swad_timeline_who.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
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

static void TL_GetAndShowOldTimeline (struct TL_Timeline *Timeline);

static void TL_ShowTimeline (struct TL_Timeline *Timeline,
                             const char *Title,long NotCodToHighlight);
static void TL_PutIconsTimeline (__attribute__((unused)) void *Args);

/*****************************************************************************/
/************************ Initialize global timeline *************************/
/*****************************************************************************/

void TL_InitTimelineGbl (struct TL_Timeline *Timeline)
  {
   /***** Reset timeline context *****/
   TL_ResetTimeline (Timeline);

   /***** Mark all my notifications about timeline as seen *****/
   TL_Ntf_MarkMyNotifAsSeen ();

   /***** Get which users *****/
   Timeline->Who = TL_Who_GetGlobalWho ();
  }

/*****************************************************************************/
/*************************** Reset timeline context **************************/
/*****************************************************************************/

void TL_ResetTimeline (struct TL_Timeline *Timeline)
  {
   Timeline->UsrOrGbl    = TL_Usr_TIMELINE_GBL;
   Timeline->Who         = TL_Who_DEFAULT_WHO;
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
   TL_Who_SaveWhoInDB (&Timeline);

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
      Not.NotCod = TL_DB_GetNotCodFromPubCod (PubCod);

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
   Timeline->UsrOrGbl  = TL_Usr_TIMELINE_GBL;
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
   Timeline->UsrOrGbl  = TL_Usr_TIMELINE_USR;
   Timeline->WhatToGet = TL_GET_RECENT_TIMELINE;
   TL_Pub_GetListPubsToShowInTimeline (Timeline);

   /***** Show timeline *****/
   TL_ShowTimeline (Timeline,
                    Str_BuildStringStr (Txt_Timeline_OF_A_USER,
					Gbl.Usrs.Other.UsrDat.FrstName),
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
      Timeline.Who = TL_Who_GetGlobalWho ();

      /***** Get list of pubications to show in timeline *****/
      Timeline.UsrOrGbl  = TL_Usr_TIMELINE_GBL;
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
   Timeline.Who = TL_Who_GetGlobalWho ();

   /***** Show old publications *****/
   Timeline.UsrOrGbl  = TL_Usr_TIMELINE_GBL;
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
      Timeline.UsrOrGbl  = TL_Usr_TIMELINE_USR;
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

   /***** Begin box *****/
   Box_BoxBegin (NULL,Title,
                 TL_PutIconsTimeline,NULL,
                 Hlp_START_Timeline,Box_NOT_CLOSABLE);

      /***** Put form to select users whom public activity is displayed *****/
      if (GlobalTimeline)
	 TL_Who_PutFormWho (Timeline);

      /***** Form to write a new post *****/
      if (GlobalTimeline ||
	  Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod))
	 TL_Pst_PutPhotoAndFormToWriteNewPost (Timeline);

      /***** New publications refreshed dynamically via AJAX *****/
      if (GlobalTimeline)
	{
	 /* Link to view new publications via AJAX */
	 TL_Pub_PutLinkToViewNewPublications ();

	 /* Hidden list where insert
	    just received (not visible) publications via AJAX */
	 HTM_UL_Begin ("id=\"just_now_timeline_list\" class=\"TL_LIST\"");
	 HTM_UL_End ();

	 /* Hidden list where insert
	    new (not visible) publications via AJAX */
	 HTM_UL_Begin ("id=\"new_timeline_list\" class=\"TL_LIST\"");
	 HTM_UL_End ();
	}

      /***** List recent publications in timeline *****/
      /* Begin list */
      HTM_UL_Begin ("id=\"timeline_list\" class=\"TL_LIST\"");

	 /* For each publication in list... */
	 for (Pub = Timeline->Pubs.Top, NumPubs = 0;
	      Pub;
	      Pub = Pub->Next, NumPubs++)
	   {
	    /* Get data of note */
	    Not.NotCod = Pub->NotCod;
	    TL_Not_GetDataOfNoteByCod (&Not);

	    /* Write note */
	    HTM_LI_Begin ("class=\"%s\"",Not.NotCod == NotCodToHighlight ? "TL_WIDTH TL_SEP TL_NEW_PUB" :
									   "TL_WIDTH TL_SEP");
	       TL_Not_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
						   TL_Pub_GetTopMessage (Pub->PubType),
						   Pub->PublisherCod);
	    HTM_LI_End ();
	   }

      /* End list */
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
