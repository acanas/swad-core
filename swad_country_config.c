// swad_country_config.c:  configuration of current country

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_box.h"
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
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_parameter.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void CtyCfg_Configuration (Vie_ViewType_t ViewType);
static void CtyCfg_PutIconToPrint (__attribute__((unused)) void *Args);
static void CtyCfg_Title (Hie_PutLink_t PutLink);
static void CtyCfg_Map (void);
static void CtyCfg_MapImage (Vie_ViewType_t ViewType,Hie_PutLink_t PutLink);
static void CtyCfg_Platform (Vie_ViewType_t ViewType);
static void CtyCfg_Name (Hie_PutLink_t PutLink);
static void CtyCfg_NumUsrs (void);
static void CtyCfg_NumInss (void);
static void CtyCfg_NumDegs (void);
static void CtyCfg_NumCrss (void);

static void CtyCfg_GetMapAttr (long HieCod,char **MapAttribution);
static void CtyCfg_FreeMapAttr (char **MapAttribution);

/*****************************************************************************/
/***************** Show information of the current country *******************/
/*****************************************************************************/

void CtyCfg_ShowConfiguration (void)
  {
   CtyCfg_Configuration (Vie_VIEW);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/***************** Print information of the current country ******************/
/*****************************************************************************/

void CtyCfg_PrintConfiguration (void)
  {
   CtyCfg_Configuration (Vie_PRINT);
  }

/*****************************************************************************/
/******************** Information of the current country *********************/
/*****************************************************************************/

static void CtyCfg_Configuration (Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_COUNTRY_Information;
   extern const char *Txt_NULL;
   static struct
     {
      void (*FunctionToDrawContextualIcons) (void *Args);
      const char **HelpLink;
     } BoxArgs[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW  ] = {CtyCfg_PutIconToPrint	,&Hlp_COUNTRY_Information	},
      [Vie_EDIT  ] = {NULL			,&Txt_NULL			},
      [Vie_CONFIG] = {NULL			,&Txt_NULL			},
      [Vie_PRINT ] = {NULL			,&Txt_NULL			},
     };
   Hie_PutLink_t PutLink;
   Exi_Exist_t MapImageExists;
   unsigned NumCtrs;
   unsigned NumCtrsWithMap;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_CTY].HieCod <= 0)		// No country selected
      return;

   /***** Initializations *****/
   PutLink = ViewType == Vie_VIEW &&
	     Gbl.Hierarchy.Node[Hie_CTY].WWW[0] ? Hie_PUT_LINK :
						  Hie_DONT_PUT_LINK;

   /***** Begin box *****/
   Box_BoxBegin (NULL,BoxArgs[ViewType].FunctionToDrawContextualIcons,NULL,
		 *BoxArgs[ViewType].HelpLink,Box_NOT_CLOSABLE);

      /***** Title *****/
      CtyCfg_Title (PutLink);

      /**************************** Left part ***********************************/
      HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

	 /* Begin table */
	 HTM_TABLE_BeginCenterPadding (2);

	    /* Platform */
	    CtyCfg_Platform (ViewType);

	    /* Country name (an link to WWW if exists) */
	    CtyCfg_Name (PutLink);

	    /* Shortcut to the country */
	    HieCfg_Shortcut (ViewType,ParCod_Cty,Gbl.Hierarchy.Node[Hie_CTY].HieCod);

	    NumCtrsWithMap = Ctr_GetCachedNumCtrsWithMapInCty (Gbl.Hierarchy.Node[Hie_CTY].HieCod);
	    switch (ViewType)
	      {
	       case Vie_VIEW:
		  NumCtrs = Hie_GetCachedNumNodesInHieLvl (Hie_CTR,	// Number of centers...
							   Hie_CTY,	// ...in country
							   Gbl.Hierarchy.Node[Hie_CTY].HieCod);

		  /* Number of users who claim to belong to this country,
		     number of institutions,
		     number of centers,
		     number of degrees,
		     number of courses */
		  CtyCfg_NumUsrs ();
		  CtyCfg_NumInss ();
		  HieCfg_NumCtrs (NumCtrs,Frm_DONT_PUT_FORM);
		  HieCfg_NumCtrsWithMap (NumCtrs,NumCtrsWithMap);
		  CtyCfg_NumDegs ();
		  CtyCfg_NumCrss ();

		  /* Number of users in courses of this country */
		  HieCfg_NumUsrsInCrss (Hie_CTY,Gbl.Hierarchy.Node[Hie_CTY].HieCod,Rol_TCH);
		  HieCfg_NumUsrsInCrss (Hie_CTY,Gbl.Hierarchy.Node[Hie_CTY].HieCod,Rol_NET);
		  HieCfg_NumUsrsInCrss (Hie_CTY,Gbl.Hierarchy.Node[Hie_CTY].HieCod,Rol_STD);
		  HieCfg_NumUsrsInCrss (Hie_CTY,Gbl.Hierarchy.Node[Hie_CTY].HieCod,Rol_UNK);
		  break;
	       case Vie_PRINT:
		  /* QR code with link to the country */
		  HieCfg_QR (ParCod_Cty,Gbl.Hierarchy.Node[Hie_CTY].HieCod);
		  break;
	       default:
		  Err_WrongTypeExit ();
		  break;
	      }

	 /* End table */
	 HTM_TABLE_End ();

      /* End of left part */
      HTM_DIV_End ();

      /**************************** Right part **********************************/
      /* Check country map */
      MapImageExists = Cty_CheckIfCountryPhotoExists (&Gbl.Hierarchy.Node[Hie_CTY]);

      if (NumCtrsWithMap || MapImageExists == Exi_EXISTS)
	{
	 /* Begin container */
	 HTM_DIV_Begin ("class=\"HIE_CFG_RIGHT\"");

	    /* Country map */
	    if (NumCtrsWithMap)
	       CtyCfg_Map ();

	    /* Country map image */
	    if (MapImageExists == Exi_EXISTS)
	       CtyCfg_MapImage (ViewType,PutLink);

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
   Ico_PutContextualIconToPrint (ActPrnCtyInf,NULL,NULL);
  }

