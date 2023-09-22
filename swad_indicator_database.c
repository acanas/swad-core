// swad_indicators_database.c: indicators of courses, operations with database

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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_indicator.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************ Store number of indicators of a course in database *************/
/*****************************************************************************/

void Ind_DB_StoreIndicatorsCrs (long CrsCod,unsigned NumIndicators)
  {
   /***** Store number of indicators of a course in database *****/
   DB_QueryUPDATE ("can not store number of indicators of a course",
		   "UPDATE crs_courses"
		     " SET NumIndicators=%u"
		   " WHERE CrsCod=%ld",
                   NumIndicators,
                   CrsCod);
  }

/*****************************************************************************/
/************ Get number of indicators of a course from database *************/
/*****************************************************************************/
// This function returns -1 if number of indicators is not yet calculated

unsigned Ind_DB_GetNumIndicatorsCrs (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get number of indicators",
		   "SELECT NumIndicators"	// row[0]
		    " FROM crs_courses"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/******************* Build query to get table of courses *********************/
/*****************************************************************************/
// Return the number of courses found

unsigned Ind_DB_GetTableOfCourses (MYSQL_RES **mysql_res,
                                   const struct Ind_Indicators *Indicators)
  {
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
         if (Indicators->DptCod >= 0)	// 0 means another department
           {
            if (Indicators->DegTypCod > 0)
               return (unsigned)
               DB_QuerySELECT (mysql_res,"can not get courses",
			       "SELECT DISTINCT "
				      "deg_degrees.FullName,"	// row[0]
				      "crs_courses.FullName,"	// row[1]
				      "crs_courses.CrsCod,"	// row[2]
				      "crs_courses.InsCrsCod"	// row[3]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_data"
			       " WHERE deg_degrees.DegTypCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_data.UsrCod"
			         " AND usr_data.DptCod=%ld"
			       " ORDER BY deg_degrees.FullName,"
					 "crs_courses.FullName",
			       Indicators->DegTypCod,
			       (unsigned) Rol_TCH,
			       Indicators->DptCod);
            else
               return (unsigned)
               DB_QuerySELECT (mysql_res,"can not get courses",
			       "SELECT DISTINCT "
				      "deg_degrees.FullName,"	// row[0]
				      "crs_courses.FullName,"	// row[1]
				      "crs_courses.CrsCod,"	// row[2]
				      "crs_courses.InsCrsCod"	// row[3]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_data"
			       " WHERE deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_data.UsrCod"
			         " AND usr_data.DptCod=%ld"
			       " ORDER BY deg_degrees.FullName,"
				 	 "crs_courses.FullName",
			       (unsigned) Rol_TCH,
			       Indicators->DptCod);
           }
         else
           {
            if (Indicators->DegTypCod > 0)
               return (unsigned)
               DB_QuerySELECT (mysql_res,"can not get courses",
			       "SELECT deg_degrees.FullName,"	// row[0]
				      "crs_courses.FullName,"	// row[1]
				      "crs_courses.CrsCod,"	// row[2]
				      "crs_courses.InsCrsCod"	// row[3]
			        " FROM deg_degrees,"
			              "crs_courses"
			       " WHERE deg_degrees.DegTypCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			       " ORDER BY deg_degrees.FullName,"
					 "crs_courses.FullName",
			       Indicators->DegTypCod);
            else
               return (unsigned)
               DB_QuerySELECT (mysql_res,"can not get courses",
			       "SELECT deg_degrees.FullName,"	// row[0]
				      "crs_courses.FullName,"	// row[1]
				      "crs_courses.CrsCod,"	// row[2]
				      "crs_courses.InsCrsCod"	// row[3]
			        " FROM deg_degrees,"
			              "crs_courses"
			       " WHERE deg_degrees.DegCod=crs_courses.DegCod"
			       " ORDER BY deg_degrees.FullName,"
					 "crs_courses.FullName");
           }
      case Hie_CTY:
         if (Indicators->DptCod >= 0)	// 0 means another department
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT DISTINCT "
				   "deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM ins_instits,"
			           "ctr_centers,"
			           "deg_degrees,"
			           "crs_courses,"
			           "crs_users,"
			           "usr_data"
			    " WHERE ins_instits.CtyCod=%ld"
			      " AND ins_instits.InsCod=ctr_centers.InsCod"
			      " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			      " AND crs_courses.CrsCod=crs_users.CrsCod"
			      " AND crs_users.Role=%u"
			      " AND crs_users.UsrCod=usr_data.UsrCod"
			      " AND usr_data.DptCod=%ld"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			    (unsigned) Rol_TCH,
			    Indicators->DptCod);
         else
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM ins_instits,"
			           "ctr_centers,"
			           "deg_degrees,"
			           "crs_courses"
			    " WHERE ins_instits.CtyCod=%ld"
			      " AND ins_instits.InsCod=ctr_centers.InsCod"
			      " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         if (Indicators->DptCod >= 0)	// 0 means another department
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT DISTINCT "
				   "deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM ctr_centers,"
			           "deg_degrees,"
			           "crs_courses,"
			           "crs_users,"
			           "usr_data"
			    " WHERE ctr_centers.InsCod=%ld"
			      " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			      " AND crs_courses.CrsCod=crs_users.CrsCod"
			      " AND crs_users.Role=%u"
			      " AND crs_users.UsrCod=usr_data.UsrCod"
			      " AND usr_data.DptCod=%ld"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_INS].HieCod,
			    (unsigned) Rol_TCH,
			    Indicators->DptCod);
         else
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM ctr_centers,"
			           "deg_degrees,"
			           "crs_courses"
			    " WHERE ctr_centers.InsCod=%ld"
			      " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         if (Indicators->DptCod >= 0)	// 0 means another department
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT DISTINCT "
				   "deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM deg_degrees,"
			           "crs_courses,"
			           "crs_users,"
			           "usr_data"
			    " WHERE deg_degrees.CtrCod=%ld"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			      " AND crs_courses.CrsCod=crs_users.CrsCod"
			      " AND crs_users.Role=%u"
			      " AND crs_users.UsrCod=usr_data.UsrCod"
			      " AND usr_data.DptCod=%ld"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			    (unsigned) Rol_TCH,
			    Indicators->DptCod);
         else
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM deg_degrees,"
			           "crs_courses"
			    " WHERE deg_degrees.CtrCod=%ld"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         if (Indicators->DptCod >= 0)	// 0 means another department
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT DISTINCT "
				   "deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM deg_degrees,"
			           "crs_courses,"
			           "crs_users,"
			           "usr_data"
			    " WHERE deg_degrees.DegCod=%ld"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			      " AND crs_courses.CrsCod=crs_users.CrsCod"
			      " AND crs_users.Role=%u"
			      " AND crs_users.UsrCod=usr_data.UsrCod"
			      " AND usr_data.DptCod=%ld"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			    (unsigned) Rol_TCH,
			    Indicators->DptCod);
         else
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM deg_degrees,"
			           "crs_courses"
			    " WHERE deg_degrees.DegCod=%ld"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         if (Indicators->DptCod >= 0)	// 0 means another department
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT DISTINCT "
				   "deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM deg_degrees,"
			           "crs_courses,"
			           "crs_users,"
			           "usr_data"
			    " WHERE crs_courses.CrsCod=%ld"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			      " AND crs_courses.CrsCod=crs_users.CrsCod"
			      " AND crs_users.CrsCod=%ld"
			      " AND crs_users.Role=%u"
			      " AND crs_users.UsrCod=usr_data.UsrCod"
			      " AND usr_data.DptCod=%ld"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			    (unsigned) Rol_TCH,
			    Indicators->DptCod);
         else
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get courses",
			    "SELECT deg_degrees.FullName,"	// row[0]
				   "crs_courses.FullName,"	// row[1]
				   "crs_courses.CrsCod,"	// row[2]
				   "crs_courses.InsCrsCod"	// row[3]
			     " FROM deg_degrees,"
			           "crs_courses"
			    " WHERE crs_courses.CrsCod=%ld"
			      " AND deg_degrees.DegCod=crs_courses.DegCod"
			 " ORDER BY deg_degrees.FullName,"
				   "crs_courses.FullName",
			    Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }
