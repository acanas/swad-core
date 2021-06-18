// swad_group_database.c: types of groups and groups operations with database

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
/*********************************** Headers *********************************/
/*****************************************************************************/

// #define _GNU_SOURCE 		// For asprintf
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For exit, system, malloc, free, rand, etc.
// #include <string.h>		// For string functions

// #include "swad_action.h"
// #include "swad_assignment_database.h"
// #include "swad_attendance_database.h"
// #include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
// #include "swad_exam_session.h"
// #include "swad_form.h"
// #include "swad_game.h"
#include "swad_global.h"
#include "swad_group.h"
// #include "swad_HTML.h"
// #include "swad_match.h"
// #include "swad_notification.h"
// #include "swad_parameter.h"
// #include "swad_program.h"
// #include "swad_project.h"
// #include "swad_setting.h"
// #include "swad_survey.h"

/*****************************************************************************/
/*************************** Private constants *******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************* External global variables from others modules *****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Private global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/*********** Lock tables to make the registration in groups atomic ***********/
/*****************************************************************************/

void Grp_DB_LockTables (void)
  {
   DB_Query ("can not lock tables to change user's groups",
	     "LOCK TABLES "
	          "grp_types WRITE,"
	          "grp_groups WRITE,"
	          "grp_users WRITE,"
	          "crs_users READ,"
	          "crs_user_settings READ,"
	          "roo_rooms READ");
   Gbl.DB.LockedTables = true;
  }

/*****************************************************************************/
/*********** Unlock tables after changes in registration in groups ***********/
/*****************************************************************************/

void Grp_DB_UnlockTables (void)
  {
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after changing user's groups",
	     "UNLOCK TABLES");
  }

/*****************************************************************************/
/************************** Create a new group type **************************/
/*****************************************************************************/

long Grp_DB_CreateGroupType (const struct GroupType *GrpTyp)
  {
   /***** Create a new group type *****/
   return
   DB_QueryINSERTandReturnCode ("can not create type of group",
				"INSERT INTO grp_types"
				" (CrsCod,GrpTypName,"
				  "Mandatory,Multiple,MustBeOpened,OpenTime)"
				" VALUES"
				" (%ld,'%s',"
				  "'%c','%c','%c',FROM_UNIXTIME(%ld))",
				Gbl.Hierarchy.Crs.CrsCod,
				GrpTyp->GrpTypName,
				GrpTyp->MandatoryEnrolment ? 'Y' :
							     'N',
				GrpTyp->MultipleEnrolment ? 'Y' :
							    'N',
				GrpTyp->MustBeOpened ? 'Y' :
						       'N',
				(long) GrpTyp->OpenTimeUTC);
  }

/*****************************************************************************/
/***************************** Create a new group ****************************/
/*****************************************************************************/

void Grp_DB_CreateGroup (const struct Grp_Groups *Grps)
  {
   /***** Create a new group *****/
   DB_QueryINSERT ("can not create group",
		   "INSERT INTO grp_groups"
		   " (GrpTypCod,GrpName,RooCod,MaxStudents,Open,FileZones)"
		   " VALUES"
		   " (%ld,'%s',%ld,%u,'N','N')",
	           Grps->GrpTyp.GrpTypCod,
	           Grps->GrpName,
	           Grps->RooCod,
	           Grps->MaxStudents);
  }

/*****************************************************************************/
/******************** Check if a group exists in database ********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpExists (long GrpCod)
  {
   return (DB_QueryCOUNT ("can not check if a group exists",
			  "SELECT COUNT(*)"
			   " FROM grp_groups"
			  " WHERE GrpCod=%ld",
			  GrpCod) != 0);
  }

/*****************************************************************************/
/******************* Check if a group belongs to a course ********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpBelongsToCrs (long GrpCod,long CrsCod)
  {
   /***** Get if a group exists from database *****/
   return (DB_QueryCOUNT ("can not check if a group belongs to a course",
			  "SELECT COUNT(*)"
			   " FROM grp_groups,"
			         "grp_types"
			  " WHERE grp_groups.GrpCod=%ld"
			    " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
			    " AND grp_types.CrsCod=%ld",
			  GrpCod,CrsCod) != 0);
  }

