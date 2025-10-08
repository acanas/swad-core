// swad_building.c: buildings in a center

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <malloc.h>		// For calloc, free
#include <stddef.h>		// For NULL
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_building.h"
#include "swad_building_database.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_room_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Bld_Building *Bld_EditingBuilding = NULL;	// Static variable to keep the building being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Bld_Order_t Bld_GetParBuildingOrder (void);
static Usr_Can_t Bld_CheckIfICanCreateBuildings (void);
static void Bld_PutIconsListingBuildings (__attribute__((unused)) void *Args);
static void Bld_PutIconToEditBuildings (void);
static void Bld_PutIconsEditingBuildings (__attribute__((unused)) void *Args);

static void Bld_EditBuildingsInternal (void);

static void Bld_GetBuildingDataFromRow (MYSQL_RES *mysql_res,
                                        struct Bld_Building *Building,
                                        Bld_WhichData_t WhichData);

static void Bld_ListBuildingsForEdition (const struct Bld_Buildings *Buildings);
static void Bld_PutParBldCod (void *BldCod);

static void Bld_RenameBuilding (Nam_ShrtOrFullName_t ShrtOrFull);

static void Bld_PutFormToCreateBuilding (void);
static void Bld_PutHeadBuildings (void);

static void Bld_EditingBuildingConstructor (void);
static void Bld_EditingBuildingDestructor (void);

/*****************************************************************************/
/************************** Reset buildings context **************************/
/*****************************************************************************/

void Bld_ResetBuildings (struct Bld_Buildings *Buildings)
  {
   Buildings->Num           = 0;	// Number of buildings
   Buildings->Lst           = NULL;	// List of buildings
   Buildings->SelectedOrder = Bld_ORDER_DEFAULT;
  }

/*****************************************************************************/
/****************************** List all buildings ***************************/
/*****************************************************************************/

void Bld_SeeBuildings (void)
  {
   extern const char *Hlp_CENTER_Buildings;
   extern const char *Txt_Buildings;
   extern const char *Txt_BUILDINGS_HELP_ORDER[Bld_NUM_ORDERS];
   extern const char *Txt_BUILDINGS_ORDER[Bld_NUM_ORDERS];
   struct Bld_Buildings Buildings;
   Bld_Order_t Order;
   unsigned NumBuilding;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Node[Hie_CTR].HieCod <= 0)		// No center selected
      return;

   /***** Reset buildings context *****/
   Bld_ResetBuildings (&Buildings);

   /***** Get parameter with the type of order in the list of buildings *****/
   Buildings.SelectedOrder = Bld_GetParBuildingOrder ();

   /***** Get list of buildings *****/
   Bld_GetListBuildings (&Buildings,Bld_ALL_DATA);

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_Buildings,Bld_PutIconsListingBuildings,NULL,
		      Hlp_CENTER_Buildings,Box_NOT_CLOSABLE,2);

      HTM_TR_Begin (NULL);
	 for (Order  = (Bld_Order_t) 0;
	      Order <= (Bld_Order_t) (Bld_NUM_ORDERS - 1);
	      Order++)
	   {
	    HTM_TH_Begin (HTM_HEAD_LEFT);
	       Frm_BeginForm (ActSeeBld);
		  Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
		  HTM_BUTTON_Submit_Begin (Txt_BUILDINGS_HELP_ORDER[Order],NULL,
					   "class=\"BT_LINK\"");
		     if (Order == Buildings.SelectedOrder)
			HTM_U_Begin ();
		     HTM_Txt (Txt_BUILDINGS_ORDER[Order]);
		     if (Order == Buildings.SelectedOrder)
			HTM_U_End ();
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	    HTM_TH_End ();
	   }
      HTM_TR_End ();

      /***** Write list of buildings *****/
      for (NumBuilding = 0, The_ResetRowColor ();
	   NumBuilding < Buildings.Num;
	   NumBuilding++, The_ChangeRowColor ())
	{
	 HTM_TR_Begin (NULL);

	    /* Short name */
	    HTM_TD_Begin ("class=\"LM DAT_%s %s\"",
			  The_GetSuffix (),The_GetColorRows ());
	       HTM_Txt (Buildings.Lst[NumBuilding].ShrtName);
	    HTM_TD_End ();

	    /* Full name */
	    HTM_TD_Begin ("class=\"LM DAT_%s %s\"",
			  The_GetSuffix (),The_GetColorRows ());
	       HTM_Txt (Buildings.Lst[NumBuilding].FullName);
	    HTM_TD_End ();

	    /* Location */
	    HTM_TD_Begin ("class=\"LM DAT_%s %s\"",
			  The_GetSuffix (),The_GetColorRows ());
	       HTM_Txt (Buildings.Lst[NumBuilding].Location);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free list of buildings *****/
   Bld_FreeListBuildings (&Buildings);
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of buildings *********/
/*****************************************************************************/

static Bld_Order_t Bld_GetParBuildingOrder (void)
  {
   return (Bld_Order_t) Par_GetParUnsignedLong ("Order",
						0,
						Bld_NUM_ORDERS - 1,
						(unsigned long) Bld_ORDER_DEFAULT);
  }

/*****************************************************************************/
/********************* Check if I can create buildings ***********************/
/*****************************************************************************/

static Usr_Can_t Bld_CheckIfICanCreateBuildings (void)
  {
   return Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM ? Usr_CAN :
						   Usr_CAN_NOT;
  }

/*****************************************************************************/
/**************** Put contextual icons in list of buildings ******************/
/*****************************************************************************/

static void Bld_PutIconsListingBuildings (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit buildings *****/
   if (Bld_CheckIfICanCreateBuildings () == Usr_CAN)
      Bld_PutIconToEditBuildings ();
  }

/*****************************************************************************/
/******************** Put a link (form) to edit buildings ********************/
/*****************************************************************************/

static void Bld_PutIconToEditBuildings (void)
  {
   Ico_PutContextualIconToEdit (ActEdiBld,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************************* Put forms to edit buildings ***********************/
/*****************************************************************************/

void Bld_EditBuildings (void)
  {
   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Edit buildings *****/
   Bld_EditBuildingsInternal ();

   /***** Building destructor *****/
   Bld_EditingBuildingDestructor ();
  }

static void Bld_EditBuildingsInternal (void)
  {
   extern const char *Hlp_CENTER_Buildings_edit;
   extern const char *Txt_Buildings;
   struct Bld_Buildings Buildings;

   /***** Reset buildings context *****/
   Bld_ResetBuildings (&Buildings);

   /***** Get list of buildings *****/
   Bld_GetListBuildings (&Buildings,Bld_ALL_DATA);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Buildings,Bld_PutIconsEditingBuildings,NULL,
                 Hlp_CENTER_Buildings_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new building *****/
      Bld_PutFormToCreateBuilding ();

      /***** Forms to edit current buildings *****/
      if (Buildings.Num)
	 Bld_ListBuildingsForEdition (&Buildings);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of buildings *****/
   Bld_FreeListBuildings (&Buildings);
  }

/*****************************************************************************/
/*************** Put contextual icons in edition of buildings ****************/
/*****************************************************************************/

static void Bld_PutIconsEditingBuildings (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view buildings *****/
   Ico_PutContextualIconToView (ActSeeBld,NULL,NULL,NULL);
  }

/*****************************************************************************/
/***************************** List all buildings ****************************/
/*****************************************************************************/

void Bld_GetListBuildings (struct Bld_Buildings *Buildings,
                           Bld_WhichData_t WhichData)
  {
   MYSQL_RES *mysql_res;
   unsigned NumBuilding;

   /***** Get buildings from database *****/
   Buildings->Num = Bld_DB_GetListBuildings (&mysql_res,WhichData,
                                             Buildings->SelectedOrder);

   /***** Count number of rows in result *****/
   if (Buildings->Num) // Buildings found...
     {
      /***** Create list with buldings in center *****/
      if ((Buildings->Lst = calloc ((size_t) Buildings->Num,
                                    sizeof (*Buildings->Lst))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the buildings *****/
      for (NumBuilding = 0;
	   NumBuilding < Buildings->Num;
	   NumBuilding++)
	 Bld_GetBuildingDataFromRow (mysql_res,&Buildings->Lst[NumBuilding],
	                             WhichData);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get building data using its code *********************/
/*****************************************************************************/

void Bld_GetBuildingDataByCod (struct Bld_Building *Building)
  {
   MYSQL_RES *mysql_res;

   /***** Clear data *****/
   Building->ShrtName[0] = '\0';
   Building->FullName[0] = '\0';
   Building->Location[0] = '\0';

   /***** Check if building code is correct *****/
   if (Building->BldCod > 0)
     {
      /***** Get data of a building from database *****/
      if (Bld_DB_GetBuildingDataByCod (&mysql_res,
				       Building->BldCod) == Exi_EXISTS) // Building found...
         Bld_GetBuildingDataFromRow (mysql_res,Building,Bld_ALL_DATA);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/******************** Get building data from database row ********************/
/*****************************************************************************/

static void Bld_GetBuildingDataFromRow (MYSQL_RES *mysql_res,
                                        struct Bld_Building *Building,
                                        Bld_WhichData_t WhichData)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get building code (row[0]) *****/
   if ((Building->BldCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongBuildingExit ();

   /***** Get the short name of the building (row[1]) *****/
   Str_Copy (Building->ShrtName,row[1],sizeof (Building->ShrtName) - 1);

   if (WhichData == Bld_ALL_DATA)
     {
      /***** Get full name (row[2])
             and location (row[3]) of the building *****/
      Str_Copy (Building->FullName,row[2],sizeof (Building->FullName) - 1);
      Str_Copy (Building->Location,row[3],sizeof (Building->Location) - 1);
     }
  }

/*****************************************************************************/
/************************** Free list of buildings ***************************/
/*****************************************************************************/

void Bld_FreeListBuildings (struct Bld_Buildings *Buildings)
  {
   if (Buildings->Lst)
     {
      /***** Free memory used by the list of buildings in institution *****/
      free (Buildings->Lst);
      Buildings->Lst = NULL;
      Buildings->Num = 0;
     }
  }

/*****************************************************************************/
/**************************** List all buildings *****************************/
/*****************************************************************************/

static void Bld_ListBuildingsForEdition (const struct Bld_Buildings *Buildings)
  {
   static Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = ActRenBldSho,
      [Nam_FULL_NAME] = ActRenBldFul,
     };
   unsigned NumBld;
   struct Bld_Building *Building;
   char *Anchor = NULL;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Bld_PutHeadBuildings ();

      /***** Write all buildings *****/
      for (NumBld = 0;
	   NumBld < Buildings->Num;
	   NumBld++)
	{
	 Building = &Buildings->Lst[NumBld];

	 /* Build anchor string */
	 Frm_SetAnchorStr (Building->BldCod,&Anchor);

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove building */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToRemove (ActRemBld,NULL,
					      Bld_PutParBldCod,&Building->BldCod);
	    HTM_TD_End ();

	    /* Building code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_ARTICLE_Begin (Anchor);
		  HTM_Long (Building->BldCod);
	       HTM_ARTICLE_End ();
	    HTM_TD_End ();

	    /* Building short name and full name */
	    Names[Nam_SHRT_NAME] = Building->ShrtName;
	    Names[Nam_FULL_NAME] = Building->FullName;
	    Nam_ExistingShortAndFullNames (ActionRename,
				           ParCod_Bld,Building->BldCod,
				           Names,
				           Frm_PUT_FORM);

	    /* Building location */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginFormAnchor (ActRenBldLoc,Anchor);
		  ParCod_PutPar (ParCod_Bld,Building->BldCod);
		  HTM_INPUT_TEXT ("Location",Bld_MAX_CHARS_LOCATION,Building->Location,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"15\" class=\"INPUT_FULL_NAME INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************* Write parameter with code of building *******************/
/*****************************************************************************/

static void Bld_PutParBldCod (void *BldCod)
  {
   if (BldCod)
      ParCod_PutPar (ParCod_Bld,*((long *) BldCod));
  }

/*****************************************************************************/
/****************************** Remove a building ****************************/
/*****************************************************************************/

void Bld_RemoveBuilding (void)
  {
   extern const char *Txt_Building_X_removed;

   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Get building code *****/
   Bld_EditingBuilding->BldCod = ParCod_GetAndCheckPar (ParCod_Bld);

   /***** Get data of the building from database *****/
   Bld_GetBuildingDataByCod (Bld_EditingBuilding);

   /***** Update rooms assigned to this building *****/
   Roo_DB_RemoveBuildingFromRooms (Bld_EditingBuilding->BldCod);

   /***** Remove building *****/
   Bld_DB_RemoveBuilding (Bld_EditingBuilding->BldCod);

   /***** Create message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Building_X_removed,
	            Bld_EditingBuilding->FullName);
  }

/*****************************************************************************/
/******************** Change the short name of a building ********************/
/*****************************************************************************/

void Bld_RenameBuildingShrt (void)
  {
   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Rename building *****/
   Bld_RenameBuilding (Nam_SHRT_NAME);
  }

/*****************************************************************************/
/******************** Change the full name of a building *********************/
/*****************************************************************************/

void Bld_RenameBuildingFull (void)
  {
   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Rename building *****/
   Bld_RenameBuilding (Nam_FULL_NAME);
  }

/*****************************************************************************/
/*********************** Change the name of a building ***********************/
/*****************************************************************************/

static void Bld_RenameBuilding (Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   extern const char *Txt_The_building_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Bld_EditingBuilding->ShrtName,
      [Nam_FULL_NAME] = Bld_EditingBuilding->FullName,
     };
   char NewName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get parameters from form *****/
   /* Get the code of the building */
   Bld_EditingBuilding->BldCod = ParCod_GetAndCheckPar (ParCod_Bld);

   /* Get the new name for the building */
   Nam_GetParShrtOrFullName (ShrtOrFull,NewName);

   /***** Get from the database the old names of the building *****/
   Bld_GetBuildingDataByCod (Bld_EditingBuilding);

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
         /***** If building was in database... *****/
         switch (Bld_DB_CheckIfBuildingNameExists (Nam_Fields[ShrtOrFull],
					           NewName,Bld_EditingBuilding->BldCod,
					           -1L,0))	// Unused
           {
            case Exi_EXISTS:
               Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,NewName);
               break;
            case Exi_DOES_NOT_EXIST:
            default:
	       /* Update the table changing old name by new name */
	       Bld_DB_UpdateBuildingField (Bld_EditingBuilding->BldCod,
					   Nam_Fields[ShrtOrFull],NewName);

	       /* Write message to show the change made */
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
				Txt_The_building_X_has_been_renamed_as_Y,
				CurrentName[ShrtOrFull],NewName);
	       break;
           }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,Txt_The_name_X_has_not_changed,
                          CurrentName[ShrtOrFull]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update building name *****/
   Str_Copy (CurrentName[ShrtOrFull],NewName,Nam_MaxBytes[ShrtOrFull]);
  }

/*****************************************************************************/
/******************** Change the location of a building **********************/
/*****************************************************************************/

void Bld_ChangeBuildingLoc (void)
  {
   extern const char *Txt_The_location_of_the_building_X_has_changed_to_Y;
   extern const char *Txt_The_location_of_the_building_X_has_not_changed;
   char NewLocation[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the building */
   Bld_EditingBuilding->BldCod = ParCod_GetAndCheckPar (ParCod_Bld);

   /* Get the new location for the building */
   Par_GetParText ("Location",NewLocation,Bld_MAX_BYTES_LOCATION);

   /***** Get from the database the old location of the building *****/
   Bld_GetBuildingDataByCod (Bld_EditingBuilding);

   /***** Check if old and new locations are the same
	  (this happens when return is pressed without changes) *****/
   if (strcmp (Bld_EditingBuilding->Location,NewLocation))	// Different locations
     {
      /* Update the table changing old name by new name */
      Bld_DB_UpdateBuildingField (Bld_EditingBuilding->BldCod,
				  "Location",NewLocation);
      Str_Copy (Bld_EditingBuilding->Location,NewLocation,
		sizeof (Bld_EditingBuilding->Location) - 1);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_location_of_the_building_X_has_changed_to_Y,
		       Bld_EditingBuilding->FullName,NewLocation);
     }
   else	// The same location
      Ale_CreateAlert (Ale_INFO,NULL,
	               Txt_The_location_of_the_building_X_has_not_changed,
		       Bld_EditingBuilding->FullName);
  }

/*****************************************************************************/
/******* Show alerts after changing a building and continue editing **********/
/*****************************************************************************/

void Bld_ContEditAfterChgBuilding (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Bld_EditBuildingsInternal ();

   /***** Building destructor *****/
   Bld_EditingBuildingDestructor ();
  }

/*****************************************************************************/
/******************** Put a form to create a new building ********************/
/*****************************************************************************/

static void Bld_PutFormToCreateBuilding (void)
  {
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewBld,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Bld_PutHeadBuildings ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove building, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Building code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Building short name and full name *****/
	 Names[Nam_SHRT_NAME] = Bld_EditingBuilding->ShrtName;
	 Names[Nam_FULL_NAME] = Bld_EditingBuilding->FullName;
	 Nam_NewShortAndFullNames (Names);

	 /***** Building location *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("Location",Bld_MAX_CHARS_LOCATION,Bld_EditingBuilding->Location,
			    HTM_NO_ATTR,
			    "size=\"15\" class=\"INPUT_FULL_NAME INPUT_%s\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
  }

/*****************************************************************************/
/****************** Write header with fields of a building *******************/
/*****************************************************************************/

static void Bld_PutHeadBuildings (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_Location;

   HTM_TR_Begin (NULL);

      HTM_TH_Span (NULL     ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code      ,HTM_HEAD_RIGHT);
      HTM_TH (Txt_Short_name,HTM_HEAD_LEFT );
      HTM_TH (Txt_Full_name ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Location  ,HTM_HEAD_LEFT );

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Receive form to create a new building *********************/
/*****************************************************************************/

void Bld_ReceiveNewBuilding (void)
  {
   extern const char *Txt_Created_new_building_X;
   char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Get parameters from form *****/
   /* Get building short name and full name */
   Names[Nam_SHRT_NAME] = Bld_EditingBuilding->ShrtName;
   Names[Nam_FULL_NAME] = Bld_EditingBuilding->FullName;
   Nam_GetParsShrtAndFullName (Names);

   /* Get building location */
   Par_GetParText ("Location",Bld_EditingBuilding->Location,Bld_MAX_BYTES_LOCATION);

   if (Bld_EditingBuilding->ShrtName[0] &&
       Bld_EditingBuilding->FullName[0])	// If there's a building name
     {
      /***** If name of building was not in database... *****/
      if (Nam_CheckIfNameExists (Bld_DB_CheckIfBuildingNameExists,
				 (const char **) Names,
				 -1L,
				 -1L,				// Unused
				 0) == Exi_DOES_NOT_EXIST)	// Unused
        {
         Bld_DB_CreateBuilding (Bld_EditingBuilding);
	 Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_building_X,
			  Names[Nam_FULL_NAME]);
        }
     }
   else	// If there is not a building name
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
  }

/*****************************************************************************/
/************************ Building constructor/destructor ********************/
/*****************************************************************************/

static void Bld_EditingBuildingConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Bld_EditingBuilding != NULL)
      Err_WrongBuildingExit ();

   /***** Allocate memory for building *****/
   if ((Bld_EditingBuilding = malloc (sizeof (*Bld_EditingBuilding))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset building *****/
   Bld_EditingBuilding->BldCod      = -1L;
   Bld_EditingBuilding->InsCod      = -1L;
   Bld_EditingBuilding->ShrtName[0] = '\0';
   Bld_EditingBuilding->FullName[0] = '\0';
   Bld_EditingBuilding->Location[0] = '\0';
  }

static void Bld_EditingBuildingDestructor (void)
  {
   /***** Free memory used for building *****/
   if (Bld_EditingBuilding != NULL)
     {
      free (Bld_EditingBuilding);
      Bld_EditingBuilding = NULL;
     }
  }
