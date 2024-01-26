// swad_tag.c: tags for questions

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_tag.h"
#include "swad_tag_database.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Tag_PutIconEnable (long TagCod);
static void Tag_PutIconDisable (long TagCod);

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
   Lay_PutContextualLinkOnlyIcon (ActEdiTag,NULL,
                                  NULL,NULL,
				  "tag.svg",Ico_BLACK);
  }

/*****************************************************************************/
/******************************* Enable a test tag ***************************/
/*****************************************************************************/

void Tag_EnableTag (void)
  {
   long TagCod = ParCod_GetAndCheckPar (ParCod_Tag);

   /***** Change tag status to enabled *****/
   Tag_DB_EnableOrDisableTag (TagCod,false);

   /***** Show again the form to edit tags *****/
   Tag_ShowFormEditTags ();
  }

/*****************************************************************************/
/****************************** Disable a test tag ***************************/
/*****************************************************************************/

void Tag_DisableTag (void)
  {
   long TagCod = ParCod_GetAndCheckPar (ParCod_Tag);

   /***** Change tag status to disabled *****/
   Tag_DB_EnableOrDisableTag (TagCod,true);

   /***** Show again the form to edit tags *****/
   Tag_ShowFormEditTags ();
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
   long ExistingTagCodNewTxt;
   long TagCodOldTxt;
   bool ComplexRenaming;

   /***** Get old and new tags from the form *****/
   Par_GetParText ("OldTagTxt",OldTagTxt,Tag_MAX_BYTES_TAG);
   Par_GetParText ("NewTagTxt",NewTagTxt,Tag_MAX_BYTES_TAG);

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
	    if ((ExistingTagCodNewTxt = Tag_DB_GetTagCodFromTagTxt (NewTagTxt)) > 0)
	       // The new tag was already in database
	       ComplexRenaming = true;

	 if (ComplexRenaming)	// Renaming is not easy
	   {
	    /***** Complex renaming made to not repeat tags:
		   - If the new tag existed for a question ==>
		     delete old tag from tst_question_tags;
		     the new tag will remain
		   - If the new tag did not exist for a question ==>
		     change old tag to new tag in tst_question_tags *****/
	    /* Get tag code of the old tag */
	    if ((TagCodOldTxt = Tag_DB_GetTagCodFromTagTxt (OldTagTxt)) <= 0)
	       Err_WrongTagExit ();

	    /* Create a temporary table with all the question codes
	       that had the new tag as one of their tags */
            Tag_DB_DropTmpTableQuestionsWithTag ();
	    Tag_DB_CreateTmpTableQuestionsWithTag (ExistingTagCodNewTxt);

	    /* Remove old tag in questions where it would be repeated */
	    /* Change old tag to new tag in questions where it would not be repeated */
	    // New tag existed for a question ==> delete old tag
	    // New tag did not exist for a question ==> change old tag to new tag
	    Tag_DB_ComplexRenameTag (TagCodOldTxt,ExistingTagCodNewTxt);

	    /* Drop temporary table, no longer necessary */
            Tag_DB_DropTmpTableQuestionsWithTag ();

	    /***** Delete old tag from tst_tags
		   because it is not longer used *****/
	    Tag_DB_RemoveTag (TagCodOldTxt);
	   }
	 else			// Renaming is easy
	    /***** Simple update replacing each instance of the old tag by the new tag *****/
	    Tag_DB_SimplexRenameTag (OldTagTxt,NewTagTxt);

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
/*********************** Insert tags in the tags table ***********************/
/*****************************************************************************/

void Tag_InsertTagsIntoDB (long QstCod,const struct Tag_Tags *Tags)
  {
   unsigned NumTag;
   unsigned TagInd;
   long TagCod;

   /***** For each tag... *****/
   for (NumTag = 0, TagInd = 0;
        TagInd < Tags->Num;
        NumTag++)
      if (Tags->Txt[NumTag][0])
        {
         /***** Check if this tag exists for current course *****/
         if ((TagCod = Tag_DB_GetTagCodFromTagTxt (Tags->Txt[NumTag])) < 0)
            /* This tag is new for current course. Add it to tags table */
            TagCod = Tag_DB_CreateNewTag (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Tags->Txt[NumTag]);

         /***** Insert tag in tst_question_tags *****/
         Tag_DB_AddTagToQst (QstCod,TagCod,TagInd);

         TagInd++;
        }
  }

/*****************************************************************************/
/***************** Count number of tags in the list of tags ******************/
/*****************************************************************************/

unsigned Tag_CountNumTagsInList (const struct Tag_Tags *Tags)
  {
   const char *Ptr;
   unsigned NumTags = 0;
   char TagText[Tag_MAX_BYTES_TAG + 1];

   /***** Go over the list of tags counting the number of tags *****/
   Ptr = Tags->List;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,TagText,Tag_MAX_BYTES_TAG);
      NumTags++;
     }

   return NumTags;
  }

/*****************************************************************************/
/********************* Show a form to select test tags ***********************/
/*****************************************************************************/

