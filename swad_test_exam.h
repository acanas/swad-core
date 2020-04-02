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

#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define TstExa_MAX_BYTES_INDEXES_ONE_QST	(Tst_MAX_OPTIONS_PER_QUESTION * (3 + 1))

#define TstExa_MAX_CHARS_ANSWERS_ONE_QST	(128 - 1)	// 127
#define TstExa_MAX_BYTES_ANSWERS_ONE_QST	((TstExa_MAX_CHARS_ANSWERS_ONE_QST + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define TstExa_SCORE_MAX	10	// Maximum score of a test (10 in Spain). Must be unsigned! // TODO: Make this configurable by teachers

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct TstExa_Exam
  {
   long ExaCod;			// Test exam code
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   unsigned NumQsts;		// Number of questions
   unsigned NumQstsNotBlank;	// Number of questions not blank
   bool AllowTeachers;		// Are teachers allowed to see this test exam?
   double Score;		// Total score of the test exam
   struct
     {
      long QstCod;		// Question code
      char StrIndexes[TstExa_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
      char StrAnswers[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user
      double Score;		// Question score
      bool AnswerIsNotBlank;	// Answer not blank?
     } Questions[TstCfg_MAX_QUESTIONS_PER_TEST];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TstExa_ResetExam (struct TstExa_Exam *Exam);
void TstExa_CreateExamInDB (struct TstExa_Exam *Exam);
void TstExa_UpdateExamInDB (const struct TstExa_Exam *Exam);

void TstExa_ShowExamAfterAssess (struct TstExa_Exam *Exam);
void TstExa_WriteQstAndAnsExam (struct UsrData *UsrDat,
				struct TstExa_Exam *Result,
				unsigned NumQst,
				MYSQL_ROW row,
				unsigned Visibility);

void TstExa_ComputeScoresAndStoreExamQuestions (struct TstExa_Exam *Exam,
                                                bool UpdateQstScore);
void TstExa_ComputeChoiceAnsScore (struct TstExa_Exam *Result,
				   unsigned NumQst,
				   struct Tst_Question *Question);
void TstExa_GetIndexesFromStr (const char StrIndexesOneQst[TstExa_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			       unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION]);
void TstExa_GetAnswersFromStr (const char StrAnswersOneQst[TstExa_MAX_BYTES_ANSWERS_ONE_QST + 1],
			       bool UsrAnswers[Tst_MAX_OPTIONS_PER_QUESTION]);

void TstExa_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade);
double TstExa_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade);
void TstExa_ShowGrade (double Grade,double MaxGrade);

void TstExa_SelUsrsToViewUsrsExams (void);
void TstExa_SelDatesToSeeMyExams (void);
void TstExa_ShowMyExams (void);
void TstExa_GetUsrsAndShowExams (void);

void TstExa_PutParamExaCod (long ExaCod);
long TstExa_GetParamExaCod (void);

void TstExa_ShowOneExam (void);
void TstExa_ShowExamAnswers (struct UsrData *UsrDat,
			     struct TstExa_Exam *Exam,
			     unsigned Visibility);
void TstExa_GetExamDataByExaCod (struct TstExa_Exam *Exam);

void TstExa_GetExamQuestionsFromDB (struct TstExa_Exam *Exam);
void TstExa_RemoveExamsMadeByUsrInAllCrss (long UsrCod);
void TstExa_RemoveExamsMadeByUsrInCrs (long UsrCod,long CrsCod);
void TstExa_RemoveCrsExams (long CrsCod);

#endif
