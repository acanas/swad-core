// swad_question_int.c: test/exam/game integer questions

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

#include <stdio.h>		// For sscanf

#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_exam_sheet.h"
#include "swad_parameter.h"
#include "swad_question_database.h"
#include "swad_question_int.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void QstInt_GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst);
static void QstInt_ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst);

static long QstInt_GetAnsFromStr (char *Str);

/*****************************************************************************/
/********************* Put input field for integer answer ********************/
/*****************************************************************************/

void QstInt_PutInputField (const struct Qst_Question *Qst)
  {
   extern const char *Txt_Integer_number;

   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_Txt (Txt_Integer_number); HTM_Colon (); HTM_NBSP ();
      HTM_INPUT_LONG ("AnsInt",(long) INT_MIN,(long) INT_MAX,Qst->Answer.Integer,
	              (Qst->Answer.Type == Qst_ANS_INT ? HTM_NO_ATTR :
							 HTM_DISABLED) |
		      HTM_REQUIRED,
		      "class=\"Exa_ANSWER_INPUT_INT INPUT_%s\"",
		      The_GetSuffix ());
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/**************************** Get answer from form ***************************/
/*****************************************************************************/

void QstInt_GetAnsFromForm (struct Qst_Question *Qst)
  {
   if (Qst_AllocateTextChoiceAnswer (Qst,0) == Err_ERROR)
      /* Abort on error */
      Ale_ShowAlertsAndExit ();

   Par_GetParText ("AnsInt",Qst->Answer.Options[0].Text,
		   Cns_MAX_DIGITS_LONG);
  }

/*****************************************************************************/
/******************* Check if question options are correct *******************/
/*****************************************************************************/

Err_SuccessOrError_t QstInt_CheckIfOptsAreCorrect (struct Qst_Question *Qst)
  {
   extern const char *Txt_You_must_enter_an_integer_value_as_the_correct_answer;

   /* First option should be filled */
   if (!Qst->Answer.Options[0].Text)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_an_integer_value_as_the_correct_answer);
      return Err_ERROR;
     }
   if (!Qst->Answer.Options[0].Text[0])
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_an_integer_value_as_the_correct_answer);
      return Err_ERROR;
     }

   Qst->Answer.Integer = QstInt_GetAnsFromStr (Qst->Answer.Options[0].Text);
   Qst->Answer.NumOpts = 1;

   return Err_SUCCESS;	// Question format without errors
  }

/*****************************************************************************/
/*********** Check if identical answer already exists in database ************/
/*****************************************************************************/

Exi_Exist_t QstInt_IdenticalAnswersExist (MYSQL_RES *mysql_res,
					  __attribute__((unused)) unsigned NumOptsQstInDB,
					  const struct Qst_Question *Qst)
  {
   MYSQL_ROW row;
   Exi_Exist_t IdenticalAnswersExist;

   row = mysql_fetch_row (mysql_res);
   IdenticalAnswersExist = QstInt_GetAnsFromStr (row[0]) ==
			   Qst->Answer.Integer ? Exi_EXISTS :
						 Exi_DOES_NOT_EXIST;

   return IdenticalAnswersExist;
  }

/*****************************************************************************/
/*********************** Get question options from row ***********************/
/*****************************************************************************/

void QstInt_GetQstOptionsFromRow (MYSQL_ROW row,struct Qst_Question *Qst,
				  __attribute__((unused)) unsigned NumOpt)
  {
   Qst_CheckIfNumberOfAnswersIsOne (Qst);
   Qst->Answer.Integer = QstInt_GetAnsFromStr (row[1]);
  }

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

void QstInt_GetCorrectAndComputeAnsScore (const char *Table,
					  struct Qst_PrintedQuestion *PrintedQst,
					  struct Qst_Question *Qst)
  {
   /***** Get the numerical value of the correct answer, and compute score *****/
   QstInt_GetCorrectAnswerFromDB (Table,Qst);
   QstInt_ComputeAnsScore (PrintedQst,Qst);
  }

