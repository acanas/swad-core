// swad_course_database.c: edition of courses operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_course.h"
#include "swad_course_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************* Add a new requested course to pending requests ****************/
/*****************************************************************************/

void Crs_DB_CreateCourse (struct Hie_Node *Crs,Hie_Status_t Status)
  {
   /***** Insert new course into pending requests *****/
   Crs->HieCod =
   DB_QueryINSERTandReturnCode ("can not create a new course",
				"INSERT INTO crs_courses"
				" (DegCod,Year,InsCrsCod,Status,RequesterUsrCod,"
				  "ShortName,FullName)"
				" VALUES"
				" (%ld,%u,'%s',%u,%ld,"
				  "'%s','%s')",
				Crs->PrtCod,
				Crs->Specific.Year,
				Crs->InstitutionalCod,
				(unsigned) Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Crs->ShrtName,
				Crs->FullName);
  }

/*****************************************************************************/
/******************* Get courses of a degree from database *******************/
/*****************************************************************************/

unsigned Crs_DB_GetCrssInDeg (MYSQL_RES **mysql_res,long DegCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get courses of a degree",
		   "SELECT CrsCod"
		    " FROM crs_courses"
		   " WHERE DegCod=%ld",
		   DegCod);
  }

/*****************************************************************************/
/******************** Get courses in current degree *********************/
/*****************************************************************************/

unsigned Crs_DB_GetCrssInCurrentDegBasic (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get courses of a degree",
		   "SELECT CrsCod,"	// row[0]
			  "ShortName"	// row[1]
		    " FROM crs_courses"
		   " WHERE DegCod=%ld"
		   " ORDER BY ShortName",
		   Gbl.Hierarchy.Node[Hie_DEG].HieCod);
  }

/*****************************************************************************/
/******************** Get courses in current degree *********************/
/*****************************************************************************/

unsigned Crs_DB_GetCrssInCurrentDegFull (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get courses of a degree",
		   "SELECT CrsCod,"		// row[0]
			  "DegCod,"		// row[1]
			  "Year,"		// row[2]
			  "InsCrsCod,"		// row[3]
			  "Status,"		// row[4]
			  "RequesterUsrCod,"	// row[5]
			  "ShortName,"		// row[6]
			  "FullName"		// row[7]
		    " FROM crs_courses"
		   " WHERE DegCod=%ld"
		     " AND (Status & %u)=0"
		   " ORDER BY Year,"
			     "ShortName",
		   Gbl.Hierarchy.Node[Hie_DEG].HieCod,
		   (unsigned) Hie_STATUS_BIT_REMOVED);	// All courses except those removed
  }

/*****************************************************************************/
/********************* Get data of a course from its code ********************/
/*****************************************************************************/

