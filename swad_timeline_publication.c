// swad_timeline_publication.c: social timeline publications

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

#define TL_Pub_MAX_BYTES_SUBQUERY (128 - 1)
struct TL_Pub_SubQueries
  {
   char *TablePublishers;
   char Publishers   [TL_Pub_MAX_BYTES_SUBQUERY + 1];
   char RangeBottom  [TL_Pub_MAX_BYTES_SUBQUERY + 1];
   char RangeTop     [TL_Pub_MAX_BYTES_SUBQUERY + 1];
   char AlreadyExists[TL_Pub_MAX_BYTES_SUBQUERY + 1];
  };

struct TL_Pub_RangePubsToGet
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

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static unsigned TL_Pub_GetMaxPubsToGet (const struct TL_Timeline *Timeline);

static void TL_Pub_CreateTmpTableCurrentTimeline (const struct TL_Timeline *Timeline);
static void TL_Pub_CreateTmpTablePublishers (void);
static void TL_Pub_DropTmpTablesUsedToQueryTimeline (void);

static void TL_Pub_CreateSubQueryPublishers (const struct TL_Timeline *Timeline,
                                             struct TL_Pub_SubQueries *SubQueries);
static void TL_Pub_CreateSubQueryAlreadyExists (const struct TL_Timeline *Timeline,
                                                struct TL_Pub_SubQueries *SubQueries);
static void TL_Pub_CreateSubQueryRangeBottom (const struct TL_Pub_RangePubsToGet *RangePubsToGet,
                                              struct TL_Pub_SubQueries *SubQueries);
static void TL_Pub_CreateSubQueryRangeTop (const struct TL_Pub_RangePubsToGet *RangePubsToGet,
                                           struct TL_Pub_SubQueries *SubQueries);

static long TL_Pub_GetPubCodFromSession (const char *FieldName);
static void TL_Pub_UpdateFirstLastPubCodesIntoSession (const struct TL_Timeline *Timeline);

static struct TL_Pub_Publication *TL_Pub_SelectTheMostRecentPub (const struct TL_Pub_SubQueries *SubQueries);

static void TL_Pub_GetDataOfPublicationFromNextRow (MYSQL_RES *mysql_res,
                                                    struct TL_Pub_Publication *Pub);
static TL_Pub_PubType_t TL_Pub_GetPubTypeFromStr (const char *Str);

/*****************************************************************************/
/*************** Get list of pubications to show in timeline *****************/
/*****************************************************************************/

