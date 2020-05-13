// swad_test_print.c: test exam prints made by users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

static void TstPrn_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct TstPrn_Print *Print,
				       unsigned NumQst,
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

static void TstPrn_GetCorrectIntAnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectFltAnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectTF_AnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectChoAnswerFromDB (struct Tst_Question *Question);
static void TstPrn_GetCorrectTxtAnswerFromDB (struct Tst_Question *Question);

//-----------------------------------------------------------------------------

static void TstPrn_WriteAnswersExam (struct UsrData *UsrDat,
                                     const struct TstPrn_Print *Print,
                                     unsigned NumQst,
				     struct Tst_Question *Question,
				     unsigned Visibility);
static void TstPrn_WriteIntAnsExam (struct UsrData *UsrDat,
                                    const struct TstPrn_Print *Print,
				    unsigned NumQst,
				    const struct Tst_Question *Question,
				    unsigned Visibility);
static void TstPrn_WriteFloatAnsExam (struct UsrData *UsrDat,
                                      const struct TstPrn_Print *Print,
				      unsigned NumQst,
				      const struct Tst_Question *Question,
				      unsigned Visibility);
static void TstPrn_WriteTFAnsExam (struct UsrData *UsrDat,
                                   const struct TstPrn_Print *Print,
				   unsigned NumQst,
				   const struct Tst_Question *Question,
				   unsigned Visibility);
static void TstPrn_WriteChoiceAnsExam (struct UsrData *UsrDat,
                                       const struct TstPrn_Print *Print,
				       unsigned NumQst,
				       struct Tst_Question *Question,
				       unsigned Visibility);
static void TstPrn_WriteTextAnsExam (struct UsrData *UsrDat,
                                     const struct TstPrn_Print *Print,
				     unsigned NumQst,
				     struct Tst_Question *Question,
				     unsigned Visibility);
static void TstPrn_WriteHeadUserCorrect (struct UsrData *UsrDat);

static void TstPrn_StoreOneQstOfPrintInDB (const struct TstPrn_Print *Print,
                                           unsigned NumQst);

static void TstPrn_PutFormToSelectUsrsToViewUsrsExams (__attribute__((unused)) void *Args);

static void TstPrn_ShowUsrsExams (__attribute__((unused)) void *Args);
static void TstPrn_ShowHeaderExams (void);
static void TstPrn_ShowExams (struct UsrData *UsrDat);
static void TstPrn_ShowExamsSummaryRow (bool ItsMe,
                                        unsigned NumExams,
                                        unsigned NumTotalQsts,
                                        unsigned NumTotalQstsNotBlank,
                                        double TotalScoreOfAllTests);
static void TstPrn_ShowTagsPresentInAnExam (long ResCod);

/*****************************************************************************/
/******************************** Reset exam *********************************/
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
   Print->NumQsts                 =
   Print->NumQstsNotBlank         = 0;
   Print->Sent                    = false;	// After creating an exam, it's not sent
   Print->AllowTeachers           = false;	// Teachers can't seen the exam if student don't allow it
   Print->Score                   = 0.0;
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
				Print->NumQsts);
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
		   Print->NumQstsNotBlank,
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
/********************* Show test exam after assessing it *********************/
/*****************************************************************************/

