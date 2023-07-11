// swad_institution.h: institutions

#ifndef _SWAD_INS
#define _SWAD_INS
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_center.h"
#include "swad_constant.h"
#include "swad_role_type.h"

/*****************************************************************************/
/************************ Public types and constants *************************/
/*****************************************************************************/

#define Ins_MAX_INSTITS_PER_USR	 10	// Used in list of my institutions

struct Ins_Instit
  {
   long InsCod;
   long CtyCod;
   Hie_Status_t Status;		// Institution status
   long RequesterUsrCod;	// User code of the person who requested the creation of this institution
   char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];
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

struct Ins_ListInss
  {
   unsigned Num;		// Number of institutions
   struct Ins_Instit *Lst;	// List of institutions
   Ins_Order_t SelectedOrder;	// Order of institutions
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ins_SeeInsWithPendingCtrs (void);

void Ins_DrawInstitutionLogoWithLink (struct Ins_Instit *Ins,unsigned Size);
void Ins_DrawInstitLogoAndNameWithLink (struct Ins_Instit *Ins,Act_Action_t Action,
                                        const char *ClassLogo);

void Ins_ShowInssOfCurrentCty (void);
void Ins_EditInstitutions (void);
void Ins_GetBasicListOfInstitutions (long CtyCod);
void Ins_GetFullListOfInstitutions (long CtyCod);

void Ins_WriteInstitutionNameAndCty (long InsCod);
bool Ins_GetInstitDataByCod (struct Ins_Instit *Ins);
void Ins_FlushCacheFullNameAndCtyOfInstitution (void);
void Ins_GetShrtNameAndCtyOfInstitution (struct Ins_Instit *Ins,
                                         char CtyName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1]);

void Ins_FreeListInstitutions (void);
void Ins_WriteSelectorOfInstitution (void);

void Ins_RemoveInstitution (void);
void Ins_RenameInsShort (void);
void Ins_RenameInsFull (void);
void Ins_RenameInstitution (struct Ins_Instit *Ins,Cns_ShrtOrFullName_t ShrtOrFullName);
void Ins_ChangeInsWWW (void);
void Ins_ChangeInsStatus (void);
void Ins_ContEditAfterChgIns (void);

void Ins_ReceiveFormReqIns (void);
void Ins_ReceiveFormNewIns (void);

unsigned Ins_GetCachedNumInssInSys (void);

void Ins_FlushCacheNumInssInCty (void);
unsigned Ins_GetNumInssInCty (long CtyCod);
unsigned Ins_GetCachedNumInssInCty (long CtyCod);

unsigned Ins_GetCachedNumInssWithCtrs (void);
unsigned Ins_GetCachedNumInssWithDegs (void);
unsigned Ins_GetCachedNumInssWithCrss (void);
unsigned Ins_GetCachedNumInssWithUsrs (Rol_Role_t Role);

void Ins_ListInssFound (MYSQL_RES **mysql_res,unsigned NumInss);

void Ins_GetMyInstits (void);
void Ins_FreeMyInstits (void);
bool Ins_CheckIfIBelongToIns (long InsCod);
void Ins_FlushCacheUsrBelongsToIns (void);
bool Ins_CheckIfUsrBelongsToIns (long UsrCod,long InsCod);

void Ins_FlushCacheNumUsrsWhoClaimToBelongToIns (void);
unsigned Ins_GetNumUsrsWhoClaimToBelongToIns (struct Ins_Instit *Ins);
unsigned Ins_GetCachedNumUsrsWhoClaimToBelongToIns (struct Ins_Instit *Ins);

void Ins_PutParInsCod (void *InsCod);

//-------------------------------- Figures ------------------------------------
void Ins_GetAndShowInstitutionsStats (void);

#endif
