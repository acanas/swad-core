// swad_place.h: places

#ifndef _SWAD_PLC
#define _SWAD_PLC
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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Plc_MAX_LENGTH_PLACE_SHORT_NAME	 32
#define Plc_MAX_LENGTH_PLACE_FULL_NAME	127

struct Place
  {
   long PlcCod;
   long InsCod;
   char ShortName[Plc_MAX_LENGTH_PLACE_SHORT_NAME+1];
   char FullName[Plc_MAX_LENGTH_PLACE_FULL_NAME+1];
   unsigned NumCtrs;
  };

typedef enum
  {
   Plc_ORDER_BY_PLACE    = 0,
   Plc_ORDER_BY_NUM_CTRS = 1,
  } Plc_PlcsOrderType_t;

#define Plc_DEFAULT_ORDER_TYPE Plc_ORDER_BY_NUM_CTRS

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Plc_SeePlaces (void);
void Plc_EditPlaces (void);
void Plc_GetListPlaces (void);
void Plc_FreeListPlaces (void);
void Plc_GetDataOfPlaceByCod (struct Place *Plc);
long Plc_GetParamPlcCod (void);
void Plc_RemovePlace (void);
void Plc_RenamePlaceShort (void);
void Plc_RenamePlaceFull (void);
void Plc_RecFormNewPlace (void);

#endif
