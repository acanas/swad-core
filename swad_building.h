// swad_building.h: buildings in a center

#ifndef _SWAD_BLD
#define _SWAD_BLD
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_constant.h"
#include "swad_name.h"
#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Bld_MAX_CHARS_LOCATION	(128 - 1)	// 127
#define Bld_MAX_BYTES_LOCATION	((Bld_MAX_CHARS_LOCATION + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Bld_Building
  {
   long BldCod;
   long InsCod;
   char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Nam_MAX_BYTES_FULL_NAME + 1];
   char Location[Bld_MAX_BYTES_LOCATION + 1];	// Examples: Campus North, City center
  };

#define Bld_NUM_ORDERS 3
typedef enum
  {
   Bld_ORDER_BY_SHRT_NAME = 0,
   Bld_ORDER_BY_FULL_NAME = 1,
   Bld_ORDER_BY_LOCATION  = 2,
  } Bld_Order_t;
#define Bld_ORDER_DEFAULT Bld_ORDER_BY_SHRT_NAME

/***** Get all data or only short name *****/
typedef enum
  {
   Bld_ALL_DATA,
   Bld_ONLY_SHRT_NAME,
  } Bld_WhichData_t;

struct Bld_Buildings
  {
   unsigned Num;		// Number of buildings
   struct Bld_Building *Lst;	// List of buildings
   Bld_Order_t SelectedOrder;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Bld_ResetBuildings (struct Bld_Buildings *Buildings);

void Bld_SeeBuildings (void);
void Bld_EditBuildings (void);
void Bld_GetListBuildings (struct Bld_Buildings *Buildings,
                           Bld_WhichData_t WhichData);
void Bld_FreeListBuildings (struct Bld_Buildings *Buildings);

void Bld_GetListBuildingsInThisCtr (void);

void Bld_GetBuildingDataByCod (struct Bld_Building *Roo);

void Bld_RemoveBuilding (void);
void Bld_RenameBuildingShort (void);
void Bld_RenameBuildingFull (void);
void Bld_ChangeBuildingLocation (void);
void Bld_ContEditAfterChgBuilding (void);

void Bld_ReceiveNewBuilding (void);

#endif
