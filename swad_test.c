// swad_test.c: self-assessment tests

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <limits.h>		// For UINT_MAX
#include <linux/limits.h>	// For PATH_MAX
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_action.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_set.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_language.h"
#include "swad_match.h"
#include "swad_media.h"
#include "swad_parameter.h"
#include "swad_question.h"
#include "swad_question_import.h"
#include "swad_tag_database.h"
#include "swad_test.h"
#include "swad_test_config.h"
#include "swad_test_print.h"
#include "swad_test_visibility.h"
#include "swad_theme.h"
#include "swad_user.h"
#include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Tst_ShowFormRequestTest (struct Qst_Questions *Questions);

static void TstPrn_GetAnswersFromForm (struct TstPrn_Print *Print);

static bool Tst_CheckIfNextTstAllowed (void);
static unsigned Tst_GetNumTstExamsGeneratedByMe (void);

static void Tst_DB_IncreaseMyNumTstExams (void);
static void Tst_DB_UpdateLastAccTst (unsigned NumQsts);

static void Tst_PutIconsTests (__attribute__((unused)) void *Args);

static void Tst_ShowFormConfigTst (void);

static void Tst_PutInputFieldNumQst (const char *Field,const char *Label,
                                     unsigned Value);

static void Tst_GetQuestionsForNewTestFromDB (struct Qst_Questions *Questions,
                                              struct TstPrn_Print *Print);
static void Tst_GenerateChoiceIndexes (struct TstPrn_PrintedQuestion *PrintedQuestion,
				       bool Shuffle);

static unsigned Tst_GetParamNumTst (void);
static unsigned Tst_GetParamNumQsts (void);
static unsigned Tst_CountNumTagsInList (const struct Tag_Tags *Tags);
static int Tst_CountNumAnswerTypesInList (const struct Qst_AnswerTypes *AnswerTypes);

/*****************************************************************************/
/********************* Request a self-assessment test ************************/
/*****************************************************************************/

