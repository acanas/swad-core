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
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static long TL_DB_GetMedCodFromPub (long PubCod,const char *DBTable);

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
		       NotCod,(unsigned) TL_Pub_ORIGINAL_NOTE) == 1)   // Result should have a unique row
     {
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

void TL_DB_AddNotesJustRetrievedToVisibleTimelineOfSession (char SessionId[Cns_BYTES_SESSION_ID + 1])
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
		   SessionId);
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

void TL_DB_RemoveNote (long NotCod,long PublisherCod)
  {
   /***** Remove note *****/
   DB_QueryDELETE ("can not remove a note",
		   "DELETE FROM tl_notes"
	           " WHERE NotCod=%ld"
	           " AND UsrCod=%ld",		// Extra check: author
		   NotCod,
		   PublisherCod);
  }

/*****************************************************************************/
/***************** Get code of media associated to post **********************/
/*****************************************************************************/

long TL_DB_GetMedCodFromPost (long PubCod)
  {
   return TL_DB_GetMedCodFromPub (PubCod,"tl_posts");
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
/********************* Get number of comments in a note **********************/
/*****************************************************************************/

unsigned TL_DB_GetNumCommentsInNote (long NotCod)
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

unsigned TL_DB_GetComments (long NotCod,MYSQL_RES **mysql_res)
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

unsigned TL_DB_GetInitialComments (long NotCod,
				   unsigned NumInitialCommentsToGet,
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
		   NumInitialCommentsToGet);
  }

/*****************************************************************************/
/*************** Get final comments of a note from database ******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned TL_DB_GetFinalComments (long NotCod,
				 unsigned NumFinalCommentsToGet,
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
		  NumFinalCommentsToGet);
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

void TL_DB_InsertCommentContent (long PubCod,
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

long TL_DB_GetMedCodFromComment (long PubCod)
  {
   return TL_DB_GetMedCodFromPub (PubCod,"tl_comments");
  }

/*****************************************************************************/
/****************** Remove favs for comment from database ********************/
/*****************************************************************************/

void TL_DB_RemoveCommentFavs (long PubCod)
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

void TL_DB_RemoveCommentContent (long PubCod)
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

void TL_DB_RemoveCommentPub (long PubCod,long PublisherCod)
  {
   /***** Remove comment publication *****/
   DB_QueryDELETE ("can not remove comment",
		   "DELETE FROM tl_pubs"
	           " WHERE PubCod=%ld"
	           " AND PublisherCod=%ld"	// Extra check: author
	           " AND PubType=%u",		// Extra check: it's a comment
		   PubCod,
		   PublisherCod,
		   (unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*************** Get code of media associated to post/comment ****************/
/*****************************************************************************/

static long TL_DB_GetMedCodFromPub (long PubCod,const char *DBTable)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long MedCod = -1L;	// Default value

   /***** Get code of media associated to comment *****/
   if (DB_QuerySELECT (&mysql_res,"can not get media code",
		       "SELECT MedCod"	// row[0]
		       " FROM %s"
		       " WHERE PubCod=%ld",
		       DBTable,PubCod) == 1)   // Result should have a unique row
     {
      /* Get media code */
      row = mysql_fetch_row (mysql_res);
      MedCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return MedCod;
  }
