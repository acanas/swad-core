// swad_layout.h: page layout

#ifndef _SWAD_LAY
#define _SWAD_LAY
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Lay_WriteStartOfPage (void);

void Lay_WriteHTTPStatus204NoContent (void);

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

void Lay_StartSection (const char *SectionId);
void Lay_EndSection (void);

void Lay_NotEnoughMemoryExit (void);
void Lay_WrongScopeExit (void);
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