void Tst_RequestTest (void)
  {
   struct Qst_Questions Questions;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Show form to generate a self-assessment test *****/
   Tst_ShowFormRequestTest (&Questions);

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/*************** Show form to generate a self-assessment test ****************/
/*****************************************************************************/

static void Tst_ShowFormRequestTest (struct Qst_Questions *Questions)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Test;
   extern const char *Txt_Number_of_questions;
   extern const char *Txt_Generate_test;
   extern const char *Txt_No_test_questions;
   MYSQL_RES *mysql_res;

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Test,
                 Tst_PutIconsTests,NULL,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

      /***** Get tags *****/
      if ((Questions->Tags.Num = Tag_DB_GetEnabledTagsFromThisCrs (&mysql_res)) != 0)
	{
	 /***** Check if minimum date-time of next access to test is older than now *****/
	 if (Tst_CheckIfNextTstAllowed ())
	   {
	    Frm_BeginForm (ActSeeTst);

	       HTM_TABLE_BeginPadding (2);

		  /***** Selection of tags *****/
		  Tag_ShowFormSelTags (&Questions->Tags,mysql_res,true);

		  /***** Selection of types of answers *****/
		  Qst_ShowFormAnswerTypes (&Questions->AnswerTypes);

		  /***** Number of questions to generate ****/
		  HTM_TR_Begin (NULL);

		     /* Label */
		     Frm_LabelColumn ("RT","NumQst",Txt_Number_of_questions);

		     /* Data */
		     HTM_TD_Begin ("class=\"LT\"");
			HTM_INPUT_LONG ("NumQst",
					(long) TstCfg_GetConfigMin (),
					(long) TstCfg_GetConfigMax (),
					(long) TstCfg_GetConfigDef (),
					HTM_DONT_SUBMIT_ON_CHANGE,
					TstCfg_GetConfigMin () == TstCfg_GetConfigMax (),
					"id=\"NumQst\"");
		     HTM_TD_End ();

		  HTM_TR_End ();

	       HTM_TABLE_End ();

	       /***** Send button *****/
	       Btn_PutConfirmButton (Txt_Generate_test);
	    Frm_EndForm ();
	   }
	}
      else
	{
	 /***** Warning message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_No_test_questions);

	 /***** Button to create a new question *****/
	 if (Qst_CheckIfICanEditQsts ())
	    Qst_PutButtonToAddQuestion ();
	}

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Generate self-assessment test ************************/
/*****************************************************************************/

void Tst_ShowNewTest (void)
  {
   extern const char *Txt_No_questions_found_matching_your_search_criteria;
   struct Qst_Questions Questions;
   struct TstPrn_Print Print;
   unsigned NumTstExamsGeneratedByMe;

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   if (Tst_CheckIfNextTstAllowed ())
     {
      /***** Check that all parameters used to generate a test are valid *****/
      if (Tst_GetParamsTst (&Questions,Tst_SHOW_TEST_TO_ANSWER))	// Get parameters from form
        {
         /***** Get questions *****/
	 TstPrn_ResetPrint (&Print);
	 Tst_GetQuestionsForNewTestFromDB (&Questions,&Print);
         if (Print.NumQsts.All)
           {
            /***** Increase number of exams generated (answered or not) by me *****/
            Tst_DB_IncreaseMyNumTstExams ();
            NumTstExamsGeneratedByMe = Tst_GetNumTstExamsGeneratedByMe ();

	    /***** Create new test exam in database *****/
	    TstPrn_CreatePrintInDB (&Print);
	    TstPrn_ComputeScoresAndStoreQuestionsOfPrint (&Print,
	                                                  false);	// Don't update question score

            /***** Show test exam to be answered *****/
            TstPrn_ShowTestPrintToFillIt (&Print,NumTstExamsGeneratedByMe,TstPrn_REQUEST);

            /***** Update date-time of my next allowed access to test *****/
            if (Gbl.Usrs.Me.Role.Logged == Rol_STD)
               Tst_DB_UpdateLastAccTst (Questions.NumQsts);
           }
         else	// No questions found
           {
            Ale_ShowAlert (Ale_INFO,Txt_No_questions_found_matching_your_search_criteria);
            Tst_ShowFormRequestTest (&Questions);	// Show the form again
           }
        }
      else
         Tst_ShowFormRequestTest (&Questions);	// Show the form again
     }

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/** Receive the draft of a test exam already (total or partially) answered ***/
/*****************************************************************************/

void Tst_ReceiveTestDraft (void)
  {
   extern const char *Txt_The_test_X_has_already_been_assessed_previously;
   extern const char *Txt_Please_review_your_answers_before_submitting_the_exam;
   unsigned NumTst;
   struct TstPrn_Print Print;

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Get basic parameters of the exam *****/
   /* Get test exam code from form */
   TstPrn_ResetPrint (&Print);
   if ((Print.PrnCod = TstPrn_GetParamPrnCod ()) <= 0)
      Err_WrongTestExit ();

   /* Get number of this test from form */
   NumTst = Tst_GetParamNumTst ();

   /***** Get test exam print from database *****/
   TstPrn_GetPrintDataByPrnCod (&Print);

   /****** Get test status in database for this session-course-num.test *****/
   if (Print.Sent)
      Ale_ShowAlert (Ale_WARNING,Txt_The_test_X_has_already_been_assessed_previously,
	             NumTst);
   else // Print not yet sent
     {
      /***** Get test exam print questions from database *****/
      TstPrn_GetPrintQuestionsFromDB (&Print);

      /***** Get answers from form to assess a test *****/
      TstPrn_GetAnswersFromForm (&Print);

      /***** Update test exam in database *****/
      TstPrn_ComputeScoresAndStoreQuestionsOfPrint (&Print,
						    false);	// Don't update question score
      TstPrn_UpdatePrintInDB (&Print);

      /***** Show question and button to send the test *****/
      /* Begin alert */
      Ale_ShowAlert (Ale_WARNING,Txt_Please_review_your_answers_before_submitting_the_exam);

      /* Show the same test exam to be answered */
      TstPrn_ShowTestPrintToFillIt (&Print,NumTst,TstPrn_CONFIRM);
     }
  }

/*****************************************************************************/
/******************************** Assess a test ******************************/
/*****************************************************************************/

void Tst_AssessTest (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Result;
   extern const char *Txt_Test_No_X_that_you_make_in_this_course;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_The_test_X_has_already_been_assessed_previously;
   unsigned NumTst;
   struct TstPrn_Print Print;

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Get basic parameters of the exam *****/
   /* Get test exam code from form */
   TstPrn_ResetPrint (&Print);
   if ((Print.PrnCod = TstPrn_GetParamPrnCod ()) <= 0)
      Err_WrongTestExit ();

   /* Get number of this test from form */
   NumTst = Tst_GetParamNumTst ();

   /***** Get test exam from database *****/
   TstPrn_GetPrintDataByPrnCod (&Print);

   /****** Get test status in database for this session-course-num.test *****/
   if (Print.Sent)
      Ale_ShowAlert (Ale_WARNING,Txt_The_test_X_has_already_been_assessed_previously,
		     NumTst);
   else	// Print not yet sent
     {
      /***** Get test exam questions from database *****/
      TstPrn_GetPrintQuestionsFromDB (&Print);

      /***** Get answers from form to assess a test *****/
      TstPrn_GetAnswersFromForm (&Print);

      /***** Get if test exam will be visible by teachers *****/
      Print.Sent          = true;	// The exam has been finished and sent by student
      Print.AllowTeachers = Par_GetParToBool ("AllowTchs");

      /***** Update test exam in database *****/
      TstPrn_ComputeScoresAndStoreQuestionsOfPrint (&Print,
						    Gbl.Usrs.Me.Role.Logged == Rol_STD);	// Update question score?
      TstPrn_UpdatePrintInDB (&Print);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Txt_Result,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);
	 Lay_WriteHeaderClassPhoto (false,false,
				    Gbl.Hierarchy.Ins.InsCod,
				    Gbl.Hierarchy.Deg.DegCod,
				    Gbl.Hierarchy.Crs.CrsCod);

	 /***** Header *****/
	 if (Gbl.Usrs.Me.IBelongToCurrentCrs)
	   {
	    HTM_DIV_Begin ("class=\"TEST_SUBTITLE\"");
	       HTM_TxtF (Txt_Test_No_X_that_you_make_in_this_course,NumTst);
	    HTM_DIV_End ();
	   }

	 /***** Write answers and solutions *****/
	 TstPrn_ShowPrintAfterAssess (&Print);

	 /***** Write total score and grade *****/
	 if (TstVis_IsVisibleTotalScore (TstCfg_GetConfigVisibility ()))
	   {
	    HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	       HTM_TxtColonNBSP (Txt_Score);
	       HTM_Double2Decimals (Print.Score);
	       HTM_BR ();
	       HTM_TxtColonNBSP (Txt_Grade);
	       TstPrn_ComputeAndShowGrade (Print.NumQsts.All,Print.Score,Tst_SCORE_MAX);
	    HTM_DIV_End ();
	   }

      /***** End box *****/
      Box_BoxEnd ();
     }
  }

/*****************************************************************************/
/****** Get questions and answers from form to assess a test exam print ******/
/*****************************************************************************/

static void TstPrn_GetAnswersFromForm (struct TstPrn_Print *Print)
  {
   unsigned QstInd;
   char StrAns[3 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Loop for every question getting user's answers *****/
   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      /* Get answers selected by user for this question */
      snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
      Par_GetParMultiToText (StrAns,Print->PrintedQuestions[QstInd].StrAnswers,
                             Qst_MAX_BYTES_ANSWERS_ONE_QST);  /* If answer type == T/F ==> " ", "T", "F"; if choice ==> "0", "2",... */
      Par_ReplaceSeparatorMultipleByComma (Print->PrintedQuestions[QstInd].StrAnswers);
     }
  }

/*****************************************************************************/
/************** Check minimum date-time of next access to test ***************/
/*****************************************************************************/
// Return true if allowed date-time of next access to test is older than now

static bool Tst_CheckIfNextTstAllowed (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_You_can_not_take_a_new_test_until;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long NumSecondsFromNowToNextAccTst = -1L;	// Access allowed when this number <= 0
   time_t TimeNextTestUTC = (time_t) 0;

   /***** Teachers and superusers are allowed to do all tests they want *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
       Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      return true;

   /***** Get date of next allowed access to test from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get last access to test",
		       "SELECT UNIX_TIMESTAMP(LastAccTst+INTERVAL (NumQstsLastTst*%lu) SECOND)-"
			      "UNIX_TIMESTAMP(),"						// row[0]
			      "UNIX_TIMESTAMP(LastAccTst+INTERVAL (NumQstsLastTst*%lu) SECOND)"	// row[1]
		        " FROM crs_user_settings"
		       " WHERE UsrCod=%ld"
		         " AND CrsCod=%ld",
		       TstCfg_GetConfigMinTimeNxtTstPerQst (),
		       TstCfg_GetConfigMinTimeNxtTstPerQst (),
		       Gbl.Usrs.Me.UsrDat.UsrCod,
		       Gbl.Hierarchy.Crs.CrsCod) == 1)
     {
      /* Get seconds from now to next access to test */
      row = mysql_fetch_row (mysql_res);
      if (row[0])
         if (sscanf (row[0],"%ld",&NumSecondsFromNowToNextAccTst) == 1)
            /* Time UTC of next access allowed (row[1]) */
            TimeNextTestUTC = Dat_GetUNIXTimeFromStr (row[1]);
     }
   else
      Err_ShowErrorAndExit ("Error when reading date of next allowed access to test.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Check if access is allowed *****/
   if (NumSecondsFromNowToNextAccTst > 0)
     {
      /***** Write warning *****/
      Ale_ShowAlert (Ale_WARNING,"%s:<br /><span id=\"date_next_test\"></span>."
		     "<script type=\"text/javascript\">"
		     "writeLocalDateHMSFromUTC('date_next_test',%ld,"
		     "%u,',&nbsp;',%u,true,true,true,0x7);"
		     "</script>",
		     Txt_You_can_not_take_a_new_test_until,
		     (long) TimeNextTestUTC,
		     (unsigned) Gbl.Prefs.DateFormat,
		     (unsigned) Gbl.Prefs.Language);

      return false;
     }
   return true;
  }

/*****************************************************************************/
/***************** Get number of test exams generated by me ******************/
/*****************************************************************************/

static unsigned Tst_GetNumTstExamsGeneratedByMe (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumTstExamsGeneratedByMe = 0;

   if (Gbl.Usrs.Me.IBelongToCurrentCrs)
     {
      /***** Get number of test exams generated by me from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get number of test exams generated",
				"SELECT NumAccTst"	// row[0]
				 " FROM crs_user_settings"
				" WHERE UsrCod=%ld"
				  " AND CrsCod=%ld",
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Gbl.Hierarchy.Crs.CrsCod);

      if (NumRows == 0)
         NumTstExamsGeneratedByMe = 0;
      else if (NumRows == 1)
        {
         /* Get number of hits */
         row = mysql_fetch_row (mysql_res);
         if (row[0] == NULL)
            NumTstExamsGeneratedByMe = 0;
         else if (sscanf (row[0],"%u",&NumTstExamsGeneratedByMe) != 1)
            NumTstExamsGeneratedByMe = 0;
        }
      else
         Err_ShowErrorAndExit ("Error when getting number of hits to test.");

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return NumTstExamsGeneratedByMe;
  }

/*****************************************************************************/
/*********** Update my number of accesses to test in this course *************/
/*****************************************************************************/

static void Tst_DB_IncreaseMyNumTstExams (void)
  {
   /***** Trivial check *****/
   if (!Gbl.Usrs.Me.IBelongToCurrentCrs)
      return;

   /***** Update my number of accesses to test in this course *****/
   DB_QueryUPDATE ("can not update the number of accesses to test",
		   "UPDATE crs_user_settings"
		     " SET NumAccTst=NumAccTst+1"
                   " WHERE UsrCod=%ld"
                     " AND CrsCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************ Update date-time of my next allowed access to test *************/
/*****************************************************************************/

static void Tst_DB_UpdateLastAccTst (unsigned NumQsts)
  {
   /***** Update date-time and number of questions of this test *****/
   DB_QueryUPDATE ("can not update time and number of questions of this test",
		   "UPDATE crs_user_settings"
		     " SET LastAccTst=NOW(),"
		          "NumQstsLastTst=%u"
                   " WHERE UsrCod=%ld"
                     " AND CrsCod=%ld",
		   NumQsts,
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/********************* Put contextual icons in tests *************************/
/*****************************************************************************/

static void Tst_PutIconsTests (__attribute__((unused)) void *Args)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
         /***** Put icon to view test results *****/
	 Ico_PutContextualIconToShowResults (ActReqSeeMyTstRes,NULL,
					     NULL,NULL);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put icon to go to test configuration *****/
	 Ico_PutContextualIconToConfigure (ActCfgTst,
					   NULL,NULL);

	 /***** Put icon to edit tags *****/
	 Tag_PutIconToEditTags ();

         /***** Put icon to view test results *****/
	 Ico_PutContextualIconToShowResults (ActReqSeeUsrTstRes,NULL,
					     NULL,NULL);
	 break;
      default:
	 break;
     }

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_TESTS);
  }

/*****************************************************************************/
/***************************** Form to rename tags ***************************/
/*****************************************************************************/

void Tst_ShowFormConfig (void)
  {
   extern const char *Txt_Please_specify_if_you_allow_downloading_the_question_bank_from_other_applications;

   /***** If current course has tests and pluggable is unknown... *****/
   if (Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown ())
      Ale_ShowAlert (Ale_WARNING,Txt_Please_specify_if_you_allow_downloading_the_question_bank_from_other_applications);

   /***** Form to configure test *****/
   Tst_ShowFormConfigTst ();
  }

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/
// Returns true if course has test tags and pluggable is unknown
// Return false if course has no test tags or pluggable is known

bool Tst_CheckIfCourseHaveTestsAndPluggableIsUnknown (void)
  {
   extern const char *TstCfg_PluggableDB[TstCfg_NUM_OPTIONS_PLUGGABLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   TstCfg_Pluggable_t Pluggable;

   /***** Get pluggability of tests for current course from database *****/
   NumRows = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get configuration of test",
		   "SELECT Pluggable"		// row[0]
		    " FROM tst_config"
		   " WHERE CrsCod=%ld",
		   Gbl.Hierarchy.Crs.CrsCod);

   if (NumRows == 0)
      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
   else // NumRows == 1
     {
      /***** Get whether test are visible via plugins or not *****/
      row = mysql_fetch_row (mysql_res);

      TstCfg_SetConfigPluggable (TstCfg_PLUGGABLE_UNKNOWN);
      for (Pluggable  = TstCfg_PLUGGABLE_NO;
	   Pluggable <= TstCfg_PLUGGABLE_YES;
	   Pluggable++)
         if (!strcmp (row[0],TstCfg_PluggableDB[Pluggable]))
           {
            TstCfg_SetConfigPluggable (Pluggable);
            break;
           }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get if current course has tests from database *****/
   if (TstCfg_GetConfigPluggable () == TstCfg_PLUGGABLE_UNKNOWN)
      return Tag_DB_CheckIfCurrentCrsHasTestTags ();	// Return true if course has tests

   return false;	// Pluggable is not unknown
  }

/*****************************************************************************/
/********************* Show a form to to configure test **********************/
/*****************************************************************************/

static void Tst_ShowFormConfigTst (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_configuring_tests;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Configure_tests;
   extern const char *Txt_Plugins;
   extern const char *Txt_TST_PLUGGABLE[TstCfg_NUM_OPTIONS_PLUGGABLE];
   extern const char *Txt_Number_of_questions;
   extern const char *Txt_minimum;
   extern const char *Txt_default;
   extern const char *Txt_maximum;
   extern const char *Txt_Minimum_time_seconds_per_question_between_two_tests;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Save_changes;
   struct Qst_Questions Questions;
   TstCfg_Pluggable_t Pluggable;
   char StrMinTimeNxtTstPerQst[Cns_MAX_DECIMAL_DIGITS_ULONG + 1];

   /***** Create test *****/
   Qst_Constructor (&Questions);

   /***** Read test configuration from database *****/
   TstCfg_GetConfigFromDB ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Configure_tests,
                 Tst_PutIconsTests,NULL,
                 Hlp_ASSESSMENT_Tests_configuring_tests,Box_NOT_CLOSABLE);

      /***** Begin form *****/
      Frm_BeginForm (ActRcvCfgTst);

	 /***** Tests are visible from plugins? *****/
	 HTM_TABLE_BeginCenterPadding (2);
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
		  HTM_TxtColon (Txt_Plugins);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LB\"");
		  for (Pluggable  = TstCfg_PLUGGABLE_NO;
		       Pluggable <= TstCfg_PLUGGABLE_YES;
		       Pluggable++)
		    {
		     HTM_LABEL_Begin ("class=\"DAT\"");
			HTM_INPUT_RADIO ("Pluggable",false,
					 "value=\"%u\"%s",
					 (unsigned) Pluggable,
					 Pluggable == TstCfg_GetConfigPluggable () ? " checked=\"checked\"" :
										     "");
			HTM_Txt (Txt_TST_PLUGGABLE[Pluggable]);
		     HTM_LABEL_End ();
		     HTM_BR ();
		    }
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Number of questions *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
		  HTM_TxtColon (Txt_Number_of_questions);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LB\"");
		  HTM_TABLE_BeginPadding (2);
		     Tst_PutInputFieldNumQst ("NumQstMin",Txt_minimum,
					      TstCfg_GetConfigMin ());	// Minimum number of questions
		     Tst_PutInputFieldNumQst ("NumQstDef",Txt_default,
					      TstCfg_GetConfigDef ());	// Default number of questions
		     Tst_PutInputFieldNumQst ("NumQstMax",Txt_maximum,
					      TstCfg_GetConfigMax ());	// Maximum number of questions
		  HTM_TABLE_End ();
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Minimum time between consecutive tests, per question *****/
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("RT","MinTimeNxtTstPerQst",
				Txt_Minimum_time_seconds_per_question_between_two_tests);

	       /* Data */
	       HTM_TD_Begin ("class=\"LB\"");
		  snprintf (StrMinTimeNxtTstPerQst,sizeof (StrMinTimeNxtTstPerQst),"%lu",
			    TstCfg_GetConfigMinTimeNxtTstPerQst ());
		  HTM_INPUT_TEXT ("MinTimeNxtTstPerQst",Cns_MAX_DECIMAL_DIGITS_ULONG,StrMinTimeNxtTstPerQst,
				  HTM_DONT_SUBMIT_ON_CHANGE,
				  "id=\"MinTimeNxtTstPerQst\" size=\"7\" required=\"required\"");
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Visibility of test exams *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
		  HTM_TxtColon (Txt_Result_visibility);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"LB\"");
		  TstVis_PutVisibilityCheckboxes (TstCfg_GetConfigVisibility ());
	       HTM_TD_End ();

	    HTM_TR_End ();

	 HTM_TABLE_End ();

	 /***** Send button *****/
	 Btn_PutConfirmButton (Txt_Save_changes);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Destroy test *****/
   Qst_Destructor (&Questions);
  }

/*****************************************************************************/
/*************** Get configuration of test for current course ****************/
/*****************************************************************************/

static void Tst_PutInputFieldNumQst (const char *Field,const char *Label,
                                     unsigned Value)
  {
   char StrValue[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RM\"");
	 HTM_LABEL_Begin ("for=\"%s\" class=\"DAT\"",Field);
	    HTM_Txt (Label);
	 HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM\"");
	 snprintf (StrValue,sizeof (StrValue),"%u",Value);
	 HTM_INPUT_TEXT (Field,Cns_MAX_DECIMAL_DIGITS_UINT,StrValue,
			 HTM_DONT_SUBMIT_ON_CHANGE,
			 "id=\"%s\" size=\"3\" required=\"required\"",Field);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Get questions for a new test from the database ***************/
/*****************************************************************************/

#define Tst_MAX_BYTES_QUERY_QUESTIONS (16 * 1024 - 1)

static void Tst_GetQuestionsForNewTestFromDB (struct Qst_Questions *Questions,
                                              struct TstPrn_Print *Print)
  {
   extern const char *Qst_DB_StrAnswerTypes[Qst_NUM_ANS_TYPES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char *Query = NULL;
   long LengthQuery;
   unsigned NumItemInList;
   const char *Ptr;
   char TagText[Tag_MAX_BYTES_TAG + 1];
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   Qst_AnswerType_t AnswerType;
   bool Shuffle;
   char StrNumQsts[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned QstInd;

   /***** Trivial check: number of questions *****/
   if (Questions->NumQsts == 0 ||
       Questions->NumQsts > TstCfg_MAX_QUESTIONS_PER_TEST)
      Err_ShowErrorAndExit ("Wrong number of questions.");

   /***** Allocate space for query *****/
   if ((Query = malloc (Tst_MAX_BYTES_QUERY_QUESTIONS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Select questions without hidden tags *****/
   /* Begin query */
   // Reject questions with any tag hidden
   // Select only questions with tags
   // DISTINCTROW is necessary to not repeat questions
   snprintf (Query,Tst_MAX_BYTES_QUERY_QUESTIONS + 1,
	     "SELECT DISTINCTROW tst_questions.QstCod,"		// row[0]
                                "tst_questions.AnsType,"	// row[1]
                                "tst_questions.Shuffle"		// row[2]
	      " FROM tst_questions,tst_question_tags,tst_tags"
	     " WHERE tst_questions.CrsCod=%ld"
	       " AND tst_questions.QstCod NOT IN"
		   " (SELECT tst_question_tags.QstCod"
		      " FROM tst_tags,tst_question_tags"
		     " WHERE tst_tags.CrsCod=%ld"
		       " AND tst_tags.TagHidden='Y'"
		       " AND tst_tags.TagCod=tst_question_tags.TagCod)"
	       " AND tst_questions.QstCod=tst_question_tags.QstCod"
	       " AND tst_question_tags.TagCod=tst_tags.TagCod"
	       " AND tst_tags.CrsCod=%ld",
	     Gbl.Hierarchy.Crs.CrsCod,
	     Gbl.Hierarchy.Crs.CrsCod,
	     Gbl.Hierarchy.Crs.CrsCod);

   if (!Questions->Tags.All) // User has not selected all the tags
     {
      /* Add selected tags */
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Questions->Tags.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tag_MAX_BYTES_TAG);
         LengthQuery = LengthQuery + 35 + strlen (TagText) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_tags.TagTxt='" :
                                     " AND (tst_tags.TagTxt='",
                     Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,TagText,Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Tst_MAX_BYTES_QUERY_QUESTIONS);
         NumItemInList++;
        }
      Str_Concat (Query,")",Tst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* Add answer types selected */
   if (!Questions->AnswerTypes.All)
     {
      LengthQuery = strlen (Query);
      NumItemInList = 0;
      Ptr = Questions->AnswerTypes.List;
      while (*Ptr)
        {
         Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Tag_MAX_BYTES_TAG);
	 AnswerType = Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
         LengthQuery = LengthQuery + 35 + strlen (Qst_DB_StrAnswerTypes[AnswerType]) + 1;
         if (LengthQuery > Tst_MAX_BYTES_QUERY_QUESTIONS - 128)
            Err_ShowErrorAndExit ("Query size exceed.");
         Str_Concat (Query,
                     NumItemInList ? " OR tst_questions.AnsType='" :
                                     " AND (tst_questions.AnsType='",
                     Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,Qst_DB_StrAnswerTypes[AnswerType],Tst_MAX_BYTES_QUERY_QUESTIONS);
         Str_Concat (Query,"'",Tst_MAX_BYTES_QUERY_QUESTIONS);
         NumItemInList++;
        }
      Str_Concat (Query,")",Tst_MAX_BYTES_QUERY_QUESTIONS);
     }

   /* End query */
   Str_Concat (Query," ORDER BY RAND() LIMIT ",Tst_MAX_BYTES_QUERY_QUESTIONS);
   snprintf (StrNumQsts,sizeof (StrNumQsts),"%u",Questions->NumQsts);
   Str_Concat (Query,StrNumQsts,Tst_MAX_BYTES_QUERY_QUESTIONS);
/*
   if (Gbl.Usrs.Me.Roles.LoggedRole == Rol_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);
*/
   /* Make the query */
   Print->NumQsts.All =
   Questions->NumQsts = (unsigned) DB_QuerySELECT (&mysql_res,"can not get questions",
			                           "%s",
			                           Query);

   /***** Get questions and answers from database *****/
   for (QstInd = 0;
	QstInd < Print->NumQsts.All;
	QstInd++)
     {
      /* Get question row */
      row = mysql_fetch_row (mysql_res);
      /*
      QstCod	row[0]
      AnsType	row[1]
      Shuffle	row[2]
      */

      /* Get question code (row[0]) */
      if ((Print->PrintedQuestions[QstInd].QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	 Err_ShowErrorAndExit ("Wrong code of question.");

      /* Get answer type (row[1]) */
      AnswerType = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

      /* Get shuffle (row[2]) */
      Shuffle = (row[2][0] == 'Y');

      /* Set indexes of answers */
      switch (AnswerType)
	{
	 case Qst_ANS_INT:
	 case Qst_ANS_FLOAT:
	 case Qst_ANS_TRUE_FALSE:
	 case Qst_ANS_TEXT:
	    Print->PrintedQuestions[QstInd].StrIndexes[0] = '\0';
	    break;
	 case Qst_ANS_UNIQUE_CHOICE:
	 case Qst_ANS_MULTIPLE_CHOICE:
            /* If answer type is unique or multiple option,
               generate indexes of answers depending on shuffle */
	    Tst_GenerateChoiceIndexes (&Print->PrintedQuestions[QstInd],Shuffle);
	    break;
	 default:
	    break;
	}

      /* Reset user's answers.
         Initially user has not answered the question ==> initially all the answers will be blank.
         If the user does not confirm the submission of their exam ==>
         ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
      Print->PrintedQuestions[QstInd].StrAnswers[0] = '\0';
     }

   /***** Get if test exam will be visible by teachers *****/
   Print->AllowTeachers = Par_GetParToBool ("AllowTchs");
  }

/*****************************************************************************/
/*************** Generate choice indexes depending on shuffle ****************/
/*****************************************************************************/

static void Tst_GenerateChoiceIndexes (struct TstPrn_PrintedQuestion *PrintedQuestion,
				       bool Shuffle)
  {
   struct Qst_Question Question;
   unsigned NumOpt;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned Index;
   bool ErrorInIndex;
   char StrInd[1 + Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Create test question *****/
   Qst_QstConstructor (&Question);
   Question.QstCod = PrintedQuestion->QstCod;

   /***** Get answers of question from database *****/
   Qst_GetAnswersQst (&Question,&mysql_res,Shuffle);
   /*
   row[0] AnsInd
   row[1] Answer
   row[2] Feedback
   row[3] MedCod
   row[4] Correct
   */

   for (NumOpt = 0;
	NumOpt < Question.Answer.NumOptions;
	NumOpt++)
     {
      /***** Get next answer *****/
      row = mysql_fetch_row (mysql_res);

      /***** Assign index (row[0]).
             Index is 0,1,2,3... if no shuffle
             or 1,3,0,2... (example) if shuffle *****/
      ErrorInIndex = false;
      if (sscanf (row[0],"%u",&Index) == 1)
        {
         if (Index >= Qst_MAX_OPTIONS_PER_QUESTION)
            ErrorInIndex = true;
        }
      else
         ErrorInIndex = true;
      if (ErrorInIndex)
         Err_WrongAnswerIndexExit ();

      snprintf (StrInd,sizeof (StrInd),NumOpt ? ",%u" :
						"%u",Index);
      Str_Concat (PrintedQuestion->StrIndexes,StrInd,
                  sizeof (PrintedQuestion->StrIndexes) - 1);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Destroy test question *****/
   Qst_QstDestructor (&Question);
  }

/*****************************************************************************/
/************ Get parameters for the selection of test questions *************/
/*****************************************************************************/
// Return true (OK) if all parameters are found, or false (error) if any necessary parameter is not found

bool Tst_GetParamsTst (struct Qst_Questions *Questions,
                       Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions)
  {
   extern const char *Txt_You_must_select_one_ore_more_tags;
   extern const char *Txt_You_must_select_one_ore_more_types_of_answer;
   extern const char *Txt_The_number_of_questions_must_be_in_the_interval_X;
   bool Error = false;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned UnsignedNum;

   /***** Tags *****/
   /* Get parameter that indicates whether all tags are selected */
   Questions->Tags.All = Par_GetParToBool ("AllTags");

   /* Get the tags */
   if ((Questions->Tags.List = malloc (Tag_MAX_BYTES_TAGS_LIST + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   Par_GetParMultiToText ("ChkTag",Questions->Tags.List,Tag_MAX_BYTES_TAGS_LIST);

   /* Check number of tags selected */
   if (Tst_CountNumTagsInList (&Questions->Tags) == 0)	// If no tags selected...
     {						// ...write alert
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_tags);
      Error = true;
     }

   /***** Types of answer *****/
   switch (ActionToDoWithQuestions)
     {
      case Tst_SHOW_TEST_TO_ANSWER:
      case Tst_EDIT_QUESTIONS:
      case Tst_SELECT_QUESTIONS_FOR_EXAM:
	 /* Get parameter that indicates if all types of answer are selected */
	 Questions->AnswerTypes.All = Par_GetParToBool ("AllAnsTypes");

	 /* Get types of answer */
	 Par_GetParMultiToText ("AnswerType",Questions->AnswerTypes.List,Qst_MAX_BYTES_LIST_ANSWER_TYPES);

	 /* Check number of types of answer */
	 if (Tst_CountNumAnswerTypesInList (&Questions->AnswerTypes) == 0)	// If no types of answer selected...
	   {								// ...write warning alert
	    Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_types_of_answer);
	    Error = true;
	   }
	 break;
      case Tst_SELECT_QUESTIONS_FOR_GAME:
	 /* The unique allowed type of answer in a game is unique choice */
	 Questions->AnswerTypes.All = false;
	 snprintf (Questions->AnswerTypes.List,sizeof (Questions->AnswerTypes.List),"%u",
		   (unsigned) Qst_ANS_UNIQUE_CHOICE);
	 break;
      default:
	 break;
     }

   /***** Get other parameters, depending on action *****/
   switch (ActionToDoWithQuestions)
     {
      case Tst_SHOW_TEST_TO_ANSWER:
	 Questions->NumQsts = Tst_GetParamNumQsts ();
	 if (Questions->NumQsts < TstCfg_GetConfigMin () ||
	     Questions->NumQsts > TstCfg_GetConfigMax ())
	   {
	    Ale_ShowAlert (Ale_WARNING,Txt_The_number_of_questions_must_be_in_the_interval_X,
		           TstCfg_GetConfigMin (),TstCfg_GetConfigMax ());
	    Error = true;
	   }
	 break;
      case Tst_EDIT_QUESTIONS:
	 /* Get starting and ending dates */
	 Dat_GetIniEndDatesFromForm ();

	 /* Get ordering criteria */
	 Par_GetParMultiToText ("Order",UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
	 if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	    Questions->SelectedOrder = (Qst_QuestionsOrder_t)
	                          ((UnsignedNum < Qst_NUM_TYPES_ORDER_QST) ? UnsignedNum :
									     0);
	 else
	    Questions->SelectedOrder = (Qst_QuestionsOrder_t) 0;
	 break;
      case Tst_SELECT_QUESTIONS_FOR_EXAM:
      case Tst_SELECT_QUESTIONS_FOR_GAME:
	 /* Get starting and ending dates */
	 Dat_GetIniEndDatesFromForm ();

	 /* Order question by stem */
	 Questions->SelectedOrder = Qst_ORDER_STEM;
	 break;
      default:
	 break;
     }

   return !Error;
  }

/*****************************************************************************/
/******** Get parameter with the number of test exam generated by me *********/
/*****************************************************************************/

static unsigned Tst_GetParamNumTst (void)
  {
   return (unsigned) Par_GetParToUnsignedLong ("NumTst",
                                               1,
                                               UINT_MAX,
                                               1);
  }

/*****************************************************************************/
/***** Get parameter with the number of questions to generate in an test *****/
/*****************************************************************************/

static unsigned Tst_GetParamNumQsts (void)
  {
   return (unsigned) Par_GetParToUnsignedLong ("NumQst",
	                                       (unsigned long) TstCfg_GetConfigMin (),
	                                       (unsigned long) TstCfg_GetConfigMax (),
	                                       (unsigned long) TstCfg_GetConfigDef ());
  }

/*****************************************************************************/
/***************** Count number of tags in the list of tags ******************/
/*****************************************************************************/

static unsigned Tst_CountNumTagsInList (const struct Tag_Tags *Tags)
  {
   const char *Ptr;
   unsigned NumTags = 0;
   char TagText[Tag_MAX_BYTES_TAG + 1];

   /***** Go over the list of tags counting the number of tags *****/
   Ptr = Tags->List;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,TagText,Tag_MAX_BYTES_TAG);
      NumTags++;
     }

   return NumTags;
  }

/*****************************************************************************/
/**** Count the number of types of answers in the list of types of answers ***/
/*****************************************************************************/

static int Tst_CountNumAnswerTypesInList (const struct Qst_AnswerTypes *AnswerTypes)
  {
   const char *Ptr;
   int NumAnsTypes = 0;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Go over the list of answer types counting the number of types of answer *****/
   Ptr = AnswerTypes->List;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
      Qst_ConvertFromUnsignedStrToAnsTyp (UnsignedStr);
      NumAnsTypes++;
     }
   return NumAnsTypes;
  }

/*****************************************************************************/
/**** Count the number of questions in the list of selected question codes ***/
/*****************************************************************************/

unsigned Tst_CountNumQuestionsInList (const char *ListQuestions)
  {
   const char *Ptr;
   unsigned NumQuestions = 0;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;

   /***** Go over list of questions counting the number of questions *****/
   Ptr = ListQuestions;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
      if (sscanf (LongStr,"%ld",&QstCod) != 1)
         Err_WrongQuestionExit ();
      NumQuestions++;
     }
   return NumQuestions;
  }

/*****************************************************************************/
/************************* Remove all tests in a course **********************/
/*****************************************************************************/

void Tst_RemoveCrsTests (long CrsCod)
  {
   /***** Remove all test exam prints made in the course *****/
   TstPrn_RemoveCrsPrints (CrsCod);

   /***** Remove test configuration of the course *****/
   DB_QueryDELETE ("can not remove configuration of tests of a course",
		   "DELETE FROM tst_config"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }
