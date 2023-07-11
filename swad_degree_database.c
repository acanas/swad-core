// swad_degree_database.c: degrees operations with database

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

#include "swad_database.h"
#include "swad_degree_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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

void Deg_DB_CreateDegree (struct Deg_Degree *Deg,Hie_Status_t Status)
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
				(unsigned) Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Deg->ShrtName,
				Deg->FullName,
				Deg->WWW);
  }

/*****************************************************************************/
/************************* Get number of degree types ************************/
/*****************************************************************************/

unsigned Deg_DB_GetNumDegreeTypes (HieLvl_Level_t Level)
  {
   /***** Get number of types of degree from database *****/
   switch (Level)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get types of degree",
	                "SELECT COUNT(*)"
		         " FROM deg_types");
      case HieLvl_CTY:
	 /* Get only degree types with degrees in the current country */
	 return (unsigned)
	 DB_QueryCOUNT ("can not get types of degree",
	                "SELECT COUNT(deg_types.*)"
		         " FROM ins_instits,"
		               "ctr_centers,"
		               "deg_degrees,"
		               "deg_types"
		        " WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegTypCod=deg_types.DegTypCod"
		     " GROUP BY deg_degrees.DegTypCod",
			 Gbl.Hierarchy.Cty.CtyCod);
      case HieLvl_INS:
	 /* Get only degree types with degrees in the current institution */
	 return (unsigned)
	 DB_QueryCOUNT ("can not get types of degree",
	                "SELECT COUNT(deg_types.*)"
		         " FROM ctr_centers,"
		               "deg_degrees,"
		               "deg_types"
		        " WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegTypCod=deg_types.DegTypCod"
		     " GROUP BY deg_degrees.DegTypCod",
			 Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
	 /* Get only degree types with degrees in the current center */
	 return (unsigned)
	 DB_QueryCOUNT ("can not get types of degree",
	                "SELECT COUNT(deg_types.*)"
		         " FROM deg_degrees,"
		               "deg_types"
		        " WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegTypCod=deg_types.DegTypCod"
		     " GROUP BY deg_degrees.DegTypCod",
			 Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
      case HieLvl_CRS:
	 /* Get only degree types with degrees in the current degree */
	 return (unsigned)
	 DB_QueryCOUNT ("can not get types of degree",
	                "SELECT COUNT(deg_types.*)"
		         " FROM deg_degrees,"
		               "deg_types"
			 " FROM deg_degrees,"
			       "deg_types"
			" WHERE deg_degrees.DegCod=%ld"
			  " AND deg_degrees.DegTypCod=deg_types.DegTypCod",
			 Gbl.Hierarchy.Deg.DegCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/****************** Create a list with all degree types **********************/
/*****************************************************************************/

unsigned Deg_DB_GetDegreeTypes (MYSQL_RES **mysql_res,
                                HieLvl_Level_t Level,DegTyp_Order_t Order)
  {
   static const char *OrderBySubQuery[DegTyp_NUM_ORDERS] =
     {
      [DegTyp_ORDER_BY_DEGREE_TYPE] = "DegTypName",
      [DegTyp_ORDER_BY_NUM_DEGREES] = "NumDegs DESC,"
	                              "DegTypName",
     };

   /***** Get types of degree from database *****/
   switch (Level)
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
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************** Get the name of a type of degree from database ***************/
/*****************************************************************************/

void Deg_DB_GetDegTypeNameByCod (struct DegTyp_DegreeType *DegTyp)
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

unsigned Deg_DB_GetDegreeDataByCod (MYSQL_RES **mysql_res,long DegCod)
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
/********** Get the institution code of a degree from its code ***************/
/*****************************************************************************/

long Deg_DB_GetInsCodOfDegreeByCod (long DegCod)
  {
   /***** Trivial check: degree code should be > 0 *****/
   if (DegCod <= 0)
      return -1L;

   /***** Get the institution code of a degree from database *****/
   return DB_QuerySELECTCode ("can not get the institution of a degree",
			     "SELECT ctr_centers.InsCod"
			      " FROM deg_degrees,"
				    "ctr_centers"
			     " WHERE deg_degrees.DegCod=%ld"
			       " AND deg_degrees.CtrCod=ctr_centers.CtrCod",
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

void Deg_DB_GetShortNameOfDegreeByCod (long DegCod,char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1])
  {
   /***** Trivial check: degree code should be > 0 *****/
   if (DegCod <= 0)
     {
      ShrtName[0] = '\0';
      return;
     }

   /***** Get the short name of a degree from database *****/
   DB_QuerySELECTString (ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME,
			 "can not get the short name of a degree",
			 "SELECT ShortName"
			  " FROM deg_degrees"
			 " WHERE DegCod=%ld",
			 DegCod);
  }

/*****************************************************************************/
/****************** Check if name of degree type exists **********************/
/*****************************************************************************/

bool Deg_DB_CheckIfDegreeTypeNameExists (const char *DegTypName,long DegTypCod)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a type of degree already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM deg_types"
		    " WHERE DegTypName='%s'"
		      " AND DegTypCod<>%ld)",
		   DegTypName,
		   DegTypCod);
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

unsigned Deg_DB_GetDegsOfCurrentCtrBasic (MYSQL_RES **mysql_res)
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
/********* Get degrees belonging to the current center from database *********/
/*****************************************************************************/

unsigned Deg_DB_GetDegsOfCurrentCtrFull (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get degrees of a center",
		   "SELECT DegCod,"		// row[0]
			  "CtrCod,"		// row[1]
			  "DegTypCod,"		// row[2]
			  "Status,"		// row[3]
			  "RequesterUsrCod,"	// row[4]
			  "ShortName,"		// row[5]
			  "FullName,"		// row[6]
			  "WWW"			// row[7]
		    " FROM deg_degrees"
		   " WHERE CtrCod=%ld"
		   " ORDER BY FullName",
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
			 Hie_GetDBStrFromLevel (HieLvl_DEG),
			 (unsigned) Hie_STATUS_BIT_PENDING);
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
			 (unsigned) Hie_STATUS_BIT_PENDING);
      default:	// Forbidden for other users
	 Err_WrongRoleExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*********************** Get degrees that have students **********************/
/*****************************************************************************/

unsigned Deg_DB_GetDegsWithStds (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get degrees with students",
		   "SELECT DISTINCT "
			  "deg_degrees.DegCod,"			// row[0]
			  "deg_degrees.CtrCod,"			// row[1]
			  "deg_degrees.DegTypCod,"		// row[2]
			  "deg_degrees.Status,"			// row[3]
			  "deg_degrees.RequesterUsrCod,"	// row[4]
			  "deg_degrees.ShortName,"		// row[5]
			  "deg_degrees.FullName,"		// row[6]
			  "deg_degrees.WWW"			// row[7]
		    " FROM deg_degrees,"
		          "crs_courses,"
		          "crs_users"
		   " WHERE deg_degrees.DegCod=crs_courses.DegCod"
		     " AND crs_courses.CrsCod=crs_users.CrsCod"
		     " AND crs_users.Role=%u"
		   " ORDER BY deg_degrees.ShortName",
		   (unsigned) Rol_STD);
  }

/*****************************************************************************/
/********************* Check if the name of degree exists ********************/
/*****************************************************************************/

bool Deg_DB_CheckIfDegNameExistsInCtr (const char *FldName,const char *Name,
                                       long DegCod,long CtrCod)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a degree already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM deg_degrees"
		    " WHERE CtrCod=%ld"
		      " AND %s='%s'"
		      " AND DegCod<>%ld)",
		   CtrCod,
		   FldName,Name,
		   DegCod);
  }

/*****************************************************************************/
/************************* Search degrees in database ************************/
/*****************************************************************************/
// Returns number of degrees found

unsigned Deg_DB_SearchDegs (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get degrees",
		   "SELECT deg_degrees.DegCod"
		    " FROM deg_degrees,"
			  "ctr_centers,"
			  "ins_instits,"
			  "cty_countrs"
		   " WHERE %s"
		     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		     " AND ctr_centers.InsCod=ins_instits.InsCod"
		     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		     "%s"
		   " ORDER BY deg_degrees.FullName,"
			     "ins_instits.FullName",
		   SearchQuery,
		   RangeQuery);
  }

/*****************************************************************************/
/***************** Get current number of degrees with courses ****************/
/*****************************************************************************/

unsigned Deg_DB_GetNumDegsWithCrss (HieLvl_Level_t Level,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of degrees with courses",
		  "SELECT COUNT(DISTINCT deg_degrees.DegCod)"
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
/**************** Get current number of degrees with users *******************/
/*****************************************************************************/

unsigned Deg_DB_GetNumDegsWithUsrs (Rol_Role_t Role,
                                    HieLvl_Level_t Level,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of degrees with users",
		  "SELECT COUNT(DISTINCT deg_degrees.DegCod)"
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
		  SubQuery,
		  (unsigned) Role);
  }

