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

static const char *Tml_DB_TableFav[Tml_Usr_NUM_FAV_SHA] =
  {
   [Tml_Usr_FAV_UNF_NOTE] = "tml_notes_fav",
   [Tml_Usr_FAV_UNF_COMM] = "tml_comments_fav",
   [Tml_Usr_SHA_UNS_NOTE] = NULL,		// Not used
  };
static const char *Tml_DB_FieldFav[Tml_Usr_NUM_FAV_SHA] =
  {
   [Tml_Usr_FAV_UNF_NOTE] = "NotCod",
   [Tml_Usr_FAV_UNF_COMM] = "PubCod",
   [Tml_Usr_SHA_UNS_NOTE] = NULL,		// Not used
  };
static Tml_Pub_PubType_t Tml_DB_PubTypeFav[Tml_Usr_NUM_FAV_SHA] =
  {
   [Tml_Usr_FAV_UNF_NOTE] = Tml_Pub_ORIGINAL_NOTE,
   [Tml_Usr_FAV_UNF_COMM] = Tml_Pub_COMMENT_TO_NOTE,
   [Tml_Usr_SHA_UNS_NOTE] = Tml_Pub_UNKNOWN,	// Not used
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

static long Tml_DB_GetMedCod (const char *Table,const char *Field,long Cod);

/*****************************************************************************/
/********************* Get which users to show in timeline *******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetWho (MYSQL_RES **mysql_res)
  {
   /***** Get which users from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get which timeline users",
		   "SELECT TimelineUsrs"	// row[0]
		    " FROM usr_last"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Get which users to show in timeline *******************/
/*****************************************************************************/

void Tml_DB_UpdateWho (Usr_Who_t Who)
  {
   /***** Update which users in database *****/
   DB_QueryUPDATE ("can not update which timeline users",
		   "UPDATE usr_last"
		     " SET TimelineUsrs=%u"
		   " WHERE UsrCod=%ld",
		   (unsigned) Who,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Get data of note using its code ***********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetDataOfNoteByCod (long NotCod,MYSQL_RES **mysql_res)
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
		    " FROM tml_notes"
		   " WHERE NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/*************** Get code of publication of the original note ****************/
/*****************************************************************************/

long Tml_DB_GetPubCodOfOriginalNote (long NotCod)
  {
   /***** Get code of publication of the original note *****/
   return DB_QuerySELECTCode ("can not get code of publication",
			      "SELECT PubCod"
			       " FROM tml_pubs"
			      " WHERE NotCod=%ld"
			        " AND PubType=%u",
			      NotCod,
			      (unsigned) Tml_Pub_ORIGINAL_NOTE);
  }

/*****************************************************************************/
/***************************** Create a new note *****************************/
/*****************************************************************************/
// Returns code of note just created

long Tml_DB_CreateNewNote (Tml_Not_NoteType_t NoteType,long Cod,
                           long PublisherCod,long HieCod)
  {
   return
   DB_QueryINSERTandReturnCode ("can not create new note",
				"INSERT INTO tml_notes"
				" (NoteType,Cod,UsrCod,HieCod,"
				  "Unavailable,TimeNote)"
				" VALUES"
				" (%u,%ld,%ld,%ld,"
				  "'N',NOW())",
				(unsigned) NoteType,	// Post, file, exam, notice, forum
				Cod,			// Post, file, exam, notice, forum code
				PublisherCod,		// Publisher code
				HieCod);		// Where in hierarchy
  }

/*****************************************************************************/
/************************* Mark a note as unavailable ************************/
/*****************************************************************************/

void Tml_DB_MarkNoteAsUnavailable (Tml_Not_NoteType_t NoteType,long Cod)
  {
   /***** Mark note as unavailable *****/
   DB_QueryUPDATE ("can not mark note as unavailable",
		   "UPDATE tml_notes SET Unavailable='Y'"
		   " WHERE NoteType=%u AND Cod=%ld",
		   (unsigned) NoteType,Cod);
  }

/*****************************************************************************/
/***** Mark possible notes involving children of a folder as unavailable *****/
/*****************************************************************************/

void Tml_DB_MarkNotesChildrenOfFolderAsUnavailable (Tml_Not_NoteType_t NoteType,
                                                    Brw_FileBrowser_t FileBrowser,
                                                    long Cod,const char *Path)
  {
   /***** Mark notes as unavailable *****/
   DB_QueryUPDATE ("can not mark notes as unavailable",
		   "UPDATE tml_notes"
		     " SET Unavailable='Y'"
		   " WHERE NoteType=%u"
		     " AND Cod IN"
			 " (SELECT FilCod"
			    " FROM brw_files"
			   " WHERE FileBrowser=%u"
			     " AND Cod=%ld"
			     " AND Path LIKE '%s/%%'"
			     " AND Public='Y')",	// Only public files
		   (unsigned) NoteType,
		   (unsigned) FileBrowser,Cod,
		   Path);
  }

/*****************************************************************************/
/******* Create temporary tables used to not get notes already shown *********/
/*****************************************************************************/

void Tml_DB_CreateTmpTableJustRetrievedNotes (void)
  {
   /***** Create temporary table with notes just retrieved *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tml_tmp_just_retrieved_notes "
	     "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
	     " ENGINE=MEMORY");
  }

void Tml_DB_CreateTmpTableVisibleTimeline (void)
  {
   /***** Create temporary table with all notes visible in timeline *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tml_tmp_visible_timeline "
	     "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
	     " ENGINE=MEMORY"
	     " SELECT NotCod"
	       " FROM tml_timelines"
	      " WHERE SessionId='%s'",
	     Gbl.Session.Id);
  }

/*****************************************************************************/
/**** Insert note in temporary tables used to not get notes already shown ****/
/*****************************************************************************/

void Tml_DB_InsertNoteInJustRetrievedNotes (long NotCod)
  {
   /* Insert note in temporary table with just retrieved notes.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tml_tmp_just_retrieved_notes"
		   " SET NotCod=%ld",
		   NotCod);
  }

void Tml_DB_InsertNoteInVisibleTimeline (long NotCod)
  {
   /* Insert note in temporary table with visible timeline.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tml_tmp_visible_timeline"
		   " SET NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/****** Add just retrieved notes to current timeline for this session ********/
/*****************************************************************************/

void Tml_DB_AddNotesJustRetrievedToVisibleTimelineOfSession (void)
  {
   /* tml_timelines contains the distinct notes in timeline of each open session:
mysql> SELECT SessionId,COUNT(*) FROM tml_timelines GROUP BY SessionId;
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
		   "INSERT IGNORE INTO tml_timelines"
	           " (SessionId,NotCod)"
	           " SELECT '%s',"
	                    "NotCod"
	             " FROM tml_tmp_just_retrieved_notes",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/******** Drop temporary tables used to not get notes already shown **********/
/*****************************************************************************/

void Tml_DB_DropTmpTableJustRetrievedNotes (void)
  {
   /***** Drop temporary table with notes just retrieved *****/
   DB_Query ("can not remove temporary table",
	     "DROP TEMPORARY TABLE IF EXISTS tml_tmp_just_retrieved_notes");
  }

void Tml_DB_DropTmpTableVisibleTimeline (void)
  {
   /***** Drop temporary table with all notes visible in timeline *****/
   DB_Query ("can not remove temporary table",
             "DROP TEMPORARY TABLE IF EXISTS tml_tmp_visible_timeline");
  }

/*****************************************************************************/
/******************* Clear unused old timelines in database ******************/
/*****************************************************************************/

void Tml_DB_ClearOldTimelinesNotesFromDB (void)
  {
   /***** Remove timelines for expired sessions *****/
   DB_QueryDELETE ("can not remove old timelines",
		   "DELETE LOW_PRIORITY FROM tml_timelines"
                   " WHERE SessionId NOT IN"
                         " (SELECT SessionId"
                            " FROM ses_sessions)");
  }

/*****************************************************************************/
/***************** Clear timeline for a session in database ******************/
/*****************************************************************************/

void Tml_DB_ClearTimelineNotesOfSessionFromDB (void)
  {
   /***** Remove timeline for a session *****/
   DB_QueryDELETE ("can not remove timeline",
		   "DELETE FROM tml_timelines"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/*************************** Remove favs for a note **************************/
/*****************************************************************************/

void Tml_DB_RemoveNoteFavs (long NotCod)
  {
   /***** Remove favs for note *****/
   DB_QueryDELETE ("can not remove favs for note",
		   "DELETE FROM tml_notes_fav"
		   " WHERE NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/******************** Remove all publications of this note *******************/
/*****************************************************************************/

void Tml_DB_RemoveNotePubs (long NotCod)
  {
   /***** Remove all publications of this note *****/
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tml_pubs"
		   " WHERE NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/******************* Remove note publication from database *******************/
/*****************************************************************************/

void Tml_DB_RemoveNote (long NotCod)
  {
   /***** Remove note *****/
   DB_QueryDELETE ("can not remove a note",
		   "DELETE FROM tml_notes"
	           " WHERE NotCod=%ld"
	           " AND UsrCod=%ld",		// Extra check: author
		   NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Remove all notes created by a user *********************/
/*****************************************************************************/

void Tml_DB_RemoveAllNotesUsr (long UsrCod)
  {
   /***** Remove all notes created by user *****/
   DB_QueryDELETE ("can not remove notes",
		   "DELETE FROM tml_notes WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/********************* Get data of post using its code ***********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetPostByCod (long PstCod,MYSQL_RES **mysql_res)
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
		    " FROM tml_posts"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/***************** Get code of media associated to post **********************/
/*****************************************************************************/

long Tml_DB_GetMedCodFromPost (long PstCod)
  {
   return Tml_DB_GetMedCod ("tml_posts","PstCod",PstCod);
  }

/*****************************************************************************/
/********************* Insert post content in database ***********************/
/*****************************************************************************/
// Returns code of just created post

long Tml_DB_CreateNewPost (const struct Tml_Pst_Content *Content)
  {
   /***** Insert post content in the database *****/
   return
   DB_QueryINSERTandReturnCode ("can not create post",
				"INSERT INTO tml_posts"
				" (Txt,MedCod)"
				" VALUES"
				" ('%s',%ld)",
				Content->Txt,
				Content->Media.MedCod);
  }

/*****************************************************************************/
/************************* Remove post from database *************************/
/*****************************************************************************/

void Tml_DB_RemovePost (long PstCod)
  {
   /***** Remove post *****/
   DB_QueryDELETE ("can not remove a post",
		   "DELETE FROM tml_posts"
		   " WHERE PstCod=%ld",
		   PstCod);
  }

/*****************************************************************************/
/************************* Remove all posts of a user ************************/
/*****************************************************************************/

void Tml_DB_RemoveAllPostsUsr (long UsrCod)
  {
   /***** Remove all posts of the user *****/
   DB_QueryDELETE ("can not remove posts",
		   "DELETE FROM tml_posts"
		   " WHERE PstCod IN"
		   " (SELECT Cod"
		      " FROM tml_notes"
	             " WHERE UsrCod=%ld"
	               " AND NoteType=%u)",
		   UsrCod,(unsigned) TL_NOTE_POST);
  }

/*****************************************************************************/
/********************* Get number of comments in a note **********************/
/*****************************************************************************/

unsigned Tml_DB_GetNumCommsInNote (long NotCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of comments in a note",
		  "SELECT COUNT(*)"
		   " FROM tml_pubs"
		  " WHERE NotCod=%ld"
		    " AND PubType=%u",
		  NotCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/************** Get publication codes of comments of a note from database *****************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetComms (long NotCod,MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT PubCod"	// row[0]
		    " FROM tml_pubs"
		   " WHERE NotCod=%ld"
		     " AND PubType=%u",
		   NotCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/************** Get initial comments of a note from database *****************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetInitialComms (long NotCod,unsigned NumInitialCommsToGet,
				 MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT tml_pubs.PubCod,"				// row[0]
			  "tml_pubs.PublisherCod,"			// row[1]
			  "tml_pubs.NotCod,"				// row[2]
			  "UNIX_TIMESTAMP(tml_pubs.TimePublish),"	// row[3]
			  "tml_comments.Txt,"				// row[4]
			  "tml_comments.MedCod"				// row[5]
		    " FROM tml_pubs,tml_comments"
		   " WHERE tml_pubs.NotCod=%ld"
		     " AND tml_pubs.PubType=%u"
		     " AND tml_pubs.PubCod=tml_comments.PubCod"
		   " ORDER BY tml_pubs.PubCod"
		   " LIMIT %lu",
		   NotCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE,
		   NumInitialCommsToGet);
  }

/*****************************************************************************/
/*************** Get final comments of a note from database ******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetFinalComms (long NotCod,unsigned NumFinalCommsToGet,
			       MYSQL_RES **mysql_res)
  {
   /***** Get final comments of a note from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT *"
		    " FROM (SELECT tml_pubs.PubCod,"				// row[0]
				  "tml_pubs.PublisherCod,"			// row[1]
				  "tml_pubs.NotCod,"				// row[2]
				  "UNIX_TIMESTAMP(tml_pubs.TimePublish),"	// row[3]
				  "tml_comments.Txt,"				// row[4]
				  "tml_comments.MedCod"				// row[5]
			    " FROM tml_pubs,tml_comments"
			   " WHERE tml_pubs.NotCod=%ld"
			     " AND tml_pubs.PubType=%u"
			     " AND tml_pubs.PubCod=tml_comments.PubCod"
			   " ORDER BY tml_pubs.PubCod DESC"
			   " LIMIT %u) AS comments"
		  " ORDER BY PubCod",
		  NotCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE,
		  NumFinalCommsToGet);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetDataOfCommByCod (long PubCod,MYSQL_RES **mysql_res)
  {
   /***** Trivial check: publication code should be > 0 *****/
   if (PubCod <= 0)
      return 0;

   /***** Get data of comment from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of comment",
		   "SELECT tml_pubs.PubCod,"				// row[0]
			  "tml_pubs.PublisherCod,"			// row[1]
			  "tml_pubs.NotCod,"				// row[2]
			  "UNIX_TIMESTAMP(tml_pubs.TimePublish),"	// row[3]
			  "tml_comments.Txt,"				// row[4]
			  "tml_comments.MedCod"				// row[5]
		    " FROM tml_pubs,tml_comments"
		   " WHERE tml_pubs.PubCod=%ld"
		     " AND tml_pubs.PubType=%u"
		     " AND tml_pubs.PubCod=tml_comments.PubCod",
		   PubCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/******************* Insert comment content in database **********************/
/*****************************************************************************/

void Tml_DB_InsertCommContent (long PubCod,
			       const struct Tml_Pst_Content *Content)
  {
   /***** Insert comment content in database *****/
   DB_QueryINSERT ("can not store comment content",
		   "INSERT INTO tml_comments"
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

long Tml_DB_GetMedCodFromComm (long PubCod)
  {
   return Tml_DB_GetMedCod ("tml_comments","PubCod",PubCod);
  }

/*****************************************************************************/
/****************** Remove favs for comment from database ********************/
/*****************************************************************************/

void Tml_DB_RemoveCommFavs (long PubCod)
  {
   /***** Remove favs for comment *****/
   DB_QueryDELETE ("can not remove favs for comment",
		   "DELETE FROM tml_comments_fav"
		   " WHERE PubCod=%ld",
		   PubCod);
  }

/*****************************************************************************/
/***************** Remove content of comment from database *******************/
/*****************************************************************************/

void Tml_DB_RemoveCommContent (long PubCod)
  {
   /***** Remove content of comment *****/
   DB_QueryDELETE ("can not remove comment content",
		   "DELETE FROM tml_comments"
		   " WHERE PubCod=%ld",
		   PubCod);
  }

/*****************************************************************************/
/***************** Remove comment publication from database ******************/
/*****************************************************************************/

void Tml_DB_RemoveCommPub (long PubCod)
  {
   /***** Remove comment publication *****/
   DB_QueryDELETE ("can not remove comment",
		   "DELETE FROM tml_pubs"
	           " WHERE PubCod=%ld"
	           " AND PublisherCod=%ld"	// Extra check: author
	           " AND PubType=%u",		// Extra check: it's a comment
		   PubCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Tml_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*********** Remove all comments in all the notes of a given user ************/
/*****************************************************************************/

void Tml_DB_RemoveAllCommsInAllNotesOf (long UsrCod)
  {
   /***** Remove all comments in all notes of the user *****/
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tml_comments"
	           " USING tml_notes,tml_pubs,tml_comments"
	           " WHERE tml_notes.UsrCod=%ld"
		   " AND tml_notes.NotCod=tml_pubs.NotCod"
                   " AND tml_pubs.PubType=%u"
	           " AND tml_pubs.PubCod=tml_comments.PubCod",
		   UsrCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*********** Remove all comments made by a given user in any note ************/
/*****************************************************************************/

void Tml_DB_RemoveAllCommsMadeBy (long UsrCod)
  {
   /***** Remove all comments made by this user in any note *****/
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tml_comments"
	           " USING tml_pubs,tml_comments"
	           " WHERE tml_pubs.PublisherCod=%ld"
	           " AND tml_pubs.PubType=%u"
	           " AND tml_pubs.PubCod=tml_comments.PubCod",
		   UsrCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*************** Get code of media associated to post/comment ****************/
/*****************************************************************************/

static long Tml_DB_GetMedCod (const char *Table,const char *Field,long Cod)
  {
   /***** Get code of media associated to comment *****/
   return DB_QuerySELECTCode ("can not get media code",
			      "SELECT MedCod"	// row[0]
			       " FROM %s"
			      " WHERE %s=%ld",
			      Table,
			      Field,Cod);
  }

/*****************************************************************************/
/******* Create temporary table and subquery with potential publishers *******/
/*****************************************************************************/

void Tml_DB_CreateSubQueryPublishers (const struct Tml_Timeline *Timeline,
                                      struct Tml_Pub_SubQueries *SubQueries)
  {
   /***** Create temporary table and subquery with potential publishers *****/
   switch (Timeline->UsrOrGbl)
     {
      case Tml_Usr_TIMELINE_USR:		// Show the timeline of a user
	 SubQueries->TablePublishers = "";
	 sprintf (SubQueries->Publishers,"tml_pubs.PublisherCod=%ld AND ",
	          Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case Tml_Usr_TIMELINE_GBL:		// Show the global timeline
	 switch (Timeline->Who)
	   {
	    case Usr_WHO_ME:		// Show my timeline
	       SubQueries->TablePublishers = "";
	       snprintf (SubQueries->Publishers,sizeof (SubQueries->Publishers),
	                 "tml_pubs.PublisherCod=%ld AND ",
	                 Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
	    case Usr_WHO_FOLLOWED:	// Show the timeline of the users I follow
	       Fol_CreateTmpTableMeAndUsrsIFollow ();
	       SubQueries->TablePublishers = ",fol_tmp_me_and_followed";
	       Str_Copy (SubQueries->Publishers,
			 "tml_pubs.PublisherCod=fol_tmp_me_and_followed.UsrCod AND ",
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

void Tml_DB_CreateSubQueryAlreadyExists (const struct Tml_Timeline *Timeline,
                                         struct Tml_Pub_SubQueries *SubQueries)
  {
   static const char *Table[Tml_NUM_WHAT_TO_GET] =
     {
      [Tml_GET_RECENT_TIMELINE] = "tml_tmp_just_retrieved_notes",	// Avoid notes just retrieved
      [Tml_GET_ONLY_NEW_PUBS  ] = "tml_tmp_just_retrieved_notes",	// Avoid notes just retrieved
      [Tml_GET_ONLY_OLD_PUBS  ] = "tml_tmp_visible_timeline",		// Avoid notes already shown
     };

   snprintf (SubQueries->AlreadyExists,sizeof (SubQueries->AlreadyExists),
	     " tml_pubs.NotCod NOT IN"
	     " (SELECT NotCod"
	        " FROM %s)",
	     Table[Timeline->WhatToGet]);
  }

/*****************************************************************************/
/***** Create subqueries with range of publications to get from tml_pubs *****/
/*****************************************************************************/

void Tml_DB_CreateSubQueryRangeBottom (long Bottom,struct Tml_Pub_SubQueries *SubQueries)
  {
   if (Bottom > 0)
      sprintf (SubQueries->RangeBottom,"tml_pubs.PubCod>%ld AND ",Bottom);
   else
      SubQueries->RangeBottom[0] = '\0';
  }

void Tml_DB_CreateSubQueryRangeTop (long Top,struct Tml_Pub_SubQueries *SubQueries)
  {
   if (Top > 0)
      sprintf (SubQueries->RangeTop,"tml_pubs.PubCod<%ld AND ",Top);
   else
      SubQueries->RangeTop[0] = '\0';
  }

/*****************************************************************************/
/******************** Select the most recent publication *********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_SelectTheMostRecentPub (const struct Tml_Pub_SubQueries *SubQueries,
                                        MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get publication",
		   "SELECT tml_pubs.PubCod,"		// row[0]
			  "tml_pubs.NotCod,"		// row[1]
			  "tml_pubs.PublisherCod,"	// row[2]
			  "tml_pubs.PubType"		// row[3]
		    " FROM tml_pubs%s"
		   " WHERE %s%s%s%s"
		   " ORDER BY tml_pubs.PubCod"
		    " DESC LIMIT 1",
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

unsigned Tml_DB_GetDataOfPubByCod (long PubCod,MYSQL_RES **mysql_res)
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
		    " FROM tml_pubs"
		   " WHERE PubCod=%ld",
		   PubCod);
  }

/*****************************************************************************/
/*********************** Get code of note of a publication *******************/
/*****************************************************************************/

long Tml_DB_GetNotCodFromPubCod (long PubCod)
  {
   /***** Get code of note from database *****/
   return DB_QuerySELECTCode ("can not get code of note",
			      "SELECT NotCod"
			       " FROM tml_pubs"
			      " WHERE PubCod=%ld",
			      PubCod);
  }

/*****************************************************************************/
/************* Get last/first publication code stored in session *************/
/*****************************************************************************/
// FieldName can be:
// "LastPubCod"
// "FirstPubCod"

long Tml_DB_GetPubCodFromSession (const char *FieldName)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;

   /***** Get last publication code from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get publication code from session",
		       "SELECT %s"		// row[0]
		        " FROM ses_sessions"
		       " WHERE SessionId='%s'",
		       FieldName,Gbl.Session.Id) == 1)
     {
      /* Get last publication code */
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

unsigned Tml_DB_GetNumPubsUsr (long UsrCod)
  {
   /***** Get number of posts from a user from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of publications from a user",
		  "SELECT COUNT(*)"
		   " FROM tml_pubs"
		  " WHERE PublisherCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/********************* Insert new publication in database ********************/
/*****************************************************************************/
// Return just created publication code

long Tml_DB_CreateNewPub (const struct Tml_Pub_Publication *Pub)
  {
   /***** Insert new publication in database *****/
   return
   DB_QueryINSERTandReturnCode ("can not publish note/comment",
				"INSERT INTO tml_pubs"
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

void Tml_DB_UpdateFirstPubCodInSession (long FirstPubCod)
  {
   DB_QueryUPDATE ("can not update first publication code into session",
		   "UPDATE ses_sessions"
		     " SET FirstPubCod=%ld"
		   " WHERE SessionId='%s'",
		   FirstPubCod,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/*************** Update last publication code stored in session **************/
/*****************************************************************************/

void Tml_DB_UpdateLastPubCodInSession (void)
  {
   DB_QueryUPDATE ("can not update last publication code into session",
		   "UPDATE ses_sessions"
		     " SET LastPubCod="
			  "(SELECT IFNULL(MAX(PubCod),0)"
			    " FROM tml_pubs)"	// The most recent publication
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/********* Update first and last publication codes stored in session *********/
/*****************************************************************************/

void Tml_DB_UpdateFirstLastPubCodsInSession (long FirstPubCod)
  {
   DB_QueryUPDATE ("can not update first/last publication codes into session",
		   "UPDATE ses_sessions"
		     " SET FirstPubCod=%ld,"
			  "LastPubCod="
			  "(SELECT IFNULL(MAX(PubCod),0)"
			    " FROM tml_pubs)"	// The most recent publication
		   " WHERE SessionId='%s'",
		   FirstPubCod,
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/************** Remove all publications published by any user ****************/
/************** related to notes authored by a given user     ****************/
/*****************************************************************************/

void Tml_DB_RemoveAllPubsPublishedByAnyUsrOfNotesAuthoredBy (long UsrCod)
  {
   /***** Remove all publications (original notes, shared notes, comments)
          published by any user
          and related to notes authored by this user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tml_pubs"
                   " USING tml_notes,tml_pubs"
	           " WHERE tml_notes.UsrCod=%ld"
                   " AND tml_notes.NotCod=tml_pubs.NotCod",
		   UsrCod);
  }

/*****************************************************************************/
/**************** Remove all publications published by a user ****************/
/*****************************************************************************/

void Tml_DB_RemoveAllPubsPublishedBy (long UsrCod)
  {
   /***** Remove all publications published by the user *****/
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tml_pubs WHERE PublisherCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/****************** Check if a user has favourited a note ********************/
/*****************************************************************************/

bool Tml_DB_CheckIfFavedByUsr (Tml_Usr_FavSha_t FavSha,long Cod,long UsrCod)
  {
   /***** Check if a user has favourited a note/comment from database *****/
   return (DB_QueryCOUNT ("can not check if a user has favourited",
			  "SELECT COUNT(*)"
			   " FROM %s"
			  " WHERE %s=%ld"
			    " AND UsrCod=%ld",
			  Tml_DB_TableFav[FavSha],
			  Tml_DB_FieldFav[FavSha],Cod,UsrCod) != 0);
  }

/*****************************************************************************/
/********* Get number of times a note/comment has been favourited ************/
/*****************************************************************************/

unsigned Tml_DB_GetNumFavers (Tml_Usr_FavSha_t FavSha,long Cod,long UsrCod)
  {
   /***** Get number of times (users) a note/comment has been favourited *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of times has been favourited",
		  "SELECT COUNT(*)"
		   " FROM %s"
		  " WHERE %s=%ld"
		    " AND UsrCod<>%ld",	// Extra check
		  Tml_DB_TableFav[FavSha],
		  Tml_DB_FieldFav[FavSha],Cod,
		  UsrCod);		// The author
  }

/*****************************************************************************/
/******* Get list of users who have marked a note/comment as favourite *******/
/*****************************************************************************/

unsigned Tml_DB_GetFavers (Tml_Usr_FavSha_t FavSha,
                           long Cod,long UsrCod,unsigned MaxUsrs,
                           MYSQL_RES **mysql_res)
  {
   /***** Get list of users who have marked a note/comment as favourite from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get favers",
		   "SELECT UsrCod"	// row[0]
		    " FROM %s"
		   " WHERE %s=%ld"
		     " AND UsrCod<>%ld"	// Extra check
		   " ORDER BY FavCod"
		   " LIMIT %u",
		   Tml_DB_TableFav[FavSha],
		   Tml_DB_FieldFav[FavSha],Cod,
		   UsrCod,
		   MaxUsrs);
  }

/*****************************************************************************/
/**************** Mark note/comment as favourite in database *****************/
/*****************************************************************************/

void Tml_DB_MarkAsFav (Tml_Usr_FavSha_t FavSha,long Cod)
  {
   /***** Insert in favourited in database *****/
   DB_QueryINSERT ("can not favourite comment",
		   "INSERT IGNORE INTO %s"
		   " (%s,UsrCod,TimeFav)"
		   " VALUES"
		   " (%ld,%ld,NOW())",
		   Tml_DB_TableFav[FavSha],
		   Tml_DB_FieldFav[FavSha],
		   Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************** Unmark note/comment as favourite in database ****************/
/*****************************************************************************/

void Tml_DB_UnmarkAsFav (Tml_Usr_FavSha_t FavSha,long Cod)
  {
   /***** Delete the mark as favourite from database *****/
   DB_QueryDELETE ("can not unfavourite",
		   "DELETE FROM %s"
		   " WHERE %s=%ld AND UsrCod=%ld",
		   Tml_DB_TableFav[FavSha],
		   Tml_DB_FieldFav[FavSha],Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** Remove all favs made by a given user to any comment **************/
/*****************************************************************************/

void Tml_DB_RemoveAllFavsMadeByUsr (Tml_Usr_FavSha_t FavSha,long UsrCod)
  {
   /* Remove all favs made by this user to any note/comment */
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM %s WHERE UsrCod=%ld",
		   Tml_DB_TableFav[FavSha],UsrCod);
  }

/*****************************************************************************/
/************ Remove all favs to notes/comments of a given user **************/
/*****************************************************************************/

void Tml_DB_RemoveAllFavsToPubsBy (Tml_Usr_FavSha_t FavSha,long UsrCod)
  {
   /***** Remove all favs to notes/comments of this user *****/
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM %s"
	           " USING tml_pubs,%s"
	           " WHERE tml_pubs.PublisherCod=%ld"	// Author of the comment
                   " AND tml_pubs.PubType=%u"
	           " AND tml_pubs.PubCod=%s.PubCod",
	           Tml_DB_TableFav[FavSha],
	           Tml_DB_TableFav[FavSha],
		   UsrCod,
		   (unsigned) Tml_DB_PubTypeFav[FavSha],
		   Tml_DB_TableFav[FavSha]);
  }

/*****************************************************************************/
/*** Remove all favs to all comments in all notes authored by a given user ***/
/*****************************************************************************/

void Tml_DB_RemoveAllFavsToAllCommsInAllNotesBy (long UsrCod)
  {
   /***** Remove all favs to all comments
          in all notes authored by this user *****/
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tml_comments_fav"
	           " USING tml_notes,tml_pubs,tml_comments_fav"
	           " WHERE tml_notes.UsrCod=%ld"		// Author of the note
	           " AND tml_notes.NotCod=tml_pubs.NotCod"
                   " AND tml_pubs.PubType=%u"
	           " AND tml_pubs.PubCod=tml_comments_fav.PubCod",
		   UsrCod,(unsigned) Tml_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/****************** Check if a user has published a note *********************/
/*****************************************************************************/

bool Tml_DB_CheckIfSharedByUsr (long NotCod,long UsrCod)
  {
   return (DB_QueryCOUNT ("can not check if a user has shared a note",
			  "SELECT COUNT(*)"
			   " FROM tml_pubs"
			  " WHERE NotCod=%ld"
			    " AND PublisherCod=%ld"
			    " AND PubType=%u",
			  NotCod,
			  UsrCod,
			  (unsigned) Tml_Pub_SHARED_NOTE) != 0);
  }

/*****************************************************************************/
/********** Get number of times a note has been shared in timeline ***********/
/*****************************************************************************/

unsigned Tml_DB_GetNumSharers (long NotCod,long UsrCod)
  {
   /***** Get number of times (users) this note has been shared *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of times a note has been shared",
		  "SELECT COUNT(*)"
		   " FROM tml_pubs"
		  " WHERE NotCod=%ld"
		    " AND PublisherCod<>%ld"
		    " AND PubType=%u",
		  NotCod,
		  UsrCod,	// Author of the note
		  (unsigned) Tml_Pub_SHARED_NOTE);
  }

/*****************************************************************************/
/***************** Get list of users who have shared a note ******************/
/*****************************************************************************/

unsigned Tml_DB_GetSharers (long NotCod,long UsrCod,unsigned MaxUsrs,
                            MYSQL_RES **mysql_res)
  {
   /***** Get list of users who have shared a note from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users",
		   "SELECT PublisherCod"	// row[0]
		    " FROM tml_pubs"
		   " WHERE NotCod=%ld"
		     " AND PublisherCod<>%ld"
		     " AND PubType=%u"
		   " ORDER BY PubCod"
		   " LIMIT %u",
		   NotCod,
		   UsrCod,
		   (unsigned) Tml_Pub_SHARED_NOTE,
		   MaxUsrs);
  }

/*****************************************************************************/
/****************** Remove shared publication from database ******************/
/*****************************************************************************/

void Tml_DB_RemoveSharedPub (long NotCod)
  {
   /***** Remove shared publication *****/
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tml_pubs"
		   " WHERE NotCod=%ld"
		   " AND PublisherCod=%ld"
		   " AND PubType=%u",	// Extra check: shared note
		   NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Tml_Pub_SHARED_NOTE);
  }
