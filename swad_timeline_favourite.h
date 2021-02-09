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
                                       TL_HowManyUsrs_t HowManyUsrs);
void TL_Fav_PutFormToSeeAllFaversComment (long PubCod,
                                          TL_HowManyUsrs_t HowManyUsrs);

void TL_Fav_ShowAllFaversNoteUsr (void);
void TL_Fav_ShowAllFaversNoteGbl (void);
void TL_Fav_FavNoteUsr (void);
void TL_Fav_FavNoteGbl (void);
void TL_Fav_UnfNoteUsr (void);
void TL_Fav_UnfNoteGbl (void);
void TL_Fav_PutFormToFavUnfNote (const struct TL_Note *SocNot,
                                 TL_HowManyUsrs_t HowManyUsrs);

void TL_Fav_ShowAllFaversComUsr (void);
void TL_Fav_ShowAllFaversComGbl (void);
void TL_Fav_FavCommentUsr (void);
void TL_Fav_FavCommentGbl (void);
void TL_Fav_UnfCommentUsr (void);
void TL_Fav_UnfCommentGbl (void);
void TL_Fav_PutFormToFavUnfComment (const struct TL_Comment *SocCom,
                                    TL_HowManyUsrs_t HowManyUsrs);

void TL_Fav_GetNumTimesANoteHasBeenFav (struct TL_Note *SocNot);
void TL_Fav_GetNumTimesACommHasBeenFav (struct TL_Comment *SocCom);

#endif
