// swad_question_float.c: test/exam/game float questions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
#include "swad_question_float.h"

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
/******************** Write float answer in an test print ********************/
/*****************************************************************************/

void QstFlt_WriteTstFillAns (const struct Qst_PrintedQuestion *PrintedQst,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   char StrAns[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"number\" name=\"%s\""
	     " class=\"Exa_ANSWER_INPUT_FLOAT INPUT_%s\""
	     " value=\"%s\" step=\"any\"",
	     StrAns,The_GetSuffix (),PrintedQst->Answer.Str);
   HTM_ElementEnd ();
  }

void QstFlt_WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     __attribute__((unused)) const char *ClassTxt,
			     __attribute__((unused)) const char *ClassFeedback)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   double AnsUsr;
   Err_SuccessOrError_t SuccessOrError;

   /***** Check if number of rows is correct *****/
   if (Qst->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   HTM_TR_Begin (NULL);

      /***** Write the user answer *****/
      if (PrintedQst->Answer.Str[0])	// If user has answered the question
	{
	 SuccessOrError = Str_GetDoubleFromStr (PrintedQst->Answer.Str,&AnsUsr);

	 // A bad formatted floating point answer will interpreted as 0.0
	 HTM_TD_Begin ("class=\"CM %s_%s\"",
		       ICanView[TstVis_VISIBLE_CORRECT_ANSWER] == Usr_CAN &&
		       SuccessOrError == Err_SUCCESS ?
			  (AnsUsr >= Qst->Answer.FloatingPoint[0] &&
			   AnsUsr <= Qst->Answer.FloatingPoint[1] ? "Qst_ANS_OK" :	// Correct
								    "Qst_ANS_BAD") :	// Wrong
								    "Qst_ANS_0",	// Blank answer
		       The_GetSuffix ());
	    if (SuccessOrError == Err_SUCCESS)
	       HTM_Double (AnsUsr);
	 HTM_TD_End ();
	}
      else					// If user has omitted the answer
	 HTM_TD_Empty (1);

      /***** Write the correct answer *****/
      HTM_TD_Begin ("class=\"CM %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
	 switch (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	   {
	    case Usr_CAN:
	       HTM_DoubleRange (Qst->Answer.FloatingPoint[0],
				Qst->Answer.FloatingPoint[1]);
	       break;
	    case Usr_CAN_NOT:
	    default:
	       Ico_PutIconNotVisible ();
	       break;
	   }
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Write float answer in an exam answer sheet *****************/
/*****************************************************************************/

void QstFlt_WriteExaBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT %s_%s\"",
	         Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
      HTM_NBSP ();
   HTM_TD_End ();
  }

void QstFlt_WriteExaCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
			     __attribute__((unused)) unsigned QstInd,
			     struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   /***** Check if number of rows is correct *****/
   if (Qst->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT %s_%s\"",
		 Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
      HTM_DoubleRange (Qst->Answer.FloatingPoint[0],
		       Qst->Answer.FloatingPoint[1]);
   HTM_TD_End ();
  }

void QstFlt_WriteExaReadAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   double AnsUsr = 0.0;
   Err_SuccessOrError_t SuccessOrError;

   /***** Check if number of rows is correct *****/
   if (Qst->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write online answer *****/
   if (Print->PrintedQsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      SuccessOrError = Str_GetDoubleFromStr (Print->PrintedQsts[QstInd].Answer.Str,
					     &AnsUsr);
      // A bad formatted floating point answer will interpreted as 0.0
      HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT %s_%s\"",
		    SuccessOrError == Err_SUCCESS ? (AnsUsr >= Qst->Answer.FloatingPoint[0] &&
						     AnsUsr <= Qst->Answer.FloatingPoint[1] ? "Qst_ANS_OK" :	// Correct
											      "Qst_ANS_BAD") :	// Wrong
						     "Qst_ANS_0",							// Blank answer
		    The_GetSuffix ());
         if (SuccessOrError == Err_SUCCESS)
	    HTM_Double (AnsUsr);
      HTM_TD_End ();
     }
   else						// If student has omitted the answer
      HTM_TD_Empty (1);
  }

void QstFlt_WriteExaEditAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   double AnsUsr;
   Err_SuccessOrError_t SuccessOrError;
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_BLANK;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   if (Qst->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Check if student answer is blank, wrong or correct *****/
   if (Print->PrintedQsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      SuccessOrError = Str_GetDoubleFromStr (Print->PrintedQsts[QstInd].Answer.Str,&AnsUsr);
      // A bad formatted floating point answer will interpreted as 0.0
      WrongOrCorrect = SuccessOrError == Err_SUCCESS ? (AnsUsr >= Qst->Answer.FloatingPoint[0] &&
							AnsUsr <= Qst->Answer.FloatingPoint[1] ? Qst_CORRECT :
												 Qst_WRONG) :
							Qst_BLANK;
     }

   /***** Write input field for the answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT\"");

      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
		" class=\"Exa_ANSWER_INPUT_FLOAT %s_%s\" value=\"%s\"",
		Id,Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix (),
		Print->PrintedQsts[QstInd].Answer.Str);
      ExaShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }
