// swad_game.c: games using remote control

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
#include <float.h>		// For DBL_MAX
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_match.h"
#include "swad_match_result.h"
#include "swad_pagination.h"
#include "swad_role.h"
#include "swad_test.h"
#include "swad_test_visibility.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Gam_MAX_CHARS_ANSWER	(1024 - 1)	// 1023
#define Gam_MAX_BYTES_ANSWER	((Gam_MAX_CHARS_ANSWER + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Gam_MAX_ANSWERS_PER_QUESTION	10

#define Gam_MAX_SELECTED_QUESTIONS		1000
#define Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS	(Gam_MAX_SELECTED_QUESTIONS * (Cns_MAX_DECIMAL_DIGITS_LONG + 1))

/* Score range [0...max.score]
   will be converted to
   grade range [0...max.grade]
   Example: Game with 5 questions, unique-choice, 4 options per question
            max.score = 5 *   1     =  5
            min.score = 5 * (-0.33) = -1,67
            max.grade given by teacher = 0.2 ==> min.grade = -0,067

              grade
                ^
                |          /
   max.grade--> +---------+
                |        /|
                |       / |
                |      /  |
                |     /   |
                |    /    |
                |   /     |
                |  /      |
                | /       |
                |/        |
    ------+---0-+---------+---------> score
          ^    /0         ^
     min.score/ |      max.score
          |  /  |   (num.questions)
          | /   |
          |/    |
          +-----+ <--min.grade
         /      |
*/
#define Gam_MAX_GRADE_DEFAULT 1.0

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Gam_ListAllGames (struct Gam_Games *Games);
static bool Gam_CheckIfICanEditGames (void);
static void Gam_PutIconsListGames (void *Games);
static void Gam_PutIconToCreateNewGame (struct Gam_Games *Games);
static void Gam_PutButtonToCreateNewGame (struct Gam_Games *Games);
static void Gam_PutParamsToCreateNewGame (void *Games);

static void Gam_ShowOneGame (struct Gam_Games *Games,
                             struct Gam_Game *Game,bool ShowOnlyThisGame);

static void Gam_PutIconToShowResultsOfGame (void *Games);
static void Gam_WriteAuthor (struct Gam_Game *Game);

static void Gam_PutHiddenParamGameOrder (Gam_Order_t SelectedOrder);

static void Gam_PutFormsToRemEditOneGame (struct Gam_Games *Games,
					  const struct Gam_Game *Game,
					  const char *Anchor);

static void Gam_PutParamsOneQst (void *Games);
static void Gam_PutHiddenParamOrder (Gam_Order_t SelectedOrder);
static Gam_Order_t Gam_GetParamOrder (void);

static void Gam_ResetGame (struct Gam_Game *Game);

static void Gam_GetGameTxtFromDB (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

static void Gam_RemoveGameFromAllTables (long GamCod);

static bool Gam_CheckIfSimilarGameExists (const struct Gam_Game *Game);

static void Gam_PutFormsEditionGame (struct Gam_Games *Games,
				     struct Gam_Game *Game,
				     char Txt[Cns_MAX_BYTES_TEXT + 1],
				     bool ItsANewGame);
static void Gam_ReceiveGameFieldsFromForm (struct Gam_Game *Game,
				           char Txt[Cns_MAX_BYTES_TEXT + 1]);
static bool Gam_CheckGameFieldsReceivedFromForm (const struct Gam_Game *Game);

static void Gam_CreateGame (struct Gam_Game *Game,const char *Txt);
static void Gam_UpdateGame (struct Gam_Game *Game,const char *Txt);

static void Gam_RemAnswersOfAQuestion (long GamCod,unsigned QstInd);

static unsigned Gam_GetMaxQuestionIndexInGame (long GamCod);
static void Gam_ListGameQuestions (struct Gam_Games *Games,struct Gam_Game *Game);
static void Gam_ListOneOrMoreQuestionsForEdition (struct Gam_Games *Games,
						  long GamCod,unsigned NumQsts,
                                                  MYSQL_RES *mysql_res,
						  bool ICanEditQuestions);
static void Gam_ListQuestionForEdition (const struct Tst_Question *Question,
                                        unsigned QstInd,bool QuestionExists);
static void Gam_PutIconToAddNewQuestions (void *Games);
static void Gam_PutButtonToAddNewQuestions (struct Gam_Games *Games);

static void Gam_AllocateListSelectedQuestions (struct Gam_Games *Games);
static void Gam_FreeListsSelectedQuestions (struct Gam_Games *Games);
static unsigned Gam_CountNumQuestionsInList (const struct Gam_Games *Games);

static void Gam_ExchangeQuestions (long GamCod,
                                   unsigned QstIndTop,unsigned QstIndBottom);

static bool Gam_CheckIfEditable (const struct Gam_Game *Game);

/*****************************************************************************/
/******************************* Reset games *********************************/
/*****************************************************************************/

void Gam_ResetGames (struct Gam_Games *Games)
  {
   Games->LstIsRead         = false;	// List not read from database...
   Games->Num               = 0;	// Total number of games
   Games->NumSelected       = 0;	// Number of games selected
   Games->Lst               = NULL;	// List of games
   Games->SelectedOrder     = Gam_ORDER_DEFAULT;
   Games->CurrentPage       = 0;
   Games->ListQuestions     = NULL;
   Games->GamCodsSelected   = NULL;	// String with selected game codes separated by separator multiple
   Games->GamCod            = -1L;	// Selected/current game code
   Games->MchCod            = -1L;	// Selected/current match code
   Games->QstInd            = 0;	// Current question index
  }

/*****************************************************************************/
/***************************** List all games ********************************/
/*****************************************************************************/

void Gam_SeeAllGames (void)
  {
   struct Gam_Games Games;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   Gam_GetParams (&Games);	// Return value ignored

   /***** Show all games *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/**************************** Show all the games *****************************/
/*****************************************************************************/

static void Gam_ListAllGames (struct Gam_Games *Games)
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
   struct Gam_Game Game;

   /***** Get number of groups in current course *****/
   if (!Gbl.Crs.Grps.NumGrps)
      Gbl.Crs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of games *****/
   Gam_GetListGames (Games,Games->SelectedOrder);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Games->Num;
   Pagination.CurrentPage = (int) Games->CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Games->CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Games,
                 Gam_PutIconsListGames,Games,
                 Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

   /***** Write links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_GAMES,
				  &Pagination,
				  (unsigned) Games->SelectedOrder,
				  NULL,0);

   if (Games->Num)
     {
      /***** Table head *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      HTM_TR_Begin (NULL);
      if (Gam_CheckIfICanEditGames ())
         HTM_TH (1,1,"CONTEXT_COL",NULL);	// Column for contextual icons

      for (Order  = (Gam_Order_t) 0;
	   Order <= (Gam_Order_t) (Gam_NUM_ORDERS - 1);
	   Order++)
	{
	 HTM_TH_Begin (1,1,"LM");

	 /* Form to change order */
	 Frm_StartForm (ActSeeAllGam);
	 Pag_PutHiddenParamPagNum (Pag_GAMES,Games->CurrentPage);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_GAMES_ORDER_HELP[Order],"BT_LINK TIT_TBL",NULL);
	 if (Order == Games->SelectedOrder)
	    HTM_U_Begin ();
	 HTM_Txt (Txt_GAMES_ORDER[Order]);
	 if (Order == Games->SelectedOrder)
	    HTM_U_End ();
	 HTM_BUTTON_End ();
	 Frm_EndForm ();

	 HTM_TH_End ();
	}

      HTM_TH (1,1,"RM",Txt_Matches);

      HTM_TR_End ();

      /***** Write all games *****/
      for (NumGame  = Pagination.FirstItemVisible;
	   NumGame <= Pagination.LastItemVisible;
	   NumGame++)
	{
	 /* Get data of this game */
	 Game.GamCod = Games->Lst[NumGame - 1].GamCod;
	 Gam_GetDataOfGameByCod (&Game);

	 /* Show game */
	 Gam_ShowOneGame (Games,
	                  &Game,
	                  false);	// Do not show only this game
	}

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No games created
      Ale_ShowAlert (Ale_INFO,Txt_No_games);

   /***** Write again links to pages *****/
   Pag_WriteLinksToPagesCentered (Pag_GAMES,
				  &Pagination,
				  (unsigned) Games->SelectedOrder,
				  NULL,0);

   /***** Button to create a new game *****/
   if (Gam_CheckIfICanEditGames ())
      Gam_PutButtonToCreateNewGame (Games);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of games *****/
   Gam_FreeListGames (Games);
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

