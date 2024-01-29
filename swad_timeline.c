// swad_timeline.c: social timeline

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
mysql> SHOW TABLES LIKE 'tml_%';
+------------------------+
| Tables_in_swad (tml_%) |
+------------------------+
| tml_comments           |
| tml_comments_fav       |
| tml_notes              |
| tml_notes_fav          |
| tml_posts              |
| tml_pubs               |
| tml_timelines          |
+------------------------+
7 rows in set (0.00 sec)

   The timeline is a set of publications.
   A publication can be:
   · an original note    (23573, 80% of 29435)
   · a shared note       ( 1007,  3% of 29435)
   · a comment to a note ( 4855, 17% of 29435)

    *Numbers are got from swad.ugr.es on may 2021

   _tml_pubs______             _tml_comments
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
  |               |                (4855)       | |
  |Publication i+3|--                           | |
  |(original note)|  \                          | |
  |_______________|   \       _tml_notes_____   | |     _cfe_exams_____
  |               |    \     |               |  | |    |               |
  |Publication i+2|--   ---->|     Note n    |<-+ |    | Call for exam | (5581)
  |(original note)|  \       |(exam announc.)|-(2622)->|_______________|
  |_______________|   \      |_______________|  11%     _brw_files_____
  |               |    \     |               |    |    |               |
  |Publication i+1|--   ---->|    Note n-1   |-(64)--->|  Public file  | (1497132)
  |(original note)|  \       | (public file) |  <1%    |_______________|
  |_______________|   \      |_______________|    |     _not_notices___
  |               |    \     |               |    |    |               |
  | Publication i |--   ---->|    Note n-2   |-(17078)>|    Notice     | (14984)
  |(original note)|  \       |    (notice)   |  72%    |_______________|
  |_______________|   \      |_______________|    |     _tml_posts_____
  |               |    \     |               |    |    |               |
  ·      ...      ·     ---->|    Note n-3   |-(3533)->|     Post s    |
  ·      ...      ·          |   (tl. post)  |  15%    |               |
  |_______________|          |_______________|    |    |_______________|
  |               |          |               |    |    |               |
  | Publication 3 |          ·      ...      ·    |    ·      ...      · (3533)
  | (shared note) |---       ·      ...      ·    |    ·      ...      ·
  |_______________|   \      |_______________|    |    |_______________|
  |               |    \     |               |    |    |               |
  | Publication 2 |     ---->|     Note 2    |<---+    |     Post 1    |
  |(original note)|--------->|   (tl. post)  |-------->|               |
  |_______________|          |_______________|         |_______________|
  |               |          |               |          _for_posts_____
  | Publication 1 |--------->|     Note 1    |         |               |
  |(original note)|          | (forum post)  |-(276)-->|  Forum post   | (66226)
  |_______________|          |_______________|   1%    |_______________|
       (29435)                    (23573)

   A note can be:
   · a timeline post      ( 3533, 15% of 23573)
   · a public file        (   64, <1% of 23573)
   · an exam announcement ( 2622, 11% of 23573)
   · a notice             (17078, 72% of 23573)
   · a forum post         (  276,  1% of 23573)
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
#include <stdlib.h>		// For free

#include "swad_box.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_global.h"
#include "swad_message.h"
#include "swad_notification.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_note.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"
#include "swad_timeline_who.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Tml_GetAndShowOldTimeline (struct Tml_Timeline *Timeline);

static void Tml_ShowTimeline (struct Tml_Timeline *Timeline,
                              long NotCodToHighlight,const char *Title);
static void Tml_PutIconsTimeline (__attribute__((unused)) void *Args);
static unsigned Tml_ListRecentPubs (const struct Tml_Timeline *Timeline,
                                    long NotCodToHighlight);

static void Tml_PutHiddenList (const char *Id);

static void Tml_GetNumNotesAndUsrsFromRow (MYSQL_RES *mysql_res,
                                           unsigned *NumNotes,
                                           unsigned *NumUsrs);

/*****************************************************************************/
/************************ Initialize global timeline *************************/
/*****************************************************************************/

void Tml_InitTimelineGbl (struct Tml_Timeline *Timeline)
  {
   /***** Reset timeline context *****/
   Tml_ResetTimeline (Timeline);

   /***** Mark all my notifications about timeline as seen *****/
   TmlNtf_MarkMyNotifAsSeen ();

   /***** Get which users *****/
   Timeline->Who = TmlWho_GetGlobalWho ();
  }

/*****************************************************************************/
/*************************** Reset timeline context **************************/
/*****************************************************************************/

