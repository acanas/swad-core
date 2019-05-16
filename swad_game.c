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

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Gam_ListAllGames (void);
static bool Gam_CheckIfICanCreateGame (void);
static void Gam_PutIconsListGames (void);
static void Gam_PutIconToCreateNewGame (void);
static void Gam_PutButtonToCreateNewGame (void);
static void Gam_PutParamsToCreateNewGame (void);
static void Gam_ParamsWhichGroupsToShow (void);
static void Gam_ShowOneGame (long GamCod,
                             bool ShowOnlyThisGame,
                             bool ListGameQuestions,
                             bool PutButtonToStart);
static void Gam_WriteAuthor (struct Game *Game);
static void Gam_WriteStatus (struct Game *Game);
static void Gam_GetParamGameOrder (void);

static void Gam_PutFormsToRemEditOneGame (const struct Game *Game,
					  const char *Anchor,
                                          bool ShowOnlyThisGame);
static void Gam_PutParamsToPlayGame1stQst (void);
static void Gam_PutParams (void);

static void Gam_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed);

static void Gam_GetGameTxtFromDB (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

static void Gam_PutButtonToResetGame (void);

static bool Gam_CheckIfSimilarGameExists (struct Game *Game);
static void Gam_SetDefaultAndAllowedScope (struct Game *Game);
static void Gam_ShowLstGrpsToEditGame (long GamCod);

static void Gam_CreateGame (struct Game *Game,const char *Txt);
static void Gam_UpdateGame (struct Game *Game,const char *Txt);
static bool Gam_CheckIfGamIsAssociatedToGrps (long GamCod);
static void Gam_RemoveAllTheGrpsAssociatedToAndGame (long GamCod);
static void Gam_CreateGrps (long GamCod);
static void Gam_GetAndWriteNamesOfGrpsAssociatedToGame (struct Game *Game);
static bool Gam_CheckIfICanDoThisGameBasedOnGrps (long GamCod);

static unsigned Gam_GetNumQstsGame (long GamCod);
static void Gam_PutParamQstCod (long QstCod);
static long Gam_GetParamQstCod (void);
static void Gam_PutParamQstInd (unsigned QstInd);
static unsigned Gam_GetParamQstInd (void);
static void Gam_RemAnswersOfAQuestion (long QstCod);

static int Gam_GetQstIndFromQstCod (long GamCod,long QstCod);	// TODO: Remove this function because a question code can be repeated
static long Gam_GetQstCodFromQstInd (long GamCod,unsigned QstInd);
static int Gam_GetMaxQuestionIndexInGame (long GamCod);
static int Gam_GetPrevQuestionIndexInGame (long GamCod,unsigned QstInd);
static int Gam_GetNextQuestionIndexInGame (long GamCod,unsigned QstInd);
static void Gam_ListGameQuestions (struct Game *Game);
static void Gam_ListOneOrMoreQuestionsForEdition (struct Game *Game,
                                                  unsigned NumQsts,
                                                  MYSQL_RES *mysql_res);
static void Gam_PutIconToAddNewQuestions (void);
static void Gam_PutButtonToAddNewQuestions (void);

static void Gam_AllocateListSelectedQuestions (void);
static void Gam_FreeListsSelectedQuestions (void);
static unsigned Gam_CountNumQuestionsInList (void);

static unsigned Gam_GetNumUsrsWhoAnswered (long GamCod,long QstCod,unsigned AnsInd);
static void Gam_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs);

// static void Gam_PutIconToRemoveOneQst (void);
static void Gam_PutParamsOneQst (void);

static void Gam_ExchangeQuestions (long GamCod,
                                   unsigned QstIndTop,unsigned QstIndBottom);

static void Gam_PutBigButtonToStartGame (long GamCod);

static void Gam_PlayGameShowQuestionAndAnswers (bool ShowAnswers);
static void Gam_PutBigButtonToContinue (Act_Action_t NextAction,
                                        long GamCod,unsigned QstInd);

static void Gam_ReceiveAndStoreUserAnswersToAGame (long GamCod);
static void Gam_IncreaseAnswerInDB (long QstCod,unsigned AnsInd);
static void Gam_RegisterIHaveAnsweredGame (long GamCod);
static bool Gam_CheckIfIHaveAnsweredGame (long GamCod);
static unsigned Gam_GetNumUsrsWhoHaveAnsweredGame (long GamCod);

/*****************************************************************************/
/*************************** List all the games ******************************/
/*****************************************************************************/

void Gam_SeeAllGames (void)
  {
   /***** Get parameters *****/
   Gam_GetParamGameOrder ();
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
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Game;
   extern const char *Txt_Status;
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

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.Crs.Grps.NumGrps)
     {
      Set_StartSettingsHead ();
      Grp_ShowFormToSelWhichGrps (ActSeeAllGam,Gam_ParamsWhichGroupsToShow);
      Set_EndSettingsHead ();
     }

   if (Gbl.Games.Num)
     {
      /***** Table head *****/
      Tbl_StartTableWideMargin (2);
      fprintf (Gbl.F.Out,"<tr>"
			 "<th class=\"CONTEXT_COL\"></th>");	// Column for contextual icons
      for (Order = Gam_ORDER_BY_START_DATE;
	   Order <= Gam_ORDER_BY_END_DATE;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");

	 /* Form to change order */
	 Frm_StartForm (ActSeeAllGam);
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Frm_LinkFormSubmit (Txt_START_END_TIME_HELP[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Games.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_START_END_TIME[Order]);
	 if (Order == Gbl.Games.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Frm_EndForm ();

	 fprintf (Gbl.F.Out,"</th>");
	}
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">"
			 "%s"
			 "</th>"
			 "<th class=\"CENTER_MIDDLE\">"
			 "%s"
			 "</th>"
			 "</tr>",
	       Txt_Game,
	       Txt_Status);

      /***** Write all the games *****/
      for (NumGame = Pagination.FirstItemVisible;
	   NumGame <= Pagination.LastItemVisible;
	   NumGame++)
	 Gam_ShowOneGame (Gbl.Games.LstGamCods[NumGame - 1],
	                  false,
	                  false,
	                  false);

      /***** End table *****/
      Tbl_EndTable ();
     }
   else	// No games created
      Ale_ShowAlert (Ale_INFO,Txt_No_games);

   /***** Button to create a new game *****/
   if (Gam_CheckIfICanCreateGame ())
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

static bool Gam_CheckIfICanCreateGame (void)
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
   if (Gam_CheckIfICanCreateGame ())
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
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
  }

/*****************************************************************************/
/**************** Put params to select which groups to show ******************/
/*****************************************************************************/

static void Gam_ParamsWhichGroupsToShow (void)
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
   Gam_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Show game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
                    false);
  }

/*****************************************************************************/
/******************************* Show one game *******************************/
/*****************************************************************************/

