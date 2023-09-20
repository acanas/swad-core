// swad_connected_database.c: connected users operations with database

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_connected.h"
#include "swad_connected_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************** Get number of connected users with a role ******************/
/*****************************************************************************/

unsigned Con_DB_GetConnectedUsrsTotal (Rol_Role_t Role)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of connected users",
		  "SELECT COUNT(*)"
		   " FROM usr_connected"
		  " WHERE RoleInLastCrs=%u",
		  (unsigned) Role);
  }

/*****************************************************************************/
/************ Get connected users belonging to current location **************/
/*****************************************************************************/

unsigned Con_DB_GetNumConnectedFromCurrentLocation (MYSQL_RES **mysql_res,Rol_Role_t Role)
  {
   /***** Get number of connected users who belong to current course from database *****/
   switch (Role)
     {
      case Rol_UNK:	// Here Rol_UNK means "any role"
	 switch (Gbl.Scope.Current)
	   {
	    case HieLvl_SYS:		// Get connected users in the whole platform
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
			                  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM usr_connected,"
			              "usr_data"
			       " WHERE usr_connected.UsrCod=usr_data.UsrCod");
	    case HieLvl_CTY:		// Get connected users in the current country
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE ins_instits.CtyCod=%ld"
			         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_CTY].HieCod);
	    case HieLvl_INS:		// Get connected users in the current institution
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_INS].HieCod);
	    case HieLvl_CTR:		// Get connected users in the current center
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_CTR].HieCod);
	    case HieLvl_DEG:		// Get connected users in the current degree
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_DEG].HieCod);
	    case HieLvl_CRS:		// Get connected users in the current course
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE crs_users.CrsCod=%ld"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_CRS].HieCod);
	    default:
	       Err_WrongHierarchyLevelExit ();
	   }
	 return 0;	// Not reached
      case Rol_GST:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number"
				    " of connected users"
				    " who belong to this location",
			 "SELECT COUNT(DISTINCT usr_connected.UsrCod),"		// row[0]
			        "COUNT(DISTINCT usr_data.Sex),"			// row[1]
			        "MIN(usr_data.Sex)"				// row[2]
			  " FROM usr_connected,"
			        "usr_data"
			 " WHERE usr_connected.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM crs_users)"
			   " AND usr_connected.UsrCod=usr_data.UsrCod");
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 switch (Gbl.Scope.Current)
	   {
	    case HieLvl_SYS:		// Get connected users in the whole platform
	       	return (unsigned)
	        DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			        "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			               "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			               "MIN(usr_data.Sex)"			// row[2]
			         " FROM usr_connected,"
			               "crs_users,"
			               "usr_data"
			        " WHERE usr_connected.UsrCod=crs_users.UsrCod"
			          " AND crs_users.Role=%u"
			          " AND usr_connected.UsrCod=usr_data.UsrCod",
			        (unsigned) Role);
	    case HieLvl_CTY:		// Get connected users in the current country
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE ins_instits.CtyCod=%ld"
			         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_CTY].HieCod,
			       (unsigned) Role);
	    case HieLvl_INS:		// Get connected users in the current institution
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_INS].HieCod,
			       (unsigned) Role);
	    case HieLvl_CTR:		// Get connected users in the current center
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_CTR].HieCod,
			       (unsigned) Role);
	    case HieLvl_DEG:		// Get connected users in the current degree
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM crs_courses,"
			              "crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_DEG].HieCod,
			       (unsigned) Role);
	    case HieLvl_CRS:		// Get connected users in the current course
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get number"
					  " of connected users"
					  " who belong to this location",
			       "SELECT COUNT(DISTINCT usr_connected.UsrCod),"	// row[0]
			              "COUNT(DISTINCT usr_data.Sex),"		// row[1]
			              "MIN(usr_data.Sex)"			// row[2]
			        " FROM crs_users,"
			              "usr_connected,"
			              "usr_data"
			       " WHERE crs_users.CrsCod=%ld"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			         " AND usr_connected.UsrCod=usr_data.UsrCod",
			       Gbl.Hierarchy.Node[HieLvl_CRS].HieCod,
			       (unsigned) Role);
	    default:
	       Err_WrongHierarchyLevelExit ();
	   }
	 return 0;	// Not reached
      default:
	 Err_WrongRoleExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/***** Get connected users who belong to current location from database ******/
/*****************************************************************************/

