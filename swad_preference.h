// swad_preference.h: user's preferences

#ifndef _SWAD_PRF
#define _SWAD_PRF
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

void Prf_EditPrefs (void);

void Prf_GetPrefsFromIP (void);
void Prf_SetPrefsFromIP (void);
void Prf_RemoveOldPrefsFromIP (void);

void Prf_PutSelectorToSelectLanguage (void);
void Prf_AskChangeLanguage (void);
void Prf_ChangeLanguage (void);
Txt_Language_t Prf_GetParamLanguage (void);

void Prf_PutIconsToSelectSideCols (void);
void Prf_PutLeftIconToHideShowCols (void);
void Prf_PutRigthIconToHideShowCols (void);
void Prf_ChangeSideCols (void);
void Prf_HideLeftCol (void);
void Prf_HideRightCol (void);
void Prf_ShowLeftCol (void);
void Prf_ShowRightCol (void);
unsigned Prf_GetParamSideCols (void);

bool Prf_GetParamPublicPhoto (void);
void Prf_ChangePublicPhoto (void);

#endif
