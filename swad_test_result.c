// swad_test_result.c: test results

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_test.h"
#include "swad_test_visibility.h"
#include "swad_user.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Tst_NUM_STATUS 2
typedef enum
  {
   Tst_STATUS_SHOWN_BUT_NOT_ASSESSED	= 0,
   Tst_STATUS_ASSESSED			= 1,
   Tst_STATUS_ERROR			= 2,
  } Tst_Status_t;

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

static void TsR_ShowUsrsTstResults (void);
static void TsR_ShowHeaderTestResults (void);
static void TsR_ShowTstResults (struct UsrData *UsrDat);
static void TsR_PutParamTstCod (long TstCod);
static long TsR_GetParamTstCod (void);
static void TsR_ShowTestResultsSummaryRow (bool ItsMe,
                                           unsigned NumExams,
                                           unsigned NumTotalQsts,
                                           unsigned NumTotalQstsNotBlank,
                                           double TotalScoreOfAllTests);
static void TsR_ShowTstTagsPresentInATestResult (long TstCod);
static void TsR_GetTestResultDataByTstCod (long TstCod,
                                           bool *AllowTeachers,
                                           time_t *TstTimeUTC,
                                           unsigned *NumQstsNotBlank,
                                           double *Score);
static void TsR_GetTestResultQuestionsFromDB (long TstCod);

/*****************************************************************************/
/************ Select users and dates to show their test results **************/
/*****************************************************************************/

void TsR_SelUsrsToViewUsrsTstResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_test_results;

   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeUsrTstRes,NULL,
				     Txt_Results,
                                     Hlp_ASSESSMENT_Tests_results,
                                     Txt_View_test_results,
				     true);	// Put form with date range
  }

/*****************************************************************************/
/******************* Select dates to show my test results ********************/
/*****************************************************************************/

void TsR_SelDatesToSeeMyTstResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_test_results;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_DO_NOT_SET,
      Dat_HMS_DO_NOT_SET
     };

   /***** Begin form *****/
   Frm_StartForm (ActSeeMyTstRes);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Results,NULL,
                      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);
   Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (SetHMS);

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_View_test_results);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** Show my test results **************************/
/*****************************************************************************/

void TsR_ShowMyTstResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Results,NULL,
                      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);

   /***** Header of the table with the list of users *****/
   TsR_ShowHeaderTestResults ();

   /***** List my test results *****/
   TstCfg_GetConfigFromDB ();	// To get feedback type
   TsR_ShowTstResults (&Gbl.Usrs.Me.UsrDat);

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/********************* Store test result in database *************************/
/*****************************************************************************/

long TsR_CreateTestResultInDB (bool AllowTeachers)
  {
   /***** Insert new test result into table *****/
   return
   DB_QueryINSERTandReturnCode ("can not create new test result",
				"INSERT INTO tst_exams"
				" (CrsCod,UsrCod,AllowTeachers,TstTime,NumQsts)"
				" VALUES"
				" (%ld,%ld,'%c',NOW(),%u)",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				AllowTeachers ? 'Y' :
						'N',
				Gbl.Test.NumQsts);
  }

/*****************************************************************************/
/********************* Store test result in database *************************/
/*****************************************************************************/

void TsR_StoreScoreOfTestResultInDB (long TstCod,
                                     unsigned NumQstsNotBlank,double Score)
  {
   /***** Update score in test result *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   DB_QueryUPDATE ("can not update result of test result",
		   "UPDATE tst_exams"
	           " SET NumQstsNotBlank=%u,Score='%.15lg'"
	           " WHERE TstCod=%ld",
		   NumQstsNotBlank,Score,
		   TstCod);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/******************* Get users and show their test results *******************/
/*****************************************************************************/

void TsR_GetUsrsAndShowTstResults (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  TsR_ShowUsrsTstResults,
                                  TsR_SelUsrsToViewUsrsTstResults);
  }

/*****************************************************************************/
/******************** Show test results for several users ********************/
/*****************************************************************************/

