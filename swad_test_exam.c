// swad_test_exam.c: test exams made by users

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
#include "swad_test.h"
#include "swad_test_exam.h"
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

#define Tst_NUM_STATUS 2
typedef enum
  {
   Tst_STATUS_SHOWN_BUT_NOT_ASSESSED	= 0,
   Tst_STATUS_ASSESSED			= 1,
   Tst_STATUS_ERROR			= 2,
  } Tst_Status_t;

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

static void TstExa_ComputeAnswerScore (struct TstExa_Exam *Exam,
				       unsigned NumQst,
				       struct Tst_Question *Question);
static void TstExa_ComputeIntAnsScore (struct TstExa_Exam *Exam,
				       unsigned NumQst,
				       struct Tst_Question *Question);
static void TstExa_GetCorrectIntAnswerFromDB (struct Tst_Question *Question);
static void TstExa_ComputeFloatAnsScore (struct TstExa_Exam *Exam,
				         unsigned NumQst,
				         struct Tst_Question *Question);
static void TstExa_GetCorrectFloatAnswerFromDB (struct Tst_Question *Question);
static void TstExa_ComputeTFAnsScore (struct TstExa_Exam *Exam,
				      unsigned NumQst,
				      struct Tst_Question *Question);
static void TstExa_GetCorrectTFAnswerFromDB (struct Tst_Question *Question);
static void TstExa_GetCorrectChoiceAnswerFromDB (struct Tst_Question *Question);

static void TstExa_ComputeScoreQst (struct TstExa_Exam *Exam,
                                    unsigned NumQst,
	                            const struct Tst_Question *Question,
                                    unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION],	// Indexes of all answers of this question
                                    bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION]);
static void TstExa_ComputeTextAnsScore (struct TstExa_Exam *Exam,
				        unsigned NumQst,
				        struct Tst_Question *Question);
static void TstExa_GetCorrectTextAnswerFromDB (struct Tst_Question *Question);

static void TstExa_WriteAnswersExam (struct UsrData *UsrDat,
                                     const struct TstExa_Exam *Exam,
                                     unsigned NumQst,
				     struct Tst_Question *Question,
				     unsigned Visibility);
static void TstExa_WriteIntAnsExam (struct UsrData *UsrDat,
                                    const struct TstExa_Exam *Exam,
				    unsigned NumQst,
				    const struct Tst_Question *Question,
				    MYSQL_RES *mysql_res,
				    unsigned Visibility);
static void TstExa_WriteFloatAnsExam (struct UsrData *UsrDat,
                                      const struct TstExa_Exam *Exam,
				      unsigned NumQst,
				      const struct Tst_Question *Question,
				      MYSQL_RES *mysql_res,
				      unsigned Visibility);
static void TstExa_WriteTFAnsExam (struct UsrData *UsrDat,
                                   const struct TstExa_Exam *Exam,
				   unsigned NumQst,
				   const struct Tst_Question *Question,
				   MYSQL_RES *mysql_res,
				   unsigned Visibility);
static void TstExa_WriteChoiceAnsExam (struct UsrData *UsrDat,
                                       const struct TstExa_Exam *Exam,
				       unsigned NumQst,
				       struct Tst_Question *Question,
				       MYSQL_RES *mysql_res,
				       unsigned Visibility);
static void TstExa_WriteTextAnsExam (struct UsrData *UsrDat,
                                     const struct TstExa_Exam *Exam,
				     unsigned NumQst,
				     struct Tst_Question *Question,
				     MYSQL_RES *mysql_res,
				     unsigned Visibility);
static void TstExa_WriteHeadUserCorrect (struct UsrData *UsrDat);
static void TstExa_WriteScoreStart (unsigned ColSpan);
static void TstExa_WriteScoreEnd (void);

static void TstExa_StoreOneExamQstInDB (const struct TstExa_Exam *Exam,
                                        unsigned NumQst);
static void Tst_UpdateQstScoreInDB (const struct TstExa_Exam *Exam,unsigned NumQst);

static void TstExa_ShowUsrsExams (void);
static void TstExa_ShowHeaderExams (void);
static void TstExa_ShowExams (struct UsrData *UsrDat);
static void TstExa_ShowExamsSummaryRow (bool ItsMe,
                                        unsigned NumExams,
                                        unsigned NumTotalQsts,
                                        unsigned NumTotalQstsNotBlank,
                                        double TotalScoreOfAllTests);
static void TstExa_ShowTagsPresentInAnExam (long ExaCod);

/*****************************************************************************/
/******************************** Reset exam *********************************/
/*****************************************************************************/

void TstExa_ResetExam (struct TstExa_Exam *Exam)
  {
   Exam->ExaCod                  = -1L;
   Exam->TimeUTC[Dat_START_TIME] =
   Exam->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Exam->NumQsts                 =
   Exam->NumQstsNotBlank         = 0;
   Exam->AllowTeachers           = false;
   Exam->Score                   = 0.0;
  }

/*****************************************************************************/
/***************** Create new blank test exam in database ********************/
/*****************************************************************************/

void TstExa_CreateExamInDB (struct TstExa_Exam *Exam)
  {
   /***** Insert new test exam into table *****/
   Exam->ExaCod =
   DB_QueryINSERTandReturnCode ("can not create new test exam",
				"INSERT INTO tst_exams"
				" (CrsCod,UsrCod,StartTime,EndTime,NumQsts,AllowTeachers,Score)"
				" VALUES"
				" (%ld,%ld,NOW(),NOW(),%u,'N',0)",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Exam->NumQsts);
  }

/*****************************************************************************/
/********************** Update test exam in database *************************/
/*****************************************************************************/

