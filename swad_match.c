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

#include "swad_database.h"
#include "swad_date.h"
#include "swad_form.h"
#include "swad_game.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_match.h"
#include "swad_match_result.h"
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

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

typedef enum
  {
   Mch_CHANGE_STATUS_BY_STUDENT,
   Mch_REFRESH_STATUS_BY_SERVER,
  } Mch_Update_t;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

const char *Mch_ShowingStringsDB[Mch_NUM_SHOWING] =
  {
   [Mch_START  ] = "start",
   [Mch_STEM   ] = "stem",
   [Mch_ANSWERS] = "answers",
   [Mch_RESULTS] = "results",
   [Mch_END    ] = "end",
  };

#define Mch_MAX_COLS 4
#define Mch_NUM_COLS_DEFAULT 1

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

long Mch_CurrentMchCod = -1L;	// Used as parameter in contextual links

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mch_PutIconsInListOfMatches (void);
static void Mch_PutIconToCreateNewMatch (void);

static void Mch_ListOneOrMoreMatches (const struct Game *Game,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res);
static void Mch_ListOneOrMoreMatchesHeading (bool ICanEditMatches);
static bool Mch_CheckIfICanEditMatches (void);
static bool Mch_CheckIfICanEditThisMatch (const struct Match *Match);
static void Mch_ListOneOrMoreMatchesIcons (const struct Match *Match);
static void Mch_ListOneOrMoreMatchesAuthor (const struct Match *Match);
static void Mch_ListOneOrMoreMatchesTimes (const struct Match *Match,unsigned UniqueId);
static void Mch_ListOneOrMoreMatchesTitleGrps (const struct Match *Match);
static void Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (const struct Match *Match);
static void Mch_ListOneOrMoreMatchesNumPlayers (const struct Match *Match);
static void Mch_ListOneOrMoreMatchesStatus (const struct Match *Match,unsigned NumQsts);
static void Mch_ListOneOrMoreMatchesResult (const struct Match *Match);
static void Mch_ListOneOrMoreMatchesResultStd (const struct Match *Match);
static void Mch_ListOneOrMoreMatchesResultTch (const struct Match *Match);

static void Mch_GetMatchDataFromRow (MYSQL_RES *mysql_res,
				     struct Match *Match);
static Mch_Showing_t Mch_GetShowingFromStr (const char *Str);

static void Mch_RemoveMatchFromAllTables (long MchCod);
static void Mch_RemoveMatchFromTable (long MchCod,const char *TableName);
static void Mch_RemoveMatchesInGameFromTable (long GamCod,const char *TableName);
static void Mch_RemoveMatchInCourseFromTable (long CrsCod,const char *TableName);
static void Mch_RemoveUsrMchResultsInCrs (long UsrCod,long CrsCod,const char *TableName);

static void Mch_PutParamsPlay (void);
static void Mch_PutParamMchCod (long MchCod);

static void Mch_PutFormNewMatch (const struct Game *Game);
static void Mch_ShowLstGrpsToCreateMatch (void);

static long Mch_CreateMatch (long GamCod,char Title[Gam_MAX_BYTES_TITLE + 1]);
static void Mch_CreateIndexes (long GamCod,long MchCod);
static void Mch_ReorderAnswer (long MchCod,unsigned QstInd,
			       long QstCod,bool Shuffle);
static void Mch_CreateGrps (long MchCod);
static void Mch_UpdateMatchStatusInDB (const struct Match *Match);

static void Mch_UpdateElapsedTimeInQuestion (const struct Match *Match);
static void Mch_GetElapsedTimeInQuestion (const struct Match *Match,
				          struct Time *Time);
