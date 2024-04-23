// swad_country.c: countries

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

#define _GNU_SOURCE 		// For asprintf
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_country_config.h"
#include "swad_country_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_survey.h"
#include "swad_www.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Hie_Node *Cty_EditingCty = NULL;	// Static variable to keep the country being edited
long Cty_CurrentCtyCod = -1L;	// Used as parameter in contextual links

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Cty_PutHeadCountriesForSeeing (bool OrderSelectable);
static void Cty_ListOneCountryForSeeing (struct Hie_Node *Cty,unsigned NumCty);

static void Cty_PutIconsListingCountries (__attribute__((unused)) void *Args);
static void Cty_PutIconToEditCountries (void);

static void Cty_EditCountriesInternal (void);
static void Cty_PutIconsEditingCountries (__attribute__((unused)) void *Args);

static void Cty_GetFullListOfCountries (void);

static void Cty_ListCountriesForEdition (void);
static void Cty_PutParOthCtyCod (void *CtyCod);

static void Cty_UpdateCtyName (long CtyCod,const char *FldName,const char *NewCtyName);

static void Cty_ShowAlertAndButtonToGoToCty (void);
static void Cty_PutParGoToCty (void *CtyCod);

static void Cty_PutFormToCreateCountry (void);
static void Cty_PutHeadCountriesForEdition (void);

static void Cty_EditingCountryConstructor (void);
static void Cty_EditingCountryDestructor (void);

static void Cty_FormToGoToMap (struct Hie_Node *Cty);

/*****************************************************************************/
/***************** List countries with pending institutions ******************/
/*****************************************************************************/

void Cty_SeeCtyWithPendingInss (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Countries_with_pending_institutions;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Institutions_ABBREVIATION;
   extern const char *Txt_There_are_no_countries_with_requests_for_institutions_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtys;
   unsigned NumCty;
   struct Hie_Node Cty;
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
      Box_BoxTableBegin (Txt_Countries_with_pending_institutions,NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_CTY]   ,HTM_HEAD_LEFT );
	    HTM_TH (Txt_Institutions_ABBREVIATION,HTM_HEAD_RIGHT);
	 HTM_TR_End ();

	 /***** List the countries *****/
	 for (NumCty = 0, The_ResetRowColor ();
	      NumCty < NumCtys;
	      NumCty++, The_ChangeRowColor ())
	   {
	    /* Get next country */
	    row = mysql_fetch_row (mysql_res);

	    /* Get country code (row[0]) */
	    Cty.HieCod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Cty.HieCod == Gbl.Hierarchy.Node[Hie_CTY].HieCod) ? "BG_HIGHLIGHT" :
									The_GetColorRows ();

	    /* Get data of country */
	    Hie_GetDataByCod[Hie_CTY] (&Cty);

	    /* Begin row for this country */
	    HTM_TR_Begin (NULL);

	       /* Country map */
	       HTM_TD_Begin ("class=\"LM DAT_%s %s\"",
	                     The_GetSuffix (),BgColor);
		  Cty_DrawCountryMapAndNameWithLink (&Cty,ActSeeIns,
						     "COUNTRY_SMALL",
						     "COUNTRY_MAP_SMALL");
	       HTM_TD_End ();

	       /* Number of pending institutions (row[1]) */
	       HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
	                     The_GetSuffix (),BgColor);
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
   Gbl.Hierarchy.List[Hie_CTY].SelectedOrder = Hie_GetParHieOrder ();

   /***** Get list of countries *****/
   Cty_GetFullListOfCountries ();
  }

