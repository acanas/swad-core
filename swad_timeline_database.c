// swad_timeline_database.c: social timeline operations with database

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
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

static const char *TL_DB_Table[TL_Fav_NUM_WHAT_TO_FAV] =
  {
   [TL_Fav_NOTE] = "tl_notes_fav",
   [TL_Fav_COMM] = "tl_comments_fav",
  };
static const char *TL_DB_Field[TL_Fav_NUM_WHAT_TO_FAV] =
  {
   [TL_Fav_NOTE] = "NotCod",
   [TL_Fav_COMM] = "PubCod",
  };
static TL_Pub_PubType_t TL_DB_PubType[TL_Fav_NUM_WHAT_TO_FAV] =
  {
   [TL_Fav_NOTE] = TL_Pub_ORIGINAL_NOTE,
   [TL_Fav_COMM] = TL_Pub_COMMENT_TO_NOTE,
  };

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

static long TL_DB_GetMedCod (const char *Table,const char *Field,long Cod);

/*****************************************************************************/
/********************* Get which users to show in timeline *******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetWho (MYSQL_RES **mysql_res)
  {
   /***** Get which users from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get which timeline users",
		   "SELECT TimelineUsrs"	// row[0]
		   " FROM usr_last WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Get which users to show in timeline *******************/
/*****************************************************************************/

