// swad_country_config.c:  configuration of current country

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
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
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
static void CtyCfg_PutIconToPrint (__attribute__((unused)) void *Args);
static void CtyCfg_Title (bool PutLink);
static void CtyCfg_Map (void);
static void CtyCfg_MapImage (bool PrintView,bool PutLink);
static void CtyCfg_Platform (bool PrintView);
static void CtyCfg_Name (bool PutLink);
static void CtyCfg_Shortcut (bool PrintView);
static void CtyCfg_QR (void);
static void CtyCfg_NumUsrs (void);
static void CtyCfg_NumInss (void);
static void CtyCfg_NumDegs (void);
static void CtyCfg_NumCrss (void);

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
   bool MapImageExists;
   unsigned NumCtrs;
   unsigned NumCtrsWithMap;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Cty.CtyCod <= 0)		// No country selected
      return;

   /***** Initializations *****/
   PutLink = !PrintView && Gbl.Hierarchy.Cty.WWW[Gbl.Prefs.Language][0];

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,
                    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,
                    CtyCfg_PutIconToPrint,NULL,
		    Hlp_COUNTRY_Information,Box_NOT_CLOSABLE);

   /***** Title *****/
   CtyCfg_Title (PutLink);

   /**************************** Left part ***********************************/
   HTM_DIV_Begin ("class=\"HIE_CFG_LEFT HIE_CFG_WIDTH\"");

      /* Begin table */
      HTM_TABLE_BeginWidePadding (2);

	 /* Platform */
	 CtyCfg_Platform (PrintView);

	 /* Country name (an link to WWW if exists) */
	 CtyCfg_Name (PutLink);

	 /* Shortcut to the country */
	 CtyCfg_Shortcut (PrintView);

	 NumCtrsWithMap = Ctr_GetCachedNumCtrsWithMapInCty (Gbl.Hierarchy.Cty.CtyCod);
	 if (PrintView)
	    /* QR code with link to the country */
	    CtyCfg_QR ();
	 else
	   {
	    NumCtrs = Ctr_GetCachedNumCtrsInCty (Gbl.Hierarchy.Cty.CtyCod);

	    /* Number of users who claim to belong to this country,
	       number of institutions,
	       number of centers,
	       number of degrees,
	       number of courses */
	    CtyCfg_NumUsrs ();
	    CtyCfg_NumInss ();
	    HieCfg_NumCtrs (NumCtrs,
			    false);	// Don't put form
	    HieCfg_NumCtrsWithMap (NumCtrs,NumCtrsWithMap);
	    CtyCfg_NumDegs ();
	    CtyCfg_NumCrss ();

	    /* Number of users in courses of this country */
	    HieCfg_NumUsrsInCrss (Hie_Lvl_CTY,Gbl.Hierarchy.Cty.CtyCod,Rol_TCH);
	    HieCfg_NumUsrsInCrss (Hie_Lvl_CTY,Gbl.Hierarchy.Cty.CtyCod,Rol_NET);
	    HieCfg_NumUsrsInCrss (Hie_Lvl_CTY,Gbl.Hierarchy.Cty.CtyCod,Rol_STD);
	    HieCfg_NumUsrsInCrss (Hie_Lvl_CTY,Gbl.Hierarchy.Cty.CtyCod,Rol_UNK);
	   }

      /* End table */
      HTM_TABLE_End ();

   /* End of left part */
   HTM_DIV_End ();

   /**************************** Right part **********************************/
   /* Check country map */
   MapImageExists = Cty_CheckIfCountryPhotoExists (&Gbl.Hierarchy.Cty);

   if (NumCtrsWithMap || MapImageExists)
     {
      /* Begin container */
      HTM_DIV_Begin ("class=\"HIE_CFG_RIGHT HIE_CFG_WIDTH\"");

	 /* Country map */
	 if (NumCtrsWithMap)
	    CtyCfg_Map ();

	 /* Country map image */
	 if (MapImageExists)
	    CtyCfg_MapImage (PrintView,PutLink);

      /* End container */
      HTM_DIV_End ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Put icon to print the configuration of a country **************/
/*****************************************************************************/

static void CtyCfg_PutIconToPrint (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToPrint (ActPrnCtyInf,
				 NULL,NULL);
  }

/*****************************************************************************/
/******************** Show title in country configuration ********************/
/*****************************************************************************/

static void CtyCfg_Title (bool PutLink)
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG\"");

      /* Begin link */
      if (PutLink)
	 HTM_A_Begin ("href=\"%s\" target=\"_blank\""
		      " class=\"FRAME_TITLE_BIG\" title=\"%s\"",
		      Gbl.Hierarchy.Cty.WWW[Gbl.Prefs.Language],
		      Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);

      /* Country name */
      HTM_Txt (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);

      /* End link */
      if (PutLink)
	 HTM_A_End ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/****************************** Draw country map *****************************/
