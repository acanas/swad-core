// swad_enrolment.h: enrolment or removing of users

#ifndef _SWAD_ENR
#define _SWAD_ENR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_course.h"
#include "swad_notification.h"
#include "swad_user.h"

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

#define Enr_ENR_REM_ONE_USR_NUM_ACTIONS 17
typedef enum
  {
   Enr_ENR_REM_ONE_USR_UNKNOWN_ACTION	=  0,
   Enr_ENROL_MODIFY_ONE_USR_IN_CRS	=  1,
   Enr_ENROL_ONE_DEG_ADMIN		=  2,
   Enr_ENROL_ONE_CTR_ADMIN		=  3,
   Enr_ENROL_ONE_INS_ADMIN		=  4,
   Enr_ADD_TO_CLIPBOARD_OTH		=  5,
   Enr_ADD_TO_CLIPBOARD_STD		=  6,
   Enr_ADD_TO_CLIPBOARD_TCH		=  7,
   Enr_OVERWRITE_CLIPBOARD_OTH		=  8,
   Enr_OVERWRITE_CLIPBOARD_STD		=  9,
   Enr_OVERWRITE_CLIPBOARD_TCH		= 10,
   Enr_REPORT_USR_AS_POSSIBLE_DUPLICATE	= 11,
   Enr_REMOVE_ONE_USR_FROM_CRS		= 12,
   Enr_REMOVE_ONE_DEG_ADMIN		= 13,
   Enr_REMOVE_ONE_CTR_ADMIN		= 14,
   Enr_REMOVE_ONE_INS_ADMIN		= 15,
   Enr_ELIMINATE_ONE_USR_FROM_PLATFORM	= 16,
  } Enr_EnrRemOneUsrAction_t;

typedef enum
  {
   Enr_REQUEST_REMOVE_USR,
   Enr_REMOVE_USR,
  } Enr_ReqDelOrDelUsr_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Enr_CheckStdsAndPutButtonToEnrolStdsInCurrentCrs (void);
void Enr_PutButtonInlineToEnrolStds (long CrsCod,
				        unsigned Level,const Lay_LastItem_t *IsLastItemInLevel,
					Lay_Highlight_t Highlight);

void Enr_PutLinkToRequestSignUp (void);

void Enr_ModifyRoleInCurrentCrs (struct Usr_Data *UsrDat,Rol_Role_t NewRole);
void Enr_EnrolUsrInCurrentCrs (struct Usr_Data *UsrDat,Rol_Role_t NewRole,
                                  Enr_KeepOrSetAccepted_t KeepOrSetAccepted);

void Enr_WriteFormToReqAnotherUsrID (Act_Action_t NextAction,void (*FuncPars) (void));

void Enr_ReqAcceptEnrolmentInCrs (void);
void Enr_GetNotifEnrolment (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                            long CrsCod,long UsrCod);
void Enr_UpdateUsrData (struct Usr_Data *UsrDat);

void Enr_ReqAdminStds (void);
void Enr_ReqAdminNonEditingTchs (void);
void Enr_ReqAdminTchs (void);

void Enr_PutLinkToRemOldUsrs (void);
void Enr_AskRemoveOldUsrs (void);
void Enr_RemoveOldUsrs (void);

void Enr_ReceiveAdminStds (void);
void Enr_ReceiveAdminNonEditTchs (void);
void Enr_ReceiveAdminTchs (void);

bool Enr_PutActionsEnrRemOneUsr (Usr_MeOrOther_t MeOrOther);

void Enr_PutLinkToAdminSeveralUsrs (Rol_Role_t Role);
void Enr_AskRemAllStdsThisCrs (void);
void Enr_RemAllStdsThisCrs (void);
unsigned Enr_RemAllStdsInCrs (struct Hie_Node *Crs);
void Enr_ReqSignUpInCrs (void);
void Enr_SignUpInCrs (void);
void Enr_GetNotifEnrolmentRequest (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                   char **ContentStr,
                                   long ReqCod,Ntf_GetContent_t GetContent);
void Enr_AskIfRejectSignUp (void);
void Enr_RejectSignUp (void);
void Enr_ShowEnrolmentRequests (void);
void Enr_UpdateEnrolmentRequests (void);

void Enr_PutLinkToAdminOneUsr (Act_Action_t NextAction);

void Enr_ReqEnrRemOth (void);
void Enr_ReqEnrRemStd (void);
void Enr_ReqEnrRemTch (void);
void Enr_AskIfEnrRemAnotherOth (void);
void Enr_AskIfEnrRemAnotherStd (void);
void Enr_AskIfEnrRemAnotherTch (void);

void Enr_ReqRemMeFromCrs (void);
void Enr_ReqRemUsrFromCrs (void);
void Enr_RemUsrFromCrs1 (void);
void Enr_RemUsrFromCrs2 (void);

void Enr_AcceptEnrolMeInCrs (void);
void Enr_CreateNewUsr1 (void);
void Enr_CreateNewUsr2 (void);
void Enr_ModifyUsr1 (void);
void Enr_ModifyUsr2 (void);

void Enr_FlushCacheUsrBelongsToCurrentCrs (void);
Usr_Belong_t Enr_CheckIfUsrBelongsToCurrentCrs (const struct Usr_Data *UsrDat);
void Enr_FlushCacheUsrHasAcceptedInCurrentCrs (void);
bool Enr_CheckIfUsrHasAcceptedInCurrentCrs (const struct Usr_Data *UsrDat);
void Enr_FlushCacheUsrSharesAnyOfMyCrs (void);
bool Enr_CheckIfUsrSharesAnyOfMyCrs (struct Usr_Data *UsrDat);

unsigned Enr_GetNumUsrsInCrss (Hie_Level_t Level,long Cod,unsigned Roles);
unsigned Enr_GetCachedNumUsrsInCrss (Hie_Level_t Level,long Cod,unsigned Roles);
unsigned Enr_GetCachedNumUsrsNotBelongingToAnyCrs (void);
double Enr_GetCachedAverageNumUsrsPerCrs (Hie_Level_t Level,long Cod,Rol_Role_t Role);
double Enr_GetCachedAverageNumCrssPerUsr (Hie_Level_t Level,long Cod,Rol_Role_t Role);

#endif
