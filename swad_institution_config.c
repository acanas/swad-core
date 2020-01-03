// swad_institution.c: institutions

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

#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy_config.h"
#include "swad_HTML.h"
#include "swad_institution.h"
#include "swad_logo.h"

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

static void InsCfg_Configuration (bool PrintView);
static void InsCfg_PutIconsToPrintAndUpload (void);
static void InsCfg_Title (bool PutLink);
static bool InsCfg_GetIfMapIsAvailable (void);
static void InsCfg_GetAverageCoord (struct Coordinates *Coord);
static void InsCfg_Map (void);
static void InsCfg_Country (bool PrintView,bool PutForm);
static void InsCfg_FullName (bool PutForm);
static void InsCfg_ShrtName (bool PutForm);
static void InsCfg_WWW (bool PrintView,bool PutForm);
static void InsCfg_Shortcut (bool PrintView);
static void InsCfg_QR (void);
static void InsCfg_NumUsrs (void);
static void InsCfg_NumCtrs (void);
static void InsCfg_NumDegs (void);
static void InsCfg_NumCrss (void);
static void InsCfg_NumDpts (void);
static void InsCfg_NumUsrsInCrssOfIns (Rol_Role_t Role);
static void InsCfg_UpdateInsCtyDB (long InsCod,long CtyCod);

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
   bool MapIsAvailable;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.InsCod <= 0)	// No institution selected
      return;

   /***** Initializations *****/
   PutLink     = !PrintView && Gbl.Hierarchy.Ins.WWW[0];
   PutFormCty  =
   PutFormName = !PrintView && Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM;
   PutFormWWW  = !PrintView && Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM;

   /***** Begin box *****/
   if (PrintView)
      Box_BoxBegin (NULL,NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);
   else
      Box_BoxBegin (NULL,NULL,InsCfg_PutIconsToPrintAndUpload,
		    Hlp_INSTITUTION_Information,Box_NOT_CLOSABLE);


   /***** Title *****/
   InsCfg_Title (PutLink);

   /**************************** Left part ***********************************/
   HTM_DIV_Begin ("class=\"HIE_CFG_LEFT\"");

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

   if (PrintView)
      /***** QR code with link to the institution *****/
      InsCfg_QR ();
   else
     {
      /***** Number of users who claim to belong to this institution,
             number of centres,
             number of degrees,
             number of courses,
             number of departments *****/
      InsCfg_NumUsrs ();
      InsCfg_NumCtrs ();
      InsCfg_NumDegs ();
      InsCfg_NumCrss ();
      InsCfg_NumDpts ();

      /***** Number of users in courses of this institution *****/
      InsCfg_NumUsrsInCrssOfIns (Rol_TCH);
      InsCfg_NumUsrsInCrssOfIns (Rol_NET);
      InsCfg_NumUsrsInCrssOfIns (Rol_STD);
      InsCfg_NumUsrsInCrssOfIns (Rol_UNK);
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** End of left part *****/
   HTM_DIV_End ();

   /**************************** Right part **********************************/
   /***** Check map *****/
   MapIsAvailable = InsCfg_GetIfMapIsAvailable ();

   if (MapIsAvailable)
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

static void InsCfg_PutIconsToPrintAndUpload (void)
  {
   /***** Icon to print info about institution *****/
   Ico_PutContextualIconToPrint (ActPrnInsInf,NULL);

   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      /***** Icon to upload logo of institution *****/
      Lgo_PutIconToChangeLogo (Hie_INS);

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();
  }

/*****************************************************************************/
/***************** Show title in institution configuration *******************/
/*****************************************************************************/

static void InsCfg_Title (bool PutLink)
  {
   HieCfg_Title (PutLink,
		    Hie_INS,				// Logo scope
		    Gbl.Hierarchy.Ins.InsCod,		// Logo code
                    Gbl.Hierarchy.Ins.ShrtName,		// Logo short name
		    Gbl.Hierarchy.Ins.FullName,		// Logo full name
		    Gbl.Hierarchy.Ins.WWW,		// Logo www
		    Gbl.Hierarchy.Ins.FullName);	// Text full name
  }

/*****************************************************************************/
/******************** Check if centre map should be shown ********************/
/*****************************************************************************/

static bool InsCfg_GetIfMapIsAvailable (void)
  {
   /***** Get number of centres of current institution
          with both coordinates set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres in an institution",
			     "SELECT COUNT(*) FROM centres"
			     " WHERE InsCod=%ld"
			     " AND Latitude<>0"
			     " AND Longitude<>0",
			     Gbl.Hierarchy.Ins.InsCod);
  }

/*****************************************************************************/
/******************** Check if centre map should be shown ********************/
/*****************************************************************************/

static void InsCfg_GetAverageCoord (struct Coordinates *Coord)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get average coordinates of centres of current institution
          with both coordinates set
          (coordinates 0, 0 means not set ==> don't show map) *****/
   if (DB_QuerySELECT (&mysql_res,"can not get centres"
				  " with coordinates",
		       "SELECT AVG(Latitude),"	// row[0]
			      "AVG(Longitude)"	// row[1]
		       " FROM centres"
		       " WHERE InsCod=%ld"
		       " AND Latitude<>0"
		       " AND Longitude<>0",
		       Gbl.Hierarchy.Ins.InsCod))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get latitude (row[0]) */
      Coord->Latitude = Str_GetDoubleFromStr (row[0]);
      if (Coord->Latitude < -90.0)
	 Coord->Latitude = -90.0;	// South Pole
      else if (Coord->Latitude > 90.0)
	 Coord->Latitude = 90.0;	// North Pole

      /* Get longitude (row[1]) */
      Coord->Longitude = Str_GetDoubleFromStr (row[1]);
      if (Coord->Longitude < -180.0)
	 Coord->Longitude = -180.0;	// West
      else if (Coord->Longitude > 180.0)
	 Coord->Longitude = 180.0;	// East
     }
   else
      Coord->Latitude  =
      Coord->Longitude = 0.0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************************** Draw centre map ******************************/
/*****************************************************************************/

#define InsCfg_MAP_CONTAINER_ID "ins_mapid"

static void InsCfg_Map (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Coordinates InsAvgCoord;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Centre Ctr;

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
   InsCfg_GetAverageCoord (&InsAvgCoord);
   Map_CreateMap (InsCfg_MAP_CONTAINER_ID,&InsAvgCoord);

   /* Add Mapbox Streets tile layer to our map */
   Map_AddTileLayer ();

   /* Get centres with coordinates */
   NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres"
						   " with coordinates",
					"SELECT CtrCod"	// row[0]
					" FROM centres"
					" WHERE InsCod=%ld"
					" AND Latitude<>0"
					" AND Longitude<>0",
					Gbl.Hierarchy.Ins.InsCod);

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
      Ctr_GetDataOfCentreByCod (&Ctr,Ctr_GET_BASIC_DATA);

      /* Add marker */
      Map_AddMarker (&Ctr.Coord);

      /* Add popup */
      Map_AddPopup (Ctr.ShrtName,Gbl.Hierarchy.Ins.ShrtName);
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
   extern const char *Txt_Country;
   unsigned NumCty;

   /***** Country *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",PutForm ? "OthCtyCod" :
	                           NULL,
		    Txt_Country);

   /* Data */
   HTM_TD_Begin ("class=\"DAT LB\"");
   if (PutForm)
     {
      /* Get list of countries */
      Cty_GetListCountries (Cty_GET_BASIC_DATA);

      /* Put form to select country */
      Frm_StartForm (ActChgInsCtyCfg);
      HTM_SELECT_Begin (true,
			"id=\"OthCtyCod\" name=\"OthCtyCod\""
		        " class=\"INPUT_SHORT_NAME\"");
      for (NumCty = 0;
	   NumCty < Gbl.Hierarchy.Sys.Ctys.Num;
	   NumCty++)
	 HTM_OPTION (HTM_Type_LONG,&Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].CtyCod,
		     Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].CtyCod == Gbl.Hierarchy.Cty.CtyCod,false,
		     "%s",Gbl.Hierarchy.Sys.Ctys.Lst[NumCty].Name[Gbl.Prefs.Language]);
      HTM_SELECT_End ();
      Frm_EndForm ();

      /* Free list of countries */
      Cty_FreeListCountries ();
     }
   else	// I can not move institution to another country
     {
      if (!PrintView)
	{
         Frm_StartFormGoTo (ActSeeCtyInf);
         Cty_PutParamCtyCod (Gbl.Hierarchy.Cty.CtyCod);
	 HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]),
				  "BT_LINK LT DAT",NULL);
         Hie_FreeGoToMsg ();
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
   HieCfg_Shortcut (PrintView,"ins",Gbl.Hierarchy.Ins.InsCod);
  }

