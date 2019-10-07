// swad_match_result.c: matches results in games using remote control

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_match.h"
#include "swad_match_result.h"
#include "swad_table.h"
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

static void McR_ShowHeaderMchResults (Usr_MeOrOther_t MeOrOther);
static void McR_ShowMchResults (Usr_MeOrOther_t MeOrOther);
static void McR_ShowMchResultsSummaryRow (bool ShowSummaryResults,
                                          unsigned NumResults,
                                          unsigned NumTotalQsts,
                                          unsigned NumTotalQstsNotBlank,
                                          double TotalScoreOfAllResults);
static void McR_GetMatchResultDataByMchCod (long MchCod,long UsrCod,
					    time_t TimeUTC[Dat_NUM_START_END_TIME],
                                            unsigned *NumQsts,
					    unsigned *NumQstsNotBlank,
					    double *Score);

static bool McR_CheckIfICanSeeMatchResult (long MchCod,long UsrCod);
static bool McR_GetVisibilityMchResultFromDB (long MchCod);

/*****************************************************************************/
/****************** Write a form to go to result of matches ******************/
/*****************************************************************************/

void McR_PutFormToViewMchResults (Act_Action_t Action)
  {
   extern const char *Txt_Results;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Lay_PutContextualLinkIconText (Action,NULL,NULL,
				  "tasks.svg",
				  Txt_Results);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Select dates to show my matches results ******************/
/*****************************************************************************/

void McR_SelDatesToSeeMyMchResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_matches_results;

   /***** Start form *****/
   Frm_StartForm (ActSeeMyMchRes);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_Results,NULL,
                      Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE,2);
   Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (false);

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_View_matches_results);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** Show my matches results *************************/
/*****************************************************************************/

void McR_ShowMyMchResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_Results;

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_Results,NULL,
                      Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE,2);

   /***** Header of the table with the list of users *****/
   McR_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results *****/
   Tst_GetConfigTstFromDB ();	// To get feedback type
   McR_ShowMchResults (Usr_ME);

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/*********** Select users and dates to show their matches results ************/
/*****************************************************************************/

void McR_SelUsrsToViewUsrsMchResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Results;
   extern const char *Txt_Users;
   extern const char *Txt_View_matches_results;
   unsigned NumTotalUsrs;

   /***** Get and update type of list,
          number of columns in class photo
          and preference about viewing photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   /***** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get and order lists of users from this course *****/
   Usr_GetListUsrs (Hie_CRS,Rol_STD);
   Usr_GetListUsrs (Hie_CRS,Rol_NET);
   Usr_GetListUsrs (Hie_CRS,Rol_TCH);
   NumTotalUsrs = Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs +
	          Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +
	          Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Results,NULL,
                 Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);

   /***** Show form to select the groups *****/
   Grp_ShowFormToSelectSeveralGroups (NULL,
	                              Grp_MY_GROUPS);

   /***** Start section with user list *****/
   Lay_StartSection (Usr_USER_LIST_SECTION_ID);

   if (NumTotalUsrs)
     {
      if (Usr_GetIfShowBigList (NumTotalUsrs,NULL,NULL))
        {
	 /***** Form to select type of list used for select several users *****/
	 Usr_ShowFormsToSelectUsrListType (NULL);

         /***** Start form *****/
         Frm_StartForm (ActSeeUsrMchRes);
         Grp_PutParamsCodGrps ();

         /***** Put list of users to select some of them *****/
         Tbl_StartTableCenterPadding (2);
         Tbl_StartRow ();

         fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP\">"
			    "%s:",
                  The_ClassFormInBox[Gbl.Prefs.Theme],Txt_Users);
         Tbl_EndCell ();

	 fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"%s LEFT_TOP\">",
                  The_ClassFormInBox[Gbl.Prefs.Theme]);
         Tbl_StartTablePadding (2);
         Usr_ListUsersToSelect (Rol_TCH);
         Usr_ListUsersToSelect (Rol_NET);
         Usr_ListUsersToSelect (Rol_STD);
         Tbl_EndTable ();
         Tbl_EndCell ();

         Tbl_EndRow ();

         /***** Starting and ending dates in the search *****/
         Dat_PutFormStartEndClientLocalDateTimesWithYesterdayToday (false);

         Tbl_EndTable ();

         /***** Send button *****/
	 Btn_PutConfirmButton (Txt_View_matches_results);

         /***** End form *****/
         Frm_EndForm ();
        }
     }
   else	// NumTotalUsrs == 0
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_UNK);

   /***** End section with user list *****/
   Lay_EndSection ();

   /***** End box *****/
   Box_EndBox ();

   /***** Free memory for users' list *****/
   Usr_FreeUsrsList (Rol_TCH);
   Usr_FreeUsrsList (Rol_NET);
   Usr_FreeUsrsList (Rol_STD);

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedUsrsCods ();

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/****************** Show matches results for several users *******************/
/*****************************************************************************/

