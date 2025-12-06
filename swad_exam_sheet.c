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

#include "swad_box.h"
#include "swad_exam_database.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_sheet.h"
#include "swad_exam_sheet_answer.h"
#include "swad_exam_sheet_question.h"
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
	    HTM_DIV_Begin (NumUsr &&
		           ViewType == Vie_PRINT ? "style=\"break-before:page;\"" :
						   NULL);

	       /* Institution, degree and course */
	       Lay_WriteHeaderClassPhoto (Hie_CRS,Vie_VIEW);

	       /* Show student */
	       ExaRes_ShowExamResultUser (Session,&UsrDat);

	       /* Exam description */
	       Exa_GetAndWriteDescription (Exams->Exam.ExaCod);

	    HTM_DIV_End ();

	    /* Show exam answer sheet */
	    HTM_DIV_Begin ("id=\"examprint_%s\" class=\"Exa_QSTS\"",	// Used for AJAX based refresh
			   Print.EnUsrCod);
	       ExaSheAns_ShowAnswers (Session,&Print,BlankOrSolved,ViewType);
	    HTM_DIV_End ();						// Used for AJAX based refresh

	    /* Show exam question sheet */
	    if (BlankOrSolved == ExaShe_BLANK)
	      {
	       HTM_DIV_Begin (NULL);
		  ExaSheQst_ShowQuestions (Session,&Print);
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

   /***** Get questions and current user's answers of exam print from database *****/
   ExaPrn_GetPrintQuestionsFromDB (&Print);

   /***** Get answer to the specified question from form *****/
   ExaPrn_GetAnswerFromForm (&Print,QstInd);

   /***** Update answer in database *****/
   /* Compute question score and store in database */
   ExaPrn_ComputeScoreAndStoreQuestionOfPrint (&Print,QstInd);

   /* Update exam print in database */
   Print.NumQsts.NotBlank = Exa_DB_GetNumQstsNotBlankInPrint (Print.PrnCod);
   Print.Score.All = Exa_DB_ComputeTotalScoreOfPrint (Print.PrnCod);
   Exa_DB_UpdatePrint (&Print);

   /***** Show table with questions to answer *****/
   ExaSheAns_ShowAnswers (&Session,&Print,ExaShe_SOLVED,Vie_VIEW);
  }
