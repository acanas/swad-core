// swad_notice_database.h: notices (yellow notes) operations with database

#ifndef _SWAD_NOT_DB
#define _SWAD_NOT_DB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Not_DB_InsertNotice (const char *Content,PriPub_PrivateOrPublic_t PrivateOrPublic);
void Not_DB_ChangeNoticeStatus (long NotCod,HidVis_HiddenOrVisible_t HiddenOrVisible);
void Not_DB_CopyNoticeToDeleted (long NotCod);
void Not_DB_UpdateNumUsrsNotifiedByEMailAboutNotice (long NotCod,
                                                     unsigned NumUsrsToBeNotifiedByEMail);

Exi_Exist_t Not_DB_GetNoticeData (MYSQL_RES **mysql_res,long NotCod);
Exi_Exist_t Not_DB_ContentNotice (MYSQL_RES **mysql_res,long NotCod);
unsigned Not_DB_GetNotices (MYSQL_RES **mysql_res,long CrsCod,
			    Not_Listing_t TypeNoticesListing);
unsigned Not_DB_GetNumNotices (MYSQL_RES **mysql_res,
			       Hie_Level_t HieLvl,HidVis_HiddenOrVisible_t HiddenOrVisible);
unsigned Not_DB_GetNumNoticesDeleted (MYSQL_RES **mysql_res,Hie_Level_t HieLvl);

void Not_DB_RemoveNotice (long NotCod);
void Not_DB_RemoveCrsNotices (long HieCod);

#endif
