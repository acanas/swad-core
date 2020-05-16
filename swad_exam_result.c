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
#include "swad_exam_event.h"
#include "swad_exam_print.h"
#include "swad_exam_result.h"
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

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void ExaRes_PutFormToSelUsrsToViewEvtResults (void *Exams);

static void ExaRes_ListMyEvtResultsInCrs (struct Exa_Exams *Exams);
static void ExaRes_ListMyEvtResultsInExa (struct Exa_Exams *Exams,long ExaCod);
static void ExaRes_ListMyEvtResultsInEvt (struct Exa_Exams *Exams,long EvtCod);
static void ExaRes_ShowAllEvtResultsInSelectedExams (void *Exams);
static void ExaRes_ListAllEvtResultsInSelectedExams (struct Exa_Exams *Exams);
static void ExaRes_ListAllEvtResultsInExa (struct Exa_Exams *Exams,long ExaCod);
static void ExaRes_ListAllEvtResultsInEvt (struct Exa_Exams *Exams,long EvtCod);

static void ExaRes_ShowResultsBegin (struct Exa_Exams *Exams,
                                     const char *Title,bool ListExamsToSelect);
static void ExaRes_ShowResultsEnd (void);

static void ExaRes_ListExamsToSelect (struct Exa_Exams *Exams);
static void ExaRes_ShowHeaderEvtResults (Usr_MeOrOther_t MeOrOther);

static void ExaRes_BuildExamsSelectedCommas (struct Exa_Exams *Exams,
                                             char **ExamsSelectedCommas);
static void ExaRes_ShowEvtResults (struct Exa_Exams *Exams,
                                   Usr_MeOrOther_t MeOrOther,
				   long EvtCod,	// <= 0 ==> any
				   long ExaCod,	// <= 0 ==> any
				   const char *ExamsSelectedCommas);
static void ExaRes_ShowEvtResultsSummaryRow (unsigned NumResults,
                                             unsigned NumTotalQsts,
                                             unsigned NumTotalQstsNotBlank,
                                             double TotalScoreOfAllResults,
					     double TotalGrade);

static bool ExaRes_CheckIfICanSeePrintResult (struct ExaEvt_Event *Event,long UsrCod);
static bool ExaRes_CheckIfICanViewScore (bool ICanViewResult,unsigned Visibility);

static void ExaRes_ShowExamAnswers (struct UsrData *UsrDat,
			            struct ExaPrn_Print *Print,
			            unsigned Visibility);
static void ExaRes_WriteQstAndAnsExam (struct UsrData *UsrDat,
				       struct ExaPrn_Print *Print,
				       unsigned NumQst,
				       struct Tst_Question *Question,
				       unsigned Visibility);

/*****************************************************************************/
/*************************** Show my events results **************************/
/*****************************************************************************/

void ExaRes_ShowMyExaResultsInCrs (void)
  {
   extern const char *Txt_Results;
   struct Exa_Exams Exams;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Get list of exams *****/
   Exa_GetListExams (&Exams,Exa_ORDER_BY_TITLE);
   Exa_GetListSelectedExaCods (&Exams);

   /***** List my events results in the current course *****/
   ExaRes_ShowResultsBegin (&Exams,Txt_Results,true);	// List exams to select
   ExaRes_ListMyEvtResultsInCrs (&Exams);
   ExaRes_ShowResultsEnd ();

   /***** Free list of exams *****/
   free (Exams.ExaCodsSelected);
   Exa_FreeListExams (&Exams);
  }

