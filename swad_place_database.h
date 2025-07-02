// swad_place_database.h: places, operations with database

#ifndef _SWAD_PLC_DB
#define _SWAD_PLC_DB
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

#include "swad_place.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Plc_DB_CreatePlace (const struct Plc_Place *Plc);
void Plc_DB_UpdatePlcName (long PlcCod,
			   const char *FldName,const char *NewPlcName);

unsigned Plc_DB_GetListPlaces (MYSQL_RES **mysql_res,Plc_Order_t SelectedOrder);
unsigned Plc_DB_GetPlaceDataByCod (MYSQL_RES **mysql_res,long PlcCod);
Exi_Exist_t Plc_DB_CheckIfPlaceNameExists (const char *FldName,const char *Name,long Cod,
					   long PrtCod,
					   __attribute__((unused)) unsigned Year);

void Plc_DB_RemovePlace (long PlcCod);

#endif