static void TsR_ShowUsrsTstResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Results;
   const char *Ptr;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Results,NULL,
		      Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE,2);

   /***** Header of the table with the list of users *****/
   TsR_ShowHeaderTestResults ();

   /***** List the test exams of the selected users *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 if (Usr_CheckIfICanViewTst (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show test results *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    TsR_ShowTstResults (&Gbl.Usrs.Other.UsrDat);
	   }
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/*********************** Show header of my test results **********************/
/*****************************************************************************/

static void TsR_ShowHeaderTestResults (void)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Date;
   extern const char *Txt_Questions;
   extern const char *Txt_Non_blank_BR_questions;
   extern const char *Txt_Score;
   extern const char *Txt_Average_BR_score_BR_per_question_BR_from_0_to_1;
   extern const char *Txt_Grade;

   HTM_TR_Begin (NULL);

   HTM_TH (1,2,"CT",Txt_User[Usr_SEX_UNKNOWN]);
   HTM_TH (1,1,"RT",Txt_Date);
   HTM_TH (1,1,"RT",Txt_Questions);
   HTM_TH (1,1,"RT",Txt_Non_blank_BR_questions);
   HTM_TH (1,1,"RT",Txt_Score);
   HTM_TH (1,1,"RT",Txt_Average_BR_score_BR_per_question_BR_from_0_to_1);
   HTM_TH (1,1,"RT",Txt_Grade);
   HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Show the test results of a user in the current course ***********/
/*****************************************************************************/

