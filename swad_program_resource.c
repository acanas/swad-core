// swad_program_resource.c: course program (resources)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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

#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_program_resource.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define PrgRsc_NUM_MOVEMENTS_UP_DOWN 2
typedef enum
  {
   PrgRsc_MOVE_UP,
   PrgRsc_MOVE_DOWN,
  } PrgRsc_MoveUpDown_t;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static long PrgSrc_RscCodToBeRemoved = -1L;

static const char *PrgRsc_RESOURCE_SECTION_ID = "rsc_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrgRsc_PutIconsViewResources (void *ItmCod);
static void PrgRsc_PutIconsEditResources (void *ItmCod);
static void PrgSrc_PutIconToViewResources (long ItmCod);
static void PrgSrc_PutIconToEditResources (long ItmCod);

static void PrgRsc_GetDataOfResourceByCod (struct PrgRsc_Resource *Resource);
static void PrgRsc_GetDataOfResource (struct PrgRsc_Resource *Resource,
                                      MYSQL_RES **mysql_res);
static void PrgRsc_ResetResource (struct PrgRsc_Resource *Resource);
static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         struct PrgRsc_Resource *Resource);
static void PrgRsc_WriteRowEditResource (unsigned NumRsc,unsigned NumResources,
                                         struct PrgRsc_Resource *Resource);
static void PrgRsc_PutFormsToRemEditOneResource (unsigned NumRsc,
                                                 unsigned NumResources,
                                                 struct PrgRsc_Resource *Resource);
static void PrgRsc_PutParams (void *RscCod);
static void PrgRsc_PutParamRscCod (long RscCod);
static long PrgRsc_GetParamRscCod (void);

static void PrgRsc_HideOrUnhideResource (bool Hide);

static void PrgRsc_MoveUpDownResource (PrgRsc_MoveUpDown_t UpDown);
static bool PrgRsc_ExchangeResources (const struct PrgRsc_Rsc *Rsc1,
                                      const struct PrgRsc_Rsc *Rsc2);

/*****************************************************************************/
/****************************** View resources *******************************/
/*****************************************************************************/

void PrgRsc_ViewResourcesAfterEdit (void)
  {
   long ItmCod;
   unsigned FormLevel;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get the code of the program item *****/
   if ((ItmCod = Prg_GetParamItmCod ()) > 0)
      FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (ItmCod));
   else
      FormLevel = 0;

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_END_EDIT_RES,NULL,-1L,ItmCod,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/****************************** Edit resources *******************************/
/*****************************************************************************/

void PrgRsc_EditResources (void)
  {
   long ItmCod;
   unsigned FormLevel;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get the code of the program item *****/
   if ((ItmCod = Prg_GetParamItmCod ()) > 0)
      FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (ItmCod));
   else
      FormLevel = 0;

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,NULL,-1L,ItmCod,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/****************************** List resources *******************************/
/*****************************************************************************/

