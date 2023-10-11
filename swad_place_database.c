// swad_place_database.c: places, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_place.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Create a new place *****************************/
/*****************************************************************************/

void Plc_DB_CreatePlace (const struct Plc_Place *Plc)
  {
   DB_QueryINSERT ("can not create place",
		   "INSERT INTO plc_places"
		   " (InsCod,ShortName,FullName)"
		   " VALUES"
		   " (%ld,'%s','%s')",
                   Gbl.Hierarchy.Node[Hie_INS].HieCod,
                   Plc->ShrtName,
                   Plc->FullName);
  }

/*****************************************************************************/
/****************** Update place name in table of places *********************/
/*****************************************************************************/

void Plc_DB_UpdatePlcName (long PlcCod,
			   const char *FldName,const char *NewPlcName)
  {
   /***** Update place changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a place",
		   "UPDATE plc_places"
		     " SET %s='%s'"
		   " WHERE PlcCod=%ld",
		   FldName,NewPlcName,
		   PlcCod);
  }

/*****************************************************************************/
/************************** Get list of all places ***************************/
/*****************************************************************************/

unsigned Plc_DB_GetListPlaces (MYSQL_RES **mysql_res,Plc_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Plc_NUM_ORDERS] =
     {
      [Plc_ORDER_BY_PLACE   ] = "FullName",
      [Plc_ORDER_BY_NUM_CTRS] = "NumCtrs DESC,"
	                        "FullName",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get places",
		   "(SELECT plc_places.PlcCod,"		// row[0]
			   "plc_places.ShortName,"	// row[1]
			   "plc_places.FullName,"	// row[2]
			   "COUNT(*) AS NumCtrs"	// row[3]
		     " FROM plc_places,"
			   "ctr_centers"
		    " WHERE plc_places.InsCod=%ld"
		      " AND plc_places.PlcCod=ctr_centers.PlcCod"
		      " AND ctr_centers.InsCod=%ld"
		 " GROUP BY plc_places.PlcCod)"
		    " UNION "
		   "(SELECT PlcCod,"
			   "ShortName,"
			   "FullName,"
			   "0 AS NumCtrs"
		     " FROM plc_places"
		    " WHERE InsCod=%ld"
		      " AND PlcCod NOT IN"
			  " (SELECT DISTINCT "
			           "PlcCod"
			     " FROM ctr_centers"
			    " WHERE InsCod=%ld))"
		 " ORDER BY %s",
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   Gbl.Hierarchy.Node[Hie_INS].HieCod,
		   OrderBySubQuery[SelectedOrder]);
  }

/*****************************************************************************/
/********************* Get data of a place from database *********************/
/*****************************************************************************/

unsigned Plc_DB_GetPlaceDataByCod (MYSQL_RES **mysql_res,long PlcCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a place",
		   "(SELECT plc_places.PlcCod,"		// row[0]
			   "plc_places.ShortName,"	// row[1]
			   "plc_places.FullName,"	// row[2]
			   "COUNT(*)"			// row[3]
		     " FROM plc_places,"
			   "ctr_centers"
		    " WHERE plc_places.PlcCod=%ld"
		      " AND plc_places.PlcCod=ctr_centers.PlcCod"
		      " AND ctr_centers.PlcCod=%ld"
		 " GROUP BY plc_places.PlcCod)"
		    " UNION "
		   "(SELECT PlcCod,"			// row[0]
			   "ShortName,"			// row[1]
			   "FullName,"			// row[2]
			   "0"				// row[3]
		     " FROM plc_places"
		    " WHERE PlcCod=%ld"
		      " AND PlcCod NOT IN"
			  " (SELECT DISTINCT "
			           "PlcCod"
			     " FROM ctr_centers))",
		   PlcCod,
		   PlcCod,
		   PlcCod);
   }

/*****************************************************************************/
/********************** Check if the name of place exists ********************/
/*****************************************************************************/

bool Plc_DB_CheckIfPlaceNameExists (const char *FldName,const char *Name,long Cod,
				    long PrtCod,
				    __attribute__((unused)) unsigned Year)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a place already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM plc_places"
		    " WHERE InsCod=%ld"
		      " AND %s='%s'"
		      " AND PlcCod<>%ld)",
		   PrtCod,
		   FldName,Name,
		   Cod);
  }

/*****************************************************************************/
/******************************** Remove place *******************************/
/*****************************************************************************/

void Plc_DB_RemovePlace (long PlcCod)
  {
   DB_QueryDELETE ("can not remove a place",
		   "DELETE FROM plc_places"
		   " WHERE PlcCod=%ld",
		   PlcCod);
  }
