// swad_place.c: places

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stddef.h>		// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_center_database.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_place.h"
#include "swad_place_database.h"

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

static const bool Plc_ICanEditPlaces[Rol_NUM_ROLES] =
  {
   /* Users who can edit */
   [Rol_INS_ADM] = true,
   [Rol_SYS_ADM] = true,
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Plc_Place *Plc_EditingPlc = NULL;	// Static variable to keep the place being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Plc_Order_t Plc_GetParPlcOrder (void);
static void Plc_PutIconsListingPlaces (__attribute__((unused)) void *Args);
static void Plc_PutIconToEditPlaces (void);
static void Plc_EditPlacesInternal (void);
static void Plc_PutIconsEditingPlaces (__attribute__((unused)) void *Args);

static void Plc_GetPlaceDataFromRow (MYSQL_RES *mysql_res,struct Plc_Place *Plc);

static void Plc_ListPlacesForEdition (const struct Plc_Places *Places);
static void Plc_PutParPlcCod (void *PlcCod);

static void Plc_RenamePlace (Cns_ShrtOrFullName_t ShrtOrFullName);

static void Plc_PutFormToCreatePlace (void);
static void Plc_PutHeadPlaces (void);

static void Plc_EditingPlaceConstructor (void);
static void Plc_EditingPlaceDestructor (void);

/*****************************************************************************/
/**************************** Reset places context ***************************/
/*****************************************************************************/

void Plc_ResetPlaces (struct Plc_Places *Places)
  {
   Places->Num           = 0;
   Places->Lst           = NULL;
   Places->SelectedOrder = Plc_ORDER_DEFAULT;
  }

/*****************************************************************************/
/***************************** List all places *******************************/
/*****************************************************************************/

void Plc_SeeAllPlaces (void)
  {
   extern const char *Hlp_INSTITUTION_Places;
   extern const char *Txt_Places;
   extern const char *Txt_PLACES_HELP_ORDER[2];
   extern const char *Txt_PLACES_ORDER[2];
   extern const char *Txt_Other_places;
   extern const char *Txt_Place_unspecified;
   struct Plc_Places Places;
   Plc_Order_t Order;
   unsigned NumPlc;
   unsigned NumCtrsWithPlc = 0;
   unsigned NumCtrsInOtherPlcs;

   if (Gbl.Hierarchy.Node[HieLvl_INS].HieCod > 0)
     {
      /***** Reset places context *****/
      Plc_ResetPlaces (&Places);

      /***** Get parameter with the type of order in the list of places *****/
      Places.SelectedOrder = Plc_GetParPlcOrder ();

      /***** Get list of places *****/
      Plc_GetListPlaces (&Places);

      /***** Table head *****/
      Box_BoxBegin (NULL,Txt_Places,
                    Plc_PutIconsListingPlaces,NULL,
                    Hlp_INSTITUTION_Places,Box_NOT_CLOSABLE);
	 HTM_TABLE_BeginWideMarginPadding (2);
	    HTM_TR_Begin (NULL);
	    for (Order  = (Plc_Order_t) 0;
		 Order <= (Plc_Order_t) (Plc_NUM_ORDERS - 1);
		 Order++)
	      {
               HTM_TH_Begin (HTM_HEAD_LEFT);

		  Frm_BeginForm (ActSeePlc);
		     Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		     HTM_BUTTON_Submit_Begin (Txt_PLACES_HELP_ORDER[Order],
		                              "class=\"BT_LINK\"");
			if (Order == Places.SelectedOrder)
			   HTM_U_Begin ();
			HTM_Txt (Txt_PLACES_ORDER[Order]);
			if (Order == Places.SelectedOrder)
			   HTM_U_End ();
		     HTM_BUTTON_End ();
		  Frm_EndForm ();

	       HTM_TH_End ();
	      }
	    HTM_TR_End ();

	    /***** Write all places and their nuber of centers *****/
	    for (NumPlc = 0;
		 NumPlc < Places.Num;
		 NumPlc++)
	      {
	       /* Write data of this place */
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("class=\"LM DAT_%s\"",
		                The_GetSuffix ());
		     HTM_Txt (Places.Lst[NumPlc].FullName);
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"RM DAT_%s\"",
		                The_GetSuffix ());
		     HTM_Unsigned (Places.Lst[NumPlc].NumCtrs);
		  HTM_TD_End ();

	       HTM_TR_End ();
	       NumCtrsWithPlc += Places.Lst[NumPlc].NumCtrs;
	      }

	    /***** Separation row *****/
	    HTM_TR_Begin (NULL);
	       HTM_TD_Begin ("colspan=\"2\" class=\"DAT_%s\"",
	                     The_GetSuffix ());
		  HTM_NBSP ();
	       HTM_TD_End ();
	    HTM_TR_End ();

	    /***** Write centers (of the current institution) with other place *****/
	    NumCtrsInOtherPlcs = Ctr_DB_GetNumCtrsInPlc (0);
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM DAT_%s\"",
	                     The_GetSuffix ());
		  HTM_Txt (Txt_Other_places);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"RM DAT_%s\"",
	                     The_GetSuffix ());
		  HTM_Unsigned (NumCtrsInOtherPlcs);
	       HTM_TD_End ();

	    HTM_TR_End ();
	    NumCtrsWithPlc += NumCtrsInOtherPlcs;

	    /***** Write centers (of the current institution) with no place *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
		  HTM_Txt (Txt_Place_unspecified);
	       HTM_TD_End ();

	       HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
		  HTM_Unsigned (Ctr_GetNumCtrsInIns (Gbl.Hierarchy.Node[HieLvl_INS].HieCod) -
				NumCtrsWithPlc);
	       HTM_TD_End ();

	    HTM_TR_End ();

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End box *****/
      Box_BoxEnd ();

      /***** Free list of places *****/
      Plc_FreeListPlaces (&Places);
     }
  }

/*****************************************************************************/
/********** Get parameter with the type or order in list of places ***********/
/*****************************************************************************/

static Plc_Order_t Plc_GetParPlcOrder (void)
  {
   return (Plc_Order_t) Par_GetParUnsignedLong ("Order",
						0,
						Plc_NUM_ORDERS - 1,
						(unsigned long) Plc_ORDER_DEFAULT);
  }

/*****************************************************************************/
/****************** Put contextual icons in list of places *******************/
/*****************************************************************************/

static void Plc_PutIconsListingPlaces (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit places *****/
   if (Plc_ICanEditPlaces[Gbl.Usrs.Me.Role.Logged])
      Plc_PutIconToEditPlaces ();
  }

/*****************************************************************************/
/********************** Put a link (form) to edit places *********************/
/*****************************************************************************/

static void Plc_PutIconToEditPlaces (void)
  {
   Ico_PutContextualIconToEdit (ActEdiPlc,NULL,
                                NULL,NULL);
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
   struct Plc_Places Places;

   /***** Reset places context *****/
   Plc_ResetPlaces (&Places);

   /***** Get list of places *****/
   Plc_GetListPlaces (&Places);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Places,
                 Plc_PutIconsEditingPlaces,NULL,
                 Hlp_INSTITUTION_Places_edit,Box_NOT_CLOSABLE);

   /***** Put a form to create a new place *****/
   Plc_PutFormToCreatePlace ();

   /***** Forms to edit current places *****/
   if (Places.Num)
      Plc_ListPlacesForEdition (&Places);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of places *****/
   Plc_FreeListPlaces (&Places);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of places *****************/
/*****************************************************************************/

static void Plc_PutIconsEditingPlaces (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view places *****/
   Ico_PutContextualIconToView (ActSeePlc,NULL,
				NULL,NULL);
  }

/*****************************************************************************/
/************************** Put icon to view places **************************/
/*****************************************************************************/

void Plc_PutIconToViewPlaces (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeePlc,NULL,
                                  Ins_PutParInsCod,&Gbl.Hierarchy.Node[HieLvl_INS].HieCod,
				  "map-marker-alt.svg",Ico_BLACK);
  }

/*****************************************************************************/
/******************************* List all places *****************************/
/*****************************************************************************/

