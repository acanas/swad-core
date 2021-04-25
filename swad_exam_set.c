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
#include "swad_exam.h"
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
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaSet_PutParamsOneQst (void *Exams);

static unsigned ExaSet_GetNumQstsInSet (long SetCod);

static bool ExaSet_CheckIfSimilarSetExists (const struct ExaSet_Set *Set,
                                            const char Title[ExaSet_MAX_BYTES_TITLE + 1]);

static void ExaSet_PutFormNewSet (struct Exa_Exams *Exams,
				  struct Exa_Exam *Exam,
				  struct ExaSet_Set *Set,
				  unsigned MaxSetInd);
static void ExaSet_ReceiveSetFieldsFromForm (struct ExaSet_Set *Set);
static bool ExaSet_CheckSetTitleReceivedFromForm (const struct ExaSet_Set *Set,
                                                  const char NewTitle[ExaSet_MAX_BYTES_TITLE + 1]);

static void ExaSet_CreateSet (struct ExaSet_Set *Set);
static void ExaSet_UpdateSet (const struct ExaSet_Set *Set);
static void ExaSet_UpdateSetTitleDB (const struct ExaSet_Set *Set,
                                     const char NewTitle[ExaSet_MAX_BYTES_TITLE + 1]);
static void ExaSet_UpdateNumQstsToExamDB (const struct ExaSet_Set *Set,
                                          unsigned NumQstsToPrint);

static void ExaSet_PutParamSetCod (long SetCod);

static unsigned ExaSet_GetSetIndFromSetCod (long ExaCod,long SetCod);
static long ExaSet_GetSetCodFromSetInd (long ExaCod,unsigned SetInd);

static unsigned ExaSet_GetMaxSetIndexInExam (long ExaCod);

static unsigned ExaSet_GetPrevSetIndexInExam (long ExaCod,unsigned SetInd);
static unsigned ExaSet_GetNextSetIndexInExam (long ExaCod,unsigned SetInd);

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
static void ExaSet_ListQuestionForEdition (struct Tst_Question *Question,
                                           unsigned QstInd,const char *Anchor);

static void ExaSet_AllocateListSelectedQuestions (struct Exa_Exams *Exams);
static void ExaSet_FreeListsSelectedQuestions (struct Exa_Exams *Exams);

static void ExaSet_CopyQstFromBankToExamSet (struct ExaSet_Set *Set,long QstCod);

static void ExaSet_RemoveMediaFromStemOfQst (long SetCod,long QstCod);
static void ExaSet_RemoveMediaFromAllAnsOfQst (long SetCod,long QstCod);

static void ExaSet_ChangeValidityQst (Tst_Validity_t Valid);

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
/********************* Get number of questions in a set **********************/
/*****************************************************************************/

