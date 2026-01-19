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
#include "swad_parameter.h"
#include "swad_question_database.h"
#include "swad_question_tf.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/* True/False internal values for forms and database */
const char *QstTF__Values[QstTF__NUM_OPTIONS] =
  {
   [QstTF__OPTION_EMPTY] = "" ,
   [QstTF__OPTION_TRUE ] = "T",
   [QstTF__OPTION_FALSE] = "F",
  };

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

static void QstTF__WriteTFOptionsToFill (QstTF__OptionTF_t OptTFStd);

/*****************************************************************************/
/*********************** Put input field for T/F answer **********************/
/*****************************************************************************/

void QstTF__PutInputField (const struct Qst_Question *Qst,QstTF__OptionTF_t OptTF)
  {
   HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_INPUT_RADIO ("AnsTF",
		       (Qst->Answer.OptionTF == OptTF ? HTM_CHECKED :
							HTM_NO_ATTR) |
		       HTM_REQUIRED |
		       (Qst->Answer.Type == Qst_ANS_TRUE_FALSE ? HTM_NO_ATTR :
								 HTM_DISABLED),
		       "value=\"%s\"",QstTF__Values[OptTF]);
      QstTF__WriteAnsTF (OptTF);
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/**************************** Get answer from form ***************************/
/*****************************************************************************/

void QstTF__GetAnsFromForm (struct Qst_Question *Qst)
  {
   char TF[1 + 1];	// (T)rue or (F)alse

   Par_GetParText ("AnsTF",TF,1);
   Qst->Answer.OptionTF = QstTF__GetOptionTFFromChar (TF[0]);
  }

/*****************************************************************************/
/******************* Check if question options are correct *******************/
/*****************************************************************************/

Err_SuccessOrError_t QstTF__CheckIfOptsAreCorrect (struct Qst_Question *Qst)
  {
   extern const char *Txt_You_must_select_a_T_F_answer;

   /***** Answer should be 'T' or 'F' *****/
   if (Qst->Answer.OptionTF == QstTF__OPTION_EMPTY)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_a_T_F_answer);
      return Err_ERROR;
     }

   Qst->Answer.NumOpts = 1;

   return Err_SUCCESS;	// Question format without errors
  }

/*****************************************************************************/
/*********************** Get question options from row ***********************/
/*****************************************************************************/

