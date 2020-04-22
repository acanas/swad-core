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

static void MchRes_PutFormToSelUsrsToViewMchResults (void *Games);

static void MchRes_ListMyMchResultsInCrs (struct Gam_Games *Games);
static void MchRes_ListMyMchResultsInGam (struct Gam_Games *Games,long GamCod);
static void MchRes_ListMyMchResultsInMch (struct Gam_Games *Games,long MchCod);
static void MchRes_ShowAllMchResultsInSelectedGames (void *Games);
static void MchRes_ListAllMchResultsInSelectedGames (struct Gam_Games *Games);
static void MchRes_ListAllMchResultsInGam (struct Gam_Games *Games,long GamCod);
static void MchRes_ListAllMchResultsInMch (struct Gam_Games *Games,long MchCod);

static void MchRes_ShowResultsBegin (struct Gam_Games *Games,
                                     const char *Title,bool ListGamesToSelect);
static void MchRes_ShowResultsEnd (void);

static void MchRes_ListGamesToSelect (struct Gam_Games *Games);
static void MchRes_ShowHeaderMchResults (Usr_MeOrOther_t MeOrOther);

static void MchRes_BuildGamesSelectedCommas (struct Gam_Games *Games,
                                             char **GamesSelectedCommas);
static void MchRes_ShowMchResults (struct Gam_Games *Games,
                                   Usr_MeOrOther_t MeOrOther,
				   long MchCod,	// <= 0 ==> any
				   long GamCod,	// <= 0 ==> any
				   const char *GamesSelectedCommas);
static void MchRes_ShowMchResultsSummaryRow (unsigned NumResults,
                                             unsigned NumTotalQsts,
                                             unsigned NumTotalQstsNotBlank,
                                             double TotalScoreOfAllResults,
					     double TotalGrade);
static void MchRes_GetMatchResultDataByMchCod (long MchCod,long UsrCod,
                                               struct TstRes_Result *Result);

static bool MchRes_CheckIfICanSeeMatchResult (struct Mch_Match *Match,long UsrCod);
static bool MchRes_CheckIfICanViewScore (bool ICanViewResult,unsigned Visibility);

/*****************************************************************************/
/*************************** Show my matches results *************************/
/*****************************************************************************/

void MchRes_ShowMyMchResultsInCrs (void)
  {
   extern const char *Txt_Results;
   struct Gam_Games Games;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get list of games *****/
   Gam_GetListGames (&Games,Gam_ORDER_BY_TITLE);
   Gam_GetListSelectedGamCods (&Games);

   /***** List my matches results in the current course *****/
   MchRes_ShowResultsBegin (&Games,Txt_Results,true);	// List games to select
   MchRes_ListMyMchResultsInCrs (&Games);
   MchRes_ShowResultsEnd ();

   /***** Free list of games *****/
   free (Games.GamCodsSelected);
   Gam_FreeListGames (&Games);
  }

static void MchRes_ListMyMchResultsInCrs (struct Gam_Games *Games)
  {
   char *GamesSelectedCommas = NULL;	// Initialized to avoid warning

   /***** Table header *****/
   MchRes_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results in the current course *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   MchRes_BuildGamesSelectedCommas (Games,&GamesSelectedCommas);
   MchRes_ShowMchResults (Games,Usr_ME,-1L,-1L,GamesSelectedCommas);
   free (GamesSelectedCommas);
  }

/*****************************************************************************/
/***************** Show my matches results in a given game *******************/
/*****************************************************************************/

