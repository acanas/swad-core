// swad_match_database.c: matches in games using remote control, operations woth database

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_match_database.h"
#include "swad_show.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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
   extern const char Sho_Show_YN[Sho_NUM_SHOW];
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
		   Sho_Show_YN[Match->Status.Show.QstResults],
		   Sho_Show_YN[Match->Status.Show.UsrResults],
		   Match->MchCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
   free (MchSubQuery);
  }

/*****************************************************************************/
/********************* Update title of an existing match *********************/
/*****************************************************************************/

void Mch_DB_UpdateMatchTitle (long MchCod,
                              const char Title[Gam_MAX_BYTES_TITLE + 1])
  {
   DB_QueryUPDATE ("can not update match",
		   "UPDATE mch_matches"
		     " SET Title='%s'"
                   " WHERE MchCod=%ld",
		   Title,
		   MchCod);
  }

/*****************************************************************************/
/******************** Toggle visibility of match results *********************/
/*****************************************************************************/

void Mch_DB_UpdateVisResultsMchUsr (long MchCod,Sho_Show_t ShowUsrResults)
  {
   extern const char Sho_Show_YN[Sho_NUM_SHOW];

   DB_QueryUPDATE ("can not toggle visibility of match results",
		   "UPDATE mch_matches"
		     " SET ShowUsrResults='%c'"
		   " WHERE MchCod=%ld",
		   Sho_Show_YN[ShowUsrResults],
		   MchCod);
  }

/*****************************************************************************/
/********************** Get match data using its code ************************/
/*****************************************************************************/

Exi_Exist_t Mch_DB_GetMatchDataByCod (MYSQL_RES **mysql_res,long MchCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get matches",
			 "SELECT MchCod,"			// row[ 0]
				"GamCod,"			// row[ 1]
				"UsrCod,"			// row[ 2]
				"UNIX_TIMESTAMP(StartTime),"	// row[ 3]
				"UNIX_TIMESTAMP(EndTime),"	// row[ 4]
				"Title,"			// row[ 5]
				"QstInd,"			// row[ 6]
				"QstCod,"			// row[ 7]
				"Showing,"			// row[ 8]
				"Countdown,"			// row[ 9]
				"NumCols,"			// row[10]
				"ShowQstResults,"		// row[11]
				"ShowUsrResults"		// row[12]
			  " FROM mch_matches"
			 " WHERE MchCod=%ld"
			   " AND GamCod IN"		// Extra check
			       " (SELECT GamCod"
				  " FROM gam_games"
				 " WHERE CrsCod=%ld)",
		       MchCod,
		       Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********* Get start of first match and end of last match in a game **********/
/*****************************************************************************/

Exi_Exist_t Mch_DB_GetStartEndMatchesInGame (MYSQL_RES **mysql_res,long GamCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get game data",
			 "SELECT UNIX_TIMESTAMP(MIN(StartTime)),"	// row[0]
				"UNIX_TIMESTAMP(MAX(EndTime))"	// row[1]
			 " FROM mch_matches"
			 " WHERE GamCod=%ld",
			 GamCod);
  }

/*****************************************************************************/
/************************* Get the matches of a game *************************/
/*****************************************************************************/

