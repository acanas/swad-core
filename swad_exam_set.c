// swad_exam_set.c: set of questions in exams

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <float.h>		// For DBL_MAX
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam.h"
#include "swad_exam_database.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_match.h"
#include "swad_match_result.h"
#include "swad_pagination.h"
#include "swad_question_database.h"
#include "swad_role.h"
#include "swad_test.h"
#include "swad_test_visibility.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define ExaSet_MAX_SELECTED_QUESTIONS			10000
#define ExaSet_MAX_BYTES_LIST_SELECTED_QUESTIONS	(ExaSet_MAX_SELECTED_QUESTIONS * (Cns_MAX_DECIMAL_DIGITS_LONG + 1))

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaSet_PutParamsOneQst (void *Exams);

static void ExaSet_PutFormNewSet (struct Exa_Exams *Exams,
				  struct Exa_Exam *Exam,
				  struct ExaSet_Set *Set,
				  unsigned MaxSetInd);
static void ExaSet_ReceiveSetFieldsFromForm (struct ExaSet_Set *Set);
static bool ExaSet_CheckSetTitleReceivedFromForm (const struct ExaSet_Set *Set,
                                                  const char NewTitle[ExaSet_MAX_BYTES_TITLE + 1]);

static void ExaSet_CreateSet (struct ExaSet_Set *Set);
static void ExaSet_UpdateSet (const struct ExaSet_Set *Set);

static void ExaSet_PutParamSetCod (long SetCod);

static void ExaSet_ListSetQuestions (struct Exa_Exams *Exams,
                                     const struct Exa_Exam *Exam,
                                     const struct ExaSet_Set *Set);
static void ExaSet_ListOneOrMoreSetsForEdition (struct Exa_Exams *Exams,
					        const struct Exa_Exam *Exam,
					        unsigned MaxSetInd,
					        unsigned NumSets,
                                                MYSQL_RES *mysql_res,
                                                bool ICanEditSets);
static void ExaSet_PutTableHeadingForSets (void);

static void ExaSet_ListOneOrMoreQuestionsForEdition (struct Exa_Exams *Exams,
						     unsigned NumQsts,
                                                     MYSQL_RES *mysql_res,
						     bool ICanEditQuestions);
static void ExaSet_ListQuestionForEdition (struct Qst_Question *Question,
                                           unsigned QstInd,const char *Anchor);

static void ExaSet_AllocateListSelectedQuestions (struct Exa_Exams *Exams);
static void ExaSet_FreeListsSelectedQuestions (struct Exa_Exams *Exams);

static void ExaSet_CopyQstFromBankToExamSet (const struct ExaSet_Set *Set,long QstCod);

static void ExaSet_RemoveMediaFromStemOfQst (long QstCod,long SetCod);
static void ExaSet_RemoveMediaFromAllAnsOfQst (long QstCod,long SetCod);

static void ExaSet_ChangeValidityQst (Qst_Validity_t Valid);

static void ExaSet_GetAndCheckParameters (struct Exa_Exams *Exams,
                                          struct Exa_Exam *Exam,
                                          struct ExaSet_Set *Set);

static long ExaSet_GetParamQstCod (void);
static void ExaSet_PutParamQstCod (void *QstCod);	// Should be a pointer to long

static void ExaSet_ExchangeSets (long ExaCod,
                                 unsigned SetIndTop,unsigned SetIndBottom);

static void ExaSet_PutIconToAddNewQuestions (void *Exams);
static void ExaSet_PutButtonToAddNewQuestions (struct Exa_Exams *Exams);

/*****************************************************************************/
/************ Put parameter to move/remove one set of questions **************/
/*****************************************************************************/

void ExaSet_PutParamsOneSet (void *Exams)
  {
   if (Exams)
     {
      Exa_PutParams (Exams);
      ExaSet_PutParamSetCod (((struct Exa_Exams *) Exams)->SetCod);
     }
  }

/*****************************************************************************/
/**************** Put parameter to move/remove one question ******************/
/*****************************************************************************/

static void ExaSet_PutParamsOneQst (void *Exams)
  {
   ExaSet_PutParamsOneSet (Exams);
   ExaSet_PutParamQstCod (&(((struct Exa_Exams *) Exams)->QstCod));
  }

/*****************************************************************************/
/********************** Get parameter with code of set ***********************/
/*****************************************************************************/

long ExaSet_GetParamSetCod (void)
  {
   /***** Get code of set *****/
   return Par_GetParToLong ("SetCod");
  }

/*****************************************************************************/
/*********************** Get set data using its code *************************/
/*****************************************************************************/

void ExaSet_GetDataOfSetByCod (struct ExaSet_Set *Set)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrSetInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Trivial check *****/
   if (Set->SetCod <= 0)
     {
      /* Initialize to empty set */
      ExaSet_ResetSet (Set);
      return;
     }

   /***** Get data of set of questions from database *****/
   if (Exa_DB_GetDataOfSetByCod (&mysql_res,Set->SetCod)) // Set found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] SetCod
      row[1] ExaCod
      row[2] SetInd
      row[3] NumQstsToPrint
      row[4] Title
      */
      /* Get set code (row[0]) */
      Set->SetCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get exam code (row[0]) */
      Set->ExaCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get set index (row[2]) */
      Set->SetInd = Str_ConvertStrToUnsigned (row[2]);
      snprintf (StrSetInd,sizeof (Set->SetInd),"%u",Set->SetInd);

      /* Get set index (row[3]) */
      Set->NumQstsToPrint = Str_ConvertStrToUnsigned (row[3]);

      /* Get the title of the set (row[4]) */
      Str_Copy (Set->Title,row[4],sizeof (Set->Title) - 1);
     }
   else
      /* Initialize to empty set */
      ExaSet_ResetSet (Set);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Put a form to create/edit an exam **********************/
