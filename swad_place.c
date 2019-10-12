// swad_place.c: places

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_place.h"
#include "swad_table.h"

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

static struct Place *Plc_EditingPlc = NULL;	// Static variable to keep the place being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Plc_GetParamPlcOrder (void);
static bool Plc_CheckIfICanCreatePlaces (void);
static void Plc_PutIconsListingPlaces (void);
static void Plc_PutIconToEditPlaces (void);
static void Plc_EditPlacesInternal (void);
static void Plc_PutIconsEditingPlaces (void);

static void Plc_ListPlacesForEdition (void);
static void Plc_PutParamPlcCod (long PlcCod);

static void Plc_RenamePlace (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Plc_CheckIfPlaceNameExists (const char *FieldName,const char *Name,long PlcCod);
static void Plc_UpdatePlcNameDB (long PlcCod,const char *FieldName,const char *NewPlcName);

static void Plc_PutFormToCreatePlace (void);
static void Plc_PutHeadPlaces (void);
static void Plc_CreatePlace (struct Place *Plc);

static void Plc_EditingPlaceConstructor (void);
static void Plc_EditingPlaceDestructor (void);

/*****************************************************************************/
/*************************** List all the places *****************************/
/*****************************************************************************/

void Plc_SeePlaces (void)
  {
   extern const char *Hlp_INSTITUTION_Places;
   extern const char *Txt_Places;
   extern const char *Txt_PLACES_HELP_ORDER[2];
   extern const char *Txt_PLACES_ORDER[2];
   extern const char *Txt_Other_places;
   extern const char *Txt_Place_unspecified;
   extern const char *Txt_New_place;
   Plc_Order_t Order;
   unsigned NumPlc;
   unsigned NumCtrsWithPlc = 0;
   unsigned NumCtrsInOtherPlcs;

   if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      /***** Get parameter with the type of order in the list of places *****/
      Plc_GetParamPlcOrder ();

      /***** Get list of places *****/
      Plc_GetListPlaces ();

      /***** Table head *****/
      Box_StartBox (NULL,Txt_Places,Plc_PutIconsListingPlaces,
                    Hlp_INSTITUTION_Places,Box_NOT_CLOSABLE);
      Tbl_TABLE_BeginWideMarginPadding (2);
      Tbl_TR_Begin (NULL);
      for (Order = Plc_ORDER_BY_PLACE;
	   Order <= Plc_ORDER_BY_NUM_CTRS;
	   Order++)
	{
	 Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
	 Frm_StartForm (ActSeePlc);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Frm_LinkFormSubmit (Txt_PLACES_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Plcs.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_PLACES_ORDER[Order]);
	 if (Order == Gbl.Plcs.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Frm_EndForm ();
	 Tbl_TH_End ();
	}
      Tbl_TR_End ();

      /***** Write all places and their nuber of centres *****/
      for (NumPlc = 0;
	   NumPlc < Gbl.Plcs.Num;
	   NumPlc++)
	{
	 /* Write data of this place */
	 Tbl_TR_Begin (NULL);

	 Tbl_TD_Begin ("class=\"DAT LEFT_MIDDLE\"");
	 fprintf (Gbl.F.Out,"%s",Gbl.Plcs.Lst[NumPlc].FullName);
	 Tbl_TD_End ();

	 Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
	 fprintf (Gbl.F.Out,"%u",Gbl.Plcs.Lst[NumPlc].NumCtrs);
	 Tbl_TD_End ();

	 Tbl_TR_End ();
	 NumCtrsWithPlc += Gbl.Plcs.Lst[NumPlc].NumCtrs;
	}

      /***** Separation row *****/
      Tbl_TR_Begin (NULL);
      Tbl_TD_Begin ("colspan=\"2\" class=\"DAT\"");
      fprintf (Gbl.F.Out,"&nbsp;");
      Tbl_TD_End ();
      Tbl_TR_End ();

      /***** Write centres (of the current institution) with other place *****/
      NumCtrsInOtherPlcs = Ctr_GetNumCtrsInPlc (0);
      Tbl_TR_Begin (NULL);

      Tbl_TD_Begin ("class=\"DAT LEFT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%s",Txt_Other_places);
      Tbl_TD_End ();

      Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%u",NumCtrsInOtherPlcs);
      Tbl_TD_End ();

      Tbl_TR_End ();
      NumCtrsWithPlc += NumCtrsInOtherPlcs;

      /***** Write centres (of the current institution) with no place *****/
      Tbl_TR_Begin (NULL);

      Tbl_TD_Begin ("class=\"DAT LEFT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%s",Txt_Place_unspecified);
      Tbl_TD_End ();

      Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%u",Ctr_GetNumCtrsInIns (Gbl.Hierarchy.Ins.InsCod) -
	                      NumCtrsWithPlc);
      Tbl_TD_End ();

      Tbl_TR_End ();

      /***** End table *****/
      Tbl_TABLE_End ();

      /***** Button to create place *****/
      if (Plc_CheckIfICanCreatePlaces ())
	{
	 Frm_StartForm (ActEdiPlc);
	 Btn_PutConfirmButton (Txt_New_place);
	 Frm_EndForm ();
	}

      /***** End box *****/
      Box_EndBox ();

      /***** Free list of places *****/
      Plc_FreeListPlaces ();
     }
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of places ***********/
/*****************************************************************************/

static void Plc_GetParamPlcOrder (void)
  {
   Gbl.Plcs.SelectedOrder = (Plc_Order_t)
	                    Par_GetParToUnsignedLong ("Order",
                                                      0,
                                                      Plc_NUM_ORDERS - 1,
                                                      (unsigned long) Plc_ORDER_DEFAULT);
  }

/*****************************************************************************/
/********************** Check if I can create places *************************/
/*****************************************************************************/

static bool Plc_CheckIfICanCreatePlaces (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_INS_ADM);
  }

