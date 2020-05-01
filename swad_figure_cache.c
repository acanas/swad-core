// swad_figure_cache.c: figures (global stats) cached in database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

//#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For sscanf
//#include <stdlib.h>		// For system, getenv, etc.
//#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_figure_cache.h"
#include "swad_scope.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

// extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define FigCch_TIME_CACHE	((time_t)(1UL * 60UL * 60UL))	// Past these seconds, update cached value

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private prototypes ***************************/
/*****************************************************************************/

/*****************************************************************************/
/************** Get number of users who have chosen an option ****************/
/*****************************************************************************/

void FigCch_UpdateFigureIntoCache (FigCch_FigureCached_t Figure,
                                   Hie_Level_t Scope,long Cod,
                                   FigCch_Type_t Type,const void *ValuePtr)
  {
   /***** Trivial check *****/
   if (Figure == FigCch_UNKNOWN)
      return;

   /***** Update figure's value in database *****/
   switch (Type)
     {
      case FigCch_Type_UNSIGNED:
	 DB_QueryREPLACE ("can not update cached figure value",
			  "REPLACE INTO figures"
			  " (Figure,Scope,Cod,ValueInt,ValueDouble)"
			  " VALUES"
			  " (%u,'%s',%ld,%u,'0.0')",
			  (unsigned) Figure,Sco_GetDBStrFromScope (Scope),Cod,
			  *((unsigned *) ValuePtr));
	 break;
      case FigCch_Type_DOUBLE:
	 DB_QueryREPLACE ("can not update cached figure value",
			  "REPLACE INTO figures"
			  " (Figure,Scope,Cod,ValueInt,ValueDouble)"
			  " VALUES"
			  " (%u,'%s',%ld,0,'%.15lg')",
			  (unsigned) Figure,Sco_GetDBStrFromScope (Scope),Cod,
			  *((double *) ValuePtr));
	 break;
     }
  }

/*****************************************************************************/
/************** Get number of users who have chosen an option ****************/
/*****************************************************************************/
// Return true is figure is found (if figure is cached and recently updated)

bool FigCch_GetFigureFromCache (FigCch_FigureCached_t Figure,
                                Hie_Level_t Scope,long Cod,
                                FigCch_Type_t Type,void *ValuePtr)
  {
   static const char *Field[FigCch_NUM_TYPES] =
     {
      [FigCch_Type_UNSIGNED] = "ValueInt",
      [FigCch_Type_DOUBLE  ] = "ValueDouble",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Found = false;

   /***** Set default value when not found *****/
   switch (Type)
     {
      case FigCch_Type_UNSIGNED:
         *((unsigned *) ValuePtr) = 0;
	 break;
      case FigCch_Type_DOUBLE:
         *((double *) ValuePtr) = 0.0;
	 break;
     }

   /***** Trivial check *****/
   if (Figure == FigCch_UNKNOWN)
      return Found;

   /***** Get figure's value if cached and recent *****/
   if (DB_QuerySELECT (&mysql_res,"can not get cached figure value",
		       "SELECT %s"
		       " FROM figures"
		       " WHERE Figure=%u AND Scope='%s' AND Cod=%ld"
		       " AND LastUpdate>FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		       Field[Type],
		       (unsigned) Figure,Sco_GetDBStrFromScope (Scope),Cod,
		       FigCch_TIME_CACHE))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get value (row[0]) */
      if (row[0])
	{
	 switch (Type)
	   {
	    case FigCch_Type_UNSIGNED:
	       if (sscanf (row[0],"%u",(unsigned *) ValuePtr) == 1)
		  Found = true;
	       break;
	    case FigCch_Type_DOUBLE:
	       if (sscanf (row[0],"%lf",(double *) ValuePtr) == 1)
		  Found = true;
	       break;
	   }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }
