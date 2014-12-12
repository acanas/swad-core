// swad_enrollment.h: enrollment (registration) or removing of users

#ifndef _SWAD_ENR
#define _SWAD_ENR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

typedef enum
  {
   Enr_REPLACE_DATA,
   Enr_INSERT_NEW_DATA,
   } Enr_ReplaceInsert_t;

typedef enum
  {
   Enr_REQUEST_REMOVE_USR,
   Enr_REMOVE_USR,
  } Enr_ReqDelOrDelUsr_t;

typedef enum
  {
   Enr_SET_ACCEPTED_TO_FALSE,
   Enr_SET_ACCEPTED_TO_TRUE,
  } Enr_KeepOrSetAccepted_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Enr_RemoveUsrFromTableClicksWithoutPhoto (long UsrCod);

void Enr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction);

void Enr_AfterCreationNewAccount (void);
void Enr_ShowFormAccount (void);
void Enr_ShowFormChangeMyAccount (void);

void Enr_AskRemoveOldUsrs (void);
void Enr_RemoveOldUsrs (void);

void Enr_ReqAcceptRegisterInCrs (void);

void Enr_GetNotifEnrollment (char *SummaryStr,
                             long CrsCod,long UsrCod,
                             unsigned MaxChars);
void Enr_CreateNewUsr (struct UsrData *UsrDat);
void Enr_UpdateUsrData (struct UsrData *UsrDat);
void Enr_UpdateInstitutionCentreDepartment (void);
void Enr_RegisterUsrInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole,
                                  Cns_QuietOrVerbose_t QuietOrVerbose,
                                  Enr_KeepOrSetAccepted_t KeepOrSetAccepted);
bool Enr_CreateNewAccountAndLogIn (void);

void Enr_RemoveUsrBriefcase (struct UsrData *UsrDat);
void Enr_ShowFormRegRemSeveralUsrs (void);
void Enr_ReceiveFormUsrsCrs (void);
void Enr_AskRemAllStdsThisCrs (void);
void Enr_RemAllStdsThisCrs (void);
unsigned Enr_RemAllStdsInCrs (struct Course *Crs);
void Enr_ReqSignUpInCrs (void);
void Enr_SignUpInCrs (void);
void Enr_GetNotifEnrollmentRequest (char *SummaryStr,char **ContentStr,
                                    long ReqCod,unsigned MaxChars,bool GetContent);
void Enr_AskIfRejectSignUp (void);
void Enr_RejectSignUp (void);
void Enr_ShowEnrollmentRequests (void);
Rol_Role_t Enr_GetRequestedRole (long UsrCod);
void Enr_ReqRegRemUsr (void);
void Enr_AskIfRegRemAnotherUsr (void);
void Enr_AddAdmToDeg (void);

void Enr_ReqRemMeFromCrs (void);
void Enr_ReqRemUsrFromCrs (void);
void Enr_RemUsrFromCrs (void);
void Enr_ReqRemOrRemUsrFromCrs (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr);
void Enr_ReqRemUsrGbl (void);
void Enr_RemAdm (void);
void Enr_RemUsrGbl (void);
void Enr_ReqDelOrDelUsrGbl (Enr_ReqDelOrDelUsr_t ReqDelOrDelUsr);
void Enr_AskIfRemAdmFromDeg (bool ItsMe);

void Enr_AcceptRegisterMeInCrs (void);
void Enr_CreatAndShowNewUsrRecordAndRegInCrs (void);
void Enr_ModifAndShowUsrCardAndRegInCrsAndGrps (void);
void Enr_AcceptUsrInCrs (long UsrCod);

#endif
