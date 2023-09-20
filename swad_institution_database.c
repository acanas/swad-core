// swad_institution_database.c: institutions operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_database.h"
#include "swad_institution.h"
#include "swad_institution_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Create a new institution *************************/
/*****************************************************************************/

long Ins_DB_CreateInstitution (const struct Hie_Node *Ins,Hie_Status_t Status)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create institution",
				"INSERT INTO ins_instits"
				" (CtyCod,Status,RequesterUsrCod,"
				  "ShortName,FullName,WWW)"
				" VALUES"
				" (%ld,%u,%ld,"
				  "'%s','%s','%s')",
				Ins->PrtCod,
				(unsigned) Status,
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

void Ins_DB_UpdateInsStatus (long InsCod,Hie_Status_t Status)
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

void Ins_DB_UpdateInsName (long InsCod,const char *FldName,const char *NewInsName)
  {
   /***** Update institution changing old name by new name */
   DB_QueryUPDATE ("can not update the name of an institution",
		   "UPDATE ins_instits"
		     " SET %s='%s'"
		   " WHERE InsCod=%ld",
	           FldName,NewInsName,
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

unsigned Ins_DB_GetInsDataByCod (MYSQL_RES **mysql_res,long InsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of an institution",
		   "SELECT InsCod,"		// row[0]
			  "CtyCod,"		// row[1]
			  "Status,"		// row[2]
			  "RequesterUsrCod,"	// row[3]
			  "ShortName,"		// row[4]
			  "FullName,"		// row[5]
			  "WWW,"		// row[6]
                  	  "0 AS NumUsrs"	// row[7] (not used)
		    " FROM ins_instits"
		   " WHERE InsCod=%ld",
		   InsCod);
  }

/*****************************************************************************/
/*********** Get the short name of an institution from its code **************/
/*****************************************************************************/

void Ins_DB_GetInsShrtName (long InsCod,char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1])
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

unsigned Ins_DB_GetInsShrtNameAndCty (MYSQL_RES **mysql_res,long InsCod)
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

bool Ins_DB_CheckIfInsNameExistsInCty (const char *FldName,
                                       const char *Name,
				       long InsCod,
				       long CtyCod)
  {
   return
   DB_QueryEXISTS ("can not check if the name of an institution already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM ins_instits"
		    " WHERE CtyCod=%ld"
		      " AND %s='%s'"
		      " AND InsCod<>%ld)",
		   CtyCod,
		   FldName,Name,
		   InsCod);
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
		   (unsigned) Hie_STATUS_BIT_PENDING);
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
		   (unsigned) Hie_STATUS_BIT_PENDING,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************* Get institutions in country ordered by short name *************/
/*****************************************************************************/

unsigned Ins_DB_GetInssInCtyOrderedByShrtName (MYSQL_RES **mysql_res,long CtyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions",
		   "SELECT DISTINCT "
		          "InsCod,"	// row[0]
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
			  "WWW,"		// row[6]
                  	  "0 AS NumUsrs"	// row[7] (not used)
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
   static const char *OrderBySubQuery[Hie_NUM_ORDERS] =
     {
      [Hie_ORDER_BY_NAME    ] = "FullName",
      [Hie_ORDER_BY_NUM_USRS] = "NumUsrs DESC,"
	                        "FullName",
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
			  " (SELECT DISTINCT "
			           "InsCod"
			     " FROM usr_data))"
			    " ORDER BY %s",
		   CtyCod,
		   CtyCod,
		   OrderBySubQuery[Gbl.Hierarchy.List[HieLvl_CTY].SelectedOrder]);
  }

/*****************************************************************************/
/*************** Get institutions ordered by number of centers ***************/
/*****************************************************************************/

