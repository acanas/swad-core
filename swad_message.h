// swad_message.h: messages between users

#ifndef _SWAD_MSG
#define _SWAD_MSG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_contracted_expanded.h"
#include "swad_course.h"
#include "swad_notification.h"
#include "swad_statistic.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Msg_MAX_CHARS_FILTER_CONTENT (128 - 1)	// 127
#define Msg_MAX_BYTES_FILTER_CONTENT ((Msg_MAX_CHARS_FILTER_CONTENT + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

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

#define Msg_NUM_REPLY 2
typedef enum
  {
   Msg_IS_NOT_REPLY,
   Msg_IS_REPLY,
  } Msg_Reply_t;

#define Msg_NUM_REPLIED 2
typedef enum
  {
   Msg_NOT_REPLIED,
   Msg_REPLIED,
  } Msg_Replied_t;

struct Msg_Messages
  {
   Msg_TypeOfMessages_t TypeOfMessages;
   unsigned NumMsgs;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char *Content;	// Message content. Must be allocated and freed
   long FilterCrsCod;	// Show only messages sent from this course code
   char FilterCrsShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];
   char FilterFromTo[Usr_MAX_BYTES_FULL_NAME + 1];		// Show only messages from/to these users
   char FilterContent[Msg_MAX_BYTES_FILTER_CONTENT + 1];	// Show only messages that match this content
   bool OnlyUnreadMsgs;	// Show only unread messages (this option is applicable only for received messages)
   long ExpandedMsgCod;	// The current expanded message code
   struct
     {
      Msg_Reply_t IsReply;	// Is the message I am editing a reply?
      Msg_Replied_t Replied;	// If the message has been replied
      long OriginalMsgCod;	// Original message code when I am editing a reply
     } Reply;
   struct
     {
      unsigned NumRecipients;
      unsigned NumErrors;
     } Rcv;
   bool OnlyOneRecipient;	// Shown only a selected recipient or also other potential recipients?
   unsigned CurrentPage;
   long MsgCod;	// Used as parameter with message to be removed
  };

struct Msg_Status
  {
   CloOpe_ClosedOrOpen_t ClosedOrOpen;
   Msg_Replied_t Replied;
   ConExp_ContractedOrExpanded_t ContractedOrExpanded;
  };

struct Msg_Class
  {
   const char *Title;
   const char *Background;
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
                          char **ContentStr,long MsgCod,Ntf_GetContent_t GetContent);

void Msg_WriteMsgNumber (unsigned long MsgNum,CloOpe_ClosedOrOpen_t ClosedOrOpen);
void Msg_WriteMsgDate (time_t TimeUTC,const char *ClassTxt,const char *ClassBg);

void Msg_BanSenderWhenShowingMsgs (void);
void Msg_UnbanSenderWhenShowingMsgs (void);
void Msg_UnbanSenderWhenListingUsrs (void);
void Msg_ListBannedUsrs (void);

//-------------------------------- Figures ------------------------------------
void Msg_GetAndShowMsgsStats (Hie_Level_t HieLvl);

#endif
