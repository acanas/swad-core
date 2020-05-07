// swad_exam_set.c: set of questions in exams

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_exam.h"
#include "swad_exam_event.h"
#include "swad_exam_result.h"
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

static void ExaSet_AllocateListSelectedQuestions (struct Exa_Exams *Exams);
static void ExaSet_FreeListsSelectedQuestions (struct Exa_Exams *Exams);

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
   Tst_PutParamQstCod (&(((struct Exa_Exams *) Exams)->QstCod));
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
   return
   (unsigned) DB_QueryCOUNT ("can not get number of questions in a set",
			     "SELECT COUNT(*) FROM exa_questions"
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
			      "SetInd,"		// row[1]
			      "NumQstsToPrint,"	// row[2]
			      "Title"		// row[3]
		       " FROM exa_sets"
		       " WHERE SetCod=%ld"
		       " AND ExaCod=%ld",	// Extra check
		       Set->SetCod,Set->ExaCod)) // Set found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] SetCod
      row[1] SetInd
      row[2] NumQstsToPrint
      row[3] Title
      */
      /* Get set code (row[0]) */
      Set->SetCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get set index (row[1]) */
      Set->SetInd = Str_ConvertStrToUnsigned (row[1]);
      snprintf (StrSetInd,sizeof (Set->SetInd),
	        "%u",
		Set->SetInd);

      /* Get set index (row[2]) */
      Set->NumQstsToPrint = Str_ConvertStrToUnsigned (row[2]);

      /* Get the title of the set (row[3]) */
      Str_Copy (Set->Title,row[3],
                ExaSet_MAX_BYTES_TITLE);
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
			  "SELECT COUNT(*) FROM exa_sets,exa_exams"
			  " WHERE exa_sets.ExaCod=%ld AND exa_sets.Title='%s'"
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
   // extern const char *Hlp_ASSESSMENT_Exams_new_set;
   // extern const char *Hlp_ASSESSMENT_Exams_edit_set;
   extern const char *Txt_New_set_of_questions;
   extern const char *Txt_Create_set_of_questions;

   /***** Begin form *****/
   Exams->ExaCod = Exam->ExaCod;
   Frm_StartForm (ActNewExaSet);
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
   Tst_WriteNumQst (MaxSetInd + 1);
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
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   ItsANewSet = (Set.SetCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();
   Exams.SetCod = Set.SetCod;

   /***** Get exam and set data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

   /***** Receive new title from form *****/
   Par_GetParToText ("Title",NewTitle,ExaSet_MAX_BYTES_TITLE);

   /***** Check if title should be changed *****/
   if (ExaSet_CheckSetTitleReceivedFromForm (&Set,NewTitle))
     {
      /* Update the table changing old title by new title */
      ExaSet_UpdateSetTitleDB (&Set,NewTitle);

      /* Update title */
      Str_Copy (Set.Title,NewTitle,
		ExaSet_MAX_BYTES_TITLE);
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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();
   Exams.SetCod = Set.SetCod;

   /***** Get exam and set data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

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
		   "UPDATE exa_sets SET Title='%s'"
		   " WHERE SetCod=%ld"
		   " AND ExaCod=%ld",	// Extra check
	           NewTitle,
	           Set->SetCod,Set->ExaCod);
  }

/*****************************************************************************/
/****** Update number of questions to appear in exam print in database *******/
/*****************************************************************************/

static void ExaSet_UpdateNumQstsToExamDB (const struct ExaSet_Set *Set,
                                          unsigned NumQstsToPrint)
  {
   /***** Update set of questions changing old number by new number *****/
   DB_QueryUPDATE ("can not update the number of questions to appear in exam print",
		   "UPDATE exa_sets SET NumQstsToPrint=%u"
		   " WHERE SetCod=%ld"
		   " AND ExaCod=%ld",	// Extra check
	           NumQstsToPrint,
	           Set->SetCod,Set->ExaCod);
  }

/*****************************************************************************/
/******************* Get number of questions of an exam *********************/
/*****************************************************************************/

unsigned ExaSet_GetNumSetsExam (long ExaCod)
  {
   /***** Get number of sets in an exam from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of sets in an exam",
			     "SELECT COUNT(*) FROM exa_sets"
			     " WHERE ExaCod=%ld",
			     ExaCod);
  }

/*****************************************************************************/
/******************* Get number of questions of an exam *********************/
/*****************************************************************************/

unsigned ExaSet_GetNumQstsExam (long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts = 0;

   /***** Get total number of questions to appear in exam print *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get number of questions in an exam print",
			"SELECT SUM(NumQstsToPrint) FROM exa_sets"
			" WHERE ExaCod=%ld",
			ExaCod))
      Lay_ShowErrorAndExit ("Error: wrong question index.");

   /***** Get number of questions (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])
      NumQsts = Str_ConvertStrToUnsigned (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumQsts;
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
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   Exa_GetExamTxtFromDB (Exam.ExaCod,Txt);
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

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
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   Exa_GetExamTxtFromDB (Exam.ExaCod,Txt);
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

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
			"SELECT SetInd FROM exa_sets"
			" WHERE SetCod=%u"
			" AND ExaCod=%ld",	// Extra check
			SetCod,ExaCod))
      Lay_ShowErrorAndExit ("Error: wrong set code.");

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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long SetCod;

   /***** Get set code from set index *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get set code",
			"SELECT SetCod FROM exa_sets"
			" WHERE ExaCod=%ld AND SetInd=%u",
			ExaCod,SetInd))
      Lay_ShowErrorAndExit ("Error: wrong set index.");

   /***** Get set code (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if ((SetCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Lay_ShowErrorAndExit ("Error: wrong set code.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return SetCod;
  }

/*****************************************************************************/
/********************* Get maximum set index in an exam **********************/
/*****************************************************************************/
// Question index can be 1, 2, 3...
// Return 0 if no questions

static unsigned ExaSet_GetMaxSetIndexInExam (long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned SetInd = 0;

   /***** Get maximum set index in an exam from database *****/
   DB_QuerySELECT (&mysql_res,"can not get max set index",
		   "SELECT MAX(SetInd)"
		   " FROM exa_sets"
		   " WHERE ExaCod=%ld",
                   ExaCod);
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are sets
      if (sscanf (row[0],"%u",&SetInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting max set index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return SetInd;
  }

/*****************************************************************************/
/*********** Get previous set index to a given set index in an exam **********/
/*****************************************************************************/
// Input set index can be 1, 2, 3... n-1
// Return set index will be 1, 2, 3... n if previous set exists, or 0 if no previous set

static unsigned ExaSet_GetPrevSetIndexInExam (long ExaCod,unsigned SetInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned PrevSetInd = 0;

   /***** Get previous set index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get previous set index",
			"SELECT MAX(SetInd) FROM exa_sets"
			" WHERE ExaCod=%ld AND SetInd<%u",
			ExaCod,SetInd))
      Lay_ShowErrorAndExit ("Error: previous set index not found.");

   /***** Get previous set index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row)
      if (row[0])
	 if (sscanf (row[0],"%u",&PrevSetInd) != 1)
	    Lay_ShowErrorAndExit ("Error when getting previous set index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PrevSetInd;
  }

/*****************************************************************************/
/*************** Get next set index to a given index in an exam **************/
/*****************************************************************************/
// Input set index can be 0, 1, 2, 3... n-1
// Return set index will be 1, 2, 3... n if next set exists, or 0 if no next set

static unsigned ExaSet_GetNextSetIndexInExam (long ExaCod,unsigned SetInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NextSetInd = ExaEvt_AFTER_LAST_QUESTION;	// End of sets has been reached

   /***** Get next set index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get next set index",
			"SELECT MIN(SetInd) FROM exa_sets"
			" WHERE ExaCod=%ld AND SetInd>%u",
			ExaCod,SetInd))
      Lay_ShowErrorAndExit ("Error: next set index not found.");

   /***** Get next set index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row)
      if (row[0])
	 if (sscanf (row[0],"%u",&NextSetInd) != 1)
	    Lay_ShowErrorAndExit ("Error when getting next set index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

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
			      "SELECT exa_questions.QstCod"	// row[0]
			      " FROM exa_questions LEFT JOIN tst_questions"	// LEFT JOIN because the question could be removed in table of test questions
			      " ON (exa_questions.QstCod=tst_questions.QstCod)"
			      " WHERE exa_questions.SetCod=%ld"
			      " ORDER BY tst_questions.Stem",
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
   HTM_TABLE_BeginWideMarginPadding (2);
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
      snprintf (StrSetInd,sizeof (Set.SetInd),
	        "%u",
		Set.SetInd);

      /* Get number of questions to exam (row[2]) */
      Set.NumQstsToPrint = Str_ConvertStrToUnsigned (row[2]);

      /* Get the title of the set (row[3]) */
      Str_Copy (Set.Title,row[3],
                ExaSet_MAX_BYTES_TITLE);

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
	{
	 Frm_StartForm (ActReqRemExaSet);
	 ExaSet_PutParamsOneSet (Exams);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}
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
      Tst_WriteNumQst (Set.SetInd);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_ARTICLE_Begin (Anchor);
      Frm_StartFormAnchor (ActChgTitExaSet,Anchor);
      ExaSet_PutParamsOneSet (Exams);
      HTM_INPUT_TEXT ("Title",ExaSet_MAX_CHARS_TITLE,Set.Title,
                      HTM_SUBMIT_ON_CHANGE,
		      "id=\"Title\" required=\"required\""
		      " class=\"TITLE_DESCRIPTION_WIDTH\"");
      Frm_EndForm ();
      HTM_ARTICLE_End ();
      HTM_TD_End ();

      /***** Current number of questions in set *****/
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Unsigned (ExaSet_GetNumQstsInSet (Set.SetCod));
      HTM_TD_End ();

      /***** Number of questions to appear in exam print *****/
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      Frm_StartFormAnchor (ActChgNumQstExaSet,Anchor);
      ExaSet_PutParamsOneSet (Exams);
      HTM_INPUT_LONG ("NumQstsToPrint",0,UINT_MAX,(long) Set.NumQstsToPrint,
                      HTM_SUBMIT_ON_CHANGE,false,
		       "class=\"INPUT_LONG\" required=\"required\"");
      Frm_EndForm ();
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
   extern const char *Txt_Number_of_questions_in_the_exam;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Header cells *****/
   HTM_TH_Empty (1);
   HTM_TH (1,1,"RB",Txt_No_INDEX);
   HTM_TH (1,1,"LB",Txt_Set_of_questions);
   HTM_TH (1,1,"RB",Txt_Number_of_questions);
   HTM_TH (1,1,"RB",Txt_Number_of_questions_in_the_exam);

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
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Tags;
   extern const char *Txt_Question;
   unsigned NumQst;
   MYSQL_ROW row;
   struct Tst_Question Question;
   bool QuestionExists;
   char *Anchor;

   /***** Build anchor string *****/
   Frm_SetAnchorStr (Exams->SetCod,&Anchor);

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Code);
   HTM_TH (1,1,"CT",Txt_Tags);
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
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] QstCod
      */
      /* Get question code (row[0]) */
      Exams->QstCod = Question.QstCod = Str_ConvertStrCodToLongCod (row[0]);

      /***** Begin row *****/
      HTM_TR_Begin (NULL);

      /***** Icons *****/
      HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

      /* Put icon to remove the question */
      if (ICanEditQuestions)
	{
	 Frm_StartForm (ActReqRemSetQst);
         ExaSet_PutParamsOneQst (Exams);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}
      else
         Ico_PutIconRemovalNotAllowed ();

      /* Put icon to edit the question */
      if (ICanEditQuestions)
	 Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
	                              Tst_PutParamQstCod,&Question.QstCod);

      HTM_TD_End ();

      /***** Question *****/
      QuestionExists = Tst_GetQstDataFromDB (&Question);
      Tst_ListQuestionForEdition (&Question,NumQst + 1,QuestionExists,Anchor);

      /***** End row *****/
      HTM_TR_End ();

      /***** Destroy test question *****/
      Tst_QstDestructor (&Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
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
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   Exa_GetExamTxtFromDB (Exam.ExaCod,Txt);
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

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
	    Lay_ShowErrorAndExit ("Wrong question code.");

	 /* Insert question in the table of questions */
	 DB_QueryINSERT ("can not add question to set",
			 "INSERT INTO exa_questions"
			 " (SetCod,QstCod)"
			 " VALUES"
			 " (%ld,%ld)",
			 Set.SetCod,QstCod);
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
      if ((Exams->ListQuestions = (char *) malloc (ExaSet_MAX_BYTES_LIST_SELECTED_QUESTIONS + 1)) == NULL)
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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

   /***** Remove the set from all the tables *****/
   /* Remove questions associated to set */
   DB_QueryDELETE ("can not remove questions associated to set",
		   "DELETE FROM exa_questions"
		   " USING exa_questions,exa_sets"
		   " WHERE exa_questions.SetCod=%ld"
                   " AND exa_questions.SetCod=exa_sets.SetCod"
		   " AND exa_sets.ExaCod=%ld",	// Extra check
		   Set.SetCod,Set.ExaCod);

   /* Remove the set itself */
   DB_QueryDELETE ("can not remove set",
		   "DELETE FROM exa_sets"
		   " WHERE SetCod=%ld"
                   " AND ExaCod=%ld",		// Extra check
		   Set.SetCod,Set.ExaCod);
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The set to be removed does not exist.");

   /* Change index of sets greater than this */
   DB_QueryUPDATE ("can not update indexes of sets",
		   "UPDATE exa_sets SET SetInd=SetInd-1"
		   " WHERE ExaCod=%ld AND SetInd>%u",
		   Set.ExaCod,Set.SetInd);

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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

   /***** Get question index *****/
   Exams.QstCod = Tst_GetParamQstCod ();

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

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);
   Exams.SetCod = Set.SetCod;

   /***** Get question index *****/
   QstCod = Tst_GetParamQstCod ();

   /***** Remove the question from set *****/
   /* Remove the question itself */
   DB_QueryDELETE ("can not remove a question from a set",
		   "DELETE FROM exa_questions"
		   " WHERE SetCod=%ld AND QstCod=%ld",
		   Set.SetCod,QstCod);
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The question to be removed does not exist.");

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
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
		   "UPDATE exa_sets SET SetInd=-%u"
		   " WHERE ExaCod=%ld AND SetCod=%ld",
		   SetIndBottom,
		   ExaCod,SetCodTop);

   /* Step 2: change bottom index to old top index  */
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets SET SetInd=%u"
		   " WHERE ExaCod=%ld AND SetCod=%ld",
		   SetIndTop,
		   ExaCod,SetCodBottom);

   /* Step 3: change top index to old bottom index */
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets SET SetInd=%u"
		   " WHERE ExaCod=%ld AND SetCod=%ld",
		   SetIndBottom,
		   ExaCod,SetCodTop);

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
			       Exa_PutParams,Exams,
			       Txt_Add_questions);
  }

/*****************************************************************************/
/***************** Put button to add new questions to exam *******************/
/*****************************************************************************/

static void ExaSet_PutButtonToAddNewQuestions (struct Exa_Exams *Exams)
  {
   extern const char *Txt_Add_questions;

   Frm_StartForm (ActReqAddQstExaSet);
   ExaSet_PutParamsOneSet (Exams);
   Btn_PutConfirmButtonInline (Txt_Add_questions);
   Frm_EndForm ();
  }