void McR_ShowUsrsMchResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_Results;
   extern const char *Txt_You_must_select_one_ore_more_users;
   const char *Ptr;

   /***** Get list of the selected users's IDs *****/
   Usr_GetListsSelectedUsrsCods ();

   /***** Get starting and ending dates *****/
   Dat_GetIniEndDatesFromForm ();

   /***** Check the number of users whose matches results will be shown *****/
   if (Usr_CountNumUsrsInListOfSelectedUsrs ())	// If some users are selected...
     {
      /***** Start box and table *****/
      Box_StartBoxTable (NULL,Txt_Results,NULL,
                         Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE,2);

      /***** Header of the table with the list of users *****/
      McR_ShowHeaderMchResults (Usr_OTHER);

      /***** List the matches results of the selected users *****/
      Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
      while (*Ptr)
	{
	 Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
	                                    Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	 Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))               // Get of the database the data of the user
	    if (Usr_CheckIfICanViewMch (&Gbl.Usrs.Other.UsrDat))
	       /***** Show matches results *****/
	       McR_ShowMchResults (Usr_OTHER);
	}

      /***** End table and box *****/
      Box_EndBoxTable ();
     }
   else	// If no users are selected...
     {
      // ...write warning alert
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_users);
      // ...and show again the form
      McR_SelUsrsToViewUsrsMchResults ();
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedUsrsCods ();
  }

/*****************************************************************************/
/********************* Show header of my matches results *********************/
/*****************************************************************************/

static void McR_ShowHeaderMchResults (Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_User[Usr_NUM_SEXS];
   extern const char *Txt_Match;
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Non_blank_BR_questions;
   extern const char *Txt_Total_BR_score;
   extern const char *Txt_Average_BR_score_BR_per_question_BR_from_0_to_1;
   extern const char *Txt_Score;
   extern const char *Txt_out_of_PART_OF_A_SCORE;

   Tbl_StartRow ();
   fprintf (Gbl.F.Out,"<th colspan=\"2\" class=\"CENTER_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"LEFT_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"LEFT_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"LEFT_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"RIGHT_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"RIGHT_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"RIGHT_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"RIGHT_TOP\">"
		      "%s"
		      "</th>"
		      "<th class=\"RIGHT_TOP\">"
		      "%s<br />%s<br />%u"
		      "</th>"
		      "<th></th>",
	    Txt_User[MeOrOther == Usr_ME ? Gbl.Usrs.Me.UsrDat.Sex :
		                           Usr_SEX_UNKNOWN],
	    Txt_START_END_TIME[Dat_START_TIME],
	    Txt_START_END_TIME[Dat_END_TIME],
	    Txt_Match,
	    Txt_Questions,
	    Txt_Non_blank_BR_questions,
	    Txt_Total_BR_score,
	    Txt_Average_BR_score_BR_per_question_BR_from_0_to_1,
	    Txt_Score,Txt_out_of_PART_OF_A_SCORE,Tst_SCORE_MAX);
   Tbl_EndRow ();
  }

/*****************************************************************************/
/********* Show the matches results of a user in the current course **********/
/*****************************************************************************/

