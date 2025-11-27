// swad_exam_question_sheet.c: exams question sheets

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

#include "swad_exam_sheet_question.h"

#include <stddef.h>		// For NULL

#include "swad_exam_print.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_exam_sheet.h"
#include "swad_global.h"
#include "swad_group.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaSheQst_WriteQst (const struct ExaPrn_Print *Print,
				unsigned QstInd,
				struct Qst_Question *Question);
static void ExaSheQst_WriteAnswers (const struct ExaPrn_Print *Print,
				    unsigned QstInd,
				    struct Qst_Question *Question);
static void ExaSheQst_WriteTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
	                           __attribute__((unused)) unsigned QstInd,
                                   __attribute__((unused)) struct Qst_Question *Question);
static void ExaSheQst_WriteChoAns (const struct ExaPrn_Print *Print,
                                   unsigned QstInd,
                                   struct Qst_Question *Question);

/*****************************************************************************/
/*********** Show the main part (table) of an exam question sheet ************/
/*****************************************************************************/

void ExaSheQst_ShowQuestions (const struct ExaSes_Session *Session,
			      const struct ExaPrn_Print *Print)
  {
   extern const char *Txt_Questions;
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
      HTM_Txt (Txt_Questions);
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
	 Question.QstCod = Print->Qsts[QstInd].QstCod;

	    /* Get question from database */
	    ExaSet_GetQstDataFromDB (&Question);

	    /* Write question */
	    ExaSheQst_WriteQst (Print,QstInd,&Question);

	 /* Destroy test question */
	 Qst_QstDestructor (&Question);
	}

   /***** End list of questions *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Write a row of an exam answer sheet, with one question ***********/
/*****************************************************************************/

static void ExaSheQst_WriteQst (const struct ExaPrn_Print *Print,
				unsigned QstInd,
				struct Qst_Question *Question)
  {
   /***** Begin row *****/
   HTM_DIV_Begin ("class=\"Exa_CONTAINER\"");

      /***** Number of question and answer type *****/
      HTM_DIV_Begin ("class=\"Exa_LEFT\"");
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Question->Answer.Type,Question->Validity);
      HTM_DIV_End ();

      /***** Stem, media and answers *****/
      HTM_DIV_Begin ("class=\"Exa_RIGHT\"");
	 Qst_WriteQstStem (Question->Stem,"Qst_TXT",HidVis_VISIBLE);
	 Med_ShowMedia (&Question->Media,"Tst_MED_SHOW_CONT","Tst_MED_SHOW");
	 ExaSheQst_WriteAnswers (Print,QstInd,Question);
      HTM_DIV_End ();

   /***** End row *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********** Write answers of a question in an exam question sheet ***********/
/*****************************************************************************/

static void ExaSheQst_WriteAnswers (const struct ExaPrn_Print *Print,
				    unsigned QstInd,
				    struct Qst_Question *Question)
  {
   void (*ExaSheQst_WriteAns[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
                                                  unsigned QstInd,
                                                  struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = NULL,
     [Qst_ANS_FLOAT          ] = NULL,
     [Qst_ANS_TRUE_FALSE     ] = ExaSheQst_WriteTF_Ans,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaSheQst_WriteChoAns,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaSheQst_WriteChoAns,
     [Qst_ANS_TEXT           ] = NULL,
    };

   /***** Write answers *****/
   if (ExaSheQst_WriteAns[Question->Answer.Type])
      ExaSheQst_WriteAns[Question->Answer.Type] (Print,QstInd,Question);
  }

/*****************************************************************************/
/*********** Write false / true answer in an exam question sheet *************/
/*****************************************************************************/

static void ExaSheQst_WriteTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
	                           __attribute__((unused)) unsigned QstInd,
                                   __attribute__((unused)) struct Qst_Question *Question)
  {
   extern const char *Txt_TF_QST[2];

   /***** Write selector for the answer *****/
   HTM_Txt (Txt_TF_QST[0]);
   HTM_Slash ();
   HTM_Txt (Txt_TF_QST[1]);
  }

/*****************************************************************************/
/***** Write single or multiple choice answer in an exam question sheet ******/
/*****************************************************************************/

static void ExaSheQst_WriteChoAns (const struct ExaPrn_Print *Print,
                                   unsigned QstInd,
                                   struct Qst_Question *Question)
  {
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->Qsts[QstInd].StrIndexes,Indexes);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 /***** Indexes are 0 1 2 3... if no shuffle
		or 3 1 0 2... (example) if shuffle *****/
	 HTM_TR_Begin (NULL);

	    /***** Write letter of this option *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("class=\"Qst_TXT_%s\"",The_GetSuffix ());
		  HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    /***** Write the option text *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("class=\"Qst_TXT_%s\"",The_GetSuffix ());
		  HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT","Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }
