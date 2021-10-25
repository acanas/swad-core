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
#include "swad_question.h"
#include "swad_question_type.h"
#include "swad_test_config.h"
#include "swad_test_print.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Tst_SCORE_MAX	10	// Maximum score of a test (10 in Spain). Must be unsigned! // TODO: Make this configurable by teachers

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

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

void Qst_Constructor (struct Qst_Questions *Questions);
void Qst_Destructor (struct Qst_Questions *Questions);

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
void Qst_ShowFormRequestEditQsts (struct Qst_Questions *Questions);
void Qst_RequestSelectQstsForExamSet (struct Exa_Exams *Exams);
void Qst_RequestSelectQstsForGame (struct Gam_Games *Games);
void Qst_ShowFormRequestSelectQstsForExamSet (struct Exa_Exams *Exams,
                                              struct Qst_Questions *Questions);
void Qst_ShowFormRequestSelectQstsForGame (struct Gam_Games *Games,
                                           struct Qst_Questions *Questions);

void Qst_ListOneOrMoreQstsForEdition (struct Qst_Questions *Questions,
                                      MYSQL_RES *mysql_res);
void Qst_ListOneOrMoreQstsForSelectionForExamSet (struct Exa_Exams *Exams,
						  unsigned NumQsts,
                                                  MYSQL_RES *mysql_res);
void Qst_ListOneOrMoreQstsForSelectionForGame (struct Gam_Games *Games,
					       unsigned NumQsts,
                                               MYSQL_RES *mysql_res);

void Qst_PutParamsEditQst (void *Questions);

unsigned Qst_GetNumAnswersQst (long QstCod);
void Qst_GetAnswersQst (struct Qst_Question *Question,MYSQL_RES **mysql_res,
                        bool Shuffle);

void Qst_WriteAnswersBank (struct Qst_Question *Question,
                           const char *ClassTxt,
                           const char *ClassFeedback);
void Qst_WriteAnsTF (char AnsTF);

void Qst_WriteParamQstCod (unsigned NumQst,long QstCod);

void Qst_CheckIfNumberOfAnswersIsOne (const struct Qst_Question *Question);

bool Tst_GetParamsTst (struct Qst_Questions *Questions,
                       Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions);
void Tst_ShowFormConfig (void);

bool Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown (void);

unsigned Tst_CountNumQuestionsInList (const char *ListQuestions);

void Qst_ShowFormEditOneQst (void);

void Qst_QstConstructor (struct Qst_Question *Question);
void Qst_QstDestructor (struct Qst_Question *Question);

bool Qst_AllocateTextChoiceAnswer (struct Qst_Question *Question,unsigned NumOpt);

Qst_AnswerType_t Qst_GetQstAnswerTypeFromDB (long QstCod);
bool Qst_GetQstDataFromDB (struct Qst_Question *Question);
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

void Tst_RemoveCrsTests (long CrsCod);
void Qst_RemoveCrsQsts (long CrsCod);

void Tst_GetTestStats (Qst_AnswerType_t AnsType,struct Tst_Stats *Stats);

unsigned Qst_DB_GetShuffledAnswersIndexes (MYSQL_RES **mysql_res,
                                           const struct Qst_Question *Question);

#endif
