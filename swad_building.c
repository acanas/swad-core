// swad_building.c: buildings in a centre

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <malloc.h>		// For calloc, free
#include <stddef.h>		// For NULL
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_building.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"

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

static struct Bld_Building *Bld_EditingBuilding = NULL;	// Static variable to keep the building being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Bld_Order_t Bld_GetParamBuildingOrder (void);
static bool Bld_CheckIfICanCreateBuildings (void);
static void Bld_PutIconsListingBuildings (__attribute__((unused)) void *Args);
static void Bld_PutIconToEditBuildings (void);
static void Bld_PutIconsEditingBuildings (__attribute__((unused)) void *Args);

static void Bld_EditBuildingsInternal (void);

static void Bld_ListBuildingsForEdition (const struct Bld_Buildings *Buildings);
static void Bld_PutParamBldCod (void *BldCod);

static void Bld_RenameBuilding (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Bld_CheckIfBuildingNameExists (const char *FieldName,const char *Name,long BldCod);
static void Bld_UpdateBuildingNameDB (long BldCod,const char *FieldName,const char *NewBuildingName);

static void Bld_PutFormToCreateBuilding (void);
static void Bld_PutHeadBuildings (void);
static void Bld_CreateBuilding (struct Bld_Building *Building);

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
/************************** List all the buildings ***************************/
/*****************************************************************************/

void Bld_SeeBuildings (void)
  {
   extern const char *Hlp_CENTRE_Buildings;
   extern const char *Txt_Buildings;
   extern const char *Txt_BUILDINGS_HELP_ORDER[Bld_NUM_ORDERS];
   extern const char *Txt_BUILDINGS_ORDER[Bld_NUM_ORDERS];
   extern const char *Txt_New_building;
   struct Bld_Buildings Buildings;
   Bld_Order_t Order;
   unsigned NumBuilding;
   unsigned RowEvenOdd;

   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ctr.CtrCod <= 0)		// No centre selected
      return;

   /***** Reset buildings context *****/
   Bld_ResetBuildings (&Buildings);

   /***** Get parameter with the type of order in the list of buildings *****/
   Buildings.SelectedOrder = Bld_GetParamBuildingOrder ();

   /***** Get list of buildings *****/
   Bld_GetListBuildings (&Buildings,Bld_ALL_DATA);

   /***** Table head *****/
   Box_BoxBegin (NULL,Txt_Buildings,
                 Bld_PutIconsListingBuildings,NULL,
		 Hlp_CENTRE_Buildings,Box_NOT_CLOSABLE);
   HTM_TABLE_BeginWideMarginPadding (2);
   HTM_TR_Begin (NULL);
   for (Order  = (Bld_Order_t) 0;
	Order <= (Bld_Order_t) (Bld_NUM_ORDERS - 1);
	Order++)
     {
      HTM_TH_Begin (1,1,"LM");
      Frm_StartForm (ActSeeBld);
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
      HTM_BUTTON_SUBMIT_Begin (Txt_BUILDINGS_HELP_ORDER[Order],"BT_LINK TIT_TBL",NULL);
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
   for (NumBuilding = 0, RowEvenOdd = 1;
	NumBuilding < Buildings.Num;
	NumBuilding++, RowEvenOdd = 1 - RowEvenOdd)
     {
      HTM_TR_Begin (NULL);

      /* Short name */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Buildings.Lst[NumBuilding].ShrtName);
      HTM_TD_End ();

      /* Full name */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Buildings.Lst[NumBuilding].FullName);
      HTM_TD_End ();

      /* Location */
      HTM_TD_Begin ("class=\"DAT LM %s\"",Gbl.ColorRows[RowEvenOdd]);
      HTM_Txt (Buildings.Lst[NumBuilding].Location);
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Button to create building *****/
   if (Bld_CheckIfICanCreateBuildings ())
     {
      Frm_StartForm (ActEdiBld);
      Btn_PutConfirmButton (Txt_New_building);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of buildings *****/
   Bld_FreeListBuildings (&Buildings);
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of buildings *********/
/*****************************************************************************/

static Bld_Order_t Bld_GetParamBuildingOrder (void)
  {
   return (Bld_Order_t) Par_GetParToUnsignedLong ("Order",
						  0,
						  Bld_NUM_ORDERS - 1,
						  (unsigned long) Bld_ORDER_DEFAULT);
  }

/*****************************************************************************/
/********************* Check if I can create buildings ***********************/
/*****************************************************************************/

static bool Bld_CheckIfICanCreateBuildings (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM);
  }

/*****************************************************************************/
/**************** Put contextual icons in list of buildings ******************/
/*****************************************************************************/

static void Bld_PutIconsListingBuildings (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit buildings *****/
   if (Bld_CheckIfICanCreateBuildings ())
      Bld_PutIconToEditBuildings ();
  }

/*****************************************************************************/
/******************** Put a link (form) to edit buildings ********************/
/*****************************************************************************/

static void Bld_PutIconToEditBuildings (void)
  {
   Ico_PutContextualIconToEdit (ActEdiBld,NULL,
                                NULL,NULL);
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
   extern const char *Hlp_CENTRE_Buildings_edit;
   extern const char *Txt_Buildings;
   struct Bld_Buildings Buildings;

   /***** Reset buildings context *****/
   Bld_ResetBuildings (&Buildings);

   /***** Get list of buildings *****/
   Bld_GetListBuildings (&Buildings,Bld_ALL_DATA);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Buildings,
                 Bld_PutIconsEditingBuildings,NULL,
                 Hlp_CENTRE_Buildings_edit,Box_NOT_CLOSABLE);

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
   Bld_PutIconToViewBuildings ();
  }

