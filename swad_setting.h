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

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Set_EditSettings (void);

void Set_GetSettingsFromIP (void);
void Set_SetSettingsFromIP (void);
void Set_RemoveOldSettingsFromIP (void);

void Set_ChangeSideCols (void);
unsigned Set_GetParamSideCols (void);

void Set_DB_UpdateMySettingsAboutDateFormat (Dat_Format_t DateFormat);
void Set_DB_ChangeFirstDayOfWeek (unsigned FirstDayOfWeek);
void Set_DB_ChangeMenu (Mnu_Menu_t Menu);

void Set_BeginSettingsHead (void);
void Set_EndSettingsHead (void);
void Set_BeginOneSettingSelector (void);
void Set_EndOneSettingSelector (void);

void Set_DB_InsertUsrInCrsSettings (long UsrCod,long CrsCod);

void Set_DB_UpdateGrpLastAccZone (const char *FieldNameDB,long GrpCod);

void Set_DB_RemUsrFromCrsSettings (long UsrCod,long CrsCod);
void Set_DB_RemUsrFromAllCrssSettings (long UsrCod);
void Set_DB_RemAllUsrsFromCrsSettings (long CrsCod);

#endif
