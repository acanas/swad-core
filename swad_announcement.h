// swad_announcement.h: Global announcements

#ifndef _SWAD_ANN
#define _SWAD_ANN
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_constant.h"
#include "swad_hidden_visible.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Ann_Announcement
  {
   long AnnCod;
   HidVis_HiddenOrVisible_t HiddenOrVisible;
   unsigned Roles;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_TEXT + 1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ann_ShowAllAnnouncements (void);
void Ann_ShowMyAnnouncementsNotMarkedAsSeen (void);

void Ann_ShowFormAnnouncement (void);
void Ann_ReceiveAnnouncement (void);
void Ann_HideAnnouncement (void);
void Ann_UnhideAnnouncement (void);
void Ann_RemoveAnnouncement (void);
void Ann_MarkAnnouncementAsSeen (void);

#endif
