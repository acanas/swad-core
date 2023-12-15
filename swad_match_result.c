// swad_match_result.c: matches results in games using remote control

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_match.h"
#include "swad_match_database.h"
#include "swad_match_result.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_test_visibility.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct MchRes_ICanView
  {
   bool Result;
   bool Score;
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void MchRes_PutFormToSelUsrsToViewMchResults (__attribute__((unused)) void *Args);

static void MchRes_ListMyMchResultsInCrs (struct Gam_Games *Games);
static void MchRes_ListMyMchResultsInGam (struct Gam_Games *Games);
static void MchRes_ListMyMchResultsInMch (struct Gam_Games *Games,long MchCod);
static void MchRes_ShowAllMchResultsInSelectedGames (void *Games);
static void MchRes_ListAllMchResultsInSelectedGames (struct Gam_Games *Games);
static void MchRes_ListAllMchResultsInGam (struct Gam_Games *Games);
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
                                             struct MchPrn_NumQuestions *NumTotalQsts,
                                             double TotalScore,
					     double TotalGrade);

static void MchRes_CheckIfICanViewMatchResult (const struct Gam_Game *Game,
                                               const struct Mch_Match *Match,
                                               long UsrCod,
                                               struct MchRes_ICanView *ICanView);

/*****************************************************************************/
/*************************** Show my matches results *************************/
/*****************************************************************************/

void MchRes_ShowMyMchResultsInCrs (void)
  {
   extern const char *Txt_Results;
   struct Gam_Games Games;

   /***** Reset games context *****/
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
   TstCfg_GetConfig ();	// Get feedback type
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
   char *Title;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,
                             false,	// Do not list game questions
			     Frm_DONT_PUT_FORM);

   /***** List my matches results in game *****/
   if (asprintf (&Title,Txt_Results_of_game_X,Games.Game.Title) < 0)
      Err_NotEnoughMemoryExit ();
   MchRes_ShowResultsBegin (&Games,Title,false);	// Do not list games to select
   free (Title);
      MchRes_ListMyMchResultsInGam (&Games);
   MchRes_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void MchRes_ListMyMchResultsInGam (struct Gam_Games *Games)
  {
   /***** Table header *****/
   MchRes_ShowHeaderMchResults (Usr_ME);

   /***** List my matches results in game *****/
   TstCfg_GetConfig ();	// Get feedback type
   MchRes_ShowMchResults (Games,Usr_ME,-1L,Games->Game.GamCod,NULL);
  }

/*****************************************************************************/
/***************** Show my matches results in a given match ******************/
/*****************************************************************************/

void MchRes_ShowMyMchResultsInMch (void)
  {
   extern const char *Txt_Results_of_match_X;
   struct Gam_Games Games;
   struct Mch_Match Match;
   char *Title;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Match.MchCod = ParCod_GetAndCheckPar (ParCod_Mch);
   Gam_GetGameDataByCod (&Games.Game);
   Mch_GetMatchDataByCod (&Match);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,
                             false,	// Do not list game questions
			     Frm_DONT_PUT_FORM);

   /***** List my matches results in match *****/
   if (asprintf (&Title,Txt_Results_of_match_X,Match.Title) < 0)
      Err_NotEnoughMemoryExit ();
   MchRes_ShowResultsBegin (&Games,Title,false);	// Do not list games to select
   free (Title);
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
   TstCfg_GetConfig ();	// Get feedback type
   MchRes_ShowMchResults (Games,Usr_ME,MchCod,-1L,NULL);
  }

/*****************************************************************************/
/****************** Get users and show their matches results *****************/
/*****************************************************************************/

void MchRes_ShowAllMchResultsInCrs (void)
  {
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Get users and show their matches results *****/
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  MchRes_ShowAllMchResultsInSelectedGames,&Games,
                                  MchRes_PutFormToSelUsrsToViewMchResults,NULL);
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
      Par_GetNextStrUntilSeparParMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_GET_ROLE_IN_CRS))
	 if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Show matches results *****/
	    Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
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
   MchRes_PutFormToSelUsrsToViewMchResults (NULL);
  }

static void MchRes_PutFormToSelUsrsToViewMchResults (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_ASSESSMENT_Games_results;
   extern const char *Txt_Results;
   extern const char *Txt_View_results;

   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeeUsrMchResCrs,
				     NULL,NULL,
				     Txt_Results,
				     Hlp_ASSESSMENT_Games_results,
				     Txt_View_results,
				     false);	// Do not put form with date range
  }

/*****************************************************************************/
/*** Show matches results of a game for the users who answered in that game **/
/*****************************************************************************/

void MchRes_ShowAllMchResultsInGam (void)
  {
   extern const char *Txt_Results_of_game_X;
   struct Gam_Games Games;
   char *Title;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,
                             false,	// Do not list game questions
			     Frm_DONT_PUT_FORM);

   /***** List matches results in game *****/
   if (asprintf (&Title,Txt_Results_of_game_X,Games.Game.Title) < 0)
      Err_NotEnoughMemoryExit ();
   MchRes_ShowResultsBegin (&Games,Title,false);	// Do not list games to select
   free (Title);
      MchRes_ListAllMchResultsInGam (&Games);
   MchRes_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void MchRes_ListAllMchResultsInGam (struct Gam_Games *Games)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;

   /***** Table head *****/
   MchRes_ShowHeaderMchResults (Usr_OTHER);

   /***** Get all users who have answered any match question in this game *****/
   NumUsrs = Mch_DB_GetUsrsWhoHavePlayedGam (&mysql_res,Games->Game.GamCod);

   /***** List matches results for each user *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs;
	NumUsr++)
      /* Get match code */
      if ((Gbl.Usrs.Other.UsrDat.UsrCod = DB_GetNextCode (mysql_res)) > 0)
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_GET_ROLE_IN_CRS))
	    if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	      {
	       /***** Show matches results *****/
	       Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	       MchRes_ShowMchResults (Games,Usr_OTHER,-1L,Games->Game.GamCod,NULL);
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
   struct Mch_Match Match;
   char *Title;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Match.MchCod = ParCod_GetAndCheckPar (ParCod_Mch);
   Gam_GetGameDataByCod (&Games.Game);
   Mch_GetMatchDataByCod (&Match);

   /***** Game begin *****/
   Gam_ShowOnlyOneGameBegin (&Games,
                             false,	// Do not list game questions
			     Frm_DONT_PUT_FORM);

   /***** List matches results in match *****/
   if (asprintf (&Title,Txt_Results_of_match_X,Match.Title) < 0)
      Err_NotEnoughMemoryExit ();
   MchRes_ShowResultsBegin (&Games,Title,false);	// Do not list games to select
   free (Title);
      MchRes_ListAllMchResultsInMch (&Games,Match.MchCod);
   MchRes_ShowResultsEnd ();

   /***** Game end *****/
   Gam_ShowOnlyOneGameEnd ();
  }

static void MchRes_ListAllMchResultsInMch (struct Gam_Games *Games,long MchCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;

   /***** Table head *****/
   MchRes_ShowHeaderMchResults (Usr_OTHER);

   /***** Get all users who have answered any match question in this game *****/
   NumUsrs = Mch_DB_GetUsrsWhoHavePlayedMch (&mysql_res,MchCod);

   /***** List matches results for each user *****/
   for (NumUsr = 0;
	NumUsr < NumUsrs;
	NumUsr++)
      /* Get match code (row[0]) */
      if ((Gbl.Usrs.Other.UsrDat.UsrCod = DB_GetNextCode (mysql_res)) > 0)
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_GET_ROLE_IN_CRS))
	    if (Usr_CheckIfICanViewTstExaMchResult (&Gbl.Usrs.Other.UsrDat))
	      {
	       /***** Show matches results *****/
	       Gbl.Usrs.Other.UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
	       MchRes_ShowMchResults (Games,Usr_OTHER,MchCod,-1L,NULL);
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
      Box_BoxBegin (NULL,Title,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);

	 /***** List games to select *****/
	 if (ListGamesToSelect)
	    MchRes_ListGamesToSelect (Games);

	 /***** Begin match results table *****/
	 HTM_SECTION_Begin (MchRes_RESULTS_TABLE_ID);
	    HTM_TABLE_BeginWidePadding (5);
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
   extern const char *Par_CodeStr[];
   extern const char *Txt_Games;
   extern const char *Txt_Game;
   extern const char *Txt_Update_results;
   unsigned UniqueId;
   unsigned NumGame;

   /***** Reset game *****/
   Gam_ResetGame (&Games->Game);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Games,
                 NULL,NULL,
                 NULL,Box_CLOSABLE);

      /***** Begin form to update the results
	     depending on the games selected *****/
      Frm_BeginFormAnchor (Gbl.Action.Act,MchRes_RESULTS_TABLE_ID);
	 Grp_PutParsCodGrps ();
	 Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);

	 /***** Begin table *****/
	 HTM_TABLE_BeginWidePadding (2);

	    /***** Heading row *****/
	    HTM_TR_Begin (NULL);
	       HTM_TH_Empty (2);
	       HTM_TH (Txt_Game,HTM_HEAD_LEFT);
	    HTM_TR_End ();

	    /***** List the events *****/
	    for (NumGame = 0, UniqueId = 1, The_ResetRowColor ();
		 NumGame < Games->Num;
		 NumGame++, UniqueId++, The_ChangeRowColor ())
	      {
	       /* Get data of this game */
	       Games->Game.GamCod = Games->Lst[NumGame].GamCod;
	       Gam_GetGameDataByCod (&Games->Game);

	       /* Write a row for this event */
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("class=\"CT DAT_%s %s\"",
		                The_GetSuffix (),
		                The_GetColorRows ());
		     HTM_INPUT_CHECKBOX (Par_CodeStr[ParCod_Gam],HTM_DONT_SUBMIT_ON_CHANGE,
					 "id=\"Gam%u\" value=\"%ld\"%s",
					 NumGame,Games->Lst[NumGame].GamCod,
					 Games->Lst[NumGame].Selected ? " checked=\"checked\"" :
									"");
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
		                The_GetSuffix (),
		                The_GetColorRows ());
		     HTM_LABEL_Begin ("for=\"Gam%u\"",NumGame);
			HTM_UnsignedColon (NumGame + 1);
		     HTM_LABEL_End ();
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
		                The_GetSuffix (),
		                The_GetColorRows ());
		     HTM_Txt (Games->Game.Title);
		  HTM_TD_End ();

	       HTM_TR_End ();
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Put button to refresh *****/
	 Lay_WriteLinkToUpdate (Txt_Update_results);

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
   extern const char *Txt_Answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_ANSWERS_non_blank;
   extern const char *Txt_ANSWERS_blank;
   extern const char *Txt_total;
   extern const char *Txt_average;

   /***** First row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_User[MeOrOther == Usr_ME ? Gbl.Usrs.Me.UsrDat.Sex :
			                          Usr_SEX_UNKNOWN],HTM_HEAD_CENTER,3,2,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_STR_TIME]               ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_START_END_TIME[Dat_END_TIME]               ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_Match                                      ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM");
      HTM_TH_Span (Txt_Questions                                  ,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span (Txt_Answers                                    ,HTM_HEAD_CENTER,1,2,"LINE_LEFT");
      HTM_TH_Span (Txt_Score                                      ,HTM_HEAD_CENTER,1,2,"LINE_LEFT");
      HTM_TH_Span (Txt_Grade                                      ,HTM_HEAD_RIGHT ,3,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span (NULL                                           ,HTM_HEAD_LEFT  ,3,1,"LINE_BOTTOM LINE_LEFT");
   HTM_TR_End ();

   /***** Second row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_ANSWERS_non_blank                          ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH      (Txt_ANSWERS_blank                              ,HTM_HEAD_RIGHT);
      HTM_TH_Span (Txt_total                                      ,HTM_HEAD_RIGHT ,1,1,"LINE_LEFT");
      HTM_TH      (Txt_average                                    ,HTM_HEAD_RIGHT);
   HTM_TR_End ();

   /***** Third row *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Span ("{-1&le;<em>p<sub>i</sub></em>&le;1}"          ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("{<em>p<sub>i</sub></em>=0}"                   ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
      HTM_TH_Span ("<em>&Sigma;p<sub>i</sub></em>"                ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM LINE_LEFT");
      HTM_TH_Span ("-1&le;"
	          "<em style=\"text-decoration:overline;\">p</em>"
	          "&le;1"                                         ,HTM_HEAD_RIGHT ,1,1,"LINE_BOTTOM");
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
   if ((*GamesSelectedCommas = malloc (MaxLength + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

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
   MYSQL_RES *mysql_res;
   struct Usr_Data *UsrDat;
   struct MchRes_ICanView ICanView;
   unsigned NumResults;
   unsigned NumResult;
   static unsigned UniqueId = 0;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct MchPrn_Print Print;
   unsigned NumQstsBlank;
   struct Mch_Match Match;
   double Grade;
   struct MchPrn_NumQuestions NumTotalQsts;
   double TotalScore;
   double TotalGrade;

   /***** Reset total number of questions and total score *****/
   NumTotalQsts.All      =
   NumTotalQsts.NotBlank = 0;
   TotalScore = 0.0;
   TotalGrade = 0.0;

   /***** Set user *****/
   UsrDat = (MeOrOther == Usr_ME) ? &Gbl.Usrs.Me.UsrDat :
				    &Gbl.Usrs.Other.UsrDat;

   /***** Make database query *****/
   NumResults = Mch_DB_GetUsrMchResults (&mysql_res,MeOrOther,MchCod,GamCod,GamesSelectedCommas);

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
	 /* Get match code */
	 MchPrn_ResetPrint (&Print);
	 if ((Print.MchCod = DB_GetNextCode (mysql_res)) < 0)
	    Err_WrongMatchExit ();

	 /* Get match result data */
	 Print.UsrCod = UsrDat->UsrCod;
	 MchPrn_GetMatchPrintDataByMchCodAndUsrCod (&Print);

	 /* Get data of match and game */
	 Match.MchCod = Print.MchCod;
	 Mch_GetMatchDataByCod (&Match);
	 Games->Game.GamCod = Match.GamCod;
	 Gam_GetGameDataByCod (&Games->Game);

	 /* Check if I can view this match result and score */
	 MchRes_CheckIfICanViewMatchResult (&Games->Game,&Match,UsrDat->UsrCod,&ICanView);

	 if (NumResult)
	    HTM_TR_Begin (NULL);

	 /* Write start/end times */
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    UniqueId++;
	    if (asprintf (&Id,"mch_res_time_%u_%u",
	                  (unsigned) StartEndTime,UniqueId) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_TD_Begin ("id=\"%s\" class=\"LT DAT_%s %s\"",
			  Id,The_GetSuffix (),
			  The_GetColorRows ());
	       Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
					     Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
					     true,true,false,0x7);
	    HTM_TD_End ();
	    free (Id);
	   }

	 /* Write match title */
	 HTM_TD_Begin ("class=\"LT DAT_%s %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
	    HTM_Txt (Match.Title);
	 HTM_TD_End ();

	 /* Accumulate questions and score */
	 if (ICanView.Score)
	   {
	    NumTotalQsts.All      += Print.NumQsts.All;
	    NumTotalQsts.NotBlank += Print.NumQsts.NotBlank;
	    TotalScore            += Print.Score;
	   }

	 /* Write number of questions */
	 HTM_TD_Begin ("class=\"RT DAT_%s LINE_LEFT %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
	    if (ICanView.Score)
	       HTM_Unsigned (Print.NumQsts.All);
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write number of non-blank answers */
	 HTM_TD_Begin ("class=\"RT DAT_%s LINE_LEFT %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
	    if (ICanView.Score)
	      {
	       if (Print.NumQsts.NotBlank)
		  HTM_Unsigned (Print.NumQsts.NotBlank);
	       else
		  HTM_Light0 ();
	      }
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write number of blank answers */
	 HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
	    NumQstsBlank = Print.NumQsts.All - Print.NumQsts.NotBlank;
	    if (ICanView.Score)
	      {
	       if (NumQstsBlank)
		  HTM_Unsigned (NumQstsBlank);
	       else
		  HTM_Light0 ();
	      }
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write score */
	 HTM_TD_Begin ("class=\"RT DAT_%s LINE_LEFT %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
	    if (ICanView.Score)
	      {
	       HTM_Double2Decimals (Print.Score);
	       HTM_Txt ("/");
	       HTM_Unsigned (Print.NumQsts.All);
	      }
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write average score per question */
	 HTM_TD_Begin ("class=\"RT DAT_%s %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
	    if (ICanView.Score)
	       HTM_Double2Decimals (Print.NumQsts.All ? Print.Score /
							(double) Print.NumQsts.All :
							0.0);
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Write grade over maximum grade */
	 HTM_TD_Begin ("class=\"RT DAT_%s LINE_LEFT %s\"",
	               The_GetSuffix (),
	               The_GetColorRows ());
	    if (ICanView.Score)
	      {
	       Grade = TstPrn_ComputeGrade (Print.NumQsts.All,Print.Score,Games->Game.MaxGrade);
	       TstPrn_ShowGrade (Grade,Games->Game.MaxGrade);
	       TotalGrade += Grade;
	      }
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 /* Link to show this result */
	 HTM_TD_Begin ("class=\"RT LINE_LEFT %s\"",
	               The_GetColorRows ());
	    if (ICanView.Result)
	      {
	       Games->Game.GamCod    = Match.GamCod;
	       Games->MchCod.Current = Match.MchCod;
	       switch (MeOrOther)
		 {
		  case Usr_ME:
		     Frm_BeginForm (ActSeeOneMchResMe);
			Mch_PutParsEdit (Games);
		        Ico_PutIconLink ("tasks.svg",Ico_BLACK,ActSeeOneMchResMe);
		     break;
		  case Usr_OTHER:
		     Frm_BeginForm (ActSeeOneMchResOth);
			Mch_PutParsEdit (Games);
			Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
		        Ico_PutIconLink ("tasks.svg",Ico_BLACK,ActSeeOneMchResOth);
		     break;
		 }
	       Frm_EndForm ();
	      }
	    else
	       Ico_PutIconNotVisible ();
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write totals for this user *****/
      MchRes_ShowMchResultsSummaryRow (NumResults,
				       &NumTotalQsts,
				       TotalScore,
				       TotalGrade);
     }
   else	// No results
     {
      /* Columns for dates and match */
      HTM_TD_Begin ("colspan=\"3\" class=\"LINE_BOTTOM %s\"",
                    The_GetColorRows ());
      HTM_TD_End ();

      /* Column for questions */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetColorRows ());
      HTM_TD_End ();

      /* Columns for answers */
      HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetColorRows ());
      HTM_TD_End ();

      /* Columns for score */
      HTM_TD_Begin ("colspan=\"2\" class=\"LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetColorRows ());
      HTM_TD_End ();

      /* Column for grade */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetColorRows ());
      HTM_TD_End ();

      /* Column for link to show the result */
      HTM_TD_Begin ("class=\"LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetColorRows ());
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   The_ChangeRowColor ();
  }

/*****************************************************************************/
/************** Show row with summary of user's matches results **************/
/*****************************************************************************/

static void MchRes_ShowMchResultsSummaryRow (unsigned NumResults,
                                             struct MchPrn_NumQuestions *NumTotalQsts,
                                             double TotalScore,
					     double TotalGrade)
  {
   extern const char *Txt_Matches;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Row title *****/
      HTM_TD_Begin ("colspan=\"3\" class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 HTM_TxtColonNBSP (Txt_Matches);
	 HTM_Unsigned (NumResults);
      HTM_TD_End ();

      /***** Write total number of questions *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (NumResults)
	    HTM_Unsigned (NumTotalQsts->All);
      HTM_TD_End ();

      /***** Write total number of non-blank answers *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (NumResults)
	    HTM_Unsigned (NumTotalQsts->NotBlank);
      HTM_TD_End ();

      /***** Write total number of blank answers *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 if (NumResults)
	    HTM_Unsigned (NumTotalQsts->All - NumTotalQsts->NotBlank);
      HTM_TD_End ();

      /***** Write total score *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 HTM_Double2Decimals (TotalScore);
	 HTM_Txt ("/");
	 HTM_Unsigned (NumTotalQsts->All);
      HTM_TD_End ();

      /***** Write average score per question *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 HTM_Double2Decimals (NumTotalQsts->All ? TotalScore /
						  (double) NumTotalQsts->All :
						  0.0);
      HTM_TD_End ();

      /***** Write total grade *****/
      HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
	 HTM_Double2Decimals (TotalGrade);
      HTM_TD_End ();

      /***** Last cell *****/
      HTM_TD_Begin ("class=\"DAT_STRONG_%s LINE_TOP LINE_BOTTOM LINE_LEFT %s\"",
                    The_GetSuffix (),
                    The_GetColorRows ());
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
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Questions;
   extern const char *Txt_Answers;
   extern const char *Txt_Score;
   extern const char *Txt_Grade;
   extern const char *Txt_Tags;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };
   struct Gam_Games Games;
   struct Mch_Match Match;
   Usr_MeOrOther_t MeOrOther;
   struct Usr_Data *UsrDat;
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   struct MchPrn_Print Print;
   struct MchRes_ICanView ICanView;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get and check parameters *****/
   Mch_GetAndCheckPars (&Games,&Match);

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
         Usr_GetParOtherUsrCodEncrypted (UsrDat);
	 break;
     }

   /***** Get match result data *****/
   Print.MchCod = Match.MchCod;
   Print.UsrCod = UsrDat->UsrCod;
   MchPrn_GetMatchPrintDataByMchCodAndUsrCod (&Print);

   /***** Check if I can view this match result and score *****/
   MchRes_CheckIfICanViewMatchResult (&Games.Game,&Match,UsrDat->UsrCod,&ICanView);
   if (!ICanView.Result)
      Err_NoPermissionExit ();

   /***** Get questions and user's answers of the match result from database *****/
   Mch_GetMatchQuestionsFromDB (&Print);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Match.Title,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Games_results,Box_NOT_CLOSABLE);
      Lay_WriteHeaderClassPhoto (Vie_VIEW);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (10);

	 /***** User *****/
	 /* Get data of the user who answer the match */
	 if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,
						       Usr_DONT_GET_PREFS,
						       Usr_GET_ROLE_IN_CRS))
	    Err_WrongUserExit ();
	 if (!Usr_CheckIfICanViewTstExaMchResult (UsrDat))
	    Err_NoPermissionExit ();

	 /* Get if user has accepted enrolment */
	 UsrDat->Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (UsrDat);

	 /* User */
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_ROLES_SINGUL_Abc[UsrDat->Roles.InCurrentCrs][UsrDat->Sex]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       ID_WriteUsrIDs (UsrDat,NULL);
	       HTM_TxtF ("&nbsp;%s",UsrDat->Surname1);
	       if (UsrDat->Surname2[0])
		  HTM_TxtF ("&nbsp;%s",UsrDat->Surname2);
	       if (UsrDat->FrstName[0])
		  HTM_TxtF (", %s",UsrDat->FrstName);
	       HTM_BR ();
	       Pho_ShowUsrPhotoIfAllowed (UsrDat,
					  ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Start/end time (for user in this match) *****/
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			     The_GetSuffix ());
		  HTM_TxtColon (Txt_START_END_TIME[StartEndTime]);
	       HTM_TD_End ();

	       if (asprintf (&Id,"match_%u",(unsigned) StartEndTime) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_TD_Begin ("id=\"%s\" class=\"LT DAT_%s\"",
			     Id,The_GetSuffix ());
		  Dat_WriteLocalDateHMSFromUTC (Id,Print.TimeUTC[StartEndTime],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
						true,true,true,0x7);
	       HTM_TD_End ();
	       free (Id);

	    HTM_TR_End ();
	   }

	 /***** Number of questions *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Questions);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       HTM_Unsigned (Print.NumQsts.All);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Number of answers *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Answers);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       HTM_Unsigned (Print.NumQsts.NotBlank);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Score *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Score);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       if (ICanView.Score)
		 {
		  HTM_STRONG_Begin ();
		     HTM_Double2Decimals (Print.Score);
		     HTM_Txt ("/");
		     HTM_Unsigned (Print.NumQsts.All);
		  HTM_STRONG_End ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Grade *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Grade);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       if (ICanView.Score)
		 {
		  HTM_STRONG_Begin ();
		     TstPrn_ComputeAndShowGrade (Print.NumQsts.All,Print.Score,Games.Game.MaxGrade);
		  HTM_STRONG_End ();
		 }
	       else
		  Ico_PutIconNotVisible ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Tags present in this result *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"RT DAT_STRONG_%s\"",
			  The_GetSuffix ());
	       HTM_TxtColon (Txt_Tags);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LB DAT_%s\"",
			  The_GetSuffix ());
	       Gam_ShowTstTagsPresentInAGame (Match.GamCod);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Write answers and solutions *****/
	 TstPrn_ShowPrintAnswers (UsrDat,
				  Print.NumQsts.All,
				  Print.PrintedQuestions,
				  Print.TimeUTC,
				  Games.Game.Visibility);

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Get if I can view match result ***********************/
/*****************************************************************************/

static void MchRes_CheckIfICanViewMatchResult (const struct Gam_Game *Game,
                                               const struct Mch_Match *Match,
                                               long UsrCod,
                                               struct MchRes_ICanView *ICanView)
  {
   /***** Check if I can view print result and score *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 // Depends on visibility of game and result (eye icons)
	 ICanView->Result = (Game->HiddenOrVisible == HidVis_VISIBLE &&	// The game is visible
			     Match->Status.ShowUsrResults &&		// The results of the match are visible to users
			     Usr_ItsMe (UsrCod) == Usr_ME);		// The result is mine
	 // Whether I belong or not to groups of match is not checked here...
	 // ...because I should be able to see old matches made in old groups to which I belonged

	 if (ICanView->Result)
	    // Depends on 5 visibility icons associated to game
	    ICanView->Score = TstVis_IsVisibleTotalScore (Game->Visibility);
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
