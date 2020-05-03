// swad_institution.h: institutions

#ifndef _SWAD_INS
#define _SWAD_INS
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_centre.h"
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
   struct
     {
      bool Valid;
      unsigned NumUsrs;
     } NumUsrsWhoClaimToBelongToIns;
  };

#define Ins_NUM_ORDERS 2
typedef enum
  {
   Ins_ORDER_BY_INSTITUTION = 0,
   Ins_ORDER_BY_NUM_USRS    = 1,
  } Ins_Order_t;
#define Ins_ORDER_DEFAULT Ins_ORDER_BY_NUM_USRS

struct ListInstits
  {
   unsigned Num;		// Number of institutions
   struct Instit *Lst;		// List of institutions
   Ins_Order_t SelectedOrder;	// Order of institutions
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void);

void Ins_DrawInstitutionLogoWithLink (struct Instit *Ins,unsigned Size);
void Ins_DrawInstitutionLogoAndNameWithLink (struct Instit *Ins,Act_Action_t Action,
                                             const char *ClassLink,const char *ClassLogo);

void Ins_ShowInssOfCurrentCty (void);
void Ins_EditInstitutions (void);
void Ins_GetBasicListOfInstitutions (long CtyCod);
void Ins_GetFullListOfInstitutions (long CtyCod);

void Ins_WriteInstitutionNameAndCty (long InsCod);
bool Ins_GetDataOfInstitutionByCod (struct Instit *Ins);
void Ins_FlushCacheShortNameOfInstitution (void);
void Ins_GetShortNameOfInstitution (struct Instit *Ins);
void Ins_FlushCacheFullNameAndCtyOfInstitution (void);

void Ins_FreeListInstitutions (void);
void Ins_WriteSelectorOfInstitution (void);
void Ins_PutParamCurrentInsCod (void *InsCod);
void Ins_PutParamInsCod (long InsCod);
long Ins_GetAndCheckParamOtherInsCod (long MinCodAllowed);
void Ins_RemoveInstitution (void);
void Ins_RenameInsShort (void);
void Ins_RenameInsFull (void);
void Ins_RenameInstitution (struct Instit *Ins,Cns_ShrtOrFullName_t ShrtOrFullName);
bool Ins_CheckIfInsNameExistsInCty (const char *FieldName,
                                    const char *Name,
				    long InsCod,
				    long CtyCod);
void Ins_ChangeInsWWW (void);
void Ins_UpdateInsWWWDB (long InsCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1]);
void Ins_ChangeInsStatus (void);
void Ins_ContEditAfterChgIns (void);

void Ins_RecFormReqIns (void);
void Ins_RecFormNewIns (void);

unsigned Ins_GetCachedNumInssInSys (void);

void Ins_FlushCacheNumInssInCty (void);
unsigned Ins_GetNumInssInCty (long CtyCod);
unsigned Ins_GetCachedNumInssInCty (long CtyCod);

unsigned Ins_GetCachedNumInssWithCtrs (const char *SubQuery,
                                       Hie_Level_t Scope,long Cod);
unsigned Ins_GetCachedNumInssWithDegs (const char *SubQuery,
                                       Hie_Level_t Scope,long Cod);
unsigned Ins_GetCachedNumInssWithCrss (const char *SubQuery,
                                       Hie_Level_t Scope,long Cod);
unsigned Ins_GetCachedNumInssWithUsrs (Rol_Role_t Role,const char *SubQuery,
                                       Hie_Level_t Scope,long Cod);

void Ins_ListInssFound (MYSQL_RES **mysql_res,unsigned NumInss);

bool Ins_GetIfMapIsAvailable (long InsCod);

#endif
