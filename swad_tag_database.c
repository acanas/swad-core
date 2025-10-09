// swad_tag_database.c: tags for questions, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_database.h"
#include "swad_global.h"
#include "swad_tag.h"
#include "swad_tag_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/********************* Insert new tag into tst_tags table ********************/
/*****************************************************************************/

long Tag_DB_CreateNewTag (long HieCod,const char *TagTxt)
  {
   /***** Insert new tag into tst_tags table *****/
   return
   DB_QueryINSERTandReturnCode ("can not create new tag",
				"INSERT INTO tst_tags"
				" (CrsCod,ChangeTime,TagTxt,TagHidden)"
				" VALUES"
				" (%ld,NOW(),'%s','Y')",	// Hidden by default
				HieCod,
				TagTxt);
  }

/*****************************************************************************/
/*************************** Add tag to a question ***************************/
/*****************************************************************************/

void Tag_DB_AddTagToQst (long QstCod,long TagCod,unsigned TagInd)
  {
   DB_QueryINSERT ("can not create tag",
		   "INSERT INTO tst_question_tags"
		   " (QstCod,TagCod,TagInd)"
		   " VALUES"
		   " (%ld,%ld,%u)",
		   QstCod,
		   TagCod,
		   TagInd);
  }

/*****************************************************************************/
/********** Create a temporary table with all the question codes *************/
/********** that had the new tag as one of their tags            *************/
/*****************************************************************************/

void Tag_DB_CreateTmpTableQuestionsWithTag (long TagCod)
  {
   DB_CreateTmpTable ("CREATE TEMPORARY TABLE tst_question_tags_tmp"
		      " ENGINE=MEMORY"
		      " SELECT QstCod"
		        " FROM tst_question_tags"
		       " WHERE TagCod=%ld",
		      TagCod);
  }

/*****************************************************************************/
/************* Drop temporary table with all the question codes **************/
/************* that had the new tag as one of their tags        **************/
/*****************************************************************************/

void Tag_DB_DropTmpTableQuestionsWithTag (void)
  {
   DB_DropTmpTable ("tst_question_tags_tmp");
  }

/*****************************************************************************/
/************************* Complex renaming of tag ***************************/
/*****************************************************************************/

void Tag_DB_ComplexRenameTag (long TagCodOldTxt,long ExistingTagCodNewTxt)
  {
   /***** Remove old tag in questions where it would be repeated *****/
   // New tag existed for a question ==> delete old tag
   DB_QueryDELETE ("can not remove a tag from some questions",
		   "DELETE FROM tst_question_tags"
		   " WHERE TagCod=%ld"
		     " AND QstCod IN"
			   // Questions that already have a tag with the new name
			 " (SELECT QstCod"
			    " FROM tst_question_tags_tmp)",
		   TagCodOldTxt);

   /***** Change old tag to new tag in questions where it would not be repeated *****/
   // New tag did not exist for a question ==> change old tag to new tag
   DB_QueryUPDATE ("can not update a tag in some questions",
		   "UPDATE tst_question_tags"
		     " SET TagCod=%ld"
		   " WHERE TagCod=%ld"
		     " AND QstCod NOT IN"
			 " (SELECT QstCod"
			    " FROM tst_question_tags_tmp)",
		   ExistingTagCodNewTxt,
		   TagCodOldTxt);
  }

/*****************************************************************************/
/**** Simple update replacing each instance of the old tag by the new tag ****/
/*****************************************************************************/

void Tag_DB_SimplexRenameTag (const char OldTagTxt[Tag_MAX_BYTES_TAG + 1],
                              const char NewTagTxt[Tag_MAX_BYTES_TAG + 1])
  {
   DB_QueryUPDATE ("can not update tag",
		   "UPDATE tst_tags"
		     " SET TagTxt='%s',"
			  "ChangeTime=NOW()"
		   " WHERE tst_tags.CrsCod=%ld"
		     " AND tst_tags.TagTxt='%s'",
		   NewTagTxt,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   OldTagTxt);
  }

