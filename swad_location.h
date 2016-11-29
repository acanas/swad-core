// swad_location.h: teacher's location

#ifndef _SWAD_LOC
#define _SWAD_LOC
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include "swad_date.h"
#include "swad_place.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Loc_MAX_LENGTH_LOCATION	255

typedef enum
  {
   LOC_START_DATE,
   LOC_END_DATE
  } Loc_StartOrEndDate_t;

struct Location
  {
   long LocCod;
   struct Date StartDate;
   struct Date EndDate;
   char Location[Loc_MAX_LENGTH_LOCATION+1];
  };

typedef enum
  {
   Loc_ORDER_BY_START_DATE = 0,
   Loc_ORDER_BY_END_DATE   = 1,
  } Loc_OrderType_t;

#define Loc_DEFAULT_ORDER_TYPE Loc_ORDER_BY_START_DATE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Loc_SeeLocations (void);
void Loc_EditLocations (void);
void Loc_GetListLocations (void);
void Loc_FreeListLocations (void);

long Loc_GetParamLocCod (void);
void Loc_RemoveLocation (void);
void Loc_ChangeStartDate (void);
void Loc_ChangeEndDate (void);
void Loc_RenameLocation (void);
void Loc_RecFormNewLocation (void);

#endif