void Tag_ShowFormSelTags (const struct Tag_Tags *Tags,
                          MYSQL_RES *mysql_res,
                          bool ShowOnlyEnabledTags)
  {
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
      HTM_TD_TxtColon (Txt_Tags);

      /***** Select all tags *****/
      HTM_TD_Begin ("class=\"REC_C2_BOT LT\"");

	 HTM_TABLE_BeginPadding (2);

	    HTM_TR_Begin (NULL);

	       if (!ShowOnlyEnabledTags)
		  HTM_TD_Empty (1);

	       HTM_TD_Begin ("class=\"LT\"");
		  HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",
				   The_GetSuffix ());
		     HTM_INPUT_CHECKBOX ("AllTags",HTM_DONT_SUBMIT_ON_CHANGE,
					 "value=\"Y\"%s onclick=\"togglecheckChildren(this,'ChkTag');\"",
					 Tags->All ? " checked=\"checked\"" :
						     "");
		     HTM_NBSPTxt (Txt_All_tags);
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
		     HTM_TD_Begin ("class=\"LT\"");
			if (TagHidden)
			   Ico_PutIconOff ("eye-slash.svg",Ico_RED  ,Txt_Tag_not_allowed);
			else
			   Ico_PutIconOff ("eye.svg"      ,Ico_GREEN,Txt_Tag_allowed    );
		     HTM_TD_End ();
		    }

		  Checked = false;
		  if (Tags->List)
		    {
		     Ptr = Tags->List;
		     while (*Ptr)
		       {
			Par_GetNextStrUntilSeparParMult (&Ptr,TagText,Tag_MAX_BYTES_TAG);
			if (!strcmp (row[1],TagText))
			  {
			   Checked = true;
			   break;
			  }
		       }
		    }

		  HTM_TD_Begin ("class=\"LT\"");
		     HTM_LABEL_Begin ("class=\"DAT_%s\"",
				      The_GetSuffix ());
			HTM_INPUT_CHECKBOX ("ChkTag",HTM_DONT_SUBMIT_ON_CHANGE,
					    "value=\"%s\"%s onclick=\"checkParent(this,'AllTags');\"",
					    row[1],
					    Checked ? " checked=\"checked\"" :
						      "");
			HTM_NBSPTxt (row[1]);
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
   extern const char *Txt_No_questions;
   extern const char *Txt_Tags;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumTags;
   unsigned NumTag;
   long TagCod;

   /***** Get current tags in current course *****/
   if ((NumTags = Tag_DB_GetAllTagsFromCurrentCrs (&mysql_res)))
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
		  Tag_PutIconEnable (TagCod);
	       else
		  Tag_PutIconDisable (TagCod);

	       /* Form to rename this tag */
	       HTM_TD_Begin ("class=\"LM\"");
		  Frm_BeginForm (ActRenTag);
		     Par_PutParString (NULL,"OldTagTxt",row[1]);
		     HTM_INPUT_TEXT ("NewTagTxt",Tag_MAX_CHARS_TAG,row[1],
				     HTM_SUBMIT_ON_CHANGE,
				     "size=\"36\" class=\"INPUT_%s\""
				     " required=\"required\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_No_questions);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Put a link and an icon to enable a tag ******************/
/*****************************************************************************/

static void Tag_PutIconEnable (long TagCod)
  {
   HTM_TD_Begin ("class=\"BM\"");
      Frm_BeginForm (ActEnaTag);
	 ParCod_PutPar (ParCod_Tag,TagCod);
	 Ico_PutIconLink ("eye-slash.svg",Ico_RED,ActEnaTag);
      Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/****************** Put a link and an icon to disable a tag ******************/
/*****************************************************************************/

static void Tag_PutIconDisable (long TagCod)
  {
   HTM_TD_Begin ("class=\"BM\"");
      Frm_BeginForm (ActDisTag);
	 ParCod_PutPar (ParCod_Tag,TagCod);
	 Ico_PutIconLink ("eye.svg",Ico_GREEN,ActDisTag);
      Frm_EndForm ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/******************** Get and write tags of a test question ******************/
/*****************************************************************************/

void Tag_GetAndWriteTagsQst (long QstCod)
  {
   extern const char *Txt_no_tags;
   unsigned NumTags;
   unsigned NumTag;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if ((NumTags = Tag_DB_GetTagsQst (&mysql_res,QstCod)))
     {
      /***** Write the tags *****/
      HTM_UL_Begin ("class=\"Tst_TAG_LIST DAT_SMALL_%s\"",
                    The_GetSuffix ());
	 for (NumTag = 0;
	      NumTag < NumTags;
	      NumTag++)
	   {
	    row = mysql_fetch_row (mysql_res);
	    HTM_LI_Begin (NULL);
	       HTM_Txt (row[0]);
	    HTM_LI_End ();
	   }
      HTM_UL_End ();
     }
   else
     {
      HTM_SPAN_Begin ("class=\"DAT_SMALL_%s\"",
                      The_GetSuffix ());
	 HTM_TxtF ("(%s)",Txt_no_tags);
      HTM_SPAN_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show list of question tags *************************/
/*****************************************************************************/

void Tag_ShowTagList (unsigned NumTags,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_no_tags;
   MYSQL_ROW row;
   unsigned NumTag;

   if (NumTags)
     {
      /***** Write the tags *****/
      HTM_UL_Begin (NULL);
	 for (NumTag = 0;
	      NumTag < NumTags;
	      NumTag++)
	   {
	    row = mysql_fetch_row (mysql_res);
	    HTM_LI_Begin (NULL);
	       HTM_Txt (row[0]);
	    HTM_LI_End ();
	   }
      HTM_UL_End ();
     }
   else
      HTM_Txt (Txt_no_tags);
  }
