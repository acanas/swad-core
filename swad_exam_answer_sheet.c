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
static void ExaAnsShe_WriteQst (ExaAnsShe_BlankOrSolved_t BlankOrSolved,
			        const struct ExaPrn_Print *Print,
                                unsigned QstInd,
                                struct Qst_Question *Question);

static void ExaAnsShe_WriteBlankAnswers (struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankIntAns (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankFltAns (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankTF_Ans (__attribute__((unused)) const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankChoAns (const struct Qst_Question *Question);
static void ExaAnsShe_WriteBlankTxtAns (__attribute__((unused)) const struct Qst_Question *Question);

static void ExaAnsShe_WriteSolvedAnswers (struct Qst_Question *Question,
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

static void ExaAnsShe_WriteHead (ExaAnsShe_AnswerType_t AnswerType);

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
   unsigned NumCols;
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
   NumCols = ExaSes_GetParNumCols (Session.NumCols);
   ExaSes_UpdateNumCols (&Session,NumCols);

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

   /***** Show form to select columns *****/
   ExaSes_ShowFormColumns (Session);

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
			false);	// Start/resume

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
   ExaRes_ShowExamResultUser (UsrDat);

   /***** Exam description *****/
   Exa_GetAndWriteDescription (Exams->Exam.ExaCod);

   /***** Show table with answers *****/
   if (Print->NumQsts.All)
      ExaAnsShe_ShowAnswers (Session,BlankOrSolved,Print);

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
	    ExaAnsShe_WriteQst (BlankOrSolved,Print,QstInd,&Question);

	 /* Destroy test question */
	 Qst_QstDestructor (&Question);
	}

   /***** End list of questions *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*** Write a row of an exam answer sheet, with the answer to one question ****/
/*****************************************************************************/

static void ExaAnsShe_WriteQst (ExaAnsShe_BlankOrSolved_t BlankOrSolved,
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
	       ExaAnsShe_WriteSolvedAnswers (Question,Print,QstInd);
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

static void ExaAnsShe_WriteSolvedAnswers (struct Qst_Question *Question,
					  const struct ExaPrn_Print *Print,
					  unsigned QstInd)
  {
   static void (*ExaAnsShe_WriteSolvedAns[ExaAnsShe_NUM_TYPES][Qst_NUM_ANS_TYPES]) (struct Qst_Question *Question,
										    const struct ExaPrn_Print *Print,
										    unsigned QstInd) =
    {
     [ExaAnsShe_CORRECT_ANSWER][Qst_ANS_INT            ] = ExaAnsShe_WriteCorrectIntAns,
     [ExaAnsShe_CORRECT_ANSWER][Qst_ANS_FLOAT          ] = ExaAnsShe_WriteCorrectFltAns,
     [ExaAnsShe_CORRECT_ANSWER][Qst_ANS_TRUE_FALSE     ] = ExaAnsShe_WriteCorrectTF_Ans,
     [ExaAnsShe_CORRECT_ANSWER][Qst_ANS_UNIQUE_CHOICE  ] = ExaAnsShe_WriteCorrectChoAns,
     [ExaAnsShe_CORRECT_ANSWER][Qst_ANS_MULTIPLE_CHOICE] = ExaAnsShe_WriteCorrectChoAns,
     [ExaAnsShe_CORRECT_ANSWER][Qst_ANS_TEXT           ] = ExaAnsShe_WriteCorrectTxtAns,

     [ExaAnsShe_ONLINE_ANSWER ][Qst_ANS_INT            ] = ExaAnsShe_WriteOnlineIntAns,
     [ExaAnsShe_ONLINE_ANSWER ][Qst_ANS_FLOAT          ] = ExaAnsShe_WriteOnlineFltAns,
     [ExaAnsShe_ONLINE_ANSWER ][Qst_ANS_TRUE_FALSE     ] = ExaAnsShe_WriteOnlineTF_Ans,
     [ExaAnsShe_ONLINE_ANSWER ][Qst_ANS_UNIQUE_CHOICE  ] = ExaAnsShe_WriteOnlineChoAns,
     [ExaAnsShe_ONLINE_ANSWER ][Qst_ANS_MULTIPLE_CHOICE] = ExaAnsShe_WriteOnlineChoAns,
     [ExaAnsShe_ONLINE_ANSWER ][Qst_ANS_TEXT           ] = ExaAnsShe_WriteOnlineTxtAns,

     [ExaAnsShe_PAPER_ANSWER  ][Qst_ANS_INT            ] = ExaAnsShe_WritePaperIntAns,
     [ExaAnsShe_PAPER_ANSWER  ][Qst_ANS_FLOAT          ] = ExaAnsShe_WritePaperFltAns,
     [ExaAnsShe_PAPER_ANSWER  ][Qst_ANS_TRUE_FALSE     ] = ExaAnsShe_WritePaperTF_Ans,
     [ExaAnsShe_PAPER_ANSWER  ][Qst_ANS_UNIQUE_CHOICE  ] = ExaAnsShe_WritePaperChoAns,
     [ExaAnsShe_PAPER_ANSWER  ][Qst_ANS_MULTIPLE_CHOICE] = ExaAnsShe_WritePaperChoAns,
     [ExaAnsShe_PAPER_ANSWER  ][Qst_ANS_TEXT           ] = ExaAnsShe_WritePaperTxtAns,
    };
   ExaAnsShe_AnswerType_t AnswerType;

   /***** Begin table *****/
   HTM_TABLE_Begin ("Exa_TBL");

      for (AnswerType  = (ExaAnsShe_AnswerType_t) 0;
	   AnswerType <= (ExaAnsShe_AnswerType_t) (ExaAnsShe_NUM_TYPES - 1);
	   AnswerType++)
        {
	 /***** Write the answer *****/
	 HTM_TR_Begin (NULL);
	    ExaAnsShe_WriteHead (AnswerType);
	    ExaAnsShe_WriteSolvedAns[AnswerType][Question->Answer.Type] (Question,Print,QstInd);
	 HTM_TR_End ();
        }

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
   if (Print->Qsts[QstInd].Ans.Online.Str[0])	// If the question has been answered
     {
      if (sscanf (Print->Qsts[QstInd].Ans.Online.Str,
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
   if (Print->Qsts[QstInd].Ans.Online.Str[0])	// If the question has been answered
     {
      Valid = Str_GetDoubleFromStr (Print->Qsts[QstInd].Ans.Online.Str,
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
   AnsTFStd = Print->Qsts[QstInd].Ans.Online.Str[0];

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
   TstPrn_GetAnswersFromStr (Print->Qsts[QstInd].Ans.Online.Str,UsrAnswers);

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
   if (Print->Qsts[QstInd].Ans.Online.Str[0])	// If the question has been answered
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Print->Qsts[QstInd].Ans.Online.Str,
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
	 HTM_Txt (Print->Qsts[QstInd].Ans.Online.Str);
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
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Ans.Paper.Str[0])	// If the question has been answered
      if (sscanf (Print->Qsts[QstInd].Ans.Paper.Str,"%ld",&AnsUsr) == 1)
	 WrongOrCorrect = (AnsUsr == Question->Answer.Integer) ? Qst_CORRECT :
								  Qst_WRONG;

   HTM_TD_Begin ("class=\"Exa_ANSWER_INT\"");

      /***** Write input field for the answer *****/
      snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
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
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Check if number of rows is correct *****/
   if (Question->Answer.NumOptions != 2)
      Err_WrongAnswerExit ();

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Ans.Paper.Str[0])	// If the question has been answered
     {
      Valid = Str_GetDoubleFromStr (Print->Qsts[QstInd].Ans.Paper.Str,&AnsUsr);
      // A bad formatted floating point answer will interpreted as 0.0
      WrongOrCorrect = Valid ? ((AnsUsr >= Question->Answer.FloatingPoint[0] &&
				 AnsUsr <= Question->Answer.FloatingPoint[1]) ? Qst_CORRECT :
										Qst_WRONG) :
			        Qst_BLANK;
     }

   HTM_TD_Begin ("class=\"Exa_ANSWER_FLOAT\"");

      /***** Write input field for the answer *****/
      snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
      HTM_TxtF ("<input type=\"number\" id=\"%s\" name=\"Ans\""
		" class=\"Exa_ANSWER_INPUT_FLOAT %s_%s\" value=\"%s\"",
		Id,ExaAnsShe_Class[WrongOrCorrect],The_GetSuffix (),
		Print->Qsts[QstInd].Ans.Paper.Str);
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
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Check if number of rows is correct *****/
   Qst_CheckIfNumberOfAnswersIsOne (Question);

   /***** Get answer true or false *****/
   AnsUsr = Print->Qsts[QstInd].Ans.Paper.Str[0];
   WrongOrCorrect = AnsUsr ? (AnsUsr == Question->Answer.TF ? Qst_CORRECT :
							      Qst_WRONG) :
			     Qst_BLANK;

   /***** Write paper answer *****/
   HTM_TD_Begin ("class=\"Exa_ANSWER_TF\"");

      /***** Write selector for the answer *****/
      snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
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
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];

   /***** Get indexes for this question from string *****/
   TstPrn_GetIndexesFromStr (Print->Qsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   TstPrn_GetAnswersFromStr (Print->Qsts[QstInd].Ans.Paper.Str,UsrAnswers);

   /***** Write paper answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Question->Answer.NumOptions;
	NumOpt++)
     {
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE\"");

	 snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
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
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Write paper answer *****/
   if (Print->Qsts[QstInd].Ans.Paper.Str[0])	// If the question has been answered
     {
      /* Filter the user answer */
      Str_Copy (TextAnsUsr,Print->Qsts[QstInd].Ans.Paper.Str,
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
      snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
      HTM_TxtF ("<input type=\"text\" id=\"%s\" name=\"Ans\" maxlength=\"%u\""
		" class=\"Exa_ANSWER_INPUT_TEXT %s_%s\" value=\"%s\"",
		Id,Qst_MAX_CHARS_ANSWERS_ONE_QST,
		ExaAnsShe_Class[WrongOrCorrect],The_GetSuffix (),
		Print->Qsts[QstInd].Ans.Paper.Str);
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
   if (NumOpt < 0)
      HTM_TxtF (" onchange=\"updateExamPrint('examprint','%s','Ans',"
			    "'act=%ld&ses=%s&SesCod=%ld&QstInd=%u',%u);",
		Id,
		Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->SesCod,QstInd,
		(unsigned) Gbl.Prefs.Language);
   else	// NumOpt >= 0
      HTM_TxtF (" onclick=\"updateExamPrint('examprint','%s_%d','Ans',"
		           "'act=%ld&ses=%s&SesCod=%ld&QstInd=%u',%u);",
		Id,NumOpt,
		Act_GetActCod (ActAnsExaPrn),Gbl.Session.Id,Print->SesCod,QstInd,
	        (unsigned) Gbl.Prefs.Language);
   HTM_Txt (" return false;\"");	// return false is necessary to not submit form
  }

/*****************************************************************************/
/************************* Write head for the answer *************************/
/*****************************************************************************/

static void ExaAnsShe_WriteHead (ExaAnsShe_AnswerType_t AnswerType)
  {
   extern const char *Txt_EXAM_ANSWER_TYPES[ExaAnsShe_NUM_TYPES];
   static struct
     {
      const char *Icon;
      Ico_Color_t Color;
     } AnswerTypeIcons[ExaAnsShe_NUM_TYPES] =
     {
      [ExaAnsShe_CORRECT_ANSWER] = {"check.svg"		,Ico_GREEN},
      [ExaAnsShe_ONLINE_ANSWER ] = {"display.svg"	,Ico_BLACK},
      [ExaAnsShe_PAPER_ANSWER  ] = {"file-signature.svg",Ico_BLACK},
     };

   HTM_TD_Begin ("class=\"LM DAT_SMALL_%s\"",The_GetSuffix ());
      Ico_PutIconOff (AnswerTypeIcons[AnswerType].Icon,
		      AnswerTypeIcons[AnswerType].Color,
		      Txt_EXAM_ANSWER_TYPES[AnswerType]);
   HTM_TD_End ();
  }
