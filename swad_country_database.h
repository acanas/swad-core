// swad_country_database.h: countries operations with database

#ifndef _SWAD_CTY_DB
#define _SWAD_CTY_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Ca�as Vargas

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

#include "swad_country.h"
#include "swad_map.h"
#include "swad_media.h"
#include "swad_search.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Cty_DB_CreateCountry (const struct Hie_Node *Cty,
			   char NameInSeveralLanguages[1 + Lan_NUM_LANGUAGES][Cty_MAX_BYTES_NAME + 1],
			   char WWWInSeveralLanguages [1 + Lan_NUM_LANGUAGES][WWW_MAX_BYTES_WWW + 1]);

unsigned Cty_DB_GetCtysBasic (MYSQL_RES **mysql_res);
unsigned Cty_DB_GetCtysWithPendingInss (MYSQL_RES **mysql_res);
unsigned Cty_DB_GetCtysFull (MYSQL_RES **mysql_res);
Exi_Exist_t Cty_DB_GetBasicCountryDataByCod (MYSQL_RES **mysql_res,long HieCod);
unsigned Cty_DB_GetNamesAndWWWsByCod (MYSQL_RES **mysql_res,long HieCod);
void Cty_DB_GetCountryName (long HieCod,Lan_Language_t Language,
			    char CtyName[Cty_MAX_BYTES_NAME + 1]);

unsigned Cty_DB_GetNumCtysInSys (__attribute__((unused)) long SysCod);

unsigned Cty_DB_GetNumCtysWithInss (Hie_Level_t HieLvl,long HieCod);
unsigned Cty_DB_GetNumCtysWithCtrs (Hie_Level_t HieLvl,long HieCod);
unsigned Cty_DB_GetNumCtysWithDegs (Hie_Level_t HieLvl,long HieCod);
unsigned Cty_DB_GetNumCtysWithCrss (Hie_Level_t HieLvl,long HieCod);
unsigned Cty_DB_GetNumCtysWithUsrs (Hie_Level_t HieLvl,long HieCod,Rol_Role_t Role);

Exi_Exist_t Cty_DB_CheckIfNumericCountryCodeExists (long HieCod);
Exi_Exist_t Cty_DB_CheckIfAlpha2CountryCodeExists (const char *Alpha2);
Exi_Exist_t Cty_DB_CheckIfCountryNameExists (Lan_Language_t Language,
					     const char *Name,long HieCod);

unsigned Cty_DB_SearchCtys (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery);

void Cty_DB_GetAvgCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom);
unsigned Cty_DB_GetCtrsWithCoordsInCurrentCty (MYSQL_RES **mysql_res);
unsigned Cty_DB_GetMapAttr (MYSQL_RES **mysql_res,long HieCod);
Exi_Exist_t Cty_DB_CheckIfMapExists (long HieCod);

void Cty_DB_UpdateCtyField (long HieCod,const char *FldName,const char *FldValue);
void Cty_DB_UpdateCtyMapAttr (const char NewMapAttribution[Med_MAX_BYTES_ATTRIBUTION + 1]);

unsigned Cty_DB_GetMyCtys (MYSQL_RES **mysql_res,
			   __attribute__((unused)) long PrtCod);
unsigned Cty_DB_GetNumUsrsWhoDontClaimToBelongToAnyCty (void);
unsigned Cty_DB_GetNumUsrsWhoClaimToBelongToAnotherCty (void);
unsigned Cty_DB_GetNumUsrsWhoClaimToBelongToCty (long HieCod);

void Cty_DB_RemoveCty (long HieCod);

#endif
