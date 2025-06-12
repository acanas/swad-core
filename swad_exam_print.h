// swad_exam_print.h: exam prints (each copy of an exam in a session for a student)

#ifndef _SWAD_EXA_PRN
#define _SWAD_EXA_PRN
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_test_print.h"

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

#define ExaPrn_MAX_QUESTIONS_PER_EXAM_PRINT	100	// Absolute maximum number of questions in an exam print

struct ExaPrn_NumQuestions
  {
   unsigned All;	// Total number of questions
   unsigned NotBlank;	// Answered questions (valid or not)
   struct
     {
      struct
        {
	 unsigned Negative;
	 unsigned Zero;
	 unsigned Positive;
        } Wrong;
      unsigned Correct;
      unsigned Blank;
      unsigned Total;
     } Valid;		// Number of valid questions
  };

struct ExaPrn_Score
  {
   double All;	// Total score of the exam print (counting valid and invalid questions)
   double Valid;	// Total score of the exam print (counting only valid questions)
  };

struct ExaPrn_Print
  {
   long PrnCod;		// Exam print code
   long SesCod;		// Session code associated to this print
   long UsrCod;		// User who answered the exam print
   char EnUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   struct ExaPrn_NumQuestions NumQsts;
   struct ExaPrn_Score Score;
   struct Qst_PrintedQuestion Qsts[ExaPrn_MAX_QUESTIONS_PER_EXAM_PRINT];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaPrn_ResetPrint (struct ExaPrn_Print *Print);

void ExaQstShe_ListSheets (void);
void ExaQstShe_PrintSheets (void);

void ExaPrn_ShowPrintToStdToFill (void);

void ExaPrn_GetQstsPrint (struct Exa_Exams *Exams,
			  const struct ExaSes_Session *Session,
			  struct Usr_Data *UsrDat,
			  struct ExaPrn_Print *Print,
			  bool Start);
void ExaPrn_GetPrintDataByPrnCod (struct ExaPrn_Print *Print);
void ExaPrn_GetPrintDataBySesCodAndUsrCod (struct ExaPrn_Print *Print);

void ExaPrn_GetPrintQuestionsFromDB (struct ExaPrn_Print *Print);

void ExaPrn_ReceiveAnswer (void);
void ExaPrn_GetAnswerFromForm (struct ExaPrn_Print *Print,unsigned QstInd);
unsigned ExaPrn_GetParQstInd (void);
void ExaPrn_ComputeScoreAndStoreQuestionOfPrint (struct ExaPrn_Print *Print,
                                                 unsigned QstInd);

void ExaPrn_ComputeAnswerScore (struct Qst_PrintedQuestion *PrintedQuestion,
				struct Qst_Question *Question);

#endif
