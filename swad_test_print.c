// swad_test_print.c: test exam prints made by users

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

#define _GNU_SOURCE 		// For asprintf
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_photo.h"
#include "swad_test.h"
#include "swad_test_print.h"
#include "swad_test_visibility.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

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
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TstPrn_ResetPrintExceptPrnCod (struct TstPrn_Print *Print);

static void TstPrn_WriteQstAndAnsToFill (struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned NumQst,
                                         struct Tst_Question *Question);
static void TstPrn_WriteAnswersToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                       unsigned NumQst,
                                       struct Tst_Question *Question);

//-----------------------------------------------------------------------------
static void TstPrn_WriteIntAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question);
static void TstPrn_WriteFltAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question);
static void TstPrn_WriteTF_AnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question);
static void TstPrn_WriteChoAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      struct Tst_Question *Question);
static void TstPrn_WriteTxtAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question);
//-----------------------------------------------------------------------------

static void TstPrn_PutCheckBoxAllowTeachers (bool AllowTeachers);

static void TstPrn_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST],
				       unsigned NumQst,
				       time_t TimeUTC[Dat_NUM_START_END_TIME],
				       struct Tst_Question *Question,
				       bool QuestionExists,
				       unsigned Visibility);

//-----------------------------------------------------------------------------
static void TstPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void TstPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void TstPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void TstPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
static void TstPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question);
//-----------------------------------------------------------------------------
static void TstPrn_GetCorrectIntAnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectFltAnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectTF_AnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectChoAnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectTxtAnswerFromDB (struct Tst_Question *Question);
//-----------------------------------------------------------------------------
static void TstPrn_WriteIntAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
static void TstPrn_WriteFltAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
static void TstPrn_WriteTF_AnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
static void TstPrn_WriteChoAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     const char *ClassTxt,
				     const char *ClassFeedback);
static void TstPrn_WriteTxtAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback);
//-----------------------------------------------------------------------------

static void TstPrn_WriteHeadUserCorrect (struct UsrData *UsrDat);

static void TstPrn_StoreOneQstOfPrintInDB (const struct TstPrn_Print *Print,
                                           unsigned NumQst);

static void TstPrn_PutFormToSelectUsrsToViewUsrsPrints (__attribute__((unused)) void *Args);

static void TstPrn_ShowUsrsPrints (__attribute__((unused)) void *Args);
static void TstPrn_ShowHeaderPrints (Usr_MeOrOther_t MeOrOther);
static void TstPrn_ShowUsrPrints (struct UsrData *UsrDat);
static void TstPrn_ShowPrintsSummaryRow (bool ItsMe,
                                         unsigned NumPrints,
                                         struct TstPrn_NumQuestions *NumTotalQsts,
                                         double TotalScore);

static void TstRes_CheckIfICanSeePrintResult (const struct TstPrn_Print *Print,
                                              long UsrCod,
                                              struct TstRes_ICanView *ICanView);

static void TstPrn_ShowTagsPresentInAPrint (long ResCod);

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
   Print->TimeUTC[Dat_START_TIME] =
   Print->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Print->NumQsts.All      =
   Print->NumQsts.NotBlank = 0;
   Print->Sent             = false;	// After creating an exam, it's not sent
   Print->AllowTeachers    = false;	// Teachers can't seen the exam if student don't allow it
   Print->Score            = 0.0;
  }

/*****************************************************************************/
/************** Create new blank test exam print in database *****************/
/*****************************************************************************/

void TstPrn_CreatePrintInDB (struct TstPrn_Print *Print)
  {
   /***** Insert new test exam print into table *****/
   Print->PrnCod =
   DB_QueryINSERTandReturnCode ("can not create new test exam print",
				"INSERT INTO tst_exams"
				" (CrsCod,UsrCod,StartTime,EndTime,NumQsts,NumQstsNotBlank,Sent,AllowTeachers,Score)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),%u,0,'N','N',0)",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Print->NumQsts.All);
  }

/*****************************************************************************/
/******************** Update test exam print in database *********************/
/*****************************************************************************/

void TstPrn_UpdatePrintInDB (const struct TstPrn_Print *Print)
  {
   /***** Update test exam print in database *****/
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   DB_QueryUPDATE ("can not update test exam",
		   "UPDATE tst_exams"
	           " SET EndTime=NOW(),"
	                "NumQstsNotBlank=%u,"
		        "Sent='%c',"
		        "AllowTeachers='%c',"
	                "Score='%.15lg'"
	           " WHERE ExaCod=%ld"
	           " AND CrsCod=%ld AND UsrCod=%ld",	// Extra checks
		   Print->NumQsts.NotBlank,
		   Print->Sent ? 'Y' :
			         'N',
		   Print->AllowTeachers ? 'Y' :
			                  'N',
		   Print->Score,
		   Print->PrnCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/****************** Show a test exam print to be answered ********************/
/*****************************************************************************/

void TstPrn_ShowTestPrintToFillIt (struct TstPrn_Print *Print,
                                   unsigned NumExamsGeneratedByMe,
                                   TstPrn_RequestOrConfirm_t RequestOrConfirm)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Test;
   extern const char *Txt_Continue;
   extern const char *Txt_Send;
   unsigned NumQst;
   struct Tst_Question Question;
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
      TstPrn_PutParamPrnCod (Print->PrnCod);
      Par_PutHiddenParamUnsigned (NULL,"NumTst",NumExamsGeneratedByMe);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

      /***** Write one row for each question *****/
      for (NumQst = 0;
	   NumQst < Print->NumQsts.All;
	   NumQst++)
	{
	 Gbl.RowEvenOdd = NumQst % 2;

	 /* Create test question */
	 Tst_QstConstructor (&Question);
	 Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

	 /* Show question */
	 if (!Tst_GetQstDataFromDB (&Question))	// Question exists
	    Lay_ShowErrorAndExit ("Wrong question.");

	 /* Write question and answers */
	 TstPrn_WriteQstAndAnsToFill (&Print->PrintedQuestions[NumQst],NumQst,&Question);

	 /* Destroy test question */
	 Tst_QstDestructor (&Question);
	}

      /***** End table *****/
      HTM_TABLE_End ();

      /***** End form *****/
      switch (RequestOrConfirm)
        {
	 case TstPrn_REQUEST:
            /* Send button */
            Btn_PutConfirmButton (Txt_Continue);
	    break;
	 case TstPrn_CONFIRM:
	    /* Will the test exam be visible by teachers? */
            TstPrn_PutCheckBoxAllowTeachers (true);

            /* Send button */
            Btn_PutCreateButton (Txt_Send);
	    break;
        }
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void TstPrn_WriteQstAndAnsToFill (struct TstPrn_PrintedQuestion *PrintedQuestion,
                                         unsigned NumQst,
                                         struct Tst_Question *Question)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1,"BIG_INDEX");
   Tst_WriteAnswerType (Question->Answer.Type,"DAT_SMALL");
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Write parameter with question code */
   Tst_WriteParamQstCod (NumQst,Question->QstCod);

   /* Stem */
   Tst_WriteQstStem (Question->Stem,"TEST_TXT",true);

   /* Media */
   Med_ShowMedia (&Question->Media,
		  "TEST_MED_SHOW_CONT",
		  "TEST_MED_SHOW");

   /* Answers */
   TstPrn_WriteAnswersToFill (PrintedQuestion,NumQst,Question);

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Write answers of a question to fill them ******************/
/*****************************************************************************/

static void TstPrn_WriteAnswersToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                       unsigned NumQst,
                                       struct Tst_Question *Question)
  {
   void (*TstPrn_WriteAnsBank[Tst_NUM_ANS_TYPES]) (const struct TstPrn_PrintedQuestion *PrintedQuestion,
						   unsigned NumQst,
						   struct Tst_Question *Question) =
    {
     [Tst_ANS_INT            ] = TstPrn_WriteIntAnsToFill,
     [Tst_ANS_FLOAT          ] = TstPrn_WriteFltAnsToFill,
     [Tst_ANS_TRUE_FALSE     ] = TstPrn_WriteTF_AnsToFill,
     [Tst_ANS_UNIQUE_CHOICE  ] = TstPrn_WriteChoAnsToFill,
     [Tst_ANS_MULTIPLE_CHOICE] = TstPrn_WriteChoAnsToFill,
     [Tst_ANS_TEXT           ] = TstPrn_WriteTxtAnsToFill,
    };

   /***** Write answers *****/
   TstPrn_WriteAnsBank[Question->Answer.Type] (PrintedQuestion,NumQst,Question);
  }