void TstPrn_ShowExamAfterAssess (struct TstPrn_Print *Print)
  {
   unsigned NumQst;
   struct Tst_Question Question;
   bool QuestionExists;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

   /***** Initialize score and number of questions not blank *****/
   Print->NumQstsNotBlank = 0;
   Print->Score = 0.0;

   for (NumQst = 0;
	NumQst < Print->NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

      /***** Get question data *****/
      QuestionExists = Tst_GetQstDataFromDB (&Question);

      /***** Write question and answers *****/
      TstPrn_WriteQstAndAnsExam (&Gbl.Usrs.Me.UsrDat,Print,
				 NumQst,&Question,QuestionExists,
				 TstCfg_GetConfigVisibility ());

      /***** Store test exam question in database *****/
      TstPrn_StoreOneQstOfPrintInDB (Print,NumQst);

      /***** Compute total score *****/
      Print->Score += Print->PrintedQuestions[NumQst].Score;
      if (Print->PrintedQuestions[NumQst].AnswerIsNotBlank)
	 Print->NumQstsNotBlank++;

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
				       struct TstPrn_Print *Print,
				       unsigned NumQst,
				       struct Tst_Question *Question,
				       bool QuestionExists,
				       unsigned Visibility)
  {
   extern const char *Txt_Score;
   extern const char *Txt_Question_removed;
   extern const char *Txt_Question_modified;
   bool QuestionUneditedAfterExam = false;
   bool IsVisibleQstAndAnsTxt = TstVis_IsVisibleQstAndAnsTxt (Visibility);

   /***** If this question has been edited later than test time
	  ==> don't show question ****/
   if (QuestionExists)
      QuestionUneditedAfterExam = (Question->EditTime < Print->TimeUTC[Dat_START_TIME]);
   else
      QuestionUneditedAfterExam = false;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1);
   if (QuestionUneditedAfterExam)
      Tst_WriteAnswerType (Question->Answer.Type);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (QuestionExists)
     {
      if (QuestionUneditedAfterExam)
	{
	 /* Stem */
	 Tst_WriteQstStem (Question->Stem,"TEST_EXA",IsVisibleQstAndAnsTxt);

	 /* Media */
	 if (IsVisibleQstAndAnsTxt)
	    Med_ShowMedia (&Question->Media,
			   "TEST_MED_SHOW_CONT",
			   "TEST_MED_SHOW");

	 /* Answers */
	 TstPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],Question);
	 TstPrn_WriteAnswersExam (UsrDat,Print,NumQst,Question,Visibility);
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_Question_modified);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);

   /* Write score retrieved from database */
   if (TstVis_IsVisibleEachQstScore (Visibility))
     {
      HTM_DIV_Begin ("class=\"DAT_SMALL LM\"");
      HTM_TxtColonNBSP (Txt_Score);
      HTM_SPAN_Begin ("class=\"%s\"",
		      Print->PrintedQuestions[NumQst].StrAnswers[0] ?
		      (Print->PrintedQuestions[NumQst].Score > 0 ? "ANS_OK" :	// Correct/semicorrect
							           "ANS_BAD") :	// Wrong
							           "ANS_0");	// Blank answer
      HTM_Double2Decimals (Print->PrintedQuestions[NumQst].Score);
      HTM_SPAN_End ();
      HTM_DIV_End ();
     }

   /* Question feedback */
   if (QuestionUneditedAfterExam)
      if (TstVis_IsVisibleFeedbackTxt (Visibility))
	 Tst_WriteQstFeedback (Question->Feedback,"TEST_EXA_LIGHT");

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
   Print->NumQstsNotBlank = 0;

   /***** Compute and store scores of all questions *****/
   for (NumQst = 0;
	NumQst < Print->NumQsts;
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
      if (Print->PrintedQuestions[NumQst].AnswerIsNotBlank)
	 Print->NumQstsNotBlank++;

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
   /***** Get the numerical value of the correct answer *****/
   TstPrn_GetCorrectIntAnswerFromDB (Question);

   /***** Compute score *****/
   TstPrn_ComputeIntAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get the numerical value of the minimum and maximum correct answers *****/
   TstPrn_GetCorrectFltAnswerFromDB (Question);

   /***** Compute score *****/
   TstPrn_ComputeFltAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get answer true or false *****/
   TstPrn_GetCorrectTF_AnswerFromDB (Question);

   /***** Compute score *****/
   TstPrn_ComputeTF_AnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get correct options of test question from database *****/
   TstPrn_GetCorrectChoAnswerFromDB (Question);

   /***** Compute the total score of this question *****/
   TstPrn_ComputeChoAnsScore (PrintedQuestion,Question);
  }

