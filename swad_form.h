// swad_form.h: forms to go to actions

#ifndef _SWAD_FRM
#define _SWAD_FRM
/*
    SWAD (Shared Workspace At a Distance),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_cryptography.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Frm_MAX_BYTES_ID (32 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 10)

#define Frm_MAX_BYTES_PARAM_ACTION	256
#define Frm_MAX_BYTES_PARAM_SESSION	(256 + Cns_BYTES_SESSION_ID)
#define Frm_MAX_BYTES_PARAM_LOCATION	256
#define Frm_MAX_BYTES_PARAMS_STR	(Frm_MAX_BYTES_PARAM_ACTION + Frm_MAX_BYTES_PARAM_SESSION + Frm_MAX_BYTES_PARAM_LOCATION)

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Frm_StartFormGoTo (Act_Action_t NextAction);
void Frm_StartForm (Act_Action_t NextAction);
void Frm_StartFormOnSubmit (Act_Action_t NextAction,const char *OnSubmit);
void Frm_StartFormAnchorOnSubmit (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit);
void Frm_StartFormUnique (Act_Action_t NextAction);
void Frm_StartFormAnchor (Act_Action_t NextAction,const char *Anchor);
void Frm_StartFormUniqueAnchor (Act_Action_t NextAction,const char *Anchor);
void Frm_StartFormUniqueAnchorOnSubmit (Act_Action_t NextAction,const char *Anchor,const char *OnSubmit);
void Frm_StartFormId (Act_Action_t NextAction,const char *Id);
void Frm_SetParamsForm (char ParamsStr[Frm_MAX_BYTES_PARAMS_STR],Act_Action_t NextAction,
                        bool PutParameterLocationIfNoSesion);
void Frm_EndForm (void);

void Frm_SetUniqueId (char UniqueId[Frm_MAX_BYTES_ID + 1]);

void Frm_SetAnchorStr (long Cod,char **Anchor);
void Frm_FreeAnchorStr (char *Anchor);

void Frm_LabelColumn (const char *TDClass,const char *Id,const char *Label);

#endif
