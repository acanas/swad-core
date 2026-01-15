// swad_role_database.c: user's roles, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_database.h"

/*****************************************************************************/
/************* Update the role of a user in the current course ***************/
/*****************************************************************************/

void Rol_DB_UpdateUsrRoleInCrs (long HieCod,long UsrCod,Rol_Role_t NewRole)
  {
   DB_QueryUPDATE ("can not modify user's role in course",
		   "UPDATE crs_users"
		     " SET Role=%u"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod=%ld",
	           (unsigned) NewRole,
	           HieCod,
	           UsrCod);
  }

/*****************************************************************************/
/********************** Get role of a user in a course ***********************/
/*****************************************************************************/

Rol_Role_t Rol_DB_GetRoleUsrInCrs (long UsrCod,long HieCod)
  {
   return
   DB_QuerySELECTRole ("can not get user's role",
		       "SELECT Role"
			" FROM crs_users"
		       " WHERE CrsCod=%ld"
			 " AND UsrCod=%ld",
		       HieCod,
		       UsrCod);
  }

/*****************************************************************************/
/**************** Get the maximum role of a user in any course ***************/
/*****************************************************************************/

Rol_Role_t Rol_DB_GetMaxRoleUsrInCrss (long UsrCod)
  {
   return
   DB_QuerySELECTRole ("can not get user's role",
		       "SELECT MAX(Role)"
			" FROM crs_users"
		       " WHERE UsrCod=%ld",
		       UsrCod);
  }

/*****************************************************************************/
/*************** Get distinct roles in all courses of a user *****************/
/*****************************************************************************/

unsigned Rol_DB_GetRolesInAllCrss (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's roles in all courses",
		   "SELECT DISTINCT Role"	// row[0]
		    " FROM crs_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************ Get requested role of a user in current course *****************/
/*****************************************************************************/

Rol_Role_t Rol_DB_GetRequestedRole (long HieCod,long UsrCod)
  {
   return DB_QuerySELECTRole ("can not get requested role",
			      "SELECT Role"
			       " FROM crs_requests"
			      " WHERE CrsCod=%ld"
			        " AND UsrCod=%ld",
			      HieCod,
			      UsrCod);
  }