/*****************************************************************************/
/****************** Put contextual icons in list of places *******************/
/*****************************************************************************/

static void Plc_PutIconsListingPlaces (void)
  {
   /***** Put icon to edit places *****/
   if (Plc_CheckIfICanCreatePlaces ())
      Plc_PutIconToEditPlaces ();

   /***** Put icon to view centres *****/
   Ctr_PutIconToViewCentres ();
  }

/*****************************************************************************/
/********************** Put a link (form) to edit places *********************/
/*****************************************************************************/

static void Plc_PutIconToEditPlaces (void)
  {
   Ico_PutContextualIconToEdit (ActEdiPlc,NULL);
  }

/*****************************************************************************/
/*************************** Put forms to edit places ************************/
/*****************************************************************************/

void Plc_EditPlaces (void)
  {
   /***** Place constructor *****/
   Plc_EditingPlaceConstructor ();

   /***** Edit places *****/
   Plc_EditPlacesInternal ();

   /***** Place destructor *****/
   Plc_EditingPlaceDestructor ();
  }

static void Plc_EditPlacesInternal (void)
  {
   extern const char *Hlp_INSTITUTION_Places_edit;
   extern const char *Txt_Places;

   /***** Get list of places *****/
   Plc_GetListPlaces ();

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Places,Plc_PutIconsEditingPlaces,
                 Hlp_INSTITUTION_Places_edit,Box_NOT_CLOSABLE);

   /***** Put a form to create a new place *****/
   Plc_PutFormToCreatePlace ();

   /***** Forms to edit current places *****/
   if (Gbl.Plcs.Num)
      Plc_ListPlacesForEdition ();

   /***** End box *****/
   Box_EndBox ();

   /***** Free list of places *****/
   Plc_FreeListPlaces ();
  }


/*****************************************************************************/
/**************** Put contextual icons in edition of places *****************/
/*****************************************************************************/

static void Plc_PutIconsEditingPlaces (void)
  {
   /***** Put icon to view places *****/
   Plc_PutIconToViewPlaces ();

   /***** Put icon to view centres *****/
   Ctr_PutIconToViewCentres ();
  }

/*****************************************************************************/
/************************** Put icon to view places **************************/
/*****************************************************************************/

void Plc_PutIconToViewPlaces (void)
  {
   extern const char *Txt_Places;

   Lay_PutContextualLinkOnlyIcon (ActSeePlc,NULL,NULL,
				  "map-marker-alt.svg",
				  Txt_Places);
  }

/*****************************************************************************/
/**************************** List all the places ****************************/
/*****************************************************************************/