void TstExa_UpdateExamInDB (const struct TstExa_Exam *Exam)
  {
   /***** Update score in test exam *****/
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot
   DB_QueryUPDATE ("can not update test exam",
		   "UPDATE tst_exams"
	           " SET EndTime=NOW(),"
	                "NumQstsNotBlank=%u,"
		        "AllowTeachers='%c',"
	                "Score='%.15lg'"
	           " WHERE ExaCod=%ld"
	           " AND CrsCod=%ld AND UsrCod=%ld",	// Extra checks
		   Exam->NumQstsNotBlank,
		   Exam->AllowTeachers ? 'Y' :
			                 'N',
		   Exam->Score,
		   Exam->ExaCod,
		   Gbl.Hierarchy.Crs.CrsCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/********************* Show test exam after assessing it *********************/
/*****************************************************************************/

void TstExa_ShowExamAfterAssess (struct TstExa_Exam *Exam)
  {
   extern const char *Txt_Question_removed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

   /***** Initialize score and number of questions not blank *****/
   Exam->NumQstsNotBlank = 0;
   Exam->Score = 0.0;

   for (NumQst = 0;
	NumQst < Exam->NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Query database *****/
      if (Tst_GetOneQuestionByCod (Exam->Questions[NumQst].QstCod,&mysql_res))	// Question exists
	{
	 /***** Write question and answers *****/
	 row = mysql_fetch_row (mysql_res);
	 TstExa_WriteQstAndAnsExam (&Gbl.Usrs.Me.UsrDat,
				    Exam,
				    NumQst,
				    row,
				    TstCfg_GetConfigVisibility ());

	 /***** Store test exam question in database *****/
	 TstExa_StoreOneExamQstInDB (Exam,
				     NumQst);	// 0, 1, 2, 3...

	 /***** Compute total score *****/
	 Exam->Score += Exam->Questions[NumQst].Score;
	 if (Exam->Questions[NumQst].AnswerIsNotBlank)
	    Exam->NumQstsNotBlank++;

	 /***** Update the number of accesses and the score of this question *****/
	 if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	    Tst_UpdateQstScoreInDB (Exam,NumQst);
	}
      else
	{
	 /***** Question does not exists *****/
         HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
         Tst_WriteNumQst (NumQst + 1);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT_LIGHT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Txt_Question_removed);
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

void TstExa_WriteQstAndAnsExam (struct UsrData *UsrDat,
				struct TstExa_Exam *Exam,
				unsigned NumQst,
				MYSQL_ROW row,
				unsigned Visibility)
  {
   struct Tst_Question Question;
   bool IsVisibleQstAndAnsTxt = TstVis_IsVisibleQstAndAnsTxt (Visibility);
   /*
   row[0] UNIX_TIMESTAMP(EditTime)
   row[1] AnsType
   row[2] Shuffle
   row[3] Stem
   row[4] Feedback
   row[5] MedCod
   row[6] NumHits
   row[7] NumHitsNotBlank
   row[8] Score
   */

   /***** Create test question *****/
   Tst_QstConstructor (&Question);
   Question.QstCod = Exam->Questions[NumQst].QstCod;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1);
   Question.Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
   Tst_WriteAnswerType (Question.Answer.Type);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Stem (row[3]) */
   Tst_WriteQstStem (row[3],"TEST_EXA",IsVisibleQstAndAnsTxt);

   /* Media (row[5]) */
   if (IsVisibleQstAndAnsTxt)
     {
      Question.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
      Med_GetMediaDataByCod (&Question.Media);
      Med_ShowMedia (&Question.Media,
		     "TEST_MED_SHOW_CONT",
		     "TEST_MED_SHOW");
     }

   /* Answers */
   TstExa_ComputeAnswerScore (Exam,NumQst,&Question);
   TstExa_WriteAnswersExam (UsrDat,
                            Exam,NumQst,&Question,
			    Visibility);

   /* Question feedback (row[4]) */
   if (TstVis_IsVisibleFeedbackTxt (Visibility))
      Tst_WriteQstFeedback (row[4],"TEST_EXA_LIGHT");

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();

   /***** Destroy test question *****/
   Tst_QstDestructor (&Question);
  }

/*****************************************************************************/
/*********** Compute score of each question and store in database ************/
/*****************************************************************************/

void TstExa_ComputeScoresAndStoreExamQuestions (struct TstExa_Exam *Exam,
                                                bool UpdateQstScore)
  {
   unsigned NumQst;
   struct Tst_Question Question;

   /***** Initialize total score *****/
   Exam->Score = 0.0;
   Exam->NumQstsNotBlank = 0;

   /***** Compute and store scores of all questions *****/
   for (NumQst = 0;
	NumQst < Exam->NumQsts;
	NumQst++)
     {
      /* Compute question score */
      Tst_QstConstructor (&Question);
      Question.QstCod = Exam->Questions[NumQst].QstCod;
      Question.Answer.Type = Tst_GetQstAnswerType (Question.QstCod);
      TstExa_ComputeAnswerScore (Exam,NumQst,&Question);
      Tst_QstDestructor (&Question);

      /* Store test exam question in database */
      TstExa_StoreOneExamQstInDB (Exam,
				  NumQst);	// 0, 1, 2, 3...

      /* Accumulate total score */
      Exam->Score += Exam->Questions[NumQst].Score;
      if (Exam->Questions[NumQst].AnswerIsNotBlank)
	 Exam->NumQstsNotBlank++;


      /* Update the number of hits and the score of this question in tests database */
      if (UpdateQstScore)
	 Tst_UpdateQstScoreInDB (Exam,NumQst);
     }
  }

/*****************************************************************************/
/************* Write answers of a question when assessing a test *************/
/*****************************************************************************/

static void TstExa_ComputeAnswerScore (struct TstExa_Exam *Exam,
				       unsigned NumQst,
				       struct Tst_Question *Question)
  {
   /***** Write answer depending on type *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         TstExa_ComputeIntAnsScore    (Exam,NumQst,Question);
         break;
      case Tst_ANS_FLOAT:
	 TstExa_ComputeFloatAnsScore  (Exam,NumQst,Question);
         break;
      case Tst_ANS_TRUE_FALSE:
         TstExa_ComputeTFAnsScore     (Exam,NumQst,Question);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         TstExa_ComputeChoiceAnsScore (Exam,NumQst,Question);
         break;
      case Tst_ANS_TEXT:
         TstExa_ComputeTextAnsScore   (Exam,NumQst,Question);
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/**************** Write integer answer when assessing a test *****************/
/*****************************************************************************/

static void TstExa_ComputeIntAnsScore (struct TstExa_Exam *Exam,
				       unsigned NumQst,
				       struct Tst_Question *Question)
  {
   long AnswerUsr;

   /***** Get the numerical value of the correct answer *****/
   TstExa_GetCorrectIntAnswerFromDB (Question);

   /***** Compute score *****/
   Exam->Questions[NumQst].Score = 0.0;		// Default score for blank or wrong answer
   Exam->Questions[NumQst].AnswerIsNotBlank = (Exam->Questions[NumQst].StrAnswers[0] != '\0');
   if (Exam->Questions[NumQst].AnswerIsNotBlank)	// If user has answered the answer
      if (sscanf (Exam->Questions[NumQst].StrAnswers,"%ld",&AnswerUsr) == 1)
	 if (AnswerUsr == Question->Answer.Integer)	// Correct answer
	    Exam->Questions[NumQst].Score = 1.0;
  }

static void TstExa_GetCorrectIntAnswerFromDB (struct Tst_Question *Question)
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

/*****************************************************************************/
/***************** Write float answer when assessing a test ******************/
/*****************************************************************************/

static void TstExa_ComputeFloatAnsScore (struct TstExa_Exam *Exam,
				         unsigned NumQst,
				         struct Tst_Question *Question)
  {
   double AnswerUsr;

   /***** Get the numerical value of the minimum and maximum correct answers *****/
   TstExa_GetCorrectFloatAnswerFromDB (Question);

   /***** Compute score *****/
   Exam->Questions[NumQst].Score = 0.0;		// Default score for blank or wrong answer
   Exam->Questions[NumQst].AnswerIsNotBlank = (Exam->Questions[NumQst].StrAnswers[0] != '\0');
   if (Exam->Questions[NumQst].AnswerIsNotBlank)	// If user has answered the answer
     {
      AnswerUsr = Str_GetDoubleFromStr (Exam->Questions[NumQst].StrAnswers);
      // A bad formatted floating point answer will interpreted as 0.0
      Exam->Questions[NumQst].Score = (AnswerUsr >= Question->Answer.FloatingPoint[0] &&
				       AnswerUsr <= Question->Answer.FloatingPoint[1]) ? 1.0 : // If correct (inside the interval)
											 0.0;  // If wrong (outside the interval)
     }
  }

static void TstExa_GetCorrectFloatAnswerFromDB (struct Tst_Question *Question)
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

/*****************************************************************************/
/************** Write false / true answer when assessing a test **************/
/*****************************************************************************/

static void TstExa_ComputeTFAnsScore (struct TstExa_Exam *Exam,
				      unsigned NumQst,
				      struct Tst_Question *Question)
  {
   /***** Get answer true or false *****/
   TstExa_GetCorrectTFAnswerFromDB (Question);

   /***** Compute score *****/
   Exam->Questions[NumQst].AnswerIsNotBlank = (Exam->Questions[NumQst].StrAnswers[0] != '\0');
   if (Exam->Questions[NumQst].AnswerIsNotBlank)	// User has selected T or F
      Exam->Questions[NumQst].Score = (Exam->Questions[NumQst].StrAnswers[0] == Question->Answer.TF) ? 1.0 :	// Correct
					                                                                  -1.0;	// Wrong
   else
      Exam->Questions[NumQst].Score = 0.0;
  }

static void TstExa_GetCorrectTFAnswerFromDB (struct Tst_Question *Question)
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

/*****************************************************************************/
/************ Compute score for single or multiple choice answer *************/
/*****************************************************************************/

void TstExa_ComputeChoiceAnsScore (struct TstExa_Exam *Exam,
				   unsigned NumQst,
				   struct Tst_Question *Question)
  {
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION];

   /***** Get correct options of test question from database *****/
   TstExa_GetCorrectChoiceAnswerFromDB (Question);

   /***** Get indexes for this question from string *****/
   TstExa_GetIndexesFromStr (Exam->Questions[NumQst].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstExa_GetAnswersFromStr (Exam->Questions[NumQst].StrAnswers,UsrAnswers);

   /***** Compute the total score of this question *****/
   TstExa_ComputeScoreQst (Exam,NumQst,Question,Indexes,UsrAnswers);
  }

static void TstExa_GetCorrectChoiceAnswerFromDB (struct Tst_Question *Question)
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

/*****************************************************************************/
/********************* Get vector of indexes from string *********************/
/*****************************************************************************/

void TstExa_GetIndexesFromStr (const char StrIndexesOneQst[TstExa_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
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
      Par_GetNextStrUntilSeparParamMult (&Ptr,StrOneIndex,Cns_MAX_DECIMAL_DIGITS_UINT);

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
/****************** Get vector of user's answers from string *****************/
/*****************************************************************************/

void TstExa_GetAnswersFromStr (const char StrAnswersOneQst[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1],
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
      Par_GetNextStrUntilSeparParamMult (&Ptr,StrOneAnswer,Cns_MAX_DECIMAL_DIGITS_UINT);

      if (sscanf (StrOneAnswer,"%u",&AnsUsr) != 1)
	 Lay_ShowErrorAndExit ("Bad user's answer.");

      if (AnsUsr >= Tst_MAX_OPTIONS_PER_QUESTION)
	 Lay_ShowErrorAndExit ("Bad user's answer.");

      UsrAnswers[AnsUsr] = true;
     }
  }

/*****************************************************************************/
/*********************** Compute the score of a question *********************/
/*****************************************************************************/

static void TstExa_ComputeScoreQst (struct TstExa_Exam *Exam,
                                    unsigned NumQst,
	                            const struct Tst_Question *Question,
                                    unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION],	// Indexes of all answers of this question
                                    bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION])
  {
   unsigned NumOpt;
   unsigned NumOptTotInQst = 0;
   unsigned NumOptCorrInQst = 0;
   unsigned NumAnsGood = 0;
   unsigned NumAnsBad = 0;

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
   Exam->Questions[NumQst].AnswerIsNotBlank = NumAnsGood != 0 || NumAnsBad != 0;
   if (Exam->Questions[NumQst].AnswerIsNotBlank)
     {
      /* Compute the score */
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE)
        {
         if (NumOptTotInQst >= 2)	// It should be 2 options at least
            Exam->Questions[NumQst].Score = (double) NumAnsGood -
                                            (double) NumAnsBad / (double) (NumOptTotInQst - 1);
         else			// 0 or 1 options (impossible)
            Exam->Questions[NumQst].Score = (double) NumAnsGood;
        }
      else	// AnswerType == Tst_ANS_MULTIPLE_CHOICE
        {
         if (NumOptCorrInQst)	// There are correct options in the question
           {
            if (NumOptCorrInQst < NumOptTotInQst)	// If there are correct options and wrong options (typical case)
               Exam->Questions[NumQst].Score = (double) NumAnsGood / (double) NumOptCorrInQst -
                                               (double) NumAnsBad / (double) (NumOptTotInQst - NumOptCorrInQst);
            else					// Si todas the opciones son correctas (caso raro)
               Exam->Questions[NumQst].Score = (double) NumAnsGood / (double) NumOptCorrInQst;
           }
         else
           {
            if (NumOptTotInQst)	// There are options but none is correct (extrange case)
               Exam->Questions[NumQst].Score = - (double) NumAnsBad / (double) NumOptTotInQst;
            else			// There are no options (impossible!)
               Exam->Questions[NumQst].Score = 0.0;
           }
        }
     }
   else	// Answer is blank
      Exam->Questions[NumQst].Score = 0.0;
  }

/*****************************************************************************/
/********************* Compute score for text answer *************************/
/*****************************************************************************/

static void TstExa_ComputeTextAnsScore (struct TstExa_Exam *Exam,
				        unsigned NumQst,
				        struct Tst_Question *Question)
  {
   unsigned NumOpt;
   char TextAnsUsr[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1];

   /***** Get correct answers for this question from database *****/
   TstExa_GetCorrectTextAnswerFromDB (Question);

   /***** Compute score *****/
   Exam->Questions[NumQst].Score = 0.0;	// Default score for blank or wrong answer
   Exam->Questions[NumQst].AnswerIsNotBlank = (Exam->Questions[NumQst].StrAnswers[0] != '\0');
   if (Exam->Questions[NumQst].AnswerIsNotBlank)	// If user has answered the answer
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Exam->Questions[NumQst].StrAnswers,
                TstExa_MAX_BYTES_ANSWERS_ONE_QST);

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
                   TstExa_MAX_BYTES_ANSWERS_ONE_QST);
         Str_ConvertToComparable (TextAnsOK);

         /* Check is user answer is correct */
         if (!strcoll (TextAnsUsr,TextAnsOK))
	    Exam->Questions[NumQst].Score = 1.0;	// Correct answer
        }
     }
  }

static void TstExa_GetCorrectTextAnswerFromDB (struct Tst_Question *Question)
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
/************ Compute and show total grade out of maximum grade **************/
/*****************************************************************************/

void TstExa_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade)
  {
   TstExa_ShowGrade (TstExa_ComputeGrade (NumQsts,Score,MaxGrade),MaxGrade);
  }

/*****************************************************************************/
/**************** Compute total grade out of maximum grade *******************/
/*****************************************************************************/

double TstExa_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade)
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

void TstExa_ShowGrade (double Grade,double MaxGrade)
  {
   /***** Write grade over maximum grade *****/
   HTM_Double2Decimals (Grade);
   HTM_Txt ("/");
   HTM_Double2Decimals (MaxGrade);
  }

/*****************************************************************************/
/************* Write answers of a question when assessing a test *************/
/*****************************************************************************/

static void TstExa_WriteAnswersExam (struct UsrData *UsrDat,
                                     const struct TstExa_Exam *Exam,
                                     unsigned NumQst,
				     struct Tst_Question *Question,
				     unsigned Visibility)
  {
   MYSQL_RES *mysql_res;

   /***** Get answer of a question from database *****/
   Tst_GetAnswersQst (Question,&mysql_res,
                      false);	// Don't shuffle
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   /***** Write answer depending on type *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         TstExa_WriteIntAnsExam    (UsrDat,Exam,
                                    NumQst,Question,mysql_res,
				    Visibility);
         break;
      case Tst_ANS_FLOAT:
	 TstExa_WriteFloatAnsExam  (UsrDat,Exam,
	                            NumQst,Question,mysql_res,
				    Visibility);
         break;
      case Tst_ANS_TRUE_FALSE:
         TstExa_WriteTFAnsExam     (UsrDat,Exam,
                                    NumQst,Question,mysql_res,
				    Visibility);
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
         TstExa_WriteChoiceAnsExam (UsrDat,Exam,
                                    NumQst,Question,mysql_res,
				    Visibility);
         break;
      case Tst_ANS_TEXT:
         TstExa_WriteTextAnsExam   (UsrDat,Exam,
                                    NumQst,Question,mysql_res,
				    Visibility);
         break;
      default:
         break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Write integer answer in a test exam *********************/
/*****************************************************************************/

static void TstExa_WriteIntAnsExam (struct UsrData *UsrDat,
                                    const struct TstExa_Exam *Exam,
				    unsigned NumQst,
				    const struct Tst_Question *Question,
				    MYSQL_RES *mysql_res,
				    unsigned Visibility)
  {
   MYSQL_ROW row;
   long IntAnswerUsr;
   long IntAnswerCorr;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get the numerical value of the correct answer *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[1],"%ld",&IntAnswerCorr) != 1)
      Lay_ShowErrorAndExit ("Wrong integer answer.");

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstExa_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (Exam->Questions[NumQst].StrAnswers[0])		// If user has answered the question
     {
      if (sscanf (Exam->Questions[NumQst].StrAnswers,"%ld",&IntAnswerUsr) == 1)
	{
         HTM_TD_Begin ("class=\"%s CM\"",
		       TstVis_IsVisibleCorrectAns (Visibility) ?
			  (IntAnswerUsr == IntAnswerCorr ? "ANS_OK" :
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
      HTM_Long (IntAnswerCorr);
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write the score of this question *****/
   if (TstVis_IsVisibleEachQstScore (Visibility))
     {
      TstExa_WriteScoreStart (2);
      if (!Exam->Questions[NumQst].StrAnswers[0])	// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (IntAnswerUsr == IntAnswerCorr)	// If correct
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else					// If wrong
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (0.0);
	}
      HTM_SPAN_End ();
      TstExa_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write float answer in an test exam *********************/
/*****************************************************************************/

static void TstExa_WriteFloatAnsExam (struct UsrData *UsrDat,
                                      const struct TstExa_Exam *Exam,
				      unsigned NumQst,
				      const struct Tst_Question *Question,
				      MYSQL_RES *mysql_res,
				      unsigned Visibility)
  {
   MYSQL_ROW row;
   unsigned i;
   double FloatAnsUsr = 0.0,Tmp;
   double FloatAnsCorr[2];
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Lay_ShowErrorAndExit ("Wrong float range.");

   /***** Get the numerical value of the minimum and maximum correct answers *****/
   for (i = 0;
	i < 2;
	i++)
     {
      row = mysql_fetch_row (mysql_res);
      FloatAnsCorr[i] = Str_GetDoubleFromStr (row[1]);
     }
   if (FloatAnsCorr[0] > FloatAnsCorr[1]) 	// The maximum and the minimum are swapped
    {
      /* Swap maximum and minimum */
      Tmp = FloatAnsCorr[0];
      FloatAnsCorr[0] = FloatAnsCorr[1];
      FloatAnsCorr[1] = Tmp;
     }

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstExa_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (Exam->Questions[NumQst].StrAnswers[0])	// If user has answered the question
     {
      FloatAnsUsr = Str_GetDoubleFromStr (Exam->Questions[NumQst].StrAnswers);
      // A bad formatted floating point answer will interpreted as 0.0
      HTM_TD_Begin ("class=\"%s CM\"",
		    TstVis_IsVisibleCorrectAns (Visibility) ?
		       ((FloatAnsUsr >= FloatAnsCorr[0] &&
			 FloatAnsUsr <= FloatAnsCorr[1]) ? "ANS_OK" :
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
      HTM_Double (FloatAnsCorr[0]);
      HTM_Txt ("; ");
      HTM_Double (FloatAnsCorr[1]);
      HTM_Txt ("]");
     }
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write the score of this question *****/
   if (TstVis_IsVisibleEachQstScore (Visibility))
     {
      TstExa_WriteScoreStart (2);
      if (!Exam->Questions[NumQst].StrAnswers[0])	// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (FloatAnsUsr >= FloatAnsCorr[0] &&
               FloatAnsUsr <= FloatAnsCorr[1])		// If correct (inside the interval)
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else						// If wrong (outside the interval)
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (0.0);
	}
      HTM_SPAN_End ();
      TstExa_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Write false / true answer in a test exam ******************/
/*****************************************************************************/

static void TstExa_WriteTFAnsExam (struct UsrData *UsrDat,
                                   const struct TstExa_Exam *Exam,
				   unsigned NumQst,
				   const struct Tst_Question *Question,
				   MYSQL_RES *mysql_res,
				   unsigned Visibility)
  {
   MYSQL_ROW row;
   char AnsTF;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   /***** Check if number of rows is correct *****/
   Tst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   row = mysql_fetch_row (mysql_res);
   AnsTF = Exam->Questions[NumQst].StrAnswers[0];

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstExa_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   HTM_TD_Begin ("class=\"%s CM\"",
		 TstVis_IsVisibleCorrectAns (Visibility) ?
		    (AnsTF == row[1][0] ? "ANS_OK" :
					  "ANS_BAD") :
		    "ANS_0");
   Tst_WriteAnsTF (AnsTF);
   HTM_TD_End ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"ANS_0 CM\"");
   if (TstVis_IsVisibleQstAndAnsTxt (Visibility) &&
       TstVis_IsVisibleCorrectAns   (Visibility))
      Tst_WriteAnsTF (row[1][0]);
   else
      Ico_PutIconNotVisible ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write the score of this question *****/
   if (TstVis_IsVisibleEachQstScore (Visibility))
     {
      TstExa_WriteScoreStart (2);
      if (AnsTF == '\0')		// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (AnsTF == row[1][0])	// If correct
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else				// If wrong
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (-1.0);
	}
      HTM_SPAN_End ();
      TstExa_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write single or multiple choice answer in a test exam ************/
/*****************************************************************************/

static void TstExa_WriteChoiceAnsExam (struct UsrData *UsrDat,
                                       const struct TstExa_Exam *Exam,
				       unsigned NumQst,
				       struct Tst_Question *Question,
				       MYSQL_RES *mysql_res,
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

   /***** Get text and correctness of answers for this question
          from database (one row per answer) *****/
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
   Tst_GetChoiceAns (Question,mysql_res);

   /***** Get indexes for this question from string *****/
   TstExa_GetIndexesFromStr (Exam->Questions[NumQst].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstExa_GetAnswersFromStr (Exam->Questions[NumQst].StrAnswers,UsrAnswers);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstExa_WriteHeadUserCorrect (UsrDat);
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

   /***** Write the score of this question *****/
   if (TstVis_IsVisibleEachQstScore (Visibility))
     {
      TstExa_WriteScoreStart (4);
      if (Exam->Questions[NumQst].Score == 0.0)
         HTM_SPAN_Begin ("class=\"ANS_0\"");
      else if (Exam->Questions[NumQst].Score > 0.0)
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
      else
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
      HTM_Double2Decimals (Exam->Questions[NumQst].Score);
      HTM_SPAN_End ();
      TstExa_WriteScoreEnd ();
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Write text answer when assessing a test *******************/
/*****************************************************************************/

static void TstExa_WriteTextAnsExam (struct UsrData *UsrDat,
                                     const struct TstExa_Exam *Exam,
				     unsigned NumQst,
				     struct Tst_Question *Question,
				     MYSQL_RES *mysql_res,
				     unsigned Visibility)
  {
   unsigned NumOpt;
   MYSQL_ROW row;
   char TextAnsUsr[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1];
   bool Correct = false;
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */
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

      /***** Copy answer text (row[1]) and convert it, that is in HTML, to rigorous HTML ******/
      Str_Copy (Question->Answer.Options[NumOpt].Text,row[1],
                Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                        Question->Answer.Options[NumOpt].Text,
                        Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);

      /***** Copy answer feedback (row[2]) and convert it, that is in HTML, to rigorous HTML ******/
      if (TstVis_IsVisibleFeedbackTxt (Visibility))
	 if (row[2])
	    if (row[2][0])
	      {
	       Str_Copy (Question->Answer.Options[NumOpt].Feedback,row[2],
	                 Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
	                         Question->Answer.Options[NumOpt].Feedback,
	                         Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,false);
	      }

      /***** Assign correctness (row[4]) of this answer (this option) *****/
      Question->Answer.Options[NumOpt].Correct = (row[4][0] == 'Y');
     }

   /***** Header with the title of each column *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);
   TstExa_WriteHeadUserCorrect (UsrDat);
   HTM_TR_End ();

   HTM_TR_Begin (NULL);

   /***** Write the user answer *****/
   if (Exam->Questions[NumQst].StrAnswers[0])	// If user has answered the question
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Exam->Questions[NumQst].StrAnswers,
                TstExa_MAX_BYTES_ANSWERS_ONE_QST);

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
                   TstExa_MAX_BYTES_ANSWERS_ONE_QST);
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
      HTM_Txt (Exam->Questions[NumQst].StrAnswers);
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

   /***** Write the score of this question *****/
   if (TstVis_IsVisibleEachQstScore (Visibility))
     {
      TstExa_WriteScoreStart (4);
      if (!Exam->Questions[NumQst].StrAnswers[0])	// If user has omitted the answer
	{
         HTM_SPAN_Begin ("class=\"ANS_0\"");
         HTM_Double2Decimals (0.0);
	}
      else if (Correct)					// If correct
	{
         HTM_SPAN_Begin ("class=\"ANS_OK\"");
         HTM_Double2Decimals (1.0);
	}
      else						// If wrong
	{
         HTM_SPAN_Begin ("class=\"ANS_BAD\"");
         HTM_Double2Decimals (0.0);
	}
      HTM_SPAN_End ();
      TstExa_WriteScoreEnd ();
     }

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********* Write head with two columns:                               ********/
/********* one for the user's answer and other for the correct answer ********/
/*****************************************************************************/

static void TstExa_WriteHeadUserCorrect (struct UsrData *UsrDat)
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
/*********** Write the start ans the end of the score of an answer ***********/
/*****************************************************************************/

static void TstExa_WriteScoreStart (unsigned ColSpan)
  {
   extern const char *Txt_Score;

   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"%u\" class=\"DAT_SMALL LM\"",ColSpan);
   HTM_TxtColonNBSP (Txt_Score);
  }

static void TstExa_WriteScoreEnd (void)
  {
   HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Store user's answers of an test exam into database ************/
/*****************************************************************************/

static void TstExa_StoreOneExamQstInDB (const struct TstExa_Exam *Exam,
                                        unsigned NumQst)
  {
   char StrIndexes[TstExa_MAX_BYTES_INDEXES_ONE_QST + 1];
   char StrAnswers[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1];

   /***** Replace each separator of multiple parameters by a comma *****/
   /* In database commas are used as separators instead of special chars */
   Par_ReplaceSeparatorMultipleByComma (Exam->Questions[NumQst].StrIndexes,StrIndexes);
   Par_ReplaceSeparatorMultipleByComma (Exam->Questions[NumQst].StrAnswers,StrAnswers);

   /***** Insert question and user's answers into database *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   DB_QueryREPLACE ("can not update a question of a test exam",
		    "REPLACE INTO tst_exam_questions"
		    " (ExaCod,QstCod,QstInd,Score,Indexes,Answers)"
		    " VALUES"
		    " (%ld,%ld,%u,'%.15lg','%s','%s')",
		    Exam->ExaCod,Exam->Questions[NumQst].QstCod,
		    NumQst,	// 0, 1, 2, 3...
		    Exam->Questions[NumQst].Score,
		    StrIndexes,
		    StrAnswers);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/*********************** Update the score of a question **********************/
/*****************************************************************************/

static void Tst_UpdateQstScoreInDB (const struct TstExa_Exam *Exam,unsigned NumQst)
  {
   /***** Update number of clicks and score of the question *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   if (Exam->Questions[NumQst].AnswerIsNotBlank)
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	              " SET NumHits=NumHits+1,NumHitsNotBlank=NumHitsNotBlank+1,"
	              "Score=Score+(%.15lg)"
                      " WHERE QstCod=%ld",
		      Exam->Questions[NumQst].Score,
		      Exam->Questions[NumQst].QstCod);
   else	// The answer is blank
      DB_QueryUPDATE ("can not update the score of a question",
		      "UPDATE tst_questions"
	              " SET NumHits=NumHits+1"
                      " WHERE QstCod=%ld",
		      Exam->Questions[NumQst].QstCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************* Select users and dates to show their test exams ***************/
/*****************************************************************************/

void TstExa_SelUsrsToViewUsrsExams (void)
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

void TstExa_SelDatesToSeeMyExams (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_test_results;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_DO_NOT_SET,
      Dat_HMS_DO_NOT_SET
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

void TstExa_ShowMyExams (void)
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
   TstExa_ShowHeaderExams ();

   /***** List my test exams *****/
   TstCfg_GetConfigFromDB ();	// To get feedback type
   TstExa_ShowExams (&Gbl.Usrs.Me.UsrDat);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/******************** Get users and show their test exams ********************/
/*****************************************************************************/

void TstExa_GetUsrsAndShowExams (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  TstExa_ShowUsrsExams,
                                  TstExa_SelUsrsToViewUsrsExams);
  }

/*****************************************************************************/
/********************* Show test exams for several users *********************/
/*****************************************************************************/

static void TstExa_ShowUsrsExams (void)
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
   TstExa_ShowHeaderExams ();

   /***** List the test exams of the selected users *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 if (Usr_CheckIfICanViewTst (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show test exams *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    TstExa_ShowExams (&Gbl.Usrs.Other.UsrDat);
	   }
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************ Show header of my test exams ***********************/
/*****************************************************************************/

static void TstExa_ShowHeaderExams (void)
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

static void TstExa_ShowExams (struct UsrData *UsrDat)
  {
   extern const char *Txt_View_test;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumExams;
   unsigned NumExam;
   static unsigned UniqueId = 0;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstExa_Exam Exam;
   unsigned NumTotalQsts = 0;
   unsigned NumTotalQstsNotBlank = 0;
   double TotalScoreOfAllTests = 0.0;
   unsigned NumExamsVisibleByTchs = 0;
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);
   bool ICanViewTest;
   bool ICanViewScore;
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
			             "AllowTeachers,"			// row[5]
			             "Score"				// row[6]
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
         TstExa_ResetExam (&Exam);
	 if ((Exam.ExaCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of test exam.");

	 /* Get if teachers are allowed to see this test exams (row[5]) */
	 Exam.AllowTeachers = (row[5][0] == 'Y');
	 ClassDat = Exam.AllowTeachers ? "DAT" :
	                                   "DAT_LIGHT";

	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       ICanViewTest  = ItsMe;
	       ICanViewScore = ItsMe &&
		               TstVis_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	    case Rol_DEG_ADM:
	    case Rol_CTR_ADM:
	    case Rol_INS_ADM:
	       ICanViewTest  =
	       ICanViewScore = ItsMe ||
	                       Exam.AllowTeachers;
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

         if (NumExam)
            HTM_TR_Begin (NULL);

         /* Write date and time (row[1] and row[2] hold UTC date-times) */
         Exam.TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
         Exam.TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[2]);
         UniqueId++;
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    if (asprintf (&Id,"tst_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	       Lay_NotEnoughMemoryExit ();
	    HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		          Id,ClassDat,Gbl.RowEvenOdd);
	    Dat_WriteLocalDateHMSFromUTC (Id,Exam.TimeUTC[StartEndTime],
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					  true,true,false,0x7);
	    HTM_TD_End ();
	    free (Id);
	   }

         /* Get number of questions (row[3]) */
         if (sscanf (row[3],"%u",&Exam.NumQsts) != 1)
            Exam.NumQsts = 0;
	 if (Exam.AllowTeachers)
	    NumTotalQsts += Exam.NumQsts;

         /* Get number of questions not blank (row[4]) */
         if (sscanf (row[4],"%u",&Exam.NumQstsNotBlank) != 1)
            Exam.NumQstsNotBlank = 0;
	 if (Exam.AllowTeachers)
	    NumTotalQstsNotBlank += Exam.NumQstsNotBlank;

         /* Get score (row[6]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[6],"%lf",&Exam.Score) != 1)
            Exam.Score = 0.0;
         Str_SetDecimalPointToLocal ();	// Return to local system
	 if (Exam.AllowTeachers)
	    TotalScoreOfAllTests += Exam.Score;

         /* Write number of questions */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewTest)
	    HTM_Unsigned (Exam.NumQsts);
	 HTM_TD_End ();

         /* Write number of questions not blank */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewTest)
	    HTM_Unsigned (Exam.NumQstsNotBlank);
	 HTM_TD_End ();

	 /* Write score */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Double2Decimals (Exam.Score);
	 HTM_TD_End ();

         /* Write average score per question */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Double2Decimals (Exam.NumQsts ? Exam.Score /
		                                (double) Exam.NumQsts :
			                        0.0);
	 HTM_TD_End ();

         /* Write grade */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewScore)
            TstExa_ComputeAndShowGrade (Exam.NumQsts,
                                     Exam.Score,
                                     TstExa_SCORE_MAX);
	 HTM_TD_End ();

	 /* Link to show this test exam */
	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewTest)
	   {
	    Frm_StartForm (Gbl.Action.Act == ActSeeMyTstRes ? ActSeeOneTstResMe :
						              ActSeeOneTstResOth);
	    TstExa_PutParamExaCod (Exam.ExaCod);
	    Ico_PutIconLink ("tasks.svg",Txt_View_test);
	    Frm_EndForm ();
	   }
	 HTM_TD_End ();
	 HTM_TR_End ();

	 if (Exam.AllowTeachers)
            NumExamsVisibleByTchs++;
        }

      /***** Write totals for this user *****/
      TstExa_ShowExamsSummaryRow (ItsMe,NumExamsVisibleByTchs,
                                     NumTotalQsts,NumTotalQstsNotBlank,
                                     TotalScoreOfAllTests);
     }
   else
     {
      HTM_TD_ColouredEmpty (7);
      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/****************** Write parameter with code of test exam *******************/
/*****************************************************************************/

void TstExa_PutParamExaCod (long ExaCod)
  {
   Par_PutHiddenParamLong (NULL,"ExaCod",ExaCod);
  }

/*****************************************************************************/
/****************** Get parameter with code of test exam *********************/
/*****************************************************************************/

long TstExa_GetParamExaCod (void)
  {
   /***** Get code of exam *****/
   return Par_GetParToLong ("ExaCod");
  }

/*****************************************************************************/
/**************** Show row with summary of user's test exams *****************/
/*****************************************************************************/

static void TstExa_ShowExamsSummaryRow (bool ItsMe,
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
      TstExa_ComputeAndShowGrade (NumTotalQsts,
                               TotalScoreOfAllTests,
                               TstExa_SCORE_MAX);
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

void TstExa_ShowOneExam (void)
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
   struct TstExa_Exam Exam;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   bool ItsMe;
   bool ICanViewTest;
   bool ICanViewScore;

   /***** Get the code of the test *****/
   TstExa_ResetExam (&Exam);
   if ((Exam.ExaCod = TstExa_GetParamExaCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of test is missing.");

   /***** Get test exam data *****/
   TstExa_GetExamDataByExaCod (&Exam);
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
			       Exam.AllowTeachers;
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
      TstExa_GetExamQuestionsFromDB (&Exam);

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
      if (!Usr_CheckIfICanViewTst (&Gbl.Usrs.Other.UsrDat))
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
	 Dat_WriteLocalDateHMSFromUTC (Id,Exam.TimeUTC[StartEndTime],
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
	        Exam.NumQsts,
	        Exam.NumQstsNotBlank,Txt_non_blank_QUESTIONS);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Score */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Score);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
	 HTM_Double2Decimals (Exam.Score);
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
         TstExa_ComputeAndShowGrade (Exam.NumQsts,
                                  Exam.Score,
                                  TstExa_SCORE_MAX);
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
      TstExa_ShowTagsPresentInAnExam (Exam.ExaCod);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write answers and solutions *****/
      TstExa_ShowExamAnswers (&Gbl.Usrs.Other.UsrDat,
			      &Exam,
			      TstCfg_GetConfigVisibility ());

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write total mark of test *****/
      if (ICanViewScore)
	{
	 HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	 HTM_TxtColonNBSP (Txt_Score);
	 HTM_Double2Decimals (Exam.Score);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Grade);
         TstExa_ComputeAndShowGrade (Exam.NumQsts,
                                  Exam.Score,
                                  TstExa_SCORE_MAX);
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

static void TstExa_ShowTagsPresentInAnExam (long ExaCod)
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
			     ExaCod);
   Tst_ShowTagList (NumTags,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Show user's and correct answers of a test exam ***************/
/*****************************************************************************/

void TstExa_ShowExamAnswers (struct UsrData *UsrDat,
			     struct TstExa_Exam *Exam,
			     unsigned Visibility)
  {
   extern const char *Txt_Question_modified;
   extern const char *Txt_Question_removed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   bool ThisQuestionHasBeenEdited;
   time_t EditTimeUTC;

   for (NumQst = 0;
	NumQst < Exam->NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Query database *****/
      if (Tst_GetOneQuestionByCod (Exam->Questions[NumQst].QstCod,&mysql_res))	// Question exists
	{
	 /***** Get row of the result of the query *****/
	 row = mysql_fetch_row (mysql_res);

	 /***** If this question has been edited later than test time
	        ==> don't show question ****/
	 EditTimeUTC = Dat_GetUNIXTimeFromStr (row[0]);
	 ThisQuestionHasBeenEdited = false;
	 if (EditTimeUTC > Exam->TimeUTC[Dat_START_TIME])
	    ThisQuestionHasBeenEdited = true;

	 if (ThisQuestionHasBeenEdited)
	   {
	    /***** Question has been edited *****/
	    HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"BIG_INDEX RT COLOR%u\"",Gbl.RowEvenOdd);
	    HTM_Unsigned (NumQst + 1);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"DAT_LIGHT LT COLOR%u\"",Gbl.RowEvenOdd);
	    HTM_Txt (Txt_Question_modified);
	    HTM_TD_End ();

	    HTM_TR_End ();
	   }
	 else
	    /***** Write questions and answers *****/
	    TstExa_WriteQstAndAnsExam (UsrDat,
				       Exam,
				       NumQst,
				       row,
				       Visibility);
	}
      else
	{
	 /***** Question does not exists *****/
         HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"BIG_INDEX RT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Unsigned (NumQst + 1);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT_LIGHT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Txt_Question_removed);
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/************ Get data of a test exam using its test exam code ***************/
/*****************************************************************************/

void TstExa_GetExamDataByExaCod (struct TstExa_Exam *Exam)
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
		              "AllowTeachers,"			// row[5]
		              "Score"				// row[6]
		       " FROM tst_exams"
		       " WHERE ExaCod=%ld AND CrsCod=%ld",
		       Exam->ExaCod,
		       Gbl.Hierarchy.Crs.CrsCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user code (row[0]) */
      Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get date-time (row[1] and row[2] hold UTC date-time) */
      Exam->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
      Exam->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[2]);

      /* Get number of questions (row[3]) */
      if (sscanf (row[3],"%u",&Exam->NumQsts) != 1)
	 Exam->NumQsts = 0;

      /* Get number of questions not blank (row[4]) */
      if (sscanf (row[4],"%u",&Exam->NumQstsNotBlank) != 1)
	 Exam->NumQstsNotBlank = 0;

      /* Get if teachers are allowed to see this test exam (row[5]) */
      Exam->AllowTeachers = (row[5][0] == 'Y');

      /* Get score (row[6]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[6],"%lf",&Exam->Score) != 1)
	 Exam->Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }
   else
     {
      Exam->TimeUTC[Dat_START_TIME] =
      Exam->TimeUTC[Dat_END_TIME  ] = 0;
      Exam->NumQsts                 = 0;
      Exam->NumQstsNotBlank         = 0;
      Exam->AllowTeachers           = false;
      Exam->Score                   = 0.0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get the questions of a test exam from database ****************/
/*****************************************************************************/

void TstExa_GetExamQuestionsFromDB (struct TstExa_Exam *Exam)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;

   /***** Get questions of a test exam from database *****/
   Exam->NumQsts =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get questions"
					 " of a test exam",
			      "SELECT QstCod,"	// row[0]
			             "Indexes,"	// row[1]
			             "Answers"	// row[2]
			      " FROM tst_exam_questions"
			      " WHERE ExaCod=%ld"
			      " ORDER BY QstInd",
			      Exam->ExaCod);

   /***** Get questions codes *****/
   for (NumQst = 0;
	NumQst < Exam->NumQsts;
	NumQst++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get question code */
      if ((Exam->Questions[NumQst].QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Get indexes for this question (row[1]) */
      Str_Copy (Exam->Questions[NumQst].StrIndexes,row[1],
                TstExa_MAX_BYTES_INDEXES_ONE_QST);

      /* Get answers selected by user for this question (row[2]) */
      Str_Copy (Exam->Questions[NumQst].StrAnswers,row[2],
                TstExa_MAX_BYTES_ANSWERS_ONE_QST);

      /* Replace each comma by a separator of multiple parameters */
      /* In database commas are used as separators instead of special chars */
      Par_ReplaceCommaBySeparatorMultiple (Exam->Questions[NumQst].StrIndexes);
      Par_ReplaceCommaBySeparatorMultiple (Exam->Questions[NumQst].StrAnswers);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Remove test exams made by a user *********************/
/*****************************************************************************/

void TstExa_RemoveExamsMadeByUsrInAllCrss (long UsrCod)
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

void TstExa_RemoveExamsMadeByUsrInCrs (long UsrCod,long CrsCod)
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

void TstExa_RemoveCrsExams (long CrsCod)
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
