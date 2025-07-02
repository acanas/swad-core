// swad_follow_database.c: user's followers and followed operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include "swad_error.h"
#include "swad_follow.h"
#include "swad_follow_database.h"
#include "swad_global.h"
#include "swad_hierarchy_type.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *FieldDB[2] =
  {
   "FollowedCod",
   "FollowerCod"
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Get users to follow *****************************/
/*****************************************************************************/

unsigned Fol_DB_GetUsrsToFollow (unsigned MaxUsrsToShow,
				 Fol_WhichUsersSuggestToFollowThem_t WhichUsersSuggestToFollowThem,
				 MYSQL_RES **mysql_res)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   char SubQuery1[256];
   char SubQuery2[256];
   char SubQuery3[256];
   char SubQuery4[256];

   /***** Build subqueries related to photos *****/
   switch (WhichUsersSuggestToFollowThem)
     {
      case Fol_SUGGEST_ONLY_USERS_WITH_PHOTO:
	 // Photo visibility should be >= profile visibility in every subquery
	 sprintf (SubQuery1,		// 1. Users followed by my followed
		  " AND usr_data.PhotoVisibility IN ('%s','%s')"
		  " AND usr_data.Photo<>''",
		  Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
		  Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
	 sprintf (SubQuery2,		// 2. Users who share any course with me
		  " AND usr_data.PhotoVisibility IN ('%s','%s','%s')"
		  " AND usr_data.Photo<>''",
		  Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
		  Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
		  Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
	 sprintf (SubQuery3,		// 3. Users who share any course with me with another role
		  " AND usr_data.PhotoVisibility IN ('%s','%s','%s','%s')"
		  " AND usr_data.Photo<>''",
		  Pri_VisibilityDB[Pri_VISIBILITY_USER  ],
		  Pri_VisibilityDB[Pri_VISIBILITY_COURSE],
		  Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
		  Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
	 sprintf (SubQuery4,		// 4. Add some likely unknown random users
		  " AND usr_data.PhotoVisibility IN ('%s','%s')"
		  " AND usr_data.Photo<>''",
		  Pri_VisibilityDB[Pri_VISIBILITY_SYSTEM],
		  Pri_VisibilityDB[Pri_VISIBILITY_WORLD ]);
	 break;
      case Fol_SUGGEST_ANY_USER:
	 SubQuery1[0] = '\0';
	 SubQuery2[0] = '\0';
	 SubQuery3[0] = '\0';
	 SubQuery4[0] = '\0';
	 break;
     }

   /***** Build query to get users to follow *****/
   // Get only users with surname 1 and first name
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users to follow",
		   "SELECT DISTINCT "
		          "UsrCod"
		    " FROM"
		   " ("
		   /***** Likely known users *****/
		   "(SELECT DISTINCT "
		           "UsrCod"
		     " FROM"
		   " ("
		   // 1. Users followed by my followed
		   "("
		   "SELECT DISTINCT "
		          "usr_follow.FollowedCod AS UsrCod"
		    " FROM usr_follow,"
		          "(SELECT FollowedCod"
			    " FROM usr_follow"
			   " WHERE FollowerCod=%ld) AS my_followed,"
		         " usr_data"
		   " WHERE usr_follow.FollowerCod=my_followed.FollowedCod"
		     " AND usr_follow.FollowedCod<>%ld"
		     " AND usr_follow.FollowedCod=usr_data.UsrCod"
		     " AND usr_data.Surname1<>''"	// Surname 1 not empty
		     " AND usr_data.FirstName<>''"	// First name not empty
		     "%s"				// SubQuery1
		   ")"
		   " UNION "
		   // 2. Users who share any course with me
		   "("
		   "SELECT DISTINCT "
		          "crs_users.UsrCod"
		    " FROM crs_users,"
			  "(SELECT CrsCod"
			    " FROM crs_users"
			   " WHERE UsrCod=%ld) AS my_crs,"
		         " usr_data"
		   " WHERE crs_users.CrsCod=my_crs.CrsCod"
		     " AND crs_users.UsrCod<>%ld"
		     " AND crs_users.UsrCod=usr_data.UsrCod"
		     " AND usr_data.Surname1<>''"	// Surname 1 not empty
		     " AND usr_data.FirstName<>''"	// First name not empty
		     "%s"				// SubQuery2
		   ")"
		   " UNION "
		   // 3. Users who share any course with me with another role
		   "("
		   "SELECT DISTINCT "
		          "crs_users.UsrCod"
		    " FROM crs_users,"
		          "(SELECT CrsCod,Role"
			    " FROM crs_users"
			   " WHERE UsrCod=%ld) AS my_crs_role,"
		         " usr_data"
		   " WHERE crs_users.CrsCod=my_crs_role.CrsCod"
		     " AND crs_users.Role<>my_crs_role.Role"
		     " AND crs_users.UsrCod=usr_data.UsrCod"
		     " AND usr_data.Surname1<>''"	// Surname 1 not empty
		     " AND usr_data.FirstName<>''"	// First name not empty
		     "%s"				// SubQuery3
		   ")"
		   ") AS LikelyKnownUsrsToFollow"
		   // Do not select my followed
		   " WHERE UsrCod NOT IN"
		   " (SELECT FollowedCod FROM usr_follow"
		   " WHERE FollowerCod=%ld)"
		   // Get only MaxUsrsToShow * 3 users
	        " ORDER BY RAND() LIMIT %u"
		   ")"
		   " UNION "
		   "("
		   /***** Likely unknown userd *****/
		   // 4. Add some likely unknown random user
		   // Be careful with the method to get some random users
		   // from the big table of users.
		   // It's much faster getting a random code and then get the first users
		   // with codes >= that random code
		   // that getting all users and then ordering by rand.
		   "SELECT usr_data.UsrCod"
		    " FROM usr_data,"
			  "(SELECT ROUND(RAND()*(SELECT MAX(UsrCod)"
			    " FROM usr_data)) AS RandomUsrCod) AS random_usr"	// a random user code
		   " WHERE usr_data.UsrCod<>%ld"
		     " AND usr_data.Surname1<>''"	// Surname 1 not empty
		     " AND usr_data.FirstName<>''"	// First name not empty
		   "%s"				// SubQuery4
		   // Do not select my followed
		     " AND usr_data.UsrCod NOT IN"
		         " (SELECT FollowedCod"
		  	    " FROM usr_follow"
		           " WHERE FollowerCod=%ld)"
		     " AND usr_data.UsrCod>=random_usr.RandomUsrCod"	// random user code could not exists in table of users
		   // Get only MaxUsrsToShow users
		   " LIMIT %u"
		   ")"
		   ") AS UsrsToFollow"
		   // Get only MaxUsrsToShow users
		" ORDER BY RAND()"
		   " LIMIT %u",

		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SubQuery1,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SubQuery2,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SubQuery3,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   MaxUsrsToShow * 2,		// 2/3 likely known users

		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   SubQuery4,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   MaxUsrsToShow,		// 1/3 likely unknown users

		   MaxUsrsToShow);
  }

/*****************************************************************************/
/*************** Check if a user is a follower of another user ***************/
/*****************************************************************************/

bool Fol_DB_CheckUsrIsFollowerOf (long FollowerCod,long FollowedCod)
  {
   if (FollowerCod == FollowedCod)
      return false;

   /***** Check if a user is a follower of another user *****/
   return
   DB_QueryEXISTS ("can not check if a user is a follower of another one",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_follow"
		    " WHERE FollowerCod=%ld"
		      " AND FollowedCod=%ld)",
		   FollowerCod,
		   FollowedCod) == Exi_EXISTS;
  }

/*****************************************************************************/
/*************************** Get number of followed **************************/
/*****************************************************************************/

unsigned Fol_DB_GetNumFollowing (long UsrCod)
  {
   /***** Get number of following from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of followed",
		  "SELECT COUNT(*)"
		   " FROM usr_follow"
		  " WHERE FollowerCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/*************************** Get number of followers *************************/
/*****************************************************************************/

unsigned Fol_DB_GetNumFollowers (long UsrCod)
  {
   /***** Get number of followers from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of followers",
		  "SELECT COUNT(*)"
		   " FROM usr_follow"
		  " WHERE FollowedCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/************************** Get list of following ****************************/
/*****************************************************************************/

unsigned Fol_DB_GetListFollowing (MYSQL_RES **mysql_res,long UsrCod)
  {
   /***** Trivial check: user code should be > 0 *****/
   if (UsrCod <= 0)
      return 0;

   /***** Get followed users from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get following",
		   "SELECT FollowedCod"	// row[0]
		    " FROM usr_follow"
		   " WHERE FollowerCod=%ld"
		" ORDER BY FollowTime DESC",
		   UsrCod);
  }

/*****************************************************************************/
/************************** Get list of followers ****************************/
/*****************************************************************************/

unsigned Fol_DB_GetListFollowers (MYSQL_RES **mysql_res,long UsrCod)
  {
   /***** Trivial check: user code should be > 0 *****/
   if (UsrCod <= 0)
      return 0;

   /***** Get followed users from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get followers",
		   "SELECT FollowerCod"
		    " FROM usr_follow"
		   " WHERE FollowedCod=%ld"
		" ORDER BY FollowTime DESC",
		   UsrCod);
  }

/*****************************************************************************/
/************** Get and show number of following and followers ***************/
/*****************************************************************************/

unsigned Fol_DB_GetNumFollowinFollowers (Hie_Level_t HieLvl,unsigned Fol)
  {
   switch (HieLvl)
     {
      case Hie_SYS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the total number of following/followers",
			"SELECT COUNT(DISTINCT %s)"
			 " FROM usr_follow",
			FieldDB[Fol]);
      case Hie_CTY:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the total number of following/followers",
			"SELECT COUNT(DISTINCT usr_follow.%s)"
			 " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "crs_users,"
			       "usr_follow"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=usr_follow.%s",
			FieldDB[Fol],
			Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			FieldDB[Fol]);
      case Hie_INS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the total number of following/followers",
			"SELECT COUNT(DISTINCT usr_follow.%s)"
			 " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "crs_users,"
			       "usr_follow"
			" WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=usr_follow.%s",
			FieldDB[Fol],
			Gbl.Hierarchy.Node[Hie_INS].HieCod,
			FieldDB[Fol]);
      case Hie_CTR:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the total number of following/followers",
			"SELECT COUNT(DISTINCT usr_follow.%s)"
			" FROM deg_degrees,"
			      "crs_courses,"
			      "crs_users,"
			      "usr_follow"
			" WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=usr_follow.%s",
			FieldDB[Fol],
			Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			FieldDB[Fol]);
      case Hie_DEG:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the total number of following/followers",
			"SELECT COUNT(DISTINCT usr_follow.%s)"
			 " FROM crs_courses,"
			       "crs_users,"
			       "usr_follow"
			" WHERE crs_courses.DegCod=%ld"
			  " AND crs_courses.CrsCod=crs_users.CrsCod"
			  " AND crs_users.UsrCod=usr_follow.%s",
			FieldDB[Fol],
			Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			FieldDB[Fol]);
      case Hie_CRS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the total number of following/followers",
			"SELECT COUNT(DISTINCT usr_follow.%s)"
			 " FROM crs_users,"
			       "usr_follow"
			" WHERE crs_users.CrsCod=%ld"
			  " AND crs_users.UsrCod=usr_follow.%s",
			FieldDB[Fol],
			Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			FieldDB[Fol]);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************** Get and show number of following and followers ***************/
/*****************************************************************************/

double Fol_DB_GetNumFollowedPerFollower (Hie_Level_t HieLvl,unsigned Fol)
  {
   switch (HieLvl)
     {
      case Hie_SYS:
	 return DB_QuerySELECTDouble ("can not get number of followed per follower",
				      "SELECT AVG(N)"
				       " FROM (SELECT COUNT(%s) AS N"
					       " FROM usr_follow"
					   " GROUP BY %s) AS F",
				      FieldDB[Fol],
				      FieldDB[1 - Fol]);
      case Hie_CTY:
	 return DB_QuerySELECTDouble ("can not get number of followed per follower",
	                              "SELECT AVG(N)"
				       " FROM (SELECT COUNT(DISTINCT usr_follow.%s) AS N"
					       " FROM ins_instits,"
						     "ctr_centers,"
						     "deg_degrees,"
						     "crs_courses,"
						     "crs_users,"
						     "usr_follow"
					      " WHERE ins_instits.CtyCod=%ld"
						" AND ins_instits.InsCod=ctr_centers.InsCod"
						" AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						" AND deg_degrees.DegCod=crs_courses.DegCod"
						" AND crs_courses.CrsCod=crs_users.CrsCod"
						" AND crs_users.UsrCod=usr_follow.%s"
					   " GROUP BY %s) AS F",
				      FieldDB[Fol],
				      Gbl.Hierarchy.Node[Hie_CTY].HieCod,
				      FieldDB[Fol],
				      FieldDB[1 - Fol]);
      case Hie_INS:
	 return DB_QuerySELECTDouble ("can not get number of followed per follower",
				      "SELECT AVG(N)"
				       " FROM (SELECT COUNT(DISTINCT usr_follow.%s) AS N"
					       " FROM ctr_centers,"
						     "deg_degrees,"
						     "crs_courses,"
						     "crs_users,"
						     "usr_follow"
					      " WHERE ctr_centers.InsCod=%ld"
						" AND ctr_centers.CtrCod=deg_degrees.CtrCod"
						" AND deg_degrees.DegCod=crs_courses.DegCod"
						" AND crs_courses.CrsCod=crs_users.CrsCod"
						" AND crs_users.UsrCod=usr_follow.%s"
					   " GROUP BY %s) AS F",
				      FieldDB[Fol],
				      Gbl.Hierarchy.Node[Hie_INS].HieCod,
				      FieldDB[Fol],
				      FieldDB[1 - Fol]);
      case Hie_CTR:
	 return DB_QuerySELECTDouble ("can not get number of followed per follower",
				      "SELECT AVG(N)"
				       " FROM (SELECT COUNT(DISTINCT usr_follow.%s) AS N"
					       " FROM deg_degrees,"
						     "crs_courses,"
						     "crs_users,"
						     "usr_follow"
					      " WHERE deg_degrees.CtrCod=%ld"
						" AND deg_degrees.DegCod=crs_courses.DegCod"
						" AND crs_courses.CrsCod=crs_users.CrsCod"
						" AND crs_users.UsrCod=usr_follow.%s"
					   " GROUP BY %s) AS F",
				      FieldDB[Fol],
				      Gbl.Hierarchy.Node[Hie_CTR].HieCod,
				      FieldDB[Fol],
				      FieldDB[1 - Fol]);
      case Hie_DEG:
	 return DB_QuerySELECTDouble ("can not get number of followed per follower",
				      "SELECT AVG(N)"
				       " FROM (SELECT COUNT(DISTINCT usr_follow.%s) AS N"
					       " FROM crs_courses,"
						     "crs_users,"
						     "usr_follow"
					      " WHERE crs_courses.DegCod=%ld"
						" AND crs_courses.CrsCod=crs_users.CrsCod"
						" AND crs_users.UsrCod=usr_follow.%s"
					   " GROUP BY %s) AS F",
				      FieldDB[Fol],
				      Gbl.Hierarchy.Node[Hie_DEG].HieCod,
				      FieldDB[Fol],
				      FieldDB[1 - Fol]);
      case Hie_CRS:
	 return DB_QuerySELECTDouble ("can not get number of followed per follower",
				      "SELECT AVG(N)"
				       " FROM (SELECT COUNT(DISTINCT usr_follow.%s) AS N"
					       " FROM crs_users,"
						     "usr_follow"
					      " WHERE crs_users.CrsCod=%ld"
						" AND crs_users.UsrCod=usr_follow.%s"
					   " GROUP BY %s) AS F",
				      FieldDB[Fol],
				      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				      FieldDB[Fol],
				      FieldDB[1 - Fol]);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0.0;	// Not reached
     }
  }

/*****************************************************************************/
/******************************** Follow user ********************************/
/*****************************************************************************/

void Fol_DB_FollowUsr (long UsrCod)
  {
   /***** Trivial check: user code should be > 0 *****/
   if (UsrCod <= 0)
      return;

   /***** Follow user in database *****/
   DB_QueryREPLACE ("can not follow user",
		    "REPLACE INTO usr_follow"
		    " (FollowerCod,FollowedCod,FollowTime)"
		    " VALUES"
		    " (%ld,%ld,NOW())",
		    Gbl.Usrs.Me.UsrDat.UsrCod,
		    UsrCod);
  }

/*****************************************************************************/
/******************************* Unfollow user *******************************/
/*****************************************************************************/

void Fol_DB_UnfollowUsr (long UsrCod)
  {
   /***** Trivial check: user code should be > 0 *****/
   if (UsrCod <= 0)
      return;

   /***** Unfollow user in database *****/
   DB_QueryDELETE ("can not unfollow user",
		   "DELETE FROM usr_follow"
		   " WHERE FollowerCod=%ld"
		     " AND FollowedCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   UsrCod);
  }

/*****************************************************************************/
/*********** Get ranking of users attending to number of followers ***********/
/*****************************************************************************/

unsigned Fol_DB_GetRankingFollowers (MYSQL_RES **mysql_res,Hie_Level_t HieLvl)
  {
   /***** Get ranking from database *****/
   switch (HieLvl)
     {
      case Hie_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT FollowedCod,"					// row[0]
			        "COUNT(FollowerCod) AS N"			// row[1]
			  " FROM usr_follow"
		      " GROUP BY FollowedCod"
		      " ORDER BY N DESC,"
			        "FollowedCod"
			 " LIMIT 100");
      case Hie_CTY:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT usr_follow.FollowedCod,"			// row[0]
			        "COUNT(DISTINCT usr_follow.FollowerCod) AS N"	// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_follow"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_follow.FollowedCod"
		      " GROUP BY usr_follow.FollowedCod"
		      " ORDER BY N DESC,"
			        "usr_follow.FollowedCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT usr_follow.FollowedCod,"			// row[0]
			        "COUNT(DISTINCT usr_follow.FollowerCod) AS N"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_follow"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_follow.FollowedCod"
		      " GROUP BY usr_follow.FollowedCod"
		      " ORDER BY N DESC,"
			        "usr_follow.FollowedCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT usr_follow.FollowedCod,"			// row[0]
			        "COUNT(DISTINCT usr_follow.FollowerCod) AS N"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_follow"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_follow.FollowedCod"
		      " GROUP BY usr_follow.FollowedCod"
		      " ORDER BY N DESC,"
		                "usr_follow.FollowedCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT usr_follow.FollowedCod,"			// row[0]
			        "COUNT(DISTINCT usr_follow.FollowerCod) AS N"	// row[1]
			  " FROM crs_courses,"
			        "crs_users,"
			        "usr_follow"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_follow.FollowedCod"
		      " GROUP BY usr_follow.FollowedCod"
		      " ORDER BY N DESC,"
			        "usr_follow.FollowedCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT usr_follow.FollowedCod,"			// row[0]
			        "COUNT(DISTINCT usr_follow.FollowerCod) AS N"	// row[1]
			  " FROM crs_users,"
			        "usr_follow"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=usr_follow.FollowedCod"
		      " GROUP BY usr_follow.FollowedCod"
		      " ORDER BY N DESC,"
			        "usr_follow.FollowedCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
         break;
      default:
         Err_WrongHierarchyLevelExit ();
         return 0;	// Not reached
     }
  }

/*****************************************************************************/
/*********************** Remove user from user follow ************************/
/*****************************************************************************/

void Fol_DB_RemoveUsrFromUsrFollow (long UsrCod)
  {
   /***** Trivial check: user code should be > 0 *****/
   if (UsrCod <= 0)
      return;

   /***** Remove user from followers and followed *****/
   DB_QueryDELETE ("can not remove user from followers and followed",
		   "DELETE FROM usr_follow"
		   " WHERE FollowerCod=%ld"
		      " OR FollowedCod=%ld",
	           UsrCod,
	           UsrCod);
  }

/*****************************************************************************/
/******* Create/drop temporary tables with me and the users I follow *********/
/*****************************************************************************/

void Fol_DB_CreateTmpTableMeAndUsrsIFollow (void)
  {
   DB_CreateTmpTable ("CREATE TEMPORARY TABLE fol_tmp_me_and_followed "
		      "(UsrCod INT NOT NULL,"
		      "UNIQUE INDEX(UsrCod))"
		      " ENGINE=MEMORY"
		      " SELECT %ld AS UsrCod"		// Me
		      " UNION"
		      " SELECT FollowedCod AS UsrCod"	// Users I follow
		        " FROM usr_follow"
		       " WHERE FollowerCod=%ld",
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      Gbl.Usrs.Me.UsrDat.UsrCod);
  }

void Fol_DB_DropTmpTableMeAndUsrsIFollow (void)
  {
   DB_DropTmpTable ("fol_tmp_me_and_followed");
  }
