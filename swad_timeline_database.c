// swad_timeline_database.c: social timeline operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_parameter.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

static const char *Tml_DB_TableFav[TmlUsr_NUM_FAV_SHA] =
  {
   [TmlUsr_FAV_UNF_NOTE] = "tml_notes_fav",
   [TmlUsr_FAV_UNF_COMM] = "tml_comments_fav",
   [TmlUsr_SHA_UNS_NOTE] = NULL,		// Not used
  };
static const char *Tml_DB_FieldFav[TmlUsr_NUM_FAV_SHA] =
  {
   [TmlUsr_FAV_UNF_NOTE] = "NotCod",
   [TmlUsr_FAV_UNF_COMM] = "PubCod",
   [TmlUsr_SHA_UNS_NOTE] = NULL,		// Not used
  };
static TmlPub_Type_t Tml_DB_PubTypeFav[TmlUsr_NUM_FAV_SHA] =
  {
   [TmlUsr_FAV_UNF_NOTE] = TmlPub_ORIGINAL_NOTE,
   [TmlUsr_FAV_UNF_COMM] = TmlPub_COMMENT_TO_NOTE,
   [TmlUsr_SHA_UNS_NOTE] = TmlPub_UNKNOWN,	// Not used
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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

unsigned Tml_DB_GetNoteDataByCod (long NotCod,MYSQL_RES **mysql_res)
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
   return DB_QuerySELECTCode ("can not get code of publication",
			      "SELECT PubCod"
			       " FROM tml_pubs"
			      " WHERE NotCod=%ld"
			        " AND PubType=%u",
			      NotCod,
			      (unsigned) TmlPub_ORIGINAL_NOTE);
  }

/*****************************************************************************/
/***************************** Create a new note *****************************/
/*****************************************************************************/
// Returns code of note just created

long Tml_DB_CreateNewNote (TmlNot_Type_t NoteType,long Cod,
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

void Tml_DB_MarkNoteAsUnavailable (TmlNot_Type_t NoteType,long Cod)
  {
   DB_QueryUPDATE ("can not mark note as unavailable",
		   "UPDATE tml_notes"
		     " SET Unavailable='Y'"
		   " WHERE NoteType=%u"
		     " AND Cod=%ld",
		   (unsigned) NoteType,
		   Cod);
  }

/*****************************************************************************/
/***** Mark possible notes involving children of a folder as unavailable *****/
/*****************************************************************************/

void Tml_DB_MarkNotesChildrenOfFolderAsUnavailable (TmlNot_Type_t NoteType,
                                                    Brw_FileBrowser_t FileBrowser,
                                                    long Cod,const char *Path)
  {
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
/********** Get number of notes and users depending on note type *************/
/*****************************************************************************/

unsigned Tml_DB_GetNumNotesAndUsrsByType (MYSQL_RES **mysql_res,
                                          TmlNot_Type_t NoteType)
  {
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(*),"				// row[0]
				"COUNT(DISTINCT UsrCod)"		// row[1]
			  " FROM tml_notes"
			 " WHERE NoteType=%u",
			 (unsigned) NoteType);
      case Hie_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM ins_instits,"
				"ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod"
			   " AND tml_notes.NoteType=%u",
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			 (unsigned) NoteType);
      case Hie_INS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod"
			   " AND tml_notes.NoteType=%u",
			 Gbl.Hierarchy.Node[Hie_INS].HieCod,
			 (unsigned) NoteType);
      case Hie_CTR:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM deg_degrees,"
				"crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod"
			   " AND tml_notes.NoteType=%u",
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			 (unsigned) NoteType);
      case Hie_DEG:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod"
			   " AND tml_notes.NoteType=%u",
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			 (unsigned) NoteType);
      case Hie_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM crs_users,"
				"tml_notes"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=tml_notes.UsrCod"
			   " AND tml_notes.NoteType=%u",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			 (unsigned) NoteType);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******************** Get total number of notes and users ********************/
/*****************************************************************************/

