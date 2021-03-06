// swad_tag.c: tags for questions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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

#include <mysql/mysql.h>	// To access MySQL databases
#include <stdbool.h>		// For boolean type
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_tag.h"
#include "swad_theme.h"

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

static void Tag_EnableOrDisableTag (long TagCod,bool TagHidden);

static long Tag_GetParamTagCode (void);

static long Tag_GetTagCodFromTagTxt (const char *TagTxt);
static long Tag_CreateNewTag (long CrsCod,const char *TagTxt);

static void Tag_PutIconEnable (long TagCod,const char *TagTxt);
static void Tag_PutIconDisable (long TagCod,const char *TagTxt);

/*****************************************************************************/
/********************************* Reset tags ********************************/
/*****************************************************************************/

void Tag_ResetTags (struct Tag_Tags *Tags)
  {
   unsigned IndTag;

   Tags->Num  = 0;
   Tags->All  = false;
   Tags->List = NULL;

   /***** Initialize all tags in question to empty string *****/
   for (IndTag = 0;
	IndTag < Tag_MAX_TAGS_PER_QUESTION;
	IndTag++)
      Tags->Txt[IndTag][0] = '\0';
  }

/*****************************************************************************/
/**************** Free memory allocated for the list of tags *****************/
/*****************************************************************************/

void Tag_FreeTagsList (struct Tag_Tags *Tags)
  {
   if (Tags->List)
     {
      free (Tags->List);
      Tag_ResetTags (Tags);
     }
  }

/*****************************************************************************/
/*************** Put a link (form) to import test questions ******************/
/*****************************************************************************/

void Tag_PutIconToEditTags (void)
  {
   extern const char *Txt_Edit_tags;

   /***** Put a link to create a file with questions *****/
   Lay_PutContextualLinkOnlyIcon (ActEdiTag,NULL,
                                  NULL,NULL,
				  "tag.svg",
				  Txt_Edit_tags);
  }

/*****************************************************************************/
/******************* Check if current course has test tags *******************/
/*****************************************************************************/
// Return the number of rows of the result

bool Tag_CheckIfCurrentCrsHasTestTags (void)
  {
   /***** Get available tags from database *****/
   return (DB_QueryCOUNT ("can not check if course has tags",
			  "SELECT COUNT(*)"
			   " FROM tst_tags"
			  " WHERE CrsCod=%ld",
			  Gbl.Hierarchy.Crs.CrsCod) != 0);
  }

/*****************************************************************************/
/********* Get all (enabled or disabled) test tags for this course ***********/
/*****************************************************************************/
// Return the number of rows of the result

unsigned Tag_GetAllTagsFromCurrentCrs (MYSQL_RES **mysql_res)
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

unsigned Tag_GetEnabledTagsFromThisCrs (MYSQL_RES **mysql_res)
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
/******************************* Enable a test tag ***************************/
/*****************************************************************************/

void Tag_EnableTag (void)
  {
   long TagCod = Tag_GetParamTagCode ();

   /***** Change tag status to enabled *****/
   Tag_EnableOrDisableTag (TagCod,false);

   /***** Show again the form to edit tags *****/
   Tag_ShowFormEditTags ();
  }

/*****************************************************************************/
/****************************** Disable a test tag ***************************/
/*****************************************************************************/

void Tag_DisableTag (void)
  {
   long TagCod = Tag_GetParamTagCode ();

   /***** Change tag status to disabled *****/
   Tag_EnableOrDisableTag (TagCod,true);

   /***** Show again the form to edit tags *****/
   Tag_ShowFormEditTags ();
  }

/*****************************************************************************/
/********** Change visibility of an existing tag into tst_tags table *********/
/*****************************************************************************/

static void Tag_EnableOrDisableTag (long TagCod,bool TagHidden)
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
/************************* Get parameter with tag code ***********************/
/*****************************************************************************/

static long Tag_GetParamTagCode (void)
  {
   long TagCod;

   /***** Get tag code *****/
   if ((TagCod = Par_GetParToLong ("TagCod")) <= 0)
      Err_WrongTagExit ();

   return TagCod;
  }

/*****************************************************************************/
/************************ Rename a tag of test questions *********************/
/*****************************************************************************/

