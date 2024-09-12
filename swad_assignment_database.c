// swad_assignment_database.c: assignments operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_assignment.h"
#include "swad_assignment_database.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public variables *****************************/
/*****************************************************************************/

unsigned (*Asg_DB_GetListAssignments[Grp_NUM_MY_ALL_GROUPS]) (MYSQL_RES **mysql_res,
                                                             Dat_StartEndTime_t SelectedOrder) =
  {
   [Grp_MY_GROUPS ] = Asg_DB_GetListAssignmentsMyGrps,
   [Grp_ALL_GROUPS] = Asg_DB_GetListAssignmentsAllGrps,
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Asg_DB_HiddenSubQuery[Rol_NUM_ROLES] =
  {
   [Rol_UNK    ] = " AND Hidden='N'",
   [Rol_GST    ] = " AND Hidden='N'",
   [Rol_USR    ] = " AND Hidden='N'",
   [Rol_STD    ] = " AND Hidden='N'",
   [Rol_NET    ] = " AND Hidden='N'",
   [Rol_TCH    ] = "",
   [Rol_DEG_ADM] = " AND Hidden='N'",
   [Rol_CTR_ADM] = " AND Hidden='N'",
   [Rol_INS_ADM] = " AND Hidden='N'",
   [Rol_SYS_ADM] = "",
  };
static const char *Asg_DB_OrderSubQuery[Dat_NUM_START_END_TIME] =
  {
   [Dat_STR_TIME] = "StartTime DESC,"
		    "EndTime DESC,"
		    "Title DESC",
   [Dat_END_TIME] = "EndTime DESC,"
		    "StartTime DESC,"
		    "Title DESC",
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************* Get list of assignments for my groups *******************/
/*****************************************************************************/

unsigned Asg_DB_GetListAssignmentsMyGrps (MYSQL_RES **mysql_res,
                                          Dat_StartEndTime_t SelectedOrder)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get assignments",
		   "SELECT AsgCod"
		    " FROM asg_assignments"
		   " WHERE CrsCod=%ld%s"
		     " AND ("
			   // Assignment is for the whole course
			   "AsgCod NOT IN"
			   " (SELECT AsgCod"
			      " FROM asg_groups)"	// Not associated to any group
			  " OR"
			   // Assignment is for some of my groups
			  " AsgCod IN"
			  " (SELECT asg_groups.AsgCod"
			     " FROM asg_groups,"
				   "grp_users"
			    " WHERE grp_users.UsrCod=%ld"
			      " AND asg_groups.GrpCod=grp_users.GrpCod)"
			  ")"
		   " ORDER BY %s",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Asg_DB_HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Asg_DB_OrderSubQuery[SelectedOrder]);
  }

/*****************************************************************************/
/****************** Get list of assignments for all groups *******************/
/*****************************************************************************/

unsigned Asg_DB_GetListAssignmentsAllGrps (MYSQL_RES **mysql_res,
                                           Dat_StartEndTime_t SelectedOrder)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get assignments",
		   "SELECT AsgCod"
		    " FROM asg_assignments"
		   " WHERE CrsCod=%ld%s"
		   " ORDER BY %s",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Asg_DB_HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
		   Asg_DB_OrderSubQuery[SelectedOrder]);
  }

/*****************************************************************************/
/******************* Get assignment data using its code **********************/
/*****************************************************************************/

unsigned Asg_DB_GetAssignmentDataByCod (MYSQL_RES **mysql_res,long AsgCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get assignment data",
		   "SELECT AsgCod,"					// row[0]
			  "Hidden,"					// row[1]
			  "UsrCod,"					// row[2]
			  "UNIX_TIMESTAMP(StartTime),"			// row[3]
			  "UNIX_TIMESTAMP(EndTime),"			// row[4]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[5]
			  "Title,"					// row[6]
			  "Folder"					// row[7]
		    " FROM asg_assignments"
		   " WHERE AsgCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   AsgCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/**************** Get assignment data using its folder name ******************/
/*****************************************************************************/

unsigned Asg_DB_GetAssignmentDataByFolder (MYSQL_RES **mysql_res,
                                           const char Folder[Brw_MAX_BYTES_FOLDER + 1])
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get assignment data",
		   "SELECT AsgCod,"					// row[0]
			  "Hidden,"					// row[1]
			  "UsrCod,"					// row[2]
			  "UNIX_TIMESTAMP(StartTime),"			// row[3]
			  "UNIX_TIMESTAMP(EndTime),"			// row[4]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[5]
			  "Title,"					// row[6]
			  "Folder"					// row[7]
		    " FROM asg_assignments"
		   " WHERE CrsCod=%ld"		// Extra check
		     " AND Folder='%s'",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Folder);
  }

/*****************************************************************************/
/************* Get title and text of assignment using its code ***************/
/*****************************************************************************/

unsigned Asg_DB_GetAssignmentTitleAndTxtByCod (MYSQL_RES **mysql_res,long AsgCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get assignment title and text",
		   "SELECT Title,"	// row[0]
			  "Txt"		// row[1]
		    " FROM asg_assignments"
		   " WHERE AsgCod=%ld",
		   AsgCod);
  }

/*****************************************************************************/
/******************** Get assignment title from database *********************/
/*****************************************************************************/

void Asg_DB_GetAssignmentTitleByCod (long AsgCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get assignment title",
		         "SELECT Title"
			  " FROM asg_assignments"
		         " WHERE AsgCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
		         AsgCod,
		         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************** Get assignment text from database **********************/
/*****************************************************************************/

void Asg_DB_GetAssignmentTxtByCod (long AsgCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   DB_QuerySELECTString (Txt,Cns_MAX_BYTES_TEXT,"can not get assignment text",
		         "SELECT Txt"
			  " FROM asg_assignments"
		         " WHERE AsgCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
		         AsgCod,
		         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******** Check if the title or the folder of an assignment exists ***********/
/*****************************************************************************/

bool Asg_DB_CheckIfSimilarAssignmentExists (const char *Field,const char *Value,
                                            long AsgCod)
  {
   return
   DB_QueryEXISTS ("can not check if similar assignments exist",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM asg_assignments"
		    " WHERE CrsCod=%ld"
		      " AND %s='%s'"
		      " AND AsgCod<>%ld)",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Field,Value,
		   AsgCod);
  }

/*****************************************************************************/
/************************ Create a new assignment ****************************/
/*****************************************************************************/

long Asg_DB_CreateAssignment (const struct Asg_Assignment *Asg,const char *Txt)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new assignment",
				"INSERT INTO asg_assignments"
				" (CrsCod,UsrCod,"
				  "StartTime,EndTime,"
				  "Title,Folder,Txt)"
				" VALUES"
				" (%ld,%ld,"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%s','%s','%s')",
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Asg->TimeUTC[Dat_STR_TIME],
				Asg->TimeUTC[Dat_END_TIME],
				Asg->Title,
				Asg->Folder,
				Txt);
  }

/*****************************************************************************/
/********************* Update an existing assignment *************************/
/*****************************************************************************/

void Asg_DB_UpdateAssignment (const struct Asg_Assignment *Asg,const char *Txt)
  {
   /***** Update the data of the assignment *****/
   DB_QueryUPDATE ("can not update assignment",
		   "UPDATE asg_assignments"
		     " SET StartTime=FROM_UNIXTIME(%ld),"
		          "EndTime=FROM_UNIXTIME(%ld),"
		          "Title='%s',"
		          "Folder='%s',"
		          "Txt='%s'"
		   " WHERE AsgCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
                   Asg->TimeUTC[Dat_STR_TIME],
                   Asg->TimeUTC[Dat_END_TIME],
                   Asg->Title,
                   Asg->Folder,
                   Txt,
                   Asg->AsgCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************************* Hide/unhide an assignment *************************/
/*****************************************************************************/

void Asg_DB_HideOrUnhideAssignment (long AsgCod,
				    HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not hide/unhide assignment",
		   "UPDATE asg_assignments"
		     " SET Hidden='%c'"
		   " WHERE AsgCod=%ld"
		     " AND CrsCod=%ld",
		   HidVis_YN[HiddenOrVisible],
                   AsgCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/**************************** Remove an assignment ***************************/
/*****************************************************************************/

void Asg_DB_RemoveAssignment (long AsgCod)
  {
   DB_QueryDELETE ("can not remove assignment",
		   "DELETE FROM asg_assignments"
		   " WHERE AsgCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
                   AsgCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************* Check if I can do an assignment ***********************/
/*****************************************************************************/

Usr_Can_t Asg_DB_CheckIfICanDoAsgBasedOnGroups (long AsgCod)
  {
   // Students and teachers can do assignments depending on groups
   return
   DB_QueryEXISTS ("can not check if I can do an assignment",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM asg_assignments"
		    " WHERE AsgCod=%ld"
		      " AND ("
			    // Assignment is for the whole course
			    "AsgCod NOT IN"
			    " (SELECT AsgCod"
			       " FROM asg_groups)"
			    " OR "
			    // Assignment is for some of my groups
			    "AsgCod IN"
			    " (SELECT asg_groups.AsgCod"
			       " FROM grp_users,"
				     "asg_groups"
			      " WHERE grp_users.UsrCod=%ld"
				" AND asg_groups.GrpCod=grp_users.GrpCod)"
			   "))",
		   AsgCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod) ? Usr_CAN :
						Usr_CAN_NOT;
  }

/*****************************************************************************/
/******************* Get groups associated to an assignment ******************/
/*****************************************************************************/

unsigned Asg_DB_GetGrps (MYSQL_RES **mysql_res,long AsgCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of an assignment",
		   "SELECT grp_types.GrpTypName,"	// row[0]
			  "grp_groups.GrpName"	// row[1]
		    " FROM asg_groups,"
			  "grp_groups,"
			  "grp_types"
		   " WHERE asg_groups.AsgCod=%ld"
		     " AND asg_groups.GrpCod=grp_groups.GrpCod"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		   " ORDER BY grp_types.GrpTypName,"
			     "grp_groups.GrpName",
		   AsgCod);
  }

/*****************************************************************************/
/********************* Create a group of an assignment ************************/
/*****************************************************************************/

void Asg_DB_CreateGroup (long AsgCod,long GrpCod)
  {
   DB_QueryINSERT ("can not associate a group to an assignment",
		   "INSERT INTO asg_groups"
		   " (AsgCod,GrpCod)"
		   " VALUES"
		   " (%ld,%ld)",
		   AsgCod,
		   GrpCod);
  }

/*****************************************************************************/
/******************* Remove one group from all assignments *******************/
/*****************************************************************************/

void Asg_DB_RemoveGroup (long GrpCod)
  {
   DB_QueryDELETE ("can not remove group from the associations"
	           " between assignments and groups",
		   "DELETE FROM asg_groups"
		   " WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/************* Remove groups of one type from all assignments ****************/
/*****************************************************************************/

void Asg_DB_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all assignments *****/
   DB_QueryDELETE ("can not remove groups of a type from the associations"
	           " between assignments and groups",
		   "DELETE FROM asg_groups"
		   " USING grp_groups,"
		          "asg_groups"
		   " WHERE grp_groups.GrpTypCod=%ld"
		     " AND grp_groups.GrpCod=asg_groups.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/********************* Remove groups of an assignment ************************/
/*****************************************************************************/

void Asg_DB_RemoveGrpsAssociatedToAnAssignment (long AsgCod)
  {
   /***** Remove groups of the assignment *****/
   DB_QueryDELETE ("can not remove the groups associated to an assignment",
		   "DELETE FROM asg_groups"
		   " WHERE AsgCod=%ld",
		   AsgCod);
  }

/*****************************************************************************/
/*********** Remove groups associated to assignments in a course *************/
/*****************************************************************************/

void Asg_DB_RemoveGrpsAssociatedToAsgsInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove groups associated to assignments in a course",
		   "DELETE FROM asg_groups"
		   " USING asg_assignments,"
		          "asg_groups"
		   " WHERE asg_assignments.CrsCod=%ld"
		     " AND asg_assignments.AsgCod=asg_groups.AsgCod",
                   CrsCod);
  }

/*****************************************************************************/
/******************** Remove all assignments in a course *********************/
/*****************************************************************************/

void Asg_DB_RemoveCrsAssignments (long CrsCod)
  {
   DB_QueryDELETE ("can not remove all assignments in a course",
		   "DELETE FROM asg_assignments"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/******** Update number of users notified in table of assignments ************/
/*****************************************************************************/

void Asg_DB_UpdateNumUsrsNotifiedByEMailAboutAssignment (long AsgCod,
                                                         unsigned NumUsrsToBeNotifiedByEMail)
  {
   DB_QueryUPDATE ("can not update the number of notifs. of an assignment",
		   "UPDATE asg_assignments"
		     " SET NumNotif=NumNotif+%u"
		   " WHERE AsgCod=%ld",
                   NumUsrsToBeNotifiedByEMail,
                   AsgCod);
  }

/*****************************************************************************/
/********** Get all user codes belonging to an assignment, except me *********/
/*****************************************************************************/

unsigned Asg_DB_GetUsrsFromAssignmentExceptMe (MYSQL_RES **mysql_res,long AsgCod)
  {
   // 1. If the assignment is available for the whole course ==> get all users enroled in the course except me
   // 2. If the assignment is available only for some groups ==> get all users who belong to any of the groups except me
   // Cases 1 and 2 are mutually exclusive, so the union returns the case 1 or 2
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users to be notified",
		   "(SELECT crs_users.UsrCod"
		     " FROM asg_assignments,"
			   "crs_users"
		   " WHERE asg_assignments.AsgCod=%ld"
		     " AND asg_assignments.AsgCod NOT IN"
			 " (SELECT AsgCod"
			    " FROM asg_groups"
			   " WHERE AsgCod=%ld)"
		     " AND asg_assignments.CrsCod=crs_users.CrsCod"
		     " AND crs_users.UsrCod<>%ld)"
		   " UNION "
		   "(SELECT DISTINCT "
		           "grp_users.UsrCod"
		     " FROM asg_groups,"
			   "grp_users"
		    " WHERE asg_groups.AsgCod=%ld"
		      " AND asg_groups.GrpCod=grp_users.GrpCod"
		      " AND grp_users.UsrCod<>%ld)",
		   AsgCod,
		   AsgCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   AsgCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/****************** Get number of courses with assignments *******************/
/*****************************************************************************/
// Returns the number of courses with assignments
// in this location (all the platform, the current degree or the current course)

unsigned Asg_DB_GetNumCoursesWithAssignments (Hie_Level_t Level)
  {
   /***** Get number of courses with assignments from database *****/
   switch (Level)
     {
      case Hie_SYS:
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with assignments",
                        "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM asg_assignments"
			" WHERE CrsCod>0");
      case Hie_CTY:
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with assignments",
                        "SELECT COUNT(DISTINCT asg_assignments.CrsCod)"
			 " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "courses,"
			       "asg_assignments"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                        Gbl.Hierarchy.Node[Hie_CTY].HieCod);
       case Hie_INS:
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with assignments",
                        "SELECT COUNT(DISTINCT asg_assignments.CrsCod)"
			 " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "asg_assignments"
			" WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                        Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with assignments",
                        "SELECT COUNT(DISTINCT asg_assignments.CrsCod)"
			 " FROM deg_degrees,"
			       "crs_courses,"
			       "asg_assignments"
			" WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                        Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with assignments",
                        "SELECT COUNT(DISTINCT asg_assignments.CrsCod)"
			 " FROM crs_courses,"
			       "asg_assignments"
			" WHERE crs_courses.DegCod=%ld"
			  " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                        Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with assignments",
                        "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM asg_assignments"
			" WHERE CrsCod=%ld",
                        Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************************ Get number of assignments **************************/
/*****************************************************************************/
// Returns the number of assignments
// in this location (all the platform, the current degree or the current course)

unsigned Asg_DB_GetNumAssignments (MYSQL_RES **mysql_res,Hie_Level_t Level)
  {
   /***** Get number of assignments from database *****/
   switch (Level)
     {
      case Hie_SYS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),"			// row[0]
                                "SUM(NumNotif)"			// row[1]
			  " FROM asg_assignments"
			 " WHERE CrsCod>0");
      case Hie_CTY:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),"			// row[0]
                                "SUM(asg_assignments.NumNotif)"	// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "asg_assignments"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                         Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),"			// row[0]
                                "SUM(asg_assignments.NumNotif)"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "asg_assignments"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                         Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),"			// row[0]
                                "SUM(asg_assignments.NumNotif)"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "asg_assignments"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                         Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),"			// row[0]
                                "SUM(asg_assignments.NumNotif)"	// row[1]
			  " FROM crs_courses,"
			        "asg_assignments"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=asg_assignments.CrsCod",
                         Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of assignments",
                         "SELECT COUNT(*),"			// row[0]
                                "SUM(NumNotif)"			// row[1]
			  " FROM asg_assignments"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
         return 0;	// Not reached
     }
  }

/*****************************************************************************/
/****************** Get number of assignments in a course ********************/
/*****************************************************************************/

unsigned Asg_DB_GetNumAssignmentsInCrs (long CrsCod)
  {
   /***** Get number of assignments in a course from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of assignments in course",
		  "SELECT COUNT(*)"
		   " FROM asg_assignments"
		  " WHERE CrsCod=%ld",
		  CrsCod);
  }
