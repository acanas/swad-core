// swad_account.h: user's account

#ifndef _SWAD_ACC
#define _SWAD_ACC
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type

#include "swad_constant.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Acc_ShowFormAccount (void);
void Acc_ShowFormChangeMyAccount (void);
bool Acc_CreateNewAccountAndLogIn (void);
void Acc_CreateNewUsr (struct UsrData *UsrDat);
void Acc_AfterCreationNewAccount (void);

void Acc_ReqRemUsrGbl (void);
void Acc_RemUsrGbl (void);

bool Acc_CheckIfICanEliminateAccount (bool ItsMe);
void Acc_AskIfRemoveMyAccount (void);
void Acc_RemoveMyAccount (void);
void Acc_CompletelyEliminateAccount (struct UsrData *UsrDat,
                                     Cns_QuietOrVerbose_t QuietOrVerbose);

#endif
