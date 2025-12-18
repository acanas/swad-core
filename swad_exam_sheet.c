// swad_exam_sheet.c: exams sheets

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
#include <string.h>		// For strcoll

#include "swad_box.h"
#include "swad_exam_database.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_sheet.h"
#include "swad_question_choice.h"
#include "swad_question_float.h"
#include "swad_question_int.h"
#include "swad_question_text.h"
#include "swad_question_tf.h"
#include "swad_global.h"
#include "swad_group.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaShe_ListOrPrintSheets (ExaShe_BlankOrSolved_t BlankOrSolved,
				      Vie_ViewType_t ViewType);

static void ExaShe_PutIconsPrintBlankSheets (void *Exams);
static void ExaShe_PutParsToPrintBlankSheets (void *Exams);

static void ExaShe_PutIconsPrintSolvedSheets (void *Exams);
static void ExaShe_PutParsToPrintSolvedSheets (void *Exams);

static void ExaShe_ShowMultipleSheets (struct Exa_Exams *Exams,
				       const struct ExaSes_Session *Session,
				       ExaShe_BlankOrSolved_t BlankOrSolved,
				       Vie_ViewType_t ViewType,
				       const struct Usr_ListCods *ListCods);

//------------------------------- Questions -----------------------------------

static void ExaShe_ShowQuestions (const struct ExaSes_Session *Session,
				  const struct ExaPrn_Print *Print);
static void ExaShe_WriteQst (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst);
static void ExaShe_WriteOptions (const struct ExaPrn_Print *Print,
				 unsigned QstInd,struct Qst_Question *Qst);
static void ExaShe_WriteOptionsTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
				       __attribute__((unused)) unsigned QstInd,
				       __attribute__((unused)) struct Qst_Question *Qst);
static void ExaShe_WriteOptionsChoAns (const struct ExaPrn_Print *Print,
				       unsigned QstInd,struct Qst_Question *Qst);

//-------------------------------- Answers ------------------------------------

static void ExaShe_ShowAnswers (const struct ExaSes_Session *Session,
			        const struct ExaPrn_Print *Print,
			        ExaShe_BlankOrSolved_t BlankOrSolved,
			        Vie_ViewType_t ViewType);
static void ExaShe_WriteQstAnswers (const struct ExaSes_Session *Session,
				    const struct ExaPrn_Print *Print,
				    ExaShe_BlankOrSolved_t BlankOrSolved,
				    Vie_ViewType_t ViewType,
				    unsigned QstInd,struct Qst_Question *Qst);

static void ExaShe_WriteBlankAnswers (struct Qst_Question *Qst);
static void ExaShe_WriteSolvedAnswers (const struct ExaSes_Session *Session,
				       const struct ExaPrn_Print *Print,
				       Vie_ViewType_t ViewType,
				       unsigned QstInd,struct Qst_Question *Qst);

static void ExaShe_WriteHead (ExaSes_Modality_t Modality);

/*****************************************************************************/
/***** Display/Print selected exam question sheets from an exam session ******/
/*****************************************************************************/

void ExaShe_ListBlankSheets (void)
  {
   ExaShe_ListOrPrintSheets (ExaShe_BLANK,Vie_VIEW);
  }

void ExaShe_PrintBlankSheets (void)
  {
   ExaShe_ListOrPrintSheets (ExaShe_BLANK,Vie_PRINT);
  }

/*****************************************************************************/
/****** Display/Print selected exam answer sheets from an exam session *******/
/*****************************************************************************/

void ExaShe_ListSolvedAnswerSheets (void)
  {
   ExaShe_ListOrPrintSheets (ExaShe_SOLVED,Vie_VIEW);
  }

void ExaShe_PrintSolvedAnswerSheets (void)
  {
   ExaShe_ListOrPrintSheets (ExaShe_SOLVED,Vie_PRINT);
  }

/*****************************************************************************/
/********* Display/Print selected exam sheets from an exam session ***********/
/*****************************************************************************/

