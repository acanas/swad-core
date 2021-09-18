// swad_game_database.c: games using remote control, operations with database

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
// #include <float.h>		// For DBL_MAX
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For free
// #include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_form.h"
#include "swad_game.h"
#include "swad_game_database.h"
#include "swad_global.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_match.h"
// #include "swad_match_result.h"
// #include "swad_pagination.h"
// #include "swad_role.h"
// #include "swad_test.h"
// #include "swad_test_visibility.h"

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
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************* Get list of all the games in the current course ***************/
/*****************************************************************************/

unsigned Gam_DB_GetListGames (MYSQL_RES **mysql_res,Gam_Order_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Gam_NUM_ORDERS] =
     {
      [Gam_ORDER_BY_START_DATE] = "StartTime DESC,"
	                          "EndTime DESC,"
	                          "gam_games.Title DESC",
      [Gam_ORDER_BY_END_DATE  ] = "EndTime DESC,"
	                          "StartTime DESC,"
	                          "gam_games.Title DESC",
      [Gam_ORDER_BY_TITLE     ] = "gam_games.Title",
     };
   ;
   char *HiddenSubQuery;
   unsigned NumGames;

   /***** Subquery: get hidden games depending on user's role *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
         if (asprintf (&HiddenSubQuery," AND gam_games.Hidden='N'") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
	 if (asprintf (&HiddenSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      default:
	 Err_WrongRoleExit ();
	 break;
     }

   /***** Get list of games from database *****/
   NumGames = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get games",
		   "SELECT gam_games.GamCod,"				// row[0]
			  "MIN(mch_matches.StartTime) AS StartTime,"	// row[1]
			  "MAX(mch_matches.EndTime) AS EndTime"		// row[2]
		    " FROM gam_games"
		    " LEFT JOIN mch_matches"
		      " ON gam_games.GamCod=mch_matches.GamCod"
		   " WHERE gam_games.CrsCod=%ld"
		      "%s"
		   " GROUP BY gam_games.GamCod"
		   " ORDER BY %s",
		   Gbl.Hierarchy.Crs.CrsCod,
		   HiddenSubQuery,
		   OrderBySubQuery[SelectedOrder]);

   /***** Free allocated memory for subquery *****/
   free (HiddenSubQuery);

   return NumGames;
  }

/*****************************************************************************/
/********************** Get game data using its code *************************/
/*****************************************************************************/

unsigned Gam_DB_GetDataOfGameByCod (MYSQL_RES **mysql_res,long GamCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get game data",
		   "SELECT gam_games.GamCod,"		// row[0]
			  "gam_games.CrsCod,"		// row[1]
			  "gam_games.Hidden,"		// row[2]
			  "gam_games.UsrCod,"		// row[3]
			  "gam_games.MaxGrade,"		// row[4]
			  "gam_games.Visibility,"	// row[5]
			  "gam_games.Title"		// row[6]
		    " FROM gam_games"
		    " LEFT JOIN mch_matches"
		      " ON gam_games.GamCod=mch_matches.GamCod"
		   " WHERE gam_games.GamCod=%ld"
		     " AND gam_games.CrsCod='%ld'",	// Extra check
		   GamCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******************* Get number of questions of a game *********************/
/*****************************************************************************/

unsigned Gam_DB_GetNumQstsGame (long GamCod)
  {
   /***** Get nuumber of questions in a game from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of questions of a game",
		  "SELECT COUNT(*)"
		   " FROM gam_questions"
		  " WHERE GamCod=%ld",
		  GamCod);
  }

/*****************************************************************************/
/*********** Get previous question index to a given index in a game **********/
/*****************************************************************************/
// Input question index can be 1, 2, 3... n-1
// Return question index will be 1, 2, 3... n if previous question exists, or 0 if no previous question

unsigned Gam_DB_GetPrevQuestionIndexInGame (long GamCod,unsigned QstInd)
  {
   /***** Get previous question index in a game from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   return DB_QuerySELECTUnsigned ("can not get previous question index",
				  "SELECT COALESCE(MAX(QstInd),0)"
				   " FROM gam_questions"
				  " WHERE GamCod=%ld"
				    " AND QstInd<%u",
				  GamCod,
				  QstInd);
  }

/*****************************************************************************/
/************* Get next question index to a given index in a game ************/
/*****************************************************************************/
// Input question index can be 0, 1, 2, 3... n-1
// Return question index will be 1, 2, 3... n if next question exists, or big number if no next question

unsigned Gam_DB_GetNextQuestionIndexInGame (long GamCod,unsigned QstInd)
  {
   /***** Get next question index in a game from database *****/
   // Although indexes are always continuous...
   // ...this implementation works even with non continuous indexes
   return DB_QuerySELECTUnsigned ("can not get next question index",
				  "SELECT COALESCE(MIN(QstInd),%u)"
				   " FROM gam_questions"
				  " WHERE GamCod=%ld"
				    " AND QstInd>%u",
				  Gam_AFTER_LAST_QUESTION,	// End of questions has been reached
				  GamCod,
				  QstInd);
  }

/*****************************************************************************/
/********************* Get number of courses with games **********************/
/*****************************************************************************/
// Returns the number of courses with games in this location

unsigned Gam_DB_GetNumCoursesWithGames (HieLvl_Level_t Scope)
  {
   /***** Get number of courses with games from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with games",
			 "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM gam_games");
      case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "gam_games"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "gam_games"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "gam_games"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with games",
			 "SELECT COUNT(DISTINCT gam_games.CrsCod)"
			  " FROM crs_courses,"
			        "gam_games"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with games",
			 "SELECT COUNT(DISTINCT CrsCod)"
			  " FROM gam_games"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/**************************** Get number of games ****************************/
/*****************************************************************************/
// Returns the number of games in this location

unsigned Gam_DB_GetNumGames (HieLvl_Level_t Scope)
  {
   /***** Get number of games from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of games",
                         "SELECT COUNT(*)"
			  " FROM gam_games");
      case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of games",
                         "SELECT COUNT(*)"
			  " FROM ins_instits,"
			        "ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "gam_games"
			 " WHERE ins_instits.CtyCod=%ld"
			   " AND ins_instits.InsCod=ctr_centers.InsCod"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Cty.CtyCod);
      case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of games",
                         "SELECT COUNT(*)"
			  " FROM ctr_centers,"
			        "deg_degrees,"
			        "crs_courses,"
			        "gam_games"
			 " WHERE ctr_centers.InsCod=%ld"
			   " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of games",
                         "SELECT COUNT(*)"
			  " FROM deg_degrees,"
			        "crs_courses,"
			        "gam_games"
			 " WHERE deg_degrees.CtrCod=%ld"
			   " AND deg_degrees.DegCod=crs_courses.DegCod"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of games",
                         "SELECT COUNT(*)"
			  " FROM crs_courses,"
			        "gam_games"
			 " WHERE crs_courses.DegCod=%ld"
			   " AND crs_courses.CrsCod=gam_games.CrsCod",
		         Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of games",
                         "SELECT COUNT(*)"
			  " FROM gam_games"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/***************** Get average number of questions per game ******************/
/*****************************************************************************/

double Gam_DB_GetNumQstsPerGame (HieLvl_Level_t Scope)
  {
   /***** Get number of questions per game from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
         return
         DB_QuerySELECTDouble ("can not get number of questions per game",
			       "SELECT AVG(NumQsts)"
			        " FROM (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
				        " FROM gam_games,"
					      "gam_questions"
				       " WHERE gam_games.GamCod=gam_questions.GamCod"
				       " GROUP BY gam_questions.GamCod) AS NumQstsTable");
      case HieLvl_CTY:
         return
         DB_QuerySELECTDouble ("can not get number of questions per game",
			       "SELECT AVG(NumQsts)"
			       " FROM (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
				       " FROM ins_instits,"
					     "ctr_centers,"
					     "deg_degrees,"
					     "crs_courses,"
					     "gam_games,"
					     "gam_questions"
				      " WHERE ins_instits.CtyCod=%ld"
				        " AND ins_instits.InsCod=ctr_centers.InsCod"
				        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				        " AND deg_degrees.DegCod=crs_courses.DegCod"
				        " AND crs_courses.CrsCod=gam_games.CrsCod"
				        " AND gam_games.GamCod=gam_questions.GamCod"
				      " GROUP BY gam_questions.GamCod) AS NumQstsTable",
			       Gbl.Hierarchy.Cty.CtyCod);
      case HieLvl_INS:
         return
         DB_QuerySELECTDouble ("can not get number of questions per game",
			       "SELECT AVG(NumQsts)"
			       " FROM (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
				       " FROM ctr_centers,"
					     "deg_degrees,"
					     "crs_courses,"
					     "gam_games,"
					     "gam_questions"
				      " WHERE ctr_centers.InsCod=%ld"
				        " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
				        " AND deg_degrees.DegCod=crs_courses.DegCod"
				        " AND crs_courses.CrsCod=gam_games.CrsCod"
				        " AND gam_games.GamCod=gam_questions.GamCod"
				     " GROUP BY gam_questions.GamCod) AS NumQstsTable",
			       Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return
         DB_QuerySELECTDouble ("can not get number of questions per game",
			       "SELECT AVG(NumQsts)"
			        " FROM (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
				        " FROM deg_degrees,"
					      "crs_courses,"
					      "gam_games,"
					      "gam_questions"
				       " WHERE deg_degrees.CtrCod=%ld"
				         " AND deg_degrees.DegCod=crs_courses.DegCod"
				         " AND crs_courses.CrsCod=gam_games.CrsCod"
				         " AND gam_games.GamCod=gam_questions.GamCod"
				       " GROUP BY gam_questions.GamCod) AS NumQstsTable",
			       Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return
         DB_QuerySELECTDouble ("can not get number of questions per game",
			       "SELECT AVG(NumQsts)"
			        " FROM (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
				        " FROM crs_courses,"
					      "gam_games,"
					      "gam_questions"
				       " WHERE crs_courses.DegCod=%ld"
				         " AND crs_courses.CrsCod=gam_games.CrsCod"
				         " AND gam_games.GamCod=gam_questions.GamCod"
				       " GROUP BY gam_questions.GamCod) AS NumQstsTable",
			       Gbl.Hierarchy.Deg.DegCod);
      case HieLvl_CRS:
         return
         DB_QuerySELECTDouble ("can not get number of questions per game",
			       "SELECT AVG(NumQsts)"
			        " FROM (SELECT COUNT(gam_questions.QstCod) AS NumQsts"
				        " FROM gam_games,"
					      "gam_questions"
				       " WHERE gam_games.Cod=%ld"
				         " AND gam_games.GamCod=gam_questions.GamCod"
				       " GROUP BY gam_questions.GamCod) AS NumQstsTable",
			       Gbl.Hierarchy.Crs.CrsCod);
      default:
	 Err_WrongScopeExit ();
	 return 0.0;	// Not reached
     }
  }
