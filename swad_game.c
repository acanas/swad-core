// swad_game.c: games using remote control

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
#include <float.h>		// For DBL_MAX
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_autolink.h"
#include "swad_box.h"
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
#include "swad_parameter.h"
#include "swad_parameter_code.h"
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

static bool Gam_CheckIfICanEditGames (void);
static bool Gam_CheckIfICanListGameQuestions (void);
static void Gam_PutIconsListingGames (void *Games);
static void Gam_PutIconToCreateNewGame (struct Gam_Games *Games);
static void Gam_PutParsToCreateNewGame (void *Games);

static void Gam_ShowGameMainData (struct Gam_Games *Games,
                                  bool ShowOnlyThisGame);

static void Gam_PutIconsViewingOneGame (void *Games);
static void Gam_PutIconsEditingOneGame (void *Games);
static void Gam_WriteAuthor (struct Gam_Game *Game);

static void Gam_PutParGameOrder (Gam_Order_t SelectedOrder);

static void Gam_PutIconsToRemEditOneGame (struct Gam_Games *Games,
					  const char *Anchor);

static void Gam_PutParsOneQst (void *Games);
static void Gam_PutParOrder (Gam_Order_t SelectedOrder);
static Gam_Order_t Gam_GetParOrder (void);

static void Gam_RemoveGameFromAllTables (long GamCod);

static void Gam_PutFormEditionGame (struct Gam_Games *Games,
				    char Txt[Cns_MAX_BYTES_TEXT + 1],
			            Gam_ExistingNewGame_t ExistingNewGame);
static void Gam_ReceiveGameFieldsFromForm (struct Gam_Game *Game,
				           char Txt[Cns_MAX_BYTES_TEXT + 1]);
static bool Gam_CheckGameFieldsReceivedFromForm (const struct Gam_Game *Game);

static void Gam_CreateGame (struct Gam_Game *Game,const char *Txt);
static void Gam_UpdateGame (struct Gam_Game *Game,const char *Txt);

static void Gam_ListGameQuestions (struct Gam_Games *Games);
static void Gam_ListOneOrMoreQuestionsForEdition (struct Gam_Games *Games,
						  unsigned NumQsts,
                                                  MYSQL_RES *mysql_res,
						  bool ICanEditQuestions);

static void Gam_PutIconToAddNewQuestions (void *Games);

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
   Games->Game.GamCod       = -1L;	// Current/selected game code
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
   Gam_GetPars (&Games);	// Return value ignored

   /***** Show all games *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/******************************* Show all games ******************************/
/*****************************************************************************/

void Gam_ListAllGames (struct Gam_Games *Games)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Games;
   extern const char *Txt_GAMES_ORDER_HELP[Gam_NUM_ORDERS];
   extern const char *Txt_GAMES_ORDER[Gam_NUM_ORDERS];
   extern const char *Txt_Matches;
   extern const char *Txt_No_games;
   Gam_Order_t Order;
   struct Pag_Pagination Pagination;
   unsigned NumGame;

   /***** Reset game *****/
   Gam_ResetGame (&Games->Game);

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
                 Gam_PutIconsListingGames,Games,
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

               HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");	// Column for contextual icons

	       for (Order  = (Gam_Order_t) 0;
		    Order <= (Gam_Order_t) (Gam_NUM_ORDERS - 1);
		    Order++)
		 {
                  HTM_TH_Begin (HTM_HEAD_LEFT);

		     /* Form to change order */
		     Frm_BeginForm (ActSeeAllGam);
			Pag_PutParPagNum (Pag_GAMES,Games->CurrentPage);
			Par_PutParUnsigned (NULL,"Order",(unsigned) Order);

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
	       Games->Game.GamCod = Games->Lst[NumGame - 1].GamCod;
	       Gam_GetGameDataByCod (&Games->Game);

	       /* Show a pair of rows with the main data of this game */
	       Gam_ShowGameMainData (Games,
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
/**************** Put contextual icons when listing games ********************/
/*****************************************************************************/

static void Gam_PutIconsListingGames (void *Games)
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

      /***** Link to get resource link *****/
      if (Rsc_CheckIfICanGetLink ())
	 Ico_PutContextualIconToGetLink (ActReqLnkGam,NULL,
					 Gam_PutPars,Games);

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_GAMES);
     }
  }

/*****************************************************************************/
/*********************** Put icon to create a new game ***********************/
/*****************************************************************************/

static void Gam_PutIconToCreateNewGame (struct Gam_Games *Games)
  {
   Ico_PutContextualIconToAdd (ActFrmNewGam,NULL,
                               Gam_PutParsToCreateNewGame,Games);
  }

/*****************************************************************************/
/******************** Put parameters to create a new game ********************/
/*****************************************************************************/

static void Gam_PutParsToCreateNewGame (void *Games)
  {
   if (Games)
     {
      Gam_PutParGameOrder (((struct Gam_Games *) Games)->SelectedOrder);
      Pag_PutParPagNum (Pag_GAMES,((struct Gam_Games *) Games)->CurrentPage);
     }
  }

/*****************************************************************************/
/******************************* Show one game *******************************/
/*****************************************************************************/

void Gam_SeeOneGame (void)
  {
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,
                        false,	// Do not list game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/******************************* Show one game *******************************/
/*****************************************************************************/

void Gam_ShowOnlyOneGame (struct Gam_Games *Games,
			  bool ListGameQuestions,
			  bool PutFormNewMatch)
  {
   Gam_ShowOnlyOneGameBegin (Games,ListGameQuestions,PutFormNewMatch);
   Gam_ShowOnlyOneGameEnd ();
  }

void Gam_ShowOnlyOneGameBegin (struct Gam_Games *Games,
			       bool ListGameQuestions,
			       bool PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Game;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Games->Game.Title[0] ? Games->Game.Title :
					     Txt_Game,
                 Gam_PutIconsViewingOneGame,Games,
		 Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

      /***** Show main data of this game *****/
      Gam_ShowGameMainData (Games,
		            true);	// Show only this game

      if (ListGameQuestions)
	 /***** Write questions of this game *****/
	 Gam_ListGameQuestions (Games);
      else
	 /***** List matches *****/
	 Mch_ListMatches (Games,PutFormNewMatch);
  }

void Gam_ShowOnlyOneGameEnd (void)
  {
   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********** Show a pair of rows with the main data of a given game ***********/
/*****************************************************************************/

static void Gam_ShowGameMainData (struct Gam_Games *Games,
                                  bool ShowOnlyThisGame)
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

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Games->Game.GamCod,&Anchor);

   /***** Begin box and table *****/
   if (ShowOnlyThisGame)
      HTM_TABLE_BeginWidePadding (2);

   /***** Begin first row of this game *****/
   HTM_TR_Begin (NULL);

      /***** Icons related to this game *****/
      if (!ShowOnlyThisGame)
	{
	 HTM_TD_Begin ("rowspan=\"2\" class=\"CONTEXT_COL %s\"",
		       The_GetColorRows ());
	    Gam_PutIconsToRemEditOneGame (Games,Anchor);
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
	 Color = Games->Game.NumUnfinishedMchs ? (Games->Game.Hidden ? "DATE_GREEN_LIGHT":
								       "DATE_GREEN") :
						 (Games->Game.Hidden ? "DATE_RED_LIGHT":
								       "DATE_RED");
	 if (ShowOnlyThisGame)
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
			  Id,Color,The_GetSuffix ());
	 else
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			  Id,Color,The_GetSuffix (),The_GetColorRows ());
	 if (Games->Game.TimeUTC[Dat_STR_TIME])
	    Dat_WriteLocalDateHMSFromUTC (Id,Games->Game.TimeUTC[StartEndTime],
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
      HTM_ARTICLE_Begin (Anchor);
	 Frm_BeginForm (ActSeeOneGam);
	    Gam_PutPars (Games);
	    HTM_BUTTON_Submit_Begin (Txt_View_game,"class=\"LT BT_LINK %s_%s\"",
				     Games->Game.Hidden ? "ASG_TITLE_LIGHT":
							  "ASG_TITLE",
				     The_GetSuffix ());
	       HTM_Txt (Games->Game.Title);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_ARTICLE_End ();

      /* Number of questions, maximum grade, visibility of results */
      HTM_DIV_Begin ("class=\"%s_%s\"",
                     Games->Game.Hidden ? "ASG_GRP_LIGHT" :
				          "ASG_GRP",
		     The_GetSuffix ());
	 HTM_TxtColonNBSP (Txt_Number_of_questions);
	 HTM_Unsigned (Games->Game.NumQsts);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Maximum_grade);
	 HTM_Double (Games->Game.MaxGrade);
	 HTM_BR ();
	 HTM_TxtColonNBSP (Txt_Result_visibility);
	 TstVis_ShowVisibilityIcons (Games->Game.Visibility,Games->Game.Hidden);
      HTM_DIV_End ();

      /***** Number of matches in game *****/
      if (ShowOnlyThisGame)
	 HTM_TD_Begin ("class=\"RT\"");
      else
	 HTM_TD_Begin ("class=\"RT %s\"",The_GetColorRows ());

      Frm_BeginForm (ActSeeOneGam);
	 Gam_PutPars (Games);
	 HTM_BUTTON_Submit_Begin (Txt_Matches,"class=\"LT BT_LINK %s_%s\"",
	                          Games->Game.Hidden ? "ASG_TITLE_LIGHT":
						       "ASG_TITLE",
				  The_GetSuffix ());
	    if (ShowOnlyThisGame)
	       HTM_TxtColonNBSP (Txt_Matches);
	    HTM_Unsigned (Games->Game.NumMchs);
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
      Gam_WriteAuthor (&Games->Game);
      HTM_TD_End ();

      /***** Text of the game *****/
      if (ShowOnlyThisGame)
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      else
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
	               The_GetColorRows ());
      Gam_DB_GetGameTxt (Games->Game.GamCod,Txt);
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      HTM_DIV_Begin ("class=\"PAR %s_%s\"",
                     Games->Game.Hidden ? "DAT_LIGHT" :
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
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/**************** Put icons when viewing or editing one game *****************/
/*****************************************************************************/

static void Gam_PutIconsViewingOneGame (void *Games)
  {
   char *Anchor = NULL;

   if (Games)
     {
      /***** Set anchor string *****/
      Frm_SetAnchorStr (((struct Gam_Games *) Games)->Game.GamCod,&Anchor);

      /***** Icons to remove/edit this game *****/
      Gam_PutIconsToRemEditOneGame (Games,Anchor);

      /***** Free anchor string *****/
      Frm_FreeAnchorStr (&Anchor);
     }
  }

static void Gam_PutIconsEditingOneGame (void *Games)
  {
   if (Games)
      /***** Icon to view game *****/
      Ico_PutContextualIconToView (ActLstOneGam,NULL,
				   Gam_PutPars,Games);
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

static void Gam_PutParGameOrder (Gam_Order_t SelectedOrder)
  {
   Par_PutParUnsigned (NULL,"Order",(unsigned) SelectedOrder);
  }

/*****************************************************************************/
/******************** Put icons to remove/edit one game **********************/
/*****************************************************************************/

static void Gam_PutIconsToRemEditOneGame (struct Gam_Games *Games,
					  const char *Anchor)
  {
   static Act_Action_t ActionHideUnhide[2] =
     {
      [false] = ActHidGam,	// Visible ==> action to hide
      [true ] = ActUnhGam,	// Hidden ==> action to unhide
     };
   static const Act_Action_t ActionShowResults[Rol_NUM_ROLES] =
     {
      [Rol_STD    ] = ActSeeMyMchResGam,
      [Rol_NET    ] = ActSeeUsrMchResGam,
      [Rol_TCH    ] = ActSeeUsrMchResGam,
      [Rol_SYS_ADM] = ActSeeUsrMchResGam,
     };

   if (Gam_CheckIfICanEditGames ())
     {
      /***** Icon to remove game *****/
      Ico_PutContextualIconToRemove (ActReqRemGam,NULL,
				     Gam_PutPars,Games);

      /***** Icon to unhide/hide game *****/
      Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
					 Gam_PutPars,Games,
					 Games->Game.Hidden);

      /***** Icon to edit game *****/
      Ico_PutContextualIconToEdit (ActEdiOneGam,NULL,
				   Gam_PutPars,Games);
     }

   if (Gam_CheckIfICanListGameQuestions ())
      /***** Icon to view game listing its questions *****/
      Ico_PutContextualIconToView (ActLstOneGam,NULL,
				   Gam_PutPars,Games);

   /***** Put icon to view matches results *****/
   if (ActionShowResults[Gbl.Usrs.Me.Role.Logged])
      Ico_PutContextualIconToShowResults (ActionShowResults[Gbl.Usrs.Me.Role.Logged],MchRes_RESULTS_BOX_ID,
					  Gam_PutPars,Games);

   /***** Link to get resource link *****/
   if (Rsc_CheckIfICanGetLink ())
      Ico_PutContextualIconToGetLink (ActReqLnkGam,NULL,
				      Gam_PutPars,Games);

  }

/*****************************************************************************/
/**************** Put parameter to move/remove one question ******************/
/*****************************************************************************/

static void Gam_PutParsOneQst (void *Games)
  {
   if (Games)
     {
      Gam_PutPars (Games);
      Gam_PutParQstInd (((struct Gam_Games *) Games)->QstInd);
     }
  }

/*****************************************************************************/
/*********************** Params used to edit a game **************************/
/*****************************************************************************/

void Gam_PutPars (void *Games)
  {
   Grp_WhichGroups_t WhichGroups;

   if (Games)
     {
      ParCod_PutPar (ParCod_Gam,((struct Gam_Games *) Games)->Game.GamCod);
      Gam_PutParOrder (((struct Gam_Games *) Games)->SelectedOrder);
      WhichGroups = Grp_GetParWhichGroups ();
      Grp_PutParWhichGroups (&WhichGroups);
      Pag_PutParPagNum (Pag_GAMES,((struct Gam_Games *) Games)->CurrentPage);
     }
  }

/*****************************************************************************/
/******************* Get parameters used to edit a game **********************/
/*****************************************************************************/

long Gam_GetPars (struct Gam_Games *Games)
  {
   /***** Get other parameters *****/
   Games->SelectedOrder = Gam_GetParOrder ();
   Games->CurrentPage = Pag_GetParPagNum (Pag_GAMES);

   /***** Get game code *****/
   return ParCod_GetPar (ParCod_Gam);
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of games *******/
/*****************************************************************************/

static void Gam_PutParOrder (Gam_Order_t SelectedOrder)
  {
   if (SelectedOrder != Gam_ORDER_DEFAULT)
      Par_PutParUnsigned (NULL,"Order",(unsigned) SelectedOrder);
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of games ************/
/*****************************************************************************/

static Gam_Order_t Gam_GetParOrder (void)
  {
   return (Gam_Order_t) Par_GetParUnsignedLong ("Order",
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
   extern const char *Par_CodeStr[];
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
   Par_GetParMultiToText (Par_CodeStr[ParCod_Gam],Games->GamCodsSelected,
                          MaxSizeListGamCodsSelected);

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
	 Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
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

void Gam_GetGameDataByCod (struct Gam_Game *Game)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of game from database *****/
   if (Gam_DB_GetGameDataByCod (&mysql_res,Game->GamCod)) // Game found...
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
   extern const char *Txt_Remove;
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get data of the game from database *****/
   Gam_GetGameDataByCod (&Games.Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Show question and button to remove game *****/
   Ale_ShowAlertAndButton (ActRemGam,NULL,NULL,
                           Gam_PutPars,&Games,
			   Btn_REMOVE_BUTTON,Txt_Remove,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_game_X,
                           Games.Game.Title);

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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get game code *****/
   Games.Game.GamCod = ParCod_GetAndCheckPar (ParCod_Gam);

   /***** Get data of the game from database *****/
   Gam_GetGameDataByCod (&Games.Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Remove game from all tables *****/
   Gam_RemoveGameFromAllTables (Games.Game.GamCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Game_X_removed,
                  Games.Game.Title);

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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get data of the game from database *****/
   Gam_GetGameDataByCod (&Games.Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Hide game *****/
   Gam_DB_HideOrUnhideGame (Games.Game.GamCod,true);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/******************************** Show a game ******************************/
/*****************************************************************************/

void Gam_UnhideGame (void)
  {
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get data of the game from database *****/
   Gam_GetGameDataByCod (&Games.Game);
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Unhide game *****/
   Gam_DB_HideOrUnhideGame (Games.Game.GamCod,false);

   /***** Show games again *****/
   Gam_ListAllGames (&Games);
  }

/*****************************************************************************/
/************************* List the questions in a game **********************/
/*****************************************************************************/

void Gam_ListGame (void)
  {
   struct Gam_Games Games;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Check if I can list game questions *****/
   if (!Gam_CheckIfICanListGameQuestions ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();

   /***** Get game data *****/
   Gam_GetGameDataByCod (&Games.Game);
   Gam_DB_GetGameTxt (Games.Game.GamCod,Txt);

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,
                        true,	// List game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/**************** Request the creation or edition of a game ******************/
/*****************************************************************************/

void Gam_ReqCreatOrEditGame (void)
  {
   struct Gam_Games Games;
   Gam_ExistingNewGame_t ExistingNewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Check if I can edit games *****/
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   Games.Game.GamCod = Gam_GetPars (&Games);
   ExistingNewGame = (Games.Game.GamCod > 0) ? Gam_EXISTING_GAME :
					       Gam_NEW_GAME;

   /***** Get game data *****/
   switch (ExistingNewGame)
     {
      case Gam_EXISTING_GAME:
	 /* Get game data from database */
	 Gam_GetGameDataByCod (&Games.Game);
	 Gam_DB_GetGameTxt (Games.Game.GamCod,Txt);
	 break;
      case Gam_NEW_GAME:
	 Txt[0] = '\0';
	 break;
     }

   /***** Put form to create/edit a game and show questions *****/
   Gam_PutFormsOneGame (&Games,ExistingNewGame);
  }

/*****************************************************************************/
/********************* Put forms to create/edit a game ***********************/
/*****************************************************************************/

void Gam_PutFormsOneGame (struct Gam_Games *Games,
			  Gam_ExistingNewGame_t ExistingNewGame)
  {
   extern const char *Hlp_ASSESSMENT_Games_edit_game;
   extern const char *Hlp_ASSESSMENT_Games_new_game;
   extern const char *Txt_Game;
   static void (*FunctionToDrawContextualIcons[]) (void *Args) =
     {
      [Gam_EXISTING_GAME] = Gam_PutIconsEditingOneGame,
      [Gam_NEW_GAME     ] = NULL,
     };
   static const char **HelpLink[] =
     {
      [Gam_EXISTING_GAME] = &Hlp_ASSESSMENT_Games_edit_game,
      [Gam_NEW_GAME     ] = &Hlp_ASSESSMENT_Games_new_game,
     };
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Initialize text / get text from database *****/
   switch (ExistingNewGame)
     {
      case Gam_EXISTING_GAME:
	 Gam_DB_GetGameTxt (Games->Game.GamCod,Txt);
	 break;
      case Gam_NEW_GAME:
         Txt[0] = '\0';
	 break;
     }

   /***** Begin box *****/
   Box_BoxBegin (NULL,
		 Games->Game.Title[0] ? Games->Game.Title :
					Txt_Game,
		 FunctionToDrawContextualIcons[ExistingNewGame],Games,
		 *HelpLink[ExistingNewGame],Box_NOT_CLOSABLE);

      /***** Put form to create/edit a game *****/
      Gam_PutFormEditionGame (Games,Txt,ExistingNewGame);

      /***** Show list of questions inside box *****/
      if (ExistingNewGame == Gam_EXISTING_GAME)
	 Gam_ListGameQuestions (Games);

   /***** End box ****/
   Box_BoxEnd ();

   /***** Show games again outside box *****/
   if (ExistingNewGame == Gam_NEW_GAME)
      Gam_ListAllGames (Games);
  }

/*****************************************************************************/
/********************* Put a form to create/edit a game **********************/
/*****************************************************************************/

static void Gam_PutFormEditionGame (struct Gam_Games *Games,
				    char Txt[Cns_MAX_BYTES_TEXT + 1],
			            Gam_ExistingNewGame_t ExistingNewGame)
  {
   extern const char *Hlp_ASSESSMENT_Games_edit_game;
   extern const char *Hlp_ASSESSMENT_Games_new_game;
   extern const char *Txt_Title;
   extern const char *Txt_Maximum_grade;
   extern const char *Txt_Result_visibility;
   extern const char *Txt_Description;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Create;
   static Act_Action_t NextAction[] =
     {
      [Gam_EXISTING_GAME] = ActChgGam,
      [Gam_NEW_GAME     ] = ActNewGam,
     };
   static Btn_Button_t Button[] =
     {
      [Gam_EXISTING_GAME] = Btn_CONFIRM_BUTTON,
      [Gam_NEW_GAME     ] = Btn_CREATE_BUTTON,
     };
   const char *TxtButton[] =
     {
      [Gam_EXISTING_GAME] = Txt_Save_changes,
      [Gam_NEW_GAME     ] = Txt_Create,
     };

   /***** Begin form *****/
   Frm_BeginForm (NextAction[ExistingNewGame]);
      Gam_PutPars (Games);

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

	 /***** Game title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_INPUT_TEXT ("Title",Gam_MAX_CHARS_TITLE,Games->Game.Title,
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
	       HTM_INPUT_FLOAT ("MaxGrade",0.0,DBL_MAX,0.01,Games->Game.MaxGrade,
				HTM_DONT_SUBMIT_ON_CHANGE,false,
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
	       TstVis_PutVisibilityCheckboxes (Games->Game.Visibility);
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

      /***** End table ****/
      HTM_TABLE_End ();

      /***** Send button *****/
      Btn_PutButton (Button[ExistingNewGame],
		     TxtButton[ExistingNewGame]);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************** Receive form to create a new game ********************/
/*****************************************************************************/

void Gam_ReceiveFormGame (void)
  {
   struct Gam_Games Games;
   Gam_ExistingNewGame_t ExistingNewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Check if I can edit games *****/
   if (!Gam_CheckIfICanEditGames ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   Games.Game.GamCod = Gam_GetPars (&Games);
   ExistingNewGame = (Games.Game.GamCod > 0) ? Gam_EXISTING_GAME :
					       Gam_NEW_GAME;

   /***** Receive game from form *****/
   Gam_ReceiveGameFieldsFromForm (&Games.Game,Txt);
   if (Gam_CheckGameFieldsReceivedFromForm (&Games.Game))
      /***** Create a new game or update an existing one *****/
      switch (ExistingNewGame)
	{
	 case Gam_EXISTING_GAME:
	    Gam_UpdateGame (&Games.Game,Txt);	// Update game data in database
	    break;
	 case Gam_NEW_GAME:
	    Gam_CreateGame (&Games.Game,Txt);	// Add new game to database
	    ExistingNewGame = Gam_EXISTING_GAME;
	    break;
	}

   /***** Show pending alerts */
   Ale_ShowAlerts (NULL);

   /***** Show current game and its questions *****/
   Gam_PutFormsOneGame (&Games,ExistingNewGame);
  }

static void Gam_ReceiveGameFieldsFromForm (struct Gam_Game *Game,
				           char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   char MaxGradeStr[64];

   /***** Get game title *****/
   Par_GetParText ("Title",Game->Title,Gam_MAX_BYTES_TITLE);

   /***** Get maximum grade *****/
   Par_GetParText ("MaxGrade",MaxGradeStr,sizeof (MaxGradeStr) - 1);
   Game->MaxGrade = Str_GetDoubleFromStr (MaxGradeStr);
   if (Game->MaxGrade < 0.0)	// Only positive values allowed
      Game->MaxGrade = 0.0;

   /***** Get visibility *****/
   Game->Visibility = TstVis_GetVisibilityFromForm ();

   /***** Get game text *****/
   Par_GetParHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
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
	 Ale_CreateAlert (Ale_WARNING,NULL,
			  Txt_Already_existed_a_game_with_the_title_X,
			  Game->Title);
	}
     }
   else	// If there is not a game title
     {
      NewGameIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
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

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Games.Game))
      Err_NoPermissionExit ();

   /***** Show form to create a new question in this game *****/
   Qst_RequestSelectQstsForGame (&Games);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
                        true,	// List game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/**************** List several test questions for selection ******************/
/*****************************************************************************/

void Gam_ListQstsToAddToGame (void)
  {
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Games.Game))
      Err_NoPermissionExit ();

   /***** List several test questions for selection *****/
   Qst_ListQuestionsToSelectForGame (&Games);
  }

/*****************************************************************************/
/****************** Write parameter with index of question *******************/
/*****************************************************************************/

void Gam_PutParQstInd (unsigned QstInd)
  {
   Par_PutParUnsigned (NULL,"QstInd",QstInd);
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

unsigned Gam_GetParQstInd (void)
  {
   long QstInd;

   if ((QstInd = Par_GetParLong ("QstInd")) <= 0)	// In games, question index should be 1, 2, 3...
      Err_WrongQuestionIndexExit ();

   return (unsigned) QstInd;
  }

/*****************************************************************************/
/************************ List the questions of a game ***********************/
/*****************************************************************************/

static void Gam_ListGameQuestions (struct Gam_Games *Games)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_Questions;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   bool ICanEditQuestions = Gam_CheckIfEditable (&Games->Game);

   /***** Get data of questions from database *****/
   NumQsts = Gam_DB_GetGameQuestionsBasic (&mysql_res,Games->Game.GamCod);

   /***** Begin box *****/
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
      Gam_ListOneOrMoreQuestionsForEdition (Games,NumQsts,mysql_res,
					    ICanEditQuestions);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************* List game questions for edition ***********************/
/*****************************************************************************/

static void Gam_ListOneOrMoreQuestionsForEdition (struct Gam_Games *Games,
						  unsigned NumQsts,
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
   MaxQstInd = Gam_DB_GetMaxQuestionIndexInGame (Games->Game.GamCod);	// 0 is no questions in game

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
						 Gam_PutParsOneQst,Games);
	       else
		  Ico_PutIconRemovalNotAllowed ();

	       /* Put icon to move up the question */
	       if (ICanEditQuestions && QstInd > 1)
		  Lay_PutContextualLinkOnlyIcon (ActUp_GamQst,Anchor,
						 Gam_PutParsOneQst,Games,
						 "arrow-up.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	       /* Put icon to move down the question */
	       if (ICanEditQuestions && QstInd < MaxQstInd)
		  Lay_PutContextualLinkOnlyIcon (ActDwnGamQst,Anchor,
						 Gam_PutParsOneQst,Games,
						 "arrow-down.svg",Ico_BLACK);
	       else
		  Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,
		                  Txt_Movement_not_allowed);

	       /* Put icon to edit the question */
	       if (ICanEditQuestions)
		  Ico_PutContextualIconToEdit (ActEdiOneTstQst,NULL,
					       Qst_PutParQstCod,&Question.QstCod);

	    HTM_TD_End ();

	    /***** Question *****/
	    QuestionExists = Qst_GetQstDataByCod (&Question);
	    Qst_ListQuestionForEdition (&Question,QstInd,QuestionExists,Anchor);

	 /***** End row *****/
	 HTM_TR_End ();

	 /***** Free anchor string *****/
	 Frm_FreeAnchorStr (&Anchor);

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
   Ico_PutContextualIconToAdd (ActAddOneGamQst,NULL,Gam_PutPars,Games);
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
   const char *Ptr;
   char LongStr[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   long QstCod;
   unsigned MaxQstInd;
   unsigned NumQstsAdded;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Games.Game))
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
	 Par_GetNextStrUntilSeparParMult (&Ptr,LongStr,Cns_MAX_DECIMAL_DIGITS_LONG);
	 if (sscanf (LongStr,"%ld",&QstCod) != 1)
	    Err_WrongQuestionExit ();

	 /* Check if question is already present in game */
	 if (Gam_DB_GetQstIndFromQstCod (Games.Game.GamCod,QstCod) == 0)	// This question is not yet in this game
	   {
	    /* Get current maximum index */
	    MaxQstInd = Gam_DB_GetMaxQuestionIndexInGame (Games.Game.GamCod);	// 0 is no questions in game

	    /* Insert question in the table of questions */
            Gam_DB_InsertQstInGame (Games.Game.GamCod,MaxQstInd + 1,QstCod);

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

   /***** Get game data again (to update number of questions) *****/
   Gam_GetGameDataByCod (&Games.Game);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
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

void Gam_ReqRemQstFromGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove;
   struct Gam_Games Games;
   unsigned QstInd;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Games.Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstInd = Gam_GetParQstInd ();

   /***** Show question and button to remove question *****/
   Games.QstInd = QstInd;
   Ale_ShowAlertAndButton (ActRemGamQst,NULL,NULL,
			   Gam_PutParsOneQst,&Games,
			   Btn_REMOVE_BUTTON,Txt_Remove,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
			   QstInd);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
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
   unsigned QstInd;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Games.Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstInd = Gam_GetParQstInd ();

   /***** Remove the question from all tables *****/
   /* Remove answers from this test question */
   Mch_DB_RemUsrAnswersOfAQuestion (Games.Game.GamCod,QstInd);

   /* Remove the question itself */
   Gam_DB_RemoveQstFromGame (Games.Game.GamCod,QstInd);

   /* Change indexes of questions greater than this */
   Mch_DB_UpdateIndexesOfQstsGreaterThan (Games.Game.GamCod,QstInd);
   Gam_DB_UpdateIndexesOfQstsGreaterThan (Games.Game.GamCod,QstInd);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
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
   unsigned QstIndTop;
   unsigned QstIndBottom;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Games.Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstIndBottom = Gam_GetParQstInd ();

   /***** Move up question *****/
   if (QstIndBottom > 1)	// 2, 3, 4...
     {
      /* Indexes of questions to be exchanged */
      QstIndTop = Gam_DB_GetPrevQuestionIndexInGame (Games.Game.GamCod,QstIndBottom);
      if (QstIndTop == 0)
	 Err_WrongQuestionIndexExit ();

      /* Exchange questions */
      Gam_ExchangeQuestions (Games.Game.GamCod,QstIndTop,QstIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
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
   unsigned QstIndTop;
   unsigned QstIndBottom;
   unsigned MaxQstInd;	// 0 if no questions

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Check if game has matches *****/
   if (!Gam_CheckIfEditable (&Games.Game))
      Err_NoPermissionExit ();

   /***** Get question index *****/
   QstIndTop = Gam_GetParQstInd ();

   /***** Get maximum question index *****/
   MaxQstInd = Gam_DB_GetMaxQuestionIndexInGame (Games.Game.GamCod);	// 0 is no questions in game

   /***** Move down question *****/
   if (QstIndTop < MaxQstInd)
     {
      /* Indexes of questions to be exchanged */
      QstIndBottom = Gam_DB_GetNextQuestionIndexInGame (Games.Game.GamCod,QstIndTop);
      if (QstIndBottom == Gam_AFTER_LAST_QUESTION)
	 Err_WrongQuestionIndexExit ();

      /* Exchange questions */
      Gam_ExchangeQuestions (Games.Game.GamCod,QstIndTop,QstIndBottom);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
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
/************* Request the creation of a new match as a teacher **************/
/*****************************************************************************/

void Gam_ReqNewMatch (void)
  {
   struct Gam_Games Games;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);

   /***** Reset game *****/
   Gam_ResetGame (&Games.Game);

   /***** Get parameters *****/
   if ((Games.Game.GamCod = Gam_GetPars (&Games)) <= 0)
      Err_WrongGameExit ();
   Gam_GetGameDataByCod (&Games.Game);

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,
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