/*****************************************************************************/

static void ExaSet_PutFormNewSet (struct Exa_Exams *Exams,
				  struct Exa_Exam *Exam,
				  struct ExaSet_Set *Set,
				  unsigned MaxSetInd)
  {
   extern const char *Txt_New_set_of_questions;
   extern const char *Txt_Create_set_of_questions;

   /***** Begin form *****/
   Exams->ExaCod = Exam->ExaCod;
   Frm_BeginForm (ActNewExaSet);
      Exa_PutParams (Exams);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_New_set_of_questions,
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);

	 /***** Table heading *****/
	 ExaSet_PutTableHeadingForSets ();

	 /***** Begin row *****/
	 HTM_TR_Begin (NULL);

	    /***** Empty column for buttons *****/
	    HTM_TD_Begin ("class=\"BM\"");
	    HTM_TD_End ();

	    /***** Index *****/
	    HTM_TD_Begin ("class=\"RM\"");
	       Qst_WriteNumQst (MaxSetInd + 1,"BIG_INDEX");
	    HTM_TD_End ();

	    /***** Title *****/
	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_INPUT_TEXT ("Title",ExaSet_MAX_CHARS_TITLE,Set->Title,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Title\" required=\"required\""
			       " class=\"TITLE_DESCRIPTION_WIDTH\"");
	    HTM_TD_End ();

	    /***** Current number of questions in set *****/
	    HTM_TD_Begin ("class=\"RM\"");
	       HTM_Unsigned (0);	// New set ==> no questions yet
	    HTM_TD_End ();

	    /***** Number of questions to appear in the exam *****/
	    HTM_TD_Begin ("class=\"RM\"");
	       HTM_INPUT_LONG ("NumQstsToPrint",0,UINT_MAX,(long) Set->NumQstsToPrint,
			       HTM_DONT_SUBMIT_ON_CHANGE,false,
				"class=\"INPUT_LONG\" required=\"required\"");
	    HTM_TD_End ();

	 /***** End row *****/
	 HTM_TR_End ();

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_set_of_questions);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Receive form to create a new set of questions **************/
/*****************************************************************************/

void ExaSet_ReceiveFormSet (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   bool ItsANewSet;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Err_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   ItsANewSet = (Set.SetCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Err_NoPermissionExit ();

   /***** If I can edit exams ==> receive set from form *****/
   ExaSet_ReceiveSetFieldsFromForm (&Set);
   if (ExaSet_CheckSetTitleReceivedFromForm (&Set,Set.Title))
     {
      /***** Create a new exam or update an existing one *****/
      if (ItsANewSet)
	 ExaSet_CreateSet (&Set);	// Add new set to database
      else
	 ExaSet_UpdateSet (&Set);	// Update set data in database
     }

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

static void ExaSet_ReceiveSetFieldsFromForm (struct ExaSet_Set *Set)
  {
   /***** Get set title *****/
   Par_GetParToText ("Title",Set->Title,ExaSet_MAX_BYTES_TITLE);

   /***** Get number of questions in set to appear in exam print *****/
   Set->NumQstsToPrint = (unsigned) Par_GetParToUnsignedLong ("NumQstsToPrint",
                                                              0,
                                                              UINT_MAX,
                                                              0);
  }

static bool ExaSet_CheckSetTitleReceivedFromForm (const struct ExaSet_Set *Set,
                                                  const char NewTitle[ExaSet_MAX_BYTES_TITLE + 1])
  {
   extern const char *Txt_Already_existed_a_set_of_questions_in_this_exam_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_set_of_questions;
   bool NewTitleIsCorrect;

   /***** Check if title is correct *****/
   NewTitleIsCorrect = true;
   if (NewTitle[0])	// If there's an set title
     {
      /***** Check if old and new titles are the same
	     (this happens when return is pressed without changes) *****/
      if (strcmp (Set->Title,NewTitle))	// Different titles
	{
	 /* If title of set was in database... */
	 if (Exa_DB_CheckIfSimilarSetExists (Set,NewTitle))
	   {
	    NewTitleIsCorrect = false;
	    Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_set_of_questions_in_this_exam_with_the_title_X,
			   Set->Title);
	   }
	}
     }
   else	// If there is not a set title
     {
      NewTitleIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_set_of_questions);
     }

   return NewTitleIsCorrect;
  }

/*****************************************************************************/
/************* Receive form to change title of set of questions **************/
/*****************************************************************************/

void ExaSet_ChangeSetTitle (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   char NewTitle[ExaSet_MAX_BYTES_TITLE + 1];

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Err_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Err_NoPermissionExit ();

   /***** Receive new title from form *****/
   Par_GetParToText ("Title",NewTitle,ExaSet_MAX_BYTES_TITLE);

   /***** Check if title should be changed *****/
   if (ExaSet_CheckSetTitleReceivedFromForm (&Set,NewTitle))
     {
      /* Update the table changing old title by new title */
      Exa_DB_UpdateSetTitle (Set.SetCod,Set.ExaCod,NewTitle);

      /* Update title */
      Str_Copy (Set.Title,NewTitle,sizeof (Set.Title) - 1);
     }

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }
/*****************************************************************************/
/***** Receive form to change number of questions to appear in the exam ******/
/*****************************************************************************/

void ExaSet_ChangeNumQstsToExam (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   unsigned NumQstsToPrint;

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Err_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Err_NoPermissionExit ();

   /***** Get number of questions in set to appear in exam print *****/
   NumQstsToPrint = (unsigned) Par_GetParToUnsignedLong ("NumQstsToPrint",
                                                         0,
                                                         UINT_MAX,
                                                         0);

   /***** Check if title should be changed *****/
   if (NumQstsToPrint != Set.NumQstsToPrint)
     {
      /* Update the table changing old number by new number */
      Exa_DB_UpdateNumQstsToExam (Set.SetCod,Set.ExaCod,NumQstsToPrint);

      /* Update title */
      Set.NumQstsToPrint = NumQstsToPrint;
     }

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/********************** Create a new set of questions ************************/
/*****************************************************************************/

static void ExaSet_CreateSet (struct ExaSet_Set *Set)
  {
   extern const char *Txt_Created_new_set_of_questions_X;
   unsigned MaxSetInd;

   /***** Get maximum set index *****/
   MaxSetInd = Exa_DB_GetMaxSetIndexInExam (Set->ExaCod);

   /***** Create a new exam *****/
   Set->SetCod = Exa_DB_CreateSet (Set,MaxSetInd + 1);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_set_of_questions_X,
                  Set->Title);
  }

/*****************************************************************************/
/******************** Update an existing set of questions ********************/
/*****************************************************************************/

static void ExaSet_UpdateSet (const struct ExaSet_Set *Set)
  {
   extern const char *Txt_The_set_of_questions_has_been_modified;

   /***** Update the data of the set of questions *****/
   Exa_DB_UpdateSet (Set);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_set_of_questions_has_been_modified);
  }

/*****************************************************************************/
/********** Request the creation or edition of an set of questions ***********/
/*****************************************************************************/

