// swad_centre.h: centres

#ifndef _SWAD_CTR
#define _SWAD_CTR
/*
    SWAD (Shared Workspace At a Distance),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_action.h"
#include "swad_constant.h"
#include "swad_role.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

typedef enum
  {
   Ctr_STATUS_BIT_PENDING = (1 << 0),	// Centre is requested, but not yet activated
   Ctr_STATUS_BIT_REMOVED = (1 << 1),	// Centre has been removed
  } Ctr_Status_Bits_t;
typedef unsigned Ctr_Status_t;

#define Ctr_NUM_STATUS_TXT	4
typedef enum
  {
   Ctr_STATUS_UNKNOWN = 0,	// Other
   Ctr_STATUS_ACTIVE  = 1,	// 00 (Status == 0)
   Ctr_STATUS_PENDING = 2,	// 01 (Status == Ctr_STATUS_BIT_PENDING)
   Ctr_STATUS_REMOVED = 3,	// 1- (Status & Ctr_STATUS_BIT_REMOVED)
  } Ctr_StatusTxt_t;

#define Ctr_MAX_CENTRES_PER_USR	 10 // Used in list of my centres

#define Ctr_MAX_LENGTH_CENTRE_SHORT_NAME	     32
#define Ctr_MAX_LENGTH_CENTRE_FULL_NAME		    127
#define Ctr_MAX_LENGTH_PHOTO_ATTRIBUTION	(4*1024)

struct Centre
  {
   long CtrCod;			// Centre code
   long InsCod;			// Institution code
   long PlcCod;			// Place code
   Ctr_Status_t Status;		// Centre status
   long RequesterUsrCod;	// User code of the person who requested the creation of this centre
   char ShortName[Ctr_MAX_LENGTH_CENTRE_SHORT_NAME+1];
   char FullName[Ctr_MAX_LENGTH_CENTRE_FULL_NAME+1];
   char WWW[Cns_MAX_LENGTH_WWW+1];
   unsigned NumDegs;		// Number of degrees in this centre
   unsigned NumTchs;		// Number of teachers in this centre
  };

typedef enum
  {
   Ctr_ORDER_BY_CENTRE   = 0,
   Ctr_ORDER_BY_NUM_TCHS = 1,
  } tCtrsOrderType;

#define Ctr_DEFAULT_ORDER_TYPE Ctr_ORDER_BY_CENTRE

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void);

void Ctr_ShowConfiguration (void);
void Ctr_PrintConfiguration (void);

void Ctr_ShowCtrsOfCurrentIns (void);
void Ctr_EditCentres (void);
void Ctr_GetListCentres (long InsCod);
bool Ctr_GetDataOfCentreByCod (struct Centre *Ctr);
long Ctr_GetInsCodOfCentreByCod (long CtrCod);
void Ctr_GetShortNameOfCentreByCod (struct Centre *Ctr);
void Ctr_FreeListCentres (void);
void Ctr_WriteSelectorOfCentre (void);
void Ctr_PutParamCtrCod (long CtrCod);
long Ctr_GetParamOtherCtrCod (void);
void Ctr_RemoveCentre (void);
void Ctr_ChangeCentreIns (void);
void Ctr_ChangeCentrePlace (void);
void Ctr_RenameCentreShort (void);
void Ctr_RenameCentreFull (void);
void Ctr_ChangeCtrWWW (void);
void Ctr_ChangeCtrStatus (void);

void Ctr_RequestLogo (void);
void Ctr_ReceiveLogo (void);
void Ctr_RemoveLogo (void);
void Ctr_RequestPhoto (void);
void Ctr_ReceivePhoto (void);
void Ctr_ChangeCtrPhotoAttribution (void);
void Ctr_RecFormReqCtr (void);
void Ctr_RecFormNewCtr (void);

unsigned Ctr_GetNumCtrsTotal (void);
unsigned Ctr_GetNumCtrsInCty (long CtyCod);
unsigned Ctr_GetNumCtrsInIns (long InsCod);
unsigned Ctr_GetNumCtrsInPlc (long PlcCod);
unsigned Ctr_GetNumCtrsWithDegs (const char *SubQuery);
unsigned Ctr_GetNumCtrsWithCrss (const char *SubQuery);
unsigned Ctr_GetNumCtrsWithUsrs (Rol_Role_t Role,const char *SubQuery);

unsigned Ctr_ListCtrsFound (const char *Query);

#endif
