// swad_exam_result.c: exams results

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_database.h"
#include "swad_date.h"
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
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_photo.h"
#include "swad_test_print.h"
#include "swad_test_visibility.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct ExaRes_ICanView
  {
   bool Result;
   bool Score;
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaRes_ListMyResultsInCrs (struct Exa_Exams *Exams);
static void ExaRes_ListMyResultsInExa (struct Exa_Exams *Exams,long ExaCod);
static void ExaRes_ListMyResultsInSes (struct Exa_Exams *Exams,long SesCod);

static void ExaRes_PutFormToSelUsrsToViewResults (__attribute__((unused)) void *Args);
static void ExaRes_ShowAllResultsInSelectedExams (void *Exams);
static void ExaRes_ListAllResultsInSelectedExams (struct Exa_Exams *Exams);
static void ExaRes_ListAllResultsInExa (struct Exa_Exams *Exams,long ExaCod);
static void ExaRes_ListAllResultsInSes (struct Exa_Exams *Exams,long SesCod);

static void ExaRes_ShowResultsBegin (struct Exa_Exams *Exams,
                                     const char *Title,bool ListExamsToSelect);
static void ExaRes_ShowResultsEnd (void);

static void ExaRes_ListExamsToSelect (struct Exa_Exams *Exams);
static void ExaRes_ShowHeaderResults (Usr_MeOrOther_t MeOrOther);

static void ExaRes_BuildExamsSelectedCommas (struct Exa_Exams *Exams,
                                             char **ExamsSelectedCommas);
static void ExaRes_ShowResults (struct Exa_Exams *Exams,
                                Usr_MeOrOther_t MeOrOther,
				long SesCod,	// <= 0 ==> any
				long ExaCod,	// <= 0 ==> any
				const char *ExamsSelectedCommas);
static void ExaRes_ShowResultsSummaryRow (unsigned NumResults,
                                          const struct ExaPrn_NumQuestions *NumTotalQsts,
                                          const struct ExaPrn_Score *TotalScore,
					  double TotalGrade);

static void ExaRes_ShowExamResult (const struct Exa_Exam *Exam,
	                           const struct ExaSes_Session *Session,
                                   struct ExaPrn_Print *Print,
                                   struct UsrData *UsrDat,
                                   struct ExaRes_ICanView *ICanView,
                                   unsigned Visibility);

static void ExaRes_CheckIfICanViewResult (const struct Exa_Exam *Exam,
                                          const struct ExaSes_Session *Session,
                                          long UsrCod,
                                          struct ExaRes_ICanView *ICanView);

static void ExaRes_ComputeValidPrintScore (struct ExaPrn_Print *Print);
static void ExaRes_ShowExamResultTime (struct ExaPrn_Print *Print);
static void ExaRes_ShowExamResultNumQsts (struct ExaPrn_Print *Print,
                                          const struct ExaRes_ICanView *ICanView);
static void ExaRes_ShowExamResultNumAnss (struct ExaPrn_Print *Print,
                                          const struct ExaRes_ICanView *ICanView);
static void ExaRes_ShowExamResultScore (struct ExaPrn_Print *Print,
                                        const struct ExaRes_ICanView *ICanView);
static void ExaRes_ShowExamResultGrade (const struct Exa_Exam *Exam,
	                                struct ExaPrn_Print *Print,
                                        const struct ExaRes_ICanView *ICanView);
static void ExaRes_ShowExamAnswers (struct UsrData *UsrDat,
			            struct ExaPrn_Print *Print,
			            unsigned Visibility);
static void ExaRes_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct ExaPrn_Print *Print,
				       unsigned QstInd,
				       struct Qst_Question *Question,
				       unsigned Visibility);

/*****************************************************************************/
/*************************** Show my sessions results **************************/
/*****************************************************************************/

void ExaRes_ShowMyResultsInCrs (void)
  {
   extern const char *Txt_Results;
   struct Exa_Exams Exams;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Get list of exams *****/
   Exa_GetListExams (&Exams,Exa_ORDER_BY_TITLE);
   Exa_GetListSelectedExaCods (&Exams);

   /***** List my sessions results in the current course *****/
   ExaRes_ShowResultsBegin (&Exams,Txt_Results,true);	// List exams to select
   ExaRes_ListMyResultsInCrs (&Exams);
   ExaRes_ShowResultsEnd ();

   /***** Free list of exams *****/
   free (Exams.ExaCodsSelected);
   Exa_FreeListExams (&Exams);
  }

static void ExaRes_ListMyResultsInCrs (struct Exa_Exams *Exams)
  {
   char *ExamsSelectedCommas = NULL;	// Initialized to avoid warning

   /***** Table header *****/
   ExaRes_ShowHeaderResults (Usr_ME);

   /***** List my sessions results in the current course *****/
   TstCfg_GetConfig ();	// Get feedback type
   ExaRes_BuildExamsSelectedCommas (Exams,&ExamsSelectedCommas);
   ExaRes_ShowResults (Exams,Usr_ME,-1L,-1L,ExamsSelectedCommas);
   free (ExamsSelectedCommas);
  }

/*****************************************************************************/
/******************** Show my results in a given exam ************************/
/*****************************************************************************/

void ExaRes_ShowMyResultsInExa (void)
  {
   extern const char *Txt_Results_of_exam_X;
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
      Err_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List my sessions results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildString (Txt_Results_of_exam_X,Exam.Title),
			    false);	// Do not list exams to select
   Str_FreeStrings ();
   ExaRes_ListMyResultsInExa (&Exams,Exam.ExaCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListMyResultsInExa (struct Exa_Exams *Exams,long ExaCod)
  {
   /***** Table header *****/
   ExaRes_ShowHeaderResults (Usr_ME);

   /***** List my sessions results in exam *****/
   TstCfg_GetConfig ();	// Get feedback type
   ExaRes_ShowResults (Exams,Usr_ME,-1L,ExaCod,NULL);
  }

/*****************************************************************************/
/****************** Show my exam results in a given session ******************/
/*****************************************************************************/

void ExaRes_ShowMyResultsInSes (void)
  {
   extern const char *Txt_Results_of_session_X;
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
      Err_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   if ((Session.SesCod = ExaSes_GetParamSesCod ()) <= 0)
      Err_WrongExamSessionExit ();
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaSes_GetDataOfSessionByCod (&Session);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List my sessions results in session *****/
   ExaRes_ShowResultsBegin (&Exams,Str_BuildString (Txt_Results_of_session_X,
                                                    Session.Title),
			    false);	// Do not list exams to select
   Str_FreeStrings ();
   ExaRes_ListMyResultsInSes (&Exams,Session.SesCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListMyResultsInSes (struct Exa_Exams *Exams,long SesCod)
  {
   /***** Table header *****/
   ExaRes_ShowHeaderResults (Usr_ME);

   /***** List my sessions results in exam *****/
   TstCfg_GetConfig ();	// Get feedback type
   ExaRes_ShowResults (Exams,Usr_ME,SesCod,-1L,NULL);
  }

/*****************************************************************************/
/**************** Select users to show their sessions results *****************/
/*****************************************************************************/

void ExaRes_SelUsrsToViewResults (void)
  {
   /***** Put form to select users *****/
   ExaRes_PutFormToSelUsrsToViewResults (NULL);
  }

static void ExaRes_PutFormToSelUsrsToViewResults (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Exams_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_results;

   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeUsrExaResCrs,
				     NULL,NULL,
				     Txt_Results,
				     Hlp_ASSESSMENT_Exams_results,
				     Txt_View_results,
				     false);	// Do not put form with date range
  }

/*****************************************************************************/
/****************** Get users and show their sessions results *****************/
/*****************************************************************************/

void ExaRes_ShowAllResultsInCrs (void)
  {
   struct Exa_Exams Exams;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Get users and show their sessions results *****/
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  ExaRes_ShowAllResultsInSelectedExams,&Exams,
                                  ExaRes_PutFormToSelUsrsToViewResults,NULL);
  }

/*****************************************************************************/
/****************** Show sessions results for several users *******************/
/*****************************************************************************/

static void ExaRes_ShowAllResultsInSelectedExams (void *Exams)
  {
   extern const char *Txt_Results;

   if (!Exams)
      return;

   /***** Get list of exams *****/
   Exa_GetListExams ((struct Exa_Exams *) Exams,Exa_ORDER_BY_TITLE);
   Exa_GetListSelectedExaCods ((struct Exa_Exams *) Exams);

   /***** List the sessions results of the selected users *****/
   ExaRes_ShowResultsBegin ((struct Exa_Exams *) Exams,
                            Txt_Results,
                            true);	// List exams to select
   ExaRes_ListAllResultsInSelectedExams ((struct Exa_Exams *) Exams);
   ExaRes_ShowResultsEnd ();

   /***** Free list of exams *****/
   free (((struct Exa_Exams *) Exams)->ExaCodsSelected);
   Exa_FreeListExams ((struct Exa_Exams *) Exams);
  }

static void ExaRes_ListAllResultsInSelectedExams (struct Exa_Exams *Exams)
  {
   char *ExamsSelectedCommas = NULL;	// Initialized to avoid warning
   const char *Ptr;

   /***** Table head *****/
   ExaRes_ShowHeaderResults (Usr_OTHER);

   /***** List the sessions results of the selected users *****/
   ExaRes_BuildExamsSelectedCommas (Exams,&ExamsSelectedCommas);
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	 if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show sessions results *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    ExaRes_ShowResults (Exams,Usr_OTHER,-1L,-1L,ExamsSelectedCommas);
	   }
     }
   free (ExamsSelectedCommas);
  }

/*****************************************************************************/
/*** Show sessions results of a exam for the users who answered in that exam **/
/*****************************************************************************/

