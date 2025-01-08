// swad_test_visibility.h: visibility of test results

#ifndef _SWAD_TST_VIS
#define _SWAD_TST_VIS
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

#include "swad_user.h"

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

#define TstVis_NUM_ITEMS_VISIBILITY 5
typedef enum
  {
   TstVis_VISIBLE_QST_ANS_TXT    = 0,	// Questions and answers text
   TstVis_VISIBLE_FEEDBACK_TXT   = 1,	// Feedback text
   TstVis_VISIBLE_CORRECT_ANSWER = 2,	// Correct answers
   TstVis_VISIBLE_EACH_QST_SCORE = 3,	// Score of each question
   TstVis_VISIBLE_TOTAL_SCORE    = 4,	// Total score
  } TstVis_Visibility_t;
#define TstVis_MIN_VISIBILITY 0						// Nothing visible
#define TstVis_MAX_VISIBILITY ((1 << TstVis_NUM_ITEMS_VISIBILITY) - 1)	// All visible
#define TstVis_VISIBILITY_DEFAULT TstVis_MAX_VISIBILITY

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TstVis_ShowVisibilityIcons (unsigned SelectedVisibility,
                                 HidVis_HiddenOrVisible_t HiddenOrVisible);
void TstVis_PutVisibilityCheckboxes (unsigned SelectedVisibility);
unsigned TstVis_GetVisibilityFromForm (void);
unsigned TstVis_GetVisibilityFromStr (const char *Str);

bool TstVis_IsVisibleQstAndAnsTxt (unsigned Visibility);
bool TstVis_IsVisibleFeedbackTxt (unsigned Visibility);
bool TstVis_IsVisibleCorrectAns (unsigned Visibility);
bool TstVis_IsVisibleEachQstScore (unsigned Visibility);
bool TstVis_IsVisibleTotalScore (unsigned Visibility);

#endif