void PrgRsc_ListItemResources (Prg_ListingType_t ListingType,long ItmCod)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Remove_resource;
   extern const char *Txt_Resources;
   MYSQL_RES *mysql_res;
   unsigned NumRsc;
   unsigned NumResources;
   struct PrgRsc_Resource Resource;
   static bool GetHiddenResources[Prg_NUM_LISTING_TYPES] =
     {
      [Prg_PRINT         ] = false,
      [Prg_VIEW          ] = false,
      [Prg_EDIT_LIST     ] = false,
      [Prg_NEW_ITEM      ] = false,
      [Prg_EDIT_ITEM     ] = false,
      [Prg_END_EDIT_ITEM ] = false,
      [Prg_EDIT_RESOURCES] = true,
      [Prg_END_EDIT_RES  ] = false,
     };
   static bool ShowListWhenEmpty[Prg_NUM_LISTING_TYPES] =
     {
      [Prg_PRINT         ] = false,
      [Prg_VIEW          ] = false,
      [Prg_EDIT_LIST     ] = true,
      [Prg_NEW_ITEM      ] = true,
      [Prg_EDIT_ITEM     ] = true,
      [Prg_END_EDIT_ITEM ] = true,
      [Prg_EDIT_RESOURCES] = true,
      [Prg_END_EDIT_RES  ] = true,
     };
   static bool FeaturedList[Prg_NUM_LISTING_TYPES] =
     {
      [Prg_PRINT         ] = false,
      [Prg_VIEW          ] = false,
      [Prg_EDIT_LIST     ] = false,
      [Prg_NEW_ITEM      ] = false,
      [Prg_EDIT_ITEM     ] = false,
      [Prg_END_EDIT_ITEM ] = false,
      [Prg_EDIT_RESOURCES] = true,
      [Prg_END_EDIT_RES  ] = true,
     };
   static void (*FunctionToDrawContextualIcons[Prg_NUM_LISTING_TYPES]) (void *Args) =
     {
      [Prg_PRINT         ] = NULL,
      [Prg_VIEW          ] = NULL,
      [Prg_EDIT_LIST     ] = PrgRsc_PutIconsEditResources,
      [Prg_NEW_ITEM      ] = PrgRsc_PutIconsEditResources,
      [Prg_EDIT_ITEM     ] = PrgRsc_PutIconsEditResources,
      [Prg_END_EDIT_ITEM ] = PrgRsc_PutIconsEditResources,
      [Prg_EDIT_RESOURCES] = PrgRsc_PutIconsViewResources,
      [Prg_END_EDIT_RES  ] = PrgRsc_PutIconsEditResources,
     };

   /***** Trivial check *****/
   if (ItmCod <= 0)
      return;

   /***** Get list of item resources from database *****/
   NumResources = Prg_DB_GetListResources (&mysql_res,ItmCod,
                                           GetHiddenResources[ListingType]);

   if (NumResources || ShowListWhenEmpty[ListingType])
     {
      /***** Begin section *****/
      if (FeaturedList[ListingType])
	 HTM_SECTION_Begin (PrgRsc_RESOURCE_SECTION_ID);

      /***** Show possible alerts *****/
      if (FeaturedList[ListingType])
	 switch (Gbl.Action.Act)
	   {
	    case ActReqRemPrgRsc:
	       /* Alert with button to remove resource */
	       Ale_ShowLastAlertAndButton (ActRemPrgRsc,PrgRsc_RESOURCE_SECTION_ID,NULL,
					   PrgRsc_PutParams,&PrgSrc_RscCodToBeRemoved,
					   Btn_REMOVE_BUTTON,Txt_Remove_resource);
	       break;
	    default:
	       Ale_ShowAlerts (PrgRsc_RESOURCE_SECTION_ID);
	       break;
	   }

      /***** Begin box *****/
      Box_BoxBegin ("100%",Txt_Resources,
		    FunctionToDrawContextualIcons[ListingType],&ItmCod,
		    Hlp_COURSE_Program,Box_NOT_CLOSABLE);

	 /***** Table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);
	    HTM_TBODY_Begin (NULL);

	       /***** Write all item resources *****/
	       for (NumRsc = 0;
		    NumRsc < NumResources;
		    NumRsc++)
		 {
		  /* Get data of this item resource */
		  PrgRsc_GetDataOfResource (&Resource,&mysql_res);

		  /* Show item */
		  switch (ListingType)
		    {
		     case Prg_EDIT_RESOURCES:
			PrgRsc_WriteRowEditResource (NumRsc,NumResources,&Resource);
			break;
		     default:
			PrgRsc_WriteRowViewResource (NumRsc,&Resource);
			break;
		    }

		  The_ChangeRowColor ();
		 }

	    /***** End table *****/
	    HTM_TBODY_End ();
	 HTM_TABLE_End ();

      /***** End box *****/
      Box_BoxEnd ();

      /***** End section *****/
      if (FeaturedList[ListingType])
	 HTM_SECTION_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Put contextual icons in list of item resources ***************/
/*****************************************************************************/

static void PrgRsc_PutIconsViewResources (void *ItmCod)
  {
   /***** Put icon to create a new item resource *****/
   if (ItmCod)
      if (*((long *) ItmCod) > 0)
	 if (Prg_CheckIfICanEditProgram ())
	    PrgSrc_PutIconToViewResources (*((long *) ItmCod));
  }

static void PrgRsc_PutIconsEditResources (void *ItmCod)
  {
   /***** Put icon to create a new item resource *****/
   if (ItmCod)
      if (*((long *) ItmCod) > 0)
	 if (Prg_CheckIfICanEditProgram ())
	    PrgSrc_PutIconToEditResources (*((long *) ItmCod));
  }

/*****************************************************************************/
/********************* Put icon to edit item resources ***********************/
/*****************************************************************************/

static void PrgSrc_PutIconToViewResources (long ItmCod)
  {
   Ico_PutContextualIconToView (ActFrmSeePrgRsc,PrgRsc_RESOURCE_SECTION_ID,
                                Prg_PutParams,&ItmCod);
  }

static void PrgSrc_PutIconToEditResources (long ItmCod)
  {
   Ico_PutContextualIconToEdit (ActFrmEdiPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
                                Prg_PutParams,&ItmCod);
  }

/*****************************************************************************/
/****************** Get item resource data using its code ********************/
/*****************************************************************************/

static void PrgRsc_GetDataOfResourceByCod (struct PrgRsc_Resource *Resource)
  {
   MYSQL_RES *mysql_res;

   if (Resource->Rsc.Cod > 0)
     {
      /***** Get data of item resource *****/
      if (Prg_DB_GetDataOfResourceByCod (&mysql_res,Resource->Rsc.Cod))
         PrgRsc_GetDataOfResource (Resource,&mysql_res);
      else
         PrgRsc_ResetResource (Resource);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Clear all item resource data *****/
      PrgRsc_ResetResource (Resource);
  }

/*****************************************************************************/
/************************* Get item resource data ****************************/
/*****************************************************************************/

static void PrgRsc_GetDataOfResource (struct PrgRsc_Resource *Resource,
                                      MYSQL_RES **mysql_res)
  {
   MYSQL_ROW row;

   /***** Get data of item resource from database *****/
   /* Get row */
   row = mysql_fetch_row (*mysql_res);
   /*
   ItmCod	row[0]
   RscCod	row[1]
   RscInd	row[2]
   Hidden	row[3]
   Title	row[4]
   */
   /* Get code of the program item (row[0]) */
   Resource->ItmCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Get code and index of the item resource (row[1], row[2]) */
   Resource->Rsc.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Resource->Rsc.Ind = Str_ConvertStrToUnsigned (row[2]);

   /* Get whether the program item is hidden (row(3)) */
   Resource->Hidden = (row[3][0] == 'Y');

   /* Get the title of the item resource (row[4]) */
   Str_Copy (Resource->Title,row[4],sizeof (Resource->Title) - 1);
  }

/*****************************************************************************/
/************************ Clear all item resource data ***********************/
/*****************************************************************************/

static void PrgRsc_ResetResource (struct PrgRsc_Resource *Resource)
  {
   Resource->ItmCod   = -1L;
   Resource->Rsc.Cod  = -1L;
   Resource->Rsc.Ind  = 0;
   Resource->Hidden   = false;
   Resource->Title[0] = '\0';
  }

/*****************************************************************************/
/************************** Show one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         struct PrgRsc_Resource *Resource)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 HTM_Unsigned (NumRsc + 1);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN PRG_RSC_%s %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 HTM_Txt (Resource->Title);
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Edit one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowEditResource (unsigned NumRsc,unsigned NumResources,
                                         struct PrgRsc_Resource *Resource)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item resource *****/
      HTM_TD_Begin ("class=\"PRG_COL1 LT %s\"",The_GetColorRows ());
	 PrgRsc_PutFormsToRemEditOneResource (NumRsc,NumResources,Resource);
      HTM_TD_End ();

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows ());
	 HTM_Unsigned (NumRsc + 1);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN %s\"",The_GetColorRows ());
	 Frm_BeginFormAnchor (ActRenPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
	    PrgRsc_PutParamRscCod (Resource->Rsc.Cod);
	    HTM_INPUT_TEXT ("Title",PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE,Resource->Title,
			    HTM_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_FULL_NAME INPUT_%s\"",
			    The_GetSuffix ());
	 Frm_EndForm ();
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void PrgRsc_PutFormsToRemEditOneResource (unsigned NumRsc,
                                                 unsigned NumResources,
                                                 struct PrgRsc_Resource *Resource)
  {
   static Act_Action_t ActionHideUnhide[2] =
     {
      [false] = ActHidPrgRsc,	// Visible ==> action to hide
      [true ] = ActUnhPrgRsc,	// Hidden ==> action to unhide
     };
   extern const char *Txt_Movement_not_allowed;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Icon to remove item resource *****/
	 Ico_PutContextualIconToRemove (ActReqRemPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                PrgRsc_PutParams,&Resource->Rsc.Cod);

	 /***** Icon to hide/unhide item resource *****/
	 Ico_PutContextualIconToHideUnhide (ActionHideUnhide,PrgRsc_RESOURCE_SECTION_ID,
					    PrgRsc_PutParams,&Resource->Rsc.Cod,
					    Resource->Hidden);

	 /***** Icon to edit program resource *****/
	 // Ico_PutContextualIconToEdit (ActFrmChgPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	 //                              PrgRsc_PutParams,&Resource->Rsc.Cod,);

	 /***** Icon to add a new resource after this resource *****/
	 // Ico_PutContextualIconToAdd (ActFrmNewPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	 //                             PrgRsc_PutParams,&Resource->Rsc.Cod,);

	 /***** Icon to move up the item *****/
	 if (NumRsc > 0)
	   Lay_PutContextualLinkOnlyIcon (ActUp_PrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                  PrgRsc_PutParams,&Resource->Rsc.Cod,
	 			          "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the item *****/
	 if (NumRsc < NumResources - 1)
	   Lay_PutContextualLinkOnlyIcon (ActDwnPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                  PrgRsc_PutParams,&Resource->Rsc.Cod,
	                                  "arrow-down.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,Txt_Movement_not_allowed);
	 break;
      case Rol_STD:
      case Rol_NET:
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/******************** Params used to edit a program item *********************/
/*****************************************************************************/

static void PrgRsc_PutParams (void *RscCod)
  {
   if (RscCod)
      if (*((long *) RscCod) > 0)
	 PrgRsc_PutParamRscCod (*((long *) RscCod));
  }

/*****************************************************************************/
/**************** Write parameter with code of program item ******************/
/*****************************************************************************/

static void PrgRsc_PutParamRscCod (long RscCod)
  {
   Par_PutHiddenParamLong (NULL,"RscCod",RscCod);
  }

/*****************************************************************************/
/***************** Get parameter with code of item resource ******************/
/*****************************************************************************/

static long PrgRsc_GetParamRscCod (void)
  {
   return Par_GetParToLong ("RscCod");
  }

/*****************************************************************************/
/******************************** Rename resource ****************************/
/*****************************************************************************/

void PrgRsc_RenameResource (void)
  {
   struct PrgRsc_Resource Resource;
   char NewTitle[PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE + 1];
   long ItmCod;
   unsigned FormLevel;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Resource.Rsc.Cod = PrgRsc_GetParamRscCod ();
   PrgRsc_GetDataOfResourceByCod (&Resource);
   if (Resource.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Rename resource *****/
   /* Get the new title for the resource */
   Par_GetParToText ("Title",NewTitle,PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE);

   /* Update database changing old title by new title */
   Prg_DB_UpdateResourceTitle (Resource.Rsc.Cod,Resource.ItmCod,NewTitle);

   /***** Get the code of the program item *****/
   ItmCod = Resource.ItmCod;
   FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (Resource.ItmCod));

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,NULL,-1L,ItmCod,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/************ Ask for confirmation of removing an item resource **************/
/*****************************************************************************/

void PrgRsc_ReqRemResource (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_resource_X;
   struct PrgRsc_Resource Resource;
   long ItmCod;
   unsigned FormLevel;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Resource.Rsc.Cod = PrgRsc_GetParamRscCod ();
   PrgRsc_GetDataOfResourceByCod (&Resource);
   if (Resource.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Create alert to remove the item resource *****/
   Ale_CreateAlert (Ale_QUESTION,PrgRsc_RESOURCE_SECTION_ID,
                    Txt_Do_you_really_want_to_remove_the_resource_X,
                    Resource.Title);
   PrgSrc_RscCodToBeRemoved = Resource.Rsc.Cod;

   /***** Get the code of the program item *****/
   ItmCod = Resource.ItmCod;
   FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (Resource.ItmCod));

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,NULL,-1L,ItmCod,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/**************************** Remove an item resource ************************/
/*****************************************************************************/

void PrgRsc_RemoveResource (void)
  {
   extern const char *Txt_Resource_X_removed;
   struct PrgRsc_Resource Resource;
   long ItmCod;
   unsigned FormLevel;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Resource.Rsc.Cod = PrgRsc_GetParamRscCod ();
   PrgRsc_GetDataOfResourceByCod (&Resource);
   if (Resource.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Remove resource *****/
   Prg_DB_RemoveResource (&Resource);

   /***** Create alert to remove the item resource *****/
   Ale_CreateAlert (Ale_SUCCESS,PrgRsc_RESOURCE_SECTION_ID,
                    Txt_Resource_X_removed,Resource.Title);

   /***** Get the code of the program item *****/
   ItmCod = Resource.ItmCod;
   FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (Resource.ItmCod));

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,NULL,-1L,ItmCod,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/***************************** Hide a program item ***************************/
/*****************************************************************************/

void PrgRsc_HideResource (void)
  {
   PrgRsc_HideOrUnhideResource (true);
  }

void PrgRsc_UnhideResource (void)
  {
   PrgRsc_HideOrUnhideResource (false);
  }

static void PrgRsc_HideOrUnhideResource (bool Hide)
  {
   struct PrgRsc_Resource Resource;
   long ItmCod;
   unsigned FormLevel;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Resource.Rsc.Cod = PrgRsc_GetParamRscCod ();
   PrgRsc_GetDataOfResourceByCod (&Resource);
   if (Resource.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Hide/unhide item resource *****/
   Prg_DB_HideOrUnhideResource (Resource.Rsc.Cod,Hide);

   /***** Get the code of the program item *****/
   ItmCod = Resource.ItmCod;
   FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (Resource.ItmCod));

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,NULL,-1L,ItmCod,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/**************************** Move up/down resource **************************/
/*****************************************************************************/

void PrgRsc_MoveUpResource (void)
  {
   PrgRsc_MoveUpDownResource (PrgRsc_MOVE_UP);
  }

void PrgRsc_MoveDownResource (void)
  {
   PrgRsc_MoveUpDownResource (PrgRsc_MOVE_DOWN);
  }

static void PrgRsc_MoveUpDownResource (PrgRsc_MoveUpDown_t UpDown)
  {
   extern const char *Txt_Movement_not_allowed;
   struct PrgRsc_Resource Resource;
   struct PrgRsc_Rsc Rsc2;
   long ItmCod;
   unsigned FormLevel;
   bool Success = false;
   static unsigned (*GetOtherRscInd[PrgRsc_NUM_MOVEMENTS_UP_DOWN])(long ItmCod,unsigned RscInd) =
     {
      [PrgRsc_MOVE_UP  ] = Prg_DB_GetRscIndBefore,
      [PrgRsc_MOVE_DOWN] = Prg_DB_GetRscIndAfter,
     };

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Resource.Rsc.Cod = PrgRsc_GetParamRscCod ();
   PrgRsc_GetDataOfResourceByCod (&Resource);
   if (Resource.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Move up/down resource *****/
   if ((Rsc2.Ind = GetOtherRscInd[UpDown] (Resource.ItmCod,Resource.Rsc.Ind)))	// 0 ==> movement not allowed
     {
      /* Get the other resource code */
      Rsc2.Cod = Prg_DB_GetRscCodFromRscInd (Resource.ItmCod,Rsc2.Ind);

      /* Exchange subtrees */
      Success = PrgRsc_ExchangeResources (&Resource.Rsc,&Rsc2);
     }
   if (!Success)
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Get the code of the program item *****/
   ItmCod = Resource.ItmCod;
   FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (Resource.ItmCod));

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,NULL,-1L,ItmCod,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/**** Exchange the order of two consecutive subtrees in a course program *****/
/*****************************************************************************/
// Return true if success

static bool PrgRsc_ExchangeResources (const struct PrgRsc_Rsc *Rsc1,
                                      const struct PrgRsc_Rsc *Rsc2)
  {
   if (Rsc1->Ind > 0 &&	// Indexes should be in the range [1, 2,...]
       Rsc2->Ind > 0)
     {
      /***** Lock table to make the move atomic *****/
      Prg_DB_LockTableResources ();

      /***** Exchange indexes of items *****/
      // This implementation works with non continuous indexes
      /*
      Example:
      Rsc1->Ind =  5
      Rsc2->Ind = 17
                                Step 1            Step 2            Step 3  (Equivalent to)
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      |Rsc.Ind|Rsc.Cod|   |Rsc.Ind|Rsc.Cod|   |Rsc.Ind|Rsc.Cod|   |Rsc.Ind|Rsc.Cod| |Rsc.Ind|Rsc.Cod|
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      |     5 |   218 |   |     5 |   218 |-->|--> 17 |   218 |   |    17 |   218 | |     5 |   240 |
      |    17 |   240 |-->|-->-17 |   240 |   |   -17 |   240 |-->|-->  5 |   240 | |    17 |   218 |
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      */
      /* Step 1: Change second index to negative,
		 necessary to preserve unique index (ItmCod,RscInd) */
      Prg_DB_UpdateRscInd (Rsc2->Cod,-(int) Rsc2->Ind);

      /* Step 2: Change first index */
      Prg_DB_UpdateRscInd (Rsc1->Cod, (int) Rsc2->Ind);

      /* Step 3: Change second index */
      Prg_DB_UpdateRscInd (Rsc2->Cod, (int) Rsc1->Ind);

      /***** Unlock table *****/
      DB_UnlockTables ();

      return true;	// Success
     }

   return false;	// No success
  }
