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

#include <float.h>		// For DBL_MAX

#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_exam_sheet.h"
#include "swad_parameter.h"
#include "swad_question_database.h"
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

static void QstFlt_GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst);
static void QstFlt_ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst);

/*****************************************************************************/
/********************* Put input field for floating answer *******************/
/*****************************************************************************/

void QstFlt_PutInputField (const struct Qst_Question *Qst,unsigned Index)
  {
   extern const char *Txt_Real_number_between_A_and_B[2];
   static const char *Fields[2] =
     {
      "AnsFloatMin",
      "AnsFloatMax"
     };

   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_Txt (Txt_Real_number_between_A_and_B[Index]); HTM_NBSP ();
      HTM_INPUT_FLOAT (Fields[Index],-DBL_MAX,DBL_MAX,
		       0,Qst->Answer.FloatingPoint[Index],
		       (Qst->Answer.Type == Qst_ANS_FLOAT ? HTM_NO_ATTR :
							    HTM_DISABLED) |
		       HTM_REQUIRED,
		       "class=\"Exa_ANSWER_INPUT_FLOAT INPUT_%s\"",
		       The_GetSuffix ());
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/**************************** Get answer from form ***************************/
/*****************************************************************************/

void QstFlt_GetAnsFromForm (struct Qst_Question *Qst)
  {
   if (Qst_AllocateTextChoiceAnswer (Qst,0) == Err_ERROR)
      /* Abort on error */
      Ale_ShowAlertsAndExit ();

   Par_GetParText ("AnsFloatMin",Qst->Answer.Options[0].Text,
		   Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);

   if (Qst_AllocateTextChoiceAnswer (Qst,1) == Err_ERROR)
      /* Abort on error */
      Ale_ShowAlertsAndExit ();

   Par_GetParText ("AnsFloatMax",Qst->Answer.Options[1].Text,
		   Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
  }

/*****************************************************************************/
/******************* Check if question options are correct *******************/
/*****************************************************************************/

Err_SuccessOrError_t QstFlt_CheckIfOptsAreCorrect (struct Qst_Question *Qst)
  {
   extern const char *Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer;
   extern const char *Txt_The_lower_limit_of_correct_answers_must_be_less_than_or_equal_to_the_upper_limit;
   unsigned NumOpt;
   Err_SuccessOrError_t SuccessOrError;

   /* First two options should be filled */
   if (!Qst->Answer.Options[0].Text ||
       !Qst->Answer.Options[1].Text)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer);
      return Err_ERROR;
     }
   if (!Qst->Answer.Options[0].Text[0] ||
       !Qst->Answer.Options[1].Text[0])
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer);
      return Err_ERROR;
     }

   /* Lower limit should be <= upper limit */
   for (SuccessOrError  = Err_SUCCESS, NumOpt = 0;
	SuccessOrError == Err_SUCCESS && NumOpt < 2;
	NumOpt++)
      SuccessOrError = Str_GetDoubleFromStr (Qst->Answer.Options[NumOpt].Text,
					     &Qst->Answer.FloatingPoint[NumOpt]);
   switch (SuccessOrError)
     {
      case Err_SUCCESS:
	 if (Qst->Answer.FloatingPoint[0] >
	     Qst->Answer.FloatingPoint[1])
	   {
	    Ale_ShowAlert (Ale_WARNING,Txt_The_lower_limit_of_correct_answers_must_be_less_than_or_equal_to_the_upper_limit);
	    return Err_ERROR;
	   }
	 break;
      case Err_ERROR:
      default:
	 Ale_ShowAlert (Ale_WARNING,Txt_You_must_enter_the_range_of_floating_point_values_allowed_as_answer);
	 return Err_ERROR;
     }

   Qst->Answer.NumOpts = 2;

   return Err_SUCCESS;	// Question format without errors
  }

/*****************************************************************************/
/*********** Check if identical answer already exists in database ************/
/*****************************************************************************/

Exi_Exist_t QstFlt_IdenticalAnswersExist (MYSQL_RES *mysql_res,
					  __attribute__((unused)) unsigned NumOptsExistingQstInDB,
					  const struct Qst_Question *Qst)
  {
   MYSQL_ROW row;
   Exi_Exist_t IdenticalAnswersExist;
   unsigned NumOpt;
   double DoubleNum;

   for (IdenticalAnswersExist  = Exi_EXISTS, NumOpt = 0;
	IdenticalAnswersExist == Exi_EXISTS && NumOpt < 2;
	NumOpt++)
     {
      row = mysql_fetch_row (mysql_res);
      switch (Str_GetDoubleFromStr (row[0],&DoubleNum))
	{
	 case Err_SUCCESS:
	    IdenticalAnswersExist = DoubleNum ==
				    Qst->Answer.FloatingPoint[NumOpt] ? Exi_EXISTS :
									Exi_DOES_NOT_EXIST;
	    break;
	 case Err_ERROR:
	 default:
	    IdenticalAnswersExist = Exi_DOES_NOT_EXIST;
	    break;
	}
     }

   return IdenticalAnswersExist;
  }

/*****************************************************************************/
/*********************** Get question options from row ***********************/
/*****************************************************************************/

void QstFlt_GetQstOptionsFromRow (MYSQL_ROW row,struct Qst_Question *Qst,
				  unsigned NumOpt)
  {
   if (Qst->Answer.NumOpts != 2)
      Err_WrongAnswerExit ();
   if (Str_GetDoubleFromStr (row[1],
			     &Qst->Answer.FloatingPoint[NumOpt]) == Err_ERROR)
      Err_WrongAnswerExit ();
  }

/*****************************************************************************/
/******* Get correct answer and compute score for each type of answer ********/
/*****************************************************************************/

void QstFlt_GetCorrectAndComputeAnsScore (const char *Table,
					  struct Qst_PrintedQuestion *PrintedQst,
				          struct Qst_Question *Qst)
  {
   /***** Get the numerical value of the minimum and maximum correct answers,
          and compute score *****/
   QstFlt_GetCorrectAnswerFromDB (Table,Qst);
   QstFlt_ComputeAnsScore (PrintedQst,Qst);
  }

static void QstFlt_GetCorrectAnswerFromDB (const char *Table,
					   struct Qst_Question *Qst)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumOpt;
   Err_SuccessOrError_t SuccessOrError;
   double Tmp;

   /***** Query database *****/
   Qst->Answer.NumOpts = Qst_DB_GetTextOfAnswers (&mysql_res,Table,Qst->QstCod);

   /***** Check if number of rows is correct *****/
   if (Qst->Answer.NumOpts != 2)
      Err_WrongAnswerExit ();

   /***** Get float range *****/
   for (SuccessOrError = Err_SUCCESS, NumOpt = 0;
	SuccessOrError == Err_SUCCESS && NumOpt < 2;
	NumOpt++)
     {
      row = mysql_fetch_row (mysql_res);
      SuccessOrError = Str_GetDoubleFromStr (row[0],&Qst->Answer.FloatingPoint[NumOpt]);
     }
   if (SuccessOrError == Err_SUCCESS)
      if (Qst->Answer.FloatingPoint[0] >
	  Qst->Answer.FloatingPoint[1]) 	// The maximum and the minimum are swapped
       {
	 /* Swap maximum and minimum */
	 Tmp = Qst->Answer.FloatingPoint[0];
	 Qst->Answer.FloatingPoint[0] = Qst->Answer.FloatingPoint[1];
	 Qst->Answer.FloatingPoint[1] = Tmp;
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void QstFlt_ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst)
  {
   double AnsUsr;

   PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_BLANK;
   PrintedQst->Answer.Score = 0.0;	// Default score for blank or wrong answer

   if (PrintedQst->Answer.Str[0])	// If user has answered the answer
     {
      PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_WRONG_ZERO;
      if (Str_GetDoubleFromStr (PrintedQst->Answer.Str,&AnsUsr) == Err_SUCCESS)
	 if (AnsUsr >= Qst->Answer.FloatingPoint[0] &&
	     AnsUsr <= Qst->Answer.FloatingPoint[1])
	   {
	    PrintedQst->Answer.IsCorrect = Qst_ANSWER_IS_CORRECT;
	    PrintedQst->Answer.Score = 1.0; // Correct (inside the interval)
	   }
     }
  }

/*****************************************************************************/
/***************************** Write float answer ****************************/
/*****************************************************************************/

void QstFlt_WriteCorrAns (struct Qst_Question *Qst,
			  const char *ClassTxt,
			  __attribute__((unused)) const char *ClassFeedback)
  {
   HTM_SPAN_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
      HTM_OpenParenthesis ();
         HTM_DoubleRange (Qst->Answer.FloatingPoint[0],
                          Qst->Answer.FloatingPoint[1]);
      HTM_CloseParenthesis ();
   HTM_SPAN_End ();
  }

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
   if (Qst->Answer.NumOpts != 2)
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

void QstFlt_WriteExaFillAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,
			     __attribute__((unused)) struct Qst_Question *Qst)
  {
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write input field for the answer *****/
   snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
   HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
	     " class=\"Exa_ANSWER_INPUT_FLOAT\" value=\"%s\" step=\"any\"",
	     Id,Print->PrintedQsts[QstInd].Answer.Str);
   ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,-1);
   HTM_ElementEnd ();
  }

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
   if (Qst->Answer.NumOpts != 2)
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
   if (Qst->Answer.NumOpts != 2)
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
   if (Qst->Answer.NumOpts != 2)
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
