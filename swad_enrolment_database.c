// swad_enrolment_database.h: enrolment (registration) or removing of users, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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
/******************** Create temporary table with my courses *****************/
/*****************************************************************************/

void Enr_DB_CreateTmpTableMyCourses (void)
  {
   DB_CreateTmpTable ("CREATE TEMPORARY TABLE IF NOT EXISTS my_courses_tmp"
		      "(CrsInd INT NOT NULL AUTO_INCREMENT,"
		       "CrsCod INT NOT NULL,"
			 "Role TINYINT NOT NULL,"
		       "DegCod INT NOT NULL,"
	               "UNIQUE INDEX(CrsInd),"
		       "UNIQUE INDEX(CrsCod,Role,DegCod)) ENGINE=MEMORY"
		      " SELECT crs_users.CrsCod,"
			      "crs_users.Role,"
			      "crs_courses.DegCod"
		        " FROM crs_users,"
			      "crs_courses,"
			      "deg_degrees"
		       " WHERE crs_users.UsrCod=%ld"
		         " AND crs_users.CrsCod=crs_courses.CrsCod"
		         " AND crs_courses.DegCod=deg_degrees.DegCod"
		    " ORDER BY deg_degrees.ShortName,"
			      "crs_courses.ShortName",
		      Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************* Get my courses from database **********************/
/*****************************************************************************/

unsigned Enr_DB_GetMyCrss (MYSQL_RES **mysql_res,long PrtCod)
  {
   if (PrtCod > 0)
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not get which courses you belong to",
		      "SELECT CrsCod,"	// row[0]
			     "Role,"	// row[1]
			     "DegCod"	// row[2]
		       " FROM my_courses_tmp"
		      " WHERE DegCod=%ld"
		   " ORDER BY CrsInd",
		      PrtCod);
   else
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not get which courses you belong to",
		      "SELECT CrsCod,"	// row[0]
			     "Role,"	// row[1]
			     "DegCod"	// row[2]
		      " FROM my_courses_tmp"
		  " ORDER BY CrsInd");
  }

/*****************************************************************************/
/********************* Drop temporary table with my courses ******************/
/*****************************************************************************/

void Enr_DB_DropTmpTableMyCourses (void)
  {
   DB_DropTmpTable ("my_courses_tmp");
  }

/*****************************************************************************/
/************************* Get my courses from database **********************/
/*****************************************************************************/

unsigned Enr_DB_GetMyCoursesNames (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's courses",
		   "SELECT crs_courses.CrsCod,"	// row[0]
			  "crs_courses.ShortName,"	// row[1]
			  "crs_courses.FullName,"	// row[2]
			  "crs_users.Role"		// row[3]
		    " FROM crs_users,"
			  "crs_courses"
		   " WHERE crs_users.UsrCod=%ld"
		     " AND crs_users.CrsCod=crs_courses.CrsCod"
		" ORDER BY crs_courses.FullName",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Check if a user belongs to a course ********************/
/*****************************************************************************/

Usr_Belong_t Enr_DB_CheckIfUsrBelongsToCrs (long UsrCod,long HieCod,
					    bool CountOnlyAcceptedCourses)
  {
   const char *SubQuery = (CountOnlyAcceptedCourses ? " AND crs_users.Accepted='Y'" :	// Only if user accepted
	                                              "");

   return
   DB_QueryEXISTS ("can not check if a user belongs to a course",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM crs_users"
		    " WHERE CrsCod=%ld"
		      " AND UsrCod=%ld"
			"%s)",
		   HieCod,
		   UsrCod,
		   SubQuery) ? Usr_BELONG :
			       Usr_DONT_BELONG;
  }

/*****************************************************************************/
/*************** Check if a user belongs to any of my courses ****************/
/*****************************************************************************/

bool Enr_DB_CheckIfUsrSharesAnyOfMyCrs (long UsrCod)
  {
   /***** Fill the list with the courses I belong to (if not already filled) *****/
   Hie_GetMyHierarchy (Hie_CRS);

   /***** Check if user shares any course with me *****/
   return
   DB_QueryEXISTS ("can not check if a user shares any course with you",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM crs_users"
		    " WHERE UsrCod=%ld"
		      " AND CrsCod IN"
			  " (SELECT CrsCod"
			     " FROM my_courses_tmp))",
		   UsrCod);
  }

/*****************************************************************************/
/*** Check if a user belongs to any of my courses but has a different role ***/
/*****************************************************************************/

bool Enr_DB_CheckIfUsrSharesAnyOfMyCrsWithDifferentRole (long UsrCod)
  {
   bool UsrSharesAnyOfMyCrsWithDifferentRole;

   /***** 1. Fast check: Am I logged? *****/
   if (!Gbl.Usrs.Me.Logged)
      return false;

   /***** 2. Slow check: Get if user shares any course with me
                         with a different role, from database *****/
   /* Fill the list with the courses I belong to (if not already filled) */
   Hie_GetMyHierarchy (Hie_CRS);

   /* Remove temporary table if exists */
   DB_DropTmpTable ("usr_courses_tmp");

   /* Create temporary table with all user's courses for a role */
   DB_CreateTmpTable ("CREATE TEMPORARY TABLE IF NOT EXISTS usr_courses_tmp "
		      "(CrsCod INT NOT NULL,Role TINYINT NOT NULL,"
		      "UNIQUE INDEX(CrsCod,Role)) ENGINE=MEMORY"
		      " SELECT CrsCod,"
			      "Role"
		        " FROM crs_users"
		       " WHERE UsrCod=%ld",
		      UsrCod);

   /* Get if a user shares any course with me from database */
   UsrSharesAnyOfMyCrsWithDifferentRole =
   DB_QueryEXISTS ("can not check if a user shares any course with you",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM my_courses_tmp,"
			   "usr_courses_tmp"
		    " WHERE my_courses_tmp.CrsCod=usr_courses_tmp.CrsCod"
		      " AND my_courses_tmp.Role<>usr_courses_tmp.Role)");

   /* Remove temporary table if exists */
   DB_DropTmpTable ("usr_courses_tmp");

   return UsrSharesAnyOfMyCrsWithDifferentRole;
  }

/*****************************************************************************/
/******** Get the user's code of a random student from current course ********/
/*****************************************************************************/
// Returns user's code or -1 if no user found

long Enr_DB_GetRamdomStdFromCrs (long HieCod)
  {
   return DB_QuerySELECTCode ("can not get a random student from a course",
			      "SELECT UsrCod"
			       " FROM crs_users"
			      " WHERE CrsCod=%ld"
			        " AND Role=%u"
			   " ORDER BY RAND()"
			      " LIMIT 1",
			      HieCod,
			      (unsigned) Rol_STD);
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
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
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
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
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
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Rol_TCH);
  }

/*****************************************************************************/
/********************* Get number of courses of a user ***********************/
/*****************************************************************************/

unsigned Enr_DB_GetNumCrssOfUsr (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses of a user",
		  "SELECT COUNT(*)"
		   " FROM crs_users"
		  " WHERE UsrCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/*************** Get number of courses of a user not accepted ****************/
/*****************************************************************************/

unsigned Enr_DB_GetNumCrssOfUsrNotAccepted (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses of a user",
		  "SELECT COUNT(*)"
		   " FROM crs_users"
		  " WHERE UsrCod=%ld"
		    " AND Accepted='N'",
		  UsrCod);
  }

/*****************************************************************************/
/********* Get number of courses in with a user have a given role ************/
/*****************************************************************************/

unsigned Enr_DB_GetNumCrssOfUsrWithARole (long UsrCod,Rol_Role_t Role)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses of a user with a role",
		  "SELECT COUNT(*)"
		   " FROM crs_users"
		  " WHERE UsrCod=%ld"
		    " AND Role=%u",
		  UsrCod,
		  (unsigned) Role);
  }

/*****************************************************************************/
/********* Get number of courses in with a user have a given role ************/
/*****************************************************************************/