unsigned Mch_DB_GetMatchesInGame (MYSQL_RES **mysql_res,long GamCod)
  {
   char *SubQuery;

   /***** Fill subquery for game *****/
   if (Gbl.Crs.Grps.MyAllGrps == Grp_MY_GROUPS)
     {
      if (asprintf (&SubQuery," AND"
			      " (MchCod NOT IN"
			       " (SELECT MchCod"
			          " FROM mch_groups)"
			       " OR"
			       " MchCod IN"
			       " (SELECT mch_groups.MchCod"
			          " FROM grp_users,"
			                "mch_groups"
			         " WHERE grp_users.UsrCod=%ld"
			           " AND grp_users.GrpCod=mch_groups.GrpCod))",
		     Gbl.Usrs.Me.UsrDat.UsrCod) < 0)
	  Err_NotEnoughMemoryExit ();
      }
    else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
       if (asprintf (&SubQuery,"%s","") < 0)
	  Err_NotEnoughMemoryExit ();

   /***** Make query *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get matches",
		   "SELECT MchCod,"			// row[ 0]
			  "GamCod,"			// row[ 1]
			  "UsrCod,"			// row[ 2]
			  "UNIX_TIMESTAMP(StartTime),"	// row[ 3]
			  "UNIX_TIMESTAMP(EndTime),"	// row[ 4]
			  "Title,"			// row[ 5]
			  "QstInd,"			// row[ 6]
			  "QstCod,"			// row[ 7]
			  "Showing,"			// row[ 8]
			  "Countdown,"			// row[ 9]
			  "NumCols,"			// row[10]
			  "ShowQstResults,"		// row[11]
			  "ShowUsrResults"		// row[12]
		    " FROM mch_matches"
		   " WHERE GamCod=%ld%s"
		" ORDER BY MchCod",
		   GamCod,
		   SubQuery);

   /***** Free allocated memory for subquery *****/
   free (SubQuery);
  }

/*****************************************************************************/
/********************* Get available matches in a game ***********************/
/*****************************************************************************/

unsigned Mch_DB_GetAvailableMatchesInGame (MYSQL_RES **mysql_res,long GamCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get matches",
		   "SELECT MchCod,"			// row[ 0]
			  "UsrCod,"			// row[ 1]
			  "UNIX_TIMESTAMP(StartTime),"	// row[ 2]
			  "UNIX_TIMESTAMP(EndTime),"	// row[ 3]
			  "Title,"			// row[ 4]
			  "QstInd"			// row[ 5]
		    " FROM mch_matches"
		   " WHERE GamCod=%ld"
		     " AND (MchCod NOT IN"
			  " (SELECT MchCod FROM mch_groups)"
			  " OR"
			  " MchCod IN"
			  " (SELECT mch_groups.MchCod"
			     " FROM mch_groups,"
				   "grp_users"
			    " WHERE grp_users.UsrCod=%ld"
			      " AND mch_groups.GrpCod=grp_users.GrpCod))"
		" ORDER BY MchCod",
		   GamCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************* Get matches between dates *************************/
/*****************************************************************************/

unsigned Mch_DB_GetMatchesBetweenDates (MYSQL_RES **mysql_res,
                                        const char *From,
                                        const char *To)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get matches",
		   "SELECT GamCod,"			// row[ 0]
			  "MchCod,"			// row[ 1]
                          "UsrCod,"			// row[ 2]
			  "UNIX_TIMESTAMP(StartTime),"	// row[ 3]
			  "UNIX_TIMESTAMP(EndTime),"	// row[ 4]
			  "Title"			// row[ 5]
		    " FROM mch_matches"
		   " WHERE StartTime>='%s'"
                     " AND StartTime<='%s'"
		" ORDER BY GamCod,"
		          "MchCod",
		   From,
		   To);
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
/********************** Remove match from all tables *************************/
/*****************************************************************************/
/*
mysql> SELECT table_name FROM information_schema.tables WHERE table_name LIKE 'mch%';
*/
void Mch_DB_RemoveMatchFromAllTables (long MchCod)
  {
   /***** Remove match from secondary tables *****/
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_players");
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_playing");
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_results");
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_answers");
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_times");
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_groups");
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_indexes");

   /***** Remove match from main table *****/
   Mch_DB_RemoveMatchFromTable (MchCod,"mch_matches");
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
/************** Get groups associated to a match from database ***************/
/*****************************************************************************/

unsigned Mch_DB_GetGrpCodsAssociatedToMatch (MYSQL_RES **mysql_res,long MchCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of a match",
		   "SELECT GrpCod"
		    " FROM mch_groups"
		   " WHERE MchCod=%ld",
	           MchCod);
  }

