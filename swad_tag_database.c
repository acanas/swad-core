// swad_tag_database.c: tags for questions, operations with database

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
/*********************************** Headers *********************************/
/*****************************************************************************/

// #include <mysql/mysql.h>	// To access MySQL databases
// #include <stdbool.h>		// For boolean type
// #include <stdlib.h>		// For free
// #include <string.h>		// For string functions

// #include "swad_action.h"
#include "swad_database.h"
// #include "swad_error.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_tag.h"
#include "swad_tag_database.h"
// #include "swad_theme.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
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
/********************* Insert new tag into tst_tags table ********************/
/*****************************************************************************/

long Tag_DB_CreateNewTag (long CrsCod,const char *TagTxt)
  {
   /***** Insert new tag into tst_tags table *****/
   return
   DB_QueryINSERTandReturnCode ("can not create new tag",
				"INSERT INTO tst_tags"
				" (CrsCod,ChangeTime,TagTxt,TagHidden)"
				" VALUES"
				" (%ld,NOW(),'%s','Y')",	// Hidden by default
				CrsCod,
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
/********** Change visibility of an existing tag into tst_tags table *********/
/*****************************************************************************/

void Tag_DB_EnableOrDisableTag (long TagCod,bool TagHidden)
  {
   /***** Insert new tag into tst_tags table *****/
   DB_QueryUPDATE ("can not update the visibility of a tag",
		   "UPDATE tst_tags"
		     " SET TagHidden='%c',"
		          "ChangeTime=NOW()"
                   " WHERE TagCod=%ld"
                     " AND CrsCod=%ld",
		   TagHidden ? 'Y' :
			       'N',
		   TagCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********* Get all (enabled or disabled) test tags for this course ***********/
/*****************************************************************************/
// Return the number of rows of the result

unsigned Tag_DB_GetAllTagsFromCurrentCrs (MYSQL_RES **mysql_res)
  {
   /***** Get available tags from database *****/
   return (unsigned) DB_QuerySELECT (mysql_res,"can not get available tags",
				     "SELECT TagCod,"	// row[0]
					    "TagTxt,"	// row[1]
					    "TagHidden"	// row[2]
				      " FROM tst_tags"
				     " WHERE CrsCod=%ld"
				     " ORDER BY TagTxt",
				     Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********************** Get enabled test tags for this course ****************/
/*****************************************************************************/
// Return the number of rows of the result

unsigned Tag_DB_GetEnabledTagsFromThisCrs (MYSQL_RES **mysql_res)
  {
   /***** Get available not hidden tags from database *****/
   return (unsigned) DB_QuerySELECT (mysql_res,"can not get available enabled tags",
				     "SELECT TagCod,"	// row[0]
					    "TagTxt"	// row[1]
				      " FROM tst_tags"
				     " WHERE CrsCod=%ld"
				       " AND TagHidden='N'"
				     " ORDER BY TagTxt",
				     Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******************* Check if current course has test tags *******************/
/*****************************************************************************/
// Return the number of rows of the result

bool Tag_DB_CheckIfCurrentCrsHasTestTags (void)
  {
   /***** Get available tags from database *****/
   return (DB_QueryCOUNT ("can not check if course has tags",
			  "SELECT COUNT(*)"
			   " FROM tst_tags"
			  " WHERE CrsCod=%ld",
			  Gbl.Hierarchy.Crs.CrsCod) != 0);
  }

/*****************************************************************************/
/***************** Check if this tag exists for current course ***************/
/*****************************************************************************/

long Tag_DB_GetTagCodFromTagTxt (const char *TagTxt)
  {
   /***** Get tag code from database *****/
   return DB_QuerySELECTCode ("can not get tag",
			      "SELECT TagCod"
			       " FROM tst_tags"
			      " WHERE CrsCod=%ld"
			        " AND TagTxt='%s'",
			      Gbl.Hierarchy.Crs.CrsCod,
			      TagTxt);
  }

/*****************************************************************************/
/************************** Remove tags from a question **********************/
/*****************************************************************************/

void Tag_DB_RemTagsFromQst (long QstCod)
  {
   /***** Remove tags *****/
   DB_QueryDELETE ("can not remove the tags of a question",
		   "DELETE FROM tst_question_tags"
		   " WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/********************** Remove unused tags in a course ***********************/
/*****************************************************************************/

void Tag_DB_RemoveUnusedTagsFromCrs (long CrsCod)
  {
   /***** Remove unused tags from tst_tags *****/
   DB_QueryDELETE ("can not remove unused tags",
		   "DELETE FROM tst_tags"
	           " WHERE CrsCod=%ld"
	             " AND TagCod NOT IN"
			 " (SELECT DISTINCT tst_question_tags.TagCod"
			    " FROM tst_questions,"
			          "tst_question_tags"
			   " WHERE tst_questions.CrsCod=%ld"
			     " AND tst_questions.QstCod=tst_question_tags.QstCod)",
		   CrsCod,
		   CrsCod);
  }
