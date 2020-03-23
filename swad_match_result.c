// swad_match_result.c: matches results in games using remote control

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
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_match.h"
#include "swad_match_result.h"
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

static void McR_ListMyMchResultsInCrs (void);
static void McR_ListMyMchResultsInGam (long GamCod);
static void McR_ListMyMchResultsInMch (long MchCod);
static void McR_ShowAllMchResultsInSelectedGames (void);
static void McR_ListAllMchResultsInSelectedGames (void);
static void McR_ListAllMchResultsInGam (long GamCod);
static void McR_ListAllMchResultsInMch (long MchCod);

static void McR_ShowResultsBegin (const char *Title,bool ListGamesToSelect);
static void McR_ShowResultsEnd (void);

static void McR_ListGamesToSelect (void);
static void McR_ShowHeaderMchResults (Usr_MeOrOther_t MeOrOther);

static void McR_BuildGamesSelectedCommas (char **GamesSelectedCommas);
static void McR_ShowMchResults (Usr_MeOrOther_t MeOrOther,
				long MchCod,	// <= 0 ==> any
				long GamCod,	// <= 0 ==> any
				const char *GamesSelectedCommas);
static void McR_ShowMchResultsSummaryRow (unsigned NumResults,
                                          unsigned NumTotalQsts,
                                          unsigned NumTotalQstsNotBlank,
                                          double TotalScoreOfAllResults,
					  double TotalGrade);
static void McR_GetMatchResultDataByMchCod (long MchCod,long UsrCod,
					    time_t TimeUTC[Dat_NUM_START_END_TIME],
                                            unsigned *NumQsts,
					    unsigned *NumQstsNotBlank,
					    double *Score);

static bool McR_CheckIfICanSeeMatchResult (struct Match *Match,long UsrCod);
static bool McR_CheckIfICanViewScore (bool ICanViewResult,unsigned Visibility);

/*****************************************************************************/
/*********** Select users and dates to show their matches results ************/
/*****************************************************************************/

void McR_SelUsrsToViewMchResults (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_matches_results;

   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeAllMchResCrs,NULL,
				     Txt_Results,
                                     Hlp_ASSESSMENT_Games_results,
                                     Txt_View_matches_results,
				     false);	// Do not put form with date range
  }

/*****************************************************************************/
/*************************** Show my matches results *************************/
/*****************************************************************************/

void McR_ShowMyMchResultsInCrs (void)
  {
   extern const char *Txt_Results;

   /***** Get list of games *****/
   Gam_GetListGames (Gam_ORDER_BY_TITLE);
   Gam_GetListSelectedGamCods ();

   /***** List my matches results in the current course *****/
   McR_ShowResultsBegin (Txt_Results,true);	// List games to select
   McR_ListMyMchResultsInCrs ();
   McR_ShowResultsEnd ();

   /***** Free list of games *****/
   free (Gbl.Games.GamCodsSelected);
   Gam_FreeListGames ();
  }

static void McR_ListMyMchResultsInCrs (void)
  {
   char *GamesSelectedCommas = NULL;	// Initialized to avoid warning

   /***** Table header *****/
   McR_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results in the current course *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   McR_BuildGamesSelectedCommas (&GamesSelectedCommas);
   McR_ShowMchResults (Usr_ME,-1L,-1L,GamesSelectedCommas);
   free (GamesSelectedCommas);
  }

/*****************************************************************************/
/***************** Show my matches results in a given game *******************/
/*****************************************************************************/

