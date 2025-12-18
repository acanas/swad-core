// swad_question_text.h: test/exam/game text questions

#ifndef _SWAD_QST_TXT
#define _SWAD_QST_TXT
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

void QstTxt_WriteBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst);
void QstTxt_WriteCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
			  __attribute__((unused)) unsigned QstInd,
			  struct Qst_Question *Qst);
void QstTxt_WriteReadAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst);
void QstTxt_WriteEditAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst);

#endif
