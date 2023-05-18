// swad_test_print.c: test prints made by users

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
#include <math.h>		// For fabs
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
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
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_question.h"
#include "swad_question_database.h"
#include "swad_test.h"
#include "swad_test_database.h"
#include "swad_test_print.h"
#include "swad_test_visibility.h"
#include "swad_user.h"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct TstRes_ICanView
  {
   bool Result;
   bool Score;
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TstPrn_ResetPrintExceptPrnCod (struct TstPrn_Print *Print);

static void TstPrn_WriteQstAndAnsToFill (struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned QstInd,
                                         struct Qst_Question *Question);
static void TstPrn_WriteAnswersToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                       unsigned QstInd,
                                       struct Qst_Question *Question);

//-----------------------------------------------------------------------------
static void TstPrn_WriteIntAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
static void TstPrn_WriteFltAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
static void TstPrn_WriteTF_AnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
static void TstPrn_WriteChoAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      struct Qst_Question *Question);
static void TstPrn_WriteTxtAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question);
//-----------------------------------------------------------------------------

static void TstPrn_PutCheckBoxAllowTeachers (bool AllowTeachers);

static void TstPrn_WriteQstAndAnsExam (struct Usr_Data *UsrDat,
				       struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST],
				       unsigned QstInd,
				       time_t TimeUTC[Dat_NUM_START_END_TIME],
				       struct Qst_Question *Question,
				       bool QuestionExists,
				       unsigned Visibility);

//-----------------------------------------------------------------------------
static void TstPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void TstPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void TstPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void TstPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
static void TstPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question);
//-----------------------------------------------------------------------------
static void TstPrn_WriteIntAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
static void TstPrn_WriteFltAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
static void TstPrn_WriteTF_AnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
static void TstPrn_WriteChoAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     const char *ClassTxt,
				     const char *ClassFeedback);
static void TstPrn_WriteTxtAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
//-----------------------------------------------------------------------------

static void TstPrn_WriteHeadUserCorrect (struct Usr_Data *UsrDat);

static void TstPrn_PutFormToSelectUsrsToViewUsrsPrints (__attribute__((unused)) void *Args);

static void TstPrn_ShowUsrsPrints (__attribute__((unused)) void *Args);
static void TstPrn_ShowHeaderPrints (Usr_MeOrOther_t MeOrOther);
static void TstPrn_ShowUsrPrints (struct Usr_Data *UsrDat);
static void TstPrn_ShowPrintsSummaryRow (Usr_MeOrOther_t MeOrOther,
                                         unsigned NumPrints,
                                         struct TstPrn_NumQuestions *NumTotalQsts,
                                         double TotalScore);

static void TstRes_CheckIfICanSeePrintResult (const struct TstPrn_Print *Print,
                                              long UsrCod,
                                              struct TstRes_ICanView *ICanView);

static void TstPrn_ShowTagsPresentInAPrint (long PrnCod);

/*****************************************************************************/
/***************************** Reset test print ******************************/
/*****************************************************************************/

void TstPrn_ResetPrint (struct TstPrn_Print *Print)
  {
   Print->PrnCod = -1L;
   TstPrn_ResetPrintExceptPrnCod (Print);
  }

static void TstPrn_ResetPrintExceptPrnCod (struct TstPrn_Print *Print)
  {
   Print->TimeUTC[Dat_STR_TIME] =
   Print->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Print->NumQsts.All      =
   Print->NumQsts.NotBlank = 0;
   Print->Sent             = false;	// After creating an exam, it's not sent
   Print->AllowTeachers    = false;	// Teachers can't seen the exam if student don't allow it
   Print->Score            = 0.0;
  }

/*****************************************************************************/
/********************* Show a test print to be answered **********************/
/*****************************************************************************/

void TstPrn_ShowTestPrintToFillIt (struct TstPrn_Print *Print,
                                   unsigned NumPrintsGeneratedByMe,
                                   TstPrn_RequestOrConfirm_t RequestOrConfirm)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Test;
   extern const char *Txt_Continue;
   extern const char *Txt_Send;
   unsigned QstInd;
   struct Qst_Question Question;
   static const Act_Action_t Action[Tst_NUM_REQUEST_OR_CONFIRM] =
     {
      [TstPrn_REQUEST] = ActReqAssTst,
      [TstPrn_CONFIRM] = ActAssTst,
     };

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Test,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (false,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   if (Print->NumQsts.All)
     {
      /***** Begin form *****/
      Frm_BeginForm (Action[RequestOrConfirm]);
	 ParCod_PutPar (ParCod_Prn,Print->PrnCod);
	 Par_PutParUnsigned (NULL,"NumTst",NumPrintsGeneratedByMe);

	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (10);

	    /***** Write one row for each question *****/
	    for (QstInd = 0, The_ResetRowColor ();
		 QstInd < Print->NumQsts.All;
		 QstInd++, The_ChangeRowColor ())
	      {
	       /* Create test question */
	       Qst_QstConstructor (&Question);
	       Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;

	       /* Show question */
	       if (!Qst_GetQstDataByCod (&Question))	// Question exists
		  Err_WrongQuestionExit ();

	       /* Write question and answers */
	       TstPrn_WriteQstAndAnsToFill (&Print->PrintedQuestions[QstInd],QstInd,&Question);

	       /* Destroy test question */
	       Qst_QstDestructor (&Question);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

         /***** Button *****/
	 switch (RequestOrConfirm)
	   {
	    case TstPrn_REQUEST:
	       /* Send button */
	       Btn_PutConfirmButton (Txt_Continue);
	       break;
	    case TstPrn_CONFIRM:
	       /* Will the test be visible by teachers? */
	       TstPrn_PutCheckBoxAllowTeachers (true);

	       /* Send button */
	       Btn_PutCreateButton (Txt_Send);
	       break;
	   }

      /***** End form *****/
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void TstPrn_WriteQstAndAnsToFill (struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned QstInd,
                                         struct Qst_Question *Question)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Number of question and answer type *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Question->Answer.Type,"DAT_SMALL");
      HTM_TD_End ();

      /***** Stem, media and answers *****/
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());

	 /* Write parameter with question code */
	 Qst_WriteParQstCod (QstInd,Question->QstCod);

	 /* Stem */
	 Qst_WriteQstStem (Question->Stem,"Qst_TXT",true);

	 /* Media */
	 Med_ShowMedia (&Question->Media,
			"Tst_MED_SHOW_CONT",
			"Tst_MED_SHOW");

	 /* Answers */
	 TstPrn_WriteAnswersToFill (PrintedQuestion,QstInd,Question);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Write answers of a question to fill them ******************/
/*****************************************************************************/

static void TstPrn_WriteAnswersToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                       unsigned QstInd,
                                       struct Qst_Question *Question)
  {
   void (*TstPrn_WriteAnsBank[Qst_NUM_ANS_TYPES]) (const struct TstPrn_PrintedQuestion *PrintedQuestion,
						   unsigned QstInd,
						   struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = TstPrn_WriteIntAnsToFill,
     [Qst_ANS_FLOAT          ] = TstPrn_WriteFltAnsToFill,
     [Qst_ANS_TRUE_FALSE     ] = TstPrn_WriteTF_AnsToFill,
     [Qst_ANS_UNIQUE_CHOICE  ] = TstPrn_WriteChoAnsToFill,
     [Qst_ANS_MULTIPLE_CHOICE] = TstPrn_WriteChoAnsToFill,
     [Qst_ANS_TEXT           ] = TstPrn_WriteTxtAnsToFill,
    };

   /***** Write answers *****/
   TstPrn_WriteAnsBank[Question->Answer.Type] (PrintedQuestion,QstInd,Question);
  }

/*****************************************************************************/
/****************** Write integer answer when seeing a test ******************/
/*****************************************************************************/

static void TstPrn_WriteIntAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
   HTM_INPUT_TEXT (StrAns,11,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"11\" class=\"INPUT_%s\"",
		   The_GetSuffix ());
  }

/*****************************************************************************/
/****************** Write float answer when seeing a test ********************/
/*****************************************************************************/

static void TstPrn_WriteFltAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
   HTM_INPUT_TEXT (StrAns,Qst_MAX_BYTES_FLOAT_ANSWER,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"11\" class=\"INPUT_%s\"",
		   The_GetSuffix ());
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test ****************/
/*****************************************************************************/