void TL_Pub_GetListPubsToShowInTimeline (struct TL_Timeline *Timeline)
  {
   struct TL_Pub_SubQueries SubQueries;
   struct TL_Pub_RangePubsToGet RangePubsToGet;
   unsigned MaxPubsToGet = TL_Pub_GetMaxPubsToGet (Timeline);
   unsigned NumPub;
   struct TL_Pub_Publication *Pub;

   /***** Clear timeline for this session in database *****/
   if (Timeline->WhatToGet == TL_GET_RECENT_TIMELINE)
      TL_Not_ClearTimelineNotesThisSessionFromDB ();

   /***** Create temporary table with notes in current timeline *****/
   TL_Pub_CreateTmpTableCurrentTimeline (Timeline);

   /***** Create temporary table and subquery with potential publishers *****/
   TL_Pub_CreateSubQueryPublishers (Timeline,&SubQueries);

   /***** Create subquery to get only notes not present in timeline *****/
   TL_Pub_CreateSubQueryAlreadyExists (Timeline,&SubQueries);

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
	 RangePubsToGet.Bottom = TL_Pub_GetPubCodFromSession ("LastPubCod");
	 break;
      case TL_GET_ONLY_OLD_PUBS:	 // Get some limited publications older than FirstPubCod
	 /* This query is made via AJAX
	    when I click in link to get old publications */
	 RangePubsToGet.Top    = TL_Pub_GetPubCodFromSession ("FirstPubCod");
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
   TL_Pub_CreateSubQueryRangeBottom (&RangePubsToGet,&SubQueries);

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

   /*
   Chained list of publications:

   Timeline->Pubs.Top   Pub #0
         ______         ______         Pub #1
        |______|------>|______|        ______        Pub #2
                       |______|    -> |______|       ______        Pub #3
                       |______|   /   |______|    ->|______|       ______
                       |______|  /    |______|   /  |______|    ->|______|
                       |_Next_|--     |______|  /   |______|   // |______|
                                      |_Next_|--    |______|  //  |______|
         ______                                     |_Next_|--/   |______|
        |______|----------------------------------------------    |_NULL_|

   Timeline->Pubs.Bottom

   */
   Timeline->Pubs.Top    =
   Timeline->Pubs.Bottom = NULL;

   for (NumPub = 0;
	NumPub < MaxPubsToGet;
	NumPub++)
     {
      /* Create subquery with top range of publications to get from tl_pubs
         In each iteration of this loop, top publication code is changed to a lower value */
      TL_Pub_CreateSubQueryRangeTop (&RangePubsToGet,&SubQueries);

      /* Select the most recent publication from tl_pubs */
      Pub = TL_Pub_SelectTheMostRecentPub (&SubQueries);

      /* Chain the previous publication with the current one */
      if (NumPub == 0)
	 Timeline->Pubs.Top          = Pub;	// Pointer to top publication
      else
	 Timeline->Pubs.Bottom->Next = Pub;	// Chain the previous publication with the current one
      Timeline->Pubs.Bottom = Pub;	// Update pointer to bottom publication

      if (Pub == NULL)	// Nothing got ==> abort loop
         break;	// Last publication

      /* Insert note in temporary tables with just retrieved notes.
	 These tables will be used to not get notes already shown */
      TL_Not_InsertNoteInJustRetrievedNotes (Pub->NotCod);
      if (Timeline->WhatToGet == TL_GET_ONLY_OLD_PUBS)
	 TL_Not_InsertNoteInVisibleTimeline (Pub->NotCod);

      /* Narrow the range for the next iteration */
      RangePubsToGet.Top = Pub->PubCod;
     }

   /***** Update first (oldest) and last (more recent) publication codes
          into session for next refresh *****/
   TL_Pub_UpdateFirstLastPubCodesIntoSession (Timeline);

   /***** Add notes just retrieved to current timeline for this session *****/
   TL_Not_AddNotesJustRetrievedToTimelineThisSession ();

   /***** Drop temporary tables *****/
   TL_Pub_DropTmpTablesUsedToQueryTimeline ();
  }

/*****************************************************************************/
/********* Get maximum number of publications to get from database ***********/
/*****************************************************************************/

static unsigned TL_Pub_GetMaxPubsToGet (const struct TL_Timeline *Timeline)
  {
   static const unsigned MaxPubsToGet[TL_NUM_WHAT_TO_GET] =
     {
      [TL_GET_RECENT_TIMELINE] = TL_Pub_MAX_REC_PUBS_TO_GET_AND_SHOW,
      [TL_GET_ONLY_NEW_PUBS  ] = TL_Pub_MAX_NEW_PUBS_TO_GET_AND_SHOW,
      [TL_GET_ONLY_OLD_PUBS  ] = TL_Pub_MAX_OLD_PUBS_TO_GET_AND_SHOW,
     };

   return MaxPubsToGet[Timeline->WhatToGet];
  }

/*****************************************************************************/
/************* Create temporary tables used to query timeline ****************/
/*****************************************************************************/

static void TL_Pub_CreateTmpTableCurrentTimeline (const struct TL_Timeline *Timeline)
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

static void TL_Pub_CreateTmpTablePublishers (void)
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

static void TL_Pub_DropTmpTablesUsedToQueryTimeline (void)
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

static void TL_Pub_CreateSubQueryPublishers (const struct TL_Timeline *Timeline,
                                             struct TL_Pub_SubQueries *SubQueries)
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
	       TL_Pub_CreateTmpTablePublishers ();
	       SubQueries->TablePublishers = ",tl_tmp_publishers";
	       Str_Copy (SubQueries->Publishers,
			 "tl_pubs.PublisherCod=tl_tmp_publishers.UsrCod AND ",
			 TL_Pub_MAX_BYTES_SUBQUERY);
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

