// swad_test_exam.c: test exams made by users

#ifndef _SWAD_TST_EXA
#define _SWAD_TST_EXA
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

#include "swad_test.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define TstRes_MAX_BYTES_INDEXES_ONE_QST	(Tst_MAX_OPTIONS_PER_QUESTION * (3 + 1))

#define TstRes_MAX_CHARS_ANSWERS_ONE_QST	(128 - 1)	// 127
#define TstRes_MAX_BYTES_ANSWERS_ONE_QST	((TstRes_MAX_CHARS_ANSWERS_ONE_QST + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define TstRes_SCORE_MAX	10	// Maximum score of a test (10 in Spain). Must be unsigned! // TODO: Make this configurable by teachers

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct TstRes_Result
  {
   long ResCod;			// Test result code
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   unsigned NumQsts;		// Number of questions
   unsigned NumQstsNotBlank;	// Number of questions not blank
   bool Sent;			// This test result has been sent or not?
				// "Sent" means that user has clicked "Send" button after finishing
   bool AllowTeachers;		// Are teachers allowed to see this test result?
   double Score;		// Total score of the test result
   struct
     {
      long QstCod;		// Question code
      char StrIndexes[TstRes_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
      char StrAnswers[TstRes_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user
      double Score;		// Question score
      bool AnswerIsNotBlank;	// Answer not blank?
     } Questions[TstCfg_MAX_QUESTIONS_PER_TEST];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TstRes_ResetResult (struct TstRes_Result *Result);
void TstRes_CreateExamInDB (struct TstRes_Result *Exam);
void TstRes_UpdateExamInDB (const struct TstRes_Result *Exam);

void TstRes_ShowExamAfterAssess (struct TstRes_Result *Exam);

void TstRes_ComputeScoresAndStoreExamQuestions (struct TstRes_Result *Exam,
                                                bool UpdateQstScore);
void TstRes_ComputeChoiceAnsScore (struct TstRes_Result *Result,
				   unsigned NumQst,
				   struct Tst_Question *Question);
void TstRes_GetIndexesFromStr (const char StrIndexesOneQst[TstRes_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			       unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION]);
void TstRes_GetAnswersFromStr (const char StrAnswersOneQst[TstRes_MAX_BYTES_ANSWERS_ONE_QST + 1],
			       bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION]);

void TstRes_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade);
double TstRes_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade);
void TstRes_ShowGrade (double Grade,double MaxGrade);

void TstRes_SelUsrsToViewUsrsExams (void);
void TstRes_SelDatesToSeeMyExams (void);
void TstRes_ShowMyExams (void);
void TstRes_GetUsrsAndShowExams (void);

void TstRes_PutParamExaCod (long ExaCod);
long TstRes_GetParamExaCod (void);

void TstRes_ShowOneExam (void);
void TstRes_ShowExamAnswers (struct UsrData *UsrDat,
			     struct TstRes_Result *Result,
			     unsigned Visibility);
void TstRes_GetExamDataByExaCod (struct TstRes_Result *Result);

void TstRes_GetExamQuestionsFromDB (struct TstRes_Result *Result);
void TstRes_RemoveExamsMadeByUsrInAllCrss (long UsrCod);
void TstRes_RemoveExamsMadeByUsrInCrs (long UsrCod,long CrsCod);
void TstRes_RemoveCrsExams (long CrsCod);

#endif
