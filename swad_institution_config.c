// swad_institution.c: institutions

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
#include "swad_hierarchy_type.h"
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

static void InsCfg_Configuration (Vie_ViewType_t ViewType);
static void InsCfg_PutIconsToPrintAndUpload (__attribute__((unused)) void *Args);
static void InsCfg_Map (void);
static void InsCfg_Country (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm);
static void InsCfg_NumUsrs (void);
static void InsCfg_NumDegs (void);
static void InsCfg_NumCrss (void);
static void InsCfg_NumDpts (void);

/*****************************************************************************/
/*************** Show information of the current institution *****************/
/*****************************************************************************/

void InsCfg_ShowConfiguration (void)
  {
   InsCfg_Configuration (Vie_VIEW);

   /***** Show help to enrol me *****/
   Hlp_ShowHelpWhatWouldYouLikeToDo ();
  }

/*****************************************************************************/
/*************** Print information of the current institution ****************/
/*****************************************************************************/

void InsCfg_PrintConfiguration (void)
  {
   InsCfg_Configuration (Vie_PRINT);
  }

/*****************************************************************************/
/***************** Information of the current institution ********************/
/*****************************************************************************/

static void InsCfg_Configuration (Vie_ViewType_t ViewType)
  {
   extern const char *Hlp_INSTITUTION_Information;
   Hie_PutLink_t PutLink;
   Frm_PutForm_t PutFormCty;
   Frm_PutForm_t PutFormName;
   Frm_PutForm_t PutFormWWW;
   unsigned NumCtrs;
   unsigned NumCtrsWithMap;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_INS].HieCod <= 0)	// No institution selected
      return;

   /***** Initializations *****/
   PutLink     = (ViewType == Vie_VIEW &&
		  Gbl.Hierarchy.Node[Hie_INS].WWW[0]) ? Hie_PUT_LINK :
							Hie_DONT_PUT_LINK;
   PutFormCty  =
   PutFormName = (ViewType == Vie_VIEW &&
		  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Frm_PUT_FORM :
							    Frm_DONT_PUT_FORM;
   PutFormWWW  = (ViewType == Vie_VIEW &&
		  Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM) ? Frm_PUT_FORM :
							    Frm_DONT_PUT_FORM;

   /***** Begin box *****/
   Box_BoxBegin (NULL,
		 ViewType == Vie_VIEW ? InsCfg_PutIconsToPrintAndUpload :
					NULL,NULL,
		 ViewType == Vie_VIEW ? Hlp_INSTITUTION_Information :
					NULL,Box_NOT_CLOSABLE);

      /***** Title *****/
      HieCfg_Title (PutLink,Hie_INS);

      /**************************** Left part ***********************************/
      HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

	 /***** Begin table *****/
	 HTM_TABLE_BeginPadding (2);

	    /***** Country *****/
	    InsCfg_Country (ViewType,PutFormCty);

	    /***** Institution name *****/
	    HieCfg_Name (PutFormName,Hie_INS,Nam_FULL_NAME);
	    HieCfg_Name (PutFormName,Hie_INS,Nam_SHRT_NAME);

	    /***** Institution WWW *****/
	    HieCfg_WWW (ViewType,PutFormWWW,ActChgInsWWWCfg,Gbl.Hierarchy.Node[Hie_INS].WWW);

	    /***** Shortcut to the institution *****/
	    HieCfg_Shortcut (ViewType,ParCod_Ins,Gbl.Hierarchy.Node[Hie_INS].HieCod);

	    NumCtrsWithMap = Ctr_GetCachedNumCtrsWithMapInIns (Gbl.Hierarchy.Node[Hie_INS].HieCod);

	    switch (ViewType)
	      {
	       case Vie_VIEW:
		  NumCtrs = Hie_GetCachedNumNodesInHieLvl (Hie_CTR,	// Number of centers...
							   Hie_INS,	// ...in institution
							   Gbl.Hierarchy.Node[Hie_INS].HieCod);

		  /***** Number of users who claim to belong to this institution,
			 number of centers,
			 number of degrees,
			 number of courses,
			 number of departments *****/
		  InsCfg_NumUsrs ();
		  HieCfg_NumCtrs (NumCtrs,Frm_PUT_FORM);
		  HieCfg_NumCtrsWithMap (NumCtrs,NumCtrsWithMap);
		  InsCfg_NumDegs ();
		  InsCfg_NumCrss ();
		  InsCfg_NumDpts ();

		  /***** Number of users in courses of this institution *****/
		  HieCfg_NumUsrsInCrss (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod,Rol_TCH);
		  HieCfg_NumUsrsInCrss (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod,Rol_NET);
		  HieCfg_NumUsrsInCrss (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod,Rol_STD);
		  HieCfg_NumUsrsInCrss (Hie_INS,Gbl.Hierarchy.Node[Hie_INS].HieCod,Rol_UNK);
		  break;
	       case Vie_PRINT:
		  /***** QR code with link to the institution *****/
		  HieCfg_QR (ParCod_Ins,Gbl.Hierarchy.Node[Hie_INS].HieCod);
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
   Ico_PutContextualIconToPrint (ActPrnInsInf,NULL,NULL);

   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      /***** Icon to upload logo of institution *****/
      Lgo_PutIconToChangeLogo (Hie_INS);

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();
  }

/*****************************************************************************/
/*************************** Draw institution map ****************************/
/*****************************************************************************/

#define InsCfg_MAP_CONTAINER_ID "ins_mapid"

static void InsCfg_Map (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   MYSQL_RES *mysql_res;
   unsigned Zoom;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Hie_Node Ctr;
   struct Map_Coordinates Coord;
   struct Map_Coordinates InsAvgCoord;

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
   Ctr_DB_GetAvgCoordAndZoomInCurrentIns (&InsAvgCoord,&Zoom);
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
      Ctr.HieCod = DB_GetNextCode (mysql_res);

      /* Get data of center */
      Hie_GetDataByCod[Hie_CTR] (&Ctr);

      /* Get coordinates of center */
      Ctr_GetCoordByCod (Ctr.HieCod,&Coord);

      /* Add marker */
      Map_AddMarker (&Coord);

      /* Add popup */
      Map_AddPopup (Ctr.ShrtName,Gbl.Hierarchy.Node[Hie_INS].ShrtName,
		    false);	// Closed
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   HTM_SCRIPT_End ();
  }

/*****************************************************************************/
/***************** Show country in institution configuration *****************/
/*****************************************************************************/

static void InsCfg_Country (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm)
  {
   extern const char *Par_CodeStr[Par_NUM_PAR_COD];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   unsigned NumCty;
   const struct Hie_Node *Cty;
   const char *Id[Frm_NUM_PUT_FORM] =
     {
      [Frm_DONT_PUT_FORM] = NULL,
      [Frm_PUT_FORM     ] = Par_CodeStr[ParCod_OthCty],
     };

   /***** Get list of countries *****/
   Cty_GetBasicListOfCountries ();

   /***** Country *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",Id[PutForm],Txt_HIERARCHY_SINGUL_Abc[Hie_CTY]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
         switch (PutForm)
           {
            case Frm_DONT_PUT_FORM:	// I can not move institution to another country
	       if (ViewType == Vie_VIEW)
		 {
		  Frm_BeginFormGoTo (ActSeeCtyInf);
		     ParCod_PutPar (ParCod_Cty,Gbl.Hierarchy.Node[Hie_CTY].HieCod);
		     HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Gbl.Hierarchy.Node[Hie_CTY].FullName),NULL,
					      "class=\"BT_LINK\"");
		     Str_FreeGoToTitle ();
		 }
	       Cty_DrawCountryMap (&Gbl.Hierarchy.Node[Hie_CTY],"COUNTRY_MAP_TINY");
	       HTM_NBSP ();
	       HTM_Txt (Gbl.Hierarchy.Node[Hie_CTY].FullName);
	       if (ViewType == Vie_VIEW)
		 {
		     HTM_BUTTON_End ();
		  Frm_EndForm ();
		 }
               break;
            case Frm_PUT_FORM:
	       /* Put form to select country */
	       Frm_BeginForm (ActChgInsCtyCfg);
		  HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				    "id=\"OthCtyCod\" name=\"OthCtyCod\""
				    " class=\"Frm_C2_INPUT INPUT_%s\"",
				    The_GetSuffix ());
		     for (NumCty = 0;
			  NumCty < Gbl.Hierarchy.List[Hie_SYS].Num;
			  NumCty++)
		       {
			Cty = &Gbl.Hierarchy.List[Hie_SYS].Lst[NumCty];
			HTM_OPTION (HTM_Type_LONG,&Cty->HieCod,
				    (Cty->HieCod == Gbl.Hierarchy.Node[Hie_CTY].HieCod) ? HTM_SELECTED :
											  HTM_NO_ATTR,
				    "%s",Cty->FullName);
		       }
		  HTM_SELECT_End ();
	       Frm_EndForm ();
               break;
           }
      HTM_TD_End ();

   HTM_TR_End ();

   // Do not free list of countries here, because it can be reused
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
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Users_of_the_institution);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_INS,
							       &Gbl.Hierarchy.Node[Hie_INS]));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Show number of degrees in institution configuration *************/
