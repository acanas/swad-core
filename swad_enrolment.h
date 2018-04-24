// swad_enrolment.h: enrolment (registration) or removing of users

#ifndef _SWAD_ENR
#define _SWAD_ENR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include "swad_notification.h"
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

#define Enr_REG_REM_ONE_USR_NUM_ACTIONS 11
typedef enum
  {
   Enr_REG_REM_ONE_USR_UNKNOWN_ACTION   =  0,
   Enr_REGISTER_MODIFY_ONE_USR_IN_CRS   =  1,
   Enr_REGISTER_ONE_DEGREE_ADMIN        =  2,
   Enr_REGISTER_ONE_CENTRE_ADMIN        =  3,
   Enr_REGISTER_ONE_INSTITUTION_ADMIN   =  4,
   Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE =  5,
   Enr_REMOVE_ONE_USR_FROM_CRS          =  6,
   Enr_REMOVE_ONE_DEGREE_ADMIN          =  7,
   Enr_REMOVE_ONE_CENTRE_ADMIN          =  8,
   Enr_REMOVE_ONE_INSTITUTION_ADMIN     =  9,
   Enr_ELIMINATE_ONE_USR_FROM_PLATFORM  = 10,
  } Enr_RegRemOneUsrAction_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs (void);
void Enr_PutButtonInlineToRegisterStds (long CrsCod);

void Enr_PutLinkToRequestSignUp (void);

void Enr_ModifyRoleInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole);
void Enr_RegisterUsrInCurrentCrs (struct UsrData *UsrDat,Rol_Role_t NewRole,
                                  Enr_KeepOrSetAccepted_t KeepOrSetAccepted);

void Enr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction,void (*FuncParams) ());

void Enr_ReqAcceptRegisterInCrs (void);
void Enr_GetNotifEnrolment (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                            long CrsCod,long UsrCod);
void Enr_UpdateUsrData (struct UsrData *UsrDat);
void Enr_FilterUsrDat (struct UsrData *UsrDat);
void Enr_UpdateInstitutionCentreDepartment (void);

void Enr_ReqAdminStds (void);
void Enr_ReqAdminNonEditingTchs (void);
void Enr_ReqAdminTchs (void);

void Enr_PutLinkToRemOldUsrs (void);
void Enr_AskRemoveOldUsrs (void);
void Enr_RemoveOldUsrs (void);

bool Enr_PutActionsRegRemOneUsr (bool ItsMe);

void Enr_ReceiveFormAdminStds (void);
void Enr_ReceiveFormAdminNonEditTchs (void);
void Enr_ReceiveFormAdminTchs (void);

void Enr_PutLinkToAdminSeveralUsrs (Rol_Role_t Role);
void Enr_AskRemAllStdsThisCrs (void);
void Enr_RemAllStdsThisCrs (void);
unsigned Enr_RemAllStdsInCrs (struct Course *Crs);
void Enr_ReqSignUpInCrs (void);
void Enr_SignUpInCrs (void);
void Enr_GetNotifEnrolmentRequest (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                   char **ContentStr,
                                   long ReqCod,bool GetContent);
void Enr_AskIfRejectSignUp (void);
void Enr_RejectSignUp (void);
void Enr_ShowEnrolmentRequests (void);
void Enr_UpdateEnrolmentRequests (void);

void Enr_PutLinkToAdminOneUsr (Act_Action_t NextAction);

void Enr_ReqRegRemOth (void);
void Enr_ReqRegRemStd (void);
void Enr_ReqRegRemTch (void);
void Enr_AskIfRegRemAnotherOth (void);
void Enr_AskIfRegRemAnotherStd (void);
void Enr_AskIfRegRemAnotherTch (void);
void Enr_AddAdmToIns (void);
void Enr_AddAdmToCtr (void);
void Enr_AddAdmToDeg (void);

void Enr_ReqRemMeFromCrs (void);
void Enr_ReqRemUsrFromCrs (void);
void Enr_RemUsrFromCrs1 (void);
void Enr_RemUsrFromCrs2 (void);
void Enr_RemAdmIns (void);
void Enr_RemAdmCtr (void);
void Enr_RemAdmDeg (void);

void Enr_AcceptRegisterMeInCrs (void);
void Enr_CreateNewUsr1 (void);
void Enr_CreateNewUsr2 (void);
void Enr_ModifyUsr1 (void);
void Enr_ModifyUsr2 (void);

void Enr_AcceptUsrInCrs (long UsrCod);

#endif
