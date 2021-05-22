// swad_country_database.h: countries operations with database

#ifndef _SWAD_CTY_DB
#define _SWAD_CTY_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_map.h"
#include "swad_media.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

unsigned Cty_DB_GetBasicListOfCountries (MYSQL_RES **mysql_res);
unsigned Cty_DB_GetListOfCountriesWithPendingInss (MYSQL_RES **mysql_res);
unsigned Cty_DB_GetFullListOfCountries (MYSQL_RES **mysql_res);

void Cty_DB_GetCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom);
unsigned Cty_DB_GetCtrsWithCoordsInCurrentCty (MYSQL_RES **mysql_res);
unsigned Cty_DB_GetMapAttr (MYSQL_RES **mysql_res,long CtyCod);
void Cty_DB_UpdateCtyMapAttr (const char NewMapAttribution[Med_MAX_BYTES_ATTRIBUTION + 1]);

#endif