/*****************************************************************************/

static void InsCfg_NumDegs (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of degrees *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_DEG]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
						      Hie_INS,	// ...in institution
						      Gbl.Hierarchy.Node[Hie_INS].HieCod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************ Show number of courses in institution configuration ************/
/*****************************************************************************/

static void InsCfg_NumCrss (void)
  {
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];

   /***** Number of courses *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_HIERARCHY_PLURAL_Abc[Hie_CRS]);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
						      Hie_INS,	// ...in institution
						      Gbl.Hierarchy.Node[Hie_INS].HieCod));
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
      Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Departments);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LB DAT_%s\"",The_GetSuffix ());
	 HTM_Unsigned (Dpt_GetNumDptsInIns (Gbl.Hierarchy.Node[Hie_INS].HieCod));
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******** Show a form for sending a logo of the current institution **********/
/*****************************************************************************/

void InsCfg_ReqLogo (void)
  {
   Lgo_RequestLogo (Hie_INS);
  }

/*****************************************************************************/
/************** Receive the logo of the current institution ******************/
/*****************************************************************************/

void InsCfg_ReceiveLogo (void)
  {
   Lgo_ReceiveLogo (Hie_INS);
  }

/*****************************************************************************/
/*************** Remove the logo of the current institution ******************/
/*****************************************************************************/

void InsCfg_RemoveLogo (void)
  {
   Lgo_RemoveLogo (Hie_INS);
  }

/*****************************************************************************/
/******************* Change the country of a institution *********************/
/*****************************************************************************/

void InsCfg_ChangeInsCty (void)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_country_of_the_institution_X_has_changed_to_Y;
   struct Hie_Node NewCty;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get the new country code for the institution *****/
   NewCty.HieCod = ParCod_GetAndCheckPar (ParCod_OthCty);

   /***** Check if country has changed *****/
   if (NewCty.HieCod != Gbl.Hierarchy.Node[Hie_INS].PrtCod)
     {
      /***** Get data of the country from database *****/
      Hie_GetDataByCod[Hie_CTY] (&NewCty);

      /***** Check if it already exists an institution with the same name in the new country *****/
      Names[Nam_SHRT_NAME] = Gbl.Hierarchy.Node[Hie_INS].ShrtName;
      Names[Nam_FULL_NAME] = Gbl.Hierarchy.Node[Hie_INS].FullName;
      if (!Nam_CheckIfNameExists (Ins_DB_CheckIfInsNameExistsInCty,
				  Names,
				  -1L,
				  NewCty.HieCod,
				  0))	// Unused
	{
	 /***** Update the table changing the country of the institution *****/
	 Ins_DB_UpdateInsCty (Gbl.Hierarchy.Node[Hie_INS].HieCod,NewCty.HieCod);
         Gbl.Hierarchy.Node[Hie_INS].PrtCod =
         Gbl.Hierarchy.Node[Hie_CTY].HieCod = NewCty.HieCod;

	 /***** Initialize again current course, degree, center... *****/
	 Hie_InitHierarchy ();

	 /***** Write message to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_country_of_the_institution_X_has_changed_to_Y,
		          Names[Nam_FULL_NAME],NewCty.FullName);
	}
     }
  }

/*****************************************************************************/
/************ Change the name of an institution in configuration *************/
/*****************************************************************************/

void InsCfg_RenameInsShort (void)
  {
   /***** Rename institution *****/
   Ins_RenameInstitution (&Gbl.Hierarchy.Node[Hie_INS],Nam_SHRT_NAME);
  }

void InsCfg_RenameInsFull (void)
  {
   /***** Rename institution *****/
   Ins_RenameInstitution (&Gbl.Hierarchy.Node[Hie_INS],Nam_FULL_NAME);
  }

/*****************************************************************************/
/********************** Change the URL of a institution **********************/
/*****************************************************************************/

void InsCfg_ChangeInsWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[WWW_MAX_BYTES_WWW + 1];

   /***** Get parameters from form *****/
   /* Get the new WWW for the institution */
   Par_GetParText ("WWW",NewWWW,WWW_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_DB_UpdateInsWWW (Gbl.Hierarchy.Node[Hie_INS].HieCod,NewWWW);
      Str_Copy (Gbl.Hierarchy.Node[Hie_INS].WWW,NewWWW,
	        sizeof (Gbl.Hierarchy.Node[Hie_INS].WWW) - 1);

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
