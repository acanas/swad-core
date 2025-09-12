// swad_message_database.h: messages between users, operations with database

#ifndef _SWAD_MSG_DB
#define _SWAD_MSG_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_hierarchy_type.h"
#include "swad_message.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Msg_DB_MAX_BYTES_MESSAGES_QUERY (4 * 1024 - 1)

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

long Msg_DB_CreateNewMsg (const char *Subject,const char *Content,long MedCod);
void Msg_DB_CreateSntMsg (long MsgCod,long CrsCod);
void Msg_DB_CreateRcvMsg (long MsgCod,long UsrCod,bool NotifyByEmail);
void Msg_DB_SetRcvMsgAsReplied (long MsgCod);
void Msg_DB_ExpandSntMsg (long MsgCod);
void Msg_DB_ExpandRcvMsg (long MsgCod);
void Msg_DB_ContractSntMsg (long MsgCod);
void Msg_DB_ContractRcvMsg (long MsgCod);
void Msg_DB_SetRcvMsgAsOpen (long MsgCod,long UsrCod);
void Msg_DB_CopyRcvMsgToDeleted (long MsgCod,long UsrCod);
void Msg_DB_CopySntMsgToDeleted (long MsgCod);

unsigned Msg_DB_GetDistinctCrssInMyRcvMsgs (MYSQL_RES **mysql_res);
unsigned Msg_DB_GetDistinctCrssInMySntMsgs (MYSQL_RES **mysql_res);
void Msg_DB_MakeFilterFromToSubquery (const struct Msg_Messages *Messages,
                                      char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1]);
unsigned Msg_DB_GetNumUnreadMsgs (const struct Msg_Messages *Messages,
                                  const char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1]);
unsigned Msg_DB_GetSntOrRcvMsgs (MYSQL_RES **mysql_res,
                                 const struct Msg_Messages *Messages,
			         long UsrCod,
			         const char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1]);
Exi_Exist_t Msg_DB_GetSubjectAndContent (MYSQL_RES **mysql_res,long MsgCod);
void Msg_DB_GetMsgSubject (long MsgCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1]);
Exi_Exist_t Msg_DB_GetMsgContent (MYSQL_RES **mysql_res,long MsgCod);
unsigned Msg_DB_GetMsgSntData (MYSQL_RES **mysql_res,long MsgCod,bool *Deleted);
bool Msg_DB_GetStatusOfSntMsg (long MsgCod);
void Msg_DB_GetStatusOfRcvMsg (long MsgCod,CloOpe_ClosedOrOpen_t *Open,
			       bool *Replied,bool *Expanded);
bool Msg_DB_CheckIfSntMsgIsDeleted (long MsgCod);
bool Msg_DB_CheckIfRcvMsgIsDeletedForAllItsRecipients (long MsgCod);
long Msg_DB_GetSender (long MsgCod);
bool Msg_DB_CheckIfMsgHasBeenReceivedByMe (long MsgCod);
unsigned Msg_DB_GetRecipientsCods (MYSQL_RES **mysql_res,
                                   long ReplyUsrCod,const char *ListRecipients);
unsigned Msg_DB_GetNumRecipients (long MsgCod);
unsigned Msg_DB_GetKnownRecipients (MYSQL_RES **mysql_res,long MsgCod);
unsigned Msg_DB_GetNumSntMsgs (Hie_Level_t HieLvl,Msg_Status_t MsgStatus);
unsigned Msg_DB_GetNumRcvMsgs (Hie_Level_t HieLvl,Msg_Status_t MsgStatus);
unsigned Msg_DB_GetNumMsgsSentByTchsCrs (long CrsCod);
unsigned Msg_DB_GetNumMsgsSentByUsr (long UsrCod);

void Msg_DB_RemoveRcvMsg (long MsgCod,long UsrCod);
void Msg_DB_RemoveSntMsg (long MsgCod);
void Msg_DB_RemoveAllRecAndSntMsgsUsr (long UsrCod);
void Msg_DB_MoveMsgContentToDeleted (long MsgCod);
void Msg_DB_MoveUnusedMsgsContentToDeleted (void);

//--------------------------- Users banned ------------------------------------
void Msg_DB_CreateUsrsPairIntoBanned (long FromUsrCod,long ToUsrCod);
unsigned Msg_DB_GetNumUsrsBannedBy (long UsrCod);
unsigned Msg_DB_GetUsrsBannedBy (MYSQL_RES **mysql_res,long UsrCod);
bool Msg_DB_CheckIfUsrIsBanned (long FromUsrCod,long ToUsrCod);

void Msg_DB_RemoveUsrsPairFromBanned (long FromUsrCod,long ToUsrCod);
void Msg_DB_RemoveUsrFromBanned (long UsrCod);

#endif
