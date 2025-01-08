// swad_timeline_publication.c: social timeline publications

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include <stdlib.h>		// For malloc and free
#include <string.h>		// For string functions

#include "swad_error.h"
#include "swad_follow.h"
#include "swad_follow_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_profile.h"
#include "swad_profile_database.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_note.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlPub_InitializeRangeOfPubs (Tml_WhatToGet_t WhatToGet,
                                          struct TmlPub_RangePubsToGet *RangePubsToGet);

static unsigned TmlPub_GetMaxPubsToGet (const struct Tml_Timeline *Timeline);

static void TmlPub_UpdateFirstLastPubCodesIntoSession (const struct Tml_Timeline *Timeline);

static struct TmlPub_Publication *TmlPub_SelectTheMostRecentPub (const struct TmlPub_SubQueries *SubQueries);

static TmlPub_Type_t TmlPub_GetPubTypeFromStr (const char *Str);

/*****************************************************************************/
/*************** Get list of pubications to show in timeline *****************/
/*****************************************************************************/
/*
             _ ______________________
            / |______________________| Tml_GET_ONLY_NEW_PUBS
     New  <   |______________________| automatically from time to time
            \_|______________________| (AJAX)
             _|_See_new_activity_(3)_|
            / |______________________| Tml_GET_RECENT_TIMELINE
           |  |______________________| user clicks on action menu
   Recent <   |______________________| or after editing timeline
           |  |______________________|
            \_|______________________|
             _|_______See_more_______|
            / |______________________| Tml_GET_ONLY_OLD_PUBS
           |  |______________________| user clicks on bottom link
     Old  <   |______________________| (AJAX)
           |  |______________________|
            \_|______________________|
*/
void TmlPub_GetListPubsToShowInTimeline (struct Tml_Timeline *Timeline)
  {
   struct TmlPub_SubQueries SubQueries;
   struct TmlPub_RangePubsToGet RangePubsToGet;
   unsigned MaxPubsToGet = TmlPub_GetMaxPubsToGet (Timeline);
   unsigned NumPub;
   struct TmlPub_Publication *Pub;

   /***** Initialize range of publications *****/
   TmlPub_InitializeRangeOfPubs (Timeline->WhatToGet,&RangePubsToGet);

   /***** Clear timeline for this session in database *****/
   if (Timeline->WhatToGet == Tml_GET_REC_PUBS)
      Tml_DB_ClearTimelineNotesOfSessionFromDB ();

   /***** Create temporary table with all notes visible in timeline *****/
   Tml_DB_CreateTmpTableTimeline (Timeline->WhatToGet);

   /***** Create subqueries *****/
   /* Create subquery with potential publishers */
   Tml_DB_CreateSubQueryPublishers (Timeline->UsrOrGbl,Timeline->Who,
                                    &SubQueries.Publishers.Table,
                                    SubQueries.Publishers.SubQuery);

   /* Create subquery with bottom range of publications to get from tml_pubs.
      Bottom pub. code remains unchanged in all iterations of the loop. */
   Tml_DB_CreateSubQueryRange (TmlPub_BOTTOM,
                               RangePubsToGet.Bottom,
                               SubQueries.Range.Bottom);

   /***** Initialize list of publications *****/
   /* Chained list of publications:

   Timeline->Pubs.Top   Pub #0
         ______         ______         Pub #1
        |______|------>|______|        ______        Pub #2
                       |______|    -> |______|       ______        Pub #3
                       |______|   /   |______|    ->|______|       ______
                       |______|  /    |______|   /  |______|    ->|______|
                       |_Next_|--     |______|  /   |______|   // |______|
                      more recent     |_Next_|--    |______|  //  |______|
         ______                                     |_Next_|--/   |______|
        |______|----------------------------------------------    |_NULL_|
                                                                    older
   Timeline->Pubs.Bottom

   */
   Timeline->Pubs.Top    =
   Timeline->Pubs.Bottom = NULL;

   /***** Get the publications in timeline *****/
   /* With the current approach, we select one by one
      the publications and notes in a loop.
      In each iteration:
	 we get the most recent publication (original, shared or comment)
	 of every set of publications corresponding to the same note,
	 checking that the note is not already retrieved.
	 After getting a publication, its note code is saved
	 in order to not get it again.

      As an alternative, we tried to get the maximum PubCod,
      i.e more recent publication (original, shared or comment),
      of every set of publications corresponding to the same note:
      SELECT MAX(PubCod) AS NewestPubCod
        FROM tml_pubs ...
    GROUP BY NotCod
    ORDER BY NewestPubCod DESC
       LIMIT 10
      but this query is slow (several seconds) with a big table.
   */
   for (NumPub = 0;
	NumPub < MaxPubsToGet;
	NumPub++)
     {
      /* Create subquery with top range of publications to get from tml_pubs
         In each iteration of this loop, top publication code is changed to a lower value */
      Tml_DB_CreateSubQueryRange (TmlPub_TOP,
                                  RangePubsToGet.Top,
                                  SubQueries.Range.Top);

      /* Select the most recent publication from tml_pubs */
      Pub = TmlPub_SelectTheMostRecentPub (&SubQueries);

      /* Chain the previous publication with the current one */
      if (NumPub == 0)
	 Timeline->Pubs.Top          = Pub;	// Pointer to top publication
      else
	 Timeline->Pubs.Bottom->Next = Pub;	// Chain the previous publication with the current one
      Timeline->Pubs.Bottom = Pub;		// Update pointer to bottom publication

      if (Pub == NULL)	// Nothing got ==> abort loop
         break;		// Last publication

      /* Insert note in temporary tables with visible notes.
	 These tables will be used to not get notes already shown */
      Tml_DB_InsertNoteInTimeline (Pub->NotCod);

      /* Narrow the range for the next iteration */
      RangePubsToGet.Top = Pub->PubCod;
     }

   /***** Update first (oldest) and last (more recent) publication codes
          into session for next refresh *****/
   TmlPub_UpdateFirstLastPubCodesIntoSession (Timeline);

   /***** Drop temporary tables *****/
   /* Drop temporary table with visible notes in timeline */
   Tml_DB_DropTmpTableTimeline ();

   /* Drop temporary table with me and users I follow */
   if (Timeline->UsrOrGbl == TmlUsr_TIMELINE_GBL &&	// Show the global timeline
       Timeline->Who == Usr_WHO_FOLLOWED)		// Show the timeline of the users I follow
      Fol_DB_DropTmpTableMeAndUsrsIFollow ();
  }

