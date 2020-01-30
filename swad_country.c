// swad_country.c: countries

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

#define _GNU_SOURCE 		// For asprintf
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_country_config.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"

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

static struct Country *Cty_EditingCty = NULL;	// Static variable to keep the country being edited
long Cty_CurrentCtyCod = -1L;	// Used as parameter in contextual links

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cty_PutHeadCountriesForSeeing (bool OrderSelectable);
static void Cty_ListOneCountryForSeeing (struct Country *Cty,unsigned NumCty);

static void Cty_PutIconsListingCountries (void);
static void Cty_PutIconToEditCountries (void);

static void Cty_GetParamCtyOrder (void);

static void Cty_EditCountriesInternal (void);
static void Cty_PutIconsEditingCountries (void);
static void Cty_PutIconToViewCountries (void);

static void Cty_ListCountriesForEdition (void);
static void Cty_PutParamOtherCtyCod (long CtyCod);
static long Cty_GetParamOtherCtyCod (void);

static bool Cty_CheckIfNumericCountryCodeExists (long CtyCod);
static bool Cty_CheckIfAlpha2CountryCodeExists (const char Alpha2[2 + 1]);
static bool Cty_CheckIfCountryNameExists (Lan_Language_t Language,const char *Name,long CtyCod);
static void Cty_UpdateCtyNameDB (long CtyCod,const char *FieldName,const char *NewCtyName);

static void Cty_ShowAlertAndButtonToGoToCty (void);
static void Cty_PutParamGoToCty (void);

static void Cty_PutFormToCreateCountry (void);
static void Cty_PutHeadCountriesForEdition (void);
static void Cty_CreateCountry (void);

static void Cty_EditingCountryConstructor (void);
static void Cty_EditingCountryDestructor (void);

static void Cty_FormToGoToMap (struct Country *Cty);

/*****************************************************************************/
/***************** List countries with pending institutions ******************/
/*****************************************************************************/

void Cty_SeeCtyWithPendingInss (void)
  {
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Countries_with_pending_institutions;
   extern const char *Txt_Country;
   extern const char *Txt_Institutions_ABBREVIATION;
   extern const char *Txt_There_are_no_countries_with_requests_for_institutions_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtys;
   unsigned NumCty;
   struct Country Cty;
   const char *BgColor;

   /***** Get countries with pending institutions *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_SYS_ADM:
         NumCtys = (unsigned) DB_QuerySELECT (&mysql_res,"can not get countries"
						         "with pending institutions",
					      "SELECT institutions.CtyCod,COUNT(*)"
					      " FROM institutions,countries"
					      " WHERE (institutions.Status & %u)<>0"
					      " AND institutions.CtyCod=countries.CtyCod"
					      " GROUP BY institutions.CtyCod"
					      " ORDER BY countries.Name_%s",
					      (unsigned) Ins_STATUS_BIT_PENDING,
					      Lan_STR_LANG_ID[Gbl.Prefs.Language]);
         break;
      default:	// Forbidden for other users
	 return;
     }

   /***** Get countries *****/
   if (NumCtys)
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Countries_with_pending_institutions,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"LM",Txt_Country);
      HTM_TH (1,1,"RM",Txt_Institutions_ABBREVIATION);

      HTM_TR_End ();

      /***** List the countries *****/
      for (NumCty = 0;
	   NumCty < NumCtys;
	   NumCty++)
        {
         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get country code (row[0]) */
         Cty.CtyCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Cty.CtyCod == Gbl.Hierarchy.Cty.CtyCod) ? "LIGHT_BLUE" :
                                                              Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of country */
         Cty_GetDataOfCountryByCod (&Cty);

         HTM_TR_Begin (NULL);

         /* Country map */
         HTM_TD_Begin ("class=\"LM %s\"",BgColor);
         Cty_DrawCountryMapAndNameWithLink (&Cty,ActSeeIns,
                                            "COUNTRY_SMALL",
                                            "COUNTRY_MAP_SMALL",
                                            "BT_LINK DAT");
         HTM_TD_End ();

         /* Number of pending institutions (row[1]) */
         HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Txt (row[1]);
         HTM_TD_End ();

         HTM_TR_End ();
         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_countries_with_requests_for_institutions_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** List all the countries **************************/
/*****************************************************************************/

void Cty_ListCountries (void)
  {
   Cty_ListCountries1 ();
   Cty_ListCountries2 ();
  }

/*****************************************************************************/
/*************************** List all the countries **************************/
/*****************************************************************************/

void Cty_ListCountries1 (void)
  {
   /***** Get parameter with the type of order in the list of countries *****/
   Cty_GetParamCtyOrder ();

   /***** Get list of countries *****/
   Cty_GetFullListOfCountries ();
  }

void Cty_ListCountries2 (void)
  {
   extern const char *Hlp_SYSTEM_Countries;
   extern const char *Txt_Countries;
   extern const char *Txt_Other_countries;
   extern const char *Txt_Country_unspecified;
   unsigned NumCty;

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Countries,Cty_PutIconsListingCountries,
                      Hlp_SYSTEM_Countries,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Cty_PutHeadCountriesForSeeing (true);	// Order selectable

   /***** Write all the countries and their number of users and institutions *****/
   for (NumCty = 0;
	NumCty < Gbl.Hierarchy.Ctys.Num;
	NumCty++)
      Cty_ListOneCountryForSeeing (&Gbl.Hierarchy.Ctys.Lst[NumCty],NumCty + 1);

   /***** Separation row *****/
   HTM_TR_Begin (NULL);
   HTM_TD_Begin ("colspan=\"8\" class=\"DAT CM\"");
   HTM_NBSP ();
   HTM_TD_End ();
   HTM_TR_End ();

   /***** Write users and institutions in other countries *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_Txt (Txt_Other_countries);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToAnotherCty ());
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Ins_GetNumInssInCty (0));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Ctr_GetNumCtrsInCty (0));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Deg_GetNumDegsInCty (0));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Crs_GetNumCrssInCty (0));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Usr_GetNumUsrsInCrss (Hie_CTY,0,
				       1 << Rol_NET |	// Non-editing teachers
				       1 << Rol_TCH));	// Teachers
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write users and institutions with unknown country *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_Txt (Txt_Country_unspecified);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Usr_GetNumUsrsWhoDontClaimToBelongToAnyCty ());
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Ins_GetNumInssInCty (-1L));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Ctr_GetNumCtrsInCty (-1L));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Deg_GetNumDegsInCty (-1L));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (Crs_GetNumCrssInCty (-1L));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Div for Google Geochart *****/
   if (Gbl.Action.Act == ActSeeCty)
     {
      HTM_DIV_Begin ("id=\"chart_div\"");
      HTM_DIV_End ();
     }

   /***** Free list of countries *****/
   Cty_FreeListCountries ();
  }

/*****************************************************************************/
/******************* Write header with fields of a country *******************/
/*****************************************************************************/

static void Cty_PutHeadCountriesForSeeing (bool OrderSelectable)
  {
   extern const char *Txt_COUNTRIES_HELP_ORDER[2];
   extern const char *Txt_COUNTRIES_ORDER[2];
   extern const char *Txt_Institutions_ABBREVIATION;
   extern const char *Txt_Centres_ABBREVIATION;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   Cty_Order_t Order;

   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);
   for (Order = Cty_ORDER_BY_COUNTRY;
	Order <= Cty_ORDER_BY_NUM_USRS;
	Order++)
     {
      HTM_TH_Begin (1,1,Order == Cty_ORDER_BY_COUNTRY ? "LM" :
						        "RM");
      if (OrderSelectable)
	{
	 Frm_StartForm (ActSeeCty);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_COUNTRIES_HELP_ORDER[Order],"BT_LINK TIT_TBL",NULL);
	 if (Order == Gbl.Hierarchy.Ctys.SelectedOrder)
	    HTM_U_Begin ();
	}
      HTM_Txt (Txt_COUNTRIES_ORDER[Order]);
      if (OrderSelectable)
	{
	 if (Order == Gbl.Hierarchy.Ctys.SelectedOrder)
	    HTM_U_End ();
	 HTM_BUTTON_End ();
	 Frm_EndForm ();
	}
      HTM_TH_End ();
     }

   HTM_TH (1,1,"RM",Txt_Institutions_ABBREVIATION);
   HTM_TH (1,1,"RM",Txt_Centres_ABBREVIATION);
   HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);
   HTM_TH (1,1,"RM",Txt_Courses_ABBREVIATION);
   HTM_TH_Begin (1,1,"RM");
   HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
   HTM_BR ();
   HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ List one country for seeing ************************/