void ExaSet_RequestCreatOrEditSet (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   bool ItsANewSet;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Err_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Err_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   ItsANewSet = (Set.SetCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   Exa_DB_GetExamTxt (Exam.ExaCod,Txt);

   /***** Get set data *****/
   if (ItsANewSet)
      /* Initialize to empty set */
      ExaSet_ResetSet (&Set);
   else
     {
      /* Get set data from database */
      ExaSet_GetDataOfSetByCod (&Set);
      Exams.SetCod = Set.SetCod;
     }

   /***** Put form to edit the exam created or updated *****/
   Exa_PutFormEditionExam (&Exams,&Exam,Txt,
			   false);	// No new exam
  }

/*****************************************************************************/
/*** Request the selection of questions to be added to a set of questions ****/
/*****************************************************************************/

void ExaSet_ReqSelectQstsToAddToSet (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Show form to select questions for set *****/
   Qst_RequestSelectQstsForExamSet (&Exams);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/**************** List several test questions for selection ******************/
/*****************************************************************************/

void ExaSet_ListQstsToAddToSet (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

   /***** List several test questions for selection *****/
   Qst_ListQuestionsToSelectForExamSet (&Exams);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/**************** Write parameter with index of set of questions *****************/
/*****************************************************************************/

static void ExaSet_PutParamSetCod (long SetCod)
  {
   Par_PutHiddenParamUnsigned (NULL,"SetCod",SetCod);
  }

/*****************************************************************************/
/************************* List the sets of an exam **************************/
/*****************************************************************************/

void ExaSet_ListExamSets (struct Exa_Exams *Exams,
                          struct Exa_Exam *Exam,
			  struct ExaSet_Set *Set)
  {
   extern const char *Hlp_ASSESSMENT_Exams_question_sets;
   extern const char *Txt_Sets_of_questions;
   MYSQL_RES *mysql_res;
   unsigned MaxSetInd;
   unsigned NumSets;
   bool ICanEditSets = Exa_CheckIfEditable (Exam);

   /***** Get maximum set index *****/
   MaxSetInd = Exa_DB_GetMaxSetIndexInExam (Exam->ExaCod);

   /***** Get data of set of questions from database *****/
   NumSets = Exa_DB_GetExamSets (&mysql_res,Exam->ExaCod);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Sets_of_questions,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams_question_sets,Box_NOT_CLOSABLE);

   /***** Show table with sets *****/
   Exams->ExaCod = Exam->ExaCod;
   if (NumSets)
      ExaSet_ListOneOrMoreSetsForEdition (Exams,Exam,
                                          MaxSetInd,
                                          NumSets,mysql_res,
				          ICanEditSets);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Put forms to create/edit a set *****/
   if (ICanEditSets)
      ExaSet_PutFormNewSet (Exams,Exam,Set,MaxSetInd);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************ List the questions of an exam ***********************/
/*****************************************************************************/

static void ExaSet_ListSetQuestions (struct Exa_Exams *Exams,
                                     const struct Exa_Exam *Exam,
                                     const struct ExaSet_Set *Set)
  {
   extern const char *Hlp_ASSESSMENT_Exams_questions;
   extern const char *Txt_Questions;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   bool ICanEditQuestions = Exa_CheckIfEditable (Exam);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Questions,
		 ICanEditQuestions ? ExaSet_PutIconToAddNewQuestions :
				     NULL,
		 ICanEditQuestions ? Exams :
				     NULL,
		 Hlp_ASSESSMENT_Exams_questions,Box_NOT_CLOSABLE);

      /***** Show table with questions *****/
      if ((NumQsts = Exa_DB_GetQstsFromSet (&mysql_res,Set->SetCod)))
	 ExaSet_ListOneOrMoreQuestionsForEdition (Exams,NumQsts,mysql_res,
						  ICanEditQuestions);

      /***** Put button to add a new question in this set *****/
      if (ICanEditQuestions)		// I can edit questions
	 ExaSet_PutButtonToAddNewQuestions (Exams);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************* List exam sets for edition ************************/
/*****************************************************************************/

static void ExaSet_ListOneOrMoreSetsForEdition (struct Exa_Exams *Exams,
					        const struct Exa_Exam *Exam,
					        unsigned MaxSetInd,
					        unsigned NumSets,
                                                MYSQL_RES *mysql_res,
                                                bool ICanEditSets)
  {
   extern const char *Txt_Sets_of_questions;
   extern const char *Txt_Move_up_X;
   extern const char *Txt_Move_down_X;
   extern const char *Txt_Movement_not_allowed;
   unsigned NumSet;
   struct ExaSet_Set Set;
   MYSQL_ROW row;
   char *Anchor;
   char StrSetInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Trivial check *****/
   if (!NumSets)
      return;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (5);

      /***** Write the heading *****/
      ExaSet_PutTableHeadingForSets ();

      /***** Write rows *****/
      for (NumSet = 0;
	   NumSet < NumSets;
	   NumSet++)
	{
	 Gbl.RowEvenOdd = NumSet % 2;

	 /***** Create set of questions *****/
	 ExaSet_ResetSet (&Set);

	 /***** Get set data *****/
	 row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] SetCod
	 row[1] SetInd
	 row[2] NumQstsToPrint
	 row[3] Title
	 */
	 /* Get set code (row[0]) */
	 Set.SetCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get set index (row[1]) */
	 Set.SetInd = Str_ConvertStrToUnsigned (row[1]);
	 snprintf (StrSetInd,sizeof (Set.SetInd),"%u",Set.SetInd);

	 /* Get number of questions to exam (row[2]) */
	 Set.NumQstsToPrint = Str_ConvertStrToUnsigned (row[2]);

	 /* Get the title of the set (row[3]) */
	 Str_Copy (Set.Title,row[3],sizeof (Set.Title) - 1);

	 /* Initialize context */
	 Exams->SetCod = Set.SetCod;
	 Exams->SetInd = Set.SetInd;

	 /***** Build anchor string *****/
	 Frm_SetAnchorStr (Set.SetCod,&Anchor);

	 /***** Begin first row *****/
	 HTM_TR_Begin (NULL);

	    /***** Icons *****/
	    HTM_TD_Begin ("rowspan=\"2\" class=\"BT%u\"",Gbl.RowEvenOdd);

	       /* Put icon to remove the set */
	       if (ICanEditSets)
		  Ico_PutContextualIconToRemove (ActReqRemExaSet,NULL,
						 ExaSet_PutParamsOneSet,Exams);
	       else
		  Ico_PutIconRemovalNotAllowed ();

	       /* Put icon to move up the question */
	       if (ICanEditSets && Set.SetInd > 1)
		 {
		  Lay_PutContextualLinkOnlyIcon (ActUp_ExaSet,Anchor,
						 ExaSet_PutParamsOneSet,Exams,
						 "arrow-up.svg",
						 Str_BuildStringStr (Txt_Move_up_X,
								     StrSetInd));
		  Str_FreeString ();
		 }
	       else
		  Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);

	       /* Put icon to move down the set */
	       if (ICanEditSets && Set.SetInd < MaxSetInd)
		 {
		  Lay_PutContextualLinkOnlyIcon (ActDwnExaSet,Anchor,
						 ExaSet_PutParamsOneSet,Exams,
						 "arrow-down.svg",
						 Str_BuildStringStr (Txt_Move_down_X,
								     StrSetInd));
		  Str_FreeString ();
		 }
	       else
		  Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);

	    HTM_TD_End ();

	    /***** Index *****/
	    HTM_TD_Begin ("rowspan=\"2\" class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	       Qst_WriteNumQst (Set.SetInd,"BIG_INDEX");
	    HTM_TD_End ();

	    /***** Title *****/
	    HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
	       HTM_ARTICLE_Begin (Anchor);
		  if (ICanEditSets)
		    {
		     Frm_BeginFormAnchor (ActChgTitExaSet,Anchor);
			ExaSet_PutParamsOneSet (Exams);
			HTM_INPUT_TEXT ("Title",ExaSet_MAX_CHARS_TITLE,Set.Title,
					HTM_SUBMIT_ON_CHANGE,
					"id=\"Title\" required=\"required\""
					" class=\"TITLE_DESCRIPTION_WIDTH\"");
		     Frm_EndForm ();
		    }
		  else
		    {
		     HTM_SPAN_Begin ("class=\"EXA_SET_TITLE\"");
			HTM_Txt (Set.Title);
		     HTM_SPAN_End ();
		    }
	       HTM_ARTICLE_End ();
	    HTM_TD_End ();

	    /***** Current number of questions in set *****/
	    HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	       HTM_SPAN_Begin ("class=\"EXA_SET_NUM_QSTS\"");
		  HTM_Unsigned (Exa_DB_GetNumQstsInSet (Set.SetCod));
	       HTM_SPAN_End ();
	    HTM_TD_End ();

	    /***** Number of questions to appear in exam print *****/
	    HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	       if (ICanEditSets)
		 {
		  Frm_BeginFormAnchor (ActChgNumQstExaSet,Anchor);
		     ExaSet_PutParamsOneSet (Exams);
		     HTM_INPUT_LONG ("NumQstsToPrint",0,UINT_MAX,(long) Set.NumQstsToPrint,
				     HTM_SUBMIT_ON_CHANGE,false,
				      "class=\"INPUT_LONG\" required=\"required\"");
		  Frm_EndForm ();
		 }
	       else
		 {
		  HTM_SPAN_Begin ("class=\"EXA_SET_NUM_QSTS\"");
		     HTM_Unsigned (Set.NumQstsToPrint);
		  HTM_SPAN_End ();
		 }
	    HTM_TD_End ();

	 /***** End first row *****/
	 HTM_TR_End ();

	 /***** Begin second row *****/
	 HTM_TR_Begin (NULL);

	    /***** Questions *****/
	    HTM_TD_Begin ("colspan=\"3\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

	       /* List questions */
	       ExaSet_ListSetQuestions (Exams,Exam,&Set);

	    HTM_TD_End ();

	 /***** End second row *****/
	 HTM_TR_End ();

	 /***** Free anchor string *****/
	 Frm_FreeAnchorStr (Anchor);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Put table heading for sets of questions *******************/
/*****************************************************************************/

static void ExaSet_PutTableHeadingForSets (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Set_of_questions;
   extern const char *Txt_Number_of_questions;
   extern const char *Txt_Number_of_questions_to_show;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Header cells *****/
      HTM_TH_Empty (1);
      HTM_TH (1,1,"RB",Txt_No_INDEX);
      HTM_TH (1,1,"LB",Txt_Set_of_questions);
      HTM_TH (1,1,"RB",Txt_Number_of_questions);
      HTM_TH (1,1,"RB",Txt_Number_of_questions_to_show);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Reset set of questions **************************/
/*****************************************************************************/

void ExaSet_ResetSet (struct ExaSet_Set *Set)
  {
   Set->ExaCod        = -1L;
   Set->SetCod        = -1L;
   Set->SetInd        = 0;
   Set->Title[0]      = '\0';
   Set->NumQstsToPrint = 0;
  }

/*****************************************************************************/
/********************* List exam questions for edition ***********************/
/*****************************************************************************/

static void ExaSet_ListOneOrMoreQuestionsForEdition (struct Exa_Exams *Exams,
						     unsigned NumQsts,
                                                     MYSQL_RES *mysql_res,
						     bool ICanEditQuestions)
  {
   extern const char *Txt_Invalid_question;
   extern const char *Txt_Valid_question;
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Question;
   unsigned QstInd;
   struct Qst_Question Question;
   char *Anchor;
   static Act_Action_t NextAction[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = ActValSetQst,	// Validate question (set it as valid question)
      [Qst_VALID_QUESTION  ] = ActInvSetQst,	// Invalidated question (set it as canceled question)
     };
   static const char *Icon[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "times-red.svg",
      [Qst_VALID_QUESTION  ] = "check-green.svg",
     };
   static const char **Title[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = &Txt_Invalid_question,
      [Qst_VALID_QUESTION  ] = &Txt_Valid_question,
     };

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (5);

      /***** Write the heading *****/
      HTM_TR_Begin (NULL);

	 HTM_TH_Empty (1);

	 HTM_TH (1,1,"CT",Txt_No_INDEX);
	 HTM_TH (1,1,"CT",Txt_Question);

      HTM_TR_End ();

      /***** Write rows *****/
      for (QstInd = 0;
	   QstInd < NumQsts;
	   QstInd++)
	{
	 Gbl.RowEvenOdd = QstInd % 2;

	 /***** Create test question *****/
	 Qst_QstConstructor (&Question);

	 /***** Get question data *****/
	 /* Get question code */
	 Exams->QstCod = Question.QstCod = DB_GetNextCode (mysql_res);
	 ExaSet_GetQstDataFromDB (&Question);

	 /***** Build anchor string *****/
	 Frm_SetAnchorStr (Exams->QstCod,&Anchor);

	 /***** Begin row *****/
	 HTM_TR_Begin (NULL);

	    /***** Icons *****/
	    HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

	       /* Put icon to remove the question */
	       if (ICanEditQuestions)
		  Ico_PutContextualIconToRemove (ActReqRemSetQst,NULL,
						 ExaSet_PutParamsOneQst,Exams);
	       else
		  Ico_PutIconRemovalNotAllowed ();

	       /* Put icon to cancel the question */
	       Lay_PutContextualLinkOnlyIcon (NextAction[Question.Validity],Anchor,
					      ExaSet_PutParamsOneQst,Exams,
					      Icon[Question.Validity],
					      *Title[Question.Validity]);

	    HTM_TD_End ();

	    /***** List question *****/
	    ExaSet_ListQuestionForEdition (&Question,QstInd + 1,Anchor);

	 /***** End row *****/
	 HTM_TR_End ();

	 /***** Free anchor string *****/
	 Frm_FreeAnchorStr (Anchor);

	 /***** Destroy test question *****/
	 Qst_QstDestructor (&Question);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/*************** Get answer type of a question from database *****************/
/*****************************************************************************/

Qst_AnswerType_t ExaSet_GetAnswerType (long QstCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Qst_AnswerType_t AnswerType;

   /***** Get type of answer from database *****/
   if (!Exa_DB_GetAnswerType (&mysql_res,QstCod))
      Err_WrongQuestionExit ();

   /* Get type of answer */
   row = mysql_fetch_row (mysql_res);
   AnswerType = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[0]);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return AnswerType;
  }

/*****************************************************************************/
/*************** Get data of a question in a set from database ***************/
/*****************************************************************************/

void ExaSet_GetQstDataFromDB (struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool QuestionExists;
   unsigned NumOpt;

   /***** Get question data from database *****/
   if ((QuestionExists = (Exa_DB_GetQstDataByCod (&mysql_res,Question->QstCod) != 0)))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get whether the question is invalid (row[0]) */
      Question->Validity = (row[0][0] == 'Y') ? Qst_INVALID_QUESTION :
	                                        Qst_VALID_QUESTION;

      /* Get the type of answer (row[1]) */
      Question->Answer.Type = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

      /* Get shuffle (row[2]) */
      Question->Answer.Shuffle = (row[2][0] == 'Y');

      /* Get the stem (row[3]) */
      Question->Stem[0] = '\0';
      if (row[3])
	 if (row[3][0])
	    Str_Copy (Question->Stem    ,row[3],Cns_MAX_BYTES_TEXT);

      /* Get the feedback (row[4]) */
      Question->Feedback[0] = '\0';
      if (row[4])
	 if (row[4][0])
	    Str_Copy (Question->Feedback,row[4],Cns_MAX_BYTES_TEXT);

      /* Get media (row[5]) */
      Question->Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
      Med_GetMediaDataByCod (&Question->Media);

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Get the answers from the database *****/
      Question->Answer.NumOptions = Exa_DB_GetQstAnswersFromSet (&mysql_res,
			                                         Question->QstCod,
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

	       /* Get text (row[1]) */
	       Question->Answer.Options[NumOpt].Text[0] = '\0';
	       if (row[1])
		  if (row[1][0])
		     Str_Copy (Question->Answer.Options[NumOpt].Text    ,row[1],
			       Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);

	       /* Get feedback (row[2]) */
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

   if (!QuestionExists)
      Err_WrongQuestionExit ();
  }

/*****************************************************************************/
/********************* List question in set for edition **********************/
/*****************************************************************************/

static void ExaSet_ListQuestionForEdition (struct Qst_Question *Question,
                                           unsigned QstInd,const char *Anchor)
  {
   static char *ClassNumQst[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "BIG_INDEX_RED",
      [Qst_VALID_QUESTION  ] = "BIG_INDEX",
     };
   static char *ClassAnswerType[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "DAT_SMALL_RED",
      [Qst_VALID_QUESTION  ] = "DAT_SMALL",
     };
   static char *ClassTxt[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "TEST_TXT_RED",
      [Qst_VALID_QUESTION  ] = "TEST_TXT",
     };
   static char *ClassFeedback[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "TEST_TXT_LIGHT_RED",
      [Qst_VALID_QUESTION  ] = "TEST_TXT_LIGHT",
     };

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      Qst_WriteNumQst (QstInd,ClassNumQst[Question->Validity]);
      Qst_WriteAnswerType (Question->Answer.Type,ClassAnswerType[Question->Validity]);
   HTM_TD_End ();

   /***** Write stem (row[3]) and media *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_ARTICLE_Begin (Anchor);

	 /* Write stem */
	 Qst_WriteQstStem (Question->Stem,ClassTxt[Question->Validity],
			   true);	// Visible

	 /* Show media */
	 Med_ShowMedia (&Question->Media,
			"TEST_MED_EDIT_LIST_CONT",
			"TEST_MED_EDIT_LIST");

	 /* Show feedback */
	 Qst_WriteQstFeedback (Question->Feedback,ClassFeedback[Question->Validity]);

	 /* Show answers */
	 Qst_WriteAnswersBank (Question,
			       ClassTxt[Question->Validity],
			       ClassFeedback[Question->Validity]);

      HTM_ARTICLE_End ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Add selected test questions to set of questions ***************/
/*****************************************************************************/

void ExaSet_AddQstsToSet (void)
  {
   extern const char *Txt_No_questions_have_been_added;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

   /***** Get selected questions *****/
   /* Allocate space for selected question codes */
   ExaSet_AllocateListSelectedQuestions (&Exams);

   /* Get question codes */
   Par_GetParMultiToText ("QstCods",Exams.ListQuestions,
			  ExaSet_MAX_BYTES_LIST_SELECTED_QUESTIONS);

   /* Check number of questions */
   if (Qst_CountNumQuestionsInList (Exams.ListQuestions))	// If questions selected...
     {
      /***** Insert questions in database *****/
      Ptr = Exams.ListQuestions;
      while (*Ptr)
	{
	 /* Get next code */
	 Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 if (sscanf (LongStr,"%ld",&QstCod) != 1)
	    Err_WrongQuestionExit ();

	 ExaSet_CopyQstFromBankToExamSet (&Set,QstCod);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_No_questions_have_been_added);

   /***** Free space for selected question codes *****/
   ExaSet_FreeListsSelectedQuestions (&Exams);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/****************** Allocate memory for list of questions ********************/
/*****************************************************************************/

static void ExaSet_AllocateListSelectedQuestions (struct Exa_Exams *Exams)
  {
   if (!Exams->ListQuestions)
     {
      if ((Exams->ListQuestions = malloc (ExaSet_MAX_BYTES_LIST_SELECTED_QUESTIONS + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
      Exams->ListQuestions[0] = '\0';
     }
  }

/*****************************************************************************/
/*********** Free memory used by list of selected question codes *************/
/*****************************************************************************/

static void ExaSet_FreeListsSelectedQuestions (struct Exa_Exams *Exams)
  {
   if (Exams->ListQuestions)
     {
      free (Exams->ListQuestions);
      Exams->ListQuestions = NULL;
     }
  }

/*****************************************************************************/
/******* Copy question and answers from back of questions to exam set ********/
/*****************************************************************************/

static void ExaSet_CopyQstFromBankToExamSet (const struct ExaSet_Set *Set,long QstCod)
  {
   extern const char *Txt_Question_removed;
   struct Qst_Question Question;
   long CloneMedCod;
   long QstCodInSet;
   unsigned NumOpt;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Create test question *****/
   Qst_QstConstructor (&Question);
   Question.QstCod = QstCod;

   /***** Get data of question from database *****/
   if (Qst_GetQstDataFromDB (&Question))
     {
      /***** Clone media *****/
      CloneMedCod = Med_CloneMedia (&Question.Media);

      /***** Add question to set *****/
      QstCodInSet = Exa_DB_AddQuestionToSet (Set->SetCod,&Question,CloneMedCod);

      /***** Get the answers from the database *****/
      Question.Answer.NumOptions = Qst_DB_GetDataOfAnswers (&mysql_res,Question.QstCod,
			                                    false);	// Don't shuffle
      /*
      row[0] AnsInd
      row[1] Answer
      row[2] Feedback
      row[3] MedCod
      row[4] Correct
      */
      for (NumOpt = 0;
	   NumOpt < Question.Answer.NumOptions;
	   NumOpt++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get media (row[3]) */
	 Question.Answer.Options[NumOpt].Media.MedCod = Str_ConvertStrCodToLongCod (row[3]);
	 Med_GetMediaDataByCod (&Question.Answer.Options[NumOpt].Media);

	 /* Clone media */
	 CloneMedCod = Med_CloneMedia (&Question.Answer.Options[NumOpt].Media);

	 /* Copy answer option to exam set */
	 Exa_DB_AddAnsToQstInSet (QstCodInSet,		// Question code in set
	                          NumOpt,		// Answer index (number of option)
				  row[1],		// Copy of text
				  row[2],		// Copy of feedback
				  CloneMedCod,		// Media code of the new cloned media
				  row[4][0] == 'Y');	// Copy of correct
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);

   /***** Destroy test question *****/
   Qst_QstDestructor (&Question);
  }

/*****************************************************************************/
/***************** Request the removal of a set of questions *****************/
/*****************************************************************************/

void ExaSet_RequestRemoveSet (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_set_of_questions_X;
   extern const char *Txt_Remove_set_of_questions;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Err_NoPermissionExit ();

   /***** Show question and button to remove question *****/
   Ale_ShowAlertAndButton (ActRemExaSet,NULL,NULL,
			   ExaSet_PutParamsOneSet,&Exams,
			   Btn_REMOVE_BUTTON,Txt_Remove_set_of_questions,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_set_of_questions_X,
			   Set.Title);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/************************* Remove a set of questions *************************/
/*****************************************************************************/

void ExaSet_RemoveSet (void)
  {
   extern const char *Txt_Set_of_questions_removed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Err_NoPermissionExit ();

   /***** Remove the set from all tables *****/
   /* Remove questions associated to set */
   Exa_DB_RemoveAllSetQuestionsFromSet (Set.SetCod,Set.ExaCod);

   /* Remove the set itself */
   Exa_DB_RemoveSetFromExam (Set.SetCod,Set.ExaCod);

   /* Change indexes of sets greater than this */
   Exa_DB_UpdateSetIndexesInExamGreaterThan (Set.ExaCod,Set.SetInd);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Set_of_questions_removed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/************ Move up position of a set of questions in an exam **************/
/*****************************************************************************/

void ExaSet_MoveUpSet (void)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   unsigned SetIndTop;
   unsigned SetIndBottom;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Err_NoPermissionExit ();

   /***** Get set index *****/
   SetIndBottom = Exa_DB_GetSetIndFromSetCod (Exam.ExaCod,Set.SetCod);

   /***** Move up set *****/
   if (SetIndBottom > 1)
     {
      /* Indexes of sets to be exchanged */
      SetIndTop = Exa_DB_GetPrevSetIndexInExam (Exam.ExaCod,SetIndBottom);
      if (SetIndTop == 0)
	 Err_ShowErrorAndExit ("Wrong set index.");

      /* Exchange sets */
      ExaSet_ExchangeSets (Exam.ExaCod,SetIndTop,SetIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/*********** Move down position of a set of questions in an exam *************/
/*****************************************************************************/

void ExaSet_MoveDownSet (void)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   unsigned SetIndTop;
   unsigned SetIndBottom;
   unsigned MaxSetInd;	// 0 if no sets

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Err_NoPermissionExit ();

   /***** Get set index *****/
   SetIndTop = Exa_DB_GetSetIndFromSetCod (Exam.ExaCod,Set.SetCod);

   /***** Get maximum set index *****/
   MaxSetInd = Exa_DB_GetMaxSetIndexInExam (Exam.ExaCod);

   /***** Move down set *****/
   if (SetIndTop < MaxSetInd)
     {
      /* Indexes of sets to be exchanged */
      SetIndBottom = Exa_DB_GetNextSetIndexInExam (Exam.ExaCod,SetIndTop);
      if (SetIndBottom == 0)
	 Err_ShowErrorAndExit ("Wrong set index.");

      /* Exchange sets */
      ExaSet_ExchangeSets (Exam.ExaCod,SetIndTop,SetIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void ExaSet_RequestRemoveQstFromSet (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   char *Anchor;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Get question index *****/
   Exams.QstCod = ExaSet_GetParamQstCod ();

   /***** Build anchor string *****/
   Frm_SetAnchorStr (Set.SetCod,&Anchor);

   /***** Show question and button to remove question *****/
   Ale_ShowAlertAndButton (ActRemExaQst,Anchor,NULL,
			   ExaSet_PutParamsOneQst,&Exams,
			   Btn_REMOVE_BUTTON,Txt_Remove_question,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			   Exams.QstCod);

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void ExaSet_RemoveQstFromSet (void)
  {
   extern const char *Txt_Question_removed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   long QstCod;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Get question index *****/
   QstCod = ExaSet_GetParamQstCod ();

   /***** Remove media associated to question *****/
   ExaSet_RemoveMediaFromStemOfQst (QstCod,Set.SetCod);
   ExaSet_RemoveMediaFromAllAnsOfQst (QstCod,Set.SetCod);

   /***** Remove the question from set *****/
   Exa_DB_RemoveSetQuestion (QstCod,Set.SetCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/************ Remove media associated to stem of a test question *************/
/*****************************************************************************/

static void ExaSet_RemoveMediaFromStemOfQst (long QstCod,long SetCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media code associated to stem of set question from database *****/
   NumMedia = Exa_DB_GetMediaFromStemOfQst (&mysql_res,QstCod,SetCod);

   /***** Go over result removing media *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);
  }

/*****************************************************************************/
/****** Remove all media associated to all answers of an exam question *******/
/*****************************************************************************/

static void ExaSet_RemoveMediaFromAllAnsOfQst (long QstCod,long SetCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media codes associated to answers of test question from database *****/
   NumMedia = Exa_DB_GetMediaFromAllAnsOfQst (&mysql_res,QstCod,SetCod);

   /***** Go over result removing media *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Validate/invalidate a question **********************/
/*****************************************************************************/

void ExaSet_ValidateQst (void)
  {
   ExaSet_ChangeValidityQst (Qst_VALID_QUESTION);
  }

void ExaSet_InvalidateQst (void)
  {
   ExaSet_ChangeValidityQst (Qst_INVALID_QUESTION);
  }

static void ExaSet_ChangeValidityQst (Qst_Validity_t Validity)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   long QstCod;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Get question index *****/
   QstCod = ExaSet_GetParamQstCod ();

   /***** Validate/unvalidate question *****/
   Exa_DB_ChangeValidityQst (QstCod,Set.SetCod,Exam.ExaCod,Gbl.Hierarchy.Crs.CrsCod,
                             Validity);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/************************** Get and check parameters *************************/
/*****************************************************************************/

static void ExaSet_GetAndCheckParameters (struct Exa_Exams *Exams,
                                          struct Exa_Exam *Exam,
                                          struct ExaSet_Set *Set)
  {
   /***** Get parameters *****/
   Exa_GetParams (Exams);
   if (Exams->ExaCod <= 0)
      Err_WrongExamExit ();
   Exam->ExaCod = Exams->ExaCod;
   Grp_GetParamWhichGroups ();
   if ((Set->SetCod = ExaSet_GetParamSetCod ()) <= 0)
      Err_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (Exam);
   if (Exam->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Err_WrongExamExit ();
   Exams->ExaCod = Exam->ExaCod;

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (Set);
   if (Set->ExaCod != Exam->ExaCod)
      Err_WrongSetExit ();
   Exams->SetCod = Set->SetCod;
  }

/*****************************************************************************/
/************ Get the parameter with the code of a test question *************/
/*****************************************************************************/

static long ExaSet_GetParamQstCod (void)
  {
   /***** Get code of test question *****/
   return Par_GetParToLong ("QstCod");
  }

/*****************************************************************************/
/************ Put parameter with question code to edit, remove... ************/
/*****************************************************************************/

static void ExaSet_PutParamQstCod (void *QstCod)	// Should be a pointer to long
  {
   if (QstCod)
      if (*((long *) QstCod) > 0)	// If question exists
	 Par_PutHiddenParamLong (NULL,"QstCod",*((long *) QstCod));
  }

/*****************************************************************************/
/*********** Exchange the order of two consecutive sets in an exam ***********/
/*****************************************************************************/

static void ExaSet_ExchangeSets (long ExaCod,
                                 unsigned SetIndTop,unsigned SetIndBottom)
  {
   long SetCodTop;
   long SetCodBottom;

   /***** Lock table to make the move atomic *****/
   Exa_DB_LockTables ();

   /***** Get set codes of the sets to be moved *****/
   SetCodTop    = Exa_DB_GetSetCodFromSetInd (ExaCod,SetIndTop   );
   SetCodBottom = Exa_DB_GetSetCodFromSetInd (ExaCod,SetIndBottom);

   /***** Exchange indexes of sets *****/
   /*
   Example:
   SetIndTop    = 1; SetCodTop    = 218
   SetIndBottom = 2; SetCodBottom = 220
                     Step 1              Step 2              Step 3
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
| SetInd | SetCod | | SetInd | SetCod | | SetInd | SetCod | | SetInd | SetCod |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
|      1 |    218 |>|     -2 |    218 |>|     -2 |    218 |>|      2 |    218 |
|      2 |    220 | |      2 |    220 | |      1 |    220 | |      1 |    220 |
|      3 |    232 | |      3 |    232 | |      3 |    232 | |      3 |    232 |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
   */
   /* Step 1: change temporarily top index to minus bottom index
              in order to not repeat unique index (ExaCod,SetInd) */
   Exa_DB_UpdateSetIndex (-((long) SetIndBottom),SetCodTop   ,ExaCod);

   /* Step 2: change bottom index to old top index  */
   Exa_DB_UpdateSetIndex (  (long) SetIndTop    ,SetCodBottom,ExaCod);

   /* Step 3: change top index to old bottom index */
   Exa_DB_UpdateSetIndex (  (long) SetIndBottom ,SetCodTop   ,ExaCod);

   /***** Unlock table *****/
   Exa_DB_LockTables ();
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to exam *******************/
/*****************************************************************************/

static void ExaSet_PutIconToAddNewQuestions (void *Exams)
  {
   extern const char *Txt_Add_questions;

   /***** Put form to create a new question *****/
   Ico_PutContextualIconToAdd (ActReqAddQstExaSet,NULL,
			       ExaSet_PutParamsOneSet,Exams,
			       Txt_Add_questions);
  }

/*****************************************************************************/
/***************** Put button to add new questions to exam *******************/
/*****************************************************************************/

static void ExaSet_PutButtonToAddNewQuestions (struct Exa_Exams *Exams)
  {
   extern const char *Txt_Add_questions;

   Frm_BeginForm (ActReqAddQstExaSet);
      ExaSet_PutParamsOneSet (Exams);
      Btn_PutConfirmButtonInline (Txt_Add_questions);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Show title of exam set in exam print ********************/
/*****************************************************************************/

void ExaSet_WriteSetTitle (const struct ExaSet_Set *Set)
  {
   extern const char *Txt_question;
   extern const char *Txt_questions;

   /***** Begin table *****/
   HTM_TABLE_BeginWide ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"EXA_SET_TITLE\"");
	 HTM_Txt (Set->Title);
      HTM_TD_End ();

      /***** Number of questions to appear in exam print *****/
      HTM_TD_Begin ("class=\"EXA_SET_NUM_QSTS\"");
	 HTM_Unsigned (Set->NumQstsToPrint);
	 HTM_NBSP ();
	 HTM_Txt (Set->NumQstsToPrint == 1 ? Txt_question :
					     Txt_questions);
      HTM_TD_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }
