// swad_exam_answer_sheet.c: exams blank answer sheets and exams solved answer sheets

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_exam_database.h"
#include "swad_exam_print.h"
#include "swad_exam_set.h"
#include "swad_exam_sheet.h"
#include "swad_exam_sheet_answer.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_question.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

static const char *ExaSheAns_Class[Qst_NUM_WRONG_CORRECT] =
  {
   [Qst_BLANK  ] = "Qst_ANS_0",
   [Qst_WRONG  ] = "Qst_ANS_BAD",
   [Qst_CORRECT] = "Qst_ANS_OK",
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaSheAns_WriteQst (const struct ExaSes_Session *Session,
			        const struct ExaPrn_Print *Print,
				ExaShe_BlankOrSolved_t BlankOrSolved,
				Vie_ViewType_t ViewType,
                                unsigned QstInd,struct Qst_Question *Question);

static void ExaSheAns_WriteBlankAnswers (struct Qst_Question *Question);
static void ExaSheAns_WriteBlankIntAns (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaSheAns_WriteBlankFltAns (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaSheAns_WriteBlankTF_Ans (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaSheAns_WriteBlankChoAns (const struct Qst_Question *Question);
static void ExaSheAns_WriteBlankTxtAns (__attribute__((unused)) const struct Qst_Question *Question);

static void ExaSheAns_WriteSolvedAnswers (const struct ExaSes_Session *Session,
					  const struct ExaPrn_Print *Print,
					  Vie_ViewType_t ViewType,
					  unsigned QstInd,
					  struct Qst_Question *Question);

static void ExaSheAns_WriteCorrectIntAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question);
static void ExaSheAns_WriteCorrectFltAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question);
static void ExaSheAns_WriteCorrectTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question);
static void ExaSheAns_WriteCorrectChoAns (const struct ExaPrn_Print *Print,
					  unsigned QstInd,struct Qst_Question *Question);
static void ExaSheAns_WriteCorrectTxtAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question);

static void ExaSheAns_WriteReadonlyIntAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteReadonlyFltAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteReadonlyTF_Ans (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteReadonlyChoAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteReadonlyTxtAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);

static void ExaSheAns_WriteEditableIntAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteEditableFltAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteEditableTF_Ans (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteEditableChoAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteEditableTxtAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question);
static void ExaSheAns_WriteJSToUpdateSheet (const struct ExaPrn_Print *Print,
	                                    unsigned QstInd,
	                                    const char *Id,int NumOpt);

static void ExaSheAns_WriteHead (ExaSes_Modality_t Modality);

/*****************************************************************************/
/************ Show the main part (table) of an exam answer sheet *************/
/*****************************************************************************/

