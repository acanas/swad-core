// swad_question_text.c: test/exam/game text questions

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

#include <string.h>		// For strcoll

#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_exam_sheet.h"
#include "swad_parameter.h"
#include "swad_question_database.h"
#include "swad_question_text.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void QstTxt_GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst);
static void QstTxt_ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst);

/*****************************************************************************/
/**************************** Get answer from form ***************************/
/*****************************************************************************/

void QstTxt_GetAnsFromForm (struct Qst_Question *Qst)
  {
   unsigned NumOpt;
   char AnsStr[6 + Cns_MAX_DIGITS_UINT + 1];
   char FbStr[5 + Cns_MAX_DIGITS_UINT + 1];

   /* Get the texts of the answers */
   for (NumOpt = 0;
        NumOpt < Qst_MAX_OPTS_PER_QST;
        NumOpt++)
     {
      if (Qst_AllocateTextChoiceAnswer (Qst,NumOpt) == Err_ERROR)
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();

      /* Get answer */
      snprintf (AnsStr,sizeof (AnsStr),"AnsStr%u",NumOpt);
      Par_GetParHTML (AnsStr,Qst->Answer.Options[NumOpt].Text,
		      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
      if (Qst->Answer.Type == Qst_ANS_TEXT)
	 /* In order to compare student answer to stored answer,
	    the text answers are stored avoiding two or more consecurive spaces */
         Str_ReplaceSeveralSpacesForOne (Qst->Answer.Options[NumOpt].Text);

      /* Get feedback */
      snprintf (FbStr,sizeof (FbStr),"FbStr%u",NumOpt);
      Par_GetParHTML (FbStr,Qst->Answer.Options[NumOpt].Feedback,
		      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);

      /* Set all options to be correct */
      if (Qst->Answer.Options[NumOpt].Text[0])
	 Qst->Answer.Options[NumOpt].Correct = Qst_CORRECT;	// All the answers are correct
     }
  }

/*****************************************************************************/
/******************* Check if question options are correct *******************/
/*****************************************************************************/

Err_SuccessOrError_t QstTxt_CheckIfOptsAreCorrect (struct Qst_Question *Qst)
  {
   extern const char *Txt_You_must_type_at_least_the_first_answer;
   extern const char *Txt_You_can_not_leave_empty_intermediate_answers;
   unsigned NumOpt;
   bool ThereIsEndOfAnswers;

   /***** First option should be filled *****/
   if (!Qst->Answer.Options[0].Text)	// If the first answer is empty
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_answer);
      return Err_ERROR;
     }
   if (!Qst->Answer.Options[0].Text[0])	// If the first answer is empty
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_answer);
      return Err_ERROR;
     }

   /***** No option should be empty before a non-empty option *****/
   for (NumOpt = 0, ThereIsEndOfAnswers = false;
	NumOpt < Qst_MAX_OPTS_PER_QST;
	NumOpt++)
      if (Qst->Answer.Options[NumOpt].Text)
	{
	 if (Qst->Answer.Options[NumOpt].Text[0])
	   {
	    if (ThereIsEndOfAnswers)
	      {
	       Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
	       return Err_ERROR;
	      }
	    Qst->Answer.NumOpts++;
	   }
	 else
	    ThereIsEndOfAnswers = true;
	}
      else
	 ThereIsEndOfAnswers = true;

   return Err_SUCCESS;	// Question format without errors
  }

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

void QstTxt_GetCorrectAndComputeAnsScore (const char *Table,
					  struct Qst_PrintedQuestion *PrintedQst,
				          struct Qst_Question *Qst)
  {
   /***** Get correct text answers for this question from database,
          and compute score *****/
   QstTxt_GetCorrectAnswerFromDB (Table,Qst);
   QstTxt_ComputeAnsScore (PrintedQst,Qst);
  }

static void QstTxt_GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;

   /***** Query database *****/
   Qst->Answer.NumOpts = Qst_DB_GetTextOfAnswers (&mysql_res,Table,Qst->QstCod);

   /***** Get text and correctness of answers for this question
          from database (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOpts;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Allocate memory for text in this choice answer *****/
      if (Qst_AllocateTextChoiceAnswer (Qst,NumOpt) == Err_ERROR)
	 /* Abort on error */
	 Ale_ShowAlertsAndExit ();

      /***** Copy answer text (row[0]) ******/
      Str_Copy (Qst->Answer.Options[NumOpt].Text,row[0],
                Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
     }

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void QstTxt_ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst)
  {
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];

   PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_BLANK;
   PrintedQst->Answer.Score = 0.0;	// Default score for blank or wrong answer

   if (PrintedQst->Answer.Str[0])	// If user has answered the answer
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,PrintedQst->Answer.Str,
		sizeof (TextAnsUsr) - 1);

      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);
      Str_ConvertToComparable (TextAnsUsr);

      PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_WRONG_ZERO;
      for (NumOpt = 0;
	   NumOpt < Qst->Answer.NumOpts;
	   NumOpt++)
        {
         /* Filter this correct answer */
         Str_Copy (TextAnsOK,Qst->Answer.Options[NumOpt].Text,
                   sizeof (TextAnsOK) - 1);
         Str_ConvertToComparable (TextAnsOK);

         /* Check is user answer is correct */
         if (!strcoll (TextAnsUsr,TextAnsOK))
           {
            PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_CORRECT;
	    PrintedQst->Answer.Score = 1.0;	// Correct answer
	    break;
           }
        }
     }
  }

/*****************************************************************************/
/***************************** Write text answer *****************************/
/*****************************************************************************/

void QstTxt_WriteCorrAns (struct Qst_Question *Qst,
			  const char *ClassTxt,
			  const char *ClassFeedback)
  {
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   unsigned NumOpt;

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Change format of answers feedback *****/
   Qst_ChangeFormatOptionsFeedback (Qst);

   HTM_TABLE_BeginPadding (2);
      for (NumOpt = 0;
	   NumOpt < Qst->Answer.NumOpts;
	   NumOpt++)
	{
	 HTM_TR_Begin (NULL);

	    /* Put an icon that indicates whether the answer is correct or wrong */
	    HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
	       Ico_PutIcon ("check.svg",Ico_BLACK,
			    Txt_TST_Answer_given_by_the_teachers,"CONTEXT_ICO16x16");
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LT\"");

	       /* Write the text of the answer and the media */
	       HTM_DIV_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
		  HTM_Txt (Qst->Answer.Options[NumOpt].Text);
		  Med_ShowMedia (&Qst->Answer.Options[NumOpt].Media,
				 "Tst_MED_EDIT_LIST_CONT","Tst_MED_EDIT_LIST");
	       HTM_DIV_End ();

	       /* Write the text of the feedback */
	       HTM_DIV_Begin ("class=\"%s_%s\"",ClassFeedback,The_GetSuffix ());
		  HTM_Txt (Qst->Answer.Options[NumOpt].Feedback);
	       HTM_DIV_End ();

	    HTM_TD_End ();

	 HTM_TR_End ();
	}
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************** Write text answer when assessing a test print ****************/
/*****************************************************************************/