/*****************************************************************************/
/*************** Get list of pubications to show in timeline *****************/
/*****************************************************************************/

static void TmlPub_InitializeRangeOfPubs (Tml_WhatToGet_t WhatToGet,
                                          struct TmlPub_RangePubsToGet *RangePubsToGet)
  {
   /*        tml_pubs
               _____  0
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
                      0 */
   switch (WhatToGet)
     {
      case Tml_GET_NEW_PUBS:	// Get the publications (without limit)
				// newer than last pub. code
	 /* Via AJAX automatically from time to time */
         RangePubsToGet->Top = 0;
	 /*  _ _____  0 <-- RangePubsToGet.Top = +infinite
    Get     / |_____|11
   these  <   |_____|10
    pubs    \_|_____| 9
            / |_____| 8 <-- RangePubsToGet.Bottom = last pub. code
    Pubs   |  |_____| 7
  already <   |_____| 6
   shown   |  |_____| 5
           |  |_____| 4
           .  |_____| .
           .  |_____| .
           .  |_____| .
          */
	 RangePubsToGet->Bottom = Tml_DB_GetPubCodFromSession (TmlPub_LAST);
	 break;
      case Tml_GET_REC_PUBS:	// Get some limited recent publications
	 /* First query to get initial timeline shown
	    ==> no notes yet in current timeline table */
	 RangePubsToGet->Top = 0;
	 /*  _ _____  0 <-- RangePubsToGet.Top = +infinite
            / |_____| 8
     Get   |  |_____| 7
     pubs <   |_____| 6
     from  |  |_____| 5
     all   .  |_____| 4
     range .  |_____| 3
           .  |_____| 2
              |_____| 1
                      0 <-- RangePubsToGet.Bottom = -infinite */
	 RangePubsToGet->Bottom = 0;
	 break;
      case Tml_GET_OLD_PUBS:	// Get some limited publications
				// older than first pub. code
	 /* Via AJAX when I click in link to get old publications */
	 RangePubsToGet->Top = Tml_DB_GetPubCodFromSession (TmlPub_FIRST);
	 /*    _____
           .  |_____| .
           .  |_____| .
           .  |_____| .
    Pubs   |  |_____| 8
  already <   |_____| 7
   shown   |  |_____| 6
           |  |_____| 5
    Get     \_|_____| 4 <-- RangePubsToGet.Top = first pub. code
   pubs     / |_____| 3
   from   <   |_____| 2
   this     \_|_____| 1
   rage               0 <-- RangePubsToGet.Bottom = -infinite */
         RangePubsToGet->Bottom = 0;
	 break;
      default:	// Not reached
	 RangePubsToGet->Top    =
	 RangePubsToGet->Bottom = 0;	// Initialized to avoid warning
	 break;
     }
  }

