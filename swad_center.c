// swad_center.c: centers

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_center.h"
#include "swad_center_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_place.h"
#include "swad_survey.h"

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

static struct Ctr_Center *Ctr_EditingCtr = NULL;	// Static variable to keep the center being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ctr_ListCenters (void);
static bool Ctr_CheckIfICanCreateCenters (void);
static void Ctr_PutIconsListingCenters (__attribute__((unused)) void *Args);
static void Ctr_PutIconToEditCenters (void);
static void Ctr_ListOneCenterForSeeing (struct Ctr_Center *Ctr,unsigned NumCtr);
static void Ctr_GetParamCtrOrder (void);

static void Ctr_EditCentersInternal (void);
static void Ctr_PutIconsEditingCenters (__attribute__((unused)) void *Args);

static void Ctr_GetDataOfCenterFromRow (struct Ctr_Center *Ctr,MYSQL_ROW row);

static void Ctr_ListCentersForEdition (const struct Plc_Places *Places);
static bool Ctr_CheckIfICanEditACenter (struct Ctr_Center *Ctr);
static Ctr_StatusTxt_t Ctr_GetStatusTxtFromStatusBits (Ctr_Status_t Status);
static Ctr_Status_t Ctr_GetStatusBitsFromStatusTxt (Ctr_StatusTxt_t StatusTxt);

static void Ctr_PutParamOtherCtrCod (void *CtrCod);

static void Ctr_UpdateInsNameDB (long CtrCod,const char *FieldName,const char *NewCtrName);

static void Ctr_ShowAlertAndButtonToGoToCtr (void);
static void Ctr_PutParamGoToCtr (void *CtrCod);

static void Ctr_PutFormToCreateCenter (const struct Plc_Places *Places);
static void Ctr_PutHeadCentersForSeeing (bool OrderSelectable);
static void Ctr_PutHeadCentersForEdition (void);
static void Ctr_ReceiveFormRequestOrCreateCtr (unsigned Status);
static void Ctr_CreateCenter (unsigned Status);

static unsigned Ctr_GetNumCtrsInCty (long CtyCod);

static void Ctr_EditingCenterConstructor (void);
static void Ctr_EditingCenterDestructor (void);

