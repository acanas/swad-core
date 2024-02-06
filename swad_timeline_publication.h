// swad_timeline_publication.h: social timeline publications

#ifndef _SWAD_TML_PUB
#define _SWAD_TML_PUB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_form.h"
#include "swad_media.h"
#include "swad_notification.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

// Number of recent publishings got and shown the first time, before refreshing
#define TmlPub_MAX_NEW_PUBS_TO_GET_AND_SHOW	10000	// New publishings retrieved (big number)
#define TmlPub_MAX_REC_PUBS_TO_GET_AND_SHOW	   10	// Recent publishings to show (first time)
#define TmlPub_MAX_OLD_PUBS_TO_GET_AND_SHOW	   20	// Old publishings are retrieved in packs of this size

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define TmlPub_NUM_PUB_TYPES	4
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table tml_notes
typedef enum
  {
   TmlPub_UNKNOWN		= 0,
   TmlPub_ORIGINAL_NOTE	= 1,
   TmlPub_SHARED_NOTE		= 2,
   TmlPub_COMMENT_TO_NOTE	= 3,
  } TmlPub_Type_t;

struct TmlPub_Publication
  {
   long PubCod;				// Publication code
   long NotCod;				// Note code
   long PublisherCod;			// Sharer or writer of the publication
   TmlPub_Type_t Type;			// Original note, shared note, comment
   struct TmlPub_Publication *Next;	// Used for chained list
  };

#define TmlPub_NUM_RANGES 2
typedef enum
  {
   TmlPub_TOP    = 0,
   TmlPub_BOTTOM = 1,
  } TmlPub_Range_t;

struct TmlPub_RangePubsToGet
  {
   long Top;	// Top pub code
   long Bottom;	// Bottom pub code
  };

#define TmlPub_NUM_FIRST_LAST 2
typedef enum
  {
   TmlPub_FIRST = 0,
   TmlPub_LAST  = 1,
  } TmlPub_FirstLast_t;

#define TmlPub_MAX_BYTES_SUBQUERY (128 - 1)
struct TmlPub_SubQueries
  {
   struct
     {
      char *Table;
      char SubQuery[TmlPub_MAX_BYTES_SUBQUERY + 1];
     } Publishers;
   struct
     {
      char Top   [TmlPub_MAX_BYTES_SUBQUERY + 1];
      char Bottom[TmlPub_MAX_BYTES_SUBQUERY + 1];
     } Range;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TmlPub_GetListPubsToShowInTimeline (struct Tml_Timeline *Timeline);
void TmlPub_FreeListPubs (struct Tml_Timeline *Timeline);

void TmlPub_InsertNewPubsInTimeline (struct Tml_Timeline *Timeline);
void TmlPub_ShowOldPubsInTimeline (struct Tml_Timeline *Timeline);

Tml_TopMessage_t TmlPub_GetTopMessage (TmlPub_Type_t PubType);

void TmlPub_PutLinkToViewNewPubs (void);
void TmlPub_PutLinkToViewOldPubs (void);

void TmlPub_GetPubDataFromRow (MYSQL_RES *mysql_res,
                               struct TmlPub_Publication *Pub);

void TmlPub_PublishPubInTimeline (struct TmlPub_Publication *Pub);

#endif