void MchRes_ShowMyMchResultsInGam (void)
  {
   extern const char *Txt_Results_of_game_X;
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List my matches results in game *****/
   MchRes_ShowResultsBegin (&Games,
                            Str_BuildStringStr (Txt_Results_of_game_X,Game.Title),
			    false);	// Do not list games to select
   Str_FreeString ();
   MchRes_ListMyMchResultsInGam (&Games,Game.GamCod);
   MchRes_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void MchRes_ListMyMchResultsInGam (struct Gam_Games *Games,long GamCod)
  {
   /***** Table header *****/
   MchRes_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results in game *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   MchRes_ShowMchResults (Games,Usr_ME,-1L,GamCod,NULL);
  }

/*****************************************************************************/
/***************** Show my matches results in a given match ******************/
/*****************************************************************************/

void MchRes_ShowMyMchResultsInMch (void)
  {
   extern const char *Txt_Results_of_match_X;
   struct Gam_Games Games;
   struct Gam_Game Game;
   struct Mch_Match Match;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   if ((Match.MchCod = Mch_GetParamMchCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of match is missing.");
   Gam_GetDataOfGameByCod (&Game);
   Mch_GetDataOfMatchByCod (&Match);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List my matches results in match *****/
   MchRes_ShowResultsBegin (&Games,Str_BuildStringStr (Txt_Results_of_match_X,Match.Title),
			    false);	// Do not list games to select
   Str_FreeString ();
   MchRes_ListMyMchResultsInMch (&Games,Match.MchCod);
   MchRes_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void MchRes_ListMyMchResultsInMch (struct Gam_Games *Games,long MchCod)
  {
   /***** Table header *****/
   MchRes_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results in game *****/
   TstCfg_GetConfigFromDB ();	// Get feedback type
   MchRes_ShowMchResults (Games,Usr_ME,MchCod,-1L,NULL);
  }

/*****************************************************************************/
/****************** Get users and show their matches results *****************/
/*****************************************************************************/

void MchRes_ShowAllMchResultsInCrs (void)
  {
   struct Gam_Games Games;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get users and show their matches results *****/
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  MchRes_ShowAllMchResultsInSelectedGames,&Games,
                                  MchRes_PutFormToSelUsrsToViewMchResults,&Games);
  }

/*****************************************************************************/
/****************** Show matches results for several users *******************/
/*****************************************************************************/

static void MchRes_ShowAllMchResultsInSelectedGames (void *Games)
  {
   extern const char *Txt_Results;

   if (!Games)
      return;

   /***** Get list of games *****/
   Gam_GetListGames ((struct Gam_Games *) Games,Gam_ORDER_BY_TITLE);
   Gam_GetListSelectedGamCods ((struct Gam_Games *) Games);

   /***** List the matches results of the selected users *****/
   MchRes_ShowResultsBegin ((struct Gam_Games *) Games,
                            Txt_Results,
                            true);	// List games to select
   MchRes_ListAllMchResultsInSelectedGames ((struct Gam_Games *) Games);
   MchRes_ShowResultsEnd ();

   /***** Free list of games *****/
   free (((struct Gam_Games *) Games)->GamCodsSelected);
   Gam_FreeListGames ((struct Gam_Games *) Games);
  }

static void MchRes_ListAllMchResultsInSelectedGames (struct Gam_Games *Games)
  {
   char *GamesSelectedCommas = NULL;	// Initialized to avoid warning
   const char *Ptr;

   /***** Table head *****/
   MchRes_ShowHeaderMchResults (Usr_OTHER);

   /***** List the matches results of the selected users *****/
   MchRes_BuildGamesSelectedCommas (Games,&GamesSelectedCommas);
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	 if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show matches results *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	    MchRes_ShowMchResults (Games,Usr_OTHER,-1L,-1L,GamesSelectedCommas);
	   }
     }
   free (GamesSelectedCommas);
  }

/*****************************************************************************/
/**************** Select users to show their matches results *****************/
/*****************************************************************************/

void MchRes_SelUsrsToViewMchResults (void)
  {
   struct Gam_Games Games;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Put form to select users *****/
   MchRes_PutFormToSelUsrsToViewMchResults (&Games);
  }

static void MchRes_PutFormToSelUsrsToViewMchResults (void *Games)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_matches_results;

   if (Games)	// Not used
      Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
					ActSeeAllMchResCrs,
					NULL,NULL,
					Txt_Results,
					Hlp_ASSESSMENT_Games_results,
					Txt_View_matches_results,
					false);	// Do not put form with date range
  }

/*****************************************************************************/
/*** Show matches results of a game for the users who answered in that game **/
/*****************************************************************************/