static void TL_Pub_CreateSubQueryAlreadyExists (const struct TL_Timeline *Timeline,
                                                struct TL_Pub_SubQueries *SubQueries)
  {
   switch (Timeline->WhatToGet)
     {
      case TL_GET_RECENT_TIMELINE:
      case TL_GET_ONLY_NEW_PUBS:
	 Str_Copy (SubQueries->AlreadyExists,
		   " tl_pubs.NotCod NOT IN"
		   " (SELECT NotCod FROM tl_tmp_just_retrieved_notes)",	// Avoid notes just retrieved
		   TL_Pub_MAX_BYTES_SUBQUERY);
         break;
      case TL_GET_ONLY_OLD_PUBS:
	 Str_Copy (SubQueries->AlreadyExists,
		   " tl_pubs.NotCod NOT IN"
		   " (SELECT NotCod FROM tl_tmp_visible_timeline)",	// Avoid notes already shown
		   TL_Pub_MAX_BYTES_SUBQUERY);
	 break;
     }
  }

/*****************************************************************************/
/***** Create subqueries with range of publications to get from tl_pubs ******/
/*****************************************************************************/

static void TL_Pub_CreateSubQueryRangeBottom (const struct TL_Pub_RangePubsToGet *RangePubsToGet,
                                              struct TL_Pub_SubQueries *SubQueries)
  {
   if (RangePubsToGet->Bottom > 0)
      sprintf (SubQueries->RangeBottom,"tl_pubs.PubCod>%ld AND ",
	       RangePubsToGet->Bottom);
   else
      SubQueries->RangeBottom[0] = '\0';
  }

static void TL_Pub_CreateSubQueryRangeTop (const struct TL_Pub_RangePubsToGet *RangePubsToGet,
                                           struct TL_Pub_SubQueries *SubQueries)
  {
   if (RangePubsToGet->Top > 0)
      sprintf (SubQueries->RangeTop,"tl_pubs.PubCod<%ld AND ",
	       RangePubsToGet->Top);
   else
      SubQueries->RangeTop[0] = '\0';
  }

/*****************************************************************************/
/************* Get last/first publication code stored in session *************/
/*****************************************************************************/
// FieldName can be:
// "LastPubCod"
// "FirstPubCod"

static long TL_Pub_GetPubCodFromSession (const char *FieldName)
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

static void TL_Pub_UpdateFirstLastPubCodesIntoSession (const struct TL_Timeline *Timeline)
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
	 FirstPubCod = Timeline->Pubs.Bottom ? Timeline->Pubs.Bottom->PubCod :
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
	 FirstPubCod = Timeline->Pubs.Bottom ? Timeline->Pubs.Bottom->PubCod :
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
/************** Free chained list of publications in timeline ****************/
/*****************************************************************************/

void TL_Pub_FreeListPubs (struct TL_Timeline *Timeline)
  {
   struct TL_Pub_Publication *Pub;
   struct TL_Pub_Publication *Next;

   /***** Go over the list freeing memory *****/
   for (Pub = Timeline->Pubs.Top;
	Pub;
	Pub = Next)
     {
      /* Save a copy of pointer to next element before freeing it */
      Next = Pub->Next;

      /* Free memory used for this publication */
      free (Pub);
     }

   /***** Reset pointers to top and bottom elements *****/
   Timeline->Pubs.Top    =
   Timeline->Pubs.Bottom = NULL;
  }

/*****************************************************************************/
/************** Select the most recent publication from tl_pubs **************/
/*****************************************************************************/

static struct TL_Pub_Publication *TL_Pub_SelectTheMostRecentPub (const struct TL_Pub_SubQueries *SubQueries)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPubs = 0;	// Initialized to avoid warning
   struct TL_Pub_Publication *Pub;

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
     {
      /* Allocate space for publication */
      if ((Pub = (struct TL_Pub_Publication *) malloc (sizeof (struct TL_Pub_Publication))) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory publication.");

      /* Get data of publication */
      TL_Pub_GetDataOfPublicationFromNextRow (mysql_res,Pub);
      Pub->Next = NULL;
     }
   else
      Pub = NULL;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Pub;
  }

/*****************************************************************************/
/******************* Show new publications in timeline ***********************/
/*****************************************************************************/
// The publications are inserted as list elements of a hidden list

void TL_Pub_InsertNewPubsInTimeline (struct TL_Timeline *Timeline)
  {
   struct TL_Pub_Publication *Pub;
   struct TL_Not_Note Not;

   /***** List new publications timeline *****/
   for (Pub = Timeline->Pubs.Top;
	Pub;
	Pub = Pub->Next)
     {
      /* Get data of note */
      Not.NotCod = Pub->NotCod;
      TL_Not_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_Not_WriteNote (Timeline,&Not,
                        Pub->TopMessage,
                        Pub->PublisherCod,
                        TL_DONT_HIGHLIGHT,
                        TL_DONT_SHOW_ALONE);
     }
  }

