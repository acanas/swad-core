// swad_center_database.c: centers operations with database

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

#include "swad_center.h"
#include "swad_center_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy.h"

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
/***************************** Create a new center ***************************/
/*****************************************************************************/

long Ctr_DB_CreateCenter (const struct Ctr_Center *Ctr,Ctr_Status_t Status)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create a new center",
				"INSERT INTO ctr_centers"
				" (InsCod,PlcCod,Status,RequesterUsrCod,"
				  "ShortName,FullName,WWW,PhotoAttribution)"
				" VALUES"
				" (%ld,%ld,%u,%ld,"
				  "'%s','%s','%s','')",
				Ctr->InsCod,
				Ctr->PlcCod,
				(unsigned) Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Ctr->ShrtName,
				Ctr->FullName,
				Ctr->WWW);
  }

/*****************************************************************************/
/************ Get basic list of centers ordered by name of center ************/
/*****************************************************************************/

unsigned Ctr_DB_GetListOfCtrsInCurrentIns (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get centers",
		   "SELECT DISTINCT CtrCod,"	// row[0]
				   "ShortName"	// row[1]
		    " FROM ctr_centers"
		   " WHERE InsCod=%ld"
		   " ORDER BY ShortName",
		   Gbl.Hierarchy.Ins.InsCod);
  }

/*****************************************************************************/
/************ Get full list of centers ordered by name of center ************/
/*****************************************************************************/

unsigned Ctr_DB_GetListOfCtrsFull (MYSQL_RES **mysql_res,long InsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get centers",
		   "SELECT CtrCod,"		// row[ 0]
			  "InsCod,"		// row[ 1]
			  "PlcCod,"		// row[ 2]
			  "Status,"		// row[ 3]
			  "RequesterUsrCod,"	// row[ 4]
			  "Latitude,"		// row[ 5]
			  "Longitude,"		// row[ 6]
			  "Altitude,"		// row[ 7]
			  "ShortName,"		// row[ 8]
			  "FullName,"		// row[ 9]
			  "WWW"			// row[10]
		    " FROM ctr_centers"
		   " WHERE InsCod=%ld"
		   " ORDER BY FullName",
		   InsCod);
  }

/*****************************************************************************/
/************* Get full list of centers                         **************/
/************* with number of users who claim to belong to them **************/
/*****************************************************************************/

unsigned Ctr_DB_GetListOfCtrsFullWithNumUsrs (MYSQL_RES **mysql_res,
                                              long InsCod,Ctr_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Ctr_NUM_ORDERS] =
     {
      [Ctr_ORDER_BY_CENTER  ] = "FullName",
      [Ctr_ORDER_BY_NUM_USRS] = "NumUsrs DESC,"
	                        "FullName",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get centers",
		   "(SELECT ctr_centers.CtrCod,"		// row[ 0]
			   "ctr_centers.InsCod,"		// row[ 1]
			   "ctr_centers.PlcCod,"		// row[ 2]
			   "ctr_centers.Status,"		// row[ 3]
			   "ctr_centers.RequesterUsrCod,"	// row[ 4]
			   "ctr_centers.Latitude,"		// row[ 5]
			   "ctr_centers.Longitude,"		// row[ 6]
			   "ctr_centers.Altitude,"		// row[ 7]
			   "ctr_centers.ShortName,"		// row[ 8]
			   "ctr_centers.FullName,"		// row[ 9]
			   "ctr_centers.WWW,"			// row[10]
			   "COUNT(*) AS NumUsrs"		// row[11]
		    " FROM ctr_centers,usr_data"
		   " WHERE ctr_centers.InsCod=%ld"
		     " AND ctr_centers.CtrCod=usr_data.CtrCod"
		   " GROUP BY ctr_centers.CtrCod)"
		   " UNION "
		   "(SELECT CtrCod,"				// row[ 0]
			   "InsCod,"				// row[ 1]
			   "PlcCod,"				// row[ 2]
			   "Status,"				// row[ 3]
			   "RequesterUsrCod,"			// row[ 4]
			   "Latitude,"				// row[ 5]
			   "Longitude,"				// row[ 6]
			   "Altitude,"				// row[ 7]
			   "ShortName,"				// row[ 8]
			   "FullName,"				// row[ 9]
			   "WWW,"				// row[10]
			   "0 AS NumUsrs"			// row[11]
		    " FROM ctr_centers"
		   " WHERE InsCod=%ld"
		     " AND CtrCod NOT IN"
		         " (SELECT DISTINCT CtrCod"
			    " FROM usr_data))"
		   " ORDER BY %s",
		   InsCod,
		   InsCod,
		   OrderBySubQuery[SelectedOrder]);
  }

/*****************************************************************************/
/******************* Get centers with pending degrees ***********************/
/*****************************************************************************/