/*****************************************************************************/
/************* Show institution QR in institution configuration **************/
/*****************************************************************************/

static void InsCfg_QR (void)
  {
   HieCfg_QR ("ins",Gbl.Hierarchy.Ins.InsCod);
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
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToIns (Gbl.Hierarchy.Ins.InsCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Show number of centres in institution configuration *************/
/*****************************************************************************/

static void InsCfg_NumCtrs (void)
  {
   extern const char *Txt_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;

   /***** Number of centres *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",NULL,Txt_Centres);

   /* Data */
   HTM_TD_Begin ("class=\"LB\"");
   Frm_StartFormGoTo (ActSeeCtr);
   Ins_PutParamInsCod (Gbl.Hierarchy.Ins.InsCod);
   HTM_BUTTON_SUBMIT_Begin (Str_BuildStringStr (Txt_Centres_of_INSTITUTION_X,
					        Gbl.Hierarchy.Ins.ShrtName),
			    "BT_LINK DAT",NULL);
   Str_FreeString ();
   HTM_Unsigned (Ctr_GetNumCtrsInIns (Gbl.Hierarchy.Ins.InsCod));
   HTM_BUTTON_End ();
   Frm_EndForm ();
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
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Deg_GetNumDegsInIns (Gbl.Hierarchy.Ins.InsCod));
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
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Crs_GetNumCrssInIns (Gbl.Hierarchy.Ins.InsCod));
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
   HTM_TD_Begin ("class=\"DAT LB\"");
   HTM_Unsigned (Dpt_GetNumDepartmentsInInstitution (Gbl.Hierarchy.Ins.InsCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Number of users in courses of this institution ***************/
/*****************************************************************************/

static void InsCfg_NumUsrsInCrssOfIns (Rol_Role_t Role)
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
   HTM_Unsigned (Usr_GetNumUsrsInCrssOfIns (Role,Gbl.Hierarchy.Ins.InsCod));
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******** Show a form for sending a logo of the current institution **********/
/*****************************************************************************/

void InsCfg_RequestLogo (void)
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
   extern const char *Txt_The_institution_X_already_exists;
   extern const char *Txt_The_country_of_the_institution_X_has_changed_to_Y;
   struct Country NewCty;

   /***** Get the new country code for the institution *****/
   NewCty.CtyCod = Cty_GetAndCheckParamOtherCtyCod (0);

   /***** Check if country has changed *****/
   if (NewCty.CtyCod != Gbl.Hierarchy.Ins.CtyCod)
     {
      /***** Get data of the country from database *****/
      Cty_GetDataOfCountryByCod (&NewCty,Cty_GET_BASIC_DATA);

      /***** Check if it already exists an institution with the same name in the new country *****/
      if (Ins_CheckIfInsNameExistsInCty ("ShortName",Gbl.Hierarchy.Ins.ShrtName,-1L,NewCty.CtyCod))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_institution_X_already_exists,
		          Gbl.Hierarchy.Ins.ShrtName);
      else if (Ins_CheckIfInsNameExistsInCty ("FullName",Gbl.Hierarchy.Ins.FullName,-1L,NewCty.CtyCod))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_institution_X_already_exists,
		          Gbl.Hierarchy.Ins.FullName);
      else
	{
	 /***** Update the table changing the country of the institution *****/
	 InsCfg_UpdateInsCtyDB (Gbl.Hierarchy.Ins.InsCod,NewCty.CtyCod);
         Gbl.Hierarchy.Ins.CtyCod =
         Gbl.Hierarchy.Cty.CtyCod = NewCty.CtyCod;

	 /***** Initialize again current course, degree, centre... *****/
	 Hie_InitHierarchy ();

	 /***** Write message to show the change made *****/
         Ale_CreateAlert (Ale_SUCCESS,NULL,
                          Txt_The_country_of_the_institution_X_has_changed_to_Y,
		          Gbl.Hierarchy.Ins.FullName,NewCty.Name[Gbl.Prefs.Language]);
	}
     }
  }

/*****************************************************************************/
/****************** Update country in table of institutions ******************/
/*****************************************************************************/

static void InsCfg_UpdateInsCtyDB (long InsCod,long CtyCod)
  {
   /***** Update country in table of institutions *****/
   DB_QueryUPDATE ("can not update the country of an institution",
		   "UPDATE institutions SET CtyCod=%ld WHERE InsCod=%ld",
                   CtyCod,InsCod);
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
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ins_UpdateInsWWWDB (Gbl.Hierarchy.Ins.InsCod,NewWWW);
      Str_Copy (Gbl.Hierarchy.Ins.WWW,NewWWW,
                Cns_MAX_BYTES_WWW);

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

