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

#define TsR_SCORE_MAX				  10	// Maximum score of a test (10 in Spain). Must be unsigned! // TODO: Make this configurable by teachers

#define TsR_MAX_BYTES_FEEDBACK_TYPE		  32

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/
/*
Visibilidad de resultados:
* Texto de preguntas y respuestas
* Texto de realimentación
* Respuestas correctas
* Puntuación de cada pregunta
* Puntuación total

Visibility of results:
* Text of questions and answers
* Feedback text
* Correct answers
* Score of each question
* Total score
*/
#define TsR_NUM_ITEMS_VISIBILITY 5
typedef enum
  {
   TsR_VISIBLE_QST_ANS_TEXT   = 0,	// Questions and answers text
   TsR_VISIBLE_FEEDBACK_TEXT  = 1,	// Feedback text
   TsR_VISIBLE_CORRECT_ANSWER = 2,	// Correct answers
   TsR_VISIBLE_EACH_QST_SCORE = 3,	// Score of each question
   TsR_VISIBLE_TOTAL_SCORE    = 4,	// Total score
  } TsR_ResultVisibility_t;
#define TsR_VISIBILITY_DEFAULT ((1 << TsR_NUM_ITEMS_VISIBILITY) - 1)	// All visible

#define TsR_NUM_TYPES_FEEDBACK		5
typedef enum
  {
   TsR_FEEDBACK_NOTHING        = 0,
   TsR_FEEDBACK_TOTAL_RESULT   = 1,
   TsR_FEEDBACK_EACH_RESULT    = 2,
   TsR_FEEDBACK_EACH_GOOD_BAD  = 3,
   TsR_FEEDBACK_FULL_FEEDBACK  = 4,
  } TsR_Feedback_t;
#define TsR_FEEDBACK_DEFAULT TsR_FEEDBACK_FULL_FEEDBACK

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TsR_SelUsrsToViewUsrsTstResults (void);
void TsR_SelDatesToSeeMyTstResults (void);
void TsR_ShowMyTstResults (void);
long TsR_CreateTestResultInDB (void);
void TsR_StoreScoreOfTestResultInDB (long TstCod,
                                     unsigned NumQstsNotBlank,double Score);
void TsR_GetUsrsAndShowTstResults (void);
void TsR_ShowOneTstResult (void);
void TsR_ShowTestResult (struct UsrData *UsrDat,
			 unsigned NumQsts,time_t TstTimeUTC);
void TsR_StoreOneTestResultQstInDB (long TstCod,long QstCod,unsigned NumQst,double Score);
void TsR_RemoveTestResultsMadeByUsrInAllCrss (long UsrCod);
void TsR_RemoveTestResultsMadeByUsrInCrs (long UsrCod,long CrsCod);
void TsR_RemoveCrsTestResults (long CrsCod);

TsR_Feedback_t TsR_GetFeedbackTypeFromForm (void);

void TsR_PutVisibilityCheckboxes (unsigned SelectedVisibility);

#endif