static void Ctr_FormToGoToMap (struct Ctr_Center *Ctr);

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
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_CTR_ADM:
         NumCtrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get centers with pending degrees",
			 "SELECT deg_degrees.CtrCod,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM deg_degrees,"
			        "ctr_admin,"
			        "ctr_centers"
			 " WHERE (deg_degrees.Status & %u)<>0"
			   " AND deg_degrees.CtrCod=ctr_admin.CtrCod"
			   " AND ctr_admin.UsrCod=%ld"
			   " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			 " GROUP BY deg_degrees.CtrCod"
			 " ORDER BY ctr_centers.ShortName",
			 (unsigned) Deg_STATUS_BIT_PENDING,
			 Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Rol_SYS_ADM:
         NumCtrs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get centers with pending degrees",
			 "SELECT deg_degrees.CtrCod,"	// row[0]
			        "COUNT(*)"		// row[1]
			  " FROM deg_degrees,"
			        "ctr_centers"
			 " WHERE (deg_degrees.Status & %u)<>0"
			   " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			 " GROUP BY deg_degrees.CtrCod"
			 " ORDER BY ctr_centers.ShortName",
			 (unsigned) Deg_STATUS_BIT_PENDING);
         break;
      default:	// Forbidden for other users
	 return;
     }
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Centers_with_pending_degrees,
                         NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

      /***** Wrtie heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"LM",Txt_Center);
      HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);

      HTM_TR_End ();

      /***** List the centers *****/
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
        {
         /* Get next center */
         row = mysql_fetch_row (mysql_res);

         /* Get center code (row[0]) */
         Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Ctr.CtrCod == Gbl.Hierarchy.Ctr.CtrCod) ? "LIGHT_BLUE" :
                                                              Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of center */
         Ctr_GetDataOfCenterByCod (&Ctr);

         /* Center logo and full name */
         HTM_TR_Begin (NULL);

         HTM_TD_Begin ("class=\"LM %s\"",BgColor);
         Ctr_DrawCenterLogoAndNameWithLink (&Ctr,ActSeeDeg,
                                            "BT_LINK DAT_NOBR","CM");
         HTM_TD_End ();

         /* Number of pending degrees (row[1]) */
         HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Txt (row[1]);
         HTM_TD_End ();

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
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
                                        const char *ClassLink,const char *ClassLogo)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
   Ctr_PutParamCtrCod (Ctr->CtrCod);

   /***** Link to action *****/
   HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Ctr->FullName),ClassLink,NULL);
   Hie_FreeGoToMsg ();

   /***** Center logo and name *****/
   Lgo_DrawLogo (HieLvl_CTR,Ctr->CtrCod,Ctr->ShrtName,16,ClassLogo,true);
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
   Ctr_GetParamCtrOrder ();

   /***** Get list of centers *****/
   Ctr_GetFullListOfCenters (Gbl.Hierarchy.Ins.InsCod);

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
   extern const char *Txt_Create_another_center;
   extern const char *Txt_Create_center;
   unsigned NumCtr;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Centers_of_INSTITUTION_X,
				          Gbl.Hierarchy.Ins.FullName),
		 Ctr_PutIconsListingCenters,NULL,
                 Hlp_INSTITUTION_Centers,Box_NOT_CLOSABLE);
   Str_FreeString ();

   if (Gbl.Hierarchy.Ctrs.Num)	// There are centers in the current institution
     {
      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Ctr_PutHeadCentersForSeeing (true);	// Order selectable

      /***** Write all the centers and their nuber of teachers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ctrs.Num;
	   NumCtr++)
	 Ctr_ListOneCenterForSeeing (&(Gbl.Hierarchy.Ctrs.Lst[NumCtr]),NumCtr + 1);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No centers created in the current institution
      Ale_ShowAlert (Ale_INFO,Txt_No_centers);

   /***** Button to create center *****/
   if (Ctr_CheckIfICanCreateCenters ())
     {
      Frm_BeginForm (ActEdiCtr);
      Btn_PutConfirmButton (Gbl.Hierarchy.Ctrs.Num ? Txt_Create_another_center :
	                                                 Txt_Create_center);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Check if I can create centers ************************/
/*****************************************************************************/

static bool Ctr_CheckIfICanCreateCenters (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_GST);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of centers *******************/
/*****************************************************************************/

static void Ctr_PutIconsListingCenters (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit centers *****/
   if (Ctr_CheckIfICanCreateCenters ())
      Ctr_PutIconToEditCenters ();

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

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
   extern const char *Txt_CENTER_STATUS[Ctr_NUM_STATUS_TXT];
   struct Plc_Place Plc;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   Ctr_StatusTxt_t StatusTxt;

   /***** Get data of place of this center *****/
   Plc.PlcCod = Ctr->PlcCod;
   Plc_GetDataOfPlaceByCod (&Plc);

   if (Ctr->Status & Ctr_STATUS_BIT_PENDING)
     {
      TxtClassNormal = "DAT_LIGHT";
      TxtClassStrong = "BT_LINK LT DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "BT_LINK LT DAT_N";
     }
   BgColor = (Ctr->CtrCod == Gbl.Hierarchy.Ctr.CtrCod) ? "LIGHT_BLUE" :
                                                         Gbl.ColorRows[Gbl.RowEvenOdd];

   HTM_TR_Begin (NULL);

   /***** Number of center in this list *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (NumCtr);
   HTM_TD_End ();

   /***** Center logo and name *****/
   HTM_TD_Begin ("class=\"LM %s\"",BgColor);
   Ctr_DrawCenterLogoAndNameWithLink (Ctr,ActSeeDeg,
                                      TxtClassStrong,"CM");
   HTM_TD_End ();

   /***** Number of users who claim to belong to this center *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Usr_GetCachedNumUsrsWhoClaimToBelongToCtr (Ctr));
   HTM_TD_End ();

   /***** Place *****/
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   HTM_Txt (Plc.ShrtName);
   HTM_TD_End ();

   /***** Number of degrees *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Deg_GetCachedNumDegsInCtr (Ctr->CtrCod));
   HTM_TD_End ();

   /***** Number of courses *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Crs_GetCachedNumCrssInCtr (Ctr->CtrCod));
   HTM_TD_End ();

   /***** Number of users in courses of this center *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Usr_GetCachedNumUsrsInCrss (HieLvl_CTR,Ctr->CtrCod,
					     1 << Rol_STD |
					     1 << Rol_NET |
					     1 << Rol_TCH));	// Any user
   HTM_TD_End ();

   /***** Center status *****/
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   if (StatusTxt != Ctr_STATUS_ACTIVE) // If active ==> do not show anything
      HTM_Txt (Txt_CENTER_STATUS[StatusTxt]);
   HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of centers **********/
/*****************************************************************************/

static void Ctr_GetParamCtrOrder (void)
  {
   Gbl.Hierarchy.Ctrs.SelectedOrder = (Ctr_Order_t)
					  Par_GetParToUnsignedLong ("Order",
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

   /***** Reset places context *****/
   Plc_ResetPlaces (&Places);

   /***** Get list of places *****/
   Places.SelectedOrder = Plc_ORDER_BY_PLACE;
   Plc_GetListPlaces (&Places);

   /***** Get list of centers *****/
   Gbl.Hierarchy.Ctrs.SelectedOrder = Ctr_ORDER_BY_CENTER;
   Ctr_GetFullListOfCenters (Gbl.Hierarchy.Ins.InsCod);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Centers_of_INSTITUTION_X,
				          Gbl.Hierarchy.Ins.FullName),
		 Ctr_PutIconsEditingCenters,NULL,
                 Hlp_INSTITUTION_Centers,Box_NOT_CLOSABLE);
   Str_FreeString ();

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
   Ctr_PutIconToViewCenters ();

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

void Ctr_PutIconToViewCenters (void)
  {
   extern const char *Txt_Centers;

   Lay_PutContextualLinkOnlyIcon (ActSeeCtr,NULL,
                                  NULL,NULL,
                                  "building.svg",
                                  Txt_Centers);
  }

/*****************************************************************************/
/************ Get basic list of centers ordered by name of center ************/
/*****************************************************************************/

void Ctr_GetBasicListOfCenters (long InsCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtr;
   struct Ctr_Center *Ctr;

   /***** Get centers from database *****/
   Gbl.Hierarchy.Ctrs.Num = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get centers",
		   "SELECT CtrCod,"		// row[ 0]
			  "InsCod,"		// row[ 1]
			  "PlcCod,"		// row[ 2]
			  "Status,"		// row[ 3]
			  "RequesterUsrCod,"	// row[ 4]
			  "Latitude,"		// row[ 5]
			  "Longitude,"		// row[ 6]
			  "Altitude,"		// row[ 7]
			  "ShortName,"		// row[ 8]
			  "FullName,"		// row[ 9]
			  "WWW"			// row[10]
		    " FROM ctr_centers"
		   " WHERE InsCod=%ld"
		   " ORDER BY FullName",
		   InsCod);

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
        {
         Ctr = &(Gbl.Hierarchy.Ctrs.Lst[NumCtr]);

         /* Get center data */
         row = mysql_fetch_row (mysql_res);
         Ctr_GetDataOfCenterFromRow (Ctr,row);

	 /* Reset number of users who claim to belong to this center */
         Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = false;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get full list of centers                         **************/
/************* with number of users who claim to belong to them **************/
/*****************************************************************************/

void Ctr_GetFullListOfCenters (long InsCod)
  {
   static const char *OrderBySubQuery[Ctr_NUM_ORDERS] =
     {
      [Ctr_ORDER_BY_CENTER  ] = "FullName",
      [Ctr_ORDER_BY_NUM_USRS] = "NumUsrs DESC,FullName",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtr;
   struct Ctr_Center *Ctr;

   /***** Get centers from database *****/
   Gbl.Hierarchy.Ctrs.Num = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get centers",
		   "(SELECT ctr_centers.CtrCod,"		// row[ 0]
			   "ctr_centers.InsCod,"		// row[ 1]
			   "ctr_centers.PlcCod,"		// row[ 2]
			   "ctr_centers.Status,"		// row[ 3]
			   "ctr_centers.RequesterUsrCod,"	// row[ 4]
			   "ctr_centers.Latitude,"		// row[ 5]
			   "ctr_centers.Longitude,"		// row[ 6]
			   "ctr_centers.Altitude,"		// row[ 7]
			   "ctr_centers.ShortName,"		// row[ 8]
			   "ctr_centers.FullName,"		// row[ 9]
			   "ctr_centers.WWW,"			// row[10]
			   "COUNT(*) AS NumUsrs"		// row[11]
		    " FROM ctr_centers,usr_data"
		   " WHERE ctr_centers.InsCod=%ld"
		     " AND ctr_centers.CtrCod=usr_data.CtrCod"
		   " GROUP BY ctr_centers.CtrCod)"
		   " UNION "
		   "(SELECT CtrCod,"				// row[ 0]
			   "InsCod,"				// row[ 1]
			   "PlcCod,"				// row[ 2]
			   "Status,"				// row[ 3]
			   "RequesterUsrCod,"			// row[ 4]
			   "Latitude,"				// row[ 5]
			   "Longitude,"				// row[ 6]
			   "Altitude,"				// row[ 7]
			   "ShortName,"				// row[ 8]
			   "FullName,"				// row[ 9]
			   "WWW,"				// row[10]
			   "0 AS NumUsrs"			// row[11]
		    " FROM ctr_centers"
		   " WHERE InsCod=%ld"
		     " AND CtrCod NOT IN"
		         " (SELECT DISTINCT CtrCod"
			    " FROM usr_data))"
		   " ORDER BY %s",
		   InsCod,
		   InsCod,
		   OrderBySubQuery[Gbl.Hierarchy.Ctrs.SelectedOrder]);

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
        {
         Ctr = &(Gbl.Hierarchy.Ctrs.Lst[NumCtr]);

         /* Get center data */
         row = mysql_fetch_row (mysql_res);
         Ctr_GetDataOfCenterFromRow (Ctr,row);

	 /* Get number of users who claim to belong to this center (row[11]) */
         Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = false;
	 if (sscanf (row[11],"%u",&(Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs)) == 1)
	    Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = true;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get data of center by code *************************/
/*****************************************************************************/

bool Ctr_GetDataOfCenterByCod (struct Ctr_Center *Ctr)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool CtrFound = false;

   /***** Clear data *****/
   Ctr->InsCod          = -1L;
   Ctr->PlcCod          = -1L;
   Ctr->Status          = (Ctr_Status_t) 0;
   Ctr->RequesterUsrCod = -1L;
   Ctr->ShrtName[0]     = '\0';
   Ctr->FullName[0]     = '\0';
   Ctr->WWW[0]          = '\0';
   Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = false;

   /***** Check if center code is correct *****/
   if (Ctr->CtrCod > 0)
     {
      /***** Get data of a center from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of a center",
			  "SELECT CtrCod,"		// row[ 0]
				 "InsCod,"		// row[ 1]
				 "PlcCod,"		// row[ 2]
				 "Status,"		// row[ 3]
				 "RequesterUsrCod,"	// row[ 4]
				 "Latitude,"		// row[ 5]
				 "Longitude,"		// row[ 6]
				 "Altitude,"		// row[ 7]
				 "ShortName,"		// row[ 8]
				 "FullName,"		// row[ 9]
				 "WWW"			// row[10]
			   " FROM ctr_centers"
			  " WHERE CtrCod=%ld",
			  Ctr->CtrCod)) // Center found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);
         Ctr_GetDataOfCenterFromRow (Ctr,row);

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

static void Ctr_GetDataOfCenterFromRow (struct Ctr_Center *Ctr,MYSQL_ROW row)
  {
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
  }

/*****************************************************************************/
/*********** Get the institution code of a center from its code **************/
/*****************************************************************************/

long Ctr_GetInsCodOfCenterByCod (long CtrCod)
  {
   /***** Trivial check: center code should be > 0 *****/
   if (CtrCod <= 0)
      return -1L;

   /***** Get the institution code of a center from database *****/
   return DB_QuerySELECTCode ("can not get the institution of a center",
			      "SELECT InsCod"
			       " FROM ctr_centers"
			      " WHERE CtrCod=%ld",
			      CtrCod);
  }

/*****************************************************************************/
/*************** Get the short name of a center from its code ****************/
/*****************************************************************************/

void Ctr_GetShortNameOfCenterByCod (struct Ctr_Center *Ctr)
  {
   /***** Trivial check: center code should be > 0 *****/
   if (Ctr->CtrCod <= 0)
     {
      Ctr->ShrtName[0] = '\0';
      return;
     }

   /***** Get the short name of a center from database *****/
   DB_QuerySELECTString (Ctr->ShrtName,sizeof (Ctr->ShrtName) - 1,
			 "can not get the short name of a center",
		         "SELECT ShortName"
			  " FROM ctr_centers"
		         " WHERE CtrCod=%ld",
		         Ctr->CtrCod);
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

   if (Gbl.Hierarchy.Ins.InsCod > 0)
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			"id=\"ctr\" name=\"ctr\" class=\"HIE_SEL\"");
   else
      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"id=\"ctr\" name=\"ctr\" class=\"HIE_SEL\""
			" disabled=\"disabled\"");
   HTM_OPTION (HTM_Type_STRING,"",
	       Gbl.Hierarchy.Ctr.CtrCod < 0,true,
	       "[%s]",Txt_Center);

   if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      /***** Get centers from database *****/
      NumCtrs = (unsigned)
      DB_QuerySELECT (&mysql_res,"can not get centers",
		      "SELECT DISTINCT CtrCod,"		// row[0]
		                      "ShortName"	// row[1]
		       " FROM ctr_centers"
		      " WHERE InsCod=%ld"
		      " ORDER BY ShortName",
		      Gbl.Hierarchy.Ins.InsCod);

      /***** Get centers *****/
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
                     CtrCod == Gbl.Hierarchy.Ctr.CtrCod,false,
		     "%s",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   HTM_SELECT_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** List all the centers ****************************/
/*****************************************************************************/

static void Ctr_ListCentersForEdition (const struct Plc_Places *Places)
  {
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTER_STATUS[Ctr_NUM_STATUS_TXT];
   unsigned NumCtr;
   struct Ctr_Center *Ctr;
   unsigned NumPlc;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct UsrData UsrDat;
   bool ICanEdit;
   unsigned NumDegs;
   unsigned NumUsrsCtr;
   unsigned NumUsrsInCrssOfCtr;
   Ctr_StatusTxt_t StatusTxt;
   unsigned StatusUnsigned;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Ctr_PutHeadCentersForEdition ();

   /***** Write all the centers *****/
   for (NumCtr = 0;
	NumCtr < Gbl.Hierarchy.Ctrs.Num;
	NumCtr++)
     {
      Ctr = &Gbl.Hierarchy.Ctrs.Lst[NumCtr];

      ICanEdit = Ctr_CheckIfICanEditACenter (Ctr);
      NumDegs = Deg_GetNumDegsInCtr (Ctr->CtrCod);
      NumUsrsCtr = Usr_GetNumUsrsWhoClaimToBelongToCtr (Ctr);
      NumUsrsInCrssOfCtr = Usr_GetNumUsrsInCrss (HieLvl_CTR,Ctr->CtrCod,
						 1 << Rol_STD |
						 1 << Rol_NET |
						 1 << Rol_TCH);	// Any user

      /* Put icon to remove center */
      HTM_TR_Begin (NULL);
      HTM_TD_Begin ("class=\"BM\"");
      if (!ICanEdit ||				// I cannot edit
	  NumDegs ||				// Center has degrees
	  NumUsrsCtr ||				// Center has users who claim to belong to it
	  NumUsrsInCrssOfCtr)			// Center has users
	 Ico_PutIconRemovalNotAllowed ();
      else	// I can remove center
	 Ico_PutContextualIconToRemove (ActRemCtr,NULL,
					Ctr_PutParamOtherCtrCod,&Ctr->CtrCod);
      HTM_TD_End ();

      /* Center code */
      HTM_TD_Begin ("class=\"DAT CODE\"");
      HTM_Long (Ctr->CtrCod);
      HTM_TD_End ();

      /* Center logo */
      HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr->FullName);
      Lgo_DrawLogo (HieLvl_CTR,Ctr->CtrCod,Ctr->ShrtName,20,NULL,true);
      HTM_TD_End ();

      /* Place */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActChgCtrPlc);
	 Ctr_PutParamOtherCtrCod (&Ctr->CtrCod);
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "name=\"PlcCod\" class=\"PLC_SEL\"");
	 HTM_OPTION (HTM_Type_STRING,"0",
		     Ctr->PlcCod == 0,false,
		     "%s",Txt_Another_place);
	 for (NumPlc = 0;
	      NumPlc < Places->Num;
	      NumPlc++)
	    HTM_OPTION (HTM_Type_LONG,&Places->Lst[NumPlc].PlcCod,
			Places->Lst[NumPlc].PlcCod == Ctr->PlcCod,false,
			"%s",Places->Lst[NumPlc].ShrtName);
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
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActRenCtrSho);
	 Ctr_PutParamOtherCtrCod (&Ctr->CtrCod);
	 HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ctr->ShrtName,
	                 HTM_SUBMIT_ON_CHANGE,
			 "class=\"INPUT_SHORT_NAME\"");
	 Frm_EndForm ();
	}
      else
	 HTM_Txt (Ctr->ShrtName);
      HTM_TD_End ();

      /* Center full name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActRenCtrFul);
	 Ctr_PutParamOtherCtrCod (&Ctr->CtrCod);
	 HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ctr->FullName,
	                 HTM_SUBMIT_ON_CHANGE,
			 "class=\"INPUT_FULL_NAME\"");
	 Frm_EndForm ();
	}
      else
	 HTM_Txt (Ctr->FullName);
      HTM_TD_End ();

      /* Center WWW */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActChgCtrWWW);
	 Ctr_PutParamOtherCtrCod (&Ctr->CtrCod);
	 HTM_INPUT_URL ("WWW",Ctr->WWW,HTM_SUBMIT_ON_CHANGE,
			"class=\"INPUT_WWW_NARROW\" required=\"required\"");
	 Frm_EndForm ();
	}
      else
	{
         Str_Copy (WWW,Ctr->WWW,sizeof (WWW) - 1);
         HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHORT\"");
         HTM_A_Begin ("href=\"%s\" target=\"_blank\""
                      " class=\"DAT\" title=\"%s\"",Ctr->WWW,Ctr->WWW);
         HTM_Txt (WWW);
         HTM_A_End ();
         HTM_DIV_End ();
	}
      HTM_TD_End ();

      /* Number of users who claim to belong to this center */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (NumUsrsCtr);
      HTM_TD_End ();

      /* Number of degrees */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (NumDegs);
      HTM_TD_End ();

      /* Number of users in courses of this center */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (NumUsrsInCrssOfCtr);
      HTM_TD_End ();

      /* Center requester */
      UsrDat.UsrCod = Ctr->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                               Usr_DONT_GET_PREFS,
                                               Usr_DONT_GET_ROLE_IN_CURRENT_CRS);
      HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
      Msg_WriteMsgAuthor (&UsrDat,true,NULL);
      HTM_TD_End ();

      /* Center status */
      StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM &&
	  StatusTxt == Ctr_STATUS_PENDING)
	{
	 Frm_BeginForm (ActChgCtrSta);
	 Ctr_PutParamOtherCtrCod (&Ctr->CtrCod);
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "name=\"Status\" class=\"INPUT_STATUS\"");

	 StatusUnsigned = (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_PENDING);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,true,false,
		     "%s",Txt_CENTER_STATUS[Ctr_STATUS_PENDING]);

	 StatusUnsigned = (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_ACTIVE);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,false,false,
		     "%s",Txt_CENTER_STATUS[Ctr_STATUS_ACTIVE]);

	 HTM_SELECT_End ();
	 Frm_EndForm ();
	}
      else if (StatusTxt != Ctr_STATUS_ACTIVE)	// If active ==> do not show anything
	 HTM_Txt (Txt_CENTER_STATUS[StatusTxt]);
      HTM_TD_End ();
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
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM ||		// I am an institution administrator or higher
                  ((Ctr->Status & Ctr_STATUS_BIT_PENDING) != 0 &&	// Center is not yet activated
                   Gbl.Usrs.Me.UsrDat.UsrCod == Ctr->RequesterUsrCod));	// I am the requester
  }

/*****************************************************************************/
/******************* Set StatusTxt depending on status bits ******************/
/*****************************************************************************/
// Ctr_STATUS_UNKNOWN = 0	// Other
// Ctr_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Ctr_STATUS_PENDING = 2	// 01 (Status == Ctr_STATUS_BIT_PENDING)
// Ctr_STATUS_REMOVED = 3	// 1- (Status & Ctr_STATUS_BIT_REMOVED)

static Ctr_StatusTxt_t Ctr_GetStatusTxtFromStatusBits (Ctr_Status_t Status)
  {
   if (Status == 0)
      return Ctr_STATUS_ACTIVE;
   if (Status == Ctr_STATUS_BIT_PENDING)
      return Ctr_STATUS_PENDING;
   if (Status & Ctr_STATUS_BIT_REMOVED)
      return Ctr_STATUS_REMOVED;
   return Ctr_STATUS_UNKNOWN;
  }

/*****************************************************************************/
/******************* Set status bits depending on StatusTxt ******************/
/*****************************************************************************/
// Ctr_STATUS_UNKNOWN = 0	// Other
// Ctr_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Ctr_STATUS_PENDING = 2	// 01 (Status == Ctr_STATUS_BIT_PENDING)
// Ctr_STATUS_REMOVED = 3	// 1- (Status & Ctr_STATUS_BIT_REMOVED)

