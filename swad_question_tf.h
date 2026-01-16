// swad_question_tf.h: test/exam/game true/false questions

#ifndef _SWAD_QST_TF
#define _SWAD_QST_TF
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

void QstTF__GetCorrectAndComputeAnsScore (const char *Table,
					  struct Qst_PrintedQuestion *PrintedQst,
				          struct Qst_Question *Qst);

void QstTF__WriteCorrAns (struct Qst_Question *Qst,
			  const char *ClassTxt,
			  __attribute__((unused)) const char *ClassFeedback);

//--------------------------------- Tests -------------------------------------

void QstTF__WriteTstFillAns (const struct Qst_PrintedQuestion *PrintedQst,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst);
void QstTF__WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     __attribute__((unused)) const char *ClassTxt,
			     __attribute__((unused)) const char *ClassFeedback);

//--------------------------------- Exams -------------------------------------

void QstTF__WriteExaBlnkQstOptions (__attribute__((unused)) const struct ExaPrn_Print *Print,
				    __attribute__((unused)) unsigned QstInd,
				    __attribute__((unused)) struct Qst_Question *Qst);
void QstTF__WriteExaFillAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst);
void QstTF__WriteExaBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst);
void QstTF__WriteExaCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
			     __attribute__((unused)) unsigned QstInd,
			     struct Qst_Question *Qst);
void QstTF__WriteExaReadAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst);
void QstTF__WriteExaEditAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst);

#endif
