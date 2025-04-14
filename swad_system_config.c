// swad_system_config.c:  configuration of system

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
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
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_role.h"
#include "swad_system_config.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void SysCfg_Configuration (Vie_ViewType_t ViewType);
static void SysCfg_PutIconToPrint (__attribute__((unused)) void *Args);
static void SysCfg_Map (void);
static void SysCfg_Platform (void);
static void SysCfg_NumCtys (void);
static void SysCfg_NumInss (void);
static void SysCfg_NumDegs (void);
static void SysCfg_NumCrss (void);

/*****************************************************************************/
/***************** Show information of the current country *******************/
/*****************************************************************************/

void SysCfg_ShowConfiguration (void)
  {
   SysCfg_Configuration (Vie_VIEW);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/***************** Print information of the current country ******************/
/*****************************************************************************/

void SysCfg_PrintConfiguration (void)
  {
   SysCfg_Configuration (Vie_PRINT);
  }

/*****************************************************************************/
/******************** Information of the current country *********************/
/*****************************************************************************/

static void SysCfg_Configuration (Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_SYSTEM_Information;
   unsigned NumCtrs;
   unsigned NumCtrsWithMap;

   /***** Begin box *****/
   Box_BoxBegin (Cfg_PLATFORM_SHORT_NAME,
		 ViewType == Vie_VIEW ? SysCfg_PutIconToPrint :
					NULL,NULL,
		 ViewType == Vie_VIEW ? Hlp_SYSTEM_Information :
					NULL,Box_NOT_CLOSABLE);

      /**************************** Left part ***********************************/
      HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

	 /***** Begin table *****/
	 HTM_TABLE_BeginCenterPadding (2);

	    /***** Platform *****/
	    SysCfg_Platform ();

	    /***** Shortcut to the system *****/
	    HieCfg_Shortcut (ViewType,ParCod_None,-1L);

	    /***** Get number of centers with map *****/
	    NumCtrsWithMap = Ctr_GetCachedNumCtrsWithMapInSys ();

	    switch (ViewType)
	      {
	       case Vie_VIEW:
		  /***** Get number of centers *****/
		  NumCtrs = Hie_GetCachedNumNodesInHieLvl (Hie_CTR,	// Number of centers...
							   Hie_SYS,	// ...in system
							   Gbl.Hierarchy.Node[Hie_SYS].HieCod);

		  /***** Number of countries,
			 number of institutions,
			 number of centers,
			 number of degrees,
			 number of courses *****/
		  SysCfg_NumCtys ();
		  SysCfg_NumInss ();
		  HieCfg_NumCtrs (NumCtrs,Frm_DONT_PUT_FORM);
		  HieCfg_NumCtrsWithMap (NumCtrs,NumCtrsWithMap);
		  SysCfg_NumDegs ();
		  SysCfg_NumCrss ();

		  /***** Number of users in courses of this country *****/
		  HieCfg_NumUsrsInCrss (Hie_SYS,-1L,Rol_TCH);
		  HieCfg_NumUsrsInCrss (Hie_SYS,-1L,Rol_NET);
		  HieCfg_NumUsrsInCrss (Hie_SYS,-1L,Rol_STD);
		  HieCfg_NumUsrsInCrss (Hie_SYS,-1L,Rol_UNK);
		  break;
	       case Vie_PRINT:
		  /***** QR code with link to the country *****/
		  HieCfg_QR (ParCod_None,-1L);
		  break;
	       default:
		  Err_WrongTypeExit ();
		  break;
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End of left part *****/
      HTM_DIV_End ();

      /**************************** Right part **********************************/
      if (NumCtrsWithMap)
	{
	 HTM_DIV_Begin ("class=\"HIE_CFG_RIGHT\"");

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
   Ico_PutContextualIconToPrint (ActPrnSysInf,NULL,NULL);
  }

/*****************************************************************************/
/****************************** Draw country map *****************************/
/*****************************************************************************/

#define SysCfg_MAP_CONTAINER_ID "sys_mapid"

static void SysCfg_Map (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   MYSQL_RES *mysql_res;
   unsigned Zoom;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Hie_Node Ins;
   struct Hie_Node Ctr;
   struct Map_Coordinates Coord;
   struct Map_Coordinates CtyAvgCoord;

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
      Ctr_DB_GetAvgCoordAndZoom (&CtyAvgCoord,&Zoom);
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
	 Ctr.HieCod = DB_GetNextCode (mysql_res);

	 /* Get data of center */
	 Hie_GetDataByCod[Hie_CTR] (&Ctr);

	 /* Get coordinates of center */
	 Ctr_GetCoordByCod (Ctr.HieCod,&Coord);

	 /* Get data of institution */
	 Ins.HieCod = Ctr.PrtCod;
	 Hie_GetDataByCod[Hie_INS] (&Ins);

	 /* Add marker */
	 Map_AddMarker (&Coord);

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
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];

   /***** Institution *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_SINGUL_Abc[Hie_SYS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_STRONG_%s\"",The_GetSuffix ());
	 HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show number of countries in system configuration ***************/
/*****************************************************************************/

static void SysCfg_NumCtys (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of countries ******/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_CTY]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 Frm_BeginFormGoTo (ActSeeCty);
	    HTM_BUTTON_Submit_Begin (Txt_HIERARCHY_PLURAL_Abc[Hie_CTY],NULL,
				     "class=\"LB BT_LINK\"");
	       HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CTY,	// Number of countries...
						            Hie_SYS,	// ...in system
						            Gbl.Hierarchy.Node[Hie_SYS].HieCod));
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
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of institutions ******/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_INS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_INS,	// Number of institutions...
						      Hie_SYS,	// ...in system
						      Gbl.Hierarchy.Node[Hie_SYS].HieCod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of degrees in system configuration ****************/
/*****************************************************************************/

static void SysCfg_NumDegs (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of degrees *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_DEG]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
						      Hie_SYS,	// ...in system
						      Gbl.Hierarchy.Node[Hie_SYS].HieCod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Show number of courses in system configuration ****************/
/*****************************************************************************/

static void SysCfg_NumCrss (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of courses *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_CRS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
						      Hie_SYS,	// ...in system
						      Gbl.Hierarchy.Node[Hie_SYS].HieCod));
      HTM_TD_End ();

   HTM_TR_End ();
  }