static Ctr_Status_t Ctr_GetStatusBitsFromStatusTxt (Ctr_StatusTxt_t StatusTxt)
  {
   switch (StatusTxt)
     {
      case Ctr_STATUS_UNKNOWN:
      case Ctr_STATUS_ACTIVE:
	 return (Ctr_Status_t) 0;
      case Ctr_STATUS_PENDING:
	 return Ctr_STATUS_BIT_PENDING;
      case Ctr_STATUS_REMOVED:
	 return Ctr_STATUS_BIT_REMOVED;
     }
   return (Ctr_Status_t) 0;
  }

/*****************************************************************************/
/******************** Write parameter with code of center ********************/
/*****************************************************************************/

void Ctr_PutParamCtrCod (long CtrCod)
  {
   Par_PutHiddenParamLong (NULL,"ctr",CtrCod);
  }

/*****************************************************************************/
/***************** Write parameter with code of other center *****************/
/*****************************************************************************/

static void Ctr_PutParamOtherCtrCod (void *CtrCod)
  {
   if (CtrCod)
      Par_PutHiddenParamLong (NULL,"OthCtrCod",*((long *) CtrCod));
  }

/*****************************************************************************/
/****************** Get parameter with code of other center ******************/
/*****************************************************************************/

long Ctr_GetAndCheckParamOtherCtrCod (long MinCodAllowed)
  {
   long CtrCod;

   /***** Get and check parameter with code of center *****/
   if ((CtrCod = Par_GetParToLong ("OthCtrCod")) < MinCodAllowed)
      Err_WrongCenterExit ();

   return CtrCod;
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
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get data of the center from database *****/
   Ctr_GetDataOfCenterByCod (Ctr_EditingCtr);

   /***** Check if this center has teachers *****/
   if (Deg_GetNumDegsInCtr (Ctr_EditingCtr->CtrCod))			// Center has degrees
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else if (Usr_GetNumUsrsWhoClaimToBelongToCtr (Ctr_EditingCtr))	// Center has users who claim to belong to it
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else if (Usr_GetNumUsrsInCrss (HieLvl_CTR,Ctr_EditingCtr->CtrCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))			// Center has users
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_center_you_must_first_remove_all_degrees_and_teachers_in_the_center);
   else	// Center has no degrees or users ==> remove it
     {
      /***** Remove all the threads and posts in forums of the center *****/
      For_RemoveForums (HieLvl_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove surveys of the center *****/
      Svy_RemoveSurveys (HieLvl_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove information related to files in center *****/
      Brw_DB_RemoveCtrFiles (Ctr_EditingCtr->CtrCod);

      /***** Remove all rooms in center *****/
      Roo_RemoveAllRoomsInCtr (Ctr_EditingCtr->CtrCod);

      /***** Remove directories of the center *****/
      snprintf (PathCtr,sizeof (PathCtr),"%s/%02u/%u",
	        Cfg_PATH_CTR_PUBLIC,
	        (unsigned) (Ctr_EditingCtr->CtrCod % 100),
	        (unsigned)  Ctr_EditingCtr->CtrCod);
      Fil_RemoveTree (PathCtr);

      /***** Remove administrators of this center *****/
      Enr_DB_RemAdmins (HieLvl_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove center *****/
      DB_QueryDELETE ("can not remove a center",
		      "DELETE FROM ctr_centers"
		      " WHERE CtrCod=%ld",
		      Ctr_EditingCtr->CtrCod);

      /***** Flush caches *****/
      Deg_FlushCacheNumDegsInCtr ();
      Crs_FlushCacheNumCrssInCtr ();
      Usr_FlushCacheNumUsrsWhoClaimToBelongToCtr ();

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
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get parameter with place code *****/
   NewPlcCod = Plc_GetParamPlcCod ();

   /***** Get data of center from database *****/
   Ctr_GetDataOfCenterByCod (Ctr_EditingCtr);

   /***** Update place in table of centers *****/
   Ctr_UpdateCtrPlcDB (Ctr_EditingCtr->CtrCod,NewPlcCod);
   Ctr_EditingCtr->PlcCod = NewPlcCod;

   /***** Create alert to show the change made
	  and put button to go to center changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_place_of_the_center_has_changed);
  }

/*****************************************************************************/
/************** Update database changing old place by new place **************/
/*****************************************************************************/

void Ctr_UpdateCtrPlcDB (long CtrCod,long NewPlcCod)
  {
   DB_QueryUPDATE ("can not update the place of a center",
		   "UPDATE ctr_centers"
		     " SET PlcCod=%ld"
		   " WHERE CtrCod=%ld",
	           NewPlcCod,
	           CtrCod);
  }

/*****************************************************************************/
/************************ Change the name of a center ************************/
/*****************************************************************************/

void Ctr_RenameCenterShort (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Rename center *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);
   Ctr_RenameCenter (Ctr_EditingCtr,Cns_SHRT_NAME);
  }

void Ctr_RenameCenterFull (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Rename center *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);
   Ctr_RenameCenter (Ctr_EditingCtr,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a center ************************/
/*****************************************************************************/

void Ctr_RenameCenter (struct Ctr_Center *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_center_X_already_exists;
   extern const char *Txt_The_center_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_center_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentCtrName = NULL;		// Initialized to avoid warning
   char NewCtrName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_SHRT_NAME;
         CurrentCtrName = Ctr->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_FULL_NAME;
         CurrentCtrName = Ctr->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the new name for the center */
   Par_GetParToText (ParamName,NewCtrName,MaxBytes);

   /***** Get from the database the old names of the center *****/
   Ctr_GetDataOfCenterByCod (Ctr);

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
         if (Ctr_CheckIfCtrNameExistsInIns (ParamName,NewCtrName,Ctr->CtrCod,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_center_X_already_exists,
			     NewCtrName);
         else
           {
            /* Update the table changing old name by new name */
            Ctr_UpdateInsNameDB (Ctr->CtrCod,FieldName,NewCtrName);

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
                          Txt_The_name_of_the_center_X_has_not_changed,
                          CurrentCtrName);
     }
  }

/*****************************************************************************/
/********************* Check if the name of center exists ********************/
/*****************************************************************************/

bool Ctr_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,
				    long CtrCod,long InsCod)
  {
   /***** Get number of centers with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a center"
			  " already existed",
			  "SELECT COUNT(*)"
			   " FROM ctr_centers"
			  " WHERE InsCod=%ld"
			    " AND %s='%s'"
			    " AND CtrCod<>%ld",
			  InsCod,
			  FieldName,
			  Name,
			  CtrCod) != 0);
  }

/*****************************************************************************/
/****************** Update center name in table of centers *******************/
/*****************************************************************************/

static void Ctr_UpdateInsNameDB (long CtrCod,const char *FieldName,const char *NewCtrName)
  {
   /***** Update center changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a center",
		   "UPDATE ctr_centers"
		     " SET %s='%s'"
		   " WHERE CtrCod=%ld",
	           FieldName,
	           NewCtrName,
	           CtrCod);
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
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get the new WWW for the center *****/
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of center *****/
   Ctr_GetDataOfCenterByCod (Ctr_EditingCtr);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_UpdateCtrWWWDB (Ctr_EditingCtr->CtrCod,NewWWW);
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
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

void Ctr_UpdateCtrWWWDB (long CtrCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   /***** Update database changing old WWW by new WWW *****/
   DB_QueryUPDATE ("can not update the web of a center",
		   "UPDATE ctr_centers"
		     " SET WWW='%s'"
		   " WHERE CtrCod=%ld",
	           NewWWW,
	           CtrCod);
  }

/*****************************************************************************/
/*********************** Change the status of a center ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrStatus (void)
  {
   extern const char *Txt_The_status_of_the_center_X_has_changed;
   Ctr_Status_t Status;
   Ctr_StatusTxt_t StatusTxt;

   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Get center code *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get parameter with status *****/
   Status = (Ctr_Status_t)
	    Par_GetParToUnsignedLong ("Status",
				      0,
				      (unsigned long) Ctr_MAX_STATUS,
				      (unsigned long) Ctr_WRONG_STATUS);
   if (Status == Ctr_WRONG_STATUS)
      Err_WrongStatusExit ();
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Status);
   Status = Ctr_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of center *****/
   Ctr_GetDataOfCenterByCod (Ctr_EditingCtr);

   /***** Update status in table of centers *****/
   DB_QueryUPDATE ("can not update the status of a center",
		   "UPDATE ctr_centers"
		     " SET Status=%u"
		   " WHERE CtrCod=%ld",
	           (unsigned) Status,
	           Ctr_EditingCtr->CtrCod);
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
                                  Ctr_PutParamGoToCtr,&Ctr_EditingCtr->CtrCod,
                                  Btn_CONFIRM_BUTTON,
                                  Hie_BuildGoToMsg (Ctr_EditingCtr->ShrtName));
      Hie_FreeGoToMsg ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Ctr_PutParamGoToCtr (void *CtrCod)
  {
   if (CtrCod)
      Ctr_PutParamCtrCod (*((long *) CtrCod));
  }

/*****************************************************************************/
/********************* Put a form to create a new center *********************/
/*****************************************************************************/

static void Ctr_PutFormToCreateCenter (const struct Plc_Places *Places)
  {
   extern const char *Txt_New_center;
   extern const char *Txt_Another_place;
   extern const char *Txt_Create_center;
   unsigned NumPlc;

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      Frm_BeginForm (ActNewCtr);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_BeginForm (ActReqCtr);
   else
      Err_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_center,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

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
   Lgo_DrawLogo (HieLvl_CTR,-1L,"",20,NULL,true);
   HTM_TD_End ();

   /***** Place *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"PlcCod\" class=\"PLC_SEL\"");
   HTM_OPTION (HTM_Type_STRING,"0",
	       Ctr_EditingCtr->PlcCod == 0,false,
	       "%s",Txt_Another_place);
   for (NumPlc = 0;
	NumPlc < Places->Num;
	NumPlc++)
      HTM_OPTION (HTM_Type_LONG,&Places->Lst[NumPlc].PlcCod,
		  Places->Lst[NumPlc].PlcCod == Ctr_EditingCtr->PlcCod,false,
		  "%s",Places->Lst[NumPlc].ShrtName);
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Center short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Ctr_EditingCtr->ShrtName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Center full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Ctr_EditingCtr->FullName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Center WWW *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_URL ("WWW",Ctr_EditingCtr->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
		  "class=\"INPUT_WWW_NARROW\" required=\"required\"");
   HTM_TD_End ();

   /***** Number of users who claim to belong to this center *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Number of degrees *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Number of users in courses of this center *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Center requester *****/
   HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,true,NULL);
   HTM_TD_End ();

   /***** Center status *****/
   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_center);

   /***** End form *****/
   Frm_EndForm ();
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

   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   for (Order  = (Ctr_Order_t) 0;
	Order <= (Ctr_Order_t) (Ctr_NUM_ORDERS - 1);
	Order++)
     {
      HTM_TH_Begin (1,1,Order == Ctr_ORDER_BY_CENTER ? "LM" :
				                       "RM");
      if (OrderSelectable)
	{
	 Frm_BeginForm (ActSeeCtr);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_CENTERS_HELP_ORDER[Order],
				  Order == Ctr_ORDER_BY_CENTER ? "BT_LINK LM TIT_TBL" :
					                         "BT_LINK RM TIT_TBL",
				  NULL);
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

   HTM_TH (1,1,"LM",Txt_Place);
   HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);
   HTM_TH (1,1,"RM",Txt_Courses_ABBREVIATION);
   HTM_TH_Begin (1,1,"RM");
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
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH_Empty (1);
   HTM_TH (1,1,"LM",Txt_Place);
   HTM_TH (1,1,"LM",Txt_Short_name_of_the_center);
   HTM_TH (1,1,"LM",Txt_Full_name_of_the_center);
   HTM_TH (1,1,"LM",Txt_WWW);
   HTM_TH (1,1,"RM",Txt_Users);
   HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);
   HTM_TH_Begin (1,1,"RM");
   HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
   HTM_BR ();
   HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH_End ();
   HTM_TH (1,1,"LM",Txt_Requester);
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
   Ctr_ReceiveFormRequestOrCreateCtr ((unsigned) Ctr_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new center *********************/
/*****************************************************************************/

void Ctr_ReceiveFormNewCtr (void)
  {
   /***** Center constructor *****/
   Ctr_EditingCenterConstructor ();

   /***** Receive form to create a new center *****/
   Ctr_ReceiveFormRequestOrCreateCtr (0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new center ****************/
/*****************************************************************************/

static void Ctr_ReceiveFormRequestOrCreateCtr (unsigned Status)
  {
   extern const char *Txt_The_center_X_already_exists;
   extern const char *Txt_Created_new_center_X;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_center;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_center;

   /***** Get parameters from form *****/
   /* Set center institution */
   Ctr_EditingCtr->InsCod = Gbl.Hierarchy.Ins.InsCod;

   /* Get place */
   if ((Ctr_EditingCtr->PlcCod = Plc_GetParamPlcCod ()) < 0)	// 0 is reserved for "other place"
      Ale_ShowAlert (Ale_ERROR,"Wrong place.");

   /* Get center short name */
   Par_GetParToText ("ShortName",Ctr_EditingCtr->ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);

   /* Get center full name */
   Par_GetParToText ("FullName",Ctr_EditingCtr->FullName,Cns_HIERARCHY_MAX_BYTES_FULL_NAME);

   /* Get center WWW */
   Par_GetParToText ("WWW",Ctr_EditingCtr->WWW,Cns_MAX_BYTES_WWW);

   if (Ctr_EditingCtr->ShrtName[0] &&
       Ctr_EditingCtr->FullName[0])	// If there's a center name
     {
      if (Ctr_EditingCtr->WWW[0])
        {
         /***** If name of center was in database... *****/
         if (Ctr_CheckIfCtrNameExistsInIns ("ShortName",Ctr_EditingCtr->ShrtName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_center_X_already_exists,
                             Ctr_EditingCtr->ShrtName);
         else if (Ctr_CheckIfCtrNameExistsInIns ("FullName",Ctr_EditingCtr->FullName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        		     Txt_The_center_X_already_exists,
                             Ctr_EditingCtr->FullName);
         else	// Add new center to database
           {
            Ctr_CreateCenter (Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_center_X,
			     Ctr_EditingCtr->FullName);
           }
        }
      else	// If there is not a web
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_web_address_of_the_new_center);
     }
   else	// If there is not a center name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_center);
  }

/*****************************************************************************/
/***************************** Create a new center ***************************/
/*****************************************************************************/

static void Ctr_CreateCenter (unsigned Status)
  {
   /***** Create a new center *****/
   Ctr_EditingCtr->CtrCod =
   DB_QueryINSERTandReturnCode ("can not create a new center",
				"INSERT INTO ctr_centers"
				" (InsCod,PlcCod,Status,RequesterUsrCod,"
				  "ShortName,FullName,WWW,PhotoAttribution)"
				" VALUES"
				" (%ld,%ld,%u,%ld,"
				  "'%s','%s','%s','')",
				Ctr_EditingCtr->InsCod,
				Ctr_EditingCtr->PlcCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Ctr_EditingCtr->ShrtName,
				Ctr_EditingCtr->FullName,
				Ctr_EditingCtr->WWW);
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
   Gbl.Cache.NumCtrsInCty.NumCtrs = (unsigned)
   DB_QueryCOUNT ("can not get number of centers in a country",
		  "SELECT COUNT(*)"
		   " FROM ins_instits,"
		         "ctr_centers"
		  " WHERE ins_instits.CtyCod=%ld"
		    " AND ins_instits.InsCod=ctr_centers.InsCod",
		  CtyCod);
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
   Gbl.Cache.NumCtrsInIns.NumCtrs = (unsigned)
   DB_QueryCOUNT ("can not get number of centers in an institution",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers"
		  " WHERE InsCod=%ld",
		  InsCod);
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
      NumCtrsWithMap = (unsigned)
      DB_QueryCOUNT ("can not get number of centers with map",
		     "SELECT COUNT(*)"
		      " FROM ctr_centers"
		     " WHERE Latitude<>0"
		        " OR Longitude<>0");
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
      NumCtrsWithMap = (unsigned)
      DB_QueryCOUNT ("can not get number of centers with map",
		     "SELECT COUNT(*)"
		      " FROM ins_instits,"
		            "ctr_centers"
		     " WHERE ins_instits.CtyCod=%ld"
		       " AND ins_instits.InsCod=ctr_centers.InsCod"
		       " AND (ctr_centers.Latitude<>0"
		         " OR ctr_centers.Longitude<>0)",
		     CtyCod);
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
      NumCtrsWithMap = (unsigned)
      DB_QueryCOUNT ("can not get number of centers with map",
		     "SELECT COUNT(*)"
		      " FROM ctr_centers"
		     " WHERE InsCod=%ld"
		       " AND (Latitude<>0"
		         " OR Longitude<>0)",
		     InsCod);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_MAP,HieLvl_INS,InsCod,
                                    FigCch_UNSIGNED,&NumCtrsWithMap);
     }

   return NumCtrsWithMap;
  }

/*****************************************************************************/
/******* Get number of centers (of the current institution) in a place *******/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInPlc (long PlcCod)
  {
   /***** Get number of centers (of the current institution) in a place *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get the number of centers in a place",
		  "SELECT COUNT(*)"
		   " FROM ctr_centers"
		  " WHERE InsCod=%ld"
		    " AND PlcCod=%ld",
		  Gbl.Hierarchy.Ins.InsCod,
		  PlcCod);
  }

/*****************************************************************************/
/********************* Get number of centers with degrees ********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithDegs (const char *SubQuery,
                                       HieLvl_Level_t Scope,long Cod)
  {
   unsigned NumCtrsWithDegs;

   /***** Get number of centers with degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_DEGS,Scope,Cod,
				   FigCch_UNSIGNED,&NumCtrsWithDegs))
     {
      /***** Get current number of centers with degrees from database and update cache *****/
      NumCtrsWithDegs = (unsigned)
      DB_QueryCOUNT ("can not get number of centers with degrees",
		     "SELECT COUNT(DISTINCT ctr_centers.CtrCod)"
		      " FROM ins_instits,"
		            "ctr_centers,"
		            "deg_degrees"
		     " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		       " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		     SubQuery);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_DEGS,Scope,Cod,
				    FigCch_UNSIGNED,&NumCtrsWithDegs);
     }

   return NumCtrsWithDegs;
  }

/*****************************************************************************/
/********************* Get number of centers with courses ********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithCrss (const char *SubQuery,
                                       HieLvl_Level_t Scope,long Cod)
  {
   unsigned NumCtrsWithCrss;

   /***** Get number of centers with courses *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_CTRS_WITH_CRSS,Scope,Cod,
				   FigCch_UNSIGNED,&NumCtrsWithCrss))
     {
      /***** Get number of centers with courses *****/
      NumCtrsWithCrss = (unsigned)
      DB_QueryCOUNT ("can not get number of centers with courses",
		     "SELECT COUNT(DISTINCT ctr_centers.CtrCod)"
		      " FROM ins_instits,"
			    "ctr_centers,"
			    "deg_degrees,"
			    "crs_courses"
		     " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		       " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		       " AND deg_degrees.DegCod=crs_courses.DegCod",
		     SubQuery);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_CTRS_WITH_CRSS,Scope,Cod,
				    FigCch_UNSIGNED,&NumCtrsWithCrss);
     }

   return NumCtrsWithCrss;
  }

