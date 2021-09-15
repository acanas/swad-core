// swad_forum_database.c: forums operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

// #define _GNU_SOURCE 		// For asprintf
#include <stdbool.h>		// For boolean type
// #include <malloc.h>		// For malloc
// #include <mysql/mysql.h>	// To access MySQL databases
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <string.h>
// #include <time.h>		// For time_t

// #include "swad_action.h"
// #include "swad_box.h"
// #include "swad_config.h"
#include "swad_database.h"
// #include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_form.h"
#include "swad_forum.h"
#include "swad_forum_database.h"
#include "swad_global.h"
// #include "swad_hierarchy.h"
// #include "swad_HTML.h"
// #include "swad_layout.h"
// #include "swad_logo.h"
// #include "swad_message.h"
// #include "swad_notification.h"
// #include "swad_pagination.h"
// #include "swad_parameter.h"
// #include "swad_profile.h"
// #include "swad_role.h"
// #include "swad_timeline.h"
// #include "swad_timeline_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************ Public constants and types *************************/
/*****************************************************************************/

/*****************************************************************************/
/*********************** Private constants and types *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

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
/******************** Get if a forum post exists in database *****************/
/*****************************************************************************/

bool For_DB_GetIfForumPstExists (long PstCod)
  {
   return (DB_QueryCOUNT ("can not check if a post of a forum already existed",
			  "SELECT COUNT(*)"
			   " FROM for_posts"
			  " WHERE PstCod=%ld",
			  PstCod) != 0);	// Post exists if it appears in table of forum posts
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
				" (ForumType,Location,FirstPstCod,LastPstCod)"
				" VALUES"
				" (%u,%ld,%ld,%ld)",
				(unsigned) Forums->Forum.Type,
				Forums->Forum.Location,
				FirstPstCod,
				FirstPstCod);
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
/*********************** Get if a forum post is enabled **********************/
/*****************************************************************************/

bool For_DB_GetIfPstIsEnabled (long PstCod)
  {
   /***** Trivial check: post code should be > 0 *****/
   if (PstCod <= 0)
      return false;

   /***** Get if post is disabled from database *****/
   return (DB_QueryCOUNT ("can not check if a post of a forum is disabled",
			  "SELECT COUNT(*)"
			   " FROM for_disabled"
			  " WHERE PstCod=%ld",
			  PstCod) == 0);	// Post is enabled if it does not appear in table of disabled posts
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
		    "USING for_posts,"
		          "for_disabled"
		   " WHERE for_posts.ThrCod=%ld"
		     " AND for_posts.PstCod=for_disabled.PstCod",
                   ThrCod);
  }