static void Gam_PutIconsListGames (void *Games)
  {
   if (Games)
     {
      /***** Put icon to create a new game *****/
      if (Gam_CheckIfICanEditGames ())
	 Gam_PutIconToCreateNewGame ((struct Gam_Games *) Games);

      /***** Put icon to view matches results *****/
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:
	    Ico_PutContextualIconToShowResults (ActSeeMyMchResCrs,NULL,
	                                        NULL,NULL);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Ico_PutContextualIconToShowResults (ActReqSeeAllMchRes,NULL,
	                                        NULL,NULL);
	    break;
	 default:
	    break;
	}

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_GAMES);
     }
  }

/*****************************************************************************/
/********************** Put icon to create a new game **********************/
/*****************************************************************************/

static void Gam_PutIconToCreateNewGame (struct Gam_Games *Games)
  {
   extern const char *Txt_New_game;

   Ico_PutContextualIconToAdd (ActFrmNewGam,NULL,
                               Gam_PutParamsToCreateNewGame,Games,
			       Txt_New_game);
  }

/*****************************************************************************/
/********************* Put button to create a new game *********************/
/*****************************************************************************/

static void Gam_PutButtonToCreateNewGame (struct Gam_Games *Games)
  {
   extern const char *Txt_New_game;

   Frm_StartForm (ActFrmNewGam);
   Gam_PutParamsToCreateNewGame (Games);
   Btn_PutConfirmButton (Txt_New_game);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Put parameters to create a new game *******************/
/*****************************************************************************/

static void Gam_PutParamsToCreateNewGame (void *Games)
  {
   if (Games)
     {
      Gam_PutHiddenParamGameOrder (((struct Gam_Games *) Games)->SelectedOrder);
      Pag_PutHiddenParamPagNum (Pag_GAMES,((struct Gam_Games *) Games)->CurrentPage);
     }
  }

/*****************************************************************************/
/****************************** Show one game ******************************/
/*****************************************************************************/

void Gam_SeeOneGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        false,	// Do not list game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/******************************* Show one game *******************************/
/*****************************************************************************/

void Gam_ShowOnlyOneGame (struct Gam_Games *Games,
			  struct Gam_Game *Game,
			  bool ListGameQuestions,
			  bool PutFormNewMatch)
  {
   Gam_ShowOnlyOneGameBegin (Games,Game,ListGameQuestions,PutFormNewMatch);
   Gam_ShowOnlyOneGameEnd ();
  }

void Gam_ShowOnlyOneGameBegin (struct Gam_Games *Games,
			       struct Gam_Game *Game,
			       bool ListGameQuestions,
			       bool PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Game;

   /***** Begin box *****/
   Games->GamCod = Game->GamCod;
   Box_BoxBegin (NULL,Txt_Game,
                 Gam_PutIconToShowResultsOfGame,Games,
		 Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

   /***** Show game *****/
   Gam_ShowOneGame (Games,
                    Game,
		    true);	// Show only this game

   if (ListGameQuestions)
       /***** Write questions of this game *****/
      Gam_ListGameQuestions (Games,Game);
   else
      /***** List matches *****/
      Mch_ListMatches (Games,Game,PutFormNewMatch);
  }

void Gam_ShowOnlyOneGameEnd (void)
  {
   /***** End box *****/
   Box_BoxEnd ();
  }

static void Gam_ShowOneGame (struct Gam_Games *Games,
                             struct Gam_Game *Game,bool ShowOnlyThisGame)
  {
   extern const char *Txt_View_game;
   extern const char *Txt_No_of_questions;
   extern const char *Txt_Maximum_grade;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Matches;
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   Dat_StartEndTime_t StartEndTime;
   const char *Color;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Game->GamCod,&Anchor);

   /***** Begin box and table *****/
   if (ShowOnlyThisGame)
      HTM_TABLE_BeginWidePadding (2);

   /***** Start first row of this game *****/
   HTM_TR_Begin (NULL);

   /***** Icons related to this game *****/
   if (Gam_CheckIfICanEditGames ())
     {
      if (ShowOnlyThisGame)
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL\"");
      else
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL COLOR%u\"",Gbl.RowEvenOdd);

      /* Icons to remove/edit this game */
      Gam_PutFormsToRemEditOneGame (Games,Game,Anchor);

      HTM_TD_End ();
     }

   /***** Start/end date/time *****/
   UniqueId++;
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"gam_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      Color = Game->NumUnfinishedMchs ? (Game->Hidden ? "DATE_GREEN_LIGHT":
							"DATE_GREEN") :
					(Game->Hidden ? "DATE_RED_LIGHT":
							"DATE_RED");
      if (ShowOnlyThisGame)
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT\"",
		       Id,Color);
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		       Id,Color,Gbl.RowEvenOdd);
      if (Game->TimeUTC[Dat_START_TIME])
	 Dat_WriteLocalDateHMSFromUTC (Id,Game->TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,true,0x7);
      HTM_TD_End ();
      free (Id);
     }

   /***** Game title and main data *****/
   if (ShowOnlyThisGame)
      HTM_TD_Begin ("class=\"LT\"");
   else
      HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /* Game title */
   Games->GamCod = Game->GamCod;
   HTM_ARTICLE_Begin (Anchor);
   Frm_StartForm (ActSeeGam);
   Gam_PutParams (Games);
   HTM_BUTTON_SUBMIT_Begin (Txt_View_game,
			    Game->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT":
					   "BT_LINK LT ASG_TITLE",
			    NULL);
   HTM_Txt (Game->Title);
   HTM_BUTTON_End ();
   Frm_EndForm ();
   HTM_ARTICLE_End ();

   /* Number of questions, maximum grade, visibility of results */
   HTM_DIV_Begin ("class=\"%s\"",Game->Hidden ? "ASG_GRP_LIGHT" :
        	                                "ASG_GRP");
   HTM_TxtColonNBSP (Txt_No_of_questions);
   HTM_Unsigned (Game->NumQsts);
   HTM_BR ();
   HTM_TxtColonNBSP (Txt_Maximum_grade);
   HTM_Double (Game->MaxGrade);
   HTM_BR ();
   HTM_TxtColonNBSP (Txt_Result_visibility);
   TstVis_ShowVisibilityIcons (Game->Visibility,Game->Hidden);
   HTM_DIV_End ();

   /***** Number of matches in game *****/
   if (ShowOnlyThisGame)
      HTM_TD_Begin ("class=\"RT\"");
   else
      HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);

   Games->GamCod = Game->GamCod;
   Frm_StartForm (ActSeeGam);
   Gam_PutParams (Games);
   HTM_BUTTON_SUBMIT_Begin (Txt_Matches,
			    Game->Hidden ? "BT_LINK LT ASG_TITLE_LIGHT" :
				           "BT_LINK LT ASG_TITLE",
			    NULL);
   if (ShowOnlyThisGame)
      HTM_TxtColonNBSP (Txt_Matches);
   HTM_Unsigned (Game->NumMchs);
   HTM_BUTTON_End ();
   Frm_EndForm ();

   HTM_TD_End ();

   /***** End 1st row of this game *****/
   HTM_TR_End ();

   /***** Start 2nd row of this game *****/
   HTM_TR_Begin (NULL);

   /***** Author of the game *****/
   if (ShowOnlyThisGame)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Gam_WriteAuthor (Game);
   HTM_TD_End ();

   /***** Text of the game *****/
   if (ShowOnlyThisGame)
      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
   else
      HTM_TD_Begin ("colspan=\"2\" class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Gam_GetGameTxtFromDB (Game->GamCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   HTM_DIV_Begin ("class=\"PAR %s\"",Game->Hidden ? "DAT_LIGHT" :
        	                                    "DAT");
   HTM_Txt (Txt);
   HTM_DIV_End ();
   HTM_TD_End ();

   /***** End 2nd row of this game *****/
   HTM_TR_End ();

   /***** End table *****/
   if (ShowOnlyThisGame)
      HTM_TABLE_End ();
   else
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
  }

