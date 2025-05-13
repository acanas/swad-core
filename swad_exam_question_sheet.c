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
#include "swad_exam_question_sheet.h"
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
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaQstShe_ListOrPrintExamQstSheets (Vie_ViewType_t ViewType);
static void ExaQstShe_PutIconsPrintExamQstSheets (void *Exams);
static void ExaQstShe_PutParsToPrintExamQstSheets (void *Exams);
static void ExaQstShe_ShowMultipleExamQstSheets (struct Exa_Exams *Exams,
						 const struct ExaSes_Session *Session,
						 Vie_ViewType_t ViewType,
						 unsigned NumUsrsInList,
						 long *LstSelectedUsrCods);
static void ExaQstShe_GetQstsAndShowExamQstSheet (struct Exa_Exams *Exams,
						  const struct ExaSes_Session *Session,
						  Vie_ViewType_t ViewType,
						  struct Usr_Data *UsrDat);
static void ExaQstShe_ShowExamQstSheet (struct Exa_Exams *Exams,
				        const struct ExaSes_Session *Session,
				        Vie_ViewType_t ViewType,
				        struct Usr_Data *UsrDat,
				        struct ExaPrn_Print *Print);
static void ExaQstShe_ShowTableQuestions (const struct ExaPrn_Print *Print);
static void ExaQstShe_WriteQst (const struct ExaPrn_Print *Print,
				unsigned QstInd,
				struct Qst_Question *Question);
static void ExaQstShe_WriteAnswers (const struct ExaPrn_Print *Print,
				    unsigned QstInd,
				    struct Qst_Question *Question);
static void ExaQstShe_WriteTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
	                           __attribute__((unused)) unsigned QstInd,
                                   __attribute__((unused)) struct Qst_Question *Question);
static void ExaQstShe_WriteChoAns (const struct ExaPrn_Print *Print,
                                   unsigned QstInd,
                                   struct Qst_Question *Question);

/*****************************************************************************/
/***** Display/Print selected exam question sheets from an exam session ******/
/*****************************************************************************/

void ExaQstShe_ListExamQstSheets (void)
  {
   ExaQstShe_ListOrPrintExamQstSheets (Vie_VIEW);
  }

void ExaQstShe_PrintExamQstSheets (void)
  {
   ExaQstShe_ListOrPrintExamQstSheets (Vie_PRINT);
  }

