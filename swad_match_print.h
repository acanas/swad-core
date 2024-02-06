// swad_match_print.h: matches prints in games using remote control

#ifndef _SWAD_MCH_PRN
#define _SWAD_MCH_PRN
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

struct MchPrn_NumQuestions
  {
   unsigned All;	// Total number of questions
   unsigned NotBlank;	// Answered questions
  };

struct MchPrn_Print
  {
   long MchCod;		// Match code
   long UsrCod;		// User who answered the match
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   struct MchPrn_NumQuestions NumQsts;	// Number of questions
   double Score;	// Total score of the match for this user
   struct TstPrn_PrintedQuestion PrintedQuestions[TstCfg_MAX_QUESTIONS_PER_TEST];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void MchPrn_ResetPrint (struct MchPrn_Print *Print);

void MchPrn_ComputeScoreAndUpdateMyMatchPrintInDB (long MchCod);

void MchPrn_GetMatchPrintDataByMchCodAndUsrCod (struct MchPrn_Print *Print);

#endif
