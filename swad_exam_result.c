// swad_exam_result.c: exams results

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_exam.h"
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
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct ExaRes_ICanView
  {
   bool Result;
   bool Score;
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

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
                                   struct UsrData *UsrDat);

static void ExaRes_CheckIfICanSeePrintResult (const struct Exa_Exam *Exam,
                                              const struct ExaSes_Session *Session,
                                              long UsrCod,
                                              struct ExaRes_ICanView *ICanView);

static void ExaRes_ComputeValidPrintScore (struct ExaPrn_Print *Print);

static void ExaRes_ShowExamAnswers (struct UsrData *UsrDat,
			            struct ExaPrn_Print *Print,
			            unsigned Visibility);
static void ExaRes_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct ExaPrn_Print *Print,
				       unsigned NumQst,
				       struct Tst_Question *Question,
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
   TstCfg_GetConfigFromDB ();	// Get feedback type
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
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List my sessions results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_exam_X,Exam.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
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
   TstCfg_GetConfigFromDB ();	// Get feedback type
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
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   if ((Session.SesCod = ExaSes_GetParamSesCod ()) <= 0)
      Lay_WrongExamSessionExit ();
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaSes_GetDataOfSessionByCod (&Session);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List my sessions results in session *****/
   ExaRes_ShowResultsBegin (&Exams,Str_BuildStringStr (Txt_Results_of_session_X,Session.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
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
   TstCfg_GetConfigFromDB ();	// Get feedback type
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
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show sessions results *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
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
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List sessions results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_exam_X,Exam.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
   ExaRes_ListAllResultsInExa (&Exams,Exam.ExaCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListAllResultsInExa (struct Exa_Exams *Exams,long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   ExaRes_ShowHeaderResults (Usr_OTHER);

   /***** Get all users who have answered any session question in this exam *****/
   NumUsrs = DB_QuerySELECT (&mysql_res,"can not get users in exam",
			     "SELECT users.UsrCod FROM"
			     " (SELECT DISTINCT exa_prints.UsrCod AS UsrCod"	// row[0]
			     " FROM exa_prints,exa_sessions,exa_exams"
			     " WHERE exa_sessions.ExaCod=%ld"
			     " AND exa_sessions.SesCod=exa_prints.SesCod"
			     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
			     " AND exa_exams.CrsCod=%ld)"			// Extra check
			     " AS users,usr_data"
			     " WHERE users.UsrCod=usr_data.UsrCod"
			     " ORDER BY usr_data.Surname1,"
			               "usr_data.Surname2,"
			               "usr_data.FirstName",
			     ExaCod,
			     Gbl.Hierarchy.Crs.CrsCod);
   if (NumUsrs)
     {
      /***** List sessions results for each user *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get session code (row[0]) */
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	       if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show sessions results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  ExaRes_ShowResults (Exams,Usr_OTHER,-1L,ExaCod,NULL);
		 }
	}
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
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   if ((Session.SesCod = ExaSes_GetParamSesCod ()) <= 0)
      Lay_WrongExamSessionExit ();

   /***** Get exam data and session *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaSes_GetDataOfSessionByCod (&Session);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Session,
	                     false);	// Do not put form to start new session

   /***** List sessions results in session *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_session_X,Session.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
   ExaRes_ListAllResultsInSes (&Exams,Session.SesCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListAllResultsInSes (struct Exa_Exams *Exams,long SesCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   ExaRes_ShowHeaderResults (Usr_OTHER);

   /***** Get all users who have answered any session question in this exam *****/
   NumUsrs = DB_QuerySELECT (&mysql_res,"can not get users in session",
			     "SELECT users.UsrCod FROM"
			     " (SELECT exa_prints.UsrCod AS UsrCod"	// row[0]
			     " FROM exa_prints,exa_sessions,exa_exams"
			     " WHERE exa_prints.SesCod=%ld"
			     " AND exa_prints.SesCod=exa_sessions.SesCod"
			     " AND exa_sessions.ExaCod=exa_exams.ExaCod"
			     " AND exa_exams.CrsCod=%ld)"		// Extra check
			     " AS users,usr_data"
			     " WHERE users.UsrCod=usr_data.UsrCod"
			     " ORDER BY usr_data.Surname1,"
			               "usr_data.Surname2,"
			               "usr_data.FirstName",
			     SesCod,
			     Gbl.Hierarchy.Crs.CrsCod);
   if (NumUsrs)
     {
      /***** List sessions results for each user *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get session code (row[0]) */
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	       if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show sessions results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  ExaRes_ShowResults (Exams,Usr_OTHER,SesCod,-1L,NULL);
		 }
	}
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

   /***** Begin box *****/
   HTM_SECTION_Begin (ExaRes_RESULTS_BOX_ID);
   Box_BoxBegin ("100%",Title,
                 NULL,NULL,
		 Hlp_ASSESSMENT_Exams_results,Box_NOT_CLOSABLE);

   /***** List exams to select *****/
   if (ListExamsToSelect)
      ExaRes_ListExamsToSelect (Exams);

   /***** Begin session results table *****/
   HTM_SECTION_Begin (ExaRes_RESULTS_TABLE_ID);
   HTM_TABLE_BeginWidePadding (5);
  }

static void ExaRes_ShowResultsEnd (void)
  {
   /***** End session results table *****/
   HTM_TABLE_End ();
   HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/********** Write list of those attendance sessions that have students *********/
/*****************************************************************************/

static void ExaRes_ListExamsToSelect (struct Exa_Exams *Exams)
  {
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
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
   Frm_StartFormAnchor (Gbl.Action.Act,ExaRes_RESULTS_TABLE_ID);
   Grp_PutParamsCodGrps ();
   Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Heading row *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,2,NULL,NULL);
   HTM_TH (1,1,"LM",Txt_Exam);

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

      HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_INPUT_CHECKBOX ("ExaCod",HTM_DONT_SUBMIT_ON_CHANGE,
			  "id=\"Gam%u\" value=\"%ld\"%s",
			  NumExam,Exams->Lst[NumExam].ExaCod,
			  Exams->Lst[NumExam].Selected ? " checked=\"checked\"" :
				                         "");
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_LABEL_Begin ("for=\"Gam%u\"",NumExam);
      HTM_TxtF ("%u:",NumExam + 1);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
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

   HTM_TH (3,2,"CT",Txt_User[MeOrOther == Usr_ME ? Gbl.Usrs.Me.UsrDat.Sex :
		                                   Usr_SEX_UNKNOWN]);
   HTM_TH (3,1,"LT",Txt_START_END_TIME[Dat_START_TIME]);
   HTM_TH (3,1,"LT",Txt_START_END_TIME[Dat_END_TIME  ]);
   HTM_TH (3,1,"LT",Txt_Session);
   HTM_TH (1,3,"CT LINE_LEFT",Txt_Questions);
   HTM_TH (1,5,"CT LINE_LEFT",Txt_Valid_answers);
   HTM_TH (1,2,"CT LINE_LEFT",Txt_Score);
   HTM_TH (3,1,"RT LINE_LEFT",Txt_Grade);
   HTM_TH (3,1,"LINE_LEFT",NULL);

   HTM_TR_End ();

   /***** Second row *****/
   HTM_TR_Begin (NULL);

   HTM_TH (2,1,"RT LINE_LEFT",Txt_total);
   HTM_TH (2,1,"RT",Txt_QUESTIONS_valid);
   HTM_TH (2,1,"RT",Txt_QUESTIONS_invalid);
   HTM_TH (1,1,"RT LINE_LEFT",Txt_ANSWERS_correct);
   HTM_TH (1,3,"CT",Txt_ANSWERS_wrong);
   HTM_TH (1,1,"RT",Txt_ANSWERS_blank);
   HTM_TH (1,1,"RT LINE_LEFT",Txt_total);
   HTM_TH (1,1,"RT",Txt_average);

   HTM_TR_End ();

   /***** Third row *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"RT LINE_LEFT","<em>p<sub>i</sub></em>=1");
   HTM_TH (1,1,"RT","-1&le;<em>p<sub>i</sub></em>&lt;0");
   HTM_TH (1,1,"RT","<em>p<sub>i</sub></em>=0");
   HTM_TH (1,1,"RT","0&lt;<em>p<sub>i</sub></em>&lt;1");
   HTM_TH (1,1,"RT","<em>p<sub>i</sub></em>=0");
   HTM_TH (1,1,"RT LINE_LEFT","<em>&Sigma;p<sub>i</sub></em>");
   HTM_TH (1,1,"RT","-1&le;<em style=\"text-decoration:overline;\">p</em>&le;1");

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
   if ((*ExamsSelectedCommas = (char *) malloc (MaxLength + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

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
   extern const char *Txt_Result;
   char *SesSubQuery;
   char *HidSesSubQuery;
   char *HidExaSubQuery;
   char *ExaSubQuery;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
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
   time_t TimeUTC[Dat_NUM_START_END_TIME];

   /***** Reset print, session and exam *****/
   ExaPrn_ResetPrint (&Print);
   ExaSes_ResetSession (&Session);
   Exa_ResetExam (&Exam);

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

   /***** Set user *****/
   UsrDat = (MeOrOther == Usr_ME) ? &Gbl.Usrs.Me.UsrDat :
				    &Gbl.Usrs.Other.UsrDat;

   /***** Build sessions subquery *****/
   if (SesCod > 0)	// One unique session
     {
      if (asprintf (&SesSubQuery," AND exa_prints.SesCod=%ld",SesCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else			// All sessions of selected exams
     {
      if (asprintf (&SesSubQuery,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /***** Subquery: get hidden sessions?
	  · A student will not be able to see their results in hidden sessions
	  · A teacher will be able to see results from other users even in hidden sessions
   *****/
   switch (MeOrOther)
     {
      case Usr_ME:	// A student watching her/his results
         if (asprintf (&HidSesSubQuery," AND exa_sessions.Hidden='N'") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      default:		// A teacher/admin watching the results of other users
	 if (asprintf (&HidSesSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
     }

   /***** Build exams subquery *****/
   if (ExaCod > 0)			// One unique exams
     {
      if (asprintf (&ExaSubQuery," AND exa_sessions.ExaCod=%ld",ExaCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else if (ExamsSelectedCommas)
     {
      if (ExamsSelectedCommas[0])	// Selected exams
	{
	 if (asprintf (&ExaSubQuery," AND exa_sessions.ExaCod IN (%s)",
		       ExamsSelectedCommas) < 0)
	    Lay_NotEnoughMemoryExit ();
	}
      else
	{
	 if (asprintf (&ExaSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	}
     }
   else					// All exams
     {
      if (asprintf (&ExaSubQuery,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /***** Subquery: get hidden exams?
	  · A student will not be able to see their results in hidden exams
	  · A teacher will be able to see results from other users even in hidden exams
   *****/
   switch (MeOrOther)
     {
      case Usr_ME:	// A student watching her/his results
         if (asprintf (&HidExaSubQuery," AND exa_exams.Hidden='N'") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      default:		// A teacher/admin watching the results of other users
	 if (asprintf (&HidExaSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
     }

   /***** Make database query *****/
   // Do not filter by groups, because a student who has changed groups
   // must be able to access exams taken in other groups
   NumResults =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get sessions results",
			      "SELECT exa_prints.PrnCod,"			// row[0]
			             "exa_prints.SesCod,"			// row[1]
				     "UNIX_TIMESTAMP(exa_prints.StartTime),"	// row[2]
				     "UNIX_TIMESTAMP(exa_prints.EndTime)"	// row[3]
			      " FROM exa_prints,exa_sessions,exa_exams"
			      " WHERE exa_prints.UsrCod=%ld"
			      "%s"	// Session subquery
			      " AND exa_prints.SesCod=exa_sessions.SesCod"
                              "%s"	// Hidden sessions subquery
			      "%s"	// Exams subquery
			      " AND exa_sessions.ExaCod=exa_exams.ExaCod"
                              "%s"	// Hidden exams subquery
			      " AND exa_exams.CrsCod=%ld"			// Extra check
			      " ORDER BY exa_sessions.Title",
			      UsrDat->UsrCod,
			      SesSubQuery,
			      HidSesSubQuery,
			      ExaSubQuery,
			      HidExaSubQuery,
			      Gbl.Hierarchy.Crs.CrsCod);
   free (HidExaSubQuery);
   free (ExaSubQuery);
   free (HidSesSubQuery);
   free (SesSubQuery);

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
	 row = mysql_fetch_row (mysql_res);

	 /* Get print code (row[0]) */
	 if ((Print.PrnCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of exam print.");

	 /* Get session code (row[1]) */
	 if ((Session.SesCod = Str_ConvertStrCodToLongCod (row[1])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of session.");

	 /* Get data of session and exam */
	 ExaSes_GetDataOfSessionByCod (&Session);
	 Exam.ExaCod = Session.ExaCod;
	 Exa_GetDataOfExamByCod (&Exam);

	 /* Check if I can view this print result and its score */
	 ExaRes_CheckIfICanSeePrintResult (&Exam,&Session,UsrDat->UsrCod,&ICanView);

	 if (NumResult)
	    HTM_TR_Begin (NULL);

	 /* Write start/end times (row[2], row[3] hold UTC start/end times) */
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[2 + StartEndTime]);
	    UniqueId++;
	    if (asprintf (&Id,"exa_res_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	       Lay_NotEnoughMemoryExit ();
	    HTM_TD_Begin ("id =\"%s\" class=\"DAT LT COLOR%u\"",
			  Id,Gbl.RowEvenOdd);
	    Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC[StartEndTime],
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					  true,true,false,0x7);
	    HTM_TD_End ();
	    free (Id);
	   }

	 /* Write session title */
	 HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Session.Title);
	 HTM_TD_End ();

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
	    TotalScore.Valid += Print.Score.Valid;
	   }

	 /* Write total number of questions */
	 HTM_TD_Begin ("class=\"DAT LINE_LEFT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanView.Score)
            HTM_Unsigned (Print.NumQsts.All);
	 else
            Ico_PutIconNotVisible ();
         HTM_TD_End ();

	 /* Valid questions */
	 HTM_TD_Begin ("class=\"DAT_GREEN RT COLOR%u\"",Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"DAT_RED RT COLOR%u\"",Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"DAT LINE_LEFT RT COLOR%u\"",Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
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

	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
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

	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"DAT LINE_LEFT RT COLOR%u\"",Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanView.Score)
	    HTM_Double2Decimals (Print.NumQsts.Valid.Total ? Print.Score.Valid /
					                     (double) Print.NumQsts.Valid.Total :
					                     0.0);
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write grade over maximum grade (taking into account only valid questions) */
	 HTM_TD_Begin ("class=\"DAT LINE_LEFT RT COLOR%u\"",Gbl.RowEvenOdd);
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
	 HTM_TD_Begin ("class=\"LINE_LEFT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanView.Result)
	   {
	    Exams->ExaCod = Session.ExaCod;
	    Exams->SesCod = Session.SesCod;
	    switch (MeOrOther)
	      {
	       case Usr_ME:
		  Frm_StartForm (ActSeeOneExaResMe);
		  ExaSes_PutParamsEdit (Exams);
		  break;
	       case Usr_OTHER:
		  Frm_StartForm (ActSeeOneExaResOth);
		  ExaSes_PutParamsEdit (Exams);
		  Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
		  break;
	      }
	    Ico_PutIconLink ("tasks.svg",Txt_Result);
	    Frm_EndForm ();
	   }
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write totals for this user *****/
      ExaRes_ShowResultsSummaryRow (NumResults,&NumTotalQsts,&TotalScore,TotalGrade);
     }
   else
     {
      /* Columns for dates and title */
      HTM_TD_Begin ("colspan=\"3\" class=\"LINE_BOTTOM COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Columns for questions */
      HTM_TD_Begin ("colspan=\"3\" class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Columns for answers */
      HTM_TD_Begin ("colspan=\"5\" class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Columns for score */
      HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Column for grade */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      /* Column for link to show the result */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();

      HTM_TR_End ();
     }

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
   extern const char *Txt_Sessions;
   unsigned NumTotalQstsInvalid;

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Row title *****/
   HTM_TD_Begin ("colspan=\"3\" class=\"DAT_N LINE_TOP LINE_BOTTOM RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtColonNBSP (Txt_Sessions);
   HTM_Unsigned (NumResults);
   HTM_TD_End ();

   /***** Write total number of questions *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM LINE_LEFT RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Unsigned (NumTotalQsts->All);
   HTM_TD_End ();

   /***** Write total number of valid questions *****/
   HTM_TD_Begin ("class=\"DAT_GREEN LINE_TOP LINE_BOTTOM RT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumTotalQsts->Valid.Total)
      HTM_Unsigned (NumTotalQsts->Valid.Total);
   else
      HTM_Light0 ();
   HTM_TD_End ();

   /***** Write total number of invalid questions *****/
   HTM_TD_Begin ("class=\"DAT_RED LINE_TOP LINE_BOTTOM RT COLOR%u\"",Gbl.RowEvenOdd);
   NumTotalQstsInvalid = NumTotalQsts->All - NumTotalQsts->Valid.Total;
   if (NumTotalQstsInvalid)
      HTM_Unsigned (NumTotalQstsInvalid);
   else
      HTM_Light0 ();
   HTM_TD_End ();

   /***** Write number of correct questions *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM LINE_LEFT RT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumTotalQsts->Valid.Correct)
      HTM_Unsigned (NumTotalQsts->Valid.Correct);
   else
      HTM_Light0 ();
   HTM_TD_End ();

   /***** Write number of wrong questions *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM RT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumTotalQsts->Valid.Wrong.Negative)
      HTM_Unsigned (NumTotalQsts->Valid.Wrong.Negative);
   else
      HTM_Light0 ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM RT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumTotalQsts->Valid.Wrong.Zero)
      HTM_Unsigned (NumTotalQsts->Valid.Wrong.Zero);
   else
      HTM_Light0 ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM RT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumTotalQsts->Valid.Wrong.Positive)
      HTM_Unsigned (NumTotalQsts->Valid.Wrong.Positive);
   else
      HTM_Light0 ();
   HTM_TD_End ();

   /***** Write number of blank questions *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM RT COLOR%u\"",Gbl.RowEvenOdd);
   if (NumTotalQsts->Valid.Blank)
      HTM_Unsigned (NumTotalQsts->Valid.Blank);
   else
      HTM_Light0 ();
   HTM_TD_End ();

   /***** Write total valid score *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM LINE_LEFT RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Double2Decimals (TotalScore->Valid);
   HTM_Txt ("/");
   HTM_Unsigned (NumTotalQsts->Valid.Total);
   HTM_TD_End ();

   /***** Write average valid score per valid question *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Double2Decimals (NumTotalQsts->Valid.Total ? TotalScore->Valid /
	                                            (double) NumTotalQsts->Valid.Total :
			                            0.0);
   HTM_TD_End ();


   /***** Write total grade *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM LINE_LEFT RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Double2Decimals (TotalGrade);
   HTM_TD_End ();

   /***** Last cell *****/
   HTM_TD_Begin ("class=\"DAT_N LINE_TOP LINE_BOTTOM LINE_LEFT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
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

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaSes_ResetSession (&Session);

   /***** Get and check parameters *****/
   ExaSes_GetAndCheckParameters (&Exams,&Exam,&Session);

   /***** Pointer to user's data *****/
   MeOrOther = (Gbl.Action.Act == ActSeeOneExaResMe ||
	        Gbl.Action.Act == ActEndExaPrn) ? Usr_ME :
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
   ExaPrn_ResetPrint (&Print);
   Print.SesCod = Session.SesCod;
   Print.UsrCod = UsrDat->UsrCod;
   ExaPrn_GetDataOfPrintByCodAndUsrCod (&Print);


   /***** Set log action and print code *****/
   if (Gbl.Action.Act == ActEndExaPrn)
     {
      // The user has clicked on the "I have finished" button in an exam print
      ExaLog_SetAction (ExaLog_FINISH_EXAM);
      ExaLog_SetPrnCod (Print.PrnCod);
      ExaLog_SetIfCanAnswer (ExaSes_CheckIfICanAnswerThisSession (&Exam,&Session));
     }

   /***** Get questions and user's answers of exam print from database *****/
   ExaPrn_GetPrintQuestionsFromDB (&Print);

   /***** Show exam result *****/
   ExaRes_ShowExamResult (&Exam,&Session,&Print,UsrDat);

   /***** Show exam log *****/
   ExaLog_ShowExamLog (&Print);
  }

/*****************************************************************************/
/***************************** Show exam result ******************************/
/*****************************************************************************/

static void ExaRes_ShowExamResult (const struct Exa_Exam *Exam,
	                           const struct ExaSes_Session *Session,
                                   struct ExaPrn_Print *Print,
                                   struct UsrData *UsrDat)
  {
   extern const char *Hlp_ASSESSMENT_Exams_results;
   extern const char *Txt_The_user_does_not_exist;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_QUESTIONS_valid;
   extern const char *Txt_QUESTIONS_invalid;
   extern const char *Txt_Valid_answers;
   extern const char *Txt_ANSWERS_correct;
   extern const char *Txt_ANSWERS_wrong;
   extern const char *Txt_ANSWERS_blank;
   extern const char *Txt_Score;
   extern const char *Txt_valid_score;
   extern const char *Txt_Grade;
   extern const char *Txt_valid_grade;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct ExaRes_ICanView ICanView;

   /***** Check if I can view this print result and its score *****/
   ExaRes_CheckIfICanSeePrintResult (Exam,Session,UsrDat->UsrCod,&ICanView);

   /***** Compute score taking into account only valid questions *****/
   if (ICanView.Score)
      ExaRes_ComputeValidPrintScore (Print);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Session->Title,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Exams_results,Box_NOT_CLOSABLE);
   Lay_WriteHeaderClassPhoto (false,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);


   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (10);

   /***** User *****/
   /* Get data of the user who answered the exam print */
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,Usr_DONT_GET_PREFS))
      Lay_ShowErrorAndExit (Txt_The_user_does_not_exist);
   if (!Usr_CheckIfICanViewTstExaMchResult (UsrDat))
      Lay_NoPermissionExit ();

   /* User */
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT_N RT\"");
   HTM_TxtColon (Txt_ROLES_SINGUL_Abc[UsrDat->Roles.InCurrentCrs.Role][UsrDat->Sex]);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LB\"");
   ID_WriteUsrIDs (UsrDat,NULL);
   HTM_TxtF ("&nbsp;%s",UsrDat->Surname1);
   if (UsrDat->Surname2[0])
      HTM_TxtF ("&nbsp;%s",UsrDat->Surname2);
   if (UsrDat->FirstName[0])
      HTM_TxtF (", %s",UsrDat->FirstName);
   HTM_BR ();
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					NULL,
		     "PHOTO45x60",Pho_ZOOM,false);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Start/end time (for user in this exam print) *****/
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtColon (Txt_START_END_TIME[StartEndTime]);
      HTM_TD_End ();

      if (asprintf (&Id,"match_%u",(unsigned) StartEndTime) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"DAT LB\"",Id);
      Dat_WriteLocalDateHMSFromUTC (Id,Print->TimeUTC[StartEndTime],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);

      HTM_TR_End ();
     }

   /***** Number of questions *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT_N RT\"");
   HTM_TxtColon (Txt_Questions);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LB\"");
   if (ICanView.Result)
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

   HTM_TR_End ();

   /***** Number of answers *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT_N RT\"");
   HTM_TxtColon (Txt_Valid_answers);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LB\"");
   if (ICanView.Result)
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

   HTM_TR_End ();

   /***** Score *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT_N RT\"");
   HTM_TxtColon (Txt_Score);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LB\"");
   if (ICanView.Score)
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

   HTM_TR_End ();

   /***** Grade *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT_N RT\"");
   HTM_TxtColon (Txt_Grade);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LB\"");
   if (ICanView.Score)
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

   HTM_TR_End ();

   /***** Write answers and solutions *****/
   if (ICanView.Result)
      ExaRes_ShowExamAnswers (UsrDat,Print,Exam->Visibility);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Get if I can see print result ************************/
/*****************************************************************************/

static void ExaRes_CheckIfICanSeePrintResult (const struct Exa_Exam *Exam,
                                              const struct ExaSes_Session *Session,
                                              long UsrCod,
                                              struct ExaRes_ICanView *ICanView)
  {
   bool ItsMe;

   /***** Check if I can view print result and score *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 // Depends on visibility of exam, session and result (eye icons)
	 ItsMe = Usr_ItsMe (UsrCod);
	 ICanView->Result = (ItsMe &&			// The result is mine
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
   unsigned NumQst;
   struct Tst_Question Question;
   bool QuestionExists;

   /***** Initialize score valid *****/
   Print->NumQsts.Valid.Correct        =
   Print->NumQsts.Valid.Wrong.Negative =
   Print->NumQsts.Valid.Wrong.Zero     =
   Print->NumQsts.Valid.Wrong.Positive =
   Print->NumQsts.Valid.Blank          =
   Print->NumQsts.Valid.Total          = 0;
   Print->Score.Valid = 0.0;

   for (NumQst = 0;
	NumQst < Print->NumQsts.All;
	NumQst++)
     {
      /***** Copy question code *****/
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

      /***** Get validity and answer type from database *****/
      QuestionExists = (DB_QuerySELECT (&mysql_res,"can not get a question",
					"SELECT Invalid,"	// row[0]
					       "AnsType"	// row[1]
					" FROM exa_set_questions"
					" WHERE QstCod=%ld",
					Question.QstCod) != 0);
      if (QuestionExists)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get whether the question is invalid (row[0]) */
	 Question.Validity = (row[0][0] == 'Y') ? Tst_INVALID_QUESTION :
						  Tst_VALID_QUESTION;

	 /* Get the type of answer (row[1]) */
	 Question.Answer.Type = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** Compute answer score *****/
      if (QuestionExists)
	 if (Question.Validity == Tst_VALID_QUESTION)
	   {
	    ExaPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],&Question);
	    switch (Print->PrintedQuestions[NumQst].AnswerIsCorrect)
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
	    Print->Score.Valid += Print->PrintedQuestions[NumQst].Score;
	   }
     }
  }

/*****************************************************************************/
/************** Show user's and correct answers of a test exam ***************/
/*****************************************************************************/

static void ExaRes_ShowExamAnswers (struct UsrData *UsrDat,
			            struct ExaPrn_Print *Print,
			            unsigned Visibility)
  {
   unsigned NumQst;
   struct Tst_Question Question;

   for (NumQst = 0;
	NumQst < Print->NumQsts.All;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;

      /***** Get question data *****/
      ExaSet_GetQstDataFromDB (&Question);

      /***** Write questions and answers *****/
      ExaRes_WriteQstAndAnsExam (UsrDat,Print,NumQst,&Question,Visibility);

      /***** Destroy test question *****/
      Tst_QstDestructor (&Question);
     }
  }

/*****************************************************************************/
/********** Write a row of a test, with one question and its answer **********/
/*****************************************************************************/

static void ExaRes_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct ExaPrn_Print *Print,
				       unsigned NumQst,
				       struct Tst_Question *Question,
				       unsigned Visibility)
  {
   extern const char *Txt_Score;
   extern const char *Txt_Invalid_question;
   bool ICanView[TstVis_NUM_ITEMS_VISIBILITY];
   static char *ClassNumQst[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "BIG_INDEX_RED",
      [Tst_VALID_QUESTION  ] = "BIG_INDEX",
     };
   static char *ClassAnswerType[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "DAT_SMALL_RED",
      [Tst_VALID_QUESTION  ] = "DAT_SMALL",
     };
   static char *ClassTxt[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "TEST_TXT_RED",
      [Tst_VALID_QUESTION  ] = "TEST_TXT",
     };
   static char *ClassFeedback[Tst_NUM_VALIDITIES] =
     {
      [Tst_INVALID_QUESTION] = "TEST_TXT_LIGHT_RED",
      [Tst_VALID_QUESTION  ] = "TEST_TXT_LIGHT",
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
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1,ClassNumQst[Question->Validity]);
   Tst_WriteAnswerType (Question->Answer.Type,ClassAnswerType[Question->Validity]);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Stem */
   Tst_WriteQstStem (Question->Stem,ClassTxt[Question->Validity],
                     ICanView[TstVis_VISIBLE_QST_ANS_TXT]);

   /* Media */
   if (ICanView[TstVis_VISIBLE_QST_ANS_TXT])
      Med_ShowMedia (&Question->Media,
		     "TEST_MED_SHOW_CONT",
		     "TEST_MED_SHOW");

   /* Answers */
   ExaPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],Question);
   TstPrn_WriteAnswersExam (UsrDat,&Print->PrintedQuestions[NumQst],Question,
                            ICanView,
                            ClassTxt[Question->Validity],
                            ClassFeedback[Question->Validity]);

   /* Write score retrieved from database */
   if (ICanView[TstVis_VISIBLE_EACH_QST_SCORE])
     {
      HTM_DIV_Begin ("class=\"DAT_SMALL LM\"");
      HTM_TxtColonNBSP (Txt_Score);
      HTM_SPAN_Begin ("class=\"%s\"",
		      Print->PrintedQuestions[NumQst].StrAnswers[0] ?
		      (Print->PrintedQuestions[NumQst].Score > 0 ? "ANS_OK" :	// Correct/semicorrect
								   "ANS_BAD") :	// Wrong
								   "ANS_0");	// Blank answer
      HTM_Double2Decimals (Print->PrintedQuestions[NumQst].Score);
      if (Question->Validity == Tst_INVALID_QUESTION)
	 HTM_TxtF (" (%s)",Txt_Invalid_question);
      HTM_SPAN_End ();
      HTM_DIV_End ();
     }

   /* Question feedback */
   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT])
      Tst_WriteQstFeedback (Question->Feedback,ClassFeedback[Question->Validity]);

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }
