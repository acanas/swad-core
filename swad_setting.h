// swad_setting.h: user's settings / preferences

#ifndef _SWAD_SET
#define _SWAD_SET
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_date.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

// Related with type of list of users
#define Set_NUM_USR_LIST_TYPES 3
typedef enum
  {
   Set_USR_LIST_UNKNOWN        = 0,
   Set_USR_LIST_AS_CLASS_PHOTO = 1,
   Set_USR_LIST_AS_LISTING     = 2,
  } Set_ShowUsrsType_t;
#define Set_SHOW_USRS_TYPE_DEFAULT Set_USR_LIST_AS_CLASS_PHOTO

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Set_EditSettings (void);

void Set_GetSettingsFromIP (void);
void Set_SetSettingsFromIP (void);

void Set_ChangeSideCols (void);
unsigned Set_GetParSideCols (void);
unsigned Set_GetSideColsFromStr (const char *Str);

//------------------------ My settings on users' list -------------------------
void Set_GetAndUpdatePrefsAboutUsrList (bool *WithPhotos);
void Set_PutParUsrListType (Set_ShowUsrsType_t ListType);
unsigned Set_GetColsClassPhoto (void);
void Set_PutParListWithPhotos (bool WithPhotos);
void Set_PutParsPrefsAboutUsrList (void);
bool Set_GetMyPrefAboutListWithPhotosFromDB (void);

void Set_BeginSettingsHead (void);
void Set_EndSettingsHead (void);
void Set_BeginOneSettingSelector (void);
void Set_EndOneSettingSelector (void);
void Set_BeginPref (bool Condition);
void Set_EndPref (void);

#endif
