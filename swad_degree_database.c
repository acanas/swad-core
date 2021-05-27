// swad_degree_database.c: degrees operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

// #include <stdbool.h>		// For boolean type
// #include <stddef.h>		// For NULL
// #include <stdlib.h>		// For free
// #include <string.h>		// For string functions

#include "swad_database.h"
// #include "swad_degree.h"
// #include "swad_degree_config.h"
#include "swad_degree_database.h"
#include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_figure_cache.h"
// #include "swad_form.h"
// #include "swad_forum.h"
#include "swad_global.h"
// #include "swad_hierarchy.h"
// #include "swad_HTML.h"
// #include "swad_logo.h"
// #include "swad_message.h"
// #include "swad_survey.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Public constants ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private types *********************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

/*****************************************************************************/
/*************************** Private prototypes ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************************** Create a new degree type *************************/
/*****************************************************************************/

void Deg_DB_CreateDegreeType (const char DegTypName[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1])
  {
   DB_QueryINSERT ("can not create a new type of degree",
		   "INSERT INTO deg_types"
		     " SET DegTypName='%s'",
                   DegTypName);
  }

/*****************************************************************************/
/***************************** Create a new degree ***************************/
/*****************************************************************************/

void Deg_DB_CreateDegree (struct Deg_Degree *Deg,unsigned Status)
  {
   Deg->DegCod =
   DB_QueryINSERTandReturnCode ("can not create a new degree",
				"INSERT INTO deg_degrees"
				" (CtrCod,DegTypCod,Status,"
			 	  "RequesterUsrCod,ShortName,FullName,WWW)"
				" VALUES"
				" (%ld,%ld,%u,"
				  "%ld,'%s','%s','%s')",
				Deg->CtrCod,
				Deg->DegTypCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Deg->ShrtName,
				Deg->FullName,
				Deg->WWW);
  }

/*****************************************************************************/
/**************** Create a list with all the degree types ********************/
/*****************************************************************************/

unsigned Deg_DB_GetDegreeTypes (MYSQL_RES **mysql_res,
                                HieLvl_Level_t Scope,DegTyp_Order_t Order)
  {
   static const char *OrderBySubQuery[DegTyp_NUM_ORDERS] =
     {
      [DegTyp_ORDER_BY_DEGREE_TYPE] = "DegTypName",
      [DegTyp_ORDER_BY_NUM_DEGREES] = "NumDegs DESC,DegTypName",
     };

   /***** Get types of degree from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
	 /* Get
	    all degree types with degrees
	    union with
	    all degree types without any degree */
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get types of degree",
			 "(SELECT deg_types.DegTypCod,"			// row[0]
				 "deg_types.DegTypName,"		// row[1]
				 "COUNT(deg_degrees.DegCod) AS NumDegs"	// row[2]
			   " FROM deg_degrees,"
			         "deg_types"
			 " WHERE deg_degrees.DegTypCod=deg_types.DegTypCod"
			 " GROUP BY deg_degrees.DegTypCod)"
			 " UNION "
			 "(SELECT DegTypCod,"				// row[0]
				 "DegTypName,"				// row[1]
				 "0 AS NumDegs"				// row[2]
				 // do not use '0' because
				 // NumDegs will be casted to string
				 // and order will be wrong
			  " FROM deg_types"
			 " WHERE DegTypCod NOT IN"
			       " (SELECT DegTypCod"
			          " FROM deg_degrees))"
			 " ORDER BY %s",
			 OrderBySubQuery[Order]);
      case HieLvl_CTY:
	 /* Get only degree types with degrees in the current country */
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get types of degree",
			 "SELECT deg_types.DegTypCod,"			// row[0]
			        "deg_types.DegTypName,"			// row[1]
			        "COUNT(deg_degrees.DegCod) AS NumDegs"	// row[2]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "deg_types"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegTypCod=deg_types.DegTypCod"
			 " GROUP BY deg_degrees.DegTypCod"
			 " ORDER BY %s",
			 Gbl.Hierarchy.Cty.CtyCod,
			 OrderBySubQuery[Order]);
      case HieLvl_INS:
	 /* Get only degree types with degrees in the current institution */
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get types of degree",
			 "SELECT deg_types.DegTypCod,"			// row[0]
			        "deg_types.DegTypName,"			// row[1]
			        "COUNT(deg_degrees.DegCod) AS NumDegs"	// row[2]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "deg_types"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegTypCod=deg_types.DegTypCod"
			 " GROUP BY deg_degrees.DegTypCod"
			 " ORDER BY %s",
			 Gbl.Hierarchy.Ins.InsCod,
			 OrderBySubQuery[Order]);
      case HieLvl_CTR:
	 /* Get only degree types with degrees in the current center */
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get types of degree",
			 "SELECT deg_types.DegTypCod,"			// row[0]
			        "deg_types.DegTypName,"			// row[1]
			        "COUNT(deg_degrees.DegCod) AS NumDegs"	// row[2]
			  " FROM deg_degrees,"
			        "deg_types"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegTypCod=deg_types.DegTypCod"
			 " GROUP BY deg_degrees.DegTypCod"
			 " ORDER BY %s",
			 Gbl.Hierarchy.Ctr.CtrCod,
			 OrderBySubQuery[Order]);
      case HieLvl_DEG:
      case HieLvl_CRS:
	 /* Get only degree types with degrees in the current degree */
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get types of degree",
			 "SELECT deg_types.DegTypCod,"			// row[0]
			        "deg_types.DegTypName,"			// row[1]
			        "COUNT(deg_degrees.DegCod) AS NumDegs"	// row[2]
			  " FROM deg_degrees,"
			        "deg_types"
			 " WHERE deg_degrees.DegCod=%ld"
			   " AND deg_degrees.DegTypCod=deg_types.DegTypCod"
			 " GROUP BY deg_degrees.DegTypCod"
			 " ORDER BY %s",
			 Gbl.Hierarchy.Deg.DegCod,
			 OrderBySubQuery[Order]);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************** Get the name of a type of degree from database ***************/
/*****************************************************************************/

void Deg_DB_GetDegTypeNameByCod (struct DegreeType *DegTyp)
  {
   DB_QuerySELECTString (DegTyp->DegTypName,sizeof (DegTyp->DegTypName) - 1,
		         "can not get the name of a type of degree",
		         "SELECT DegTypName"
			  " FROM deg_types"
		         " WHERE DegTypCod=%ld",
		         DegTyp->DegTypCod);
  }

/*****************************************************************************/
/********************* Get data of a degree from its code ********************/
/*****************************************************************************/

unsigned Deg_DB_GetDataOfDegreeByCod (MYSQL_RES **mysql_res,long DegCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a degree",
		   "SELECT DegCod,"		// row[0]
			  "CtrCod,"		// row[1]
			  "DegTypCod,"		// row[2]
			  "Status,"		// row[3]
			  "RequesterUsrCod,"	// row[4]
			  "ShortName,"		// row[5]
			  "FullName,"		// row[6]
			  "WWW"			// row[7]
		    " FROM deg_degrees"
		   " WHERE DegCod=%ld",
		   DegCod);
  }

/*****************************************************************************/
/************* Get the center code of a degree from its code *****************/
/*****************************************************************************/

long Deg_DB_GetCtrCodOfDegreeByCod (long DegCod)
  {
   /***** Trivial check: degree code should be > 0 *****/
   if (DegCod <= 0)
      return -1L;

   /***** Get the center code of a degree from database *****/
   return DB_QuerySELECTCode ("can not get the center of a degree",
			      "SELECT CtrCod"
			       " FROM deg_degrees"
			      " WHERE DegCod=%ld",
			      DegCod);
  }

/*****************************************************************************/
/************* Get the short name of a degree from its code ******************/
/*****************************************************************************/

void Deg_DB_GetShortNameOfDegreeByCod (struct Deg_Degree *Deg)
  {
   /***** Trivial check: degree code should be > 0 *****/
   if (Deg->DegCod > 0)
     {
      Deg->ShrtName[0] = '\0';
      return;
     }

   /***** Get the short name of a degree from database *****/
   DB_QuerySELECTString (Deg->ShrtName,sizeof (Deg->ShrtName) - 1,
			 "can not get the short name of a degree",
			 "SELECT ShortName"
			  " FROM deg_degrees"
			 " WHERE DegCod=%ld",
			 Deg->DegCod);
  }

/*****************************************************************************/
/****************** Check if name of degree type exists **********************/
/*****************************************************************************/

