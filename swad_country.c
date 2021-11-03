// swad_country.c: countries

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
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_country_config.h"
#include "swad_country_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_survey.h"

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

static struct Cty_Countr *Cty_EditingCty = NULL;	// Static variable to keep the country being edited
long Cty_CurrentCtyCod = -1L;	// Used as parameter in contextual links

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cty_PutHeadCountriesForSeeing (bool OrderSelectable);
static void Cty_ListOneCountryForSeeing (struct Cty_Countr *Cty,unsigned NumCty);

static void Cty_PutIconsListingCountries (__attribute__((unused)) void *Args);
static void Cty_PutIconToEditCountries (void);

static void Cty_GetParamCtyOrder (void);

static void Cty_EditCountriesInternal (void);
static void Cty_PutIconsEditingCountries (__attribute__((unused)) void *Args);
static void Cty_PutIconToViewCountries (void);

static void Cty_ListCountriesForEdition (void);
static void Cty_PutParamOtherCtyCod (void *CtyCod);
static long Cty_GetParamOtherCtyCod (void);

static void Cty_UpdateCtyName (long CtyCod,const char *FieldName,const char *NewCtyName);

static void Cty_ShowAlertAndButtonToGoToCty (void);
static void Cty_PutParamGoToCty (void *CtyCod);

static void Cty_PutFormToCreateCountry (void);
static void Cty_PutHeadCountriesForEdition (void);

static void Cty_EditingCountryConstructor (void);
static void Cty_EditingCountryDestructor (void);

static void Cty_FormToGoToMap (struct Cty_Countr *Cty);

/*****************************************************************************/
/***************** List countries with pending institutions ******************/
/*****************************************************************************/

void Cty_SeeCtyWithPendingInss (void)
  {
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Countries_with_pending_institutions;
   extern const char *Txt_Country;
   extern const char *Txt_Institutions_ABBREVIATION;
   extern const char *Txt_There_are_no_countries_with_requests_for_institutions_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtys;
   unsigned NumCty;
   struct Cty_Countr Cty;
   const char *BgColor;

   /***** Trivial check: only system admins can see countries with pending institutions *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_SYS_ADM:
         break;
      default:	// Forbidden for other users
	 return;
     }

   /***** Get countries with pending institutions *****/
   if ((NumCtys = Cty_DB_GetCtysWithPendingInss (&mysql_res)))
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Countries_with_pending_institutions,
                         NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH (1,1,"LM",Txt_Country);
	    HTM_TH (1,1,"RM",Txt_Institutions_ABBREVIATION);
	 HTM_TR_End ();

	 /***** List the countries *****/
	 for (NumCty = 0;
	      NumCty < NumCtys;
	      NumCty++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
	   {
	    /* Get next country */
	    row = mysql_fetch_row (mysql_res);

	    /* Get country code (row[0]) */
	    Cty.CtyCod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Cty.CtyCod == Gbl.Hierarchy.Cty.CtyCod) ? "LIGHT_BLUE" :
								 Gbl.ColorRows[Gbl.RowEvenOdd];

	    /* Get data of country */
	    Cty_GetDataOfCountryByCod (&Cty);

	    /* Begin row for this country */
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

	    /* End row for this country */
	    HTM_TR_End ();
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
/****************************** List all countries ***************************/
/*****************************************************************************/

void Cty_ListCountries (void)
  {
   Cty_ListCountries1 ();
   Cty_ListCountries2 ();
  }

/*****************************************************************************/
/****************************** List all countries ***************************/
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
   Box_BoxTableBegin (NULL,Txt_Countries,
                      Cty_PutIconsListingCountries,NULL,
                      Hlp_SYSTEM_Countries,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      Cty_PutHeadCountriesForSeeing (true);	// Order selectable

      /***** Write all countries and their number of users and institutions *****/
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

	 /* Number of users who claim to belong to another country */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Usr_GetCachedNumUsrsWhoClaimToBelongToAnotherCty ());
	 HTM_TD_End ();

	 /* Number of institutions in other countries */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Ins_GetCachedNumInssInCty (0));
	 HTM_TD_End ();

	 /* Number of centers in other countries */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Ctr_GetCachedNumCtrsInCty (0));
	 HTM_TD_End ();

	 /* Number of degrees in other countries */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Deg_GetCachedNumDegsInCty (0));
	 HTM_TD_End ();

	 /* Number of courses in other countries */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Crs_GetCachedNumCrssInCty (0));
	 HTM_TD_End ();

	 /* Number of users in courses of other countries */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Usr_GetCachedNumUsrsInCrss (HieLvl_CTY,0,
						      1 << Rol_STD |
						      1 << Rol_NET |
						      1 << Rol_TCH));	// Any user
	 HTM_TD_End ();

      HTM_TR_End ();

      /***** Write users and institutions with unknown country *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"DAT RM\"");
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"DAT LM\"");
	    HTM_Txt (Txt_Country_unspecified);
	 HTM_TD_End ();

	 /* Number of users who do not claim to belong to any country */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Usr_GetCachedNumUsrsWhoDontClaimToBelongToAnyCty ());
	 HTM_TD_End ();

	 /* Number of institutions with unknown country */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Ins_GetCachedNumInssInCty (-1L));
	 HTM_TD_End ();

	 /* Number of centers with unknown country */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Ctr_GetCachedNumCtrsInCty (-1L));
	 HTM_TD_End ();

	 /* Number of degrees with unknown country */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Deg_GetCachedNumDegsInCty (-1L));
	 HTM_TD_End ();

	 /* Number of courses with unknown country */
	 HTM_TD_Begin ("class=\"DAT RM\"");
	    HTM_Unsigned (Crs_GetCachedNumCrssInCty (-1L));
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
   extern const char *Txt_Centers_ABBREVIATION;
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
	    Frm_BeginForm (ActSeeCty);
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
      HTM_TH (1,1,"RM",Txt_Centers_ABBREVIATION);
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

