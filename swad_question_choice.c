// swad_question_choice.c: test/exam/game choice questions

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

#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_exam_sheet.h"
#include "swad_question_choice.h"

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
/******* Write unique / multiple choice answer in an exam answer sheet *******/
/*****************************************************************************/

void QstCho_WriteBlnkAns (const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;

   /***** Write blank answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      /* Write option letter */
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
	 HTM_Option (NumOpt);
      HTM_TD_End ();
     }
  }

void QstCho_WriteCorrAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   unsigned NumOpt;

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Write correct answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      /* Write option letter */
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		    Qst_AnswerDisplay[Qst->Answer.Options[Indexes[NumOpt]].Correct].ClassTch,
		    The_GetSuffix ());
	 HTM_Option (NumOpt);
      HTM_TD_End ();
     }
  }

void QstCho_WriteReadAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;
   Qst_WrongOrCorrect_t WrongOrCorrect;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   Qst_GetAnswersFromStr (Print->PrintedQsts[QstInd].Answer.Str,UsrAnswers);

   /***** Write online answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      WrongOrCorrect = Qst->Answer.Options[Indexes[NumOpt]].Correct;

      /* Write letter depending on user's answer */
      if (UsrAnswers[Indexes[NumOpt]] == HTM_CHECKED)	// This answer has been selected by the user
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		       Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix ());
	    HTM_Txt (Qst_AnswerDisplay[WrongOrCorrect].Symbol);
	}
      else	// This answer has NOT been selected by the user
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		       Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
	    HTM_NBSP ();
	}
      HTM_TD_End ();
     }
  }

void QstCho_WriteEditAns (const struct ExaPrn_Print *Print,
			  unsigned QstInd,struct Qst_Question *Qst)
  {
   static const char *InputType[Qst_NUM_ANS_TYPES] =
     {
      [Qst_ANS_UNIQUE_CHOICE  ] = "radio",
      [Qst_ANS_MULTIPLE_CHOICE] = "checkbox",
     };
   unsigned NumOpt;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   Qst_GetAnswersFromStr (Print->PrintedQsts[QstInd].Answer.Str,UsrAnswers);

   /***** Write input field for the answer *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE\"");

         snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
	 HTM_TxtF ("<input type=\"%s\" id=\"%s_%u\" name=\"Ans\" value=\"%u\"",
		   InputType[Qst->Answer.Type],Id,NumOpt,Indexes[NumOpt]);
	 if ((UsrAnswers[Indexes[NumOpt]] & HTM_CHECKED))
	    HTM_Txt (" checked");
	 ExaShe_WriteJSToUpdateSheet (Print,QstInd,Id,(int) NumOpt);
	 HTM_ElementEnd ();

      HTM_TD_End ();
     }
  }
