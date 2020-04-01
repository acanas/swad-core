// swad_test_results.h: test results

#ifndef _SWAD_TSR
#define _SWAD_TSR
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

#define Tst_MAX_OPTIONS_PER_QUESTION		  10
#define Tst_MAX_BYTES_INDEXES_ONE_QST		(Tst_MAX_OPTIONS_PER_QUESTION * (3 + 1))
#define Tst_MAX_BYTES_ANSWERS_ONE_QST		(Tst_MAX_OPTIONS_PER_QUESTION * (3 + 1))

#define TsR_SCORE_MAX	10	// Maximum score of a test (10 in Spain). Must be unsigned! // TODO: Make this configurable by teachers

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct TsR_Result
  {
   long TstCod;			// Exam code
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   unsigned NumQsts;		// Number of questions
   unsigned NumQstsNotBlank;	// Number of questions not blank
   bool AllowTeachers;		// Are teachers allowed to see this test result?
   double Score;		// Total score of the test result
   struct
     {
      long QstCod;		// Question code
      char StrIndexes[Tst_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
      char StrAnswers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user
      double Score;		// Question score
      bool AnswerIsNotBlank;	// Answer not blank?
     } Questions[TstCfg_MAX_QUESTIONS_PER_TEST];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TsR_SelUsrsToViewUsrsTstResults (void);
void TsR_SelDatesToSeeMyTstResults (void);
void TsR_ShowMyTstResults (void);
void TsR_CreateTestResultInDB (struct TsR_Result *Result);
void TsR_UpdateScoreOfTestResultInDB (const struct TsR_Result *Result);
void TsR_GetUsrsAndShowTstResults (void);
void TsR_ShowOneTstResult (void);
void TsR_ShowTestResult (struct UsrData *UsrDat,
			 struct TsR_Result *Result,
			 unsigned Visibility);
void TsR_StoreOneTestResultQstInDB (const struct TsR_Result *Result,
                                    unsigned NumQst);
void TsR_RemoveTestResultsMadeByUsrInAllCrss (long UsrCod);
void TsR_RemoveTestResultsMadeByUsrInCrs (long UsrCod,long CrsCod);
void TsR_RemoveCrsTestResults (long CrsCod);

#endif
