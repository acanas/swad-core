// swad_test_print.c: test prints made by users

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

#define _GNU_SOURCE 		// For asprintf
#include <float.h>		// For DBL_MAX
#include <math.h>		// For fabs
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_question.h"
#include "swad_question_choice.h"
#include "swad_question_database.h"
#include "swad_question_float.h"
#include "swad_question_int.h"
#include "swad_question_text.h"
#include "swad_question_tf.h"
#include "swad_question_type.h"
#include "swad_tag.h"
#include "swad_test.h"
#include "swad_test_database.h"
#include "swad_test_print.h"
#include "swad_test_visibility.h"
#include "swad_user.h"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct TstRes_ICanView
  {
   Usr_Can_t Result;
   Usr_Can_t Score;
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TstPrn_ResetPrintExceptPrnCod (struct TstPrn_Print *Print);

static void TstPrn_WriteQstAndAnsToFill (struct Qst_PrintedQuestion *PrintedQst,
                                         unsigned QstInd,struct Qst_Question *Qst);
static void TstPrn_WriteAnswersToFill (const struct Qst_PrintedQuestion *PrintedQst,
                                       unsigned QstInd,struct Qst_Question *Qst);

//-----------------------------------------------------------------------------

static void TstPrn_PutCheckBoxAllowTeachers (DenAll_DenyOrAllow_t DenyOrAllowTeachers);

static void TstPrn_WriteQstAndAnsExam (struct Usr_Data *UsrDat,
				       struct Qst_PrintedQuestion PrintedQsts[TstCfg_MAX_QUESTIONS_PER_TEST],
				       unsigned QstInd,
				       time_t TimeUTC[Dat_NUM_START_END_TIME],
				       struct Qst_Question *Qst,
				       Exi_Exist_t QstExists,
				       unsigned Visibility);

//-----------------------------------------------------------------------------

static void TstPrn_PutFormToSelectUsrsToViewUsrsPrints (__attribute__((unused)) void *Args);

static void TstPrn_ShowUsrsPrints (__attribute__((unused)) void *Args);
static void TstPrn_ShowHeaderPrints (Usr_MeOrOther_t MeOrOther);
static void TstPrn_ShowUsrPrints (struct Usr_Data *UsrDat);
static void TstPrn_ShowPrintsSummaryRow (Usr_MeOrOther_t MeOrOther,
                                         unsigned NumPrints,
                                         struct TstPrn_NumQuestions *NumTotalQsts,
                                         double TotalScore);

static void TstRes_CheckIfICanSeePrintResult (const struct TstPrn_Print *Print,
                                              long UsrCod,
                                              struct TstRes_ICanView *ICanView);

static void TstPrn_ShowTagsPresentInAPrint (long PrnCod);

/*****************************************************************************/
/***************************** Reset test print ******************************/
/*****************************************************************************/

void TstPrn_ResetPrint (struct TstPrn_Print *Print)
  {
   Print->PrnCod = -1L;
   TstPrn_ResetPrintExceptPrnCod (Print);
  }

static void TstPrn_ResetPrintExceptPrnCod (struct TstPrn_Print *Print)
  {
   Print->TimeUTC[Dat_STR_TIME] =
   Print->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Print->NumQsts.All      =
   Print->NumQsts.NotBlank = 0;
   Print->Sent             = false;		// After creating an exam, it's not sent
   Print->DenyOrAllowTchs  = DenAll_DENY;	// Teachers can't see the exam if student don't allow it
   Print->Score            = 0.0;
  }

/*****************************************************************************/
/********************* Show a test print to be answered **********************/
/*****************************************************************************/

void TstPrn_ShowTestPrintToFillIt (struct TstPrn_Print *Print,
                                   unsigned NumPrintsGeneratedByMe,
                                   TstPrn_RequestOrConfirm_t RequestOrConfirm)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Test;
   unsigned QstInd;
   struct Qst_Question Qst;
   static Act_Action_t Action[Tst_NUM_REQUEST_OR_CONFIRM] =
     {
      [TstPrn_REQUEST] = ActReqAssTst,
      [TstPrn_CONFIRM] = ActAssTst,
     };

   /***** Begin box *****/
   Box_BoxBegin (Txt_Test,NULL,NULL,Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);

   if (Print->NumQsts.All)
     {
      /***** Begin form *****/
      Frm_BeginForm (Action[RequestOrConfirm]);
	 ParCod_PutPar (ParCod_Prn,Print->PrnCod);
	 Par_PutParUnsigned (NULL,"NumTst",NumPrintsGeneratedByMe);

	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (10);

	    /***** Write one row for each question *****/
	    for (QstInd = 0, The_ResetRowColor ();
		 QstInd < Print->NumQsts.All;
		 QstInd++, The_ChangeRowColor ())
	      {
	       /* Create question */
	       Qst_QstConstructor (&Qst);
	       Qst.QstCod = Print->PrintedQsts[QstInd].QstCod;

		  /* Show question */
		  if (Qst_GetQstDataByCod (&Qst) == Exi_DOES_NOT_EXIST)
		     Err_WrongQuestionExit ();

		  /* Write question and answers */
		  TstPrn_WriteQstAndAnsToFill (&Print->PrintedQsts[QstInd],
					       QstInd,&Qst);

	       /* Destroy question */
	       Qst_QstDestructor (&Qst);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

         /***** Button *****/
	 switch (RequestOrConfirm)
	   {
	    case TstPrn_REQUEST:
	       /* Send button */
	       Btn_PutButton (Btn_DONE,NULL);
	       break;
	    case TstPrn_CONFIRM:
	       /* Will the test be visible by teachers? */
	       TstPrn_PutCheckBoxAllowTeachers (DenAll_ALLOW);

	       /* Send button */
	       Btn_PutButton (Btn_SEND,NULL);
	       break;
	   }

      /***** End form *****/
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void TstPrn_WriteQstAndAnsToFill (struct Qst_PrintedQuestion *PrintedQst,
                                         unsigned QstInd,struct Qst_Question *Qst)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Number of question and answer type *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Qst->Answer.Type,Qst->Validity);
      HTM_TD_End ();

      /***** Stem, media and answers *****/
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());

	 /* Write parameter with question code */
	 Qst_WriteParQstCod (QstInd,Qst->QstCod);

	 /* Stem */
	 Qst_WriteQstStem (Qst->Stem,"Qst_TXT",HidVis_VISIBLE);

	 /* Media */
	 Med_ShowMedia (&Qst->Media,"Tst_MED_SHOW_CONT","Tst_MED_SHOW");

	 /* Answers */
	 TstPrn_WriteAnswersToFill (PrintedQst,QstInd,Qst);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Write answers of a question to fill them ******************/
/*****************************************************************************/

static void TstPrn_WriteAnswersToFill (const struct Qst_PrintedQuestion *PrintedQst,
                                       unsigned QstInd,struct Qst_Question *Qst)
  {
   void (*TstPrn_WriteAnsBank[Qst_NUM_ANS_TYPES]) (const struct Qst_PrintedQuestion *PrintedQst,
						   unsigned QstInd,
						   struct Qst_Question *Qst) =
    {
     [Qst_ANS_INT            ] = QstInt_WriteTstFillAns,
     [Qst_ANS_FLOAT          ] = QstFlt_WriteTstFillAns,
     [Qst_ANS_TRUE_FALSE     ] = QstTF__WriteTstFillAns,
     [Qst_ANS_UNIQUE_CHOICE  ] = QstCho_WriteTstFillAns,
     [Qst_ANS_MULTIPLE_CHOICE] = QstCho_WriteTstFillAns,
     [Qst_ANS_TEXT           ] = QstTxt_WriteTstFillAns,
    };

   /***** Write answers *****/
   TstPrn_WriteAnsBank[Qst->Answer.Type] (PrintedQst,QstInd,Qst);
  }

/*****************************************************************************/
/**************** Put checkbox to allow teachers to see test *****************/
/*****************************************************************************/

static void TstPrn_PutCheckBoxAllowTeachers (DenAll_DenyOrAllow_t DenyOrAllowTeachers)
  {
   extern const char *Txt_Allow_teachers_to_consult_this_test;
   HTM_Attributes_t Attributes[DenAll_NUM_DENY_ALLOW] =
     {
      [DenAll_DENY ] = HTM_NO_ATTR,
      [DenAll_ALLOW] = HTM_CHECKED,	// Teachers can see test exam
     };

   /***** Test exam will be available for teachers? *****/
   HTM_DIV_Begin ("class=\"CM\"");
      HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	 HTM_INPUT_CHECKBOX ("AllowTchs",Attributes[DenyOrAllowTeachers],
			     "value=\"Y\"");
	 HTM_Txt (Txt_Allow_teachers_to_consult_this_test);
      HTM_LABEL_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************ Show test after assessing it ***********************/
/*****************************************************************************/

void TstPrn_ShowPrintAfterAssess (struct TstPrn_Print *Print)
  {
   unsigned QstInd;
   struct Qst_Question Qst;
   Exi_Exist_t QuestionExists;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

      /***** Initialize score and number of questions not blank *****/
      Print->NumQsts.NotBlank = 0;
      Print->Score = 0.0;

      for (QstInd = 0, The_ResetRowColor ();
	   QstInd < Print->NumQsts.All;
	   QstInd++, The_ChangeRowColor ())
	{
	 /***** Create question *****/
	 Qst_QstConstructor (&Qst);
	 Qst.QstCod = Print->PrintedQsts[QstInd].QstCod;

	    /***** Get question data *****/
	    QuestionExists = Qst_GetQstDataByCod (&Qst);

	    /***** Write question and answers *****/
	    TstPrn_WriteQstAndAnsExam (&Gbl.Usrs.Me.UsrDat,
				       Print->PrintedQsts,QstInd,
				       Print->TimeUTC,
				       &Qst,QuestionExists,
				       TstCfg_GetConfigVisibility ());

	 /***** Destroy question *****/
	 Qst_QstDestructor (&Qst);

	 /***** Store test question in database *****/
	 Tst_DB_StoreOneQstOfPrint (Print,QstInd);

	 /***** Compute total score *****/
	 Print->Score += Print->PrintedQsts[QstInd].Answer.Score;
	 if (Print->PrintedQsts[QstInd].Answer.Str[0])	// User's answer is not blank
	    Print->NumQsts.NotBlank++;

	 /***** Update the number of accesses and the score of this question *****/
	 if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
	    Qst_DB_UpdateQstScore (Print,QstInd);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void TstPrn_WriteQstAndAnsExam (struct Usr_Data *UsrDat,
				       struct Qst_PrintedQuestion PrintedQsts[TstCfg_MAX_QUESTIONS_PER_TEST],
				       unsigned QstInd,
				       time_t TimeUTC[Dat_NUM_START_END_TIME],
				       struct Qst_Question *Qst,
				       Exi_Exist_t QstExists,
				       unsigned Visibility)
  {
   extern const char *Txt_Score;
   extern const char *Txt_Question_removed;
   extern const char *Txt_Question_modified;
   static HidVis_HiddenOrVisible_t HiddenOrVisible[Usr_NUM_CAN] =
     {
      [Usr_CAN_NOT] = HidVis_HIDDEN,
      [Usr_CAN    ] = HidVis_VISIBLE,
     };
   bool QuestionUneditedAfterExam = false;
   Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY];

   /***** Check if I can view each part of the question *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] = TstVis_StudentsCanViewQstAndAnsTxt (Visibility);
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] = TstVis_StudentsCanViewFeedbackTxt  (Visibility);
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] = TstVis_StudentsCanViewCorrectAns   (Visibility);
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = TstVis_StudentsCanViewEachQstScore (Visibility);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] =
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] =
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] =
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = Usr_CAN;
	 break;
      default:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] =
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] =
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] =
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = Usr_CAN_NOT;
	 break;
     }

   /***** If this question has been edited later than test time
	  ==> don't show question ****/
   switch (QstExists)
     {
      case Exi_EXISTS:
	 QuestionUneditedAfterExam = (Qst->EditTime < TimeUTC[Dat_STR_TIME]);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 QuestionUneditedAfterExam = false;
	 break;
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Number of question and answer type *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 if (QuestionUneditedAfterExam)
	    Qst_WriteAnswerType (Qst->Answer.Type,Qst->Validity);
      HTM_TD_End ();

      /***** Stem, media and answers *****/
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	 switch (QstExists)
	   {
	    case Exi_EXISTS:
	       if (QuestionUneditedAfterExam)
		 {
		  /* Stem */
		  Qst_WriteQstStem (Qst->Stem,"Qst_TXT",
				    HiddenOrVisible[ICanView[TstVis_VISIBLE_QST_ANS_TXT]]);

		  /* Media */
		  if (ICanView[TstVis_VISIBLE_QST_ANS_TXT] == Usr_CAN)
		     Med_ShowMedia (&Qst->Media,
				    "Tst_MED_SHOW_CONT","Tst_MED_SHOW");

		  /* Answers */
		  Qst_ComputeAnswerScore ("tst_answers",&PrintedQsts[QstInd],Qst);
		  TstPrn_WriteAnswersExam (&PrintedQsts[QstInd],Qst,
					   ICanView,"Qst_TXT","Qst_TXT_LIGHT",
					   UsrDat);

		  /* Write score retrieved from database */
		  if (ICanView[TstVis_VISIBLE_EACH_QST_SCORE] == Usr_CAN)
		    {
		     HTM_DIV_Begin ("class=\"LM DAT_SMALL_%s\"",The_GetSuffix ());
			HTM_Txt (Txt_Score); HTM_Colon (); HTM_NBSP ();
			HTM_SPAN_Begin ("class=\"%s_%s\"",
					PrintedQsts[QstInd].Answer.Str[0] ?
					(PrintedQsts[QstInd].Answer.Score > 0 ? "Qst_ANS_OK" :	// Correct
										     "Qst_ANS_BAD") :	// Wrong
										     "Qst_ANS_0",	// Blank answer
					The_GetSuffix ());
			   HTM_Double2Decimals (PrintedQsts[QstInd].Answer.Score);
			HTM_SPAN_End ();
		     HTM_DIV_End ();
		    }
		 }
	       else
		  Ale_ShowAlert (Ale_WARNING,Txt_Question_modified);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);
	       break;
	   }

	 /* Question feedback */
	 if (QuestionUneditedAfterExam)
	    if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT] == Usr_CAN)
	       Qst_WriteQstFeedback (Qst->Feedback,"Qst_TXT_LIGHT");

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******** Get questions and answers from form to assess a test print *********/
/*****************************************************************************/

void TstPrn_GetAnswersFromForm (struct TstPrn_Print *Print)
  {
   unsigned QstInd;
   char StrAns[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Loop for every question getting user's answers *****/
   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      /* Get answers selected by user for this question */
      snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
      Par_GetParMultiToText (StrAns,Print->PrintedQsts[QstInd].Answer.Str,
                             Qst_MAX_BYTES_ANSWERS_ONE_QST);  /* If answer type == T/F ==> " ", "T", "F"; if choice ==> "0", "2",... */
      Par_ReplaceSeparatorMultipleByComma (Print->PrintedQsts[QstInd].Answer.Str);
     }
  }

/*****************************************************************************/
/*********** Compute score of each question and store in database ************/
/*****************************************************************************/

void TstPrn_ComputeScoresAndStoreQuestionsOfPrint (struct TstPrn_Print *Print)
  {
   unsigned QstInd;
   struct Qst_Question Qst;

   /***** Initialize total score *****/
   Print->Score = 0.0;
   Print->NumQsts.NotBlank = 0;

   /***** Compute and store scores of all questions *****/
   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      /* Create question */
      Qst_QstConstructor (&Qst);
      Qst.QstCod = Print->PrintedQsts[QstInd].QstCod;
      Qst.Answer.Type = Qst_DB_GetQstAnswerType (Qst.QstCod);

	 /* Compute question score */
	 Qst_ComputeAnswerScore ("tst_answers",&Print->PrintedQsts[QstInd],&Qst);

      /* Destroy question */
      Qst_QstDestructor (&Qst);

      /* Store test question in database */
      Tst_DB_StoreOneQstOfPrint (Print,
				 QstInd);	// 0, 1, 2, 3...

      /* Accumulate total score */
      Print->Score += Print->PrintedQsts[QstInd].Answer.Score;
      if (Print->PrintedQsts[QstInd].Answer.Str[0])	// User's answer is not blank
	 Print->NumQsts.NotBlank++;

      /* Update the number of hits and the score of this question in tests database */
      if (Print->Sent &&			// Print just sent to be assessed
	  Gbl.Usrs.Me.Role.Logged == Rol_STD)	// I am a student
	 Qst_DB_UpdateQstScore (Print,QstInd);
     }
  }

/*****************************************************************************/
/************* Write answers of a question when assessing a test *************/
/*****************************************************************************/

void TstPrn_WriteAnswersExam (const struct Qst_PrintedQuestion *PrintedQst,
			      struct Qst_Question *Qst,
			      Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			      const char *ClassTxt,
			      const char *ClassFeedback,
			      struct Usr_Data *UsrDat)
  {
   void (*TstPrn_WritePrntAns[Qst_NUM_ANS_TYPES]) (const struct Qst_PrintedQuestion *PrintedQst,
						   struct Qst_Question *Qst,
						   Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
						   const char *ClassTxt,
						   const char *ClassFeedback) =
    {
     [Qst_ANS_INT            ] = QstInt_WriteTstPrntAns,
     [Qst_ANS_FLOAT          ] = QstFlt_WriteTstPrntAns,
     [Qst_ANS_TRUE_FALSE     ] = QstTF__WriteTstPrntAns,
     [Qst_ANS_UNIQUE_CHOICE  ] = QstCho_WriteTstPrntAns,
     [Qst_ANS_MULTIPLE_CHOICE] = QstCho_WriteTstPrntAns,
     [Qst_ANS_TEXT           ] = QstTxt_WriteTstPrntAns,
    };

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      /***** Heading *****/
      HTM_TR_Begin (NULL);
	 TstPrn_WriteHeadUserCorrect (UsrDat);
	 switch (Qst->Answer.Type)
	   {
	    case Qst_ANS_UNIQUE_CHOICE:
	    case Qst_ANS_MULTIPLE_CHOICE:
	       HTM_TD_Empty (2);
	       break;
	    default:
	       break;
	   }
      HTM_TR_End ();

      /***** Write student and correct answers *****/
      TstPrn_WritePrntAns[Qst->Answer.Type] (PrintedQst,Qst,
						  ICanView,ClassTxt,ClassFeedback);

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********* Write head with two columns:                               ********/
/********* one for the user's answer and other for the correct answer ********/
/*****************************************************************************/

void TstPrn_WriteHeadUserCorrect (const struct Usr_Data *UsrDat)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   HTM_TD_Begin ("class=\"CM DAT_SMALL_%s\"",The_GetSuffix ());
      HTM_Txt (Txt_ROLES_SINGUL_Abc[UsrDat->Roles.InCurrentCrs][UsrDat->Sex]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM DAT_SMALL_%s\"",The_GetSuffix ());
      HTM_Txt (Txt_ROLES_PLURAL_Abc[Rol_TCH][Usr_SEX_UNKNOWN]);
   HTM_TD_End ();
  }

/*****************************************************************************/
/*************** Select users and dates to show their tests ******************/
/*****************************************************************************/

void TstPrn_SelUsrsToViewUsrsPrints (void)
  {
   TstPrn_PutFormToSelectUsrsToViewUsrsPrints (NULL);
  }

static void TstPrn_PutFormToSelectUsrsToViewUsrsPrints (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeUsrTstResCrs,
				     NULL,NULL,
				     Act_GetActionText (ActSeeUsrTstResCrs),
				     Hlp_ASSESSMENT_Tests_results,
				     Btn_VIEW_RESULTS,
				     Frm_PUT_FORM);	// Put form with date range
  }

