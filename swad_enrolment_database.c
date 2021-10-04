// swad_enrolment_database.h: enrolment (registration) or removing of users, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include "swad_enrolment.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_global.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Private global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/*************** Register user in current course in database *****************/
/*****************************************************************************/

void Enr_DB_InsertUsrInCurrentCrs (long UsrCod,long CrsCod,Rol_Role_t NewRole,
                                   Enr_KeepOrSetAccepted_t KeepOrSetAccepted)
  {
   DB_QueryINSERT ("can not register user in course",
		   "INSERT INTO crs_users"
		   " (CrsCod,UsrCod,Role,Accepted)"
		   " VALUES"
		   " (%ld,%ld,%u,'%c')",
	           CrsCod,
	           UsrCod,
	           (unsigned) NewRole,
	           KeepOrSetAccepted == Enr_SET_ACCEPTED_TO_TRUE ? 'Y' :
							           'N');
  }

/*****************************************************************************/
/********* Set a user's acceptation to true in the current course ************/
/*****************************************************************************/

void Enr_DB_AcceptUsrInCrs (long UsrCod,long CrsCod)
  {
   /***** Set enrolment of a user to "accepted" in the current course *****/
   DB_QueryUPDATE ("can not confirm user's enrolment",
		   "UPDATE crs_users"
		     " SET Accepted='Y'"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod=%ld",
                   CrsCod,
                   UsrCod);
  }

/*****************************************************************************/
/*********** Get all user codes belonging to the current course **************/
/*****************************************************************************/

unsigned Enr_DB_GetUsrsFromCurrentCrs (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users from current course",
		   "SELECT UsrCod"
		    " FROM crs_users"
		   " WHERE CrsCod=%ld",
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******* Get all user codes belonging to the current course, except me *******/
/*****************************************************************************/

unsigned Enr_DB_GetUsrsFromCurrentCrsExceptMe (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users from current course",
		   "SELECT UsrCod"
		    " FROM crs_users"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod<>%ld",
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***** Get all teachers codes belonging to the current course, except me *****/
/*****************************************************************************/

unsigned Enr_DB_GetTchsFromCurrentCrsExceptMe (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get teachers from current course",
		   "SELECT UsrCod"
		    " FROM crs_users"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod<>%ld"
		     " AND Role=%u",	// Teachers only
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Rol_TCH);
  }

/*****************************************************************************/
/************************** Remove user from course **************************/
/*****************************************************************************/

void Enr_DB_RemUsrFromCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove a user from a course",
		   "DELETE FROM crs_users"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod=%ld",
		   CrsCod,
		   UsrCod);
  }

/*****************************************************************************/
/************************ Remove a user from a course ************************/
/*****************************************************************************/