static void TstPrn_GetCorrectAndComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				                    struct Tst_Question *Question)
  {
   /***** Get correct answers for this question from database *****/
   TstPrn_GetCorrectTxtAnswerFromDB (Question);

   /***** Compute score *****/
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

      /***** Copy answer text (row[0]) and convert it, that is in HTML, to rigorous HTML ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[0],
                Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Question->Answer.Options[NumOpt].Text,
                        Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
     }

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

   PrintedQuestion->Score = 0.0;		// Default score for blank or wrong answer
   PrintedQuestion->AnswerIsNotBlank = (PrintedQuestion->StrAnswers[0] != '\0');
   if (PrintedQuestion->AnswerIsNotBlank)	// If user has answered the answer
      if (sscanf (PrintedQuestion->StrAnswers,"%ld",&AnswerUsr) == 1)
	 if (AnswerUsr == Question->Answer.Integer)	// Correct answer
	    PrintedQuestion->Score = 1.0;
  }

void TstPrn_ComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				  const struct Tst_Question *Question)
  {
   double AnswerUsr;

   PrintedQuestion->Score = 0.0;		// Default score for blank or wrong answer
   PrintedQuestion->AnswerIsNotBlank = (PrintedQuestion->StrAnswers[0] != '\0');
   if (PrintedQuestion->AnswerIsNotBlank)	// If user has answered the answer
     {
      AnswerUsr = Str_GetDoubleFromStr (PrintedQuestion->StrAnswers);

      // A bad formatted floating point answer will interpreted as 0.0
      PrintedQuestion->Score = (AnswerUsr >= Question->Answer.FloatingPoint[0] &&
			        AnswerUsr <= Question->Answer.FloatingPoint[1]) ? 1.0 : // If correct (inside the interval)
										  0.0;  // If wrong (outside the interval)
     }
  }

void TstPrn_ComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
			        const struct Tst_Question *Question)
  {
   PrintedQuestion->AnswerIsNotBlank = (PrintedQuestion->StrAnswers[0] != '\0');
   if (PrintedQuestion->AnswerIsNotBlank)	// User has selected T or F
      PrintedQuestion->Score = (PrintedQuestion->StrAnswers[0] == Question->Answer.TF) ? 1.0 :	// Correct
					                                                -1.0;	// Wrong
   else
      PrintedQuestion->Score = 0.0;
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

   /* The answer is blank? */
   PrintedQuestion->AnswerIsNotBlank = NumAnsGood != 0 || NumAnsBad != 0;
   if (PrintedQuestion->AnswerIsNotBlank)
     {
      /* Compute the score */
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
        {
         if (NumOptTotInQst >= 2)	// It should be 2 options at least
            PrintedQuestion->Score = (double) NumAnsGood -
                                     (double) NumAnsBad / (double) (NumOptTotInQst - 1);
         else				// 0 or 1 options (impossible)
            PrintedQuestion->Score = (double) NumAnsGood;
        }
      else	// AnswerType == Tst_ANS_MULTIPLE_CHOICE
        {
         if (NumOptCorrInQst)	// There are correct options in the question
           {
            if (NumOptCorrInQst < NumOptTotInQst)	// If there are correct options and wrong options (typical case)
               PrintedQuestion->Score = (double) NumAnsGood / (double) NumOptCorrInQst -
                                        (double) NumAnsBad  / (double) (NumOptTotInQst - NumOptCorrInQst);
            else					// If all options are correct (extrange case)
               PrintedQuestion->Score = (double) NumAnsGood / (double) NumOptCorrInQst;
           }
         else
           {
            if (NumOptTotInQst)	// There are options but none is correct (extrange case)
               PrintedQuestion->Score = - (double) NumAnsBad / (double) NumOptTotInQst;
            else		// There are no options (impossible!)
               PrintedQuestion->Score = 0.0;
           }
        }
     }
   else	// Answer is blank
      PrintedQuestion->Score = 0.0;
  }

