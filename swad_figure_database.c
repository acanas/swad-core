// swad_figure_database.c: figures (global stats) operations with database

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

#include "swad_alert.h"
#include "swad_database.h"
#include "swad_figure_database.h"
#include "swad_hierarchy.h"

/*****************************************************************************/
/****************** Update unsigned figure value into cache ******************/
/*****************************************************************************/

void Fig_DB_UpdateUnsignedFigureIntoCache (FigCch_FigureCached_t Figure,
                                           Hie_Level_t HieLvl,long HieCod,
                                           unsigned Value)
  {
   DB_QueryREPLACE ("can not update cached figure value",
		    "REPLACE INTO fig_figures"
		    " (Figure,Scope,Cod,ValueInt,ValueDouble)"
		    " VALUES"
		    " (%u,'%s',%ld,%u,'0.0')",
		    (unsigned) Figure,
		    Hie_GetDBStrFromLevel (HieLvl),
		    HieCod,
		    Value);
  }

/*****************************************************************************/
/******************* Update double figure value into cache *******************/
/*****************************************************************************/

void Fig_DB_UpdateDoubleFigureIntoCache (FigCch_FigureCached_t Figure,
                                         Hie_Level_t HieLvl,long HieCod,
                                         double Value)
  {
   Str_SetDecimalPointToUS ();	// To write the decimal point as a dot
   DB_QueryREPLACE ("can not update cached figure value",
		    "REPLACE INTO fig_figures"
		    " (Figure,Scope,Cod,ValueInt,ValueDouble)"
		    " VALUES"
		    " (%u,'%s',%ld,0,'%.15lg')",
		    (unsigned) Figure,
		    Hie_GetDBStrFromLevel (HieLvl),
		    HieCod,
		    Value);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************************** Get figure from cache ****************************/
/*****************************************************************************/

unsigned Fig_DB_GetFigureFromCache (MYSQL_RES **mysql_res,
                                    FigCch_FigureCached_t Figure,
                                    Hie_Level_t HieLvl,long HieCod,
                                    FigCch_Type_t Type,time_t TimeCached)
  {
   static const char *Field[FigCch_NUM_TYPES] =
     {
      [FigCch_UNSIGNED] = "ValueInt",
      [FigCch_DOUBLE  ] = "ValueDouble",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get cached figure value",
		   "SELECT %s"		// row[0]
		    " FROM fig_figures"
		   " WHERE Figure=%u"
		     " AND Scope='%s'"
		     " AND Cod=%ld"
		     " AND LastUpdate>FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
		   Field[Type],
		   (unsigned) Figure,Hie_GetDBStrFromLevel (HieLvl),HieCod,
		   TimeCached);
  }
