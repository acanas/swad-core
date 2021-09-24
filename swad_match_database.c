// swad_match_database.c: matches in games using remote control, operations woth database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_database.h"
// #include "swad_date.h"
#include "swad_error.h"
// #include "swad_form.h"
// #include "swad_game.h"
// #include "swad_game_database.h"
#include "swad_global.h"
// #include "swad_group_database.h"
// #include "swad_HTML.h"
// #include "swad_match.h"
#include "swad_match_database.h"
// #include "swad_match_result.h"
// #include "swad_role.h"
// #include "swad_setting.h"
// #include "swad_test.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

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

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/********************** Create a new match in a game *************************/
/*****************************************************************************/

long Mch_DB_CreateMatch (long GamCod,char Title[Mch_MAX_BYTES_TITLE + 1])
  {
   return
   DB_QueryINSERTandReturnCode ("can not create match",
				"INSERT mch_matches"
				" (GamCod,UsrCod,StartTime,EndTime,Title,"
				  "QstInd,QstCod,Showing,Countdown,"
				  "NumCols,ShowQstResults,ShowUsrResults)"
				" VALUES"
				" (%ld,"	// GamCod
				  "%ld,"	// UsrCod
				  "NOW(),"	// StartTime
				  "NOW(),"	// EndTime
				  "'%s',"	// Title
				  "0,"		// QstInd: Match has not started, so not the first question yet
				  "-1,"		// QstCod: Non-existent question
				  "'%s',"	// Showing: What is being shown
				  "-1,"		// Countdown: No countdown
				  "%u,"		// NumCols: Number of columns in answers
				  "'N',"	// ShowQstResults: Don't show question results initially
				  "'N')",	// ShowUsrResults: Don't show user results initially
				GamCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,	// Game creator
				Title,
				Mch_ShowingStringsDB[Mch_SHOWING_DEFAULT],
				Mch_NUM_COLS_DEFAULT);
  }

/*****************************************************************************/
/******************** Update a game match being played ***********************/
/*****************************************************************************/

void Mch_DB_UpdateMatchStatus (const struct Mch_Match *Match)
  {
   char *MchSubQuery;

   /***** Update end time only if match is currently being played *****/
   if (Match->Status.Playing)	// Match is being played
     {
      if (asprintf (&MchSubQuery,"mch_matches.EndTime=NOW(),") < 0)
         Err_NotEnoughMemoryExit ();
     }
   else				// Match is paused, not being played
     {
      if (asprintf (&MchSubQuery,"%s","") < 0)
         Err_NotEnoughMemoryExit ();
     }

   /***** Update match status in database *****/
   DB_QueryUPDATE ("can not update match being played",
		   "UPDATE mch_matches,"
		          "gam_games"
		     " SET %s"
			  "mch_matches.QstInd=%u,"
			  "mch_matches.QstCod=%ld,"
			  "mch_matches.Showing='%s',"
		          "mch_matches.Countdown=%ld,"
		          "mch_matches.NumCols=%u,"
			  "mch_matches.ShowQstResults='%c',"
			  "mch_matches.ShowUsrResults='%c'"
		   " WHERE mch_matches.MchCod=%ld"
		     " AND mch_matches.GamCod=gam_games.GamCod"
		     " AND gam_games.CrsCod=%ld",	// Extra check
		   MchSubQuery,
		   Match->Status.QstInd,
		   Match->Status.QstCod,
		   Mch_ShowingStringsDB[Match->Status.Showing],
		   Match->Status.Countdown,
		   Match->Status.NumCols,
		   Match->Status.ShowQstResults ? 'Y' :
			                          'N',
		   Match->Status.ShowUsrResults ? 'Y' :
			                          'N',
		   Match->MchCod,
		   Gbl.Hierarchy.Crs.CrsCod);
   free (MchSubQuery);
  }

/*****************************************************************************/
/********************* Update title of an existing match *********************/
/*****************************************************************************/

void Mch_DB_UpdateMatchTitle (const struct Mch_Match *Match)
  {
   DB_QueryUPDATE ("can not update match",
		   "UPDATE mch_matches"
		     " SET Title='%s'"
                   " WHERE MchCod=%ld",
		   Match->Title,
		   Match->MchCod);
  }

/*****************************************************************************/
/****************** Get parameter with what is being shown *******************/
/*****************************************************************************/

Mch_Showing_t Mch_DB_GetShowingFromStr (const char *Str)
  {
   Mch_Showing_t Showing;

   for (Showing  = (Mch_Showing_t) 0;
	Showing <= (Mch_Showing_t) (Mch_NUM_SHOWING - 1);
	Showing++)
      if (!strcmp (Str,Mch_ShowingStringsDB[Showing]))
         return Showing;

   return (Mch_Showing_t) Mch_SHOWING_DEFAULT;
  }

/*****************************************************************************/
/********************** Get number of matches in a game **********************/
/*****************************************************************************/

unsigned Mch_DB_GetNumMchsInGame (long GamCod)
  {
   /***** Trivial check *****/
   if (GamCod < 0)	// A non-existing game...
      return 0;		// ...has no matches

   /***** Get number of matches in a game from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of matches of a game",
		  "SELECT COUNT(*)"
		   " FROM mch_matches"
		  " WHERE GamCod=%ld",
		  GamCod);
  }

/*****************************************************************************/
/*************** Get number of unfinished matches in a game ******************/
/*****************************************************************************/

unsigned Mch_DB_GetNumUnfinishedMchsInGame (long GamCod)
  {
   /***** Trivial check *****/
   if (GamCod < 0)	// A non-existing game...
      return 0;		// ...has no matches

   /***** Get number of matches in a game from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of unfinished matches of a game",
		  "SELECT COUNT(*)"
		   " FROM mch_matches"
		  " WHERE GamCod=%ld"
		    " AND Showing<>'%s'",
		  GamCod,
		  Mch_ShowingStringsDB[Mch_END]);
  }

/*****************************************************************************/
/************************* Remove match from table ***************************/
/*****************************************************************************/

void Mch_DB_RemoveMatchFromTable (long MchCod,const char *TableName)
  {
   DB_QueryDELETE ("can not remove match from table",
		   "DELETE FROM %s"
		   " WHERE MchCod=%ld",
		   TableName,
		   MchCod);
  }

/*****************************************************************************/
/****************** Remove matches in game from main table *******************/
/*****************************************************************************/

void Mch_DB_RemoveMatchesInGameFromMainTable (long GamCod)
  {
   DB_QueryDELETE ("can not remove matches of a game",
		   "DELETE FROM mch_matches"
		   " WHERE GamCod=%ld",
		   GamCod);
  }

/*****************************************************************************/
/***************** Remove matches in game from secondary table ***************/
/*****************************************************************************/

void Mch_DB_RemoveMatchesInGameFromOtherTable (long GamCod,const char *TableName)
  {
   DB_QueryDELETE ("can not remove matches of a game from table",
		   "DELETE FROM %s"
		   " USING mch_matches,"
		          "%s"
		   " WHERE mch_matches.GamCod=%ld"
		     " AND mch_matches.MchCod=%s.MchCod",
		   TableName,
		   TableName,
		   GamCod,
		   TableName);
  }

/*****************************************************************************/
/***************** Remove matches in course from main table ******************/
/*****************************************************************************/

void Mch_DB_RemoveMatchesInCrsFromMainTable (long CrsCod)
  {
   DB_QueryDELETE ("can not remove matches of a course",
		   "DELETE FROM mch_matches"
		   " USING gam_games,"
		          "mch_matches"
		   " WHERE gam_games.CrsCod=%ld"
		     " AND gam_games.GamCod=mch_matches.GamCod",
		   CrsCod);
  }

/*****************************************************************************/
/*************** Remove matches in course from secondary table ***************/
/*****************************************************************************/

void Mch_DB_RemoveMatchesInCrsFromOtherTable (long CrsCod,const char *TableName)
  {
   DB_QueryDELETE ("can not remove matches of a course from table",
		   "DELETE FROM %s"
		   " USING gam_games,"
		          "mch_matches,"
		          "%s"
		   " WHERE gam_games.CrsCod=%ld"
		     " AND gam_games.GamCod=mch_matches.GamCod"
		     " AND mch_matches.MchCod=%s.MchCod",
		   TableName,
		   TableName,
		   CrsCod,
		   TableName);
  }

/*****************************************************************************/
/****************** Remove matches made by a user from table *****************/
/*****************************************************************************/

void Mch_DB_RemoveMatchesMadeByUsrFromTable (long UsrCod,const char *TableName)
  {
   DB_QueryDELETE ("can not remove matches of a user from table",
		   "DELETE FROM %s"
		   " WHERE UsrCod=%ld",
		   TableName,
		   UsrCod);
  }

/*****************************************************************************/
/******* Remove matches made by a user in a course from secondary table ******/
/*****************************************************************************/

void Mch_DB_RemoveMatchesMadeByUsrInCrsFromTable (long UsrCod,long CrsCod,
                                                  const char *TableName)
  {
   DB_QueryDELETE ("can not remove matches of a user from table",
		   "DELETE FROM %s"
		   " USING gam_games,"
		          "mch_matches,"
		          "%s"
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
/******************** Create group associated to a match *********************/
/*****************************************************************************/

void Mch_DB_AssociateGroupToMatch (long MchCod,long GrpCod)
  {
   DB_QueryINSERT ("can not associate a group to a match",
		   "INSERT INTO mch_groups"
		   " (MchCod,GrpCod)"
		   " VALUES"
		   " (%ld,%ld)",
		   MchCod,
		   GrpCod);
  }

/*****************************************************************************/
/********************* Remove one group from all matches *********************/
/*****************************************************************************/

void Mch_DB_RemoveGroup (long GrpCod)
  {
   DB_QueryDELETE ("can not remove group"
	           " from the associations between matches and groups",
		   "DELETE FROM mch_groups"
		   " WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/***************** Remove groups of one type from all matches ****************/
/*****************************************************************************/

void Mch_DB_RemoveGroupsOfType (long GrpTypCod)
  {
   DB_QueryDELETE ("can not remove groups of a type"
	           " from the associations between matches and groups",
		   "DELETE FROM mch_groups"
		   " USING grp_groups,"
		          "mch_groups"
		   " WHERE grp_groups.GrpTypCod=%ld"
		     " AND grp_groups.GrpCod=mch_groups.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/******** Remove answers of a question from all matches of this game *********/
/*****************************************************************************/

void Mch_DB_RemAnswersOfAQuestion (long GamCod,unsigned QstInd)
  {
   DB_QueryDELETE ("can not remove the answers of a question",
		   "DELETE FROM mch_answers"
		   " USING mch_matches,"
		          "mch_answers"
		   " WHERE mch_matches.GamCod=%ld"	// From all matches of this game...
		     " AND mch_matches.MchCod=mch_answers.MchCod"
		     " AND mch_answers.QstInd=%u",	// ...remove only answers to this question
		   GamCod,
		   QstInd);
  }

/*****************************************************************************/
/********* Create entry for this question in table of match indexes **********/
/*****************************************************************************/

void Mch_DB_CreateQstIndexes (long MchCod,unsigned QstInd,
                              const char StrAnswersOneQst[Tst_MAX_BYTES_ANSWERS_ONE_QST + 1])
  {
   DB_QueryINSERT ("can not create indexes of a question",
		   "INSERT INTO mch_indexes"
		   " (MchCod,QstInd,Indexes)"
		   " VALUES"
		   " (%ld,%u,'%s')",
		   MchCod,
		   QstInd,
		   StrAnswersOneQst);
  }

/*****************************************************************************/
/***************** Get indexes for a question from database ******************/
/*****************************************************************************/

void Mch_DB_GetIndexes (long MchCod,unsigned QstInd,
		        char StrIndexesOneQst[Tst_MAX_BYTES_INDEXES_ONE_QST + 1])
  {
   DB_QuerySELECTString (StrIndexesOneQst,Tst_MAX_BYTES_INDEXES_ONE_QST,
                         "can not get indexes of a question",
			 "SELECT Indexes"	// row[0]
			  " FROM mch_indexes"
			 " WHERE MchCod=%ld"
			   " AND QstInd=%u",
		 	 MchCod,
			 QstInd);
  }

/*****************************************************************************/
/********** Update elapsed time in current question (by a teacher) ***********/
/*****************************************************************************/

void Mch_DB_UpdateElapsedTimeInQuestion (long MchCod,long QstInd)
  {
   DB_QueryINSERT ("can not update elapsed time in question",
		   "INSERT INTO mch_times"
		   " (MchCod,QstInd,ElapsedTime)"
		   " VALUES"
		   " (%ld,%u,SEC_TO_TIME(%u))"
		   " ON DUPLICATE KEY"
		   " UPDATE ElapsedTime=ADDTIME(ElapsedTime,SEC_TO_TIME(%u))",
		   MchCod,
		   QstInd,
		   Cfg_SECONDS_TO_REFRESH_MATCH_TCH,
		   Cfg_SECONDS_TO_REFRESH_MATCH_TCH);
  }