unsigned Tml_DB_GetNumNotesAndUsrsTotal (MYSQL_RES **mysql_res)
  {
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(*),"				// row[0]
				"COUNT(DISTINCT UsrCod)"		// row[1]
			 " FROM tml_notes");
      case Hie_CTY:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM ins_instits,"
				"ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod",
			 Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM ctr_centers,"
				"deg_degrees,"
				"crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod",
			 Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM deg_degrees,"
				"crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod",
			 Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM crs_courses,"
				"crs_users,"
				"tml_notes"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=crs_users.CrsCod"
			   " AND crs_users.UsrCod=tml_notes.UsrCod",
			 Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
	 return (unsigned)
	 DB_QuerySELECT (mysql_res,"can not get number of social notes",
			 "SELECT COUNT(DISTINCT tml_notes.NotCod),"	// row[0]
				"COUNT(DISTINCT tml_notes.UsrCod)"	// row[1]
			  " FROM crs_users,"
				"tml_notes"
			 " WHERE crs_users.CrsCod=%ld"
			   " AND crs_users.UsrCod=tml_notes.UsrCod",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/******* Create temporary tables used to not get notes already shown *********/
/*****************************************************************************/

void Tml_DB_CreateTmpTableTimeline (Tml_WhatToGet_t WhatToGet)
  {
   switch (WhatToGet)
     {
      case Tml_GET_OLD_PUBS:
	 DB_CreateTmpTable ("CREATE TEMPORARY TABLE tml_tmp_timeline "
			    "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
			    " ENGINE=MEMORY"
			    " SELECT NotCod"
			      " FROM tml_timelines"
			     " WHERE SessionId='%s'",
			    Gbl.Session.Id);
	 break;
      case Tml_GET_NEW_PUBS:
      case Tml_GET_REC_PUBS:
      default:
	 DB_CreateTmpTable ("CREATE TEMPORARY TABLE tml_tmp_timeline "
			    "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
			    " ENGINE=MEMORY");
	 break;
     }
  }

/*****************************************************************************/
/**** Insert note in temporary table used to not get notes already shown *****/
/*****************************************************************************/

void Tml_DB_InsertNoteInTimeline (long NotCod)
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
   /* Insert note in temporary table with visible timeline.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not insert note in timeline",
		   "INSERT IGNORE INTO tml_tmp_timeline"
		   " SET NotCod=%ld",
		   NotCod);

   DB_QueryINSERT ("can not insert note in timeline",
		   "INSERT IGNORE INTO tml_timelines"
	           " (SessionId,NotCod)"
	           " VALUES"
	           " ('%s',%ld)",
		   Gbl.Session.Id,
		   NotCod);
  }

/*****************************************************************************/
/********** Drop temporary table with all notes visible in timeline **********/
/*****************************************************************************/

void Tml_DB_DropTmpTableTimeline (void)
  {
   DB_DropTmpTable ("tml_tmp_timeline");
  }

/*****************************************************************************/
/******************* Clear unused old timelines in database ******************/
/*****************************************************************************/

void Tml_DB_ClearOldTimelinesNotesFromDB (void)
  {
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
   DB_QueryDELETE ("can not remove notes",
		   "DELETE FROM tml_notes"
		   " WHERE UsrCod=%ld",
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

long Tml_DB_CreateNewPost (const struct TmlPst_Content *Content)
  {
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
   DB_QueryDELETE ("can not remove posts",
		   "DELETE FROM tml_posts"
		   " WHERE PstCod IN"
		   " (SELECT Cod"
		      " FROM tml_notes"
	             " WHERE UsrCod=%ld"
	               " AND NoteType=%u)",
		   UsrCod,(unsigned) TmlNot_POST);
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
		  NotCod,
		  (unsigned) TmlPub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/******* Get publication codes of comments of a note from database ***********/
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
		   NotCod,(unsigned) TmlPub_COMMENT_TO_NOTE);
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
		   NotCod,(unsigned) TmlPub_COMMENT_TO_NOTE,
		   NumInitialCommsToGet);
  }

/*****************************************************************************/
/*************** Get final comments of a note from database ******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetFinalComms (long NotCod,unsigned NumFinalCommsToGet,
			       MYSQL_RES **mysql_res)
  {
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
		  NotCod,(unsigned) TmlPub_COMMENT_TO_NOTE,
		  NumFinalCommsToGet);
  }

/*****************************************************************************/
/******************* Get data of comment using its code **********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetCommDataByCod (long PubCod,MYSQL_RES **mysql_res)
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
		   PubCod,(unsigned) TmlPub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/******************* Insert comment content in database **********************/
/*****************************************************************************/

void Tml_DB_InsertCommContent (long PubCod,
			       const struct TmlPst_Content *Content)
  {
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
   DB_QueryDELETE ("can not remove comment",
		   "DELETE FROM tml_pubs"
	           " WHERE PubCod=%ld"
	             " AND PublisherCod=%ld"	// Extra check: author
	             " AND PubType=%u",		// Extra check: it's a comment
		   PubCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) TmlPub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/************* Remove all comments in all notes for a given user *************/
/*****************************************************************************/

void Tml_DB_RemoveAllCommsInAllNotesOf (long UsrCod)
  {
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tml_comments"
	           " USING tml_notes,"
	                  "tml_pubs,"
	                  "tml_comments"
	           " WHERE tml_notes.UsrCod=%ld"
		     " AND tml_notes.NotCod=tml_pubs.NotCod"
                     " AND tml_pubs.PubType=%u"
	             " AND tml_pubs.PubCod=tml_comments.PubCod",
		   UsrCod,(unsigned) TmlPub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*********** Remove all comments made by a given user in any note ************/
/*****************************************************************************/

void Tml_DB_RemoveAllCommsMadeBy (long UsrCod)
  {
   DB_QueryDELETE ("can not remove comments",
		   "DELETE FROM tml_comments"
	           " USING tml_pubs,"
	                  "tml_comments"
	           " WHERE tml_pubs.PublisherCod=%ld"
	             " AND tml_pubs.PubType=%u"
	             " AND tml_pubs.PubCod=tml_comments.PubCod",
		   UsrCod,
		   (unsigned) TmlPub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/*************** Get code of media associated to post/comment ****************/
/*****************************************************************************/

static long Tml_DB_GetMedCod (const char *Table,const char *Field,long Cod)
  {
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

void Tml_DB_CreateSubQueryPublishers (TmlUsr_UsrOrGbl_t UsrOrGbl,Usr_Who_t Who,
                                      char **Table,
                                      char SubQuery[TmlPub_MAX_BYTES_SUBQUERY + 1])
  {
   switch (UsrOrGbl)
     {
      case TmlUsr_TIMELINE_USR:	// Show the timeline of a user
	 *Table = "";
	 sprintf (SubQuery,"tml_pubs.PublisherCod=%ld AND ",
	          Gbl.Usrs.Other.UsrDat.UsrCod);
	 break;
      case TmlUsr_TIMELINE_GBL:	// Show the global timeline
	 switch (Who)
	   {
	    case Usr_WHO_ME:		// Show my timeline
	       *Table = "";
	       snprintf (SubQuery,TmlPub_MAX_BYTES_SUBQUERY + 1,
	                 "tml_pubs.PublisherCod=%ld AND ",
	                 Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
	    case Usr_WHO_FOLLOWED:	// Show the timeline of the users I follow
	       Fol_DB_CreateTmpTableMeAndUsrsIFollow ();
	       *Table = ",fol_tmp_me_and_followed";
	       Str_Copy (SubQuery,
			 "tml_pubs.PublisherCod=fol_tmp_me_and_followed.UsrCod AND ",
			 TmlPub_MAX_BYTES_SUBQUERY);
	       break;
	    case Usr_WHO_ALL:		// Show the timeline of all users
	       *Table = "";
	       SubQuery[0] = '\0';
	       break;
	    default:
	       Err_WrongWhoExit ();
	       break;
	   }
	 break;
     }
  }

/*****************************************************************************/
/***** Create subqueries with range of publications to get from tml_pubs *****/
/*****************************************************************************/

void Tml_DB_CreateSubQueryRange (TmlPub_Range_t Range,long PubCod,
                                 char SubQuery[TmlPub_MAX_BYTES_SUBQUERY + 1])
  {
   static const char Operator[TmlPub_NUM_RANGES] =
     {
      [TmlPub_TOP   ] = '<',
      [TmlPub_BOTTOM] = '>',
     };

   if (PubCod > 0)
      snprintf (SubQuery,TmlPub_MAX_BYTES_SUBQUERY + 1,
                "tml_pubs.PubCod%c%ld AND ",Operator[Range],PubCod);
   else
      SubQuery[0] = '\0';
  }

/*****************************************************************************/
/******************** Select the most recent publication *********************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_SelectTheMostRecentPub (MYSQL_RES **mysql_res,
                                        const struct TmlPub_SubQueries *SubQueries)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get publication",
		   "SELECT tml_pubs.PubCod,"		// row[0]
			  "tml_pubs.NotCod,"		// row[1]
			  "tml_pubs.PublisherCod,"	// row[2]
			  "tml_pubs.PubType"		// row[3]
		    " FROM tml_pubs%s"
		   " WHERE %s%s%s"
                   	 " tml_pubs.NotCod NOT IN"
			 " (SELECT NotCod"
			    " FROM tml_tmp_timeline)"
		   " ORDER BY tml_pubs.PubCod DESC"
		   " LIMIT 1",
		   SubQueries->Publishers.Table,
		   SubQueries->Range.Bottom,
		   SubQueries->Range.Top,
		   SubQueries->Publishers.SubQuery);
  }

/*****************************************************************************/
/****************** Get data of publication using its code *******************/
/*****************************************************************************/
// Returns the number of rows got

unsigned Tml_DB_GetPubDataByCod (long PubCod,MYSQL_RES **mysql_res)
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
   return DB_QuerySELECTCode ("can not get code of note",
			      "SELECT NotCod"
			       " FROM tml_pubs"
			      " WHERE PubCod=%ld",
			      PubCod);
  }

/*****************************************************************************/
/************* Get first/last publication code stored in session *************/
/*****************************************************************************/

long Tml_DB_GetPubCodFromSession (TmlPub_FirstLast_t FirstLast)
  {
   static const char *FldName[TmlPub_NUM_FIRST_LAST] =
     {
      [TmlPub_FIRST] = "FirstPubCod",
      [TmlPub_LAST ] = "LastPubCod",
     };
   long PubCod;

   /***** Get last publication code from database *****/
   PubCod = DB_QuerySELECTCode ("can not get publication code from session",
		                "SELECT %s"		// row[0]
		                 " FROM ses_sessions"
		                " WHERE SessionId='%s'",
		                FldName[FirstLast],
		                Gbl.Session.Id);
   if (PubCod < 0)
      PubCod = 0;

   return PubCod;
  }

/*****************************************************************************/
/****************** Get number of publications from a user *******************/
/*****************************************************************************/

unsigned Tml_DB_GetNumPubsUsr (long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of publications from a user",
		  "SELECT COUNT(*)"
		   " FROM tml_pubs"
		  " WHERE PublisherCod=%ld",
		  UsrCod);
  }

/*****************************************************************************/
/************** Get all publisher codes in a note, except me *****************/
/*****************************************************************************/

unsigned Tml_DB_GetPublishersInNoteExceptMe (MYSQL_RES **mysql_res,long PubCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get publishers of a note",
		   "SELECT DISTINCT "
		          "PublisherCod"
		    " FROM tml_pubs"
		   " WHERE NotCod=(SELECT NotCod"
				   " FROM tml_pubs"
				  " WHERE PubCod=%ld)"
				    " AND PublisherCod<>%ld",
		   PubCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Insert new publication in database ********************/
/*****************************************************************************/
// Return just created publication code

long Tml_DB_CreateNewPub (const struct TmlPub_Publication *Pub)
  {
   return
   DB_QueryINSERTandReturnCode ("can not publish note/comment",
				"INSERT INTO tml_pubs"
				" (NotCod,PublisherCod,PubType,TimePublish)"
				" VALUES"
				" (%ld,%ld,%u,NOW())",
				Pub->NotCod,
				Pub->PublisherCod,
				(unsigned) Pub->Type);
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
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tml_pubs"
                   " USING tml_notes,"
                          "tml_pubs"
	           " WHERE tml_notes.UsrCod=%ld"
                     " AND tml_notes.NotCod=tml_pubs.NotCod",
		   UsrCod);
  }

/*****************************************************************************/
/**************** Remove all publications published by a user ****************/
/*****************************************************************************/

void Tml_DB_RemoveAllPubsPublishedBy (long UsrCod)
  {
   DB_QueryDELETE ("can not remove publications",
		   "DELETE FROM tml_pubs"
		   " WHERE PublisherCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/****************** Check if a user has favourited a note ********************/
/*****************************************************************************/

bool Tml_DB_CheckIfFavedByUsr (TmlUsr_FavSha_t FavSha,long Cod,long UsrCod)
  {
   return
   DB_QueryEXISTS ("can not check if a user has favourited",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM %s"
		    " WHERE %s=%ld"
		      " AND UsrCod=%ld)",
		   Tml_DB_TableFav[FavSha],
		   Tml_DB_FieldFav[FavSha],Cod,UsrCod);
  }

/*****************************************************************************/
/********* Get number of times a note/comment has been favourited ************/
/*****************************************************************************/

unsigned Tml_DB_GetNumFavers (TmlUsr_FavSha_t FavSha,long Cod,long UsrCod)
  {
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

unsigned Tml_DB_GetFavers (TmlUsr_FavSha_t FavSha,
                           long Cod,long UsrCod,unsigned MaxUsrs,
                           MYSQL_RES **mysql_res)
  {
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

void Tml_DB_MarkAsFav (TmlUsr_FavSha_t FavSha,long Cod)
  {
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

void Tml_DB_UnmarkAsFav (TmlUsr_FavSha_t FavSha,long Cod)
  {
   DB_QueryDELETE ("can not unfavourite",
		   "DELETE FROM %s"
		   " WHERE %s=%ld"
		     " AND UsrCod=%ld",
		   Tml_DB_TableFav[FavSha],
		   Tml_DB_FieldFav[FavSha],Cod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** Remove all favs made by a given user to any comment **************/
/*****************************************************************************/

void Tml_DB_RemoveAllFavsMadeByUsr (TmlUsr_FavSha_t FavSha,long UsrCod)
  {
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM %s"
		   " WHERE UsrCod=%ld",
		   Tml_DB_TableFav[FavSha],
		   UsrCod);
  }

/*****************************************************************************/
/************ Remove all favs to notes/comments of a given user **************/
/*****************************************************************************/

void Tml_DB_RemoveAllFavsToPubsBy (TmlUsr_FavSha_t FavSha,long UsrCod)
  {
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM %s"
	           " USING tml_pubs,"
	                  "%s"
	           " WHERE tml_pubs.PublisherCod=%ld"	// Author of the comment
                     " AND tml_pubs.PubType=%u"
	             " AND tml_pubs.%s=%s.%s",
	           Tml_DB_TableFav[FavSha],
	           Tml_DB_TableFav[FavSha],
		   UsrCod,
		   (unsigned) Tml_DB_PubTypeFav[FavSha],
		   Tml_DB_FieldFav[FavSha],
		   Tml_DB_TableFav[FavSha],
		   Tml_DB_FieldFav[FavSha]);
  }

/*****************************************************************************/
/*** Remove all favs to all comments in all notes authored by a given user ***/
/*****************************************************************************/

void Tml_DB_RemoveAllFavsToAllCommsInAllNotesBy (long UsrCod)
  {
   DB_QueryDELETE ("can not remove favs",
		   "DELETE FROM tml_comments_fav"
	           " USING tml_notes,"
	                  "tml_pubs,"
	                  "tml_comments_fav"
	           " WHERE tml_notes.UsrCod=%ld"		// Author of the note
	             " AND tml_notes.NotCod=tml_pubs.NotCod"
                     " AND tml_pubs.PubType=%u"
	             " AND tml_pubs.PubCod=tml_comments_fav.PubCod",
		   UsrCod,
		   (unsigned) TmlPub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/****************** Check if a user has published a note *********************/
/*****************************************************************************/

bool Tml_DB_CheckIfSharedByUsr (long NotCod,long UsrCod)
  {
   return
   DB_QueryEXISTS ("can not check if a user has shared a note",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM tml_pubs"
		    " WHERE NotCod=%ld"
		      " AND PublisherCod=%ld"
		      " AND PubType=%u)",
		   NotCod,
		   UsrCod,
		   (unsigned) TmlPub_SHARED_NOTE);
  }

/*****************************************************************************/
/********** Get number of times a note has been shared in timeline ***********/
/*****************************************************************************/

unsigned Tml_DB_GetNumSharers (long NotCod,long UsrCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of times a note has been shared",
		  "SELECT COUNT(*)"
		   " FROM tml_pubs"
		  " WHERE NotCod=%ld"
		    " AND PublisherCod<>%ld"
		    " AND PubType=%u",
		  NotCod,
		  UsrCod,	// Author of the note
		  (unsigned) TmlPub_SHARED_NOTE);
  }

/*****************************************************************************/
/***************** Get list of users who have shared a note ******************/
/*****************************************************************************/

unsigned Tml_DB_GetSharers (long NotCod,long UsrCod,unsigned MaxUsrs,
                            MYSQL_RES **mysql_res)
  {
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
		   (unsigned) TmlPub_SHARED_NOTE,
		   MaxUsrs);
  }

/*****************************************************************************/
/****************** Remove shared publication from database ******************/
/*****************************************************************************/

void Tml_DB_RemoveSharedPub (long NotCod)
  {
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tml_pubs"
		   " WHERE NotCod=%ld"
		     " AND PublisherCod=%ld"
		     " AND PubType=%u",	// Extra check: shared note
		   NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) TmlPub_SHARED_NOTE);
  }
