// swad_timeline_favourite.h: social timeline favourites

#ifndef _SWAD_TML_FAV
#define _SWAD_TML_FAV
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
/************************ Public constants and types *************************/
/*****************************************************************************/

#define TmlFav_ICON_FAV	"heart.svg"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TmlFav_ShowAllFaversNoteUsr (void);
void TmlFav_ShowAllFaversNoteGbl (void);
void TmlFav_FavNoteUsr (void);
void TmlFav_FavNoteGbl (void);
void TmlFav_UnfNoteUsr (void);
void TmlFav_UnfNoteGbl (void);

void TmlFav_ShowAllFaversComUsr (void);
void TmlFav_ShowAllFaversComGbl (void);
void TmlFav_FavCommUsr (void);
void TmlFav_FavCommGbl (void);
void TmlFav_UnfCommUsr (void);
void TmlFav_UnfCommGbl (void);

#endif