static void ExaShe_ListOrPrintSheets (ExaShe_BlankOrSolved_t BlankOrSolved,
				      Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_ASSESSMENT_Exams_sheets;
   extern const char *Txt_List_of_exam_sheets;
   void (*FunctionToDrawContextualIcons[ExaShe_NUM_BLANK_OR_SOLVED]) (void *Args) =
     {
      [ExaShe_BLANK ] = ExaShe_PutIconsPrintBlankSheets,
      [ExaShe_SOLVED] = ExaShe_PutIconsPrintSolvedSheets,
     };
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   unsigned NumColsFromForm;
   Pho_ShowPhotos_t ShowPhotosFromForm;
   struct Usr_ListCods ListCods;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get parameters *****/
   /* Get exams context and session */
   Exa_GetPars (&Exams,Exa_CHECK_EXA_COD);
   Exams.SesCod.Sel =
   Exams.SesCod.Par = Session.SesCod = ParCod_GetAndCheckPar (ParCod_Ses);
   Exa_GetExamDataByCod (&Exams.Exam);
   ExaSes_GetSessionDataByCod (&Session);

   /* Get list of groups selected */
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /* Get and update number of columns */
   NumColsFromForm = ExaSes_GetParNumCols (Session.NumCols);
   ExaSes_UpdateNumCols (&Session,NumColsFromForm);

   /* Get and update whether to display users' photos */
   ShowPhotosFromForm = Pho_GetParShowPhotos ();
   ExaSes_UpdateShowPhotos (&Session,ShowPhotosFromForm);

   if (ViewType == Vie_VIEW)
     {
      /***** Exam begin *****/
      Exa_ShowOnlyOneExamBegin (&Exams,Frm_DONT_PUT_FORM);

	 /***** List of users for selection *****/
	 ExaSes_ListUsersForSelection (&Exams,&Session);
     }

   /***** Get list of selected users *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected,
					  Usr_GET_LIST_ALL_USRS);

   /***** Count number of valid users in list of encrypted user codes *****/
   ListCods.NumUsrs = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected);

   if (ListCods.NumUsrs)
     {
      /***** Get list of students selected *****/
      Usr_GetListSelectedUsrCods (&Gbl.Usrs.Selected,&ListCods);

      /***** Get exam data and session *****/
      Exa_GetExamDataByCod (&Exams.Exam);
      ExaSes_GetSessionDataByCod (&Session);

      /***** Begin section and box *****/
      if (ViewType == Vie_VIEW)
	{
	 Box_BoxBegin (Txt_List_of_exam_sheets,
		       FunctionToDrawContextualIcons[BlankOrSolved],&Exams,
		       Hlp_ASSESSMENT_Exams_sheets,Box_NOT_CLOSABLE);

	    /* Settings */
	    ExaSes_ShowFormSettings (&Session);
	}

      /***** Show table with exam sheets *****/
      ExaShe_ShowMultipleSheets (&Exams,&Session,BlankOrSolved,ViewType,
				 &ListCods);

      /***** End box and section *****/
      if (ViewType == Vie_VIEW)
	 Box_BoxEnd ();

      /***** Free list of user codes *****/
      Usr_FreeListSelectedUsrCods (&ListCods);
     }

   if (ViewType == Vie_VIEW)
      /***** Exam end *****/
      Exa_ShowOnlyOneExamEnd ();

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/************* Put icon to print selected exam question sheets ***************/
/*****************************************************************************/

static void ExaShe_PutIconsPrintBlankSheets (void *Exams)
  {
   if (Exams)
      Ico_PutContextualIconToPrint (ActPrnExaQstShe,
				    ExaShe_PutParsToPrintBlankSheets,Exams);
  }

static void ExaShe_PutParsToPrintBlankSheets (void *Exams)
  {
   if (Exams)
     {
      Exa_PutPars (Exams);
      Grp_PutParsCodGrps ();
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
     }
  }

/*****************************************************************************/
/************* Put icon to print selected exam answer sheets *****************/
/*****************************************************************************/

