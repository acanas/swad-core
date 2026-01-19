// swad_question_type.h: definition of types for questions

#ifndef _SWAD_QST_TYP
#define _SWAD_QST_TYP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_exam_type.h"
#include "swad_media.h"
#include "swad_tag_type.h"

/*****************************************************************************/
/************************* Public constants and types ************************/
/*****************************************************************************/

#define Qst_NUM_ANS_TYPES	6
#define Qst_MAX_BYTES_LIST_ANSWER_TYPES	(Qst_NUM_ANS_TYPES * (Cns_MAX_DIGITS_UINT + 1))
typedef enum
  {
   Qst_ANS_INT             = 0,
   Qst_ANS_FLOAT           = 1,
   Qst_ANS_TRUE_FALSE      = 2,
   Qst_ANS_UNIQUE_CHOICE   = 3,
   Qst_ANS_MULTIPLE_CHOICE = 4,
   Qst_ANS_TEXT            = 5,
   Qst_ANS_UNKNOWN         = 6,	// Unknown/all/any type of answer
  } Qst_AnswerType_t;

#define QstTF__NUM_OPTIONS 3
typedef enum
  {
   QstTF__OPTION_EMPTY,
   QstTF__OPTION_TRUE,
   QstTF__OPTION_FALSE,
  } QstTF__OptionTF_t;

#define Qst_MAX_BYTES_ANSWER_TYPE	32

#define Qst_MAX_OPTS_PER_QST	10

#define Qst_MAX_BYTES_INDEXES_ONE_QST	(Qst_MAX_OPTS_PER_QST * (3 + 1))

#define Qst_MAX_CHARS_ANSWERS_ONE_QST	(128 - 1)	// 127
#define Qst_MAX_BYTES_ANSWERS_ONE_QST	((Qst_MAX_CHARS_ANSWERS_ONE_QST + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

typedef enum
  {
   Qst_ANSWER_IS_CORRECT,
   Qst_ANSWER_IS_WRONG_NEGATIVE,
   Qst_ANSWER_IS_WRONG_ZERO,
   Qst_ANSWER_IS_WRONG_POSITIVE,
   Qst_ANSWER_IS_BLANK,
  } Qst_Correct_t;

struct Qst_Answer
  {
   char Str[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by the student/teacher in form
   Qst_Correct_t IsCorrect;			// Is answer wrong, medium or correct?
   double Score;				// Answer score
  };
struct Qst_PrintedQuestion
  {
   long QstCod;		// Question code
   long SetCod;		// Only for exams
   char StrIndexes[Qst_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
   struct Qst_Answer Answer;			// Answers
  };

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

#define Qst_NUM_SHUFFLE 2
typedef enum
  {
   Qst_DONT_SHUFFLE = 0,
   Qst_SHUFFLE      = 1,
  } Qst_Shuffle_t;

#define Qst_NUM_WRONG_CORRECT 3
typedef enum
  {
   Qst_BLANK,
   Qst_WRONG,
   Qst_CORRECT,
  } Qst_WrongOrCorrect_t;

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
      unsigned NumOpts;		// Number of options
      Qst_Shuffle_t Shuffle;
      long Integer;
      double FloatingPoint[2];
      QstTF__OptionTF_t OptionTF;
      struct
	{
	 Qst_WrongOrCorrect_t Correct;
	 char *Text;
	 char *Feedback;
	 struct Med_Media Media;
	} Options[Qst_MAX_OPTS_PER_QST];
     } Answer;
   unsigned long NumHits;
   unsigned long NumHitsNotBlank;
   double Score;
   ExaSet_Validity_t Validity;	// If a question in an exam has been marked as invalid
  };

struct Qst_Questions
  {
   struct Tag_Tags Tags;		// Selected tags
   struct Qst_AnswerTypes AnswerTypes;	// Selected answer types
   Qst_QuestionsOrder_t SelectedOrder;	// Order for listing questions
   unsigned NumQsts;			// Number of questions
   struct Qst_Question Qst;		// Selected / editing question
  };

struct Qst_Stats
  {
   unsigned NumCoursesWithQuestions;
   unsigned NumCoursesWithPluggableQuestions;
   unsigned NumQsts;
   double AvgQstsPerCourse;
   unsigned long NumHits;
   double AvgHitsPerCourse;
   double AvgHitsPerQuestion;
   double TotalScore;
   double AvgScorePerQuestion;
  };

struct Qst_AnswerDisplay
  {
   const char *ClassTch;
   const char *ClassStd;
   const char *Symbol;
  };

#endif
