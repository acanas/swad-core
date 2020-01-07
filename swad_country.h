// swad_country.h: countries

#ifndef _SWAD_CTY
#define _SWAD_CTY
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases
#include <stdbool.h>		// For boolean type

#include "swad_action.h"
#include "swad_hierarchy.h"
#include "swad_institution.h"
#include "swad_language.h"
#include "swad_role_type.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Cty_MAX_COUNTRS_PER_USR	 10	// Used in list of my countries

#define Cty_MAX_CHARS_NAME	(48 - 1)	// 47
#define Cty_MAX_BYTES_NAME	((Cty_MAX_CHARS_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 767

struct Country
  {
   long CtyCod;
   char Alpha2[2 + 1];
   char Name[1 + Lan_NUM_LANGUAGES][Cty_MAX_BYTES_NAME + 1];
   char WWW [1 + Lan_NUM_LANGUAGES][Cns_MAX_BYTES_WWW + 1];
   struct
     {
      unsigned Num;		// Number of institutions in this country
      struct Instit *Lst;	// List of institutions in this country
      Ins_Order_t SelectedOrder;
     } Inss;
   unsigned NumCtrs;
   unsigned NumDegs;
   unsigned NumCrss;
   struct
     {
      bool Valid;
      unsigned NumUsrs;
     } NumUsrsWhoClaimToBelongToCty;
  };

#define Cty_NUM_ORDERS 2
typedef enum
  {
   Cty_ORDER_BY_COUNTRY  = 0,
   Cty_ORDER_BY_NUM_USRS = 1,
  } Cty_Order_t;
#define Cty_ORDER_DEFAULT Cty_ORDER_BY_NUM_USRS

typedef enum
  {
   Cty_GET_BASIC_DATA,
   Cty_GET_EXTRA_DATA,
  } Cty_GetExtraData_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Cty_SeeCtyWithPendingInss (void);

void Cty_ListCountries (void);
void Cty_ListCountries1 (void);
void Cty_ListCountries2 (void);

bool Cty_CheckIfICanEditCountries (void);

void Cty_DrawCountryMapAndNameWithLink (struct Country *Cty,Act_Action_t Action,
                                        const char *ClassContainer,
                                        const char *ClassMap,
                                        const char *ClassLink);
void Cty_DrawCountryMap (struct Country *Cty,const char *Class);
bool Cty_CheckIfCountryPhotoExists (struct Country *Cty);

void Cty_WriteScriptGoogleGeochart (void);
void Cty_PutHiddenParamCtyOrder (void);
void Cty_EditCountries (void);
void Cty_GetListCountries (Cty_GetExtraData_t GetExtraData);
void Cty_FreeListCountries (void);
void Cty_WriteSelectorOfCountry (void);
void Cty_WriteCountryName (long CtyCod,const char *ClassLink);
bool Cty_GetDataOfCountryByCod (struct Country *Cty,Cty_GetExtraData_t GetExtraData);
void Cty_FlushCacheCountryName (void);
void Cty_GetCountryName (long CtyCod,char CtyName[Cty_MAX_BYTES_NAME + 1]);
void Cty_PutParamCtyCod (long CtyCod);
long Cty_GetAndCheckParamOtherCtyCod (long MinCodAllowed);
void Cty_RemoveCountry (void);
void Cty_RenameCountry (void);
void Cty_ChangeCtyWWW (void);
void Cty_ContEditAfterChgCty (void);
void Cty_RecFormNewCountry (void);

unsigned Cty_GetNumCtysTotal (void);
unsigned Cty_GetNumCtysWithInss (const char *SubQuery);
unsigned Cty_GetNumCtysWithCtrs (const char *SubQuery);
unsigned Cty_GetNumCtysWithDegs (const char *SubQuery);
unsigned Cty_GetNumCtysWithCrss (const char *SubQuery);
unsigned Cty_GetNumCtysWithUsrs (Rol_Role_t Role,const char *SubQuery);

void Cty_ListCtysFound (MYSQL_RES **mysql_res,unsigned NumCtys);

#endif
