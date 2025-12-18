// swad_exam_sheet.h: exams sheets

#ifndef _SWAD_EXA_SHE
#define _SWAD_EXA_SHE
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_exam_print.h"
#include "swad_exam_session.h"
#include "swad_view.h"

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

#define ExaShe_NUM_BLANK_OR_SOLVED 2
typedef enum
  {
   ExaShe_BLANK,
   ExaShe_SOLVED,
  } ExaShe_BlankOrSolved_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaShe_ListBlankSheets (void);
void ExaShe_PrintBlankSheets (void);

void ExaShe_ListSolvedAnswerSheets (void);
void ExaShe_PrintSolvedAnswerSheets (void);

void ExaShe_ReceiveAnswer (void);
void ExaShe_WriteJSToUpdateSheet (const struct ExaPrn_Print *Print,
	                          unsigned QstInd,
	                          const char *Id,int NumOpt);

#endif
