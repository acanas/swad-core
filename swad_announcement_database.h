// swad_announcement_database.h: Global announcements operations with database

#ifndef _SWAD_ANN_DB
#define _SWAD_ANN_DB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Ann_DB_GetAllAnnouncements (MYSQL_RES **mysql_res);
unsigned Ann_DB_GetAnnouncementsICanSee (MYSQL_RES **mysql_res);
unsigned Ann_DB_GetAnnouncementsForUnknownUsers (MYSQL_RES **mysql_res);
unsigned Ann_DB_GetAnnouncementsNotSeen (MYSQL_RES **mysql_res);

void Ann_DB_CreateAnnouncement (unsigned Roles,
                                const char *Subject,const char *Content);

void Ann_DB_ChangeAnnouncementStatus (long AnnCod,HidVis_HiddenOrVisible_t HiddenOrVisible);

void Ann_DB_RemoveUsrsWhoSawAnnouncement (long AnnCod);
void Ann_DB_RemoveAnnouncement (long AnnCod);

void Ann_DB_MarkAnnouncementAsSeenByMe (long AnnCod);

void Ann_DB_RemoveUsrFromSeenAnnouncements (long UsrCod);

#endif
