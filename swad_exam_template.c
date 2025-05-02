// swad_exam_print.c: exam prints (each copy of an exam in a session for a student)

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

#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam.h"
#include "swad_exam_database.h"
#include "swad_exam_log.h"
#include "swad_exam_print.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_view.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************** Private types ******************************/
/*****************************************************************************/

#define ExaPrn_TYPES_OF_VIEW 2
typedef enum
  {
   ExaPrn_VIEW_SEL_USR,		// View selected users
   ExaPrn_PRNT_SEL_USR,		// Print selected users
  } ExaPrn_TypeOfView_t;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

//-----------------------------------------------------------------------------

static void ExaTmp_ListOrPrintExaTmps (Vie_ViewType_t ViewType);
static void ExaTmp_PutIconsPrintExaTmps (void *Exams);
static void ExaTmp_PutParsToPrintExaTmps (void *Exams);
static void ExaTmp_ShowMultipleExaTmps (struct Exa_Exams *Exams,
				        const struct ExaSes_Session *Session,
					Vie_ViewType_t ViewType,
					unsigned NumUsrsInList,
					long *LstSelectedUsrCods);
static void ExaTmp_ShowTemplateToTch (struct Exa_Exams *Exams,
      				      const struct ExaSes_Session *Session,
				      Vie_ViewType_t ViewType,
				      struct Usr_Data *UsrDat);

static void ExaTmp_ShowTemplateWithSolutions (struct Exa_Exams *Exams,
					      const struct ExaSes_Session *Session,
					      Vie_ViewType_t ViewType,
					      struct Usr_Data *UsrDat,
					      struct ExaPrn_Print *Print);
static void ExaTmp_ShowTableTemplateWithSolutions (const struct Usr_Data *UsrDat,
						   const struct ExaPrn_Print *Print);

static void ExaTmp_WriteQst (const struct Usr_Data *UsrDat,
			     const struct ExaPrn_Print *Print,
                             unsigned QstInd,
                             struct Qst_Question *Question);
void ExaTmp_WriteAnswers (const struct Usr_Data *UsrDat,
			  const struct ExaPrn_Print *Print,
			  unsigned QstInd,
			  struct Qst_Question *Question);

static void ExaTmp_WriteIntAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question);
static void ExaTmp_WriteFltAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question);
static void ExaTmp_WriteTF_Ans (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question);
static void ExaTmp_WriteChoAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question);
static void ExaTmp_WriteTxtAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question);
static void ExaTmp_WriteHeadUserCorrect (const struct Usr_Data *UsrDat);

//-----------------------------------------------------------------------------

/*****************************************************************************/
/********** List/Print selected exam templates from an exam session **********/
/*****************************************************************************/

void ExaTmp_ListSelectedExaTmps (void)
  {
   ExaTmp_ListOrPrintExaTmps (Vie_VIEW);
  }

void ExaTmp_PrintSelectedExaTmps (void)
  {
   ExaTmp_ListOrPrintExaTmps (Vie_PRINT);
  }

static void ExaTmp_ListOrPrintExaTmps (Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_ASSESSMENT_Exams;	// TODO: Change to link to section of listing/printing selected exams in a session
   extern const char *Txt_Listing_of_templates_in_session_X;
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   char *Title;
   unsigned NumUsrsInList;
   long *LstSelectedUsrCods;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get parameters *****/
   /* Get exams context and session */
   Exa_GetPars (&Exams,Exa_CHECK_EXA_COD);
   Exams.SesCod.Sel =
   Exams.SesCod.Par = Session.SesCod = ParCod_GetAndCheckPar (ParCod_Ses);

   /* Get list of groups selected */
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get exam data and session *****/
   Exa_GetExamDataByCod (&Exams.Exam);
   ExaSes_GetSessionDataByCod (&Session);

   /***** Exam begin *****/
   if (ViewType == Vie_VIEW)
      Exa_ShowOnlyOneExamBegin (&Exams,Frm_DONT_PUT_FORM);

   /***** Get lists of the selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Count number of valid users in list of encrypted user codes *****/
   NumUsrsInList = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected);

   if (NumUsrsInList)
     {
      /***** Get list of students selected to show their attendances *****/
      Usr_GetListSelectedUsrCods (&Gbl.Usrs.Selected,NumUsrsInList,&LstSelectedUsrCods);

      /***** Get exam data and session *****/
      Exa_GetExamDataByCod (&Exams.Exam);
      ExaSes_GetSessionDataByCod (&Session);

      /***** Begin section and box *****/
      if (ViewType == Vie_VIEW)
	{
	 HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);
	    if (asprintf (&Title,Txt_Listing_of_templates_in_session_X,
			  Session.Title) < 0)
	       Err_NotEnoughMemoryExit ();
	    Box_BoxBegin (Title,ExaTmp_PutIconsPrintExaTmps,&Exams,
			  Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
	    free (Title);
	}

      /***** Show table with exam templates *****/
      ExaTmp_ShowMultipleExaTmps (&Exams,&Session,ViewType,
				  NumUsrsInList,LstSelectedUsrCods);

      /***** End box and section *****/
      if (ViewType == Vie_VIEW)
	{
	    Box_BoxEnd ();
	 HTM_SECTION_End ();
	}

      /***** Free list of user codes *****/
      Usr_FreeListSelectedUsrCods (LstSelectedUsrCods);
     }

   /***** Exam end *****/
   if (ViewType == Vie_VIEW)
      Exa_ShowOnlyOneExamEnd ();
  }