unsigned Enr_DB_GetNumCrssOfUsrWithARoleNotAccepted (long UsrCod,Rol_Role_t Role)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses of a user with a role",
		  "SELECT COUNT(*)"
		   " FROM crs_users"
		  " WHERE UsrCod=%ld"
		    " AND Role=%u"
		    " AND Accepted='N'",
		  UsrCod,
		  (unsigned) Role);
  }

/*****************************************************************************/
/****** Get number of users with some given roles in courses of a user *******/
/*****************************************************************************/

#define Enr_DB_MAX_BYTES_ROLES_STR (Rol_NUM_ROLES * (Cns_MAX_DIGITS_UINT + 1))
unsigned Enr_DB_GetNumUsrsInCrssOfAUsr (long UsrCod,Rol_Role_t UsrRole,
                                        unsigned OthersRoles)
  {
   Rol_Role_t Role;
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];
   char OthersRolesStr[Enr_DB_MAX_BYTES_ROLES_STR + 1];
   char SubQueryRole[64];
   unsigned NumUsrs;
   // This query can be made in a unique, but slower, query
   // The temporary table achieves speedup from ~2s to few ms

   /***** Remove temporary table if exists *****/
   DB_DropTmpTable ("usr_courses_tmp");

   /***** Create temporary table with all user's courses
          as student/non-editing teacher/teacher *****/
   switch (UsrRole)
     {
      case Rol_STD:	// Student
      case Rol_NET:	// Non-editing teacher
      case Rol_TCH:	// Teacher
	 sprintf (SubQueryRole," AND Role=%u",
	          (unsigned) UsrRole);
	 break;
      default:
	 SubQueryRole[0] = '\0';
	 Err_WrongRoleExit ();
	 break;
     }
   DB_CreateTmpTable ("CREATE TEMPORARY TABLE IF NOT EXISTS usr_courses_tmp"
		      " (CrsCod INT NOT NULL,UNIQUE INDEX (CrsCod))"
		      " ENGINE=MEMORY"
		      " SELECT CrsCod"
		        " FROM crs_users"
		       " WHERE UsrCod=%ld"
			   "%s",
		      UsrCod,
		      SubQueryRole);

   /***** Get the number of students/teachers in a course from database ******/
   OthersRolesStr[0] = '\0';
   for (Role =  Rol_STD;	// First possible role in a course
	Role <= Rol_TCH;	// Last possible role in a course
	Role++)
      if ((OthersRoles & (1 << Role)))
        {
         sprintf (UnsignedStr,"%u",(unsigned) Role);
         if (OthersRolesStr[0])	// Not empty
	    Str_Concat (OthersRolesStr,",",sizeof (OthersRolesStr) - 1);
	 Str_Concat (OthersRolesStr,UnsignedStr,sizeof (OthersRolesStr) - 1);
        }
   NumUsrs = (unsigned)
   DB_QueryCOUNT ("can not get number of users",
		  "SELECT COUNT(DISTINCT crs_users.UsrCod)"
		   " FROM crs_users,"
			 "usr_courses_tmp"
		  " WHERE crs_users.CrsCod=usr_courses_tmp.CrsCod"
		    " AND crs_users.Role IN (%s)",
		  OthersRolesStr);

   /***** Remove temporary table *****/
   DB_DropTmpTable ("usr_courses_tmp");

   return NumUsrs;
  }

/*****************************************************************************/
/******* Get total number of users of one or several roles in courses ********/
/*****************************************************************************/

#define Enr_DB_MAX_BYTES_SUBQUERY_ROLES (Rol_NUM_ROLES * (10 + 1) - 1)

