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
#include "swad_exam_event.h"
#include "swad_exam_result.h"
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

#define Exa_MAX_SELECTED_QUESTIONS		10000
#define Exa_MAX_BYTES_LIST_SELECTED_QUESTIONS	(Exa_MAX_SELECTED_QUESTIONS * (Cns_MAX_DECIMAL_DIGITS_LONG + 1))

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
static bool Exa_CheckIfICanEditExams (void);
static void Exa_PutIconsListExams (void *Exams);
static void Exa_PutIconToCreateNewExam (struct Exa_Exams *Exams);
static void Exa_PutButtonToCreateNewExam (struct Exa_Exams *Exams);
static void Exa_PutParamsToCreateNewExam (void *Exams);

static void Exa_ShowOneExam (struct Exa_Exams *Exams,
                             struct Exa_Exam *Exam,bool ShowOnlyThisExam);

static void Exa_PutIconToShowResultsOfExam (void *Exams);
static void Exa_WriteAuthor (struct Exa_Exam *Exam);

static void Exa_PutHiddenParamExamOrder (Exa_Order_t SelectedOrder);

static void Exa_PutFormsToRemEditOneExam (struct Exa_Exams *Exams,
					  const struct Exa_Exam *Exam,
					  const char *Anchor);

static void ExaSet_PutParamsOneSet (void *Exams);
static void Exa_PutParamsOneQst (void *Exams);
static void Exa_PutHiddenParamOrder (Exa_Order_t SelectedOrder);
static Exa_Order_t Exa_GetParamOrder (void);

static void Exa_GetExamTxtFromDB (long ExaCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

static void Exa_RemoveExamFromAllTables (long ExaCod);

static bool ExaSet_CheckIfSimilarSetExists (const struct ExaSet_Set *Set);
static bool Exa_CheckIfSimilarExamExists (const struct Exa_Exam *Exam);

static void Exa_PutFormsOneExam (struct Exa_Exams *Exams,
				 struct Exa_Exam *Exam,
				 struct ExaSet_Set *Set,
				 bool ItsANewExam);

static void ExaSet_PutFormNewSet (struct Exa_Exams *Exams,
				  struct Exa_Exam *Exam,
				  struct ExaSet_Set *Set,
				  unsigned MaxSetInd);
static void ExaSet_ReceiveSetFieldsFromForm (struct ExaSet_Set *Set);
static bool ExaSet_CheckSetFieldsReceivedFromForm (const struct ExaSet_Set *Set);

static void Exa_PutFormEditionExam (struct Exa_Exams *Exams,
				    struct Exa_Exam *Exam,
				    char Txt[Cns_MAX_BYTES_TEXT + 1],
				    bool ItsANewExam);
static void Exa_ReceiveExamFieldsFromForm (struct Exa_Exam *Exam,
				           char Txt[Cns_MAX_BYTES_TEXT + 1]);
static bool Exa_CheckExamFieldsReceivedFromForm (const struct Exa_Exam *Exam);

static void ExaSet_CreateSet (struct ExaSet_Set *Set);
static void ExaSet_UpdateSet (struct ExaSet_Set *Set);

static void Exa_CreateExam (struct Exa_Exam *Exam,const char *Txt);
static void Exa_UpdateExam (struct Exa_Exam *Exam,const char *Txt);

static void ExaSet_PutParamSetCod (long SetCod);

static void Exa_RemAnswersOfAQuestion (long ExaCod,unsigned QstInd);

static unsigned ExaSet_GetSetIndFromSetCod (long ExaCod,long SetCod);
static long ExaSet_GetSetCodFromSetInd (long ExaCod,unsigned SetInd);

static unsigned ExaSet_GetMaxSetIndexInExam (long ExaCod);
static unsigned Exa_GetMaxQuestionIndexInExam (long ExaCod);

static unsigned ExaSet_GetPrevSetIndexInExam (long ExaCod,unsigned SetInd);
static unsigned ExaSet_GetNextSetIndexInExam (long ExaCod,unsigned SetInd);

static void ExaSet_ListExamSets (struct Exa_Exams *Exams,
                                 struct Exa_Exam *Exam,
				 struct ExaSet_Set *Set);
static void Exa_ListExamQuestions (struct Exa_Exams *Exams,struct Exa_Exam *Exam);
static void ExaSet_ListOneOrMoreSetsForEdition (struct Exa_Exams *Exams,
					        long ExaCod,
					        unsigned MaxSetInd,
					        unsigned NumSets,
                                                MYSQL_RES *mysql_res,
                                                bool ICanEditSets);
static void ExaSet_PutTableHeadingForSets (void);

static void ExaSet_ResetSet (struct ExaSet_Set *Set);
static void Exa_PutParamSetCod (void *SetCod);
static void Exa_ListOneOrMoreQuestionsForEdition (struct Exa_Exams *Exams,
						  long ExaCod,unsigned NumQsts,
                                                  MYSQL_RES *mysql_res,
						  bool ICanEditQuestions);
static void Exa_ListQuestionForEdition (const struct Tst_Question *Question,
                                        unsigned QstInd,bool QuestionExists);
static void Exa_PutIconToAddNewQuestions (void *Exams);
static void Exa_PutButtonToAddNewQuestions (struct Exa_Exams *Exams);

static void Exa_AllocateListSelectedQuestions (struct Exa_Exams *Exams);
static void Exa_FreeListsSelectedQuestions (struct Exa_Exams *Exams);
static unsigned Exa_CountNumQuestionsInList (const struct Exa_Exams *Exams);

static void ExaSet_ExchangeSets (long ExaCod,
                                 unsigned SetIndTop,unsigned SetIndBottom);
static void Exa_ExchangeQuestions (long ExaCod,
                                   unsigned QstIndTop,unsigned QstIndBottom);

static bool Exa_CheckIfEditable (const struct Exa_Exam *Exam);

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
   Exams->EvtCod            = -1L;	// Selected/current event code
   Exams->SetInd            = 0;	// Current set index
   Exams->QstInd            = 0;	// Current question index
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
   Exam->NumSets                 = 0;
   Exam->NumQsts                 = 0;
   Exam->NumEvts                 = 0;
   Exam->NumUnfinishedEvts       = 0;
   Exam->Hidden                  = false;
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
   extern const char *Txt_Events;
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
      HTM_TABLE_BeginWideMarginPadding (2);
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

      HTM_TH (1,1,"RM",Txt_Events);

      HTM_TR_End ();

      /***** Write all exams *****/
      for (NumExam  = Pagination.FirstItemVisible;
	   NumExam <= Pagination.LastItemVisible;
	   NumExam++)
	{
	 /* Get data of this exam */
	 Exam.ExaCod = Exams->Lst[NumExam - 1].ExaCod;
	 Exa_GetDataOfExamByCod (&Exam);

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

static bool Exa_CheckIfICanEditExams (void)
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

      /***** Put icon to view events results *****/
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

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Show exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        false,	// Do not list exam questions
	                false);	// Do not put form to start new event
  }

/*****************************************************************************/
/******************************* Show one exam *******************************/
/*****************************************************************************/

void Exa_ShowOnlyOneExam (struct Exa_Exams *Exams,
			  struct Exa_Exam *Exam,
			  bool ListExamQuestions,
			  bool PutFormNewMatch)
  {
   Exa_ShowOnlyOneExamBegin (Exams,Exam,ListExamQuestions,PutFormNewMatch);
   Exa_ShowOnlyOneExamEnd ();
  }

void Exa_ShowOnlyOneExamBegin (struct Exa_Exams *Exams,
			       struct Exa_Exam *Exam,
			       bool ListExamQuestions,
			       bool PutFormNewMatch)
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

   if (ListExamQuestions)
       /***** Write questions of this exam *****/
      Exa_ListExamQuestions (Exams,Exam);
   else
      /***** List events *****/
      ExaEvt_ListEvents (Exams,Exam,PutFormNewMatch);
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
   extern const char *Txt_Set_of_questions;
   extern const char *Txt_Maximum_grade;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Events;
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   Dat_StartEndTime_t StartEndTime;
   const char *Color;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Set anchor string *****/
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
      Exa_PutFormsToRemEditOneExam (Exams,Exam,Anchor);

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
      Color = Exam->NumUnfinishedEvts ? (Exam->Hidden ? "DATE_GREEN_LIGHT":
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
				       true,true,true,0x7);
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
   HTM_TxtColonNBSP (Txt_Set_of_questions);
   HTM_Unsigned (Exam->NumSets);
   HTM_BR ();
   HTM_TxtColonNBSP (Txt_Maximum_grade);
   HTM_Double (Exam->MaxGrade);
   HTM_BR ();
   HTM_TxtColonNBSP (Txt_Result_visibility);
   TstVis_ShowVisibilityIcons (Exam->Visibility,Exam->Hidden);
   HTM_DIV_End ();

   /***** Number of events in exam *****/
   if (ShowOnlyThisExam)
      HTM_TD_Begin ("class=\"RT\"");
   else
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);

   Exams->ExaCod = Exam->ExaCod;
   Frm_StartForm (ActSeeExa);
   Exa_PutParams (Exams);
   HTM_BUTTON_SUBMIT_Begin (Txt_Events,
			    Exam->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT" :
				           "BT_LINK LT ASG_TITLE",
			    NULL);
   if (ShowOnlyThisExam)
      HTM_TxtColonNBSP (Txt_Events);
   HTM_Unsigned (Exam->NumEvts);
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
/************* Put icon to show results of events in an exam *****************/
/*****************************************************************************/

static void Exa_PutIconToShowResultsOfExam (void *Exams)
  {
   if (Exams)
     {
      /***** Put icon to view events results *****/
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

static void Exa_PutFormsToRemEditOneExam (struct Exa_Exams *Exams,
					  const struct Exa_Exam *Exam,
					  const char *Anchor)
  {
   Exams->ExaCod = Exam->ExaCod;

   /***** Put icon to remove exam *****/
   Ico_PutContextualIconToRemove (ActReqRemExa,
                                  Exa_PutParams,Exams);

   /***** Put icon to unhide/hide exam *****/
   if (Exam->Hidden)
      Ico_PutContextualIconToUnhide (ActShoExa,Anchor,
                                     Exa_PutParams,Exams);
   else
      Ico_PutContextualIconToHide (ActHidExa,Anchor,
                                   Exa_PutParams,Exams);

   /***** Put icon to edit exam *****/
   Ico_PutContextualIconToEdit (ActEdiOneExa,NULL,
                                Exa_PutParams,Exams);
  }

/*****************************************************************************/
/************ Put parameter to move/remove one set of questions **************/
/*****************************************************************************/

static void ExaSet_PutParamsOneSet (void *Exams)
  {
   if (Exams)
     {
      Exa_PutParams (Exams);
      ExaSet_PutParamSetCod (((struct Exa_Exams *) Exams)->SetCod);
     }
  }

/*****************************************************************************/
/**************** Put parameter to move/remove one question ******************/
/*****************************************************************************/

static void Exa_PutParamsOneQst (void *Exams)
  {
   if (Exams)
     {
      Exa_PutParams (Exams);
      Exa_PutParamQstInd (((struct Exa_Exams *) Exams)->QstInd);
     }
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
/********************** Get parameter with code of set ***********************/
/*****************************************************************************/

long ExaSet_GetParamSetCod (void)
  {
   /***** Get code of set *****/
   return Par_GetParToLong ("SetCod");
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
			            "MIN(exa_events.StartTime) AS StartTime,"	// row[1]
			            "MAX(exa_events.EndTime) AS EndTime"	// row[2]
			     " FROM exa_exams"
			     " LEFT JOIN exa_events"
			     " ON exa_exams.ExaCod=exa_events.ExaCod"
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
/********************* Get list of exam events selected **********************/
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
/*********************** Get set data using its code *************************/
/*****************************************************************************/

void ExaSet_GetDataOfSetByCod (struct ExaSet_Set *Set)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrSetInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Get data of set of questions from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get set data",
		       "SELECT SetCod,"		// row[0]
			      "SetInd,"		// row[1]
			      "NumQstsToExam,"	// row[2]
			      "Title"		// row[3]
		       " FROM exa_sets"
		       " WHERE SetCod=%ld"
		       " AND ExaCod=%ld",	// Extra check
		       Set->SetCod,Set->ExaCod)) // Set found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] SetCod
      row[1] SetInd
      row[2] NumQstsToExam
      row[3] Title
      */
      /* Get set code (row[0]) */
      Set->SetCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get set index (row[1]) */
      Set->SetInd = Str_ConvertStrToUnsigned (row[1]);
      snprintf (StrSetInd,sizeof (Set->SetInd),
	        "%u",
		Set->SetInd);

      /* Get set index (row[2]) */
      Set->NumQstsToExam = Str_ConvertStrToUnsigned (row[2]);

      /* Get the title of the set (row[3]) */
      Str_Copy (Set->Title,row[3],
                ExaSet_MAX_BYTES_TITLE);
     }
   else
      /* Initialize to empty set */
      ExaSet_ResetSet (Set);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get exam data using its code *************************/
/*****************************************************************************/

void Exa_GetDataOfExamByCod (struct Exa_Exam *Exam)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

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
			     " LEFT JOIN exa_events"
			     " ON exa_exams.ExaCod=exa_events.ExaCod"
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

      /* Get number of events */
      Exam->NumEvts = ExaEvt_GetNumEventsInExam (Exam->ExaCod);

      /* Get number of unfinished events */
      Exam->NumUnfinishedEvts = ExaEvt_GetNumUnfinishedEventsInExam (Exam->ExaCod);
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
				" FROM exa_events"
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

static void Exa_GetExamTxtFromDB (long ExaCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
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

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);

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

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get exam code *****/
   if ((Exam.ExaCod = Exa_GetParamExamCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of exam is missing.");

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);

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
   /***** Remove all events in this exam *****/
   ExaEvt_RemoveEventsInExamFromAllTables (ExaCod);

   /***** Remove exam question *****/
   DB_QueryDELETE ("can not remove exam questions",
		   "DELETE FROM exa_questions WHERE ExaCod=%ld",
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
   /***** Remove all events in this course *****/
   ExaEvt_RemoveEventInCourseFromAllTables (CrsCod);

   /***** Remove the questions in exams *****/
   DB_QueryDELETE ("can not remove questions in course exams",
		   "DELETE FROM exa_questions"
		   " USING exa_exams,exa_questions"
		   " WHERE exa_exams.CrsCod=%ld"
		   " AND exa_exams.ExaCod=exa_questions.ExaCod",
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

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Hide exam *****/
   DB_QueryUPDATE ("can not hide exam",
		   "UPDATE exa_exams SET Hidden='Y' WHERE ExaCod=%ld",
		   Exam.ExaCod);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/******************************** Show an exam ******************************/
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

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get data of the exam from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Show exam *****/
   DB_QueryUPDATE ("can not show exam",
		   "UPDATE exa_exams SET Hidden='N' WHERE ExaCod=%ld",
		   Exam.ExaCod);

   /***** Show exams again *****/
   Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/************** Check if the title of a set of questions exists **************/
/*****************************************************************************/

static bool ExaSet_CheckIfSimilarSetExists (const struct ExaSet_Set *Set)
  {
   /***** Get number of set of questions with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar sets of questions",
			  "SELECT COUNT(*) FROM exa_sets,exa_exams"
			  " WHERE exa_sets.ExaCod=%ld AND exa_sets.Title='%s'"
			  " AND exa_sets.SetCod<>%ld"
			  " AND exa_sets.ExaCod=exa_exams.ExaCod"
			  " AND exa_exams.CrsCod=%ld",	// Extra check
			  Set->ExaCod,Set->Title,
			  Set->SetCod,
			  Gbl.Hierarchy.Crs.CrsCod) != 0);
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

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   ItsANewExam = (Exam.ExaCod <= 0);

   /***** Get exam data *****/
   if (ItsANewExam)
      /* Initialize to empty exam */
      Exa_ResetExam (&Exam);
   else
      /* Get exam data from database */
      Exa_GetDataOfExamByCod (&Exam);

   /***** Put form to create/edit an exam and show sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,ItsANewExam);

   /***** Show exams or questions *****/
   if (ItsANewExam)
      /* Show exams again */
      Exa_ListAllExams (&Exams);
  }

/*****************************************************************************/
/******************** Put forms to create/edit an exam ***********************/
/*****************************************************************************/

static void Exa_PutFormsOneExam (struct Exa_Exams *Exams,
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

static void ExaSet_PutFormNewSet (struct Exa_Exams *Exams,
				  struct Exa_Exam *Exam,
				  struct ExaSet_Set *Set,
				  unsigned MaxSetInd)
  {
   // extern const char *Hlp_ASSESSMENT_Exams_new_set;
   // extern const char *Hlp_ASSESSMENT_Exams_edit_set;
   extern const char *Txt_New_set_of_questions;
   extern const char *Txt_Create_set_of_questions;

   /***** Begin form *****/
   Exams->ExaCod = Exam->ExaCod;
   Frm_StartForm (ActNewExaSet);
   Exa_PutParams (Exams);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_set_of_questions,
		      NULL,NULL,
		      NULL,Box_NOT_CLOSABLE,2);

   /***** Table heading *****/
   ExaSet_PutTableHeadingForSets ();

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Empty column for buttons *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Index *****/
   HTM_TD_Begin ("class=\"RM\"");
   Tst_WriteNumQst (MaxSetInd + 1);
   HTM_TD_End ();

   /***** Title *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Title",ExaSet_MAX_CHARS_TITLE,Set->Title,false,
		   "id=\"Title\" required=\"required\""
		   " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_TD_End ();

   /***** Current number of questions in set *****/
   HTM_TD_Begin ("class=\"RM\"");
   HTM_Unsigned (0);	// New set ==> no questions yet
   HTM_TD_End ();

   /***** Number of questions to appear in the exam *****/
   HTM_TD_Begin ("class=\"RM\"");
   HTM_INPUT_LONG ("NumQstsToExam",0,UINT_MAX,(long) Set->NumQstsToExam,false,
		    "class=\"INPUT_LONG\" required=\"required\"");
   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_set_of_questions);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Receive form to create a new set of questions **************/
/*****************************************************************************/

void ExaSet_RecFormSet (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   bool ItsANewSet;

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   ItsANewSet = (Set.SetCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** If I can edit exams ==> receive set from form *****/
   ExaSet_ReceiveSetFieldsFromForm (&Set);
   if (ExaSet_CheckSetFieldsReceivedFromForm (&Set))
     {
      /***** Create a new exam or update an existing one *****/
      if (ItsANewSet)
	 ExaSet_CreateSet (&Set);	// Add new set to database
      else
	 ExaSet_UpdateSet (&Set);	// Update set data in database
     }

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

static void ExaSet_ReceiveSetFieldsFromForm (struct ExaSet_Set *Set)
  {
   /***** Get set title *****/
   Par_GetParToText ("Title",Set->Title,ExaSet_MAX_BYTES_TITLE);

   /***** Get number of questions in set to appear in exam *****/
   Set->NumQstsToExam = (unsigned) Par_GetParToUnsignedLong ("NumQstsToExam",
                                                             0,
                                                             UINT_MAX,
                                                             0);
  }

static bool ExaSet_CheckSetFieldsReceivedFromForm (const struct ExaSet_Set *Set)
  {
   extern const char *Txt_Already_existed_a_set_of_questions_in_this_exam_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_set_of_questions;
   bool NewSetIsCorrect;

   /***** Check if title is correct *****/
   NewSetIsCorrect = true;
   if (Set->Title[0])	// If there's an set title
     {
      /* If title of set was in database... */
      if (ExaSet_CheckIfSimilarSetExists (Set))
	{
	 NewSetIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_set_of_questions_in_this_exam_with_the_title_X,
			Set->Title);
	}
     }
   else	// If there is not a set title
     {
      NewSetIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_set_of_questions);
     }

   return NewSetIsCorrect;
  }

/*****************************************************************************/
/********************* Put a form to create/edit an exam **********************/
/*****************************************************************************/

static void Exa_PutFormEditionExam (struct Exa_Exams *Exams,
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
   HTM_INPUT_TEXT ("Title",Exa_MAX_CHARS_TITLE,Exam->Title,false,
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

void Exa_RecFormExam (void)
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

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   ItsANewExam = (Exam.ExaCod <= 0);

   /***** If I can edit exams ==> receive exam from form *****/
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
/********************** Create a new set of questions ************************/
/*****************************************************************************/

static void ExaSet_CreateSet (struct ExaSet_Set *Set)
  {
   extern const char *Txt_Created_new_set_of_questions_X;
   unsigned MaxSetInd;

   /***** Get maximum set index *****/
   MaxSetInd = ExaSet_GetMaxSetIndexInExam (Set->ExaCod);

   /***** Create a new exam *****/
   Set->SetCod =
   DB_QueryINSERTandReturnCode ("can not create new set of questions",
				"INSERT INTO exa_sets"
				" (ExaCod,SetInd,NumQstsToExam,Title)"
				" VALUES"
				" (%ld,%u,%u,'%s')",
				Set->ExaCod,
				MaxSetInd + 1,
				Set->NumQstsToExam,
				Set->Title);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_set_of_questions_X,
                  Set->Title);
  }

/*****************************************************************************/
/******************** Update an existing set of questions ********************/
/*****************************************************************************/

static void ExaSet_UpdateSet (struct ExaSet_Set *Set)
  {
   extern const char *Txt_The_set_of_questions_has_been_modified;

   /***** Update the data of the set of questions *****/
   DB_QueryUPDATE ("can not update set of questions",
		   "UPDATE exa_sets"
		   " SET ExaCod=%ld,"
		        "SetInd=%u,"
		        "NumQstsToExam=%u,"
		        "Title='%s'"
		   " WHERE SetCod=%ld",
		   Set->ExaCod,
		   Set->SetInd,
		   Set->NumQstsToExam,
	           Set->Title,
	           Set->SetCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_set_of_questions_has_been_modified);
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
/******************* Get number of questions of an exam *********************/
/*****************************************************************************/

unsigned ExaSet_GetNumSetsExam (long ExaCod)
  {
   /***** Get number of sets in an exam from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of sets in an exam",
			     "SELECT COUNT(*) FROM exa_sets"
			     " WHERE ExaCod=%ld",
			     ExaCod);
  }

/*****************************************************************************/
/******************* Get number of questions of an exam *********************/
/*****************************************************************************/

unsigned ExaSet_GetNumQstsExam (long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts = 0;

   /***** Get total number of questions to appear in exam *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get number of questions in an exam",
			"SELECT SUM(NumQstsToExam) FROM exa_sets"
			" WHERE ExaCod=%ld",
			ExaCod))
      Lay_ShowErrorAndExit ("Error: wrong question index.");

   /***** Get number of questions (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])
      NumQsts = Str_ConvertStrToUnsigned (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumQsts;
  }

/*****************************************************************************/
/********** Request the creation or edition of an set of questions ***********/
/*****************************************************************************/

void ExaSet_RequestCreatOrEditSet (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   bool ItsANewSet;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Check if I can edit exams *****/
   if (!Exa_CheckIfICanEditExams ())
      Lay_NoPermissionExit ();

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   ItsANewSet = (Set.SetCod <= 0);

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exa_GetExamTxtFromDB (Exam.ExaCod,Txt);

   /***** Get set data *****/
   if (ItsANewSet)
      /* Initialize to empty set */
      ExaSet_ResetSet (&Set);
   else
      /* Get set data from database */
      ExaSet_GetDataOfSetByCod (&Set);

   /***** Put form to edit the exam created or updated *****/
   Exa_PutFormEditionExam (&Exams,&Exam,Txt,
			   false);	// No new exam
  }

/*****************************************************************************/
/*************** Put a form to edit/create a question in exam ****************/
/*****************************************************************************/

void Exa_RequestNewQuestion (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Check if exam has events *****/
   if (Exa_CheckIfEditable (&Exam))
     {
      /***** Show form to create a new question in this exam *****/
      Exams.ExaCod = Exam.ExaCod;
      Tst_RequestSelectTestsForExam (&Exams);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        true,	// List exam questions
	                false);	// Do not put form to start new event
  }

/*****************************************************************************/
/**************** List several test questions for selection ******************/
/*****************************************************************************/

void Exa_ListQuestionsToSelect (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Check if exam has events *****/
   if (Exa_CheckIfEditable (&Exam))
     {
      /***** List several test questions for selection *****/
      Exams.ExaCod = Exam.ExaCod;
      Tst_ListQuestionsToSelectForExam (&Exams);
     }
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/**************** Write parameter with index of set of questions *****************/
/*****************************************************************************/

static void ExaSet_PutParamSetCod (long SetCod)
  {
   Par_PutHiddenParamUnsigned (NULL,"SetCod",SetCod);
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
/********************** Remove answers of an exam question ********************/
/*****************************************************************************/

static void Exa_RemAnswersOfAQuestion (long ExaCod,unsigned QstInd)
  {
   /***** Remove answers from all events of this exam *****/
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM exa_answers"
		   " USING exa_events,exa_answers"
		   " WHERE exa_events.ExaCod=%ld"	// From all events of this exam...
		   " AND exa_events.EvtCod=exa_answers.EvtCod"
		   " AND exa_answers.QstInd=%u",	// ...remove only answers to this question
		   ExaCod,QstInd);
  }

/*****************************************************************************/
/****************** Get set index given exam and set code ********************/
/*****************************************************************************/

static unsigned ExaSet_GetSetIndFromSetCod (long ExaCod,long SetCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long SetInd;

   /***** Get set index from set code *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get set index",
			"SELECT SetInd FROM exa_sets"
			" WHERE SetCod=%u"
			" AND ExaCod=%ld",	// Extra check
			SetCod,ExaCod))
      Lay_ShowErrorAndExit ("Error: wrong set code.");

   /***** Get set code (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   SetInd = Str_ConvertStrToUnsigned (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return SetInd;
  }

/*****************************************************************************/
/****************** Get set code given exam and set index ********************/
/*****************************************************************************/

static long ExaSet_GetSetCodFromSetInd (long ExaCod,unsigned SetInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long SetCod;

   /***** Get set code from set index *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get set code",
			"SELECT SetCod FROM exa_sets"
			" WHERE ExaCod=%ld AND SetInd=%u",
			ExaCod,SetInd))
      Lay_ShowErrorAndExit ("Error: wrong set index.");

   /***** Get set code (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if ((SetCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Lay_ShowErrorAndExit ("Error: wrong set code.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return SetCod;
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
			"SELECT QstCod FROM exa_questions"
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
/********************* Get maximum set index in an exam **********************/
/*****************************************************************************/
// Question index can be 1, 2, 3...
// Return 0 if no questions

static unsigned ExaSet_GetMaxSetIndexInExam (long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned SetInd = 0;

   /***** Get maximum set index in an exam from database *****/
   DB_QuerySELECT (&mysql_res,"can not get max set index",
		   "SELECT MAX(SetInd)"
		   " FROM exa_sets"
		   " WHERE ExaCod=%ld",
                   ExaCod);
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are sets
      if (sscanf (row[0],"%u",&SetInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting max set index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return SetInd;
  }

/*****************************************************************************/
/****************** Get maximum question index in an exam *********************/
/*****************************************************************************/
// Question index can be 1, 2, 3...
// Return 0 if no questions

static unsigned Exa_GetMaxQuestionIndexInExam (long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned QstInd = 0;

   /***** Get maximum question index in an exam from database *****/
   DB_QuerySELECT (&mysql_res,"can not get max question index",
		   "SELECT MAX(QstInd)"
		   " FROM exa_questions"
		   " WHERE ExaCod=%ld",
                   ExaCod);
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
      if (sscanf (row[0],"%u",&QstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting max question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/*********** Get previous set index to a given set index in an exam **********/
/*****************************************************************************/
// Input set index can be 1, 2, 3... n-1
// Return set index will be 1, 2, 3... n if previous set exists, or 0 if no previous set

static unsigned ExaSet_GetPrevSetIndexInExam (long ExaCod,unsigned SetInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned PrevSetInd = 0;

   /***** Get previous set index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get previous set index",
			"SELECT MAX(SetInd) FROM exa_sets"
			" WHERE ExaCod=%ld AND SetInd<%u",
			ExaCod,SetInd))
      Lay_ShowErrorAndExit ("Error: previous set index not found.");

   /***** Get previous set index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row)
      if (row[0])
	 if (sscanf (row[0],"%u",&PrevSetInd) != 1)
	    Lay_ShowErrorAndExit ("Error when getting previous set index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PrevSetInd;
  }

/*****************************************************************************/
/*************** Get next set index to a given index in an exam **************/
/*****************************************************************************/
// Input set index can be 0, 1, 2, 3... n-1
// Return set index will be 1, 2, 3... n if next set exists, or 0 if no next set

static unsigned ExaSet_GetNextSetIndexInExam (long ExaCod,unsigned SetInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NextSetInd = ExaEvt_AFTER_LAST_QUESTION;	// End of sets has been reached

   /***** Get next set index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get next set index",
			"SELECT MIN(SetInd) FROM exa_sets"
			" WHERE ExaCod=%ld AND SetInd>%u",
			ExaCod,SetInd))
      Lay_ShowErrorAndExit ("Error: next set index not found.");

   /***** Get next set index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row)
      if (row[0])
	 if (sscanf (row[0],"%u",&NextSetInd) != 1)
	    Lay_ShowErrorAndExit ("Error when getting next set index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NextSetInd;
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
			"SELECT MAX(QstInd) FROM exa_questions"
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
   unsigned NextQstInd = ExaEvt_AFTER_LAST_QUESTION;	// End of questions has been reached

   /***** Get next question index in an exam from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get next question index",
			"SELECT MIN(QstInd) FROM exa_questions"
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
/************************* List the sets of an exam **************************/
/*****************************************************************************/

static void ExaSet_ListExamSets (struct Exa_Exams *Exams,
                                 struct Exa_Exam *Exam,
				 struct ExaSet_Set *Set)
  {
   extern const char *Hlp_ASSESSMENT_Exams_question_sets;
   extern const char *Txt_Sets_of_questions;
   extern const char *Txt_This_exam_has_no_sets_of_questions;
   MYSQL_RES *mysql_res;
   unsigned MaxSetInd;
   unsigned NumSets;
   bool ICanEditSets = Exa_CheckIfEditable (Exam);

   /***** Get maximum set index *****/
   MaxSetInd = ExaSet_GetMaxSetIndexInExam (Exam->ExaCod);

   /***** Get data of set of questions from database *****/
   NumSets = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get sets of questions",
			      "SELECT SetCod,"		// row[0]
			             "SetInd,"		// row[1]
				     "NumQstsToExam,"	// row[2]
				     "Title"		// row[3]
			      " FROM exa_sets"
			      " WHERE ExaCod=%ld"
			      " ORDER BY SetInd",
			      Exam->ExaCod);

   /***** Begin box *****/
   Exams->ExaCod = Exam->ExaCod;
   Box_BoxBegin (NULL,Txt_Sets_of_questions,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams_question_sets,Box_NOT_CLOSABLE);

   /***** Show table with sets *****/
   if (NumSets)
      ExaSet_ListOneOrMoreSetsForEdition (Exams,
                                          Exam->ExaCod,MaxSetInd,
                                          NumSets,mysql_res,
				          ICanEditSets);
   else		// This exam has no sets
      Ale_ShowAlert (Ale_INFO,Txt_This_exam_has_no_sets_of_questions);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Put forms to create/edit a set *****/
   ExaSet_PutFormNewSet (Exams,Exam,Set,MaxSetInd);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************ List the questions of an exam ***********************/
/*****************************************************************************/

static void Exa_ListExamQuestions (struct Exa_Exams *Exams,struct Exa_Exam *Exam)
  {
   extern const char *Hlp_ASSESSMENT_Exams_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_This_exam_has_no_questions;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   bool ICanEditQuestions = Exa_CheckIfEditable (Exam);

   /***** Get data of questions from database *****/
   NumQsts = (unsigned)
             DB_QuerySELECT (&mysql_res,"can not get exam questions",
			      "SELECT QstInd,"	// row[0]
				     "QstCod"	// row[1]
			      " FROM exa_questions"
			      " WHERE ExaCod=%ld"
			      " ORDER BY QstInd",
			      Exam->ExaCod);

   /***** Begin box *****/
   Exams->ExaCod = Exam->ExaCod;
   if (ICanEditQuestions)
      Box_BoxBegin (NULL,Txt_Questions,
		    Exa_PutIconToAddNewQuestions,Exams,
		    Hlp_ASSESSMENT_Exams_questions,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Questions,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Exams_questions,Box_NOT_CLOSABLE);

   /***** Show table with questions *****/
   if (NumQsts)
      Exa_ListOneOrMoreQuestionsForEdition (Exams,
                                            Exam->ExaCod,NumQsts,mysql_res,
					    ICanEditQuestions);
   else	// This exam has no questions
      Ale_ShowAlert (Ale_INFO,Txt_This_exam_has_no_questions);

   /***** Put button to add a new question in this exam *****/
   if (ICanEditQuestions)		// I can edit questions
      Exa_PutButtonToAddNewQuestions (Exams);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************* List exam sets for edition ************************/
/*****************************************************************************/

static void ExaSet_ListOneOrMoreSetsForEdition (struct Exa_Exams *Exams,
					        long ExaCod,
					        unsigned MaxSetInd,
					        unsigned NumSets,
                                                MYSQL_RES *mysql_res,
                                                bool ICanEditSets)
  {
   extern const char *Txt_Sets_of_questions;
   extern const char *Txt_Move_up_X;
   extern const char *Txt_Move_down_X;
   extern const char *Txt_Movement_not_allowed;
   unsigned NumSet;
   MYSQL_ROW row;
   struct ExaSet_Set Set;
   char *Anchor = NULL;
   char StrSetInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   ExaSet_PutTableHeadingForSets ();

   /***** Write rows *****/
   for (NumSet = 0;
	NumSet < NumSets;
	NumSet++)
     {
      Gbl.RowEvenOdd = NumSet % 2;

      /***** Create set of questions *****/
      ExaSet_ResetSet (&Set);

      /***** Get set data *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] SetCod
      row[1] SetInd
      row[2] NumQstsToExam
      row[3] Title
      */
      /* Get set code (row[0]) */
      Set.SetCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get set index (row[1]) */
      Set.SetInd = Str_ConvertStrToUnsigned (row[1]);
      snprintf (StrSetInd,sizeof (Set.SetInd),
	        "%u",
		Set.SetInd);

      /* Get set index (row[2]) */
      Set.NumQstsToExam = Str_ConvertStrToUnsigned (row[2]);

      /* Get the title of the set (row[3]) */
      Str_Copy (Set.Title,row[3],
                ExaSet_MAX_BYTES_TITLE);

      /* Initialize context */
      Exams->ExaCod = ExaCod;
      Exams->SetCod = Set.SetCod;
      Exams->SetInd = Set.SetInd;

      /***** Build anchor string *****/
      Frm_SetAnchorStr (Set.SetCod,&Anchor);

      /***** Begin row *****/
      HTM_TR_Begin (NULL);

      /***** Icons *****/
      HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

      /* Put icon to remove the set */
      if (ICanEditSets)
	{
	 Frm_StartForm (ActReqRemExaSet);
	 ExaSet_PutParamsOneSet (Exams);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}
      else
         Ico_PutIconRemovalNotAllowed ();

      /* Put icon to move up the question */
      if (ICanEditSets && Set.SetInd > 1)
	{
	 Lay_PutContextualLinkOnlyIcon (ActUp_ExaSet,Anchor,
	                                ExaSet_PutParamsOneSet,Exams,
				        "arrow-up.svg",
					Str_BuildStringStr (Txt_Move_up_X,
							    StrSetInd));
	 Str_FreeString ();
	}
      else
         Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);

      /* Put icon to move down the set */
      if (ICanEditSets && Set.SetInd < MaxSetInd)
	{
	 Lay_PutContextualLinkOnlyIcon (ActDwnExaSet,Anchor,
	                                ExaSet_PutParamsOneSet,Exams,
				        "arrow-down.svg",
					Str_BuildStringStr (Txt_Move_down_X,
							    StrSetInd));
	 Str_FreeString ();
	}
      else
         Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);

      /* Put icon to edit the set */
      if (ICanEditSets)
	 // Ico_PutContextualIconToEdit (ActEdiOneExaSet,NULL,
	 //                              Exa_PutParamSetCod,&Set.SetCod);
	 Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,	// TODO: Change for ActEdiOneExaSet
	                              Exa_PutParamSetCod,&Set.SetCod);

      HTM_TD_End ();

      /***** Index *****/
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      Tst_WriteNumQst (Set.SetInd);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_ARTICLE_Begin (Anchor);
      HTM_Txt (Set.Title);
      HTM_ARTICLE_End ();
      HTM_TD_End ();

      /***** Current number of questions in set *****/
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Unsigned (Set.NumQstsToExam);	// TODO: Change to current number of questions in set
      HTM_TD_End ();

      /***** Number of questions to appear in exam *****/
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Unsigned (Set.NumQstsToExam);
      HTM_TD_End ();

      /***** End row *****/
      HTM_TR_End ();

      /***** Free anchor string *****/
      Frm_FreeAnchorStr (Anchor);
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Put table heading for sets of questions *******************/
/*****************************************************************************/

static void ExaSet_PutTableHeadingForSets (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Set_of_questions;
   extern const char *Txt_Number_of_questions;
   extern const char *Txt_Number_of_questions_in_the_exam;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Header cells *****/
   HTM_TH_Empty (1);
   HTM_TH (1,1,"RB",Txt_No_INDEX);
   HTM_TH (1,1,"LB",Txt_Set_of_questions);
   HTM_TH (1,1,"RB",Txt_Number_of_questions);
   HTM_TH (1,1,"RB",Txt_Number_of_questions_in_the_exam);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Reset set of questions **************************/
/*****************************************************************************/

static void ExaSet_ResetSet (struct ExaSet_Set *Set)
  {
   Set->ExaCod        = -1L;
   Set->SetCod        = -1L;
   Set->SetInd        = 0;
   Set->Title[0]      = '\0';
   Set->NumQstsToExam = 0;
  }

/*****************************************************************************/
/*************** Put parameter with set code to edit, remove... **************/
/*****************************************************************************/

static void Exa_PutParamSetCod (void *SetCod)	// Should be a pointer to long
  {
   if (SetCod)
      if (*((long *) SetCod) > 0)	// If set exists
	 Par_PutHiddenParamLong (NULL,"SetCod",*((long *) SetCod));
  }

/*****************************************************************************/
/********************* List exam questions for edition ***********************/
/*****************************************************************************/

static void Exa_ListOneOrMoreQuestionsForEdition (struct Exa_Exams *Exams,
						  long ExaCod,unsigned NumQsts,
                                                  MYSQL_RES *mysql_res,
						  bool ICanEditQuestions)
  {
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Tags;
   extern const char *Txt_Question;
   extern const char *Txt_Move_up_X;
   extern const char *Txt_Move_down_X;
   extern const char *Txt_Movement_not_allowed;
   unsigned NumQst;
   MYSQL_ROW row;
   struct Tst_Question Question;
   unsigned QstInd;
   unsigned MaxQstInd;
   char StrQstInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   bool QuestionExists;

   /***** Get maximum question index *****/
   MaxQstInd = Exa_GetMaxQuestionIndexInExam (ExaCod);

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Code);
   HTM_TH (1,1,"CT",Txt_Tags);
   HTM_TH (1,1,"CT",Txt_Question);

   HTM_TR_End ();

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);

      /***** Get question data *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] QstInd
      row[1] QstCod
      */

      /* Get question index (row[0]) */
      QstInd = Str_ConvertStrToUnsigned (row[0]);
      snprintf (StrQstInd,sizeof (StrQstInd),
	        "%u",
		QstInd);

      /* Get question code (row[1]) */
      Question.QstCod = Str_ConvertStrCodToLongCod (row[1]);

      /***** Icons *****/
      Exams->ExaCod = ExaCod;
      Exams->QstInd = QstInd;
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

      /* Put icon to remove the question */
      if (ICanEditQuestions)
	{
	 Frm_StartForm (ActReqRemExaQst);
	 Exa_PutParams (Exams);
	 Exa_PutParamQstInd (QstInd);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}
      else
         Ico_PutIconRemovalNotAllowed ();

      /* Put icon to move up the question */
      if (ICanEditQuestions && QstInd > 1)
	{
	 Lay_PutContextualLinkOnlyIcon (ActUp_ExaQst,NULL,
	                                Exa_PutParamsOneQst,Exams,
				        "arrow-up.svg",
					Str_BuildStringStr (Txt_Move_up_X,
							    StrQstInd));
	 Str_FreeString ();
	}
      else
         Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);

      /* Put icon to move down the question */
      if (ICanEditQuestions && QstInd < MaxQstInd)
	{
	 Lay_PutContextualLinkOnlyIcon (ActDwnExaQst,NULL,
	                                Exa_PutParamsOneQst,Exams,
				        "arrow-down.svg",
					Str_BuildStringStr (Txt_Move_down_X,
							    StrQstInd));
	 Str_FreeString ();
	}
      else
         Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);

      /* Put icon to edit the question */
      if (ICanEditQuestions)
	 Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
	                              Tst_PutParamQstCod,&Question.QstCod);

      HTM_TD_End ();

      /***** Question *****/
      QuestionExists = Tst_GetQstDataFromDB (&Question);
      Exa_ListQuestionForEdition (&Question,QstInd,QuestionExists);

      HTM_TR_End ();

      /***** Destroy test question *****/
      Tst_QstDestructor (&Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************** List exam question for edition ***********************/
/*****************************************************************************/

static void Exa_ListQuestionForEdition (const struct Tst_Question *Question,
                                        unsigned QstInd,bool QuestionExists)
  {
   extern const char *Txt_Question_removed;

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (QstInd);
   if (QuestionExists)
      Tst_WriteAnswerType (Question->Answer.Type);
   HTM_TD_End ();

   /***** Write question code *****/
   HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtF ("%ld&nbsp;",Question->QstCod);
   HTM_TD_End ();

   /***** Write the question tags *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (QuestionExists)
      Tst_GetAndWriteTagsQst (Question->QstCod);
   HTM_TD_End ();

   /***** Write stem (row[3]) and media *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (QuestionExists)
     {
      /* Write stem */
      Tst_WriteQstStem (Question->Stem,"TEST_EDI",
			true);	// Visible

      /* Show media */
      Med_ShowMedia (&Question->Media,
		     "TEST_MED_EDIT_LIST_STEM_CONTAINER",
		     "TEST_MED_EDIT_LIST_STEM");

      /* Show feedback */
      Tst_WriteQstFeedback (Question->Feedback,"TEST_EDI_LIGHT");

      /* Show answers */
      Tst_WriteAnswersListing (Question);
     }
   else
     {
      HTM_SPAN_Begin ("class=\"DAT_LIGHT\"");
      HTM_Txt (Txt_Question_removed);
      HTM_SPAN_End ();
     }
   HTM_TD_End ();
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to exam *******************/
/*****************************************************************************/

static void Exa_PutIconToAddNewQuestions (void *Exams)
  {
   extern const char *Txt_Add_questions;

   /***** Put form to create a new question *****/
   Ico_PutContextualIconToAdd (ActAddOneExaQst,NULL,
			       Exa_PutParams,Exams,
			       Txt_Add_questions);
  }

/*****************************************************************************/
/***************** Put button to add new questions to exam *******************/
/*****************************************************************************/

static void Exa_PutButtonToAddNewQuestions (struct Exa_Exams *Exams)
  {
   extern const char *Txt_Add_questions;

   Frm_StartForm (ActAddOneExaQst);
   Exa_PutParams (Exams);
   Btn_PutConfirmButton (Txt_Add_questions);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Add selected test questions to exam ********************/
/*****************************************************************************/

void Exa_AddQuestionsToExam (void)
  {
   extern const char *Txt_No_questions_have_been_added;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;
   unsigned MaxQstInd;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Check if exam has events *****/
   if (Exa_CheckIfEditable (&Exam))
     {
      /***** Get selected questions *****/
      /* Allocate space for selected question codes */
      Exa_AllocateListSelectedQuestions (&Exams);

      /* Get question codes */
      Par_GetParMultiToText ("QstCods",Exams.ListQuestions,
			     Exa_MAX_BYTES_LIST_SELECTED_QUESTIONS);

      /* Check number of questions */
      if (Exa_CountNumQuestionsInList (&Exams))	// If questions selected...
	{
	 /***** Insert questions in database *****/
	 Ptr = Exams.ListQuestions;
	 while (*Ptr)
	   {
	    /* Get next code */
	    Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	    if (sscanf (LongStr,"%ld",&QstCod) != 1)
	       Lay_ShowErrorAndExit ("Wrong question code.");

	    /* Get current maximum index */
	    MaxQstInd = Exa_GetMaxQuestionIndexInExam (Exam.ExaCod);	// -1 if no questions

	    /* Insert question in the table of questions */
	    DB_QueryINSERT ("can not create question",
			    "INSERT INTO exa_questions"
			    " (ExaCod,QstCod,QstInd)"
			    " VALUES"
			    " (%ld,%ld,%u)",
			    Exam.ExaCod,QstCod,MaxQstInd + 1);
	   }
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_No_questions_have_been_added);

      /***** Free space for selected question codes *****/
      Exa_FreeListsSelectedQuestions (&Exams);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        true,	// List exam questions
	                false);	// Do not put form to start new event
  }

/*****************************************************************************/
/****************** Allocate memory for list of questions ********************/
/*****************************************************************************/

static void Exa_AllocateListSelectedQuestions (struct Exa_Exams *Exams)
  {
   if (!Exams->ListQuestions)
     {
      if ((Exams->ListQuestions = (char *) malloc (Exa_MAX_BYTES_LIST_SELECTED_QUESTIONS + 1)) == NULL)
         Lay_NotEnoughMemoryExit ();
      Exams->ListQuestions[0] = '\0';
     }
  }

/*****************************************************************************/
/*********** Free memory used by list of selected question codes *************/
/*****************************************************************************/

static void Exa_FreeListsSelectedQuestions (struct Exa_Exams *Exams)
  {
   if (Exams->ListQuestions)
     {
      free (Exams->ListQuestions);
      Exams->ListQuestions = NULL;
     }
  }

/*****************************************************************************/
/**** Count the number of questions in the list of selected question codes ***/
/*****************************************************************************/

static unsigned Exa_CountNumQuestionsInList (const struct Exa_Exams *Exams)
  {
   const char *Ptr;
   unsigned NumQuestions = 0;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;

   /***** Go over list of questions counting the number of questions *****/
   Ptr = Exams->ListQuestions;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
      if (sscanf (LongStr,"%ld",&QstCod) != 1)
         Lay_ShowErrorAndExit ("Wrong question code.");
      NumQuestions++;
     }
   return NumQuestions;
  }

/*****************************************************************************/
/***************** Request the removal of a set of questions *****************/
/*****************************************************************************/

void ExaSet_RequestRemoveSet (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_set_of_questions_X;
   extern const char *Txt_Remove_set_of_questions;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Exams.SetCod = Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);

   /***** Show question and button to remove question *****/
   Ale_ShowAlertAndButton (ActRemExaSet,NULL,NULL,
			   ExaSet_PutParamsOneSet,&Exams,
			   Btn_REMOVE_BUTTON,Txt_Remove_set_of_questions,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_set_of_questions_X,
			   Set.Title);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/************************* Remove a set of questions *************************/
/*****************************************************************************/

void ExaSet_RemoveSet (void)
  {
   extern const char *Txt_Set_of_questions_removed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);

   /***** Remove the set from all the tables *****/
   /* Remove questions associated to set */
   DB_QueryDELETE ("can not remove questions associated to set",
		   "DELETE FROM exa_questions"
		   " USING exa_questions,exa_sets"
		   " WHERE exa_questions.SetCod=%ld"
                   " AND exa_questions.SetCod=exa_sets.SetCod"
		   " AND exa_sets.ExaCod=%ld",	// Extra check
		   Set.SetCod,Set.ExaCod);

   /* Remove the set itself */
   DB_QueryDELETE ("can not remove set",
		   "DELETE FROM exa_sets"
		   " WHERE SetCod=%ld"
                   " AND ExaCod=%ld",		// Extra check
		   Set.SetCod,Set.ExaCod);
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The set to be removed does not exist.");

   /* Change index of sets greater than this */
   DB_QueryUPDATE ("can not update indexes of sets",
		   "UPDATE exa_sets SET SetInd=SetInd-1"
		   " WHERE ExaCod=%ld AND SetInd>%u",
		   Set.ExaCod,Set.SetInd);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Set_of_questions_removed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/************ Move up position of a set of questions in an exam **************/
/*****************************************************************************/

void ExaSet_MoveUpSet (void)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   unsigned SetIndTop;
   unsigned SetIndBottom;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);

   /***** Get set index *****/
   SetIndBottom = ExaSet_GetSetIndFromSetCod (Exam.ExaCod,Set.SetCod);

   /***** Move up set *****/
   if (SetIndBottom > 1)
     {
      /* Indexes of sets to be exchanged */
      SetIndTop = ExaSet_GetPrevSetIndexInExam (Exam.ExaCod,SetIndBottom);
      if (!SetIndTop)
	 Lay_ShowErrorAndExit ("Wrong index of set.");

      /* Exchange sets */
      ExaSet_ExchangeSets (Exam.ExaCod,SetIndTop,SetIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/*********** Move down position of a set of questions in an exam *************/
/*****************************************************************************/

void ExaSet_MoveDownSet (void)
  {
   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_This_exam_has_no_sets_of_questions;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSet_Set Set;
   unsigned SetIndTop;
   unsigned SetIndBottom;
   unsigned MaxSetInd;	// 0 if no sets

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam and set *****/
   Exa_ResetExam (&Exam);
   ExaSet_ResetSet (&Set);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Set.ExaCod = Exam.ExaCod = Exams.ExaCod;
   Set.SetCod = ExaSet_GetParamSetCod ();
   if (Set.SetCod <= 0)
      Lay_WrongSetExit ();

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   if (!Exa_CheckIfEditable (&Exam))
      Lay_NoPermissionExit ();

   /***** Get set data from database *****/
   ExaSet_GetDataOfSetByCod (&Set);

   /***** Get set index *****/
   SetIndTop = ExaSet_GetSetIndFromSetCod (Exam.ExaCod,Set.SetCod);

   /***** Get maximum set index *****/
   MaxSetInd = ExaSet_GetMaxSetIndexInExam (Exam.ExaCod);

   /***** Move down set *****/
   if (MaxSetInd)
     {
      if (SetIndTop < MaxSetInd)
	{
	 /* Indexes of sets to be exchanged */
	 SetIndBottom = ExaSet_GetNextSetIndexInExam (Exam.ExaCod,SetIndTop);
	 if (!SetIndBottom)
	    Lay_ShowErrorAndExit ("Wrong index of set.");

	 /* Exchange sets */
	 ExaSet_ExchangeSets (Exam.ExaCod,SetIndTop,SetIndBottom);
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_This_exam_has_no_sets_of_questions);

   /***** Show current exam and its sets *****/
   Exa_PutFormsOneExam (&Exams,&Exam,&Set,
                        false);	// It's not a new exam
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Exa_RequestRemoveQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   unsigned QstInd;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Check if exam has events *****/
   if (Exa_CheckIfEditable (&Exam))
     {
      /***** Get question index *****/
      QstInd = Exa_GetParamQstInd ();

      /***** Show question and button to remove question *****/
      Exams.ExaCod = Exam.ExaCod;
      Exams.QstInd = QstInd;
      Ale_ShowAlertAndButton (ActRemExaQst,NULL,NULL,
                              Exa_PutParamsOneQst,&Exams,
			      Btn_REMOVE_BUTTON,Txt_Remove_question,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			      QstInd);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        true,	// List exam questions
	                false);	// Do not put form to start new event
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Exa_RemoveQst (void)
  {
   extern const char *Txt_Question_removed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   unsigned QstInd;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Check if exam has events *****/
   if (Exa_CheckIfEditable (&Exam))
     {
      /***** Get question index *****/
      QstInd = Exa_GetParamQstInd ();

      /***** Remove the question from all the tables *****/
      /* Remove answers from this test question */
      Exa_RemAnswersOfAQuestion (Exam.ExaCod,QstInd);

      /* Remove the question itself */
      DB_QueryDELETE ("can not remove a question",
		      "DELETE FROM exa_questions"
		      " WHERE ExaCod=%ld AND QstInd=%u",
		      Exam.ExaCod,QstInd);
      if (!mysql_affected_rows (&Gbl.mysql))
	 Lay_ShowErrorAndExit ("The question to be removed does not exist.");

      /* Change index of questions greater than this */
      DB_QueryUPDATE ("can not update indexes of questions in table of answers",
		      "UPDATE exa_answers,exa_events"
		      " SET exa_answers.QstInd=exa_answers.QstInd-1"
		      " WHERE exa_events.ExaCod=%ld"
		      " AND exa_events.EvtCod=exa_answers.EvtCod"
		      " AND exa_answers.QstInd>%u",
		      Exam.ExaCod,QstInd);
      DB_QueryUPDATE ("can not update indexes of questions",
		      "UPDATE exa_questions SET QstInd=QstInd-1"
		      " WHERE ExaCod=%ld AND QstInd>%u",
		      Exam.ExaCod,QstInd);

      /***** Write message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        true,	// List exam questions
	                false);	// Do not put form to start new event
  }

/*****************************************************************************/
/***************** Move up position of a question in an exam ******************/
/*****************************************************************************/

void Exa_MoveUpQst (void)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   unsigned QstIndTop;
   unsigned QstIndBottom;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Check if exam has events *****/
   if (Exa_CheckIfEditable (&Exam))
     {
      /***** Get question index *****/
      QstIndBottom = Exa_GetParamQstInd ();

      /***** Move up question *****/
      if (QstIndBottom > 1)
	{
	 /* Indexes of questions to be exchanged */
	 QstIndTop = Exa_GetPrevQuestionIndexInExam (Exam.ExaCod,QstIndBottom);
	 if (!QstIndTop)
	    Lay_ShowErrorAndExit ("Wrong index of question.");

	 /* Exchange questions */
	 Exa_ExchangeQuestions (Exam.ExaCod,QstIndTop,QstIndBottom);
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        true,	// List exam questions
	                false);	// Do not put form to start new event
  }

/*****************************************************************************/
/**************** Move down position of a question in an exam *****************/
/*****************************************************************************/

void Exa_MoveDownQst (void)
  {
   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_This_exam_has_no_questions;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   unsigned QstIndTop;
   unsigned QstIndBottom;
   unsigned MaxQstInd;	// 0 if no questions

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Check if exam has events *****/
   if (Exa_CheckIfEditable (&Exam))
     {
      /***** Get question index *****/
      QstIndTop = Exa_GetParamQstInd ();

      /***** Get maximum question index *****/
      MaxQstInd = Exa_GetMaxQuestionIndexInExam (Exam.ExaCod);

      /***** Move down question *****/
      if (MaxQstInd)
	{
	 if (QstIndTop < MaxQstInd)
	   {
	    /* Indexes of questions to be exchanged */
	    QstIndBottom = Exa_GetNextQuestionIndexInExam (Exam.ExaCod,QstIndTop);
	    if (!QstIndBottom)
	       Lay_ShowErrorAndExit ("Wrong index of question.");

	    /* Exchange questions */
	    Exa_ExchangeQuestions (Exam.ExaCod,QstIndTop,QstIndBottom);
	   }
	 else
	    Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_This_exam_has_no_questions);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        true,	// List exam questions
	                false);	// Do not put form to start new event
  }

/*****************************************************************************/
/*********** Exchange the order of two consecutive sets in an exam ***********/
/*****************************************************************************/

static void ExaSet_ExchangeSets (long ExaCod,
                                 unsigned SetIndTop,unsigned SetIndBottom)
  {
   long SetCodTop;
   long SetCodBottom;

   /***** Lock table to make the move atomic *****/
   DB_Query ("can not lock tables to exchange sets of questions",
	     "LOCK TABLES exa_sets WRITE");
   Gbl.DB.LockedTables = true;

   /***** Get set codes of the sets to be moved *****/
   SetCodTop    = ExaSet_GetSetCodFromSetInd (ExaCod,SetIndTop);
   SetCodBottom = ExaSet_GetSetCodFromSetInd (ExaCod,SetIndBottom);

   /***** Exchange indexes of sets *****/
   /*
   Example:
   SetIndTop    = 1; SetCodTop    = 218
   SetIndBottom = 2; SetCodBottom = 220
                     Step 1              Step 2              Step 3
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
| SetInd | SetCod | | SetInd | SetCod | | SetInd | SetCod | | SetInd | SetCod |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
|      1 |    218 |>|     -2 |    218 |>|     -2 |    218 |>|      2 |    218 |
|      2 |    220 | |      2 |    220 | |      1 |    220 | |      1 |    220 |
|      3 |    232 | |      3 |    232 | |      3 |    232 | |      3 |    232 |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
   */
   /* Step 1: change temporarily top index to minus bottom index
              in order to not repeat unique index (ExaCod,SetInd) */
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets SET SetInd=-%u"
		   " WHERE ExaCod=%ld AND SetCod=%ld",
		   SetIndBottom,
		   ExaCod,SetCodTop);

   /* Step 2: change bottom index to old top index  */
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets SET SetInd=%u"
		   " WHERE ExaCod=%ld AND SetCod=%ld",
		   SetIndTop,
		   ExaCod,SetCodBottom);

   /* Step 3: change top index to old bottom index */
   DB_QueryUPDATE ("can not exchange indexes of sets",
		   "UPDATE exa_sets SET SetInd=%u"
		   " WHERE ExaCod=%ld AND SetCod=%ld",
		   SetIndBottom,
		   ExaCod,SetCodTop);

   /***** Unlock table *****/
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after exchanging sets of questions",
	     "UNLOCK TABLES");
  }

/*****************************************************************************/
/********* Exchange the order of two consecutive questions in an exam *********/
/*****************************************************************************/

static void Exa_ExchangeQuestions (long ExaCod,
                                   unsigned QstIndTop,unsigned QstIndBottom)
  {
   long QstCodTop;
   long QstCodBottom;

   /***** Lock table to make the move atomic *****/
   DB_Query ("can not lock tables to move exam question",
	     "LOCK TABLES exa_questions WRITE");
   Gbl.DB.LockedTables = true;

   /***** Get question code of the questions to be moved *****/
   QstCodTop    = Exa_GetQstCodFromQstInd (ExaCod,QstIndTop);
   QstCodBottom = Exa_GetQstCodFromQstInd (ExaCod,QstIndBottom);

   /***** Exchange indexes of questions *****/
   /*
   Example:
   QstIndTop    = 1; QstCodTop    = 218
   QstIndBottom = 2; QstCodBottom = 220
   +--------+--------+		+--------+--------+	+--------+--------+
   | QstInd | QstCod |		| QstInd | QstCod |	| QstInd | QstCod |
   +--------+--------+		+--------+--------+	+--------+--------+
   |      1 |    218 |  ----->	|      2 |    218 |  =	|      1 |    220 |
   |      2 |    220 |		|      1 |    220 |	|      2 |    218 |
   |      3 |    232 |		|      3 |    232 |	|      3 |    232 |
   +--------+--------+		+--------+--------+	+--------+--------+
 */
   DB_QueryUPDATE ("can not exchange indexes of questions",
		   "UPDATE exa_questions SET QstInd=%u"
		   " WHERE ExaCod=%ld AND QstCod=%ld",
	           QstIndBottom,
	           ExaCod,QstCodTop);

   DB_QueryUPDATE ("can not exchange indexes of questions",
		   "UPDATE exa_questions SET QstInd=%u"
		   " WHERE ExaCod=%ld AND QstCod=%ld",
	           QstIndTop,
	           ExaCod,QstCodBottom);

   /***** Unlock table *****/
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after moving exam questions",
	     "UNLOCK TABLES");
  }

/*****************************************************************************/
/*********** Get number of events and check is edition is possible **********/
/*****************************************************************************/
// Before calling this function, number of events must be calculated

static bool Exa_CheckIfEditable (const struct Exa_Exam *Exam)
  {
   if (Exa_CheckIfICanEditExams ())
      /***** Questions are editable only if exam has no events *****/
      return (bool) (Exam->NumEvts == 0);	// Exams with events should not be edited
   else
      return false;	// Questions are not editable
  }

/*****************************************************************************/
/********************* Put button to create a new event **********************/
/*****************************************************************************/

void Exa_PutButtonNewEvent (struct Exa_Exams *Exams,long ExaCod)
  {
   extern const char *Txt_New_event;

   Exams->ExaCod = ExaCod;
   Frm_StartFormAnchor (ActReqNewExaEvt,ExaEvt_NEW_EVENT_SECTION_ID);
   Exa_PutParams (Exams);
   Btn_PutConfirmButton (Txt_New_event);
   Frm_EndForm ();
  }

/*****************************************************************************/
/************* Request the creation of a new event as a teacher **************/
/*****************************************************************************/

void Exa_RequestNewEvent (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);

   /***** Show exam *****/
   Exa_ShowOnlyOneExam (&Exams,&Exam,
                        false,	// Do not list exam questions
                        true);	// Put form to start new event
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
			 " (SELECT COUNT(exa_questions.QstCod) AS NumQsts"
			 " FROM exa_exams,exa_questions"
			 " WHERE exa_exams.ExaCod=exa_questions.ExaCod"
			 " GROUP BY exa_questions.ExaCod) AS NumQstsTable");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_questions.QstCod) AS NumQsts"
			 " FROM institutions,centres,degrees,courses,exa_exams,exa_questions"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_questions.ExaCod"
			 " GROUP BY exa_questions.ExaCod) AS NumQstsTable",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_questions.QstCod) AS NumQsts"
			 " FROM centres,degrees,courses,exa_exams,exa_questions"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_questions.ExaCod"
			 " GROUP BY exa_questions.ExaCod) AS NumQstsTable",
		         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_questions.QstCod) AS NumQsts"
			 " FROM degrees,courses,exa_exams,exa_questions"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_questions.ExaCod"
			 " GROUP BY exa_questions.ExaCod) AS NumQstsTable",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_questions.QstCod) AS NumQsts"
			 " FROM courses,exa_exams,exa_questions"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=exa_exams.CrsCod"
			 " AND exa_exams.ExaCod=exa_questions.ExaCod"
			 " GROUP BY exa_questions.ExaCod) AS NumQstsTable",
		         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per exam",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(exa_questions.QstCod) AS NumQsts"
			 " FROM exa_exams,exa_questions"
			 " WHERE exa_exams.Cod=%ld"
			 " AND exa_exams.ExaCod=exa_questions.ExaCod"
			 " GROUP BY exa_questions.ExaCod) AS NumQstsTable",
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
/************************* Show test tags in an exam **************************/
/*****************************************************************************/

void Exa_ShowTstTagsPresentInAnExam (long ExaCod)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumTags;

   /***** Get all tags of questions in this exam *****/
   NumTags = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get tags"
					" present in an event result",
			     "SELECT tst_tags.TagTxt"	// row[0]
			     " FROM"
			     " (SELECT DISTINCT(tst_question_tags.TagCod)"
			     " FROM tst_question_tags,exa_questions"
			     " WHERE exa_questions.ExaCod=%ld"
			     " AND exa_questions.QstCod=tst_question_tags.QstCod)"
			     " AS TagsCods,tst_tags"
			     " WHERE TagsCods.TagCod=tst_tags.TagCod"
			     " ORDER BY tst_tags.TagTxt",
			     ExaCod);
   Tst_ShowTagList (NumTags,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
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
			     " FROM tst_answers,exa_questions"
			     " WHERE exa_questions.ExaCod=%ld"
			     " AND exa_questions.QstCod=tst_answers.QstCod"
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
