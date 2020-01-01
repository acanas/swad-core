// swad_country_config.c:  configuration of current country

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy_config.h"
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

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void CtyCfg_Configuration (bool PrintView);
static void CtyCfg_PutIconToPrint (void);
static void CtyCfg_Title (bool PutLink);
static void CtyCfg_Map (bool PrintView,bool PutLink);
static void CtyCfg_Name (bool PutLink);
static void CtyCfg_Shortcut (bool PrintView);
static void CtyCfg_QR (void);
static void CtyCfg_NumUsrs (void);
static void CtyCfg_NumInss (void);
static void CtyCfg_NumCtrs (void);
static void CtyCfg_NumDegs (void);
static void CtyCfg_NumCrss (void);
static void CtyCfg_NumUsrsInCrssOfCty (Rol_Role_t Role);

static void CtyCfg_GetMapAttr (long CtyCod,char **MapAttribution);
static void CtyCfg_FreeMapAttr (char **MapAttribution);

/*****************************************************************************/
/***************** Show information of the current country *******************/
/*****************************************************************************/

void CtyCfg_ShowConfiguration (void)
  {
   CtyCfg_Configuration (false);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/***************** Print information of the current country ******************/
/*****************************************************************************/

void CtyCfg_PrintConfiguration (void)
  {
   CtyCfg_Configuration (true);
  }

/*****************************************************************************/
/******************** Information of the current country *********************/
/*****************************************************************************/

static void CtyCfg_Configuration (bool PrintView)
  {
   extern const char *Hlp_COUNTRY_Information;
   bool PutLink;
   bool MapExists;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Cty.CtyCod <= 0)		// No country selected
      return;

   /***** Initializations *****/
   PutLink = !PrintView && Gbl.Hierarchy.Cty.WWW[Gbl.Prefs.Language][0];

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,CtyCfg_PutIconToPrint,
		    Hlp_COUNTRY_Information,Box_NOT_CLOSABLE);

   /***** Title *****/
   CtyCfg_Title (PutLink);

   /**************************** Left part ***********************************/
   HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Country name (an link to WWW if exists) *****/
   CtyCfg_Name (PutLink);

   /***** Shortcut to the country *****/
   CtyCfg_Shortcut (PrintView);

   if (PrintView)
      /***** QR code with link to the country *****/
      CtyCfg_QR ();
   else
     {
      /***** Number of users who claim to belong to this centre,
             number of institutions,
             number of centres,
             number of degrees,
             number of courses *****/
      CtyCfg_NumUsrs ();
      CtyCfg_NumInss ();
      CtyCfg_NumCtrs ();
      CtyCfg_NumDegs ();
      CtyCfg_NumCrss ();

      /***** Number of users in courses of this country *****/
      CtyCfg_NumUsrsInCrssOfCty (Rol_TCH);
      CtyCfg_NumUsrsInCrssOfCty (Rol_NET);
      CtyCfg_NumUsrsInCrssOfCty (Rol_STD);
      CtyCfg_NumUsrsInCrssOfCty (Rol_UNK);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End of left part *****/
   HTM_DIV_End ();

   /**************************** Right part **********************************/
   /***** Check map *****/
   MapExists = Cty_CheckIfCountryMapExists (&Gbl.Hierarchy.Cty);

   if (MapExists)
     {
      HTM_DIV_Begin ("class=\"HIE_CFG_RIGHT\"");

      /***** Country map *****/
      CtyCfg_Map (PrintView,PutLink);

      HTM_DIV_End ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Put icon to print the configuration of a country **************/
/*****************************************************************************/

static void CtyCfg_PutIconToPrint (void)
  {
   Ico_PutContextualIconToPrint (ActPrnCtyInf,NULL);
  }

/*****************************************************************************/
/******************** Show title in country configuration ********************/
/*****************************************************************************/

static void CtyCfg_Title (bool PutLink)
  {
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG\"");
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\""
	           " class=\"FRAME_TITLE_BIG\" title=\"%s\"",
	           Gbl.Hierarchy.Cty.WWW[Gbl.Prefs.Language],
	           Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   HTM_Txt (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   if (PutLink)
      HTM_A_End ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************* Show map in country configuration *********************/
/*****************************************************************************/

static void CtyCfg_Map (bool PrintView,bool PutLink)
  {
   char *MapAttribution = NULL;

   /***** Get map attribution *****/
   CtyCfg_GetMapAttr (Gbl.Hierarchy.Cty.CtyCod,&MapAttribution);

   /***** Map image *****/
   HTM_DIV_Begin ("class=\"DAT_SMALL CM\"");
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\"",
		   Gbl.Hierarchy.Cty.WWW[Gbl.Prefs.Language]);
   Cty_DrawCountryMap (&Gbl.Hierarchy.Cty,PrintView ? "COUNTRY_MAP_PRINT" :
						      "COUNTRY_MAP_SHOW");
   if (PutLink)
      HTM_A_End ();
   HTM_DIV_End ();

   /***** Map attribution *****/
   if (!PrintView && Cty_CheckIfICanEditCountries ())
     {
      HTM_DIV_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgCtyMapAtt);
      HTM_TEXTAREA_Begin ("id=\"AttributionArea\" name=\"Attribution\" rows=\"3\""
			  " onchange=\"document.getElementById('%s').submit();return false;\"",
			  Gbl.Form.Id);
      if (MapAttribution)
	 HTM_Txt (MapAttribution);
      HTM_TEXTAREA_End ();
      Frm_EndForm ();
      HTM_DIV_End ();
     }
   else if (MapAttribution)
     {
      HTM_DIV_Begin ("class=\"ATTRIBUTION\"");
      HTM_Txt (MapAttribution);
      HTM_DIV_End ();
     }

   /***** Free memory used for map attribution *****/
   CtyCfg_FreeMapAttr (&MapAttribution);
  }

/*****************************************************************************/
/**************** Show country name in country configuration *****************/
/*****************************************************************************/

static void CtyCfg_Name (bool PutLink)
  {
   extern const char *Txt_Country;

   /***** Country name *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Country);

   /* Data */
   HTM_TD_Begin ("class=\"DAT_N LB\"");
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT_N\"",
	           Gbl.Hierarchy.Cty.WWW[Gbl.Prefs.Language]);
   HTM_Txt (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
   if (PutLink)
      HTM_A_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Show country shortcut in country configuration ***************/
/*****************************************************************************/

static void CtyCfg_Shortcut (bool PrintView)
  {
   HieCfg_Shortcut (PrintView,"cty",Gbl.Hierarchy.Cty.CtyCod);
  }

/*****************************************************************************/
/***************** Show country QR in country configuration ******************/
/*****************************************************************************/

static void CtyCfg_QR (void)
  {
   HieCfg_QR ("cty",Gbl.Hierarchy.Cty.CtyCod);
  }

/*****************************************************************************/
/*** Show number of users who claim to belong to country in country config. **/
/*****************************************************************************/

static void CtyCfg_NumUsrs (void)
  {
   extern const char *Txt_Users_of_the_country;

   /***** Number of users *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Users_of_the_country);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToCty (Gbl.Hierarchy.Cty.CtyCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Show number of institutions in country configuration ************/
/*****************************************************************************/

static void CtyCfg_NumInss (void)
  {
   extern const char *Txt_Institutions;
   extern const char *Txt_Institutions_of_COUNTRY_X;

   /***** Number of institutions ******/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Institutions);

   /* Data */
   HTM_TD_Begin ("class=\"LB\"");
   Frm_StartFormGoTo (ActSeeIns);
   Cty_PutParamCtyCod (Gbl.Hierarchy.Cty.CtyCod);
   HTM_BUTTON_SUBMIT_Begin (Str_BuildStringStr (Txt_Institutions_of_COUNTRY_X,
					        Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]),
			    "BT_LINK DAT",NULL);
   Str_FreeString ();
   HTM_Unsigned (Ins_GetNumInssInCty (Gbl.Hierarchy.Cty.CtyCod));
   HTM_BUTTON_End ();
   Frm_EndForm ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of centres in country configuration ***************/
/*****************************************************************************/

static void CtyCfg_NumCtrs (void)
  {
   extern const char *Txt_Centres;

   /***** Number of centres *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Centres);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Ctr_GetNumCtrsInCty (Gbl.Hierarchy.Cty.CtyCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of degrees in country configuration ***************/
/*****************************************************************************/

static void CtyCfg_NumDegs (void)
  {
   extern const char *Txt_Degrees;

   /***** Number of degrees *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Degrees);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Deg_GetNumDegsInCty (Gbl.Hierarchy.Cty.CtyCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of courses in country configuration ***************/
/*****************************************************************************/

static void CtyCfg_NumCrss (void)
  {
   extern const char *Txt_Courses;

   /***** Number of courses *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Courses);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Crs_GetNumCrssInCty (Gbl.Hierarchy.Cty.CtyCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Number of users in courses of this country *****************/
/*****************************************************************************/

static void CtyCfg_NumUsrsInCrssOfCty (Rol_Role_t Role)
  {
   extern const char *Txt_Users_in_courses;
   extern const char *Txt_ROLES_PLURAL_Abc[Rol_NUM_ROLES][Usr_NUM_SEXS];

   /***** Number of users in courses *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,
		    Role == Rol_UNK ? Txt_Users_in_courses :
		                      Txt_ROLES_PLURAL_Abc[Role][Usr_SEX_UNKNOWN]);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Usr_GetNumUsrsInCrssOfCty (Role,Gbl.Hierarchy.Cty.CtyCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Get map attribution from database **********************/
/*****************************************************************************/

static void CtyCfg_GetMapAttr (long CtyCod,char **MapAttribution)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   /***** Free possible former map attribution *****/
   CtyCfg_FreeMapAttr (MapAttribution);

   /***** Get photo attribution from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get photo attribution",
		       "SELECT MapAttribution FROM countries WHERE CtyCod=%ld",
	               CtyCod))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the attribution of the map of the country (row[0]) */
      if (row[0])
	 if (row[0][0])
	   {
	    Length = strlen (row[0]);
	    if ((*MapAttribution = (char *) malloc (Length + 1)) == NULL)
	       Lay_ShowErrorAndExit ("Error allocating memory for map attribution.");
	    Str_Copy (*MapAttribution,row[0],
	              Length);
	   }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Free memory used for map attribution ********************/
/*****************************************************************************/

static void CtyCfg_FreeMapAttr (char **MapAttribution)
  {
   if (*MapAttribution)
     {
      free (*MapAttribution);
      *MapAttribution = NULL;
     }
  }

/*****************************************************************************/
/*********** Change the attribution of the map of current country ************/
/*****************************************************************************/

void CtyCfg_ChangeCtyMapAttr (void)
  {
   char NewMapAttribution[Med_MAX_BYTES_ATTRIBUTION + 1];

   /***** Get parameters from form *****/
   /* Get the new map attribution for the country */
   Par_GetParToText ("Attribution",NewMapAttribution,Med_MAX_BYTES_ATTRIBUTION);

   /***** Update the table changing old attribution by new attribution *****/
   DB_QueryUPDATE ("can not update the map attribution of a country",
		   "UPDATE countries SET MapAttribution='%s'"
		   " WHERE CtyCod='%03ld'",
	           NewMapAttribution,Gbl.Hierarchy.Cty.CtyCod);

   /***** Show the country information again *****/
   CtyCfg_ShowConfiguration ();
  }
