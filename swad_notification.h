// swad_notification.h: notifications about events, sent by e-mail

#ifndef _SWAD_NTF
#define _SWAD_NTF
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Ntf_NUM_NOTIFY_EVENTS	15
// If the numbers assigned to each event type change,
// it is necessary to change old numbers to new ones in database tables notif and sta_notif
typedef enum
  {
   Ntf_EVENT_UNKNOWN		=  0,

   /* Course tab */
   Ntf_EVENT_DOCUMENT_FILE	=  1,
   Ntf_EVENT_SHARED_FILE	=  2,

   /* Assessment tab */
   Ntf_EVENT_ASSIGNMENT		=  3,
   Ntf_EVENT_EXAM_ANNOUNCEMENT	=  4,
   Ntf_EVENT_MARKS_FILE		=  5,

   /* Users tab */
   Ntf_EVENT_ENROLLMENT_STUDENT	=  6,
   Ntf_EVENT_ENROLLMENT_TEACHER	=  7,
   Ntf_EVENT_ENROLLMENT_REQUEST	=  8,

   /* Social tab */
   // Ntf_EVENT_FOLLOWER
   // Ntf_EVENT_FORUM_POST_COURSE
   // Ntf_EVENT_FORUM_REPLY

   /* Messages tab */
   Ntf_EVENT_NOTICE		=  9,
   Ntf_EVENT_FORUM_POST_COURSE	= 10,	// TODO: Move to social tab
   Ntf_EVENT_FORUM_REPLY	= 11,	// TODO: Move to social tab
   Ntf_EVENT_MESSAGE		= 12,

   /* Statistics tab */
   Ntf_EVENT_SURVEY		= 13,

   /* Profile tab */
   Ntf_EVENT_FOLLOWER		= 14,	// TODO: Move to social tab

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

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ntf_ShowMyNotifications (void);
Ntf_StatusTxt_t Ntf_GetStatusTxtFromStatusBits (Ntf_Status_t Status);
void Ntf_GetNotifSummaryAndContent (char *SummaryStr,char **ContentStr,Ntf_NotifyEvent_t NotifyEvent,
                                    long Cod,long CrsCod,long UsrCod,unsigned MaxChars,bool GetContent);
void Ntf_SetNotifAsSeen (Ntf_NotifyEvent_t NotifyEvent,long Cod,long ToUsrCod);
void Ntf_SetNotifAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod);
void Ntf_SetNotifToOneUsrAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod,long ToUsrCod);
void Ntf_SetNotifInCrsAsRemoved (long CrsCod,long ToUsrCod);
void Ntf_SetNotifOneFileAsRemoved (Brw_FileBrowser_t FileBrowser,
                                   long Cod,const char *Path);
void Ntf_SetNotifChildrenOfFolderAsRemoved (Brw_FileBrowser_t FileBrowser,
                                            long Cod,const char *Path);
void Ntf_SetNotifFilesInGroupAsRemoved (long GrpCod);
unsigned Ntf_StoreNotifyEventsToAllUsrs (Ntf_NotifyEvent_t NotifyEvent,long Cod);
void Ntf_StoreNotifyEventToOneUser (Ntf_NotifyEvent_t NotifyEvent,
                                    struct UsrData *UsrDat,
                                    long Cod,Ntf_Status_t Status);
void Ntf_SendPendingNotifByEMailToAllUsrs (void);
Ntf_NotifyEvent_t Ntf_GetNotifyEventFromDB (const char *Str);
void Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (unsigned NumUsrsToBeNotifiedByEMail);
void Ntf_MarkAllNotifAsSeen (void);
void Ntf_PutFormChangeNotifSentByEMail (void);
void Ntf_ChangeNotifyEvents (void);

void Ntf_WriteNumberOfNewNtfs (void);
void Ntf_RemoveUsrNtfs (long ToUsrCod);

#endif
