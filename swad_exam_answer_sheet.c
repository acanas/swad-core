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
#include "swad_exam_answer_sheet.h"
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
/**************************** Private constants ******************************/
/*****************************************************************************/

static const char *ExaAnsShe_Class[Qst_NUM_WRONG_CORRECT] =
     {
      [Qst_BLANK  ] = "Qst_ANS_0",
      [Qst_WRONG  ] = "Qst_ANS_BAD",
      [Qst_CORRECT] = "Qst_ANS_OK",
     };

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define ExaAnsShe_NUM_BLANK_OR_SOLVED 2
typedef enum
  {
   ExaAnsShe_BLANK,
   ExaAnsShe_SOLVED,
  } ExaAnsShe_BlankOrSolved_t;

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaAnsShe_ListOrPrintSheets (Vie_ViewType_t ViewType,
					 ExaAnsShe_BlankOrSolved_t BlankOrSolved);
static void ExaAnsShe_PutIconsPrintBlankExamAnsSheets (void *Exams);
static void ExaAnsShe_PutIconsPrintSolvedExamAnsSheets (void *Exams);
static void ExaAnsShe_PutParsToPrintExamAnsSheets (void *Exams);
static void ExaAnsShe_ShowMultipleSheets (struct Exa_Exams *Exams,
					  const struct ExaSes_Session *Session,
					  Vie_ViewType_t ViewType,
					  ExaAnsShe_BlankOrSolved_t BlankOrSolved,
					  unsigned NumUsrsInList,
					  long *LstSelectedUsrCods);
static void ExaAnsShe_GetQstsAndShowSheet (struct Exa_Exams *Exams,
					   const struct ExaSes_Session *Session,
					   Vie_ViewType_t ViewType,
					   ExaAnsShe_BlankOrSolved_t BlankOrSolved,
					   struct Usr_Data *UsrDat);
static void ExaAnsShe_ShowSheet (struct Exa_Exams *Exams,
			         const struct ExaSes_Session *Session,
			         Vie_ViewType_t ViewType,
			         ExaAnsShe_BlankOrSolved_t BlankOrSolved,
			         struct Usr_Data *UsrDat,
			         struct ExaPrn_Print *Print);
static void ExaAnsShe_ShowAnswers (const struct ExaSes_Session *Session,
				   ExaAnsShe_BlankOrSolved_t BlankOrSolved,
				   const struct ExaPrn_Print *Print);
static void ExaAnsShe_WriteQst (const struct ExaSes_Session *Session,
				ExaAnsShe_BlankOrSolved_t BlankOrSolved,
			        const struct ExaPrn_Print *Print,
                                unsigned QstInd,
                                struct Qst_Question *Question);

static void ExaAnsShe_WriteBlankAnswers (struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankIntAns (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankFltAns (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankTF_Ans (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankChoAns (const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankTxtAns (__attribute__((unused)) const struct Qst_Question *Question);

static void ExaAnsShe_WriteSolvedAnswers (const struct ExaSes_Session *Session,
					  struct Qst_Question *Question,
					  const struct ExaPrn_Print *Print,
					  unsigned QstInd);

static void ExaAnsShe_WriteCorrectIntAns (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd);
static void ExaAnsShe_WriteCorrectFltAns (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd);
static void ExaAnsShe_WriteCorrectTF_Ans (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd);
static void ExaAnsShe_WriteCorrectChoAns (struct Qst_Question *Question,
					  const struct ExaPrn_Print *Print,
					  unsigned QstInd);
static void ExaAnsShe_WriteCorrectTxtAns (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd);

static void ExaAnsShe_WriteOnlineIntAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd);
static void ExaAnsShe_WriteOnlineFltAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd);
static void ExaAnsShe_WriteOnlineTF_Ans (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd);
static void ExaAnsShe_WriteOnlineChoAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd);
static void ExaAnsShe_WriteOnlineTxtAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd);

static void ExaAnsShe_WritePaperIntAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd);
static void ExaAnsShe_WritePaperFltAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd);
static void ExaAnsShe_WritePaperTF_Ans (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd);
static void ExaAnsShe_WritePaperChoAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd);
static void ExaAnsShe_WritePaperTxtAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd);
static void ExaAnsShe_WriteJSToUpdateSheet (const struct ExaPrn_Print *Print,
	                                    unsigned QstInd,
	                                    const char *Id,int NumOpt);

