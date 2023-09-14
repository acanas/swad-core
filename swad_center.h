// swad_center.h: centers

#ifndef _SWAD_CTR
#define _SWAD_CTR
/*
    SWAD (Shared Workspace At a Distance),
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

#define Ctr_MAX_CENTERS_PER_USR	 10 // Used in list of my centers

struct Ctr_Center
  {
   long Cod;			// Center code
   long InsCod;			// Institution code
   long PlcCod;			// Place code
   Hie_Status_t Status;		// Center status
   long RequesterUsrCod;	// User code of the person who requested the creation of this center
   struct Map_Coordinates Coord;	// Geographical coordinates
   char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct
     {
      bool Valid;
      unsigned NumUsrs;
     } NumUsrsWhoClaimToBelong;
  };

#define Ctr_NUM_ORDERS 2
typedef enum
  {
   Ctr_ORDER_BY_CENTER   = 0,
   Ctr_ORDER_BY_NUM_USRS = 1,
  } Ctr_Order_t;
#define Ctr_ORDER_DEFAULT Ctr_ORDER_BY_CENTER

struct Ctr_ListCtrs
  {
   unsigned Num;		// Number of centers
   struct Ctr_Center *Lst;	// List of centers
   Ctr_Order_t SelectedOrder;	// Order of centers
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void);

void Ctr_DrawCenterLogoAndNameWithLink (struct Ctr_Center *Ctr,Act_Action_t Action,
                                        const char *ClassLogo);

void Ctr_ShowCtrsOfCurrentIns (void);
void Ctr_EditCenters (void);

void Ctr_GetBasicListOfCenters (long InsCod);
void Ctr_GetFullListOfCenters (long InsCod,Ctr_Order_t SelectedOrder);
bool Ctr_GetCenterDataByCod (struct Ctr_Center *Ctr);
void Ctr_FreeListCenters (void);
void Ctr_WriteSelectorOfCenter (void);
void Ctr_RemoveCenter (void);
void Ctr_ChangeCtrPlc (void);
void Ctr_RenameCenterShort (void);
void Ctr_RenameCenterFull (void);
void Ctr_RenameCenter (struct Ctr_Center *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName);
void Ctr_ChangeCtrWWW (void);
void Ctr_ChangeCtrStatus (void);
void Ctr_ContEditAfterChgCtr (void);

void Ctr_ReceiveFormReqCtr (void);
void Ctr_ReceiveFormNewCtr (void);

unsigned Ctr_GetCachedNumCtrsInSys (void);

void Ctr_FlushCacheNumCtrsInCty (void);
unsigned Ctr_GetCachedNumCtrsInCty (long CtyCod);

void Ctr_FlushCacheNumCtrsInIns (void);
unsigned Ctr_GetNumCtrsInIns (long InsCod);
unsigned Ctr_GetCachedNumCtrsInIns (long InsCod);

unsigned Ctr_GetCachedNumCtrsWithMapInSys (void);
unsigned Ctr_GetCachedNumCtrsWithMapInCty (long CtyCod);
unsigned Ctr_GetCachedNumCtrsWithMapInIns (long InsCod);

unsigned Ctr_GetCachedNumCtrsWithDegs (void);
unsigned Ctr_GetCachedNumCtrsWithCrss (void);
unsigned Ctr_GetCachedNumCtrsWithUsrs (Rol_Role_t Role);

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs);

bool Ctr_GetIfMapIsAvailable (const struct Ctr_Center *Ctr);

void Ctr_GetMyCenters (void);
void Ctr_FreeMyCenters (void);
bool Ctr_CheckIfIBelongToCtr (long CtrCod);
void Ctr_FlushCacheUsrBelongsToCtr (void);
bool Ctr_CheckIfUsrBelongsToCtr (long UsrCod,long CtrCod);

void Ctr_FlushCacheNumUsrsWhoClaimToBelongToCtr (void);
unsigned Ctr_GetNumUsrsWhoClaimToBelongToCtr (struct Ctr_Center *Ctr);
unsigned Ctr_GetCachedNumUsrsWhoClaimToBelongToCtr (struct Ctr_Center *Ctr);

#endif
