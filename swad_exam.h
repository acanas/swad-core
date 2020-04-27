// swad_exam.h: exams

#ifndef _SWAD_EXA
#define _SWAD_EXA
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#include "swad_date.h"
#include "swad_scope.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Exa_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Exa_MAX_BYTES_TITLE	((Exa_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define ExaSet_MAX_CHARS_TITLE	(128 - 1)	// 127
#define ExaSet_MAX_BYTES_TITLE	((ExaSet_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Exa_NUM_ORDERS 3
typedef enum
  {
   Exa_ORDER_BY_START_DATE = 0,
   Exa_ORDER_BY_END_DATE   = 1,
   Exa_ORDER_BY_TITLE      = 2,
  } Exa_Order_t;
#define Exa_ORDER_DEFAULT Exa_ORDER_BY_START_DATE

#define Exa_NUM_ANS_TYPES	2
typedef enum
  {
   Exa_ANS_UNIQUE_CHOICE   = 0,
   Exa_ANS_MULTIPLE_CHOICE = 1,
  } Exa_AnswerType_t;
#define Exa_ANSWER_TYPE_DEFAULT Exa_ANS_UNIQUE_CHOICE

struct Exa_ExamSelected
  {
   long ExaCod;		// Exam code
   bool Selected;	// Is this exam selected when seeing match results?
  };

/* Exams context */
struct Exa_Exams
  {
   bool LstIsRead;		// Is the list already read from database...
				// ...or it needs to be read?
   unsigned Num;		// Total number of exams
   unsigned NumSelected;	// Number of exams selected
   struct Exa_ExamSelected *Lst;// List of exams
   Exa_Order_t SelectedOrder;
   unsigned CurrentPage;
   char *ListQuestions;
   char *ExaCodsSelected;	// String with selected exam codes separated by separator multiple
   long ExaCod;			// Selected/current exam code
   long SetCod;			// Selected/current set code
   long EvtCod;			// Selected/current match code
   unsigned SetInd;		// Current set index
   unsigned QstInd;		// Current question index
  };

struct Exa_Exam
  {
   long ExaCod;			// Exam code
   long CrsCod;			// Course code
   long UsrCod;			// Author code
   double MaxGrade;		// Score range [0...max.score]
				// will be converted to
				// grade range [0...max.grade]
   unsigned Visibility;		// Visibility of results
   char Title[Exa_MAX_BYTES_TITLE + 1];
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   bool Hidden;			// Exam is hidden
   unsigned NumSets;		// Number of sets in the exam
   unsigned NumQsts;		// Number of questions in the exam
   unsigned NumEvts;		// Number of events in the exam
   unsigned NumUnfinishedEvts;	// Number of unfinished events in the exam
  };

struct ExaSet_Set
  {
   long ExaCod;			// Exam code
   long SetCod;			// Set code
   unsigned SetInd;		// Set index (position in the exam)
   unsigned NumQstsToExam;	// Number of questions in this set taht will appear in the exam
   char Title[ExaSet_MAX_BYTES_TITLE + 1];	// Title of the set
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Exa_ResetExams (struct Exa_Exams *Exams);
void Exa_ResetExam (struct Exa_Exam *Exam);

void Exa_SeeAllExams (void);
void Exa_SeeOneExam (void);
void Exa_ShowOnlyOneExam (struct Exa_Exams *Exams,
			  struct Exa_Exam *Exam,
			  bool ListExamQuestions,
			  bool PutFormNewMatch);
void Exa_ShowOnlyOneExamBegin (struct Exa_Exams *Exams,
			       struct Exa_Exam *Exam,
			       bool ListExamQuestions,
			       bool PutFormNewMatch);
void Exa_ShowOnlyOneExamEnd (void);

void Exa_SetCurrentExaCod (long ExaCod);
void Exa_PutParams (void *Exams);
void Exa_PutParamExamCod (long ExaCod);
long Exa_GetParamExamCod (void);
long ExaSet_GetParamSetCod (void);
void Exa_GetParams (struct Exa_Exams *Exams);

void Exa_GetListExams (struct Exa_Exams *Exams,Exa_Order_t SelectedOrder);
void Exa_GetListSelectedExaCods (struct Exa_Exams *Exams);
void Exa_GetDataOfExamByCod (struct Exa_Exam *Exam);
void Exa_GetDataOfExamByFolder (struct Exa_Exam *Exam);
void Exa_FreeListExams (struct Exa_Exams *Exams);

void Exa_AskRemExam (void);
void Exa_RemoveExam (void);
void Exa_RemoveExamsCrs (long CrsCod);

void Exa_HideExam (void);
void Exa_UnhideExam (void);

void Exa_RequestCreatOrEditExam (void);

void ExaSet_RecFormSet (void);
void ExaSet_ChangeSetTitle (void);
void ExaSet_ChangeNumQstsToExam (void);

void Exa_RecFormExam (void);
bool Mch_CheckIfMatchIsAssociatedToGrp (long EvtCod,long GrpCod);

unsigned ExaSet_GetNumSetsExam (long ExaCod);
unsigned ExaSet_GetNumQstsExam (long ExaCod);

void ExaSet_RequestCreatOrEditSet (void);
void Exa_RequestNewQuestion (void);
void Exa_ListQuestionsToSelect (void);

void Exa_PutParamQstInd (unsigned QstInd);
unsigned Exa_GetParamQstInd (void);
long Exa_GetQstCodFromQstInd (long ExaCod,unsigned QstInd);

unsigned Exa_GetPrevQuestionIndexInExam (long ExaCod,unsigned QstInd);
unsigned Exa_GetNextQuestionIndexInExam (long ExaCod,unsigned QstInd);

void Exa_AddQuestionsToExam (void);

void ExaSet_RequestRemoveSet (void);
void ExaSet_RemoveSet (void);

void ExaSet_MoveUpSet (void);
void ExaSet_MoveDownSet (void);

void Exa_RequestRemoveQst (void);
void Exa_RemoveQst (void);

void Exa_MoveUpQst (void);
void Exa_MoveDownQst (void);

void Exa_PutButtonNewEvent (struct Exa_Exams *Exams,long ExaCod);
void Exa_RequestNewEvent (void);

unsigned Exa_GetNumCoursesWithExams (Hie_Level_t Scope);
unsigned Exa_GetNumExams (Hie_Level_t Scope);
double Exa_GetNumQstsPerCrsExam (Hie_Level_t Scope);

void Exa_ShowTstTagsPresentInAnExam (long ExaCod);

void Exa_GetScoreRange (long ExaCod,double *MinScore,double *MaxScore);

#endif