unsigned Ctr_DB_GetCtrsWithPendingDegs (MYSQL_RES **mysql_res)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_CTR_ADM:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get centers with pending degrees",
			 "SELECT deg_degrees.CtrCod,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM deg_degrees,"
			        "ctr_admin,"
			        "ctr_centers"
			 " WHERE (deg_degrees.Status & %u)<>0"
			   " AND deg_degrees.CtrCod=ctr_admin.CtrCod"
			   " AND ctr_admin.UsrCod=%ld"
			   " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			 " GROUP BY deg_degrees.CtrCod"
			 " ORDER BY ctr_centers.ShortName",
			 (unsigned) Deg_STATUS_BIT_PENDING,
			 Gbl.Usrs.Me.UsrDat.UsrCod);
      case Rol_SYS_ADM:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get centers with pending degrees",
			 "SELECT deg_degrees.CtrCod,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM deg_degrees,"
			        "ctr_centers"
			 " WHERE (deg_degrees.Status & %u)<>0"
			   " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			 " GROUP BY deg_degrees.CtrCod"
			 " ORDER BY ctr_centers.ShortName",
			 (unsigned) Deg_STATUS_BIT_PENDING);
      default:		// Forbidden for other users
	 Err_WrongRoleExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************************ Get data of center by code *************************/
/*****************************************************************************/

unsigned Ctr_DB_GetDataOfCenterByCod (MYSQL_RES **mysql_res,long CtrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a center",
		   "SELECT CtrCod,"		// row[ 0]
			  "InsCod,"		// row[ 1]
			  "PlcCod,"		// row[ 2]
			  "Status,"		// row[ 3]
			  "RequesterUsrCod,"	// row[ 4]
			  "Latitude,"		// row[ 5]
			  "Longitude,"		// row[ 6]
			  "Altitude,"		// row[ 7]
			  "ShortName,"		// row[ 8]
			  "FullName,"		// row[ 9]
			  "WWW"			// row[10]
		    " FROM ctr_centers"
		   " WHERE CtrCod=%ld",
		   CtrCod);
  }

/*****************************************************************************/
/*********** Get the institution code of a center from its code **************/
/*****************************************************************************/

long Ctr_DB_GetInsCodOfCenterByCod (long CtrCod)
  {
   /***** Trivial check: center code should be > 0 *****/
   if (CtrCod <= 0)
      return -1L;

   /***** Get the institution code of a center from database *****/
   return DB_QuerySELECTCode ("can not get the institution of a center",
			      "SELECT InsCod"
			       " FROM ctr_centers"
			      " WHERE CtrCod=%ld",
			      CtrCod);
  }

/*****************************************************************************/
/*************** Get the short name of a center from its code ****************/
/*****************************************************************************/

void Ctr_DB_GetShortNameOfCenterByCod (long CtrCod,char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1])
  {
   /***** Trivial check: center code should be > 0 *****/
   if (CtrCod <= 0)
     {
      ShrtName[0] = '\0';
      return;
     }

   /***** Get the short name of a center from database *****/
   DB_QuerySELECTString (ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME,
			 "can not get the short name of a center",
		         "SELECT ShortName"
			  " FROM ctr_centers"
		         " WHERE CtrCod=%ld",
		         CtrCod);
  }

/*****************************************************************************/
/******************** Get photo attribution from database ********************/
/*****************************************************************************/

unsigned Ctr_DB_GetPhotoAttribution (MYSQL_RES **mysql_res,long CtrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get photo attribution",
		   "SELECT PhotoAttribution"	// row[0]
		    " FROM ctr_centers"
		   " WHERE CtrCod=%ld",
		   CtrCod);
  }

/*****************************************************************************/
/********************* Check if the name of center exists ********************/
/*****************************************************************************/

bool Ctr_DB_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,
				       long CtrCod,long InsCod)
  {
   /***** Get number of centers with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a center"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM ctr_centers"
			  " WHERE InsCod=%ld"
			    " AND %s='%s'"
			    " AND CtrCod<>%ld",
			  InsCod,
			  FieldName,
			  Name,
			  CtrCod) != 0);
  }

/*****************************************************************************/
/******************** Get number of centers in a country *********************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsInCty (long CtyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers in a country",
		  "SELECT COUNT(*)"
		   " FROM ins_instits,"
		         "ctr_centers"
		  " WHERE ins_instits.CtyCod=%ld"
		    " AND ins_instits.InsCod=ctr_centers.InsCod",
		  CtyCod);
  }

/*****************************************************************************/
/****************** Get number of centers in an institution ******************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsInIns (long InsCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers in an institution",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers"
		  " WHERE InsCod=%ld",
		  InsCod);
  }

/*****************************************************************************/
/******* Get number of centers (of the current institution) in a place *******/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsInPlc (long PlcCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of centers in a place",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers"
		  " WHERE InsCod=%ld"
		    " AND PlcCod=%ld",
		  Gbl.Hierarchy.Ins.InsCod,
		  PlcCod);
  }

