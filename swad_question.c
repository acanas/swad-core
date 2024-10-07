// swad_question.c: test/exam/game questions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_set.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_question.h"
#include "swad_question_database.h"
#include "swad_question_import.h"
#include "swad_tag_database.h"
#include "swad_test.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

// strings are limited to Qst_MAX_BYTES_ANSWER_TYPE characters
const char *Qst_StrAnswerTypesXML[Qst_NUM_ANS_TYPES] =
  {
   [Qst_ANS_INT            ] = "int",
   [Qst_ANS_FLOAT          ] = "float",
   [Qst_ANS_TRUE_FALSE     ] = "TF",
   [Qst_ANS_UNIQUE_CHOICE  ] = "uniqueChoice",
   [Qst_ANS_MULTIPLE_CHOICE] = "multipleChoice",
   [Qst_ANS_TEXT           ] = "text",
  };

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

// Test images will be saved with:
// - maximum width of Tst_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of Tst_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Qst_IMAGE_SAVED_MAX_WIDTH	768
#define Qst_IMAGE_SAVED_MAX_HEIGHT	768
#define Qst_IMAGE_SAVED_QUALITY		 90	// 1 to 100

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Qst_PutFormToEditQstMedia (const struct Med_Media *Media,int NumMedia,
				       HTM_Attributes_t Attributes);

static void Qst_WriteIntAns (struct Qst_Question *Question,
                             const char *ClassTxt,
                             __attribute__((unused)) const char *ClassFeedback);
static void Qst_WriteFltAns (struct Qst_Question *Question,
                             const char *ClassTxt,
                             __attribute__((unused)) const char *ClassFeedback);
static void Qst_WriteTF_Ans (struct Qst_Question *Question,
                             const char *ClassTxt,
                             __attribute__((unused)) const char *ClassFeedback);
static void Qst_WriteChoAns (struct Qst_Question *Question,
                             const char *ClassTxt,
                             const char *ClassFeedback);

/*****************************************************************************/
/***************************** Test constructor ******************************/
/*****************************************************************************/

void Qst_Constructor (struct Qst_Questions *Questions)
  {
   /***** Reset tags *****/
   Tag_ResetTags (&Questions->Tags);

   /***** Reset answer types *****/
   Questions->AnswerTypes.All = false;
   Questions->AnswerTypes.List[0] = '\0';

   /***** Reset selected order *****/
   Questions->SelectedOrder = Qst_DEFAULT_ORDER;

   /***** Question constructor *****/
   Qst_QstConstructor (&Questions->Question);
  }

/*****************************************************************************/
/****************************** Test destructor ******************************/
/*****************************************************************************/

void Qst_Destructor (struct Qst_Questions *Questions)
  {
   /***** Question destructor *****/
   Qst_QstDestructor (&Questions->Question);

   /***** Free tag list *****/
   Tag_FreeTagsList (&Questions->Tags);
  }

/*****************************************************************************/
/*********************** Request the edition of tests ************************/
/*****************************************************************************/