/*****************************************************************************/
/****** Put icon to print selected exam templates from an exam session *******/
/*****************************************************************************/

static void ExaTmp_PutIconsPrintExaTmps (void *Exams)
  {
   if (Exams)
      Ico_PutContextualIconToPrint (ActPrnExaTmpSes,
				    ExaTmp_PutParsToPrintExaTmps,Exams);
  }

static void ExaTmp_PutParsToPrintExaTmps (void *Exams)
  {
   if (Exams)
     {
      Exa_PutPars (Exams);
      Grp_PutParsCodGrps ();
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
     }
  }

/*****************************************************************************/
/******* Show table with selected exam templates from an exam session ********/
/*****************************************************************************/

static void ExaTmp_ShowMultipleExaTmps (struct Exa_Exams *Exams,
				        const struct ExaSes_Session *Session,
					Vie_ViewType_t ViewType,
					unsigned NumUsrsInList,
					long *LstSelectedUsrCods)
  {
   struct Usr_Data UsrDat;
   unsigned NumUsr;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** List the exam prints (one for each user) *****/
   for (NumUsr = 0;
	NumUsr < NumUsrsInList;
	NumUsr++)
     {
      UsrDat.UsrCod = LstSelectedUsrCods[NumUsr];
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,		// Get from the database the data of the student
						   Usr_DONT_GET_PREFS,
						   Usr_GET_ROLE_IN_CRS))
	{
	 /***** Show exam print *****/
	 HTM_DIV_Begin (ViewType == Vie_PRINT &&
	                NumUsr ? "style=\"break-before:page;\"" :
				 NULL);
	    ExaTmp_ShowTemplateToTch (Exams,Session,ViewType,&UsrDat);
	 HTM_DIV_End ();
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/********** Show a template of the exam to a teacher to be printed ***********/
/*****************************************************************************/

static void ExaTmp_ShowTemplateToTch (struct Exa_Exams *Exams,
      				      const struct ExaSes_Session *Session,
				      Vie_ViewType_t ViewType,
				      struct Usr_Data *UsrDat)
  {
   struct ExaPrn_Print Print;

   /***** Create print or get existing print *****/
   ExaPrn_GetQstsPrint (Exams,Session,UsrDat,&Print,
			false);	// Start/resume

   /***** Show exam print to be shown on screen or printed on paper *****/
   ExaTmp_ShowTemplateWithSolutions (Exams,Session,ViewType,UsrDat,&Print);
  }

/*****************************************************************************/
/******* Show exam template to be shown on screen or printed on paper ********/
/*****************************************************************************/

static void ExaTmp_ShowTemplateWithSolutions (struct Exa_Exams *Exams,
					      const struct ExaSes_Session *Session,
					      Vie_ViewType_t ViewType,
					      struct Usr_Data *UsrDat,
					      struct ExaPrn_Print *Print)
  {
   extern const char *Hlp_ASSESSMENT_Exams_answer_exam;

   /***** Begin box *****/
   if (ViewType == Vie_VIEW)
      Box_BoxBegin (Session->Title,NULL,NULL,
		    Hlp_ASSESSMENT_Exams_answer_exam,Box_NOT_CLOSABLE);

   /***** Heading *****/
   /* Institution, degree and course */
   Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);

   /***** Show student *****/
   HTM_TABLE_BeginWideMarginPadding (10);
      ExaRes_ShowExamResultUser (UsrDat);
   HTM_TABLE_End ();

   /***** Exam description *****/
   Exa_GetAndWriteDescription (Exams->Exam.ExaCod);

   /***** Show table with questions *****/
   if (Print->NumQsts.All)
      ExaTmp_ShowTableTemplateWithSolutions (UsrDat,Print);

   /***** End box *****/
   if (ViewType == Vie_VIEW)
      Box_BoxEnd ();
  }

/*****************************************************************************/
/********** Show the main part (table) of an exam print to be shown **********/
/*****************************************************************************/

static void ExaTmp_ShowTableTemplateWithSolutions (const struct Usr_Data *UsrDat,
						   const struct ExaPrn_Print *Print)
  {
   unsigned QstInd;
   struct Qst_Question Question;

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

      /***** Write one row for each question *****/
      for (QstInd = 0;
	   QstInd < Print->NumQsts.All;
	   QstInd++)
	{
	 /* Create test question */
	 Qst_QstConstructor (&Question);
	 Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;

	 /* Get question from database */
	 ExaSet_GetQstDataFromDB (&Question);

	 /* Write question and answers */
	 ExaTmp_WriteQst (UsrDat,Print,QstInd,&Question);

	 /* Destroy test question */
	 Qst_QstDestructor (&Question);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***** Write a row of an exam template, with the answer to one question ******/
/*****************************************************************************/

static void ExaTmp_WriteQst (const struct Usr_Data *UsrDat,
			     const struct ExaPrn_Print *Print,
                             unsigned QstInd,
                             struct Qst_Question *Question)
  {
   static struct ExaSet_Set CurrentSet =
     {
      .ExaCod = -1L,
      .SetCod = -1L,
      .SetInd = 0,
      .NumQstsToPrint = 0,
      .Title[0] = '\0'
     };

   /***** If this is the first question *****/
   if (QstInd == 0)
      CurrentSet.SetCod = -1L;	// Reset current set

   if (Print->PrintedQuestions[QstInd].SetCod != CurrentSet.SetCod)
     {
      /***** Get data of this set *****/
      CurrentSet.SetCod = Print->PrintedQuestions[QstInd].SetCod;
      ExaSet_GetSetDataByCod (&CurrentSet);

      /***** Title for this set *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"2\" class=\"%s\"",The_GetColorRows ());
	    ExaSet_WriteSetTitle (&CurrentSet);
	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Number of question and answer type *****/
      HTM_TD_Begin ("class=\"RT\"");
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Question->Answer.Type,"DAT_SMALL");
      HTM_TD_End ();

      /***** Answers *****/
      HTM_TD_Begin ("class=\"LT\"");

	 /* Answers */
	 ExaTmp_WriteAnswers (UsrDat,Print,QstInd,Question);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Write solutions of a question in a template *****************/
/*****************************************************************************/

void ExaTmp_WriteAnswers (const struct Usr_Data *UsrDat,
			  const struct ExaPrn_Print *Print,
			  unsigned QstInd,
			  struct Qst_Question *Question)
  {
   void (*ExaTmp_WriteAns[Qst_NUM_ANS_TYPES]) (const struct Usr_Data *UsrDat,
					       const struct ExaPrn_Print *Print,
					       unsigned QstInd,
					       struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = ExaTmp_WriteIntAns,
     [Qst_ANS_FLOAT          ] = ExaTmp_WriteFltAns,
     [Qst_ANS_TRUE_FALSE     ] = ExaTmp_WriteTF_Ans,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaTmp_WriteChoAns,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaTmp_WriteChoAns,
     [Qst_ANS_TEXT           ] = ExaTmp_WriteTxtAns,
    };

   /***** Get correct answer and compute answer score depending on type *****/
   ExaTmp_WriteAns[Question->Answer.Type] (UsrDat,Print,QstInd,Question);
  }

/*****************************************************************************/
/******************* Write integer answer in a test print ********************/
/*****************************************************************************/

static void ExaTmp_WriteIntAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question)
  {
   long IntAnswerUsr;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Begin table *****/
   HTM_TABLE_Begin ("Ind_TBL");	// TODO: Change class

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 ExaTmp_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 if (Print->PrintedQuestions[QstInd].StrAnswers[0])		// If user has answered the question
	   {
	    if (sscanf (Print->PrintedQuestions[QstInd].StrAnswers,"%ld",&IntAnswerUsr) == 1)
	      {
	       HTM_TD_Begin ("class=\"CM %s_%s\"",
			     IntAnswerUsr == Question->Answer.Integer ? "Qst_ANS_OK" :	// Correct
									"Qst_ANS_BAD",	// Wrong
			     The_GetSuffix ());
		  HTM_Long (IntAnswerUsr);
	       HTM_TD_End ();
	      }
	    else
	      {
	       HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",The_GetSuffix ());
		  HTM_Question ();
	       HTM_TD_End ();
	      }
	   }
	 else							// If user has omitted the answer
	    HTM_TD_Empty (1);

	 /***** Write the correct answer *****/
	 HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",The_GetSuffix ());
	    HTM_Long (Question->Answer.Integer);
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write float answer in an test print ********************/
/*****************************************************************************/

static void ExaTmp_WriteFltAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question)
  {
   double FloatAnsUsr = 0.0;

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Begin table *****/
   HTM_TABLE_Begin ("Ind_TBL");	// TODO: Change class

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 ExaTmp_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 if (Print->PrintedQuestions[QstInd].StrAnswers[0])	// If user has answered the question
	   {
	    FloatAnsUsr = Str_GetDoubleFromStr (Print->PrintedQuestions[QstInd].StrAnswers);
	    // A bad formatted floating point answer will interpreted as 0.0
	    HTM_TD_Begin ("class=\"CM %s_%s\"",
			  (FloatAnsUsr >= Question->Answer.FloatingPoint[0] &&
			   FloatAnsUsr <= Question->Answer.FloatingPoint[1]) ? "Qst_ANS_OK" :	// Correct
									       "Qst_ANS_BAD",	// Wrong,		// Blank answer
			  The_GetSuffix ());
	       HTM_Double (FloatAnsUsr);
	    HTM_TD_End ();
	   }
	 else					// If user has omitted the answer
	    HTM_TD_Empty (1);

	 /***** Write the correct answer *****/
	 HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",The_GetSuffix ());
	    HTM_DoubleRange (Question->Answer.FloatingPoint[0],
			     Question->Answer.FloatingPoint[1]);
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Write false / true answer in a test print *****************/
/*****************************************************************************/

static void ExaTmp_WriteTF_Ans (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question)
  {
   char AnsTFUsr;

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   AnsTFUsr = Print->PrintedQuestions[QstInd].StrAnswers[0];

   /***** Begin table *****/
   HTM_TABLE_Begin ("Ind_TBL");	// TODO: Change class

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 ExaTmp_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 HTM_TD_Begin ("class=\"CM %s_%s\"",
		       AnsTFUsr == Question->Answer.TF ? "Qst_ANS_OK" :	// Correct
							 "Qst_ANS_BAD",	// Blank answer
		       The_GetSuffix ());
	    Qst_WriteAnsTF (AnsTFUsr);
	 HTM_TD_End ();

	 /***** Write the correct answer *****/
	 HTM_TD_Begin ("class=\"CM Qst_ANS_0_%s\"",The_GetSuffix ());
	    Qst_WriteAnsTF (Question->Answer.TF);
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************* Write single or multiple choice answer ******************/
/*****************************************************************************/

static void ExaTmp_WriteChoAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question)
  {
   extern const char *Txt_TST_Answer_given_by_the_user;
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   struct Answer
     {
      char *Class;
      char *Str;
     };
   static struct Answer AnsWrongOrCorrect[Qst_NUM_WRONG_CORRECT] =
     {
      [Qst_WRONG  ] = {.Class = "Qst_ANS_BAD",.Str = "&cross;"},
      [Qst_CORRECT] = {.Class = "Qst_ANS_OK" ,.Str = "&check;"}
     };
   unsigned NumOpt;
   Qst_WrongOrCorrect_t OptionWrongOrCorrect;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   const struct Answer *Ans;

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->PrintedQuestions[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (Print->PrintedQuestions[QstInd].StrAnswers,UsrAnswers);

   /***** Begin table *****/
   HTM_TABLE_Begin ("Ind_TBL");	// TODO: Change class

      HTM_TR_Begin (NULL);
	 ExaTmp_WriteHeadUserCorrect (UsrDat);
	 HTM_TD_Empty (1);
      HTM_TR_End ();

      /***** Write answers (one row per answer) *****/
      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 OptionWrongOrCorrect = Question->Answer.Options[Indexes[NumOpt]].Correct;

	 HTM_TR_Begin (NULL);

	    /* Draw icon depending on user's answer */
	    if (UsrAnswers[Indexes[NumOpt]] == HTM_CHECKED)	// This answer has been selected by the user
	      {
	       Ans = &AnsWrongOrCorrect[OptionWrongOrCorrect];
	       HTM_TD_Begin ("class=\"CT %s_%s\" title=\"%s\"",
			     Ans->Class,The_GetSuffix (),
			     Txt_TST_Answer_given_by_the_user);
		  HTM_Txt (Ans->Str);
	       HTM_TD_End ();
	      }
	    else	// This answer has NOT been selected by the user
	       HTM_TD_Empty (1);

	    /* Draw icon that indicates whether the answer is correct */
	    switch (OptionWrongOrCorrect)
	      {
	       case Qst_CORRECT:
		  HTM_TD_Begin ("class=\"CT Qst_ANS_0_%s\" title=\"%s\"",
				The_GetSuffix (),
				Txt_TST_Answer_given_by_the_teachers);
		     HTM_Txt ("&bull;");
		  HTM_TD_End ();
		  break;
	       case Qst_WRONG:
	       default:
		  HTM_TD_Empty (1);
		  break;
	      }

	    /* Answer letter (a, b, c,...) */
	    HTM_TD_Begin ("class=\"LT Qst_TXT_%s\"",The_GetSuffix ());
	       HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************************* Write text answer ***************************/
/*****************************************************************************/

static void ExaTmp_WriteTxtAns (const struct Usr_Data *UsrDat,
			        const struct ExaPrn_Print *Print,
				unsigned QstInd,
			        struct Qst_Question *Question)
  {
   static const char *Class[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [Qst_WRONG  ] = "Qst_ANS_BAD",
      [Qst_CORRECT] = "Qst_ANS_OK",
     };
   unsigned NumOpt;
   char TextAnsUsr[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   char TextAnsOK[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_WRONG;

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Begin table *****/
   HTM_TABLE_Begin ("Ind_TBL");	// TODO: Change class

      /***** Header with the title of each column *****/
      HTM_TR_Begin (NULL);
	 ExaTmp_WriteHeadUserCorrect (UsrDat);
      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Write the user answer *****/
	 if (Print->PrintedQuestions[QstInd].StrAnswers[0])	// If user has answered the question
	   {
	    /* Filter the user answer */
	    Str_Copy (TextAnsUsr,Print->PrintedQuestions[QstInd].StrAnswers,
		      sizeof (TextAnsUsr) - 1);

	    /* In order to compare student answer to stored answer,
	       the text answers are stored avoiding two or more consecurive spaces */
	    Str_ReplaceSeveralSpacesForOne (TextAnsUsr);

	    Str_ConvertToComparable (TextAnsUsr);

	    for (NumOpt = 0;
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

	    HTM_TD_Begin ("class=\"CT %s_%s\"",
			  Class[WrongOrCorrect],The_GetSuffix ());
	       HTM_Txt (Print->PrintedQuestions[QstInd].StrAnswers);
	    HTM_TD_End ();
	   }
	 else						// If user has omitted the answer
            HTM_TD_Empty (1);

	 /***** Write the correct answers *****/
	 HTM_TD_Begin ("class=\"CT\"");
	    HTM_UL_Begin ("class=\"LIST_LEFT\"");
	       for (NumOpt = 0;
		    NumOpt < Question->Answer.NumOptions;
		    NumOpt++)
		 {
		  HTM_LI_Begin ("class=\"LT Qst_ANS_0_%s\"",The_GetSuffix ());
			HTM_Txt (Question->Answer.Options[NumOpt].Text);
		  HTM_LI_End ();
		 }
	    HTM_UL_End ();
	 HTM_TD_End ();

      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********* Write head with two columns:                               ********/
/********* one for the user's answer and other for the correct answer ********/
/*****************************************************************************/

static void ExaTmp_WriteHeadUserCorrect (const struct Usr_Data *UsrDat)
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
