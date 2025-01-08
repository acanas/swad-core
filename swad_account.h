// swad_account.h: user's account

#ifndef _SWAD_ACC
#define _SWAD_ACC
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include <stdbool.h>		// For boolean type

#include "swad_constant.h"
#include "swad_user.h"

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   Acc_REQUEST_REMOVE_USR,
   Acc_REMOVE_USR,
  } Acc_ReqOrRemUsr_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Acc_PutLinkToCreateAccount (void);
void Acc_ShowFormMyAccount (void);
void Acc_CheckIfEmptyAccountExists (void);
void Acc_ShowFormCreateMyAccount (void);

void Acc_ShowFormGoToRequestNewAccount (void);
void Acc_ShowFormChgMyAccount (void);
void Acc_ShowFormChgOtherUsrAccount (void);
void Acc_PutLinkToRemoveMyAccount (__attribute__((unused)) void *Args);
bool Acc_CreateMyNewAccountAndLogIn (void);
void Acc_CreateNewUsr (struct Usr_Data *UsrDat,Usr_MeOrOther_t MeOrOther);
void Acc_AfterCreationNewAccount (void);

void Acc_GetUsrCodAndRemUsrGbl (void);
void Acc_ReqRemAccountOrRemAccount (Acc_ReqOrRemUsr_t RequestOrRemove);

Usr_Can_t Acc_CheckIfICanEliminateAccount (long UsrCod);
void Acc_AskIfRemoveMyAccount (void);
void Acc_RemoveMyAccount (void);
void Acc_CompletelyEliminateAccount (struct Usr_Data *UsrDat,
                                     Cns_Verbose_t Verbose);

void Acc_PutIconToChangeUsrAccount (struct Usr_Data *UsrDat);

#endif