void Tml_ResetTimeline (struct Tml_Timeline *Timeline)
  {
   Timeline->UsrOrGbl    = TmlUsr_TIMELINE_GBL;
   Timeline->Who         = TmlWho_DEFAULT_WHO;
   Timeline->WhatToGet   = Tml_GET_REC_PUBS;
   Timeline->Pubs.Top    =
   Timeline->Pubs.Bottom = NULL,
   Timeline->NotCod      = -1L;
   Timeline->PubCod      = -1L;
  }

/*****************************************************************************/
/**************** Show highlighted note and global timeline ******************/
/*****************************************************************************/

void Tml_ShowTimelineGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Save which users in database *****/
   TmlWho_SaveWhoInDB (&Timeline);

   /***** Show highlighted note and global timeline *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

/*****************************************************************************/
/**************** Show highlighted note and global timeline ******************/
/*****************************************************************************/

void Tml_ShowNoteAndTimelineGbl (struct Tml_Timeline *Timeline)
  {
   long PubCod;
   struct TmlNot_Note Not;

   /***** Get parameter with the code of a publication *****/
   // This parameter is optional. It can be provided by a notification.
   // If > 0 ==> the associated note will be shown highlighted
   //            get its code from database and show it above the timeline
   if ((PubCod = ParCod_GetPar (ParCod_Pub)) > 0)
     {
      if ((Not.NotCod = Tml_DB_GetNotCodFromPubCod (PubCod)) > 0)
          TmlNot_ShowHighlightedNote (Timeline,&Not);
     }
   else
      Not.NotCod = -1L;	//  ==> no highlighted note

   /***** Show timeline with possible highlighted note *****/
   Tml_ShowTimelineGblHighlighting (Timeline,Not.NotCod);
  }

/*****************************************************************************/
/******************* Show global timeline highlighting a note ****************/
/*****************************************************************************/

void Tml_ShowTimelineGblHighlighting (struct Tml_Timeline *Timeline,long NotCod)
  {
   extern const char *Txt_Timeline;

   /***** Get list of pubications to show in timeline *****/
   Timeline->UsrOrGbl  = TmlUsr_TIMELINE_GBL;
   Timeline->WhatToGet = Tml_GET_REC_PUBS;
   TmlPub_GetListPubsToShowInTimeline (Timeline);

   /***** Show timeline *****/
   Tml_ShowTimeline (Timeline,NotCod,Txt_Timeline);

   /***** Free chained list of publications *****/
   TmlPub_FreeListPubs (Timeline);
  }

/*****************************************************************************/
/********************* Show timeline of a selected user **********************/
/*****************************************************************************/

void Tml_ShowTimelineUsr (struct Tml_Timeline *Timeline)
  {
   Tml_ShowTimelineUsrHighlighting (Timeline,-1L);
  }

/*****************************************************************************/
/************ Show timeline of a selected user highlighting a note ***********/
/*****************************************************************************/

void Tml_ShowTimelineUsrHighlighting (struct Tml_Timeline *Timeline,long NotCod)
  {
   extern const char *Txt_Timeline_OF_A_USER;
   char *Title;

   /***** Get list of pubications to show in timeline *****/
   Timeline->UsrOrGbl  = TmlUsr_TIMELINE_USR;
   Timeline->WhatToGet = Tml_GET_REC_PUBS;
   TmlPub_GetListPubsToShowInTimeline (Timeline);

   /***** Show timeline *****/
   if (asprintf (&Title,Txt_Timeline_OF_A_USER,Gbl.Usrs.Other.UsrDat.FrstName) < 0)
      Err_NotEnoughMemoryExit ();
   Tml_ShowTimeline (Timeline,NotCod,Title);
   free (Title);

   /***** Free chained list of publications *****/
   TmlPub_FreeListPubs (Timeline);
  }

/*****************************************************************************/
/************** Refresh new publications in timeline via AJAX ****************/
/*****************************************************************************/

void Tml_RefreshNewTimelineGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Trivial check: session should be open *****/
   if (!Gbl.Session.IsOpen)	// If session has been closed...
      return;			// ...do not write anything

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get which users *****/
   Timeline.Who = TmlWho_GetGlobalWho ();

   /***** Get list of publications to show in timeline *****/
   Timeline.UsrOrGbl  = TmlUsr_TIMELINE_GBL;
   Timeline.WhatToGet = Tml_GET_NEW_PUBS;
   TmlPub_GetListPubsToShowInTimeline (&Timeline);

   /***** Show new timeline *****/
   TmlPub_InsertNewPubsInTimeline (&Timeline);

   /***** Free chained list of publications *****/
   TmlPub_FreeListPubs (&Timeline);
  }

/*****************************************************************************/
/**************** View old publications in timeline via AJAX *****************/
/*****************************************************************************/

void Tml_RefreshOldTimelineGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get which users *****/
   Timeline.Who = TmlWho_GetGlobalWho ();

   /***** Show old publications *****/
   Timeline.UsrOrGbl  = TmlUsr_TIMELINE_GBL;
   Timeline.WhatToGet = Tml_GET_OLD_PUBS;
   Tml_GetAndShowOldTimeline (&Timeline);
  }