static void ExaAnsShe_WriteHead (ExaSes_Modality_t Modality);

/*****************************************************************************/
/****** Display/Print selected exam answer sheets from an exam session *******/
/*****************************************************************************/

void ExaAnsShe_ListBlankSheets (void)
  {
   ExaAnsShe_ListOrPrintSheets (Vie_VIEW,ExaAnsShe_BLANK);
  }

void ExaAnsShe_PrintBlankSheets (void)
  {
   ExaAnsShe_ListOrPrintSheets (Vie_PRINT,ExaAnsShe_BLANK);
  }

void ExaAnsShe_ListSolvedSheets (void)
  {
   ExaAnsShe_ListOrPrintSheets (Vie_VIEW,ExaAnsShe_SOLVED);
  }

void ExaAnsShe_PrintSolvedSheets (void)
  {
   ExaAnsShe_ListOrPrintSheets (Vie_PRINT,ExaAnsShe_SOLVED);
  }

static void ExaAnsShe_ListOrPrintSheets (Vie_ViewType_t ViewType,
					 ExaAnsShe_BlankOrSolved_t BlankOrSolved)
  {
   extern const char *Hlp_ASSESSMENT_Exams;	// TODO: Change to link to section of listing/printing selected exams in a session
   extern const char *Txt_List_of_exam_answer_sheets_for_session_X;
   static void (*ExaAnsShe_PutIcons[ExaAnsShe_NUM_BLANK_OR_SOLVED]) (void *Exams) =
     {
      [ExaAnsShe_BLANK ] = ExaAnsShe_PutIconsPrintBlankExamAnsSheets,
      [ExaAnsShe_SOLVED] = ExaAnsShe_PutIconsPrintSolvedExamAnsSheets
     };

   struct Exa_Exams Exams;
   struct ExaSes_Session Session;
   unsigned NumColsFromForm;
   Pho_ShowPhotos_t ShowPhotosFromForm;
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

   /***** Get list of selected users if not already got *****/
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected,
					  Usr_GET_LIST_ALL_USRS);

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
	    if (asprintf (&Title,Txt_List_of_exam_answer_sheets_for_session_X,
			  Session.Title) < 0)
	       Err_NotEnoughMemoryExit ();
	    Box_BoxBegin (Title,ExaAnsShe_PutIcons[BlankOrSolved],&Exams,
			  Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);
	    free (Title);
	}

      /***** Show table with exam answer sheets *****/
      ExaAnsShe_ShowMultipleSheets (&Exams,&Session,ViewType,BlankOrSolved,
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

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/************* Put icon to print selected exam answer sheets *****************/
/*****************************************************************************/

static void ExaAnsShe_PutIconsPrintBlankExamAnsSheets (void *Exams)
  {
   if (Exams)
      Ico_PutContextualIconToPrint (ActPrnBlkExaAnsShe,
				    ExaAnsShe_PutParsToPrintExamAnsSheets,Exams);
  }

static void ExaAnsShe_PutIconsPrintSolvedExamAnsSheets (void *Exams)
  {
   if (Exams)
      Ico_PutContextualIconToPrint (ActPrnSolExaAnsShe,
				    ExaAnsShe_PutParsToPrintExamAnsSheets,Exams);
  }

static void ExaAnsShe_PutParsToPrintExamAnsSheets (void *Exams)
  {
   if (Exams)
     {
      Exa_PutPars (Exams);
      Grp_PutParsCodGrps ();
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
     }
  }

/*****************************************************************************/
/***** Show table with selected exam answer sheets from an exam session ******/
/*****************************************************************************/

static void ExaAnsShe_ShowMultipleSheets (struct Exa_Exams *Exams,
					  const struct ExaSes_Session *Session,
					  Vie_ViewType_t ViewType,
					  ExaAnsShe_BlankOrSolved_t BlankOrSolved,
					  unsigned NumUsrsInList,
					  long *LstSelectedUsrCods)
  {
   struct Usr_Data UsrDat;
   unsigned NumUsr;

   /***** Settings *****/
   ExaSes_ShowFormSettings (Session);

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
						      Usr_GET_ROLE_IN_CRS) == Exi_EXISTS)
	   {
	    /***** Show exam print *****/
	    HTM_DIV_Begin (ViewType == Vie_PRINT &&
			   NumUsr ? "style=\"break-before:page;\"" :
				    NULL);
	       ExaAnsShe_GetQstsAndShowSheet (Exams,Session,
					      ViewType,BlankOrSolved,&UsrDat);
	    HTM_DIV_End ();
	   }
	}

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/**************** Get questions and show an exam answer sheet ****************/
/*****************************************************************************/

