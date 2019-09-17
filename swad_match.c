// swad_match.c: matches in games using remote control

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
#include "swad_match.h"
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

#define Mch_ICON_CLOSE		"fas fa-times"
#define Mch_ICON_PLAY		"fas fa-play"
#define Mch_ICON_PAUSE		"fas fa-pause"
#define Mch_ICON_PREVIOUS	"fas fa-step-backward"
#define Mch_ICON_NEXT		"fas fa-step-forward"
#define Mch_ICON_RESULTS	"fas fa-chart-bar"

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Mch_NUM_SHOWING 4
typedef enum
  {
   Mch_NOTHING,	// Don't show anything
   Mch_STEM,	// Showing only the question stem
   Mch_ANSWERS,	// Showing the question stem and the answers
   Mch_RESULTS,	// Showing the results
  } Mch_Showing_t;
#define Mch_SHOWING_DEFAULT Mch_NOTHING

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
      bool ShowResults;
      Mch_Showing_t Showing;
      bool Playing;
      unsigned NumPlayers;
     } Status;
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

const char *Mch_ShowingStringsDB[Mch_NUM_SHOWING] =
  {
   "nothing",
   "stem",
   "answers",
   "results",
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

long Mch_CurrentMchCod = -1L;	// Used as parameter in contextual links

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mch_GetDataOfMatchByCod (struct Match *Match);

static void Mch_PutIconToPlayNewMatch (void);
static void Mch_ListOneOrMoreMatches (struct Game *Game,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res);
static void Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (struct Match *Match);
static void Mch_GetMatchDataFromRow (MYSQL_RES *mysql_res,
				     struct Match *Match);
static Mch_Showing_t Mch_GetShowingFromStr (const char *Str);

static void Mch_PutParamCurrentMchCod (void);
static void Mch_PutParamMatchCod (long MchCod);
static long Mch_GetParamMatchCod (void);

static void Mch_PutButtonNewMatch (long GamCod);

static void Mch_PutFormNewMatch (struct Game *Game);
static void Mch_ShowLstGrpsToCreateMatch (void);

static long Mch_CreateMatch (long GamCod,char Title[Gam_MAX_BYTES_TITLE + 1]);
static void Mch_CreateGrps (long MchCod);
static void Mch_UpdateMatchStatusInDB (struct Match *Match);

static void Mch_UpdateElapsedTimeInQuestion (struct Match *Match);
static void Mch_GetElapsedTimeInQuestion (struct Match *Match,
				          struct Time *Time);
static void Mch_GetElapsedTimeInMatch (struct Match *Match,
				       struct Time *Time);
static void Mch_GetElapsedTime (unsigned NumRows,MYSQL_RES *mysql_res,
				struct Time *Time);

static void Mch_SetMatchStatusToPrev (struct Match *Match);
static void Mch_SetMatchStatusToPrevQst (struct Match *Match);
static void Mch_SetMatchStatusToStart (struct Match *Match);

static void Mch_SetMatchStatusToNext (struct Match *Match);
static void Mch_SetMatchStatusToNextQst (struct Match *Match);
static void Mch_SetMatchStatusToEnd (struct Match *Match);

static void Mch_ShowMatchStatusForTch (struct Match *Match);
static void Mch_ShowMatchStatusForStd (struct Match *Match);
static bool Mch_CheckIfIPlayThisMatchBasedOnGrps (long MchCod);
static void Mch_ShowLeftColumnTch (struct Match *Match);
static void Mch_ShowLeftColumnStd (struct Match *Match);
static void Mch_ShowNumQstInMatch (struct Match *Match);
static void Mch_PutMatchControlButtons (struct Match *Match);
static void Mch_PutCheckboxResult (struct Match *Match);
static void Mch_ShowNumPlayers (struct Match *Match);
static void Mch_ShowMatchTitle (struct Match *Match);
static void Mch_ShowQuestionAndAnswersTch (struct Match *Match);
static void Mch_ShowQuestionAndAnswersStd (struct Match *Match);

static void Mch_PutParamAnswer (unsigned AnsInd);
static unsigned Mch_GetParamAnswer (void);

static void Mch_PutBigButton (Act_Action_t NextAction,long MchCod,
			      const char *Icon,const char *Txt);
static void Mch_PutBigButtonOff (const char *Icon);
static void Mch_PutBigButtonClose (void);

static void Mch_ShowWaitImage (const char *Txt);

static void Mch_RemoveOldPlayers (void);
static void Mch_UpdateMatchAsBeingPlayed (long MchCod);
static void Mch_SetMatchAsNotBeingPlayed (long MchCod);
static bool Mch_GetIfMatchIsBeingPlayed (long MchCod);
static void Mch_RegisterMeAsPlayerInMatch (long MchCod);
static void Mch_GetNumPlayers (struct Match *Match);

static int Mch_GetQstAnsFromDB (long MchCod,unsigned QstInd);

static unsigned Mch_GetNumUsrsWhoHaveChosenAns (long MchCod,unsigned QstInd,unsigned AnsInd);
static unsigned Mch_GetNumUsrsWhoHaveAnswerMch (long MchCod);
static void Mch_DrawBarNumUsrs (unsigned NumAnswerersAns,unsigned NumAnswerersQst,bool Correct);

/*****************************************************************************/
/************************* List the matches of a game ************************/
/*****************************************************************************/

void Mch_ListMatches (struct Game *Game,bool PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games_matches;
   extern const char *Txt_Matches;
   extern long Gam_CurrentGamCod;	// Used as parameter in contextual links;
   char *SubQuery;
   MYSQL_RES *mysql_res;
   unsigned NumMatches;

   /***** Get data of matches from database *****/
   /* Fill subquery for game */
   if (Gbl.Crs.Grps.WhichGrps == Grp_ONLY_MY_GROUPS)
     {
      if (asprintf (&SubQuery," AND"
			      "(MchCod NOT IN"
			      " (SELECT MchCod FROM mch_groups)"
			      " OR"
			      " MchCod IN"
			      " (SELECT mch_groups.MchCod"
			      " FROM mch_groups,crs_grp_usr"
			      " WHERE crs_grp_usr.UsrCod=%ld"
			      " AND mch_groups.GrpCod=crs_grp_usr.GrpCod))",
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
					          "ShowResults,"			// row[8]
						  "Showing"				// row[9]
					   " FROM mch_matches"
					   " WHERE GamCod=%ld%s"
					   " ORDER BY MchCod",
					   Game->GamCod,
					   SubQuery);

   /* Free allocated memory for subquery */
   free ((void *) SubQuery);

   /***** Start box *****/
   Gam_CurrentGamCod = Game->GamCod;
   Box_StartBox (NULL,Txt_Matches,Mch_PutIconToPlayNewMatch,
                 Hlp_ASSESSMENT_Games_matches,Box_NOT_CLOSABLE);

   if (NumMatches)
      /***** Show the table with the matches *****/
      Mch_ListOneOrMoreMatches (Game,NumMatches,mysql_res);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Put button to play a new match in this game *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 if (PutFormNewMatch)
	    Mch_PutFormNewMatch (Game);			// Form to fill in data and start playing a new match
	 else
	    Mch_PutButtonNewMatch (Game->GamCod);	// Button to create a new match
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

static void Mch_GetDataOfMatchByCod (struct Match *Match)
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
					       "ShowResults,"				// row[8]
					       "Showing"				// row[9]
					" FROM mch_matches"
					" WHERE MchCod=%ld"
					" AND GamCod IN"		// Extra check
					" (SELECT GamCod FROM gam_games"
					" WHERE CrsCod='%ld')",
					Match->MchCod,
					Gbl.Hierarchy.Crs.CrsCod);
   if (NumRows) // Match found...
      /***** Get match data from row *****/
      Mch_GetMatchDataFromRow (mysql_res,Match);
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
      Match->Status.ShowResults      = true;
      Match->Status.Showing          = Mch_STEM;
      Match->Status.Playing          = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Put icon to add a new questions to game *******************/
/*****************************************************************************/

static void Mch_PutIconToPlayNewMatch (void)
  {
   extern const char *Txt_New_match;

   /***** Put form to create a new question *****/
   Ico_PutContextualIconToAdd (ActReqNewMchTch,Mch_NEW_MATCH_SECTION_ID,Gam_PutParams,
			       Txt_New_match);
  }

/*****************************************************************************/
/*********************** List game matches for edition ***********************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatches (struct Game *Game,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Match;
   extern const char *Txt_Players;
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
                      "<th class=\"RIGHT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_ROLES_SINGUL_Abc[Rol_TCH][Usr_SEX_UNKNOWN],
	    Txt_START_END_TIME[Gam_ORDER_BY_START_DATE],
	    Txt_START_END_TIME[Gam_ORDER_BY_END_DATE],
            Txt_Match,
	    Txt_Players,
            Txt_Status);

   /***** Write rows *****/
   for (NumMatch = 0, UniqueId = 1;
	NumMatch < NumMatches;
	NumMatch++, UniqueId++)
     {
      Gbl.RowEvenOdd = NumMatch % 2;

      /***** Get match data from row *****/
      Mch_GetMatchDataFromRow (mysql_res,&Match);

      /***** Icons *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"BT%u\">",Gbl.RowEvenOdd);

      /* Put icon to remove the match */
      Frm_StartForm (ActReqRemMchTch);
      Mch_PutParamMatchCod (Match.MchCod);
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
	       Match.Status.QstInd >= Mch_AFTER_LAST_QUESTION ? "DATE_RED" :
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
	       Match.Status.QstInd >= Mch_AFTER_LAST_QUESTION ? "DATE_RED" :
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
	 Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (&Match);

      fprintf (Gbl.F.Out,"</td>");

      /***** Number of players who have answered any question in the match ******/
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">"
	                 "%u"
	                 "</td>",
	       Gbl.RowEvenOdd,
	       Mch_GetNumUsrsWhoHaveAnswerMch (Match.MchCod));

      /***** Match status ******/
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_TOP COLOR%u\">",Gbl.RowEvenOdd);

      if (Match.Status.QstInd < Mch_AFTER_LAST_QUESTION)	// Unfinished match
	 /* Current question index / total of questions */
	 fprintf (Gbl.F.Out,"<div class=\"DAT\">%u/%u</div>",
		  Match.Status.QstInd,Game->NumQsts);

      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:
	    /* Icon to play as student */
	    Mch_CurrentMchCod = Match.MchCod;
	    Lay_PutContextualLinkOnlyIcon (ActPlyMchStd,NULL,
					   Mch_PutParamCurrentMchCod,
					   Match.Status.QstInd < Mch_AFTER_LAST_QUESTION ? "play.svg" :
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
	    Mch_CurrentMchCod = Match.MchCod;
	    Lay_PutContextualLinkOnlyIcon (ActResMchTch,NULL,
					   Mch_PutParamCurrentMchCod,
					   Match.Status.QstInd < Mch_AFTER_LAST_QUESTION ? "play.svg" :
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
/************* Get and write the names of the groups of a match **************/
/*****************************************************************************/

static void Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (struct Match *Match)
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
			     " FROM mch_groups,crs_grp,crs_grp_types"
			     " WHERE mch_groups.MchCod=%ld"
			     " AND mch_groups.GrpCod=crs_grp.GrpCod"
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
/******************** Get game data from a database row **********************/
/*****************************************************************************/

static void Mch_GetMatchDataFromRow (MYSQL_RES *mysql_res,
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
   row[8]	ShowResults
   row[9]	Showing
   */
   /* Current question index (row[6]) */
   Match->Status.QstInd = Gam_GetQstIndFromStr (row[6]);

   /* Current question code (row[7]) */
   Match->Status.QstCod = Str_ConvertStrCodToLongCod (row[7]);

   /* Get whether to show results or not (row(8)) */
   Match->Status.ShowResults = (row[8][0] == 'Y');

   /* Get what to show (stem, answers, results) (row(9)) */
   Match->Status.Showing = Mch_GetShowingFromStr (row[9]);

   /***** Get whether the match is being played or not *****/
   if (Match->Status.QstInd >= Mch_AFTER_LAST_QUESTION)	// Finished
      Match->Status.Playing = false;
   else							// Unfinished
      Match->Status.Playing = Mch_GetIfMatchIsBeingPlayed (Match->MchCod);
  }

/*****************************************************************************/
/****************** Get parameter with what is being shown *******************/
/*****************************************************************************/

static Mch_Showing_t Mch_GetShowingFromStr (const char *Str)
  {
   Mch_Showing_t Showing;

   for (Showing = (Mch_Showing_t) 0;
	Showing <= (Mch_Showing_t) (Mch_NUM_SHOWING - 1);
	Showing++)
      if (!strcmp (Str,Mch_ShowingStringsDB[Showing]))
         return Showing;

   return (Mch_Showing_t) Mch_SHOWING_DEFAULT;
  }

/*****************************************************************************/
/************** Request the removal of a match (game instance) ***************/
/*****************************************************************************/

void Mch_RequestRemoveMatchTch (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_match_X;
   extern const char *Txt_Remove_match;
   struct Match Match;

   /***** Get parameters *****/
   /* Get match code */
   if ((Match.MchCod = Mch_GetParamMatchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");

   /***** Get data of the match from database *****/
   Mch_GetDataOfMatchByCod (&Match);

   /***** Show question and button to remove question *****/
   Mch_CurrentMchCod = Match.MchCod;
   Ale_ShowAlertAndButton (ActRemMchTch,NULL,NULL,Mch_PutParamCurrentMchCod,
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

void Mch_RemoveMatchTch (void)
  {
   extern const char *Txt_Match_X_removed;
   struct Match Match;

   /***** Get parameters *****/
   /* Get match code */
   if ((Match.MchCod = Mch_GetParamMatchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");

   /***** Get data of the match from database *****/
   Mch_GetDataOfMatchByCod (&Match);

   /***** Remove the match from all the tables *****/
   /* Remove match players */
   DB_QueryDELETE ("can not remove match players",
		   "DELETE FROM mch_players"
		   " USING mch_players,mch_matches,gam_games"
		   " WHERE mch_players.MchCod=%ld"
		   " AND mch_players.MchCod=mch_matches.MchCod"
		   " AND mch_matches.GamCod=gam_games.GamCod"
		   " AND gam_games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove match from list of matches being played */
   DB_QueryDELETE ("can not remove match from matches being played",
		   "DELETE FROM mch_playing"
		   " USING mch_playing,mch_matches,gam_games"
		   " WHERE mch_playing.MchCod=%ld"
		   " AND mch_playing.MchCod=mch_matches.MchCod"
		   " AND mch_matches.GamCod=gam_games.GamCod"
		   " AND gam_games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove students' answers to match */
   DB_QueryDELETE ("can not remove students' answers associated to a match",
		   "DELETE FROM mch_answers"
		   " USING mch_answers,mch_matches,gam_games"
		   " WHERE mch_answers.MchCod=%ld"
		   " AND mch_answers.MchCod=mch_matches.MchCod"
		   " AND mch_matches.GamCod=gam_games.GamCod"
		   " AND gam_games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove groups associated to the match */
   DB_QueryDELETE ("can not remove the groups associated to a match",
		   "DELETE FROM mch_groups"
		   " USING mch_groups,mch_matches,gam_games"
		   " WHERE mch_groups.MchCod=%ld"
		   " AND mch_groups.MchCod=mch_matches.MchCod"
		   " AND mch_matches.GamCod=gam_games.GamCod"
		   " AND gam_games.CrsCod=%ld",	// Extra check
		   Match.MchCod,Gbl.Hierarchy.Crs.CrsCod);

   /* Remove the match itself */
   DB_QueryDELETE ("can not remove a match",
		   "DELETE FROM mch_matches"
		   " USING mch_matches,gam_games"
		   " WHERE mch_matches.MchCod=%ld"
		   " AND mch_matches.GamCod=gam_games.GamCod"
		   " AND gam_games.CrsCod=%ld",	// Extra check
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
/***************** Put parameter with current match code *********************/
/*****************************************************************************/

static void Mch_PutParamCurrentMchCod (void)
  {
   if (Mch_CurrentMchCod > 0)
      Mch_PutParamMatchCod (Mch_CurrentMchCod);
  }

/*****************************************************************************/
/******************** Write parameter with code of match **********************/
/*****************************************************************************/

static void Mch_PutParamMatchCod (long MchCod)
  {
   Par_PutHiddenParamLong ("MchCod",MchCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of match **********************/
/*****************************************************************************/

static long Mch_GetParamMatchCod (void)
  {
   /***** Get code of match *****/
   return Par_GetParToLong ("MchCod");
  }

/*****************************************************************************/
/********************* Put button to create a new match **********************/
/*****************************************************************************/

static void Mch_PutButtonNewMatch (long GamCod)
  {
   extern const char *Txt_New_match;

   Frm_StartFormAnchor (ActReqNewMchTch,Mch_NEW_MATCH_SECTION_ID);
   Gam_PutParamGameCod (GamCod);
   Btn_PutConfirmButton (Txt_New_match);
   Frm_EndForm ();
  }

/*****************************************************************************/
/****** Put a big button to play match (start a new match) as a teacher ******/
/*****************************************************************************/

static void Mch_PutFormNewMatch (struct Game *Game)
  {
   extern const char *Hlp_ASSESSMENT_Games_new_match;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_match;
   extern const char *Txt_Title;
   extern const char *Txt_Play;

   /***** Start section for a new match *****/
   Lay_StartSection (Mch_NEW_MATCH_SECTION_ID);

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
   Mch_ShowLstGrpsToCreateMatch ();

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
/***************** Show list of groups to create a new match *****************/
/*****************************************************************************/

static void Mch_ShowLstGrpsToCreateMatch (void)
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
/********************* Create a new match (by a teacher) *********************/
/*****************************************************************************/

void Mch_CreateNewMatchTch (void)
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
   Gbl.Games.MchCodBeingPlayed = Mch_CreateMatch (GamCod,Title);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/******* Show button to actually start / resume a match (by a teacher) *******/
/*****************************************************************************/

void Mch_RequestStartResumeMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Create a new match in a game *************************/
/*****************************************************************************/

static long Mch_CreateMatch (long GamCod,char Title[Gam_MAX_BYTES_TITLE + 1])
  {
   long MchCod;

   /***** Insert this new match into database *****/
   MchCod = DB_QueryINSERTandReturnCode ("can not create match",
				         "INSERT mch_matches "
				         "(GamCod,UsrCod,StartTime,EndTime,Title,"
				         "QstInd,QstCod,ShowResults,Showing)"
				         " VALUES "
				         "(%ld,"	// GamCod
				         "%ld,"		// UsrCod
				         "NOW(),"	// StartTime
				         "NOW(),"	// EndTime
				         "'%s',"	// Title
				         "0,"		// QstInd: Match has not started, so not the first question yet
				         "-1,"		// QstCod: Non-existent question
				         "'N',"		// ShowResults: Don't show results initially
				         "'%s')",	// Showing: What is being shown
				         GamCod,
				         Gbl.Usrs.Me.UsrDat.UsrCod,	// Game creator
				         Title,
					 Mch_ShowingStringsDB[Mch_SHOWING_DEFAULT]);

   /***** Create groups associated to the match *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Mch_CreateGrps (MchCod);

   return MchCod;
  }

/*****************************************************************************/
/******************* Create groups associated to a match *********************/
/*****************************************************************************/

static void Mch_CreateGrps (long MchCod)
  {
   unsigned NumGrpSel;

   /***** Create groups associated to the match *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to a match",
		      "INSERT INTO mch_groups"
		      " (MchCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      MchCod,Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/***************** Insert/update a game match being played *******************/
/*****************************************************************************/

#define Mch_MAX_BYTES_SUBQUERY 128

static void Mch_UpdateMatchStatusInDB (struct Match *Match)
  {
   char MchSubQuery[Mch_MAX_BYTES_SUBQUERY];

   /***** Update end time only if match is currently being played *****/
   if (Match->Status.Playing)
      Str_Copy (MchSubQuery,"mch_matches.EndTime=NOW(),",
		Mch_MAX_BYTES_SUBQUERY);
   else
      MchSubQuery[0] = '\0';

   /***** Update match status in database *****/
   DB_QueryUPDATE ("can not update match being played",
		   "UPDATE mch_matches,gam_games"
		   " SET %s"
			"mch_matches.QstInd=%u,"
			"mch_matches.QstCod=%ld,"
			"mch_matches.ShowResults='%c',"
			"mch_matches.Showing='%s'"
		   " WHERE mch_matches.MchCod=%ld"
		   " AND mch_matches.GamCod=gam_games.GamCod"
		   " AND gam_games.CrsCod=%ld",	// Extra check
		   MchSubQuery,
		   Match->Status.QstInd,Match->Status.QstCod,
		   Match->Status.ShowResults ? 'Y' : 'N',
		   Mch_ShowingStringsDB[Match->Status.Showing],
		   Match->MchCod,Gbl.Hierarchy.Crs.CrsCod);

   if (Match->Status.Playing)
      /* Update match as being played */
      Mch_UpdateMatchAsBeingPlayed (Match->MchCod);
   else
      /* Update match as not being played */
      Mch_SetMatchAsNotBeingPlayed (Match->MchCod);
  }

/*****************************************************************************/
/********** Update elapsed time in current question (by a teacher) ***********/
/*****************************************************************************/

static void Mch_UpdateElapsedTimeInQuestion (struct Match *Match)
  {
   /***** Update elapsed time in current question in database *****/
   if (Match->Status.Playing &&
       Match->Status.QstInd > 0 &&
       Match->Status.QstInd < Mch_AFTER_LAST_QUESTION)
      DB_QueryINSERT ("can not update elapsed time in question",
		      "INSERT INTO mch_times (MchCod,QstInd,ElapsedTime)"
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

static void Mch_GetElapsedTimeInQuestion (struct Match *Match,
					  struct Time *Time)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRows;

   /***** Query database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get elapsed time",
				        "SELECT ElapsedTime"
				        " FROM mch_times"
				        " WHERE MchCod=%ld AND QstInd=%u",
				        Match->MchCod,Match->Status.QstInd);

   /***** Get elapsed time from query result *****/
   Mch_GetElapsedTime (NumRows,mysql_res,Time);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get elapsed time in a match *************************/
/*****************************************************************************/

static void Mch_GetElapsedTimeInMatch (struct Match *Match,
				       struct Time *Time)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRows;

   /***** Query database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get elapsed time",
				        "SELECT SEC_TO_TIME(SUM(TIME_TO_SEC(ElapsedTime)))"
				        " FROM mch_times WHERE MchCod=%ld",
				        Match->MchCod);

   /***** Get elapsed time from query result *****/
   Mch_GetElapsedTime (NumRows,mysql_res,Time);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*********************** Get elapsed time in a match *************************/
/*****************************************************************************/

static void Mch_GetElapsedTime (unsigned NumRows,MYSQL_RES *mysql_res,
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

void Mch_PauseMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Match.Status.Playing = false;	// Pause match

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/** Show current match status (current question, answers...) (by a teacher) **/
/*****************************************************************************/

void Mch_ResumeMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** If not yet finished, update status *****/
   if (Match.Status.QstInd < Mch_AFTER_LAST_QUESTION)	// Unfinished
     {
      if (Match.Status.QstInd == 0)			// Match has been created, but it has not started
	 Mch_SetMatchStatusToNext (&Match);
      Match.Status.Playing = true;			// Start/resume match
     }

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********* Toggle the display of results in a match (by a teacher) ***********/
/*****************************************************************************/

void Mch_ToggleDisplayResultsMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Match.Status.ShowResults = !Match.Status.ShowResults;	// Toggle display
   if (!Match.Status.ShowResults &&
        Match.Status.Showing == Mch_RESULTS)
     Match.Status.Showing = Mch_ANSWERS;	// Hide results

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Show previous question in a match (by a teacher) **************/
/*****************************************************************************/

void Mch_BackMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Mch_SetMatchStatusToPrev (&Match);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*************** Show next question in a match (by a teacher) ****************/
/*****************************************************************************/

void Mch_ForwardMatchTch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Update status *****/
   Mch_SetMatchStatusToNext (&Match);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForTch (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************** Set match status to previous (backward) status ***************/
/*****************************************************************************/

static void Mch_SetMatchStatusToPrev (struct Match *Match)
  {
   /***** What to show *****/
   if (Match->Status.QstInd == 0)				// Start
      Mch_SetMatchStatusToStart (Match);
   else if (Match->Status.QstInd >= Mch_AFTER_LAST_QUESTION)	// End
      Mch_SetMatchStatusToPrevQst (Match);
   else								// Between start and end
      switch (Match->Status.Showing)
	{
	 case Mch_NOTHING:
	 case Mch_STEM:
	    Mch_SetMatchStatusToPrevQst (Match);
	    break;
	 case Mch_ANSWERS:
	    Match->Status.Showing = Mch_STEM;
	    break;
	 case Mch_RESULTS:
	    Match->Status.Showing = Mch_ANSWERS;
	    break;
	}
  }

static void Mch_SetMatchStatusToPrevQst (struct Match *Match)
  {
   /***** Get index of the previous question *****/
   Match->Status.QstInd = Gam_GetPrevQuestionIndexInGame (Match->GamCod,
							  Match->Status.QstInd);
   if (Match->Status.QstInd == 0)		// Start of questions has been reached
      Mch_SetMatchStatusToStart (Match);
   else
     {
      Match->Status.QstCod = Gam_GetQstCodFromQstInd (Match->GamCod,
						      Match->Status.QstInd);
      Match->Status.Showing = Match->Status.ShowResults ? Mch_RESULTS :
							  Mch_ANSWERS;
     }
  }

static void Mch_SetMatchStatusToStart (struct Match *Match)
  {
   Match->Status.QstInd  = 0;				// Before first question
   Match->Status.QstCod  = -1L;
   Match->Status.Playing = false;
   Match->Status.Showing = Mch_NOTHING;
  }

/*****************************************************************************/
/**************** Set match status to next (forward) status ******************/
/*****************************************************************************/

static void Mch_SetMatchStatusToNext (struct Match *Match)
  {
   /***** What to show *****/
   if (Match->Status.QstInd == 0)				// Start
      Mch_SetMatchStatusToNextQst (Match);
   else if (Match->Status.QstInd >= Mch_AFTER_LAST_QUESTION)	// End
      Mch_SetMatchStatusToEnd (Match);
   else								// Between start and end
      switch (Match->Status.Showing)
	{
	 case Mch_NOTHING:
	    Match->Status.Showing = Mch_STEM;
	    break;
	 case Mch_STEM:
	    Match->Status.Showing = Mch_ANSWERS;
	    break;
	 case Mch_ANSWERS:
	    if (Match->Status.ShowResults)
	       Match->Status.Showing = Mch_RESULTS;
	    else
	       Mch_SetMatchStatusToNextQst (Match);
	    break;
	 case Mch_RESULTS:
	    Mch_SetMatchStatusToNextQst (Match);
	    break;
	}
  }

static void Mch_SetMatchStatusToNextQst (struct Match *Match)
  {
   /***** Get index of the next question *****/
   Match->Status.QstInd = Gam_GetNextQuestionIndexInGame (Match->GamCod,
							  Match->Status.QstInd);

   /***** Get question code *****/
   if (Match->Status.QstInd >= Mch_AFTER_LAST_QUESTION)	// Finished
      Mch_SetMatchStatusToEnd (Match);
   else							// Unfinished
     {
      Match->Status.QstCod = Gam_GetQstCodFromQstInd (Match->GamCod,
						      Match->Status.QstInd);
      Match->Status.Showing = Mch_STEM;
     }
  }

static void Mch_SetMatchStatusToEnd (struct Match *Match)
  {
   Match->Status.QstInd  = Mch_AFTER_LAST_QUESTION;	// After last question
   Match->Status.QstCod  = -1L;
   Match->Status.Playing = false;
   Match->Status.Showing = Mch_NOTHING;
  }

/*****************************************************************************/
/******* Show current match status (number, question, answers, button) *******/
/*****************************************************************************/

static void Mch_ShowMatchStatusForTch (struct Match *Match)
  {
   /***** Get current number of players *****/
   Mch_GetNumPlayers (Match);

   /***** Left column *****/
   Mch_ShowLeftColumnTch (Match);

   /***** Right column *****/
   /* Start right container */
   fprintf (Gbl.F.Out,"<div class=\"MATCH_RIGHT\">");

   /* Top row: match title */
   Mch_ShowMatchTitle (Match);

   /* Bottom row: current question and possible answers */
   Mch_ShowQuestionAndAnswersTch (Match);

   /* End right container */
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************ Show current question being played for a student ***************/
/*****************************************************************************/

static void Mch_ShowMatchStatusForStd (struct Match *Match)
  {
   extern const char *Txt_Please_wait_;
   bool IBelongToGroups;

   /***** Do I belong to valid groups to play this match? *****/
   IBelongToGroups = Gbl.Usrs.Me.IBelongToCurrentCrs &&
		     Mch_CheckIfIPlayThisMatchBasedOnGrps (Match->MchCod);
   if (!IBelongToGroups)
      Lay_ShowErrorAndExit ("You can not play this match!");

   /***** Get current number of players *****/
   Mch_GetNumPlayers (Match);

   /***** Left column *****/
   Mch_ShowLeftColumnStd (Match);

   /***** Right column *****/
   /* Start right container */
   fprintf (Gbl.F.Out,"<div class=\"MATCH_RIGHT\">");

   /***** Top row *****/
   Mch_ShowMatchTitle (Match);

   /***** Bottom row *****/
   if (Match->Status.QstInd < Mch_AFTER_LAST_QUESTION)	// Unfinished
     {
      fprintf (Gbl.F.Out,"<div class=\"MATCH_BOTTOM\">");

      /***** Update players ******/
      Mch_RegisterMeAsPlayerInMatch (Match->MchCod);

      if (Match->Status.Playing)
	 /* Show current question and possible answers */
	 Mch_ShowQuestionAndAnswersStd (Match);
      else	// Not being played
	 Mch_ShowWaitImage (Txt_Please_wait_);

      fprintf (Gbl.F.Out,"</div>");
     }

   /* End right container */
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a match **************/
/*****************************************************************************/

static bool Mch_CheckIfIPlayThisMatchBasedOnGrps (long MchCod)
  {
   /***** Get if I can play a match from database *****/
   return (DB_QueryCOUNT ("can not check if I can play a match",
			  "SELECT COUNT(*) FROM mch_matches"
			  " WHERE MchCod=%ld"
			  " AND (MchCod NOT IN (SELECT MchCod FROM mch_groups) OR"
			  " MchCod IN (SELECT mch_groups.MchCod FROM mch_groups,crs_grp_usr"
			  " WHERE crs_grp_usr.UsrCod=%ld"
			  " AND mch_groups.GrpCod=crs_grp_usr.GrpCod))",
			  MchCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
  }

/*****************************************************************************/
/******** Show left botton column when playing a match (as a teacher) ********/
/*****************************************************************************/

static void Mch_ShowLeftColumnTch (struct Match *Match)
  {
   extern const char *Txt_MATCH_respond;
   struct Time Time;
   unsigned NumAnswerersQst;

   /***** Start left container *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_LEFT\">");

   /***** Top *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TOP\">");

   /* Write elapsed time in match */
   Mch_GetElapsedTimeInMatch (Match,&Time);
   Dat_WriteHoursMinutesSeconds (&Time);

   fprintf (Gbl.F.Out,"</div>");

   /***** Write number of question *****/
   Mch_ShowNumQstInMatch (Match);

   /***** Write elapsed time in question *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TIME_QST\">");
   if (Match->Status.QstInd > 0 &&
       Match->Status.QstInd < Mch_AFTER_LAST_QUESTION)
     {
      Mch_GetElapsedTimeInQuestion (Match,&Time);
      Dat_WriteHoursMinutesSeconds (&Time);
     }
   else
      fprintf (Gbl.F.Out,"-");
   fprintf (Gbl.F.Out,"</div>");

   /***** Buttons *****/
   Mch_PutMatchControlButtons (Match);

   /***** Write button to request viewing results *****/
   Mch_PutCheckboxResult (Match);

   /***** Number of players *****/
   Mch_ShowNumPlayers (Match);

   /***** Number of users who have answered this question *****/
   if (Match->Status.Playing)
     {
      NumAnswerersQst = Mch_GetNumUsrsWhoHaveAnswerQst (Match->MchCod,
			                                Match->Status.QstInd);
      fprintf (Gbl.F.Out,"<div class=\"MATCH_NUM_ANSWERERS\">"
                         "%s<br />"
                         "<strong>%u/%u</strong>"
	                 "</div>",
	       Txt_MATCH_respond,
	       NumAnswerersQst,Match->Status.NumPlayers);
     }

   /***** End left container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/******** Show left botton column when playing a match (as a student) ********/
/*****************************************************************************/

static void Mch_ShowLeftColumnStd (struct Match *Match)
  {
   /***** Start left container *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_LEFT\">");

   /***** Top *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TOP\"></div>");

   /***** Write number of question *****/
   Mch_ShowNumQstInMatch (Match);

   /***** End left container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************* Show number of question in game ***********************/
/*****************************************************************************/

static void Mch_ShowNumQstInMatch (struct Match *Match)
  {
   extern const char *Txt_MATCH_Start;
   extern const char *Txt_MATCH_End;
   unsigned NumQsts = Gam_GetNumQstsGame (Match->GamCod);

   fprintf (Gbl.F.Out,"<div class=\"MATCH_NUM_QST\">");
   if (Match->Status.QstInd == 0)				// Not started
      fprintf (Gbl.F.Out,"%s",Txt_MATCH_Start);
   else if (Match->Status.QstInd >= Mch_AFTER_LAST_QUESTION)	// Finished
      fprintf (Gbl.F.Out,"%s",Txt_MATCH_End);
   else
      fprintf (Gbl.F.Out,"%u/%u",Match->Status.QstInd,NumQsts);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Put buttons to control a match ***********************/
/*****************************************************************************/

static void Mch_PutMatchControlButtons (struct Match *Match)
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
      Mch_PutBigButtonClose ();
   else
      /* Put button to go back */
      Mch_PutBigButton (ActBckMchTch,Match->MchCod,
			Mch_ICON_PREVIOUS,Txt_Go_back);
   fprintf (Gbl.F.Out,"</div>");

   /***** Center button *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_CENTER_CONTAINER\">");
   if (Match->Status.Playing)					// Being played
      /* Put button to pause match */
      Mch_PutBigButton (ActPauMchTch,
			Match->MchCod,
			Mch_ICON_PAUSE,Txt_Pause);
   else								// Paused
     {
      if (Match->Status.QstInd < Mch_AFTER_LAST_QUESTION)	// Not finished
	 /* Put button to play match */
	 Mch_PutBigButton (ActPlyMchTch,
			   Match->MchCod,
			   Mch_ICON_PLAY,Match->Status.QstInd == 0 ? Txt_Start :
								     Txt_Resume);
      else							// Finished
	 /* Put disabled button to play match */
	 Mch_PutBigButtonOff (Mch_ICON_PLAY);
     }
   fprintf (Gbl.F.Out,"</div>");

   /***** Right button *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_RIGHT_CONTAINER\">");
   if (Match->Status.QstInd >= Mch_AFTER_LAST_QUESTION)	// Finished
      /* Put button to close browser tab */
      Mch_PutBigButtonClose ();
   else
      /* Put button to show answers */
      Mch_PutBigButton (ActFwdMchTch,Match->MchCod,
			Mch_ICON_NEXT,Txt_Go_forward);
   fprintf (Gbl.F.Out,"</div>");

   /***** End buttons container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/***************** Put checkbox to select if show results ********************/
/*****************************************************************************/

static void Mch_PutCheckboxResult (struct Match *Match)
  {
   extern const char *Txt_View_results;

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_SHOW_RESULTS\">");

   /***** Start form *****/
   Frm_StartForm (ActChgDisResMchTch);
   Mch_PutParamMatchCod (Match->MchCod);	// Current match being played

   /***** Put icon with link *****/
   /* Submitting onmousedown instead of default onclick
      is necessary in order to be fast
      and not lose clicks due to refresh */
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

   /***** End form *****/
   Frm_EndForm ();

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************** Show number of players ***************************/
/*****************************************************************************/

static void Mch_ShowNumPlayers (struct Match *Match)
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

static void Mch_ShowMatchTitle (struct Match *Match)
  {
   /***** Match title *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_TOP\">%s</div>",Match->Title);
  }

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a teacher) *****/
/*****************************************************************************/

static void Mch_ShowQuestionAndAnswersTch (struct Match *Match)
  {
   extern const char *Txt_MATCH_Paused;
   extern const char *Txt_View_results;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Trivial check: question index should be correct *****/
   if (Match->Status.QstInd == 0 ||
       Match->Status.QstInd >= Mch_AFTER_LAST_QUESTION)
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

   fprintf (Gbl.F.Out,"<div class=\"MATCH_BOTTOM\">");	// Bottom

   /* Write stem (row[1]) */
   Tst_WriteQstStem (row[1],"MATCH_TCH_STEM");

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
      case Mch_NOTHING:
      case Mch_STEM:
	 /* Don't write anything */
	 break;
      case Mch_ANSWERS:
	 if (Match->Status.Playing)			// Being played
	    /* Write answers */
	    Tst_WriteAnswersMatchResult (Match->MchCod,
					 Match->Status.QstInd,
					 Match->Status.QstCod,
					 "MATCH_TCH_ANS",
					 false);	// Don't show result
	 else						// Not being played
	    Mch_ShowWaitImage (Txt_MATCH_Paused);
	 break;
      case Mch_RESULTS:
	 /* Write answers with results */
	 Tst_WriteAnswersMatchResult (Match->MchCod,
				      Match->Status.QstInd,
				      Match->Status.QstCod,
				      "MATCH_TCH_ANS",
				      true);		// Show result
	 break;
     }

   fprintf (Gbl.F.Out,"</div>");			// Bottom
  }

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a student) *****/
/*****************************************************************************/

static void Mch_ShowQuestionAndAnswersStd (struct Match *Match)
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
   if (Match->Status.Showing == Mch_ANSWERS)
     {
      if (Tst_CheckIfQuestionIsValidForGame (Match->Status.QstCod))
	{
	 /***** Get student's answer to this question
		(<0 ==> no answer) *****/
	 StdAnsInd = Mch_GetQstAnsFromDB (Match->MchCod,
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
	    Mch_PutParamMatchCod (Match->MchCod);	// Current match being played
	    Gam_PutParamQstInd (Match->Status.QstInd);	// Current question index shown
	    Mch_PutParamAnswer (Index);			// Index for this option
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
/******************* Write parameter with student's answer *******************/
/*****************************************************************************/

static void Mch_PutParamAnswer (unsigned AnsInd)
  {
   Par_PutHiddenParamUnsigned ("Ans",AnsInd);
  }

/*****************************************************************************/
/******************* Get parameter with student's answer *********************/
/*****************************************************************************/

static unsigned Mch_GetParamAnswer (void)
  {
   long LongNum;

   LongNum = Par_GetParToLong ("Ans");
   if (LongNum < 0)
      Lay_ShowErrorAndExit ("Wrong answer index.");

   return (unsigned) LongNum;
  }

/*****************************************************************************/
/*********************** Put a big button to do action ***********************/
/*****************************************************************************/

static void Mch_PutBigButton (Act_Action_t NextAction,long MchCod,
			      const char *Icon,const char *Txt)
  {
   /***** Start form *****/
   Frm_StartForm (NextAction);
   Mch_PutParamMatchCod (MchCod);

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

static void Mch_PutBigButtonOff (const char *Icon)
  {
   /***** Put inactive icon *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_BUTTON_CONTAINER\">"
                      "<div class=\"MATCH_BUTTON_OFF\">"
	              "<i class=\"%s\"></i>"
	              "</div>"
	              "</div>",
	    Icon);
  }

static void Mch_PutBigButtonClose (void)
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
	    Txt_Close,Mch_ICON_CLOSE);
  }

/*****************************************************************************/
/****************************** Show wait image ******************************/
/*****************************************************************************/

static void Mch_ShowWaitImage (const char *Txt)
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

static void Mch_RemoveOldPlayers (void)
  {
   /***** Delete matches not being played *****/
   DB_QueryDELETE ("can not update matches as not being played",
		   "DELETE FROM mch_playing"
		   " WHERE TS<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Cfg_SECONDS_TO_REFRESH_GAME*3);

   /***** Delete players who have left matches *****/
   DB_QueryDELETE ("can not update match players",
		   "DELETE FROM mch_players"
		   " WHERE TS<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Cfg_SECONDS_TO_REFRESH_GAME*3);
  }

static void Mch_UpdateMatchAsBeingPlayed (long MchCod)
  {
   /***** Insert match as being played *****/
   DB_QueryREPLACE ("can not set match as being played",
		    "REPLACE mch_playing (MchCod) VALUE (%ld)",
		    MchCod);
  }

static void Mch_SetMatchAsNotBeingPlayed (long MchCod)
  {
   /***** Delete all match players ******/
   DB_QueryDELETE ("can not update match players",
		    "DELETE FROM mch_players"
		    " WHERE MchCod=%ld",
		    MchCod);

   /***** Delete match as being played ******/
   DB_QueryDELETE ("can not set match as not being played",
		    "DELETE FROM mch_playing"
		    " WHERE MchCod=%ld",
		    MchCod);
  }

static bool Mch_GetIfMatchIsBeingPlayed (long MchCod)
  {
   /***** Get if a match is being played or not *****/
   return
   (bool) (DB_QueryCOUNT ("can not get if match is being played",
			  "SELECT COUNT(*) FROM mch_playing"
			  " WHERE MchCod=%ld",
			  MchCod) != 0);
  }

static void Mch_RegisterMeAsPlayerInMatch (long MchCod)
  {
   /***** Insert me as match player *****/
   DB_QueryREPLACE ("can not insert match player",
		    "REPLACE mch_players (MchCod,UsrCod) VALUES (%ld,%ld)",
		    MchCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

static void Mch_GetNumPlayers (struct Match *Match)
  {
   /***** Get number of players who are playing a match *****/
   Match->Status.NumPlayers =
   (unsigned) DB_QueryCOUNT ("can not get number of players",
			     "SELECT COUNT(*) FROM mch_players"
			     " WHERE MchCod=%ld",
			     Match->MchCod);
  }

/*****************************************************************************/
/******************* Show the results of a finished match ********************/
/*****************************************************************************/

void Mch_ShowFinishedMatchResults (void)
  {
   Ale_ShowAlert (Ale_INFO,"To be implemented...");
  }

/*****************************************************************************/
/********************** Get code of match being played ***********************/
/*****************************************************************************/

void Mch_GetMatchBeingPlayed (void)
  {
   /***** Get match code ****/
   if ((Gbl.Games.MchCodBeingPlayed = Mch_GetParamMatchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");
  }

/*****************************************************************************/
/********* Show game being played to me as student in a new window ***********/
/*****************************************************************************/

void Mch_ShowMatchToMeAsStd (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForStd (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Refresh match for a teacher via AJAX *********************/
/*****************************************************************************/

void Mch_RefreshMatchTch (void)
  {
   struct Match Match;

   if (!Gbl.Session.IsOpen)	// If session has been closed, do not write anything
      return;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Update elapsed time in this question *****/
   Mch_UpdateElapsedTimeInQuestion (&Match);

   /***** Show current match status *****/
   Mch_ShowMatchStatusForTch (&Match);
  }

/*****************************************************************************/
/*************** Refresh current game for a student via AJAX *****************/
/*****************************************************************************/

void Mch_RefreshMatchStd (void)
  {
   struct Match Match;

   if (!Gbl.Session.IsOpen)	// If session has been closed, do not write anything
      return;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Show current match status *****/
   Mch_ShowMatchStatusForStd (&Match);
  }

/*****************************************************************************/
/**** Receive previous question answer in a match question from database *****/
/*****************************************************************************/

static int Mch_GetQstAnsFromDB (long MchCod,unsigned QstInd)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   int StdAnsInd = -1;	// <0 ==> no answer selected

   /***** Get student's answer *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get student's answer to a match question",
					"SELECT AnsInd FROM mch_answers"
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

void Mch_ReceiveQstAnsFromStd (void)
  {
   struct Match Match;
   unsigned QstInd;
   unsigned StdAnsInd;
   int PreviousStdAnsInd;

   /***** Remove old players.
          This function must be called before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

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
      StdAnsInd = Mch_GetParamAnswer ();

      /***** Get previous student's answer to this question
	     (<0 ==> no answer) *****/
      PreviousStdAnsInd = Mch_GetQstAnsFromDB (Match.MchCod,QstInd);

      /***** Store student's answer *****/
      if (PreviousStdAnsInd == (int) StdAnsInd)
	 DB_QueryDELETE ("can not register your answer to the match question",
			  "DELETE FROM mch_answers"
			  " WHERE MchCod=%ld AND UsrCod=%ld AND QstInd=%u",
			  Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,QstInd);
      else
	 DB_QueryREPLACE ("can not register your answer to the match question",
			  "REPLACE mch_answers"
			  " (MchCod,UsrCod,QstInd,AnsInd)"
			  " VALUES"
			  " (%ld,%ld,%u,%u)",
			  Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,QstInd,StdAnsInd);
     }

   /***** Show current match status *****/
   fprintf (Gbl.F.Out,"<div id=\"game\" class=\"MATCH_CONT\">");
   Mch_ShowMatchStatusForStd (&Match);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*** Get number of users who selected this answer and draw proportional bar **/
/*****************************************************************************/

void Mch_GetAndDrawBarNumUsrsWhoHaveChosenAns (long MchCod,unsigned QstInd,unsigned AnsInd,
					       unsigned NumAnswerersQst,bool Correct)
  {
   unsigned NumAnswerersAns;

   /***** Get number of users who selected this answer *****/
   NumAnswerersAns = Mch_GetNumUsrsWhoHaveChosenAns (MchCod,QstInd,AnsInd);

   /***** Show stats of this answer *****/
   Mch_DrawBarNumUsrs (NumAnswerersAns,NumAnswerersQst,Correct);
  }

/*****************************************************************************/
/******* Get number of users who have answered a question in a match *********/
/*****************************************************************************/

unsigned Mch_GetNumUsrsWhoHaveAnswerQst (long MchCod,unsigned QstInd)
  {
   /***** Get number of users who have answered
          a question in match from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of users who hasve answered a question",
			     "SELECT COUNT(*) FROM mch_answers"
			     " WHERE MchCod=%ld AND QstInd=%u",
			     MchCod,QstInd);
  }

/*****************************************************************************/
/*** Get number of users who have chosen a given answer of a game question ***/
/*****************************************************************************/

static unsigned Mch_GetNumUsrsWhoHaveChosenAns (long MchCod,unsigned QstInd,unsigned AnsInd)
  {
   /***** Get number of users who have chosen
          an answer of a question from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of users who have chosen an answer",
			     "SELECT COUNT(*) FROM mch_answers"
			     " WHERE MchCod=%ld AND QstInd=%u AND AnsInd=%u",
			     MchCod,QstInd,AnsInd);
  }

/*****************************************************************************/
/****** Get number of users who have answered any question in a match ********/
/*****************************************************************************/

static unsigned Mch_GetNumUsrsWhoHaveAnswerMch (long MchCod)
  {
   /***** Get number of users who have answered
          any question in match from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of users who hasve answered a match",
			     "SELECT COUNT(DISTINCT UsrCod) FROM mch_answers"
			     " WHERE MchCod=%ld",
			     MchCod);
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

// #define Mch_MAX_BAR_WIDTH 400
#define Mch_MAX_BAR_WIDTH 100

static void Mch_DrawBarNumUsrs (unsigned NumAnswerersAns,unsigned NumAnswerersQst,bool Correct)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned i;
   unsigned BarWidth = 0;

   /***** String with the number of users *****/
   if (NumAnswerersQst)
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u&nbsp;(%u%%&nbsp;%s&nbsp;%u)",
                NumAnswerersAns,
                (unsigned) ((((float) NumAnswerersAns * 100.0) / (float) NumAnswerersQst) + 0.5),
                Txt_of_PART_OF_A_TOTAL,NumAnswerersQst);
   else
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "0&nbsp;(0%%&nbsp;%s&nbsp;%u)",
                Txt_of_PART_OF_A_TOTAL,NumAnswerersQst);

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"MATCH_RESULT\">");

   /***** Draw bar with a with proportional to the number of clicks *****/
   if (NumAnswerersAns && NumAnswerersQst)
      BarWidth = (unsigned) ((((float) NumAnswerersAns * (float) Mch_MAX_BAR_WIDTH) /
	                       (float) NumAnswerersQst) + 0.5);

   /***** Bar proportional to number of users *****/
   Tbl_StartTableWide (0);
   fprintf (Gbl.F.Out,"<tr class=\"MATCH_RES_TR\">");
   for (i = 0;
	i < 100;
	i++)
      fprintf (Gbl.F.Out,"<td class=\"%s\"></td>",
	       (i < BarWidth) ? (Correct ? "MATCH_RES_CORRECT" :
					   "MATCH_RES_WRONG") :
				"MATCH_RES_VOID");
   fprintf (Gbl.F.Out,"</tr>");
   Tbl_EndTable ();

   /***** Write the number of users *****/
   fprintf (Gbl.F.Out,"%s",Gbl.Title);

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/****************** Write a form to go to result of matches ******************/
/*****************************************************************************/

void Mch_PutFormToViewResultsOfMatches (Act_Action_t Action)
  {
   extern const char *Txt_Matches_results;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Lay_PutContextualLinkIconText (Action,NULL,NULL,
				  "tasks.svg",
				  Txt_Matches_results);
   fprintf (Gbl.F.Out,"</div>");
  }