static void Cty_ListOneCountryForSeeing (struct Cty_Countr *Cty,unsigned NumCty)
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
	 HTM_Unsigned (Usr_GetCachedNumUsrsWhoClaimToBelongToCty (Cty));
      HTM_TD_End ();

      /***** Number of institutions *****/
      HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Unsigned (Ins_GetCachedNumInssInCty (Cty->CtyCod));
      HTM_TD_End ();

      /***** Number of centers *****/
      HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Unsigned (Ctr_GetCachedNumCtrsInCty (Cty->CtyCod));
      HTM_TD_End ();

      /***** Number of degrees *****/
      HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Unsigned (Deg_GetCachedNumDegsInCty (Cty->CtyCod));
      HTM_TD_End ();

      /***** Number of courses *****/
      HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Unsigned (Crs_GetCachedNumCrssInCty (Cty->CtyCod));
      HTM_TD_End ();

      /***** Number of users in courses *****/
      HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Unsigned (Usr_GetCachedNumUsrsInCrss (HieLvl_CTY,Cty->CtyCod,
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

static void Cty_PutIconsListingCountries (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit countries *****/
   if (Cty_CheckIfICanEditCountries ())
      Cty_PutIconToEditCountries ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
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
   Ico_PutContextualIconToEdit (ActEdiCty,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/********************* Draw country map and name with link *******************/
/*****************************************************************************/

void Cty_DrawCountryMapAndNameWithLink (struct Cty_Countr *Cty,Act_Action_t Action,
                                        const char *ClassContainer,
                                        const char *ClassMap,
                                        const char *ClassLink)
  {
   char CountryName[Cty_MAX_BYTES_NAME + 1];

   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
   Cty_PutParamCtyCod (Cty->CtyCod);

      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"%s\"",ClassContainer);

	 /***** Link to action *****/
	 HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Cty->Name[Gbl.Prefs.Language]),
				  ClassLink,NULL);
	 Hie_FreeGoToMsg ();

	    /***** Draw country map *****/
	    Cty_DrawCountryMap (Cty,ClassMap);

	    /***** Write country name *****/
	    Str_Copy (CountryName,Cty->Name[Gbl.Prefs.Language],sizeof (CountryName) - 1);
	    HTM_TxtF ("&nbsp;%s&nbsp;",CountryName);
	    HTM_TxtF ("(%s)",Cty->Alpha2);

	 /***** End link *****/
	 HTM_BUTTON_End ();

      /***** End container *****/
      HTM_DIV_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** Map *****/
   Cty_FormToGoToMap (Cty);
  }

/*****************************************************************************/
/***************************** Draw country map ******************************/
/*****************************************************************************/

void Cty_DrawCountryMap (struct Cty_Countr *Cty,const char *Class)
  {
   char *URL;

   /***** Draw country map *****/
   if (Cty_CheckIfCountryPhotoExists (Cty))
     {
      if (asprintf (&URL,"%s/%s",
		    Cfg_URL_ICON_COUNTRIES_PUBLIC,
	            Cty->Alpha2) < 0)
	 Err_NotEnoughMemoryExit ();
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

bool Cty_CheckIfCountryPhotoExists (struct Cty_Countr *Cty)
  {
   char PathMap[PATH_MAX + 1];

   snprintf (PathMap,sizeof (PathMap),"%s/%s/%s.png",
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
   unsigned NumUsrsCty;
   unsigned NumInss;
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

      /***** Write all countries and their number of users and institutions *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Ctys.Num;
	   NumCty++)
	{
	 NumUsrsCty = Usr_GetCachedNumUsrsWhoClaimToBelongToCty (&Gbl.Hierarchy.Ctys.Lst[NumCty]);
	 if (NumUsrsCty)
	   {
	    NumInss = Ins_GetCachedNumInssInCty (Gbl.Hierarchy.Ctys.Lst[NumCty].CtyCod);

	    /* Write data of this country */
	    HTM_TxtF ("	['%s', %u, %u],\n",
		      Gbl.Hierarchy.Ctys.Lst[NumCty].Alpha2,NumUsrsCty,NumInss);
	    if (NumUsrsCty > MaxUsrsInCountry)
	       MaxUsrsInCountry = NumUsrsCty;
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
   Box_BoxBegin (NULL,Txt_Countries,
                 Cty_PutIconsEditingCountries,NULL,
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

static void Cty_PutIconsEditingCountries (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view countries *****/
   Cty_PutIconToViewCountries ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/************************ Put icon to view countries *************************/
/*****************************************************************************/

static void Cty_PutIconToViewCountries (void)
  {
   extern const char *Txt_Countries;

   Lay_PutContextualLinkOnlyIcon (ActSeeCty,NULL,
                                  NULL,NULL,
                                  "globe.svg",
                                  Txt_Countries);
  }

/*****************************************************************************/
/********** Get basic list of countries ordered by name of country ***********/
/*****************************************************************************/

void Cty_GetBasicListOfCountries (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCty;
   struct Cty_Countr *Cty;
   Lan_Language_t Lan;

   /***** Trivial check: if list is already got, nothing to do *****/
   if (Gbl.Hierarchy.Ctys.Num)
      return;

   /***** Get countries from database *****/
   if ((Gbl.Hierarchy.Ctys.Num = Cty_DB_GetCtysBasic (&mysql_res))) // Countries found...
     {
      /***** Create list with countries *****/
      if ((Gbl.Hierarchy.Ctys.Lst = calloc ((size_t) Gbl.Hierarchy.Ctys.Num,
                                            sizeof (*Gbl.Hierarchy.Ctys.Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Ctys.Num;
	   NumCty++)
        {
         Cty = &(Gbl.Hierarchy.Ctys.Lst[NumCty]);

         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get numerical country code (row[0]) */
         if ((Cty->CtyCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongCountrExit ();

         /* Reset names and webs */
	 for (Lan  = (Lan_Language_t) 1;
	      Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	      Lan++)
	   {
	    Cty->Name[Lan][0] = '\0';
	    Cty->WWW[Lan][0] = '\0';
	   }

	 /* Get the name of the country in current language */
	 Str_Copy (Cty->Name[Gbl.Prefs.Language],row[1],
		   sizeof (Cty->Name[Gbl.Prefs.Language]) - 1);

	 /* Reset number of users who claim to belong to country */
	 Cty->NumUsrsWhoClaimToBelongToCty.Valid = false;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Get full list of countries with names in all languages ***********/
/********** and number of users who claim to belong to them        ***********/
/*****************************************************************************/

void Cty_GetFullListOfCountries (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCty;
   struct Cty_Countr *Cty;
   Lan_Language_t Lan;

   /***** Trivial check: if list is already got, nothing to do *****/
   if (Gbl.Hierarchy.Ctys.Num)
      return;

   /***** Get countries from database *****/
   if ((Gbl.Hierarchy.Ctys.Num = Cty_DB_GetCtysFull (&mysql_res))) // Countries found...
     {
      /***** Create list with countries *****/
      if ((Gbl.Hierarchy.Ctys.Lst = calloc ((size_t) Gbl.Hierarchy.Ctys.Num,
                                            sizeof (*Gbl.Hierarchy.Ctys.Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Ctys.Num;
	   NumCty++)
        {
         Cty = &(Gbl.Hierarchy.Ctys.Lst[NumCty]);

         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get numerical country code (row[0]) */
         if ((Cty->CtyCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongCountrExit ();

         /* Get Alpha-2 country code (row[1]) */
         Str_Copy (Cty->Alpha2,row[1],sizeof (Cty->Alpha2) - 1);

	 /* Get the name of the country in several languages */
	 for (Lan  = (Lan_Language_t) 1;
	      Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	      Lan++)
	   {
	    Str_Copy (Cty->Name[Lan],row[1 + Lan],
	              sizeof (Cty->Name[Lan]) - 1);
	    Str_Copy (Cty->WWW[Lan],row[1 + Lan_NUM_LANGUAGES + Lan],
		      sizeof (Cty->WWW[Lan]) - 1);
	   }

	 /* Get number of users who claim to belong to this country */
	 Cty->NumUsrsWhoClaimToBelongToCty.Valid = false;
	 if (sscanf (row[1 + Lan_NUM_LANGUAGES * 2 + 1],"%u",
		     &(Cty->NumUsrsWhoClaimToBelongToCty.NumUsrs)) == 1)
	    Cty->NumUsrsWhoClaimToBelongToCty.Valid = true;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Write selector of country ************************/
/*****************************************************************************/

void Cty_WriteSelectorOfCountry (void)
  {
   extern const char *Txt_Country;
   unsigned NumCty;

   /***** Get list of countries *****/
   Cty_GetBasicListOfCountries ();

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeIns);

      /***** Begin selector of country *****/
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			"id=\"cty\" name=\"cty\" class=\"HIE_SEL\"");

         /***** Initial disabled option *****/
	 HTM_OPTION (HTM_Type_STRING,"",Gbl.Hierarchy.Cty.CtyCod < 0,true,
		     "[%s]",Txt_Country);

	 /***** List countries *****/
	 for (NumCty = 0;
	      NumCty < Gbl.Hierarchy.Ctys.Num;
	      NumCty++)
	    HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Ctys.Lst[NumCty].CtyCod,
			Gbl.Hierarchy.Ctys.Lst[NumCty].CtyCod == Gbl.Hierarchy.Cty.CtyCod,false,
			"%s",Gbl.Hierarchy.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);

      /***** End selector of country *****/
      HTM_SELECT_End ();

   /***** End form *****/
   Frm_EndForm ();

   // Do not free list of countries here, because it can be reused
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
      Frm_BeginForm (ActSeeCtyInf);
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

bool Cty_GetDataOfCountryByCod (struct Cty_Countr *Cty)
  {
   extern const char *Txt_Another_country;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
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
                      sizeof (Cty->Name[Lan]) - 1);
         else
            Cty->Name[Lan][0] = '\0';
      return false;
     }

   // Here Cty->CtyCod > 0

   /***** Get data of a country from database *****/
   CtyFound = (Cty_DB_GetDataOfCountryByCod (&mysql_res,Cty->CtyCod) != 0);
   if (CtyFound) // Country found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get Alpha-2 country code (row[0]) */
      Str_Copy (Cty->Alpha2,row[0],sizeof (Cty->Alpha2) - 1);

      /* Get name and WWW of the country in current language */
      Str_Copy (Cty->Name[Gbl.Prefs.Language],row[1],
                sizeof (Cty->Name[Gbl.Prefs.Language]) - 1);
      Str_Copy (Cty->WWW[Gbl.Prefs.Language],row[2],
		sizeof (Cty->WWW[Gbl.Prefs.Language]) - 1);
     }

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
      Str_Copy (CtyName,Gbl.Cache.CountryName.CtyName,Cty_MAX_BYTES_NAME);
      return;
     }

   /***** 3. Slow: get country name from database *****/
   Cty_DB_GetCountryName (CtyCod,Language,CtyName);
   Gbl.Cache.CountryName.CtyCod   = CtyCod;
   Gbl.Cache.CountryName.Language = Language;
   Str_Copy (Gbl.Cache.CountryName.CtyName,CtyName,Cty_MAX_BYTES_NAME);
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
/***************************** List all countries ****************************/
/*****************************************************************************/

static void Cty_ListCountriesForEdition (void)
  {
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   unsigned NumCty;
   struct Cty_Countr *Cty;
   unsigned NumInss;
   unsigned NumUsrsCty;
   Lan_Language_t Lan;

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Write heading *****/
      Cty_PutHeadCountriesForEdition ();

      /***** Write all countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Ctys.Num;
	   NumCty++)
	{
	 Cty = &Gbl.Hierarchy.Ctys.Lst[NumCty];
	 NumInss = Ins_GetNumInssInCty (Cty->CtyCod);
	 NumUsrsCty = Usr_GetNumUsrsWhoClaimToBelongToCty (Cty);

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove country */
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"BT\"",1 + Lan_NUM_LANGUAGES);
	       if (NumInss ||					// Country has institutions
		   NumUsrsCty)					// Country has users
		  // Deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else if (Usr_GetNumUsrsInCrss (HieLvl_CTY,Cty->CtyCod,
					      1 << Rol_STD |
					      1 << Rol_NET |
					      1 << Rol_TCH))	// Country has users
		  // Deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else
		  Ico_PutContextualIconToRemove (ActRemCty,NULL,
						 Cty_PutParamOtherCtyCod,&Cty->CtyCod);
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
	       HTM_Unsigned (NumUsrsCty);
	    HTM_TD_End ();

	    /* Number of institutions */
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"DAT RT\"",1 + Lan_NUM_LANGUAGES);
	       HTM_Unsigned (NumInss);
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
		  HTM_TxtColon (Txt_STR_LANG_NAME[Lan]);
	       HTM_TD_End ();

	       /* Name */
	       HTM_TD_Begin ("class=\"LT\"");
		  Frm_BeginForm (ActRenCty);
		  Cty_PutParamOtherCtyCod (&Cty->CtyCod);
		  Par_PutHiddenParamUnsigned (NULL,"Lan",(unsigned) Lan);
		     HTM_INPUT_TEXT ("Name",Cty_MAX_CHARS_NAME,Cty->Name[Lan],
				     HTM_SUBMIT_ON_CHANGE,
				     "size=\"15\"");
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /* WWW */
	       HTM_TD_Begin ("class=\"LT\"");
		  Frm_BeginForm (ActChgCtyWWW);
		  Cty_PutParamOtherCtyCod (&Cty->CtyCod);
		  Par_PutHiddenParamUnsigned (NULL,"Lan",(unsigned) Lan);
		     HTM_INPUT_URL ("WWW",Cty->WWW[Lan],HTM_SUBMIT_ON_CHANGE,
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

static void Cty_PutParamOtherCtyCod (void *CtyCod)
  {
   if (CtyCod)
      Par_PutHiddenParamLong (NULL,"OthCtyCod",*((long *) CtyCod));
  }

/*****************************************************************************/
/******************* Get parameter with code of country **********************/
/*****************************************************************************/

long Cty_GetAndCheckParamOtherCtyCod (long MinCodAllowed)
  {
   long CtyCod;

   /***** Get and check parameter with code of country *****/
   if ((CtyCod = Cty_GetParamOtherCtyCod ()) < MinCodAllowed)
      Err_WrongCountrExit ();

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
   else if (Usr_GetNumUsrsInCrss (HieLvl_CTY,Cty_EditingCty->CtyCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))			// Country has users
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else	// Country has no users ==> remove it
     {
      /***** Remove surveys of the country *****/
      Svy_RemoveSurveys (HieLvl_CTY,Cty_EditingCty->CtyCod);

      /***** Remove country *****/
      Cty_DB_RemoveCty (Cty_EditingCty->CtyCod);

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
	 if (Cty_DB_CheckIfCountryNameExists (Language,NewCtyName,Cty_EditingCty->CtyCod))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_country_X_already_exists,
		             NewCtyName);
	 else
	   {
	    /* Update the table changing old name by new name */
	    snprintf (FieldName,sizeof (FieldName),"Name_%s",
		      Lan_STR_LANG_ID[Language]);
	    Cty_UpdateCtyName (Cty_EditingCty->CtyCod,FieldName,NewCtyName);

	    /* Write message to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_The_country_X_has_been_renamed_as_Y,
		             Cty_EditingCty->Name[Language],NewCtyName);

	    /* Update country name */
	    Str_Copy (Cty_EditingCty->Name[Language],NewCtyName,
		      sizeof (Cty_EditingCty->Name[Language]) - 1);
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
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Cty_UpdateCtyName (long CtyCod,const char *FieldName,const char *NewCtyName)
  {
   /***** Update country changing old name by new name */
   Cty_DB_UpdateCtyField (CtyCod,FieldName,NewCtyName);

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
   char FieldName[3 + 1 + 2 + 1];	// Example: "WWW_en"

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
   snprintf (FieldName,sizeof (FieldName),"Name_%s",
	     Lan_STR_LANG_ID[Language]);
   Cty_DB_UpdateCtyField (Cty_EditingCty->CtyCod,FieldName,NewWWW);
   Str_Copy (Cty_EditingCty->WWW[Language],NewWWW,
	     sizeof (Cty_EditingCty->WWW[Language]) - 1);

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
      Ale_ShowLastAlertAndButton (ActSeeIns,NULL,NULL,
                                  Cty_PutParamGoToCty,&Cty_EditingCty->CtyCod,
                                  Btn_CONFIRM_BUTTON,
				  Hie_BuildGoToMsg (Cty_EditingCty->Name[Gbl.Prefs.Language]));
      Hie_FreeGoToMsg ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Cty_PutParamGoToCty (void *CtyCod)
  {
   if (CtyCod)
      /***** Put parameter *****/
      Cty_PutParamCtyCod (*((long *) CtyCod));
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
   Frm_BeginForm (ActNewCty);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_New_country,
			 NULL,NULL,
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
		  snprintf (StrCtyCod,sizeof (StrCtyCod),"%03ld",Cty_EditingCty->CtyCod);
	       else
		  StrCtyCod[0] = '\0';
	       HTM_INPUT_TEXT ("OthCtyCod",3,StrCtyCod,HTM_DONT_SUBMIT_ON_CHANGE,
			       "size=\"3\" required=\"required\"");
	    HTM_TD_End ();

	    /***** Alphabetic country code with 2 letters (ISO 3166-1) *****/
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"RT\"",1 + Lan_NUM_LANGUAGES);
	       HTM_INPUT_TEXT ("Alpha2",2,Cty_EditingCty->Alpha2,HTM_DONT_SUBMIT_ON_CHANGE,
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
		  snprintf (StrName,sizeof (StrName),"Name_%s",Lan_STR_LANG_ID[Lan]);
		  HTM_INPUT_TEXT (StrName,Cty_MAX_CHARS_NAME,Cty_EditingCty->Name[Lan],
				  HTM_DONT_SUBMIT_ON_CHANGE,
				  "size=\"15\" required=\"required\"");
	       HTM_TD_End ();

	       /* WWW */
	       HTM_TD_Begin ("class=\"LM\"");
		  snprintf (StrName,sizeof (StrName),"WWW_%s",Lan_STR_LANG_ID[Lan]);
		  HTM_INPUT_URL (StrName,Cty_EditingCty->WWW[Lan],HTM_DONT_SUBMIT_ON_CHANGE,
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

void Cty_ReceiveFormNewCountry (void)
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
   else if (Cty_DB_CheckIfNumericCountryCodeExists (Cty_EditingCty->CtyCod))
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
         if (Cty_DB_CheckIfAlpha2CountryCodeExists (Cty_EditingCty->Alpha2))
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
               snprintf (ParamName,sizeof (ParamName),"Name_%s",Lan_STR_LANG_ID[Lan]);
               Par_GetParToText (ParamName,Cty_EditingCty->Name[Lan],Cty_MAX_BYTES_NAME);

               if (Cty_EditingCty->Name[Lan][0])	// If there's a country name
                 {
                  /***** If name of country was in database... *****/
                  if (Cty_DB_CheckIfCountryNameExists (Lan,Cty_EditingCty->Name[Lan],-1L))
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

               snprintf (ParamName,sizeof (ParamName),"WWW_%s",Lan_STR_LANG_ID[Lan]);
               Par_GetParToText (ParamName,Cty_EditingCty->WWW[Lan],Cns_MAX_BYTES_WWW);
              }
           }
        }
     }

   if (CreateCountry)
     {
      Cty_DB_CreateCountry (Cty_EditingCty);	// Add new country to database
      Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_country_X,
		     Cty_EditingCty->Name);
     }
  }

/*****************************************************************************/
/*********************** Get total number of countries ***********************/
/*****************************************************************************/

unsigned Cty_GetCachedNumCtysInSys (void)
  {
   unsigned NumCtys;

   /***** Get number of countries from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTYS,HieLvl_SYS,-1L,
                                   FigCch_UNSIGNED,&NumCtys))
     {
      /***** Get current number of countries from database and update cache *****/
      NumCtys = (unsigned) DB_GetNumRowsTable ("cty_countrs");
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTYS,HieLvl_SYS,-1L,
                                    FigCch_UNSIGNED,&NumCtys);
     }

   return NumCtys;
  }

/*****************************************************************************/
/***************** Get number of countries with institutions *****************/
/*****************************************************************************/

unsigned Cty_GetCachedNumCtysWithInss (void)
  {
   unsigned NumCtysWithInss;

   /***** Get number of countries with institutions from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTYS_WITH_INSS,HieLvl_SYS,-1L,
				   FigCch_UNSIGNED,&NumCtysWithInss))
     {
      /***** Get current number of countries with institutions from cache *****/
      NumCtysWithInss = Cty_DB_GetNumCtysWithInss ();
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTYS_WITH_INSS,HieLvl_SYS,-1L,
				    FigCch_UNSIGNED,&NumCtysWithInss);
     }

   return NumCtysWithInss;
  }

/*****************************************************************************/
/******************* Get number of countries with centers ********************/
/*****************************************************************************/

unsigned Cty_GetCachedNumCtysWithCtrs (void)
  {
   unsigned NumCtysWithCtrs;

   /***** Get number of countries with centers from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTYS_WITH_CTRS,HieLvl_SYS,-1L,
				   FigCch_UNSIGNED,&NumCtysWithCtrs))
     {
      /***** Get current number of countries with centers from database and update cache *****/
      NumCtysWithCtrs = Cty_DB_GetNumCtysWithCtrs ();
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTYS_WITH_CTRS,HieLvl_SYS,-1L,
				    FigCch_UNSIGNED,&NumCtysWithCtrs);
     }

   return NumCtysWithCtrs;
  }

/*****************************************************************************/
/******************* Get number of countries with degrees ********************/
/*****************************************************************************/

unsigned Cty_GetCachedNumCtysWithDegs (void)
  {
   unsigned NumCtysWithDegs;

   /***** Get number of countries with degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTYS_WITH_DEGS,HieLvl_SYS,-1L,
				   FigCch_UNSIGNED,&NumCtysWithDegs))
     {
      /***** Get current number of countries with degrees from database and update cache *****/
      NumCtysWithDegs = Cty_DB_GetNumCtysWithDegs ();
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTYS_WITH_DEGS,HieLvl_SYS,-1L,
				    FigCch_UNSIGNED,&NumCtysWithDegs);
     }

   return NumCtysWithDegs;
  }

/*****************************************************************************/
/******************* Get number of countries with courses ********************/
/*****************************************************************************/

unsigned Cty_GetCachedNumCtysWithCrss (void)
  {
   unsigned NumCtysWithCrss;

   /***** Get number of countries with courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTYS_WITH_CRSS,HieLvl_SYS,-1L,
				   FigCch_UNSIGNED,&NumCtysWithCrss))
     {
      /***** Get current number of countries with courses from database and update cache *****/
      NumCtysWithCrss = Cty_DB_GetNumCtysWithCrss ();
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTYS_WITH_CRSS,HieLvl_SYS,-1L,
				    FigCch_UNSIGNED,&NumCtysWithCrss);
     }

   return NumCtysWithCrss;
  }

/*****************************************************************************/
/******************* Get number of countries with users **********************/
/*****************************************************************************/

unsigned Cty_GetCachedNumCtysWithUsrs (Rol_Role_t Role)
  {
   static const FigCch_FigureCached_t FigureCtys[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_CTYS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_CTYS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_CTYS_WITH_TCHS,	// Teachers
     };
   unsigned NumCtysWithUsrs;
   long Cod = Sco_GetCurrentCod ();

   /***** Get number of countries with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureCtys[Role],Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumCtysWithUsrs))
     {
      /***** Get current number of countries with users from database and update cache *****/
      NumCtysWithUsrs = Cty_DB_GetNumCtysWithUsrs (Role,Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigureCtys[Role],Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumCtysWithUsrs);
     }

   return NumCtysWithUsrs;
  }

/*****************************************************************************/
/***************************** List countries found **************************/
/*****************************************************************************/

void Cty_ListCtysFound (MYSQL_RES **mysql_res,unsigned NumCtys)
  {
   extern const char *Txt_country;
   extern const char *Txt_countries;
   unsigned NumCty;
   struct Cty_Countr Cty;

   /***** Query database *****/
   if (NumCtys)
     {
      /***** Begin box and table *****/
      /* Number of countries found */
      Box_BoxTableBegin (NULL,Str_BuildStringLongStr ((long) NumCtys,
						      NumCtys == 1 ? Txt_country :
								     Txt_countries),
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      Str_FreeString ();

	 /***** Write heading *****/
	 Cty_PutHeadCountriesForSeeing (false);	// Order not selectable

	 /***** List the countries (one row per country) *****/
	 for (NumCty  = 1;
	      NumCty <= NumCtys;
	      NumCty++)
	   {
	    /* Get next country */
	    Cty.CtyCod = DB_GetNextCode (*mysql_res);

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
      Err_WrongCountrExit ();

   /***** Allocate memory for country *****/
   if ((Cty_EditingCty = malloc (sizeof (*Cty_EditingCty))) == NULL)
      Err_NotEnoughMemoryExit ();

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

static void Cty_FormToGoToMap (struct Cty_Countr *Cty)
  {
   extern const char *Txt_Map;

   if (Cty_DB_GetIfMapIsAvailable (Cty->CtyCod))
     {
      Cty_EditingCty = Cty;	// Used to pass parameter with the code of the country
      Lay_PutContextualLinkOnlyIcon (ActSeeCtyInf,NULL,
                                     Cty_PutParamGoToCty,&Cty_EditingCty->CtyCod,
				     "map-marker-alt.svg",
				     Txt_Map);
     }
  }

/*****************************************************************************/
/**** Get all my countries (those of my courses) and store them in a list ****/
/*****************************************************************************/

void Cty_GetMyCountrs (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCty;
   unsigned NumCtys;
   long CtyCod;

   /***** If my countries are yet filled, there's nothing to do *****/
   if (!Gbl.Usrs.Me.MyCtys.Filled)
     {
      Gbl.Usrs.Me.MyCtys.Num = 0;

      /***** Get my institutions from database *****/
      NumCtys = Cty_DB_GetCtysFromUsr (&mysql_res,Gbl.Usrs.Me.UsrDat.UsrCod);
      for (NumCty = 0;
	   NumCty < NumCtys;
	   NumCty++)
	{
	 /* Get next country */
	 row = mysql_fetch_row (mysql_res);

	 /* Get country code */
	 if ((CtyCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	   {
	    if (Gbl.Usrs.Me.MyCtys.Num == Cty_MAX_COUNTRS_PER_USR)
	       Err_ShowErrorAndExit ("Maximum number of countries of a user exceeded.");

	    Gbl.Usrs.Me.MyCtys.Ctys[Gbl.Usrs.Me.MyCtys.Num].CtyCod  = CtyCod;
	    Gbl.Usrs.Me.MyCtys.Ctys[Gbl.Usrs.Me.MyCtys.Num].MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);

	    Gbl.Usrs.Me.MyCtys.Num++;
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Set boolean that indicates that my institutions are yet filled *****/
      Gbl.Usrs.Me.MyCtys.Filled = true;
     }
  }

/*****************************************************************************/
/************************ Free the list of my countries ************************/
/*****************************************************************************/

void Cty_FreeMyCountrs (void)
  {
   if (Gbl.Usrs.Me.MyCtys.Filled)
     {
      /***** Reset list *****/
      Gbl.Usrs.Me.MyCtys.Filled = false;
      Gbl.Usrs.Me.MyCtys.Num    = 0;
     }
  }

/*****************************************************************************/
/********************** Check if I belong to a country **********************/
/*****************************************************************************/

bool Cty_CheckIfIBelongToCty (long CtyCod)
  {
   unsigned NumMyCty;

   /***** Fill the list with the institutions I belong to *****/
   Cty_GetMyCountrs ();

   /***** Check if the country passed as parameter is any of my countries *****/
   for (NumMyCty = 0;
        NumMyCty < Gbl.Usrs.Me.MyCtys.Num;
        NumMyCty++)
      if (Gbl.Usrs.Me.MyCtys.Ctys[NumMyCty].CtyCod == CtyCod)
         return true;
   return false;
  }