/*****************************************************************************/
/********* Get maximum number of publications to get from database ***********/
/*****************************************************************************/

static unsigned TmlPub_GetMaxPubsToGet (const struct Tml_Timeline *Timeline)
  {
   static unsigned MaxPubsToGet[Tml_NUM_WHAT_TO_GET] =
     {
      [Tml_GET_NEW_PUBS] = TmlPub_MAX_NEW_PUBS_TO_GET_AND_SHOW,
      [Tml_GET_REC_PUBS] = TmlPub_MAX_REC_PUBS_TO_GET_AND_SHOW,
      [Tml_GET_OLD_PUBS] = TmlPub_MAX_OLD_PUBS_TO_GET_AND_SHOW,
     };

   return MaxPubsToGet[Timeline->WhatToGet];
  }

/*****************************************************************************/
/************* Update first (oldest) and last (more recent)    ***************/
/************* publication codes into session for next refresh ***************/
/*****************************************************************************/

static void TmlPub_UpdateFirstLastPubCodesIntoSession (const struct Tml_Timeline *Timeline)
  {
   long FirstPubCod;

   switch (Timeline->WhatToGet)
     {
      case Tml_GET_NEW_PUBS:	// Get only new publications
	 Tml_DB_UpdateLastPubCodInSession ();
	 break;
      case Tml_GET_REC_PUBS:	// Get recent publications
      case Tml_GET_OLD_PUBS:	// Get only old publications
	 // The oldest publication code retrieved and shown
	 FirstPubCod = Timeline->Pubs.Bottom ? Timeline->Pubs.Bottom->PubCod :
			                       0;
	 if (Timeline->WhatToGet == Tml_GET_REC_PUBS)
            Tml_DB_UpdateFirstLastPubCodsInSession (FirstPubCod);
	 else
	    Tml_DB_UpdateFirstPubCodInSession (FirstPubCod);
	 break;
     }
  }

/*****************************************************************************/
/************** Free chained list of publications in timeline ****************/
/*****************************************************************************/

