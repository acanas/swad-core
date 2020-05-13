// swad_test_print.h: test exam prints made by users

#ifndef _SWAD_TST_PRN
#define _SWAD_TST_PRN
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_test_config.h"
#include "swad_test_type.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct TstPrn_PrintedQuestion
  {
   long QstCod;		// Question code
   long SetCod;		// Only for exams
   char StrIndexes[Tst_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
   char StrAnswers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user
   double Score;		// Question score
   bool AnswerIsNotBlank;	// Answer not blank?
  };

struct TstPrn_Print
  {
   long PrnCod;			// Test print code
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   unsigned NumQsts;		// Number of questions
   unsigned NumQstsNotBlank;	// Number of questions not blank
   bool Sent;			// This test print has been sent or not?
				// "Sent" means that user has clicked "Send" button after finishing
   bool AllowTeachers;		// Are teachers allowed to see this test result?
   double Score;		// Total score of the test print
   struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TstPrn_ResetPrint (struct TstPrn_Print *Print);
void TstPrn_CreatePrintInDB (struct TstPrn_Print *Print);
void TstPrn_UpdatePrintInDB (const struct TstPrn_Print *Print);

void TstPrn_ShowExamAfterAssess (struct TstPrn_Print *Print);

void TstPrn_ComputeScoresAndStoreQuestionsOfPrint (struct TstPrn_Print *Print,
                                                bool UpdateQstScore);
void TstPrn_ComputeAnswerScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				struct Tst_Question *Question);

//-----------------------------------------------------------------------------
void TstPrn_ComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
		                const struct Tst_Question *Question);
void TstPrn_ComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				const struct Tst_Question *Question);
void TstPrn_ComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
			        const struct Tst_Question *Question);
void TstPrn_ComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
	                        const struct Tst_Question *Question);
void TstPrn_ComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				const struct Tst_Question *Question);
//-----------------------------------------------------------------------------

void TstPrn_GetIndexesFromStr (const char StrIndexesOneQst[Tst_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			       unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION]);
void TstPrn_GetAnswersFromStr (const char StrAnswersOneQst[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1],
			       bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION]);

void TstPrn_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade);
double TstPrn_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade);
void TstPrn_ShowGrade (double Grade,double MaxGrade);

void TstPrn_SelUsrsToViewUsrsExams (void);
void TstPrn_SelDatesToSeeMyExams (void);
void TstPrn_ShowMyExams (void);
void TstPrn_GetUsrsAndShowExams (void);

void TstPrn_PutParamPrnCod (long ExaCod);
long TstPrn_GetParamPrnCod (void);

void TstPrn_ShowOneExam (void);
void TstPrn_ShowExamAnswers (struct UsrData *UsrDat,
			     struct TstPrn_Print *Print,
			     unsigned Visibility);
void TstPrn_GetPrintDataByPrnCod (struct TstPrn_Print *Print);

void TstPrn_GetPrintQuestionsFromDB (struct TstPrn_Print *Print);
void TstPrn_RemoveExamsMadeByUsrInAllCrss (long UsrCod);
void TstPrn_RemoveExamsMadeByUsrInCrs (long UsrCod,long CrsCod);
void TstPrn_RemoveCrsExams (long CrsCod);

#endif