unsigned Enr_DB_GetNumUsrsInCrss (Hie_Level_t Level,long Cod,unsigned Roles,
                                  bool AnyUserInCourses)
  {
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];
   char SubQueryRoles[Enr_DB_MAX_BYTES_SUBQUERY_ROLES + 1];
   bool FirstRole;
   bool MoreThanOneRole;
   Rol_Role_t Role;
   Rol_Role_t FirstRoleRequested;

   /***** Get first role requested *****/
   FirstRoleRequested = Rol_UNK;
   for (Role  = Rol_STD;
        Role <= Rol_TCH;
        Role++)
      if (Roles & (1 << Role))
	{
	 FirstRoleRequested = Role;
	 break;
	}

   /***** Check if more than one role is requested *****/
   MoreThanOneRole = false;
   if (FirstRoleRequested != Rol_UNK)
      for (Role = FirstRoleRequested + 1;
	   Role <= Rol_TCH;
	   Role++)
	 if (Roles & (1 << Role))
	   {
	    MoreThanOneRole = true;
	    break;
	   }

   /***** Build subquery for roles *****/
   if (MoreThanOneRole)
     {
      Str_Copy (SubQueryRoles," IN (",sizeof (SubQueryRoles) - 1);
      for (Role  = Rol_STD, FirstRole = true;
	   Role <= Rol_TCH;
	   Role++)
	 if (Roles & (1 << Role))
	   {
	    snprintf (UnsignedStr,sizeof (UnsignedStr),"%u",(unsigned) Role);
	    if (FirstRole)	// Not the first role
	       FirstRole = false;
	    else
	       Str_Concat (SubQueryRoles,",",sizeof (SubQueryRoles) - 1);
	    Str_Concat (SubQueryRoles,UnsignedStr,sizeof (SubQueryRoles) - 1);
	   }
      Str_Concat (SubQueryRoles,")",sizeof (SubQueryRoles) - 1);
     }
   else	// Only one role
      sprintf (SubQueryRoles,"=%u",FirstRoleRequested);

   /***** Get number of users from database *****/
   switch (Level)
     {
      case Hie_SYS:
         if (AnyUserInCourses)	// Any user
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT UsrCod)"
			    " FROM crs_users");
         else
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT UsrCod)"
			    " FROM crs_users"
			   " WHERE Role"
			       "%s",
			   SubQueryRoles);
      case Hie_CTY:
         if (AnyUserInCourses)	// Any user
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM ins_instits,"
				  "ctr_centers,"
				  "deg_degrees,"
				  "crs_courses,"
				  "crs_users"
			   " WHERE ins_instits.CtyCod=%ld"
			     " AND ins_instits.InsCod=ctr_centers.InsCod"
			     " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			     " AND deg_degrees.DegCod=crs_courses.DegCod"
			     " AND crs_courses.CrsCod=crs_users.CrsCod",
			   Cod);
         else
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM ins_instits,"
				  "ctr_centers,"
				  "deg_degrees,"
				  "crs_courses,"
				  "crs_users"
			   " WHERE ins_instits.CtyCod=%ld"
			     " AND ins_instits.InsCod=ctr_centers.InsCod"
			     " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			     " AND deg_degrees.DegCod=crs_courses.DegCod"
			     " AND crs_courses.CrsCod=crs_users.CrsCod"
			     " AND crs_users.Role"
			       "%s",
			   Cod,
			   SubQueryRoles);
      case Hie_INS:
         if (AnyUserInCourses)	// Any user
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM ctr_centers,"
				  "deg_degrees,"
				  "crs_courses,"
				  "crs_users"
			   " WHERE ctr_centers.InsCod=%ld"
			     " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			     " AND deg_degrees.DegCod=crs_courses.DegCod"
			     " AND crs_courses.CrsCod=crs_users.CrsCod",
			   Cod);
         else
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM ctr_centers,"
				  "deg_degrees,"
				  "crs_courses,"
				  "crs_users"
			   " WHERE ctr_centers.InsCod=%ld"
			     " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			     " AND deg_degrees.DegCod=crs_courses.DegCod"
			     " AND crs_courses.CrsCod=crs_users.CrsCod"
			     " AND crs_users.Role"
			       "%s",
			   Cod,
			   SubQueryRoles);
      case Hie_CTR:
         if (AnyUserInCourses)	// Any user
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM deg_degrees,"
				  "crs_courses,"
				  "crs_users"
			   " WHERE deg_degrees.CtrCod=%ld"
			     " AND deg_degrees.DegCod=crs_courses.DegCod"
			     " AND crs_courses.CrsCod=crs_users.CrsCod",
			   Cod);
         else
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM deg_degrees,"
				  "crs_courses,"
				  "crs_users"
			   " WHERE deg_degrees.CtrCod=%ld"
			     " AND deg_degrees.DegCod=crs_courses.DegCod"
			     " AND crs_courses.CrsCod=crs_users.CrsCod"
			     " AND crs_users.Role"
			       "%s",
			   Cod,
			   SubQueryRoles);
      case Hie_DEG:
         if (AnyUserInCourses)	// Any user
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM crs_courses,"
			  	  "crs_users"
			   " WHERE crs_courses.DegCod=%ld"
			     " AND crs_courses.CrsCod=crs_users.CrsCod",
			   Cod);
         else
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT crs_users.UsrCod)"
			    " FROM crs_courses,"
				  "crs_users"
			   " WHERE crs_courses.DegCod=%ld"
			     " AND crs_courses.CrsCod=crs_users.CrsCod"
			     " AND crs_users.Role"
			       "%s",
			  Cod,
			  SubQueryRoles);
      case Hie_CRS:
         if (AnyUserInCourses)	// Any user
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT UsrCod)"
			    " FROM crs_users"
			   " WHERE CrsCod=%ld",
			   Cod);
         else
            return (unsigned)
            DB_QueryCOUNT ("can not get number of users",
			   "SELECT COUNT(DISTINCT UsrCod)"
			    " FROM crs_users"
			   " WHERE CrsCod=%ld"
			     " AND Role"
			       "%s",
			   Cod,
			   SubQueryRoles);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******** Get total number of users who do not belong to any course **********/
/*****************************************************************************/

unsigned Enr_DB_GetNumUsrsNotBelongingToAnyCrs (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users"
		  " who do not belong to any course",
		  "SELECT COUNT(*)"
		   " FROM usr_data"
		  " WHERE UsrCod NOT IN"
			" (SELECT DISTINCT "
			         "UsrCod"
			   " FROM crs_users)");
  }

/*****************************************************************************/
/************ Get average number of courses with users of a type *************/
/*****************************************************************************/

double Enr_DB_GetAverageNumUsrsPerCrs (Hie_Level_t Level,long Cod,Rol_Role_t Role)
  {
   switch (Level)
     {
      case Hie_SYS:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(UsrCod) AS NumUsrs"
						  " FROM crs_users"
					      " GROUP BY CrsCod) AS NumUsrsTable");
	 else
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(UsrCod) AS NumUsrs"
						  " FROM crs_users"
						 " WHERE Role=%u"
					      " GROUP BY CrsCod) AS NumUsrsTable",
					 (unsigned) Role);
      case Hie_CTY:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM ins_instits,"
						        "ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE ins_instits.CtyCod=%ld"
						   " AND ins_instits.InsCod=ctr_centers.InsCod"
						   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM ins_instits,"
						        "ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE ins_instits.CtyCod=%ld"
						   " AND ins_instits.InsCod=ctr_centers.InsCod"
						   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
						   " AND crs_users.Role=%u"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod,
					 (unsigned) Role);
      case Hie_INS:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE ctr_centers.InsCod=%ld"
						   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE ctr_centers.InsCod=%ld"
						   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
						   " AND crs_users.Role=%u"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod,
					 (unsigned) Role);
      case Hie_CTR:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE deg_degrees.CtrCod=%ld"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE deg_degrees.CtrCod=%ld"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
						   " AND crs_users.Role=%u"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod,
					 (unsigned) Role);
      case Hie_DEG:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM crs_courses,"
						        "crs_users"
						 " WHERE crs_courses.DegCod=%ld"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of users per course",
					 "SELECT AVG(NumUsrs)"
					  " FROM (SELECT COUNT(crs_users.UsrCod) AS NumUsrs"
						  " FROM crs_courses,"
						        "crs_users"
						 " WHERE crs_courses.DegCod=%ld"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
						   " AND crs_users.Role=%u"
					      " GROUP BY crs_users.CrsCod) AS NumUsrsTable",
					 Cod,
					 (unsigned) Role);
      case Hie_CRS:
	 return (double) Enr_GetNumUsrsInCrss (Hie_CRS,Cod,
				               Role == Rol_UNK ? 1 << Rol_STD |
							         1 << Rol_NET |
							         1 << Rol_TCH :	// Any user
							         1 << Role);

      default:
         Err_WrongHierarchyLevelExit ();
         return 0.0;	// Not reached
     }
  }