/*****************************************************************************/
/*********************** Select dates to show my tests ***********************/
/*****************************************************************************/

void TstPrn_SelDatesToSeeMyPrints (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   static Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Begin form *****/
   Frm_BeginForm (ActSeeMyTstResCrs);

      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Results,NULL,NULL,
			 Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);

	 Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonEnd (Btn_VIEW_RESULTS);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************************* Show my tests *******************************/
/*****************************************************************************/

void TstPrn_ShowMyPrints (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_Results,NULL,NULL,
                      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);

      /***** Header of the table with the list of users *****/
      TstPrn_ShowHeaderPrints (Usr_ME);

      /***** List my tests *****/
      TstCfg_GetConfig ();	// To get visibility
      TstPrn_ShowUsrPrints (&Gbl.Usrs.Me.UsrDat);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/********************** Get users and show their test ************************/
/*****************************************************************************/

void TstPrn_GetUsrsAndShowPrints (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  TstPrn_ShowUsrsPrints,NULL,
                                  TstPrn_PutFormToSelectUsrsToViewUsrsPrints,NULL);
  }

/*****************************************************************************/
/********************* Show test prints for several users ********************/
/*****************************************************************************/

static void TstPrn_ShowUsrsPrints (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   const char *Ptr;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_Results,NULL,NULL,
		      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,5);

      /***** Header of the table with the list of users *****/
      TstPrn_ShowHeaderPrints (Usr_OTHER);

      /***** List the tests of the selected users *****/
      for (Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
           *Ptr;
          )
	{
	 Par_GetNextStrUntilSeparParMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					  Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	 Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
						      Usr_DONT_GET_PREFS,
						      Usr_GET_ROLE_IN_CRS) == Exi_EXISTS)
	    if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat) == Usr_CAN)
	      {
	       /***** Show tests *****/
	       Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	       TstPrn_ShowUsrPrints (&Gbl.Usrs.Other.UsrDat);
	      }
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/************************** Show header of my tests **************************/
/*****************************************************************************/