static void ExaShe_PutIconsPrintSolvedSheets (void *Exams)
  {
   if (Exams)
      Ico_PutContextualIconToPrint (ActPrnExaAnsShe,
				    ExaShe_PutParsToPrintSolvedSheets,Exams);
  }

static void ExaShe_PutParsToPrintSolvedSheets (void *Exams)
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

static void ExaShe_ShowMultipleSheets (struct Exa_Exams *Exams,
				       const struct ExaSes_Session *Session,
				       ExaShe_BlankOrSolved_t BlankOrSolved,
				       Vie_ViewType_t ViewType,
				       const struct Usr_ListCods *ListCods)
  {
   struct Usr_Data UsrDat;
   unsigned NumUsr;
   struct ExaPrn_Print Print;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

      /***** List the exam prints (one for each user) *****/
      for (NumUsr = 0;
	   NumUsr < ListCods->NumUsrs;
	   NumUsr++)
	{
	 /* Get student data from database */
	 UsrDat.UsrCod = ListCods->Lst[NumUsr];
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
						      Usr_DONT_GET_PREFS,
						      Usr_GET_ROLE_IN_CRS) == Exi_EXISTS)
	   {
	    /* Get if student has accepted enrolment in current course */
	    UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);

	    /* Create print or get existing print */
	    ExaPrn_GetQstsPrint (Exams,Session,&UsrDat,&Print,ExaPrn_DO_NOT_UPDATE_DATES);

	    /* Begin box */
	    if (ViewType == Vie_VIEW)
	       Box_BoxBegin (Session->Title,NULL,NULL,NULL,Box_NOT_CLOSABLE);

	    /* Heading */
	    // HTM_DIV_Begin (NumUsr && ViewType == Vie_PRINT ? "class=\"PAGE_EVEN\"" :
	    // 						     NULL);
	    HTM_DIV_Begin (NumUsr && ViewType == Vie_PRINT ? "class=\"PAGE_EVEN\"" :
							     NULL);
	       Exa_Header (Exams->Exam.ExaCod,&UsrDat,Session->ShowPhotos);
	    HTM_DIV_End ();

	    /* Show exam answer sheet */
	    HTM_DIV_Begin ("id=\"examprint_%s\" class=\"Exa_QSTS\"",	// Used for AJAX based refresh
			   Print.EnUsrCod);
	       ExaShe_ShowAnswers (Session,&Print,BlankOrSolved,ViewType);
	    HTM_DIV_End ();						// Used for AJAX based refresh

	    /* Show exam question sheet */
	    if (BlankOrSolved == ExaShe_BLANK)
	      {
	       HTM_DIV_Begin (" class=\"Exa_QSTS\"");
		  ExaShe_ShowQuestions (Session,&Print);
	       HTM_DIV_End ();
	      }

	    /* End box */
	    if (ViewType == Vie_VIEW)
	       Box_BoxEnd ();
	   }
	}

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*********** Show the main part (table) of an exam question sheet ************/
/*****************************************************************************/

static void ExaShe_ShowQuestions (const struct ExaSes_Session *Session,
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
   struct Qst_Question Qst;

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
	 if (Print->PrintedQsts[QstInd].SetCod != CurrentSet.SetCod)
	   {
	    /* Get data of this set */
	    CurrentSet.SetCod = Print->PrintedQsts[QstInd].SetCod;
	    ExaSet_GetSetDataByCod (&CurrentSet);

	    /* Title for this set */
	    HTM_DIV_Begin ("class=\"Exa_COL_SPAN %s\"",The_GetColorRows ());
	       ExaSet_WriteSetTitle (&CurrentSet);
	    HTM_DIV_End ();
	   }

	 /* Create question */
	 Qst_QstConstructor (&Qst);
	 Qst.QstCod = Print->PrintedQsts[QstInd].QstCod;

	    /* Get question from database */
	    ExaSet_GetQstDataFromDB (&Qst);

	    /* Write question */
	    ExaShe_WriteQst (Print,QstInd,&Qst);

	 /* Destroy question */
	 Qst_QstDestructor (&Qst);
	}

   /***** End list of questions *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Write a row of an exam answer sheet, with one question ***********/
