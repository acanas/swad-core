// swad_message_database.h: messages between users, operations with database

#ifndef _SWAD_MSG_DB
#define _SWAD_MSG_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

// #include "swad_course.h"
// #include "swad_notification.h"
// #include "swad_statistic.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Msg_MAX_BYTES_MESSAGES_QUERY (4 * 1024 - 1)

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Msg_DB_ExpandSntMsg (long MsgCod);
void Msg_DB_ExpandRcvMsg (long MsgCod);
void Msg_DB_ContractSntMsg (long MsgCod);
void Msg_DB_ContractRcvMsg (long MsgCod);

void Msg_DB_MakeFilterFromToSubquery (const struct Msg_Messages *Messages,
                                      char FilterFromToSubquery[Msg_MAX_BYTES_MESSAGES_QUERY + 1]);
unsigned Msg_DB_GetSntOrRcvMsgs (MYSQL_RES **mysql_res,
                                 const struct Msg_Messages *Messages,
			         long UsrCod,
			         const char *FilterFromToSubquery);
unsigned Msg_DB_GetSubjectAndContent (MYSQL_RES **mysql_res,long MsgCod);
void Msg_DB_GetMsgSubject (long MsgCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1]);
bool Msg_DB_GetStatusOfSntMsg (long MsgCod);
void Msg_DB_GetStatusOfRcvMsg (long MsgCod,
                               bool *Open,bool *Replied,bool *Expanded);
unsigned Msg_DB_GetNumUsrsBannedByMe (void);

#endif
