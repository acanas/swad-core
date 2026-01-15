// swad_layout.h: page layout

#ifndef _SWAD_LAY
#define _SWAD_LAY
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
#include "swad_HTML.h"
#include "swad_icon.h"
#include "swad_view.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Lay_HIDE_BOTH_COLUMNS	0						// 00
#define Lay_SHOW_RIGHT_COLUMN	1						// 01
#define Lay_SHOW_LEFT_COLUMN	2						// 10
#define Lay_SHOW_BOTH_COLUMNS (Lay_SHOW_LEFT_COLUMN | Lay_SHOW_RIGHT_COLUMN)	// 11

// Used to avoid writing more than once the HTML head or end when aborting program on error
// Respect the order, because from one state you move to the next
typedef enum
  {
   Lay_NOTHING_WRITTEN,
   Lay_WRITING_HTML_START,
   Lay_HTML_START_WRITTEN,
   Lay_DIVS_END_WRITTEN,
   Lay_HTML_END_WRITTEN,
  } Lay_LayoutStatus_t;

#define Lay_NUM_LAST 2
typedef enum
  {
   Lay_NO_LAST,
   Lay_LAST,
  } Lay_Last_t;

#define Lay_NUM_HORIZONTAL_LINE_AT_RIGHT 2
typedef enum
  {
   Lay_NO_HORIZONTAL_LINE_AT_RIGHT,
   Lay_HORIZONTAL_LINE_AT_RIGHT,
  } Lay_HorizontalLineAtRight_t;

#define Lay_NUM_SHOW 2
typedef enum
  {
   Lay_DONT_SHOW,
   Lay_SHOW,
  } Lay_Show_t;

typedef enum
  {
   Lay_SHOWING_ONLY_ONE,
   Lay_SHOWING_SEVERAL
  } Lay_ShowingOneOrSeveral_t;

#define Lay_NUM_HIGHLIGHT 2
typedef enum
  {
   Lay_DONT_HIGHLIGHT,
   Lay_HIGHLIGHT,
  } Lay_Highlight_t;

#define Lay_NUM_SPACES 2
typedef enum
  {
   Lay_NORMAL_SPACES,
   Lay_NON_BR_SPACES,
  } Lay_Spaces_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Lay_SetLayoutStatus (Lay_LayoutStatus_t LayoutStatus);
Lay_LayoutStatus_t Lay_GetLayoutStatus (void);

void Lay_WriteStartOfPage (void);

void Lay_WriteHTTPStatus204NoContent (void);

void Lay_WriteEndOfPage (void);

void Lay_PutContextualLinkOnlyIcon (Act_Action_t NextAction,const char *Anchor,
				    void (*FuncPars) (void *Args),void *Args,
				    const char *Icon,Ico_Color_t Color);
void Lay_PutContextualLinkIconText (Act_Action_t NextAction,const char *Anchor,
				    void (*FuncPars) (void *Args),void *Args,
				    const char *Icon,Ico_Color_t Color,
				    const char *Text,const char *OnSubmit);
void Lay_PutContextualCheckbox (Act_Action_t NextAction,
				void (*FuncPars) (void *Args),void *Args,
                                const char *CheckboxName,
                                HTM_Attributes_t Attributes,
                                const char *Title,const char *Text);

void Lay_WriteAboutZone (void);

void Lay_RefreshRightColumn (void);
void Lay_RefreshLastClicks (void);

void Lay_WriteHeaderClassPhoto (Hie_Level_t HieLvl,Vie_ViewType_t ViewType);

void Lay_AdvertisementMobile (void);

void Lay_IndentDependingOnLevel (unsigned Level,
				 const Lay_Last_t *IsLastItemInLevel,
				 Lay_HorizontalLineAtRight_t HorizontalLineAtRight);

void Lay_HelpPlainEditor (void);
void Lay_HelpRichEditor (void);

void Lay_BeginHTMLFile (FILE *File,const char *Title);

void Lay_WriteIndex (unsigned Index,const char *Class);
void Lay_WriteLinkToUpdate (const char *Txt,const char *OnSubmit);

//--------------------------- Show / don't show -------------------------------
Lay_Show_t Lay_GetShowFromYN (char Ch);
Lay_Show_t Lay_GetParShow (const char *ParName);
Lay_Show_t Lay_ToggleShow (Lay_Show_t Show);

//-------------------------------- Figures ------------------------------------
void Lay_GetAndShowNumUsrsPerSideColumns (Hie_Level_t HieLvl);

#endif
