// swad_timeline_database.h: social timeline operations with database

#ifndef _SWAD_TML_DB
#define _SWAD_TML_DB
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_browser.h"
#include "swad_database.h"
#include "swad_timeline_comment.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_note.h"
#include "swad_timeline_post.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

/****************************** Timeline from who ****************************/
unsigned Tml_DB_GetWho (MYSQL_RES **mysql_res);
void Tml_DB_UpdateWho (Usr_Who_t Who);

/*********************************** Notes ***********************************/
unsigned Tml_DB_GetNoteDataByCod (long NotCod,MYSQL_RES **mysql_res);
long Tml_DB_GetPubCodOfOriginalNote (long NotCod);
long Tml_DB_CreateNewNote (TmlNot_Type_t NoteType,long Cod,
                           long PublisherCod,long HieCod);
void Tml_DB_MarkNoteAsUnavailable (TmlNot_Type_t NoteType,long Cod);
void Tml_DB_MarkNotesChildrenOfFolderAsUnavailable (TmlNot_Type_t NoteType,
                                                    Brw_FileBrowser_t FileBrowser,
                                                    long Cod,const char *Path);

unsigned Tml_DB_GetNumNotesAndUsrsByType (MYSQL_RES **mysql_res,
					  Hie_Level_t HieLvl,
                                          TmlNot_Type_t NoteType);
unsigned Tml_DB_GetNumNotesAndUsrsTotal (MYSQL_RES **mysql_res,Hie_Level_t HieLvl);

void Tml_DB_CreateTmpTableTimeline (Tml_WhatToGet_t WhatToGet);
void Tml_DB_InsertNoteInTimeline (long NotCod);
void Tml_DB_DropTmpTableTimeline (void);
void Tml_DB_ClearOldTimelinesNotesFromDB (void);
void Tml_DB_ClearTimelineNotesOfSessionFromDB (void);

void Tml_DB_RemoveNoteFavs (long NotCod);
void Tml_DB_RemoveNotePubs (long NotCod);
void Tml_DB_RemoveNote (long NotCod);
void Tml_DB_RemoveAllNotesUsr (long UsrCod);

/********************************* Posts *************************************/
unsigned Tml_DB_GetPostByCod (long PstCod,MYSQL_RES **mysql_res);
long Tml_DB_GetMedCodFromPost (long PubCod);
long Tml_DB_CreateNewPost (const struct TmlPst_Content *Content);
void Tml_DB_RemovePost (long PstCod);
void Tml_DB_RemoveAllPostsUsr (long UsrCod);

/******************************** Comments ***********************************/
unsigned Tml_DB_GetNumCommsInNote (long NotCod);
unsigned Tml_DB_GetComms (long NotCod,MYSQL_RES **mysql_res);
unsigned Tml_DB_GetInitialComms (long NotCod,unsigned NumInitialCommsToGet,
				 MYSQL_RES **mysql_res);
unsigned Tml_DB_GetFinalComms (long NotCod,unsigned NumFinalCommsToGet,
			       MYSQL_RES **mysql_res);
unsigned Tml_DB_GetCommDataByCod (long PubCod,MYSQL_RES **mysql_res);
void Tml_DB_InsertCommContent (long PubCod,
			       const struct TmlPst_Content *Content);
long Tml_DB_GetMedCodFromComm (long PubCod);
void Tml_DB_RemoveCommFavs (long PubCod);
void Tml_DB_RemoveCommContent (long PubCod);
void Tml_DB_RemoveCommPub (long PubCod);
void Tml_DB_RemoveAllCommsInAllNotesOf (long UsrCod);
void Tml_DB_RemoveAllCommsMadeBy (long UsrCod);

/****************************** Publications *********************************/
void Tml_DB_CreateSubQueryPublishers (TmlUsr_UsrOrGbl_t UsrOrGbl,Usr_Who_t Who,
                                      char **Table,
                                      char SubQuery[TmlPub_MAX_BYTES_SUBQUERY + 1]);
void Tml_DB_CreateSubQueryRange (TmlPub_Range_t Range,long PubCod,
                                 char SubQuery[TmlPub_MAX_BYTES_SUBQUERY + 1]);
unsigned Tml_DB_SelectTheMostRecentPub (MYSQL_RES **mysql_res,
                                        const struct TmlPub_SubQueries *SubQueries);
unsigned Tml_DB_GetPubDataByCod (long PubCod,MYSQL_RES **mysql_res);
long Tml_DB_GetNotCodFromPubCod (long PubCod);
long Tml_DB_GetPubCodFromSession (TmlPub_FirstLast_t FirstLast);
unsigned Tml_DB_GetNumPubsUsr (long UsrCod);
unsigned Tml_DB_GetPublishersInNoteExceptMe (MYSQL_RES **mysql_res,long PubCod);
long Tml_DB_CreateNewPub (const struct TmlPub_Publication *Pub);
void Tml_DB_UpdateFirstPubCodInSession (long FirstPubCod);
void Tml_DB_UpdateLastPubCodInSession (void);
void Tml_DB_UpdateFirstLastPubCodsInSession (long FirstPubCod);
void Tml_DB_RemoveAllPubsPublishedByAnyUsrOfNotesAuthoredBy (long UsrCod);
void Tml_DB_RemoveAllPubsPublishedBy (long UsrCod);

/****************************** Favourites ***********************************/
bool Tml_DB_CheckIfFavedByUsr (TmlUsr_FavSha_t FavSha,long Cod,long UsrCod);
unsigned Tml_DB_GetNumFavers (TmlUsr_FavSha_t FavSha,long Cod,long UsrCod);
unsigned Tml_DB_GetFavers (TmlUsr_FavSha_t FavSha,
                           long Cod,long UsrCod,unsigned MaxUsrs,
                           MYSQL_RES **mysql_res);
void Tml_DB_MarkAsFav (TmlUsr_FavSha_t FavSha,long Cod);
void Tml_DB_UnmarkAsFav (TmlUsr_FavSha_t FavSha,long Cod);
void Tml_DB_RemoveAllFavsMadeByUsr (TmlUsr_FavSha_t FavSha,long UsrCod);
void Tml_DB_RemoveAllFavsToPubsBy (TmlUsr_FavSha_t FavSha,long UsrCod);
void Tml_DB_RemoveAllFavsToAllCommsInAllNotesBy (long UsrCod);

/******************************** Shared *************************************/
bool Tml_DB_CheckIfSharedByUsr (long NotCod,long UsrCod);
unsigned Tml_DB_GetNumSharers (long NotCod,long UsrCod);
unsigned Tml_DB_GetSharers (long NotCod,long UsrCod,unsigned MaxUsrs,
                            MYSQL_RES **mysql_res);
void Tml_DB_RemoveSharedPub (long NotCod);

#endif
