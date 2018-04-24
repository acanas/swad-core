// swad_announcement.h: Global announcements

#ifndef _SWAD_ANN
#define _SWAD_ANN
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define Ann_NUM_STATUS 2
typedef enum
  {
   Ann_ACTIVE_ANNOUNCEMENT   = 0,
   Ann_OBSOLETE_ANNOUNCEMENT = 1,
  } Ann_Status_t;	// Don't change these numbers because they are used in database

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ann_ShowAllAnnouncements (void);
void Ann_ShowMyAnnouncementsNotMarkedAsSeen (void);

void Ann_ShowFormAnnouncement (void);
void Ann_ReceiveAnnouncement (void);
void Ann_HideActiveAnnouncement (void);
void Ann_RevealHiddenAnnouncement (void);
void Ann_RemoveAnnouncement (void);
void Ann_MarkAnnouncementAsSeen (void);
void Ann_RemoveUsrFromSeenAnnouncements (long UsrCod);

#endif
