// swad_setting.h: user's settings / preferences

#ifndef _SWAD_SET
#define _SWAD_SET
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_date.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

// Related with user photo shape
#define Set_NUM_USR_PHOTOS 3
typedef enum
  {
   Set_USR_PHOTO_CIRCLE    = 0,
   Set_USR_PHOTO_ELLIPSE   = 1,
   Set_USR_PHOTO_RECTANGLE = 2,
  } Set_UsrPhotos_t;
#define Set_USR_PHOTOS_DEFAULT Set_USR_PHOTO_CIRCLE

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
unsigned Set_GetParamSideCols (void);

void Set_ChangeUsrPhotos (void);
Set_UsrPhotos_t Set_GetParamUsrPhotos (void);

//------------------------ My settings on users' list -------------------------
void Set_PutParamsPrefsAboutUsrList (void);
void Set_GetAndUpdatePrefsAboutUsrList (void);
void Set_PutParamUsrListType (Set_ShowUsrsType_t ListType);
void Set_GetAndUpdateColsClassPhoto (void);
void Set_GetMyColsClassPhotoFromDB (void);
void Set_PutParamColsClassPhoto (void);
void Set_PutParamListWithPhotos (void);
void Set_GetMyPrefAboutListWithPhotosFromDB (void);

void Set_BeginSettingsHead (void);
void Set_EndSettingsHead (void);
void Set_BeginOneSettingSelector (void);
void Set_EndOneSettingSelector (void);

#endif
