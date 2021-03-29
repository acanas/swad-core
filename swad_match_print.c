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

#include "swad_database.h"
#include "swad_date.h"
#include "swad_global.h"
#include "swad_match.h"
#include "swad_match_print.h"

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

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void MchPrn_UpdateMyMatchPrintInDB (struct MchPrn_Print *Print);

/*****************************************************************************/
/**************************** Reset match print ******************************/
/*****************************************************************************/

void MchPrn_ResetPrint (struct MchPrn_Print *Print)
  {
   Print->MchCod = -1L;
   Print->UsrCod = -1L;
   Print->TimeUTC[Dat_START_TIME] =
   Print->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
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
   Mch_ComputeScore (&Print);

   /***** Update my match result in database *****/
   MchPrn_UpdateMyMatchPrintInDB (&Print);
  }

/*****************************************************************************/
/******************** Create/update my result in a match *********************/
/*****************************************************************************/

static void MchPrn_UpdateMyMatchPrintInDB (struct MchPrn_Print *Print)
  {
   Str_SetDecimalPointToUS ();	// To print the floating point as a dot
   if (DB_QueryCOUNT ("can not get if match result exists",
		      "SELECT COUNT(*)"
		       " FROM mch_results"
		      " WHERE MchCod=%ld"
		        " AND UsrCod=%ld",
		      Print->MchCod,Print->UsrCod))	// Match print exists
      /* Update result */
      DB_QueryUPDATE ("can not update match result",
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
		       Print->MchCod,Print->UsrCod);
   else							// Match print doesn't exist
      /* Create result */
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
		       Print->MchCod,Print->UsrCod,
		       Print->NumQsts.All,
		       Print->NumQsts.NotBlank,
		       Print->Score);
   Str_SetDecimalPointToLocal ();	// Return to local system
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
   if (DB_QuerySELECT (&mysql_res,"can not get data of a match print",
		       "SELECT UNIX_TIMESTAMP(mch_results.StartTime),"	// row[1]
			      "UNIX_TIMESTAMP(mch_results.EndTime),"	// row[2]
		              "mch_results.NumQsts,"			// row[3]
		              "mch_results.NumQstsNotBlank,"		// row[4]
		              "mch_results.Score"			// row[5]
		        " FROM mch_results,"
		              "mch_matches,"
		              "gam_games"
		       " WHERE mch_results.MchCod=%ld"
		         " AND mch_results.UsrCod=%ld"
		         " AND mch_results.MchCod=mch_matches.MchCod"
		         " AND mch_matches.GamCod=gam_games.GamCod"
		         " AND gam_games.CrsCod=%ld",	// Extra check
		       Print->MchCod,Print->UsrCod,
		       Gbl.Hierarchy.Crs.CrsCod) == 1)
     {
      row = mysql_fetch_row (mysql_res);

      /* Get start time (row[0] and row[1] hold UTC date-times) */
      for (StartEndTime = (Dat_StartEndTime_t) 0;
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
