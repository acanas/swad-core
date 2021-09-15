// swad_forum_database.h: forums operations with database

#ifndef _SWAD_FOR_DB
#define _SWAD_FOR_DB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

// #include "swad_center.h"
// #include "swad_degree.h"
// #include "swad_institution.h"
// #include "swad_notification.h"
// #include "swad_pagination.h"
// #include "swad_scope.h"
// #include "swad_string.h"

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//------------------------------- Posts ---------------------------------------
long For_DB_InsertForumPst (long ThrCod,long UsrCod,
                            const char *Subject,const char *Content,
                            long MedCod);
bool For_DB_GetIfForumPstExists (long PstCod);
unsigned For_DB_GetThreadAndNumPostsGivenPstCod (MYSQL_RES **mysql_res,long PstCod);
void For_DB_RemovePst (long PstCod);
void For_DB_RemoveThreadPsts (long ThrCod);

//------------------------------ Threads --------------------------------------
long For_DB_InsertForumThread (const struct For_Forums *Forums,
                               long FirstPstCod);
void For_DB_GetThrSubject (long ThrCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1]);
void For_DB_RemoveThread (long ThrCod);

//--------------------------- Disabled posts ----------------------------------
void For_DB_InsertPstIntoDisabled (long PstCod);
bool For_DB_GetIfPstIsEnabled (long PstCod);
void For_DB_RemovePstFromDisabled (long PstCod);
void For_DB_RemoveDisabledPstsInThread (long ThrCod);

#endif