unsigned Crs_DB_GetCourseDataByCod (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a course",
		   "SELECT CrsCod,"		// row[0]
			  "DegCod,"		// row[1]
			  "Year,"		// row[2]
			  "InsCrsCod,"		// row[3]
			  "Status,"		// row[4]
			  "RequesterUsrCod,"	// row[5]
			  "ShortName,"		// row[6]
			  "FullName"		// row[7]
		    " FROM crs_courses"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/********************* Get degree code from course code **********************/
/*****************************************************************************/

long Crs_DB_GetCurrentDegCodFromCurrentCrsCod (void)
  {
   return DB_QuerySELECTCode ("can not get the degree of a course",
			      "SELECT DegCod"
			       " FROM crs_courses"
			      " WHERE CrsCod=%ld",
			      Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******* Get the short names of degree and course from a course code *********/
/*****************************************************************************/

void Crs_DB_GetShortNamesByCod (long CrsCod,
                                char CrsShortName[Hie_MAX_BYTES_SHRT_NAME + 1],
                                char DegShortName[Hie_MAX_BYTES_SHRT_NAME + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   DegShortName[0] = CrsShortName[0] = '\0';

   /***** Trivial check: course code should be > 0 *****/
   if (CrsCod <= 0)
      return;

   /***** Get the short name of a degree from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get the short name of a course",
		       "SELECT crs_courses.ShortName,"	// row[0]
			      "deg_degrees.ShortName"	// row[1]
			" FROM crs_courses,"
			      "deg_degrees"
		       " WHERE crs_courses.CrsCod=%ld"
			 " AND crs_courses.DegCod=deg_degrees.DegCod",
		       CrsCod) == 1)
     {
      /***** Get the course short name and degree short name *****/
      row = mysql_fetch_row (mysql_res);
      Str_Copy (CrsShortName,row[0],Hie_MAX_BYTES_SHRT_NAME);
      Str_Copy (DegShortName,row[1],Hie_MAX_BYTES_SHRT_NAME);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Check if course code exists in existing courses **************/
/*****************************************************************************/

bool Crs_DB_CheckIfCrsCodExists (long CrsCod)
  {
   return
   DB_QueryEXISTS ("can not check if a course already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM crs_courses"
		    " WHERE CrsCod=%ld)",
		   CrsCod);
  }

/*****************************************************************************/
/********** Check if the name of course exists in existing courses ***********/
/*****************************************************************************/

bool Crs_DB_CheckIfCrsNameExistsInYearOfDeg (const char *FldName,const char *Name,long CrsCod,
                                             long DegCod,unsigned Year)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a course already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM crs_courses"
		    " WHERE DegCod=%ld"
		      " AND Year=%u"
		      " AND %s='%s'"
		      " AND CrsCod<>%ld)",
		   DegCod,
		   Year,
		   FldName,
		   Name,
		   CrsCod);
  }

/*****************************************************************************/
/************************** Write courses of a user **************************/
/*****************************************************************************/

unsigned Crs_DB_GetCrssOfAUsr (MYSQL_RES **mysql_res,long UsrCod,Rol_Role_t Role)
  {
   char *SubQuery;
   unsigned NumCrss;

   /***** Get courses of a user from database *****/
   if (Role == Rol_UNK)	// Role == Rol_UNK ==> any role
     {
      if (asprintf (&SubQuery,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&SubQuery," AND crs_users.Role=%u",(unsigned) Role) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   NumCrss = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get courses of a user",
		   "SELECT deg_degrees.DegCod,"		// row[0]
			  "crs_courses.CrsCod,"		// row[1]
			  "deg_degrees.ShortName,"	// row[2]
			  "deg_degrees.FullName,"	// row[3]
			  "crs_courses.Year,"		// row[4]
			  "crs_courses.FullName,"	// row[5]
			  "ctr_centers.ShortName,"	// row[6]
			  "crs_users.Accepted"		// row[7]
		    " FROM crs_users,"
		          "crs_courses,"
		          "deg_degrees,"
		          "ctr_centers"
		   " WHERE crs_users.UsrCod=%ld%s"
		     " AND crs_users.CrsCod=crs_courses.CrsCod"
		     " AND crs_courses.DegCod=deg_degrees.DegCod"
		     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		   " ORDER BY deg_degrees.FullName,"
		             "crs_courses.Year,"
		             "crs_courses.FullName",
		   UsrCod,SubQuery);

   /***** Free allocated memory for subquery *****/
   free (SubQuery);

   return NumCrss;
  }

/*****************************************************************************/
/************************** Get old courses from database ********************/
/*****************************************************************************/

unsigned Crs_DB_GetOldCrss (MYSQL_RES **mysql_res,unsigned long SecondsWithoutAccess)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get old courses",
		   "SELECT CrsCod"
		    " FROM crs_last"
		   " WHERE LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)"
		     " AND CrsCod NOT IN"
		         " (SELECT DISTINCT "
		                  "CrsCod"
			    " FROM crs_users)",
		   SecondsWithoutAccess);
  }

/*****************************************************************************/
/************************* Search courses in database ************************/
/*****************************************************************************/
// Returns number of courses found

unsigned Crs_DB_SearchCrss (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get courses",
		   "SELECT deg_degrees.DegCod,"		// row[0]
			  "crs_courses.CrsCod,"		// row[1]
			  "deg_degrees.ShortName,"	// row[2]
			  "deg_degrees.FullName,"	// row[3]
			  "crs_courses.Year,"		// row[4]
			  "crs_courses.FullName,"	// row[5]
			  "ctr_centers.ShortName"	// row[6]
		    " FROM crs_courses,"
			  "deg_degrees,"
			  "ctr_centers,"
			  "ins_instits,"
			  "cty_countrs"
		   " WHERE %s"
		     " AND crs_courses.DegCod=deg_degrees.DegCod"
		     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		     " AND ctr_centers.InsCod=ins_instits.InsCod"
		     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		     "%s"
		   " ORDER BY crs_courses.FullName,"
			     "ins_instits.FullName,"
			     "deg_degrees.FullName,"
			     "crs_courses.Year",
		   SearchQuery,
		   RangeQuery);
  }

/*****************************************************************************/
/********************** Get number of courses in system **********************/
/*****************************************************************************/

unsigned Crs_DB_GetNumCrssInSys (__attribute__((unused)) long SysCod)
  {
   return (unsigned) DB_GetNumRowsTable ("crs_courses");
  }

/*****************************************************************************/
/******************** Get number of courses in a country *********************/
/*****************************************************************************/

unsigned Crs_DB_GetNumCrssInCty (long CtyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in a country",
		  "SELECT COUNT(*)"
		   " FROM ins_instits,"
		         "ctr_centers,"
		         "deg_degrees,"
		         "crs_courses"
		  " WHERE ins_instits.CtyCod=%ld"
		    " AND ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  CtyCod);
  }

/*****************************************************************************/
/****************** Get number of courses in an institution ******************/
/*****************************************************************************/

unsigned Crs_DB_GetNumCrssInIns (long InsCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in an institution",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers,"
		         "deg_degrees,"
		         "crs_courses"
		  " WHERE ctr_centers.InsCod=%ld"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  InsCod);
  }

