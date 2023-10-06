// swad_message.h: messages between users

#ifndef _SWAD_MSG
#define _SWAD_MSG
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

#include "swad_course.h"
#include "swad_notification.h"
#include "swad_statistic.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Msg_MAX_CHARS_FILTER_CONTENT (128 - 1)	// 127
#define Msg_MAX_BYTES_FILTER_CONTENT ((Msg_MAX_CHARS_FILTER_CONTENT + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Msg_MAX_BYTES_LIST_OTHER_RECIPIENTS	((Cns_MAX_BYTES_USR_LOGIN + 2) * Cfg_MAX_USRS_IN_LIST)

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Msg_NUM_TYPES_OF_MSGS 3
typedef enum
  {
   Msg_WRITING,
   Msg_RECEIVED,
   Msg_SENT,
  } Msg_TypeOfMessages_t;

#define Msg_NUM_STATUS 3
typedef enum
  {
   Msg_STATUS_ALL,
   Msg_STATUS_DELETED,
   Msg_STATUS_NOTIFIED,
  } Msg_Status_t;

struct Msg_Course
  {
   long CrsCod;
   char ShrtName[Cns_MAX_BYTES_SHRT_NAME + 1];
  };	// Distinct courses in my messages sent or received

struct Msg_Messages
  {
   Msg_TypeOfMessages_t TypeOfMessages;
   unsigned NumMsgs;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   long FilterCrsCod;	// Show only messages sent from this course code
   char FilterCrsShrtName[Cns_MAX_BYTES_SHRT_NAME + 1];
   char FilterFromTo[Usr_MAX_BYTES_FULL_NAME + 1];		// Show only messages from/to these users
   char FilterContent[Msg_MAX_BYTES_FILTER_CONTENT + 1];	// Show only messages that match this content
   bool ShowOnlyUnreadMsgs;	// Show only unread messages (this option is applicable only for received messages)
   long ExpandedMsgCod;	// The current expanded message code
   struct
     {
      bool IsReply;			// Is the message I am editing a reply?
      long OriginalMsgCod;		// Original message code when I am editing a reply
     } Reply;
   bool ShowOnlyOneRecipient;	// Shown only a selected recipient or also other potential recipients?
   unsigned CurrentPage;
   long MsgCod;	// Used as parameter with message to be removed
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Msg_FormMsgUsrs (void);
void Msg_RecMsgFromUsr (void);

void Msg_ReqDelAllRecMsgs (void);
void Msg_ReqDelAllSntMsgs (void);
void Msg_DelAllRecMsgs (void);
void Msg_DelAllSntMsgs (void);
void Msg_DelSntMsg (void);
void Msg_DelRecMsg (void);
void Msg_ExpSntMsg (void);
void Msg_ExpRecMsg (void);
void Msg_ConSntMsg (void);
void Msg_ConRecMsg (void);

void Msg_ShowSntMsgs (void);
void Msg_ShowRecMsgs (void);

void Msg_PutParsMsgsFilters (void *Messages);
void Msg_GetNotifMessage (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                          char **ContentStr,long MsgCod,bool GetContent);

void Msg_WriteMsgNumber (unsigned long MsgNum,bool NewMsg);
void Msg_WriteMsgDate (time_t TimeUTC,const char *ClassTxt,const char *ClassBg);
void Msg_WriteMsgContent (char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                          bool InsertLinks,bool ChangeBRToRet);

void Msg_BanSenderWhenShowingMsgs (void);
void Msg_UnbanSenderWhenShowingMsgs (void);
void Msg_UnbanSenderWhenListingUsrs (void);
void Msg_ListBannedUsrs (void);

//-------------------------------- Figures ------------------------------------
void Msg_GetAndShowMsgsStats (void);

#endif
