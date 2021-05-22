// swad_country_database.h: countries operations with database

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
// #include <stdbool.h>		// For boolean type
// #include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For free
// #include <string.h>		// For string functions

#include "swad_country_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"

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
/********** Get basic list of countries ordered by name of country ***********/
/*****************************************************************************/

unsigned Cty_DB_GetBasicListOfCountries (MYSQL_RES **mysql_res)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get countries",
		   "SELECT CtyCod,"	// row[0]
			  "Name_%s"	// row[1]
		    " FROM cty_countrs"
		   " ORDER BY Name_%s",
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
  }

/*****************************************************************************/
/******************* Get countries with pending institutions *****************/
/*****************************************************************************/

unsigned Cty_DB_GetListOfCountriesWithPendingInss (MYSQL_RES **mysql_res)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get countries"
			     " with pending institutions",
		   "SELECT ins_instits.CtyCod,"	// row[0]
			  "COUNT(*)"		// row[1]
		    " FROM ins_instits,"
			  "cty_countrs"
		   " WHERE (ins_instits.Status & %u)<>0"
		     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		   " GROUP BY ins_instits.CtyCod"
		   " ORDER BY cty_countrs.Name_%s",
		   (unsigned) Ins_STATUS_BIT_PENDING,
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
  }

/*****************************************************************************/
/********** Get full list of countries with names in all languages ***********/
/********** and number of users who claim to belong to them        ***********/
/*****************************************************************************/

#define Cty_MAX_BYTES_SUBQUERY_CTYS	((1 + Lan_NUM_LANGUAGES) * 32)

unsigned Cty_DB_GetFullListOfCountries (MYSQL_RES **mysql_res)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char StrField[32];
   char SubQueryNam1[Cty_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryNam2[Cty_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryWWW1[Cty_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryWWW2[Cty_MAX_BYTES_SUBQUERY_CTYS + 1];
   char *OrderBySubQuery = NULL;
   static const char *OrderBySubQueryFmt[Cty_NUM_ORDERS] =
     {
      [Cty_ORDER_BY_COUNTRY ] = "Name_%s",
      [Cty_ORDER_BY_NUM_USRS] = "NumUsrs DESC,Name_%s",
     };
   unsigned NumCtys;
   Lan_Language_t Lan;

   /***** Get countries from database *****/
   SubQueryNam1[0] = '\0';
   SubQueryNam2[0] = '\0';
   SubQueryWWW1[0] = '\0';
   SubQueryWWW2[0] = '\0';
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      snprintf (StrField,sizeof (StrField),"cty_countrs.Name_%s,",Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryNam1,StrField,sizeof (SubQueryNam1) - 1);
      snprintf (StrField,sizeof (StrField),"Name_%s,",Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryNam2,StrField,sizeof (SubQueryNam2) - 1);

      snprintf (StrField,sizeof (StrField),"cty_countrs.WWW_%s,",Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryWWW1,StrField,sizeof (SubQueryWWW1) - 1);
      snprintf (StrField,sizeof (StrField),"WWW_%s,",Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryWWW2,StrField,sizeof (SubQueryWWW2) - 1);
     }

   /* Build order subquery */
   if (asprintf (&OrderBySubQuery,OrderBySubQueryFmt[Gbl.Hierarchy.Ctys.SelectedOrder],
		 Lan_STR_LANG_ID[Gbl.Prefs.Language]) < 0)
      Err_NotEnoughMemoryExit ();

   /* Query database */
   NumCtys = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get countries",
		   "(SELECT cty_countrs.CtyCod,"	// row[0]
			   "cty_countrs.Alpha2,"	// row[1]
			   "%s"				// row[...]
			   "%s"				// row[...]
			   "COUNT(*) AS NumUsrs"	// row[...]
		    " FROM cty_countrs,"
			  "usr_data"
		   " WHERE cty_countrs.CtyCod=usr_data.CtyCod"
		   " GROUP BY cty_countrs.CtyCod)"
		   " UNION "
		   "(SELECT CtyCod,"			// row[0]
			   "Alpha2,"			// row[1]
			   "%s"				// row[...]
			   "%s"				// row[...]
			   "0 AS NumUsrs"		// row[...]
		    " FROM cty_countrs"
		   " WHERE CtyCod NOT IN"
		         " (SELECT DISTINCT CtyCod"
			    " FROM usr_data))"
		   " ORDER BY %s",
		   SubQueryNam1,SubQueryWWW1,
		   SubQueryNam2,SubQueryWWW2,OrderBySubQuery);

   /* Free memory for subquery */
   free (OrderBySubQuery);

   return NumCtys;
  }

/*****************************************************************************/
/*********** Get average coordinates of centers in current country ***********/
/*****************************************************************************/

void Cty_DB_GetCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom)
  {
   char *Query;

   /***** Get average coordinates of centers of current country
          with both coordinates set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   if (asprintf (&Query,
		 "SELECT AVG(ctr_centers.Latitude),"						// row[0]
			"AVG(ctr_centers.Longitude),"						// row[1]
			"GREATEST(MAX(ctr_centers.Latitude)-MIN(ctr_centers.Latitude),"
				 "MAX(ctr_centers.Longitude)-MIN(ctr_centers.Longitude))"	// row[2]
		  " FROM ins_instits,"
		        "ctr_centers"
		 " WHERE ins_instits.CtyCod=%ld"
		   " AND ins_instits.InsCod=ctr_centers.InsCod"
		   " AND ctr_centers.Latitude<>0"
		   " AND ctr_centers.Longitude<>0",
		 Gbl.Hierarchy.Cty.CtyCod) < 0)
      Err_NotEnoughMemoryExit ();
   Map_GetCoordAndZoom (Coord,Zoom,Query);
   free (Query);
  }

/*****************************************************************************/
/*********** Get centres which have coordinates in current country ***********/
/*****************************************************************************/

unsigned Cty_DB_GetCtrsWithCoordsInCurrentCty (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get centers with coordinates",
		   "SELECT ctr_centers.CtrCod"
		    " FROM ins_instits,"
			  "ctr_centers"
		   " WHERE ins_instits.CtyCod=%ld"
		     " AND ins_instits.InsCod=ctr_centers.InsCod"
		     " AND ctr_centers.Latitude<>0"
		     " AND ctr_centers.Longitude<>0",
		   Gbl.Hierarchy.Cty.CtyCod);
  }

/*****************************************************************************/
/******************** Get map attribution from database **********************/
/*****************************************************************************/

unsigned Cty_DB_GetMapAttr (MYSQL_RES **mysql_res,long CtyCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get map attribution",
		   "SELECT MapAttribution"	// row[0]
		    " FROM cty_countrs"
		   " WHERE CtyCod=%ld",
		   CtyCod);
  }

/*****************************************************************************/
/*********** Update the attribution of the map of current country ************/
/*****************************************************************************/

void Cty_DB_UpdateCtyMapAttr (const char NewMapAttribution[Med_MAX_BYTES_ATTRIBUTION + 1])
  {
   DB_QueryUPDATE ("can not update the map attribution",
		   "UPDATE cty_countrs"
		     " SET MapAttribution='%s'"
		   " WHERE CtyCod='%03ld'",
	           NewMapAttribution,
	           Gbl.Hierarchy.Cty.CtyCod);
  }

