// swad_layout.h: page layout

#ifndef _SWAD_LAY
#define _SWAD_LAY
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

// Colors used in headers, backgrounds, etc.
#define YELLOW_BACKGROUND_MSG	"#FFFDB9"
#define DARK_GRAY		"#404040"
#define DARK_BLUE		"#8CBCCE"
#define MEDIUM_BLUE		"#AFD1DD"
#define LIGHT_BLUE		"#C0DAE4"
#define VERY_LIGHT_BLUE		"#DDECF1"
#define LIGHTEST_BLUE		"#EAF3F6"
#define LIGHT_GREEN		"#C0FF40"
#define LIGHTEST_GREEN		"#F5FFD7"

#define Lay_MAX_BYTES_ALERT (16*1024)		// Max. size for alert message
// Important: the size of alert message must be enough large to store the longest message.

#define Lay_MAX_BYTES_TITLE 1024	// Max. size for window status message

#define Lay_HIDE_BOTH_COLUMNS	0						// 00
#define Lay_SHOW_RIGHT_COLUMN	1						// 01
#define Lay_SHOW_LEFT_COLUMN	2						// 10
#define Lay_SHOW_BOTH_COLUMNS (Lay_SHOW_LEFT_COLUMN | Lay_SHOW_RIGHT_COLUMN)	// 11

/*****************************************************************************/
/********************************* Public types ******************************/
/*****************************************************************************/

#define Lay_NUM_ALERT_TYPES 4
typedef enum
  {
   Lay_INFO    = 0,
   Lay_SUCCESS = 1,
   Lay_WARNING = 2,
   Lay_ERROR   = 3,
  } Lay_AlertType_t;

#define Lay_NUM_LAYOUTS 2
typedef enum
  {
   Lay_LAYOUT_DESKTOP = 0,
   Lay_LAYOUT_MOBILE  = 1,
   Lay_LAYOUT_UNKNOWN = 2,
  } Lay_Layout_t;	// Stored in database. Don't change numbers!

#define Lay_LAYOUT_DEFAULT Lay_LAYOUT_DESKTOP

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

// void Lay_PrintDigest512 (unsigned char digest512[512/8]);

void Lay_WriteStartOfPage (void);
void Lay_WriteTitle (const char *Title);

void Lay_PutFormToEdit (Act_Action_t Action);
void Lay_PutSendIcon (const char *Icon,const char *Alt,const char *Text);
void Lay_PutSendButton (const char *TextSendButton);
void Lay_StartRoundFrameTable10 (const char *Width,unsigned CellPadding,const char *Title);
void Lay_StartRoundFrameTable10Shadow (const char *Width,unsigned CellPadding);
void Lay_EndRoundFrameTable10 (void);
void Lay_ShowErrorAndExit (const char *Message);
void Lay_ShowAlert (Lay_AlertType_t MsgType,const char *Message);
void Lay_RefreshNotifsAndConnected (void);
void Lay_RefreshLastClicks (void);
void Lay_WritePageFooter (void);

void Lay_WriteHeaderClassPhoto (unsigned NumColumns,bool PrintView,bool DrawingClassPhoto,
                                long InsCod,long DegCod,long CrsCod);

void Lay_PutIconsToSelectLayout (void);
void Lay_ChangeLayout (void);
Lay_Layout_t Lay_GetParamLayout (void);
Lay_Layout_t Lay_GetLayoutFromStr (const char *Str);

void Lay_PutLinkToPrintView1 (Act_Action_t NextAction);
void Lay_PutLinkToPrintView2 (void);

void Lay_AdvertisementMobile (void);

void Lay_IndentDependingOnLevel (unsigned Level,bool IsLastItemInLevel[]);

#endif