void ExaSheAns_ShowAnswers (const struct ExaSes_Session *Session,
			    const struct ExaPrn_Print *Print,
			    ExaShe_BlankOrSolved_t BlankOrSolved,
			    Vie_ViewType_t ViewType)
  {
   extern const char *Txt_Answers;
   static struct ExaSet_Set CurrentSet =
     {
      .ExaCod = -1L,
      .SetCod = -1L,
      .SetInd = 0,
      .NumQstsToPrint = 0,
      .Title[0] = '\0'
     };
   unsigned QstInd;
   struct Qst_Question Question;

   /***** Heading *****/
   HTM_DIV_Begin ("class=\"Exa_COL_SPAN Exa_SHEET_TITLE_%s\"",The_GetSuffix ());
      HTM_Txt (Txt_Answers);
   HTM_DIV_End ();

   /***** Write questions in columns *****/
   HTM_DIV_Begin ("class=\"Exa_COLS_%u\"",Session->NumCols);

      /***** Write one row for each question *****/
      for (QstInd = 0, CurrentSet.SetCod = -1L;
	   QstInd < Print->NumQsts.All;
	   QstInd++)
	{
	 if (Print->Qsts[QstInd].SetCod != CurrentSet.SetCod)
	   {
	    /* Get data of this set */
	    CurrentSet.SetCod = Print->Qsts[QstInd].SetCod;
	    ExaSet_GetSetDataByCod (&CurrentSet);

	    /* Title for this set */
	    HTM_DIV_Begin ("class=\"Exa_COL_SPAN %s\"",The_GetColorRows ());
	       ExaSet_WriteSetTitle (&CurrentSet);
	    HTM_DIV_End ();
	   }

	 /* Create test question */
	 Qst_QstConstructor (&Question);

	    /* Get question from database */
	    Question.QstCod = Print->Qsts[QstInd].QstCod;
	    ExaSet_GetQstDataFromDB (&Question);

	    /* Write question answers */
	    ExaSheAns_WriteQst (Session,Print,BlankOrSolved,ViewType,
				QstInd,&Question);

	 /* Destroy test question */
	 Qst_QstDestructor (&Question);
	}

   /***** End list of questions *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*** Write a row of an exam answer sheet, with the answer to one question ****/
/*****************************************************************************/

static void ExaSheAns_WriteQst (const struct ExaSes_Session *Session,
			        const struct ExaPrn_Print *Print,
				ExaShe_BlankOrSolved_t BlankOrSolved,
				Vie_ViewType_t ViewType,
                                unsigned QstInd,struct Qst_Question *Question)
  {
   /***** Begin row *****/
   HTM_DIV_Begin ("class=\"Exa_CONTAINER\"");

      /***** Number of question and answer type *****/
      HTM_DIV_Begin ("class=\"Exa_LEFT\"");
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Question->Answer.Type,Question->Validity);
      HTM_DIV_End ();

      /***** Answers *****/
      HTM_DIV_Begin ("class=\"Exa_RIGHT\"");
	 switch (BlankOrSolved)
	   {
	    case ExaShe_BLANK:
	       ExaSheAns_WriteBlankAnswers (Question);
	       break;
	    case ExaShe_SOLVED:
	       Frm_BeginFormNoAction ();	// Form that can not be submitted, to avoid enter key to send it
		  ExaSheAns_WriteSolvedAnswers (Session,Print,ViewType,
						QstInd,Question);
	       Frm_EndForm ();
	       break;
	    default:
	       break;
	   }
      HTM_DIV_End ();

   /***** End row *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************ Write answers of a question in an exam answer sheet ************/
/*****************************************************************************/

static void ExaSheAns_WriteBlankAnswers (struct Qst_Question *Question)
  {
   static void (*ExaSheAns_WriteBlankAns[Qst_NUM_ANS_TYPES]) (const struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = ExaSheAns_WriteBlankIntAns,
     [Qst_ANS_FLOAT          ] = ExaSheAns_WriteBlankFltAns,
     [Qst_ANS_TRUE_FALSE     ] = ExaSheAns_WriteBlankTF_Ans,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaSheAns_WriteBlankChoAns,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaSheAns_WriteBlankChoAns,
     [Qst_ANS_TEXT           ] = ExaSheAns_WriteBlankTxtAns,
    };

   ExaSheAns_WriteBlankAns[Question->Answer.Type] (Question);
  }

/*****************************************************************************/
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

static void ExaSheAns_WriteBlankIntAns (__attribute__((unused)) const struct Qst_Question *Question)
  {
   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write the student answer *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("class=\"Exa_ANSWER_INT Qst_ANS_0_%s\"",The_GetSuffix ());
	    HTM_NBSP ();
	 HTM_TD_End ();
      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/**************** Write float answer in an exam answer sheet *****************/
/*****************************************************************************/

static void ExaSheAns_WriteBlankFltAns (__attribute__((unused)) const struct Qst_Question *Question)
  {
   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write the student answer *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT Qst_ANS_0_%s\"",The_GetSuffix ());
	    HTM_NBSP ();
	 HTM_TD_End ();
      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************* Write false / true answer in an exam answer sheet *************/
/*****************************************************************************/

static void ExaSheAns_WriteBlankTF_Ans (__attribute__((unused)) const struct Qst_Question *Question)
  {
   extern const char *Txt_TF_QST[2];
   unsigned NumOpt;

   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write the student answer *****/
      HTM_TR_Begin (NULL);

	 for (NumOpt = 0;
	      NumOpt < 2;
	      NumOpt++)
	   {
	    HTM_TD_Begin ("class=\"Exa_ANSWER_TF Qst_ANS_0_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_TF_QST[NumOpt]);
	    HTM_TD_End ();
	   }

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/****** Write single or multiple choice answer in an exam answer sheet *******/
/*****************************************************************************/

static void ExaSheAns_WriteBlankChoAns (const struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write student's answers (one column per answer) *****/
      HTM_TR_Begin (NULL);
	 for (NumOpt = 0;
	      NumOpt < Question->Answer.NumOptions;
	      NumOpt++)
	   {
	    HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE Qst_ANS_0_%s\"",The_GetSuffix ());
	       HTM_Option (NumOpt);
	    HTM_TD_End ();
	   }
      HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/**************** Write text answer in an exam answer sheet ******************/
/*****************************************************************************/

static void ExaSheAns_WriteBlankTxtAns (__attribute__((unused)) const struct Qst_Question *Question)
  {
   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write the student answer *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT Qst_ANS_0_%s\"",The_GetSuffix ());
	    HTM_NBSP ();
	 HTM_TD_End ();
      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************ Write answers of a question in an exam answer sheet ************/
/*****************************************************************************/

static void ExaSheAns_WriteSolvedAnswers (const struct ExaSes_Session *Session,
					  const struct ExaPrn_Print *Print,
					  Vie_ViewType_t ViewType,
					  unsigned QstInd,
					  struct Qst_Question *Question)
  {
   static void (*ExaSheAns_WriteCorrectAns[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
								unsigned QstInd,
								struct Qst_Question *Question) =
     {
      [Qst_ANS_INT            ] = ExaSheAns_WriteCorrectIntAns,
      [Qst_ANS_FLOAT          ] = ExaSheAns_WriteCorrectFltAns,
      [Qst_ANS_TRUE_FALSE     ] = ExaSheAns_WriteCorrectTF_Ans,
      [Qst_ANS_UNIQUE_CHOICE  ] = ExaSheAns_WriteCorrectChoAns,
      [Qst_ANS_MULTIPLE_CHOICE] = ExaSheAns_WriteCorrectChoAns,
      [Qst_ANS_TEXT           ] = ExaSheAns_WriteCorrectTxtAns,
     };
   static void (*ExaSheAns_WriteReadonlyAns[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
								 unsigned QstInd,
								 struct Qst_Question *Question) =
     {
      [Qst_ANS_INT            ] = ExaSheAns_WriteReadonlyIntAns,
      [Qst_ANS_FLOAT          ] = ExaSheAns_WriteReadonlyFltAns,
      [Qst_ANS_TRUE_FALSE     ] = ExaSheAns_WriteReadonlyTF_Ans,
      [Qst_ANS_UNIQUE_CHOICE  ] = ExaSheAns_WriteReadonlyChoAns,
      [Qst_ANS_MULTIPLE_CHOICE] = ExaSheAns_WriteReadonlyChoAns,
      [Qst_ANS_TEXT           ] = ExaSheAns_WriteReadonlyTxtAns,
     };
   static void (*ExaSheAns_WriteEditableAns[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
								 unsigned QstInd,
								 struct Qst_Question *Question) =
     {
      [Qst_ANS_INT            ] = ExaSheAns_WriteEditableIntAns,
      [Qst_ANS_FLOAT          ] = ExaSheAns_WriteEditableFltAns,
      [Qst_ANS_TRUE_FALSE     ] = ExaSheAns_WriteEditableTF_Ans,
      [Qst_ANS_UNIQUE_CHOICE  ] = ExaSheAns_WriteEditableChoAns,
      [Qst_ANS_MULTIPLE_CHOICE] = ExaSheAns_WriteEditableChoAns,
      [Qst_ANS_TEXT           ] = ExaSheAns_WriteEditableTxtAns,
     };

   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write the answer *****/
      HTM_TR_Begin (NULL);
	 ExaSheAns_WriteHead (ExaSes_NONE);
	 ExaSheAns_WriteCorrectAns[Question->Answer.Type] (Print,QstInd,Question);
      HTM_TR_End ();

      /***** Write the answer *****/
      HTM_TR_Begin (NULL);
	 ExaSheAns_WriteHead (Session->Modality);
	 if (Session->Modality == ExaSes_PAPER && ViewType == Vie_VIEW)
	    ExaSheAns_WriteEditableAns[Question->Answer.Type] (Print,QstInd,Question);
	 else
	    ExaSheAns_WriteReadonlyAns[Question->Answer.Type] (Print,QstInd,Question);
      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

static void ExaSheAns_WriteCorrectIntAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question)
  {
   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_INT Qst_ANS_0_%s\"",The_GetSuffix ());
      HTM_Long (Question->Answer.Integer);
   HTM_TD_End ();
  }

/*****************************************************************************/
/**************** Write float answer in an exam answer sheet *****************/
/*****************************************************************************/

static void ExaSheAns_WriteCorrectFltAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question)
  {
   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT Qst_ANS_0_%s\"",The_GetSuffix ());
      HTM_DoubleRange (Question->Answer.FloatingPoint[0],
		       Question->Answer.FloatingPoint[1]);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Write false / true answer in an exam answer sheet *************/
/*****************************************************************************/

static void ExaSheAns_WriteCorrectTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question)
  {
   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF Qst_ANS_0_%s\"",The_GetSuffix ());
      Qst_WriteAnsTF (Question->Answer.TF);
   HTM_TD_End ();
  }

/*****************************************************************************/
/****** Write single or multiple choice answer in an exam answer sheet *******/
/*****************************************************************************/

static void ExaSheAns_WriteCorrectChoAns (const struct ExaPrn_Print *Print,
					  unsigned QstInd,
					  struct Qst_Question *Question)
  {
   static const char *Correct[Qst_NUM_WRONG_CORRECT] =
     {
      [Qst_BLANK  ] = "Qst_ANS_0",
      [Qst_WRONG  ] = "Qst_TXT_LIGHT",
      [Qst_CORRECT] = "Qst_ANS_0",
     };
   unsigned NumOpt;
   Qst_WrongOrCorrect_t OptionWrongOrCorrect;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->Qsts[QstInd].StrIndexes,Indexes);

   /***** Write correct answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      OptionWrongOrCorrect = Question->Answer.Options[Indexes[NumOpt]].Correct;

      /* Write option letter */
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		    Correct[OptionWrongOrCorrect],The_GetSuffix ());
	 HTM_Option (NumOpt);
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/**************** Write text answer in an exam answer sheet ******************/
/*****************************************************************************/

static void ExaSheAns_WriteCorrectTxtAns (__attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd,
					  struct Qst_Question *Question)
  {
   unsigned NumOpt;

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Write the correct answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT Qst_ANS_0_%s\"",The_GetSuffix ());
      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 if (NumOpt)
	   {
	    HTM_Semicolon (); HTM_NBSP ();
	   }
	 HTM_Txt (Question->Answer.Options[NumOpt].Text);
	}
   HTM_TD_End ();
  }