/*****************************************************************************/

static void ExaShe_WriteQst (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   /***** Begin row *****/
   HTM_DIV_Begin ("class=\"Exa_CONTAINER\"");

      /***** Number of question and answer type *****/
      HTM_DIV_Begin ("class=\"Exa_LEFT\"");
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Qst->Answer.Type,Qst->Validity);
      HTM_DIV_End ();

      /***** Stem, media and answers *****/
      HTM_DIV_Begin ("class=\"Exa_RIGHT\"");
	 Qst_WriteQstStem (Qst->Stem,"Qst_TXT",HidVis_VISIBLE);
	 Med_ShowMedia (&Qst->Media,"Tst_MED_SHOW_CONT","Tst_MED_SHOW");
	 ExaShe_WriteOptions (Print,QstInd,Qst);
      HTM_DIV_End ();

   /***** End row *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********** Write options of a question in an exam question sheet ***********/
/*****************************************************************************/

static void ExaShe_WriteOptions (const struct ExaPrn_Print *Print,
				 unsigned QstInd,struct Qst_Question *Qst)
  {
   void (*ExaSheQst_WriteOptionsAns[Qst_NUM_ANS_TYPES]) (const struct ExaPrn_Print *Print,
							 unsigned QstInd,
							 struct Qst_Question *Qst) =
    {
     [Qst_ANS_INT            ] = NULL,
     [Qst_ANS_FLOAT          ] = NULL,
     [Qst_ANS_TRUE_FALSE     ] = ExaShe_WriteOptionsTF_Ans,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaShe_WriteOptionsChoAns,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaShe_WriteOptionsChoAns,
     [Qst_ANS_TEXT           ] = NULL,
    };

   /***** Write answers *****/
   if (ExaSheQst_WriteOptionsAns[Qst->Answer.Type])
      ExaSheQst_WriteOptionsAns[Qst->Answer.Type] (Print,QstInd,Qst);
  }

/*****************************************************************************/
/*********** Write false / true answer in an exam question sheet *************/
/*****************************************************************************/

static void ExaShe_WriteOptionsTF_Ans (__attribute__((unused)) const struct ExaPrn_Print *Print,
				       __attribute__((unused)) unsigned QstInd,
				       __attribute__((unused)) struct Qst_Question *Qst)
  {
   Qst_WriteAnsTF (Qst_OPTION_TRUE);
   HTM_Slash ();
   Qst_WriteAnsTF (Qst_OPTION_FALSE);
  }

/*****************************************************************************/
/***** Write single or multiple choice answer in an exam question sheet ******/
/*****************************************************************************/