void Tml_RefreshOldTimelineUsr (void)
  {
   struct Tml_Timeline Timeline;

   /***** Get user whom profile is displayed *****/
   if (!Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())	// If user does not exist...
      return;							// ...do not write anything

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** If user exists, show old publications *****/
   Timeline.UsrOrGbl  = TmlUsr_TIMELINE_USR;
   Timeline.WhatToGet = Tml_GET_OLD_PUBS;
   Tml_GetAndShowOldTimeline (&Timeline);
  }

/*****************************************************************************/
/**************** Get and show old publications in timeline ******************/
/*****************************************************************************/

static void Tml_GetAndShowOldTimeline (struct Tml_Timeline *Timeline)
  {
   /***** Get list of pubications to show in timeline *****/
   TmlPub_GetListPubsToShowInTimeline (Timeline);

   /***** Show old timeline *****/
   TmlPub_ShowOldPubsInTimeline (Timeline);

   /***** Free chained list of publications *****/
   TmlPub_FreeListPubs (Timeline);
  }

/*****************************************************************************/
/******************************* Show timeline *******************************/
/*****************************************************************************/
/*
               _____
            / |_____| just_now_timeline_list (Posts retrieved automatically
           |  |_____|                         via AJAX from time to time.
           |  |_____|                         They are transferred inmediately
           |     |                            to new_timeline_list.)
  Hidden  <    __v__
           |  |_____| new_timeline_list (Posts retrieved but hidden.
           |  |_____|                    When user clicks to view them,
           |  |_____|                    the most recent of each note is
            \ |_____|                    is transferred
                 |                       to visible timeline_list.)
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
static void Tml_ShowTimeline (struct Tml_Timeline *Timeline,
                              long NotCodToHighlight,const char *Title)
  {
   extern const char *Hlp_START_Timeline;
   bool GlobalTimeline = (Gbl.Usrs.Other.UsrDat.UsrCod <= 0);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Title,
                 Tml_PutIconsTimeline,NULL,
                 Hlp_START_Timeline,Box_NOT_CLOSABLE);

      /***** Put form to select users whom public activity is displayed *****/
      if (GlobalTimeline)
	 TmlWho_PutFormWho (Timeline);

      /***** Form to write a new post *****/
      if (GlobalTimeline || Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_ME)
	 TmlPst_PutPhotoAndFormToWriteNewPost (Timeline);

      /***** New publications refreshed dynamically via AJAX *****/
      if (GlobalTimeline)
	{
	 /* Hidden lists to insert publications received via AJAX:
	    1. just received (not visible) publications
	    2. new (not visible) publications */
	 Tml_PutHiddenList ("just_now_timeline_list");
	 Tml_PutHiddenList ("new_timeline_list");

	 /* Link to view new publications via AJAX */
	 TmlPub_PutLinkToViewNewPubs ();
	}

      /***** List recent publications in timeline.
             If the number of publications shown is the maximum,
	     probably there will be more, so show link to get more *****/
      if (Tml_ListRecentPubs (Timeline,NotCodToHighlight) == TmlPub_MAX_REC_PUBS_TO_GET_AND_SHOW)
	{
	 /* Link to view old publications via AJAX */
	 TmlPub_PutLinkToViewOldPubs ();

	 /* Hidden list to insert old publications via AJAX */
	 Tml_PutHiddenList ("old_timeline_list");
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Put contextual icons in timeline **********************/
/*****************************************************************************/

static void Tml_PutIconsTimeline (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_TIMELINE);
  }

/*****************************************************************************/
/******************* List recent publications in timeline ********************/
/*****************************************************************************/
// Returns number of notes shown

static unsigned Tml_ListRecentPubs (const struct Tml_Timeline *Timeline,
                                    long NotCodToHighlight)
  {
   const struct TmlPub_Publication *Pub;
   struct TmlNot_Note Not;
   unsigned NumNotesShown;

   /***** Begin list *****/
   HTM_UL_Begin ("id=\"timeline_list\" class=\"Tml_LIST\"");

      /***** For each publication in list... *****/
      for (Pub = Timeline->Pubs.Top, NumNotesShown = 0;
	   Pub;
	   Pub = Pub->Next, NumNotesShown++)
	{
	 /* Get data of note */
	 Not.NotCod = Pub->NotCod;
	 TmlNot_GetNoteDataByCod (&Not);

	 /* Write list item (note) */
	 if (Not.NotCod == NotCodToHighlight)
	    HTM_LI_Begin ("class=\"Tml_WIDTH Tml_SEP Tml_NEW_PUB_%s\"",
			  The_GetSuffix ());
	 else
	    HTM_LI_Begin ("class=\"Tml_WIDTH Tml_SEP\"");
	 TmlNot_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
					      TmlPub_GetTopMessage (Pub->Type),
					      Pub->PublisherCod);
	 HTM_LI_End ();
	}

   /***** End list *****/
   HTM_UL_End ();

   return NumNotesShown;
  }