void Qst_ReqEditQsts (void)
  {
   struct Qst_Questions Questions;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Show form to generate a self-assessment test *****/
   Qst_ShowFormRequestEditQsts (&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/******* Select tags and dates for edition of the self-assessment test *******/
/*****************************************************************************/

void Qst_ShowFormRequestEditQsts (struct Qst_Questions *Questions)
  {
   extern const char *Hlp_ASSESSMENT_Questions;
   extern const char *Txt_No_questions;
   extern const char *Txt_Question_bank;
   extern const char *Txt_Show_questions;
   MYSQL_RES *mysql_res;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Begin box *****/
   Box_BoxBegin (Txt_Question_bank,Qst_PutIconsRequestBankQsts,NULL,
                 Hlp_ASSESSMENT_Questions,Box_NOT_CLOSABLE);

      /***** Get tags already present in the table of questions *****/
      if ((Questions->Tags.Num = Tag_DB_GetAllTagsFromCurrentCrs (&mysql_res)))
	{
	 Frm_BeginForm (ActLstTstQst);
	    Par_PutParUnsigned (NULL,"Order",(unsigned) Qst_DEFAULT_ORDER);

	    HTM_TABLE_BeginCenterPadding (2);

	       /***** Selection of tags *****/
	       Tag_ShowFormSelTags (&Questions->Tags,mysql_res,
				    Tag_SHOW_ALL_TAGS);

	       /***** Selection of types of answers *****/
	       Qst_ShowFormAnswerTypes (&Questions->AnswerTypes);

	       /***** Starting and ending dates in the search *****/
	       Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

	    HTM_TABLE_End ();

	    /***** Send button *****/
	    Btn_PutConfirmButton (Txt_Show_questions);

	 Frm_EndForm ();
	}
      else	// No test questions
	 /***** Warning message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_No_questions);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Show form for select the types of answers *****************/
/*****************************************************************************/

void Qst_ShowFormAnswerTypes (const struct Qst_AnswerTypes *AnswerTypes)
  {
   extern const char *Txt_Types_of_answers;
   extern const char *Txt_All_types_of_answers;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Qst_NUM_ANS_TYPES];
   Qst_AnswerType_t AnsType;
   HTM_Attributes_t Attributes;
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];
   const char *Ptr;

   HTM_TR_Begin (NULL);

      /***** Label *****/
      Frm_LabelColumn ("Frm_C1 RT","",Txt_Types_of_answers);

      /***** Select all types of answers *****/
      HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	 HTM_TABLE_BeginPadding (2);

	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
		  HTM_INPUT_CHECKBOX ("AllAnsTypes",
				      AnswerTypes->All ? HTM_CHECKED :
	        			                 HTM_NO_ATTR,
				      "value=\"Y\""
				      " onclick=\"togglecheckChildren(this,'AnswerType');\"");
		  HTM_Txt (Txt_All_types_of_answers);
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Type of answer *****/
	 for (AnsType  = (Qst_AnswerType_t) 0;
	      AnsType <= (Qst_AnswerType_t) (Qst_NUM_ANS_TYPES - 1);
	      AnsType++)
	   {
	    HTM_TR_Begin (NULL);

	       Attributes = HTM_NO_ATTR;
	       Ptr = AnswerTypes->List;
	       while (*Ptr)
		 {
		  Par_GetNextStrUntilSeparParMult (&Ptr,UnsignedStr,
						   Cns_MAX_DIGITS_UINT);
		  if (Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr) == AnsType)
		    {
		     Attributes = HTM_CHECKED;
		     break;
		    }
		 }
	       HTM_TD_Begin ("class=\"LM\"");
		  HTM_LABEL_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
		     HTM_INPUT_CHECKBOX ("AnswerType",
					 Attributes,
					 "value=\"%u\""
					 " onclick=\"checkParent(this,'AllAnsTypes');\"",
					 (unsigned) AnsType);
		     HTM_Txt (Txt_TST_STR_ANSWER_TYPES[AnsType]);
		  HTM_LABEL_End ();
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

	 HTM_TABLE_End ();
      HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Select test questions for a game ************************/
/*****************************************************************************/

void Qst_RequestSelectQstsForExamSet (struct Exa_Exams *Exams)
  {
   struct Qst_Questions Questions;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Show form to select test for exam *****/
   Qst_ShowFormRequestSelectQstsForExamSet (Exams,&Questions);	// No tags selected

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/******************* Select test questions for a game ************************/
/*****************************************************************************/

void Qst_RequestSelectQstsForGame (struct Gam_Games *Games)
  {
   struct Qst_Questions Questions;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Show form to select test for game *****/
   Qst_ShowFormRequestSelectQstsForGame (Games,&Questions);	// No tags selected

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/************** Show form to select test questions for a exam ****************/
/*****************************************************************************/

void Qst_ShowFormRequestSelectQstsForExamSet (struct Exa_Exams *Exams,
                                              struct Qst_Questions *Questions)
  {
   extern const char *Hlp_ASSESSMENT_Exams_questions;
   extern const char *Txt_No_questions;
   extern const char *Txt_Select_questions;
   extern const char *Txt_Show_questions;
   MYSQL_RES *mysql_res;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Begin box *****/
   Box_BoxBegin (Txt_Select_questions,NULL,NULL,
                 Hlp_ASSESSMENT_Exams_questions,Box_NOT_CLOSABLE);

      /***** Get tags already present in the table of questions *****/
      if ((Questions->Tags.Num = Tag_DB_GetAllTagsFromCurrentCrs (&mysql_res)))
	{
	 Frm_BeginForm (ActLstTstQstForSet);
	    ExaSet_PutParsOneSet (Exams);

	    HTM_TABLE_BeginPadding (2);

	       /***** Selection of tags *****/
	       Tag_ShowFormSelTags (&Questions->Tags,mysql_res,
				    Tag_SHOW_ALL_TAGS);

	       /***** Selection of types of answers *****/
	       Qst_ShowFormAnswerTypes (&Questions->AnswerTypes);

	       /***** Starting and ending dates in the search *****/
	       Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

	    HTM_TABLE_End ();

	    /***** Send button *****/
	    Btn_PutConfirmButton (Txt_Show_questions);

	 Frm_EndForm ();
	}
      else	// No test questions
	 /***** Warning message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_No_questions);

   /***** End box *****/
   Box_BoxEnd ();

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Show form to select test questions for a game ****************/
/*****************************************************************************/

void Qst_ShowFormRequestSelectQstsForGame (struct Gam_Games *Games,
                                           struct Qst_Questions *Questions)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_No_questions;
   extern const char *Txt_Select_questions;
   extern const char *Txt_Show_questions;
   MYSQL_RES *mysql_res;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Begin box *****/
   Box_BoxBegin (Txt_Select_questions,NULL,NULL,
                 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

      /***** Get tags already present in the table of questions *****/
      if ((Questions->Tags.Num = Tag_DB_GetAllTagsFromCurrentCrs (&mysql_res)))
	{
	 Frm_BeginForm (ActGamLstTstQst);
	    Gam_PutPars (Games);

	    HTM_TABLE_BeginPadding (2);

	       /***** Selection of tags *****/
	       Tag_ShowFormSelTags (&Questions->Tags,mysql_res,
				    Tag_SHOW_ALL_TAGS);

	       /***** Starting and ending dates in the search *****/
	       Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

	    HTM_TABLE_End ();

	    /***** Send button *****/
	    Btn_PutConfirmButton (Txt_Show_questions);

	 Frm_EndForm ();
	}
      else	// No test questions
	 /***** Warning message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_No_questions);

   /***** End box *****/
   Box_BoxEnd ();

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Put contextual icons in tests *************************/
/*****************************************************************************/

void Qst_PutIconsRequestBankQsts (__attribute__((unused)) void *Args)
  {
   /***** Put icon to create a new test question *****/
   Ico_PutContextualIconToAdd (ActEdiOneTstQst,NULL,NULL,NULL);

   /***** Put icon to edit tags *****/
   Tag_PutIconToEditTags ();

   /***** Put icon to import questions *****/
   QstImp_PutIconToImportQuestions ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_TESTS);
  }

/*****************************************************************************/
/********************* Put contextual icons in tests *************************/
/*****************************************************************************/

void Qst_PutIconsEditBankQsts (void *Questions)
  {
   /***** Put form to remove selected test questions *****/
   switch (Gbl.Action.Act)
     {
      case ActLstTstQst:	// List selected test questions for edition
      case ActReqRemSevTstQst:	// Request removal of selected questions
      case ActReqRemOneTstQst:	// Request removal of a question
      case ActRemOneTstQst:	// Remove a question
      case ActChgShfTstQst:	// Change shuffle of a question
	 Ico_PutContextualIconToRemove (ActReqRemSevTstQst,NULL,
					Qst_PutParsEditQst,Questions);
	 break;
      default:
	 break;
     }

   if (Gbl.Action.Act != ActEdiOneTstQst)
      /***** Put form to create a new test question *****/
      Ico_PutContextualIconToAdd (ActEdiOneTstQst,NULL,NULL,NULL);

   /***** Put icon to edit tags *****/
   Tag_PutIconToEditTags ();

   /***** Put icon to export questions *****/
   QstImp_PutIconToExportQuestions (Questions);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_TESTS);
  }

/*****************************************************************************/
/********************* List game question for edition ************************/
/*****************************************************************************/

void Qst_ListQuestionForEdition (struct Qst_Question *Question,
                                 unsigned QstInd,bool QuestionExists,
                                 const char *Anchor)
  {
   extern const char *Txt_Question_removed;

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
      Lay_WriteIndex (QstInd,"BIG_INDEX");
      if (QuestionExists)
	 Qst_WriteAnswerType (Question->Answer.Type,"DAT_SMALL");
   HTM_TD_End ();

   /***** Write question code *****/
   HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s CT\"",
                 The_GetSuffix (),The_GetColorRows ());
      HTM_TxtF ("%ld&nbsp;",Question->QstCod);
   HTM_TD_End ();

   /***** Write the question tags *****/
   HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
      if (QuestionExists)
	 Tag_GetAndWriteTagsQst (Question->QstCod);
   HTM_TD_End ();

   /***** Write stem (row[3]) and media *****/
   HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
      HTM_ARTICLE_Begin (Anchor);
	 if (QuestionExists)
	   {
	    /* Write stem */
	    Qst_WriteQstStem (Question->Stem,"Qst_TXT",HidVis_VISIBLE);

	    /* Show media */
	    Med_ShowMedia (&Question->Media,
			   "Tst_MED_EDIT_LIST_CONT",
			   "Tst_MED_EDIT_LIST");

	    /* Show feedback */
	    Qst_WriteQstFeedback (Question->Feedback,"Qst_TXT_LIGHT");

	    /* Show answers */
	    Qst_WriteAnswers (Question,"Qst_TXT","Qst_TXT_LIGHT");
	   }
	 else
	   {
	    HTM_SPAN_Begin ("class=\"DAT_LIGHT_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_Question_removed);
	    HTM_SPAN_End ();
	   }
      HTM_ARTICLE_End ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/************************** Write the type of answer *************************/
/*****************************************************************************/

void Qst_WriteAnswerType (Qst_AnswerType_t AnswerType,const char *Class)
  {
   extern const char *Txt_TST_STR_ANSWER_TYPES[Qst_NUM_ANS_TYPES];

   HTM_DIV_Begin ("class=\"%s_%s\"",Class,The_GetSuffix ());
      HTM_Txt (Txt_TST_STR_ANSWER_TYPES[AnswerType]);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Write the stem of a test question *********************/
/*****************************************************************************/

void Qst_WriteQstStem (const char *Stem,const char *ClassStem,
		       HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   unsigned long StemLength;
   char *StemRigorousHTML;

   /***** DIV begin *****/
   HTM_DIV_Begin ("class=\"%s_%s\"",ClassStem,The_GetSuffix ());

      /***** Write stem *****/
      if (Stem && HiddenOrVisible == HidVis_VISIBLE)
	{
	 if (Stem[0])
	   {
	    /* Convert the stem, that is in HTML, to rigorous HTML */
	    StemLength = strlen (Stem) * Str_MAX_BYTES_PER_CHAR;
	    if ((StemRigorousHTML = malloc (StemLength + 1)) == NULL)
	       Err_NotEnoughMemoryExit ();
	    Str_Copy (StemRigorousHTML,Stem,StemLength);

	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      StemRigorousHTML,StemLength,
			      Str_DONT_REMOVE_SPACES);

	    /* Write stem text */
	    HTM_Txt (StemRigorousHTML);

	    /* Free memory allocated for the stem */
	    free (StemRigorousHTML);
	   }
	}
      else
	 Ico_PutIconNotVisible ();

   /***** DIV end *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************* Put form to upload a new image for a test question ************/
/*****************************************************************************/

static void Qst_PutFormToEditQstMedia (const struct Med_Media *Media,int NumMedia,
				       HTM_Attributes_t Attributes)
  {
   extern const char *Txt_No_image_video;
   extern const char *Txt_Current_image_video;
   extern const char *Txt_Change_image_video;
   static unsigned UniqueId = 0;
   struct Med_ParUpload ParUploadMedia;
   char *ClassInput;

   if (Media->Name[0])
     {
      /***** Set names of parameters depending on number of image in form *****/
      Med_SetParsNames (&ParUploadMedia,NumMedia);

      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"Tst_MED_EDIT_FORM\"");

	 /***** Choice 1: No media *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_INPUT_RADIO (ParUploadMedia.Action,
			     Attributes,
			     "value=\"%u\"",(unsigned) Med_ACTION_NO_MEDIA);
	    HTM_Txt (Txt_No_image_video);
	 HTM_LABEL_End ();
	 HTM_BR ();

	 /***** Choice 2: Current media *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_INPUT_RADIO (ParUploadMedia.Action,
			     Attributes | HTM_CHECKED,
			     "value=\"%u\"",(unsigned) Med_ACTION_KEEP_MEDIA);
	    HTM_Txt (Txt_Current_image_video);
	 HTM_LABEL_End ();
	 Med_ShowMedia (Media,
			"Tst_MED_EDIT_ONE_CONT",
			"Tst_MED_EDIT_ONE");

	 /***** Choice 3: Change media *****/
	 UniqueId++;
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_INPUT_RADIO (ParUploadMedia.Action,
			     Attributes,
			     "id=\"chg_img_%u\" value=\"%u\"",
			     UniqueId,(unsigned) Med_ACTION_NEW_MEDIA);
	    HTM_TxtColonNBSP (Txt_Change_image_video);
	 HTM_LABEL_End ();
	 if (asprintf (&ClassInput,"Tst_MED_INPUT INPUT_%s",
	               The_GetSuffix ()) < 0)
	    Err_NotEnoughMemoryExit ();
	 Med_PutMediaUploader (NumMedia,ClassInput);
	 free (ClassInput);

      /***** End container *****/
      HTM_DIV_End ();
     }
   else	// No current image
     {
      /***** Attached media *****/
      if (asprintf (&ClassInput,"Tst_MED_INPUT INPUT_%s",The_GetSuffix ()) < 0)
	 Err_NotEnoughMemoryExit ();
      Med_PutMediaUploader (NumMedia,ClassInput);
      free (ClassInput);
     }
  }

/*****************************************************************************/
/******************* Write the feedback of a test question *******************/
/*****************************************************************************/

void Qst_WriteQstFeedback (const char *Feedback,const char *ClassFeedback)
  {
   unsigned long FeedbackLength;
   char *FeedbackRigorousHTML;

   if (Feedback)
      if (Feedback[0])
	{
	 /***** Convert the feedback, that is in HTML, to rigorous HTML *****/
	 FeedbackLength = strlen (Feedback) * Str_MAX_BYTES_PER_CHAR;
	 if ((FeedbackRigorousHTML = malloc (FeedbackLength + 1)) == NULL)
	    Err_NotEnoughMemoryExit ();
	 Str_Copy (FeedbackRigorousHTML,Feedback,FeedbackLength);
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   FeedbackRigorousHTML,FeedbackLength,
			   Str_DONT_REMOVE_SPACES);

	 /***** Write the feedback *****/
	 HTM_DIV_Begin ("class=\"%s_%s\"",ClassFeedback,The_GetSuffix ());
	    HTM_Txt (FeedbackRigorousHTML);
	 HTM_DIV_End ();

	 /***** Free memory allocated for the feedback *****/
	 free (FeedbackRigorousHTML);
	}
  }

/*****************************************************************************/
/***************** List several test questions for edition *******************/
/*****************************************************************************/

void Qst_ListQuestionsToEdit (void)
  {
   struct Qst_Questions Questions;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParsTst (&Questions,Tst_EDIT_QUESTIONS))	// Get parameters from the form
     {
      /***** Get question codes from database *****/
      if ((Questions.NumQsts = Qst_DB_GetQsts (&mysql_res,&Questions)))
        {
	 /* Contextual menu */
	 if (QstImp_GetCreateXMLParFromForm ())
	   {
            Mnu_ContextMenuBegin ();
	       QstImp_CreateXML (Questions.NumQsts,mysql_res);	// Create XML file with exported questions...
								// ...and put a link to download it
            Mnu_ContextMenuEnd ();
	   }

	 /* Show the table with the questions */
         Qst_ListOneOrMoreQstsForEdition (&Questions,mysql_res);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Qst_ShowFormRequestEditQsts (&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/************ List several test questions for selection for exam *************/
/*****************************************************************************/

void Qst_ListQuestionsToSelectForExamSet (struct Exa_Exams *Exams)
  {
   struct Qst_Questions Questions;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParsTst (&Questions,Tst_SELECT_QUESTIONS_FOR_EXAM))	// Get parameters from the form
     {
      if ((Questions.NumQsts = Qst_DB_GetQsts (&mysql_res,&Questions)))
	 /* Show the table with the questions */
         Qst_ListOneOrMoreQstsForSelectionForExamSet (Exams,Questions.NumQsts,mysql_res);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Qst_ShowFormRequestSelectQstsForExamSet (Exams,&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/************ List several test questions for selection for game *************/
/*****************************************************************************/

void Qst_ListQuestionsToSelectForGame (struct Gam_Games *Games)
  {
   struct Qst_Questions Questions;
   MYSQL_RES *mysql_res;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters, query the database and list the questions *****/
   if (Tst_GetParsTst (&Questions,Tst_SELECT_QUESTIONS_FOR_GAME))	// Get parameters from the form
     {
      if ((Questions.NumQsts = Qst_DB_GetQsts (&mysql_res,&Questions)))
	 /* Show the table with the questions */
         Qst_ListOneOrMoreQstsForSelectionForGame (Games,Questions.NumQsts,mysql_res);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /* Show the form again */
      Qst_ShowFormRequestSelectQstsForGame (Games,&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/****************** List for edition one or more test questions **************/
/*****************************************************************************/

void Qst_ListOneOrMoreQstsForEdition (struct Qst_Questions *Questions,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Hlp_ASSESSMENT_Questions;
   extern const char *Txt_Questions;
   unsigned QstInd;
   MYSQL_ROW row;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_Questions,Qst_PutIconsEditBankQsts,Questions,
		      Hlp_ASSESSMENT_Questions,Box_NOT_CLOSABLE,5);

      /***** Write the heading *****/
      Qst_WriteHeadingRowQuestionsForEdition (Questions);

      /***** Write rows *****/
      for (QstInd = 0, The_ResetRowColor ();
	   QstInd < Questions->NumQsts;
	   QstInd++, The_ChangeRowColor ())
	{
	 /***** Create test question *****/
	 Qst_QstConstructor (&Questions->Question);

	 /***** Get question code (row[0]) *****/
	 row = mysql_fetch_row (mysql_res);
	 if ((Questions->Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Err_WrongQuestionExit ();

	 /***** Write question row *****/
	 Qst_WriteQuestionListing (Questions,QstInd);

	 /***** Destroy test question *****/
	 Qst_QstDestructor (&Questions->Question);
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/*********** Write heading row in listing of questions for edition ***********/
/*****************************************************************************/

void Qst_WriteHeadingRowQuestionsForEdition (struct Qst_Questions *Questions)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Date;
   extern const char *Txt_Tags;
   extern const char *Txt_Shuffle;
   extern const char *Txt_TST_STR_ORDER_FULL[Qst_NUM_TYPES_ORDER_QST];
   extern const char *Txt_TST_STR_ORDER_SHORT[Qst_NUM_TYPES_ORDER_QST];
   Qst_QuestionsOrder_t Order;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** First columns *****/
      HTM_TH_Empty (1);
      HTM_TH (Txt_No_INDEX,HTM_HEAD_CENTER);
      HTM_TH (Txt_Code    ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Date    ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Tags    ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Shuffle ,HTM_HEAD_CENTER);

      /***** Columns which data can be ordered *****/
      /* Stem and answers of question */
      /* Number of times that the question has been answered */
      /* Average score */
      for (Order  = (Qst_QuestionsOrder_t) 0;
	   Order <= (Qst_QuestionsOrder_t) (Qst_NUM_TYPES_ORDER_QST - 1);
	   Order++)
	{
         HTM_TH_Begin (HTM_HEAD_LEFT);

	    if (Questions->NumQsts > 1)
	      {
	       Frm_BeginForm (ActLstTstQst);
		  Qst_PutParsEditQst (Questions);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_TST_STR_ORDER_FULL[Order],
		                           "class=\"BT_LINK\"");
		     if (Order == Questions->SelectedOrder)
			HTM_U_Begin ();
	      }
	    HTM_Txt (Txt_TST_STR_ORDER_SHORT[Order]);
	    if (Questions->NumQsts > 1)
	      {
		     if (Order == Questions->SelectedOrder)
			HTM_U_End ();
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }

	 HTM_TH_End ();
	}

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********** Write question row in listing of questions for edition ***********/
/*****************************************************************************/

void Qst_WriteQuestionListing (struct Qst_Questions *Questions,unsigned QstInd)
  {
   static unsigned UniqueId = 0;
   char *Id;

   /***** Get and show question data *****/
   if (Qst_GetQstDataByCod (&Questions->Question))
     {
      /***** Begin table row *****/
      HTM_TR_Begin (NULL);

	 /***** Icons *****/
	 HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());

	    /* Write icon to remove the question */
	    Ico_PutContextualIconToRemove (ActReqRemOneTstQst,NULL,
					   Qst_PutParsEditQst,Questions);

	    /* Write icon to edit the question */
	    Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
					 Qst_PutParQstCod,&Questions->Question.QstCod);

	 HTM_TD_End ();

	 /* Number of question and answer type */
	 HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	    Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	    Qst_WriteAnswerType (Questions->Question.Answer.Type,"DAT_SMALL");
	 HTM_TD_End ();

	 /* Question code */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    HTM_TxtF ("%ld&nbsp;",Questions->Question.QstCod);
	 HTM_TD_End ();

	 /* Date (row[0] has the UTC date-time) */
	 if (asprintf (&Id,"tst_date_%u",++UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"CT DAT_SMALL_%s %s\"",
		       Id,The_GetSuffix (),The_GetColorRows ());
	    Dat_WriteLocalDateHMSFromUTC (Id,Questions->Question.EditTime,
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					  Dat_WRITE_TODAY |
					  Dat_WRITE_DATE_ON_SAME_DAY |
					  Dat_WRITE_HOUR |
					  Dat_WRITE_MINUTE |
					  Dat_WRITE_SECOND);
	 HTM_TD_End ();
	 free (Id);

	 /* Question tags */
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	    Tag_GetAndWriteTagsQst (Questions->Question.QstCod);
	 HTM_TD_End ();

	 /* Shuffle (row[2]) */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    if (Questions->Question.Answer.Type == Qst_ANS_UNIQUE_CHOICE ||
		Questions->Question.Answer.Type == Qst_ANS_MULTIPLE_CHOICE)
	      {
	       Frm_BeginForm (ActChgShfTstQst);
		  Qst_PutParsEditQst (Questions);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Questions->SelectedOrder);
		  HTM_INPUT_CHECKBOX ("Shuffle",
				      (Questions->Question.Answer.Shuffle ? HTM_CHECKED :
									    HTM_NO_ATTR) |
				      HTM_SUBMIT_ON_CHANGE,
				      "value=\"Y\"");
	       Frm_EndForm ();
	      }
	 HTM_TD_End ();

	 /* Stem (row[3]) */
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	    Qst_WriteQstStem (Questions->Question.Stem,"Qst_TXT",HidVis_VISIBLE);

	    /***** Get and show media (row[5]) *****/
	    Med_ShowMedia (&Questions->Question.Media,
			   "Tst_MED_EDIT_LIST_CONT",
			   "Tst_MED_EDIT_LIST");

	    /* Feedback (row[4]) and answers */
	    Qst_WriteQstFeedback (Questions->Question.Feedback,"Qst_TXT_LIGHT");
	    Qst_WriteAnswers (&Questions->Question,"Qst_TXT","Qst_TXT_LIGHT");
	 HTM_TD_End ();

	 /* Number of times this question has been answered */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    HTM_UnsignedLong (Questions->Question.NumHits);
	 HTM_TD_End ();

	 /* Average score */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    if (Questions->Question.NumHits)
	       HTM_Double2Decimals (Questions->Question.Score /
				    (double) Questions->Question.NumHits);
	    else
	       HTM_Txt ("N.A.");
	 HTM_TD_End ();

	 /* Number of times this question has been answered (not blank) */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    HTM_UnsignedLong (Questions->Question.NumHitsNotBlank);
	 HTM_TD_End ();

	 /* Average score (not blank) */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    if (Questions->Question.NumHitsNotBlank)
	       HTM_Double2Decimals (Questions->Question.Score /
				    (double) Questions->Question.NumHitsNotBlank);
	    else
	       HTM_Txt ("N.A.");
	 HTM_TD_End ();

      /***** End table row *****/
      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/*************** List for selection one or more test questions ***************/
/*****************************************************************************/

void Qst_ListOneOrMoreQstsForSelectionForExamSet (struct Exa_Exams *Exams,
						  unsigned NumQsts,
                                                  MYSQL_RES *mysql_res)
  {
   extern const char *Hlp_ASSESSMENT_Exams_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Date;
   extern const char *Txt_Tags;
   extern const char *Txt_Type;
   extern const char *Txt_Shuffle;
   extern const char *Txt_Question;
   extern const char *Txt_Add_questions;
   unsigned QstInd;
   struct Qst_Question Question;
   MYSQL_ROW row;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Questions,NULL,NULL,
		 Hlp_ASSESSMENT_Exams_questions,Box_NOT_CLOSABLE);

      /***** Begin form *****/
      Frm_BeginForm (ActAddQstToExa);
	 ExaSet_PutParsOneSet (Exams);

	 /***** Select all questions *****/
	 Qst_PutCheckboxToSelectAllQuestions ();

	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (5);

	    /***** Write the heading *****/
	    HTM_TR_Begin (NULL);
	       HTM_TH_Empty (1);
	       HTM_TH (Txt_No_INDEX,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Code    ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Date    ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Tags    ,HTM_HEAD_LEFT  );
	       HTM_TH (Txt_Type    ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Shuffle ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Question,HTM_HEAD_CENTER);
	    HTM_TR_End ();

	    /***** Write rows *****/
	    for (QstInd = 0, The_ResetRowColor ();
		 QstInd < NumQsts;
		 QstInd++, The_ChangeRowColor ())
	      {
	       /* Create test question */
	       Qst_QstConstructor (&Question);

	       /* Get question code (row[0]) */
	       row = mysql_fetch_row (mysql_res);
	       if ((Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
		  Err_WrongQuestionExit ();

	       /* Write question row */
	       Qst_WriteQuestionRowForSelection (QstInd,&Question);

	       /* Destroy test question */
	       Qst_QstDestructor (&Question);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Button to add questions *****/
	 Btn_PutCreateButton (Txt_Add_questions);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** List for selection one or more test questions ***************/
/*****************************************************************************/

void Qst_ListOneOrMoreQstsForSelectionForGame (struct Gam_Games *Games,
					       unsigned NumQsts,
                                               MYSQL_RES *mysql_res)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Date;
   extern const char *Txt_Tags;
   extern const char *Txt_Type;
   extern const char *Txt_Shuffle;
   extern const char *Txt_Question;
   extern const char *Txt_Add_questions;
   unsigned QstInd;
   struct Qst_Question Question;
   MYSQL_ROW row;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Questions,NULL,NULL,
		 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

      /***** Begin form *****/
      Frm_BeginForm (ActAddTstQstToGam);
	 Gam_PutPars (Games);

	 /***** Select all questions *****/
	 Qst_PutCheckboxToSelectAllQuestions ();

	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (5);

	    /***** Write the heading *****/
	    HTM_TR_Begin (NULL);
	       HTM_TH_Empty (1);
	       HTM_TH (Txt_No_INDEX,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Code    ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Date    ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Tags    ,HTM_HEAD_LEFT  );
	       HTM_TH (Txt_Type    ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Shuffle ,HTM_HEAD_CENTER);
	       HTM_TH (Txt_Question,HTM_HEAD_CENTER);
	    HTM_TR_End ();

	    /***** Write rows *****/
	    for (QstInd = 0, The_ResetRowColor ();
		 QstInd < NumQsts;
		 QstInd++, The_ChangeRowColor ())
	      {
	       /* Create test question */
	       Qst_QstConstructor (&Question);

	       /* Get question code (row[0]) */
	       row = mysql_fetch_row (mysql_res);
	       if ((Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
		  Err_WrongQuestionExit ();

	       /* Write question row */
	       Qst_WriteQuestionRowForSelection (QstInd,&Question);

	       /* Destroy test question */
	       Qst_QstDestructor (&Question);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Button to add questions *****/
	 Btn_PutCreateButton (Txt_Add_questions);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Checkbox to select all questions to add them to set/game **********/
/*****************************************************************************/

void Qst_PutCheckboxToSelectAllQuestions (void)
  {
   extern const char *Txt_All_questions;

   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_INPUT_CHECKBOX ("AllQsts",
			  HTM_NO_ATTR,
			  "value=\"Y\""
			  " onclick=\"togglecheckChildren(this,'QstCods');\"");
      HTM_Txt (Txt_All_questions);
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/********************** Write question row for selection *********************/
/*****************************************************************************/

void Qst_WriteQuestionRowForSelection (unsigned QstInd,
                                       struct Qst_Question *Question)
  {
   extern const char *Txt_TST_STR_ANSWER_TYPES[Qst_NUM_ANS_TYPES];
   static unsigned UniqueId = 0;
   char *Id;

   /***** Get and show questvoidion data *****/
   if (Qst_GetQstDataByCod (Question))
     {
      /***** Begin table row *****/
      HTM_TR_Begin (NULL);

	 /* Write checkbox to select the question */
	 HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
	    HTM_INPUT_CHECKBOX ("QstCods",
				HTM_NO_ATTR,
				"value=\"%ld\""
				" onclick=\"checkParent(this,'AllQsts');\"",
				Question->QstCod);
   	 HTM_TD_End ();

	 /* Write number of question */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    HTM_TxtF ("%u&nbsp;",QstInd + 1);
	 HTM_TD_End ();

	 /* Write question code */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    HTM_TxtF ("%ld&nbsp;",Question->QstCod);
	 HTM_TD_End ();

	 /* Write the date (row[0] has the UTC date-time) */
	 if (asprintf (&Id,"tst_date_%u",++UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"CT DAT_SMALL_%s %s\">",
		       Id,The_GetSuffix (),The_GetColorRows ());
	    Dat_WriteLocalDateHMSFromUTC (Id,Question->EditTime,
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					  Dat_WRITE_TODAY |
					  Dat_WRITE_DATE_ON_SAME_DAY |
					  Dat_WRITE_HOUR |
					  Dat_WRITE_MINUTE |
					  Dat_WRITE_SECOND);
	 HTM_TD_End ();
	 free (Id);

	 /* Write the question tags */
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	    Tag_GetAndWriteTagsQst (Question->QstCod);
	 HTM_TD_End ();

	 /* Write the question type */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    HTM_TxtF ("%s&nbsp;",Txt_TST_STR_ANSWER_TYPES[Question->Answer.Type]);
	 HTM_TD_End ();

	 /* Write if shuffle is allowed */
	 HTM_TD_Begin ("class=\"CT DAT_SMALL_%s %s\"",
	               The_GetSuffix (),The_GetColorRows ());
	    HTM_INPUT_CHECKBOX ("Shuffle",
				(Question->Answer.Shuffle ? HTM_CHECKED :
						            HTM_NO_ATTR) |
				HTM_DISABLED,
				"value=\"Y\"");
	 HTM_TD_End ();

	 /* Write stem */
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	    Qst_WriteQstStem (Question->Stem,"Qst_TXT",HidVis_VISIBLE);

	    /***** Get and show media *****/
	    Med_ShowMedia (&Question->Media,
			   "Tst_MED_EDIT_LIST_CONT",
			   "Tst_MED_EDIT_LIST");

	    /* Write feedback */
	    Qst_WriteQstFeedback (Question->Feedback,"Qst_TXT_LIGHT");

	    /* Write answers */
	    Qst_WriteAnswers (Question,"Qst_TXT","Qst_TXT_LIGHT");
	 HTM_TD_End ();

      /***** End table row *****/
      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/************ Put hidden parameters for edition of test questions ************/
/*****************************************************************************/

void Qst_PutParsEditQst (void *Questions)
  {
   if (Questions)
     {
      Par_PutParChar ("AllTags",((struct Qst_Questions *) Questions)->Tags.All ? 'Y' :
					                                         'N');
      Par_PutParString (NULL,"ChkTag",((struct Qst_Questions *) Questions)->Tags.List ? ((struct Qst_Questions *) Questions)->Tags.List :
								                        "");
      Par_PutParChar ("AllAnsTypes",((struct Qst_Questions *) Questions)->AnswerTypes.All ? 'Y' :
								                            'N');
      Par_PutParString (NULL,"AnswerType",((struct Qst_Questions *) Questions)->AnswerTypes.List);

      ParCod_PutPar (ParCod_Qst,((struct Qst_Questions *) Questions)->Question.QstCod);
      // if (Test->NumQsts == 1)
      //    Par_PutParChar ("OnlyThisQst",'Y'); // If there are only one row, don't list again after removing
      Dat_WriteParsIniEndDates ();
     }
  }

/*****************************************************************************/
/**************** Get and write the answers of a test question ***************/
/*****************************************************************************/

void Qst_WriteAnswers (struct Qst_Question *Question,
                       const char *ClassTxt,
                       const char *ClassFeedback)
  {
   void (*Tst_WriteAns[Qst_NUM_ANS_TYPES]) (struct Qst_Question *Question,
                                            const char *ClassTxt,
                                            const char *ClassFeedback) =
    {
     [Qst_ANS_INT            ] = Qst_WriteIntAns,
     [Qst_ANS_FLOAT          ] = Qst_WriteFltAns,
     [Qst_ANS_TRUE_FALSE     ] = Qst_WriteTF_Ans,
     [Qst_ANS_UNIQUE_CHOICE  ] = Qst_WriteChoAns,
     [Qst_ANS_MULTIPLE_CHOICE] = Qst_WriteChoAns,
     [Qst_ANS_TEXT           ] = Qst_WriteChoAns,
    };

   /***** Write answers *****/
   Tst_WriteAns[Question->Answer.Type] (Question,ClassTxt,ClassFeedback);
  }

/*****************************************************************************/
/*********************** List a test question for edition ********************/
/*****************************************************************************/

void Qst_ListOneQstToEdit (struct Qst_Questions *Questions)
  {
   extern const char *Hlp_ASSESSMENT_Questions;
   extern const char *Txt_Questions;

   /***** List only one question *****/
   Questions->NumQsts = 1;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_Questions,Qst_PutIconsEditBankQsts,Questions,
		      Hlp_ASSESSMENT_Questions,Box_NOT_CLOSABLE,5);

      /***** Write the heading *****/
      Qst_WriteHeadingRowQuestionsForEdition (Questions);

      /***** Write question row *****/
      Qst_WriteQuestionListing (Questions,0);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/****************** Write integer answer when editing a test *****************/
/*****************************************************************************/

static void Qst_WriteIntAns (struct Qst_Question *Question,
                             const char *ClassTxt,
                             __attribute__((unused)) const char *ClassFeedback)
  {
   HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
      HTM_TxtF ("(%ld)",Question->Answer.Integer);
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/****************** Write float answer when editing a test *******************/
/*****************************************************************************/

static void Qst_WriteFltAns (struct Qst_Question *Question,
                             const char *ClassTxt,
                             __attribute__((unused)) const char *ClassFeedback)
  {
   HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
      HTM_Txt ("([");
      HTM_Double (Question->Answer.FloatingPoint[0]);
      HTM_Txt ("; ");
      HTM_Double (Question->Answer.FloatingPoint[1]);
      HTM_Txt ("])");
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/*********** Write false / true answer when listing test questions ***********/
/*****************************************************************************/

static void Qst_WriteTF_Ans (struct Qst_Question *Question,
                             const char *ClassTxt,
                             __attribute__((unused)) const char *ClassFeedback)
  {
   /***** Write answer *****/
   HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
      HTM_Txt ("(");
      Qst_WriteAnsTF (Question->Answer.TF);
      HTM_Txt (")");
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/**** Write single or multiple choice answer when listing test questions *****/
/*****************************************************************************/

static void Qst_WriteChoAns (struct Qst_Question *Question,
                             const char *ClassTxt,
                             const char *ClassFeedback)
  {
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   unsigned NumOpt;

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Change format of answers feedback *****/
   Qst_ChangeFormatAnswersFeedback (Question);

   HTM_TABLE_BeginPadding (2);
      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 HTM_TR_Begin (NULL);

	    /* Put an icon that indicates whether the answer is correct or wrong */
	    HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
	       if (Question->Answer.Options[NumOpt].Correct)
		  Ico_PutIcon ("check.svg",Ico_BLACK,
		               Txt_TST_Answer_given_by_the_teachers,"CONTEXT_ICO16x16");
	    HTM_TD_End ();

	    /* Write the number of option */
	    HTM_TD_Begin ("class=\"LT %s_%s\"",ClassTxt,The_GetSuffix ());
	       HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LT\"");

	       /* Write the text of the answer and the media */
	       HTM_DIV_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
		  HTM_Txt (Question->Answer.Options[NumOpt].Text);
		  Med_ShowMedia (&Question->Answer.Options[NumOpt].Media,
				 "Tst_MED_EDIT_LIST_CONT",
				 "Tst_MED_EDIT_LIST");
	       HTM_DIV_End ();

	       /* Write the text of the feedback */
	       HTM_DIV_Begin ("class=\"%s_%s\"",ClassFeedback,The_GetSuffix ());
		  HTM_Txt (Question->Answer.Options[NumOpt].Feedback);
	       HTM_DIV_End ();

	    HTM_TD_End ();

	 HTM_TR_End ();
	}
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/**************** Get correct answer for each type of answer *****************/
/*****************************************************************************/

void Qst_GetCorrectIntAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions = Qst_DB_GetTextOfAnswers (&mysql_res,Question->QstCod);

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get correct answer *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&Question->Answer.Integer) != 1)
      Err_WrongAnswerExit ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

void Qst_GetCorrectFltAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;
   double Tmp;

   /***** Query database *****/
   Question->Answer.NumOptions = Qst_DB_GetTextOfAnswers (&mysql_res,Question->QstCod);

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Get float range *****/
   for (NumOpt = 0;
	NumOpt < 2;
	NumOpt++)
     {
      row = mysql_fetch_row (mysql_res);
      Question->Answer.FloatingPoint[NumOpt] = Str_GetDoubleFromStr (row[0]);
     }
   if (Question->Answer.FloatingPoint[0] >
       Question->Answer.FloatingPoint[1]) 	// The maximum and the minimum are swapped
    {
      /* Swap maximum and minimum */
      Tmp = Question->Answer.FloatingPoint[0];
      Question->Answer.FloatingPoint[0] = Question->Answer.FloatingPoint[1];
      Question->Answer.FloatingPoint[1] = Tmp;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

void Qst_GetCorrectTF_AnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions = Qst_DB_GetTextOfAnswers (&mysql_res,Question->QstCod);

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer *****/
   row = mysql_fetch_row (mysql_res);
   Question->Answer.TF = row[0][0];

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

void Qst_GetCorrectChoAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions = Qst_DB_GetQstAnswersCorr (&mysql_res,Question->QstCod);

   /***** Get options *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /* Get next answer */
      row = mysql_fetch_row (mysql_res);

      /* Assign correctness (row[0]) of this answer (this option) */
      Question->Answer.Options[NumOpt].Correct = (row[0][0] == 'Y');
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

void Qst_GetCorrectTxtAnswerFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions = Qst_DB_GetTextOfAnswers (&mysql_res,Question->QstCod);

   /***** Get text and correctness of answers for this question
          from database (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Allocate memory for text in this choice answer *****/
      if (!Qst_AllocateTextChoiceAnswer (Question,NumOpt))
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();

      /***** Copy answer text (row[0]) ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[0],
                Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
     }

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test *****************/
/*****************************************************************************/

void Qst_WriteAnsTF (char AnsTF)
  {
   extern const char *Txt_TF_QST[2];

   switch (AnsTF)
     {
      case 'T':		// true
         HTM_Txt (Txt_TF_QST[0]);
         break;
      case 'F':		// false
         HTM_Txt (Txt_TF_QST[1]);
         break;
      default:		// no answer
         HTM_NBSP ();
         break;
     }
  }

/*****************************************************************************/
/*************** Write parameter with the code of a question *****************/
/*****************************************************************************/

void Qst_WriteParQstCod (unsigned QstInd,long QstCod)
  {
   char StrAns[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   snprintf (StrAns,sizeof (StrAns),"Qst%010u",QstInd);
   Par_PutParLong (NULL,StrAns,QstCod);
  }

/*****************************************************************************/
/********************* Check if number of answers is one *********************/
/*****************************************************************************/

void Qst_CheckIfNumberOfAnswersIsOne (const struct Qst_Question *Question)
  {
   if (Question->Answer.NumOptions != 1)
      Err_WrongAnswerExit ();
  }

/*****************************************************************************/
/***************** Change format of answers text / feedback ******************/
/*****************************************************************************/

void Qst_ChangeFormatAnswersText (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Change format of answers text *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
      /* Convert answer text, that is in HTML, to rigorous HTML */
      if (Question->Answer.Options[NumOpt].Text[0])
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   Question->Answer.Options[NumOpt].Text,
			   Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,
			   Str_DONT_REMOVE_SPACES);
  }

void Qst_ChangeFormatAnswersFeedback (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Change format of answers text and feedback *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
      /* Convert answer feedback, that is in HTML, to rigorous HTML */
      if (Question->Answer.Options[NumOpt].Feedback)
	 if (Question->Answer.Options[NumOpt].Feedback[0])
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      Question->Answer.Options[NumOpt].Feedback,
			      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,
			      Str_DONT_REMOVE_SPACES);
  }

/*****************************************************************************/
/** Convert a string with the type of answer in database to type of answer ***/
/*****************************************************************************/

Qst_AnswerType_t Qst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeDB)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
   Qst_AnswerType_t AnsType;

   if (StrAnsTypeDB != NULL)
      if (StrAnsTypeDB[0])
	 for (AnsType  = (Qst_AnswerType_t) 0;
	      AnsType <= (Qst_AnswerType_t) (Qst_NUM_ANS_TYPES - 1);
	      AnsType++)
	    if (!strcmp (StrAnsTypeDB,Qst_DB_StrAnswerTypes[AnsType]))
	       return AnsType;

   return Qst_ANS_UNKNOWN;
  }

/*****************************************************************************/
/************ Convert a string with an unsigned to answer type ***************/
/*****************************************************************************/

Qst_AnswerType_t Qst_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr)
  {
   unsigned AnsType;

   if (sscanf (UnsignedStr,"%u",&AnsType) != 1)
      Err_WrongAnswerExit ();
   if (AnsType >= Qst_NUM_ANS_TYPES)
      Err_WrongAnswerExit ();
   return (Qst_AnswerType_t) AnsType;
  }

/*****************************************************************************/
/******************** Show form to edit one test question ********************/
/*****************************************************************************/

void Qst_ShowFormEditOneQst (void)
  {
   extern const char *Txt_Question_removed;
   struct Qst_Question Question;
   Frm_PutForm_t PutFormToEditQuestion;

   /***** Create test question *****/
   Qst_QstConstructor (&Question);

   /***** Get question data *****/
   Question.QstCod = ParCod_GetPar (ParCod_Qst);
   if (Question.QstCod <= 0)	// New question
      PutFormToEditQuestion = Frm_PUT_FORM;
   else
      PutFormToEditQuestion = Qst_GetQstDataByCod (&Question) ? Frm_PUT_FORM :
								Frm_DONT_PUT_FORM;

   /***** Put form to edit question *****/
   switch (PutFormToEditQuestion)
     {
      case Frm_DONT_PUT_FORM:
	 Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);
	 break;
      case Frm_PUT_FORM:
         Qst_PutFormEditOneQst (&Question);
	 break;
     }

   /***** Destroy test question *****/
   Qst_QstDestructor (&Question);
  }

/*****************************************************************************/
/******************** Show form to edit one test question ********************/
/*****************************************************************************/

// This function may be called from three places:
// 1. By clicking "New question" icon
// 2. By clicking "Edit" icon in a listing of existing questions
// 3. From the action associated to reception of a question, on error in the parameters received from the form

void Qst_PutFormEditOneQst (struct Qst_Question *Question)
  {
   extern const char *Hlp_ASSESSMENT_Questions_writing_a_question;
   extern const char *Txt_Question_code_X;
   extern const char *Txt_Question;
   extern const char *Txt_Tags;
   extern const char *Txt_new_tag;
   extern const char *Txt_Wording;
   extern const char *Txt_Feedback;
   extern const char *Txt_optional;
   extern const char *Txt_Type;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Qst_NUM_ANS_TYPES];
   extern const char *Txt_Answers;
   extern const char *Txt_Integer_number;
   extern const char *Txt_Real_number_between_A_and_B_1;
   extern const char *Txt_Real_number_between_A_and_B_2;
   extern const char *Txt_TF_QST[2];
   extern const char *Txt_Shuffle;
   extern const char *Txt_Expand;
   extern const char *Txt_Contract;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Create;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumTags;
   unsigned IndTag;
   unsigned NumTag;
   unsigned NumOpt;
   Qst_AnswerType_t AnsType;
   bool IsThisTag;
   bool TagFound;
   bool AnswerHasContent;
   bool DisplayRightColumn;
   char StrTagTxt[6 + Cns_MAX_DIGITS_UINT + 1];
   char StrInteger[Cns_MAX_DIGITS_UINT + 1];
   char *Title;
   char *FuncOnChange;
   bool NewQuestion = (Question->QstCod > 0);
   HTM_Attributes_t Checked;
   HTM_Attributes_t RadioDisabled;
   HTM_Attributes_t CheckboxDisabled;
   HTM_Attributes_t ChoiceDisabled;

   /***** Begin box *****/
   if (NewQuestion)	// The question already has assigned a code
     {
      if (asprintf (&Title,Txt_Question_code_X,Question->QstCod) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxBegin (Title,Qst_PutIconToRemoveOneQst,&Question->QstCod,
                    Hlp_ASSESSMENT_Questions_writing_a_question,Box_NOT_CLOSABLE);
      free (Title);
     }
   else
      Box_BoxBegin (Txt_Question,NULL,NULL,
                    Hlp_ASSESSMENT_Questions_writing_a_question,Box_NOT_CLOSABLE);

   /***** Begin form *****/
   Frm_BeginForm (NewQuestion ? ActNewTstQst :
				ActChgTstQst);
      ParCod_PutPar (ParCod_Qst,Question->QstCod);

      /***** Begin table *****/
      HTM_TABLE_Begin ("TBL_SCROLL");	// Table for this question

	 /***** Help for text editor *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"2\"");
	       Lay_HelpPlainEditor ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Get tags already existing for questions in current course *****/
	 NumTags = Tag_DB_GetAllTagsFromCurrentCrs (&mysql_res);

	 /***** Write the tags *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","",Txt_Tags);

	    /* Tags */
	    HTM_TD_Begin ("class=\"LT\"");

	       HTM_TABLE_BeginWidePadding (2);	// Table for tags

		  for (IndTag = 0;
		       IndTag < Tag_MAX_TAGS_PER_QUESTION;
		       IndTag++)
		    {
		     HTM_TR_Begin (NULL);

			/***** Write the tags already existing in a selector *****/
			HTM_TD_Begin ("class=\"Qst_TAG_CELL LM\"");
			   if (asprintf (&FuncOnChange,"changeTxtTag('%u');",IndTag) < 0)
			      Err_NotEnoughMemoryExit ();
			   HTM_SELECT_Begin (HTM_NO_ATTR,FuncOnChange,
					     "id=\"SelTag%u\" name=\"SelTag%u\""
					     " class=\"Qst_TAG_SEL INPUT_%s\"",
					     IndTag,IndTag,The_GetSuffix ());
			   free (FuncOnChange);
			      HTM_OPTION (HTM_Type_STRING,"",
			                  HTM_NO_ATTR,
			                  "&nbsp;");
			      mysql_data_seek (mysql_res,0);
			      TagFound = false;
			      for (NumTag  = 1;
				   NumTag <= NumTags;
				   NumTag++)
				{
				 row = mysql_fetch_row (mysql_res);
				 /*
				 row[0] TagCod
				 row[1] TagTxt
				 row[2] TagHidden
				 */
				 IsThisTag = false;
				 if (!strcasecmp (Question->Tags.Txt[IndTag],row[1]))
				   {
				    IsThisTag = true;
				    TagFound = true;
				   }
				 HTM_OPTION (HTM_Type_STRING,row[1],
					     IsThisTag ? HTM_SELECTED :
							 HTM_NO_ATTR,
					     "%s",row[1]);
				}
			      /* If it's a new tag received from the form */
			      if (!TagFound && Question->Tags.Txt[IndTag][0])
				 HTM_OPTION (HTM_Type_STRING,Question->Tags.Txt[IndTag],
					     HTM_SELECTED,
					     "%s",Question->Tags.Txt[IndTag]);
			      HTM_OPTION (HTM_Type_STRING,"",
					  HTM_NO_ATTR,
					  "[%s]",Txt_new_tag);
			   HTM_SELECT_End ();
			HTM_TD_End ();

			/***** Input of a new tag *****/
			HTM_TD_Begin ("class=\"Qst_TAG_CELL RM\"");
			   snprintf (StrTagTxt,sizeof (StrTagTxt),"TagTxt%u",IndTag);
			   HTM_INPUT_TEXT (StrTagTxt,Tag_MAX_CHARS_TAG,
					   Question->Tags.Txt[IndTag],
					   HTM_NO_ATTR,
					   "id=\"%s\""
					   " class=\"Qst_TAG_TXT INPUT_%s\""
					   " onchange=\"changeSelTag('%u')\"",
					   StrTagTxt,The_GetSuffix (),IndTag);
			HTM_TD_End ();

		     HTM_TR_End ();
		    }

	       HTM_TABLE_End ();	// Table for tags
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);

	 /***** Stem and image *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Stem",Txt_Wording);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_TEXTAREA_Begin (HTM_REQUIRED,
				   "id=\"Stem\" name=\"Stem\" rows=\"5\""
			           " class=\"Qst_STEM_TXT INPUT_%s\"",
				   The_GetSuffix ());
		  HTM_Txt (Question->Stem);
	       HTM_TEXTAREA_End ();
	       HTM_BR ();
	       Qst_PutFormToEditQstMedia (&Question->Media,-1,HTM_NO_ATTR);

	       /***** Feedback *****/
	       HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
		  HTM_TxtF ("%s&nbsp;(%s):",Txt_Feedback,Txt_optional);
		  HTM_BR ();
		  HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				      "name=\"Feedback\" rows=\"2\""
			              " class=\"Qst_STEM_TXT INPUT_%s\"",
			              The_GetSuffix ());
		     if (Question->Feedback[0])
			HTM_Txt (Question->Feedback);
		  HTM_TEXTAREA_End ();
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Type of answer *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","",Txt_Type);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT FORM_IN_%s\"",The_GetSuffix ());
	       for (AnsType  = (Qst_AnswerType_t) 0;
		    AnsType <= (Qst_AnswerType_t) (Qst_NUM_ANS_TYPES - 1);
		    AnsType++)
		 {
		  HTM_LABEL_Begin (NULL);
		     HTM_INPUT_RADIO ("AnswerType",
				      (AnsType == Question->Answer.Type) ? HTM_CHECKED :
								           HTM_NO_ATTR,
				      "value=\"%u\""
				      " onclick=\"enableDisableAns(this.form);\"",
				      (unsigned) AnsType);
		     HTM_TxtF ("%s&nbsp;",Txt_TST_STR_ANSWER_TYPES[AnsType]);
		  HTM_LABEL_End ();
		  HTM_BR ();
		 }
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Answers *****/
	 /* Integer answer */
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","",Txt_Answers);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
		  HTM_TxtColonNBSP (Txt_Integer_number);
		  snprintf (StrInteger,sizeof (StrInteger),"%ld",Question->Answer.Integer);
		  HTM_INPUT_TEXT ("AnsInt",Cns_MAX_DIGITS_LONG,StrInteger,
				  (Question->Answer.Type == Qst_ANS_INT) ? HTM_NO_ATTR :
								           HTM_DISABLED | HTM_REQUIRED,
				  "size=\"11\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       HTM_LABEL_End ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /* Floating point answer */
	 HTM_TR_Begin (NULL);
	    HTM_TD_Empty (1);
	    HTM_TD_Begin ("class=\"LT\"");
	       Qst_PutFloatInputField (Txt_Real_number_between_A_and_B_1,"AnsFloatMin",
				       Question,0);
	       Qst_PutFloatInputField (Txt_Real_number_between_A_and_B_2,"AnsFloatMax",
				       Question,1);
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /* T/F answer */
	 HTM_TR_Begin (NULL);
	    HTM_TD_Empty (1);
	    HTM_TD_Begin ("class=\"LT\"");
	       Qst_PutTFInputField (Question,Txt_TF_QST[0],'T');
	       Qst_PutTFInputField (Question,Txt_TF_QST[1],'F');
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /* Questions can be shuffled? */
	 HTM_TR_Begin (NULL);
	    HTM_TD_Empty (1);
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
		  HTM_INPUT_CHECKBOX ("Shuffle",
				      (Question->Answer.Shuffle ? HTM_CHECKED :
	        				                  HTM_NO_ATTR) |
	        		      ((Question->Answer.Type != Qst_ANS_UNIQUE_CHOICE &&
			                Question->Answer.Type != Qst_ANS_MULTIPLE_CHOICE) ? HTM_DISABLED :
										            HTM_NO_ATTR),
				      "value=\"Y\"");
		  HTM_Txt (Txt_Shuffle);
	       HTM_LABEL_End ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /* Simple or multiple choice answers */
	 HTM_TR_Begin (NULL);
	    HTM_TD_Empty (1);
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_TABLE_BeginWidePadding (2);	// Table with choice answers

	       RadioDisabled = (Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE) ? HTM_NO_ATTR :
										  HTM_DISABLED;
	       CheckboxDisabled = (Question->Answer.Type == Qst_ANS_MULTIPLE_CHOICE) ? HTM_NO_ATTR :
									               HTM_DISABLED;
	       ChoiceDisabled = (Question->Answer.Type != Qst_ANS_UNIQUE_CHOICE &&
			         Question->Answer.Type != Qst_ANS_MULTIPLE_CHOICE &&
			         Question->Answer.Type != Qst_ANS_TEXT) ? HTM_DISABLED :
								          HTM_NO_ATTR;
	       for (NumOpt = 0, The_ResetRowColor ();
		    NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
		    NumOpt++, The_ChangeRowColor ())
		 {
		  AnswerHasContent = false;
		  if (Question->Answer.Options[NumOpt].Text)
		     if (Question->Answer.Options[NumOpt].Text[0] ||			// Text
			 Question->Answer.Options[NumOpt].Media.Type != Med_TYPE_NONE)	// or media
			AnswerHasContent = true;
		  DisplayRightColumn = NumOpt < 2 ||	// Display at least the two first options
				       AnswerHasContent;

		  HTM_TR_Begin (NULL);

		     /***** Left column: selectors *****/
		     HTM_TD_Begin ("class=\"Qst_ANS_LEFT_COL %s\"",
		                   The_GetColorRows ());
			Checked = Question->Answer.Options[NumOpt].Correct ? HTM_CHECKED :
									     HTM_NO_ATTR;

			/* Radio selector for unique choice answers */
			HTM_INPUT_RADIO ("AnsUni",
					 Checked | RadioDisabled |
				         ((NumOpt < 2) ? HTM_REQUIRED :  // First or second options required
						         HTM_NO_ATTR),
					 "value=\"%u\""
					 " onclick=\"enableDisableAns(this.form);\"",
					 NumOpt);

			/* Checkbox for multiple choice answers */
			HTM_INPUT_CHECKBOX ("AnsMulti",
					    Checked | CheckboxDisabled,
					    "value=\"%u\"",NumOpt);

		     HTM_TD_End ();

		     /***** Center column: letter of the answer and expand / contract icon *****/
		     HTM_TD_Begin ("class=\"FORM_IN_%s Qst_ANS_CENTER_COL %s\"",
				   The_GetSuffix (),The_GetColorRows ());
			HTM_TxtF ("%c)",'a' + (char) NumOpt);

			/* Icon to expand (show the answer) */
			HTM_A_Begin ("href=\"\" id=\"exp_%u\"%s"
				     " onclick=\"toggleAnswer('%u');return false;\"",
				     NumOpt,
				     DisplayRightColumn ? " style=\"display:none;\"" :	// Answer does have content ==> Hide icon
							  "",
				     NumOpt);
			   if (asprintf (&Title,"%s %c)",Txt_Expand,'a' + (char) NumOpt) < 0)
			      Err_NotEnoughMemoryExit ();
			   Ico_PutIcon ("caret-right.svg",Ico_BLACK,Title,"ICO16x16");
			   free (Title);
			HTM_A_End ();

			/* Icon to contract (hide the answer) */
			HTM_A_Begin ("href=\"\" id=\"con_%u\"%s"
				     " onclick=\"toggleAnswer(%u);return false;\"",
				     NumOpt,
				     DisplayRightColumn ? "" :
							  " style=\"display:none;\"",	// Answer does not have content ==> Hide icon
				     NumOpt);
			   if (asprintf (&Title,"%s %c)",Txt_Contract,'a' + (char) NumOpt) < 0)
			      Err_NotEnoughMemoryExit ();
			   Ico_PutIcon ("caret-down.svg",Ico_BLACK,Title,"ICO16x16");
			   free (Title);
			HTM_A_End ();

		     HTM_TD_End ();

		     /***** Right column: content of the answer *****/
		     HTM_TD_Begin ("class=\"Qst_ANS_RIGHT_COL %s\"",
		                   The_GetColorRows ());
			HTM_DIV_Begin ("id=\"ans_%u\"%s",
				       NumOpt,
				       DisplayRightColumn ? "" :
							    " style=\"display:none;\"");	// Answer does not have content ==> Hide column

			   /* Answer text */
			   HTM_TEXTAREA_Begin (ChoiceDisabled,
					       "name=\"AnsStr%u\" rows=\"5\""
				               " class=\"Qst_ANS_TXT INPUT_%s\"",
					       NumOpt,The_GetSuffix ());
			      if (AnswerHasContent)
				 HTM_Txt (Question->Answer.Options[NumOpt].Text);
			   HTM_TEXTAREA_End ();

			   /* Media */
			   Qst_PutFormToEditQstMedia (&Question->Answer.Options[NumOpt].Media,
						      (int) NumOpt,ChoiceDisabled);

			   /* Feedback */
			   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
			      HTM_TxtF ("%s&nbsp;(%s):",Txt_Feedback,Txt_optional);
			      HTM_BR ();
			      HTM_TEXTAREA_Begin (ChoiceDisabled,
						  "name=\"FbStr%u\" rows=\"2\""
				                  " class=\"Qst_ANS_TXT INPUT_%s\"",
						  NumOpt,The_GetSuffix ());
				 if (Question->Answer.Options[NumOpt].Feedback)
				    if (Question->Answer.Options[NumOpt].Feedback[0])
				       HTM_Txt (Question->Answer.Options[NumOpt].Feedback);
			      HTM_TEXTAREA_End ();
			   HTM_LABEL_End ();

			/* End of right column */
			HTM_DIV_End ();
		     HTM_TD_End ();

		  HTM_TR_End ();
		 }
	       HTM_TABLE_End ();	// Table with choice answers
	    HTM_TD_End ();
	 HTM_TR_End ();

      /***** End table *****/
      HTM_TABLE_End ();	// Table for this question

      /***** Send button *****/
      if (Question->QstCod > 0)	// The question already has assigned a code
	 Btn_PutConfirmButton (Txt_Save_changes);
      else
	 Btn_PutCreateButton (Txt_Create);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Put input field for floating answer *******************/
/*****************************************************************************/

void Qst_PutFloatInputField (const char *Label,const char *Field,
                             const struct Qst_Question *Question,
                             unsigned Index)
  {
   char StrDouble[32];

   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_TxtF ("%s&nbsp;",Label);
      snprintf (StrDouble,sizeof (StrDouble),"%.15lg",
		Question->Answer.FloatingPoint[Index]);
      HTM_INPUT_TEXT (Field,Qst_MAX_BYTES_FLOAT_ANSWER,StrDouble,
		      ((Question->Answer.Type == Qst_ANS_FLOAT) ? HTM_NO_ATTR :
								  HTM_DISABLED) | HTM_REQUIRED,
		      "size=\"11\" class=\"INPUT_%s\"",
		      The_GetSuffix ());
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/*********************** Put input field for T/F answer **********************/
/*****************************************************************************/

void Qst_PutTFInputField (const struct Qst_Question *Question,
                          const char *Label,char Value)
  {
   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_INPUT_RADIO ("AnsTF",
		       ((Question->Answer.TF == Value) ? HTM_CHECKED :
							 HTM_NO_ATTR) |
		       HTM_REQUIRED |
		       ((Question->Answer.Type == Qst_ANS_TRUE_FALSE) ? HTM_NO_ATTR :
								        HTM_DISABLED),
		       "value=\"%c\"",Value);
      HTM_Txt (Label);
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/********************* Initialize a new question to zero *********************/
/*****************************************************************************/

void Qst_QstConstructor (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Reset question tags *****/
   Tag_ResetTags (&Question->Tags);

   /***** Reset edition time *****/
   Question->EditTime = (time_t) 0;

   /***** Allocate memory for stem and feedback *****/
   if ((Question->Stem = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   Question->Stem[0] = '\0';

   if ((Question->Feedback = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   Question->Feedback[0] = '\0';

   /***** Initialize answers *****/
   Question->Answer.Type       = Qst_ANS_UNIQUE_CHOICE;
   Question->Answer.NumOptions = 0;
   Question->Answer.Shuffle    = false;
   Question->Answer.TF         = ' ';

   /* Initialize image attached to stem */
   Med_MediaConstructor (&Question->Media);

   /* Initialize options */
   for (NumOpt = 0;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
     {
      Question->Answer.Options[NumOpt].Correct  = false;
      Question->Answer.Options[NumOpt].Text     = NULL;
      Question->Answer.Options[NumOpt].Feedback = NULL;

      /* Initialize image attached to option */
      Med_MediaConstructor (&Question->Answer.Options[NumOpt].Media);
     }
   Question->Answer.Integer = 0;
   Question->Answer.FloatingPoint[0] =
   Question->Answer.FloatingPoint[1] = 0.0;

   /***** Initialize stats *****/
   Question->NumHits =
   Question->NumHitsNotBlank = 0;
   Question->Score = 0.0;

   /***** Mark question as valid *****/
   Question->Validity = Qst_VALID_QUESTION;
  }

/*****************************************************************************/
/***************** Free memory allocated for test question *******************/
/*****************************************************************************/

void Qst_QstDestructor (struct Qst_Question *Question)
  {
   Qst_FreeTextChoiceAnswers (Question);
   Qst_FreeMediaOfQuestion (Question);
   if (Question->Feedback)
     {
      free (Question->Feedback);
      Question->Feedback = NULL;
     }
   if (Question->Stem)
     {
      free (Question->Stem);
      Question->Stem = NULL;
     }
  }

/*****************************************************************************/
/******************* Allocate memory for a choice answer *********************/
/*****************************************************************************/
// Return false on error

bool Qst_AllocateTextChoiceAnswer (struct Qst_Question *Question,unsigned NumOpt)
  {
   if ((Question->Answer.Options[NumOpt].Text =
	malloc (Qst_MAX_BYTES_ANSWER_OR_FEEDBACK + 1)) == NULL)
     {
      Ale_CreateAlert (Ale_ERROR,NULL,
		       "Not enough memory to store answer.");
      return false;
     }
   if ((Question->Answer.Options[NumOpt].Feedback =
	malloc (Qst_MAX_BYTES_ANSWER_OR_FEEDBACK + 1)) == NULL)
     {
      Ale_CreateAlert (Ale_ERROR,NULL,
		       "Not enough memory to store feedback.");
      return false;
     }

   Question->Answer.Options[NumOpt].Text[0] =
   Question->Answer.Options[NumOpt].Feedback[0] = '\0';
   return true;
  }

/*****************************************************************************/
/******************** Free memory of all choice answers **********************/
/*****************************************************************************/

void Qst_FreeTextChoiceAnswers (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   for (NumOpt = 0;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Qst_FreeTextChoiceAnswer (Question,NumOpt);
  }

/*****************************************************************************/
/********************** Free memory of a choice answer ***********************/
/*****************************************************************************/

void Qst_FreeTextChoiceAnswer (struct Qst_Question *Question,unsigned NumOpt)
  {
   if (Question->Answer.Options[NumOpt].Text)
     {
      free (Question->Answer.Options[NumOpt].Text);
      Question->Answer.Options[NumOpt].Text = NULL;
     }
   if (Question->Answer.Options[NumOpt].Feedback)
     {
      free (Question->Answer.Options[NumOpt].Feedback);
      Question->Answer.Options[NumOpt].Feedback = NULL;
     }
  }

/*****************************************************************************/
/***************** Initialize images of a question to zero *******************/
/*****************************************************************************/

void Qst_ResetMediaOfQuestion (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Reset media for stem *****/
   Med_ResetMedia (&Question->Media);

   /***** Reset media for every answer option *****/
   for (NumOpt = 0;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Med_ResetMedia (&Question->Answer.Options[NumOpt].Media);
  }

/*****************************************************************************/
/*********************** Free images of a question ***************************/
/*****************************************************************************/

void Qst_FreeMediaOfQuestion (struct Qst_Question *Question)
  {
   unsigned NumOpt;

   Med_MediaDestructor (&Question->Media);
   for (NumOpt = 0;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Med_MediaDestructor (&Question->Answer.Options[NumOpt].Media);
  }

/*****************************************************************************/
/********************* Get question data using its code **********************/
/*****************************************************************************/

bool Qst_GetQstDataByCod (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool QuestionExists;
   unsigned NumTags;
   unsigned NumTag;
   unsigned NumOpt;

   /***** Get question data from database *****/
   if ((QuestionExists = (Qst_DB_GetQstDataByCod (&mysql_res,Question->QstCod) != 0)))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get edition time (row[0] holds the start UTC time) */
      Question->EditTime = Dat_GetUNIXTimeFromStr (row[0]);

      /* Get the type of answer (row[1]) */
      Question->Answer.Type = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

      /* Get shuffle (row[2]) */
      Question->Answer.Shuffle = (row[2][0] == 'Y');

      /* Get the stem (row[3]) and the feedback (row[4]) */
      Question->Stem    [0] = '\0';
      Question->Feedback[0] = '\0';
      if (row[3])
	 if (row[3][0])
	    Str_Copy (Question->Stem    ,row[3],Cns_MAX_BYTES_TEXT);
      if (row[4])
	 if (row[4][0])
	    Str_Copy (Question->Feedback,row[4],Cns_MAX_BYTES_TEXT);

      /* Get media (row[5]) */
      Question->Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
      Med_GetMediaDataByCod (&Question->Media);

      /* Get number of hits
	 (number of times that the question has been answered,
	 including blank answers) (row[6])
         and number of hits not blank
	 (number of times that the question has been answered
	 with a not blank answer) (row[7]) */
      if (sscanf (row[6],"%lu",&Question->NumHits        ) != 1)
	 Question->NumHits         = 0;
      if (sscanf (row[7],"%lu",&Question->NumHitsNotBlank) != 1)
	 Question->NumHitsNotBlank = 0;

      /* Get the acumulated score of the question (row[8]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[8],"%lf",&Question->Score) != 1)
	 Question->Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Get the tags from the database *****/
      NumTags = Tag_DB_GetTagsQst (&mysql_res,Question->QstCod);
      for (NumTag = 0;
	   NumTag < NumTags;
	   NumTag++)
	{
	 row = mysql_fetch_row (mysql_res);
	 Str_Copy (Question->Tags.Txt[NumTag],row[0],
	           sizeof (Question->Tags.Txt[NumTag]) - 1);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Get the answers from the database *****/
      Question->Answer.NumOptions = Qst_DB_GetAnswersData (&mysql_res,Question->QstCod,
			                                   false);	// Don't shuffle
      /*
      row[0] AnsInd
      row[1] Answer
      row[2] Feedback
      row[3] MedCod
      row[4] Correct
      */
      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 row = mysql_fetch_row (mysql_res);
	 switch (Question->Answer.Type)
	   {
	    case Qst_ANS_INT:
	       Qst_CheckIfNumberOfAnswersIsOne (Question);
	       Question->Answer.Integer = Qst_GetIntAnsFromStr (row[1]);
	       break;
	    case Qst_ANS_FLOAT:
	       if (Question->Answer.NumOptions != 2)
		  Err_WrongAnswerExit ();
	       Question->Answer.FloatingPoint[NumOpt] = Str_GetDoubleFromStr (row[1]);
	       break;
	    case Qst_ANS_TRUE_FALSE:
	       Qst_CheckIfNumberOfAnswersIsOne (Question);
	       Question->Answer.TF = row[1][0];
	       break;
	    case Qst_ANS_UNIQUE_CHOICE:
	    case Qst_ANS_MULTIPLE_CHOICE:
	    case Qst_ANS_TEXT:
	       /* Check number of options */
	       if (Question->Answer.NumOptions > Qst_MAX_OPTIONS_PER_QUESTION)
		  Err_WrongAnswerExit ();

	       /*  Allocate space for text and feedback */
	       if (!Qst_AllocateTextChoiceAnswer (Question,NumOpt))
		  /* Abort on error */
		  Ale_ShowAlertsAndExit ();

	       /* Get text (row[1]) and feedback (row[2])*/
	       Question->Answer.Options[NumOpt].Text   [0] = '\0';
	       if (row[1])
		  if (row[1][0])
		     Str_Copy (Question->Answer.Options[NumOpt].Text    ,row[1],
			       Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
	       Question->Answer.Options[NumOpt].Feedback[0] = '\0';
	       if (row[2])
		  if (row[2][0])
		     Str_Copy (Question->Answer.Options[NumOpt].Feedback,row[2],
			       Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);

	       /* Get media (row[3]) */
	       Question->Answer.Options[NumOpt].Media.MedCod = Str_ConvertStrCodToLongCod (row[3]);
	       Med_GetMediaDataByCod (&Question->Answer.Options[NumOpt].Media);

	       /* Get if this option is correct (row[4]) */
	       Question->Answer.Options[NumOpt].Correct = (row[4][0] == 'Y');
	       break;
	    default:
	       break;
	   }
	}
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return QuestionExists;
  }

/*****************************************************************************/
/******* Get media code associated with a test question from database ********/
/*****************************************************************************/
// NumOpt <  0 ==> media associated to stem
// NumOpt >= 0 ==> media associated to answer

long Qst_GetMedCodFromDB (long CrsCod,long QstCod,int NumOpt)
  {
   /***** Trivial check: question code should be > 0 *****/
   if (QstCod <= 0)
      return -1L;

   /***** Query depending on NumOpt *****/
   if (NumOpt < 0)
      // Get media associated to stem
      return Qst_DB_GetQstMedCod (CrsCod,QstCod);
   else
      // Get media associated to answer
      return Qst_DB_GetMedCodFromAnsOfQst (QstCod,(unsigned) NumOpt);
  }

/*****************************************************************************/
/***** Get possible media associated with a test question from database ******/
/*****************************************************************************/
// NumOpt <  0 ==> media associated to stem
// NumOpt >= 0 ==> media associated to an answer option

void Qst_GetMediaFromDB (long CrsCod,long QstCod,int NumOpt,
                         struct Med_Media *Media)
  {
   /***** Get media *****/
   Media->MedCod = Qst_GetMedCodFromDB (CrsCod,QstCod,NumOpt);
   Med_GetMediaDataByCod (Media);
  }

/*****************************************************************************/
/***************************** Receive a question ****************************/
/*****************************************************************************/

void Qst_ReceiveQst (void)
  {
   struct Qst_Questions Questions;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters of the question from form *****/
   Qst_GetQstFromForm (&Questions.Question);

   /***** Make sure that tags, text and answer are not empty *****/
   if (Qst_CheckIfQstFormatIsCorrectAndCountNumOptions (&Questions.Question))
     {
      /***** Move images to definitive directories *****/
      Qst_MoveMediaToDefinitiveDirectories (&Questions.Question);

      /***** Insert or update question, tags and answer in the database *****/
      Qst_InsertOrUpdateQstTagsAnsIntoDB (&Questions.Question);

      /***** Show the question just inserted in the database *****/
      snprintf (Questions.AnswerTypes.List,sizeof (Questions.AnswerTypes.List),"%u",
		(unsigned) Questions.Question.Answer.Type);
      Qst_ListOneQstToEdit (&Questions);
     }
   else	// Question is wrong
     {
      /***** Whether images has been received or not, reset images *****/
      Qst_ResetMediaOfQuestion (&Questions.Question);

      /***** Put form to edit question again *****/
      Qst_PutFormEditOneQst (&Questions.Question);
     }

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/**************** Get parameters of a test question from form ****************/
/*****************************************************************************/

void Qst_GetQstFromForm (struct Qst_Question *Question)
  {
   unsigned NumTag;
   unsigned NumTagRead;
   unsigned NumOpt;
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];
   char TagStr[6 + Cns_MAX_DIGITS_UINT + 1];
   char AnsStr[6 + Cns_MAX_DIGITS_UINT + 1];
   char FbStr[5 + Cns_MAX_DIGITS_UINT + 1];
   char StrMultiAns[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TF[1 + 1];	// (T)rue or (F)alse
   const char *Ptr;
   unsigned NumCorrectAns;

   /***** Get question code *****/
   Question->QstCod = ParCod_GetPar (ParCod_Qst);

   /***** Get answer type *****/
   Question->Answer.Type = (Qst_AnswerType_t)
			   Par_GetParUnsignedLong ("AnswerType",
						   0,
						   Qst_NUM_ANS_TYPES - 1,
						   (unsigned long) Qst_ANS_UNKNOWN);
   if (Question->Answer.Type == Qst_ANS_UNKNOWN)
      Err_WrongAnswerExit ();

   /***** Get question tags *****/
   for (NumTag = 0;
	NumTag < Tag_MAX_TAGS_PER_QUESTION;
	NumTag++)
     {
      snprintf (TagStr,sizeof (TagStr),"TagTxt%u",NumTag);
      Par_GetParText (TagStr,Question->Tags.Txt[NumTag],Tag_MAX_BYTES_TAG);

      if (Question->Tags.Txt[NumTag][0])
        {
         Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                           Question->Tags.Txt[NumTag],Tag_MAX_BYTES_TAG,
                           Str_REMOVE_SPACES);
         /* Check if not repeated */
         for (NumTagRead = 0;
              NumTagRead < NumTag;
              NumTagRead++)
            if (!strcmp (Question->Tags.Txt[NumTagRead],Question->Tags.Txt[NumTag]))
              {
               Question->Tags.Txt[NumTag][0] = '\0';
               break;
              }
        }
     }

   /***** Get question stem *****/
   Par_GetParHTML ("Stem",Question->Stem,Cns_MAX_BYTES_TEXT);

   /***** Get question feedback *****/
   Par_GetParHTML ("Feedback",Question->Feedback,Cns_MAX_BYTES_TEXT);

   /***** Get media associated to the stem (action, file and title) *****/
   Question->Media.Width   = Qst_IMAGE_SAVED_MAX_WIDTH;
   Question->Media.Height  = Qst_IMAGE_SAVED_MAX_HEIGHT;
   Question->Media.Quality = Qst_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Question->QstCod,
                         -1,	// < 0 ==> the image associated to the stem
                         &Question->Media,
                         Qst_GetMediaFromDB,
			 NULL);
   Ale_ShowAlerts (NULL);

   /***** Get answers *****/
   Question->Answer.Shuffle = false;
   switch (Question->Answer.Type)
     {
      case Qst_ANS_INT:
         if (!Qst_AllocateTextChoiceAnswer (Question,0))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

	 Par_GetParText ("AnsInt",Question->Answer.Options[0].Text,
			 Cns_MAX_DIGITS_LONG);
	 break;
      case Qst_ANS_FLOAT:
         if (!Qst_AllocateTextChoiceAnswer (Question,0))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

	 Par_GetParText ("AnsFloatMin",Question->Answer.Options[0].Text,
	                 Qst_MAX_BYTES_FLOAT_ANSWER);

         if (!Qst_AllocateTextChoiceAnswer (Question,1))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

	 Par_GetParText ("AnsFloatMax",Question->Answer.Options[1].Text,
	                 Qst_MAX_BYTES_FLOAT_ANSWER);
	 break;
      case Qst_ANS_TRUE_FALSE:
	 Par_GetParText ("AnsTF",TF,1);
	 Question->Answer.TF = TF[0];
	 break;
      case Qst_ANS_UNIQUE_CHOICE:
      case Qst_ANS_MULTIPLE_CHOICE:
         /* Get shuffle */
         Question->Answer.Shuffle = Par_GetParBool ("Shuffle");
	 /* falls through */
	 /* no break */
      case Qst_ANS_TEXT:
         /* Get the texts of the answers */
         for (NumOpt = 0;
              NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
              NumOpt++)
           {
            if (!Qst_AllocateTextChoiceAnswer (Question,NumOpt))
	       /* Abort on error */
	       Ale_ShowAlertsAndExit ();

            /* Get answer */
            snprintf (AnsStr,sizeof (AnsStr),"AnsStr%u",NumOpt);
	    Par_GetParHTML (AnsStr,Question->Answer.Options[NumOpt].Text,
	                      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
	    if (Question->Answer.Type == Qst_ANS_TEXT)
	       /* In order to compare student answer to stored answer,
	          the text answers are stored avoiding two or more consecurive spaces */
               Str_ReplaceSeveralSpacesForOne (Question->Answer.Options[NumOpt].Text);

            /* Get feedback */
            snprintf (FbStr,sizeof (FbStr),"FbStr%u",NumOpt);
	    Par_GetParHTML (FbStr,Question->Answer.Options[NumOpt].Feedback,
	                      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);

	    /* Get media associated to the answer (action, file and title) */
	    if (Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE ||
		Question->Answer.Type == Qst_ANS_MULTIPLE_CHOICE)
	      {
	       Question->Answer.Options[NumOpt].Media.Width   = Qst_IMAGE_SAVED_MAX_WIDTH;
	       Question->Answer.Options[NumOpt].Media.Height  = Qst_IMAGE_SAVED_MAX_HEIGHT;
	       Question->Answer.Options[NumOpt].Media.Quality = Qst_IMAGE_SAVED_QUALITY;
	       Med_GetMediaFromForm (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Question->QstCod,
	                             (int) NumOpt,	// >= 0 ==> the image associated to an answer
	                             &Question->Answer.Options[NumOpt].Media,
				     Qst_GetMediaFromDB,
				     NULL);
	       Ale_ShowAlerts (NULL);
	      }
           }

         /* Get the numbers of correct answers */
         if (Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE)
           {
	    NumCorrectAns = (unsigned) Par_GetParUnsignedLong ("AnsUni",
	                                                       0,
	                                                       Qst_MAX_OPTIONS_PER_QUESTION - 1,
	                                                       0);
            Question->Answer.Options[NumCorrectAns].Correct = true;
           }
      	 else if (Question->Answer.Type == Qst_ANS_MULTIPLE_CHOICE)
           {
	    Par_GetParMultiToText ("AnsMulti",StrMultiAns,Qst_MAX_BYTES_ANSWERS_ONE_QST);
 	    Ptr = StrMultiAns;
            while (*Ptr)
              {
  	       Par_GetNextStrUntilSeparParMult (&Ptr,UnsignedStr,
  						Cns_MAX_DIGITS_UINT);
	       if (sscanf (UnsignedStr,"%u",&NumCorrectAns) != 1)
	          Err_WrongAnswerExit ();
               if (NumCorrectAns >= Qst_MAX_OPTIONS_PER_QUESTION)
	          Err_WrongAnswerExit ();
               Question->Answer.Options[NumCorrectAns].Correct = true;
              }
           }
         else // Tst_ANS_TEXT
            for (NumOpt = 0;
        	 NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
        	 NumOpt++)
               if (Question->Answer.Options[NumOpt].Text[0])
                  Question->Answer.Options[NumOpt].Correct = true;	// All the answers are correct
	 break;
      default:
         break;
     }

   /***** Adjust variables related to this test question *****/
   for (NumTag = 0, Question->Tags.Num = 0;
        NumTag < Tag_MAX_TAGS_PER_QUESTION;
        NumTag++)
      if (Question->Tags.Txt[NumTag][0])
         Question->Tags.Num++;
  }

/*****************************************************************************/
/*********************** Check if a question is correct **********************/
/*****************************************************************************/
// Returns false if question format is wrong
// Counts Question->Answer.NumOptions
// Computes Question->Answer.Integer and Question->Answer.FloatingPoint[0..1]

bool Qst_CheckIfQstFormatIsCorrectAndCountNumOptions (struct Qst_Question *Question)
  {
   extern const char *Txt_You_must_type_at_least_one_tag_for_the_question;
   extern const char *Txt_You_must_type_the_question_stem;
   extern const char *Txt_You_must_select_a_T_F_answer;
   extern const char *Txt_You_can_not_leave_empty_intermediate_answers;
   extern const char *Txt_You_must_type_at_least_the_first_two_answers;
   extern const char *Txt_You_must_mark_an_answer_as_correct;
   extern const char *Txt_You_must_type_at_least_the_first_answer;
   extern const char *Txt_You_must_enter_an_integer_value_as_the_correct_answer;
   extern const char *Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer;
   extern const char *Txt_The_lower_limit_of_correct_answers_must_be_less_than_or_equal_to_the_upper_limit;
   unsigned NumOpt;
   unsigned NumLastOpt;
   bool ThereIsEndOfAnswers;
   unsigned i;

   /***** This function also counts the number of options. Initialize this number to 0. *****/
   Question->Answer.NumOptions = 0;

   /***** A question must have at least one tag *****/
   if (!Question->Tags.Num) // There are no tags with text
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_one_tag_for_the_question);
      return false;
     }

   /***** A question must have a stem *****/
   if (!Question->Stem[0])
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_the_question_stem);
      return false;
     }

   /***** Check answer *****/
   switch (Question->Answer.Type)
     {
      case Qst_ANS_INT:
	 /* First option should be filled */
         if (!Question->Answer.Options[0].Text)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_an_integer_value_as_the_correct_answer);
            return false;
           }
         if (!Question->Answer.Options[0].Text[0])
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_an_integer_value_as_the_correct_answer);
            return false;
           }

         Question->Answer.Integer = Qst_GetIntAnsFromStr (Question->Answer.Options[0].Text);
         Question->Answer.NumOptions = 1;
         break;
      case Qst_ANS_FLOAT:
	 /* First two options should be filled */
         if (!Question->Answer.Options[0].Text ||
             !Question->Answer.Options[1].Text)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer);
            return false;
           }
         if (!Question->Answer.Options[0].Text[0] ||
             !Question->Answer.Options[1].Text[0])
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer);
            return false;
           }

         /* Lower limit should be <= upper limit */
         for (i = 0;
              i < 2;
              i++)
            Question->Answer.FloatingPoint[i] = Str_GetDoubleFromStr (Question->Answer.Options[i].Text);
         if (Question->Answer.FloatingPoint[0] >
             Question->Answer.FloatingPoint[1])
           {
            Ale_ShowAlert (Ale_WARNING,Txt_The_lower_limit_of_correct_answers_must_be_less_than_or_equal_to_the_upper_limit);
            return false;
           }

         Question->Answer.NumOptions = 2;
         break;
      case Qst_ANS_TRUE_FALSE:
	 /* Answer should be 'T' or 'F' */
         if (Question->Answer.TF != 'T' &&
             Question->Answer.TF != 'F')
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_a_T_F_answer);
            return false;
           }

         Question->Answer.NumOptions = 1;
         break;
      case Qst_ANS_UNIQUE_CHOICE:
      case Qst_ANS_MULTIPLE_CHOICE:
	 /* No option should be empty before a non-empty option */
         for (NumOpt = 0, NumLastOpt = 0, ThereIsEndOfAnswers = false;
              NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
              NumOpt++)
            if (Question->Answer.Options[NumOpt].Text)
              {
               if (Question->Answer.Options[NumOpt].Text[0] ||				// Text
        	   Question->Answer.Options[NumOpt].Media.Type != Med_TYPE_NONE)	// or media
                 {
                  if (ThereIsEndOfAnswers)
                    {
                     Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
                     return false;
                    }
                  NumLastOpt = NumOpt;
                  Question->Answer.NumOptions++;
                 }
               else
                  ThereIsEndOfAnswers = true;
              }
            else
               ThereIsEndOfAnswers = true;

         /* The two first options must be filled */
         if (NumLastOpt < 1)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_two_answers);
            return false;
           }

         /* Its mandatory to mark at least one option as correct */
         for (NumOpt  = 0;
              NumOpt <= NumLastOpt;
              NumOpt++)
            if (Question->Answer.Options[NumOpt].Correct)
               break;
         if (NumOpt > NumLastOpt)
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_mark_an_answer_as_correct);
            return false;
           }
         break;
      case Qst_ANS_TEXT:
	 /* First option should be filled */
         if (!Question->Answer.Options[0].Text)		// If the first answer is empty
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_answer);
            return false;
           }
         if (!Question->Answer.Options[0].Text[0])	// If the first answer is empty
           {
            Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_answer);
            return false;
           }

	 /* No option should be empty before a non-empty option */
         for (NumOpt = 0, ThereIsEndOfAnswers=false;
              NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
              NumOpt++)
            if (Question->Answer.Options[NumOpt].Text)
              {
               if (Question->Answer.Options[NumOpt].Text[0])
                 {
                  if (ThereIsEndOfAnswers)
                    {
                     Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
                     return false;
                    }
                  Question->Answer.NumOptions++;
                 }
               else
                  ThereIsEndOfAnswers = true;
              }
            else
               ThereIsEndOfAnswers = true;
         break;
      default:
         break;
     }

    return true;	// Question format without errors
   }

/*****************************************************************************/
/*********** Check if a test question already exists in database *************/
/*****************************************************************************/

bool Qst_CheckIfQuestionExistsInDB (struct Qst_Question *Question)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
   MYSQL_RES *mysql_res_qst;
   MYSQL_RES *mysql_res_ans;
   MYSQL_ROW row;
   bool IdenticalQuestionFound = false;
   bool IdenticalAnswers;
   unsigned NumQst;
   unsigned NumQstsWithThisStem;
   unsigned NumOpt;
   unsigned NumOptsExistingQstInDB;
   unsigned i;

   /***** Check if there are existing questions in database
          with the same stem that the one of this question *****/
   if ((NumQstsWithThisStem = Qst_DB_GetQstCodFromTypeAnsStem (&mysql_res_qst,Question)))
     {
      /***** Check if the answer exists in any of the questions with the same stem *****/
      /* For each question with the same stem */
      for (NumQst = 0;
           !IdenticalQuestionFound && NumQst < NumQstsWithThisStem;
           NumQst++)
        {
	 /* Get question code */
         if ((Question->QstCod = DB_GetNextCode (mysql_res_qst)) < 0)
            Err_WrongQuestionExit ();

         /* Get answers from this question */
         NumOptsExistingQstInDB = Qst_DB_GetTextOfAnswers (&mysql_res_ans,Question->QstCod);

         switch (Question->Answer.Type)
           {
            case Qst_ANS_INT:
               row = mysql_fetch_row (mysql_res_ans);
               IdenticalQuestionFound = (Qst_GetIntAnsFromStr (row[0]) == Question->Answer.Integer);
               break;
            case Qst_ANS_FLOAT:
               for (IdenticalAnswers = true, i = 0;
                    IdenticalAnswers && i < 2;
                    i++)
                 {
                  row = mysql_fetch_row (mysql_res_ans);
                  IdenticalAnswers = (Str_GetDoubleFromStr (row[0]) == Question->Answer.FloatingPoint[i]);
                 }
               IdenticalQuestionFound = IdenticalAnswers;
               break;
            case Qst_ANS_TRUE_FALSE:
               row = mysql_fetch_row (mysql_res_ans);
               IdenticalQuestionFound = (Str_ConvertToUpperLetter (row[0][0]) == Question->Answer.TF);
               break;
            case Qst_ANS_UNIQUE_CHOICE:
            case Qst_ANS_MULTIPLE_CHOICE:
            case Qst_ANS_TEXT:
               if (NumOptsExistingQstInDB == Question->Answer.NumOptions)
                 {
                  for (IdenticalAnswers = true, NumOpt = 0;
                       IdenticalAnswers && NumOpt < NumOptsExistingQstInDB;
                       NumOpt++)
                    {
                     row = mysql_fetch_row (mysql_res_ans);

                     if (strcasecmp (row[0],Question->Answer.Options[NumOpt].Text))
                        IdenticalAnswers = false;
                    }
                 }
               else	// Different number of answers (options)
                  IdenticalAnswers = false;
               IdenticalQuestionFound = IdenticalAnswers;
               break;
            default:
               break;
           }

         /* Free structure that stores the query result for answers */
         DB_FreeMySQLResult (&mysql_res_ans);
        }
     }
   else	// Stem does not exist
      IdenticalQuestionFound = false;

   /* Free structure that stores the query result for questions */
   DB_FreeMySQLResult (&mysql_res_qst);

   return IdenticalQuestionFound;
  }

/*****************************************************************************/
/* Move images associates to a test question to their definitive directories */
/*****************************************************************************/

void Qst_MoveMediaToDefinitiveDirectories (struct Qst_Question *Question)
  {
   unsigned NumOpt;
   long CurrentMedCodInDB;

   /***** Media associated to question stem *****/
   CurrentMedCodInDB = Qst_GetMedCodFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Question->QstCod,
                                            -1);	// Get current media code associated to stem
   Med_RemoveKeepOrStoreMedia (CurrentMedCodInDB,&Question->Media);

   /****** Move media associated to answers *****/
   switch (Question->Answer.Type)
     {
      case Qst_ANS_UNIQUE_CHOICE:
      case Qst_ANS_MULTIPLE_CHOICE:
	 for (NumOpt = 0;
	      NumOpt < Question->Answer.NumOptions;
	      NumOpt++)
	   {
	    CurrentMedCodInDB = Qst_GetMedCodFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,Question->QstCod,
						     (int) NumOpt);	// Get current media code associated to this option
	    Med_RemoveKeepOrStoreMedia (CurrentMedCodInDB,&Question->Answer.Options[NumOpt].Media);
	   }
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/******************** Get a integer number from a string *********************/
/*****************************************************************************/

long Qst_GetIntAnsFromStr (char *Str)
  {
   long LongNum;

   if (Str == NULL)
      return 0.0;

   /***** The string is "scanned" as long *****/
   if (sscanf (Str,"%ld",&LongNum) != 1)	// If the string does not hold a valid integer number...
     {
      LongNum = 0L;	// ...the number is reset to 0
      Str[0] = '\0';	// ...and the string is reset to ""
     }

   return LongNum;
  }

/*****************************************************************************/
/***************** Request the removal of selected questions *****************/
/*****************************************************************************/

void Qst_ReqRemSelectedQsts (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_selected_questions;
   struct Qst_Questions Questions;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters *****/
   if (Tst_GetParsTst (&Questions,Tst_EDIT_QUESTIONS))	// Get parameters from the form
     {
      /***** Show question and button to remove question *****/
      Ale_ShowAlertRemove (ActRemSevTstQst,NULL,
			   Qst_PutParsEditQst,&Questions,
			   Txt_Do_you_really_want_to_remove_the_selected_questions,
			   NULL);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"Wrong parameters.");

   /***** Continue editing questions *****/
   Qst_ListQuestionsToEdit ();

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/************************** Remove several questions *************************/
/*****************************************************************************/

void Qst_RemoveSelectedQsts (void)
  {
   extern const char *Txt_Questions_removed_X;
   struct Qst_Questions Questions;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   long QstCod;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get parameters *****/
   if (Tst_GetParsTst (&Questions,Tst_EDIT_QUESTIONS))	// Get parameters
     {
      /***** Get question codes *****/
      Questions.NumQsts = Qst_DB_GetQsts (&mysql_res,&Questions);

      /***** Remove questions one by one *****/
      for (NumQst = 0;
	   NumQst < Questions.NumQsts;
	   NumQst++)
	{
	 /* Get question code (row[0]) */
	 row = mysql_fetch_row (mysql_res);
	 if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Err_WrongQuestionExit ();

	 /* Remove test question from database */
	 Qst_RemoveOneQstFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,QstCod);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Write message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Questions_removed_X,Questions.NumQsts);
     }

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/********************* Put icon to remove one question ***********************/
/*****************************************************************************/

void Qst_PutIconToRemoveOneQst (void *QstCod)
  {
   Ico_PutContextualIconToRemove (ActReqRemOneTstQst,NULL,
                                  Qst_PutParsRemoveOnlyThisQst,QstCod);
  }

/*****************************************************************************/
/******************** Request the removal of a question **********************/
/*****************************************************************************/

void Qst_ReqRemOneQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   bool EditingOnlyThisQst;
   struct Qst_Questions Questions;
   char StrQstCod[Cns_MAX_DIGITS_LONG + 1];

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get main parameters from form *****/
   /* Get the question code */
   Questions.Question.QstCod = ParCod_GetAndCheckPar (ParCod_Qst);

   /* Get a parameter that indicates whether it's necessary
      to continue listing the rest of questions */
   EditingOnlyThisQst = Par_GetParBool ("OnlyThisQst");

   /* Get other parameters */
   if (!EditingOnlyThisQst)
      if (!Tst_GetParsTst (&Questions,Tst_EDIT_QUESTIONS))
	 Err_ShowErrorAndExit ("Wrong test parameters.");

   /***** Show question and button to remove question *****/
   sprintf (StrQstCod,"%ld",Questions.Question.QstCod);
   Ale_ShowAlertRemove (ActRemOneTstQst,NULL,
			EditingOnlyThisQst ? Qst_PutParsRemoveOnlyThisQst :
					     Qst_PutParsEditQst,
			EditingOnlyThisQst ? (void *) &Questions.Question.QstCod :
					     (void *) &Questions,
			Txt_Do_you_really_want_to_remove_the_question_X,
			StrQstCod);

   /***** Continue editing questions *****/
   if (EditingOnlyThisQst)
      Qst_ListOneQstToEdit (&Questions);
   else
      Qst_ListQuestionsToEdit ();

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/***** Put parameters to remove question when editing only one question ******/
/*****************************************************************************/

void Qst_PutParsRemoveOnlyThisQst (void *QstCod)
  {
   if (QstCod)
     {
      ParCod_PutPar (ParCod_Qst,*(long *) QstCod);
      Par_PutParChar ("OnlyThisQst",'Y');
     }
  }

/*****************************************************************************/
/***************************** Remove a question *****************************/
/*****************************************************************************/

void Qst_RemoveOneQst (void)
  {
   extern const char *Txt_Question_removed;
   long QstCod;
   bool EditingOnlyThisQst;

   /***** Get the question code *****/
   QstCod = ParCod_GetAndCheckPar (ParCod_Qst);

   /***** Get a parameter that indicates whether it's necessary
          to continue listing the rest of questions ******/
   EditingOnlyThisQst = Par_GetParBool ("OnlyThisQst");

   /***** Remove test question from database *****/
   Qst_RemoveOneQstFromDB (Gbl.Hierarchy.Node[Hie_CRS].HieCod,QstCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

   /***** Continue editing questions *****/
   if (!EditingOnlyThisQst)
      Qst_ListQuestionsToEdit ();
  }

/*****************************************************************************/
/********************** Remove a question from database **********************/
/*****************************************************************************/

void Qst_RemoveOneQstFromDB (long CrsCod,long QstCod)
  {
   /***** Remove media associated to question *****/
   Qst_RemoveMediaFromStemOfQst (CrsCod,QstCod);
   Qst_RemoveMediaFromAllAnsOfQst (CrsCod,QstCod);

   /***** Remove the question from all tables *****/
   /* Remove answers and tags from this test question */
   Qst_DB_RemAnsFromQst (QstCod);
   Tag_DB_RemTagsFromQst (QstCod);
   Tag_DB_RemoveUnusedTagsFromCrs (CrsCod);

   /* Remove the question itself */
   Qst_DB_RemoveQst (CrsCod,QstCod);
  }

/*****************************************************************************/
/*********************** Change the shuffle of a question ********************/
/*****************************************************************************/

void Qst_ChangeShuffleQst (void)
  {
   extern const char *Txt_The_answers_of_the_question_with_code_X_will_appear_shuffled;
   extern const char *Txt_The_answers_of_the_question_with_code_X_will_appear_without_shuffling;
   struct Qst_Questions Questions;
   bool EditingOnlyThisQst;
   bool Shuffle;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Get the question code *****/
   Questions.Question.QstCod = ParCod_GetAndCheckPar (ParCod_Qst);

   /***** Get a parameter that indicates whether it's necessary to continue listing the rest of questions ******/
   EditingOnlyThisQst = Par_GetParBool ("OnlyThisQst");

   /***** Get a parameter that indicates whether it's possible to shuffle the answers of this question ******/
   Shuffle = Par_GetParBool ("Shuffle");

   /***** Update the question changing the current shuffle *****/
   Qst_DB_UpdateQstShuffle (Questions.Question.QstCod,Shuffle);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Shuffle ? Txt_The_answers_of_the_question_with_code_X_will_appear_shuffled :
                                        Txt_The_answers_of_the_question_with_code_X_will_appear_without_shuffling,
                  Questions.Question.QstCod);

   /***** Continue editing questions *****/
   if (EditingOnlyThisQst)
      Qst_ListOneQstToEdit (&Questions);
   else
      Qst_ListQuestionsToEdit ();

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/************ Put parameter with question code to edit, remove... ************/
/*****************************************************************************/

void Qst_PutParQstCod (void *QstCod)	// Should be a pointer to long
  {
   if (QstCod)
      ParCod_PutPar (ParCod_Qst,*((long *) QstCod));
  }

/*****************************************************************************/
/******** Insert or update question, tags and answer in the database *********/
/*****************************************************************************/

void Qst_InsertOrUpdateQstTagsAnsIntoDB (struct Qst_Question *Question)
  {
   /***** Insert or update question in the table of questions *****/
   Qst_InsertOrUpdateQstIntoDB (Question);

   if (Question->QstCod > 0)
     {
      /***** Insert tags in the tags table *****/
      Tag_InsertTagsIntoDB (Question->QstCod,&Question->Tags);

      /***** Remove unused tags in current course *****/
      Tag_DB_RemoveUnusedTagsFromCrs (Gbl.Hierarchy.Node[Hie_CRS].HieCod);

      /***** Insert answers in the answers table *****/
      Qst_InsertAnswersIntoDB (Question);
     }
  }

/*****************************************************************************/
/*********** Insert or update question in the table of questions *************/
/*****************************************************************************/

void Qst_InsertOrUpdateQstIntoDB (struct Qst_Question *Question)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];

   if (Question->QstCod < 0)	// It's a new question
      /***** Insert question in the table of questions *****/
      Question->QstCod = Qst_DB_CreateQst (Question);
   else			// It's an existing question
     {
      /***** Update existing question *****/
      Qst_DB_UpdateQst (Question);

      /***** Remove answers and tags from this test question *****/
      Qst_DB_RemAnsFromQst (Question->QstCod);
      Tag_DB_RemTagsFromQst (Question->QstCod);
     }
  }

/*****************************************************************************/
/******************* Insert answers in the answers table *********************/
/*****************************************************************************/

void Qst_InsertAnswersIntoDB (struct Qst_Question *Question)
  {
   void (*Qst_DB_CreateAnswer[Qst_NUM_ANS_TYPES]) (struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = Qst_DB_CreateIntAnswer,
     [Qst_ANS_FLOAT          ] = Qst_DB_CreateFltAnswer,
     [Qst_ANS_TRUE_FALSE     ] = Qst_DB_CreateTF_Answer,
     [Qst_ANS_UNIQUE_CHOICE  ] = Qst_DB_CreateChoAnswer,
     [Qst_ANS_MULTIPLE_CHOICE] = Qst_DB_CreateChoAnswer,
     [Qst_ANS_TEXT           ] = Qst_DB_CreateChoAnswer,
    };

   /***** Create answer *****/
   Qst_DB_CreateAnswer[Question->Answer.Type] (Question);
  }

/*****************************************************************************/
/**** Count the number of types of answers in the list of types of answers ***/
/*****************************************************************************/

unsigned Qst_CountNumAnswerTypesInList (const struct Qst_AnswerTypes *AnswerTypes)
  {
   const char *Ptr;
   unsigned NumAnsTypes = 0;
   char UnsignedStr[Cns_MAX_DIGITS_UINT + 1];

   /***** Go over the list of answer types counting the number of types of answer *****/
   Ptr = AnswerTypes->List;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,UnsignedStr,Cns_MAX_DIGITS_UINT);
      Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
      NumAnsTypes++;
     }
   return NumAnsTypes;
  }

/*****************************************************************************/
/**** Count the number of questions in the list of selected question codes ***/
/*****************************************************************************/

unsigned Qst_CountNumQuestionsInList (const char *ListQuestions)
  {
   const char *Ptr;
   unsigned NumQuestions = 0;
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   long QstCod;

   /***** Go over list of questions counting the number of questions *****/
   Ptr = ListQuestions;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DIGITS_LONG);
      if (sscanf (LongStr,"%ld",&QstCod) != 1)
         Err_WrongQuestionExit ();
      NumQuestions++;
     }
   return NumQuestions;
  }

/*****************************************************************************/
/********************* Remove all questions in a course **********************/
/*****************************************************************************/

void Qst_RemoveCrsQsts (long CrsCod)
  {
   /***** Remove associations between questions and tags in the course *****/
   Tag_DB_RemTagsInQstsInCrs (CrsCod);

   /***** Remove test tags in the course *****/
   Tag_DB_RemTagsInCrs (CrsCod);

   /***** Remove media associated to test questions in the course *****/
   Qst_RemoveAllMedFilesFromStemOfAllQstsInCrs (CrsCod);
   Qst_RemoveAllMedFilesFromAnsOfAllQstsInCrs (CrsCod);

   /***** Remove test answers in the course *****/
   Qst_DB_RemAnssFromQstsInCrs (CrsCod);

   /***** Remove test questions in the course *****/
   Qst_DB_RemoveQstsInCrs (CrsCod);
  }

/*****************************************************************************/
/************ Remove media associated to stem of a test question *************/
/*****************************************************************************/

void Qst_RemoveMediaFromStemOfQst (long CrsCod,long QstCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media code associated to stem of test question from database *****/
   NumMedia = Qst_DB_GetMedCodFromStemOfQst (&mysql_res,CrsCod,QstCod);

   /***** Go over result removing media *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******* Remove all media associated to all answers of a test question *******/
/*****************************************************************************/

void Qst_RemoveMediaFromAllAnsOfQst (long CrsCod,long QstCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media codes associated to answers of test questions from database *****/
   NumMedia = Qst_DB_GetMedCodsFromAnssOfQst (&mysql_res,CrsCod,QstCod);

   /***** Go over result removing media *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/** Remove all media associated to stems of all test questions in a course ***/
/*****************************************************************************/

void Qst_RemoveAllMedFilesFromStemOfAllQstsInCrs (long CrsCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media codes associated to stems of test questions from database *****/
   NumMedia = Qst_DB_GetMedCodsFromStemsOfQstsInCrs (&mysql_res,CrsCod);

   /***** Go over result removing media files *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/* Remove media associated to all answers of all test questions in a course **/
/*****************************************************************************/

void Qst_RemoveAllMedFilesFromAnsOfAllQstsInCrs (long CrsCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get names of media files associated to answers of test questions from database *****/
   NumMedia = Qst_DB_GetMedCodsFromAnssOfQstsInCrs (&mysql_res,CrsCod);

   /***** Go over result removing media files *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get number of test questions ************************/
/*****************************************************************************/
// Returns the number of test questions
// in this location (all the platform, current degree or current course)

unsigned Qst_GetNumQuestions (Hie_Level_t Level,Qst_AnswerType_t AnsType,
                              struct Qst_Stats *Stats)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Reset default stats *****/
   Stats->NumQsts = 0;
   Stats->NumHits = 0L;
   Stats->TotalScore = 0.0;

   /***** Get number of questions from database *****/
   if (Qst_DB_GetNumQsts (&mysql_res,Level,AnsType))
     {
      /***** Get number of questions *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&(Stats->NumQsts)) != 1)
	 Err_ShowErrorAndExit ("Error when getting number of test questions.");

      if (Stats->NumQsts)
	{
	 if (sscanf (row[1],"%lu",&(Stats->NumHits)) != 1)
	    Err_ShowErrorAndExit ("Error when getting total number of hits in test questions.");

	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
	 if (sscanf (row[2],"%lf",&(Stats->TotalScore)) != 1)
	    Err_ShowErrorAndExit ("Error when getting total score in test questions.");
	 Str_SetDecimalPointToLocal ();	// Return to local system
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Stats->NumQsts;
  }

/*****************************************************************************/
/*********************** Get stats about test questions **********************/
/*****************************************************************************/

void Qst_GetTestStats (Qst_AnswerType_t AnsType,struct Qst_Stats *Stats)
  {
   Stats->NumQsts = 0;
   Stats->NumCoursesWithQuestions = Stats->NumCoursesWithPluggableQuestions = 0;
   Stats->AvgQstsPerCourse = 0.0;
   Stats->NumHits = 0L;
   Stats->AvgHitsPerCourse = 0.0;
   Stats->AvgHitsPerQuestion = 0.0;
   Stats->TotalScore = 0.0;
   Stats->AvgScorePerQuestion = 0.0;

   if (Qst_GetNumQuestions (Gbl.Scope.Current,AnsType,Stats))
     {
      if ((Stats->NumCoursesWithQuestions = Qst_DB_GetNumCrssWithQsts (Gbl.Scope.Current,AnsType)))
        {
         Stats->NumCoursesWithPluggableQuestions = Qst_DB_GetNumCrssWithPluggableQsts (Gbl.Scope.Current,AnsType);
         Stats->AvgQstsPerCourse = (double) Stats->NumQsts / (double) Stats->NumCoursesWithQuestions;
         Stats->AvgHitsPerCourse = (double) Stats->NumHits / (double) Stats->NumCoursesWithQuestions;
        }
      Stats->AvgHitsPerQuestion = (double) Stats->NumHits / (double) Stats->NumQsts;
      if (Stats->NumHits)
         Stats->AvgScorePerQuestion = Stats->TotalScore / (double) Stats->NumHits;
     }
  }