void TL_DB_UpdateWho (Usr_Who_t Who)
  {
   /***** Update which users in database *****/
   DB_QueryUPDATE ("can not update which timeline users",
		   "UPDATE usr_last SET TimelineUsrs=%u"
		   " WHERE UsrCod=%ld",
		   (unsigned) Who,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Get data of note using its code ***********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetDataOfNoteByCod (long NotCod,MYSQL_RES **mysql_res)
  {
   /***** Trivial check: note code should be > 0 *****/
   if (NotCod <= 0)
      return 0;

   /***** Get data of note from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of note",
		   "SELECT NotCod,"			// row[0]
			  "NoteType,"			// row[1]
			  "Cod,"			// row[2]
			  "UsrCod,"			// row[3]
			  "HieCod,"			// row[4]
			  "Unavailable,"		// row[5]
			  "UNIX_TIMESTAMP(TimeNote)"	// row[6]
		   " FROM tl_notes"
		   " WHERE NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/*************** Get code of publication of the original note ****************/
/*****************************************************************************/

long TL_DB_GetPubCodOfOriginalNote (long NotCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long OriginalPubCod = -1L;	// Default value

   /***** Get code of publication of the original note *****/
   if (DB_QuerySELECT (&mysql_res,"can not get code of publication",
		       "SELECT PubCod FROM tl_pubs"
		       " WHERE NotCod=%ld AND PubType=%u",
		       NotCod,(unsigned) TL_Pub_ORIGINAL_NOTE) == 1)
     {	// Result should have a unique row
      /* Get code of publication (row[0]) */
      row = mysql_fetch_row (mysql_res);
      OriginalPubCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return OriginalPubCod;
  }

/*****************************************************************************/
/***************************** Create a new note *****************************/
/*****************************************************************************/
// Returns code of note just created

long TL_DB_CreateNewNote (TL_Not_NoteType_t NoteType,long Cod,
                          long PublisherCod,long HieCod)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new note",
				"INSERT INTO tl_notes"
				" (NoteType,Cod,UsrCod,HieCod,Unavailable,TimeNote)"
				" VALUES"
				" (%u,%ld,%ld,%ld,'N',NOW())",
				(unsigned) NoteType,	// Post, file, exam, notice, forum
				Cod,			// Post, file, exam, notice, forum code
				PublisherCod,		// Publisher code
				HieCod);		// Where in hierarchy
  }

/*****************************************************************************/
/************************* Mark a note as unavailable ************************/
/*****************************************************************************/

void TL_DB_MarkNoteAsUnavailable (TL_Not_NoteType_t NoteType,long Cod)
  {
   /***** Mark note as unavailable *****/
   DB_QueryUPDATE ("can not mark note as unavailable",
		   "UPDATE tl_notes SET Unavailable='Y'"
		   " WHERE NoteType=%u AND Cod=%ld",
		   (unsigned) NoteType,Cod);
  }

/*****************************************************************************/
/***** Mark possible notes involving children of a folder as unavailable *****/
/*****************************************************************************/

void TL_DB_MarkNotesChildrenOfFolderAsUnavailable (TL_Not_NoteType_t NoteType,
                                                   Brw_FileBrowser_t FileBrowser,
                                                   long Cod,
                                                   const char *Path)
  {
   /***** Mark notes as unavailable *****/
   DB_QueryUPDATE ("can not mark notes as unavailable",
		   "UPDATE tl_notes SET Unavailable='Y'"
		   " WHERE NoteType=%u AND Cod IN"
		   " (SELECT FilCod FROM files"
		   " WHERE FileBrowser=%u AND Cod=%ld"
		   " AND Path LIKE '%s/%%' AND Public='Y')",	// Only public files
		   (unsigned) NoteType,
		   (unsigned) FileBrowser,Cod,
		   Path);
  }

/*****************************************************************************/
/******* Create temporary tables used to not get notes already shown *********/
/*****************************************************************************/

void TL_DB_CreateTmpTableJustRetrievedNotes (void)
  {
   /***** Create temporary table with notes just retrieved *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_tmp_just_retrieved_notes "
	     "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
	     " ENGINE=MEMORY");
  }

void TL_DB_CreateTmpTableVisibleTimeline (void)
  {
   /***** Create temporary table with all notes visible in timeline *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_tmp_visible_timeline "
	     "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
	     " ENGINE=MEMORY"
	     " SELECT NotCod FROM tl_timelines WHERE SessionId='%s'",
	     Gbl.Session.Id);
  }

/*****************************************************************************/
/**** Insert note in temporary tables used to not get notes already shown ****/
/*****************************************************************************/

void TL_DB_InsertNoteInJustRetrievedNotes (long NotCod)
  {
   /* Insert note in temporary table with just retrieved notes.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tl_tmp_just_retrieved_notes"
		   " SET NotCod=%ld",
		   NotCod);
  }

void TL_DB_InsertNoteInVisibleTimeline (long NotCod)
  {
   /* Insert note in temporary table with visible timeline.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tl_tmp_visible_timeline"
		   " SET NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/****** Add just retrieved notes to current timeline for this session ********/
/*****************************************************************************/

void TL_DB_AddNotesJustRetrievedToVisibleTimelineOfSession (void)
  {
   /* tl_timelines contains the distinct notes in timeline of each open session:
mysql> SELECT SessionId,COUNT(*) FROM tl_timelines GROUP BY SessionId;
+---------------------------------------------+----------+
| SessionId                                   | COUNT(*) |
+---------------------------------------------+----------+
| u-X-R3gKki7eKMXrNCP8bGhwOAZuVngRy7FNGZFMKzI |       52 | --> 52 distinct notes
| u1CoqL1YWl3_hR4wk4bI7vhnc-uRcCmIDyKYAgBB6kk |       10 |
| u8xqamzkorHfY4BvYRMXjNhzHvQyigZUZemO0YiMn48 |       10 |
| u_n2V_L3KrFjnd4SqZk0gxMFwZHRuWZ8_EIVTU9sdpI |       10 |
| V6pGe1kGGS_uO5i__waqXKnuDkPYaDZHNAYr-Zv-GJQ |        2 |
| vqDRz-iiM8v10Dl8ThwqIqmDRIklz8szJaqflwXZucs |       10 |
| w11juqKPx6lg-f_pL2ZBYqlagU1mEepSvvk9L3gDGac |       10 | --> 10 distinct notes
| wLg4e8KQljCcVuFWIkJjNeti89kAiwOZ3iyXdzm_eDk |       10 |
| wnU85YrwJHhZGWIZhd7LQfQTPrclIWHfMF3DcB-Rcgw |        4 |
| wRzRJFnHfzW61fZYnvMIaMRlkuWUeEyqXVQ6JeWA32k |       11 |
+---------------------------------------------+----------+
10 rows in set (0,01 sec)
   */
   DB_QueryINSERT ("can not insert notes in timeline",
		   "INSERT IGNORE INTO tl_timelines"
	           " (SessionId,NotCod)"
	           " SELECT '%s',NotCod FROM tl_tmp_just_retrieved_notes",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/******** Drop temporary tables used to not get notes already shown **********/
/*****************************************************************************/

void TL_DB_DropTmpTableJustRetrievedNotes (void)
  {
   /***** Drop temporary table with notes just retrieved *****/
   DB_Query ("can not remove temporary table",
	     "DROP TEMPORARY TABLE IF EXISTS tl_tmp_just_retrieved_notes");
  }

void TL_DB_DropTmpTableVisibleTimeline (void)
  {
   /***** Drop temporary table with all notes visible in timeline *****/
   DB_Query ("can not remove temporary table",
             "DROP TEMPORARY TABLE IF EXISTS tl_tmp_visible_timeline");
  }

/*****************************************************************************/
/******************* Clear unused old timelines in database ******************/
/*****************************************************************************/

void TL_DB_ClearOldTimelinesNotesFromDB (void)
  {
   /***** Remove timelines for expired sessions *****/
   DB_QueryDELETE ("can not remove old timelines",
		   "DELETE LOW_PRIORITY FROM tl_timelines"
                   " WHERE SessionId NOT IN (SELECT SessionId FROM sessions)");
  }

/*****************************************************************************/
/***************** Clear timeline for a session in database ******************/
/*****************************************************************************/

void TL_DB_ClearTimelineNotesOfSessionFromDB (void)
  {
   /***** Remove timeline for a session *****/
   DB_QueryDELETE ("can not remove timeline",
		   "DELETE FROM tl_timelines"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/*************************** Remove favs for a note **************************/
/*****************************************************************************/

void TL_DB_RemoveNoteFavs (long NotCod)
  {
   /***** Remove favs for note *****/
   DB_QueryDELETE ("can not remove favs for note",
		   "DELETE FROM tl_notes_fav"
		   " WHERE NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/******************** Remove all publications of this note *******************/
/*****************************************************************************/

void TL_DB_RemoveNotePubs (long NotCod)
  {
   /***** Remove all publications of this note *****/
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tl_pubs"
		   " WHERE NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/******************* Remove note publication from database *******************/
/*****************************************************************************/

void TL_DB_RemoveNote (long NotCod)
  {
   /***** Remove note *****/
   DB_QueryDELETE ("can not remove a note",
		   "DELETE FROM tl_notes"
	           " WHERE NotCod=%ld"
	           " AND UsrCod=%ld",		// Extra check: author
		   NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Remove all notes created by a user *********************/
/*****************************************************************************/

void TL_DB_RemoveAllNotesUsr (long UsrCod)
  {
   /***** Remove all notes created by user *****/
   DB_QueryDELETE ("can not remove notes",
		   "DELETE FROM tl_notes WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/********************* Get data of post using its code ***********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetPostByCod (long PstCod,MYSQL_RES **mysql_res)
  {
   /***** Trivial check: post code should be > 0 *****/
   if (PstCod <= 0)
      return 0;

   /***** Get data of post from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the content"
			     " of a post",
		   "SELECT Txt,"	// row[0]
			  "MedCod"	// row[1]
		   " FROM tl_posts"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/***************** Get code of media associated to post **********************/
/*****************************************************************************/

long TL_DB_GetMedCodFromPost (long PstCod)
  {
   return TL_DB_GetMedCod ("tl_posts","PstCod",PstCod);
  }

/*****************************************************************************/
/********************* Insert post content in database ***********************/
/*****************************************************************************/
// Returns code of just created post

long TL_DB_CreateNewPost (const struct TL_Pst_PostContent *Content)
  {
   /***** Insert post content in the database *****/
   return
   DB_QueryINSERTandReturnCode ("can not create post",
				"INSERT INTO tl_posts"
				" (Txt,MedCod)"
				" VALUES"
				" ('%s',%ld)",
				Content->Txt,
				Content->Media.MedCod);
  }

/*****************************************************************************/
/************************* Remove post from database *************************/
/*****************************************************************************/

void TL_DB_RemovePost (long PstCod)
  {
   /***** Remove post *****/
   DB_QueryDELETE ("can not remove a post",
		   "DELETE FROM tl_posts"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/************************* Remove all posts of a user ************************/
/*****************************************************************************/

void TL_DB_RemoveAllPostsUsr (long UsrCod)
  {
   /***** Remove all posts of the user *****/
   DB_QueryDELETE ("can not remove posts",
		   "DELETE FROM tl_posts"
		   " WHERE PstCod IN"
		   " (SELECT Cod FROM tl_notes"
	           " WHERE UsrCod=%ld AND NoteType=%u)",
		   UsrCod,(unsigned) TL_NOTE_POST);
  }

/*****************************************************************************/
/********************* Get number of comments in a note **********************/
/*****************************************************************************/

unsigned TL_DB_GetNumCommsInNote (long NotCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of comments in a note",
		  "SELECT COUNT(*) FROM tl_pubs"
		  " WHERE NotCod=%ld AND PubType=%u",
		  NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/************** Get publication codes of comments of a note from database *****************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetComms (long NotCod,MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT PubCod"	// row[0]
		   " FROM tl_pubs"
		   " WHERE NotCod=%ld AND PubType=%u",
		   NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/************** Get initial comments of a note from database *****************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetInitialComms (long NotCod,
				unsigned NumInitialCommsToGet,
				MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT tl_pubs.PubCod,"			// row[0]
			  "tl_pubs.PublisherCod,"		// row[1]
			  "tl_pubs.NotCod,"			// row[2]
			  "UNIX_TIMESTAMP(tl_pubs.TimePublish),"// row[3]
			  "tl_comments.Txt,"			// row[4]
			  "tl_comments.MedCod"			// row[5]
		   " FROM tl_pubs,tl_comments"
		   " WHERE tl_pubs.NotCod=%ld"
		   " AND tl_pubs.PubType=%u"
		   " AND tl_pubs.PubCod=tl_comments.PubCod"
		   " ORDER BY tl_pubs.PubCod"
		   " LIMIT %lu",
		   NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE,
		   NumInitialCommsToGet);
  }

/*****************************************************************************/
/*************** Get final comments of a note from database ******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetFinalComms (long NotCod,unsigned NumFinalCommsToGet,
			      MYSQL_RES **mysql_res)
  {
   /***** Get final comments of a note from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT * FROM "
		   "("
		   "SELECT tl_pubs.PubCod,"			// row[0]
			  "tl_pubs.PublisherCod,"		// row[1]
			  "tl_pubs.NotCod,"			// row[2]
			  "UNIX_TIMESTAMP(tl_pubs.TimePublish),"// row[3]
			  "tl_comments.Txt,"			// row[4]
			  "tl_comments.MedCod"			// row[5]
	          " FROM tl_pubs,tl_comments"
		  " WHERE tl_pubs.NotCod=%ld"
		  " AND tl_pubs.PubType=%u"
		  " AND tl_pubs.PubCod=tl_comments.PubCod"
		  " ORDER BY tl_pubs.PubCod DESC LIMIT %u"
		  ") AS comments"
		  " ORDER BY PubCod",
		  NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE,
		  NumFinalCommsToGet);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetDataOfCommByCod (long PubCod,MYSQL_RES **mysql_res)
  {
   /***** Trivial check: publication code should be > 0 *****/
   if (PubCod <= 0)
      return 0;

   /***** Get data of comment from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of comment",
		   "SELECT tl_pubs.PubCod,"			// row[0]
			  "tl_pubs.PublisherCod,"		// row[1]
			  "tl_pubs.NotCod,"			// row[2]
			  "UNIX_TIMESTAMP(tl_pubs.TimePublish),"// row[3]
			  "tl_comments.Txt,"			// row[4]
			  "tl_comments.MedCod"			// row[5]
		   " FROM tl_pubs,tl_comments"
		   " WHERE tl_pubs.PubCod=%ld"
		   " AND tl_pubs.PubType=%u"
		   " AND tl_pubs.PubCod=tl_comments.PubCod",
		   PubCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/******************* Insert comment content in database **********************/
/*****************************************************************************/

void TL_DB_InsertCommContent (long PubCod,
			      const struct TL_Pst_PostContent *Content)
  {
   /***** Insert comment content in database *****/
   DB_QueryINSERT ("can not store comment content",
		   "INSERT INTO tl_comments"
		   " (PubCod,Txt,MedCod)"
		   " VALUES"
		   " (%ld,'%s',%ld)",
		   PubCod,
		   Content->Txt,
		   Content->Media.MedCod);
  }

/*****************************************************************************/
/**************** Get code of media associated to comment ********************/
/*****************************************************************************/

long TL_DB_GetMedCodFromComm (long PubCod)
  {
   return TL_DB_GetMedCod ("tl_comments","PubCod",PubCod);
  }

/*****************************************************************************/
/****************** Remove favs for comment from database ********************/
/*****************************************************************************/

void TL_DB_RemoveCommFavs (long PubCod)
  {
   /***** Remove favs for comment *****/
   DB_QueryDELETE ("can not remove favs for comment",
		   "DELETE FROM tl_comments_fav"
		   " WHERE PubCod=%ld",
		   PubCod);
  }

/*****************************************************************************/
/***************** Remove content of comment from database *******************/
/*****************************************************************************/

void TL_DB_RemoveCommContent (long PubCod)
  {
   /***** Remove content of comment *****/
   DB_QueryDELETE ("can not remove comment content",
		   "DELETE FROM tl_comments"
		   " WHERE PubCod=%ld",
		   PubCod);
  }

/*****************************************************************************/
/***************** Remove comment publication from database ******************/
/*****************************************************************************/

void TL_DB_RemoveCommPub (long PubCod)
  {
   /***** Remove comment publication *****/
   DB_QueryDELETE ("can not remove comment",
		   "DELETE FROM tl_pubs"
	           " WHERE PubCod=%ld"
	           " AND PublisherCod=%ld"	// Extra check: author
	           " AND PubType=%u",		// Extra check: it's a comment
		   PubCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*********** Remove all comments in all the notes of a given user ************/
/*****************************************************************************/

void TL_DB_RemoveAllCommsInAllNotesOf (long UsrCod)
  {
   /***** Remove all comments in all notes of the user *****/
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments"
	           " USING tl_notes,tl_pubs,tl_comments"
	           " WHERE tl_notes.UsrCod=%ld"
		   " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*********** Remove all comments made by a given user in any note ************/
/*****************************************************************************/

void TL_DB_RemoveAllCommsMadeBy (long UsrCod)
  {
   /***** Remove all comments made by this user in any note *****/
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tl_comments"
	           " USING tl_pubs,tl_comments"
	           " WHERE tl_pubs.PublisherCod=%ld"
	           " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*************** Get code of media associated to post/comment ****************/
/*****************************************************************************/

static long TL_DB_GetMedCod (const char *Table,const char *Field,long Cod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long MedCod = -1L;	// Default value

   /***** Get code of media associated to comment *****/
   if (DB_QuerySELECT (&mysql_res,"can not get media code",
		       "SELECT MedCod"	// row[0]
		       " FROM %s"
		       " WHERE %s=%ld",
		      Table,Field,Cod) == 1)   // Result should have a unique row
     {
      /* Get media code */
      row = mysql_fetch_row (mysql_res);
      MedCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return MedCod;
  }

/*****************************************************************************/
/******* Create temporary table and subquery with potential publishers *******/
/*****************************************************************************/

void TL_DB_CreateSubQueryPublishers (const struct TL_Timeline *Timeline,
                                     struct TL_Pub_SubQueries *SubQueries)
  {
   /***** Create temporary table and subquery with potential publishers *****/
   switch (Timeline->UsrOrGbl)
     {
      case TL_Usr_TIMELINE_USR:		// Show the timeline of a user
	 SubQueries->TablePublishers = "";
	 sprintf (SubQueries->Publishers,"tl_pubs.PublisherCod=%ld AND ",
	          Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case TL_Usr_TIMELINE_GBL:		// Show the global timeline
	 switch (Timeline->Who)
	   {
	    case Usr_WHO_ME:		// Show my timeline
	       SubQueries->TablePublishers = "";
	       snprintf (SubQueries->Publishers,sizeof (SubQueries->Publishers),
	                 "tl_pubs.PublisherCod=%ld AND ",
	                 Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
	    case Usr_WHO_FOLLOWED:	// Show the timeline of the users I follow
	       Fol_CreateTmpTableMeAndUsrsIFollow ();
	       SubQueries->TablePublishers = ",fol_tmp_me_and_followed";
	       Str_Copy (SubQueries->Publishers,
			 "tl_pubs.PublisherCod=fol_tmp_me_and_followed.UsrCod AND ",
			 sizeof (SubQueries->Publishers) - 1);
	       break;
	    case Usr_WHO_ALL:		// Show the timeline of all users
	       SubQueries->TablePublishers = "";
	       SubQueries->Publishers[0] = '\0';
	       break;
	    default:
	       Lay_WrongWhoExit ();
	       break;
	   }
	 break;
     }
  }

/*****************************************************************************/
/********* Create subquery to get only notes not present in timeline *********/
/*****************************************************************************/

void TL_DB_CreateSubQueryAlreadyExists (const struct TL_Timeline *Timeline,
                                        struct TL_Pub_SubQueries *SubQueries)
  {
   static const char *Table[TL_NUM_WHAT_TO_GET] =
     {
      [TL_GET_RECENT_TIMELINE] = "tl_tmp_just_retrieved_notes",	// Avoid notes just retrieved
      [TL_GET_ONLY_NEW_PUBS  ] = "tl_tmp_just_retrieved_notes",	// Avoid notes just retrieved
      [TL_GET_ONLY_OLD_PUBS  ] = "tl_tmp_visible_timeline",	// Avoid notes already shown
     };

   snprintf (SubQueries->AlreadyExists,sizeof (SubQueries->AlreadyExists),
	     " tl_pubs.NotCod NOT IN (SELECT NotCod FROM %s)",
	     Table[Timeline->WhatToGet]);
  }

/*****************************************************************************/
/***** Create subqueries with range of publications to get from tl_pubs ******/
/*****************************************************************************/

void TL_DB_CreateSubQueryRangeBottom (const struct TL_Pub_RangePubsToGet *RangePubsToGet,
                                      struct TL_Pub_SubQueries *SubQueries)
  {
   if (RangePubsToGet->Bottom > 0)
      sprintf (SubQueries->RangeBottom,"tl_pubs.PubCod>%ld AND ",
	       RangePubsToGet->Bottom);
   else
      SubQueries->RangeBottom[0] = '\0';
  }

void TL_DB_CreateSubQueryRangeTop (const struct TL_Pub_RangePubsToGet *RangePubsToGet,
                                   struct TL_Pub_SubQueries *SubQueries)
  {
   if (RangePubsToGet->Top > 0)
      sprintf (SubQueries->RangeTop,"tl_pubs.PubCod<%ld AND ",
	       RangePubsToGet->Top);
   else
      SubQueries->RangeTop[0] = '\0';
  }

/*****************************************************************************/
/******************** Select the most recent publication *********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_SelectTheMostRecentPub (const struct TL_Pub_SubQueries *SubQueries,
                                       MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get publication",
		   "SELECT tl_pubs.PubCod,"		// row[0]
			  "tl_pubs.NotCod,"		// row[1]
			  "tl_pubs.PublisherCod,"	// row[2]
			  "tl_pubs.PubType"		// row[3]
		   " FROM tl_pubs%s"
		   " WHERE %s%s%s%s"
		   " ORDER BY tl_pubs.PubCod DESC LIMIT 1",
		   SubQueries->TablePublishers,
		   SubQueries->RangeBottom,
		   SubQueries->RangeTop,
		   SubQueries->Publishers,
		   SubQueries->AlreadyExists);
  }

/*****************************************************************************/
/****************** Get data of publication using its code *******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetDataOfPubByCod (long PubCod,MYSQL_RES **mysql_res)
  {
   /***** Trivial check: publication code should be > 0 *****/
   if (PubCod <= 0)
      return 0;

   /***** Get data of note from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of publication",
		   "SELECT PubCod,"		// row[0]
		          "NotCod,"		// row[1]
			  "PublisherCod,"	// row[2]
			  "PubType"		// row[3]
		   " FROM tl_pubs WHERE PubCod=%ld",
		   PubCod);
  }

/*****************************************************************************/
/*********************** Get code of note of a publication *******************/
/*****************************************************************************/

long TL_DB_GetNotCodFromPubCod (long PubCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long NotCod = -1L;	// Default value

   /***** Get code of note from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get code of note",
		       "SELECT NotCod FROM tl_pubs"
		       " WHERE PubCod=%ld",
		       PubCod) == 1)   // Result should have a unique row
     {
      /* Get code of note */
      row = mysql_fetch_row (mysql_res);
      NotCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NotCod;
  }

/*****************************************************************************/
/************* Get last/first publication code stored in session *************/
/*****************************************************************************/
// FieldName can be:
// "LastPubCod"
// "FirstPubCod"

long TL_DB_GetPubCodFromSession (const char *FieldName)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;

   /***** Get last publication code from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get publication code from session",
		       "SELECT %s FROM sessions"
		       " WHERE SessionId='%s'",
		       FieldName,Gbl.Session.Id) == 1)
     {
      /***** Get last publication code *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%ld",&PubCod) != 1)
	 PubCod = 0;
     }
   else
      PubCod = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PubCod;
  }

/*****************************************************************************/
/****************** Get number of publications from a user *******************/
/*****************************************************************************/

unsigned long TL_DB_GetNumPubsUsr (long UsrCod)
  {
   /***** Get number of posts from a user from database *****/
   return DB_QueryCOUNT ("can not get number of publications from a user",
			 "SELECT COUNT(*) FROM tl_pubs"
			 " WHERE PublisherCod=%ld",
			 UsrCod);
  }

/*****************************************************************************/
/********************* Insert new publication in database ********************/
/*****************************************************************************/
// Return just created publication code

long TL_DB_CreateNewPub (const struct TL_Pub_Publication *Pub)
  {
   /***** Insert new publication in database *****/
   return
   DB_QueryINSERTandReturnCode ("can not publish note/comment",
				"INSERT INTO tl_pubs"
				" (NotCod,PublisherCod,PubType,TimePublish)"
				" VALUES"
				" (%ld,%ld,%u,NOW())",
				Pub->NotCod,
				Pub->PublisherCod,
				(unsigned) Pub->PubType);
  }

/*****************************************************************************/
/************** Update first publication code stored in session **************/
/*****************************************************************************/

void TL_DB_UpdateFirstPubCodInSession (long FirstPubCod)
  {
   DB_QueryUPDATE ("can not update first publication code into session",
		   "UPDATE sessions"
		   " SET FirstPubCod=%ld"
		   " WHERE SessionId='%s'",
		   FirstPubCod,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/*************** Update last publication code stored in session **************/
/*****************************************************************************/

void TL_DB_UpdateLastPubCodInSession (void)
  {
   DB_QueryUPDATE ("can not update last publication code into session",
		   "UPDATE sessions"
		   " SET LastPubCod="
			"(SELECT IFNULL(MAX(PubCod),0)"
			" FROM tl_pubs)"	// The most recent publication
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/********* Update first and last publication codes stored in session *********/
/*****************************************************************************/

void TL_DB_UpdateFirstLastPubCodsInSession (long FirstPubCod)
  {
   DB_QueryUPDATE ("can not update first/last publication codes into session",
		   "UPDATE sessions"
		   " SET FirstPubCod=%ld,"
			"LastPubCod="
			"(SELECT IFNULL(MAX(PubCod),0)"
			" FROM tl_pubs)"	// The most recent publication
		   " WHERE SessionId='%s'",
		   FirstPubCod,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/************** Remove all publications published by any user ****************/
/************** related to notes authored by a given user     ****************/
/*****************************************************************************/

void TL_DB_RemoveAllPubsPublishedByAnyUsrOfNotesAuthoredBy (long UsrCod)
  {
   /***** Remove all publications (original notes, shared notes, comments)
          published by any user
          and related to notes authored by this user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tl_pubs"
                   " USING tl_notes,tl_pubs"
	           " WHERE tl_notes.UsrCod=%ld"
                   " AND tl_notes.NotCod=tl_pubs.NotCod",
		   UsrCod);
  }

/*****************************************************************************/
/**************** Remove all publications published by a user ****************/
/*****************************************************************************/

void TL_DB_RemoveAllPubsPublishedBy (long UsrCod)
  {
   /***** Remove all publications published by the user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tl_pubs WHERE PublisherCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/****************** Check if a user has favourited a note ********************/
/*****************************************************************************/

bool TL_DB_CheckIfFavedByUsr (TL_Fav_WhatToFav_t WhatToFav,
                              long Cod,long UsrCod)
  {
   /***** Check if a user has favourited a note/comment from database *****/
   return (DB_QueryCOUNT ("can not check if a user has favourited",
			  "SELECT COUNT(*) FROM %s"
			  " WHERE %s=%ld AND UsrCod=%ld",
			  TL_DB_Table[WhatToFav],
			  TL_DB_Field[WhatToFav],Cod,UsrCod) != 0);
  }

/*****************************************************************************/
/********* Get number of times a note/comment has been favourited ************/
/*****************************************************************************/

unsigned TL_DB_GetNumTimesHasBeenFav (TL_Fav_WhatToFav_t WhatToFav,
                                      long Cod,long UsrCod)
  {
   /***** Get number of times (users) a note/comment has been favourited *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of times has been favourited",
		  "SELECT COUNT(*) FROM %s"
		  " WHERE %s=%ld"
		  " AND UsrCod<>%ld",	// Extra check
		  TL_DB_Table[WhatToFav],
		  TL_DB_Field[WhatToFav],Cod,
		  UsrCod);		// The author
  }

/*****************************************************************************/
/******* Get list of users who have marked a note/comment as favourite *******/
/*****************************************************************************/

unsigned TL_DB_GetListUsrsHaveFaved (TL_Fav_WhatToFav_t WhatToFav,
                                     long Cod,long UsrCod,
                                     unsigned MaxUsrs,
                                     MYSQL_RES **mysql_res)
  {
   /***** Get list of users who have marked a note/comment as favourite from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get favers",
		   "SELECT UsrCod"	// row[0]
		   " FROM %s"
		   " WHERE %s=%ld"
		   " AND UsrCod<>%ld"	// Extra check
		   " ORDER BY FavCod LIMIT %u",
		   TL_DB_Table[WhatToFav],
		   TL_DB_Field[WhatToFav],Cod,
		   UsrCod,
		   MaxUsrs);
  }

/*****************************************************************************/
/**************** Mark note/comment as favourite in database *****************/
/*****************************************************************************/

void TL_DB_MarkAsFav (TL_Fav_WhatToFav_t WhatToFav,long Cod)
  {
   /***** Insert in favourited in database *****/
   DB_QueryINSERT ("can not favourite comment",
		   "INSERT IGNORE INTO %s"
		   " (%s,UsrCod,TimeFav)"
		   " VALUES"
		   " (%ld,%ld,NOW())",
		   TL_DB_Table[WhatToFav],
		   TL_DB_Field[WhatToFav],
		   Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************** Unmark note/comment as favourite in database ****************/
/*****************************************************************************/

void TL_DB_UnmarkAsFav (TL_Fav_WhatToFav_t WhatToFav,long Cod)
  {
   /***** Delete the mark as favourite from database *****/
   DB_QueryDELETE ("can not unfavourite",
		   "DELETE FROM %s"
		   " WHERE %s=%ld AND UsrCod=%ld",
		   TL_DB_Table[WhatToFav],
		   TL_DB_Field[WhatToFav],Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** Remove all favs made by a given user to any comment **************/
/*****************************************************************************/

void TL_DB_RemoveAllFavsMadeByUsr (TL_Fav_WhatToFav_t WhatToFav,long UsrCod)
  {
   /* Remove all favs made by this user to any note/comment */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM %s WHERE UsrCod=%ld",
		   TL_DB_Table[WhatToFav],UsrCod);
  }

/*****************************************************************************/
/************ Remove all favs to notes/comments of a given user **************/
/*****************************************************************************/

void TL_DB_RemoveAllFavsToPubsBy (TL_Fav_WhatToFav_t WhatToFav,long UsrCod)
  {
   /***** Remove all favs to notes/comments of this user *****/
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM %s"
	           " USING tl_pubs,%s"
	           " WHERE tl_pubs.PublisherCod=%ld"	// Author of the comment
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=%s.PubCod",
	           TL_DB_Table[WhatToFav],
	           TL_DB_Table[WhatToFav],
		   UsrCod,
		   (unsigned) TL_DB_PubType[WhatToFav],
		   TL_DB_Table[WhatToFav]);
  }

/*****************************************************************************/
/*** Remove all favs to all comments in all notes authored by a given user ***/
/*****************************************************************************/

void TL_DB_RemoveAllFavsToAllCommsInAllNotesBy (long UsrCod)
  {
   /***** Remove all favs to all comments
          in all notes authored by this user *****/
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tl_comments_fav"
	           " USING tl_notes,tl_pubs,tl_comments_fav"
	           " WHERE tl_notes.UsrCod=%ld"		// Author of the note
	           " AND tl_notes.NotCod=tl_pubs.NotCod"
                   " AND tl_pubs.PubType=%u"
	           " AND tl_pubs.PubCod=tl_comments_fav.PubCod",
		   UsrCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/****************** Check if a user has published a note *********************/
/*****************************************************************************/

bool TL_DB_CheckIfNoteIsSharedByUsr (long NotCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user has shared a note",
			  "SELECT COUNT(*) FROM tl_pubs"
			  " WHERE NotCod=%ld"
			  " AND PublisherCod=%ld"
			  " AND PubType=%u",
			  NotCod,
			  UsrCod,
			  (unsigned) TL_Pub_SHARED_NOTE) != 0);
  }

/*****************************************************************************/
/********** Get number of times a note has been shared in timeline ***********/
/*****************************************************************************/

unsigned TL_DB_GetNumTimesANoteHasBeenShared (struct TL_Not_Note *Not)
  {
   /***** Get number of times (users) this note has been shared *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of times a note has been shared",
		  "SELECT COUNT(*) FROM tl_pubs"
		  " WHERE NotCod=%ld"
		  " AND PublisherCod<>%ld"
		  " AND PubType=%u",
		  Not->NotCod,
		  Not->UsrCod,	// Author of the note
		  (unsigned) TL_Pub_SHARED_NOTE);
  }

/*****************************************************************************/
/***************** Get list of users who have shared a note ******************/
/*****************************************************************************/

unsigned TL_DB_GetListUsrsHaveShared (long NotCod,long UsrCod,
                                      unsigned MaxUsrs,
                                      MYSQL_RES **mysql_res)
  {
   /***** Get list of users who have shared a note from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users",
		   "SELECT PublisherCod"	// row[0]
		   " FROM tl_pubs"
		   " WHERE NotCod=%ld"
		   " AND PublisherCod<>%ld"
		   " AND PubType=%u"
		   " ORDER BY PubCod LIMIT %u",
		   NotCod,
		   UsrCod,
		   (unsigned) TL_Pub_SHARED_NOTE,
		   MaxUsrs);
  }

/*****************************************************************************/
/****************** Remove shared publication from database ******************/
/*****************************************************************************/

void TL_DB_RemoveSharedPub (long NotCod)
  {
   /***** Remove shared publication *****/
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tl_pubs"
		   " WHERE NotCod=%ld"
		   " AND PublisherCod=%ld"
		   " AND PubType=%u",	// Extra check: shared note
		   NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) TL_Pub_SHARED_NOTE);
  }