unsigned Con_DB_GetConnectedFromCurrentLocation (MYSQL_RES **mysql_res,Rol_Role_t Role)
  {
   switch (Role)
     {
      case Rol_GST:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get list of connected users"
				   " who belong to this location",
			 "SELECT UsrCod,"						// row[0]
			        "LastCrsCod,"						// row[1]
			        "UNIX_TIMESTAMP()-"
			        "UNIX_TIMESTAMP(LastTime) AS Dif"			// row[2]
			  " FROM usr_connected"
			 " WHERE UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM crs_users)"
			 " ORDER BY Dif");
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 switch (Gbl.Scope.Current)
	   {
	    case HieLvl_SYS:		// Show connected users in the whole platform
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get list of connected users"
					 " who belong to this location",
			       "SELECT DISTINCT "
			              "usr_connected.UsrCod,"				// row[0]
			              "usr_connected.LastCrsCod,"			// row[1]
			              "UNIX_TIMESTAMP()-"
			              "UNIX_TIMESTAMP(usr_connected.LastTime) AS Dif"	// row[2]
			        " FROM usr_connected,"
			              "crs_users"
			       " WHERE usr_connected.UsrCod=crs_users.UsrCod"
			         " AND crs_users.Role=%u"
			       " ORDER BY Dif",
			       (unsigned) Role);
	    case HieLvl_CTY:		// Show connected users in the current country
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get list of connected users"
					 " who belong to this location",
			       "SELECT DISTINCT "
			              "usr_connected.UsrCod,"				// row[0]
			              "usr_connected.LastCrsCod,"			// row[1]
			              "UNIX_TIMESTAMP()-"
			              "UNIX_TIMESTAMP(usr_connected.LastTime) AS Dif"	// row[2]
			        " FROM ins_instits,"
			              "ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected"
			       " WHERE ins_instits.CtyCod=%ld"
			         " AND ins_instits.InsCod=ctr_centers.InsCod"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			       " ORDER BY Dif",
			       Gbl.Hierarchy.Node[HieLvl_CTY].HieCod,
			       (unsigned) Role);
	    case HieLvl_INS:		// Show connected users in the current institution
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get list of connected users"
					 " who belong to this location",
			       "SELECT DISTINCT "
			              "usr_connected.UsrCod,"				// row[0]
			              "usr_connected.LastCrsCod,"			// row[1]
				      "UNIX_TIMESTAMP()-"
				      "UNIX_TIMESTAMP(usr_connected.LastTime) AS Dif"	// row[2]
			        " FROM ctr_centers,"
			              "deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected"
			       " WHERE ctr_centers.InsCod=%ld"
			         " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			       " ORDER BY Dif",
			       Gbl.Hierarchy.Node[HieLvl_INS].HieCod,
			       (unsigned) Role);
	    case HieLvl_CTR:		// Show connected users in the current center
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get list of connected users"
					 " who belong to this location",
			       "SELECT DISTINCT "
			              "usr_connected.UsrCod,"				// row[0]
			              "usr_connected.LastCrsCod,"			// row[1]
			              "UNIX_TIMESTAMP()-"
			              "UNIX_TIMESTAMP(usr_connected.LastTime) AS Dif"	// row[2]
			        " FROM deg_degrees,"
			              "crs_courses,"
			              "crs_users,"
			              "usr_connected"
			       " WHERE deg_degrees.CtrCod=%ld"
			         " AND deg_degrees.DegCod=crs_courses.DegCod"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			       " ORDER BY Dif",
			       Gbl.Hierarchy.Node[HieLvl_CTR].HieCod,
			       (unsigned) Role);
	    case HieLvl_DEG:		// Show connected users in the current degree
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get list of connected users"
					 " who belong to this location",
			       "SELECT DISTINCT "
			              "usr_connected.UsrCod,"				// row[0]
			              "usr_connected.LastCrsCod,"			// row[1]
			              "UNIX_TIMESTAMP()-"
			              "UNIX_TIMESTAMP(usr_connected.LastTime) AS Dif"	// row[2]
			        " FROM crs_courses,"
			              "crs_users,"
			              "usr_connected"
			       " WHERE crs_courses.DegCod=%ld"
			         " AND crs_courses.CrsCod=crs_users.CrsCod"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			       " ORDER BY Dif",
			       Gbl.Hierarchy.Node[HieLvl_DEG].HieCod,
			       (unsigned) Role);
	    case HieLvl_CRS:		// Show connected users in the current course
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get list of connected users"
					 " who belong to this location",
			       "SELECT usr_connected.UsrCod,"				// row[0]
			              "usr_connected.LastCrsCod,"			// row[1]
			              "UNIX_TIMESTAMP()-"
			              "UNIX_TIMESTAMP(usr_connected.LastTime) AS Dif"	// row[2]
			        " FROM crs_users,"
			              "usr_connected"
			       " WHERE crs_users.CrsCod=%ld"
			         " AND crs_users.Role=%u"
			         " AND crs_users.UsrCod=usr_connected.UsrCod"
			       " ORDER BY Dif",
			       Gbl.Hierarchy.Node[HieLvl_CRS].HieCod,
			       (unsigned) Role);
	    default:
	       Err_WrongHierarchyLevelExit ();
	   }
	 return 0;	// Not reached
      default:
	 Err_WrongRoleExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********************* Modify my entry in connected list *********************/
/*****************************************************************************/

void Con_DB_UpdateMeInConnectedList (void)
  {
   /***** Update my entry in connected list.
          The role which is stored is the role of the last click *****/
   DB_QueryREPLACE ("can not update list of connected users",
		    "REPLACE INTO usr_connected"
		    " (UsrCod,RoleInLastCrs,LastCrsCod,LastTime)"
		    " VALUES"
		    " (%ld,%u,%ld,NOW())",
                    Gbl.Usrs.Me.UsrDat.UsrCod,
                    (unsigned) Gbl.Usrs.Me.Role.Logged,
                    Gbl.Hierarchy.Node[HieLvl_CRS].HieCod);
  }

/*****************************************************************************/
/************************** Remove old connected uses ************************/
/*****************************************************************************/

void Con_DB_RemoveOldConnected (void)
  {
   DB_QueryDELETE ("can not remove old users from list of connected users",
		   "DELETE FROM usr_connected"
		   " WHERE UsrCod NOT IN"
		         " (SELECT DISTINCT "
		                  "UsrCod"
		            " FROM ses_sessions)");
  }

/*****************************************************************************/
/************* Remove a given user from list of connected users **************/
/*****************************************************************************/

void Con_DB_RemoveUsrFromConnected (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from table of connected users",
		   "DELETE FROM usr_connected"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