void ExaRes_ShowAllResultsInExa (void)
  {
   extern const char *Txt_Results_of_exam_X;
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
      Err_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List sessions results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildString (Txt_Results_of_exam_X,Exam.Title),
			    false);	// Do not list exams to select
   Str_FreeStrings ();
   ExaRes_ListAllResultsInExa (&Exams,Exam.ExaCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListAllResultsInExa (struct Exa_Exams *Exams,long ExaCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;

   /***** Table head *****/
   ExaRes_ShowHeaderResults (Usr_OTHER);

   /***** Get all users who have answered any session question in this exam *****/
   NumUsrs = Exa_DB_GetAllUsrsWhoHaveMadeExam (&mysql_res,ExaCod);

   /***** List sessions results for each user *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs;
	NumUsr++)
      /* Get session code */
      if ((Gbl.Usrs.Other.UsrDat.UsrCod = DB_GetNextCode (mysql_res)) > 0)
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	    if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	      {
	       /***** Show sessions results *****/
	       Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	       ExaRes_ShowResults (Exams,Usr_OTHER,-1L,ExaCod,NULL);
	      }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/** Show sessions results of a session for the users who answered in that session */
/*****************************************************************************/

void ExaRes_ShowAllResultsInSes (void)
  {
   extern const char *Txt_Results_of_session_X;
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
      Err_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   if ((Session.SesCod = ExaSes_GetParamSesCod ()) <= 0)
      Err_WrongExamSessionExit ();

   /***** Get exam data and session *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaSes_GetDataOfSessionByCod (&Session);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List sessions results in session *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildString (Txt_Results_of_session_X,Session.Title),
			    false);	// Do not list exams to select
   Str_FreeStrings ();
   ExaRes_ListAllResultsInSes (&Exams,Session.SesCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListAllResultsInSes (struct Exa_Exams *Exams,long SesCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;

   /***** Table head *****/
   ExaRes_ShowHeaderResults (Usr_OTHER);

   /***** Get all users who have answered any session question in this exam *****/
   NumUsrs = Exa_DB_GetAllUsrsWhoHaveMadeSession (&mysql_res,SesCod);

   /***** List sessions results for each user *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs;
	NumUsr++)
      /* Get session code (row[0]) */
      if ((Gbl.Usrs.Other.UsrDat.UsrCod = DB_GetNextCode (mysql_res)) > 0)
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	    if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	      {
	       /***** Show sessions results *****/
	       Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	       ExaRes_ShowResults (Exams,Usr_OTHER,SesCod,-1L,NULL);
	      }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show results (begin / end) *************************/
/*****************************************************************************/

static void ExaRes_ShowResultsBegin (struct Exa_Exams *Exams,
                                     const char *Title,bool ListExamsToSelect)
  {
   extern const char *Hlp_ASSESSMENT_Exams_results;

   /***** Begin section *****/
   HTM_SECTION_Begin (ExaRes_RESULTS_BOX_ID);

      /***** Begin box *****/
      Box_BoxBegin ("100%",Title,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Exams_results,Box_NOT_CLOSABLE);

	 /***** List exams to select *****/
	 if (ListExamsToSelect)
	    ExaRes_ListExamsToSelect (Exams);

	 /***** Begin session results section *****/
	 HTM_SECTION_Begin (ExaRes_RESULTS_TABLE_ID);

	    /* Begin session results table */
	    HTM_TABLE_BeginWidePadding (5);
  }

static void ExaRes_ShowResultsEnd (void)
  {
	    /* End session results table */
	    HTM_TABLE_End ();

	 /***** End session results section *****/
	 HTM_SECTION_End ();

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/********** Write list of those attendance sessions that have students *********/
/*****************************************************************************/

static void ExaRes_ListExamsToSelect (struct Exa_Exams *Exams)
  {
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *Txt_Exams;
   extern const char *Txt_Exam;
   extern const char *Txt_Update_results;
   unsigned UniqueId;
   unsigned NumExam;
   struct Exa_Exam Exam;

   /***** Reset exam *****/
   Exa_ResetExam (&Exam);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Exams,
		 NULL,NULL,
		 NULL,Box_CLOSABLE);

      /***** Begin form to update the results
	     depending on the exams selected *****/
      Frm_BeginFormAnchor (Gbl.Action.Act,ExaRes_RESULTS_TABLE_ID);
	 Grp_PutParamsCodGrps ();
	 Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);

	 /***** Begin table *****/
	 HTM_TABLE_BeginWidePadding (2);

	    /***** Heading row *****/
	    HTM_TR_Begin (NULL);
	       HTM_TH (1,2,NULL    ,NULL);
	       HTM_TH (1,1,Txt_Exam,"LM");
	    HTM_TR_End ();

	    /***** List the sessions *****/
	    for (NumExam = 0, UniqueId = 1, Gbl.RowEvenOdd = 0;
		 NumExam < Exams->Num;
		 NumExam++, UniqueId++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
	      {
	       /* Get data of this exam */
	       Exam.ExaCod = Exams->Lst[NumExam].ExaCod;
	       Exa_GetDataOfExamByCod (&Exam);
	       Exams->ExaCod = Exam.ExaCod;

	       /* Write a row for this session */
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("class=\"%s CT %s\"",
		                The_ClassDat[Gbl.Prefs.Theme],
		                Gbl.ColorRows[Gbl.RowEvenOdd]);
		     HTM_INPUT_CHECKBOX ("ExaCod",HTM_DONT_SUBMIT_ON_CHANGE,
					 "id=\"Gam%u\" value=\"%ld\"%s",
					 NumExam,Exams->Lst[NumExam].ExaCod,
					 Exams->Lst[NumExam].Selected ? " checked=\"checked\"" :
									"");
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"%s RT %s\"",
		                The_ClassDat[Gbl.Prefs.Theme],
		                Gbl.ColorRows[Gbl.RowEvenOdd]);
		     HTM_LABEL_Begin ("for=\"Gam%u\"",NumExam);
			HTM_TxtF ("%u:",NumExam + 1);
		     HTM_LABEL_End ();
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"%s LT %s\"",
		                The_ClassDat[Gbl.Prefs.Theme],
		                Gbl.ColorRows[Gbl.RowEvenOdd]);
		     HTM_Txt (Exam.Title);
		  HTM_TD_End ();

	       HTM_TR_End ();
	      }

	    /***** Put button to refresh *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("colspan=\"3\" class=\"CM\"");
		  HTM_BUTTON_Animated_Begin (Txt_Update_results,
					     The_ClassFormLinkInBoxBold[Gbl.Prefs.Theme],
					     NULL);
		     Ico_PutCalculateIconWithText (Txt_Update_results);
		  HTM_BUTTON_End ();
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Show header of my sessions results *********************/
/*****************************************************************************/

static void ExaRes_ShowHeaderResults (Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Session;
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Valid_answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_total;
   extern const char *Txt_QUESTIONS_valid;
   extern const char *Txt_QUESTIONS_invalid;
   extern const char *Txt_ANSWERS_correct;
   extern const char *Txt_ANSWERS_wrong;
   extern const char *Txt_ANSWERS_blank;
   extern const char *Txt_average;

   /***** First row *****/
   HTM_TR_Begin (NULL);
      HTM_TH (3,2,Txt_User[MeOrOther == Usr_ME ? Gbl.Usrs.Me.UsrDat.Sex :
						 Usr_SEX_UNKNOWN],
                                                   "CT LINE_BOTTOM");
      HTM_TH (3,1,Txt_START_END_TIME[Dat_STR_TIME],"LT LINE_BOTTOM");
      HTM_TH (3,1,Txt_START_END_TIME[Dat_END_TIME],"LT LINE_BOTTOM");
      HTM_TH (3,1,Txt_Session                     ,"LT LINE_BOTTOM");
      HTM_TH (1,3,Txt_Questions                   ,"CT LINE_LEFT");
      HTM_TH (1,5,Txt_Valid_answers               ,"CT LINE_LEFT");
      HTM_TH (1,2,Txt_Score                       ,"CT LINE_LEFT");
      HTM_TH (3,1,Txt_Grade                       ,"RT LINE_BOTTOM LINE_LEFT");
      HTM_TH (3,1,NULL                            ,"LINE_BOTTOM LINE_LEFT");
   HTM_TR_End ();

   /***** Second row *****/
   HTM_TR_Begin (NULL);
      HTM_TH (2,1,Txt_total            ,"RT LINE_BOTTOM LINE_LEFT");
      HTM_TH (2,1,Txt_QUESTIONS_valid  ,"RT LINE_BOTTOM");
      HTM_TH (2,1,Txt_QUESTIONS_invalid,"RT LINE_BOTTOM");
      HTM_TH (1,1,Txt_ANSWERS_correct  ,"RT LINE_LEFT");
      HTM_TH (1,3,Txt_ANSWERS_wrong    ,"CT");
      HTM_TH (1,1,Txt_ANSWERS_blank    ,"RT");
      HTM_TH (1,1,Txt_total            ,"RT LINE_LEFT");
      HTM_TH (1,1,Txt_average          ,"RT");
   HTM_TR_End ();

   /***** Third row *****/
   HTM_TR_Begin (NULL);
      HTM_TH (1,1,"{<em>p<sub>i</sub></em>=1}"         ,"RT LINE_BOTTOM LINE_LEFT");
      HTM_TH (1,1,"{-1&le;<em>p<sub>i</sub></em>&lt;0}","RT LINE_BOTTOM");
      HTM_TH (1,1,"{<em>p<sub>i</sub></em>=0}"         ,"RT LINE_BOTTOM");
      HTM_TH (1,1,"{0&lt;<em>p<sub>i</sub></em>&lt;1}" ,"RT LINE_BOTTOM");
      HTM_TH (1,1,"{<em>p<sub>i</sub></em>=0}"         ,"RT LINE_BOTTOM");
      HTM_TH (1,1,"<em>&Sigma;p<sub>i</sub></em>"      ,"RT LINE_BOTTOM LINE_LEFT");
      HTM_TH (1,1,"-1&le;"
	          "<em style=\"text-decoration:overline;\">p</em>"
	          "&le;1"                              ,"RT LINE_BOTTOM");
   HTM_TR_End ();
  }

/*****************************************************************************/
/******* Build string with list of selected exams separated by commas ********/
/******* from list of selected exams                                  ********/
/*****************************************************************************/

static void ExaRes_BuildExamsSelectedCommas (struct Exa_Exams *Exams,
                                             char **ExamsSelectedCommas)
  {
   size_t MaxLength;
   unsigned NumExam;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Allocate memory for subquery of exams selected *****/
   MaxLength = (size_t) Exams->NumSelected * (Cns_MAX_DECIMAL_DIGITS_LONG + 1);
   if ((*ExamsSelectedCommas = malloc (MaxLength + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Build subquery with list of selected exams *****/
   (*ExamsSelectedCommas)[0] = '\0';
   for (NumExam = 0;
	NumExam < Exams->Num;
	NumExam++)
      if (Exams->Lst[NumExam].Selected)
	{
	 sprintf (LongStr,"%ld",Exams->Lst[NumExam].ExaCod);
	 if ((*ExamsSelectedCommas)[0])
	    Str_Concat (*ExamsSelectedCommas,",",MaxLength);
	 Str_Concat (*ExamsSelectedCommas,LongStr,MaxLength);
	}
  }

/*****************************************************************************/
/********* Show the sessions results of a user in the current course *********/
/*****************************************************************************/

static void ExaRes_ShowResults (struct Exa_Exams *Exams,
                                Usr_MeOrOther_t MeOrOther,
				long SesCod,	// <= 0 ==> any
				long ExaCod,	// <= 0 ==> any
				const char *ExamsSelectedCommas)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *Txt_Result;
   MYSQL_RES *mysql_res;
   struct UsrData *UsrDat;
   unsigned NumResults;
   unsigned NumResult;
   struct ExaRes_ICanView ICanView;
   static unsigned UniqueId = 0;
   char *Id;
   struct ExaPrn_Print Print;
   struct ExaSes_Session Session;
   struct Exa_Exam Exam;
   Dat_StartEndTime_t StartEndTime;
   unsigned NumQstsInvalid;
   struct ExaPrn_NumQuestions NumTotalQsts;
   struct ExaPrn_Score TotalScore;
   double Grade;
   double TotalGrade = 0.0;

   /***** Reset total number of questions and total score *****/
   NumTotalQsts.All                  =
   NumTotalQsts.NotBlank             =
   NumTotalQsts.Valid.Correct        =
   NumTotalQsts.Valid.Wrong.Negative =
   NumTotalQsts.Valid.Wrong.Zero     =
   NumTotalQsts.Valid.Wrong.Positive =
   NumTotalQsts.Valid.Blank          =
   NumTotalQsts.Valid.Total          = 0;
   TotalScore.All   =
   TotalScore.Valid = 0.0;

   /***** Make database query *****/
   // Do not filter by groups, because a student who has changed groups
   // must be able to access exams taken in other groups
   NumResults = Exa_DB_GetResults (&mysql_res,MeOrOther,SesCod,ExaCod,ExamsSelectedCommas);

   /***** Set user *****/
   UsrDat = (MeOrOther == Usr_ME) ? &Gbl.Usrs.Me.UsrDat :
				    &Gbl.Usrs.Other.UsrDat;

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);
      Usr_ShowTableCellWithUsrData (UsrDat,NumResults);

      /***** Get and print sessions results *****/
      if (NumResults)
	{
	 for (NumResult = 0;
	      NumResult < NumResults;
	      NumResult++)
	   {
	    /* Get print code (row[0]) */
	    if ((Print.PrnCod = DB_GetNextCode (mysql_res)) <= 0)
	       Err_WrongExamExit ();

	    /* Get print data */
	    ExaPrn_GetDataOfPrintByPrnCod (&Print);

	    /* Get data of session and exam */
	    Session.SesCod = Print.SesCod;
	    ExaSes_GetDataOfSessionByCod (&Session);
	    Exam.ExaCod = Session.ExaCod;
	    Exa_GetDataOfExamByCod (&Exam);

	    /* Check if I can view this print result and its score */
	    ExaRes_CheckIfICanViewResult (&Exam,&Session,UsrDat->UsrCod,&ICanView);

	    if (NumResult)
	       HTM_TR_Begin (NULL);

	    /* Write start/end times */
	    for (StartEndTime  = (Dat_StartEndTime_t) 0;
		 StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
		 StartEndTime++)
	      {
	       UniqueId++;
	       if (asprintf (&Id,"exa_res_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_TD_Begin ("id =\"%s\" class=\"%s LT %s\"",
			     Id,The_ClassDat[Gbl.Prefs.Theme],
			     Gbl.ColorRows[Gbl.RowEvenOdd]);
		  Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
						true,true,false,0x7);
	       HTM_TD_End ();
	       free (Id);
	      }

	    /* Write session title */
	    HTM_TD_Begin ("class=\"%s LT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       HTM_Txt (Session.Title);
	    HTM_TD_End ();

	    /* Get and accumulate questions and score */
	    if (ICanView.Score)
	      {
	       /* Get questions and user's answers of exam print from database */
	       ExaPrn_GetPrintQuestionsFromDB (&Print);
	       NumTotalQsts.All += Print.NumQsts.All;

	       /* Compute score taking into account only valid questions */
	       ExaRes_ComputeValidPrintScore (&Print);
	       NumTotalQsts.Valid.Correct        += Print.NumQsts.Valid.Correct;
	       NumTotalQsts.Valid.Wrong.Negative += Print.NumQsts.Valid.Wrong.Negative;
	       NumTotalQsts.Valid.Wrong.Zero     += Print.NumQsts.Valid.Wrong.Zero;
	       NumTotalQsts.Valid.Wrong.Positive += Print.NumQsts.Valid.Wrong.Positive;
	       NumTotalQsts.Valid.Blank          += Print.NumQsts.Valid.Blank;
	       NumTotalQsts.Valid.Total          += Print.NumQsts.Valid.Total;
	       TotalScore.Valid                  += Print.Score.Valid;
	      }

	    /* Write total number of questions */
	    HTM_TD_Begin ("class=\"%s RT LINE_LEFT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		  HTM_Unsigned (Print.NumQsts.All);
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Valid questions */
	    HTM_TD_Begin ("class=\"DAT_GREEN RT %s\"",
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  if (Print.NumQsts.Valid.Total)
		     HTM_Unsigned (Print.NumQsts.Valid.Total);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Invalid questions */
	    HTM_TD_Begin ("class=\"DAT_RED RT %s\"",
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  NumQstsInvalid = Print.NumQsts.All - Print.NumQsts.Valid.Total;
		  if (NumQstsInvalid)
		     HTM_Unsigned (NumQstsInvalid);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write number of correct questions */
	    HTM_TD_Begin ("class=\"%s RT LINE_LEFT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  if (Print.NumQsts.Valid.Correct)
		     HTM_Unsigned (Print.NumQsts.Valid.Correct);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write number of wrong questions */
	    HTM_TD_Begin ("class=\"%s RT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  if (Print.NumQsts.Valid.Wrong.Negative)
		     HTM_Unsigned (Print.NumQsts.Valid.Wrong.Negative);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"%s RT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  if (Print.NumQsts.Valid.Wrong.Zero)
		     HTM_Unsigned (Print.NumQsts.Valid.Wrong.Zero);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"%s RT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  if (Print.NumQsts.Valid.Wrong.Positive)
		     HTM_Unsigned (Print.NumQsts.Valid.Wrong.Positive);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write number of blank questions */
	    HTM_TD_Begin ("class=\"%s RT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  if (Print.NumQsts.Valid.Blank)
		     HTM_Unsigned (Print.NumQsts.Valid.Blank);
		  else
		     HTM_Light0 ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write score valid (taking into account only valid questions) */
	    HTM_TD_Begin ("class=\"%s RT LINE_LEFT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  HTM_Double2Decimals (Print.Score.Valid);
		  HTM_Txt ("/");
		  HTM_Unsigned (Print.NumQsts.Valid.Total);
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write average score per question (taking into account only valid questions) */
	    HTM_TD_Begin ("class=\"%s RT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		  HTM_Double2Decimals (Print.NumQsts.Valid.Total ? Print.Score.Valid /
								   (double) Print.NumQsts.Valid.Total :
								   0.0);
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Write grade over maximum grade (taking into account only valid questions) */
	    HTM_TD_Begin ("class=\"%s RT LINE_LEFT %s\"",
	                  The_ClassDat[Gbl.Prefs.Theme],
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Score)
		 {
		  Grade = TstPrn_ComputeGrade (Print.NumQsts.Valid.Total,Print.Score.Valid,Exam.MaxGrade);
		  TstPrn_ShowGrade (Grade,Exam.MaxGrade);
		  TotalGrade += Grade;
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    /* Link to show this result */
	    HTM_TD_Begin ("class=\"RT LINE_LEFT %s\"",
	                  Gbl.ColorRows[Gbl.RowEvenOdd]);
	       if (ICanView.Result)
		 {
		  Exams->ExaCod = Session.ExaCod;
		  Exams->SesCod = Session.SesCod;
		  switch (MeOrOther)
		    {
		     case Usr_ME:
			Frm_BeginForm (ActSeeOneExaResMe);
			   ExaSes_PutParamsEdit (Exams);
			break;
		     case Usr_OTHER:
			Frm_BeginForm (ActSeeOneExaResOth);
			   ExaSes_PutParamsEdit (Exams);
			   Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
			break;
		    }
		     Ico_PutIconLink ("tasks.svg",Ico_BLACK,Txt_Result);
		  Frm_EndForm ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	    HTM_TR_End ();
	   }

	 /***** Write totals for this user *****/
	 HTM_TR_Begin (NULL);
	 ExaRes_ShowResultsSummaryRow (NumResults,&NumTotalQsts,&TotalScore,TotalGrade);
	}
      else
	{
	 /* Columns for dates and title */
	 HTM_TD_Begin ("colspan=\"3\" class=\"LINE_BOTTOM %s\"",
		       Gbl.ColorRows[Gbl.RowEvenOdd]);
	 HTM_TD_End ();

	 /* Columns for questions */
	 HTM_TD_Begin ("colspan=\"3\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
		       Gbl.ColorRows[Gbl.RowEvenOdd]);
	 HTM_TD_End ();

	 /* Columns for answers */
	 HTM_TD_Begin ("colspan=\"5\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
		       Gbl.ColorRows[Gbl.RowEvenOdd]);
	 HTM_TD_End ();

	 /* Columns for score */
	 HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
		       Gbl.ColorRows[Gbl.RowEvenOdd]);
	 HTM_TD_End ();

	 /* Column for grade */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
		       Gbl.ColorRows[Gbl.RowEvenOdd]);
	 HTM_TD_End ();

	 /* Column for link to show the result */
	 HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
		       Gbl.ColorRows[Gbl.RowEvenOdd]);
	 HTM_TD_End ();
	}

   /***** End last row *****/
   HTM_TR_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/**************** Show row with summary of user's exam results ***************/
/*****************************************************************************/

static void ExaRes_ShowResultsSummaryRow (unsigned NumResults,
                                          const struct ExaPrn_NumQuestions *NumTotalQsts,
                                          const struct ExaPrn_Score *TotalScore,
					  double TotalGrade)
  {
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *Txt_Sessions;
   unsigned NumTotalQstsInvalid;

   /***** Row title *****/
   HTM_TD_Begin ("colspan=\"3\" class=\"RM %s LINE_TOP LINE_BOTTOM %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      HTM_TxtColonNBSP (Txt_Sessions);
      HTM_Unsigned (NumResults);
   HTM_TD_End ();

   /***** Write total number of questions *****/
   HTM_TD_Begin ("class=\"RM %s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      HTM_Unsigned (NumTotalQsts->All);
   HTM_TD_End ();

   /***** Write total number of valid questions *****/
   HTM_TD_Begin ("class=\"RT DAT_GREEN LINE_TOP LINE_BOTTOM %s\"",
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      if (NumTotalQsts->Valid.Total)
	 HTM_Unsigned (NumTotalQsts->Valid.Total);
      else
	 HTM_Light0 ();
   HTM_TD_End ();

   /***** Write total number of invalid questions *****/
   HTM_TD_Begin ("class=\"DAT_RED RT LINE_TOP LINE_BOTTOM %s\"",
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      NumTotalQstsInvalid = NumTotalQsts->All - NumTotalQsts->Valid.Total;
      if (NumTotalQstsInvalid)
	 HTM_Unsigned (NumTotalQstsInvalid);
      else
	 HTM_Light0 ();
   HTM_TD_End ();

   /***** Write number of correct questions *****/
   HTM_TD_Begin ("class=\"RT %s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      if (NumTotalQsts->Valid.Correct)
	 HTM_Unsigned (NumTotalQsts->Valid.Correct);
      else
	 HTM_Light0 ();
   HTM_TD_End ();

   /***** Write number of wrong questions *****/
   HTM_TD_Begin ("class=\"RT %s LINE_TOP LINE_BOTTOM %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      if (NumTotalQsts->Valid.Wrong.Negative)
	 HTM_Unsigned (NumTotalQsts->Valid.Wrong.Negative);
      else
	 HTM_Light0 ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"RT %s LINE_TOP LINE_BOTTOM %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      if (NumTotalQsts->Valid.Wrong.Zero)
	 HTM_Unsigned (NumTotalQsts->Valid.Wrong.Zero);
      else
	 HTM_Light0 ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"RT %s LINE_TOP LINE_BOTTOM %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      if (NumTotalQsts->Valid.Wrong.Positive)
	 HTM_Unsigned (NumTotalQsts->Valid.Wrong.Positive);
      else
	 HTM_Light0 ();
   HTM_TD_End ();

   /***** Write number of blank questions *****/
   HTM_TD_Begin ("class=\"RT %s LINE_TOP LINE_BOTTOM %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      if (NumTotalQsts->Valid.Blank)
	 HTM_Unsigned (NumTotalQsts->Valid.Blank);
      else
	 HTM_Light0 ();
   HTM_TD_End ();

   /***** Write total valid score *****/
   HTM_TD_Begin ("class=\"RM %s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      HTM_Double2Decimals (TotalScore->Valid);
      HTM_Txt ("/");
      HTM_Unsigned (NumTotalQsts->Valid.Total);
   HTM_TD_End ();

   /***** Write average valid score per valid question *****/
   HTM_TD_Begin ("class=\"RM %s LINE_TOP LINE_BOTTOM %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      HTM_Double2Decimals (NumTotalQsts->Valid.Total ? TotalScore->Valid /
						       (double) NumTotalQsts->Valid.Total :
						       0.0);
   HTM_TD_End ();


   /***** Write total grade *****/
   HTM_TD_Begin ("class=\"RM %s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
      HTM_Double2Decimals (TotalGrade);
   HTM_TD_End ();

   /***** Last cell *****/
   HTM_TD_Begin ("class=\"%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                 The_ClassDatStrong[Gbl.Prefs.Theme],
                 Gbl.ColorRows[Gbl.RowEvenOdd]);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************** Show one exam result after finish answering it ***************/
/*****************************************************************************/

void ExaRes_ShowExaResultAfterFinish (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;
   struct ExaPrn_Print Print;
   struct ExaRes_ICanView ICanView =
     {
      .Result = true,	// I have just finish answering, so show result...
      .Score  = false,	// ...but not score
     };
   unsigned Visibility = 1 << TstVis_VISIBLE_QST_ANS_TXT;	// Show only questions and answers text

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Get exam print data *****/
   Print.SesCod = Session.SesCod;
   Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   ExaPrn_GetDataOfPrintBySesCodAndUsrCod (&Print);

   /***** Set log action and print code *****/
   // The user has clicked on the "I have finished" button in an exam print
   ExaLog_SetAction (ExaLog_FINISH_EXAM);
   ExaLog_SetPrnCod (Print.PrnCod);
   ExaLog_SetIfCanAnswer (ExaSes_CheckIfICanAnswerThisSession (&Exam,&Session));

   /***** Get questions and user's answers of exam print from database *****/
   ExaPrn_GetPrintQuestionsFromDB (&Print);

   /***** Show exam result *****/
   ExaRes_ShowExamResult (&Exam,&Session,&Print,
                          &Gbl.Usrs.Me.UsrDat,&ICanView,Visibility);
  }

/*****************************************************************************/
/*************************** Show one exam result ****************************/
/*****************************************************************************/

void ExaRes_ShowOneExaResult (void)
  {
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaSes_Session Session;
   Usr_MeOrOther_t MeOrOther;
   struct UsrData *UsrDat;
   struct ExaPrn_Print Print;
   struct ExaRes_ICanView ICanView;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Pointer to user's data *****/
   MeOrOther = (Gbl.Action.Act == ActSeeOneExaResMe) ? Usr_ME :
	                                               Usr_OTHER;
   switch (MeOrOther)
     {
      case Usr_ME:
	 UsrDat = &Gbl.Usrs.Me.UsrDat;
	 break;
      case Usr_OTHER:
      default:
	 UsrDat = &Gbl.Usrs.Other.UsrDat;
         Usr_GetParamOtherUsrCodEncrypted (UsrDat);
	 break;
     }

   /***** Get exam print data *****/
   Print.SesCod = Session.SesCod;
   Print.UsrCod = UsrDat->UsrCod;
   ExaPrn_GetDataOfPrintBySesCodAndUsrCod (&Print);

   /***** Get questions and user's answers of exam print from database *****/
   ExaPrn_GetPrintQuestionsFromDB (&Print);

   /***** Check if I can view this print result and its score *****/
   ExaRes_CheckIfICanViewResult (&Exam,&Session,UsrDat->UsrCod,&ICanView);

   /***** Show exam result *****/
   ExaRes_ShowExamResult (&Exam,&Session,&Print,
                          UsrDat,&ICanView,Exam.Visibility);

   /***** Show exam log *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ExaLog_ShowExamLog (&Print);
	 break;
      default:	// Other users can not see log
	 return;
     }
  }

/*****************************************************************************/
/***************************** Show exam result ******************************/
/*****************************************************************************/

static void ExaRes_ShowExamResult (const struct Exa_Exam *Exam,
	                           const struct ExaSes_Session *Session,
                                   struct ExaPrn_Print *Print,
                                   struct UsrData *UsrDat,
                                   struct ExaRes_ICanView *ICanView,
                                   unsigned Visibility)
  {
   extern const char *Hlp_ASSESSMENT_Exams_results;

   /***** Compute score taking into account only valid questions *****/
   ExaRes_ComputeValidPrintScore (Print);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Session->Title,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams_results,Box_NOT_CLOSABLE);

      /***** Header *****/
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Check user data *****/
      /* Get data of the user who answered the exam print */
      if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,
						    Usr_DONT_GET_PREFS,
						    Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
	 Err_WrongUserExit ();
      if (!Usr_CheckIfICanViewTstExaMchResult (UsrDat))
	 Err_NoPermissionExit ();

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

	 /* User */
	 ExaRes_ShowExamResultUser (UsrDat);

	 /* Start/end time (for user in this exam print) */
	 ExaRes_ShowExamResultTime (Print);

	 /* Number of questions */
	 ExaRes_ShowExamResultNumQsts (Print,ICanView);

	 /* Number of answers */
	 ExaRes_ShowExamResultNumAnss (Print,ICanView);

	 /* Score */
	 ExaRes_ShowExamResultScore (Print,ICanView);

	 /* Grade */
	 ExaRes_ShowExamResultGrade (Exam,Print,ICanView);

	 /* Answers and solutions */
	 if (ICanView->Result)
	    ExaRes_ShowExamAnswers (UsrDat,Print,Visibility);

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* Get if I can view print result ************************/
/*****************************************************************************/

static void ExaRes_CheckIfICanViewResult (const struct Exa_Exam *Exam,
                                          const struct ExaSes_Session *Session,
                                          long UsrCod,
                                          struct ExaRes_ICanView *ICanView)
  {
   /***** Check if I can view print result and score *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 // Depends on visibility of exam, session and result (eye icons)
	 ICanView->Result = (Usr_ItsMe (UsrCod) &&	// The result is mine
			     !Exam->Hidden &&		// The exam is visible
			     !Session->Hidden &&	// The session is visible
			     Session->ShowUsrResults);	// The results of the session are visible to users
	 // Whether I belong or not to groups of session is not checked here...
	 // ...because I should be able to see old exams made in old groups to which I belonged

	 if (ICanView->Result)
	    // Depends on 5 visibility icons associated to exam
	    ICanView->Score = TstVis_IsVisibleTotalScore (Exam->Visibility);
	 else
	    ICanView->Score = false;
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ICanView->Result =
	 ICanView->Score  = true;
	 break;
      default:
	 ICanView->Result =
	 ICanView->Score  = false;
	 break;
     }
  }

/*****************************************************************************/
/****** Compute total score of exam print counting only valid questions ******/
/*****************************************************************************/

static void ExaRes_ComputeValidPrintScore (struct ExaPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned QstInd;
   struct Qst_Question Question;
   bool QuestionExists;

   /***** Initialize score valid *****/
   Print->NumQsts.Valid.Correct        =
   Print->NumQsts.Valid.Wrong.Negative =
   Print->NumQsts.Valid.Wrong.Zero     =
   Print->NumQsts.Valid.Wrong.Positive =
   Print->NumQsts.Valid.Blank          =
   Print->NumQsts.Valid.Total          = 0;
   Print->Score.Valid = 0.0;

   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      /***** Copy question code *****/
      Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;

      /***** Get validity and answer type from database *****/
      if ((QuestionExists = (Exa_DB_GetValidityAndAnswerType (&mysql_res,Question.QstCod) != 0)))
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get whether the question is invalid (row[0]) */
	 Question.Validity = (row[0][0] == 'Y') ? Qst_INVALID_QUESTION :
						  Qst_VALID_QUESTION;

	 /* Get the type of answer (row[1]) */
	 Question.Answer.Type = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Compute answer score *****/
      if (QuestionExists)
	 if (Question.Validity == Qst_VALID_QUESTION)
	   {
	    ExaPrn_ComputeAnswerScore (&Print->PrintedQuestions[QstInd],&Question);
	    switch (Print->PrintedQuestions[QstInd].AnswerIsCorrect)
	      {
	       case TstPrn_ANSWER_IS_CORRECT:
	          Print->NumQsts.Valid.Correct++;
		  break;
	       case TstPrn_ANSWER_IS_WRONG_NEGATIVE:
	          Print->NumQsts.Valid.Wrong.Negative++;
		  break;
	       case TstPrn_ANSWER_IS_WRONG_ZERO:
	          Print->NumQsts.Valid.Wrong.Zero++;
		  break;
	       case TstPrn_ANSWER_IS_WRONG_POSITIVE:
	          Print->NumQsts.Valid.Wrong.Positive++;
		  break;
	       case TstPrn_ANSWER_IS_BLANK:
	          Print->NumQsts.Valid.Blank++;
		  break;
	      }
	    Print->NumQsts.Valid.Total++;
	    Print->Score.Valid += Print->PrintedQuestions[QstInd].Score;
	   }
     }
  }

/*****************************************************************************/
/************************ Show user row in exam result ***********************/
/*****************************************************************************/

void ExaRes_ShowExamResultUser (struct UsrData *UsrDat)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static const char *ClassPhoto[Pho_NUM_SHAPES] =
     {
      [Pho_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [Pho_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [Pho_SHAPE_OVAL     ] = "PHOTOO45x60",
      [Pho_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };

   /***** Row begin *****/
   HTM_TR_Begin (NULL);

      /***** Label *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_ClassDatStrong[Gbl.Prefs.Theme]);
	 HTM_TxtColon (Txt_ROLES_SINGUL_Abc[UsrDat->Roles.InCurrentCrs][UsrDat->Sex]);
      HTM_TD_End ();

      /***** User's data *****/
      HTM_TD_Begin ("class=\"%s LB\"",The_ClassDat[Gbl.Prefs.Theme]);
	 ID_WriteUsrIDs (UsrDat,NULL);
	 HTM_TxtF ("&nbsp;%s",UsrDat->Surname1);
	 if (UsrDat->Surname2[0])
	    HTM_TxtF ("&nbsp;%s",UsrDat->Surname2);
	 if (UsrDat->FrstName[0])
	    HTM_TxtF (", %s",UsrDat->FrstName);
	 HTM_BR ();
	 Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                            ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM,
	                            false);
      HTM_TD_End ();

   /***** Row end *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********************* Show start/end time in exam print *********************/
/*****************************************************************************/

static void ExaRes_ShowExamResultTime (struct ExaPrn_Print *Print)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   Dat_StartEndTime_t StartEndTime;
   char *Id;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      /***** Row begin *****/
      HTM_TR_Begin (NULL);

	 /***** Label *****/
	 HTM_TD_Begin ("class=\"RT %s\"",The_ClassDatStrong[Gbl.Prefs.Theme]);
	    HTM_TxtColon (Txt_START_END_TIME[StartEndTime]);
	 HTM_TD_End ();

	 /***** Time *****/
	 if (asprintf (&Id,"match_%u",(unsigned) StartEndTime) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LB\"",
	               Id,The_ClassDat[Gbl.Prefs.Theme]);
	    Dat_WriteLocalDateHMSFromUTC (Id,Print->TimeUTC[StartEndTime],
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
					  true,true,true,0x7);
	 HTM_TD_End ();
	 free (Id);

      /***** Row end *****/
      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/******************* Show number of questions in exam print ******************/
/*****************************************************************************/

static void ExaRes_ShowExamResultNumQsts (struct ExaPrn_Print *Print,
                                          const struct ExaRes_ICanView *ICanView)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *Txt_Questions;
   extern const char *Txt_QUESTIONS_valid;
   extern const char *Txt_QUESTIONS_invalid;

   /***** Row begin *****/
   HTM_TR_Begin (NULL);

      /***** Label *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_ClassDatStrong[Gbl.Prefs.Theme]);
	 HTM_TxtColon (Txt_Questions);
      HTM_TD_End ();

      /***** Number of questions *****/
      HTM_TD_Begin ("class=\"LB %s\"",The_ClassDat[Gbl.Prefs.Theme]);
	 if (ICanView->Result)
	   {
	    HTM_TxtF ("%u",Print->NumQsts.All);
	    if (Print->NumQsts.All != Print->NumQsts.Valid.Total)
	      {
	       HTM_Txt (" (");

	       /* Valid questions */
	       HTM_SPAN_Begin ("class=\"DAT_GREEN\"");
		  HTM_TxtColonNBSP (Txt_QUESTIONS_valid);
		  HTM_Unsigned (Print->NumQsts.Valid.Total);
	       HTM_SPAN_End ();

	       HTM_TxtF ("; ");

	       /* Invalid questions */
	       HTM_SPAN_Begin ("class=\"DAT_RED\"");
		  HTM_TxtColonNBSP (Txt_QUESTIONS_invalid);
		  HTM_Unsigned (Print->NumQsts.All - Print->NumQsts.Valid.Total);
	       HTM_SPAN_End ();

	       HTM_Txt (")");
	      }
	   }
	 else
	    Ico_PutIconNotVisible ();
      HTM_TD_End ();

   /***** Row end *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Show number of answers in exam print *******************/
/*****************************************************************************/

static void ExaRes_ShowExamResultNumAnss (struct ExaPrn_Print *Print,
                                          const struct ExaRes_ICanView *ICanView)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *Txt_Valid_answers;
   extern const char *Txt_ANSWERS_correct;
   extern const char *Txt_ANSWERS_wrong;
   extern const char *Txt_ANSWERS_blank;

   /***** Row begin *****/
   HTM_TR_Begin (NULL);

      /***** Label *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_ClassDatStrong[Gbl.Prefs.Theme]);
	 HTM_TxtColon (Txt_Valid_answers);
      HTM_TD_End ();

      /***** Number of answers *****/
      HTM_TD_Begin ("class=\"LB %s\"",The_ClassDat[Gbl.Prefs.Theme]);
	 if (ICanView->Score)
	    HTM_TxtF ("%s(<em>p<sub>i</sub></em>=1):&nbsp;%u; "
		      "%s(-1&le;<em>p<sub>i</sub></em>&lt;0):&nbsp;%u; "
		      "%s(<em>p<sub>i</sub></em>=0):&nbsp;%u; "
		      "%s(0&lt;<em>p<sub>i</sub></em>&lt;1):&nbsp;%u; "
		      "%s(<em>p<sub>i</sub></em>=0):&nbsp;%u",
		      Txt_ANSWERS_correct,Print->NumQsts.Valid.Correct,
		      Txt_ANSWERS_wrong  ,Print->NumQsts.Valid.Wrong.Negative,
		      Txt_ANSWERS_wrong  ,Print->NumQsts.Valid.Wrong.Zero,
		      Txt_ANSWERS_wrong  ,Print->NumQsts.Valid.Wrong.Positive,
		      Txt_ANSWERS_blank  ,Print->NumQsts.Valid.Blank);
	 else
	    Ico_PutIconNotVisible ();
      HTM_TD_End ();

   /***** Row end *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Show score in exam print *************************/
/*****************************************************************************/

static void ExaRes_ShowExamResultScore (struct ExaPrn_Print *Print,
                                        const struct ExaRes_ICanView *ICanView)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *Txt_Score;
   extern const char *Txt_valid_score;

   /***** Row begin *****/
   HTM_TR_Begin (NULL);

      /***** Label *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_ClassDatStrong[Gbl.Prefs.Theme]);
	 HTM_TxtColon (Txt_Score);
      HTM_TD_End ();

      /***** Score *****/
      HTM_TD_Begin ("class=\"LB %s\"",The_ClassDat[Gbl.Prefs.Theme]);
	 if (ICanView->Score)
	   {
	    /* Score counting all questions */
	    if (Print->NumQsts.All == Print->NumQsts.Valid.Total)
	       HTM_STRONG_Begin ();
	    HTM_Double2Decimals (Print->Score.All);
	    HTM_Txt ("/");
	    HTM_Unsigned (Print->NumQsts.All);
	    if (Print->NumQsts.All == Print->NumQsts.Valid.Total)
	       HTM_STRONG_End ();

	    /* Scoure counting only valid questions */
	    if (Print->NumQsts.All != Print->NumQsts.Valid.Total)
	      {
	       HTM_Txt ("; ");
	       HTM_TxtColonNBSP (Txt_valid_score);
	       HTM_STRONG_Begin ();
		  HTM_Double2Decimals (Print->Score.Valid);
		  HTM_Txt ("/");
		  HTM_Unsigned (Print->NumQsts.Valid.Total);
	       HTM_STRONG_End ();
	      }
	   }
	 else
	    Ico_PutIconNotVisible ();
      HTM_TD_End ();

   /***** Row end *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Show grade in exam print *************************/
/*****************************************************************************/

static void ExaRes_ShowExamResultGrade (const struct Exa_Exam *Exam,
	                                struct ExaPrn_Print *Print,
                                        const struct ExaRes_ICanView *ICanView)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassDatStrong[The_NUM_THEMES];
   extern const char *Txt_Grade;
   extern const char *Txt_valid_grade;

   /***** Row begin *****/
   HTM_TR_Begin (NULL);

      /***** Label *****/
      HTM_TD_Begin ("class=\"RT %s\"",The_ClassDatStrong[Gbl.Prefs.Theme]);
	 HTM_TxtColon (Txt_Grade);
      HTM_TD_End ();

      /***** Grade *****/
      HTM_TD_Begin ("class=\"%s LB\"",The_ClassDat[Gbl.Prefs.Theme]);
	 if (ICanView->Score)
	   {
	    /* Grade counting all questions */
	    if (Print->NumQsts.All == Print->NumQsts.Valid.Total)
	       HTM_STRONG_Begin ();
	    TstPrn_ComputeAndShowGrade (Print->NumQsts.All,Print->Score.All,Exam->MaxGrade);
	    if (Print->NumQsts.All == Print->NumQsts.Valid.Total)
	       HTM_STRONG_End ();

	    /* Grade counting only valid questions */
	    if (Print->NumQsts.All != Print->NumQsts.Valid.Total)
	      {
	       HTM_Txt ("; ");
	       HTM_TxtColonNBSP (Txt_valid_grade);
	       HTM_STRONG_Begin ();
		  TstPrn_ComputeAndShowGrade (Print->NumQsts.Valid.Total,Print->Score.Valid,Exam->MaxGrade);
	       HTM_STRONG_End ();
	      }
	   }
	 else
	    Ico_PutIconNotVisible ();
      HTM_TD_End ();

   /***** Row end *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Show user's and correct answers of a test ******************/
/*****************************************************************************/

static void ExaRes_ShowExamAnswers (struct UsrData *UsrDat,
			            struct ExaPrn_Print *Print,
			            unsigned Visibility)
  {
   unsigned QstInd;
   struct Qst_Question Question;

   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      Gbl.RowEvenOdd = QstInd % 2;

      /***** Create test question *****/
      Qst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[QstInd].QstCod;

      /***** Get question data *****/
      ExaSet_GetQstDataFromDB (&Question);

      /***** Write questions and answers *****/
      ExaRes_WriteQstAndAnsExam (UsrDat,Print,QstInd,&Question,Visibility);

      /***** Destroy test question *****/
      Qst_QstDestructor (&Question);
     }
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void ExaRes_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct ExaPrn_Print *Print,
				       unsigned QstInd,
				       struct Qst_Question *Question,
				       unsigned Visibility)
  {
   extern const char *Txt_Score;
   extern const char *Txt_Invalid_question;
   bool ICanView[TstVis_NUM_ITEMS_VISIBILITY];
   static char *ClassNumQst[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "BIG_INDEX_RED",
      [Qst_VALID_QUESTION  ] = "BIG_INDEX",
     };
   static char *ClassAnswerType[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "DAT_SMALL_RED",
      [Qst_VALID_QUESTION  ] = "DAT_SMALL",
     };
   static char *ClassTxt[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "TEST_TXT_RED",
      [Qst_VALID_QUESTION  ] = "TEST_TXT",
     };
   static char *ClassFeedback[Qst_NUM_VALIDITIES] =
     {
      [Qst_INVALID_QUESTION] = "TEST_TXT_LIGHT_RED",
      [Qst_VALID_QUESTION  ] = "TEST_TXT_LIGHT",
     };

   /***** Check if I can view each part of the question *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] = TstVis_IsVisibleQstAndAnsTxt (Visibility);
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] = TstVis_IsVisibleFeedbackTxt  (Visibility);
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] = TstVis_IsVisibleCorrectAns   (Visibility);
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = TstVis_IsVisibleEachQstScore (Visibility);
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
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = true;
	 break;
      default:
	 ICanView[TstVis_VISIBLE_QST_ANS_TXT   ] =
	 ICanView[TstVis_VISIBLE_FEEDBACK_TXT  ] =
	 ICanView[TstVis_VISIBLE_CORRECT_ANSWER] =
	 ICanView[TstVis_VISIBLE_EACH_QST_SCORE] = false;
	 break;
     }

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Number of question and answer type *****/
      HTM_TD_Begin ("class=\"RT %s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);
	 Qst_WriteNumQst (QstInd + 1,ClassNumQst[Question->Validity]);
	 Qst_WriteAnswerType (Question->Answer.Type,ClassAnswerType[Question->Validity]);
      HTM_TD_End ();

      /***** Stem, media and answers *****/
      HTM_TD_Begin ("class=\"LT %s\"",Gbl.ColorRows[Gbl.RowEvenOdd]);

	 /* Stem */
	 Qst_WriteQstStem (Question->Stem,ClassTxt[Question->Validity],
			   ICanView[TstVis_VISIBLE_QST_ANS_TXT]);

	 /* Media */
	 if (ICanView[TstVis_VISIBLE_QST_ANS_TXT])
	    Med_ShowMedia (&Question->Media,
			   "TEST_MED_SHOW_CONT",
			   "TEST_MED_SHOW");

	 /* Answers */
	 ExaPrn_ComputeAnswerScore (&Print->PrintedQuestions[QstInd],Question);
	 TstPrn_WriteAnswersExam (UsrDat,&Print->PrintedQuestions[QstInd],Question,
				  ICanView,
				  ClassTxt[Question->Validity],
				  ClassFeedback[Question->Validity]);

	 /* Write score retrieved from database */
	 if (ICanView[TstVis_VISIBLE_EACH_QST_SCORE])
	   {
	    HTM_DIV_Begin ("class=\"DAT_SMALL LM\"");
	       HTM_TxtColonNBSP (Txt_Score);
	       HTM_SPAN_Begin ("class=\"%s\"",
			       Print->PrintedQuestions[QstInd].StrAnswers[0] ?
			       (Print->PrintedQuestions[QstInd].Score > 0 ? "ANS_OK" :	// Correct/semicorrect
									    "ANS_BAD") :	// Wrong
									    "ANS_0");	// Blank answer
		  HTM_Double2Decimals (Print->PrintedQuestions[QstInd].Score);
		  if (Question->Validity == Qst_INVALID_QUESTION)
		     HTM_TxtF (" (%s)",Txt_Invalid_question);
	       HTM_SPAN_End ();
	    HTM_DIV_End ();
	   }

	 /* Question feedback */
	 if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
	    Qst_WriteQstFeedback (Question->Feedback,ClassFeedback[Question->Validity]);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }
