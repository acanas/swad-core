// swad_game.c: games using remote control

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_autolink.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_game_database.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_match.h"
#include "swad_match_database.h"
#include "swad_match_result.h"
#include "swad_pagination.h"
#include "swad_program_database.h"
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

#define Gam_MAX_SELECTED_QUESTIONS		10000
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
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Gam_ListAllGames (struct Gam_Games *Games);
static bool Gam_CheckIfICanEditGames (void);
static bool Gam_CheckIfICanListGameQuestions (void);
static void Gam_PutIconsListGames (void *Games);
static void Gam_PutIconToCreateNewGame (struct Gam_Games *Games);
static void Gam_PutButtonToCreateNewGame (struct Gam_Games *Games);
static void Gam_PutParamsToCreateNewGame (void *Games);

static void Gam_ShowOneGame (struct Gam_Games *Games,
                             struct Gam_Game *Game,bool ShowOnlyThisGame);

static void Gam_PutIconsOneGame (void *Games);
static void Gam_WriteAuthor (struct Gam_Game *Game);

static void Gam_PutHiddenParamGameOrder (Gam_Order_t SelectedOrder);

static void Gam_PutFormsToRemEditOneGame (struct Gam_Games *Games,
					  const struct Gam_Game *Game,
					  const char *Anchor);

static void Gam_PutParamsOneQst (void *Games);
static void Gam_PutHiddenParamOrder (Gam_Order_t SelectedOrder);
static Gam_Order_t Gam_GetParamOrder (void);

static void Gam_RemoveGameFromAllTables (long GamCod);

static void Gam_PutFormsEditionGame (struct Gam_Games *Games,
				     struct Gam_Game *Game,
				     char Txt[Cns_MAX_BYTES_TEXT + 1],
				     bool ItsANewGame);
static void Gam_ReceiveGameFieldsFromForm (struct Gam_Game *Game,
				           char Txt[Cns_MAX_BYTES_TEXT + 1]);
static bool Gam_CheckGameFieldsReceivedFromForm (const struct Gam_Game *Game);

static void Gam_CreateGame (struct Gam_Game *Game,const char *Txt);
static void Gam_UpdateGame (struct Gam_Game *Game,const char *Txt);

static void Gam_ListGameQuestions (struct Gam_Games *Games,struct Gam_Game *Game);
static void Gam_ListOneOrMoreQuestionsForEdition (struct Gam_Games *Games,
						  long GamCod,unsigned NumQsts,
                                                  MYSQL_RES *mysql_res,
						  bool ICanEditQuestions);

static void Gam_PutIconToAddNewQuestions (void *Games);
static void Gam_PutButtonToAddNewQuestions (struct Gam_Games *Games);

static void Gam_AllocateListSelectedQuestions (struct Gam_Games *Games);
static void Gam_FreeListsSelectedQuestions (struct Gam_Games *Games);

static void Gam_ExchangeQuestions (long GamCod,
                                   unsigned QstIndTop,unsigned QstIndBottom);

static bool Gam_CheckIfEditable (const struct Gam_Game *Game);

/*****************************************************************************/
/*************************** Reset games context *****************************/
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
   Games->GamCod            = -1L;	// Current/selected game code
   Games->MchCod.Current    =
   Games->MchCod.Selected   = -1L;	// Current/selected match code
   Games->QstInd            = 0;	// Current question index
  }

/*****************************************************************************/
/*************************** Initialize game to empty ************************/
/*****************************************************************************/

void Gam_ResetGame (struct Gam_Game *Game)
  {
   /***** Initialize to empty game *****/
   Game->GamCod                  = -1L;
   Game->CrsCod                  = -1L;
   Game->UsrCod                  = -1L;
   Game->MaxGrade                = Gam_MAX_GRADE_DEFAULT;
   Game->Visibility              = TstVis_VISIBILITY_DEFAULT;
   Game->TimeUTC[Dat_STR_TIME] = (time_t) 0;
   Game->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Game->Title[0]                = '\0';
   Game->NumQsts                 = 0;
   Game->NumMchs                 = 0;
   Game->NumUnfinishedMchs       = 0;
   Game->Hidden                  = false;
  }

/*****************************************************************************/
/***************************** List all games ********************************/
/*****************************************************************************/

void Gam_SeeAllGames (void)
  {
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   Gam_GetParams (&Games);	// Return value ignored

   /***** Show all games *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/******************************* Show all games ******************************/
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

   /***** Reset game *****/
   Gam_ResetGame (&Game);

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
      Pag_WriteLinksToPagesCentered (Pag_GAMES,&Pagination,
				     Games,-1L);

      if (Games->Num)
	{
	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (5);

	    /***** Table head *****/
	    HTM_TR_Begin (NULL);

	       if (Gam_CheckIfICanEditGames () ||
		   Gam_CheckIfICanListGameQuestions ())
		  HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons

	       for (Order  = (Gam_Order_t) 0;
		    Order <= (Gam_Order_t) (Gam_NUM_ORDERS - 1);
		    Order++)
		 {
                  HTM_TH_Begin (HTM_HEAD_LEFT);

		     /* Form to change order */
		     Frm_BeginForm (ActSeeAllGam);
			Pag_PutHiddenParamPagNum (Pag_GAMES,Games->CurrentPage);
			Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);

			HTM_BUTTON_Submit_Begin (Txt_GAMES_ORDER_HELP[Order],
			                         "class=\"BT_LINK\"");
			   if (Order == Games->SelectedOrder)
			      HTM_U_Begin ();
			   HTM_Txt (Txt_GAMES_ORDER[Order]);
			   if (Order == Games->SelectedOrder)
			      HTM_U_End ();
			HTM_BUTTON_End ();

		     Frm_EndForm ();

		  HTM_TH_End ();
		 }

	       HTM_TH (Txt_Matches,HTM_HEAD_RIGHT);

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
      Pag_WriteLinksToPagesCentered (Pag_GAMES,&Pagination,
				     Games,-1L);

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
   static const bool ICanEditGames[Rol_NUM_ROLES] =
     {
      [Rol_TCH    ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanEditGames[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/**************** Check if I can list questions in games *********************/
/*****************************************************************************/

static bool Gam_CheckIfICanListGameQuestions (void)
  {
   static const bool ICanListGameQuestions[Rol_NUM_ROLES] =
     {
      [Rol_NET    ] = true,
      [Rol_TCH    ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanListGameQuestions[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/***************** Put contextual icons in list of games *******************/
/*****************************************************************************/

static void Gam_PutIconsListGames (void *Games)
  {
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD    ] = ActSeeMyMchResCrs,
      [Rol_NET    ] = ActReqSeeUsrMchRes,
      [Rol_TCH    ] = ActReqSeeUsrMchRes,
      [Rol_SYS_ADM] = ActReqSeeUsrMchRes,
     };

   if (Games)
     {
      /***** Put icon to create a new game *****/
      if (Gam_CheckIfICanEditGames ())
	 Gam_PutIconToCreateNewGame ((struct Gam_Games *) Games);

      /***** Put icon to view matches results *****/
      if (NextAction[Gbl.Usrs.Me.Role.Logged])
	 Ico_PutContextualIconToShowResults (NextAction[Gbl.Usrs.Me.Role.Logged],NULL,
					     NULL,NULL);

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_GAMES);
     }
  }

/*****************************************************************************/
/********************** Put icon to create a new game **********************/
/*****************************************************************************/

static void Gam_PutIconToCreateNewGame (struct Gam_Games *Games)
  {
   Ico_PutContextualIconToAdd (ActFrmNewGam,NULL,
                               Gam_PutParamsToCreateNewGame,Games);
  }

/*****************************************************************************/
/********************* Put button to create a new game *********************/
/*****************************************************************************/

static void Gam_PutButtonToCreateNewGame (struct Gam_Games *Games)
  {
   extern const char *Txt_New_game;

   Frm_BeginForm (ActFrmNewGam);
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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
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
                 Gam_PutIconsOneGame,Games,
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
   extern const char *Txt_Number_of_questions;
   extern const char *Txt_Maximum_grade;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Matches;
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   Dat_StartEndTime_t StartEndTime;
   const char *Color;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   bool ICanEditGames     = Gam_CheckIfICanEditGames ();
   bool ICanListQuestions = Gam_CheckIfICanListGameQuestions ();

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Game->GamCod,&Anchor);

   /***** Begin box and table *****/
   if (ShowOnlyThisGame)
      HTM_TABLE_BeginWidePadding (2);

   /***** Begin first row of this game *****/
   HTM_TR_Begin (NULL);

      /***** Icons related to this game *****/
      if (ICanEditGames ||
	  ICanListQuestions)
	{
	 Games->GamCod = Game->GamCod;

	 if (ShowOnlyThisGame)
	    HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL\"");
	 else
	    HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL %s\"",
	                  The_GetColorRows ());

	 if (ICanEditGames)
	    /* Icons to remove/edit this game */
	    Gam_PutFormsToRemEditOneGame (Games,Game,Anchor);
	 else
	    /* Put icon to view game listing its questions */
	    Ico_PutContextualIconToView (ActLstOneGam,NULL,
					 Gam_PutParams,Games);

	 HTM_TD_End ();
	}

      /***** Start/end date/time *****/
      UniqueId++;
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
	{
	 if (asprintf (&Id,"gam_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 Color = Game->NumUnfinishedMchs ? (Game->Hidden ? "DATE_GREEN_LIGHT":
							   "DATE_GREEN") :
					   (Game->Hidden ? "DATE_RED_LIGHT":
							   "DATE_RED");
	 if (ShowOnlyThisGame)
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
			  Id,Color,The_GetSuffix ());
	 else
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			  Id,Color,The_GetSuffix (),The_GetColorRows ());
	 if (Game->TimeUTC[Dat_STR_TIME])
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
	 HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());

      /* Game title */
      Games->GamCod = Game->GamCod;
      HTM_ARTICLE_Begin (Anchor);
	 Frm_BeginForm (ActSeeGam);
	    Gam_PutParams (Games);
	    HTM_BUTTON_Submit_Begin (Txt_View_game,"class=\"LT BT_LINK %s_%s\"",
				     Game->Hidden ? "ASG_TITLE_LIGHT":
						    "ASG_TITLE",
				     The_GetSuffix ());
	       HTM_Txt (Game->Title);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_ARTICLE_End ();

      /* Number of questions, maximum grade, visibility of results */
      HTM_DIV_Begin ("class=\"%s_%s\"",
                     Game->Hidden ? "ASG_GRP_LIGHT" :
				    "ASG_GRP",
		     The_GetSuffix ());
	 HTM_TxtColonNBSP (Txt_Number_of_questions);
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
	 HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());

      Games->GamCod = Game->GamCod;
      Frm_BeginForm (ActSeeGam);
	 Gam_PutParams (Games);
	 HTM_BUTTON_Submit_Begin (Txt_Matches,"class=\"LT BT_LINK %s_%s\"",
	                          Game->Hidden ? "ASG_TITLE_LIGHT":
				                 "ASG_TITLE",
				  The_GetSuffix ());
	    if (ShowOnlyThisGame)
	       HTM_TxtColonNBSP (Txt_Matches);
	    HTM_Unsigned (Game->NumMchs);
	 HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_TD_End ();

   /***** End 1st row of this game *****/
   HTM_TR_End ();

   /***** Begin 2nd row of this game *****/
   HTM_TR_Begin (NULL);

      /***** Author of the game *****/
      if (ShowOnlyThisGame)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
	               The_GetColorRows ());
      Gam_WriteAuthor (Game);
      HTM_TD_End ();

      /***** Text of the game *****/
      if (ShowOnlyThisGame)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
	               The_GetColorRows ());
      Gam_DB_GetGameTxt (Game->GamCod,Txt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      HTM_DIV_Begin ("class=\"PAR %s_%s\"",
                     Game->Hidden ? "DAT_LIGHT" :
				    "DAT",
		     The_GetSuffix ());
	 HTM_Txt (Txt);
      HTM_DIV_End ();
      HTM_TD_End ();

   /***** End 2nd row of this game *****/
   HTM_TR_End ();

   /***** End table *****/
   if (ShowOnlyThisGame)
      HTM_TABLE_End ();
   else
      The_ChangeRowColor ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
  }

