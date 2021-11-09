// swad_timeline_publication.h: social timeline publications

#ifndef _SWAD_TML_PUB
#define _SWAD_TML_PUB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#define Tml_Pub_MAX_REC_PUBS_TO_GET_AND_SHOW	   10	// Recent publishings to show (first time)
#define Tml_Pub_MAX_NEW_PUBS_TO_GET_AND_SHOW	10000	// New publishings retrieved (big number)
#define Tml_Pub_MAX_OLD_PUBS_TO_GET_AND_SHOW	   20	// Old publishings are retrieved in packs of this size

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Tml_Pub_NUM_PUB_TYPES	4
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table tml_notes
typedef enum
  {
   Tml_Pub_UNKNOWN		= 0,
   Tml_Pub_ORIGINAL_NOTE	= 1,
   Tml_Pub_SHARED_NOTE		= 2,
   Tml_Pub_COMMENT_TO_NOTE	= 3,
  } Tml_Pub_Type_t;

struct Tml_Pub_Publication
  {
   long PubCod;				// Publication code
   long NotCod;				// Note code
   long PublisherCod;			// Sharer or writer of the publication
   Tml_Pub_Type_t Type;			// Original note, shared note, comment
   struct Tml_Pub_Publication *Next;	// Used for chained list
  };

struct Tml_Pub_RangePubsToGet
  {
   long Top;
   long Bottom;
  };

#define Tml_Pub_MAX_BYTES_SUBQUERY (128 - 1)
struct Tml_Pub_SubQueries
  {
   char *TablePublishers;
   char Publishers   [Tml_Pub_MAX_BYTES_SUBQUERY + 1];
   char RangeBottom  [Tml_Pub_MAX_BYTES_SUBQUERY + 1];
   char RangeTop     [Tml_Pub_MAX_BYTES_SUBQUERY + 1];
   char AlreadyExists[Tml_Pub_MAX_BYTES_SUBQUERY + 1];
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Tml_Pub_GetListPubsToShowInTimeline (struct Tml_Timeline *Timeline);
void Tml_Pub_FreeListPubs (struct Tml_Timeline *Timeline);

void Tml_Pub_InsertNewPubsInTimeline (struct Tml_Timeline *Timeline);
void Tml_Pub_ShowOldPubsInTimeline (struct Tml_Timeline *Timeline);

Tml_TopMessage_t Tml_Pub_GetTopMessage (Tml_Pub_Type_t PubType);

void Tml_Pub_PutLinkToViewNewPubs (void);
void Tml_Pub_PutLinkToViewOldPubs (void);

void Tml_Pub_PutHiddenParamPubCod (long PubCod);
long Tml_Pub_GetParamPubCod (void);

void Tml_Pub_GetDataOfPubFromNextRow (MYSQL_RES *mysql_res,
                                      struct Tml_Pub_Publication *Pub);

void Tml_Pub_PublishPubInTimeline (struct Tml_Pub_Publication *Pub);

#endif
