// swad_country.h: countries

#ifndef _SWAD_CTY
#define _SWAD_CTY
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Ca�as Vargas

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
#include "swad_hierarchy_level.h"
#include "swad_institution.h"
#include "swad_language.h"
#include "swad_role_type.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Cty_MAX_COUNTRS_PER_USR	 10	// Used in list of my countries

#define Cty_MAX_CHARS_NAME	(48 - 1)	// 47
#define Cty_MAX_BYTES_NAME	((Cty_MAX_CHARS_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 767

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Cty_SeeCtyWithPendingInss (void);

void Cty_ListCountries (void);
void Cty_ListCountries1 (void);
void Cty_ListCountries2 (void);

bool Cty_CheckIfICanEditCountries (void);

void Cty_DrawCountryMapAndNameWithLink (struct Hie_Node *Cty,Act_Action_t Action,
                                        const char *ClassContainer,
                                        const char *ClassMap);
void Cty_DrawCountryMap (struct Hie_Node *Cty,const char *Class);
bool Cty_CheckIfCountryPhotoExists (struct Hie_Node *Cty);

void Cty_WriteScriptGoogleGeochart (void);
void Cty_PutParCtyOrder (void);
void Cty_EditCountries (void);
void Cty_GetBasicListOfCountries (void);
void Cty_WriteSelectorOfCountry (void);
void Cty_WriteCountryName (long CtyCod);
bool Cty_GetBasicCountryDataByCod (struct Hie_Node *Cty);
void Cty_GetNamesAndWWWsByCod (struct Hie_Node *Cty,
			       char NameInSeveralLanguages[1 + Lan_NUM_LANGUAGES][Cty_MAX_BYTES_NAME + 1],
			       char WWWInSeveralLanguages [1 + Lan_NUM_LANGUAGES][Cns_MAX_BYTES_WWW + 1]);
void Cty_FlushCacheCountryName (void);
void Cty_GetCountryNameInLanguage (long CtyCod,Lan_Language_t Language,
				   char CtyName[Cty_MAX_BYTES_NAME + 1]);
void Cty_RemoveCountry (void);
void Cty_RenameCountry (void);
void Cty_ChangeCtyWWW (void);
void Cty_ContEditAfterChgCty (void);
void Cty_ReceiveFormNewCountry (void);

unsigned Cty_GetCachedNumCtysInSys (void);

unsigned Cty_GetCachedNumCtysWithInss (void);
unsigned Cty_GetCachedNumCtysWithCtrs (void);
unsigned Cty_GetCachedNumCtysWithDegs (void);
unsigned Cty_GetCachedNumCtysWithCrss (void);

unsigned Cty_GetCachedNumCtysWithUsrs (Rol_Role_t Role);

void Cty_ListCtysFound (MYSQL_RES **mysql_res,unsigned NumCtys);

void Cty_GetMyCountrs (void);
void Cty_FreeMyCountrs (void);
bool Cty_CheckIfIBelongToCty (long CtyCod);

void Cty_FlushCacheNumUsrsWhoDontClaimToBelongToAnyCty (void);
unsigned Cty_GetNumUsrsWhoDontClaimToBelongToAnyCty (void);
unsigned Cty_GetCachedNumUsrsWhoDontClaimToBelongToAnyCty (void);

void Cty_FlushCacheNumUsrsWhoClaimToBelongToAnotherCty (void);
unsigned Cty_GetNumUsrsWhoClaimToBelongToAnotherCty (void);
unsigned Cty_GetCachedNumUsrsWhoClaimToBelongToAnotherCty (void);

void Cty_FlushCacheNumUsrsWhoClaimToBelongToCty (void);
unsigned Cty_GetNumUsrsWhoClaimToBelongToCty (struct Hie_Node *Cty);
unsigned Cty_GetCachedNumUsrsWhoClaimToBelongToCty (struct Hie_Node *Cty);

#endif
