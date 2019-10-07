// swad_game.c: games using remote control

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

#include "swad_database.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_match.h"
#include "swad_match_result.h"
#include "swad_pagination.h"
#include "swad_role.h"
#include "swad_table.h"
#include "swad_test.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Gam_MAX_CHARS_ANSWER	(1024 - 1)	// 1023
#define Gam_MAX_BYTES_ANSWER	((Gam_MAX_CHARS_ANSWER + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Gam_MAX_BYTES_LIST_ANSWER_TYPES	(10 + (Gam_NUM_ANS_TYPES - 1) * (1 + 10))

const char *Gam_StrAnswerTypesDB[Gam_NUM_ANS_TYPES] =
  {
   "unique_choice",
   "multiple_choice",
  };

#define Gam_MAX_ANSWERS_PER_QUESTION	10

#define Gam_MAX_SELECTED_QUESTIONS		1000
#define Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS	(Gam_MAX_SELECTED_QUESTIONS * (1 + 10 + 1))

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

long Gam_CurrentGamCod = -1L;	// Used as parameter in contextual links
unsigned Gam_CurrentQstInd = 0;	// Used as parameter in contextual links

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Gam_ListAllGames (void);
static bool Gam_CheckIfICanEditGames (void);
static void Gam_PutIconsListGames (void);
static void Gam_PutIconToCreateNewGame (void);
static void Gam_PutButtonToCreateNewGame (void);
static void Gam_PutParamsToCreateNewGame (void);
static void Gam_WriteAuthor (struct Game *Game);

static void Gam_PutFormsToRemEditOneGame (const struct Game *Game,
					  const char *Anchor);

static void Gam_PutHiddenParamOrder (void);
static void Gam_GetParamOrder (void);

static void Gam_ResetGame (struct Game *Game);

