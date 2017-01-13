// swad_message.h: messages between users

#ifndef _SWAD_MSG
#define _SWAD_MSG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include "swad_statistic.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Msg_MAX_LENGTH_FILTER_CONTENT 80

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Msg_NUM_TYPES_OF_MSGS 2
typedef enum
  {
   Msg_MESSAGES_RECEIVED,
   Msg_MESSAGES_SENT,
  } Msg_TypeOfMessages_t;

typedef enum
  {
   Msg_STATUS_ALL,
   Msg_STATUS_DELETED,
   Msg_STATUS_NOTIFIED,
  } Msg_Status_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Msg_FormMsgUsrs (void);
void Msg_PutHiddenParamsSubjectAndContent (void);
void Msg_PutHiddenParamAnotherRecipient (const struct UsrData *UsrDat);
void Msg_PutHiddenParamOtherRecipients (void);
void Msg_RecMsgFromUsr (void);

void Msg_ReqDelAllRecMsgs (void);
void Msg_ReqDelAllSntMsgs (void);
void Msg_DelAllRecMsgs (void);
void Msg_DelAllSntMsgs (void);
void Msg_GetParamMsgsCrsCod (void);
void Msg_GetParamFilterFromTo (void);
void Msg_GetParamFilterContent (void);
void Msg_DelSntMsg (void);
void Msg_DelRecMsg (void);
void Msg_ExpSntMsg (void);
void Msg_ExpRecMsg (void);
void Msg_ConSntMsg (void);
void Msg_ConRecMsg (void);
void Msg_SetReceivedMsgAsOpen (long MsgCod,long UsrCod);

void Msg_DelAllRecAndSntMsgsUsr (long UsrCod);
void Msg_MoveUnusedMsgsContentToDeleted (void);

void Msg_ShowSntMsgs (void);
void Msg_ShowRecMsgs (void);

unsigned Msg_GetNumMsgsSentByTchsCrs (long CrsCod);
unsigned long Msg_GetNumMsgsSentByUsr (long UsrCod);
unsigned Msg_GetNumMsgsSent (Sco_Scope_t Scope,Msg_Status_t MsgStatus);
unsigned Msg_GetNumMsgsReceived (Sco_Scope_t Scope,Msg_Status_t MsgStatus);

void Msg_PutHiddenParamsMsgsFilters (void);
void Msg_GetDistinctCoursesInMyMessages (void);
void Msg_ShowFormSelectCourseSentOrRecMsgs (void);
void Msg_ShowFormToFilterMsgs (void);
void Msg_GetMsgSubject (long MsgCod,char *Subject);
void Msg_GetNotifMessage (char *SummaryStr,char **ContentStr,long MsgCod,
                          unsigned MaxChars,bool GetContent);

void Msg_WriteMsgNumber (unsigned long MsgNum,bool NewMsg);
void Msg_WriteMsgAuthor (struct UsrData *UsrDat,
                         unsigned WidthOfNameColumn,unsigned MaxCharsInName,
                         const char *Style,bool Enabled,const char *BgColor);
bool Msg_WriteCrsOrgMsg (long CrsCod);
void Msg_WriteListUsrsDstMsg (long MsgCod);
void Msg_WriteMsgDate (time_t TimeUTC,const char *ClassBackground);
void Msg_WriteMsgContent (char *Content,unsigned long MaxLength,bool InsertLinks,bool ChangeBRToRet);

void Msg_PutHiddenParamMsgCod (long MsgCod);

void Msg_BanSenderWhenShowingMsgs (void);
void Msg_UnbanSenderWhenShowingMsgs (void);
void Msg_UnbanSenderWhenListingUsrs (void);
void Msg_RemoveUsrFromBanned (long UsrCod);
void Msg_ListBannedUsrs (void);

#endif
