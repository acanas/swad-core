// swad_alert.h: alerts

#ifndef _SWAD_ALE
#define _SWAD_ALE
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

#include "swad_action.h"
#include "swad_button.h"
#include "swad_layout.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Ale_MAX_BYTES_ALERT (16 * 1024 - 1)	// Max. size for alert message
// Important: the size of alert message must be enough large to store the longest message.

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

void Ale_ResetAlert (void);

void Ale_ShowPendingAlert (void);
void Ale_ShowDelayedAlert (void);
void Ale_ShowAlert (Ale_AlertType_t AlertType,const char *fmt,...);
void Ale_ShowAlertAndButton (Ale_AlertType_t AlertType,const char *Txt,
                             Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                             void (*FuncParams) (),
                             Btn_Button_t Button,const char *TxtButton);
void Ale_ShowAlertAndButton1 (Ale_AlertType_t AlertType,const char *Txt);
void Ale_ShowAlertAndButton2 (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit,
                              void (*FuncParams) (),
                              Btn_Button_t Button,const char *TxtButton);

#endif
