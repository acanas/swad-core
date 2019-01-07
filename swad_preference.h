// swad_preference.h: user's preferences

#ifndef _SWAD_PRE
#define _SWAD_PRE
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Pre_EditPrefs (void);

void Pre_GetPrefsFromIP (void);
void Pre_SetPrefsFromIP (void);
void Pre_RemoveOldPrefsFromIP (void);

void Pre_ChangeSideCols (void);
void Pre_HideLeftCol (void);
void Pre_HideRightCol (void);
void Pre_ShowLeftCol (void);
void Pre_ShowRightCol (void);
unsigned Pre_GetParamSideCols (void);

#endif
