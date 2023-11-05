// swad_notification.h: notifications about events, sent by email

#ifndef _SWAD_NTF
#define _SWAD_NTF
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Ntf_MAX_BYTES_NOTIFY_EVENT	(32 - 1)	// 31

#define Ntf_MAX_CHARS_NOTIFY_LOCATION	(128 - 1)	// 127
#define Ntf_MAX_BYTES_NOTIFY_LOCATION	(2048 + (Ntf_MAX_CHARS_NOTIFY_LOCATION + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 4095

#define Ntf_MAX_CHARS_SUMMARY	(256 - 1)	// 255
#define Ntf_MAX_BYTES_SUMMARY	((Ntf_MAX_CHARS_SUMMARY + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 4095

#define Ntf_NOTIFICATIONS_ID	"notif"

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Ntf_NUM_NOTIFY_EVENTS	(1 + 20)
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones
// in database tables ntf_notifications, sta_notifications and usr_data

// TODO: Change numbers (also in database)!!!!!!!!!!!!!!
typedef enum
  {
   Ntf_EVENT_UNKNOWN		=  0,
   /* Start tab */
   Ntf_EVENT_TML_COMMENT	= 10,	// New comment to one of my social publishings (notes or comments)
   Ntf_EVENT_TML_FAV		= 11,	// New fav of one of my social publishings (notes or comments)
   Ntf_EVENT_TML_SHARE		= 12,	// New sharing of one of my social notes
   Ntf_EVENT_TML_MENTION	= 13,	// New mention
   Ntf_EVENT_FOLLOWER		= 14,
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Center tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   Ntf_EVENT_ASSIGNMENT		=  4,
   Ntf_EVENT_CALL_FOR_EXAM	=  5,
   /* Files tab */
   Ntf_EVENT_DOCUMENT_FILE	=  1,
   Ntf_EVENT_TEACHERS_FILE	=  2,
   Ntf_EVENT_SHARED_FILE	=  3,
   Ntf_EVENT_MARKS_FILE		=  6,
   /* Users tab */
   Ntf_EVENT_ENROLMENT_STD	=  7,
   Ntf_EVENT_ENROLMENT_NET	= 20,
   Ntf_EVENT_ENROLMENT_TCH	=  8,
   Ntf_EVENT_ENROLMENT_REQUEST	=  9,
   /* Messages tab */
   Ntf_EVENT_NOTICE		= 17,
   Ntf_EVENT_FORUM_POST_COURSE	= 15,	// New post in forums of my courses
   Ntf_EVENT_FORUM_REPLY	= 16,	// New reply to one of my posts in any forum
   Ntf_EVENT_MESSAGE		= 18,
   /* Analytics tab */
   Ntf_EVENT_SURVEY		= 19,
   /* Profile tab */
  } Ntf_NotifyEvent_t;

typedef enum
  {
   Ntf_STATUS_BIT_EMAIL   = (1 << 0),	// User want to receive notification by email
   Ntf_STATUS_BIT_SENT    = (1 << 1),	// Email has been sent
   Ntf_STATUS_BIT_READ    = (1 << 2),	// User has seen the event which caused the notification
   Ntf_STATUS_BIT_REMOVED = (1 << 3),	// The event which caused the notification has been removed
  } Ntf_Status_Bits_t;
typedef unsigned Ntf_Status_t;

#define Ntf_NUM_STATUS_TXT	4
typedef enum
  {
   Ntf_STATUS_NO_EMAIL        = 0,	// ---0 !(Status & Ntf_STATUS_BIT_EMAIL)
   Ntf_STATUS_EMAIL_PENDING   = 1,	// 0001  (Status & Ntf_STATUS_BIT_EMAIL) && !(Status & Ntf_STATUS_BIT_SENT) && !(Status & (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED))
   Ntf_STATUS_EMAIL_CANCELLED = 2,	// XX01  (Status & Ntf_STATUS_BIT_EMAIL) && !(Status & Ntf_STATUS_BIT_SENT) &&  (Status & (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED))
   Ntf_STATUS_EMAIL_SENT      = 3,	// --11  (Status & Ntf_STATUS_BIT_EMAIL) &&  (Status & Ntf_STATUS_BIT_SENT)
  } Ntf_StatusTxt_t;

typedef enum
  {
   Ntf_DONT_GET_CONTENT = 0,
   Ntf_GET_CONTENT      = 1,
  } Ntf_GetContent_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ntf_ShowMyNotifications (void);

Ntf_NotifyEvent_t Ntf_GetParNotifyEvent (void);

Ntf_StatusTxt_t Ntf_GetStatusTxtFromStatusBits (Ntf_Status_t Status);
void Ntf_GetNotifSummaryAndContent (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                    char **ContentStr,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    long Cod,long CrsCod,long UsrCod,
                                    Ntf_GetContent_t GetContent);

void Ntf_MarkNotifOneFileAsRemoved (const char *Path);
void Ntf_MarkNotifChildrenOfFolderAsRemoved (const char *Path);
unsigned Ntf_StoreNotifyEventsToAllUsrs (Ntf_NotifyEvent_t NotifyEvent,long Cod);
void Ntf_SendPendingNotifByEMailToAllUsrs (void);
Ntf_NotifyEvent_t Ntf_GetNotifyEventFromStr (const char *Str);
void Ntf_MarkAllNotifAsSeen (void);
void Ntf_PutFormChangeNotifSentByEMail (void);
void Ntf_ChangeNotifyEvents (void);

void Ntf_WriteNumberOfNewNtfs (void);

//-------------------------------- Figures ------------------------------------
void Ntf_GetAndShowNumUsrsPerNotifyEvent (void);

#endif