/*****************************************************************************/
/********************* Show old publications in timeline *********************/
/*****************************************************************************/
// The publications are inserted as list elements of a hidden list

void TL_Pub_ShowOldPubsInTimeline (struct TL_Timeline *Timeline)
  {
   struct TL_Pub_Publication *Pub;
   struct TL_Not_Note Not;

   /***** List old publications in timeline *****/
   for (Pub = Timeline->Pubs.Top;
	Pub;
	Pub = Pub->Next)
     {
      /* Get data of note */
      Not.NotCod = Pub->NotCod;
      TL_Not_GetDataOfNoteByCod (&Not);

      /* Write note */
      TL_Not_WriteNote (Timeline,&Not,
                        Pub->TopMessage,
                        Pub->PublisherCod,
                        TL_DONT_HIGHLIGHT,
                        TL_DONT_SHOW_ALONE);
     }
  }

/*****************************************************************************/
/***************** Put link to view new publications in timeline *************/
/*****************************************************************************/

void TL_Pub_PutLinkToViewNewPublications (void)
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

void TL_Pub_PutLinkToViewOldPublications (void)
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
/*************** Put parameter with the code of a publication ****************/
/*****************************************************************************/

void TL_Pub_PutHiddenParamPubCod (long PubCod)
  {
   Par_PutHiddenParamLong (NULL,"PubCod",PubCod);
  }

/*****************************************************************************/
/**************** Get parameter with the code of a publication ***************/
/*****************************************************************************/

long TL_Pub_GetParamPubCod (void)
  {
   /***** Get comment code *****/
   return Par_GetParToLong ("PubCod");
  }

/*****************************************************************************/
/*********** Create a notification for the author of a post/comment **********/
/*****************************************************************************/

void TL_Pub_CreateNotifToAuthor (long AuthorCod,long PubCod,
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

long TL_Pub_GetNotCodFromPubCod (long PubCod)
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
/***************** Get data of publication using its code ********************/
/*****************************************************************************/

static void TL_Pub_GetDataOfPublicationFromNextRow (MYSQL_RES *mysql_res,
                                                    struct TL_Pub_Publication *Pub)
  {
   static const TL_TopMessage_t TopMessages[TL_NUM_PUB_TYPES] =
     {
      [TL_PUB_UNKNOWN        ] = TL_TOP_MESSAGE_NONE,
      [TL_PUB_ORIGINAL_NOTE  ] = TL_TOP_MESSAGE_NONE,
      [TL_PUB_SHARED_NOTE    ] = TL_TOP_MESSAGE_SHARED,
      [TL_Pub_COMMENT_TO_NOTE] = TL_TOP_MESSAGE_COMMENTED,
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
   Pub->PubType      = TL_Pub_GetPubTypeFromStr ((const char *) row[3]);
   Pub->TopMessage   = TopMessages[Pub->PubType];
  }

/*****************************************************************************/
/******* Get publication type from string number coming from database ********/
/*****************************************************************************/

static TL_Pub_PubType_t TL_Pub_GetPubTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < TL_NUM_PUB_TYPES)
         return (TL_Pub_PubType_t) UnsignedNum;

   return TL_PUB_UNKNOWN;
  }

/*****************************************************************************/
/***************** Get notification of a new publication *********************/
/*****************************************************************************/

void TL_Pub_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                 char **ContentStr,
                                 long PubCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct TL_Pub_Publication Pub;
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
      TL_Pub_GetDataOfPublicationFromNextRow (mysql_res,&Pub);

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
      case TL_Pub_COMMENT_TO_NOTE:
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
/********************* Publish note/comment in timeline **********************/
/*****************************************************************************/
// Pub->PubCod is set by the function

void TL_Pub_PublishPubInTimeline (struct TL_Pub_Publication *Pub)
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
/****************** Get number of publications from a user *******************/
/*****************************************************************************/

unsigned long TL_Pub_GetNumPubsUsr (long UsrCod)
  {
   /***** Get number of posts from a user from database *****/
   return DB_QueryCOUNT ("can not get number of publications from a user",
			 "SELECT COUNT(*) FROM tl_pubs"
			 " WHERE PublisherCod=%ld",
			 UsrCod);
  }