/*****************************************************************************/
/****************** Write integer answer when seeing a test ******************/
/*****************************************************************************/

static void TstPrn_WriteIntAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",NumQst);
   HTM_INPUT_TEXT (StrAns,11,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"11\"");
  }

/*****************************************************************************/
/****************** Write float answer when seeing a test ********************/
/*****************************************************************************/

static void TstPrn_WriteFltAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",NumQst);
   HTM_INPUT_TEXT (StrAns,Tst_MAX_BYTES_FLOAT_ANSWER,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"11\"");
  }

/*****************************************************************************/
/************** Write false / true answer when seeing a test ****************/
/*****************************************************************************/

static void TstPrn_WriteTF_AnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question)
  {
   extern const char *Txt_TF_QST[2];

   /***** Write selector for the answer *****/
   /* Initially user has not answered the question ==> initially all the answers will be blank.
      If the user does not confirm the submission of their exam ==>
      ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"Ans%010u\"",NumQst);
   HTM_OPTION (HTM_Type_STRING,"" ,PrintedQuestion->StrAnswers[0] == '\0',false,"&nbsp;");
   HTM_OPTION (HTM_Type_STRING,"T",PrintedQuestion->StrAnswers[0] == 'T' ,false,"%s",Txt_TF_QST[0]);
   HTM_OPTION (HTM_Type_STRING,"F",PrintedQuestion->StrAnswers[0] == 'F' ,false,"%s",Txt_TF_QST[1]);
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/******** Write single or multiple choice answer when seeing a test **********/
/*****************************************************************************/

static void TstPrn_WriteChoAnsToFill (const struct TstPrn_PrintedQuestion *PrintedQuestion,
                                      unsigned NumQst,
                                      struct Tst_Question *Question)
  {
   unsigned NumOpt;
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION];
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Change format of answers text *****/
   Tst_ChangeFormatAnswersText (Question);

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
      /* Initially user has not answered the question ==> initially all the answers will be blank.
	 If the user does not confirm the submission of their exam ==>
	 ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
      HTM_TD_Begin ("class=\"LT\"");

      snprintf (StrAns,sizeof (StrAns),"Ans%010u",NumQst);
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
	 HTM_INPUT_RADIO (StrAns,false,
			  "id=\"Ans%010u_%u\" value=\"%u\"%s"
			  " onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u);\"",
			  NumQst,NumOpt,
			  Indexes[NumOpt],
			  UsrAnswers[Indexes[NumOpt]] ? " checked=\"checked\"" :
				                           "",
                          NumQst,Question->Answer.NumOptions);
      else // Answer.Type == Tst_ANS_MULTIPLE_CHOICE
	 HTM_INPUT_CHECKBOX (StrAns,HTM_DONT_SUBMIT_ON_CHANGE,
			     "id=\"Ans%010u_%u\" value=\"%u\"%s",
			     NumQst,NumOpt,
			     Indexes[NumOpt],
			     UsrAnswers[Indexes[NumOpt]] ? " checked=\"checked\"" :
				                           "");

      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");
      HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"TEST_TXT\"",NumQst,NumOpt);
      HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
      HTM_LABEL_End ();
      HTM_TD_End ();

      /***** Write the option text *****/
      HTM_TD_Begin ("class=\"LT\"");
      HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"TEST_TXT\"",NumQst,NumOpt);
      HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
      HTM_LABEL_End ();
      Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
                     "TEST_MED_SHOW_CONT",
                     "TEST_MED_SHOW");
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
                                      unsigned NumQst,
                                      __attribute__((unused)) struct Tst_Question *Question)
  {
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",NumQst);
   HTM_INPUT_TEXT (StrAns,Tst_MAX_CHARS_ANSWERS_ONE_QST,PrintedQuestion->StrAnswers,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"40\"");
  }

/*****************************************************************************/
/************* Put checkbox to allow teachers to see test exam ***************/
/*****************************************************************************/

static void TstPrn_PutCheckBoxAllowTeachers (bool AllowTeachers)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Allow_teachers_to_consult_this_test;

   /***** Test exam will be available for teachers? *****/
   HTM_DIV_Begin ("class=\"CM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("AllowTchs",HTM_DONT_SUBMIT_ON_CHANGE,
		       "value=\"Y\"%s",
                       AllowTeachers ? " checked=\"checked\"" :	// Teachers can see test exam
                		       "");
   HTM_TxtF ("&nbsp;%s",Txt_Allow_teachers_to_consult_this_test);
   HTM_LABEL_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show test exam after assessing it *********************/
/*****************************************************************************/

