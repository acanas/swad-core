// swad_system_config.c:  configuration of system

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

#include "swad_box.h"
#include "swad_config.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_config.h"
#include "swad_HTML.h"
#include "swad_role.h"
#include "swad_system_config.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

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

static void SysCfg_Configuration (bool PrintView);
static void SysCfg_PutIconToPrint (void);
static bool SysCfg_GetIfMapIsAvailable (void);
static void SysCfg_GetCoordAndZoom (struct Coordinates *Coord,unsigned *Zoom);
static void SysCfg_Map (void);
static void SysCfg_Platform (void);
static void SysCfg_Shortcut (bool PrintView);
static void SysCfg_QR (void);
static void SysCfg_NumCtys (void);
static void SysCfg_NumInss (void);
static void SysCfg_NumCtrs (void);
static void SysCfg_NumDegs (void);
static void SysCfg_NumCrss (void);
static void SysCfg_NumUsrsInCrss (Rol_Role_t Role);

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
   bool MapIsAvailable;

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,Cfg_PLATFORM_SHORT_NAME,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,Cfg_PLATFORM_SHORT_NAME,SysCfg_PutIconToPrint,
		    Hlp_SYSTEM_Information,Box_NOT_CLOSABLE);

   /**************************** Left part ***********************************/
   HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Platform *****/
   SysCfg_Platform ();

   /***** Shortcut to the country *****/
   SysCfg_Shortcut (PrintView);

   if (PrintView)
      /***** QR code with link to the country *****/
      SysCfg_QR ();
   else
     {
      /***** Number of countries,
             number of institutions,
             number of centres,
             number of degrees,
             number of courses *****/
      SysCfg_NumCtys ();
      SysCfg_NumInss ();
      SysCfg_NumCtrs ();
      SysCfg_NumDegs ();
      SysCfg_NumCrss ();

      /***** Number of users in courses of this country *****/
      SysCfg_NumUsrsInCrss (Rol_TCH);
      SysCfg_NumUsrsInCrss (Rol_NET);
      SysCfg_NumUsrsInCrss (Rol_STD);
      SysCfg_NumUsrsInCrss (Rol_UNK);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End of left part *****/
   HTM_DIV_End ();

   /**************************** Right part **********************************/
   /***** Check map *****/
   MapIsAvailable = SysCfg_GetIfMapIsAvailable ();

   if (MapIsAvailable)
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

static void SysCfg_PutIconToPrint (void)
  {
   Ico_PutContextualIconToPrint (ActPrnSysInf,NULL);
  }

/*****************************************************************************/
/******************** Check if country map should be shown *******************/
/*****************************************************************************/

static bool SysCfg_GetIfMapIsAvailable (void)
  {
   /***** Get number of centres with both coordinates set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get centres with coordinates",
			     "SELECT COUNT(*) FROM centres"
			     " WHERE centres.Latitude<>0"
			     " AND centres.Longitude<>0");
  }

/*****************************************************************************/
/********* Get average coordinates of centres in current institution *********/
/*****************************************************************************/

static void SysCfg_GetCoordAndZoom (struct Coordinates *Coord,unsigned *Zoom)
  {
   char *Query;

   /***** Get average coordinates of centres with both coordinates set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   if (asprintf (&Query,
		 "SELECT AVG(Latitude),"				// row[0]
			"AVG(Longitude),"				// row[1]
			"GREATEST(MAX(Latitude)-MIN(Latitude),"
				 "MAX(Longitude)-MIN(Longitude))"	// row[2]
		 " FROM centres"
		 " WHERE Latitude<>0"
		 " AND Longitude<>0") < 0)
      Lay_NotEnoughMemoryExit ();
   Map_GetCoordAndZoom (Coord,Zoom,Query);
   free (Query);
  }

/*****************************************************************************/
/****************************** Draw country map *****************************/
/*****************************************************************************/

#define SysCfg_MAP_CONTAINER_ID "cty_mapid"

static void SysCfg_Map (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Coordinates CtyAvgCoord;
   unsigned Zoom;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Centre Ctr;
   struct Instit Ins;

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
   SysCfg_GetCoordAndZoom (&CtyAvgCoord,&Zoom);
   Map_CreateMap (SysCfg_MAP_CONTAINER_ID,&CtyAvgCoord,Zoom);

   /* Add Mapbox Streets tile layer to our map */
   Map_AddTileLayer ();

   /* Get centres with coordinates */
   NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres"
						   " with coordinates",
					"SELECT CtrCod"	// row[0]
					" FROM centres"
					" WHERE centres.Latitude<>0"
					" AND centres.Longitude<>0");

   /* Add a marker and a popup for each centre */
   for (NumCtr = 0;
	NumCtr < NumCtrs;
	NumCtr++)
     {
      /* Get next centre */
      row = mysql_fetch_row (mysql_res);

      /* Get centre code (row[0]) */
      Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get data of centre */
      Ctr_GetDataOfCentreByCod (&Ctr);

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
   HTM_TD_Begin ("class=\"DAT_N LB\"");
   HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Show platform shortcut in system configuration ***************/
/*****************************************************************************/

static void SysCfg_Shortcut (bool PrintView)
  {
   HieCfg_Shortcut (PrintView,NULL,-1L);
  }

/*****************************************************************************/
/***************** Show country QR in country configuration ******************/
/*****************************************************************************/

static void SysCfg_QR (void)
  {
   HieCfg_QR (NULL,-1L);
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
   HTM_TD_Begin ("class=\"LB\"");
   Frm_StartFormGoTo (ActSeeCty);
   HTM_BUTTON_SUBMIT_Begin (Txt_Countries,"BT_LINK DAT",NULL);
   HTM_Unsigned (Cty_GetNumCtysTotal ());
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
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Ins_GetNumInssTotal ());
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Show number of centres in system configuration ***************/
/*****************************************************************************/

static void SysCfg_NumCtrs (void)
  {
   extern const char *Txt_Centres;

   /***** Number of centres *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Centres);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Ctr_GetNumCtrsTotal ());
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
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Deg_GetNumDegsTotal ());
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
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Crs_GetNumCrssTotal ());
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Number of users in courses of the system ******************/
/*****************************************************************************/

static void SysCfg_NumUsrsInCrss (Rol_Role_t Role)
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
   HTM_Unsigned (Usr_GetNumUsrsInCrss (Hie_SYS,-1L,
				       Role == Rol_UNK ? (1 << Rol_STD) |
							 (1 << Rol_NET) |
							 (1 << Rol_TCH) :	// Any user
							 (1 << Role)));
   HTM_TD_End ();

   HTM_TR_End ();
  }
