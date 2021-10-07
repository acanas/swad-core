// swad_profile_database.c: user's public profile, operations with database

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
#include "swad_error.h"
#include "swad_global.h"
#include "swad_profile_database.h"

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
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Create user's figures **************************/
/*****************************************************************************/

void Prf_DB_CreateUsrFigures (long UsrCod,const struct UsrFigures *UsrFigures,
                              bool CreatingMyOwnAccount)
  {
   char SubQueryFirstClickTime[64];

   if (CreatingMyOwnAccount)
      // This is the first click
      Str_Copy (SubQueryFirstClickTime,"NOW()",sizeof (SubQueryFirstClickTime) - 1);
   else
      snprintf (SubQueryFirstClickTime,sizeof (SubQueryFirstClickTime),
                "FROM_UNIXTIME(%ld)",
	        (long) UsrFigures->FirstClickTimeUTC);	//   0 ==> unknown first click time or user never logged

   DB_QueryINSERT ("can not create user's figures",
		   "INSERT INTO usr_figures"
		   " (UsrCod,FirstClickTime,"
		     "NumClicks,NumSocPub,NumFileViews,NumForPst,NumMsgSnt)"
		   " VALUES"
		   " (%ld,%s,"
		     "%d,%d,%d,%d,%d)",
		   UsrCod,
		   SubQueryFirstClickTime,
		   UsrFigures->NumClicks,		// -1 ==> unknown number of clicks
		   UsrFigures->NumTimelinePubs,		// -1 ==> unknown number of timeline publications
		   UsrFigures->NumFileViews,		// -1 ==> unknown number of file views
		   UsrFigures->NumForumPosts,		// -1 ==> unknown number of forum posts
		   UsrFigures->NumMessagesSent);	// -1 ==> unknown number of messages sent
  }

/*****************************************************************************/
/****************** Update first click time in user's figures ****************/
/*****************************************************************************/

void Prf_DB_UpdateFirstClickTimeUsr (long UsrCod,time_t FirstClickTimeUTC)
  {
   DB_QueryUPDATE ("can not update user's figures",
		   "UPDATE usr_figures"
		     " SET FirstClickTime=FROM_UNIXTIME(%ld)"
		   " WHERE UsrCod=%ld",
		   (long) FirstClickTimeUTC,
		   UsrCod);
  }

/*****************************************************************************/
/***************** Update number of clicks in user's figures *****************/
/*****************************************************************************/

void Prf_DB_UpdateNumClicksUsr (long UsrCod,int NumClicks)
  {
   DB_QueryUPDATE ("can not update user's figures",
		   "UPDATE usr_figures"
		     " SET NumClicks=%d"
		   " WHERE UsrCod=%ld",
		   NumClicks,	// -1 ==> unknown number of clicks
		   UsrCod);
  }

/*****************************************************************************/
/********* Update number of timeline publications in user's figures **********/
/*****************************************************************************/

void Prf_DB_UpdateNumTimelinePubsUsr (long UsrCod,int NumTimelinePubs)
  {
   DB_QueryUPDATE ("can not update user's figures",
		   "UPDATE usr_figures"
		     " SET NumSocPub=%d"
		   " WHERE UsrCod=%ld",
		   NumTimelinePubs,		// -1 ==> unknown number of timeline publications
		   UsrCod);
  }

/*****************************************************************************/
/*************** Update number of file views in user's figures ***************/
/*****************************************************************************/

void Prf_DB_UpdateNumFileViewsUsr (long UsrCod,int NumFileViews)
  {
   DB_QueryUPDATE ("can not update user's figures",
		   "UPDATE usr_figures"
		     " SET NumFileViews=%d"
		   " WHERE UsrCod=%ld",
		   NumFileViews,		// -1 ==> unknown number of file views
		   UsrCod);
  }

/*****************************************************************************/
/************** Update number of forum posts in user's figures ***************/
/*****************************************************************************/

void Prf_DB_UpdateNumForumPostsUsr (long UsrCod,int NumForumPosts)
  {
   DB_QueryUPDATE ("can not update user's figures",
		   "UPDATE usr_figures"
		     " SET NumForPst=%d"
		   " WHERE UsrCod=%ld",
		   NumForumPosts,		// -1 ==> unknown number of forum posts
		   UsrCod);
  }

/*****************************************************************************/
/************** Update number of messages sent in user's figures *************/
/*****************************************************************************/

void Prf_DB_UpdateNumMessagesSentUsr (long UsrCod,int NumMessagesSent)
  {
   DB_QueryUPDATE ("can not update user's figures",
		   "UPDATE usr_figures"
		     " SET NumMsgSnt=%d"
		   " WHERE UsrCod=%ld",
		   NumMessagesSent,
		   UsrCod);
  }

