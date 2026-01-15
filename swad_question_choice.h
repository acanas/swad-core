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

void QstCho_WritePrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			  struct Qst_Question *Qst,
			  Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			  const char *ClassTxt,
			  const char *ClassFeedback);

void QstCho_WriteBlnkAns (const struct Qst_Question *Qst);
void QstCho_WriteCorrAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst);
void QstCho_WriteReadAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst);
void QstCho_WriteEditAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst);

#endif