void TstPrn_ComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				 const struct Tst_Question *Question)
  {
   unsigned NumOpt;
   char TextAnsUsr[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];

   PrintedQuestion->Score = 0.0;	// Default score for blank or wrong answer
   PrintedQuestion->AnswerIsNotBlank = (PrintedQuestion->StrAnswers[0] != '\0');
   if (PrintedQuestion->AnswerIsNotBlank)	// If user has answered the answer
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,PrintedQuestion->StrAnswers,
                Tst_MAX_BYTES_ANSWERS_ONE_QST);

      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);
      Str_ConvertToComparable (TextAnsUsr);

      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
        {
         /* Filter this correct answer */
         Str_Copy (TextAnsOK,Question->Answer.Options[NumOpt].Text,
                   Tst_MAX_BYTES_ANSWERS_ONE_QST);
         Str_ConvertToComparable (TextAnsOK);

         /* Check is user answer is correct */
         if (!strcoll (TextAnsUsr,TextAnsOK))
	    PrintedQuestion->Score = 1.0;	// Correct answer
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

static void TstPrn_WriteAnswersExam (struct UsrData *UsrDat,
                                     const struct TstPrn_Print *Print,
                                     unsigned NumQst,
				     struct Tst_Question *Question,
				     unsigned Visibility)
  {
   /***** Write answer depending on type *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         TstPrn_WriteIntAnsExam    (UsrDat,Print,NumQst,Question,Visibility);
         break;
      case Tst_ANS_FLOAT:
	 TstPrn_WriteFloatAnsExam  (UsrDat,Print,NumQst,Question,Visibility);
         break;
      case Tst_ANS_TRUE_FALSE:
         TstPrn_WriteTFAnsExam     (UsrDat,Print,NumQst,Question,Visibility);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         TstPrn_WriteChoiceAnsExam (UsrDat,Print,NumQst,Question,Visibility);
         break;
      case Tst_ANS_TEXT:
         TstPrn_WriteTextAnsExam   (UsrDat,Print,NumQst,Question,Visibility);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************* Write integer answer in a test exam *********************/
/*****************************************************************************/

static void TstPrn_WriteIntAnsExam (struct UsrData *UsrDat,
                                    const struct TstPrn_Print *Print,
				    unsigned NumQst,
				    const struct Tst_Question *Question,
				    unsigned Visibility)
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
   if (Print->PrintedQuestions[NumQst].StrAnswers[0])		// If user has answered the question
     {
      if (sscanf (Print->PrintedQuestions[NumQst].StrAnswers,"%ld",&IntAnswerUsr) == 1)
	{
         HTM_TD_Begin ("class=\"%s CM\"",
		       TstVis_IsVisibleCorrectAns (Visibility) ?
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
   if (TstVis_IsVisibleQstAndAnsTxt (Visibility) &&
       TstVis_IsVisibleCorrectAns   (Visibility))
      HTM_Long (Question->Answer.Integer);
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write float answer in an test exam *********************/
/*****************************************************************************/

static void TstPrn_WriteFloatAnsExam (struct UsrData *UsrDat,
                                      const struct TstPrn_Print *Print,
				      unsigned NumQst,
				      const struct Tst_Question *Question,
				      unsigned Visibility)
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
   if (Print->PrintedQuestions[NumQst].StrAnswers[0])	// If user has answered the question
     {
      FloatAnsUsr = Str_GetDoubleFromStr (Print->PrintedQuestions[NumQst].StrAnswers);
      // A bad formatted floating point answer will interpreted as 0.0
      HTM_TD_Begin ("class=\"%s CM\"",
		    TstVis_IsVisibleCorrectAns (Visibility) ?
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
   if (TstVis_IsVisibleQstAndAnsTxt (Visibility) &&
       TstVis_IsVisibleCorrectAns   (Visibility))
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
/***************** Write false / true answer in a test exam ******************/
/*****************************************************************************/

static void TstPrn_WriteTFAnsExam (struct UsrData *UsrDat,
                                   const struct TstPrn_Print *Print,
				   unsigned NumQst,
				   const struct Tst_Question *Question,
				   unsigned Visibility)
  {
   char AnsTFUsr;

   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   AnsTFUsr = Print->PrintedQuestions[NumQst].StrAnswers[0];

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstPrn_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   HTM_TD_Begin ("class=\"%s CM\"",
		 TstVis_IsVisibleCorrectAns (Visibility) ?
		    (AnsTFUsr == Question->Answer.TF ? "ANS_OK" :
					               "ANS_BAD") :
		    "ANS_0");
   Tst_WriteAnsTF (AnsTFUsr);
   HTM_TD_End ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (TstVis_IsVisibleQstAndAnsTxt (Visibility) &&
       TstVis_IsVisibleCorrectAns   (Visibility))
      Tst_WriteAnsTF (Question->Answer.TF);
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write single or multiple choice answer in a test exam ************/
/*****************************************************************************/

static void TstPrn_WriteChoiceAnsExam (struct UsrData *UsrDat,
                                       const struct TstPrn_Print *Print,
				       unsigned NumQst,
				       struct Tst_Question *Question,
				       unsigned Visibility)
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

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->PrintedQuestions[NumQst].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (Print->PrintedQuestions[NumQst].StrAnswers,UsrAnswers);

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
         if (TstVis_IsVisibleCorrectAns (Visibility))
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
      if (TstVis_IsVisibleCorrectAns (Visibility))
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
      HTM_TD_Begin ("class=\"ANS_TXT LT\"");
      HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
      HTM_TD_End ();

      /* Answer text and feedback */
      HTM_TD_Begin ("class=\"LT\"");

      HTM_DIV_Begin ("class=\"ANS_TXT\"");
      if (TstVis_IsVisibleQstAndAnsTxt (Visibility))
	{
	 HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
	 Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
			"TEST_MED_SHOW_CONT",
			"TEST_MED_SHOW");
	}
      else
         Ico_PutIconNotVisible ();
      HTM_DIV_End ();

      if (TstVis_IsVisibleCorrectAns (Visibility))
	 if (Question->Answer.Options[Indexes[NumOpt]].Feedback)
	    if (Question->Answer.Options[Indexes[NumOpt]].Feedback[0])
	      {
	       HTM_DIV_Begin ("class=\"TEST_EXA_LIGHT\"");
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
/***************** Write text answer when assessing a test *******************/
/*****************************************************************************/

static void TstPrn_WriteTextAnsExam (struct UsrData *UsrDat,
                                     const struct TstPrn_Print *Print,
				     unsigned NumQst,
				     struct Tst_Question *Question,
				     unsigned Visibility)
  {
   unsigned NumOpt;
   char TextAnsUsr[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   bool Correct = false;

   /***** Get text and correctness of answers for this question from database (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      /***** Convert answer text, that is in HTML, to rigorous HTML ******/
      if (Question->Answer.Options[NumOpt].Text[0])
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   Question->Answer.Options[NumOpt].Text,
			   Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);

      /***** Convert answer feedback, that is in HTML, to rigorous HTML ******/
      if (TstVis_IsVisibleFeedbackTxt (Visibility))
	 if (Question->Answer.Options[NumOpt].Feedback[0])
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      Question->Answer.Options[NumOpt].Feedback,
			      Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
     }

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstPrn_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (Print->PrintedQuestions[NumQst].StrAnswers[0])	// If user has answered the question
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Print->PrintedQuestions[NumQst].StrAnswers,
                Tst_MAX_BYTES_ANSWERS_ONE_QST);

      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);

      Str_ConvertToComparable (TextAnsUsr);

      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
        {
         /* Filter this correct answer */
         Str_Copy (TextAnsOK,Question->Answer.Options[NumOpt].Text,
                   Tst_MAX_BYTES_ANSWERS_ONE_QST);
         Str_ConvertToComparable (TextAnsOK);

         /* Check is user answer is correct */
         if (!strcoll (TextAnsUsr,TextAnsOK))
           {
            Correct = true;
            break;
           }
        }
      HTM_TD_Begin ("class=\"%s CT\"",
		    TstVis_IsVisibleCorrectAns (Visibility) ?
		       (Correct ? "ANS_OK" :
				  "ANS_BAD") :
		       "ANS_0");
      HTM_Txt (Print->PrintedQuestions[NumQst].StrAnswers);
     }
   else						// If user has omitted the answer
      HTM_TD_Begin (NULL);
   HTM_TD_End ();

   /***** Write the correct answers *****/
   if (TstVis_IsVisibleQstAndAnsTxt (Visibility) &&
       TstVis_IsVisibleCorrectAns   (Visibility))
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

         if (TstVis_IsVisibleFeedbackTxt (Visibility))
	    if (Question->Answer.Options[NumOpt].Feedback)
	       if (Question->Answer.Options[NumOpt].Feedback[0])
		 {
		  HTM_DIV_Begin ("class=\"TEST_EXA_LIGHT\"");
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

void TstPrn_SelUsrsToViewUsrsExams (void)
  {
   TstPrn_PutFormToSelectUsrsToViewUsrsExams (NULL);
  }

static void TstPrn_PutFormToSelectUsrsToViewUsrsExams (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_test_results;

   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeUsrTstRes,
				     NULL,NULL,
				     Txt_Results,
				     Hlp_ASSESSMENT_Tests_results,
				     Txt_View_test_results,
				     true);	// Put form with date range
  }

/*****************************************************************************/
/******************** Select dates to show my test exams *********************/
/*****************************************************************************/

void TstPrn_SelDatesToSeeMyExams (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_test_results;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_START_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME  ] = Dat_HMS_DO_NOT_SET
     };

   /***** Begin form *****/
   Frm_StartForm (ActSeeMyTstRes);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Results,
                      NULL,NULL,
                      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);
   Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_View_test_results);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** Show my test exams ****************************/
/*****************************************************************************/

void TstPrn_ShowMyExams (void)
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
   TstPrn_ShowHeaderExams ();

   /***** List my test exams *****/
   TstCfg_GetConfigFromDB ();	// To get feedback type
   TstPrn_ShowExams (&Gbl.Usrs.Me.UsrDat);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/******************** Get users and show their test exams ********************/
/*****************************************************************************/

void TstPrn_GetUsrsAndShowExams (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  TstPrn_ShowUsrsExams,NULL,
                                  TstPrn_PutFormToSelectUsrsToViewUsrsExams,NULL);
  }

/*****************************************************************************/
/********************* Show test exams for several users *********************/
/*****************************************************************************/

static void TstPrn_ShowUsrsExams (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   const char *Ptr;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Results,
                      NULL,NULL,
		      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);

   /***** Header of the table with the list of users *****/
   TstPrn_ShowHeaderExams ();

   /***** List the test exams of the selected users *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show test exams *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    TstPrn_ShowExams (&Gbl.Usrs.Other.UsrDat);
	   }
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************ Show header of my test exams ***********************/
/*****************************************************************************/

static void TstPrn_ShowHeaderExams (void)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Non_blank_BR_questions;
   extern const char *Txt_Score;
   extern const char *Txt_Average_BR_score_BR_per_question_BR_from_0_to_1;
   extern const char *Txt_Grade;

   HTM_TR_Begin (NULL);

   HTM_TH (1,2,"CT",Txt_User[Usr_SEX_UNKNOWN]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Dat_START_TIME]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Dat_END_TIME  ]);
   HTM_TH (1,1,"RT",Txt_Questions);
   HTM_TH (1,1,"RT",Txt_Non_blank_BR_questions);
   HTM_TH (1,1,"RT",Txt_Score);
   HTM_TH (1,1,"RT",Txt_Average_BR_score_BR_per_question_BR_from_0_to_1);
   HTM_TH (1,1,"RT",Txt_Grade);
   HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show the test exams of a user in the current course ************/
/*****************************************************************************/

static void TstPrn_ShowExams (struct UsrData *UsrDat)
  {
   extern const char *Txt_View_test;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumExams;
   unsigned NumExam;
   static unsigned UniqueId = 0;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstPrn_Print Print;
   unsigned NumTotalQsts = 0;
   unsigned NumTotalQstsNotBlank = 0;
   double TotalScoreOfAllTests = 0.0;
   unsigned NumExamsVisibleByTchs = 0;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);
   struct
     {
      bool NumQsts;
      bool Score;
      bool Exam;
     } ICanView;
   char *ClassDat;

   /***** Make database query *****/
   /*           From here...                 ...to here
         ___________|_____                   _____|___________
   -----|______Exam_|_____|-----------------|_____|_Exam______|-----> time
      Start         |    End              Start   |          End
   */
   NumExams =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get test exams of a user",
			      "SELECT ExaCod,"				// row[0]
			             "UNIX_TIMESTAMP(StartTime),"	// row[1]
			             "UNIX_TIMESTAMP(EndTime),"		// row[2]
			             "NumQsts,"				// row[3]
			             "NumQstsNotBlank,"			// row[4]
			             "Sent,"				// row[5]
			             "AllowTeachers,"			// row[6]
			             "Score"				// row[7]
			      " FROM tst_exams"
			      " WHERE CrsCod=%ld AND UsrCod=%ld"
			      " AND EndTime>=FROM_UNIXTIME(%ld)"
			      " AND StartTime<=FROM_UNIXTIME(%ld)"
			      " ORDER BY ExaCod",
			      Gbl.Hierarchy.Crs.CrsCod,
			      UsrDat->UsrCod,
			      (long) Gbl.DateRange.TimeUTC[Dat_START_TIME],
			      (long) Gbl.DateRange.TimeUTC[Dat_END_TIME  ]);

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);
   Usr_ShowTableCellWithUsrData (UsrDat,NumExams);

   /***** Get and print test exams *****/
   if (NumExams)
     {
      for (NumExam = 0;
           NumExam < NumExams;
           NumExam++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get test code (row[0]) */
         TstPrn_ResetPrint (&Print);
	 if ((Print.PrnCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of test exam.");

	 /* Get if exam has been sent (row[5]) */
	 Print.Sent = (row[5][0] == 'Y');

	 /* Get if teachers are allowed to see this test exam (row[6]) */
	 Print.AllowTeachers = (row[6][0] == 'Y');
	 ClassDat = Print.AllowTeachers ? "DAT" :
	                                 "DAT_LIGHT";

	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       ICanView.NumQsts  = Print.Sent && ItsMe;
	       ICanView.Score    = Print.Sent && ItsMe &&
		                   TstVis_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	       ICanView.Exam     = Print.Sent && ItsMe;
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	    case Rol_DEG_ADM:
	    case Rol_CTR_ADM:
	    case Rol_INS_ADM:
	       ICanView.NumQsts  = Print.Sent;	// If the exam has been sent,
						// teachers can see the number of questions
	       ICanView.Score    =
	       ICanView.Exam     = Print.Sent && (ItsMe || Print.AllowTeachers);
	       break;
	    case Rol_SYS_ADM:
	       ICanView.NumQsts  =
	       ICanView.Score    =
	       ICanView.Exam     = true;
	       break;
	    default:
	       ICanView.NumQsts  =
	       ICanView.Score    =
	       ICanView.Exam     = false;
               break;
	   }

         if (NumExam)
            HTM_TR_Begin (NULL);

         /* Write date and time (row[1] and row[2] hold UTC date-times) */
         Print.TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
         Print.TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[2]);
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

         /* Get number of questions (row[3]) */
         if (sscanf (row[3],"%u",&Print.NumQsts) != 1)
            Print.NumQsts = 0;
	 if (Print.AllowTeachers)
	    NumTotalQsts += Print.NumQsts;

         /* Get number of questions not blank (row[4]) */
         if (sscanf (row[4],"%u",&Print.NumQstsNotBlank) != 1)
            Print.NumQstsNotBlank = 0;
	 if (Print.AllowTeachers)
	    NumTotalQstsNotBlank += Print.NumQstsNotBlank;

         /* Get score (row[7]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[7],"%lf",&Print.Score) != 1)
            Print.Score = 0.0;
         Str_SetDecimalPointToLocal ();	// Return to local system
	 if (Print.AllowTeachers)
	    TotalScoreOfAllTests += Print.Score;

         /* Write number of questions */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.NumQsts)
	    HTM_Unsigned (Print.NumQsts);
	 HTM_TD_End ();

         /* Write number of questions not blank */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.NumQsts)
	    HTM_Unsigned (Print.NumQstsNotBlank);
	 HTM_TD_End ();

	 /* Write score */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.Score)
	    HTM_Double2Decimals (Print.Score);
	 HTM_TD_End ();

         /* Write average score per question */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.Score)
	    HTM_Double2Decimals (Print.NumQsts ? Print.Score /
		                                 (double) Print.NumQsts :
			                         0.0);
	 HTM_TD_End ();

         /* Write grade */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanView.Score)
            TstPrn_ComputeAndShowGrade (Print.NumQsts,Print.Score,
                                        Tst_SCORE_MAX);
	 HTM_TD_End ();

	 /* Link to show this test exam */
	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanView.Exam)
	   {
	    Frm_StartForm (Gbl.Action.Act == ActSeeMyTstRes ? ActSeeOneTstResMe :
						              ActSeeOneTstResOth);
	    TstPrn_PutParamPrnCod (Print.PrnCod);
	    Ico_PutIconLink ("tasks.svg",Txt_View_test);
	    Frm_EndForm ();
	   }
	 HTM_TD_End ();
	 HTM_TR_End ();

	 if (Print.AllowTeachers)
            NumExamsVisibleByTchs++;
        }

      /***** Write totals for this user *****/
      TstPrn_ShowExamsSummaryRow (ItsMe,NumExamsVisibleByTchs,
                                  NumTotalQsts,NumTotalQstsNotBlank,
                                  TotalScoreOfAllTests);
     }
   else
     {
      HTM_TD_ColouredEmpty (8);
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

static void TstPrn_ShowExamsSummaryRow (bool ItsMe,
                                        unsigned NumExams,
                                        unsigned NumTotalQsts,
                                        unsigned NumTotalQstsNotBlank,
                                        double TotalScoreOfAllTests)
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
			      NumExams;
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
   HTM_TD_Begin ("colspan=\"2\" class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtColonNBSP (Txt_Visible_tests);
   HTM_Unsigned (NumExams);
   HTM_TD_End ();

   /***** Write total number of questions *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (NumExams)
      HTM_Unsigned (NumTotalQsts);
   HTM_TD_End ();

   /***** Write total number of questions not blank *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (NumExams)
      HTM_Unsigned (NumTotalQstsNotBlank);
   HTM_TD_End ();

   /***** Write total score *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      HTM_Double2Decimals (TotalScoreOfAllTests);
   HTM_TD_End ();

   /***** Write average score per question *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      HTM_Double2Decimals (NumTotalQsts ? TotalScoreOfAllTests / (double) NumTotalQsts :
			         0.0);
   HTM_TD_End ();

   /***** Write score over Tst_SCORE_MAX *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      TstPrn_ComputeAndShowGrade (NumTotalQsts,TotalScoreOfAllTests,
                                  Tst_SCORE_MAX);
   HTM_TD_End ();

   /***** Last cell *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Show one test exam of another user *********************/
/*****************************************************************************/

void TstPrn_ShowOneExam (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Test_result;
   extern const char *Txt_The_user_does_not_exist;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_non_blank_QUESTIONS;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_Tags;
   struct TstPrn_Print Print;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   bool ItsMe;
   bool ICanViewTest;
   bool ICanViewScore;

   /***** Get the code of the test *****/
   TstPrn_ResetPrint (&Print);
   if ((Print.PrnCod = TstPrn_GetParamPrnCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of test is missing.");

   /***** Get test exam data *****/
   TstPrn_GetPrintDataByPrnCod (&Print);
   TstCfg_SetConfigVisibility (TstVis_MAX_VISIBILITY);

   /***** Check if I can view this test exam *****/
   ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewTest = ItsMe;
	 if (ItsMe)
	   {
	    TstCfg_GetConfigFromDB ();	// To get feedback type
	    ICanViewScore = TstVis_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	   }
	 else
	    ICanViewScore = false;
	 break;
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 switch (Gbl.Action.Act)
	   {
	    case ActSeeOneTstResMe:
	       ICanViewTest  =
	       ICanViewScore = ItsMe;
	       break;
	    case ActSeeOneTstResOth:
	       ICanViewTest  =
	       ICanViewScore = ItsMe ||
			       Print.AllowTeachers;
	       break;
	    default:
	       ICanViewTest  =
	       ICanViewScore = false;
	       break;
	   }
	 break;
      case Rol_SYS_ADM:
	 ICanViewTest  =
	 ICanViewScore = true;
	 break;
      default:
	 ICanViewTest  =
	 ICanViewScore = false;
	 break;
     }

   if (ICanViewTest)	// I am allowed to view this test exam
     {
      /***** Get questions and user's answers of the test exam from database *****/
      TstPrn_GetPrintQuestionsFromDB (&Print);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Txt_Test_result,
                    NULL,NULL,
                    Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (5);

      /***** Header row *****/
      /* Get data of the user who made the test */
      if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 Lay_ShowErrorAndExit (Txt_The_user_does_not_exist);
      if (!Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
         Lay_NoPermissionExit ();

      /* User */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role][Gbl.Usrs.Other.UsrDat.Sex]);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
      HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname1);
      if (Gbl.Usrs.Other.UsrDat.Surname2[0])
	 HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname2);
      if (Gbl.Usrs.Other.UsrDat.FirstName[0])
	 HTM_TxtF (", %s",Gbl.Usrs.Other.UsrDat.FirstName);
      HTM_BR ();
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Other.UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,ShowPhoto ? PhotoURL :
							   NULL,
			"PHOTO45x60",Pho_ZOOM,false);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Test date */
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 if (asprintf (&Id,"tst_date_%u",(unsigned) StartEndTime) < 0)
	    Lay_NotEnoughMemoryExit ();

	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"DAT_N RT\"");
	 HTM_TxtF ("%s:",Txt_START_END_TIME[StartEndTime]);
	 HTM_TD_End ();

	 HTM_TD_Begin ("id=\"%s\" class=\"DAT LT\"",Id);
	 Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,true,0x7);
	 HTM_TD_End ();

	 HTM_TR_End ();

	 free (Id);
	}

      /* Number of questions */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Questions);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      HTM_TxtF ("%u (%u %s)",
	        Print.NumQsts,
	        Print.NumQstsNotBlank,Txt_non_blank_QUESTIONS);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Score */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Score);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
	 HTM_Double2Decimals (Print.Score);
      else
         Ico_PutIconNotVisible ();
      HTM_TD_End ();

      /* Grade */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Grade);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
         TstPrn_ComputeAndShowGrade (Print.NumQsts,Print.Score,
                                     Tst_SCORE_MAX);
      else
         Ico_PutIconNotVisible ();
      HTM_TD_End ();

      HTM_TR_End ();

      /* Tags present in this test */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Tags);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      TstPrn_ShowTagsPresentInAnExam (Print.PrnCod);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write answers and solutions *****/
      TstPrn_ShowExamAnswers (&Gbl.Usrs.Other.UsrDat,&Print,
			      TstCfg_GetConfigVisibility ());

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write total mark of test *****/
      if (ICanViewScore)
	{
	 HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	 HTM_TxtColonNBSP (Txt_Score);
	 HTM_Double2Decimals (Print.Score);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Grade);
         TstPrn_ComputeAndShowGrade (Print.NumQsts,Print.Score,
                                     Tst_SCORE_MAX);
	 HTM_DIV_End ();
	}

      /***** End box *****/
      Box_BoxEnd ();
     }
   else	// I am not allowed to view this test exam
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/********************* Show test tags in this test exam **********************/
/*****************************************************************************/

