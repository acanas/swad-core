// swad_country_database.h: countries operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_country_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_database.h"
#include "swad_search.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Cty_DB_MAX_BYTES_SUBQUERY_CTYS		((1 + Lan_NUM_LANGUAGES) * 32)
#define Cty_DB_MAX_BYTES_SUBQUERY_CTYS_NAME	((1 + Lan_NUM_LANGUAGES) * Cty_MAX_BYTES_NAME)
#define Cty_DB_MAX_BYTES_SUBQUERY_CTYS_WWW	((1 + Lan_NUM_LANGUAGES) * Cns_MAX_BYTES_WWW)

/*****************************************************************************/
/**************************** Create a new country ***************************/
/*****************************************************************************/

void Cty_DB_CreateCountry (const struct Hie_Node *Cty,
			   char NameInSeveralLanguages[1 + Lan_NUM_LANGUAGES][Cty_MAX_BYTES_NAME + 1],
			   char WWWInSeveralLanguages [1 + Lan_NUM_LANGUAGES][Cns_MAX_BYTES_WWW + 1])
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t Lan;
   char StrField[32];
   char SubQueryNam1[Cty_DB_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryNam2[Cty_DB_MAX_BYTES_SUBQUERY_CTYS_NAME + 1];
   char SubQueryWWW1[Cty_DB_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryWWW2[Cty_DB_MAX_BYTES_SUBQUERY_CTYS_WWW + 1];

   /***** Create a new country *****/
   SubQueryNam1[0] = '\0';
   SubQueryNam2[0] = '\0';
   SubQueryWWW1[0] = '\0';
   SubQueryWWW2[0] = '\0';
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      snprintf (StrField,sizeof (StrField),",Name_%s",Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryNam1,StrField,sizeof (SubQueryNam1) - 1);

      Str_Concat (SubQueryNam2,",'",sizeof (SubQueryNam2) - 1);
      Str_Concat (SubQueryNam2,NameInSeveralLanguages[Lan],sizeof (SubQueryNam2) - 1);
      Str_Concat (SubQueryNam2,"'",sizeof (SubQueryNam2) - 1);

      snprintf (StrField,sizeof (StrField),",WWW_%s",Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryWWW1,StrField,sizeof (SubQueryWWW1) - 1);

      Str_Concat (SubQueryWWW2,",'",sizeof (SubQueryWWW2) - 1);
      Str_Concat (SubQueryWWW2,WWWInSeveralLanguages[Lan],sizeof (SubQueryWWW2) - 1);
      Str_Concat (SubQueryWWW2,"'",sizeof (SubQueryWWW2) - 1);
     }
   DB_QueryINSERT ("can not create country",
		   "INSERT INTO cty_countrs"
		   " (CtyCod,Alpha2,MapAttribution%s%s)"
		   " VALUES"
		   " ('%03ld','%s',''%s%s)",
                   SubQueryNam1,
                   SubQueryWWW1,
                   Cty->HieCod,
                   Cty->ShrtName,
		   SubQueryNam2,
		   SubQueryWWW2);
  }

/*****************************************************************************/
/********** Get basic list of countries ordered by name of country ***********/
/*****************************************************************************/

unsigned Cty_DB_GetCtysBasic (MYSQL_RES **mysql_res)
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

unsigned Cty_DB_GetCtysWithPendingInss (MYSQL_RES **mysql_res)
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
		   (unsigned) Hie_STATUS_BIT_PENDING,
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
  }

/*****************************************************************************/
/********** Get full list of countries with names in all languages ***********/
/********** and number of users who claim to belong to them        ***********/
/*****************************************************************************/