/*****************************************************************************/

static void Cty_ListOneCountryForSeeing (struct Country *Cty,unsigned NumCty)
  {
   const char *BgColor;

   BgColor = (Cty->CtyCod == Gbl.Hierarchy.Cty.CtyCod) ? "LIGHT_BLUE" :
							 Gbl.ColorRows[Gbl.RowEvenOdd];

   HTM_TR_Begin (NULL);

   /***** Number of country in this list *****/
   HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
   HTM_Unsigned (NumCty);
   HTM_TD_End ();

   /***** Country map (and link to WWW if exists) *****/
   HTM_TD_Begin ("class=\"LM %s\"",BgColor);
   Cty_DrawCountryMapAndNameWithLink (Cty,ActSeeIns,
				      "COUNTRY_SMALL",
				      "COUNTRY_MAP_SMALL",
				      "BT_LINK DAT_N");
   HTM_TD_End ();

   /***** Number of users who claim to belong to this country *****/
   HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
   HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToCty (Cty));
   HTM_TD_End ();

   /***** Other stats *****/
   HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
   HTM_Unsigned (Ins_GetNumInssInCty (Cty->CtyCod));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
   HTM_Unsigned (Ctr_GetNumCtrsInCty (Cty->CtyCod));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
   HTM_Unsigned (Deg_GetNumDegsInCty (Cty->CtyCod));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
   HTM_Unsigned (Crs_GetNumCrssInCty (Cty->CtyCod));
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
   HTM_Unsigned (Usr_GetNumUsrsInCrss (Hie_CTY,Cty->CtyCod,
				       1 << Rol_STD |
				       1 << Rol_NET |
				       1 << Rol_TCH));	// Any user
   HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/***************** Put contextual icons in list of countries *****************/
/*****************************************************************************/

static void Cty_PutIconsListingCountries (void)
  {
   /***** Put icon to edit countries *****/
   if (Cty_CheckIfICanEditCountries ())
      Cty_PutIconToEditCountries ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************** Check if I can edit countries ************************/
/*****************************************************************************/

bool Cty_CheckIfICanEditCountries (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
  }

/*****************************************************************************/
/************************ Put icon to edit countries *************************/
/*****************************************************************************/

static void Cty_PutIconToEditCountries (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCty,NULL);
  }

/*****************************************************************************/
/********************* Draw country map and name with link *******************/
/*****************************************************************************/

void Cty_DrawCountryMapAndNameWithLink (struct Country *Cty,Act_Action_t Action,
                                        const char *ClassContainer,
                                        const char *ClassMap,
                                        const char *ClassLink)
  {
   char CountryName[Cty_MAX_BYTES_NAME + 1];

   /***** Begin form *****/
   Frm_StartFormGoTo (Action);
   Cty_PutParamCtyCod (Cty->CtyCod);
   HTM_DIV_Begin ("class=\"%s\"",ClassContainer);

   /***** Link to action *****/
   HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Cty->Name[Gbl.Prefs.Language]),
			    ClassLink,NULL);
   Hie_FreeGoToMsg ();

   /***** Draw country map *****/
   Cty_DrawCountryMap (Cty,ClassMap);

   /***** Write country name *****/
   Str_Copy (CountryName,Cty->Name[Gbl.Prefs.Language],
             Cty_MAX_BYTES_NAME);
   HTM_TxtF ("&nbsp;%s&nbsp;",CountryName);
   HTM_TxtF ("(%s)",Cty->Alpha2);

   /***** End link *****/
   HTM_BUTTON_End ();

   /***** End form *****/
   HTM_DIV_End ();
   Frm_EndForm ();

   /***** Map *****/
   Cty_FormToGoToMap (Cty);
  }

/*****************************************************************************/
/***************************** Draw country map ******************************/
/*****************************************************************************/

void Cty_DrawCountryMap (struct Country *Cty,const char *Class)
  {
   char *URL;

   /***** Draw country map *****/
   if (Cty_CheckIfCountryPhotoExists (Cty))
     {
      if (asprintf (&URL,"%s/%s",
		    Cfg_URL_ICON_COUNTRIES_PUBLIC,
	            Cty->Alpha2) < 0)
	 Lay_NotEnoughMemoryExit ();
      HTM_IMG (URL,Str_BuildStringStr ("%s.png",Cty->Alpha2),
	       Cty->Name[Gbl.Prefs.Language],
	       "class=\"%s\"",Class);
      Str_FreeString ();
      free (URL);
     }
   else
      Ico_PutIcon ("tr16x16.gif",Cty->Name[Gbl.Prefs.Language],Class);
  }

/*****************************************************************************/
/*********************** Check if country map exists *************************/
/*****************************************************************************/

bool Cty_CheckIfCountryPhotoExists (struct Country *Cty)
  {
   char PathMap[PATH_MAX + 1];

   snprintf (PathMap,sizeof (PathMap),
	     "%s/%s/%s.png",
	     Cfg_PATH_ICON_COUNTRIES_PUBLIC,
	     Cty->Alpha2,
	     Cty->Alpha2);
   return Fil_CheckIfPathExists (PathMap);
  }

/*****************************************************************************/
/********************** Write script for Google Geochart *********************/
/*****************************************************************************/

