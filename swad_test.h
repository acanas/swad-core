// swad_test.h: self-assessment tests

#ifndef _SWAD_TST
#define _SWAD_TST
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

#include "swad_game.h"
#include "swad_media.h"
#include "swad_test_config.h"
// #include "swad_test_exam.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Tst_MAX_TAGS_PER_QUESTION		   5

#define Tst_MAX_CHARS_TAG			(128 - 1)	// 127
#define Tst_MAX_BYTES_TAG			((Tst_MAX_CHARS_TAG + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Tst_MAX_CHARS_ANSWER_OR_FEEDBACK	(1024 - 1)	// 1023
#define Tst_MAX_BYTES_ANSWER_OR_FEEDBACK	((Tst_MAX_CHARS_ANSWER_OR_FEEDBACK + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Tst_MAX_BYTES_ANSWER_TYPE		  32

#define Tst_MAX_OPTIONS_PER_QUESTION		  10

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

struct Tst_Tags
  {
   unsigned Num;
   bool All;
   char *List;
   char Txt[Tst_MAX_TAGS_PER_QUESTION][Tst_MAX_BYTES_TAG + 1];
  };

#define Tst_NUM_ANS_TYPES	6
#define Tst_MAX_BYTES_LIST_ANSWER_TYPES	(Tst_NUM_ANS_TYPES * (Cns_MAX_DECIMAL_DIGITS_UINT + 1))
typedef enum
  {
   Tst_ANS_INT             = 0,
   Tst_ANS_FLOAT           = 1,
   Tst_ANS_TRUE_FALSE      = 2,
   Tst_ANS_UNIQUE_CHOICE   = 3,
   Tst_ANS_MULTIPLE_CHOICE = 4,
   Tst_ANS_TEXT            = 5,
   Tst_ANS_ALL             = 6,	// All/any type of answer
  } Tst_AnswerType_t;

struct Tst_AnswerTypes
  {
   bool All;
   char List[Tst_MAX_BYTES_LIST_ANSWER_TYPES + 1];
  };

#define Tst_NUM_TYPES_ORDER_QST	5
typedef enum
  {
   Tst_ORDER_STEM                    = 0,
   Tst_ORDER_NUM_HITS                = 1,
   Tst_ORDER_AVERAGE_SCORE           = 2,
   Tst_ORDER_NUM_HITS_NOT_BLANK      = 3,
   Tst_ORDER_AVERAGE_SCORE_NOT_BLANK = 4,
  } Tst_QuestionsOrder_t;
#define Tst_DEFAULT_ORDER Tst_ORDER_STEM

struct Tst_Question
  {
   long QstCod;
   struct Tst_Tags Tags;
   time_t EditTime;
   struct
     {
      char *Text;
      size_t Length;
     } Stem, Feedback;
   struct Media Media;
   struct
     {
      Tst_AnswerType_t Type;
      unsigned NumOptions;
      bool Shuffle;
      char TF;
      struct
	{
	 bool Correct;
	 char *Text;
	 char *Feedback;
	 struct Media Media;
	} Options[Tst_MAX_OPTIONS_PER_QUESTION];
      long Integer;
      double FloatingPoint[2];
     } Answer;
  };

struct Tst_Test
  {
   struct Tst_Tags Tags;		// Selected tags
   struct Tst_AnswerTypes AnswerTypes;	// Selected answer types
   Tst_QuestionsOrder_t SelectedOrder;	// Order for listing questions
   unsigned NumQsts;			// Number of questions
   struct Tst_Question Question;	// Selected / editing question
  };

typedef enum
  {
   Tst_SHOW_TEST_TO_ANSWER,		// Showing a test to a student
   Tst_SHOW_TEST_RESULT,		// Showing the assessment of a test
   Tst_EDIT_TEST,			// Editing test questions
   Tst_SELECT_QUESTIONS_FOR_GAME,	// Selecting test questions for a game
  } Tst_ActionToDoWithQuestions_t;

struct Tst_Stats
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

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Tst_RequestTest (void);
void Tst_ShowNewTest (void);
void Tst_RequestAssessTest (void);
void Tst_AssessTest (void);

void Tst_ShowTagList (unsigned NumTags,MYSQL_RES *mysql_res);

void Tst_WriteNumQst (unsigned NumQst);
void Tst_WriteAnswerType (Tst_AnswerType_t AnswerType);
void Tst_WriteQstStem (const char *Stem,const char *ClassStem,bool Visible);
void Tst_WriteQstFeedback (const char *Feedback,const char *ClassFeedback);

void Tst_RequestEditTests (void);
void Tst_RequestSelectTestsForGame (void);

void Tst_ListQuestionsToEdit (void);
void Tst_ListQuestionsToSelect (void);
bool Tst_GetOneQuestionByCod (long QstCod,MYSQL_RES **mysql_res);
void Tst_WriteParamEditQst (const struct Tst_Test *Test);

unsigned Tst_GetNumAnswersQst (long QstCod);
void Tst_GetAnswersQst (struct Tst_Question *Question,MYSQL_RES **mysql_res,
                        bool Shuffle);
void Tst_WriteAnswersListing (struct Tst_Question *Question);
bool Tst_CheckIfQuestionIsValidForGame (long QstCod);
void Tst_WriteAnsTF (char AnsTF);
void Tst_GetChoiceAns (struct Tst_Question *Question,MYSQL_RES *mysql_res);
void Tst_CheckIfNumberOfAnswersIsOne (const struct Tst_Question *Question);

unsigned long Tst_GetTagsQst (long QstCod,MYSQL_RES **mysql_res);
void Tst_GetAndWriteTagsQst (long QstCod);

void Tst_ShowFormConfig (void);
void Tst_EnableTag (void);
void Tst_DisableTag (void);
void Tst_RenameTag (void);

bool Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown (void);

void Tst_ShowFormEditOneQst (void);

void Tst_QstConstructor (struct Tst_Question *Question);
void Tst_QstDestructor (struct Tst_Question *Question);

bool Tst_AllocateTextChoiceAnswer (struct Tst_Question *Question,unsigned NumOpt);

Tst_AnswerType_t Tst_GetQstAnswerType (long QstCod);
void Tst_GetQstDataFromDB (struct Tst_Question *Question,
                           char Stem[Cns_MAX_BYTES_TEXT + 1],
                           char Feedback[Cns_MAX_BYTES_TEXT + 1]);
Tst_AnswerType_t Tst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeBD);
void Tst_ReceiveQst (void);
bool Tst_CheckIfQstFormatIsCorrectAndCountNumOptions (struct Tst_Question *Question);

bool Tst_CheckIfQuestionExistsInDB (struct Tst_Question *Question);

long Tst_GetIntAnsFromStr (char *Str);

void Tst_RequestRemoveSelectedQsts (void);
void Tst_RemoveSelectedQsts (void);
void Tst_RequestRemoveOneQst (void);
void Tst_RemoveOneQst (void);

void Tst_ChangeShuffleQst (void);

void Tst_PutParamQstCod (void *QstCodPtr);

void Tst_InsertOrUpdateQstTagsAnsIntoDB (struct Tst_Question *Question);

void Tst_RemoveCrsTests (long CrsCod);

void Tst_GetTestStats (Tst_AnswerType_t AnsType,struct Tst_Stats *Stats);

#endif