static void ExaRes_ListMyEvtResultsInCrs (struct Exa_Exams *Exams)
  {
   char *ExamsSelectedCommas = NULL;	// Initialized to avoid warning

   /***** Table header *****/
   ExaRes_ShowHeaderEvtResults (Usr_ME);

   /***** List my events results in the current course *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   ExaRes_BuildExamsSelectedCommas (Exams,&ExamsSelectedCommas);
   ExaRes_ShowEvtResults (Exams,Usr_ME,-1L,-1L,ExamsSelectedCommas);
   free (ExamsSelectedCommas);
  }

/*****************************************************************************/
/***************** Show my events results in a given exam *******************/
/*****************************************************************************/

void ExaRes_ShowMyExaResultsInExa (void)
  {
   extern const char *Txt_Results_of_exam_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;

   /***** Get exam data from database *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Event,
	                     false);	// Do not put form to start new event

   /***** List my events results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_exam_X,Exam.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
   ExaRes_ListMyEvtResultsInExa (&Exams,Exam.ExaCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListMyEvtResultsInExa (struct Exa_Exams *Exams,long ExaCod)
  {
   /***** Table header *****/
   ExaRes_ShowHeaderEvtResults (Usr_ME);

   /***** List my events results in exam *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   ExaRes_ShowEvtResults (Exams,Usr_ME,-1L,ExaCod,NULL);
  }

/*****************************************************************************/
/***************** Show my events results in a given event ******************/
/*****************************************************************************/

void ExaRes_ShowMyExaResultsInEvt (void)
  {
   extern const char *Txt_Results_of_event_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   if ((Event.EvtCod = ExaEvt_GetParamEvtCod ()) <= 0)
      Lay_WrongEventExit ();
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaEvt_GetDataOfEventByCod (&Event);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Event,
	                     false);	// Do not put form to start new event

   /***** List my events results in event *****/
   ExaRes_ShowResultsBegin (&Exams,Str_BuildStringStr (Txt_Results_of_event_X,Event.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
   ExaRes_ListMyEvtResultsInEvt (&Exams,Event.EvtCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListMyEvtResultsInEvt (struct Exa_Exams *Exams,long EvtCod)
  {
   /***** Table header *****/
   ExaRes_ShowHeaderEvtResults (Usr_ME);

   /***** List my events results in exam *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   ExaRes_ShowEvtResults (Exams,Usr_ME,EvtCod,-1L,NULL);
  }

/*****************************************************************************/
/****************** Get users and show their events results *****************/
/*****************************************************************************/

void ExaRes_ShowAllExaResultsInCrs (void)
  {
   struct Exa_Exams Exams;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Get users and show their events results *****/
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  ExaRes_ShowAllEvtResultsInSelectedExams,&Exams,
                                  ExaRes_PutFormToSelUsrsToViewEvtResults,&Exams);
  }

/*****************************************************************************/
/****************** Show events results for several users *******************/
/*****************************************************************************/

static void ExaRes_ShowAllEvtResultsInSelectedExams (void *Exams)
  {
   extern const char *Txt_Results;

   if (!Exams)
      return;

   /***** Get list of exams *****/
   Exa_GetListExams ((struct Exa_Exams *) Exams,Exa_ORDER_BY_TITLE);
   Exa_GetListSelectedExaCods ((struct Exa_Exams *) Exams);

   /***** List the events results of the selected users *****/
   ExaRes_ShowResultsBegin ((struct Exa_Exams *) Exams,
                            Txt_Results,
                            true);	// List exams to select
   ExaRes_ListAllEvtResultsInSelectedExams ((struct Exa_Exams *) Exams);
   ExaRes_ShowResultsEnd ();

   /***** Free list of exams *****/
   free (((struct Exa_Exams *) Exams)->ExaCodsSelected);
   Exa_FreeListExams ((struct Exa_Exams *) Exams);
  }

static void ExaRes_ListAllEvtResultsInSelectedExams (struct Exa_Exams *Exams)
  {
   char *ExamsSelectedCommas = NULL;	// Initialized to avoid warning
   const char *Ptr;

   /***** Table head *****/
   ExaRes_ShowHeaderEvtResults (Usr_OTHER);

   /***** List the events results of the selected users *****/
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
	    /***** Show events results *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    ExaRes_ShowEvtResults (Exams,Usr_OTHER,-1L,-1L,ExamsSelectedCommas);
	   }
     }
   free (ExamsSelectedCommas);
  }

/*****************************************************************************/
/**************** Select users to show their events results *****************/
/*****************************************************************************/

void ExaRes_SelUsrsToViewExaResults (void)
  {
   struct Exa_Exams Exams;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);

   /***** Put form to select users *****/
   ExaRes_PutFormToSelUsrsToViewEvtResults (&Exams);
  }

static void ExaRes_PutFormToSelUsrsToViewEvtResults (void *Exams)
  {
   extern const char *Hlp_ASSESSMENT_Exams_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_matches_results;

   if (Exams)	// Not used
      Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
					ActSeeAllExaEvtResCrs,
					NULL,NULL,
					Txt_Results,
					Hlp_ASSESSMENT_Exams_results,
					Txt_View_matches_results,
					false);	// Do not put form with date range
  }

/*****************************************************************************/
/*** Show events results of a exam for the users who answered in that exam **/
/*****************************************************************************/

void ExaRes_ShowAllExaResultsInExa (void)
  {
   extern const char *Txt_Results_of_exam_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Event,
	                     false);	// Do not put form to start new event

   /***** List events results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_exam_X,Exam.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
   ExaRes_ListAllEvtResultsInExa (&Exams,Exam.ExaCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListAllEvtResultsInExa (struct Exa_Exams *Exams,long ExaCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   ExaRes_ShowHeaderEvtResults (Usr_OTHER);

   /***** Get all users who have answered any event question in this exam *****/
   NumUsrs = DB_QuerySELECT (&mysql_res,"can not get users in exam",
			     "SELECT users.UsrCod FROM"
			     " (SELECT DISTINCT exa_prints.UsrCod AS UsrCod"	// row[0]
			     " FROM exa_prints,exa_events,exa_exams"
			     " WHERE exa_events.ExaCod=%ld"
			     " AND exa_events.EvtCod=exa_prints.EvtCod"
			     " AND exa_events.ExaCod=exa_exams.ExaCod"
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
      /***** List events results for each user *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get event code (row[0]) */
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	       if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show events results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  ExaRes_ShowEvtResults (Exams,Usr_OTHER,-1L,ExaCod,NULL);
		 }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/** Show events results of a event for the users who answered in that event */
/*****************************************************************************/

void ExaRes_ShowAllExaResultsInEvt (void)
  {
   extern const char *Txt_Results_of_event_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get parameters *****/
   Exa_GetParams (&Exams);
   if (Exams.ExaCod <= 0)
      Lay_WrongExamExit ();
   Exam.ExaCod = Exams.ExaCod;
   if ((Event.EvtCod = ExaEvt_GetParamEvtCod ()) <= 0)
      Lay_WrongEventExit ();

   /***** Get exam data and event *****/
   Exa_GetDataOfExamByCod (&Exam);
   Exams.ExaCod = Exam.ExaCod;
   ExaEvt_GetDataOfEventByCod (&Event);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,&Event,
	                     false);	// Do not put form to start new event

   /***** List events results in event *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_event_X,Event.Title),
			    false);	// Do not list exams to select
   Str_FreeString ();
   ExaRes_ListAllEvtResultsInEvt (&Exams,Event.EvtCod);
   ExaRes_ShowResultsEnd ();

   /***** Exam end *****/
   Exa_ShowOnlyOneExamEnd ();
  }

static void ExaRes_ListAllEvtResultsInEvt (struct Exa_Exams *Exams,long EvtCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   ExaRes_ShowHeaderEvtResults (Usr_OTHER);

   /***** Get all users who have answered any event question in this exam *****/
   NumUsrs = DB_QuerySELECT (&mysql_res,"can not get users in event",
			     "SELECT users.UsrCod FROM"
			     " (SELECT exa_prints.UsrCod AS UsrCod"	// row[0]
			     " FROM exa_prints,exa_events,exa_exams"
			     " WHERE exa_prints.EvtCod=%ld"
			     " AND exa_prints.EvtCod=exa_events.EvtCod"
			     " AND exa_events.ExaCod=exa_exams.ExaCod"
			     " AND exa_exams.CrsCod=%ld)"		// Extra check
			     " AS users,usr_data"
			     " WHERE users.UsrCod=usr_data.UsrCod"
			     " ORDER BY usr_data.Surname1,"
			               "usr_data.Surname2,"
			               "usr_data.FirstName",
			     EvtCod,
			     Gbl.Hierarchy.Crs.CrsCod);
   if (NumUsrs)
     {
      /***** List events results for each user *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get event code (row[0]) */
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	       if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show events results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  ExaRes_ShowEvtResults (Exams,Usr_OTHER,EvtCod,-1L,NULL);
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

   /***** Begin event results table *****/
   HTM_SECTION_Begin (ExaRes_RESULTS_TABLE_ID);
   HTM_TABLE_BeginWidePadding (2);
  }

static void ExaRes_ShowResultsEnd (void)
  {
   /***** End event results table *****/
   HTM_TABLE_End ();
   HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/********** Write list of those attendance events that have students *********/
/*****************************************************************************/

static void ExaRes_ListExamsToSelect (struct Exa_Exams *Exams)
  {
   extern const char *Hlp_ASSESSMENT_Exams_results;
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
                 Hlp_ASSESSMENT_Exams_results,Box_CLOSABLE);

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

   /***** List the events *****/
   for (NumExam = 0, UniqueId = 1, Gbl.RowEvenOdd = 0;
	NumExam < Exams->Num;
	NumExam++, UniqueId++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      /* Get data of this exam */
      Exam.ExaCod = Exams->Lst[NumExam].ExaCod;
      Exa_GetDataOfExamByCod (&Exam);
      Exams->ExaCod = Exam.ExaCod;

      /* Write a row for this event */
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
/********************* Show header of my events results *********************/
/*****************************************************************************/

static void ExaRes_ShowHeaderEvtResults (Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Event;
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Non_blank_BR_questions;
   extern const char *Txt_Score;
   extern const char *Txt_Average_BR_score_BR_per_question_BR_from_0_to_1;
   extern const char *Txt_Grade;

   HTM_TR_Begin (NULL);

   HTM_TH (1,2,"CT",Txt_User[MeOrOther == Usr_ME ? Gbl.Usrs.Me.UsrDat.Sex :
		                                           Usr_SEX_UNKNOWN]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Dat_START_TIME]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Dat_END_TIME  ]);
   HTM_TH (1,1,"LT",Txt_Event);
   HTM_TH (1,1,"RT",Txt_Questions);
   HTM_TH (1,1,"RT",Txt_Non_blank_BR_questions);
   HTM_TH (1,1,"RT",Txt_Score);
   HTM_TH (1,1,"RT",Txt_Average_BR_score_BR_per_question_BR_from_0_to_1);
   HTM_TH (1,1,"RT",Txt_Grade);
   HTM_TH_Empty (1);

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
/********* Show the events results of a user in the current course **********/
/*****************************************************************************/

static void ExaRes_ShowEvtResults (struct Exa_Exams *Exams,
                                   Usr_MeOrOther_t MeOrOther,
				   long EvtCod,	// <= 0 ==> any
				   long ExaCod,	// <= 0 ==> any
				   const char *ExamsSelectedCommas)
  {
   extern const char *Txt_Event_result;
   char *EvtSubQuery;
   char *ExaSubQuery;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData *UsrDat;
   bool ICanViewResult;
   bool ICanViewScore;
   unsigned NumResults;
   unsigned NumResult;
   static unsigned UniqueId = 0;
   char *Id;
   struct ExaEvt_Event Event;
   Dat_StartEndTime_t StartEndTime;
   unsigned NumQstsInThisResult;
   unsigned NumQstsNotBlankInThisResult;
   unsigned NumTotalQsts = 0;
   unsigned NumTotalQstsNotBlank = 0;
   double ScoreInThisResult;
   double TotalScoreOfAllResults = 0.0;
   double MaxGrade;
   double Grade;
   double TotalGrade = 0.0;
   unsigned Visibility;
   time_t TimeUTC[Dat_NUM_START_END_TIME];

   /***** Reset event *****/
   ExaEvt_ResetEvent (&Event);

   /***** Set user *****/
   UsrDat = (MeOrOther == Usr_ME) ? &Gbl.Usrs.Me.UsrDat :
				    &Gbl.Usrs.Other.UsrDat;

   /***** Build events subquery *****/
   if (EvtCod > 0)
     {
      if (asprintf (&EvtSubQuery," AND exa_prints.EvtCod=%ld",EvtCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&EvtSubQuery,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /***** Build exams subquery *****/
   if (ExaCod > 0)
     {
      if (asprintf (&ExaSubQuery," AND exa_events.ExaCod=%ld",ExaCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else if (ExamsSelectedCommas)
     {
      if (ExamsSelectedCommas[0])
	{
	 if (asprintf (&ExaSubQuery," AND exa_events.ExaCod IN (%s)",
		       ExamsSelectedCommas) < 0)
	    Lay_NotEnoughMemoryExit ();
	}
      else
	{
	 if (asprintf (&ExaSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	}
     }
   else
     {
      if (asprintf (&ExaSubQuery,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /***** Make database query *****/
   NumResults =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get events results",
			      "SELECT exa_prints.EvtCod,"			// row[0]
				     "UNIX_TIMESTAMP(exa_prints.StartTime),"	// row[1]
				     "UNIX_TIMESTAMP(exa_prints.EndTime),"	// row[2]
				     "exa_prints.NumQsts,"			// row[3]
				     "exa_prints.NumQstsNotBlank,"		// row[4]
				     "exa_prints.Score,"			// row[5]
				     "exa_exams.MaxGrade,"			// row[6]
				     "exa_exams.Visibility"			// row[7]
			      " FROM exa_prints,exa_events,exa_exams"
			      " WHERE exa_prints.UsrCod=%ld"
			      "%s"	// Event subquery
			      " AND exa_prints.EvtCod=exa_events.EvtCod"
			      "%s"	// Exams subquery
			      " AND exa_events.ExaCod=exa_exams.ExaCod"
			      " AND exa_exams.CrsCod=%ld"			// Extra check
			      " ORDER BY exa_events.Title",
			      UsrDat->UsrCod,
			      EvtSubQuery,
			      ExaSubQuery,
			      Gbl.Hierarchy.Crs.CrsCod);
   free (ExaSubQuery);
   free (EvtSubQuery);

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);
   Usr_ShowTableCellWithUsrData (UsrDat,NumResults);

   /***** Get and print events results *****/
   if (NumResults)
     {
      for (NumResult = 0;
	   NumResult < NumResults;
	   NumResult++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get event code (row[0]) */
	 if ((Event.EvtCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of event.");
	 ExaEvt_GetDataOfEventByCod (&Event);

	 /* Get visibility (row[7]) */
	 Visibility = TstVis_GetVisibilityFromStr (row[7]);

	 /* Show event result? */
	 ICanViewResult = ExaRes_CheckIfICanSeePrintResult (&Event,UsrDat->UsrCod);
         ICanViewScore  = ExaRes_CheckIfICanViewScore (ICanViewResult,Visibility);

	 if (NumResult)
	    HTM_TR_Begin (NULL);

	 /* Write start/end times (row[1], row[2] hold UTC start/end times) */
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[1 + StartEndTime]);
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

	 /* Write event title */
	 HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Event.Title);
	 HTM_TD_End ();

	 if (ICanViewScore)
	   {
	    /* Get number of questions (row[3]) */
	    if (sscanf (row[3],"%u",&NumQstsInThisResult) != 1)
	       NumQstsInThisResult = 0;
	    NumTotalQsts += NumQstsInThisResult;

	    /* Get number of questions not blank (row[4]) */
	    if (sscanf (row[4],"%u",&NumQstsNotBlankInThisResult) != 1)
	       NumQstsNotBlankInThisResult = 0;
	    NumTotalQstsNotBlank += NumQstsNotBlankInThisResult;

	    Str_SetDecimalPointToUS ();		// To get the decimal point as a dot

	    /* Get score (row[5]) */
	    if (sscanf (row[5],"%lf",&ScoreInThisResult) != 1)
	       ScoreInThisResult = 0.0;
	    TotalScoreOfAllResults += ScoreInThisResult;

	    /* Get maximum grade (row[6]) */
	    if (sscanf (row[6],"%lf",&MaxGrade) != 1)
	       MaxGrade = 0.0;

	    Str_SetDecimalPointToLocal ();	// Return to local system
	   }

	 /* Write number of questions */
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Unsigned (NumQstsInThisResult);
	 else
            Ico_PutIconNotVisible ();
         HTM_TD_End ();

	 /* Write number of questions not blank */
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Unsigned (NumQstsNotBlankInThisResult);
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write score */
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Double2Decimals (ScoreInThisResult);
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write average score per question */
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Double2Decimals (NumQstsInThisResult ? ScoreInThisResult /
					               (double) NumQstsInThisResult :
					               0.0);
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write grade over maximum grade */
	 HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewScore)
	   {
            Grade = TstPrn_ComputeGrade (NumQstsInThisResult,ScoreInThisResult,MaxGrade);
	    TstPrn_ShowGrade (Grade,MaxGrade);
	    TotalGrade += Grade;
	   }
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Link to show this result */
	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewResult)
	   {
	    Exams->ExaCod = Event.ExaCod;
	    Exams->EvtCod = Event.EvtCod;
	    switch (MeOrOther)
	      {
	       case Usr_ME:
		  Frm_StartForm (ActSeeOneExaEvtResMe);
		  ExaEvt_PutParamsEdit (Exams);
		  break;
	       case Usr_OTHER:
		  Frm_StartForm (ActSeeOneExaEvtResOth);
		  ExaEvt_PutParamsEdit (Exams);
		  Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EncryptedUsrCod);
		  break;
	      }
	    Ico_PutIconLink ("tasks.svg",Txt_Event_result);
	    Frm_EndForm ();
	   }
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write totals for this user *****/
      ExaRes_ShowEvtResultsSummaryRow (NumResults,
				       NumTotalQsts,NumTotalQstsNotBlank,
				       TotalScoreOfAllResults,
				       TotalGrade);
     }
   else
     {
      HTM_TD_ColouredEmpty (9);
      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************** Show row with summary of user's events results **************/
/*****************************************************************************/

static void ExaRes_ShowEvtResultsSummaryRow (unsigned NumResults,
                                             unsigned NumTotalQsts,
                                             unsigned NumTotalQstsNotBlank,
                                             double TotalScoreOfAllResults,
					     double TotalGrade)
  {
   extern const char *Txt_Events;

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Row title *****/
   HTM_TD_Begin ("colspan=\"3\" class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtColonNBSP (Txt_Events);
   HTM_Unsigned (NumResults);
   HTM_TD_End ();

   /***** Write total number of questions *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (NumResults)
      HTM_Unsigned (NumTotalQsts);
   HTM_TD_End ();

   /***** Write total number of questions not blank *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (NumResults)
      HTM_Unsigned (NumTotalQstsNotBlank);
   HTM_TD_End ();

   /***** Write total score *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Double2Decimals (TotalScoreOfAllResults);
   HTM_TD_End ();

   /***** Write average score per question *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Double2Decimals (NumTotalQsts ? TotalScoreOfAllResults /
	                               (double) NumTotalQsts :
			               0.0);
   HTM_TD_End ();

   /***** Write total grade *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Double2Decimals (TotalGrade);
   HTM_TD_End ();

   /***** Last cell *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Show one event result of another user *******************/
/*****************************************************************************/

void ExaRes_ShowOneExaResult (void)
  {
   extern const char *Hlp_ASSESSMENT_Exams_results;
   extern const char *Txt_The_user_does_not_exist;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_non_blank_QUESTIONS;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;
   Usr_MeOrOther_t MeOrOther;
   struct UsrData *UsrDat;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct ExaPrn_Print Print;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool ICanViewResult;
   bool ICanViewScore;

   /***** Reset exams context *****/
   Exa_ResetExams (&Exams);
   Exa_ResetExam (&Exam);
   ExaEvt_ResetEvent (&Event);

   /***** Get and check parameters *****/
   ExaEvt_GetAndCheckParameters (&Exams,&Exam,&Event);

   /***** Pointer to user's data *****/
   MeOrOther = (Gbl.Action.Act == ActSeeOneExaEvtResMe) ? Usr_ME :
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

   /***** Get event result data *****/
   ExaPrn_ResetPrint (&Print);
   Print.EvtCod = Event.EvtCod;
   Print.UsrCod = UsrDat->UsrCod;
   ExaPrn_GetPrintDataByEvtCodAndUsrCod (&Print);

   /***** Check if I can view this print result *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewResult = ExaRes_CheckIfICanSeePrintResult (&Event,UsrDat->UsrCod);
	 if (ICanViewResult)
	    ICanViewScore = TstVis_IsVisibleTotalScore (Exam.Visibility);
	 else
	    ICanViewScore = false;
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ICanViewResult =
	 ICanViewScore  = true;
	 break;
      default:
	 ICanViewResult =
	 ICanViewScore  = false;
	 break;
     }

   if (ICanViewResult)	// I am allowed to view this event result
     {
      /***** Get questions and user's answers of exam print from database *****/
      ExaPrn_GetPrintQuestionsFromDB (&Print);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Event.Title,
                    NULL,NULL,
                    Hlp_ASSESSMENT_Exams_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (5);

      /***** Header row *****/
      /* Get data of the user who answer the event */
      if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,Usr_DONT_GET_PREFS))
	 Lay_ShowErrorAndExit (Txt_The_user_does_not_exist);
      if (!Usr_CheckIfICanViewTstExaMchResult (UsrDat))
         Lay_NoPermissionExit ();

      /* User */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_ROLES_SINGUL_Abc[UsrDat->Roles.InCurrentCrs.Role][UsrDat->Sex]);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
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

      /* Start/end time (for user in this event) */
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"DAT_N RT\"");
	 HTM_TxtF ("%s:",Txt_START_END_TIME[StartEndTime]);
	 HTM_TD_End ();

	 if (asprintf (&Id,"match_%u",(unsigned) StartEndTime) < 0)
	    Lay_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"DAT LT\"",Id);
	 Dat_WriteLocalDateHMSFromUTC (Id,Exam.TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,true,0x7);
	 HTM_TD_End ();
         free (Id);

	 HTM_TR_End ();
	}

      /* Number of questions */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Questions);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      HTM_TxtF ("%u (%u %s)",
                Print.NumQsts,
                Print.NumQstsNotBlank,Txt_non_blank_QUESTIONS);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Score */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Score);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
         HTM_Double2Decimals (Print.Score);
      else
         Ico_PutIconNotVisible ();
      HTM_TD_End ();

      HTM_TR_End ();

      /* Grade */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Grade);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
         TstPrn_ComputeAndShowGrade (Print.NumQsts,Print.Score,
                                     Exam.MaxGrade);
      else
         Ico_PutIconNotVisible ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write answers and solutions *****/
      ExaRes_ShowExamAnswers (UsrDat,&Print,Exam.Visibility);

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write total mark of event result *****/
      if (ICanViewScore)
	{
	 HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	 HTM_TxtColonNBSP (Txt_Score);
	 HTM_Double2Decimals (Print.Score);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Grade);
         TstPrn_ComputeAndShowGrade (Print.NumQsts,Print.Score,
                                     Exam.MaxGrade);
         HTM_DIV_End ();
	}

      /***** End box *****/
      Box_BoxEnd ();
     }
   else	// I am not allowed to view this event result
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/********************** Get if I can see event result ************************/
/*****************************************************************************/

static bool ExaRes_CheckIfICanSeePrintResult (struct ExaEvt_Event *Event,long UsrCod)
  {
   bool ItsMe;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ItsMe = Usr_ItsMe (UsrCod);
	 if (ItsMe && Event->ShowUsrResults)
	    return ExaEvt_CheckIfICanListThisEventBasedOnGrps (Event->EvtCod);
         return false;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/********************** Get if I can see event result ************************/
/*****************************************************************************/

static bool ExaRes_CheckIfICanViewScore (bool ICanViewResult,unsigned Visibility)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 if (ICanViewResult)
	    return TstVis_IsVisibleTotalScore (Visibility);
	 return false;
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
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
	NumQst < Print->NumQsts;
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
   bool IsVisibleQstAndAnsTxt = TstVis_IsVisibleQstAndAnsTxt (Visibility);

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

   /***** Number of question and answer type *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (NumQst + 1);
   Tst_WriteAnswerType (Question->Answer.Type);
   HTM_TD_End ();

   /***** Stem, media and answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Stem */
   Tst_WriteQstStem (Question->Stem,"TEST_EXA",IsVisibleQstAndAnsTxt);

   /* Media */
   if (IsVisibleQstAndAnsTxt)
      Med_ShowMedia (&Question->Media,
		     "TEST_MED_SHOW_CONT",
		     "TEST_MED_SHOW");

   /* Answers */
   ExaPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],Question);
   TstPrn_WriteAnswersExam (UsrDat,&Print->PrintedQuestions[NumQst],Question,Visibility);

   /* Write score retrieved from database */
   if (TstVis_IsVisibleEachQstScore (Visibility))
     {
      HTM_DIV_Begin ("class=\"DAT_SMALL LM\"");
      HTM_TxtColonNBSP (Txt_Score);
      HTM_SPAN_Begin ("class=\"%s\"",
		      Print->PrintedQuestions[NumQst].StrAnswers[0] ?
		      (Print->PrintedQuestions[NumQst].Score > 0 ? "ANS_OK" :	// Correct/semicorrect
							           "ANS_BAD") :	// Wrong
							           "ANS_0");	// Blank answer
      HTM_Double2Decimals (Print->PrintedQuestions[NumQst].Score);
      HTM_SPAN_End ();
      HTM_DIV_End ();
     }

   /* Question feedback */
   if (TstVis_IsVisibleFeedbackTxt (Visibility))
      Tst_WriteQstFeedback (Question->Feedback,"TEST_EXA_LIGHT");

   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }
