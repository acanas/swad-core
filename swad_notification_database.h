// swad_notification.h: notifications about events, sent by email

#ifndef _SWAD_NTF_DB
#define _SWAD_NTF_DB
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

#include "swad_browser.h"
#include "swad_notification.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ntf_DB_StoreNotifyEventToUsr (Ntf_NotifyEvent_t NotifyEvent,
                                   long ToUsrCod,long Cod,Ntf_Status_t Status,
                                   long HieCods[Hie_NUM_LEVELS]);
void Ntf_DB_UpdateMyLastAccessToNotifications (void);
void Ntf_DB_MarkAllMyNotifAsSeen (void);
void Ntf_DB_MarkPendingNtfsAsSent (long ToUsrCod);
void Ntf_DB_MarkNotifAsSeenUsingNtfCod (long NtfCod);
void Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_NotifyEvent_t NotifyEvent,long Cod);
void Ntf_DB_MarkNotifsInCrsAsSeen (Ntf_NotifyEvent_t NotifyEvent);
void Ntf_DB_MarkNotifsAsSeen (Ntf_NotifyEvent_t NotifyEvent);
void Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod,long ToUsrCod);
void Ntf_DB_MarkNotifInCrsAsRemoved (long ToUsrCod,long HieCod);
void Ntf_DB_MarkNotifAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod);
void Ntf_DB_MarkNotifFilesInGroupAsRemoved (long GrpCod);
void Ntf_DB_MarkNotifChildrenOfFolderAsRemoved (Ntf_NotifyEvent_t NotifyEvent,
                                                Brw_FileBrowser_t FileBrowser,
                                                long Cod,const char *Path);
void Ntf_DB_UpdateNumNotifSent (const struct Hie_Node Hie[Hie_NUM_LEVELS],
                                Ntf_NotifyEvent_t NotifyEvent,
                                unsigned NumEvents,unsigned NumMails);

unsigned Ntf_DB_GetNumNotifSent (MYSQL_RES **mysql_res,
                                 const struct Hie_Node Hie[Hie_NUM_LEVELS],
                                 Ntf_NotifyEvent_t NotifyEvent);
unsigned Ntf_DB_GetMyNotifications (MYSQL_RES **mysql_res,
				    Lay_Show_t ShowAllNotifications);
unsigned Ntf_DB_GetMyRecentNotifications (MYSQL_RES **mysql_res,time_t BeginTime);
unsigned Ntf_DB_GetPendingNtfsToUsr (MYSQL_RES **mysql_res,long ToUsrCod);
unsigned Ntf_DB_GetNumAllMyUnseenNtfs (void);
unsigned Ntf_DB_GetNumMyNewUnseenNtfs (void);
unsigned Ntf_DB_GetUsrsWhoMustBeNotified (MYSQL_RES **mysql_res);
unsigned Ntf_DB_GetNumNotifs (MYSQL_RES **mysql_res,
			      Hie_Level_t HieLvl,Ntf_NotifyEvent_t NotifyEvent);

void Ntf_DB_RemoveUsrNtfs (long ToUsrCod);
void Ntf_DB_RemoveOldNtfs (void);

#endif
