// swad_institution_database.c: institutions operations with database

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_institution.h"
#include "swad_institution_database.h"

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
/************************** Create a new institution *************************/
/*****************************************************************************/

long Ins_DB_CreateInstitution (const struct Ins_Instit *Ins,unsigned Status)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create institution",
				"INSERT INTO ins_instits"
				" (CtyCod,Status,RequesterUsrCod,"
				  "ShortName,FullName,WWW)"
				" VALUES"
				" (%ld,%u,%ld,"
				  "'%s','%s','%s')",
				Ins->CtyCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Ins->ShrtName,
				Ins->FullName,
				Ins->WWW);
  }

/*****************************************************************************/
/****************** Update country in table of institutions ******************/
/*****************************************************************************/

void Ins_DB_UpdateInsCty (long InsCod,long CtyCod)
  {
   DB_QueryUPDATE ("can not update the country of an institution",
		   "UPDATE ins_instits"
		     " SET CtyCod=%ld"
		   " WHERE InsCod=%ld",
                   CtyCod,
                   InsCod);
  }

/*****************************************************************************/
/****************** Update status in table of institutions *******************/
/*****************************************************************************/

void Ins_DB_UpdateInsStatus (long InsCod,Ins_Status_t Status)
  {
   DB_QueryUPDATE ("can not update the status of an institution",
		   "UPDATE ins_instits"
		     " SET Status=%u"
		   " WHERE InsCod=%ld",
                   (unsigned) Status,
                   InsCod);
  }

/*****************************************************************************/
/*************************** Update institution name *************************/
/*****************************************************************************/

void Ins_DB_UpdateInsName (long InsCod,const char *FieldName,const char *NewInsName)
  {
   /***** Update institution changing old name by new name */
   DB_QueryUPDATE ("can not update the name of an institution",
		   "UPDATE ins_instits"
		     " SET %s='%s'"
		   " WHERE InsCod=%ld",
	           FieldName,NewInsName,
	           InsCod);
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

void Ins_DB_UpdateInsWWW (long InsCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   /***** Update database changing old WWW by new WWW *****/
   DB_QueryUPDATE ("can not update the web of an institution",
		   "UPDATE ins_instits"
		     " SET WWW='%s'"
		   " WHERE InsCod=%ld",
	           NewWWW,
	           InsCod);
  }

/*****************************************************************************/
/************************* Get data of an institution ************************/
/*****************************************************************************/

unsigned Ins_DB_GetDataOfInstitutionByCod (MYSQL_RES **mysql_res,long InsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of an institution",
		   "SELECT InsCod,"		// row[0]
			  "CtyCod,"		// row[1]
			  "Status,"		// row[2]
			  "RequesterUsrCod,"	// row[3]
			  "ShortName,"		// row[4]
			  "FullName,"		// row[5]
			  "WWW"			// row[6]
		    " FROM ins_instits"
		   " WHERE InsCod=%ld",
		   InsCod);
  }

/*****************************************************************************/
/*********** Get the short name of an institution from its code **************/
/*****************************************************************************/

void Ins_DB_GetShortNameOfInstitution (long InsCod,char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1])
  {
   /***** Trivial check: institution code should be > 0 *****/
   if (InsCod <= 0)
     {
      ShrtName[0] = '\0';	// Empty name
      return;
     }

   /***** Get short name of institution from database *****/
   DB_QuerySELECTString (ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME,
			 "can not get the short name of an institution",
			 "SELECT ShortName"
			  " FROM ins_instits"
			 " WHERE InsCod=%ld",
			 InsCod);
  }

/*****************************************************************************/
/******** Get short name and country of an institution from its code *********/
/*****************************************************************************/

unsigned Ins_DB_GetShrtNameAndCtyOfIns (MYSQL_RES **mysql_res,long InsCod)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get short name and country"
			    " of an institution",
		   "SELECT ins_instits.ShortName,"	// row[0]
			  "cty_countrs.Name_%s"		// row[1]
		    " FROM ins_instits,"
			  "cty_countrs"
		   " WHERE ins_instits.InsCod=%ld"
		     " AND ins_instits.CtyCod=cty_countrs.CtyCod",
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   InsCod);
  }

/*****************************************************************************/
/****** Check if the name of institution exists in the current country *******/
/*****************************************************************************/

