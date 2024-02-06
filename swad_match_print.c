// swad_match_print.c: matches prints in games using remote control

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