unsigned Cty_DB_GetCtysFull (MYSQL_RES **mysql_res)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char *OrderBySubQuery = NULL;
   static const char *OrderBySubQueryFmt[Hie_NUM_ORDERS] =
     {
      [Hie_ORDER_BY_NAME    ] = "Name_%s",
      [Hie_ORDER_BY_NUM_USRS] = "NumUsrs DESC,"
	                        "Name_%s",
     };
   unsigned NumCtys;

   /***** Get countries from database *****/
   /* Build order subquery */
   if (asprintf (&OrderBySubQuery,
		 OrderBySubQueryFmt[Gbl.Hierarchy.List[Hie_SYS].SelectedOrder],
		 Lan_STR_LANG_ID[Gbl.Prefs.Language]) < 0)
      Err_NotEnoughMemoryExit ();

   /* Query database */
   NumCtys = (unsigned)
   DB_QuerySELECT (mysql_res,"can not get countries",
		   "(SELECT cty_countrs.CtyCod,"	// row[0]
			   "cty_countrs.Alpha2,"	// row[1]
			   "cty_countrs.Name_%s,"	// row[2]
			   "cty_countrs.WWW_%s,"	// row[3]
			   "COUNT(*) AS NumUsrs"	// row[4]
		    " FROM cty_countrs,"
			  "usr_data"
		   " WHERE cty_countrs.CtyCod=usr_data.CtyCod"
		" GROUP BY cty_countrs.CtyCod)"
		   " UNION "
		   "(SELECT CtyCod,"			// row[0]
			   "Alpha2,"			// row[1]
			   "cty_countrs.Name_%s,"	// row[2]
			   "cty_countrs.WWW_%s,"	// row[3]
			   "0 AS NumUsrs"		// row[4]
		    " FROM cty_countrs"
		   " WHERE CtyCod NOT IN"
		         " (SELECT DISTINCT "
		                  "CtyCod"
			    " FROM usr_data))"
		" ORDER BY %s",
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   OrderBySubQuery);

   /* Free memory for subquery */
   free (OrderBySubQuery);

   return NumCtys;
  }

/*****************************************************************************/
/***************** Get basic data of country given its code ******************/
/*****************************************************************************/

unsigned Cty_DB_GetBasicCountryDataByCod (MYSQL_RES **mysql_res,long CtyCod)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a country",
		   "SELECT Alpha2,"	// row[0]
			  "Name_%s,"	// row[1]
			  "WWW_%s"	// row[2]
		    " FROM cty_countrs"
		   " WHERE CtyCod='%03ld'",
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   Lan_STR_LANG_ID[Gbl.Prefs.Language],
		   CtyCod);
  }

/*****************************************************************************/
/*********** Get all names and WWWs of a country given its code **************/
/*****************************************************************************/

unsigned Cty_DB_GetNamesAndWWWsByCod (MYSQL_RES **mysql_res,long CtyCod)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char StrField[32];
   char SubQueryNam[Cty_DB_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryWWW[Cty_DB_MAX_BYTES_SUBQUERY_CTYS + 1];
   Lan_Language_t Lan;

   /***** Get countries from database *****/
   SubQueryNam[0] = '\0';
   SubQueryWWW[0] = '\0';
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      snprintf (StrField,sizeof (StrField),"Name_%s,",Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryNam,StrField,sizeof (SubQueryNam) - 1);

      snprintf (StrField,sizeof (StrField),
	        Lan == Lan_NUM_LANGUAGES ? "WWW_%s" :
					   "WWW_%s,",
	        Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryWWW,StrField,sizeof (SubQueryWWW) - 1);
     }

   /* Query database */
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get countries",
		   "SELECT %s"		// row[...]
			   "%s"		// row[...]
		    " FROM cty_countrs"
	           " WHERE CtyCod='%03ld'",
		   SubQueryNam,SubQueryWWW,
		   CtyCod);
  }

/*****************************************************************************/
/******************* Get name of a country in a language *********************/
/*****************************************************************************/

void Cty_DB_GetCountryName (long CtyCod,Lan_Language_t Language,
			    char CtyName[Cty_MAX_BYTES_NAME + 1])
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   DB_QuerySELECTString (CtyName,Cty_MAX_BYTES_NAME,
                         "can not get the name of a country",
		         "SELECT Name_%s"
		          " FROM cty_countrs"
		         " WHERE CtyCod='%03ld'",
	                 Lan_STR_LANG_ID[Language],
	                 CtyCod);
  }

/*****************************************************************************/
/******************** Get number of countries in system **********************/
/*****************************************************************************/

unsigned Cty_DB_GetNumCtysInSys (__attribute__((unused)) long SysCod)
  {
   return (unsigned) DB_GetNumRowsTable ("cty_countrs");
  }

/*****************************************************************************/
/***************** Get number of countries with institutions *****************/
/*****************************************************************************/

unsigned Cty_DB_GetNumCtysWithInss (Hie_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of countries with institutions",
		  "SELECT COUNT(DISTINCT cty_countrs.CtyCod)"
		   " FROM cty_countrs,"
			 "ins_instits"
		  " WHERE %s"
		         "cty_countrs.CtyCod=ins_instits.CtyCod",
		  SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with centers ********************/
/*****************************************************************************/

unsigned Cty_DB_GetNumCtysWithCtrs (Hie_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of countries with centers",
		  "SELECT COUNT(DISTINCT cty_countrs.CtyCod)"
		   " FROM cty_countrs,"
			 "ins_instits,"
			 "ctr_centers"
		  " WHERE %s"
		         "cty_countrs.CtyCod=ins_instits.CtyCod"
		    " AND ins_instits.InsCod=ctr_centers.InsCod",
		  SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with degrees ********************/
/*****************************************************************************/

unsigned Cty_DB_GetNumCtysWithDegs (Hie_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of countries with degrees",
		  "SELECT COUNT(DISTINCT cty_countrs.CtyCod)"
		   " FROM cty_countrs,"
			 "ins_instits,"
			 "ctr_centers,"
			 "deg_degrees"
		  " WHERE %s"
		         "cty_countrs.CtyCod=ins_instits.CtyCod"
		    " AND ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with courses ********************/
/*****************************************************************************/

unsigned Cty_DB_GetNumCtysWithCrss (Hie_Level_t Level,long HieCod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,HieCod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of countries with courses",
		  "SELECT COUNT(DISTINCT cty_countrs.CtyCod)"
		   " FROM cty_countrs,"
			 "ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses"
		  " WHERE %s"
		         "cty_countrs.CtyCod=ins_instits.CtyCod"
		    " AND ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod",
		  SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with users **********************/
/*****************************************************************************/

unsigned Cty_DB_GetNumCtysWithUsrs (Rol_Role_t Role,
                                    Hie_Level_t Level,long Cod)
  {
   char SubQuery[128];

   Hie_DB_BuildSubquery (SubQuery,Level,Cod);

   return (unsigned)
   DB_QueryCOUNT ("can not get number of countries with users",
		  "SELECT COUNT(DISTINCT cty_countrs.CtyCod)"
		   " FROM cty_countrs,"
			 "ins_instits,"
			 "ctr_centers,"
			 "deg_degrees,"
			 "crs_courses,"
			 "crs_users"
		  " WHERE %s"
			 "cty_countrs.CtyCod=ins_instits.CtyCod"
		    " AND ins_instits.InsCod=ctr_centers.InsCod"
		    " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		    " AND deg_degrees.DegCod=crs_courses.DegCod"
		    " AND crs_courses.CrsCod=crs_users.CrsCod"
		    " AND crs_users.Role=%u",
		  SubQuery,(unsigned) Role);
  }

/*****************************************************************************/
/******************* Check if a numeric country code exists ******************/
/*****************************************************************************/

bool Cty_DB_CheckIfNumericCountryCodeExists (long CtyCod)
  {
   return
   DB_QueryEXISTS ("can not check if the numeric code of a country already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM cty_countrs"
		    " WHERE CtyCod='%03ld')",
		   CtyCod);
  }

/*****************************************************************************/
/*************** Check if an alphabetic country code exists ******************/
/*****************************************************************************/

bool Cty_DB_CheckIfAlpha2CountryCodeExists (const char *Alpha2)
  {
   return
   DB_QueryEXISTS ("can not check if the alphabetic code of a country already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM cty_countrs"
		    " WHERE Alpha2='%s')",
		   Alpha2);
  }

/*****************************************************************************/
/******************** Check if the name of country exists ********************/
/*****************************************************************************/

bool Cty_DB_CheckIfCountryNameExists (Lan_Language_t Language,const char *Name,long CtyCod)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return
   DB_QueryEXISTS ("can not check if the name of a country already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM cty_countrs"
		    " WHERE Name_%s='%s'"
		      " AND CtyCod<>'%03ld')",
		   Lan_STR_LANG_ID[Language],Name,
		   CtyCod);
  }

/*****************************************************************************/
/************************ Search countries in database ***********************/
/*****************************************************************************/
// Returns number of countries found

unsigned Cty_DB_SearchCtys (MYSQL_RES **mysql_res,
                            const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1],
                            const char *RangeQuery)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get countries",
		   "SELECT CtyCod"
		    " FROM cty_countrs"
		   " WHERE %s"
		      "%s"
		   " ORDER BY Name_%s",
		   SearchQuery,
		   RangeQuery,
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
  }

/*****************************************************************************/
/*********** Get average coordinates of centers in current country ***********/
/*****************************************************************************/

void Cty_DB_GetAvgCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom)
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
		 Gbl.Hierarchy.Node[Hie_CTY].HieCod) < 0)
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
		   Gbl.Hierarchy.Node[Hie_CTY].HieCod);
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
/************ Check if any of the centers in a country has map ***************/
/*****************************************************************************/

bool Cty_DB_CheckIfMapIsAvailable (long CtyCod)
  {
   /***** Check if any center in current country has a coordinate set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   return
   DB_QueryEXISTS ("can not check if map is available",
		   "SELECT EXISTS"
		   "(SELECT *"
		    " FROM ins_instits,"
			  "ctr_centers"
		   " WHERE ins_instits.CtyCod=%ld"
		     " AND ins_instits.InsCod=ctr_centers.InsCod"
		     " AND (ctr_centers.Latitude<>0"
		       " OR ctr_centers.Longitude<>0))",
		   CtyCod);
  }

/*****************************************************************************/
/******** Update country changing old field value by new field value *********/
/*****************************************************************************/

void Cty_DB_UpdateCtyField (long CtyCod,const char *FldName,const char *FldValue)
  {
   DB_QueryUPDATE ("can not update a field value of a country",
		   "UPDATE cty_countrs"
		     " SET %s='%s'"
		   " WHERE CtyCod='%03ld'",
	           FldName,FldValue,
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
	           Gbl.Hierarchy.Node[Hie_CTY].HieCod);
  }

/*****************************************************************************/
/**************** Get the countries of a user from database ******************/
/*****************************************************************************/
// Returns the number of rows of the result

unsigned Cty_DB_GetCtysFromUsr (MYSQL_RES **mysql_res,long UsrCod)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the countries a user belongs to",
		   "SELECT cty_countrs.CtyCod,"	// row[0]
			  "MAX(crs_users.Role)"	// row[1]
		    " FROM crs_users,"
			  "crs_courses,"
			  "deg_degrees,"
			  "ctr_centers,"
			  "ins_instits,"
			  "cty_countrs"
		   " WHERE crs_users.UsrCod=%ld"
		     " AND crs_users.CrsCod=crs_courses.CrsCod"
		     " AND crs_courses.DegCod=deg_degrees.DegCod"
		     " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
		     " AND ctr_centers.InsCod=ins_instits.InsCod"
		     " AND ins_instits.CtyCod=cty_countrs.CtyCod"
		" GROUP BY cty_countrs.CtyCod"
		" ORDER BY cty_countrs.Name_%s",
		   UsrCod,
		   Lan_STR_LANG_ID[Gbl.Prefs.Language]);
  }

/*****************************************************************************/
/******* Get number of users who don't claim to belong to any country ********/
/*****************************************************************************/

unsigned Cty_DB_GetNumUsrsWhoDontClaimToBelongToAnyCty (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users",
		  "SELECT COUNT(UsrCod)"
		   " FROM usr_data"
		  " WHERE CtyCod<0");
  }

/*****************************************************************************/
/******** Get number of users who claim to belong to another country *********/
/*****************************************************************************/

unsigned Cty_DB_GetNumUsrsWhoClaimToBelongToAnotherCty (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users",
		  "SELECT COUNT(UsrCod)"
		   " FROM usr_data"
		  " WHERE CtyCod=0");
  }

/*****************************************************************************/
/*********** Get number of users who claim to belong to a country ************/
/*****************************************************************************/

unsigned Cty_DB_GetNumUsrsWhoClaimToBelongToCty (long CtyCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users",
		  "SELECT COUNT(UsrCod)"
		   " FROM usr_data"
		  " WHERE CtyCod=%ld",
		  CtyCod);
  }

/*****************************************************************************/
/******************************* Remove a country ****************************/
/*****************************************************************************/

void Cty_DB_RemoveCty (long CtyCod)
  {
   DB_QueryDELETE ("can not remove a country",
		   "DELETE FROM cty_countrs"
		   " WHERE CtyCod='%03ld'",
		   CtyCod);
  }