/*****************************************************************************/
/************** Get groups associated to a match from database ***************/
/*****************************************************************************/

unsigned Mch_DB_GetGrpNamesAssociatedToMatch (MYSQL_RES **mysql_res,long MchCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get groups of a match",
		   "SELECT grp_types.GrpTypName,"	// row[0]
			  "grp_groups.GrpName"		// row[1]
		    " FROM mch_groups,"
			  "grp_groups,"
			  "grp_types"
		   " WHERE mch_groups.MchCod=%ld"
		     " AND mch_groups.GrpCod=grp_groups.GrpCod"
		     " AND grp_groups.GrpTypCod=grp_types.GrpTypCod"
		" ORDER BY grp_types.GrpTypName,"
			  "grp_groups.GrpName",
		   MchCod);
  }

/*****************************************************************************/
/************ Check if I belong to any of the groups of a match **************/
/*****************************************************************************/

Usr_Can_t Mch_DB_CheckIfICanPlayThisMatchBasedOnGrps (long MchCod)
  {
   return
   DB_QueryEXISTS ("can not check if I can play a match",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM mch_matches"
		    " WHERE MchCod=%ld"
		      " AND (MchCod NOT IN"
			   " (SELECT MchCod"
			      " FROM mch_groups)"
			   " OR"
			   " MchCod IN"
			   " (SELECT mch_groups.MchCod"
			      " FROM grp_users,"
				    "mch_groups"
			     " WHERE grp_users.UsrCod=%ld"
			       " AND grp_users.GrpCod=mch_groups.GrpCod)))",
		   MchCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod) == Exi_EXISTS ? Usr_CAN :
							      Usr_CAN_NOT;
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
/*************** Get the questions of a match from database ******************/
/*****************************************************************************/

unsigned Mch_DB_GetMatchQuestions (MYSQL_RES **mysql_res,long MchCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get questions and indexes of options"
			     " of a match result",
		   "SELECT gam_questions.QstCod,"	// row[0]
			  "gam_questions.QstInd,"	// row[1]
			  "mch_indexes.Indexes"		// row[2]
		    " FROM mch_matches,"
		          "gam_questions,"
		          "mch_indexes"
		   " WHERE mch_matches.MchCod=%ld"
		     " AND mch_matches.GamCod=gam_questions.GamCod"
		     " AND mch_matches.MchCod=mch_indexes.MchCod"
		     " AND gam_questions.QstInd=mch_indexes.QstInd"
		" ORDER BY gam_questions.QstInd",
		   MchCod);
  }

/*****************************************************************************/
/******************** Update my answer to match question *********************/
/*****************************************************************************/

void Mch_DB_UpdateMyAnswerToMatchQuestion (const struct Mch_Match *Match,
                                           const struct Mch_UsrAnswer *UsrAnswer)
  {
   DB_QueryREPLACE ("can not register your answer to the match question",
		    "REPLACE mch_answers"
		    " (MchCod,UsrCod,QstInd,NumOpt,AnsInd)"
		    " VALUES"
		    " (%ld,%ld,%u,%d,%d)",
		    Match->MchCod,
		    Gbl.Usrs.Me.UsrDat.UsrCod,
		    Match->Status.QstInd,
		    UsrAnswer->NumOpt,
		    UsrAnswer->AnsInd);
  }

/*****************************************************************************/
/*** Update indexes of user answers to questions greater than a given one ****/
/*****************************************************************************/

void Mch_DB_UpdateIndexesOfQstsGreaterThan (long GamCod,unsigned QstInd)
  {
   DB_QueryUPDATE ("can not update indexes of questions",
		   "UPDATE mch_answers,"
		          "mch_matches"
		     " SET mch_answers.QstInd=mch_answers.QstInd-1"
		   " WHERE mch_matches.GamCod=%ld"
		     " AND mch_matches.MchCod=mch_answers.MchCod"
		     " AND mch_answers.QstInd>%u",
		   GamCod,
		   QstInd);
  }

