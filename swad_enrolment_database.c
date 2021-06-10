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

// #define _GNU_SOURCE 		// For asprintf
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
// #include <string.h>		// For string functions

// #include "swad_account.h"
// #include "swad_announcement.h"
// #include "swad_attendance_database.h"
// #include "swad_box.h"
#include "swad_database.h"
// #include "swad_duplicate.h"
#include "swad_enrolment.h"
#include "swad_enrolment_database.h"
// #include "swad_error.h"
// #include "swad_exam_print.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_hierarchy.h"
// #include "swad_HTML.h"
// #include "swad_ID.h"
// #include "swad_match.h"
// #include "swad_message.h"
// #include "swad_notification.h"
// #include "swad_parameter.h"
// #include "swad_photo.h"
// #include "swad_role.h"
// #include "swad_test_print.h"
// #include "swad_user.h"

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

void Enr_DB_InsertUsrInCurrentCrs (long UsrCod,Rol_Role_t NewRole,
                                   Enr_KeepOrSetAccepted_t KeepOrSetAccepted)
  {
   DB_QueryINSERT ("can not register user in course",
		   "INSERT INTO crs_users"
		   " (CrsCod,UsrCod,Role,Accepted)"
		   " VALUES"
		   " (%ld,%ld,%u,'%c')",
	           Gbl.Hierarchy.Crs.CrsCod,
	           UsrCod,
	           (unsigned) NewRole,
	           KeepOrSetAccepted == Enr_SET_ACCEPTED_TO_TRUE ? 'Y' :
							           'N');
  }

/*****************************************************************************/
/**************** Update institution, center and department ******************/
/*****************************************************************************/

void Enr_DB_UpdateInstitutionCenterDepartment (void)
  {
   DB_QueryUPDATE ("can not update institution, center and department",
		   "UPDATE usr_data"
		     " SET InsCtyCod=%ld,"
		          "InsCod=%ld,"
		          "CtrCod=%ld,"
		          "DptCod=%ld"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.UsrDat.InsCtyCod,
	           Gbl.Usrs.Me.UsrDat.InsCod,
	           Gbl.Usrs.Me.UsrDat.Tch.CtrCod,
	           Gbl.Usrs.Me.UsrDat.Tch.DptCod,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********* Set a user's acceptation to true in the current course ************/
/*****************************************************************************/

void Enr_DB_AcceptUsrInCrs (long UsrCod)
  {
   /***** Set enrolment of a user to "accepted" in the current course *****/
   DB_QueryUPDATE ("can not confirm user's enrolment",
		   "UPDATE crs_users"
		     " SET Accepted='Y'"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod=%ld",
                   Gbl.Hierarchy.Crs.CrsCod,
                   UsrCod);
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
/*** Try to get and old request of me in the current course from database ****/
/*****************************************************************************/

long Enr_DB_GetMyLastEnrolmentRequestInCurrentCrs (void)
  {
   return
   DB_QuerySELECTCode ("can not get enrolment request",
		       "SELECT ReqCod"
			" FROM crs_requests"
		       " WHERE CrsCod=%ld"
			 " AND UsrCod=%ld",
		       Gbl.Hierarchy.Crs.CrsCod,
		       Gbl.Usrs.Me.UsrDat.UsrCod);
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
/*************** Remove all users from settings in a course ******************/
/*****************************************************************************/

void Enr_DB_RemAllUsrsFromCrsSettings (long CrsCod)
  {
   DB_QueryDELETE ("can not remove users from a course settings",
		   "DELETE FROM crs_user_settings"
		   " WHERE CrsCod=%ld",
		   CrsCod);
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
/******* Remove user as administrator of any degree/center/institution *******/
/*****************************************************************************/

void Enr_DB_RemUsrAsAdmin (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user as administrator",
		   "DELETE FROM usr_admins"
		   " WHERE UsrCod=%ld",
                   UsrCod);
  }

/*****************************************************************************/
/********* Remove administrators of an institution, center or degree *********/
/*****************************************************************************/

void Enr_DB_RemAdmins (HieLvl_Level_t Scope,long Cod)
  {
   DB_QueryDELETE ("can not remove administrators",
		   "DELETE FROM usr_admins"
		   " WHERE Scope='%s'"
		     " AND Cod=%ld",
                   Sco_GetDBStrFromScope (Scope),
                   Cod);
  }
