// swad_icon.h: icons

#ifndef _SWAD_ICO
#define _SWAD_ICO
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#define Ico_NUM_ICON_SETS 2

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

typedef enum
  {
   Ico_ICON_SET_AWESOME = 0,
   Ico_ICON_SET_NUVOLA  = 1,
   } Ico_IconSet_t;
#define Ico_ICON_SET_DEFAULT Ico_ICON_SET_AWESOME

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ico_PutIconsToSelectIconSet (void);
void Ico_ChangeIconSet (void);
Ico_IconSet_t Ico_GetParamIconSet (void);
Ico_IconSet_t Ico_GetIconSetFromStr (const char *Str);

void Ico_PutContextualIconToRemove (Act_Action_t NextAction,void (*FuncParams) ());
void Ico_PutContextualIconToEdit (Act_Action_t NextAction,void (*FuncParams) ());
void Ico_PutContextualIconToView (Act_Action_t NextAction,void (*FuncParams) ());
void Ico_PutContextualIconToUnhide (Act_Action_t NextAction,void (*FuncParams) ());
void Ico_PutContextualIconToHide (Act_Action_t NextAction,void (*FuncParams) ());
void Ico_PutContextualIconToPrint (Act_Action_t NextAction,void (*FuncParams) ());

void Ico_PutIconLink (const char *Icon,const char *Title,const char *Text,
                      const char *LinkStyle,const char *OnSubmit);
void Ico_PutIcon (const char *Icon,const char *Alt);
void Ico_PutIconWithText (const char *Icon,const char *Alt,const char *Text);

void Ico_PutCalculateIcon (const char *Alt);
void Ico_PutCalculateIconWithText (const char *Alt,const char *Text);

void Ico_PutIconRemovalNotAllowed (void);
void Ico_PutIconRemove (void);

#endif