static void McR_ShowMchResults (Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_Today;
   extern const char *Txt_Match_result;
   extern const char *Txt_Hidden_result;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData *UsrDat;
   bool ShowResultThisMatch;
   bool ShowSummaryResults = true;
   unsigned NumResults;
   unsigned NumResult;
   static unsigned UniqueId = 0;
   struct Match Match;
   Dat_StartEndTime_t StartEndTime;
   unsigned NumQstsInThisResult;
   unsigned NumQstsNotBlankInThisResult;
   unsigned NumTotalQsts = 0;
   unsigned NumTotalQstsNotBlank = 0;
   double ScoreInThisResult;
   double TotalScoreOfAllResults = 0.0;
   time_t TimeUTC[Dat_NUM_START_END_TIME];

   /***** Set user *****/
   UsrDat = (MeOrOther == Usr_ME) ? &Gbl.Usrs.Me.UsrDat :
	                            &Gbl.Usrs.Other.UsrDat;

   /***** Make database query *****/
   NumResults =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get matches results of a user",
			      "SELECT mch_results.MchCod,"			// row[0]
			             "UNIX_TIMESTAMP(mch_results.StartTime),"	// row[1]
			             "UNIX_TIMESTAMP(mch_results.EndTime),"	// row[2]
			             "mch_results.NumQsts,"			// row[3]
			             "mch_results.NumQstsNotBlank,"		// row[4]
			             "mch_results.Score"			// row[5]
			      " FROM mch_results,mch_matches,gam_games"
			      " WHERE mch_results.UsrCod=%ld"
			      " AND mch_results.MchCod=mch_matches.MchCod"
			      " AND mch_matches.GamCod=gam_games.GamCod"
			      " AND gam_games.CrsCod=%ld"			// Extra check
			      " AND mch_matches.EndTime>=FROM_UNIXTIME(%ld)"
			      " AND mch_matches.StartTime<=FROM_UNIXTIME(%ld)"
			      " ORDER BY MchCod",
			      UsrDat->UsrCod,
			      Gbl.Hierarchy.Crs.CrsCod,
			      (long) Gbl.DateRange.TimeUTC[Dat_START_TIME],
			      (long) Gbl.DateRange.TimeUTC[Dat_END_TIME]);

   /***** Show user's data *****/
   Tbl_StartRow ();
   Usr_ShowTableCellWithUsrData (UsrDat,NumResults);

   /***** Get and print matches results *****/
   if (NumResults)
     {
      for (NumResult = 0;
           NumResult < NumResults;
           NumResult++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get match code (row[0]) */
	 if ((Match.MchCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of match.");
         Mch_GetDataOfMatchByCod (&Match);

	 /* Show match result? */
	 ShowResultThisMatch = McR_CheckIfICanSeeMatchResult (Match.MchCod,UsrDat->UsrCod);
	 ShowSummaryResults = ShowSummaryResults && ShowResultThisMatch;

         if (NumResult)
            Tbl_StartRow ();

         /* Write start/end times (row[1], row[2] hold UTC start/end times) */
         for (StartEndTime = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
           {
	    TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[1 + StartEndTime]);
	    UniqueId++;
	    fprintf (Gbl.F.Out,"<td id =\"mch_time_%u_%u\""
		               " class=\"DAT LEFT_TOP COLOR%u\">"
			       "<script type=\"text/javascript\">"
			       "writeLocalDateHMSFromUTC('mch_time_%u_%u',"
			       "%ld,%u,'<br />','%s',true,false,0x7);"
			       "</script>",
		     (unsigned) StartEndTime,UniqueId,
		     Gbl.RowEvenOdd,
		     (unsigned) StartEndTime,UniqueId,
		     (long) TimeUTC[StartEndTime],
		     (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
	    Tbl_EndCell ();
           }

         /* Write match title */
	 fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP COLOR%u\">%s",
	          Gbl.RowEvenOdd,Match.Title);
	 Tbl_EndCell ();

         /* Get number of questions (row[3]) */
         if (sscanf (row[3],"%u",&NumQstsInThisResult) != 1)
            NumQstsInThisResult = 0;
	 NumTotalQsts += NumQstsInThisResult;

         /* Get number of questions not blank (row[4]) */
         if (sscanf (row[4],"%u",&NumQstsNotBlankInThisResult) != 1)
            NumQstsNotBlankInThisResult = 0;
	 NumTotalQstsNotBlank += NumQstsNotBlankInThisResult;

	 if (ShowResultThisMatch)
	   {
	    /* Get score (row[5]) */
	    Str_SetDecimalPointToUS ();		// To get the decimal point as a dot
	    if (sscanf (row[5],"%lf",&ScoreInThisResult) != 1)
	       ScoreInThisResult = 0.0;
	    Str_SetDecimalPointToLocal ();	// Return to local system
	    TotalScoreOfAllResults += ScoreInThisResult;
	   }

         /* Write number of questions */
	 fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">%u",
	          Gbl.RowEvenOdd,NumQstsInThisResult);
	 Tbl_EndCell ();

         /* Write number of questions not blank */
	 fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">%u",
	          Gbl.RowEvenOdd,NumQstsNotBlankInThisResult);
	 Tbl_EndCell ();

	 /* Write score */
	 fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">",
	          Gbl.RowEvenOdd);
	 if (ShowResultThisMatch)
	    fprintf (Gbl.F.Out,"%.2lf",
		     ScoreInThisResult);
	 Tbl_EndCell ();

         /* Write average score per question */
	 fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">",
	          Gbl.RowEvenOdd);
	 if (ShowResultThisMatch)
	    fprintf (Gbl.F.Out,"%.2lf",
		     NumQstsInThisResult ? ScoreInThisResult / (double) NumQstsInThisResult :
			                   0.0);
	 Tbl_EndCell ();

         /* Write score over Tst_SCORE_MAX */
	 fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">",
	          Gbl.RowEvenOdd);
	 if (ShowResultThisMatch)
	    fprintf (Gbl.F.Out,"%.2lf",
		     NumQstsInThisResult ? ScoreInThisResult * Tst_SCORE_MAX / (double) NumQstsInThisResult :
			                   0.0);
	 Tbl_EndCell ();

	 /* Link to show this result */
	 fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP COLOR%u\">",
		  Gbl.RowEvenOdd);
	 if (ShowResultThisMatch)
	   {
	    Gam_SetParamCurrentGamCod (Match.GamCod);	// Used to pass parameter
	    Mch_SetParamCurrentMchCod (Match.MchCod);	// Used to pass parameter
	    switch (MeOrOther)
	      {
	       case Usr_ME:
		  Frm_StartForm (ActSeeOneMchResMe);
		  Mch_PutParamsEdit ();
		  break;
	       case Usr_OTHER:
		  Frm_StartForm (ActSeeOneMchResOth);
		  Mch_PutParamsEdit ();
		  Usr_PutParamOtherUsrCodEncrypted ();
		  break;
	      }
	    Ico_PutIconLink ("tasks.svg",Txt_Match_result);
	    Frm_EndForm ();
	   }
	 else
	    Ico_PutIconOff ("eye-slash.svg",Txt_Hidden_result);
	 Tbl_EndCell ();

         Tbl_EndRow ();
        }

      /***** Write totals for this user *****/
      McR_ShowMchResultsSummaryRow (ShowSummaryResults,
				    NumResults,
                                    NumTotalQsts,NumTotalQstsNotBlank,
                                    TotalScoreOfAllResults);
     }
   else
     {
      Tbl_PutEmptyColouredCells (8);
      Tbl_EndRow ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/************** Show row with summary of user's matches results **************/
/*****************************************************************************/

static void McR_ShowMchResultsSummaryRow (bool ShowSummaryResults,
                                          unsigned NumResults,
                                          unsigned NumTotalQsts,
                                          unsigned NumTotalQstsNotBlank,
                                          double TotalScoreOfAllResults)
  {
   extern const char *Txt_Matches;

   /***** Start row *****/
   Tbl_StartRow ();

   /***** Row title *****/
   fprintf (Gbl.F.Out,"<td colspan=\"3\""
	              " class=\"DAT_N_LINE_TOP RIGHT_MIDDLE COLOR%u\">"
		      "%s: %u",
	    Gbl.RowEvenOdd,
	    Txt_Matches,NumResults);
   Tbl_EndCell ();

   /***** Write total number of questions *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE COLOR%u\">",
	    Gbl.RowEvenOdd);
   if (NumResults)
      fprintf (Gbl.F.Out,"%u",NumTotalQsts);
   Tbl_EndCell ();

   /***** Write total number of questions not blank *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE COLOR%u\">",
	    Gbl.RowEvenOdd);
   if (NumResults)
      fprintf (Gbl.F.Out,"%u",NumTotalQstsNotBlank);
   Tbl_EndCell ();

   /***** Write total score *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE COLOR%u\">",
	    Gbl.RowEvenOdd);
   if (ShowSummaryResults)
      fprintf (Gbl.F.Out,"%.2lf",TotalScoreOfAllResults);
   Tbl_EndCell ();

   /***** Write average score per question *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE COLOR%u\">",
	    Gbl.RowEvenOdd);
   if (ShowSummaryResults)
      fprintf (Gbl.F.Out,"%.2lf",
	       NumTotalQsts ? TotalScoreOfAllResults / (double) NumTotalQsts :
			      0.0);
   Tbl_EndCell ();

   /***** Write score over Tst_SCORE_MAX *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N_LINE_TOP RIGHT_MIDDLE COLOR%u\">",
	    Gbl.RowEvenOdd);
   if (ShowSummaryResults)
      fprintf (Gbl.F.Out,"%.2lf",
	       NumTotalQsts ? TotalScoreOfAllResults * Tst_SCORE_MAX /
			      (double) NumTotalQsts :
			      0.0);
   Tbl_EndCell ();

   /***** Last cell *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT_N_LINE_TOP COLOR%u\">",
	    Gbl.RowEvenOdd);
   Tbl_EndCell ();

   /***** End row *****/
   Tbl_EndRow ();
  }

/*****************************************************************************/
/******************* Show one match result of another user *******************/
/*****************************************************************************/

void McR_ShowOneMchResult (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_Match_result;
   extern const char *Txt_The_user_does_not_exist;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Today;
   extern const char *Txt_Questions;
   extern const char *Txt_non_blank_QUESTIONS;
   extern const char *Txt_Score;
   extern const char *Txt_out_of_PART_OF_A_SCORE;
   extern const char *Txt_Tags;
   struct Game Game;
   struct Match Match;
   Usr_MeOrOther_t MeOrOther;
   struct UsrData *UsrDat;
   time_t TimeUTC[Dat_NUM_START_END_TIME];	// Match result UTC date-time
   Dat_StartEndTime_t StartEndTime;
   unsigned NumQsts;
   unsigned NumQstsNotBlank;
   double TotalScore;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool ItsMe;
   bool ICanPlayThisMatchBasedOnGrps;
   bool ICanViewResult;
   bool ICanViewScore;

   /***** Get and check parameters *****/
   Mch_GetAndCheckParameters (&Game,&Match);

   /***** Pointer to user's data *****/
   MeOrOther = (Gbl.Action.Act == ActSeeOneMchResMe) ? Usr_ME :
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

   /***** Get match result data *****/
   McR_GetMatchResultDataByMchCod (Match.MchCod,UsrDat->UsrCod,
				   TimeUTC,
				   &NumQsts,
				   &NumQstsNotBlank,
				   &TotalScore);
   Gbl.Test.Config.Feedback = Tst_FEEDBACK_FULL_FEEDBACK;   // Initialize feedback to maximum

   /***** Check if I can view this match result *****/
   ItsMe = Usr_ItsMe (UsrDat->UsrCod);
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 switch (MeOrOther)
	   {
	    case Usr_ME:
	       ICanPlayThisMatchBasedOnGrps = Mch_CheckIfICanPlayThisMatchBasedOnGrps (Match.MchCod);
	       ICanViewResult = ItsMe && ICanPlayThisMatchBasedOnGrps &&
		                Match.Status.ShowUsrResults;

	       if (ICanViewResult)
		 {
		  Tst_GetConfigTstFromDB ();	// To get feedback type
		  ICanViewScore = Gbl.Test.Config.Feedback != Tst_FEEDBACK_NOTHING;
		 }
	       else
		  ICanViewScore  = false;
	       break;
	    default:
	       ICanViewResult =
	       ICanViewScore  = false;
	       break;
	   }
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 switch (MeOrOther)
	   {
	    case Usr_ME:
	       ICanViewResult =
	       ICanViewScore  = ItsMe;
	       break;
	    case Usr_OTHER:
	       ICanViewResult =
	       ICanViewScore  = true;
	       break;
	    default:
	       ICanViewResult =
	       ICanViewScore  = false;
	       break;
	   }
	 break;
      case Rol_SYS_ADM:
	 ICanViewResult =
	 ICanViewScore  = true;
	 break;
      default:
	 ICanViewResult =
	 ICanViewScore  = false;
	 break;
     }

   if (ICanViewResult)	// I am allowed to view this match result
     {
      /***** Get questions and user's answers of the match result from database *****/
      McR_GetMatchResultQuestionsFromDB (Match.MchCod,UsrDat->UsrCod,
					 &NumQsts,&NumQstsNotBlank);

      /***** Start box *****/
      Box_StartBox (NULL,Txt_Match_result,NULL,
                    Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Start table *****/
      Tbl_StartTableWideMarginPadding (10);

      /***** Header row *****/
      /* Get data of the user who answer the match */
      if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,Usr_DONT_GET_PREFS))
	 Lay_ShowErrorAndExit (Txt_The_user_does_not_exist);
      if (!Usr_CheckIfICanViewTst (UsrDat))
         Act_NoPermissionExit ();

      /* User */
      Tbl_StartRow ();

      fprintf (Gbl.F.Out,"<td class=\"DAT_N RIGHT_TOP\">"
			 "%s:",
	       Txt_ROLES_SINGUL_Abc[UsrDat->Roles.InCurrentCrs.Role][UsrDat->Sex]);
      Tbl_EndCell ();

      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">");
      ID_WriteUsrIDs (UsrDat,NULL);
      fprintf (Gbl.F.Out," %s",
	       UsrDat->Surname1);
      if (UsrDat->Surname2[0])
	 fprintf (Gbl.F.Out," %s",
		  UsrDat->Surname2);
      if (UsrDat->FirstName[0])
	 fprintf (Gbl.F.Out,", %s",
		  UsrDat->FirstName);
      fprintf (Gbl.F.Out,"<br />");
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					   NULL,
			"PHOTO45x60",Pho_ZOOM,false);
      Tbl_EndCell ();

      Tbl_EndRow ();

      /* Start/end time (for user in this match) */
      for (StartEndTime = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 Tbl_StartRow ();

	 fprintf (Gbl.F.Out,"<td class=\"DAT_N RIGHT_TOP\">"
			    "%s:",
		  Txt_START_END_TIME[StartEndTime]);
	 Tbl_EndCell ();

	 fprintf (Gbl.F.Out,"<td id=\"match_%u\" class=\"DAT LEFT_TOP\">"
			    "<script type=\"text/javascript\">"
			    "writeLocalDateHMSFromUTC('match_%u',%ld,"
			    "%u,',&nbsp;','%s',true,true,0x7);"
			    "</script>",
		  (unsigned) StartEndTime,
		  (unsigned) StartEndTime,
		  TimeUTC[StartEndTime],
		  (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
	 Tbl_EndCell ();

	 Tbl_EndRow ();
	}

      /* Number of questions */
      Tbl_StartRow ();

      fprintf (Gbl.F.Out,"<td class=\"DAT_N RIGHT_TOP\">"
			 "%s:",
	       Txt_Questions);
      Tbl_EndCell ();

      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">"
			 "%u (%u %s)",
	       NumQsts,NumQstsNotBlank,Txt_non_blank_QUESTIONS);
      Tbl_EndCell ();

      Tbl_EndRow ();

      /* Score */
      Tbl_StartRow ();

      fprintf (Gbl.F.Out,"<td class=\"DAT_N RIGHT_TOP\">"
			 "%s:",
	       Txt_Score);
      Tbl_EndCell ();

      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">");
      if (ICanViewScore)
	 fprintf (Gbl.F.Out,"%.2lf (%.2lf",
		  TotalScore,
		  NumQsts ? TotalScore * Tst_SCORE_MAX / (double) NumQsts :
			    0.0);
      else
	 fprintf (Gbl.F.Out,"? (?");	// No feedback
      fprintf (Gbl.F.Out," %s %u)",
	       Txt_out_of_PART_OF_A_SCORE,Tst_SCORE_MAX);
      Tbl_EndCell ();

      Tbl_EndRow ();

      /* Tags present in this result */
      Tbl_StartRow ();

      fprintf (Gbl.F.Out,"<td class=\"DAT_N RIGHT_TOP\">"
			 "%s:",
	       Txt_Tags);
      Tbl_EndCell ();

      fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\">");
      Gam_ShowTstTagsPresentInAGame (Match.GamCod);
      Tbl_EndCell ();

      Tbl_EndRow ();

      /***** Write answers and solutions *****/
      Tst_ShowTestResult (UsrDat,NumQsts,TimeUTC[Dat_START_TIME]);

      /***** End table *****/
      Tbl_EndTable ();

      /***** Write total mark of match result *****/
      if (ICanViewScore)
         Tst_ShowTstTotalMark (NumQsts,TotalScore);

      /***** End box *****/
      Box_EndBox ();
     }
   else	// I am not allowed to view this match result
      Act_NoPermissionExit ();
  }