static void TstPrn_ShowHeaderPrints (Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Grade;
   extern const char *Txt_Questions;
   extern const char *Txt_Answers;
   extern const char *Txt_Score;
   extern const char *Txt_ANSWERS_non_blank;
   extern const char *Txt_ANSWERS_blank;
   extern const char *Txt_total;
   extern const char *Txt_average;

   /***** First row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_User[MeOrOther == Usr_ME ?
	                       Gbl.Usrs.Me.UsrDat.Sex :
		  	       Usr_SEX_UNKNOWN         ],HTM_HEAD_CENTER,3,2,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_STR_TIME]     ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_END_TIME]     ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (NULL                                 ,HTM_HEAD_CENTER,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_Grade                            ,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_Score                            ,HTM_HEAD_CENTER,1,2,"LINE_LEFT");
      HTM_TH_Span (Txt_Questions                        ,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span (Txt_Answers                          ,HTM_HEAD_CENTER,1,2,"LINE_LEFT");
   HTM_TR_End ();

   /***** Second row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_total                            ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH      (Txt_average                          ,HTM_HEAD_RIGHT );
      HTM_TH_Span (Txt_ANSWERS_non_blank                ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH      (Txt_ANSWERS_blank                    ,HTM_HEAD_RIGHT );
   HTM_TR_End ();

   /***** Third row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span ("<em>&Sigma;p<sub>i</sub></em>"      ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("-1&le;"
	           "<em style=\"text-decoration:overline;\">p</em>"
	           "&le;1"                              ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
      HTM_TH_Span ("{-1&le;<em>p<sub>i</sub></em>&le;1}",HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("{<em>p<sub>i</sub></em>=0}"         ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show the test prints of a user in the current course ***********/
/*****************************************************************************/

static void TstPrn_ShowUsrPrints (struct Usr_Data *UsrDat)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPrints;
   unsigned NumPrint;
   static unsigned UniqueId = 0;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstPrn_Print Print;
   unsigned NumQstsBlank;
   struct TstPrn_NumQuestions NumTotalQsts;
   double TotalScore;
   unsigned NumPrintsVisibleByTchs = 0;
   struct TstRes_ICanView ICanView;
   static const char *ClassDat[DenAll_NUM_DENY_ALLOW] =
     {
      [DenAll_DENY ] = "DAT_LIGHT",
      [DenAll_ALLOW] = "DAT"
     };

   /***** Reset total number of questions and total score *****/
   NumTotalQsts.All      =
   NumTotalQsts.NotBlank = 0;
   TotalScore = 0.0;

   /***** Make database query *****/
   NumPrints = Tst_DB_GetUsrPrintsInCurrentCrs (&mysql_res,UsrDat->UsrCod);

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);

      Usr_ShowTableCellWithUsrData (UsrDat,NumPrints + 1);

      /***** Get and print tests *****/
      if (NumPrints)
	{
	 for (NumPrint = 0;
	      NumPrint < NumPrints;
	      NumPrint++)
	   {
	    /* Get print code (row[0]) */
	    if ((Print.PrnCod = DB_GetNextCode (mysql_res)) <= 0)
	       Err_WrongTestExit ();

	    /* Get print data */
	    TstPrn_GetPrintDataByPrnCod (&Print);

	    /* Get if I can see print result and score */
	    TstRes_CheckIfICanSeePrintResult (&Print,UsrDat->UsrCod,&ICanView);

	    if (NumPrint)
	       HTM_TR_Begin (NULL);

	    /* Write dates and times */
	    UniqueId++;
	    for (StartEndTime  = (Dat_StartEndTime_t) 0;
		 StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
		 StartEndTime++)
	      {
	       if (asprintf (&Id,"tst_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			     Id,ClassDat[Print.DenyOrAllowTchs],The_GetSuffix (),
			     The_GetColorRows ());
		  Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
						Dat_WRITE_TODAY |
						Dat_WRITE_DATE_ON_SAME_DAY |
						Dat_WRITE_HOUR |
						Dat_WRITE_MINUTE |
						Dat_WRITE_SECOND);
	       HTM_TD_End ();
	       free (Id);
	      }

	    /* Link to show this test */
	    HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());
	       switch (ICanView.Result)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (Gbl.Action.Act == ActSeeMyTstResCrs ? ActSeeOneTstResMe :
									  ActSeeOneTstResOth);
			ParCod_PutPar (ParCod_Prn,Print.PrnCod);
			Ico_PutIconLink ("tasks.svg",Ico_BLACK,
					 Gbl.Action.Act == ActSeeMyTstResCrs ? ActSeeOneTstResMe :
									       ActSeeOneTstResOth);
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Accumulate questions and score */
	    if (ICanView.Score == Usr_CAN)
	      {
	       NumTotalQsts.All      += Print.NumQsts.All;
	       NumTotalQsts.NotBlank += Print.NumQsts.NotBlank;
	       TotalScore            += Print.Score;
	      }

	    /* Write grade */
	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
	                  ClassDat[Print.DenyOrAllowTchs],The_GetSuffix (),
	                  The_GetColorRows ());
	       switch (ICanView.Score)
		 {
		  case Usr_CAN:
		     Qst_ComputeAndShowGrade (Print.NumQsts.All,Print.Score,
						 Tst_SCORE_MAX);
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Write score */
	    HTM_TD_Begin ("class=\"RT %s_%s LINE_LEFT %s\"",
	                  ClassDat[Print.DenyOrAllowTchs],The_GetSuffix (),
	                  The_GetColorRows ());
	       switch (ICanView.Score)
		 {
		  case Usr_CAN:
		     HTM_DoublePartOfUnsigned (Print.Score,Print.NumQsts.All);
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Write average score per question */
	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
	                  ClassDat[Print.DenyOrAllowTchs],The_GetSuffix (),
	                  The_GetColorRows ());
	       switch (ICanView.Score)
		 {
		  case Usr_CAN:
		     HTM_Double2Decimals (Print.NumQsts.All ? Print.Score /
							      (double) Print.NumQsts.All :
							      0.0);
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Write number of questions */
	    HTM_TD_Begin ("class=\"RT %s_%s LINE_LEFT %s\"",
	                  ClassDat[Print.DenyOrAllowTchs],The_GetSuffix (),
	                  The_GetColorRows ());
	       switch (ICanView.Result)
		 {
		  case Usr_CAN:
		     HTM_Unsigned (Print.NumQsts.All);
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Write number of non-blank answers */
	    HTM_TD_Begin ("class=\"RT %s_%s LINE_LEFT %s\"",
	                  ClassDat[Print.DenyOrAllowTchs],The_GetSuffix (),
	                  The_GetColorRows ());
	       switch (ICanView.Result)
		 {
		  case Usr_CAN:
		     HTM_UnsignedLight0 (Print.NumQsts.NotBlank);
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Write number of blank answers */
	    HTM_TD_Begin ("class=\"RT %s_%s %s\"",
	                  ClassDat[Print.DenyOrAllowTchs],The_GetSuffix (),
	                  The_GetColorRows ());
	       switch (ICanView.Result)
		 {
		  case Usr_CAN:
		     NumQstsBlank = Print.NumQsts.All - Print.NumQsts.NotBlank;
		     HTM_UnsignedLight0 (NumQstsBlank);
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	    HTM_TR_End ();

	    if (Print.DenyOrAllowTchs == DenAll_ALLOW)
	       NumPrintsVisibleByTchs++;
	   }

	 /***** Write totals for this user *****/
	 TstPrn_ShowPrintsSummaryRow (Usr_ItsMe (UsrDat->UsrCod),
	                              NumPrintsVisibleByTchs,&NumTotalQsts,
	                              TotalScore);
	}
      else
	{
	 /* Columns for dates */
	 HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 /* Column for link to show the result */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM %s\"",The_GetColorRows ());
	 HTM_TD_End ();

	 /* Column for grade */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM %s\"",The_GetColorRows ());
	 HTM_TD_End ();

	 /* Columns for score */
	 HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 /* Column for questions */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",The_GetColorRows ());
	 HTM_TD_End ();

	 /* Columns for answers */
	 HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
	               The_GetColorRows ());
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/****************** Show row with summary of user's tess *********************/
/*****************************************************************************/

static void TstPrn_ShowPrintsSummaryRow (Usr_MeOrOther_t MeOrOther,
                                         unsigned NumPrints,
                                         struct TstPrn_NumQuestions *NumTotalQsts,
                                         double TotalScore)
  {
   extern const char *Txt_Visible_tests;
   Usr_Can_t ICanViewTotalScore;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 switch (MeOrOther)
	   {
	    case Usr_ME:
	       ICanViewTotalScore = TstVis_StudentsCanViewTotalScore (TstCfg_GetConfigVisibility ());
	       break;
	    case Usr_OTHER:
	    default:
	       ICanViewTotalScore = Usr_CAN_NOT;
	       break;
	   }
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 switch (MeOrOther)
	   {
	    case Usr_ME:
	       ICanViewTotalScore = Usr_CAN;
	       break;
	    case Usr_OTHER:
	    default:
	       ICanViewTotalScore = NumPrints ? Usr_CAN :
		        		        Usr_CAN_NOT;
	       break;
	   }
	 break;
      case Rol_SYS_ADM:
	 ICanViewTotalScore = Usr_CAN;
	 break;
      default:
	 ICanViewTotalScore = Usr_CAN_NOT;
	 break;
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Row title *****/
      HTM_TD_Begin ("colspan=\"2\""
	            " class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 HTM_Txt (Txt_Visible_tests); HTM_Colon ();
      HTM_TD_End ();

      /***** Cell for links to show results *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 HTM_Unsigned (NumPrints);
      HTM_TD_End ();

      /***** Write grade over Tst_SCORE_MAX *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 if (ICanViewTotalScore == Usr_CAN)
	    Qst_ComputeAndShowGrade (NumTotalQsts->All,TotalScore,Tst_SCORE_MAX);
      HTM_TD_End ();

      /***** Write total score *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 if (ICanViewTotalScore == Usr_CAN)
	    HTM_DoublePartOfUnsigned (TotalScore,NumTotalQsts->All);
      HTM_TD_End ();

      /***** Write average score per question *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 if (ICanViewTotalScore == Usr_CAN)
	    HTM_Double2Decimals (NumTotalQsts->All ? TotalScore /
						     (double) NumTotalQsts->All :
						     0.0);
      HTM_TD_End ();

      /***** Write total number of questions *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 if (NumPrints)
	    HTM_Unsigned (NumTotalQsts->All);
      HTM_TD_End ();

      /***** Write total number of non-blank answers *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 if (NumPrints)
	    HTM_Unsigned (NumTotalQsts->NotBlank);
      HTM_TD_End ();

      /***** Write total number of blank answers *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 if (NumPrints)
	    HTM_Unsigned (NumTotalQsts->All - NumTotalQsts->NotBlank);
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********************** Show one test of another user ***********************/
/*****************************************************************************/

void TstPrn_ShowOnePrint (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Result;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_Tags;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };
   struct TstPrn_Print Print;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstRes_ICanView ICanView;

   /***** Get the code of the test *****/
   TstPrn_ResetPrint (&Print);
   Print.PrnCod = ParCod_GetAndCheckPar (ParCod_Prn);

   /***** Get test data *****/
   TstPrn_GetPrintDataByPrnCod (&Print);

   /***** Check if I can see print result and score *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
      TstCfg_GetConfig ();	// To get visibility
   TstRes_CheckIfICanSeePrintResult (&Print,Gbl.Usrs.Other.UsrDat.UsrCod,&ICanView);
   if (ICanView.Result == Usr_CAN_NOT)	// I am not allowed to view this test
      Err_NoPermissionExit ();

   /***** Get questions and user's answers of the test from database *****/
   if (TstPrn_GetPrintQuestionsFromDB (&Print) == Err_ERROR)
      Err_WrongExamExit ();

   /***** Begin box *****/
   Box_BoxBegin (Txt_Result,NULL,NULL,
	         Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

	 /***** User *****/
	 /* Get data of the user who made the test */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
						      Usr_DONT_GET_PREFS,
						      Usr_GET_ROLE_IN_CRS) == Exi_DOES_NOT_EXIST)
	    Err_WrongUserExit ();
	 if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat) == Usr_CAN_NOT)
	    Err_NoPermissionExit ();

	 /* Get if user has accepted enrolment */
	 Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);

	 /* User */
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_Txt (Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs][Gbl.Usrs.Other.UsrDat.Sex]);
	       HTM_Colon ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	       ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
	       HTM_SP ();
	       HTM_Txt (Gbl.Usrs.Other.UsrDat.Surname1);
	       if (Gbl.Usrs.Other.UsrDat.Surname2[0])
	         {
		  HTM_SP ();
		  HTM_Txt (Gbl.Usrs.Other.UsrDat.Surname2);
	         }
	       if (Gbl.Usrs.Other.UsrDat.FrstName[0])
	         {
		  HTM_Comma ();
		  HTM_SP ();
		  HTM_Txt (Gbl.Usrs.Other.UsrDat.FrstName);
	         }
	       HTM_BR ();
	       Pho_ShowUsrPhotoIfAllowed (&Gbl.Usrs.Other.UsrDat,
					  ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Start/end time (for user in this test print) *****/
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    if (asprintf (&Id,"tst_date_%u",(unsigned) StartEndTime) < 0)
	       Err_NotEnoughMemoryExit ();

	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",The_GetSuffix ());
		  HTM_Txt (Txt_START_END_TIME[StartEndTime]); HTM_Colon ();
	       HTM_TD_End ();

	       HTM_TD_Begin ("id=\"%s\" class=\"LT DAT_%s\"",Id,The_GetSuffix ());
		  Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
						Dat_WRITE_TODAY |
						Dat_WRITE_DATE_ON_SAME_DAY |
						Dat_WRITE_WEEK_DAY |
						Dat_WRITE_HOUR |
						Dat_WRITE_MINUTE |
						Dat_WRITE_SECOND);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    free (Id);
	   }

	 /***** Number of questions *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_Questions); HTM_Colon ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (Print.NumQsts.All);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Number of answers *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_Answers); HTM_Colon ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (Print.NumQsts.NotBlank);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Score *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_Score); HTM_Colon ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	       switch (ICanView.Score)
		 {
		  case Usr_CAN:
		     HTM_STRONG_Begin ();
			HTM_DoublePartOfUnsigned (Print.Score,Print.NumQsts.All);
		     HTM_STRONG_End ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Grade *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_Grade); HTM_Colon ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	       switch (ICanView.Score)
		 {
		  case Usr_CAN:
		     HTM_STRONG_Begin ();
			Qst_ComputeAndShowGrade (Print.NumQsts.All,Print.Score,
						    Tst_SCORE_MAX);
		     HTM_STRONG_End ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Tags present in this test *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_Tags); HTM_Colon ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	       TstPrn_ShowTagsPresentInAPrint (Print.PrnCod);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Write answers and solutions *****/
	 TstPrn_ShowPrintAnswers (&Gbl.Usrs.Other.UsrDat,
				  Print.NumQsts.All,
				  Print.PrintedQsts,
				  Print.TimeUTC,
				  TstCfg_GetConfigVisibility ());

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****************** Get if I can see print result and score ******************/
/*****************************************************************************/

