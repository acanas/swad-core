// swad_icon.h: icons

#ifndef _SWAD_ICO
#define _SWAD_ICO
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

const char *Ico_GetIcon (const char *IconWithoutExtension);

void Ico_PutIconsToSelectIconSet (void);
void Ico_ChangeIconSet (void);
Ico_IconSet_t Ico_GetParamIconSet (void);
Ico_IconSet_t Ico_GetIconSetFromStr (const char *Str);

void Ico_PutContextualIconToAdd (Act_Action_t NextAction,const char *Anchor,
				 void (*FuncParams) (void),
				 const char *Txt);
void Ico_PutContextualIconToRemove (Act_Action_t NextAction,void (*FuncParams) (void));
void Ico_PutContextualIconToEdit (Act_Action_t NextAction,void (*FuncParams) (void));
void Ico_PutContextualIconToViewFiles (Act_Action_t NextAction,void (*FuncParams) (void));
void Ico_PutContextualIconToView (Act_Action_t NextAction,void (*FuncParams) (void));
void Ico_PutContextualIconToUnhide (Act_Action_t NextAction,const char *Anchor,void (*FuncParams) (void));
void Ico_PutContextualIconToHide (Act_Action_t NextAction,const char *Anchor,void (*FuncParams) (void));
void Ico_PutContextualIconToPrint (Act_Action_t NextAction,void (*FuncParams) (void));

void Ico_PutDivIcon (const char *DivClass,const char *Icon,const char *Title);
void Ico_PutDivIconLink (const char *DivClass,const char *Icon,const char *Title);
void Ico_PutIconLink (const char *Icon,const char *Title);
void Ico_PutIconTextLink (const char *Icon,const char *Text);
void Ico_PutSettingIconLink (const char *Icon,const char *Title);
void Ico_PutIconOff (const char *Icon,const char *Title);

void Ico_PutCalculateIcon (const char *Title);
void Ico_PutCalculateIconWithText (const char *Text);

void Ico_PutIconRemovalNotAllowed (void);
void Ico_PutIconRemove (void);

void Ico_PutIconCut (void);
void Ico_PutIconPaste (void);

#endif