void Enr_DB_RemUsrFromAllCrss (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from all courses",
		   "DELETE FROM crs_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*************** Remove all users from settings in a course ******************/
/*****************************************************************************/

void Enr_DB_RemAllUsrsFromCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove users from a course",
		   "DELETE FROM crs_users"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/********* Set a user's acceptation to true in the current course ************/
/*****************************************************************************/

unsigned Enr_DB_GetEnrolmentRequests (MYSQL_RES **mysql_res,unsigned RolesSelected)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:                // Show requesters for the whole platform
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:
	       // Requests in all courses in which I am teacher
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM crs_users,"
				      "crs_requests"
			       " WHERE crs_users.UsrCod=%ld"
				 " AND crs_users.Role=%u"
				 " AND crs_users.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       (unsigned) Rol_TCH,
			       RolesSelected);
	    case Rol_DEG_ADM:
	       // Requests in all degrees administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,Sco_GetDBStrFromScope (HieLvl_DEG),
			       RolesSelected);
	    case Rol_CTR_ADM:
	       // Requests in all centers administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,Sco_GetDBStrFromScope (HieLvl_CTR),
			       RolesSelected);
	    case Rol_INS_ADM:
	       // Requests in all institutions administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=ctr_centers.InsCod"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,Sco_GetDBStrFromScope (HieLvl_INS),
			       RolesSelected);
	   case Rol_SYS_ADM:
	       // All requests
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT ReqCod,"				// row[0]
				      "CrsCod,"				// row[1]
				      "UsrCod,"				// row[2]
				      "Role,"				// row[3]
				      "UNIX_TIMESTAMP(RequestTime)"	// row[4]
				" FROM crs_requests"
			       " WHERE ((1<<Role)&%u)<>0"
			       " ORDER BY RequestTime DESC",
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 break;
      case HieLvl_CTY:                // Show requesters for the current country
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:
	       // Requests in courses of this country in which I am teacher
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// roe[4]
				" FROM crs_users,"
				      "ins_instits,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE crs_users.UsrCod=%ld"
				 " AND crs_users.Role=%u"
				 " AND crs_users.CrsCod=crs_courses.CrsCod"
				 " AND crs_courses.DegCod=deg_degrees.DegCod"
				 " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
				 " AND ctr_centers.InsCod=ins_instits.InsCod"
				 " AND ins_instits.CtyCod=%ld"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       (unsigned) Rol_TCH,
			       Gbl.Hierarchy.Cty.CtyCod,
			       RolesSelected);
	    case Rol_DEG_ADM:
	       // Requests in degrees of this country administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "ins_instits,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=deg_degrees.DegCod"
				 " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
				 " AND ctr_centers.InsCod=ins_instits.InsCod"
				 " AND ins_instits.CtyCod=%ld"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       Sco_GetDBStrFromScope (HieLvl_DEG),
			       Gbl.Hierarchy.Cty.CtyCod,
			       RolesSelected);
	    case Rol_CTR_ADM:
	       // Requests in centers of this country administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "ins_instits,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=ctr_centers.CtrCod"
				 " AND ctr_centers.InsCod=ins_instits.InsCod"
				 " AND ins_instits.CtyCod=%ld"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       Sco_GetDBStrFromScope (HieLvl_CTR),
			       Gbl.Hierarchy.Cty.CtyCod,
			       RolesSelected);
	    case Rol_INS_ADM:
	       // Requests in institutions of this country administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "ins_instits,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=ins_instits.InsCod"
				 " AND ins_instits.CtyCod=%ld"
				 " AND ins_instits.InsCod=ctr_centers.InsCod"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       Sco_GetDBStrFromScope (HieLvl_INS),
			       Gbl.Hierarchy.Cty.CtyCod,
			       RolesSelected);
	    case Rol_SYS_ADM:
	       // Requests in any course of this country
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM ins_instits,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE ins_instits.CtyCod=%ld"
				 " AND ins_instits.InsCod=ctr_centers.InsCod"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Hierarchy.Cty.CtyCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case HieLvl_INS:                // Show requesters for the current institution
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:
	       // Requests in courses of this institution in which I am teacher
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM crs_users,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE crs_users.UsrCod=%ld"
				 " AND crs_users.Role=%u"
				 " AND crs_users.CrsCod=crs_courses.CrsCod"
				 " AND crs_courses.DegCod=deg_degrees.DegCod"
				 " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
				 " AND ctr_centers.InsCod=%ld"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       (unsigned) Rol_TCH,
			       Gbl.Hierarchy.Ins.InsCod,
			       RolesSelected);
	    case Rol_DEG_ADM:
	       // Requests in degrees of this institution administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=deg_degrees.DegCod"
				 " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
				 " AND ctr_centers.InsCod=%ld"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       Sco_GetDBStrFromScope (HieLvl_DEG),
			       Gbl.Hierarchy.Ins.InsCod,
			       RolesSelected);
	    case Rol_CTR_ADM:
	       // Requests in centers of this institution administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=ctr_centers.CtrCod"
				 " AND ctr_centers.InsCod=%ld"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,Sco_GetDBStrFromScope (HieLvl_CTR),
			       Gbl.Hierarchy.Ins.InsCod,
			       RolesSelected);
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this institution
	    case Rol_SYS_ADM:
	       // Requests in any course of this institution
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE ctr_centers.InsCod=%ld"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Hierarchy.Ins.InsCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case HieLvl_CTR:                // Show requesters for the current center
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:
	       // Requests in courses of this center in which I am teacher
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM crs_users,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE crs_users.UsrCod=%ld"
				 " AND crs_users.Role=%u"
				 " AND crs_users.CrsCod=crs_courses.CrsCod"
				 " AND crs_courses.DegCod=deg_degrees.DegCod"
				 " AND deg_degrees.CtrCod=%ld"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       (unsigned) Rol_TCH,
			       Gbl.Hierarchy.Ctr.CtrCod,
			       RolesSelected);
	    case Rol_DEG_ADM:
	       // Requests in degrees of this center administrated by me
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM usr_admins,"
				      "deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE usr_admins.UsrCod=%ld"
				 " AND usr_admins.Scope='%s'"
				 " AND usr_admins.Cod=deg_degrees.DegCod"
				 " AND deg_degrees.CtrCod=%ld"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       Sco_GetDBStrFromScope (HieLvl_DEG),
			       Gbl.Hierarchy.Ctr.CtrCod,
			       RolesSelected);
	    case Rol_CTR_ADM:	// If I am logged as admin of this center     , I can view all the requesters from this center
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this center
	    case Rol_SYS_ADM:
	       // Request in any course of this center
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM deg_degrees,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE deg_degrees.CtrCod=%ld"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Hierarchy.Ctr.CtrCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case HieLvl_DEG:        // Show requesters for the current degree
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:
	       // Requests in courses of this degree in which I am teacher
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM crs_users,"
				      "crs_courses,"
				      "crs_requests"
			       " WHERE crs_users.UsrCod=%ld"
				 " AND crs_users.Role=%u"
				 " AND crs_users.CrsCod=crs_courses.CrsCod"
				 " AND crs_courses.DegCod=%ld"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       (unsigned) Rol_TCH,
			       Gbl.Hierarchy.Deg.DegCod,
			       RolesSelected);
	    case Rol_DEG_ADM:	// If I am logged as admin of this degree     , I can view all the requesters from this degree
	    case Rol_CTR_ADM:	// If I am logged as admin of this center     , I can view all the requesters from this degree
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this degree
	    case Rol_SYS_ADM:
	       // Requests in any course of this degree
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT crs_requests.ReqCod,"				// row[0]
				      "crs_requests.CrsCod,"				// row[1]
				      "crs_requests.UsrCod,"				// row[2]
				      "crs_requests.Role,"				// row[3]
				      "UNIX_TIMESTAMP(crs_requests.RequestTime)"	// row[4]
				" FROM crs_courses,"
				      "crs_requests"
			       " WHERE crs_courses.DegCod=%ld"
				 " AND crs_courses.CrsCod=crs_requests.CrsCod"
				 " AND ((1<<crs_requests.Role)&%u)<>0"
			       " ORDER BY crs_requests.RequestTime DESC",
			       Gbl.Hierarchy.Deg.DegCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case HieLvl_CRS:        // Show requesters for the current course
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:	// If I am logged as teacher of this course   , I can view all the requesters from this course
	    case Rol_DEG_ADM:	// If I am logged as admin of this degree     , I can view all the requesters from this course
	    case Rol_CTR_ADM:	// If I am logged as admin of this center     , I can view all the requesters from this course
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all the requesters from this course
	    case Rol_SYS_ADM:
	       // Requests in this course
	       return (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get requests for enrolment",
			       "SELECT ReqCod,"				// row[0]
				      "CrsCod,"				// row[1]
				      "UsrCod,"				// row[2]
				      "Role,"				// row[3]
				      "UNIX_TIMESTAMP(RequestTime)"	// row[4]
				" FROM crs_requests"
			       " WHERE CrsCod=%ld"
				 " AND ((1<<Role)&%u)<>0"
			       " ORDER BY RequestTime DESC",
			       Gbl.Hierarchy.Crs.CrsCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      default:
	 Err_WrongScopeExit ();
	 return 0;		// Not reached
     }
  }

/*****************************************************************************/
/****** Get enrolment request (user and requested role) given its code *******/
/*****************************************************************************/

unsigned Enr_DB_GetEnrolmentRequestByCod (MYSQL_RES **mysql_res,long ReqCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get enrolment request",
		   "SELECT UsrCod,"		// row[0]
			  "Role"		// row[1]
		    " FROM crs_requests"
		   " WHERE ReqCod=%ld",
		   ReqCod);
  }

/*****************************************************************************/
/********** Try to get an enrolment request from a user in a course **********/
/*****************************************************************************/

long Enr_DB_GetUsrEnrolmentRequestInCrs (long UsrCod,long CrsCod)
  {
   return
   DB_QuerySELECTCode ("can not get enrolment request",
		       "SELECT ReqCod"
			" FROM crs_requests"
		       " WHERE CrsCod=%ld"
			 " AND UsrCod=%ld",
		       CrsCod,
		       UsrCod);
  }

/*****************************************************************************/
/************ Create my enrolment request in the current course **************/
/*****************************************************************************/

long Enr_DB_CreateMyEnrolmentRequestInCurrentCrs (Rol_Role_t NewRole)
  {
   return
   DB_QueryINSERTandReturnCode ("can not save enrolment request",
				"INSERT INTO crs_requests"
				" (CrsCod,UsrCod,Role,RequestTime)"
				" VALUES"
				" (%ld,%ld,%u,NOW())",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				(unsigned) NewRole);
  }

