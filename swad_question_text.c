// swad_question_text.c: test/exam/game text questions

/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <string.h>		// For strcoll

#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_exam_sheet.h"
#include "swad_question_text.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/**************** Write text answer in an exam answer sheet ******************/
/*****************************************************************************/

void QstTxt_WriteBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT %s_%s\"",
		 Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
      HTM_NBSP ();
   HTM_TD_End ();
  }

void QstTxt_WriteCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
			  __attribute__((unused)) unsigned QstInd,
			  struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT %s_%s\"",
		 Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
      for (NumOpt = 0;
	   NumOpt < Qst->Answer.NumOptions;
	   NumOpt++)
	{
	 if (NumOpt)
	   {
	    HTM_Semicolon (); HTM_NBSP ();
	   }
	 HTM_Txt (Qst->Answer.Options[NumOpt].Text);
	}
   HTM_TD_End ();
  }

void QstTxt_WriteReadAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   Qst_WrongOrCorrect_t WrongOrCorrect;

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Write online answer *****/
   if (Print->PrintedQsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_Copy (TextAnsUsr,Print->PrintedQsts[QstInd].Answer.Str,
		sizeof (TextAnsUsr) - 1);
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);
      Str_ConvertToComparable (TextAnsUsr);

      for (NumOpt = 0, WrongOrCorrect = Qst_WRONG;
	   NumOpt < Qst->Answer.NumOptions;
	   NumOpt++)
	{
	 /* Filter this correct answer */
	 Str_Copy (TextAnsOK,Qst->Answer.Options[NumOpt].Text,
		   sizeof (TextAnsOK) - 1);
	 Str_ConvertToComparable (TextAnsOK);

	 /* Check is user answer is correct */
	 if (!strcoll (TextAnsUsr,TextAnsOK))
	   {
	    WrongOrCorrect = Qst_CORRECT;
	    break;
	   }
	}

      HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT %s_%s\"",
		    Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix ());
	 HTM_Txt (Print->PrintedQsts[QstInd].Answer.Str);
      HTM_TD_End ();
     }
   else						// If student has omitted the answer
      HTM_TD_Empty (1);
  }

void QstTxt_WriteEditAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_BLANK;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if student answer is blank, wrong or correct *****/
   if (Print->PrintedQsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_Copy (TextAnsUsr,Print->PrintedQsts[QstInd].Answer.Str,
	        sizeof (TextAnsUsr) - 1);
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);
      Str_ConvertToComparable (TextAnsUsr);

      /* Change format of answers text */
      Qst_ChangeFormatOptionsText (Qst);

      for (NumOpt = 0, WrongOrCorrect = Qst_WRONG;
	   NumOpt < Qst->Answer.NumOptions;
	   NumOpt++)
	{
	 /* Filter this correct answer */
	 Str_Copy (TextAnsOK,Qst->Answer.Options[NumOpt].Text,
		   sizeof (TextAnsOK) - 1);
	 Str_ConvertToComparable (TextAnsOK);

	 /* Check is user answer is correct */
	 if (!strcoll (TextAnsUsr,TextAnsOK))
	   {
	    WrongOrCorrect = Qst_CORRECT;
	    break;
	   }
	}
     }

   /***** Write input field for the answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT\"");

      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\" maxlength=\"%u\""
		" class=\"Exa_ANSWER_INPUT_TEXT %s_%s\" value=\"%s\"",
		Id,Qst_MAX_CHARS_ANSWERS_ONE_QST,
		Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix (),
		Print->PrintedQsts[QstInd].Answer.Str);
      ExaShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }
