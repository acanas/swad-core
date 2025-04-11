// swad_match.c: matches in games using remote control

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_game_database.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_HTML.h"
#include "swad_match.h"
#include "swad_match_database.h"
#include "swad_match_result.h"
#include "swad_parameter_code.h"
#include "swad_question_database.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_test.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Mch_ICON_CLOSE		"fas fa-times"
#define Mch_ICON_PLAY		"fas fa-play"
#define Mch_ICON_PAUSE		"fas fa-pause"
#define Mch_ICON_PREVIOUS	"fas fa-step-backward"
#define Mch_ICON_NEXT		"fas fa-step-forward"
#define Mch_ICON_RESULTS	"fas fa-chart-bar"

#define Mch_COUNTDOWN_SECONDS_LARGE  60
#define Mch_COUNTDOWN_SECONDS_MEDIUM 30
#define Mch_COUNTDOWN_SECONDS_SMALL  10

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   Mch_CHANGE_STATUS_BY_STUDENT,
   Mch_REFRESH_STATUS_BY_SERVER,
  } Mch_Update_t;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

long Mch_MchCodBeingPlayed;	// Used to refresh game via AJAX

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mch_SetMchCodBeingPlayed (long MchCod);

static void Mch_PutIconsInListOfMatches (void *Games);
static void Mch_PutIconToCreateNewMatch (struct Gam_Games *Games);

static void Mch_ListOneOrMoreMatches (struct Gam_Games *Games,
      				      Frm_PutForm_t PutFormMatch,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res);
static void Mch_ListOneOrMoreMatchesHeading (Usr_Can_t ICanEditMatches);
static Usr_Can_t Mch_CheckIfICanEditMatches (void);
static Usr_Can_t Mch_CheckIfICanEditThisMatch (const struct Mch_Match *Match);
static Usr_Can_t Mch_CheckIfICanChangeVisibilityOfResults (const struct Mch_Match *Match);
static void Mch_ListOneOrMoreMatchesIcons (struct Gam_Games *Games,
                                           const struct Mch_Match *Match,
                                           const char *Anchor);
static void Mch_ListOneOrMoreMatchesAuthor (const struct Gam_Games *Games,
				            const struct Mch_Match *Match);
static void Mch_ListOneOrMoreMatchesTimes (const struct Gam_Games *Games,
					   const struct Mch_Match *Match,
					   unsigned UniqueId);
static void Mch_ListOneOrMoreMatchesTitleGrps (const struct Gam_Games *Games,
					       const struct Mch_Match *Match,
                                               const char *Anchor);
static void Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (const struct Gam_Games *Games,
							 const struct Mch_Match *Match);
static void Mch_ListOneOrMoreMatchesNumPlayers (const struct Gam_Games *Games,
					        const struct Mch_Match *Match);
static void Mch_ListOneOrMoreMatchesStatus (const struct Gam_Games *Games,
					    struct Mch_Match *Match,
					    unsigned NumQsts);
static void Mch_PutParMchCod (void *MchCod);
static void Mch_ListOneOrMoreMatchesResult (struct Gam_Games *Games,
                                            const struct Mch_Match *Match);
static void Mch_ListOneOrMoreMatchesResultStd (struct Gam_Games *Games,
                                               const struct Mch_Match *Match);
static void Mch_ListOneOrMoreMatchesResultTch (struct Gam_Games *Games,
                                               const struct Mch_Match *Match);

static void Mch_GetMatchDataFromRow (MYSQL_RES *mysql_res,
				     struct Mch_Match *Match);

static void Mch_PutFormMatch (struct Mch_Match *Match);
static void Mch_ParsFormMatch (void *Match);
static void Mch_ShowLstGrpsToEditMatch (long MchCod);
static void Mch_UpdateMatchTitleAndGrps (const struct Mch_Match *Match);

static long Mch_CreateMatch (long GamCod,char Title[Mch_MAX_BYTES_TITLE + 1]);
static void Mch_CreateIndexes (long GamCod,long MchCod);
static void Mch_ReorderAnswer (long MchCod,unsigned QstInd,
			       const struct Qst_Question *Question);
static void Mch_CreateGrps (long MchCod);
static void Mch_UpdateMatchStatusInDB (const struct Mch_Match *Match);

static void Mch_UpdateElapsedTimeInQuestion (const struct Mch_Match *Match);
static void Mch_GetElapsedTimeInQuestion (const struct Mch_Match *Match,
				          struct Dat_Time *Time);
static void Mch_GetElapsedTimeInMatch (const struct Mch_Match *Match,
				       struct Dat_Time *Time);
static void Mch_GetElapsedTime (unsigned NumRows,MYSQL_RES *mysql_res,
				struct Dat_Time *Time);

static void Mch_SetMatchStatusToPrev (struct Mch_Match *Match);
static void Mch_SetMatchStatusToPrevQst (struct Mch_Match *Match);
static void Mch_SetMatchStatusToStart (struct Mch_Match *Match);

static void Mch_SetMatchStatusToNext (struct Mch_Match *Match);
static void Mch_SetMatchStatusToNextQst (struct Mch_Match *Match);
static void Mch_SetMatchStatusToEnd (struct Mch_Match *Match);

static void Mch_ShowMatchStatusForTch (struct Mch_Match *Match);
static void Mch_ShowMatchStatusForStd (struct Mch_Match *Match,Mch_Update_t Update);

static void Mch_ShowLeftColumnTch (struct Mch_Match *Match);
static void Mch_ShowRefreshablePartTch (struct Mch_Match *Match);
static void Mch_WriteElapsedTimeInMch (struct Mch_Match *Match);
static void Mch_WriteElapsedTimeInQst (struct Mch_Match *Match);
static void Mch_WriteNumRespondersQst (struct Mch_Match *Match);
static void Mch_PutFormCountdown (struct Mch_Match *Match,long Seconds,const char *Color);
static void Mch_PutCountdownAndHourglassIcon (struct Mch_Match *Match);
static void Mch_PutFormsCountdown (struct Mch_Match *Match);

static void Mch_ShowRightColumnTch (const struct Mch_Match *Match);
static void Mch_ShowLeftColumnStd (const struct Mch_Match *Match,
				   const struct Mch_UsrAnswer *UsrAnswer);
static void Mch_ShowRightColumnStd (struct Mch_Match *Match,
				    const struct Mch_UsrAnswer *UsrAnswer,
				    Mch_Update_t Update);

static void Mch_ShowNumQstInMch (const struct Mch_Match *Match);
static void Mch_PutMatchControlButtons (const struct Mch_Match *Match);
static void Mch_ShowFormColumns (const struct Mch_Match *Match);
static void Mch_PutParNumCols (unsigned NumCols);

static void Mch_ShowMatchTitleTch (const struct Mch_Match *Match);
static void Mch_ShowMatchTitleStd (const struct Mch_Match *Match);

static void Mch_PutCheckboxResult (const struct Mch_Match *Match);
static void Mch_PutIfAnswered (const struct Mch_Match *Match,bool Answered);
static void Mch_PutIconToRemoveMyAnswer (const struct Mch_Match *Match);
static void Mch_ShowQuestionAndAnswersTch (const struct Mch_Match *Match);
static void Mch_WriteAnswersMatchResult (const struct Mch_Match *Match,
                                         struct Qst_Question *Question,
                                         const char *Class,bool ShowResult);
static void Mch_WriteChoiceAnsViewMatch (const struct Mch_Match *Match,
                                         struct Qst_Question *Question,
                                         const char *Class,bool ShowResult);
static void Mch_ShowQuestionAndAnswersStd (const struct Mch_Match *Match,
					   const struct Mch_UsrAnswer *UsrAnswer,
					   Mch_Update_t Update);

static void Mch_ShowMatchScore (const struct Mch_Match *Match);
static void Mch_DrawEmptyScoreRow (unsigned NumRow,double MinScore,double MaxScore);
static void Mch_DrawScoreRow (double Score,double MinScore,double MaxScore,
			      unsigned NumRow,unsigned NumUsrs,unsigned MaxUsrs);
static const char *Mch_GetClassBorder (unsigned NumRow);

static void Mch_PutParNumOpt (unsigned NumOpt);
static unsigned Mch_GetParNumOpt (void);

static void Mch_PutBigButton (Act_Action_t NextAction,const char *Id,
			      long MchCod,const char *Icon,const char *Txt);
static void Mch_PutBigButtonHidden (const char *Icon);
// static void Mch_PutBigButtonClose (void);

static void Mch_ShowWaitImage (const char *Txt);

/*****************************************************************************/
/*************** Set/Get match code of the match being played ****************/
/*****************************************************************************/

static void Mch_SetMchCodBeingPlayed (long MchCod)
  {
   Mch_MchCodBeingPlayed = MchCod;
  }

long Mch_GetMchCodBeingPlayed (void)
  {
   return Mch_MchCodBeingPlayed;
  }

/*****************************************************************************/
/********************************* Reset match *******************************/
/*****************************************************************************/

void Mch_ResetMatch (struct Mch_Match *Match)
  {
   Dat_StartEndTime_t StartEndTime;

   /***** Initialize to empty match *****/
   Match->MchCod                  = -1L;
   Match->GamCod                  = -1L;
   Match->UsrCod                  = -1L;
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Match->TimeUTC[StartEndTime] = (time_t) 0;
   Match->Title[0]                = '\0';
   Match->Status.QstInd           = 0;
   Match->Status.QstCod           = -1L;
   Match->Status.QstStartTimeUTC  = (time_t) 0;
   Match->Status.Showing          = Mch_SHOWING_DEFAULT;
   Match->Status.Countdown        = 0;
   Match->Status.NumCols          = 1;
   Match->Status.ShowQstResults   = false;
   Match->Status.ShowUsrResults   = false;
   Match->Status.Playing          = false;
   Match->Status.NumPlayers       = 0;
  };

/*****************************************************************************/
/************************* List the matches of a game ************************/
/*****************************************************************************/

void Mch_ListMatches (struct Gam_Games *Games,Frm_PutForm_t PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games_matches;
   extern const char *Txt_Matches;
   MYSQL_RES *mysql_res;
   unsigned NumMatches;

   /***** Get data of matches from database *****/
   NumMatches = Mch_DB_GetMatchesInGame (&mysql_res,Games->Game.GamCod);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Matches,Mch_PutIconsInListOfMatches,Games,
		    Hlp_ASSESSMENT_Games_matches,Box_NOT_CLOSABLE);

	 /***** Select whether show only my groups or all groups *****/
	 if (Gbl.Crs.Grps.NumGrps && Mch_CheckIfICanEditMatches () == Usr_CAN)
	   {
	    Set_BeginSettingsHead ();
	       Grp_ShowFormToSelMyAllGrps (ActSeeOneGam,Gam_PutPars,Games);
	    Set_EndSettingsHead ();
	   }

	 /***** Show the table with the matches *****/
	 Mch_ListOneOrMoreMatches (Games,PutFormNewMatch,NumMatches,mysql_res);

      /***** End box *****/
      Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get match data using its code ************************/
/*****************************************************************************/

