// swad_timeline_notification.h: social timeline notifications

#ifndef _SWAD_TML_NTF
#define _SWAD_TML_NTF
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

#include "swad_notification.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TmlNtf_CreateNotifToAuthor (long AuthorCod,long PubCod,
                                 Ntf_NotifyEvent_t NotifyEvent);

void TmlNtf_GetNotifPublication (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                 char **ContentStr,
                                 long PubCod,Ntf_GetContent_t GetContent);

void TmlNtf_MarkMyNotifAsSeen (void);

#endif
