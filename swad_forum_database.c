// swad_forum_database.c: forums operations with database

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

#include <stdbool.h>		// For boolean type

#include "swad_database.h"
#include "swad_error.h"
#include "swad_forum.h"
#include "swad_forum_database.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************* Get number of threads in a forum ************************/
/*****************************************************************************/

unsigned For_DB_GetNumThrsInForum (const struct For_Forum *Forum)
  {
   char SubQuery[256];

   /***** Get number of threads in a forum from database *****/
   if (Forum->HieCod > 0)
      sprintf (SubQuery," AND HieCod=%ld",Forum->HieCod);
   else
      SubQuery[0] = '\0';

   return (unsigned)
   DB_QueryCOUNT ("can not get number of threads in a forum",
		  "SELECT COUNT(*)"
		   " FROM for_threads"
		  " WHERE ForumType=%u"
		   "%s",
		  (unsigned) Forum->Type,
		  SubQuery);
  }

/*****************************************************************************/
/**** Get number of threads in forum with a modify time > a specified time ***/
/*****************************************************************************/

unsigned For_DB_GetNumThrsInForumNewerThan (const struct For_Forum *Forum,
                                            const char *Time)
  {
   char SubQuery[256];

   /***** Get number of threads with a last message modify time
          > specified time from database *****/
   if (Forum->HieCod > 0)
      sprintf (SubQuery," AND for_threads.HieCod=%ld",Forum->HieCod);
   else
      SubQuery[0] = '\0';

   return (unsigned)
   DB_QueryCOUNT ("can not check if there are new posts in a forum",
		  "SELECT COUNT(*)"
		   " FROM for_threads,"
			 "for_posts"
		  " WHERE for_threads.ForumType=%u"
		    "%s"
		    " AND for_threads.LastPstCod=for_posts.PstCod"
		    " AND for_posts.ModifTime>'%s'",
		  (unsigned) Forum->Type,
		  SubQuery,
		  Time);
  }

/*****************************************************************************/
/***************** Get number of posts of a user in a forum ******************/
/*****************************************************************************/

unsigned For_DB_GetNumPstsOfUsrInForum (const struct For_Forum *Forum,
                                        long UsrCod)
  {
   char SubQuery[256];

   /***** Get number of posts from database *****/
   if (Forum->HieCod > 0)
      sprintf (SubQuery," AND for_threads.HieCod=%ld",Forum->HieCod);
   else
      SubQuery[0] = '\0';

   return (unsigned)
   DB_QueryCOUNT ("can not get the number of posts of a user in a forum",
		  "SELECT COUNT(*)"
		   " FROM for_posts,"
			 "for_threads"
		  " WHERE for_posts.UsrCod=%ld"
		    " AND for_posts.ThrCod=for_threads.ThrCod"
		    " AND for_threads.ForumType=%u"
		    "%s",
		  UsrCod,
		  (unsigned) Forum->Type,
		  SubQuery);
  }

/*****************************************************************************/
/************* Remove all threads and posts in forums of a scope *************/
/*****************************************************************************/

void For_DB_RemoveForums (Hie_Level_t Level,long HieCod)
  {
   static const struct
     {
      For_ForumType_t Usrs;
      For_ForumType_t Tchs;
     } ForumType[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = {For_FORUM_GLOBAL_USRS,For_FORUM_GLOBAL_TCHS},	// No forums for this scope
      [Hie_SYS] = {For_FORUM_GLOBAL_USRS,For_FORUM_GLOBAL_TCHS},	// Not removable
      [Hie_CTY] = {For_FORUM_GLOBAL_USRS,For_FORUM_GLOBAL_TCHS},	// No forums for this scope
      [Hie_INS] = {For_FORUM_INSTIT_USRS,For_FORUM_INSTIT_TCHS},
      [Hie_CTR] = {For_FORUM_CENTER_USRS,For_FORUM_CENTER_TCHS},
      [Hie_DEG] = {For_FORUM_DEGREE_USRS,For_FORUM_DEGREE_TCHS},
      [Hie_CRS] = {For_FORUM_COURSE_USRS,For_FORUM_COURSE_TCHS},
     };

   /***** Remove disabled posts *****/
   DB_QueryDELETE ("can not remove the disabled posts in forums",
		   "DELETE FROM for_disabled"
		   " USING for_threads,"
		          "for_posts,"
		          "for_disabled"
		   " WHERE"
		     " (for_threads.ForumType=%u"
		      " OR"
		      " for_threads.ForumType=%u)"
		     " AND for_threads.HieCod=%ld"
		     " AND for_threads.ThrCod=for_posts.ThrCod"
		     " AND for_posts.PstCod=for_disabled.PstCod",
	           ForumType[Level].Usrs,
	           ForumType[Level].Tchs,
	           HieCod);

   /***** Remove posts *****/
   DB_QueryDELETE ("can not remove posts in forums",
		   "DELETE FROM for_posts"
		   " USING for_threads,"
		          "for_posts"
		   " WHERE"
		     " (for_threads.ForumType=%u"
		      " OR"
		      " for_threads.ForumType=%u)"
		     " AND for_threads.HieCod=%ld"
		     " AND for_threads.ThrCod=for_posts.ThrCod",
	           ForumType[Level].Usrs,
	           ForumType[Level].Tchs,
	           HieCod);

   /***** Remove threads read *****/
   DB_QueryDELETE ("can not remove read threads in forums",
		   "DELETE FROM for_read"
		   " USING for_threads,"
		          "for_read"
		   " WHERE"
		     " (for_threads.ForumType=%u"
		      " OR"
		      " for_threads.ForumType=%u)"
		     " AND for_threads.HieCod=%ld"
		     " AND for_threads.ThrCod=for_read.ThrCod",
	           ForumType[Level].Usrs,
	           ForumType[Level].Tchs,
	           HieCod);

   /***** Remove threads *****/
   DB_QueryDELETE ("can not remove threads in forums",
		   "DELETE FROM for_threads"
		   " WHERE"
		    " (for_threads.ForumType=%u"
		     " OR"
		     " for_threads.ForumType=%u)"
		     " AND HieCod=%ld",
	           ForumType[Level].Usrs,
	           ForumType[Level].Tchs,
	           HieCod);
  }

