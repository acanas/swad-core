// swad_privacy.h: users' photo and public profile visibility

#ifndef _SWAD_PRI
#define _SWAD_PRI
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

#include "swad_privacy_visibility_type.h"
#include "swad_user.h"

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

/***** Visibility (who can see user's photo / public profile) *****/
/* Allowed visibility for photo */
#define Pri_PHOTO_ALLOWED_VIS			(1 << Pri_VISIBILITY_USER)   | \
						(1 << Pri_VISIBILITY_COURSE) | \
						(1 << Pri_VISIBILITY_SYSTEM) | \
						(1 << Pri_VISIBILITY_WORLD)
/* Allowed visibility for basic profile */
#define Pri_BASIC_PROFILE_ALLOWED_VIS		(1 << Pri_VISIBILITY_SYSTEM) | \
						(1 << Pri_VISIBILITY_WORLD)
/* Allowed visibility for extended profile */
#define Pri_EXTENDED_PROFILE_ALLOWED_VIS	(1 << Pri_VISIBILITY_USER)   | \
						(1 << Pri_VISIBILITY_COURSE) | \
						(1 << Pri_VISIBILITY_SYSTEM) | \
						(1 << Pri_VISIBILITY_WORLD)
/* Allowed visibility for timeline */
#define Pri_TIMELINE_ALLOWED_VIS		(1 << Pri_VISIBILITY_SYSTEM)

#define Pri_PHOTO_VIS_DEFAULT			Pri_VISIBILITY_SYSTEM
#define Pri_BASIC_PROFILE_VIS_DEFAULT		Pri_VISIBILITY_SYSTEM
#define Pri_EXTENDED_PROFILE_VIS_DEFAULT	Pri_VISIBILITY_SYSTEM

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Pri_EditMyPrivacy (void);

Pri_Visibility_t Pri_GetVisibilityFromStr (const char *Str);
Pri_Visibility_t Pri_GetParVisibility (const char *ParName,
                                       unsigned MaskAllowedVisibility);

Usr_Can_t Pri_CheckIfICanView (Pri_Visibility_t Visibility,struct Usr_Data *UsrDat);

//-------------------------------- Figures ------------------------------------
void Pri_GetAndShowNumUsrsPerPrivacy (void);

#endif