void QstTxt_WriteTstFillAns (const struct Qst_PrintedQuestion *PrintedQst,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   char StrAns[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
   HTM_INPUT_TEXT (StrAns,Qst_MAX_CHARS_ANSWERS_ONE_QST,
		   PrintedQst->Answer.Str,
                   HTM_NO_ATTR,
		   "size=\"40\" class=\"INPUT_%s\"",The_GetSuffix ());
  }

void QstTxt_WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     __attribute__((unused)) const char *ClassTxt,
			     __attribute__((unused)) const char *ClassFeedback)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   Qst_WrongOrCorrect_t WrongOrCorrect;

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Change format of answers feedback *****/
   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT] == Usr_CAN)
      Qst_ChangeFormatOptionsFeedback (Qst);

   HTM_TR_Begin (NULL);

      /***** Write the user answer *****/
      if (PrintedQst->Answer.Str[0])	// If user has answered the question
	{
	 /* Filter the user answer */
	 Str_Copy (TextAnsUsr,PrintedQst->Answer.Str,sizeof (TextAnsUsr) - 1);

	 /* In order to compare student answer to stored answer,
	    the text answers are stored avoiding two or more consecurive spaces */
	 Str_ReplaceSeveralSpacesForOne (TextAnsUsr);

	 Str_ConvertToComparable (TextAnsUsr);

	 if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER] == Usr_CAN)
	    for (NumOpt = 0, WrongOrCorrect = Qst_WRONG;
		 NumOpt < Qst->Answer.NumOpts;
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
	 else
	    WrongOrCorrect = Qst_BLANK;

	 HTM_TD_Begin ("class=\"CT %s_%s\"",
		       Qst_AnswerDisplay[WrongOrCorrect].ClassStd,
		       The_GetSuffix ());
	    HTM_Txt (PrintedQst->Answer.Str);
	 HTM_TD_End ();
	}
      else						// If user has omitted the answer
	 HTM_TD_Empty (1);

      /***** Write the correct answers *****/
      switch (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	{
	 case Usr_CAN:
	    HTM_TD_Begin ("class=\"CT\"");
	       HTM_TABLE_BeginPadding (2);

		  for (NumOpt = 0;
		       NumOpt < Qst->Answer.NumOpts;
		       NumOpt++)
		    {
		     HTM_TR_Begin (NULL);

			/* Answer letter (a, b, c,...) */
			HTM_TD_Begin ("class=\"LT %s_%s\"",
				      Qst_AnswerDisplay[Qst_BLANK].ClassTch,
				      The_GetSuffix ());
			   HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
			HTM_TD_End ();

			/* Answer text and feedback */
			HTM_TD_Begin ("class=\"LT\"");

			   HTM_DIV_Begin ("class=\"%s_%s\"",
					  Qst_AnswerDisplay[Qst_BLANK].ClassTch,
					  The_GetSuffix ());
			      HTM_Txt (Qst->Answer.Options[NumOpt].Text);
			   HTM_DIV_End ();

			   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT] == Usr_CAN)
			      if (Qst->Answer.Options[NumOpt].Feedback)
				 if (Qst->Answer.Options[NumOpt].Feedback[0])
				   {
				    HTM_DIV_Begin ("class=\"Qst_TXT_LIGHT\"");
				       HTM_Txt (Qst->Answer.Options[NumOpt].Feedback);
				    HTM_DIV_End ();
				   }

			HTM_TD_End ();

		     HTM_TR_End ();
		    }

	       HTM_TABLE_End ();
	    HTM_TD_End ();
	    break;
	 case Usr_CAN_NOT:
	 default:
	    HTM_TD_Begin ("class=\"CT %s_%s\"",
			  Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
	       Ico_PutIconNotVisible ();
	    HTM_TD_End ();
	    break;
	}

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Write text answer in an exam answer sheet ******************/
/*****************************************************************************/

void QstTxt_WriteExaFillAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\""
	     " size=\"40\" maxlength=\"%u\" value=\"%s\"",
	     Id,Qst_MAX_CHARS_ANSWERS_ONE_QST,
	     Print->PrintedQsts[QstInd].Answer.Str);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_ElementEnd ();
  }

void QstTxt_WriteExaBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];

   HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT %s_%s\"",
		 Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
      HTM_NBSP ();
   HTM_TD_End ();
  }

void QstTxt_WriteExaCorrAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
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
	   NumOpt < Qst->Answer.NumOpts;
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

void QstTxt_WriteExaReadAns (const struct ExaPrn_Print *Print,
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
	   NumOpt < Qst->Answer.NumOpts;
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

void QstTxt_WriteExaEditAns (const struct ExaPrn_Print *Print,
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
	   NumOpt < Qst->Answer.NumOpts;
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