/*****************************************************************************/
/************* Put icon to show results of matches in a game *****************/
/*****************************************************************************/

static void Gam_PutIconsOneGame (void *Games)
  {
   static const Act_Action_t NextAction[Rol_NUM_ROLES] =
     {
      [Rol_STD    ] = ActSeeMyMchResGam,
      [Rol_NET    ] = ActSeeUsrMchResGam,
      [Rol_TCH    ] = ActSeeUsrMchResGam,
      [Rol_SYS_ADM] = ActSeeUsrMchResGam,
     };

   if (Games)
     {
      /***** Put icon to view matches results *****/
      if (NextAction[Gbl.Usrs.Me.Role.Logged])
	 Ico_PutContextualIconToShowResults (NextAction[Gbl.Usrs.Me.Role.Logged],MchRes_RESULTS_BOX_ID,
					     Gam_PutParams,Games);

      /***** Link to get resource link *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||		// Only if I am a teacher
	  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)	// or a superuser
	 Ico_PutContextualIconToGetLink (ActReqLnkGam,NULL,
					 Gam_PutParams,Games);
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
   static Act_Action_t ActionHideUnhide[2] =
     {
      [false] = ActHidGam,	// Visible ==> action to hide
      [true ] = ActUnhGam,	// Hidden ==> action to unhide
     };

   /***** Icon to remove game *****/
   Ico_PutContextualIconToRemove (ActReqRemGam,NULL,
                                  Gam_PutParams,Games);

   /***** Icon to unhide/hide game *****/
   Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
				      Gam_PutParams,Games,
				      Game->Hidden);

   /***** Icon to edit game *****/
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
/************************** Get list of all games ****************************/
/*****************************************************************************/

void Gam_GetListGames (struct Gam_Games *Games,Gam_Order_t SelectedOrder)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGame;

   /***** Free list of games *****/
   if (Games->LstIsRead)
      Gam_FreeListGames (Games);

   /***** Get list of games from database *****/
   if ((Games->Num = Gam_DB_GetListGames (&mysql_res,SelectedOrder))) // Games found...
     {
      /***** Create list of games *****/
      if ((Games->Lst = malloc ((size_t) Games->Num *
                                sizeof (*Games->Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the games codes *****/
      for (NumGame = 0;
	   NumGame < Games->Num;
	   NumGame++)
        {
         /* Get next game code (row[0]) */
         row = mysql_fetch_row (mysql_res);
         if ((Games->Lst[NumGame].GamCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongGameExit ();
        }
     }

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

   /***** Default selected *****/
   Games->NumSelected = 0;

   /***** Trivial check: there are games visibles by me *****/
   if (!Games->Num)
      return;

   /***** Allocate memory for list of games selected *****/
   MaxSizeListGamCodsSelected = Games->Num * (Cns_MAX_DECIMAL_DIGITS_LONG + 1);
   if ((Games->GamCodsSelected = malloc (MaxSizeListGamCodsSelected + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

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

   /***** Get data of game from database *****/
   if (Gam_DB_GetDataOfGameByCod (&mysql_res,Game->GamCod)) // Game found...
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
      Str_Copy (Game->Title,row[6],sizeof (Game->Title) - 1);

      /* Get number of questions */
      Game->NumQsts = Gam_DB_GetNumQstsGame (Game->GamCod);

      /* Get number of matches */
      Game->NumMchs = Mch_DB_GetNumMchsInGame (Game->GamCod);

      /* Get number of unfinished matches */
      Game->NumUnfinishedMchs = Mch_DB_GetNumUnfinishedMchsInGame (Game->GamCod);
     }
   else
      /* Initialize to empty game */
      Gam_ResetGame (Game);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   if (Game->GamCod > 0)
     {
      /***** Get start and end times from database *****/
      if (Mch_DB_GetStartEndMatchesInGame (&mysql_res,Game->GamCod))
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get start date (row[0] holds the start UTC time)
	    and end   date (row[1] holds the end   UTC time) */
	 Game->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[0]);
	 Game->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[1]);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }
   else
     {
      Game->TimeUTC[Dat_STR_TIME] =
      Game->TimeUTC[Dat_END_TIME] = (time_t) 0;
     }
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
/*************** Ask for confirmation of removing of a game ******************/
/*****************************************************************************/

void Gam_AskRemGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_game_X;
   extern const char *Txt_Remove_game;
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) <= 0)
      Err_WrongGameExit ();

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

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

   /***** Remove game questions *****/
   Gam_DB_RemoveGameQsts (GamCod);

   /***** Remove game *****/
   Gam_DB_RemoveGame (GamCod);
  }

/*****************************************************************************/
/*********************** Remove all games of a course ************************/
/*****************************************************************************/

void Gam_RemoveCrsGames (long CrsCod)
  {
   /***** Remove all matches in this course *****/
   Mch_RemoveMatchesInCourseFromAllTables (CrsCod);

   /***** Remove the questions in games *****/
   Gam_DB_RemoveCrsGameQsts (CrsCod);

   /***** Remove the games *****/
   Gam_DB_RemoveCrsGames (CrsCod);
  }

/*****************************************************************************/
/******************************** Hide a game ******************************/
/*****************************************************************************/

void Gam_HideGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Hide game *****/
   Gam_DB_HideOrUnhideGame (Game.GamCod,true);

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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Unhide game *****/
   Gam_DB_HideOrUnhideGame (Game.GamCod,false);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/************************* List the questions in a game **********************/
/*****************************************************************************/

void Gam_ListGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Check if I can list game questions *****/
   if (!Gam_CheckIfICanListGameQuestions ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get game data *****/
   Gam_GetDataOfGameByCod (&Game);
   Gam_DB_GetGameTxt (Game.GamCod,Txt);

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        true,	// List game questions
	                false);	// Do not put form to start new match
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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Check if I can edit games *****/
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

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
      Gam_DB_GetGameTxt (Game.GamCod,Txt);
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
   Frm_BeginForm (ItsANewGame ? ActNewGam :
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
	    HTM_INPUT_TEXT ("Title",Gam_MAX_CHARS_TITLE,Game->Title,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "id=\"Title\""
			    " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

      /***** Maximum grade *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RM FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtColon (Txt_Maximum_grade);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_FLOAT ("MaxGrade",0.0,DBL_MAX,0.01,Game->MaxGrade,false,
			     " class=\"INPUT_%s\" required=\"required\"",
			     The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

      /***** Visibility of results *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RT FORM_IN_%s\"",The_GetSuffix ());
	    HTM_TxtColon (Txt_Result_visibility);
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
				" class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
				The_GetSuffix ());
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

void Gam_ReceiveFormGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;
   bool ItsANewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Check if I can edit games *****/
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

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
      Err_NoPermissionExit ();
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
   bool NewGameIsCorrect;

   /***** Check if title is correct *****/
   NewGameIsCorrect = true;
   if (Game->Title[0])	// If there's a game title
     {
      /* If title of game was in database... */
      if (Gam_DB_CheckIfSimilarGameExists (Game))
	{
	 NewGameIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_a_game_with_the_title_X,
			Game->Title);
	}
     }
   else	// If there is not a game title
     {
      NewGameIsCorrect = false;
      Ale_ShowAlertYouMustSpecifyTheTitle ();
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
   Game->GamCod = Gam_DB_CreateGame (Game,Txt);

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
   Gam_DB_UpdateGame (Game,Txt);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_game_has_been_modified);
  }

/*****************************************************************************/
/*************** Put a form to edit/create a question in game ****************/
/*****************************************************************************/

void Gam_ReqSelectQstsToAddToGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Game))
      Err_NoPermissionExit ();

   /***** Show form to create a new question in this game *****/
   Games.GamCod = Game.GamCod;
   Qst_RequestSelectQstsForGame (&Games);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        true,	// List game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/**************** List several test questions for selection ******************/
/*****************************************************************************/

void Gam_ListQstsToAddToGame (void)
  {
   struct Gam_Games Games;
   struct Gam_Game Game;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Game))
      Err_NoPermissionExit ();

   /***** List several test questions for selection *****/
   Games.GamCod = Game.GamCod;
   Qst_ListQuestionsToSelectForGame (&Games);
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

   if ((QstInd = Par_GetParToLong ("QstInd")) <= 0)	// In games, question index should be 1, 2, 3...
      Err_WrongQuestionIndexExit ();

   return (unsigned) QstInd;
  }

