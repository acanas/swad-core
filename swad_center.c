// swad_center.c: centers

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
#include "swad_admin_database.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_center.h"
#include "swad_center_config.h"
#include "swad_center_database.h"
#include "swad_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum_database.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_place.h"
#include "swad_room_database.h"
#include "swad_survey.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Ctr_Center *Ctr_EditingCtr = NULL;	// Static variable to keep the center being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ctr_ListCenters (void);
static void Ctr_PutIconsListingCenters (__attribute__((unused)) void *Args);
static void Ctr_PutIconToEditCenters (void);
static void Ctr_ListOneCenterForSeeing (struct Ctr_Center *Ctr,unsigned NumCtr);
static void Ctr_GetParCtrOrder (void);

static void Ctr_EditCentersInternal (void);
static void Ctr_PutIconsEditingCenters (__attribute__((unused)) void *Args);

static void Ctr_GetCenterDataFromRow (MYSQL_RES *mysql_res,
				      struct Ctr_Center *Ctr,
                                      bool GetNumUsrsWhoClaimToBelongToCtr);

static void Ctr_ListCentersForEdition (const struct Plc_Places *Places);
static bool Ctr_CheckIfICanEditACenter (struct Ctr_Center *Ctr);

static void Ctr_ShowAlertAndButtonToGoToCtr (void);

static void Ctr_PutFormToCreateCenter (const struct Plc_Places *Places);
static void Ctr_PutHeadCentersForSeeing (bool OrderSelectable);
static void Ctr_PutHeadCentersForEdition (void);
static void Ctr_ReceiveFormRequestOrCreateCtr (Hie_Status_t Status);

static unsigned Ctr_GetNumCtrsInCty (long CtyCod);

static void Ctr_EditingCenterConstructor (void);
static void Ctr_EditingCenterDestructor (void);

static void Ctr_FormToGoToMap (struct Ctr_Center *Ctr);

static void Ctr_PutParCtrCod (void *CtrCod);

/*****************************************************************************/
/******************* List centers with pending degrees ***********************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void)
  {
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Centers_with_pending_degrees;
   extern const char *Txt_Center;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_There_are_no_centers_with_requests_for_degrees_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Ctr_Center Ctr;
   const char *BgColor;

   /***** Get centers with pending degrees *****/
   if ((NumCtrs = Ctr_DB_GetCtrsWithPendingDegs (&mysql_res)))
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Centers_with_pending_degrees,
                         NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

	 /***** Wrtie heading *****/
	 HTM_TR_Begin (NULL);

            HTM_TH (Txt_Center              ,HTM_HEAD_LEFT );
            HTM_TH (Txt_Degrees_ABBREVIATION,HTM_HEAD_RIGHT);

	 HTM_TR_End ();

	 /***** List the centers *****/
	 for (NumCtr = 0, The_ResetRowColor ();
	      NumCtr < NumCtrs;
	      NumCtr++, The_ChangeRowColor ())
	   {
	    /* Get next center */
	    row = mysql_fetch_row (mysql_res);

	    /* Get center code (row[0]) */
	    Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Ctr.CtrCod == Gbl.Hierarchy.Ctr.CtrCod) ? "BG_HIGHLIGHT" :
								 The_GetColorRows ();

	    /* Get data of center */
	    Ctr_GetCenterDataByCod (&Ctr);

	    /* Center logo and full name */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM DAT_%s NOWRAP %s\"",
	                     The_GetSuffix (),BgColor);
		  Ctr_DrawCenterLogoAndNameWithLink (&Ctr,ActSeeDeg,"CM");
	       HTM_TD_End ();

	       /* Number of pending degrees (row[1]) */
	       HTM_TD_Begin ("class=\"RM DAT_%s %s\"",
	                     The_GetSuffix (),BgColor);
		  HTM_Txt (row[1]);
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_centers_with_requests_for_degrees_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Draw center logo and name with link ********************/
/*****************************************************************************/

void Ctr_DrawCenterLogoAndNameWithLink (struct Ctr_Center *Ctr,Act_Action_t Action,
                                        const char *ClassLogo)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
      ParCod_PutPar (ParCod_Ctr,Ctr->CtrCod);

      /***** Link to action *****/
      HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Ctr->FullName),
                               "class=\"BT_LINK LT\"");
      Str_FreeGoToTitle ();

	 /***** Center logo and name *****/
	 Lgo_DrawLogo (HieLvl_CTR,Ctr->CtrCod,Ctr->ShrtName,16,ClassLogo);
	 HTM_TxtF ("&nbsp;%s",Ctr->FullName);

      /***** End link *****/
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** Map *****/
   Ctr_FormToGoToMap (Ctr);
  }

/*****************************************************************************/
/*************** Show the centers of the current institution *****************/
/*****************************************************************************/

void Ctr_ShowCtrsOfCurrentIns (void)
  {
   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.InsCod <= 0)		// No institution selected
      return;

   /***** Get parameter with the type of order in the list of centers *****/
   Ctr_GetParCtrOrder ();

   /***** Get list of centers *****/
   Ctr_GetFullListOfCenters (Gbl.Hierarchy.Ins.InsCod,
                             Gbl.Hierarchy.Ctrs.SelectedOrder);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** List centers *****/
   Ctr_ListCenters ();

   /***** Free list of centers *****/
   Ctr_FreeListCenters ();
  }

/*****************************************************************************/
/******************** List centers in this institution ***********************/
/*****************************************************************************/

static void Ctr_ListCenters (void)
  {
   extern const char *Hlp_INSTITUTION_Centers;
   extern const char *Txt_Centers_of_INSTITUTION_X;
   extern const char *Txt_No_centers;
   char *Title;
   unsigned NumCtr;

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Centers_of_INSTITUTION_X,Gbl.Hierarchy.Ins.FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (NULL,Title,Ctr_PutIconsListingCenters,NULL,
                 Hlp_INSTITUTION_Centers,Box_NOT_CLOSABLE);
   free (Title);

      if (Gbl.Hierarchy.Ctrs.Num)	// There are centers in the current institution
	{
	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);

	    /***** Write heading *****/
	    Ctr_PutHeadCentersForSeeing (true);	// Order selectable

	    /***** Write all centers and their nuber of teachers *****/
	    for (NumCtr = 0;
		 NumCtr < Gbl.Hierarchy.Ctrs.Num;
		 NumCtr++)
	       Ctr_ListOneCenterForSeeing (&(Gbl.Hierarchy.Ctrs.Lst[NumCtr]),NumCtr + 1);

	 /***** End table *****/
	 HTM_TABLE_End ();
	}
      else	// No centers created in the current institution
	 Ale_ShowAlert (Ale_INFO,Txt_No_centers);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/***************** Put contextual icons in list of centers *******************/
/*****************************************************************************/