void TmlPub_FreeListPubs (struct Tml_Timeline *Timeline)
  {
   struct TmlPub_Publication *Pub;
   struct TmlPub_Publication *Next;

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
/************** Select the most recent publication from tml_pubs *************/
/*****************************************************************************/

static struct TmlPub_Publication *TmlPub_SelectTheMostRecentPub (const struct TmlPub_SubQueries *SubQueries)
  {
   MYSQL_RES *mysql_res;
   struct TmlPub_Publication *Pub;

   /***** Select the most recent publication from database *****/
   if (Tml_DB_SelectTheMostRecentPub (&mysql_res,SubQueries) == 1)
     {
      /* Allocate space for publication */
      if ((Pub = malloc (sizeof (*Pub))) == NULL)
         Err_NotEnoughMemoryExit ();

      /* Get data of publication */
      TmlPub_GetPubDataFromRow (mysql_res,Pub);
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

void TmlPub_InsertNewPubsInTimeline (struct Tml_Timeline *Timeline)
  {
   struct TmlPub_Publication *Pub;
   struct TmlNot_Note Not;

   /***** List new publications in timeline *****/
   for (Pub = Timeline->Pubs.Top;
	Pub;
	Pub = Pub->Next)
     {
      /* Get data of note */
      Not.NotCod = Pub->NotCod;
      TmlNot_GetNoteDataByCod (&Not);

      /* Write note */
      HTM_LI_Begin ("class=\"Tml_WIDTH Tml_SEP Tml_NEW_PUB_%s\""
	            " data-note-code=\"%ld\"",	// Note code to be read later...
	            The_GetSuffix (),		// ...from JavaScript...
                    Not.NotCod);		// ...to avoid repeating notes
	 TmlNot_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
					     TmlPub_GetTopMessage (Pub->Type),
					     Pub->PublisherCod);
      HTM_LI_End ();
     }
  }

/*****************************************************************************/
/********************* Show old publications in timeline *********************/
/*****************************************************************************/
// The publications are inserted as list elements of a hidden list

void TmlPub_ShowOldPubsInTimeline (struct Tml_Timeline *Timeline)
  {
   struct TmlPub_Publication *Pub;
   struct TmlNot_Note Not;

   /***** List old publications in timeline *****/
   for (Pub = Timeline->Pubs.Top;
	Pub;
	Pub = Pub->Next)
     {
      /* Get data of note */
      Not.NotCod = Pub->NotCod;
      TmlNot_GetNoteDataByCod (&Not);

      /* Write note */
      HTM_LI_Begin ("class=\"Tml_WIDTH Tml_SEP\"");
	 TmlNot_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
					     TmlPub_GetTopMessage (Pub->Type),
					     Pub->PublisherCod);
      HTM_LI_End ();
     }
  }

/*****************************************************************************/
/************* Get a top message given the type of publication ***************/
/*****************************************************************************/

Tml_TopMessage_t TmlPub_GetTopMessage (TmlPub_Type_t PubType)
  {
   static const Tml_TopMessage_t TopMessages[TmlPub_NUM_PUB_TYPES] =
     {
      [TmlPub_SHARED_NOTE    ] = Tml_TOP_MESSAGE_SHARED,
      [TmlPub_COMMENT_TO_NOTE] = Tml_TOP_MESSAGE_COMMENTED,
     };

   return TopMessages[PubType];
  }

/*****************************************************************************/
/***************** Put link to view new publications in timeline *************/
/*****************************************************************************/

void TmlPub_PutLinkToViewNewPubs (void)
  {
   extern const char *Txt_See_new_activity;

   /***** Link to view (show hidden) new publications *****/
   /* Begin container */
   // div is hidden. When new notes arrive to the client via AJAX, div is shown
   HTM_DIV_Begin ("id=\"view_new_container\""
		  " class=\"Tml_WIDTH Tml_SEP BG_HIGHLIGHT\""
		  " style=\"display:none;\"");

      /* Begin anchor */
      HTM_A_Begin ("href=\"\" class=\"FORM_IN_%s BOLD\""
		   " onclick=\"moveNewTimelineToTimeline();return false;\"",
		   The_GetSuffix ());

         /* Text */
	 HTM_TxtF ("%s (",Txt_See_new_activity);
	 HTM_SPAN_Begin ("id=\"view_new_count\"");
	    HTM_Unsigned (0);
	 HTM_SPAN_End ();
	 HTM_Txt (")");

      /* End anchor */
      HTM_A_End ();

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Put link to view old publications in timeline *************/
/*****************************************************************************/

void TmlPub_PutLinkToViewOldPubs (void)
  {
   extern const char *Txt_See_more;

   /***** Begin container *****/
   HTM_DIV_Begin ("id=\"view_old_pubs_container\" class=\"Tml_WIDTH Tml_SEP\"");

     /***** Put button to refresh *****/
     // div is visible. When refresh starts, div is hidden
      HTM_DIV_Begin ("id=\"view_more\"");
	 HTM_BUTTON_Begin (Txt_See_more,
			   "class=\"BT_LINK FORM_IN_%s BOLD\""
			   " onclick=\"refreshOldTimeline();return false;\"",
			   The_GetSuffix ());
	    Ico_PutIconTextUpdate (Txt_See_more);
	 HTM_BUTTON_End ();
      HTM_DIV_End ();

      /***** Put icon refreshing *****/
      // div is hidden. When refresh starts, div is unhidden
      HTM_DIV_Begin ("id=\"loading_old_timeline\" style=\"display:none;\"");
	 Ico_PutIcon ("Spin-1s-200px.gif",Ico_BLACK,Txt_See_more,"Tml_WAIT_IMG");
      HTM_DIV_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Get data of publication using its code ********************/
/*****************************************************************************/

void TmlPub_GetPubDataFromRow (MYSQL_RES *mysql_res,
                               struct TmlPub_Publication *Pub)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]: PubCod
   row[1]: NotCod
   row[2]: PublisherCod
   row[3]: PubType
   */
   /***** Get code of publication (row[0]), code of note (row[1]),
          publisher's code (row[2]) and type of publication (row[3]) *****/
   Pub->PubCod       = Str_ConvertStrCodToLongCod (row[0]);
   Pub->NotCod       = Str_ConvertStrCodToLongCod (row[1]);
   Pub->PublisherCod = Str_ConvertStrCodToLongCod (row[2]);
   Pub->Type         = TmlPub_GetPubTypeFromStr (row[3]);
  }

/*****************************************************************************/
/******* Get publication type from string number coming from database ********/
/*****************************************************************************/

static TmlPub_Type_t TmlPub_GetPubTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   /***** Get publication type from string *****/
   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < TmlPub_NUM_PUB_TYPES)
         return (TmlPub_Type_t) UnsignedNum;

   return TmlPub_UNKNOWN;
  }

/*****************************************************************************/
/********************* Publish note/comment in timeline **********************/
/*****************************************************************************/
// Pub->PubCod is set by the function

void TmlPub_PublishPubInTimeline (struct TmlPub_Publication *Pub)
  {
   /***** Publish note in timeline *****/
   Pub->PubCod = Tml_DB_CreateNewPub (Pub);

   /***** Increment number of publications in user's figures *****/
   Prf_DB_IncrementNumTimelinePubsUsr (Pub->PublisherCod);
  }
