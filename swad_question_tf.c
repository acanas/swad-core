// swad_question_tf.c: test/exam/game true/false questions

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
#include "swad_database.h"
#include "swad_exam_sheet.h"
#include "swad_question_database.h"
#include "swad_question_tf.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void QstTF__GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst);
static void QstTF__ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst);

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

void QstTF__GetCorrectAndComputeAnsScore (const char *Table,
					  struct Qst_PrintedQuestion *PrintedQst,
				          struct Qst_Question *Qst)
  {
   /***** Get answer true or false, and compute score *****/
   QstTF__GetCorrectAnswerFromDB (Table,Qst);
   QstTF__ComputeAnsScore (PrintedQst,Qst);
  }

static void QstTF__GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Qst->Answer.NumOptions = Qst_DB_GetTextOfAnswers (&mysql_res,
						     Table,Qst->QstCod);

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Get answer *****/
   row = mysql_fetch_row (mysql_res);
   Qst->Answer.OptionTF = Qst_GetOptionTFFromChar (row[0][0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void QstTF__ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst)
  {
   static double Scores[] =
     {
      [TstPrn_ANSWER_IS_CORRECT       ] =  1.0,
      [TstPrn_ANSWER_IS_WRONG_NEGATIVE] = -1.0,
      [TstPrn_ANSWER_IS_WRONG_ZERO    ] =  0.0,	// Not used
      [TstPrn_ANSWER_IS_WRONG_POSITIVE] =  0.0,	// Not used
      [TstPrn_ANSWER_IS_BLANK         ] =  0.0,
     };

   PrintedQst->Answer.IsCorrect = PrintedQst->Answer.Str[0] ?	// If user has selected T or F
				  (Qst_GetOptionTFFromChar (PrintedQst->Answer.Str[0]) == Qst->Answer.OptionTF ?
				     TstPrn_ANSWER_IS_CORRECT :
				     TstPrn_ANSWER_IS_WRONG_NEGATIVE) :
				  TstPrn_ANSWER_IS_BLANK;
   PrintedQst->Answer.Score = Scores[PrintedQst->Answer.IsCorrect];
  }

/*****************************************************************************/
/************************* Write false / true answer *************************/
/*****************************************************************************/

void QstTF__WriteCorrAns (struct Qst_Question *Qst,
			  const char *ClassTxt,
			  __attribute__((unused)) const char *ClassFeedback)
  {
   HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
      HTM_OpenParenthesis ();
	 Qst_WriteAnsTF (Qst->Answer.OptionTF);
      HTM_CloseParenthesis ();
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/***************** Write false / true answer in a test print *****************/
/*****************************************************************************/

void QstTF__WriteTstFillAns (const struct Qst_PrintedQuestion *PrintedQst,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   /***** Write selector for the answer *****/
   /* Initially user has not answered the question ==> initially all answers will be blank.
      If the user does not confirm the submission of their exam ==>
      ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
   HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
		     "name=\"Ans%010u\" class=\"INPUT_%s\"",
		     QstInd,The_GetSuffix ());
      Qst_WriteTFOptionsToFill (Qst_GetOptionTFFromChar (PrintedQst->Answer.Str[0]));
   HTM_SELECT_End ();
  }

void QstTF__WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     __attribute__((unused)) const char *ClassTxt,
			     __attribute__((unused)) const char *ClassFeedback)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   Qst_OptionTF_t OptTFStd;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Get answer true or false *****/
   OptTFStd = Qst_GetOptionTFFromChar (PrintedQst->Answer.Str[0]);

   HTM_TR_Begin (NULL);

      /***** Write the user answer *****/
      HTM_TD_Begin ("class=\"CM %s_%s\"",
		    ICanView[TstVis_VISIBLE_CORRECT_ANSWER] == Usr_CAN ?
		       (OptTFStd == Qst->Answer.OptionTF ? "Qst_ANS_OK" :		// Correct
							   "Qst_ANS_BAD") :	// Wrong
							   "Qst_ANS_0",		// Blank answer
		    The_GetSuffix ());
	 Qst_WriteAnsTF (OptTFStd);
      HTM_TD_End ();

      /***** Write the correct answer *****/
      HTM_TD_Begin ("class=\"CM %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
	 switch (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	   {
	    case Usr_CAN:
	       Qst_WriteAnsTF (Qst->Answer.OptionTF);
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
/*********** Write false / true answer in an exam question sheet *************/
/*****************************************************************************/

void QstTF__WriteExaBlnkQstOptions (__attribute__((unused)) const struct ExaPrn_Print *Print,
				    __attribute__((unused)) unsigned QstInd,
				    __attribute__((unused)) struct Qst_Question *Qst)
  {
   Qst_WriteAnsTF (Qst_OPTION_TRUE);
   HTM_Slash ();
   Qst_WriteAnsTF (Qst_OPTION_FALSE);
  }

/*****************************************************************************/
/************* Write true / false answer in an exam answer sheet *************/
/*****************************************************************************/

void QstTF__WriteExaFillAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write selector for the answer *****/
   /* Initially user has not answered the question ==> initially all answers will be blank.
      If the user does not confirm the submission of their exam ==>
      ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<select id=\"%s\" name=\"Ans\"",Id);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_ElementEnd ();
      Qst_WriteTFOptionsToFill (Qst_GetOptionTFFromChar (Print->PrintedQsts[QstInd].Answer.Str[0]));
   HTM_Txt ("</select>");
  }

void QstTF__WriteExaBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   Qst_OptionTF_t OptTF;

   for (OptTF  = Qst_OPTION_TRUE;
	OptTF <= Qst_OPTION_FALSE;
	OptTF++)
     {
      HTM_TD_Begin ("class=\"Exa_ANSWER_TF %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
	 Qst_WriteAnsTF (OptTF);
      HTM_TD_End ();
     }
  }

void QstTF__WriteExaCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
			     __attribute__((unused)) unsigned QstInd,
			     struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF %s_%s\"",
		 Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
      Qst_WriteAnsTF (Qst->Answer.OptionTF);
   HTM_TD_End ();
  }

void QstTF__WriteExaReadAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   Qst_OptionTF_t OptTFStd;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Get answer true or false *****/
   OptTFStd = Qst_GetOptionTFFromChar (Print->PrintedQsts[QstInd].Answer.Str[0]);

   /***** Write online answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF %s_%s\"",
		 OptTFStd == Qst->Answer.OptionTF ? "Qst_ANS_OK" :	// Correct
						    "Qst_ANS_BAD",	// Blank answer
		 The_GetSuffix ());
      Qst_WriteAnsTF (OptTFStd);
   HTM_TD_End ();
  }

void QstTF__WriteExaEditAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   Qst_OptionTF_t OptTFStd = Qst_GetOptionTFFromChar (Print->PrintedQsts[QstInd].Answer.Str[0]);
   Qst_WrongOrCorrect_t WrongOrCorrect;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Check if student answer is blank, wrong or correct *****/
   switch (OptTFStd)
     {
      case Qst_OPTION_TRUE:
      case Qst_OPTION_FALSE:
	 WrongOrCorrect = OptTFStd == Qst->Answer.OptionTF ? Qst_CORRECT :
							     Qst_WRONG;
	 break;
      case Qst_OPTION_EMPTY:
      default:
	 WrongOrCorrect = Qst_BLANK;
	 break;
     }

   /***** Write selector for the answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF\"");

      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<select id=\"%s\" name=\"Ans\" class=\"%s_%s\"",
		Id,Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix ());
      ExaShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();
         Qst_WriteTFOptionsToFill (OptTFStd);
      HTM_Txt ("</select>");

   HTM_TD_End ();
  }
