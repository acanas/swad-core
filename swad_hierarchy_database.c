// swad_hierarchy_database.c: hierarchy (system, institution, center, degree, course), operations with database

/*
    SWAD (Shared Workspace At a Distance),
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

#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_database.h"
#include "swad_hierarchy_type.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Hie_DB_SubQueryOnlyAcceptedCourses[Hie_DB_NUM_COUNT_ONLY_ACCEPTED_CRSS] =
  {
   [Hie_DB_ANY_COURSE           ] = "",
   [Hie_DB_ONLY_ACCEPTED_COURSES] = " AND crs_users.Accepted='Y'",
  };

/*****************************************************************************/
/******************* Get number of countries with users **********************/
/*****************************************************************************/

void Hie_DB_BuildSubquery (char SubQuery[128],Hie_Level_t HieLvl,long HieCod)
  {
   static const char *Format[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = "",					// Unknown
      [Hie_SYS] = "",					// System
      [Hie_CTY] = "ins_instits.CtyCod=%ld AND ",	// Country
      [Hie_INS] = "ctr_centers.InsCod=%ld AND ",	// Institution
      [Hie_CTR] = "deg_degrees.CtrCod=%ld AND ",	// Center
      [Hie_DEG] = "crs_courses.DegCod=%ld AND ",	// Degree
      [Hie_CRS] = "crs_users.CrsCod=%ld AND ",		// Course
     };

   if (HieCod > 0)
      sprintf (SubQuery,Format[HieLvl],HieCod);
   else
      SubQuery[0] = '\0';
  }

/*****************************************************************************/
/****** Get institutions, centers and degrees administrated by an admin *****/
/*****************************************************************************/

unsigned Hie_DB_GetInsCtrDegAdminBy (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get institutions, centers, degrees"
			     " admin by a user",
		   "(SELECT %u AS S,"			// row[0]
		           "-1 AS Cod,"			// row[1]
		           "'' AS FullName"		// row[2]
		     " FROM usr_admins"
		    " WHERE UsrCod=%ld"
		      " AND Scope='%s')"
		    " UNION "
		   "(SELECT %u AS S,"			// row[0]
			   "usr_admins.Cod,"		// row[1]
			   "ins_instits.FullName"	// row[2]
		     " FROM usr_admins,"
			   "ins_instits"
		    " WHERE usr_admins.UsrCod=%ld"
		      " AND usr_admins.Scope='%s'"
		      " AND usr_admins.Cod=ins_instits.InsCod)"
		    " UNION "
		   "(SELECT %u AS S,"			// row[0]
			   "usr_admins.Cod,"		// row[1]
			   "ctr_centers.FullName"	// row[2]
		     " FROM usr_admins,"
			   "ctr_centers"
		    " WHERE usr_admins.UsrCod=%ld"
		      " AND usr_admins.Scope='%s'"
		      " AND usr_admins.Cod=ctr_centers.CtrCod)"
		    " UNION "
		   "(SELECT %u AS S,"			// row[0]
			   "usr_admins.Cod,"		// row[1]
			   "deg_degrees.FullName"	// row[2]
		     " FROM usr_admins,"
		           "deg_degrees"
		    " WHERE usr_admins.UsrCod=%ld"
		      " AND usr_admins.Scope='%s'"
		      " AND usr_admins.Cod=deg_degrees.DegCod)"
		 " ORDER BY S,"
		           "FullName",
		   (unsigned) Hie_SYS,UsrCod,Hie_GetDBStrFromLevel (Hie_SYS),
		   (unsigned) Hie_INS,UsrCod,Hie_GetDBStrFromLevel (Hie_INS),
		   (unsigned) Hie_CTR,UsrCod,Hie_GetDBStrFromLevel (Hie_CTR),
		   (unsigned) Hie_DEG,UsrCod,Hie_GetDBStrFromLevel (Hie_DEG));
  }
