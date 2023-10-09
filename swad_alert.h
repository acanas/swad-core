// swad_alert.h: alerts

#ifndef _SWAD_ALE
#define _SWAD_ALE
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_button.h"
#include "swad_layout.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Ale_MAX_ALERTS	10		// Max. number of alerts stored before of being shown

/*****************************************************************************/
/********************************* Public types ******************************/
/*****************************************************************************/

#define Ale_NUM_ALERT_TYPES 7
typedef enum
  {
   Ale_NONE      = 0,
   Ale_CLIPBOARD = 1,
   Ale_INFO      = 2,
   Ale_SUCCESS   = 3,
   Ale_QUESTION  = 4,
   Ale_WARNING   = 5,
   Ale_ERROR     = 6,
  } Ale_AlertType_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ale_CreateAlert (Ale_AlertType_t Type,const char *Section,
                      const char *fmt,...);

size_t Ale_GetNumAlerts (void);
Ale_AlertType_t Ale_GetTypeOfLastAlert (void);
const char *Ale_GetTextOfLastAlert (void);

void Ale_ResetAllAlerts (void);

void Ale_ShowAlertsAndExit ();
void Ale_ShowAlerts (const char *Section);
void Ale_ShowAlert (Ale_AlertType_t AlertType,const char *fmt,...);
void Ale_ShowLastAlertAndButton (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                                 void (*FuncPars) (void *Args),void *Args,
				 Btn_Button_t Button,const char *TxtButton);
void Ale_ShowLastAlertAndButton1 (void);
void Ale_ShowAlertRemove (Act_Action_t NextAction,const char *Anchor,
                          void (*FuncPars) (void *Args),void *Args,
			  const char *Txt,const char *Title);
void Ale_ShowAlertAndButton (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                             void (*FuncPars) (void *Args),void *Args,
                             Btn_Button_t Button,const char *TxtButton,
			     Ale_AlertType_t AlertType,const char *fmt,...);
void Ale_ShowAlertAndButton1 (Ale_AlertType_t AlertType,const char *fmt,...);
void Ale_ShowAlertAndButton2 (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                              void (*FuncPars) (void *Args),void *Args,
                              Btn_Button_t Button,const char *TxtButton);

void Ale_ShowAlertYouCanNotLeaveFieldEmpty (void);
void Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission (void);
void Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission (void);
void Ale_CreateAlertYouCanNotLeaveFieldEmpty (void);
void Ale_CreateAlertYouMustSpecifyTheTitle (void);
void Ale_CreateAlertYouMustSpecifyTheWebAddress (void);
void Ale_CreateAlertYouMustSpecifyTheName (void);
void Ale_CreateAlertYouMustSpecifyShrtNameAndFullName (void);

#endif
