// swad_institution.h: institutions

#ifndef _SWAD_INS
#define _SWAD_INS
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

#include "swad_constant.h"
#include "swad_role.h"

/*****************************************************************************/
/************************ Public types and constants *************************/
/*****************************************************************************/

typedef enum
  {
   Ins_STATUS_BIT_PENDING = (1 << 0),	// Institution is requested, but not yet activated
   Ins_STATUS_BIT_REMOVED = (1 << 1),	// Institution has been removed
  } Ins_Status_Bits_t;
typedef unsigned Ins_Status_t;

#define Ins_NUM_STATUS_TXT	4
typedef enum
  {
   Ins_STATUS_UNKNOWN = 0,	// Other
   Ins_STATUS_ACTIVE  = 1,	// 00 (Status == 0)
   Ins_STATUS_PENDING = 2,	// 01 (Status == Ins_STATUS_BIT_PENDING)
   Ins_STATUS_REMOVED = 3,	// 1- (Status & Ins_STATUS_BIT_REMOVED)
  } Ins_StatusTxt_t;

#define Ins_MAX_INSTITUTIONS_PER_USR	 10	// Used in list of my institutions

#define Ins_MAX_LENGTH_INSTITUTION_SHORT_NAME	 32
#define Ins_MAX_LENGTH_INSTITUTION_FULL_NAME   1024

struct Institution
  {
   long InsCod;
   long CtyCod;
   Ins_Status_t Status;		// Institution status
   long RequesterUsrCod;	// User code of the person who requested the creation of this institution
   char ShortName[Ins_MAX_LENGTH_INSTITUTION_SHORT_NAME+1];
   char FullName[Ins_MAX_LENGTH_INSTITUTION_FULL_NAME+1];
   char WWW[Cns_MAX_LENGTH_WWW+1];
   unsigned NumStds;
   unsigned NumTchs;
   unsigned NumUsrs;
   unsigned NumCtrs;
   unsigned NumDpts;
   unsigned NumDegs;
  };

typedef enum
  {
   Ins_ORDER_BY_INSTITUTION = 0,
   Ins_ORDER_BY_NUM_USRS    = 1,
  } Ins_InssOrderType_t;

#define Ins_DEFAULT_ORDER_TYPE Ins_ORDER_BY_NUM_USRS

typedef enum
  {
   Ins_GET_MINIMAL_DATA,
   Ins_GET_EXTRA_DATA,
  } Ins_GetExtraData_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void);

void Ins_ShowConfiguration (void);
void Ins_PrintConfiguration (void);

void Ins_ShowInssOfCurrentCty (void);
void Ins_EditInstitutions (void);
void Ins_GetListInstitutions (long CtyCod,Ins_GetExtraData_t GetExtraData);
bool Ins_GetDataOfInstitutionByCod (struct Institution *Ins,
                                    Ins_GetExtraData_t GetExtraData);
void Ins_FreeListInstitutions (void);
void Ins_WriteSelectorOfInstitution (Act_Action_t NextAction);
void Ins_PutParamInsCod (long InsCod);
long Ins_GetParamOtherInsCod (void);
void Ins_RemoveInstitution (void);
void Ins_RenameInsShort (void);
void Ins_RenameInsFull (void);
void Ins_ChangeInsCountry (void);
void Ins_ChangeInsWWW (void);
void Ins_ChangeInsStatus (void);
void Ins_RequestLogo (void);
void Ins_ReceiveLogo (void);
void Ins_RecFormReqIns (void);
void Ins_RecFormNewIns (void);

unsigned Ins_GetNumInssTotal (void);
unsigned Ins_GetNumInssInCty (long CtyCod);
unsigned Ins_GetNumInssWithCtrs (const char *SubQuery);
unsigned Ins_GetNumInssWithDegs (const char *SubQuery);
unsigned Ins_GetNumInssWithCrss (const char *SubQuery);
unsigned Ins_GetNumInssWithUsrs (Rol_Role_t Role,const char *SubQuery);

void Ins_DrawInstitutionLogo (long InsCod,const char *AltText,unsigned Size,const char *Style);

unsigned Ins_ListInssFound (const char *Query);

#endif
