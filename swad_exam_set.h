// swad_exam_set.h: set of questions in exams

#ifndef _SWAD_EXA_SET
#define _SWAD_EXA_SET
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
#include "swad_question.h"
#include "swad_question_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaSet_PutParsOneSet (void *Exams);

void ExaSet_GetSetDataByCod (struct ExaSet_Set *Set);

void ExaSet_ReceiveSet (void);
void ExaSet_ChangeSetTitle (void);
void ExaSet_ChangeNumQstsToExam (void);

void ExaSet_ReqSelectQstsToAddToSet (void);
void ExaSet_ListQstsToAddToSet (void);

void ExaSet_ListExamSets (struct Exa_Exams *Exams);

void ExaSet_ResetSet (struct ExaSet_Set *Set);
void ExaSet_GetSetDataFromRow (MYSQL_RES *mysql_res,struct ExaSet_Set *Set);

Qst_AnswerType_t ExaSet_GetAnswerType (long QstCod);
void ExaSet_GetQstDataFromDB (struct Qst_Question *Question);
ExaSet_Validity_t ExaSet_GetInvalidOrValidFromYN (char Ch);

void ExaSet_AddQstsToSet (void);

void ExaSet_ReqRemSet (void);
void ExaSet_RemoveSet (void);

void ExaSet_MoveUpSet (void);
void ExaSet_MoveDownSet (void);

void ExaSet_ReqRemQstFromSet (void);
void ExaSet_RemoveQstFromSet (void);

void ExaSet_ValidateQst (void);
void ExaSet_InvalidateQst (void);

void ExaSet_WriteSetTitle (const struct ExaSet_Set *Set);

#endif