static void TsR_ShowTstResults (struct UsrData *UsrDat)
  {
   extern const char *Txt_View_test;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumExams;
   unsigned NumTest;
   static unsigned UniqueId = 0;
   char *Id;
   long TstCod;
   unsigned NumQstsInThisTest;
   unsigned NumQstsNotBlankInThisTest;
   unsigned NumTotalQsts = 0;
   unsigned NumTotalQstsNotBlank = 0;
   double ScoreInThisTest;
   double TotalScoreOfAllTests = 0.0;
   unsigned NumExamsVisibleByTchs = 0;
   bool AllowTeachers;	// Can teachers of this course see the test result?
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);
   bool ICanViewTest;
   bool ICanViewScore;
   time_t TimeUTC;
   char *ClassDat;

   /***** Make database query *****/
   NumExams =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get test exams of a user",
			      "SELECT TstCod,"			// row[0]
			             "AllowTeachers,"		// row[1]
			             "UNIX_TIMESTAMP(TstTime),"	// row[2]
			             "NumQsts,"			// row[3]
			             "NumQstsNotBlank,"		// row[4]
			             "Score"			// row[5]
			      " FROM tst_exams"
			      " WHERE CrsCod=%ld AND UsrCod=%ld"
			      " AND TstTime>=FROM_UNIXTIME(%ld)"
			      " AND TstTime<=FROM_UNIXTIME(%ld)"
			      " ORDER BY TstCod",
			      Gbl.Hierarchy.Crs.CrsCod,
			      UsrDat->UsrCod,
			      (long) Gbl.DateRange.TimeUTC[Dat_START_TIME],
			      (long) Gbl.DateRange.TimeUTC[Dat_END_TIME  ]);

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);
   Usr_ShowTableCellWithUsrData (UsrDat,NumExams);

   /***** Get and print test results *****/
   if (NumExams)
     {
      for (NumTest = 0;
           NumTest < NumExams;
           NumTest++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get test code (row[0]) */
	 if ((TstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of test result.");

	 /* Get if teachers are allowed to see this test result (row[1]) */
	 AllowTeachers = (row[1][0] == 'Y');
	 ClassDat = AllowTeachers ? "DAT" :
	                            "DAT_LIGHT";

	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_STD:
	       ICanViewTest  = ItsMe;
	       ICanViewScore = ItsMe &&
		               TsV_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	    case Rol_DEG_ADM:
	    case Rol_CTR_ADM:
	    case Rol_INS_ADM:
	       ICanViewTest  =
	       ICanViewScore = ItsMe ||
	                       AllowTeachers;
	       break;
	    case Rol_SYS_ADM:
	       ICanViewTest  =
	       ICanViewScore = true;
	       break;
	    default:
	       ICanViewTest  =
	       ICanViewScore = false;
               break;
	   }

         if (NumTest)
            HTM_TR_Begin (NULL);

         /* Write date and time (row[2] holds UTC date-time) */
         TimeUTC = Dat_GetUNIXTimeFromStr (row[2]);
         UniqueId++;
	 if (asprintf (&Id,"tst_date_%u",UniqueId) < 0)
	    Lay_NotEnoughMemoryExit ();
	 HTM_TD_Begin ("id=\"%s\" class=\"%s RT COLOR%u\"",
		       Id,ClassDat,Gbl.RowEvenOdd);
	 Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				       true,true,false,0x7);
	 HTM_TD_End ();
         free (Id);

         /* Get number of questions (row[3]) */
         if (sscanf (row[3],"%u",&NumQstsInThisTest) != 1)
            NumQstsInThisTest = 0;
	 if (AllowTeachers)
	    NumTotalQsts += NumQstsInThisTest;

         /* Get number of questions not blank (row[4]) */
         if (sscanf (row[4],"%u",&NumQstsNotBlankInThisTest) != 1)
            NumQstsNotBlankInThisTest = 0;
	 if (AllowTeachers)
	    NumTotalQstsNotBlank += NumQstsNotBlankInThisTest;

         /* Get score (row[5]) */
	 Str_SetDecimalPointToUS ();		// To get the decimal point as a dot
         if (sscanf (row[5],"%lf",&ScoreInThisTest) != 1)
            ScoreInThisTest = 0.0;
         Str_SetDecimalPointToLocal ();	// Return to local system
	 if (AllowTeachers)
	    TotalScoreOfAllTests += ScoreInThisTest;

         /* Write number of questions */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewTest)
	    HTM_Unsigned (NumQstsInThisTest);
	 HTM_TD_End ();

         /* Write number of questions not blank */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewTest)
	    HTM_Unsigned (NumQstsNotBlankInThisTest);
	 HTM_TD_End ();

	 /* Write score */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Double2Decimals (ScoreInThisTest);
	 HTM_TD_End ();

         /* Write average score per question */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewScore)
	    HTM_Double2Decimals (NumQstsInThisTest ? ScoreInThisTest /
		                            (double) NumQstsInThisTest :
			                    0.0);
	 HTM_TD_End ();

         /* Write grade */
	 HTM_TD_Begin ("class=\"%s RT COLOR%u\"",ClassDat,Gbl.RowEvenOdd);
	 if (ICanViewScore)
            Tst_ComputeAndShowGrade (NumQstsInThisTest,ScoreInThisTest,TsR_SCORE_MAX);
	 HTM_TD_End ();

	 /* Link to show this result */
	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewTest)
	   {
	    Frm_StartForm (Gbl.Action.Act == ActSeeMyTstRes ? ActSeeOneTstResMe :
						              ActSeeOneTstResOth);
	    TsR_PutParamTstCod (TstCod);
	    Ico_PutIconLink ("tasks.svg",Txt_View_test);
	    Frm_EndForm ();
	   }
	 HTM_TD_End ();
	 HTM_TR_End ();

	 if (AllowTeachers)
            NumExamsVisibleByTchs++;
        }

      /***** Write totals for this user *****/
      TsR_ShowTestResultsSummaryRow (ItsMe,NumExamsVisibleByTchs,
                                     NumTotalQsts,NumTotalQstsNotBlank,
                                     TotalScoreOfAllTests);
     }
   else
     {
      HTM_TD_ColouredEmpty (7);
      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/******************** Write parameter with code of test **********************/
/*****************************************************************************/

static void TsR_PutParamTstCod (long TstCod)
  {
   Par_PutHiddenParamLong (NULL,"TstCod",TstCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of test ***********************/
/*****************************************************************************/

static long TsR_GetParamTstCod (void)
  {
   /***** Get code of test *****/
   return Par_GetParToLong ("TstCod");
  }

/*****************************************************************************/
/**************** Show row with summary of user's test results ***************/
/*****************************************************************************/

static void TsR_ShowTestResultsSummaryRow (bool ItsMe,
                                           unsigned NumExams,
                                           unsigned NumTotalQsts,
                                           unsigned NumTotalQstsNotBlank,
                                           double TotalScoreOfAllTests)
  {
   extern const char *Txt_Visible_tests;
   bool ICanViewTotalScore;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewTotalScore = ItsMe &&
		              TsV_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 ICanViewTotalScore = ItsMe ||
			      NumExams;
	 break;
      case Rol_SYS_ADM:
	 ICanViewTotalScore = true;
	 break;
      default:
	 ICanViewTotalScore = false;
	 break;
     }

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Row title *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtColonNBSP (Txt_Visible_tests);
   HTM_Unsigned (NumExams);
   HTM_TD_End ();

   /***** Write total number of questions *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (NumExams)
      HTM_Unsigned (NumTotalQsts);
   HTM_TD_End ();

   /***** Write total number of questions not blank *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (NumExams)
      HTM_Unsigned (NumTotalQstsNotBlank);
   HTM_TD_End ();

   /***** Write total score *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      HTM_Double2Decimals (TotalScoreOfAllTests);
   HTM_TD_End ();

   /***** Write average score per question *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      HTM_Double2Decimals (NumTotalQsts ? TotalScoreOfAllTests / (double) NumTotalQsts :
			         0.0);
   HTM_TD_End ();

   /***** Write score over Tst_SCORE_MAX *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   if (ICanViewTotalScore)
      Tst_ComputeAndShowGrade (NumTotalQsts,TotalScoreOfAllTests,TsR_SCORE_MAX);
   HTM_TD_End ();

   /***** Last cell *****/
   HTM_TD_Begin ("class=\"DAT_N_LINE_TOP COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Show one test result of another user ********************/
/*****************************************************************************/

void TsR_ShowOneTstResult (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests_results;
   extern const char *Txt_Test_result;
   extern const char *Txt_The_user_does_not_exist;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Date;
   extern const char *Txt_Questions;
   extern const char *Txt_non_blank_QUESTIONS;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_Tags;
   long TstCod;
   bool AllowTeachers = false;	// Initialized to avoid warning
   time_t TstTimeUTC = 0;	// Test result UTC date-time, initialized to avoid warning
   unsigned NumQstsNotBlank;
   double TotalScore;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool ItsMe;
   bool ICanViewTest;
   bool ICanViewScore;

   /***** Get the code of the test *****/
   if ((TstCod = TsR_GetParamTstCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of test is missing.");

   /***** Get test result data *****/
   TsR_GetTestResultDataByTstCod (TstCod,
                                  &AllowTeachers,
                                  &TstTimeUTC,
				  &NumQstsNotBlank,
				  &TotalScore);
   TstCfg_SetConfigVisibility (TsV_MAX_VISIBILITY);

   /***** Check if I can view this test result *****/
   ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewTest = ItsMe;
	 if (ItsMe)
	   {
	    TstCfg_GetConfigFromDB ();	// To get feedback type
	    ICanViewScore = TsV_IsVisibleTotalScore (TstCfg_GetConfigVisibility ());
	   }
	 else
	    ICanViewScore = false;
	 break;
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 switch (Gbl.Action.Act)
	   {
	    case ActSeeOneTstResMe:
	       ICanViewTest  =
	       ICanViewScore = ItsMe;
	       break;
	    case ActSeeOneTstResOth:
	       ICanViewTest  =
	       ICanViewScore = ItsMe ||
			       AllowTeachers;
	       break;
	    default:
	       ICanViewTest  =
	       ICanViewScore = false;
	       break;
	   }
	 break;
      case Rol_SYS_ADM:
	 ICanViewTest  =
	 ICanViewScore = true;
	 break;
      default:
	 ICanViewTest  =
	 ICanViewScore = false;
	 break;
     }

   if (ICanViewTest)	// I am allowed to view this test result
     {
      /***** Get questions and user's answers of the test result from database *****/
      TsR_GetTestResultQuestionsFromDB (TstCod);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Txt_Test_result,NULL,
                    Hlp_ASSESSMENT_Tests_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

      /***** Header row *****/
      /* Get data of the user who made the test */
      if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 Lay_ShowErrorAndExit (Txt_The_user_does_not_exist);
      if (!Usr_CheckIfICanViewTst (&Gbl.Usrs.Other.UsrDat))
         Lay_NoPermissionExit ();

      /* User */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_ROLES_SINGUL_Abc[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role][Gbl.Usrs.Other.UsrDat.Sex]);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
      HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname1);
      if (Gbl.Usrs.Other.UsrDat.Surname2[0])
	 HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname2);
      if (Gbl.Usrs.Other.UsrDat.FirstName[0])
	 HTM_TxtF (", %s",Gbl.Usrs.Other.UsrDat.FirstName);
      HTM_BR ();
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Other.UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,ShowPhoto ? PhotoURL :
							   NULL,
			"PHOTO45x60",Pho_ZOOM,false);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Test date */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Date);
      HTM_TD_End ();

      HTM_TD_Begin ("id=\"test\" class=\"DAT LT\"");
      Dat_WriteLocalDateHMSFromUTC ("test",TstTimeUTC,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				    true,true,true,0x7);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Number of questions */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Questions);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      HTM_TxtF ("%u (%u %s)",
	        Gbl.Test.NumQsts,NumQstsNotBlank,Txt_non_blank_QUESTIONS);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Score */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Score);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
	 HTM_Double2Decimals (TotalScore);
      else
         Ico_PutIconNotVisible ();
      HTM_TD_End ();

      /* Grade */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Grade);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
         Tst_ComputeAndShowGrade (Gbl.Test.NumQsts,TotalScore,TsR_SCORE_MAX);
      else
         Ico_PutIconNotVisible ();
      HTM_TD_End ();

      HTM_TR_End ();

      /* Tags present in this test */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Tags);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      TsR_ShowTstTagsPresentInATestResult (TstCod);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write answers and solutions *****/
      TsR_ShowTestResult (&Gbl.Usrs.Other.UsrDat,
			  Gbl.Test.NumQsts,TstTimeUTC,
			  TstCfg_GetConfigVisibility ());

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write total mark of test *****/
      if (ICanViewScore)
	{
	 HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	 HTM_TxtColonNBSP (Txt_Score);
	 HTM_Double2Decimals (TotalScore);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Grade);
         Tst_ComputeAndShowGrade (Gbl.Test.NumQsts,TotalScore,TsR_SCORE_MAX);
	 HTM_DIV_End ();
	}

      /***** End box *****/
      Box_BoxEnd ();
     }
   else	// I am not allowed to view this test result
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/******************** Show test tags in this test result *********************/
/*****************************************************************************/

