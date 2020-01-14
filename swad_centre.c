// swad_centre.c: centres

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
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_centre.h"
#include "swad_centre_config.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
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

static struct Centre *Ctr_EditingCtr = NULL;	// Static variable to keep the centre being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ctr_ListCentres (void);
static bool Ctr_CheckIfICanCreateCentres (void);
static void Ctr_PutIconsListingCentres (void);
static void Ctr_PutIconToEditCentres (void);
static void Ctr_ListOneCentreForSeeing (struct Centre *Ctr,unsigned NumCtr);
static void Ctr_GetParamCtrOrder (void);

static void Ctr_EditCentresInternal (void);
static void Ctr_PutIconsEditingCentres (void);

static void Ctr_GetDataOfCentreFromRow (struct Centre *Ctr,MYSQL_ROW row);

static void Ctr_ListCentresForEdition (void);
static bool Ctr_CheckIfICanEditACentre (struct Centre *Ctr);
static Ctr_StatusTxt_t Ctr_GetStatusTxtFromStatusBits (Ctr_Status_t Status);
static Ctr_Status_t Ctr_GetStatusBitsFromStatusTxt (Ctr_StatusTxt_t StatusTxt);

static void Ctr_PutParamOtherCtrCod (long CtrCod);

static void Ctr_UpdateInsNameDB (long CtrCod,const char *FieldName,const char *NewCtrName);

static void Ctr_ShowAlertAndButtonToGoToCtr (void);
static void Ctr_PutParamGoToCtr (void);

static void Ctr_PutFormToCreateCentre (void);
static void Ctr_PutHeadCentresForSeeing (bool OrderSelectable);
static void Ctr_PutHeadCentresForEdition (void);
static void Ctr_RecFormRequestOrCreateCtr (unsigned Status);
static void Ctr_CreateCentre (unsigned Status);

static void Ctr_EditingCentreConstructor (void);
static void Ctr_EditingCentreDestructor (void);

/*****************************************************************************/
/******************* List centres with pending degrees ***********************/
/*****************************************************************************/

void Ctr_SeeCtrWithPendingDegs (void)
  {
   extern const char *Hlp_SYSTEM_Hierarchy_pending;
   extern const char *Txt_Centres_with_pending_degrees;
   extern const char *Txt_Centre;
   extern const char *Txt_Degrees_ABBREVIATION;
   extern const char *Txt_There_are_no_centres_with_requests_for_degrees_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   struct Centre Ctr;
   const char *BgColor;

   /***** Get centres with pending degrees *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_CTR_ADM:
         NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres"
							 " with pending degrees",
					      "SELECT degrees.CtrCod,COUNT(*)"
					      " FROM degrees,ctr_admin,centres"
					      " WHERE (degrees.Status & %u)<>0"
					      " AND degrees.CtrCod=ctr_admin.CtrCod"
					      " AND ctr_admin.UsrCod=%ld"
					      " AND degrees.CtrCod=centres.CtrCod"
					      " GROUP BY degrees.CtrCod ORDER BY centres.ShortName",
					      (unsigned) Deg_STATUS_BIT_PENDING,
					      Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Rol_SYS_ADM:
         NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres"
							 " with pending degrees",
					      "SELECT degrees.CtrCod,COUNT(*)"
					      " FROM degrees,centres"
					      " WHERE (degrees.Status & %u)<>0"
					      " AND degrees.CtrCod=centres.CtrCod"
					      " GROUP BY degrees.CtrCod ORDER BY centres.ShortName",
					      (unsigned) Deg_STATUS_BIT_PENDING);
         break;
      default:	// Forbidden for other users
	 return;
     }
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Centres_with_pending_degrees,NULL,
                         Hlp_SYSTEM_Hierarchy_pending,Box_NOT_CLOSABLE,2);

      /***** Wrtie heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"LM",Txt_Centre);
      HTM_TH (1,1,"RM",Txt_Degrees_ABBREVIATION);

      HTM_TR_End ();

      /***** List the centres *****/
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
        {
         /* Get next centre */
         row = mysql_fetch_row (mysql_res);

         /* Get centre code (row[0]) */
         Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Ctr.CtrCod == Gbl.Hierarchy.Ctr.CtrCod) ? "LIGHT_BLUE" :
                                                              Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of centre */
         Ctr_GetDataOfCentreByCod (&Ctr);

         /* Centre logo and full name */
         HTM_TR_Begin (NULL);

         HTM_TD_Begin ("class=\"LM %s\"",BgColor);
         Ctr_DrawCentreLogoAndNameWithLink (&Ctr,ActSeeDeg,
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
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_centres_with_requests_for_degrees_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Draw centre logo and name with link ********************/
/*****************************************************************************/

void Ctr_DrawCentreLogoAndNameWithLink (struct Centre *Ctr,Act_Action_t Action,
                                        const char *ClassLink,const char *ClassLogo)
  {
   /***** Begin form *****/
   Frm_StartFormGoTo (Action);
   Ctr_PutParamCtrCod (Ctr->CtrCod);

   /***** Link to action *****/
   HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Ctr->FullName),ClassLink,NULL);
   Hie_FreeGoToMsg ();

   /***** Centre logo and name *****/
   Lgo_DrawLogo (Hie_CTR,Ctr->CtrCod,Ctr->ShrtName,16,ClassLogo,true);
   HTM_TxtF ("&nbsp;%s",Ctr->FullName);

   /***** End link *****/
   HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************** Show the centres of the current institution *****************/
/*****************************************************************************/

void Ctr_ShowCtrsOfCurrentIns (void)
  {
   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ins.InsCod <= 0)		// No institution selected
      return;

   /***** Get parameter with the type of order in the list of centres *****/
   Ctr_GetParamCtrOrder ();

   /***** Get list of centres *****/
   Ctr_GetFullListOfCentres (Gbl.Hierarchy.Ins.InsCod);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** List centres *****/
   Ctr_ListCentres ();

   /***** Free list of centres *****/
   Ctr_FreeListCentres ();
  }

/*****************************************************************************/
/******************** List centres in this institution ***********************/
/*****************************************************************************/