/*****************************************************************************/
/************* Update my enrolment request in the current course *************/
/*****************************************************************************/

void Enr_DB_UpdateMyEnrolmentRequestInCurrentCrs (long ReqCod,Rol_Role_t NewRole)
  {
   DB_QueryUPDATE ("can not update enrolment request",
		   "UPDATE crs_requests"
		     " SET Role=%u,"
			  "RequestTime=NOW()"
		   " WHERE ReqCod=%ld"
		     " AND CrsCod=%ld"
		     " AND UsrCod=%ld",
		   (unsigned) NewRole,
		   ReqCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************** Remove enrolment request *************************/
/*****************************************************************************/

void Enr_DB_RemRequest (long ReqCod)
  {
   DB_QueryDELETE ("can not remove a request for enrolment",
		   "DELETE FROM crs_requests"
		   " WHERE ReqCod=%ld",
                   ReqCod);
  }

/*****************************************************************************/
/*********** Remove all users' requests for inscription in a course **********/
/*****************************************************************************/

void Enr_DB_RemCrsRequests (long CrsCod)
  {
   DB_QueryDELETE ("can not remove requests for inscription to a course",
		   "DELETE FROM crs_requests"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/************ Remove user's requests for inscription from a course ***********/
/*****************************************************************************/

void Enr_DB_RemUsrRequests (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's requests for inscription",
		   "DELETE FROM crs_requests"
		   " WHERE UsrCod=%ld",
	           UsrCod);
  }

/*****************************************************************************/
/******************* Remove expired requests for enrolment ******************/
/*****************************************************************************/

void Enr_DB_RemoveExpiredEnrolmentRequests (void)
  {
   /***** Mark possible notifications as removed
          Important: do this before removing the request *****/
   DB_QueryUPDATE ("can not set notification(s) as removed",
		   "UPDATE ntf_notifications,"
		          "crs_requests"
		     " SET ntf_notifications.Status=(ntf_notifications.Status | %u)"
		   " WHERE ntf_notifications.NotifyEvent=%u"
		     " AND ntf_notifications.Cod=crs_requests.ReqCod"
		     " AND crs_requests.RequestTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
	           (unsigned) Ntf_STATUS_BIT_REMOVED,
	           (unsigned) Ntf_EVENT_ENROLMENT_REQUEST,
	           Cfg_TIME_TO_DELETE_ENROLMENT_REQUESTS);

   /***** Remove expired requests for enrolment *****/
   DB_QueryDELETE ("can not remove expired requests for enrolment",
		   "DELETE LOW_PRIORITY FROM crs_requests"
		   " WHERE RequestTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_ENROLMENT_REQUESTS);
  }
