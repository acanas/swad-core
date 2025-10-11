// swad_place.h: places

#ifndef _SWAD_PLC
#define _SWAD_PLC
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

#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

struct Plc_Place
  {
   long PlcCod;
   long HieCod;	// Institution code
   char ShrtName[Nam_MAX_BYTES_SHRT_NAME + 1];
   char FullName[Nam_MAX_BYTES_FULL_NAME + 1];
   unsigned NumCtrs;
  };

#define Plc_NUM_ORDERS 2
typedef enum
  {
   Plc_ORDER_BY_PLACE    = 0,
   Plc_ORDER_BY_NUM_CTRS = 1,
  } Plc_Order_t;
#define Plc_ORDER_DEFAULT Plc_ORDER_BY_NUM_CTRS

struct Plc_Places
  {
   unsigned Num;		// Number of places
   struct Plc_Place *Lst;	// List of places
   Plc_Order_t SelectedOrder;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Plc_ResetPlaces (struct Plc_Places *Places);

void Plc_SeeAllPlaces (void);
void Plc_EditPlaces (void);
void Plc_PutIconToViewPlaces (void);
void Plc_GetListPlaces (struct Plc_Places *Places);
void Plc_FreeListPlaces (struct Plc_Places *Places);
void Plc_GetPlaceDataByCod (struct Plc_Place *Plc);
void Plc_RemovePlace (void);
void Plc_RenamePlaceShrt (void);
void Plc_RenamePlaceFull (void);
void Plc_ContEditAfterChgPlc (void);

void Plc_ReceiveNewPlace (void);

#endif