/*****************************************************************************/
/**************** Insert new post in the table of forum posts ****************/
/*****************************************************************************/

long For_DB_InsertForumPst (long ThrCod,long UsrCod,
                            const char *Subject,const char *Content,
                            long MedCod)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create a new post in a forum",
				"INSERT INTO for_posts"
				" (ThrCod,UsrCod,CreatTime,ModifTime,NumNotif,"
				  "Subject,Content,MedCod)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),0,"
				  "'%s','%s',%ld)",
				ThrCod,
				UsrCod,
				Subject,
				Content,
				MedCod);
  }

/*****************************************************************************/
/********* Update number of users notified in table of forum posts **********/
/*****************************************************************************/

void For_DB_UpdateNumUsrsNotifiedByEMailAboutPost (long PstCod,
                                                   unsigned NumUsrsToBeNotifiedByEMail)
  {
   DB_QueryUPDATE ("can not update the number of notifications of a post",
		   "UPDATE for_posts"
		     " SET NumNotif=NumNotif+%u"
		   " WHERE PstCod=%ld",
                   NumUsrsToBeNotifiedByEMail,
                   PstCod);
  }

/*****************************************************************************/
/******************** Get if a forum post exists in database *****************/
/*****************************************************************************/

bool For_DB_CheckIfForumPstExists (long PstCod)
  {
   return
   DB_QueryEXISTS ("can not check if a post of a forum already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM for_posts"
		    " WHERE PstCod=%ld)",
		   PstCod);	// Post exists if it appears in table of forum posts
  }

/*****************************************************************************/
/*************************** Get data of a forum post ************************/
/*****************************************************************************/

unsigned For_DB_GetPstData (MYSQL_RES **mysql_res,long PstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a post",
		   "SELECT UsrCod,"			// row[0]
			  "UNIX_TIMESTAMP(CreatTime),"	// row[1]
			  "Subject,"			// row[2]
			  "Content,"			// row[3]
			  "MedCod"			// row[4]
		    " FROM for_posts"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/***************** Get subject and content for a forum post ******************/
/*****************************************************************************/

unsigned For_DB_GetPstSubjectAndContent (MYSQL_RES **mysql_res,long PstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get subject and content",
		   "SELECT Subject,"	// row[0]
			  "Content"	// row[1]
		    " FROM for_posts"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/********* Get the forum thread, type and hierarchy code of a post ***********/
/*****************************************************************************/

unsigned For_DB_GetThreadForumTypeAndHieCodOfAPost (MYSQL_RES **mysql_res,long PstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get forum thread, type and hierarchy",
		   "SELECT for_threads.ThrCod,"		// row[0]
			  "for_threads.ForumType,"	// row[1]
			  "for_threads.HieCod"	// row[2]
		    " FROM for_posts,"
			  "for_threads"
		   " WHERE for_posts.PstCod=%ld"
		     " AND for_posts.ThrCod=for_threads.ThrCod",
		   PstCod);
  }

/*****************************************************************************/
/*********** Get the forum type and hierarchy code of a thread ***************/
/*****************************************************************************/

unsigned For_DB_GetForumTypeAndHieCodOfAThread (MYSQL_RES **mysql_res,long ThrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get forum type and hierarchy",
		   "SELECT ForumType,"	// row[0]
			  "HieCod"	// row[1]
		    " FROM for_threads"
		   " WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/*********************** Get number of posts from a user *********************/
/*****************************************************************************/

unsigned For_DB_GetNumPostsUsr (long UsrCod)
  {
   return DB_QueryCOUNT ("can not get number of forum posts from a user",
			 "SELECT COUNT(*)"
			  " FROM for_posts"
			 " WHERE UsrCod=%ld",
			 UsrCod);
  }

/*****************************************************************************/
/************************* Get posts of a thread *****************************/
/*****************************************************************************/

unsigned For_DB_GetPostsOfAThread (MYSQL_RES **mysql_res,long ThrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get posts of a thread",
		   "SELECT PstCod,"			// row[0]
			  "UNIX_TIMESTAMP(CreatTime)"	// row[1]
		    " FROM for_posts"
		   " WHERE ThrCod=%ld"
		   " ORDER BY PstCod",
		   ThrCod);
  }

/*****************************************************************************/
/*********** Get the number of posts in the thread than holds a post *********/
/*****************************************************************************/

unsigned For_DB_GetThreadAndNumPostsGivenPstCod (MYSQL_RES **mysql_res,long PstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get number of posts in a thread",
		   "SELECT ThrCod,"		// row[1]
			 " COUNT(PstCod)"	// row[0]
		    " FROM for_posts"
		   " WHERE ThrCod IN"
		         " (SELECT ThrCod"
			    " FROM for_posts"
			   " WHERE PstCod=%ld)"
	        " GROUP BY ThrCod;",
		   PstCod);
  }

/*****************************************************************************/
/************* Get all publisher codes in a thread, except me ****************/
/*****************************************************************************/