static void ExaShe_WriteOptionsChoAns (const struct ExaPrn_Print *Print,
				       unsigned QstInd,struct Qst_Question *Qst)
  {
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Qst->Answer.NumOptions;
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
		  HTM_Txt (Qst->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Qst->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT","Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******* Receive answer from a student's paper exam filled by teacher ********/
/*****************************************************************************/

void ExaShe_ReceiveAnswer (void)
  {
   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   struct ExaPrn_Print Print;
   unsigned QstInd;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exams.Exam);
   ExaSes_ResetSession (&Session);

   /***** Get session code *****/
   Print.SesCod = ParCod_GetAndCheckPar (ParCod_Ses);

   /***** Get student's code *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData () == Exi_DOES_NOT_EXIST)
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

   /***** Get print data *****/
   Print.UsrCod = Gbl.Usrs.Other.UsrDat.UsrCod;
   ExaPrn_GetPrintDataBySesCodAndUsrCod (&Print);
   if (Print.PrnCod <= 0)
      Err_WrongExamExit ();
   Str_Copy (Print.EnUsrCod,Gbl.Usrs.Other.UsrDat.EnUsrCod,
	     sizeof (Print.EnUsrCod) - 1);

   /***** Get session data *****/
   Session.SesCod = Print.SesCod;
   ExaSes_GetSessionDataByCod (&Session);
   if (Session.SesCod <= 0)
      Err_WrongExamExit ();
   Exams.SesCod.Sel =
   Exams.SesCod.Par = Session.SesCod;

   /***** Get exam data *****/
   Exams.Exam.ExaCod = Session.ExaCod;
   Exa_GetExamDataByCod (&Exams.Exam);
   if (Exams.Exam.ExaCod <= 0)
      Err_WrongExamExit ();
   if (Exams.Exam.HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongExamExit ();

   /***** Get question index from form *****/
   QstInd = ExaPrn_GetParQstInd ();

   /***** Update answer and print in database *****/
   ExaPrn_UpdateAnswerAndPrint (&Print,QstInd);

   /***** Show table with questions to answer *****/
   ExaShe_ShowAnswers (&Session,&Print,ExaShe_SOLVED,Vie_VIEW);
  }

/*****************************************************************************/
/************ Show the main part (table) of an exam answer sheet *************/
/*****************************************************************************/

static void ExaShe_ShowAnswers (const struct ExaSes_Session *Session,
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
   struct Qst_Question Qst;

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
	 if (Print->PrintedQsts[QstInd].SetCod != CurrentSet.SetCod)
	   {
	    /* Get data of this set */
	    CurrentSet.SetCod = Print->PrintedQsts[QstInd].SetCod;
	    ExaSet_GetSetDataByCod (&CurrentSet);

	    /* Title for this set */
	    HTM_DIV_Begin ("class=\"Exa_COL_SPAN %s\"",The_GetColorRows ());
	       ExaSet_WriteSetTitle (&CurrentSet);
	    HTM_DIV_End ();
	   }

	 /* Create question */
	 Qst_QstConstructor (&Qst);
	 Qst.QstCod = Print->PrintedQsts[QstInd].QstCod;

	    /* Get question from database */
	    ExaSet_GetQstDataFromDB (&Qst);

	    /* Write question answers */
	    ExaShe_WriteQstAnswers (Session,Print,BlankOrSolved,ViewType,
			            QstInd,&Qst);

	 /* Destroy question */
	 Qst_QstDestructor (&Qst);
	}

   /***** End list of questions *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*** Write a row of an exam answer sheet, with the answer to one question ****/
/*****************************************************************************/

static void ExaShe_WriteQstAnswers (const struct ExaSes_Session *Session,
				    const struct ExaPrn_Print *Print,
				    ExaShe_BlankOrSolved_t BlankOrSolved,
				    Vie_ViewType_t ViewType,
				    unsigned QstInd,struct Qst_Question *Qst)
  {
   /***** Begin row *****/
   HTM_DIV_Begin ("class=\"Exa_CONTAINER\"");

      /***** Number of question and answer type *****/
      HTM_DIV_Begin ("class=\"Exa_LEFT\"");
	 Lay_WriteIndex (QstInd + 1,"BIG_INDEX");
	 Qst_WriteAnswerType (Qst->Answer.Type,Qst->Validity);
      HTM_DIV_End ();

      /***** Answers *****/
      HTM_DIV_Begin ("class=\"Exa_RIGHT\"");
	 switch (BlankOrSolved)
	   {
	    case ExaShe_BLANK:
	       ExaShe_WriteBlankAnswers (Qst);
	       break;
	    case ExaShe_SOLVED:
	       Frm_BeginFormNoAction ();	// Form that can not be submitted, to avoid enter key to send it
		  ExaShe_WriteSolvedAnswers (Session,Print,ViewType,
					     QstInd,Qst);
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

static void ExaShe_WriteBlankAnswers (struct Qst_Question *Qst)
  {
   static void (*ExaSheAns_WriteBlankAns[Qst_NUM_ANS_TYPES]) (const struct Qst_Question *Qst) =
    {
     [Qst_ANS_INT            ] = QstInt_WriteBlnkAns,
     [Qst_ANS_FLOAT          ] = QstFlt_WriteBlnkAns,
     [Qst_ANS_TRUE_FALSE     ] = QstTF__WriteBlnkAns,
     [Qst_ANS_UNIQUE_CHOICE  ] = QstCho_WriteBlnkAns,
     [Qst_ANS_MULTIPLE_CHOICE] = QstCho_WriteBlnkAns,
     [Qst_ANS_TEXT           ] = QstTxt_WriteBlnkAns,
    };

   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      HTM_TR_Begin (NULL);
         ExaSheAns_WriteBlankAns[Qst->Answer.Type] (Qst);
      HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************ Write answers of a question in an exam answer sheet ************/
/*****************************************************************************/

static void ExaShe_WriteSolvedAnswers (const struct ExaSes_Session *Session,
				       const struct ExaPrn_Print *Print,
				       Vie_ViewType_t ViewType,
				       unsigned QstInd,struct Qst_Question *Qst)
  {
   #define ExaSheAns_NUM_LAYOUTS 3
   enum
     {
      CORRECT,
      READONLY,
      EDITABLE,
     } Layout = Session->Modality == ExaSes_PAPER && ViewType == Vie_VIEW ? EDITABLE :
									    READONLY;
   static void (*ExaSheAns_WriteAns[Qst_NUM_ANS_TYPES][ExaSheAns_NUM_LAYOUTS]) (const struct ExaPrn_Print *Print,
									        unsigned QstInd,
									        struct Qst_Question *Qst) =
     {
      [Qst_ANS_INT            ][CORRECT ] = QstInt_WriteCorrAns,
      [Qst_ANS_INT            ][READONLY] = QstInt_WriteReadAns,
      [Qst_ANS_INT            ][EDITABLE] = QstInt_WriteEditAns,

      [Qst_ANS_FLOAT          ][CORRECT ] = QstFlt_WriteCorrAns,
      [Qst_ANS_FLOAT          ][READONLY] = QstFlt_WriteReadAns,
      [Qst_ANS_FLOAT          ][EDITABLE] = QstFlt_WriteEditAns,

      [Qst_ANS_TRUE_FALSE     ][CORRECT ] = QstTF__WriteCorrAns,
      [Qst_ANS_TRUE_FALSE     ][READONLY] = QstTF__WriteReadAns,
      [Qst_ANS_TRUE_FALSE     ][EDITABLE] = QstTF__WriteEditAns,

      [Qst_ANS_UNIQUE_CHOICE  ][CORRECT ] = QstCho_WriteCorrAns,
      [Qst_ANS_UNIQUE_CHOICE  ][READONLY] = QstCho_WriteReadAns,
      [Qst_ANS_UNIQUE_CHOICE  ][EDITABLE] = QstCho_WriteEditAns,

      [Qst_ANS_MULTIPLE_CHOICE][CORRECT ] = QstCho_WriteCorrAns,
      [Qst_ANS_MULTIPLE_CHOICE][READONLY] = QstCho_WriteReadAns,
      [Qst_ANS_MULTIPLE_CHOICE][EDITABLE] = QstCho_WriteEditAns,

      [Qst_ANS_TEXT           ][CORRECT ] = QstTxt_WriteCorrAns,
      [Qst_ANS_TEXT           ][READONLY] = QstTxt_WriteReadAns,
      [Qst_ANS_TEXT           ][EDITABLE] = QstTxt_WriteEditAns,
     };

   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write the correct answer *****/
      HTM_TR_Begin (NULL);
	 ExaShe_WriteHead (ExaSes_NONE);
	 ExaSheAns_WriteAns[Qst->Answer.Type][CORRECT] (Print,QstInd,Qst);
      HTM_TR_End ();

      /***** Write the editable/readonly answer *****/
      HTM_TR_Begin (NULL);
	 ExaShe_WriteHead (Session->Modality);
	 ExaSheAns_WriteAns[Qst->Answer.Type][Layout] (Print,QstInd,Qst);
      HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************** Receive answer to an exam sheet **********************/
/*****************************************************************************/

void ExaShe_WriteJSToUpdateSheet (const struct ExaPrn_Print *Print,
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

static void ExaShe_WriteHead (ExaSes_Modality_t Modality)
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