/*****************************************************************************/
/************************ Put icon to view buildings *************************/
/*****************************************************************************/

void Bld_PutIconToViewBuildings (void)
  {
   extern const char *Txt_Buildings;

   Lay_PutContextualLinkOnlyIcon (ActSeeBld,NULL,
                                  NULL,NULL,
                                  "building.svg",
                                  Txt_Buildings);
  }

/*****************************************************************************/
/************************** List all the buildings ***************************/
/*****************************************************************************/

void Bld_GetListBuildings (struct Bld_Buildings *Buildings,
                           Bld_WhichData_t WhichData)
  {
   static const char *OrderBySubQuery[Bld_NUM_ORDERS] =
     {
      [Bld_ORDER_BY_SHRT_NAME] = "ShortName",
      [Bld_ORDER_BY_FULL_NAME] = "FullName",
      [Bld_ORDER_BY_LOCATION ] = "Location,ShortName",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumBuilding;
   struct Bld_Building *Building;

   /***** Get buildings from database *****/
   switch (WhichData)
     {
      case Bld_ALL_DATA:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get buildings",
				   "SELECT BldCod,"
					  "ShortName,"
					  "FullName,"
					  "Location"
				   " FROM buildings"
				   " WHERE CtrCod=%ld"
				   " ORDER BY %s",
				   Gbl.Hierarchy.Ctr.CtrCod,
				   OrderBySubQuery[Buildings->SelectedOrder]);
	 break;
      case Bld_ONLY_SHRT_NAME:
      default:
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get buildings",
				   "SELECT BldCod,"
					  "ShortName"
				   " FROM buildings"
				   " WHERE CtrCod=%ld"
				   " ORDER BY ShortName",
				   Gbl.Hierarchy.Ctr.CtrCod);
	 break;
     }

   /***** Count number of rows in result *****/
   if (NumRows) // Buildings found...
     {
      Buildings->Num = (unsigned) NumRows;

      /***** Create list with courses in centre *****/
      if ((Buildings->Lst = (struct Bld_Building *)
	                     calloc (NumRows,
	                             sizeof (struct Bld_Building))) == NULL)
          Lay_NotEnoughMemoryExit ();

      /***** Get the buildings *****/
      for (NumBuilding = 0;
	   NumBuilding < Buildings->Num;
	   NumBuilding++)
        {
         Building = &Buildings->Lst[NumBuilding];

         /* Get next building */
         row = mysql_fetch_row (mysql_res);

         /* Get building code (row[0]) */
         if ((Building->BldCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of building.");

         /* Get the short name of the building (row[1]) */
         Str_Copy (Building->ShrtName,row[1],
                   Bld_MAX_BYTES_SHRT_NAME);

         if (WhichData == Bld_ALL_DATA)
           {
	    /* Get the full name of the building (row[2]) */
	    Str_Copy (Building->FullName,row[2],
		      Bld_MAX_BYTES_FULL_NAME);

	    /* Get the full name of the building (row[3]) */
	    Str_Copy (Building->Location,row[3],
		      Bld_MAX_BYTES_LOCATION);
           }
        }
     }
   else
      Buildings->Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Get building full name **************************/
/*****************************************************************************/

void Bld_GetDataOfBuildingByCod (struct Bld_Building *Building)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Building->ShrtName[0] = '\0';
   Building->FullName[0] = '\0';
   Building->Location[0] = '\0';

   /***** Check if building code is correct *****/
   if (Building->BldCod > 0)
     {
      /***** Get data of a building from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a building",
			        "SELECT ShortName,"	// row[0]
				       "FullName,"	// row[1]
				       "Location"	// row[2]
				" FROM buildings"
				" WHERE BldCod=%ld",
				Building->BldCod);

      /***** Count number of rows in result *****/
      if (NumRows) // Building found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the building (row[0]) */
         Str_Copy (Building->ShrtName,row[0],
                   Bld_MAX_BYTES_SHRT_NAME);

         /* Get the full name of the building (row[1]) */
         Str_Copy (Building->FullName,row[1],
                   Bld_MAX_BYTES_FULL_NAME);

         /* Get the location of the building (row[2]) */
         Str_Copy (Building->Location,row[2],
                   Bld_MAX_BYTES_LOCATION);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
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
/************************* List all the buildings ****************************/
/*****************************************************************************/

static void Bld_ListBuildingsForEdition (const struct Bld_Buildings *Buildings)
  {
   unsigned NumBld;
   struct Bld_Building *Building;
   char *Anchor = NULL;

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Bld_PutHeadBuildings ();

   /***** Write all the buildings *****/
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
				     Bld_PutParamBldCod,&Building->BldCod);
      HTM_TD_End ();

      /* Building code */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_ARTICLE_Begin (Anchor);
      HTM_Long (Building->BldCod);
      HTM_ARTICLE_End ();
      HTM_TD_End ();

      /* Building short name */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartFormAnchor (ActRenBldSho,Anchor);
      Bld_PutParamBldCod (&Building->BldCod);
      HTM_INPUT_TEXT ("ShortName",Bld_MAX_CHARS_SHRT_NAME,Building->ShrtName,
                      HTM_SUBMIT_ON_CHANGE,
		      "size=\"10\" class=\"INPUT_SHORT_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Building full name */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartFormAnchor (ActRenBldFul,Anchor);
      Bld_PutParamBldCod (&Building->BldCod);
      HTM_INPUT_TEXT ("FullName",Bld_MAX_CHARS_FULL_NAME,Building->FullName,
                      HTM_SUBMIT_ON_CHANGE,
		      "size=\"20\" class=\"INPUT_FULL_NAME\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Building location */
      HTM_TD_Begin ("class=\"LM\"");
      Frm_StartFormAnchor (ActRenBldLoc,Anchor);
      Bld_PutParamBldCod (&Building->BldCod);
      HTM_INPUT_TEXT ("Location",Bld_MAX_CHARS_LOCATION,Building->Location,
                      HTM_SUBMIT_ON_CHANGE,
		      "size=\"15\" class=\"INPUT_FULL_NAME\"");
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

static void Bld_PutParamBldCod (void *BldCod)
  {
   if (BldCod)
      Par_PutHiddenParamLong (NULL,"BldCod",*((long *) BldCod));
  }

/*****************************************************************************/
/******************** Get parameter with code of building ********************/
/*****************************************************************************/

long Bld_GetParamBldCod (void)
  {
   /***** Get code of building *****/
   return Par_GetParToLong ("BldCod");
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
   if ((Bld_EditingBuilding->BldCod = Bld_GetParamBldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of building is missing.");

   /***** Get data of the building from database *****/
   Bld_GetDataOfBuildingByCod (Bld_EditingBuilding);

   /***** Update rooms assigned to this building *****/
   /* TODO: Uncomment when rooms be associated to building
   DB_QueryUPDATE ("can not update building in groups",
		   "UPDATE rooms SET BldCod=0"	// 0 means another building
		   " WHERE BldCod=%ld",
		   Bld_EditingBuilding->BldCod);
   */

   /***** Remove building *****/
   DB_QueryDELETE ("can not remove a building",
		   "DELETE FROM buildings WHERE BldCod=%ld",
		   Bld_EditingBuilding->BldCod);

   /***** Create message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Building_X_removed,
	            Bld_EditingBuilding->FullName);
  }

/*****************************************************************************/
/******************** Remove all buildings in a centre ***********************/
/*****************************************************************************/

void Bld_RemoveAllBuildingsInCtr (long CtrCod)
  {
   /***** Remove all buildings in centre *****/
   DB_QueryDELETE ("can not remove buildings",
		   "DELETE FROM buildings"
                   " WHERE CtrCod=%ld",
		   CtrCod);
  }

/*****************************************************************************/
/******************** Change the short name of a building ********************/
/*****************************************************************************/

void Bld_RenameBuildingShort (void)
  {
   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Rename building *****/
   Bld_RenameBuilding (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/******************** Change the full name of a building *********************/
/*****************************************************************************/

void Bld_RenameBuildingFull (void)
  {
   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Rename building *****/
   Bld_RenameBuilding (Cns_FULL_NAME);
  }

/*****************************************************************************/
/*********************** Change the name of a building ***********************/
/*****************************************************************************/

static void Bld_RenameBuilding (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_building_X_already_exists;
   extern const char *Txt_The_building_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_building_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentClaName = NULL;		// Initialized to avoid warning
   char NewClaName[Bld_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Bld_MAX_BYTES_SHRT_NAME;
         CurrentClaName = Bld_EditingBuilding->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Bld_MAX_BYTES_FULL_NAME;
         CurrentClaName = Bld_EditingBuilding->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the building */
   if ((Bld_EditingBuilding->BldCod = Bld_GetParamBldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of building is missing.");

   /* Get the new name for the building */
   Par_GetParToText (ParamName,NewClaName,MaxBytes);

   /***** Get from the database the old names of the building *****/
   Bld_GetDataOfBuildingByCod (Bld_EditingBuilding);

   /***** Check if new name is empty *****/
   if (NewClaName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentClaName,NewClaName))	// Different names
        {
         /***** If building was in database... *****/
         if (Bld_CheckIfBuildingNameExists (ParamName,NewClaName,Bld_EditingBuilding->BldCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_building_X_already_exists,
                             NewClaName);
         else
           {
            /* Update the table changing old name by new name */
            Bld_UpdateBuildingNameDB (Bld_EditingBuilding->BldCod,FieldName,NewClaName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_building_X_has_been_renamed_as_Y,
                             CurrentClaName,NewClaName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_building_X_has_not_changed,
                          CurrentClaName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update building name *****/
   Str_Copy (CurrentClaName,NewClaName,
             MaxBytes);
  }

/*****************************************************************************/
/******************** Check if the name of building exists *******************/
/*****************************************************************************/

static bool Bld_CheckIfBuildingNameExists (const char *FieldName,const char *Name,long BldCod)
  {
   /***** Get number of buildings with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a building"
			  " already existed",
			  "SELECT COUNT(*) FROM buildings"
			  " WHERE CtrCod=%ld"
			  " AND %s='%s' AND BldCod<>%ld",
			  Gbl.Hierarchy.Ctr.CtrCod,
			  FieldName,Name,BldCod) != 0);
  }

/*****************************************************************************/
/****************** Update building name in table of buildings *******************/
/*****************************************************************************/

static void Bld_UpdateBuildingNameDB (long BldCod,const char *FieldName,const char *NewBuildingName)
  {
   /***** Update building changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a building",
		   "UPDATE buildings SET %s='%s' WHERE BldCod=%ld",
		   FieldName,NewBuildingName,BldCod);
  }

/*****************************************************************************/
/******************** Change the location of a building **********************/
/*****************************************************************************/

void Bld_ChangeBuildingLocation (void)
  {
   extern const char *Txt_The_location_of_the_building_X_has_changed_to_Y;
   extern const char *Txt_The_location_of_the_building_X_has_not_changed;
   char NewLocation[Bld_MAX_BYTES_FULL_NAME + 1];

   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the building */
   if ((Bld_EditingBuilding->BldCod = Bld_GetParamBldCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of building is missing.");

   /* Get the new location for the building */
   Par_GetParToText ("Location",NewLocation,Bld_MAX_BYTES_LOCATION);

   /***** Get from the database the old location of the building *****/
   Bld_GetDataOfBuildingByCod (Bld_EditingBuilding);

   /***** Check if old and new locations are the same
	  (this happens when return is pressed without changes) *****/
   if (strcmp (Bld_EditingBuilding->Location,NewLocation))	// Different locations
     {
      /* Update the table changing old name by new name */
      Bld_UpdateBuildingNameDB (Bld_EditingBuilding->BldCod,"Location",NewLocation);
      Str_Copy (Bld_EditingBuilding->Location,NewLocation,
		Bld_MAX_BYTES_LOCATION);

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
   extern const char *Txt_New_building;
   extern const char *Txt_Create_building;

   /***** Begin form *****/
   Frm_StartForm (ActNewBld);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_building,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Bld_PutHeadBuildings ();

   HTM_TR_Begin (NULL);

   /***** Column to remove building, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Building code *****/
   HTM_TD_Begin ("class=\"CODE\"");
   HTM_TD_End ();

   /***** Building short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Bld_MAX_CHARS_SHRT_NAME,Bld_EditingBuilding->ShrtName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"10\" class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Building full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Bld_MAX_CHARS_FULL_NAME,Bld_EditingBuilding->FullName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"20\" class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Building location *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Location",Bld_MAX_CHARS_LOCATION,Bld_EditingBuilding->Location,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"15\" class=\"INPUT_FULL_NAME\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_building);

   /***** End form *****/
   Frm_EndForm ();
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

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH (1,1,"LM",Txt_Short_name);
   HTM_TH (1,1,"LM",Txt_Full_name);
   HTM_TH (1,1,"LM",Txt_Location);

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Receive form to create a new building *********************/
/*****************************************************************************/

void Bld_ReceiveFormNewBuilding (void)
  {
   extern const char *Txt_The_building_X_already_exists;
   extern const char *Txt_Created_new_building_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_building;

   /***** Building constructor *****/
   Bld_EditingBuildingConstructor ();

   /***** Get parameters from form *****/
   /* Get building short name */
   Par_GetParToText ("ShortName",Bld_EditingBuilding->ShrtName,Bld_MAX_BYTES_SHRT_NAME);

   /* Get building full name */
   Par_GetParToText ("FullName",Bld_EditingBuilding->FullName,Bld_MAX_BYTES_FULL_NAME);

   /* Get building location */
   Par_GetParToText ("Location",Bld_EditingBuilding->Location,Bld_MAX_BYTES_LOCATION);

   if (Bld_EditingBuilding->ShrtName[0] &&
       Bld_EditingBuilding->FullName[0])	// If there's a building name
     {
      /***** If name of building was in database... *****/
      if (Bld_CheckIfBuildingNameExists ("ShortName",Bld_EditingBuilding->ShrtName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_building_X_already_exists,
                          Bld_EditingBuilding->ShrtName);
      else if (Bld_CheckIfBuildingNameExists ("FullName",Bld_EditingBuilding->FullName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_building_X_already_exists,
                          Bld_EditingBuilding->FullName);
      else	// Add new building to database
        {
         Bld_CreateBuilding (Bld_EditingBuilding);
	 Ale_CreateAlert (Ale_SUCCESS,NULL,
	                  Txt_Created_new_building_X,
			  Bld_EditingBuilding->FullName);
        }
     }
   else	// If there is not a building name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_building);
  }

/*****************************************************************************/
/************************** Create a new building ****************************/
/*****************************************************************************/

static void Bld_CreateBuilding (struct Bld_Building *Building)
  {
   /***** Create a new building *****/
   DB_QueryINSERT ("can not create building",
		   "INSERT INTO buildings"
		   " (CtrCod,ShortName,FullName,Location)"
		   " VALUES"
		   " (%ld,'%s','%s','%s')",
                   Gbl.Hierarchy.Ctr.CtrCod,
		   Building->ShrtName,Building->FullName,Building->Location);
  }

/*****************************************************************************/
/************************ Building constructor/destructor ********************/
/*****************************************************************************/

static void Bld_EditingBuildingConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Bld_EditingBuilding != NULL)
      Lay_ShowErrorAndExit ("Error initializing building.");

   /***** Allocate memory for building *****/
   if ((Bld_EditingBuilding = (struct Bld_Building *) malloc (sizeof (struct Bld_Building))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for building.");

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