unsigned For_DB_GetPublishersInThreadExceptMe (MYSQL_RES **mysql_res,long PstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get publishers in thread",
		   "SELECT DISTINCT "
		          "UsrCod"
		    " FROM for_posts"
		   " WHERE ThrCod=(SELECT ThrCod"
				   " FROM for_posts"
				  " WHERE PstCod=%ld)"
		     " AND UsrCod<>%ld",
		   PstCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Remove post from forum posts table ********************/
/*****************************************************************************/

void For_DB_RemovePst (long PstCod)
  {
   DB_QueryDELETE ("can not remove a post from a forum",
		   "DELETE FROM for_posts"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/************************ Remove all posts in a thread ***********************/
/*****************************************************************************/

void For_DB_RemoveThreadPsts (long ThrCod)
  {
   DB_QueryDELETE ("can not remove the posts of a thread of a forum",
		   "DELETE FROM for_posts"
		   " WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/*************** Insert a new thread in table of forum threads ***************/
/*****************************************************************************/
// Returns the code of the new inserted thread

long For_DB_InsertForumThread (const struct For_Forums *Forums,
                               long FirstPstCod)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create a new thread in a forum",
				"INSERT INTO for_threads"
				" (ForumType,HieCod,FirstPstCod,LastPstCod)"
				" VALUES"
				" (%u,%ld,%ld,%ld)",
				(unsigned) Forums->Forum.Type,
				Forums->Forum.HieCod,
				FirstPstCod,
				FirstPstCod);
  }

/*****************************************************************************/
/********* Modify the codes of the first and last posts of a thread **********/
/*****************************************************************************/

void For_DB_UpdateThrFirstAndLastPst (long ThrCod,long FirstPstCod,long LastPstCod)
  {
   DB_QueryUPDATE ("can not update a thread of a forum",
		   "UPDATE for_threads"
		     " SET FirstPstCod=%ld,"
		          "LastPstCod=%ld"
		   " WHERE ThrCod=%ld",
                   FirstPstCod,
                   LastPstCod,
                   ThrCod);
  }

/*****************************************************************************/
/************** Modify the code of the last post of a thread *****************/
/*****************************************************************************/

void For_DB_UpdateThrLastPst (long ThrCod,long LastPstCod)
  {
   /***** Update the code of the last post of a thread *****/
   DB_QueryUPDATE ("can not update a thread of a forum",
		   "UPDATE for_threads"
		     " SET LastPstCod=%ld"
		   " WHERE ThrCod=%ld",
                   LastPstCod,
                   ThrCod);
  }

/*****************************************************************************/
/********** Show available threads of a forum highlighting a thread **********/
/*****************************************************************************/

unsigned For_DB_GetForumThreads (MYSQL_RES **mysql_res,
                                 const struct For_Forums *Forums)
  {
   char SubQuery[256];

   /***** Get threads of a forum from database *****/
   if (Forums->Forum.HieCod > 0)
      sprintf (SubQuery," AND for_threads.HieCod=%ld",
	       Forums->Forum.HieCod);
   else
      SubQuery[0] = '\0';

   switch (Forums->ThreadsOrder)
     {
      case Dat_STR_TIME:	// First post time
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get thread of a forum",
			 "SELECT for_threads.ThrCod"	// row[0]
			  " FROM for_threads,"
				"for_posts"
			 " WHERE for_threads.ForumType=%u"
			   "%s"
			   " AND for_threads.FirstPstCod=for_posts.PstCod"
			 " ORDER BY for_posts.CreatTime DESC",
			 (unsigned) Forums->Forum.Type,
			 SubQuery);
      case Dat_END_TIME:	// Last post time
         return (unsigned)
         DB_QuerySELECT (mysql_res,"can not get thread of a forum",
			 "SELECT for_threads.ThrCod"	// row[0]
			  " FROM for_threads,"
				"for_posts"
			 " WHERE for_threads.ForumType=%u"
			   "%s"
			   " AND for_threads.LastPstCod=for_posts.PstCod"
			 " ORDER BY for_posts.CreatTime DESC",
			 (unsigned) Forums->Forum.Type,
			 SubQuery);
      default:	// Impossible
	 return 0;
     }
  }

/*****************************************************************************/
/***************************** Get data of a thread **************************/
/*****************************************************************************/

unsigned For_DB_GetThreadData (MYSQL_RES **mysql_res,long ThrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a thread of a forum",
		       "SELECT m0.PstCod,"			// row[0]
			      "m1.PstCod,"			// row[1]
			      "m0.UsrCod,"			// row[2]
			      "m1.UsrCod,"			// row[3]
			      "UNIX_TIMESTAMP(m0.CreatTime),"	// row[4]
			      "UNIX_TIMESTAMP(m1.CreatTime),"	// row[5]
			      "m0.Subject"
			" FROM for_threads,"
			      "for_posts AS m0,"
			      "for_posts AS m1"
		       " WHERE for_threads.ThrCod=%ld"
			 " AND for_threads.FirstPstCod=m0.PstCod"
			 " AND for_threads.LastPstCod=m1.PstCod",
		       ThrCod);
  }

/*****************************************************************************/
/***************************** Get thread subject ****************************/
/*****************************************************************************/

void For_DB_GetThreadTitle (long ThrCod,char *Subject,size_t TitleSize)
  {
   DB_QuerySELECTString (Subject,TitleSize,"can not get thread subject",
		         "SELECT for_posts.Subject"	// row[0]
			  " FROM for_threads,"
			        "for_posts"
		         " WHERE for_threads.ThrCod=%ld"
			   " AND for_threads.FirstPstCod=for_posts.PstCod",
		         ThrCod);
  }

/*****************************************************************************/
/***************** Get if a thread belongs to current forum ******************/
/*****************************************************************************/

bool For_DB_CheckIfThrBelongsToForum (long ThrCod,const struct For_Forum *Forum)
  {
   char SubQuery[256];

   /***** Get if a thread belong to current forum from database *****/
   if (Forum->HieCod > 0)
      sprintf (SubQuery," AND for_threads.HieCod=%ld",Forum->HieCod);
   else
      SubQuery[0] = '\0';

   return
   DB_QueryEXISTS ("can not check if a thread belong to current forum",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM for_threads"
		    " WHERE ThrCod=%ld"
		      " AND ForumType=%u"
		        "%s)",
		   ThrCod,
		   (unsigned) Forum->Type,
		   SubQuery);
  }

/*****************************************************************************/
/**************** Get the code of the last post of a thread ******************/
/*****************************************************************************/

long For_DB_GetThrLastPst (long ThrCod)
  {
   long LastPstCod;

   /***** Get the code of the last post of a thread from database *****/
   LastPstCod = DB_QuerySELECTCode ("can not get the most recent post"
			            " of a thread of a forum",
				    "SELECT PstCod"
				     " FROM for_posts"
				    " WHERE ThrCod=%ld"
				    " ORDER BY CreatTime DESC"
				    " LIMIT 1",
				    ThrCod);
   if (LastPstCod <= 0)
      Err_ShowErrorAndExit ("Error when getting the most recent post of a thread of a forum.");

   return LastPstCod;
  }

/*****************************************************************************/
/********************* Get the thread subject from a thread ******************/
/*****************************************************************************/

void For_DB_GetThrSubject (long ThrCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1])
  {
   DB_QuerySELECTString (Subject,Cns_MAX_BYTES_SUBJECT,
                         "can not get the subject of a thread of a forum",
			 "SELECT for_posts.Subject"	// row[0]
			  " FROM for_threads,"
				"for_posts"
			 " WHERE for_threads.ThrCod=%ld"
			   " AND for_threads.FirstPstCod=for_posts.PstCod",
			 ThrCod);
  }

/*****************************************************************************/
/********** Get number of users that have write posts in a thread ************/
/*****************************************************************************/

unsigned For_DB_GetNumOfWritersInThr (long ThrCod)
  {
   /***** Get number of distinct writers in a thread from database *****/
   return DB_QueryCOUNT ("can not get the number of writers in a thread of a forum",
			 "SELECT COUNT(DISTINCT UsrCod)"
			  " FROM for_posts"
			 " WHERE ThrCod=%ld",
			 ThrCod);
  }

/*****************************************************************************/
/********************** Get number of posts in a thread **********************/
/*****************************************************************************/

unsigned For_DB_GetNumPstsInThr (long ThrCod)
  {
   /***** Get number of posts in a thread from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of posts in a thread of a forum",
		  "SELECT COUNT(*)"
		   " FROM for_posts"
		  " WHERE ThrCod=%ld",
		  ThrCod);
  }

/*****************************************************************************/
/************** Get whether there are posts of mine in a thread **************/
/*****************************************************************************/

unsigned For_DB_GetNumMyPstsInThr (long ThrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not check if you have written posts in a thead of a forum",
		  "SELECT COUNT(*)"
		   " FROM for_posts"
		  " WHERE ThrCod=%ld"
		    " AND UsrCod=%ld",
		  ThrCod,
		  Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/**** Get number of posts in thread with a modify time > a specified time ****/
/*****************************************************************************/

unsigned For_DB_GetNumPstsInThrNewerThan (long ThrCod,const char *Time)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not check if there are new posts in a thread of a forum",
		  "SELECT COUNT(*)"
		   " FROM for_posts"
		  " WHERE ThrCod=%ld"
		    " AND ModifTime>'%s'",
		  ThrCod,
		  Time);
  }

/*****************************************************************************/
/*************** Delete a thread from the forum thread table *****************/
/*****************************************************************************/