void Cty_ListCountries2 (void)
  {
   extern const char *Hlp_SYSTEM_Countries;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Other_countries;
   extern const char *Txt_Country_unspecified;
   unsigned NumCty;

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Div for Google Geochart *****/
   if (Gbl.Action.Act == ActSeeCty)
     {
      HTM_DIV_Begin ("id=\"chart_div\"");
      HTM_DIV_End ();
     }

   /***** Begin box and table *****/
   Box_BoxBegin (Txt_HIERARCHY_PLURAL_Abc[Hie_CTY],
		 Cty_PutIconsListingCountries,NULL,
		 Hlp_SYSTEM_Countries,Box_NOT_CLOSABLE);
      HTM_TABLE_Begin ("TBL_SCROLL");

	 /***** Heading *****/
	 Cty_PutHeadCountriesForSeeing (true);	// Order selectable

	 /***** Write all countries and their number of users and institutions *****/
	 for (NumCty = 0;
	      NumCty < Gbl.Hierarchy.List[Hie_SYS].Num;
	      NumCty++)
	    Cty_ListOneCountryForSeeing (&Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty],NumCty + 1);

	 /***** Separation row *****/
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"8\" class=\"CM DAT_%s\"",
			  The_GetSuffix ());
	       HTM_NBSP ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Write users and institutions in other countries *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Empty (1);

	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_Other_countries);
	    HTM_TD_End ();

	    /* Number of users who claim to belong to another country */
	    HTM_TD_Unsigned (Cty_GetCachedNumUsrsWhoClaimToBelongToAnotherCty ());

	    /* Number of institutions in other countries */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_INS,	// Number of institutions...
							   Hie_CTY,0));	// ...in other countries

	    /* Number of centers in other countries */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CTR,	// Number of centers...
							   Hie_CTY,0));	// ...in other countries

	    /* Number of degrees in other countries */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
							   Hie_CTY,0));	// ...in other countries

	    /* Number of courses in other countries */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
							   Hie_CTY,0));	// ...in other countries

	    /* Number of users in courses of other countries */
	    HTM_TD_Unsigned (Enr_GetCachedNumUsrsInCrss (Hie_CTY,0,
							1 << Rol_STD |
							1 << Rol_NET |
							1 << Rol_TCH));	// Any user

	 HTM_TR_End ();

	 /***** Write users and institutions with unknown country *****/
	 HTM_TR_Begin (NULL);

	    HTM_TD_Empty (1);
	    HTM_TD_Txt_Left (Txt_Country_unspecified);

	    /* Number of users who do not claim to belong to any country */
	    HTM_TD_Unsigned (Cty_GetCachedNumUsrsWhoDontClaimToBelongToAnyCty ());

	    /* Number of institutions with unknown country */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_INS,	// Number of institutions...
							    Hie_CTY,-1L));	// ...with unknown country

	    /* Number of centers with unknown country */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CTR,	// Number of centers...
							    Hie_CTY,-1L));	// ...with unknown country

	    /* Number of degrees with unknown country */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
							    Hie_CTY,-1L));	// ...with unknown country

	    /* Number of courses with unknown country */
	    HTM_TD_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
							    Hie_CTY,-1L));	// ...with unknown country

	    HTM_TD_Unsigned (0);

	 HTM_TR_End ();

   /***** End table and box *****/
      HTM_TABLE_End ();
   Box_BoxEnd ();

   /***** Free list of countries *****/
   Hie_FreeList (Hie_SYS);
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
   Hie_Order_t Order;
   static HTM_HeadAlign Align[Hie_NUM_ORDERS] =
     {
      [Hie_ORDER_BY_NAME    ] = HTM_HEAD_LEFT,
      [Hie_ORDER_BY_NUM_USRS] = HTM_HEAD_RIGHT
     };

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);
      for (Order  = (Hie_Order_t) 0;
	   Order <= (Hie_Order_t) (Hie_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (Align[Order]);
	    if (OrderSelectable)
	      {
	       Frm_BeginForm (ActSeeCty);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_COUNTRIES_HELP_ORDER[Order],
					   "class=\"BT_LINK\"");
		     if (Order == Gbl.Hierarchy.List[Hie_SYS].SelectedOrder)
			HTM_U_Begin ();
	      }
	    HTM_Txt (Txt_COUNTRIES_ORDER[Order]);
	    if (OrderSelectable)
	      {
		     if (Order == Gbl.Hierarchy.List[Hie_SYS].SelectedOrder)
			HTM_U_End ();
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	 HTM_TH_End ();
	}
      HTM_TH (Txt_Institutions_ABBREVIATION,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Centers_ABBREVIATION     ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Degrees_ABBREVIATION     ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Courses_ABBREVIATION     ,HTM_HEAD_RIGHT);
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ List one country for seeing ************************/
/*****************************************************************************/

static void Cty_ListOneCountryForSeeing (struct Hie_Node *Cty,unsigned NumCty)
  {
   const char *BgColor;

   BgColor = (Cty->HieCod == Gbl.Hierarchy.Node[Hie_CTY].HieCod) ? "BG_HIGHLIGHT" :
							        The_GetColorRows ();

   HTM_TR_Begin (NULL);

      /***** Number of country in this list *****/
      HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
                    The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumCty);
      HTM_TD_End ();

      /***** Country map (and link to WWW if exists) *****/
      HTM_TD_Begin ("class=\"LM DAT_STRONG_%s %s\"",
                    The_GetSuffix (),BgColor);
	 Cty_DrawCountryMapAndNameWithLink (Cty,ActSeeIns,
					    "COUNTRY_SMALL",
					    "COUNTRY_MAP_SMALL");
      HTM_TD_End ();

      /***** Number of users who claim to belong to this country *****/
      HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
                    The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_CTY,Cty));
      HTM_TD_End ();

      /***** Number of institutions *****/
      HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
                    The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_INS,	// Number of institutions...
						      Hie_CTY,	// ...in country
						      Cty->HieCod));
      HTM_TD_End ();

      /***** Number of centers *****/
      HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
                    The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CTR,	// Number of centers...
						      Hie_CTY,	// ...in country
						      Cty->HieCod));
      HTM_TD_End ();

      /***** Number of degrees *****/
      HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
                    The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
						      Hie_CTY,	// ...in country
						      Cty->HieCod));
      HTM_TD_End ();

      /***** Number of courses *****/
      HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
                    The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
						      Hie_CTY,	// ...in country
						      Cty->HieCod));
      HTM_TD_End ();

      /***** Number of users in courses *****/
      HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
                    The_GetSuffix (),BgColor);
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (Hie_CTY,Cty->HieCod,
						   1 << Rol_STD |
						   1 << Rol_NET |
						   1 << Rol_TCH));	// Any user
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Put contextual icons in list of countries *****************/
/*****************************************************************************/

static void Cty_PutIconsListingCountries (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit countries *****/
   if (Cty_CheckIfICanEditCountries () == Usr_CAN)
      Cty_PutIconToEditCountries ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/********************** Check if I can edit countries ************************/
/*****************************************************************************/

Usr_Can_t Cty_CheckIfICanEditCountries (void)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
						     Usr_CAN_NOT;
  }

/*****************************************************************************/
/************************ Put icon to edit countries *************************/
/*****************************************************************************/

static void Cty_PutIconToEditCountries (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCty,NULL,NULL,NULL);
  }

/*****************************************************************************/
/********************* Draw country map and name with link *******************/
/*****************************************************************************/

void Cty_DrawCountryMapAndNameWithLink (struct Hie_Node *Cty,Act_Action_t Action,
                                        const char *ClassContainer,
                                        const char *ClassMap)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
      ParCod_PutPar (ParCod_Cty,Cty->HieCod);

      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"%s\"",ClassContainer);

	 /***** Link to action *****/
	 HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Cty->FullName),
				  "class=\"BT_LINK\"");
	 Str_FreeGoToTitle ();

	    /***** Draw country map *****/
	    Cty_DrawCountryMap (Cty,ClassMap);

	    /***** Write country name *****/
	    HTM_TxtF ("&nbsp;%s&nbsp;(%s)",Cty->FullName,Cty->ShrtName);

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

