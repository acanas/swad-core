// swad_timeline_user.h: social timeline users

#ifndef _SWAD_TL_USR
#define _SWAD_TL_USR
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

#include "swad_form.h"
#include "swad_media.h"
#include "swad_notification.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define TL_Usr_ICON_ELLIPSIS	"ellipsis-h.svg"

#define TL_Usr_DEF_USRS_SHOWN	5	// Default maximum number of users shown who have share/fav a note
#define TL_Usr_MAX_USRS_SHOWN	1000	// Top     maximum number of users shown who have share/fav a note

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   TL_Usr_TIMELINE_USR,		// Show the timeline of a user
   TL_Usr_TIMELINE_GBL,		// Show the timeline of the users follwed by me
  } TL_Usr_UsrOrGbl_t;

typedef enum
  {
   TL_Usr_SHOW_FEW_USRS,	// Show a few first favers/sharers
   TL_Usr_SHOW_ALL_USRS,	// Show all favers/sharers
  } TL_Usr_HowManyUsrs_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TL_Usr_RemoveUsrContent (long UsrCod);

void TL_Usr_ShowNumSharersOrFavers (unsigned NumUsrs);
void TL_Usr_ShowSharersOrFavers (MYSQL_RES **mysql_res,
			         unsigned NumUsrs,unsigned NumFirstUsrs);
void TL_Usr_PutFormToSeeAllFaversSharers (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		                          const char *ParamFormat,long ParamCod,
                                          TL_Usr_HowManyUsrs_t HowManyUsrs);
void TL_Usr_FormFavSha (Act_Action_t ActionGbl,Act_Action_t ActionUsr,
		        const char *ParamFormat,long ParamCod,
		        const char *Icon,const char *Title);

#endif