/*****************************************************************************/
/******************** Get user's answer to a match question ******************/
/*****************************************************************************/

Exi_Exist_t Mch_DB_GetUsrAnsToQst (MYSQL_RES **mysql_res,
                                   long MchCod,long UsrCod,unsigned QstInd)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get user's answer to a match question",
			 "SELECT NumOpt,"	// row[0]
				"AnsInd"	// row[1]
			  " FROM mch_answers"
			 " WHERE MchCod=%ld"
			   " AND UsrCod=%ld"
			   " AND QstInd=%u",
			 MchCod,
			 UsrCod,
			 QstInd);
  }

/*****************************************************************************/
/********** Get number of users who answered a question in a match ***********/
/*****************************************************************************/

unsigned Mch_DB_GetNumUsrsWhoAnsweredQst (long MchCod,unsigned QstInd)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users who answered a question",
		  "SELECT COUNT(*)"
		   " FROM mch_answers"
		  " WHERE MchCod=%ld"
		    " AND QstInd=%u",
		  MchCod,
		  QstInd);
  }

/*****************************************************************************/
/*** Get number of users who have chosen a given answer of a game question ***/
/*****************************************************************************/

unsigned Mch_DB_GetNumUsrsWhoHaveChosenAns (long MchCod,unsigned QstInd,unsigned AnsInd)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users who have chosen an answer",
		  "SELECT COUNT(*)"
		   " FROM mch_answers"
		  " WHERE MchCod=%ld"
		    " AND QstInd=%u"
		    " AND AnsInd=%u",
		  MchCod,
		  QstInd,
		  AnsInd);
  }

/*****************************************************************************/
/******************* Remove my answer to match question **********************/
/*****************************************************************************/

void Mch_DB_RemoveMyAnswerToMatchQuestion (const struct Mch_Match *Match)
  {
   DB_QueryDELETE ("can not remove your answer to the match question",
		    "DELETE FROM mch_answers"
		    " WHERE MchCod=%ld"
		      " AND UsrCod=%ld"
		      " AND QstInd=%u",
		    Match->MchCod,
		    Gbl.Usrs.Me.UsrDat.UsrCod,
		    Match->Status.QstInd);
  }

/*****************************************************************************/
/***** Remove users' answers of a question from all matches of this game *****/
/*****************************************************************************/

void Mch_DB_RemUsrAnswersOfAQuestion (long GamCod,unsigned QstInd)
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
                              const char StrAnswersOneQst[Qst_MAX_BYTES_ANSWERS_ONE_QST + 1])
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
		        char StrIndexesOneQst[Qst_MAX_BYTES_INDEXES_ONE_QST + 1])
  {
   DB_QuerySELECTString (StrIndexesOneQst,Qst_MAX_BYTES_INDEXES_ONE_QST,
                         "can not get indexes of a question",
			 "SELECT Indexes"	// row[0]
			  " FROM mch_indexes"
			 " WHERE MchCod=%ld"
			   " AND QstInd=%u",
		 	 MchCod,
			 QstInd);
  }

/*****************************************************************************/
/********************** Update match as being played *************************/
/*****************************************************************************/

void Mch_DB_UpdateMatchAsBeingPlayed (long MchCod)
  {
   DB_QueryREPLACE ("can not set match as being played",
		    "REPLACE mch_playing"
		    " (MchCod)"
		     " VALUE"
		     " (%ld)",
		    MchCod);
  }

/*****************************************************************************/
/******************* Register me as a player in a match **********************/
/*****************************************************************************/

void Mch_DB_RegisterMeAsPlayerInMatch (long MchCod)
  {
   DB_QueryREPLACE ("can not insert match player",
		    "REPLACE mch_players"
		    " (MchCod,UsrCod)"
		    " VALUES"
		    " (%ld,%ld)",
		    MchCod,
		    Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************** Check if match is being played ***********************/
/*****************************************************************************/

bool Mch_DB_CheckIfMatchIsBeingPlayed (long MchCod)
  {
   return
   DB_QueryEXISTS ("can not check if match is being played",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM mch_playing"
		    " WHERE MchCod=%ld)",
		   MchCod) == Exi_EXISTS;
  }

/*****************************************************************************/
/************** Get number of players who are playing a match ****************/
/*****************************************************************************/

unsigned Mch_DB_GetNumPlayers (long MchCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of players",
		  "SELECT COUNT(*)"
		   " FROM mch_players"
		  " WHERE MchCod=%ld",
		  MchCod);
  }

/*****************************************************************************/
/**************** Update match as paused, not being played *******************/
/*****************************************************************************/

void Mch_DB_RemoveMatchFromBeingPlayed (long MchCod)
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

/*****************************************************************************/
/**************************** Remove old players *****************************/
/*****************************************************************************/

void Mch_DB_RemoveOldPlaying (void)
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

/*****************************************************************************/
/******************* Get elapsed time in a match question ********************/
/*****************************************************************************/

Exi_Exist_t Mch_DB_GetElapsedTimeInQuestion (MYSQL_RES **mysql_res,
					     long MchCod,unsigned QstInd)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get elapsed time",
			 "SELECT ElapsedTime"	// row[0]
			  " FROM mch_times"
			 " WHERE MchCod=%ld"
			   " AND QstInd=%u",
			 MchCod,
			 QstInd);
  }

/*****************************************************************************/
/*********************** Get elapsed time in a match *************************/
/*****************************************************************************/

Exi_Exist_t Mch_DB_GetElapsedTimeInMatch (MYSQL_RES **mysql_res,long MchCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get elapsed time",
			 "SELECT SEC_TO_TIME(SUM(TIME_TO_SEC(ElapsedTime)))"	// row[0]
			  " FROM mch_times"
			 " WHERE MchCod=%ld",
			 MchCod);
  }

/*****************************************************************************/
/***************************** Create match print ****************************/
/*****************************************************************************/

