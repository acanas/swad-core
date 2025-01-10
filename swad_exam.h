// swad_exam.h: exams

#ifndef _SWAD_EXA
#define _SWAD_EXA
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include "swad_old_new.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Exa_ResetExams (struct Exa_Exams *Exams);
void Exa_ResetExam (struct Exa_Exam *Exam);

void Exa_SeeAllExams (void);
void Exa_ListAllExams (struct Exa_Exams *Exams);

Usr_Can_t Exa_CheckIfICanEditExams (void);

void Exa_SeeOneExam (void);
void Exa_ShowOnlyOneExam (struct Exa_Exams *Exams,Frm_PutForm_t PutFormSession);
void Exa_ShowOnlyOneExamBegin (struct Exa_Exams *Exams,
			       Frm_PutForm_t PutFormSession);
void Exa_ShowOnlyOneExamEnd (void);

void Exa_SetCurrentExaCod (long ExaCod);
void Exa_PutPars (void *Exams);
void Exa_GetPars (struct Exa_Exams *Exams,Exa_CheckExaCod_t CheckExaCod);

void Exa_GetListExams (struct Exa_Exams *Exams,Exa_Order_t SelectedOrder);
void Exa_GetListSelectedExaCods (struct Exa_Exams *Exams);
void Exa_GetExamDataByCod (struct Exa_Exam *Exam);
void Exa_GetExamDataByFolder (struct Exa_Exam *Exam);
void Exa_FreeListExams (struct Exa_Exams *Exams);

void Exa_AskRemExam (void);
void Exa_RemoveExam (void);
void Exa_RemoveCrsExams (long CrsCod);

void Exa_HideExam (void);
void Exa_UnhideExam (void);

void Exa_ReqCreatOrEditExam (void);
void Exa_PutFormsOneExam (struct Exa_Exams *Exams,OldNew_OldNew_t OldNewExam);

void Exa_ReceiveExam (void);

//-------------------------------- Figures ------------------------------------
void Exa_GetAndShowExamsStats (void);

#endif