static void QstInt_GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Query database *****/
   Qst->Answer.NumOpts = Qst_DB_GetTextOfAnswers (&mysql_res,Table,Qst->QstCod);

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Get correct answer *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%ld",&Qst->Answer.Integer) != 1)
      Err_WrongAnswerExit ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void QstInt_ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst)
  {
   long AnsUsr;

   PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_BLANK;
   PrintedQst->Answer.Score = 0.0;	// Default score for blank or wrong answer

   if (PrintedQst->Answer.Str[0])	// If user has answered the answer
     {
      PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_WRONG_ZERO;
      if (sscanf (PrintedQst->Answer.Str,"%ld",&AnsUsr) == 1)
	 if (AnsUsr == Qst->Answer.Integer)	// Correct answer
	   {
	    PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_CORRECT;
	    PrintedQst->Answer.Score = 1.0;
	   }
     }
  }

/*****************************************************************************/
/**************************** Write integer answer ***************************/
/*****************************************************************************/

void QstInt_WriteCorrAns (struct Qst_Question *Qst,
			  const char *ClassTxt,
			  __attribute__((unused)) const char *ClassFeedback)
  {
   HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
      HTM_OpenParenthesis ();
         HTM_Long (Qst->Answer.Integer);
      HTM_CloseParenthesis ();
   HTM_SPAN_End ();
  }

/*****************************************************************************/
/******************* Write integer answer in a test print ********************/
/*****************************************************************************/

void QstInt_WriteTstFillAns (const struct Qst_PrintedQuestion *PrintedQst,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   char StrAns[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"number\" name=\"%s\""
	     " class=\"Exa_ANSWER_INPUT_FLOAT INPUT_%s\""
	     " value=\"%s\"",
	     StrAns,The_GetSuffix (),PrintedQst->Answer.Str);
   HTM_ElementEnd ();
  }

void QstInt_WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     __attribute__((unused)) const char *ClassTxt,
			     __attribute__((unused)) const char *ClassFeedback)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   Qst_WrongOrCorrect_t WrongOrCorrect;
   long AnsUsr;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   HTM_TR_Begin (NULL);

      /***** Write the user answer *****/
      if (PrintedQst->Answer.Str[0])		// If user has answered the question
	{
	 WrongOrCorrect = Qst_BLANK;
	 if (sscanf (PrintedQst->Answer.Str,"%ld",&AnsUsr) == 1)
	    if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER] == Usr_CAN)
	       WrongOrCorrect = AnsUsr == Qst->Answer.Integer ? Qst_CORRECT :
								Qst_WRONG;
	 HTM_TD_Begin ("class=\"CM %s_%s\"",
		       Qst_AnswerDisplay[WrongOrCorrect].ClassStd,
		       The_GetSuffix ());
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
      else							// If user has omitted the answer
	 HTM_TD_Empty (1);

      /***** Write the correct answer *****/
      HTM_TD_Begin ("class=\"CM %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
	 switch (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	   {
	    case Usr_CAN:
	       HTM_Long (Qst->Answer.Integer);
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
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

void QstInt_WriteExaFillAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
	     " class=\"Exa_ANSWER_INPUT_INT\" value=\"%s\"",
	     Id,Print->PrintedQsts[QstInd].Answer.Str);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_ElementEnd ();
  }

void QstInt_WriteExaBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   HTM_TD_Begin ("class=\"Exa_ANSWER_INT %s_%s\"",
	         Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
      HTM_NBSP ();
   HTM_TD_End ();
  }

void QstInt_WriteExaCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
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

void QstInt_WriteExaReadAns (const struct ExaPrn_Print *Print,
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

void QstInt_WriteExaEditAns (const struct ExaPrn_Print *Print,
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

/*****************************************************************************/
/******************** Get a integer number from a string *********************/
/*****************************************************************************/

static long QstInt_GetAnsFromStr (char *Str)
  {
   long LongNum;

   if (Str == NULL)
      return 0.0;

   /***** The string is "scanned" as long *****/
   if (sscanf (Str,"%ld",&LongNum) != 1)	// If the string does not hold a valid integer number...
     {
      LongNum = 0L;	// ...the number is reset to 0
      Str[0] = '\0';	// ...and the string is reset to ""
     }

   return LongNum;
  }
