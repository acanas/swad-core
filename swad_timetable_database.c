// swad_timetable_database.c: timetables, operations with database

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

#include "swad_action_list.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_timetable_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Public constants and types ***********************/
/*****************************************************************************/

const char *Tmt_DB_ClassType[Tmt_NUM_CLASS_TYPES] =
  {
   [Tmt_FREE     ] = "free",
   [Tmt_LECTURE  ] = "lecture",
   [Tmt_PRACTICAL] = "practical",
   [Tmt_TUTORING ] = "tutoring",
  };

/*****************************************************************************/
/********************* Insert class in course timetable **********************/
/*****************************************************************************/

void Tmt_DB_InsertHourInCrsTimeTable (long CrsCod,
                                      const struct Tmt_Column *Column,
                                      unsigned Weekday,unsigned Hour,unsigned Min,
                                      unsigned SecondsPerInterval)
  {
   DB_QueryINSERT ("can not insert hour in course timetable",
		   "INSERT INTO tmt_courses"
		   " (CrsCod,GrpCod,Weekday,StartTime,Duration,"
		   "ClassType,Info)"
		   " VALUES"
		   " (%ld,%ld,%u,'%02u:%02u:00',SEC_TO_TIME(%u),"
		   "'%s','%s')",
                   CrsCod,
	           Column->GrpCod,
	           Weekday,Hour,Min,
	           Column->DurationIntervals * SecondsPerInterval,
		   Tmt_DB_ClassType[Column->ClassType],
		   Column->Info);
  }

/*****************************************************************************/
/********************* Insert class in course timetable **********************/
/*****************************************************************************/

void Tmt_DB_InsertHourInUsrTimeTable (long UsrCod,
                                      const struct Tmt_Column *Column,
                                      unsigned Weekday,unsigned Hour,unsigned Min,
                                      unsigned SecondsPerInterval)
  {
   DB_QueryINSERT ("can not insert hour in user timetable",
		   "INSERT INTO tmt_tutoring"
		   " (UsrCod,Weekday,StartTime,Duration,Info)"
		   " VALUES"
		   " (%ld,%u,'%02u:%02u:00',SEC_TO_TIME(%u),'%s')",
		   UsrCod,
		   Weekday,Hour,Min,
		   Column->DurationIntervals * SecondsPerInterval,
		   Column->Info);
  }

/*****************************************************************************/
/************************ Get timetable from database ************************/
/*****************************************************************************/

unsigned Tmt_DB_GetTimeTable (MYSQL_RES **mysql_res,
                              Tmt_TimeTableType_t Type,long UsrCod)
  {
   switch (Type)
     {
      case Tmt_MY_TIMETABLE:
         switch (Gbl.Crs.Grps.WhichGrps)
           {
            case Grp_MY_GROUPS:
               return (unsigned)
               DB_QuerySELECT (mysql_res,"can not get timetable",
			       "SELECT tmt_courses.Weekday,"				// row[0]
				      "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
				      "TIME_TO_SEC(tmt_courses.Duration) AS D,"		// row[2]
				      "tmt_courses.Info,"				// row[3]
				      "tmt_courses.ClassType,"				// row[4]
				      "tmt_courses.GrpCod,"				// row[5]
				      "tmt_courses.CrsCod"				// row[6]
			        " FROM crs_users,"
			              "tmt_courses"
			       " WHERE crs_users.UsrCod=%ld"
			         " AND tmt_courses.GrpCod=-1"
			         " AND tmt_courses.CrsCod=crs_users.CrsCod"
			       " UNION DISTINCT "
			       "SELECT tmt_courses.Weekday,"				// row[0]
				      "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
				      "TIME_TO_SEC(tmt_courses.Duration) AS D,"		// row[2]
				      "tmt_courses.Info,"				// row[3]
				      "tmt_courses.ClassType,"				// row[4]
				      "tmt_courses.GrpCod,"				// row[5]
				      "tmt_courses.CrsCod"				// row[6]
			        " FROM grp_users,"
				      "tmt_courses"
			       " WHERE grp_users.UsrCod=%ld"
			         " AND grp_users.GrpCod=tmt_courses.GrpCod"
			       " UNION "
			       "SELECT Weekday,"					// row[0]
				      "TIME_TO_SEC(StartTime) AS S,"			// row[1]
				      "TIME_TO_SEC(Duration) AS D,"			// row[2]
				      "Info,"						// row[3]
				      "'tutoring' AS ClassType,"			// row[4]
				      "-1 AS GrpCod,"					// row[5]
				      "-1 AS CrsCod"					// row[6]
			        " FROM tmt_tutoring"
			       " WHERE UsrCod=%ld"
			       " ORDER BY Weekday,"
			                 "S,"
			                 "ClassType,"
			                 "GrpCod,"
			                 "Info,"
			                 "D DESC,"
			                 "CrsCod",
			     UsrCod,
			     UsrCod,
			     UsrCod);
            case Grp_ALL_GROUPS:
               return (unsigned)
               DB_QuerySELECT (mysql_res,"can not get timetable",
			       "SELECT tmt_courses.Weekday,"				// row[0]
				      "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
				      "TIME_TO_SEC(tmt_courses.Duration) AS D,"		// row[2]
				      "tmt_courses.Info,"				// row[3]
				      "tmt_courses.ClassType,"				// row[4]
				      "tmt_courses.GrpCod,"				// row[5]
				      "tmt_courses.CrsCod"				// row[6]
			        " FROM crs_users,"
			              "tmt_courses"
			       " WHERE crs_users.UsrCod=%ld"
			         " AND tmt_courses.CrsCod=crs_users.CrsCod"
			       " UNION "
			       "SELECT Weekday,"					// row[0]
				      "TIME_TO_SEC(StartTime) AS S,"			// row[1]
				      "TIME_TO_SEC(Duration) AS D,"			// row[2]
				      "Info,"						// row[3]
				      "'tutoring' AS ClassType,"			// row[4]
				      "-1 AS GrpCod,"					// row[5]
				      "-1 AS CrsCod"					// row[6]
			        " FROM tmt_tutoring"
			       " WHERE UsrCod=%ld"
			       " ORDER BY Weekday,"
				         "S,"
				         "ClassType,"
				         "GrpCod,"
				         "Info,"
				         "D DESC,"
				         "CrsCod",
			       UsrCod,
			       UsrCod);
           }
         return 0;	// Not reached
      case Tmt_COURSE_TIMETABLE:
         if (Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS ||
             Gbl.Action.Act == ActEdiCrsTT ||
             Gbl.Action.Act == ActChgCrsTT)	// If we are editing, all groups are shown
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get timetable",
			    "SELECT Weekday,"				// row[0]
				   "TIME_TO_SEC(StartTime) AS S,"	// row[1]
				   "TIME_TO_SEC(Duration) AS D,"	// row[2]
				   "Info,"				// row[3]
				   "ClassType,"				// row[4]
				   "GrpCod"				// row[5]
			     " FROM tmt_courses"
			    " WHERE CrsCod=%ld"
			    " ORDER BY Weekday,"
				      "S,"
				      "ClassType,"
				      "GrpCod,"
				      "Info,"
				      "D DESC",
			    Gbl.Hierarchy.Node[HieLvl_CRS].Cod);
         else
            return (unsigned)
            DB_QuerySELECT (mysql_res,"can not get timetable",
			    "SELECT tmt_courses.Weekday,"			// row[0]
				   "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
				   "TIME_TO_SEC(tmt_courses.Duration) AS D,"	// row[2]
				   "tmt_courses.Info,"				// row[3]
				   "tmt_courses.ClassType,"			// row[4]
				   "tmt_courses.GrpCod"				// row[5]
			     " FROM tmt_courses,"
				   "crs_users"
			    " WHERE tmt_courses.CrsCod=%ld"
			      " AND tmt_courses.GrpCod=-1"
			      " AND crs_users.UsrCod=%ld"
			      " AND tmt_courses.CrsCod=crs_users.CrsCod"
			    " UNION DISTINCT "
			    "SELECT tmt_courses.Weekday,"			// row[0]
				   "TIME_TO_SEC(tmt_courses.StartTime) AS S,"	// row[1]
				   "TIME_TO_SEC(tmt_courses.Duration) AS D,"	// row[2]
				   "tmt_courses.Info,"				// row[3]
				   "tmt_courses.ClassType,"			// row[4]
				   "tmt_courses.GrpCod"				// row[5]
			     " FROM tmt_courses,"
				   "grp_users"
			    " WHERE tmt_courses.CrsCod=%ld"
			      " AND grp_users.UsrCod=%ld"
			      " AND tmt_courses.GrpCod=grp_users.GrpCod"
			    " ORDER BY Weekday,"
				      "S,"
				      "ClassType,"
				      "GrpCod,"
				      "Info,"
				      "D DESC",
			    Gbl.Hierarchy.Node[HieLvl_CRS].Cod,UsrCod,
			    Gbl.Hierarchy.Node[HieLvl_CRS].Cod,UsrCod);
      case Tmt_TUTORING_TIMETABLE:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get timetable",
		         "SELECT Weekday,"			// row[0]
			        "TIME_TO_SEC(StartTime) AS S,"	// row[1]
			        "TIME_TO_SEC(Duration) AS D,"	// row[2]
			        "Info"				// row[3]
			  " FROM tmt_tutoring"
		         " WHERE UsrCod=%ld"
		         " ORDER BY Weekday,"
				   "S,"
				   "Info,"
				   "D DESC",
		         UsrCod);
      default:
         return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********** Orphan all groups of a given type in course timetable ************/
/*****************************************************************************/

void Tmt_DB_OrphanAllGrpsOfATypeInCrsTimeTable (long GrpTypCod)
  {
   DB_QueryUPDATE ("can not update all groups of a type in course timetable",
		   "UPDATE tmt_courses"
		     " SET GrpCod=-1"
		   " WHERE GrpCod IN"
			 " (SELECT GrpCod"
			   " FROM grp_groups"
			  " WHERE GrpTypCod=%ld)",
                   GrpTypCod);
  }

/*****************************************************************************/
/********************* Orphan a group in course timetable ********************/
/*****************************************************************************/

void Tmt_DB_OrphanGrpInCrsTimeTable (long GrpCod)
  {
   DB_QueryUPDATE ("can not update a group in course timetable",
		   "UPDATE tmt_courses"
		     " SET GrpCod=-1"
		   " WHERE GrpCod=%ld",
                   GrpCod);
  }

/*****************************************************************************/
/*************************** Remove course timetable *************************/
/*****************************************************************************/

void Tmt_DB_RemoveCrsTimeTable (long CrsCod)
  {
   DB_QueryDELETE ("can not remove timetable",
		   "DELETE FROM tmt_courses"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/**************************** Remove user timetable **************************/
/*****************************************************************************/

void Tmt_DB_RemoveUsrTimeTable (long UsrCod)
  {
   DB_QueryDELETE ("can not remove former timetable",
		   "DELETE FROM tmt_tutoring"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