void TstPrn_ShowPrintAfterAssess (struct TstPrn_Print *Print)
  {
   unsigned NumQst;
   struct Tst_Question Question;
   bool QuestionExists;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

   /***** Initialize score and number of questions not blank *****/
   Print->NumQsts.NotBlank = 0;
   Print->Score = 0.0;

   for (NumQst = 0;
	NumQst < Print->NumQsts.All;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

      /***** Get question data *****/
      QuestionExists = Tst_GetQstDataFromDB (&Question);

      /***** Write question and answers *****/
      TstPrn_WriteQstAndAnsExam (&Gbl.Usrs.Me.UsrDat,
				 Print->PrintedQuestions,NumQst,
				 Print->TimeUTC,
				 &Question,QuestionExists,
				 TstCfg_GetConfigVisibility ());

      /***** Store test exam question in database *****/
      TstPrn_StoreOneQstOfPrintInDB (Print,NumQst);

      /***** Compute total score *****/
      Print->Score += Print->PrintedQuestions[NumQst].Score;
      if (Print->PrintedQuestions[NumQst].StrAnswers[0])	// User's answer is not blank
	 Print->NumQsts.NotBlank++;

      /***** Update the number of accesses and the score of this question *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	 Tst_UpdateQstScoreInDB (&Print->PrintedQuestions[NumQst]);

      /***** Destroy test question *****/
      Tst_QstDestructor (&Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void TstPrn_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST],
				       unsigned NumQst,
				       time_t TimeUTC[Dat_NUM_START_END_TIME],
				       struct Tst_Question *Question,
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
      QuestionUneditedAfterExam = (Question->EditTime < TimeUTC[Dat_START_TIME]);
   else
      QuestionUneditedAfterExam = false;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1,"BIG_INDEX");
   if (QuestionUneditedAfterExam)
      Tst_WriteAnswerType (Question->Answer.Type,"DAT_SMALL");
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (QuestionExists)
     {
      if (QuestionUneditedAfterExam)
	{
	 /* Stem */
	 Tst_WriteQstStem (Question->Stem,"TEST_TXT",ICanView[TstVis_VISIBLE_QST_ANS_TXT]);

	 /* Media */
	 if (ICanView[TstVis_VISIBLE_QST_ANS_TXT])
	    Med_ShowMedia (&Question->Media,
			   "TEST_MED_SHOW_CONT",
			   "TEST_MED_SHOW");

	 /* Answers */
	 TstPrn_ComputeAnswerScore (&PrintedQuestions[NumQst],Question);
	 TstPrn_WriteAnswersExam (UsrDat,&PrintedQuestions[NumQst],Question,
	                          ICanView,"TEST_TXT","TEST_TXT_LIGHT");
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_Question_modified);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);

   /* Write score retrieved from database */
   if (ICanView[TstVis_VISIBLE_EACH_QST_SCORE])
     {
      HTM_DIV_Begin ("class=\"DAT_SMALL LM\"");
      HTM_TxtColonNBSP (Txt_Score);
      HTM_SPAN_Begin ("class=\"%s\"",
		      PrintedQuestions[NumQst].StrAnswers[0] ?
		      (PrintedQuestions[NumQst].Score > 0 ? "ANS_OK" :	// Correct/semicorrect
							    "ANS_BAD") :// Wrong
							    "ANS_0");	// Blank answer
      HTM_Double2Decimals (PrintedQuestions[NumQst].Score);
      HTM_SPAN_End ();
      HTM_DIV_End ();
     }

   /* Question feedback */
   if (QuestionUneditedAfterExam)
      if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
	 Tst_WriteQstFeedback (Question->Feedback,"TEST_TXT_LIGHT");

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Compute score of each question and store in database ************/
/*****************************************************************************/

void TstPrn_ComputeScoresAndStoreQuestionsOfPrint (struct TstPrn_Print *Print,
                                                   bool UpdateQstScore)
  {
   unsigned NumQst;
   struct Tst_Question Question;

   /***** Initialize total score *****/
   Print->Score = 0.0;
   Print->NumQsts.NotBlank = 0;

   /***** Compute and store scores of all questions *****/
   for (NumQst = 0;
	NumQst < Print->NumQsts.All;
	NumQst++)
     {
      /* Compute question score */
      Tst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;
      Question.Answer.Type = Tst_GetQstAnswerTypeFromDB (Question.QstCod);
      TstPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],&Question);
      Tst_QstDestructor (&Question);

      /* Store test exam question in database */
      TstPrn_StoreOneQstOfPrintInDB (Print,
				     NumQst);	// 0, 1, 2, 3...

      /* Accumulate total score */
      Print->Score += Print->PrintedQuestions[NumQst].Score;
      if (Print->PrintedQuestions[NumQst].StrAnswers[0])	// User's answer is not blank
	 Print->NumQsts.NotBlank++;

      /* Update the number of hits and the score of this question in tests database */
      if (UpdateQstScore)
	 Tst_UpdateQstScoreInDB (&Print->PrintedQuestions[NumQst]);
     }
  }

/*****************************************************************************/
/******************* Get correct answer and compute score ********************/
/*****************************************************************************/

void TstPrn_ComputeAnswerScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				struct Tst_Question *Question)
  {
   void (*TstPrn_GetCorrectAndComputeAnsScore[Tst_NUM_ANS_TYPES]) (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                                   struct Tst_Question *Question) =
    {
     [Tst_ANS_INT            ] = TstPrn_GetCorrectAndComputeIntAnsScore,
     [Tst_ANS_FLOAT          ] = TstPrn_GetCorrectAndComputeFltAnsScore,
     [Tst_ANS_TRUE_FALSE     ] = TstPrn_GetCorrectAndComputeTF_AnsScore,
     [Tst_ANS_UNIQUE_CHOICE  ] = TstPrn_GetCorrectAndComputeChoAnsScore,
     [Tst_ANS_MULTIPLE_CHOICE] = TstPrn_GetCorrectAndComputeChoAnsScore,
     [Tst_ANS_TEXT           ] = TstPrn_GetCorrectAndComputeTxtAnsScore,
    };

   /***** Get correct answer and compute answer score depending on type *****/
   TstPrn_GetCorrectAndComputeAnsScore[Question->Answer.Type] (PrintedQuestion,Question);
  }

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

static void TstPrn_GetCorrectAndComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get the numerical value of the correct answer,
          and compute score *****/
   TstPrn_GetCorrectIntAnswerFromDB (Question);
   TstPrn_ComputeIntAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get the numerical value of the minimum and maximum correct answers,
          and compute score *****/
   TstPrn_GetCorrectFltAnswerFromDB (Question);
   TstPrn_ComputeFltAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get answer true or false,
          and compute score *****/
   TstPrn_GetCorrectTF_AnswerFromDB (Question);
   TstPrn_ComputeTF_AnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get correct options of test question from database,
          and compute score *****/
   TstPrn_GetCorrectChoAnswerFromDB (Question);
   TstPrn_ComputeChoAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get correct text answers for this question from database,
          and compute score *****/
   TstPrn_GetCorrectTxtAnswerFromDB (Question);
   TstPrn_ComputeTxtAnsScore (PrintedQuestion,Question);
  }

