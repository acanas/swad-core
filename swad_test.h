// swad_test.h: self-assessment tests

#ifndef _SWAD_TST
#define _SWAD_TST
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

#include "swad_exam.h"
#include "swad_game.h"
#include "swad_media.h"
#include "swad_question_type.h"
#include "swad_test_config.h"
#include "swad_test_print.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Tst_MAX_CHARS_ANSWER_OR_FEEDBACK	(1024 - 1)	// 1023
#define Tst_MAX_BYTES_ANSWER_OR_FEEDBACK	((Tst_MAX_CHARS_ANSWER_OR_FEEDBACK + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Qst_MAX_BYTES_ANSWER_TYPE		  32

#define Tst_SCORE_MAX	10	// Maximum score of a test (10 in Spain). Must be unsigned! // TODO: Make this configurable by teachers

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

struct Tst_Test
  {
   struct Tag_Tags Tags;		// Selected tags
   struct Qst_AnswerTypes AnswerTypes;	// Selected answer types
   Qst_QuestionsOrder_t SelectedOrder;	// Order for listing questions
   unsigned NumQsts;			// Number of questions
   struct Qst_Question Question;	// Selected / editing question
  };

typedef enum
  {
   Tst_SHOW_TEST_TO_ANSWER,		// Showing a test to a student
   Tst_SHOW_TEST_RESULT,		// Showing the assessment of a test
   Tst_EDIT_QUESTIONS,			// Editing test questions
   Tst_SELECT_QUESTIONS_FOR_EXAM,	// Selecting test questions for a set of questions in an exam
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
void Tst_ReceiveTestDraft (void);
void Tst_AssessTest (void);

void Qst_ListQuestionForEdition (struct Qst_Question *Question,
                                 unsigned QstInd,bool QuestionExists,
                                 const char *Anchor);
void Qst_WriteNumQst (unsigned NumQst,const char *Class);
void Qst_WriteAnswerType (Qst_AnswerType_t AnswerType,const char *Class);
void Qst_WriteQstStem (const char *Stem,const char *ClassStem,bool Visible);
void Qst_WriteQstFeedback (const char *Feedback,const char *ClassFeedback);

void Qst_RequestEditQsts (void);
void Qst_RequestSelectQstsForExamSet (struct Exa_Exams *Exams);
void Qst_RequestSelectQstsForGame (struct Gam_Games *Games);

void Qst_ListQuestionsToEdit (void);
void Qst_ListQuestionsToSelectForExamSet (struct Exa_Exams *Exams);
void Qst_ListQuestionsToSelectForGame (struct Gam_Games *Games);

void Qst_PutParamsEditQst (void *Test);

unsigned Qst_GetNumAnswersQst (long QstCod);
void Qst_GetAnswersQst (struct Qst_Question *Question,MYSQL_RES **mysql_res,
                        bool Shuffle);

void Qst_ChangeFormatAnswersText (struct Qst_Question *Question);
void Qst_ChangeFormatAnswersFeedback (struct Qst_Question *Question);

void Qst_WriteAnswersBank (struct Qst_Question *Question,
                           const char *ClassTxt,
                           const char *ClassFeedback);
void Qst_WriteAnsTF (char AnsTF);

void Qst_WriteParamQstCod (unsigned NumQst,long QstCod);

void Qst_CheckIfNumberOfAnswersIsOne (const struct Qst_Question *Question);

void Tst_ShowFormConfig (void);

bool Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown (void);

unsigned Tst_CountNumQuestionsInList (const char *ListQuestions);

void Qst_ShowFormEditOneQst (void);

void Qst_QstConstructor (struct Qst_Question *Question);
void Qst_QstDestructor (struct Qst_Question *Question);

bool Qst_AllocateTextChoiceAnswer (struct Qst_Question *Question,unsigned NumOpt);

Qst_AnswerType_t Qst_GetQstAnswerTypeFromDB (long QstCod);
bool Qst_GetQstDataFromDB (struct Qst_Question *Question);
Qst_AnswerType_t Qst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeDB);
void Qst_ReceiveQst (void);
bool Qst_CheckIfQstFormatIsCorrectAndCountNumOptions (struct Qst_Question *Question);

bool Qst_CheckIfQuestionExistsInDB (struct Qst_Question *Question);

long Qst_GetIntAnsFromStr (char *Str);

void Qst_RequestRemoveSelectedQsts (void);
void Qst_RemoveSelectedQsts (void);
void Qst_RequestRemoveOneQst (void);
void Qst_RemoveOneQst (void);

void Qst_ChangeShuffleQst (void);

long Qst_GetParamQstCod (void);
void Qst_PutParamQstCod (void *QstCod);

void Qst_InsertOrUpdateQstTagsAnsIntoDB (struct Qst_Question *Question);

void Qst_UpdateQstScoreInDB (struct TstPrn_PrintedQuestion *PrintedQuestion);

void Tst_RemoveCrsTests (long CrsCod);
void Qst_RemoveCrsQsts (long CrsCod);

void Tst_GetTestStats (Qst_AnswerType_t AnsType,struct Tst_Stats *Stats);

unsigned Qst_DB_GetShuffledAnswersIndexes (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Question);

#endif
