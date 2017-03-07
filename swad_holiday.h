// swad_holiday.h: holidays

#ifndef _SWAD_HLD
#define _SWAD_HLD
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#define Hld_MAX_CHARS_HOLIDAY_NAME	(128 - 1)
#define Hld_MAX_BYTES_HOLIDAY_NAME	(Hld_MAX_CHARS_HOLIDAY_NAME * Str_MAX_BYTES_PER_CHAR)

#define Hld_NUM_TYPES_HOLIDAY 2
typedef enum
  {
   Hld_HOLIDAY           = 0,
   Hld_NON_SCHOOL_PERIOD = 1,
  } Hld_HolidayType_t;
#define Hld_HOLIDAY_TYPE_DEFAULT Hld_HOLIDAY

typedef enum
  {
   HLD_START_DATE,
   HLD_END_DATE
  } Hld_StartOrEndDate_t;

struct Holiday
  {
   long HldCod;
   long PlcCod;
   char PlaceFullName[Plc_MAX_BYTES_PLACE_FULL_NAME + 1];
   Hld_HolidayType_t HldTyp;
   struct Date StartDate;
   struct Date EndDate;
   char Name[Hld_MAX_BYTES_HOLIDAY_NAME + 1];
  };

#define Hld_NUM_ORDERS 2
typedef enum
  {
   Hld_ORDER_BY_PLACE      = 0,
   Hld_ORDER_BY_START_DATE = 1,
  } Hld_Order_t;
#define Hld_DEFAULT_ORDER_TYPE Hld_ORDER_BY_START_DATE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Hld_SeeHolidays (void);
void Hld_EditHolidays (void);
void Hld_GetListHolidays (void);
void Hld_FreeListHolidays (void);

long Hld_GetParamHldCod (void);
void Hld_RemoveHoliday (void);
void Hld_ChangeHolidayPlace (void);
void Hld_ChangeHolidayType (void);
void Hld_ChangeStartDate (void);
void Hld_ChangeEndDate (void);
void Hld_RenameHoliday (void);
void Hld_RecFormNewHoliday (void);

#endif
