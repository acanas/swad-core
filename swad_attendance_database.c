// swad_attendance_database.c: control of attendance operations with database

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_attendance.h"
#include "swad_attendance_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Public variables ****************************/
/*****************************************************************************/

unsigned (*Att_DB_GetListAttEvents[Grp_NUM_WHICH_GROUPS]) (MYSQL_RES **mysql_res,
							   Dat_StartEndTime_t SelectedOrder,
							   Att_OrderNewestOldest_t OrderNewestOldest) =
 {
  [Grp_MY_GROUPS ] = Att_DB_GetListAttEventsMyGrps,
  [Grp_ALL_GROUPS] = Att_DB_GetListAttEventsAllGrps,
 };

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

static const char *Att_DB_HiddenSubQuery[Rol_NUM_ROLES] =
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
static const char *Att_DB_OrderBySubQuery[Dat_NUM_START_END_TIME][Att_NUM_ORDERS_NEWEST_OLDEST] =
  {
   [Dat_STR_TIME][Att_NEWEST_FIRST] = "StartTime DESC,"
	                              "EndTime DESC,"
	                              "Title DESC",
   [Dat_STR_TIME][Att_OLDEST_FIRST] = "StartTime,"
	                              "EndTime,"
	                              "Title",

   [Dat_END_TIME][Att_NEWEST_FIRST] = "EndTime DESC,"
	                                "StartTime DESC,"
	                                "Title DESC",
   [Dat_END_TIME][Att_OLDEST_FIRST] = "EndTime,"
	                                "StartTime,"
	                                "Title",
  };

/*****************************************************************************/
/******************************** Private types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

/*****************************************************************************/
/**************** Get list of attendance events in my groups *****************/
/*****************************************************************************/

unsigned Att_DB_GetListAttEventsMyGrps (MYSQL_RES **mysql_res,
                                        Dat_StartEndTime_t SelectedOrder,
                                        Att_OrderNewestOldest_t OrderNewestOldest)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get attendance events",
		   "SELECT AttCod"
		    " FROM att_events"
		   " WHERE CrsCod=%ld"
		     "%s"
		     " AND (AttCod NOT IN"
			  " (SELECT AttCod"
			   " FROM att_groups)"
			  " OR"
			  " AttCod IN"
			  " (SELECT att_groups.AttCod"
			     " FROM grp_users,"
				   "att_groups"
			    " WHERE grp_users.UsrCod=%ld"
			      " AND att_groups.GrpCod=grp_users.GrpCod))"
		   " ORDER BY %s",
		   Gbl.Hierarchy.Crs.CrsCod,
		   Att_DB_HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Att_DB_OrderBySubQuery[SelectedOrder][OrderNewestOldest]);
  }

/*****************************************************************************/
/********************* Get list of all attendance events *********************/
/*****************************************************************************/

unsigned Att_DB_GetListAttEventsAllGrps (MYSQL_RES **mysql_res,
                                         Dat_StartEndTime_t SelectedOrder,
                                         Att_OrderNewestOldest_t OrderNewestOldest)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get attendance events",
		   "SELECT AttCod"
		    " FROM att_events"
		   " WHERE CrsCod=%ld%s"
		   " ORDER BY %s",
		   Gbl.Hierarchy.Crs.CrsCod,
		   Att_DB_HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
		   Att_DB_OrderBySubQuery[SelectedOrder][OrderNewestOldest]);
  }

/*****************************************************************************/
/********************* Get list of all attendance events *********************/
/*****************************************************************************/

unsigned Att_DB_GetDataOfAllAttEvents (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get attendance events",
		   "SELECT AttCod,"				// row[0]
			  "Hidden,"				// row[1]
			  "UsrCod,"				// row[2]
			  "UNIX_TIMESTAMP(StartTime) AS ST,"	// row[3]
			  "UNIX_TIMESTAMP(EndTime) AS ET,"	// row[4]
			  "CommentTchVisible,"			// row[5]
			  "Title,"				// row[6]
			  "Txt"					// row[7]
		    " FROM att_events"
		   " WHERE CrsCod=%d"
		   " ORDER BY ST DESC,"
			     "ET DESC,"
			     "Title DESC",
		   CrsCod);
  }

/*****************************************************************************/
/**************** Get attendance event data using its code *******************/
/*****************************************************************************/

unsigned Att_DB_GetDataOfAttEventByCod (MYSQL_RES **mysql_res,long AttCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get attendance event data",
		   "SELECT AttCod,"					// row[0]
			  "CrsCod,"					// row[1]
			  "Hidden,"					// row[2]
			  "UsrCod,"					// row[3]
			  "UNIX_TIMESTAMP(StartTime),"			// row[4]
			  "UNIX_TIMESTAMP(EndTime),"			// row[5]
			  "NOW() BETWEEN StartTime AND EndTime,"	// row[6]
			  "CommentTchVisible,"				// row[7]
			  "Title"					// row[8]
		    " FROM att_events"
		   " WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/***************** Get attendance event text from database *******************/
/*****************************************************************************/

void Att_DB_GetAttEventDescription (long AttCod,char Description[Cns_MAX_BYTES_TEXT + 1])
  {
   /***** Get text of attendance event from database *****/
   DB_QuerySELECTString (Description,Cns_MAX_BYTES_TEXT,"can not get attendance event text",
		         "SELECT Txt"
			  " FROM att_events"
		         " WHERE AttCod=%ld"
			   " AND CrsCod=%ld",
		         AttCod,
		         Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/***** Check if the title or the folder of an attendance event exists ********/
/*****************************************************************************/

bool Att_DB_CheckIfSimilarAttEventExists (const char *Field,const char *Value,long AttCod)
  {
   return
   DB_QueryEXISTS ("can not check similar attendance events",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM att_events"
		    " WHERE CrsCod=%ld"
		      " AND %s='%s'"
		      " AND AttCod<>%ld)",
		   Gbl.Hierarchy.Crs.CrsCod,
		   Field,Value,
		   AttCod);
  }

/*****************************************************************************/
/********************* Create a new attendance event *************************/
/*****************************************************************************/

long Att_DB_CreateAttEvent (const struct Att_Event *Event,const char *Description)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new attendance event",
				"INSERT INTO att_events"
				" (CrsCod,Hidden,UsrCod,"
				  "StartTime,EndTime,"
				  "CommentTchVisible,Title,Txt)"
				" VALUES"
				" (%ld,'%c',%ld,"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%c','%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Event->Hidden ? 'Y' :
					        'N',
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Event->TimeUTC[Dat_STR_TIME],
				Event->TimeUTC[Dat_END_TIME],
				Event->CommentTchVisible ? 'Y' :
							   'N',
				Event->Title,
				Description);
  }

/*****************************************************************************/
/****************** Update the data of an attendance event *******************/
/*****************************************************************************/

void Att_DB_UpdateAttEvent (const struct Att_Event *Event,const char *Description)
  {
   DB_QueryUPDATE ("can not update attendance event",
		   "UPDATE att_events"
		     " SET Hidden='%c',"
		          "StartTime=FROM_UNIXTIME(%ld),"
		          "EndTime=FROM_UNIXTIME(%ld),"
		          "CommentTchVisible='%c',"
		          "Title='%s',"
		          "Txt='%s'"
		   " WHERE AttCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
                   Event->Hidden ? 'Y' :
        	                   'N',
                   Event->TimeUTC[Dat_STR_TIME],
                   Event->TimeUTC[Dat_END_TIME],
                   Event->CommentTchVisible ? 'Y' :
        	                              'N',
                   Event->Title,
                   Description,
                   Event->AttCod,
                   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********************** Hide/unhide an attendance event **********************/
/*****************************************************************************/

void Att_DB_HideOrUnhideAttEvent (long AttCod,bool Hide)
  {
   DB_QueryUPDATE ("can not hide/unhide assignment",
		   "UPDATE att_events"
		     " SET Hidden='%c'"
		   " WHERE AttCod=%ld"
		     " AND CrsCod=%ld",
		   Hide ? 'Y' :
			  'N',
                   AttCod,
                   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/****************** Create group of an attendance event **********************/
/*****************************************************************************/

void Att_DB_CreateGroup (long AttCod,long GrpCod)
  {
   DB_QueryINSERT ("can not associate a group to an attendance event",
		   "INSERT INTO att_groups"
		   " (AttCod,GrpCod)"
		   " VALUES"
		   " (%ld,%ld)",
		   AttCod,
		   GrpCod);
  }

/*****************************************************************************/
/************ Get group codes associated to an attendance event **************/
/*****************************************************************************/

unsigned Att_DB_GetGrpCodsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of an attendance event",
		   "SELECT GrpCod"	// row[0]
		    " FROM att_groups"
		   " WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/**************** Get groups associated to an attendance event ***************/
/*****************************************************************************/

unsigned Att_DB_GetGroupsAssociatedToEvent (MYSQL_RES **mysql_res,long AttCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of an attendance event",
		   "SELECT grp_types.GrpTypName,"	// row[0]
			  "grp_groups.GrpName,"		// row[1]
			  "roo_rooms.ShortName"		// row[2]
		    " FROM (att_groups,"
			   "grp_groups,"
			   "grp_types)"
		    " LEFT JOIN roo_rooms"
		      " ON grp_groups.RooCod=roo_rooms.RooCod"
		   " WHERE att_groups.AttCod=%ld"
		     " AND att_groups.GrpCod=grp_groups.GrpCod"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		   " ORDER BY grp_types.GrpTypName,"
			     "grp_groups.GrpName",
		   AttCod);
  }

/*****************************************************************************/
/*************** Remove one group from all attendance events *****************/
/*****************************************************************************/

void Att_DB_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all attendance events *****/
   DB_QueryDELETE ("can not remove group from the associations"
	           " between attendance events and groups",
		   "DELETE FROM att_groups"
		   " WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/********** Remove groups of one type from all attendance events *************/
/*****************************************************************************/

void Att_DB_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all attendance events *****/
   DB_QueryDELETE ("can not remove groups of a type from the associations"
		   " between attendance events and groups",
		   "DELETE FROM att_groups"
		   " USING grp_groups,"
		          "att_groups"
		   " WHERE grp_groups.GrpTypCod=%ld"
		     " AND grp_groups.GrpCod=att_groups.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/****************** Remove groups of an attendance event *********************/
/*****************************************************************************/

void Att_DB_RemoveGrpsAssociatedToAnAttEvent (long AttCod)
  {
   DB_QueryDELETE ("can not remove the groups"
		   " associated to an attendance event",
		   "DELETE FROM att_groups"
		   " WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/******* Get number of students from a list who attended to an event *********/
/*****************************************************************************/

unsigned Att_DB_GetNumStdsTotalWhoAreInAttEvent (long AttCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of students registered in an event",
		  "SELECT COUNT(*)"
		   " FROM att_users"
		  " WHERE AttCod=%ld"
		    " AND Present='Y'",
		  AttCod);
  }

/*****************************************************************************/
/********** Get number of users from a list in an attendance event ***********/
/*****************************************************************************/

unsigned Att_DB_GetNumStdsFromListWhoAreInAttEvent (long AttCod,const char *SubQueryUsrs)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of students from a list"
		  " who are registered in an event",
		  "SELECT COUNT(*)"
		   " FROM att_users"
		  " WHERE AttCod=%ld"
		    " AND UsrCod IN (%s)"
		    " AND Present='Y'",
		  AttCod,SubQueryUsrs);
  }

/*****************************************************************************/
/******************* Check if a user attended to an event ********************/
/*****************************************************************************/
// Return if user is in table

bool Att_DB_CheckIfUsrIsInTableAttUsr (long AttCod,long UsrCod,bool *Present)
  {
   char StrPresent[1 + 1];

   /***** Check if a user is registered in an event in database *****/
   DB_QuerySELECTString (StrPresent,1,"can not check if a user"
	                              " is already registered in an event",
		         "SELECT Present"
			  " FROM att_users"
		         " WHERE AttCod=%ld"
			   " AND UsrCod=%ld",
		         AttCod,
		         UsrCod);
   if (StrPresent[0])
     {
      *Present = (StrPresent[0] == 'Y');
      return true;	// User is in table
     }
   *Present = false;
   return false;	// User is not in table
  }

/*****************************************************************************/
/********** Get if a student attended to an event and get comments ***********/
/*****************************************************************************/

unsigned Att_DB_GetPresentAndComments (MYSQL_RES **mysql_res,long AttCod,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get if a student"
			     " is already registered in an event",
		   "SELECT Present,"		// row[0]
			  "CommentStd,"	// row[1]
			  "CommentTch"	// row[2]
		    " FROM att_users"
		   " WHERE AttCod=%ld"
		     " AND UsrCod=%ld",
		   AttCod,
		   UsrCod);
  }

/*****************************************************************************/
/********* Register a user in an attendance event changing comments **********/
/*****************************************************************************/

void Att_DB_RegUsrInAttEventChangingComments (long AttCod,long UsrCod,
                                              bool Present,
                                              const char *CommentStd,
                                              const char *CommentTch)
  {
   /***** Register user as assistant to an event in database *****/
   DB_QueryREPLACE ("can not register user in an event",
		    "REPLACE INTO att_users"
		    " (AttCod,UsrCod,Present,CommentStd,CommentTch)"
		    " VALUES"
		    " (%ld,%ld,'%c','%s','%s')",
                    AttCod,
                    UsrCod,
                    Present ? 'Y' :
        	              'N',
                    CommentStd,
                    CommentTch);
  }

/*****************************************************************************/
/**************** Set user as present in an attendance event *****************/
/*****************************************************************************/

void Att_DB_SetUsrAsPresent (long AttCod,long UsrCod)
  {
   DB_QueryUPDATE ("can not set user as present in an event",
		   "UPDATE att_users"
		     " SET Present='Y'"
		   " WHERE AttCod=%ld"
		     " AND UsrCod=%ld",
		   AttCod,
		   UsrCod);
  }

/*****************************************************************************/
/********************** Remove a user from an event **************************/
/*****************************************************************************/

void Att_DB_RemoveUsrFromAttEvent (long AttCod,long UsrCod)
  {
   DB_QueryDELETE ("can not remove student from an event",
		   "DELETE FROM att_users"
		   " WHERE AttCod=%ld"
		     " AND UsrCod=%ld",
                   AttCod,UsrCod);
  }

/*****************************************************************************/
/************ Remove users absent without comments from an event *************/
/*****************************************************************************/

void Att_DB_RemoveUsrsAbsentWithoutCommentsFromAttEvent (long AttCod)
  {
   /***** Clean table att_users *****/
   DB_QueryDELETE ("can not remove users absent"
	           " without comments from an event",
		   "DELETE FROM att_users"
		   " WHERE AttCod=%ld"
		     " AND Present='N'"
		     " AND CommentStd=''"
		     " AND CommentTch=''",
	           AttCod);
  }

/*****************************************************************************/
/*********** Remove all users registered in an attendance event **************/
/*****************************************************************************/

void Att_DB_RemoveAllUsrsFromAnAttEvent (long AttCod)
  {
   DB_QueryDELETE ("can not remove attendance event",
		   "DELETE FROM att_users"
		   " WHERE AttCod=%ld",
		   AttCod);
  }

/*****************************************************************************/
/*** Remove one user from all attendance events where he/she is registered ***/
/*****************************************************************************/

void Att_DB_RemoveUsrFromAllAttEvents (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user from all attendance events",
		   "DELETE FROM att_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************* Remove one student from all attendance events *****************/
/*****************************************************************************/

void Att_DB_RemoveUsrFromCrsAttEvents (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove user from attendance events of a course",
		   "DELETE FROM att_users"
		   " USING att_events,"
		          "att_users"
		   " WHERE att_events.CrsCod=%ld"
		     " AND att_events.AttCod=att_users.AttCod"
		     " AND att_users.UsrCod=%ld",
                   CrsCod,UsrCod);
  }

/*****************************************************************************/
/*********************** Remove an attendance event **************************/
/*****************************************************************************/

void Att_DB_RemoveAttEventFromCurrentCrs (long AttCod)
  {
   DB_QueryDELETE ("can not remove attendance event",
		   "DELETE FROM att_events"
		   " WHERE AttCod=%ld AND CrsCod=%ld",
                   AttCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************* Remove users in all attendance events of a course *************/
/*****************************************************************************/

void Att_DB_RemoveUsrsFromCrsAttEvents (long CrsCod)
  {
   DB_QueryDELETE ("can not remove users registered"
		   " in attendance events of a course",
		   "DELETE FROM att_users"
		   " USING att_events,"
		          "att_users"
		   " WHERE att_events.CrsCod=%ld"
		     " AND att_events.AttCod=att_users.AttCod",
                   CrsCod);
  }

/*****************************************************************************/
/************ Remove groups in all attendance events of a course *************/
/*****************************************************************************/

void Att_DB_RemoveGrpsAssociatedToCrsAttEvents (long CrsCod)
  {
   DB_QueryDELETE ("can not remove all groups associated"
		   " to attendance events of a course",
		   "DELETE FROM att_groups"
		   " USING att_events,"
		          "att_groups"
		   " WHERE att_events.CrsCod=%ld"
		     " AND att_events.AttCod=att_groups.AttCod",
                   CrsCod);
  }

/*****************************************************************************/
/***************** Remove all attendance events of a course ******************/
/*****************************************************************************/

void Att_DB_RemoveCrsAttEvents (long CrsCod)
  {
   DB_QueryDELETE ("can not remove all attendance events of a course",
		   "DELETE FROM att_events"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/*************** Get number of attendance events in a course *****************/
/*****************************************************************************/

unsigned Att_DB_GetNumAttEventsInCrs (long CrsCod)
  {
   /***** Get number of attendance events in a course from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of attendance events in course",
		  "SELECT COUNT(*)"
		   " FROM att_events"
		  " WHERE CrsCod=%ld",
		  CrsCod);
  }

/*****************************************************************************/
/*************** Get number of courses with attendance events ****************/
/*****************************************************************************/
// Returns the number of courses with attendance events
// in this location (all the platform, the current degree or the current course)

unsigned Att_DB_GetNumCoursesWithAttEvents (HieLvl_Level_t Scope)
  {
   switch (Scope)
     {
      case HieLvl_SYS:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT CrsCod)"
				" FROM att_events"
			       " WHERE CrsCod>0");
      case HieLvl_INS:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT att_events.CrsCod)"
				" FROM ctr_centers,"
				      "deg_degrees,"
				      "crs_courses,"
				      "att_events"
			       " WHERE ctr_centers.InsCod=%ld"
				 " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT att_events.CrsCod)"
				" FROM deg_degrees,"
				      "crs_courses,"
				      "att_events"
			       " WHERE deg_degrees.CtrCod=%ld"
				 " AND deg_degrees.DegCod=crs_courses.DegCod"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT att_events.CrsCod)"
				" FROM crs_courses,"
				      "att_events"
			       " WHERE crs_courses.DegCod=%ld"
				 " AND crs_courses.CrsCod=att_events.CrsCod",
			       Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return DB_QueryCOUNT ("can not get number of courses with attendance events",
			       "SELECT COUNT(DISTINCT CrsCod)"
				" FROM att_events"
			       " WHERE CrsCod=%ld",
			       Gbl.Hierarchy.Crs.CrsCod);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********************* Get number of attendance events ***********************/
/*****************************************************************************/

unsigned Att_DB_GetNumAttEvents (MYSQL_RES **mysql_res,HieLvl_Level_t Scope)
  {
   switch (Scope)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(NumNotif)"			// row[1]
			  " FROM att_events"
			 " WHERE CrsCod>0");
      case HieLvl_INS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(att_events.NumNotif)"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "att_events"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(att_events.NumNotif)"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "att_events"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(att_events.NumNotif)"	// row[1]
			  " FROM crs_courses,"
			        "att_events"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=att_events.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get number of attendance events",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(NumNotif)"			// row[1]
			  " FROM att_events"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
      default:
	 Err_WrongScopeExit ();
	 return 0;	// Not reached
     }
  }