static void Gam_ShowOneGame (long GamCod,
                             bool ShowOnlyThisGame,
                             bool ListGameQuestions,
                             bool PutButtonToStart)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Game;
   extern const char *Txt_Today;
   extern const char *Txt_View_game;
   extern const char *Txt_No_of_questions;
   extern const char *Txt_No_of_users;
   extern const char *Txt_Scope;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_Users;
   extern const char *Txt_Play;
   extern const char *Txt_View_game_results;
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

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Game.GamCod,&Anchor);

   /***** Start table *****/
   if (ShowOnlyThisGame)
      Tbl_StartTableWide (2);

   /***** Write first row of data of this assignment *****/
   /* Forms to remove/edit this assignment */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td rowspan=\"2\" class=\"CONTEXT_COL");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   if (Game.Status.ICanEdit)
      Gam_PutFormsToRemEditOneGame (&Game,Anchor,ShowOnlyThisGame);
   fprintf (Gbl.F.Out,"</td>");

   /* Start date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"gam_date_start_%u\" class=\"%s LEFT_TOP",
	    UniqueId,
            Game.Status.Visible ? (Game.Status.Open ? "DATE_GREEN" :
        	                                      "DATE_RED") :
                                  (Game.Status.Open ? "DATE_GREEN_LIGHT" :
                                                      "DATE_RED_LIGHT"));
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('gam_date_start_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>"
	              "</td>",
            UniqueId,Game.TimeUTC[Gam_START_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /* End date/time */
   fprintf (Gbl.F.Out,"<td id=\"gam_date_end_%u\" class=\"%s LEFT_TOP",
            UniqueId,
            Game.Status.Visible ? (Game.Status.Open ? "DATE_GREEN" :
        	                                      "DATE_RED") :
                                  (Game.Status.Open ? "DATE_GREEN_LIGHT" :
                                                      "DATE_RED_LIGHT"));
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('gam_date_end_%u',%ld,"
                      "%u,'<br />','%s',false,true,0x7);"
                      "</script>"
	              "</td>",
            UniqueId,Game.TimeUTC[Gam_END_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /* Game title */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Lay_StartArticle (Anchor);
   Frm_StartForm (ActSeeOneGam);
   Gam_PutParamGameCod (GamCod);
   Gam_PutHiddenParamGameOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
   Frm_LinkFormSubmit (Txt_View_game,
                       Game.Status.Visible ? "ASG_TITLE" :
	                                     "ASG_TITLE_LIGHT",NULL);
   fprintf (Gbl.F.Out,"%s</a>",
            Game.Title);
   Frm_EndForm ();
   Lay_EndArticle ();

   /* Number of questions and number of distinct users who have already answered this game */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: %u; %s: %u</div>"
	              "</td>",
            Game.Status.Visible ? "ASG_GRP" :
        	                  "ASG_GRP_LIGHT",
            Txt_No_of_questions,
            Game.NumQsts,
            Txt_No_of_users,
            Game.NumUsrs);

   /* Status of the game */
   fprintf (Gbl.F.Out,"<td rowspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Gam_WriteStatus (&Game);

   if (ShowOnlyThisGame)
     {
      fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");

      Frm_StartForm (ActPlyGam);
      Gam_PutParamGameCod (Game.GamCod);
      Gam_PutHiddenParamGameOrder ();
      Grp_PutParamWhichGrps ();
      Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
      Btn_PutCreateButtonInline (Txt_Play);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else	// Show several games
     {
      /* Possible button to answer this game */
      if (Game.Status.ICanAnswer)
	{
	 fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");

	 Frm_StartForm (ActSeeOneGam);
	 Gam_PutParamGameCod (Game.GamCod);
	 Gam_PutHiddenParamGameOrder ();
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
	 Btn_PutCreateButtonInline (Txt_Play);
	 Frm_EndForm ();

	 fprintf (Gbl.F.Out,"</div>");
	}
      /* Possible button to see the result of the game */
      else if (Game.Status.ICanViewResults)
	{
	 fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");

	 Frm_StartForm (ActSeeOneGam);
	 Gam_PutParamGameCod (Game.GamCod);
	 Gam_PutHiddenParamGameOrder ();
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
	 Btn_PutConfirmButtonInline (Txt_View_game_results);
	 Frm_EndForm ();

	 fprintf (Gbl.F.Out,"</div>");
	}
     }

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Write second row of data of this game *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Author of the game */
   Gam_WriteAuthor (&Game);

   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"LEFT_TOP");
   if (!ShowOnlyThisGame)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Scope of the game */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: ",
            Game.Status.Visible ? "ASG_GRP" :
        	                  "ASG_GRP_LIGHT",
            Txt_Scope);
   switch (Game.Scope)
     {
      case Hie_UNK:	// Unknown
         Lay_ShowErrorAndExit ("Wrong game scope.");
         break;
      case Hie_SYS:	// System
         fprintf (Gbl.F.Out,"%s",
                  Cfg_PLATFORM_SHORT_NAME);
	 break;
      case Hie_CTY:	// Country
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Country,Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
	 break;
      case Hie_INS:	// Institution
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Institution,Gbl.Hierarchy.Ins.ShrtName);
	 break;
      case Hie_CTR:	// Centre
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Centre,Gbl.Hierarchy.Ctr.ShrtName);
	 break;
      case Hie_DEG:	// Degree
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Degree,Gbl.Hierarchy.Deg.ShrtName);
 	 break;
      case Hie_CRS:	// Course
	 fprintf (Gbl.F.Out,"%s %s",
	          Txt_Course,Gbl.Hierarchy.Crs.ShrtName);
	 break;
     }
   fprintf (Gbl.F.Out,"</div>");

   /* Users' roles who can answer the game */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s:<br />",
            Game.Status.Visible ? "ASG_GRP" :
        	                  "ASG_GRP_LIGHT",
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_STD |
                           1 << Rol_NET |
			   1 << Rol_TCH,
			   Game.Roles,
			   true,false);
   fprintf (Gbl.F.Out,"</div>");

   /* Groups whose users can answer this game */
   if (Game.Scope == Hie_CRS)
      if (Gbl.Crs.Grps.NumGrps)
         Gam_GetAndWriteNamesOfGrpsAssociatedToGame (&Game);

   /* Text of the game */
   Gam_GetGameTxtFromDB (Game.GamCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<div class=\"PAR %s\">%s</div>"
                      "</td>"
                      "</tr>",
            Game.Status.Visible ? "DAT" :
        	                  "DAT_LIGHT",
            Txt);

   /***** Write questions of this game *****/
   if (ListGameQuestions)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"5\">");
      Gam_ListGameQuestions (&Game);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End table *****/
   if (ShowOnlyThisGame)
      Tbl_EndTable ();
   else
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   /***** Put big button to start playing *****/
   if (PutButtonToStart)
      Gam_PutBigButtonToStartGame (Game.GamCod);

   /***** End box *****/
   if (ShowOnlyThisGame)
      Box_EndBox ();
  }

/*****************************************************************************/
/*********************** Write the author of a game ************************/
/*****************************************************************************/

static void Gam_WriteAuthor (struct Game *Game)
  {
   Usr_WriteAuthor1Line (Game->UsrCod,!Game->Status.Visible);
  }

/*****************************************************************************/
/************************ Write status of a game ***************************/
/*****************************************************************************/

static void Gam_WriteStatus (struct Game *Game)
  {
   extern const char *Txt_Hidden_game;
   extern const char *Txt_Visible_game;
   extern const char *Txt_Closed_game;
   extern const char *Txt_Open_game;
   extern const char *Txt_SURVEY_Type_of_user_not_allowed;
   extern const char *Txt_SURVEY_Type_of_user_allowed;
   extern const char *Txt_GAME_You_belong_to_the_scope_of_the_game;
   extern const char *Txt_GAME_You_dont_belong_to_the_scope_of_the_game;
   extern const char *Txt_SURVEY_You_have_already_answered;
   extern const char *Txt_SURVEY_You_have_not_answered;

   /***** Start list with items of status *****/
   fprintf (Gbl.F.Out,"<ul>");

   /* Write whether game is visible or hidden */
   if (Game->Status.Visible)
      fprintf (Gbl.F.Out,"<li class=\"STATUS_GREEN\">%s</li>",
               Txt_Visible_game);
   else
      fprintf (Gbl.F.Out,"<li class=\"STATUS_RED_LIGHT\">%s</li>",
               Txt_Hidden_game);

   /* Write whether game is open or closed */
   if (Game->Status.Open)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_Open_game);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_Closed_game);

   /* Write whether game can be answered by me or not depending on user type */
   if (Game->Status.IAmLoggedWithAValidRoleToAnswer)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_SURVEY_Type_of_user_allowed);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_SURVEY_Type_of_user_not_allowed);

   /* Write whether game can be answered by me or not depending on groups */
   if (Game->Status.IBelongToScope)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_GAME_You_belong_to_the_scope_of_the_game);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_GAME_You_dont_belong_to_the_scope_of_the_game);

   /* Write whether game has been already answered by me or not */
   if (Game->Status.IHaveAnswered)
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_GREEN" :
        	                     "STATUS_GREEN_LIGHT",
               Txt_SURVEY_You_have_already_answered);
   else
      fprintf (Gbl.F.Out,"<li class=\"%s\">%s</li>",
               Game->Status.Visible ? "STATUS_RED" :
        	                     "STATUS_RED_LIGHT",
               Txt_SURVEY_You_have_not_answered);

   /***** End list with items of status *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of games ************/
/*****************************************************************************/

static void Gam_GetParamGameOrder (void)
  {
   Gbl.Games.SelectedOrder = (Gam_Order_t)
	                     Par_GetParToUnsignedLong ("Order",
	                                               0,
	                                               Gam_NUM_ORDERS - 1,
	                                               (unsigned long) Gam_ORDER_DEFAULT);
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
					  const char *Anchor,
                                          bool ShowOnlyThisGame)
  {
   extern const char *Txt_Reset;
   extern const char *Txt_Play;

   Gbl.Games.CurrentGamCod = Game->GamCod;	// Used as parameter in contextual links

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

   if (ShowOnlyThisGame)
      /***** Put icon to show first question *****/
      Lay_PutContextualLinkOnlyIcon (ActPlyGam1stQst,NULL,
				     Gam_PutParamsToPlayGame1stQst,
				     "play.svg",
				     Txt_Play);
   else
      /***** Put icon to play game *****/
      Lay_PutContextualLinkOnlyIcon (ActPlyGam,NULL,Gam_PutParams,
				     "play.svg",
				     Txt_Play);
  }

/*****************************************************************************/
/************* Params used to play the first question of a game **************/
/*****************************************************************************/

static void Gam_PutParamsToPlayGame1stQst (void)
  {
   Gam_PutParams ();
   Gam_PutParamQstInd (0);	// Start by first question in game
  }

/*****************************************************************************/
/******************** Params used to edit/play a game ************************/
/*****************************************************************************/

static void Gam_PutParams (void)
  {
   if (Gbl.Games.CurrentGamCod > 0)
      Gam_PutParamGameCod (Gbl.Games.CurrentGamCod);

   Att_PutHiddenParamAttOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_GAMES,Gbl.Games.CurrentPage);
  }

/*****************************************************************************/
/*********************** Get list of all the games *************************/
/*****************************************************************************/

