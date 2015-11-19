// swad_country.h: countries

#ifndef _SWAD_CTY
#define _SWAD_CTY
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type

#include "swad_action.h"
#include "swad_role.h"
#include "swad_text.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Cty_MAX_BYTES_COUNTRY_NAME	    255
#define Cty_MAX_LENGTH_COUNTRY_WWW	    255
#define Cty_MAX_LENGTH_MAP_ATTRIBUTION	(4*1024)

struct Country
  {
   long CtyCod;
   char Alpha2[2+1];
   char Name[Txt_NUM_LANGUAGES][Cty_MAX_BYTES_COUNTRY_NAME+1];
   char WWW[Txt_NUM_LANGUAGES][Cty_MAX_LENGTH_COUNTRY_WWW+1];
   unsigned NumUsrs;
   unsigned NumStds;
   unsigned NumTchs;
   unsigned NumInss;
  };

typedef enum
  {
   Cty_ORDER_BY_COUNTRY  = 0,
   Cty_ORDER_BY_NUM_USRS = 1,
  } Cty_CtysOrderType_t;

#define Cty_DEFAULT_ORDER_TYPE Cty_ORDER_BY_NUM_USRS

typedef enum
  {
   Cty_GET_ONLY_COUNTRIES,
   Cty_GET_EXTRA_DATA,
  } Cty_GetExtraData_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Cty_SeeCtyWithPendingInss (void);

void Cty_ShowConfiguration (void);
void Cty_PrintConfiguration (void);

void Cty_ListCountries (void);
void Cty_ListCountries1 (void);
void Cty_ListCountries2 (void);

void Cty_DrawCountryMapAndNameWithLink (struct Country *Cty,Act_Action_t Action,
                                        const char *ClassLink,const char *ClassMap);
void Cty_DrawCountryMap (struct Country *Cty,const char *Class);
bool Cty_CheckIfCountryMapExists (struct Country *Cty);

void Cty_WriteScriptGoogleGeochart (void);
void Cty_PutHiddenParamCtyOrderType (void);
void Cty_EditCountries (void);
void Cty_GetListCountries (Cty_GetExtraData_t GetExtraData);
void Cty_FreeListCountries (void);
void Cty_WriteSelectorOfCountry (void);
bool Cty_GetDataOfCountryByCod (struct Country *Cty);
void Cty_GetCountryName (long CtyCod,char CtyName[Cty_MAX_BYTES_COUNTRY_NAME+1]);
void Cty_PutParamCtyCod (long CtyCod);
long Cty_GetParamOtherCtyCod (void);
void Cty_RemoveCountry (void);
void Cty_RenameCountry (void);
void Cty_ChangeCtyWWW (void);
void Cty_ChangeCtyMapAttribution (void);
void Cty_RecFormNewCountry (void);

unsigned Cty_GetNumCtysTotal (void);
unsigned Cty_GetNumCtysWithInss (const char *SubQuery);
unsigned Cty_GetNumCtysWithCtrs (const char *SubQuery);
unsigned Cty_GetNumCtysWithDegs (const char *SubQuery);
unsigned Cty_GetNumCtysWithCrss (const char *SubQuery);
unsigned Cty_GetNumCtysWithUsrs (Rol_Role_t Role,const char *SubQuery);

#endif