static void ExaAnsShe_GetQstsAndShowSheet (struct Exa_Exams *Exams,
					   const struct ExaSes_Session *Session,
					   Vie_ViewType_t ViewType,
					   ExaAnsShe_BlankOrSolved_t BlankOrSolved,
					   struct Usr_Data *UsrDat)
  {
   struct ExaPrn_Print Print;

   /***** Create print or get existing print *****/
   ExaPrn_GetQstsPrint (Exams,Session,UsrDat,&Print,
			ExaPrn_DO_NOT_UPDATE_DATES);

   /***** Show exam answer sheet *****/
   ExaAnsShe_ShowSheet (Exams,Session,ViewType,BlankOrSolved,UsrDat,&Print);
  }

/*****************************************************************************/
/*************************** Show exam answer sheet **************************/
/*****************************************************************************/

static void ExaAnsShe_ShowSheet (struct Exa_Exams *Exams,
			         const struct ExaSes_Session *Session,
			         Vie_ViewType_t ViewType,
			         ExaAnsShe_BlankOrSolved_t BlankOrSolved,
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
   ExaRes_ShowExamResultUser (Session,UsrDat);

   /***** Exam description *****/
   Exa_GetAndWriteDescription (Exams->Exam.ExaCod);

   /***** Show table with answers *****/
   if (Print->NumQsts.All)
     {
      HTM_DIV_Begin ("id=\"examprint_%s\" class=\"Exa_QSTS\"",	// Used for AJAX based refresh
		     Print->EnUsrCod);
	 ExaAnsShe_ShowAnswers (Session,BlankOrSolved,Print);
      HTM_DIV_End ();						// Used for AJAX based refresh
     }

   /***** End box *****/
   if (ViewType == Vie_VIEW)
      Box_BoxEnd ();
  }

/*****************************************************************************/
/************ Show the main part (table) of an exam answer sheet *************/
/*****************************************************************************/

static void ExaAnsShe_ShowAnswers (const struct ExaSes_Session *Session,
				   ExaAnsShe_BlankOrSolved_t BlankOrSolved,
				   const struct ExaPrn_Print *Print)
  {
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

   CurrentSet.SetCod = -1L;	// Reset current set

   /***** Write questions in columns *****/
   HTM_DIV_Begin ("class=\"Exa_COLS_%u\"",Session->NumCols);

      /***** Write one row for each question *****/
      for (QstInd = 0;
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
	    ExaAnsShe_WriteQst (Session,BlankOrSolved,Print,QstInd,&Question);

	 /* Destroy test question */
	 Qst_QstDestructor (&Question);
	}

   /***** End list of questions *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*** Write a row of an exam answer sheet, with the answer to one question ****/
/*****************************************************************************/

static void ExaAnsShe_WriteQst (const struct ExaSes_Session *Session,
				ExaAnsShe_BlankOrSolved_t BlankOrSolved,
			        const struct ExaPrn_Print *Print,
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

      /***** Answers *****/
      HTM_DIV_Begin ("class=\"Exa_RIGHT\"");
	 switch (BlankOrSolved)
	   {
	    case ExaAnsShe_BLANK:
	       ExaAnsShe_WriteBlankAnswers (Question);
	       break;
	    case ExaAnsShe_SOLVED:
	       Frm_BeginFormNoAction ();	// Form that can not be submitted, to avoid enter key to send it
		  ExaAnsShe_WriteSolvedAnswers (Session,Question,Print,QstInd);
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

static void ExaAnsShe_WriteBlankAnswers (struct Qst_Question *Question)
  {
   static void (*ExaAnsShe_WriteBlankAns[Qst_NUM_ANS_TYPES]) (const struct Qst_Question *Question) =
    {
     [Qst_ANS_INT            ] = ExaAnsShe_WriteBlankIntAns,
     [Qst_ANS_FLOAT          ] = ExaAnsShe_WriteBlankFltAns,
     [Qst_ANS_TRUE_FALSE     ] = ExaAnsShe_WriteBlankTF_Ans,
     [Qst_ANS_UNIQUE_CHOICE  ] = ExaAnsShe_WriteBlankChoAns,
     [Qst_ANS_MULTIPLE_CHOICE] = ExaAnsShe_WriteBlankChoAns,
     [Qst_ANS_TEXT           ] = ExaAnsShe_WriteBlankTxtAns,
    };

   ExaAnsShe_WriteBlankAns[Question->Answer.Type] (Question);
  }

/*****************************************************************************/
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

static void ExaAnsShe_WriteBlankIntAns (__attribute__((unused)) const struct Qst_Question *Question)
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

static void ExaAnsShe_WriteBlankFltAns (__attribute__((unused)) const struct Qst_Question *Question)
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

static void ExaAnsShe_WriteBlankTF_Ans (__attribute__((unused)) const struct Qst_Question *Question)
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

static void ExaAnsShe_WriteBlankChoAns (const struct Qst_Question *Question)
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

static void ExaAnsShe_WriteBlankTxtAns (__attribute__((unused)) const struct Qst_Question *Question)
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

static void ExaAnsShe_WriteSolvedAnswers (const struct ExaSes_Session *Session,
					  struct Qst_Question *Question,
					  const struct ExaPrn_Print *Print,
					  unsigned QstInd)
  {
   static void (*ExaAnsShe_WriteSolvedAns[ExaSes_NUM_MODALITIES][Qst_NUM_ANS_TYPES]) (struct Qst_Question *Question,
										      const struct ExaPrn_Print *Print,
										      unsigned QstInd) =
    {
     [ExaSes_NONE  ][Qst_ANS_INT            ] = ExaAnsShe_WriteCorrectIntAns,
     [ExaSes_NONE  ][Qst_ANS_FLOAT          ] = ExaAnsShe_WriteCorrectFltAns,
     [ExaSes_NONE  ][Qst_ANS_TRUE_FALSE     ] = ExaAnsShe_WriteCorrectTF_Ans,
     [ExaSes_NONE  ][Qst_ANS_UNIQUE_CHOICE  ] = ExaAnsShe_WriteCorrectChoAns,
     [ExaSes_NONE  ][Qst_ANS_MULTIPLE_CHOICE] = ExaAnsShe_WriteCorrectChoAns,
     [ExaSes_NONE  ][Qst_ANS_TEXT           ] = ExaAnsShe_WriteCorrectTxtAns,

     [ExaSes_ONLINE][Qst_ANS_INT            ] = ExaAnsShe_WriteOnlineIntAns,
     [ExaSes_ONLINE][Qst_ANS_FLOAT          ] = ExaAnsShe_WriteOnlineFltAns,
     [ExaSes_ONLINE][Qst_ANS_TRUE_FALSE     ] = ExaAnsShe_WriteOnlineTF_Ans,
     [ExaSes_ONLINE][Qst_ANS_UNIQUE_CHOICE  ] = ExaAnsShe_WriteOnlineChoAns,
     [ExaSes_ONLINE][Qst_ANS_MULTIPLE_CHOICE] = ExaAnsShe_WriteOnlineChoAns,
     [ExaSes_ONLINE][Qst_ANS_TEXT           ] = ExaAnsShe_WriteOnlineTxtAns,

     [ExaSes_PAPER ][Qst_ANS_INT            ] = ExaAnsShe_WritePaperIntAns,
     [ExaSes_PAPER ][Qst_ANS_FLOAT          ] = ExaAnsShe_WritePaperFltAns,
     [ExaSes_PAPER ][Qst_ANS_TRUE_FALSE     ] = ExaAnsShe_WritePaperTF_Ans,
     [ExaSes_PAPER ][Qst_ANS_UNIQUE_CHOICE  ] = ExaAnsShe_WritePaperChoAns,
     [ExaSes_PAPER ][Qst_ANS_MULTIPLE_CHOICE] = ExaAnsShe_WritePaperChoAns,
     [ExaSes_PAPER ][Qst_ANS_TEXT           ] = ExaAnsShe_WritePaperTxtAns,
    };

   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      /***** Write the answer *****/
      HTM_TR_Begin (NULL);
	 ExaAnsShe_WriteHead (ExaSes_NONE);
	 ExaAnsShe_WriteSolvedAns[ExaSes_NONE][Question->Answer.Type] (Question,Print,QstInd);
      HTM_TR_End ();

      /***** Write the answer *****/
      HTM_TR_Begin (NULL);
	 ExaAnsShe_WriteHead (Session->Modality);
	 ExaAnsShe_WriteSolvedAns[Session->Modality][Question->Answer.Type] (Question,Print,QstInd);
      HTM_TR_End ();

   HTM_TABLE_End ();
  }

/*****************************************************************************/
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

static void ExaAnsShe_WriteCorrectIntAns (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd)
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

static void ExaAnsShe_WriteCorrectFltAns (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd)
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

static void ExaAnsShe_WriteCorrectTF_Ans (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd)
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

static void ExaAnsShe_WriteCorrectChoAns (struct Qst_Question *Question,
					  const struct ExaPrn_Print *Print,
					  unsigned QstInd)
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

static void ExaAnsShe_WriteCorrectTxtAns (struct Qst_Question *Question,
					  __attribute__((unused)) const struct ExaPrn_Print *Print,
					  __attribute__((unused)) unsigned QstInd)
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

static void ExaAnsShe_WriteOnlineIntAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd)
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
   else							// If student has omitted the answer
      HTM_TD_Empty (1);
  }

/*****************************************************************************/
/**************** Write float answer in an exam answer sheet *****************/
/*****************************************************************************/

static void ExaAnsShe_WriteOnlineFltAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd)
  {
   double AnsUsr = 0.0;
   bool Valid;

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write online answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      Valid = Str_GetDoubleFromStr (Print->Qsts[QstInd].Answer.Str,
			            &AnsUsr);
      // A bad formatted floating point answer will interpreted as 0.0
      HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT %s_%s\"",
		    Valid ? ((AnsUsr >= Question->Answer.FloatingPoint[0] &&
		              AnsUsr <= Question->Answer.FloatingPoint[1]) ? "Qst_ANS_OK" :	// Correct
									     "Qst_ANS_BAD") :	// Wrong
			     "Qst_ANS_0",							// Blank answer
		    The_GetSuffix ());
         if (Valid)
	    HTM_Double (AnsUsr);
      HTM_TD_End ();
     }
   else							// If student has omitted the answer
      HTM_TD_Empty (1);
  }