void Plc_GetListPlaces (struct Plc_Places *Places)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPlc;

   /***** Get places from database *****/
   if ((Places->Num = Plc_DB_GetListPlaces (&mysql_res,Places->SelectedOrder))) // Places found...
     {
      /***** Create list with courses in center *****/
      if ((Places->Lst = calloc ((size_t) Places->Num,
                                 sizeof (*Places->Lst))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the places *****/
      for (NumPlc = 0;
	   NumPlc < Places->Num;
	   NumPlc++)
         Plc_GetPlaceDataFromRow (mysql_res,&Places->Lst[NumPlc]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get place data using its code **********************/
/*****************************************************************************/

void Plc_GetPlaceDataByCod (struct Plc_Place *Plc)
  {
   extern const char *Txt_Place_unspecified;
   extern const char *Txt_Another_place;
   MYSQL_RES *mysql_res;

   /***** Clear data *****/
   Plc->ShrtName[0] = '\0';
   Plc->FullName[0] = '\0';
   Plc->NumCtrs = 0;

   /***** Check if place code is correct *****/
   if (Plc->PlcCod < 0)
     {
      Str_Copy (Plc->ShrtName,Txt_Place_unspecified,sizeof (Plc->ShrtName) - 1);
      Str_Copy (Plc->FullName,Txt_Place_unspecified,sizeof (Plc->FullName) - 1);
     }
   else if (Plc->PlcCod == 0)
     {
      Str_Copy (Plc->ShrtName,Txt_Another_place    ,sizeof (Plc->ShrtName) - 1);
      Str_Copy (Plc->FullName,Txt_Another_place    ,sizeof (Plc->FullName) - 1);
     }
   else if (Plc->PlcCod > 0)
     {
      /***** Get data of a place from database *****/
      if (Plc_DB_GetPlaceDataByCod (&mysql_res,Plc->PlcCod)) // Place found...
         Plc_GetPlaceDataFromRow (mysql_res,Plc);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/********************** Get place data from database row *********************/
/*****************************************************************************/

static void Plc_GetPlaceDataFromRow (MYSQL_RES *mysql_res,struct Plc_Place *Plc)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get place code (row[0]) *****/
   if ((Plc->PlcCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongPlaceExit ();

   /***** Get the short (row[1]) and full (row[2]) names of the place *****/
   Str_Copy (Plc->ShrtName,row[1],sizeof (Plc->ShrtName) - 1);
   Str_Copy (Plc->FullName,row[2],sizeof (Plc->FullName) - 1);

   /***** Get number of centers in this place (row[3]) *****/
   if (sscanf (row[3],"%u",&Plc->NumCtrs) != 1)
      Plc->NumCtrs = 0;
  }

/*****************************************************************************/
/**************************** Free list of places ****************************/
/*****************************************************************************/

void Plc_FreeListPlaces (struct Plc_Places *Places)
  {
   if (Places->Lst)
     {
      /***** Free memory used by the list of places in institution *****/
      free (Places->Lst);
      Places->Lst = NULL;
      Places->Num = 0;
     }
  }

/*****************************************************************************/
/***************************** List all places *******************************/
/*****************************************************************************/

static void Plc_ListPlacesForEdition (const struct Plc_Places *Places)
  {
   unsigned NumPlc;
   struct Plc_Place *Plc;

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Write heading *****/
      Plc_PutHeadPlaces ();

      /***** Write all places *****/
      for (NumPlc = 0;
	   NumPlc < Places->Num;
	   NumPlc++)
	{
	 Plc = &Places->Lst[NumPlc];

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove place */
	    HTM_TD_Begin ("class=\"BM\"");
	       if (Plc->NumCtrs)	// Place has centers ==> deletion forbidden
		  Ico_PutIconRemovalNotAllowed ();
	       else
		  Ico_PutContextualIconToRemove (ActRemPlc,NULL,
						 Plc_PutParPlcCod,&Plc->PlcCod);
	    HTM_TD_End ();

	    /* Place code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Plc->PlcCod);
	    HTM_TD_End ();

	    /* Place short name */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenPlcSho);
		  ParCod_PutPar (ParCod_Plc,Plc->PlcCod);
		  HTM_INPUT_TEXT ("ShortName",Plc_MAX_CHARS_PLACE_SHRT_NAME,Plc->ShrtName,
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_SHORT_NAME INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Place full name */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenPlcFul);
		  ParCod_PutPar (ParCod_Plc,Plc->PlcCod);
		  HTM_INPUT_TEXT ("FullName",Plc_MAX_CHARS_PLACE_FULL_NAME,Plc->FullName,
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_FULL_NAME INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Number of centers */
	    HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	       HTM_Unsigned (Plc->NumCtrs);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write parameter with code of place *********************/
/*****************************************************************************/

static void Plc_PutParPlcCod (void *PlcCod)
  {
   if (PlcCod)
      ParCod_PutPar (ParCod_Plc,*((long *) PlcCod));
  }

/*****************************************************************************/
/******************************* Remove a place ******************************/
/*****************************************************************************/

void Plc_RemovePlace (void)
  {
   extern const char *Txt_To_remove_a_place_you_must_first_remove_all_centers_of_that_place;
   extern const char *Txt_Place_X_removed;

   /***** Place constructor *****/
   Plc_EditingPlaceConstructor ();

   /***** Get place code *****/
   Plc_EditingPlc->PlcCod = ParCod_GetAndCheckPar (ParCod_Plc);

   /***** Get place data from database *****/
   Plc_GetPlaceDataByCod (Plc_EditingPlc);

   /***** Check if this place has centers *****/
   if (Plc_EditingPlc->NumCtrs)	// Place has centers ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_a_place_you_must_first_remove_all_centers_of_that_place);
   else			// Place has no centers ==> remove it
     {
      /***** Remove place *****/
      Plc_DB_RemovePlace (Plc_EditingPlc->PlcCod);

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
   extern const char *Txt_The_place_X_already_exists;
   extern const char *Txt_The_place_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   const char *ParName = NULL;	// Initialized to avoid warning
   const char *FldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;	// Initialized to avoid warning
   char *CurrentPlcName = NULL;	// Initialized to avoid warning
   char NewPlcName[Plc_MAX_BYTES_PLACE_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParName = "ShortName";
         FldName = "ShortName";
         MaxBytes = Plc_MAX_BYTES_PLACE_SHRT_NAME;
         CurrentPlcName = Plc_EditingPlc->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParName = "FullName";
         FldName = "FullName";
         MaxBytes = Plc_MAX_BYTES_PLACE_FULL_NAME;
         CurrentPlcName = Plc_EditingPlc->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the place */
   Plc_EditingPlc->PlcCod = ParCod_GetAndCheckPar (ParCod_Plc);

   /* Get the new name for the place */
   Par_GetParText (ParName,NewPlcName,MaxBytes);

   /***** Get place old names from database  *****/
   Plc_GetPlaceDataByCod (Plc_EditingPlc);

   /***** Check if new name is empty *****/
   if (NewPlcName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentPlcName,NewPlcName))	// Different names
        {
         /***** If place was in database... *****/
         if (Plc_DB_CheckIfPlaceNameExists (Plc_EditingPlc->PlcCod,ParName,NewPlcName))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_place_X_already_exists,
                             NewPlcName);
         else
           {
            /* Update the table changing old name by new name */
            Plc_DB_UpdatePlcName (Plc_EditingPlc->PlcCod,FldName,NewPlcName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_place_X_has_been_renamed_as_Y,
                             CurrentPlcName,NewPlcName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,CurrentPlcName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update place name *****/
   Str_Copy (CurrentPlcName,NewPlcName,MaxBytes);
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
   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewPlc,NULL,NULL,NULL);

      /***** Write heading *****/
      Plc_PutHeadPlaces ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove place, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Place code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Place short name *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    HTM_INPUT_TEXT ("ShortName",Plc_MAX_CHARS_PLACE_SHRT_NAME,Plc_EditingPlc->ShrtName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_SHORT_NAME INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Place full name *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    HTM_INPUT_TEXT ("FullName",Plc_MAX_CHARS_PLACE_FULL_NAME,Plc_EditingPlc->FullName,
			    HTM_DONT_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_FULL_NAME INPUT_%s\""
			    " required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Number of centers *****/
	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (0);
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
  }

/*****************************************************************************/
/******************** Write header with fields of a place ********************/
/*****************************************************************************/

static void Plc_PutHeadPlaces (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_Centers;

   HTM_TR_Begin (NULL);
      HTM_TH_Span (NULL     ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code      ,HTM_HEAD_RIGHT );
      HTM_TH (Txt_Short_name,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Full_name ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Centers   ,HTM_HEAD_RIGHT );
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new place **********************/
/*****************************************************************************/

void Plc_ReceiveFormNewPlace (void)
  {
   extern const char *Txt_The_place_X_already_exists;
   extern const char *Txt_Created_new_place_X;

   /***** Place constructor *****/
   Plc_EditingPlaceConstructor ();

   /***** Get parameters from form *****/
   /* Get place short name */
   Par_GetParText ("ShortName",Plc_EditingPlc->ShrtName,Plc_MAX_BYTES_PLACE_SHRT_NAME);

   /* Get place full name */
   Par_GetParText ("FullName",Plc_EditingPlc->FullName,Plc_MAX_BYTES_PLACE_FULL_NAME);

   if (Plc_EditingPlc->ShrtName[0] &&
       Plc_EditingPlc->FullName[0])	// If there's a place name
     {
      /***** If name of place was in database... *****/
      if (Plc_DB_CheckIfPlaceNameExists (-1L,"ShortName",Plc_EditingPlc->ShrtName))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_place_X_already_exists,
                          Plc_EditingPlc->ShrtName);
      else if (Plc_DB_CheckIfPlaceNameExists (-1L,"FullName",Plc_EditingPlc->FullName))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_place_X_already_exists,
                          Plc_EditingPlc->FullName);
      else	// Add new place to database
        {
         Plc_DB_CreatePlace (Plc_EditingPlc);
	 Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_place_X,
			  Plc_EditingPlc->FullName);
        }
     }
   else	// If there is not a place name
      Ale_CreateAlertYouMustSpecifyTheShortNameAndTheFullName ();
  }

/*****************************************************************************/
/************************* Place constructor/destructor **********************/
/*****************************************************************************/

static void Plc_EditingPlaceConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Plc_EditingPlc != NULL)
      Err_WrongPlaceExit ();

   /***** Allocate memory for place *****/
   if ((Plc_EditingPlc = malloc (sizeof (*Plc_EditingPlc))) == NULL)
      Err_NotEnoughMemoryExit ();

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
      free (Plc_EditingPlc);
      Plc_EditingPlc = NULL;
     }
  }
