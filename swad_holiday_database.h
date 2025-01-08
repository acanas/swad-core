// swad_holiday_database.h: holidays operations with database

#ifndef _SWAD_HLD_DB
#define _SWAD_HLD_DB
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_date.h"
#include "swad_holiday.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Hld_DB_CreateHoliday (const struct Hld_Holiday *Hld);
void Hld_DB_ChangePlace (long HldCod,long PlcCod);
void Hld_DB_ChangeType (long HldCod,Hld_HolidayType_t HldTyp);
void Hld_DB_ChangeDate (long HldCod,const char *StrStartOrEndDate,
                        const struct Dat_Date *NewDate);
void Hld_DB_ChangeName (long HldCod,char NewHldName[Hld_MAX_BYTES_HOLIDAY_NAME + 1]);

unsigned Hld_DB_GetListHolidays (MYSQL_RES **mysql_res,Hld_Order_t SelectedOrder);
unsigned Hld_DB_GetHolidayDataByCod (MYSQL_RES **mysql_res,long HldCod);

void Hld_DB_RemoveHoliday (long HldCod);

#endif