/*****************************************************************************/
/************************ List the questions of a game ***********************/
/*****************************************************************************/

static void Gam_ListGameQuestions (struct Gam_Games *Games,struct Gam_Game *Game)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_Questions;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   bool ICanEditQuestions = Gam_CheckIfEditable (Game);

   /***** Get data of questions from database *****/
   NumQsts = Gam_DB_GetGameQuestionsBasic (&mysql_res,Game->GamCod);

   /***** Begin box *****/
   Games->GamCod = Game->GamCod;
   if (ICanEditQuestions)
      Box_BoxBegin (NULL,Txt_Questions,
		    Gam_PutIconToAddNewQuestions,Games,
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
   extern const char *Txt_Movement_not_allowed;
   unsigned NumQst;
   MYSQL_ROW row;
   struct Qst_Question Question;
   unsigned QstInd;
   unsigned MaxQstInd;
   char StrQstInd[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   bool QuestionExists;
   char *Anchor = NULL;

   /***** Trivial check *****/
   if (!NumQsts)
      return;

   /***** Get maximum question index *****/
   MaxQstInd = Gam_DB_GetMaxQuestionIndexInGame (GamCod);	// 0 is no questions in game

   /***** Write the heading *****/
   HTM_TABLE_BeginWideMarginPadding (5);

      HTM_TR_Begin (NULL);
	 HTM_TH_Empty (1);
         HTM_TH (Txt_No_INDEX,HTM_HEAD_CENTER);
         HTM_TH (Txt_Code    ,HTM_HEAD_CENTER);
         HTM_TH (Txt_Tags    ,HTM_HEAD_CENTER);
         HTM_TH (Txt_Question,HTM_HEAD_CENTER);
      HTM_TR_End ();

      /***** Write rows *****/
      for (NumQst = 0, The_ResetRowColor ();
	   NumQst < NumQsts;
	   NumQst++, The_ChangeRowColor ())
	{
	 /***** Create test question *****/
	 Qst_QstConstructor (&Question);

	 /***** Get question data *****/
	 row = mysql_fetch_row (mysql_res);
	 /*
	 row[0] QstCod
	 row[1] QstInd
	 */
	 /* Get question code (row[0]) */
	 Question.QstCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get question index (row[1]) */
	 QstInd = Str_ConvertStrToUnsigned (row[1]);
	 snprintf (StrQstInd,sizeof (StrQstInd),"%u",QstInd);

	 /* Initialize context */
	 Games->GamCod = GamCod;
	 Games->QstInd = QstInd;

	 /***** Build anchor string *****/
	 Frm_SetAnchorStr (Question.QstCod,&Anchor);

	 /***** Begin row *****/
	 HTM_TR_Begin (NULL);

	    /***** Icons *****/
	    HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());

	       /* Put icon to remove the question */
	       if (ICanEditQuestions)
		  Ico_PutContextualIconToRemove (ActReqRemGamQst,NULL,
						 Gam_PutParamsOneQst,Games);
	       else
		  Ico_PutIconRemovalNotAllowed ();

	       /* Put icon to move up the question */
	       if (ICanEditQuestions && QstInd > 1)
		  Lay_PutContextualLinkOnlyIcon (ActUp_GamQst,Anchor,
						 Gam_PutParamsOneQst,Games,
						 "arrow-up.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	       /* Put icon to move down the question */
	       if (ICanEditQuestions && QstInd < MaxQstInd)
		  Lay_PutContextualLinkOnlyIcon (ActDwnGamQst,Anchor,
						 Gam_PutParamsOneQst,Games,
						 "arrow-down.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	       /* Put icon to edit the question */
	       if (ICanEditQuestions)
		  Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
					       Qst_PutParamQstCod,&Question.QstCod);

	    HTM_TD_End ();

	    /***** Question *****/
	    QuestionExists = Qst_GetQstDataFromDB (&Question);
	    Qst_ListQuestionForEdition (&Question,QstInd,QuestionExists,Anchor);

	 /***** End row *****/
	 HTM_TR_End ();

	 /***** Free anchor string *****/
	 Frm_FreeAnchorStr (Anchor);

	 /***** Destroy test question *****/
	 Qst_QstDestructor (&Question);
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to game *******************/
/*****************************************************************************/

static void Gam_PutIconToAddNewQuestions (void *Games)
  {
   Ico_PutContextualIconToAdd (ActAddOneGamQst,NULL,Gam_PutParams,Games);
  }

/*****************************************************************************/
/***************** Put button to add new questions to game *******************/
/*****************************************************************************/

static void Gam_PutButtonToAddNewQuestions (struct Gam_Games *Games)
  {
   extern const char *Txt_Add_questions;

   Frm_BeginForm (ActAddOneGamQst);
      Gam_PutParams (Games);

      Btn_PutConfirmButton (Txt_Add_questions);

   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Add selected test questions to game ********************/
/*****************************************************************************/

void Gam_AddQstsToGame (void)
  {
   extern const char *Txt_No_questions_have_been_added;
   extern const char *Txt_A_question_has_been_added;
   extern const char *Txt_X_questions_have_been_added;
   struct Gam_Games Games;
   struct Gam_Game Game;
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;
   unsigned MaxQstInd;
   unsigned NumQstsAdded;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Game))
      Err_NoPermissionExit ();

   /***** Get selected questions *****/
   /* Allocate space for selected question codes */
   Gam_AllocateListSelectedQuestions (&Games);

   /* Get question codes */
   Par_GetParMultiToText ("QstCods",Games.ListQuestions,
			  Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS);

   /* Check number of questions */
   NumQstsAdded = 0;
   if (Qst_CountNumQuestionsInList (Games.ListQuestions))	// If questions selected...
     {
      /***** Insert questions in database *****/
      Ptr = Games.ListQuestions;
      while (*Ptr)
	{
	 /* Get next code */
	 Par_GetNextStrUntilSeparParamMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 if (sscanf (LongStr,"%ld",&QstCod) != 1)
	    Err_WrongQuestionExit ();

	 /* Check if question is already present in game */
	 if (Gam_DB_GetQstIndFromQstCod (Game.GamCod,QstCod) == 0)	// This question is not yet in this game
	   {
	    /* Get current maximum index */
	    MaxQstInd = Gam_DB_GetMaxQuestionIndexInGame (Game.GamCod);	// 0 is no questions in game

	    /* Insert question in the table of questions */
            Gam_DB_InsertQstInGame (Game.GamCod,MaxQstInd + 1,QstCod);

	    NumQstsAdded++;
	   }
	}
     }

   /***** Show warning in no questions added *****/
   if (NumQstsAdded == 0)
      Ale_ShowAlert (Ale_WARNING,Txt_No_questions_have_been_added);
   else if (NumQstsAdded == 1)
      Ale_ShowAlert (Ale_SUCCESS,Txt_A_question_has_been_added);
   else
      Ale_ShowAlert (Ale_SUCCESS,Txt_X_questions_have_been_added,NumQstsAdded);

   /***** Free space for selected question codes *****/
   Gam_FreeListsSelectedQuestions (&Games);

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
      if ((Games->ListQuestions = malloc (Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
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
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Gam_RequestRemoveQstFromGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstInd;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstInd = Gam_GetParamQstInd ();

   /***** Show question and button to remove question *****/
   Games.GamCod = Game.GamCod;
   Games.QstInd = QstInd;
   Ale_ShowAlertAndButton (ActRemGamQst,NULL,NULL,
			   Gam_PutParamsOneQst,&Games,
			   Btn_REMOVE_BUTTON,Txt_Remove_question,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			   QstInd);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        true,	// List game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Gam_RemoveQstFromGame (void)
  {
   extern const char *Txt_Question_removed;
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstInd;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstInd = Gam_GetParamQstInd ();

   /***** Remove the question from all tables *****/
   /* Remove answers from this test question */
   Mch_DB_RemUsrAnswersOfAQuestion (Game.GamCod,QstInd);

   /* Remove the question itself */
   Gam_DB_RemoveQstFromGame (Game.GamCod,QstInd);

   /* Change indexes of questions greater than this */
   Mch_DB_UpdateIndexesOfQstsGreaterThan (Game.GamCod,QstInd);
   Gam_DB_UpdateIndexesOfQstsGreaterThan (Game.GamCod,QstInd);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

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
   extern const char *Txt_Movement_not_allowed;
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstIndTop;
   unsigned QstIndBottom;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstIndBottom = Gam_GetParamQstInd ();

   /***** Move up question *****/
   if (QstIndBottom > 1)	// 2, 3, 4...
     {
      /* Indexes of questions to be exchanged */
      QstIndTop = Gam_DB_GetPrevQuestionIndexInGame (Game.GamCod,QstIndBottom);
      if (QstIndTop == 0)
	 Err_WrongQuestionIndexExit ();

      /* Exchange questions */
      Gam_ExchangeQuestions (Game.GamCod,QstIndTop,QstIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

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
   extern const char *Txt_Movement_not_allowed;
   struct Gam_Games Games;
   struct Gam_Game Game;
   unsigned QstIndTop;
   unsigned QstIndBottom;
   unsigned MaxQstInd;	// 0 if no questions

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstIndTop = Gam_GetParamQstInd ();

   /***** Get maximum question index *****/
   MaxQstInd = Gam_DB_GetMaxQuestionIndexInGame (Game.GamCod);	// 0 is no questions in game

   /***** Move down question *****/
   if (QstIndTop < MaxQstInd)
     {
      /* Indexes of questions to be exchanged */
      QstIndBottom = Gam_DB_GetNextQuestionIndexInGame (Game.GamCod,QstIndTop);
      if (QstIndBottom == Gam_AFTER_LAST_QUESTION)
	 Err_WrongQuestionIndexExit ();

      /* Exchange questions */
      Gam_ExchangeQuestions (Game.GamCod,QstIndTop,QstIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

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
   Gam_DB_LockTable ();

   /***** Get question code of the questions to be moved *****/
   QstCodTop    = Gam_DB_GetQstCodFromQstInd (GamCod,QstIndTop);
   QstCodBottom = Gam_DB_GetQstCodFromQstInd (GamCod,QstIndBottom);

   /***** Exchange indexes of questions *****/
   /*
   Example:
   QstIndTop    = 1; QstCodTop    = 218
   QstIndBottom = 2; QstCodBottom = 220
                     Step 1              Step 2              Step 3
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
| QstInd | QstCod | | QstInd | QstCod | | QstInd | QstCod | | QstInd | QstCod |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
|      1 |    218 |>|     -2 |    218 |>|     -2 |    218 |>|      2 |    218 |
|      2 |    220 | |      2 |    220 | |      1 |    220 | |      1 |    220 |
|      3 |    232 | |      3 |    232 | |      3 |    232 | |      3 |    232 |
+--------+--------+ +--------+--------+ +--------+--------+ +--------+--------+
   */
   /* Step 1: change temporarily top index to minus bottom index
              in order to not repeat unique index (GamCod,QstInd) */
   Gam_DB_UpdateQstIndex (-((long) QstIndBottom),GamCod,QstCodTop   );

   /* Step 2: change bottom index to old top index  */
   Gam_DB_UpdateQstIndex (  (long) QstIndTop    ,GamCod,QstCodBottom);

   /* Step 3: change top index to old bottom index */
   Gam_DB_UpdateQstIndex (  (long) QstIndBottom ,GamCod,QstCodTop   );

   /***** Unlock table *****/
   DB_UnlockTables ();
  }

/*****************************************************************************/
/*********** Get number of matches and check is edition is possible **********/
/*****************************************************************************/
// Before calling this function, number of matches must be calculated

static bool Gam_CheckIfEditable (const struct Gam_Game *Game)
  {
   if (Gam_CheckIfICanEditGames ())
      /***** Questions are editable only if game has no matches *****/
      return Game->NumMchs == 0;	// Games with matches should not be edited
   else
      return false;			// Questions are not editable
  }

/*****************************************************************************/
/********************* Put button to create a new match **********************/
/*****************************************************************************/

void Gam_PutButtonNewMatch (struct Gam_Games *Games,long GamCod)
  {
   extern const char *Txt_New_match;

   Frm_BeginFormAnchor (ActReqNewMch,Mch_NEW_MATCH_SECTION_ID);
      Games->GamCod = GamCod;
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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Game);

   /***** Get parameters *****/
   if ((Game.GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetDataOfGameByCod (&Game);

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,&Game,
                        false,	// Do not list game questions
                        true);	// Put form to start new match
  }

/*****************************************************************************/
/************************* Show test tags in a game **************************/
/*****************************************************************************/

void Gam_ShowTstTagsPresentInAGame (long GamCod)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumTags;

   /***** Get all tags of questions in this game *****/
   NumTags = Gam_DB_GetTstTagsPresentInAGame (&mysql_res,GamCod);
   Tag_ShowTagList (NumTags,mysql_res);

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
   unsigned NumQsts;
   unsigned NumQst;
   unsigned NumAnswers;

   /***** Get maximum score of a game from database *****/
   NumQsts = Gam_DB_GetNumAnswersOfQstsInGame (&mysql_res,GamCod);
   for (NumQst = 0, *MinScore = *MaxScore = 0.0;
	NumQst < NumQsts;
	NumQst++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get number of answers (row[0]) for this question */
      if (sscanf (row[0],"%u",&NumAnswers) != 1)
         NumAnswers = 0;

      /* Check number of answers */
      if (NumAnswers < 2)
         Err_WrongAnswerExit ();

      /* Accumulate minimum and maximum score */
      *MinScore += -1.0 / (double) (NumAnswers - 1);
      *MaxScore +=  1.0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Show stats about games **************************/
/*****************************************************************************/

void Gam_GetAndShowGamesStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_games;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_games;
   extern const char *Txt_Number_of_BR_courses_with_BR_games;
   extern const char *Txt_Average_number_BR_of_games_BR_per_course;
   unsigned NumGames;
   unsigned NumCoursesWithGames = 0;
   double NumGamesPerCourse = 0.0;

   /***** Get the number of games from this location *****/
   if ((NumGames = Gam_DB_GetNumGames (Gbl.Scope.Current)))
      if ((NumCoursesWithGames = Gam_DB_GetNumCoursesWithGames (Gbl.Scope.Current)) != 0)
         NumGamesPerCourse = (double) NumGames / (double) NumCoursesWithGames;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_GAMES],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_games,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_games                      ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_games      ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_games_BR_per_course,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of games *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumGames);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumCoursesWithGames);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Double2Decimals (NumGamesPerCourse);
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/***************************** Get link to game ******************************/
/*****************************************************************************/

void Gam_GetLinkToGame (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Gam_Games Games;
   long GamCod;
   char Title[Gam_MAX_BYTES_TITLE + 1];

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Get parameters *****/
   if ((GamCod = Gam_GetParams (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get game title *****/
   Gam_DB_GetGameTitle (GamCod,Title);

   /***** Copy link to game into resource clipboard *****/
   Prg_DB_CopyToClipboard (PrgRsc_GAME,GamCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/*********************** Write game in course program ************************/
/*****************************************************************************/

void GamRsc_WriteGameInCrsProgram (long GamCod,bool PutFormToGo,
                                   const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[Act_NUM_ACTIONS];
   char Title[Gam_MAX_BYTES_TITLE + 1];

   /***** Get game title *****/
   Gam_DB_GetGameTitle (GamCod,Title);

   /***** Begin form to go to game *****/
   if (PutFormToGo)
     {
      Frm_BeginForm (ActSeeGam);
         Gam_PutParamGameCod (GamCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[ActSeeGam],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,ActSeeGam);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write game title of exam *****/
   HTM_Txt (Title);

   /***** End form to download file *****/
   if (PutFormToGo)
     {
      /* End form */
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/*********************** Get game title from game code ***********************/
/*****************************************************************************/

void GamRsc_GetTitleFromGamCod (long GamCod,char *Title,size_t TitleSize)
  {
   char TitleFromDB[Gam_MAX_BYTES_TITLE + 1];

   /***** Get game title *****/
   Gam_DB_GetGameTitle (GamCod,TitleFromDB);
   Str_Copy (Title,TitleFromDB,TitleSize);
  }
