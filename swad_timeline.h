// swad_timeline.h: social timeline

#ifndef _SWAD_TML
#define _SWAD_TML
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#define Tml_TIMELINE_SECTION_ID	"timeline"

/*
   Timeline images will be saved with:
   · maximum width  of Tml_IMAGE_SAVED_MAX_HEIGHT
   · maximum height of Tml_IMAGE_SAVED_MAX_HEIGHT
   · maintaining the original aspect ratio (aspect ratio recommended: 3:2)
*/
#define Tml_IMAGE_SAVED_MAX_WIDTH	768
#define Tml_IMAGE_SAVED_MAX_HEIGHT	768
#define Tml_IMAGE_SAVED_QUALITY		 90	// 1 to 100
// in timeline posts, the quality should not be high in order to speed up the loading of images

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Tml_NUM_TOP_MESSAGES (1 + 4)
typedef enum
  {
   Tml_TOP_MESSAGE_NONE		= 0,	// 0 to avoid need of explicit inicialization
   Tml_TOP_MESSAGE_COMMENTED	= 1,
   Tml_TOP_MESSAGE_FAVED	= 2,
   Tml_TOP_MESSAGE_SHARED	= 3,
   Tml_TOP_MESSAGE_MENTIONED	= 4,
  } Tml_TopMessage_t;

#define Tml_NUM_WHAT_TO_GET 3
typedef enum
  {
   Tml_GET_NEW_PUBS,	// New publications are retrieved via AJAX...
			// automatically from time to time
   Tml_GET_REC_PUBS,	// Recent timeline is shown...
			// when the user clicks on action menu,...
			// or after editing timeline
   Tml_GET_OLD_PUBS,	// Old publications are retrieved via AJAX...
			// when the user clicks on link...
			// at bottom of timeline
  } Tml_WhatToGet_t;

struct Tml_Timeline
  {
   TmlUsr_UsrOrGbl_t UsrOrGbl;
   Usr_Who_t Who;
   Tml_WhatToGet_t WhatToGet;
   struct
     {
      struct TmlPub_Publication *Top;		// Points to first element in list of publications
      struct TmlPub_Publication *Bottom;	// Points to last  element in list of publications
     } Pubs;
   long NotCod;		// Used as parameter about social note to be edited, removed...
   long PubCod;		// Used as parameter about social publishing to be edited, removed...
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Tml_InitTimelineGbl (struct Tml_Timeline *Timeline);

void Tml_ResetTimeline (struct Tml_Timeline *Timeline);

void Tml_ShowTimelineGbl (void);
void Tml_ShowNoteAndTimelineGbl (struct Tml_Timeline *Timeline);
void Tml_ShowTimelineGblHighlighting (struct Tml_Timeline *Timeline,long NotCod);

void Tml_ShowTimelineUsr (struct Tml_Timeline *Timeline);
void Tml_ShowTimelineUsrHighlighting (struct Tml_Timeline *Timeline,long NotCod);

void Tml_RefreshNewTimelineGbl (void);

void Tml_RefreshOldTimelineGbl (void);
void Tml_RefreshOldTimelineUsr (void);

void Tml_WriteDateTime (time_t TimeUTC);

//-------------------------------- Figures ------------------------------------
void Tml_GetAndShowTimelineActivityStats (Hie_Level_t HieLvl);

#endif
