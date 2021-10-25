// swad_question.h: test/exam/game questions

#ifndef _SWAD_QST
#define _SWAD_QST
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type
#include <time.h>		// For time_t

#include "swad_exam.h"
#include "swad_game.h"
#include "swad_question_type.h"
#include "swad_media.h"
#include "swad_string.h"
#include "swad_tag.h"
// #include "swad_test.h"
// #include "swad_test_config.h"
// #include "swad_test_visibility.h"
// #include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Qst_MAX_CHARS_ANSWER_OR_FEEDBACK	(1024 - 1)	// 1023
#define Qst_MAX_BYTES_ANSWER_OR_FEEDBACK	((Qst_MAX_CHARS_ANSWER_OR_FEEDBACK + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Qst_AnswerTypes
  {
   bool All;
   char List[Qst_MAX_BYTES_LIST_ANSWER_TYPES + 1];
  };

#define Qst_NUM_TYPES_ORDER_QST	5
typedef enum
  {
   Qst_ORDER_STEM                    = 0,
   Qst_ORDER_NUM_HITS                = 1,
   Qst_ORDER_AVERAGE_SCORE           = 2,
   Qst_ORDER_NUM_HITS_NOT_BLANK      = 3,
   Qst_ORDER_AVERAGE_SCORE_NOT_BLANK = 4,
  } Qst_QuestionsOrder_t;
#define Qst_DEFAULT_ORDER Qst_ORDER_STEM

#define Qst_NUM_VALIDITIES 2
typedef enum
  {
   Qst_INVALID_QUESTION,
   Qst_VALID_QUESTION,
  } Qst_Validity_t;

struct Qst_Question
  {
   long QstCod;
   struct Tag_Tags Tags;
   time_t EditTime;
   char *Stem;
   char *Feedback;
   struct Med_Media Media;
   struct
     {
      Qst_AnswerType_t Type;
      unsigned NumOptions;
      bool Shuffle;
      char TF;
      struct
	{
	 bool Correct;
	 char *Text;
	 char *Feedback;
	 struct Med_Media Media;
	} Options[Qst_MAX_OPTIONS_PER_QUESTION];
      long Integer;
      double FloatingPoint[2];
     } Answer;
   unsigned long NumHits;
   unsigned long NumHitsNotBlank;
   double Score;
   Qst_Validity_t Validity;	// If a question in an exam has been marked as invalid
  };

struct Qst_Questions
  {
   struct Tag_Tags Tags;		// Selected tags
   struct Qst_AnswerTypes AnswerTypes;	// Selected answer types
   Qst_QuestionsOrder_t SelectedOrder;	// Order for listing questions
   unsigned NumQsts;			// Number of questions
   struct Qst_Question Question;	// Selected / editing question
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Qst_ListQuestionsToEdit (void);
void Qst_ListQuestionsToSelectForExamSet (struct Exa_Exams *Exams);
void Qst_ListQuestionsToSelectForGame (struct Gam_Games *Games);

void Qst_GetQuestions (struct Qst_Questions *Questions,MYSQL_RES **mysql_res);

void Qst_ChangeFormatAnswersText (struct Qst_Question *Question);
void Qst_ChangeFormatAnswersFeedback (struct Qst_Question *Question);

Qst_AnswerType_t Qst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeDB);
Qst_AnswerType_t Qst_ConvertFromUnsignedStrToAnsTyp (const char *UnsignedStr);

#endif
