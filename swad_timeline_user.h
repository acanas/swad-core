// swad_timeline_user.h: social timeline users

#ifndef _SWAD_TML_USR
#define _SWAD_TML_USR
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_form.h"
#include "swad_media.h"
#include "swad_notification.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define TmlUsr_DEF_USRS_SHOWN	5	// Default maximum number of users shown who have share/fav a note
#define TmlUsr_MAX_USRS_SHOWN	1000	// Top     maximum number of users shown who have share/fav a note

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   TmlUsr_TIMELINE_USR,	// Show the timeline of a user
   TmlUsr_TIMELINE_GBL,	// Show the timeline of the users follwed by me
  } TmlUsr_UsrOrGbl_t;

#define TmlUsr_NUM_HOW_MANY_USRS 2
typedef enum
  {
   TmlUsr_SHOW_FEW_USRS,	// Show a few first favers/sharers
   TmlUsr_SHOW_ALL_USRS,	// Show all favers/sharers
  } TmlUsr_HowManyUsrs_t;

#define TmlUsr_NUM_FAV_SHA 3
typedef enum
  {
   TmlUsr_FAV_UNF_NOTE = 0,
   TmlUsr_FAV_UNF_COMM = 1,
   TmlUsr_SHA_UNS_NOTE = 2,
  } TmlUsr_FavSha_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TmlUsr_RemoveUsrContent (long UsrCod);

Exi_Exist_t TmlUsr_CheckIfFavedSharedByUsr (TmlUsr_FavSha_t FavSha,
					    long Cod,long UsrCod);

void TmlUsr_PutIconFavSha (TmlUsr_FavSha_t FavSha,
                           long Cod,long UsrCod,unsigned NumUsrs,
                           TmlUsr_HowManyUsrs_t HowManyUsrs);

Usr_Can_t TmlUsr_CheckIfICanFavSha (long Cod,long UsrCod);
Usr_Can_t TmlUsr_CheckIfICanRemove (long Cod,long UsrCod);

#endif