/*****************************************************************************/

#define CtyCfg_MAP_CONTAINER_ID "cty_mapid"

static void CtyCfg_Map (void)
  {
   MYSQL_RES *mysql_res;
   struct Map_Coordinates CtyAvgCoord;
   unsigned Zoom;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Ins_Instit Ins;
   struct Ctr_Center Ctr;

   /***** Leaflet CSS *****/
   Map_LeafletCSS ();

   /***** Leaflet script *****/
   Map_LeafletScript ();

   /***** Container for the map *****/
   HTM_DIV_Begin ("id=\"%s\"",CtyCfg_MAP_CONTAINER_ID);
   HTM_DIV_End ();

   /***** Script to draw the map *****/
   HTM_SCRIPT_Begin (NULL,NULL);

      /* Let's create a map with pretty Mapbox Streets tiles */
      Cty_DB_GetCoordAndZoom (&CtyAvgCoord,&Zoom);
      Map_CreateMap (CtyCfg_MAP_CONTAINER_ID,&CtyAvgCoord,Zoom);

      /* Add Mapbox Streets tile layer to our map */
      Map_AddTileLayer ();

      /* Get centers which have coordinates in the current country */
      NumCtrs = Cty_DB_GetCtrsWithCoordsInCurrentCty (&mysql_res);

      /* Add a marker and a popup for each center */
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
	{
	 /* Get next center */
	 Ctr.CtrCod = DB_GetNextCode (mysql_res);

	 /* Get data of center */
	 Ctr_GetDataOfCenterByCod (&Ctr);

	 /* Get data of institution */
	 Ins.InsCod = Ctr.InsCod;
	 Ins_GetDataOfInstitutionByCod (&Ins);

	 /* Add marker */
	 Map_AddMarker (&Ctr.Coord);

	 /* Add popup */
	 Map_AddPopup (Ctr.ShrtName,Ins.ShrtName,
		       false);	// Closed
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/************* Show country map image in country configuration ***************/
/*****************************************************************************/

static void CtyCfg_MapImage (bool PrintView,bool PutLink)
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
	 Frm_BeginForm (ActChgCtyMapAtt);
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
/****************** Show platform in country configuration *******************/
/*****************************************************************************/

static void CtyCfg_Platform (bool PrintView)
  {
   extern const char *Txt_Platform;

   /***** Institution *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Platform);

      /* Data */
      HTM_TD_Begin ("class=\"DAT LB\"");
	 if (!PrintView)
	   {
	    Frm_BeginFormGoTo (ActSeeSysInf);
	    HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Cfg_PLATFORM_SHORT_NAME),
				     "BT_LINK LT DAT",NULL);
	    Hie_FreeGoToMsg ();
	   }
	 Ico_PutIcon ("swad64x64.png",Cfg_PLATFORM_FULL_NAME,"ICO20x20");
	 HTM_NBSP ();
	 HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
	 if (!PrintView)
	   {
	    HTM_BUTTON_End ();
	    Frm_EndForm ();
	   }
      HTM_TD_End ();

   HTM_TR_End ();
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
	 HTM_Unsigned (Usr_GetCachedNumUsrsWhoClaimToBelongToCty (&Gbl.Hierarchy.Cty));
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
	 Frm_BeginFormGoTo (ActSeeIns);
	 Cty_PutParamCtyCod (Gbl.Hierarchy.Cty.CtyCod);
	    HTM_BUTTON_SUBMIT_Begin (Str_BuildStringStr (Txt_Institutions_of_COUNTRY_X,
							 Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]),
				     "BT_LINK DAT",NULL);
	    Str_FreeString ();
	       HTM_Unsigned (Ins_GetCachedNumInssInCty (Gbl.Hierarchy.Cty.CtyCod));
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
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
	 HTM_Unsigned (Deg_GetCachedNumDegsInCty (Gbl.Hierarchy.Cty.CtyCod));
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
	 HTM_Unsigned (Crs_GetCachedNumCrssInCty (Gbl.Hierarchy.Cty.CtyCod));
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

   /***** Get map attribution from database *****/
   if (Cty_DB_GetMapAttr (&mysql_res,CtyCod))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the attribution of the map of the country (row[0]) */
      if (row[0])
	 if (row[0][0])
	   {
	    Length = strlen (row[0]);
	    if ((*MapAttribution = malloc (Length + 1)) == NULL)
               Err_NotEnoughMemoryExit ();
	    Str_Copy (*MapAttribution,row[0],Length);
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

   /***** Change old attribution by new attribution in database *****/
   Cty_DB_UpdateCtyMapAttr (NewMapAttribution);

   /***** Show the country information again *****/
   CtyCfg_ShowConfiguration ();
  }