/*****************************************************************************/
/************ Get average number of courses with users of a role *************/
/*****************************************************************************/

double Enr_DB_GetAverageNumCrssPerUsr (Hie_Level_t Level,long Cod,Rol_Role_t Role)
  {
   switch (Level)
     {
      case Hie_SYS:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(CrsCod) AS NumCrss"
						  " FROM crs_users"
					      " GROUP BY UsrCod) AS NumCrssTable");
	 else
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(CrsCod) AS NumCrss"
						  " FROM crs_users"
						 " WHERE Role=%u"
					      " GROUP BY UsrCod) AS NumCrssTable",
					 (unsigned) Role);
      case Hie_CTY:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM ins_instits,"
						        "ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE ins_instits.CtyCod=%ld"
						   " AND ins_instits.InsCod=ctr_centers.InsCod"
						   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM ins_instits,"
						        "ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE ins_instits.CtyCod=%ld"
						   " AND ins_instits.InsCod=ctr_centers.InsCod"
						   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
						   " AND crs_users.Role=%u"
					      " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod,
					 (unsigned) Role);
      case Hie_INS:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE ctr_centers.InsCod=%ld"
						   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM ctr_centers,"
						        "deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						  " WHERE ctr_centers.InsCod=%ld"
						    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						    " AND deg_degrees.DegCod=crs_courses.DegCod"
						    " AND crs_courses.CrsCod=crs_users.CrsCod"
						    " AND crs_users.Role=%u"
					       " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod,
					 (unsigned) Role);
      case Hie_CTR:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE deg_degrees.CtrCod=%ld"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM deg_degrees,"
						        "crs_courses,"
						        "crs_users"
						 " WHERE deg_degrees.CtrCod=%ld"
						   " AND deg_degrees.DegCod=crs_courses.DegCod"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
						   " AND crs_users.Role=%u"
					      " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod,
					 (unsigned) Role);
      case Hie_DEG:
	 if (Role == Rol_UNK)	// Any user
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM crs_courses,"
						        "crs_users"
						 " WHERE crs_courses.DegCod=%ld"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
					      " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod);
	 else
	    return DB_QuerySELECTDouble ("can not get number of courses per user",
					 "SELECT AVG(NumCrss)"
					  " FROM (SELECT COUNT(crs_users.CrsCod) AS NumCrss"
						  " FROM crs_courses,"
						        "crs_users"
						 " WHERE crs_courses.DegCod=%ld"
						   " AND crs_courses.CrsCod=crs_users.CrsCod"
						   " AND crs_users.Role=%u"
					      " GROUP BY crs_users.UsrCod) AS NumCrssTable",
					 Cod,
					 (unsigned) Role);
      case Hie_CRS:
         return 1.0;
      default:
         Err_WrongHierarchyLevelExit ();
         return 0.0;	// Not reached
     }
  }

/*****************************************************************************/
/************************** Remove user from course **************************/
/*****************************************************************************/