void Mch_GetMatchDataByCod (struct Mch_Match *Match)
  {
   MYSQL_RES *mysql_res;

   /***** Get data of match from database *****/
   if (Mch_DB_GetMatchDataByCod (&mysql_res,Match->MchCod)) // Match found...
      /* Get match data from row */
      Mch_GetMatchDataFromRow (mysql_res,Match);
   else
      /* Initialize to empty match */
      Mch_ResetMatch (Match);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Put icons in list of matches of a game *******************/
/*****************************************************************************/

static void Mch_PutIconsInListOfMatches (void *Games)
  {
   if (Games)
      if (Mch_CheckIfICanEditMatches () == Usr_CAN)
         /***** Put icon to create a new match in current game *****/
	 Mch_PutIconToCreateNewMatch ((struct Gam_Games *) Games);
  }

/*****************************************************************************/
/********************* Put icon to create a new match ************************/
/*****************************************************************************/

static void Mch_PutIconToCreateNewMatch (struct Gam_Games *Games)
  {
   Ico_PutContextualIconToAdd (ActReqNewMch,Mch_NEW_MATCH_SECTION_ID,
                               Gam_PutPars,Games);
  }

/*****************************************************************************/
/*********************** List game matches for edition ***********************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatches (struct Gam_Games *Games,
      				      Frm_PutForm_t PutFormMatch,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res)
  {
   unsigned NumMatch;
   unsigned UniqueId;
   struct Mch_Match Match;
   char *Anchor;
   Usr_Can_t ICanEditMatches = Mch_CheckIfICanEditMatches ();
   long MchCodToBeEdited = PutFormMatch == Frm_PUT_FORM &&
			   Games->MchCod > 0 ? Games->MchCod :
					       -1L;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write the heading *****/
      if (NumMatches)
	 Mch_ListOneOrMoreMatchesHeading (ICanEditMatches);

      /***** Write rows *****/
      for (NumMatch = 0, UniqueId = 1, The_ResetRowColor ();
	   NumMatch < NumMatches;
	   NumMatch++, UniqueId++, The_ChangeRowColor ())
	{
	 /***** Get match data from row *****/
	 Mch_GetMatchDataFromRow (mysql_res,&Match);

	 if (Mch_CheckIfICanPlayThisMatchBasedOnGrps (&Match) == Usr_CAN)
	   {
	    /***** Build anchor string *****/
	    if (asprintf (&Anchor,"mch_%ld",Match.MchCod) < 0)
	       Err_NotEnoughMemoryExit ();

	    /***** First row for this match with match data ****/
	    HTM_TR_Begin (NULL);

	       /* Icons */
	       if (ICanEditMatches == Usr_CAN)
		  Mch_ListOneOrMoreMatchesIcons (Games,&Match,Anchor);

	       /* Start/end date/time */
	       Mch_ListOneOrMoreMatchesTimes (Games,&Match,UniqueId);

	       /* Title and groups */
	       Mch_ListOneOrMoreMatchesTitleGrps (Games,&Match,Anchor);

	       /* Number of players who have played the match */
	       Mch_ListOneOrMoreMatchesNumPlayers (Games,&Match);

	       /* Match status */
	       Mch_ListOneOrMoreMatchesStatus (Games,&Match,Games->Game.NumQsts);

	       /* Match result visible? */
	       Mch_ListOneOrMoreMatchesResult (Games,&Match);

	    HTM_TR_End ();

	    /***** Second row: match author ****/
	    HTM_TR_Begin (NULL);
	       Mch_ListOneOrMoreMatchesAuthor (Games,&Match);
	    HTM_TR_End ();

	    /***** Third row for this match used for edition ****/
	    if (ICanEditMatches == Usr_CAN &&
		PutFormMatch == Frm_PUT_FORM &&		// Editing...
		Match.MchCod == MchCodToBeEdited)	// ...this match
	       /***** Check if I can edit this match *****/
	       if (Mch_CheckIfICanEditThisMatch (&Match) == Usr_CAN)
		 {
		  HTM_TR_Begin (NULL);
		     HTM_TD_Begin ("colspan=\"7\" class=\"LT %s\"",
		                   The_GetColorRows ());
			Mch_PutFormMatch (&Match);	// Form to edit this match
		     HTM_TD_End ();
		  HTM_TR_End ();
		 }

	    /***** Free anchor string *****/
	    free (Anchor);
	   }
	}

      /***** Put button to play a new match in this game *****/
      if (ICanEditMatches == Usr_CAN &&
	  PutFormMatch == Frm_PUT_FORM &&
	  MchCodToBeEdited <= 0)
	{
	 /* Reset match */
         Mch_ResetMatch (&Match);
	 Match.GamCod = Games->Game.GamCod;
	 Str_Copy (Match.Title,Games->Game.Title,sizeof (Match.Title) - 1);

	 /* Put form to create new match */
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"7\" class=\"LT %s\"",The_GetColorRows ());
	       Mch_PutFormMatch (&Match);	// Form to create match
	    HTM_TD_End ();
	 HTM_TR_End ();
        }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Put a column for match start and end times ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesHeading (Usr_Can_t ICanEditMatches)
  {
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Match;
   extern const char *Txt_Players;
   extern const char *Txt_Status;
   extern const char *Txt_Results;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Column for icons *****/
      if (ICanEditMatches == Usr_CAN)
	 HTM_TH_Empty (1);

      /***** The rest of columns *****/
      HTM_TH (Txt_START_END_TIME[Gam_ORDER_BY_STR_DATE]	,HTM_HEAD_LEFT  );
      HTM_TH (Txt_START_END_TIME[Gam_ORDER_BY_END_DATE]	,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Match					,HTM_HEAD_LEFT  );
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 Ico_PutIconOn ("users.svg",Ico_BLUE,Txt_Players);
      HTM_TH_End ();
      HTM_TH (Txt_Status				,HTM_HEAD_CENTER);
      HTM_TH (Txt_Results				,HTM_HEAD_CENTER);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********************** Check if I can edit matches *************************/
/*****************************************************************************/

static Usr_Can_t Mch_CheckIfICanEditMatches (void)
  {
   static Usr_Can_t ICanEditMatches[Rol_NUM_ROLES] =
     {
      [Rol_NET    ] = Usr_CAN,
      [Rol_TCH    ] = Usr_CAN,
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return ICanEditMatches[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/***************** Check if I can edit (remove/resume) a match ***************/
/*****************************************************************************/

static Usr_Can_t Mch_CheckIfICanEditThisMatch (const struct Mch_Match *Match)
  {
   if (Match->MchCod <= 0)
      return Usr_CAN;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return (Match->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod) ? Usr_CAN :	// Only if I am the creator
							       Usr_CAN_NOT;
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return Usr_CAN;
      default:
	 return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/*********** Check if visibility of match results can be changed *************/
/*****************************************************************************/

static Usr_Can_t Mch_CheckIfICanChangeVisibilityOfResults (const struct Mch_Match *Match)
  {
   if (Match->Status.ShowUsrResults ||		// Results are currently visible
       Match->Status.Showing == Mch_END)	// Match has finished
      return Mch_CheckIfICanEditThisMatch (Match);

   return Usr_CAN_NOT;
  }

/*****************************************************************************/
/************************* Put a column for icons ****************************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesIcons (struct Gam_Games *Games,
                                           const struct Mch_Match *Match,
                                           const char *Anchor)
  {
   HTM_TD_Begin ("rowspan=\"2\" class=\"BT %s\"",The_GetColorRows ());

      switch (Mch_CheckIfICanEditThisMatch (Match))
	{
	 case Usr_CAN:
	    Games->MchCod = Match->MchCod;

	    /***** Put icon to remove the match *****/
	    Ico_PutContextualIconToRemove (ActReqRemMch,NULL,Mch_PutParsEdit,Games);

	    /***** Put icon to edit the match *****/
	    Ico_PutContextualIconToEdit (ActReqChgMch,Anchor,Mch_PutParsEdit,Games);
	    break;
	 case Usr_CAN_NOT:
	 default:
	    Ico_PutIconRemovalNotAllowed ();
	    break;
	}

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Put a column for teacher who created the match ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesAuthor (const struct Gam_Games *Games,
				            const struct Mch_Match *Match)
  {
   /***** Match author (teacher) *****/
   HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
      Usr_WriteAuthor1Line (Match->UsrCod,Games->Game.Hidden);
   HTM_TD_End ();
  }

/*****************************************************************************/
/***************** Put a column for match start and end times ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesTimes (const struct Gam_Games *Games,
					   const struct Mch_Match *Match,
					   unsigned UniqueId)
  {
   extern const char *Dat_TimeStatusClass[Dat_NUM_TIME_STATUS][HidVis_NUM_HIDDEN_VISIBLE];
   Dat_StartEndTime_t StartEndTime;
   char *Id;
   static Dat_TimeStatus_t TimeStatus[Mch_NUM_SHOWING] =
     {
      [Mch_START  ] = Dat_FUTURE,	// Start: don't show anything
      [Mch_STEM   ] = Dat_PRESENT,	// Showing only the question stem
      [Mch_ANSWERS] = Dat_PRESENT,	// Showing the question stem and the answers
      [Mch_RESULTS] = Dat_PRESENT,	// Showing the results
      [Mch_END    ] = Dat_PAST,		// End: don't show anything
     };

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"mch_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
		    Id,
		    Dat_TimeStatusClass[TimeStatus[Match->Status.Showing]][Games->Game.Hidden],
		    The_GetSuffix (),The_GetColorRows ());
	 Dat_WriteLocalDateHMSFromUTC (Id,Match->TimeUTC[StartEndTime],
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       Dat_WRITE_TODAY |
				       Dat_WRITE_DATE_ON_SAME_DAY |
				       Dat_WRITE_WEEK_DAY |
				       Dat_WRITE_HOUR |
				       Dat_WRITE_MINUTE |
				       Dat_WRITE_SECOND);
      HTM_TD_End ();
      free (Id);
     }
  }

/*****************************************************************************/
/***************** Put a column for match title and grous ********************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesTitleGrps (const struct Gam_Games *Games,
					       const struct Mch_Match *Match,
                                               const char *Anchor)
  {
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Play;
   extern const char *Txt_Resume;

   HTM_TD_Begin ("rowspan=\"2\" class=\"LT %s\"",The_GetColorRows ());
      HTM_ARTICLE_Begin (Anchor);

	 /***** Match title *****/
	 Frm_BeginForm (Gbl.Usrs.Me.Role.Logged == Rol_STD ? ActJoiMch :
							     ActResMch);
	    ParCod_PutPar (ParCod_Mch,Match->MchCod);

	    HTM_BUTTON_Submit_Begin (Gbl.Usrs.Me.Role.Logged == Rol_STD ? Txt_Play :
									  Txt_Resume,
				     NULL,
				     "class=\"LT BT_LINK %s_%s\"",
				     HidVis_TitleClass[Games->Game.Hidden],
				     The_GetSuffix ());
	       HTM_Txt (Match->Title);
	    HTM_BUTTON_End ();

	 Frm_EndForm ();

	 /***** Groups whose students can answer this match *****/
	 if (Gbl.Crs.Grps.NumGrps)
	    Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (Games,Match);

      HTM_ARTICLE_End ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Get and write the names of the groups of a match **************/
/*****************************************************************************/

static void Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (const struct Gam_Games *Games,
							 const struct Mch_Match *Match)
  {
   extern const char *HidVis_GroupClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumGrps;
   unsigned NumGrp;

   /***** Get groups associated to a match from database *****/
   NumGrps = Mch_DB_GetGrpNamesAssociatedToMatch (&mysql_res,Match->MchCod);

   HTM_DIV_Begin ("class=\"%s_%s\"",
		  HidVis_GroupClass[Games->Game.Hidden],
		  The_GetSuffix ());

      /***** Write heading *****/
      HTM_Txt (NumGrps == 1 ? Txt_Group  :
			      Txt_Groups);
      HTM_Colon (); HTM_NBSP ();

      /***** Write groups *****/
      if (NumGrps) // Groups found...
	{
	 /* Get and write the group types and names */
	 for (NumGrp = 0;
	      NumGrp < NumGrps;
	      NumGrp++)
	   {
	    /* Get next group */
	    row = mysql_fetch_row (mysql_res);

	    /* Write group type name and group name */
	    HTM_Txt (row[0]);
	    HTM_SP ();
	    HTM_Txt (row[1]);

	    /* Write separator */
	    HTM_ListSeparator (NumGrp,NumGrps);
	   }
	}
      else
	 Grp_WriteTheWholeCourse ();

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Put a column for number of players **********************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesNumPlayers (const struct Gam_Games *Games,
					        const struct Mch_Match *Match)
  {
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Number of players who have answered any question in the match ******/
   HTM_TD_Begin ("rowspan=\"2\" class=\"RT %s_%s %s\"",
                 HidVis_DataClass[Games->Game.Hidden],The_GetSuffix (),
                 The_GetColorRows ());
      HTM_Unsigned (Mch_DB_GetNumUsrsWhoHavePlayedMch (Match->MchCod));
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************** Put a column for match status ************************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesStatus (const struct Gam_Games *Games,
					    struct Mch_Match *Match,
					    unsigned NumQsts)
  {
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];

   HTM_TD_Begin ("rowspan=\"2\" class=\"CT DAT_%s %s\"",
                 The_GetSuffix (),The_GetColorRows ());

      if (Match->Status.Showing != Mch_END)	// Match not over
	{
	 /* Current question index / total of questions */
	 HTM_DIV_Begin ("class=\"%s_%s\"",
			HidVis_DataClass[Games->Game.Hidden],
			The_GetSuffix ());
	    HTM_Unsigned (Match->Status.QstInd); HTM_Slash ();
	    HTM_Unsigned (NumQsts);
	 HTM_DIV_End ();
	}

      /* Icon to join match or resume match */
      Lay_PutContextualLinkOnlyIcon (Gbl.Usrs.Me.Role.Logged == Rol_STD ? ActJoiMch :
									  ActResMch,
				     NULL,
				     Mch_PutParMchCod,&Match->MchCod,
				     Match->Status.Showing == Mch_END ? "flag-checkered.svg" :
									"play.svg",Ico_BLACK);

   HTM_TD_End ();
  }

/*****************************************************************************/
/******************** Write parameter with code of match *********************/
/*****************************************************************************/

static void Mch_PutParMchCod (void *MchCod)
  {
   if (MchCod)
      ParCod_PutPar (ParCod_Mch,*((long *) MchCod));
  }

/*****************************************************************************/
/**************** Put a column for visibility of match result ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesResult (struct Gam_Games *Games,
                                            const struct Mch_Match *Match)
  {
   static void (*Function[Rol_NUM_ROLES]) (struct Gam_Games *Games,
                                           const struct Mch_Match *Match) =
     {
      [Rol_STD    ] = Mch_ListOneOrMoreMatchesResultStd,
      [Rol_NET    ] = Mch_ListOneOrMoreMatchesResultTch,
      [Rol_TCH    ] = Mch_ListOneOrMoreMatchesResultTch,
      [Rol_SYS_ADM] = Mch_ListOneOrMoreMatchesResultTch,
     };

   HTM_TD_Begin ("rowspan=\"2\" class=\"CT %s\"",The_GetColorRows ());

      if (Function[Gbl.Usrs.Me.Role.Logged])
	 Function[Gbl.Usrs.Me.Role.Logged] (Games,Match);
      else
	 Err_WrongRoleExit ();

   HTM_TD_End ();
  }

static void Mch_ListOneOrMoreMatchesResultStd (struct Gam_Games *Games,
                                               const struct Mch_Match *Match)
  {
   Games->MchCod = Match->MchCod;

   /***** Is match result visible or hidden? *****/
   if (Match->Status.ShowUsrResults)
      /* Result is visible by me */
      Lay_PutContextualLinkOnlyIcon (ActSeeMyMchResMch,MchRes_RESULTS_BOX_ID,
				     Mch_PutParsEdit,Games,
				     "trophy.svg",Ico_BLACK);
   else
      /* Result is forbidden to me */
      Ico_PutIconNotVisible ();
  }

static void Mch_ListOneOrMoreMatchesResultTch (struct Gam_Games *Games,
                                               const struct Mch_Match *Match)
  {
   extern const char *Txt_Visible_results;
   extern const char *Txt_Hidden_results;

   Games->MchCod = Match->MchCod;

   /***** Show match results *****/
   if (Mch_CheckIfICanEditThisMatch (Match) == Usr_CAN)
      Lay_PutContextualLinkOnlyIcon (ActSeeUsrMchResMch,MchRes_RESULTS_BOX_ID,
				     Mch_PutParsEdit,Games,
				     "trophy.svg",Ico_BLACK);

   /***** Check if visibility of session results can be changed *****/
   switch (Mch_CheckIfICanChangeVisibilityOfResults (Match))
     {
      case Usr_CAN:
	 /* I can edit visibility */
	 if (Match->Status.ShowUsrResults)
	    Lay_PutContextualLinkOnlyIcon (ActChgVisResMchUsr,NULL,
					   Mch_PutParsEdit,Games,
					   "eye.svg",Ico_GREEN);
	 else
	    Lay_PutContextualLinkOnlyIcon (ActChgVisResMchUsr,NULL,
					   Mch_PutParsEdit,Games,
					   "eye-slash.svg",Ico_RED);
	 break;
      case Usr_CAN_NOT:
      default:
	 /* I can not edit visibility */
	 if (Match->Status.ShowUsrResults)
	    Ico_PutIconOff ("eye.svg"      ,Ico_GREEN,Txt_Visible_results);
	 else
	    Ico_PutIconOff ("eye-slash.svg",Ico_RED  ,Txt_Hidden_results );
	 break;
     }
  }

/*****************************************************************************/
/******************** Toggle visibility of match results *********************/
/*****************************************************************************/

void Mch_ToggleVisResultsMchUsr (void)
  {
   struct Gam_Games Games;
   struct Mch_Match Match;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get and check parameters *****/
   Mch_GetAndCheckPars (&Games,&Match);

   /***** Check if visibility of match results can be changed *****/
   if (Mch_CheckIfICanChangeVisibilityOfResults (&Match) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Toggle visibility of match results *****/
   Match.Status.ShowUsrResults = !Match.Status.ShowUsrResults;
   Mch_DB_UpdateVisResultsMchUsr (Match.MchCod,Match.Status.ShowUsrResults);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
                        false,	// Do not list game questions
                        Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/******************** Get game data from a database row **********************/
/*****************************************************************************/

static void Mch_GetMatchDataFromRow (MYSQL_RES *mysql_res,
				     struct Mch_Match *Match)
  {
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;
   long LongNum;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[ 0]	MchCod
   row[ 1]	GamCod
   row[ 2]	UsrCod
   row[ 3]	UNIX_TIMESTAMP(StartTime)
   row[ 4]	UNIX_TIMESTAMP(EndTime)
   row[ 5]	Title
   */
   /***** Get match data *****/
   /* Code of the match (row[0]) */
   if ((Match->MchCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongMatchExit ();

   /* Code of the game (row[1]) */
   if ((Match->GamCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Err_WrongGameExit ();

   /* Get match teacher (row[2]) */
   Match->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get start/end times (row[3], row[4] hold start/end UTC times) */
   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Match->TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[3 + StartEndTime]);

   /* Get the title of the match (row[5]) */
   Str_Copy (Match->Title,row[5],sizeof (Match->Title) - 1);

   /***** Get current match status *****/
   /*
   row[ 6]	QstInd
   row[ 7]	QstCod
   row[ 8]	Showing
   row[ 9]	Countdown
   row[10]	NumCols
   row[11]	ShowQstResults
   row[12]	ShowUsrResults
   */
   /* Current question index (row[6]) */
   Match->Status.QstInd = Str_ConvertStrToUnsigned (row[6]);

   /* Current question code (row[7]) */
   Match->Status.QstCod = Str_ConvertStrCodToLongCod (row[7]);

   /* Get what to show (stem, answers, results) (row(8)) */
   Match->Status.Showing = Mch_DB_GetShowingFromStr (row[8]);

   /* Get countdown (row[9]) */
   Match->Status.Countdown = Str_ConvertStrCodToLongCod (row[9]);

   /* Get number of columns (row[10]) */
   LongNum = Str_ConvertStrCodToLongCod (row[10]);
   Match->Status.NumCols =  (LongNum <= 1           ) ? 1 :
                           ((LongNum >= Mch_MAX_COLS) ? Mch_MAX_COLS :
                        	                        (unsigned) LongNum);

   /* Get whether to show question results or not (row(11)) */
   Match->Status.ShowQstResults = (row[11][0] == 'Y');

   /* Get whether to show user results or not (row(12)) */
   Match->Status.ShowUsrResults = (row[12][0] == 'Y');

   /***** Get whether the match is being played or not *****/
   if (Match->Status.Showing == Mch_END)	// Match over
      Match->Status.Playing = false;
   else						// Match not over
      Match->Status.Playing = Mch_DB_CheckIfMatchIsBeingPlayed (Match->MchCod);
  }

/*****************************************************************************/
/************** Request the removal of a match (game instance) ***************/
/*****************************************************************************/

void Mch_ReqRemMatch (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_match_X;
   struct Gam_Games Games;
   struct Mch_Match Match;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get and check parameters *****/
   Mch_GetAndCheckPars (&Games,&Match);

   /***** Show question and button to remove question *****/
   Ale_ShowAlertRemove (ActRemMch,NULL,
                        Mch_PutParsEdit,&Games,
			Txt_Do_you_really_want_to_remove_the_match_X,
	                Match.Title);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
                        false,	// Do not list game questions
			Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/********************** Remove a match (game instance) ***********************/
/*****************************************************************************/

void Mch_RemoveMatch (void)
  {
   extern const char *Txt_Match_X_removed;
   struct Gam_Games Games;
   struct Mch_Match Match;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get and check parameters *****/
   Mch_GetAndCheckPars (&Games,&Match);

   /***** Check if I can remove this match *****/
   if (Mch_CheckIfICanEditThisMatch (&Match) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Remove the match from all database tables *****/
   Mch_DB_RemoveMatchFromAllTables (Match.MchCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Match_X_removed,
		  Match.Title);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
                        false,	// Do not list game questions
			Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/******************** Remove match in game from all tables *******************/
/*****************************************************************************/

void Mch_RemoveMatchesInGameFromAllTables (long GamCod)
  {
   /***** Remove matches from secondary tables *****/
   Mch_DB_RemoveMatchesInGameFromOtherTable (GamCod,"mch_players");
   Mch_DB_RemoveMatchesInGameFromOtherTable (GamCod,"mch_playing");
   Mch_DB_RemoveMatchesInGameFromOtherTable (GamCod,"mch_results");
   Mch_DB_RemoveMatchesInGameFromOtherTable (GamCod,"mch_answers");
   Mch_DB_RemoveMatchesInGameFromOtherTable (GamCod,"mch_times");
   Mch_DB_RemoveMatchesInGameFromOtherTable (GamCod,"mch_groups");
   Mch_DB_RemoveMatchesInGameFromOtherTable (GamCod,"mch_indexes");

   /***** Remove matches in game from main table *****/
   Mch_DB_RemoveMatchesInGameFromMainTable (GamCod);
  }

/*****************************************************************************/
/******************* Remove match in course from all tables ******************/
/*****************************************************************************/

void Mch_RemoveMatchesInCourseFromAllTables (long CrsCod)
  {
   /***** Remove matches from secondary tables *****/
   Mch_DB_RemoveMatchesInCrsFromOtherTable (CrsCod,"mch_players");
   Mch_DB_RemoveMatchesInCrsFromOtherTable (CrsCod,"mch_playing");
   Mch_DB_RemoveMatchesInCrsFromOtherTable (CrsCod,"mch_results");
   Mch_DB_RemoveMatchesInCrsFromOtherTable (CrsCod,"mch_answers");
   Mch_DB_RemoveMatchesInCrsFromOtherTable (CrsCod,"mch_times");
   Mch_DB_RemoveMatchesInCrsFromOtherTable (CrsCod,"mch_groups");
   Mch_DB_RemoveMatchesInCrsFromOtherTable (CrsCod,"mch_indexes");

   /***** Remove matches in course from main table *****/
   Mch_DB_RemoveMatchesInCrsFromMainTable (CrsCod);
  }

/*****************************************************************************/
/*************** Remove matches made by user in all courses ******************/
/*****************************************************************************/

void Mch_RemoveMatchesMadeByUsrInAllCrss (long UsrCod)
  {
   /***** Remove student from secondary tables *****/
   Mch_DB_RemoveMatchesMadeByUsrFromTable (UsrCod,"mch_players");
   Mch_DB_RemoveMatchesMadeByUsrFromTable (UsrCod,"mch_results");
   Mch_DB_RemoveMatchesMadeByUsrFromTable (UsrCod,"mch_answers");
  }

/*****************************************************************************/
/***************** Remove matches made by user in a course *******************/
/*****************************************************************************/

void Mch_RemoveMatchesMadeByUsrInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove student from secondary tables *****/
   Mch_DB_RemoveMatchesMadeByUsrInCrsFromTable (UsrCod,CrsCod,"mch_players");
   Mch_DB_RemoveMatchesMadeByUsrInCrsFromTable (UsrCod,CrsCod,"mch_results");
   Mch_DB_RemoveMatchesMadeByUsrInCrsFromTable (UsrCod,CrsCod,"mch_answers");
  }

/*****************************************************************************/
/**************** Request the creation or edition of a match *****************/
/*****************************************************************************/

void Mch_ReqCreatOrEditMatch (void)
  {
   struct Gam_Games Games;
   struct Mch_Match Match;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get and check parameters *****/
   Mch_GetAndCheckPars (&Games,&Match);

   /***** Check if I can edit this match *****/
   if (Mch_CheckIfICanEditThisMatch (&Match) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Show game *****/
   Gam_ShowOnlyOneGame (&Games,
                        false,	// Do not list game questions
                        Frm_PUT_FORM);
  }

/*****************************************************************************/
/*********************** Params used to edit a match *************************/
/*****************************************************************************/

void Mch_PutParsEdit (void *Games)
  {
   if (Games)
     {
      Gam_PutPars (Games);
      ParCod_PutPar (ParCod_Mch,((struct Gam_Games *) Games)->MchCod);
     }
  }

/*****************************************************************************/
/************************** Get and check parameters *************************/
/*****************************************************************************/

void Mch_GetAndCheckPars (struct Gam_Games *Games,
                          struct Mch_Match *Match)
  {
   /***** Get parameters *****/
   /* Get parameters of game */
   if ((Games->Game.GamCod = Gam_GetPars (Games)) <= 0)
      Err_WrongGameExit ();
   Grp_GetParMyAllGrps ();
   Gam_GetGameDataByCod (&Games->Game);
   if (Games->Game.CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)
      Err_WrongGameExit ();

   /* Get match code */
   if ((Match->MchCod = ParCod_GetPar (ParCod_Mch)) > 0)
     {
      Mch_GetMatchDataByCod (Match);
      if (Games->Game.GamCod != Match->GamCod)
	 Err_WrongGameExit ();
     }
   else
      Mch_ResetMatch (Match);

   /***** Initialize context *****/
   Games->MchCod = Match->MchCod;
  }

/*****************************************************************************/
/******************* Put a form to change and existing match *****************/
/*****************************************************************************/

static void Mch_PutFormMatch (struct Mch_Match *Match)
  {
   extern const char *Hlp_ASSESSMENT_Games_matches;
   extern const char *Txt_Title;
   static struct
     {
      Act_Action_t Action;
      Btn_Button_t Button;
     } Forms[OldNew_NUM_OLD_NEW] =
     {
      [OldNew_OLD] = {ActChgMch,Btn_SAVE_CHANGES},
      [OldNew_NEW] = {ActNewMch,Btn_CREATE      }
     };
   OldNew_OldNew_t OldNewMatch = (Match->MchCod > 0) ? OldNew_OLD :
						       OldNew_NEW;;

   /***** Begin section for match *****/
   HTM_SECTION_Begin (Mch_NEW_MATCH_SECTION_ID);

      /***** Begin form to create/edit *****/
      Frm_BeginFormTable (Forms[OldNewMatch].Action,
			  Mch_NEW_MATCH_SECTION_ID,
			  Mch_ParsFormMatch,Match,"TBL_WIDE");

	 /***** Match title *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_INPUT_TEXT ("Title",Mch_MAX_CHARS_TITLE,Match->Title,
			       HTM_REQUIRED,
			       "id=\"Title\" class=\"Frm_C2_INPUT INPUT_%s\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Groups *****/
	 Mch_ShowLstGrpsToEditMatch (Match->MchCod);

      /***** End form to create *****/
      Frm_EndFormTable (Forms[OldNewMatch].Button);

   /***** End section for match *****/
   HTM_SECTION_End ();
  }

static void Mch_ParsFormMatch (void *Match)
  {
   ParCod_PutPar (ParCod_Gam,((struct Mch_Match *) Match)->GamCod);
   ParCod_PutPar (ParCod_Mch,((struct Mch_Match *) Match)->MchCod);
  }

/*****************************************************************************/
/***************** Show list of groups to create a new match *****************/
/*****************************************************************************/

static void Mch_ShowLstGrpsToEditMatch (long MchCod)
  {
   extern const char *Txt_Groups;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_GRP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.NumGrpTypes)
     {
      HTM_TR_Begin (NULL);

         /* Label */
	 Frm_LabelColumn ("Frm_C1 RT","",Txt_Groups);

	 /* Groups */
	 HTM_TD_Begin ("class=\"Frm_C2 LT\"");

	    /***** First row: checkbox to select the whole course *****/
	    HTM_LABEL_Begin (NULL);
	       HTM_INPUT_CHECKBOX ("WholeCrs",
				   Grp_DB_CheckIfAssociatedToGrps ("mch_groups",
								   "MchCod",
								   MchCod) ? HTM_NO_ATTR :
									     HTM_CHECKED,
				   "id=\"WholeCrs\" value=\"Y\""
				   " onclick=\"uncheckChildren(this,'GrpCods')\"");
	       Grp_WriteTheWholeCourse ();
	    HTM_LABEL_End ();

	    /***** List the groups for each group type *****/
	    Grp_ListGrpsToEditAsgAttSvyEvtMch (Grp_MATCH,MchCod);

	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/********************* Create a new match (by a teacher) *********************/
/*****************************************************************************/

void Mch_CreateNewMatch (void)
  {
   long GamCod;
   char Title[Mch_MAX_BYTES_TITLE + 1];

   /***** Get form parameters *****/
   /* Get match code */
   GamCod = ParCod_GetAndCheckPar (ParCod_Gam);

   /* Get match title */
   Par_GetParText ("Title",Title,Mch_MAX_BYTES_TITLE);

   /* Get groups for this match */
   Grp_GetParCodsSeveralGrps ();

   /***** Create a new match *****/
   Mch_SetMchCodBeingPlayed (Mch_CreateMatch (GamCod,Title));

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/************************ Change a match (by a teacher) **********************/
/*****************************************************************************/

void Mch_ChangeMatch (void)
  {
   struct Gam_Games Games;
   struct Mch_Match Match;

   /***** Reset games context *****/
   Gam_ResetGames (&Games);
   Gam_ResetGame (&Games.Game);
   Mch_ResetMatch (&Match);

   /***** Get and check parameters *****/
   Mch_GetAndCheckPars (&Games,&Match);

   /***** Check if I can update this match *****/
   if (Mch_CheckIfICanEditThisMatch (&Match) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Get match title and groups *****/
   /* Get match title */
   Par_GetParText ("Title",Match.Title,Mch_MAX_BYTES_TITLE);

   /* Get groups for this match */
   Grp_GetParCodsSeveralGrps ();

   /***** Update match *****/
   Mch_UpdateMatchTitleAndGrps (&Match);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Games,
                        false,	// Do not list game questions
			Frm_DONT_PUT_FORM);
  }

/*****************************************************************************/
/*************** Update title and groups of an existing match ****************/
/*****************************************************************************/

static void Mch_UpdateMatchTitleAndGrps (const struct Mch_Match *Match)
  {
   /***** Update match title into database *****/
   Mch_DB_UpdateMatchTitle (Match->MchCod,Match->Title);

   /***** Update groups associated to the match *****/
   Mch_DB_RemoveMatchFromTable (Match->MchCod,"mch_groups");	// Remove all groups associated to this match
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Mch_CreateGrps (Match->MchCod);				// Associate selected groups
  }

/*****************************************************************************/
/******* Show button to actually start / resume a match (by a teacher) *******/
/*****************************************************************************/

void Mch_ResumeMatch (void)
  {
   struct Mch_Match Match;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Check if I have permission to resume match *****/
   if (Mch_CheckIfICanEditThisMatch (&Match) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForTch (&Match);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Create a new match in a game *************************/
/*****************************************************************************/

static long Mch_CreateMatch (long GamCod,char Title[Mch_MAX_BYTES_TITLE + 1])
  {
   long MchCod;

   /***** Insert this new match into database *****/
   MchCod = Mch_DB_CreateMatch (GamCod,Title);

   /***** Create indexes for answers *****/
   Mch_CreateIndexes (GamCod,MchCod);

   /***** Create groups associated to the match *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Mch_CreateGrps (MchCod);

   return MchCod;
  }

/*****************************************************************************/
/*********************** Create indexes for a match **************************/
/*****************************************************************************/
/* Everytime a new match is created,
   the answers of each shufflable question are shuffled.
   The shuffling is stored in a table of indexes
   that will be read when showing a match */

static void Mch_CreateIndexes (long GamCod,long MchCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQsts;
   unsigned NumQst;
   struct Qst_Question Question;
   unsigned QstInd;

   /***** Get questions of the game *****/
   NumQsts = Gam_DB_GetGameQuestionsFull (&mysql_res,GamCod);

   /***** For each question in game... *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      /***** Create test question *****/
      Qst_QstConstructor (&Question);

      /***** Get question data *****/
      row = mysql_fetch_row (mysql_res);
      /*
      gam_questions.QstCod	row[0]
      gam_questions.QstInd	row[1]
      tst_questions.AnsType	row[2]
      tst_questions.Shuffle	row[3]
      */

      /* Get question code (row[0]) */
      if ((Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	 Err_WrongQuestionExit ();

      /* Get question index (row[1]) */
      QstInd = Str_ConvertStrToUnsigned (row[1]);

      /* Get answer type (row[2]) */
      Question.Answer.Type = Qst_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
      if (Question.Answer.Type != Qst_ANS_UNIQUE_CHOICE)
	 Err_WrongAnswerExit ();

      /* Get shuffle (row[3]) */
      Question.Answer.Shuffle = Qst_GetShuffleFromYN (row[3][0]);

      /***** Reorder answer *****/
      Mch_ReorderAnswer (MchCod,QstInd,&Question);

      /***** Destroy test question *****/
      Qst_QstDestructor (&Question);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Reorder answers of a match question *********************/
/*****************************************************************************/

static void Mch_ReorderAnswer (long MchCod,unsigned QstInd,
			       const struct Qst_Question *Question)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAnss;
   unsigned NumAns;
   unsigned AnsInd;
   char StrOneAnswer[Cns_MAX_DIGITS_UINT + 1];
   char StrAnswersOneQst[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1];

   /***** Initialize list of answers to empty string *****/
   StrAnswersOneQst[0] = '\0';

   /***** Get suffled/not-shuffled answers indexes of question *****/
   NumAnss = Qst_DB_GetShuffledAnswersIndexes (&mysql_res,Question);

   /***** For each answer in question... *****/
   for (NumAns = 0;
	NumAns < NumAnss;
	NumAns++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get answer index (row[0]) */
      AnsInd = Str_ConvertStrToUnsigned (row[0]);
      snprintf (StrOneAnswer,sizeof (StrOneAnswer),"%u",AnsInd);

      /* Concatenate answer index to list of answers */
      if (NumAns)
         Str_Concat (StrAnswersOneQst,",",sizeof (StrAnswersOneQst) - 1);
      Str_Concat (StrAnswersOneQst,StrOneAnswer,sizeof (StrAnswersOneQst) - 1);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Create entry for this question in table of match indexes *****/
   Mch_DB_CreateQstIndexes (MchCod,QstInd,StrAnswersOneQst);
  }

/*****************************************************************************/
/***************** Get indexes for a question from database ******************/
/*****************************************************************************/

void Mch_GetIndexes (long MchCod,unsigned QstInd,
		     unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION])
  {
   char StrIndexesOneQst[Qst_MAX_BYTES_INDEXES_ONE_QST + 1];

   /***** Get indexes for a question from database *****/
   Mch_DB_GetIndexes (MchCod,QstInd,StrIndexesOneQst);
   if (!StrIndexesOneQst[0])
      Err_WrongAnswerIndexExit ();

   /***** Get indexes from string *****/
   TstPrn_GetIndexesFromStr (StrIndexesOneQst,Indexes);
  }

/*****************************************************************************/
/******************* Create groups associated to a match *********************/
/*****************************************************************************/

static void Mch_CreateGrps (long MchCod)
  {
   unsigned NumGrpSel;

   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      Mch_DB_AssociateGroupToMatch (MchCod,
                                    Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/***************** Insert/update a game match being played *******************/
/*****************************************************************************/

static void Mch_UpdateMatchStatusInDB (const struct Mch_Match *Match)
  {
   /***** Update match status in database *****/
   Mch_DB_UpdateMatchStatus (Match);

   /***** Update match as being/not-being played */
   if (Match->Status.Playing)	// Match is being played
      /* Update match as being played */
      Mch_DB_UpdateMatchAsBeingPlayed (Match->MchCod);
   else				// Match is paused, not being played
      /* Update match as not being played */
      Mch_DB_RemoveMatchFromBeingPlayed (Match->MchCod);
  }

/*****************************************************************************/
/********** Update elapsed time in current question (by a teacher) ***********/
/*****************************************************************************/

static void Mch_UpdateElapsedTimeInQuestion (const struct Mch_Match *Match)
  {
   /***** Update elapsed time in current question in database *****/
   if (Match->Status.Playing &&		// Match is being played
       Match->Status.Showing != Mch_START &&
       Match->Status.Showing != Mch_END)
      Mch_DB_UpdateElapsedTimeInQuestion (Match->MchCod,Match->Status.QstInd);
  }

/*****************************************************************************/
/******************* Get elapsed time in a match question ********************/
/*****************************************************************************/

static void Mch_GetElapsedTimeInQuestion (const struct Mch_Match *Match,
					  struct Dat_Time *Time)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRows;

   /***** Query database *****/
   NumRows = Mch_DB_GetElapsedTimeInQuestion (&mysql_res,
                                              Match->MchCod,
                                              Match->Status.QstInd);

   /***** Get elapsed time from query result *****/
   Mch_GetElapsedTime (NumRows,mysql_res,Time);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get elapsed time in a match *************************/
/*****************************************************************************/

static void Mch_GetElapsedTimeInMatch (const struct Mch_Match *Match,
				       struct Dat_Time *Time)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRows;

   /***** Query database *****/
   NumRows = Mch_DB_GetElapsedTimeInMatch (&mysql_res,Match->MchCod);

   /***** Get elapsed time from query result *****/
   Mch_GetElapsedTime (NumRows,mysql_res,Time);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get elapsed time in a match *************************/
/*****************************************************************************/

static void Mch_GetElapsedTime (unsigned NumRows,MYSQL_RES *mysql_res,
				struct Dat_Time *Time)
  {
   MYSQL_ROW row;
   bool ElapsedTimeGotFromDB = false;

   /***** Get time from H...H:MM:SS string *****/
   if (NumRows)
     {
      row = mysql_fetch_row (mysql_res);

      if (row[0])
	 /* Get the elapsed time (row[0]) */
	 if (sscanf (row[0],"%u:%02u:%02u",
	             &Time->Hour,
	             &Time->Minute,
	             &Time->Second) == 3)
	    ElapsedTimeGotFromDB = true;
     }

   /***** Initialize time to default value (0) *****/
   if (!ElapsedTimeGotFromDB)
      Time->Hour   =
      Time->Minute =
      Time->Second = 0;
  }

/*****************************************************************************/
/********************** Play/pause match (by a teacher) **********************/
/*****************************************************************************/

void Mch_PlayPauseMatch (void)
  {
   struct Mch_Match Match;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Update status *****/
   if (Match.Status.Playing)	// Match is being played             ==> pause it
      Match.Status.Playing = false;		// Pause match
   else				// Match is paused, not being played ==> play it
      /* If not over, update status */
      if (Match.Status.Showing != Mch_END)	// Match not over
	 Match.Status.Playing = true;		// Start/resume match

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForTch (&Match);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Change number of columns in answers of a match (by a teacher) *******/
/*****************************************************************************/

void Mch_ChangeNumColsMch (void)
  {
   struct Mch_Match Match;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Get number of columns *****/
   Match.Status.NumCols = (unsigned)
	                  Par_GetParUnsignedLong ("NumCols",
						  1,
						  Mch_MAX_COLS,
						  Mch_NUM_COLS_DEFAULT);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForTch (&Match);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********* Toggle the display of results in a match (by a teacher) ***********/
/*****************************************************************************/

void Mch_ToggleVisResultsMchQst (void)
  {
   struct Mch_Match Match;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Update status *****/
   Match.Status.ShowQstResults = !Match.Status.ShowQstResults;	// Toggle display
   if (Match.Status.Showing == Mch_RESULTS &&
       !Match.Status.ShowQstResults)
     Match.Status.Showing = Mch_ANSWERS;	// Hide results

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForTch (&Match);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************* Show previous question in a match (by a teacher) **************/
/*****************************************************************************/

void Mch_BackMatch (void)
  {
   struct Mch_Match Match;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Update status *****/
   Mch_SetMatchStatusToPrev (&Match);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForTch (&Match);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Show next question in a match (by a teacher) ****************/
/*****************************************************************************/

void Mch_ForwardMatch (void)
  {
   struct Mch_Match Match;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Update status *****/
   Mch_SetMatchStatusToNext (&Match);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForTch (&Match);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************** Set match status to previous (backward) status ***************/
/*****************************************************************************/

static void Mch_SetMatchStatusToPrev (struct Mch_Match *Match)
  {
   /***** What to show *****/
   switch (Match->Status.Showing)
     {
      case Mch_START:
	 Mch_SetMatchStatusToStart (Match);
	 break;
      case Mch_STEM:
      case Mch_END:
	 Mch_SetMatchStatusToPrevQst (Match);
	 break;
      case Mch_ANSWERS:
	 Match->Status.Showing = Mch_STEM;
	 break;
      case Mch_RESULTS:
	 Match->Status.Showing = Mch_ANSWERS;
	 break;
     }
   Match->Status.Countdown = -1L;		// No countdown

   /***** Force showing results to false when match is not at the end *****/
   Match->Status.ShowUsrResults = false;	// Force results to be hidden
  }

/*****************************************************************************/
/****************** Set match status to previous question ********************/
/*****************************************************************************/

static void Mch_SetMatchStatusToPrevQst (struct Mch_Match *Match)
  {
   /***** Get index of the previous question *****/
   Match->Status.QstInd = Gam_DB_GetPrevQuestionIndexInGame (Match->GamCod,
							     Match->Status.QstInd);
   if (Match->Status.QstInd)		// Start of questions not reached
     {
      Match->Status.QstCod = Gam_DB_GetQstCodFromQstInd (Match->GamCod,
						         Match->Status.QstInd);
      Match->Status.Showing = Match->Status.ShowQstResults ? Mch_RESULTS :
							     Mch_ANSWERS;
     }
   else					// Start of questions reached
      Mch_SetMatchStatusToStart (Match);
  }

/*****************************************************************************/
/************************ Set match status to start **************************/
/*****************************************************************************/

static void Mch_SetMatchStatusToStart (struct Mch_Match *Match)
  {
   Match->Status.QstInd  = 0;				// Before first question
   Match->Status.QstCod  = -1L;
   Match->Status.Playing = false;
   Match->Status.Showing = Mch_START;
  }

/*****************************************************************************/
/**************** Set match status to next (forward) status ******************/
/*****************************************************************************/

static void Mch_SetMatchStatusToNext (struct Mch_Match *Match)
  {
   /***** What to show *****/
   switch (Match->Status.Showing)
     {
      case Mch_START:
	 Mch_SetMatchStatusToNextQst (Match);
	 break;
      case Mch_STEM:
	 Match->Status.Showing = Mch_ANSWERS;
	 break;
      case Mch_ANSWERS:
	 if (Match->Status.ShowQstResults)
	    Match->Status.Showing = Mch_RESULTS;
	 else
	    Mch_SetMatchStatusToNextQst (Match);
	 break;
      case Mch_RESULTS:
	 Mch_SetMatchStatusToNextQst (Match);
	 break;
      case Mch_END:
	 Mch_SetMatchStatusToEnd (Match);
	 break;
     }
   Match->Status.Countdown = -1L;	// No countdown
  }

/*****************************************************************************/
/****************** Set match status to next question ************************/
/*****************************************************************************/

static void Mch_SetMatchStatusToNextQst (struct Mch_Match *Match)
  {
   /***** Get index of the next question *****/
   Match->Status.QstInd = Gam_DB_GetNextQuestionIndexInGame (Match->GamCod,
							     Match->Status.QstInd);

   /***** Get question code *****/
   if (Match->Status.QstInd < Gam_AFTER_LAST_QUESTION)	// End of questions not reached
     {
      Match->Status.QstCod = Gam_DB_GetQstCodFromQstInd (Match->GamCod,
						         Match->Status.QstInd);
      Match->Status.Showing = Mch_STEM;
     }
   else							// End of questions reached
      Mch_SetMatchStatusToEnd (Match);
  }

/*****************************************************************************/
/************************* Set match status to end ***************************/
/*****************************************************************************/

static void Mch_SetMatchStatusToEnd (struct Mch_Match *Match)
  {
   Match->Status.QstInd  = Gam_AFTER_LAST_QUESTION;	// After last question
   Match->Status.QstCod  = -1L;
   Match->Status.Playing = false;
   Match->Status.Showing = Mch_END;
  }

/*****************************************************************************/
/******* Show current match status (number, question, answers, button) *******/
/*****************************************************************************/

static void Mch_ShowMatchStatusForTch (struct Mch_Match *Match)
  {
   /***** Left column *****/
   Mch_ShowLeftColumnTch (Match);

   /***** Right column *****/
   Mch_ShowRightColumnTch (Match);
  }

/*****************************************************************************/
/************ Show current question being played for a student ***************/
/*****************************************************************************/

static void Mch_ShowMatchStatusForStd (struct Mch_Match *Match,Mch_Update_t Update)
  {
   struct Mch_UsrAnswer UsrAnswer;

   /***** Can I play this match? *****/
   if (Mch_CheckIfICanPlayThisMatchBasedOnGrps (Match) == Usr_CAN_NOT)
      Err_NoPermissionExit ();

   /***** Get student's answer to this question
	  (<0 ==> no answer) *****/
   Mch_GetQstAnsFromDB (Match->MchCod,
			Gbl.Usrs.Me.UsrDat.UsrCod,
			Match->Status.QstInd,
			&UsrAnswer);

   /***** Left column *****/
   Mch_ShowLeftColumnStd (Match,&UsrAnswer);

   /***** Right column *****/
   Mch_ShowRightColumnStd (Match,&UsrAnswer,Update);
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a match **************/
/*****************************************************************************/

Usr_Can_t Mch_CheckIfICanPlayThisMatchBasedOnGrps (const struct Mch_Match *Match)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 /***** Check if I belong to any of the groups
	        associated to the match *****/
	 return Mch_DB_CheckIfICanPlayThisMatchBasedOnGrps (Match->MchCod);
      case Rol_NET:
	 /***** Only if I am the creator *****/
	 return (Match->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod) ? Usr_CAN :
							       Usr_CAN_NOT;
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return Usr_CAN;
      default:
	 return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/*********** Show left column when playing a match (as a teacher) ************/
/*****************************************************************************/

static void Mch_ShowLeftColumnTch (struct Mch_Match *Match)
  {
   /***** Begin left container *****/
   HTM_DIV_Begin ("class=\"MCH_LEFT_TCH\"");

      /***** Refreshable part *****/
      HTM_DIV_Begin ("id=\"match_left\" class=\"MCH_REFRESHABLE_TEACHER\"");
	 Mch_ShowRefreshablePartTch (Match);
      HTM_DIV_End ();

      /***** Put forms to start countdown *****/
      Mch_PutFormsCountdown (Match);

      /***** Buttons *****/
      Mch_PutMatchControlButtons (Match);

      /***** Put forms to choice which projects to show *****/
      Set_BeginSettingsHead ();
	 Mch_ShowFormColumns (Match);
      Set_EndSettingsHead ();

      /***** Write button to request viewing results *****/
      Mch_PutCheckboxResult (Match);

   /***** End left container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Show left refreshable part for teachers *******************/
/*****************************************************************************/

static void Mch_ShowRefreshablePartTch (struct Mch_Match *Match)
  {
   /***** Write elapsed time in match *****/
   Mch_WriteElapsedTimeInMch (Match);

   /***** Write number of question *****/
   Mch_ShowNumQstInMch (Match);

   /***** Write elapsed time in question *****/
   Mch_WriteElapsedTimeInQst (Match);

   /***** Number of users who have responded this question *****/
   Mch_WriteNumRespondersQst (Match);

   /***** Write hourglass *****/
   Mch_PutCountdownAndHourglassIcon (Match);
  }

/*****************************************************************************/
/******************** Write elapsed time in current match ********************/
/*****************************************************************************/

static void Mch_WriteElapsedTimeInMch (struct Mch_Match *Match)
  {
   struct Dat_Time Time;

   HTM_DIV_Begin ("class=\"MCH_TOP CT\"");

      /***** Get elapsed time in match *****/
      Mch_GetElapsedTimeInMatch (Match,&Time);

      /***** Write elapsed time in hh:mm'ss" format *****/
      Dat_WriteHoursMinutesSeconds (&Time);

   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************** Write elapsed time in current question *******************/
/*****************************************************************************/

static void Mch_WriteElapsedTimeInQst (struct Mch_Match *Match)
  {
   struct Dat_Time Time;

   HTM_DIV_Begin ("class=\"MCH_TIME_QST\"");

      switch (Match->Status.Showing)
	{
	 case Mch_START:
	 case Mch_END:
	    HTM_Hyphen ();	// Do not write elapsed time
	    break;
	 default:
	    Mch_GetElapsedTimeInQuestion (Match,&Time);
	    Dat_WriteHoursMinutesSeconds (&Time);
	    break;
	}

   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Write number of responders to a match question **************/
/*****************************************************************************/

static void Mch_WriteNumRespondersQst (struct Mch_Match *Match)
  {
   extern const char *Txt_MATCH_respond;

   /***** Begin block *****/
   HTM_DIV_Begin ("class=\"MCH_NUM_ANSWERERS\"");

      HTM_Txt (Txt_MATCH_respond);
      HTM_BR ();
      HTM_STRONG_Begin ();

	 /***** Write number of responders *****/
	 switch (Match->Status.Showing)
	   {
	    case Mch_START:
	    case Mch_END:
	       HTM_Hyphen ();	// Do not write number of responders
	       break;
	    default:
	       HTM_Unsigned (Mch_DB_GetNumUsrsWhoAnsweredQst (Match->MchCod,
							      Match->Status.QstInd));
	       break;
	   }

	 /***** Write number of players *****/
	 if (Match->Status.Playing)	// Match is being played
	   {
	    /* Get current number of players */
	    Match->Status.NumPlayers = Mch_DB_GetNumPlayers (Match->MchCod);

	    /* Show current number of players */
	    HTM_Slash ();
	    HTM_Unsigned (Match->Status.NumPlayers);
	   }

      HTM_STRONG_End ();

   /***** End block *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Write current countdown and hourglass icon ******************/
/*****************************************************************************/

static void Mch_PutCountdownAndHourglassIcon (struct Mch_Match *Match)
  {
   extern const char *Txt_Countdown;
   const char *Class;
   const char *Icon;

   /***** Set hourglass icon depending on countdown *****/
   if (Match->Status.Showing == Mch_END)				// Match over
     {
      Class = "BT_LINK_OFF MCH_BUTTON_HIDDEN MCH_GREEN";
      Icon  = "fa-hourglass-start";
     }
   else if (Match->Status.Countdown < 0)				// No countdown
     {
      Class = "BT_LINK_OFF MCH_BUTTON_OFF MCH_GREEN";
      Icon  = "fa-hourglass-start";
     }
   else if (Match->Status.Countdown > Mch_COUNTDOWN_SECONDS_MEDIUM)	// Countdown in progress
     {
      Class = "BT_LINK_OFF MCH_BUTTON_OFF MCH_LIMEGREEN";
      Icon  = "fa-hourglass-start";
     }
   else if (Match->Status.Countdown > Mch_COUNTDOWN_SECONDS_SMALL)	// Countdown in progress
     {
      Class = "BT_LINK_OFF MCH_BUTTON_OFF MCH_YELLOW";
      Icon  = "fa-hourglass-half";
     }
   else									// Countdown about to end
     {
      Class = "BT_LINK_OFF MCH_BUTTON_OFF MCH_RED";
      Icon  = "fa-hourglass-end";
     }

   /***** Write countdown and put hourglass icon *****/
   HTM_DIV_Begin ("class=\"MCH_SHOW_HOURGLASS\"");
      HTM_DIV_Begin ("class=\"MCH_BIGBUTTON_CONT\"");
	 HTM_BUTTON_Begin (Txt_Countdown,"class=\"%s\"",Class);

	    /* Countdown */
	    HTM_NBSP ();
	    if (Match->Status.Countdown > 0)
	      {
	       HTM_Unsigned02 ((unsigned) Match->Status.Countdown);
	       HTM_Seconds ();
	      }
	    HTM_BR ();

	    /* Icon */
	    HTM_TxtF ("<i class=\"fas %s\"></i>",Icon);

	 HTM_BUTTON_End ();
      HTM_DIV_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Put all forms to start countdowns **********************/
/*****************************************************************************/

static void Mch_PutFormsCountdown (struct Mch_Match *Match)
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"MCH_SHOW_HOURGLASS\"");

      /***** Put forms to start countdown *****/
      Mch_PutFormCountdown (Match,-1                          ,"MCH_GREEN"    );
      Mch_PutFormCountdown (Match,Mch_COUNTDOWN_SECONDS_LARGE ,"MCH_LIMEGREEN");
      Mch_PutFormCountdown (Match,Mch_COUNTDOWN_SECONDS_MEDIUM,"MCH_YELLOW"   );
      Mch_PutFormCountdown (Match,Mch_COUNTDOWN_SECONDS_SMALL ,"MCH_RED"      );

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****** Put a form to start a countdown with a given number of seconds *******/
/*****************************************************************************/

static void Mch_PutFormCountdown (struct Mch_Match *Match,long Seconds,const char *Color)
  {
   extern const char *Txt_Countdown;
   static const char *fmt[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = "class=\"BT_LINK_OFF MCH_BUTTON_HIDDEN\"",
      [Frm_PUT_FORM     ] = "class=\"BT_LINK MCH_BUTTON_ON\"",
     };
   char *OnSubmit;
   Frm_PutForm_t PutForm = (Match->Status.Showing != Mch_END) ? Frm_PUT_FORM :
								Frm_DONT_PUT_FORM;
   const char *Title[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = Txt_Countdown,
     };

   if (PutForm == Frm_PUT_FORM)
     {
      /***** Begin form *****/
      if (asprintf (&OnSubmit,"updateMatchTch('match_left',"
			      "'act=%ld&ses=%s&MchCod=%ld&Countdown=%ld');"
			      " return false;",	// return false is necessary to not submit form
		    Act_GetActCod (ActMchCntDwn),Gbl.Session.Id,
		    Match->MchCod,Seconds) < 0)
	 Err_NotEnoughMemoryExit ();
      Frm_BeginFormOnSubmit (ActUnk,OnSubmit);
     }

      /***** Put icon *****/
      HTM_DIV_Begin ("class=\"MCH_SMALLBUTTON_CONT %s\"",Color);

	 HTM_BUTTON_Submit_Begin (Title[PutForm],NULL,fmt[PutForm]);

	    HTM_NBSP ();
	    if (Seconds >= 0)
	      {
	       HTM_Long (Seconds); HTM_Seconds ();
	      }
	    else
	      {
	       HTM_Infinite (); HTM_NBSP ();
	      }

	 HTM_BUTTON_End ();

      HTM_DIV_End ();

   /***** End form *****/
   if (PutForm == Frm_PUT_FORM)
     {
      Frm_EndForm ();
      free (OnSubmit);
     }
  }

/*****************************************************************************/
/********** Show right column when playing a match (as a teacher) ************/
/*****************************************************************************/

static void Mch_ShowRightColumnTch (const struct Mch_Match *Match)
  {
   /***** Begin right container *****/
   HTM_DIV_Begin ("class=\"MCH_RIGHT_TCH\"");

      /***** Top row: match title *****/
      Mch_ShowMatchTitleTch (Match);

      /***** Bottom row: current question and possible answers *****/
      if (Match->Status.Showing == Mch_END)	// Match over
	 Mch_ShowMatchScore (Match);
      else					// Match not over
	 Mch_ShowQuestionAndAnswersTch (Match);

   /***** End right container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********** Show left column when playing a match (as a student) ************/
/*****************************************************************************/

static void Mch_ShowLeftColumnStd (const struct Mch_Match *Match,
				   const struct Mch_UsrAnswer *UsrAnswer)
  {
   bool Answered = UsrAnswer->NumOpt >= 0;

   /***** Begin left container *****/
   HTM_DIV_Begin ("class=\"MCH_LEFT_STD\"");

      /***** Top *****/
      HTM_DIV_Begin ("class=\"MCH_TOP CT\"");
      HTM_DIV_End ();

      /***** Write number of question *****/
      Mch_ShowNumQstInMch (Match);

      switch (Match->Status.Showing)
	{
	 case Mch_START:
	 case Mch_END:
	    break;
	 default:
	    /***** Write whether question is answered or not *****/
	    Mch_PutIfAnswered (Match,Answered);

	    if (Match->Status.Playing &&		// Match is being played
		Match->Status.Showing == Mch_ANSWERS &&	// Teacher's screen is showing question answers
		Answered)				// I have answered this question
	       /***** Put icon to remove my answet *****/
	       Mch_PutIconToRemoveMyAnswer (Match);
	    break;
	}

   /***** End left container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Show right column when playing a match (as a student) ************/
/*****************************************************************************/

static void Mch_ShowRightColumnStd (struct Mch_Match *Match,
				    const struct Mch_UsrAnswer *UsrAnswer,
				    Mch_Update_t Update)
  {
   extern const char *Txt_Please_wait_;

   /***** Begin right container *****/
   HTM_DIV_Begin ("class=\"MCH_RIGHT_STD\"");

      /***** Top row *****/
      Mch_ShowMatchTitleStd (Match);

      /***** Bottom row *****/
      if (Match->Status.Playing)		// Match is being played
	{
	 if (Match->Status.Showing == Mch_END)	// Match over
	    Mch_ShowWaitImage (Txt_Please_wait_);
	 else					// Match not over
	   {
	    HTM_DIV_Begin ("class=\"MCH_BOTTOM\"");

	       /***** Update players ******/
	       if (Mch_RegisterMeAsPlayerInMatch (Match))
		 {
		  if (Match->Status.Showing == Mch_ANSWERS)	// Teacher's screen is showing question answers
		     /* Show current question and possible answers */
		     Mch_ShowQuestionAndAnswersStd (Match,UsrAnswer,Update);
		 }
	       else
		  Ale_ShowAlert (Ale_ERROR,"You can not join this match.");

	    HTM_DIV_End ();
	   }
	}
      else					// Match is not being played
	 Mch_ShowWaitImage (Txt_Please_wait_);

   /***** End right container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show number of question in game ***********************/
/*****************************************************************************/

static void Mch_ShowNumQstInMch (const struct Mch_Match *Match)
  {
   extern const char *Txt_MATCH_Start;
   extern const char *Txt_MATCH_End;
   unsigned NumQsts = Gam_DB_GetNumQstsGame (Match->GamCod);

   HTM_DIV_Begin ("class=\"MCH_NUM_QST\"");
      switch (Match->Status.Showing)
	{
	 case Mch_START:	// Not started
	    HTM_Txt (Txt_MATCH_Start);
	    break;
	 case Mch_END:		// Match over
	    HTM_Txt (Txt_MATCH_End);
	    break;
	 default:
	    HTM_Unsigned (Match->Status.QstInd); HTM_Slash ();
	    HTM_Unsigned (NumQsts);
	    break;
	}
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Put buttons to control a match ***********************/
/*****************************************************************************/

static void Mch_PutMatchControlButtons (const struct Mch_Match *Match)
  {
   extern const char *Txt_Go_back;
   extern const char *Txt_Go_forward;
   extern const char *Txt_Pause;
   extern const char *Txt_Start;
   extern const char *Txt_Resume;

   /***** Begin buttons container *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTONS_CONT\"");

      /***** Left button *****/
      HTM_DIV_Begin ("class=\"MCH_BUTTON_LEFT_CONT\"");
	 switch (Match->Status.Showing)
	   {
	    case Mch_START:
	       /* Put disabled button to go back.
	          Previously there was a button with a red cross
	          to close the browser tab using window.close,
	          but it no longer works in Firefox. */
	       Mch_PutBigButtonHidden (Mch_ICON_PREVIOUS);
	       break;
	    default:
	       /* Put button to go back */
	       Mch_PutBigButton (ActBckMch,"backward",Match->MchCod,
				 Mch_ICON_PREVIOUS,Txt_Go_back);
	       break;
	   }
      HTM_DIV_End ();

      /***** Center button *****/
      HTM_DIV_Begin ("class=\"MCH_BUTTON_CENTER_CONT\"");
	 if (Match->Status.Playing)					// Match is being played
	    /* Put button to pause match */
	    Mch_PutBigButton (ActPlyPauMch,"play_pause",Match->MchCod,
			      Mch_ICON_PAUSE,Txt_Pause);
	 else								// Match is paused, not being played
	   {
	    switch (Match->Status.Showing)
	      {
	       case Mch_START:	// Match just started, before first question
		  /* Put button to start playing match */
		  Mch_PutBigButton (ActPlyPauMch,"play_pause",Match->MchCod,
				    Mch_ICON_PLAY,Txt_Start);
		  break;
	       case Mch_END:	// Match over
		  /* Put disabled button to play match */
		  Mch_PutBigButtonHidden (Mch_ICON_PLAY);
		  break;
	       default:
		  /* Put button to resume match */
		  Mch_PutBigButton (ActPlyPauMch,"play_pause",Match->MchCod,
				    Mch_ICON_PLAY,Txt_Resume);
	      }
	   }
      HTM_DIV_End ();

      /***** Right button *****/
      HTM_DIV_Begin ("class=\"MCH_BUTTON_RIGHT_CONT\"");
	 switch (Match->Status.Showing)
	   {
	    case Mch_END:	// Match over
	       /* Put disabled button to go forward.
	          Previously there was a button with a red cross
	          to close the browser tab using window.close,
	          but it no longer works in Firefox. */
	       Mch_PutBigButtonHidden (Mch_ICON_NEXT);
	       break;
	    default:		// Match not over
	       /* Put button to show answers */
	       Mch_PutBigButton (ActFwdMch,"forward",Match->MchCod,
				 Mch_ICON_NEXT,Txt_Go_forward);
	       break;
	   }
      HTM_DIV_End ();

   /***** End buttons container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/** Show form to choice whether to show answers in one column or two columns */
/*****************************************************************************/

static void Mch_ShowFormColumns (const struct Mch_Match *Match)
  {
   extern const char *Txt_column;
   extern const char *Txt_columns;
   char *Title;
   unsigned NumCols;
   static const char *NumColsIcon[1 + Mch_MAX_COLS] =
     {
      "",		// Not used
      "1col.png",	// 1 column
      "2col.png",	// 2 columns
      "3col.png",	// 3 columns
      "4col.png",	// 4 columns
     };

   Set_BeginOneSettingSelector ();
      for (NumCols  = 1;
	   NumCols <= Mch_MAX_COLS;
	   NumCols++)
	{
	 /* Begin container for this option */
	 HTM_DIV_Begin ("class=\"%s\"",
			(Match->Status.NumCols == NumCols) ? "MCH_NUM_COL_ON" :
							     "MCH_NUM_COL_OFF");

	    /* Begin form */
	    Frm_BeginForm (ActChgNumColMch);
	       ParCod_PutPar (ParCod_Mch,Match->MchCod);	// Current match being played
	       Mch_PutParNumCols (NumCols);		// Number of columns

	       /* Number of columns */
	       if (asprintf (&Title,"%u %s",NumCols,
					    NumCols == 1 ? Txt_column :
							   Txt_columns) < 0)
		  Err_NotEnoughMemoryExit ();
	       Ico_PutSettingIconLink (NumColsIcon[NumCols],Ico_BLACK,Title);
	       free (Title);

	    /* End form */
	    Frm_EndForm ();

	 /* End container for this option */
	 HTM_DIV_End ();
	}
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/******** Write parameter with number of columns in answers of match *********/
/*****************************************************************************/

static void Mch_PutParNumCols (unsigned NumCols)	// Number of columns
  {
   Par_PutParUnsigned (NULL,"NumCols",NumCols);
  }

/*****************************************************************************/
/***************** Put checkbox to select if show results ********************/
/*****************************************************************************/

static void Mch_PutCheckboxResult (const struct Mch_Match *Match)
  {
   extern const char *Txt_View_results;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"MCH_SHOW_RESULTS DAT_%s\"",The_GetSuffix ());

      /***** Begin form *****/
      Frm_BeginForm (ActChgVisResMchQst);
	 ParCod_PutPar (ParCod_Mch,Match->MchCod);	// Current match being played

	 /***** Put icon with link *****/
	 HTM_BUTTON_Submit_Begin (Act_GetActionText (ActChgVisResMchQst),NULL,
	                          "class=\"BT_LINK ICO_HIGHLIGHT\"");
	    HTM_TxtF ("<i class=\"%s\"></i>",
		      Match->Status.ShowQstResults ? "fas fa-toggle-on" :
						     "fas fa-toggle-off");
	    HTM_NBSP ();
	    HTM_Txt (Txt_View_results);
	 HTM_BUTTON_End ();

      /***** End form *****/
      Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Put checkbox to select if show results ********************/
/*****************************************************************************/

static void Mch_PutIfAnswered (const struct Mch_Match *Match,bool Answered)
  {
   extern const char *Txt_MATCH_QUESTION_Answered;
   extern const char *Txt_MATCH_QUESTION_Unanswered;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"MCH_SHOW_ANSWERED\"");

      /***** Put icon with link *****/
      if (Match->Status.Playing &&			// Match is being played
	  Match->Status.Showing == Mch_ANSWERS &&	// Teacher's screen is showing question answers
	  Answered)				// I have answered this question
	{
	 /* Begin form */
	 Frm_BeginForm (ActSeeMchAnsQstStd);
	    ParCod_PutPar (ParCod_Mch,Match->MchCod);	// Current match being played

	    HTM_BUTTON_Submit_Begin (Act_GetActionText (ActSeeMchAnsQstStd),NULL,
	                             "class=\"BT_LINK DAT_SMALL_GREEN_%s\""
	                             " onmousedown=\"this.form.submit();return false;\"",
	                             The_GetSuffix ());
	       HTM_TxtF ("<i class=\"%s\"></i>","fas fa-check-circle");
	       HTM_NBSP ();
	       HTM_Txt (Txt_MATCH_QUESTION_Answered);
	    HTM_BUTTON_End ();

	 /* End form */
	 Frm_EndForm ();
	}
      else
	{
	 HTM_DIV_Begin ("class=\"%s_%s\"",Answered ? "DAT_SMALL_GREEN" :
		                                     "DAT_SMALL_RED",
			The_GetSuffix ());
	    HTM_TxtF ("<i class=\"%s\" title=\"%s\"></i>",
		      Answered ? "fas fa-check-circle" :
				 "fas fa-exclamation-circle",
		      Answered ? Txt_MATCH_QUESTION_Answered :
				 Txt_MATCH_QUESTION_Unanswered);
	    HTM_NBSP ();
	    HTM_Txt (Answered ? Txt_MATCH_QUESTION_Answered :
				Txt_MATCH_QUESTION_Unanswered);
	 HTM_DIV_End ();
	}

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Put checkbox to remove my answer ***********************/
/*****************************************************************************/

static void Mch_PutIconToRemoveMyAnswer (const struct Mch_Match *Match)
  {
   extern const char *Txt_Delete_my_answer;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"MCH_REM_MY_ANS\"");

      /***** Begin form *****/
      Frm_BeginForm (ActRemMchAnsQstStd);
	 ParCod_PutPar (ParCod_Mch,Match->MchCod);	// Current match being played
	 Gam_PutParQstInd (Match->Status.QstInd);	// Current question index shown

	 /***** Put icon with link *****/
	 HTM_DIV_Begin ("class=\"MCH_BIGBUTTON_CONT\"");
	    HTM_BUTTON_Submit_Begin (Txt_Delete_my_answer,NULL,
	                             "BT_LINK MCH_BUTTON_ON ICO_DARKRED\""
	                             " onmousedown=\"this.form.submit();return false;\"");
	       HTM_Txt ("<i class=\"fas fa-trash\"></i>");
	    HTM_BUTTON_End ();
	 HTM_DIV_End ();

      /***** End form *****/
      Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************************** Show match title ******************************/
/*****************************************************************************/

static void Mch_ShowMatchTitleTch (const struct Mch_Match *Match)
  {
   /***** Match title *****/
   HTM_DIV_Begin ("class=\"MCH_TOP LT\"");
      HTM_Txt (Match->Title);
   HTM_DIV_End ();
  }

static void Mch_ShowMatchTitleStd (const struct Mch_Match *Match)
  {
   /***** Match title *****/
   HTM_DIV_Begin ("class=\"MCH_TOP CT\"");
      HTM_Txt (Match->Title);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a teacher) *****/
/*****************************************************************************/

static void Mch_ShowQuestionAndAnswersTch (const struct Mch_Match *Match)
  {
   extern const char *Txt_MATCH_Paused;
   extern const char *Txt_Question_removed;
   struct Qst_Question Question;

   /***** Create test question *****/
   Qst_QstConstructor (&Question);
   Question.QstCod = Match->Status.QstCod;

   /***** Trivial check: do not show anything on match start and end *****/
   switch (Match->Status.Showing)
     {
      case Mch_START:
      case Mch_END:
	 return;
      default:
	 break;
     }

   /***** Get data of question from database *****/
   if (Qst_GetQstDataByCod (&Question))
     {
      /***** Show question *****/
      /* Check answer type */
      if (Question.Answer.Type != Qst_ANS_UNIQUE_CHOICE)
	 Err_WrongAnswerExit ();

      /* Begin container */
      HTM_DIV_Begin ("class=\"MCH_BOTTOM\"");	// Bottom

	 /* Write stem */
	 Qst_WriteQstStem (Question.Stem,"MCH_TCH_STEM",HidVis_VISIBLE);

	 /* Show media */
	 Med_ShowMedia (&Question.Media,
			"Tst_MED_EDIT_LIST_CONT",
			"Tst_MED_EDIT_LIST");

	 /***** Write answers? *****/
	 switch (Match->Status.Showing)
	   {
	    case Mch_ANSWERS:
	       if (Match->Status.Playing)		// Match is being played
		  /* Write answers */
		  Mch_WriteAnswersMatchResult (Match,
					       &Question,
					       "MCH_TCH_ANS",
					       false);	// Don't show result
	       else					// Match is paused, not being played
		  Mch_ShowWaitImage (Txt_MATCH_Paused);
	       break;
	    case Mch_RESULTS:
	       /* Write answers with results */
	       Mch_WriteAnswersMatchResult (Match,
					    &Question,
					    "MCH_TCH_ANS",
					    true);	// Show result
	       break;
	    default:
	       /* Don't write anything */
	       break;
	   }

      /* End container */
      HTM_DIV_End ();				// Bottom
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Question_removed);

   /***** Destroy test question *****/
   Qst_QstDestructor (&Question);
  }

/*****************************************************************************/
/************* Write answers of a question when seeing a match ***************/
/*****************************************************************************/

static void Mch_WriteAnswersMatchResult (const struct Mch_Match *Match,
                                         struct Qst_Question *Question,
                                         const char *Class,bool ShowResult)
  {
   /***** Write answer depending on type *****/
   if (Question->Answer.Type == Qst_ANS_UNIQUE_CHOICE)
      Mch_WriteChoiceAnsViewMatch (Match,
				   Question,
				   Class,ShowResult);
   else
      Err_WrongAnswerTypeExit ();
  }

/*****************************************************************************/
/******** Write single or multiple choice answer when seeing a match *********/
/*****************************************************************************/

static void Mch_WriteChoiceAnsViewMatch (const struct Mch_Match *Match,
                                         struct Qst_Question *Question,
                                         const char *Class,bool ShowResult)
  {
   unsigned NumOpt;
   bool RowIsOpen = false;
   unsigned NumRespondersQst;
   unsigned NumRespondersAns;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question

   /***** Get number of users who have answered this question from database *****/
   NumRespondersQst = Mch_DB_GetNumUsrsWhoAnsweredQst (Match->MchCod,Match->Status.QstInd);

   /***** Change format of answers text *****/
   Qst_ChangeFormatAnswersText (Question);

   /***** Get indexes for this question in match *****/
   Mch_GetIndexes (Match->MchCod,Match->Status.QstInd,Indexes);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (0);

      /***** Show options distributed in columns *****/
      for (NumOpt = 0;
	   NumOpt < Question->Answer.NumOptions;
	   NumOpt++)
	{
	 /***** Begin row? *****/
	 if (NumOpt % Match->Status.NumCols == 0)
	   {
	    HTM_TR_Begin (NULL);
	    RowIsOpen = true;
	   }

	    /***** Write letter for this option *****/
	    HTM_TD_Begin ("class=\"MCH_TCH_BUTTON_TD\"");
	       HTM_DIV_Begin ("class=\"MCH_TCH_BUTTON BT_%c\"",'A' + (char) NumOpt);
		  HTM_Option (NumOpt);
	       HTM_DIV_End ();
	    HTM_TD_End ();

	    /***** Write the option text and the result *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"Ans%06u_%u\" class=\"%s\"",Match->Status.QstInd,NumOpt,Class);
		  HTM_Txt (Question->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Question->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT",
			      "Tst_MED_SHOW");

	       /* Show result (number of users who answered? */
	       if (ShowResult)
		 {
		  /* Get number of users who selected this answer */
		  NumRespondersAns = Mch_DB_GetNumUsrsWhoHaveChosenAns (Match->MchCod,Match->Status.QstInd,Indexes[NumOpt]);

		  /* Draw proportional bar for this answer */
		  Mch_DrawBarNumUsrs (NumRespondersAns,NumRespondersQst,
				      Question->Answer.Options[Indexes[NumOpt]].Correct);
		 }
	       else
		  /* Draw empty bar for this answer
		     in order to show the same layout that the one shown with results */
		  Mch_DrawBarNumUsrs (0,0,
				      Qst_WRONG);	// Not used when length of bar is 0

	    HTM_TD_End ();

	 /***** End row? *****/
	 if (NumOpt % Match->Status.NumCols == Match->Status.NumCols - 1)
	   {
	    HTM_TR_End ();
	    RowIsOpen = false;
	   }
	}

      /***** End row? *****/
      if (RowIsOpen)
	 HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a student) *****/
/*****************************************************************************/

static void Mch_ShowQuestionAndAnswersStd (const struct Mch_Match *Match,
					   const struct Mch_UsrAnswer *UsrAnswer,
					   Mch_Update_t Update)
  {
   unsigned NumOptions;
   unsigned NumOpt;

   /***** Get number of options in this question *****/
   NumOptions = Qst_DB_GetNumAnswersQst (Match->Status.QstCod);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (8);

      for (NumOpt = 0;
	   NumOpt < NumOptions;
	   NumOpt++)
	{
	 /***** Begin row *****/
	 HTM_TR_Begin (NULL);

	    /***** Write letter for this option *****/
	    /* Begin table cell */
	    HTM_TD_Begin ("class=\"MCH_STD_CELL\"");

	       /* Form with button.
		  Sumitting onmousedown instead of default onclick
		  is necessary in order to be fast
		  and not lose clicks due to refresh */
	       Frm_BeginForm (ActAnsMchQstStd);
		  ParCod_PutPar (ParCod_Mch,Match->MchCod);	// Current match being played
		  Gam_PutParQstInd (Match->Status.QstInd);	// Current question index shown
		  Mch_PutParNumOpt (NumOpt);			// Number of button

		  HTM_BUTTON_Submit_Begin (NULL,NULL,
					   "class=\"MCH_STD_BUTTON%s BT_%c\""
					   " onmousedown=\"this.form.submit();return false;\"",
				           UsrAnswer->NumOpt == (int) NumOpt &&	// Student's answer
					   Update == Mch_CHANGE_STATUS_BY_STUDENT ? " MCH_STD_ANSWER_SELECTED" :
										    "",
					   'A' + (char) NumOpt);
		     HTM_Option (NumOpt);
		  HTM_BUTTON_End ();

	       Frm_EndForm ();

	    /* End table cell */
	    HTM_TD_End ();

	 /***** End row *****/
	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************************** Show match scores *****************************/
/*****************************************************************************/

#define Mch_NUM_ROWS_SCORE 50

static void Mch_ShowMatchScore (const struct Mch_Match *Match)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumScores;
   unsigned NumScore;
   double MinScore;
   double MaxScore;
   double Range;
   double NumRowsPerScorePoint;
   double Score;
   unsigned MaxUsrs;
   unsigned NumUsrs;
   unsigned NumRowForThisScore;
   unsigned NumRow;

   /***** Get minimum and maximum scores *****/
   Gam_GetScoreRange (Match->GamCod,&MinScore,&MaxScore);
   Range = MaxScore - MinScore;
   if (Range == 0.0)
      return;

   NumRowsPerScorePoint = (double) Mch_NUM_ROWS_SCORE / Range;

   /***** Get maximum number of users *****/
   MaxUsrs = Mch_DB_GetMaxUsrsPerScore (Match->MchCod);

   /***** Get scores from database *****/
   NumScores = Mch_DB_GetNumUsrsPerScore (&mysql_res,Match->MchCod);

   /***** Begin table ****/
   HTM_TABLE_BeginWide ();

      /***** Get and draw scores *****/
      for (NumScore = 0, NumRow = 0;
	   NumScore < NumScores;
	   NumScore++)
	{
	 /***** Get score and number of users from database *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get score (row[0]) */
	 Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
	 if (sscanf (row[0],"%lf",&Score) != 1)
	    Score = 0.0;
	 Str_SetDecimalPointToLocal ();	// Return to local system

	 /* Get number of users (row[1]) *****/
	 if (sscanf (row[1],"%u",&NumUsrs) != 1)
	    NumUsrs = 0;

	 /***** Draw empty rows until reaching the adequate row *****/
	 NumRowForThisScore = (unsigned) ((MaxScore - Score) * NumRowsPerScorePoint);
	 if (NumRowForThisScore == Mch_NUM_ROWS_SCORE)
	    NumRowForThisScore = Mch_NUM_ROWS_SCORE - 1;
	 for (;
	      NumRow < NumRowForThisScore;
	      NumRow++)
	    Mch_DrawEmptyScoreRow (NumRow,MinScore,MaxScore);

	 /***** Draw row for this score *****/
	 Mch_DrawScoreRow (Score,MinScore,MaxScore,NumRow,NumUsrs,MaxUsrs);

	 NumRow++;
	}

      /***** Draw final empty rows *****/
      for (;
	   NumRow < Mch_NUM_ROWS_SCORE;
	   NumRow++)
	 Mch_DrawEmptyScoreRow (NumRow,MinScore,MaxScore);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Draw empty score row ****************************/
/*****************************************************************************/

static void Mch_DrawEmptyScoreRow (unsigned NumRow,double MinScore,double MaxScore)
  {
   /***** Draw row *****/
   HTM_TR_Begin (NULL);

      /* Write score */
      HTM_TD_Begin ("class=\"MCH_SCO_SCO\"");
	 if (NumRow == 0)
	   {
	    HTM_DoubleFewDigits (MaxScore);
	    HTM_NBSP ();
	   }
	 else if (NumRow == Mch_NUM_ROWS_SCORE - 1)
	   {
	    HTM_DoubleFewDigits (MinScore);
	    HTM_NBSP ();
	   }
      HTM_TD_End ();

      /* Empty column with bar and number of users */
      HTM_TD_Begin ("class=\"MCH_SCO_NUM%s\"",Mch_GetClassBorder (NumRow));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************************* Draw score row ******************************/
/*****************************************************************************/

static void Mch_DrawScoreRow (double Score,double MinScore,double MaxScore,
			      unsigned NumRow,unsigned NumUsrs,unsigned MaxUsrs)
  {
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   unsigned Color;
   unsigned BarWidth;
   char *Icon;
   char *Title;

   /***** Compute color *****/
   /*
   +----------------- MaxScore
   | score9_1x1.png
   +-----------------
   | score8_1x1.png
   +-----------------
   | score7_1x1.png
   +-----------------
   | score6_1x1.png
   +-----------------
   | score5_1x1.png
   +-----------------
   | score4_1x1.png
   +-----------------
   | score3_1x1.png
   +-----------------
   | score2_1x1.png
   +-----------------
   | score1_1x1.png
   +-----------------
   | score0_1x1.png
   +----------------- MinScore
   */
   Color = (unsigned) (((Score - MinScore) / (MaxScore - MinScore)) * 10.0);
   if (Color == 10)
      Color = 9;

   /***** Compute bar width *****/
   if (MaxUsrs > 0)
     {
      BarWidth = (unsigned) (((NumUsrs * 95.0) / MaxUsrs) + 0.5);
      if (BarWidth == 0)
	 BarWidth = 1;
     }
   else
      BarWidth = 0;

   /***** Draw row *****/
   HTM_TR_Begin (NULL);

      /* Write score */
      HTM_TD_Begin ("class=\"MCH_SCO_SCO\"");
	 HTM_DoubleFewDigits (Score);
	 HTM_NBSP ();
      HTM_TD_End ();

      /* Draw bar and write number of users for this score */
      HTM_TD_Begin ("class=\"MCH_SCO_NUM%s\"",Mch_GetClassBorder (NumRow));
	 if (asprintf (&Icon,"score%u_1x1.png",Color) < 0)	// Background
	    Err_NotEnoughMemoryExit ();
	 if (asprintf (&Title,"%u %s",NumUsrs,
				      NumUsrs == 1 ? Txt_ROLES_SINGUL_abc[Rol_STD][Usr_SEX_UNKNOWN] :
						     Txt_ROLES_PLURAL_abc[Rol_STD][Usr_SEX_UNKNOWN]) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_IMG (Cfg_URL_ICON_PUBLIC,Icon,Title,
		  "class=\"MCH_SCO_BAR\" style=\"width:%u%%;\"",BarWidth);
	 free (Title);
	 free (Icon);
	 HTM_NBSP (); HTM_Unsigned (NumUsrs);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/****** Write parameter with number of option (button) pressed by user *******/
/*****************************************************************************/

static const char *Mch_GetClassBorder (unsigned NumRow)
  {
   return NumRow == 0                      ? " MCH_SCO_TOP" :
	 (NumRow == Mch_NUM_ROWS_SCORE - 1 ? " MCH_SCO_BOT" :
		                             " MCH_SCO_MID");
  }

/*****************************************************************************/
/****** Write parameter with number of option (button) pressed by user *******/
/*****************************************************************************/

static void Mch_PutParNumOpt (unsigned NumOpt)
  {
   Par_PutParUnsigned (NULL,"NumOpt",NumOpt);
  }

/*****************************************************************************/
/******* Get parameter with number of option (button) pressed by user ********/
/*****************************************************************************/

static unsigned Mch_GetParNumOpt (void)
  {
   long NumOpt;

   NumOpt = Par_GetParLong ("NumOpt");
   if (NumOpt < 0)
      Err_WrongAnswerExit ();

   return (unsigned) NumOpt;
  }

/*****************************************************************************/
/*********************** Put a big button to do action ***********************/
/*****************************************************************************/

static void Mch_PutBigButton (Act_Action_t NextAction,const char *Id,
			      long MchCod,const char *Icon,const char *Txt)
  {
   /***** Begin form *****/
   Frm_BeginFormId (NextAction,Id);
      ParCod_PutPar (ParCod_Mch,MchCod);

      /***** Put icon with link *****/
      HTM_DIV_Begin ("class=\"MCH_BIGBUTTON_CONT\"");
	 HTM_BUTTON_Submit_Begin (Txt,NULL,
	                          "class=\"BT_LINK MCH_BUTTON_ON ICO_BLACK_%s\"",
	                          The_GetSuffix ());
	    HTM_TxtF ("<i class=\"%s\"></i>",Icon);
	 HTM_BUTTON_End ();
      HTM_DIV_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************** Put a big button hidden **************************/
/*****************************************************************************/

static void Mch_PutBigButtonHidden (const char *Icon)
  {
   /***** Put inactive icon *****/
   HTM_DIV_Begin ("class=\"MCH_BIGBUTTON_CONT\"");
      HTM_BUTTON_Begin (NULL,"class=\"BT_LINK_OFF MCH_BUTTON_HIDDEN ICO_BLACK_%s\"",
                        The_GetSuffix ());
	 HTM_TxtF ("<i class=\"%s\"></i>",Icon);
      HTM_BUTTON_End ();
   HTM_DIV_End ();
  }
/*****************************************************************************/
/********************** Put a big button to close window *********************/
/*****************************************************************************/
/*
static void Mch_PutBigButtonClose (void)
  {
   extern const char *Txt_Close;

   ***** Put icon with link *****
   HTM_DIV_Begin ("class=\"MCH_BIGBUTTON_CONT\"");
      HTM_BUTTON_Begin (Txt_Close,
                        "class=\"BT_LINK MCH_BUTTON_ON ICO_DARKRED\""
                        " onclick=\"window.close();return false;\"");
	 HTM_TxtF ("<i class=\"%s\"></i>",Mch_ICON_CLOSE);
      HTM_BUTTON_End ();
   HTM_DIV_End ();
  }
*/
/*****************************************************************************/
/****************************** Show wait image ******************************/
/*****************************************************************************/

static void Mch_ShowWaitImage (const char *Txt)
  {
   HTM_DIV_Begin ("class=\"MCH_WAIT_CONT\"");
      Ico_PutIcon ("Spin-1s-200px.gif",Ico_UNCHANGED,Txt,"MCH_WAIT_IMG");
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************* Register me as a player in a match **********************/
/*****************************************************************************/
// Return true on success

bool Mch_RegisterMeAsPlayerInMatch (struct Mch_Match *Match)
  {
   /***** Trivial check: match code must be > 0 *****/
   if (Match->MchCod <= 0)
      return false;

   /***** Trivial check: match must be being played *****/
   if (!Match->Status.Playing)				// Match is paused, not being played
      return false;

   /***** Trivial check: match must not be over *****/
   if (Match->Status.Showing == Mch_END)		// Match over
      return false;

   /***** Trivial check: only a student can join a match *****/
   if (Gbl.Usrs.Me.Role.Logged != Rol_STD)		// I am not logged as student
      return false;

   /***** Insert me as match player *****/
   Mch_DB_RegisterMeAsPlayerInMatch (Match->MchCod);
   return true;
  }

/*****************************************************************************/
/********************** Get code of match being played ***********************/
/*****************************************************************************/

void Mch_GetMatchBeingPlayed (void)
  {
   long MchCodBeingPlayed;

   /***** Get match code ****/
   MchCodBeingPlayed = ParCod_GetAndCheckPar (ParCod_Mch);

   Mch_SetMchCodBeingPlayed (MchCodBeingPlayed);
  }

/*****************************************************************************/
/********************* Show match being played as student ********************/
/*****************************************************************************/

void Mch_JoinMatchAsStd (void)
  {
   struct Mch_Match Match;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForStd (&Match,Mch_CHANGE_STATUS_BY_STUDENT);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****** Remove student's answer to a question and show match as student ******/
/*****************************************************************************/

void Mch_RemMyQstAnsAndShowMchStatus (void)
  {
   struct Mch_Match Match;
   unsigned QstInd;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Get question index from form *****/
   QstInd = Gam_GetParQstInd ();

   /***** Remove my answer to this question *****/
   Mch_RemoveMyQuestionAnswer (&Match,QstInd);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForStd (&Match,Mch_CHANGE_STATUS_BY_STUDENT);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Remove student's answer to a question ******************/
/*****************************************************************************/

void Mch_RemoveMyQuestionAnswer (const struct Mch_Match *Match,unsigned QstInd)
  {
   /***** Check that teacher's screen is showing answers
          and question index is the current one being played *****/
   if (Match->Status.Playing &&			// Match is being played
       Match->Status.Showing == Mch_ANSWERS &&	// Teacher's screen is showing answers
       QstInd == Match->Status.QstInd)		// Removing answer to the current question being played
     {
      /***** Remove my answer to this question *****/
      Mch_DB_RemoveMyAnswerToMatchQuestion (Match);

      /***** Compute score and update my match result *****/
      MchPrn_ComputeScoreAndUpdateMyMatchPrintInDB (Match->MchCod);
     }
  }

/*****************************************************************************/
/******************** Start match countdown (by a teacher) *******************/
/*****************************************************************************/

void Mch_StartCountdown (void)
  {
   struct Mch_Match Match;
   long NewCountdown;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Get countdown parameter ****/
   NewCountdown = Par_GetParLong ("Countdown");

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Start countdown *****/
   Match.Status.Countdown = NewCountdown;

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   Mch_ShowRefreshablePartTch (&Match);
  }

/*****************************************************************************/
/****************** Refresh match for a teacher via AJAX *********************/
/*****************************************************************************/

void Mch_RefreshMatchTch (void)
  {
   struct Mch_Match Match;
   enum {REFRESH_LEFT,REFRESH_ALL} WhatToRefresh;

   /***** Trivial check: if no current session, don't do anything *****/
   if (Gbl.Session.Status != Ses_OPEN)
      return;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_DB_RemoveOldPlaying ();

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Update countdown *****/
   // If current countdown is < 0 ==> no countdown in progress
   WhatToRefresh = REFRESH_LEFT;
   if (Match.Status.Playing &&		// Match is being played
       Match.Status.Countdown >= 0)	// Countdown in progress
     {
      /* Decrease countdown */
      Match.Status.Countdown -= Cfg_SECONDS_TO_REFRESH_MATCH_TCH;

      /* On countdown reached, set match status to next (forward) status */
      if (Match.Status.Countdown <= 0)	// End of countdown reached
	{
	 Mch_SetMatchStatusToNext (&Match);
	 WhatToRefresh = REFRESH_ALL;	// Refresh the whole page
	}
     }

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Update elapsed time in this question *****/
   Mch_UpdateElapsedTimeInQuestion (&Match);

   /***** Show current match status *****/
   switch (WhatToRefresh)
     {
      case REFRESH_LEFT:	// Refresh only left part
         HTM_Txt ("match_left|0|");	// 0 ==> do not evaluate MatJax scripts after updating HTML
         Mch_ShowRefreshablePartTch (&Match);
         break;
      case REFRESH_ALL:		// Refresh the whole page
         HTM_Txt ("match|1|");		// 1 ==> evaluate MatJax scripts after updating HTML
         Mch_ShowMatchStatusForTch (&Match);
         break;
     }
  }

/*****************************************************************************/
/*************** Refresh current game for a student via AJAX *****************/
/*****************************************************************************/

void Mch_RefreshMatchStd (void)
  {
   struct Mch_Match Match;

   /***** Trivial check: if no current session, don't do anything *****/
   if (Gbl.Session.Status != Ses_OPEN)
      return;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Show current match status *****/
   Mch_ShowMatchStatusForStd (&Match,Mch_REFRESH_STATUS_BY_SERVER);
  }

/*****************************************************************************/
/**** Receive previous question answer in a match question from database *****/
/*****************************************************************************/

void Mch_GetQstAnsFromDB (long MchCod,long UsrCod,unsigned QstInd,
		          struct Mch_UsrAnswer *UsrAnswer)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Set default values for number of option and answer index *****/
   UsrAnswer->NumOpt = -1;	// < 0 ==> no answer selected
   UsrAnswer->AnsInd = -1;	// < 0 ==> no answer selected

   /***** Get student's answer *****/
   if (Mch_DB_GetUsrAnsToQst (&mysql_res,MchCod,UsrCod,QstInd)) // Answer found...
     {
      row = mysql_fetch_row (mysql_res);

      /***** Get number of option index (row[0]) *****/
      if (sscanf (row[0],"%d",&(UsrAnswer->NumOpt)) != 1)
         Err_WrongAnswerExit ();

      /***** Get answer index (row[1]) *****/
      if (sscanf (row[1],"%d",&(UsrAnswer->AnsInd)) != 1)
         Err_WrongAnswerIndexExit ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Receive question answer from student when playing a match *********/
/*****************************************************************************/

void Mch_ReceiveQuestionAnswer (void)
  {
   struct Mch_Match Match;
   unsigned QstInd;	// 0 means that the game has not started. First question has index 1.
   struct Mch_UsrAnswer UsrAnswer;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   /***** Get data of the match from database *****/
   Match.MchCod = Mch_GetMchCodBeingPlayed ();
   Mch_GetMatchDataByCod (&Match);

   /***** Get question index from form *****/
   QstInd = Gam_GetParQstInd ();

   /***** Get number of option selected by student from form *****/
   UsrAnswer.NumOpt = Mch_GetParNumOpt ();

   /***** Store answer *****/
   Mch_StoreQuestionAnswer (&Match,QstInd,&UsrAnswer);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
      Mch_ShowMatchStatusForStd (&Match,Mch_CHANGE_STATUS_BY_STUDENT);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Store question answer from student when playing a match **********/
/*****************************************************************************/

void Mch_StoreQuestionAnswer (const struct Mch_Match *Match,unsigned QstInd,
                              struct Mch_UsrAnswer *UsrAnswer)
  {
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];
   struct Mch_UsrAnswer PreviousUsrAnswer;

   /***** Check that teacher's screen is showing answers
          and question index is the current one being played *****/
   if (Match->Status.Playing &&			// Match is being played
       Match->Status.Showing == Mch_ANSWERS &&	// Teacher's screen is showing answers
       QstInd == Match->Status.QstInd)		// Receiving an answer to the current question being played
     {
      /***** Get indexes for this question from database *****/
      Mch_GetIndexes (Match->MchCod,Match->Status.QstInd,Indexes);

      /***** Get answer index *****/
      /*
      Indexes[4] = {0,3,1,2}
      +--------+--------+----------+---------+
      | Button | Option | Answer   | Correct |
      | letter | number | index    |         |
      | screen | screen | database |         |
      +--------+--------+----------+---------+
      |   a    |    0   |    0     |         |
      |   b    |    1   |    3     |         |
      |   c    |    2   |    1     |    Y    | <---- User press button #2 (index = 1, correct)
      |   d    |    3   |    2     |         |
      +--------+--------+----------+---------+
      UsrAnswer->NumOpt = 2
      UsrAnswer->AnsInd = 1
      */
      UsrAnswer->AnsInd = Indexes[UsrAnswer->NumOpt];

      /***** Get previous student's answer to this question
	     (<0 ==> no answer) *****/
      Mch_GetQstAnsFromDB (Match->MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,Match->Status.QstInd,
			   &PreviousUsrAnswer);

      /***** Store student's answer *****/
      if (UsrAnswer->NumOpt >= 0 &&
	  UsrAnswer->AnsInd >= 0 &&
	  UsrAnswer->AnsInd != PreviousUsrAnswer.AnsInd)
	{
	 /***** Update my answer to this question *****/
	 Mch_DB_UpdateMyAnswerToMatchQuestion (Match,UsrAnswer);

	 /***** Compute score and update my match result *****/
	 MchPrn_ComputeScoreAndUpdateMyMatchPrintInDB (Match->MchCod);
	}
     }
  }

/*****************************************************************************/
/*************** Get the questions of a match from database ******************/
/*****************************************************************************/

void Mch_GetMatchQuestionsFromDB (struct MchPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumQst;
   unsigned QstInd;
   struct Mch_UsrAnswer UsrAnswer;

   /***** Get questions and answers of a match result *****/
   Print->NumQsts.All = Mch_DB_GetMatchQuestions (&mysql_res,Print->MchCod);

   for (NumQst = 0, Print->NumQsts.NotBlank = 0;
	NumQst < Print->NumQsts.All;
	NumQst++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get question code (row[0]) */
      if ((Print->PrintedQuestions[NumQst].QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	 Err_WrongQuestionExit ();

      /* Get question index (row[1]) */
      QstInd = Str_ConvertStrToUnsigned (row[1]);

      /* Get indexes for this question (row[2]) */
      Str_Copy (Print->PrintedQuestions[NumQst].StrIndexes,row[2],
                sizeof (Print->PrintedQuestions[NumQst].StrIndexes) - 1);

      /* Get answers selected by user for this question */
      Mch_GetQstAnsFromDB (Print->MchCod,Print->UsrCod,QstInd,&UsrAnswer);
      if (UsrAnswer.AnsInd >= 0)	// UsrAnswer.AnsInd >= 0 ==> answer selected
	{
         snprintf (Print->PrintedQuestions[NumQst].StrAnswers,
                   sizeof (Print->PrintedQuestions[NumQst].StrAnswers),
                   "%d",UsrAnswer.AnsInd);
         Print->NumQsts.NotBlank++;
        }
      else				// UsrAnswer.AnsInd < 0 ==> no answer selected
	 Print->PrintedQuestions[NumQst].StrAnswers[0] = '\0';	// Empty answer
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

#define Mch_MAX_BAR_WIDTH 100

void Mch_DrawBarNumUsrs (unsigned NumRespondersAns,unsigned NumRespondersQst,
			 Qst_WrongOrCorrect_t WrongOrCorrect)
  {
   static const char *WroCor_Class[Qst_NUM_WRONG_CORRECT] =
     {
      [Qst_WRONG  ] = "MCH_RES_WRONG",
      [Qst_CORRECT] = "MCH_RES_CORRECT",
     };
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned i;
   unsigned BarWidth = 0;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"MCH_RESULT\"");

      /***** Draw bar with a with proportional to the number of clicks *****/
      if (NumRespondersAns && NumRespondersQst)
	 BarWidth = (unsigned) ((((double) NumRespondersAns * (double) Mch_MAX_BAR_WIDTH) /
				  (double) NumRespondersQst) + 0.5);

      /***** Bar proportional to number of users *****/
      HTM_TABLE_BeginWide ();
	 HTM_TR_Begin ("class=\"MCH_RES_TR\"");
	    for (i = 0;
		 i < 100;
		 i++)
	      {
	       HTM_TD_Begin ("class=\"%s\"",
			     (i < BarWidth) ? WroCor_Class[WrongOrCorrect] :
					      "MCH_RES_VOID");
	       HTM_TD_End ();
	      }
	 HTM_TR_End ();
      HTM_TABLE_End ();

      /***** Write the number of users *****/
      if (NumRespondersAns && NumRespondersQst)
        {
	 HTM_Unsigned (NumRespondersAns); HTM_NBSP ();
	 HTM_OpenParenthesis ();
	    HTM_Unsigned ((unsigned) ((((double) NumRespondersAns * 100.0) /
		                        (double) NumRespondersQst) + 0.5));
	    HTM_Percent (); HTM_NBSP ();
	    HTM_Txt (Txt_of_PART_OF_A_TOTAL); HTM_NBSP ();
	    HTM_Unsigned (NumRespondersQst);
	 HTM_CloseParenthesis ();
        }
      else
	 HTM_NBSP ();

   /***** End container *****/
   HTM_DIV_End ();
  }