static void Ctr_ListCentres (void)
  {
   extern const char *Hlp_INSTITUTION_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;
   extern const char *Txt_No_centres;
   extern const char *Txt_Create_another_centre;
   extern const char *Txt_Create_centre;
   unsigned NumCtr;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Centres_of_INSTITUTION_X,
				          Gbl.Hierarchy.Ins.FullName),
		 Ctr_PutIconsListingCentres,
                 Hlp_INSTITUTION_Centres,Box_NOT_CLOSABLE);
   Str_FreeString ();

   if (Gbl.Hierarchy.Ctrs.Num)	// There are centres in the current institution
     {
      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Ctr_PutHeadCentresForSeeing (true);	// Order selectable

      /***** Write all the centres and their nuber of teachers *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ctrs.Num;
	   NumCtr++)
	 Ctr_ListOneCentreForSeeing (&(Gbl.Hierarchy.Ctrs.Lst[NumCtr]),NumCtr + 1);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No centres created in the current institution
      Ale_ShowAlert (Ale_INFO,Txt_No_centres);

   /***** Button to create centre *****/
   if (Ctr_CheckIfICanCreateCentres ())
     {
      Frm_StartForm (ActEdiCtr);
      Btn_PutConfirmButton (Gbl.Hierarchy.Ctrs.Num ? Txt_Create_another_centre :
	                                                 Txt_Create_centre);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Check if I can create centres ************************/
/*****************************************************************************/

static bool Ctr_CheckIfICanCreateCentres (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_GST);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of centres *******************/
/*****************************************************************************/

static void Ctr_PutIconsListingCentres (void)
  {
   /***** Put icon to edit centres *****/
   if (Ctr_CheckIfICanCreateCentres ())
      Ctr_PutIconToEditCentres ();

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************** Put link (form) to edit centres **********************/
/*****************************************************************************/

static void Ctr_PutIconToEditCentres (void)
  {
   Ico_PutContextualIconToEdit (ActEdiCtr,NULL);
  }

/*****************************************************************************/
/************************* List one centre for seeing ************************/
/*****************************************************************************/

static void Ctr_ListOneCentreForSeeing (struct Centre *Ctr,unsigned NumCtr)
  {
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   struct Place Plc;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   Ctr_StatusTxt_t StatusTxt;

   /***** Get data of place of this centre *****/
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

   /***** Number of centre in this list *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (NumCtr);
   HTM_TD_End ();

   /***** Centre logo and name *****/
   HTM_TD_Begin ("class=\"LM %s\"",BgColor);
   Ctr_DrawCentreLogoAndNameWithLink (Ctr,ActSeeDeg,
                                      TxtClassStrong,"CM");
   HTM_TD_End ();

   /***** Number of users who claim to belong to this centre *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToCtr (Ctr));
   HTM_TD_End ();

   /***** Place *****/
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   HTM_Txt (Plc.ShrtName);
   HTM_TD_End ();

   /***** Map *****/
   HTM_TD_Begin ("class=\"%s CM %s\"",TxtClassNormal,BgColor);
   Ctr_FormToGoToMap (Ctr);
   HTM_TD_End ();

   /***** Number of degrees *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Deg_GetNumDegsInCtr (Ctr->CtrCod));
   HTM_TD_End ();

   /***** Number of courses *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Crs_GetNumCrssInCtr (Ctr->CtrCod));
   HTM_TD_End ();

   /***** Number of users in courses of this centre *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Usr_GetNumUsrsInCrss (Hie_CTR,Ctr->CtrCod,
				       1 << Rol_STD |
				       1 << Rol_NET |
				       1 << Rol_TCH));	// Any user
   HTM_TD_End ();

   /***** Centre status *****/
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   if (StatusTxt != Ctr_STATUS_ACTIVE) // If active ==> do not show anything
      HTM_Txt (Txt_CENTRE_STATUS[StatusTxt]);
   HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of centres **********/
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
/************************** Put forms to edit centres ************************/
/*****************************************************************************/

void Ctr_EditCentres (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Edit centres *****/
   Ctr_EditCentresInternal ();

   /***** Centre destructor *****/
   Ctr_EditingCentreDestructor ();
  }

static void Ctr_EditCentresInternal (void)
  {
   extern const char *Hlp_INSTITUTION_Centres;
   extern const char *Txt_Centres_of_INSTITUTION_X;

   /***** Get list of places *****/
   Gbl.Plcs.SelectedOrder = Plc_ORDER_BY_PLACE;
   Plc_GetListPlaces ();

   /***** Get list of centres *****/
   Gbl.Hierarchy.Ctrs.SelectedOrder = Ctr_ORDER_BY_CENTRE;
   Ctr_GetFullListOfCentres (Gbl.Hierarchy.Ins.InsCod);

   /***** Write menu to select country and institution *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Centres_of_INSTITUTION_X,
				          Gbl.Hierarchy.Ins.FullName),
		 Ctr_PutIconsEditingCentres,
                 Hlp_INSTITUTION_Centres,Box_NOT_CLOSABLE);
   Str_FreeString ();

   /***** Put a form to create a new centre *****/
   Ctr_PutFormToCreateCentre ();

   /***** List current centres *****/
   if (Gbl.Hierarchy.Ctrs.Num)
      Ctr_ListCentresForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of centres *****/
   Ctr_FreeListCentres ();

   /***** Free list of places *****/
   Plc_FreeListPlaces ();
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of centres *****************/
/*****************************************************************************/

static void Ctr_PutIconsEditingCentres (void)
  {
   /***** Put icon to view centres *****/
   Ctr_PutIconToViewCentres ();

   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_HIERARCHY;
   Fig_PutIconToShowFigure ();
  }

void Ctr_PutIconToViewCentres (void)
  {
   extern const char *Txt_Centres;

   Lay_PutContextualLinkOnlyIcon (ActSeeCtr,NULL,NULL,
                                  "building.svg",
                                  Txt_Centres);
  }

/*****************************************************************************/
/************ Get basic list of centres ordered by name of centre ************/
/*****************************************************************************/

void Ctr_GetBasicListOfCentres (long InsCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCtr;
   struct Centre *Ctr;

   /***** Get centres from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get centres",
			     "SELECT CtrCod,"		// row[ 0]
			            "InsCod,"		// row[ 1]
			            "PlcCod,"		// row[ 2]
			            "Status,"		// row[ 3]
			            "RequesterUsrCod,"	// row[ 4]
			            "Latitude,"		// row[ 5]
			            "Longitude,"	// row[ 6]
			            "Altitude,"		// row[ 7]
			            "ShortName,"	// row[ 8]
			            "FullName,"		// row[ 9]
			            "WWW"		// row[10]
			     " FROM centres"
			     " WHERE InsCod=%ld"
			     " ORDER BY FullName",
			     InsCod);

   if (NumRows) // Centres found...
     {
      // NumRows should be equal to Deg->NumCourses
      Gbl.Hierarchy.Ctrs.Num = (unsigned) NumRows;

      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.Ctrs.Lst = (struct Centre *) calloc (NumRows,
								  sizeof (struct Centre))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the centres *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ctrs.Num;
	   NumCtr++)
        {
         Ctr = &(Gbl.Hierarchy.Ctrs.Lst[NumCtr]);

         /* Get centre data */
         row = mysql_fetch_row (mysql_res);
         Ctr_GetDataOfCentreFromRow (Ctr,row);

	 /* Reset number of users who claim to belong to this centre */
         Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = false;
        }
     }
   else
      Gbl.Hierarchy.Ctrs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get full list of centres                         **************/
/************* with number of users who claim to belong to them **************/
/*****************************************************************************/

void Ctr_GetFullListOfCentres (long InsCod)
  {
   static const char *OrderBySubQuery[Ctr_NUM_ORDERS] =
     {
      [Ctr_ORDER_BY_CENTRE  ] = "FullName",
      [Ctr_ORDER_BY_NUM_USRS] = "NumUsrs DESC,FullName",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumCtr;
   struct Centre *Ctr;

   /***** Get centres from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get centres",
			     "(SELECT centres.CtrCod,"		// row[ 0]
			             "centres.InsCod,"		// row[ 1]
			             "centres.PlcCod,"		// row[ 2]
			             "centres.Status,"		// row[ 3]
			             "centres.RequesterUsrCod,"	// row[ 4]
			             "centres.Latitude,"	// row[ 5]
			             "centres.Longitude,"	// row[ 6]
			             "centres.Altitude,"	// row[ 7]
			             "centres.ShortName,"	// row[ 8]
			             "centres.FullName,"	// row[ 9]
			             "centres.WWW,"		// row[10]
				     "COUNT(*) AS NumUsrs"	// row[11]
			     " FROM centres,usr_data"
			     " WHERE centres.InsCod=%ld"
			     " AND centres.CtrCod=usr_data.CtrCod"
			     " GROUP BY centres.CtrCod)"
			     " UNION "
			     "(SELECT CtrCod,"			// row[ 0]
			             "InsCod,"			// row[ 1]
			             "PlcCod,"			// row[ 2]
			             "Status,"			// row[ 3]
			             "RequesterUsrCod,"		// row[ 4]
			             "Latitude,"		// row[ 5]
			             "Longitude,"		// row[ 6]
			             "Altitude,"		// row[ 7]
			             "ShortName,"		// row[ 8]
			             "FullName,"		// row[ 9]
			             "WWW,"			// row[10]
				     "0 AS NumUsrs"		// row[11]
			     " FROM centres"
			     " WHERE InsCod=%ld"
			     " AND CtrCod NOT IN"
			     " (SELECT DISTINCT CtrCod FROM usr_data))"
			     " ORDER BY %s",
			     InsCod,InsCod,
			     OrderBySubQuery[Gbl.Hierarchy.Ctrs.SelectedOrder]);

   if (NumRows) // Centres found...
     {
      // NumRows should be equal to Deg->NumCourses
      Gbl.Hierarchy.Ctrs.Num = (unsigned) NumRows;

      /***** Create list with courses in degree *****/
      if ((Gbl.Hierarchy.Ctrs.Lst = (struct Centre *) calloc (NumRows,
								  sizeof (struct Centre))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the centres *****/
      for (NumCtr = 0;
	   NumCtr < Gbl.Hierarchy.Ctrs.Num;
	   NumCtr++)
        {
         Ctr = &(Gbl.Hierarchy.Ctrs.Lst[NumCtr]);

         /* Get centre data */
         row = mysql_fetch_row (mysql_res);
         Ctr_GetDataOfCentreFromRow (Ctr,row);

	 /* Get number of users who claim to belong to this centre (row[11]) */
         Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = false;
	 if (sscanf (row[11],"%u",&(Ctr->NumUsrsWhoClaimToBelongToCtr.NumUsrs)) == 1)
	    Ctr->NumUsrsWhoClaimToBelongToCtr.Valid = true;
        }
     }
   else
      Gbl.Hierarchy.Ctrs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get data of centre by code *************************/
/*****************************************************************************/

bool Ctr_GetDataOfCentreByCod (struct Centre *Ctr)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
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

   /***** Check if centre code is correct *****/
   if (Ctr->CtrCod > 0)
     {
      /***** Get data of a centre from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a centre",
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
				" FROM centres"
				" WHERE CtrCod=%ld",
				Ctr->CtrCod);
      if (NumRows) // Centre found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);
         Ctr_GetDataOfCentreFromRow (Ctr,row);

         /* Set return value */
         CtrFound = true;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return CtrFound;
  }

/*****************************************************************************/
/********** Get data of a centre from a row resulting of a query *************/
/*****************************************************************************/

static void Ctr_GetDataOfCentreFromRow (struct Centre *Ctr,MYSQL_ROW row)
  {
   /***** Get centre code (row[0]) *****/
   if ((Ctr->CtrCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of centre.");

   /***** Get institution code (row[1]) *****/
   Ctr->InsCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get place code (row[2]) *****/
   Ctr->PlcCod = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get centre status (row[3]) *****/
   if (sscanf (row[3],"%u",&(Ctr->Status)) != 1)
      Lay_ShowErrorAndExit ("Wrong centre status.");

   /***** Get requester user's code (row[4]) *****/
   Ctr->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[4]);

   /***** Get latitude (row[5]) *****/
   Ctr->Coord.Latitude = Map_GetLatitudeFromStr (row[5]);

   /***** Get longitude (row[6]) *****/
   Ctr->Coord.Longitude = Map_GetLongitudeFromStr (row[6]);

   /***** Get altitude (row[7]) *****/
   Ctr->Coord.Altitude = Map_GetAltitudeFromStr (row[7]);

   /***** Get the short name of the centre (row[8]) *****/
   Str_Copy (Ctr->ShrtName,row[8],
	     Hie_MAX_BYTES_SHRT_NAME);

   /***** Get the full name of the centre (row[9]) *****/
   Str_Copy (Ctr->FullName,row[9],
	     Hie_MAX_BYTES_FULL_NAME);

   /***** Get the URL of the centre (row[10]) *****/
   Str_Copy (Ctr->WWW,row[10],
	     Cns_MAX_BYTES_WWW);
  }

/*****************************************************************************/
/*********** Get the institution code of a centre from its code **************/
/*****************************************************************************/

long Ctr_GetInsCodOfCentreByCod (long CtrCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long InsCod = -1L;

   if (CtrCod > 0)
     {
      /***** Get the institution code of a centre from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the institution of a centre",
			  "SELECT InsCod FROM centres WHERE CtrCod=%ld",
			  CtrCod) == 1)
	{
	 /***** Get the institution code of this centre *****/
	 row = mysql_fetch_row (mysql_res);
	 InsCod = Str_ConvertStrCodToLongCod (row[0]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return InsCod;
  }

/*****************************************************************************/
/*************** Get the short name of a centre from its code ****************/
/*****************************************************************************/

void Ctr_GetShortNameOfCentreByCod (struct Centre *Ctr)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Ctr->ShrtName[0] = '\0';
   if (Ctr->CtrCod > 0)
     {
      /***** Get the short name of a centre from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the short name of a centre",
			  "SELECT ShortName FROM centres"
			  " WHERE CtrCod=%ld",
			  Ctr->CtrCod) == 1)
	{
	 /***** Get the short name of this centre *****/
	 row = mysql_fetch_row (mysql_res);

	 Str_Copy (Ctr->ShrtName,row[0],
	           Hie_MAX_BYTES_SHRT_NAME);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Free list of centres ***************************/
/*****************************************************************************/

void Ctr_FreeListCentres (void)
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
/************************** Write selector of centre *************************/
/*****************************************************************************/

void Ctr_WriteSelectorOfCentre (void)
  {
   extern const char *Txt_Centre;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCtrs;
   unsigned NumCtr;
   long CtrCod;

   /***** Begin form *****/
   Frm_StartFormGoTo (ActSeeDeg);

   if (Gbl.Hierarchy.Ins.InsCod > 0)
      HTM_SELECT_Begin (true,
			"id=\"ctr\" name=\"ctr\" class=\"HIE_SEL\"");
   else
      HTM_SELECT_Begin (false,
			"id=\"ctr\" name=\"ctr\" class=\"HIE_SEL\""
			" disabled=\"disabled\"");
   HTM_OPTION (HTM_Type_STRING,"",
	       Gbl.Hierarchy.Ctr.CtrCod < 0,true,
	       "[%s]",Txt_Centre);

   if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      /***** Get centres from database *****/
      NumCtrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get centres",
					   "SELECT DISTINCT CtrCod,ShortName"
					   " FROM centres"
					   " WHERE InsCod=%ld"
					   " ORDER BY ShortName",
					   Gbl.Hierarchy.Ins.InsCod);

      /***** Get centres *****/
      for (NumCtr = 0;
	   NumCtr < NumCtrs;
	   NumCtr++)
        {
         /* Get next centre */
         row = mysql_fetch_row (mysql_res);

         /* Get centre code (row[0]) */
         if ((CtrCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of centre.");

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
/*************************** List all the centres ****************************/
/*****************************************************************************/

static void Ctr_ListCentresForEdition (void)
  {
   extern const char *Txt_Another_place;
   extern const char *Txt_CENTRE_STATUS[Ctr_NUM_STATUS_TXT];
   unsigned NumCtr;
   struct Centre *Ctr;
   unsigned NumPlc;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct UsrData UsrDat;
   bool ICanEdit;
   unsigned NumUsrsInCrssOfCtr;
   Ctr_StatusTxt_t StatusTxt;
   unsigned StatusUnsigned;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Ctr_PutHeadCentresForEdition ();

   /***** Write all the centres *****/
   for (NumCtr = 0;
	NumCtr < Gbl.Hierarchy.Ctrs.Num;
	NumCtr++)
     {
      Ctr = &Gbl.Hierarchy.Ctrs.Lst[NumCtr];

      ICanEdit = Ctr_CheckIfICanEditACentre (Ctr);
      NumUsrsInCrssOfCtr = Usr_GetNumUsrsInCrss (Hie_CTR,Ctr->CtrCod,
						 1 << Rol_STD |
						 1 << Rol_NET |
						 1 << Rol_TCH);	// Any user

      /* Put icon to remove centre */
      HTM_TR_Begin (NULL);
      HTM_TD_Begin ("class=\"BM\"");
      if (!ICanEdit ||						// I cannot edit
	  NumUsrsInCrssOfCtr)					// Centre has users
	 Ico_PutIconRemovalNotAllowed ();
      else if (Deg_GetNumDegsInCtr (Ctr->CtrCod))		// Centre has degrees
	 Ico_PutIconRemovalNotAllowed ();
      else if (Usr_GetNumUsrsWhoClaimToBelongToCtr (Ctr))	// Centre has users who claim to belong to it
	 Ico_PutIconRemovalNotAllowed ();
      else	// I can remove centre
        {
         Frm_StartForm (ActRemCtr);
         Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
         Ico_PutIconRemove ();
         Frm_EndForm ();
        }
      HTM_TD_End ();

      /* Centre code */
      HTM_TD_Begin ("class=\"DAT CODE\"");
      HTM_Long (Ctr->CtrCod);
      HTM_TD_End ();

      /* Centre logo */
      HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr->FullName);
      Lgo_DrawLogo (Hie_CTR,Ctr->CtrCod,Ctr->ShrtName,20,NULL,true);
      HTM_TD_End ();

      /* Place */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActChgCtrPlc);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 HTM_SELECT_Begin (true,
			   "name=\"PlcCod\" class=\"PLC_SEL\"");
	 HTM_OPTION (HTM_Type_STRING,"0",
		     Ctr->PlcCod == 0,false,
		     "%s",Txt_Another_place);
	 for (NumPlc = 0;
	      NumPlc < Gbl.Plcs.Num;
	      NumPlc++)
	    HTM_OPTION (HTM_Type_LONG,&Gbl.Plcs.Lst[NumPlc].PlcCod,
			Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr->PlcCod,false,
			"%s",Gbl.Plcs.Lst[NumPlc].ShrtName);
	 HTM_SELECT_End ();
	 Frm_EndForm ();
	}
      else
	 for (NumPlc = 0;
	      NumPlc < Gbl.Plcs.Num;
	      NumPlc++)
	    if (Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr->PlcCod)
	       HTM_Txt (Gbl.Plcs.Lst[NumPlc].ShrtName);
      HTM_TD_End ();

      /* Centre short name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActRenCtrSho);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,Ctr->ShrtName,true,
			 "class=\"INPUT_SHORT_NAME\"");
	 Frm_EndForm ();
	}
      else
	 HTM_Txt (Ctr->ShrtName);
      HTM_TD_End ();

      /* Centre full name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActRenCtrFul);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,Ctr->FullName,true,
			 "class=\"INPUT_FULL_NAME\"");
	 Frm_EndForm ();
	}
      else
	 HTM_Txt (Ctr->FullName);
      HTM_TD_End ();

      /* Centre WWW */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_StartForm (ActChgCtrWWW);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 HTM_INPUT_URL ("WWW",Ctr->WWW,true,
			"class=\"INPUT_WWW_NARROW\" required=\"required\"");
	 Frm_EndForm ();
	}
      else
	{
         Str_Copy (WWW,Ctr->WWW,
                   Cns_MAX_BYTES_WWW);
         HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHORT\"");
         HTM_A_Begin ("href=\"%s\" target=\"_blank\""
                      " class=\"DAT\" title=\"%s\"",Ctr->WWW,Ctr->WWW);
         HTM_Txt (WWW);
         HTM_A_End ();
         HTM_DIV_End ();
	}
      HTM_TD_End ();

      /* Number of users who claim to belong to this centre */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (Usr_GetNumUsrsWhoClaimToBelongToCtr (Ctr));
      HTM_TD_End ();

      /* Number of degrees */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (Deg_GetNumDegsInCtr (Ctr->CtrCod));
      HTM_TD_End ();

      /* Number of users in courses of this centre */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (NumUsrsInCrssOfCtr);
      HTM_TD_End ();

      /* Centre requester */
      UsrDat.UsrCod = Ctr->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
      HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
      Msg_WriteMsgAuthor (&UsrDat,true,NULL);
      HTM_TD_End ();

      /* Centre status */
      StatusTxt = Ctr_GetStatusTxtFromStatusBits (Ctr->Status);
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM &&
	  StatusTxt == Ctr_STATUS_PENDING)
	{
	 Frm_StartForm (ActChgCtrSta);
	 Ctr_PutParamOtherCtrCod (Ctr->CtrCod);
	 HTM_SELECT_Begin (true,
			   "name=\"Status\" class=\"INPUT_STATUS\"");

	 StatusUnsigned = (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_PENDING);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,true,false,
		     "%s",Txt_CENTRE_STATUS[Ctr_STATUS_PENDING]);

	 StatusUnsigned = (unsigned) Ctr_GetStatusBitsFromStatusTxt (Ctr_STATUS_ACTIVE);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,false,false,
		     "%s",Txt_CENTRE_STATUS[Ctr_STATUS_ACTIVE]);

	 HTM_SELECT_End ();
	 Frm_EndForm ();
	}
      else if (StatusTxt != Ctr_STATUS_ACTIVE)	// If active ==> do not show anything
	 HTM_Txt (Txt_CENTRE_STATUS[StatusTxt]);
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a centre *******************/
/*****************************************************************************/

static bool Ctr_CheckIfICanEditACentre (struct Centre *Ctr)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM ||		// I am an institution administrator or higher
                  ((Ctr->Status & Ctr_STATUS_BIT_PENDING) != 0 &&	// Centre is not yet activated
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
/******************** Write parameter with code of centre ********************/
/*****************************************************************************/

void Ctr_PutParamCtrCod (long CtrCod)
  {
   Par_PutHiddenParamLong (NULL,"ctr",CtrCod);
  }

/*****************************************************************************/
/***************** Write parameter with code of other centre *****************/
/*****************************************************************************/

static void Ctr_PutParamOtherCtrCod (long CtrCod)
  {
   Par_PutHiddenParamLong (NULL,"OthCtrCod",CtrCod);
  }

/*****************************************************************************/
/****************** Get parameter with code of other centre ******************/
/*****************************************************************************/

long Ctr_GetAndCheckParamOtherCtrCod (long MinCodAllowed)
  {
   long CtrCod;

   /***** Get and check parameter with code of centre *****/
   if ((CtrCod = Par_GetParToLong ("OthCtrCod")) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of centre is missing or invalid.");

   return CtrCod;
  }

/*****************************************************************************/
/******************************* Remove a centre *****************************/
/*****************************************************************************/

void Ctr_RemoveCentre (void)
  {
   extern const char *Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre;
   extern const char *Txt_Centre_X_removed;
   char PathCtr[PATH_MAX + 1];

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get centre code *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get data of the centre from database *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Check if this centre has teachers *****/
   if (Deg_GetNumDegsInCtr (Ctr_EditingCtr->CtrCod))			// Centre has degrees
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre);
   else if (Usr_GetNumUsrsWhoClaimToBelongToCtr (Ctr_EditingCtr))	// Centre has users who claim to belong to it
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre);
   else if (Usr_GetNumUsrsInCrss (Hie_CTR,Ctr_EditingCtr->CtrCod,
				  1 << Rol_STD |
				  1 << Rol_NET |
				  1 << Rol_TCH))			// Centre has users
      Ale_ShowAlert (Ale_WARNING,
		     Txt_To_remove_a_centre_you_must_first_remove_all_degrees_and_teachers_in_the_centre);
   else	// Centre has no degrees or users ==> remove it
     {
      /***** Remove all the threads and posts in forums of the centre *****/
      For_RemoveForums (Hie_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove surveys of the centre *****/
      Svy_RemoveSurveys (Hie_CTR,Ctr_EditingCtr->CtrCod);

      /***** Remove information related to files in centre *****/
      Brw_RemoveCtrFilesFromDB (Ctr_EditingCtr->CtrCod);

      /***** Remove all classrooms in centre *****/
      Cla_RemoveAllClassroomsInCtr (Ctr_EditingCtr->CtrCod);

      /***** Remove directories of the centre *****/
      snprintf (PathCtr,sizeof (PathCtr),
	        "%s/%02u/%u",
	        Cfg_PATH_CTR_PUBLIC,
	        (unsigned) (Ctr_EditingCtr->CtrCod % 100),
	        (unsigned) Ctr_EditingCtr->CtrCod);
      Fil_RemoveTree (PathCtr);

      /***** Remove centre *****/
      DB_QueryDELETE ("can not remove a centre",
		      "DELETE FROM centres WHERE CtrCod=%ld",
		      Ctr_EditingCtr->CtrCod);

      /***** Flush caches *****/
      Deg_FlushCacheNumDegsInCtr ();
      Crs_FlushCacheNumCrssInCtr ();
      Usr_FlushCacheNumUsrsWhoClaimToBelongToCtr ();

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Centre_X_removed,
	               Ctr_EditingCtr->FullName);

      Ctr_EditingCtr->CtrCod = -1L;	// To not showing button to go to centre
     }
  }

/*****************************************************************************/
/************************ Change the place of a centre ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrPlc (void)
  {
   extern const char *Txt_The_place_of_the_centre_has_changed;
   long NewPlcCod;

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get centre code *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get parameter with place code *****/
   NewPlcCod = Plc_GetParamPlcCod ();

   /***** Get data of centre from database *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Update place in table of centres *****/
   Ctr_UpdateCtrPlcDB (Ctr_EditingCtr->CtrCod,NewPlcCod);
   Ctr_EditingCtr->PlcCod = NewPlcCod;

   /***** Create alert to show the change made
	  and put button to go to centre changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_place_of_the_centre_has_changed);
  }

/*****************************************************************************/
/************** Update database changing old place by new place **************/
/*****************************************************************************/

void Ctr_UpdateCtrPlcDB (long CtrCod,long NewPlcCod)
  {
   DB_QueryUPDATE ("can not update the place of a centre",
		   "UPDATE centres SET PlcCod=%ld WHERE CtrCod=%ld",
	           NewPlcCod,CtrCod);
  }

/*****************************************************************************/
/************************ Change the name of a centre ************************/
/*****************************************************************************/

void Ctr_RenameCentreShort (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Rename centre *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);
   Ctr_RenameCentre (Ctr_EditingCtr,Cns_SHRT_NAME);
  }

void Ctr_RenameCentreFull (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Rename centre *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);
   Ctr_RenameCentre (Ctr_EditingCtr,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a centre ************************/
/*****************************************************************************/

void Ctr_RenameCentre (struct Centre *Ctr,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_centre_X_already_exists;
   extern const char *Txt_The_centre_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_centre_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentCtrName = NULL;		// Initialized to avoid warning
   char NewCtrName[Hie_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Hie_MAX_BYTES_SHRT_NAME;
         CurrentCtrName = Ctr->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Hie_MAX_BYTES_FULL_NAME;
         CurrentCtrName = Ctr->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the new name for the centre */
   Par_GetParToText (ParamName,NewCtrName,MaxBytes);

   /***** Get from the database the old names of the centre *****/
   Ctr_GetDataOfCentreByCod (Ctr);

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
        	             Txt_The_centre_X_already_exists,
			     NewCtrName);
         else
           {
            /* Update the table changing old name by new name */
            Ctr_UpdateInsNameDB (Ctr->CtrCod,FieldName,NewCtrName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_centre_X_has_been_renamed_as_Y,
                             CurrentCtrName,NewCtrName);

	    /* Change current centre name in order to display it properly */
	    Str_Copy (CurrentCtrName,NewCtrName,
	              MaxBytes);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_centre_X_has_not_changed,
                          CurrentCtrName);
     }
  }

/*****************************************************************************/
/********************* Check if the name of centre exists ********************/
/*****************************************************************************/

bool Ctr_CheckIfCtrNameExistsInIns (const char *FieldName,const char *Name,
				    long CtrCod,long InsCod)
  {
   /***** Get number of centres with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a centre"
			  " already existed",
			  "SELECT COUNT(*) FROM centres"
			  " WHERE InsCod=%ld AND %s='%s' AND CtrCod<>%ld",
			  InsCod,FieldName,Name,CtrCod) != 0);
  }

/*****************************************************************************/
/****************** Update centre name in table of centres *******************/
/*****************************************************************************/

static void Ctr_UpdateInsNameDB (long CtrCod,const char *FieldName,const char *NewCtrName)
  {
   /***** Update centre changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a centre",
		   "UPDATE centres SET %s='%s' WHERE CtrCod=%ld",
	           FieldName,NewCtrName,CtrCod);
  }

/*****************************************************************************/
/************************* Change the URL of a centre ************************/
/*****************************************************************************/

void Ctr_ChangeCtrWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get the code of the centre *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get the new WWW for the centre *****/
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of centre *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update database changing old WWW by new WWW *****/
      Ctr_UpdateCtrWWWDB (Ctr_EditingCtr->CtrCod,NewWWW);
      Str_Copy (Ctr_EditingCtr->WWW,NewWWW,
		Cns_MAX_BYTES_WWW);

      /***** Write message to show the change made
	     and put button to go to centre changed *****/
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
   DB_QueryUPDATE ("can not update the web of a centre",
		   "UPDATE centres SET WWW='%s' WHERE CtrCod=%ld",
	           NewWWW,CtrCod);
  }

/*****************************************************************************/
/*********************** Change the status of a centre ***********************/
/*****************************************************************************/

void Ctr_ChangeCtrStatus (void)
  {
   extern const char *Txt_The_status_of_the_centre_X_has_changed;
   Ctr_Status_t Status;
   Ctr_StatusTxt_t StatusTxt;

   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Get centre code *****/
   Ctr_EditingCtr->CtrCod = Ctr_GetAndCheckParamOtherCtrCod (1);

   /***** Get parameter with status *****/
   Status = (Ctr_Status_t)
	    Par_GetParToUnsignedLong ("Status",
				      0,
				      (unsigned long) Ctr_MAX_STATUS,
				      (unsigned long) Ctr_WRONG_STATUS);
   if (Status == Ctr_WRONG_STATUS)
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Ctr_GetStatusTxtFromStatusBits (Status);
   Status = Ctr_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of centre *****/
   Ctr_GetDataOfCentreByCod (Ctr_EditingCtr);

   /***** Update status in table of centres *****/
   DB_QueryUPDATE ("can not update the status of a centre",
		   "UPDATE centres SET Status=%u WHERE CtrCod=%ld",
	           (unsigned) Status,Ctr_EditingCtr->CtrCod);
   Ctr_EditingCtr->Status = Status;

   /***** Write message to show the change made
	  and put button to go to centre changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_status_of_the_centre_X_has_changed,
	            Ctr_EditingCtr->ShrtName);
  }

/*****************************************************************************/
/********* Show alerts after changing a centre and continue editing **********/
/*****************************************************************************/

void Ctr_ContEditAfterChgCtr (void)
  {
   /***** Write message to show the change made
	  and put button to go to centre changed *****/
   Ctr_ShowAlertAndButtonToGoToCtr ();

   /***** Show the form again *****/
   Ctr_EditCentresInternal ();

   /***** Centre destructor *****/
   Ctr_EditingCentreDestructor ();
  }

/*****************************************************************************/
/***************** Write message to show the change made  ********************/
/***************** and put button to go to centre changed ********************/
/*****************************************************************************/

static void Ctr_ShowAlertAndButtonToGoToCtr (void)
  {
   // If the centre being edited is different to the current one...
   if (Ctr_EditingCtr->CtrCod != Gbl.Hierarchy.Ctr.CtrCod)
     {
      /***** Alert with button to go to centre *****/
      Ale_ShowLastAlertAndButton (ActSeeDeg,NULL,NULL,Ctr_PutParamGoToCtr,
                                  Btn_CONFIRM_BUTTON,
				  Hie_BuildGoToMsg (Ctr_EditingCtr->ShrtName));
      Hie_FreeGoToMsg ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Ctr_PutParamGoToCtr (void)
  {
   Ctr_PutParamCtrCod (Ctr_EditingCtr->CtrCod);
  }

/*****************************************************************************/
/********************* Put a form to create a new centre *********************/
/*****************************************************************************/

static void Ctr_PutFormToCreateCentre (void)
  {
   extern const char *Txt_New_centre;
   extern const char *Txt_Another_place;
   extern const char *Txt_Create_centre;
   unsigned NumPlc;

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM)
      Frm_StartForm (ActNewCtr);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_StartForm (ActReqCtr);
   else
      Lay_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_centre,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Ctr_PutHeadCentresForEdition ();

   HTM_TR_Begin (NULL);

   /***** Column to remove centre, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Centre code *****/
   HTM_TD_Begin ("class=\"CODE\"");
   HTM_TD_End ();

   /***** Centre logo *****/
   HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Ctr_EditingCtr->FullName);
   Lgo_DrawLogo (Hie_CTR,-1L,"",20,NULL,true);
   HTM_TD_End ();

   /***** Place *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_SELECT_Begin (false,
		     "name=\"PlcCod\" class=\"PLC_SEL\"");
   HTM_OPTION (HTM_Type_STRING,"0",
	       Ctr_EditingCtr->PlcCod == 0,false,
	       "%s",Txt_Another_place);
   for (NumPlc = 0;
	NumPlc < Gbl.Plcs.Num;
	NumPlc++)
      HTM_OPTION (HTM_Type_LONG,&Gbl.Plcs.Lst[NumPlc].PlcCod,
		  Gbl.Plcs.Lst[NumPlc].PlcCod == Ctr_EditingCtr->PlcCod,false,
		  "%s",Gbl.Plcs.Lst[NumPlc].ShrtName);
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Centre short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Hie_MAX_CHARS_SHRT_NAME,Ctr_EditingCtr->ShrtName,false,
		   "class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Centre full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Hie_MAX_CHARS_FULL_NAME,Ctr_EditingCtr->FullName,false,
		   "class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Centre WWW *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_URL ("WWW",Ctr_EditingCtr->WWW,false,
		  "class=\"INPUT_WWW_NARROW\" required=\"required\"");
   HTM_TD_End ();

   /***** Number of users who claim to belong to this centre *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Number of degrees *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Number of users in courses of this centre *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Centre requester *****/
   HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,true,NULL);
   HTM_TD_End ();

   /***** Centre status *****/
   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_centre);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Ctr_PutHeadCentresForSeeing (bool OrderSelectable)
  {
   extern const char *Txt_CENTRES_HELP_ORDER[2];
   extern const char *Txt_CENTRES_ORDER[2];
   extern const char *Txt_Place;
   extern const char *Txt_Map;
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
      HTM_TH_Begin (1,1,Order == Ctr_ORDER_BY_CENTRE ? "LM" :
				                       "RM");
      if (OrderSelectable)
	{
	 Frm_StartForm (ActSeeCtr);
	 Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	 HTM_BUTTON_SUBMIT_Begin (Txt_CENTRES_HELP_ORDER[Order],
				  Order == Ctr_ORDER_BY_CENTRE ? "BT_LINK LM TIT_TBL" :
					                         "BT_LINK RM TIT_TBL",
				  NULL);
	 if (Order == Gbl.Hierarchy.Ctrs.SelectedOrder)
	    HTM_U_Begin ();
	}
      HTM_Txt (Txt_CENTRES_ORDER[Order]);
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
   HTM_TH (1,1,"CM",Txt_Map);
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

static void Ctr_PutHeadCentresForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Place;
   extern const char *Txt_Short_name_of_the_centre;
   extern const char *Txt_Full_name_of_the_centre;
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
   HTM_TH (1,1,"LM",Txt_Short_name_of_the_centre);
   HTM_TH (1,1,"LM",Txt_Full_name_of_the_centre);
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
/****************** Receive form to request a new centre *********************/
/*****************************************************************************/

void Ctr_RecFormReqCtr (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Receive form to request a new centre *****/
   Ctr_RecFormRequestOrCreateCtr ((unsigned) Ctr_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new centre *********************/
/*****************************************************************************/

void Ctr_RecFormNewCtr (void)
  {
   /***** Centre constructor *****/
   Ctr_EditingCentreConstructor ();

   /***** Receive form to create a new centre *****/
   Ctr_RecFormRequestOrCreateCtr (0);
  }

/*****************************************************************************/
/************* Receive form to request or create a new centre ****************/
/*****************************************************************************/

static void Ctr_RecFormRequestOrCreateCtr (unsigned Status)
  {
   extern const char *Txt_The_centre_X_already_exists;
   extern const char *Txt_Created_new_centre_X;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_centre;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_centre;

   /***** Get parameters from form *****/
   /* Set centre institution */
   Ctr_EditingCtr->InsCod = Gbl.Hierarchy.Ins.InsCod;

   /* Get place */
   if ((Ctr_EditingCtr->PlcCod = Plc_GetParamPlcCod ()) < 0)	// 0 is reserved for "other place"
      Ale_ShowAlert (Ale_ERROR,"Wrong place.");

   /* Get centre short name */
   Par_GetParToText ("ShortName",Ctr_EditingCtr->ShrtName,Hie_MAX_BYTES_SHRT_NAME);

   /* Get centre full name */
   Par_GetParToText ("FullName",Ctr_EditingCtr->FullName,Hie_MAX_BYTES_FULL_NAME);

   /* Get centre WWW */
   Par_GetParToText ("WWW",Ctr_EditingCtr->WWW,Cns_MAX_BYTES_WWW);

   if (Ctr_EditingCtr->ShrtName[0] &&
       Ctr_EditingCtr->FullName[0])	// If there's a centre name
     {
      if (Ctr_EditingCtr->WWW[0])
        {
         /***** If name of centre was in database... *****/
         if (Ctr_CheckIfCtrNameExistsInIns ("ShortName",Ctr_EditingCtr->ShrtName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_centre_X_already_exists,
                             Ctr_EditingCtr->ShrtName);
         else if (Ctr_CheckIfCtrNameExistsInIns ("FullName",Ctr_EditingCtr->FullName,-1L,Gbl.Hierarchy.Ins.InsCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        		     Txt_The_centre_X_already_exists,
                             Ctr_EditingCtr->FullName);
         else	// Add new centre to database
           {
            Ctr_CreateCentre (Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_centre_X,
			     Ctr_EditingCtr->FullName);
           }
        }
      else	// If there is not a web
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_web_address_of_the_new_centre);
     }
   else	// If there is not a centre name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_centre);
  }

/*****************************************************************************/
/***************************** Create a new centre ***************************/
/*****************************************************************************/

static void Ctr_CreateCentre (unsigned Status)
  {
   /***** Create a new centre *****/
   Ctr_EditingCtr->CtrCod =
   DB_QueryINSERTandReturnCode ("can not create a new centre",
				"INSERT INTO centres"
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
/************************** Get number of centres ****************************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsTotal (void)
  {
   /***** Get total number of centres from database *****/
   return (unsigned) DB_GetNumRowsTable ("centres");
  }

/*****************************************************************************/
/******************* Get number of centres in a country **********************/
/*****************************************************************************/

void Ctr_FlushCacheNumCtrsInCty (void)
  {
   Gbl.Cache.NumCtrsInCty.CtyCod  = -1L;
   Gbl.Cache.NumCtrsInCty.NumCtrs = 0;
  }

unsigned Ctr_GetNumCtrsInCty (long CtyCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (CtyCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (CtyCod == Gbl.Cache.NumCtrsInCty.CtyCod)
      return Gbl.Cache.NumCtrsInCty.NumCtrs;

   /***** 3. Slow: number of centres in a country from database *****/
   Gbl.Cache.NumCtrsInCty.CtyCod  = CtyCod;
   Gbl.Cache.NumCtrsInCty.NumCtrs =
   (unsigned) DB_QueryCOUNT ("can not get number of centres in a country",
			     "SELECT COUNT(*) FROM institutions,centres"
			     " WHERE institutions.CtyCod=%ld"
			     " AND institutions.InsCod=centres.InsCod",
			     CtyCod);
   return Gbl.Cache.NumCtrsInCty.NumCtrs;
  }

/*****************************************************************************/
/**************** Get number of centres in an institution ********************/
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

   /***** 3. Slow: number of centres in an institution from database *****/
   Gbl.Cache.NumCtrsInIns.InsCod  = InsCod;
   Gbl.Cache.NumCtrsInIns.NumCtrs =
   (unsigned) DB_QueryCOUNT ("can not get number of centres in an institution",
			     "SELECT COUNT(*) FROM centres"
			     " WHERE InsCod=%ld",
			     InsCod);

   return Gbl.Cache.NumCtrsInIns.NumCtrs;
  }

/*****************************************************************************/
/******* Get number of centres (of the current institution) in a place *******/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsInPlc (long PlcCod)
  {
   /***** Get number of centres (of the current institution) in a place *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of centres in a place",
			     "SELECT COUNT(*) FROM centres"
			     " WHERE InsCod=%ld AND PlcCod=%ld",
			     Gbl.Hierarchy.Ins.InsCod,PlcCod);
  }

/*****************************************************************************/
/********************* Get number of centres with degrees ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithDegs (const char *SubQuery)
  {
   /***** Get number of centres with degrees from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres with degrees",
			     "SELECT COUNT(DISTINCT centres.CtrCod)"
			     " FROM institutions,centres,degrees"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod",
			     SubQuery);
  }

/*****************************************************************************/
/********************* Get number of centres with courses ********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithCrss (const char *SubQuery)
  {
   /***** Get number of centres with courses from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres with courses",
			     "SELECT COUNT(DISTINCT centres.CtrCod)"
			     " FROM institutions,centres,degrees,courses"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod",
			     SubQuery);
  }

/*****************************************************************************/
/********************* Get number of centres with users **********************/
/*****************************************************************************/

unsigned Ctr_GetNumCtrsWithUsrs (Rol_Role_t Role,const char *SubQuery)
  {
   /***** Get number of centres with users from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of centres with users",
			     "SELECT COUNT(DISTINCT centres.CtrCod)"
			     " FROM institutions,centres,degrees,courses,crs_usr"
			     " WHERE %sinstitutions.InsCod=centres.InsCod"
			     " AND centres.CtrCod=degrees.CtrCod"
			     " AND degrees.DegCod=courses.DegCod"
			     " AND courses.CrsCod=crs_usr.CrsCod"
			     " AND crs_usr.Role=%u",
			     SubQuery,(unsigned) Role);
  }

/*****************************************************************************/
/****************************** List centres found ***************************/
/*****************************************************************************/

void Ctr_ListCtrsFound (MYSQL_RES **mysql_res,unsigned NumCtrs)
  {
   extern const char *Txt_centre;
   extern const char *Txt_centres;
   MYSQL_ROW row;
   unsigned NumCtr;
   struct Centre Ctr;

   /***** Query database *****/
   if (NumCtrs)
     {
      /***** Begin box and table *****/
      /* Number of centres found */
      Box_BoxTableBegin (NULL,Str_BuildStringLongStr ((long) NumCtrs,
						      (NumCtrs == 1) ? Txt_centre :
	                                                               Txt_centres),
			 NULL,NULL,Box_NOT_CLOSABLE,2);
      Str_FreeString ();

      /***** Write heading *****/
      Ctr_PutHeadCentresForSeeing (false);	// Order not selectable

      /***** List the centres (one row per centre) *****/
      for (NumCtr = 1;
	   NumCtr <= NumCtrs;
	   NumCtr++)
	{
	 /* Get next centre */
	 row = mysql_fetch_row (*mysql_res);

	 /* Get centre code (row[0]) */
	 Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get data of centre */
	 Ctr_GetDataOfCentreByCod (&Ctr);

	 /* Write data of this centre */
	 Ctr_ListOneCentreForSeeing (&Ctr,NumCtr);
	}

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Centre constructor/destructor **********************/
/*****************************************************************************/

static void Ctr_EditingCentreConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Ctr_EditingCtr != NULL)
      Lay_ShowErrorAndExit ("Error initializing centre.");

   /***** Allocate memory for centre *****/
   if ((Ctr_EditingCtr = (struct Centre *) malloc (sizeof (struct Centre))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for centre.");

   /***** Reset centre *****/
   Ctr_EditingCtr->CtrCod          = -1L;
   Ctr_EditingCtr->InsCod          = -1L;
   Ctr_EditingCtr->PlcCod          = -1L;
   Ctr_EditingCtr->Status          = (Ctr_Status_t) 0;
   Ctr_EditingCtr->RequesterUsrCod = -1L;
   Ctr_EditingCtr->ShrtName[0]     = '\0';
   Ctr_EditingCtr->FullName[0]     = '\0';
   Ctr_EditingCtr->WWW[0]          = '\0';
  }

static void Ctr_EditingCentreDestructor (void)
  {
   /***** Free memory used for centre *****/
   if (Ctr_EditingCtr != NULL)
     {
      free (Ctr_EditingCtr);
      Ctr_EditingCtr = NULL;
     }
  }

/*****************************************************************************/
/************************ Form to go to centre map ***************************/
/*****************************************************************************/

void Ctr_FormToGoToMap (struct Centre *Ctr)
  {
   extern const char *Txt_Map;

   if (Ctr_GetIfMapIsAvailable (&Ctr))
     {
      Ctr_EditingCtr = Ctr;	// Used to pass parameter with the code of the centre
      Lay_PutContextualLinkOnlyIcon (ActSeeCtrInf,NULL,Ctr_PutParamGoToCtr,
				     "map-marker-alt.svg",
				     Txt_Map);
     }
  }

/*****************************************************************************/
/************************ Check if a centre has map **************************/
/*****************************************************************************/

bool Ctr_GetIfMapIsAvailable (const struct Centre *Ctr)
  {
   /***** Coordinates 0, 0 means not set ==> don't show map *****/
   return (bool) (Ctr->Coord.Latitude ||
                  Ctr->Coord.Longitude);
  }