void Tag_RenameTag (void)
  {
   extern const char *Txt_The_tag_X_has_been_renamed_as_Y;
   extern const char *Txt_The_tag_X_has_not_changed;
   char OldTagTxt[Tag_MAX_BYTES_TAG + 1];
   char NewTagTxt[Tag_MAX_BYTES_TAG + 1];
   long ExistingTagCod;
   long OldTagCod;
   bool ComplexRenaming;

   /***** Get old and new tags from the form *****/
   Par_GetParToText ("OldTagTxt",OldTagTxt,Tag_MAX_BYTES_TAG);
   Par_GetParToText ("NewTagTxt",NewTagTxt,Tag_MAX_BYTES_TAG);

   /***** Check that the new tag is not empty *****/
   if (NewTagTxt[0])	// New tag not empty
     {
      /***** Check if the old tag is equal to the new one *****/
      if (!strcmp (OldTagTxt,NewTagTxt))	// The old and the new tag
						// are exactly the same (case sensitively).
						// This happens when user press INTRO
						// without changing anything in the form.
         Ale_ShowAlert (Ale_INFO,Txt_The_tag_X_has_not_changed,
                        NewTagTxt);
      else					// The old and the new tag
						// are not exactly the same (case sensitively).
	{
	 /***** Check if renaming is complex or easy *****/
	 ComplexRenaming = false;
	 if (strcasecmp (OldTagTxt,NewTagTxt))	// The old and the new tag
						// are not the same (case insensitively)
	    /* Check if the new tag text is equal to any of the tags
	       already present in the database */
	    if ((ExistingTagCod = Tag_GetTagCodFromTagTxt (NewTagTxt)) > 0)
	       // The new tag was already in database
	       ComplexRenaming = true;

	 if (ComplexRenaming)	// Renaming is not easy
	   {
	    /***** Complex update made to not repeat tags:
		   - If the new tag existed for a question ==>
		     delete old tag from tst_question_tags;
		     the new tag will remain
		   - If the new tag did not exist for a question ==>
		     change old tag to new tag in tst_question_tags *****/
	    /* Get tag code of the old tag */
	    if ((OldTagCod = Tag_GetTagCodFromTagTxt (OldTagTxt)) <= 0)
	       Err_WrongTagExit ();

	    /* Create a temporary table with all the question codes
	       that had the new tag as one of their tags */
	    DB_Query ("can not remove temporary table",
		      "DROP TEMPORARY TABLE IF EXISTS tst_question_tags_tmp");

	    DB_Query ("can not create temporary table",
		      "CREATE TEMPORARY TABLE tst_question_tags_tmp"
		      " ENGINE=MEMORY"
		      " SELECT QstCod"
		        " FROM tst_question_tags"
	   	       " WHERE TagCod=%ld",
		      ExistingTagCod);

	    /* Remove old tag in questions where it would be repeated */
	    // New tag existed for a question ==> delete old tag
	    DB_QueryDELETE ("can not remove a tag from some questions",
			    "DELETE FROM tst_question_tags"
			    " WHERE TagCod=%ld"
			      " AND QstCod IN"
			          " (SELECT QstCod"
			             " FROM tst_question_tags_tmp)",
			    OldTagCod);

	    /* Change old tag to new tag in questions where it would not be repeated */
	    // New tag did not exist for a question ==> change old tag to new tag
	    DB_QueryUPDATE ("can not update a tag in some questions",
			    "UPDATE tst_question_tags"
			      " SET TagCod=%ld"
			    " WHERE TagCod=%ld"
			      " AND QstCod NOT IN"
			          " (SELECT QstCod"
			             " FROM tst_question_tags_tmp)",
			    ExistingTagCod,
			    OldTagCod);

	    /* Drop temporary table, no longer necessary */
	    DB_Query ("can not remove temporary table",
		      "DROP TEMPORARY TABLE IF EXISTS tst_question_tags_tmp");

	    /***** Delete old tag from tst_tags
		   because it is not longer used *****/
	    DB_QueryDELETE ("can not remove old tag",
			    "DELETE FROM tst_tags"
			    " WHERE TagCod=%ld",
			    OldTagCod);
	   }
	 else			// Renaming is easy
	   {
	    /***** Simple update replacing each instance of the old tag by the new tag *****/
	    DB_QueryUPDATE ("can not update tag",
			    "UPDATE tst_tags"
			      " SET TagTxt='%s',"
			           "ChangeTime=NOW()"
			    " WHERE tst_tags.CrsCod=%ld"
			      " AND tst_tags.TagTxt='%s'",
			    NewTagTxt,
			    Gbl.Hierarchy.Crs.CrsCod,
			    OldTagTxt);
	   }

	 /***** Write message to show the change made *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_tag_X_has_been_renamed_as_Y,
		        OldTagTxt,NewTagTxt);
	}
     }
   else			// New tag empty
      Ale_ShowAlertYouCanNotLeaveFieldEmpty ();

   /***** Show again the form to edit tags *****/
   Tag_ShowFormEditTags ();
  }

/*****************************************************************************/
/***************** Check if this tag exists for current course ***************/
/*****************************************************************************/

static long Tag_GetTagCodFromTagTxt (const char *TagTxt)
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
/*********************** Insert tags in the tags table ***********************/
/*****************************************************************************/

void Tag_InsertTagsIntoDB (long QstCod,const struct Tag_Tags *Tags)
  {
   unsigned NumTag;
   unsigned TagIdx;
   long TagCod;

   /***** For each tag... *****/
   for (NumTag = 0, TagIdx = 0;
        TagIdx < Tags->Num;
        NumTag++)
      if (Tags->Txt[NumTag][0])
        {
         /***** Check if this tag exists for current course *****/
         if ((TagCod = Tag_GetTagCodFromTagTxt (Tags->Txt[NumTag])) < 0)
            /* This tag is new for current course. Add it to tags table */
            TagCod = Tag_CreateNewTag (Gbl.Hierarchy.Crs.CrsCod,Tags->Txt[NumTag]);

         /***** Insert tag in tst_question_tags *****/
         DB_QueryINSERT ("can not create tag",
			 "INSERT INTO tst_question_tags"
                         " (QstCod,TagCod,TagInd)"
                         " VALUES"
                         " (%ld,%ld,%u)",
			 QstCod,
			 TagCod,
			 TagIdx);

         TagIdx++;
        }
  }

/*****************************************************************************/
/********************* Insert new tag into tst_tags table ********************/
/*****************************************************************************/

static long Tag_CreateNewTag (long CrsCod,const char *TagTxt)
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
/********************* Show a form to select test tags ***********************/
/*****************************************************************************/

void Tag_ShowFormSelTags (const struct Tag_Tags *Tags,
                          MYSQL_RES *mysql_res,
                          bool ShowOnlyEnabledTags)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Tags;
   extern const char *Txt_All_tags;
   extern const char *Txt_Tag_not_allowed;
   extern const char *Txt_Tag_allowed;
   unsigned NumTag;
   MYSQL_ROW row;
   bool TagHidden = false;
   bool Checked;
   const char *Ptr;
   char TagText[Tag_MAX_BYTES_TAG + 1];
   /*
   row[0] TagCod
   row[1] TagTxt
   row[2] TagHidden
   */
   HTM_TR_Begin (NULL);

   /***** Label *****/
   HTM_TD_Begin ("class=\"RT %s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtColon (Txt_Tags);
   HTM_TD_End ();

   /***** Select all tags *****/
   HTM_TD_Begin ("class=\"LT\"");

   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   if (!ShowOnlyEnabledTags)
      HTM_TD_Empty (1);

   HTM_TD_Begin ("class=\"LM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("AllTags",HTM_DONT_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s onclick=\"togglecheckChildren(this,'ChkTag');\"",
		       Tags->All ? " checked=\"checked\"" :
			           "");
   HTM_TxtF ("&nbsp;%s",Txt_All_tags);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Select tags one by one *****/
   for (NumTag = 1;
	NumTag <= Tags->Num;
	NumTag++)
     {
      row = mysql_fetch_row (mysql_res);
      HTM_TR_Begin (NULL);

      if (!ShowOnlyEnabledTags)
        {
         TagHidden = (row[2][0] == 'Y');
         HTM_TD_Begin ("class=\"LM\"");
         Ico_PutIconOff (TagHidden ? "eye-slash-red.svg" :
                                     "eye-green.svg",
			 TagHidden ? Txt_Tag_not_allowed :
			             Txt_Tag_allowed);
         HTM_TD_End ();
        }

      Checked = false;
      if (Tags->List)
        {
         Ptr = Tags->List;
         while (*Ptr)
           {
            Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tag_MAX_BYTES_TAG);
            if (!strcmp (row[1],TagText))
              {
               Checked = true;
               break;
              }
           }
        }

      HTM_TD_Begin ("class=\"LM\"");
      HTM_LABEL_Begin ("class=\"DAT\"");
      HTM_INPUT_CHECKBOX ("ChkTag",HTM_DONT_SUBMIT_ON_CHANGE,
			  "value=\"%s\"%s onclick=\"checkParent(this,'AllTags');\"",
			  row[1],
			  Checked ? " checked=\"checked\"" :
				    "");
      HTM_TxtF ("&nbsp;%s",row[1]);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   HTM_TABLE_End ();
   HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show a form to enable/disable and rename test tags ************/
/*****************************************************************************/

void Tag_ShowFormEditTags (void)
  {
   extern const char *Hlp_ASSESSMENT_Questions_editing_tags;
   extern const char *Txt_No_test_questions;
   extern const char *Txt_Tags;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumTags;
   unsigned NumTag;
   long TagCod;

   /***** Get current tags in current course *****/
   if ((NumTags = Tag_GetAllTagsFromCurrentCrs (&mysql_res)))
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Tags,
                         NULL,NULL,
                         Hlp_ASSESSMENT_Questions_editing_tags,Box_NOT_CLOSABLE,2);

      /***** Show tags *****/
      for (NumTag = 0;
	   NumTag < NumTags;
	   NumTag++)
        {
         row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] TagCod
	 row[1] TagTxt
	 row[2] TagHidden
	 */
         if ((TagCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongTagExit ();

         HTM_TR_Begin (NULL);

         /* Form to enable / disable this tag */
         if (row[2][0] == 'Y')	// Tag disabled
            Tag_PutIconEnable (TagCod,row[1]);
         else
            Tag_PutIconDisable (TagCod,row[1]);

         /* Form to rename this tag */
         HTM_TD_Begin ("class=\"LM\"");
         Frm_BeginForm (ActRenTag);
         Par_PutHiddenParamString (NULL,"OldTagTxt",row[1]);
	 HTM_INPUT_TEXT ("NewTagTxt",Tag_MAX_CHARS_TAG,row[1],
	                 HTM_SUBMIT_ON_CHANGE,
			 "size=\"36\" required=\"required\"");
         Frm_EndForm ();
         HTM_TD_End ();

         HTM_TR_End ();
        }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_No_test_questions);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Put a link and an icon to enable a tag ******************/
/*****************************************************************************/

static void Tag_PutIconEnable (long TagCod,const char *TagTxt)
  {
   extern const char *Txt_Tag_X_not_allowed_Click_to_allow_it;

   HTM_TD_Begin ("class=\"BM\"");
   Frm_BeginForm (ActEnaTag);
   Par_PutHiddenParamLong (NULL,"TagCod",TagCod);
   Ico_PutIconLink ("eye-slash-red.svg",
		    Str_BuildStringStr (Txt_Tag_X_not_allowed_Click_to_allow_it,
				        TagTxt));
   Str_FreeString ();
   Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/****************** Put a link and an icon to disable a tag ******************/
/*****************************************************************************/

static void Tag_PutIconDisable (long TagCod,const char *TagTxt)
  {
   extern const char *Txt_Tag_X_allowed_Click_to_disable_it;

   HTM_TD_Begin ("class=\"BM\"");
   Frm_BeginForm (ActDisTag);
   Par_PutHiddenParamLong (NULL,"TagCod",TagCod);
   Ico_PutIconLink ("eye-green.svg",
		    Str_BuildStringStr (Txt_Tag_X_allowed_Click_to_disable_it,
				        TagTxt));
   Str_FreeString ();
   Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/************************** Remove tags from a question **********************/
/*****************************************************************************/

void Tag_RemTagsFromQst (long QstCod)
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

void Tag_RemoveUnusedTagsFromCrs (long CrsCod)
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
