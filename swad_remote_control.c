// swad_remote_control.c: remote control

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_remote_control.h"
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

#define Rmt_MAX_CHARS_ANSWER	(1024 - 1)	// 1023
#define Rmt_MAX_BYTES_ANSWER	((Rmt_MAX_CHARS_ANSWER + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 16383

#define Rmt_MAX_BYTES_LIST_ANSWER_TYPES	(10 + (Rmt_NUM_ANS_TYPES - 1) * (1 + 10))

const char *Rmt_StrAnswerTypesDB[Rmt_NUM_ANS_TYPES] =
  {
   "unique_choice",
   "multiple_choice",
  };

#define Rmt_MAX_ANSWERS_PER_QUESTION	10

struct GameQuestion
  {
   long QstCod;
   unsigned QstInd;
   Rmt_AnswerType_t AnswerType;
   struct
     {
      char *Text;
     } AnsChoice[Rmt_MAX_ANSWERS_PER_QUESTION];
   bool AllAnsTypes;
   char ListAnsTypes[Rmt_MAX_BYTES_LIST_ANSWER_TYPES + 1];
  };

#define Rmt_MAX_SELECTED_QUESTIONS		1000
#define Rmt_MAX_BYTES_LIST_SELECTED_QUESTIONS	(Rmt_MAX_SELECTED_QUESTIONS * (1 + 10 + 1))

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Rmt_ListAllGames (struct GameQuestion *GameQst);
static bool Rmt_CheckIfICanCreateGame (void);
static void Rmt_PutIconsListGames (void);
static void Rmt_PutIconToCreateNewGame (void);
static void Rmt_PutButtonToCreateNewGame (void);
static void Rmt_PutParamsToCreateNewGame (void);
static void Rmt_PutFormToSelectWhichGroupsToShow (void);
static void Rmt_ParamsWhichGroupsToShow (void);
static void Rmt_ShowOneGame (long GamCod,struct GameQuestion *GameQst,
                               bool ShowOnlyThisGameComplete);
static void Rmt_WriteAuthor (struct Game *Game);
static void Rmt_WriteStatus (struct Game *Game);
static void Rmt_GetParamGameOrder (void);

static void Rmt_PutFormsToRemEditOneGame (long GamCod,bool Visible);
static void Rmt_PutParams (void);

static void Rmt_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed);

static void Rmt_GetGameTxtFromDB (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);

static void Rmt_PutButtonToResetGame (void);

static bool Rmt_CheckIfSimilarGameExists (struct Game *Game);
static void Rmt_SetDefaultAndAllowedScope (struct Game *Game);
static void Rmt_ShowLstGrpsToEditGame (long GamCod);
static void Rmt_UpdateNumUsrsNotifiedByEMailAboutGame (long GamCod,
                                                       unsigned NumUsrsToBeNotifiedByEMail);
static void Rmt_CreateGame (struct Game *Game,const char *Txt);
static void Rmt_UpdateGame (struct Game *Game,const char *Txt);
static bool Rmt_CheckIfGamIsAssociatedToGrps (long GamCod);
static void Rmt_RemoveAllTheGrpsAssociatedToAndGame (long GamCod);
static void Rmt_CreateGrps (long GamCod);
static void Rmt_GetAndWriteNamesOfGrpsAssociatedToGame (struct Game *Game);
static bool Rmt_CheckIfICanDoThisGameBasedOnGrps (long GamCod);

static unsigned Rmt_GetNumQstsGame (long GamCod);
static void Rmt_InitQst (struct GameQuestion *GameQst);
static void Rmt_PutParamQstCod (long QstCod);
static long Rmt_GetParamQstCod (void);
static void Rmt_RemAnswersOfAQuestion (long QstCod);
static Rmt_AnswerType_t Rmt_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeBD);
// static bool Rmt_CheckIfAnswerExists (long QstCod,unsigned AnsInd);
// static bool Rmt_AllocateTextChoiceAnswer (struct GameQuestion *GameQst,unsigned NumAns);
// static void Rmt_FreeTextChoiceAnswers (struct GameQuestion *GameQst,unsigned NumAnswers);
// static void Rmt_FreeTextChoiceAnswer (struct GameQuestion *GameQst,unsigned NumAns);

static unsigned Rmt_GetQstIndFromQstCod (long QstCod);
static unsigned Rmt_GetNextQuestionIndexInGame (long GamCod);
static void Rmt_ListGameQuestions (struct Game *Game,struct GameQuestion *GameQst);
static void Rmt_PutIconToAddNewQuestions (void);
static void Rmt_PutButtonToAddNewQuestions (void);

static void Rmt_AllocateListSelectedQuestions (void);
static void Rmt_FreeListsSelectedQuestions (void);
static unsigned Rmt_CountNumQuestionsInList (void);

// static void Rmt_WriteQstStem (const char *Stem);
static void Rmt_WriteAnswersOfAQst (Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions,
                                    struct Game *Game,struct GameQuestion *GameQst);
static void Rmt_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs);

// static void Rmt_PutIconToRemoveOneQst (void);
static void Rmt_PutParamsRemoveOneQst (void);

static void Rmt_ReceiveAndStoreUserAnswersToAGame (long GamCod);
static void Rmt_IncreaseAnswerInDB (long QstCod,unsigned AnsInd);
static void Rmt_RegisterIHaveAnsweredGame (long GamCod);
static bool Rmt_CheckIfIHaveAnsweredGame (long GamCod);
static unsigned Rmt_GetNumUsrsWhoHaveAnsweredGame (long GamCod);

/*****************************************************************************/
/************************** List all the games *****************************/
/*****************************************************************************/