static void TsR_ShowTstTagsPresentInATestResult (long TstCod)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumTags;

   /***** Get all tags of questions in this test *****/
   NumTags = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get tags"
					" present in a test result",
			     "SELECT tst_tags.TagTxt"	// row[0]
			     " FROM"
			     " (SELECT DISTINCT(tst_question_tags.TagCod)"
			     " FROM tst_question_tags,tst_exam_questions"
			     " WHERE tst_exam_questions.TstCod=%ld"
			     " AND tst_exam_questions.QstCod=tst_question_tags.QstCod)"
			     " AS TagsCods,tst_tags"
			     " WHERE TagsCods.TagCod=tst_tags.TagCod"
			     " ORDER BY tst_tags.TagTxt",
			     TstCod);
   Tst_ShowTagList (NumTags,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Show the result of a test *************************/
/*****************************************************************************/

void TsR_ShowTestResult (struct UsrData *UsrDat,
			 unsigned NumQsts,time_t TstTimeUTC,
			 unsigned Visibility)
  {
   extern const char *Txt_Question_modified;
   extern const char *Txt_Question_removed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   double ScoreThisQst;
   bool AnswerIsNotBlank;
   bool ThisQuestionHasBeenEdited;
   time_t EditTimeUTC;

   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Query database *****/
      if (Tst_GetOneQuestionByCod (Gbl.Test.QstCodes[NumQst],&mysql_res))	// Question exists
	{
	 /***** Get row of the result of the query *****/
	 row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] UNIX_TIMESTAMP(EditTime)
	 row[1] AnsType
	 row[2] Shuffle
	 row[3] Stem
	 row[4] Feedback
	 row[5] MedCod
	 row[6] NumHits
	 row[7] NumHitsNotBlank
	 row[8] Score
	 */

	 /***** If this question has been edited later than test time
	        ==> don't show question ****/
	 EditTimeUTC = Dat_GetUNIXTimeFromStr (row[0]);
	 ThisQuestionHasBeenEdited = false;
	 if (EditTimeUTC > TstTimeUTC)
	    ThisQuestionHasBeenEdited = true;

	 if (ThisQuestionHasBeenEdited)
	   {
	    /***** Question has been edited *****/
	    HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"BIG_INDEX RT COLOR%u\"",Gbl.RowEvenOdd);
	    HTM_Unsigned (NumQst + 1);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"DAT_LIGHT LT COLOR%u\"",Gbl.RowEvenOdd);
	    HTM_Txt (Txt_Question_modified);
	    HTM_TD_End ();

	    HTM_TR_End ();
	   }
	 else
	    /***** Write questions and answers *****/
	    Tst_WriteQstAndAnsTest (Tst_SHOW_TEST_RESULT,
	                            UsrDat,
				    NumQst,Gbl.Test.QstCodes[NumQst],
				    row,
				    Visibility,
				    &ScoreThisQst,	// Not used here
				    &AnswerIsNotBlank);	// Not used here
	}
      else
	{
	 /***** Question does not exists *****/
         HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"BIG_INDEX RT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Unsigned (NumQst + 1);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT_LIGHT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Txt_Question_removed);
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/********* Get data of a test result using its test result code **************/
/*****************************************************************************/

static void TsR_GetTestResultDataByTstCod (long TstCod,
                                           bool *AllowTeachers,
                                           time_t *TstTimeUTC,
                                           unsigned *NumQstsNotBlank,
                                           double *Score)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Make database query *****/
   if (DB_QuerySELECT (&mysql_res,"can not get data"
				  " of a test result of a user",
		       "SELECT UsrCod,"				// row[0]
		              "AllowTeachers,"			// row[1]
			      "UNIX_TIMESTAMP(TstTime),"	// row[2]
		              "NumQsts,"			// row[3]
		              "NumQstsNotBlank,"		// row[4]
		              "Score"				// row[5]
		       " FROM tst_exams"
		       " WHERE TstCod=%ld AND CrsCod=%ld",
		       TstCod,
		       Gbl.Hierarchy.Crs.CrsCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user code (row[0]) */
      Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get if teachers are allowed to see this test result (row[1]) */
      *AllowTeachers = (row[1][0] == 'Y');

      /* Get date-time (row[2] holds UTC date-time) */
      *TstTimeUTC = Dat_GetUNIXTimeFromStr (row[2]);

      /* Get number of questions (row[3]) */
      if (sscanf (row[3],"%u",&Gbl.Test.NumQsts) != 1)
	 Gbl.Test.NumQsts = 0;

      /* Get number of questions not blank (row[4]) */
      if (sscanf (row[4],"%u",NumQstsNotBlank) != 1)
	 *NumQstsNotBlank = 0;

      /* Get score (row[5]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[5],"%lf",Score) != 1)
	 *Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Store user's answers of an test result into database ***********/
/*****************************************************************************/

void TsR_StoreOneTestResultQstInDB (long TstCod,long QstCod,unsigned NumQst,double Score)
  {
   char Indexes[Tst_MAX_BYTES_INDEXES_ONE_QST + 1];
   char Answers[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];

   /***** Replace each separator of multiple parameters by a comma *****/
   /* In database commas are used as separators instead of special chars */
   Par_ReplaceSeparatorMultipleByComma (Gbl.Test.StrIndexesOneQst[NumQst],Indexes);
   Par_ReplaceSeparatorMultipleByComma (Gbl.Test.StrAnswersOneQst[NumQst],Answers);

   /***** Insert question and user's answers into database *****/
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   DB_QueryINSERT ("can not insert a question of a test result",
		   "INSERT INTO tst_exam_questions"
		   " (TstCod,QstCod,QstInd,Score,Indexes,Answers)"
		   " VALUES"
		   " (%ld,%ld,%u,'%.15lg','%s','%s')",
		   TstCod,QstCod,
		   NumQst,	// 0, 1, 2, 3...
		   Score,
		   Indexes,
		   Answers);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************ Get the questions of a test result from database ***************/
/*****************************************************************************/

static void TsR_GetTestResultQuestionsFromDB (long TstCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;

   /***** Get questions of a test result from database *****/
   Gbl.Test.NumQsts =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get questions"
					 " of a test result",
			      "SELECT QstCod,"	// row[0]
			             "Indexes,"	// row[1]
			             "Answers"	// row[2]
			      " FROM tst_exam_questions"
			      " WHERE TstCod=%ld ORDER BY QstInd",
			      TstCod);

   /***** Get questions codes *****/
   for (NumQst = 0;
	NumQst < Gbl.Test.NumQsts;
	NumQst++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get question code */
      if ((Gbl.Test.QstCodes[NumQst] = Str_ConvertStrCodToLongCod (row[0])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Get indexes for this question (row[1]) */
      Str_Copy (Gbl.Test.StrIndexesOneQst[NumQst],row[1],
                Tst_MAX_BYTES_INDEXES_ONE_QST);

      /* Get answers selected by user for this question (row[2]) */
      Str_Copy (Gbl.Test.StrAnswersOneQst[NumQst],row[2],
                Tst_MAX_BYTES_ANSWERS_ONE_QST);

      /* Replace each comma by a separator of multiple parameters */
      /* In database commas are used as separators instead of special chars */
      Par_ReplaceCommaBySeparatorMultiple (Gbl.Test.StrIndexesOneQst[NumQst]);
      Par_ReplaceCommaBySeparatorMultiple (Gbl.Test.StrAnswersOneQst[NumQst]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Remove test results made by a user ********************/
/*****************************************************************************/

void TsR_RemoveTestResultsMadeByUsrInAllCrss (long UsrCod)
  {
   /***** Remove test results made by the specified user *****/
   DB_QueryDELETE ("can not remove test results made by a user",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,tst_exam_questions"
                   " WHERE tst_exams.UsrCod=%ld"
                   " AND tst_exams.TstCod=tst_exam_questions.TstCod",
		   UsrCod);

   DB_QueryDELETE ("can not remove test results made by a user",
		   "DELETE FROM tst_exams"
	           " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************** Remove test results made by a user in a course ***************/
/*****************************************************************************/

void TsR_RemoveTestResultsMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove test results made by the specified user *****/
   DB_QueryDELETE ("can not remove test results made by a user in a course",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,tst_exam_questions"
                   " WHERE tst_exams.CrsCod=%ld AND tst_exams.UsrCod=%ld"
                   " AND tst_exams.TstCod=tst_exam_questions.TstCod",
		   CrsCod,UsrCod);

   DB_QueryDELETE ("can not remove test results made by a user in a course",
		   "DELETE FROM tst_exams"
	           " WHERE CrsCod=%ld AND UsrCod=%ld",
		   CrsCod,UsrCod);
  }

/*****************************************************************************/
/****************** Remove all test results made in a course *****************/
/*****************************************************************************/

void TsR_RemoveCrsTestResults (long CrsCod)
  {
   /***** Remove questions of test results made in the course *****/
   DB_QueryDELETE ("can not remove test results made in a course",
		   "DELETE FROM tst_exam_questions"
	           " USING tst_exams,tst_exam_questions"
                   " WHERE tst_exams.CrsCod=%ld"
                   " AND tst_exams.TstCod=tst_exam_questions.TstCod",
		   CrsCod);

   /***** Remove test results made in the course *****/
   DB_QueryDELETE ("can not remove test results made in a course",
		   "DELETE FROM tst_exams WHERE CrsCod=%ld",
		   CrsCod);
  }
