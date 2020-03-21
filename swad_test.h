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
#include "swad_test_result.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Tst_MAX_TAGS_PER_QUESTION		   5

#define Tst_MAX_CHARS_TAG			(128 - 1)	// 127
#define Tst_MAX_BYTES_TAG			((Tst_MAX_CHARS_TAG + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Tst_MAX_OPTIONS_PER_QUESTION		  10
#define Tst_MAX_BYTES_INDEXES_ONE_QST		(Tst_MAX_OPTIONS_PER_QUESTION * (3 + 1))
#define Tst_MAX_BYTES_ANSWERS_ONE_QST		(Tst_MAX_OPTIONS_PER_QUESTION * (3 + 1))

#define Tst_MAX_CHARS_ANSWER_OR_FEEDBACK	(1024 - 1)	// 1023
#define Tst_MAX_BYTES_ANSWER_OR_FEEDBACK	((Tst_MAX_CHARS_ANSWER_OR_FEEDBACK + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Tst_MAX_BYTES_ANSWER_TYPE		  32

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

#define Tst_NUM_TYPES_ORDER_QST	5
typedef enum
  {
   Tst_ORDER_STEM                    = 0,
   Tst_ORDER_NUM_HITS                = 1,
   Tst_ORDER_AVERAGE_SCORE           = 2,
   Tst_ORDER_NUM_HITS_NOT_BLANK      = 3,
   Tst_ORDER_AVERAGE_SCORE_NOT_BLANK = 4,
  } Tst_QuestionsOrder_t;

struct Tst_Test
  {
   struct Tst_Tags Tags;
   unsigned NumQsts;
   long QstCodes[TstCfg_MAX_QUESTIONS_PER_TEST];	// Codes of the sent/received questions in a test
   char StrIndexesOneQst[TstCfg_MAX_QUESTIONS_PER_TEST]
                        [Tst_MAX_BYTES_INDEXES_ONE_QST + 1];	// 0 1 2 3, 3 0 2 1, etc.
   char StrAnswersOneQst[TstCfg_MAX_QUESTIONS_PER_TEST]
                        [Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];	// Answers selected by user
   bool AllAnsTypes;
   char ListAnsTypes[Tst_MAX_BYTES_LIST_ANSWER_TYPES + 1];
   Tst_QuestionsOrder_t SelectedOrder;
  };

struct Tst_Question
  {
   struct
     {
      char *Text;
      size_t Length;
     } Stem, Feedback;
   struct Media Media;
   bool Shuffle;
   struct
     {
      Tst_AnswerType_t Type;
      unsigned NumOptions;
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
void Tst_AssessTest (void);

void Tst_ComputeAndShowGrade (unsigned NumQsts,double Score,double MaxGrade);
double Tst_ComputeGrade (unsigned NumQsts,double Score,double MaxGrade);
void Tst_ShowGrade (double Grade,double MaxGrade);

void Tst_ShowTagList (unsigned NumTags,MYSQL_RES *mysql_res);

void Tst_WriteQstAndAnsTest (Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions,
			     struct UsrData *UsrDat,
                             unsigned NumQst,
                             long QstCod,
                             MYSQL_ROW row,
			     unsigned Visibility,
                             double *ScoreThisQst,bool *AnswerIsNotBlank);
void Tst_WriteQstStem (const char *Stem,const char *ClassStem,bool Visible);
void Tst_WriteQstFeedback (const char *Feedback,const char *ClassFeedback);

void Tst_RequestEditTests (void);
void Tst_RequestSelectTestsForGame (void);

void Tst_ListQuestionsToEdit (void);
void Tst_ListQuestionsToSelect (void);
bool Tst_GetOneQuestionByCod (long QstCod,MYSQL_RES **mysql_res);
void Tst_WriteParamEditQst (void);

void Tst_ResetGblTags (void);
void Tst_SetParamGblTags (const struct Tst_Tags *TagsSrc);
void Tst_GetParamGblTags (struct Tst_Tags *TagsDst);

unsigned Tst_GetNumAnswersQst (long QstCod);
unsigned Tst_GetAnswersQst (long QstCod,MYSQL_RES **mysql_res,bool Shuffle);
void Tst_GetCorrectAnswersFromDB (long QstCod,struct Tst_Question *Question);
void Tst_WriteAnswersEdit (long QstCod,Tst_AnswerType_t AnswerType);
bool Tst_CheckIfQuestionIsValidForGame (long QstCod);
void Tst_WriteAnsTF (char AnsTF);
void Tst_GetIndexesFromStr (const char StrIndexesOneQst[Tst_MAX_BYTES_INDEXES_ONE_QST + 1],	// 0 1 2 3, 3 0 2 1, etc.
			    unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION]);
void Tst_GetAnswersFromStr (const char StrAnswersOneQst[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1],
			    bool AnswersUsr[Tst_MAX_OPTIONS_PER_QUESTION]);
void Tst_ComputeScoreQst (const struct Tst_Question *Question,
                          unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION],
                          bool AnswersUsr[Tst_MAX_OPTIONS_PER_QUESTION],
			  double *ScoreThisQst,bool *AnswerIsNotBlank);
void Tst_WriteChoiceAnsViewMatch (long MchCod,unsigned QstInd,long QstCod,
				  unsigned NumCols,const char *Class,bool ShowResult);
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

int Tst_AllocateTextChoiceAnswer (struct Tst_Question *Question,unsigned NumOpt);

Tst_AnswerType_t Tst_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeBD);
void Tst_ReceiveQst (void);
bool Tst_CheckIfQstFormatIsCorrectAndCountNumOptions (struct Tst_Question *Question,
                                                      const struct Tst_Tags *Tags);

bool Tst_CheckIfQuestionExistsInDB (const struct Tst_Question *Question);

long Tst_GetIntAnsFromStr (char *Str);

void Tst_RequestRemoveSelectedQsts (void);
void Tst_RemoveSelectedQsts (void);
void Tst_RequestRemoveOneQst (void);
void Tst_RemoveOneQst (void);

void Tst_ChangeShuffleQst (void);

void Tst_SetParamGblQstCod (long QstCod);
long Tst_GetParamGblQstCod (void);
void Tst_PutParamGblQstCod (void);
void Tst_PutParamQstCod (long QstCod);

long Tst_InsertOrUpdateQstTagsAnsIntoDB (long QstCod,
                                         struct Tst_Question *Question,
                                         const struct Tst_Tags *Tags);

void Tst_RemoveCrsTests (long CrsCod);

void Tst_GetTestStats (Tst_AnswerType_t AnsType,struct Tst_Stats *Stats);

#endif
