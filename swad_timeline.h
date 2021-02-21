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
#include "swad_timeline_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define TL_TIMELINE_SECTION_ID	"timeline"

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
   TL_Usr_UsrOrGbl_t UsrOrGbl;
   Usr_Who_t Who;
   TL_WhatToGet_t WhatToGet;
   struct
     {
      struct TL_Pub_Publication *Top;	// Points to first element in list of publications
      struct TL_Pub_Publication *Bottom;	// Points to last  element in list of publications
     } Pubs;
   long NotCod;		// Used as parameter about social note to be edited, removed...
   long PubCod;		// Used as parameter about social publishing to be edited, removed...
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

#define TL_NUM_ACTIONS 4
typedef enum
  {
   TL_RECEIVE_POST,	// Receive post
   TL_RECEIVE_COMM,	// Receive comment
   TL_REQ_REM_NOTE,	// Request removal note
   TL_REQ_REM_COMM,	// Request removal comment
  } TL_FormAction_t;

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

void TL_FormStart (const struct TL_Timeline *Timeline,TL_FormAction_t Action);

void TL_WriteDateTime (time_t TimeUTC);

#endif