void McR_ShowMyMchResultsInGam (void)
  {
   extern const char *Txt_Results_of_game_X;
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List my matches results in game *****/
   McR_ShowResultsBegin (Str_BuildStringStr (Txt_Results_of_game_X,Game.Title),
			 false);	// Do not list games to select
   Str_FreeString ();
   McR_ListMyMchResultsInGam (Game.GamCod);
   McR_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void McR_ListMyMchResultsInGam (long GamCod)
  {
   /***** Table header *****/
   McR_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results in game *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   McR_ShowMchResults (Usr_ME,-1L,GamCod,NULL);
  }

/*****************************************************************************/
/***************** Show my matches results in a given match ******************/
/*****************************************************************************/

void McR_ShowMyMchResultsInMch (void)
  {
   extern const char *Txt_Results_of_match_X;
   struct Game Game;
   struct Match Match;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   if ((Match.MchCod = Mch_GetParamMchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");
   Gam_GetDataOfGameByCod (&Game);
   Mch_GetDataOfMatchByCod (&Match);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List my matches results in match *****/
   McR_ShowResultsBegin (Str_BuildStringStr (Txt_Results_of_match_X,Match.Title),
			 false);	// Do not list games to select
   Str_FreeString ();
   McR_ListMyMchResultsInMch (Match.MchCod);
   McR_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void McR_ListMyMchResultsInMch (long MchCod)
  {
   /***** Table header *****/
   McR_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results in game *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   McR_ShowMchResults (Usr_ME,MchCod,-1L,NULL);
  }

/*****************************************************************************/
/****************** Get users and show their matches results *****************/
/*****************************************************************************/

void McR_ShowAllMchResultsInCrs (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  McR_ShowAllMchResultsInSelectedGames,
                                  McR_SelUsrsToViewMchResults);
  }

/*****************************************************************************/
/****************** Show matches results for several users *******************/
/*****************************************************************************/

static void McR_ShowAllMchResultsInSelectedGames (void)
  {
   extern const char *Txt_Results;

   /***** Get list of games *****/
   Gam_GetListGames (Gam_ORDER_BY_TITLE);
   Gam_GetListSelectedGamCods ();

   /***** List the matches results of the selected users *****/
   McR_ShowResultsBegin (Txt_Results,true);	// List games to select
   McR_ListAllMchResultsInSelectedGames ();
   McR_ShowResultsEnd ();

   /***** Free list of games *****/
   free (Gbl.Games.GamCodsSelected);
   Gam_FreeListGames ();
  }

static void McR_ListAllMchResultsInSelectedGames (void)
  {
   char *GamesSelectedCommas = NULL;	// Initialized to avoid warning
   const char *Ptr;

   /***** Table head *****/
   McR_ShowHeaderMchResults (Usr_OTHER);

   /***** List the matches results of the selected users *****/
   McR_BuildGamesSelectedCommas (&GamesSelectedCommas);
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 if (Usr_CheckIfICanViewMch (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show matches results *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    McR_ShowMchResults (Usr_OTHER,-1L,-1L,GamesSelectedCommas);
	   }
     }
   free (GamesSelectedCommas);
  }

/*****************************************************************************/
/*** Show matches results of a game for the users who answered in that game **/
/*****************************************************************************/

void McR_ShowAllMchResultsInGam (void)
  {
   extern const char *Txt_Results_of_game_X;
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List matches results in game *****/
   McR_ShowResultsBegin (Str_BuildStringStr (Txt_Results_of_game_X,Game.Title),
			 false);	// Do not list games to select
   Str_FreeString ();
   McR_ListAllMchResultsInGam (Game.GamCod);
   McR_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void McR_ListAllMchResultsInGam (long GamCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   McR_ShowHeaderMchResults (Usr_OTHER);

   /***** Get all users who have answered any match question in this game *****/
   NumUsrs = DB_QuerySELECT (&mysql_res,"can not get users in game",
			     "SELECT users.UsrCod FROM"
			     " (SELECT DISTINCT mch_results.UsrCod AS UsrCod"	// row[0]
			     " FROM mch_results,mch_matches,gam_games"
			     " WHERE mch_matches.GamCod=%ld"
			     " AND mch_matches.MchCod=mch_results.MchCod"
			     " AND mch_matches.GamCod=gam_games.GamCod"
			     " AND gam_games.CrsCod=%ld)"			// Extra check
			     " AS users,usr_data"
			     " WHERE users.UsrCod=usr_data.UsrCod"
			     " ORDER BY usr_data.Surname1,"
			               "usr_data.Surname2,"
			               "usr_data.FirstName",
			     GamCod,
			     Gbl.Hierarchy.Crs.CrsCod);
   if (NumUsrs)
     {
      /***** List matches results for each user *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get match code (row[0]) */
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	       if (Usr_CheckIfICanViewMch (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show matches results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  McR_ShowMchResults (Usr_OTHER,-1L,GamCod,NULL);
		 }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/** Show matches results of a match for the users who answered in that match */
/*****************************************************************************/

void McR_ShowAllMchResultsInMch (void)
  {
   extern const char *Txt_Results_of_match_X;
   struct Game Game;
   struct Match Match;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   if ((Match.MchCod = Mch_GetParamMchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");
   Gam_GetDataOfGameByCod (&Game);
   Mch_GetDataOfMatchByCod (&Match);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List matches results in match *****/
   McR_ShowResultsBegin (Str_BuildStringStr (Txt_Results_of_match_X,Match.Title),
			 false);	// Do not list games to select
   Str_FreeString ();
   McR_ListAllMchResultsInMch (Match.MchCod);
   McR_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void McR_ListAllMchResultsInMch (long MchCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   McR_ShowHeaderMchResults (Usr_OTHER);

   /***** Get all users who have answered any match question in this game *****/
   NumUsrs = DB_QuerySELECT (&mysql_res,"can not get users in match",
			     "SELECT users.UsrCod FROM"
			     " (SELECT mch_results.UsrCod AS UsrCod"	// row[0]
			     " FROM mch_results,mch_matches,gam_games"
			     " WHERE mch_results.MchCod=%ld"
			     " AND mch_results.MchCod=mch_matches.MchCod"
			     " AND mch_matches.GamCod=gam_games.GamCod"
			     " AND gam_games.CrsCod=%ld)"		// Extra check
			     " AS users,usr_data"
			     " WHERE users.UsrCod=usr_data.UsrCod"
			     " ORDER BY usr_data.Surname1,"
			               "usr_data.Surname2,"
			               "usr_data.FirstName",
			     MchCod,
			     Gbl.Hierarchy.Crs.CrsCod);
   if (NumUsrs)
     {
      /***** List matches results for each user *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get match code (row[0]) */
	 if ((Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	       if (Usr_CheckIfICanViewMch (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show matches results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  McR_ShowMchResults (Usr_OTHER,MchCod,-1L,NULL);
		 }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show results (begin / end) *************************/
/*****************************************************************************/

static void McR_ShowResultsBegin (const char *Title,bool ListGamesToSelect)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;

   /***** Begin box *****/
   HTM_SECTION_Begin (McR_RESULTS_BOX_ID);
   Box_BoxBegin ("100%",Title,NULL,
		 Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);

   /***** List games to select *****/
   if (ListGamesToSelect)
      McR_ListGamesToSelect ();

   /***** Begin match results table *****/
   HTM_SECTION_Begin (McR_RESULTS_TABLE_ID);
   HTM_TABLE_BeginWidePadding (2);
  }

static void McR_ShowResultsEnd (void)
  {
   /***** End match results table *****/
   HTM_TABLE_End ();
   HTM_SECTION_End ();

   /***** End box *****/
   Box_BoxEnd ();
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/********** Write list of those attendance events that have students *********/
/*****************************************************************************/

static void McR_ListGamesToSelect (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
   extern const char *Txt_Games;
   extern const char *Txt_Game;
   extern const char *Txt_Update_results;
   unsigned UniqueId;
   unsigned NumGame;
   struct Game Game;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Games,NULL,Hlp_ASSESSMENT_Games_results,Box_CLOSABLE);

   /***** Begin form to update the results
	  depending on the games selected *****/
   Frm_StartFormAnchor (Gbl.Action.Act,McR_RESULTS_TABLE_ID);
   Grp_PutParamsCodGrps ();
   Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Heading row *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,2,NULL,NULL);
   HTM_TH (1,1,"LM",Txt_Game);

   HTM_TR_End ();

   /***** List the events *****/
   for (NumGame = 0, UniqueId = 1, Gbl.RowEvenOdd = 0;
	NumGame < Gbl.Games.Num;
	NumGame++, UniqueId++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      /* Get data of this game */
      Game.GamCod = Gbl.Games.Lst[NumGame].GamCod;
      Gam_GetDataOfGameByCod (&Game);

      /* Write a row for this event */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_INPUT_CHECKBOX ("GamCod",HTM_DONT_SUBMIT_ON_CHANGE,
			  "id=\"Gam%u\" value=\"%ld\"%s",
			  NumGame,Gbl.Games.Lst[NumGame].GamCod,
			  Gbl.Games.Lst[NumGame].Selected ? " checked=\"checked\"" :
				                            "");
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_LABEL_Begin ("for=\"Gam%u\"",NumGame);
      HTM_TxtF ("%u:",NumGame + 1);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_Txt (Game.Title);
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
/********************* Show header of my matches results *********************/
/*****************************************************************************/

static void McR_ShowHeaderMchResults (Usr_MeOrOther_t MeOrOther)
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
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Dat_END_TIME]);
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
/******* Build string with list of selected games separated by commas ********/
/******* from list of selected games                                  ********/
/*****************************************************************************/

static void McR_BuildGamesSelectedCommas (char **GamesSelectedCommas)
  {
   size_t MaxLength;
   unsigned NumGame;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Allocate memory for subquery of games selected *****/
   MaxLength = (size_t) Gbl.Games.NumSelected * (Cns_MAX_DECIMAL_DIGITS_LONG + 1);
   if ((*GamesSelectedCommas = (char *) malloc (MaxLength + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Build subquery with list of selected games *****/
   (*GamesSelectedCommas)[0] = '\0';
   for (NumGame = 0;
	NumGame < Gbl.Games.Num;
	NumGame++)
      if (Gbl.Games.Lst[NumGame].Selected)
	{
	 sprintf (LongStr,"%ld",Gbl.Games.Lst[NumGame].GamCod);
	 if ((*GamesSelectedCommas)[0])
	    Str_Concat (*GamesSelectedCommas,",",MaxLength);
	 Str_Concat (*GamesSelectedCommas,LongStr,MaxLength);
	}
  }

/*****************************************************************************/
/********* Show the matches results of a user in the current course **********/
/*****************************************************************************/

static void McR_ShowMchResults (Usr_MeOrOther_t MeOrOther,
				long MchCod,	// <= 0 ==> any
				long GamCod,	// <= 0 ==> any
				const char *GamesSelectedCommas)
  {
   extern const char *Txt_Match_result;
   char *MchSubQuery;
   char *GamSubQuery;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData *UsrDat;
   bool ICanViewResult;
   bool ICanViewScore;
   unsigned NumResults;
   unsigned NumResult;
   static unsigned UniqueId = 0;
   char *Id;
   struct Match Match;
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

   /***** Build matches subquery *****/
   if (MchCod > 0)
     {
      if (asprintf (&MchSubQuery," AND mch_results.MchCod=%ld",MchCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&MchSubQuery,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /***** Build games subquery *****/
   if (GamCod > 0)
     {
      if (asprintf (&GamSubQuery," AND mch_matches.GamCod=%ld",GamCod) < 0)
	 Lay_NotEnoughMemoryExit ();
     }
   else if (GamesSelectedCommas)
     {
      if (GamesSelectedCommas[0])
	{
	 if (asprintf (&GamSubQuery," AND mch_matches.GamCod IN (%s)",
		       GamesSelectedCommas) < 0)
	    Lay_NotEnoughMemoryExit ();
	}
      else
	{
	 if (asprintf (&GamSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	}
     }
   else
     {
      if (asprintf (&GamSubQuery,"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /***** Make database query *****/
   NumResults =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get matches results",
			      "SELECT mch_results.MchCod,"			// row[0]
				     "UNIX_TIMESTAMP(mch_results.StartTime),"	// row[1]
				     "UNIX_TIMESTAMP(mch_results.EndTime),"	// row[2]
				     "mch_results.NumQsts,"			// row[3]
				     "mch_results.NumQstsNotBlank,"		// row[4]
				     "mch_results.Score,"			// row[5]
				     "gam_games.MaxGrade,"			// row[6]
				     "gam_games.Visibility"			// row[7]
			      " FROM mch_results,mch_matches,gam_games"
			      " WHERE mch_results.UsrCod=%ld"
			      "%s"	// Match subquery
			      " AND mch_results.MchCod=mch_matches.MchCod"
			      "%s"	// Games subquery
			      " AND mch_matches.GamCod=gam_games.GamCod"
			      " AND gam_games.CrsCod=%ld"			// Extra check
			      " ORDER BY mch_matches.Title",
			      UsrDat->UsrCod,
			      MchSubQuery,
			      GamSubQuery,
			      Gbl.Hierarchy.Crs.CrsCod);
   free (GamSubQuery);
   free (MchSubQuery);

   /***** Show user's data *****/
   HTM_TR_Begin (NULL);
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

	 /* Get visibility (row[7]) */
	 Visibility = TsV_GetVisibilityFromStr (row[7]);

	 /* Show match result? */
	 ICanViewResult = McR_CheckIfICanSeeMatchResult (&Match,UsrDat->UsrCod);
         ICanViewScore  = McR_CheckIfICanViewScore (ICanViewResult,Visibility);

	 if (NumResult)
	    HTM_TR_Begin (NULL);

	 /* Write start/end times (row[1], row[2] hold UTC start/end times) */
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[1 + StartEndTime]);
	    UniqueId++;
	    if (asprintf (&Id,"mch_res_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	       Lay_NotEnoughMemoryExit ();
	    HTM_TD_Begin ("id =\"%s\" class=\"DAT LT COLOR%u\"",
			  Id,Gbl.RowEvenOdd);
	    Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC[StartEndTime],
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					  true,true,false,0x7);
	    HTM_TD_End ();
	    free (Id);
	   }

	 /* Write match title */
	 HTM_TD_Begin ("class=\"DAT LT COLOR%u\"",Gbl.RowEvenOdd);
	 HTM_Txt (Match.Title);
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
            Grade = Tst_ComputeGrade (NumQstsInThisResult,ScoreInThisResult,MaxGrade);
	    Tst_ShowGrade (Grade,MaxGrade);
	    TotalGrade += Grade;
	   }
	 else
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Link to show this result */
	 HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
	 if (ICanViewResult)
	   {
	    Gam_SetCurrentGamCod (Match.GamCod);	// Used to pass parameter
	    Mch_SetCurrentMchCod (Match.MchCod);	// Used to pass parameter
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
            Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write totals for this user *****/
      McR_ShowMchResultsSummaryRow (NumResults,
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
/************** Show row with summary of user's matches results **************/
/*****************************************************************************/

static void McR_ShowMchResultsSummaryRow (unsigned NumResults,
                                          unsigned NumTotalQsts,
                                          unsigned NumTotalQstsNotBlank,
                                          double TotalScoreOfAllResults,
					  double TotalGrade)
  {
   extern const char *Txt_Matches;

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Row title *****/
   HTM_TD_Begin ("colspan=\"3\" class=\"DAT_N_LINE_TOP RM COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtColonNBSP (Txt_Matches);
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
/******************* Show one match result of another user *******************/
/*****************************************************************************/

void McR_ShowOneMchResult (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_The_user_does_not_exist;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_non_blank_QUESTIONS;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_Tags;
   struct Game Game;
   struct Match Match;
   Usr_MeOrOther_t MeOrOther;
   struct UsrData *UsrDat;
   time_t TimeUTC[Dat_NUM_START_END_TIME];	// Match result UTC date-time
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   unsigned NumQsts;
   unsigned NumQstsNotBlank;
   struct Tst_UsrAnswers UsrAnswers;
   double TotalScore;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
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

   /***** Check if I can view this match result *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewResult = McR_CheckIfICanSeeMatchResult (&Match,UsrDat->UsrCod);
	 if (ICanViewResult)
	    ICanViewScore = TsV_IsVisibleTotalScore (Game.Visibility);
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

   if (ICanViewResult)	// I am allowed to view this match result
     {
      /***** Get questions and user's answers of the match result from database *****/
      McR_GetMatchResultQuestionsFromDB (Match.MchCod,UsrDat->UsrCod,
					 &NumQsts,&NumQstsNotBlank,
					 &UsrAnswers);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Match.Title,NULL,
                    Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

      /***** Header row *****/
      /* Get data of the user who answer the match */
      if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,Usr_DONT_GET_PREFS))
	 Lay_ShowErrorAndExit (Txt_The_user_does_not_exist);
      if (!Usr_CheckIfICanViewTst (UsrDat))
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

      /* Start/end time (for user in this match) */
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
	 Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC[StartEndTime],
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
      HTM_TxtF ("%u (%u %s)",NumQsts,NumQstsNotBlank,Txt_non_blank_QUESTIONS);
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

      HTM_TR_End ();

      /* Grade */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT_N RT\"");
      HTM_TxtF ("%s:",Txt_Grade);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LT\"");
      if (ICanViewScore)
         Tst_ComputeAndShowGrade (NumQsts,TotalScore,Game.MaxGrade);
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
      Gam_ShowTstTagsPresentInAGame (Match.GamCod);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write answers and solutions *****/
      TsR_ShowTestResult (UsrDat,NumQsts,&UsrAnswers,TimeUTC[Dat_START_TIME],
			  Game.Visibility);

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write total mark of match result *****/
      if (ICanViewScore)
	{
	 HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	 HTM_TxtColonNBSP (Txt_Score);
	 HTM_Double2Decimals (TotalScore);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Grade);
         Tst_ComputeAndShowGrade (NumQsts,TotalScore,Game.MaxGrade);
         HTM_DIV_End ();
	}

      /***** End box *****/
      Box_BoxEnd ();
     }
   else	// I am not allowed to view this match result
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/************ Get the questions of a match result from database **************/
/*****************************************************************************/

void McR_GetMatchResultQuestionsFromDB (long MchCod,long UsrCod,
					unsigned *NumQsts,unsigned *NumQstsNotBlank,
				        struct Tst_UsrAnswers *UsrAnswers)
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
      if ((UsrAnswers->QstCodes[NumQst] = Str_ConvertStrCodToLongCod (row[0])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Get question index (row[1]) */
      if ((LongNum = Str_ConvertStrCodToLongCod (row[1])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");
      QstInd = (unsigned) LongNum;

      /* Get indexes for this question (row[2]) */
      Str_Copy (UsrAnswers->StrIndexesOneQst[NumQst],row[2],
                Tst_MAX_BYTES_INDEXES_ONE_QST);

      /* Get answers selected by user for this question */
      Mch_GetQstAnsFromDB (MchCod,UsrCod,QstInd,&UsrAnswer);
      if (UsrAnswer.AnsInd >= 0)	// UsrAnswer.AnsInd >= 0 ==> answer selected
	{
         snprintf (UsrAnswers->StrAnswersOneQst[NumQst],Tst_MAX_BYTES_ANSWERS_ONE_QST + 1,
		   "%d",UsrAnswer.AnsInd);
         (*NumQstsNotBlank)++;
        }
      else				// UsrAnswer.AnsInd < 0 ==> no answer selected
	 UsrAnswers->StrAnswersOneQst[NumQst][0] = '\0';	// Empty answer

      /* Replace each comma by a separator of multiple parameters */
      /* In database commas are used as separators instead of special chars */
      Par_ReplaceCommaBySeparatorMultiple (UsrAnswers->StrIndexesOneQst[NumQst]);
      Par_ReplaceCommaBySeparatorMultiple (UsrAnswers->StrAnswersOneQst[NumQst]);
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
/********************** Get if I can see match result ************************/
/*****************************************************************************/

static bool McR_CheckIfICanSeeMatchResult (struct Match *Match,long UsrCod)
  {
   bool ItsMe;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ItsMe = Usr_ItsMe (UsrCod);
	 if (ItsMe && Match->Status.ShowUsrResults)
	    return Mch_CheckIfICanPlayThisMatchBasedOnGrps (Match);
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
/********************** Get if I can see match result ************************/
/*****************************************************************************/

static bool McR_CheckIfICanViewScore (bool ICanViewResult,unsigned Visibility)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 if (ICanViewResult)
	    return TsV_IsVisibleTotalScore (Visibility);
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

