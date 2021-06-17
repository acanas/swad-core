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
// #include "swad_error.h"
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
/************ Check if an assignment is associated to a group ****************/
/*****************************************************************************/

bool Grp_DB_CheckIfAssociatedToGrp (const char *Table,const char *Field,
                                    long Cod,long GrpCod)
  {
   /***** Get if an assignment, attendance event, survey, exam event or match
          is associated to a given group from database *****/
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
/*** Check if an assignment, attendance event, survey, exam event or match ***/
/*** is associated to any group                                            ***/
/*****************************************************************************/

bool Grp_DB_CheckIfAssociatedToGrps (const char *Table,const char *Field,long Cod)
  {
   /***** Trivial check *****/
   if (Cod <= 0)	// Assignment, attendance event, survey, exam event or match code
      return false;

   /***** Get if an assignment, attendance event, survey, exam event or match
          is associated to any group from database *****/
   return (DB_QueryCOUNT ("can not check if associated to groups",
			  "SELECT COUNT(*)"
			   " FROM %s"
			  " WHERE %s=%ld",
			  Table,
			  Field,Cod) != 0);
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
