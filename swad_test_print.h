// swad_test_print.h: test prints made by users

#ifndef _SWAD_TST_PRN
#define _SWAD_TST_PRN
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_question.h"
#include "swad_question_type.h"
#include "swad_test.h"
#include "swad_test_config.h"
#include "swad_test_visibility.h"
#include "swad_user.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct TstPrn_NumQuestions
  {
   unsigned All;	// Total number of questions
   unsigned NotBlank;	// Answered questions
  };

typedef enum
  {
   TstPrn_ANSWER_IS_CORRECT,
   TstPrn_ANSWER_IS_WRONG_NEGATIVE,
   TstPrn_ANSWER_IS_WRONG_ZERO,
   TstPrn_ANSWER_IS_WRONG_POSITIVE,
   TstPrn_ANSWER_IS_BLANK,
  } TstPrn_Correct_t;

struct TstPrn_PrintedQuestion
  {
   long QstCod;		// Question code
   long SetCod;		// Only for exams
   char StrIndexes[Qst_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
   char StrAnswers[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user
   TstPrn_Correct_t AnswerIsCorrect;	// Is question wrong, medium or correct?
   double Score;			// Question score
  };

struct TstPrn_Print
  {
   long PrnCod;			// Test print code
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   struct TstPrn_NumQuestions NumQsts;	// Number of questions
   bool Sent;				// This test print has been sent or not?
					// "Sent" means that user has clicked "Send" button after finishing
   HidVis_HiddenOrVisible_t VisibleByTchs;	// Are teachers allowed to see this test result?
   double Score;			// Total score of the test print
   struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST];
  };

#define Tst_NUM_REQUEST_OR_CONFIRM 2
typedef enum
  {
   TstPrn_REQUEST,
   TstPrn_CONFIRM,
  } TstPrn_RequestOrConfirm_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TstPrn_ResetPrint (struct TstPrn_Print *Print);

void TstPrn_ShowTestPrintToFillIt (struct TstPrn_Print *Print,
                                   unsigned NumPrintsGeneratedByMe,
                                   TstPrn_RequestOrConfirm_t RequestOrConfirm);

void TstPrn_ShowPrintAfterAssess (struct TstPrn_Print *Print);

void TstPrn_GetAnswersFromForm (struct TstPrn_Print *Print);

void TstPrn_ComputeScoresAndStoreQuestionsOfPrint (struct TstPrn_Print *Print,
                                                   bool UpdateQstScore);
void TstPrn_ComputeAnswerScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				struct Qst_Question *Question);

//-----------------------------------------------------------------------------

void TstPrn_ComputeIntAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
		                const struct Qst_Question *Question);
void TstPrn_ComputeFltAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				const struct Qst_Question *Question);
void TstPrn_ComputeTF_AnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
			        const struct Qst_Question *Question);
void TstPrn_ComputeChoAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
	                        const struct Qst_Question *Question);
void TstPrn_ComputeTxtAnsScore (struct TstPrn_PrintedQuestion *PrintedQuestion,
				const struct Qst_Question *Question);

//-----------------------------------------------------------------------------

void Qst_ChangeFormatAnswersText (struct Qst_Question *Question);
void Qst_ChangeFormatAnswersFeedback (struct Qst_Question *Question);

void TstPrn_GetIndexesFromStr (const char StrIndexesOneQst[Qst_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			       unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION]);
void TstPrn_GetAnswersFromStr (const char StrAnswersOneQst[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1],
			       HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION]);

void TstPrn_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade);
double TstPrn_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade);

void TstPrn_WriteAnswersExam (struct Usr_Data *UsrDat,
                              const struct TstPrn_PrintedQuestion *PrintedQuestion,
			      struct Qst_Question *Question,
			      Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			      const char *ClassTxt,
			      const char *ClassFeedback);

void TstPrn_SelUsrsToViewUsrsPrints (void);
void TstPrn_SelDatesToSeeMyPrints (void);
void TstPrn_ShowMyPrints (void);
void TstPrn_GetUsrsAndShowPrints (void);

void TstPrn_ShowOnePrint (void);
void TstPrn_ShowPrintAnswers (struct Usr_Data *UsrDat,
			      unsigned NumQsts,
			      struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST],
			      time_t TimeUTC[Dat_NUM_START_END_TIME],
			      unsigned Visibility);
void TstPrn_GetPrintDataByPrnCod (struct TstPrn_Print *Print);

bool TstPrn_GetPrintQuestionsFromDB (struct TstPrn_Print *Print);
void TstPrn_RemovePrintsMadeByUsrInAllCrss (long UsrCod);
void TstPrn_RemovePrintsMadeByUsrInCrs (long UsrCod,long CrsCod);
void TstPrn_RemoveCrsPrints (long CrsCod);

unsigned TstPrn_GetNumPrintsGeneratedByMe (void);

#endif