/*****************************************************************************/
/*************** Increment number of clicks made by a user *******************/
/*****************************************************************************/

void Prf_DB_IncrementNumClicksUsr (long UsrCod)
  {
   /***** Increment number of clicks *****/
   // If NumClicks < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's clicks",
		   "UPDATE IGNORE usr_figures"
		     " SET NumClicks=NumClicks+1"
		   " WHERE UsrCod=%ld"
		     " AND NumClicks>=0",
	           UsrCod);
  }

/*****************************************************************************/
/********* Increment number of timeline publications sent by a user **********/
/*****************************************************************************/

void Prf_DB_IncrementNumTimelinePubsUsr (long UsrCod)
  {
   /***** Increment number of timeline publications *****/
   // If NumSocPub < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's timeline publications",
		   "UPDATE IGNORE usr_figures"
		     " SET NumSocPub=NumSocPub+1"
		   " WHERE UsrCod=%ld"
		     " AND NumSocPub>=0",
	           UsrCod);
  }

/*****************************************************************************/
/************** Increment number of file views sent by a user ****************/
/*****************************************************************************/

void Prf_DB_IncrementNumFileViewsUsr (long UsrCod)
  {
   /***** Increment number of file views *****/
   // If NumFileViews < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's file views",
		   "UPDATE IGNORE usr_figures"
		     " SET NumFileViews=NumFileViews+1"
		   " WHERE UsrCod=%ld"
		     " AND NumFileViews>=0",
	           UsrCod);
  }

/*****************************************************************************/
/************* Increment number of forum posts sent by a user ****************/
/*****************************************************************************/

void Prf_DB_IncrementNumForPstUsr (long UsrCod)
  {
   /***** Increment number of forum posts *****/
   // If NumForPst < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's forum posts",
		   "UPDATE IGNORE usr_figures"
		     " SET NumForPst=NumForPst+1"
		   " WHERE UsrCod=%ld"
		     " AND NumForPst>=0",
	           UsrCod);
  }

/*****************************************************************************/
/*************** Increment number of messages sent by a user *****************/
/*****************************************************************************/

void Prf_DB_IncrementNumMsgSntUsr (long UsrCod)
  {
   /***** Increment number of messages sent *****/
   // If NumMsgSnt < 0 ==> not yet calculated, so do nothing
   DB_QueryUPDATE ("can not increment user's messages sent",
		   "UPDATE IGNORE usr_figures"
		     " SET NumMsgSnt=NumMsgSnt+1"
		   " WHERE UsrCod=%ld"
		     " AND NumMsgSnt>=0",
	           UsrCod);
  }

/*****************************************************************************/
/***************** Get ranking of a user according to a figure ***************/
/*****************************************************************************/

unsigned Prf_DB_GetUsrRankingFigure (long UsrCod,const char *FieldName)
  {
   /***** Select number of rows with figure
          greater than the figure of this user *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get ranking using a figure",
		  "SELECT COUNT(*)+1"
		   " FROM usr_figures"
		  " WHERE UsrCod<>%ld"	// Really not necessary here
		    " AND %s>(SELECT %s"
			      " FROM usr_figures"
			     " WHERE UsrCod=%ld)",
		  UsrCod,
		  FieldName,
		  FieldName,
		  UsrCod);
  }

/*****************************************************************************/
/************************** Get ranking of a figure **************************/
/*****************************************************************************/

unsigned Prf_DB_GetRankingFigure (MYSQL_RES **mysql_res,const char *FieldName)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT UsrCod,"		// row[0]
			        "%s"			// row[1]
			  " FROM usr_figures"
			 " WHERE %s>0"
			   " AND UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY %s DESC,"
			           "UsrCod"
			 " LIMIT 100",
			 FieldName,
			 FieldName,
			 FieldName);
      case HieLvl_CTY:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Cty.CtyCod,
			 FieldName,
			 FieldName);
      case HieLvl_INS:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Ins.InsCod,
			 FieldName,
			 FieldName);
      case HieLvl_CTR:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Ctr.CtrCod,
			 FieldName,
			 FieldName);
      case HieLvl_DEG:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Deg.DegCod,
			 FieldName,
			 FieldName);
      case HieLvl_CRS:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"	// row[0]
			        "usr_figures.%s"	// row[1]
			  " FROM crs_users,"
			        "usr_figures"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.%s>0"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY usr_figures.%s DESC,"
			           "usr_figures.UsrCod"
			 " LIMIT 100",
			 FieldName,
			 Gbl.Hierarchy.Crs.CrsCod,
			 FieldName,
			 FieldName);
      default:
         Err_WrongScopeExit ();
         return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********* Get ranking of users attending to number of clicks per day ********/
/*****************************************************************************/

