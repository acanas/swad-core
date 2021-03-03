// swad_timeline_database.h: social timeline operations with database

#ifndef _SWAD_TL_DB
#define _SWAD_TL_DB
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

#include "swad_database.h"
#include "swad_file_browser.h"
#include "swad_timeline_comment.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_note.h"
#include "swad_timeline_post.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

/****************************** Timeline from who ****************************/
unsigned TL_DB_GetWho (MYSQL_RES **mysql_res);
void TL_DB_UpdateWho (Usr_Who_t Who);

/*********************************** Notes ***********************************/
unsigned TL_DB_GetDataOfNoteByCod (long NotCod,MYSQL_RES **mysql_res);
long TL_DB_GetPubCodOfOriginalNote (long NotCod);
long TL_DB_CreateNewNote (TL_Not_NoteType_t NoteType,long Cod,
                          long PublisherCod,long HieCod);
void TL_DB_MarkNoteAsUnavailable (TL_Not_NoteType_t NoteType,long Cod);
void TL_DB_MarkNotesChildrenOfFolderAsUnavailable (TL_Not_NoteType_t NoteType,
                                                   Brw_FileBrowser_t FileBrowser,
                                                   long Cod,
                                                   const char *Path);
void TL_DB_CreateTmpTableJustRetrievedNotes (void);
void TL_DB_CreateTmpTableVisibleTimeline (void);
void TL_DB_InsertNoteInJustRetrievedNotes (long NotCod);
void TL_DB_InsertNoteInVisibleTimeline (long NotCod);
void TL_DB_AddNotesJustRetrievedToVisibleTimelineOfSession (void);
void TL_DB_DropTmpTableJustRetrievedNotes (void);
void TL_DB_DropTmpTableVisibleTimeline (void);
void TL_DB_ClearOldTimelinesNotesFromDB (void);
void TL_DB_ClearTimelineNotesOfSessionFromDB (void);
void TL_DB_RemoveNoteFavs (long NotCod);
void TL_DB_RemoveNotePubs (long NotCod);
void TL_DB_RemoveNote (long NotCod);
void TL_DB_RemoveAllNotesUsr (long UsrCod);

/********************************* Posts *************************************/
unsigned TL_DB_GetPostByCod (long PstCod,MYSQL_RES **mysql_res);
long TL_DB_GetMedCodFromPost (long PubCod);
long TL_DB_CreateNewPost (const struct TL_Pst_Content *Content);
void TL_DB_RemovePost (long PstCod);
void TL_DB_RemoveAllPostsUsr (long UsrCod);

/******************************** Comments ***********************************/
unsigned TL_DB_GetNumCommsInNote (long NotCod);
unsigned TL_DB_GetComms (long NotCod,MYSQL_RES **mysql_res);
unsigned TL_DB_GetInitialComms (long NotCod,
				unsigned NumInitialCommsToGet,
				MYSQL_RES **mysql_res);
unsigned TL_DB_GetFinalComms (long NotCod,unsigned NumFinalCommsToGet,
			      MYSQL_RES **mysql_res);
unsigned TL_DB_GetDataOfCommByCod (long PubCod,MYSQL_RES **mysql_res);
void TL_DB_InsertCommContent (long PubCod,
			      const struct TL_Pst_Content *Content);
long TL_DB_GetMedCodFromComm (long PubCod);
void TL_DB_RemoveCommFavs (long PubCod);
void TL_DB_RemoveCommContent (long PubCod);
void TL_DB_RemoveCommPub (long PubCod);
void TL_DB_RemoveAllCommsInAllNotesOf (long UsrCod);
void TL_DB_RemoveAllCommsMadeBy (long UsrCod);

/****************************** Publications *********************************/
void TL_DB_CreateSubQueryPublishers (const struct TL_Timeline *Timeline,
                                     struct TL_Pub_SubQueries *SubQueries);
void TL_DB_CreateSubQueryAlreadyExists (const struct TL_Timeline *Timeline,
                                        struct TL_Pub_SubQueries *SubQueries);
void TL_DB_CreateSubQueryRangeBottom (long Bottom,struct TL_Pub_SubQueries *SubQueries);
void TL_DB_CreateSubQueryRangeTop (long Top,struct TL_Pub_SubQueries *SubQueries);
unsigned TL_DB_SelectTheMostRecentPub (const struct TL_Pub_SubQueries *SubQueries,
                                       MYSQL_RES **mysql_res);
unsigned TL_DB_GetDataOfPubByCod (long PubCod,MYSQL_RES **mysql_res);
long TL_DB_GetNotCodFromPubCod (long PubCod);
long TL_DB_GetPubCodFromSession (const char *FieldName);
unsigned long TL_DB_GetNumPubsUsr (long UsrCod);
long TL_DB_CreateNewPub (const struct TL_Pub_Publication *Pub);
void TL_DB_UpdateFirstPubCodInSession (long FirstPubCod);
void TL_DB_UpdateLastPubCodInSession (void);
void TL_DB_UpdateFirstLastPubCodsInSession (long FirstPubCod);
void TL_DB_RemoveAllPubsPublishedByAnyUsrOfNotesAuthoredBy (long UsrCod);
void TL_DB_RemoveAllPubsPublishedBy (long UsrCod);

/****************************** Favourites ***********************************/
bool TL_DB_CheckIfFavedByUsr (TL_Usr_FavSha_t FavSha,long Cod,long UsrCod);
unsigned TL_DB_GetNumFavers (TL_Usr_FavSha_t FavSha,long Cod,long UsrCod);
unsigned TL_DB_GetFavers (TL_Usr_FavSha_t FavSha,
                          long Cod,long UsrCod,unsigned MaxUsrs,
                          MYSQL_RES **mysql_res);
void TL_DB_MarkAsFav (TL_Usr_FavSha_t FavSha,long Cod);
void TL_DB_UnmarkAsFav (TL_Usr_FavSha_t FavSha,long Cod);
void TL_DB_RemoveAllFavsMadeByUsr (TL_Usr_FavSha_t FavSha,long UsrCod);
void TL_DB_RemoveAllFavsToPubsBy (TL_Usr_FavSha_t FavSha,long UsrCod);
void TL_DB_RemoveAllFavsToAllCommsInAllNotesBy (long UsrCod);

/******************************** Shared *************************************/
bool TL_DB_CheckIfSharedByUsr (long NotCod,long UsrCod);
unsigned TL_DB_GetNumSharers (long NotCod,long UsrCod);
unsigned TL_DB_GetSharers (long NotCod,long UsrCod,unsigned MaxUsrs,
                           MYSQL_RES **mysql_res);
void TL_DB_RemoveSharedPub (long NotCod);

#endif