void Gam_GetListGames (void)
  {
   char *SubQuery[Hie_NUM_LEVELS];
   static const char *OrderBySubQuery[Gam_NUM_ORDERS] =
     {
      "StartTime DESC,EndTime DESC,Title DESC",	// Gam_ORDER_BY_START_DATE
      "EndTime DESC,StartTime DESC,Title DESC",	// Gam_ORDER_BY_END_DATE
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned NumGame;
   unsigned ScopesAllowed = 0;
   unsigned HiddenAllowed = 0;
   long Cods[Hie_NUM_LEVELS];
   Hie_Level_t Scope;
   bool SubQueryFilled = false;

   /***** Free list of games *****/
   if (Gbl.Games.LstIsRead)
      Gam_FreeListGames ();

   /***** Set allowed and hidden scopes to get list depending on my user's role *****/
   Gam_SetAllowedAndHiddenScopes (&ScopesAllowed,&HiddenAllowed);

   /***** Get list of games from database *****/
   Cods[Hie_SYS] = -1L;				// System
   Cods[Hie_CTY] = Gbl.Hierarchy.Cty.CtyCod;	// Country
   Cods[Hie_INS] = Gbl.Hierarchy.Ins.InsCod;	// Institution
   Cods[Hie_CTR] = Gbl.Hierarchy.Ctr.CtrCod;	// Centre
   Cods[Hie_DEG] = Gbl.Hierarchy.Deg.DegCod;	// Degree
   Cods[Hie_CRS] = Gbl.Hierarchy.Crs.CrsCod;	// Course

   /* Fill subqueries for system, country, institution, centre and degree */
   for (Scope  = Hie_SYS;
	Scope <= Hie_DEG;
	Scope++)
      if (ScopesAllowed & 1 << Scope)
	{
	 if (asprintf (&SubQuery[Scope],"%s(Scope='%s' AND Cod=%ld%s)",
	               SubQueryFilled ? " OR " :
	        	                "",
		       Sco_GetDBStrFromScope (Scope),Cods[Scope],
		       (HiddenAllowed & 1 << Scope) ? "" :
						      " AND Hidden='N'") < 0)
	    Lay_NotEnoughMemoryExit ();
	 SubQueryFilled = true;
	}
      else
        {
	 if (asprintf (&SubQuery[Scope],"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
        }

   /* Fill subquery for course */
   if (ScopesAllowed & 1 << Hie_CRS)
     {
      if (Gbl.Crs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
        {
	 if (asprintf (&SubQuery[Hie_CRS],"%s("
						"Scope='%s' AND Cod=%ld%s"
						" AND "
						"(GamCod NOT IN"
						" (SELECT GamCod FROM gam_grp)"
						" OR"
						" GamCod IN"
						" (SELECT gam_grp.GamCod"
						" FROM gam_grp,crs_grp_usr"
						" WHERE crs_grp_usr.UsrCod=%ld"
						" AND gam_grp.GrpCod=crs_grp_usr.GrpCod))"
						")",
		       SubQueryFilled ? " OR " :
					"",
		       Sco_GetDBStrFromScope (Hie_CRS),Cods[Hie_CRS],
		       (HiddenAllowed & 1 << Hie_CRS) ? "" :
							" AND Hidden='N'",
		       Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	    Lay_NotEnoughMemoryExit ();
        }
      else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
        {
	 if (asprintf (&SubQuery[Hie_CRS],"%s(Scope='%s' AND Cod=%ld%s)",
		       SubQueryFilled ? " OR " :
					"",
		       Sco_GetDBStrFromScope (Hie_CRS),Cods[Hie_CRS],
		       (HiddenAllowed & 1 << Hie_CRS) ? "" :
							" AND Hidden='N'") < 0)
	    Lay_NotEnoughMemoryExit ();
        }
      SubQueryFilled = true;
     }
   else
     {
      if (asprintf (&SubQuery[Hie_CRS],"%s","") < 0)
	 Lay_NotEnoughMemoryExit ();
     }

   /* Make query */
   if (SubQueryFilled)
     {
      /* Make query */
      NumRows = DB_QuerySELECT (&mysql_res,"can not get games",
				"SELECT GamCod FROM games"
				" WHERE %s%s%s%s%s%s"
				" ORDER BY %s",
				SubQuery[Hie_SYS],
				SubQuery[Hie_CTY],
				SubQuery[Hie_INS],
				SubQuery[Hie_CTR],
				SubQuery[Hie_DEG],
				SubQuery[Hie_CRS],
				OrderBySubQuery[Gbl.Games.SelectedOrder]);
     }
   else
      Lay_ShowErrorAndExit ("Can not get list of games.");

   /* Free allocated memory for subqueries */
   for (Scope  = Hie_SYS;
	Scope <= Hie_CRS;
	Scope++)
      free ((void *) SubQuery[Scope]);

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
/*** Set allowed and hidden scopes to get list depending on my user's role ***/
/*****************************************************************************/

static void Gam_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_UNK:	// User not logged in *********************************
	 *ScopesAllowed = 0;
	 *HiddenAllowed = 0;
         break;
      case Rol_GST:	// User not belonging to any course *******************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 break;
      case Rol_USR:	// Student or teacher in other courses...
   	   	   	// ...but not belonging to the current course *********
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.Hierarchy.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.Hierarchy.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.Hierarchy.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.Hierarchy.Deg.DegCod))
		     *ScopesAllowed |= 1 << Hie_DEG;
		 }
	      }
	   }
         break;
      case Rol_STD:	// Student in current course **************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.Hierarchy.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.Hierarchy.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.Hierarchy.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.Hierarchy.Deg.DegCod))
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
		     if (Gbl.Usrs.Me.IBelongToCurrentCrs)
			*ScopesAllowed |= 1 << Hie_CRS;
		    }
		 }
	      }
	   }
         break;
      case Rol_NET:	// Non-editing teacher in current course **************
      case Rol_TCH:	// Teacher in current course **************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.Hierarchy.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.Hierarchy.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.Hierarchy.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.Hierarchy.Deg.DegCod))
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
		     if (Gbl.Usrs.Me.IBelongToCurrentCrs)
		       {
			*ScopesAllowed |= 1 << Hie_CRS;
			*HiddenAllowed |= 1 << Hie_CRS;	// A non-editing teacher or teacher can view hidden course games
		       }
		    }
		 }
	      }
	   }
         break;
      case Rol_DEG_ADM:	// Degree administrator *******************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.Hierarchy.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Gbl.Hierarchy.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Gbl.Hierarchy.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  if (Gbl.Hierarchy.Deg.DegCod > 0)		// Degree selected
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
		     *HiddenAllowed |= 1 << Hie_DEG;	// A degree admin can view hidden degree games
		    }
	         }
	      }
	   }
	 break;
      case Rol_CTR_ADM:	// Centre administrator *******************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.Hierarchy.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Gbl.Hierarchy.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       if (Gbl.Hierarchy.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
		  *HiddenAllowed |= 1 << Hie_CTR;		// A centre admin can view hidden centre games
		 }
	      }
	   }
	 break;
      case Rol_INS_ADM:	// Institution administrator **************************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.Hierarchy.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    if (Gbl.Hierarchy.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       *HiddenAllowed |= 1 << Hie_INS;		// An institution admin can view hidden institution games
	      }
	   }
	 break;
      case Rol_SYS_ADM:	// System administrator (superuser) *******************
	 *ScopesAllowed = 1 << Hie_SYS;
	 *HiddenAllowed = 1 << Hie_SYS;			// A system admin can view hidden system games
	 if (Gbl.Hierarchy.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Hie_CTY;
	    *HiddenAllowed |= 1 << Hie_CTY;		// A system admin can view hidden country games
	    if (Gbl.Hierarchy.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Hie_INS;
	       *HiddenAllowed |= 1 << Hie_INS;		// A system admin can view hidden institution games
	       if (Gbl.Hierarchy.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Hie_CTR;
	          *HiddenAllowed |= 1 << Hie_CTR;		// A system admin can view hidden centre games
		  if (Gbl.Hierarchy.Deg.DegCod > 0)		// Degree selected
		    {
		     *ScopesAllowed |= 1 << Hie_DEG;
	             *HiddenAllowed |= 1 << Hie_DEG;	// A system admin can view hidden degree games
		     if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
		       {
			*ScopesAllowed |= 1 << Hie_CRS;
	                *HiddenAllowed |= 1 << Hie_CRS;	// A system admin can view hidden course games
		       }
		    }
		 }
	      }
	   }
	 break;
     }
  }

/*****************************************************************************/
/********************* Get game data using its code ************************/
/*****************************************************************************/

void Gam_GetDataOfGameByCod (struct Game *Game)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get data of game from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get game data",
			     "SELECT GamCod,Scope,Cod,Hidden,Roles,UsrCod,"
			     "UNIX_TIMESTAMP(StartTime),"
			     "UNIX_TIMESTAMP(EndTime),"
			     "NOW() BETWEEN StartTime AND EndTime,"
			     "Title"
			     " FROM games"
			     " WHERE GamCod=%ld",
			     Game->GamCod);
   if (NumRows) // Game found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the game (row[0]) */
      Game->GamCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get game scope (row[1]) */
      if ((Game->Scope = Sco_GetScopeFromDBStr (row[1])) == Hie_UNK)
         Lay_ShowErrorAndExit ("Wrong game scope.");

      /* Get code of the country, institution, centre, degree or course (row[2]) */
      Game->Cod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get whether the game is hidden (row[3]) */
      Game->Status.Visible = (row[3][0] == 'N');

      /* Get roles (row[4]) */
      if (sscanf (row[4],"%u",&Game->Roles) != 1)
      	 Lay_ShowErrorAndExit ("Error when reading roles of game.");

      /* Get author of the game (row[5]) */
      Game->UsrCod = Str_ConvertStrCodToLongCod (row[5]);

      /* Get start date (row[6] holds the start UTC time) */
      Game->TimeUTC[Att_START_TIME] = Dat_GetUNIXTimeFromStr (row[6]);

      /* Get end   date (row[7] holds the end   UTC time) */
      Game->TimeUTC[Att_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[7]);

      /* Get whether the game is open or closed (row(8)) */
      Game->Status.Open = (row[8][0] == '1');

      /* Get the title of the game (row[9]) */
      Str_Copy (Game->Title,row[9],
                Gam_MAX_BYTES_SURVEY_TITLE);

      /* Get number of questions and number of users who have already answer this game */
      Game->NumQsts = Gam_GetNumQstsGame (Game->GamCod);
      Game->NumUsrs = Gam_GetNumUsrsWhoHaveAnsweredGame (Game->GamCod);

      /* Am I logged with a valid role to answer this game? */
      Game->Status.IAmLoggedWithAValidRoleToAnswer = (Game->Roles & (1 << Gbl.Usrs.Me.Role.Logged));

      /* Do I belong to valid groups to answer this game? */
      switch (Game->Scope)
        {
	 case Hie_UNK:	// Unknown
            Lay_ShowErrorAndExit ("Wrong game scope.");
	    break;
	 case Hie_SYS:	// System
            Game->Status.IBelongToScope = Gbl.Usrs.Me.Logged;
	    break;
	 case Hie_CTY:	// Country
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToCty (Game->Cod);
	    break;
	 case Hie_INS:	// Institution
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToIns (Game->Cod);
	    break;
	 case Hie_CTR:	// Centre
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToCtr (Game->Cod);
	    break;
	 case Hie_DEG:	// Degree
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToDeg (Game->Cod);
	    break;
	 case Hie_CRS:	// Course
	    Game->Status.IBelongToScope = Usr_CheckIfIBelongToCrs (Game->Cod) &&
					  Gam_CheckIfICanDoThisGameBasedOnGrps (Game->GamCod);
	    break;
        }

      /* Have I answered this game? */
      Game->Status.IHaveAnswered = Gam_CheckIfIHaveAnsweredGame (Game->GamCod);

      /* Can I answer game? */
      Game->Status.ICanAnswer = (Game->NumQsts != 0) &&
                                 Game->Status.Visible &&
                                 Game->Status.Open &&
                                 Game->Status.IAmLoggedWithAValidRoleToAnswer &&
                                 Game->Status.IBelongToScope &&
                                !Game->Status.IHaveAnswered;

      /* Can I view results of the game?
         Can I edit game? */
      switch (Gbl.Usrs.Me.Role.Logged)
        {
         case Rol_STD:
            Game->Status.ICanViewResults = (Game->Scope == Hie_CRS ||
        	                            Game->Scope == Hie_DEG ||
        	                            Game->Scope == Hie_CTR ||
        	                            Game->Scope == Hie_INS ||
        	                            Game->Scope == Hie_CTY ||
        	                            Game->Scope == Hie_SYS) &&
        	                           (Game->NumQsts != 0) &&
                                            Game->Status.Visible &&
                                            Game->Status.Open &&
                                            Game->Status.IAmLoggedWithAValidRoleToAnswer &&
                                            Game->Status.IBelongToScope &&
                                            Game->Status.IHaveAnswered;
            Game->Status.ICanEdit         = false;
            break;
         case Rol_NET:
            Game->Status.ICanViewResults = (Game->Scope == Hie_CRS ||
        	                            Game->Scope == Hie_DEG ||
        	                            Game->Scope == Hie_CTR ||
        	                            Game->Scope == Hie_INS ||
        	                            Game->Scope == Hie_CTY ||
        	                            Game->Scope == Hie_SYS) &&
        	                            Game->NumQsts != 0 &&
                                           !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        = false;
            break;
         case Rol_TCH:
            Game->Status.ICanViewResults = (Game->Scope == Hie_CRS ||
        	                            Game->Scope == Hie_DEG ||
        	                            Game->Scope == Hie_CTR ||
        	                            Game->Scope == Hie_INS ||
        	                            Game->Scope == Hie_CTY ||
        	                            Game->Scope == Hie_SYS) &&
        	                            Game->NumQsts != 0 &&
                                           !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Hie_CRS &&
                                            Game->Status.IBelongToScope;
            break;
         case Rol_DEG_ADM:
            Game->Status.ICanViewResults = (Game->Scope == Hie_DEG ||
        	                            Game->Scope == Hie_CTR ||
        	                            Game->Scope == Hie_INS ||
        	                            Game->Scope == Hie_CTY ||
        	                            Game->Scope == Hie_SYS) &&
        	                           (Game->NumQsts != 0) &&
                                           !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Hie_DEG &&
                                            Game->Status.IBelongToScope;
            break;
         case Rol_CTR_ADM:
            Game->Status.ICanViewResults = (Game->Scope == Hie_CTR ||
        	                            Game->Scope == Hie_INS ||
        	                            Game->Scope == Hie_CTY ||
        	                            Game->Scope == Hie_SYS) &&
        	                           (Game->NumQsts != 0) &&
                                           !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Hie_CTR &&
                                            Game->Status.IBelongToScope;
            break;
         case Rol_INS_ADM:
            Game->Status.ICanViewResults = (Game->Scope == Hie_INS ||
        	                            Game->Scope == Hie_CTY ||
        	                            Game->Scope == Hie_SYS) &&
        	                           (Game->NumQsts != 0) &&
                                           !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Hie_INS &&
                                            Game->Status.IBelongToScope;
            break;
         case Rol_SYS_ADM:
            Game->Status.ICanViewResults = (Game->NumQsts != 0);
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
      Game->Scope                   = Hie_UNK;
      Game->Roles                   = 0;
      Game->UsrCod                  = -1L;
      Game->TimeUTC[Gam_START_TIME] =
      Game->TimeUTC[Gam_END_TIME  ] = (time_t) 0;
      Game->Title[0]                = '\0';
      Game->NumQsts                 = 0;
      Game->NumUsrs                 = 0;
      Game->Status.Visible                         = true;
      Game->Status.Open                            = false;
      Game->Status.IAmLoggedWithAValidRoleToAnswer = false;
      Game->Status.IBelongToScope                  = false;
      Game->Status.IHaveAnswered                   = false;
      Game->Status.ICanAnswer                      = false;
      Game->Status.ICanViewResults                 = false;
      Game->Status.ICanEdit                        = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** Free list of games ***************************/
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
/******************* Write parameter with code of game *********************/
/*****************************************************************************/

void Gam_PutParamGameCod (long GamCod)
  {
   Par_PutHiddenParamLong ("GamCod",GamCod);
  }

/*****************************************************************************/
/******************** Get parameter with code of game **********************/
/*****************************************************************************/

long Gam_GetParamGameCod (void)
  {
   /***** Get code of game *****/
   return Par_GetParToLong ("GamCod");
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing of a game ****************/
/*****************************************************************************/

void Gam_AskRemGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_game_X;
   extern const char *Txt_Remove_game;
   struct Game Game;

   /***** Get parameters *****/
   Gam_GetParamGameOrder ();
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
   Gbl.Games.CurrentGamCod = Game.GamCod;
   Ale_ShowAlertAndButton (ActRemGam,NULL,NULL,Gam_PutParams,
			   Btn_REMOVE_BUTTON,Txt_Remove_game,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_game_X,
                           Game.Title);

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/****************************** Remove a game ******************************/
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

   /***** Remove all the users in this game *****/
   DB_QueryDELETE ("can not remove users who are answered a game",
		   "DELETE FROM gam_users WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Remove all the questions in this game *****/
   DB_QueryDELETE ("can not remove questions of a game",
		   "DELETE FROM gam_questions WHERE GamCod=%ld",
		   Game.GamCod);

   /***** Remove all the groups of this game *****/
   Gam_RemoveAllTheGrpsAssociatedToAndGame (Game.GamCod);

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
   struct Game Game;

   /***** Get parameters *****/
   Gam_GetParamGameOrder ();
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
   Ale_ShowAlert (Ale_WARNING,Txt_Do_you_really_want_to_reset_the_game_X,
                  Game.Title);

   /***** Button of confirmation of reset *****/
   Gbl.Games.CurrentGamCod = Game.GamCod;
   Gam_PutButtonToResetGame ();

   /***** Show games again *****/
   Gam_ListAllGames ();
  }

/*****************************************************************************/
/************************* Put button to reset game ************************/
/*****************************************************************************/

static void Gam_PutButtonToResetGame (void)
  {
   extern const char *Txt_Reset_game;

   Frm_StartForm (ActRstGam);
   Gam_PutParams ();
   Btn_PutRemoveButton (Txt_Reset_game);
   Frm_EndForm ();
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

   /***** Remove all the users in this game *****/
   DB_QueryDELETE ("can not remove users who are answered a game",
		   "DELETE FROM gam_users WHERE GamCod=%ld",
		   Game.GamCod);

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
			  " WHERE Scope='%s' AND Cod=%ld"
			  " AND Title='%s' AND GamCod<>%ld",
			  Sco_GetDBStrFromScope (Game->Scope),Game->Cod,
			  Game->Title,Game->GamCod) != 0);
  }

/*****************************************************************************/
/********************* Put a form to create a new game *********************/
/*****************************************************************************/