/*****************************************************************************/
/************* Put icon to show results of matches in a game *****************/
/*****************************************************************************/

static void Gam_PutIconToShowResultsOfGame (void *Games)
  {
   if (Games)
     {
      /***** Put icon to view matches results *****/
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:
	    Ico_PutContextualIconToShowResults (ActSeeMyMchResGam,MchRes_RESULTS_BOX_ID,
						Gam_PutParams,Games);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Ico_PutContextualIconToShowResults (ActSeeAllMchResGam,MchRes_RESULTS_BOX_ID,
						Gam_PutParams,Games);
	    break;
	 default:
	    break;
	}
     }
  }

/*****************************************************************************/
/*********************** Write the author of a game ************************/
/*****************************************************************************/

static void Gam_WriteAuthor (struct Gam_Game *Game)
  {
   Usr_WriteAuthor1Line (Game->UsrCod,Game->Hidden);
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of games *******/
/*****************************************************************************/

static void Gam_PutHiddenParamGameOrder (Gam_Order_t SelectedOrder)
  {
   Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) SelectedOrder);
  }

/*****************************************************************************/
/******************** Put a link (form) to edit one game *********************/
/*****************************************************************************/

static void Gam_PutFormsToRemEditOneGame (struct Gam_Games *Games,
					  const struct Gam_Game *Game,
					  const char *Anchor)
  {
   Games->GamCod = Game->GamCod;

   /***** Put icon to remove game *****/
   Ico_PutContextualIconToRemove (ActReqRemGam,
                                  Gam_PutParams,Games);

   /***** Put icon to unhide/hide game *****/
   if (Game->Hidden)
      Ico_PutContextualIconToUnhide (ActShoGam,Anchor,
                                     Gam_PutParams,Games);
   else
      Ico_PutContextualIconToHide (ActHidGam,Anchor,
                                   Gam_PutParams,Games);

   /***** Put icon to edit game *****/
   Ico_PutContextualIconToEdit (ActEdiOneGam,NULL,
                                Gam_PutParams,Games);
  }

/*****************************************************************************/
/**************** Put parameter to move/remove one question ******************/
/*****************************************************************************/

static void Gam_PutParamsOneQst (void *Games)
  {
   if (Games)
     {
      Gam_PutParams (Games);
      Gam_PutParamQstInd (((struct Gam_Games *) Games)->QstInd);
     }
  }

/*****************************************************************************/
/*********************** Params used to edit a game **************************/
/*****************************************************************************/

void Gam_PutParams (void *Games)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Games)
     {
      if (((struct Gam_Games *) Games)->GamCod > 0)
	 Gam_PutParamGameCod (((struct Gam_Games *) Games)->GamCod);
      Gam_PutHiddenParamOrder (((struct Gam_Games *) Games)->SelectedOrder);
      WhichGroups = Grp_GetParamWhichGroups ();
      Grp_PutParamWhichGroups (&WhichGroups);
      Pag_PutHiddenParamPagNum (Pag_GAMES,((struct Gam_Games *) Games)->CurrentPage);
     }
  }

/*****************************************************************************/
/******************** Write parameter with code of game **********************/
/*****************************************************************************/

void Gam_PutParamGameCod (long GamCod)
  {
   Par_PutHiddenParamLong (NULL,"GamCod",GamCod);
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
/******************* Get parameters used to edit a game **********************/
/*****************************************************************************/

long Gam_GetParams (struct Gam_Games *Games)
  {
   /***** Get other parameters *****/
   Games->SelectedOrder = Gam_GetParamOrder ();
   Games->CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   return Gam_GetParamGameCod ();
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of games *******/
/*****************************************************************************/

static void Gam_PutHiddenParamOrder (Gam_Order_t SelectedOrder)
  {
   if (SelectedOrder != Gam_ORDER_DEFAULT)
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) SelectedOrder);
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of games ************/
/*****************************************************************************/

static Gam_Order_t Gam_GetParamOrder (void)
  {
   return (Gam_Order_t) Par_GetParToUnsignedLong ("Order",
						  0,
						  Gam_NUM_ORDERS - 1,
						  (unsigned long) Gam_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*********************** Get list of all the games *************************/
/*****************************************************************************/

void Gam_GetListGames (struct Gam_Games *Games,Gam_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Gam_NUM_ORDERS] =
     {
      [Gam_ORDER_BY_START_DATE] = "StartTime DESC,EndTime DESC,gam_games.Title DESC",
      [Gam_ORDER_BY_END_DATE  ] = "EndTime DESC,StartTime DESC,gam_games.Title DESC",
      [Gam_ORDER_BY_TITLE     ] = "gam_games.Title",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char *HiddenSubQuery;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned NumGame;

   /***** Free list of games *****/
   if (Games->LstIsRead)
      Gam_FreeListGames (Games);

   /***** Subquery: get hidden games depending on user's role *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
         if (asprintf (&HiddenSubQuery," AND gam_games.Hidden='N'") < 0)
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
			     OrderBySubQuery[SelectedOrder]);

   /***** Free allocated memory for subquery *****/
   free (HiddenSubQuery);

   if (NumRows) // Games found...
     {
      Games->Num = (unsigned) NumRows;

      /***** Create list of games *****/
      if ((Games->Lst = (struct Gam_GameSelected *) malloc (NumRows * sizeof (struct Gam_GameSelected))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the games codes *****/
      for (NumGame = 0;
	   NumGame < Games->Num;
	   NumGame++)
        {
         /* Get next game code (row[0]) */
         row = mysql_fetch_row (mysql_res);
         if ((Games->Lst[NumGame].GamCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Lay_ShowErrorAndExit ("Error: wrong game code.");
        }
     }
   else
      Games->Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Games->LstIsRead = true;
  }

/*****************************************************************************/
/********************* Get list of game events selected **********************/
/*****************************************************************************/

void Gam_GetListSelectedGamCods (struct Gam_Games *Games)
  {
   unsigned MaxSizeListGamCodsSelected;
   unsigned NumGame;
   const char *Ptr;
   long GamCod;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];

   /***** Allocate memory for list of games selected *****/
   MaxSizeListGamCodsSelected = Games->Num * (Cns_MAX_DECIMAL_DIGITS_LONG + 1);
   if ((Games->GamCodsSelected = (char *) malloc (MaxSizeListGamCodsSelected + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Get parameter multiple with list of games selected *****/
   Par_GetParMultiToText ("GamCod",Games->GamCodsSelected,MaxSizeListGamCodsSelected);

   /***** Set which games will be shown as selected (checkboxes on) *****/
   if (Games->GamCodsSelected[0])	// Some games selected
     {
      /* Reset selection */
      for (NumGame = 0;
	   NumGame < Games->Num;
	   NumGame++)
	 Games->Lst[NumGame].Selected = false;
      Games->NumSelected = 0;

      /* Set some games as selected */
      for (Ptr = Games->GamCodsSelected;
	   *Ptr;
	   )
	{
	 /* Get next game selected */
	 Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 GamCod = Str_ConvertStrCodToLongCod (LongStr);

	 /* Set each game in *StrGamCodsSelected as selected */
	 for (NumGame = 0;
	      NumGame < Games->Num;
	      NumGame++)
	    if (Games->Lst[NumGame].GamCod == GamCod)
	      {
	       Games->Lst[NumGame].Selected = true;
	       Games->NumSelected++;
	       break;
	      }
	}
     }
   else					// No games selected
     {
      /***** Set all games as selected *****/
      for (NumGame = 0;
	   NumGame < Games->Num;
	   NumGame++)
	 Games->Lst[NumGame].Selected = true;
      Games->NumSelected = Games->Num;
     }
  }

/*****************************************************************************/
/********************** Get game data using its code *************************/
/*****************************************************************************/

void Gam_GetDataOfGameByCod (struct Gam_Game *Game)
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
			            "gam_games.MaxGrade,"	// row[4]
			            "gam_games.Visibility,"	// row[5]
			            "gam_games.Title"		// row[6]
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

      /* Get maximum grade (row[4]) */
      Game->MaxGrade = Str_GetDoubleFromStr (row[4]);
      if (Game->MaxGrade < 0.0)	// Only positive values allowed
	 Game->MaxGrade = 0.0;

      /* Get visibility (row[5]) */
      Game->Visibility = TstVis_GetVisibilityFromStr (row[5]);

      /* Get the title of the game (row[6]) */
      Str_Copy (Game->Title,row[6],
                Gam_MAX_BYTES_TITLE);

      /* Get number of questions */
      Game->NumQsts = Gam_GetNumQstsGame (Game->GamCod);

      /* Get number of matches */
      Game->NumMchs = Mch_GetNumMchsInGame (Game->GamCod);

      /* Get number of unfinished matches */
      Game->NumUnfinishedMchs = Mch_GetNumUnfinishedMchsInGame (Game->GamCod);
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

static void Gam_ResetGame (struct Gam_Game *Game)
  {
   /***** Initialize to empty game *****/
   Game->GamCod                  = -1L;
   Game->CrsCod                  = -1L;
   Game->UsrCod                  = -1L;
   Game->MaxGrade                = Gam_MAX_GRADE_DEFAULT;
   Game->Visibility              = TstVis_VISIBILITY_DEFAULT;
   Game->TimeUTC[Dat_START_TIME] = (time_t) 0;
   Game->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Game->Title[0]                = '\0';
   Game->NumQsts                 = 0;
   Game->NumMchs                 = 0;
   Game->NumUnfinishedMchs       = 0;
   Game->Hidden                  = false;
  }

/*****************************************************************************/
/***************************** Free list of games ****************************/
/*****************************************************************************/

void Gam_FreeListGames (struct Gam_Games *Games)
  {
   if (Games->LstIsRead && Games->Lst)
     {
      /***** Free memory used by the list of games *****/
      free (Games->Lst);
      Games->Lst       = NULL;
      Games->Num       = 0;
      Games->LstIsRead = false;
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
/*************** Ask for confirmation of removing of a game ******************/
/*****************************************************************************/

void Gam_AskRemGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_game_X;
   extern const char *Txt_Remove_game;
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Lay_NoPermissionExit ();

   /***** Show question and button to remove game *****/
   Games.GamCod = Game.GamCod;
   Ale_ShowAlertAndButton (ActRemGam,NULL,NULL,
                           Gam_PutParams,&Games,
			   Btn_REMOVE_BUTTON,Txt_Remove_game,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_game_X,
                           Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/******************************* Remove a game *******************************/
/*****************************************************************************/

void Gam_RemoveGame (void)
  {
   extern const char *Txt_Game_X_removed;
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Lay_NoPermissionExit ();

   /***** Remove game from all tables *****/
   Gam_RemoveGameFromAllTables (Game.GamCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Game_X_removed,
                  Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
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
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Lay_NoPermissionExit ();

   /***** Hide game *****/
   DB_QueryUPDATE ("can not hide game",
		   "UPDATE gam_games SET Hidden='Y' WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/******************************** Show a game ******************************/
/*****************************************************************************/

void Gam_UnhideGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Lay_NoPermissionExit ();

   /***** Show game *****/
   DB_QueryUPDATE ("can not show game",
		   "UPDATE gam_games SET Hidden='N' WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/******************* Check if the title of a game exists *******************/
/*****************************************************************************/

static bool Gam_CheckIfSimilarGameExists (const struct Gam_Game *Game)
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
   struct Gam_Games Games;
   struct Gam_Game Game;
   bool ItsANewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Check if I can edit games *****/
   if (!Gam_CheckIfICanEditGames ())
      Lay_NoPermissionExit ();

   /***** Get parameters *****/
   ItsANewGame = ((Game.GamCod = Gam_GetParams (&Games)) <= 0);

   /***** Get game data *****/
   if (ItsANewGame)
     {
      /* Initialize to empty game */
      Gam_ResetGame (&Game);
      Txt[0] = '\0';
     }
   else
     {
      /* Get game data from database */
      Gam_GetDataOfGameByCod (&Game);
      Gam_GetGameTxtFromDB (Game.GamCod,Txt);
     }

   /***** Put forms to create/edit a game *****/
   Gam_PutFormsEditionGame (&Games,&Game,Txt,ItsANewGame);

   /***** Show games or questions *****/
   if (ItsANewGame)
      /* Show games again */
      Gam_ListAllGames (&Games);
   else
      /* Show questions of the game ready to be edited */
      Gam_ListGameQuestions (&Games,&Game);

  }

/*****************************************************************************/
/********************* Put a form to create/edit a game **********************/
/*****************************************************************************/

static void Gam_PutFormsEditionGame (struct Gam_Games *Games,
				     struct Gam_Game *Game,
				     char Txt[Cns_MAX_BYTES_TEXT + 1],
				     bool ItsANewGame)
  {
   extern const char *Hlp_ASSESSMENT_Games_new_game;
   extern const char *Hlp_ASSESSMENT_Games_edit_game;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_game;
   extern const char *Txt_Edit_game;
   extern const char *Txt_Title;
   extern const char *Txt_Maximum_grade;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Description;
   extern const char *Txt_Create_game;
   extern const char *Txt_Save_changes;

   /***** Begin form *****/
   Games->GamCod = Game->GamCod;
   Frm_StartForm (ItsANewGame ? ActNewGam :
				ActChgGam);
   Gam_PutParams (Games);

   /***** Begin box and table *****/
   if (ItsANewGame)
      Box_BoxTableBegin (NULL,Txt_New_game,
                         NULL,NULL,
			 Hlp_ASSESSMENT_Games_new_game,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,
			 Game->Title[0] ? Game->Title :
					  Txt_Edit_game,
			 NULL,NULL,
			 Hlp_ASSESSMENT_Games_edit_game,Box_NOT_CLOSABLE,2);

   /***** Game title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_INPUT_TEXT ("Title",Gam_MAX_CHARS_TITLE,Game->Title,false,
		   "id=\"Title\" required=\"required\""
		   " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Maximum grade *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Maximum_grade);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_FLOAT ("MaxGrade",0.0,DBL_MAX,0.01,Game->MaxGrade,false,
		    "required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Visibility of results *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Result_visibility);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LB\"");
   TstVis_PutVisibilityCheckboxes (Game->Visibility);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Game text *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Txt",Txt_Description);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"5\""
	               " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_Txt (Txt);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   if (ItsANewGame)
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_game);
   else
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************** Receive form to create a new game ********************/
/*****************************************************************************/

void Gam_RecFormGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;
   bool ItsANewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Check if I can edit games *****/
   if (!Gam_CheckIfICanEditGames ())
      Lay_NoPermissionExit ();

   /***** Get parameters *****/
   ItsANewGame = ((Game.GamCod = Gam_GetParams (&Games)) <= 0);

   /***** If I can edit games ==> receive game from form *****/
   if (Gam_CheckIfICanEditGames ())
     {
      Gam_ReceiveGameFieldsFromForm (&Game,Txt);
      if (Gam_CheckGameFieldsReceivedFromForm (&Game))
	{
         /***** Create a new game or update an existing one *****/
	 if (ItsANewGame)
	    Gam_CreateGame (&Game,Txt);	// Add new game to database
	 else
	    Gam_UpdateGame (&Game,Txt);	// Update game data in database

         /***** Put forms to edit the game created or updated *****/
         Gam_PutFormsEditionGame (&Games,&Game,Txt,
                                  false);	// No new game

         /***** Show questions of the game ready to be edited ******/
         Gam_ListGameQuestions (&Games,&Game);
	}
      else
	{
         /***** Put forms to create/edit the game *****/
         Gam_PutFormsEditionGame (&Games,&Game,Txt,ItsANewGame);

         /***** Show games or questions *****/
         if (ItsANewGame)
            /* Show games again */
            Gam_ListAllGames (&Games);
         else
            /* Show questions of the game ready to be edited */
            Gam_ListGameQuestions (&Games,&Game);
	}
     }
   else
      Lay_NoPermissionExit ();
  }

static void Gam_ReceiveGameFieldsFromForm (struct Gam_Game *Game,
				           char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   char MaxGradeStr[64];

   /***** Get game title *****/
   Par_GetParToText ("Title",Game->Title,Gam_MAX_BYTES_TITLE);

   /***** Get maximum grade *****/
   Par_GetParToText ("MaxGrade",MaxGradeStr,sizeof (MaxGradeStr) - 1);
   Game->MaxGrade = Str_GetDoubleFromStr (MaxGradeStr);
   if (Game->MaxGrade < 0.0)	// Only positive values allowed
      Game->MaxGrade = 0.0;

   /***** Get visibility *****/
   Game->Visibility = TstVis_GetVisibilityFromForm ();

   /***** Get game text *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
  }

static bool Gam_CheckGameFieldsReceivedFromForm (const struct Gam_Game *Game)
  {
   extern const char *Txt_Already_existed_a_game_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_game;
   bool NewGameIsCorrect;

   /***** Check if title is correct *****/
   NewGameIsCorrect = true;
   if (Game->Title[0])	// If there's a game title
     {
      /* If title of game was in database... */
      if (Gam_CheckIfSimilarGameExists (Game))
	{
	 NewGameIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_game_with_the_title_X,
			Game->Title);
	}
     }
   else	// If there is not a game title
     {
      NewGameIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_game);
     }

   return NewGameIsCorrect;
  }

/*****************************************************************************/
/**************************** Create a new game ******************************/
/*****************************************************************************/

static void Gam_CreateGame (struct Gam_Game *Game,const char *Txt)
  {
   extern const char *Txt_Created_new_game_X;

   /***** Create a new game *****/
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   Game->GamCod =
   DB_QueryINSERTandReturnCode ("can not create new game",
				"INSERT INTO gam_games"
				" (CrsCod,Hidden,UsrCod,MaxGrade,Visibility,Title,Txt)"
				" VALUES"
				" (%ld,'N',%ld,%.15lg,%u,'%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Game->MaxGrade,
				Game->Visibility,
				Game->Title,
				Txt);
   Str_SetDecimalPointToLocal ();	// Return to local system

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_game_X,
                  Game->Title);
  }

/*****************************************************************************/
/************************* Update an existing game *************************/
/*****************************************************************************/

static void Gam_UpdateGame (struct Gam_Game *Game,const char *Txt)
  {
   extern const char *Txt_The_game_has_been_modified;

   /***** Update the data of the game *****/
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
   DB_QueryUPDATE ("can not update game",
		   "UPDATE gam_games"
		   " SET CrsCod=%ld,"
		        "MaxGrade=%.15lg,"
		        "Visibility=%u,"
		        "Title='%s',"
		        "Txt='%s'"
		   " WHERE GamCod=%ld",
		   Gbl.Hierarchy.Crs.CrsCod,
		   Game->MaxGrade,
		   Game->Visibility,
	           Game->Title,
	           Txt,
	           Game->GamCod);
   Str_SetDecimalPointToLocal ();	// Return to local system

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_game_has_been_modified);
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
/*************** Put a form to edit/create a question in game ****************/
/*****************************************************************************/

void Gam_RequestNewQuestion (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (Gam_CheckIfEditable (&Game))
     {
      /***** Show form to create a new question in this game *****/
      Games.GamCod = Game.GamCod;
      Tst_RequestSelectTestsForGame (&Games);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        true,	// List game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/**************** List several test questions for selection ******************/
/*****************************************************************************/

void Gam_ListTstQuestionsToSelect (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (Gam_CheckIfEditable (&Game))
     {
      /***** List several test questions for selection *****/
      Games.GamCod = Game.GamCod;
      Tst_ListQuestionsToSelect (&Games);
     }
   else
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/****************** Write parameter with index of question *******************/
/*****************************************************************************/

void Gam_PutParamQstInd (unsigned QstInd)
  {
   Par_PutHiddenParamUnsigned (NULL,"QstInd",QstInd);
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
      Lay_ShowErrorAndExit ("Error: wrong question index.");

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
		   "SELECT MAX(QstInd)"
		   " FROM gam_questions"
		   " WHERE GamCod=%ld",
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
   if (row)
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
   if (row)
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

static void Gam_ListGameQuestions (struct Gam_Games *Games,struct Gam_Game *Game)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_This_game_has_no_questions;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   bool ICanEditQuestions = Gam_CheckIfEditable (Game);

   /***** Get data of questions from database *****/
   NumQsts = (unsigned)
             DB_QuerySELECT (&mysql_res,"can not get game questions",
			      "SELECT QstInd,"	// row[0]
				     "QstCod"	// row[1]
			      " FROM gam_questions"
			      " WHERE GamCod=%ld"
			      " ORDER BY QstInd",
			      Game->GamCod);

   /***** Begin box *****/
   Games->GamCod = Game->GamCod;
   if (ICanEditQuestions)
      Box_BoxBegin (NULL,Txt_Questions,
		    Gam_PutIconToAddNewQuestions,&Gbl,
		    Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Txt_Questions,
		    NULL,NULL,
		    Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

   /***** Show table with questions *****/
   if (NumQsts)
      Gam_ListOneOrMoreQuestionsForEdition (Games,
                                            Game->GamCod,NumQsts,mysql_res,
					    ICanEditQuestions);
   else	// This game has no questions
      Ale_ShowAlert (Ale_INFO,Txt_This_game_has_no_questions);

   /***** Put button to add a new question in this game *****/
   if (ICanEditQuestions)		// I can edit questions
      Gam_PutButtonToAddNewQuestions (Games);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* List game questions for edition ***********************/
/*****************************************************************************/

static void Gam_ListOneOrMoreQuestionsForEdition (struct Gam_Games *Games,
						  long GamCod,unsigned NumQsts,
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
   unsigned NumQst;
   MYSQL_ROW row;
   struct Tst_Question Question;
   unsigned QstInd;
   unsigned MaxQstInd;
   char StrQstInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   bool QuestionExists;

   /***** Get maximum question index *****/
   MaxQstInd = Gam_GetMaxQuestionIndexInGame (GamCod);

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Code);
   HTM_TH (1,1,"CT",Txt_Tags);
   HTM_TH (1,1,"CT",Txt_Question);

   HTM_TR_End ();

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      /***** Create test question *****/
      Tst_QstConstructor (&Question);

      /***** Get question data *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[0] QstInd
      row[1] QstCod
      */

      /* Get question index (row[0]) */
      QstInd = Str_ConvertStrToUnsigned (row[0]);
      snprintf (StrQstInd,sizeof (StrQstInd),
	        "%u",
		QstInd);

      /* Get question code (row[1]) */
      Question.QstCod = Str_ConvertStrCodToLongCod (row[1]);

      /***** Icons *****/
      Games->GamCod = GamCod;
      Games->QstInd = QstInd;
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

      /* Put icon to remove the question */
      if (ICanEditQuestions)
	{
	 Frm_StartForm (ActReqRemGamQst);
	 Gam_PutParams (Games);
	 Gam_PutParamQstInd (QstInd);
	 Ico_PutIconRemove ();
	 Frm_EndForm ();
	}
      else
         Ico_PutIconRemovalNotAllowed ();

      /* Put icon to move up the question */
      if (ICanEditQuestions && QstInd > 1)
	{
	 Lay_PutContextualLinkOnlyIcon (ActUp_GamQst,NULL,
	                                Gam_PutParamsOneQst,Games,
				        "arrow-up.svg",
					Str_BuildStringStr (Txt_Move_up_X,
							    StrQstInd));
	 Str_FreeString ();
	}
      else
         Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);

      /* Put icon to move down the question */
      if (ICanEditQuestions && QstInd < MaxQstInd)
	{
	 Lay_PutContextualLinkOnlyIcon (ActDwnGamQst,NULL,
	                                Gam_PutParamsOneQst,Games,
				        "arrow-down.svg",
					Str_BuildStringStr (Txt_Move_down_X,
							    StrQstInd));
	 Str_FreeString ();
	}
      else
         Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);

      /* Put icon to edit the question */
      if (ICanEditQuestions)
	 Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
	                              Tst_PutParamQstCod,&Question.QstCod);

      HTM_TD_End ();

      /***** Question *****/
      QuestionExists = Tst_GetQstDataFromDB (&Question);
      Gam_ListQuestionForEdition (&Question,QstInd,QuestionExists);

      HTM_TR_End ();

      /***** Destroy test question *****/
      Tst_QstDestructor (&Question);
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************** List game question for edition ***********************/
/*****************************************************************************/

static void Gam_ListQuestionForEdition (const struct Tst_Question *Question,
                                        unsigned QstInd,bool QuestionExists)
  {
   extern const char *Txt_Question_removed;

   /***** Number of question and answer type (row[1]) *****/
   HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteNumQst (QstInd);
   if (QuestionExists)
      Tst_WriteAnswerType (Question->Answer.Type);
   HTM_TD_End ();

   /***** Write question code *****/
   HTM_TD_Begin ("class=\"DAT_SMALL CT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TxtF ("%ld&nbsp;",Question->QstCod);
   HTM_TD_End ();

   /***** Write the question tags *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (QuestionExists)
      Tst_GetAndWriteTagsQst (Question->QstCod);
   HTM_TD_End ();

   /***** Write stem (row[3]) and media *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (QuestionExists)
     {
      /* Write stem */
      Tst_WriteQstStem (Question->Stem,"TEST_EDI",
			true);	// Visible

      /* Show media */
      Med_ShowMedia (&Question->Media,
		     "TEST_MED_EDIT_LIST_STEM_CONTAINER",
		     "TEST_MED_EDIT_LIST_STEM");

      /* Show feedback */
      Tst_WriteQstFeedback (Question->Feedback,"TEST_EDI_LIGHT");

      /* Show answers */
      Tst_WriteAnswersListing (Question);
     }
   else
     {
      HTM_SPAN_Begin ("class=\"DAT_LIGHT\"");
      HTM_Txt (Txt_Question_removed);
      HTM_SPAN_End ();
     }
   HTM_TD_End ();
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to game *******************/
/*****************************************************************************/

static void Gam_PutIconToAddNewQuestions (void *Games)
  {
   extern const char *Txt_Add_questions;

   if (Games)
      /***** Put form to create a new question *****/
      Ico_PutContextualIconToAdd (ActAddOneGamQst,NULL,
				  Gam_PutParams,Games,
				  Txt_Add_questions);
  }

/*****************************************************************************/
/***************** Put button to add new questions to game *******************/
/*****************************************************************************/

static void Gam_PutButtonToAddNewQuestions (struct Gam_Games *Games)
  {
   extern const char *Txt_Add_questions;

   Frm_StartForm (ActAddOneGamQst);
   Gam_PutParams (Games);
   Btn_PutConfirmButton (Txt_Add_questions);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Add selected test questions to game ********************/
/*****************************************************************************/

void Gam_AddTstQuestionsToGame (void)
  {
   extern const char *Txt_No_questions_have_been_added;
   struct Gam_Games Games;
   struct Gam_Game Game;
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;
   unsigned MaxQstInd;

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (Gam_CheckIfEditable (&Game))
     {
      /***** Get selected questions *****/
      /* Allocate space for selected question codes */
      Gam_AllocateListSelectedQuestions (&Games);

      /* Get question codes */
      Par_GetParMultiToText ("QstCods",Games.ListQuestions,
			     Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS);

      /* Check number of questions */
      if (Gam_CountNumQuestionsInList (&Games))	// If questions selected...
	{
	 /***** Insert questions in database *****/
	 Ptr = Games.ListQuestions;
	 while (*Ptr)
	   {
	    /* Get next code */
	    Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
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
      Gam_FreeListsSelectedQuestions (&Games);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        true,	// List game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/****************** Allocate memory for list of questions ********************/
/*****************************************************************************/

static void Gam_AllocateListSelectedQuestions (struct Gam_Games *Games)
  {
   if (!Games->ListQuestions)
     {
      if ((Games->ListQuestions = (char *) malloc (Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS + 1)) == NULL)
         Lay_NotEnoughMemoryExit ();;
      Games->ListQuestions[0] = '\0';
     }
  }

/*****************************************************************************/
/*********** Free memory used by list of selected question codes *************/
/*****************************************************************************/

static void Gam_FreeListsSelectedQuestions (struct Gam_Games *Games)
  {
   if (Games->ListQuestions)
     {
      free (Games->ListQuestions);
      Games->ListQuestions = NULL;
     }
  }

/*****************************************************************************/
/**** Count the number of questions in the list of selected question codes ***/
/*****************************************************************************/

static unsigned Gam_CountNumQuestionsInList (const struct Gam_Games *Games)
  {
   const char *Ptr;
   unsigned NumQuestions = 0;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;

   /***** Go over list of questions counting the number of questions *****/
   Ptr = Games->ListQuestions;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
      if (sscanf (LongStr,"%ld",&QstCod) != 1)
         Lay_ShowErrorAndExit ("Wrong question code.");
      NumQuestions++;
     }
   return NumQuestions;
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Gam_RequestRemoveQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstInd;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (Gam_CheckIfEditable (&Game))
     {
      /***** Get question index *****/
      QstInd = Gam_GetParamQstInd ();

      /***** Show question and button to remove question *****/
      Games.GamCod = Game.GamCod;
      Games.QstInd = QstInd;
      Ale_ShowAlertAndButton (ActRemGamQst,NULL,NULL,
                              Gam_PutParamsOneQst,&Gbl,
			      Btn_REMOVE_BUTTON,Txt_Remove_question,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			      QstInd);
     }
   else
      Lay_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        true,	// List game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Gam_RemoveQst (void)
  {
   extern const char *Txt_Question_removed;
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstInd;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (Gam_CheckIfEditable (&Game))
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
      Lay_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
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
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstIndTop;
   unsigned QstIndBottom;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (Gam_CheckIfEditable (&Game))
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
      Lay_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
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
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstIndTop;
   unsigned QstIndBottom;
   unsigned MaxQstInd;	// 0 if no questions

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (Gam_CheckIfEditable (&Game))
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
      Lay_NoPermissionExit ();

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
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
// Before calling this function, number of matches must be calculated

static bool Gam_CheckIfEditable (const struct Gam_Game *Game)
  {
   if (Gam_CheckIfICanEditGames ())
      /***** Questions are editable only if game has no matches *****/
      return (bool) (Game->NumMchs == 0);	// Games with matches should not be edited
   else
      return false;	// Questions are not editable
  }

/*****************************************************************************/
/********************* Put button to create a new match **********************/
/*****************************************************************************/

void Gam_PutButtonNewMatch (struct Gam_Games *Games,long GamCod)
  {
   extern const char *Txt_New_match;

   Games->GamCod = GamCod;
   Frm_StartFormAnchor (ActReqNewMch,Mch_NEW_MATCH_SECTION_ID);
   Gam_PutParams (Games);
   Btn_PutConfirmButton (Txt_New_match);
   Frm_EndForm ();
  }

/*****************************************************************************/
/************* Request the creation of a new match as a teacher **************/
/*****************************************************************************/

void Gam_RequestNewMatch (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Gam_GetDataOfGameByCod (&Game);

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
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

double Gam_GetNumQstsPerCrsGame (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   double NumQstsPerGame;

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

   /***** Get average number of questions per game *****/
   row = mysql_fetch_row (mysql_res);
   NumQstsPerGame = Str_GetDoubleFromStr (row[0]);

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
/*************** Get maximum score of a game from database *******************/
/*****************************************************************************/

void Gam_GetScoreRange (long GamCod,double *MinScore,double *MaxScore)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   unsigned NumRow;
   unsigned NumAnswers;

   /***** Get maximum score of a game from database *****/
   NumRows = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get data of a question",
			     "SELECT COUNT(tst_answers.AnsInd) AS N"
			     " FROM tst_answers,gam_questions"
			     " WHERE gam_questions.GamCod=%ld"
			     " AND gam_questions.QstCod=tst_answers.QstCod"
			     " GROUP BY tst_answers.QstCod",
			     GamCod);
   for (NumRow = 0, *MinScore = *MaxScore = 0.0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get min answers (row[0]) */
      if (sscanf (row[0],"%u",&NumAnswers) != 1)
         NumAnswers = 0;

      /* Accumulate minimum and maximum score */
      if (NumAnswers < 2)
	 Lay_ShowErrorAndExit ("Wrong number of answers.");
      *MinScore += -1.0 / (double) (NumAnswers - 1);
      *MaxScore +=  1.0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
