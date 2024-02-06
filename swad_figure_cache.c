// swad_figure_cache.c: figures (global stats) cached in database

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

#include <stdio.h>		// For sscanf

#include "swad_alert.h"
#include "swad_database.h"
#include "swad_figure_cache.h"
#include "swad_figure_database.h"
#include "swad_scope.h"
#include "swad_string.h"

/*****************************************************************************/
/*************************** Update figure into cache ************************/
/*****************************************************************************/

void FigCch_UpdateFigureIntoCache (FigCch_FigureCached_t Figure,
                                   Hie_Level_t Level,long HieCod,
                                   FigCch_Type_t Type,const void *ValuePtr)
  {
   /***** Trivial check *****/
   if (Figure == FigCch_UNKNOWN)
      return;

   /***** Update figure's value in database *****/
   switch (Type)
     {
      case FigCch_UNSIGNED:
	 Fig_DB_UpdateUnsignedFigureIntoCache (Figure,Level,HieCod,
	                                      *((unsigned *) ValuePtr));
	 break;
      case FigCch_DOUBLE:
	 Fig_DB_UpdateDoubleFigureIntoCache (Figure,Level,HieCod,
			                     *((double *) ValuePtr));
	 break;
     }
  }

/*****************************************************************************/
/************************** Get figure from cache ****************************/
/*****************************************************************************/
// Return true is figure is found (if figure is cached and recently updated)

bool FigCch_GetFigureFromCache (FigCch_FigureCached_t Figure,
                                Hie_Level_t Level,long HieCod,
                                FigCch_Type_t Type,void *ValuePtr)
  {
   /* The higher the level, the longer a value remains cached */
   time_t TimeCached[Hie_NUM_LEVELS] =	// Time in seconds
     {
      [Hie_UNK] = (time_t) (                 0),	// Unknown
      [Hie_SYS] = (time_t) (24UL * 60UL * 60UL),	// System
      [Hie_CTY] = (time_t) (12UL * 60UL * 60UL),	// Country
      [Hie_INS] = (time_t) ( 6UL * 60UL * 60UL),	// Institution
      [Hie_CTR] = (time_t) ( 3UL * 60UL * 60UL),	// Center
      [Hie_DEG] = (time_t) ( 1UL * 60UL * 60UL),	// Degree
      [Hie_CRS] = (time_t) (              60UL),	// Course
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Found = false;

   /***** Set default value when not found *****/
   switch (Type)
     {
      case FigCch_UNSIGNED:
         *((unsigned *) ValuePtr) = 0;
	 break;
      case FigCch_DOUBLE:
         *((double *) ValuePtr) = 0.0;
	 break;
     }

   /***** Trivial check *****/
   if (Figure == FigCch_UNKNOWN ||	// Unknown figure
       Level == Hie_UNK)		// Unknown scope
      return false;

   /***** Get figure's value if cached and recent *****/
   if (Fig_DB_GetFigureFromCache (&mysql_res,Figure,Level,HieCod,Type,TimeCached[Level]))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get value (row[0]) */
      if (row[0])
	{
	 switch (Type)
	   {
	    case FigCch_UNSIGNED:
	       if (sscanf (row[0],"%u",(unsigned *) ValuePtr) == 1)
		  Found = true;
	       break;
	    case FigCch_DOUBLE:
               Str_SetDecimalPointToUS ();	// To write the decimal point as a dot
	       if (sscanf (row[0],"%lf",(double *) ValuePtr) == 1)
		  Found = true;
               Str_SetDecimalPointToLocal ();	// Return to local system
	       break;
	   }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }
