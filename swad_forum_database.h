// swad_forum_database.h: forums operations with database

#ifndef _SWAD_FOR_DB
#define _SWAD_FOR_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_forum.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//------------------------------- Forums --------------------------------------
unsigned For_DB_GetNumThrsInForum (const struct For_Forum *Forum);
unsigned For_DB_GetNumThrsInForumNewerThan (const struct For_Forum *Forum,
                                            const char *Time);
unsigned For_DB_GetNumPstsOfUsrInForum (const struct For_Forum *Forum,
                                        long UsrCod);
void For_DB_RemoveForums (Hie_Level_t Level,long HieCod);

//------------------------------- Posts ---------------------------------------
long For_DB_InsertForumPst (long ThrCod,long UsrCod,
                            const char *Subject,const char *Content,
                            long MedCod);
void For_DB_UpdateNumUsrsNotifiedByEMailAboutPost (long PstCod,
                                                   unsigned NumUsrsToBeNotifiedByEMail);
bool For_DB_CheckIfForumPstExists (long PstCod);
unsigned For_DB_GetPstData (MYSQL_RES **mysql_res,long PstCod);
unsigned For_DB_GetPstSubjectAndContent (MYSQL_RES **mysql_res,long PstCod);
unsigned For_DB_GetThreadForumTypeAndHieCodOfAPost (MYSQL_RES **mysql_res,long PstCod);
unsigned For_DB_GetForumTypeAndHieCodOfAThread (MYSQL_RES **mysql_res,long ThrCod);
unsigned For_DB_GetNumPostsUsr (long UsrCod);
unsigned For_DB_GetPostsOfAThread (MYSQL_RES **mysql_res,long ThrCod);
unsigned For_DB_GetThreadAndNumPostsGivenPstCod (MYSQL_RES **mysql_res,long PstCod);
unsigned For_DB_GetPublishersInThreadExceptMe (MYSQL_RES **mysql_res,long PstCod);
void For_DB_RemovePst (long PstCod);
void For_DB_RemoveThreadPsts (long ThrCod);

//------------------------------ Threads --------------------------------------
long For_DB_InsertForumThread (const struct For_Forums *Forums,
                               long FirstPstCod);
void For_DB_UpdateThrFirstAndLastPst (long ThrCod,long FirstPstCod,long LastPstCod);
void For_DB_UpdateThrLastPst (long ThrCod,long LastPstCod);
unsigned For_DB_GetForumThreads (MYSQL_RES **mysql_res,
                                 const struct For_Forums *Forums);
unsigned For_DB_GetThreadData (MYSQL_RES **mysql_res,long ThrCod);
void For_DB_GetThreadTitle (long ThrCod,char *Subject,size_t TitleSize);
bool For_DB_CheckIfThrBelongsToForum (long ThrCod,const struct For_Forum *Forum);
long For_DB_GetThrLastPst (long ThrCod);
void For_DB_GetThrSubject (long ThrCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1]);
unsigned For_DB_GetNumOfWritersInThr (long ThrCod);
unsigned For_DB_GetNumPstsInThr (long ThrCod);
unsigned For_DB_GetNumMyPstsInThr (long ThrCod);
unsigned For_DB_GetNumPstsInThrNewerThan (long ThrCod,const char *Time);
void For_DB_RemoveThread (long ThrCod);

//----------------------------- Thread read -----------------------------------
void For_DB_UpdateThrReadTime (long ThrCod,
                               time_t CreatTimeUTCOfTheMostRecentPostRead);
unsigned For_DB_GetNumReadersOfThr (long ThrCod);
unsigned For_DB_GetThrReadTime (MYSQL_RES **mysql_res,long ThrCod);
unsigned For_DB_GetLastTimeIReadForum (MYSQL_RES **mysql_res,
                                       const struct For_Forum *Forum);
unsigned For_DB_GetLastTimeIReadThread (MYSQL_RES **mysql_res,long ThrCod);
void For_DB_RemoveThrFromReadThrs (long ThrCod);
void For_DB_RemoveUsrFromReadThrs (long UsrCod);

//-------------------------- Thread clipboard ---------------------------------
void For_DB_InsertThrInMyClipboard (long ThrCod);
void For_DB_MoveThrToCurrentForum (const struct For_Forums *Forums);
long For_DB_GetThrInMyClipboard (void);
void For_DB_RemoveThrFromClipboard (long ThrCod);
void For_DB_RemoveUsrFromClipboard (long UsrCod);
void For_DB_RemoveExpiredClipboards (void);

//--------------------------- Disabled posts ----------------------------------
void For_DB_InsertPstIntoDisabled (long PstCod);
Cns_DisabledOrEnabled_t For_DB_GetIfPstIsDisabledOrEnabled (long PstCod);
void For_DB_RemovePstFromDisabled (long PstCod);
void For_DB_RemoveDisabledPstsInThread (long ThrCod);

//----------------------------- Statistics ------------------------------------
unsigned For_DB_GetNumTotalForumsOfType (For_ForumType_t ForumType,
					 long HieCod[Hie_NUM_LEVELS]);
unsigned For_DB_GetNumTotalThrsInForumsOfType (For_ForumType_t ForumType,
					       long HieCod[Hie_NUM_LEVELS]);
unsigned For_DB_GetNumTotalPstsInForumsOfType (For_ForumType_t ForumType,
					       long HieCod[Hie_NUM_LEVELS],
                                               unsigned *NumUsrsToBeNotifiedByEMail);

#endif
