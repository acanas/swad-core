// swad_notification.h: notifications about events, sent by email

#ifndef _SWAD_NTF_DB
#define _SWAD_NTF_DB
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

#include "swad_browser.h"
#include "swad_notification.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/


/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ntf_DB_MarkNotifChildrenOfFolderAsRemoved (Ntf_NotifyEvent_t NotifyEvent,
                                                Brw_FileBrowser_t FileBrowser,
                                                long Cod,const char *Path);

#endif