/*****************************************************************************/
/********************* Get number of centers with users **********************/
/*****************************************************************************/

unsigned Ctr_GetCachedNumCtrsWithUsrs (Rol_Role_t Role,const char *SubQuery,
                                       HieLvl_Level_t Scope,long Cod)
  {
   static const FigCch_FigureCached_t FigureCtrs[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_CTRS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_CTRS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_CTRS_WITH_TCHS,	// Teachers
     };
   unsigned NumCtrsWithUsrs;

   /***** Get number of centers with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureCtrs[Role],Scope,Cod,
				   FigCch_UNSIGNED,&NumCtrsWithUsrs))
     {
      /***** Get current number of centers with users from database and update cache *****/
      NumCtrsWithUsrs = (unsigned)
      DB_QueryCOUNT ("can not get number of centers with users",
		     "SELECT COUNT(DISTINCT ctr_centers.CtrCod)"
		      " FROM ins_instits,"
			    "ctr_centers,"
			    "deg_degrees,"
			    "crs_courses,"
			    "crs_users"
		     " WHERE %s"
		            "institutions.InsCod=ctr_centers.InsCod"
		       " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		       " AND deg_degrees.DegCod=crs_courses.DegCod"
		       " AND crs_courses.CrsCod=crs_users.CrsCod"
		       " AND crs_users.Role=%u",
		     SubQuery,(unsigned) Role);
      FigCch_UpdateFigureIntoCache (FigureCtrs[Role],Scope,Cod,
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
   struct Ctr_Center Ctr;

   /***** Query database *****/
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      /* Number of centers found */
      Box_BoxTableBegin (NULL,Str_BuildStringLongStr ((long) NumCtrs,
						      (NumCtrs == 1) ? Txt_center :
	                                                               Txt_centers),
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      Str_FreeString ();

      /***** Write heading *****/
      Ctr_PutHeadCentersForSeeing (false);	// Order not selectable

      /***** List the centers (one row per center) *****/
      for (NumCtr = 1;
	   NumCtr <= NumCtrs;
	   NumCtr++)
	{
	 /* Get next center */
	 Ctr.CtrCod = DB_GetNextCode (*mysql_res);

	 /* Get data of center */
	 Ctr_GetDataOfCenterByCod (&Ctr);

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
   Ctr_EditingCtr->Status          = (Ctr_Status_t) 0;
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
   extern const char *Txt_Map;

   if (Ctr_GetIfMapIsAvailable (Ctr))
     {
      Ctr_EditingCtr = Ctr;	// Used to pass parameter with the code of the center
      Lay_PutContextualLinkOnlyIcon (ActSeeCtrInf,NULL,
                                     Ctr_PutParamGoToCtr,&Ctr_EditingCtr->CtrCod,
				     "map-marker-alt.svg",
				     Txt_Map);
     }
  }

/*****************************************************************************/
/************************ Check if a center has map **************************/
/*****************************************************************************/

bool Ctr_GetIfMapIsAvailable (const struct Ctr_Center *Ctr)
  {
   /***** Coordinates 0, 0 means not set ==> don't show map *****/
   return (bool) (Ctr->Coord.Latitude ||
                  Ctr->Coord.Longitude);
  }