/*****************************************************************************/
/******************* Check if name of group type exists **********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpTypNameExistsInCurrentCrs (const char *GrpTypName,long GrpTypCod)
  {
   /***** Get number of group types with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of type of group"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM grp_types"
			  " WHERE CrsCod=%ld"
			    " AND GrpTypName='%s'"
			    " AND GrpTypCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,
			  GrpTypName,
			  GrpTypCod) != 0);
  }

/*****************************************************************************/
/************************ Check if name of group exists **********************/
/*****************************************************************************/

bool Grp_DB_CheckIfGrpNameExistsForGrpTyp (long GrpTypCod,const char *GrpName,long GrpCod)
  {
   /***** Get number of groups with a type and a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of group already existed",
			  "SELECT COUNT(*)"
			   " FROM grp_groups"
			  " WHERE GrpTypCod=%ld"
			    " AND GrpName='%s'"
			    " AND GrpCod<>%ld",
			  GrpTypCod,
			  GrpName,
			  GrpCod) != 0);
  }

/*****************************************************************************/
/********************* Count number of users in a group **********************/
/*****************************************************************************/

unsigned Grp_DB_CountNumUsrsInGrp (Rol_Role_t Role,long GrpCod)
  {
   /***** Get number of students in a group from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users in a group",
		  "SELECT COUNT(*)"
		   " FROM grp_users,"
			 "grp_groups,"
			 "grp_types,"
			 "crs_users"
		  " WHERE grp_users.GrpCod=%ld"
		    " AND grp_users.GrpCod=grp_groups.GrpCod"
		    " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		    " AND grp_types.CrsCod=crs_users.CrsCod"
		    " AND grp_users.UsrCod=crs_users.UsrCod"
		    " AND crs_users.Role=%u",
		  GrpCod,
		  (unsigned) Role);
  }

/*****************************************************************************/
/*** Count # of users of current course not belonging to groups of a type ****/
/*****************************************************************************/

unsigned Grp_DB_CountNumUsrsInNoGrpsOfType (Rol_Role_t Role,long GrpTypCod)
  {
   /***** Get number of users not belonging to groups of a type ******/
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of users"
		  " not belonging to groups of a type",
		  "SELECT COUNT(UsrCod)"
		   " FROM crs_users"
		  " WHERE CrsCod=%ld"
		    " AND Role=%u"
		    " AND UsrCod NOT IN"
		        " (SELECT DISTINCT grp_users.UsrCod"
			   " FROM grp_groups,"
			         "grp_users"
			  " WHERE grp_groups.GrpTypCod=%ld"
			    " AND grp_groups.GrpCod=grp_users.GrpCod)",
		  Gbl.Hierarchy.Crs.CrsCod,
		  (unsigned) Role,
		  GrpTypCod);
  }

/*****************************************************************************/
/********* Check if I belong to any groups of a given type I belong **********/
/*****************************************************************************/

bool Grp_DB_CheckIfIBelongToGrpsOfType (long GrpTypCod)
  {
   /***** Get a group which I belong to from database *****/
   return (DB_QueryCOUNT ("can not check if you belong to a group type",
			  "SELECT COUNT(grp_groups.GrpCod)"
			   " FROM grp_groups,"
				 "grp_users"
			  " WHERE grp_groups.GrpTypCod=%ld"
			    " AND grp_groups.GrpCod=grp_users.GrpCod"
			    " AND grp_users.UsrCod=%ld",	// I belong
			  GrpTypCod,
			  Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
  }

/*****************************************************************************/
/************** Get group types with groups in current course ****************/
/*****************************************************************************/

unsigned Grp_DB_GetGrpTypesWithGrpsInCurrentCrs (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get types of group of a course",
		   "SELECT grp_types.GrpTypCod,"			// row[0]
			  "grp_types.GrpTypName,"			// row[1]
			  "grp_types.Mandatory,"			// row[2]
			  "grp_types.Multiple,"				// row[3]
			  "grp_types.MustBeOpened,"			// row[4]
			  "UNIX_TIMESTAMP(grp_types.OpenTime),"		// row[5]
			  "COUNT(grp_groups.GrpCod)"			// row[6]
		    " FROM grp_types,"
			  "grp_groups"
		   " WHERE grp_types.CrsCod=%ld"
		     " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		   " GROUP BY grp_types.GrpTypCod"
		   " ORDER BY grp_types.GrpTypName",
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********** Get group types with or without groups in current course *********/
/*****************************************************************************/

unsigned Grp_DB_GetAllGrpTypesInCurrentCrs (MYSQL_RES **mysql_res)
  {
   // The tables in the second part of the UNION requires ALIAS in order to LOCK TABLES when registering in groups
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get types of group of a course",
		   "(SELECT grp_types.GrpTypCod,"			// row[0]
			   "grp_types.GrpTypName AS GrpTypName,"	// row[1]
			   "grp_types.Mandatory,"			// row[2]
			   "grp_types.Multiple,"			// row[3]
			   "grp_types.MustBeOpened,"			// row[4]
			   "UNIX_TIMESTAMP(grp_types.OpenTime),"	// row[5]
			   "COUNT(grp_groups.GrpCod)"			// row[6]
		     " FROM grp_types,"
			   "grp_groups"
		    " WHERE grp_types.CrsCod=%ld"
		      " AND grp_types.GrpTypCod=grp_groups.GrpTypCod"
		    " GROUP BY grp_types.GrpTypCod)"
		   " UNION "
		   "(SELECT GrpTypCod,"					// row[0]
			   "GrpTypName,"				// row[1]
			   "Mandatory,"					// row[2]
			   "Multiple,"					// row[3]
			   "MustBeOpened,"				// row[4]
			   "UNIX_TIMESTAMP(OpenTime),"			// row[5]
			   "0"
		     " FROM grp_types"
		    " WHERE CrsCod=%ld"
		      " AND GrpTypCod NOT IN"
			  " (SELECT GrpTypCod"
			     " FROM grp_groups))"
		   " ORDER BY GrpTypName",
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/*********** Get group types in current course that must be opened ***********/
/*****************************************************************************/

unsigned Grp_DB_GetGrpTypesInCurrentCrsToBeOpened (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the types of group to be opened",
		   "SELECT GrpTypCod"
		    " FROM grp_types"
		   " WHERE CrsCod=%ld"
		     " AND MustBeOpened='Y'"
		     " AND OpenTime<=NOW()",
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/*********** Query the number of groups that hay in this course **************/
/*****************************************************************************/

unsigned Grp_DB_CountNumGrpsInCurrentCrs (void)
  {
   /***** Get number of group in current course from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of groups in this course",
		  "SELECT COUNT(*)"
		   " FROM grp_types,"
			 "grp_groups"
		  " WHERE grp_types.CrsCod=%ld"
		    " AND grp_types.GrpTypCod=grp_groups.GrpTypCod",
		  Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/****************** Count number of groups in a group type *******************/
/*****************************************************************************/

unsigned Grp_DB_CountNumGrpsInThisCrsOfType (long GrpTypCod)
  {
   /***** Get number of groups of a type from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of groups of a type",
		  "SELECT COUNT(*)"
		   " FROM grp_groups"
		  " WHERE GrpTypCod=%ld",
		  GrpTypCod);
  }

/*****************************************************************************/
/******************** Get groups of a type in this course ********************/
/*****************************************************************************/

unsigned Grp_DB_GetGrpsOfType (MYSQL_RES **mysql_res,long GrpTypCod)
  {
   /***** Get groups of a type from database *****/
   // Don't use INNER JOIN because there are groups without assigned room
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of a type",
		   "SELECT grp_groups.GrpCod,"		// row[0]
			  "grp_groups.GrpName,"		// row[1]
			  "grp_groups.RooCod,"		// row[2]
			  "roo_rooms.ShortName,"	// row[3]
			  "grp_groups.MaxStudents,"	// row[4]
			  "grp_groups.Open,"		// row[5]
			  "grp_groups.FileZones"	// row[6]
		    " FROM grp_groups"
		    " LEFT JOIN roo_rooms"
		      " ON grp_groups.RooCod=roo_rooms.RooCod"
		   " WHERE grp_groups.GrpTypCod=%ld"
		   " ORDER BY grp_groups.GrpName",
		   GrpTypCod);
  }

/*****************************************************************************/
/********************** Get the type of group of a group *********************/
/*****************************************************************************/

long Grp_DB_GetGrpTypeFromGrp (long GrpCod)
  {
   long GrpTypCod;

   /***** Get group type of a group from database *****/
   GrpTypCod = DB_QuerySELECTCode ("can not get the type of a group",
				   "SELECT GrpTypCod"
				    " FROM grp_groups"
				   " WHERE GrpCod=%ld",
				   GrpCod);
   if (GrpTypCod <= 0)
      Err_WrongGrpTypExit ();

   return GrpTypCod;
  }

/*****************************************************************************/
/*********** Check if an assignment, attendance event, survey,    ************/
/*********** exam session or match is associated to a given group ************/
/*****************************************************************************/

bool Grp_DB_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                    long Cod,long GrpCod)
  {
   return (DB_QueryCOUNT ("can not check if associated to a group",
			  "SELECT COUNT(*)"
			   " FROM %s"
			  " WHERE %s=%ld"
			    " AND GrpCod=%ld",
		  	  Table,
		  	  Field,Cod,
		  	  GrpCod) != 0);
  }

/*****************************************************************************/
/************ Check if an assignment, attendance event, survey, **************/
/************ exam session or match is associated to any group  **************/
/*****************************************************************************/

bool Grp_DB_CheckIfAssociatedToGrps (const char *Table,const char *Field,long Cod)
  {
   /***** Trivial check *****/
   if (Cod <= 0)	// Assignment, attendance event, survey, exam event or match code
      return false;

   /***** Check if an assignment, attendance event, survey,
          exam session or match is associated to any group *****/
   return (DB_QueryCOUNT ("can not check if associated to groups",
			  "SELECT COUNT(*)"
			   " FROM %s"
			  " WHERE %s=%ld",
			  Table,
			  Field,Cod) != 0);
  }

/*****************************************************************************/
/******************** Open all the closed groups of a tyoe *******************/
/*****************************************************************************/

void Grp_DB_OpenGrpsOfType (long GrpTypCod)
  {
   DB_QueryUPDATE ("can not open groups",
		   "UPDATE grp_groups"
		     " SET Open='Y'"
		   " WHERE GrpTypCod=%ld"
		     " AND Open='N'",
		   GrpTypCod);
  }

/*****************************************************************************/
/******************** Set type of group to not be opened *********************/
/*****************************************************************************/

void Grp_DB_ClearMustBeOpened (long GrpTypCod)
  {
   DB_QueryUPDATE ("can not update the opening of a type of group",
		   "UPDATE grp_types"
		     " SET MustBeOpened='N'"
		   " WHERE GrpTypCod=%ld",
		   GrpTypCod);
  }

/*****************************************************************************/
/*********************** Register a user in a group **************************/
/*****************************************************************************/

void Grp_DB_AddUsrToGrp (long UsrCod,long GrpCod)
  {
   DB_QueryINSERT ("can not add a user to a group",
		   "INSERT INTO grp_users"
		   " (GrpCod,UsrCod)"
		   " VALUES"
		   " (%ld,%ld)",
                   GrpCod,
                   UsrCod);
  }

/*****************************************************************************/
/************************* Remove a user from a group ************************/
/*****************************************************************************/

void Grp_DB_RemoveUsrFromGrp (long UsrCod,long GrpCod)
  {
   DB_QueryDELETE ("can not remove a user from a group",
		   "DELETE FROM grp_users"
		   " WHERE GrpCod=%ld"
		     " AND UsrCod=%ld",
                   GrpCod,
                   UsrCod);
  }

/*****************************************************************************/
/*************** Remove a user of all the groups of a course *****************/
/*****************************************************************************/

void Grp_DB_RemUsrFromAllGrpsInCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove a user from all groups of a course",
		   "DELETE FROM grp_users"
		   " WHERE UsrCod=%ld"
		     " AND GrpCod IN"
		         " (SELECT grp_groups.GrpCod"
		            " FROM grp_types,"
		                  "grp_groups"
		           " WHERE grp_types.CrsCod=%ld"
		             " AND grp_types.GrpTypCod=grp_groups.GrpTypCod)",
                   UsrCod,
                   CrsCod);
  }

/*****************************************************************************/
/*********************** Remove a user from all groups ***********************/
/*****************************************************************************/

void Grp_DB_RemUsrFromAllGrps (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from the groups he/she belongs to",
		   "DELETE FROM grp_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
