// swad_form.h: forms to go to actions

#ifndef _SWAD_FRM
#define _SWAD_FRM
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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

#include <stdbool.h>		// For boolean type

#include "swad_action_list.h"
#include "swad_button.h"
#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_parameter_code.h"

/*****************************************************************************/
/************************** Public constants and types ***********************/
/*****************************************************************************/

#define Frm_MAX_BYTES_ID (32 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 10)

#define Frm_MAX_BYTES_PARAM_ACTION	256
#define Frm_MAX_BYTES_PARAM_SESSION	(256 + Cns_BYTES_SESSION_ID)
#define Frm_MAX_BYTES_PARAM_LOCATION	256
#define Frm_MAX_BYTES_PARAMS_STR	(Frm_MAX_BYTES_PARAM_ACTION + Frm_MAX_BYTES_PARAM_SESSION + Frm_MAX_BYTES_PARAM_LOCATION)

#define Frm_NUM_PUT_FORM 2
typedef enum
  {
   Frm_DONT_PUT_FORM,
   Frm_PUT_FORM,
  } Frm_PutForm_t;

typedef enum
  {
   Frm_DONT_PUT_PAR_LOCATION_IF_NO_SESSION,
   Frm_PUT_PAR_LOCATION_IF_NO_SESSION,
  } Frm_PutParLocation_t;

typedef enum
  {
   Frm_OUTSIDE_FORM,
   Frm_INSIDE_FORM,
  } Frm_Inside_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Frm_Inside_t Frm_CheckIfInside (void);

void Frm_BeginFormGoTo (Act_Action_t NextAction);
void Frm_BeginForm (Act_Action_t NextAction);
void Frm_BeginFormAnchor (Act_Action_t NextAction,const char *Anchor);
void Frm_BeginFormOnSubmit (Act_Action_t NextAction,const char *OnSubmit);
void Frm_BeginFormAnchorOnSubmit (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit);
void Frm_BeginFormId (Act_Action_t NextAction,const char *Id);
void Frm_BeginFormIdAnchor (Act_Action_t NextAction,const char *Id,const char *Anchor);
void Frm_BeginFormNoAction (void);
void Frm_SetParsForm (char ParsStr[Frm_MAX_BYTES_PARAMS_STR + 1],
		      Act_Action_t NextAction,
                      Frm_PutParLocation_t PutParLocation);
void Frm_EndForm (void);

void Frm_BeginFormTable (Act_Action_t NextAction,const char *Anchor,
                         void (*FuncPars) (void *Args),void *Args,
                         const char *ClassTable);
void Frm_EndFormTable (Btn_Button_t Button);

void Frm_SetUniqueId (char UniqueId[Frm_MAX_BYTES_ID + 1]);

void Frm_SetAnchorStr (long Cod,char **Anchor);
void Frm_FreeAnchorStr (char **Anchor);

void Frm_LabelColumn (const char *TDClass,const char *Id,const char *Label);

#endif