static void ExaQstShe_ListOrPrintExamQstSheets (Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_ASSESSMENT_Exams;	// TODO: Change to link to section of listing/printing selected exams in a session
   extern const char *Txt_List_of_exam_question_sheets_for_session_X;
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

   if (ViewType == Vie_VIEW)
     {
      /***** Exam begin *****/
      Exa_ShowOnlyOneExamBegin (&Exams,Frm_DONT_PUT_FORM);

	 /***** List of users for selection *****/
	 ExaSes_ListUsersForSelection (&Exams,&Session);
     }

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
	 HTM_SECTION_Begin (ExaSes_EXAM_SHEET_OPTIONS_SECTION_ID);
	    if (asprintf (&Title,Txt_List_of_exam_question_sheets_for_session_X,
			  Session.Title) < 0)
	       Err_NotEnoughMemoryExit ();
	    Box_BoxBegin (Title,ExaQstShe_PutIconsPrintExamQstSheets,&Exams,
			  Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
	    free (Title);
	}

      /***** Show table with exam prints *****/
      ExaQstShe_ShowMultipleExamQstSheets (&Exams,&Session,ViewType,
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

   if (ViewType == Vie_VIEW)
      /***** Exam end *****/
      Exa_ShowOnlyOneExamEnd ();
  }

/*****************************************************************************/
/************* Put icon to print selected exam question sheets ***************/
/*****************************************************************************/

static void ExaQstShe_PutIconsPrintExamQstSheets (void *Exams)
  {
   if (Exams)
      Ico_PutContextualIconToPrint (ActPrnExaQstShe,
				    ExaQstShe_PutParsToPrintExamQstSheets,Exams);
  }

static void ExaQstShe_PutParsToPrintExamQstSheets (void *Exams)
  {
   if (Exams)
     {
      Exa_PutPars (Exams);
      Grp_PutParsCodGrps ();
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
     }
  }

/*****************************************************************************/
/**** Show table with selected exam question sheets from an exam session *****/
/*****************************************************************************/

static void ExaQstShe_ShowMultipleExamQstSheets (struct Exa_Exams *Exams,
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
	    ExaQstShe_GetQstsAndShowExamQstSheet (Exams,Session,ViewType,&UsrDat);
	 HTM_DIV_End ();
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*************** Get questions and show an exam question sheet ***************/
/*****************************************************************************/

static void ExaQstShe_GetQstsAndShowExamQstSheet (struct Exa_Exams *Exams,
						  const struct ExaSes_Session *Session,
						  Vie_ViewType_t ViewType,
						  struct Usr_Data *UsrDat)
  {
   struct ExaPrn_Print Print;

   /***** Create print or get existing print *****/
   ExaPrn_GetQstsPrint (Exams,Session,UsrDat,&Print,
			false);	// Start/resume

   /***** Show exam question sheet *****/
   ExaQstShe_ShowExamQstSheet (Exams,Session,ViewType,UsrDat,&Print);
  }

/*****************************************************************************/
/************************** Show exam question sheet *************************/
/*****************************************************************************/

static void ExaQstShe_ShowExamQstSheet (struct Exa_Exams *Exams,
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
      ExaQstShe_ShowTableQuestions (Print);

   /***** End box *****/
   if (ViewType == Vie_VIEW)
      Box_BoxEnd ();
  }

/*****************************************************************************/
/*********** Show the main part (table) of an exam question sheet ************/
/*****************************************************************************/

static void ExaQstShe_ShowTableQuestions (const struct ExaPrn_Print *Print)
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

	 /* Write question */
	 ExaQstShe_WriteQst (Print,QstInd,&Question);

	 /* Destroy test question */
	 Qst_QstDestructor (&Question);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********** Write a row of an exam answer sheet, with one question ***********/
/*****************************************************************************/

static void ExaQstShe_WriteQst (const struct ExaPrn_Print *Print,
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

      /***** Stem, media and answers *****/
      HTM_TD_Begin ("class=\"LT\"");

	 /* Stem */
	 Qst_WriteQstStem (Question->Stem,"Qst_TXT",HidVis_VISIBLE);

	 /* Media */
	 Med_ShowMedia (&Question->Media,
			"Tst_MED_SHOW_CONT",
			"Tst_MED_SHOW");

	 /* Answers */
	 ExaQstShe_WriteAnswers (Print,QstInd,Question);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Write answers of a question in an exam question sheet ***********/
/*****************************************************************************/

static void ExaQstShe_WriteAnswers (const struct ExaPrn_Print *Print,
				    unsigned QstInd,
				    struct Qst_Question *Question)
  {
   void (*ExaQstShe_WriteAns[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
                                                  unsigned QstInd,
                                                  struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = NULL,
     [Qst_ANS_FLOAT          ] = NULL,
     [Qst_ANS_TRUE_FALSE     ] = ExaQstShe_WriteTF_Ans,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaQstShe_WriteChoAns,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaQstShe_WriteChoAns,
     [Qst_ANS_TEXT           ] = NULL,
    };

   /***** Write answers *****/
   if (ExaQstShe_WriteAns[Question->Answer.Type])
      ExaQstShe_WriteAns[Question->Answer.Type] (Print,QstInd,Question);
  }

/*****************************************************************************/
/*********** Write false / true answer in an exam question sheet *************/
/*****************************************************************************/

static void ExaQstShe_WriteTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
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

static void ExaQstShe_WriteChoAns (const struct ExaPrn_Print *Print,
                                   unsigned QstInd,
                                   struct Qst_Question *Question)
  {
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->PrintedQuestions[QstInd].StrIndexes,Indexes);

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
			      "Tst_MED_SHOW_CONT",
			      "Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }
