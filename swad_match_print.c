// swad_match_print.c: matches prints in games using remote control

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

#include <math.h>	// For fabs

#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_match.h"
#include "swad_match_database.h"
#include "swad_match_print.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void MchPrn_ComputeScore (struct MchPrn_Print *Print);
static void MchPrn_UpdateMatchPrintInDB (const struct MchPrn_Print *Print);

static void MchPrn_ListPrintsToFix (long MchCod);

/*****************************************************************************/
/**************************** Reset match print ******************************/
/*****************************************************************************/

void MchPrn_ResetPrint (struct MchPrn_Print *Print)
  {
   Print->MchCod = -1L;
   Print->UsrCod = -1L;
   Print->TimeUTC[Dat_STR_TIME] =
   Print->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Print->NumQsts.All      =
   Print->NumQsts.NotBlank = 0;
   Print->Score            = 0.0;
  }

/*****************************************************************************/
/*********** Compute score and create/update my result in a match ************/
/*****************************************************************************/

void MchPrn_ComputeScoreAndUpdateMyMatchPrintInDB (long MchCod)
  {
   struct MchPrn_Print Print;

   /***** Compute my match result *****/
   MchPrn_ResetPrint (&Print);
   Print.MchCod = MchCod;
   Print.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Mch_GetMatchQuestionsFromDB (&Print);
   MchPrn_ComputeScore (&Print);

   /***** Update my match result in database *****/
   MchPrn_UpdateMatchPrintInDB (&Print);
  }

/*****************************************************************************/
/***************** Compute match print score for a student *******************/
/*****************************************************************************/

static void MchPrn_ComputeScore (struct MchPrn_Print *Print)
  {
   unsigned NumQst;
   struct Qst_Question Question;

   for (NumQst = 0, Print->Score = 0.0;
	NumQst < Print->NumQsts.All;
	NumQst++)
     {
      /***** Create test question *****/
      Qst_QstConstructor (&Question);
      Question.QstCod = Print->PrintedQuestions[NumQst].QstCod;
      Question.Answer.Type = Qst_ANS_UNIQUE_CHOICE;

      /***** Compute score for this answer ******/
      TstPrn_ComputeAnswerScore (&Print->PrintedQuestions[NumQst],&Question);

      /***** Update total score *****/
      Print->Score += Print->PrintedQuestions[NumQst].Score;

      /***** Destroy test question *****/
      Qst_QstDestructor (&Question);
     }
  }

/*****************************************************************************/
/************************* Create/update match print *************************/
/*****************************************************************************/

static void MchPrn_UpdateMatchPrintInDB (const struct MchPrn_Print *Print)
  {
   if (Mch_DB_CheckIfMatchPrintExists (Print))	// Match print exists
      /* Update match print */
      Mch_DB_UpdateMatchPrint (Print);
   else					// Match print doesn't exist
      /* Create match print */
      Mch_DB_CreateMatchPrint (Print);
  }

/*****************************************************************************/
/********* Get data of a match print using match code and user code **********/
/*****************************************************************************/

void MchPrn_GetMatchPrintDataByMchCodAndUsrCod (struct MchPrn_Print *Print)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Dat_StartEndTime_t StartEndTime;

   /***** Make database query *****/
   if (Mch_DB_GetMatchPrintData (&mysql_res,Print) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get start time (row[0] and row[1] hold UTC date-times) */
      for (StartEndTime  = (Dat_StartEndTime_t) 0;
	   StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	   StartEndTime++)
         Print->TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[StartEndTime]);

      /* Get number of questions (row[2]) */
      if (sscanf (row[2],"%u",&Print->NumQsts.All) != 1)
	 Print->NumQsts.All = 0;

      /* Get number of questions not blank (row[3]) */
      if (sscanf (row[3],"%u",&Print->NumQsts.NotBlank) != 1)
	 Print->NumQsts.NotBlank = 0;

      /* Get score (row[4]) */
      Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
      if (sscanf (row[4],"%lf",&Print->Score) != 1)
	 Print->Score = 0.0;
      Str_SetDecimalPointToLocal ();	// Return to local system
     }
   else
      MchPrn_ResetPrint (Print);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******** Recompute scores stored in match prints made on 2021-11-25 *********/
/*****************************************************************************/

void MchPrn_PutLinkToFixMatchesPrintsScores (void)
  {
   Lay_PutContextualLinkIconText (ActFixMchSco,NULL,
                                  NULL,NULL,
				  "recycle.svg",
				  "Recalcular puntuaci&oacute;n partidas");
  }