/*****************************************************************************/
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

static void ExaSheAns_WriteReadonlyIntAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   long IntAnswerUsr;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Write online answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      if (sscanf (Print->Qsts[QstInd].Answer.Str,
		  "%ld",&IntAnswerUsr) == 1)
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_INT %s_%s\"",
		       IntAnswerUsr == Question->Answer.Integer ? "Qst_ANS_OK" :	// Correct
								  "Qst_ANS_BAD",	// Wrong
		       The_GetSuffix ());
	    HTM_Long (IntAnswerUsr);
	 HTM_TD_End ();
	}
      else
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_INT Qst_ANS_0_%s\"",The_GetSuffix ());
	    HTM_Question ();
	 HTM_TD_End ();
	}
     }
   else						// If student has omitted the answer
      HTM_TD_Empty (1);
  }

/*****************************************************************************/
/**************** Write float answer in an exam answer sheet *****************/
/*****************************************************************************/

static void ExaSheAns_WriteReadonlyFltAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   double AnsUsr = 0.0;
   Err_SuccessOrError_t SuccessOrError;

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write online answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      SuccessOrError = Str_GetDoubleFromStr (Print->Qsts[QstInd].Answer.Str,
					     &AnsUsr);
      // A bad formatted floating point answer will interpreted as 0.0
      HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT %s_%s\"",
		    SuccessOrError == Err_SUCCESS ? (AnsUsr >= Question->Answer.FloatingPoint[0] &&
						     AnsUsr <= Question->Answer.FloatingPoint[1] ? "Qst_ANS_OK" :	// Correct
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

/*****************************************************************************/
/************* Write false / true answer in an exam answer sheet *************/
/*****************************************************************************/

static void ExaSheAns_WriteReadonlyTF_Ans (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   char AnsTFStd;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   AnsTFStd = Print->Qsts[QstInd].Answer.Str[0];

   /***** Write online answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF %s_%s\"",
		 AnsTFStd == Question->Answer.TF ? "Qst_ANS_OK" :	// Correct
						   "Qst_ANS_BAD",	// Blank answer
		 The_GetSuffix ());
      Qst_WriteAnsTF (AnsTFStd);
   HTM_TD_End ();
  }

/*****************************************************************************/
/****** Write single or multiple choice answer in an exam answer sheet *******/
/*****************************************************************************/

static void ExaSheAns_WriteReadonlyChoAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   struct
     {
      char *Class;
      char *Str;
     } Online[Qst_NUM_WRONG_CORRECT] =
     {
      [Qst_BLANK  ] = {.Class = "Qst_ANS_0"  ,.Str = "&nbsp;" },
      [Qst_WRONG  ] = {.Class = "Qst_ANS_BAD",.Str = "&cross;"},
      [Qst_CORRECT] = {.Class = "Qst_ANS_OK" ,.Str = "&check;"}
     };
   unsigned NumOpt;
   Qst_WrongOrCorrect_t OptionWrongOrCorrect;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->Qsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (Print->Qsts[QstInd].Answer.Str,UsrAnswers);

   /***** Write online answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      OptionWrongOrCorrect = Question->Answer.Options[Indexes[NumOpt]].Correct;

      /* Write letter depending on user's answer */
      if (UsrAnswers[Indexes[NumOpt]] == HTM_CHECKED)	// This answer has been selected by the user
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		       Online[OptionWrongOrCorrect].Class,The_GetSuffix ());
	    HTM_Txt (Online[OptionWrongOrCorrect].Str);
	 HTM_TD_End ();
	}
      else	// This answer has NOT been selected by the user
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE Qst_ANS_0_%s\"",
		       The_GetSuffix ());
	    HTM_NBSP ();
	 HTM_TD_End ();
	}
     }
  }