/*****************************************************************************/
/*********************** Get number of centers with map **********************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsWithMap (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers with map",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers"
		  " WHERE Latitude<>0"
		     " OR Longitude<>0");
  }

/*****************************************************************************/
/**************** Get number of centers with map in a country ****************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsWithMapInCty (long CtyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers with map",
		  "SELECT COUNT(*)"
		   " FROM ins_instits,"
			 "ctr_centers"
		  " WHERE ins_instits.CtyCod=%ld"
		    " AND ins_instits.InsCod=ctr_centers.InsCod"
		    " AND (ctr_centers.Latitude<>0"
		      " OR ctr_centers.Longitude<>0)",
		  CtyCod);
  }

/*****************************************************************************/
/************* Get number of centers with map in an institution **************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsWithMapInIns (long InsCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers with map",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers"
		  " WHERE InsCod=%ld"
		    " AND (Latitude<>0"
		      " OR Longitude<>0)",
		  InsCod);
  }

/*****************************************************************************/
/********************* Get number of centers with degrees ********************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsWithDegs (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers with degrees",
		  "SELECT COUNT(DISTINCT ctr_centers.CtrCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees"
		  " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  SubQuery);
  }

/*****************************************************************************/
/********************* Get number of centers with courses ********************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsWithCrss (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers with courses",
		  "SELECT COUNT(DISTINCT ctr_centers.CtrCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses"
		  " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  SubQuery);
  }

/*****************************************************************************/
/********************* Get number of centers with users **********************/
/*****************************************************************************/

unsigned Ctr_DB_GetNumCtrsWithUsrs (Rol_Role_t Role,
                                    HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of centers with users",
		  "SELECT COUNT(DISTINCT ctr_centers.CtrCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses,"
			 "crs_users"
		  " WHERE %s"
			 "institutions.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod"
		    " AND crs_courses.CrsCod=crs_users.CrsCod"
		    " AND crs_users.Role=%u",
		  SubQuery,(unsigned) Role);
  }

/*****************************************************************************/
/******************* Update institution in table of centers ******************/
/*****************************************************************************/

void Ctr_DB_UpdateCtrIns (long CtrCod,long NewInsCod)
  {
   DB_QueryUPDATE ("can not update the institution of a center",
		   "UPDATE ctr_centers"
		     " SET InsCod=%ld"
		   " WHERE CtrCod=%ld",
                   NewInsCod,
                   CtrCod);
  }

/*****************************************************************************/
/************** Update database changing old place by new place **************/
/*****************************************************************************/

void Ctr_DB_UpdateCtrPlc (long CtrCod,long NewPlcCod)
  {
   DB_QueryUPDATE ("can not update the place of a center",
		   "UPDATE ctr_centers"
		     " SET PlcCod=%ld"
		   " WHERE CtrCod=%ld",
	           NewPlcCod,
	           CtrCod);
  }

/*****************************************************************************/
/****************** Update center name in table of centers *******************/
/*****************************************************************************/

void Ctr_DB_UpdateCtrName (long CtrCod,const char *FieldName,const char *NewCtrName)
  {
   /***** Update center changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a center",
		   "UPDATE ctr_centers"
		     " SET %s='%s'"
		   " WHERE CtrCod=%ld",
	           FieldName,
	           NewCtrName,
	           CtrCod);
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

void Ctr_DB_UpdateCtrWWW (long CtrCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   DB_QueryUPDATE ("can not update the web of a center",
		   "UPDATE ctr_centers"
		     " SET WWW='%s'"
		   " WHERE CtrCod=%ld",
	           NewWWW,
	           CtrCod);
  }

/*****************************************************************************/
/******* Update the table changing old attribution by new attribution ********/
/*****************************************************************************/

void Ctr_DB_UpdateCtrPhotoAttribution (long CtrCod,const char NewPhotoAttribution[Med_MAX_BYTES_ATTRIBUTION + 1])
  {
   DB_QueryUPDATE ("can not update the photo attribution of a center",
		   "UPDATE ctr_centers"
		     " SET PhotoAttribution='%s'"
		   " WHERE CtrCod=%ld",
	           NewPhotoAttribution,
	           CtrCod);
  }

/*****************************************************************************/
/******** Update database changing old coordinate by new coordinate **********/
/*****************************************************************************/

void Ctr_DB_UpdateCtrCoordinate (long CtrCod,
				 const char *CoordField,double NewCoord)
  {
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   DB_QueryUPDATE ("can not update a coordinate of a center",
		   "UPDATE ctr_centers"
		     " SET %s='%.15lg'"
		   " WHERE CtrCod=%ld",
	           CoordField,
	           NewCoord,
	           CtrCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/******************** Update status in table of centers **********************/
/*****************************************************************************/

void Ctr_DB_UpdateCtrStatus (long CtrCod,Ctr_Status_t NewStatus)
  {
   DB_QueryUPDATE ("can not update the status of a center",
		   "UPDATE ctr_centers"
		     " SET Status=%u"
		   " WHERE CtrCod=%ld",
	           (unsigned) NewStatus,
	           CtrCod);
  }

/*****************************************************************************/
/****************************** Remove a center ******************************/
/*****************************************************************************/

void Ctr_DB_RemoveCenter (long CtrCod)
  {
   DB_QueryDELETE ("can not remove a center",
		   "DELETE FROM ctr_centers"
		   " WHERE CtrCod=%ld",
		   CtrCod);
  }