void For_DB_RemoveThread (long ThrCod)
  {
   DB_QueryDELETE ("can not remove a thread from a forum",
		   "DELETE FROM for_threads"
		   " WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/************* Update read date of a thread for the current user *************/
/*****************************************************************************/
// Update for_read table indicating that this thread page and previous ones
// have been read and have no new posts for the current user
// (even if any previous pages have been no read actually)
// Note that database is not updated with the current time,
// but with the creation time of the most recent post in this thread read by me.

void For_DB_UpdateThrReadTime (long ThrCod,
                               time_t CreatTimeUTCOfTheMostRecentPostRead)
  {
   /***** Insert or replace pair ThrCod-UsrCod in for_read *****/
   DB_QueryREPLACE ("can not update the status of reading"
		    " of a thread of a forum",
		    "REPLACE INTO for_read"
		    " (ThrCod,UsrCod,ReadTime)"
		    " VALUES"
		    " (%ld,%ld,FROM_UNIXTIME(%ld))",
	            ThrCod,
	            Gbl.Usrs.Me.UsrDat.UsrCod,
	            (long) CreatTimeUTCOfTheMostRecentPostRead);
  }

/*****************************************************************************/
/**************** Get number of users that have read a thread ****************/
/*****************************************************************************/

unsigned For_DB_GetNumReadersOfThr (long ThrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of readers of a thread of a forum",
		  "SELECT COUNT(*)"
		   " FROM for_read"
		  " WHERE ThrCod=%ld",
		  ThrCod);
  }

/*****************************************************************************/
/****************** Get thread read time for the current user ****************/
/*****************************************************************************/

unsigned For_DB_GetThrReadTime (MYSQL_RES **mysql_res,long ThrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get date of reading"
			     " of a thread of a forum",
		   "SELECT UNIX_TIMESTAMP(ReadTime)"	// row[0]
		    " FROM for_read"
		   " WHERE ThrCod=%ld"
		     " AND UsrCod=%ld",
		   ThrCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********************** Get last time I read a forum ************************/
/*****************************************************************************/

unsigned For_DB_GetLastTimeIReadForum (MYSQL_RES **mysql_res,
                                       const struct For_Forum *Forum)
  {
   char SubQuery[256];

   if (Forum->HieCod > 0)
      sprintf (SubQuery," AND for_threads.HieCod=%ld",Forum->HieCod);
   else
      SubQuery[0] = '\0';

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the date of reading of a forum",
		   "SELECT IFNULL(MAX(for_read.ReadTime),FROM_UNIXTIME(0))"	// row[0]
		    " FROM for_read,"
			  "for_threads"
		   " WHERE for_read.UsrCod=%ld"
		     " AND for_read.ThrCod=for_threads.ThrCod"
		     " AND for_threads.ForumType=%u"
		     "%s",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Forum->Type,
		   SubQuery);
  }

/*****************************************************************************/
/*********************** Get last time I read a thread ***********************/
/*****************************************************************************/

unsigned For_DB_GetLastTimeIReadThread (MYSQL_RES **mysql_res,long ThrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the date of reading of a thread",
		   "SELECT ReadTime"		// row[0]
		    " FROM for_read"
		   " WHERE ThrCod=%ld"
		     " AND UsrCod=%ld",
		   ThrCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Remove thread read status for a thread ****************/
/*****************************************************************************/

void For_DB_RemoveThrFromReadThrs (long ThrCod)
  {
   DB_QueryDELETE ("can not remove the status of reading"
		   " of a thread of a forum",
		   "DELETE FROM for_read"
		   " WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/********************** Remove thread read status for a user *****************/
/*****************************************************************************/

void For_DB_RemoveUsrFromReadThrs (long UsrCod)
  {
   DB_QueryDELETE ("can not remove the status of reading by a user"
		   " of all threads in a forum",
		   "DELETE FROM for_read"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*********************** Insert thread in my clipboard ***********************/
/*****************************************************************************/

void For_DB_InsertThrInMyClipboard (long ThrCod)
  {
   DB_QueryREPLACE ("can not add thread to clipboard",
		    "REPLACE INTO for_clipboards"
		    " (ThrCod,UsrCod)"
		    " VALUES"
		    " (%ld,%ld)",
                    ThrCod,
                    Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************ Move a thread to current forum *********************/
/*****************************************************************************/

void For_DB_MoveThrToCurrentForum (const struct For_Forums *Forums)
  {
   /***** Move a thread to current forum *****/
   switch (Forums->Forum.Type)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE for_threads"
			   " SET ForumType=%u,"
			        "HieCod=-1"
			 " WHERE ThrCod=%ld",
                         (unsigned) Forums->Forum.Type,
                         Forums->Thread.Current);
         break;
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE for_threads"
			   " SET ForumType=%u,"
			        "HieCod=%ld"
			 " WHERE ThrCod=%ld",
		         (unsigned) Forums->Forum.Type,
		         Forums->Forum.HieCod,
		         Forums->Thread.Current);
         break;
      case For_FORUM_CENTER_USRS:
      case For_FORUM_CENTER_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE for_threads"
			   " SET ForumType=%u,"
			        "HieCod=%ld"
			 " WHERE ThrCod=%ld",
                         (unsigned) Forums->Forum.Type,
                         Forums->Forum.HieCod,
                         Forums->Thread.Current);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE for_threads"
			   " SET ForumType=%u,"
			        "HieCod=%ld"
			 " WHERE ThrCod=%ld",
		         (unsigned) Forums->Forum.Type,
		         Forums->Forum.HieCod,
		         Forums->Thread.Current);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         DB_QueryUPDATE ("can not move a thread to current forum",
			 "UPDATE for_threads"
			   " SET ForumType=%u,"
			        "HieCod=%ld"
			 " WHERE ThrCod=%ld",
		         (unsigned) Forums->Forum.Type,
		         Forums->Forum.HieCod,
		         Forums->Thread.Current);
         break;
      default:
	 Err_WrongForumExit ();
	 break;
     }
  }

/*****************************************************************************/
/**************** Get if there is a thread ready to be moved *****************/
/*****************************************************************************/

long For_DB_GetThrInMyClipboard (void)
  {
   return DB_QuerySELECTCode ("can not check if there is"
			      " any thread ready to be moved",
	                      "SELECT ThrCod"
	                       " FROM for_clipboards"
	                      " WHERE UsrCod=%ld",
			      Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/**************** Remove thread code from thread clipboard *******************/
/*****************************************************************************/

void For_DB_RemoveThrFromClipboard (long ThrCod)
  {
   DB_QueryDELETE ("can not remove a thread from clipboard",
		   "DELETE FROM for_clipboards"
		   " WHERE ThrCod=%ld",
		   ThrCod);
  }

/*****************************************************************************/
/********************* Remove thread clipboard of a user *********************/
/*****************************************************************************/

void For_DB_RemoveUsrFromClipboard (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a thread from the clipboard of a user",
		   "DELETE FROM for_clipboards"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************* Remove expired thread clipboards (from all users) *************/
/*****************************************************************************/

void For_DB_RemoveExpiredClipboards (void)
  {
   DB_QueryDELETE ("can not remove old threads from clipboards",
		   "DELETE LOW_PRIORITY FROM for_clipboards"
		   " WHERE TimeInsert<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_THREAD_CLIPBOARD);
  }

/*****************************************************************************/
/****************** Insert post into table of banned posts *******************/
/*****************************************************************************/

void For_DB_InsertPstIntoDisabled (long PstCod)
  {
   DB_QueryREPLACE ("can not ban a post of a forum",
		    "REPLACE INTO for_disabled"
		    " (PstCod,UsrCod,DisableTime)"
		    " VALUES"
		    " (%ld,%ld,NOW())",
                    PstCod,
                    Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********************** Get if a forum post is disabled *********************/
/*****************************************************************************/

Cns_DisabledOrEnabled_t For_DB_GetIfPstIsDisabledOrEnabled (long PstCod)
  {
   /***** Trivial check: post code should be > 0 *****/
   if (PstCod <= 0)
      return Cns_DISABLED;

   /***** Get if post is disabled from database *****/
   return
   DB_QueryEXISTS ("can not check if a post of a forum is disabled",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM for_disabled"
		    " WHERE PstCod=%ld)",
		   PstCod) ? Cns_DISABLED :
			     Cns_ENABLED;	// Post is enabled if it does not appear in table of disabled posts
  }

/*****************************************************************************/
/****************** Remove post from table of disabled posts *****************/
/*****************************************************************************/

void For_DB_RemovePstFromDisabled (long PstCod)
  {
   DB_QueryDELETE ("can not unban a post of a forum",
		   "DELETE FROM for_disabled"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/*********************** Delete disabled posts in thread *********************/
/*****************************************************************************/

void For_DB_RemoveDisabledPstsInThread (long ThrCod)
  {
   DB_QueryDELETE ("can not unban the posts of a thread of a forum",
		   "DELETE FROM for_disabled"
		   " USING for_posts,"
		          "for_disabled"
		   " WHERE for_posts.ThrCod=%ld"
		     " AND for_posts.PstCod=for_disabled.PstCod",
                   ThrCod);
  }

/*****************************************************************************/
/********************** Get number of forums of a type ***********************/
/*****************************************************************************/

unsigned For_DB_GetNumTotalForumsOfType (For_ForumType_t ForumType,
					 long HieCod[Hie_NUM_LEVELS])
  {
   /***** Get number of forums of a type from database *****/
   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         return 1;	// Only one forum
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
         if (HieCod[Hie_INS] > 0)	// Number of institutions forums for an institution <= 1
            return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT HieCod)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
                           (unsigned) ForumType,
                           HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of institution forums for a country
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_CTY]);

				       // Number of institutions forums for the whole platform
	 return (unsigned)
	 DB_QueryCOUNT ("can not get number of forums of a type",
			 "SELECT COUNT(DISTINCT HieCod)"
			  " FROM for_threads"
			 " WHERE ForumType=%u",
			 (unsigned) ForumType);
      case For_FORUM_CENTER_USRS:
      case For_FORUM_CENTER_TCHS:
         if (HieCod[Hie_CTR] > 0)	// Number of center forums for a center <= 1
            return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT HieCod)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
                           (unsigned) ForumType,
                           HieCod[Hie_CTR]);

         if (HieCod[Hie_INS] > 0)	// Number of center forums for an institution
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "ctr_centers"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of center forums for a country
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "ctr_centers,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_CTY]);

				       // Number of center forums for the whole platform
         return (unsigned)
         DB_QueryCOUNT ("can not get number of forums of a type",
		        "SELECT COUNT(DISTINCT HieCod)"
		         " FROM for_threads"
		        " WHERE ForumType=%u",
		        (unsigned) ForumType);
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         if (HieCod[Hie_DEG] > 0)	// Number of degree forums for a degree <= 1
            return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT HieCod)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
                           (unsigned) ForumType,
                           HieCod[Hie_DEG]);

         if (HieCod[Hie_CTR] > 0)	// Number of degree forums for a center
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "deg_degrees"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_CTR]);

         if (HieCod[Hie_INS] > 0)	// Number of degree forums for an institution
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "deg_degrees,"
			          "ctr_centers"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of degree forums for a country
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "deg_degrees,"
			          "ctr_centers,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_CTY]);

				       // Number of degree forums for the whole platform
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of forums of a type",
			"SELECT COUNT(DISTINCT HieCod)"
			 " FROM for_threads"
			" WHERE ForumType=%u",
			(unsigned) ForumType);
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         if (HieCod[Hie_CRS] > 0)	// Number of course forums for a course <= 1
            return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT HieCod)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
                           (unsigned) ForumType,
                           HieCod[Hie_CRS]);

         if (HieCod[Hie_DEG] > 0)	// Number of course forums for a degree
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "crs_courses"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_DEG]);

         if (HieCod[Hie_CTR] > 0)	// Number of course forums for a center
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "crs_courses,"
			          "deg_degrees"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			    " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_CTR]);

         if (HieCod[Hie_INS] > 0)	// Number of course forums for an institution
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "crs_courses,"
			          "deg_degrees,"
			          "ctr_centers"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of course forums for a country
	    return (unsigned)
            DB_QueryCOUNT ("can not get number of forums of a type",
			   "SELECT COUNT(DISTINCT for_threads.HieCod)"
			    " FROM for_threads,"
			          "crs_courses,"
			          "deg_degrees,"
			          "ctr_centers,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
		           (unsigned) ForumType,
		           HieCod[Hie_CTY]);

				       // Number of course forums for the whole platform
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of forums of a type",
			"SELECT COUNT(DISTINCT HieCod)"
			 " FROM for_threads"
			" WHERE ForumType=%u",
			(unsigned) ForumType);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/*********** Get total number of threads in forums of this type **************/
