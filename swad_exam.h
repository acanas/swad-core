// swad_exam.h: exams

#ifndef _SWAD_EXA
#define _SWAD_EXA
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_exam_type.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Exa_ResetExams (struct Exa_Exams *Exams);
void Exa_ResetExam (struct Exa_Exam *Exam);

void Exa_SeeAllExams (void);

bool Exa_CheckIfICanEditExams (void);

void Exa_SeeOneExam (void);
void Exa_ShowOnlyOneExam (struct Exa_Exams *Exams,
			  struct Exa_Exam *Exam,
			  struct ExaSes_Session *Session,
			  bool PutFormSession);
void Exa_ShowOnlyOneExamBegin (struct Exa_Exams *Exams,
			       struct Exa_Exam *Exam,
			       struct ExaSes_Session *Session,
			       bool PutFormSession);
void Exa_ShowOnlyOneExamEnd (void);

void Exa_SetCurrentExaCod (long ExaCod);
void Exa_PutParams (void *Exams);
void Exa_PutParamExamCod (long ExaCod);
long Exa_GetParamExamCod (void);
void Exa_GetParams (struct Exa_Exams *Exams);

void Exa_GetListExams (struct Exa_Exams *Exams,Exa_Order_t SelectedOrder);
void Exa_GetListSelectedExaCods (struct Exa_Exams *Exams);
void Exa_GetDataOfExamByCod (struct Exa_Exam *Exam);
void Exa_GetDataOfExamByFolder (struct Exa_Exam *Exam);
void Exa_FreeListExams (struct Exa_Exams *Exams);

void Exa_GetExamTxtFromDB (long ExaCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

void Exa_AskRemExam (void);
void Exa_RemoveExam (void);
void Exa_RemoveCrsExams (long CrsCod);

void Exa_HideExam (void);
void Exa_UnhideExam (void);

void Exa_RequestCreatOrEditExam (void);
void Exa_PutFormsOneExam (struct Exa_Exams *Exams,
			  struct Exa_Exam *Exam,
			  struct ExaSet_Set *Set,
			  bool ItsANewExam);
void Exa_PutFormEditionExam (struct Exa_Exams *Exams,
			     struct Exa_Exam *Exam,
			     char Txt[Cns_MAX_BYTES_TEXT + 1],
			     bool ItsANewExam);

void Exa_ReceiveFormExam (void);

void Exa_PutParamQstInd (unsigned QstInd);
unsigned Exa_GetParamQstInd (void);
long Exa_GetQstCodFromQstInd (long ExaCod,unsigned QstInd);

unsigned Exa_GetPrevQuestionIndexInExam (long ExaCod,unsigned QstInd);
unsigned Exa_GetNextQuestionIndexInExam (long ExaCod,unsigned QstInd);

bool Exa_CheckIfEditable (const struct Exa_Exam *Exam);

unsigned Exa_GetNumCoursesWithExams (Hie_Level_t Scope);
unsigned Exa_GetNumExams (Hie_Level_t Scope);
double Exa_GetNumQstsPerCrsExam (Hie_Level_t Scope);

void Exa_GetScoreRange (long ExaCod,double *MinScore,double *MaxScore);

#endif
