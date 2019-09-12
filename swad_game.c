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

#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_role.h"
#include "swad_setting.h"
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

#define Gam_NEW_MATCH_SECTION_ID	"new_match"

#define Gam_AFTER_LAST_QUESTION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database to indicate that a match is finished

#define Gam_ICON_CLOSE		"fas fa-times"
#define Gam_ICON_PLAY		"fas fa-play"
#define Gam_ICON_PAUSE		"fas fa-pause"
#define Gam_ICON_PREVIOUS	"fas fa-step-backward"
#define Gam_ICON_NEXT		"fas fa-step-forward"
#define Gam_ICON_RESULTS	"fas fa-chart-bar"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Gam_NUM_SHOWING 4
typedef enum
  {
   Gam_WORDING,	// Showing only the question wording
   Gam_ANSWERS,	// Showing the question wording and the answers
   Gam_REQUEST,	// Requesting confirmation to show the results
   Gam_RESULTS,	// Showing the results
  } Gam_Showing_t;
#define Gam_SHOWING_DEFAULT Gam_WORDING
const char *Gam_ShowingStringsDB[Gam_NUM_SHOWING] =
  {
   "wording",
   "answers",
   "request",
   "results",
  };

struct Match
  {
   long MchCod;
   long GamCod;
   long UsrCod;
   time_t TimeUTC[2];
   char Title[Gam_MAX_BYTES_TITLE + 1];
   struct
     {
      unsigned QstInd;	// 0 means that the game has not started. First question has index 1.
      long QstCod;
      time_t QstStartTimeUTC;
      Gam_Showing_t Showing;
      bool BeingPlayed;
      unsigned NumPlayers;
     } Status;
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

long Gam_CurrentGamCod = -1L;	// Used as parameter in contextual links
long Gam_CurrentMchCod = -1L;	// Used as parameter in contextual links
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
static void Gam_ShowOneGame (long GamCod,
                             bool ShowOnlyThisGame,
                             bool ListGameQuestions,
                             bool PutFormNewMatch);
static void Gam_WriteAuthor (struct Game *Game);

static void Gam_PutFormsToRemEditOneGame (const struct Game *Game,
					  const char *Anchor);
static void Gam_PutParams (void);
static void Gam_PutParamCurrentMchCod (void);
static void Gam_PutHiddenParamOrder (void);
static void Gam_GetParamOrder (void);

static void Gam_GetGameTxtFromDB (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

static void Gam_PutParamMatchCod (long MchCod);
static long Gam_GetParamMatchCod (void);

static bool Gam_CheckIfSimilarGameExists (struct Game *Game);
static void Gam_ShowLstGrpsToCreateMatch (void);

static void Gam_CreateGame (struct Game *Game,const char *Txt);
static void Gam_UpdateGame (struct Game *Game,const char *Txt);
static void Gam_CreateGrps (long MchCod);
static void Gam_GetAndWriteNamesOfGrpsAssociatedToMatch (struct Match *Match);
static bool Gam_CheckIfIPlayThisMatchBasedOnGrps (long GamCod);

static unsigned Gam_GetNumQstsGame (long GamCod);
static void Gam_PutParamQstInd (unsigned QstInd);
static unsigned Gam_GetParamQstInd (void);
static void Gam_PutParamAnswer (unsigned AnsInd);
static unsigned Gam_GetParamAnswer (void);
static unsigned Gam_GetQstIndFromStr (const char *UnsignedStr);
static Gam_Showing_t Gam_GetShowingFromStr (const char *Str);
static void Gam_RemAnswersOfAQuestion (long GamCod,unsigned QstInd);

static long Gam_GetQstCodFromQstInd (long GamCod,unsigned QstInd);
static unsigned Gam_GetMaxQuestionIndexInGame (long GamCod);
static unsigned Gam_GetPrevQuestionIndexInGame (long GamCod,unsigned QstInd);
static unsigned Gam_GetNextQuestionIndexInGame (long GamCod,unsigned QstInd);
static void Gam_ListGameQuestions (struct Game *Game);
static void Gam_ListOneOrMoreQuestionsForEdition (long GamCod,unsigned NumQsts,
                                                  MYSQL_RES *mysql_res);
static void Gam_PutIconToAddNewQuestions (void);
static void Gam_PutButtonToAddNewQuestions (void);

static void Gam_AllocateListSelectedQuestions (void);
static void Gam_FreeListsSelectedQuestions (void);
static unsigned Gam_CountNumQuestionsInList (void);

static unsigned Gam_GetNumAnswerers (struct Match *Match);
static unsigned Gam_GetNumUsrsWhoAnswered (long MchCod,unsigned QstInd,unsigned AnsInd);
static void Gam_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs);

static void Gam_PutParamsOneQst (void);

static void Gam_ExchangeQuestions (long GamCod,
                                   unsigned QstIndTop,unsigned QstIndBottom);

static void Gam_ListMatches (struct Game *Game,bool PutFormNewMatch);
static void Gam_PutIconToPlayNewMatch (void);
static void Gam_ListOneOrMoreMatches (struct Game *Game,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res);
static void Gam_GetMatchDataFromRow (MYSQL_RES *mysql_res,
				     struct Match *Match);
static void Gam_PutButtonNewMatch (long GamCod);

static void Gam_PutFormNewMatch (struct Game *Game);

static long Gam_CreateMatch (long GamCod,char Title[Gam_MAX_BYTES_TITLE + 1]);
static void Gam_UpdateMatchStatusInDB (struct Match *Match);

static void Gam_UpdateElapsedTimeInQuestion (struct Match *Match);
static void Gam_GetElapsedTimeInQuestion (struct Match *Match,
				          struct Time *Time);
static void Gam_GetElapsedTimeInMatch (struct Match *Match,
				       struct Time *Time);
static void Gam_GetElapsedTime (unsigned NumRows,MYSQL_RES *mysql_res,
				struct Time *Time);

static void Gam_SetMatchStatusToPrev (struct Match *Match);
static void Gam_SetMatchStatusToNext (struct Match *Match);
static void Gam_ShowMatchStatusForTch (struct Match *Match);
static void Gam_ShowMatchStatusForStd (struct Match *Match);
static void Gam_ShowLeftColumnTch (struct Match *Match);
static void Gam_ShowLeftColumnStd (struct Match *Match);
static void Gam_PutMatchControlButtons (struct Match *Match);
// static void Gam_PutCheckboxResult (struct Match *Match);
static void Gam_ShowNumQstInGame (struct Match *Match);
static void Gam_ShowNumPlayers (struct Match *Match);
static void Gam_ShowMatchTitle (struct Match *Match);
static void Gam_ShowQuestionAndAnswersTch (struct Match *Match);
static void Gam_ShowQuestionAndAnswersStd (struct Match *Match);

static void Gam_PutBigButton (Act_Action_t NextAction,long MchCod,
			      const char *Icon,const char *Txt);
static void Gam_PutBigButtonOff (const char *Icon);
static void Gam_PutBigButtonClose (void);

static void Gam_ShowWaitImage (const char *Txt);

static void Gam_RemoveOldPlayers (void);
static void Gam_UpdateMatchAsBeingPlayed (long MchCod);
static void Gam_SetMatchAsNotBeingPlayed (long MchCod);
static bool Gam_GetIfMatchIsBeingPlayed (long MchCod);
static void Gam_RegisterMeAsPlayerInMatch (long MchCod);
static void Gam_GetNumPlayers (struct Match *Match);

static void Gam_ShowMatchStatusForStd (struct Match *Match);
static int Gam_GetQstAnsFromDB (long MchCod,unsigned QstInd);

/*****************************************************************************/
/*************************** List all the games ******************************/
/*****************************************************************************/

void Gam_SeeAllGames (void)
  {
   /***** Get parameters *****/
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Show all the games *****/
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
   extern const char *Txt_No_games;
   Gam_Order_t Order;
   struct Pagination Pagination;
   unsigned NumGame;

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
      Tbl_StartTableWideMargin (2);
      fprintf (Gbl.F.Out,"<tr>");
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
      fprintf (Gbl.F.Out,"</tr>");

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
/******************* Check if I can create a new game **********************/
/*****************************************************************************/

static bool Gam_CheckIfICanEditGames (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
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
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
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

static void Gam_ShowOneGame (long GamCod,
                             bool ShowOnlyThisGame,
                             bool ListGameQuestions,
                             bool PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Game;
   extern const char *Txt_Today;
   extern const char *Txt_View_game;
   extern const char *Txt_No_of_questions;
   extern const char *Txt_New_match;
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   struct Game Game;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Start box *****/
   if (ShowOnlyThisGame)
      Box_StartBox (NULL,Txt_Game,NULL,
                    Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

   /***** Get data of this game *****/
   Game.GamCod = GamCod;
   Gam_GetDataOfGameByCod (&Game);
   Gam_CurrentGamCod = Game.GamCod;	// Used as parameter in contextual links

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Game.GamCod,&Anchor);

   /***** Start table *****/
   if (ShowOnlyThisGame)
      Tbl_StartTableWide (2);

   /***** Start first row of this game *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Icons related to this game *****/
   if (Game.Status.ICanEdit)
     {
      fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"CONTEXT_COL");
      if (!ShowOnlyThisGame)
	 fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
      fprintf (Gbl.F.Out,"\">");

      /* Icons to remove/edit this game */
      Gam_PutFormsToRemEditOneGame (&Game,Anchor);

      if (ShowOnlyThisGame)
	 /* Icon to start a new match */
	 Lay_PutContextualLinkOnlyIcon (ActReqNewMchTch,Gam_NEW_MATCH_SECTION_ID,
					Gam_PutParams,
					"play.svg",
					Txt_New_match);

      fprintf (Gbl.F.Out,"</td>");
     }

   /***** Start date/time *****/
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"gam_date_start_%u\" class=\"%s LEFT_TOP",
	    UniqueId,
            Game.Status.Visible ? "DATE_GREEN" :
                                  "DATE_GREEN_LIGHT");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   if (Game.TimeUTC[Gam_START_TIME])
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
			 "writeLocalDateHMSFromUTC('gam_date_start_%u',%ld,"
			 "%u,'<br />','%s',true,true,0x7);"
			 "</script>",
	       UniqueId,Game.TimeUTC[Gam_START_TIME],
	       (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
   fprintf (Gbl.F.Out,"</td>");

   /***** End date/time *****/
   fprintf (Gbl.F.Out,"<td id=\"gam_date_end_%u\" class=\"%s LEFT_TOP",
            UniqueId,
            Game.Status.Visible ? "DATE_GREEN" :
                                  "DATE_GREEN_LIGHT");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   if (Game.TimeUTC[Gam_END_TIME])
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
			 "writeLocalDateHMSFromUTC('gam_date_end_%u',%ld,"
			 "%u,'<br />','%s',false,true,0x7);"
			 "</script>",
	       UniqueId,Game.TimeUTC[Gam_END_TIME],
	       (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
   fprintf (Gbl.F.Out,"</td>");

   /***** Game title and main data *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Game title */
   Lay_StartArticle (Anchor);
   Frm_StartForm (ActSeeGam);
   Gam_PutParamGameCod (GamCod);
   Frm_LinkFormSubmit (Txt_View_game,
                       Game.Status.Visible ? "ASG_TITLE" :
	                                     "ASG_TITLE_LIGHT",NULL);
   fprintf (Gbl.F.Out,"%s</a>",
            Game.Title);
   Frm_EndForm ();
   Lay_EndArticle ();

   /* Number of questions */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: %u</div>",
            Game.Status.Visible ? "ASG_GRP" :
        	                  "ASG_GRP_LIGHT",
            Txt_No_of_questions,
            Game.NumQsts);

   fprintf (Gbl.F.Out,"</td>");

   /***** End 1st row of this game *****/
   fprintf (Gbl.F.Out,"</tr>");

   /***** Start 2nd row of this game *****/
   fprintf (Gbl.F.Out,"<tr>");

   /***** Author of the game *****/
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Gam_WriteAuthor (&Game);
   fprintf (Gbl.F.Out,"</td>");

   /***** Text of the game *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Gam_GetGameTxtFromDB (Game.GamCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<div class=\"PAR %s\">%s</div>"
                      "</td>",
            Game.Status.Visible ? "DAT" :
        	                  "DAT_LIGHT",
            Txt);

   /***** End 2nd row of this game *****/
   fprintf (Gbl.F.Out,"</tr>");

   /***** End table *****/
   if (ShowOnlyThisGame)
      Tbl_EndTable ();
   else
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   if (ShowOnlyThisGame)
     {
      /***** List matches *****/
      Gam_ListMatches (&Game,PutFormNewMatch);

      /***** Write questions of this game *****/
      if (ListGameQuestions)
	 Gam_ListGameQuestions (&Game);

      /***** End box *****/
      Box_EndBox ();
     }
  }

/*****************************************************************************/
/*********************** Write the author of a game ************************/
/*****************************************************************************/

static void Gam_WriteAuthor (struct Game *Game)
  {
   Usr_WriteAuthor1Line (Game->UsrCod,!Game->Status.Visible);
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
   extern const char *Txt_Reset;

   /***** Put icon to remove game *****/
   Ico_PutContextualIconToRemove (ActReqRemGam,Gam_PutParams);

   /***** Put icon to reset game *****/
   Lay_PutContextualLinkOnlyIcon (ActReqRstGam,NULL,Gam_PutParams,
				  "recycle.svg",
				  Txt_Reset);

   /***** Put icon to hide/show game *****/
   if (Game->Status.Visible)
      Ico_PutContextualIconToHide (ActHidGam,Anchor,Gam_PutParams);
   else
      Ico_PutContextualIconToUnhide (ActShoGam,Anchor,Gam_PutParams);

   /***** Put icon to edit game *****/
   Ico_PutContextualIconToEdit (ActEdiOneGam,Gam_PutParams);
  }

/*****************************************************************************/
/******************** Params used to edit/play a game ************************/
/*****************************************************************************/

static void Gam_PutParams (void)
  {
   if (Gam_CurrentGamCod > 0)
      Gam_PutParamGameCod (Gam_CurrentGamCod);

   Gam_PutHiddenParamOrder ();
   Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
  }

static void Gam_PutParamCurrentMchCod (void)
  {
   if (Gam_CurrentMchCod > 0)
      Gam_PutParamMatchCod (Gam_CurrentMchCod);
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
      "StartTime DESC,EndTime DESC,games.Title DESC",	// Gam_ORDER_BY_START_DATE
      "EndTime DESC,StartTime DESC,games.Title DESC",	// Gam_ORDER_BY_END_DATE
      "games.Title DESC",				// Gam_ORDER_BY_TITLE
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned NumGame;

   /***** Free list of games *****/
   if (Gbl.Games.LstIsRead)
      Gam_FreeListGames ();

   /***** Get list of games from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get games",
			     "SELECT games.GamCod,"
			            "MIN(gam_matches.StartTime) AS StartTime,"
			            "MAX(gam_matches.EndTime) AS EndTime"
			     " FROM games"
			     " LEFT JOIN gam_matches"
			     " ON games.GamCod=gam_matches.GamCod"
			     " WHERE games.CrsCod=%ld"
			     " GROUP BY games.GamCod"
			     " ORDER BY %s",
			     Gbl.Hierarchy.Crs.CrsCod,
			     OrderBySubQuery[Gbl.Games.SelectedOrder]);
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
         /* Get next game code */
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
			     "SELECT games.GamCod,"					// row[0]
			            "games.Hidden,"					// row[1]
			            "games.UsrCod,"					// row[2]
			            "games.Title"					// row[3]
			     " FROM games"
			     " LEFT JOIN gam_matches"
			     " ON games.GamCod=gam_matches.GamCod"
			     " WHERE games.GamCod=%ld",
			     Game->GamCod);
   if (NumRows) // Game found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the game (row[0]) */
      Game->GamCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get whether the game is hidden (row[1]) */
      Game->Status.Visible = (row[1][0] == 'N');

      /* Get author of the game (row[2]) */
      Game->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get the title of the game (row[3]) */
      Str_Copy (Game->Title,row[3],
                Gam_MAX_BYTES_TITLE);

      /* Get number of questions */
      Game->NumQsts = Gam_GetNumQstsGame (Game->GamCod);

      /* Can I view results of the game?
         Can I edit game? */
      switch (Gbl.Usrs.Me.Role.Logged)
        {
         case Rol_STD:
            Game->Status.ICanViewResults = Game->NumQsts != 0 &&
                                           Game->Status.Visible;
            Game->Status.ICanEdit         = false;
            break;
         case Rol_NET:
            Game->Status.ICanViewResults = Game->NumQsts != 0;
            Game->Status.ICanEdit        = false;
            break;
         case Rol_TCH:
         case Rol_DEG_ADM:
         case Rol_CTR_ADM:
         case Rol_INS_ADM:
            Game->Status.ICanViewResults = Game->NumQsts != 0;
            Game->Status.ICanEdit        = true;
            break;
         case Rol_SYS_ADM:
            Game->Status.ICanViewResults = Game->NumQsts != 0;
            Game->Status.ICanEdit        = true;
            break;
         default:
            Game->Status.ICanViewResults = false;
            Game->Status.ICanEdit        = false;
            break;
        }
     }
   else
     {
      /* Initialize to empty game */
      Game->GamCod                  = -1L;
      Game->UsrCod                  = -1L;
      Game->Title[0]                = '\0';
      Game->NumQsts                 = 0;
      Game->Status.Visible          = true;
      Game->Status.ICanViewResults  = false;
      Game->Status.ICanEdit         = false;
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   if (Game->GamCod > 0)
     {
      /***** Get start and end times from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get game data",
				"SELECT UNIX_TIMESTAMP(MIN(StartTime)),"	// row[0]
				       "UNIX_TIMESTAMP(MAX(EndTime))"		// row[1]
				" FROM gam_matches"
				" WHERE GamCod=%ld",
				Game->GamCod);
      if (NumRows)
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get start date (row[0] holds the start UTC time) */
	 Game->TimeUTC[Gam_START_TIME] = Dat_GetUNIXTimeFromStr (row[0]);

	 /* Get end   date (row[1] holds the end   UTC time) */
	 Game->TimeUTC[Gam_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[1]);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }
   else
     {
      Game->TimeUTC[Gam_START_TIME] =
      Game->TimeUTC[Gam_END_TIME  ] = (time_t) 0;
     }
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
			     "SELECT Txt FROM games WHERE GamCod=%ld",
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
/******************** Write parameter with code of match **********************/
/*****************************************************************************/

static void Gam_PutParamMatchCod (long MchCod)
  {
   Par_PutHiddenParamLong ("MchCod",MchCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of match **********************/
/*****************************************************************************/

static long Gam_GetParamMatchCod (void)
  {
   /***** Get code of match *****/
   return Par_GetParToLong ("MchCod");
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
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not remove this game.");

   /***** Show question and button to remove game *****/
   Gam_CurrentGamCod = Game.GamCod;
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
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not remove this game.");

   /***** Remove all the questions in this game *****/
   DB_QueryDELETE ("can not remove questions of a game",
		   "DELETE FROM gam_questions WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Remove all the matches in this game *****/
   /* Remove groups in matches of the game */
   DB_QueryDELETE ("can not remove the groups associated to matches of a game",
		   "DELETE FROM gam_grp USING gam_grp,gam_matches"
		   " WHERE gam_matches.GrpCod=%ld"
		   " AND gam_matches.MchCod=gam_grp.MchCod",
		   Game.GamCod);
   /* Remove matches of the game */
   DB_QueryDELETE ("can not remove matches of a game",
		   "DELETE FROM gam_matches WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Remove game *****/
   DB_QueryDELETE ("can not remove game",
		   "DELETE FROM games WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Game_X_removed,
                  Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/***************** Ask for confirmation of reset of a game *****************/
/*****************************************************************************/

void Gam_AskResetGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_reset_the_game_X;
   extern const char *Txt_Reset_game;
   struct Game Game;

   /***** Get parameters *****/
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not reset this game.");

   /***** Ask for confirmation of reset *****/
   Gam_CurrentGamCod = Game.GamCod;
   Ale_ShowAlertAndButton (ActRstGam,NULL,NULL,
			   Gam_PutParams,
                           Btn_REMOVE_BUTTON,
			   Txt_Reset_game,
			   Ale_QUESTION,Txt_Do_you_really_want_to_reset_the_game_X,
			   Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/******************************* Reset a game ******************************/
/*****************************************************************************/

void Gam_ResetGame (void)
  {
   extern const char *Txt_Game_X_reset;
   struct Game Game;

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not reset this game.");

   /***** Reset all the answers in this game *****/
   DB_QueryUPDATE ("can not reset answers of a game",
		   "UPDATE gam_answers,gam_questions"
		   " SET gam_answers.NumUsrs=0"
		   " WHERE gam_questions.GamCod=%ld"
		   " AND gam_questions.QstCod=gam_answers.QstCod",
		   Game.GamCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Game_X_reset,
                  Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/******************************** Hide a game ******************************/
/*****************************************************************************/

void Gam_HideGame (void)
  {
   struct Game Game;

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not hide this game.");

   /***** Hide game *****/
   DB_QueryUPDATE ("can not hide game",
		   "UPDATE games SET Hidden='Y' WHERE GamCod=%ld",
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

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not unhide this game.");

   /***** Show game *****/
   DB_QueryUPDATE ("can not show game",
		   "UPDATE games SET Hidden='N' WHERE GamCod=%ld",
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
			  "SELECT COUNT(*) FROM games"
			  " WHERE CrsCod=%ld AND Title='%s'"
			  " AND GamCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,Game->Title,
			  Game->GamCod) != 0);
  }

/*****************************************************************************/
/*********************** Put a form to create a new game *********************/
/*****************************************************************************/

void Gam_RequestCreatOrEditGame (void)
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
   struct Game Game;
   bool ItsANewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get parameters *****/
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get the code of the game *****/
   ItsANewGame = ((Game.GamCod = Gam_GetParamGameCod ()) == -1L);

   /***** Get from the database the data of the game *****/
   if (ItsANewGame)
     {
      /***** Put link (form) to create new game *****/
      if (!Gam_CheckIfICanEditGames ())
         Lay_ShowErrorAndExit ("You can not create a new game here.");

      /* Initialize to empty game */
      Game.GamCod = -1L;
      Game.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Game.TimeUTC[Gam_START_TIME] = (time_t) 0;
      Game.TimeUTC[Gam_END_TIME  ] = (time_t) 0;
      Game.Title[0]                = '\0';
      Game.NumQsts                 = 0;
      Game.Status.Visible          = true;
      Game.Status.ICanViewResults  = false;
     }
   else
     {
      /* Get data of the game from database */
      Gam_GetDataOfGameByCod (&Game);
      if (!Game.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this game.");

      /* Get text of the game from database */
      Gam_GetGameTxtFromDB (Game.GamCod,Txt);
     }

   /***** Start form *****/
   Gam_CurrentGamCod = Game.GamCod;
   Frm_StartForm (ItsANewGame ? ActNewGam :
	                        ActChgGam);
   Gam_PutParams ();

   /***** Start box and table *****/
   if (ItsANewGame)
      Box_StartBoxTable (NULL,Txt_New_game,NULL,
                         Hlp_ASSESSMENT_Games_new_game,Box_NOT_CLOSABLE,2);
   else
      Box_StartBoxTable (NULL,
                         Game.Title[0] ? Game.Title :
                	                 Txt_Edit_game,
                         NULL,
                         Hlp_ASSESSMENT_Games_edit_game,Box_NOT_CLOSABLE,2);

   /***** Game title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Title\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" id=\"Title\" name=\"Title\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Title,
            Gam_MAX_CHARS_TITLE,Game.Title);

   /***** Game text *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"Txt\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea id=\"Txt\" name=\"Txt\""
                      " cols=\"60\" rows=\"10\">",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Description);
   if (!ItsANewGame)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>");

   /***** End table, send button and end box *****/
   if (ItsANewGame)
      Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_game);
   else
      Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show questions of the game ready to be edited *****/
   if (!ItsANewGame)
      Gam_ListGameQuestions (&Game);
  }

/*****************************************************************************/
/***************** Show list of groups to create a new match *****************/
/*****************************************************************************/

static void Gam_ShowLstGrpsToCreateMatch (void)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.Num)
     {
      /***** Start box and table *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s:"
	                 "</td>"
                         "<td class=\"LEFT_TOP\">",
               The_ClassFormInBox[Gbl.Prefs.Theme],
               Txt_Groups);
      Box_StartBoxTable ("95%",NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,0);

      /***** First row: checkbox to select the whole course *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"7\" class=\"DAT LEFT_MIDDLE\">"
                         "<label>"
                         "<input type=\"checkbox\""
                         " id=\"WholeCrs\" name=\"WholeCrs\" value=\"Y\""
                         " checked=\"checked\""
                         " onclick=\"uncheckChildren(this,'GrpCods')\" />"
	                 "%s %s"
	                 "</label>"
	                 "</td>"
	                 "</tr>",
               Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttSvyMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
                                            -1L,	// -1 means "New match"
					    Grp_MATCH);

      /***** End table and box *****/
      Box_EndBoxTable ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
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
   ItsANewGame = ((NewGame.GamCod = Gam_GetParamGameCod ()) == -1L);

   if (!ItsANewGame)
     {
      /* Get data of the old (current) game from database */
      OldGame.GamCod = NewGame.GamCod;
      Gam_GetDataOfGameByCod (&OldGame);
      if (!OldGame.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this game.");
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
				"INSERT INTO games"
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
		   "UPDATE games"
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
			  "SELECT COUNT(*) FROM gam_grp"
			  " WHERE MchCod=%ld AND GrpCod=%ld",
			  MchCod,GrpCod) != 0);
  }

/*****************************************************************************/
/******************** Remove one group from all the games ********************/
/*****************************************************************************/
// TODO: Check if this function should be called when removing group

void Gam_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the games *****/
   DB_QueryDELETE ("can not remove group"
	           " from the associations between games and groups",
		   "DELETE FROM gam_grp WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/**************** Remove groups of one type from all the games ***************/
/*****************************************************************************/
// TODO: Check if this function should be called when removing group type

void Gam_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the games *****/
   DB_QueryDELETE ("can not remove groups of a type"
	           " from the associations between games and groups",
		   "DELETE FROM gam_grp USING crs_grp,gam_grp"
		   " WHERE crs_grp.GrpTypCod=%ld"
		   " AND crs_grp.GrpCod=gam_grp.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/******************* Create groups associated to a match *********************/
/*****************************************************************************/

static void Gam_CreateGrps (long MchCod)
  {
   unsigned NumGrpSel;

   /***** Create groups associated to the match *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to a match",
		      "INSERT INTO gam_grp"
		      " (MchCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      MchCod,Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/************* Get and write the names of the groups of a match **************/
/*****************************************************************************/

static void Gam_GetAndWriteNamesOfGrpsAssociatedToMatch (struct Match *Match)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to a match from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get groups of a match",
			     "SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
			     " FROM gam_grp,crs_grp,crs_grp_types"
			     " WHERE gam_grp.MchCod=%ld"
			     " AND gam_grp.GrpCod=crs_grp.GrpCod"
			     " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
			     " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
			     Match->MchCod);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<div class=\"ASG_GRP\">%s: ",
            NumRows == 1 ? Txt_Group  :
                           Txt_Groups);

   /***** Write groups *****/
   if (NumRows) // Groups found...
     {
      /* Get and write the group types and names */
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Write group type name and group name */
         fprintf (Gbl.F.Out,"%s %s",row[0],row[1]);

         if (NumRows >= 2)
           {
            if (NumRow == NumRows-2)
               fprintf (Gbl.F.Out," %s ",Txt_and);
            if (NumRows >= 3)
              if (NumRow < NumRows-2)
                  fprintf (Gbl.F.Out,", ");
           }
        }
     }
   else
      fprintf (Gbl.F.Out,"%s %s",
               Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);

   fprintf (Gbl.F.Out,"</div>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Remove all the games of a place on the hierarchy **************/
/************* (country, institution, centre, degree or course) **************/
/*****************************************************************************/

void Gam_RemoveGames (Hie_Level_t Scope,long Cod)
  {
   /***** Remove all the answers in course games *****/
   DB_QueryDELETE ("can not remove answers of games"
		   " in a place on the hierarchy"
		   "DELETE FROM gam_answers"
		   " USING games,gam_questions,gam_answers"
		   " WHERE games.Scope='%s' AND games.Cod=%ld"
		   " AND games.GamCod=gam_questions.GamCod"
		   " AND gam_questions.QstCod=gam_answers.QstCod",
                   Sco_GetDBStrFromScope (Scope),Cod);

   /***** Remove all the questions in course games *****/
   DB_QueryDELETE ("can not remove questions of games"
		   " in a place on the hierarchy",
		   "DELETE FROM gam_questions"
		   " USING games,gam_questions"
		   " WHERE games.Scope='%s' AND games.Cod=%ld"
		   " AND games.GamCod=gam_questions.GamCod",
                   Sco_GetDBStrFromScope (Scope),Cod);

   /***** Remove groups *****/
   DB_QueryDELETE ("can not remove all the groups"
	           " associated to games of a course",
		   "DELETE FROM gam_grp"
		   " USING games,gam_grp"
		   " WHERE games.Scope='%s' AND games.Cod=%ld"
		   " AND games.GamCod=gam_grp.GamCod",
                   Sco_GetDBStrFromScope (Scope),Cod);

   /***** Remove course games *****/
   DB_QueryDELETE ("can not remove all the games in a place on the hierarchy",
		   "DELETE FROM games WHERE Scope='%s' AND Cod=%ld",
                   Sco_GetDBStrFromScope (Scope),Cod);
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a match **************/
/*****************************************************************************/

static bool Gam_CheckIfIPlayThisMatchBasedOnGrps (long MchCod)
  {
   /***** Get if I can play a match from database *****/
   return (DB_QueryCOUNT ("can not check if I can play a match",
			  "SELECT COUNT(*) FROM gam_matches"
			  " WHERE MchCod=%ld"
			  " AND (MchCod NOT IN (SELECT MchCod FROM gam_grp) OR"
			  " MchCod IN (SELECT gam_grp.MchCod FROM gam_grp,crs_grp_usr"
			  " WHERE crs_grp_usr.UsrCod=%ld"
			  " AND gam_grp.GrpCod=crs_grp_usr.GrpCod))",
			  MchCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
  }

/*****************************************************************************/
/******************* Get number of questions of a game *********************/
/*****************************************************************************/

static unsigned Gam_GetNumQstsGame (long GamCod)
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

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get other parameters *****/
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Show form to create a new question in this game *****/
   Tst_ShowFormAskSelectTstsForGame (Game.GamCod);

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/****************** Write parameter with index of question *******************/
/*****************************************************************************/

static void Gam_PutParamQstInd (unsigned QstInd)
  {
   Par_PutHiddenParamUnsigned ("QstInd",QstInd);
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

static unsigned Gam_GetParamQstInd (void)
  {
   long LongNum;

   LongNum = Par_GetParToLong ("QstInd");
   if (LongNum < 0)
      Lay_ShowErrorAndExit ("Wrong question index.");

   return (unsigned) LongNum;
  }

/*****************************************************************************/
/******************* Write parameter with student's answer *******************/
/*****************************************************************************/

static void Gam_PutParamAnswer (unsigned AnsInd)
  {
   Par_PutHiddenParamUnsigned ("Ans",AnsInd);
  }

/*****************************************************************************/
/******************* Get parameter with student's answer *********************/
/*****************************************************************************/

static unsigned Gam_GetParamAnswer (void)
  {
   long LongNum;

   LongNum = Par_GetParToLong ("Ans");
   if (LongNum < 0)
      Lay_ShowErrorAndExit ("Wrong answer index.");

   return (unsigned) LongNum;
  }

/*****************************************************************************/
/******************* Get parameter with index of question ********************/
/*****************************************************************************/

static unsigned Gam_GetQstIndFromStr (const char *UnsignedStr)
  {
   long LongNum;

   LongNum = Str_ConvertStrCodToLongCod (UnsignedStr);
   return (LongNum > 0) ? (unsigned) LongNum :
	                  0;
  }

/*****************************************************************************/
/****************** Get parameter with what is being shown *******************/
/*****************************************************************************/

static Gam_Showing_t Gam_GetShowingFromStr (const char *Str)
  {
   Gam_Showing_t Showing;

   for (Showing = (Gam_Showing_t) 0;
	Showing <= (Gam_Showing_t) (Gam_NUM_SHOWING - 1);
	Showing++)
      if (!strcmp (Str,Gam_ShowingStringsDB[Showing]))
         return Showing;

   return (Gam_Showing_t) Gam_SHOWING_DEFAULT;
  }

/*****************************************************************************/
/********************** Remove answers of a game question ********************/
/*****************************************************************************/

static void Gam_RemAnswersOfAQuestion (long GamCod,unsigned QstInd)
  {
   /***** Remove answers *****/
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM gam_answers"
		   " WHERE GamCod=%ld AND QstInd=%u",
		   GamCod,QstInd);
  }

/*****************************************************************************/
/************ Get question code given game and index of question *************/
/*****************************************************************************/

static long Gam_GetQstCodFromQstInd (long GamCod,unsigned QstInd)
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

static unsigned Gam_GetPrevQuestionIndexInGame (long GamCod,unsigned QstInd)
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

static unsigned Gam_GetNextQuestionIndexInGame (long GamCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NextQstInd = Gam_AFTER_LAST_QUESTION;	// End of questions has been reached

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
   extern const char *Txt_Done;
   MYSQL_RES *mysql_res;
   unsigned NumQsts;
   bool Editing = (Gbl.Action.Act == ActEdiOneGam    ||
	           Gbl.Action.Act == ActAddOneGamQst);
   Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions;

   /***** How to show the questions ******/
   if (Editing)
      ActionToDoWithQuestions = Tst_SHOW_GAME_RESULT;
   else
      ActionToDoWithQuestions = Tst_SHOW_GAME_TO_ANSWER;

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
   Gam_CurrentGamCod = Game->GamCod;
   Box_StartBox (NULL,Txt_Questions,Game->Status.ICanEdit ? Gam_PutIconToAddNewQuestions :
                                                            NULL,
                 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

   if (NumQsts)
     {
      /***** Show the table with the questions *****/
      Gam_ListOneOrMoreQuestionsForEdition (Game->GamCod,NumQsts,mysql_res);

      if (ActionToDoWithQuestions == Tst_SHOW_GAME_TO_ANSWER)
	{
	 /***** Button to create/modify game *****/
	 Btn_PutConfirmButton (Txt_Done);

	 /***** End form *****/
	 Frm_EndForm ();
	}
     }
   else	// This game has no questions
      Ale_ShowAlert (Ale_INFO,Txt_This_game_has_no_questions);

   if (Game->Status.ICanEdit &&	// I can edit
       (!NumQsts ||		// This game has no questions
	Editing))		// I am editing
      /***** Put button to add a new question in this game *****/
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
                                                  MYSQL_RES *mysql_res)
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
   long QstCod;
   char StrQstInd[10 + 1];

   /***** Get maximum question index *****/
   MaxQstInd = Gam_GetMaxQuestionIndexInGame (GamCod);

   /***** Write the heading *****/
   Tbl_StartTableWideMargin (2);
   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
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
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Code,
            Txt_Tags,
            Txt_Question);

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
      QstCod = Str_ConvertStrCodToLongCod (row[1]);

      /***** Icons *****/
      Gam_CurrentGamCod = GamCod;
      Gam_CurrentQstInd = QstInd;
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"BT%u\">",Gbl.RowEvenOdd);

      /* Put icon to remove the question */
      Frm_StartForm (ActReqRemGamQst);
      Gam_PutParamGameCod (GamCod);
      Gam_PutParamQstInd (QstInd);
      Ico_PutIconRemove ();
      Frm_EndForm ();

      /* Put icon to move up the question */
      if (QstInd > 1)
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
      if (QstInd < MaxQstInd)
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
      Gbl.Test.QstCod = QstCod;
      Ico_PutContextualIconToEdit (ActEdiOneTstQst,Tst_PutParamQstCod);

      fprintf (Gbl.F.Out,"</td>");

      /* Write number of question */
      fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP COLOR%u\">"
			 "<div class=\"BIG_INDEX\">%s</div>",
	       Gbl.RowEvenOdd,
	       StrQstInd);

      /* Write answer type (row[2]) */
      Gbl.Test.AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
      fprintf (Gbl.F.Out,"<div class=\"DAT_SMALL\">%s</div>"
			 "</td>",
	       Txt_TST_STR_ANSWER_TYPES[Gbl.Test.AnswerType]);

      /* Write question code */
      fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL CENTER_TOP COLOR%u\">"
	                 "%ld&nbsp;"
	                 "</td>",
               Gbl.RowEvenOdd,Gbl.Test.QstCod);

      /* Write the question tags */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
               Gbl.RowEvenOdd);
      Tst_GetAndWriteTagsQst (Gbl.Test.QstCod);
      fprintf (Gbl.F.Out,"</td>");

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
      Tst_WriteAnswersMatchResult (-1L,QstInd,QstCod,
                                   "TEST_EDI",false);	// Don't show result

      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Destroy test question *****/
      Tst_QstDestructor ();
     }

   /***** End table *****/
   Tbl_EndTable ();

   /***** Button to add a new question *****/
   Gam_PutButtonToAddNewQuestions ();

   /***** End box *****/
   Box_EndBox ();
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
   extern const char *Txt_You_must_select_one_ore_more_questions;
   struct Game Game;
   const char *Ptr;
   char LongStr[1 + 10 + 1];
   long QstCod;
   unsigned MaxQstInd;

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get selected questions *****/
   /* Allocate space for selected question codes */
   Gam_AllocateListSelectedQuestions ();

   /* Get question codes */
   Par_GetParMultiToText ("QstCods",Gbl.Games.ListQuestions,
                          Gam_MAX_BYTES_LIST_SELECTED_QUESTIONS);

   /* Check number of questions */
   if (Gam_CountNumQuestionsInList () == 0)	// If no questions selected...
     {						// ...write warning alert
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_questions);

      // TODO: Show form again!!!
     }

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

   /***** Free space for selected question codes *****/
   Gam_FreeListsSelectedQuestions ();

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
/*** Get number of users who selected this answer and draw proportional bar **/
/*****************************************************************************/

void Gam_GetAndDrawBarNumUsrsWhoAnswered (long MchCod,unsigned QstInd,unsigned AnsInd,unsigned NumUsrs)
  {
   unsigned NumUsrsThisAnswer;

   /***** Get number of users who selected this answer *****/
   NumUsrsThisAnswer = Gam_GetNumUsrsWhoAnswered (MchCod,QstInd,AnsInd);

   /***** Show stats of this answer *****/
   Gam_DrawBarNumUsrs (NumUsrsThisAnswer,NumUsrs);
  }

/*****************************************************************************/
/***** Get number of users who have answered current question in a match *****/
/*****************************************************************************/

static unsigned Gam_GetNumAnswerers (struct Match *Match)
  {
   /***** Get number of users who have answered the current question in a match from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of questions of a game",
			     "SELECT COUNT(*) FROM gam_answers"
			     " WHERE MchCod=%ld AND QstInd=%u",
			     Match->MchCod,Match->Status.QstInd);
  }

/*****************************************************************************/
/**** Get number of users who selected a given answer of a game question *****/
/*****************************************************************************/

static unsigned Gam_GetNumUsrsWhoAnswered (long MchCod,unsigned QstInd,unsigned AnsInd)
  {
   /***** Get number of users who have chosen
          an answer of a question from database *****/
   return (unsigned) DB_QueryCOUNT ("can not get number of users who answered",
				    "SELECT COUNT(*)"
				    " FROM gam_answers"
				    " WHERE MchCod=%ld"
				    " AND QstInd=%u"
				    " AND AnsInd=%u",
				    MchCod,QstInd,AnsInd);
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

#define Gam_MAX_BAR_WIDTH 125

static void Gam_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned BarWidth = 0;

   /***** String with the number of users *****/
   if (MaxUsrs)
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u&nbsp;(%u%%&nbsp;%s&nbsp;%u)",
                NumUsrs,
                (unsigned) ((((float) NumUsrs * 100.0) / (float) MaxUsrs) + 0.5),
                Txt_of_PART_OF_A_TOTAL,MaxUsrs);
   else
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "0&nbsp;(0%%&nbsp;%s&nbsp;%u)",
                Txt_of_PART_OF_A_TOTAL,MaxUsrs);

   /***** Draw bar with a with proportional to the number of clicks *****/
   if (NumUsrs && MaxUsrs)
      BarWidth = (unsigned) ((((float) NumUsrs * (float) Gam_MAX_BAR_WIDTH) /
	                       (float) MaxUsrs) + 0.5);
   if (BarWidth < 2)
      BarWidth = 2;
   fprintf (Gbl.F.Out,"<img src=\"%s/c1x16.gif\""
	              " alt=\"%s\" title=\"%s\""
                      " class=\"LEFT_TOP\""
	              " style=\"width:%upx; height:20px;\" />"
	              "&nbsp;",
      Cfg_URL_ICON_PUBLIC,
      Gbl.Title,
      Gbl.Title,
      BarWidth);

   /***** Write the number of users *****/
   fprintf (Gbl.F.Out,"%s",Gbl.Title);
  }

/*****************************************************************************/
/**************** Put parameter to move/remove one question ******************/
/*****************************************************************************/

static void Gam_PutParamsOneQst (void)
  {
   Gam_PutParamGameCod (Gam_CurrentGamCod);
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
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question index */
   QstInd = Gam_GetParamQstInd ();

   /***** Show question and button to remove question *****/
   Gam_CurrentGamCod = Game.GamCod;
   Gam_CurrentQstInd = QstInd;
   Ale_ShowAlertAndButton (ActRemGamQst,NULL,NULL,Gam_PutParamsOneQst,
			   Btn_REMOVE_BUTTON,Txt_Remove_question,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
	                   QstInd);

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
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question index */
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
		   "UPDATE gam_answers SET QstInd=QstInd-1"
		   " WHERE GamCod=%ld AND QstInd>%u",
                   Game.GamCod,QstInd);
   DB_QueryUPDATE ("can not update indexes of questions",
		   "UPDATE gam_questions SET QstInd=QstInd-1"
		   " WHERE GamCod=%ld AND QstInd>%u",
                   Game.GamCod,QstInd);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Question_removed);

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
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question index */
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
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question index */
   QstIndTop = Gam_GetParamQstInd ();

   /* Get maximum question index */
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

   /***** Lock table to make the inscription atomic *****/
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
/************************* List the matches of a game ************************/
/*****************************************************************************/

static void Gam_ListMatches (struct Game *Game,bool PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games_matches;
   extern const char *Txt_Matches;
   char *SubQuery;
   MYSQL_RES *mysql_res;
   unsigned NumMatches;

   /***** Get data of matches from database *****/
   /* Fill subquery for game */
   if (Gbl.Crs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
     {
      if (asprintf (&SubQuery," AND"
			      "(MchCod NOT IN"
			      " (SELECT MchCod FROM gam_grp)"
			      " OR"
			      " MchCod IN"
			      " (SELECT gam_grp.MchCod"
			      " FROM gam_grp,crs_grp_usr"
			      " WHERE crs_grp_usr.UsrCod=%ld"
			      " AND gam_grp.GrpCod=crs_grp_usr.GrpCod))",
		     Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	  Lay_NotEnoughMemoryExit ();
      }
    else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
       if (asprintf (&SubQuery,"%s","") < 0)
	  Lay_NotEnoughMemoryExit ();

   /* Make query */
   NumMatches = (unsigned) DB_QuerySELECT (&mysql_res,"can not get matches",
					   "SELECT MchCod,"				// row[0]
						  "GamCod,"				// row[1]
						  "UsrCod,"				// row[2]
						  "UNIX_TIMESTAMP(StartTime),"		// row[3]
						  "UNIX_TIMESTAMP(EndTime),"		// row[4]
						  "Title,"				// row[5]
						  "QstInd,"				// row[6]
						  "QstCod,"				// row[7]
						  "UNIX_TIMESTAMP(QstStartTime),"	// row[8]
						  "Showing"				// row[9]
					   " FROM gam_matches"
					   " WHERE GamCod=%ld%s"
					   " ORDER BY MchCod",
					   Game->GamCod,
					   SubQuery);

   /* Free allocated memory for subquery */
   free ((void *) SubQuery);

   /***** Start box *****/
   Gam_CurrentGamCod = Game->GamCod;
   Box_StartBox (NULL,Txt_Matches,Gam_PutIconToPlayNewMatch,
                 Hlp_ASSESSMENT_Games_matches,Box_NOT_CLOSABLE);

   if (NumMatches)
      /***** Show the table with the matches *****/
      Gam_ListOneOrMoreMatches (Game,NumMatches,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Put button to play a new match in this game *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 if (PutFormNewMatch)
	    Gam_PutFormNewMatch (Game);			// Form to fill in data and start playing a new match
	 else
	    Gam_PutButtonNewMatch (Game->GamCod);	// Button to create a new match
	 break;
      default:
	 break;
     }

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/********************** Get match data using its code ************************/
/*****************************************************************************/

void Gam_GetDataOfMatchByCod (struct Match *Match)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   /***** Get data of match from database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get matches",
					"SELECT MchCod,"				// row[0]
					       "GamCod,"				// row[1]
					       "UsrCod,"				// row[2]
					       "UNIX_TIMESTAMP(StartTime),"		// row[3]
					       "UNIX_TIMESTAMP(EndTime),"		// row[4]
					       "Title,"					// row[5]
					       "QstInd,"				// row[6]
					       "QstCod,"				// row[7]
					       "UNIX_TIMESTAMP(QstStartTime),"		// row[8]
					       "Showing"				// row[9]
					" FROM gam_matches"
					" WHERE MchCod=%ld"
					" AND GamCod IN"		// Extra check
					" (SELECT GamCod FROM games"
					" WHERE CrsCod='%ld')",
					Match->MchCod,
					Gbl.Hierarchy.Crs.CrsCod);
   if (NumRows) // Match found...
      /***** Get match data from row *****/
      Gam_GetMatchDataFromRow (mysql_res,Match);
   else
     {
      /* Initialize to empty match */
      Match->MchCod                  = -1L;
      Match->GamCod                  = -1L;
      Match->UsrCod                  = -1L;
      Match->TimeUTC[Gam_START_TIME] =
      Match->TimeUTC[Gam_END_TIME  ] = (time_t) 0;
      Match->Title[0]                = '\0';
      Match->Status.QstInd           = 0;
      Match->Status.QstCod           = -1L;
      Match->Status.QstStartTimeUTC  = (time_t) 0;
      Match->Status.Showing          = Gam_WORDING;
      Match->Status.BeingPlayed      = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to game *******************/
/*****************************************************************************/

static void Gam_PutIconToPlayNewMatch (void)
  {
   extern const char *Txt_New_match;

   /***** Put form to create a new question *****/
   Ico_PutContextualIconToAdd (ActReqNewMchTch,Gam_NEW_MATCH_SECTION_ID,Gam_PutParams,
			       Txt_New_match);
  }

/*****************************************************************************/
/*********************** List game matches for edition ***********************/
/*****************************************************************************/

static void Gam_ListOneOrMoreMatches (struct Game *Game,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Match;
   extern const char *Txt_Status;
   extern const char *Txt_Play;
   extern const char *Txt_Resume;
   extern const char *Txt_Today;
   unsigned NumMatch;
   unsigned UniqueId;
   struct Match Match;

   /***** Write the heading *****/
   Tbl_StartTableWideMargin (2);
   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"RIGHT_TOP\">"
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
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_ROLES_SINGUL_Abc[Rol_TCH][Usr_SEX_UNKNOWN],
	    Txt_START_END_TIME[Gam_ORDER_BY_START_DATE],
	    Txt_START_END_TIME[Gam_ORDER_BY_END_DATE],
            Txt_Match,
            Txt_Status);

   /***** Write rows *****/
   for (NumMatch = 0, UniqueId = 1;
	NumMatch < NumMatches;
	NumMatch++, UniqueId++)
     {
      Gbl.RowEvenOdd = NumMatch % 2;

      /***** Get match data from row *****/
      Gam_GetMatchDataFromRow (mysql_res,&Match);

      /***** Icons *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"BT%u\">",Gbl.RowEvenOdd);

      /* Put icon to remove the match */
      Frm_StartForm (ActReqRemMchTch);
      Gam_PutParamMatchCod (Match.MchCod);
      Ico_PutIconRemove ();
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</td>");

      /***** Number of match ******/
      fprintf (Gbl.F.Out,"<td class=\"BIG_INDEX RIGHT_TOP COLOR%u\">%u</td>",
	       Gbl.RowEvenOdd,NumMatch + 1);

      /***** Match player *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
	       Gbl.RowEvenOdd);
      Usr_WriteAuthor1Line (Match.UsrCod,false);
      fprintf (Gbl.F.Out,"</td>");

      /***** Start date/time *****/
      fprintf (Gbl.F.Out,"<td id=\"mch_date_start_%u\""
	                 " class=\"%s LEFT_TOP COLOR%u\">",
	       UniqueId,
	       Match.Status.QstInd >= Gam_AFTER_LAST_QUESTION ? "DATE_RED" :
		                                                "DATE_GREEN",
	       Gbl.RowEvenOdd);
      fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
			 "writeLocalDateHMSFromUTC('mch_date_start_%u',%ld,"
			 "%u,'<br />','%s',true,true,0x7);"
			 "</script>"
			 "</td>",
	       UniqueId,Match.TimeUTC[Gam_START_TIME],
	       (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

      /***** End date/time *****/
      fprintf (Gbl.F.Out,"<td id=\"mch_date_end_%u\""
	                 " class=\"%s LEFT_TOP COLOR%u\">",
	       UniqueId,
	       Match.Status.QstInd >= Gam_AFTER_LAST_QUESTION ? "DATE_RED" :
			                                        "DATE_GREEN",
	       Gbl.RowEvenOdd);
      fprintf (Gbl.F.Out,"\">"
			 "<script type=\"text/javascript\">"
			 "writeLocalDateHMSFromUTC('mch_date_end_%u',%ld,"
			 "%u,'<br />','%s',false,true,0x7);"
			 "</script>"
			 "</td>",
	       UniqueId,Match.TimeUTC[Gam_END_TIME],
	       (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

      /***** Title and groups *****/
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",Gbl.RowEvenOdd);

      /* Title */
      fprintf (Gbl.F.Out,"<span class=\"ASG_TITLE\">%s</span>",Match.Title);

      /* Groups whose students can answer this match */
      if (Gbl.Crs.Grps.NumGrps)
	 Gam_GetAndWriteNamesOfGrpsAssociatedToMatch (&Match);

      fprintf (Gbl.F.Out,"</td>");

      /***** Match status ******/
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">",Gbl.RowEvenOdd);

      if (Match.Status.QstInd < Gam_AFTER_LAST_QUESTION)	// Unfinished match
	 /* Current question index / total of questions */
	 fprintf (Gbl.F.Out,"<div class=\"DAT\">%u/%u</div>",
		  Match.Status.QstInd,Game->NumQsts);

      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:
	    /* Icon to play as student */
	    Gam_CurrentMchCod = Match.MchCod;
	    Lay_PutContextualLinkOnlyIcon (ActPlyMchStd,NULL,
					   Gam_PutParamCurrentMchCod,
					   Match.Status.QstInd < Gam_AFTER_LAST_QUESTION ? "play.svg" :
											   "flag-checkered.svg",
					   Txt_Play);
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_DEG_ADM:
	 case Rol_CTR_ADM:
	 case Rol_INS_ADM:
	 case Rol_SYS_ADM:
	    /* Icon to resume */
	    Gam_CurrentMchCod = Match.MchCod;
	    Lay_PutContextualLinkOnlyIcon (ActResMchTch,NULL,
					   Gam_PutParamCurrentMchCod,
					   Match.Status.QstInd < Gam_AFTER_LAST_QUESTION ? "play.svg" :
											   "flag-checkered.svg",
					   Txt_Resume);
	    break;
	 default:
	    break;
	}

      fprintf (Gbl.F.Out,"</td>");

      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** End table *****/
   Tbl_EndTable ();
  }

/*****************************************************************************/
/******************** Get game data from a database row **********************/
/*****************************************************************************/

static void Gam_GetMatchDataFromRow (MYSQL_RES *mysql_res,
				     struct Match *Match)
  {
   MYSQL_ROW row;

   /***** Get match data *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]	MchCod
   row[1]	GamCod
   row[2]	UsrCod
   row[3]	UNIX_TIMESTAMP(StartTime)
   row[4]	UNIX_TIMESTAMP(EndTime)
   row[5]	Title
   */
   /***** Get match data *****/
   /* Code of the match (row[0]) */
   if ((Match->MchCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of match.");

   /* Code of the game (row[1]) */
   if ((Match->GamCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of game.");

   /* Get match teacher (row[2]) */
   Match->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get start date (row[3] holds the start UTC time) */
   Match->TimeUTC[Gam_START_TIME] = Dat_GetUNIXTimeFromStr (row[3]);

   /* Get end   date (row[4] holds the end   UTC time) */
   Match->TimeUTC[Gam_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[4]);

   /* Get the title of the game (row[5]) */
   if (row[5])
      Str_Copy (Match->Title,row[5],
		Gam_MAX_BYTES_TITLE);
   else
      Match->Title[0] = '\0';

   /***** Get current match status *****/
   /*
   row[6]	QstInd
   row[7]	QstCod
   row[8]	UNIX_TIMESTAMP(QstStartTime)
   row[9]	Showing
   */
   /* Current question index (row[6]) */
   Match->Status.QstInd = Gam_GetQstIndFromStr (row[6]);

   /* Current question code (row[7]) */
   Match->Status.QstCod = Str_ConvertStrCodToLongCod (row[7]);

   /* Get question start date (row[8] holds the start UTC time) */
   Match->Status.QstStartTimeUTC = Dat_GetUNIXTimeFromStr (row[8]);

   /* Get what to show (stem, answers, results) (row(9)) */
   Match->Status.Showing = Gam_GetShowingFromStr (row[9]);

   /***** Get whether the match is being played or not *****/
   if (Match->Status.QstInd >= Gam_AFTER_LAST_QUESTION)	// Finished
      Match->Status.BeingPlayed = false;
   else							// Unfinished
      Match->Status.BeingPlayed = Gam_GetIfMatchIsBeingPlayed (Match->MchCod);
  }

/*****************************************************************************/
/************** Request the removal of a match (game instance) ***************/
/*****************************************************************************/

void Gam_RequestRemoveMatchTch (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_match_X;
   extern const char *Txt_Remove_match;
   struct Match Match;

   /***** Get parameters *****/
   /* Get match code */
   if ((Match.MchCod = Gam_GetParamMatchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");

   /***** Get data of the match from database *****/
   Gam_GetDataOfMatchByCod (&Match);

   /***** Show question and button to remove question *****/
   Gam_CurrentMchCod = Match.MchCod;
   Ale_ShowAlertAndButton (ActRemMchTch,NULL,NULL,Gam_PutParamCurrentMchCod,
			   Btn_REMOVE_BUTTON,Txt_Remove_match,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_match_X,
	                   Match.Title);

   /***** Show current game *****/
   Gam_ShowOneGame (Match.GamCod,
                    true,	// Show only this game
                    false,	// Do not list game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/********************** Remove a match (game instance) ***********************/
/*****************************************************************************/

void Gam_RemoveMatchTch (void)
  {
   extern const char *Txt_Match_X_removed;
   struct Match Match;

   /***** Get parameters *****/
   /* Get match code */
   if ((Match.MchCod = Gam_GetParamMatchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");

   /***** Get data of the match from database *****/
   Gam_GetDataOfMatchByCod (&Match);

   /***** Remove the match from all the tables *****/
   /* Remove match players */
   DB_QueryDELETE ("can not remove match players",
		   "DELETE FROM gam_players"
		   " USING gam_players,gam_matches,games"
		   " WHERE gam_players.MchCod=%ld"
		   " AND gam_players.MchCod=gam_matches.MchCod"
		   " AND gam_matches.GamCod=games.GamCod"
		   " AND games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove match from list of matches being played */
   DB_QueryDELETE ("can not remove match from matches being played",
		   "DELETE FROM gam_mch_being_played"
		   " USING gam_mch_being_played,gam_matches,games"
		   " WHERE gam_mch_being_played.MchCod=%ld"
		   " AND gam_mch_being_played.MchCod=gam_matches.MchCod"
		   " AND gam_matches.GamCod=games.GamCod"
		   " AND games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove students' answers to match */
   DB_QueryDELETE ("can not remove students' answers associated to a match",
		   "DELETE FROM gam_answers"
		   " USING gam_answers,gam_matches,games"
		   " WHERE gam_answers.MchCod=%ld"
		   " AND gam_answers.MchCod=gam_matches.MchCod"
		   " AND gam_matches.GamCod=games.GamCod"
		   " AND games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove groups associated to the match */
   DB_QueryDELETE ("can not remove the groups associated to a match",
		   "DELETE FROM gam_grp"
		   " USING gam_grp,gam_matches,games"
		   " WHERE gam_grp.MchCod=%ld"
		   " AND gam_grp.MchCod=gam_matches.MchCod"
		   " AND gam_matches.GamCod=games.GamCod"
		   " AND games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove the match itself */
   DB_QueryDELETE ("can not remove a match",
		   "DELETE FROM gam_matches"
		   " USING gam_matches,games"
		   " WHERE gam_matches.MchCod=%ld"
		   " AND gam_matches.GamCod=games.GamCod"
		   " AND games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The match to be removed does not exist.");

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Match_X_removed,
		  Match.Title);

   /***** Show current game *****/
   Gam_ShowOneGame (Match.GamCod,
                    true,	// Show only this game
                    false,	// Do not list game questions
		    false);	// Do not put form to start new match
  }

/*****************************************************************************/
/********************* Put button to create a new match **********************/
/*****************************************************************************/

static void Gam_PutButtonNewMatch (long GamCod)
  {
   extern const char *Txt_New_match;

   Frm_StartFormAnchor (ActReqNewMchTch,Gam_NEW_MATCH_SECTION_ID);
   Gam_PutParamGameCod (GamCod);
   Btn_PutConfirmButton (Txt_New_match);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Start playing a game as a teacher ***********************/
/*****************************************************************************/

void Gam_RequestNewMatchTch (void)
  {
   long GamCod;

   /***** Get parameters *****/
   Gam_GetParamOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   if ((GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Show game *****/
   Gam_ShowOneGame (GamCod,
                    true,	// Show only this game
                    false,	// Do not list game questions
                    true);	// Put form to start new match
  }

/*****************************************************************************/
/****** Put a big button to play match (start a new match) as a teacher ******/
/*****************************************************************************/

static void Gam_PutFormNewMatch (struct Game *Game)
  {
   extern const char *Hlp_ASSESSMENT_Games_new_match;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_match;
   extern const char *Txt_Title;
   extern const char *Txt_Play;

   /***** Start section for a new match *****/
   Lay_StartSection (Gam_NEW_MATCH_SECTION_ID);

   /***** Start form *****/
   Frm_StartForm (ActNewMchTch);
   Gam_PutParamGameCod (Game->GamCod);
   Gam_PutParamQstInd (0);	// Start by first question in game

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_match,NULL,
		      Hlp_ASSESSMENT_Games_new_match,Box_NOT_CLOSABLE,2);

   /***** Match title *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Title\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" id=\"Title\" name=\"Title\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Title,
            Gam_MAX_CHARS_TITLE,Game->Title);

   /***** Groups *****/
   Gam_ShowLstGrpsToCreateMatch ();

   /***** End table *****/
   Tbl_EndTable ();

   /***** Put icon with link *****/
   Frm_LinkFormSubmit (Txt_Play,NULL,NULL);
   fprintf (Gbl.F.Out,"<img src=\"%s/play.svg\""
		      " alt=\"%s\" title=\"%s\""
	              " class=\"CONTEXT_OPT ICO_HIGHLIGHT ICO64x64\" />",
            Cfg_URL_ICON_PUBLIC,Txt_Play,Txt_Play);
   fprintf (Gbl.F.Out,"</a>");

   /***** End box *****/
   Box_EndBox ();

   /***** End form *****/
   Frm_EndForm ();

   /***** End section for a new match *****/
   Lay_EndSection ();
  }

/*****************************************************************************/
/********************* Create a new match (by a teacher) *********************/
/*****************************************************************************/

void Gam_CreateNewMatchTch (void)
  {
   long GamCod;
   char Title[Gam_MAX_BYTES_TITLE + 1];

   /***** Get form parameters *****/
   /* Get match code */
   if ((GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get match title */
   Par_GetParToText ("Title",Title,Gam_MAX_BYTES_TITLE);

   /* Get groups for this games */
   Grp_GetParCodsSeveralGrps ();

   /***** Create a new match *****/
   Gbl.Games.MchCodBeingPlayed = Gam_CreateMatch (GamCod,Title);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/******* Show button to actually start / resume a match (by a teacher) *******/
/*****************************************************************************/

void Gam_RequestStartResumeMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Update match status in database *****/
   Gam_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Create a new match in a game *************************/
/*****************************************************************************/

static long Gam_CreateMatch (long GamCod,char Title[Gam_MAX_BYTES_TITLE + 1])
  {
   long MchCod;

   /***** Insert this new match into database *****/
   MchCod = DB_QueryINSERTandReturnCode ("can not create match",
				         "INSERT gam_matches"
				         " (GamCod,UsrCod,StartTime,EndTime,Title,ShowResults,"
				         "QstInd,QstCod,QstStartTime,Showing)"
				         " VALUES"
				         " (%ld,"	// GamCod
				         "%ld,"		// UsrCod
				         "NOW(),"	// StartTime
				         "NOW(),"	// EndTime
				         "'%s',"	// Title
				         "'N',"		// ShowResults: Don't show results initially
				         "0,"		// QstInd: Match has not started, so not the first question yet
				         "-1,"		// QstCod: Non-existent question
				         "NOW(),"	// QstStartTime
				         "'%s'",	// What is being shown
				         GamCod,
				         Gbl.Usrs.Me.UsrDat.UsrCod,	// Game creator
				         Title,
					 Gam_ShowingStringsDB[Gam_SHOWING_DEFAULT]);

   /***** Create groups associated to the match *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Gam_CreateGrps (MchCod);

   return MchCod;
  }

/*****************************************************************************/
/***************** Insert/update a game match being played *******************/
/*****************************************************************************/

static void Gam_UpdateMatchStatusInDB (struct Match *Match)
  {
   /***** Update match status in database *****/
   DB_QueryUPDATE ("can not update match being played",
		   "UPDATE gam_matches,games"
		   " SET gam_matches.EndTime=NOW(),"
			"gam_matches.QstInd=%u,"
			"gam_matches.QstCod=%ld,"
			"gam_matches.QstStartTime=NOW(),"
			"gam_matches.Showing='%s'"
		   " WHERE gam_matches.MchCod=%ld"
		   " AND gam_matches.GamCod=games.GamCod"
		   " AND games.CrsCod=%ld",	// Extra check
		   Match->Status.QstInd,Match->Status.QstCod,
		   Gam_ShowingStringsDB[Match->Status.Showing],
		   Match->MchCod,Gbl.Hierarchy.Crs.CrsCod);

   if (Match->Status.BeingPlayed)
      /* Update match as being played */
      Gam_UpdateMatchAsBeingPlayed (Match->MchCod);
   else
      /* Update match as not being played */
      Gam_SetMatchAsNotBeingPlayed (Match->MchCod);
  }

/*****************************************************************************/
/********** Update elapsed time in current question (by a teacher) ***********/
/*****************************************************************************/

static void Gam_UpdateElapsedTimeInQuestion (struct Match *Match)
  {
   /***** Update elapsed time in current question in database *****/
   if (Match->Status.BeingPlayed &&
       Match->Status.QstInd > 0 &&
       Match->Status.QstInd < Gam_AFTER_LAST_QUESTION)
      DB_QueryINSERT ("can not update elapsed time in question",
		      "INSERT INTO gam_time (MchCod,QstInd,ElapsedTime)"
		      " VALUES (%ld,%u,SEC_TO_TIME(%u))"
		      " ON DUPLICATE KEY"
		      " UPDATE ElapsedTime=ADDTIME(ElapsedTime,SEC_TO_TIME(%u))",
		      Match->MchCod,Match->Status.QstInd,
		      Cfg_SECONDS_TO_REFRESH_GAME,
		      Cfg_SECONDS_TO_REFRESH_GAME);
  }

/*****************************************************************************/
/******************* Get elapsed time in a match question ********************/
/*****************************************************************************/

static void Gam_GetElapsedTimeInQuestion (struct Match *Match,
					  struct Time *Time)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRows;

   /***** Query database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get elapsed time",
				        "SELECT ElapsedTime"
				        " FROM gam_time"
				        " WHERE MchCod=%ld AND QstInd=%u",
				        Match->MchCod,Match->Status.QstInd);

   /***** Get elapsed time from query result *****/
   Gam_GetElapsedTime (NumRows,mysql_res,Time);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get elapsed time in a match *************************/
/*****************************************************************************/

static void Gam_GetElapsedTimeInMatch (struct Match *Match,
				       struct Time *Time)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRows;

   /***** Query database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get elapsed time",
				        "SELECT SEC_TO_TIME(SUM(TIME_TO_SEC(ElapsedTime)))"
				        " FROM gam_time WHERE MchCod=%ld",
				        Match->MchCod);

   /***** Get elapsed time from query result *****/
   Gam_GetElapsedTime (NumRows,mysql_res,Time);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get elapsed time in a match *************************/
/*****************************************************************************/

static void Gam_GetElapsedTime (unsigned NumRows,MYSQL_RES *mysql_res,
				struct Time *Time)
  {
   MYSQL_ROW row;
   bool ElapsedTimeGotFromDB = false;

   /***** Get time from H...H:MM:SS string *****/
   if (NumRows)
     {
      row = mysql_fetch_row (mysql_res);

      if (row[0])
	 /* Get the elapsed time (row[0]) */
	 if (sscanf (row[0],"%u:%02u:%02u",&Time->Hour,&Time->Minute,&Time->Second) == 3)
	    ElapsedTimeGotFromDB = true;
     }

   /***** Initialize time to default value (0) *****/
   if (!ElapsedTimeGotFromDB)
      Time->Hour   =
      Time->Minute =
      Time->Second = 0;
  }

/*****************************************************************************/
/********************* Pause current match (by a teacher) ********************/
/*****************************************************************************/

void Gam_PauseMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Match.Status.BeingPlayed    = false;		// Resume match

   /***** Update match status in database *****/
   Gam_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/** Show current match status (current question, answers...) (by a teacher) **/
/*****************************************************************************/

void Gam_ResumeMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** If not yet finished, update status *****/
   if (Match.Status.QstInd < Gam_AFTER_LAST_QUESTION)	// Unfinished
     {
      if (Match.Status.QstInd == 0)			// Match has been created, but it has not started
	 Gam_SetMatchStatusToNext (&Match);
      Match.Status.BeingPlayed = true;			// Start/resume match
     }

   /***** Update match status in database *****/
   Gam_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/** Show stem, hiding answers, of current question in a match (by a teacher) */
/*****************************************************************************/
/*
void Gam_ShowStemQstMatchTch (void)
  {
   struct Match Match;

   ***** Remove old players.
          This function must be called before getting match status. *****
   Gam_RemoveOldPlayers ();

   ***** Get data of the match from database *****
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   ***** Update status *****
   Match.Status.Showing = Gam_WORDING;		// Show only the stem

   ***** Update match status in database *****
   Gam_UpdateMatchStatusInDB (&Match);

   ***** Show current match status *****
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }
*/
/*****************************************************************************/
/**** Show stem and answers of current question in a match (by a teacher) ****/
/*****************************************************************************/
/*
void Gam_ShowAnssQstMatchTch (void)
  {
   struct Match Match;

   ***** Remove old players.
          This function must be called before getting match status. *****
   Gam_RemoveOldPlayers ();

   ***** Get data of the match from database *****
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   ***** Update status *****
   Match.Status.Showing = Gam_ANSWERS;		// Show answers

   ***** Update match status in database *****
   Gam_UpdateMatchStatusInDB (&Match);

   ***** Show current match status *****
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }
*/
/*****************************************************************************/
/******** Show results of current question in a match (by a teacher) *********/
/*****************************************************************************/

void Gam_ShowResultsQstMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Match.Status.Showing = Gam_RESULTS;		// Show results

   /***** Update match status in database *****/
   Gam_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Show previous question in a match (by a teacher) **************/
/*****************************************************************************/

void Gam_BackMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Gam_SetMatchStatusToPrev (&Match);

   /***** Update match status in database *****/
   Gam_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*************** Show next question in a match (by a teacher) ****************/
/*****************************************************************************/

void Gam_ForwardMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Gam_SetMatchStatusToNext (&Match);

   /***** Update match status in database *****/
   Gam_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************** Show current question in a match (by a teacher) **************/
/*****************************************************************************/
/*
void Gam_CurrQstMatchTch (void)
  {
   struct Match Match;

   ***** Remove old players.
          This function must be called before getting match status. *****
   Gam_RemoveOldPlayers ();

   ***** Get data of the match from database *****
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   ***** Toggle display of results *****
   Match.Status.ShowResults = !Match.Status.ShowResults;

   ***** Update match status in database *****
   Gam_UpdateMatchStatusInDB (&Match);

   ***** Show current match status *****
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }
*/
/*****************************************************************************/
/************** Set match status to previous (backward) status ***************/
/*****************************************************************************/

static void Gam_SetMatchStatusToPrev (struct Match *Match)
  {
   /***** What to show *****/
   switch (Match->Status.Showing)
     {
      case Gam_WORDING:
	 Match->Status.Showing = Gam_REQUEST;

	 /***** Get index of the previous question *****/
	 Match->Status.QstInd = Gam_GetPrevQuestionIndexInGame (Match->GamCod,
								Match->Status.QstInd);
	 if (Match->Status.QstInd == 0)		// Start of questions has been reached
	   {
	    Match->Status.QstCod = -1L;		// No previous questions
	    Match->Status.BeingPlayed = false;	// Match is not being played
	   }
	 else
	    Match->Status.QstCod = Gam_GetQstCodFromQstInd (Match->GamCod,
							    Match->Status.QstInd);
	 break;
      case Gam_ANSWERS:
	 Match->Status.Showing = Gam_WORDING;
	 break;
      case Gam_REQUEST:
	 Match->Status.Showing = Gam_ANSWERS;
	 break;
      case Gam_RESULTS:
	 Match->Status.Showing = Gam_REQUEST;
	 break;
     }
  }

/*****************************************************************************/
/**************** Set match status to next (forward) status ******************/
/*****************************************************************************/

static void Gam_SetMatchStatusToNext (struct Match *Match)
  {
   /***** What to show *****/
   switch (Match->Status.Showing)
     {
      case Gam_WORDING:
	 Match->Status.Showing = Gam_ANSWERS;
	 break;
      case Gam_ANSWERS:
	 Match->Status.Showing = Gam_REQUEST;
	 break;
      case Gam_REQUEST:
      case Gam_RESULTS:
	 Match->Status.Showing = Gam_WORDING;

	 /***** Get index of the next question *****/
	 Match->Status.QstInd = Gam_GetNextQuestionIndexInGame (Match->GamCod,
								Match->Status.QstInd);
	 if (Match->Status.QstInd < Gam_AFTER_LAST_QUESTION)	// Unfinished
	    Match->Status.QstCod = Gam_GetQstCodFromQstInd (Match->GamCod,
							    Match->Status.QstInd);
	 else							// Finished
	   {
	    Match->Status.QstCod = -1L;				// No more questions
	    Match->Status.BeingPlayed = false;
	   }
	 break;
     }
  }

/*****************************************************************************/
/******* Show current match status (number, question, answers, button) *******/
/*****************************************************************************/

static void Gam_ShowMatchStatusForTch (struct Match *Match)
  {
   /***** Get current number of players *****/
   Gam_GetNumPlayers (Match);

   /***** Left column *****/
   Gam_ShowLeftColumnTch (Match);

   /***** Right column *****/
   /* Start right container */
   fprintf (Gbl.F.Out,"<div class=\"MATCH_RIGHT\">");

   /* Top row: match title */
   Gam_ShowMatchTitle (Match);

   /* Bottom row: current question and possible answers */
   Gam_ShowQuestionAndAnswersTch (Match);

   /* End right container */
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************ Show current question being played for a student ***************/
/*****************************************************************************/

static void Gam_ShowMatchStatusForStd (struct Match *Match)
  {
   extern const char *Txt_Please_wait_;
   bool IBelongToGroups;

   /***** Do I belong to valid groups to play this match? *****/
   IBelongToGroups = Gbl.Usrs.Me.IBelongToCurrentCrs &&
		     Gam_CheckIfIPlayThisMatchBasedOnGrps (Match->MchCod);
   if (!IBelongToGroups)
      Lay_ShowErrorAndExit ("You can not play this match!");

   /***** Get current number of players *****/
   Gam_GetNumPlayers (Match);

   /***** Left column *****/
   Gam_ShowLeftColumnStd (Match);

   /***** Right column *****/
   /* Start right container */
   fprintf (Gbl.F.Out,"<div class=\"MATCH_RIGHT\">");

   /***** Top row *****/
   Gam_ShowMatchTitle (Match);

   /***** Bottom row *****/
   if (Match->Status.QstInd < Gam_AFTER_LAST_QUESTION)	// Unfinished
     {
      fprintf (Gbl.F.Out,"<div class=\"MATCH_BOTTOM\">");

      /***** Update players ******/
      Gam_RegisterMeAsPlayerInMatch (Match->MchCod);

      if (Match->Status.BeingPlayed)
	 /* Show current question and possible answers */
	 Gam_ShowQuestionAndAnswersStd (Match);
      else	// Not being played
	 Gam_ShowWaitImage (Txt_Please_wait_);

      fprintf (Gbl.F.Out,"</div>");
     }

   /* End right container */
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******** Show left botton column when playing a match (as a teacher) ********/
/*****************************************************************************/

static void Gam_ShowLeftColumnTch (struct Match *Match)
  {
   extern const char *Txt_MATCH_respond;
   struct Time Time;
   unsigned NumAnswerers;

   /***** Start left container *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_LEFT\">");

   /***** Top *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TOP\">");

   /* Write elapsed time in match */
   Gam_GetElapsedTimeInMatch (Match,&Time);
   Dat_WriteHoursMinutesSeconds (&Time);

   fprintf (Gbl.F.Out,"</div>");

   /***** Write number of question *****/
   Gam_ShowNumQstInGame (Match);

   /***** Write elapsed time in question *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TIME_QST\">");
   if (Match->Status.QstInd > 0 &&
       Match->Status.QstInd < Gam_AFTER_LAST_QUESTION)
     {
      Gam_GetElapsedTimeInQuestion (Match,&Time);
      Dat_WriteHoursMinutesSeconds (&Time);
     }
   else
      fprintf (Gbl.F.Out,"-");
   fprintf (Gbl.F.Out,"</div>");

   /***** Buttons *****/
   Gam_PutMatchControlButtons (Match);

   /***** Checkbox to show results *****/
   // Gam_PutCheckboxResult (Match);

   /***** Number of players *****/
   Gam_ShowNumPlayers (Match);

   /***** Number of users who have answered *****/
   if (Match->Status.BeingPlayed)
     {
      NumAnswerers = Gam_GetNumAnswerers (Match);
      fprintf (Gbl.F.Out,"<div class=\"MATCH_NUM_ANSWERERS\">"
                         "%s<br />"
                         "<strong>%u/%u</strong>"
	                 "</div>",
	       Txt_MATCH_respond,
	       NumAnswerers,Match->Status.NumPlayers);
     }

   /***** End left container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******** Show left botton column when playing a match (as a student) ********/
/*****************************************************************************/

static void Gam_ShowLeftColumnStd (struct Match *Match)
  {
   /***** Start left container *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_LEFT\">");

   /***** Top *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TOP\"></div>");

   /***** Write number of question *****/
   Gam_ShowNumQstInGame (Match);

   /***** End left container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Put buttons to control a match ***********************/
/*****************************************************************************/

static void Gam_PutMatchControlButtons (struct Match *Match)
  {
   extern const char *Txt_Go_back;
   extern const char *Txt_Go_forward;
   extern const char *Txt_Pause;
   extern const char *Txt_Start;
   extern const char *Txt_Resume;

   /***** Start buttons container *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTONS_CONTAINER\">");

   /***** Left button *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_LEFT_CONTAINER\">");
   if (Match->Status.QstInd == 0)
      /* Put button to close browser tab */
      Gam_PutBigButtonClose ();
   else
      /* Put button to go back */
      Gam_PutBigButton (ActBckMchTch,Match->MchCod,
			Gam_ICON_PREVIOUS,Txt_Go_back);
   fprintf (Gbl.F.Out,"</div>");

   /***** Center button *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_CENTER_CONTAINER\">");
   if (Match->Status.BeingPlayed)				// Being played
      /* Put button to pause match */
      Gam_PutBigButton (ActPauMchTch,
			Match->MchCod,
			Gam_ICON_PAUSE,Txt_Pause);
   else								// Paused
     {
      if (Match->Status.QstInd < Gam_AFTER_LAST_QUESTION)	// Not finished
	 /* Put button to play match */
	 Gam_PutBigButton (ActPlyMchTch,
			   Match->MchCod,
			   Gam_ICON_PLAY,Match->Status.QstInd == 0 ? Txt_Start :
								     Txt_Resume);
      else							// Finished
	 /* Put disabled button to play match */
	 Gam_PutBigButtonOff (Gam_ICON_PLAY);
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Right button *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_RIGHT_CONTAINER\">");
   if (Match->Status.QstInd >= Gam_AFTER_LAST_QUESTION)	// Finished
      /* Put button to close browser tab */
      Gam_PutBigButtonClose ();
   else
      /* Put button to show answers */
      Gam_PutBigButton (ActFwdMchTch,Match->MchCod,
			Gam_ICON_NEXT,Txt_Go_forward);
   fprintf (Gbl.F.Out,"</div>");

   /***** End buttons container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************* Show number of question in game ***********************/
/*****************************************************************************/

static void Gam_ShowNumQstInGame (struct Match *Match)
  {
   extern const char *Txt_MATCH_Start;
   extern const char *Txt_MATCH_End;
   unsigned NumQsts = Gam_GetNumQstsGame (Match->GamCod);

   fprintf (Gbl.F.Out,"<div class=\"MATCH_NUM_QST\">");
   if (Match->Status.QstInd == 0)				// Not started
      fprintf (Gbl.F.Out,"%s",Txt_MATCH_Start);
   else if (Match->Status.QstInd >= Gam_AFTER_LAST_QUESTION)	// Finished
      fprintf (Gbl.F.Out,"%s",Txt_MATCH_End);
   else
      fprintf (Gbl.F.Out,"%u/%u",Match->Status.QstInd,NumQsts);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************** Show number of players ***************************/
/*****************************************************************************/

static void Gam_ShowNumPlayers (struct Match *Match)
  {
   extern const char *Txt_Players;

   fprintf (Gbl.F.Out,"<div class=\"MATCH_NUM_PLAYERS\">"
	              "%s<br />"
                      "<strong>%u</strong>"
	              "</div>",
	    Txt_Players,Match->Status.NumPlayers);
  }

/*****************************************************************************/
/***************************** Show match title ******************************/
/*****************************************************************************/

static void Gam_ShowMatchTitle (struct Match *Match)
  {
   /***** Match title *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TOP\">%s</div>",Match->Title);
  }

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a teacher) *****/
/*****************************************************************************/

static void Gam_ShowQuestionAndAnswersTch (struct Match *Match)
  {
   extern const char *Txt_MATCH_Paused;
   extern const char *Txt_View_results;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Trivial check: question index should be correct *****/
   if (Match->Status.QstInd == 0 ||
       Match->Status.QstInd >= Gam_AFTER_LAST_QUESTION)
      return;

   /***** Get data of question from database *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get data of a question",
			"SELECT AnsType,"	// row[0]
			       "Stem,"		// row[1]
			       "MedCod"		// row[2]
			" FROM tst_questions"
			" WHERE QstCod=%ld",
			Match->Status.QstCod))
      Ale_ShowAlert (Ale_ERROR,"Question doesn't exist.");
   row = mysql_fetch_row (mysql_res);

   /***** Show question *****/
   /* Get answer type (row[0]) */
   Gbl.Test.AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[0]);
   // TODO: Check that answer type is correct (unique choice)

   fprintf (Gbl.F.Out,"<div class=\"MATCH_BOTTOM\">");

   /* Write stem (row[1]) */
   Tst_WriteQstStem (row[1],"MATCH_TCH_QST");

   /* Get media (row[2]) */
   Gbl.Test.Media.MedCod = Str_ConvertStrCodToLongCod (row[2]);
   Med_GetMediaDataByCod (&Gbl.Test.Media);

   /* Show media */
   Med_ShowMedia (&Gbl.Test.Media,
		  "TEST_MED_EDIT_LIST_STEM_CONTAINER",
		  "TEST_MED_EDIT_LIST_STEM");

   /* Write answers? */
   switch (Match->Status.Showing)
     {
      case Gam_WORDING:
	 /* Don't write anything */
	 break;
      case Gam_ANSWERS:
	 if (Match->Status.BeingPlayed)
	    /* Write answers */
	    Tst_WriteAnswersMatchResult (Match->MchCod,
					 Match->Status.QstInd,
					 Match->Status.QstCod,
					 "MATCH_TCH_QST",false);	// Don't show result
	 else	// Not being played
	    Gam_ShowWaitImage (Txt_MATCH_Paused);
	 break;
      case Gam_REQUEST:
	 /* Write button to request viewing results */
         Gam_PutBigButton (ActShoResMchTch,Match->MchCod,
			   Gam_ICON_RESULTS,Txt_View_results);
	 break;
      case Gam_RESULTS:
	 /* Write answers with results */
	 Tst_WriteAnswersMatchResult (Match->MchCod,
				      Match->Status.QstInd,
				      Match->Status.QstCod,
				      "MATCH_TCH_QST",true);	// Show result
	 break;
     }

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************ Put button to show results *************************/
/*****************************************************************************/
/*
static void Gam_PutCheckboxResult (struct Match *Match)
  {
   extern const char *Txt_View_results;

   ***** Start container *****
   fprintf (Gbl.F.Out,"<div class=\"MATCH_SHOW_RESULTS\">");

   ***** Start form *****
   Frm_StartForm (ActChgShoResMchTch);
   Gam_PutParamMatchCod (Match->MchCod);	// Current match being played

   ***** Put icon with link *****
   * Submitting onmousedown instead of default onclick
      is necessary in order to be fast
      and not lose clicks due to refresh *
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_OPT\">"
	              "<a href=\"\" class=\"ICO_HIGHLIGHT\""
	              " title=\"%s\" "
	              " onmousedown=\"document.getElementById('%s').submit();"
	              " return false;\">"
	              "<i class=\"%s\"></i>"
	              "&nbsp;%s"
	              "</a>"
	              "</div>",
	    Txt_View_results,
	    Gbl.Form.Id,
	    Match->Status.ShowResults ? "fas fa-toggle-on" :
		                        "fas fa-toggle-off",
	    Txt_View_results);

   ***** End form *****
   Frm_EndForm ();

   ***** End container *****
   fprintf (Gbl.F.Out,"</div>");
  }
*/

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a student) *****/
/*****************************************************************************/

static void Gam_ShowQuestionAndAnswersStd (struct Match *Match)
  {
   bool Shuffle = false;	// TODO: Read shuffle from question
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   int StdAnsInd;
   unsigned NumOptions;
   unsigned NumOpt;
   unsigned Index;
   bool ErrorInIndex = false;

   /***** Show question *****/
   /* Write buttons for answers? */
   if (Match->Status.Showing == Gam_ANSWERS)
     {
      if (Tst_CheckIfQuestionIsValidForGame (Match->Status.QstCod))
	{
	 /***** Get student's answer to this question
		(<0 ==> no answer) *****/
	 StdAnsInd = Gam_GetQstAnsFromDB (Match->MchCod,
					  Match->Status.QstInd);

	 /***** Get number of options in this question *****/
	 NumOptions = Tst_GetNumAnswersQst (Match->Status.QstCod);

	 /***** Get answers of question from database *****/
	 Shuffle = false;
	 NumOptions = Tst_GetAnswersQst (Match->Status.QstCod,&mysql_res,Shuffle);
	 /*
	 row[0] AnsInd
	 row[1] Answer
	 row[2] Feedback
	 row[3] MedCod
	 row[4] Correct
	 */

	 /***** Start table *****/
	 Tbl_StartTableWide (8);

	 for (NumOpt = 0;
	      NumOpt < NumOptions;
	      NumOpt++)
	   {
	    /***** Get next answer *****/
	    row = mysql_fetch_row (mysql_res);

	    /***** Assign index (row[0]).
		   Index is 0,1,2,3... if no shuffle
		   or 1,3,0,2... (example) if shuffle *****/
	    if (sscanf (row[0],"%u",&Index) == 1)
	      {
	       if (Index >= Tst_MAX_OPTIONS_PER_QUESTION)
		  ErrorInIndex = true;
	      }
	    else
	       ErrorInIndex = true;
	    if (ErrorInIndex)
	       Lay_ShowErrorAndExit ("Wrong index of answer when showing a test.");

	    /***** Start row *****/
	    // if (NumOpt % 2 == 0)
	    fprintf (Gbl.F.Out,"<tr>");

	    /***** Write letter for this option *****/
	    /* Start table cell */
	    fprintf (Gbl.F.Out,"<td class=\"MATCH_STD_CELL\">");

	    /* Form with button.
	       Sumitting onmousedown instead of default onclick
	       is necessary in order to be fast
	       and not lose clicks due to refresh */
	    Frm_StartForm (ActAnsMchQstStd);
	    Gam_PutParamMatchCod (Match->MchCod);	// Current match being played
	    Gam_PutParamQstInd (Match->Status.QstInd);	// Current question index shown
	    Gam_PutParamAnswer (Index);			// Index for this option
	    fprintf (Gbl.F.Out,"<button type=\"submit\""
			       " onmousedown=\"document.getElementById('%s').submit();"
			       "return false;\" class=\"",
		     Gbl.Form.Id);
	    if (StdAnsInd == (int) NumOpt)	// Student's answer
	       fprintf (Gbl.F.Out,"MATCH_STD_ANSWER_SELECTED ");
	    fprintf (Gbl.F.Out,"MATCH_STD_BUTTON BT_%c\">"
			       "%c"
			       "</button>",
		     'A' + (char) NumOpt,
		     'a' + (char) NumOpt);
	    Frm_EndForm ();

	    /* End table cell */
	    fprintf (Gbl.F.Out,"</td>");

	    /***** End row *****/
	    // if (NumOpt % 2 == 1)
	    fprintf (Gbl.F.Out,"</tr>");
	   }

	 /***** End table *****/
	 Tbl_EndTable ();
	}
      else
	 Ale_ShowAlert (Ale_ERROR,"Type of answer not valid in a game.");
     }
  }

/*****************************************************************************/
/*********************** Put a big button to do action ***********************/
/*****************************************************************************/

static void Gam_PutBigButton (Act_Action_t NextAction,long MchCod,
			      const char *Icon,const char *Txt)
  {
   /***** Start form *****/
   Frm_StartForm (NextAction);
   Gam_PutParamMatchCod (MchCod);

   /***** Put icon with link *****/
   /* Submitting onmousedown instead of default onclick
      is necessary in order to be fast
      and not lose clicks due to refresh */
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_CONTAINER\">"
                      "<a href=\"\" class=\"MATCH_BUTTON_ON\" title=\"%s\" "
	              " onmousedown=\"document.getElementById('%s').submit();"
	              " return false;\">"
	              "<i class=\"%s\"></i>"
	              "</a>"
	              "</div>",
	    Txt,
	    Gbl.Form.Id,
	    Icon);

   /***** End form *****/
   Frm_EndForm ();
  }

static void Gam_PutBigButtonOff (const char *Icon)
  {
   /***** Put inactive icon *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_CONTAINER\">"
                      "<div class=\"MATCH_BUTTON_OFF\">"
	              "<i class=\"%s\"></i>"
	              "</div>"
	              "</div>",
	    Icon);
  }

static void Gam_PutBigButtonClose (void)
  {
   extern const char *Txt_Close;

   /***** Put icon with link *****/
   /* onmousedown instead of default onclick
      is necessary in order to be fast
      and not lose clicks due to refresh */
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_CONTAINER\">"
                      "<a href=\"\" class=\"MATCH_BUTTON_ON\" title=\"%s\" "
	              " onmousedown=\"window.close();"
	              " return false;\"\">"
	              "<i class=\"%s\"></i>"
	              "</a>"
	              "</div>",
	    Txt_Close,Gam_ICON_CLOSE);
  }

/*****************************************************************************/
/****************************** Show wait image ******************************/
/*****************************************************************************/

static void Gam_ShowWaitImage (const char *Txt)
  {
   fprintf (Gbl.F.Out,"<div class=\"MATCH_WAIT_CONTAINER\">"
		      "<img src=\"%s/wait.gif\""
		      " alt=\"%s\" title=\"%s\""
		      " class=\"MATCH_WAIT_IMAGE\" />"
		      "</div>",
	    Cfg_URL_ICON_PUBLIC,
	    Txt,
	    Txt);
  }

/*****************************************************************************/
/**************************** Remove old players *****************************/
/*****************************************************************************/

static void Gam_RemoveOldPlayers (void)
  {
   /***** Delete matches not being played *****/
   DB_QueryDELETE ("can not update matches as not being played",
		   "DELETE FROM gam_mch_being_played"
		   " WHERE TS<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Cfg_SECONDS_TO_REFRESH_GAME*3);

   /***** Delete players who have left matches *****/
   DB_QueryDELETE ("can not update match players",
		   "DELETE FROM gam_players"
		   " WHERE TS<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Cfg_SECONDS_TO_REFRESH_GAME*3);
  }

static void Gam_UpdateMatchAsBeingPlayed (long MchCod)
  {
   /***** Insert match as being played *****/
   DB_QueryREPLACE ("can not set match as being played",
		    "REPLACE gam_mch_being_played (MchCod) VALUE (%ld)",
		    MchCod);
  }

static void Gam_SetMatchAsNotBeingPlayed (long MchCod)
  {
   /***** Delete all match players ******/
   DB_QueryDELETE ("can not update match players",
		    "DELETE FROM gam_players"
		    " WHERE MchCod=%ld",
		    MchCod);

   /***** Delete match as being played ******/
   DB_QueryDELETE ("can not set match as not being played",
		    "DELETE FROM gam_mch_being_played"
		    " WHERE MchCod=%ld",
		    MchCod);
  }

static bool Gam_GetIfMatchIsBeingPlayed (long MchCod)
  {
   /***** Get if a match is being played or not *****/
   return
   (bool) (DB_QueryCOUNT ("can not get if match is being played",
			  "SELECT COUNT(*) FROM gam_mch_being_played"
			  " WHERE MchCod=%ld",
			  MchCod) != 0);
  }

static void Gam_RegisterMeAsPlayerInMatch (long MchCod)
  {
   /***** Insert me as match player *****/
   DB_QueryREPLACE ("can not insert match player",
		    "REPLACE gam_players (MchCod,UsrCod) VALUES (%ld,%ld)",
		    MchCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

static void Gam_GetNumPlayers (struct Match *Match)
  {
   /***** Get number of players who are playing a match *****/
   Match->Status.NumPlayers = (unsigned) DB_QueryCOUNT ("can not get number of players",
							"SELECT COUNT(*) FROM gam_players"
							" WHERE MchCod=%ld",
							Match->MchCod);
  }

/*****************************************************************************/
/******************* Show the results of a finished match ********************/
/*****************************************************************************/

void Gam_ShowFinishedMatchResults (void)
  {
   Ale_ShowAlert (Ale_INFO,"To be implemented...");
  }

/*****************************************************************************/
/********************** Get code of match being played ***********************/
/*****************************************************************************/

void Gam_GetMatchBeingPlayed (void)
  {
   /***** Get match code ****/
   if ((Gbl.Games.MchCodBeingPlayed = Gam_GetParamMatchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");
  }

/*****************************************************************************/
/********* Show game being played to me as student in a new window ***********/
/*****************************************************************************/

void Gam_ShowMatchToMeAsStd (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForStd (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Refresh match for a teacher via AJAX *********************/
/*****************************************************************************/

void Gam_RefreshMatchTch (void)
  {
   struct Match Match;

   if (!Gbl.Session.IsOpen)	// If session has been closed, do not write anything
      return;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Update match status in database *****/
   Gam_UpdateMatchStatusInDB (&Match);

   /***** Update elapsed time in this question *****/
   Gam_UpdateElapsedTimeInQuestion (&Match);

   /***** Show current match status *****/
   Gam_ShowMatchStatusForTch (&Match);
  }

/*****************************************************************************/
/*************** Refresh current game for a student via AJAX *****************/
/*****************************************************************************/

void Gam_RefreshMatchStd (void)
  {
   struct Match Match;

   if (!Gbl.Session.IsOpen)	// If session has been closed, do not write anything
      return;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Show current match status *****/
   Gam_ShowMatchStatusForStd (&Match);
  }

/*****************************************************************************/
/**** Receive previous question answer in a match question from database *****/
/*****************************************************************************/

static int Gam_GetQstAnsFromDB (long MchCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   int StdAnsInd = -1;	// <0 ==> no answer selected

   /***** Get student's answer *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get student's answer to a match question",
					"SELECT AnsInd FROM gam_answers"
					" WHERE MchCod=%ld AND UsrCod=%ld AND QstInd=%u",
					MchCod,
					Gbl.Usrs.Me.UsrDat.UsrCod,
					QstInd);
   if (NumRows) // Answer found...
     {
      /***** Get answer index *****/
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%d",&StdAnsInd) != 1)
	 Lay_ShowErrorAndExit ("Error when getting student's answer to a match question.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return StdAnsInd;
  }

/*****************************************************************************/
/********* Receive question answer from student when playing a match *********/
/*****************************************************************************/

void Gam_ReceiveQstAnsFromStd (void)
  {
   struct Match Match;
   unsigned QstInd;
   unsigned StdAnsInd;
   int PreviousStdAnsInd;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Gam_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Gam_GetDataOfMatchByCod (&Match);

   /***** Get question index from form *****/
   QstInd = Gam_GetParamQstInd ();

   /***** Check that question index is the current one being played *****/
   if (QstInd == Match.Status.QstInd)	// Receiving an answer
					// to the current question being played
     {
      /***** Get answer index *****/
      /*-------+--------------+
      | Button | Answer index |
      +--------+--------------+
      |   a    |       0      |
      |   b    |       1      |
      |   c    |       2      |
      |   d    |       3      |
      |  ...   |      ...     |
      +--------+-------------*/
      StdAnsInd = Gam_GetParamAnswer ();

      /***** Get previous student's answer to this question
	     (<0 ==> no answer) *****/
      PreviousStdAnsInd = Gam_GetQstAnsFromDB (Match.MchCod,QstInd);

      /***** Store student's answer *****/
      if (PreviousStdAnsInd == (int) StdAnsInd)
	 DB_QueryDELETE ("can not register your answer to the match question",
			  "DELETE FROM gam_answers"
			  " WHERE MchCod=%ld AND UsrCod=%ld AND QstInd=%u",
			  Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,QstInd);
      else
	 DB_QueryREPLACE ("can not register your answer to the match question",
			  "REPLACE gam_answers"
			  " (MchCod,UsrCod,QstInd,AnsInd)"
			  " VALUES"
			  " (%ld,%ld,%u,%u)",
			  Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,QstInd,StdAnsInd);
     }

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Gam_ShowMatchStatusForStd (&Match);
   fprintf (Gbl.F.Out,"</div>");
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
			 "SELECT COUNT(DISTINCT Cod)"
			 " FROM games"
			 " WHERE Scope='%s'",
                         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT games.Cod)"
			 " FROM institutions,centres,degrees,courses,games"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
                         Gbl.Hierarchy.Ins.InsCod,
                         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT games.Cod)"
			 " FROM centres,degrees,courses,games"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
		         Gbl.Hierarchy.Ins.InsCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 " FROM degrees,courses,games"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
                         Gbl.Hierarchy.Ctr.CtrCod,
                         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT games.Cod)"
			 " FROM courses,games"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
		         Gbl.Hierarchy.Deg.DegCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with games",
			 "SELECT COUNT(DISTINCT Cod)"
			 " FROM games"
			 " WHERE Scope='%s' AND Cod=%ld",
                         Sco_GetDBStrFromScope (Hie_CRS),
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
			 " FROM games"
			 " WHERE Scope='%s'",
                         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM institutions,centres,degrees,courses,games"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
		         Gbl.Hierarchy.Cty.CtyCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM centres,degrees,courses,games"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
		         Gbl.Hierarchy.Ins.InsCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM degrees,courses,games"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
		         Gbl.Hierarchy.Ctr.CtrCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM courses,games"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'",
		         Gbl.Hierarchy.Deg.DegCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of games",
                         "SELECT COUNT(*)"
			 " FROM games"
			 " WHERE games.Scope='%s'"
			 " AND CrsCod=%ld",
                         Sco_GetDBStrFromScope (Hie_CRS),
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
			 " FROM games,gam_questions"
			 " WHERE games.Scope='%s'"
			 " AND games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM institutions,centres,degrees,courses,games,gam_questions"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'"
			 " AND games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                         Gbl.Hierarchy.Cty.CtyCod,
                         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM centres,degrees,courses,games,gam_questions"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'"
			 " AND games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
		         Gbl.Hierarchy.Ins.InsCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM degrees,courses,games,gam_questions"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'"
			 " AND games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                         Gbl.Hierarchy.Ctr.CtrCod,
                         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM courses,games,gam_questions"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=games.Cod"
			 " AND games.Scope='%s'"
			 " AND games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
		         Gbl.Hierarchy.Deg.DegCod,
		         Sco_GetDBStrFromScope (Hie_CRS));
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of questions per game",
			 "SELECT AVG(NumQsts) FROM"
			 " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
			 " FROM games,gam_questions"
			 " WHERE games.Scope='%s' AND games.Cod=%ld"
			 " AND games.GamCod=gam_questions.GamCod"
			 " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                         Sco_GetDBStrFromScope (Hie_CRS),Gbl.Hierarchy.Crs.CrsCod);
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