void Mch_DB_CreateMatchPrint (const struct MchPrn_Print *Print)
  {
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot

   DB_QueryINSERT ("can not create match result",
		    "INSERT mch_results "
		    "(MchCod,UsrCod,StartTime,EndTime,NumQsts,NumQstsNotBlank,Score)"
		    " VALUES "
		    "(%ld,"		// MchCod
		    "%ld,"		// UsrCod
		    "NOW(),"		// StartTime
		    "NOW(),"		// EndTime
		    "%u,"		// NumQsts
		    "%u,"		// NumQstsNotBlank
		    "'%.15lg')",	// Score
		    Print->MchCod,
		    Print->UsrCod,
		    Print->NumQsts.All,
		    Print->NumQsts.NotBlank,
		    Print->Score);

   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/***************************** Update match print ****************************/
/*****************************************************************************/

void Mch_DB_UpdateMatchPrint (const struct MchPrn_Print *Print)
  {
   Str_SetDecimalPointToUS ();		// To print the floating point as a dot

   DB_QueryUPDATE ("can not update match print",
		    "UPDATE mch_results"
		      " SET EndTime=NOW(),"
			   "NumQsts=%u,"
			   "NumQstsNotBlank=%u,"
			   "Score='%.15lg'"
		    " WHERE MchCod=%ld"
		      " AND UsrCod=%ld",
		    Print->NumQsts.All,
		    Print->NumQsts.NotBlank,
		    Print->Score,
		    Print->MchCod,
		    Print->UsrCod);

   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/*********************** Check if match print exists *************************/
/*****************************************************************************/

Exi_Exist_t Mch_DB_CheckIfMatchPrintExists (const struct MchPrn_Print *Print)
  {
   return
   DB_QueryEXISTS ("can not check if match print exists",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM mch_results"
		    " WHERE MchCod=%ld"
		      " AND UsrCod=%ld)",
		   Print->MchCod,
		   Print->UsrCod);
  }

/*****************************************************************************/
/********* Get data of a match print using match code and user code **********/
/*****************************************************************************/

Exi_Exist_t Mch_DB_GetMatchPrintData (MYSQL_RES **mysql_res,
                                      const struct MchPrn_Print *Print)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get data of a match print",
			 "SELECT UNIX_TIMESTAMP(mch_results.StartTime),"	// row[1]
				"UNIX_TIMESTAMP(mch_results.EndTime),"		// row[2]
				"mch_results.NumQsts,"				// row[3]
				"mch_results.NumQstsNotBlank,"			// row[4]
				"mch_results.Score"				// row[5]
			  " FROM mch_results,"
				"mch_matches,"
				"gam_games"
			 " WHERE mch_results.MchCod=%ld"
			   " AND mch_results.UsrCod=%ld"
			   " AND mch_results.MchCod=mch_matches.MchCod"
			   " AND mch_matches.GamCod=gam_games.GamCod"
			   " AND gam_games.CrsCod=%ld",		// Extra check
			 Print->MchCod,
			 Print->UsrCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********* Get data of a match print using match code and user code **********/
/*****************************************************************************/

unsigned Mch_DB_GetPrintsInMatch (MYSQL_RES **mysql_res,long MchCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get match prints",
		   "SELECT UsrCod,"		// row[0]
			  "NumQsts,"		// row[1]
			  "NumQstsNotBlank,"	// row[2]
			  "Score"		// row[3]
		    " FROM mch_results"
		   " WHERE MchCod=%ld",
		   MchCod);
  }

/*****************************************************************************/
/************ Get number of users who have played a given match **************/
/*****************************************************************************/

unsigned Mch_DB_GetNumUsrsWhoHavePlayedMch (long MchCod)
  {
   /***** Get number of results in the match
          (number of users who have a result for this match, even blank result)
          from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users who have played a match",
		  "SELECT COUNT(*)"
		   " FROM mch_results"
		  " WHERE MchCod=%ld",
		  MchCod);
  }

/*****************************************************************************/
/**** Get all users who have answered any match question in a given match *****/
/*****************************************************************************/

unsigned Mch_DB_GetUsrsWhoHavePlayedMch (MYSQL_RES **mysql_res,long MchCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users in match",
		   "SELECT users.UsrCod"
		    " FROM (SELECT mch_results.UsrCod AS UsrCod"
			    " FROM mch_results,"
				  "mch_matches,"
				  "gam_games"
			   " WHERE mch_results.MchCod=%ld"
			     " AND mch_results.MchCod=mch_matches.MchCod"
			     " AND mch_matches.GamCod=gam_games.GamCod"
			     " AND gam_games.CrsCod=%ld) AS users,"	// Extra check
			  "usr_data"
		   " WHERE users.UsrCod=usr_data.UsrCod"
	        " ORDER BY usr_data.Surname1,"
			  "usr_data.Surname2,"
			  "usr_data.FirstName",
		   MchCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/**** Get all users who have answered any match question in a given game *****/
/*****************************************************************************/

unsigned Mch_DB_GetUsrsWhoHavePlayedGam (MYSQL_RES **mysql_res,long GamCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users in game",
		   "SELECT users.UsrCod"
		    " FROM (SELECT DISTINCT "
		                  "mch_results.UsrCod AS UsrCod"
			    " FROM mch_results,"
				  "mch_matches,"
				  "gam_games"
			   " WHERE mch_matches.GamCod=%ld"
			     " AND mch_matches.MchCod=mch_results.MchCod"
			     " AND mch_matches.GamCod=gam_games.GamCod"
			     " AND gam_games.CrsCod=%ld) AS users,"		// Extra check
			   "usr_data"
		   " WHERE users.UsrCod=usr_data.UsrCod"
		" ORDER BY usr_data.Surname1,"
			  "usr_data.Surname2,"
			  "usr_data.FirstName",
		   GamCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********* Show the matches results of a user in the current course **********/
/*****************************************************************************/

unsigned Mch_DB_GetUsrMchResults (MYSQL_RES **mysql_res,
                                  Usr_MeOrOther_t MeOrOther,
				  long MchCod,	// <= 0 ==> any
				  long GamCod,	// <= 0 ==> any
				  const char *GamesSelectedCommas)
  {
   char *MchSubQuery;
   char *GamSubQuery;
   char *HidGamSubQuery;
   unsigned NumResults;

   /***** Build matches subquery *****/
   if (MchCod > 0)
     {
      if (asprintf (&MchSubQuery," AND mch_results.MchCod=%ld",MchCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else
     {
      if (asprintf (&MchSubQuery,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }

   /***** Build games subquery *****/
   if (GamCod > 0)
     {
      if (asprintf (&GamSubQuery," AND mch_matches.GamCod=%ld",GamCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else if (GamesSelectedCommas)
     {
      if (GamesSelectedCommas[0])
	{
	 if (asprintf (&GamSubQuery," AND mch_matches.GamCod IN (%s)",
		       GamesSelectedCommas) < 0)
	    Err_NotEnoughMemoryExit ();
	}
      else
	{
	 if (asprintf (&GamSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	}
     }
   else
     {
      if (asprintf (&GamSubQuery,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }

   /***** Subquery: get hidden games?
	  · A student will not be able to see their results in hidden games
	  · A teacher will be able to see results from other users even in hidden games
   *****/
   switch (MeOrOther)
     {
      case Usr_ME:	// A student watching her/his results
         if (asprintf (&HidGamSubQuery," AND gam_games.Hidden='N'") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      default:		// A teacher/admin watching the results of other users
	 if (asprintf (&HidGamSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
     }

   /***** Make database query *****/
   NumResults = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get matches results",
		   "SELECT mch_results.MchCod"
		    " FROM mch_results,"
			  "mch_matches,"
			  "gam_games"
		   " WHERE mch_results.UsrCod=%ld"
		      "%s"	// Match subquery
		     " AND mch_results.MchCod=mch_matches.MchCod"
		     "%s"	// Games subquery
		     " AND mch_matches.GamCod=gam_games.GamCod"
		     "%s"	// Hidden games subquery
		     " AND gam_games.CrsCod=%ld"	// Extra check
		" ORDER BY mch_matches.Title",
		   MeOrOther == Usr_ME ? Gbl.Usrs.Me.UsrDat.UsrCod :
				         Gbl.Usrs.Other.UsrDat.UsrCod,
		   MchSubQuery,
		   GamSubQuery,
		   HidGamSubQuery,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /***** Free subqueries *****/
   free (HidGamSubQuery);
   free (GamSubQuery);
   free (MchSubQuery);

   return NumResults;
  }

/*****************************************************************************/
/********* Get maximum number of users per score in match results ************/
/*****************************************************************************/

unsigned Mch_DB_GetMaxUsrsPerScore (long MchCod)
  {
   return DB_QuerySELECTUnsigned ("can not get max users",
				  "SELECT MAX(NumUsrs)"
				   " FROM (SELECT COUNT(*) AS NumUsrs"
					   " FROM mch_results"
					  " WHERE MchCod=%ld"
				       " GROUP BY Score"
				       " ORDER BY Score) AS Scores",
				  MchCod);
  }

/*****************************************************************************/
/************** Get number of users per score in match results ***************/
/*****************************************************************************/

unsigned Mch_DB_GetNumUsrsPerScore (MYSQL_RES **mysql_res,long MchCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get scores",
		   "SELECT Score,"		// row[0]
			  "COUNT(*) AS NumUsrs"	// row[1]
		    " FROM mch_results"
		   " WHERE MchCod=%ld"
		" GROUP BY Score"
		" ORDER BY Score DESC",
		   MchCod);
  }