unsigned Prf_DB_GetRankingClicksPerDay (MYSQL_RES **mysql_res)
  {
   switch (Gbl.Scope.Current)
     {
      case HieLvl_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT UsrCod,"						// row[0]
			        "NumClicks/(DATEDIFF(NOW(),"
			                   "FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM usr_figures"
			 " WHERE NumClicks>0"
			   " AND FirstClickTime>FROM_UNIXTIME(0)"
			   " AND UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
			           "UsrCod"
			 " LIMIT 100");
      case HieLvl_CTY:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Cty.CtyCod);
      case HieLvl_INS:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM crs_courses,"
			        "crs_users,"
			        "usr_figures"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN"
			       " (SELECT UsrCod"
			          " FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
	 return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get ranking",
			 "SELECT DISTINCTROW "
			        "usr_figures.UsrCod,"					// row[0]
			        "usr_figures.NumClicks/(DATEDIFF(NOW(),"
			        "usr_figures.FirstClickTime)+1) AS NumClicksPerDay"	// row[1]
			  " FROM crs_users,"
			        "usr_figures"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=usr_figures.UsrCod"
			   " AND usr_figures.NumClicks>0"
			   " AND usr_figures.FirstClickTime>FROM_UNIXTIME(0)"
			   " AND usr_figures.UsrCod NOT IN (SELECT UsrCod FROM usr_banned)"
			 " ORDER BY NumClicksPerDay DESC,"
				   "usr_figures.UsrCod"
			 " LIMIT 100",
			 Gbl.Hierarchy.Crs.CrsCod);
      default:
         Err_WrongScopeExit ();
         return 0;	// Not reached
     }
  }

/*****************************************************************************/
/********************* Get number of users with a figure *********************/
/*****************************************************************************/

unsigned Prf_DB_GetNumUsrsWithFigure (const char *FieldName)
  {
   /***** Select number of rows with values already calculated *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users with a figure",
		  "SELECT COUNT(*)"
		   " FROM usr_figures"
		  " WHERE %s>=0",
		  FieldName);
  }

/*****************************************************************************/
/****** Get ranking of a user according to the number of clicks per day ******/
/*****************************************************************************/

unsigned Prf_DB_GetRankingNumClicksPerDay (long UsrCod)
  {
   /***** Select number of rows with number of clicks per day
          greater than the clicks per day of this user *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get ranking using number of clicks per day",
		  "SELECT COUNT(*)+1"
		   " FROM (SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1) AS NumClicksPerDay"
			   " FROM usr_figures"
			  " WHERE UsrCod<>%ld"	// Necessary because the following comparison is not exact in floating point
			    " AND NumClicks>0"
			    " AND FirstClickTime>FROM_UNIXTIME(0)) AS TableNumClicksPerDay"
		  " WHERE NumClicksPerDay>"
		         "(SELECT NumClicks/(DATEDIFF(NOW(),FirstClickTime)+1)"
			   " FROM usr_figures"
			  " WHERE UsrCod=%ld"
			    " AND NumClicks>0"
			    " AND FirstClickTime>FROM_UNIXTIME(0))",
		  UsrCod,
		  UsrCod);
  }

/*****************************************************************************/
/************** Get number of users with number of clicks per day ************/
/*****************************************************************************/

unsigned Prf_DB_GetNumUsrsWithNumClicksPerDay (void)
  {
   /***** Select number of rows with values already calculated *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users with number of clicks per day",
		  "SELECT COUNT(*)"
		   " FROM usr_figures"
		  " WHERE NumClicks>0"
		    " AND FirstClickTime>FROM_UNIXTIME(0)");
  }

/*****************************************************************************/
/*** Check if it exists an entry for this user in table of user's figures ****/
/*****************************************************************************/

bool Prf_DB_CheckIfUsrFiguresExists (long UsrCod)
  {
   return (DB_QueryCOUNT ("can not get user's first click",
			  "SELECT COUNT(*)"
			   " FROM usr_figures"
			  " WHERE UsrCod=%ld",
			  UsrCod) != 0);
  }

/*****************************************************************************/
/********************** Get user's figures from database *********************/
/*****************************************************************************/

unsigned Prf_DB_GetUsrFigures (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's figures",
		   "SELECT UNIX_TIMESTAMP(FirstClickTime),"	// row[0]
			  "DATEDIFF(NOW(),FirstClickTime)+1,"	// row[1]
			  "NumClicks,"				// row[2]
			  "NumSocPub,"				// row[3]
			  "NumFileViews,"			// row[4]
			  "NumForPst,"				// row[5]
			  "NumMsgSnt"				// row[6]
		    " FROM usr_figures"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/**************************** Remove user's figures **************************/
/*****************************************************************************/

void Prf_DB_RemoveUsrFigures (long UsrCod)
  {
   DB_QueryDELETE ("can not delete user's figures",
		   "DELETE FROM usr_figures"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
