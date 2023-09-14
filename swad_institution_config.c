// swad_institution.c: institutions

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_center_database.h"
#include "swad_database.h"
#include "swad_department_database.h"
#include "swad_error.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_config.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_institution_database.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_place.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void InsCfg_Configuration (bool PrintView);
static void InsCfg_PutIconsToPrintAndUpload (__attribute__((unused)) void *Args);
static void InsCfg_Title (bool PutLink);
static void InsCfg_Map (void);
static void InsCfg_Country (bool PrintView,bool PutForm);
static void InsCfg_FullName (bool PutForm);
static void InsCfg_ShrtName (bool PutForm);
static void InsCfg_WWW (bool PrintView,bool PutForm);
static void InsCfg_Shortcut (bool PrintView);
static void InsCfg_QR (void);
static void InsCfg_NumUsrs (void);
static void InsCfg_NumDegs (void);
static void InsCfg_NumCrss (void);
static void InsCfg_NumDpts (void);

/*****************************************************************************/
/*************** Show information of the current institution *****************/
/*****************************************************************************/

void InsCfg_ShowConfiguration (void)
  {
   InsCfg_Configuration (false);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/*************** Print information of the current institution ****************/
/*****************************************************************************/

void InsCfg_PrintConfiguration (void)
  {
   InsCfg_Configuration (true);
  }

/*****************************************************************************/
/***************** Information of the current institution ********************/
/*****************************************************************************/

static void InsCfg_Configuration (bool PrintView)
  {
   extern const char *Hlp_INSTITUTION_Information;
   bool PutLink;
   bool PutFormCty;
   bool PutFormName;
   bool PutFormWWW;
   unsigned NumCtrs;
   unsigned NumCtrsWithMap;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.Cod <= 0)	// No institution selected
      return;

   /***** Initializations *****/
   PutLink     = !PrintView && Gbl.Hierarchy.Ins.WWW[0];
   PutFormCty  =
   PutFormName = !PrintView && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM;
   PutFormWWW  = !PrintView && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM;

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,
                    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,
                    InsCfg_PutIconsToPrintAndUpload,NULL,
		    Hlp_INSTITUTION_Information,Box_NOT_CLOSABLE);


   /***** Title *****/
   InsCfg_Title (PutLink);

   /**************************** Left part ***********************************/
   HTM_DIV_Begin ("class=\"HIE_CFG_LEFT HIE_CFG_WIDTH\"");

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

	 /***** Country *****/
	 InsCfg_Country (PrintView,PutFormCty);

	 /***** Institution name *****/
	 InsCfg_FullName (PutFormName);
	 InsCfg_ShrtName (PutFormName);

	 /***** Institution WWW *****/
	 InsCfg_WWW (PrintView,PutFormWWW);

	 /***** Shortcut to the institution *****/
	 InsCfg_Shortcut (PrintView);

	 NumCtrsWithMap = Ctr_GetCachedNumCtrsWithMapInIns (Gbl.Hierarchy.Ins.Cod);
	 if (PrintView)
	    /***** QR code with link to the institution *****/
	    InsCfg_QR ();
	 else
	   {
	    NumCtrs = Ctr_GetCachedNumCtrsInIns (Gbl.Hierarchy.Ins.Cod);

	    /***** Number of users who claim to belong to this institution,
		   number of centers,
		   number of degrees,
		   number of courses,
		   number of departments *****/
	    InsCfg_NumUsrs ();
	    HieCfg_NumCtrs (NumCtrs,
			    true);	// Put form
	    HieCfg_NumCtrsWithMap (NumCtrs,NumCtrsWithMap);
	    InsCfg_NumDegs ();
	    InsCfg_NumCrss ();
	    InsCfg_NumDpts ();

	    /***** Number of users in courses of this institution *****/
	    HieCfg_NumUsrsInCrss (HieLvl_INS,Gbl.Hierarchy.Ins.Cod,Rol_TCH);
	    HieCfg_NumUsrsInCrss (HieLvl_INS,Gbl.Hierarchy.Ins.Cod,Rol_NET);
	    HieCfg_NumUsrsInCrss (HieLvl_INS,Gbl.Hierarchy.Ins.Cod,Rol_STD);
	    HieCfg_NumUsrsInCrss (HieLvl_INS,Gbl.Hierarchy.Ins.Cod,Rol_UNK);
	   }

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End of left part *****/
   HTM_DIV_End ();

   /**************************** Right part **********************************/
   if (NumCtrsWithMap)
     {
      HTM_DIV_Begin ("class=\"HIE_CFG_RIGHT HIE_CFG_WIDTH\"");

	 /***** Institution map *****/
	 InsCfg_Map ();

      HTM_DIV_End ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* Put contextual icons in configuration of an institution ***********/
/*****************************************************************************/

static void InsCfg_PutIconsToPrintAndUpload (__attribute__((unused)) void *Args)
  {
   /***** Icon to print info about institution *****/
   Ico_PutContextualIconToPrint (ActPrnInsInf,
				 NULL,NULL);

   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      /***** Icon to upload logo of institution *****/
      Lgo_PutIconToChangeLogo (HieLvl_INS);

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();
  }

/*****************************************************************************/
/***************** Show title in institution configuration *******************/
/*****************************************************************************/

static void InsCfg_Title (bool PutLink)
  {
   HieCfg_Title (PutLink,
		    HieLvl_INS,				// Logo scope
		    Gbl.Hierarchy.Ins.Cod,		// Logo code
                    Gbl.Hierarchy.Ins.ShrtName,		// Logo short name
		    Gbl.Hierarchy.Ins.FullName,		// Logo full name
		    Gbl.Hierarchy.Ins.WWW,		// Logo www
		    Gbl.Hierarchy.Ins.FullName);	// Text full name
  }

/*****************************************************************************/
/****************************** Draw center map ******************************/
/*****************************************************************************/

#define InsCfg_MAP_CONTAINER_ID "ins_mapid"

static void InsCfg_Map (void)
  {
   MYSQL_RES *mysql_res;
   struct Map_Coordinates InsAvgCoord;
   unsigned Zoom;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Ctr_Center Ctr;

   /***** Leaflet CSS *****/
   Map_LeafletCSS ();

   /***** Leaflet script *****/
   Map_LeafletScript ();

   /***** Container for the map *****/
   HTM_DIV_Begin ("id=\"%s\"",InsCfg_MAP_CONTAINER_ID);
   HTM_DIV_End ();

   /***** Script to draw the map *****/
   HTM_SCRIPT_Begin (NULL,NULL);

   /* Let's create a map with pretty Mapbox Streets tiles */
   Ctr_DB_GetCoordAndZoomInCurrentIns (&InsAvgCoord,&Zoom);
   Map_CreateMap (InsCfg_MAP_CONTAINER_ID,&InsAvgCoord,Zoom);

   /* Add Mapbox Streets tile layer to our map */
   Map_AddTileLayer ();

   /* Get centers with coordinates */
   NumCtrs = Ctr_DB_GetCtrsWithCoordsInCurrentIns (&mysql_res);

   /* Add a marker and a popup for each center */
   for (NumCtr = 0;
	NumCtr < NumCtrs;
	NumCtr++)
     {
      /* Get next center */
      Ctr.Cod = DB_GetNextCode (mysql_res);

      /* Get data of center */
      Ctr_GetCenterDataByCod (&Ctr);

      /* Add marker */
      Map_AddMarker (&Ctr.Coord);

      /* Add popup */
      Map_AddPopup (Ctr.ShrtName,Gbl.Hierarchy.Ins.ShrtName,
		    false);	// Closed
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/***************** Show country in institution configuration *****************/
/*****************************************************************************/

static void InsCfg_Country (bool PrintView,bool PutForm)
  {
   extern const char *Par_CodeStr[];
   extern const char *Txt_Country;
   unsigned NumCty;
   const struct Cty_Countr *CtyInLst;

   /***** Get list of countries *****/
   Cty_GetBasicListOfCountries ();

   /***** Country *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",PutForm ? Par_CodeStr[ParCod_OthCty] :
				      NULL,
		       Txt_Country);

      /* Data */
      HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	 if (PutForm)
	   {
	    /* Put form to select country */
	    Frm_BeginForm (ActChgInsCtyCfg);
	       HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				 "id=\"OthCtyCod\" name=\"OthCtyCod\""
				 " class=\"INPUT_SHORT_NAME INPUT_%s\"",
				 The_GetSuffix ());
		  for (NumCty = 0;
		       NumCty < Gbl.Hierarchy.Ctys.Num;
		       NumCty++)
		    {
		     CtyInLst = &Gbl.Hierarchy.Ctys.Lst[NumCty];
		     HTM_OPTION (HTM_Type_LONG,&CtyInLst->Cod,
				 CtyInLst->Cod == Gbl.Hierarchy.Cty.Cod ? HTM_OPTION_SELECTED :
										HTM_OPTION_UNSELECTED,
				 HTM_OPTION_ENABLED,
				 "%s",CtyInLst->Name[Gbl.Prefs.Language]);
		    }
	       HTM_SELECT_End ();
	    Frm_EndForm ();
	   }
	 else	// I can not move institution to another country
	   {
	    if (!PrintView)
	      {
	       Frm_BeginFormGoTo (ActSeeCtyInf);
		  ParCod_PutPar (ParCod_Cty,Gbl.Hierarchy.Cty.Cod);
		  HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]),
					   "class=\"BT_LINK\"");
		  Str_FreeGoToTitle ();
	      }
	    Cty_DrawCountryMap (&Gbl.Hierarchy.Cty,"COUNTRY_MAP_TINY");
	    HTM_NBSP ();
	    HTM_Txt (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
	    if (!PrintView)
	      {
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	   }
      HTM_TD_End ();

   HTM_TR_End ();

   // Do not free list of countries here, because it can be reused
  }

/*****************************************************************************/
/********* Show institution full name in institution configuration ***********/
/*****************************************************************************/

static void InsCfg_FullName (bool PutForm)
  {
   extern const char *Txt_Institution;

   HieCfg_FullName (PutForm,Txt_Institution,ActRenInsFulCfg,
		    Gbl.Hierarchy.Ins.FullName);
  }

/*****************************************************************************/
/********* Show institution short name in institution configuration **********/
/*****************************************************************************/

static void InsCfg_ShrtName (bool PutForm)
  {
   HieCfg_ShrtName (PutForm,ActRenInsShoCfg,Gbl.Hierarchy.Ins.ShrtName);
  }

/*****************************************************************************/
/************ Show institution WWW in institution configuration **************/
/*****************************************************************************/

static void InsCfg_WWW (bool PrintView,bool PutForm)
  {
   HieCfg_WWW (PrintView,PutForm,ActChgInsWWWCfg,Gbl.Hierarchy.Ins.WWW);
  }

/*****************************************************************************/
/********** Show institution shortcut in institution configuration ***********/
/*****************************************************************************/

static void InsCfg_Shortcut (bool PrintView)
  {
   HieCfg_Shortcut (PrintView,ParCod_Ins,Gbl.Hierarchy.Ins.Cod);
  }

/*****************************************************************************/
/************* Show institution QR in institution configuration **************/
/*****************************************************************************/

static void InsCfg_QR (void)
  {
   HieCfg_QR (ParCod_Ins,Gbl.Hierarchy.Ins.Cod);
  }

/*****************************************************************************/
/** Show number of users who claim to belong to instit. in instit. config. ***/
/*****************************************************************************/

static void InsCfg_NumUsrs (void)
  {
   extern const char *Txt_Users_of_the_institution;

   /***** Number of users *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Users_of_the_institution);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Ins_GetCachedNumUsrsWhoClaimToBelongToIns (&Gbl.Hierarchy.Ins));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Show number of degrees in institution configuration *************/
/*****************************************************************************/

static void InsCfg_NumDegs (void)
  {
   extern const char *Txt_Degrees;

   /***** Number of degrees *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Degrees);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Deg_GetCachedNumDegsInIns (Gbl.Hierarchy.Ins.Cod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show number of courses in institution configuration ************/
/*****************************************************************************/

static void InsCfg_NumCrss (void)
  {
   extern const char *Txt_Courses;

   /***** Number of courses *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Courses);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Crs_GetCachedNumCrssInIns (Gbl.Hierarchy.Ins.Cod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********** Show number of departments in institution configuration **********/
/*****************************************************************************/

static void InsCfg_NumDpts (void)
  {
   extern const char *Txt_Departments;

   /***** Number of departments *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",NULL,Txt_Departments);

      /* Data */
      HTM_TD_Begin ("class=\"LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Dpt_GetNumDptsInIns (Gbl.Hierarchy.Ins.Cod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******** Show a form for sending a logo of the current institution **********/
/*****************************************************************************/

void InsCfg_ReqLogo (void)
  {
   Lgo_RequestLogo (HieLvl_INS);
  }

/*****************************************************************************/
/************** Receive the logo of the current institution ******************/
/*****************************************************************************/

void InsCfg_ReceiveLogo (void)
  {
   Lgo_ReceiveLogo (HieLvl_INS);
  }

/*****************************************************************************/
/*************** Remove the logo of the current institution ******************/
/*****************************************************************************/

void InsCfg_RemoveLogo (void)
  {
   Lgo_RemoveLogo (HieLvl_INS);
  }

/*****************************************************************************/
/******************* Change the country of a institution *********************/
/*****************************************************************************/

void InsCfg_ChangeInsCty (void)
  {
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_country_of_the_institution_X_has_changed_to_Y;
   struct Cty_Countr NewCty;

   /***** Get the new country code for the institution *****/
   NewCty.Cod = ParCod_GetAndCheckPar (ParCod_OthCty);

   /***** Check if country has changed *****/
   if (NewCty.Cod != Gbl.Hierarchy.Ins.PrtCod)
     {
      /***** Get data of the country from database *****/
      Cty_GetCountryDataByCod (&NewCty);

      /***** Check if it already exists an institution with the same name in the new country *****/
      if (Ins_DB_CheckIfInsNameExistsInCty ("ShortName",Gbl.Hierarchy.Ins.ShrtName,-1L,NewCty.Cod))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_institution_X_already_exists,
		          Gbl.Hierarchy.Ins.ShrtName);
      else if (Ins_DB_CheckIfInsNameExistsInCty ("FullName",Gbl.Hierarchy.Ins.FullName,-1L,NewCty.Cod))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_institution_X_already_exists,
		          Gbl.Hierarchy.Ins.FullName);
      else
	{
	 /***** Update the table changing the country of the institution *****/
	 Ins_DB_UpdateInsCty (Gbl.Hierarchy.Ins.Cod,NewCty.Cod);
         Gbl.Hierarchy.Ins.PrtCod =
         Gbl.Hierarchy.Cty.Cod = NewCty.Cod;

	 /***** Initialize again current course, degree, center... *****/
	 Hie_InitHierarchy ();

	 /***** Write message to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_country_of_the_institution_X_has_changed_to_Y,
		          Gbl.Hierarchy.Ins.FullName,NewCty.Name[Gbl.Prefs.Language]);
	}
     }
  }

/*****************************************************************************/
/************ Change the name of an institution in configuration *************/
/*****************************************************************************/

void InsCfg_RenameInsShort (void)
  {
   /***** Rename institution *****/
   Ins_RenameInstitution (&Gbl.Hierarchy.Ins,Cns_SHRT_NAME);
  }

void InsCfg_RenameInsFull (void)
  {
   /***** Rename institution *****/
   Ins_RenameInstitution (&Gbl.Hierarchy.Ins,Cns_FULL_NAME);
  }

/*****************************************************************************/
/********************** Change the URL of a institution **********************/
/*****************************************************************************/

void InsCfg_ChangeInsWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Get parameters from form *****/
   /* Get the new WWW for the institution */
   Par_GetParText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_DB_UpdateInsWWW (Gbl.Hierarchy.Ins.Cod,NewWWW);
      Str_Copy (Gbl.Hierarchy.Ins.WWW,NewWWW,sizeof (Gbl.Hierarchy.Ins.WWW) - 1);

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_new_web_address_is_X,
		     NewWWW);
     }
   else
      Ale_ShowAlertYouCanNotLeaveFieldEmpty ();

   /***** Show the form again *****/
   InsCfg_ShowConfiguration ();
  }

/*****************************************************************************/
/*** Show msg. of success after changing an institution in instit. config. ***/
/*****************************************************************************/

void InsCfg_ContEditAfterChgIns (void)
  {
   /***** Write success / warning message *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   InsCfg_ShowConfiguration ();
  }