/*****************************************************************************/
/**************** Write text answer in an exam answer sheet ******************/
/*****************************************************************************/

static void ExaSheAns_WriteReadonlyTxtAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   static const char *Class[Qst_NUM_WRONG_CORRECT] =
     {
      [Qst_BLANK  ] = "Qst_ANS_0",
      [Qst_WRONG  ] = "Qst_ANS_BAD",
      [Qst_CORRECT] = "Qst_ANS_OK",
     };
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   Qst_WrongOrCorrect_t WrongOrCorrect;

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Write online answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Print->Qsts[QstInd].Answer.Str,
		sizeof (TextAnsUsr) - 1);

      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);

      Str_ConvertToComparable (TextAnsUsr);

      for (NumOpt = 0, WrongOrCorrect = Qst_WRONG;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 /* Filter this correct answer */
	 Str_Copy (TextAnsOK,Question->Answer.Options[NumOpt].Text,
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
		    Class[WrongOrCorrect],The_GetSuffix ());
	 HTM_Txt (Print->Qsts[QstInd].Answer.Str);
      HTM_TD_End ();
     }
   else						// If student has omitted the answer
      HTM_TD_Empty (1);
  }

/*****************************************************************************/
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

static void ExaSheAns_WriteEditableIntAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   long AnsUsr;
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_BLANK;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
      if (sscanf (Print->Qsts[QstInd].Answer.Str,"%ld",&AnsUsr) == 1)
	 WrongOrCorrect = AnsUsr == Question->Answer.Integer ? Qst_CORRECT :
							       Qst_WRONG;

   HTM_TD_Begin ("class=\"Exa_ANSWER_INT\"");

      /***** Write input field for the answer *****/
      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
		" class=\"Exa_ANSWER_INPUT_INT %s_%s\" value=\"",
		Id,ExaSheAns_Class[WrongOrCorrect],The_GetSuffix ());
      if (WrongOrCorrect != Qst_BLANK)
	 HTM_Long (AnsUsr);
      HTM_Char ('"');
      ExaSheAns_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/**************** Write float answer in an exam answer sheet *****************/