static void TstPrn_ShowTagsPresentInAnExam (long ResCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumTags;

   /***** Get all tags of questions in this test *****/
   NumTags = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get tags"
					" present in a test exam",
			     "SELECT tst_tags.TagTxt"	// row[0]
			     " FROM"
			     " (SELECT DISTINCT(tst_question_tags.TagCod)"
			     " FROM tst_question_tags,tst_exam_questions"
			     " WHERE tst_exam_questions.ExaCod=%ld"
			     " AND tst_exam_questions.QstCod=tst_question_tags.QstCod)"
			     " AS TagsCods,tst_tags"
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

void TstPrn_ShowExamAnswers (struct UsrData *UsrDat,
			     struct TstPrn_Print *Print,
			     unsigned Visibility)
  {
   unsigned NumQst;
   struct Tst_Question Question;
   bool QuestionExists;

   for (NumQst = 0;
	NumQst < Print->NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

      /***** Get question data *****/
      QuestionExists = Tst_GetQstDataFromDB (&Question);

      /***** Write questions and answers *****/
      TstPrn_WriteQstAndAnsExam (UsrDat,Print,
                                 NumQst,&Question,QuestionExists,
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
		       " WHERE ExaCod=%ld AND CrsCod=%ld",
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
      if (sscanf (row[3],"%u",&Print->NumQsts) != 1)
	 Print->NumQsts = 0;

      /* Get number of questions not blank (row[4]) */
      if (sscanf (row[4],"%u",&Print->NumQstsNotBlank) != 1)
	 Print->NumQstsNotBlank = 0;

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
   if (NumQsts == Print->NumQsts)
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
		   Tst_MAX_BYTES_INDEXES_ONE_QST);

	 /* Get answers selected by user for this question (row[3]) */
	 Str_Copy (Print->PrintedQuestions[NumQst].StrAnswers,row[3],
		   Tst_MAX_BYTES_ANSWERS_ONE_QST);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumQsts != Print->NumQsts)
      Lay_WrongExamExit ();
  }

/*****************************************************************************/
/********************** Remove test exams made by a user *********************/
/*****************************************************************************/

void TstPrn_RemoveExamsMadeByUsrInAllCrss (long UsrCod)
  {
   /***** Remove test exams made by the specified user *****/
   DB_QueryDELETE ("can not remove test exams made by a user",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,tst_exam_questions"
                   " WHERE tst_exams.UsrCod=%ld"
                   " AND tst_exams.ExaCod=tst_exam_questions.ExaCod",
		   UsrCod);

   DB_QueryDELETE ("can not remove test exams made by a user",
		   "DELETE FROM tst_exams"
	           " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*************** Remove test exams made by a user in a course ****************/
/*****************************************************************************/

void TstPrn_RemoveExamsMadeByUsrInCrs (long UsrCod,long CrsCod)
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
/******************* Remove all test exams made in a course ******************/
/*****************************************************************************/

void TstPrn_RemoveCrsExams (long CrsCod)
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