static void TstPrn_WriteTF_AnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   extern const char *Txt_TF_QST[2];

   /***** Write selector for the answer *****/
   /* Initially user has not answered the question ==> initially all answers will be blank.
      If the user does not confirm the submission of their exam ==>
      ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
		     "name=\"Ans%010u\" class=\"INPUT_%s\"",
		     QstInd,The_GetSuffix ());
      HTM_OPTION (HTM_Type_STRING,"" ,
                  PrintedQuestion->StrAnswers[0] == '\0' ? HTM_OPTION_SELECTED :
	                				   HTM_OPTION_UNSELECTED,
                  HTM_OPTION_ENABLED,
                  "&nbsp;");
      HTM_OPTION (HTM_Type_STRING,"T",
                  PrintedQuestion->StrAnswers[0] == 'T' ? HTM_OPTION_SELECTED :
	                				  HTM_OPTION_UNSELECTED,
                  HTM_OPTION_ENABLED,
                  "%s",Txt_TF_QST[0]);
      HTM_OPTION (HTM_Type_STRING,"F",
                  PrintedQuestion->StrAnswers[0] == 'F' ? HTM_OPTION_SELECTED :
	                				  HTM_OPTION_UNSELECTED,
                  HTM_OPTION_ENABLED,
                  "%s",Txt_TF_QST[1]);
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/******** Write single or multiple choice answer when seeing a test **********/
/*****************************************************************************/

static void TstPrn_WriteChoAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      struct Qst_Question *Question)
  {
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (PrintedQuestion->StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (PrintedQuestion->StrAnswers,UsrAnswers);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 /***** Indexes are 0 1 2 3... if no shuffle
		or 3 1 0 2... (example) if shuffle *****/
	 HTM_TR_Begin (NULL);

	    /***** Write selectors and letter of this option *****/
	    /* Initially user has not answered the question ==> initially all answers will be blank.
	       If the user does not confirm the submission of their exam ==>
	       ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
	    HTM_TD_Begin ("class=\"LT\"");

	       snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
	       if (Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE)
		  HTM_INPUT_RADIO (StrAns,HTM_DONT_SUBMIT_ON_CLICK,
				   "id=\"Ans%010u_%u\" value=\"%u\"%s"
				   " onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u);\"",
				   QstInd,NumOpt,
				   Indexes[NumOpt],
				   UsrAnswers[Indexes[NumOpt]] ? " checked=\"checked\"" :
								    "",
				   QstInd,Question->Answer.NumOptions);
	       else // Answer.Type == Tst_ANS_MULTIPLE_CHOICE
		  HTM_INPUT_CHECKBOX (StrAns,HTM_DONT_SUBMIT_ON_CHANGE,
				      "id=\"Ans%010u_%u\" value=\"%u\"%s",
				      QstInd,NumOpt,
				      Indexes[NumOpt],
				      UsrAnswers[Indexes[NumOpt]] ? " checked=\"checked\"" :
								    "");

	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"Qst_TXT_%s\"",
	                        QstInd,NumOpt,The_GetSuffix ());
		  HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    /***** Write the option text *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"Qst_TXT_%s\"",
	                        QstInd,NumOpt,The_GetSuffix ());
		  HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT",
			      "Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write text answer when seeing a test *******************/
/*****************************************************************************/

static void TstPrn_WriteTxtAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned QstInd,
                                      __attribute__((unused)) struct Qst_Question *Question)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
   HTM_INPUT_TEXT (StrAns,Qst_MAX_CHARS_ANSWERS_ONE_QST,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"40\" class=\"INPUT_%s\"",
		   The_GetSuffix ());
  }

/*****************************************************************************/
/**************** Put checkbox to allow teachers to see test *****************/
/*****************************************************************************/

static void TstPrn_PutCheckBoxAllowTeachers (bool AllowTeachers)
  {
   extern const char *Txt_Allow_teachers_to_consult_this_test;

   /***** Test exam will be available for teachers? *****/
   HTM_DIV_Begin ("class=\"CM\"");
      HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	 HTM_INPUT_CHECKBOX ("AllowTchs",HTM_DONT_SUBMIT_ON_CHANGE,
			     "value=\"Y\"%s",
			     AllowTeachers ? " checked=\"checked\"" :	// Teachers can see test exam
					     "");
	 HTM_TxtF ("&nbsp;%s",Txt_Allow_teachers_to_consult_this_test);
      HTM_LABEL_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************ Show test after assessing it ***********************/
/*****************************************************************************/

void TstPrn_ShowPrintAfterAssess (struct TstPrn_Print *Print)
  {
   unsigned QstInd;
   struct Qst_Question Question;
   bool QuestionExists;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

      /***** Initialize score and number of questions not blank *****/
      Print->NumQsts.NotBlank = 0;
      Print->Score = 0.0;

      for (QstInd = 0, The_ResetRowColor ();
	   QstInd < Print->NumQsts.All;
	   QstInd++, The_ChangeRowColor ())
	{
	 /***** Create test question *****/
	 Qst_QstConstructor (&Question);
	 Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;

	 /***** Get question data *****/
	 QuestionExists = Qst_GetQstDataByCod (&Question);

	 /***** Write question and answers *****/
	 TstPrn_WriteQstAndAnsExam (&Gbl.Usrs.Me.UsrDat,
				    Print->PrintedQuestions,QstInd,
				    Print->TimeUTC,
				    &Question,QuestionExists,
				    TstCfg_GetConfigVisibility ());

	 /***** Store test question in database *****/
	 Tst_DB_StoreOneQstOfPrint (Print,QstInd);

	 /***** Compute total score *****/
	 Print->Score += Print->PrintedQuestions[QstInd].Score;
	 if (Print->PrintedQuestions[QstInd].StrAnswers[0])	// User's answer is not blank
	    Print->NumQsts.NotBlank++;

	 /***** Update the number of accesses and the score of this question *****/
	 if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	    Qst_DB_UpdateQstScore (Print->PrintedQuestions[QstInd].QstCod,
	                           Print->PrintedQuestions[QstInd].StrAnswers[0] != '\0',
	                           Print->PrintedQuestions[QstInd].Score);

	 /***** Destroy test question *****/
	 Qst_QstDestructor (&Question);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void TstPrn_WriteQstAndAnsExam (struct Usr_Data *UsrDat,
				       struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST],
				       unsigned QstInd,
				       time_t TimeUTC[Dat_NUM_START_END_TIME],
				       struct Qst_Question *Question,
				       bool QuestionExists,
				       unsigned Visibility)
  {
   extern const char *Txt_Score;
   extern const char *Txt_Question_removed;
   extern const char *Txt_Question_modified;
   bool QuestionUneditedAfterExam = false;
   bool ICanView[TstVis_NUM_ITEMS_VISIBILITY];

   /***** Check if I can view each part of the question *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] = TstVis_IsVisibleQstAndAnsTxt (Visibility);
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] = TstVis_IsVisibleFeedbackTxt  (Visibility);
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] = TstVis_IsVisibleCorrectAns   (Visibility);
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = TstVis_IsVisibleEachQstScore (Visibility);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] =
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] =
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] =
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = true;
	 break;
      default:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] =
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] =
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] =
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = false;
	 break;
     }

   /***** If this question has been edited later than test time
	  ==> don't show question ****/
   if (QuestionExists)
      QuestionUneditedAfterExam = (Question->EditTime < TimeUTC[Dat_STR_TIME]);
   else
      QuestionUneditedAfterExam = false;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Number of question and answer type *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 if (QuestionUneditedAfterExam)
	    Qst_WriteAnswerType (Question->Answer.Type,"DAT_SMALL");
      HTM_TD_End ();

      /***** Stem, media and answers *****/
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	 if (QuestionExists)
	   {
	    if (QuestionUneditedAfterExam)
	      {
	       /* Stem */
	       Qst_WriteQstStem (Question->Stem,"Qst_TXT",
	                         ICanView[TstVis_VISIBLE_QST_ANS_TXT]);

	       /* Media */
	       if (ICanView[TstVis_VISIBLE_QST_ANS_TXT])
		  Med_ShowMedia (&Question->Media,
				 "Tst_MED_SHOW_CONT",
				 "Tst_MED_SHOW");

	       /* Answers */
	       TstPrn_ComputeAnswerScore (&PrintedQuestions[QstInd],Question);
	       TstPrn_WriteAnswersExam (UsrDat,&PrintedQuestions[QstInd],Question,
					ICanView,"Qst_TXT","Qst_TXT_LIGHT");
	      }
	    else
	       Ale_ShowAlert (Ale_WARNING,Txt_Question_modified);
	   }
	 else
	    Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);

	 /* Write score retrieved from database */
	 if (ICanView[TstVis_VISIBLE_EACH_QST_SCORE])
	   {
	    HTM_DIV_Begin ("class=\"LM DAT_SMALL_%s\"",
	                   The_GetSuffix ());
	       HTM_TxtColonNBSP (Txt_Score);
	       HTM_SPAN_Begin ("class=\"%s_%s\"",
			       PrintedQuestions[QstInd].StrAnswers[0] ?
			       (PrintedQuestions[QstInd].Score > 0 ? "Qst_ANS_OK" :	// Correct
								     "Qst_ANS_BAD") :	// Wrong
								     "Qst_ANS_0",	// Blank answer
			       The_GetSuffix ());
		  HTM_Double2Decimals (PrintedQuestions[QstInd].Score);
	       HTM_SPAN_End ();
	    HTM_DIV_End ();
	   }

	 /* Question feedback */
	 if (QuestionUneditedAfterExam)
	    if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
	       Qst_WriteQstFeedback (Question->Feedback,"Qst_TXT_LIGHT");

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******** Get questions and answers from form to assess a test print *********/
/*****************************************************************************/

void TstPrn_GetAnswersFromForm (struct TstPrn_Print *Print)
  {
   unsigned QstInd;
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Loop for every question getting user's answers *****/
   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      /* Get answers selected by user for this question */
      snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
      Par_GetParMultiToText (StrAns,Print->PrintedQuestions[QstInd].StrAnswers,
                             Qst_MAX_BYTES_ANSWERS_ONE_QST);  /* If answer type == T/F ==> " ", "T", "F"; if choice ==> "0", "2",... */
      Par_ReplaceSeparatorMultipleByComma (Print->PrintedQuestions[QstInd].StrAnswers);
     }
  }

/*****************************************************************************/
/*********** Compute score of each question and store in database ************/
/*****************************************************************************/

void TstPrn_ComputeScoresAndStoreQuestionsOfPrint (struct TstPrn_Print *Print,
                                                   bool UpdateQstScore)
  {
   unsigned QstInd;
   struct Qst_Question Question;

   /***** Initialize total score *****/
   Print->Score = 0.0;
   Print->NumQsts.NotBlank = 0;

   /***** Compute and store scores of all questions *****/
   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      /* Compute question score */
      Qst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;
      Question.Answer.Type = Qst_DB_GetQstAnswerType (Question.QstCod);
      TstPrn_ComputeAnswerScore (&Print->PrintedQuestions[QstInd],&Question);
      Qst_QstDestructor (&Question);

      /* Store test question in database */
      Tst_DB_StoreOneQstOfPrint (Print,
				 QstInd);	// 0, 1, 2, 3...

      /* Accumulate total score */
      Print->Score += Print->PrintedQuestions[QstInd].Score;
      if (Print->PrintedQuestions[QstInd].StrAnswers[0])	// User's answer is not blank
	 Print->NumQsts.NotBlank++;

      /* Update the number of hits and the score of this question in tests database */
      if (UpdateQstScore)
	 Qst_DB_UpdateQstScore (Print->PrintedQuestions[QstInd].QstCod,
				Print->PrintedQuestions[QstInd].StrAnswers[0] != '\0',
				Print->PrintedQuestions[QstInd].Score);
     }
  }

/*****************************************************************************/
/******************* Get correct answer and compute score ********************/
/*****************************************************************************/

void TstPrn_ComputeAnswerScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				struct Qst_Question *Question)
  {
   void (*TstPrn_GetCorrectAndComputeAnsScore[Qst_NUM_ANS_TYPES]) (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                                   struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = TstPrn_GetCorrectAndComputeIntAnsScore,
     [Qst_ANS_FLOAT          ] = TstPrn_GetCorrectAndComputeFltAnsScore,
     [Qst_ANS_TRUE_FALSE     ] = TstPrn_GetCorrectAndComputeTF_AnsScore,
     [Qst_ANS_UNIQUE_CHOICE  ] = TstPrn_GetCorrectAndComputeChoAnsScore,
     [Qst_ANS_MULTIPLE_CHOICE] = TstPrn_GetCorrectAndComputeChoAnsScore,
     [Qst_ANS_TEXT           ] = TstPrn_GetCorrectAndComputeTxtAnsScore,
    };

   /***** Get correct answer and compute answer score depending on type *****/
   TstPrn_GetCorrectAndComputeAnsScore[Question->Answer.Type] (PrintedQuestion,Question);
  }

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

static void TstPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get the numerical value of the correct answer,
          and compute score *****/
   Qst_GetCorrectIntAnswerFromDB (Question);
   TstPrn_ComputeIntAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get the numerical value of the minimum and maximum correct answers,
          and compute score *****/
   Qst_GetCorrectFltAnswerFromDB (Question);
   TstPrn_ComputeFltAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get answer true or false,
          and compute score *****/
   Qst_GetCorrectTF_AnswerFromDB (Question);
   TstPrn_ComputeTF_AnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get correct options of test question from database,
          and compute score *****/
   Qst_GetCorrectChoAnswerFromDB (Question);
   TstPrn_ComputeChoAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Qst_Question *Question)
  {
   /***** Get correct text answers for this question from database,
          and compute score *****/
   Qst_GetCorrectTxtAnswerFromDB (Question);
   TstPrn_ComputeTxtAnsScore (PrintedQuestion,Question);
  }

/*****************************************************************************/
/************** Compute answer score for each type of answer *****************/
/*****************************************************************************/

void TstPrn_ComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
		                const struct Qst_Question *Question)
  {
   long AnswerUsr;

   PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_BLANK;
   PrintedQuestion->Score = 0.0;	// Default score for blank or wrong answer

   if (PrintedQuestion->StrAnswers[0])	// If user has answered the answer
     {
      PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_ZERO;
      if (sscanf (PrintedQuestion->StrAnswers,"%ld",&AnswerUsr) == 1)
	 if (AnswerUsr == Question->Answer.Integer)	// Correct answer
	   {
	    PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_CORRECT;
	    PrintedQuestion->Score = 1.0;
	   }
     }
  }

void TstPrn_ComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				const struct Qst_Question *Question)
  {
   double AnswerUsr;

   PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_BLANK;
   PrintedQuestion->Score = 0.0;	// Default score for blank or wrong answer

   if (PrintedQuestion->StrAnswers[0])	// If user has answered the answer
     {
      PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_ZERO;
      AnswerUsr = Str_GetDoubleFromStr (PrintedQuestion->StrAnswers);

      // A bad formatted floating point answer will interpreted as 0.0
      if (AnswerUsr >= Question->Answer.FloatingPoint[0] &&
	  AnswerUsr <= Question->Answer.FloatingPoint[1])
	{
	 PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_CORRECT;
         PrintedQuestion->Score = 1.0; // Correct (inside the interval)
	}
     }
  }

void TstPrn_ComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
			        const struct Qst_Question *Question)
  {
   PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_BLANK;
   PrintedQuestion->Score = 0.0;

   if (PrintedQuestion->StrAnswers[0])	// If user has selected T or F
     {
      if (PrintedQuestion->StrAnswers[0] == Question->Answer.TF)
	{
 	 PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_CORRECT;
         PrintedQuestion->Score = 1.0;	// Correct
	}
      else
	{
 	 PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_NEGATIVE;
         PrintedQuestion->Score = -1.0;	// Wrong
	}
     }
  }

