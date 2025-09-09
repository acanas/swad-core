// swad_center.c: centers

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

#define _GNU_SOURCE 		// For asprintf
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
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_place.h"
#include "swad_room_database.h"
#include "swad_survey.h"
#include "swad_www.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Hie_Node *Ctr_EditingCtr = NULL;	// Static variable to keep the center being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ctr_ListCenters (void);
static void Ctr_PutIconsListingCenters (__attribute__((unused)) void *Args);
static void Ctr_PutIconToEditCenters (void);
static void Ctr_ListOneCenterForSeeing (struct Hie_Node *Ctr,unsigned NumCtr);

static void Ctr_EditCentersInternal (void);
static void Ctr_PutIconsEditingCenters (__attribute__((unused)) void *Args);

static void Ctr_GetCenterDataFromRow (MYSQL_RES *mysql_res,
				      struct Hie_Node *Ctr,
                                      bool GetNumUsrsWhoClaimToBelongToCtr);
static void Ctr_GetCoordFromRow (MYSQL_RES *mysql_res,
				 struct Map_Coordinates *Coord);

static void Ctr_ListCentersForEdition (const struct Plc_Places *Places);
static Usr_Can_t Ctr_CheckIfICanEditACenter (struct Hie_Node *Ctr);

static void Ctr_ShowAlertAndButtonToGoToCtr (void);

static void Ctr_PutFormToCreateCenter (const struct Plc_Places *Places);
static void Ctr_PutHeadCentersForSeeing (bool OrderSelectable);
static void Ctr_PutHeadCentersForEdition (void);
static void Ctr_ReceiveRequestOrCreateCtr (Hie_Status_t Status);

static void Ctr_EditingCenterConstructor (void);
static void Ctr_EditingCenterDestructor (void);

static void Ctr_FormToGoToMap (struct Hie_Node *Ctr);

static void Ctr_PutParCtrCod (void *CtrCod);

/*****************************************************************************/
/******************* List centers with pending degrees ***********************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Centers_with_pending_degrees;
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_There_are_no_centers_with_requests_for_degrees_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Hie_Node Ctr;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   const char *BgColor;

   /***** Get centers with pending degrees *****/
   if ((NumCtrs = Ctr_DB_GetCtrsWithPendingDegs (&mysql_res)))
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Centers_with_pending_degrees,NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

	 /***** Wrtie heading *****/
	 HTM_TR_Begin (NULL);
            HTM_TH (Txt_HIERARCHY_SINGUL_Abc[Hie_CTR],HTM_HEAD_LEFT );
            HTM_TH (Txt_Degrees_ABBREVIATION  ,HTM_HEAD_RIGHT);
	 HTM_TR_End ();

	 /***** List the centers *****/
	 for (NumCtr = 0, The_ResetRowColor ();
	      NumCtr < NumCtrs;
	      NumCtr++, The_ChangeRowColor ())
	   {
	    /* Get next center */
	    row = mysql_fetch_row (mysql_res);

	    /* Get center code (row[0]) */
	    Ctr.HieCod = Str_ConvertStrCodToLongCod (row[0]);
	    BgColor = (Ctr.HieCod == Gbl.Hierarchy.Node[Hie_CTR].HieCod) ? "BG_HIGHLIGHT" :
									The_GetColorRows ();

	    /* Get data of center */
	    SuccessOrError = Hie_GetDataByCod[Hie_CTR] (&Ctr);

	    /* Center logo and full name */
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM DAT_%s NOWRAP %s\"",The_GetSuffix (),BgColor);
		  Ctr_DrawCenterLogoAndNameWithLink (&Ctr,ActSeeDeg,"CM ICO16x16");
	       HTM_TD_End ();

	       /* Number of pending degrees (row[1]) */
	       HTM_TD_Begin ("class=\"RM DAT_%s %s\"",The_GetSuffix (),BgColor);
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

