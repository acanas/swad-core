// swad_exam_result.h: exams results

#ifndef _SWAD_EXA_RES
#define _SWAD_EXA_RES
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

#include "swad_test_print.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define ExaRes_RESULTS_BOX_ID		"exares_box"
#define ExaRes_RESULTS_TABLE_ID		"exares_table"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaRes_ShowMyResultsInCrs (void);
void ExaRes_ShowMyResultsInExa (void);
void ExaRes_ShowMyResultsInSes (void);

void ExaRes_SelUsrsToViewResults (void);
void ExaRes_ShowAllResultsInCrs (void);
void ExaRes_ShowAllResultsInExa (void);
void ExaRes_ShowAllResultsInSes (void);

void ExaRes_ShowExaResultAfterFinish (void);
void ExaRes_ShowOneExaResult (void);
void ExaRes_ComputeValidPrintScore (struct ExaPrn_Print *Print);
void ExaRes_ShowExamResultUser (const struct ExaSes_Session *Session,
				struct Usr_Data *UsrDat);

#endif