void Gam_RequestCreatOrEditGame (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_new_game;
   extern const char *Hlp_ASSESSMENT_Games_edit_game;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_game;
   extern const char *Txt_Scope;
   extern const char *Txt_Edit_game;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Users;
   extern const char *Txt_Create_game;
   extern const char *Txt_Save_changes;
   struct Game Game;
   bool ItsANewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get parameters *****/
   Gam_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get the code of the game *****/
   ItsANewGame = ((Game.GamCod = Gam_GetParamGameCod ()) == -1L);

   /***** Get from the database the data of the game *****/
   if (ItsANewGame)
     {
      /***** Put link (form) to create new game *****/
      if (!Gam_CheckIfICanCreateGame ())
         Lay_ShowErrorAndExit ("You can not create a new game here.");

      /* Initialize to empty game */
      Game.GamCod = -1L;
      Game.Scope  = Hie_UNK;
      Game.Roles  = (1 << Rol_STD);
      Game.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Game.TimeUTC[Gam_START_TIME] = Gbl.StartExecutionTimeUTC;
      Game.TimeUTC[Gam_END_TIME  ] = Gbl.StartExecutionTimeUTC + (24 * 60 * 60);	// +24 hours
      Game.Title[0] = '\0';
      Game.NumQsts = 0;
      Game.NumUsrs = 0;
      Game.Status.Visible = true;
      Game.Status.Open = true;
      Game.Status.IAmLoggedWithAValidRoleToAnswer = false;
      Game.Status.IBelongToScope = false;
      Game.Status.IHaveAnswered = false;
      Game.Status.ICanAnswer = false;
      Game.Status.ICanViewResults = false;
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
   Gbl.Games.CurrentGamCod = Game.GamCod;
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

   /***** Scope of the game *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"ScopeGame\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Scope);
   Gam_SetDefaultAndAllowedScope (&Game);
   Sco_GetScope ("ScopeGame");
   Sco_PutSelectorScope ("ScopeGame",false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

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
            Gam_MAX_CHARS_SURVEY_TITLE,Game.Title);

   /***** Game start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Game.TimeUTC,Dat_FORM_SECONDS_ON);

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

   /***** Users' roles who can answer the game *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP %s\">%s:"
	              "</td>"
                      "<td class=\"DAT LEFT_MIDDLE\">",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_STD |
                           1 << Rol_NET |
                           1 << Rol_TCH,
                           Game.Roles,
                           false,false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Groups *****/
   Gam_ShowLstGrpsToEditGame (Game.GamCod);

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
/****** Set default and allowed scopes depending on logged user's role *******/
/*****************************************************************************/

static void Gam_SetDefaultAndAllowedScope (struct Game *Game)
  {
   bool ICanEdit = false;

   /***** Set default scope *****/
   Gbl.Scope.Default = Hie_UNK;
   Gbl.Scope.Allowed = 0;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:	// Teachers only can edit course games
	 if (Game->Scope == Hie_UNK)	// Scope not defined
	    Game->Scope = Hie_CRS;
	 if (Game->Scope == Hie_CRS)
	   {
	    Gbl.Scope.Default = Game->Scope;
	    Gbl.Scope.Allowed = 1 << Hie_CRS;
	    ICanEdit = true;
	   }
         break;
      case Rol_DEG_ADM:	// Degree admins only can edit degree games
	 if (Game->Scope == Hie_UNK)	// Scope not defined
	    Game->Scope = Hie_DEG;
	 if (Game->Scope == Hie_DEG)
	   {
	    Gbl.Scope.Default = Game->Scope;
	    Gbl.Scope.Allowed = 1 << Hie_DEG;
	    ICanEdit = true;
	   }
         break;
      case Rol_CTR_ADM:	// Centre admins only can edit centre games
	 if (Game->Scope == Hie_UNK)	// Scope not defined
	    Game->Scope = Hie_CTR;
	 if (Game->Scope == Hie_CTR)
	   {
	    Gbl.Scope.Default = Game->Scope;
	    Gbl.Scope.Allowed = 1 << Hie_CTR;
	    ICanEdit = true;
	   }
         break;
      case Rol_INS_ADM:	// Institution admins only can edit institution games
	 if (Game->Scope == Hie_UNK)	// Scope not defined
	    Game->Scope = Hie_INS;
	 if (Game->Scope == Hie_INS)
	   {
	    Gbl.Scope.Default = Game->Scope;
	    Gbl.Scope.Allowed = 1 << Hie_INS;
	    ICanEdit = true;
	   }
         break;
      case Rol_SYS_ADM:// System admins can edit any game
	 if (Game->Scope == Hie_UNK)	// Scope not defined
	   {
	    if      (Gbl.Hierarchy.Level == Hie_CRS)
	       Game->Scope = Hie_CRS;
	    else if (Gbl.Hierarchy.Deg.DegCod > 0)
	       Game->Scope = Hie_DEG;
	    else if (Gbl.Hierarchy.Ctr.CtrCod > 0)
	       Game->Scope = Hie_CTR;
	    else if (Gbl.Hierarchy.Ins.InsCod > 0)
	       Game->Scope = Hie_INS;
	    else if (Gbl.Hierarchy.Cty.CtyCod > 0)
	       Game->Scope = Hie_CTY;
	    else
	       Game->Scope = Hie_SYS;
	   }
         Gbl.Scope.Default = Game->Scope;
         Gbl.Scope.Allowed = 1 << Hie_SYS |
	                     1 << Hie_CTY |
	                     1 << Hie_INS |
	                     1 << Hie_CTR |
                             1 << Hie_DEG |
                             1 << Hie_CRS;
	 ICanEdit = true;
	 break;
      default:
	 break;
     }

   if (!ICanEdit)
      Lay_NoPermissionExit ();
  }

/*****************************************************************************/
/******************** Show list of groups to edit a game *******************/
/*****************************************************************************/

static void Gam_ShowLstGrpsToEditGame (long GamCod)
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
                         " id=\"WholeCrs\" name=\"WholeCrs\" value=\"Y\"");
      if (!Gam_CheckIfGamIsAssociatedToGrps (GamCod))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"uncheckChildren(this,'GrpCods')\" />"
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
            Grp_ListGrpsToEditAsgAttSvyGam (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
                                               GamCod,Grp_SURVEY);

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

   if (ItsANewGame)
      NewGame.Scope = Hie_UNK;
   else
     {
      /* Get data of the old (current) game from database */
      OldGame.GamCod = NewGame.GamCod;
      Gam_GetDataOfGameByCod (&OldGame);
      if (!OldGame.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this game.");
      NewGame.Scope = OldGame.Scope;
     }

   /***** Get scope *****/
   Gam_SetDefaultAndAllowedScope (&NewGame);
   Sco_GetScope ("ScopeGame");
   switch (Gbl.Scope.Current)
     {
      case Hie_SYS:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
         NewGame.Scope = Hie_SYS;
         NewGame.Cod = -1L;
         break;
      case Hie_CTY:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Hie_CTY;
	 NewGame.Cod = Gbl.Hierarchy.Cty.CtyCod;
         break;
      case Hie_INS:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_INS_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Hie_INS;
	 NewGame.Cod = Gbl.Hierarchy.Ins.InsCod;
         break;
      case Hie_CTR:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_CTR_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Hie_CTR;
	 NewGame.Cod = Gbl.Hierarchy.Ctr.CtrCod;
         break;
      case Hie_DEG:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_DEG_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Hie_DEG;
	 NewGame.Cod = Gbl.Hierarchy.Deg.DegCod;
         break;
      case Hie_CRS:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_TCH)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Hie_CRS;
	 NewGame.Cod = Gbl.Hierarchy.Crs.CrsCod;
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get start/end date-times *****/
   NewGame.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewGame.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get game title *****/
   Par_GetParToText ("Title",NewGame.Title,Gam_MAX_BYTES_SURVEY_TITLE);

   /***** Get game text and insert links *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewGame.TimeUTC[Gam_START_TIME] == 0)
      NewGame.TimeUTC[Gam_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewGame.TimeUTC[Gam_END_TIME] == 0)
      NewGame.TimeUTC[Gam_END_TIME] = NewGame.TimeUTC[Gam_START_TIME] + 24 * 60 * 60;	// +24 hours

   /***** Get users who can answer this game *****/
   NewGame.Roles = Rol_GetSelectedRoles ();

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
      /* Get groups for this games */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewGame)
         Gam_CreateGame (&NewGame,Txt);	// Add new game to database
      else
         Gam_UpdateGame (&NewGame,Txt);

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();
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
				" (Scope,Cod,Hidden,Roles,UsrCod,StartTime,EndTime,Title,Txt)"
				" VALUES"
				" ('%s',%ld,'N',%u,%ld,"
				"FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				"'%s','%s')",
				Sco_GetDBStrFromScope (Game->Scope),Game->Cod,
				Game->Roles,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Game->TimeUTC[Gam_START_TIME],
				Game->TimeUTC[Gam_END_TIME  ],
				Game->Title,
				Txt);

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Gam_CreateGrps (Game->GamCod);

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
		   " SET Scope='%s',Cod=%ld,Roles=%u,"
		   "StartTime=FROM_UNIXTIME(%ld),"
		   "EndTime=FROM_UNIXTIME(%ld),"
		   "Title='%s',Txt='%s'"
		   " WHERE GamCod=%ld",
	           Sco_GetDBStrFromScope (Game->Scope),Game->Cod,
	           Game->Roles,
	           Game->TimeUTC[Gam_START_TIME],
	           Game->TimeUTC[Gam_END_TIME  ],
	           Game->Title,
	           Txt,
	           Game->GamCod);

   /***** Update groups *****/
   /* Remove old groups */
   Gam_RemoveAllTheGrpsAssociatedToAndGame (Game->GamCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Gam_CreateGrps (Game->GamCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_game_has_been_modified);
  }

/*****************************************************************************/
/*************** Check if a game is associated to any group ****************/
/*****************************************************************************/

static bool Gam_CheckIfGamIsAssociatedToGrps (long GamCod)
  {
   /***** Get if a game is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if a game is associated to groups",
			  "SELECT COUNT(*) FROM gam_grp"
			  " WHERE GamCod=%ld",
			  GamCod) != 0);
  }

/*****************************************************************************/
/**************** Check if a game is associated to a group *****************/
/*****************************************************************************/

bool Gam_CheckIfGamIsAssociatedToGrp (long GamCod,long GrpCod)
  {
   /***** Get if a game is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if a game is associated to a group",
			  "SELECT COUNT(*) FROM gam_grp"
			  " WHERE GamCod=%ld AND GrpCod=%ld",
			  GamCod,GrpCod) != 0);
  }

/*****************************************************************************/
/************************* Remove groups of a game *************************/
/*****************************************************************************/

static void Gam_RemoveAllTheGrpsAssociatedToAndGame (long GamCod)
  {
   /***** Remove groups of the game *****/
   DB_QueryDELETE ("can not remove the groups associated to a game",
		   "DELETE FROM gam_grp WHERE GamCod=%ld",
		   GamCod);
  }

/*****************************************************************************/
/******************* Remove one group from all the games *******************/
/*****************************************************************************/

void Gam_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the games *****/
   DB_QueryDELETE ("can not remove group"
	           " from the associations between games and groups",
		   "DELETE FROM gam_grp WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/*************** Remove groups of one type from all the games **************/
/*****************************************************************************/

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
/************************ Create groups of a game **************************/
/*****************************************************************************/

static void Gam_CreateGrps (long GamCod)
  {
   unsigned NumGrpSel;

   /***** Create groups of the game *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to a game",
		      "INSERT INTO gam_grp"
		      " (GamCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      GamCod,Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/************ Get and write the names of the groups of a game **************/
/*****************************************************************************/

static void Gam_GetAndWriteNamesOfGrpsAssociatedToGame (struct Game *Game)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to a game from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get groups of a game",
			     "SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
			     " FROM gam_grp,crs_grp,crs_grp_types"
			     " WHERE gam_grp.GamCod=%ld"
			     " AND gam_grp.GrpCod=crs_grp.GrpCod"
			     " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
			     " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
			     Game->GamCod);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: ",
            Game->Status.Visible ? "ASG_GRP" :
        	                  "ASG_GRP_LIGHT",
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
/************ Remove all the games of a place on the hierarchy *************/
/************ (country, institution, centre, degree or course)   *************/
/*****************************************************************************/

void Gam_RemoveGames (Hie_Level_t Scope,long Cod)
  {
   /***** Remove all the users in course games *****/
   DB_QueryDELETE ("can not remove users who had answered games"
		   " in a place on the hierarchy",
		   "DELETE FROM gam_users"
		   " USING games,gam_users"
		   " WHERE games.Scope='%s' AND games.Cod=%ld"
		   " AND games.GamCod=gam_users.GamCod",
                   Sco_GetDBStrFromScope (Scope),Cod);

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
/************ Check if I belong to any of the groups of a game *************/
/*****************************************************************************/

static bool Gam_CheckIfICanDoThisGameBasedOnGrps (long GamCod)
  {
   /***** Get if I can do a game from database *****/
   return (DB_QueryCOUNT ("can not check if I can do a game",
			  "SELECT COUNT(*) FROM games"
			  " WHERE GamCod=%ld"
			  " AND (GamCod NOT IN (SELECT GamCod FROM gam_grp) OR"
			  " GamCod IN (SELECT gam_grp.GamCod FROM gam_grp,crs_grp_usr"
			  " WHERE crs_grp_usr.UsrCod=%ld"
			  " AND gam_grp.GrpCod=crs_grp_usr.GrpCod))",
			  GamCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
  }

/*****************************************************************************/
/******************* Get number of questions of a game *********************/
/*****************************************************************************/

static unsigned Gam_GetNumQstsGame (long GamCod)
  {
   /***** Get data of questions from database *****/
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
   Gam_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Show form to create a new question in this game *****/
   Tst_ShowFormAskSelectTstsForGame (Game.GamCod);

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
                    false);
  }

/*****************************************************************************/
/****************** Write parameter with code of question ********************/
/*****************************************************************************/

static void Gam_PutParamQstCod (long QstCod)
  {
   Par_PutHiddenParamLong ("QstCod",QstCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of question *********************/
/*****************************************************************************/

static long Gam_GetParamQstCod (void)
  {
   return Par_GetParToLong ("QstCod");
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
   long LongInt;

   LongInt = Par_GetParToLong ("QstInd");
   if (LongInt < 0)
      Lay_ShowErrorAndExit ("Wrong question index.");

   return (unsigned) LongInt;
  }

/*****************************************************************************/
/********************* Remove answers of a game question *******************/
/*****************************************************************************/

static void Gam_RemAnswersOfAQuestion (long QstCod)
  {
   /***** Remove answers *****/
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM gam_answers WHERE QstCod=%ld",
		   QstCod);
  }

/*****************************************************************************/
/******************** Get next question index in a game **********************/
/*****************************************************************************/
// TODO: Remove this function because a question code can be repeated

static int Gam_GetQstIndFromQstCod (long GamCod,long QstCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   int QstInd = -1;

   /***** Get number of games with a field value from database *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get question index",
			"SELECT QstInd FROM gam_questions"
			" WHERE GamCod=%ld AND QstCod=%ld",
			GamCod,QstCod))
      Lay_ShowErrorAndExit ("Error when getting question index.");

   /***** Get question index (row[0]) *****/
   row = mysql_fetch_row (mysql_res);
   if (row[0])
      if (sscanf (row[0],"%d",&QstInd) != 1)
	 Lay_ShowErrorAndExit ("Error when getting question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
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
// Question index can be 0, 1, 2,...
// Return -1 if no questions

static int Gam_GetMaxQuestionIndexInGame (long GamCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   int QstInd = -1;

   /***** Get maximum question index in a game from database *****/
   DB_QuerySELECT (&mysql_res,"can not get last question index",
		   "SELECT MAX(QstInd) FROM gam_questions WHERE GamCod=%ld",
                   GamCod);
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
      if (sscanf (row[0],"%d",&QstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting last question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/*********** Get previous question index to a given index in a game **********/
/*****************************************************************************/
// Question index can be 0, 1, 2,...
// Return -1 if no previous question

static int Gam_GetPrevQuestionIndexInGame (long GamCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   int PrevQstInd = -1;

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
      if (sscanf (row[0],"%d",&PrevQstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting previous question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PrevQstInd;
  }

/*****************************************************************************/
/************* Get next question index to a given index in a game ************/
/*****************************************************************************/
// Question index can be 0, 1, 2,...
// Return -1 if no next question

static int Gam_GetNextQuestionIndexInGame (long GamCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   int NextQstInd = -1;

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
      if (sscanf (row[0],"%d",&NextQstInd) != 1)
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
   if (Game->Status.ICanAnswer && !Editing)
      ActionToDoWithQuestions = Tst_SHOW_GAME_TO_ANSWER;
   else
      ActionToDoWithQuestions = Tst_SHOW_GAME_RESULT;

   /***** Get data of questions from database *****/
   NumQsts = (unsigned) DB_QuerySELECT (&mysql_res,"can not get data of a question",
				        "SELECT tst_questions.QstCod,"		// row[0]
					       "tst_questions.AnsType,"		// row[1]
					       "tst_questions.Stem,"		// row[2]
					       "tst_questions.Feedback,"	// row[3]
					       "tst_questions.MedCod"		// row[4]
					" FROM gam_questions,tst_questions"
					" WHERE gam_questions.GamCod=%ld"
					" AND gam_questions.QstCod=tst_questions.QstCod"
					" ORDER BY gam_questions.QstInd",
					Game->GamCod);

   /***** Start box *****/
   Gbl.Games.CurrentGamCod = Game->GamCod;
   Box_StartBox (NULL,Txt_Questions,Game->Status.ICanEdit ? Gam_PutIconToAddNewQuestions :
                                                            NULL,
                 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

   if (NumQsts)
     {
      /***** Show the table with the questions *****/
      Gam_ListOneOrMoreQuestionsForEdition (Game,NumQsts,mysql_res);

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

static void Gam_ListOneOrMoreQuestionsForEdition (struct Game *Game,
                                                  unsigned NumQsts,
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
   unsigned UniqueId;
   long QstCod;
   char StrNumQst[10 + 1];

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
   for (NumQst = 0, UniqueId = 1;
	NumQst < NumQsts;
	NumQst++, UniqueId++)
     {
      Gbl.RowEvenOdd = NumQst % 2;

      snprintf (StrNumQst,sizeof (StrNumQst),
	        "%u",
		NumQst + 1);

      row = mysql_fetch_row (mysql_res);
      /*
      row[0] QstCod
      row[1] AnsType
      row[2] Stem
      row[3] Feedback
      row[4] MedCod
      */
      /***** Create test question *****/
      Tst_QstConstructor ();

      /* row[0] holds the code of the question */
      if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
         Lay_ShowErrorAndExit ("Wrong code of question.");

      /***** Icons *****/
      Gbl.Games.CurrentGamCod = Game->GamCod;
      Gbl.Games.CurrentQstCod = QstCod;
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"BT%u\">",Gbl.RowEvenOdd);

      /* Put icon to remove the question */
      Frm_StartForm (ActReqRemGamQst);
      Gam_PutParamGameCod (Game->GamCod);
      Gam_PutParamQstCod (QstCod);
      Ico_PutIconRemove ();
      Frm_EndForm ();

      /* Put icon to move up the question */
      if (NumQst)
	{
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Move_up_X,
		   StrNumQst);
	 Lay_PutContextualLinkOnlyIcon (ActUp_GamQst,NULL,Gam_PutParamsOneQst,
				        "arrow-up.svg",
					Gbl.Title);
	}
      else
         Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);

      /* Put icon to move down the question */
      if (NumQst + 1 < NumQsts)
	{
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Move_down_X,
		   StrNumQst);
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
	       StrNumQst);

      /* Write answer type (row[1]) */
      Gbl.Test.AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);
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

      /* Write stem (row[2]) */
      fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
	       Gbl.RowEvenOdd);
      Tst_WriteQstStem (row[2],"TEST_EDI");

      /* Get media (row[4]) */
      Gbl.Test.Media.MedCod = Str_ConvertStrCodToLongCod (row[4]);
      Med_GetMediaDataByCod (&Gbl.Test.Media);

      /* Show media */
      Med_ShowMedia (&Gbl.Test.Media,
                     "TEST_MED_EDIT_LIST_STEM_CONTAINER",
                     "TEST_MED_EDIT_LIST_STEM");

      /* Show feedback (row[3]) */
      Tst_WriteQstFeedback (row[3],"TEST_EDI_LIGHT");

      /* Show answers */
      Tst_WriteAnswersGameResult (Game,NumQst,QstCod,
                                  "TEST_EDI",true);	// Show result

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
   int MaxQstInd;

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
	              Game.GamCod,QstCod,(unsigned) (MaxQstInd + 1));
     }

   /***** Free space for selected question codes *****/
   Gam_FreeListsSelectedQuestions ();

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
                    false);
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

void Gam_GetAndDrawBarNumUsrsWhoAnswered (struct Game *Game,long QstCod,unsigned AnsInd)
  {
   unsigned NumUsrsThisAnswer;

   /***** Get number of users who selected this answer *****/
   NumUsrsThisAnswer = Gam_GetNumUsrsWhoAnswered (Game->GamCod,QstCod,AnsInd);

   /***** Show stats of this answer *****/
   if (Game->Status.ICanViewResults)
      Gam_DrawBarNumUsrs (NumUsrsThisAnswer,Game->NumUsrs);
  }

/*****************************************************************************/
/**** Get number of users who selected a given answer of a game question *****/
/*****************************************************************************/

static unsigned Gam_GetNumUsrsWhoAnswered (long GamCod,long QstCod,unsigned AnsInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs = 0;	// Default returned value

   /***** Get answers of a question from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get number of users who answered",
		       "SELECT NumUsrs FROM gam_answers"
		       " WHERE GamCod=%ld AND QstCod=%ld AND AnsInd=%u",
		       GamCod,QstCod,AnsInd))
     {
      row = mysql_fetch_row (mysql_res);
      if (row[0])	// There are users who selected this answer
	 if (sscanf (row[0],"%u",&NumUsrs) != 1)
	    Lay_ShowErrorAndExit ("Error when getting number of users who answered.");
     }

   return NumUsrs;
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
   fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\" style=\"width:%upx;\">",
            Gam_MAX_BAR_WIDTH + 125);
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
   fprintf (Gbl.F.Out,"%s</td>",Gbl.Title);
  }

/*****************************************************************************/
/********************* Put icon to remove one question ***********************/
/*****************************************************************************/
/*
static void Gam_PutIconToRemoveOneQst (void)
  {
   Ico_PutContextualIconToRemove (ActReqRemGamQst,Gam_PutParamsRemoveOneQst);
  }
*/
/*****************************************************************************/
/**************** Put parameter to move/remove one question ******************/
/*****************************************************************************/

static void Gam_PutParamsOneQst (void)
  {
   Gam_PutParamGameCod (Gbl.Games.CurrentGamCod);
   Gam_PutParamQstCod (Gbl.Games.CurrentQstCod);
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Gam_RequestRemoveQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   struct Game Game;
   long QstCod;
   unsigned QstInd;

   /***** Get parameters from form *****/
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question code */
   if ((QstCod = Gam_GetParamQstCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   QstInd = (unsigned) Gam_GetQstIndFromQstCod (Game.GamCod,QstCod);	// TODO: Remove this function because a question code can be repeated

   /***** Show question and button to remove question *****/
   Gbl.Games.CurrentGamCod = Game.GamCod;
   Gbl.Games.CurrentQstCod = QstCod;
   Ale_ShowAlertAndButton (ActRemGamQst,NULL,NULL,Gam_PutParamsOneQst,
			   Btn_REMOVE_BUTTON,Txt_Remove_question,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_question_X,
	                   (unsigned long) (QstInd + 1));

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
                    false);
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Gam_RemoveQst (void)
  {
   extern const char *Txt_Question_removed;
   struct Game Game;
   long QstCod;
   unsigned QstInd;

   /***** Get parameters from form *****/
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question code */
   if ((QstCod = Gam_GetParamQstCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   QstInd = (unsigned) Gam_GetQstIndFromQstCod (Game.GamCod,QstCod);	// TODO: Remove this function because a question code can be repeated

   /***** Remove the question from all the tables *****/
   /* Remove answers from this test question */
   Gam_RemAnswersOfAQuestion (QstCod);

   /* Remove the question itself */
   DB_QueryDELETE ("can not remove a question",
		   "DELETE FROM gam_questions WHERE QstCod=%ld",
		   QstCod);
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The question to be removed does not exist.");

   /* Change index of questions greater than this */
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
                    false);
  }

/*****************************************************************************/
/***************** Move up position of a question in a game ******************/
/*****************************************************************************/

void Gam_MoveUpQst (void)
  {
   extern const char *Txt_The_question_has_been_moved_up;
   struct Game Game;
   long QstCod;
   int QstIndTop;
   int QstIndBottom;

   /***** Get parameters from form *****/
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question code */
   if ((QstCod = Gam_GetParamQstCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   QstIndBottom = Gam_GetQstIndFromQstCod (Game.GamCod,QstCod);	// TODO: Remove this function because a question code can be repeated

   /***** Move up question *****/
   if (QstIndBottom > 0)
     {
      /* Indexes of questions to be exchanged */
      QstIndTop = Gam_GetPrevQuestionIndexInGame (Game.GamCod,QstIndBottom);
      if (QstIndTop < 0)
         Lay_ShowErrorAndExit ("Wrong index of question.");

      /* Exchange questions */
      Gam_ExchangeQuestions (Game.GamCod,
			     (unsigned) QstIndTop,(unsigned) QstIndBottom);

      /* Success alert */
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_question_has_been_moved_up);
     }

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
                    false);
  }

/*****************************************************************************/
/**************** Move down position of a question in a game *****************/
/*****************************************************************************/

void Gam_MoveDownQst (void)
  {
   extern const char *Txt_The_question_has_been_moved_down;
   struct Game Game;
   long QstCod;
   int QstIndTop;
   int QstIndBottom;
   int MaxQstInd;	// -1 if no questions

   /***** Get parameters from form *****/
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question code */
   if ((QstCod = Gam_GetParamQstCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   QstIndTop = Gam_GetQstIndFromQstCod (Game.GamCod,QstCod);	// TODO: Remove this function because a question code can be repeated

   /* Get maximum question index */
   MaxQstInd = Gam_GetMaxQuestionIndexInGame (Game.GamCod);

   /***** Move down question *****/
   if (MaxQstInd > 0)
      if (QstIndTop < MaxQstInd)
	{
	 /* Indexes of questions to be exchanged */
         QstIndBottom = Gam_GetNextQuestionIndexInGame (Game.GamCod,QstIndTop);
	 if (QstIndBottom < 0)
	    Lay_ShowErrorAndExit ("Wrong index of question.");

	 /* Exchange questions */
	 Gam_ExchangeQuestions (Game.GamCod,
	                        (unsigned) QstIndTop,(unsigned) QstIndBottom);

         /* Success alert */
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_question_has_been_moved_down);
	}

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
                    false);
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
   QstIndTop    = 0; QstCodTop    = 218
   QstIndBottom = 1; QstCodBottom = 220
   +--------+--------+		+--------+--------+	+--------+--------+
   | QstInd | QstCod |		| QstInd | QstCod |	| QstInd | QstCod |
   +--------+--------+		+--------+--------+	+--------+--------+
   |      0 |    218 |  ----->	|      1 |    218 |  =	|      0 |    220 |
   |      1 |    220 |		|      0 |    220 |	|      1 |    218 |
   |      2 |    232 |		|      2 |    232 |	|      2 |    232 |
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
/************************* Start playing a game ******************************/
/*****************************************************************************/

void Gam_PlayGame (void)
  {
   struct Game Game;

   /***** Get parameters *****/
   Gam_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_GAMES);

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Show game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    false,
                    true);	// Put button to start
  }

/*****************************************************************************/
/********************* Put a big button to start game ************************/
/*****************************************************************************/

static void Gam_PutBigButtonToStartGame (long GamCod)
  {
   extern const char *Txt_Play;

   /***** Start form *****/
   Frm_StartForm (ActPlyGam1stQst);
   Gam_PutParamGameCod (GamCod);
   Gam_PutParamQstInd (0);	// Start by first question in game

   /***** Put icon with link *****/
   Frm_LinkFormSubmit (Txt_Play,NULL,NULL);
   fprintf (Gbl.F.Out,"<img src=\"%s/play.svg\""
		      " alt=\"%s\" title=\"%s\""
	              " class=\"CONTEXT_OPT ICO_HIGHLIGHT ICO64x64\" />",
            Cfg_URL_ICON_PUBLIC,Txt_Play,Txt_Play);
   fprintf (Gbl.F.Out,"</a>");

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Show next question when playing a game *********************/
/*****************************************************************************/

void Gam_PlayGameNextQuestion (void)
  {
   Gam_PlayGameShowQuestionAndAnswers (false);	// Don't show answers
  }

/*****************************************************************************/
/************ Show question and its answers when playing a game **************/
/*****************************************************************************/

void Gam_PlayGameShowAnswers (void)
  {
   Gam_PlayGameShowQuestionAndAnswers (true);	// Show answers
  }

/*****************************************************************************/
/************ Show question and its answers when playing a game **************/
/*****************************************************************************/

static void Gam_PlayGameShowQuestionAndAnswers (bool ShowAnswers)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Game Game;
   unsigned QstInd;
   int NxtQstInd;
   long QstCod;

   /***** Get parameters *****/
   /* Get game code */
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question index */
   QstInd = Gam_GetParamQstInd ();

   /***** Get data of question from database *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get data of a question",
			"SELECT tst_questions.QstCod,"		// row[0]
			       "tst_questions.AnsType,"		// row[1]
			       "tst_questions.Stem,"		// row[2]
			       "tst_questions.MedCod"		// row[3]
			" FROM gam_questions,tst_questions"
			" WHERE gam_questions.GamCod=%ld"
			" AND gam_questions.QstInd=%u"
			" AND gam_questions.QstCod=tst_questions.QstCod",
			Game.GamCod,QstInd))
      Ale_ShowAlert (Ale_ERROR,"Question doesn't exist.");
   row = mysql_fetch_row (mysql_res);

   /***** Show question *****/
   /* Start container for number and question */
   fprintf (Gbl.F.Out,"<div class=\"GAM_PLAY_CONTAINER\">");

   /* Write number of question */
   fprintf (Gbl.F.Out,"<div class=\"GAM_PLAY_NUM_QST\">%u</div>",
	    QstInd + 1);

   fprintf (Gbl.F.Out,"<div class=\"GAM_PLAY_QST_CONTAINER\">");

   /* Write stem (row[2]) */
   Tst_WriteQstStem (row[2],"GAM_PLAY_QST");

   /* Get media (row[3]) */
   Gbl.Test.Media.MedCod = Str_ConvertStrCodToLongCod (row[3]);
   Med_GetMediaDataByCod (&Gbl.Test.Media);

   /* Show media */
   Med_ShowMedia (&Gbl.Test.Media,
		  "TEST_MED_EDIT_LIST_STEM_CONTAINER",
		  "TEST_MED_EDIT_LIST_STEM");

   /* Write answers? */
   if (ShowAnswers)
     {
      /* Get question code (row[0]) */
      if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	 Lay_ShowErrorAndExit ("Error: wrong question code.");

      /* Get answer type (row[1]) */
      Gbl.Test.AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[1]);

      /* Write answers */
      Tst_WriteAnswersGameResult (&Game,QstInd,QstCod,
                                  "GAM_PLAY_QST",false);	// Don't show result
     }
   else
      fprintf (Gbl.F.Out,"&nbsp;");

   fprintf (Gbl.F.Out,"</div>");

   /***** Put button to continue *****/
   fprintf (Gbl.F.Out,"<div class=\"GAM_PLAY_NXT_CONTAINER\">");
   if (ShowAnswers)
     {
      /* Get index of the next question */
      NxtQstInd = Gam_GetNextQuestionIndexInGame (Game.GamCod,QstInd);
      if (NxtQstInd > 0)
	 /* Put button to show next question */
	 Gam_PutBigButtonToContinue (ActPlyGamNxtQst,Game.GamCod,(unsigned) NxtQstInd);
     }
   else
      /* Put button to show answers */
      Gam_PutBigButtonToContinue (ActPlyGamAns,Game.GamCod,QstInd);
   fprintf (Gbl.F.Out,"</div>");

   /***** End container for question *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*********************** Put a big button to continue ************************/
/*****************************************************************************/

static void Gam_PutBigButtonToContinue (Act_Action_t NextAction,
                                        long GamCod,unsigned QstInd)
  {
   extern const char *Txt_Continue;

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"GAM_PLAY_CONTINUE_CONTAINER\">");

   /***** Start form *****/
   Frm_StartForm (NextAction);
   Gam_PutParamGameCod (GamCod);
   Gam_PutParamQstInd (QstInd);

   /***** Put icon with link *****/
   Frm_LinkFormSubmit (Txt_Continue,"GAM_PLAY_CONTINUE ICO_HIGHLIGHT",NULL);
   fprintf (Gbl.F.Out,"<img src=\"%s/step-forward.svg\""
	              " alt=\"%s\" title=\"%s\" class=\"ICO64x64\" />"
	              "<br />"
	              "%s",
            Cfg_URL_ICON_PUBLIC,Txt_Continue,Txt_Continue,
            Txt_Continue);
   fprintf (Gbl.F.Out,"</a>");

   /***** End form *****/
   Frm_EndForm ();

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************ Receive answers of a game ************************/
/*****************************************************************************/

void Gam_ReceiveGameAnswers (void)
  {
   extern const char *Txt_You_already_played_this_game_before;
   extern const char *Txt_Thanks_for_playing_the_game;
   struct Game Game;

   /***** Get game code *****/
   if ((Game.GamCod = Gam_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Gam_GetDataOfGameByCod (&Game);

   /***** Check if I have no answered this game formerly *****/
   if (Game.Status.IHaveAnswered)
      Ale_ShowAlert (Ale_WARNING,Txt_You_already_played_this_game_before);
   else
     {
      /***** Receive and store user's answers *****/
      Gam_ReceiveAndStoreUserAnswersToAGame (Game.GamCod);
      Ale_ShowAlert (Ale_INFO,Txt_Thanks_for_playing_the_game);
     }

   /***** Show current game *****/
   Gam_ShowOneGame (Game.GamCod,
                    true,	// Show only this game
                    true,	// List game questions
                    false);
  }

/*****************************************************************************/
/**************** Get and store user's answers to a game *******************/
/*****************************************************************************/

static void Gam_ReceiveAndStoreUserAnswersToAGame (long GamCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   unsigned NumQsts;
   long QstCod;
   char ParamName[3 + 10 + 6 + 1];
   char StrAnswersIndexes[Tst_MAX_OPTIONS_PER_QUESTION * (10 + 1)];
   const char *Ptr;
   char UnsignedStr[10 + 1];
   unsigned AnsInd;

   /***** Get questions of this game from database *****/
   NumQsts = (unsigned) DB_QuerySELECT (&mysql_res,"can not get questions of a game",
					"SELECT QstCod FROM gam_questions"
					" WHERE GamCod=%ld ORDER BY QstCod",
					GamCod);
   if (NumQsts)	// The game has questions
     {
      /***** Get questions *****/
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
        {
         /* Get next answer */
         row = mysql_fetch_row (mysql_res);

         /* Get question code (row[0]) */
         if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Lay_ShowErrorAndExit ("Error: wrong question code.");

         /* Get possible parameter with the user's answer */
         snprintf (ParamName,sizeof (ParamName),
                   "Ans%010u",
		   (unsigned) QstCod);
         // Lay_ShowAlert (Lay_INFO,ParamName);
         Par_GetParMultiToText (ParamName,StrAnswersIndexes,
                                Gam_MAX_ANSWERS_PER_QUESTION * (10 + 1));
         Ptr = StrAnswersIndexes;
         while (*Ptr)
           {
            Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,10);
            if (sscanf (UnsignedStr,"%u",&AnsInd) == 1)
               // Parameter exists, so user has marked this answer, so store it in database
               Gam_IncreaseAnswerInDB (QstCod,AnsInd);
           }
        }
     }
   else		// The game has no questions and answers
      Lay_ShowErrorAndExit ("Error: this game has no questions.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Register that you have answered this game *****/
   Gam_RegisterIHaveAnsweredGame (GamCod);
  }

/*****************************************************************************/
/************ Increase number of users who have marked one answer ************/
/*****************************************************************************/

static void Gam_IncreaseAnswerInDB (long QstCod,unsigned AnsInd)
  {
   /***** Increase number of users who have selected
          the answer AnsInd in the question QstCod *****/
   DB_QueryUPDATE ("can not register your answer to the game",
		   "UPDATE gam_answers SET NumUsrs=NumUsrs+1"
		   " WHERE QstCod=%ld AND AnsInd=%u",
                   QstCod,AnsInd);
  }

/*****************************************************************************/
/******************* Register that I have answered a game ********************/
/*****************************************************************************/

static void Gam_RegisterIHaveAnsweredGame (long GamCod)
  {
   DB_QueryINSERT ("can not register that you have answered the game",
		   "INSERT INTO gam_users"
		   " (GamCod,UsrCod)"
		   " VALUES"
		   " (%ld,%ld)",
                   GamCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Check if I have answered a game ************************/
/*****************************************************************************/

static bool Gam_CheckIfIHaveAnsweredGame (long GamCod)
  {
   /***** Get number of games with a field value from database *****/
   return (DB_QueryCOUNT ("can not check if you have answered a game",
			  "SELECT COUNT(*) FROM gam_users"
			  " WHERE GamCod=%ld AND UsrCod=%ld",
			  GamCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
  }

/*****************************************************************************/
/************** Get number of users who have answered a game *****************/
/*****************************************************************************/

static unsigned Gam_GetNumUsrsWhoHaveAnsweredGame (long GamCod)
  {
   /***** Get number of games with a field value from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of users"
			     " who have answered a game",
			     "SELECT COUNT(*) FROM gam_users"
			     " WHERE GamCod=%ld",
			     GamCod);
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
