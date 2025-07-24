// swad_action.h: actions

#ifndef _SWAD_ACT
#define _SWAD_ACT
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_action_list.h"
#include "swad_hierarchy_type.h"
#include "swad_tab.h"
#include "swad_user_type.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Act_MAX_CHARS_ACTION_TXT	(256 - 1)			// 255
#define Act_MAX_BYTES_ACTION_TXT	Act_MAX_CHARS_ACTION_TXT	// 255

#define Act_MAX_OPTIONS_IN_MENU_PER_TAB	  13

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define Act_NUM_CONTENTS 2
typedef enum
  {
   Act_NORM,
   Act_DATA,
  } Act_Content_t;

typedef enum
  {
   Act_UNK,	// Unknown tab
   Act_1ST,	// The main (original, first) tab in the browser
   Act_NEW,	// A new (second) blank tab in the browser
   Act_2ND,	// The second tab in the browser
   Act_AJA,	// Update a zone of the page using AJAX
   Act_REF,	// Update a zone of the page using AJAX, with auto-refresh
   Act_UPL,	// Upload a file. Do not write HTML content. Write Status code instead for Dropzone.js
   Act_DWN,	// Download a file in a new tab. Do not write HTML content.
   Act_204,	// Do not write HTML content. HTTP will return Status 204 No Content
   Act_SVC,	// Web service. Send output to client using SOAP.
  } Act_BrowserTab_t;

struct Act_Actions
  {
   long ActCod;	// Unique, time-persistent numerical code for the action
   unsigned IndexInMenu;
   Tab_Tab_t Tab;
   void (*FunctionPriori) (void);
   void (*FunctionPosteriori) (void);
   unsigned Permission[Hie_NUM_LEVELS][Usr_NUM_BELONG];
   Act_Content_t ContentType;
   Act_BrowserTab_t BrowserTab;
  };

struct Act_ActionFunc
  {
   Act_Action_t NextAction;
   void (*FuncPars) (void *Args);
   void *Args;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Act_Action_t Act_GetActionFromActCod (long ActCod);
long Act_GetActCod (Act_Action_t Action);
unsigned Act_GetIndexInMenu (Act_Action_t Action);
Tab_Tab_t Act_GetTab (Act_Action_t Action);
Act_Action_t Act_GetSuperAction (Act_Action_t Action);
Usr_Can_t Act_CheckIfICanExecuteAction (Act_Action_t Action);
Act_Content_t Act_GetContentType (Act_Action_t Action);
Act_BrowserTab_t Act_GetBrowserTab (Act_Action_t Action);
void (*Act_GetFunctionPriori (Act_Action_t Action)) (void);
void (*Act_GetFunctionPosteriori (Act_Action_t Action)) (void);

const char *Act_GetIconFromAction (Act_Action_t Action);

const char *Act_GetTitleAction (Act_Action_t Action);
const char *Act_GetActionText (Act_Action_t Action);

void Act_AdjustActionWhenNoUsrLogged (void);
void Act_AdjustActionWhenClickOnMenu (void);
void Act_AdjustCurrentAction (void);

#endif
