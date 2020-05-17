// swad_exam.c: exams

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
#include <float.h>		// For DBL_MAX
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_exam.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_exam_type.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_match.h"
#include "swad_match_result.h"
#include "swad_pagination.h"
#include "swad_role.h"
#include "swad_test.h"
#include "swad_test_visibility.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Exa_MAX_CHARS_ANSWER	(1024 - 1)	// 1023
#define Exa_MAX_BYTES_ANSWER	((Exa_MAX_CHARS_ANSWER + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Exa_MAX_ANSWERS_PER_QUESTION	10

/* Score range [0...max.score]
   will be converted to
   grade range [0...max.grade]
   Example: Exam with 5 questions, unique-choice, 4 options per question
            max.score = 5 *   1     =  5
            min.score = 5 * (-0.33) = -1,67
            max.grade given by teacher = 0.2 ==> min.grade = -0,067

              grade
                ^
                |          /
   max.grade--> +---------+
                |        /|
                |       / |
                |      /  |
                |     /   |
                |    /    |
                |   /     |
                |  /      |
                | /       |
                |/        |
    ------+---0-+---------+---------> score
          ^    /0         ^
     min.score/ |      max.score
          |  /  |   (num.questions)
          | /   |
          |/    |
          +-----+ <--min.grade
         /      |
*/
#define Exa_MAX_GRADE_DEFAULT 1.0

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Exa_ListAllExams (struct Exa_Exams *Exams);
static void Exa_PutIconsListExams (void *Exams);
static void Exa_PutIconToCreateNewExam (struct Exa_Exams *Exams);
static void Exa_PutButtonToCreateNewExam (struct Exa_Exams *Exams);
static void Exa_PutParamsToCreateNewExam (void *Exams);

static void Exa_ShowOneExam (struct Exa_Exams *Exams,
                             struct Exa_Exam *Exam,bool ShowOnlyThisExam);

static void Exa_PutIconToShowResultsOfExam (void *Exams);
static void Exa_WriteAuthor (struct Exa_Exam *Exam);

static void Exa_PutHiddenParamExamOrder (Exa_Order_t SelectedOrder);

static void Exa_PutIconsToRemEditOneExam (struct Exa_Exams *Exams,
					  const struct Exa_Exam *Exam,
					  const char *Anchor);

static void Exa_PutHiddenParamOrder (Exa_Order_t SelectedOrder);
static Exa_Order_t Exa_GetParamOrder (void);

static void Exa_RemoveExamFromAllTables (long ExaCod);

static bool Exa_CheckIfSimilarExamExists (const struct Exa_Exam *Exam);

static void Exa_ReceiveExamFieldsFromForm (struct Exa_Exam *Exam,
				           char Txt[Cns_MAX_BYTES_TEXT + 1]);
static bool Exa_CheckExamFieldsReceivedFromForm (const struct Exa_Exam *Exam);

static void Exa_CreateExam (struct Exa_Exam *Exam,const char *Txt);
static void Exa_UpdateExam (struct Exa_Exam *Exam,const char *Txt);

/*****************************************************************************/
/******************************* Reset exams *********************************/
/*****************************************************************************/

void Exa_ResetExams (struct Exa_Exams *Exams)
  {
   Exams->LstIsRead         = false;	// List not read from database...
   Exams->Num               = 0;	// Total number of exams
   Exams->NumSelected       = 0;	// Number of exams selected
   Exams->Lst               = NULL;	// List of exams
   Exams->SelectedOrder     = Exa_ORDER_DEFAULT;
   Exams->CurrentPage       = 0;
   Exams->ListQuestions     = NULL;
   Exams->ExaCodsSelected   = NULL;	// String with selected exam codes separated by separator multiple
   Exams->ExaCod            = -1L;	// Selected/current exam code
   Exams->SesCod            = -1L;	// Selected/current session code
   Exams->SetInd            = 0;	// Current set index
   Exams->QstCod            = -1L;	// Current question code
  }

/*****************************************************************************/
/*************************** Initialize exam to empty ************************/
/*****************************************************************************/

void Exa_ResetExam (struct Exa_Exam *Exam)
  {
   /***** Initialize to empty exam *****/
   Exam->ExaCod                  = -1L;
   Exam->CrsCod                  = -1L;
   Exam->UsrCod                  = -1L;
   Exam->MaxGrade                = Exa_MAX_GRADE_DEFAULT;
   Exam->Visibility              = TstVis_VISIBILITY_DEFAULT;
   Exam->TimeUTC[Dat_START_TIME] = (time_t) 0;
   Exam->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Exam->Title[0]                = '\0';
   Exam->Hidden                  = false;
   Exam->NumSets                 = 0;
   Exam->NumQsts                 = 0;
   Exam->NumSess                 = 0;
   Exam->NumOpenSess             = 0;
  }

/*****************************************************************************/
/***************************** List all exams ********************************/
/*****************************************************************************/

void Exa_SeeAllExams (void)
  {
   struct Exa_Exams Exams;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);

   /***** Show all exams *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/**************************** Show all the exams *****************************/
/*****************************************************************************/