static void Mch_GetElapsedTimeInMatch (const struct Match *Match,
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
static void Mch_ShowMatchStatusForStd (struct Match *Match,Mch_Update_t Update);

static void Mch_ShowLeftColumnTch (struct Match *Match);
static void Mch_ShowRefreshablePartTch (struct Match *Match);
static void Mch_WriteElapsedTimeInMch (struct Match *Match);
static void Mch_WriteElapsedTimeInQst (struct Match *Match);
static void Mch_WriteNumRespondersQst (struct Match *Match);

static void Mch_ShowRightColumnTch (const struct Match *Match);
static void Mch_ShowLeftColumnStd (const struct Match *Match,
				   const struct Mch_UsrAnswer *UsrAnswer);
static void Mch_ShowRightColumnStd (struct Match *Match,
				    const struct Mch_UsrAnswer *UsrAnswer,
				    Mch_Update_t Update);

static void Mch_ShowNumQstInMch (const struct Match *Match);
static void Mch_PutMatchControlButtons (const struct Match *Match);
static void Mch_ShowFormColumns (const struct Match *Match);
static void Mch_PutParamNumCols (unsigned NumCols);

static void Mch_ShowMatchTitle (const struct Match *Match);
static void Mch_PutCheckboxResult (const struct Match *Match);
static void Mch_PutIfAnswered (const struct Match *Match,bool Answered);
static void Mch_PutIconToRemoveMyAnswer (const struct Match *Match);
static void Mch_ShowQuestionAndAnswersTch (const struct Match *Match);
static void Mch_WriteAnswersMatchResult (const struct Match *Match,
                                         const char *Class,bool ShowResult);
static bool Mch_ShowQuestionAndAnswersStd (const struct Match *Match,
					   const struct Mch_UsrAnswer *UsrAnswer,
					   Mch_Update_t Update);

static void Mch_ShowMatchScore (const struct Match *Match);
static void Mch_DrawEmptyRowScore (unsigned NumRow,double MinScore,double MaxScore);
static void Mch_DrawScoreRow (double Score,double MinScore,double MaxScore,
			      unsigned NumRow,unsigned NumUsrs,unsigned MaxUsrs);
static const char *Mch_GetClassBorder (unsigned NumRow);

static void Mch_PutParamNumOpt (unsigned NumOpt);
static unsigned Mch_GetParamNumOpt (void);

static void Mch_PutBigButton (Act_Action_t NextAction,const char *Id,
			      long MchCod,const char *Icon,const char *Txt);
static void Mch_PutBigButtonOff (const char *Icon);
static void Mch_PutBigButtonClose (void);

static void Mch_ShowWaitImage (const char *Txt);

static void Mch_RemoveOldPlayers (void);
static void Mch_UpdateMatchAsBeingPlayed (long MchCod);
static void Mch_SetMatchAsNotBeingPlayed (long MchCod);
static bool Mch_GetIfMatchIsBeingPlayed (long MchCod);
static void Mch_GetNumPlayers (struct Match *Match);

static void Mch_RemoveMyAnswerToMatchQuestion (const struct Match *Match);

static double Mch_ComputeScore (unsigned NumQsts);

static unsigned Mch_GetNumUsrsWhoHaveChosenAns (long MchCod,unsigned QstInd,unsigned AnsInd);
static unsigned Mch_GetNumUsrsWhoHaveAnswerMch (long MchCod);
static void Mch_DrawBarNumUsrs (unsigned NumRespondersAns,unsigned NumRespondersQst,bool Correct);

static long Mch_GetCurrentMchCod (void);

/*****************************************************************************/
/************************* List the matches of a game ************************/
/*****************************************************************************/

void Mch_ListMatches (struct Game *Game,bool PutFormNewMatch)
  {
   extern const char *Hlp_ASSESSMENT_Games_matches;
   extern const char *Txt_Matches;
   char *SubQuery;
   MYSQL_RES *mysql_res;
   unsigned NumMatches;

   /***** Get data of matches from database *****/
   /* Fill subquery for game */
   if (Gbl.Crs.Grps.WhichGrps == Grp_MY_GROUPS)
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
					   "SELECT MchCod,"				// row[ 0]
						  "GamCod,"				// row[ 1]
						  "UsrCod,"				// row[ 2]
						  "UNIX_TIMESTAMP(StartTime),"		// row[ 3]
						  "UNIX_TIMESTAMP(EndTime),"		// row[ 4]
						  "Title,"				// row[ 5]
						  "QstInd,"				// row[ 6]
						  "QstCod,"				// row[ 7]
						  "Showing,"				// row[ 8]
						  "NumCols,"				// row[ 9]
					          "ShowQstResults,"			// row[10]
					          "ShowUsrResults"			// row[11]
					   " FROM mch_matches"
					   " WHERE GamCod=%ld%s"
					   " ORDER BY MchCod",
					   Game->GamCod,
					   SubQuery);

   /* Free allocated memory for subquery */
   free (SubQuery);

   /***** Begin box *****/
   Gam_SetCurrentGamCod (Game->GamCod);	// Used to pass parameter
   Box_BoxBegin ("100%",Txt_Matches,Mch_PutIconsInListOfMatches,
                 Hlp_ASSESSMENT_Games_matches,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.Crs.Grps.NumGrps)
     {
      Set_StartSettingsHead ();
      Grp_ShowFormToSelWhichGrps (ActSeeGam,Gam_PutParams);
      Set_EndSettingsHead ();
     }

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
	    Gam_PutButtonNewMatch (Game->GamCod);	// Button to create a new match
	 break;
      default:
	 break;
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Get match data using its code ************************/
/*****************************************************************************/

void Mch_GetDataOfMatchByCod (struct Match *Match)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;
   Dat_StartEndTime_t StartEndTime;

   /***** Get data of match from database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get matches",
					"SELECT MchCod,"			// row[ 0]
					       "GamCod,"			// row[ 1]
					       "UsrCod,"			// row[ 2]
					       "UNIX_TIMESTAMP(StartTime),"	// row[ 3]
					       "UNIX_TIMESTAMP(EndTime),"	// row[ 4]
					       "Title,"				// row[ 5]
					       "QstInd,"			// row[ 6]
					       "QstCod,"			// row[ 7]
					       "Showing,"			// row[ 8]
					       "NumCols,"			// row[ 9]
					       "ShowQstResults,"		// row[10]
					       "ShowUsrResults"			// row[11]
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
      for (StartEndTime = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
         Match->TimeUTC[StartEndTime] = (time_t) 0;
      Match->Title[0]                = '\0';
      Match->Status.QstInd           = 0;
      Match->Status.QstCod           = -1L;
      Match->Status.QstStartTimeUTC  = (time_t) 0;
      Match->Status.Showing          = Mch_START;
      Match->Status.Playing          = false;
      Match->Status.NumPlayers       = 0;
      Match->Status.NumCols          = Mch_NUM_COLS_DEFAULT;
      Match->Status.ShowQstResults   = false;
      Match->Status.ShowUsrResults   = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Put icons in list of matches of a game *******************/
/*****************************************************************************/

static void Mch_PutIconsInListOfMatches (void)
  {
   bool ICanEditMatches = Mch_CheckIfICanEditMatches ();

   /***** Put icon to create a new match in current game *****/
   if (ICanEditMatches)
      Mch_PutIconToCreateNewMatch ();
  }

/*****************************************************************************/
/********************* Put icon to create a new match ************************/
/*****************************************************************************/

static void Mch_PutIconToCreateNewMatch (void)
  {
   extern const char *Txt_New_match;

   /***** Put form to create a new match *****/
   Ico_PutContextualIconToAdd (ActReqNewMch,Mch_NEW_MATCH_SECTION_ID,Gam_PutParams,
			       Txt_New_match);
  }

/*****************************************************************************/
/*********************** List game matches for edition ***********************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatches (const struct Game *Game,
				      unsigned NumMatches,
                                      MYSQL_RES *mysql_res)
  {
   unsigned NumMatch;
   unsigned UniqueId;
   struct Match Match;
   bool ICanEditMatches = Mch_CheckIfICanEditMatches ();

   /***** Write the heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Mch_ListOneOrMoreMatchesHeading (ICanEditMatches);

   /***** Write rows *****/
   for (NumMatch = 0, UniqueId = 1;
	NumMatch < NumMatches;
	NumMatch++, UniqueId++)
     {
      Gbl.RowEvenOdd = NumMatch % 2;

      /***** Get match data from row *****/
      Mch_GetMatchDataFromRow (mysql_res,&Match);

      if (Mch_CheckIfICanPlayThisMatchBasedOnGrps (&Match))
	{
	 /***** Write row for this match ****/
	 HTM_TR_Begin (NULL);

	 /* Icons */
	 if (ICanEditMatches)
	    Mch_ListOneOrMoreMatchesIcons (&Match);

	 /* Match player */
	 Mch_ListOneOrMoreMatchesAuthor (&Match);

	 /* Start/end date/time */
	 Mch_ListOneOrMoreMatchesTimes (&Match,UniqueId);

	 /* Title and groups */
	 Mch_ListOneOrMoreMatchesTitleGrps (&Match);

	 /* Number of players who have answered any question in the match */
	 Mch_ListOneOrMoreMatchesNumPlayers (&Match);

	 /* Match status */
	 Mch_ListOneOrMoreMatchesStatus (&Match,Game->NumQsts);

	 /* Match result visible? */
	 Mch_ListOneOrMoreMatchesResult (&Match);
	}
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/***************** Put a column for match start and end times ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesHeading (bool ICanEditMatches)
  {
   extern const char *Txt_ROLES_SINGUL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_START_END_TIME[Dat_NUM_START_END_TIME];
   extern const char *Txt_Match;
   extern const char *Txt_Players;
   extern const char *Txt_Status;
   extern const char *Txt_Result;

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Column for icons *****/
   if (ICanEditMatches)
      HTM_TH_Empty (1);

   /***** The rest of columns *****/
   HTM_TH (1,1,"LT",Txt_ROLES_SINGUL_Abc[Rol_TCH][Usr_SEX_UNKNOWN]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Gam_ORDER_BY_START_DATE]);
   HTM_TH (1,1,"LT",Txt_START_END_TIME[Gam_ORDER_BY_END_DATE]);
   HTM_TH (1,1,"LT",Txt_Match);
   HTM_TH (1,1,"RT",Txt_Players);
   HTM_TH (1,1,"CT",Txt_Status);
   HTM_TH (1,1,"CT",Txt_Result);

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********************** Check if I can edit matches *************************/
/*****************************************************************************/

static bool Mch_CheckIfICanEditMatches (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
  }

/*****************************************************************************/
/***************** Check if I can edit (remove/resume) a match ***************/
/*****************************************************************************/

static bool Mch_CheckIfICanEditThisMatch (const struct Match *Match)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return (Match->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);	// Only if I am the creator
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************************* Put a column for icons ****************************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesIcons (const struct Match *Match)
  {
   HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);

   /***** Put icon to remove the match *****/
   if (Mch_CheckIfICanEditThisMatch (Match))
     {
      Gam_SetCurrentGamCod (Match->GamCod);	// Used to pass parameter
      Mch_SetCurrentMchCod (Match->MchCod);	// Used to pass parameter
      Frm_StartForm (ActReqRemMch);
      Mch_PutParamsEdit ();
      Ico_PutIconRemove ();
      Frm_EndForm ();
     }
   else
      Ico_PutIconRemovalNotAllowed ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Put a column for teacher who created the match ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesAuthor (const struct Match *Match)
  {
   /***** Match author (teacher) *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Usr_WriteAuthor1Line (Match->UsrCod,false);
   HTM_TD_End ();
  }

/*****************************************************************************/
/***************** Put a column for match start and end times ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesTimes (const struct Match *Match,unsigned UniqueId)
  {
   Dat_StartEndTime_t StartEndTime;
   char *Id;

   for (StartEndTime = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"mch_time_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		    Id,
		    Match->Status.Showing == Mch_END ? "DATE_RED" :
						       "DATE_GREEN",
		    Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,Match->TimeUTC[StartEndTime],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);
     }
  }

/*****************************************************************************/
/***************** Put a column for match title and grous ********************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesTitleGrps (const struct Match *Match)
  {
   extern const char *Txt_Play;
   extern const char *Txt_Resume;

   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);

   /***** Match title *****/
   Mch_SetCurrentMchCod (Match->MchCod);	// Used to pass parameter
   Frm_StartForm (Gbl.Usrs.Me.Role.Logged == Rol_STD ? ActJoiMch :
						       ActResMch);
   Mch_PutParamsPlay ();
   HTM_BUTTON_SUBMIT_Begin (Gbl.Usrs.Me.Role.Logged == Rol_STD ? Txt_Play :
								 Txt_Resume,
			    "BT_LINK ASG_TITLE",NULL);
   HTM_Txt (Match->Title);
   HTM_BUTTON_End ();
   Frm_EndForm ();

   /***** Groups whose students can answer this match *****/
   if (Gbl.Crs.Grps.NumGrps)
      Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (Match);

   HTM_TD_End ();
  }

/*****************************************************************************/
/************* Get and write the names of the groups of a match **************/
/*****************************************************************************/

static void Mch_GetAndWriteNamesOfGrpsAssociatedToMatch (const struct Match *Match)
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
   HTM_DIV_Begin ("class=\"ASG_GRP\"");
   HTM_TxtF ("%s:&nbsp;",NumRows == 1 ? Txt_Group  :
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
         HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

         if (NumRows >= 2)
           {
            if (NumRow == NumRows-2)
               HTM_TxtF (" %s ",Txt_and);
            if (NumRows >= 3)
              if (NumRow < NumRows-2)
                  HTM_Txt (", ");
           }
        }
     }
   else
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Put a column for number of players **********************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesNumPlayers (const struct Match *Match)
  {
   /***** Number of players who have answered any question in the match ******/
   HTM_TD_Begin ("class=\"DAT RT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_Unsigned (Mch_GetNumUsrsWhoHaveAnswerMch (Match->MchCod));
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************** Put a column for match status ************************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesStatus (const struct Match *Match,unsigned NumQsts)
  {
   extern const char *Txt_Play;
   extern const char *Txt_Resume;

   HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);

   if (Match->Status.Showing != Mch_END)	// Match not over
     {
      /* Current question index / total of questions */
      HTM_DIV_Begin ("class=\"DAT\"");
      HTM_TxtF ("%u/%u",Match->Status.QstInd,NumQsts);
      HTM_DIV_End ();
     }

   /* Icon to join match or resume match */
   Mch_SetCurrentMchCod (Match->MchCod);	// Used to pass parameter
   Lay_PutContextualLinkOnlyIcon (Gbl.Usrs.Me.Role.Logged == Rol_STD ? ActJoiMch :
								       ActResMch,
				  NULL,
				  Mch_PutParamsPlay,
				  Match->Status.Showing == Mch_END ? "flag-checkered.svg" :
					                             "play.svg",
				  Gbl.Usrs.Me.Role.Logged == Rol_STD ? Txt_Play :
								       Txt_Resume);

   HTM_TD_End ();
  }

/*****************************************************************************/
/**************** Put a column for visibility of match result ****************/
/*****************************************************************************/

static void Mch_ListOneOrMoreMatchesResult (const struct Match *Match)
  {
   HTM_TD_Begin ("class=\"DAT CT COLOR%u\"",Gbl.RowEvenOdd);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 Mch_ListOneOrMoreMatchesResultStd (Match);
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 Mch_ListOneOrMoreMatchesResultTch (Match);
	 break;
      default:
	 Rol_WrongRoleExit ();
	 break;
     }

   HTM_TD_End ();
  }

static void Mch_ListOneOrMoreMatchesResultStd (const struct Match *Match)
  {
   extern const char *Txt_Hidden_results;
   extern const char *Txt_Results;

   /***** Is match result visible or hidden? *****/
   if (Match->Status.ShowUsrResults)
     {
      /* Result is visible by me */
      Gam_SetCurrentGamCod (Match->GamCod);	// Used to pass parameter
      Mch_SetCurrentMchCod (Match->MchCod);	// Used to pass parameter
      Lay_PutContextualLinkOnlyIcon (ActSeeMyMchResMch,McR_RESULTS_BOX_ID,
				     Mch_PutParamsEdit,
				     "trophy.svg",
				     Txt_Results);
     }
   else
      /* Result is forbidden to me */
      Ico_PutIconOff ("eye-slash.svg",Txt_Hidden_results);
  }

static void Mch_ListOneOrMoreMatchesResultTch (const struct Match *Match)
  {
   extern const char *Txt_Visible_results;
   extern const char *Txt_Hidden_results;
   extern const char *Txt_Results;

   /***** Can I edit match vivibility? *****/
   if (Mch_CheckIfICanEditThisMatch (Match))
     {
      Gam_SetCurrentGamCod (Match->GamCod);	// Used to pass parameter
      Mch_SetCurrentMchCod (Match->MchCod);	// Used to pass parameter

      /* Show match results */
      Lay_PutContextualLinkOnlyIcon (ActSeeAllMchResMch,McR_RESULTS_BOX_ID,
				     Mch_PutParamsEdit,
				     "trophy.svg",
				     Txt_Results);

      /* I can edit visibility */
      Lay_PutContextualLinkOnlyIcon (ActChgVisResMchUsr,NULL,
				     Mch_PutParamsEdit,
				     Match->Status.ShowUsrResults ? "eye.svg" :
								    "eye-slash.svg",
				     Match->Status.ShowUsrResults ? Txt_Visible_results :
								    Txt_Hidden_results);
     }
   else
      /* I can not edit visibility */
      Ico_PutIconOff (Match->Status.ShowUsrResults ? "eye.svg" :
						     "eye-slash.svg",
		      Match->Status.ShowUsrResults ? Txt_Visible_results :
						     Txt_Hidden_results);
  }

/*****************************************************************************/
/******************** Toggle visibility of match results *********************/
/*****************************************************************************/