void Rmt_SeeAllGames (void)
  {
   struct GameQuestion GameQst;

   /***** Get parameters *****/
   Rmt_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Show all the games *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/*************************** Show all the games ****************************/
/*****************************************************************************/

static void Rmt_ListAllGames (struct GameQuestion *GameQst)
  {
   extern const char *Hlp_ASSESSMENT_Games;
   extern const char *Txt_Games;
   extern const char *Txt_START_END_TIME_HELP[Dat_NUM_START_END_TIME];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Game;
   extern const char *Txt_Status;
   extern const char *Txt_No_games;
   Rmt_Order_t Order;
   struct Pagination Pagination;
   unsigned NumGame;

   /***** Get number of groups in current course *****/
   if (!Gbl.CurrentCrs.Grps.NumGrps)
      Gbl.CurrentCrs.Grps.WhichGrps = Grp_ALL_GROUPS;

   /***** Get list of games *****/
   Rmt_GetListGames ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Games.Num;
   Pagination.CurrentPage = (int) Gbl.Games.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Games.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_SURVEYS,
                                     0,
                                     &Pagination);

   /***** Start box *****/
   Box_StartBox ("100%",Txt_Games,Rmt_PutIconsListGames,
                 Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.CurrentCrs.Grps.NumGrps)
      Rmt_PutFormToSelectWhichGroupsToShow ();

   if (Gbl.Games.Num)
     {
      /***** Table head *****/
      Tbl_StartTableWideMargin (2);
      fprintf (Gbl.F.Out,"<tr>"
			 "<th class=\"CONTEXT_COL\"></th>");	// Column for contextual icons
      for (Order = Rmt_ORDER_BY_START_DATE;
	   Order <= Rmt_ORDER_BY_END_DATE;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");

	 /* Form to change order */
	 Act_FormStart (ActSeeAllGam);
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_SURVEYS,Gbl.Games.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_START_END_TIME_HELP[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Games.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_START_END_TIME[Order]);
	 if (Order == Gbl.Games.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();

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
	 Rmt_ShowOneGame (Gbl.Games.LstGamCods[NumGame - 1],GameQst,false);

      /***** End table *****/
      Tbl_EndTable ();
     }
   else	// No games created
      Ale_ShowAlert (Ale_INFO,Txt_No_games);

   /***** Button to create a new game *****/
   if (Rmt_CheckIfICanCreateGame ())
      Rmt_PutButtonToCreateNewGame ();

   /***** End box *****/
   Box_EndBox ();

   /***** Write again links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_SURVEYS,
                                     0,
                                     &Pagination);

   /***** Free list of games *****/
   Rmt_FreeListGames ();
  }

/*****************************************************************************/
/******************* Check if I can create a new game **********************/
/*****************************************************************************/

static bool Rmt_CheckIfICanCreateGame (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
         return (Gbl.CurrentCrs.Crs.CrsCod > 0);
      case Rol_DEG_ADM:
         return (Gbl.CurrentDeg.Deg.DegCod > 0);	// Always true
      case Rol_CTR_ADM:
         return (Gbl.CurrentCtr.Ctr.CtrCod > 0);	// Always true
      case Rol_INS_ADM:
         return (Gbl.CurrentIns.Ins.InsCod > 0);	// Always true
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

static void Rmt_PutIconsListGames (void)
  {
   /***** Put icon to create a new game *****/
   if (Rmt_CheckIfICanCreateGame ())
      Rmt_PutIconToCreateNewGame ();

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_SURVEYS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************** Put icon to create a new game **********************/
/*****************************************************************************/

static void Rmt_PutIconToCreateNewGame (void)
  {
   extern const char *Txt_New_game;

   Lay_PutContextualLink (ActFrmNewGam,NULL,Rmt_PutParamsToCreateNewGame,
                          "plus64x64.png",
                          Txt_New_game,NULL,
		          NULL);
  }

/*****************************************************************************/
/********************* Put button to create a new game *********************/
/*****************************************************************************/

static void Rmt_PutButtonToCreateNewGame (void)
  {
   extern const char *Txt_New_game;

   Act_FormStart (ActFrmNewGam);
   Rmt_PutParamsToCreateNewGame ();
   Btn_PutConfirmButton (Txt_New_game);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************* Put parameters to create a new game *******************/
/*****************************************************************************/

static void Rmt_PutParamsToCreateNewGame (void)
  {
   Rmt_PutHiddenParamGameOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_SURVEYS,Gbl.Games.CurrentPage);
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void Rmt_PutFormToSelectWhichGroupsToShow (void)
  {
   fprintf (Gbl.F.Out,"<div style=\"display:table; margin:0 auto;\">");
   Grp_ShowFormToSelWhichGrps (ActSeeAllGam,Rmt_ParamsWhichGroupsToShow);
   fprintf (Gbl.F.Out,"</div>");
  }

static void Rmt_ParamsWhichGroupsToShow (void)
  {
   Rmt_PutHiddenParamGameOrder ();
   Pag_PutHiddenParamPagNum (Pag_SURVEYS,Gbl.Games.CurrentPage);
  }

/*****************************************************************************/
/****************************** Show one game ******************************/
/*****************************************************************************/

void Rmt_SeeOneGame (void)
  {
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Initialize question to zero *****/
   Rmt_InitQst (&GameQst);

   /***** Get parameters *****/
   Rmt_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Show game *****/
   Rmt_ShowOneGame (Game.GamCod,&GameQst,true);
  }

/*****************************************************************************/
/****************************** Show one game ******************************/
/*****************************************************************************/

static void Rmt_ShowOneGame (long GamCod,struct GameQuestion *GameQst,
                               bool ShowOnlyThisGameComplete)
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
   static unsigned UniqueId = 0;
   struct Game Game;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Start box *****/
   if (ShowOnlyThisGameComplete)
      Box_StartBox (NULL,Txt_Game,NULL,
                    Hlp_ASSESSMENT_Games,Box_NOT_CLOSABLE);

   /***** Get data of this game *****/
   Game.GamCod = GamCod;
   Rmt_GetDataOfGameByCod (&Game);

   /***** Start table *****/
   if (ShowOnlyThisGameComplete)
      Tbl_StartTableWide (2);

   /***** Write first row of data of this assignment *****/
   /* Forms to remove/edit this assignment */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td rowspan=\"2\" class=\"CONTEXT_COL");
   if (!ShowOnlyThisGameComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   if (Game.Status.ICanEdit)
      Rmt_PutFormsToRemEditOneGame (Game.GamCod,Game.Status.Visible);
   fprintf (Gbl.F.Out,"</td>");

   /* Start date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"gam_date_start_%u\" class=\"%s LEFT_TOP",
	    UniqueId,
            Game.Status.Visible ? (Game.Status.Open ? "DATE_GREEN" :
        	                                    "DATE_RED") :
                                 (Game.Status.Open ? "DATE_GREEN_LIGHT" :
                                                    "DATE_RED_LIGHT"));
   if (!ShowOnlyThisGameComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('gam_date_start_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>"
	              "</td>",
            UniqueId,Game.TimeUTC[Rmt_START_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /* End date/time */
   fprintf (Gbl.F.Out,"<td id=\"gam_date_end_%u\" class=\"%s LEFT_TOP",
            UniqueId,
            Game.Status.Visible ? (Game.Status.Open ? "DATE_GREEN" :
        	                                      "DATE_RED") :
                                  (Game.Status.Open ? "DATE_GREEN_LIGHT" :
                                                      "DATE_RED_LIGHT"));
   if (!ShowOnlyThisGameComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('gam_date_end_%u',%ld,"
                      "%u,'<br />','%s',false,true,0x7);"
                      "</script>"
	              "</td>",
            UniqueId,Game.TimeUTC[Rmt_END_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /* Game title */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (!ShowOnlyThisGameComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Put form to view game */
   Act_FormStart (ActSeeOneGam);
   Rmt_PutParamGameCod (GamCod);
   Rmt_PutHiddenParamGameOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_SURVEYS,Gbl.Games.CurrentPage);
   Act_LinkFormSubmit (Txt_View_game,
                       Game.Status.Visible ? "ASG_TITLE" :
	                                    "ASG_TITLE_LIGHT",NULL);
   fprintf (Gbl.F.Out,"%s</a>",
            Game.Title);
   Act_FormEnd ();

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
   if (!ShowOnlyThisGameComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   Rmt_WriteStatus (&Game);

   if (!ShowOnlyThisGameComplete)
     {
      /* Possible button to answer this game */
      if (Game.Status.ICanAnswer)
	{
	 fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");

	 Act_FormStart (ActSeeOneGam);
	 Rmt_PutParamGameCod (Game.GamCod);
	 Rmt_PutHiddenParamGameOrder ();
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_SURVEYS,Gbl.Games.CurrentPage);
	 Btn_PutCreateButtonInline (Txt_Play);
	 Act_FormEnd ();

	 fprintf (Gbl.F.Out,"</div>");
	}
      /* Possible button to see the result of the game */
      else if (Game.Status.ICanViewResults)
	{
	 fprintf (Gbl.F.Out,"<div class=\"BUTTONS_AFTER_ALERT\">");

	 Act_FormStart (ActSeeOneGam);
	 Rmt_PutParamGameCod (Game.GamCod);
	 Rmt_PutHiddenParamGameOrder ();
	 Grp_PutParamWhichGrps ();
	 Pag_PutHiddenParamPagNum (Pag_SURVEYS,Gbl.Games.CurrentPage);
	 Btn_PutConfirmButtonInline (Txt_View_game_results);
	 Act_FormEnd ();

	 fprintf (Gbl.F.Out,"</div>");
	}
     }

   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Write second row of data of this game *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"LEFT_TOP");
   if (!ShowOnlyThisGameComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Author of the game */
   Rmt_WriteAuthor (&Game);

   fprintf (Gbl.F.Out,"</td>"
                      "<td class=\"LEFT_TOP");
   if (!ShowOnlyThisGameComplete)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");

   /* Scope of the game */
   fprintf (Gbl.F.Out,"<div class=\"%s\">%s: ",
            Game.Status.Visible ? "ASG_GRP" :
        	                 "ASG_GRP_LIGHT",
            Txt_Scope);
   switch (Game.Scope)
     {
      case Sco_SCOPE_UNK:	// Unknown
         Lay_ShowErrorAndExit ("Wrong game scope.");
         break;
      case Sco_SCOPE_SYS:	// System
         fprintf (Gbl.F.Out,"%s",
                  Cfg_PLATFORM_SHORT_NAME);
	 break;
      case Sco_SCOPE_CTY:	// Country
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Country,Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
	 break;
      case Sco_SCOPE_INS:	// Institution
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Institution,Gbl.CurrentIns.Ins.ShrtName);
	 break;
      case Sco_SCOPE_CTR:	// Centre
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Centre,Gbl.CurrentCtr.Ctr.ShrtName);
	 break;
      case Sco_SCOPE_DEG:	// Degree
         fprintf (Gbl.F.Out,"%s %s",
                  Txt_Degree,Gbl.CurrentDeg.Deg.ShrtName);
 	 break;
      case Sco_SCOPE_CRS:	// Course
	 fprintf (Gbl.F.Out,"%s %s",
	          Txt_Course,Gbl.CurrentCrs.Crs.ShrtName);
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
   if (Game.Scope == Sco_SCOPE_CRS)
      if (Gbl.CurrentCrs.Grps.NumGrps)
         Rmt_GetAndWriteNamesOfGrpsAssociatedToGame (&Game);

   /* Text of the game */
   Rmt_GetGameTxtFromDB (Game.GamCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to rigorous HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<p class=\"%s\">"
                      "%s"
                      "</p>"
                      "</td>"
                      "</tr>",
            Game.Status.Visible ? "DAT" :
        	                 "DAT_LIGHT",
            Txt);

   /***** Write questions of this game *****/
   if (ShowOnlyThisGameComplete)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"5\">");
      Rmt_ListGameQuestions (&Game,GameQst);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Mark possible notification as seen *****/
   if (Game.Scope == Sco_SCOPE_CRS)	// Only course games are notified
      Ntf_MarkNotifAsSeen (Ntf_EVENT_SURVEY,
	                   GamCod,Game.Cod,
	                   Gbl.Usrs.Me.UsrDat.UsrCod);

   if (ShowOnlyThisGameComplete)
     {
      /***** End table *****/
      Tbl_EndTable ();

      /***** End box *****/
      Box_EndBox ();
     }
  }

/*****************************************************************************/
/*********************** Write the author of a game ************************/
/*****************************************************************************/

static void Rmt_WriteAuthor (struct Game *Game)
  {
   Usr_WriteAuthor1Line (Game->UsrCod,!Game->Status.Visible);
  }

/*****************************************************************************/
/************************ Write status of a game ***************************/
/*****************************************************************************/

static void Rmt_WriteStatus (struct Game *Game)
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
/********* Get parameter with the type or order in list of games ***********/
/*****************************************************************************/

static void Rmt_GetParamGameOrder (void)
  {
   Gbl.Games.SelectedOrder = (Rmt_Order_t)
	                    Par_GetParToUnsignedLong ("Order",
	                                              0,
	                                              Rmt_NUM_ORDERS - 1,
	                                              (unsigned long) Rmt_ORDER_DEFAULT);
  }

/*****************************************************************************/
/***** Put a hidden parameter with the type of order in list of games ******/
/*****************************************************************************/

void Rmt_PutHiddenParamGameOrder (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Games.SelectedOrder);
  }

/*****************************************************************************/
/******************** Put a link (form) to edit one game *********************/
/*****************************************************************************/

static void Rmt_PutFormsToRemEditOneGame (long GamCod,bool Visible)
  {
   extern const char *Txt_Reset;

   Gbl.Games.CurrentGamCod = GamCod;	// Used as parameter in contextual links

   /***** Put form to remove game *****/
   Ico_PutContextualIconToRemove (ActReqRemGam,Rmt_PutParams);

   /***** Put form to reset game *****/
   Lay_PutContextualLink (ActReqRstGam,NULL,Rmt_PutParams,
                          "recycle64x64.png",
                          Txt_Reset,NULL,
		          NULL);

   /***** Put form to hide/show game *****/
   if (Visible)
      Ico_PutContextualIconToHide (ActHidGam,Rmt_PutParams);
   else
      Ico_PutContextualIconToUnhide (ActShoGam,Rmt_PutParams);

   /***** Put form to edit game *****/
   Ico_PutContextualIconToEdit (ActEdiOneGam,Rmt_PutParams);
  }

/*****************************************************************************/
/********************** Params used to edit a game *************************/
/*****************************************************************************/

static void Rmt_PutParams (void)
  {
   if (Gbl.Games.CurrentGamCod > 0)
      Rmt_PutParamGameCod (Gbl.Games.CurrentGamCod);
   Att_PutHiddenParamAttOrder ();
   Grp_PutParamWhichGrps ();
   Pag_PutHiddenParamPagNum (Pag_SURVEYS,Gbl.Games.CurrentPage);
  }

/*****************************************************************************/
/*********************** Get list of all the games *************************/
/*****************************************************************************/

void Rmt_GetListGames (void)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char SubQuery[Sco_NUM_SCOPES][256];
   char OrderBySubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumGame;
   unsigned ScopesAllowed = 0;
   unsigned HiddenAllowed = 0;
   long Cods[Sco_NUM_SCOPES];
   Sco_Scope_t Scope;
   bool SubQueryFilled;

   /***** Free list of games *****/
   if (Gbl.Games.LstIsRead)
      Rmt_FreeListGames ();

   /***** Set allowed and hidden scopes to get list depending on my user's role *****/
   Rmt_SetAllowedAndHiddenScopes (&ScopesAllowed,&HiddenAllowed);

   /***** Get list of games from database *****/
   Cods[Sco_SCOPE_SYS] = -1L;				// System
   Cods[Sco_SCOPE_CTY] = Gbl.CurrentCty.Cty.CtyCod;	// Country
   Cods[Sco_SCOPE_INS] = Gbl.CurrentIns.Ins.InsCod;	// Institution
   Cods[Sco_SCOPE_CTR] = Gbl.CurrentCtr.Ctr.CtrCod;	// Centre
   Cods[Sco_SCOPE_DEG] = Gbl.CurrentDeg.Deg.DegCod;	// Degree
   Cods[Sco_SCOPE_CRS] = Gbl.CurrentCrs.Crs.CrsCod;	// Course

   /* Fill subqueries for system, country, institution, centre and degree */
   for (Scope = Sco_SCOPE_SYS, SubQueryFilled = false;
	Scope <= Sco_SCOPE_DEG;
	Scope++)
      if (ScopesAllowed & 1 << Scope)
	{
	 sprintf (SubQuery[Scope],"%s(Scope='%s' AND Cod=%ld%s)",
	          SubQueryFilled ? " OR " :
	        	           "",
		  Sco_ScopeDB[Scope],Cods[Scope],
		  (HiddenAllowed & 1 << Scope) ? "" :
						 " AND Hidden='N'");
	 SubQueryFilled = true;
	}
      else
	 SubQuery[Scope][0] = '\0';

   /* Fill subquery for course */
   if (ScopesAllowed & 1 << Sco_SCOPE_CRS)
     {
      if (Gbl.CurrentCrs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
	 sprintf (SubQuery[Sco_SCOPE_CRS],"%s("
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
		  Sco_ScopeDB[Sco_SCOPE_CRS],Cods[Sco_SCOPE_CRS],
		  (HiddenAllowed & 1 << Sco_SCOPE_CRS) ? "" :
						         " AND Hidden='N'",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
      else	// Gbl.CurrentCrs.Grps.WhichGrps == Grp_ALL_GROUPS
	 sprintf (SubQuery[Sco_SCOPE_CRS],"%s(Scope='%s' AND Cod=%ld%s)",
	          SubQueryFilled ? " OR " :
	        	           "",
		  Sco_ScopeDB[Sco_SCOPE_CRS],Cods[Sco_SCOPE_CRS],
		  (HiddenAllowed & 1 << Sco_SCOPE_CRS) ? "" :
						         " AND Hidden='N'");
      SubQueryFilled = true;
     }
   else
      SubQuery[Sco_SCOPE_CRS][0] = '\0';

   /* Build query */
   if (SubQueryFilled)
     {
      switch (Gbl.Games.SelectedOrder)
	{
	 case Rmt_ORDER_BY_START_DATE:
	    sprintf (OrderBySubQuery,"StartTime DESC,EndTime DESC,Title DESC");
	    break;
	 case Rmt_ORDER_BY_END_DATE:
	    sprintf (OrderBySubQuery,"EndTime DESC,StartTime DESC,Title DESC");
	    break;
	}

      sprintf (Query,"SELECT GamCod FROM games"
		     " WHERE %s%s%s%s%s%s"
		     " ORDER BY %s",
		  SubQuery[Sco_SCOPE_SYS],
		  SubQuery[Sco_SCOPE_CTY],
		  SubQuery[Sco_SCOPE_INS],
		  SubQuery[Sco_SCOPE_CTR],
		  SubQuery[Sco_SCOPE_DEG],
		  SubQuery[Sco_SCOPE_CRS],
		  OrderBySubQuery);
     }
   else
      Lay_ShowErrorAndExit ("Can not get list of games.");

   /* Make query */
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get games");

   if (NumRows) // Games found...
     {
      Gbl.Games.Num = (unsigned) NumRows;

      /***** Create list of games *****/
      if ((Gbl.Games.LstGamCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of games.");

      /***** Get the games codes *****/
      for (NumGame = 0;
	   NumGame < Gbl.Games.Num;
	   NumGame++)
        {
         /* Get next game code */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.Games.LstGamCods[NumGame] = Str_ConvertStrCodToLongCod (row[0])) < 0)
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

static void Rmt_SetAllowedAndHiddenScopes (unsigned *ScopesAllowed,
                                           unsigned *HiddenAllowed)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_UNK:	// User not logged in *********************************
	 *ScopesAllowed = 0;
	 *HiddenAllowed = 0;
         break;
      case Rol_GST:	// User not belonging to any course *******************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 break;
      case Rol_USR:	// Student or teacher in other courses...
   	   	   	// ...but not belonging to the current course *********
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.CurrentCty.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod))
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		 }
	      }
	   }
         break;
      case Rol_STD:	// Student in current course **************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.CurrentCty.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod))
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		     if (Gbl.Usrs.Me.IBelongToCurrentCrs)
			*ScopesAllowed |= 1 << Sco_SCOPE_CRS;
		    }
		 }
	      }
	   }
         break;
      case Rol_NET:	// Non-editing teacher in current course **************
      case Rol_TCH:	// Teacher in current course **************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Usr_CheckIfIBelongToCty (Gbl.CurrentCty.Cty.CtyCod))
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Usr_CheckIfIBelongToIns (Gbl.CurrentIns.Ins.InsCod))
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Usr_CheckIfIBelongToCtr (Gbl.CurrentCtr.Ctr.CtrCod))
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Usr_CheckIfIBelongToDeg (Gbl.CurrentDeg.Deg.DegCod))
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		     if (Gbl.Usrs.Me.IBelongToCurrentCrs)
		       {
			*ScopesAllowed |= 1 << Sco_SCOPE_CRS;
			*HiddenAllowed |= 1 << Sco_SCOPE_CRS;	// A non-editing teacher or teacher can view hidden course games
		       }
		    }
		 }
	      }
	   }
         break;
      case Rol_DEG_ADM:	// Degree administrator *******************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  if (Gbl.CurrentDeg.Deg.DegCod > 0)		// Degree selected
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
		     *HiddenAllowed |= 1 << Sco_SCOPE_DEG;	// A degree admin can view hidden degree games
		    }
	         }
	      }
	   }
	 break;
      case Rol_CTR_ADM:	// Centre administrator *******************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
		  *HiddenAllowed |= 1 << Sco_SCOPE_CTR;		// A centre admin can view hidden centre games
		 }
	      }
	   }
	 break;
      case Rol_INS_ADM:	// Institution administrator **************************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 0;
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       *HiddenAllowed |= 1 << Sco_SCOPE_INS;		// An institution admin can view hidden institution games
	      }
	   }
	 break;
      case Rol_SYS_ADM:	// System administrator (superuser) *******************
	 *ScopesAllowed = 1 << Sco_SCOPE_SYS;
	 *HiddenAllowed = 1 << Sco_SCOPE_SYS;			// A system admin can view hidden system games
	 if (Gbl.CurrentCty.Cty.CtyCod > 0)			// Country selected
	   {
	    *ScopesAllowed |= 1 << Sco_SCOPE_CTY;
	    *HiddenAllowed |= 1 << Sco_SCOPE_CTY;		// A system admin can view hidden country games
	    if (Gbl.CurrentIns.Ins.InsCod > 0)			// Institution selected
	      {
	       *ScopesAllowed |= 1 << Sco_SCOPE_INS;
	       *HiddenAllowed |= 1 << Sco_SCOPE_INS;		// A system admin can view hidden institution games
	       if (Gbl.CurrentCtr.Ctr.CtrCod > 0)		// Centre selected
		 {
		  *ScopesAllowed |= 1 << Sco_SCOPE_CTR;
	          *HiddenAllowed |= 1 << Sco_SCOPE_CTR;		// A system admin can view hidden centre games
		  if (Gbl.CurrentDeg.Deg.DegCod > 0)		// Degree selected
		    {
		     *ScopesAllowed |= 1 << Sco_SCOPE_DEG;
	             *HiddenAllowed |= 1 << Sco_SCOPE_DEG;	// A system admin can view hidden degree games
		     if (Gbl.CurrentCrs.Crs.CrsCod > 0)		// Course selected
		       {
			*ScopesAllowed |= 1 << Sco_SCOPE_CRS;
	                *HiddenAllowed |= 1 << Sco_SCOPE_CRS;	// A system admin can view hidden course games
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

void Rmt_GetDataOfGameByCod (struct Game *Game)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Build query *****/
   sprintf (Query,"SELECT GamCod,Scope,Cod,Hidden,Roles,UsrCod,"
                  "UNIX_TIMESTAMP(StartTime),"
                  "UNIX_TIMESTAMP(EndTime),"
                  "NOW() BETWEEN StartTime AND EndTime,"
                  "Title"
                  " FROM games"
                  " WHERE GamCod=%ld",
            Game->GamCod);

   /***** Get data of game from database *****/
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get game data");

   if (NumRows) // Game found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the game (row[0]) */
      Game->GamCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get game scope (row[1]) */
      if ((Game->Scope = Sco_GetScopeFromDBStr (row[1])) == Sco_SCOPE_UNK)
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
                Rmt_MAX_BYTES_SURVEY_TITLE);

      /* Get number of questions and number of users who have already answer this game */
      Game->NumQsts = Rmt_GetNumQstsGame (Game->GamCod);
      Game->NumUsrs = Rmt_GetNumUsrsWhoHaveAnsweredGame (Game->GamCod);

      /* Am I logged with a valid role to answer this game? */
      Game->Status.IAmLoggedWithAValidRoleToAnswer = (Game->Roles & (1 << Gbl.Usrs.Me.Role.Logged));

      /* Do I belong to valid groups to answer this game? */
      switch (Game->Scope)
        {
	 case Sco_SCOPE_UNK:	// Unknown
            Lay_ShowErrorAndExit ("Wrong game scope.");
	    break;
	 case Sco_SCOPE_SYS:	// System
            Game->Status.IBelongToScope = Gbl.Usrs.Me.Logged;
	    break;
	 case Sco_SCOPE_CTY:	// Country
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToCty (Game->Cod);
	    break;
	 case Sco_SCOPE_INS:	// Institution
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToIns (Game->Cod);
	    break;
	 case Sco_SCOPE_CTR:	// Centre
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToCtr (Game->Cod);
	    break;
	 case Sco_SCOPE_DEG:	// Degree
            Game->Status.IBelongToScope = Usr_CheckIfIBelongToDeg (Game->Cod);
	    break;
	 case Sco_SCOPE_CRS:	// Course
	    Game->Status.IBelongToScope = Usr_CheckIfIBelongToCrs (Game->Cod) &&
					 Rmt_CheckIfICanDoThisGameBasedOnGrps (Game->GamCod);
	    break;
        }

      /* Have I answered this game? */
      Game->Status.IHaveAnswered = Rmt_CheckIfIHaveAnsweredGame (Game->GamCod);

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
            Game->Status.ICanViewResults = (Game->Scope == Sco_SCOPE_CRS ||
        	                           Game->Scope == Sco_SCOPE_DEG ||
        	                           Game->Scope == Sco_SCOPE_CTR ||
        	                           Game->Scope == Sco_SCOPE_INS ||
        	                           Game->Scope == Sco_SCOPE_CTY ||
        	                           Game->Scope == Sco_SCOPE_SYS) &&
        	                          (Game->NumQsts != 0) &&
                                           Game->Status.Visible &&
                                           Game->Status.Open &&
                                           Game->Status.IAmLoggedWithAValidRoleToAnswer &&
                                           Game->Status.IBelongToScope &&
                                           Game->Status.IHaveAnswered;
            Game->Status.ICanEdit         = false;
            break;
         case Rol_NET:
            Game->Status.ICanViewResults = (Game->Scope == Sco_SCOPE_CRS ||
        	                           Game->Scope == Sco_SCOPE_DEG ||
        	                           Game->Scope == Sco_SCOPE_CTR ||
        	                           Game->Scope == Sco_SCOPE_INS ||
        	                           Game->Scope == Sco_SCOPE_CTY ||
        	                           Game->Scope == Sco_SCOPE_SYS) &&
        	                           Game->NumQsts != 0 &&
                                          !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        = false;
            break;
         case Rol_TCH:
            Game->Status.ICanViewResults = (Game->Scope == Sco_SCOPE_CRS ||
        	                           Game->Scope == Sco_SCOPE_DEG ||
        	                           Game->Scope == Sco_SCOPE_CTR ||
        	                           Game->Scope == Sco_SCOPE_INS ||
        	                           Game->Scope == Sco_SCOPE_CTY ||
        	                           Game->Scope == Sco_SCOPE_SYS) &&
        	                           Game->NumQsts != 0 &&
                                          !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Sco_SCOPE_CRS &&
                                           Game->Status.IBelongToScope;
            break;
         case Rol_DEG_ADM:
            Game->Status.ICanViewResults = (Game->Scope == Sco_SCOPE_DEG ||
        	                           Game->Scope == Sco_SCOPE_CTR ||
        	                           Game->Scope == Sco_SCOPE_INS ||
        	                           Game->Scope == Sco_SCOPE_CTY ||
        	                           Game->Scope == Sco_SCOPE_SYS) &&
        	                          (Game->NumQsts != 0) &&
                                          !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Sco_SCOPE_DEG &&
                                           Game->Status.IBelongToScope;
            break;
         case Rol_CTR_ADM:
            Game->Status.ICanViewResults = (Game->Scope == Sco_SCOPE_CTR ||
        	                           Game->Scope == Sco_SCOPE_INS ||
        	                           Game->Scope == Sco_SCOPE_CTY ||
        	                           Game->Scope == Sco_SCOPE_SYS) &&
        	                          (Game->NumQsts != 0) &&
                                          !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Sco_SCOPE_CTR &&
                                           Game->Status.IBelongToScope;
            break;
         case Rol_INS_ADM:
            Game->Status.ICanViewResults = (Game->Scope == Sco_SCOPE_INS ||
        	                           Game->Scope == Sco_SCOPE_CTY ||
        	                           Game->Scope == Sco_SCOPE_SYS) &&
        	                          (Game->NumQsts != 0) &&
                                          !Game->Status.ICanAnswer;
            Game->Status.ICanEdit        =  Game->Scope == Sco_SCOPE_INS &&
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
      Game->Scope                   = Sco_SCOPE_UNK;
      Game->Roles                   = 0;
      Game->UsrCod                  = -1L;
      Game->TimeUTC[Rmt_START_TIME] =
      Game->TimeUTC[Rmt_END_TIME  ] = (time_t) 0;
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

void Rmt_FreeListGames (void)
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

static void Rmt_GetGameTxtFromDB (long GamCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of game from database *****/
   sprintf (Query,"SELECT Txt FROM games WHERE GamCod=%ld",GamCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get game text");

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
/******************** Get summary and content of a game  *******************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Rmt_GetNotifGame (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                         char **ContentStr,
                         long GamCod,bool GetContent)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Build query *****/
   sprintf (Query,"SELECT Title,Txt FROM games WHERE GamCod=%ld",
            GamCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get row *****/
            row = mysql_fetch_row (mysql_res);

            /***** Get summary *****/
            Str_Copy (SummaryStr,row[0],
                      Ntf_MAX_BYTES_SUMMARY);

            /***** Get content *****/
            if (GetContent)
              {
               Length = strlen (row[1]);
               if ((*ContentStr = (char *) malloc (Length + 1)) == NULL)
                  Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
               Str_Copy (*ContentStr,row[1],
                         Length);
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/******************* Write parameter with code of game *********************/
/*****************************************************************************/

void Rmt_PutParamGameCod (long GamCod)
  {
   Par_PutHiddenParamLong ("GamCod",GamCod);
  }

/*****************************************************************************/
/******************** Get parameter with code of game **********************/
/*****************************************************************************/

long Rmt_GetParamGameCod (void)
  {
   /***** Get code of game *****/
   return Par_GetParToLong ("GamCod");
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing of a game ****************/
/*****************************************************************************/

void Rmt_AskRemGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_game_X;
   extern const char *Txt_Remove_game;
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get parameters *****/
   Rmt_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Rmt_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not remove this game.");

   /***** Show question and button to remove game *****/
   Gbl.Games.CurrentGamCod = Game.GamCod;
   sprintf (Gbl.Alert.Txt,Txt_Do_you_really_want_to_remove_the_game_X,
            Game.Title);
   Ale_ShowAlertAndButton (Ale_QUESTION,Gbl.Alert.Txt,
                           ActRemGam,NULL,NULL,Rmt_PutParams,
			   Btn_REMOVE_BUTTON,Txt_Remove_game);

   /***** Show games again *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/****************************** Remove a game ******************************/
/*****************************************************************************/

void Rmt_RemoveGame (void)
  {
   extern const char *Txt_Game_X_removed;
   char Query[512];
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Rmt_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not remove this game.");

   /***** Remove all the users in this game *****/
   sprintf (Query,"DELETE FROM gam_users WHERE GamCod=%ld",
            Game.GamCod);
   DB_QueryDELETE (Query,"can not remove users who are answered a game");

   /***** Remove all the questions in this game *****/
   sprintf (Query,"DELETE FROM gam_questions"
                  " WHERE GamCod=%ld",
            Game.GamCod);
   DB_QueryDELETE (Query,"can not remove questions of a game");

   /***** Remove all the groups of this game *****/
   Rmt_RemoveAllTheGrpsAssociatedToAndGame (Game.GamCod);

   /***** Remove game *****/
   sprintf (Query,"DELETE FROM games WHERE GamCod=%ld",
            Game.GamCod);
   DB_QueryDELETE (Query,"can not remove game");

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifAsRemoved (Ntf_EVENT_SURVEY,Game.GamCod);

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Game_X_removed,
            Game.Title);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show games again *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/***************** Ask for confirmation of reset of a game *****************/
/*****************************************************************************/

void Rmt_AskResetGame (void)
  {
   extern const char *Txt_Do_you_really_want_to_reset_the_game_X;
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get parameters *****/
   Rmt_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Rmt_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not reset this game.");

   /***** Ask for confirmation of reset *****/
   sprintf (Gbl.Alert.Txt,Txt_Do_you_really_want_to_reset_the_game_X,
            Game.Title);
   Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);

   /***** Button of confirmation of reset *****/
   Gbl.Games.CurrentGamCod = Game.GamCod;
   Rmt_PutButtonToResetGame ();

   /***** Show games again *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/************************* Put button to reset game ************************/
/*****************************************************************************/

static void Rmt_PutButtonToResetGame (void)
  {
   extern const char *Txt_Reset_game;

   Act_FormStart (ActRstGam);
   Rmt_PutParams ();
   Btn_PutRemoveButton (Txt_Reset_game);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************************* Reset a game ******************************/
/*****************************************************************************/

void Rmt_ResetGame (void)
  {
   extern const char *Txt_Game_X_reset;
   char Query[512];
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Rmt_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not reset this game.");

   /***** Remove all the users in this game *****/
   sprintf (Query,"DELETE FROM gam_users WHERE GamCod=%ld",
            Game.GamCod);
   DB_QueryDELETE (Query,"can not remove users who are answered a game");

   /***** Reset all the answers in this game *****/
   sprintf (Query,"UPDATE gam_answers,gam_questions SET gam_answers.NumUsrs=0"
                  " WHERE gam_questions.GamCod=%ld"
                  " AND gam_questions.QstCod=gam_answers.QstCod",
            Game.GamCod);
   DB_QueryUPDATE (Query,"can not reset answers of a game");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Game_X_reset,
            Game.Title);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show games again *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/******************************** Hide a game ******************************/
/*****************************************************************************/

void Rmt_HideGame (void)
  {
   extern const char *Txt_Game_X_is_now_hidden;
   char Query[128];
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Rmt_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not hide this game.");

   /***** Hide game *****/
   sprintf (Query,"UPDATE games SET Hidden='Y' WHERE GamCod=%ld",
            Game.GamCod);
   DB_QueryUPDATE (Query,"can not hide game");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Game_X_is_now_hidden,
            Game.Title);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show games again *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/******************************** Show a game ******************************/
/*****************************************************************************/

void Rmt_UnhideGame (void)
  {
   extern const char *Txt_Game_X_is_now_visible;
   char Query[128];
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Rmt_GetDataOfGameByCod (&Game);
   if (!Game.Status.ICanEdit)
      Lay_ShowErrorAndExit ("You can not unhide this game.");

   /***** Show game *****/
   sprintf (Query,"UPDATE games SET Hidden='N' WHERE GamCod=%ld",
            Game.GamCod);
   DB_QueryUPDATE (Query,"can not show game");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Game_X_is_now_visible,
            Game.Title);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show games again *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/******************* Check if the title of a game exists *******************/
/*****************************************************************************/

static bool Rmt_CheckIfSimilarGameExists (struct Game *Game)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[512 + Rmt_MAX_BYTES_SURVEY_TITLE];

   /***** Get number of games with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM games"
                  " WHERE Scope='%s' AND Cod=%ld"
                  " AND Title='%s' AND GamCod<>%ld",
            Sco_ScopeDB[Game->Scope],Game->Cod,
            Game->Title,Game->GamCod);
   return (DB_QueryCOUNT (Query,"can not get similar games") != 0);
  }

/*****************************************************************************/
/********************* Put a form to create a new game *********************/
/*****************************************************************************/

void Rmt_RequestCreatOrEditGame (void)
  {
   extern const char *Hlp_ASSESSMENT_Games_new_game;
   extern const char *Hlp_ASSESSMENT_Games_edit_game;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_game;
   extern const char *Txt_Scope;
   extern const char *Txt_Edit_game;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Users;
   extern const char *Txt_Create_game;
   extern const char *Txt_Save;
   struct Game Game;
   struct GameQuestion GameQst;
   bool ItsANewGame;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get parameters *****/
   Rmt_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Get the code of the game *****/
   ItsANewGame = ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L);

   /***** Get from the database the data of the game *****/
   if (ItsANewGame)
     {
      /***** Put link (form) to create new game *****/
      if (!Rmt_CheckIfICanCreateGame ())
         Lay_ShowErrorAndExit ("You can not create a new game here.");

      /* Initialize to empty game */
      Game.GamCod = -1L;
      Game.Scope  = Sco_SCOPE_UNK;
      Game.Roles  = (1 << Rol_STD);
      Game.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Game.TimeUTC[Rmt_START_TIME] = Gbl.StartExecutionTimeUTC;
      Game.TimeUTC[Rmt_END_TIME  ] = Gbl.StartExecutionTimeUTC + (24 * 60 * 60);	// +24 hours
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
      Rmt_GetDataOfGameByCod (&Game);
      if (!Game.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this game.");

      /* Get text of the game from database */
      Rmt_GetGameTxtFromDB (Game.GamCod,Txt);
     }

   /***** Start form *****/
   Gbl.Games.CurrentGamCod = Game.GamCod;
   Act_FormStart (ItsANewGame ? ActNewGam :
	                        ActChgGam);
   Rmt_PutParams ();

   /***** Start box and table *****/
   if (ItsANewGame)
      Box_StartBoxTable (NULL,Txt_New_game,NULL,
                         Hlp_ASSESSMENT_Games_new_game,Box_NOT_CLOSABLE,2);
   else
      Box_StartBoxTable (NULL,Txt_Edit_game,NULL,
                         Hlp_ASSESSMENT_Games_edit_game,Box_NOT_CLOSABLE,2);

   /***** Scope of the game *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"ScopeGame\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Scope);
   Rmt_SetDefaultAndAllowedScope (&Game);
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
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Title,
            Rmt_MAX_CHARS_SURVEY_TITLE,Game.Title);

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
            The_ClassForm[Gbl.Prefs.Theme],
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
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Users);
   Rol_WriteSelectorRoles (1 << Rol_STD |
                           1 << Rol_NET |
                           1 << Rol_TCH,
                           Game.Roles,
                           false,false);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /***** Groups *****/
   Rmt_ShowLstGrpsToEditGame (Game.GamCod);

   /***** End table, send button and end box *****/
   if (ItsANewGame)
      Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_game);
   else
      Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_Save);

   /***** End form *****/
   Act_FormEnd ();

   /***** Show questions of the game ready to be edited *****/
   if (!ItsANewGame)
      Rmt_ListGameQuestions (&Game,&GameQst);
  }

/*****************************************************************************/
/****** Set default and allowed scopes depending on logged user's role *******/
/*****************************************************************************/

static void Rmt_SetDefaultAndAllowedScope (struct Game *Game)
  {
   bool ICanEdit = false;

   /***** Set default scope *****/
   Gbl.Scope.Default = Sco_SCOPE_UNK;
   Gbl.Scope.Allowed = 0;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:	// Teachers only can edit course games
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0)
	   {
	    if (Game->Scope == Sco_SCOPE_UNK)	// Scope not defined
	       Game->Scope = Sco_SCOPE_CRS;
	    if (Game->Scope == Sco_SCOPE_CRS)
	      {
               Gbl.Scope.Default = Game->Scope;
	       Gbl.Scope.Allowed = 1 << Sco_SCOPE_CRS;
	       ICanEdit = true;
	      }
	   }
         break;
      case Rol_DEG_ADM:	// Degree admins only can edit degree games
	 if (Game->Scope == Sco_SCOPE_UNK)	// Scope not defined
	    Game->Scope = Sco_SCOPE_DEG;
	 if (Game->Scope == Sco_SCOPE_DEG)
	   {
	    Gbl.Scope.Default = Game->Scope;
	    Gbl.Scope.Allowed = 1 << Sco_SCOPE_DEG;
	    ICanEdit = true;
	   }
         break;
      case Rol_CTR_ADM:	// Centre admins only can edit centre games
	 if (Game->Scope == Sco_SCOPE_UNK)	// Scope not defined
	    Game->Scope = Sco_SCOPE_CTR;
	 if (Game->Scope == Sco_SCOPE_CTR)
	   {
	    Gbl.Scope.Default = Game->Scope;
	    Gbl.Scope.Allowed = 1 << Sco_SCOPE_CTR;
	    ICanEdit = true;
	   }
         break;
      case Rol_INS_ADM:	// Institution admins only can edit institution games
	 if (Game->Scope == Sco_SCOPE_UNK)	// Scope not defined
	    Game->Scope = Sco_SCOPE_INS;
	 if (Game->Scope == Sco_SCOPE_INS)
	   {
	    Gbl.Scope.Default = Game->Scope;
	    Gbl.Scope.Allowed = 1 << Sco_SCOPE_INS;
	    ICanEdit = true;
	   }
         break;
      case Rol_SYS_ADM:// System admins can edit any game
	 if (Game->Scope == Sco_SCOPE_UNK)	// Scope not defined
	   {
	    if      (Gbl.CurrentCrs.Crs.CrsCod > 0)
	       Game->Scope = Sco_SCOPE_CRS;
	    else if (Gbl.CurrentDeg.Deg.DegCod > 0)
	       Game->Scope = Sco_SCOPE_DEG;
	    else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
	       Game->Scope = Sco_SCOPE_CTR;
	    else if (Gbl.CurrentIns.Ins.InsCod > 0)
	       Game->Scope = Sco_SCOPE_INS;
	    else if (Gbl.CurrentCty.Cty.CtyCod > 0)
	       Game->Scope = Sco_SCOPE_CTY;
	    else
	       Game->Scope = Sco_SCOPE_SYS;
	   }
         Gbl.Scope.Default = Game->Scope;
         Gbl.Scope.Allowed = 1 << Sco_SCOPE_SYS |
	                     1 << Sco_SCOPE_CTY |
	                     1 << Sco_SCOPE_INS |
	                     1 << Sco_SCOPE_CTR |
                             1 << Sco_SCOPE_DEG |
                             1 << Sco_SCOPE_CRS;
	 ICanEdit = true;
	 break;
      default:
	 break;
     }

   if (!ICanEdit)
      Lay_ShowErrorAndExit ("You don't have permission to edit games here.");
  }

/*****************************************************************************/
/******************** Show list of groups to edit a game *******************/
/*****************************************************************************/

static void Rmt_ShowLstGrpsToEditGame (long GamCod)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.CurrentCrs.Grps.GrpTypes.Num)
     {
      /***** Start box and table *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s:"
	                 "</td>"
                         "<td class=\"LEFT_TOP\">",
               The_ClassForm[Gbl.Prefs.Theme],
               Txt_Groups);
      Box_StartBoxTable ("95%",NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,0);

      /***** First row: checkbox to select the whole course *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td colspan=\"7\" class=\"DAT LEFT_MIDDLE\">"
                         "<label>"
                         "<input type=\"checkbox\""
                         " id=\"WholeCrs\" name=\"WholeCrs\" value=\"Y\"");
      if (!Rmt_CheckIfGamIsAssociatedToGrps (GamCod))
         fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," onclick=\"uncheckChildren(this,'GrpCods')\" />"
	                 "%s %s"
	                 "</label>"
	                 "</td>"
	                 "</tr>",
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShrtName);

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.CurrentCrs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttSvyGam (&Gbl.CurrentCrs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
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

void Rmt_RecFormGame (void)
  {
   extern const char *Txt_Already_existed_a_game_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_game;
   struct Game OldGame;
   struct Game NewGame;
   struct GameQuestion GameQst;
   bool ItsANewGame;
   bool NewGameIsCorrect = true;
   unsigned NumUsrsToBeNotifiedByEMail;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the game *****/
   ItsANewGame = ((NewGame.GamCod = Rmt_GetParamGameCod ()) == -1L);

   if (ItsANewGame)
      NewGame.Scope = Sco_SCOPE_UNK;
   else
     {
      /* Get data of the old (current) game from database */
      OldGame.GamCod = NewGame.GamCod;
      Rmt_GetDataOfGameByCod (&OldGame);
      if (!OldGame.Status.ICanEdit)
         Lay_ShowErrorAndExit ("You can not update this game.");
      NewGame.Scope = OldGame.Scope;
     }

   /***** Get scope *****/
   Rmt_SetDefaultAndAllowedScope (&NewGame);
   Sco_GetScope ("ScopeGame");
   switch (Gbl.Scope.Current)
     {
      case Sco_SCOPE_SYS:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
         NewGame.Scope = Sco_SCOPE_SYS;
         NewGame.Cod = -1L;
         break;
      case Sco_SCOPE_CTY:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Sco_SCOPE_CTY;
	 NewGame.Cod = Gbl.CurrentCty.Cty.CtyCod;
         break;
      case Sco_SCOPE_INS:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_INS_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Sco_SCOPE_INS;
	 NewGame.Cod = Gbl.CurrentIns.Ins.InsCod;
         break;
      case Sco_SCOPE_CTR:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_CTR_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Sco_SCOPE_CTR;
	 NewGame.Cod = Gbl.CurrentCtr.Ctr.CtrCod;
         break;
      case Sco_SCOPE_DEG:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_DEG_ADM)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Sco_SCOPE_DEG;
	 NewGame.Cod = Gbl.CurrentDeg.Deg.DegCod;
         break;
      case Sco_SCOPE_CRS:
	 if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM &&
	     Gbl.Usrs.Me.Role.Logged != Rol_TCH)
	    Lay_ShowErrorAndExit ("Wrong game scope.");
	 NewGame.Scope = Sco_SCOPE_CRS;
	 NewGame.Cod = Gbl.CurrentCrs.Crs.CrsCod;
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }

   /***** Get start/end date-times *****/
   NewGame.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewGame.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get game title *****/
   Par_GetParToText ("Title",NewGame.Title,Rmt_MAX_BYTES_SURVEY_TITLE);

   /***** Get game text and insert links *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewGame.TimeUTC[Rmt_START_TIME] == 0)
      NewGame.TimeUTC[Rmt_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewGame.TimeUTC[Rmt_END_TIME] == 0)
      NewGame.TimeUTC[Rmt_END_TIME] = NewGame.TimeUTC[Rmt_START_TIME] + 24 * 60 * 60;	// +24 hours

   /***** Get users who can answer this game *****/
   NewGame.Roles = Rol_GetSelectedRoles ();

   /***** Check if title is correct *****/
   if (NewGame.Title[0])	// If there's a game title
     {
      /* If title of game was in database... */
      if (Rmt_CheckIfSimilarGameExists (&NewGame))
        {
         NewGameIsCorrect = false;
         sprintf (Gbl.Alert.Txt,Txt_Already_existed_a_game_with_the_title_X,
                  NewGame.Title);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
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
         Rmt_CreateGame (&NewGame,Txt);	// Add new game to database
      else
         Rmt_UpdateGame (&NewGame,Txt);

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();
     }
   else
      Rmt_RequestCreatOrEditGame ();

   /***** Notify by email about the new game *****/
   if (NewGame.Scope == Sco_SCOPE_CRS)	// Notify only the games for a course, not for a degree or global
      if ((NumUsrsToBeNotifiedByEMail = Ntf_StoreNotifyEventsToAllUsrs (Ntf_EVENT_SURVEY,NewGame.GamCod)))
         Rmt_UpdateNumUsrsNotifiedByEMailAboutGame (NewGame.GamCod,NumUsrsToBeNotifiedByEMail);

   /***** Show games again *****/
   Rmt_ListAllGames (&GameQst);
  }

/*****************************************************************************/
/*********** Update number of users notified in table of games *************/
/*****************************************************************************/

static void Rmt_UpdateNumUsrsNotifiedByEMailAboutGame (long GamCod,
                                                         unsigned NumUsrsToBeNotifiedByEMail)
  {
   char Query[256];

   /***** Update number of users notified *****/
   sprintf (Query,"UPDATE games SET NumNotif=NumNotif+%u"
                  " WHERE GamCod=%ld",
            NumUsrsToBeNotifiedByEMail,GamCod);
   DB_QueryUPDATE (Query,"can not update the number of notifications of a game");
  }

/*****************************************************************************/
/*************************** Create a new game *****************************/
/*****************************************************************************/

static void Rmt_CreateGame (struct Game *Game,const char *Txt)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   extern const char *Txt_Created_new_game_X;
   char Query[1024 +
              Rmt_MAX_BYTES_SURVEY_TITLE +
              Cns_MAX_BYTES_TEXT];

   /***** Create a new game *****/
   sprintf (Query,"INSERT INTO games"
	          " (Scope,Cod,Hidden,Roles,UsrCod,StartTime,EndTime,Title,Txt)"
                  " VALUES"
                  " ('%s',%ld,'N',%u,%ld,"
                  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
                  "'%s','%s')",
            Sco_ScopeDB[Game->Scope],Game->Cod,
            Game->Roles,
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Game->TimeUTC[Rmt_START_TIME],
            Game->TimeUTC[Rmt_END_TIME  ],
            Game->Title,
            Txt);
   Game->GamCod = DB_QueryINSERTandReturnCode (Query,"can not create new game");

   /***** Create groups *****/
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Rmt_CreateGrps (Game->GamCod);

   /***** Write success message *****/
   sprintf (Gbl.Alert.Txt,Txt_Created_new_game_X,
            Game->Title);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
  }

/*****************************************************************************/
/************************* Update an existing game *************************/
/*****************************************************************************/

static void Rmt_UpdateGame (struct Game *Game,const char *Txt)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   extern const char *Txt_The_game_has_been_modified;
   char Query[1024 +
              Rmt_MAX_BYTES_SURVEY_TITLE +
              Cns_MAX_BYTES_TEXT];

   /***** Update the data of the game *****/
   sprintf (Query,"UPDATE games"
	          " SET Scope='%s',Cod=%ld,Roles=%u,"
	          "StartTime=FROM_UNIXTIME(%ld),"
	          "EndTime=FROM_UNIXTIME(%ld),"
	          "Title='%s',Txt='%s'"
                  " WHERE GamCod=%ld",
            Sco_ScopeDB[Game->Scope],Game->Cod,
            Game->Roles,
            Game->TimeUTC[Rmt_START_TIME],
            Game->TimeUTC[Rmt_END_TIME  ],
            Game->Title,
            Txt,
            Game->GamCod);
   DB_QueryUPDATE (Query,"can not update game");

   /***** Update groups *****/
   /* Remove old groups */
   Rmt_RemoveAllTheGrpsAssociatedToAndGame (Game->GamCod);

   /* Create new groups */
   if (Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps)
      Rmt_CreateGrps (Game->GamCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_game_has_been_modified);
  }

/*****************************************************************************/
/*************** Check if a game is associated to any group ****************/
/*****************************************************************************/

static bool Rmt_CheckIfGamIsAssociatedToGrps (long GamCod)
  {
   char Query[128];

   /***** Get if a game is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM gam_grp WHERE GamCod=%ld",
            GamCod);
   return (DB_QueryCOUNT (Query,"can not check if a game is associated to groups") != 0);
  }

/*****************************************************************************/
/**************** Check if a game is associated to a group *****************/
/*****************************************************************************/

bool Rmt_CheckIfGamIsAssociatedToGrp (long GamCod,long GrpCod)
  {
   char Query[256];

   /***** Get if a game is associated to a group from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM gam_grp"
	          " WHERE GamCod=%ld AND GrpCod=%ld",
            GamCod,GrpCod);
   return (DB_QueryCOUNT (Query,"can not check if a game is associated to a group") != 0);
  }

/*****************************************************************************/
/************************* Remove groups of a game *************************/
/*****************************************************************************/

static void Rmt_RemoveAllTheGrpsAssociatedToAndGame (long GamCod)
  {
   char Query[128];

   /***** Remove groups of the game *****/
   sprintf (Query,"DELETE FROM gam_grp WHERE GamCod=%ld",
            GamCod);
   DB_QueryDELETE (Query,"can not remove the groups associated to a game");
  }

/*****************************************************************************/
/******************* Remove one group from all the games *******************/
/*****************************************************************************/

void Rmt_RemoveGroup (long GrpCod)
  {
   char Query[128];

   /***** Remove group from all the games *****/
   sprintf (Query,"DELETE FROM gam_grp WHERE GrpCod=%ld",
	    GrpCod);
   DB_QueryDELETE (Query,"can not remove group"
	                 " from the associations between games and groups");
  }

/*****************************************************************************/
/*************** Remove groups of one type from all the games **************/
/*****************************************************************************/

void Rmt_RemoveGroupsOfType (long GrpTypCod)
  {
   char Query[256];

   /***** Remove group from all the games *****/
   sprintf (Query,"DELETE FROM gam_grp USING crs_grp,gam_grp"
                  " WHERE crs_grp.GrpTypCod=%ld"
                  " AND crs_grp.GrpCod=gam_grp.GrpCod",
            GrpTypCod);
   DB_QueryDELETE (Query,"can not remove groups of a type"
	                 " from the associations between games and groups");
  }

/*****************************************************************************/
/************************ Create groups of a game **************************/
/*****************************************************************************/

static void Rmt_CreateGrps (long GamCod)
  {
   unsigned NumGrpSel;
   char Query[256];

   /***** Create groups of the game *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.CurrentCrs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
     {
      /* Create group */
      sprintf (Query,"INSERT INTO gam_grp"
	             " (GamCod,GrpCod)"
	             " VALUES"
	             " (%ld,%ld)",
               GamCod,Gbl.CurrentCrs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
      DB_QueryINSERT (Query,"can not associate a group to a game");
     }
  }

/*****************************************************************************/
/************ Get and write the names of the groups of a game **************/
/*****************************************************************************/

static void Rmt_GetAndWriteNamesOfGrpsAssociatedToGame (struct Game *Game)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to a game from database *****/
   sprintf (Query,"SELECT crs_grp_types.GrpTypName,crs_grp.GrpName"
                  " FROM gam_grp,crs_grp,crs_grp_types"
                  " WHERE gam_grp.GamCod=%ld"
                  " AND gam_grp.GrpCod=crs_grp.GrpCod"
                  " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
                  " ORDER BY crs_grp_types.GrpTypName,crs_grp.GrpName",
            Game->GamCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get groups of a game");

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
               Txt_The_whole_course,Gbl.CurrentCrs.Crs.ShrtName);

   fprintf (Gbl.F.Out,"</div>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Remove all the games of a place on the hierarchy *************/
/************ (country, institution, centre, degree or course)   *************/
/*****************************************************************************/

void Rmt_RemoveGames (Sco_Scope_t Scope,long Cod)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[512];

   /***** Remove all the users in course games *****/
   sprintf (Query,"DELETE FROM gam_users"
	          " USING games,gam_users"
                  " WHERE games.Scope='%s' AND games.Cod=%ld"
                  " AND games.GamCod=gam_users.GamCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove users"
	                 " who had answered games in a place on the hierarchy");

   /***** Remove all the answers in course games *****/
   sprintf (Query,"DELETE FROM gam_answers"
	          " USING games,gam_questions,gam_answers"
                  " WHERE games.Scope='%s' AND games.Cod=%ld"
                  " AND games.GamCod=gam_questions.GamCod"
                  " AND gam_questions.QstCod=gam_answers.QstCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove answers of games in a place on the hierarchy");

   /***** Remove all the questions in course games *****/
   sprintf (Query,"DELETE FROM gam_questions"
	          " USING games,gam_questions"
                  " WHERE games.Scope='%s' AND games.Cod=%ld"
                  " AND games.GamCod=gam_questions.GamCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove questions of games in a place on the hierarchy");

   /***** Remove groups *****/
   sprintf (Query,"DELETE FROM gam_grp"
	          " USING games,gam_grp"
                  " WHERE games.Scope='%s' AND games.Cod=%ld"
                  " AND games.GamCod=gam_grp.GamCod",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove all the groups"
	                 " associated to games of a course");

   /***** Remove course games *****/
   sprintf (Query,"DELETE FROM games"
	          " WHERE Scope='%s' AND Cod=%ld",
            Sco_ScopeDB[Scope],Cod);
   DB_QueryDELETE (Query,"can not remove all the games in a place on the hierarchy");
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a game *************/
/*****************************************************************************/

static bool Rmt_CheckIfICanDoThisGameBasedOnGrps (long GamCod)
  {
   char Query[512];

   /***** Get if I can do a game from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM games"
                  " WHERE GamCod=%ld"
                  " AND (GamCod NOT IN (SELECT GamCod FROM gam_grp) OR"
                  " GamCod IN (SELECT gam_grp.GamCod FROM gam_grp,crs_grp_usr"
                  " WHERE crs_grp_usr.UsrCod=%ld"
                  " AND gam_grp.GrpCod=crs_grp_usr.GrpCod))",
            GamCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if I can do a game") != 0);
  }

/*****************************************************************************/
/******************* Get number of questions of a game *********************/
/*****************************************************************************/

static unsigned Rmt_GetNumQstsGame (long GamCod)
  {
   char Query[128];

   /***** Get data of questions from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM gam_questions WHERE GamCod=%ld",
            GamCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of questions of a game");
  }

/*****************************************************************************/
/*********** Put a form to edit/create a question in game  *****************/
/*****************************************************************************/

void Rmt_RequestEditQuestion (void)
  {
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Initialize question to zero *****/
   Rmt_InitQst (&GameQst);

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get the question code */
   GameQst.QstCod = Rmt_GetParamQstCod ();

   /***** Get other parameters *****/
   Rmt_GetParamGameOrder ();
   Grp_GetParamWhichGrps ();
   Gbl.Games.CurrentPage = Pag_GetParamPagNum (Pag_SURVEYS);

   /***** Show form to create a new question in this game *****/
   Tst_ShowFormAskSelectTstsForGame (Game.GamCod);

   /***** Show current game *****/
   Rmt_ShowOneGame (Game.GamCod,&GameQst,true);
  }

/*****************************************************************************/
/********************* Initialize a new question to zero *********************/
/*****************************************************************************/

static void Rmt_InitQst (struct GameQuestion *GameQst)
  {
   unsigned NumAns;

   GameQst->QstCod = -1L;
   GameQst->QstInd = 0;
   GameQst->AnswerType = Rmt_ANS_UNIQUE_CHOICE;
   for (NumAns = 0;
	NumAns < Rmt_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
      GameQst->AnsChoice[NumAns].Text = NULL;
  }

/*****************************************************************************/
/****************** Write parameter with code of question ********************/
/*****************************************************************************/

static void Rmt_PutParamQstCod (long QstCod)
  {
   Par_PutHiddenParamLong ("QstCod",QstCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of question *********************/
/*****************************************************************************/

static long Rmt_GetParamQstCod (void)
  {
   /***** Get code of question *****/
   return Par_GetParToLong ("QstCod");
  }

/*****************************************************************************/
/********************* Remove answers of a game question *******************/
/*****************************************************************************/

static void Rmt_RemAnswersOfAQuestion (long QstCod)
  {
   char Query[128];

   /***** Remove answers *****/
   sprintf (Query,"DELETE FROM gam_answers WHERE QstCod=%ld",
            QstCod);
   DB_QueryDELETE (Query,"can not remove the answers of a question");
  }

/*****************************************************************************/
/*********** Convert a string with the answer type to answer type ************/
/*****************************************************************************/

static Rmt_AnswerType_t Rmt_ConvertFromStrAnsTypDBToAnsTyp (const char *StrAnsTypeBD)
  {
   Rmt_AnswerType_t AnsType;

   for (AnsType = (Rmt_AnswerType_t) 0;
	AnsType < Rmt_NUM_ANS_TYPES;
	AnsType++)
      if (!strcmp (StrAnsTypeBD,Rmt_StrAnswerTypesDB[AnsType]))
         return AnsType;

   return (Rmt_AnswerType_t) 0;
  }

/*****************************************************************************/
/*********** Check if an answer of a question exists in database *************/
/*****************************************************************************/
/*
static bool Rmt_CheckIfAnswerExists (long QstCod,unsigned AnsInd)
  {
   char Query[256];

   ***** Get answers of a question from database *****
   sprintf (Query,"SELECT COUNT(*) FROM gam_answers"
                  " WHERE QstCod=%ld AND AnsInd=%u",
            QstCod,AnsInd);
   return (DB_QueryCOUNT (Query,"can not check if an answer exists") != 0);
  }
*/
/*****************************************************************************/
/**** Get number of users who selected a given answer of a game question *****/
/*****************************************************************************/

unsigned Rmt_GetNumUsrsWhoAnswered (long GamCod,long QstCod,unsigned AnsInd)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs = 0;	// Default returned value

   /***** Get answers of a question from database *****/
   sprintf (Query,"SELECT NumUsrs FROM gam_answers"
                  " WHERE GamCod=%ld AND QstCod=%ld AND AnsInd=%u",
            GamCod,QstCod,AnsInd);
   DB_QuerySELECT (Query,&mysql_res,"can not get number of users who answered");
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are users who selected this answer
      if (sscanf (row[0],"%u",&NumUsrs) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of users who answered.");

   return NumUsrs;
  }

/*****************************************************************************/
/******************* Allocate memory for a choice answer *********************/
/*****************************************************************************/
/*
static bool Rmt_AllocateTextChoiceAnswer (struct GameQuestion *GameQst,unsigned NumAns)
  {
   Rmt_FreeTextChoiceAnswer (GameQst,NumAns);
   if ((GameQst->AnsChoice[NumAns].Text = malloc (Rmt_MAX_BYTES_ANSWER + 1)) == NULL)
     {
      sprintf (Gbl.Alert.Txt,"Not enough memory to store answer.");
      return false;
     }
   GameQst->AnsChoice[NumAns].Text[0] = '\0';
   return true;
  }
*/
/*****************************************************************************/
/******************** Free memory of all choice answers **********************/
/*****************************************************************************/
/*
static void Rmt_FreeTextChoiceAnswers (struct GameQuestion *GameQst,unsigned NumAnswers)
  {
   unsigned NumAns;

   for (NumAns = 0;
	NumAns < NumAnswers;
	NumAns++)
      Rmt_FreeTextChoiceAnswer (GameQst,NumAns);
  }
*/
/*****************************************************************************/
/********************** Free memory of a choice answer ***********************/
/*****************************************************************************/
/*
static void Rmt_FreeTextChoiceAnswer (struct GameQuestion *GameQst,unsigned NumAns)
  {
   if (GameQst->AnsChoice[NumAns].Text)
     {
      free ((void *) GameQst->AnsChoice[NumAns].Text);
      GameQst->AnsChoice[NumAns].Text = NULL;
     }
  }
*/
/*****************************************************************************/
/************************ Receive a question of a game ***********************/
/*****************************************************************************/
/*
void Rmt_ReceiveQst (void)
  {
   extern const char *Txt_You_must_type_the_stem_of_the_question;
   extern const char *Txt_You_can_not_leave_empty_intermediate_answers;
   extern const char *Txt_You_must_type_at_least_the_first_two_answers;
   extern const char *Txt_The_game_has_been_modified;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   char Query[512 + Cns_MAX_BYTES_TEXT];
   struct Game Game;
   struct GameQuestion GameQst;
   unsigned NumAns;
   char AnsStr[8 + 10 + 1];
   unsigned NumLastAns;
   bool ThereIsEndOfAnswers;
   bool Error = false;

   ***** Initialize new question to zero *****
   Rmt_InitQst (&GameQst);

   ***** Get parameters from form *****
   * Get game code *
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   * Get question code *
   GameQst.QstCod = Rmt_GetParamQstCod ();

   * Get answer type *
   GameQst.AnswerType = (Rmt_AnswerType_t)
	               Par_GetParToUnsignedLong ("AnswerType",
	                                         0,
	                                         Rmt_NUM_ANS_TYPES - 1,
                                                 (unsigned long) Rmt_ANSWER_TYPE_DEFAULT);

   * Get question text *
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);

   * Get the texts of the answers *
   for (NumAns = 0;
	NumAns < Rmt_MAX_ANSWERS_PER_QUESTION;
	NumAns++)
     {
      if (!Rmt_AllocateTextChoiceAnswer (&GameQst,NumAns))
	 Lay_ShowErrorAndExit (Gbl.Alert.Txt);
      sprintf (AnsStr,"AnsStr%u",NumAns);
      Par_GetParToHTML (AnsStr,GameQst.AnsChoice[NumAns].Text,Rmt_MAX_BYTES_ANSWER);
     }

   ***** Make sure that stem and answer are not empty *****
   if (Txt[0])
     {
      if (GameQst.AnsChoice[0].Text[0])	// If the first answer has been filled
        {
         for (NumAns = 0, NumLastAns = 0, ThereIsEndOfAnswers = false;
              !Error && NumAns < Rmt_MAX_ANSWERS_PER_QUESTION;
              NumAns++)
            if (GameQst.AnsChoice[NumAns].Text[0])
              {
               if (ThereIsEndOfAnswers)
                 {
                  Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_empty_intermediate_answers);
                  Error = true;
                 }
               else
                  NumLastAns = NumAns;
              }
            else
               ThereIsEndOfAnswers = true;
         if (!Error)
           {
            if (NumLastAns < 1)
              {
               Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_two_answers);
               Error = true;
              }
           }
        }
      else	// If first answer is empty
        {
         Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_at_least_the_first_two_answers);
         Error = true;
        }
     }
   else
     {
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_type_the_stem_of_the_question);
      Error = true;
     }

   if (Error)
      Tst_ShowFormAskSelectTstsForGame (Game.GamCod);
   else
     {
      ***** Form is received OK ==> insert question and answer in the database *****
      if (GameQst.QstCod < 0)	// It's a new question
        {
         GameQst.QstInd = Rmt_GetNextQuestionIndexInGame (Game.GamCod);

         * Insert question in the table of questions *
         sprintf (Query,"INSERT INTO gam_questions"
                        " (GamCod,QstInd,AnsType,Stem)"
                        " VALUES"
                        " (%ld,%u,'%s','%s')",
	          Game.GamCod,GameQst.QstInd,Rmt_StrAnswerTypesDB[GameQst.AnswerType],Txt);
         GameQst.QstCod = DB_QueryINSERTandReturnCode (Query,"can not create question");
        }
      else			// It's an existing question
        {
         * Update question *
         sprintf (Query,"UPDATE gam_questions SET Stem='%s',AnsType='%s'"
                        " WHERE QstCod=%ld AND GamCod=%ld",
                  Txt,Rmt_StrAnswerTypesDB[GameQst.AnswerType],
                  GameQst.QstCod,Game.GamCod);
         DB_QueryUPDATE (Query,"can not update question");
        }

      * Insert, update or delete answers in the answers table *
      for (NumAns = 0;
	   NumAns < Rmt_MAX_ANSWERS_PER_QUESTION;
	   NumAns++)
         if (Rmt_CheckIfAnswerExists (GameQst.QstCod,NumAns))	// If this answer exists...
           {
            if (GameQst.AnsChoice[NumAns].Text[0])	// Answer is not empty
              {
               * Update answer text *
               sprintf (Query,"UPDATE gam_answers SET Answer='%s'"
                              " WHERE QstCod=%ld AND AnsInd=%u",
                        GameQst.AnsChoice[NumAns].Text,GameQst.QstCod,NumAns);
               DB_QueryUPDATE (Query,"can not update answer");
              }
            else	// Answer is empty
              {
               * Delete answer from database *
               sprintf (Query,"DELETE FROM gam_answers"
                              " WHERE QstCod=%ld AND AnsInd=%u",
                        GameQst.QstCod,NumAns);
               DB_QueryDELETE (Query,"can not delete answer");
              }
           }
         else	// If this answer does not exist...
           {
            if (GameQst.AnsChoice[NumAns].Text[0])	// Answer is not empty
              {
               * Create answer into database *
               sprintf (Query,"INSERT INTO gam_answers"
        	              " (QstCod,AnsInd,NumUsrs,Answer)"
                              " VALUES"
                              " (%ld,%u,0,'%s')",
                        GameQst.QstCod,NumAns,GameQst.AnsChoice[NumAns].Text);
               DB_QueryINSERT (Query,"can not create answer");
              }
           }

      ***** List the questions of this game, including the new one just inserted into the database *****
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_game_has_been_modified);
     }

   ***** Free answers *****
   Rmt_FreeTextChoiceAnswers (&GameQst,Rmt_MAX_ANSWERS_PER_QUESTION);

   ***** Show current game *****
   Rmt_ShowOneGame (Game.GamCod,&GameQst,true);
  }
*/
/*****************************************************************************/
/******************** Get next question index in a game **********************/
/*****************************************************************************/

static unsigned Rmt_GetQstIndFromQstCod (long QstCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned QstInd = 0;

   /***** Get number of games with a field value from database *****/
   sprintf (Query,"SELECT QstInd FROM gam_questions WHERE QstCod=%ld",
            QstCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get question index");

   /***** Get number of users *****/
   if (NumRows)
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&QstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting question index.");
     }
   else
      Lay_ShowErrorAndExit ("Error when getting question index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/******************* Get next question index in a game *********************/
/*****************************************************************************/

static unsigned Rmt_GetNextQuestionIndexInGame (long GamCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned QstInd = 0;

   /***** Get maximum question index in a game from database *****/
   sprintf (Query,"SELECT MAX(QstInd) FROM gam_questions WHERE GamCod=%ld",
            GamCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get last question index");
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are questions
     {
      if (sscanf (row[0],"%u",&QstInd) != 1)
         Lay_ShowErrorAndExit ("Error when getting last question index.");
      QstInd++;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return QstInd;
  }

/*****************************************************************************/
/************************ List the questions of a game *********************/
/*****************************************************************************/

static void Rmt_ListGameQuestions (struct Game *Game,struct GameQuestion *GameQst)
  {
   extern const char *Hlp_ASSESSMENT_Games_questions;
   extern const char *Txt_Questions;
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Type;
   extern const char *Txt_Question;
   extern const char *Txt_SURVEY_STR_ANSWER_TYPES[Rmt_NUM_ANS_TYPES];
   extern const char *Txt_This_game_has_no_questions;
   extern const char *Txt_Done;
   extern const char *Txt_Edit_question;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned NumQst;
   bool Editing = (Gbl.Action.Act == ActEdiOneGam    ||
	           Gbl.Action.Act == ActEdiOneGamQst);
   Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions;

   /***** How to show the questions ******/
   if (Game->Status.ICanAnswer && !Editing)
      ActionToDoWithQuestions = Tst_SHOW_GAME_TO_ANSWER;
   else
      ActionToDoWithQuestions = Tst_SHOW_GAME_RESULT;

   /***** Get data of questions from database *****/
   sprintf (Query,"SELECT gam_questions.QstCod,gam_questions.QstInd,"
	          "tst_questions.AnsType,tst_questions.Stem"
                  " FROM gam_questions,tst_questions"
                  " WHERE gam_questions.GamCod=%ld"
                  " AND gam_questions.QstCod=tst_questions.QstCod"
                  " ORDER BY gam_questions.QstInd",
            Game->GamCod);
   NumQsts = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get data of a question");

   /***** Start box *****/
   Gbl.Games.CurrentGamCod = Game->GamCod;
   Box_StartBox (NULL,Txt_Questions,Game->Status.ICanEdit ? Rmt_PutIconToAddNewQuestions :
                                                            NULL,
                 Hlp_ASSESSMENT_Games_questions,Box_NOT_CLOSABLE);

   if (NumQsts)
     {
      if (ActionToDoWithQuestions == Tst_SHOW_GAME_TO_ANSWER)
	{
	 /***** Start form to send answers to game *****/
	 Act_FormStart (ActAnsGam);
	 Rmt_PutParamGameCod (Game->GamCod);
	}

      /***** Write the heading *****/
      Tbl_StartTableWideMargin (2);
      fprintf (Gbl.F.Out,"<tr>");
      if (Game->Status.ICanEdit)
         fprintf (Gbl.F.Out,"<th colspan=\"2\"></th>");
      fprintf (Gbl.F.Out,"<th class=\"CENTER_TOP\">"
	                 "%s"
	                 "</th>"
                         "<th class=\"CENTER_TOP\">"
                         "%s"
                         "</th>"
                         "<th class=\"LEFT_TOP\">"
                         "%s"
                         "</th>"
                         "</tr>",
               Txt_No_INDEX,
               Txt_Type,
               Txt_Question);

      /***** Write questions one by one *****/
      for (NumQst = 0;
	   NumQst < NumQsts;
	   NumQst++)
        {
         Gbl.RowEvenOdd = (int) (NumQst % 2);

         row = mysql_fetch_row (mysql_res);

         /* row[0] holds the code of the question */
         if (sscanf (row[0],"%ld",&(GameQst->QstCod)) != 1)
            Lay_ShowErrorAndExit ("Wrong code of question.");

         fprintf (Gbl.F.Out,"<tr>");

         if (Game->Status.ICanEdit)
           {
            /* Write icon to remove the question */
            fprintf (Gbl.F.Out,"<td class=\"BT%u\">",Gbl.RowEvenOdd);
            Act_FormStart (ActReqRemGamQst);
            Rmt_PutParamGameCod (Game->GamCod);
            Rmt_PutParamQstCod (GameQst->QstCod);
            Ico_PutIconRemove ();
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>");

            /* Write icon to edit the question */
            fprintf (Gbl.F.Out,"<td class=\"BT%u\">",Gbl.RowEvenOdd);
            Act_FormStart (ActEdiOneGamQst);
            Rmt_PutParamGameCod (Game->GamCod);
            Rmt_PutParamQstCod (GameQst->QstCod);
            fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/edit64x64.png\""
        	               " alt=\"%s\" title=\"%s\""
        	               " class=\"ICO20x20\" />",
                     Gbl.Prefs.IconsURL,
                     Txt_Edit_question,
                     Txt_Edit_question);
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>");
           }

         /* Write index of question inside game (row[1]) */
         if (sscanf (row[1],"%u",&(GameQst->QstInd)) != 1)
            Lay_ShowErrorAndExit ("Error: wrong question index.");
         fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL CENTER_TOP COLOR%u\">"
                            "%u"
                            "</td>",
                  Gbl.RowEvenOdd,GameQst->QstInd + 1);

         /* Write the question type (row[2]) */
         GameQst->AnswerType = Rmt_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
         fprintf (Gbl.F.Out,"<td class=\"DAT_SMALL CENTER_TOP COLOR%u\">"
                            "%s"
                            "</td>",
	          Gbl.RowEvenOdd,
                  Txt_SURVEY_STR_ANSWER_TYPES[GameQst->AnswerType]);

         /* Write the stem (row[3]) and the answers of this question */
         fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP COLOR%u\">",
	          Gbl.RowEvenOdd);
         // Rmt_WriteQstStem (row[3]);
         Rmt_WriteAnswersOfAQst (ActionToDoWithQuestions,Game,GameQst);
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      Tbl_EndTable ();

      if (ActionToDoWithQuestions == Tst_SHOW_GAME_TO_ANSWER)
	{
	 /***** Button to create/modify game *****/
	 Btn_PutConfirmButton (Txt_Done);

	 /***** End form *****/
	 Act_FormEnd ();
	}
     }
   else	// This game has no questions
      Ale_ShowAlert (Ale_INFO,Txt_This_game_has_no_questions);

   if (Game->Status.ICanEdit &&	// I can edit
       (!NumQsts ||		// This game has no questions
	Editing))		// I am editing
      /***** Put button to add a new question in this game *****/
      Rmt_PutButtonToAddNewQuestions ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to game *******************/
/*****************************************************************************/

static void Rmt_PutIconToAddNewQuestions (void)
  {
   extern const char *Txt_Add_questions;

   /***** Put form to create a new question *****/
   Lay_PutContextualLink (ActEdiOneGamQst,NULL,Rmt_PutParams,
                          "plus64x64.png",
                          Txt_Add_questions,NULL,
		          NULL);
  }

/*****************************************************************************/
/***************** Put button to add new questions to game *******************/
/*****************************************************************************/

static void Rmt_PutButtonToAddNewQuestions (void)
  {
   extern const char *Txt_Add_questions;

   Act_FormStart (ActEdiOneGamQst);
   Rmt_PutParams ();
   Btn_PutConfirmButton (Txt_Add_questions);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Add selected test questions to game ********************/
/*****************************************************************************/

void Rmt_AddTstQuestionsToGame (void)
  {
   extern const char *Txt_You_must_select_one_ore_more_questions;
   struct Game Game;
   const char *Ptr;
   char LongStr[1 + 10 + 1];
   struct GameQuestion GameQst;
   char Query[256];

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get selected questions *****/
   /* Allocate space for selected question codes */
   Rmt_AllocateListSelectedQuestions ();

   /* Get question codes */
   Par_GetParMultiToText ("QstCods",Gbl.Games.ListQuestions,
                          Rmt_MAX_BYTES_LIST_SELECTED_QUESTIONS);

   /* Check number of questions */
   if (Rmt_CountNumQuestionsInList () == 0)	// If no questions selected...
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
      if (sscanf (LongStr,"%ld",&GameQst.QstCod) != 1)
         Lay_ShowErrorAndExit ("Wrong question code.");

      /* Get next index */
      GameQst.QstInd = Rmt_GetNextQuestionIndexInGame (Game.GamCod);

      /* Insert question in the table of questions */
      sprintf (Query,"INSERT INTO gam_questions"
		     " (GamCod,QstCod,QstInd)"
		     " VALUES"
		     " (%ld,%ld,%u)",
	       Game.GamCod,GameQst.QstCod,GameQst.QstInd);
      DB_QueryINSERT (Query,"can not create question");
     }

   /***** Free space for selected question codes *****/
   Rmt_FreeListsSelectedQuestions ();
  }

/*****************************************************************************/
/****************** Allocate memory for list of questions ********************/
/*****************************************************************************/

static void Rmt_AllocateListSelectedQuestions (void)
  {
   if (!Gbl.Games.ListQuestions)
     {
      if ((Gbl.Games.ListQuestions = (char *) malloc (Rmt_MAX_BYTES_LIST_SELECTED_QUESTIONS + 1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store list of questions.");
      Gbl.Games.ListQuestions[0] = '\0';
     }
  }

/*****************************************************************************/
/*********** Free memory used by list of selected question codes *************/
/*****************************************************************************/

static void Rmt_FreeListsSelectedQuestions (void)
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

static unsigned Rmt_CountNumQuestionsInList (void)
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
/****************** Write the heading of a game question *******************/
/*****************************************************************************/
/*
static void Rmt_WriteQstStem (const char *Stem)
  {
   char *HeadingRigorousHTML;
   size_t Length;

   * Convert the stem, that is in HTML, to rigorous HTML *
   Length = strlen (Stem) * Str_MAX_BYTES_PER_CHAR;
   if ((HeadingRigorousHTML = malloc (Length + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store stem of question.");
   Str_Copy (HeadingRigorousHTML,Stem,
             Length);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     HeadingRigorousHTML,Length,false);

   * Write the stem *
   fprintf (Gbl.F.Out,"%s",HeadingRigorousHTML);

   * Free memory allocated for the stem *
   free ((void *) HeadingRigorousHTML);
  }
*/
/*****************************************************************************/
/************** Get and write the answers of a game question ***************/
/*****************************************************************************/

static void Rmt_WriteAnswersOfAQst (Tst_ActionToDoWithQuestions_t ActionToDoWithQuestions,
                                    struct Game *Game,struct GameQuestion *GameQst)
  {
   extern const char *Txt_Question_removed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   double ScoreThisQst;
   bool AnswerIsNotBlank;

   /***** Query database *****/
   if (Tst_GetOneQuestionByCod (GameQst->QstCod,&mysql_res))	// Question exists
     {
      /***** Get row of the result of the query *****/
      row = mysql_fetch_row (mysql_res);
      /*
      row[ 0] QstCod
      row[ 1] UNIX_TIMESTAMP(EditTime)
      row[ 2] AnsType
      row[ 3] Shuffle
      row[ 4] Stem
      row[ 5] Feedback
      row[ 6] ImageName
      row[ 7] ImageTitle
      row[ 8] ImageURL
      row[ 9] NumHits
      row[10] NumHitsNotBlank
      row[11] Score
      */

      /***** Write question and answers *****/
      Gbl.Games.CurrentGamCod = Game->GamCod;
      Tst_WriteQstAndAnsTest (ActionToDoWithQuestions,
			      GameQst->QstInd,GameQst->QstCod,row,
			      &ScoreThisQst,&AnswerIsNotBlank);
     }
   else
      /***** Question does not exists *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"TEST_NUM_QST RIGHT_TOP COLOR%u\">"
			 "%u"
			 "</td>"
			 "<td class=\"DAT_LIGHT LEFT_TOP COLOR%u\">"
			 "%s"
			 "</td>"
			 "</tr>",
	       Gbl.RowEvenOdd,GameQst->QstInd + 1,
	       Gbl.RowEvenOdd,Txt_Question_removed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);



/*








   unsigned NumAnswers;
   unsigned NumAns;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrsThisAnswer;

   ***** Get answers of this question *****
   NumAnswers = Rmt_GetNumUsrsWhoAnswered (GameQst->QstCod,&mysql_res);	// Result: AnsInd,NumUsrs,Answer

   ***** Write the answers *****
   if (NumAnswers)
     {
      * Check number of answers *
      if (NumAnswers > Rmt_MAX_ANSWERS_PER_QUESTION)
	 Lay_ShowErrorAndExit ("Wrong number of answers.");

      * Write one row for each answer *
      Tbl_StartTable (5);
      for (NumAns = 0;
	   NumAns < NumAnswers;
	   NumAns++)
	{
	 row = mysql_fetch_row (mysql_res);

	 * Get number of users who have marked this answer (row[1]) *
	 if (sscanf (row[1],"%u",&NumUsrsThisAnswer) != 1)
	    Lay_ShowErrorAndExit ("Error when getting number of users who have marked an answer.");

	 * Convert the answer (row[2]), that is in HTML, to rigorous HTML *
	 if (!Rmt_AllocateTextChoiceAnswer (GameQst,NumAns))
            Lay_ShowErrorAndExit (Gbl.Alert.Txt);
	 Str_Copy (GameQst->AnsChoice[NumAns].Text,row[2],
	           Rmt_MAX_BYTES_ANSWER);
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   GameQst->AnsChoice[NumAns].Text,Rmt_MAX_BYTES_ANSWER,false);

	 * Selectors and label with the letter of the answer *
	 fprintf (Gbl.F.Out,"<tr>");

	 if (PutFormAnswerGame)
	   {
	    * Write selector to choice this answer *
	    fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">"
			       "<input type=\"");
	    if (GameQst->AnswerType == Rmt_ANS_UNIQUE_CHOICE)
	       fprintf (Gbl.F.Out,"radio\""
				  " onclick=\"selectUnselectRadio(this,this.form.Ans%010u,%u)\"",
			(unsigned) GameQst->QstCod,NumAnswers);
	    else // GameQst->AnswerType == Rmt_ANS_MULTIPLE_CHOICE
	       fprintf (Gbl.F.Out,"checkbox\"");
	    fprintf (Gbl.F.Out," id=\"Ans%010u_%010u\" name=\"Ans%010u\""
		               " value=\"%u\" />"
			       "</td>",
		     (unsigned) GameQst->QstCod,NumAns,(unsigned) GameQst->QstCod,
		     NumAns);
	   }

	 * Write the number of option *
	 fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\" style=\"width:50px;\">"
			    "<label for=\"Ans%010u_%010u\" class=\"DAT\">"
			    "%u)"
			    "</label>"
			    "</td>",
		  (unsigned) GameQst->QstCod,NumAns,NumAns + 1);

	 * Write the text of the answer *
	 fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">"
			    "<label for=\"Ans%010u_%010u\" class=\"DAT\">%s</label>"
	                    "</td>",
		  (unsigned) GameQst->QstCod,NumAns,
		  GameQst->AnsChoice[NumAns].Text);

	 * Show stats of this answer *
	 if (Game->Status.ICanViewResults)
	    Rmt_DrawBarNumUsrs (NumUsrsThisAnswer,Game->NumUsrs);

	 fprintf (Gbl.F.Out,"</tr>");

	 * Free memory allocated for the answer *
	 Rmt_FreeTextChoiceAnswer (GameQst,NumAns);
	}
      Tbl_EndTable ();
     }

   ***** Free structure that stores the query result *****
   DB_FreeMySQLResult (&mysql_res);
*/
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

#define Rmt_MAX_BAR_WIDTH 125

static void Rmt_DrawBarNumUsrs (unsigned NumUsrs,unsigned MaxUsrs)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned BarWidth = 0;

   /***** String with the number of users *****/
   if (MaxUsrs)
      sprintf (Gbl.Title,"%u&nbsp;(%u%%&nbsp;%s&nbsp;%u)",
               NumUsrs,
               (unsigned) ((((float) NumUsrs * 100.0) / (float) MaxUsrs) + 0.5),
               Txt_of_PART_OF_A_TOTAL,MaxUsrs);
   else
      sprintf (Gbl.Title,"0&nbsp;(0%%&nbsp;%s&nbsp;%u)",
               Txt_of_PART_OF_A_TOTAL,MaxUsrs);

   /***** Draw bar with a with proportional to the number of clicks *****/
   fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_TOP\" style=\"width:%upx;\">",
            Rmt_MAX_BAR_WIDTH + 125);
   if (NumUsrs && MaxUsrs)
      BarWidth = (unsigned) ((((float) NumUsrs * (float) Rmt_MAX_BAR_WIDTH) /
	                       (float) MaxUsrs) + 0.5);
   if (BarWidth < 2)
      BarWidth = 2;
   fprintf (Gbl.F.Out,"<img src=\"%s/c1x16.gif\""
	              " alt=\"%s\" title=\"%s\""
                      " class=\"LEFT_TOP\""
	              " style=\"width:%upx; height:20px;\" />"
	              "&nbsp;",
      Gbl.Prefs.IconsURL,
      Gbl.Title,
      Gbl.Title,
      BarWidth);

   /***** Write the number of users *****/
   fprintf (Gbl.F.Out,"%s</td>"
	              "</tr>",
            Gbl.Title);
  }

/*****************************************************************************/
/********************* Put icon to remove one question ***********************/
/*****************************************************************************/
/*
static void Rmt_PutIconToRemoveOneQst (void)
  {
   Ico_PutContextualIconToRemove (ActReqRemGamQst,Rmt_PutParamsRemoveOneQst);
  }
*/
/*****************************************************************************/
/****************** Put parameter to remove one question *********************/
/*****************************************************************************/

static void Rmt_PutParamsRemoveOneQst (void)
  {
   Rmt_PutParamGameCod (Gbl.Games.CurrentGamCod);
   Rmt_PutParamQstCod (Gbl.Games.CurrentQstCod);
  }

/*****************************************************************************/
/********************** Request the removal of a question ********************/
/*****************************************************************************/

void Rmt_RequestRemoveQst (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   extern const char *Txt_Remove_question;
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get parameters from form *****/
   /* Get game code */
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question code */
   if ((GameQst.QstCod = Rmt_GetParamQstCod ()) < 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   GameQst.QstInd = Rmt_GetQstIndFromQstCod (GameQst.QstCod);

   /***** Show question and button to remove question *****/
   Gbl.Games.CurrentGamCod = Game.GamCod;
   Gbl.Games.CurrentQstCod = GameQst.QstCod;
   sprintf (Gbl.Alert.Txt,Txt_Do_you_really_want_to_remove_the_question_X,
	    (unsigned long) (GameQst.QstInd + 1));
   Ale_ShowAlertAndButton (Ale_QUESTION,Gbl.Alert.Txt,
                           ActRemGamQst,NULL,NULL,Rmt_PutParamsRemoveOneQst,
			   Btn_REMOVE_BUTTON,Txt_Remove_question);

   /***** Show current game *****/
   Rmt_ShowOneGame (Game.GamCod,&GameQst,true);
  }

/*****************************************************************************/
/****************************** Remove a question ****************************/
/*****************************************************************************/

void Rmt_RemoveQst (void)
  {
   extern const char *Txt_Question_removed;
   char Query[512];
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get parameters from form *****/
   /* Get game code */
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /* Get question code */
   if ((GameQst.QstCod = Rmt_GetParamQstCod ()) < 0)
      Lay_ShowErrorAndExit ("Wrong code of question.");

   /* Get question index */
   GameQst.QstInd = Rmt_GetQstIndFromQstCod (GameQst.QstCod);

   /***** Remove the question from all the tables *****/
   /* Remove answers from this test question */
   Rmt_RemAnswersOfAQuestion (GameQst.QstCod);

   /* Remove the question itself */
   sprintf (Query,"DELETE FROM gam_questions WHERE QstCod=%ld",
            GameQst.QstCod);
   DB_QueryDELETE (Query,"can not remove a question");
   if (!mysql_affected_rows (&Gbl.mysql))
      Lay_ShowErrorAndExit ("The question to be removed does not exist.");

   /* Change index of questions greater than this */
   sprintf (Query,"UPDATE gam_questions SET QstInd=QstInd-1"
                  " WHERE GamCod=%ld AND QstInd>%u",
            Game.GamCod,GameQst.QstInd);
   DB_QueryUPDATE (Query,"can not update indexes of questions");

   /***** Write message *****/
   sprintf (Gbl.Alert.Txt,"%s",Txt_Question_removed);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show current game *****/
   Rmt_ShowOneGame (Game.GamCod,&GameQst,true);
  }

/*****************************************************************************/
/************************ Receive answers of a game ************************/
/*****************************************************************************/

void Rmt_ReceiveGameAnswers (void)
  {
   extern const char *Txt_You_already_played_this_game_before;
   extern const char *Txt_Thanks_for_playing_the_game;
   struct Game Game;
   struct GameQuestion GameQst;

   /***** Get game code *****/
   if ((Game.GamCod = Rmt_GetParamGameCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");

   /***** Get data of the game from database *****/
   Rmt_GetDataOfGameByCod (&Game);

   /***** Check if I have no answered this game formerly *****/
   if (Game.Status.IHaveAnswered)
      Ale_ShowAlert (Ale_WARNING,Txt_You_already_played_this_game_before);
   else
     {
      /***** Receive and store user's answers *****/
      Rmt_ReceiveAndStoreUserAnswersToAGame (Game.GamCod);
      Ale_ShowAlert (Ale_INFO,Txt_Thanks_for_playing_the_game);
     }

   /***** Show current game *****/
   Rmt_ShowOneGame (Game.GamCod,&GameQst,true);
  }

/*****************************************************************************/
/**************** Get and store user's answers to a game *******************/
/*****************************************************************************/

static void Rmt_ReceiveAndStoreUserAnswersToAGame (long GamCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   unsigned NumQsts;
   long QstCod;
   char ParamName[3 + 10 + 6 + 1];
   char StrAnswersIndexes[Rmt_MAX_ANSWERS_PER_QUESTION * (10 + 1)];
   const char *Ptr;
   char UnsignedStr[10 + 1];
   unsigned AnsInd;

   /***** Get questions of this game from database *****/
   sprintf (Query,"SELECT QstCod FROM gam_questions"
                  " WHERE GamCod=%ld ORDER BY QstCod",
            GamCod);
   DB_QuerySELECT (Query,&mysql_res,"can not get questions of a game");

   if ((NumQsts = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get games"))) // The game has questions
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
         sprintf (ParamName,"Ans%010u",(unsigned) QstCod);
         // Lay_ShowAlert (Lay_INFO,ParamName);
         Par_GetParMultiToText (ParamName,StrAnswersIndexes,
                                Rmt_MAX_ANSWERS_PER_QUESTION * (10 + 1));
         Ptr = StrAnswersIndexes;
         while (*Ptr)
           {
            Par_GetNextStrUntilSeparParamMult (&Ptr,UnsignedStr,10);
            if (sscanf (UnsignedStr,"%u",&AnsInd) == 1)
               // Parameter exists, so user has marked this answer, so store it in database
               Rmt_IncreaseAnswerInDB (QstCod,AnsInd);
           }
        }
     }
   else		// The game has no questions and answers
      Lay_ShowErrorAndExit ("Error: this game has no questions.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Register that you have answered this game *****/
   Rmt_RegisterIHaveAnsweredGame (GamCod);
  }

/*****************************************************************************/
/************ Increase number of users who have marked one answer ************/
/*****************************************************************************/

static void Rmt_IncreaseAnswerInDB (long QstCod,unsigned AnsInd)
  {
   char Query[256];

   /***** Increase number of users who have selected
          the answer AnsInd in the question QstCod *****/
   sprintf (Query,"UPDATE gam_answers SET NumUsrs=NumUsrs+1"
                  " WHERE QstCod=%ld AND AnsInd=%u",
            QstCod,AnsInd);
   DB_QueryINSERT (Query,"can not register your answer to the game");
  }

/*****************************************************************************/
/******************* Register that I have answered a game ********************/
/*****************************************************************************/

static void Rmt_RegisterIHaveAnsweredGame (long GamCod)
  {
   char Query[256];

   sprintf (Query,"INSERT INTO gam_users"
	          " (GamCod,UsrCod)"
                  " VALUES"
                  " (%ld,%ld)",
            GamCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryINSERT (Query,"can not register that you have answered the game");
  }

/*****************************************************************************/
/******************** Check if I have answered a game ************************/
/*****************************************************************************/

static bool Rmt_CheckIfIHaveAnsweredGame (long GamCod)
  {
   char Query[256];

   /***** Get number of games with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM gam_users"
                  " WHERE GamCod=%ld AND UsrCod=%ld",
            GamCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (DB_QueryCOUNT (Query,"can not check if you have answered a game") != 0);
  }

/*****************************************************************************/
/************** Get number of users who have answered a game *****************/
/*****************************************************************************/

static unsigned Rmt_GetNumUsrsWhoHaveAnsweredGame (long GamCod)
  {
   char Query[128];

   /***** Get number of games with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM gam_users WHERE GamCod=%ld",
            GamCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of users who have answered a game");
  }

/*****************************************************************************/
/********************* Get number of courses with games **********************/
/*****************************************************************************/
// Returns the number of courses with games for courses
// in this location (all the platform, current degree or current course)

unsigned Rmt_GetNumCoursesWithCrsGames (Sco_Scope_t Scope)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with games from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(DISTINCT Cod)"
                        " FROM games"
                        " WHERE Scope='%s'",
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT games.Cod)"
                        " FROM institutions,centres,degrees,courses,games"
			" WHERE institutions.CtyCod=%ld"
			" AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT games.Cod)"
                        " FROM centres,degrees,courses,games"
                        " WHERE centres.InsCod=%ld"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT games.Cod)"
                        " FROM degrees,courses,games"
                        " WHERE degrees.CtrCod=%ld"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT games.Cod)"
                        " FROM courses,games"
                        " WHERE courses.DegCod=%ld"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT Cod)"
                        " FROM games"
                        " WHERE Scope='%s' AND Cod=%ld",
                  Sco_ScopeDB[Sco_SCOPE_CRS],
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of courses with games");

   /***** Get number of games *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with games.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/********************* Get number of games for courses ***********************/
/*****************************************************************************/
// Returns the number of games for courses
// in this location (all the platform, current degree or current course)

unsigned Rmt_GetNumCrsGames (Sco_Scope_t Scope,unsigned *NumNotif)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGames;

   /***** Get number of games from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM games"
                        " WHERE Scope='%s'",
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*),SUM(games.NumNotif)"
                        " FROM institutions,centres,degrees,courses,games"
                        " WHERE institutions.CtyCod=%ld"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentCty.Cty.CtyCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*),SUM(games.NumNotif)"
                        " FROM centres,degrees,courses,games"
                        " WHERE centres.InsCod=%ld"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*),SUM(games.NumNotif)"
                        " FROM degrees,courses,games"
                        " WHERE degrees.CtrCod=%ld"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*),SUM(games.NumNotif)"
                        " FROM courses,games"
                        " WHERE courses.DegCod=%ld"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*),SUM(NumNotif)"
                        " FROM games"
                        " WHERE games.Scope='%s'"
                        " AND CrsCod=%ld",
                  Sco_ScopeDB[Sco_SCOPE_CRS],
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of games");

   /***** Get number of games *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumGames) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of games.");

   /***** Get number of notifications by email *****/
   if (row[1])
     {
      if (sscanf (row[1],"%u",NumNotif) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications of games.");
     }
   else
      *NumNotif = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumGames;
  }

/*****************************************************************************/
/************* Get average number of questions per course game ***************/
/*****************************************************************************/

float Rmt_GetNumQstsPerCrsGame (Sco_Scope_t Scope)
  {
   extern const char *Sco_ScopeDB[Sco_NUM_SCOPES];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   float NumQstsPerGame;

   /***** Get number of questions per game from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
                        " FROM games,gam_questions"
                        " WHERE games.Scope='%s'"
                        " AND games.GamCod=gam_questions.GamCod"
                        " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
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
                  Gbl.CurrentCty.Cty.CtyCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
                        " FROM centres,degrees,courses,games,gam_questions"
                        " WHERE centres.InsCod=%ld"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'"
                        " AND games.GamCod=gam_questions.GamCod"
                        " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                  Gbl.CurrentIns.Ins.InsCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
                        " FROM degrees,courses,games,gam_questions"
                        " WHERE degrees.CtrCod=%ld"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'"
                        " AND games.GamCod=gam_questions.GamCod"
                        " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
                        " FROM courses,games,gam_questions"
                        " WHERE courses.DegCod=%ld"
                        " AND courses.CrsCod=games.Cod"
                        " AND games.Scope='%s'"
                        " AND games.GamCod=gam_questions.GamCod"
                        " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                  Gbl.CurrentDeg.Deg.DegCod,
                  Sco_ScopeDB[Sco_SCOPE_CRS]);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT AVG(NumQsts) FROM"
                        " (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
                        " FROM games,gam_questions"
                        " WHERE games.Scope='%s' AND games.Cod=%ld"
                        " AND games.GamCod=gam_questions.GamCod"
                        " GROUP BY gam_questions.GamCod) AS NumQstsTable",
                  Sco_ScopeDB[Sco_SCOPE_CRS],Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of questions per game");

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   NumQstsPerGame = Str_GetFloatNumFromStr (row[0]);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumQstsPerGame;
  }