bool Deg_DB_CheckIfDegreeTypeNameExists (const char *DegTypName,long DegTypCod)
  {
   /***** Get number of degree types with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a type of degree"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM deg_types"
			  " WHERE DegTypName='%s'"
			    " AND DegTypCod<>%ld",
			  DegTypName,DegTypCod) != 0);
  }

/*****************************************************************************/
/*************** Get number of degrees of a type from database ***************/
/*****************************************************************************/

unsigned Deg_DB_GetNumDegsOfType (long DegTypCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of degrees of a type",
		  "SELECT COUNT(*)"
		   " FROM deg_degrees"
		  " WHERE DegTypCod=%ld",
		  DegTypCod);
  }

/*****************************************************************************/
/******************** Get degrees of a type from database ********************/
/*****************************************************************************/

unsigned Deg_DB_GetDegsOfType (MYSQL_RES **mysql_res,long DegTypCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get degrees of a type",
		   "SELECT DegCod"
		    " FROM deg_degrees"
		   " WHERE DegTypCod=%ld",
		   DegTypCod);
  }

/*****************************************************************************/
/********* Get degrees belonging to the current center from database *********/
/*****************************************************************************/

unsigned Deg_DB_GetDegsOfCurrentCtr (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get degrees of a center",
		   "SELECT DegCod,"	// row[0]
			  "ShortName"	// row[1]
		    " FROM deg_degrees"
		   " WHERE CtrCod=%ld"
		   " ORDER BY ShortName",
		   Gbl.Hierarchy.Ctr.CtrCod);
  }

/*****************************************************************************/
/******************** Get degrees with pending courses ***********************/
/*****************************************************************************/

unsigned Deg_DB_GetDegsWithPendingCrss (MYSQL_RES **mysql_res)
  {
   /***** Get degrees with pending courses *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_DEG_ADM:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get degrees with pending courses",
			 "SELECT crs_courses.DegCod,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM usr_admins,"
			        "crs_courses,"
			        "deg_degrees"
			 " WHERE usr_admins.UsrCod=%ld"
			   " AND usr_admins.Scope='%s'"
			   " AND usr_admins.Cod=crs_courses.DegCod"
			   " AND (crs_courses.Status & %u)<>0"
			   " AND crs_courses.DegCod=deg_degrees.DegCod"
			 " GROUP BY crs_courses.DegCod"
			 " ORDER BY deg_degrees.ShortName",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 Sco_GetDBStrFromScope (HieLvl_DEG),
			 (unsigned) Crs_STATUS_BIT_PENDING);
      case Rol_SYS_ADM:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get degrees with pending courses",
			 "SELECT crs_courses.DegCod,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM crs_courses,"
			        "deg_degrees"
			 " WHERE (crs_courses.Status & %u)<>0"
			   " AND crs_courses.DegCod=deg_degrees.DegCod"
			 " GROUP BY crs_courses.DegCod"
			 " ORDER BY deg_degrees.ShortName",
			 (unsigned) Crs_STATUS_BIT_PENDING);
      default:	// Forbidden for other users
	 Err_WrongRoleExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************************* Update degree type name ***************************/
/*****************************************************************************/

void Deg_DB_UpdateDegTypName (long DegTypCod,const char NewNameDegTyp[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1])
  {
   DB_QueryUPDATE ("can not update the type of a degree",
		   "UPDATE deg_types"
		     " SET DegTypName='%s'"
		   " WHERE DegTypCod=%ld",
		   NewNameDegTyp,
		   DegTypCod);
  }

/*****************************************************************************/
/**************** Update degree center in table of degrees *******************/
/*****************************************************************************/

void Deg_DB_UpdateDegCtr (long DegCod,long CtrCod)
  {
   DB_QueryUPDATE ("can not update the center of a degree",
		   "UPDATE deg_degrees"
		     " SET CtrCod=%ld"
		   " WHERE DegCod=%ld",
                   CtrCod,
                   DegCod);
  }

/*****************************************************************************/
/*************************** Remove the degree type **************************/
/*****************************************************************************/

void Deg_DB_RemoveDegTyp (long DegTypCod)
  {
   DB_QueryDELETE ("can not remove a type of degree",
		   "DELETE FROM deg_types"
		   " WHERE DegTypCod=%ld",
		   DegTypCod);
  }
