// swad_building_database.h: buildings in a center operations with database

#ifndef _SWAD_BLD_DB
#define _SWAD_BLD_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_building.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Bld_DB_CreateBuilding (const struct Bld_Building *Building);

unsigned Bld_DB_GetListBuildings (MYSQL_RES **mysql_res,
                                  Bld_WhichData_t WhichData,
                                  Bld_Order_t SelectedOrder);
Exi_Exist_t Bld_DB_GetBuildingDataByCod (MYSQL_RES **mysql_res,long BldCod);
Exi_Exist_t Bld_DB_CheckIfBuildingNameExists (const char *FldName,const char *Name,long Cod,
					      __attribute__((unused)) long PrtCod,
					      __attribute__((unused)) unsigned Year);

void Bld_DB_UpdateBuildingField (long BldCod,
				 const char *FldName,const char *Value);

void Bld_DB_RemoveBuilding (long BldCod);
void Bld_DB_RemoveAllBuildingsInCtr (long HieCod);

#endif