/*****************************************************************************/
/*********** Put a hidden list where publications will be inserted ***********/
/*****************************************************************************/

static void Tml_PutHiddenList (const char *Id)
  {
   HTM_UL_Begin ("id=\"%s\" class=\"Tml_LIST\"",Id);
   HTM_UL_End ();
  }

/*****************************************************************************/
/******************* Write the date of creation of a note ********************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

void Tml_WriteDateTime (time_t TimeUTC)
  {
   char IdDateTime[Frm_MAX_BYTES_ID + 1];

   /***** Create unique Id *****/
   Frm_SetUniqueId (IdDateTime);

   /***** Container where the date-time is written *****/
   HTM_DIV_Begin ("id=\"%s\" class=\"Tml_RIGHT_TIME DAT_LIGHT_%s\"",
                  IdDateTime,The_GetSuffix ());
   HTM_DIV_End ();

   /***** Script to write date and time in browser local time *****/
   // This must be out of the div where the output is written
   // because it will be evaluated in a loop in JavaScript
   Dat_WriteLocalDateHMSFromUTC (IdDateTime,TimeUTC,
		                 Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				 true,true,false,0x6);
  }

/*****************************************************************************/
/******************* Get and show number of timeline notes *******************/
/*****************************************************************************/

void Tml_GetAndShowTimelineActivityStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_timeline;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Type;
   extern const char *Txt_Number_of_posts;
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_Number_of_posts_BR_per_user;
   extern const char *Txt_TIMELINE_NOTE[Tml_NOT_NUM_NOTE_TYPES];
   extern const char *Txt_Total;
   MYSQL_RES *mysql_res;
   TmlNot_Type_t NoteType;
   unsigned NumNotes;
   unsigned NumUsrs;
   unsigned NumUsrsTotal;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_TIMELINE],NULL,NULL,
                      Hlp_ANALYTICS_Figures_timeline,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Type                       ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_posts            ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_users            ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users           ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_posts_BR_per_user,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Get total number of users *****/
      NumUsrsTotal = Usr_GetTotalNumberOfUsers ();

      /***** Get total number of timeline notes and users for each note type *****/
      for (NoteType  = (TmlNot_Type_t) 0;
	   NoteType <= (TmlNot_Type_t) (Tml_NOT_NUM_NOTE_TYPES - 1);
	   NoteType++)
	{
	 /***** Get number of timeline notes and users for this type *****/
	 if (Tml_DB_GetNumNotesAndUsrsByType (&mysql_res,NoteType))
	    Tml_GetNumNotesAndUsrsFromRow (mysql_res,&NumNotes,&NumUsrs);
	 else
	    NumNotes = NumUsrs = 0;

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

	 /***** Write number of timeline notes and number of users *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_Txt_Left (Txt_TIMELINE_NOTE[NoteType]);
	    HTM_TD_Unsigned (NumNotes);
	    HTM_TD_Unsigned (NumUsrs);
	    HTM_TD_Percentage (NumUsrs,NumUsrsTotal);
	    HTM_TD_Ratio (NumNotes,NumUsrs);
	 HTM_TR_End ();
	}

      /***** Get and write totals *****/
      if (Tml_DB_GetNumNotesAndUsrsTotal (&mysql_res))
	 Tml_GetNumNotesAndUsrsFromRow (mysql_res,&NumNotes,&NumUsrs);
      else
	 NumNotes = NumUsrs = 0;

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /* Write totals */
      HTM_TR_Begin (NULL);

	 HTM_TD_LINE_TOP_Txt (Txt_Total);
	 HTM_TD_LINE_TOP_Unsigned (NumNotes);
	 HTM_TD_LINE_TOP_Unsigned (NumUsrs);
	 HTM_TD_LINE_TOP_Percentage (NumUsrs,NumUsrsTotal);
	 HTM_TD_LINE_TOP_Ratio (NumNotes,NumUsrs);

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************** Get number of notes and users from database row **************/
/*****************************************************************************/

static void Tml_GetNumNotesAndUsrsFromRow (MYSQL_RES *mysql_res,
                                           unsigned *NumNotes,
                                           unsigned *NumUsrs)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get number of notes *****/
   if (row[0])
      if (sscanf (row[0],"%u",NumNotes) != 1)
	 *NumNotes = 0;

   /***** Get number of users *****/
   if (row[1])
      if (sscanf (row[1],"%u",NumUsrs) != 1)
	 *NumUsrs = 0;
  }