void Plc_GetListPlaces (void)
  {
   static const char *OrderBySubQuery[Plc_NUM_ORDERS] =
     {
      "FullName",		// Plc_ORDER_BY_PLACE
      "NumCtrs DESC,FullName",	// Plc_ORDER_BY_NUM_CTRS
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumPlc;
   struct Place *Plc;

   /***** Get places from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get places",
			     "(SELECT places.PlcCod,"
				     "places.ShortName,"
				     "places.FullName,"
				     "COUNT(*) AS NumCtrs"
			     " FROM places,centres"
			     " WHERE places.InsCod=%ld"
			     " AND places.PlcCod=centres.PlcCod"
			     " AND centres.InsCod=%ld"
			     " GROUP BY places.PlcCod)"
			     " UNION "
			     "(SELECT PlcCod,"
				     "ShortName,"
				     "FullName,"
				     "0 AS NumCtrs"
			     " FROM places"
			     " WHERE InsCod=%ld"
			     " AND PlcCod NOT IN"
			     " (SELECT DISTINCT PlcCod FROM centres"
			     " WHERE InsCod=%ld))"
			     " ORDER BY %s",
			     Gbl.Hierarchy.Ins.InsCod,
			     Gbl.Hierarchy.Ins.InsCod,
			     Gbl.Hierarchy.Ins.InsCod,
			     Gbl.Hierarchy.Ins.InsCod,
			     OrderBySubQuery[Gbl.Plcs.SelectedOrder]);

   /***** Count number of rows in result *****/
   if (NumRows) // Places found...
     {
      Gbl.Plcs.Num = (unsigned) NumRows;

      /***** Create list with courses in centre *****/
      if ((Gbl.Plcs.Lst = (struct Place *) calloc (NumRows,sizeof (struct Place))) == NULL)
          Lay_NotEnoughMemoryExit ();

      /***** Get the places *****/
      for (NumPlc = 0;
	   NumPlc < Gbl.Plcs.Num;
	   NumPlc++)
        {
         Plc = &(Gbl.Plcs.Lst[NumPlc]);

         /* Get next place */
         row = mysql_fetch_row (mysql_res);

         /* Get place code (row[0]) */
         if ((Plc->PlcCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of place.");

         /* Get the short name of the place (row[1]) */
         Str_Copy (Plc->ShrtName,row[1],
                   Plc_MAX_BYTES_PLACE_SHRT_NAME);

         /* Get the full name of the place (row[2]) */
         Str_Copy (Plc->FullName,row[2],
                   Plc_MAX_BYTES_PLACE_FULL_NAME);

         /* Get number of centres in this place (row[3]) */
         if (sscanf (row[3],"%u",&Plc->NumCtrs) != 1)
            Plc->NumCtrs = 0;
        }
     }
   else
      Gbl.Plcs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************************** Get place full name ****************************/
/*****************************************************************************/

void Plc_GetDataOfPlaceByCod (struct Place *Plc)
  {
   extern const char *Txt_Place_unspecified;
   extern const char *Txt_Another_place;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Plc->ShrtName[0] = '\0';
   Plc->FullName[0] = '\0';
   Plc->NumCtrs = 0;

   /***** Check if place code is correct *****/
   if (Plc->PlcCod < 0)
     {
      Str_Copy (Plc->ShrtName,Txt_Place_unspecified,
                Plc_MAX_BYTES_PLACE_SHRT_NAME);
      Str_Copy (Plc->FullName,Txt_Place_unspecified,
                Plc_MAX_BYTES_PLACE_FULL_NAME);
     }
   else if (Plc->PlcCod == 0)
     {
      Str_Copy (Plc->ShrtName,Txt_Another_place,
                Plc_MAX_BYTES_PLACE_SHRT_NAME);
      Str_Copy (Plc->FullName,Txt_Another_place,
                Plc_MAX_BYTES_PLACE_FULL_NAME);
     }
   else if (Plc->PlcCod > 0)
     {
      /***** Get data of a place from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a place",
			        "(SELECT places.ShortName,"
					"places.FullName,"
					"COUNT(*)"
				" FROM places,centres"
				" WHERE places.PlcCod=%ld"
				" AND places.PlcCod=centres.PlcCod"
				" AND centres.PlcCod=%ld"
				" GROUP BY places.PlcCod)"
				" UNION "
				"(SELECT ShortName,"
					"FullName,"
					"0"
				" FROM places"
				" WHERE PlcCod=%ld"
				" AND PlcCod NOT IN"
				" (SELECT DISTINCT PlcCod FROM centres))",
				Plc->PlcCod,
				Plc->PlcCod,
				Plc->PlcCod);

      /***** Count number of rows in result *****/
      if (NumRows) // Place found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the place (row[0]) */
         Str_Copy (Plc->ShrtName,row[0],
                   Plc_MAX_BYTES_PLACE_SHRT_NAME);

         /* Get the full name of the place (row[1]) */
         Str_Copy (Plc->FullName,row[1],
                   Plc_MAX_BYTES_PLACE_FULL_NAME);

         /* Get number of centres in this place (row[2]) */
         if (sscanf (row[2],"%u",&Plc->NumCtrs) != 1)
            Plc->NumCtrs = 0;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Free list of places ****************************/
/*****************************************************************************/

void Plc_FreeListPlaces (void)
  {
   if (Gbl.Plcs.Lst)
     {
      /***** Free memory used by the list of places in institution *****/
      free ((void *) Gbl.Plcs.Lst);
      Gbl.Plcs.Lst = NULL;
      Gbl.Plcs.Num = 0;
     }
  }

/*****************************************************************************/
/*************************** List all the places *****************************/
/*****************************************************************************/

static void Plc_ListPlacesForEdition (void)
  {
   unsigned NumPlc;
   struct Place *Plc;

   /***** Write heading *****/
   Tbl_TABLE_BeginWidePadding (2);
   Plc_PutHeadPlaces ();

   /***** Write all the places *****/
   for (NumPlc = 0;
	NumPlc < Gbl.Plcs.Num;
	NumPlc++)
     {
      Plc = &Gbl.Plcs.Lst[NumPlc];

      Tbl_TR_Begin (NULL);

      /* Put icon to remove place */
      Tbl_TD_Begin ("class=\"BM\"");
      if (Plc->NumCtrs)	// Place has centres ==> deletion forbidden
         Ico_PutIconRemovalNotAllowed ();
      else
        {
         Frm_StartForm (ActRemPlc);
         Plc_PutParamPlcCod (Plc->PlcCod);
         Ico_PutIconRemove ();
         Frm_EndForm ();
        }
      Tbl_TD_End ();

      /* Place code */
      Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%ld",Plc->PlcCod);
      Tbl_TD_End ();

      /* Place short name */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActRenPlcSho);
      Plc_PutParamPlcCod (Plc->PlcCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_SHORT_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Plc_MAX_CHARS_PLACE_SHRT_NAME,Plc->ShrtName,Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Place full name */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActRenPlcFul);
      Plc_PutParamPlcCod (Plc->PlcCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_FULL_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Plc_MAX_CHARS_PLACE_FULL_NAME,Plc->FullName,Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Number of centres */
      Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%u",Plc->NumCtrs);
      Tbl_TD_End ();

      Tbl_TR_End ();
     }

   /***** End table *****/
   Tbl_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write parameter with code of place *********************/
/*****************************************************************************/

static void Plc_PutParamPlcCod (long PlcCod)
  {
   Par_PutHiddenParamLong ("PlcCod",PlcCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of place **********************/
/*****************************************************************************/

long Plc_GetParamPlcCod (void)
  {
   /***** Get code of place *****/
   return Par_GetParToLong ("PlcCod");
  }

/*****************************************************************************/
/******************************* Remove a place ******************************/
/*****************************************************************************/

void Plc_RemovePlace (void)
  {
   extern const char *Txt_To_remove_a_place_you_must_first_remove_all_centres_of_that_place;
   extern const char *Txt_Place_X_removed;

   /***** Place constructor *****/
   Plc_EditingPlaceConstructor ();

   /***** Get place code *****/
   if ((Plc_EditingPlc->PlcCod = Plc_GetParamPlcCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of place is missing.");

   /***** Get data of the place from database *****/
   Plc_GetDataOfPlaceByCod (Plc_EditingPlc);

   /***** Check if this place has centres *****/
   if (Plc_EditingPlc->NumCtrs)	// Place has centres ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_a_place_you_must_first_remove_all_centres_of_that_place);
   else			// Place has no centres ==> remove it
     {
      /***** Remove place *****/
      DB_QueryDELETE ("can not remove a place",
		      "DELETE FROM places WHERE PlcCod=%ld",
		      Plc_EditingPlc->PlcCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_Place_X_removed,
                       Plc_EditingPlc->FullName);
     }
  }

/*****************************************************************************/
/********************* Change the short name of a place **********************/
/*****************************************************************************/

void Plc_RenamePlaceShort (void)
  {
   /***** Place constructor *****/
   Plc_EditingPlaceConstructor ();

   /***** Rename place *****/
   Plc_RenamePlace (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a place ***********************/
/*****************************************************************************/

void Plc_RenamePlaceFull (void)
  {
   /***** Place constructor *****/
   Plc_EditingPlaceConstructor ();

   /***** Rename place *****/
   Plc_RenamePlace (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a place *************************/
/*****************************************************************************/

static void Plc_RenamePlace (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_place_X_empty;
   extern const char *Txt_The_place_X_already_exists;
   extern const char *Txt_The_place_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_place_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentPlcName = NULL;		// Initialized to avoid warning
   char NewPlcName[Plc_MAX_BYTES_PLACE_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Plc_MAX_BYTES_PLACE_SHRT_NAME;
         CurrentPlcName = Plc_EditingPlc->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Plc_MAX_BYTES_PLACE_FULL_NAME;
         CurrentPlcName = Plc_EditingPlc->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the place */
   if ((Plc_EditingPlc->PlcCod = Plc_GetParamPlcCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of place is missing.");

   /* Get the new name for the place */
   Par_GetParToText (ParamName,NewPlcName,MaxBytes);

   /***** Get from the database the old names of the place *****/
   Plc_GetDataOfPlaceByCod (Plc_EditingPlc);

   /***** Check if new name is empty *****/
   if (!NewPlcName[0])
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_can_not_leave_the_name_of_the_place_X_empty,
                       CurrentPlcName);
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentPlcName,NewPlcName))	// Different names
        {
         /***** If place was in database... *****/
         if (Plc_CheckIfPlaceNameExists (ParamName,NewPlcName,Plc_EditingPlc->PlcCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_place_X_already_exists,
                             NewPlcName);
         else
           {
            /* Update the table changing old name by new name */
            Plc_UpdatePlcNameDB (Plc_EditingPlc->PlcCod,FieldName,NewPlcName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_place_X_has_been_renamed_as_Y,
                             CurrentPlcName,NewPlcName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_place_X_has_not_changed,
                          CurrentPlcName);
     }

   /***** Update place name *****/
   Str_Copy (CurrentPlcName,NewPlcName,
             MaxBytes);
  }

/*****************************************************************************/
/********************** Check if the name of place exists ********************/
/*****************************************************************************/

static bool Plc_CheckIfPlaceNameExists (const char *FieldName,const char *Name,long PlcCod)
  {
   /***** Get number of places with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a place"
			  " already existed",
			  "SELECT COUNT(*) FROM places"
			  " WHERE InsCod=%ld"
			  " AND %s='%s' AND PlcCod<>%ld",
			  Gbl.Hierarchy.Ins.InsCod,
			  FieldName,Name,PlcCod) != 0);
  }

/*****************************************************************************/
/****************** Update place name in table of places *********************/
/*****************************************************************************/

static void Plc_UpdatePlcNameDB (long PlcCod,const char *FieldName,const char *NewPlcName)
  {
   /***** Update place changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a place",
		   "UPDATE places SET %s='%s' WHERE PlcCod=%ld",
		   FieldName,NewPlcName,PlcCod);
  }

/*****************************************************************************/
/********** Show alerts after changing a place and continue editing **********/
/*****************************************************************************/

void Plc_ContEditAfterChgPlc (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Plc_EditPlacesInternal ();

   /***** Place destructor *****/
   Plc_EditingPlaceDestructor ();
  }

/*****************************************************************************/
/********************* Put a form to create a new place **********************/
/*****************************************************************************/

static void Plc_PutFormToCreatePlace (void)
  {
   extern const char *Txt_New_place;
   extern const char *Txt_Create_place;

   /***** Start form *****/
   Frm_StartForm (ActNewPlc);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_place,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Plc_PutHeadPlaces ();

   Tbl_TR_Begin (NULL);

   /***** Column to remove place, disabled here *****/
   Tbl_TD_Begin ("class=\"BM\"");
   Tbl_TD_End ();

   /***** Place code *****/
   Tbl_TD_Begin ("class=\"CODE\"");
   Tbl_TD_End ();

   /***** Place short name *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />",
            Plc_MAX_CHARS_PLACE_SHRT_NAME,Plc_EditingPlc->ShrtName);
   Tbl_TD_End ();

   /***** Place full name *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />",
            Plc_MAX_CHARS_PLACE_FULL_NAME,Plc_EditingPlc->FullName);
   Tbl_TD_End ();

   /***** Number of centres *****/
   Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"0");
   Tbl_TD_End ();

   Tbl_TR_End ();

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_place);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Write header with fields of a place ********************/
/*****************************************************************************/

static void Plc_PutHeadPlaces (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_Centres;

   Tbl_TR_Begin (NULL);

   Tbl_TH_Begin ("class=\"BM\"");
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_Code);
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_Short_name);
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_Full_name);
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_Centres);
   Tbl_TH_End ();

   Tbl_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new place **********************/
/*****************************************************************************/

void Plc_RecFormNewPlace (void)
  {
   extern const char *Txt_The_place_X_already_exists;
   extern const char *Txt_Created_new_place_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_place;

   /***** Place constructor *****/
   Plc_EditingPlaceConstructor ();

   /***** Get parameters from form *****/
   /* Get place short name */
   Par_GetParToText ("ShortName",Plc_EditingPlc->ShrtName,Plc_MAX_BYTES_PLACE_SHRT_NAME);

   /* Get place full name */
   Par_GetParToText ("FullName",Plc_EditingPlc->FullName,Plc_MAX_BYTES_PLACE_FULL_NAME);

   if (Plc_EditingPlc->ShrtName[0] &&
       Plc_EditingPlc->FullName[0])	// If there's a place name
     {
      /***** If name of place was in database... *****/
      if (Plc_CheckIfPlaceNameExists ("ShortName",Plc_EditingPlc->ShrtName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_place_X_already_exists,
                          Plc_EditingPlc->ShrtName);
      else if (Plc_CheckIfPlaceNameExists ("FullName",Plc_EditingPlc->FullName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_place_X_already_exists,
                          Plc_EditingPlc->FullName);
      else	// Add new place to database
        {
         Plc_CreatePlace (Plc_EditingPlc);
	 Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_place_X,
			  Plc_EditingPlc->FullName);
        }
     }
   else	// If there is not a place name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_place);
  }

/*****************************************************************************/
/**************************** Create a new place *****************************/
/*****************************************************************************/

static void Plc_CreatePlace (struct Place *Plc)
  {
   /***** Create a new place *****/
   DB_QueryINSERT ("can not create place",
		   "INSERT INTO places"
		   " (InsCod,ShortName,FullName)"
		   " VALUES"
		   " (%ld,'%s','%s')",
                   Gbl.Hierarchy.Ins.InsCod,Plc->ShrtName,Plc->FullName);
  }

/*****************************************************************************/
/************************* Place constructor/destructor **********************/
/*****************************************************************************/

static void Plc_EditingPlaceConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Plc_EditingPlc != NULL)
      Lay_ShowErrorAndExit ("Error initializing place.");

   /***** Allocate memory for place *****/
   if ((Plc_EditingPlc = (struct Place *) malloc (sizeof (struct Place))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for place.");

   /***** Reset place *****/
   Plc_EditingPlc->PlcCod      = -1L;
   Plc_EditingPlc->InsCod      = -1L;
   Plc_EditingPlc->ShrtName[0] = '\0';
   Plc_EditingPlc->FullName[0] = '\0';
   Plc_EditingPlc->NumCtrs     = 0;
  }

static void Plc_EditingPlaceDestructor (void)
  {
   /***** Free memory used for place *****/
   if (Plc_EditingPlc != NULL)
     {
      free ((void *) Plc_EditingPlc);
      Plc_EditingPlc = NULL;
     }
  }
