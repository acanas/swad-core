// swad_system_config.c:  configuration of system

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
#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_center_database.h"
#include "swad_config.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy_config.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_role.h"
#include "swad_system_config.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void SysCfg_Configuration (bool PrintView);
static void SysCfg_PutIconToPrint (__attribute__((unused)) void *Args);
static void SysCfg_Map (void);
static void SysCfg_Platform (void);
static void SysCfg_Shortcut (bool PrintView);
static void SysCfg_QR (void);
static void SysCfg_NumCtys (void);
static void SysCfg_NumInss (void);
static void SysCfg_NumDegs (void);
static void SysCfg_NumCrss (void);

/*****************************************************************************/
/***************** Show information of the current country *******************/
/*****************************************************************************/

void SysCfg_ShowConfiguration (void)
  {
   SysCfg_Configuration (false);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/***************** Print information of the current country ******************/
/*****************************************************************************/

void SysCfg_PrintConfiguration (void)
  {
   SysCfg_Configuration (true);
  }

/*****************************************************************************/
/******************** Information of the current country *********************/
/*****************************************************************************/

static void SysCfg_Configuration (bool PrintView)
  {
   extern const char *Hlp_SYSTEM_Information;
   unsigned NumCtrs;
   unsigned NumCtrsWithMap;

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,Cfg_PLATFORM_SHORT_NAME,
                    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Cfg_PLATFORM_SHORT_NAME,
                    SysCfg_PutIconToPrint,NULL,
		    Hlp_SYSTEM_Information,Box_NOT_CLOSABLE);

   /**************************** Left part ***********************************/
   HTM_DIV_Begin ("class=\"HIE_CFG_LEFT HIE_CFG_WIDTH\"");

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

	 /***** Platform *****/
	 SysCfg_Platform ();

	 /***** Shortcut to the country *****/
	 SysCfg_Shortcut (PrintView);

	 /***** Get number of centers with map *****/
	 NumCtrsWithMap = Ctr_GetCachedNumCtrsWithMapInSys ();

	 if (PrintView)
	    /***** QR code with link to the country *****/
	    SysCfg_QR ();
	 else
	   {
	    /***** Get number of centers *****/
	    NumCtrs = Ctr_GetCachedNumCtrsInSys ();

	    /***** Number of countries,
		   number of institutions,
		   number of centers,
		   number of degrees,
		   number of courses *****/
	    SysCfg_NumCtys ();
	    SysCfg_NumInss ();
	    HieCfg_NumCtrs (NumCtrs,
			    false);	// Don't put form
	    HieCfg_NumCtrsWithMap (NumCtrs,NumCtrsWithMap);
	    SysCfg_NumDegs ();
	    SysCfg_NumCrss ();

	    /***** Number of users in courses of this country *****/
	    HieCfg_NumUsrsInCrss (HieLvl_SYS,-1L,Rol_TCH);
	    HieCfg_NumUsrsInCrss (HieLvl_SYS,-1L,Rol_NET);
	    HieCfg_NumUsrsInCrss (HieLvl_SYS,-1L,Rol_STD);
	    HieCfg_NumUsrsInCrss (HieLvl_SYS,-1L,Rol_UNK);
	   }

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End of left part *****/
   HTM_DIV_End ();

   /**************************** Right part **********************************/
   if (NumCtrsWithMap)
     {
      HTM_DIV_Begin ("class=\"HIE_CFG_RIGHT HIE_CFG_WIDTH\"");

	 /***** Country map *****/
	 SysCfg_Map ();

      HTM_DIV_End ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* Put icon to print the configuration of a country **************/
/*****************************************************************************/

static void SysCfg_PutIconToPrint (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToPrint (ActPrnSysInf,
				 NULL,NULL);
  }

/*****************************************************************************/
/****************************** Draw country map *****************************/
/*****************************************************************************/

#define SysCfg_MAP_CONTAINER_ID "sys_mapid"

static void SysCfg_Map (void)
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
   HTM_DIV_Begin ("id=\"%s\"",SysCfg_MAP_CONTAINER_ID);
   HTM_DIV_End ();

   /***** Script to draw the map *****/
   HTM_SCRIPT_Begin (NULL,NULL);

      /* Let's create a map with pretty Mapbox Streets tiles */
      Ctr_DB_GetCoordAndZoom (&CtyAvgCoord,&Zoom);
      Map_CreateMap (SysCfg_MAP_CONTAINER_ID,&CtyAvgCoord,Zoom);

      /* Add Mapbox Streets tile layer to our map */
      Map_AddTileLayer ();

      /* Get centers with coordinates */
      NumCtrs = Ctr_DB_GetCtrsWithCoords (&mysql_res);

      /* Add a marker and a popup for each center */
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
	{
	 /* Get next center */
	 Ctr.Cod = DB_GetNextCode (mysql_res);

	 /* Get data of center */
	 Ctr_GetCenterDataByCod (&Ctr);

	 /* Get data of institution */
	 Ins.Cod = Ctr.PrtCod;
	 Ins_GetInstitDataByCod (&Ins);

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
/****************** Show platform in country configuration *******************/
/*****************************************************************************/

static void SysCfg_Platform (void)
  {
   extern const char *Txt_Platform;

   /***** Institution *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Platform);

   /* Data */
   HTM_TD_Begin ("class=\"LB DAT_STRONG_%s\"",The_GetSuffix ());
   HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Show platform shortcut in system configuration ***************/
/*****************************************************************************/

static void SysCfg_Shortcut (bool PrintView)
  {
   HieCfg_Shortcut (PrintView,ParCod_None,-1L);
  }

/*****************************************************************************/
/***************** Show country QR in country configuration ******************/
/*****************************************************************************/

static void SysCfg_QR (void)
  {
   HieCfg_QR (ParCod_None,-1L);
  }

/*****************************************************************************/
/************ Show number of countries in system configuration ***************/
/*****************************************************************************/

static void SysCfg_NumCtys (void)
  {
   extern const char *Txt_Countries;

   /***** Number of countries ******/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Countries);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 Frm_BeginFormGoTo (ActSeeCty);
	    HTM_BUTTON_Submit_Begin (Txt_Countries,"class=\"LB BT_LINK\"");
	       HTM_Unsigned (Cty_GetCachedNumCtysInSys ());
	    HTM_BUTTON_End ();
	 Frm_EndForm ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Show number of institutions in system configuration *************/
/*****************************************************************************/

static void SysCfg_NumInss (void)
  {
   extern const char *Txt_Institutions;

   /***** Number of institutions ******/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Institutions);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Ins_GetCachedNumInssInSys ());
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of degrees in system configuration ****************/
/*****************************************************************************/

static void SysCfg_NumDegs (void)
  {
   extern const char *Txt_Degrees;

   /***** Number of degrees *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Degrees);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Deg_GetCachedNumDegsInSys ());
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of courses in system configuration ****************/
/*****************************************************************************/

static void SysCfg_NumCrss (void)
  {
   extern const char *Txt_Courses;

   /***** Number of courses *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Courses);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Crs_GetCachedNumCrssInSys ());
      HTM_TD_End ();

   HTM_TR_End ();
  }
