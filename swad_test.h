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

#include "swad_exam.h"
#include "swad_game.h"
#include "swad_media.h"
#include "swad_test_config.h"
#include "swad_test_print.h"
#include "swad_test_type.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Tst_MAX_CHARS_ANSWER_OR_FEEDBACK	(1024 - 1)	// 1023
#define Tst_MAX_BYTES_ANSWER_OR_FEEDBACK	((Tst_MAX_CHARS_ANSWER_OR_FEEDBACK + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Tst_MAX_BYTES_ANSWER_TYPE		  32

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

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

struct Tst_Test
  {
   struct Tag_Tags Tags;		// Selected tags
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

void Tst_ShowTagList (unsigned NumTags,MYSQL_RES *mysql_res);

void Tst_ListQuestionForEdition (const struct Tst_Question *Question,
                                 unsigned QstInd,bool QuestionExists,
                                 const char *Anchor);
void Tst_WriteNumQst (unsigned NumQst);
void Tst_WriteAnswerType (Tst_AnswerType_t AnswerType);
void Tst_WriteQstStem (const char *Stem,const char *ClassStem,bool Visible);
void Tst_WriteQstFeedback (const char *Feedback,const char *ClassFeedback);

void Tst_RequestEditTests (void);
void Tst_RequestSelectTestsForSet (struct Exa_Exams *Exams);
void Tst_RequestSelectTestsForGame (struct Gam_Games *Games);

void Tst_ListQuestionsToEdit (void);
void Tst_ListQuestionsToSelectForSet (struct Exa_Exams *Exams);
void Tst_ListQuestionsToSelectForGame (struct Gam_Games *Games);

void Tst_GenerateChoiceIndexes (struct TstPrn_PrintedQuestion *PrintedQuestion,
				bool Shuffle);

void Tst_WriteParamEditQst (const struct Tst_Test *Test);

unsigned Tst_GetNumAnswersQst (long QstCod);
void Tst_GetAnswersQst (struct Tst_Question *Question,MYSQL_RES **mysql_res,
                        bool Shuffle);
void Tst_WriteAnswersBank (const struct Tst_Question *Question);
bool Tst_CheckIfQuestionIsValidForGame (long QstCod);
void Tst_WriteAnsTF (char AnsTF);
void Tst_GetChoiceAns (struct Tst_Question *Question,MYSQL_RES *mysql_res);

void Tst_WriteParamQstCod (unsigned NumQst,long QstCod);

void Tst_CheckIfNumberOfAnswersIsOne (const struct Tst_Question *Question);

unsigned long Tst_GetTagsQst (long QstCod,MYSQL_RES **mysql_res);
void Tst_GetAndWriteTagsQst (long QstCod);

void Tst_ShowFormConfig (void);

bool Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown (void);

unsigned Tst_CountNumQuestionsInList (const char *ListQuestions);

void Tst_ShowFormEditOneQst (void);

void Tst_QstConstructor (struct Tst_Question *Question);
void Tst_QstDestructor (struct Tst_Question *Question);

bool Tst_AllocateTextChoiceAnswer (struct Tst_Question *Question,unsigned NumOpt);

Tst_AnswerType_t Tst_GetQstAnswerTypeFromDB (long QstCod);
bool Tst_GetQstDataFromDB (struct Tst_Question *Question);
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

long Tst_GetParamQstCod (void);
void Tst_PutParamQstCod (void *QstCod);

void Tst_InsertOrUpdateQstTagsAnsIntoDB (struct Tst_Question *Question);

void Tst_UpdateQstScoreInDB (struct TstPrn_PrintedQuestion *PrintedQuestion);

void Tst_RemoveCrsTests (long CrsCod);

void Tst_GetTestStats (Tst_AnswerType_t AnsType,struct Tst_Stats *Stats);

#endif
