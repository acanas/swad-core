// swad_question_choice.h: test/exam/game choice questions

#ifndef _SWAD_QST_CHO
#define _SWAD_QST_CHO
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

#include "swad_exam_print.h"
#include "swad_question.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void QstCho_GetAnsFromForm (struct Qst_Question *Qst);

void QstCho_GetCorrectAndComputeAnsScore (const char *Table,
					  struct Qst_PrintedQuestion *PrintedQst,
				          struct Qst_Question *Qst);

void QstCho_WriteCorrAns (struct Qst_Question *Qst,
			  const char *ClassTxt,
			  const char *ClassFeedback);
//--------------------------------- Tests -------------------------------------

void QstCho_WriteTstFillAns (const struct Qst_PrintedQuestion *PrintedQst,
                             unsigned QstInd,struct Qst_Question *Qst);
void QstCho_WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     const char *ClassTxt,
			     const char *ClassFeedback);

//--------------------------------- Exams -------------------------------------

void QstCho_WriteExaBlnkQstOptions (const struct ExaPrn_Print *Print,
				    unsigned QstInd,struct Qst_Question *Qst);
void QstCho_WriteExaFillAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst);
void QstCho_WriteExaBlnkAns (const struct Qst_Question *Qst);
void QstCho_WriteExaCorrAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst);
void QstCho_WriteExaReadAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst);
void QstCho_WriteExaEditAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst);

#endif