static void Gam_GetGameTxtFromDB (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

static void Gam_RemoveGameFromAllTables (long GamCod);

static bool Gam_CheckIfSimilarGameExists (struct Game *Game);

static void Gam_PutFormsEditionGame (struct Game *Game,bool ItsANewGame);

static void Gam_CreateGame (struct Game *Game,const char *Txt);
static void Gam_UpdateGame (struct Game *Game,const char *Txt);

static void Gam_RemAnswersOfAQuestion (long GamCod,unsigned QstInd);

static unsigned Gam_GetMaxQuestionIndexInGame (long GamCod);
static void Gam_ListGameQuestions (struct Game *Game);
static void Gam_ListOneOrMoreQuestionsForEdition (long GamCod,unsigned NumQsts,
                                                  MYSQL_RES *mysql_res,
						  bool ICanEditQuestions);
static void Gam_PutIconToAddNewQuestions (void);
static void Gam_PutButtonToAddNewQuestions (void);

static void Gam_AllocateListSelectedQuestions (void);
static void Gam_FreeListsSelectedQuestions (void);
static unsigned Gam_CountNumQuestionsInList (void);

static void Gam_PutParamsOneQst (void);

static void Gam_ExchangeQuestions (long GamCod,
                                   unsigned QstIndTop,unsigned QstIndBottom);

static bool Gam_GetNumMchsGameAndCheckIfEditable (struct Game *Game);

static long Gam_GetParamCurrentGamCod (void);

/*****************************************************************************/
/***************************** List all games ********************************/
/*****************************************************************************/

void Gam_SeeAllGames (void)
  {
   /***** Get parameters *****/
   Gam_GetParams ();	// Return value ignored

   /***** Show all games *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/**************************** Show all the games *****************************/
/*****************************************************************************/

static void Gam_ListAllGames (void)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Games;
   extern const char *Txt_GAMES_ORDER_HELP[Gam_NUM_ORDERS];
   extern const char *Txt_GAMES_ORDER[Gam_NUM_ORDERS];
   extern const char *Txt_Matches;
   extern const char *Txt_No_games;
   Gam_Order_t Order;
   struct Pagination Pagination;
   unsigned NumGame;

   /***** Put link to view matches results *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
         McR_PutFormToViewMchResults (ActReqSeeMyMchRes);
         break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
         McR_PutFormToViewMchResults (ActReqSeeUsrMchRes);
	 break;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
	 break;
      default:
	 Rol_WrongRoleExit ();
	 break;
     }

   /***** Get number of groups in current course *****/
   if (!Gbl.Crs.Grps.NumGrps)
      Gbl.Crs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of games *****/
   Gam_GetListGames ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Games.Num;
   Pagination.CurrentPage = (int) Gbl.Games.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Games.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_GAMES,
                                     0,
                                     &Pagination);

   /***** Start box *****/
   Box_StartBox ("100%",Txt_Games,Gam_PutIconsListGames,
                 Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

   if (Gbl.Games.Num)
     {
      /***** Table head *****/
      Tbl_StartTableWideMarginPadding (2);
      Tbl_StartRow ();
      if (Gam_CheckIfICanEditGames ())
         fprintf (Gbl.F.Out,"<th class=\"CONTEXT_COL\"></th>");	// Column for contextual icons

      for (Order  = (Gam_Order_t) 0;
	   Order <= (Gam_Order_t) (Gam_NUM_ORDERS - 1);
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");

	 /* Form to change order */
	 Frm_StartForm (ActSeeAllGam);
	 Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Frm_LinkFormSubmit (Txt_GAMES_ORDER_HELP[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Games.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_GAMES_ORDER[Order]);
	 if (Order == Gbl.Games.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Frm_EndForm ();

	 fprintf (Gbl.F.Out,"</th>");
	}

      fprintf (Gbl.F.Out,"<th class=\"RIGHT_MIDDLE\">%s</th>",Txt_Matches);

      Tbl_EndRow ();

      /***** Write all the games *****/
      for (NumGame = Pagination.FirstItemVisible;
	   NumGame <= Pagination.LastItemVisible;
	   NumGame++)
	 Gam_ShowOneGame (Gbl.Games.LstGamCods[NumGame - 1],
	                  false,
	                  false,	// Do not list game questions
	                  false);	// Do not put form to start new match

      /***** End table *****/
      Tbl_EndTable ();
     }
   else	// No games created
      Ale_ShowAlert (Ale_INFO,Txt_No_games);

   /***** Button to create a new game *****/
   if (Gam_CheckIfICanEditGames ())
      Gam_PutButtonToCreateNewGame ();

   /***** End box *****/
   Box_EndBox ();

   /***** Write again links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_GAMES,
                                     0,
                                     &Pagination);

   /***** Free list of games *****/
   Gam_FreeListGames ();
  }

/*****************************************************************************/
/************************ Check if I can edit games **************************/
/*****************************************************************************/

static bool Gam_CheckIfICanEditGames (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/***************** Put contextual icons in list of games *******************/
/*****************************************************************************/

static void Gam_PutIconsListGames (void)
  {
   /***** Put icon to create a new game *****/
   if (Gam_CheckIfICanEditGames ())
      Gam_PutIconToCreateNewGame ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_GAMES;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************** Put icon to create a new game **********************/
/*****************************************************************************/

static void Gam_PutIconToCreateNewGame (void)
  {
   extern const char *Txt_New_game;

   Ico_PutContextualIconToAdd (ActFrmNewGam,NULL,Gam_PutParamsToCreateNewGame,
			       Txt_New_game);
  }

/*****************************************************************************/
/********************* Put button to create a new game *********************/
/*****************************************************************************/

static void Gam_PutButtonToCreateNewGame (void)
  {
   extern const char *Txt_New_game;

   Frm_StartForm (ActFrmNewGam);
   Gam_PutParamsToCreateNewGame ();
   Btn_PutConfirmButton (Txt_New_game);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Put parameters to create a new game *******************/
/*****************************************************************************/

static void Gam_PutParamsToCreateNewGame (void)
  {
   Gam_PutHiddenParamGameOrder ();
   Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
  }

/*****************************************************************************/
/****************************** Show one game ******************************/
/*****************************************************************************/

void Gam_SeeOneGame (void)
  {
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Show game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    false,	// Do not list game questions
	            false);	// Do not put form to start new match
  }

/*****************************************************************************/
/******************************* Show one game *******************************/
/*****************************************************************************/

void Gam_ShowOneGame (long GamCod,
                      bool ShowOnlyThisGame,
                      bool ListGameQuestions,
                      bool PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Game;
   extern const char *Txt_Today;
   extern const char *Txt_View_game;
   extern const char *Txt_No_of_questions;
   extern const char *Txt_Matches;
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   struct Game Game;
   Dat_StartEndTime_t StartEndTime;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Start box *****/
   if (ShowOnlyThisGame)
      Box_StartBox (NULL,Txt_Game,NULL,
                    Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

   /***** Get data of this game *****/
   Game.GamCod = GamCod;
   Gam_GetDataOfGameByCod (&Game);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Game.GamCod,&Anchor);

   /***** Start table *****/
   if (ShowOnlyThisGame)
      Tbl_StartTableWidePadding (2);

   /***** Start first row of this game *****/
   Tbl_StartRow ();

   /***** Icons related to this game *****/
   if (Gam_CheckIfICanEditGames ())
     {
      fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"CONTEXT_COL");
      if (!ShowOnlyThisGame)
	 fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
      fprintf (Gbl.F.Out,"\">");

      /* Icons to remove/edit this game */
      Gam_PutFormsToRemEditOneGame (&Game,Anchor);

      Tbl_EndCell ();
     }

   /***** Start/end date/time *****/
   UniqueId++;
   for (StartEndTime = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      fprintf (Gbl.F.Out,"<td id=\"gam_date_%u_%u\" class=\"%s LEFT_TOP",
	       (unsigned) StartEndTime,UniqueId,
	       Game.Hidden ? "DATE_GREEN_LIGHT":
		             "DATE_GREEN");
      if (!ShowOnlyThisGame)
	 fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
      fprintf (Gbl.F.Out,"\">");
      if (Game.TimeUTC[Dat_START_TIME])
	 fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
			    "writeLocalDateHMSFromUTC('gam_date_%u_%u',%ld,"
			    "%u,'<br />','%s',true,true,0x7);"
			    "</script>",
		  (unsigned) StartEndTime,UniqueId,
		  Game.TimeUTC[StartEndTime],
		  (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
      Tbl_EndCell ();
     }

   /***** Game title and main data *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Game title */
   Gam_SetParamCurrentGamCod (GamCod);	// Used to pass parameter
   Lay_StartArticle (Anchor);
   Frm_StartForm (ActSeeGam);
   Gam_PutParams ();
   Frm_LinkFormSubmit (Txt_View_game,
                       Game.Hidden ? "ASG_TITLE_LIGHT":
                	             "ASG_TITLE",NULL);
   fprintf (Gbl.F.Out,"%s</a>",
            Game.Title);
   Frm_EndForm ();
   Lay_EndArticle ();

   /* Number of questions */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: %u</div>",
            Game.Hidden ? "ASG_GRP_LIGHT" :
        	          "ASG_GRP",
            Txt_No_of_questions,
            Game.NumQsts);

   Tbl_EndCell ();

   /***** Number of matches in game *****/
   fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   Gam_SetParamCurrentGamCod (GamCod);	// Used to pass parameter
   Frm_StartForm (ActSeeGam);
   Gam_PutParams ();
   Frm_LinkFormSubmit (Txt_Matches,
                       Game.Hidden ? "ASG_TITLE_LIGHT" :
                	             "ASG_TITLE",NULL);
   if (ShowOnlyThisGame)
      fprintf (Gbl.F.Out,"%s:&nbsp;",Txt_Matches);
   fprintf (Gbl.F.Out,"%u</a>",Game.NumMchs);
   Frm_EndForm ();

   Tbl_EndCell ();

   /***** End 1st row of this game *****/
   Tbl_EndRow ();

   /***** Start 2nd row of this game *****/
   Tbl_StartRow ();

   /***** Author of the game *****/
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Gam_WriteAuthor (&Game);
   Tbl_EndCell ();

   /***** Text of the game *****/
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Gam_GetGameTxtFromDB (Game.GamCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<div class=\"PAR %s\">%s</div>",
            Game.Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Txt);
   Tbl_EndCell ();

   /***** End 2nd row of this game *****/
   Tbl_EndRow ();

   /***** End table *****/
   if (ShowOnlyThisGame)
      Tbl_EndTable ();
   else
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   if (ShowOnlyThisGame)
     {
      if (ListGameQuestions)
          /***** Write questions of this game *****/
	 Gam_ListGameQuestions (&Game);
      else
	 /***** List matches *****/
	 Mch_ListMatches (&Game,PutFormNewMatch);

      /***** End box *****/
      Box_EndBox ();
     }
  }

/*****************************************************************************/
/*********************** Write the author of a game ************************/
/*****************************************************************************/

static void Gam_WriteAuthor (struct Game *Game)
  {
   Usr_WriteAuthor1Line (Game->UsrCod,Game->Hidden);
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of games *******/
/*****************************************************************************/

void Gam_PutHiddenParamGameOrder (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Games.SelectedOrder);
  }

/*****************************************************************************/
/******************** Put a link (form) to edit one game *********************/
/*****************************************************************************/

static void Gam_PutFormsToRemEditOneGame (const struct Game *Game,
					  const char *Anchor)
  {
   Gam_SetParamCurrentGamCod (Game->GamCod);	// Used to pass parameter

   /***** Put icon to remove game *****/
   Ico_PutContextualIconToRemove (ActReqRemGam,Gam_PutParams);

   /***** Put icon to unhide/hide game *****/
   if (Game->Hidden)
      Ico_PutContextualIconToUnhide (ActShoGam,Anchor,Gam_PutParams);
   else
      Ico_PutContextualIconToHide (ActHidGam,Anchor,Gam_PutParams);

   /***** Put icon to edit game *****/
   Ico_PutContextualIconToEdit (ActEdiOneGam,Gam_PutParams);
  }

/*****************************************************************************/
/*********************** Params used to edit a game **************************/
/*****************************************************************************/

void Gam_PutParams (void)
  {
   long CurrentGamCod = Gam_GetParamCurrentGamCod ();

   if (CurrentGamCod > 0)
      Gam_PutParamGameCod (CurrentGamCod);

   Gam_PutHiddenParamOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
  }

/*****************************************************************************/
/******************* Get parameters used to edit a game **********************/
/*****************************************************************************/

long Gam_GetParams (void)
  {
   /***** Get other parameters *****/
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   return Gam_GetParamGameCod ();
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of games *******/
/*****************************************************************************/

static void Gam_PutHiddenParamOrder (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Games.SelectedOrder);
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of games ************/
/*****************************************************************************/

static void Gam_GetParamOrder (void)
  {
   Gbl.Games.SelectedOrder = (Gam_Order_t)
	                     Par_GetParToUnsignedLong ("Order",
	                                               0,
	                                               Gam_NUM_ORDERS - 1,
	                                               (unsigned long) Gam_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*********************** Get list of all the games *************************/
/*****************************************************************************/

void Gam_GetListGames (void)
  {
   static const char *OrderBySubQuery[Gam_NUM_ORDERS] =
     {
      "StartTime DESC,EndTime DESC,gam_games.Title DESC",	// Gam_ORDER_BY_START_DATE
      "EndTime DESC,StartTime DESC,gam_games.Title DESC",	// Gam_ORDER_BY_END_DATE
      "gam_games.Title DESC",					// Gam_ORDER_BY_TITLE
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char *HiddenSubQuery;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned NumGame;

   /***** Free list of games *****/
   if (Gbl.Games.LstIsRead)
      Gam_FreeListGames ();

   /***** Subquery: get hidden games depending on user's role *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
         if (asprintf (&HiddenSubQuery," AND Hidden='N'") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 if (asprintf (&HiddenSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	 break;
      default:
	 Rol_WrongRoleExit ();
	 break;
     }

   /***** Get list of games from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get games",
			     "SELECT gam_games.GamCod,"				// row[0]
			            "MIN(mch_matches.StartTime) AS StartTime,"	// row[1]
			            "MAX(mch_matches.EndTime) AS EndTime"	// row[2]
			     " FROM gam_games"
			     " LEFT JOIN mch_matches"
			     " ON gam_games.GamCod=mch_matches.GamCod"
			     " WHERE gam_games.CrsCod=%ld"
			     "%s"
			     " GROUP BY gam_games.GamCod"
			     " ORDER BY %s",
			     Gbl.Hierarchy.Crs.CrsCod,
			     HiddenSubQuery,
			     OrderBySubQuery[Gbl.Games.SelectedOrder]);

   /***** Free allocated memory for subquery *****/
   free ((void *) HiddenSubQuery);

   if (NumRows) // Games found...
     {
      Gbl.Games.Num = (unsigned) NumRows;

      /***** Create list of games *****/
      if ((Gbl.Games.LstGamCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the games codes *****/
      for (NumGame = 0;
	   NumGame < Gbl.Games.Num;
	   NumGame++)
        {
         /* Get next game code (row[0]) */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.Games.LstGamCods[NumGame] = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Lay_ShowErrorAndExit ("Error: wrong game code.");
        }
     }
   else
      Gbl.Games.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.Games.LstIsRead = true;
  }

/*****************************************************************************/
/********************** Get game data using its code *************************/
/*****************************************************************************/

void Gam_GetDataOfGameByCod (struct Game *Game)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get data of game from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get game data",
			     "SELECT gam_games.GamCod,"		// row[0]
			            "gam_games.CrsCod,"		// row[1]
			            "gam_games.Hidden,"		// row[2]
			            "gam_games.UsrCod,"		// row[3]
			            "gam_games.Title"		// row[4]
			     " FROM gam_games"
			     " LEFT JOIN mch_matches"
			     " ON gam_games.GamCod=mch_matches.GamCod"
			     " WHERE gam_games.GamCod=%ld",
			     Game->GamCod);
   if (NumRows) // Game found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the game (row[0]) */
      Game->GamCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get code of the course (row[1]) */
      Game->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get whether the game is hidden (row[2]) */
      Game->Hidden = (row[2][0] == 'Y');

      /* Get author of the game (row[3]) */
      Game->UsrCod = Str_ConvertStrCodToLongCod (row[3]);

      /* Get the title of the game (row[4]) */
      Str_Copy (Game->Title,row[4],
                Gam_MAX_BYTES_TITLE);

      /* Get number of questions */
      Game->NumQsts = Gam_GetNumQstsGame (Game->GamCod);

      /* Get number of matches */
      Game->NumMchs = Mch_GetNumMchsInGame (Game->GamCod);
     }
   else
      /* Initialize to empty game */
      Gam_ResetGame (Game);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   if (Game->GamCod > 0)
     {
      /***** Get start and end times from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get game data",
				"SELECT UNIX_TIMESTAMP(MIN(StartTime)),"	// row[0]
				       "UNIX_TIMESTAMP(MAX(EndTime))"		// row[1]
				" FROM mch_matches"
				" WHERE GamCod=%ld",
				Game->GamCod);
      if (NumRows)
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get start date (row[0] holds the start UTC time) */
	 Game->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[0]);

	 /* Get end   date (row[1] holds the end   UTC time) */
	 Game->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[1]);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }
   else
     {
      Game->TimeUTC[Dat_START_TIME] =
      Game->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
     }
  }

/*****************************************************************************/
/*************************** Initialize game to empty ************************/
/*****************************************************************************/

static void Gam_ResetGame (struct Game *Game)
  {
   /***** Initialize to empty game *****/
   Game->GamCod = -1L;
   Game->CrsCod = -1L;
   Game->UsrCod = -1L;
   Game->TimeUTC[Dat_START_TIME] = (time_t) 0;
   Game->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Game->Title[0]                = '\0';
   Game->NumQsts                 = 0;
   Game->NumMchs                 = 0;
   Game->Hidden                  = false;
  }

/*****************************************************************************/
/***************************** Free list of games ****************************/
/*****************************************************************************/

void Gam_FreeListGames (void)
  {
   if (Gbl.Games.LstIsRead && Gbl.Games.LstGamCods)
     {
      /***** Free memory used by the list of games *****/
      free ((void *) Gbl.Games.LstGamCods);
      Gbl.Games.LstGamCods = NULL;
      Gbl.Games.Num = 0;
      Gbl.Games.LstIsRead = false;
     }
  }

/*****************************************************************************/
/********************** Get game text from database ************************/
/*****************************************************************************/

static void Gam_GetGameTxtFromDB (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of game from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get game text",
			     "SELECT Txt FROM gam_games WHERE GamCod=%ld",
			     GamCod);

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Txt,row[0],
                Cns_MAX_BYTES_TEXT);
     }
   else
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting game text.");
  }

/*****************************************************************************/
/******************** Write parameter with code of game **********************/
/*****************************************************************************/

void Gam_PutParamGameCod (long GamCod)
  {
   Par_PutHiddenParamLong ("GamCod",GamCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of game ***********************/
/*****************************************************************************/

long Gam_GetParamGameCod (void)
  {
   /***** Get code of game *****/
   return Par_GetParToLong ("GamCod");
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing of a game ******************/
/*****************************************************************************/

void Gam_AskRemGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_game_X;
   extern const char *Txt_Remove_game;
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Act_NoPermissionExit ();

   /***** Show question and button to remove game *****/
   Gam_SetParamCurrentGamCod (Game.GamCod);	// Used to pass parameter
   Ale_ShowAlertAndButton (ActRemGam,NULL,NULL,Gam_PutParams,
			   Btn_REMOVE_BUTTON,Txt_Remove_game,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_game_X,
                           Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/******************************* Remove a game *******************************/
/*****************************************************************************/

void Gam_RemoveGame (void)
  {
   extern const char *Txt_Game_X_removed;
   struct Game Game;

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Act_NoPermissionExit ();

   /***** Remove game from all tables *****/
   Gam_RemoveGameFromAllTables (Game.GamCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Game_X_removed,
                  Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/*********************** Remove game from all tables *************************/
/*****************************************************************************/

static void Gam_RemoveGameFromAllTables (long GamCod)
  {
   /***** Remove all matches in this game *****/
   Mch_RemoveMatchesInGameFromAllTables (GamCod);

   /***** Remove game question *****/
   DB_QueryDELETE ("can not remove game questions",
		   "DELETE FROM gam_questions WHERE GamCod=%ld",
		   GamCod);

   /***** Remove game *****/
   DB_QueryDELETE ("can not remove game",
		   "DELETE FROM gam_games WHERE GamCod=%ld",
		   GamCod);
  }

/*****************************************************************************/
/******************** Remove all the games of a course ***********************/
/*****************************************************************************/

void Gam_RemoveGamesCrs (long CrsCod)
  {
   /***** Remove all matches in this course *****/
   Mch_RemoveMatchInCourseFromAllTables (CrsCod);

   /***** Remove the questions in games *****/
   DB_QueryDELETE ("can not remove questions in course games",
		   "DELETE FROM gam_questions"
		   " USING gam_games,gam_questions"
		   " WHERE gam_games.CrsCod=%ld"
		   " AND gam_games.GamCod=gam_questions.GamCod",
                   CrsCod);

   /***** Remove the games *****/
   DB_QueryDELETE ("can not remove course games",
		   "DELETE FROM gam_games"
		   " WHERE CrsCod=%ld",
                   CrsCod);
  }

/*****************************************************************************/
/******************************** Hide a game ******************************/
/*****************************************************************************/

void Gam_HideGame (void)
  {
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Act_NoPermissionExit ();

   /***** Hide game *****/
   DB_QueryUPDATE ("can not hide game",
		   "UPDATE gam_games SET Hidden='Y' WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/******************************** Show a game ******************************/
/*****************************************************************************/

void Gam_UnhideGame (void)
  {
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Act_NoPermissionExit ();

   /***** Show game *****/
   DB_QueryUPDATE ("can not show game",
		   "UPDATE gam_games SET Hidden='N' WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/******************* Check if the title of a game exists *******************/
/*****************************************************************************/

static bool Gam_CheckIfSimilarGameExists (struct Game *Game)
  {
   /***** Get number of games with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar games",
			  "SELECT COUNT(*) FROM gam_games"
			  " WHERE CrsCod=%ld AND Title='%s'"
			  " AND GamCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,Game->Title,
			  Game->GamCod) != 0);
  }

/*****************************************************************************/
/**************** Request the creation or edition of a game ******************/
/*****************************************************************************/

void Gam_RequestCreatOrEditGame (void)
  {
   struct Game Game;
   bool ItsANewGame;

   /***** Check if I can create new games *****/
   if (!Gam_CheckIfICanEditGames ())
      Act_NoPermissionExit ();

   /***** Get parameters *****/
   Game.GamCod = Gam_GetParams ();
   ItsANewGame = (Game.GamCod < 0);

   /***** Put forms to create/edit a game *****/
   Gam_PutFormsEditionGame (&Game,ItsANewGame);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/********************* Put a form to create/edit a game **********************/
/*****************************************************************************/

static void Gam_PutFormsEditionGame (struct Game *Game,bool ItsANewGame)
  {
   extern const char *Hlp_ASSESSMENT_Games_new_game;
   extern const char *Hlp_ASSESSMENT_Games_edit_game;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_game;
   extern const char *Txt_Edit_game;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Create_game;
   extern const char *Txt_Save_changes;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get game data *****/
   if (ItsANewGame)
      /* Initialize to empty game */
      Gam_ResetGame (Game);
   else
     {
      /* Get game data from database */
      Gam_GetDataOfGameByCod (Game);
      Gam_GetGameTxtFromDB (Game->GamCod,Txt);
     }

   /***** Start form *****/
   Gam_SetParamCurrentGamCod (Game->GamCod);	// Used to pass parameter
   Frm_StartForm (ItsANewGame ? ActNewGam :
				ActChgGam);
   Gam_PutParams ();

   /***** Start box and table *****/
   if (ItsANewGame)
      Box_StartBoxTable (NULL,Txt_New_game,NULL,
			 Hlp_ASSESSMENT_Games_new_game,Box_NOT_CLOSABLE,2);
   else
      Box_StartBoxTable (NULL,
			 Game->Title[0] ? Game->Title :
					  Txt_Edit_game,
			 NULL,
			 Hlp_ASSESSMENT_Games_edit_game,Box_NOT_CLOSABLE,2);

   /***** Game title *****/
   Tbl_StartRow ();

   fprintf (Gbl.F.Out,"<td class=\"RIGHT_MIDDLE\">"
		      "<label for=\"Title\" class=\"%s\">%s:</label>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Title);
   Tbl_EndCell ();

   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
		      "<input type=\"text\" id=\"Title\" name=\"Title\""
		      " size=\"45\" maxlength=\"%u\" value=\"%s\""
		      " required=\"required\" />",
	    Gam_MAX_CHARS_TITLE,Game->Title);
   Tbl_EndCell ();

   Tbl_EndRow ();

   /***** Game text *****/
   Tbl_StartRow ();

   fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP\">"
		      "<label for=\"Txt\" class=\"%s\">%s:</label>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_Description);
   Tbl_EndCell ();

   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">"
		      "<textarea id=\"Txt\" name=\"Txt\""
		      " cols=\"60\" rows=\"10\">");
   if (!ItsANewGame)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>");
   Tbl_EndCell ();

   Tbl_EndRow ();

   /***** End table, send button and end box *****/
   if (ItsANewGame)
      Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_game);
   else
      Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show questions of the game ready to be edited *****/
   if (!ItsANewGame)
      Gam_ListGameQuestions (Game);
  }

/*****************************************************************************/
/********************* Receive form to create a new game *******************/
/*****************************************************************************/

void Gam_RecFormGame (void)
  {
   extern const char *Txt_Already_existed_a_game_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_game;
   struct Game OldGame;
   struct Game NewGame;
   bool ItsANewGame;
   bool NewGameIsCorrect = true;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the game *****/
   NewGame.GamCod = Gam_GetParamGameCod ();

   /***** Check if game has matches *****/
   if (Gam_CheckIfICanEditGames ())
     {
      ItsANewGame = (NewGame.GamCod < 0);
      if (!ItsANewGame)
	{
	 /* Get data of the old (current) game from database */
	 OldGame.GamCod = NewGame.GamCod;
	 Gam_GetDataOfGameByCod (&OldGame);
	 if (!Gam_CheckIfICanEditGames ())
            Act_NoPermissionExit ();
	}

      /***** Get game title *****/
      Par_GetParToText ("Title",NewGame.Title,Gam_MAX_BYTES_TITLE);

      /***** Get game text and insert links *****/
      Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

      /***** Check if title is correct *****/
      if (NewGame.Title[0])	// If there's a game title
	{
	 /* If title of game was in database... */
	 if (Gam_CheckIfSimilarGameExists (&NewGame))
	   {
	    NewGameIsCorrect = false;
	    Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_game_with_the_title_X,
			   NewGame.Title);
	   }
	}
      else	// If there is not a game title
	{
	 NewGameIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_game);
	}

      /***** Create a new game or update an existing one *****/
      if (NewGameIsCorrect)
	{
	 if (ItsANewGame)
	    Gam_CreateGame (&NewGame,Txt);	// Add new game to database
	 else
	    Gam_UpdateGame (&NewGame,Txt);
	}
      else
	 Gam_RequestCreatOrEditGame ();

      /***** Put forms to create/edit a game *****/
      Gam_PutFormsEditionGame (&NewGame,false);
     }
   else
      Act_NoPermissionExit ();

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/**************************** Create a new game ******************************/
/*****************************************************************************/

static void Gam_CreateGame (struct Game *Game,const char *Txt)
  {
   extern const char *Txt_Created_new_game_X;

   /***** Create a new game *****/
   Game->GamCod =
   DB_QueryINSERTandReturnCode ("can not create new game",
				"INSERT INTO gam_games"
				" (CrsCod,Hidden,UsrCod,Title,Txt)"
				" VALUES"
				" (%ld,'N',%ld,'%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Game->Title,
				Txt);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_game_X,
                  Game->Title);
  }

/*****************************************************************************/
/************************* Update an existing game *************************/
/*****************************************************************************/

static void Gam_UpdateGame (struct Game *Game,const char *Txt)
  {
   extern const char *Txt_The_game_has_been_modified;

   /***** Update the data of the game *****/
   DB_QueryUPDATE ("can not update game",
		   "UPDATE gam_games"
		   " SET CrsCod=%ld,"
		        "Title='%s',"
		        "Txt='%s'"
		   " WHERE GamCod=%ld",
		   Gbl.Hierarchy.Crs.CrsCod,
	           Game->Title,
	           Txt,
	           Game->GamCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_game_has_been_modified);
  }

/*****************************************************************************/
/************* Check if a match is associated to a given group ***************/
/*****************************************************************************/

bool Gam_CheckIfMatchIsAssociatedToGrp (long MchCod,long GrpCod)
  {
   /***** Get if a match is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if a match is associated to a group",
			  "SELECT COUNT(*) FROM mch_groups"
			  " WHERE MchCod=%ld AND GrpCod=%ld",
			  MchCod,GrpCod) != 0);
  }

/*****************************************************************************/
/******************* Get number of questions of a game *********************/
/*****************************************************************************/

unsigned Gam_GetNumQstsGame (long GamCod)
  {
   /***** Get nuumber of questions in a game from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of questions of a game",
			     "SELECT COUNT(*) FROM gam_questions"
			     " WHERE GamCod=%ld",
			     GamCod);
  }

/*****************************************************************************/
/*********** Put a form to edit/create a question in game  *****************/
/*****************************************************************************/

void Gam_RequestNewQuestion (void)
  {
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Check if game has matches *****/
   if (Gam_GetNumMchsGameAndCheckIfEditable (&Game))
     {
      /***** Show form to create a new question in this game *****/
      Gam_SetParamCurrentGamCod (Game.GamCod);	// Used to pass parameter
      Tst_ShowFormAskSelectTstsForGame ();
     }
   else
      Act_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/**************** List several test questions for selection ******************/
/*****************************************************************************/

void Gam_ListTstQuestionsToSelect (void)
  {
   struct Game Game;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Check if game has matches *****/
   if (Gam_GetNumMchsGameAndCheckIfEditable (&Game))
     {
      /***** List several test questions for selection *****/
      Gam_SetParamCurrentGamCod (Game.GamCod);	// Used to pass parameter
      Tst_ListQuestionsToSelect ();
     }
   else
      Act_NoPermissionExit ();
  }

/*****************************************************************************/
/****************** Write parameter with index of question *******************/
/*****************************************************************************/

void Gam_PutParamQstInd (unsigned QstInd)
  {
   Par_PutHiddenParamUnsigned ("QstInd",QstInd);
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

unsigned Gam_GetParamQstInd (void)
  {
   long QstInd;

   QstInd = Par_GetParToLong ("QstInd");
   if (QstInd < 0)
      Lay_ShowErrorAndExit ("Wrong question index.");

   return (unsigned) QstInd;
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

unsigned Gam_GetQstIndFromStr (const char *UnsignedStr)
  {
   long QstInd;

   QstInd = Str_ConvertStrCodToLongCod (UnsignedStr);
   return (QstInd > 0) ? (unsigned) QstInd :
	                 0;
  }

/*****************************************************************************/
/********************** Remove answers of a game question ********************/
/*****************************************************************************/

static void Gam_RemAnswersOfAQuestion (long GamCod,unsigned QstInd)
  {
   /***** Remove answers from all matches of this game *****/
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM mch_answers"
		   " USING mch_matches,mch_answers"
		   " WHERE mch_matches.GamCod=%ld"	// From all matches of this game...
		   " AND mch_matches.MchCod=mch_answers.MchCod"
		   " AND mch_answers.QstInd=%u",	// ...remove only answers to this question
		   GamCod,QstInd);
  }

/*****************************************************************************/
/************ Get question code given game and index of question *************/
/*****************************************************************************/

long Gam_GetQstCodFromQstInd (long GamCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long QstCod;

   /***** Get question code of thw question to be moved up *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get question code",
			"SELECT QstCod FROM gam_questions"
			" WHERE GamCod=%ld AND QstInd=%u",
			GamCod,QstInd))
      Lay_ShowErrorAndExit ("Error: wrong question code.");

   /***** Get question code (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Lay_ShowErrorAndExit ("Error: wrong question code.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstCod;
  }

/*****************************************************************************/
/****************** Get maximum question index in a game *********************/
/*****************************************************************************/
// Question index can be 1, 2, 3...
// Return 0 if no questions

static unsigned Gam_GetMaxQuestionIndexInGame (long GamCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned QstInd = 0;

   /***** Get maximum question index in a game from database *****/
   DB_QuerySELECT (&mysql_res,"can not get last question index",
		   "SELECT MAX(QstInd) FROM gam_questions WHERE GamCod=%ld",
                   GamCod);
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
      if (sscanf (row[0],"%u",&QstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting last question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/*********** Get previous question index to a given index in a game **********/
/*****************************************************************************/
// Input question index can be 1, 2, 3... n-1
// Return question index will be 1, 2, 3... n if previous question exists, or 0 if no previous question

unsigned Gam_GetPrevQuestionIndexInGame (long GamCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned PrevQstInd = 0;

   /***** Get previous question index in a game from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get previous question index",
			"SELECT MAX(QstInd) FROM gam_questions"
			" WHERE GamCod=%ld AND QstInd<%u",
			GamCod,QstInd))
      Lay_ShowErrorAndExit ("Error: previous question index not found.");

   /***** Get previous question index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])
      if (sscanf (row[0],"%u",&PrevQstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting previous question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PrevQstInd;
  }

/*****************************************************************************/
/************* Get next question index to a given index in a game ************/
/*****************************************************************************/
// Input question index can be 0, 1, 2, 3... n-1
// Return question index will be 1, 2, 3... n if next question exists, or 0 if no next question

unsigned Gam_GetNextQuestionIndexInGame (long GamCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NextQstInd = Mch_AFTER_LAST_QUESTION;	// End of questions has been reached

   /***** Get next question index in a game from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   if (!DB_QuerySELECT (&mysql_res,"can not get next question index",
			"SELECT MIN(QstInd) FROM gam_questions"
			" WHERE GamCod=%ld AND QstInd>%u",
			GamCod,QstInd))
      Lay_ShowErrorAndExit ("Error: next question index not found.");

   /***** Get next question index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])
      if (sscanf (row[0],"%u",&NextQstInd) != 1)
	 Lay_ShowErrorAndExit ("Error when getting next question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NextQstInd;
  }

/*****************************************************************************/
/************************ List the questions of a game ***********************/
/*****************************************************************************/

static void Gam_ListGameQuestions (struct Game *Game)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_This_game_has_no_questions;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   bool ICanEditQuestions = Gam_GetNumMchsGameAndCheckIfEditable (Game);

   /***** Get data of questions from database *****/
   NumQsts = (unsigned) DB_QuerySELECT (&mysql_res,"can not get data of a question",
				        "SELECT gam_questions.QstInd,"		// row[0]
					       "gam_questions.QstCod,"		// row[1]
					       "tst_questions.AnsType,"		// row[2]
					       "tst_questions.Stem,"		// row[3]
					       "tst_questions.Feedback,"	// row[4]
					       "tst_questions.MedCod"		// row[5]
					" FROM gam_questions,tst_questions"
					" WHERE gam_questions.GamCod=%ld"
					" AND gam_questions.QstCod=tst_questions.QstCod"
					" ORDER BY gam_questions.QstInd",
					Game->GamCod);

   /***** Start box *****/
   Gam_SetParamCurrentGamCod (Game->GamCod);	// Used to pass parameter
   Box_StartBox (NULL,Txt_Questions,ICanEditQuestions ? Gam_PutIconToAddNewQuestions :
                                                        NULL,
                 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

   /***** Show table with questions *****/
   if (NumQsts)
      Gam_ListOneOrMoreQuestionsForEdition (Game->GamCod,NumQsts,mysql_res,
					    ICanEditQuestions);
   else	// This game has no questions
      Ale_ShowAlert (Ale_INFO,Txt_This_game_has_no_questions);

   /***** Put button to add a new question in this game *****/
   if (ICanEditQuestions)		// I can edit questions
      Gam_PutButtonToAddNewQuestions ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/********************* List game questions for edition ***********************/
/*****************************************************************************/

static void Gam_ListOneOrMoreQuestionsForEdition (long GamCod,unsigned NumQsts,
                                                  MYSQL_RES *mysql_res,
						  bool ICanEditQuestions)
  {
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Code;
   extern const char *Txt_Tags;
   extern const char *Txt_Question;
   extern const char *Txt_Move_up_X;
   extern const char *Txt_Move_down_X;
   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Tst_NUM_ANS_TYPES];
   unsigned NumQst;
   MYSQL_ROW row;
   unsigned QstInd;
   unsigned MaxQstInd;
   char StrQstInd[10 + 1];

   /***** Get maximum question index *****/
   MaxQstInd = Gam_GetMaxQuestionIndexInGame (GamCod);

   /***** Write the heading *****/
   Tbl_StartTableWideMarginPadding (2);
   Tbl_StartRow ();
   fprintf (Gbl.F.Out,"<th></th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>",
            Txt_No_INDEX,
            Txt_Code,
            Txt_Tags,
            Txt_Question);
   Tbl_EndRow ();

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      row = mysql_fetch_row (mysql_res);
      /*
      row[0] QstInd
      row[1] QstCod
      row[2] AnsType
      row[3] Stem
      row[4] Feedback
      row[5] MedCod
      */
      /***** Create test question *****/
      Tst_QstConstructor ();

      /* Get question index (row[0]) */
      QstInd = Gam_GetQstIndFromStr (row[0]);
      snprintf (StrQstInd,sizeof (StrQstInd),
	        "%u",
		QstInd);

      /* Get question code (row[1]) */
      Gbl.Test.QstCod = Str_ConvertStrCodToLongCod (row[1]);

      /***** Icons *****/
      Gam_SetParamCurrentGamCod (GamCod);	// Used to pass parameter
      Gam_CurrentQstInd = QstInd;
      Tbl_StartRow ();
      fprintf (Gbl.F.Out,"<td class=\"BT%u\">",Gbl.RowEvenOdd);

      /* Put icon to remove the question */
      if (ICanEditQuestions)
	{
	 Frm_StartForm (ActReqRemGamQst);
	 Gam_PutParams ();
	 Gam_PutParamQstInd (QstInd);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}
      else
         Ico_PutIconRemovalNotAllowed ();

      /* Put icon to move up the question */
      if (ICanEditQuestions && QstInd > 1)
	{
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Move_up_X,
		   StrQstInd);
	 Lay_PutContextualLinkOnlyIcon (ActUp_GamQst,NULL,Gam_PutParamsOneQst,
				        "arrow-up.svg",
					Gbl.Title);
	}
      else
         Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);

      /* Put icon to move down the question */
      if (ICanEditQuestions && QstInd < MaxQstInd)
	{
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Move_down_X,
		   StrQstInd);
	 Lay_PutContextualLinkOnlyIcon (ActDwnGamQst,NULL,Gam_PutParamsOneQst,
				        "arrow-down.svg",
					Gbl.Title);
	}
      else
         Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);

      /* Put icon to edit the question */
      if (ICanEditQuestions)
	 Ico_PutContextualIconToEdit (ActEdiOneTstQst,Tst_PutParamQstCod);

      Tbl_EndCell ();

      /* Write number of question */
      fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP COLOR%u\">"
			 "<div class=\"BIG_INDEX\">%s</div>",
	       Gbl.RowEvenOdd,
	       StrQstInd);

      /* Write answer type (row[2]) */
      Gbl.Test.AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
      fprintf (Gbl.F.Out,"<div class=\"DAT_SMALL\">%s</div>",
	       Txt_TST_STR_ANSWER_TYPES[Gbl.Test.AnswerType]);
      Tbl_EndCell ();

      /* Write question code */
      fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL CENTER_TOP COLOR%u\">"
	                 "%ld&nbsp;",
               Gbl.RowEvenOdd,Gbl.Test.QstCod);
      Tbl_EndCell ();

      /* Write the question tags */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
               Gbl.RowEvenOdd);
      Tst_GetAndWriteTagsQst (Gbl.Test.QstCod);
      Tbl_EndCell ();

      /* Write stem (row[3]) */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
	       Gbl.RowEvenOdd);
      Tst_WriteQstStem (row[3],"TEST_EDI");

      /* Get media (row[5]) */
      Gbl.Test.Media.MedCod = Str_ConvertStrCodToLongCod (row[5]);
      Med_GetMediaDataByCod (&Gbl.Test.Media);

      /* Show media */
      Med_ShowMedia (&Gbl.Test.Media,
                     "TEST_MED_EDIT_LIST_STEM_CONTAINER",
                     "TEST_MED_EDIT_LIST_STEM");

      /* Show feedback (row[4]) */
      Tst_WriteQstFeedback (row[4],"TEST_EDI_LIGHT");

      /* Show answers */
      Tst_WriteAnswersEdit (Gbl.Test.QstCod);

      Tbl_EndCell ();
      Tbl_EndRow ();

      /***** Destroy test question *****/
      Tst_QstDestructor ();
     }

   /***** End table *****/
   Tbl_EndTable ();
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to game *******************/
/*****************************************************************************/

static void Gam_PutIconToAddNewQuestions (void)
  {
   extern const char *Txt_Add_questions;

   /***** Put form to create a new question *****/
   Ico_PutContextualIconToAdd (ActAddOneGamQst,NULL,Gam_PutParams,
			       Txt_Add_questions);
  }

/*****************************************************************************/
/***************** Put button to add new questions to game *******************/
/*****************************************************************************/

static void Gam_PutButtonToAddNewQuestions (void)
  {
   extern const char *Txt_Add_questions;

   Frm_StartForm (ActAddOneGamQst);
   Gam_PutParams ();
   Btn_PutConfirmButton (Txt_Add_questions);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Add selected test questions to game ********************/
/*****************************************************************************/

void Gam_AddTstQuestionsToGame (void)
  {
   extern const char *Txt_No_questions_have_been_added;
   struct Game Game;
   const char *Ptr;
   char LongStr[1 + 10 + 1];
   long QstCod;
   unsigned MaxQstInd;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Check if game has matches *****/
   if (Gam_GetNumMchsGameAndCheckIfEditable (&Game))
     {
      /***** Get selected questions *****/
      /* Allocate space for selected question codes */
      Gam_AllocateListSelectedQuestions ();

      /* Get question codes */
      Par_GetParMultiToText ("QstCods",Gbl.Games.ListQuestions,
			     Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS);

      /* Check number of questions */
      if (Gam_CountNumQuestionsInList ())	// If questions selected...
	{
	 /***** Insert questions in database *****/
	 Ptr = Gbl.Games.ListQuestions;
	 while (*Ptr)
	   {
	    /* Get next code */
	    Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,1 + 10);
	    if (sscanf (LongStr,"%ld",&QstCod) != 1)
	       Lay_ShowErrorAndExit ("Wrong question code.");

	    /* Get current maximum index */
	    MaxQstInd = Gam_GetMaxQuestionIndexInGame (Game.GamCod);	// -1 if no questions

	    /* Insert question in the table of questions */
	    DB_QueryINSERT ("can not create question",
			    "INSERT INTO gam_questions"
			    " (GamCod,QstCod,QstInd)"
			    " VALUES"
			    " (%ld,%ld,%u)",
			    Game.GamCod,QstCod,MaxQstInd + 1);
	   }
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_No_questions_have_been_added);

      /***** Free space for selected question codes *****/
      Gam_FreeListsSelectedQuestions ();
     }
   else
      Act_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/****************** Allocate memory for list of questions ********************/
/*****************************************************************************/

static void Gam_AllocateListSelectedQuestions (void)
  {
   if (!Gbl.Games.ListQuestions)
     {
      if ((Gbl.Games.ListQuestions = (char *) malloc (Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS + 1)) == NULL)
         Lay_NotEnoughMemoryExit ();;
      Gbl.Games.ListQuestions[0] = '\0';
     }
  }

/*****************************************************************************/
/*********** Free memory used by list of selected question codes *************/
/*****************************************************************************/

static void Gam_FreeListsSelectedQuestions (void)
  {
   if (Gbl.Games.ListQuestions)
     {
      free ((void *) Gbl.Games.ListQuestions);
      Gbl.Games.ListQuestions = NULL;
     }
  }

/*****************************************************************************/
/**** Count the number of questions in the list of selected question codes ***/
/*****************************************************************************/

static unsigned Gam_CountNumQuestionsInList (void)
  {
   const char *Ptr;
   unsigned NumQuestions = 0;
   char LongStr[1 + 10 + 1];
   long QstCod;

   /***** Go over the list Gbl.Test.ListAnsTypes counting the number of types of answer *****/
   Ptr = Gbl.Games.ListQuestions;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,1 + 10);
      if (sscanf (LongStr,"%ld",&QstCod) != 1)
         Lay_ShowErrorAndExit ("Wrong question code.");
      NumQuestions++;
     }
   return NumQuestions;
  }

/*****************************************************************************/
/**************** Put parameter to move/remove one question ******************/
/*****************************************************************************/

static void Gam_PutParamsOneQst (void)
  {
   Gam_PutParams ();
   Gam_PutParamQstInd (Gam_CurrentQstInd);
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Gam_RequestRemoveQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   struct Game Game;
   unsigned QstInd;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Check if game has matches *****/
   if (Gam_GetNumMchsGameAndCheckIfEditable (&Game))
     {
      /***** Get question index *****/
      QstInd = Gam_GetParamQstInd ();

      /***** Show question and button to remove question *****/
      Gam_SetParamCurrentGamCod (Game.GamCod);	// Used to pass parameter
      Gam_CurrentQstInd = QstInd;
      Ale_ShowAlertAndButton (ActRemGamQst,NULL,NULL,Gam_PutParamsOneQst,
			      Btn_REMOVE_BUTTON,Txt_Remove_question,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			      QstInd);
     }
   else
      Act_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Gam_RemoveQst (void)
  {
   extern const char *Txt_Question_removed;
   struct Game Game;
   unsigned QstInd;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Check if game has matches *****/
   if (Gam_GetNumMchsGameAndCheckIfEditable (&Game))
     {
      /***** Get question index *****/
      QstInd = Gam_GetParamQstInd ();

      /***** Remove the question from all the tables *****/
      /* Remove answers from this test question */
      Gam_RemAnswersOfAQuestion (Game.GamCod,QstInd);

      /* Remove the question itself */
      DB_QueryDELETE ("can not remove a question",
		      "DELETE FROM gam_questions"
		      " WHERE GamCod=%ld AND QstInd=%u",
		      Game.GamCod,QstInd);
      if (!mysql_affected_rows (&Gbl.mysql))
	 Lay_ShowErrorAndExit ("The question to be removed does not exist.");

      /* Change index of questions greater than this */
      DB_QueryUPDATE ("can not update indexes of questions in table of answers",
		      "UPDATE mch_answers,mch_matches"
		      " SET mch_answers.QstInd=mch_answers.QstInd-1"
		      " WHERE mch_matches.GamCod=%ld"
		      " AND mch_matches.MchCod=mch_answers.MchCod"
		      " AND mch_answers.QstInd>%u",
		      Game.GamCod,QstInd);
      DB_QueryUPDATE ("can not update indexes of questions",
		      "UPDATE gam_questions SET QstInd=QstInd-1"
		      " WHERE GamCod=%ld AND QstInd>%u",
		      Game.GamCod,QstInd);

      /***** Write message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);
     }
   else
      Act_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/***************** Move up position of a question in a game ******************/
/*****************************************************************************/

void Gam_MoveUpQst (void)
  {
   extern const char *Txt_The_question_has_been_moved_up;
   extern const char *Txt_Movement_not_allowed;
   struct Game Game;
   unsigned QstIndTop;
   unsigned QstIndBottom;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Check if game has matches *****/
   if (Gam_GetNumMchsGameAndCheckIfEditable (&Game))
     {
      /***** Get question index *****/
      QstIndBottom = Gam_GetParamQstInd ();

      /***** Move up question *****/
      if (QstIndBottom > 1)
	{
	 /* Indexes of questions to be exchanged */
	 QstIndTop = Gam_GetPrevQuestionIndexInGame (Game.GamCod,QstIndBottom);
	 if (!QstIndTop)
	    Lay_ShowErrorAndExit ("Wrong index of question.");

	 /* Exchange questions */
	 Gam_ExchangeQuestions (Game.GamCod,QstIndTop,QstIndBottom);

	 /* Success alert */
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_question_has_been_moved_up);
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
     }
   else
      Act_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/**************** Move down position of a question in a game *****************/
/*****************************************************************************/

void Gam_MoveDownQst (void)
  {
   extern const char *Txt_The_question_has_been_moved_down;
   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_This_game_has_no_questions;
   struct Game Game;
   unsigned QstIndTop;
   unsigned QstIndBottom;
   unsigned MaxQstInd;	// 0 if no questions

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Check if game has matches *****/
   if (Gam_GetNumMchsGameAndCheckIfEditable (&Game))
     {
      /***** Get question index *****/
      QstIndTop = Gam_GetParamQstInd ();

      /***** Get maximum question index *****/
      MaxQstInd = Gam_GetMaxQuestionIndexInGame (Game.GamCod);

      /***** Move down question *****/
      if (MaxQstInd)
	{
	 if (QstIndTop < MaxQstInd)
	   {
	    /* Indexes of questions to be exchanged */
	    QstIndBottom = Gam_GetNextQuestionIndexInGame (Game.GamCod,QstIndTop);
	    if (!QstIndBottom)
	       Lay_ShowErrorAndExit ("Wrong index of question.");

	    /* Exchange questions */
	    Gam_ExchangeQuestions (Game.GamCod,QstIndTop,QstIndBottom);

	    /* Success alert */
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_question_has_been_moved_down);
	   }
	 else
	    Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_This_game_has_no_questions);
     }
   else
      Act_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/********* Exchange the order of two consecutive questions in a game *********/
/*****************************************************************************/

static void Gam_ExchangeQuestions (long GamCod,
                                   unsigned QstIndTop,unsigned QstIndBottom)
  {
   long QstCodTop;
   long QstCodBottom;

   /***** Lock table to make the move atomic *****/
   DB_Query ("can not lock tables to move game question",
	     "LOCK TABLES gam_questions WRITE");
   Gbl.DB.LockedTables = true;

   /***** Get question code of the questions to be moved *****/
   QstCodTop    = Gam_GetQstCodFromQstInd (GamCod,QstIndTop);
   QstCodBottom = Gam_GetQstCodFromQstInd (GamCod,QstIndBottom);

   /***** Exchange indexes of questions *****/
   /*
   Example:
   QstIndTop    = 1; QstCodTop    = 218
   QstIndBottom = 2; QstCodBottom = 220
   +--------+--------+		+--------+--------+	+--------+--------+
   | QstInd | QstCod |		| QstInd | QstCod |	| QstInd | QstCod |
   +--------+--------+		+--------+--------+	+--------+--------+
   |      1 |    218 |  ----->	|      2 |    218 |  =	|      1 |    220 |
   |      2 |    220 |		|      1 |    220 |	|      2 |    218 |
   |      3 |    232 |		|      3 |    232 |	|      3 |    232 |
   +--------+--------+		+--------+--------+	+--------+--------+
 */
   DB_QueryUPDATE ("can not exchange indexes of questions",
		   "UPDATE gam_questions SET QstInd=%u"
		   " WHERE GamCod=%ld AND QstCod=%ld",
	           QstIndBottom,
	           GamCod,QstCodTop);

   DB_QueryUPDATE ("can not exchange indexes of questions",
		   "UPDATE gam_questions SET QstInd=%u"
		   " WHERE GamCod=%ld AND QstCod=%ld",
	           QstIndTop,
	           GamCod,QstCodBottom);

   /***** Unlock table *****/
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after moving game questions",
	     "UNLOCK TABLES");
  }


/*****************************************************************************/
/*********** Get number of matches and check is edition is possible **********/
/*****************************************************************************/
// Games with matches should not be edited

static bool Gam_GetNumMchsGameAndCheckIfEditable (struct Game *Game)
  {
   /***** Get number of matches *****/
   Game->NumMchs = Mch_GetNumMchsInGame (Game->GamCod);

   if (Gam_CheckIfICanEditGames ())
      /***** Questions are editable only if game has no matches *****/
      return (bool) (Game->NumMchs == 0);
   else
      return false;	// Questions are not editable
  }

/*****************************************************************************/
/********************* Put button to create a new match **********************/
/*****************************************************************************/

void Gam_PutButtonNewMatch (long GamCod)
  {
   extern const char *Txt_New_match;

   Gam_SetParamCurrentGamCod (GamCod);	// Used to pass parameter
   Frm_StartFormAnchor (ActReqNewMch,Mch_NEW_MATCH_SECTION_ID);
   Gam_PutParams ();
   Btn_PutConfirmButton (Txt_New_match);
   Frm_EndForm ();
  }

/*****************************************************************************/
/************* Request the creation of a new match as a teacher **************/
/*****************************************************************************/

void Gam_RequestNewMatch (void)
  {
   long GamCod;

   /***** Get parameters *****/
   if ((GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Show game *****/
   Gam_ShowOneGame (GamCod,
                    true,	// Show only this game
                    false,	// Do not list game questions
                    true);	// Put form to start new match
  }

/*****************************************************************************/
/********************* Get number of courses with games **********************/
/*****************************************************************************/
// Returns the number of courses with games in this location

unsigned Gam_GetNumCoursesWithGames (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with games from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM gam_games");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			 " FROM institutions,centres,degrees,courses,gam_games"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			 " FROM centres,degrees,courses,gam_games"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			 " FROM degrees,courses,gam_games"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			 " FROM courses,gam_games"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM gam_games"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of games *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with games.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/**************************** Get number of games ****************************/
/*****************************************************************************/
// Returns the number of games in this location

unsigned Gam_GetNumGames (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGames;

   /***** Get number of games from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM gam_games");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM institutions,centres,degrees,courses,gam_games"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM centres,degrees,courses,gam_games"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM degrees,courses,gam_games"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM courses,gam_games"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM gam_games"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of games *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumGames) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of games.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumGames;
  }

/*****************************************************************************/
/************* Get average number of questions per course game ***************/
/*****************************************************************************/

float Gam_GetNumQstsPerCrsGame (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   float NumQstsPerGame;

   /***** Get number of questions per game from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM gam_games,gam_questions"
			 " WHERE gam_games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM institutions,centres,degrees,courses,gam_games,gam_questions"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod"
			 " AND gam_games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM centres,degrees,courses,gam_games,gam_questions"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod"
			 " AND gam_games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
		         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM degrees,courses,gam_games,gam_questions"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=gam_games.CrsCod"
			 " AND gam_games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM courses,gam_games,gam_questions"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=gam_games.CrsCod"
			 " AND gam_games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
		         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM gam_games,gam_questions"
			 " WHERE gam_games.Cod=%ld"
			 " AND gam_games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   NumQstsPerGame = Str_GetFloatNumFromStr (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumQstsPerGame;
  }

/*****************************************************************************/
/************************* Show test tags in a game **************************/
/*****************************************************************************/

void Gam_ShowTstTagsPresentInAGame (long GamCod)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumTags;

   /***** Get all tags of questions in this game *****/
   NumTags = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get tags"
					" present in a match result",
			     "SELECT tst_tags.TagTxt"	// row[0]
			     " FROM"
			     " (SELECT DISTINCT(tst_question_tags.TagCod)"
			     " FROM tst_question_tags,gam_questions"
			     " WHERE gam_questions.GamCod=%ld"
			     " AND gam_questions.QstCod=tst_question_tags.QstCod)"
			     " AS TagsCods,tst_tags"
			     " WHERE TagsCods.TagCod=tst_tags.TagCod"
			     " ORDER BY tst_tags.TagTxt",
			     GamCod);
   Tst_ShowTagList (NumTags,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** Access to variable used to pass parameter ******************/
/*****************************************************************************/

void Gam_SetParamCurrentGamCod (long GamCod)
  {
   Gam_CurrentGamCod = GamCod;
  }

static long Gam_GetParamCurrentGamCod (void)
  {
   return Gam_CurrentGamCod;
  }