void Enr_DB_RemUsrFromCrs (long UsrCod,long HieCod)
  {
   DB_QueryDELETE ("can not remove a user from a course",
		   "DELETE FROM crs_users"
		   " WHERE CrsCod=%ld"
		     " AND UsrCod=%ld",
		   HieCod,
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

void Enr_DB_RemAllUsrsFromCrs (long HieCod)
  {
   DB_QueryDELETE ("can not remove users from a course",
		   "DELETE FROM crs_users"
		   " WHERE CrsCod=%ld",
		   HieCod);
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
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
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
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********* Set a user's acceptation to true in the current course ************/
/*****************************************************************************/

unsigned Enr_DB_GetEnrolmentRequests (MYSQL_RES **mysql_res,unsigned RolesSelected)
  {
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:                // Show requesters for the whole platform
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
			       Gbl.Usrs.Me.UsrDat.UsrCod,Hie_GetDBStrFromLevel (Hie_DEG),
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
			       Gbl.Usrs.Me.UsrDat.UsrCod,Hie_GetDBStrFromLevel (Hie_CTR),
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
			       Gbl.Usrs.Me.UsrDat.UsrCod,Hie_GetDBStrFromLevel (Hie_INS),
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
      case Hie_CTY:                // Show requesters for the current country
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
			       Gbl.Hierarchy.Node[Hie_CTY].HieCod,
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
			       Hie_GetDBStrFromLevel (Hie_DEG),
			       Gbl.Hierarchy.Node[Hie_CTY].HieCod,
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
			       Hie_GetDBStrFromLevel (Hie_CTR),
			       Gbl.Hierarchy.Node[Hie_CTY].HieCod,
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
			       Hie_GetDBStrFromLevel (Hie_INS),
			       Gbl.Hierarchy.Node[Hie_CTY].HieCod,
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
			       Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case Hie_INS:                // Show requesters for the current institution
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
			       Gbl.Hierarchy.Node[Hie_INS].HieCod,
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
			       Hie_GetDBStrFromLevel (Hie_DEG),
			       Gbl.Hierarchy.Node[Hie_INS].HieCod,
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
			       Gbl.Usrs.Me.UsrDat.UsrCod,Hie_GetDBStrFromLevel (Hie_CTR),
			       Gbl.Hierarchy.Node[Hie_INS].HieCod,
			       RolesSelected);
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all requesters from this institution
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
			       Gbl.Hierarchy.Node[Hie_INS].HieCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case Hie_CTR:                // Show requesters for the current center
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
			       Gbl.Hierarchy.Node[Hie_CTR].HieCod,
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
			       Hie_GetDBStrFromLevel (Hie_DEG),
			       Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			       RolesSelected);
	    case Rol_CTR_ADM:	// If I am logged as admin of this center     , I can view all requesters from this center
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all requesters from this center
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
			       Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case Hie_DEG:        // Show requesters for the current degree
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
			       Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			       RolesSelected);
	    case Rol_DEG_ADM:	// If I am logged as admin of this degree     , I can view all requesters from this degree
	    case Rol_CTR_ADM:	// If I am logged as admin of this center     , I can view all requesters from this degree
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all requesters from this degree
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
			       Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      case Hie_CRS:        // Show requesters for the current course
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:	// If I am logged as teacher of this course   , I can view all requesters from this course
	    case Rol_DEG_ADM:	// If I am logged as admin of this degree     , I can view all requesters from this course
	    case Rol_CTR_ADM:	// If I am logged as admin of this center     , I can view all requesters from this course
	    case Rol_INS_ADM:	// If I am logged as admin of this institution, I can view all requesters from this course
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
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			       RolesSelected);
	    default:
	       Err_NoPermissionExit ();
	       return 0;	// Not reached
	   }
	 return 0;		// Not reached
      default:
	 Err_WrongHierarchyLevelExit ();
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

long Enr_DB_GetUsrEnrolmentRequestInCrs (long UsrCod,long HieCod)
  {
   return
   DB_QuerySELECTCode ("can not get enrolment request",
		       "SELECT ReqCod"
			" FROM crs_requests"
		       " WHERE CrsCod=%ld"
			 " AND UsrCod=%ld",
		       HieCod,
		       UsrCod);
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

void Enr_DB_RemCrsRequests (long HieCod)
  {
   DB_QueryDELETE ("can not remove requests for inscription to a course",
		   "DELETE FROM crs_requests"
		   " WHERE CrsCod=%ld",
		   HieCod);
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