/*****************************************************************************/
/************ Get the questions of a match result from database **************/
/*****************************************************************************/

void McR_GetMatchResultQuestionsFromDB (long MchCod,long UsrCod,
					unsigned *NumQsts,unsigned *NumQstsNotBlank)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   long LongNum;
   unsigned QstInd;
   struct Mch_UsrAnswer UsrAnswer;

   /***** Get questions and answers of a match result *****/
   *NumQsts = (unsigned)
	      DB_QuerySELECT (&mysql_res,"can not get questions and answers"
		                         " of a match result",
			      "SELECT gam_questions.QstCod,"	// row[0]
				     "gam_questions.QstInd,"	// row[1]
				     "mch_indexes.Indexes"	// row[2]
			      " FROM mch_matches,gam_questions,mch_indexes"
			      " WHERE mch_matches.MchCod=%ld"
			      " AND mch_matches.GamCod=gam_questions.GamCod"
			      " AND mch_matches.MchCod=mch_indexes.MchCod"
			      " AND gam_questions.QstInd=mch_indexes.QstInd"
			      " ORDER BY gam_questions.QstInd",
			      MchCod);
   for (NumQst = 0, *NumQstsNotBlank = 0;
	NumQst < *NumQsts;
	NumQst++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get question code (row[0]) */
      if ((Gbl.Test.QstCodes[NumQst] = Str_ConvertStrCodToLongCod (row[0])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Get question index (row[1]) */
      if ((LongNum = Str_ConvertStrCodToLongCod (row[1])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");
      QstInd = (unsigned) LongNum;

      /* Get indexes for this question (row[2]) */
      Str_Copy (Gbl.Test.StrIndexesOneQst[NumQst],row[2],
                Tst_MAX_BYTES_INDEXES_ONE_QST);

      /* Get answers selected by user for this question */
      Mch_GetQstAnsFromDB (MchCod,UsrCod,QstInd,&UsrAnswer);
      if (UsrAnswer.AnsInd >= 0)	// UsrAnswer.AnsInd >= 0 ==> answer selected
	{
         snprintf (Gbl.Test.StrAnswersOneQst[NumQst],Tst_MAX_BYTES_ANSWERS_ONE_QST + 1,
		   "%d",UsrAnswer.AnsInd);
         (*NumQstsNotBlank)++;
        }
      else				// UsrAnswer.AnsInd < 0 ==> no answer selected
	 Gbl.Test.StrAnswersOneQst[NumQst][0] = '\0';	// Empty answer

      /* Replace each comma by a separator of multiple parameters */
      /* In database commas are used as separators instead of special chars */
      Par_ReplaceCommaBySeparatorMultiple (Gbl.Test.StrIndexesOneQst[NumQst]);
      Par_ReplaceCommaBySeparatorMultiple (Gbl.Test.StrAnswersOneQst[NumQst]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get data of a match result using its match code ***************/
/*****************************************************************************/

static void McR_GetMatchResultDataByMchCod (long MchCod,long UsrCod,
					    time_t TimeUTC[Dat_NUM_START_END_TIME],
                                            unsigned *NumQsts,
					    unsigned *NumQstsNotBlank,
					    double *Score)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;

   /***** Make database query *****/
   if (DB_QuerySELECT (&mysql_res,"can not get data"
				  " of a match result of a user",
		       "SELECT UNIX_TIMESTAMP(mch_results.StartTime),"		// row[1]
			      "UNIX_TIMESTAMP(mch_results.EndTime),"		// row[2]
		              "mch_results.NumQsts,"				// row[3]
		              "mch_results.NumQstsNotBlank,"			// row[4]
		              "mch_results.Score"				// row[5]
		       " FROM mch_results,mch_matches,gam_games"
		       " WHERE mch_results.MchCod=%ld"
		       " AND mch_results.UsrCod=%ld"
		       " AND mch_results.MchCod=mch_matches.MchCod"
		       " AND mch_matches.GamCod=gam_games.GamCod"
		       " AND gam_games.CrsCod=%ld",	// Extra check
		       MchCod,UsrCod,
		       Gbl.Hierarchy.Crs.CrsCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get start time (row[0] and row[1] hold UTC date-times) */
      for (StartEndTime = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
         TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[StartEndTime]);

      /* Get number of questions (row[2]) */
      if (sscanf (row[2],"%u",NumQsts) != 1)
	 *NumQsts = 0;

      /* Get number of questions not blank (row[3]) */
      if (sscanf (row[3],"%u",NumQstsNotBlank) != 1)
	 *NumQstsNotBlank = 0;

      /* Get score (row[4]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[4],"%lf",Score) != 1)
	 *Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Get if I can see match result  ************************/
/*****************************************************************************/

static bool McR_CheckIfICanSeeMatchResult (long MchCod,long UsrCod)
  {
   bool ItsMe;
   bool ShowResultThisMatch;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ItsMe = Usr_ItsMe (UsrCod);
	 if (ItsMe && Gbl.Test.Config.Feedback != Tst_FEEDBACK_NOTHING)
	    ShowResultThisMatch = McR_GetVisibilityMchResultFromDB (MchCod);
	 else
	    ShowResultThisMatch = false;
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 ShowResultThisMatch = true;
	 break;
      default:
	 ShowResultThisMatch = false;
	 break;
     }

   return ShowResultThisMatch;
  }

/*****************************************************************************/
/********************* Get visibility of match result ************************/
/*****************************************************************************/

static bool McR_GetVisibilityMchResultFromDB (long MchCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool ShowUsrResults;

   /***** Get visibility of match result from database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get if show result",
					"SELECT ShowUsrResults"		// row[0]
					" FROM mch_matches"
					" WHERE MchCod=%ld"
					" AND GamCod IN"		// Extra check
					" (SELECT GamCod FROM gam_games"
					" WHERE CrsCod='%ld')",
					MchCod,
					Gbl.Hierarchy.Crs.CrsCod);
   if (NumRows) // Match found...
     {
      /* Get whether to show user results or not (row(0)) */
      row = mysql_fetch_row (mysql_res);
      ShowUsrResults = (row[0][0] == 'Y');
     }
   else
      ShowUsrResults = false;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return ShowUsrResults;
  }
