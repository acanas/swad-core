// swad_timeline_favourite.h: social timeline favourites

#ifndef _SWAD_TL_FAV
#define _SWAD_TL_FAV
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

#include "swad_timeline_comment.h"
#include "swad_timeline_note.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TL_Fav_PutFormToSeeAllFaversNote (long NotCod,
                                       TL_Usr_HowManyUsrs_t HowManyUsrs);
void TL_Fav_PutFormToSeeAllFaversComment (long PubCod,
                                          TL_Usr_HowManyUsrs_t HowManyUsrs);

void TL_Fav_ShowAllFaversNoteUsr (void);
void TL_Fav_ShowAllFaversNoteGbl (void);
void TL_Fav_FavNoteUsr (void);
void TL_Fav_FavNoteGbl (void);
void TL_Fav_UnfNoteUsr (void);
void TL_Fav_UnfNoteGbl (void);
void TL_Fav_PutIconToFavUnfNote (const struct TL_Not_Note *Not,
                                 TL_Usr_HowManyUsrs_t HowManyUsrs);

void TL_Fav_ShowAllFaversComUsr (void);
void TL_Fav_ShowAllFaversComGbl (void);
void TL_Fav_FavCommentUsr (void);
void TL_Fav_FavCommentGbl (void);
void TL_Fav_UnfCommentUsr (void);
void TL_Fav_UnfCommentGbl (void);
void TL_Fav_PutIconToFavUnfComment (const struct TL_Com_Comment *Com,
                                    TL_Usr_HowManyUsrs_t HowManyUsrs);

void TL_Fav_GetNumTimesANoteHasBeenFav (struct TL_Not_Note *Not);
void TL_Fav_GetNumTimesACommHasBeenFav (struct TL_Com_Comment *Com);

#endif