void Mch_ToggleVisibilResultsMchUsr (void)
  {
   struct Game Game;
   struct Match Match;

   /***** Get and check parameters *****/
   Mch_GetAndCheckParameters (&Game,&Match);

   /***** Check if I have permission to change visibility *****/
   if (!Mch_CheckIfICanEditThisMatch (&Match))
      Lay_NoPermissionExit ();

   /***** Toggle visibility of match results *****/
   Match.Status.ShowUsrResults = !Match.Status.ShowUsrResults;
   DB_QueryUPDATE ("can not toggle visibility of match results",
		   "UPDATE mch_matches"
		   " SET ShowUsrResults='%c'"
		   " WHERE MchCod=%ld",
		   Match.Status.ShowUsrResults ? 'Y' :
			                         'N',
		   Match.MchCod);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Game,
                        false,	// Do not list game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/******************** Get game data from a database row **********************/
/*****************************************************************************/

static void Mch_GetMatchDataFromRow (MYSQL_RES *mysql_res,
				     struct Match *Match)
  {
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;
   long LongNum;

   /***** Get match data *****/
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
      Lay_ShowErrorAndExit ("Wrong code of match.");

   /* Code of the game (row[1]) */
   if ((Match->GamCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of game.");

   /* Get match teacher (row[2]) */
   Match->UsrCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get start/end times (row[3], row[4] hold start/end UTC times) */
   for (StartEndTime = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
      Match->TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[3 + StartEndTime]);

   /* Get the title of the game (row[5]) */
   if (row[5])
      Str_Copy (Match->Title,row[5],
		Gam_MAX_BYTES_TITLE);
   else
      Match->Title[0] = '\0';

   /***** Get current match status *****/
   /*
   row[ 6]	QstInd
   row[ 7]	QstCod
   row[ 8]	Showing
   row[ 9]	NumCols
   row[10]	ShowQstResults
   row[11]	ShowUsrResults
   */
   /* Current question index (row[6]) */
   Match->Status.QstInd = Gam_GetQstIndFromStr (row[6]);

   /* Current question code (row[7]) */
   Match->Status.QstCod = Str_ConvertStrCodToLongCod (row[7]);

   /* Get what to show (stem, answers, results) (row(8)) */
   Match->Status.Showing = Mch_GetShowingFromStr (row[8]);

   /* Get number of columns (row[9]) */
   LongNum = Str_ConvertStrCodToLongCod (row[9]);
   Match->Status.NumCols =  (LongNum <= 1           ) ? 1 :
                           ((LongNum >= Mch_MAX_COLS) ? Mch_MAX_COLS :
                        	                        (unsigned) LongNum);

   /* Get whether to show question results or not (row(10)) */
   Match->Status.ShowQstResults = (row[10][0] == 'Y');

   /* Get whether to show user results or not (row(11)) */
   Match->Status.ShowUsrResults = (row[11][0] == 'Y');

   /***** Get whether the match is being played or not *****/
   if (Match->Status.Showing == Mch_END)	// Match over
      Match->Status.Playing = false;
   else						// Match not over
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

void Mch_RequestRemoveMatch (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_match_X;
   extern const char *Txt_Remove_match;
   struct Game Game;
   struct Match Match;

   /***** Get and check parameters *****/
   Mch_GetAndCheckParameters (&Game,&Match);

   /***** Show question and button to remove question *****/
   Gam_SetCurrentGamCod (Match.GamCod);	// Used to pass parameter
   Mch_SetCurrentMchCod (Match.MchCod);	// Used to pass parameter
   Ale_ShowAlertAndButton (ActRemMch,NULL,NULL,Mch_PutParamsEdit,
			   Btn_REMOVE_BUTTON,Txt_Remove_match,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_match_X,
	                   Match.Title);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Game,
                        false,	// Do not list game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/********************** Remove a match (game instance) ***********************/
/*****************************************************************************/

void Mch_RemoveMatch (void)
  {
   extern const char *Txt_Match_X_removed;
   struct Game Game;
   struct Match Match;

   /***** Get and check parameters *****/
   Mch_GetAndCheckParameters (&Game,&Match);

   /***** Check if I can remove this match *****/
   if (!Mch_CheckIfICanEditThisMatch (&Match))
      Lay_NoPermissionExit ();

   /***** Remove the match from all database tables *****/
   Mch_RemoveMatchFromAllTables (Match.MchCod);

   /***** Write message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Match_X_removed,
		  Match.Title);

   /***** Show current game *****/
   Gam_ShowOnlyOneGame (&Game,
                        false,	// Do not list game questions
	                false);	// Do not put form to start new match
  }

/*****************************************************************************/
/********************** Remove match from all tables *************************/
/*****************************************************************************/
/*
mysql> SELECT table_name FROM information_schema.tables WHERE table_name LIKE 'mch%';
*/
static void Mch_RemoveMatchFromAllTables (long MchCod)
  {
   /***** Remove match from secondary tables *****/
   Mch_RemoveMatchFromTable (MchCod,"mch_players");
   Mch_RemoveMatchFromTable (MchCod,"mch_playing");
   Mch_RemoveMatchFromTable (MchCod,"mch_results");
   Mch_RemoveMatchFromTable (MchCod,"mch_answers");
   Mch_RemoveMatchFromTable (MchCod,"mch_times");
   Mch_RemoveMatchFromTable (MchCod,"mch_groups");
   Mch_RemoveMatchFromTable (MchCod,"mch_indexes");

   /***** Remove match from main table *****/
   DB_QueryDELETE ("can not remove match",
		   "DELETE FROM mch_matches WHERE MchCod=%ld",
		   MchCod);
  }

static void Mch_RemoveMatchFromTable (long MchCod,const char *TableName)
  {
   /***** Remove match from secondary table *****/
   DB_QueryDELETE ("can not remove match from table",
		   "DELETE FROM %s WHERE MchCod=%ld",
		   TableName,
		   MchCod);
  }

/*****************************************************************************/
/******************** Remove match in game from all tables *******************/
/*****************************************************************************/

void Mch_RemoveMatchesInGameFromAllTables (long GamCod)
  {
   /***** Remove matches from secondary tables *****/
   Mch_RemoveMatchesInGameFromTable (GamCod,"mch_players");
   Mch_RemoveMatchesInGameFromTable (GamCod,"mch_playing");
   Mch_RemoveMatchesInGameFromTable (GamCod,"mch_results");
   Mch_RemoveMatchesInGameFromTable (GamCod,"mch_answers");
   Mch_RemoveMatchesInGameFromTable (GamCod,"mch_times");
   Mch_RemoveMatchesInGameFromTable (GamCod,"mch_groups");
   Mch_RemoveMatchesInGameFromTable (GamCod,"mch_indexes");

   /***** Remove matches from main table *****/
   DB_QueryDELETE ("can not remove matches of a game",
		   "DELETE FROM mch_matches WHERE GamCod=%ld",
		   GamCod);
  }

static void Mch_RemoveMatchesInGameFromTable (long GamCod,const char *TableName)
  {
   /***** Remove matches in game from secondary table *****/
   DB_QueryDELETE ("can not remove matches of a game from table",
		   "DELETE FROM %s"
		   " USING mch_matches,%s"
		   " WHERE mch_matches.GamCod=%ld"
		   " AND mch_matches.MchCod=%s.MchCod",
		   TableName,
		   TableName,
		   GamCod,
		   TableName);
  }

/*****************************************************************************/
/******************* Remove match in course from all tables ******************/
/*****************************************************************************/

void Mch_RemoveMatchInCourseFromAllTables (long CrsCod)
  {
   /***** Remove matches from secondary tables *****/
   Mch_RemoveMatchInCourseFromTable (CrsCod,"mch_players");
   Mch_RemoveMatchInCourseFromTable (CrsCod,"mch_playing");
   Mch_RemoveMatchInCourseFromTable (CrsCod,"mch_results");
   Mch_RemoveMatchInCourseFromTable (CrsCod,"mch_answers");
   Mch_RemoveMatchInCourseFromTable (CrsCod,"mch_times");
   Mch_RemoveMatchInCourseFromTable (CrsCod,"mch_groups");
   Mch_RemoveMatchInCourseFromTable (CrsCod,"mch_indexes");

   /***** Remove matches from main table *****/
   DB_QueryDELETE ("can not remove matches of a course",
		   "DELETE FROM mch_matches"
		   " USING gam_games,mch_matches"
		   " WHERE gam_games.CrsCod=%ld"
		   " AND gam_games.GamCod=mch_matches.GamCod",
		   CrsCod);
  }

static void Mch_RemoveMatchInCourseFromTable (long CrsCod,const char *TableName)
  {
   /***** Remove matches in course from secondary table *****/
   DB_QueryDELETE ("can not remove matches of a course from table",
		   "DELETE FROM %s"
		   " USING gam_games,mch_matches,%s"
		   " WHERE gam_games.CrsCod=%ld"
		   " AND gam_games.GamCod=mch_matches.GamCod"
		   " AND mch_matches.MchCod=%s.MchCod",
		   TableName,
		   TableName,
		   CrsCod,
		   TableName);
  }

/*****************************************************************************/
/***************** Remove user from secondary match tables *******************/
/*****************************************************************************/

void Mch_RemoveUsrFromMatchTablesInCrs (long UsrCod,long CrsCod)
  {
   /***** Remove student from secondary tables *****/
   Mch_RemoveUsrMchResultsInCrs (UsrCod,CrsCod,"mch_players");
   Mch_RemoveUsrMchResultsInCrs (UsrCod,CrsCod,"mch_results");
   Mch_RemoveUsrMchResultsInCrs (UsrCod,CrsCod,"mch_answers");
  }

static void Mch_RemoveUsrMchResultsInCrs (long UsrCod,long CrsCod,const char *TableName)
  {
   /***** Remove matches in course from secondary table *****/
   DB_QueryDELETE ("can not remove matches of a user from table",
		   "DELETE FROM %s"
		   " USING gam_games,mch_matches,%s"
		   " WHERE gam_games.CrsCod=%ld"
		   " AND gam_games.GamCod=mch_matches.GamCod"
		   " AND mch_matches.MchCod=%s.MchCod"
		   " AND %s.UsrCod=%ld",
		   TableName,
		   TableName,
		   CrsCod,
		   TableName,
		   TableName,
		   UsrCod);
  }

/*****************************************************************************/
/*********************** Params used to edit a match *************************/
/*****************************************************************************/

void Mch_PutParamsEdit (void)
  {
   Gam_PutParams ();
   Mch_PutParamsPlay ();
  }

/*****************************************************************************/
/*********************** Params used to edit a match *************************/
/*****************************************************************************/

static void Mch_PutParamsPlay (void)
  {
   long CurrentMchCod = Mch_GetCurrentMchCod ();

   if (CurrentMchCod > 0)
      Mch_PutParamMchCod (CurrentMchCod);
  }

/*****************************************************************************/
/******************** Write parameter with code of match **********************/
/*****************************************************************************/

static void Mch_PutParamMchCod (long MchCod)
  {
   Par_PutHiddenParamLong (NULL,"MchCod",MchCod);
  }

/*****************************************************************************/
/************************** Get and check parameters *************************/
/*****************************************************************************/

void Mch_GetAndCheckParameters (struct Game *Game,struct Match *Match)
  {
   /***** Get parameters *****/
   /* Get parameters of game */
   if ((Game->GamCod = Gam_GetParams ()) == -1L)
      Lay_ShowErrorAndExit ("Code of game is missing.");
   Grp_GetParamWhichGrps ();
   Gam_GetDataOfGameByCod (Game);

   /* Get match code */
   if ((Match->MchCod = Mch_GetParamMchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");
   Mch_GetDataOfMatchByCod (Match);

   /***** Ensure parameters are correct *****/
   if (Game->GamCod != Match->GamCod)
      Lay_ShowErrorAndExit ("Wrong game code.");
   if (Game->CrsCod != Gbl.Hierarchy.Crs.CrsCod)
      Lay_ShowErrorAndExit ("Match does not belong to this course.");
  }

/*****************************************************************************/
/********************* Get parameter with code of match **********************/
/*****************************************************************************/

long Mch_GetParamMchCod (void)
  {
   /***** Get code of match *****/
   return Par_GetParToLong ("MchCod");
  }

/*****************************************************************************/
/****** Put a big button to play match (start a new match) as a teacher ******/
/*****************************************************************************/

static void Mch_PutFormNewMatch (const struct Game *Game)
  {
   extern const char *Hlp_ASSESSMENT_Games_matches;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_match;
   extern const char *Txt_Title;
   extern const char *Txt_Play;

   /***** Start section for a new match *****/
   HTM_SECTION_Begin (Mch_NEW_MATCH_SECTION_ID);

   /***** Begin form *****/
   Frm_StartForm (ActNewMch);
   Gam_PutParamGameCod (Game->GamCod);
   Gam_PutParamQstInd (0);	// Start by first question in game

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_match,NULL,
		      Hlp_ASSESSMENT_Games_matches,Box_NOT_CLOSABLE,2);

   /***** Match title *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RM\"");
   HTM_LABEL_Begin ("for=\"Title\" class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_Title);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Title",Gam_MAX_CHARS_TITLE,Game->Title,false,
		   "id=\"Title\" size=\"45\" required=\"required\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Groups *****/
   Mch_ShowLstGrpsToCreateMatch ();

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Put icon to submit the form *****/
   HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"play.svg",
		    Txt_Play,"CONTEXT_OPT ICO_HIGHLIGHT ICO64x64");

   /***** End box *****/
   Box_BoxEnd ();

   /***** End form *****/
   Frm_EndForm ();

   /***** End section for a new match *****/
   HTM_SECTION_End ();
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
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Groups);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");
      Box_BoxTableBegin ("95%",NULL,NULL,
                         NULL,Box_NOT_CLOSABLE,0);

      /***** First row: checkbox to select the whole course *****/
      HTM_TR_Begin (NULL);
      HTM_TD_Begin ("colspan=\"7\" class=\"DAT LM\"");
      HTM_LABEL_Begin (NULL);
      HTM_INPUT_CHECKBOX ("WholeCrs",true,
			  "id=\"WholeCrs\" value=\"Y\" checked=\"checked\""
			  " onclick=\"uncheckChildren(this,'GrpCods')\"");
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttSvyMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
                                            -1L,	// -1 means "New match"
					    Grp_MATCH);

      /***** End table and box *****/
      Box_BoxTableEnd ();
      HTM_TD_End ();
      HTM_TR_End ();
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

void Mch_ResumeMatch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Check if I have permission to resume match *****/
   if (!Mch_CheckIfICanEditThisMatch (&Match))
      Lay_NoPermissionExit ();

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

static long Mch_CreateMatch (long GamCod,char Title[Gam_MAX_BYTES_TITLE + 1])
  {
   long MchCod;

   /***** Insert this new match into database *****/
   MchCod = DB_QueryINSERTandReturnCode ("can not create match",
				         "INSERT mch_matches "
				         "(GamCod,UsrCod,StartTime,EndTime,Title,"
				         "QstInd,QstCod,Showing,NumCols,"
				         "ShowQstResults,ShowUsrResults)"
				         " VALUES "
				         "(%ld,"	// GamCod
				         "%ld,"		// UsrCod
				         "NOW(),"	// StartTime
				         "NOW(),"	// EndTime
				         "'%s',"	// Title
				         "0,"		// QstInd: Match has not started, so not the first question yet
				         "-1,"		// QstCod: Non-existent question
				         "'%s',"	// Showing: What is being shown
					 "%u,"		// NumCols: Number of columns in answers
				         "'N',"		// ShowQstResults: Don't show question results initially
				         "'N')",	// ShowUsrResults: Don't show user results initially
				         GamCod,
				         Gbl.Usrs.Me.UsrDat.UsrCod,	// Game creator
				         Title,
					 Mch_ShowingStringsDB[Mch_SHOWING_DEFAULT],
					 Mch_NUM_COLS_DEFAULT);

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
   long QstCod;
   long LongNum;
   unsigned QstInd;
   Tst_AnswerType_t AnswerType;
   bool Shuffle;

   /***** Get questions of the game *****/
   NumQsts = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get questions of a game",
			     "SELECT gam_questions.QstCod,"	// row[0]
			            "gam_questions.QstInd,"	// row[1]
			            "tst_questions.AnsType,"	// row[2]
			            "tst_questions.Shuffle"	// row[3]
			     " FROM gam_questions,tst_questions"
			     " WHERE gam_questions.GamCod=%ld"
			     " AND gam_questions.QstCod=tst_questions.QstCod"
			     " ORDER BY gam_questions.QstInd",
			     GamCod);

   /***** For each question in game... *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      /***** Get question data *****/
      row = mysql_fetch_row (mysql_res);

      /* Get question code (row[0]) */
      if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	 Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Get question index (row[1]) */
      if ((LongNum = Str_ConvertStrCodToLongCod (row[1])) < 0)
	 Lay_ShowErrorAndExit ("Wrong question index.");
      QstInd = (unsigned) LongNum;

      /* Get answer type (row[2]) */
      AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
      if (AnswerType != Tst_ANS_UNIQUE_CHOICE)
	 Lay_ShowErrorAndExit ("Wrong answer type.");

      /* Get shuffle (row[3]) */
      Shuffle = (row[3][0] == 'Y');

      /***** Reorder answer *****/
      Mch_ReorderAnswer (MchCod,QstInd,QstCod,Shuffle);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Reorder answers of a match question *********************/
/*****************************************************************************/

static void Mch_ReorderAnswer (long MchCod,unsigned QstInd,
			       long QstCod,bool Shuffle)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumAnss;
   unsigned NumAns;
   long LongNum;
   unsigned AnsInd;
   char StrOneAnswer[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   char StrAnswersOneQst[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1];

   /***** Initialize list of answers to empty string *****/
   StrAnswersOneQst[0] = '\0';

   /***** Get questions of the game *****/
   NumAnss = (unsigned)
	     DB_QuerySELECT (&mysql_res,"can not get questions of a game",
			     "SELECT AnsInd"	// row[0]
			     " FROM tst_answers"
			     " WHERE QstCod=%ld"
			     " ORDER BY %s",
			     QstCod,
			     Shuffle ? "RAND()" :	// Use RAND() because is really random; RAND(NOW()) repeats order
				       "AnsInd");

   /***** For each answer in question... *****/
   for (NumAns = 0;
	NumAns < NumAnss;
	NumAns++)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get answer index (row[0]) */
      if ((LongNum = Str_ConvertStrCodToLongCod (row[0])) < 0)
	 Lay_ShowErrorAndExit ("Wrong answer index.");
      AnsInd = (unsigned) LongNum;
      snprintf (StrOneAnswer,sizeof (StrOneAnswer),
		"%u",AnsInd);

      /* Concatenate answer index to list of answers */
      if (NumAns)
         Str_Concat (StrAnswersOneQst,",",
		     Tst_MAX_BYTES_ANSWERS_ONE_QST);
      Str_Concat (StrAnswersOneQst,StrOneAnswer,
		  Tst_MAX_BYTES_ANSWERS_ONE_QST);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Create entry for this question in table of match indexes *****/
   DB_QueryINSERT ("can not create match indexes",
		   "INSERT INTO mch_indexes"
		   " (MchCod,QstInd,Indexes)"
		   " VALUES"
		   " (%ld,%u,'%s')",
		   MchCod,QstInd,StrAnswersOneQst);
  }

/*****************************************************************************/
/***************** Get indexes for a question from database ******************/
/*****************************************************************************/

void Mch_GetIndexes (long MchCod,unsigned QstInd,
		     unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char StrIndexesOneQst[Tst_MAX_BYTES_INDEXES_ONE_QST + 1];

   /***** Get indexes for a question from database *****/
   if (!DB_QuerySELECT (&mysql_res,"can not get data of a question",
			"SELECT Indexes"	// row[0]
			" FROM mch_indexes"
			" WHERE MchCod=%ld AND QstInd=%u",
			MchCod,QstInd))
      Lay_ShowErrorAndExit ("No indexes found for a question.");
   row = mysql_fetch_row (mysql_res);

   /* Get indexes (row[0]) */
   Str_Copy (StrIndexesOneQst,row[0],
	     Tst_MAX_BYTES_INDEXES_ONE_QST);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Get indexes from string *****/
   Par_ReplaceCommaBySeparatorMultiple (StrIndexesOneQst);
   Tst_GetIndexesFromStr (StrIndexesOneQst,Indexes);
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
/********************* Remove one group from all matches *********************/
/*****************************************************************************/

void Mch_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the matches *****/
   DB_QueryDELETE ("can not remove group"
	           " from the associations between matches and groups",
		   "DELETE FROM mch_groups WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/***************** Remove groups of one type from all matches ****************/
/*****************************************************************************/

void Mch_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the matches *****/
   DB_QueryDELETE ("can not remove groups of a type"
	           " from the associations between matches and groups",
		   "DELETE FROM mch_groups"
		   " USING crs_grp,mch_groups"
		   " WHERE crs_grp.GrpTypCod=%ld"
		   " AND crs_grp.GrpCod=mch_groups.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/***************** Insert/update a game match being played *******************/
/*****************************************************************************/

#define Mch_MAX_BYTES_SUBQUERY 128

static void Mch_UpdateMatchStatusInDB (const struct Match *Match)
  {
   char MchSubQuery[Mch_MAX_BYTES_SUBQUERY];

   /***** Update end time only if match is currently being played *****/
   if (Match->Status.Playing)	// Match is being played
      Str_Copy (MchSubQuery,"mch_matches.EndTime=NOW(),",
		Mch_MAX_BYTES_SUBQUERY);
   else				// Match is paused, not being played
      MchSubQuery[0] = '\0';

   /***** Update match status in database *****/
   DB_QueryUPDATE ("can not update match being played",
		   "UPDATE mch_matches,gam_games"
		   " SET %s"
			"mch_matches.QstInd=%u,"
			"mch_matches.QstCod=%ld,"
			"mch_matches.Showing='%s',"
		        "mch_matches.NumCols=%u,"
			"mch_matches.ShowQstResults='%c'"
		   " WHERE mch_matches.MchCod=%ld"
		   " AND mch_matches.GamCod=gam_games.GamCod"
		   " AND gam_games.CrsCod=%ld",	// Extra check
		   MchSubQuery,
		   Match->Status.QstInd,Match->Status.QstCod,
		   Mch_ShowingStringsDB[Match->Status.Showing],
		   Match->Status.NumCols,
		   Match->Status.ShowQstResults ? 'Y' : 'N',
		   Match->MchCod,Gbl.Hierarchy.Crs.CrsCod);

   if (Match->Status.Playing)	// Match is being played
      /* Update match as being played */
      Mch_UpdateMatchAsBeingPlayed (Match->MchCod);
   else				// Match is paused, not being played
      /* Update match as not being played */
      Mch_SetMatchAsNotBeingPlayed (Match->MchCod);
  }

/*****************************************************************************/
/********** Update elapsed time in current question (by a teacher) ***********/
/*****************************************************************************/

static void Mch_UpdateElapsedTimeInQuestion (const struct Match *Match)
  {
   /***** Update elapsed time in current question in database *****/
   if (Match->Status.Playing &&		// Match is being played
       Match->Status.Showing != Mch_START &&
       Match->Status.Showing != Mch_END)
      DB_QueryINSERT ("can not update elapsed time in question",
		      "INSERT INTO mch_times (MchCod,QstInd,ElapsedTime)"
		      " VALUES (%ld,%u,SEC_TO_TIME(%u))"
		      " ON DUPLICATE KEY"
		      " UPDATE ElapsedTime=ADDTIME(ElapsedTime,SEC_TO_TIME(%u))",
		      Match->MchCod,Match->Status.QstInd,
		      Cfg_SECONDS_TO_REFRESH_MATCH_TCH,
		      Cfg_SECONDS_TO_REFRESH_MATCH_TCH);
  }

/*****************************************************************************/
/******************* Get elapsed time in a match question ********************/
/*****************************************************************************/

static void Mch_GetElapsedTimeInQuestion (const struct Match *Match,
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

static void Mch_GetElapsedTimeInMatch (const struct Match *Match,
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
/********************** Play/pause match (by a teacher) **********************/
/*****************************************************************************/

void Mch_PlayPauseMatch (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

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
   struct Match Match;

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Get number of columns *****/
   Match.Status.NumCols = (unsigned)
	                  Par_GetParToUnsignedLong ("NumCols",
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

void Mch_ToggleVisibilResultsMchQst (void)
  {
   struct Match Match;

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

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
   struct Match Match;

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

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
   struct Match Match;

   /***** Remove old players.
          This function must be called by a teacher
          before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

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

static void Mch_SetMatchStatusToPrev (struct Match *Match)
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
  }

static void Mch_SetMatchStatusToPrevQst (struct Match *Match)
  {
   /***** Get index of the previous question *****/
   Match->Status.QstInd = Gam_GetPrevQuestionIndexInGame (Match->GamCod,
							  Match->Status.QstInd);
   if (Match->Status.QstInd)		// Start of questions not reached
     {
      Match->Status.QstCod = Gam_GetQstCodFromQstInd (Match->GamCod,
						      Match->Status.QstInd);
      Match->Status.Showing = Match->Status.ShowQstResults ? Mch_RESULTS :
							     Mch_ANSWERS;
     }
   else					// Start of questions reached
      Mch_SetMatchStatusToStart (Match);
  }

static void Mch_SetMatchStatusToStart (struct Match *Match)
  {
   Match->Status.QstInd  = 0;				// Before first question
   Match->Status.QstCod  = -1L;
   Match->Status.Playing = false;
   Match->Status.Showing = Mch_START;
  }

/*****************************************************************************/
/**************** Set match status to next (forward) status ******************/
/*****************************************************************************/

static void Mch_SetMatchStatusToNext (struct Match *Match)
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
  }

static void Mch_SetMatchStatusToNextQst (struct Match *Match)
  {
   /***** Get index of the next question *****/
   Match->Status.QstInd = Gam_GetNextQuestionIndexInGame (Match->GamCod,
							  Match->Status.QstInd);

   /***** Get question code *****/
   if (Match->Status.QstInd < Mch_AFTER_LAST_QUESTION)	// End of questions not reached
     {
      Match->Status.QstCod = Gam_GetQstCodFromQstInd (Match->GamCod,
						      Match->Status.QstInd);
      Match->Status.Showing = Mch_STEM;
     }
   else							// End of questions reached
      Mch_SetMatchStatusToEnd (Match);
  }

static void Mch_SetMatchStatusToEnd (struct Match *Match)
  {
   Match->Status.QstInd  = Mch_AFTER_LAST_QUESTION;	// After last question
   Match->Status.QstCod  = -1L;
   Match->Status.Playing = false;
   Match->Status.Showing = Mch_END;
  }

/*****************************************************************************/
/******* Show current match status (number, question, answers, button) *******/
/*****************************************************************************/

static void Mch_ShowMatchStatusForTch (struct Match *Match)
  {
   /***** Left column *****/
   Mch_ShowLeftColumnTch (Match);

   /***** Right column *****/
   Mch_ShowRightColumnTch (Match);
  }

/*****************************************************************************/
/************ Show current question being played for a student ***************/
/*****************************************************************************/

static void Mch_ShowMatchStatusForStd (struct Match *Match,Mch_Update_t Update)
  {
   bool ICanPlayThisMatchBasedOnGrps;
   struct Mch_UsrAnswer UsrAnswer;

   /***** Can I play this match? *****/
   ICanPlayThisMatchBasedOnGrps = Mch_CheckIfICanPlayThisMatchBasedOnGrps (Match);
   if (!ICanPlayThisMatchBasedOnGrps)
      Lay_NoPermissionExit ();

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
/********************** Get number of matches in a game **********************/
/*****************************************************************************/

unsigned Mch_GetNumMchsInGame (long GamCod)
  {
   /***** Trivial check *****/
   if (GamCod < 0)	// A non-existing game...
      return 0;		// ...has no matches

   /***** Get number of matches in a game from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of matches of a game",
			     "SELECT COUNT(*) FROM mch_matches"
			     " WHERE GamCod=%ld",
			     GamCod);
  }

/*****************************************************************************/
/*************** Get number of unfinished matches in a game ******************/
/*****************************************************************************/

unsigned Mch_GetNumUnfinishedMchsInGame (long GamCod)
  {
   /***** Trivial check *****/
   if (GamCod < 0)	// A non-existing game...
      return 0;		// ...has no matches

   /***** Get number of matches in a game from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of unfinished matches of a game",
			     "SELECT COUNT(*) FROM mch_matches"
			     " WHERE GamCod=%ld AND Showing<>'%s'",
			     GamCod,Mch_ShowingStringsDB[Mch_END]);
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a match **************/
/*****************************************************************************/

bool Mch_CheckIfICanPlayThisMatchBasedOnGrps (const struct Match *Match)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 /***** Check if I belong to any of the groups
	        associated to the match *****/
	 return (DB_QueryCOUNT ("can not check if I can play a match",
				"SELECT COUNT(*) FROM mch_matches"
				" WHERE MchCod=%ld"
				" AND"
				"(MchCod NOT IN"
				" (SELECT MchCod FROM mch_groups)"
				" OR"
				" MchCod IN"
				" (SELECT mch_groups.MchCod"
				" FROM mch_groups,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld"
				" AND mch_groups.GrpCod=crs_grp_usr.GrpCod))",
				Match->MchCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
	 break;
      case Rol_NET:
	 /***** Only if I am the creator *****/
	 return (Match->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/*********** Show left column when playing a match (as a teacher) ************/
/*****************************************************************************/

static void Mch_ShowLeftColumnTch (struct Match *Match)
  {
   /***** Start left container *****/
   HTM_DIV_Begin ("class=\"MCH_LEFT\"");

   /***** Refreshable part *****/
   HTM_DIV_Begin ("id=\"match_left\" class=\"MCH_REFRESHABLE_TEACHER\"");
   Mch_ShowRefreshablePartTch (Match);
   HTM_DIV_End ();

   /***** Buttons *****/
   Mch_PutMatchControlButtons (Match);

   /***** Put forms to choice which projects to show *****/
   Set_StartSettingsHead ();
   Mch_ShowFormColumns (Match);
   Set_EndSettingsHead ();

   /***** Write button to request viewing results *****/
   Mch_PutCheckboxResult (Match);

   /***** End left container *****/
   HTM_DIV_End ();
  }

static void Mch_ShowRefreshablePartTch (struct Match *Match)
  {
   /***** Write elapsed time in match *****/
   Mch_WriteElapsedTimeInMch (Match);

   /***** Write number of question *****/
   Mch_ShowNumQstInMch (Match);

   /***** Write elapsed time in question *****/
   Mch_WriteElapsedTimeInQst (Match);

   /***** Number of users who have responded this question *****/
   Mch_WriteNumRespondersQst (Match);
  }

static void Mch_WriteElapsedTimeInMch (struct Match *Match)
  {
   struct Time Time;

   HTM_DIV_Begin ("class=\"MCH_TOP\"");

   /***** Get elapsed time in match *****/
   Mch_GetElapsedTimeInMatch (Match,&Time);

   /***** Write elapsed time in hh:mm'ss" format *****/
   Dat_WriteHoursMinutesSeconds (&Time);

   HTM_DIV_End ();
  }

static void Mch_WriteElapsedTimeInQst (struct Match *Match)
  {
   struct Time Time;

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

static void Mch_WriteNumRespondersQst (struct Match *Match)
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
	 HTM_Unsigned (Mch_GetNumUsrsWhoAnsweredQst (Match->MchCod,
					             Match->Status.QstInd));
	 break;
     }

   /***** Write number of players *****/
   if (Match->Status.Playing)	// Match is being played
     {
      /* Get current number of players */
      Mch_GetNumPlayers (Match);

      /* Show current number of players */
      HTM_TxtF ("/%u",Match->Status.NumPlayers);
     }

   /***** End block *****/
   HTM_STRONG_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********** Show right column when playing a match (as a teacher) ************/
/*****************************************************************************/

static void Mch_ShowRightColumnTch (const struct Match *Match)
  {
   /***** Start right container *****/
   HTM_DIV_Begin ("class=\"MCH_RIGHT\"");

   /***** Top row: match title *****/
   Mch_ShowMatchTitle (Match);

   /***** Bottom row: current question and possible answers *****/
   if (Match->Status.Showing == Mch_END)	// Match over
      Mch_ShowMatchScore (Match);
   else						// Match not over
      Mch_ShowQuestionAndAnswersTch (Match);

   /***** End right container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********** Show left column when playing a match (as a student) ************/
/*****************************************************************************/

static void Mch_ShowLeftColumnStd (const struct Match *Match,
				   const struct Mch_UsrAnswer *UsrAnswer)
  {
   bool Answered = UsrAnswer->NumOpt >= 0;

   /***** Start left container *****/
   HTM_DIV_Begin ("class=\"MCH_LEFT\"");

   /***** Top *****/
   HTM_DIV_Begin ("class=\"MCH_TOP\"");
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

	 if (Match->Status.Playing &&			// Match is being played
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

static void Mch_ShowRightColumnStd (struct Match *Match,
				    const struct Mch_UsrAnswer *UsrAnswer,
				    Mch_Update_t Update)
  {
   extern const char *Txt_Please_wait_;

   /***** Start right container *****/
   HTM_DIV_Begin ("class=\"MCH_RIGHT\"");

   /***** Top row *****/
   Mch_ShowMatchTitle (Match);

   /***** Bottom row *****/
   if (Match->Status.Playing)			// Match is being played
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
	       if (!Mch_ShowQuestionAndAnswersStd (Match,UsrAnswer,Update))
                  Ale_ShowAlert (Ale_ERROR,"Wrong question.");
	   }
	 else
	    Ale_ShowAlert (Ale_ERROR,"You can not join this match.");

	 HTM_DIV_End ();
	}
     }
   else						// Match is not being played
      Mch_ShowWaitImage (Txt_Please_wait_);

   /***** End right container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show number of question in game ***********************/
/*****************************************************************************/

static void Mch_ShowNumQstInMch (const struct Match *Match)
  {
   extern const char *Txt_MATCH_Start;
   extern const char *Txt_MATCH_End;
   unsigned NumQsts = Gam_GetNumQstsGame (Match->GamCod);

   HTM_DIV_Begin ("class=\"MCH_NUM_QST\"");
   switch (Match->Status.Showing)
     {
      case Mch_START:	// Not started
         HTM_Txt (Txt_MATCH_Start);
         break;
      case Mch_END:	// Match over
         HTM_Txt (Txt_MATCH_End);
         break;
      default:
         HTM_TxtF ("%u/%u",Match->Status.QstInd,NumQsts);
         break;
     }
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Put buttons to control a match ***********************/
/*****************************************************************************/

static void Mch_PutMatchControlButtons (const struct Match *Match)
  {
   extern const char *Txt_Go_back;
   extern const char *Txt_Go_forward;
   extern const char *Txt_Pause;
   extern const char *Txt_Start;
   extern const char *Txt_Resume;

   /***** Start buttons container *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTONS_CONTAINER\"");

   /***** Left button *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTON_LEFT_CONTAINER\"");
   switch (Match->Status.Showing)
     {
      case Mch_START:
	 /* Put button to close browser tab */
	 Mch_PutBigButtonClose ();
	 break;
      default:
	 /* Put button to go back */
	 Mch_PutBigButton (ActBckMch,"backward",Match->MchCod,
			   Mch_ICON_PREVIOUS,Txt_Go_back);
	 break;
     }
   HTM_DIV_End ();

   /***** Center button *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTON_CENTER_CONTAINER\"");
   if (Match->Status.Playing)					// Match is being played
      /* Put button to pause match */
      Mch_PutBigButton (ActPlyPauMch,"play_pause",Match->MchCod,
			Mch_ICON_PAUSE,Txt_Pause);
   else								// Match is paused, not being played
     {
      switch (Match->Status.Showing)
        {
	 case Mch_START:		// Match just started, before first question
	    /* Put button to start playing match */
	    Mch_PutBigButton (ActPlyPauMch,"play_pause",Match->MchCod,
			      Mch_ICON_PLAY,Txt_Start);
	    break;
	 case Mch_END:			// Match over
	    /* Put disabled button to play match */
	    Mch_PutBigButtonOff (Mch_ICON_PLAY);
	    break;
	 default:
	    /* Put button to resume match */
	    Mch_PutBigButton (ActPlyPauMch,"play_pause",Match->MchCod,
			      Mch_ICON_PLAY,Txt_Resume);
        }
     }
   HTM_DIV_End ();

   /***** Right button *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTON_RIGHT_CONTAINER\"");
   if (Match->Status.Showing == Mch_END)	// Match over
      /* Put button to close browser tab */
      Mch_PutBigButtonClose ();
   else						// Match not over
      /* Put button to show answers */
      Mch_PutBigButton (ActFwdMch,"forward",Match->MchCod,
			Mch_ICON_NEXT,Txt_Go_forward);
   HTM_DIV_End ();

   /***** End buttons container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/** Show form to choice whether to show answers in one column or two columns */
/*****************************************************************************/

static void Mch_ShowFormColumns (const struct Match *Match)
  {
   extern const char *Txt_column;
   extern const char *Txt_columns;
   unsigned NumCols;
   static const char *NumColsIcon[1 + Mch_MAX_COLS] =
     {
      "",		// Not used
      "1col.png",	// 1 column
      "2col.png",	// 2 columns
      "3col.png",	// 3 columns
      "4col.png",	// 4 columns
     };

   /***** Begin selector *****/
   Set_StartOneSettingSelector ();

   for (NumCols  = 1;
	NumCols <= Mch_MAX_COLS;
	NumCols++)
     {
      HTM_DIV_Begin ("class=\"%s\"",
		     (Match->Status.NumCols == NumCols) ? "PREF_ON" :
							  "PREF_OFF");

      /***** Begin form *****/
      Frm_StartForm (ActChgNumColMch);
      Mch_PutParamMchCod (Match->MchCod);	// Current match being played
      Mch_PutParamNumCols (NumCols);		// Number of columns

      snprintf (Gbl.Title,sizeof (Gbl.Title),
		"%u %s",
		NumCols,NumCols == 1 ? Txt_column :
		                       Txt_columns);
      Ico_PutSettingIconLink (NumColsIcon[NumCols],Gbl.Title);

      /***** End form *****/
      Frm_EndForm ();

      HTM_DIV_End ();
     }

   /***** End selector *****/
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/******** Write parameter with number of columns in answers of match *********/
/*****************************************************************************/

static void Mch_PutParamNumCols (unsigned NumCols)	// Number of columns
  {
   Par_PutHiddenParamUnsigned (NULL,"NumCols",NumCols);
  }

/*****************************************************************************/
/***************** Put checkbox to select if show results ********************/
/*****************************************************************************/

static void Mch_PutCheckboxResult (const struct Match *Match)
  {
   extern const char *Txt_View_results;

   /***** Start container *****/
   HTM_DIV_Begin ("class=\"MCH_SHOW_RESULTS\"");

   /***** Start form *****/
   Frm_StartForm (ActChgVisResMchQst);
   Mch_PutParamMchCod (Match->MchCod);	// Current match being played

   /***** Put icon with link *****/
   HTM_BUTTON_SUBMIT_Begin (Txt_View_results,"BT_LINK DAT ICO_HIGHLIGHT",NULL);
   HTM_TxtF ("<i class=\"%s\"></i>",
	     Match->Status.ShowQstResults ? "fas fa-toggle-on" :
		                            "fas fa-toggle-off");
   HTM_TxtF ("&nbsp;%s",Txt_View_results);
   HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Put checkbox to select if show results ********************/
/*****************************************************************************/

static void Mch_PutIfAnswered (const struct Match *Match,bool Answered)
  {
   extern const char *Txt_View_my_answer;
   extern const char *Txt_MATCH_QUESTION_Answered;
   extern const char *Txt_MATCH_QUESTION_Unanswered;

   /***** Start container *****/
   HTM_DIV_Begin ("class=\"MCH_SHOW_RESULTS\"");

   /***** Put icon with link *****/
   if (Match->Status.Playing &&			// Match is being played
       Match->Status.Showing == Mch_ANSWERS &&	// Teacher's screen is showing question answers
       Answered)				// I have answered this question
     {
      /* Start form */
      Frm_StartForm (ActSeeMchAnsQstStd);
      Mch_PutParamMchCod (Match->MchCod);	// Current match being played

      HTM_BUTTON_OnMouseDown_Begin (Txt_View_my_answer,"BT_LINK DAT_SMALL_GREEN");
      HTM_TxtF ("<i class=\"%s\"></i>","fas fa-check-circle");
      HTM_TxtF ("&nbsp;%s",Txt_MATCH_QUESTION_Answered);
      HTM_BUTTON_End ();

      /* End form */
      Frm_EndForm ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"%s\"",Answered ? "DAT_SMALL_GREEN" :
	                                       "DAT_SMALL_RED");
      HTM_TxtF ("<i class=\"%s\" title=\"%s\"></i>",
		Answered ? "fas fa-check-circle" :
		           "fas fa-exclamation-circle",
		Answered ? Txt_MATCH_QUESTION_Answered :
		           Txt_MATCH_QUESTION_Unanswered);
      HTM_TxtF ("&nbsp;%s",Answered ? Txt_MATCH_QUESTION_Answered :
		                      Txt_MATCH_QUESTION_Unanswered);
      HTM_DIV_End ();
     }

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***************** Put checkbox to select if show results ********************/
/*****************************************************************************/

static void Mch_PutIconToRemoveMyAnswer (const struct Match *Match)
  {
   extern const char *Txt_Delete_my_answer;

   /***** Start container *****/
   HTM_DIV_Begin ("class=\"MCH_SHOW_RESULTS\"");

   /***** Start form *****/
   Frm_StartForm (ActRemMchAnsQstStd);
   Mch_PutParamMchCod (Match->MchCod);		// Current match being played
   Gam_PutParamQstInd (Match->Status.QstInd);	// Current question index shown

   /***** Put icon with link *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTON_CONTAINER\"");
   HTM_BUTTON_OnMouseDown_Begin (Txt_Delete_my_answer,"BT_LINK MCH_BUTTON_ON ICO_RED");
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

static void Mch_ShowMatchTitle (const struct Match *Match)
  {
   /***** Match title *****/
   HTM_DIV_Begin ("class=\"MCH_TOP\"");
   HTM_Txt (Match->Title);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a teacher) *****/
/*****************************************************************************/

static void Mch_ShowQuestionAndAnswersTch (const struct Match *Match)
  {
   extern const char *Txt_MATCH_Paused;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

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
   if (Gbl.Test.AnswerType != Tst_ANS_UNIQUE_CHOICE)
      Lay_ShowErrorAndExit ("Wrong answer type.");

   HTM_DIV_Begin ("class=\"MCH_BOTTOM\"");	// Bottom

   /* Write stem (row[1]) */
   Tst_WriteQstStem (row[1],"MCH_TCH_STEM");

   /* Get media (row[2]) */
   Gbl.Test.Media.MedCod = Str_ConvertStrCodToLongCod (row[2]);
   Med_GetMediaDataByCod (&Gbl.Test.Media);

   /* Show media */
   Med_ShowMedia (&Gbl.Test.Media,
		  "TEST_MED_EDIT_LIST_STEM_CONTAINER",
		  "TEST_MED_EDIT_LIST_STEM");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Write answers? *****/
   switch (Match->Status.Showing)
     {
      case Mch_ANSWERS:
	 if (Match->Status.Playing)			// Match is being played
	    /* Write answers */
	    Mch_WriteAnswersMatchResult (Match,
					 "MCH_TCH_ANS",
					 false);	// Don't show result
	 else						// Match is paused, not being played
	    Mch_ShowWaitImage (Txt_MATCH_Paused);
	 break;
      case Mch_RESULTS:
	 /* Write answers with results */
	 Mch_WriteAnswersMatchResult (Match,
				      "MCH_TCH_ANS",
				      true);		// Show result
	 break;
      default:
	 /* Don't write anything */
	 break;
     }

   HTM_DIV_End ();				// Bottom
  }

/*****************************************************************************/
/************* Write answers of a question when viewing a match **************/
/*****************************************************************************/

static void Mch_WriteAnswersMatchResult (const struct Match *Match,
                                         const char *Class,bool ShowResult)
  {
   /***** Write answer depending on type *****/
   if (Gbl.Test.AnswerType == Tst_ANS_UNIQUE_CHOICE)
      Tst_WriteChoiceAnsViewMatch (Match->MchCod,Match->Status.QstInd,Match->Status.QstCod,
				   Match->Status.NumCols,Class,ShowResult);
   else
      Ale_ShowAlert (Ale_ERROR,"Type of answer not valid in a game.");
  }

/*****************************************************************************/
/***** Show question and its answers when playing a match (as a student) *****/
/*****************************************************************************/
// Return true on valid question, false on invalid question

static bool Mch_ShowQuestionAndAnswersStd (const struct Match *Match,
					   const struct Mch_UsrAnswer *UsrAnswer,
					   Mch_Update_t Update)
  {
   unsigned NumOptions;
   unsigned NumOpt;
   char *Class;

   /***** Trivial check: this question must be valid for games *****/
   if (!Tst_CheckIfQuestionIsValidForGame (Match->Status.QstCod))
      return false;

   /***** Get number of options in this question *****/
   NumOptions = Tst_GetNumAnswersQst (Match->Status.QstCod);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (8);

   for (NumOpt = 0;
	NumOpt < NumOptions;
	NumOpt++)
     {
      /***** Start row *****/
      HTM_TR_Begin (NULL);

      /***** Write letter for this option *****/
      /* Begin table cell */
      HTM_TD_Begin ("class=\"MCH_STD_CELL\"");

      /* Form with button.
	 Sumitting onmousedown instead of default onclick
	 is necessary in order to be fast
	 and not lose clicks due to refresh */
      Frm_StartForm (ActAnsMchQstStd);
      Mch_PutParamMchCod (Match->MchCod);		// Current match being played
      Gam_PutParamQstInd (Match->Status.QstInd);	// Current question index shown
      Mch_PutParamNumOpt (NumOpt);		// Number of button

      if (asprintf (&Class,"MCH_STD_BUTTON%s BT_%c",
		    UsrAnswer->NumOpt == (int) NumOpt &&	// Student's answer
		    Update == Mch_CHANGE_STATUS_BY_STUDENT ? " MCH_STD_ANSWER_SELECTED" :
							     "",
		    'A' + (char) NumOpt) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_BUTTON_OnMouseDown_Begin (NULL,Class);
      HTM_TxtF ("%c",'a' + (char) NumOpt);
      HTM_BUTTON_End ();
      free (Class);

      Frm_EndForm ();

      /* End table cell */
      HTM_TD_End ();

      /***** End row *****/
      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   return true;
  }

/*****************************************************************************/
/***************************** Show match scores *****************************/
/*****************************************************************************/

#define Mch_NUM_ROWS_SCORE 50

static void Mch_ShowMatchScore (const struct Match *Match)
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
   unsigned MaxUsrs = 0;
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
   if (DB_QuerySELECT (&mysql_res,"can not get max users",
		       "SELECT MAX(NumUsrs)"
		       " FROM "
		       "(SELECT COUNT(*) AS NumUsrs"	// row[1]
		       " FROM mch_results"
		       " WHERE MchCod=%ld"
		       " GROUP BY Score"
		       " ORDER BY Score) AS Scores",
		       Match->MchCod))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get maximum number of users (row[0]) *****/
      if (row)
         if (row[0])
	    if (sscanf (row[0],"%u",&MaxUsrs) != 1)
	       MaxUsrs = 0;
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Get scores from database *****/
   NumScores = (unsigned)
	       DB_QuerySELECT (&mysql_res,"can not get scores",
			       "SELECT Score,"			// row[0]
				      "COUNT(*) AS NumUsrs"	// row[1]
			       " FROM mch_results"
			       " WHERE MchCod=%ld"
			       " GROUP BY Score"
			       " ORDER BY Score DESC",
			       Match->MchCod);

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
         Mch_DrawEmptyRowScore (NumRow,MinScore,MaxScore);

      /***** Draw row for this score *****/
      Mch_DrawScoreRow (Score,MinScore,MaxScore,NumRow,NumUsrs,MaxUsrs);
      NumRow++;
     }

   /***** Draw final empty rows *****/
   for (;
	NumRow < Mch_NUM_ROWS_SCORE;
	NumRow++)
      Mch_DrawEmptyRowScore (NumRow,MinScore,MaxScore);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void Mch_DrawEmptyRowScore (unsigned NumRow,double MinScore,double MaxScore)
  {
   /***** Draw row *****/
   HTM_TR_Begin (NULL);

   /* Write score */
   HTM_TD_Begin ("class=\"MCH_SCO_SCO\"");
   if (NumRow == 0)
     {
      Str_WriteDoubleNumToFile (Gbl.F.Out,MaxScore);
      HTM_NBSP ();
     }
   else if (NumRow == Mch_NUM_ROWS_SCORE - 1)
     {
      Str_WriteDoubleNumToFile (Gbl.F.Out,MinScore);
      HTM_NBSP ();
     }
   HTM_TD_End ();

   /* Empty column with bar and number of users */
   HTM_TD_Begin ("class=\"MCH_SCO_NUM%s\"",Mch_GetClassBorder (NumRow));
   HTM_TD_End ();

   HTM_TR_End ();
  }

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
   Str_WriteDoubleNumToFile (Gbl.F.Out,Score);
   HTM_NBSP ();
   HTM_TD_End ();

   /* Draw bar and write number of users for this score */
   HTM_TD_Begin ("class=\"MCH_SCO_NUM%s\"",Mch_GetClassBorder (NumRow));
   if (asprintf (&Icon,"score%u_1x1.png",Color) < 0)	// Background
      Lay_NotEnoughMemoryExit ();
   if (asprintf (&Title,"%u %s",
		 NumUsrs,
		 NumUsrs == 1 ? Txt_ROLES_SINGUL_abc[Rol_STD][Usr_SEX_UNKNOWN] :
		                Txt_ROLES_PLURAL_abc[Rol_STD][Usr_SEX_UNKNOWN]) < 0)
      Lay_NotEnoughMemoryExit ();
   HTM_IMG (Cfg_URL_ICON_PUBLIC,Icon,Title,
	    "class=\"MCH_SCO_BAR\" style=\"width:%u%%;\"",BarWidth);
   HTM_TxtF ("&nbsp;%u",NumUsrs);
   free (Title);
   free (Icon);
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

static void Mch_PutParamNumOpt (unsigned NumOpt)
  {
   Par_PutHiddenParamUnsigned (NULL,"NumOpt",NumOpt);
  }

/*****************************************************************************/
/******* Get parameter with number of option (button) pressed by user ********/
/*****************************************************************************/

static unsigned Mch_GetParamNumOpt (void)
  {
   long NumOpt;

   NumOpt = Par_GetParToLong ("NumOpt");
   if (NumOpt < 0)
      Lay_ShowErrorAndExit ("Wrong number of option.");

   return (unsigned) NumOpt;
  }

/*****************************************************************************/
/*********************** Put a big button to do action ***********************/
/*****************************************************************************/

static void Mch_PutBigButton (Act_Action_t NextAction,const char *Id,
			      long MchCod,const char *Icon,const char *Txt)
  {
   /***** Begin form *****/
   Frm_StartFormId (NextAction,Id);
   Mch_PutParamMchCod (MchCod);

   /***** Put icon with link *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTON_CONTAINER\"");
   HTM_BUTTON_SUBMIT_Begin (Txt,"BT_LINK MCH_BUTTON_ON ICO_BLACK",NULL);
   HTM_TxtF ("<i class=\"%s\"></i>",Icon);
   HTM_BUTTON_End ();
   HTM_DIV_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

static void Mch_PutBigButtonOff (const char *Icon)
  {
   /***** Put inactive icon *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTON_CONTAINER\"");
   HTM_BUTTON_BUTTON_Begin (NULL,"BT_LINK_OFF MCH_BUTTON_OFF ICO_BLACK",NULL);
   HTM_TxtF ("<i class=\"%s\"></i>",Icon);
   HTM_BUTTON_End ();
   HTM_DIV_End ();
  }

static void Mch_PutBigButtonClose (void)
  {
   extern const char *Txt_Close;

   /***** Put icon with link *****/
   HTM_DIV_Begin ("class=\"MCH_BUTTON_CONTAINER\"");
   HTM_BUTTON_BUTTON_Begin (Txt_Close,"BT_LINK MCH_BUTTON_ON ICO_BLACK","window.close();");
   HTM_TxtF ("<i class=\"%s\"></i>",Mch_ICON_CLOSE);
   HTM_BUTTON_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************************** Show wait image ******************************/
/*****************************************************************************/

static void Mch_ShowWaitImage (const char *Txt)
  {
   HTM_DIV_Begin ("class=\"MCH_WAIT_CONTAINER\"");
   Ico_PutIcon ("wait.gif",Txt,"MCH_WAIT_IMG");
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Remove old players *****************************/
/*****************************************************************************/

static void Mch_RemoveOldPlayers (void)
  {
   /***** Delete matches not being played by teacher *****/
   DB_QueryDELETE ("can not update matches as not being played",
		   "DELETE FROM mch_playing"
		   " WHERE TS<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Cfg_SECONDS_TO_REFRESH_MATCH_TCH*3);

   /***** Delete players (students) who have left matches *****/
   DB_QueryDELETE ("can not update match players",
		   "DELETE FROM mch_players"
		   " WHERE TS<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Cfg_SECONDS_TO_REFRESH_MATCH_STD*3);
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
/******************* Register me as a player in a match **********************/
/*****************************************************************************/
// Return true on success

bool Mch_RegisterMeAsPlayerInMatch (struct Match *Match)
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
   DB_QueryREPLACE ("can not insert match player",
		    "REPLACE mch_players (MchCod,UsrCod) VALUES (%ld,%ld)",
		    Match->MchCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return true;
  }

/*****************************************************************************/
/********************** Get code of match being played ***********************/
/*****************************************************************************/

void Mch_GetMatchBeingPlayed (void)
  {
   /***** Get match code ****/
   if ((Gbl.Games.MchCodBeingPlayed = Mch_GetParamMchCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of match is missing.");
  }

/*****************************************************************************/
/********************* Show match being played as student ********************/
/*****************************************************************************/

void Mch_JoinMatchAsStd (void)
  {
   struct Match Match;

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
   Mch_ShowMatchStatusForStd (&Match,Mch_CHANGE_STATUS_BY_STUDENT);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****** Remove student's answer to a question and show match as student ******/
/*****************************************************************************/

void Mch_RemoveMyQuestionAnswer (void)
  {
   struct Match Match;
   unsigned QstInd;

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Get question index from form *****/
   QstInd = Gam_GetParamQstInd ();

   /***** Check that teacher's screen is showing answers
          and question index is the current one being played *****/
   if (Match.Status.Playing &&			// Match is being played
       Match.Status.Showing == Mch_ANSWERS &&	// Teacher's screen is showing answers
       QstInd == Match.Status.QstInd)		// Removing answer to the current question being played
      /***** Remove answer to this question *****/
      Mch_RemoveMyAnswerToMatchQuestion (&Match);

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
   Mch_ShowMatchStatusForStd (&Match,Mch_CHANGE_STATUS_BY_STUDENT);
   HTM_DIV_End ();
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
          This function must be called by a teacher
          before getting match status. *****/
   Mch_RemoveOldPlayers ();

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Update match status in database *****/
   Mch_UpdateMatchStatusInDB (&Match);

   /***** Update elapsed time in this question *****/
   Mch_UpdateElapsedTimeInQuestion (&Match);

   /***** Show current match status *****/
   Mch_ShowRefreshablePartTch (&Match);
  }

/*****************************************************************************/
/*************** Refresh current game for a student via AJAX *****************/
/*****************************************************************************/

void Mch_RefreshMatchStd (void)
  {
   struct Match Match;

   if (!Gbl.Session.IsOpen)	// If session has been closed, do not write anything
      return;

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

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
   unsigned NumRows;

   /***** Set default values for number of option and answer index *****/
   UsrAnswer->NumOpt = -1;	// < 0 ==> no answer selected
   UsrAnswer->AnsInd = -1;	// < 0 ==> no answer selected

   /***** Get student's answer *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get user's answer to a match question",
					"SELECT NumOpt,"	// row[0]
					       "AnsInd"		// row[1]
					" FROM mch_answers"
					" WHERE MchCod=%ld"
					" AND UsrCod=%ld"
					" AND QstInd=%u",
					MchCod,UsrCod,QstInd);
   if (NumRows) // Answer found...
     {
      row = mysql_fetch_row (mysql_res);

      /***** Get number of option index (row[0]) *****/
      if (sscanf (row[0],"%d",&(UsrAnswer->NumOpt)) != 1)
	 Lay_ShowErrorAndExit ("Error when getting student's answer to a match question.");

      /***** Get answer index (row[1]) *****/
      if (sscanf (row[1],"%d",&(UsrAnswer->AnsInd)) != 1)
	 Lay_ShowErrorAndExit ("Error when getting student's answer to a match question.");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Receive question answer from student when playing a match *********/
/*****************************************************************************/

void Mch_ReceiveQuestionAnswer (void)
  {
   struct Match Match;
   unsigned QstInd;
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];
   struct Mch_UsrAnswer PreviousUsrAnswer;
   struct Mch_UsrAnswer UsrAnswer;
   unsigned NumQsts;
   unsigned NumQstsNotBlank;
   double TotalScore;

   /***** Get data of the match from database *****/
   Match.MchCod = Gbl.Games.MchCodBeingPlayed;
   Mch_GetDataOfMatchByCod (&Match);

   /***** Get question index from form *****/
   QstInd = Gam_GetParamQstInd ();

   /***** Check that teacher's screen is showing answers
          and question index is the current one being played *****/
   if (Match.Status.Showing == Mch_ANSWERS &&	// Teacher's screen is showing answers
       QstInd == Match.Status.QstInd)		// Receiving an answer to the current question being played
     {
      /***** Get indexes for this question from database *****/
      Mch_GetIndexes (Match.MchCod,Match.Status.QstInd,Indexes);

      /***** Get answer index *****/
      /*
      Indexes[4] = {0,3,1,2}
      +-------+--------+----------+---------+
      | Button | Option | Answer   | Correct |
      | letter | number | index    |         |
      | screen | screen | database |         |
      +--------+--------+----------+---------+
      |   a    |    0   |    0     |         |
      |   b    |    1   |    3     |         |
      |   c    |    2   |    1     |    Y    | <---- User press button #2 (index = 1, correct)
      |   d    |    3   |    2     |         |
      +--------+--------+----------+---------+
      UsrAnswer.NumOpt = 2
      UsrAnswer.AnsInd = 1
      */
      UsrAnswer.NumOpt = Mch_GetParamNumOpt ();
      UsrAnswer.AnsInd = Indexes[UsrAnswer.NumOpt];

      /***** Get previous student's answer to this question
	     (<0 ==> no answer) *****/
      Mch_GetQstAnsFromDB (Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,Match.Status.QstInd,
			   &PreviousUsrAnswer);

      /***** Store student's answer *****/
      if (UsrAnswer.NumOpt >= 0 &&
	  UsrAnswer.AnsInd >= 0 &&
	  UsrAnswer.AnsInd != PreviousUsrAnswer.AnsInd)
	 DB_QueryREPLACE ("can not register your answer to the match question",
			  "REPLACE mch_answers"
			  " (MchCod,UsrCod,QstInd,NumOpt,AnsInd)"
			  " VALUES"
			  " (%ld,%ld,%u,%d,%d)",
			  Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,Match.Status.QstInd,
			  UsrAnswer.NumOpt,
			  UsrAnswer.AnsInd);

      /***** Update student's match result *****/
      McR_GetMatchResultQuestionsFromDB (Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,
					 &NumQsts,&NumQstsNotBlank);
      TotalScore = Mch_ComputeScore (NumQsts);

      Str_SetDecimalPointToUS ();	// To print the floating point as a dot
      if (DB_QueryCOUNT ("can not get if match result exists",
			 "SELECT COUNT(*) FROM mch_results"
			 " WHERE MchCod=%ld AND UsrCod=%ld",
			 Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod))	// Result exists
	 /* Update result */
	 DB_QueryUPDATE ("can not update match result",
			  "UPDATE mch_results"
			  " SET EndTime=NOW(),"
			       "NumQsts=%u,"
			       "NumQstsNotBlank=%u,"
			       "Score='%lf'"
			  " WHERE MchCod=%ld AND UsrCod=%ld",
			  NumQsts,NumQstsNotBlank,TotalScore,
			  Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod);
      else								// Result doesn't exist
	 /* Create result */
	 DB_QueryINSERT ("can not create match result",
			  "INSERT mch_results "
			  "(MchCod,UsrCod,StartTime,EndTime,NumQsts,NumQstsNotBlank,Score)"
			  " VALUES "
			  "(%ld,"	// MchCod
			  "%ld,"	// UsrCod
			  "NOW(),"	// StartTime
			  "NOW(),"	// EndTime
			  "%u,"		// NumQsts
			  "%u,"		// NumQstsNotBlank
			  "'%lf')",	// Score
			  Match.MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,
			  NumQsts,NumQstsNotBlank,TotalScore);
      Str_SetDecimalPointToLocal ();	// Return to local system
     }

   /***** Show current match status *****/
   HTM_DIV_Begin ("id=\"match\" class=\"MCH_CONT\"");
   Mch_ShowMatchStatusForStd (&Match,Mch_CHANGE_STATUS_BY_STUDENT);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Remove answer to match question ***********************/
/*****************************************************************************/

static void Mch_RemoveMyAnswerToMatchQuestion (const struct Match *Match)
  {
   DB_QueryDELETE ("can not remove your answer to the match question",
		    "DELETE FROM mch_answers"
		    " WHERE MchCod=%ld AND UsrCod=%ld AND QstInd=%u",
		    Match->MchCod,Gbl.Usrs.Me.UsrDat.UsrCod,Match->Status.QstInd);
  }

/*****************************************************************************/
/******************** Compute match score for a student **********************/
/*****************************************************************************/

static double Mch_ComputeScore (unsigned NumQsts)
  {
   unsigned NumQst;
   double ScoreThisQst;
   bool AnswerIsNotBlank;
   unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   bool AnswersUsr[Tst_MAX_OPTIONS_PER_QUESTION];
   double TotalScore = 0.0;

   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      /***** Get indexes for this question from string *****/
      Tst_GetIndexesFromStr (Gbl.Test.StrIndexesOneQst[NumQst],Indexes);

      /***** Get the user's answers for this question from string *****/
      Tst_GetAnswersFromStr (Gbl.Test.StrAnswersOneQst[NumQst],AnswersUsr);

      /***** Get correct answers of test question from database *****/
      Tst_GetCorrectAnswersFromDB (Gbl.Test.QstCodes[NumQst]);

      /***** Compute the total score of this question *****/
      Tst_ComputeScoreQst (Indexes,AnswersUsr,&ScoreThisQst,&AnswerIsNotBlank);

      /***** Compute total score *****/
      TotalScore += ScoreThisQst;
     }

   return TotalScore;
  }

/*****************************************************************************/
/*** Get number of users who selected this answer and draw proportional bar **/
/*****************************************************************************/

void Mch_GetAndDrawBarNumUsrsWhoHaveChosenAns (long MchCod,unsigned QstInd,unsigned AnsInd,
					       unsigned NumRespondersQst,bool Correct)
  {
   unsigned NumRespondersAns;

   /***** Get number of users who selected this answer *****/
   NumRespondersAns = Mch_GetNumUsrsWhoHaveChosenAns (MchCod,QstInd,AnsInd);

   /***** Show stats of this answer *****/
   Mch_DrawBarNumUsrs (NumRespondersAns,NumRespondersQst,Correct);
  }

/*****************************************************************************/
/********** Get number of users who answered a question in a match ***********/
/*****************************************************************************/

unsigned Mch_GetNumUsrsWhoAnsweredQst (long MchCod,unsigned QstInd)
  {
   /***** Get number of users who answered
          a question in a match from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of users who answered a question",
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
   (unsigned) DB_QueryCOUNT ("can not get number of users who have answered a match",
			     "SELECT COUNT(DISTINCT UsrCod) FROM mch_answers"
			     " WHERE MchCod=%ld",
			     MchCod);
  }

/*****************************************************************************/
/***************** Draw a bar with the percentage of answers *****************/
/*****************************************************************************/

// #define Mch_MAX_BAR_WIDTH 400
#define Mch_MAX_BAR_WIDTH 100

static void Mch_DrawBarNumUsrs (unsigned NumRespondersAns,unsigned NumRespondersQst,bool Correct)
  {
   extern const char *Txt_of_PART_OF_A_TOTAL;
   unsigned i;
   unsigned BarWidth = 0;

   /***** String with the number of users *****/
   if (NumRespondersQst)
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "%u&nbsp;(%u%%&nbsp;%s&nbsp;%u)",
                NumRespondersAns,
                (unsigned) ((((double) NumRespondersAns * 100.0) / (double) NumRespondersQst) + 0.5),
                Txt_of_PART_OF_A_TOTAL,NumRespondersQst);
   else
      snprintf (Gbl.Title,sizeof (Gbl.Title),
	        "0&nbsp;(0%%&nbsp;%s&nbsp;%u)",
                Txt_of_PART_OF_A_TOTAL,NumRespondersQst);

   /***** Start container *****/
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
		    (i < BarWidth) ? (Correct ? "MCH_RES_CORRECT" :
					        "MCH_RES_WRONG") :
				     "MCH_RES_VOID");
      HTM_TD_End ();
     }
   HTM_TR_End ();
   HTM_TABLE_End ();

   /***** Write the number of users *****/
   HTM_Txt (Gbl.Title);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************** Access to variable used to pass parameter ******************/
/*****************************************************************************/

void Mch_SetCurrentMchCod (long MchCod)
  {
   Mch_CurrentMchCod = MchCod;
  }

static long Mch_GetCurrentMchCod (void)
  {
   return Mch_CurrentMchCod;
  }
