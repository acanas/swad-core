// swad_centre.h: centres

#ifndef _SWAD_CTR
#define _SWAD_CTR
/*
    SWAD (Shared Workspace At a Distance),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_degree.h"
#include "swad_map.h"
#include "swad_role_type.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

typedef enum
  {
   Ctr_STATUS_BIT_PENDING = (1 << 0),	// Centre is requested, but not yet activated
   Ctr_STATUS_BIT_REMOVED = (1 << 1),	// Centre has been removed
  } Ctr_Status_Bits_t;

typedef unsigned Ctr_Status_t;
#define Ctr_MAX_STATUS		((Ctr_Status_t) 3)
#define Ctr_WRONG_STATUS	((Ctr_Status_t) (Ctr_MAX_STATUS + 1))

#define Ctr_NUM_STATUS_TXT	4
typedef enum
  {
   Ctr_STATUS_UNKNOWN = 0,	// Other
   Ctr_STATUS_ACTIVE  = 1,	// 00 (Status == 0)
   Ctr_STATUS_PENDING = 2,	// 01 (Status == Ctr_STATUS_BIT_PENDING)
   Ctr_STATUS_REMOVED = 3,	// 1- (Status & Ctr_STATUS_BIT_REMOVED)
  } Ctr_StatusTxt_t;

#define Ctr_MAX_CENTRES_PER_USR	 10 // Used in list of my centres

struct Centre
  {
   long CtrCod;			// Centre code
   long InsCod;			// Institution code
   long PlcCod;			// Place code
   Ctr_Status_t Status;		// Centre status
   long RequesterUsrCod;	// User code of the person who requested the creation of this centre
   struct Coordinates Coord;	// Geographical coordinates
   char ShrtName[Hie_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Hie_MAX_BYTES_FULL_NAME + 1];
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct ListDegrees Degs;	// List of degrees in this centre
  };

#define Ctr_NUM_ORDERS 2
typedef enum
  {
   Ctr_ORDER_BY_CENTRE   = 0,
   Ctr_ORDER_BY_NUM_USRS = 1,
  } Ctr_Order_t;
#define Ctr_ORDER_DEFAULT Ctr_ORDER_BY_CENTRE

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void);

void Ctr_DrawCentreLogoAndNameWithLink (struct Centre *Ctr,Act_Action_t Action,
                                        const char *ClassLink,const char *ClassLogo);

void Ctr_ShowCtrsOfCurrentIns (void);
void Ctr_EditCentres (void);

void Ctr_PutIconToViewCentres (void);

void Ctr_GetListCentres (long InsCod);
bool Ctr_GetDataOfCentreByCod (struct Centre *Ctr);
long Ctr_GetInsCodOfCentreByCod (long CtrCod);
void Ctr_GetShortNameOfCentreByCod (struct Centre *Ctr);
void Ctr_FreeListCentres (void);
void Ctr_WriteSelectorOfCentre (void);
void Ctr_PutParamCtrCod (long CtrCod);
long Ctr_GetAndCheckParamOtherCtrCod (long MinCodAllowed);
void Ctr_RemoveCentre (void);
void Ctr_ChangeCtrPlc (void);
void Ctr_UpdateCtrPlcDB (long CtrCod,long NewPlcCod);
void Ctr_RenameCentreShort (void);
void Ctr_RenameCentreFull (void);
void Ctr_RenameCentre (struct Centre *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName);
bool Ctr_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,
				    long CtrCod,long InsCod);
void Ctr_ChangeCtrWWW (void);
void Ctr_UpdateCtrWWWDB (long CtrCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1]);
void Ctr_ChangeCtrStatus (void);
void Ctr_ContEditAfterChgCtr (void);

void Ctr_RecFormReqCtr (void);
void Ctr_RecFormNewCtr (void);

unsigned Ctr_GetNumCtrsTotal (void);
unsigned Ctr_GetNumCtrsInCty (long CtyCod);
void Ctr_FlushCacheNumCtrsInIns (void);
unsigned Ctr_GetNumCtrsInIns (long InsCod);
unsigned Ctr_GetNumCtrsInPlc (long PlcCod);
unsigned Ctr_GetNumCtrsWithDegs (const char *SubQuery);
unsigned Ctr_GetNumCtrsWithCrss (const char *SubQuery);
unsigned Ctr_GetNumCtrsWithUsrs (Rol_Role_t Role,const char *SubQuery);

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs);

#endif