/*****************************************************************************/
/************* Write false / true answer in an exam answer sheet *************/
/*****************************************************************************/

static void ExaAnsShe_WriteOnlineTF_Ans (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd)
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

static void ExaAnsShe_WriteOnlineChoAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd)
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

static void ExaAnsShe_WriteOnlineTxtAns (struct Qst_Question *Question,
					 const struct ExaPrn_Print *Print,
					 unsigned QstInd)
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
   else							// If student has omitted the answer
      HTM_TD_Empty (1);
  }

/*****************************************************************************/
/*************** Write integer answer in an exam answer sheet ****************/
/*****************************************************************************/

static void ExaAnsShe_WritePaperIntAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd)
  {
   long AnsUsr;
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_BLANK;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
      if (sscanf (Print->Qsts[QstInd].Answer.Str,"%ld",&AnsUsr) == 1)
	 WrongOrCorrect = (AnsUsr == Question->Answer.Integer) ? Qst_CORRECT :
								 Qst_WRONG;

   HTM_TD_Begin ("class=\"Exa_ANSWER_INT\"");

      /***** Write input field for the answer *****/
      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
		" class=\"Exa_ANSWER_INPUT_INT %s_%s\" value=\"",
		Id,ExaAnsShe_Class[WrongOrCorrect],The_GetSuffix ());
      if (WrongOrCorrect != Qst_BLANK)
	 HTM_Long (AnsUsr);
      HTM_Char ('"');
      ExaAnsShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/**************** Write float answer in an exam answer sheet *****************/
/*****************************************************************************/

static void ExaAnsShe_WritePaperFltAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd)
  {
   double AnsUsr;
   bool Valid;
   Qst_WrongOrCorrect_t WrongOrCorrect = Qst_BLANK;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Answer.Str[0])	// If the question has been answered
     {
      Valid = Str_GetDoubleFromStr (Print->Qsts[QstInd].Answer.Str,&AnsUsr);
      // A bad formatted floating point answer will interpreted as 0.0
      WrongOrCorrect = Valid ? ((AnsUsr >= Question->Answer.FloatingPoint[0] &&
				 AnsUsr <= Question->Answer.FloatingPoint[1]) ? Qst_CORRECT :
										Qst_WRONG) :
			        Qst_BLANK;
     }

   HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT\"");

      /***** Write input field for the answer *****/
      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
		" class=\"Exa_ANSWER_INPUT_FLOAT %s_%s\" value=\"%s\"",
		Id,ExaAnsShe_Class[WrongOrCorrect],The_GetSuffix (),
		Print->Qsts[QstInd].Answer.Str);
      ExaAnsShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Write false / true answer in an exam answer sheet *************/
