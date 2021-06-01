// swad_building_database.c: buildings in a center operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

// #include <malloc.h>		// For calloc, free
// #include <stddef.h>		// For NULL
// #include <string.h>		// For string functions

// #include "swad_box.h"
#include "swad_building.h"
#include "swad_database.h"
// #include "swad_error.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_HTML.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/


/*****************************************************************************/
/************************** Create a new building ****************************/
/*****************************************************************************/

void Bld_DB_CreateBuilding (const struct Bld_Building *Building)
  {
   DB_QueryINSERT ("can not create building",
		   "INSERT INTO buildings"
		   " (CtrCod,ShortName,FullName,Location)"
		   " VALUES"
		   " (%ld,'%s','%s','%s')",
                   Gbl.Hierarchy.Ctr.CtrCod,
		   Building->ShrtName,
		   Building->FullName,
		   Building->Location);
  }

/*****************************************************************************/
/****************** Get list of buildings in current center ******************/
/*****************************************************************************/

unsigned Bld_DB_GetListBuildings (MYSQL_RES **mysql_res,
                                  Bld_WhichData_t WhichData,
                                  Bld_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Bld_NUM_ORDERS] =
     {
      [Bld_ORDER_BY_SHRT_NAME] = "ShortName",
      [Bld_ORDER_BY_FULL_NAME] = "FullName",
      [Bld_ORDER_BY_LOCATION ] = "Location,ShortName",
     };

   /***** Get buildings from database *****/
   switch (WhichData)
     {
      case Bld_ALL_DATA:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get buildings",
		         "SELECT BldCod,"		// row[0]
			        "ShortName,"		// row[1]
			        "FullName,"		// row[2]
			        "Location"		// row[3]
			  " FROM bld_buildings"
		         " WHERE CtrCod=%ld"
		         " ORDER BY %s",
		         Gbl.Hierarchy.Ctr.CtrCod,
		         OrderBySubQuery[SelectedOrder]);
      case Bld_ONLY_SHRT_NAME:
      default:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get buildings",
		         "SELECT BldCod,"		// row[0]
			        "ShortName"		// row[1]
			  " FROM bld_buildings"
		         " WHERE CtrCod=%ld"
		         " ORDER BY ShortName",
		         Gbl.Hierarchy.Ctr.CtrCod);
     }
  }

/*****************************************************************************/
/**************** Get building data giving the building code *****************/
/*****************************************************************************/

unsigned Bld_DB_GetDataOfBuildingByCod (MYSQL_RES **mysql_res,long BldCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a building",
		   "SELECT ShortName,"	// row[0]
			  "FullName,"	// row[1]
			  "Location"	// row[2]
		    " FROM bld_buildings"
		   " WHERE BldCod=%ld",
		   BldCod);
  }

/*****************************************************************************/
/******************** Check if the name of building exists *******************/
/*****************************************************************************/

bool Bld_DB_CheckIfBuildingNameExists (const char *FieldName,const char *Name,long BldCod)
  {
   /***** Get number of buildings with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a building"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM bld_buildings"
			  " WHERE CtrCod=%ld"
			    " AND %s='%s'"
			    " AND BldCod<>%ld",
			  Gbl.Hierarchy.Ctr.CtrCod,
			  FieldName,Name,BldCod) != 0);
  }


/*****************************************************************************/
/*************** Update building changing old name by new name ***************/
/*****************************************************************************/

void Bld_DB_UpdateBuildingName (long BldCod,const char *FieldName,const char *NewBuildingName)
  {
   DB_QueryUPDATE ("can not update the name of a building",
		   "UPDATE bld_buildings"
		     " SET %s='%s'"
		   " WHERE BldCod=%ld",
		   FieldName,
		   NewBuildingName,
		   BldCod);
  }

/*****************************************************************************/
/****************************** Remove building ******************************/
/*****************************************************************************/

void Bld_DB_RemoveBuilding (long BldCod)
  {
   DB_QueryDELETE ("can not remove a building",
		   "DELETE FROM bld_buildings"
		   " WHERE BldCod=%ld",
		   BldCod);
  }

/*****************************************************************************/
/******************** Remove all buildings in a center ***********************/
/*****************************************************************************/

void Bld_DB_RemoveAllBuildingsInCtr (long CtrCod)
  {
   /***** Remove all buildings in center *****/
   DB_QueryDELETE ("can not remove buildings",
		   "DELETE FROM buildings"
                   " WHERE CtrCod=%ld",
		   CtrCod);
  }