bool Ins_DB_CheckIfInsNameExistsInCty (const char *FieldName,
                                       const char *Name,
				       long InsCod,
				       long CtyCod)
  {
   return (DB_QueryCOUNT ("can not check if the name of an institution"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM ins_instits"
			  " WHERE CtyCod=%ld"
			    " AND %s='%s'"
			    " AND InsCod<>%ld",
			  CtyCod,FieldName,Name,InsCod) != 0);
  }

/*****************************************************************************/
/***************** Get all institutions with pending centers *****************/
/*****************************************************************************/

unsigned Ins_DB_GetAllInsWithPendingCtr (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions with pending centers",
		   "SELECT ctr_centers.InsCod,"	// row[0]
			  "COUNT(*)"		// row[1]
		    " FROM ctr_centers,"
			  "ins_instits"
		   " WHERE (ctr_centers.Status & %u)<>0"
		     " AND ctr_centers.InsCod=ins_instits.InsCod"
		   " GROUP BY ctr_centers.InsCod"
		   " ORDER BY ins_instits.ShortName",
		   (unsigned) Ctr_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/************* Get institutions with pending centers admin by me *************/
/*****************************************************************************/

unsigned Ins_DB_GetInsWithPendingCtrsAdminByMe (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions with pending centers",
		   "SELECT ctr_centers.InsCod,"	// row[0]
			  "COUNT(*)"		// row[1]
		    " FROM ctr_centers,"
			  "ins_admin,"
			  "ins_instits"
		   " WHERE (ctr_centers.Status & %u)<>0"
		     " AND ctr_centers.InsCod=ins_admin.InsCod"
		     " AND ins_admin.UsrCod=%ld"
		     " AND ctr_centers.InsCod=ins_instits.InsCod"
		   " GROUP BY ctr_centers.InsCod"
		   " ORDER BY ins_instits.ShortName",
		   (unsigned) Ctr_STATUS_BIT_PENDING,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************* Get institutions in country ordered by short name *************/
/*****************************************************************************/

unsigned Ins_DB_GetInssInCtyOrderedByShrtName (MYSQL_RES **mysql_res,long CtyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions",
		   "SELECT DISTINCT InsCod,"	// row[0]
				   "ShortName"	// row[1]
		    " FROM ins_instits"
		   " WHERE CtyCod=%ld"
		   " ORDER BY ShortName",
		   CtyCod);
  }

/*****************************************************************************/
/******* Get basic list of institutions ordered by name of institution *******/
/*****************************************************************************/

unsigned Ins_DB_GetInssInCtyOrderedByFullName (MYSQL_RES **mysql_res,long CtyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions",
		   "SELECT InsCod,"		// row[0]
			  "CtyCod,"		// row[1]
			  "Status,"		// row[2]
			  "RequesterUsrCod,"	// row[3]
			  "ShortName,"		// row[4]
			  "FullName,"		// row[5]
			  "WWW"			// row[6]
		    " FROM ins_instits"
		   " WHERE CtyCod=%ld"
		   " ORDER BY FullName",
		   CtyCod);
  }
/*****************************************************************************/
/************* Get full list of institutions                    **************/
/************* with number of users who claim to belong to them **************/
/*****************************************************************************/

unsigned Ins_DB_GetFullListOfInssInCty (MYSQL_RES **mysql_res,long CtyCod)
  {
   static const char *OrderBySubQuery[Ins_NUM_ORDERS] =
     {
      [Ins_ORDER_BY_INSTITUTION] = "FullName",
      [Ins_ORDER_BY_NUM_USRS   ] = "NumUsrs DESC,FullName",
     };

   /***** Get institutions from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions",
		   "(SELECT ins_instits.InsCod,"		// row[0]
			   "ins_instits.CtyCod,"		// row[1]
			   "ins_instits.Status,"		// row[2]
			   "ins_instits.RequesterUsrCod,"	// row[3]
			   "ins_instits.ShortName,"		// row[4]
			   "ins_instits.FullName,"		// row[5]
			   "ins_instits.WWW,"			// row[6]
			   "COUNT(*) AS NumUsrs"		// row[7]
		    " FROM ins_instits,"
			  "usr_data"
		   " WHERE ins_instits.CtyCod=%ld"
		     " AND ins_instits.InsCod=usr_data.InsCod"
		   " GROUP BY ins_instits.InsCod)"
		   " UNION "
		   "(SELECT InsCod,"				// row[0]
			   "CtyCod,"				// row[1]
			   "Status,"				// row[2]
			   "RequesterUsrCod,"			// row[3]
			   "ShortName,"				// row[4]
			   "FullName,"				// row[5]
			   "WWW,"				// row[6]
			   "0 AS NumUsrs"			// row[7]
		     " FROM ins_instits"
		    " WHERE CtyCod=%ld"
		      " AND InsCod NOT IN"
			  " (SELECT DISTINCT InsCod"
			     " FROM usr_data))"
			    " ORDER BY %s",
		   CtyCod,
		   CtyCod,
		   OrderBySubQuery[Gbl.Hierarchy.Inss.SelectedOrder]);
  }

/*****************************************************************************/
/**************** Get number of institutions in a country ********************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssInCty (long CtyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of institutions in a country",
		  "SELECT COUNT(*)"
		   " FROM ins_instits"
		  " WHERE CtyCod=%ld",
		  CtyCod);
  }

/*****************************************************************************/
/****************** Get number of institutions with centres ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithCtrs (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with centers",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers"
		  " WHERE %sinstitutions.InsCod=ctr_centers.InsCod",
		  SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with degrees ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithDegs (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with degrees",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees"
		  " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with courses ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithCrss (HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with courses",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
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
/************* Get current number of institutions with users *****************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInnsWithUsrs (Rol_Role_t Role,
                                    HieLvl_Level_t Scope,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Scope,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with users",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses,"
			 "crs_users"
		  " WHERE %s"
			 "ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod"
		    " AND crs_courses.CrsCod=crs_users.CrsCod"
		    " AND crs_users.Role=%u",
		  SubQuery,(unsigned) Role);
  }

/*****************************************************************************/
/********** Check if any of the centers in an institution has map ************/
/*****************************************************************************/

bool Ins_DB_GetIfMapIsAvailable (long InsCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool MapIsAvailable = false;

   /***** Get if any center in current institution has a coordinate set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   if (DB_QuerySELECT (&mysql_res,"can not get if map is available",
		       "SELECT EXISTS"	// row[0]
		       "(SELECT *"
		         " FROM ctr_centers"
		        " WHERE InsCod=%ld"
		          " AND (Latitude<>0"
		               " OR"
		               " Longitude<>0))",
		       InsCod))
     {
      /* Get if map is available */
      row = mysql_fetch_row (mysql_res);
      MapIsAvailable = (row[0][0] == '1');
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return MapIsAvailable;
  }

/*****************************************************************************/
/********* Get average coordinates of centers in current institution *********/
/*****************************************************************************/

void Ins_DB_GetCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom)
  {
   char *Query;

   /***** Get average coordinates of centers of current institution
          with both coordinates set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   if (asprintf (&Query,
		 "SELECT AVG(Latitude),"				// row[0]
			"AVG(Longitude),"				// row[1]
			"GREATEST(MAX(Latitude)-MIN(Latitude),"
				 "MAX(Longitude)-MIN(Longitude))"	// row[2]
		  " FROM ctr_centers"
		 " WHERE InsCod=%ld"
		   " AND Latitude<>0"
		   " AND Longitude<>0",
		 Gbl.Hierarchy.Ins.InsCod) < 0)
      Err_NotEnoughMemoryExit ();
   Map_GetCoordAndZoom (Coord,Zoom,Query);
   free (Query);
  }

/*****************************************************************************/
/******** Get centres which have coordinates in current institution **********/
/*****************************************************************************/

unsigned Ins_DB_GetCtrsWithCoordsInCurrentIns (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get centers with coordinates",
		   "SELECT CtrCod"	// row[0]
		    " FROM ctr_centers"
		   " WHERE InsCod=%ld"
		     " AND Latitude<>0"
		     " AND Longitude<>0",
		   Gbl.Hierarchy.Ins.InsCod);
  }

/*****************************************************************************/
/***************************** Remove institution ****************************/
/*****************************************************************************/

void Ins_DB_RemoveInstitution (long InsCod)
  {
   DB_QueryDELETE ("can not remove an institution",
		   "DELETE FROM ins_instits"
		   " WHERE InsCod=%ld",
		   InsCod);
  }