static void Ctr_PutIconsListingCenters (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit centers *****/
   if (Hie_CheckIfICanEdit ())
      Ctr_PutIconToEditCenters ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/********************** Put link (form) to edit centers **********************/
/*****************************************************************************/

static void Ctr_PutIconToEditCenters (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCtr,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/************************* List one center for seeing ************************/
/*****************************************************************************/

static void Ctr_ListOneCenterForSeeing (struct Ctr_Center *Ctr,unsigned NumCtr)
  {
   extern const char *Txt_CENTER_STATUS[Hie_NUM_STATUS_TXT];
   struct Plc_Place Plc;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;

   if (Ctr->Status & Hie_STATUS_BIT_PENDING)
     {
      TxtClassNormal =
      TxtClassStrong = "DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "DAT_STRONG";
     }
   BgColor = (Ctr->CtrCod == Gbl.Hierarchy.Ctr.CtrCod) ? "BG_HIGHLIGHT" :
                                                         The_GetColorRows ();

   HTM_TR_Begin (NULL);

      /***** Number of center in this list *****/
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumCtr);
      HTM_TD_End ();

      /***** Center logo and name *****/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
                    TxtClassStrong,The_GetSuffix (),BgColor);
	 Ctr_DrawCenterLogoAndNameWithLink (Ctr,ActSeeDeg,"CM");
      HTM_TD_End ();

      /***** Number of users who claim to belong to this center *****/
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Ctr_GetCachedNumUsrsWhoClaimToBelongToCtr (Ctr));
      HTM_TD_End ();

      /***** Place *****/
      HTM_TD_Begin ("class=\"LM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 Plc.PlcCod = Ctr->PlcCod;
	 Plc_GetPlaceDataByCod (&Plc);
	 HTM_Txt (Plc.ShrtName);
      HTM_TD_End ();

      /***** Number of degrees *****/
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Deg_GetCachedNumDegsInCtr (Ctr->CtrCod));
      HTM_TD_End ();

      /***** Number of courses *****/
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Crs_GetCachedNumCrssInCtr (Ctr->CtrCod));
      HTM_TD_End ();

      /***** Number of users in courses of this center *****/
      HTM_TD_Begin ("class=\"RM %s_%s %s\"",
                    TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (HieLvl_CTR,Ctr->CtrCod,
						   1 << Rol_STD |
						   1 << Rol_NET |
						   1 << Rol_TCH));	// Any user
      HTM_TD_End ();

      /***** Center status *****/
      Hie_WriteStatusCell (Ctr->Status,TxtClassNormal,BgColor,Txt_CENTER_STATUS);

   HTM_TR_End ();
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of centers **********/
/*****************************************************************************/

static void Ctr_GetParCtrOrder (void)
  {
   Gbl.Hierarchy.Ctrs.SelectedOrder = (Ctr_Order_t)
				      Par_GetParUnsignedLong ("Order",
							      0,
							      Ctr_NUM_ORDERS - 1,
							      (unsigned long) Ctr_ORDER_DEFAULT);
  }

/*****************************************************************************/
/************************** Put forms to edit centers ************************/
/*****************************************************************************/

void Ctr_EditCenters (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Edit centers *****/
   Ctr_EditCentersInternal ();

   /***** Center destructor *****/
   Ctr_EditingCenterDestructor ();
  }

static void Ctr_EditCentersInternal (void)
  {
   extern const char *Hlp_INSTITUTION_Centers;
   extern const char *Txt_Centers_of_INSTITUTION_X;
   struct Plc_Places Places;
   char *Title;

   /***** Reset places context *****/
   Plc_ResetPlaces (&Places);

   /***** Get list of places *****/
   Places.SelectedOrder = Plc_ORDER_BY_PLACE;
   Plc_GetListPlaces (&Places);

   /***** Get list of centers *****/
   Gbl.Hierarchy.Ctrs.SelectedOrder = Ctr_ORDER_BY_CENTER;
   Ctr_GetFullListOfCenters (Gbl.Hierarchy.Ins.InsCod,
                             Gbl.Hierarchy.Ctrs.SelectedOrder);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Centers_of_INSTITUTION_X,Gbl.Hierarchy.Ins.FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (NULL,Title,Ctr_PutIconsEditingCenters,NULL,
                 Hlp_INSTITUTION_Centers,Box_NOT_CLOSABLE);
   free (Title);

      /***** Put a form to create a new center *****/
      Ctr_PutFormToCreateCenter (&Places);

      /***** List current centers *****/
      if (Gbl.Hierarchy.Ctrs.Num)
	 Ctr_ListCentersForEdition (&Places);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of centers *****/
   Ctr_FreeListCenters ();

   /***** Free list of places *****/
   Plc_FreeListPlaces (&Places);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of centers *****************/
/*****************************************************************************/

static void Ctr_PutIconsEditingCenters (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view centers *****/
   Ico_PutContextualIconToView (ActSeeCtr,NULL,
				NULL,NULL);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/************ Get basic list of centers ordered by name of center ************/
/*****************************************************************************/

void Ctr_GetBasicListOfCenters (long InsCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumCtr;

   /***** Get centers from database *****/
   Gbl.Hierarchy.Ctrs.Num = Ctr_DB_GetListOfCtrsFull (&mysql_res,InsCod);

   if (Gbl.Hierarchy.Ctrs.Num) // Centers found...
     {
      /***** Create list with centers in institution *****/
      if ((Gbl.Hierarchy.Ctrs.Lst = calloc ((size_t) Gbl.Hierarchy.Ctrs.Num,
                                            sizeof (*Gbl.Hierarchy.Ctrs.Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the centers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ctrs.Num;
	   NumCtr++)
         /* Get center data */
         Ctr_GetCenterDataFromRow (mysql_res,&Gbl.Hierarchy.Ctrs.Lst[NumCtr],
                                   false);	// Don't get number of users who claim to belong to this center
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get full list of centers                         **************/
/************* with number of users who claim to belong to them **************/
/*****************************************************************************/

void Ctr_GetFullListOfCenters (long InsCod,Ctr_Order_t SelectedOrder)
  {
   MYSQL_RES *mysql_res;
   unsigned NumCtr;

   /***** Get centers from database *****/
   Gbl.Hierarchy.Ctrs.Num = Ctr_DB_GetListOfCtrsFullWithNumUsrs (&mysql_res,InsCod,SelectedOrder);

   if (Gbl.Hierarchy.Ctrs.Num) // Centers found...
     {
      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.Ctrs.Lst = calloc ((size_t) Gbl.Hierarchy.Ctrs.Num,
                                            sizeof (*Gbl.Hierarchy.Ctrs.Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the centers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ctrs.Num;
	   NumCtr++)
         /* Get center data */
         Ctr_GetCenterDataFromRow (mysql_res,&Gbl.Hierarchy.Ctrs.Lst[NumCtr],
                                   true);	// Get number of users who claim to belong to this center
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get data of center by code *************************/
/*****************************************************************************/

bool Ctr_GetCenterDataByCod (struct Ctr_Center *Ctr)
  {
   MYSQL_RES *mysql_res;
   bool CtrFound = false;

   /***** Clear data *****/
   Ctr->InsCod          = -1L;
   Ctr->PlcCod          = -1L;
   Ctr->Status          = (Hie_Status_t) 0;
   Ctr->RequesterUsrCod = -1L;
   Ctr->ShrtName[0]     = '\0';
   Ctr->FullName[0]     = '\0';
   Ctr->WWW[0]          = '\0';
   Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = false;

   /***** Check if center code is correct *****/
   if (Ctr->CtrCod > 0)
     {
      /***** Get data of a center from database *****/
      if (Ctr_DB_GetCenterDataByCod (&mysql_res,Ctr->CtrCod)) // Center found...
        {
         /* Get center data */
         Ctr_GetCenterDataFromRow (mysql_res,Ctr,
                                   false);	// Don't get number of users who claim to belong to this center

         /* Set return value */
         CtrFound = true;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return CtrFound;
  }

/*****************************************************************************/
/********** Get data of a center from a row resulting of a query *************/
/*****************************************************************************/

static void Ctr_GetCenterDataFromRow (MYSQL_RES *mysql_res,
				      struct Ctr_Center *Ctr,
                                      bool GetNumUsrsWhoClaimToBelongToCtr)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get center code (row[0]) *****/
   if ((Ctr->CtrCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongCenterExit ();

   /***** Get institution code (row[1]) *****/
   Ctr->InsCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get place code (row[2]) *****/
   Ctr->PlcCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get center status (row[3]) *****/
   if (sscanf (row[3],"%u",&(Ctr->Status)) != 1)
      Err_WrongStatusExit ();

   /***** Get requester user's code (row[4]) *****/
   Ctr->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[4]);

   /***** Get latitude (row[5], longitude (row[6]) and altitude (row[7])*****/
   Ctr->Coord.Latitude  = Map_GetLatitudeFromStr  (row[5]);
   Ctr->Coord.Longitude = Map_GetLongitudeFromStr (row[6]);
   Ctr->Coord.Altitude  = Map_GetAltitudeFromStr  (row[7]);

   /***** Get short name (row[8]), full name (row[9])
          and URL (row[10]) of the center *****/
   Str_Copy (Ctr->ShrtName,row[ 8],sizeof (Ctr->ShrtName) - 1);
   Str_Copy (Ctr->FullName,row[ 9],sizeof (Ctr->FullName) - 1);
   Str_Copy (Ctr->WWW     ,row[10],sizeof (Ctr->WWW     ) - 1);

   /* Get number of users who claim to belong to this center (row[11]) */
   Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = false;
   if (GetNumUsrsWhoClaimToBelongToCtr)
      if (sscanf (row[11],"%u",&(Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs)) == 1)
	 Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = true;
  }

/*****************************************************************************/
/**************************** Free list of centers ***************************/
/*****************************************************************************/

void Ctr_FreeListCenters (void)
  {
   if (Gbl.Hierarchy.Ctrs.Lst)
     {
      /***** Free memory used by the list of courses in degree *****/
      free (Gbl.Hierarchy.Ctrs.Lst);
      Gbl.Hierarchy.Ctrs.Lst = NULL;
      Gbl.Hierarchy.Ctrs.Num = 0;
     }
  }

/*****************************************************************************/
/************************** Write selector of center *************************/
/*****************************************************************************/

void Ctr_WriteSelectorOfCenter (void)
  {
   extern const char *Txt_Center;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   long CtrCod;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeDeg);

      /***** Begin selector *****/
      if (Gbl.Hierarchy.Ins.InsCod > 0)
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
			   "id=\"ctr\" name=\"ctr\" class=\"HIE_SEL INPUT_%s\"",
			   The_GetSuffix ());
      else
	 HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
			   "id=\"ctr\" name=\"ctr\" class=\"HIE_SEL INPUT_%s\""
			   " disabled=\"disabled\"",
			   The_GetSuffix ());
      HTM_OPTION (HTM_Type_STRING,"",
		  Gbl.Hierarchy.Ctr.CtrCod < 0 ? HTM_OPTION_SELECTED :
					         HTM_OPTION_UNSELECTED,
		  HTM_OPTION_DISABLED,
		  "[%s]",Txt_Center);

      if (Gbl.Hierarchy.Ins.InsCod > 0)
	{
	 /***** Get centers in current institution from database *****/
	 NumCtrs = Ctr_DB_GetListOfCtrsInCurrentIns (&mysql_res);
	 for (NumCtr = 0;
	      NumCtr < NumCtrs;
	      NumCtr++)
	   {
	    /* Get next center */
	    row = mysql_fetch_row (mysql_res);

	    /* Get center code (row[0]) */
	    if ((CtrCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	       Err_WrongCenterExit ();

	    /* Write option */
	    HTM_OPTION (HTM_Type_LONG,&CtrCod,
			Gbl.Hierarchy.Ctr.CtrCod > 0 &&
			CtrCod == Gbl.Hierarchy.Ctr.CtrCod ? HTM_OPTION_SELECTED :
							     HTM_OPTION_UNSELECTED,
			HTM_OPTION_ENABLED,
			"%s",row[1]);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}

      /***** End selector *****/
      HTM_SELECT_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************************** List all centers ******************************/
/*****************************************************************************/

static void Ctr_ListCentersForEdition (const struct Plc_Places *Places)
  {
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTER_STATUS[Hie_NUM_STATUS_TXT];
   unsigned NumCtr;
   struct Ctr_Center *Ctr;
   unsigned NumPlc;
   const struct Plc_Place *PlcInLst;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct Usr_Data UsrDat;
   bool ICanEdit;
   unsigned NumDegs;
   unsigned NumUsrsCtr;
   unsigned NumUsrsInCrssOfCtr;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Write heading *****/
      Ctr_PutHeadCentersForEdition ();

      /***** Write all centers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ctrs.Num;
	   NumCtr++)
	{
	 Ctr = &Gbl.Hierarchy.Ctrs.Lst[NumCtr];

	 ICanEdit = Ctr_CheckIfICanEditACenter (Ctr);
	 NumDegs = Deg_GetNumDegsInCtr (Ctr->CtrCod);
	 NumUsrsCtr = Ctr_GetNumUsrsWhoClaimToBelongToCtr (Ctr);
	 NumUsrsInCrssOfCtr = Enr_GetNumUsrsInCrss (HieLvl_CTR,Ctr->CtrCod,
						    1 << Rol_STD |
						    1 << Rol_NET |
						    1 << Rol_TCH);	// Any user

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove center */
	    HTM_TD_Begin ("class=\"BM\"");
	       if (!ICanEdit ||				// I cannot edit
		   NumDegs ||				// Center has degrees
		   NumUsrsCtr ||			// Center has users who claim to belong to it
		   NumUsrsInCrssOfCtr)			// Center has users
		  Ico_PutIconRemovalNotAllowed ();
	       else	// I can remove center
		  Ico_PutContextualIconToRemove (ActRemCtr,NULL,
						 Hie_PutParOtherHieCod,&Ctr->CtrCod);
	    HTM_TD_End ();

	    /* Center code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Ctr->CtrCod);
	    HTM_TD_End ();

	    /* Center logo */
	    HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr->FullName);
	       Lgo_DrawLogo (HieLvl_CTR,Ctr->CtrCod,Ctr->ShrtName,20,NULL);
	    HTM_TD_End ();

	    /* Place */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       if (ICanEdit)
		 {
		  Frm_BeginForm (ActChgCtrPlc);
		     ParCod_PutPar (ParCod_OthHie,Ctr->CtrCod);
		     HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
				       "name=\"PlcCod\""
				       " class=\"PLC_SEL INPUT_%s\"",
				       The_GetSuffix ());
			HTM_OPTION (HTM_Type_STRING,"0",
				    Ctr->PlcCod == 0 ? HTM_OPTION_SELECTED :
						       HTM_OPTION_UNSELECTED,
				    HTM_OPTION_ENABLED,
				    "%s",Txt_Another_place);
			for (NumPlc = 0;
			     NumPlc < Places->Num;
			     NumPlc++)
			  {
			   PlcInLst = &Places->Lst[NumPlc];
			   HTM_OPTION (HTM_Type_LONG,&PlcInLst->PlcCod,
				       PlcInLst->PlcCod == Ctr->PlcCod ? HTM_OPTION_SELECTED :
									 HTM_OPTION_UNSELECTED,
				       HTM_OPTION_ENABLED,
				       "%s",PlcInLst->ShrtName);
			  }
		     HTM_SELECT_End ();
		  Frm_EndForm ();
		 }
	       else
		  for (NumPlc = 0;
		       NumPlc < Places->Num;
		       NumPlc++)
		     if (Places->Lst[NumPlc].PlcCod == Ctr->PlcCod)
			HTM_Txt (Places->Lst[NumPlc].ShrtName);
	    HTM_TD_End ();

	    /* Center short name */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       if (ICanEdit)
		 {
		  Frm_BeginForm (ActRenCtrSho);
		     ParCod_PutPar (ParCod_OthHie,Ctr->CtrCod);
		     HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ctr->ShrtName,
				     HTM_SUBMIT_ON_CHANGE,
				     "class=\"INPUT_SHORT_NAME INPUT_%s\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
		 }
	       else
		  HTM_Txt (Ctr->ShrtName);
	    HTM_TD_End ();

	    /* Center full name */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       if (ICanEdit)
		 {
		  Frm_BeginForm (ActRenCtrFul);
		     ParCod_PutPar (ParCod_OthHie,Ctr->CtrCod);
		     HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ctr->FullName,
				     HTM_SUBMIT_ON_CHANGE,
				     "class=\"INPUT_FULL_NAME INPUT_%s\"",
				     The_GetSuffix ());
		  Frm_EndForm ();
		 }
	       else
		  HTM_Txt (Ctr->FullName);
	    HTM_TD_End ();

	    /* Center WWW */
	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       if (ICanEdit)
		 {
		  Frm_BeginForm (ActChgCtrWWW);
		     ParCod_PutPar (ParCod_OthHie,Ctr->CtrCod);
		     HTM_INPUT_URL ("WWW",Ctr->WWW,HTM_SUBMIT_ON_CHANGE,
				    "class=\"INPUT_WWW_NARROW INPUT_%s\""
				    " required=\"required\"",
				    The_GetSuffix ());
		  Frm_EndForm ();
		 }
	       else
		 {
		  Str_Copy (WWW,Ctr->WWW,sizeof (WWW) - 1);
		  HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHORT\"");
		     HTM_A_Begin ("href=\"%s\" target=\"_blank\""
				  " class=\"DAT_%s\" title=\"%s\"",
				  Ctr->WWW,
				  The_GetSuffix (),
				  Ctr->WWW);
			HTM_Txt (WWW);
		     HTM_A_End ();
		  HTM_DIV_End ();
		 }
	    HTM_TD_End ();

	    /* Number of users who claim to belong to this center */
	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (NumUsrsCtr);
	    HTM_TD_End ();

	    /* Number of degrees */
	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (NumDegs);
	    HTM_TD_End ();

	    /* Number of users in courses of this center */
	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (NumUsrsInCrssOfCtr);
	    HTM_TD_End ();

	    /* Center requester */
	    UsrDat.UsrCod = Ctr->RequesterUsrCod;
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
						     Usr_DONT_GET_PREFS,
						     Usr_DONT_GET_ROLE_IN_CURRENT_CRS);
	    HTM_TD_Begin ("class=\"DAT_%s INPUT_REQUESTER LT\"",
	                  The_GetSuffix ());
	       Usr_WriteAuthor (&UsrDat,Cns_ENABLED);
	    HTM_TD_End ();

	    /* Center status */
	    Hie_WriteStatusCellEditable (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM,
	                                 Ctr->Status,ActChgCtrSta,Ctr->CtrCod,
                                         Txt_CENTER_STATUS);

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a center *******************/
/*****************************************************************************/

static bool Ctr_CheckIfICanEditACenter (struct Ctr_Center *Ctr)
  {
   return  Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM ||		// I am an institution administrator or higher
           ((Ctr->Status & Hie_STATUS_BIT_PENDING) != 0 &&	// Center is not yet activated
           Gbl.Usrs.Me.UsrDat.UsrCod == Ctr->RequesterUsrCod);	// I am the requester
  }

/*****************************************************************************/
/******************************* Remove a center *****************************/
/*****************************************************************************/

void Ctr_RemoveCenter (void)
  {
   extern const char *Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center;
   extern const char *Txt_Center_X_removed;
   char PathCtr[PATH_MAX + 1];

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get center code *****/
   Ctr_EditingCtr->CtrCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get data of the center from database *****/
   Ctr_GetCenterDataByCod (Ctr_EditingCtr);

   /***** Check if this center has teachers *****/
   if (Deg_GetNumDegsInCtr (Ctr_EditingCtr->CtrCod))			// Center has degrees
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else if (Ctr_GetNumUsrsWhoClaimToBelongToCtr (Ctr_EditingCtr))	// Center has users who claim to belong to it
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else if (Enr_GetNumUsrsInCrss (HieLvl_CTR,Ctr_EditingCtr->CtrCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))			// Center has users
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else	// Center has no degrees or users ==> remove it
     {
      /***** Remove all threads and posts in forums of the center *****/
      For_DB_RemoveForums (HieLvl_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove surveys of the center *****/
      Svy_RemoveSurveys (HieLvl_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove information related to files in center *****/
      Brw_DB_RemoveCtrFiles (Ctr_EditingCtr->CtrCod);

      /***** Remove all rooms in center *****/
      Roo_DB_RemoveAllRoomsInCtr (Ctr_EditingCtr->CtrCod);

      /***** Remove directories of the center *****/
      snprintf (PathCtr,sizeof (PathCtr),"%s/%02u/%u",
	        Cfg_PATH_CTR_PUBLIC,
	        (unsigned) (Ctr_EditingCtr->CtrCod % 100),
	        (unsigned)  Ctr_EditingCtr->CtrCod);
      Fil_RemoveTree (PathCtr);

      /***** Remove administrators of this center *****/
      Adm_DB_RemAdmins (HieLvl_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove center *****/
      Ctr_DB_RemoveCenter (Ctr_EditingCtr->CtrCod);

      /***** Flush caches *****/
      Deg_FlushCacheNumDegsInCtr ();
      Crs_FlushCacheNumCrssInCtr ();
      Ctr_FlushCacheNumUsrsWhoClaimToBelongToCtr ();

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Center_X_removed,
	               Ctr_EditingCtr->FullName);

      Ctr_EditingCtr->CtrCod = -1L;	// To not showing button to go to center
     }
  }

/*****************************************************************************/
/************************ Change the place of a center ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrPlc (void)
  {
   extern const char *Txt_The_place_of_the_center_has_changed;
   long NewPlcCod;

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get center code *****/
   Ctr_EditingCtr->CtrCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get parameter with place code *****/
   NewPlcCod = ParCod_GetAndCheckParMin (ParCod_Plc,0);	// 0 (another place) is allowed here

   /***** Get data of center from database *****/
   Ctr_GetCenterDataByCod (Ctr_EditingCtr);

   /***** Update place in table of centers *****/
   Ctr_DB_UpdateCtrPlc (Ctr_EditingCtr->CtrCod,NewPlcCod);
   Ctr_EditingCtr->PlcCod = NewPlcCod;

   /***** Create alert to show the change made
	  and put button to go to center changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_place_of_the_center_has_changed);
  }

/*****************************************************************************/
/************************ Change the name of a center ************************/
/*****************************************************************************/

void Ctr_RenameCenterShort (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Rename center *****/
   Ctr_EditingCtr->CtrCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ctr_RenameCenter (Ctr_EditingCtr,Cns_SHRT_NAME);
  }

void Ctr_RenameCenterFull (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Rename center *****/
   Ctr_EditingCtr->CtrCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ctr_RenameCenter (Ctr_EditingCtr,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a center ************************/
/*****************************************************************************/

void Ctr_RenameCenter (struct Ctr_Center *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_center_X_already_exists;
   extern const char *Txt_The_center_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   const char *ParName = NULL;	// Initialized to avoid warning
   const char *FldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;	// Initialized to avoid warning
   char *CurrentCtrName = NULL;	// Initialized to avoid warning
   char NewCtrName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParName = "ShortName";
         FldName = "ShortName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_SHRT_NAME;
         CurrentCtrName = Ctr->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParName = "FullName";
         FldName = "FullName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_FULL_NAME;
         CurrentCtrName = Ctr->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the new name for the center */
   Par_GetParText (ParName,NewCtrName,MaxBytes);

   /***** Get from the database the old names of the center *****/
   Ctr_GetCenterDataByCod (Ctr);

   /***** Check if new name is empty *****/
   if (!NewCtrName[0])
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentCtrName,NewCtrName))	// Different names
        {
         /***** If degree was in database... *****/
         if (Ctr_DB_CheckIfCtrNameExistsInIns (ParName,NewCtrName,Ctr->CtrCod,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_center_X_already_exists,
			     NewCtrName);
         else
           {
            /* Update the table changing old name by new name */
            Ctr_DB_UpdateCtrName (Ctr->CtrCod,FldName,NewCtrName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_center_X_has_been_renamed_as_Y,
                             CurrentCtrName,NewCtrName);

	    /* Change current center name in order to display it properly */
	    Str_Copy (CurrentCtrName,NewCtrName,MaxBytes);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,CurrentCtrName);
     }
  }

/*****************************************************************************/
/************************* Change the URL of a center ************************/
/*****************************************************************************/

void Ctr_ChangeCtrWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get the code of the center *****/
   Ctr_EditingCtr->CtrCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get the new WWW for the center *****/
   Par_GetParText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of center *****/
   Ctr_GetCenterDataByCod (Ctr_EditingCtr);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_DB_UpdateCtrWWW (Ctr_EditingCtr->CtrCod,NewWWW);
      Str_Copy (Ctr_EditingCtr->WWW,NewWWW,sizeof (Ctr_EditingCtr->WWW) - 1);

      /***** Write message to show the change made
	     and put button to go to center changed *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
		       NewWWW);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/*********************** Change the status of a center ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrStatus (void)
  {
   extern const char *Txt_The_status_of_the_center_X_has_changed;
   Hie_Status_t Status;

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get parameters from form *****/
   /* Get center code */
   Ctr_EditingCtr->CtrCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get parameter with status */
   Status = Hie_GetParStatus ();	// New status

   /***** Get data of center *****/
   Ctr_GetCenterDataByCod (Ctr_EditingCtr);

   /***** Update status *****/
   Ctr_DB_UpdateCtrStatus (Ctr_EditingCtr->CtrCod,Status);
   Ctr_EditingCtr->Status = Status;

   /***** Write message to show the change made
	  and put button to go to center changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_status_of_the_center_X_has_changed,
	            Ctr_EditingCtr->ShrtName);
  }

/*****************************************************************************/
/********* Show alerts after changing a center and continue editing **********/
/*****************************************************************************/

void Ctr_ContEditAfterChgCtr (void)
  {
   /***** Write message to show the change made
	  and put button to go to center changed *****/
   Ctr_ShowAlertAndButtonToGoToCtr ();

   /***** Show the form again *****/
   Ctr_EditCentersInternal ();

   /***** Center destructor *****/
   Ctr_EditingCenterDestructor ();
  }

/*****************************************************************************/
/***************** Write message to show the change made  ********************/
/***************** and put button to go to center changed ********************/
/*****************************************************************************/

static void Ctr_ShowAlertAndButtonToGoToCtr (void)
  {
   // If the center being edited is different to the current one...
   if (Ctr_EditingCtr->CtrCod != Gbl.Hierarchy.Ctr.CtrCod)
     {
      /***** Alert with button to go to center *****/
      Ale_ShowLastAlertAndButton (ActSeeDeg,NULL,NULL,
                                  Ctr_PutParCtrCod,&Ctr_EditingCtr->CtrCod,
                                  Btn_CONFIRM_BUTTON,
                                  Str_BuildGoToTitle (Ctr_EditingCtr->ShrtName));
      Str_FreeGoToTitle ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

/*****************************************************************************/
/********************* Put a form to create a new center *********************/
/*****************************************************************************/

static void Ctr_PutFormToCreateCenter (const struct Plc_Places *Places)
  {
   extern const char *Txt_Another_place;
   Act_Action_t NextAction = ActUnk;
   unsigned NumPlc;
   const struct Plc_Place *PlcInLst;

   /***** Set action depending on role *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      NextAction = ActNewCtr;
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      NextAction = ActReqCtr;
   else
      Err_NoPermissionExit ();

   /***** Begin form to create *****/
   Frm_BeginFormTable (NextAction,NULL,NULL,NULL);

      /***** Write heading *****/
      Ctr_PutHeadCentersForEdition ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove center, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Center code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Center logo *****/
	 HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr_EditingCtr->FullName);
	    Lgo_DrawLogo (HieLvl_CTR,-1L,"",20,NULL);
	 HTM_TD_End ();

	 /***** Place *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
			      "name=\"PlcCod\" class=\"PLC_SEL INPUT_%s\"",
			      The_GetSuffix ());
	       HTM_OPTION (HTM_Type_STRING,"0",
			   Ctr_EditingCtr->PlcCod == 0 ? HTM_OPTION_SELECTED :
							 HTM_OPTION_UNSELECTED,
			   HTM_OPTION_ENABLED,
			   "%s",Txt_Another_place);
	       for (NumPlc = 0;
		    NumPlc < Places->Num;
		    NumPlc++)
		 {
		  PlcInLst = &Places->Lst[NumPlc];
		  HTM_OPTION (HTM_Type_LONG,&PlcInLst->PlcCod,
			      PlcInLst->PlcCod == Ctr_EditingCtr->PlcCod ? HTM_OPTION_SELECTED :
									   HTM_OPTION_UNSELECTED,
			      HTM_OPTION_ENABLED,
			      "%s",PlcInLst->ShrtName);
		 }
	    HTM_SELECT_End ();
	 HTM_TD_End ();

	 /***** Center short name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ctr_EditingCtr->ShrtName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_SHORT_NAME INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Center full name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ctr_EditingCtr->FullName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_FULL_NAME INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Center WWW *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("WWW",Ctr_EditingCtr->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
			   "class=\"INPUT_WWW_NARROW INPUT_%s\""
			   " required=\"required\"",
			   The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Number of users who claim to belong to this center *****/
	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Number of degrees *****/
	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Number of users in courses of this center *****/
	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

	 /***** Center requester *****/
	 HTM_TD_Begin ("class=\"DAT_%s INPUT_REQUESTER LT\"",
		       The_GetSuffix ());
	    Usr_WriteAuthor (&Gbl.Usrs.Me.UsrDat,Cns_ENABLED);
	 HTM_TD_End ();

	 /***** Center status *****/
	 HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Ctr_PutHeadCentersForSeeing (bool OrderSelectable)
  {
   extern const char *Txt_CENTERS_HELP_ORDER[2];
   extern const char *Txt_CENTERS_ORDER[2];
   extern const char *Txt_Place;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   Ctr_Order_t Order;
   static HTM_HeadAlign Align[Ctr_NUM_ORDERS] =
     {
      [Ctr_ORDER_BY_CENTER  ] = HTM_HEAD_LEFT,
      [Ctr_ORDER_BY_NUM_USRS] = HTM_HEAD_RIGHT
     };

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);

      for (Order  = (Ctr_Order_t) 0;
	   Order <= (Ctr_Order_t) (Ctr_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (Align[Order]);
	    if (OrderSelectable)
	      {
	       Frm_BeginForm (ActSeeCtr);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_CENTERS_HELP_ORDER[Order],
					   "class=\"BT_LINK\"");
		     if (Order == Gbl.Hierarchy.Ctrs.SelectedOrder)
			HTM_U_Begin ();
	      }
	    HTM_Txt (Txt_CENTERS_ORDER[Order]);
	    if (OrderSelectable)
	      {
		     if (Order == Gbl.Hierarchy.Ctrs.SelectedOrder)
			HTM_U_End ();
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	 HTM_TH_End ();
	}

      HTM_TH (Txt_Place               ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Degrees_ABBREVIATION,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Courses_ABBREVIATION,HTM_HEAD_RIGHT);
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Ctr_PutHeadCentersForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Place;
   extern const char *Txt_Short_name_of_the_center;
   extern const char *Txt_Full_name_of_the_center;
   extern const char *Txt_WWW;
   extern const char *Txt_Users;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);
      HTM_TH (Txt_Code                    ,HTM_HEAD_RIGHT);
      HTM_TH_Empty (1);
      HTM_TH (Txt_Place                   ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Short_name_of_the_center,HTM_HEAD_LEFT );
      HTM_TH (Txt_Full_name_of_the_center ,HTM_HEAD_LEFT );
      HTM_TH (Txt_WWW                     ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Users                   ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Degrees_ABBREVIATION    ,HTM_HEAD_RIGHT);
      HTM_TH_Begin (HTM_HEAD_RIGHT);
	 HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
	 HTM_BR ();
	 HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
      HTM_TH_End ();
      HTM_TH (Txt_Requester               ,HTM_HEAD_LEFT );
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Receive form to request a new center *********************/
/*****************************************************************************/

void Ctr_ReceiveFormReqCtr (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Receive form to request a new center *****/
   Ctr_ReceiveFormRequestOrCreateCtr ((Hie_Status_t) Hie_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new center *********************/
/*****************************************************************************/

void Ctr_ReceiveFormNewCtr (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Receive form to create a new center *****/
   Ctr_ReceiveFormRequestOrCreateCtr ((Hie_Status_t) 0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new center ****************/
/*****************************************************************************/

static void Ctr_ReceiveFormRequestOrCreateCtr (Hie_Status_t Status)
  {
   extern const char *Txt_The_center_X_already_exists;
   extern const char *Txt_Created_new_center_X;

   /***** Get parameters from form *****/
   /* Set center institution */
   Ctr_EditingCtr->InsCod = Gbl.Hierarchy.Ins.InsCod;

   /* Get place */
   Ctr_EditingCtr->PlcCod = ParCod_GetAndCheckParMin (ParCod_Plc,0);	// 0 (another place) is allowed here

   /* Get center short name and full name */
   Par_GetParText ("ShortName",Ctr_EditingCtr->ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);
   Par_GetParText ("FullName" ,Ctr_EditingCtr->FullName,Cns_HIERARCHY_MAX_BYTES_FULL_NAME);

   /* Get center WWW */
   Par_GetParText ("WWW",Ctr_EditingCtr->WWW,Cns_MAX_BYTES_WWW);

   if (Ctr_EditingCtr->ShrtName[0] &&
       Ctr_EditingCtr->FullName[0])	// If there's a center name
     {
      if (Ctr_EditingCtr->WWW[0])
        {
         /***** If name of center was in database... *****/
         if (Ctr_DB_CheckIfCtrNameExistsInIns ("ShortName",Ctr_EditingCtr->ShrtName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_center_X_already_exists,
                             Ctr_EditingCtr->ShrtName);
         else if (Ctr_DB_CheckIfCtrNameExistsInIns ("FullName",Ctr_EditingCtr->FullName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        		     Txt_The_center_X_already_exists,
                             Ctr_EditingCtr->FullName);
         else	// Add new center to database
           {
            Ctr_EditingCtr->CtrCod = Ctr_DB_CreateCenter (Ctr_EditingCtr,Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_center_X,
			     Ctr_EditingCtr->FullName);
           }
        }
      else	// If there is not a web
         Ale_CreateAlertYouMustSpecifyTheWebAddress ();
     }
   else	// If there is not a center name
      Ale_CreateAlertYouMustSpecifyTheShortNameAndTheFullName ();
  }

/*****************************************************************************/
/************************** Get number of centers ****************************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsInSys (void)
  {
   unsigned NumCtrs;

   /***** Get number of centers from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS,HieLvl_SYS,-1L,
				   FigCch_UNSIGNED,&NumCtrs))
     {
      /***** Get current number of centers from database and update cache *****/
      NumCtrs = (unsigned) DB_GetNumRowsTable ("ctr_centers");
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS,HieLvl_SYS,-1L,
				    FigCch_UNSIGNED,&NumCtrs);
     }

   return NumCtrs;
  }

/*****************************************************************************/
/******************* Get number of centers in a country **********************/
/*****************************************************************************/

void Ctr_FlushCacheNumCtrsInCty (void)
  {
   Gbl.Cache.NumCtrsInCty.CtyCod  = -1L;
   Gbl.Cache.NumCtrsInCty.NumCtrs = 0;
  }

static unsigned Ctr_GetNumCtrsInCty (long CtyCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (CtyCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (CtyCod == Gbl.Cache.NumCtrsInCty.CtyCod)
      return Gbl.Cache.NumCtrsInCty.NumCtrs;

   /***** 3. Slow: number of centers in a country from database *****/
   Gbl.Cache.NumCtrsInCty.CtyCod  = CtyCod;
   Gbl.Cache.NumCtrsInCty.NumCtrs = Ctr_DB_GetNumCtrsInCty (CtyCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS,HieLvl_CTY,Gbl.Cache.NumCtrsInCty.CtyCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumCtrsInCty.NumCtrs);
   return Gbl.Cache.NumCtrsInCty.NumCtrs;
  }

unsigned Ctr_GetCachedNumCtrsInCty (long CtyCod)
  {
   unsigned NumCtrs;

   /***** Get number of centers from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS,HieLvl_CTY,CtyCod,
				   FigCch_UNSIGNED,&NumCtrs))
      /***** Get current number of centers from database and update cache *****/
      NumCtrs = Ctr_GetNumCtrsInCty (CtyCod);

   return NumCtrs;
  }

/*****************************************************************************/
/**************** Get number of centers in an institution ********************/
/*****************************************************************************/

void Ctr_FlushCacheNumCtrsInIns (void)
  {
   Gbl.Cache.NumCtrsInIns.InsCod  = -1L;
   Gbl.Cache.NumCtrsInIns.NumCtrs = 0;
  }

unsigned Ctr_GetNumCtrsInIns (long InsCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (InsCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (InsCod == Gbl.Cache.NumCtrsInIns.InsCod)
      return Gbl.Cache.NumCtrsInIns.NumCtrs;

   /***** 3. Slow: number of centers in an institution from database *****/
   Gbl.Cache.NumCtrsInIns.InsCod  = InsCod;
   Gbl.Cache.NumCtrsInIns.NumCtrs = Ctr_DB_GetNumCtrsInIns (InsCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS,HieLvl_INS,Gbl.Cache.NumCtrsInIns.InsCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumCtrsInIns.NumCtrs);
   return Gbl.Cache.NumCtrsInIns.NumCtrs;
  }

unsigned Ctr_GetCachedNumCtrsInIns (long InsCod)
  {
   unsigned NumCtrs;

   /***** Get number of centers from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS,HieLvl_INS,InsCod,
				   FigCch_UNSIGNED,&NumCtrs))
      /***** Get current number of centers from database and update cache *****/
      NumCtrs = Ctr_GetNumCtrsInIns (InsCod);

   return NumCtrs;
  }

/*****************************************************************************/
/********************** Get number of centers with map ***********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithMapInSys (void)
  {
   unsigned NumCtrsWithMap;

   /***** Get number of centers with map from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_MAP,HieLvl_SYS,-1L,
                                   FigCch_UNSIGNED,&NumCtrsWithMap))
     {
      /***** Get current number of centers with map from database and update cache *****/
      /* Ccoordinates 0, 0 means not set ==> don't show map */
      NumCtrsWithMap = (unsigned) Ctr_DB_GetNumCtrsWithMap ();
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_MAP,HieLvl_SYS,-1L,
                                    FigCch_UNSIGNED,&NumCtrsWithMap);
     }

   return NumCtrsWithMap;
  }

/*****************************************************************************/
/************** Get number of centers with map in a country ******************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithMapInCty (long CtyCod)
  {
   unsigned NumCtrsWithMap;

   /***** Get number of centers with map from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_MAP,HieLvl_CTY,CtyCod,
                                   FigCch_UNSIGNED,&NumCtrsWithMap))
     {
      /***** Get current number of centers with map from database and update cache *****/
      /* Ccoordinates 0, 0 means not set ==> don't show map */
      NumCtrsWithMap = Ctr_DB_GetNumCtrsWithMapInCty (CtyCod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_MAP,HieLvl_CTY,CtyCod,
                                    FigCch_UNSIGNED,&NumCtrsWithMap);
     }

   return NumCtrsWithMap;
  }

/*****************************************************************************/
/************* Get number of centers with map in an institution **************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithMapInIns (long InsCod)
  {
   unsigned NumCtrsWithMap;

   /***** Get number of centers with map from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_MAP,HieLvl_INS,InsCod,
                                   FigCch_UNSIGNED,&NumCtrsWithMap))
     {
      /***** Get current number of centers with map from database and update cache *****/
      /* Ccoordinates 0, 0 means not set ==> don't show map */
      NumCtrsWithMap = Ctr_DB_GetNumCtrsWithMapInIns (InsCod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_MAP,HieLvl_INS,InsCod,
                                    FigCch_UNSIGNED,&NumCtrsWithMap);
     }

   return NumCtrsWithMap;
  }

/*****************************************************************************/
/********************* Get number of centers with degrees ********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithDegs (void)
  {
   unsigned NumCtrsWithDegs;
   long Cod = Hie_GetCurrentCod ();

   /***** Get number of centers with degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_DEGS,Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumCtrsWithDegs))
     {
      /***** Get current number of centers with degrees from database and update cache *****/
      NumCtrsWithDegs = Ctr_DB_GetNumCtrsWithDegs (Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_DEGS,Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumCtrsWithDegs);
     }

   return NumCtrsWithDegs;
  }

/*****************************************************************************/
/********************* Get number of centers with courses ********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithCrss (void)
  {
   unsigned NumCtrsWithCrss;
   long Cod = Hie_GetCurrentCod ();

   /***** Get number of centers with courses *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_CRSS,Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumCtrsWithCrss))
     {
      /***** Get number of centers with courses *****/
      NumCtrsWithCrss = Ctr_DB_GetNumCtrsWithCrss (Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_CRSS,Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumCtrsWithCrss);
     }

   return NumCtrsWithCrss;
  }

/*****************************************************************************/
/********************* Get number of centers with users **********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithUsrs (Rol_Role_t Role)
  {
   static const FigCch_FigureCached_t FigureCtrs[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_CTRS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_CTRS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_CTRS_WITH_TCHS,	// Teachers
     };
   unsigned NumCtrsWithUsrs;
   long Cod = Hie_GetCurrentCod ();

   /***** Get number of centers with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureCtrs[Role],Gbl.Scope.Current,Cod,
				   FigCch_UNSIGNED,&NumCtrsWithUsrs))
     {
      /***** Get current number of centers with users from database and update cache *****/
      NumCtrsWithUsrs = Ctr_DB_GetNumCtrsWithUsrs (Role,Gbl.Scope.Current,Cod);
      FigCch_UpdateFigureIntoCache (FigureCtrs[Role],Gbl.Scope.Current,Cod,
				    FigCch_UNSIGNED,&NumCtrsWithUsrs);
     }

   return NumCtrsWithUsrs;
  }

/*****************************************************************************/
/****************************** List centers found ***************************/
/*****************************************************************************/

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs)
  {
   extern const char *Txt_center;
   extern const char *Txt_centers;
   unsigned NumCtr;
   char *Title;
   struct Ctr_Center Ctr;

   /***** Query database *****/
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      /* Number of centers found */
      if (asprintf (&Title,"%u %s",NumCtrs,
				   NumCtrs == 1 ? Txt_center :
	                                          Txt_centers) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (NULL,Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

	 /***** Write heading *****/
	 Ctr_PutHeadCentersForSeeing (false);	// Order not selectable

	 /***** List the centers (one row per center) *****/
	 for (NumCtr  = 1, The_ResetRowColor ();
	      NumCtr <= NumCtrs;
	      NumCtr++, The_ChangeRowColor ())
	   {
	    /* Get next center */
	    Ctr.CtrCod = DB_GetNextCode (*mysql_res);

	    /* Get data of center */
	    Ctr_GetCenterDataByCod (&Ctr);

	    /* Write data of this center */
	    Ctr_ListOneCenterForSeeing (&Ctr,NumCtr);
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Center constructor/destructor **********************/
/*****************************************************************************/

static void Ctr_EditingCenterConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Ctr_EditingCtr != NULL)
      Err_WrongCenterExit ();

   /***** Allocate memory for center *****/
   if ((Ctr_EditingCtr = malloc (sizeof (*Ctr_EditingCtr))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset center *****/
   Ctr_EditingCtr->CtrCod          = -1L;
   Ctr_EditingCtr->InsCod          = -1L;
   Ctr_EditingCtr->PlcCod          = -1L;
   Ctr_EditingCtr->Status          = (Hie_Status_t) 0;
   Ctr_EditingCtr->RequesterUsrCod = -1L;
   Ctr_EditingCtr->ShrtName[0]     = '\0';
   Ctr_EditingCtr->FullName[0]     = '\0';
   Ctr_EditingCtr->WWW[0]          = '\0';
  }

static void Ctr_EditingCenterDestructor (void)
  {
   /***** Free memory used for center *****/
   if (Ctr_EditingCtr != NULL)
     {
      free (Ctr_EditingCtr);
      Ctr_EditingCtr = NULL;
     }
  }

/*****************************************************************************/
/************************ Form to go to center map ***************************/
/*****************************************************************************/

static void Ctr_FormToGoToMap (struct Ctr_Center *Ctr)
  {
   if (Ctr_GetIfMapIsAvailable (Ctr))
     {
      Ctr_EditingCtr = Ctr;	// Used to pass parameter with the code of the center
      Lay_PutContextualLinkOnlyIcon (ActSeeCtrInf,NULL,
                                     Ctr_PutParCtrCod,&Ctr_EditingCtr->CtrCod,
				     "map-marker-alt.svg",Ico_BLACK);
     }
  }

/*****************************************************************************/
/************************ Check if a center has map **************************/
/*****************************************************************************/

bool Ctr_GetIfMapIsAvailable (const struct Ctr_Center *Ctr)
  {
   /***** Coordinates 0, 0 means not set ==> don't show map *****/
   return Ctr->Coord.Latitude ||
          Ctr->Coord.Longitude;
  }

/*****************************************************************************/
/***** Get all my centers (those of my courses) and store them in a list *****/
/*****************************************************************************/

void Ctr_GetMyCenters (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtr;
   unsigned NumCtrs;
   long CtrCod;

   /***** If my centers are yet filled, there's nothing to do *****/
   if (!Gbl.Usrs.Me.MyCtrs.Filled)
     {
      Gbl.Usrs.Me.MyCtrs.Num = 0;

      /***** Get my centers from database *****/
      NumCtrs = Ctr_DB_GetCtrsFromUsr (&mysql_res,
                                       Gbl.Usrs.Me.UsrDat.UsrCod,-1L);
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
	{
	 /* Get next center */
	 row = mysql_fetch_row (mysql_res);

	 /* Get center code */
	 if ((CtrCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
	   {
	    if (Gbl.Usrs.Me.MyCtrs.Num == Ctr_MAX_CENTERS_PER_USR)
	       Err_ShowErrorAndExit ("Maximum number of centers of a user exceeded.");

	    Gbl.Usrs.Me.MyCtrs.Ctrs[Gbl.Usrs.Me.MyCtrs.Num].CtrCod = CtrCod;
	    Gbl.Usrs.Me.MyCtrs.Ctrs[Gbl.Usrs.Me.MyCtrs.Num].MaxRole = Rol_ConvertUnsignedStrToRole (row[1]);

	    Gbl.Usrs.Me.MyCtrs.Num++;
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

      /***** Set boolean that indicates that my centers are yet filled *****/
      Gbl.Usrs.Me.MyCtrs.Filled = true;
     }
  }

/*****************************************************************************/
/************************ Free the list of my centers ************************/
/*****************************************************************************/

void Ctr_FreeMyCenters (void)
  {
   if (Gbl.Usrs.Me.MyCtrs.Filled)
     {
      /***** Reset list *****/
      Gbl.Usrs.Me.MyCtrs.Filled = false;
      Gbl.Usrs.Me.MyCtrs.Num    = 0;
     }
  }

/*****************************************************************************/
/*********************** Check if I belong to a center ***********************/
/*****************************************************************************/

bool Ctr_CheckIfIBelongToCtr (long CtrCod)
  {
   unsigned NumMyCtr;

   /***** Fill the list with the centers I belong to *****/
   Ctr_GetMyCenters ();

   /***** Check if the center passed as parameter is any of my centers *****/
   for (NumMyCtr = 0;
        NumMyCtr < Gbl.Usrs.Me.MyCtrs.Num;
        NumMyCtr++)
      if (Gbl.Usrs.Me.MyCtrs.Ctrs[NumMyCtr].CtrCod == CtrCod)
         return true;
   return false;
  }

/*****************************************************************************/
/******************* Check if a user belongs to a center *********************/
/*****************************************************************************/

void Ctr_FlushCacheUsrBelongsToCtr (void)
  {
   Gbl.Cache.UsrBelongsToCtr.UsrCod = -1L;
   Gbl.Cache.UsrBelongsToCtr.CtrCod = -1L;
   Gbl.Cache.UsrBelongsToCtr.Belongs = false;
  }

bool Ctr_CheckIfUsrBelongsToCtr (long UsrCod,long CtrCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (UsrCod <= 0 ||
       CtrCod <= 0)
      return false;

   /***** 2. Fast check: If cached... *****/
   if (UsrCod == Gbl.Cache.UsrBelongsToCtr.UsrCod &&
       CtrCod == Gbl.Cache.UsrBelongsToCtr.CtrCod)
      return Gbl.Cache.UsrBelongsToCtr.Belongs;

   /***** 3. Slow check: Get is user belongs to center from database *****/
   Gbl.Cache.UsrBelongsToCtr.UsrCod = UsrCod;
   Gbl.Cache.UsrBelongsToCtr.CtrCod = CtrCod;
   Gbl.Cache.UsrBelongsToCtr.Belongs = Ctr_DB_CheckIfUsrBelongsToCtr (UsrCod,CtrCod);
   return Gbl.Cache.UsrBelongsToCtr.Belongs;
  }

/*****************************************************************************/
/*********** Get number of users who claim to belong to a center *************/
/*****************************************************************************/

void Ctr_FlushCacheNumUsrsWhoClaimToBelongToCtr (void)
  {
   Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.CtrCod  = -1L;
   Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.NumUsrs = 0;
  }

unsigned Ctr_GetNumUsrsWhoClaimToBelongToCtr (struct Ctr_Center *Ctr)
  {
   /***** 1. Fast check: Trivial case *****/
   if (Ctr->CtrCod <= 0)
      return 0;

   /***** 2. Fast check: If already got... *****/
   if (Ctr->NumUsrsWhoClaimToBelongToCtr.Valid)
      return Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs;

   /***** 3. Fast check: If cached... *****/
   if (Ctr->CtrCod == Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.CtrCod)
     {
      Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs = Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.NumUsrs;
      Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = true;
      return Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs;
     }

   /***** 4. Slow: number of users who claim to belong to a center
                   from database *****/
   Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.CtrCod  = Ctr->CtrCod;
   Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.NumUsrs =
   Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs = Ctr_DB_GetNumUsrsWhoClaimToBelongToCtr (Ctr->CtrCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_USRS_BELONG_CTR,HieLvl_CTR,Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.CtrCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumUsrsWhoClaimToBelongToCtr.NumUsrs);
   return Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs;
  }

unsigned Ctr_GetCachedNumUsrsWhoClaimToBelongToCtr (struct Ctr_Center *Ctr)
  {
   unsigned NumUsrsCtr;

   /***** Get number of users who claim to belong to center from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_USRS_BELONG_CTR,HieLvl_CTR,Ctr->CtrCod,
                                   FigCch_UNSIGNED,&NumUsrsCtr))
      /***** Get current number of users who claim to belong to center from database and update cache *****/
      NumUsrsCtr = Ctr_GetNumUsrsWhoClaimToBelongToCtr (Ctr);

   return NumUsrsCtr;
  }

/*****************************************************************************/
/******************** Write parameter with code of center ********************/
/*****************************************************************************/

static void Ctr_PutParCtrCod (void *CtrCod)
  {
   if (CtrCod)
      ParCod_PutPar (ParCod_Ctr,*((long *) CtrCod));
  }