static void Exa_ListAllExams (struct Exa_Exams *Exams)
  {
   extern const char *Hlp_ASSESSMENT_Exams;
   extern const char *Txt_Exams;
   extern const char *Txt_EXAMS_ORDER_HELP[Exa_NUM_ORDERS];
   extern const char *Txt_EXAMS_ORDER[Exa_NUM_ORDERS];
   extern const char *Txt_Sessions;
   extern const char *Txt_No_exams;
   Exa_Order_t Order;
   struct Pagination Pagination;
   unsigned NumExam;
   struct Exa_Exam Exam;

   /***** Get number of groups in current course *****/
   if (!Gbl.Crs.Grps.NumGrps)
      Gbl.Crs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of exams *****/
   Exa_GetListExams (Exams,Exams->SelectedOrder);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Exams->Num;
   Pagination.CurrentPage = (int) Exams->CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Exams->CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Exams,
                 Exa_PutIconsListExams,Exams,
                 Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);

   /***** Write links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_EXAMS,&Pagination,
				  Exams,-1L);

   if (Exams->Num)
     {
      /***** Table head *****/
      HTM_TABLE_BeginWideMarginPadding (5);
      HTM_TR_Begin (NULL);
      if (Exa_CheckIfICanEditExams ())
         HTM_TH (1,1,"CONTEXT_COL",NULL);	// Column for contextual icons

      for (Order  = (Exa_Order_t) 0;
	   Order <= (Exa_Order_t) (Exa_NUM_ORDERS - 1);
	   Order++)
	{
	 HTM_TH_Begin (1,1,"LM");

	 /* Form to change order */
	 Frm_StartForm (ActSeeAllExa);
	 Pag_PutHiddenParamPagNum (Pag_EXAMS,Exams->CurrentPage);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_EXAMS_ORDER_HELP[Order],"BT_LINK TIT_TBL",NULL);
	 if (Order == Exams->SelectedOrder)
	    HTM_U_Begin ();
	 HTM_Txt (Txt_EXAMS_ORDER[Order]);
	 if (Order == Exams->SelectedOrder)
	    HTM_U_End ();
	 HTM_BUTTON_End ();
	 Frm_EndForm ();

	 HTM_TH_End ();
	}

      HTM_TH (1,1,"RM",Txt_Sessions);

      HTM_TR_End ();

      /***** Write all exams *****/
      for (NumExam  = Pagination.FirstItemVisible;
	   NumExam <= Pagination.LastItemVisible;
	   NumExam++)
	{
	 /* Get data of this exam */
	 Exam.ExaCod = Exams->Lst[NumExam - 1].ExaCod;
	 Exa_GetDataOfExamByCod (&Exam);
         Exams->ExaCod = Exam.ExaCod;

	 /* Show exam */
	 Exa_ShowOneExam (Exams,
	                  &Exam,
	                  false);	// Do not show only this exam
	}

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No exams created
      Ale_ShowAlert (Ale_INFO,Txt_No_exams);

   /***** Write again links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_EXAMS,&Pagination,
				  Exams,-1L);

   /***** Button to create a new exam *****/
   if (Exa_CheckIfICanEditExams ())
      Exa_PutButtonToCreateNewExam (Exams);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of exams *****/
   Exa_FreeListExams (Exams);
  }

/*****************************************************************************/
/************************ Check if I can edit exams **************************/
/*****************************************************************************/

bool Exa_CheckIfICanEditExams (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/***************** Put contextual icons in list of exams *******************/
/*****************************************************************************/

static void Exa_PutIconsListExams (void *Exams)
  {
   if (Exams)
     {
      /***** Put icon to create a new exam *****/
      if (Exa_CheckIfICanEditExams ())
	 Exa_PutIconToCreateNewExam ((struct Exa_Exams *) Exams);

      /***** Put icon to view sessions results *****/
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:
	    Ico_PutContextualIconToShowResults (ActSeeMyExaEvtResCrs,NULL,
	                                        NULL,NULL);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Ico_PutContextualIconToShowResults (ActReqSeeAllExaEvtRes,NULL,
	                                        NULL,NULL);
	    break;
	 default:
	    break;
	}

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_EXAMS);
     }
  }

/*****************************************************************************/
/********************** Put icon to create a new exam **********************/
/*****************************************************************************/

static void Exa_PutIconToCreateNewExam (struct Exa_Exams *Exams)
  {
   extern const char *Txt_New_exam;

   Ico_PutContextualIconToAdd (ActFrmNewExa,NULL,
                               Exa_PutParamsToCreateNewExam,Exams,
			       Txt_New_exam);
  }

/*****************************************************************************/
/********************* Put button to create a new exam *********************/
/*****************************************************************************/

static void Exa_PutButtonToCreateNewExam (struct Exa_Exams *Exams)
  {
   extern const char *Txt_New_exam;

   Frm_StartForm (ActFrmNewExa);
   Exa_PutParamsToCreateNewExam (Exams);
   Btn_PutConfirmButton (Txt_New_exam);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Put parameters to create a new exam *******************/
/*****************************************************************************/

static void Exa_PutParamsToCreateNewExam (void *Exams)
  {
   if (Exams)
     {
      Exa_PutHiddenParamExamOrder (((struct Exa_Exams *) Exams)->SelectedOrder);
      Pag_PutHiddenParamPagNum (Pag_EXAMS,((struct Exa_Exams *) Exams)->CurrentPage);
     }
  }

/*****************************************************************************/
/****************************** Show one exam ******************************/
/*****************************************************************************/

void Exa_SeeOneExam (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Show exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,&Session,
	                false);	// Do not put form for session
  }

/*****************************************************************************/
/******************************* Show one exam *******************************/
/*****************************************************************************/

void Exa_ShowOnlyOneExam (struct Exa_Exams *Exams,
			  struct Exa_Exam *Exam,
			  struct ExaSes_Session *Session,
			  bool PutFormSession)
  {
   Exa_ShowOnlyOneExamBegin (Exams,Exam,Session,PutFormSession);
   Exa_ShowOnlyOneExamEnd ();
  }

void Exa_ShowOnlyOneExamBegin (struct Exa_Exams *Exams,
			       struct Exa_Exam *Exam,
			       struct ExaSes_Session *Session,
			       bool PutFormSession)
  {
   extern const char *Hlp_ASSESSMENT_Exams;
   extern const char *Txt_Exam;

   /***** Begin box *****/
   Exams->ExaCod = Exam->ExaCod;
   Box_BoxBegin (NULL,Txt_Exam,
                 Exa_PutIconToShowResultsOfExam,Exams,
		 Hlp_ASSESSMENT_Exams,Box_NOT_CLOSABLE);

   /***** Show exam *****/
   Exa_ShowOneExam (Exams,
                    Exam,
		    true);	// Show only this exam

   /***** List sessions *****/
   ExaSes_ListSessions (Exams,Exam,Session,PutFormSession);
  }

void Exa_ShowOnlyOneExamEnd (void)
  {
   /***** End box *****/
   Box_BoxEnd ();
  }