void Cty_DrawCountryMap (const struct Hie_Node *Cty,const char *Class)
  {
   char *URL;
   char *Icon;

   /***** Draw country map *****/
   if (Cty_CheckIfCountryPhotoExists (Cty))
     {
      if (asprintf (&URL,"%s/%s",Cfg_URL_ICON_COUNTRIES_PUBLIC,Cty->ShrtName) < 0)
	 Err_NotEnoughMemoryExit ();
      if (asprintf (&Icon,"%s.png",Cty->ShrtName) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_IMG (URL,Icon,Cty->FullName,"class=\"%s\"",Class);
      free (Icon);
      free (URL);
     }
   else
      Ico_PutIcon ("tr16x16.gif",Ico_UNCHANGED,
		   Cty->FullName,Class);
  }

/*****************************************************************************/
/*********************** Check if country map exists *************************/
/*****************************************************************************/

bool Cty_CheckIfCountryPhotoExists (const struct Hie_Node *Cty)
  {
   char PathMap[PATH_MAX + 1];

   snprintf (PathMap,sizeof (PathMap),"%s/%s/%s.png",
	     Cfg_PATH_ICON_COUNTRIES_PUBLIC,
	     Cty->ShrtName,
	     Cty->ShrtName);
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
	   NumCty < Gbl.Hierarchy.List[Hie_SYS].Num;
	   NumCty++)
	{
	 NumUsrsCty = Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_CTY,
							      &Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty]);
	 if (NumUsrsCty)
	   {
	    NumInss = Hie_GetCachedNumNodesInHieLvl (Hie_INS,	// Number of institutions...
						     Hie_CTY,	// ...in country
						     Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty].HieCod);

	    /* Write data of this country */
	    HTM_TxtF ("	['%s', %u, %u],\n",
		      Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty].ShrtName,
		      NumUsrsCty,NumInss);
	    if (NumUsrsCty > MaxUsrsInCountry)
	       MaxUsrsInCountry = NumUsrsCty;
	    NumCtysWithUsrs++;
	   }
	}

      /***** Write end of the script *****/
      HTM_TxtF ("	]);\n"
		"	var options = {\n"
	        "		width:300,\n"
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
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Get list of countries *****/
   Gbl.Hierarchy.List[Hie_SYS].SelectedOrder = Hie_ORDER_BY_NAME;
   Cty_GetFullListOfCountries ();

   /***** Write menu to select country *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   Box_BoxBegin (Txt_HIERARCHY_PLURAL_Abc[Hie_CTY],
                 Cty_PutIconsEditingCountries,NULL,
                 Hlp_SYSTEM_Countries,Box_NOT_CLOSABLE);

      /***** Put a form to create a new country *****/
      Cty_PutFormToCreateCountry ();

      /***** Forms to edit current countries *****/
      if (Gbl.Hierarchy.List[Hie_SYS].Num)
	 Cty_ListCountriesForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of countries *****/
   Hie_FreeList (Hie_SYS);
  }

/*****************************************************************************/
/*************** Put contextual icons in edition of countries ****************/
/*****************************************************************************/

static void Cty_PutIconsEditingCountries (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view countries *****/
   Ico_PutContextualIconToView (ActSeeCty,NULL,NULL,NULL);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/********** Get basic list of countries ordered by name of country ***********/
/*****************************************************************************/

void Cty_GetBasicListOfCountries (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCty;
   struct Hie_Node *Cty;
   // Lan_Language_t Lan;

   /***** Trivial check: if list is already got, nothing to do *****/
   if (Gbl.Hierarchy.List[Hie_SYS].Num)
      return;

   /***** Get countries from database *****/
   if ((Gbl.Hierarchy.List[Hie_SYS].Num = Cty_DB_GetCtysBasic (&mysql_res))) // Countries found...
     {
      /***** Create list with countries *****/
      if ((Gbl.Hierarchy.List[Hie_SYS].Lst = calloc ((size_t) Gbl.Hierarchy.List[Hie_SYS].Num,
							sizeof (*Gbl.Hierarchy.List[Hie_SYS].Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.List[Hie_SYS].Num;
	   NumCty++)
        {
         Cty = &(Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty]);

         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get numerical country code (row[0]) */
         if ((Cty->HieCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongCountrExit ();

	 /* Get the name of the country in current language */
	 Str_Copy (Cty->FullName,row[1],sizeof (Cty->FullName) - 1);

	 /* Reset number of users who claim to belong to country */
	 Cty->NumUsrsWhoClaimToBelong.Valid = false;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Get full list of countries with names in all languages ***********/
/********** and number of users who claim to belong to them        ***********/
/*****************************************************************************/

static void Cty_GetFullListOfCountries (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCty;
   struct Hie_Node *Cty;

   /***** Trivial check: if list is already got, nothing to do *****/
   if (Gbl.Hierarchy.List[Hie_SYS].Num)
      return;

   /***** Get countries from database *****/
   if ((Gbl.Hierarchy.List[Hie_SYS].Num = Cty_DB_GetCtysFull (&mysql_res))) // Countries found...
     {
      /***** Create list with countries *****/
      if ((Gbl.Hierarchy.List[Hie_SYS].Lst = calloc ((size_t) Gbl.Hierarchy.List[Hie_SYS].Num,
							sizeof (*Gbl.Hierarchy.List[Hie_SYS].Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.List[Hie_SYS].Num;
	   NumCty++)
        {
         Cty = &(Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty]);

         /* Get next country */
         row = mysql_fetch_row (mysql_res);

         /* Get numerical country code (row[0]) */
         if ((Cty->HieCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongCountrExit ();

         /* Get Alpha-2 country code (row[1]) */
         Str_Copy (Cty->ShrtName,row[1],sizeof (Cty->ShrtName) - 1);

	 /* Get the name and the web of the country in the current language */
	 Str_Copy (Cty->FullName,row[2],sizeof (Cty->FullName) - 1);
	 Str_Copy (Cty->WWW     ,row[3],sizeof (Cty->WWW     ) - 1);

	 /* Get number of users who claim to belong to this country */
	 Cty->NumUsrsWhoClaimToBelong.Valid = false;
	 if (sscanf (row[4],"%u",
		     &(Cty->NumUsrsWhoClaimToBelong.NumUsrs)) == 1)
	    Cty->NumUsrsWhoClaimToBelong.Valid = true;
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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   unsigned NumCty;
   const struct Hie_Node *Cty;

   /***** Get list of countries *****/
   Cty_GetBasicListOfCountries ();

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeIns);

      /***** Begin selector of country *****/
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			"id=\"cty\" name=\"cty\" class=\"HIE_SEL INPUT_%s\"",
			The_GetSuffix ());

         /***** Initial disabled option *****/
	 HTM_OPTION (HTM_Type_STRING,"",
	             Gbl.Hierarchy.Node[Hie_CTY].HieCod < 0 ? HTM_OPTION_SELECTED :
	        					      HTM_OPTION_UNSELECTED,
	             HTM_OPTION_DISABLED,
		     "[%s]",Txt_HIERARCHY_SINGUL_Abc[Hie_CTY]);

	 /***** List countries *****/
	 for (NumCty = 0;
	      NumCty < Gbl.Hierarchy.List[Hie_SYS].Num;
	      NumCty++)
	   {
	    Cty = &Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty];
	    HTM_OPTION (HTM_Type_LONG,&Cty->HieCod,
			Cty->HieCod == Gbl.Hierarchy.Node[Hie_CTY].HieCod ? HTM_OPTION_SELECTED :
									    HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%s",Cty->FullName);
	   }

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

void Cty_WriteCountryName (long CtyCod)
  {
   char CtyName[Cty_MAX_BYTES_NAME + 1];
   Frm_PutForm_t PutForm = (!Frm_CheckIfInside () &&						// Only if not inside another form
                            Act_GetBrowserTab (Gbl.Action.Act) == Act_1ST) ? Frm_PUT_FORM :	// Only in main browser tab
                        						     Frm_DONT_PUT_FORM;

   /***** Get country name *****/
   Cty_GetCountryNameInLanguage (CtyCod,Gbl.Prefs.Language,CtyName);

   switch (PutForm)
     {
      case Frm_DONT_PUT_FORM:
	 /***** Write country name without link *****/
	 HTM_Txt (CtyName);
	 break;
      case Frm_PUT_FORM:
	 /***** Write country name with link to country information *****/
	 Frm_BeginForm (ActSeeCtyInf);
	    ParCod_PutPar (ParCod_Cty,CtyCod);
	    HTM_BUTTON_Submit_Begin (Act_GetActionText (ActSeeCtyInf),
				     "class=\"BT_LINK\"");
	       HTM_Txt (CtyName);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
	 break;
     }
  }

/*****************************************************************************/
/***************** Get basic data of country given its code ******************/
/*****************************************************************************/

bool Cty_GetCountrDataByCod (struct Hie_Node *Node)
  {
   extern const char *Txt_Another_country;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Found;

   if (Node->HieCod < 0)
      return false;

   /***** Clear data *****/
   Node->ShrtName[0] = '\0';
   Node->FullName[0] = '\0';
   Node->WWW[0]      = '\0';
   Node->NumUsrsWhoClaimToBelong.Valid = false;

   /***** If another country *****/
   if (Node->HieCod == 0)
     {
      Str_Copy (Node->FullName,Txt_Another_country,sizeof (Node->FullName) - 1);
      return false;
     }

   // Here Cty->CtyCod > 0

   /***** Get data of a country from database *****/
   Found = (Cty_DB_GetBasicCountryDataByCod (&mysql_res,Node->HieCod) != 0);
   if (Found) // Country found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get Alpha-2 country code (row[0]) */
      Str_Copy (Node->ShrtName,row[0],sizeof (Node->ShrtName) - 1);

      /* Get name and WWW of the country in current language */
      Str_Copy (Node->FullName,row[1],sizeof (Node->FullName) - 1);
      Str_Copy (Node->WWW     ,row[2],sizeof (Node->WWW     ) - 1);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }

/*****************************************************************************/
/*********** Get all names and WWWs of a country given its code **************/
/*****************************************************************************/

void Cty_GetNamesAndWWWsByCod (struct Hie_Node *Cty,
			       char NameInSeveralLanguages[1 + Lan_NUM_LANGUAGES][Cty_MAX_BYTES_NAME + 1],
			       char WWWInSeveralLanguages [1 + Lan_NUM_LANGUAGES][WWW_MAX_BYTES_WWW + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Lan_Language_t Lan;

   /***** Get data of a country from database *****/
   if (Cty_DB_GetNamesAndWWWsByCod (&mysql_res,Cty->HieCod) != 0) // Country found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get names and webs of the country in several languages */
      for (Lan  = (Lan_Language_t) 1;
	   Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	   Lan++)
	{
	 Str_Copy (NameInSeveralLanguages[Lan],row[Lan - 1],
	           Cty_MAX_BYTES_NAME);
	 Str_Copy (WWWInSeveralLanguages[Lan],row[Lan_NUM_LANGUAGES + Lan - 1],
	           WWW_MAX_BYTES_WWW - 1);
	}
     }
   else
      for (Lan  = (Lan_Language_t) 1;
   	   Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
   	   Lan++)
        {
         NameInSeveralLanguages[Lan][0] = '\0';
         WWWInSeveralLanguages[Lan][0] = '\0';
        }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************************** Get country name ******************************/
/*****************************************************************************/

void Cty_FlushCacheCountryName (void)
  {
   Gbl.Cache.CountryName.Valid = false;
  }

void Cty_GetCountryNameInLanguage (long CtyCod,Lan_Language_t Language,
				   char CtyName[Cty_MAX_BYTES_NAME + 1])
  {
   /***** 1. Fast check: Trivial case *****/
   if (CtyCod <= 0)
     {
      CtyName[0] = '\0';	// Empty name
      return;
     }

   /***** 2. Fast check: If cached... *****/
   if (Gbl.Cache.CountryName.Valid &&
       CtyCod   == Gbl.Cache.CountryName.HieCod &&
       Language == Gbl.Cache.CountryName.Language)
     {
      Str_Copy (CtyName,Gbl.Cache.CountryName.CtyName,Cty_MAX_BYTES_NAME);
      return;
     }

   /***** 3. Slow: get country name from database *****/
   Cty_DB_GetCountryName (CtyCod,Language,CtyName);
   Gbl.Cache.CountryName.HieCod   = CtyCod;
   Gbl.Cache.CountryName.Language = Language;
   Str_Copy (Gbl.Cache.CountryName.CtyName,CtyName,Cty_MAX_BYTES_NAME);
   Gbl.Cache.CountryName.Valid = true;
  }

/*****************************************************************************/
/***************************** List all countries ****************************/
/*****************************************************************************/

static void Cty_ListCountriesForEdition (void)
  {
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   unsigned NumCty;
   struct Hie_Node *Cty;
   unsigned NumInss;
   unsigned NumUsrsCty;
   Lan_Language_t Lan;
   char NameInSeveralLanguages[1 + Lan_NUM_LANGUAGES][Cty_MAX_BYTES_NAME + 1];
   char WWWInSeveralLanguages [1 + Lan_NUM_LANGUAGES][WWW_MAX_BYTES_WWW + 1];

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Cty_PutHeadCountriesForEdition ();

      /***** Write all countries *****/
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.List[Hie_SYS].Num;
	   NumCty++)
	{
	 Cty = &Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty];
	 NumInss = Hie_GetNumNodesInHieLvl (Hie_INS,	// Number of institutions...
					    Hie_CTY,	// ...in country
					    Cty->HieCod);
	 NumUsrsCty = Hie_GetNumUsrsWhoClaimToBelongTo (Hie_CTY,Cty);

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove country */
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"BT\"",1 + Lan_NUM_LANGUAGES);
	       if (NumInss ||					// Country has institutions
		   NumUsrsCty)					// Country has users
		  // Deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else if (Enr_GetNumUsrsInCrss (Hie_CTY,Cty->HieCod,
					      1 << Rol_STD |
					      1 << Rol_NET |
					      1 << Rol_TCH))	// Country has users
		  // Deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else
		  Ico_PutContextualIconToRemove (ActRemCty,NULL,
						 Cty_PutParOthCtyCod,&Cty->HieCod);
	    HTM_TD_End ();

	    /* Numerical country code (ISO 3166-1) */
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"RT DAT_%s\"",
	                  1 + Lan_NUM_LANGUAGES,The_GetSuffix ());
	       HTM_TxtF ("%03ld",Cty->HieCod);
	    HTM_TD_End ();

	    /* Alphabetic country code with 2 letters (ISO 3166-1) */
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"RT DAT_%s\"",
	                  1 + Lan_NUM_LANGUAGES,The_GetSuffix ());
	       HTM_Txt (Cty->ShrtName);
	    HTM_TD_End ();

	    HTM_TD_Empty (3);

	    /* Number of users */
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"RT DAT_%s\"",
	                  1 + Lan_NUM_LANGUAGES,The_GetSuffix ());
	       HTM_Unsigned (NumUsrsCty);
	    HTM_TD_End ();

	    /* Number of institutions */
	    HTM_TD_Begin ("rowspan=\"%u\" class=\"RT DAT_%s\"",
	                  1 + Lan_NUM_LANGUAGES,The_GetSuffix ());
	       HTM_Unsigned (NumInss);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /* Country name in several languages */
	 Cty_GetNamesAndWWWsByCod (Cty,
				   NameInSeveralLanguages,
				   WWWInSeveralLanguages);
	 for (Lan  = (Lan_Language_t) 1;
	      Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	      Lan++)
	   {
	    HTM_TR_Begin (NULL);

	       /* Language */
	       HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
		  HTM_TxtColon (Txt_STR_LANG_NAME[Lan]);
	       HTM_TD_End ();

	       /* Name */
	       HTM_TD_Begin ("class=\"LT\"");
		  Frm_BeginForm (ActRenCty);
		     ParCod_PutPar (ParCod_OthCty,Cty->HieCod);
		     Par_PutParUnsigned (NULL,"Lan",(unsigned) Lan);
		     HTM_INPUT_TEXT ("Name",Cty_MAX_CHARS_NAME,NameInSeveralLanguages[Lan],
				     HTM_SUBMIT_ON_CHANGE,
				     "size=\"15\" class=\"INPUT_%s\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
	       HTM_TD_End ();

	       /* WWW */
	       HTM_TD_Begin ("class=\"LT\"");
		  Frm_BeginForm (ActChgCtyWWW);
		     ParCod_PutPar (ParCod_OthCty,Cty->HieCod);
		     Par_PutParUnsigned (NULL,"Lan",(unsigned) Lan);
		     HTM_INPUT_URL ("WWW",WWWInSeveralLanguages[Lan],HTM_SUBMIT_ON_CHANGE,
				    "class=\"INPUT_WWW INPUT_%s\""
				    " required=\"required\"",
				    The_GetSuffix ());
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

static void Cty_PutParOthCtyCod (void *CtyCod)
  {
   if (CtyCod)
      ParCod_PutPar (ParCod_OthCty,*((long *) CtyCod));
  }

/*****************************************************************************/
/****************************** Remove a country *****************************/
/*****************************************************************************/

void Cty_RemoveCountry (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_You_can_not_remove_a_country_with_institutions_or_users;
   extern const char *Txt_Country_X_removed;

   /***** Country constructor *****/
   Cty_EditingCountryConstructor ();

   /***** Get country code *****/
   Cty_EditingCty->HieCod = ParCod_GetAndCheckPar (ParCod_OthCty);

   /***** Get data of the country from database *****/
   Hie_GetDataByCod[Hie_CTY] (Cty_EditingCty);

   /***** Check if this country has users *****/
   if (Hie_GetNumNodesInHieLvl (Hie_INS,	// Number of institutions...
				Hie_CTY,	// ...in country
				Cty_EditingCty->HieCod))			// Country has institutions ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else if (Hie_GetNumUsrsWhoClaimToBelongTo (Hie_CTY,Cty_EditingCty))	// Country has users ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else if (Enr_GetNumUsrsInCrss (Hie_CTY,Cty_EditingCty->HieCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))			// Country has users
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_remove_a_country_with_institutions_or_users);
   else	// Country has no users ==> remove it
     {
      /***** Remove surveys of the country *****/
      Svy_RemoveSurveys (Hie_CTY,Cty_EditingCty->HieCod);

      /***** Remove country *****/
      Cty_DB_RemoveCty (Cty_EditingCty->HieCod);

      /***** Flush cache *****/
      Cty_FlushCacheCountryName ();
      Hie_FlushCachedNumNodesInHieLvl (Hie_INS,Hie_CTY);	// Number of institutions in country
      Hie_FlushCachedNumNodesInHieLvl (Hie_CTR,Hie_CTY);	// Number of centers in country
      Hie_FlushCachedNumNodesInHieLvl (Hie_DEG,Hie_CTY);	// Number of degrees in country
      Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_CTY);	// Number of courses in country
      Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_CTY);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Country_X_removed,
	               Cty_EditingCty->FullName);

      Cty_EditingCty->HieCod = -1L;	// To not showing button to go to country
     }
  }

/*****************************************************************************/
/************************ Change the name of a country ***********************/
/*****************************************************************************/

void Cty_RenameCountry (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_country_X_already_exists;
   extern const char *Txt_The_country_X_has_been_renamed_as_Y;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_The_name_X_has_not_changed;
   char OldCtyName[Cty_MAX_BYTES_NAME + 1];
   char NewCtyName[Cty_MAX_BYTES_NAME + 1];
   Lan_Language_t Language;
   char FldName[4 + 1 + 2 + 1];	// Example: "Name_en"

   /***** Country constructor *****/
   Cty_EditingCountryConstructor ();

   /***** Get the code of the country *****/
   Cty_EditingCty->HieCod = ParCod_GetAndCheckPar (ParCod_OthCty);

   /***** Get the lenguage *****/
   Language = Lan_GetParLanguage ();

   /***** Get the new name for the country *****/
   Par_GetParText ("Name",NewCtyName,Cty_MAX_BYTES_NAME);

   /***** Get from the database the data of the country *****/
   Hie_GetDataByCod[Hie_CTY] (Cty_EditingCty);

   /***** Check if new name is empty *****/
   if (NewCtyName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      Cty_GetCountryNameInLanguage (Cty_EditingCty->HieCod,Language,OldCtyName);
      if (strcmp (OldCtyName,NewCtyName))	// Different names
	{
	 /***** If country was in database... *****/
	 if (Cty_DB_CheckIfCountryNameExists (Language,NewCtyName,Cty_EditingCty->HieCod))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_country_X_already_exists,
		             NewCtyName);
	 else
	   {
	    /* Update the table changing old name by new name */
	    snprintf (FldName,sizeof (FldName),"Name_%s",
		      Lan_STR_LANG_ID[Language]);
	    Cty_UpdateCtyName (Cty_EditingCty->HieCod,FldName,NewCtyName);

	    /* Write message to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_The_country_X_has_been_renamed_as_Y,
		             OldCtyName,NewCtyName);
	   }
	}
      else	// The same name
	 Ale_CreateAlert (Ale_INFO,NULL,
	                  Txt_The_name_X_has_not_changed,
	                  OldCtyName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/************ Update institution name in table of institutions ***************/
/*****************************************************************************/

static void Cty_UpdateCtyName (long CtyCod,const char *FldName,const char *NewCtyName)
  {
   /***** Update country changing old name by new name */
   Cty_DB_UpdateCtyField (CtyCod,FldName,NewCtyName);

   /***** Flush cache *****/
   Cty_FlushCacheCountryName ();
  }

/*****************************************************************************/
/************************ Change the URL of a country ************************/
/*****************************************************************************/

void Cty_ChangeCtyWWW (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[WWW_MAX_BYTES_WWW + 1];
   Lan_Language_t Language;
   char FldName[3 + 1 + 2 + 1];	// Example: "WWW_en"

   /***** Country constructor *****/
   Cty_EditingCountryConstructor ();

   /***** Get the code of the country *****/
   Cty_EditingCty->HieCod = ParCod_GetAndCheckPar (ParCod_OthCty);

   /***** Get the lenguage *****/
   Language = Lan_GetParLanguage ();

   /***** Get the new WWW for the country *****/
   Par_GetParText ("WWW",NewWWW,WWW_MAX_BYTES_WWW);

   /***** Get from the database the data of the country *****/
   Hie_GetDataByCod[Hie_CTY] (Cty_EditingCty);

   /***** Update the table changing old WWW by new WWW *****/
   snprintf (FldName,sizeof (FldName),"WWW_%s",
	     Lan_STR_LANG_ID[Language]);
   Cty_DB_UpdateCtyField (Cty_EditingCty->HieCod,FldName,NewWWW);

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
   if (Cty_EditingCty->HieCod != Gbl.Hierarchy.Node[Hie_CTY].HieCod)
     {
      /***** Alert with button to go to couuntry *****/
      Ale_ShowLastAlertAndButton (ActSeeIns,NULL,NULL,
                                  Cty_PutParGoToCty,&Cty_EditingCty->HieCod,
                                  Btn_CONFIRM_BUTTON,
				  Str_BuildGoToTitle (Cty_EditingCty->FullName));
      Str_FreeGoToTitle ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Cty_PutParGoToCty (void *CtyCod)
  {
   if (CtyCod)
      ParCod_PutPar (ParCod_Cty,*((long *) CtyCod));
  }

/*****************************************************************************/
/********************* Put a form to create a new country ********************/
/*****************************************************************************/

static void Cty_PutFormToCreateCountry (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_STR_LANG_NAME[1 + Lan_NUM_LANGUAGES];
   Lan_Language_t Lan;
   char StrCtyCod[Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char StrName[32];

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewCty,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Cty_PutHeadCountriesForEdition ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove country, disabled here *****/
	 HTM_TD_Begin ("rowspan=\"%u\" class=\"BT\"",1 + Lan_NUM_LANGUAGES);
	 HTM_TD_End ();

	 /***** Numerical country code (ISO 3166-1) *****/
	 HTM_TD_Begin ("rowspan=\"%u\" class=\"RT\"",1 + Lan_NUM_LANGUAGES);
	    if (Cty_EditingCty->HieCod > 0)
	       snprintf (StrCtyCod,sizeof (StrCtyCod),"%03ld",Cty_EditingCty->HieCod);
	    else
	       StrCtyCod[0] = '\0';
	    HTM_INPUT_TEXT (Par_CodeStr[ParCod_OthCty],3,StrCtyCod,HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"3\" class=\"INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Alphabetic country code with 2 letters (ISO 3166-1) *****/
	 HTM_TD_Begin ("rowspan=\"%u\" class=\"RT\"",1 + Lan_NUM_LANGUAGES);
	    HTM_INPUT_TEXT ("Alpha2",2,Cty_EditingCty->ShrtName,HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"2\" class=\"INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 HTM_TD_Empty (3);

	 /***** Number of users *****/
	 HTM_TD_Begin ("rowspan=\"%u\" class=\"RT DAT_%s\"",
		       1 + Lan_NUM_LANGUAGES,The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Number of institutions *****/
	 HTM_TD_Begin ("rowspan=\"%u\" class=\"RT DAT_%s\"",
		       1 + Lan_NUM_LANGUAGES,The_GetSuffix ());
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
	    HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	       HTM_Txt (Txt_STR_LANG_NAME[Lan]);
	    HTM_TD_End ();

	    /* Name */
	    HTM_TD_Begin ("class=\"LM\"");
	       snprintf (StrName,sizeof (StrName),"Name_%s",Lan_STR_LANG_ID[Lan]);
	       HTM_INPUT_TEXT (StrName,Cty_MAX_CHARS_NAME,"",
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "size=\"15\" class=\"INPUT_%s\""
			       " required=\"required\"",
			       The_GetSuffix ());
	    HTM_TD_End ();

	    /* WWW */
	    HTM_TD_Begin ("class=\"LM\"");
	       snprintf (StrName,sizeof (StrName),"WWW_%s",Lan_STR_LANG_ID[Lan]);
	       HTM_INPUT_URL (StrName,"",HTM_DONT_SUBMIT_ON_CHANGE,
			      "class=\"INPUT_WWW INPUT_%s\""
			      " required=\"required\"",
			      The_GetSuffix ());
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
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

      HTM_TH_Span (NULL                           ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Numeric_BR_code_BR_ISO_3166_1   ,HTM_HEAD_RIGHT );
      HTM_TH (Txt_Alphabetic_BR_code_BR_ISO_3166_1,HTM_HEAD_RIGHT );
      HTM_TH_Empty (1);
      HTM_TH (Txt_Name                            ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_WWW                             ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Users                           ,HTM_HEAD_RIGHT );
      HTM_TH (Txt_Institutions_ABBREVIATION       ,HTM_HEAD_RIGHT );

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new country ********************/
/*****************************************************************************/

void Cty_ReceiveNewCountry (void)
  {
   extern const char *Txt_You_must_specify_the_numerical_code_of_the_new_country;
   extern const char *Txt_The_numerical_code_X_already_exists;
   extern const char *Txt_The_alphabetical_code_X_is_not_correct;
   extern const char *Txt_The_alphabetical_code_X_already_exists;
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_The_country_X_already_exists;
   extern const char *Txt_You_must_specify_the_name;
   extern const char *Txt_Created_new_country_X;
   char ParName[32];
   bool CreateCountry = true;
   Lan_Language_t Lan;
   char NameInSeveralLanguages[1 + Lan_NUM_LANGUAGES][Cty_MAX_BYTES_NAME + 1];
   char WWWInSeveralLanguages [1 + Lan_NUM_LANGUAGES][WWW_MAX_BYTES_WWW + 1];
   unsigned i;

   /***** Country constructoor *****/
   Cty_EditingCountryConstructor ();
   for (Lan  = (Lan_Language_t) 1;
	Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
	Lan++)
     {
      NameInSeveralLanguages[Lan][0] = '\0';
      WWWInSeveralLanguages [Lan][0] = '\0';
     }

   /***** Get parameters from form *****/
   /* Get numeric country code */
   if ((Cty_EditingCty->HieCod = ParCod_GetPar (ParCod_OthCty)) < 0)
     {
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_numerical_code_of_the_new_country);
      CreateCountry = false;
     }
   else if (Cty_DB_CheckIfNumericCountryCodeExists (Cty_EditingCty->HieCod))
     {
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_The_numerical_code_X_already_exists,
                       Cty_EditingCty->HieCod);
      CreateCountry = false;
     }
   else	// Numeric code correct
     {
      /* Get alphabetic-2 country code */
      Par_GetParText ("Alpha2",Cty_EditingCty->ShrtName,2);
      Str_ConvertToUpperText (Cty_EditingCty->ShrtName);
      for (i = 0;
	   i < 2 && CreateCountry;
	   i++)
         if (Cty_EditingCty->ShrtName[i] < 'A' ||
             Cty_EditingCty->ShrtName[i] > 'Z')
           {
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_alphabetical_code_X_is_not_correct,
                             Cty_EditingCty->ShrtName);
            CreateCountry = false;
           }
      if (CreateCountry)
        {
         if (Cty_DB_CheckIfAlpha2CountryCodeExists (Cty_EditingCty->ShrtName))
           {
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_alphabetical_code_X_already_exists,
                             Cty_EditingCty->ShrtName);
            CreateCountry = false;
           }
         else	// Alphabetic code correct
           {
            /* Get country name and WWW in different languages */
            for (Lan  = (Lan_Language_t) 1;
        	 Lan <= (Lan_Language_t) Lan_NUM_LANGUAGES;
        	 Lan++)
              {
               snprintf (ParName,sizeof (ParName),"Name_%s",Lan_STR_LANG_ID[Lan]);
               Par_GetParText (ParName,NameInSeveralLanguages[Lan],Cty_MAX_BYTES_NAME);

               if (NameInSeveralLanguages[Lan][0])	// If there's a country name
                 {
                  /***** If name of country was in database... *****/
                  if (Cty_DB_CheckIfCountryNameExists (Lan,NameInSeveralLanguages[Lan],-1L))
                    {
                     Ale_CreateAlert (Ale_WARNING,NULL,
                	              Txt_The_country_X_already_exists,
                                      NameInSeveralLanguages[Lan]);
                     CreateCountry = false;
                     break;
                    }
                 }
               else	// If there is not a country name
                 {
                  Ale_CreateAlert (Ale_WARNING,NULL,
                	           Txt_You_must_specify_the_name);
                  CreateCountry = false;
                  break;
                 }

               snprintf (ParName,sizeof (ParName),"WWW_%s",Lan_STR_LANG_ID[Lan]);
               Par_GetParText (ParName,WWWInSeveralLanguages[Lan],WWW_MAX_BYTES_WWW);
              }
           }
        }
     }

   if (CreateCountry)
     {
      Cty_DB_CreateCountry (Cty_EditingCty,	// Add new country to database
	   		    NameInSeveralLanguages,
	   		    WWWInSeveralLanguages);

      Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_country_X,
		     Cty_EditingCty->FullName);
     }
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
   long Cod = Hie_GetCurrentCod ();

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
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   char *Title;
   unsigned NumCty;
   struct Hie_Node Cty;

   /***** Query database *****/
   if (NumCtys)
     {
      /***** Begin box and table *****/
      /* Number of countries found */
      if (asprintf (&Title,"%u %s",NumCtys,
				   NumCtys == 1 ? Txt_HIERARCHY_SINGUL_abc[Hie_CTY] :
						  Txt_HIERARCHY_PLURAL_abc[Hie_CTY]) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

	 /***** Write heading *****/
	 Cty_PutHeadCountriesForSeeing (false);	// Order not selectable

	 /***** List the countries (one row per country) *****/
	 for (NumCty  = 1, The_ResetRowColor ();
	      NumCty <= NumCtys;
	      NumCty++, The_ChangeRowColor ())
	   {
	    /* Get next country */
	    Cty.HieCod = DB_GetNextCode (*mysql_res);

	    /* Get data of country */
	    Hie_GetDataByCod[Hie_CTY] (&Cty);

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
   /***** Pointer must be NULL *****/
   if (Cty_EditingCty != NULL)
      Err_WrongCountrExit ();

   /***** Allocate memory for country *****/
   if ((Cty_EditingCty = malloc (sizeof (*Cty_EditingCty))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset country *****/
   Cty_EditingCty->HieCod         = -1L;
   Cty_EditingCty->ShrtName[0] = '\0';
   Cty_EditingCty->FullName[0] = '\0';
   Cty_EditingCty->WWW[0]      = '\0';
   Cty_EditingCty->NumUsrsWhoClaimToBelong.Valid = false;
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

static void Cty_FormToGoToMap (struct Hie_Node *Cty)
  {
   if (Cty_DB_CheckIfMapIsAvailable (Cty->HieCod))
     {
      Cty_EditingCty = Cty;	// Used to pass parameter with the code of the country
      Lay_PutContextualLinkOnlyIcon (ActSeeCtyInf,NULL,
                                     Cty_PutParGoToCty,&Cty_EditingCty->HieCod,
				     "map-marker-alt.svg",Ico_BLACK);
     }
  }

/*****************************************************************************/
/******* Get number of users who don't claim to belong to any country ********/
/*****************************************************************************/

void Cty_FlushCacheNumUsrsWhoDontClaimToBelongToAnyCty (void)
  {
   Gbl.Cache.NumUsrsWhoDontClaimToBelongToAnyCty.Valid = false;
  }

unsigned Cty_GetCachedNumUsrsWhoDontClaimToBelongToAnyCty (void)
  {
   unsigned NumUsrs;

   /***** Get number of user who don't claim to belong to any country from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_USRS_BELONG_CTY,Hie_CTY,-1L,
				   FigCch_UNSIGNED,&NumUsrs))
      /***** Get current number of user who don't claim to belong to any country from database and update cache *****/
      NumUsrs = Cty_GetNumUsrsWhoDontClaimToBelongToAnyCty ();

   return NumUsrs;
  }

unsigned Cty_GetNumUsrsWhoDontClaimToBelongToAnyCty (void)
  {
   /***** 1. Fast check: If cached... *****/
   if (Gbl.Cache.NumUsrsWhoDontClaimToBelongToAnyCty.Valid)
      return Gbl.Cache.NumUsrsWhoDontClaimToBelongToAnyCty.NumUsrs;

   /***** 2. Slow: number of users who don't claim to belong to any country
                   from database *****/
   Gbl.Cache.NumUsrsWhoDontClaimToBelongToAnyCty.NumUsrs = Cty_DB_GetNumUsrsWhoDontClaimToBelongToAnyCty ();
   Gbl.Cache.NumUsrsWhoDontClaimToBelongToAnyCty.Valid = true;
   FigCch_UpdateFigureIntoCache (FigCch_NUM_USRS_BELONG_CTY,Hie_CTY,-1L,
				 FigCch_UNSIGNED,&Gbl.Cache.NumUsrsWhoDontClaimToBelongToAnyCty.NumUsrs);
   return Gbl.Cache.NumUsrsWhoDontClaimToBelongToAnyCty.NumUsrs;
  }

/*****************************************************************************/
/******** Get number of users who claim to belong to another country *********/
/*****************************************************************************/

void Cty_FlushCacheNumUsrsWhoClaimToBelongToAnotherCty (void)
  {
   Gbl.Cache.NumUsrsWhoClaimToBelongToAnotherCty.Valid = false;
  }

unsigned Cty_GetCachedNumUsrsWhoClaimToBelongToAnotherCty (void)
  {
   unsigned NumUsrsCty;

   /***** Get number of users who claim to belong to another country form cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_USRS_BELONG_CTY,Hie_CTY,0,
                                   FigCch_UNSIGNED,&NumUsrsCty))
      /***** Get current number of users who claim to belong to another country from database and update cache *****/
      NumUsrsCty = Cty_GetNumUsrsWhoClaimToBelongToAnotherCty ();

   return NumUsrsCty;
  }

unsigned Cty_GetNumUsrsWhoClaimToBelongToAnotherCty (void)
  {
   /***** 1. Fast check: If cached... *****/
   if (Gbl.Cache.NumUsrsWhoClaimToBelongToAnotherCty.Valid)
      return Gbl.Cache.NumUsrsWhoClaimToBelongToAnotherCty.NumUsrs;

   /***** 2. Slow: number of users who claim to belong to another country
                   from database *****/
   Gbl.Cache.NumUsrsWhoClaimToBelongToAnotherCty.NumUsrs = Cty_DB_GetNumUsrsWhoClaimToBelongToAnotherCty ();
   Gbl.Cache.NumUsrsWhoClaimToBelongToAnotherCty.Valid = true;
   FigCch_UpdateFigureIntoCache (FigCch_NUM_USRS_BELONG_CTY,Hie_CTY,0,
				 FigCch_UNSIGNED,&Gbl.Cache.NumUsrsWhoClaimToBelongToAnotherCty.NumUsrs);
   return Gbl.Cache.NumUsrsWhoClaimToBelongToAnotherCty.NumUsrs;
  }