unsigned Ins_DB_GetInssOrderedByNumCtrs (MYSQL_RES **mysql_res)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT InsCod,"		// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ctr_centers"
		      " GROUP BY InsCod"
		      " ORDER BY N DESC");
      case HieLvl_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ins_instits,"
				"ctr_centers"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
		      " GROUP BY ctr_centers.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_CTY].HieCod);
      case HieLvl_INS:
      case HieLvl_CTR:
      case HieLvl_DEG:
      case HieLvl_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT InsCod,"		// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ctr_centers"
			 " WHERE InsCod=%ld"
		      " GROUP BY InsCod",
			 Gbl.Hierarchy.Node[HieLvl_INS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************** Get institutions ordered by number of degrees ****************/
/*****************************************************************************/

unsigned Ins_DB_GetInssOrderedByNumDegs (MYSQL_RES **mysql_res)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ctr_centers,"
				"deg_degrees"
			 " WHERE ctr_centers.CtrCod=deg_degrees.CtrCod"
		      " GROUP BY InsCod"
		      " ORDER BY N DESC");
      case HieLvl_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ins_instits,"
				"ctr_centers,"
				"deg_degrees"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		      " GROUP BY ctr_centers.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_CTY].HieCod);
      case HieLvl_INS:
      case HieLvl_CTR:
      case HieLvl_DEG:
      case HieLvl_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ctr_centers,"
				"deg_degrees"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		      " GROUP BY ctr_centers.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_INS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*************** Get institutions ordered by number of courses ***************/
/*****************************************************************************/

unsigned Ins_DB_GetInssOrderedByNumCrss (MYSQL_RES **mysql_res)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"crs_courses"
			 " WHERE ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
		      " GROUP BY InsCod"
		      " ORDER BY N DESC");
      case HieLvl_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ins_instits,"
				"ctr_centers,"
				"deg_degrees,"
				"crs_courses"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
		      " GROUP BY ctr_centers.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_CTY].HieCod);
      case HieLvl_INS:
      case HieLvl_CTR:
      case HieLvl_DEG:
      case HieLvl_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"crs_courses"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
		      " GROUP BY ctr_centers.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_INS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*************** Get institutions ordered by users in courses ****************/
/*****************************************************************************/

unsigned Ins_DB_GetInssOrderedByNumUsrsInCrss (MYSQL_RES **mysql_res)
  {
   /***** Get institutions ordered by number of users in courses *****/
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"			// row[0]
				"COUNT(DISTINCT crs_users.UsrCod) AS N"	// row[1]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"crs_users"
			 " WHERE ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
		      " GROUP BY InsCod"
		      " ORDER BY N DESC");
      case HieLvl_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"			// row[0]
				"COUNT(DISTINCT crs_users.UsrCod) AS N"	// row[1]
			  " FROM ins_instits,"
				"ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"crs_users"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
		      " GROUP BY ctr_centers.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_CTY].HieCod);
      case HieLvl_INS:
      case HieLvl_CTR:
      case HieLvl_DEG:
      case HieLvl_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT ctr_centers.InsCod,"			// row[0]
				"COUNT(DISTINCT crs_users.UsrCod) AS N"	// row[1]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"crs_users"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
		      " GROUP BY ctr_centers.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_INS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/** Get institutions ordered by number of users who claim to belong to them **/
/*****************************************************************************/

unsigned Ins_DB_GetInssOrderedByNumUsrsWhoClaimToBelongToThem (MYSQL_RES **mysql_res)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT InsCod,"		// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM usr_data"
			 " WHERE InsCod>0"
		      " GROUP BY InsCod"
		      " ORDER BY N DESC");
      case HieLvl_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT usr_data.InsCod,"	// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM ins_instits,usr_data"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=usr_data.InsCod"
		      " GROUP BY usr_data.InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_CTY].HieCod);
      case HieLvl_INS:
      case HieLvl_CTR:
      case HieLvl_DEG:
      case HieLvl_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get institutions",
			 "SELECT InsCod,"		// row[0]
				"COUNT(*) AS N"		// row[1]
			  " FROM usr_data"
			 " WHERE InsCod=%ld"
		      " GROUP BY InsCod"
		      " ORDER BY N DESC",
			 Gbl.Hierarchy.Node[HieLvl_INS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********************** Search institutions in database **********************/
/*****************************************************************************/
// Returns number of institutions found

unsigned Ins_DB_SearchInss (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions",
		   "SELECT ins_instits.InsCod"
		    " FROM ins_instits,"
			  "cty_countrs"
		   " WHERE %s"
		     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		     "%s"
		   " ORDER BY ins_instits.FullName,"
			     "cty_countrs.Name_%s",
		   SearchQuery,
		   RangeQuery,
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
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

unsigned Ins_DB_GetNumInssWithCtrs (HieLvl_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with centers",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers"
		  " WHERE %s"
		         "ins_instits.InsCod=ctr_centers.InsCod",
		  SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with degrees ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithDegs (HieLvl_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with degrees",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees"
		  " WHERE %s"
		         "ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  SubQuery);
  }

/*****************************************************************************/
/****************** Get number of institutions with courses ******************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInssWithCrss (HieLvl_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of institutions with courses",
		  "SELECT COUNT(DISTINCT ins_instits.InsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses"
		  " WHERE %s"
		         "ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  SubQuery);
  }

/*****************************************************************************/
/************* Get current number of institutions with users *****************/
/*****************************************************************************/

unsigned Ins_DB_GetNumInnsWithUsrs (Rol_Role_t Role,
                                    HieLvl_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

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
/************** Get the institutions of a user from database *****************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned Ins_DB_GetInssFromUsr (MYSQL_RES **mysql_res,long UsrCod,long CtyCod)
  {
   if (CtyCod > 0)
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not get the institutions a user belongs to",
		      "SELECT ins_instits.InsCod,"	// row[0]
			     "MAX(crs_users.Role)"	// row[1]
		       " FROM crs_users,"
			     "crs_courses,"
			     "deg_degrees,"
			     "ctr_centers,"
			     "ins_instits"
		      " WHERE crs_users.UsrCod=%ld"
		        " AND crs_users.CrsCod=crs_courses.CrsCod"
		        " AND crs_courses.DegCod=deg_degrees.DegCod"
		        " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		        " AND ctr_centers.InsCod=ins_instits.InsCod"
		        " AND ins_instits.CtyCod=%ld"
		   " GROUP BY ins_instits.InsCod"
		   " ORDER BY ins_instits.ShortName",
		      UsrCod,
		      CtyCod);
   else
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not get the institutions a user belongs to",
		      "SELECT ins_instits.InsCod,"	// row[0]
			     "MAX(crs_users.Role)"	// row[1]
		       " FROM crs_users,"
			     "crs_courses,"
			     "deg_degrees,"
			     "ctr_centers,"
			     "ins_instits"
		      " WHERE crs_users.UsrCod=%ld"
		        " AND crs_users.CrsCod=crs_courses.CrsCod"
		        " AND crs_courses.DegCod=deg_degrees.DegCod"
		        " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		        " AND ctr_centers.InsCod=ins_instits.InsCod"
		   " GROUP BY ins_instits.InsCod"
		   " ORDER BY ins_instits.ShortName",
		      UsrCod);
  }

/*****************************************************************************/
/**************** Check if a user belongs to an institution ******************/
/*****************************************************************************/

bool Ins_DB_CheckIfUsrBelongsToIns (long UsrCod,long HieCod,
				    bool CountOnlyAcceptedCourses)
  {
   const char *SubQuery = (CountOnlyAcceptedCourses ? " AND crs_users.Accepted='Y'" :	// Only if user accepted
                                                      "");

   return (DB_QueryCOUNT ("can not check if a user belongs to an institution",
			  "SELECT COUNT(DISTINCT ctr_centers.InsCod)"
			   " FROM crs_users,"
				 "crs_courses,"
				 "deg_degrees,"
				 "ctr_centers"
			  " WHERE crs_users.UsrCod=%ld"
			      "%s"
			    " AND crs_users.CrsCod=crs_courses.CrsCod"
			    " AND crs_courses.DegCod=deg_degrees.DegCod"
			    " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			    " AND ctr_centers.InsCod=%ld",
			  UsrCod,
			  SubQuery,
			  HieCod) != 0);
  }

/*****************************************************************************/
/******** Get number of users who claim to belong to an institution **********/
/*****************************************************************************/

unsigned Ins_DB_GetNumUsrsWhoClaimToBelongToIns (long InsCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users",
		  "SELECT COUNT(UsrCod)"
		   " FROM usr_data"
		  " WHERE InsCod=%ld",
		  InsCod);
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