/*****************************************************************************/
/********************* Get number of courses in a center *********************/
/*****************************************************************************/

unsigned Crs_DB_GetNumCrssInCtr (long CtrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in a center",
		  "SELECT COUNT(*)"
		   " FROM deg_degrees,"
		         "crs_courses"
		  " WHERE deg_degrees.CtrCod=%ld"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  CtrCod);
  }

/*****************************************************************************/
/********************* Get number of courses in a degree *********************/
/*****************************************************************************/

unsigned Crs_DB_GetNumCrssInDeg (long DegCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of courses in a degree",
		  "SELECT COUNT(*)"
		   " FROM crs_courses"
		  " WHERE DegCod=%ld",
		  DegCod);
  }

/*****************************************************************************/
/******************** Get number of courses with users ***********************/
/*****************************************************************************/

unsigned Crs_DB_GetNumCrssWithUsrs (Rol_Role_t Role,
                                    Hie_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of courses with users",
		  "SELECT COUNT(DISTINCT crs_courses.CrsCod)"
		   " FROM ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses,"
			 "crs_users"
		  " WHERE %s"
			 "ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod"
		    " AND crs_courses.CrsCod=crs_users.CrsCod"
		    " AND crs_users.Role=%u",
		  SubQuery,
		  (unsigned) Role);
  }

/*****************************************************************************/
/**************** Get all courses of a user from database ********************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned Crs_DB_GetCrssFromUsr (MYSQL_RES **mysql_res,long UsrCod,long PrtCod)
  {
   if (PrtCod > 0)	// Courses in a degree
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not get the courses a user belongs to",
		      "SELECT crs_users.CrsCod,"	// row[0]
			     "crs_users.Role,"		// row[1]
			     "crs_courses.DegCod"	// row[2]
		       " FROM crs_users,"
			     "crs_courses"
		      " WHERE crs_users.UsrCod=%ld"
		        " AND crs_users.CrsCod=crs_courses.CrsCod"
		        " AND crs_courses.DegCod=%ld"
		      " ORDER BY crs_courses.ShortName",
		      UsrCod,
		      PrtCod);
   else			// All the courses
      return (unsigned)
      DB_QuerySELECT (mysql_res,"can not get the courses a user belongs to",
		      "SELECT crs_users.CrsCod,"	// row[0]
			     "crs_users.Role,"		// row[1]
			     "crs_courses.DegCod"	// row[2]
		       " FROM crs_users,"
			     "crs_courses,"
			     "deg_degrees"
		      " WHERE crs_users.UsrCod=%ld"
		        " AND crs_users.CrsCod=crs_courses.CrsCod"
		        " AND crs_courses.DegCod=deg_degrees.DegCod"
		      " ORDER BY deg_degrees.ShortName,"
			        "crs_courses.ShortName",
		      UsrCod);
  }

/*****************************************************************************/
/************* Change the institutional course code of a course **************/
/*****************************************************************************/

void Crs_DB_UpdateInstitutionalCrsCod (long CrsCod,const char *NewInstitutionalCrsCod)
  {
   DB_QueryUPDATE ("can not update the institutional code of a course",
		   "UPDATE crs_courses"
		     " SET InsCrsCod='%s'"
		   " WHERE CrsCod=%ld",
                   NewInstitutionalCrsCod,
                   CrsCod);
  }

/*****************************************************************************/
/****************** Change the year/semester of a course *********************/
/*****************************************************************************/

void Crs_DB_UpdateCrsYear (long CrsCod,unsigned NewYear)
  {
   DB_QueryUPDATE ("can not update the year of a course",
		   "UPDATE crs_courses"
		     " SET Year=%u"
		   " WHERE CrsCod=%ld",
	           NewYear,
	           CrsCod);
  }

/*****************************************************************************/
/***************** Update course name in table of courses ********************/
/*****************************************************************************/

void Crs_DB_UpdateCrsName (long CrsCod,const char *FldName,const char *NewCrsName)
  {
   DB_QueryUPDATE ("can not update the name of a course",
		   "UPDATE crs_courses"
		     " SET %s='%s'"
		   " WHERE CrsCod=%ld",
	           FldName,NewCrsName,
	           CrsCod);
  }

/*****************************************************************************/
/********************** Update degree in table of courses ********************/
/*****************************************************************************/

void Crs_DB_UpdateCrsDeg (long CrsCod,long DegCod)
  {
   DB_QueryUPDATE ("can not move course to another degree",
		   "UPDATE crs_courses"
		     " SET DegCod=%ld"
		   " WHERE CrsCod=%ld",
	           DegCod,
	           CrsCod);
  }

/*****************************************************************************/
/********************** Change the status of a course ************************/
/*****************************************************************************/

void Crs_DB_UpdateCrsStatus (long CrsCod,Hie_Status_t Status)
  {
   DB_QueryUPDATE ("can not update the status of a course",
		   "UPDATE crs_courses"
		     " SET Status=%u"
		   " WHERE CrsCod=%ld",
                   (unsigned) Status,
                   CrsCod);
  }

/*****************************************************************************/
/***************** Update my last click in current course ********************/
/*****************************************************************************/

void Crs_DB_UpdateCrsLastClick (void)
  {
   /***** Update last access to current course *****/
   DB_QueryREPLACE ("can not update last access to current course",
		    "REPLACE INTO crs_last"
		    " (CrsCod,LastTime)"
		    " VALUES"
		    " (%ld,NOW())",
		    Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************** Remove information about a course **********************/
/*****************************************************************************/

void Crs_DB_RemoveCrsInfo (long CrsCod)
  {
   /* Remove information source of the course */
   DB_QueryDELETE ("can not remove info sources of a course",
		   "DELETE FROM crs_info_src"
		   " WHERE CrsCod=%ld",
		   CrsCod);

   /* Remove information text of the course */
   DB_QueryDELETE ("can not remove info of a course",
		   "DELETE FROM crs_info_txt"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/********************** Remove timetable of a course *************************/
/*****************************************************************************/

void Crs_DB_RemoveCrsTimetable (long CrsCod)
  {
   DB_QueryDELETE ("can not remove the timetable of a course",
		   "DELETE FROM tmt_courses"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/***** Remove course from table of last accesses to courses in database ******/
/*****************************************************************************/

void Crs_DB_RemoveCrsLast (long CrsCod)
  {
   DB_QueryDELETE ("can not remove a course",
		   "DELETE FROM crs_last"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/************* Remove course from table of courses in database ***************/
/*****************************************************************************/

void Crs_DB_RemoveCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove a course",
		   "DELETE FROM crs_courses"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }
