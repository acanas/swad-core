// swad_holiday_database.c: holidays operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_holiday_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************** Create a new holiday or no school period *****************/
/*****************************************************************************/

void Hld_DB_CreateHoliday (const struct Hld_Holiday *Hld)
  {
   DB_QueryINSERT ("can not create holiday",
		   "INSERT INTO hld_holidays"
		   " (InsCod,PlcCod,HldTyp,StartDate,EndDate,Name)"
		   " VALUES"
		   " (%ld,%ld,%u,'%04u%02u%02u','%04u%02u%02u','%s')",
	           Gbl.Hierarchy.Node[Hie_INS].HieCod,
	           Hld->PlcCod,
	           (unsigned) Hld->HldTyp,
	           Hld->StartDate.Year,
	           Hld->StartDate.Month,
	           Hld->StartDate.Day,
	           Hld->EndDate.Year,
	           Hld->EndDate.Month,
	           Hld->EndDate.Day,
	           Hld->Name);
  }

/*****************************************************************************/
/************************* Change the place of a holiday *********************/
/*****************************************************************************/

void Hld_DB_ChangePlace (long HldCod,long PlcCod)
  {
   DB_QueryUPDATE ("can not update the place of a holiday",
		   "UPDATE hld_holidays"
		     " SET PlcCod=%ld"
		   " WHERE HldCod=%ld",
                   PlcCod,
                   HldCod);
  }

/*****************************************************************************/
/************************* Change the type of a holiday **********************/
/*****************************************************************************/

void Hld_DB_ChangeType (long HldCod,Hld_HolidayType_t HldTyp)
  {
   DB_QueryUPDATE ("can not update the type of a holiday",
		   "UPDATE hld_holidays"
		     " SET HldTyp=%u,"
		          "EndDate=StartDate"
		   " WHERE HldCod=%ld",
	           (unsigned) HldTyp,
	           HldCod);
  }

/*****************************************************************************/
/**************** Change the start/end date of a holiday *********************/
/*****************************************************************************/

void Hld_DB_ChangeDate (long HldCod,const char *StrStartOrEndDate,
                        const struct Dat_Date *NewDate)
  {
   DB_QueryUPDATE ("can not update the date of a holiday",
		   "UPDATE hld_holidays"
		     " SET %s='%04u%02u%02u'"
		   " WHERE HldCod=%ld",
	           StrStartOrEndDate,
	           NewDate->Year,
	           NewDate->Month,
	           NewDate->Day,
	           HldCod);
  }

/*****************************************************************************/
/************* Update holiday name changing old name by new name *************/
/*****************************************************************************/

void Hld_DB_ChangeName (long HldCod,char NewHldName[Hld_MAX_BYTES_HOLIDAY_NAME + 1])
  {
   DB_QueryUPDATE ("can not update the text of a holiday",
		   "UPDATE hld_holidays"
		     " SET Name='%s'"
		   " WHERE HldCod=%ld",
		   NewHldName,
		   HldCod);
  }

/*****************************************************************************/
/***************************** List all holidays *****************************/
/*****************************************************************************/

unsigned Hld_DB_GetListHolidays (MYSQL_RES **mysql_res,Hld_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Hld_NUM_ORDERS] =
     {
      [Hld_ORDER_BY_PLACE   ] = "Place,"
	                        "StartDate",
      [Hld_ORDER_BY_STR_DATE] = "StartDate,"
	                        "Place",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get holidays",
		   "(SELECT hld_holidays.HldCod,"						// row[0]
			   "hld_holidays.PlcCod,"						// row[1]
			   "plc_places.FullName as Place,"					// row[2]
			   "hld_holidays.HldTyp,"						// row[3]
			   "DATE_FORMAT(hld_holidays.StartDate,'%%Y%%m%%d') AS StartDate,"	// row[4]
			   "DATE_FORMAT(hld_holidays.EndDate,'%%Y%%m%%d') AS EndDate,"		// row[5]
			   "hld_holidays.Name"							// row[6]
		     " FROM hld_holidays,"
			   "plc_places"
		    " WHERE hld_holidays.InsCod=%ld"
		      " AND hld_holidays.PlcCod=plc_places.PlcCod"
		      " AND plc_places.InsCod=%ld)"
		    " UNION "
		   "(SELECT HldCod,"								// row[0]
			   "PlcCod,"								// row[1]
			   "'' as Place,"							// row[2]
			   "HldTyp,"								// row[3]
			   "DATE_FORMAT(StartDate,'%%Y%%m%%d') AS StartDate,"			// row[4]
			   "DATE_FORMAT(EndDate,'%%Y%%m%%d') AS EndDate,"			// row[5]
			   "Name"								// row[6]
		     " FROM hld_holidays"
		    " WHERE InsCod=%ld"
		      " AND PlcCod NOT IN"
			   "(SELECT DISTINCT "
			           "PlcCod"
			     " FROM plc_places"
			    " WHERE InsCod=%ld))"
		 " ORDER BY %s",
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   OrderBySubQuery[SelectedOrder]);
  }

/*****************************************************************************/
/************************* Get holiday data by code **************************/
/*****************************************************************************/

Exi_Exist_t Hld_DB_GetHolidayDataByCod (MYSQL_RES **mysql_res,long HldCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get data of a holiday",
			 "(SELECT hld_holidays.HldCod,"					// row[0]
				 "hld_holidays.PlcCod,"					// row[1]
				 "plc_places.FullName as Place,"			// row[2]
				 "hld_holidays.HldTyp,"					// row[3]
				 "DATE_FORMAT(hld_holidays.StartDate,'%%Y%%m%%d'),"	// row[4]
				 "DATE_FORMAT(hld_holidays.EndDate,'%%Y%%m%%d'),"	// row[5]
				 "hld_holidays.Name"					// row[6]
			   " FROM hld_holidays,"
				 "plc_places"
			  " WHERE hld_holidays.HldCod=%ld"
			    " AND hld_holidays.InsCod=%ld"
			    " AND hld_holidays.PlcCod=plc_places.PlcCod"
			    " AND plc_places.InsCod=%ld)"
			  " UNION "
			 "(SELECT HldCod,"						// row[0]
				 "PlcCod,"						// row[1]
				 "'' as Place,"						// row[2]
				 "HldTyp,"						// row[3]
				 "DATE_FORMAT(StartDate,'%%Y%%m%%d'),"			// row[4]
				 "DATE_FORMAT(EndDate,'%%Y%%m%%d'),"			// row[5]
				 "Name"							// row[6]
			   " FROM hld_holidays"
			  " WHERE HldCod=%ld"
			    " AND InsCod=%ld"
			    " AND PlcCod NOT IN"
				 "(SELECT DISTINCT "
					 "PlcCod"
				   " FROM plc_places"
				  " WHERE InsCod=%ld))",
			 HldCod,
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 HldCod,
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 Gbl.Hierarchy.Node[Hie_INS].HieCod);
  }

/*****************************************************************************/
/******************************* Remove a holiday ****************************/
/*****************************************************************************/

void Hld_DB_RemoveHoliday (long HldCod)
  {
   DB_QueryDELETE ("can not remove a holiday",
		   "DELETE FROM hld_holidays"
		   " WHERE HldCod=%ld",
		   HldCod);
  }