void TstPrn_ComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
	                        const struct Qst_Question *Question)
  {
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   unsigned NumOpt;
   unsigned NumOptTotInQst = 0;
   unsigned NumOptCorrInQst = 0;
   unsigned NumAnsGood = 0;
   unsigned NumAnsBad = 0;

   PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_BLANK;
   PrintedQuestion->Score = 0.0;

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (PrintedQuestion->StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (PrintedQuestion->StrAnswers,UsrAnswers);

   /***** Compute the total score of this question *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      NumOptTotInQst++;
      if (Question->Answer.Options[Indexes[NumOpt]].Correct)
         NumOptCorrInQst++;

      if (UsrAnswers[Indexes[NumOpt]])	// This answer has been selected by the user
        {
         if (Question->Answer.Options[Indexes[NumOpt]].Correct)
            NumAnsGood++;
         else
            NumAnsBad++;
        }
     }

   /* The answer is not blank? */
   if (NumAnsGood || NumAnsBad)	// If user has answered the answer
     {
      /* Compute the score */
      if (Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE)
        {
         if (NumOptTotInQst >= 2)	// It should be 2 options at least
           {
            if (NumAnsGood == 1 && NumAnsBad == 0)
              {
               PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_CORRECT;
               PrintedQuestion->Score = 1;
              }
            else if (NumAnsGood == 0 && NumAnsBad == 1)
              {
               PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_NEGATIVE;
               PrintedQuestion->Score = -1.0 / (double) (NumOptTotInQst - 1);
              }
            // other case should be impossible
           }
         // other case should be impossible
        }
      else	// AnswerType == Tst_ANS_MULTIPLE_CHOICE
        {
         if (NumOptCorrInQst)	// There are correct options in the question
           {
            if (NumAnsGood == NumOptCorrInQst && NumAnsBad == 0)
              {
	       PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_CORRECT;
	       PrintedQuestion->Score = 1.0;
              }
            else
              {
	       if (NumOptCorrInQst < NumOptTotInQst)	// If there are correct options and wrong options (typical case)
		 {
		  PrintedQuestion->Score = (double) NumAnsGood / (double) NumOptCorrInQst -
					   (double) NumAnsBad  / (double) (NumOptTotInQst - NumOptCorrInQst);
		  if (PrintedQuestion->Score > 0.000001)
		     PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_POSITIVE;
		  else if (PrintedQuestion->Score < -0.000001)
		     PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_NEGATIVE;
		  else	// Score is 0
		     PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_ZERO;
		 }
	       else					// If all options are correct (extrange case)
		 {
		  if (NumAnsGood == 0)
		    {
		     PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_ZERO;
		     PrintedQuestion->Score = 0.0;
		    }
		  else
		    {
		     PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_POSITIVE;
		     PrintedQuestion->Score = (double) NumAnsGood / (double) NumOptCorrInQst;
		    }
		 }
              }
           }
         // other case should be impossible
        }
     }
  }

void TstPrn_ComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				 const struct Qst_Question *Question)
  {
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];

   PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_BLANK;
   PrintedQuestion->Score = 0.0;	// Default score for blank or wrong answer

   if (PrintedQuestion->StrAnswers[0])	// If user has answered the answer
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,PrintedQuestion->StrAnswers,sizeof (TextAnsUsr) - 1);

      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);
      Str_ConvertToComparable (TextAnsUsr);

      PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_WRONG_ZERO;
      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
        {
         /* Filter this correct answer */
         Str_Copy (TextAnsOK,Question->Answer.Options[NumOpt].Text,sizeof (TextAnsOK) - 1);
         Str_ConvertToComparable (TextAnsOK);

         /* Check is user answer is correct */
         if (!strcoll (TextAnsUsr,TextAnsOK))
           {
            PrintedQuestion->AnswerIsCorrect = TstPrn_ANSWER_IS_CORRECT;
	    PrintedQuestion->Score = 1.0;	// Correct answer
	    break;
           }
        }
     }
  }

/*****************************************************************************/
/********** Get vector of unsigned indexes from string with indexes **********/
/*****************************************************************************/

void TstPrn_GetIndexesFromStr (const char StrIndexesOneQst[Qst_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			       unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION])
  {
   unsigned NumOpt;
   const char *Ptr;
   char StrOneIndex[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Get indexes from string *****/
   for (NumOpt = 0, Ptr = StrIndexesOneQst;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION && *Ptr;
	NumOpt++)
     {
      Par_GetNextStrUntilComma (&Ptr,StrOneIndex,Cns_MAX_DECIMAL_DIGITS_UINT);

      if (sscanf (StrOneIndex,"%u",&(Indexes[NumOpt])) != 1)
	 Err_WrongAnswerIndexExit ();

      if (Indexes[NumOpt] >= Qst_MAX_OPTIONS_PER_QUESTION)
	 Err_WrongAnswerIndexExit ();
     }

   /***** Initialize remaining to 0 *****/
   for (;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Indexes[NumOpt] = 0;
  }

/*****************************************************************************/
/************ Get vector of bool answers from string with answers ************/
/*****************************************************************************/

void TstPrn_GetAnswersFromStr (const char StrAnswersOneQst[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1],
			       bool UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION])
  {
   unsigned NumOpt;
   const char *Ptr;
   char StrOneAnswer[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned AnsUsr;

   /***** Initialize all answers to false *****/
   for (NumOpt = 0;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      UsrAnswers[NumOpt] = false;

   /***** Set selected answers to true *****/
   for (NumOpt = 0, Ptr = StrAnswersOneQst;
	NumOpt < Qst_MAX_OPTIONS_PER_QUESTION && *Ptr;
	NumOpt++)
     {
      Par_GetNextStrUntilComma (&Ptr,StrOneAnswer,Cns_MAX_DECIMAL_DIGITS_UINT);

      if (sscanf (StrOneAnswer,"%u",&AnsUsr) != 1)
	 Err_WrongAnswerExit ();

      if (AnsUsr >= Qst_MAX_OPTIONS_PER_QUESTION)
	 Err_WrongAnswerExit ();

      UsrAnswers[AnsUsr] = true;
     }
  }

/*****************************************************************************/
/************ Compute and show total grade out of maximum grade **************/
/*****************************************************************************/

void TstPrn_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade)
  {
   TstPrn_ShowGrade (TstPrn_ComputeGrade (NumQsts,Score,MaxGrade),MaxGrade);
  }

/*****************************************************************************/
/**************** Compute total grade out of maximum grade *******************/
/*****************************************************************************/

double TstPrn_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade)
  {
   double MaxScore;
   double Grade;

   /***** Compute grade *****/
   if (NumQsts)
     {
      MaxScore = (double) NumQsts;
      Grade = Score * MaxGrade / MaxScore;
     }
   else
      Grade = 0.0;

   return Grade;
  }

/*****************************************************************************/
/****************** Show total grade out of maximum grade ********************/
/*****************************************************************************/

void TstPrn_ShowGrade (double Grade,double MaxGrade)
  {
   /***** Write grade over maximum grade *****/
   HTM_Double2Decimals (Grade);
   HTM_Txt ("/");
   HTM_Double2Decimals (MaxGrade);
  }

/*****************************************************************************/
/************* Write answers of a question when assessing a test *************/
/*****************************************************************************/

void TstPrn_WriteAnswersExam (struct Usr_Data *UsrDat,
                              const struct TstPrn_PrintedQuestion *PrintedQuestion,
			      struct Qst_Question *Question,
			      bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			      const char *ClassTxt,
			      const char *ClassFeedback)
  {
   void (*TstPrn_WriteAnsExam[Qst_NUM_ANS_TYPES]) (struct Usr_Data *UsrDat,
                                                   const struct TstPrn_PrintedQuestion *PrintedQuestion,
				                   struct Qst_Question *Question,
				                   bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				                   const char *ClassTxt,
				                   const char *ClassFeedback) =
    {
     [Qst_ANS_INT            ] = TstPrn_WriteIntAnsPrint,
     [Qst_ANS_FLOAT          ] = TstPrn_WriteFltAnsPrint,
     [Qst_ANS_TRUE_FALSE     ] = TstPrn_WriteTF_AnsPrint,
     [Qst_ANS_UNIQUE_CHOICE  ] = TstPrn_WriteChoAnsPrint,
     [Qst_ANS_MULTIPLE_CHOICE] = TstPrn_WriteChoAnsPrint,
     [Qst_ANS_TEXT           ] = TstPrn_WriteTxtAnsPrint,
    };

   /***** Get correct answer and compute answer score depending on type *****/
   TstPrn_WriteAnsExam[Question->Answer.Type] (UsrDat,PrintedQuestion,Question,
	                                       ICanView,ClassTxt,ClassFeedback);
  }

/*****************************************************************************/
/******************* Write integer answer in a test print ********************/
/*****************************************************************************/

static void TstPrn_WriteIntAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   long IntAnswerUsr;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 TstPrn_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 if (PrintedQuestion->StrAnswers[0])		// If user has answered the question
	   {
	    if (sscanf (PrintedQuestion->StrAnswers,"%ld",&IntAnswerUsr) == 1)
	      {
	       HTM_TD_Begin ("class=\"CM %s_%s\"",
			     ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ?
				(IntAnswerUsr == Question->Answer.Integer ? "Qst_ANS_OK" :	// Correct
									    "Qst_ANS_BAD") :	// Wrong
									    "Qst_ANS_0",	// Blank answer
			     The_GetSuffix ());
		  HTM_Long (IntAnswerUsr);
	       HTM_TD_End ();
	      }
	    else
	      {
	       HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",
	                     The_GetSuffix ());
		  HTM_Txt ("?");
	       HTM_TD_End ();
	      }
	   }
	 else							// If user has omitted the answer
	    HTM_TD_Empty (1);

	 /***** Write the correct answer *****/
	 HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",The_GetSuffix ());
	    if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	       HTM_Long (Question->Answer.Integer);
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write float answer in an test print ********************/
/*****************************************************************************/

static void TstPrn_WriteFltAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   double FloatAnsUsr = 0.0;

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 TstPrn_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 if (PrintedQuestion->StrAnswers[0])	// If user has answered the question
	   {
	    FloatAnsUsr = Str_GetDoubleFromStr (PrintedQuestion->StrAnswers);
	    // A bad formatted floating point answer will interpreted as 0.0
	    HTM_TD_Begin ("class=\"CM %s_%s\"",
			  ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ?
			     ((FloatAnsUsr >= Question->Answer.FloatingPoint[0] &&
			       FloatAnsUsr <= Question->Answer.FloatingPoint[1]) ? "Qst_ANS_OK" :	// Correct
										   "Qst_ANS_BAD") :	// Wrong
										   "Qst_ANS_0",		// Blank answer
			  The_GetSuffix ());
	       HTM_Double (FloatAnsUsr);
	    HTM_TD_End ();
	   }
	 else					// If user has omitted the answer
	    HTM_TD_Empty (1);

	 /***** Write the correct answer *****/
	 HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",The_GetSuffix ());
	    if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	      {
	       HTM_Txt ("[");
	       HTM_Double (Question->Answer.FloatingPoint[0]);
	       HTM_Txt ("; ");
	       HTM_Double (Question->Answer.FloatingPoint[1]);
	       HTM_Txt ("]");
	      }
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Write false / true answer in a test print *****************/
/*****************************************************************************/

static void TstPrn_WriteTF_AnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   char AnsTFUsr;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   AnsTFUsr = PrintedQuestion->StrAnswers[0];

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 TstPrn_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 HTM_TD_Begin ("class=\"CM %s_%s\"",
		       ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ?
			  (AnsTFUsr == Question->Answer.TF ? "Qst_ANS_OK" :	// Correct
							     "Qst_ANS_BAD") :	// Wrong
							     "Qst_ANS_0",	// Blank answer
		       The_GetSuffix ());
	    Qst_WriteAnsTF (AnsTFUsr);
	 HTM_TD_End ();

	 /***** Write the correct answer *****/
	 HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",The_GetSuffix ());
	    if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	       Qst_WriteAnsTF (Question->Answer.TF);
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write single or multiple choice answer in a test print ***********/
/*****************************************************************************/

static void TstPrn_WriteChoAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     const char *ClassTxt,
				     const char *ClassFeedback)
  {
   extern const char *Txt_TST_Answer_given_by_the_user;
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   struct
     {
      char *Class;
      char *Str;
     } Ans;

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Change format of answers feedback *****/
   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
      Qst_ChangeFormatAnswersFeedback (Question);

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (PrintedQuestion->StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (PrintedQuestion->StrAnswers,UsrAnswers);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      HTM_TR_Begin (NULL);
	 TstPrn_WriteHeadUserCorrect (UsrDat);
	 HTM_TD_Empty (2);
      HTM_TR_End ();

      /***** Write answers (one row per answer) *****/
      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 HTM_TR_Begin (NULL);

	    /* Draw icon depending on user's answer */
	    if (UsrAnswers[Indexes[NumOpt]] == true)	// This answer has been selected by the user
	      {
	       if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
		 {
		  if (Question->Answer.Options[Indexes[NumOpt]].Correct)
		    {
		     Ans.Class = "Qst_ANS_OK";	// Correct
		     Ans.Str   = "&check;";
		    }
		  else
		    {
		     Ans.Class = "Qst_ANS_BAD";	// Wrong
		     Ans.Str   = "&cross;";
		    }
		 }
	       else
		 {
		  Ans.Class = "Qst_ANS_0";		// Blank answer
		  Ans.Str   = "&bull;";
		 }

	       HTM_TD_Begin ("class=\"CT %s_%s\" title=\"%s\"",
			     Ans.Class,The_GetSuffix (),
			     Txt_TST_Answer_given_by_the_user);
		  HTM_Txt (Ans.Str);
	       HTM_TD_End ();
	      }
	    else	// This answer has NOT been selected by the user
	       HTM_TD_Empty (1);

	    /* Draw icon that indicates whether the answer is correct */
	    if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	      {
	       if (Question->Answer.Options[Indexes[NumOpt]].Correct)
		 {
		  HTM_TD_Begin ("class=\"CT Qst_ANS_0_%s\" title=\"%s\"",
		                The_GetSuffix (),
				Txt_TST_Answer_given_by_the_teachers);
		     HTM_Txt ("&bull;");
		  HTM_TD_End ();
		 }
	       else
		  HTM_TD_Empty (1);
	      }
	    else
	      {
	       HTM_TD_Begin ("class=\"CT Qst_ANS_0_%s\"",
	                     The_GetSuffix ());
		  Ico_PutIconNotVisible ();
	       HTM_TD_End ();
	      }

	    /* Answer letter (a, b, c,...) */
	    HTM_TD_Begin ("class=\"LT %s_%s\"",
	                  ClassTxt,The_GetSuffix ());
	       HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
	    HTM_TD_End ();

	    /* Answer text and feedback */
	    HTM_TD_Begin ("class=\"LT\"");

	       HTM_DIV_Begin ("class=\"%s_%s\"",
	                      ClassTxt,The_GetSuffix ());
		  if (ICanView[TstVis_VISIBLE_QST_ANS_TXT])
		    {
		     HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
		     Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
				    "Tst_MED_SHOW_CONT",
				    "Tst_MED_SHOW");
		    }
		  else
		     Ico_PutIconNotVisible ();
	       HTM_DIV_End ();

	       if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
		  if (Question->Answer.Options[Indexes[NumOpt]].Feedback)
		     if (Question->Answer.Options[Indexes[NumOpt]].Feedback[0])
		       {
			HTM_DIV_Begin ("class=\"%s_%s\"",
			               ClassFeedback,The_GetSuffix ());
			   HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Feedback);
			HTM_DIV_End ();
		       }

	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************** Write text answer when assessing a test print ****************/
/*****************************************************************************/