void QstTF__GetQstOptionsFromRow (MYSQL_ROW row,struct Qst_Question *Qst,
				  __attribute__((unused)) unsigned NumOpt)
  {
   Qst_CheckIfNumberOfAnswersIsOne (Qst);
   Qst->Answer.OptionTF = QstTF__GetOptionTFFromChar (row[1][0]);
  }

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
   Qst->Answer.NumOpts = Qst_DB_GetTextOfAnswers (&mysql_res,Table,Qst->QstCod);

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Get answer *****/
   row = mysql_fetch_row (mysql_res);
   Qst->Answer.OptionTF = QstTF__GetOptionTFFromChar (row[0][0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void QstTF__ComputeAnsScore (struct Qst_PrintedQuestion *PrintedQst,
				    const struct Qst_Question *Qst)
  {
   static double Scores[] =
     {
      [Qst_ANSWER_IS_CORRECT       ] =  1.0,
      [Qst_ANSWER_IS_WRONG_NEGATIVE] = -1.0,
      [Qst_ANSWER_IS_WRONG_ZERO    ] =  0.0,	// Not used
      [Qst_ANSWER_IS_WRONG_POSITIVE] =  0.0,	// Not used
      [Qst_ANSWER_IS_BLANK         ] =  0.0,
     };

   PrintedQst->Answer.IsCorrect = PrintedQst->Answer.Str[0] ?	// If user has selected T or F
				  (QstTF__GetOptionTFFromChar (PrintedQst->Answer.Str[0]) == Qst->Answer.OptionTF ?
				     Qst_ANSWER_IS_CORRECT :
				     Qst_ANSWER_IS_WRONG_NEGATIVE) :
				  Qst_ANSWER_IS_BLANK;
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
	 QstTF__WriteAnsTF (Qst->Answer.OptionTF);
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
      QstTF__WriteTFOptionsToFill (QstTF__GetOptionTFFromChar (PrintedQst->Answer.Str[0]));
   HTM_SELECT_End ();
  }

void QstTF__WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     __attribute__((unused)) const char *ClassTxt,
			     __attribute__((unused)) const char *ClassFeedback)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   QstTF__OptionTF_t OptTFStd;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Get answer true or false *****/
   OptTFStd = QstTF__GetOptionTFFromChar (PrintedQst->Answer.Str[0]);

   HTM_TR_Begin (NULL);

      /***** Write the user answer *****/
      HTM_TD_Begin ("class=\"CM %s_%s\"",
		    ICanView[TstVis_VISIBLE_CORRECT_ANSWER] == Usr_CAN ?
		       (OptTFStd == Qst->Answer.OptionTF ? "Qst_ANS_OK" :		// Correct
							   "Qst_ANS_BAD") :	// Wrong
							   "Qst_ANS_0",		// Blank answer
		    The_GetSuffix ());
	 QstTF__WriteAnsTF (OptTFStd);
      HTM_TD_End ();

      /***** Write the correct answer *****/
      HTM_TD_Begin ("class=\"CM %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassTch,The_GetSuffix ());
	 switch (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	   {
	    case Usr_CAN:
	       QstTF__WriteAnsTF (Qst->Answer.OptionTF);
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
   QstTF__WriteAnsTF (QstTF__OPTION_TRUE);
   HTM_Slash ();
   QstTF__WriteAnsTF (QstTF__OPTION_FALSE);
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
      QstTF__WriteTFOptionsToFill (QstTF__GetOptionTFFromChar (Print->PrintedQsts[QstInd].Answer.Str[0]));
   HTM_Txt ("</select>");
  }

void QstTF__WriteExaBlnkAns (__attribute__((unused)) const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   QstTF__OptionTF_t OptTF;

   for (OptTF  = QstTF__OPTION_TRUE;
	OptTF <= QstTF__OPTION_FALSE;
	OptTF++)
     {
      HTM_TD_Begin ("class=\"Exa_ANSWER_TF %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
	 QstTF__WriteAnsTF (OptTF);
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
      QstTF__WriteAnsTF (Qst->Answer.OptionTF);
   HTM_TD_End ();
  }

void QstTF__WriteExaReadAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   QstTF__OptionTF_t OptTFStd;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Get answer true or false *****/
   OptTFStd = QstTF__GetOptionTFFromChar (Print->PrintedQsts[QstInd].Answer.Str[0]);

   /***** Write online answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF %s_%s\"",
		 OptTFStd == Qst->Answer.OptionTF ? "Qst_ANS_OK" :	// Correct
						    "Qst_ANS_BAD",	// Blank answer
		 The_GetSuffix ());
      QstTF__WriteAnsTF (OptTFStd);
   HTM_TD_End ();
  }

void QstTF__WriteExaEditAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   QstTF__OptionTF_t OptTFStd = QstTF__GetOptionTFFromChar (Print->PrintedQsts[QstInd].Answer.Str[0]);
   Qst_WrongOrCorrect_t WrongOrCorrect;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Qst);

   /***** Check if student answer is blank, wrong or correct *****/
   switch (OptTFStd)
     {
      case QstTF__OPTION_TRUE:
      case QstTF__OPTION_FALSE:
	 WrongOrCorrect = OptTFStd == Qst->Answer.OptionTF ? Qst_CORRECT :
							     Qst_WRONG;
	 break;
      case QstTF__OPTION_EMPTY:
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
         QstTF__WriteTFOptionsToFill (OptTFStd);
      HTM_Txt ("</select>");

   HTM_TD_End ();
  }

/*****************************************************************************/
/******************** Get true / false option from char **********************/
/*****************************************************************************/

QstTF__OptionTF_t QstTF__GetOptionTFFromChar (char TF)
  {
   QstTF__OptionTF_t OptTF;

   if (TF)
      for (OptTF  = QstTF__OPTION_TRUE;
	   OptTF <= QstTF__OPTION_FALSE;
	   OptTF++)
	 if (TF == QstTF__Values[OptTF][0])
	    return OptTF;

   return QstTF__OPTION_EMPTY;
  }

/*****************************************************************************/
/********* Write true / false options to be filled in a form select **********/
/*****************************************************************************/

static void QstTF__WriteTFOptionsToFill (QstTF__OptionTF_t OptTFStd)
  {
   extern const char *Txt_TF_QST[QstTF__NUM_OPTIONS];	// Value displayed on screen
   QstTF__OptionTF_t OptTF;

   /***** Write selector for the answer *****/
   for (OptTF  = (QstTF__OptionTF_t) 0;
	OptTF <= (QstTF__OptionTF_t) (QstTF__NUM_OPTIONS - 1);
	OptTF++)
      HTM_OPTION (HTM_Type_STRING,QstTF__Values[OptTF],
		  OptTFStd == OptTF ? HTM_SELECTED :
				      HTM_NO_ATTR,
		  Txt_TF_QST[OptTF]);
  }

/*****************************************************************************/
/************** Write true / false answer when seeing a test *****************/
/*****************************************************************************/

void QstTF__WriteAnsTF (QstTF__OptionTF_t OptionTF)
  {
   extern const char *Txt_TF_QST[QstTF__NUM_OPTIONS];	// Value displayed on screen

   HTM_Txt (Txt_TF_QST[OptionTF]);
  }