/*****************************************************************************/
/******************** Get number of degrees in a country *********************/
/*****************************************************************************/

unsigned Deg_DB_GetNumDegsInCty (long CtyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of degrees in a country",
		  "SELECT COUNT(*)"
		   " FROM ins_instits,"
		         "ctr_centers,"
		         "deg_degrees"
		  " WHERE ins_instits.CtyCod=%ld"
		    " AND ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  CtyCod);
  }

/*****************************************************************************/
/***************** Get number of degrees in an institution *******************/
/*****************************************************************************/

unsigned Deg_DB_GetNumDegsInIns (long InsCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of degrees in an institution",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers,"
		         "deg_degrees"
		  " WHERE ctr_centers.InsCod=%ld"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  InsCod);
  }

/*****************************************************************************/
/******************** Get number of degrees in a center **********************/
/*****************************************************************************/

unsigned Deg_DB_GetNumDegsInCtr (long CtrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of degrees in a center",
		  "SELECT COUNT(*)"
		   " FROM deg_degrees"
		  " WHERE CtrCod=%ld",
		  CtrCod);
  }

/*****************************************************************************/
/************************* Update degree type name ***************************/
/*****************************************************************************/

void Deg_DB_UpdateDegTypName (long DegTypCod,
                              const char NewNameDegTyp[DegTyp_MAX_BYTES_DEGREE_TYPE_NAME + 1])
  {
   DB_QueryUPDATE ("can not update the type of a degree",
		   "UPDATE deg_types"
		     " SET DegTypName='%s'"
		   " WHERE DegTypCod=%ld",
		   NewNameDegTyp,
		   DegTypCod);
  }

/*****************************************************************************/
/***************** Update degree name in table of degrees ********************/
/*****************************************************************************/

void Deg_DB_UpdateDegNameDB (long DegCod,const char *FldName,
                             const char NewDegName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1])
  {
   DB_QueryUPDATE ("can not update the name of a degree",
		   "UPDATE deg_degrees"
		     " SET %s='%s'"
		   " WHERE DegCod=%ld",
	           FldName,
	           NewDegName,
	           DegCod);
  }

/*****************************************************************************/
/*********************** Update the center of a degree ***********************/
/*****************************************************************************/

void Deg_DB_UpdateDegCtr (long DegCod,long NewCtrCod)
  {
   DB_QueryUPDATE ("can not update the center of a degree",
		   "UPDATE deg_degrees"
		     " SET CtrCod=%ld"
		   " WHERE DegCod=%ld",
                   NewCtrCod,
                   DegCod);
  }

/*****************************************************************************/
/*********************** Update the type of a degree *************************/
/*****************************************************************************/

void Deg_DB_UpdateDegTyp (long DegCod,long NewDegTypCod)
  {
   DB_QueryUPDATE ("can not update the type of a degree",
		   "UPDATE deg_degrees"
		     " SET DegTypCod=%ld"
		   " WHERE DegCod=%ld",
	           NewDegTypCod,
	           DegCod);
  }

/*****************************************************************************/
/************************ Update the WWW of a degree *************************/
/*****************************************************************************/

void Deg_DB_UpdateDegWWW (long DegCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   DB_QueryUPDATE ("can not update the web of a degree",
		   "UPDATE deg_degrees"
		     " SET WWW='%s'"
		   " WHERE DegCod=%ld",
	           NewWWW,
	           DegCod);
  }

