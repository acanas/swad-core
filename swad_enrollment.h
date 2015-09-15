// swad_enrollment.h: enrollment (registration) or removing of users

#ifndef _SWAD_ENR
#define _SWAD_ENR
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
   Enr_SET_ACCEPTED_TO_FALSE,
   Enr_SET_ACCEPTED_TO_TRUE,
  } Enr_KeepOrSetAccepted_t;

#define Enr_NUM_ACTIONS_REG_REM_ONE_USR 9
typedef enum
  {
   Enr_REGISTER_MODIFY_ONE_USR_IN_CRS  = 0,
   Enr_REGISTER_ONE_DEGREE_ADMIN       = 1,
   Enr_REGISTER_ONE_CENTRE_ADMIN       = 2,
   Enr_REGISTER_ONE_INSTITUTION_ADMIN  = 3,
   Enr_REMOVE_ONE_USR_FROM_CRS         = 4,
   Enr_REMOVE_ONE_DEGREE_ADMIN         = 5,
   Enr_REMOVE_ONE_CENTRE_ADMIN         = 6,
   Enr_REMOVE_ONE_INSTITUTION_ADMIN    = 7,
   Enr_ELIMINATE_ONE_USR_FROM_PLATFORM = 8,
  } Enr_RegRemOneUsrAction_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Enr_PutLinkToRequestSignUp (void);

void Enr_ModifyRoleInCurrentCrs (struct UsrData *UsrDat,
                                 Rol_Role_t NewRole,
                                 Cns_QuietOrVerbose_t QuietOrVerbose);
void Enr_RegisterUsrInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole,
                                  Cns_QuietOrVerbose_t QuietOrVerbose,
                                  Enr_KeepOrSetAccepted_t KeepOrSetAccepted);

void Enr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction);

void Enr_ReqAcceptRegisterInCrs (void);
void Enr_GetNotifEnrollment (char *SummaryStr,
                             long CrsCod,long UsrCod,
                             unsigned MaxChars);
void Enr_UpdateUsrData (struct UsrData *UsrDat);
void Enr_FilterUsrDat (struct UsrData *UsrDat);
void Enr_UpdateInstitutionCentreDepartment (void);

void Enr_ReqAdminStds (void);
void Enr_ReqAdminTchs (void);

void Enr_PutLinkToRemOldUsrs (void);
void Enr_AskRemoveOldUsrs (void);
void Enr_RemoveOldUsrs (void);

bool Enr_PutActionsRegRemOneUsr (bool ItsMe);

void Enr_ReceiveFormAdminStds (void);
void Enr_ReceiveFormAdminTchs (void);

void Enr_PutLinkToRemAllStdsThisCrs (void);
void Enr_PutLinkToAdminSeveralUsrs (Rol_Role_t Role);
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

void Enr_PutLinkToAdminOneUsr (Act_Action_t NextAction);

void Enr_ReqRegRemGst (void);
void Enr_ReqRegRemStd (void);
void Enr_ReqRegRemTch (void);
void Enr_ReqRegRemAdm (void);
void Enr_AskIfRegRemAnotherGst (void);
void Enr_AskIfRegRemAnotherStd (void);
void Enr_AskIfRegRemAnotherTch (void);
void Enr_AskIfRegRemAnotherAdm (void);
void Enr_AddAdmToIns (void);
void Enr_AddAdmToCtr (void);
void Enr_AddAdmToDeg (void);

void Enr_ReqRemMeFromCrs (void);
void Enr_ReqRemUsrFromCrs (void);
void Enr_RemUsrFromCrs (void);
void Enr_RemAdmIns (void);
void Enr_RemAdmCtr (void);
void Enr_RemAdmDeg (void);

void Enr_AcceptRegisterMeInCrs (void);
void Enr_CreateNewUsr (void);
void Enr_ModifyUsr (void);
void Enr_AcceptUsrInCrs (long UsrCod);

#endif
