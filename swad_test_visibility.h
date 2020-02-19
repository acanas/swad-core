// swad_test_visibility.h: visibility of test results

#ifndef _SWAD_TSV
#define _SWAD_TSV
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define TsV_NUM_ITEMS_VISIBILITY 5
typedef enum
  {
   TsV_VISIBLE_QST_ANS_TXT    = 0,	// Questions and answers text
   TsV_VISIBLE_FEEDBACK_TXT   = 1,	// Feedback text
   TsV_VISIBLE_CORRECT_ANSWER = 2,	// Correct answers
   TsV_VISIBLE_EACH_QST_SCORE = 3,	// Score of each question
   TsV_VISIBLE_TOTAL_SCORE    = 4,	// Total score
  } TsV_Visibility_t;
#define TsV_MIN_VISIBILITY 0						// Nothing visible
#define TsV_MAX_VISIBILITY ((1 << TsV_NUM_ITEMS_VISIBILITY) - 1)	// All visible
#define TsV_VISIBILITY_DEFAULT TsV_MAX_VISIBILITY

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TsV_ShowVisibilityIcons (unsigned SelectedVisibility);
void TsV_ShowVisibilityIconsAndTxt (unsigned SelectedVisibility);
void TsV_PutVisibilityCheckboxes (unsigned SelectedVisibility);
unsigned TsV_GetVisibilityFromForm (void);
unsigned TsV_GetVisibilityFromStr (const char *Str);

bool TsV_IsVisibleQstAndAnsTxt (unsigned Visibility);
bool TsV_IsVisibleFeedbackTxt (unsigned Visibility);
bool TsV_IsVisibleCorrectAns (unsigned Visibility);
bool TsV_IsVisibleEachQstScore (unsigned Visibility);
bool TsV_IsVisibleTotalScore (unsigned Visibility);

#endif
