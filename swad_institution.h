// swad_institution.h: institutions

#ifndef _SWAD_INS
#define _SWAD_INS
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_constant.h"
#include "swad_role_type.h"

/*****************************************************************************/
/************************ Public types and constants *************************/
/*****************************************************************************/

typedef enum
  {
   Ins_STATUS_BIT_PENDING = (1 << 0),	// Institution is requested, but not yet activated
   Ins_STATUS_BIT_REMOVED = (1 << 1),	// Institution has been removed
  } Ins_Status_Bits_t;

typedef unsigned Ins_Status_t;
#define Ins_MAX_STATUS		((Ins_Status_t) 3)
#define Ins_WRONG_STATUS	((Ins_Status_t) (Ins_MAX_STATUS + 1))

#define Ins_NUM_STATUS_TXT	4
typedef enum
  {
   Ins_STATUS_UNKNOWN = 0,	// Other
   Ins_STATUS_ACTIVE  = 1,	// 00 (Status == 0)
   Ins_STATUS_PENDING = 2,	// 01 (Status == Ins_STATUS_BIT_PENDING)
   Ins_STATUS_REMOVED = 3,	// 1- (Status & Ins_STATUS_BIT_REMOVED)
  } Ins_StatusTxt_t;

#define Ins_MAX_INSTITS_PER_USR	 10	// Used in list of my institutions

struct Instit
  {
   long InsCod;
   long CtyCod;
   Ins_Status_t Status;		// Institution status
   long RequesterUsrCod;	// User code of the person who requested the creation of this institution
   char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Hie_MAX_BYTES_FULL_NAME + 1];
   char WWW[Cns_MAX_BYTES_WWW + 1];
   unsigned NumUsrsWhoClaimToBelongToIns;
   unsigned NumCtrs;
   unsigned NumDegs;
   unsigned NumCrss;
   unsigned NumDpts;
   unsigned NumUsrs;		// Number of users in courses of this institution
  };

#define Ins_NUM_ORDERS 2
typedef enum
  {
   Ins_ORDER_BY_INSTITUTION = 0,
   Ins_ORDER_BY_NUM_USRS    = 1,
  } Ins_Order_t;
#define Ins_ORDER_DEFAULT Ins_ORDER_BY_NUM_USRS

typedef enum
  {
   Ins_GET_BASIC_DATA,
   Ins_GET_EXTRA_DATA,
  } Ins_GetExtraData_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void);

void Ins_DrawInstitutionLogoWithLink (struct Instit *Ins,unsigned Size);
void Ins_DrawInstitutionLogoAndNameWithLink (struct Instit *Ins,Act_Action_t Action,
                                             const char *ClassLink,const char *ClassLogo);

void Ins_ShowConfiguration (void);
void Ins_PrintConfiguration (void);

void Ins_ShowInssOfCurrentCty (void);
void Ins_EditInstitutions (void);
void Ins_GetListInstitutions (long CtyCod,Ins_GetExtraData_t GetExtraData);

void Ins_WriteInstitutionNameAndCty (long InsCod);
bool Ins_GetDataOfInstitutionByCod (struct Instit *Ins,
                                    Ins_GetExtraData_t GetExtraData);
void Ins_FlushCacheShortNameOfInstitution (void);
void Ins_GetShortNameOfInstitution (struct Instit *Ins);
void Ins_FlushCacheFullNameAndCtyOfInstitution (void);

void Ins_FreeListInstitutions (void);
void Ins_WriteSelectorOfInstitution (void);
void Ins_PutParamInsCod (long InsCod);
long Ins_GetAndCheckParamOtherInsCod (long MinCodAllowed);
void Ins_RemoveInstitution (void);
void Ins_RenameInsShort (void);
void Ins_RenameInsShortInConfig (void);
void Ins_RenameInsFull (void);
void Ins_RenameInsFullInConfig (void);
void Ins_ChangeInsCtyInConfig (void);
void Ins_ContEditAfterChgInsInConfig (void);
void Ins_ChangeInsWWW (void);
void Ins_ChangeInsWWWInConfig (void);
void Ins_ChangeInsStatus (void);
void Ins_ContEditAfterChgIns (void);

void Ins_RequestLogo (void);
void Ins_ReceiveLogo (void);
void Ins_RemoveLogo (void);
void Ins_RecFormReqIns (void);
void Ins_RecFormNewIns (void);

unsigned Ins_GetNumInssTotal (void);
unsigned Ins_GetNumInssInCty (long CtyCod);
unsigned Ins_GetNumInssWithCtrs (const char *SubQuery);
unsigned Ins_GetNumInssWithDegs (const char *SubQuery);
unsigned Ins_GetNumInssWithCrss (const char *SubQuery);
unsigned Ins_GetNumInssWithUsrs (Rol_Role_t Role,const char *SubQuery);

void Ins_ListInssFound (MYSQL_RES **mysql_res,unsigned NumInss);

#endif