void Cty_WriteScriptGoogleGeochart (void)
  {
   extern const char *Txt_Country_NO_HTML;
   extern const char *Txt_Users_NO_HTML;
   extern const char *Txt_Institutions_NO_HTML;
   unsigned NumCty;
   unsigned NumUsrsWhoClaimToBelongToCty;
   unsigned MaxUsrsInCountry = 0;
   unsigned NumCtysWithUsrs = 0;

   /***** Write start of the script *****/
   HTM_SCRIPT_Begin ("https://www.google.com/jsapi",NULL);
   HTM_SCRIPT_End ();

   HTM_SCRIPT_Begin (NULL,NULL);
   HTM_TxtF ("	google.load('visualization', '1', {'packages': ['geochart']});\n"
             "	google.setOnLoadCallback(drawRegionsMap);\n"
             "	function drawRegionsMap() {\n"
             "	var data = new google.visualization.DataTable();\n"
             "	data.addColumn('string', '%s');\n"
             "	data.addColumn('number', '%s');\n"
             "	data.addColumn('number', '%s');\n"
             "	data.addRows([\n",
             Txt_Country_NO_HTML,
             Txt_Users_NO_HTML,
             Txt_Institutions_NO_HTML);

   /***** Write all the countries and their number of users and institutions *****/
   for (NumCty = 0;
	NumCty < Gbl.Hierarchy.Ctys.Num;
	NumCty++)
     {
      NumUsrsWhoClaimToBelongToCty = Usr_GetNumUsrsWhoClaimToBelongToCty (&Gbl.Hierarchy.Ctys.Lst[NumCty]);
      if (NumUsrsWhoClaimToBelongToCty)
        {
         /* Write data of this country */
         HTM_TxtF ("	['%s', %u, %u],\n",
                   Gbl.Hierarchy.Ctys.Lst[NumCty].Alpha2,
                   NumUsrsWhoClaimToBelongToCty,
                   Ins_GetNumInssInCty (Gbl.Hierarchy.Ctys.Lst[NumCty].CtyCod));
         if (NumUsrsWhoClaimToBelongToCty > MaxUsrsInCountry)
            MaxUsrsInCountry = NumUsrsWhoClaimToBelongToCty;
         NumCtysWithUsrs++;
        }
     }

   /***** Write end of the script *****/
   HTM_TxtF ("	]);\n"
             "	var options = {\n"
             "		width:600,\n"
             "		height:360,\n"
             "		backgroundColor:'white',\n"
             "		datalessRegionColor:'white',\n"
             "		colorAxis:{colors:['#EAF1F4','#4D88A1'],minValue:0,maxValue:%u}};\n"
             "	var chart = new google.visualization.GeoChart(document.getElementById('chart_div'));\n"
             "	chart.draw(data, options);\n"
             "	};\n",
             NumCtysWithUsrs ? MaxUsrsInCountry :
        	               0);
   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/******** Get parameter with the type or order in list of countries **********/
/*****************************************************************************/

static void Cty_GetParamCtyOrder (void)
  {
   Gbl.Hierarchy.Ctys.SelectedOrder = (Cty_Order_t)
					  Par_GetParToUnsignedLong ("Order",
								    0,
								    Cty_NUM_ORDERS - 1,
								    (unsigned long) Cty_ORDER_DEFAULT);
  }

/*****************************************************************************/
/******************** Put forms to edit institution types ********************/
/*****************************************************************************/

void Cty_EditCountries (void)
  {
   /***** Country constructor *****/
   Cty_EditingCountryConstructor ();

   /***** Edit countries *****/
   Cty_EditCountriesInternal ();

   /***** Country destructor *****/
   Cty_EditingCountryDestructor ();
  }

static void Cty_EditCountriesInternal (void)
  {
   extern const char *Hlp_SYSTEM_Countries;
   extern const char *Txt_Countries;

   /***** Get list of countries *****/
   Gbl.Hierarchy.Ctys.SelectedOrder = Cty_ORDER_BY_COUNTRY;
   Cty_GetFullListOfCountries ();

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Countries,Cty_PutIconsEditingCountries,
                 Hlp_SYSTEM_Countries,Box_NOT_CLOSABLE);

   /***** Put a form to create a new country *****/
   Cty_PutFormToCreateCountry ();

   /***** Forms to edit current countries *****/
   if (Gbl.Hierarchy.Ctys.Num)
      Cty_ListCountriesForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of countries *****/
   Cty_FreeListCountries ();
  }

/*****************************************************************************/
/*************** Put contextual icons in edition of countries ****************/
/*****************************************************************************/

static void Cty_PutIconsEditingCountries (void)
  {
   /***** Put icon to view countries *****/
   Cty_PutIconToViewCountries ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/************************ Put icon to view countries *************************/
/*****************************************************************************/

static void Cty_PutIconToViewCountries (void)
  {
   extern const char *Txt_Countries;

   Lay_PutContextualLinkOnlyIcon (ActSeeCty,NULL,NULL,
                                  "globe.svg",
                                  Txt_Countries);
  }

/*****************************************************************************/
/********** Get basic list of countries ordered by name of country ***********/
/*****************************************************************************/

void Cty_GetBasicListOfCountries (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;
   unsigned NumCty;
   struct Country *Cty;
   Lan_Language_t Lan;

   /***** Get countries from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get countries",
			     "SELECT CtyCod,Alpha2,Name_%s"
			     " FROM countries ORDER BY Name_%s",
			     Lan_STR_LANG_ID[Gbl.Prefs.Language],
			     Lan_STR_LANG_ID[Gbl.Prefs.Language]);
   if (NumRows) // Countries found...
     {
      Gbl.Hierarchy.Ctys.Num = (unsigned) NumRows;

      /***** Create list with countries *****/
      if ((Gbl.Hierarchy.Ctys.Lst = (struct Country *)
	                                calloc (NumRows,sizeof (struct Country))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Ctys.Num;
	   NumCty++)
        {
         Cty = &(Gbl.Hierarchy.Ctys.Lst[NumCty]);

         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get numerical country code (row[0]) */
         if ((Cty->CtyCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of country.");

         /* Get Alpha-2 country code (row[1]) */
         Str_Copy (Cty->Alpha2,row[1],
                   2);

	 for (Lan  = (Lan_Language_t) 1;
	      Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	      Lan++)
	   {
	    Cty->Name[Lan][0] = '\0';
	    Cty->WWW[Lan][0] = '\0';
	   }

	 /* Get the name of the country in current language */
	 Str_Copy (Cty->Name[Gbl.Prefs.Language],row[2],
		   Cty_MAX_BYTES_NAME);

	 /* Reset number of users who claim to belong to country */
	 Cty->NumUsrsWhoClaimToBelongToCty.Valid = false;
        }
     }
   else
      Gbl.Hierarchy.Ctys.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Get full list of countries with names in all languages ***********/
/********** and number of users who claim to belong to them        ***********/
/*****************************************************************************/

#define Cty_MAX_BYTES_SUBQUERY_CTYS	((1 + Lan_NUM_LANGUAGES) * 32)

void Cty_GetFullListOfCountries (void)
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;
   unsigned NumCty;
   struct Country *Cty;
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
      snprintf (StrField,sizeof (StrField),
		"countries.Name_%s,",
		Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryNam1,StrField,
		  Cty_MAX_BYTES_SUBQUERY_CTYS);
      snprintf (StrField,sizeof (StrField),
		"Name_%s,",
		Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryNam2,StrField,
		  Cty_MAX_BYTES_SUBQUERY_CTYS);

      snprintf (StrField,sizeof (StrField),
		"countries.WWW_%s,",
		Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryWWW1,StrField,
		  Cty_MAX_BYTES_SUBQUERY_CTYS);
      snprintf (StrField,sizeof (StrField),
		"WWW_%s,",
		Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryWWW2,StrField,
		  Cty_MAX_BYTES_SUBQUERY_CTYS);
     }

   /* Build order subquery */
   if (asprintf (&OrderBySubQuery,OrderBySubQueryFmt[Gbl.Hierarchy.Ctys.SelectedOrder],
		 Lan_STR_LANG_ID[Gbl.Prefs.Language]) < 0)
      Lay_NotEnoughMemoryExit ();

   /* Query database */
   NumRows = DB_QuerySELECT (&mysql_res,"can not get countries",
			     "(SELECT countries.CtyCod,countries.Alpha2,"
			     "%s%sCOUNT(*) AS NumUsrs"
			     " FROM countries,usr_data"
			     " WHERE countries.CtyCod=usr_data.CtyCod"
			     " GROUP BY countries.CtyCod)"
			     " UNION "
			     "(SELECT CtyCod,Alpha2,%s%s0 AS NumUsrs"
			     " FROM countries"
			     " WHERE CtyCod NOT IN"
			     " (SELECT DISTINCT CtyCod FROM usr_data))"
			     " ORDER BY %s",
			     SubQueryNam1,SubQueryWWW1,
			     SubQueryNam2,SubQueryWWW2,OrderBySubQuery);

   /* Free memory for subquery */
   free (OrderBySubQuery);

   if (NumRows) // Countries found...
     {
      Gbl.Hierarchy.Ctys.Num = (unsigned) NumRows;

      /***** Create list with countries *****/
      if ((Gbl.Hierarchy.Ctys.Lst = (struct Country *)
	                                calloc (NumRows,sizeof (struct Country))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Ctys.Num;
	   NumCty++)
        {
         Cty = &(Gbl.Hierarchy.Ctys.Lst[NumCty]);

         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get numerical country code (row[0]) */
         if ((Cty->CtyCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of country.");

         /* Get Alpha-2 country code (row[1]) */
         Str_Copy (Cty->Alpha2,row[1],
                   2);

	 /* Get the name of the country in several languages */
	 for (Lan  = (Lan_Language_t) 1;
	      Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	      Lan++)
	   {
	    Str_Copy (Cty->Name[Lan],row[1 + Lan],
		      Cty_MAX_BYTES_NAME);
	    Str_Copy (Cty->WWW[Lan],row[1 + Lan_NUM_LANGUAGES + Lan],
		      Cns_MAX_BYTES_WWW);
	   }

	 /* Get number of users who claim to belong to this country */
	 Cty->NumUsrsWhoClaimToBelongToCty.Valid = false;
	 if (sscanf (row[1 + Lan_NUM_LANGUAGES * 2 + 1],"%u",
		     &(Cty->NumUsrsWhoClaimToBelongToCty.NumUsrs)) == 1)
	    Cty->NumUsrsWhoClaimToBelongToCty.Valid = true;
        }
     }
   else
      Gbl.Hierarchy.Ctys.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Write selector of country ************************/
/*****************************************************************************/

void Cty_WriteSelectorOfCountry (void)
  {
   extern const char *Txt_Country;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtys;
   unsigned NumCty;
   long CtyCod;

   /***** Begin form *****/
   Frm_StartFormGoTo (ActSeeIns);
   HTM_SELECT_Begin (true,
		     "id=\"cty\" name=\"cty\" class=\"HIE_SEL\"");
   HTM_OPTION (HTM_Type_STRING,"",Gbl.Hierarchy.Cty.CtyCod < 0,true,
	       "[%s]",Txt_Country);

   /***** Get countries from database *****/
   NumCtys = (unsigned) DB_QuerySELECT (&mysql_res,"can not get countries",
				        "SELECT DISTINCT CtyCod,Name_%s"
					" FROM countries"
					" ORDER BY countries.Name_%s",
					Lan_STR_LANG_ID[Gbl.Prefs.Language],
					Lan_STR_LANG_ID[Gbl.Prefs.Language]);

   /***** List countries *****/
   for (NumCty = 0;
	NumCty < NumCtys;
	NumCty++)
     {
      /* Get next country */
      row = mysql_fetch_row (mysql_res);

      /* Get country code (row[0]) */
      if ((CtyCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of country.");

      /* Write option */
      HTM_OPTION (HTM_Type_LONG,&CtyCod,
		  CtyCod == Gbl.Hierarchy.Cty.CtyCod,false,
		  "%s",row[1]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End form *****/
   HTM_SELECT_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** Write country name ****************************/
/*****************************************************************************/
// If ClassLink == NULL ==> do not put link

void Cty_WriteCountryName (long CtyCod,const char *ClassLink)
  {
   char CtyName[Cty_MAX_BYTES_NAME + 1];
   bool PutForm = ClassLink &&
	          !Gbl.Form.Inside &&						// Only if not inside another form
                  Act_GetBrowserTab (Gbl.Action.Act) == Act_BRW_1ST_TAB;	// Only in main browser tab

   /***** Get country name *****/
   Cty_GetCountryName (CtyCod,Gbl.Prefs.Language,CtyName);

   if (PutForm)
     {
      /***** Write country name with link to country information *****/
      Frm_StartForm (ActSeeCtyInf);
      Cty_PutParamCtyCod (CtyCod);
      HTM_BUTTON_SUBMIT_Begin (Act_GetActionText (ActSeeCtyInf),ClassLink,NULL);
      HTM_Txt (CtyName);
      HTM_BUTTON_End ();
      Frm_EndForm ();
     }
   else
      /***** Write country name without link *****/
      HTM_Txt (CtyName);
  }

/*****************************************************************************/
/***************** Get basic data of country given its code ******************/
/*****************************************************************************/

bool Cty_GetDataOfCountryByCod (struct Country *Cty)
  {
   extern const char *Txt_Another_country;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;
   Lan_Language_t Lan;
   bool CtyFound;

   if (Cty->CtyCod < 0)
      return false;

   /***** Clear data *****/
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      Cty->Name[Lan][0] = '\0';
      Cty->WWW[Lan][0] = '\0';
     }
   Cty->NumUsrsWhoClaimToBelongToCty.Valid = false;

   /***** Check if country code is correct *****/
   if (Cty->CtyCod == 0)
     {
      for (Lan  = (Lan_Language_t) 1;
	   Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	   Lan++)
         if (Lan == Gbl.Prefs.Language)
            Str_Copy (Cty->Name[Lan],Txt_Another_country,
                      Cty_MAX_BYTES_NAME);
         else
            Cty->Name[Lan][0] = '\0';
      return false;
     }

   // Here Cty->CtyCod > 0

   /***** Get data of a country from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a country",
			     "SELECT Alpha2,Name_%s,WWW_%s"
			     " FROM countries"
			     " WHERE CtyCod='%03ld'",
			     Lan_STR_LANG_ID[Gbl.Prefs.Language],
			     Lan_STR_LANG_ID[Gbl.Prefs.Language],
			     Cty->CtyCod);

   /***** Count number of rows in result *****/
   if (NumRows) // Country found...
     {
      CtyFound = true;

      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get Alpha-2 country code (row[0]) */
      Str_Copy (Cty->Alpha2,row[0],
                2);

      /* Get name and WWW of the country in current language */
      Str_Copy (Cty->Name[Gbl.Prefs.Language],row[1],
		Cty_MAX_BYTES_NAME);
      Str_Copy (Cty->WWW[Gbl.Prefs.Language],row[2],
		Cns_MAX_BYTES_WWW);
     }
   else
      CtyFound = false;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return CtyFound;
  }

/*****************************************************************************/
/***************************** Get country name ******************************/
/*****************************************************************************/

void Cty_FlushCacheCountryName (void)
  {
   Gbl.Cache.CountryName.CtyCod     = -1L;
   Gbl.Cache.CountryName.Language   = Lan_LANGUAGE_UNKNOWN;
   Gbl.Cache.CountryName.CtyName[0] = '\0';
  }

void Cty_GetCountryName (long CtyCod,Lan_Language_t Language,
			 char CtyName[Cty_MAX_BYTES_NAME + 1])
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** 1. Fast check: Trivial case *****/
   if (CtyCod <= 0)
     {
      CtyName[0] = '\0';	// Empty name
      return;
     }

   /***** 2. Fast check: If cached... *****/
   if (CtyCod   == Gbl.Cache.CountryName.CtyCod &&
       Language == Gbl.Cache.CountryName.Language)
     {
      Str_Copy (CtyName,Gbl.Cache.CountryName.CtyName,
		Cty_MAX_BYTES_NAME);
      return;
     }

   /***** 3. Slow: get country name from database *****/
   Gbl.Cache.CountryName.CtyCod   = CtyCod;
   Gbl.Cache.CountryName.Language = Language;

   if (DB_QuerySELECT (&mysql_res,"can not get the name of a country",
		       "SELECT Name_%s FROM countries WHERE CtyCod='%03ld'",
	               Lan_STR_LANG_ID[Language],CtyCod)) // Country found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the name of the country */
      Str_Copy (Gbl.Cache.CountryName.CtyName,row[0],
		Cty_MAX_BYTES_NAME);
     }
   else
      Gbl.Cache.CountryName.CtyName[0] = '\0';

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   Str_Copy (CtyName,Gbl.Cache.CountryName.CtyName,
	     Cty_MAX_BYTES_NAME);
  }

/*****************************************************************************/
/*************************** Free list of countries **************************/
/*****************************************************************************/

void Cty_FreeListCountries (void)
  {
   if (Gbl.Hierarchy.Ctys.Lst)
     {
      /***** Free memory used by the list of courses in institution *****/
      free (Gbl.Hierarchy.Ctys.Lst);
      Gbl.Hierarchy.Ctys.Lst = NULL;
      Gbl.Hierarchy.Ctys.Num = 0;
     }
  }

/*****************************************************************************/
/*************************** List all the countries **************************/
/*****************************************************************************/

static void Cty_ListCountriesForEdition (void)
  {
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   unsigned NumCty;
   struct Country *Cty;
   unsigned NumInssInCty;
   Lan_Language_t Lan;

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Cty_PutHeadCountriesForEdition ();

   /***** Write all countries *****/
   for (NumCty = 0;
	NumCty < Gbl.Hierarchy.Ctys.Num;
	NumCty++)
     {
      Cty = &Gbl.Hierarchy.Ctys.Lst[NumCty];
      NumInssInCty = Ins_GetNumInssInCty (Cty->CtyCod);

      HTM_TR_Begin (NULL);

      /* Put icon to remove country */
      HTM_TD_Begin ("rowspan=\"%u\" class=\"BT\"",1 + Lan_NUM_LANGUAGES);
      if (NumInssInCty)						// Country has institutions
	 // Deletion forbidden
	 Ico_PutIconRemovalNotAllowed ();
      else if (Usr_GetNumUsrsWhoClaimToBelongToCty (Cty))	// Country has users
	 // Deletion forbidden
	 Ico_PutIconRemovalNotAllowed ();
      else if (Usr_GetNumUsrsInCrss (Hie_CTY,Cty->CtyCod,
				     1 << Rol_STD |
				     1 << Rol_NET |
				     1 << Rol_TCH))		// Country has users
	 // Deletion forbidden
	 Ico_PutIconRemovalNotAllowed ();
      else
        {
         Frm_StartForm (ActRemCty);
         Cty_PutParamOtherCtyCod (Cty->CtyCod);
         Ico_PutIconRemove ();
         Frm_EndForm ();
        }
      HTM_TD_End ();

      /* Numerical country code (ISO 3166-1) */
      HTM_TD_Begin ("rowspan=\"%u\" class=\"DAT RT\"",1 + Lan_NUM_LANGUAGES);
      HTM_TxtF ("%03ld",Cty->CtyCod);
      HTM_TD_End ();

      /* Alphabetic country code with 2 letters (ISO 3166-1) */
      HTM_TD_Begin ("rowspan=\"%u\" class=\"DAT RT\"",1 + Lan_NUM_LANGUAGES);
      HTM_Txt (Cty->Alpha2);
      HTM_TD_End ();

      HTM_TD_Empty (3);

      /* Number of users */
      HTM_TD_Begin ("rowspan=\"%u\" class=\"DAT RT\"",1 + Lan_NUM_LANGUAGES);
      HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToCty (Cty));
      HTM_TD_End ();

      /* Number of institutions */
      HTM_TD_Begin ("rowspan=\"%u\" class=\"DAT RT\"",1 + Lan_NUM_LANGUAGES);
      HTM_Unsigned (NumInssInCty);
      HTM_TD_End ();

      HTM_TR_End ();

      /* Country name in several languages */
      for (Lan  = (Lan_Language_t) 1;
	   Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	   Lan++)
        {
         HTM_TR_Begin (NULL);

	 /* Language */
         HTM_TD_Begin ("class=\"DAT RM\"");
         HTM_TxtF ("%s:",Txt_STR_LANG_NAME[Lan]);
         HTM_TD_End ();

         /* Name */
         HTM_TD_Begin ("class=\"LT\"");
         Frm_StartForm (ActRenCty);
         Cty_PutParamOtherCtyCod (Cty->CtyCod);
         Par_PutHiddenParamUnsigned (NULL,"Lan",(unsigned) Lan);
	 HTM_INPUT_TEXT ("Name",Cty_MAX_CHARS_NAME,Cty->Name[Lan],true,
			 "size=\"15\"");
         Frm_EndForm ();
         HTM_TD_End ();

         /* WWW */
         HTM_TD_Begin ("class=\"LT\"");
         Frm_StartForm (ActChgCtyWWW);
         Cty_PutParamOtherCtyCod (Cty->CtyCod);
         Par_PutHiddenParamUnsigned (NULL,"Lan",(unsigned) Lan);
	 HTM_INPUT_URL ("WWW",Cty->WWW[Lan],true,
			"class=\"INPUT_WWW_NARROW\" required=\"required\"");
         Frm_EndForm ();
         HTM_TD_End ();

         HTM_TR_End ();
        }
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write parameter with code of country *******************/
/*****************************************************************************/

void Cty_PutParamCtyCod (long CtyCod)
  {
   Par_PutHiddenParamLong (NULL,"cty",CtyCod);
  }

/*****************************************************************************/
/******************** Write parameter with code of country *******************/
/*****************************************************************************/

static void Cty_PutParamOtherCtyCod (long CtyCod)
  {
   Par_PutHiddenParamLong (NULL,"OthCtyCod",CtyCod);
  }

/*****************************************************************************/
/******************* Get parameter with code of country **********************/
/*****************************************************************************/

long Cty_GetAndCheckParamOtherCtyCod (long MinCodAllowed)
  {
   long CtyCod;

   /***** Get and check parameter with code of country *****/
   if ((CtyCod = Cty_GetParamOtherCtyCod ()) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of country is missing or invalid.");

   return CtyCod;
  }

static long Cty_GetParamOtherCtyCod (void)
  {
   /***** Get code of country *****/
   return Par_GetParToLong ("OthCtyCod");
  }

/*****************************************************************************/
/****************************** Remove a country *****************************/
/*****************************************************************************/

void Cty_RemoveCountry (void)
  {
   extern const char *Txt_You_can_not_remove_a_country_with_institutions_or_users;
   extern const char *Txt_Country_X_removed;

   /***** Country constructor *****/
   Cty_EditingCountryConstructor ();

   /***** Get country code *****/
   Cty_EditingCty->CtyCod = Cty_GetAndCheckParamOtherCtyCod (0);

   /***** Get data of the country from database *****/
   Cty_GetDataOfCountryByCod (Cty_EditingCty);

   /***** Check if this country has users *****/
   if (Ins_GetNumInssInCty (Cty_EditingCty->CtyCod))			// Country has institutions ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else if (Usr_GetNumUsrsWhoClaimToBelongToCty (Cty_EditingCty))	// Country has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else if (Usr_GetNumUsrsInCrss (Hie_CTY,Cty_EditingCty->CtyCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))			// Country has users
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else	// Country has no users ==> remove it
     {
      /***** Remove surveys of the country *****/
      Svy_RemoveSurveys (Hie_CTY,Cty_EditingCty->CtyCod);

      /***** Remove country *****/
      DB_QueryDELETE ("can not remove a country",
		      "DELETE FROM countries WHERE CtyCod='%03ld'",
		      Cty_EditingCty->CtyCod);

      /***** Flush cache *****/
      Cty_FlushCacheCountryName ();
      Ins_FlushCacheNumInssInCty ();
      Ctr_FlushCacheNumCtrsInCty ();
      Deg_FlushCacheNumDegsInCty ();
      Crs_FlushCacheNumCrssInCty ();
      Usr_FlushCacheNumUsrsWhoClaimToBelongToCty ();

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Country_X_removed,
	               Cty_EditingCty->Name[Gbl.Prefs.Language]);

      Cty_EditingCty->CtyCod = -1L;	// To not showing button to go to country
     }
  }

/*****************************************************************************/
/************************ Change the name of a country ***********************/
/*****************************************************************************/

void Cty_RenameCountry (void)
  {
   extern const char *Txt_The_country_X_already_exists;
   extern const char *Txt_The_country_X_has_been_renamed_as_Y;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_The_name_of_the_country_X_has_not_changed;
   char NewCtyName[Cty_MAX_BYTES_NAME + 1];
   Lan_Language_t Language;
   char FieldName[4 + 1 + 2 + 1];	// Example: "Name_en"

   /***** Country constructor *****/
   Cty_EditingCountryConstructor ();

   /***** Get the code of the country *****/
   Cty_EditingCty->CtyCod = Cty_GetAndCheckParamOtherCtyCod (0);

   /***** Get the lenguage *****/
   Language = Lan_GetParamLanguage ();

   /***** Get the new name for the country *****/
   Par_GetParToText ("Name",NewCtyName,Cty_MAX_BYTES_NAME);

   /***** Get from the database the data of the country *****/
   Cty_GetDataOfCountryByCod (Cty_EditingCty);

   /***** Check if new name is empty *****/
   if (NewCtyName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      Cty_GetCountryName (Cty_EditingCty->CtyCod,Language,
			  Cty_EditingCty->Name[Language]);
      if (strcmp (Cty_EditingCty->Name[Language],NewCtyName))	// Different names
	{
	 /***** If country was in database... *****/
	 if (Cty_CheckIfCountryNameExists (Language,NewCtyName,Cty_EditingCty->CtyCod))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_country_X_already_exists,
		             NewCtyName);
	 else
	   {
	    /* Update the table changing old name by new name */
	    snprintf (FieldName,sizeof (FieldName),
		      "Name_%s",
		      Lan_STR_LANG_ID[Language]);
	    Cty_UpdateCtyNameDB (Cty_EditingCty->CtyCod,FieldName,NewCtyName);

	    /* Write message to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_The_country_X_has_been_renamed_as_Y,
		             Cty_EditingCty->Name[Language],NewCtyName);

	    /* Update country name */
	    Str_Copy (Cty_EditingCty->Name[Language],NewCtyName,
		      Cty_MAX_BYTES_NAME);
	   }
	}
      else	// The same name
	 Ale_CreateAlert (Ale_INFO,NULL,
	                  Txt_The_name_of_the_country_X_has_not_changed,
		          Cty_EditingCty->Name[Language]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/******************* Check if a numeric country code exists ******************/
/*****************************************************************************/

static bool Cty_CheckIfNumericCountryCodeExists (long CtyCod)
  {
   /***** Get number of countries with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the numeric code"
	                  " of a country already existed",
			  "SELECT COUNT(*) FROM countries"
			  " WHERE CtyCod='%03ld'",
			  CtyCod) != 0);
  }

/*****************************************************************************/
/*************** Check if an alphabetic country code exists ******************/
/*****************************************************************************/

static bool Cty_CheckIfAlpha2CountryCodeExists (const char Alpha2[2 + 1])
  {
   /***** Get number of countries with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the alphabetic code"
	                  " of a country already existed",
			  "SELECT COUNT(*) FROM countries"
			  " WHERE Alpha2='%s'",
			  Alpha2) != 0);
  }

/*****************************************************************************/
/******************** Check if the name of country exists ********************/
/*****************************************************************************/

static bool Cty_CheckIfCountryNameExists (Lan_Language_t Language,const char *Name,long CtyCod)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   /***** Get number of countries with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name"
	                  " of a country already existed",
			  "SELECT COUNT(*) FROM countries"
			  " WHERE Name_%s='%s' AND CtyCod<>'%03ld'",
			  Lan_STR_LANG_ID[Language],Name,CtyCod) != 0);
  }

/*****************************************************************************/
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Cty_UpdateCtyNameDB (long CtyCod,const char *FieldName,const char *NewCtyName)
  {
   /***** Update country changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a country",
		   "UPDATE countries SET %s='%s' WHERE CtyCod='%03ld'",
	           FieldName,NewCtyName,CtyCod);

   /***** Flush cache *****/
   Cty_FlushCacheCountryName ();
  }

/*****************************************************************************/
/************************ Change the URL of a country ************************/
/*****************************************************************************/

void Cty_ChangeCtyWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   char NewWWW[Cns_MAX_BYTES_WWW + 1];
   Lan_Language_t Language;

   /***** Country constructor *****/
   Cty_EditingCountryConstructor ();

   /***** Get the code of the country *****/
   Cty_EditingCty->CtyCod = Cty_GetAndCheckParamOtherCtyCod (0);

   /***** Get the lenguage *****/
   Language = Lan_GetParamLanguage ();

   /***** Get the new WWW for the country *****/
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get from the database the data of the country *****/
   Cty_GetDataOfCountryByCod (Cty_EditingCty);

   /***** Update the table changing old WWW by new WWW *****/
   DB_QueryUPDATE ("can not update the web of a country",
		   "UPDATE countries SET WWW_%s='%s'"
		   " WHERE CtyCod='%03ld'",
	           Lan_STR_LANG_ID[Language],NewWWW,Cty_EditingCty->CtyCod);
   Str_Copy (Cty_EditingCty->WWW[Language],NewWWW,
	     Cns_MAX_BYTES_WWW);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_new_web_address_is_X,
	            NewWWW);
  }

/*****************************************************************************/
/********* Show alerts after changing a country and continue editing *********/
/*****************************************************************************/

void Cty_ContEditAfterChgCty (void)
  {
   /***** Write message to show the change made
	  and put button to go to country changed *****/
   Cty_ShowAlertAndButtonToGoToCty ();

   /***** Show the form again *****/
   Cty_EditCountriesInternal ();

   /***** Country destructor *****/
   Cty_EditingCountryDestructor ();
  }

/*****************************************************************************/
/***************** Write message to show the change made   *******************/
/***************** and put button to go to country changed *******************/
/*****************************************************************************/

static void Cty_ShowAlertAndButtonToGoToCty (void)
  {
   // If the country being edited is different to the current one...
   if (Cty_EditingCty->CtyCod != Gbl.Hierarchy.Cty.CtyCod)
     {
      /***** Alert with button to go to couuntry *****/
      Ale_ShowLastAlertAndButton (ActSeeIns,NULL,NULL,Cty_PutParamGoToCty,
                                  Btn_CONFIRM_BUTTON,
				  Hie_BuildGoToMsg (Cty_EditingCty->Name[Gbl.Prefs.Language]));
      Hie_FreeGoToMsg ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Cty_PutParamGoToCty (void)
  {
   /***** Put parameter *****/
   Cty_PutParamCtyCod (Cty_EditingCty->CtyCod);
  }

/*****************************************************************************/
/********************* Put a form to create a new country ********************/
/*****************************************************************************/

static void Cty_PutFormToCreateCountry (void)
  {
   extern const char *Txt_New_country;
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Create_country;
   Lan_Language_t Lan;
   char StrCtyCod[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char StrName[32];

   /***** Begin form *****/
   Frm_StartForm (ActNewCty);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_country,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Cty_PutHeadCountriesForEdition ();

   HTM_TR_Begin (NULL);

   /***** Column to remove country, disabled here *****/
   HTM_TD_Begin ("rowspan=\"%u\" class=\"BT\"",1 + Lan_NUM_LANGUAGES);
   HTM_TD_End ();

   /***** Numerical country code (ISO 3166-1) *****/
   HTM_TD_Begin ("rowspan=\"%u\" class=\"RT\"",1 + Lan_NUM_LANGUAGES);
   if (Cty_EditingCty->CtyCod > 0)
      snprintf (StrCtyCod,sizeof (StrCtyCod),
		"%03ld",
		Cty_EditingCty->CtyCod);
   else
      StrCtyCod[0] = '\0';
   HTM_INPUT_TEXT ("OthCtyCod",3,StrCtyCod,false,
		   "size=\"3\" required=\"required\"");
   HTM_TD_End ();

   /***** Alphabetic country code with 2 letters (ISO 3166-1) *****/
   HTM_TD_Begin ("rowspan=\"%u\" class=\"RT\"",1 + Lan_NUM_LANGUAGES);
   HTM_INPUT_TEXT ("Alpha2",2,Cty_EditingCty->Alpha2,false,
		   "size=\"2\" required=\"required\"");
   HTM_TD_End ();

   HTM_TD_Empty (3);

   /***** Number of users *****/
   HTM_TD_Begin ("rowspan=\"%u\" class=\"DAT RT\"",1 + Lan_NUM_LANGUAGES);
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Number of institutions *****/
   HTM_TD_Begin ("rowspan=\"%u\" class=\"DAT RT\"",1 + Lan_NUM_LANGUAGES);
   HTM_Unsigned (0);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Country name in several languages *****/
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      HTM_TR_Begin (NULL);

      /* Language */
      HTM_TD_Begin ("class=\"DAT RT\"");
      HTM_Txt (Txt_STR_LANG_NAME[Lan]);
      HTM_TD_End ();

      /* Name */
      HTM_TD_Begin ("class=\"LM\"");
      snprintf (StrName,sizeof (StrName),
		"Name_%s",
		Lan_STR_LANG_ID[Lan]);
      HTM_INPUT_TEXT (StrName,Cty_MAX_CHARS_NAME,Cty_EditingCty->Name[Lan],false,
		      "size=\"15\" required=\"required\"");
      HTM_TD_End ();

      /* WWW */
      HTM_TD_Begin ("class=\"LM\"");
      snprintf (StrName,sizeof (StrName),
		"WWW_%s",
		Lan_STR_LANG_ID[Lan]);
      HTM_INPUT_URL (StrName,Cty_EditingCty->WWW[Lan],false,
		     "class=\"INPUT_WWW_NARROW\" required=\"required\"");
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_country);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************* Write header with fields of a country *******************/
/*****************************************************************************/

static void Cty_PutHeadCountriesForEdition (void)
  {
   extern const char *Txt_Numeric_BR_code_BR_ISO_3166_1;
   extern const char *Txt_Alphabetic_BR_code_BR_ISO_3166_1;
   extern const char *Txt_Name;
   extern const char *Txt_WWW;
   extern const char *Txt_Users;
   extern const char *Txt_Institutions_ABBREVIATION;

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"RM",Txt_Numeric_BR_code_BR_ISO_3166_1);
   HTM_TH (1,1,"RM",Txt_Alphabetic_BR_code_BR_ISO_3166_1);
   HTM_TH_Empty (1);
   HTM_TH (1,1,"LM",Txt_Name);
   HTM_TH (1,1,"LM",Txt_WWW);
   HTM_TH (1,1,"RM",Txt_Users);
   HTM_TH (1,1,"RM",Txt_Institutions_ABBREVIATION);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new country ********************/
/*****************************************************************************/

void Cty_RecFormNewCountry (void)
  {
   extern const char *Txt_You_must_specify_the_numerical_code_of_the_new_country;
   extern const char *Txt_The_numerical_code_X_already_exists;
   extern const char *Txt_The_alphabetical_code_X_is_not_correct;
   extern const char *Txt_The_alphabetical_code_X_already_exists;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_The_country_X_already_exists;
   extern const char *Txt_You_must_specify_the_name_of_the_new_country_in_all_languages;
   extern const char *Txt_Created_new_country_X;
   char ParamName[32];
   bool CreateCountry = true;
   Lan_Language_t Lan;
   unsigned i;

   /***** Country constructoor *****/
   Cty_EditingCountryConstructor ();

   /***** Get parameters from form *****/
   /* Get numeric country code */
   if ((Cty_EditingCty->CtyCod = Cty_GetParamOtherCtyCod ()) < 0)
     {
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_numerical_code_of_the_new_country);
      CreateCountry = false;
     }
   else if (Cty_CheckIfNumericCountryCodeExists (Cty_EditingCty->CtyCod))
     {
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_The_numerical_code_X_already_exists,
                       Cty_EditingCty->CtyCod);
      CreateCountry = false;
     }
   else	// Numeric code correct
     {
      /* Get alphabetic-2 country code */
      Par_GetParToText ("Alpha2",Cty_EditingCty->Alpha2,2);
      Str_ConvertToUpperText (Cty_EditingCty->Alpha2);
      for (i = 0;
	   i < 2 && CreateCountry;
	   i++)
         if (Cty_EditingCty->Alpha2[i] < 'A' ||
             Cty_EditingCty->Alpha2[i] > 'Z')
           {
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_alphabetical_code_X_is_not_correct,
                             Cty_EditingCty->Alpha2);
            CreateCountry = false;
           }
      if (CreateCountry)
        {
         if (Cty_CheckIfAlpha2CountryCodeExists (Cty_EditingCty->Alpha2))
           {
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_alphabetical_code_X_already_exists,
                             Cty_EditingCty->Alpha2);
            CreateCountry = false;
           }
         else	// Alphabetic code correct
           {
            /* Get country name and WWW in different languages */
            for (Lan  = (Lan_Language_t) 1;
        	 Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
        	 Lan++)
              {
               snprintf (ParamName,sizeof (ParamName),
        	         "Name_%s",
			 Lan_STR_LANG_ID[Lan]);
               Par_GetParToText (ParamName,Cty_EditingCty->Name[Lan],Cty_MAX_BYTES_NAME);

               if (Cty_EditingCty->Name[Lan][0])	// If there's a country name
                 {
                  /***** If name of country was in database... *****/
                  if (Cty_CheckIfCountryNameExists (Lan,Cty_EditingCty->Name[Lan],-1L))
                    {
                     Ale_CreateAlert (Ale_WARNING,NULL,
                	              Txt_The_country_X_already_exists,
                                      Cty_EditingCty->Name[Lan]);
                     CreateCountry = false;
                     break;
                    }
                 }
               else	// If there is not a country name
                 {
                  Ale_CreateAlert (Ale_WARNING,NULL,
                	           Txt_You_must_specify_the_name_of_the_new_country_in_all_languages);
                  CreateCountry = false;
                  break;
                 }

               snprintf (ParamName,sizeof (ParamName),
        	         "WWW_%s",
			 Lan_STR_LANG_ID[Lan]);
               Par_GetParToText (ParamName,Cty_EditingCty->WWW[Lan],Cns_MAX_BYTES_WWW);
              }
           }
        }
     }

   if (CreateCountry)
     {
      Cty_CreateCountry ();	// Add new country to database
      Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_country_X,
		     Cty_EditingCty->Name);
     }
  }

/*****************************************************************************/
/**************************** Create a new country ***************************/
/*****************************************************************************/

#define Cty_MAX_BYTES_SUBQUERY_CTYS_NAME	((1 + Lan_NUM_LANGUAGES) * Cty_MAX_BYTES_NAME)
#define Cty_MAX_BYTES_SUBQUERY_CTYS_WWW		((1 + Lan_NUM_LANGUAGES) * Cns_MAX_BYTES_WWW)

static void Cty_CreateCountry (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t Lan;
   char StrField[32];
   char SubQueryNam1[Cty_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryNam2[Cty_MAX_BYTES_SUBQUERY_CTYS_NAME + 1];
   char SubQueryWWW1[Cty_MAX_BYTES_SUBQUERY_CTYS + 1];
   char SubQueryWWW2[Cty_MAX_BYTES_SUBQUERY_CTYS_WWW + 1];

   /***** Create a new country *****/
   SubQueryNam1[0] = '\0';
   SubQueryNam2[0] = '\0';
   SubQueryWWW1[0] = '\0';
   SubQueryWWW2[0] = '\0';
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      snprintf (StrField,sizeof (StrField),
	        ",Name_%s",
		Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryNam1,StrField,
                  Cty_MAX_BYTES_SUBQUERY_CTYS);

      Str_Concat (SubQueryNam2,",'",
                  Cty_MAX_BYTES_SUBQUERY_CTYS_NAME);
      Str_Concat (SubQueryNam2,Cty_EditingCty->Name[Lan],
                  Cty_MAX_BYTES_SUBQUERY_CTYS_NAME);
      Str_Concat (SubQueryNam2,"'",
                  Cty_MAX_BYTES_SUBQUERY_CTYS_NAME);

      snprintf (StrField,sizeof (StrField),
	        ",WWW_%s",
		Lan_STR_LANG_ID[Lan]);
      Str_Concat (SubQueryWWW1,StrField,
                  Cty_MAX_BYTES_SUBQUERY_CTYS);

      Str_Concat (SubQueryWWW2,",'",
                  Cty_MAX_BYTES_SUBQUERY_CTYS_WWW);
      Str_Concat (SubQueryWWW2,Cty_EditingCty->WWW[Lan],
                  Cty_MAX_BYTES_SUBQUERY_CTYS_WWW);
      Str_Concat (SubQueryWWW2,"'",
                  Cty_MAX_BYTES_SUBQUERY_CTYS_WWW);
     }
   DB_QueryINSERT ("can not create country",
		   "INSERT INTO countries"
		   " (CtyCod,Alpha2,MapAttribution%s%s)"
		   " VALUES"
		   " ('%03ld','%s',''%s%s)",
                   SubQueryNam1,SubQueryWWW1,
                   Cty_EditingCty->CtyCod,Cty_EditingCty->Alpha2,
		   SubQueryNam2,SubQueryWWW2);
  }

/*****************************************************************************/
/*********************** Get total number of countries ***********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysTotal (void)
  {
   /***** Get total number of countries from database *****/
   return (unsigned) DB_GetNumRowsTable ("countries");
  }

/*****************************************************************************/
/***************** Get number of countries with institutions *****************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithInss (const char *SubQuery)
  {
   /***** Get number of countries with institutions from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of countries"
			     " with institutions",
			     "SELECT COUNT(DISTINCT countries.CtyCod)"
			     " FROM countries,institutions"
			     " WHERE %scountries.CtyCod=institutions.CtyCod",
			     SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with centres ********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithCtrs (const char *SubQuery)
  {
   /***** Get number of countries with centres from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of countries with centres",
			     "SELECT COUNT(DISTINCT countries.CtyCod)"
			     " FROM countries,institutions,centres"
			     " WHERE %scountries.CtyCod=institutions.CtyCod"
			     " AND institutions.InsCod=centres.InsCod",
			     SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with degrees ********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithDegs (const char *SubQuery)
  {
   /***** Get number of countries with degrees from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of countries with degrees",
			     "SELECT COUNT(DISTINCT countries.CtyCod)"
			     " FROM countries,institutions,centres,degrees"
			     " WHERE %scountries.CtyCod=institutions.CtyCod"
			     " AND institutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod",
			     SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with courses ********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithCrss (const char *SubQuery)
  {
   /***** Get number of countries with courses from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of countries with courses",
			     "SELECT COUNT(DISTINCT countries.CtyCod)"
			     " FROM countries,institutions,centres,degrees,courses"
			     " WHERE %scountries.CtyCod=institutions.CtyCod"
			     " AND institutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod",
			     SubQuery);
  }

/*****************************************************************************/
/******************* Get number of countries with users **********************/
/*****************************************************************************/

unsigned Cty_GetNumCtysWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   /***** Get number of countries with users from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of countries with users",
			     "SELECT COUNT(DISTINCT countries.CtyCod)"
			     " FROM countries,institutions,centres,degrees,courses,crs_usr"
			     " WHERE %scountries.CtyCod=institutions.CtyCod"
			     " AND institutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod"
			     " AND courses.CrsCod=crs_usr.CrsCod"
			     " AND crs_usr.Role=%u",
			     SubQuery,(unsigned) Role);
  }

/*****************************************************************************/
/***************************** List countries found **************************/
/*****************************************************************************/

void Cty_ListCtysFound (MYSQL_RES **mysql_res,unsigned NumCtys)
  {
   extern const char *Txt_country;
   extern const char *Txt_countries;
   MYSQL_ROW row;
   unsigned NumCty;
   struct Country Cty;

   /***** Query database *****/
   if (NumCtys)
     {
      /***** Begin box and table *****/
      /* Number of countries found */
      Box_BoxTableBegin (NULL,Str_BuildStringLongStr ((long) NumCtys,
						      NumCtys == 1 ? Txt_country :
								     Txt_countries),
			 NULL,NULL,Box_NOT_CLOSABLE,2);
      Str_FreeString ();

      /***** Write heading *****/
      Cty_PutHeadCountriesForSeeing (false);	// Order not selectable

      /***** List the countries (one row per country) *****/
      for (NumCty  = 1;
	   NumCty <= NumCtys;
	   NumCty++)
	{
	 /* Get next country */
	 row = mysql_fetch_row (*mysql_res);

	 /* Get country code (row[0]) */
	 Cty.CtyCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get data of country */
	 Cty_GetDataOfCountryByCod (&Cty);

	 /* Write data of this country */
	 Cty_ListOneCountryForSeeing (&Cty,NumCty);
	}

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/*********************** Country constructor/destructor **********************/
/*****************************************************************************/

static void Cty_EditingCountryConstructor (void)
  {
   Lan_Language_t Lan;

   /***** Pointer must be NULL *****/
   if (Cty_EditingCty != NULL)
      Lay_ShowErrorAndExit ("Error initializing country.");

   /***** Allocate memory for country *****/
   if ((Cty_EditingCty = (struct Country *) malloc (sizeof (struct Country))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for country.");

   /***** Reset country *****/
   Cty_EditingCty->CtyCod = -1L;
   Cty_EditingCty->Alpha2[0] = '\0';
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      Cty_EditingCty->Name[Lan][0] = '\0';
      Cty_EditingCty->WWW [Lan][0] = '\0';
     }
   Cty_EditingCty->NumUsrsWhoClaimToBelongToCty.Valid = false;
  }

static void Cty_EditingCountryDestructor (void)
  {
   /***** Free memory used for country *****/
   if (Cty_EditingCty != NULL)
     {
      free (Cty_EditingCty);
      Cty_EditingCty = NULL;
     }
  }

/*****************************************************************************/
/************************ Form to go to country map **************************/
/*****************************************************************************/

static void Cty_FormToGoToMap (struct Country *Cty)
  {
   extern const char *Txt_Map;

   if (Cty_GetIfMapIsAvailable (Cty->CtyCod))
     {
      Cty_EditingCty = Cty;	// Used to pass parameter with the code of the country
      Lay_PutContextualLinkOnlyIcon (ActSeeCtyInf,NULL,Cty_PutParamGoToCty,
				     "map-marker-alt.svg",
				     Txt_Map);
     }
  }

/*****************************************************************************/
/************ Check if any of the centres in a country has map ***************/
/*****************************************************************************/

bool Cty_GetIfMapIsAvailable (long CtyCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool MapIsAvailable = false;

   /***** Get if any centre in current country has a coordinate set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   if (DB_QuerySELECT (&mysql_res,"can not get if map is available",
		       "SELECT EXISTS"
		       "(SELECT * FROM institutions,centres"
		       " WHERE institutions.CtyCod=%ld"
		       " AND institutions.InsCod=centres.InsCod"
		       " AND (centres.Latitude<>0 OR centres.Longitude<>0))",
		       CtyCod))
     {
      /* Get if map is available */
      row = mysql_fetch_row (mysql_res);
      MapIsAvailable = (row[0][0] == '1');
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   return MapIsAvailable;
  }