static void TstRes_CheckIfICanSeePrintResult (const struct TstPrn_Print *Print,
                                              long UsrCod,
                                              struct TstRes_ICanView *ICanView)
  {
   /***** Check if I can view print result and score *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 // Depends on whether the print is sent or not
	 // if the print is not sent ==> I can not view results
	 ICanView->Result = Print->Sent &&
			    Usr_ItsMe (UsrCod) == Usr_ME ? Usr_CAN :
							   Usr_CAN_NOT;
	 switch (ICanView->Result)
	   {
	    case Usr_CAN:
	       // Depends on 5 visibility icons associated to tests
	       ICanView->Score = TstVis_StudentsCanViewTotalScore (TstCfg_GetConfigVisibility ());
	       break;
	    case Usr_CAN_NOT:
	    default:
	       ICanView->Score = Usr_CAN_NOT;
	       break;
	   }
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 // Depends on whether the print is sent or not, and whether teachers are allowed
	 // if the print is not sent ==> I can not view results
	 // if teachers are not allowed ==> I can not view results (except if the print is mine)
	 ICanView->Result =
	 ICanView->Score  = Print->Sent &&
	                    (Print->DenyOrAllowTchs == DenAll_ALLOW ||
	                     Usr_ItsMe (UsrCod) == Usr_ME) ? Usr_CAN :
							     Usr_CAN_NOT;
	 break;
      case Rol_SYS_ADM:
	 ICanView->Result =
	 ICanView->Score  = Usr_CAN;
	 break;
      default:
	 ICanView->Result =
	 ICanView->Score  = Usr_CAN_NOT;
	 break;
     }
  }

/*****************************************************************************/
/************************ Show test tags in this test ************************/
/*****************************************************************************/

static void TstPrn_ShowTagsPresentInAPrint (long PrnCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumTags;

   /***** Get all tags of questions in this test print *****/
   NumTags = Tst_DB_GetTagsPresentInAPrint (&mysql_res,PrnCod);
   Tag_ShowTagList (NumTags,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** Show user's and correct answers of a test ******************/
/*****************************************************************************/

void TstPrn_ShowPrintAnswers (struct Usr_Data *UsrDat,
			      unsigned NumQsts,
			      struct Qst_PrintedQuestion PrintedQsts[TstCfg_MAX_QUESTIONS_PER_TEST],
			      time_t TimeUTC[Dat_NUM_START_END_TIME],
			      unsigned Visibility)
  {
   unsigned QstInd;
   struct Qst_Question Qst;
   Exi_Exist_t QuestionExists;

   for (QstInd = 0, The_ResetRowColor ();
	QstInd < NumQsts;
	QstInd++, The_ChangeRowColor ())
     {
      /***** Create question *****/
      Qst_QstConstructor (&Qst);
      Qst.QstCod = PrintedQsts[QstInd].QstCod;

	 /***** Get question data *****/
	 QuestionExists = Qst_GetQstDataByCod (&Qst);

	 /***** Write questions and answers *****/
	 TstPrn_WriteQstAndAnsExam (UsrDat,
				    PrintedQsts,QstInd,
				    TimeUTC,
				    &Qst,QuestionExists,
				    Visibility);

      /***** Destroy question *****/
      Qst_QstDestructor (&Qst);
     }
  }

/*****************************************************************************/
/**************** Get data of a test using its test code *********************/
/*****************************************************************************/

void TstPrn_GetPrintDataByPrnCod (struct TstPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Make database query *****/
   switch (Tst_DB_GetPrintDataByPrnCod (&mysql_res,Print->PrnCod))
     {
      case Exi_EXISTS:
	 row = mysql_fetch_row (mysql_res);

	 /* Get user code (row[0]) */
	 Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get date-time (row[1] and row[2] hold UTC date-time) */
	 Print->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
	 Print->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[2]);

	 /* Get number of questions (row[3])
	    and number of questions not blank (row[4]) */
	 if (sscanf (row[3],"%u",&Print->NumQsts.All     ) != 1)
	    Print->NumQsts.All      = 0;
	 if (sscanf (row[4],"%u",&Print->NumQsts.NotBlank) != 1)
	    Print->NumQsts.NotBlank = 0;

	 /* Get if print has been sent (row[5])
	    and if teachers are allowed to see this test print (row[6]) */
	 Print->Sent            = (row[5][0] == 'Y');
	 Print->DenyOrAllowTchs = DenAll_GetAllowFromYN (row[6][0]);

	 /* Get score (row[7]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
	 if (sscanf (row[7],"%lf",&Print->Score) != 1)
	    Print->Score = 0.0;
	 Str_SetDecimalPointToLocal ();	// Return to local system
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 TstPrn_ResetPrintExceptPrnCod (Print);
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get the questions of a test print from database ***************/
/*****************************************************************************/

Err_SuccessOrError_t TstPrn_GetPrintQuestionsFromDB (struct TstPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned QstInd;

   /***** Get questions of a test print from database *****/
   NumQsts = Tst_DB_GetPrintQuestions (&mysql_res,Print->PrnCod);

   /***** Get questions *****/
   if (NumQsts == Print->NumQsts.All)
      for (QstInd = 0;
	   QstInd < NumQsts;
	   QstInd++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get question code (row[0]) */
	 if ((Print->PrintedQsts[QstInd].QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Err_WrongQuestionExit ();

	 /* Get score (row[1]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
         if (sscanf (row[1],"%lf",&Print->PrintedQsts[QstInd].Answer.Score) != 1)
            Err_ShowErrorAndExit ("Wrong question score.");
         Str_SetDecimalPointToLocal ();	// Return to local system

	 /* Get indexes for this question (row[2])
	    and answers selected by user for this question (row[3]) */
	 Str_Copy (Print->PrintedQsts[QstInd].StrIndexes,row[2],
		   sizeof (Print->PrintedQsts[QstInd].StrIndexes) - 1);
	 Str_Copy (Print->PrintedQsts[QstInd].Answer.Str,row[3],
		   sizeof (Print->PrintedQsts[QstInd].Answer.Str) - 1);
	}

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumQsts == Print->NumQsts.All ? Err_SUCCESS :
					  Err_ERROR;
  }

/*****************************************************************************/
/********************** Remove test prints made by a user ********************/
/*****************************************************************************/

void TstPrn_RemovePrintsMadeByUsrInAllCrss (long UsrCod)
  {
   /***** Remove test prints questions for the given user *****/
   Tst_DB_RemovePrintQuestionsMadeByUsrInAllCrss (UsrCod);

   /***** Remove test prints made by the given user *****/
   Tst_DB_RemovePrintsMadeByUsrInAllCrss (UsrCod);
  }

/*****************************************************************************/
/************** Remove test prints made by a user in a course ****************/
/*****************************************************************************/

void TstPrn_RemovePrintsMadeByUsrInCrs (long UsrCod,long HieCod)
  {
   /***** Remove test prints questions for the given user *****/
   Tst_DB_RemovePrintQuestionsMadeByUsrInCrs (UsrCod,HieCod);

   /***** Remove test prints made by the given user *****/
   Tst_DB_RemovePrintsMadeByUsrInCrs (UsrCod,HieCod);
  }

/*****************************************************************************/
/****************** Remove all test prints made in a course ******************/
/*****************************************************************************/

void TstPrn_RemoveCrsPrints (long HieCod)
  {
   /***** Remove questions of tests made in the course *****/
   Tst_DB_RemovePrintQuestionsMadeInCrs (HieCod);

   /***** Remove tests made in the course *****/
   Tst_DB_RemovePrintsMadeByInCrs (HieCod);
  }

/*****************************************************************************/
/***************** Get number of test prints generated by me *****************/
/*****************************************************************************/

unsigned TstPrn_GetNumPrintsGeneratedByMe (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   unsigned NumPrintsGeneratedByMe = 0;

   if (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_BELONG)
     {
      /***** Get number of test prints generated by me from database *****/
      NumRows = Tst_DB_GetNumPrintsGeneratedByMe (&mysql_res);

      if (NumRows == 0)
         NumPrintsGeneratedByMe = 0;
      else if (NumRows == 1)
        {
         /* Get number of hits */
         row = mysql_fetch_row (mysql_res);
         if (row[0] == NULL)
            NumPrintsGeneratedByMe = 0;
         else if (sscanf (row[0],"%u",&NumPrintsGeneratedByMe) != 1)
            NumPrintsGeneratedByMe = 0;
        }
      else
         Err_ShowErrorAndExit ("Error when getting number of tests.");

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return NumPrintsGeneratedByMe;
  }