/*****************************************************************************/
/********** Change visibility of an existing tag into tst_tags table *********/
/*****************************************************************************/

void Tag_DB_EnableOrDisableTag (long TagCod,HidVis_HiddenOrVisible_t TagHidden)
  {
   extern const char HidVis_Hidden_YN[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Insert new tag into tst_tags table *****/
   DB_QueryUPDATE ("can not update the visibility of a tag",
		   "UPDATE tst_tags"
		     " SET TagHidden='%c',"
		          "ChangeTime=NOW()"
                   " WHERE TagCod=%ld"
                     " AND CrsCod=%ld",
		   HidVis_Hidden_YN[TagHidden],
		   TagCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************************* Get tags of a test question ***********************/
/*****************************************************************************/

unsigned Tag_DB_GetTagsQst (MYSQL_RES **mysql_res,long QstCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the tags of a question",
		   "SELECT tst_tags.TagTxt"	// row[0]
		    " FROM tst_question_tags,"
			  "tst_tags"
		   " WHERE tst_question_tags.QstCod=%ld"
		     " AND tst_question_tags.TagCod=tst_tags.TagCod"
		     " AND tst_tags.CrsCod=%ld"
		" ORDER BY tst_question_tags.TagInd",
		   QstCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********* Get all (enabled or disabled) test tags for this course ***********/
/*****************************************************************************/
// Return the number of rows of the result

unsigned Tag_DB_GetAllTagsFromCurrentCrs (MYSQL_RES **mysql_res)
  {
   /***** Get available tags from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get available tags",
		   "SELECT TagCod,"	// row[0]
			  "TagTxt,"	// row[1]
			  "TagHidden"	// row[2]
		    " FROM tst_tags"
		   " WHERE CrsCod=%ld"
		" ORDER BY TagTxt",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************** Get enabled test tags for this course ****************/
/*****************************************************************************/
// If TagCod  > 0 ==> get only that tag if enabled
// If TagCod <= 0 ==> get all enabled tags
// Return the number of rows of the result

unsigned Tag_DB_GetEnabledTagsFromCrs (MYSQL_RES **mysql_res,
				       long TagCod,long HieCod)
  {
   /***** Get available not hidden tags from database *****/
   return (unsigned)
   TagCod > 0 ? DB_QuerySELECT (mysql_res,"can not get available enabled tags",
				"SELECT TagCod,"	// row[0]
				       "TagTxt"		// row[1]
				 " FROM tst_tags"
				" WHERE TagCod=%ld"
				  " AND CrsCod=%ld"	// Extra check
				  " AND TagHidden='N'",
				TagCod,
				HieCod) :
		DB_QuerySELECT (mysql_res,"can not get available enabled tags",
				"SELECT TagCod,"	// row[0]
				       "TagTxt"		// row[1]
				 " FROM tst_tags"
				" WHERE CrsCod=%ld"
				  " AND TagHidden='N'"
			     " ORDER BY TagTxt",
				HieCod);
  }

/*****************************************************************************/
/******************* Check if current course has test tags *******************/
/*****************************************************************************/
// Return the number of rows of the result

Exi_Exist_t Tag_DB_CheckIfTagsExistInCurrentCrs (void)
  {
   return
   DB_QueryEXISTS ("can not check if course has tags",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM tst_tags"
		    " WHERE CrsCod=%ld)",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/**************** Get tag code from tag text in current course ***************/
/*****************************************************************************/

long Tag_DB_GetTagCodFromTagTxt (const char *TagTxt)
  {
   /***** Get tag code from database *****/
   return DB_QuerySELECTCode ("can not get tag",
			      "SELECT TagCod"
			       " FROM tst_tags"
			      " WHERE CrsCod=%ld"
			        " AND TagTxt='%s'",
			      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			      TagTxt);
  }

/*****************************************************************************/
/************************ Get tag title from database ************************/
/*****************************************************************************/

void Tag_DB_GetTagTitle (long TagCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get tag title",
		         "SELECT TagTxt"
			  " FROM tst_tags"
		         " WHERE TagCod=%ld"
			   " AND CrsCod=%ld",	// Extra check
		         TagCod,
		         Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*** Get tags of recent test questions from database giving a course code ****/
/*****************************************************************************/

unsigned Tag_DB_GetRecentTags (MYSQL_RES **mysql_res,
		               long HieCod,time_t BeginTime)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get question tags",
		   "SELECT QstCod,"	// row[0]
			  "TagCod,"	// row[1]
			  "TagInd"	// row[2]
		    " FROM tst_question_tags"
		   " WHERE QstCod IN "
			  "(SELECT tst_questions.QstCod"
			    " FROM tst_questions,"
				  "tst_question_tags,"
				  "tst_tags"
			   " WHERE tst_questions.CrsCod=%ld"
			     " AND tst_questions.QstCod NOT IN"
				 " (SELECT tst_question_tags.QstCod"
				    " FROM tst_tags,"
					  "tst_question_tags"
				   " WHERE tst_tags.CrsCod=%ld"
				     " AND tst_tags.TagHidden='Y'"
				     " AND tst_tags.TagCod=tst_question_tags.TagCod)"
			     " AND tst_questions.QstCod=tst_question_tags.QstCod"
			     " AND tst_question_tags.TagCod=tst_tags.TagCod"
			     " AND tst_tags.CrsCod=%ld"
			     " AND (tst_questions.EditTime>=FROM_UNIXTIME(%ld)"
				  " OR "
				   "tst_tags.ChangeTime>=FROM_UNIXTIME(%ld)))"
		" ORDER BY QstCod,"
			  "TagInd",
		   HieCod,
		   HieCod,
		   HieCod,
		   (long) BeginTime,
		   (long) BeginTime);
  }

/*****************************************************************************/
/******** Remove associations between questions and tags in a course *********/
/*****************************************************************************/

void Tag_DB_RemTagsInQstsInCrs (long HieCod)
  {
   DB_QueryDELETE ("can not remove tags associated"
		   " to questions in a course",
		   "DELETE FROM tst_question_tags"
	           " USING tst_questions,"
	                  "tst_question_tags"
                   " WHERE tst_questions.CrsCod=%ld"
                     " AND tst_questions.QstCod=tst_question_tags.QstCod",
		   HieCod);
  }

/*****************************************************************************/
/**************************** Remove tags in a course ************************/
/*****************************************************************************/

void Tag_DB_RemTagsInCrs (long HieCod)
  {
   DB_QueryDELETE ("can not remove tags of questions in a course",
		   "DELETE FROM tst_tags"
		   " WHERE CrsCod=%ld",
		   HieCod);
  }

/*****************************************************************************/
/************************** Remove tags from a question **********************/
/*****************************************************************************/

void Tag_DB_RemTagsFromQst (long QstCod)
  {
   DB_QueryDELETE ("can not remove the tags of a question",
		   "DELETE FROM tst_question_tags"
		   " WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/******************************** Remove a tag *******************************/
/*****************************************************************************/

void Tag_DB_RemoveTag (long TagCod)
  {
   DB_QueryDELETE ("can not remove tag",
		   "DELETE FROM tst_tags"
		   " WHERE TagCod=%ld",
		   TagCod);
  }

/*****************************************************************************/
/********************** Remove unused tags in a course ***********************/
/*****************************************************************************/

void Tag_DB_RemoveUnusedTagsFromCrs (long HieCod)
  {
   /***** Remove unused tags from tst_tags *****/
   DB_QueryDELETE ("can not remove unused tags",
		   "DELETE FROM tst_tags"
	           " WHERE CrsCod=%ld"
	             " AND TagCod NOT IN"
			 " (SELECT DISTINCT "
			          "tst_question_tags.TagCod"
			    " FROM tst_questions,"
			          "tst_question_tags"
			   " WHERE tst_questions.CrsCod=%ld"
			     " AND tst_questions.QstCod=tst_question_tags.QstCod)",
		   HieCod,
		   HieCod);
  }
