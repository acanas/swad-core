// swad_MFU_database.c: Most Frequently Used actions, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_database.h"
#include "swad_global.h"
#include "swad_MFU.h"
#include "swad_MFU_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************** Update score for the current action ********************/
/*****************************************************************************/

void MFU_DB_UpdateScoreForCurrentAction (long ActCod,double Score)
  {
   DB_QueryREPLACE ("can not update most frequently used actions",
		    "REPLACE INTO act_frequent"
		    " (UsrCod,ActCod,Score,LastClick)"
		    " VALUES"
		    " (%ld,%ld,'%15lg',NOW())",
	            Gbl.Usrs.Me.UsrDat.UsrCod,
	            ActCod,
	            Score);
  }

/*****************************************************************************/
/********************** Update score for other actions ***********************/
/*****************************************************************************/

void MFU_DB_UpdateScoreForOtherActions (long ActCod)
  {
   DB_QueryUPDATE ("can not update most frequently used actions",
		   "UPDATE act_frequent"
		     " SET Score=GREATEST(Score*'%.15lg','%.15lg')"
		   " WHERE UsrCod=%ld"
		     " AND ActCod<>%ld",
                   MFU_DECREASE_FACTOR,
                   MFU_MIN_SCORE,
                   Gbl.Usrs.Me.UsrDat.UsrCod,
                   ActCod);
  }

/*****************************************************************************/
/******************** Get most frequently used actions ***********************/
/*****************************************************************************/

unsigned MFU_DB_GetMFUActionsOrderByScore (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get most frequently used actions",
		   "SELECT ActCod"
		    " FROM act_frequent"
		   " WHERE UsrCod=%ld"
		   " ORDER BY Score DESC,"
			     "LastClick DESC",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Get most frequently used actions ***********************/
/*****************************************************************************/

unsigned MFU_DB_GetMFUActionsOrderByLastClick (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the most frequently used actions",
		   "SELECT ActCod"
		    " FROM act_frequent"
		   " WHERE UsrCod=%ld"
		   " ORDER BY LastClick DESC,"
			     "Score DESC",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************ Get score of current action ************************/
/*****************************************************************************/

unsigned MFU_DB_GetScoreForCurrentAction (MYSQL_RES **mysql_res,long ActCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get score for current action",
		   "SELECT Score"	// row[0]
		    " FROM act_frequent"
		   " WHERE UsrCod=%ld"
		     " AND ActCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   ActCod);
  }

