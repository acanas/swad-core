// swad_question_int.c: test/exam/game integer questions

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

#include <stdio.h>		// For sscanf

#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_exam_sheet.h"
#include "swad_question_int.h"

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
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

void QstInt_WriteBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   HTM_TD_Begin ("class=\"Exa_ANSWER_INT %s_%s\"",
	         Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
      HTM_NBSP ();
   HTM_TD_End ();
  }

void QstInt_WriteCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
			  __attribute__((unused)) unsigned QstInd,
			  struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_INT %s_%s\"",
		 Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
      HTM_Long (Qst->Answer.Integer);
   HTM_TD_End ();
  }

void QstInt_WriteReadAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   Qst_WrongOrCorrect_t WrongOrCorrect;
   long AnsUsr;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Write online answer *****/
   if (Print->PrintedQsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      WrongOrCorrect = Qst_BLANK;
      if (sscanf (Print->PrintedQsts[QstInd].Answer.Str,"%ld",&AnsUsr) == 1)
	 WrongOrCorrect = AnsUsr == Qst->Answer.Integer ? Qst_CORRECT :
							  Qst_WRONG;

      HTM_TD_Begin ("class=\"Exa_ANSWER_INT %s_%s\"",
	            Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix ());
	 switch (WrongOrCorrect)
	   {
	    case Qst_WRONG:
	    case Qst_CORRECT:
	       HTM_Long (AnsUsr);
	       break;
	    case Qst_BLANK:
	    default:
	       HTM_Question ();
	       break;
	   }
      HTM_TD_End ();
     }
   else							// If student has omitted the answer
      HTM_TD_Empty (1);
  }

void QstInt_WriteEditAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   Qst_WrongOrCorrect_t WrongOrCorrect;
   long AnsUsr;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Check if student answer is blank, wrong or correct *****/
   WrongOrCorrect = Qst_BLANK;
   if (Print->PrintedQsts[QstInd].Answer.Str[0])	// If the question has been answered
      if (sscanf (Print->PrintedQsts[QstInd].Answer.Str,"%ld",&AnsUsr) == 1)
	 WrongOrCorrect = AnsUsr == Qst->Answer.Integer ? Qst_CORRECT :
							  Qst_WRONG;

   /***** Write input field for the answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_INT\"");

      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
		" class=\"Exa_ANSWER_INPUT_INT %s_%s\" value=\"",
		Id,Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix ());
      if (WrongOrCorrect != Qst_BLANK)
	 HTM_Long (AnsUsr);
      HTM_Char ('"');
      ExaShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }
