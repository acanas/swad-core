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
#include "swad_exam_result.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_photo.h"
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
static void ExaRes_GetEventResultDataByEvtCod (long EvtCod,long UsrCod,
                                               struct TstRes_Result *Result);

static bool ExaRes_CheckIfICanSeeEventResult (struct ExaEvt_Event *Event,long UsrCod);
static bool ExaRes_CheckIfICanViewScore (bool ICanViewResult,unsigned Visibility);

/*****************************************************************************/
/*************************** Show my events results *************************/
/*****************************************************************************/

void ExaRes_ShowMyExaResultsInCrs (void)
  {
   extern const char *Txt_Results;
   struct Exa_Exams Exams;

   /***** Reset exams *****/
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
   extern const char *Txt_Results_of_game_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Reset exams *****/
   Exa_ResetExams (&Exams);

   /***** Get parameters *****/
   if ((Exam.ExaCod = Exa_GetParams (&Exams)) <= 0)
      Lay_ShowErrorAndExit ("Code of exam is missing.");
   Exa_GetDataOfExamByCod (&Exam);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,
                             false,	// Do not list exam questions
	                     false);	// Do not put form to start new event

   /***** List my events results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_game_X,Exam.Title),
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
   extern const char *Txt_Results_of_match_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams *****/
   Exa_ResetExams (&Exams);

   /***** Get parameters *****/
   if ((Exam.ExaCod = Exa_GetParams (&Exams)) <= 0)
      Lay_ShowErrorAndExit ("Code of exam is missing.");
   if ((Event.EvtCod = ExaEvt_GetParamEvtCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of event is missing.");
   Exa_GetDataOfExamByCod (&Exam);
   ExaEvt_GetDataOfEventByCod (&Event);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,
                             false,	// Do not list exam questions
	                     false);	// Do not put form to start new event

   /***** List my events results in event *****/
   ExaRes_ShowResultsBegin (&Exams,Str_BuildStringStr (Txt_Results_of_match_X,Event.Title),
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

   /***** Reset exams *****/
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

   /***** Reset exams *****/
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
   extern const char *Txt_Results_of_game_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;

   /***** Reset exams *****/
   Exa_ResetExams (&Exams);

   /***** Get parameters *****/
   if ((Exam.ExaCod = Exa_GetParams (&Exams)) <= 0)
      Lay_ShowErrorAndExit ("Code of exam is missing.");
   Exa_GetDataOfExamByCod (&Exam);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,
                             false,	// Do not list exam questions
	                     false);	// Do not put form to start new event

   /***** List events results in exam *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_game_X,Exam.Title),
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
			     " (SELECT DISTINCT exa_results.UsrCod AS UsrCod"	// row[0]
			     " FROM exa_results,exa_events,exa_exams"
			     " WHERE exa_events.ExaCod=%ld"
			     " AND exa_events.EvtCod=exa_results.EvtCod"
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
   extern const char *Txt_Results_of_match_X;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;

   /***** Reset exams *****/
   Exa_ResetExams (&Exams);

   /***** Get parameters *****/
   if ((Exam.ExaCod = Exa_GetParams (&Exams)) <= 0)
      Lay_ShowErrorAndExit ("Code of exam is missing.");
   if ((Event.EvtCod = ExaEvt_GetParamEvtCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of event is missing.");
   Exa_GetDataOfExamByCod (&Exam);
   ExaEvt_GetDataOfEventByCod (&Event);

   /***** Exam begin *****/
   Exa_ShowOnlyOneExamBegin (&Exams,&Exam,
                             false,	// Do not list exam questions
	                     false);	// Do not put form to start new event

   /***** List events results in event *****/
   ExaRes_ShowResultsBegin (&Exams,
                            Str_BuildStringStr (Txt_Results_of_match_X,Event.Title),
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
			     " (SELECT exa_results.UsrCod AS UsrCod"	// row[0]
			     " FROM exa_results,exa_events,exa_exams"
			     " WHERE exa_results.EvtCod=%ld"
			     " AND exa_results.EvtCod=exa_events.EvtCod"
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
   extern const char *Txt_Match;
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
   HTM_TH (1,1,"LT",Txt_Match);
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
   extern const char *Txt_Match_result;
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

   /***** Set user *****/
   UsrDat = (MeOrOther == Usr_ME) ? &Gbl.Usrs.Me.UsrDat :
				    &Gbl.Usrs.Other.UsrDat;

   /***** Build events subquery *****/
   if (EvtCod > 0)
     {
      if (asprintf (&EvtSubQuery," AND exa_results.EvtCod=%ld",EvtCod) < 0)
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
			      "SELECT exa_results.EvtCod,"			// row[0]
				     "UNIX_TIMESTAMP(exa_results.StartTime),"	// row[1]
				     "UNIX_TIMESTAMP(exa_results.EndTime),"	// row[2]
				     "exa_results.NumQsts,"			// row[3]
				     "exa_results.NumQstsNotBlank,"		// row[4]
				     "exa_results.Score,"			// row[5]
				     "exa_exams.MaxGrade,"			// row[6]
				     "exa_exams.Visibility"			// row[7]
			      " FROM exa_results,exa_events,exa_exams"
			      " WHERE exa_results.UsrCod=%ld"
			      "%s"	// Event subquery
			      " AND exa_results.EvtCod=exa_events.EvtCod"
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
	 ICanViewResult = ExaRes_CheckIfICanSeeEventResult (&Event,UsrDat->UsrCod);
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
            Grade = TstRes_ComputeGrade (NumQstsInThisResult,ScoreInThisResult,MaxGrade);
	    TstRes_ShowGrade (Grade,MaxGrade);
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
	    Ico_PutIconLink ("tasks.svg",Txt_Match_result);
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
   extern const char *Txt_Tags;
   struct Exa_Exams Exams;
   struct Exa_Exam Exam;
   struct ExaEvt_Event Event;
   Usr_MeOrOther_t MeOrOther;
   struct UsrData *UsrDat;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstRes_Result Result;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool ICanViewResult;
   bool ICanViewScore;

   /***** Reset exams *****/
   Exa_ResetExams (&Exams);

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
   TstRes_ResetResult (&Result);
   ExaRes_GetEventResultDataByEvtCod (Event.EvtCod,UsrDat->UsrCod,&Result);

   /***** Check if I can view this event result *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewResult = ExaRes_CheckIfICanSeeEventResult (&Event,UsrDat->UsrCod);
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
      /***** Get questions and user's answers of the event result from database *****/
      ExaRes_GetExamResultQuestionsFromDB (Event.EvtCod,UsrDat->UsrCod,
					   &Result);

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
                Result.NumQsts,
                Result.NumQstsNotBlank,Txt_non_blank_QUESTIONS);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Score */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Score);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
         HTM_Double2Decimals (Result.Score);
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
         TstRes_ComputeAndShowGrade (Result.NumQsts,Result.Score,
                                     Exam.MaxGrade);
      else
         Ico_PutIconNotVisible ();
      HTM_TD_End ();

      HTM_TR_End ();

      /* Tags present in this result */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Tags);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      Exa_ShowTstTagsPresentInAnExam (Event.ExaCod);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write answers and solutions *****/
      TstRes_ShowExamAnswers (UsrDat,&Result,Exam.Visibility);

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write total mark of event result *****/
      if (ICanViewScore)
	{
	 HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	 HTM_TxtColonNBSP (Txt_Score);
	 HTM_Double2Decimals (Result.Score);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Grade);
         TstRes_ComputeAndShowGrade (Result.NumQsts,Result.Score,
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
/************ Get the questions of a event result from database **************/
/*****************************************************************************/

void ExaRes_GetExamResultQuestionsFromDB (long EvtCod,long UsrCod,
				          struct TstRes_Result *Result)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   long LongNum;
   unsigned QstInd;
   struct ExaEvt_UsrAnswer UsrAnswer;

   /***** Get questions and answers of a event result *****/
   Result->NumQsts = (unsigned)
		     DB_QuerySELECT (&mysql_res,"can not get questions and answers"
						" of a event result",
				     "SELECT exa_questions.QstCod,"	// row[0]
					    "exa_questions.QstInd,"	// row[1]
					    "exa_indexes.Indexes"	// row[2]
				     " FROM exa_events,exa_questions,exa_indexes"
				     " WHERE exa_events.EvtCod=%ld"
				     " AND exa_events.ExaCod=exa_questions.ExaCod"
				     " AND exa_events.EvtCod=exa_indexes.EvtCod"
				     " AND exa_questions.QstInd=exa_indexes.QstInd"
				     " ORDER BY exa_questions.QstInd",
				     EvtCod);
   for (NumQst = 0, Result->NumQstsNotBlank = 0;
	NumQst < Result->NumQsts;
	NumQst++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get question code (row[0]) */
      if ((Result->Questions[NumQst].QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Get question index (row[1]) */
      if ((LongNum = Str_ConvertStrCodToLongCod (row[1])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");
      QstInd = (unsigned) LongNum;

      /* Get indexes for this question (row[2]) */
      Str_Copy (Result->Questions[NumQst].StrIndexes,row[2],
                TstRes_MAX_BYTES_INDEXES_ONE_QST);

      /* Get answers selected by user for this question */
      ExaEvt_GetQstAnsFromDB (EvtCod,UsrCod,QstInd,&UsrAnswer);
      if (UsrAnswer.AnsInd >= 0)	// UsrAnswer.AnsInd >= 0 ==> answer selected
	{
         snprintf (Result->Questions[NumQst].StrAnswers,TstRes_MAX_BYTES_ANSWERS_ONE_QST + 1,
		   "%d",UsrAnswer.AnsInd);
         Result->NumQstsNotBlank++;
        }
      else				// UsrAnswer.AnsInd < 0 ==> no answer selected
	 Result->Questions[NumQst].StrAnswers[0] = '\0';	// Empty answer

      /* Replace each comma by a separator of multiple parameters */
      /* In database commas are used as separators instead of special chars */
      Par_ReplaceCommaBySeparatorMultiple (Result->Questions[NumQst].StrIndexes);
      Par_ReplaceCommaBySeparatorMultiple (Result->Questions[NumQst].StrAnswers);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get data of a event result using its event code ***************/
/*****************************************************************************/

static void ExaRes_GetEventResultDataByEvtCod (long EvtCod,long UsrCod,
                                               struct TstRes_Result *Result)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;

   /***** Make database query *****/
   if (DB_QuerySELECT (&mysql_res,"can not get data"
				  " of a event result of a user",
		       "SELECT UNIX_TIMESTAMP(exa_results.StartTime),"	// row[1]
			      "UNIX_TIMESTAMP(exa_results.EndTime),"	// row[2]
		              "exa_results.NumQsts,"			// row[3]
		              "exa_results.NumQstsNotBlank,"		// row[4]
		              "exa_results.Score"			// row[5]
		       " FROM exa_results,exa_events,exa_exams"
		       " WHERE exa_results.EvtCod=%ld"
		       " AND exa_results.UsrCod=%ld"
		       " AND exa_results.EvtCod=exa_events.EvtCod"
		       " AND exa_events.ExaCod=exa_exams.ExaCod"
		       " AND exa_exams.CrsCod=%ld",	// Extra check
		       EvtCod,UsrCod,
		       Gbl.Hierarchy.Crs.CrsCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get start time (row[0] and row[1] hold UTC date-times) */
      for (StartEndTime = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
         Result->TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[StartEndTime]);

      /* Get number of questions (row[2]) */
      if (sscanf (row[2],"%u",&Result->NumQsts) != 1)
	 Result->NumQsts = 0;

      /* Get number of questions not blank (row[3]) */
      if (sscanf (row[3],"%u",&Result->NumQstsNotBlank) != 1)
	 Result->NumQstsNotBlank = 0;

      /* Get score (row[4]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[4],"%lf",&Result->Score) != 1)
	 Result->Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }
   else
     {
      Result->NumQsts = 0;
      Result->NumQstsNotBlank = 0;
      Result->Score = 0.0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get if I can see event result ************************/
/*****************************************************************************/

static bool ExaRes_CheckIfICanSeeEventResult (struct ExaEvt_Event *Event,long UsrCod)
  {
   bool ItsMe;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ItsMe = Usr_ItsMe (UsrCod);
	 if (ItsMe && Event->Status.ShowUsrResults)
	    return ExaEvt_CheckIfICanPlayThisEventBasedOnGrps (Event);
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