/*****************************************************************************/
/******************** Show title in country configuration ********************/
/*****************************************************************************/

static void CtyCfg_Title (Hie_PutLink_t PutLink)
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"FRAME_TITLE FRAME_TITLE_BIG FRAME_TITLE_%s\"",
                  The_GetSuffix ());

      /* Begin link */
      if (PutLink == Hie_PUT_LINK)
	 HTM_A_Begin ("href=\"%s\" target=\"_blank\" title=\"%s\""
		      " class=\"FRAME_TITLE_BIG FRAME_TITLE_%s\"",
		      Gbl.Hierarchy.Node[Hie_CTY].WWW,
		      Gbl.Hierarchy.Node[Hie_CTY].FullName,The_GetSuffix ());

      /* Country name */
      HTM_Txt (Gbl.Hierarchy.Node[Hie_CTY].FullName);

      /* End link */
      if (PutLink == Hie_PUT_LINK)
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
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   MYSQL_RES *mysql_res;
   struct Map_Coordinates CtyAvgCoord;
   unsigned Zoom;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Hie_Node Ins;
   struct Hie_Node Ctr;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   struct Map_Coordinates Coord;

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
      Cty_DB_GetAvgCoordAndZoom (&CtyAvgCoord,&Zoom);
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
	 Ctr.HieCod = DB_GetNextCode (mysql_res);

	 /* Get data of center */
	 SuccessOrError = Hie_GetDataByCod[Hie_CTR] (&Ctr);

	 /* Get coordinates of center */
	 Ctr_GetCoordByCod (Ctr.HieCod,&Coord);

	 /* Get data of institution */
	 Ins.HieCod = Ctr.PrtCod;
	 SuccessOrError = Hie_GetDataByCod[Hie_INS] (&Ins);

	 /* Add marker */
	 Map_AddMarker (&Coord);

	 /* Add popup */
	 Map_AddPopup (Ctr.ShrtName,Ins.ShrtName,CloOpe_CLOSED);
	}

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/************* Show country map image in country configuration ***************/
/*****************************************************************************/

static void CtyCfg_MapImage (Vie_ViewType_t ViewType,Hie_PutLink_t PutLink)
  {
   static const char *Class[Vie_NUM_VIEW_TYPES] =
     {
      [Vie_VIEW  ] = "COUNTRY_MAP_SHOW",
      [Vie_EDIT  ] = NULL,
      [Vie_CONFIG] = NULL,
      [Vie_PRINT ] = "COUNTRY_MAP_PRINT",
     };
   char *MapAttribution = NULL;

   /***** Get map attribution *****/
   CtyCfg_GetMapAttr (Gbl.Hierarchy.Node[Hie_CTY].HieCod,&MapAttribution);

   /***** Map image *****/
   HTM_DIV_Begin ("class=\"CM\"");
      if (PutLink == Hie_PUT_LINK)
	 HTM_A_Begin ("href=\"%s\" target=\"_blank\"",
	              Gbl.Hierarchy.Node[Hie_CTY].WWW);
      Cty_DrawCountryMap (&Gbl.Hierarchy.Node[Hie_CTY],Class[ViewType]);
      if (PutLink == Hie_PUT_LINK)
	 HTM_A_End ();
   HTM_DIV_End ();

   /***** Map attribution *****/
   if (ViewType == Vie_VIEW && Cty_CheckIfICanEditCountries () == Usr_CAN)
     {
      HTM_DIV_Begin ("class=\"CM\"");
	 Frm_BeginForm (ActChgCtyMapAtt);
	    HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				"id=\"AttributionArea\" name=\"Attribution\""
				" rows=\"3\""
				" onchange=\"this.form.submit();return false;\"");
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

static void CtyCfg_Platform (Vie_ViewType_t ViewType)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];

   /***** Institution *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_SINGUL_Abc[Hie_SYS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
	 if (ViewType == Vie_VIEW)
	   {
	    Frm_BeginFormGoTo (ActSeeSysInf);
	       HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Cfg_PLATFORM_SHORT_NAME),NULL,
					"class=\"LT BT_LINK\"");
	       Str_FreeGoToTitle ();
	   }
	 Ico_PutIcon ("swad64x64.png",Ico_UNCHANGED,Cfg_PLATFORM_FULL_NAME,"ICO20x20");
	 HTM_NBSP ();
	 HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
	 if (ViewType == Vie_VIEW)
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

static void CtyCfg_Name (Hie_PutLink_t PutLink)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];

   /***** Country name *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_SINGUL_Abc[Hie_CTY]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	 if (PutLink == Hie_PUT_LINK)
	    HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT_STRONG_%s\"",
			 Gbl.Hierarchy.Node[Hie_CTY].WWW,The_GetSuffix ());
	 HTM_Txt (Gbl.Hierarchy.Node[Hie_CTY].FullName);
	 if (PutLink == Hie_PUT_LINK)
	    HTM_A_End ();
      HTM_TD_End ();

   HTM_TR_End ();
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
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Users_of_the_country);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_CTY,
							       &Gbl.Hierarchy.Node[Hie_CTY]));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Show number of institutions in country configuration ************/
/*****************************************************************************/

static void CtyCfg_NumInss (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Institutions_of_COUNTRY_X;
   char *Title;

   /***** Number of institutions ******/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_INS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
	 Frm_BeginFormGoTo (ActSeeIns);
	    ParCod_PutPar (ParCod_Cty,Gbl.Hierarchy.Node[Hie_CTY].HieCod);
	    if (asprintf (&Title,Txt_Institutions_of_COUNTRY_X,
	                  Gbl.Hierarchy.Node[Hie_CTY].FullName) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_BUTTON_Submit_Begin (Title,NULL,"class=\"LT BT_LINK\"");
	    free (Title);
	       HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_INS,	// Number of institutions...
							    Hie_CTY,	// ...in country
							    Gbl.Hierarchy.Node[Hie_CTY].HieCod));
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
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of degrees *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_DEG]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
						      Hie_CTY,	// ...in country
						      Gbl.Hierarchy.Node[Hie_CTY].HieCod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of courses in country configuration ***************/
/*****************************************************************************/

static void CtyCfg_NumCrss (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of courses *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_CRS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
						      Hie_CTY,	// ...in country
						      Gbl.Hierarchy.Node[Hie_CTY].HieCod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Get map attribution from database **********************/
/*****************************************************************************/

static void CtyCfg_GetMapAttr (long HieCod,char **MapAttribution)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   /***** Free possible former map attribution *****/
   CtyCfg_FreeMapAttr (MapAttribution);

   /***** Get map attribution from database *****/
   if (Cty_DB_GetMapAttr (&mysql_res,HieCod))
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
   Par_GetParText ("Attribution",NewMapAttribution,Med_MAX_BYTES_ATTRIBUTION);

   /***** Change old attribution by new attribution in database *****/
   Cty_DB_UpdateCtyMapAttr (NewMapAttribution);

   /***** Show the country information again *****/
   CtyCfg_ShowConfiguration ();
  }