/*****************************************************************************/
/*********************** Update the status of a degree ***********************/
/*****************************************************************************/

void Deg_DB_UpdateDegStatus (long DegCod,Hie_Status_t NewStatus)
  {
   DB_QueryUPDATE ("can not update the status of a degree",
		   "UPDATE deg_degrees"
		     " SET Status=%u"
		   " WHERE DegCod=%ld",
                   (unsigned) NewStatus,
                   DegCod);
  }

/*****************************************************************************/
/***************** Get the degrees of a user from database *******************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned Deg_DB_GetDegsFromUsr (MYSQL_RES **mysql_res,long UsrCod,long CtrCod)
  {
   if (CtrCod > 0)
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not check the degrees a user belongs to",
		      "SELECT deg_degrees.DegCod,"	// row[0]
			     "MAX(crs_users.Role)"	// row[1]
		       " FROM crs_users,"
			     "crs_courses,"
			     "deg_degrees"
		      " WHERE crs_users.UsrCod=%ld"
		        " AND crs_users.CrsCod=crs_courses.CrsCod"
		        " AND crs_courses.DegCod=deg_degrees.DegCod"
		        " AND deg_degrees.CtrCod=%ld"
		   " GROUP BY deg_degrees.DegCod"
		   " ORDER BY deg_degrees.ShortName",
		      UsrCod,
		      CtrCod);
   else
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not check the degrees a user belongs to",
		      "SELECT deg_degrees.DegCod,"	// row[0]
			     "MAX(crs_users.Role)"	// row[1]
		       " FROM crs_users,"
			     "crs_courses,"
			     "deg_degrees"
		      " WHERE crs_users.UsrCod=%ld"
		        " AND crs_users.CrsCod=crs_courses.CrsCod"
		        " AND crs_courses.DegCod=deg_degrees.DegCod"
		   " GROUP BY deg_degrees.DegCod"
		   " ORDER BY deg_degrees.ShortName",
		      UsrCod);
  }

/*****************************************************************************/
/********* Get the degree in which a user is enroled in more courses *********/
/*****************************************************************************/

unsigned Deg_DB_GetUsrMainDeg (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's main degree",
		   "SELECT deg_degrees.ShortName,"	// row[0]
			  "main_degree.MaxRole"		// row[1]
		   " FROM deg_degrees,"

			 // The second table contain only one row with the main degree
			" (SELECT crs_courses.DegCod AS DegCod,"
				 "MAX(crs_users.Role) AS MaxRole,"
				 "COUNT(*) AS N"
			  " FROM crs_users,"
			        "crs_courses"
			 " WHERE crs_users.UsrCod=%ld"
			   " AND crs_users.CrsCod=crs_courses.CrsCod"
		      " GROUP BY crs_courses.DegCod"
		      " ORDER BY N DESC"	// Ordered by number of courses in which user is enroled
			 " LIMIT 1)"		// We need only the main degree
			" AS main_degree"

		 " WHERE deg_degrees.DegCod=main_degree.DegCod",
		 UsrCod);
  }

/*****************************************************************************/
/******************* Check if a user belongs to a degree *********************/
/*****************************************************************************/

bool Deg_DB_CheckIfUsrBelongsToDeg (long UsrCod,long DegCod)
  {
   return
   DB_QueryEXISTS ("can not check if a user belongs to a degree",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM crs_users,"
			   "crs_courses"
		    " WHERE crs_users.UsrCod=%ld"
		      " AND crs_users.Accepted='Y'"	// Only if user accepted
		      " AND crs_users.CrsCod=crs_courses.CrsCod"
		      " AND crs_courses.DegCod=%ld)",
		   UsrCod,
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

/*****************************************************************************/
/****************************** Remove a degree ******************************/
/*****************************************************************************/

void Deg_DB_RemoveDeg (long DegCod)
  {
   DB_QueryDELETE ("can not remove a degree",
		   "DELETE FROM deg_degrees"
		   " WHERE DegCod=%ld",
		   DegCod);
  }
