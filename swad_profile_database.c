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

// extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

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
/********** Get ranking of a user according to the number of clicks **********/
/*****************************************************************************/

unsigned Prf_DB_GetRankingFigure (long UsrCod,const char *FieldName)
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