/*****************************************************************************/

static void ExaAnsShe_WritePaperTF_Ans (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd)
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
   WrongOrCorrect = AnsUsr ? (AnsUsr == Question->Answer.TF ? Qst_CORRECT :
							      Qst_WRONG) :
			     Qst_BLANK;

   /***** Write paper answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF\"");

      /***** Write selector for the answer *****/
      snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
      HTM_TxtF ("<select id=\"%s\" name=\"Ans\" class=\"%s_%s\"",
		Id,ExaAnsShe_Class[WrongOrCorrect],The_GetSuffix ());
      ExaAnsShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();
	 HTM_OPTION (HTM_Type_STRING,"" ,(AnsUsr == '\0') ? HTM_SELECTED :
							    HTM_NO_ATTR,
		     Txt_NBSP);
	 HTM_OPTION (HTM_Type_STRING,"T",(AnsUsr == 'T' ) ? HTM_SELECTED :
							    HTM_NO_ATTR,
		     Txt_TF_QST[0]);
	 HTM_OPTION (HTM_Type_STRING,"F",(AnsUsr == 'F' ) ? HTM_SELECTED :
							    HTM_NO_ATTR,
		     Txt_TF_QST[1]);
      HTM_Txt ("</select>");

   HTM_TD_End ();
  }

/*****************************************************************************/
/****** Write single or multiple choice answer in an exam answer sheet *******/
/*****************************************************************************/

static void ExaAnsShe_WritePaperChoAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd)
  {
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
		   Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE ? "radio" :
								    "checkbox",
		   Id,NumOpt,Indexes[NumOpt]);
	 if ((UsrAnswers[Indexes[NumOpt]] & HTM_CHECKED))
	    HTM_Txt (" checked");
	 ExaAnsShe_WriteJSToUpdateSheet (Print,QstInd,Id,(int) NumOpt);
	 HTM_ElementEnd ();

      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/**************** Write text answer in an exam answer sheet ******************/
/*****************************************************************************/

static void ExaAnsShe_WritePaperTxtAns (struct Qst_Question *Question,
					const struct ExaPrn_Print *Print,
					unsigned QstInd)
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
		ExaAnsShe_Class[WrongOrCorrect],The_GetSuffix (),
		Print->Qsts[QstInd].Answer.Str);
      ExaAnsShe_WriteJSToUpdateSheet (Print,QstInd,Id,-1);
      HTM_ElementEnd ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/********************** Receive answer to an exam print **********************/
/*****************************************************************************/

static void ExaAnsShe_WriteJSToUpdateSheet (const struct ExaPrn_Print *Print,
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
/******* Receive answer from a student's paper exam filled by teacher ********/
/*****************************************************************************/

void ExaAnsShe_ReceiveAnswer (void)
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
   if (Exams.Exam.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
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
   ExaAnsShe_ShowAnswers (&Session,ExaAnsShe_SOLVED,&Print);
  }

/*****************************************************************************/
/************************* Write head for the answer *************************/
/*****************************************************************************/

static void ExaAnsShe_WriteHead (ExaSes_Modality_t Modality)
  {
   extern const char *Txt_EXAM_ANSWER_TYPES[ExaSes_NUM_MODALITIES];
   static struct
     {
      const char *Icon;
      Ico_Color_t Color;
     } AnswerTypeIcons[ExaSes_NUM_MODALITIES] =
     {
      [ExaSes_NONE  ] = {"check.svg"		,Ico_GREEN},
      [ExaSes_ONLINE] = {"display.svg"	,Ico_BLACK},
      [ExaSes_PAPER ] = {"file-signature.svg",Ico_BLACK},
     };

   HTM_TD_Begin ("class=\"LM DAT_SMALL_%s\"",The_GetSuffix ());
      Ico_PutIconOff (AnswerTypeIcons[Modality].Icon,
		      AnswerTypeIcons[Modality].Color,
		      Txt_EXAM_ANSWER_TYPES[Modality]);
   HTM_TD_End ();
  }