static void Exa_ShowOneExam (struct Exa_Exams *Exams,
                             struct Exa_Exam *Exam,bool ShowOnlyThisExam)
  {
   extern const char *Txt_View_exam;
   extern const char *Txt_Sets_of_questions;
   extern const char *Txt_Maximum_grade;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Sessions;
   char *Anchor;
   static unsigned UniqueId = 0;
   char *Id;
   Dat_StartEndTime_t StartEndTime;
   const char *Color;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Build anchor string *****/
   Frm_SetAnchorStr (Exam->ExaCod,&Anchor);

   /***** Begin box and table *****/
   if (ShowOnlyThisExam)
      HTM_TABLE_BeginWidePadding (2);

   /***** Start first row of this exam *****/
   HTM_TR_Begin (NULL);

   /***** Icons related to this exam *****/
   if (Exa_CheckIfICanEditExams ())
     {
      if (ShowOnlyThisExam)
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL\"");
      else
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL COLOR%u\"",Gbl.RowEvenOdd);

      /* Icons to remove/edit this exam */
      Exa_PutIconsToRemEditOneExam (Exams,Exam,Anchor);

      HTM_TD_End ();
     }

   /***** Start/end date/time *****/
   UniqueId++;
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"exa_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      Color = Exam->NumOpenSess ? (Exam->Hidden ? "DATE_GREEN_LIGHT":
						  "DATE_GREEN") :
				  (Exam->Hidden ? "DATE_RED_LIGHT":
						  "DATE_RED");
      if (ShowOnlyThisExam)
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT\"",
		       Id,Color);
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		       Id,Color,Gbl.RowEvenOdd);
      if (Exam->TimeUTC[Dat_START_TIME])
	 Dat_WriteLocalDateHMSFromUTC (Id,Exam->TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,true,0x6);
      HTM_TD_End ();
      free (Id);
     }

   /***** Exam title and main data *****/
   if (ShowOnlyThisExam)
      HTM_TD_Begin ("class=\"LT\"");
   else
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Exam title */
   Exams->ExaCod = Exam->ExaCod;
   HTM_ARTICLE_Begin (Anchor);
   Frm_StartForm (ActSeeExa);
   Exa_PutParams (Exams);
   HTM_BUTTON_SUBMIT_Begin (Txt_View_exam,
			    Exam->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT":
					   "BT_LINK LT ASG_TITLE",
			    NULL);
   HTM_Txt (Exam->Title);
   HTM_BUTTON_End ();
   Frm_EndForm ();
   HTM_ARTICLE_End ();

   /* Number of questions, maximum grade, visibility of results */
   HTM_DIV_Begin ("class=\"%s\"",Exam->Hidden ? "ASG_GRP_LIGHT" :
        	                                "ASG_GRP");
   HTM_TxtColonNBSP (Txt_Sets_of_questions);
   HTM_Unsigned (Exam->NumSets);
   HTM_BR ();
   HTM_TxtColonNBSP (Txt_Maximum_grade);
   HTM_Double (Exam->MaxGrade);
   HTM_BR ();
   HTM_TxtColonNBSP (Txt_Result_visibility);
   TstVis_ShowVisibilityIcons (Exam->Visibility,Exam->Hidden);
   HTM_DIV_End ();

   /***** Number of sessions in exam *****/
   if (ShowOnlyThisExam)
      HTM_TD_Begin ("class=\"RT\"");
   else
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);

   Exams->ExaCod = Exam->ExaCod;
   Frm_StartForm (ActSeeExa);
   Exa_PutParams (Exams);
   HTM_BUTTON_SUBMIT_Begin (Txt_Sessions,
			    Exam->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT" :
				           "BT_LINK LT ASG_TITLE",
			    NULL);
   if (ShowOnlyThisExam)
      HTM_TxtColonNBSP (Txt_Sessions);
   HTM_Unsigned (Exam->NumSess);
   HTM_BUTTON_End ();
   Frm_EndForm ();

   HTM_TD_End ();

   /***** End 1st row of this exam *****/
   HTM_TR_End ();

   /***** Start 2nd row of this exam *****/
   HTM_TR_Begin (NULL);

   /***** Author of the exam *****/
   if (ShowOnlyThisExam)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Exa_WriteAuthor (Exam);
   HTM_TD_End ();

   /***** Text of the exam *****/
   if (ShowOnlyThisExam)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Exa_GetExamTxtFromDB (Exam->ExaCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   HTM_DIV_Begin ("class=\"PAR %s\"",Exam->Hidden ? "DAT_LIGHT" :
        	                                    "DAT");
   HTM_Txt (Txt);
   HTM_DIV_End ();
   HTM_TD_End ();

   /***** End 2nd row of this exam *****/
   HTM_TR_End ();

   /***** End table *****/
   if (ShowOnlyThisExam)
      HTM_TABLE_End ();
   else
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
  }

/*****************************************************************************/
/************ Put icon to show results of sessions in an exam ****************/
/*****************************************************************************/

static void Exa_PutIconToShowResultsOfExam (void *Exams)
  {
   if (Exams)
     {
      /***** Put icon to view sessions results *****/
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:
	    Ico_PutContextualIconToShowResults (ActSeeMyExaEvtResExa,ExaRes_RESULTS_BOX_ID,
						Exa_PutParams,Exams);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Ico_PutContextualIconToShowResults (ActSeeAllExaEvtResExa,ExaRes_RESULTS_BOX_ID,
						Exa_PutParams,Exams);
	    break;
	 default:
	    break;
	}
     }
  }

/*****************************************************************************/
/*********************** Write the author of an exam ************************/
/*****************************************************************************/

static void Exa_WriteAuthor (struct Exa_Exam *Exam)
  {
   Usr_WriteAuthor1Line (Exam->UsrCod,Exam->Hidden);
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of exams *******/
/*****************************************************************************/

static void Exa_PutHiddenParamExamOrder (Exa_Order_t SelectedOrder)
  {
   Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) SelectedOrder);
  }

/*****************************************************************************/
/******************** Put a link (form) to edit one exam *********************/
/*****************************************************************************/

static void Exa_PutIconsToRemEditOneExam (struct Exa_Exams *Exams,
					  const struct Exa_Exam *Exam,
					  const char *Anchor)
  {
   Exams->ExaCod = Exam->ExaCod;

   /***** Icon to remove exam *****/
   Ico_PutContextualIconToRemove (ActReqRemExa,
                                  Exa_PutParams,Exams);

   /***** Icon to unhide/hide exam *****/
   if (Exam->Hidden)
      Ico_PutContextualIconToUnhide (ActShoExa,Anchor,
                                     Exa_PutParams,Exams);
   else
      Ico_PutContextualIconToHide (ActHidExa,Anchor,
                                   Exa_PutParams,Exams);

   /***** Icon to edit exam *****/
   Ico_PutContextualIconToEdit (ActEdiOneExa,NULL,
                                Exa_PutParams,Exams);
  }

/*****************************************************************************/
/*********************** Params used to edit an exam **************************/
/*****************************************************************************/

void Exa_PutParams (void *Exams)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Exams)
     {
      if (((struct Exa_Exams *) Exams)->ExaCod > 0)
	 Exa_PutParamExamCod (((struct Exa_Exams *) Exams)->ExaCod);
      Exa_PutHiddenParamOrder (((struct Exa_Exams *) Exams)->SelectedOrder);
      WhichGroups = Grp_GetParamWhichGroups ();
      Grp_PutParamWhichGroups (&WhichGroups);
      Pag_PutHiddenParamPagNum (Pag_EXAMS,((struct Exa_Exams *) Exams)->CurrentPage);
     }
  }

/*****************************************************************************/
/******************** Write parameter with code of exam **********************/
/*****************************************************************************/

void Exa_PutParamExamCod (long ExaCod)
  {
   Par_PutHiddenParamLong (NULL,"ExaCod",ExaCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of exam ***********************/
/*****************************************************************************/

long Exa_GetParamExamCod (void)
  {
   /***** Get code of exam *****/
   return Par_GetParToLong ("ExaCod");
  }

/*****************************************************************************/
/******************* Get parameters used to edit an exam **********************/
/*****************************************************************************/

void Exa_GetParams (struct Exa_Exams *Exams)
  {
   /***** Get other parameters *****/
   Exams->SelectedOrder = Exa_GetParamOrder ();
   Exams->CurrentPage = Pag_GetParamPagNum (Pag_EXAMS);

   /***** Get exam code *****/
   Exams->ExaCod = Exa_GetParamExamCod ();
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of exams *******/
/*****************************************************************************/

static void Exa_PutHiddenParamOrder (Exa_Order_t SelectedOrder)
  {
   if (SelectedOrder != Exa_ORDER_DEFAULT)
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) SelectedOrder);
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of exams ************/
/*****************************************************************************/

static Exa_Order_t Exa_GetParamOrder (void)
  {
   return (Exa_Order_t) Par_GetParToUnsignedLong ("Order",
						  0,
						  Exa_NUM_ORDERS - 1,
						  (unsigned long) Exa_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*********************** Get list of all the exams *************************/
/*****************************************************************************/

void Exa_GetListExams (struct Exa_Exams *Exams,Exa_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Exa_NUM_ORDERS] =
     {
      [Exa_ORDER_BY_START_DATE] = "StartTime DESC,EndTime DESC,exa_exams.Title DESC",
      [Exa_ORDER_BY_END_DATE  ] = "EndTime DESC,StartTime DESC,exa_exams.Title DESC",
      [Exa_ORDER_BY_TITLE     ] = "exa_exams.Title",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char *HiddenSubQuery;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned NumExam;

   /***** Free list of exams *****/
   if (Exams->LstIsRead)
      Exa_FreeListExams (Exams);

   /***** Subquery: get hidden exams depending on user's role *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
         if (asprintf (&HiddenSubQuery," AND exa_exams.Hidden='N'") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 if (asprintf (&HiddenSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      default:
	 Rol_WrongRoleExit ();
	 break;
     }

   /***** Get list of exams from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get exams",
			     "SELECT exa_exams.ExaCod,"				// row[0]
			            "MIN(exa_sessions.StartTime) AS StartTime,"	// row[1]
			            "MAX(exa_sessions.EndTime) AS EndTime"	// row[2]
			     " FROM exa_exams"
			     " LEFT JOIN exa_sessions"
			     " ON exa_exams.ExaCod=exa_sessions.ExaCod"
			     " WHERE exa_exams.CrsCod=%ld"
			     "%s"
			     " GROUP BY exa_exams.ExaCod"
			     " ORDER BY %s",
			     Gbl.Hierarchy.Crs.CrsCod,
			     HiddenSubQuery,
			     OrderBySubQuery[SelectedOrder]);

   /***** Free allocated memory for subquery *****/
   free (HiddenSubQuery);

   if (NumRows) // Exams found...
     {
      Exams->Num = (unsigned) NumRows;

      /***** Create list of exams *****/
      if ((Exams->Lst = (struct Exa_ExamSelected *) malloc (NumRows * sizeof (struct Exa_ExamSelected))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the exams codes *****/
      for (NumExam = 0;
	   NumExam < Exams->Num;
	   NumExam++)
        {
         /* Get next exam code (row[0]) */
         row = mysql_fetch_row (mysql_res);
         if ((Exams->Lst[NumExam].ExaCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Lay_ShowErrorAndExit ("Error: wrong exam code.");
        }
     }
   else
      Exams->Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Exams->LstIsRead = true;
  }

/*****************************************************************************/
/******************** Get list of exam sessions selected *********************/
/*****************************************************************************/

void Exa_GetListSelectedExaCods (struct Exa_Exams *Exams)
  {
   unsigned MaxSizeListExaCodsSelected;
   unsigned NumExam;
   const char *Ptr;
   long ExaCod;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Allocate memory for list of exams selected *****/
   MaxSizeListExaCodsSelected = Exams->Num * (Cns_MAX_DECIMAL_DIGITS_LONG + 1);
   if ((Exams->ExaCodsSelected = (char *) malloc (MaxSizeListExaCodsSelected + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Get parameter multiple with list of exams selected *****/
   Par_GetParMultiToText ("ExaCod",Exams->ExaCodsSelected,MaxSizeListExaCodsSelected);

   /***** Set which exams will be shown as selected (checkboxes on) *****/
   if (Exams->ExaCodsSelected[0])	// Some exams selected
     {
      /* Reset selection */
      for (NumExam = 0;
	   NumExam < Exams->Num;
	   NumExam++)
	 Exams->Lst[NumExam].Selected = false;
      Exams->NumSelected = 0;

      /* Set some exams as selected */
      for (Ptr = Exams->ExaCodsSelected;
	   *Ptr;
	   )
	{
	 /* Get next exam selected */
	 Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 ExaCod = Str_ConvertStrCodToLongCod (LongStr);

	 /* Set each exam in *StrExaCodsSelected as selected */
	 for (NumExam = 0;
	      NumExam < Exams->Num;
	      NumExam++)
	    if (Exams->Lst[NumExam].ExaCod == ExaCod)
	      {
	       Exams->Lst[NumExam].Selected = true;
	       Exams->NumSelected++;
	       break;
	      }
	}
     }
   else					// No exams selected
     {
      /***** Set all exams as selected *****/
      for (NumExam = 0;
	   NumExam < Exams->Num;
	   NumExam++)
	 Exams->Lst[NumExam].Selected = true;
      Exams->NumSelected = Exams->Num;
     }
  }

/*****************************************************************************/
/********************** Get exam data using its code *************************/
/*****************************************************************************/

void Exa_GetDataOfExamByCod (struct Exa_Exam *Exam)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Trivial check *****/
   if (Exam->ExaCod <= 0)
     {
      /* Initialize to empty exam */
      Exa_ResetExam (Exam);
      return;
     }

   /***** Get exam data from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get exam data",
			     "SELECT exa_exams.ExaCod,"		// row[0]
			            "exa_exams.CrsCod,"		// row[1]
			            "exa_exams.Hidden,"		// row[2]
			            "exa_exams.UsrCod,"		// row[3]
			            "exa_exams.MaxGrade,"	// row[4]
			            "exa_exams.Visibility,"	// row[5]
			            "exa_exams.Title"		// row[6]
			     " FROM exa_exams"
			     " LEFT JOIN exa_sessions"
			     " ON exa_exams.ExaCod=exa_sessions.ExaCod"
			     " WHERE exa_exams.ExaCod=%ld",
			     Exam->ExaCod);
   if (NumRows) // Exam found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the exam (row[0]) */
      Exam->ExaCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get code of the course (row[1]) */
      Exam->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get whether the exam is hidden (row[2]) */
      Exam->Hidden = (row[2][0] == 'Y');

      /* Get author of the exam (row[3]) */
      Exam->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

      /* Get maximum grade (row[4]) */
      Exam->MaxGrade = Str_GetDoubleFromStr (row[4]);
      if (Exam->MaxGrade < 0.0)	// Only positive values allowed
	 Exam->MaxGrade = 0.0;

      /* Get visibility (row[5]) */
      Exam->Visibility = TstVis_GetVisibilityFromStr (row[5]);

      /* Get the title of the exam (row[6]) */
      Str_Copy (Exam->Title,row[6],
                Exa_MAX_BYTES_TITLE);

      /* Get number of sets */
      Exam->NumSets = ExaSet_GetNumSetsExam (Exam->ExaCod);

      /* Get number of questions */
      Exam->NumQsts = ExaSet_GetNumQstsExam (Exam->ExaCod);

      /* Get number of sessions */
      Exam->NumSess = ExaSes_GetNumSessionsInExam (Exam->ExaCod);

      /* Get number of open sessions */
      Exam->NumOpenSess = ExaSes_GetNumOpenSessionsInExam (Exam->ExaCod);
     }
   else
      /* Initialize to empty exam */
      Exa_ResetExam (Exam);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   if (Exam->ExaCod > 0)
     {
      /***** Get start and end times from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get exam data",
				"SELECT UNIX_TIMESTAMP(MIN(StartTime)),"	// row[0]
				       "UNIX_TIMESTAMP(MAX(EndTime))"		// row[1]
				" FROM exa_sessions"
				" WHERE ExaCod=%ld",
				Exam->ExaCod);
      if (NumRows)
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get start date (row[0] holds the start UTC time) */
	 Exam->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[0]);

	 /* Get end   date (row[1] holds the end   UTC time) */
	 Exam->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[1]);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }
   else
     {
      Exam->TimeUTC[Dat_START_TIME] =
      Exam->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
     }
  }

/*****************************************************************************/
/***************************** Free list of exams ****************************/
/*****************************************************************************/

void Exa_FreeListExams (struct Exa_Exams *Exams)
  {
   if (Exams->LstIsRead && Exams->Lst)
     {
      /***** Free memory used by the list of exams *****/
      free (Exams->Lst);
      Exams->Lst       = NULL;
      Exams->Num       = 0;
      Exams->LstIsRead = false;
     }
  }

/*****************************************************************************/
/********************** Get exam text from database ************************/
/*****************************************************************************/

void Exa_GetExamTxtFromDB (long ExaCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of exam from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get exam text",
			     "SELECT Txt FROM exa_exams WHERE ExaCod=%ld",
			     ExaCod);

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Txt,row[0],
                Cns_MAX_BYTES_TEXT);
     }
   else
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting exam text.");
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing of an exam ******************/
/*****************************************************************************/

void Exa_AskRemExam (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_exam_X;
   extern const char *Txt_Remove_exam;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Show question and button to remove exam *****/
   Exams.ExaCod = Exam.ExaCod;
   Ale_ShowAlertAndButton (ActRemExa,NULL,NULL,
                           Exa_PutParams,&Exams,
			   Btn_REMOVE_BUTTON,Txt_Remove_exam,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_exam_X,
                           Exam.Title);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/******************************* Remove an exam *******************************/
/*****************************************************************************/

void Exa_RemoveExam (void)
  {
   extern const char *Txt_Exam_X_removed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);

   /***** Get exam code *****/
   if ((Exam.ExaCod = Exa_GetParamExamCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of exam is missing.");

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Remove exam from all tables *****/
   Exa_RemoveExamFromAllTables (Exam.ExaCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Exam_X_removed,
                  Exam.Title);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/*********************** Remove exam from all tables *************************/
/*****************************************************************************/

static void Exa_RemoveExamFromAllTables (long ExaCod)
  {
   /***** Remove all sessions in this exam *****/
   ExaSes_RemoveSessionsInExamFromAllTables (ExaCod);

   /***** Remove exam questions *****/
   DB_QueryDELETE ("can not remove exam questions",
		   "DELETE FROM exa_set_questions"
		   " USING exa_sets,exa_set_questions"
		   " WHERE exa_sets.ExaCod=%ld"
		   " AND exa_sets.SetCod=exa_set_questions.SetCod",
		   ExaCod);

   /***** Remove exam sets *****/
   DB_QueryDELETE ("can not remove exam sets",
		   "DELETE FROM exa_sets"
		   " WHERE ExaCod=%ld",
		   ExaCod);

   /***** Remove exam *****/
   DB_QueryDELETE ("can not remove exam",
		   "DELETE FROM exa_exams WHERE ExaCod=%ld",
		   ExaCod);
  }

/*****************************************************************************/
/******************** Remove all the exams of a course ***********************/
/*****************************************************************************/

void Exa_RemoveExamsCrs (long CrsCod)
  {
   /***** Remove all sessions in this course *****/
   ExaSes_RemoveSessionInCourseFromAllTables (CrsCod);

   /***** Remove the questions in exams *****/
   DB_QueryDELETE ("can not remove questions in course exams",
		   "DELETE FROM exa_set_questions"
		   " USING exa_exams,exa_sets,exa_set_questions"
		   " WHERE exa_exams.CrsCod=%ld"
		   " AND exa_exams.ExaCod=exa_sets.ExaCod",
		   " AND exa_sets.SetCod=exa_set_questions.SetCod",
                   CrsCod);

   /***** Remove the sets in exams *****/
   DB_QueryDELETE ("can not remove sets in course exams",
		   "DELETE FROM exa_sets"
		   " USING exa_exams,exa_sets"
		   " WHERE exa_exams.CrsCod=%ld"
		   " AND exa_exams.ExaCod=exa_sets.ExaCod",
                   CrsCod);

   /***** Remove the exams *****/
   DB_QueryDELETE ("can not remove course exams",
		   "DELETE FROM exa_exams"
		   " WHERE CrsCod=%ld",
                   CrsCod);
  }

/*****************************************************************************/
/******************************** Hide an exam ******************************/
/*****************************************************************************/

void Exa_HideExam (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Hide exam *****/
   DB_QueryUPDATE ("can not hide exam",
		   "UPDATE exa_exams SET Hidden='Y' WHERE ExaCod=%ld",
		   Exam.ExaCod);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/******************************* Unhide an exam ******************************/
/*****************************************************************************/

void Exa_UnhideExam (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Unhide exam *****/
   DB_QueryUPDATE ("can not unhide exam",
		   "UPDATE exa_exams SET Hidden='N' WHERE ExaCod=%ld",
		   Exam.ExaCod);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/******************* Check if the title of an exam exists *******************/
/*****************************************************************************/

static bool Exa_CheckIfSimilarExamExists (const struct Exa_Exam *Exam)
  {
   /***** Get number of exams with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar exams",
			  "SELECT COUNT(*) FROM exa_exams"
			  " WHERE CrsCod=%ld AND Title='%s'"
			  " AND ExaCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,Exam->Title,
			  Exam->ExaCod) != 0);
  }

/*****************************************************************************/
/**************** Request the creation or edition of an exam ******************/
/*****************************************************************************/

void Exa_RequestCreatOrEditExam (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   bool ItsANewExam;

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   Exam.ExaCod = Exams.ExaCod;
   ItsANewExam = (Exam.ExaCod <= 0);

   /***** Get exam data *****/
   if (ItsANewExam)
      /* Initialize to empty exam */
      Exa_ResetExam (&Exam);
   else
     {
      /* Get exam data from database */
      Exa_GetDataOfExamByCod (&Exam);
      Exams.ExaCod = Exam.ExaCod;
     }

   /***** Put form to create/edit an exam and show sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,ItsANewExam);
  }

/*****************************************************************************/
/******************** Put forms to create/edit an exam ***********************/
/*****************************************************************************/

void Exa_PutFormsOneExam (struct Exa_Exams *Exams,
			  struct Exa_Exam *Exam,
			  struct ExaSet_Set *Set,
			  bool ItsANewExam)
  {
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Initialize text / get text from database *****/
   if (ItsANewExam)
      Txt[0] = '\0';
   else
      Exa_GetExamTxtFromDB (Exam->ExaCod,Txt);

   /***** Put form to create/edit an exam *****/
   Exa_PutFormEditionExam (Exams,Exam,Txt,ItsANewExam);

   /***** Show other lists *****/
   if (ItsANewExam)
      /* Show exams again */
      Exa_ListAllExams (Exams);
   else
      /* Show list of sets */
      ExaSet_ListExamSets (Exams,Exam,Set);
  }

/*****************************************************************************/
/********************* Put a form to create/edit an exam **********************/
/*****************************************************************************/

void Exa_PutFormEditionExam (struct Exa_Exams *Exams,
			     struct Exa_Exam *Exam,
			     char Txt[Cns_MAX_BYTES_TEXT + 1],
			     bool ItsANewExam)
  {
   extern const char *Hlp_ASSESSMENT_Exams_new_exam;
   extern const char *Hlp_ASSESSMENT_Exams_edit_exam;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_exam;
   extern const char *Txt_Edit_exam;
   extern const char *Txt_Title;
   extern const char *Txt_Maximum_grade;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Description;
   extern const char *Txt_Create_exam;
   extern const char *Txt_Save_changes;

   /***** Begin form *****/
   Exams->ExaCod = Exam->ExaCod;
   Frm_StartForm (ItsANewExam ? ActNewExa :
				ActChgExa);
   Exa_PutParams (Exams);

   /***** Begin box and table *****/
   if (ItsANewExam)
      Box_BoxTableBegin (NULL,Txt_New_exam,
                         NULL,NULL,
			 Hlp_ASSESSMENT_Exams_new_exam,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,
			 Exam->Title[0] ? Exam->Title :
					  Txt_Edit_exam,
			 NULL,NULL,
			 Hlp_ASSESSMENT_Exams_edit_exam,Box_NOT_CLOSABLE,2);

   /***** Exam title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_INPUT_TEXT ("Title",Exa_MAX_CHARS_TITLE,Exam->Title,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "id=\"Title\" required=\"required\""
		   " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Maximum grade *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Maximum_grade);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_FLOAT ("MaxGrade",0.0,DBL_MAX,0.01,Exam->MaxGrade,false,
		    "required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Visibility of results *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Result_visibility);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LB\"");
   TstVis_PutVisibilityCheckboxes (Exam->Visibility);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Exam text *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Txt",Txt_Description);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"5\""
	               " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_Txt (Txt);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   if (ItsANewExam)
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_exam);
   else
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************** Receive form to create a new exam ********************/
/*****************************************************************************/

void Exa_ReceiveFormExam (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   bool ItsANewExam;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   Exam.ExaCod = Exams.ExaCod;
   ItsANewExam = (Exam.ExaCod <= 0);

   /***** Get all current exam data from database *****/
   // Some data, not received from form,
   // are necessary to show exam and sets of questions again
   if (!ItsANewExam)
     {
      Exa_GetDataOfExamByCod (&Exam);
      Exams.ExaCod = Exam.ExaCod;
     }

   /***** If I can edit exams ==>
          overwrite some exam data with the data received from form *****/
   Exa_ReceiveExamFieldsFromForm (&Exam,Txt);
   if (Exa_CheckExamFieldsReceivedFromForm (&Exam))
     {
      /***** Create a new exam or update an existing one *****/
      if (ItsANewExam)
	{
	 Exa_CreateExam (&Exam,Txt);	// Add new exam to database
	 ItsANewExam = false;
	}
      else
	 Exa_UpdateExam (&Exam,Txt);	// Update exam data in database
     }

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
			ItsANewExam);	// It's not a new exam
  }

static void Exa_ReceiveExamFieldsFromForm (struct Exa_Exam *Exam,
				           char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   char MaxGradeStr[64];

   /***** Get exam title *****/
   Par_GetParToText ("Title",Exam->Title,Exa_MAX_BYTES_TITLE);

   /***** Get maximum grade *****/
   Par_GetParToText ("MaxGrade",MaxGradeStr,sizeof (MaxGradeStr) - 1);
   Exam->MaxGrade = Str_GetDoubleFromStr (MaxGradeStr);
   if (Exam->MaxGrade < 0.0)	// Only positive values allowed
      Exam->MaxGrade = 0.0;

   /***** Get visibility *****/
   Exam->Visibility = TstVis_GetVisibilityFromForm ();

   /***** Get exam text *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
  }

static bool Exa_CheckExamFieldsReceivedFromForm (const struct Exa_Exam *Exam)
  {
   extern const char *Txt_Already_existed_an_exam_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_exam;
   bool NewExamIsCorrect;

   /***** Check if title is correct *****/
   NewExamIsCorrect = true;
   if (Exam->Title[0])	// If there's an exam title
     {
      /* If title of exam was in database... */
      if (Exa_CheckIfSimilarExamExists (Exam))
	{
	 NewExamIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_exam_with_the_title_X,
			Exam->Title);
	}
     }
   else	// If there is not an exam title
     {
      NewExamIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_exam);
     }

   return NewExamIsCorrect;
  }

/*****************************************************************************/
/**************************** Create a new exam ******************************/
/*****************************************************************************/

static void Exa_CreateExam (struct Exa_Exam *Exam,const char *Txt)
  {
   extern const char *Txt_Created_new_exam_X;

   /***** Create a new exam *****/
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   Exam->ExaCod =
   DB_QueryINSERTandReturnCode ("can not create new exam",
				"INSERT INTO exa_exams"
				" (CrsCod,Hidden,UsrCod,MaxGrade,Visibility,Title,Txt)"
				" VALUES"
				" (%ld,'N',%ld,%.15lg,%u,'%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Exam->MaxGrade,
				Exam->Visibility,
				Exam->Title,
				Txt);
   Str_SetDecimalPointToLocal ();	// Return to local system

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_exam_X,
                  Exam->Title);
  }

/*****************************************************************************/
/************************* Update an existing exam *************************/
/*****************************************************************************/

static void Exa_UpdateExam (struct Exa_Exam *Exam,const char *Txt)
  {
   extern const char *Txt_The_exam_has_been_modified;

   /***** Update the data of the exam *****/
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   DB_QueryUPDATE ("can not update exam",
		   "UPDATE exa_exams"
		   " SET CrsCod=%ld,"
		        "MaxGrade=%.15lg,"
		        "Visibility=%u,"
		        "Title='%s',"
		        "Txt='%s'"
		   " WHERE ExaCod=%ld",
		   Gbl.Hierarchy.Crs.CrsCod,
		   Exam->MaxGrade,
		   Exam->Visibility,
	           Exam->Title,
	           Txt,
	           Exam->ExaCod);
   Str_SetDecimalPointToLocal ();	// Return to local system

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_exam_has_been_modified);
  }

/*****************************************************************************/
/****************** Write parameter with index of question *******************/
/*****************************************************************************/

void Exa_PutParamQstInd (unsigned QstInd)
  {
   Par_PutHiddenParamUnsigned (NULL,"QstInd",QstInd);
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

unsigned Exa_GetParamQstInd (void)
  {
   long QstInd;

   QstInd = Par_GetParToLong ("QstInd");
   if (QstInd < 0)
      Lay_ShowErrorAndExit ("Wrong question index.");

   return (unsigned) QstInd;
  }

/*****************************************************************************/
/************ Get question code given exam and index of question *************/
/*****************************************************************************/

long Exa_GetQstCodFromQstInd (long ExaCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long QstCod;

   /***** Get question code of the question to be moved up *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get question code",
			"SELECT QstCod FROM exa_set_questions"
			" WHERE ExaCod=%ld AND QstInd=%u",
			ExaCod,QstInd))
      Lay_ShowErrorAndExit ("Error: wrong question index.");

   /***** Get question code (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Lay_ShowErrorAndExit ("Error: wrong question code.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstCod;
  }

/*****************************************************************************/
/*********** Get previous question index to a given index in an exam **********/
/*****************************************************************************/
// Input question index can be 1, 2, 3... n-1
// Return question index will be 1, 2, 3... n if previous question exists, or 0 if no previous question

unsigned Exa_GetPrevQuestionIndexInExam (long ExaCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned PrevQstInd = 0;

   /***** Get previous question index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get previous question index",
			"SELECT MAX(QstInd) FROM exa_set_questions"
			" WHERE ExaCod=%ld AND QstInd<%u",
			ExaCod,QstInd))
      Lay_ShowErrorAndExit ("Error: previous question index not found.");

   /***** Get previous question index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row)
      if (row[0])
	 if (sscanf (row[0],"%u",&PrevQstInd) != 1)
	    Lay_ShowErrorAndExit ("Error when getting previous question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PrevQstInd;
  }

/*****************************************************************************/
/************* Get next question index to a given index in an exam ************/
/*****************************************************************************/
// Input question index can be 0, 1, 2, 3... n-1
// Return question index will be 1, 2, 3... n if next question exists, or 0 if no next question

unsigned Exa_GetNextQuestionIndexInExam (long ExaCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NextQstInd = ExaSes_AFTER_LAST_QUESTION;	// End of questions has been reached

   /***** Get next question index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get next question index",
			"SELECT MIN(QstInd) FROM exa_set_questions"
			" WHERE ExaCod=%ld AND QstInd>%u",
			ExaCod,QstInd))
      Lay_ShowErrorAndExit ("Error: next question index not found.");

   /***** Get next question index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row)
      if (row[0])
	 if (sscanf (row[0],"%u",&NextQstInd) != 1)
	    Lay_ShowErrorAndExit ("Error when getting next question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NextQstInd;
  }

/*****************************************************************************/
/*************** Put parameter with set code to edit, remove... **************/
/*****************************************************************************/
/*
static void Exa_PutParamSetCod (void *SetCod)	// Should be a pointer to long
  {
   if (SetCod)
      if (*((long *) SetCod) > 0)	// If set exists
	 Par_PutHiddenParamLong (NULL,"SetCod",*((long *) SetCod));
  }
*/

/*****************************************************************************/
/********** Get number of sessions and check is edition is possible **********/
/*****************************************************************************/
// Before calling this function, number of sessions must be calculated

bool Exa_CheckIfEditable (const struct Exa_Exam *Exam)
  {
   if (Exa_CheckIfICanEditExams ())
      /***** Questions are editable only if exam has no sessions *****/
      return (bool) (Exam->NumSess == 0);	// Exams with sessions should not be edited
   else
      return false;	// Questions are not editable
  }

/*****************************************************************************/
/********************* Get number of courses with exams **********************/
/*****************************************************************************/
// Returns the number of courses with exams in this location

unsigned Exa_GetNumCoursesWithExams (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with exams from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with exams",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM exa_exams");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with exams",
			 "SELECT COUNT(DISTINCT exa_exams.CrsCod)"
			 " FROM institutions,centres,degrees,courses,exa_exams"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with exams",
			 "SELECT COUNT(DISTINCT exa_exams.CrsCod)"
			 " FROM centres,degrees,courses,exa_exams"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod",
		         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with exams",
			 "SELECT COUNT(DISTINCT exa_exams.CrsCod)"
			 " FROM degrees,courses,exa_exams"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with exams",
			 "SELECT COUNT(DISTINCT exa_exams.CrsCod)"
			 " FROM courses,exa_exams"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=exa_exams.CrsCod",
		         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with exams",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM exa_exams"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of exams *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with exams.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/**************************** Get number of exams ****************************/
/*****************************************************************************/
// Returns the number of exams in this location

unsigned Exa_GetNumExams (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumExams;

   /***** Get number of exams from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of exams",
                         "SELECT COUNT(*)"
			 " FROM exa_exams");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of exams",
                         "SELECT COUNT(*)"
			 " FROM institutions,centres,degrees,courses,exa_exams"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod",
		         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of exams",
                         "SELECT COUNT(*)"
			 " FROM centres,degrees,courses,exa_exams"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod",
		         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of exams",
                         "SELECT COUNT(*)"
			 " FROM degrees,courses,exa_exams"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod",
		         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of exams",
                         "SELECT COUNT(*)"
			 " FROM courses,exa_exams"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=exa_exams.CrsCod",
		         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of exams",
                         "SELECT COUNT(*)"
			 " FROM exa_exams"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of exams *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumExams) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of exams.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumExams;
  }

/*****************************************************************************/
/************* Get average number of questions per course exam ***************/
/*****************************************************************************/

double Exa_GetNumQstsPerCrsExam (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   double NumQstsPerExam;

   /***** Get number of questions per exam from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_set_questions.QstCod) AS NumQsts"
			 " FROM exa_exams,exa_set_questions"
			 " WHERE exa_exams.ExaCod=exa_set_questions.ExaCod"
			 " GROUP BY exa_set_questions.ExaCod) AS NumQstsTable");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_set_questions.QstCod) AS NumQsts"
			 " FROM institutions,centres,degrees,courses,exa_exams,exa_set_questions"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_set_questions.ExaCod"
			 " GROUP BY exa_set_questions.ExaCod) AS NumQstsTable",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_set_questions.QstCod) AS NumQsts"
			 " FROM centres,degrees,courses,exa_exams,exa_set_questions"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_set_questions.ExaCod"
			 " GROUP BY exa_set_questions.ExaCod) AS NumQstsTable",
		         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_set_questions.QstCod) AS NumQsts"
			 " FROM degrees,courses,exa_exams,exa_set_questions"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_set_questions.ExaCod"
			 " GROUP BY exa_set_questions.ExaCod) AS NumQstsTable",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_set_questions.QstCod) AS NumQsts"
			 " FROM courses,exa_exams,exa_set_questions"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_set_questions.ExaCod"
			 " GROUP BY exa_set_questions.ExaCod) AS NumQstsTable",
		         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_set_questions.QstCod) AS NumQsts"
			 " FROM exa_exams,exa_set_questions"
			 " WHERE exa_exams.Cod=%ld"
			 " AND exa_exams.ExaCod=exa_set_questions.ExaCod"
			 " GROUP BY exa_set_questions.ExaCod) AS NumQstsTable",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get average number of questions per exam *****/
   row = mysql_fetch_row (mysql_res);
   NumQstsPerExam = Str_GetDoubleFromStr (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumQstsPerExam;
  }

/*****************************************************************************/
/*************** Get maximum score of an exam from database *******************/
/*****************************************************************************/

void Exa_GetScoreRange (long ExaCod,double *MinScore,double *MaxScore)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   unsigned NumRow;
   unsigned NumAnswers;

   /***** Get maximum score of an exam from database *****/
   NumRows = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get data of a question",
			     "SELECT COUNT(tst_answers.AnsInd) AS N"
			     " FROM tst_answers,exa_set_questions"
			     " WHERE exa_set_questions.ExaCod=%ld"
			     " AND exa_set_questions.QstCod=tst_answers.QstCod"
			     " GROUP BY tst_answers.QstCod",
			     ExaCod);
   for (NumRow = 0, *MinScore = *MaxScore = 0.0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get min answers (row[0]) */
      if (sscanf (row[0],"%u",&NumAnswers) != 1)
         NumAnswers = 0;

      /* Accumulate minimum and maximum score */
      if (NumAnswers < 2)
	 Lay_ShowErrorAndExit ("Wrong number of answers.");
      *MinScore += -1.0 / (double) (NumAnswers - 1);
      *MaxScore +=  1.0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