static void TstPrn_WriteTxtAnsPrint (struct Usr_Data *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Qst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   bool Correct = false;

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Change format of answers feedback *****/
   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
      Qst_ChangeFormatAnswersFeedback (Question);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 TstPrn_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 if (PrintedQuestion->StrAnswers[0])	// If user has answered the question
	   {
	    /* Filter the user answer */
	    Str_Copy (TextAnsUsr,PrintedQuestion->StrAnswers,sizeof (TextAnsUsr) - 1);

	    /* In order to compare student answer to stored answer,
	       the text answers are stored avoiding two or more consecurive spaces */
	    Str_ReplaceSeveralSpacesForOne (TextAnsUsr);

	    Str_ConvertToComparable (TextAnsUsr);

	    for (NumOpt = 0;
		 NumOpt < Question->Answer.NumOptions;
		 NumOpt++)
	      {
	       /* Filter this correct answer */
	       Str_Copy (TextAnsOK,Question->Answer.Options[NumOpt].Text,sizeof (TextAnsOK) - 1);
	       Str_ConvertToComparable (TextAnsOK);

	       /* Check is user answer is correct */
	       if (!strcoll (TextAnsUsr,TextAnsOK))
		 {
		  Correct = true;
		  break;
		 }
	      }
	    HTM_TD_Begin ("class=\"CT %s_%s\"",
			  ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ? (Correct ? "Qst_ANS_OK" :	// Correct
							                       "Qst_ANS_BAD") :	// Wrong
									       "Qst_ANS_0",	// Blank answer
			  The_GetSuffix ());
	       HTM_Txt (PrintedQuestion->StrAnswers);
	    HTM_TD_End ();
	   }
	 else						// If user has omitted the answer
            HTM_TD_Empty (1);

	 /***** Write the correct answers *****/
	 if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	   {
	    HTM_TD_Begin ("class=\"CT\"");
	       HTM_TABLE_BeginPadding (2);

		  for (NumOpt = 0;
		       NumOpt < Question->Answer.NumOptions;
		       NumOpt++)
		    {
		     HTM_TR_Begin (NULL);

			/* Answer letter (a, b, c,...) */
			HTM_TD_Begin ("class=\"LT Qst_ANS_0_%s\"",
			              The_GetSuffix ());
			   HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
			HTM_TD_End ();

			/* Answer text and feedback */
			HTM_TD_Begin ("class=\"LT\"");

			   HTM_DIV_Begin ("class=\"Qst_ANS_0_%s\"",
			                  The_GetSuffix ());
			      HTM_Txt (Question->Answer.Options[NumOpt].Text);
			   HTM_DIV_End ();

			   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
			      if (Question->Answer.Options[NumOpt].Feedback)
				 if (Question->Answer.Options[NumOpt].Feedback[0])
				   {
				    HTM_DIV_Begin ("class=\"Qst_TXT_LIGHT\"");
				       HTM_Txt (Question->Answer.Options[NumOpt].Feedback);
				    HTM_DIV_End ();
				   }

			HTM_TD_End ();

		     HTM_TR_End ();
		    }

	       HTM_TABLE_End ();
	    HTM_TD_End ();
	   }
	 else
	   {
	    HTM_TD_Begin ("class=\"CT Qst_ANS_0_%s\"",
	                  The_GetSuffix ());
	       Ico_PutIconNotVisible ();
	    HTM_TD_End ();
	   }
      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********* Write head with two columns:                               ********/
/********* one for the user's answer and other for the correct answer ********/
/*****************************************************************************/

static void TstPrn_WriteHeadUserCorrect (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   HTM_TD_Begin ("class=\"CM DAT_SMALL_%s\"",The_GetSuffix ());
      HTM_Txt (Txt_User[UsrDat->Sex]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"CM DAT_SMALL_%s\"",The_GetSuffix ());
      HTM_Txt (Txt_ROLES_PLURAL_Abc[Rol_TCH][Usr_SEX_UNKNOWN]);
   HTM_TD_End ();
  }

/*****************************************************************************/
/*************** Select users and dates to show their tests ******************/
/*****************************************************************************/

void TstPrn_SelUsrsToViewUsrsPrints (void)
  {
   TstPrn_PutFormToSelectUsrsToViewUsrsPrints (NULL);
  }

static void TstPrn_PutFormToSelectUsrsToViewUsrsPrints (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_View_results;

   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeUsrTstResCrs,
				     NULL,NULL,
				     Act_GetActionText (ActSeeUsrTstResCrs),
				     Hlp_ASSESSMENT_Tests_results,
				     Txt_View_results,
				     true);	// Put form with date range
  }

/*****************************************************************************/
/*********************** Select dates to show my tests ***********************/
/*****************************************************************************/

void TstPrn_SelDatesToSeeMyPrints (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_results;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Begin form *****/
   Frm_BeginForm (ActSeeMyTstResCrs);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Results,
			 NULL,NULL,
			 Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);

	 Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_View_results);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************************* Show my tests *******************************/
/*****************************************************************************/

void TstPrn_ShowMyPrints (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Results,
                      NULL,NULL,
                      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);

      /***** Header of the table with the list of users *****/
      TstPrn_ShowHeaderPrints (Usr_ME);

      /***** List my tests *****/
      TstCfg_GetConfig ();	// To get visibility
      TstPrn_ShowUsrPrints (&Gbl.Usrs.Me.UsrDat);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/********************** Get users and show their test ************************/
/*****************************************************************************/

void TstPrn_GetUsrsAndShowPrints (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  TstPrn_ShowUsrsPrints,NULL,
                                  TstPrn_PutFormToSelectUsrsToViewUsrsPrints,NULL);
  }

/*****************************************************************************/
/********************* Show test prints for several users ********************/
/*****************************************************************************/

static void TstPrn_ShowUsrsPrints (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   const char *Ptr;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Results,
                      NULL,NULL,
		      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,5);

      /***** Header of the table with the list of users *****/
      TstPrn_ShowHeaderPrints (Usr_OTHER);

      /***** List the tests of the selected users *****/
      Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					    Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	 Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
						      Usr_DONT_GET_PREFS,
						      Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	    if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	      {
	       /***** Show tests *****/
	       Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	       TstPrn_ShowUsrPrints (&Gbl.Usrs.Other.UsrDat);
	      }
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************** Show header of my tests **************************/
/*****************************************************************************/

static void TstPrn_ShowHeaderPrints (Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_ANSWERS_non_blank;
   extern const char *Txt_ANSWERS_blank;
   extern const char *Txt_total;
   extern const char *Txt_average;

   /***** First row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_User[MeOrOther == Usr_ME ?
	                       Gbl.Usrs.Me.UsrDat.Sex :
		  	       Usr_SEX_UNKNOWN         ],HTM_HEAD_CENTER,3,2,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_STR_TIME]     ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_END_TIME]     ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_Questions                        ,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span (Txt_Answers                          ,HTM_HEAD_CENTER,1,2,"LINE_LEFT");
      HTM_TH_Span (Txt_Score                            ,HTM_HEAD_CENTER,1,2,"LINE_LEFT");
      HTM_TH_Span (Txt_Grade                            ,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span (NULL                                 ,HTM_HEAD_CENTER,3,1,"LINE_BOTTOM LINE_LEFT");
   HTM_TR_End ();

   /***** Second row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_ANSWERS_non_blank                ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH      (Txt_ANSWERS_blank                    ,HTM_HEAD_RIGHT );
      HTM_TH_Span (Txt_total                            ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH      (Txt_average                          ,HTM_HEAD_RIGHT );
   HTM_TR_End ();

   /***** Third row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span ("{-1&le;<em>p<sub>i</sub></em>&le;1}",HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("{<em>p<sub>i</sub></em>=0}"         ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
      HTM_TH_Span ("<em>&Sigma;p<sub>i</sub></em>"      ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("-1&le;"
	           "<em style=\"text-decoration:overline;\">p</em>"
	           "&le;1"                              ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show the test prints of a user in the current course ***********/
/*****************************************************************************/

static void TstPrn_ShowUsrPrints (struct Usr_Data *UsrDat)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPrints;
   unsigned NumPrint;
   static unsigned UniqueId = 0;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstPrn_Print Print;
   unsigned NumQstsBlank;
   struct TstPrn_NumQuestions NumTotalQsts;
   double TotalScore;
   unsigned NumPrintsVisibleByTchs = 0;
   struct TstRes_ICanView ICanView;
   const char *ClassDat;

   /***** Reset total number of questions and total score *****/
   NumTotalQsts.All      =
   NumTotalQsts.NotBlank = 0;
   TotalScore = 0.0;

   /***** Make database query *****/
   NumPrints = Tst_DB_GetUsrPrintsInCurrentCrs (&mysql_res,UsrDat->UsrCod);

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);

      Usr_ShowTableCellWithUsrData (UsrDat,NumPrints);

      /***** Get and print tests *****/
      if (NumPrints)
	{
	 for (NumPrint = 0;
	      NumPrint < NumPrints;
	      NumPrint++)
	   {
	    /* Get print code (row[0]) */
	    if ((Print.PrnCod = DB_GetNextCode (mysql_res)) <= 0)
	       Err_WrongTestExit ();

	    /* Get print data */
	    TstPrn_GetPrintDataByPrnCod (&Print);
	    ClassDat = Print.AllowTeachers ? "DAT" :
					     "DAT_LIGHT";

	    /* Get if I can see print result and score */
	    TstRes_CheckIfICanSeePrintResult (&Print,UsrDat->UsrCod,&ICanView);

	    if (NumPrint)
	       HTM_TR_Begin (NULL);

	    /* Write dates and times */
	    UniqueId++;
	    for (StartEndTime  = (Dat_StartEndTime_t) 0;
		 StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
		 StartEndTime++)
	      {
	       if (asprintf (&Id,"tst_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			     Id,ClassDat,The_GetSuffix (),
			     The_GetColorRows ());
		  Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
						true,true,false,0x7);
	       HTM_TD_End ();
	       free (Id);
	      }

	    /* Accumulate questions and score */
	    if (ICanView.Score)
	      {
	       NumTotalQsts.All      += Print.NumQsts.All;
	       NumTotalQsts.NotBlank += Print.NumQsts.NotBlank;
	       TotalScore            += Print.Score;
	      }

	    /* Write number of questions */
	    HTM_TD_Begin ("class=\"RT %s_%s LINE_LEFT %s\"",
	                  ClassDat,The_GetSuffix (),
	                  The_GetColorRows ());
	       if (ICanView.Result)
		  HTM_Unsigned (Print.NumQsts.All);
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write number of non-blank answers */
	    HTM_TD_Begin ("class=\"RT %s_%s LINE_LEFT %s\"",
	                  ClassDat,The_GetSuffix (),
	                  The_GetColorRows ());
	       if (ICanView.Result)
		 {
		  if (Print.NumQsts.NotBlank)
		     HTM_Unsigned (Print.NumQsts.NotBlank);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write number of blank answers */
	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
	                  ClassDat,The_GetSuffix (),
	                  The_GetColorRows ());
	       if (ICanView.Result)
		 {
		  NumQstsBlank = Print.NumQsts.All - Print.NumQsts.NotBlank;
		  if (NumQstsBlank)
		     HTM_Unsigned (NumQstsBlank);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write score */
	    HTM_TD_Begin ("class=\"RT %s_%s LINE_LEFT %s\"",
	                  ClassDat,The_GetSuffix (),
	                  The_GetColorRows ());
	       if (ICanView.Score)
		 {
		  HTM_Double2Decimals (Print.Score);
		  HTM_Txt ("/");
		  HTM_Unsigned (Print.NumQsts.All);
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write average score per question */
	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
	                  ClassDat,The_GetSuffix (),
	                  The_GetColorRows ());
	       if (ICanView.Score)
		  HTM_Double2Decimals (Print.NumQsts.All ? Print.Score /
							   (double) Print.NumQsts.All :
							   0.0);
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write grade */
	    HTM_TD_Begin ("class=\"RT %s_%s LINE_LEFT %s\"",
	                  ClassDat,The_GetSuffix (),
	                  The_GetColorRows ());
	       if (ICanView.Score)
		  TstPrn_ComputeAndShowGrade (Print.NumQsts.All,Print.Score,Tst_SCORE_MAX);
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Link to show this test */
	    HTM_TD_Begin ("class=\"RT LINE_LEFT %s\"",
	                  The_GetColorRows ());
	       if (ICanView.Result)
		 {
		  Frm_BeginForm (Gbl.Action.Act == ActSeeMyTstResCrs ? ActSeeOneTstResMe :
								       ActSeeOneTstResOth);
		     ParCod_PutPar (ParCod_Prn,Print.PrnCod);
		     Ico_PutIconLink ("tasks.svg",Ico_BLACK,
		                      Gbl.Action.Act == ActSeeMyTstResCrs ? ActSeeOneTstResMe :
								            ActSeeOneTstResOth);
		  Frm_EndForm ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    HTM_TR_End ();

	    if (Print.AllowTeachers)
	       NumPrintsVisibleByTchs++;
	   }

	 /***** Write totals for this user *****/
	 TstPrn_ShowPrintsSummaryRow (Usr_ItsMe (UsrDat->UsrCod),
	                              NumPrintsVisibleByTchs,
				      &NumTotalQsts,TotalScore);
	}
      else
	{
	 /* Columns for dates */
	 HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 /* Column for questions */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 /* Columns for answers */
	 HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 /* Columns for score */
	 HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 /* Column for grade */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 /* Column for link to show the result */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/****************** Show row with summary of user's tess *********************/
/*****************************************************************************/

static void TstPrn_ShowPrintsSummaryRow (Usr_MeOrOther_t MeOrOther,
                                         unsigned NumPrints,
                                         struct TstPrn_NumQuestions *NumTotalQsts,
                                         double TotalScore)
  {
   extern const char *Txt_Visible_tests;
   bool ICanViewTotalScore;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewTotalScore = MeOrOther == Usr_ME &&
		              TstVis_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 ICanViewTotalScore = MeOrOther == Usr_ME ||
			      NumPrints;
	 break;
      case Rol_SYS_ADM:
	 ICanViewTotalScore = true;
	 break;
      default:
	 ICanViewTotalScore = false;
	 break;
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Row title *****/
      HTM_TD_Begin ("colspan=\"2\""
	            " class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 HTM_TxtColonNBSP (Txt_Visible_tests);
	 HTM_Unsigned (NumPrints);
      HTM_TD_End ();

      /***** Write total number of questions *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (NumPrints)
	    HTM_Unsigned (NumTotalQsts->All);
      HTM_TD_End ();

      /***** Write total number of non-blank answers *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (NumPrints)
	    HTM_Unsigned (NumTotalQsts->NotBlank);
      HTM_TD_End ();

      /***** Write total number of blank answers *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (NumPrints)
	    HTM_Unsigned (NumTotalQsts->All - NumTotalQsts->NotBlank);
      HTM_TD_End ();

      /***** Write total score *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (ICanViewTotalScore)
	   {
	    HTM_Double2Decimals (TotalScore);
	    HTM_Txt ("/");
	    HTM_Unsigned (NumTotalQsts->All);
	   }
      HTM_TD_End ();

      /***** Write average score per question *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (ICanViewTotalScore)
	    HTM_Double2Decimals (NumTotalQsts->All ? TotalScore / (double) NumTotalQsts->All :
						     0.0);
      HTM_TD_End ();

      /***** Write grade over Tst_SCORE_MAX *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (ICanViewTotalScore)
	    TstPrn_ComputeAndShowGrade (NumTotalQsts->All,TotalScore,Tst_SCORE_MAX);
      HTM_TD_End ();

      /***** Last cell *****/
      HTM_TD_Begin ("class=\"DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********************** Show one test of another user ***********************/
/*****************************************************************************/

void TstPrn_ShowOnePrint (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Result;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_Tags;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };
   struct TstPrn_Print Print;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstRes_ICanView ICanView;

   /***** Get the code of the test *****/
   TstPrn_ResetPrint (&Print);
   Print.PrnCod = ParCod_GetAndCheckPar (ParCod_Prn);

   /***** Get test data *****/
   TstPrn_GetPrintDataByPrnCod (&Print);

   /***** Check if I can see print result and score *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
      TstCfg_GetConfig ();	// To get visibility
   TstRes_CheckIfICanSeePrintResult (&Print,Gbl.Usrs.Other.UsrDat.UsrCod,&ICanView);
   if (!ICanView.Result)	// I am not allowed to view this test
      Err_NoPermissionExit ();

   /***** Get questions and user's answers of the test from database *****/
   if (!TstPrn_GetPrintQuestionsFromDB (&Print))
      Err_WrongExamExit ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Result,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

	 /***** User *****/
	 /* Get data of the user who made the test */
	 if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
						       Usr_DONT_GET_PREFS,
						       Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	    Err_WrongUserExit ();
	 if (!Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	    Err_NoPermissionExit ();

	 /* User */
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs][Gbl.Usrs.Other.UsrDat.Sex]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	       ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
	       HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname1);
	       if (Gbl.Usrs.Other.UsrDat.Surname2[0])
		  HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname2);
	       if (Gbl.Usrs.Other.UsrDat.FrstName[0])
		  HTM_TxtF (", %s",Gbl.Usrs.Other.UsrDat.FrstName);
	       HTM_BR ();
	       Pho_ShowUsrPhotoIfAllowed (&Gbl.Usrs.Other.UsrDat,
					  ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Start/end time (for user in this test print) *****/
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    if (asprintf (&Id,"tst_date_%u",(unsigned) StartEndTime) < 0)
	       Err_NotEnoughMemoryExit ();

	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  HTM_TxtColon (Txt_START_END_TIME[StartEndTime]);
	       HTM_TD_End ();

	       HTM_TD_Begin ("id=\"%s\" class=\"LB DAT_%s\"",
			     Id,The_GetSuffix ());
		  Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
						true,true,true,0x7);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    free (Id);
	   }

	 /***** Number of questions *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Questions);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       HTM_Unsigned (Print.NumQsts.All);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Number of answers *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Answers);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       HTM_Unsigned (Print.NumQsts.NotBlank);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Score *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Score);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       if (ICanView.Score)
		 {
		  HTM_STRONG_Begin ();
		     HTM_Double2Decimals (Print.Score);
		     HTM_Txt ("/");
		     HTM_Unsigned (Print.NumQsts.All);
		  HTM_STRONG_End ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Grade *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Grade);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       if (ICanView.Score)
		 {
		  HTM_STRONG_Begin ();
		     TstPrn_ComputeAndShowGrade (Print.NumQsts.All,Print.Score,Tst_SCORE_MAX);
		  HTM_STRONG_End ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Tags present in this test *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Tags);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       TstPrn_ShowTagsPresentInAPrint (Print.PrnCod);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Write answers and solutions *****/
	 TstPrn_ShowPrintAnswers (&Gbl.Usrs.Other.UsrDat,
				  Print.NumQsts.All,
				  Print.PrintedQuestions,
				  Print.TimeUTC,
				  TstCfg_GetConfigVisibility ());

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****************** Get if I can see print result and score ******************/
/*****************************************************************************/

static void TstRes_CheckIfICanSeePrintResult (const struct TstPrn_Print *Print,
                                              long UsrCod,
                                              struct TstRes_ICanView *ICanView)
  {
   /***** Check if I can view print result and score *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 // Depends on whether the print is sent or not
	 // if the print is not sent ==> I can not view results
	 ICanView->Result = Print->Sent && Usr_ItsMe (UsrCod) == Usr_ME;

	 if (ICanView->Result)
	    // Depends on 5 visibility icons associated to tests
	    ICanView->Score = TstVis_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	 else
	    ICanView->Score = false;
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 // Depends on whether the print is sent or not, and whether teachers are allowed
	 // if the print is not sent ==> I can not view results
	 // if teachers are not allowed ==> I can not view results (except if the print is mine)
	 ICanView->Result =
	 ICanView->Score  = Print->Sent &&
	                    (Print->AllowTeachers || Usr_ItsMe (UsrCod) == Usr_ME);
	 break;
      case Rol_SYS_ADM:
	 ICanView->Result =
	 ICanView->Score  = true;
	 break;
      default:
	 ICanView->Result =
	 ICanView->Score  = false;
	 break;
     }
  }

/*****************************************************************************/
/************************ Show test tags in this test ************************/
/*****************************************************************************/

static void TstPrn_ShowTagsPresentInAPrint (long PrnCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumTags;

   /***** Get all tags of questions in this test print *****/
   NumTags = Tst_DB_GetTagsPresentInAPrint (&mysql_res,PrnCod);
   Tag_ShowTagList (NumTags,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** Show user's and correct answers of a test ******************/
/*****************************************************************************/

void TstPrn_ShowPrintAnswers (struct Usr_Data *UsrDat,
			      unsigned NumQsts,
			      struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST],
			      time_t TimeUTC[Dat_NUM_START_END_TIME],
			      unsigned Visibility)
  {
   unsigned QstInd;
   struct Qst_Question Question;
   bool QuestionExists;

   for (QstInd = 0, The_ResetRowColor ();
	QstInd < NumQsts;
	QstInd++, The_ChangeRowColor ())
     {
      /***** Create test question *****/
      Qst_QstConstructor (&Question);
      Question.QstCod = PrintedQuestions[QstInd].QstCod;

      /***** Get question data *****/
      QuestionExists = Qst_GetQstDataByCod (&Question);

      /***** Write questions and answers *****/
      TstPrn_WriteQstAndAnsExam (UsrDat,
				 PrintedQuestions,QstInd,
				 TimeUTC,
				 &Question,QuestionExists,
				 Visibility);

      /***** Destroy test question *****/
      Qst_QstDestructor (&Question);
     }
  }

/*****************************************************************************/
/**************** Get data of a test using its test code *********************/
/*****************************************************************************/

void TstPrn_GetPrintDataByPrnCod (struct TstPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Make database query *****/
   if (Tst_DB_GetPrintDataByPrnCod (&mysql_res,Print->PrnCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user code (row[0]) */
      Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get date-time (row[1] and row[2] hold UTC date-time) */
      Print->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
      Print->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[2]);

      /* Get number of questions (row[3])
         and number of questions not blank (row[4]) */
      if (sscanf (row[3],"%u",&Print->NumQsts.All     ) != 1)
	 Print->NumQsts.All      = 0;
      if (sscanf (row[4],"%u",&Print->NumQsts.NotBlank) != 1)
	 Print->NumQsts.NotBlank = 0;

      /* Get if print has been sent (row[5])
         and if teachers are allowed to see this test print (row[6]) */
      Print->Sent          = (row[5][0] == 'Y');
      Print->AllowTeachers = (row[6][0] == 'Y');

      /* Get score (row[7]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[7],"%lf",&Print->Score) != 1)
	 Print->Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }
   else
      TstPrn_ResetPrintExceptPrnCod (Print);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get the questions of a test print from database ***************/
/*****************************************************************************/

bool TstPrn_GetPrintQuestionsFromDB (struct TstPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned QstInd;

   /***** Get questions of a test print from database *****/
   NumQsts = Tst_DB_GetPrintQuestions (&mysql_res,Print->PrnCod);

   /***** Get questions *****/
   if (NumQsts == Print->NumQsts.All)
      for (QstInd = 0;
	   QstInd < NumQsts;
	   QstInd++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get question code (row[0]) */
	 if ((Print->PrintedQuestions[QstInd].QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Err_WrongQuestionExit ();

	 /* Get score (row[1]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[1],"%lf",&Print->PrintedQuestions[QstInd].Score) != 1)
            Err_ShowErrorAndExit ("Wrong question score.");
         Str_SetDecimalPointToLocal ();	// Return to local system

	 /* Get indexes for this question (row[2])
	    and answers selected by user for this question (row[3]) */
	 Str_Copy (Print->PrintedQuestions[QstInd].StrIndexes,row[2],
		   sizeof (Print->PrintedQuestions[QstInd].StrIndexes) - 1);
	 Str_Copy (Print->PrintedQuestions[QstInd].StrAnswers,row[3],
		   sizeof (Print->PrintedQuestions[QstInd].StrAnswers) - 1);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return (NumQsts == Print->NumQsts.All);
  }

/*****************************************************************************/
/********************** Remove test prints made by a user ********************/
/*****************************************************************************/

void TstPrn_RemovePrintsMadeByUsrInAllCrss (long UsrCod)
  {
   /***** Remove test prints questions for the given user *****/
   Tst_DB_RemovePrintQuestionsMadeByUsrInAllCrss (UsrCod);

   /***** Remove test prints made by the given user *****/
   Tst_DB_RemovePrintsMadeByUsrInAllCrss (UsrCod);
  }

/*****************************************************************************/
/************** Remove test prints made by a user in a course ****************/
/*****************************************************************************/

void TstPrn_RemovePrintsMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove test prints questions for the given user *****/
   Tst_DB_RemovePrintQuestionsMadeByUsrInCrs (UsrCod,CrsCod);

   /***** Remove test prints made by the given user *****/
   Tst_DB_RemovePrintsMadeByUsrInCrs (UsrCod,CrsCod);
  }

/*****************************************************************************/
/****************** Remove all test prints made in a course ******************/
/*****************************************************************************/

void TstPrn_RemoveCrsPrints (long CrsCod)
  {
   /***** Remove questions of tests made in the course *****/
   Tst_DB_RemovePrintQuestionsMadeInCrs (CrsCod);

   /***** Remove tests made in the course *****/
   Tst_DB_RemovePrintsMadeByInCrs (CrsCod);
  }

/*****************************************************************************/
/***************** Get number of test prints generated by me *****************/
/*****************************************************************************/

unsigned TstPrn_GetNumPrintsGeneratedByMe (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   unsigned NumPrintsGeneratedByMe = 0;

   if (Gbl.Usrs.Me.IBelongToCurrentCrs)
     {
      /***** Get number of test prints generated by me from database *****/
      NumRows = Tst_DB_GetNumPrintsGeneratedByMe (&mysql_res);

      if (NumRows == 0)
         NumPrintsGeneratedByMe = 0;
      else if (NumRows == 1)
        {
         /* Get number of hits */
         row = mysql_fetch_row (mysql_res);
         if (row[0] == NULL)
            NumPrintsGeneratedByMe = 0;
         else if (sscanf (row[0],"%u",&NumPrintsGeneratedByMe) != 1)
            NumPrintsGeneratedByMe = 0;
        }
      else
         Err_ShowErrorAndExit ("Error when getting number of tests.");

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return NumPrintsGeneratedByMe;
  }
