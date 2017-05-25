// swad_layout.h: page layout

#ifndef _SWAD_LAY
#define _SWAD_LAY
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

#include "swad_action.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Lay_MAX_BYTES_TITLE (4096 - 1)

#define Lay_HIDE_BOTH_COLUMNS	0						// 00
#define Lay_SHOW_RIGHT_COLUMN	1						// 01
#define Lay_SHOW_LEFT_COLUMN	2						// 10
#define Lay_SHOW_BOTH_COLUMNS (Lay_SHOW_LEFT_COLUMN | Lay_SHOW_RIGHT_COLUMN)	// 11

/*****************************************************************************/
/********************************* Public types ******************************/
/*****************************************************************************/

typedef enum
  {
   Lay_NO_BUTTON,
   Lay_CREATE_BUTTON,
   Lay_CONFIRM_BUTTON,
   Lay_REMOVE_BUTTON,
  } Lay_Button_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Lay_WriteStartOfPage (void);

void Lay_PutContextualIconToRemove (Act_Action_t NextAction,void (*FuncParams) ());
void Lay_PutContextualIconToEdit (Act_Action_t NextAction,void (*FuncParams) ());
void Lay_PutContextualIconToView (Act_Action_t NextAction,void (*FuncParams) ());
void Lay_PutContextualIconToUnhide (Act_Action_t NextAction,void (*FuncParams) ());
void Lay_PutContextualIconToHide (Act_Action_t NextAction,void (*FuncParams) ());
void Lay_PutContextualIconToPrint (Act_Action_t NextAction,void (*FuncParams) ());
void Lay_PutContextualLink (Act_Action_t NextAction,const char *Anchor,
                            void (*FuncParams) (),
                            const char *Icon,
                            const char *Title,const char *Text,
                            const char *OnSubmit);
void Lay_PutContextualCheckbox (Act_Action_t NextAction,
                                void (*FuncParams) (),
                                const char *CheckboxName,
                                bool Checked,bool Disabled,
                                const char *Title,const char *Text);
void Lay_PutIconLink (const char *Icon,const char *Title,const char *Text,
                      const char *LinkStyle,const char *OnSubmit);
void Lay_PutIconWithText (const char *Icon,const char *Alt,const char *Text);
void Lay_PutCalculateIcon (const char *Alt);
void Lay_PutCalculateIconWithText (const char *Alt,const char *Text);

void Lay_PutIconRemovalNotAllowed (void);
void Lay_PutIconBRemovalNotAllowed (void);
void Lay_PutIconRemove (void);

void Lay_PutButton (Lay_Button_t Button,const char *TxtButton);
void Lay_PutCreateButton (const char *TxtButton);
void Lay_PutCreateButtonInline (const char *TxtButton);
void Lay_PutConfirmButton (const char *TxtButton);
void Lay_PutConfirmButtonInline (const char *TxtButton);
void Lay_PutRemoveButton (const char *TxtButton);
void Lay_PutRemoveButtonInline (const char *TxtButton);

void Lay_StartRoundFrameTable (const char *Width,const char *Title,
                               void (*FunctionToDrawContextualIcons) (void),
                               const char *HelpLink,
                               unsigned CellPadding);
void Lay_StartRoundFrame (const char *Width,const char *Title,
                          void (*FunctionToDrawContextualIcons) (void),
                          const char *HelpLink);
void Lay_StartRoundFrameShadow (const char *Width,const char *Title,
                                void (*FunctionToDrawContextualIcons) (void),
                                const char *HelpLink);
void Lay_StartRoundFrameTableShadow (const char *Width,const char *Title,
                                     void (*FunctionToDrawContextualIcons) (void),
                                     const char *HelpLink,
                                     unsigned CellPadding);
void Lay_EndRoundFrameTable (void);
void Lay_EndRoundFrameTableWithButton (Lay_Button_t Button,const char *TxtButton);
void Lay_EndRoundFrameWithButton (Lay_Button_t Button,const char *TxtButton);
void Lay_EndRoundFrame (void);

void Lay_StartTable (unsigned CellPadding);
void Lay_StartTableCenter (unsigned CellPadding);
void Lay_StartTableWide (unsigned CellPadding);
void Lay_StartTableWideMargin (unsigned CellPadding);
void Lay_EndTable (void);

void Lay_StartSection (const char *SectionId);
void Lay_EndSection (void);

void Lay_ShowErrorAndExit (const char *Txt);

void Lay_RefreshNotifsAndConnected (void);
void Lay_RefreshLastClicks (void);

void Lay_WriteHeaderClassPhoto (bool PrintView,bool DrawingClassPhoto,
                                long InsCod,long DegCod,long CrsCod);

void Lay_AdvertisementMobile (void);

void Lay_IndentDependingOnLevel (unsigned Level,bool IsLastItemInLevel[]);

void Lay_HelpPlainEditor (void);
void Lay_HelpRichEditor (void);

void Lay_StartHTMLFile (FILE *File,const char *Title);

#endif