void MchRes_ShowAllMchResultsInGam (void)
  {
   extern const char *Txt_Results_of_game_X;
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List matches results in game *****/
   MchRes_ShowResultsBegin (&Games,
                            Str_BuildStringStr (Txt_Results_of_game_X,Game.Title),
			    false);	// Do not list games to select
   Str_FreeString ();
   MchRes_ListAllMchResultsInGam (&Games,Game.GamCod);
   MchRes_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void MchRes_ListAllMchResultsInGam (struct Gam_Games *Games,long GamCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   MchRes_ShowHeaderMchResults (Usr_OTHER);

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
	       if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show matches results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  MchRes_ShowMchResults (Games,Usr_OTHER,-1L,GamCod,NULL);
		 }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/** Show matches results of a match for the users who answered in that match */
/*****************************************************************************/

void MchRes_ShowAllMchResultsInMch (void)
  {
   extern const char *Txt_Results_of_match_X;
   struct Gam_Games Games;
   struct Gam_Game Game;
   struct Mch_Match Match;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   if ((Match.MchCod = Mch_GetParamMchCod ()) <= 0)
      Lay_ShowErrorAndExit ("Code of match is missing.");
   Gam_GetDataOfGameByCod (&Game);
   Mch_GetDataOfMatchByCod (&Match);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,&Game,
                             false,	// Do not list game questions
	                     false);	// Do not put form to start new match

   /***** List matches results in match *****/
   MchRes_ShowResultsBegin (&Games,
                            Str_BuildStringStr (Txt_Results_of_match_X,Match.Title),
			    false);	// Do not list games to select
   Str_FreeString ();
   MchRes_ListAllMchResultsInMch (&Games,Match.MchCod);
   MchRes_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void MchRes_ListAllMchResultsInMch (struct Gam_Games *Games,long MchCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumUsrs;
   unsigned long NumUsr;

   /***** Table head *****/
   MchRes_ShowHeaderMchResults (Usr_OTHER);

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
	       if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
		 {
		  /***** Show matches results *****/
		  Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
		  MchRes_ShowMchResults (Games,Usr_OTHER,MchCod,-1L,NULL);
		 }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show results (begin / end) *************************/
/*****************************************************************************/

static void MchRes_ShowResultsBegin (struct Gam_Games *Games,
                                     const char *Title,bool ListGamesToSelect)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;

   /***** Begin box *****/
   HTM_SECTION_Begin (MchRes_RESULTS_BOX_ID);
   Box_BoxBegin ("100%",Title,
                 NULL,NULL,
		 Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);

   /***** List games to select *****/
   if (ListGamesToSelect)
      MchRes_ListGamesToSelect (Games);

   /***** Begin match results table *****/
   HTM_SECTION_Begin (MchRes_RESULTS_TABLE_ID);
   HTM_TABLE_BeginWidePadding (2);
  }

static void MchRes_ShowResultsEnd (void)
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

static void MchRes_ListGamesToSelect (struct Gam_Games *Games)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
   extern const char *Txt_Games;
   extern const char *Txt_Game;
   extern const char *Txt_Update_results;
   unsigned UniqueId;
   unsigned NumGame;
   struct Gam_Game Game;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Games,
                 NULL,NULL,
                 Hlp_ASSESSMENT_Games_results,Box_CLOSABLE);

   /***** Begin form to update the results
	  depending on the games selected *****/
   Frm_StartFormAnchor (Gbl.Action.Act,MchRes_RESULTS_TABLE_ID);
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
	NumGame < Games->Num;
	NumGame++, UniqueId++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
     {
      /* Get data of this game */
      Game.GamCod = Games->Lst[NumGame].GamCod;
      Gam_GetDataOfGameByCod (&Game);

      /* Write a row for this event */
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);
      HTM_INPUT_CHECKBOX ("GamCod",HTM_DONT_SUBMIT_ON_CHANGE,
			  "id=\"Gam%u\" value=\"%ld\"%s",
			  NumGame,Games->Lst[NumGame].GamCod,
			  Games->Lst[NumGame].Selected ? " checked=\"checked\"" :
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

static void MchRes_ShowHeaderMchResults (Usr_MeOrOther_t MeOrOther)
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
/******* Build string with list of selected games separated by commas ********/
/******* from list of selected games                                  ********/
/*****************************************************************************/

static void MchRes_BuildGamesSelectedCommas (struct Gam_Games *Games,
                                             char **GamesSelectedCommas)
  {
   size_t MaxLength;
   unsigned NumGame;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Allocate memory for subquery of games selected *****/
   MaxLength = (size_t) Games->NumSelected * (Cns_MAX_DECIMAL_DIGITS_LONG + 1);
   if ((*GamesSelectedCommas = (char *) malloc (MaxLength + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Build subquery with list of selected games *****/
   (*GamesSelectedCommas)[0] = '\0';
   for (NumGame = 0;
	NumGame < Games->Num;
	NumGame++)
      if (Games->Lst[NumGame].Selected)
	{
	 sprintf (LongStr,"%ld",Games->Lst[NumGame].GamCod);
	 if ((*GamesSelectedCommas)[0])
	    Str_Concat (*GamesSelectedCommas,",",MaxLength);
	 Str_Concat (*GamesSelectedCommas,LongStr,MaxLength);
	}
  }

/*****************************************************************************/
/********* Show the matches results of a user in the current course **********/
/*****************************************************************************/

static void MchRes_ShowMchResults (struct Gam_Games *Games,
                                   Usr_MeOrOther_t MeOrOther,
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
   struct Mch_Match Match;
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
	 Visibility = TstVis_GetVisibilityFromStr (row[7]);

	 /* Show match result? */
	 ICanViewResult = MchRes_CheckIfICanSeeMatchResult (&Match,UsrDat->UsrCod);
         ICanViewScore  = MchRes_CheckIfICanViewScore (ICanViewResult,Visibility);

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
	    Games->GamCod = Match.GamCod;
	    Games->MchCod = Match.MchCod;
	    switch (MeOrOther)
	      {
	       case Usr_ME:
		  Frm_StartForm (ActSeeOneMchResMe);
		  Mch_PutParamsEdit (Games);
		  break;
	       case Usr_OTHER:
		  Frm_StartForm (ActSeeOneMchResOth);
		  Mch_PutParamsEdit (Games);
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
      MchRes_ShowMchResultsSummaryRow (NumResults,
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

static void MchRes_ShowMchResultsSummaryRow (unsigned NumResults,
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

void MchRes_ShowOneMchResult (void)
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
   struct Gam_Games Games;
   struct Gam_Game Game;
   struct Mch_Match Match;
   Usr_MeOrOther_t MeOrOther;
   struct UsrData *UsrDat;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct TstRes_Result Result;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   bool ICanViewResult;
   bool ICanViewScore;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get and check parameters *****/
   Mch_GetAndCheckParameters (&Games,&Game,&Match);

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
   TstRes_ResetResult (&Result);
   MchRes_GetMatchResultDataByMchCod (Match.MchCod,UsrDat->UsrCod,&Result);

   /***** Check if I can view this match result *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 ICanViewResult = MchRes_CheckIfICanSeeMatchResult (&Match,UsrDat->UsrCod);
	 if (ICanViewResult)
	    ICanViewScore = TstVis_IsVisibleTotalScore (Game.Visibility);
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
      MchRes_GetMatchResultQuestionsFromDB (Match.MchCod,UsrDat->UsrCod,
					    &Result);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Match.Title,
                    NULL,NULL,
                    Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (false,false,
				 Gbl.Hierarchy.Ins.InsCod,
				 Gbl.Hierarchy.Deg.DegCod,
				 Gbl.Hierarchy.Crs.CrsCod);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (5);

      /***** Header row *****/
      /* Get data of the user who answer the match */
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
	 Dat_WriteLocalDateHMSFromUTC (Id,Result.TimeUTC[StartEndTime],
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
                                     Game.MaxGrade);
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
      TstRes_ShowExamAnswers (UsrDat,&Result,Game.Visibility);

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Write total mark of match result *****/
      if (ICanViewScore)
	{
	 HTM_DIV_Begin ("class=\"DAT_N_BOLD CM\"");
	 HTM_TxtColonNBSP (Txt_Score);
	 HTM_Double2Decimals (Result.Score);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Grade);
         TstRes_ComputeAndShowGrade (Result.NumQsts,Result.Score,
                                     Game.MaxGrade);
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

void MchRes_GetMatchResultQuestionsFromDB (long MchCod,long UsrCod,
				           struct TstRes_Result *Result)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   long LongNum;
   unsigned QstInd;
   struct Mch_UsrAnswer UsrAnswer;

   /***** Get questions and answers of a match result *****/
   Result->NumQsts = (unsigned)
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
      Mch_GetQstAnsFromDB (MchCod,UsrCod,QstInd,&UsrAnswer);
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
/************* Get data of a match result using its match code ***************/
/*****************************************************************************/

static void MchRes_GetMatchResultDataByMchCod (long MchCod,long UsrCod,
                                               struct TstRes_Result *Result)
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
/********************** Get if I can see match result ************************/
/*****************************************************************************/

static bool MchRes_CheckIfICanSeeMatchResult (struct Mch_Match *Match,long UsrCod)
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

static bool MchRes_CheckIfICanViewScore (bool ICanViewResult,unsigned Visibility)
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