/*****************************************************************************/

static void ExaSheAns_WriteEditableFltAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   double AnsUsr;
   Err_SuccessOrError_t SuccessOrError;
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_BLANK;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      SuccessOrError = Str_GetDoubleFromStr (Print->Qsts[QstInd].Answer.Str,&AnsUsr);
      // A bad formatted floating point answer will interpreted as 0.0
      WrongOrCorrect = SuccessOrError == Err_SUCCESS ? (AnsUsr >= Question->Answer.FloatingPoint[0] &&
							AnsUsr <= Question->Answer.FloatingPoint[1] ? Qst_CORRECT :
												      Qst_WRONG) :
							Qst_BLANK;
     }

   HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT\"");

      /***** Write input field for the answer *****/
      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
		" class=\"Exa_ANSWER_INPUT_FLOAT %s_%s\" value=\"%s\"",
		Id,ExaSheAns_Class[WrongOrCorrect],The_GetSuffix (),
		Print->Qsts[QstInd].Answer.Str);
      ExaSheAns_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Write false / true answer in an exam answer sheet *************/
/*****************************************************************************/

static void ExaSheAns_WriteEditableTF_Ans (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   extern const char *Txt_NBSP;
   extern const char *Txt_TF_QST[2];
   char AnsUsr;
   Qst_WrongOrCorrect_t WrongOrCorrect;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   AnsUsr = Print->Qsts[QstInd].Answer.Str[0];
   WrongOrCorrect = AnsUsr == '\0' ? Qst_BLANK :
				     (AnsUsr == Question->Answer.TF ? Qst_CORRECT :
							              Qst_WRONG);

   /***** Write paper answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF\"");

      /***** Write selector for the answer *****/
      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<select id=\"%s\" name=\"Ans\" class=\"%s_%s\"",
		Id,ExaSheAns_Class[WrongOrCorrect],The_GetSuffix ());
      ExaSheAns_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();
	 HTM_OPTION (HTM_Type_STRING,"" ,AnsUsr == '\0' ? HTM_SELECTED :
							  HTM_NO_ATTR,
		     Txt_NBSP);
	 HTM_OPTION (HTM_Type_STRING,"T",AnsUsr == 'T'  ? HTM_SELECTED :
							  HTM_NO_ATTR,
		     Txt_TF_QST[0]);
	 HTM_OPTION (HTM_Type_STRING,"F",AnsUsr == 'F'  ? HTM_SELECTED :
							  HTM_NO_ATTR,
		     Txt_TF_QST[1]);
      HTM_Txt ("</select>");

   HTM_TD_End ();
  }

/*****************************************************************************/
/****** Write single or multiple choice answer in an exam answer sheet *******/
/*****************************************************************************/

static void ExaSheAns_WriteEditableChoAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
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
   TstPrn_GetIndexesFromStr (Print->Qsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (Print->Qsts[QstInd].Answer.Str,UsrAnswers);

   /***** Write paper answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE\"");

         snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
	 HTM_TxtF ("<input type=\"%s\" id=\"%s_%u\" name=\"Ans\" value=\"%u\"",
		   InputType[Question->Answer.Type],
		   Id,NumOpt,Indexes[NumOpt]);
	 if ((UsrAnswers[Indexes[NumOpt]] & HTM_CHECKED))
	    HTM_Txt (" checked");
	 ExaSheAns_WriteJSToUpdateSheet (Print,QstInd,Id,(int) NumOpt);
	 HTM_ElementEnd ();

      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/**************** Write text answer in an exam answer sheet ******************/
/*****************************************************************************/

static void ExaSheAns_WriteEditableTxtAns (const struct ExaPrn_Print *Print,
					   unsigned QstInd,
					   struct Qst_Question *Question)
  {
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_BLANK;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Print->Qsts[QstInd].Answer.Str,
	        sizeof (TextAnsUsr) - 1);

      /* In order to compare student answer to stored answer,
	 the text answers are stored avoiding two or more consecurive spaces */
      Str_ReplaceSeveralSpacesForOne (TextAnsUsr);

      Str_ConvertToComparable (TextAnsUsr);

      /* Change format of answers text */
      Qst_ChangeFormatAnswersText (Question);

      for (NumOpt = 0, WrongOrCorrect = Qst_WRONG;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 /* Filter this correct answer */
	 Str_Copy (TextAnsOK,Question->Answer.Options[NumOpt].Text,
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

   HTM_TD_Begin ("class=\"Exa_ANSWER_TEXT\"");

      /***** Write input field for the answer *****/
      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\" maxlength=\"%u\""
		" class=\"Exa_ANSWER_INPUT_TEXT %s_%s\" value=\"%s\"",
		Id,Qst_MAX_CHARS_ANSWERS_ONE_QST,
		ExaSheAns_Class[WrongOrCorrect],The_GetSuffix (),
		Print->Qsts[QstInd].Answer.Str);
      ExaSheAns_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

static void ExaSheAns_WriteJSToUpdateSheet (const struct ExaPrn_Print *Print,
	                                    unsigned QstInd,
	                                    const char *Id,int NumOpt)
  {
   char *Pars;

   if (asprintf (&Pars,"act=%ld&ses=%s&SesCod=%ld&OtherUsrCod=%s&QstInd=%u",
		 Act_GetActCod (ActAnsExaAnsShe),Gbl.Session.Id,
		 Print->SesCod,Print->EnUsrCod,QstInd) < 0)
      Err_NotEnoughMemoryExit ();

   if (NumOpt < 0)
      HTM_TxtF (" onchange=\"updateExamPrint('examprint_%s','%s','Ans','%s',%u);",
		Print->EnUsrCod,Id,Pars,(unsigned) Gbl.Prefs.Language);
   else	// NumOpt >= 0
      HTM_TxtF (" onclick=\"updateExamPrint('examprint_%s','%s_%d','Ans','%s',%u);",
		Print->EnUsrCod,Id,NumOpt,Pars,(unsigned) Gbl.Prefs.Language);
   HTM_Txt (" return false;\"");	// return false is necessary to not submit form

   free (Pars);
  }

/*****************************************************************************/
/************************* Write head for the answer *************************/
/*****************************************************************************/

static void ExaSheAns_WriteHead (ExaSes_Modality_t Modality)
  {
   extern const char *Txt_EXAM_ANSWER_TYPES[ExaSes_NUM_MODALITIES];
   static struct
     {
      const char *Icon;
      Ico_Color_t Color;
     } AnswerTypeIcons[ExaSes_NUM_MODALITIES] =
     {
      [ExaSes_NONE  ] = {.Icon = "check.svg"		,.Color = Ico_GREEN},
      [ExaSes_ONLINE] = {.Icon = "display.svg"		,.Color = Ico_BLACK},
      [ExaSes_PAPER ] = {.Icon = "file-signature.svg"	,.Color = Ico_BLACK},
     };

   HTM_TD_Begin ("class=\"LM DAT_SMALL_%s\"",The_GetSuffix ());
      Ico_PutIconOff (AnswerTypeIcons[Modality].Icon,
		      AnswerTypeIcons[Modality].Color,
		      Txt_EXAM_ANSWER_TYPES[Modality]);
   HTM_TD_End ();
  }