/*****************************************************************************/

unsigned For_DB_GetNumTotalThrsInForumsOfType (For_ForumType_t ForumType,
					       long HieCod[Hie_NUM_LEVELS])
  {
   /***** Get total number of threads in forums of this type from database *****/
   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
					// Total number of threads in forums of this type
         return (unsigned)
         DB_QueryCOUNT ("can not get the number of threads in forums of a type",
		        "SELECT COUNT(*)"
		         " FROM for_threads"
		        " WHERE ForumType=%u",
		        (unsigned) ForumType);
      case For_FORUM_INSTIT_USRS:
      case For_FORUM_INSTIT_TCHS:
         if (HieCod[Hie_INS] > 0)	// Number of threads in institution forums for an institution
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of threads in institution forums for a country
	    return (unsigned)
	    DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CTY]);

         				// Number of threads in institution forums for the whole platform
         return (unsigned)
         DB_QueryCOUNT ("can not get the number of threads in forums of a type",
		        "SELECT COUNT(*)"
		         " FROM for_threads"
		        " WHERE ForumType=%u",
		        (unsigned) ForumType);
      case For_FORUM_CENTER_USRS:
      case For_FORUM_CENTER_TCHS:
         if (HieCod[Hie_CTR] > 0)	// Number of threads in center forums for a center <= 1
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CTR]);

         if (HieCod[Hie_INS] > 0)	// Number of threads in center forums for an institution
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "ctr_centers"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of threads in center forums for a country
	    return (unsigned)
	    DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "ctr_centers,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CTY]);

         				// Number of threads in center forums for the whole platform
         return (unsigned)
         DB_QueryCOUNT ("can not get the number of threads in forums of a type",
		        "SELECT COUNT(*)"
		         " FROM for_threads"
		        " WHERE ForumType=%u",
		        (unsigned) ForumType);
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         if (HieCod[Hie_DEG] > 0)	// Number of threads in degree forums for a degree
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_DEG]);

         if (HieCod[Hie_CTR] > 0)	// Number of threads in degree forums for a center
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "deg_degrees"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CTR]);

         if (HieCod[Hie_INS] > 0)	// Number of threads in degree forums for an institution
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			  "SELECT COUNT(*)"
			   " FROM for_threads,"
			         "deg_degrees,"
			         "ctr_centers"
			  " WHERE for_threads.ForumType=%u"
			    " AND for_threads.HieCod=deg_degrees.DegCod"
			    " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			    " AND ctr_centers.InsCod=%ld",
			  (unsigned) ForumType,
			  HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of threads in degree forums for a country
	    return (unsigned)
	    DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "deg_degrees,"
			          "ctr_centers,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CTY]);

         				// Number of threads in degree forums for the whole platform
         return (unsigned)
         DB_QueryCOUNT ("can not get the number of threads in forums of a type",
		        "SELECT COUNT(*)"
		         " FROM for_threads"
		        " WHERE ForumType=%u",
		        (unsigned) ForumType);
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         if (HieCod[Hie_CRS] > 0)	// Number of threads in course forums for a course
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads"
			   " WHERE ForumType=%u"
			     " AND HieCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CRS]);

         if (HieCod[Hie_DEG] > 0)	// Number of threads in course forums for a degree
	    return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "crs_courses"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_DEG]);

         if (HieCod[Hie_CTR] > 0)	// Number of threads in course forums for a center
	    return (unsigned)
	    DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "crs_courses,"
			          "deg_degrees"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CTR]);

         if (HieCod[Hie_INS] > 0)	// Number of threads in course forums for an institution
            return (unsigned)
            DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "crs_courses,"
			          "deg_degrees,"
			          "ctr_centers"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_INS]);

         if (HieCod[Hie_CTY] > 0)	// Number of threads in course forums for a country
	    return (unsigned)
	    DB_QueryCOUNT ("can not get the number of threads in forums of a type",
			   "SELECT COUNT(*)"
			    " FROM for_threads,"
			          "crs_courses,"
			          "deg_degrees,"
			          "ctr_centers,"
			          "ins_instits"
			   " WHERE for_threads.ForumType=%u"
			     " AND for_threads.HieCod=crs_courses.CrsCod"
			     " AND crs_courses.DegCod=deg_degrees.DegCod"
			     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			     " AND ctr_centers.InsCod=ins_instits.InsCod"
			     " AND ins_instits.CtyCod=%ld",
			   (unsigned) ForumType,
			   HieCod[Hie_CTY]);

         				// Number of threads in course forums for the whole platform
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of threads in forums of a type",
		        "SELECT COUNT(*)"
		         " FROM for_threads"
		        " WHERE ForumType=%u",
		        (unsigned) ForumType);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/************** Get total number of posts in forums of a type ****************/
/*****************************************************************************/

unsigned For_DB_GetNumTotalPstsInForumsOfType (For_ForumType_t ForumType,
					       long HieCod[Hie_NUM_LEVELS],
                                               unsigned *NumUsrsToBeNotifiedByEMail)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPosts;

   /***** Get total number of posts in forums of this type from database *****/
   switch (ForumType)
     {
      case For_FORUM_GLOBAL_USRS:
      case For_FORUM_GLOBAL_TCHS:
      case For_FORUM__SWAD__USRS:
      case For_FORUM__SWAD__TCHS:
         // Total number of posts in forums of this type
         DB_QuerySELECT (&mysql_res,"can not get the total number"
				    " of forums of a type",
			 "SELECT COUNT(*),"			// row[0]
			        "SUM(for_posts.NumNotif)"	// row[1]
			  " FROM for_threads,"
			        "for_posts "
			 " WHERE for_threads.ForumType=%u"
			   " AND for_threads.ThrCod=for_posts.ThrCod",
                         (unsigned) ForumType);
         break;
      case For_FORUM_INSTIT_USRS:	case For_FORUM_INSTIT_TCHS:
         if (HieCod[Hie_INS] > 0)	// Number of posts in institutions forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_INS]);
         else if (HieCod[Hie_CTY] > 0)	// Number of posts in institutions forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "ins_instits,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=ins_instits.InsCod"
			      " AND ins_instits.CtyCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_CTY]);
         else				// Number of posts in institution forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts "
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
		            (unsigned) ForumType);
         break;
      case For_FORUM_CENTER_USRS:
      case For_FORUM_CENTER_TCHS:
         if (HieCod[Hie_CTR] > 0)	// Number of posts in center forums for a center
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_CTR]);
         else if (HieCod[Hie_INS] > 0)	// Number of posts in center forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "ctr_centers,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=ctr_centers.CtrCod"
			      " AND ctr_centers.InsCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_INS]);
         else if (HieCod[Hie_CTY] > 0)	// Number of posts in center forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "ctr_centers,"
			           "ins_instits,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=ctr_centers.CtrCod"
			      " AND ctr_centers.InsCod=ins_instits.InsCod"
			      " AND ins_instits.CtyCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_CTY]);
         else				// Number of posts in center forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
		            (unsigned) ForumType);
         break;
      case For_FORUM_DEGREE_USRS:
      case For_FORUM_DEGREE_TCHS:
         if (HieCod[Hie_DEG] > 0)	// Number of posts in degree forums for a degree
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_DEG]);
         else if (HieCod[Hie_CTR] > 0)	// Number of posts in degree forums for a center
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "deg_degrees,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=deg_degrees.DegCod"
			      " AND deg_degrees.CtrCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_CTR]);
         else if (HieCod[Hie_INS] > 0)	// Number of posts in degree forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "deg_degrees,"
			           "ctr_centers,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=deg_degrees.DegCod"
			      " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			      " AND ctr_centers.InsCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_INS]);
         else if (HieCod[Hie_CTY] > 0)	// Number of posts in degree forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "deg_degrees,"
			           "ctr_centers,"
			           "ins_instits,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=deg_degrees.DegCod"
			      " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			      " AND ctr_centers.InsCod=ins_instits.InsCod"
			      " AND ins_instits.CtyCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_CTY]);
         else				// Number of posts in degree forums for the whole platform
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts "
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType);
         break;
      case For_FORUM_COURSE_USRS:
      case For_FORUM_COURSE_TCHS:
         if (HieCod[Hie_CRS] > 0)	// Number of posts in course forums for a course
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts "
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_CRS]);
         else if (HieCod[Hie_DEG] > 0)	// Number of posts in course forums for a degree
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "crs_courses,"
			           "for_posts "
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=crs_courses.CrsCod"
			      " AND crs_courses.DegCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
		            (unsigned) ForumType,
		            HieCod[Hie_DEG]);
         else if (HieCod[Hie_CTR] > 0)	// Number of posts in course forums for a center
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "crs_courses,"
			           "deg_degrees,"
			           "for_posts "
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=crs_courses.CrsCod"
			      " AND crs_courses.DegCod=deg_degrees.DegCod"
			      " AND deg_degrees.CtrCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
		            (unsigned) ForumType,
		            HieCod[Hie_CTR]);
         else if (HieCod[Hie_INS] > 0)	// Number of posts in course forums for an institution
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "crs_courses,"
			           "deg_degrees,"
			           "ctr_centers,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=crs_courses.CrsCod"
			      " AND crs_courses.DegCod=deg_degrees.DegCod"
			      " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			      " AND ctr_centers.InsCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_INS]);
         else if (HieCod[Hie_CTY] > 0)	// Number of posts in course forums for a country
            DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "crs_courses,"
			           "deg_degrees,"
			           "ctr_centers,"
			           "ins_instits,"
			           "for_posts"
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.HieCod=crs_courses.CrsCod"
			      " AND crs_courses.DegCod=deg_degrees.DegCod"
			      " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			      " AND ctr_centers.InsCod=ins_instits.InsCod"
			      " AND ins_instits.CtyCod=%ld"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
                            (unsigned) ForumType,
                            HieCod[Hie_CTY]);
         else				// Number of posts in course forums for the whole platform
	    DB_QuerySELECT (&mysql_res,"can not get the total number"
				       " of forums of a type",
			    "SELECT COUNT(*),"			// row[0]
			           "SUM(for_posts.NumNotif)"	// row[1]
			     " FROM for_threads,"
			           "for_posts "
			    " WHERE for_threads.ForumType=%u"
			      " AND for_threads.ThrCod=for_posts.ThrCod",
		            (unsigned) ForumType);
         break;
      default:
	 return 0;
     }

   /* Get row with number of posts */
   row = mysql_fetch_row (mysql_res);

   /* Get number of posts (row[0]) */
   if (sscanf (row[0],"%u",&NumPosts) != 1)
      Err_ShowErrorAndExit ("Error when getting the total number of forums of a type.");

   /* Get number of users notified (row[1]) */
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumUsrsToBeNotifiedByEMail) != 1)
         Err_ShowErrorAndExit ("Error when getting the total number of forums of a type.");
     }
   else
      *NumUsrsToBeNotifiedByEMail = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumPosts;
  }