/*****************************************************************************/
/**************** Get correct answer for each type of answer *****************/
/*****************************************************************************/

static void TstPrn_GetCorrectIntAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			       " FROM tst_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get correct answer *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&Question->Answer.Integer) != 1)
      Lay_ShowErrorAndExit ("Wrong integer answer.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void TstPrn_GetCorrectFltAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;
   double Tmp;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			       " FROM tst_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Lay_ShowErrorAndExit ("Wrong float range.");

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

static void TstPrn_GetCorrectTF_AnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			       " FROM tst_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer *****/
   row = mysql_fetch_row (mysql_res);
   Question->Answer.TF = row[0][0];

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void TstPrn_GetCorrectChoAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Correct"		// row[0]
			       " FROM tst_answers"
			      " WHERE QstCod=%ld"
			      " ORDER BY AnsInd",
			      Question->QstCod);
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

static void TstPrn_GetCorrectTxtAnswerFromDB (struct Tst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Question->Answer.NumOptions =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get answers of a question",
			      "SELECT Answer"		// row[0]
			       " FROM tst_answers"
			      " WHERE QstCod=%ld",
			      Question->QstCod);

   /***** Get text and correctness of answers for this question from database (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Allocate memory for text in this choice answer *****/
      if (!Tst_AllocateTextChoiceAnswer (Question,NumOpt))
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();

      /***** Copy answer text (row[0]) ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[0],
                Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
     }

   /***** Change format of answers text *****/
   Tst_ChangeFormatAnswersText (Question);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Compute answer score for each type of answer *****************/
/*****************************************************************************/