void MchPrn_FixMatchesPrintsScores (void)
  {
   extern const char *Txt_Matches;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumMatches;
   unsigned NumMatch;
   unsigned UniqueId;
   struct Mch_Match Match;
   Dat_StartEndTime_t StartEndTime;

   /***** Reset match *****/
   Mch_ResetMatch (&Match);

   Box_BoxBegin ("100%",Txt_Matches,
                 NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);

      /***** Get data of matches from database *****/
      NumMatches = Mch_DB_GetMatchesBetweenDates (&mysql_res,
						  "2021-11-24 20:00:00",	// From
						  "2021-11-25 16:00:00");	// To

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

	 /***** Write rows *****/
	 for (NumMatch = 0, UniqueId = 1;
	      NumMatch < NumMatches;
	      NumMatch++, UniqueId++)
	   {
	    Gbl.RowEvenOdd = NumMatch % 2;

	    /***** Get match data *****/
	    row = mysql_fetch_row (mysql_res);
	    /*
	    row[0]: GamCod
	    row[1]: MchCod
	    row[2]: UsrCod
	    row[3]: UNIX_TIMESTAMP(StartTime)
	    row[4]: UNIX_TIMESTAMP(EndTime)
	    row[5]: Title
	    */
	    /* Code of the game (row[0]) */
	    if ((Match.GamCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	       Err_WrongGameExit ();

	    /* Code of the match (row[1]) */
	    if ((Match.MchCod = Str_ConvertStrCodToLongCod (row[1])) <= 0)
	       Err_WrongMatchExit ();

	    /* Get match teacher (row[2]) */
	    Match.UsrCod = Str_ConvertStrCodToLongCod (row[2]);

	    /* Get start/end times (row[3], row[4] hold start/end UTC times) */
	    for (StartEndTime  = (Dat_StartEndTime_t) 0;
		 StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
		 StartEndTime++)
	       Match.TimeUTC[StartEndTime] = Dat_GetUNIXTimeFromStr (row[3 + StartEndTime]);

	    /* Get the title of the match (row[5]) */
	    if (row[5])
	       Str_Copy (Match.Title,row[5],sizeof (Match.Title) - 1);
	    else
	       Match.Title[0] = '\0';

	    /***** List match *****/
	    HTM_TR_Begin (NULL);

	       /* Game code */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  HTM_Txt ("GamCod: ");
		  HTM_Long (Match.GamCod);
	       HTM_TD_End ();

	       /* Match code */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  HTM_Txt ("MchCod: ");
		  HTM_Long (Match.MchCod);
	       HTM_TD_End ();

	       /* Match author */
	       Mch_ListOneOrMoreMatchesAuthor (&Match);

	       /* Start/end date/time */
	       Mch_ListOneOrMoreMatchesTimes (&Match,UniqueId);

	       /* Match title */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  HTM_Txt (Match.Title);
	       HTM_TD_End ();

	       /* Number of players who have played the match */
	       Mch_ListOneOrMoreMatchesNumPlayers (&Match);

	       /* Filling */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
	       HTM_TD_End ();

	    HTM_TR_End ();

	    /***** Match prints *****/
	    MchPrn_ListPrintsToFix (Match.MchCod);
	   }

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End box *****/
   Box_BoxEnd ();
  }

static void MchPrn_ListPrintsToFix (long MchCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumPrints;
   unsigned NumPrint;
   struct MchPrn_Print Print;
   double StoredScore;

   /***** Get data of prints from database *****/
   NumPrints = Mch_DB_GetPrintsInMatch (&mysql_res,MchCod);

	 /***** Write rows *****/
	 for (NumPrint = 0;
	      NumPrint < NumPrints;
	      NumPrint++)
	   {
	    /***** Get match print data *****/
	    Print.MchCod = MchCod;
	    row = mysql_fetch_row (mysql_res);
	    /*
	    row[0]: UsrCod
	    row[1]: NumQsts
	    row[2]: NumQstsNotBlank
	    row[3]: Score
	    */
	    /* Get student code (row[0]) */
	    Print.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	    /* Get number of questions (row[1]) */
	    if (sscanf (row[1],"%u",&Print.NumQsts.All) != 1)
	       Print.NumQsts.All = 0;

	    /* Get number of questions not blank (row[2]) */
	    if (sscanf (row[2],"%u",&Print.NumQsts.NotBlank) != 1)
	       Print.NumQsts.NotBlank = 0;

	    /* Get score (row[3]) */
	    Str_SetDecimalPointToUS ();	// To get the decimal point as a dot
	    if (sscanf (row[3],"%lf",&StoredScore) != 1)
	       StoredScore = 0.0;
	    Str_SetDecimalPointToLocal ();	// Return to local system

	    /***** Write row *****/
	    HTM_TR_Begin (NULL);
	       /* Indent */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
	       HTM_TD_End ();
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
	       HTM_TD_End ();

	       /* Student */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  Usr_WriteAuthor1Line (Print.UsrCod,false);
	       HTM_TD_End ();

	       /* Number of questions */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  HTM_Txt ("NumQsts: ");
		  HTM_Unsigned (Print.NumQsts.All);
	       HTM_TD_End ();

	       /* Number of questions not blank */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  HTM_Txt ("NumQstsNotBlank: ");
		  HTM_Unsigned (Print.NumQsts.NotBlank);
	       HTM_TD_End ();

	       /* Stored score */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  HTM_Txt ("Stored score: ");
		  HTM_Double (StoredScore);
	       HTM_TD_End ();

	       /* Compute score */
	       Mch_GetMatchQuestionsFromDB (&Print);
	       MchPrn_ComputeScore (&Print);

	       /* Computed score */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
		  HTM_Txt ("Computed score: ");
		  HTM_Double (Print.Score);
	       HTM_TD_End ();

	       /* Store computed score? */
	       HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
	          if (fabs (StoredScore-Print.Score) < 0.0000000001)
		     HTM_Txt ("=");
	          else
	            {
	             HTM_Txt ("!");
	             Mch_DB_UpdateMatchPrintScore (&Print);
	            }
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
