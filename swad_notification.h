// swad_notification.h: notifications about events, sent by email

#ifndef _SWAD_NTF
#define _SWAD_NTF
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

#include "swad_browser_type.h"
#include "swad_notification_type.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ntf_ShowMyNotifications (void);

Ntf_NotifyEvent_t Ntf_GetParNotifyEvent (void);

Ntf_StatusTxt_t Ntf_GetStatusTxtFromStatusBits (Ntf_Status_t Status);
void Ntf_GetNotifSummaryAndContent (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                    char **ContentStr,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    long Cod,long HieCod,long UsrCod,
                                    Ntf_GetContent_t GetContent);

void Ntf_MarkNotifOneFileAsRemoved (Brw_Zone_t Zone,const char *Path);
void Ntf_MarkNotifChildrenOfFolderAsRemoved (const struct Brw_FileBrowser *FileBrowser);
unsigned Ntf_StoreNotifyEventsToAllUsrs (Ntf_NotifyEvent_t NotifyEvent,long Cod);
void Ntf_SendPendingNotifByEMailToAllUsrs (void);
Ntf_NotifyEvent_t Ntf_GetNotifyEventFromStr (const char *Str);
void Ntf_MarkAllNotifAsSeen (void);
void Ntf_PutFormChangeNotifSentByEMail (void);
void Ntf_ChangeNotifyEvents (void);

void Ntf_WriteNumberOfNewNtfs (void);

//-------------------------------- Figures ------------------------------------
void Ntf_GetAndShowNumUsrsPerNotifyEvent (Hie_Level_t HieLvl);

#endif