void TstPrn_ComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
		                const struct Tst_Question *Question)
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
				const struct Tst_Question *Question)
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
			        const struct Tst_Question *Question)
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
	                        const struct Tst_Question *Question)
  {
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION];
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
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
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
				 const struct Tst_Question *Question)
  {
   unsigned NumOpt;
   char TextAnsUsr[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];

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

void TstPrn_GetIndexesFromStr (const char StrIndexesOneQst[Tst_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			       unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION])
  {
   unsigned NumOpt;
   const char *Ptr;
   char StrOneIndex[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Get indexes from string *****/
   for (NumOpt = 0, Ptr = StrIndexesOneQst;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION && *Ptr;
	NumOpt++)
     {
      Par_GetNextStrUntilComma (&Ptr,StrOneIndex,Cns_MAX_DECIMAL_DIGITS_UINT);

      if (sscanf (StrOneIndex,"%u",&(Indexes[NumOpt])) != 1)
	 Lay_ShowErrorAndExit ("Wrong index of answer.");

      if (Indexes[NumOpt] >= Tst_MAX_OPTIONS_PER_QUESTION)
	 Lay_ShowErrorAndExit ("Wrong index of answer.");
     }

   /***** Initialize remaining to 0 *****/
   for (;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      Indexes[NumOpt] = 0;
  }

/*****************************************************************************/
/************ Get vector of bool answers from string with answers ************/
/*****************************************************************************/

void TstPrn_GetAnswersFromStr (const char StrAnswersOneQst[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1],
			       bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION])
  {
   unsigned NumOpt;
   const char *Ptr;
   char StrOneAnswer[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned AnsUsr;

   /***** Initialize all answers to false *****/
   for (NumOpt = 0;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
	NumOpt++)
      UsrAnswers[NumOpt] = false;

   /***** Set selected answers to true *****/
   for (NumOpt = 0, Ptr = StrAnswersOneQst;
	NumOpt < Tst_MAX_OPTIONS_PER_QUESTION && *Ptr;
	NumOpt++)
     {
      Par_GetNextStrUntilComma (&Ptr,StrOneAnswer,Cns_MAX_DECIMAL_DIGITS_UINT);

      if (sscanf (StrOneAnswer,"%u",&AnsUsr) != 1)
	 Lay_ShowErrorAndExit ("Bad user's answer.");

      if (AnsUsr >= Tst_MAX_OPTIONS_PER_QUESTION)
	 Lay_ShowErrorAndExit ("Bad user's answer.");

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

void TstPrn_WriteAnswersExam (struct UsrData *UsrDat,
                              const struct TstPrn_PrintedQuestion *PrintedQuestion,
			      struct Tst_Question *Question,
			      bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			      const char *ClassTxt,
			      const char *ClassFeedback)
  {
   void (*TstPrn_WriteAnsExam[Tst_NUM_ANS_TYPES]) (struct UsrData *UsrDat,
                                                   const struct TstPrn_PrintedQuestion *PrintedQuestion,
				                   struct Tst_Question *Question,
				                   bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				                   const char *ClassTxt,
				                   const char *ClassFeedback) =
    {
     [Tst_ANS_INT            ] = TstPrn_WriteIntAnsPrint,
     [Tst_ANS_FLOAT          ] = TstPrn_WriteFltAnsPrint,
     [Tst_ANS_TRUE_FALSE     ] = TstPrn_WriteTF_AnsPrint,
     [Tst_ANS_UNIQUE_CHOICE  ] = TstPrn_WriteChoAnsPrint,
     [Tst_ANS_MULTIPLE_CHOICE] = TstPrn_WriteChoAnsPrint,
     [Tst_ANS_TEXT           ] = TstPrn_WriteTxtAnsPrint,
    };

   /***** Get correct answer and compute answer score depending on type *****/
   TstPrn_WriteAnsExam[Question->Answer.Type] (UsrDat,PrintedQuestion,Question,
	                                       ICanView,ClassTxt,ClassFeedback);
  }

/*****************************************************************************/
/******************* Write integer answer in a test print ********************/
/*****************************************************************************/

static void TstPrn_WriteIntAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   long IntAnswerUsr;

   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstPrn_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (PrintedQuestion->StrAnswers[0])		// If user has answered the question
     {
      if (sscanf (PrintedQuestion->StrAnswers,"%ld",&IntAnswerUsr) == 1)
	{
         HTM_TD_Begin ("class=\"%s CM\"",
		       ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ?
			  (IntAnswerUsr == Question->Answer.Integer ? "ANS_OK" :
							              "ANS_BAD") :
			  "ANS_0");
         HTM_Long (IntAnswerUsr);
         HTM_TD_End ();
	}
      else
        {
         HTM_TD_Begin ("class=\"ANS_0 CM\"");
         HTM_Txt ("?");
         HTM_TD_End ();
        }
     }
   else							// If user has omitted the answer
      HTM_TD_Empty (1);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (ICanView[TstVis_VISIBLE_QST_ANS_TXT] &&
       ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
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

static void TstPrn_WriteFltAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   double FloatAnsUsr = 0.0;

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Lay_ShowErrorAndExit ("Wrong float range.");

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstPrn_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (PrintedQuestion->StrAnswers[0])	// If user has answered the question
     {
      FloatAnsUsr = Str_GetDoubleFromStr (PrintedQuestion->StrAnswers);
      // A bad formatted floating point answer will interpreted as 0.0
      HTM_TD_Begin ("class=\"%s CM\"",
		    ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ?
		       ((FloatAnsUsr >= Question->Answer.FloatingPoint[0] &&
			 FloatAnsUsr <= Question->Answer.FloatingPoint[1]) ? "ANS_OK" :
							                     "ANS_BAD") :
		       "ANS_0");
      HTM_Double (FloatAnsUsr);
     }
   else					// If user has omitted the answer
      HTM_TD_Begin (NULL);
   HTM_TD_End ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (ICanView[TstVis_VISIBLE_QST_ANS_TXT] &&
       ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
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

static void TstPrn_WriteTF_AnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   char AnsTFUsr;

   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   AnsTFUsr = PrintedQuestion->StrAnswers[0];

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstPrn_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   HTM_TD_Begin ("class=\"%s CM\"",
		 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ?
		    (AnsTFUsr == Question->Answer.TF ? "ANS_OK" :
					               "ANS_BAD") :
		    "ANS_0");
   Tst_WriteAnsTF (AnsTFUsr);
   HTM_TD_End ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (ICanView[TstVis_VISIBLE_QST_ANS_TXT] &&
       ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
      Tst_WriteAnsTF (Question->Answer.TF);
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write single or multiple choice answer in a test print ***********/
/*****************************************************************************/

static void TstPrn_WriteChoAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     const char *ClassTxt,
				     const char *ClassFeedback)
  {
   extern const char *Txt_TST_Answer_given_by_the_user;
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   unsigned NumOpt;
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION];
   struct
     {
      char *Class;
      char *Str;
     } Ans;

   /***** Change format of answers text *****/
   Tst_ChangeFormatAnswersText (Question);

   /***** Change format of answers feedback *****/
   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
      Tst_ChangeFormatAnswersFeedback (Question);

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
               Ans.Class = "ANS_OK";
               Ans.Str   = "&check;";
              }
            else
              {
               Ans.Class = "ANS_BAD";
               Ans.Str   = "&cross;";
              }
           }
         else
	   {
	    Ans.Class = "ANS_0";
	    Ans.Str   = "&bull;";
	   }

	 HTM_TD_Begin ("class=\"%s CT\" title=\"%s\"",
		       Ans.Class,Txt_TST_Answer_given_by_the_user);
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
	    HTM_TD_Begin ("class=\"ANS_0 CT\" title=\"%s\"",
		          Txt_TST_Answer_given_by_the_teachers);
	    HTM_Txt ("&bull;");
	    HTM_TD_End ();
           }
         else
            HTM_TD_Empty (1);
        }
      else
	{
	 HTM_TD_Begin ("class=\"ANS_0 CT\"");
         Ico_PutIconNotVisible ();
         HTM_TD_End ();
	}

      /* Answer letter (a, b, c,...) */
      HTM_TD_Begin ("class=\"%s LT\"",ClassTxt);
      HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
      HTM_TD_End ();

      /* Answer text and feedback */
      HTM_TD_Begin ("class=\"LT\"");

      HTM_DIV_Begin ("class=\"%s\"",ClassTxt);
      if (ICanView[TstVis_VISIBLE_QST_ANS_TXT])
	{
	 HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
	 Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
			"TEST_MED_SHOW_CONT",
			"TEST_MED_SHOW");
	}
      else
         Ico_PutIconNotVisible ();
      HTM_DIV_End ();

      if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	 if (Question->Answer.Options[Indexes[NumOpt]].Feedback)
	    if (Question->Answer.Options[Indexes[NumOpt]].Feedback[0])
	      {
	       HTM_DIV_Begin ("class=\"%s\"",ClassFeedback);
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

static void TstPrn_WriteTxtAnsPrint (struct UsrData *UsrDat,
                                     const struct TstPrn_PrintedQuestion *PrintedQuestion,
				     struct Tst_Question *Question,
				     bool ICanView[TstVis_NUM_ITEMS_VISIBILITY],
				     __attribute__((unused)) const char *ClassTxt,
				     __attribute__((unused)) const char *ClassFeedback)
  {
   unsigned NumOpt;
   char TextAnsUsr[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   bool Correct = false;

   /***** Change format of answers text *****/
   Tst_ChangeFormatAnswersText (Question);

   /***** Change format of answers feedback *****/
   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
      Tst_ChangeFormatAnswersFeedback (Question);

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
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
      HTM_TD_Begin ("class=\"%s CT\"",
		    ICanView[TstVis_VISIBLE_CORRECT_ANSWER] ? (Correct ? "ANS_OK" :
				                     "ANS_BAD") :
		                          "ANS_0");
      HTM_Txt (PrintedQuestion->StrAnswers);
     }
   else						// If user has omitted the answer
      HTM_TD_Begin (NULL);
   HTM_TD_End ();

   /***** Write the correct answers *****/
   if (ICanView[TstVis_VISIBLE_QST_ANS_TXT] &&
       ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
     {
      HTM_TD_Begin ("class=\"CT\"");
      HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
        {
	 HTM_TR_Begin (NULL);

         /* Answer letter (a, b, c,...) */
         HTM_TD_Begin ("class=\"ANS_0 LT\"");
         HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
         HTM_TD_End ();

         /* Answer text and feedback */
         HTM_TD_Begin ("class=\"LT\"");

         HTM_DIV_Begin ("class=\"ANS_0\"");
         HTM_Txt (Question->Answer.Options[NumOpt].Text);
         HTM_DIV_End ();

         if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
	    if (Question->Answer.Options[NumOpt].Feedback)
	       if (Question->Answer.Options[NumOpt].Feedback[0])
		 {
		  HTM_DIV_Begin ("class=\"TEST_TXT_LIGHT\"");
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
      HTM_TD_Begin ("class=\"ANS_0 CT\"");
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

static void TstPrn_WriteHeadUserCorrect (struct UsrData *UsrDat)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   HTM_TD_Begin ("class=\"DAT_SMALL CM\"");
   HTM_Txt (Txt_User[UsrDat->Sex]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_SMALL CM\"");
   HTM_Txt (Txt_ROLES_PLURAL_Abc[Rol_TCH][Usr_SEX_UNKNOWN]);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************ Store user's answers of an test print into database ************/
/*****************************************************************************/

static void TstPrn_StoreOneQstOfPrintInDB (const struct TstPrn_Print *Print,
                                           unsigned NumQst)
  {
   /***** Insert question and user's answers into database *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   DB_QueryREPLACE ("can not update a question of a test exam",
		    "REPLACE INTO tst_exam_questions"
		    " (ExaCod,QstCod,QstInd,Score,Indexes,Answers)"
		    " VALUES"
		    " (%ld,%ld,%u,'%.15lg','%s','%s')",
		    Print->PrnCod,Print->PrintedQuestions[NumQst].QstCod,
		    NumQst,	// 0, 1, 2, 3...
		    Print->PrintedQuestions[NumQst].Score,
		    Print->PrintedQuestions[NumQst].StrIndexes,
		    Print->PrintedQuestions[NumQst].StrAnswers);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************* Select users and dates to show their test exams ***************/
/*****************************************************************************/

void TstPrn_SelUsrsToViewUsrsPrints (void)
  {
   TstPrn_PutFormToSelectUsrsToViewUsrsPrints (NULL);
  }

static void TstPrn_PutFormToSelectUsrsToViewUsrsPrints (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_results;

   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeUsrTstResCrs,
				     NULL,NULL,
				     Txt_Results,
				     Hlp_ASSESSMENT_Tests_results,
				     Txt_View_results,
				     true);	// Put form with date range
  }

/*****************************************************************************/
/******************** Select dates to show my test exams *********************/
/*****************************************************************************/

void TstPrn_SelDatesToSeeMyPrints (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_results;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_START_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME  ] = Dat_HMS_DO_NOT_SET
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
/***************************** Show my test exams ****************************/
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

   /***** List my test exams *****/
   TstCfg_GetConfigFromDB ();	// To get visibility
   TstPrn_ShowUsrPrints (&Gbl.Usrs.Me.UsrDat);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/******************** Get users and show their test exams ********************/
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

   /***** List the test exams of the selected users *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show test exams *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    TstPrn_ShowUsrPrints (&Gbl.Usrs.Other.UsrDat);
	   }
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************ Show header of my test exams ***********************/
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

   HTM_TH (3,2,"CT LINE_BOTTOM",Txt_User[MeOrOther == Usr_ME ? Gbl.Usrs.Me.UsrDat.Sex :
		                                               Usr_SEX_UNKNOWN]);
   HTM_TH (3,1,"LT LINE_BOTTOM",Txt_START_END_TIME[Dat_START_TIME]);
   HTM_TH (3,1,"LT LINE_BOTTOM",Txt_START_END_TIME[Dat_END_TIME  ]);
   HTM_TH (3,1,"RT LINE_BOTTOM LINE_LEFT",Txt_Questions);
   HTM_TH (1,2,"CT LINE_LEFT",Txt_Answers);
   HTM_TH (1,2,"CT LINE_LEFT",Txt_Score);
   HTM_TH (3,1,"RT LINE_BOTTOM LINE_LEFT",Txt_Grade);
   HTM_TH (3,1,"LINE_BOTTOM LINE_LEFT",NULL);

   HTM_TR_End ();

   /***** Second row *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"RT LINE_LEFT",Txt_ANSWERS_non_blank);
   HTM_TH (1,1,"RT",Txt_ANSWERS_blank);
   HTM_TH (1,1,"RT LINE_LEFT",Txt_total);
   HTM_TH (1,1,"RT",Txt_average);

   HTM_TR_End ();

   /***** Third row *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"RT LINE_BOTTOM LINE_LEFT","{-1&le;<em>p<sub>i</sub></em>&le;1}");
   HTM_TH (1,1,"RT LINE_BOTTOM","{<em>p<sub>i</sub></em>=0}");
   HTM_TH (1,1,"RT LINE_BOTTOM LINE_LEFT","<em>&Sigma;p<sub>i</sub></em>");
   HTM_TH (1,1,"RT LINE_BOTTOM","-1&le;<em style=\"text-decoration:overline;\">p</em>&le;1");

   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show the test prints of a user in the current course ***********/
/*****************************************************************************/

static void TstPrn_ShowUsrPrints (struct UsrData *UsrDat)
  {
   extern const char *Txt_View_test;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
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
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);
   struct TstRes_ICanView ICanView;
   char *ClassDat;

   /***** Reset total number of questions and total score *****/
   NumTotalQsts.All      =
   NumTotalQsts.NotBlank = 0;
   TotalScore = 0.0;

   /***** Make database query *****/
   /*           From here...                 ...to here
         ___________|_____                   _____|___________
   -----|______Exam_|_____|-----------------|_____|_Exam______|-----> time
      Start         |    End              Start   |          End
   */
   NumPrints =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get test exams of a user",
			      "SELECT ExaCod"			// row[0]
			       " FROM tst_exams"
			      " WHERE CrsCod=%ld"
			        " AND UsrCod=%ld"
			        " AND EndTime>=FROM_UNIXTIME(%ld)"
			        " AND StartTime<=FROM_UNIXTIME(%ld)"
			      " ORDER BY ExaCod",
			      Gbl.Hierarchy.Crs.CrsCod,
			      UsrDat->UsrCod,
			      (long) Gbl.DateRange.TimeUTC[Dat_START_TIME],
			      (long) Gbl.DateRange.TimeUTC[Dat_END_TIME  ]);

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);
   Usr_ShowTableCellWithUsrData (UsrDat,NumPrints);

   /***** Get and print test exams *****/
   if (NumPrints)
     {
      for (NumPrint = 0;
           NumPrint < NumPrints;
           NumPrint++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get print code (row[0]) */
	 if ((Print.PrnCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of test exam.");

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
	       Lay_NotEnoughMemoryExit ();
	    HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		          Id,ClassDat,Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"%s RT LINE_LEFT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.Result)
	    HTM_Unsigned (Print.NumQsts.All);
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

         /* Write number of non-blank answers */
	 HTM_TD_Begin ("class=\"%s RT LINE_LEFT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"%s RT LINE_LEFT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.Score)
	    HTM_Double2Decimals (Print.NumQsts.All ? Print.Score /
		                                     (double) Print.NumQsts.All :
			                             0.0);
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

         /* Write grade */
	 HTM_TD_Begin ("class=\"%s RT LINE_LEFT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.Score)
            TstPrn_ComputeAndShowGrade (Print.NumQsts.All,Print.Score,Tst_SCORE_MAX);
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Link to show this test exam */
	 HTM_TD_Begin ("class=\"RT LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanView.Result)
	   {
	    Frm_BeginForm (Gbl.Action.Act == ActSeeMyTstResCrs ? ActSeeOneTstResMe :
						                 ActSeeOneTstResOth);
	    TstPrn_PutParamPrnCod (Print.PrnCod);
	    Ico_PutIconLink ("tasks.svg",Txt_View_test);
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
      TstPrn_ShowPrintsSummaryRow (ItsMe,NumPrintsVisibleByTchs,
                                   &NumTotalQsts,TotalScore);
     }
   else
     {
      /* Columns for dates */
      HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Column for questions */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Columns for answers */
      HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Columns for score */
      HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Column for grade */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Column for link to show the result */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*************** Write parameter with code of test exam print ****************/
/*****************************************************************************/

void TstPrn_PutParamPrnCod (long ExaCod)
  {
   Par_PutHiddenParamLong (NULL,"PrnCod",ExaCod);
  }

/*****************************************************************************/
/*************** Get parameter with code of test exam print ******************/
/*****************************************************************************/

long TstPrn_GetParamPrnCod (void)
  {
   /***** Get code of test exam print *****/
   return Par_GetParToLong ("PrnCod");
  }

/*****************************************************************************/
/**************** Show row with summary of user's test exams *****************/
/*****************************************************************************/

static void TstPrn_ShowPrintsSummaryRow (bool ItsMe,
                                         unsigned NumPrints,
                                         struct TstPrn_NumQuestions *NumTotalQsts,
                                         double TotalScore)
  {
   extern const char *Txt_Visible_tests;
   bool ICanViewTotalScore;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewTotalScore = ItsMe &&
		              TstVis_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 ICanViewTotalScore = ItsMe ||
			      NumPrints;
	 break;
      case Rol_SYS_ADM:
	 ICanViewTotalScore = true;
	 break;
      default:
	 ICanViewTotalScore = false;
	 break;
     }

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Row title *****/
   HTM_TD_Begin ("colspan=\"2\" class=\"DAT_N RM LINE_TOP LINE_BOTTOM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtColonNBSP (Txt_Visible_tests);
   HTM_Unsigned (NumPrints);
   HTM_TD_End ();

   /***** Write total number of questions *****/
   HTM_TD_Begin ("class=\"DAT_N RM LINE_TOP LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumPrints)
      HTM_Unsigned (NumTotalQsts->All);
   HTM_TD_End ();

   /***** Write total number of non-blank answers *****/
   HTM_TD_Begin ("class=\"DAT_N RM LINE_TOP LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumPrints)
      HTM_Unsigned (NumTotalQsts->NotBlank);
   HTM_TD_End ();

   /***** Write total number of blank answers *****/
   HTM_TD_Begin ("class=\"DAT_N RM LINE_TOP LINE_BOTTOM COLOR%u\"",Gbl.RowEvenOdd);
   if (NumPrints)
      HTM_Unsigned (NumTotalQsts->All - NumTotalQsts->NotBlank);
   HTM_TD_End ();

   /***** Write total score *****/
   HTM_TD_Begin ("class=\"DAT_N RM LINE_TOP LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
     {
      HTM_Double2Decimals (TotalScore);
      HTM_Txt ("/");
      HTM_Unsigned (NumTotalQsts->All);
     }
   HTM_TD_End ();

   /***** Write average score per question *****/
   HTM_TD_Begin ("class=\"DAT_N RM LINE_TOP LINE_BOTTOM COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      HTM_Double2Decimals (NumTotalQsts->All ? TotalScore / (double) NumTotalQsts->All :
			                       0.0);
   HTM_TD_End ();

   /***** Write grade over Tst_SCORE_MAX *****/
   HTM_TD_Begin ("class=\"DAT_N RM LINE_TOP LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      TstPrn_ComputeAndShowGrade (NumTotalQsts->All,TotalScore,Tst_SCORE_MAX);
   HTM_TD_End ();

   /***** Last cell *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Show one test exam of another user *********************/
/*****************************************************************************/

void TstPrn_ShowOnePrint (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Result;
   extern const char *Txt_The_user_does_not_exist;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_Tags;
   struct TstPrn_Print Print;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstRes_ICanView ICanView;

   /***** Get the code of the test *****/
   TstPrn_ResetPrint (&Print);
   if ((Print.PrnCod = TstPrn_GetParamPrnCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of test is missing.");

   /***** Get test exam data *****/
   TstPrn_GetPrintDataByPrnCod (&Print);

   /***** Get if I can see print result and score *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
      TstCfg_GetConfigFromDB ();	// To get visibility
   TstRes_CheckIfICanSeePrintResult (&Print,Gbl.Usrs.Other.UsrDat.UsrCod,&ICanView);

   if (ICanView.Result)	// I am allowed to view this test print result
     {
      /***** Get questions and user's answers of the test exam from database *****/
      TstPrn_GetPrintQuestionsFromDB (&Print);

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
      if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 Lay_ShowErrorAndExit (Txt_The_user_does_not_exist);
      if (!Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
         Lay_NoPermissionExit ();

      /* User */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtColon (Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role][Gbl.Usrs.Other.UsrDat.Sex]);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LB\"");
      ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
      HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname1);
      if (Gbl.Usrs.Other.UsrDat.Surname2[0])
	 HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname2);
      if (Gbl.Usrs.Other.UsrDat.FrstName[0])
	 HTM_TxtF (", %s",Gbl.Usrs.Other.UsrDat.FrstName);
      HTM_BR ();
      Pho_ShowUsrPhotoIfAllowed (&Gbl.Usrs.Other.UsrDat,"PHOTO45x60",Pho_ZOOM,false);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Start/end time (for user in this test print) *****/
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 if (asprintf (&Id,"tst_date_%u",(unsigned) StartEndTime) < 0)
	    Lay_NotEnoughMemoryExit ();

	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"DAT_N RT\"");
	 HTM_TxtColon (Txt_START_END_TIME[StartEndTime]);
	 HTM_TD_End ();

	 HTM_TD_Begin ("id=\"%s\" class=\"DAT LB\"",Id);
	 Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,true,0x7);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 free (Id);
	}

      /***** Number of questions *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtColon (Txt_Questions);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LB\"");
      HTM_Unsigned (Print.NumQsts.All);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Number of answers *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtColon (Txt_Answers);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LB\"");
      HTM_Unsigned (Print.NumQsts.NotBlank);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Score *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtColon (Txt_Score);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LB\"");
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

      /***** Grade *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtColon (Txt_Grade);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LB\"");
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

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtColon (Txt_Tags);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LB\"");
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
   else	// I am not allowed to view this test exam
      Lay_NoPermissionExit ();
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
	 ICanView->Result = Print->Sent && Usr_ItsMe (UsrCod);

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
	 ICanView->Score  = Print->Sent && (Print->AllowTeachers || Usr_ItsMe (UsrCod));
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
/********************* Show test tags in this test exam **********************/
/*****************************************************************************/

static void TstPrn_ShowTagsPresentInAPrint (long ResCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumTags;

   /***** Get all tags of questions in this test *****/
   NumTags = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get tags"
			      " present in a test exam",
		   "SELECT tst_tags.TagTxt"	// row[0]
		    " FROM (SELECT DISTINCT(tst_question_tags.TagCod)"
			    " FROM tst_question_tags,"
				  "tst_exam_questions"
			   " WHERE tst_exam_questions.ExaCod=%ld"
			     " AND tst_exam_questions.QstCod=tst_question_tags.QstCod) AS TagsCods,"
			  "tst_tags"
		   " WHERE TagsCods.TagCod=tst_tags.TagCod"
		   " ORDER BY tst_tags.TagTxt",
		   ResCod);
   Tst_ShowTagList (NumTags,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Show user's and correct answers of a test exam ***************/
/*****************************************************************************/

void TstPrn_ShowPrintAnswers (struct UsrData *UsrDat,
			      unsigned NumQsts,
			      struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST],
			      time_t TimeUTC[Dat_NUM_START_END_TIME],
			      unsigned Visibility)
  {
   unsigned NumQst;
   struct Tst_Question Question;
   bool QuestionExists;

   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);
      Question.QstCod = PrintedQuestions[NumQst].QstCod;

      /***** Get question data *****/
      QuestionExists = Tst_GetQstDataFromDB (&Question);

      /***** Write questions and answers *****/
      TstPrn_WriteQstAndAnsExam (UsrDat,
				 PrintedQuestions,NumQst,
				 TimeUTC,
				 &Question,QuestionExists,
				 Visibility);

      /***** Destroy test question *****/
      Tst_QstDestructor (&Question);
     }
  }

/*****************************************************************************/
/************ Get data of a test exam using its test exam code ***************/
/*****************************************************************************/

void TstPrn_GetPrintDataByPrnCod (struct TstPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Make database query *****/
   if (DB_QuerySELECT (&mysql_res,"can not get data of a test exam",
		       "SELECT UsrCod,"				// row[0]
			      "UNIX_TIMESTAMP(StartTime),"	// row[1]
			      "UNIX_TIMESTAMP(EndTime),"	// row[2]
		              "NumQsts,"			// row[3]
		              "NumQstsNotBlank,"		// row[4]
			      "Sent,"				// row[5]
		              "AllowTeachers,"			// row[6]
		              "Score"				// row[7]
		        " FROM tst_exams"
		       " WHERE ExaCod=%ld"
		         " AND CrsCod=%ld",
		       Print->PrnCod,
		       Gbl.Hierarchy.Crs.CrsCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user code (row[0]) */
      Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get date-time (row[1] and row[2] hold UTC date-time) */
      Print->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
      Print->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[2]);

      /* Get number of questions (row[3]) */
      if (sscanf (row[3],"%u",&Print->NumQsts.All) != 1)
	 Print->NumQsts.All = 0;

      /* Get number of questions not blank (row[4]) */
      if (sscanf (row[4],"%u",&Print->NumQsts.NotBlank) != 1)
	 Print->NumQsts.NotBlank = 0;

      /* Get if exam has been sent (row[5]) */
      Print->Sent = (row[5][0] == 'Y');

      /* Get if teachers are allowed to see this test exam (row[6]) */
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
/*********** Get the questions of a test exam print from database ************/
/*****************************************************************************/

void TstPrn_GetPrintQuestionsFromDB (struct TstPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned NumQst;

   /***** Get questions of a test exam print from database *****/
   NumQsts =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get questions"
					 " of a test exam",
			      "SELECT QstCod,"	// row[0]
			             "Score,"	// row[1]
			             "Indexes,"	// row[2]
			             "Answers"	// row[3]
			       " FROM tst_exam_questions"
			      " WHERE ExaCod=%ld"
			      " ORDER BY QstInd",
			      Print->PrnCod);

   /***** Get questions *****/
   if (NumQsts == Print->NumQsts.All)
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get question code (row[0]) */
	 if ((Print->PrintedQuestions[NumQst].QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of question.");

	 /* Get score (row[1]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[1],"%lf",&Print->PrintedQuestions[NumQst].Score) != 1)
            Lay_ShowErrorAndExit ("Wrong question score.");
         Str_SetDecimalPointToLocal ();	// Return to local system

	 /* Get indexes for this question (row[2]) */
	 Str_Copy (Print->PrintedQuestions[NumQst].StrIndexes,row[2],
		   sizeof (Print->PrintedQuestions[NumQst].StrIndexes) - 1);

	 /* Get answers selected by user for this question (row[3]) */
	 Str_Copy (Print->PrintedQuestions[NumQst].StrAnswers,row[3],
		   sizeof (Print->PrintedQuestions[NumQst].StrAnswers) - 1);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumQsts != Print->NumQsts.All)
      Lay_WrongExamExit ();
  }

/*****************************************************************************/
/******************* Remove test exam prints made by a user ******************/
/*****************************************************************************/

void TstPrn_RemovePrintsMadeByUsrInAllCrss (long UsrCod)
  {
   /***** Remove test prints questions for the given user *****/
   DB_QueryDELETE ("can not remove test exams made by a user",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,tst_exam_questions"
                   " WHERE tst_exams.UsrCod=%ld"
                   " AND tst_exams.ExaCod=tst_exam_questions.ExaCod",
		   UsrCod);

   /***** Remove test prints made by the given user *****/
   DB_QueryDELETE ("can not remove test exams made by a user",
		   "DELETE FROM tst_exams"
	           " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************ Remove test exam prints made by a user in a course *************/
/*****************************************************************************/

void TstPrn_RemovePrintsMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove test exams made by the given user *****/
   DB_QueryDELETE ("can not remove test exams made by a user in a course",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,tst_exam_questions"
                   " WHERE tst_exams.CrsCod=%ld AND tst_exams.UsrCod=%ld"
                   " AND tst_exams.ExaCod=tst_exam_questions.ExaCod",
		   CrsCod,UsrCod);

   DB_QueryDELETE ("can not remove test exams made by a user in a course",
		   "DELETE FROM tst_exams"
	           " WHERE CrsCod=%ld AND UsrCod=%ld",
		   CrsCod,UsrCod);
  }

/*****************************************************************************/
/**************** Remove all test exam prints made in a course ***************/
/*****************************************************************************/

void TstPrn_RemoveCrsPrints (long CrsCod)
  {
   /***** Remove questions of test exams made in the course *****/
   DB_QueryDELETE ("can not remove test exams made in a course",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,tst_exam_questions"
                   " WHERE tst_exams.CrsCod=%ld"
                   " AND tst_exams.ExaCod=tst_exam_questions.ExaCod",
		   CrsCod);

   /***** Remove test exams made in the course *****/
   DB_QueryDELETE ("can not remove test exams made in a course",
		   "DELETE FROM tst_exams WHERE CrsCod=%ld",
		   CrsCod);
  }