static unsigned ExaSet_GetNumQstsInSet (long SetCod)
  {
   /***** Get number of questions in set from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of questions in a set",
		  "SELECT COUNT(*)"
		   " FROM exa_set_questions"
		  " WHERE SetCod=%ld",
		  SetCod);
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
   if (DB_QuerySELECT (&mysql_res,"can not get set data",
		       "SELECT SetCod,"		// row[0]
                              "ExaCod,"		// row[1]
			      "SetInd,"		// row[2]
			      "NumQstsToPrint,"	// row[3]
			      "Title"		// row[4]
		        " FROM exa_sets"
		       " WHERE SetCod=%ld",
		       Set->SetCod)) // Set found...
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
/************** Check if the title of a set of questions exists **************/
/*****************************************************************************/

static bool ExaSet_CheckIfSimilarSetExists (const struct ExaSet_Set *Set,
                                            const char Title[ExaSet_MAX_BYTES_TITLE + 1])
  {
   /***** Get number of set of questions with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar sets of questions",
			  "SELECT COUNT(*)"
			   " FROM exa_sets,"
			         "exa_exams"
			  " WHERE exa_sets.ExaCod=%ld"
			    " AND exa_sets.Title='%s'"
			    " AND exa_sets.SetCod<>%ld"
			    " AND exa_sets.ExaCod=exa_exams.ExaCod"
			    " AND exa_exams.CrsCod=%ld",	// Extra check
			  Set->ExaCod,Title,
			  Set->SetCod,
			  Gbl.Hierarchy.Crs.CrsCod) != 0);
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
   Tst_WriteNumQst (MaxSetInd + 1,"BIG_INDEX");
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
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   ItsANewSet = (Set.SetCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

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
	 if (ExaSet_CheckIfSimilarSetExists (Set,NewTitle))
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
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Receive new title from form *****/
   Par_GetParToText ("Title",NewTitle,ExaSet_MAX_BYTES_TITLE);

   /***** Check if title should be changed *****/
   if (ExaSet_CheckSetTitleReceivedFromForm (&Set,NewTitle))
     {
      /* Update the table changing old title by new title */
      ExaSet_UpdateSetTitleDB (&Set,NewTitle);

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
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Check if exam is editable *****/
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get number of questions in set to appear in exam print *****/
   NumQstsToPrint = (unsigned) Par_GetParToUnsignedLong ("NumQstsToPrint",
                                                         0,
                                                         UINT_MAX,
                                                         0);

   /***** Check if title should be changed *****/
   if (NumQstsToPrint != Set.NumQstsToPrint)
     {
      /* Update the table changing old number by new number */
      ExaSet_UpdateNumQstsToExamDB (&Set,NumQstsToPrint);

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
   MaxSetInd = ExaSet_GetMaxSetIndexInExam (Set->ExaCod);

   /***** Create a new exam *****/
   Set->SetCod =
   DB_QueryINSERTandReturnCode ("can not create new set of questions",
				"INSERT INTO exa_sets"
				" (ExaCod,SetInd,NumQstsToPrint,Title)"
				" VALUES"
				" (%ld,%u,%u,'%s')",
				Set->ExaCod,
				MaxSetInd + 1,
				Set->NumQstsToPrint,
				Set->Title);

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
   DB_QueryUPDATE ("can not update set of questions",
		   "UPDATE exa_sets"
		     " SET ExaCod=%ld,"
		          "SetInd=%u,"
		          "NumQstsToPrint=%u,"
		          "Title='%s'"
		   " WHERE SetCod=%ld",
		   Set->ExaCod,
		   Set->SetInd,
		   Set->NumQstsToPrint,
	           Set->Title,
	           Set->SetCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_set_of_questions_has_been_modified);
  }

/*****************************************************************************/
/************************ Update set title in database ***********************/
/*****************************************************************************/

static void ExaSet_UpdateSetTitleDB (const struct ExaSet_Set *Set,
                                     const char NewTitle[ExaSet_MAX_BYTES_TITLE + 1])
  {
   /***** Update set of questions changing old title by new title *****/
   DB_QueryUPDATE ("can not update the title of a set of questions",
		   "UPDATE exa_sets"
		     " SET Title='%s'"
		   " WHERE SetCod=%ld"
		     " AND ExaCod=%ld",	// Extra check
	           NewTitle,
	           Set->SetCod,
	           Set->ExaCod);
  }

/*****************************************************************************/
/****** Update number of questions to appear in exam print in database *******/
/*****************************************************************************/

static void ExaSet_UpdateNumQstsToExamDB (const struct ExaSet_Set *Set,
                                          unsigned NumQstsToPrint)
  {
   /***** Update set of questions changing old number by new number *****/
   DB_QueryUPDATE ("can not update the number of questions to appear in exam print",
		   "UPDATE exa_sets"
		     " SET NumQstsToPrint=%u"
		   " WHERE SetCod=%ld"
		     " AND ExaCod=%ld",	// Extra check
	           NumQstsToPrint,
	           Set->SetCod,
	           Set->ExaCod);
  }

/*****************************************************************************/
/******************* Get number of questions of an exam *********************/
/*****************************************************************************/

unsigned ExaSet_GetNumSetsExam (long ExaCod)
  {
   /***** Get number of sets in an exam from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of sets in an exam",
		  "SELECT COUNT(*)"
		   " FROM exa_sets"
		  " WHERE ExaCod=%ld",
		  ExaCod);
  }

/*****************************************************************************/
/******************* Get number of questions of an exam *********************/
/*****************************************************************************/

unsigned ExaSet_GetNumQstsExam (long ExaCod)
  {
   /***** Get total number of questions to appear in exam print *****/
   return DB_QuerySELECTUnsigned ("can not get number of questions in an exam print",
				  "SELECT SUM(NumQstsToPrint)"
				   " FROM exa_sets"
				  " WHERE ExaCod=%ld",
				  ExaCod);
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
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   ItsANewSet = (Set.SetCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   Exa_GetExamTxtFromDB (Exam.ExaCod,Txt);

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
   Tst_RequestSelectTestsForSet (&Exams);

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
   Tst_ListQuestionsToSelectForSet (&Exams);

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
/****************** Get set index given exam and set code ********************/
/*****************************************************************************/

static unsigned ExaSet_GetSetIndFromSetCod (long ExaCod,long SetCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long SetInd;

   /***** Get set index from set code *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get set index",
			"SELECT SetInd"		// row[0]
			 " FROM exa_sets"
			" WHERE SetCod=%u"
			  " AND ExaCod=%ld",	// Extra check
			SetCod,ExaCod))
      Lay_WrongSetExit ();

   /***** Get set code (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   SetInd = Str_ConvertStrToUnsigned (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return SetInd;
  }

/*****************************************************************************/
/****************** Get set code given exam and set index ********************/
/*****************************************************************************/

static long ExaSet_GetSetCodFromSetInd (long ExaCod,unsigned SetInd)
  {
   long SetCod;

   /***** Get set code from set index *****/
   SetCod = DB_QuerySELECTCode ("can not get set code",
				"SELECT SetCod"
				 " FROM exa_sets"
				" WHERE ExaCod=%ld"
				  " AND SetInd=%u",
				ExaCod,
				SetInd);
   if (SetCod <= 0)
      Lay_WrongSetExit ();

   return SetCod;
  }

/*****************************************************************************/
/********************* Get maximum set index in an exam **********************/
/*****************************************************************************/
// Question index can be 1, 2, 3...
// Return 0 if no questions

static unsigned ExaSet_GetMaxSetIndexInExam (long ExaCod)
  {
   /***** Get maximum set index in an exam from database *****/
   return DB_QuerySELECTUnsigned ("can not get max set index",
				  "SELECT MAX(SetInd)"
				   " FROM exa_sets"
				  " WHERE ExaCod=%ld",
				  ExaCod);
  }

/*****************************************************************************/
/*********** Get previous set index to a given set index in an exam **********/
/*****************************************************************************/
// Input set index can be 1, 2, 3... n-1
// Return set index will be 1, 2, 3... n if previous set exists, or 0 if no previous set

static unsigned ExaSet_GetPrevSetIndexInExam (long ExaCod,unsigned SetInd)
  {
   /***** Get previous set index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   return DB_QuerySELECTUnsigned ("can not get previous set index",
				  "SELECT MAX(SetInd)"
				   " FROM exa_sets"
				  " WHERE ExaCod=%ld"
				    " AND SetInd<%u",
				  ExaCod,
				  SetInd);
  }

/*****************************************************************************/
/*************** Get next set index to a given index in an exam **************/
/*****************************************************************************/
// Input set index can be 0, 1, 2, 3... n-1
// Return set index will be 1, 2, 3... n if next set exists, or big number if no next set

static unsigned ExaSet_GetNextSetIndexInExam (long ExaCod,unsigned SetInd)
  {
   unsigned NextSetInd;

   /***** Get next set index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   NextSetInd = DB_QuerySELECTUnsigned ("can not get next set index",
					"SELECT MIN(SetInd)"
					 " FROM exa_sets"
					" WHERE ExaCod=%ld"
					  " AND SetInd>%u",
					ExaCod,
					SetInd);
   if (NextSetInd == 0)
      NextSetInd = ExaSes_AFTER_LAST_QUESTION;	// End of sets has been reached

   return NextSetInd;
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
   MaxSetInd = ExaSet_GetMaxSetIndexInExam (Exam->ExaCod);

   /***** Get data of set of questions from database *****/
   NumSets = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get sets of questions",
		   "SELECT SetCod,"		// row[0]
			  "SetInd,"		// row[1]
			  "NumQstsToPrint,"	// row[2]
			  "Title"		// row[3]
		    " FROM exa_sets"
		   " WHERE ExaCod=%ld"
		   " ORDER BY SetInd",
		   Exam->ExaCod);

   /***** Begin box *****/
   Exams->ExaCod = Exam->ExaCod;
   Box_BoxBegin (NULL,Txt_Sets_of_questions,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams_question_sets,Box_NOT_CLOSABLE);

   /***** Show table with sets *****/
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

   /***** Get data of questions from database *****/
   NumQsts = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get exam questions",
		   "SELECT QstCod"	// row[0]
		    " FROM exa_set_questions"
		   " WHERE SetCod=%ld"
		   " ORDER BY Stem",
		   Set->SetCod);

   /***** Begin box *****/
   if (ICanEditQuestions)
      Box_BoxBegin (NULL,Txt_Questions,
		    ExaSet_PutIconToAddNewQuestions,Exams,
		    Hlp_ASSESSMENT_Exams_questions,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Questions,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Exams_questions,Box_NOT_CLOSABLE);

   /***** Show table with questions *****/
   if (NumQsts)
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

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (5);
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
      Tst_WriteNumQst (Set.SetInd,"BIG_INDEX");
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_ARTICLE_Begin (Anchor);
      if (ICanEditSets)
	{
	 Frm_StartFormAnchor (ActChgTitExaSet,Anchor);
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
      HTM_Unsigned (ExaSet_GetNumQstsInSet (Set.SetCod));
      HTM_SPAN_End ();
      HTM_TD_End ();

      /***** Number of questions to appear in exam print *****/
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      if (ICanEditSets)
	{
	 Frm_StartFormAnchor (ActChgNumQstExaSet,Anchor);
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
   unsigned NumQst;
   struct Tst_Question Question;
   char *Anchor;
   static Act_Action_t NextAction[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = ActValSetQst,	// Validate question (set it as valid question)
      [Tst_VALID_QUESTION  ] = ActInvSetQst,	// Invalidated question (set it as canceled question)
     };
   static const char *Icon[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "times-red.svg",
      [Tst_VALID_QUESTION  ] = "check-green.svg",
     };
   const char *Title[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = Txt_Invalid_question,
      [Tst_VALID_QUESTION  ] = Txt_Valid_question,
     };

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (5);
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Question);

   HTM_TR_End ();

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);

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
				     Title[Question.Validity]);
      HTM_TD_End ();

      /***** List question *****/
      ExaSet_ListQuestionForEdition (&Question,NumQst + 1,Anchor);

      /***** End row *****/
      HTM_TR_End ();

      /***** Free anchor string *****/
      Frm_FreeAnchorStr (Anchor);

      /***** Destroy test question *****/
      Tst_QstDestructor (&Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/*************** Get answer type of a question from database *****************/
/*****************************************************************************/

Tst_AnswerType_t ExaSet_GetQstAnswerTypeFromDB (long QstCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Tst_AnswerType_t AnswerType;

   /***** Get type of answer from database *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get the type of a question",
		       "SELECT AnsType"		// row[0]
		        " FROM exa_set_questions"
		       " WHERE QstCod=%ld",
		       QstCod))
      Lay_WrongQuestionExit ();

   /* Get type of answer */
   row = mysql_fetch_row (mysql_res);
   AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[0]);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return AnswerType;
  }

/*****************************************************************************/
/*************** Get data of a question in a set from database ***************/
/*****************************************************************************/

void ExaSet_GetQstDataFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool QuestionExists;
   unsigned NumOpt;

   /***** Get question data from database *****/
   QuestionExists = (DB_QuerySELECT (&mysql_res,"can not get a question",
				     "SELECT Invalid,"			// row[0]
				            "AnsType,"			// row[1]
					    "Shuffle,"			// row[2]
					    "Stem,"			// row[3]
					    "Feedback,"			// row[4]
					    "MedCod"			// row[5]
				      " FROM exa_set_questions"
				     " WHERE QstCod=%ld",
				     Question->QstCod) != 0);

   if (QuestionExists)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get whether the question is invalid (row[0]) */
      Question->Validity = (row[0][0] == 'Y') ? Tst_INVALID_QUESTION :
	                                        Tst_VALID_QUESTION;

      /* Get the type of answer (row[1]) */
      Question->Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

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
      ExaSet_GetAnswersQst (Question,&mysql_res,
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
	    case Tst_ANS_INT:
	       Tst_CheckIfNumberOfAnswersIsOne (Question);
	       Question->Answer.Integer = Tst_GetIntAnsFromStr (row[1]);
	       break;
	    case Tst_ANS_FLOAT:
	       if (Question->Answer.NumOptions != 2)
		  Lay_WrongAnswerExit ();
	       Question->Answer.FloatingPoint[NumOpt] = Str_GetDoubleFromStr (row[1]);
	       break;
	    case Tst_ANS_TRUE_FALSE:
	       Tst_CheckIfNumberOfAnswersIsOne (Question);
	       Question->Answer.TF = row[1][0];
	       break;
	    case Tst_ANS_UNIQUE_CHOICE:
	    case Tst_ANS_MULTIPLE_CHOICE:
	    case Tst_ANS_TEXT:
	       /* Check number of options */
	       if (Question->Answer.NumOptions > Tst_MAX_OPTIONS_PER_QUESTION)
		  Lay_WrongAnswerExit ();

	       /*  Allocate space for text and feedback */
	       if (!Tst_AllocateTextChoiceAnswer (Question,NumOpt))
		  /* Abort on error */
		  Ale_ShowAlertsAndExit ();

	       /* Get text (row[1]) */
	       Question->Answer.Options[NumOpt].Text[0] = '\0';
	       if (row[1])
		  if (row[1][0])
		     Str_Copy (Question->Answer.Options[NumOpt].Text    ,row[1],
			       Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

	       /* Get feedback (row[2]) */
	       Question->Answer.Options[NumOpt].Feedback[0] = '\0';
	       if (row[2])
		  if (row[2][0])
		     Str_Copy (Question->Answer.Options[NumOpt].Feedback,row[2],
			       Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

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
      Lay_WrongQuestionExit ();
  }

/*****************************************************************************/
/*************** Get answers of a test question from database ****************/
/*****************************************************************************/

void ExaSet_GetAnswersQst (struct Tst_Question *Question,MYSQL_RES **mysql_res,
                           bool Shuffle)
  {
   /***** Get answers of a question from database *****/
   Question->Answer.NumOptions = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get answers of a question",
		   "SELECT AnsInd,"		// row[0]
			  "Answer,"		// row[1]
			  "Feedback,"		// row[2]
			  "MedCod,"		// row[3]
			  "Correct"		// row[4]
		    " FROM exa_set_answers"
		   " WHERE QstCod=%ld"
		   " ORDER BY %s",
		   Question->QstCod,
		   Shuffle ? "RAND()" :
		             "AnsInd");
   if (!Question->Answer.NumOptions)
      Ale_ShowAlert (Ale_ERROR,"Error when getting answers of a question.");
  }


/*****************************************************************************/
/********************* List question in set for edition **********************/
/*****************************************************************************/

static void ExaSet_ListQuestionForEdition (struct Tst_Question *Question,
                                           unsigned QstInd,const char *Anchor)
  {
   static char *ClassNumQst[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "BIG_INDEX_RED",
      [Tst_VALID_QUESTION  ] = "BIG_INDEX",
     };
   static char *ClassAnswerType[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "DAT_SMALL_RED",
      [Tst_VALID_QUESTION  ] = "DAT_SMALL",
     };
   static char *ClassTxt[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "TEST_TXT_RED",
      [Tst_VALID_QUESTION  ] = "TEST_TXT",
     };
   static char *ClassFeedback[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "TEST_TXT_LIGHT_RED",
      [Tst_VALID_QUESTION  ] = "TEST_TXT_LIGHT",
     };

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (QstInd,ClassNumQst[Question->Validity]);
   Tst_WriteAnswerType (Question->Answer.Type,ClassAnswerType[Question->Validity]);
   HTM_TD_End ();

   /***** Write stem (row[3]) and media *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_ARTICLE_Begin (Anchor);

   /* Write stem */
   Tst_WriteQstStem (Question->Stem,ClassTxt[Question->Validity],
		     true);	// Visible

   /* Show media */
   Med_ShowMedia (&Question->Media,
		  "TEST_MED_EDIT_LIST_CONT",
		  "TEST_MED_EDIT_LIST");

   /* Show feedback */
   Tst_WriteQstFeedback (Question->Feedback,ClassFeedback[Question->Validity]);

   /* Show answers */
   Tst_WriteAnswersBank (Question,
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
   if (Tst_CountNumQuestionsInList (Exams.ListQuestions))	// If questions selected...
     {
      /***** Insert questions in database *****/
      Ptr = Exams.ListQuestions;
      while (*Ptr)
	{
	 /* Get next code */
	 Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 if (sscanf (LongStr,"%ld",&QstCod) != 1)
	    Lay_WrongQuestionExit ();

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
         Lay_NotEnoughMemoryExit ();
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

static void ExaSet_CopyQstFromBankToExamSet (struct ExaSet_Set *Set,long QstCod)
  {
   extern const char *Tst_StrAnswerTypesDB[Tst_NUM_ANS_TYPES];
   extern const char *Txt_Question_removed;
   struct Tst_Question Question;
   long CloneMedCod;
   long QstCodInSet;
   unsigned NumOpt;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   static char CharInvalid[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = 'Y',
      [Tst_VALID_QUESTION  ] = 'N'
     };

   /***** Create test question *****/
   Tst_QstConstructor (&Question);
   Question.QstCod = QstCod;

   /***** Get data of question from database *****/
   if (Tst_GetQstDataFromDB (&Question))
     {
      /***** Clone media *****/
      CloneMedCod = Med_CloneMedia (&Question.Media);

      /***** Insert question in table of questions *****/
      QstCodInSet =
      DB_QueryINSERTandReturnCode ("can not add question to set",
				   "INSERT INTO exa_set_questions"
				   " (SetCod,Invalid,AnsType,Shuffle,"
				     "Stem,Feedback,MedCod)"
				   " VALUES"
				   " (%ld,'%c','%s','%c',"
				    "'%s','%s',%ld)",
				   Set->SetCod,
				   CharInvalid[Question.Validity],
				   Tst_StrAnswerTypesDB[Question.Answer.Type],
				   Question.Answer.Shuffle ? 'Y' :
							     'N',
				   Question.Stem,
				   Question.Feedback,
				   CloneMedCod);

      /***** Get the answers from the database *****/
      Tst_GetAnswersQst (&Question,&mysql_res,
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
	 DB_QueryINSERT ("can not add answer to set",
			 "INSERT INTO exa_set_answers"
			 " (QstCod,AnsInd,Answer,Feedback,MedCod,Correct)"
			 " VALUES"
			 " (%ld,%u,'%s','%s',%ld,'%s')",
			 QstCodInSet,	// Question code in set
			 NumOpt,	// Answer index (number of option)
			 row[1],	// Copy of text
			 row[2],	// Copy of feedback
			 CloneMedCod,	// Media code of the new cloned media
			 row[4]);	// Copy of correct
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);

   /***** Destroy test question *****/
   Tst_QstDestructor (&Question);
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
      Lay_NoPermissionExit ();

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
      Lay_NoPermissionExit ();

   /***** Remove the set from all the tables *****/
   /* Remove questions associated to set */
   DB_QueryDELETE ("can not remove questions associated to set",
		   "DELETE FROM exa_set_questions"
		   " USING exa_set_questions,"
		          "exa_sets"
		   " WHERE exa_set_questions.SetCod=%ld"
                     " AND exa_set_questions.SetCod=exa_sets.SetCod"
		     " AND exa_sets.ExaCod=%ld",	// Extra check
		   Set.SetCod,
		   Set.ExaCod);

   /* Remove the set itself */
   DB_QueryDELETE ("can not remove set",
		   "DELETE FROM exa_sets"
		   " WHERE SetCod=%ld"
                     " AND ExaCod=%ld",		// Extra check
		   Set.SetCod,
		   Set.ExaCod);
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The set to be removed does not exist.");

   /* Change index of sets greater than this */
   DB_QueryUPDATE ("can not update indexes of sets",
		   "UPDATE exa_sets"
		     " SET SetInd=SetInd-1"
		   " WHERE ExaCod=%ld"
		     " AND SetInd>%u",
		   Set.ExaCod,
		   Set.SetInd);

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
      Lay_NoPermissionExit ();

   /***** Get set index *****/
   SetIndBottom = ExaSet_GetSetIndFromSetCod (Exam.ExaCod,Set.SetCod);

   /***** Move up set *****/
   if (SetIndBottom > 1)
     {
      /* Indexes of sets to be exchanged */
      SetIndTop = ExaSet_GetPrevSetIndexInExam (Exam.ExaCod,SetIndBottom);
      if (!SetIndTop)
	 Lay_ShowErrorAndExit ("Wrong index of set.");

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
      Lay_NoPermissionExit ();

   /***** Get set index *****/
   SetIndTop = ExaSet_GetSetIndFromSetCod (Exam.ExaCod,Set.SetCod);

   /***** Get maximum set index *****/
   MaxSetInd = ExaSet_GetMaxSetIndexInExam (Exam.ExaCod);

   /***** Move down set *****/
   if (SetIndTop < MaxSetInd)
     {
      /* Indexes of sets to be exchanged */
      SetIndBottom = ExaSet_GetNextSetIndexInExam (Exam.ExaCod,SetIndTop);
      if (!SetIndBottom)
	 Lay_ShowErrorAndExit ("Wrong index of set.");

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
   ExaSet_RemoveMediaFromStemOfQst (Set.SetCod,QstCod);
   ExaSet_RemoveMediaFromAllAnsOfQst (Set.SetCod,QstCod);

   /***** Remove the question from set *****/
   /* Remove the question itself */
   DB_QueryDELETE ("can not remove a question from a set",
		   "DELETE FROM exa_set_questions"
		   " WHERE QstCod=%ld"
		     " AND SetCod=%ld",	// Extra check
		   QstCod,
		   Set.SetCod);
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_WrongQuestionExit ();

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/************ Remove media associated to stem of a test question *************/
/*****************************************************************************/

static void ExaSet_RemoveMediaFromStemOfQst (long SetCod,long QstCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media code associated to stem of test question from database *****/
   NumMedia = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get media",
		   "SELECT MedCod"	// row[0]
		    " FROM exa_set_questions"
		   " WHERE QstCod=%ld"
		     " AND SetCod=%ld",	// Extra check
		   QstCod,
		   SetCod);

   /***** Go over result removing media *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);
  }

/*****************************************************************************/
/****** Remove all media associated to all answers of an exam question *******/
/*****************************************************************************/

static void ExaSet_RemoveMediaFromAllAnsOfQst (long SetCod,long QstCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMedia;

   /***** Get media codes associated to answers of test question from database *****/
   NumMedia = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get media",
		   "SELECT exa_set_answers.MedCod"	// row[0]
		    " FROM exa_set_answers,"
			  "exa_set_questions"
		   " WHERE exa_set_answers.QstCod=%ld"
		     " AND exa_set_answers.QstCod=exa_set_questions.QstCod"
		     " AND exa_set_questions.SetCod=%ld"	// Extra check
		     " AND exa_set_questions.QstCod=%ld",	// Extra check
		   QstCod,
		   SetCod,
		   QstCod);

   /***** Go over result removing media *****/
   Med_RemoveMediaFromAllRows (NumMedia,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************************** Validate a question ***************************/
/*****************************************************************************/

void ExaSet_ValidateQst (void)
  {
   ExaSet_ChangeValidityQst (Tst_VALID_QUESTION);
  }

void ExaSet_InvalidateQst (void)
  {
   ExaSet_ChangeValidityQst (Tst_INVALID_QUESTION);
  }

static void ExaSet_ChangeValidityQst (Tst_Validity_t Validity)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   long QstCod;
   static char CharInvalid[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = 'Y',
      [Tst_VALID_QUESTION  ] = 'N'
     };

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get and check parameters *****/
   ExaSet_GetAndCheckParameters (&Exams,&Exam,&Set);

   /***** Get question index *****/
   QstCod = ExaSet_GetParamQstCod ();

   /***** Validate question *****/
   DB_QueryUPDATE ("can not validate question",
		   "UPDATE exa_set_questions,"
		          "exa_sets,exa_exams"
		     " SET exa_set_questions.Invalid='%c'"
		   " WHERE exa_set_questions.QstCod=%ld"
		     " AND exa_set_questions.SetCod=%ld"	// Extra check
		     " AND exa_set_questions.SetCod=exa_sets.SetCod"
		     " AND exa_sets.ExaCod=%ld"			// Extra check
		     " AND exa_sets.ExaCod=exa_exams.ExaCod"
		     " AND exa_exams.CrsCod=%ld",		// Extra check
		   CharInvalid[Validity],
		   QstCod,
		   Set.SetCod,
		   Exam.ExaCod,
		   Gbl.Hierarchy.Crs.CrsCod);

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
      Lay_WrongExamExit ();
   Exam->ExaCod = Exams->ExaCod;
   Grp_GetParamWhichGroups ();
   if ((Set->SetCod = ExaSet_GetParamSetCod ()) <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (Exam);
   if (Exam->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_WrongExamExit ();
   Exams->ExaCod = Exam->ExaCod;

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (Set);
   if (Set->ExaCod != Exam->ExaCod)
      Lay_WrongSetExit ();
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
   DB_Query ("can not lock tables to exchange sets of questions",
	     "LOCK TABLES exa_sets WRITE");
   Gbl.DB.LockedTables = true;

   /***** Get set codes of the sets to be moved *****/
   SetCodTop    = ExaSet_GetSetCodFromSetInd (ExaCod,SetIndTop);
   SetCodBottom = ExaSet_GetSetCodFromSetInd (ExaCod,SetIndBottom);

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
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets"
		     " SET SetInd=-%u"
		   " WHERE ExaCod=%ld"
		     " AND SetCod=%ld",
		   SetIndBottom,
		   ExaCod,
		   SetCodTop);

   /* Step 2: change bottom index to old top index  */
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets"
		     " SET SetInd=%u"
		   " WHERE ExaCod=%ld"
		     " AND SetCod=%ld",
		   SetIndTop,
		   ExaCod,
		   SetCodBottom);

   /* Step 3: change top index to old bottom index */
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets"
		     " SET SetInd=%u"
		   " WHERE ExaCod=%ld"
		     " AND SetCod=%ld",
		   SetIndBottom,
		   ExaCod,
		   SetCodTop);

   /***** Unlock table *****/
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after exchanging sets of questions",
	     "UNLOCK TABLES");
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