void Ctr_DrawCenterLogoAndNameWithLink (struct Hie_Node *Ctr,Act_Action_t Action,
                                        const char *IconClass)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
      ParCod_PutPar (ParCod_Ctr,Ctr->HieCod);

      /***** Link to action *****/
      HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Ctr->FullName),NULL,
                               "class=\"LT BT_LINK\"");
      Str_FreeGoToTitle ();

	 /***** Center logo and name *****/
	 Lgo_DrawLogo (Hie_CTR,Ctr,IconClass);
	 HTM_NBSP ();
         HTM_Txt (Ctr->FullName);

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
   if (Gbl.Hierarchy.Node[Hie_INS].HieCod <= 0)		// No institution selected
      return;

   /***** Get parameter with the type of order in the list of centers *****/
   Gbl.Hierarchy.List[Hie_INS].SelectedOrder = Hie_GetParHieOrder ();

   /***** Get list of centers *****/
   Ctr_GetFullListOfCenters (Gbl.Hierarchy.Node[Hie_INS].HieCod,
                             Gbl.Hierarchy.List[Hie_INS].SelectedOrder);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** List centers *****/
   Ctr_ListCenters ();

   /***** Free list of centers *****/
   Hie_FreeList (Hie_INS);
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
   if (asprintf (&Title,Txt_Centers_of_INSTITUTION_X,
		 Gbl.Hierarchy.Node[Hie_INS].FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Ctr_PutIconsListingCenters,NULL,
                 Hlp_INSTITUTION_Centers,Box_NOT_CLOSABLE);
   free (Title);

      if (Gbl.Hierarchy.List[Hie_INS].Num)	// There are centers in the current institution
	{
	 /***** Begin table *****/
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Heading *****/
	    Ctr_PutHeadCentersForSeeing (true);	// Order selectable

	    /***** Write all centers and their nuber of teachers *****/
	    for (NumCtr = 0;
		 NumCtr < Gbl.Hierarchy.List[Hie_INS].Num;
		 NumCtr++)
	       Ctr_ListOneCenterForSeeing (&(Gbl.Hierarchy.List[Hie_INS].Lst[NumCtr]),
					   NumCtr + 1);

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
   if (Hie_CheckIfICanEdit () == Usr_CAN)
      Ctr_PutIconToEditCenters ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/********************** Put link (form) to edit centers **********************/
/*****************************************************************************/

static void Ctr_PutIconToEditCenters (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCtr,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************************* List one center for seeing ************************/
/*****************************************************************************/

static void Ctr_ListOneCenterForSeeing (struct Hie_Node *Ctr,unsigned NumCtr)
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
   BgColor = (Ctr->HieCod == Gbl.Hierarchy.Node[Hie_CTR].HieCod) ? "BG_HIGHLIGHT" :
								   The_GetColorRows ();

   HTM_TR_Begin (NULL);

      /***** Number of center in this list *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (NumCtr);
      HTM_TD_End ();

      /***** Center logo and name *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",TxtClassStrong,The_GetSuffix (),BgColor);
	 Ctr_DrawCenterLogoAndNameWithLink (Ctr,ActSeeDeg,"CT ICO16x16");
      HTM_TD_End ();

      /***** Number of users who claim to belong to this center *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumUsrsWhoClaimToBelongTo (Hie_CTR,Ctr));
      HTM_TD_End ();

      /***** Place *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 Plc.PlcCod = Ctr->Specific.PlcCod;
	 Plc_GetPlaceDataByCod (&Plc);
	 HTM_Txt (Plc.ShrtName);
      HTM_TD_End ();

      /***** Number of degrees *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
						      Hie_CTR,	// ...in center
						      Ctr->HieCod));
      HTM_TD_End ();

      /***** Number of courses *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Hie_GetCachedNumNodesInHieLvl (Hie_CRS,	// Number of courses...
						      Hie_CTR,	// ...in center
						      Ctr->HieCod));
      HTM_TD_End ();

      /***** Number of users in courses of this center *****/
      HTM_TD_Begin ("class=\"RT %s_%s %s\"",TxtClassNormal,The_GetSuffix (),BgColor);
	 HTM_Unsigned (Enr_GetCachedNumUsrsInCrss (Hie_CTR,Ctr->HieCod,
						   1 << Rol_STD |
						   1 << Rol_NET |
						   1 << Rol_TCH));	// Any user
      HTM_TD_End ();

      /***** Center status *****/
      Hie_WriteStatusCell (Ctr->Status,TxtClassNormal,BgColor,Txt_CENTER_STATUS);

   HTM_TR_End ();
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
   Gbl.Hierarchy.List[Hie_INS].SelectedOrder = Hie_ORDER_BY_NAME;
   Ctr_GetFullListOfCenters (Gbl.Hierarchy.Node[Hie_INS].HieCod,
                             Gbl.Hierarchy.List[Hie_INS].SelectedOrder);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   if (asprintf (&Title,Txt_Centers_of_INSTITUTION_X,
		 Gbl.Hierarchy.Node[Hie_INS].FullName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin (Title,Ctr_PutIconsEditingCenters,NULL,
                 Hlp_INSTITUTION_Centers,Box_NOT_CLOSABLE);
   free (Title);

      /***** Put a form to create a new center *****/
      Ctr_PutFormToCreateCenter (&Places);

      /***** List current centers *****/
      if (Gbl.Hierarchy.List[Hie_INS].Num)
	 Ctr_ListCentersForEdition (&Places);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of centers *****/
   Hie_FreeList (Hie_INS);

   /***** Free list of places *****/
   Plc_FreeListPlaces (&Places);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of centers *****************/
/*****************************************************************************/

static void Ctr_PutIconsEditingCenters (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view centers *****/
   Ico_PutContextualIconToView (ActSeeCtr,NULL,NULL,NULL);

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
   Gbl.Hierarchy.List[Hie_INS].Num = Ctr_DB_GetListOfCtrsFull (&mysql_res,InsCod);

   if (Gbl.Hierarchy.List[Hie_INS].Num) // Centers found...
     {
      /***** Create list with centers in institution *****/
      if ((Gbl.Hierarchy.List[Hie_INS].Lst = calloc ((size_t) Gbl.Hierarchy.List[Hie_INS].Num,
							sizeof (*Gbl.Hierarchy.List[Hie_INS].Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the centers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.List[Hie_INS].Num;
	   NumCtr++)
         /* Get center data */
         Ctr_GetCenterDataFromRow (mysql_res,&Gbl.Hierarchy.List[Hie_INS].Lst[NumCtr],
                                   false);	// Don't get number of users who claim to belong to this center
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get full list of centers                         **************/
/************* with number of users who claim to belong to them **************/
/*****************************************************************************/

void Ctr_GetFullListOfCenters (long InsCod,Hie_Order_t SelectedOrder)
  {
   MYSQL_RES *mysql_res;
   unsigned NumCtr;

   /***** Get centers from database *****/
   Gbl.Hierarchy.List[Hie_INS].Num = Ctr_DB_GetListOfCtrsFullWithNumUsrs (&mysql_res,InsCod,SelectedOrder);

   if (Gbl.Hierarchy.List[Hie_INS].Num) // Centers found...
     {
      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.List[Hie_INS].Lst = calloc ((size_t) Gbl.Hierarchy.List[Hie_INS].Num,
							sizeof (*Gbl.Hierarchy.List[Hie_INS].Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the centers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.List[Hie_INS].Num;
	   NumCtr++)
         /* Get center data */
         Ctr_GetCenterDataFromRow (mysql_res,&Gbl.Hierarchy.List[Hie_INS].Lst[NumCtr],
                                   true);	// Get number of users who claim to belong to this center
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get data of center by code *************************/
/*****************************************************************************/

Err_SuccessOrError_t Ctr_GetCenterDataByCod (struct Hie_Node *Node)
  {
   MYSQL_RES *mysql_res;
   Err_SuccessOrError_t SuccessOrError = Err_ERROR;

   /***** Clear data *****/
   Node->PrtCod          = -1L;
   Node->Specific.PlcCod = -1L;
   Node->Status          = (Hie_Status_t) 0;
   Node->RequesterUsrCod = -1L;
   Node->ShrtName[0]     = '\0';
   Node->FullName[0]     = '\0';
   Node->WWW[0]          = '\0';
   Node->NumUsrsWhoClaimToBelong.Valid = false;

   /***** Check if center code is correct *****/
   if (Node->HieCod > 0)
     {
      /***** Get data of a center from database *****/
      if (Ctr_DB_GetCenterDataByCod (&mysql_res,Node->HieCod) == Exi_EXISTS) // Center found...
        {
         /* Get center data */
         Ctr_GetCenterDataFromRow (mysql_res,Node,
                                   false);	// Don't get number of users
						// who claim to belong to this center

         /* Set return value */
         SuccessOrError = Err_SUCCESS;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return SuccessOrError;
  }

/*****************************************************************************/
/******************** Get coordinates of center by code **********************/
/*****************************************************************************/

void Ctr_GetCoordByCod (long CtrCod,struct Map_Coordinates *Coord)
  {
   MYSQL_RES *mysql_res;

   /***** Get coordinates of a center from database *****/
   if (Ctr_DB_GetCoordByCod (&mysql_res,CtrCod)) // Center found...
      Ctr_GetCoordFromRow (mysql_res,Coord);
   else
      Coord->Latitude = Coord->Longitude = Coord->Altitude = 0.0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Get data of a center from a row resulting of a query *************/
/*****************************************************************************/

static void Ctr_GetCenterDataFromRow (MYSQL_RES *mysql_res,
				      struct Hie_Node *Ctr,
                                      bool GetNumUsrsWhoClaimToBelongToCtr)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get center code (row[0]) *****/
   if ((Ctr->HieCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongCenterExit ();

   /***** Get institution code (row[1]) *****/
   Ctr->PrtCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get place code (row[2]) *****/
   Ctr->Specific.PlcCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get center status (row[3]) *****/
   if (sscanf (row[3],"%u",&(Ctr->Status)) != 1)
      Err_WrongStatusExit ();

   /***** Get requester user's code (row[4]) *****/
   Ctr->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[4]);

   /***** Get short name (row[5]), full name (row[6])
          and URL (row[7]) of the center *****/
   Str_Copy (Ctr->ShrtName,row[5],sizeof (Ctr->ShrtName) - 1);
   Str_Copy (Ctr->FullName,row[6],sizeof (Ctr->FullName) - 1);
   Str_Copy (Ctr->WWW     ,row[7],sizeof (Ctr->WWW     ) - 1);

   /* Get number of users who claim to belong to this center (row[8]) */
   Ctr->NumUsrsWhoClaimToBelong.Valid = false;
   if (GetNumUsrsWhoClaimToBelongToCtr)
      if (sscanf (row[8],"%u",&(Ctr->NumUsrsWhoClaimToBelong.NumUsrs)) == 1)
	 Ctr->NumUsrsWhoClaimToBelong.Valid = true;
  }

/*****************************************************************************/
/********* Get coordinares of a center from a row resulting of a query *******/
/*****************************************************************************/

static void Ctr_GetCoordFromRow (MYSQL_RES *mysql_res,
				 struct Map_Coordinates *Coord)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get latitude (row[0], longitude (row[1]) and altitude (row[2])*****/
   Coord->Latitude  = Map_GetLatitudeFromStr  (row[0]);
   Coord->Longitude = Map_GetLongitudeFromStr (row[1]);
   Coord->Altitude  = Map_GetAltitudeFromStr  (row[2]);
  }

/*****************************************************************************/
/************************** Write selector of center *************************/
/*****************************************************************************/

void Ctr_WriteSelectorOfCenter (void)
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   long CtrCod;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeDeg);

      /***** Begin selector *****/
      HTM_SELECT_Begin ((Gbl.Hierarchy.Node[Hie_INS].HieCod > 0) ? HTM_SUBMIT_ON_CHANGE :
								   HTM_DISABLED,
			NULL,
			"id=\"ctr\" name=\"ctr\" class=\"HIE_SEL INPUT_%s\"",
			The_GetSuffix ());

	 /***** Initial disabled option *****/
	 HTM_OPTION (HTM_Type_STRING,"",
		     ((Gbl.Hierarchy.Node[Hie_CTR].HieCod < 0) ? HTM_SELECTED :
							         HTM_NO_ATTR) | HTM_DISABLED,
		     "[%s]",Txt_HIERARCHY_SINGUL_Abc[Hie_CTR]);

	 if (Gbl.Hierarchy.Node[Hie_INS].HieCod > 0)
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
			   Gbl.Hierarchy.Node[Hie_CTR].HieCod > 0 &&
			   (CtrCod == Gbl.Hierarchy.Node[Hie_CTR].HieCod) ? HTM_SELECTED :
									    HTM_NO_ATTR,
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
   static Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = ActRenCtrSho,
      [Nam_FULL_NAME] = ActRenCtrFul,
     };
   unsigned NumCtr;
   struct Hie_Node *Ctr;
   unsigned NumPlc;
   const struct Plc_Place *Plc;
   char WWW[WWW_MAX_BYTES_WWW + 1];
   struct Usr_Data UsrDat;
   Usr_Can_t ICanEdit;
   unsigned NumDegs;
   unsigned NumUsrsCtr;
   unsigned NumUsrsInCrssOfCtr;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Ctr_PutHeadCentersForEdition ();

      /***** Write all centers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.List[Hie_INS].Num;
	   NumCtr++)
	{
	 Ctr = &Gbl.Hierarchy.List[Hie_INS].Lst[NumCtr];

	 ICanEdit = Ctr_CheckIfICanEditACenter (Ctr);
	 NumDegs = Hie_GetNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
					    Hie_CTR,	// ...in center
					    Ctr->HieCod);
	 NumUsrsCtr = Hie_GetNumUsrsWhoClaimToBelongTo (Hie_CTR,Ctr);
	 NumUsrsInCrssOfCtr = Enr_GetNumUsrsInCrss (Hie_CTR,Ctr->HieCod,
						    1 << Rol_STD |
						    1 << Rol_NET |
						    1 << Rol_TCH);	// Any user

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove center */
	    HTM_TD_Begin ("class=\"BT\"");
	       if (ICanEdit == Usr_CAN_NOT ||		// I cannot edit
		   NumDegs ||				// Center has degrees
		   NumUsrsCtr ||			// Center has users who claim to belong to it
		   NumUsrsInCrssOfCtr)			// Center has users
		  Ico_PutIconRemovalNotAllowed ();
	       else	// I can remove center
		  Ico_PutContextualIconToRemove (ActRemCtr,NULL,
						 Hie_PutParOtherHieCod,&Ctr->HieCod);
	    HTM_TD_End ();

	    /* Center code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Ctr->HieCod);
	    HTM_TD_End ();

	    /* Center logo */
	    HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr->FullName);
	       Lgo_DrawLogo (Hie_CTR,Ctr,"ICO20x20");
	    HTM_TD_End ();

	    /* Place */
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	       switch (ICanEdit)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActChgCtrPlc);
			ParCod_PutPar (ParCod_OthHie,Ctr->HieCod);
			HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,NULL,
					  "name=\"PlcCod\""
					  " class=\"PLC_SEL INPUT_%s\"",
					  The_GetSuffix ());
			   HTM_OPTION (HTM_Type_STRING,"0",
				       (Ctr->Specific.PlcCod == 0) ? HTM_SELECTED :
								     HTM_NO_ATTR,
				       "%s",Txt_Another_place);
			   for (NumPlc = 0;
				NumPlc < Places->Num;
				NumPlc++)
			     {
			      Plc = &Places->Lst[NumPlc];
			      HTM_OPTION (HTM_Type_LONG,&Plc->PlcCod,
					  (Plc->PlcCod == Ctr->Specific.PlcCod) ? HTM_SELECTED :
										  HTM_NO_ATTR,
					  "%s",Plc->ShrtName);
			     }
			HTM_SELECT_End ();
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     for (NumPlc = 0;
			  NumPlc < Places->Num;
			  NumPlc++)
			if (Places->Lst[NumPlc].PlcCod == Ctr->Specific.PlcCod)
			   HTM_Txt (Places->Lst[NumPlc].ShrtName);
		     break;
		 }
	    HTM_TD_End ();

	    /* Center short name and full name */
	    Names[Nam_SHRT_NAME] = Ctr->ShrtName;
	    Names[Nam_FULL_NAME] = Ctr->FullName;
	    Nam_ExistingShortAndFullNames (ActionRename,
				           ParCod_OthHie,Ctr->HieCod,
				           Names,
				           ICanEdit == Usr_CAN ? Frm_PUT_FORM :
				        			   Frm_DONT_PUT_FORM);

	    /* Center WWW */
	    HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	       switch (ICanEdit)
		 {
		  case Usr_CAN:
		     Frm_BeginForm (ActChgCtrWWW);
			ParCod_PutPar (ParCod_OthHie,Ctr->HieCod);
			HTM_INPUT_URL ("WWW",Ctr->WWW,
				       HTM_REQUIRED | HTM_SUBMIT_ON_CHANGE,
				       "class=\"INPUT_WWW INPUT_%s\"",
				       The_GetSuffix ());
		     Frm_EndForm ();
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Str_Copy (WWW,Ctr->WWW,sizeof (WWW) - 1);
		     HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHRT\"");
			HTM_A_Begin ("href=\"%s\" target=\"_blank\""
				     " class=\"DAT_%s\" title=\"%s\"",
				     Ctr->WWW,The_GetSuffix (),Ctr->WWW);
			   HTM_Txt (WWW);
			HTM_A_End ();
		     HTM_DIV_End ();
		     break;
		 }
	    HTM_TD_End ();

	    /* Number of users who claim to belong to this center */
	    HTM_TD_Unsigned (NumUsrsCtr);

	    /* Number of degrees */
	    HTM_TD_Unsigned (NumDegs);

	    /* Number of users in courses of this center */
	    HTM_TD_Unsigned (NumUsrsInCrssOfCtr);

	    /* Center requester */
	    UsrDat.UsrCod = Ctr->RequesterUsrCod;
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
						     Usr_DONT_GET_PREFS,
						     Usr_DONT_GET_ROLE_IN_CRS);
	    HTM_TD_Begin ("class=\"DAT_%s INPUT_REQUESTER LT\"",The_GetSuffix ());
	       Usr_WriteAuthor (&UsrDat,For_ENABLED);
	    HTM_TD_End ();

	    /* Center status */
	    Hie_WriteStatusCellEditable (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM ? Usr_CAN :
										  Usr_CAN_NOT,
	                                 Ctr->Status,ActChgCtrSta,Ctr->HieCod,
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

static Usr_Can_t Ctr_CheckIfICanEditACenter (struct Hie_Node *Ctr)
  {
   return (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM ||		// I am an institution administrator or higher
           ((Ctr->Status & Hie_STATUS_BIT_PENDING) &&	// Center is not yet activated
            Gbl.Usrs.Me.UsrDat.UsrCod == Ctr->RequesterUsrCod)) ? Usr_CAN :	// I am the requester
        							  Usr_CAN_NOT;
  }

/*****************************************************************************/
/******************************* Remove a center *****************************/
/*****************************************************************************/

void Ctr_RemoveCenter (void)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center;
   extern const char *Txt_Center_X_removed;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   char PathCtr[PATH_MAX + 1];

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get center code *****/
   Ctr_EditingCtr->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get data of the center from database *****/
   SuccessOrError = Hie_GetDataByCod[Hie_CTR] (Ctr_EditingCtr);

   /***** Check if this center has teachers *****/
   if (Hie_GetNumNodesInHieLvl (Hie_DEG,	// Number of degrees...
				Hie_CTR,	// ...in center
				Ctr_EditingCtr->HieCod))	// Center has degrees
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else if (Hie_GetNumUsrsWhoClaimToBelongTo (Hie_CTR,Ctr_EditingCtr))	// Center has users who claim to belong to it
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else if (Enr_GetNumUsrsInCrss (Hie_CTR,Ctr_EditingCtr->HieCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))			// Center has users
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else	// Center has no degrees or users ==> remove it
     {
      /***** Remove all threads and posts in forums of the center *****/
      For_DB_RemoveForums (Hie_CTR,Ctr_EditingCtr->HieCod);

      /***** Remove surveys of the center *****/
      Svy_RemoveSurveys (Hie_CTR,Ctr_EditingCtr->HieCod);

      /***** Remove information related to files in center *****/
      Brw_DB_RemoveCtrFiles (Ctr_EditingCtr->HieCod);

      /***** Remove all rooms in center *****/
      Roo_DB_RemoveAllRoomsInCtr (Ctr_EditingCtr->HieCod);

      /***** Remove directories of the center *****/
      snprintf (PathCtr,sizeof (PathCtr),"%s/%02u/%u",
	        Cfg_PATH_CTR_PUBLIC,
	        (unsigned) (Ctr_EditingCtr->HieCod % 100),
	        (unsigned)  Ctr_EditingCtr->HieCod);
      Fil_RemoveTree (PathCtr);

      /***** Remove administrators of this center *****/
      Adm_DB_RemAdmins (Hie_CTR,Ctr_EditingCtr->HieCod);

      /***** Remove center *****/
      Ctr_DB_RemoveCenter (Ctr_EditingCtr->HieCod);

      /***** Flush caches *****/
      Hie_FlushCachedNumNodesInHieLvl (Hie_DEG,Hie_CTR);	// Number of degrees in center
      Hie_FlushCachedNumNodesInHieLvl (Hie_CRS,Hie_CTR);	// Number of courses in center
      Hie_FlushCacheNumUsrsWhoClaimToBelongTo (Hie_CTR);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Center_X_removed,
	               Ctr_EditingCtr->FullName);

      Ctr_EditingCtr->HieCod = -1L;	// To not showing button to go to center
     }
  }

/*****************************************************************************/
/************************ Change the place of a center ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrPlc (void)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_place_of_the_center_has_changed;
   long NewPlcCod;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get center code *****/
   Ctr_EditingCtr->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get parameter with place code *****/
   NewPlcCod = ParCod_GetAndCheckParMin (ParCod_Plc,0);	// 0 (another place) is allowed here

   /***** Get data of center from database *****/
   SuccessOrError = Hie_GetDataByCod[Hie_CTR] (Ctr_EditingCtr);

   /***** Update place in table of centers *****/
   Ctr_DB_UpdateCtrPlc (Ctr_EditingCtr->HieCod,NewPlcCod);
   Ctr_EditingCtr->Specific.PlcCod = NewPlcCod;

   /***** Create alert to show the change made
	  and put button to go to center changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_The_place_of_the_center_has_changed);
  }

/*****************************************************************************/
/************************ Change the name of a center ************************/
/*****************************************************************************/

void Ctr_RenameCenterShrt (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Rename center *****/
   Ctr_EditingCtr->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ctr_RenameCenter (Ctr_EditingCtr,Nam_SHRT_NAME);
  }

void Ctr_RenameCenterFull (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Rename center *****/
   Ctr_EditingCtr->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);
   Ctr_RenameCenter (Ctr_EditingCtr,Nam_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a center ************************/
/*****************************************************************************/

void Ctr_RenameCenter (struct Hie_Node *Ctr,Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   extern const char *Txt_The_center_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Ctr->ShrtName,
      [Nam_FULL_NAME] = Ctr->FullName,
     };
   char NewName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get parameters from form *****/
   /* Get the new name for the center */
   Nam_GetParShrtOrFullName (ShrtOrFull,NewName);

   /***** Get from the database the old names of the center *****/
   SuccessOrError = Hie_GetDataByCod[Hie_CTR] (Ctr);

   /***** Check if new name is empty *****/
   if (!NewName[0])
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
         /***** If degree was in database... *****/
         switch (Ctr_DB_CheckIfCtrNameExistsInIns (Nam_Fields[ShrtOrFull],
					           NewName,Ctr->HieCod,
					           Gbl.Hierarchy.Node[Hie_INS].HieCod,
					           0))	// Unused
           {
            case Exi_EXISTS:
	       Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,NewName);
	       break;
            case Exi_DOES_NOT_EXIST:
            default:
	       /* Update the table changing old name by new name */
	       Ctr_DB_UpdateCtrName (Ctr->HieCod,
				     Nam_Fields[ShrtOrFull],NewName);

	       /* Write message to show the change made */
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
				Txt_The_center_X_has_been_renamed_as_Y,
				CurrentName[ShrtOrFull],NewName);

	       /* Change current center name in order to display it properly */
	       Str_Copy (CurrentName[ShrtOrFull],NewName,
		         Nam_MaxBytes[ShrtOrFull]);
	       break;
           }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,
                          CurrentName[ShrtOrFull]);
     }
  }

/*****************************************************************************/
/************************* Change the URL of a center ************************/
/*****************************************************************************/

void Ctr_ChangeCtrWWW (void)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_new_web_address_is_X;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;
   char NewWWW[WWW_MAX_BYTES_WWW + 1];

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get the code of the center *****/
   Ctr_EditingCtr->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /***** Get the new WWW for the center *****/
   Par_GetParText ("WWW",NewWWW,WWW_MAX_BYTES_WWW);

   /***** Get data of center *****/
   SuccessOrError = Hie_GetDataByCod[Hie_CTR] (Ctr_EditingCtr);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_DB_UpdateCtrWWW (Ctr_EditingCtr->HieCod,NewWWW);
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
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_The_status_of_the_center_X_has_changed;
   Hie_Status_t Status;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get parameters from form *****/
   /* Get center code */
   Ctr_EditingCtr->HieCod = ParCod_GetAndCheckPar (ParCod_OthHie);

   /* Get parameter with status */
   Status = Hie_GetParStatus ();	// New status

   /***** Get data of center *****/
   SuccessOrError = Hie_GetDataByCod[Hie_CTR] (Ctr_EditingCtr);

   /***** Update status *****/
   Ctr_DB_UpdateCtrStatus (Ctr_EditingCtr->HieCod,Status);
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
   if (Ctr_EditingCtr->HieCod != Gbl.Hierarchy.Node[Hie_CTR].HieCod)
      /***** Alert with button to go to center *****/
      Ale_ShowLastAlertAndButtonGo (ActSeeDeg,
                                    Ctr_PutParCtrCod,&Ctr_EditingCtr->HieCod,
                                    Ctr_EditingCtr->ShrtName);
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
   struct Hie_Node Ctr;
   unsigned NumPlc;
   const struct Plc_Place *Plc;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Set action depending on role *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      NextAction = ActNewCtr;
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      NextAction = ActReqCtr;
   else
      Err_NoPermissionExit ();

   /***** Begin form to create *****/
   Frm_BeginFormTable (NextAction,NULL,NULL,NULL,"TBL_SCROLL");

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
	    Ctr.HieCod = -1L;
	    Ctr.ShrtName[0] = '\0';
	    Lgo_DrawLogo (Hie_CTR,&Ctr,"ICO20x20");
	 HTM_TD_End ();

	 /***** Place *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
			      "name=\"PlcCod\" class=\"PLC_SEL INPUT_%s\"",
			      The_GetSuffix ());
	       HTM_OPTION (HTM_Type_STRING,"0",
			   (Ctr_EditingCtr->Specific.PlcCod == 0) ? HTM_SELECTED :
							            HTM_NO_ATTR,
			   "%s",Txt_Another_place);
	       for (NumPlc = 0;
		    NumPlc < Places->Num;
		    NumPlc++)
		 {
		  Plc = &Places->Lst[NumPlc];
		  HTM_OPTION (HTM_Type_LONG,&Plc->PlcCod,
			      (Plc->PlcCod == Ctr_EditingCtr->Specific.PlcCod) ? HTM_SELECTED :
									         HTM_NO_ATTR,
			      "%s",Plc->ShrtName);
		 }
	    HTM_SELECT_End ();
	 HTM_TD_End ();

	 /***** Center short name and full name *****/
	 Names[Nam_SHRT_NAME] = Ctr_EditingCtr->ShrtName;
	 Names[Nam_FULL_NAME] = Ctr_EditingCtr->FullName;
	 Nam_NewShortAndFullNames (Names);

	 /***** Center WWW *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("WWW",Ctr_EditingCtr->WWW,
			   HTM_REQUIRED,
			   "class=\"INPUT_WWW INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Number of users who claim to belong to this center,
	        number of degrees and
	        number of users in courses of this center *****/
	 HTM_TD_Unsigned (0);
	 HTM_TD_Unsigned (0);
	 HTM_TD_Unsigned (0);

	 /***** Center requester *****/
	 HTM_TD_Begin ("class=\"DAT_%s INPUT_REQUESTER LT\"",The_GetSuffix ());
	    Usr_WriteAuthor (&Gbl.Usrs.Me.UsrDat,For_ENABLED);
	 HTM_TD_End ();

	 /***** Center status *****/
	 HTM_TD_Empty (1);

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
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
   Hie_Order_t Order;
   static HTM_HeadAlign Align[Hie_NUM_ORDERS] =
     {
      [Hie_ORDER_BY_NAME    ] = HTM_HEAD_LEFT,
      [Hie_ORDER_BY_NUM_USRS] = HTM_HEAD_RIGHT
     };

   HTM_TR_Begin (NULL);

      HTM_TH_Empty (1);

      for (Order  = (Hie_Order_t) 0;
	   Order <= (Hie_Order_t) (Hie_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (Align[Order]);
	    if (OrderSelectable)
	      {
	       Frm_BeginForm (ActSeeCtr);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_CENTERS_HELP_ORDER[Order],NULL,
					   "class=\"BT_LINK RT\"");
		     if (Order == Gbl.Hierarchy.List[Hie_INS].SelectedOrder)
			HTM_U_Begin ();
	      }
	    HTM_Txt (Txt_CENTERS_ORDER[Order]);
	    if (OrderSelectable)
	      {
		     if (Order == Gbl.Hierarchy.List[Hie_INS].SelectedOrder)
			HTM_U_End ();
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	 HTM_TH_End ();
	}

      HTM_TH (Txt_Place               ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Degrees_ABBREVIATION,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Courses_ABBREVIATION,HTM_HEAD_RIGHT);
      Usr_THTchsPlusStds ();
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
      Usr_THTchsPlusStds ();
      HTM_TH (Txt_Requester               ,HTM_HEAD_LEFT );
      HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Receive form to request a new center *********************/
/*****************************************************************************/

void Ctr_ReceiveReqCtr (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Receive form to request a new center *****/
   Ctr_ReceiveRequestOrCreateCtr ((Hie_Status_t) Hie_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new center *********************/
/*****************************************************************************/

void Ctr_ReceiveNewCtr (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Receive form to create a new center *****/
   Ctr_ReceiveRequestOrCreateCtr ((Hie_Status_t) 0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new center ****************/
/*****************************************************************************/

static void Ctr_ReceiveRequestOrCreateCtr (Hie_Status_t Status)
  {
   extern const char *Txt_Created_new_center_X;
   char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Get parameters from form *****/
   /* Set center institution */
   Ctr_EditingCtr->PrtCod = Gbl.Hierarchy.Node[Hie_INS].HieCod;

   /* Get place */
   Ctr_EditingCtr->Specific.PlcCod = ParCod_GetAndCheckParMin (ParCod_Plc,0);	// 0 (another place) is allowed here

   /* Get center short name and full name */
   Names[Nam_SHRT_NAME] = Ctr_EditingCtr->ShrtName;
   Names[Nam_FULL_NAME] = Ctr_EditingCtr->FullName;
   Nam_GetParsShrtAndFullName (Names);

   /* Get center WWW */
   Par_GetParText ("WWW",Ctr_EditingCtr->WWW,WWW_MAX_BYTES_WWW);

   if (Ctr_EditingCtr->ShrtName[0] &&
       Ctr_EditingCtr->FullName[0])	// If there's a center name
     {
      if (Ctr_EditingCtr->WWW[0])
        {
         /***** If name of center was in database... *****/
	 if (Nam_CheckIfNameExists (Ctr_DB_CheckIfCtrNameExistsInIns,
				    (const char **) Names,
				    -1L,
				    Gbl.Hierarchy.Node[Hie_INS].HieCod,
				    0) == Exi_DOES_NOT_EXIST)	// Unused
           {
            Ctr_EditingCtr->HieCod = Ctr_DB_CreateCenter (Ctr_EditingCtr,Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_center_X,
			     Names[Nam_FULL_NAME]);
           }
        }
      else	// If there is not a web
         Ale_CreateAlertYouMustSpecifyTheWebAddress ();
     }
   else		// If there is not a center name
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
  }

/*****************************************************************************/
/********************** Get number of centers with map ***********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithMapInSys (void)
  {
   unsigned NumCtrsWithMap;

   /***** Get number of centers with map from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_MAP,Hie_SYS,-1L,
                                   FigCch_UNSIGNED,&NumCtrsWithMap))
     {
      /***** Get current number of centers with map from database and update cache *****/
      /* Ccoordinates 0, 0 means not set ==> don't show map */
      NumCtrsWithMap = (unsigned) Ctr_DB_GetNumCtrsWithMap ();
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_MAP,Hie_SYS,-1L,
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
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_MAP,Hie_CTY,CtyCod,
                                   FigCch_UNSIGNED,&NumCtrsWithMap))
     {
      /***** Get current number of centers with map from database and update cache *****/
      /* Ccoordinates 0, 0 means not set ==> don't show map */
      NumCtrsWithMap = Ctr_DB_GetNumCtrsWithMapInCty (CtyCod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_MAP,Hie_CTY,CtyCod,
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
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_MAP,Hie_INS,InsCod,
                                   FigCch_UNSIGNED,&NumCtrsWithMap))
     {
      /***** Get current number of centers with map from database and update cache *****/
      /* Ccoordinates 0, 0 means not set ==> don't show map */
      NumCtrsWithMap = Ctr_DB_GetNumCtrsWithMapInIns (InsCod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_MAP,Hie_INS,InsCod,
                                    FigCch_UNSIGNED,&NumCtrsWithMap);
     }

   return NumCtrsWithMap;
  }

/*****************************************************************************/
/********************* Get number of centers with users **********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithUsrs (Hie_Level_t HieLvl,Rol_Role_t Role)
  {
   static FigCch_FigureCached_t FigureCtrs[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_CTRS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_CTRS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_CTRS_WITH_TCHS,	// Teachers
     };
   unsigned NumNodesWithUsrs;
   long HieCod = Hie_GetHieCod (HieLvl);

   /***** Get number of centers with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureCtrs[Role],HieLvl,HieCod,
				   FigCch_UNSIGNED,&NumNodesWithUsrs))
     {
      /***** Get current number of centers with users from database and update cache *****/
      NumNodesWithUsrs = Ctr_DB_GetNumCtrsWithUsrs (HieLvl,HieCod,Role);
      FigCch_UpdateFigureIntoCache (FigureCtrs[Role],HieLvl,HieCod,
				    FigCch_UNSIGNED,&NumNodesWithUsrs);
     }

   return NumNodesWithUsrs;
  }

/*****************************************************************************/
/****************************** List centers found ***************************/
/*****************************************************************************/

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_HIERARCHY_SINGUL_abc[Hie_NUM_LEVELS];
   extern const char *Txt_HIERARCHY_PLURAL_abc[Hie_NUM_LEVELS];
   unsigned NumCtr;
   char *Title;
   struct Hie_Node Ctr;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;

   /***** Query database *****/
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      /* Number of centers found */
      if (asprintf (&Title,"%u %s",NumCtrs,
				   NumCtrs == 1 ? Txt_HIERARCHY_SINGUL_abc[Hie_CTR] :
	                                          Txt_HIERARCHY_PLURAL_abc[Hie_CTR]) < 0)
	 Err_NotEnoughMemoryExit ();
      Box_BoxTableBegin (Title,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);
      free (Title);

	 /***** Write heading *****/
	 Ctr_PutHeadCentersForSeeing (false);	// Order not selectable

	 /***** List the centers (one row per center) *****/
	 for (NumCtr  = 1, The_ResetRowColor ();
	      NumCtr <= NumCtrs;
	      NumCtr++, The_ChangeRowColor ())
	   {
	    /* Get next center */
	    Ctr.HieCod = DB_GetNextCode (*mysql_res);

	    /* Get data of center */
	    SuccessOrError = Hie_GetDataByCod[Hie_CTR] (&Ctr);

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
   Ctr_EditingCtr->HieCod             = -1L;
   Ctr_EditingCtr->PrtCod          = -1L;
   Ctr_EditingCtr->Specific.PlcCod = -1L;
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

static void Ctr_FormToGoToMap (struct Hie_Node *Ctr)
  {
   struct Map_Coordinates Coord;

   /***** Get coordinates of center *****/
   Ctr_GetCoordByCod (Ctr->HieCod,&Coord);

   if (Map_CheckIfCoordAreAvailable (&Coord))
     {
      Ctr_EditingCtr = Ctr;	// Used to pass parameter with the code of the center
      Lay_PutContextualLinkOnlyIcon (ActSeeCtrInf,NULL,
                                     Ctr_PutParCtrCod,&Ctr_EditingCtr->HieCod,
				     "map-marker-alt.svg",Ico_BLACK);
     }
  }

/*****************************************************************************/
/******************** Write parameter with code of center ********************/
/*****************************************************************************/

static void Ctr_PutParCtrCod (void *CtrCod)
  {
   if (CtrCod)
      ParCod_PutPar (ParCod_Ctr,*((long *) CtrCod));
  }
