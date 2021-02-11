// swad_timeline.h: social timeline

#ifndef _SWAD_TL
#define _SWAD_TL
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

#define TL_TIMELINE_SECTION_ID	"timeline"

#define TL_ICON_ELLIPSIS	"ellipsis-h.svg"

#define TL_DEF_USRS_SHOWN	5	// Default maximum number of users shown who have share/fav a note
#define TL_MAX_USRS_SHOWN	1000	// Top     maximum number of users shown who have share/fav a note

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

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define TL_NUM_TOP_MESSAGES (1 + 6)
typedef enum
  {
   TL_TOP_MESSAGE_NONE		= 0,
   TL_TOP_MESSAGE_COMMENTED	= 1,
   TL_TOP_MESSAGE_FAVED		= 2,
   TL_TOP_MESSAGE_UNFAVED	= 3,
   TL_TOP_MESSAGE_SHARED	= 4,
   TL_TOP_MESSAGE_UNSHARED	= 5,
   TL_TOP_MESSAGE_MENTIONED	= 6,
  } TL_TopMessage_t;

typedef enum
  {
   TL_TIMELINE_USR,	// Show the timeline of a user
   TL_TIMELINE_GBL,	// Show the timeline of the users follwed by me
  } TL_UsrOrGbl_t;

#define TL_NUM_WHAT_TO_GET 3
typedef enum
  {
   TL_GET_RECENT_TIMELINE,	// Recent timeline is shown when the user clicks on action menu,...
				// or after editing timeline
   TL_GET_ONLY_NEW_PUBS,	// New publications are retrieved via AJAX
				// automatically from time to time
   TL_GET_ONLY_OLD_PUBS,	// Old publications are retrieved via AJAX
				// when the user clicks on link at bottom of timeline
  } TL_WhatToGet_t;

struct TL_Timeline
  {
   TL_UsrOrGbl_t UsrOrGbl;
   Usr_Who_t Who;
   TL_WhatToGet_t WhatToGet;
   struct
     {
      struct TL_Publication *Top;	// Points to first element in list of publications
      struct TL_Publication *Bottom;	// Points to last  element in list of publications
     } Pubs;
   long NotCod;		// Used as parameter about social note to be edited, removed...
   long PubCod;		// Used as parameter about social publishing to be edited, removed...
  };

typedef enum
  {
   TL_SHOW_FEW_USRS,	// Show a few first favers/sharers
   TL_SHOW_ALL_USRS,	// Show all favers/sharers
  } TL_HowManyUsrs_t;

#define TL_NUM_PUB_TYPES	4
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database table tl_notes
typedef enum
  {
   TL_PUB_UNKNOWN		= 0,
   TL_PUB_ORIGINAL_NOTE		= 1,
   TL_PUB_SHARED_NOTE		= 2,
   TL_PUB_COMMENT_TO_NOTE	= 3,
  } TL_PubType_t;

struct TL_Publication
  {
   long PubCod;
   long NotCod;
   long PublisherCod;		// Sharer or writer of the publication
   TL_PubType_t PubType;
   TL_TopMessage_t TopMessage;	// Used to show feedback on the action made
   struct TL_Publication *Next;	// Used for chained list
  };

struct TL_PostContent
  {
   char Txt[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Med_Media Media;
  };

typedef enum
  {
   TL_DONT_HIGHLIGHT,
   TL_HIGHLIGHT,
  } TL_Highlight_t;

typedef enum
  {
   TL_DONT_SHOW_ALONE,
   TL_SHOW_ALONE,
  } TL_ShowAlone_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TL_InitTimelineGbl (struct TL_Timeline *Timeline);

void TL_ResetTimeline (struct TL_Timeline *Timeline);

void TL_ShowTimelineGbl (void);
void TL_ShowNoteAndTimelineGbl (struct TL_Timeline *Timeline);
void TL_ShowTimelineGblHighlightingNot (struct TL_Timeline *Timeline,
	                                long NotCod);

void TL_ShowTimelineUsr (struct TL_Timeline *Timeline);
void TL_ShowTimelineUsrHighlightingNot (struct TL_Timeline *Timeline,
                                        long NotCod);

void TL_RefreshNewTimelineGbl (void);

void TL_RefreshOldTimelineGbl (void);
void TL_RefreshOldTimelineUsr (void);

void TL_MarkMyNotifAsSeen (void);

void TL_FormStart (const struct TL_Timeline *Timeline,
	           Act_Action_t ActionGbl,
                   Act_Action_t ActionUsr);

void TL_GetParamWho (void);
Usr_Who_t TL_GetGlobalWho (void);

void TL_WriteTopMessage (TL_TopMessage_t TopMessage,long PublisherCod);

void TL_WriteDateTime (time_t TimeUTC);

void TL_GetAndWritePost (long PstCod);

void TL_PublishPubInTimeline (struct TL_Publication *Pub);

void TL_PutTextarea (const char *Placeholder,const char *ClassTextArea);

void TL_ReceivePostUsr (void);
void TL_ReceivePostGbl (void);

void TL_Com_ShowHiddenCommentsUsr (void);
void TL_Com_ShowHiddenCommentsGbl (void);

void TL_PutHiddenParamPubCod (long PubCod);
long TL_GetParamPubCod (void);

void TL_CreateNotifToAuthor (long AuthorCod,long PubCod,
                             Ntf_NotifyEvent_t NotifyEvent);

void TL_RemoveUsrContent (long UsrCod);

void TL_ShowNumSharersOrFavers (unsigned NumUsrs);
void TL_ShowSharersOrFavers (MYSQL_RES **mysql_res,
			     unsigned NumUsrs,unsigned NumFirstUsrs);
void TL_PutFormToSeeAllFaversSharers (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		                      const char *ParamFormat,long ParamCod,
                                      TL_HowManyUsrs_t HowManyUsrs);
void TL_FormFavSha (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		    const char *ParamFormat,long ParamCod,
		    const char *Icon,const char *Title);

void TL_ClearOldTimelinesDB (void);

void TL_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                             char **ContentStr,
                             long PubCod,bool GetContent);

unsigned long TL_GetNumPubsUsr (long UsrCod);

#endif
