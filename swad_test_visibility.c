// swad_test_visibility.c: visibility of test exams

/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For malloc, free

#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_test_visibility.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Show visibility *****************************/
/*****************************************************************************/

void TstVis_ShowVisibilityIcons (unsigned SelectedVisibility,bool Hidden)
  {
   extern const char *Txt_TST_STR_VISIBILITY[TstVis_NUM_ITEMS_VISIBILITY];
   extern const char *Txt_TST_HIDDEN_VISIBLE[2];
   static const char *Icons[TstVis_NUM_ITEMS_VISIBILITY][2] =
     {
      [TstVis_VISIBLE_QST_ANS_TXT   ][false] = "file-alt-red.svg",
      [TstVis_VISIBLE_QST_ANS_TXT   ][true ] = "file-alt-green.svg",

      [TstVis_VISIBLE_FEEDBACK_TXT  ][false] = "file-signature-red.svg",
      [TstVis_VISIBLE_FEEDBACK_TXT  ][true ] = "file-signature-green.svg",

      [TstVis_VISIBLE_CORRECT_ANSWER][false] = "spell-check-red.svg",
      [TstVis_VISIBLE_CORRECT_ANSWER][true ] = "spell-check-green.svg",

      [TstVis_VISIBLE_EACH_QST_SCORE][false] = "tasks-red.svg",
      [TstVis_VISIBLE_EACH_QST_SCORE][true ] = "tasks-green.svg",

      [TstVis_VISIBLE_TOTAL_SCORE   ][false] = "check-circle-regular-red.svg",
      [TstVis_VISIBLE_TOTAL_SCORE   ][true ] = "check-circle-regular-green.svg",
     };
   TstVis_Visibility_t Visibility;
   bool ItemVisible;
   char *Title;

   for (Visibility  = (TstVis_Visibility_t) 0;
	Visibility <= (TstVis_Visibility_t) (TstVis_NUM_ITEMS_VISIBILITY - 1);
	Visibility++)
     {
      ItemVisible = (SelectedVisibility & (1 << Visibility)) != 0;
      if (asprintf (&Title,"%s: %s",
		    Txt_TST_STR_VISIBILITY[Visibility],
		    Txt_TST_HIDDEN_VISIBLE[ItemVisible]) < 0)
	 Lay_NotEnoughMemoryExit ();
      if (ItemVisible && !Hidden)
	 Ico_PutIconOn  (Icons[Visibility][ItemVisible],Title);
      else
	 Ico_PutIconOff (Icons[Visibility][ItemVisible],Title);
      free (Title);
     }
  }

/*****************************************************************************/
/************* Put checkboxes in form to select exam visibility **************/
/*****************************************************************************/

void TstVis_PutVisibilityCheckboxes (unsigned SelectedVisibility)
  {
   extern const char *Txt_TST_STR_VISIBILITY[TstVis_NUM_ITEMS_VISIBILITY];
   static const char *Icons[TstVis_NUM_ITEMS_VISIBILITY] =
     {
      [TstVis_VISIBLE_QST_ANS_TXT   ] = "file-alt.svg",
      [TstVis_VISIBLE_FEEDBACK_TXT  ] = "file-signature.svg",
      [TstVis_VISIBLE_CORRECT_ANSWER] = "spell-check.svg",
      [TstVis_VISIBLE_EACH_QST_SCORE] = "tasks.svg",
      [TstVis_VISIBLE_TOTAL_SCORE   ] = "check-circle-regular.svg",
     };
   TstVis_Visibility_t Visibility;
   bool ItemVisible;

   for (Visibility  = (TstVis_Visibility_t) 0;
	Visibility <= (TstVis_Visibility_t) (TstVis_NUM_ITEMS_VISIBILITY - 1);
	Visibility++)
     {
      ItemVisible = (SelectedVisibility & (1 << Visibility)) != 0;
      HTM_LABEL_Begin ("class=\"DAT\"");
      HTM_INPUT_CHECKBOX ("Visibility",HTM_DONT_SUBMIT_ON_CHANGE,
		          "value=\"%u\"%s",
		          (unsigned) Visibility,
		          ItemVisible ? " checked=\"checked\"" :
		        	        "");
      Ico_PutIconOn (Icons[Visibility],Txt_TST_STR_VISIBILITY[Visibility]);
      HTM_Txt (Txt_TST_STR_VISIBILITY[Visibility]);
      HTM_LABEL_End ();
      HTM_BR ();
     }
  }

/*****************************************************************************/
/************************** Get visibility from form *************************/
/*****************************************************************************/

unsigned TstVis_GetVisibilityFromForm (void)
  {
   size_t MaxSizeListVisibilitySelected;
   char *StrVisibilitySelected;
   const char *Ptr;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned UnsignedNum;
   TstVis_Visibility_t VisibilityItem;
   unsigned Visibility = 0;	// Nothing selected

   /***** Allocate memory for list of attendance events selected *****/
   MaxSizeListVisibilitySelected = TstVis_NUM_ITEMS_VISIBILITY * (Cns_MAX_DECIMAL_DIGITS_UINT + 1);
   if ((StrVisibilitySelected = (char *) malloc (MaxSizeListVisibilitySelected + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Get parameter multiple with list of visibility items selected *****/
   Par_GetParMultiToText ("Visibility",StrVisibilitySelected,MaxSizeListVisibilitySelected);

   /***** Set which attendance events will be shown as selected (checkboxes on) *****/
   if (StrVisibilitySelected[0])	// There are events selected
      for (Ptr = StrVisibilitySelected;
	   *Ptr;
	  )
	{
	 /* Get next visibility item selected */
	 Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
         if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
            if (UnsignedNum < TstVis_NUM_ITEMS_VISIBILITY)
              {
               VisibilityItem = (TstVis_Visibility_t) UnsignedNum;
               Visibility |= (1 << VisibilityItem);
              }
	}

   return Visibility;
  }

/*****************************************************************************/
/************************** Get visibility from string *************************/
/*****************************************************************************/

unsigned TstVis_GetVisibilityFromStr (const char *Str)
  {
   unsigned UnsignedNum;
   unsigned Visibility = TstVis_MIN_VISIBILITY;	// In nothing is read, return minimum visibility

   /***** Get visibility from string *****/
   if (Str)
      if (Str[0])
         if (sscanf (Str,"%u",&UnsignedNum) == 1)
            Visibility = UnsignedNum & TstVis_MAX_VISIBILITY;

   return Visibility;
  }

/*****************************************************************************/
/***************************** Get visibility items **************************/
/*****************************************************************************/

bool TstVis_IsVisibleQstAndAnsTxt (unsigned Visibility)
  {
   return (Visibility & (1 << TstVis_VISIBLE_QST_ANS_TXT)) != 0;
  }

bool TstVis_IsVisibleFeedbackTxt (unsigned Visibility)
  {
   return (Visibility & (1 << TstVis_VISIBLE_FEEDBACK_TXT)) != 0;
  }

bool TstVis_IsVisibleCorrectAns (unsigned Visibility)
  {
   return (Visibility & (1 << TstVis_VISIBLE_CORRECT_ANSWER)) != 0;
  }

bool TstVis_IsVisibleEachQstScore (unsigned Visibility)
  {
   return (Visibility & (1 << TstVis_VISIBLE_EACH_QST_SCORE)) != 0;
  }

bool TstVis_IsVisibleTotalScore (unsigned Visibility)
  {
   return (Visibility & (1 << TstVis_VISIBLE_TOTAL_SCORE)) != 0;
  }
