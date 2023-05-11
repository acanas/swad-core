// swad_program_resource.c: course program (resources)

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
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdio.h>		// For asprintf

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_button.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/****************************************************************************/

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

static const char *PrgRsc_RESOURCE_SECTION_ID = "rsc_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrgRsc_PutIconsViewResources (void *ItmCod);
static void PrgRsc_PutIconsEditResources (void *ItmCod);

static void PrgRsc_GetResourceDataFromRow (MYSQL_RES *mysql_res,
                                           struct Prg_Item *Item);
static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         const struct Prg_Item *Item);
static void PrgRsc_WriteRowEditResource (unsigned NumRsc,unsigned NumResources,
                                         struct Prg_Item *Item,bool EditLink);
static void PrgRsc_WriteRowNewResource (unsigned NumResources,
                                        struct Prg_Item *Item,bool EditLink);
static void PrgRsc_PutFormsToRemEditOneResource (struct Prg_Item *Item,
                                                 unsigned NumRsc,
                                                 unsigned NumResources);
static void PrgRsc_PutParRscCod (void *RscCod);
static void PrgRsc_HideOrUnhideResource (bool Hide);

static void PrgRsc_MoveUpDownResource (PrgRsc_MoveUpDown_t UpDown);
static bool PrgRsc_ExchangeResources (const struct Prg_ResourceHierarchy *Rsc1,
                                      const struct Prg_ResourceHierarchy *Rsc2);

static void PrgRsc_ShowClipboard (void);
static void PrgRsc_PutIconsClipboard (__attribute__((unused)) void *Args);

/*****************************************************************************/
/****************************** View resources *******************************/
/*****************************************************************************/

void PrgRsc_ViewResourcesAfterEdit (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_END_EDIT_RES,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/****************************** Edit resources *******************************/
/*****************************************************************************/

void PrgRsc_EditResources (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/****************************** List resources *******************************/
/*****************************************************************************/

void PrgRsc_ListItemResources (Prg_ListingType_t ListingType,
                               struct Prg_Item *Item,
                               long SelectedItmCod,
                               long SelectedRscCod)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Remove;
   extern const char *Txt_Resources_of_X;
   MYSQL_RES *mysql_res;
   unsigned NumRsc;
   unsigned NumResources;
   bool EditingResourcesOfThisItem;
   char *Title;
   static bool Editing[Prg_NUM_LISTING_TYPES] =
     {
      [Prg_PRINT               ] = false,
      [Prg_VIEW                ] = false,
      [Prg_EDIT_ITEMS          ] = true,
      [Prg_FORM_NEW_END_ITEM   ] = true,
      [Prg_FORM_NEW_CHILD_ITEM ] = true,
      [Prg_FORM_EDIT_ITEM      ] = true,
      [Prg_END_EDIT_ITEM       ] = true,
      [Prg_RECEIVE_ITEM        ] = true,
      [Prg_EDIT_RESOURCES      ] = true,
      [Prg_EDIT_RESOURCE_LINK  ] = true,
      [Prg_CHANGE_RESOURCE_LINK] = true,
      [Prg_END_EDIT_RES        ] = true,
     };
   static bool EditingResources[Prg_NUM_LISTING_TYPES] =
     {
      [Prg_PRINT               ] = false,
      [Prg_VIEW                ] = false,
      [Prg_EDIT_ITEMS          ] = false,
      [Prg_FORM_NEW_END_ITEM   ] = false,
      [Prg_FORM_NEW_CHILD_ITEM ] = false,
      [Prg_FORM_EDIT_ITEM      ] = false,
      [Prg_END_EDIT_ITEM       ] = false,
      [Prg_RECEIVE_ITEM        ] = false,
      [Prg_EDIT_RESOURCES      ] = true,
      [Prg_EDIT_RESOURCE_LINK  ] = true,
      [Prg_CHANGE_RESOURCE_LINK] = true,
      [Prg_END_EDIT_RES        ] = false,
     };

   /***** Trivial check *****/
   if (Item->Hierarchy.ItmCod <= 0)
      return;

   /***** Get list of item resources from database *****/
   NumResources = Prg_DB_GetListResources (&mysql_res,Item->Hierarchy.ItmCod,
                                           EditingResources[ListingType]);

   if (NumResources || Editing[ListingType])
     {
      if (Item->Hierarchy.ItmCod == SelectedItmCod)
	{
	 /***** Begin section *****/
	 HTM_SECTION_Begin (PrgRsc_RESOURCE_SECTION_ID);

	 /***** Show possible alerts *****/
	 if (Gbl.Action.Act == ActReqRemPrgRsc)
	    /* Alert with button to remove resource */
	    Ale_ShowLastAlertAndButton (ActRemPrgRsc,PrgRsc_RESOURCE_SECTION_ID,NULL,
					PrgRsc_PutParRscCod,&SelectedRscCod,
					Btn_REMOVE_BUTTON,Txt_Remove);
	 else
	    Ale_ShowAlerts (PrgRsc_RESOURCE_SECTION_ID);
	}

      /***** Begin box *****/
      EditingResourcesOfThisItem = EditingResources[ListingType] &&
				   (Item->Hierarchy.ItmCod == SelectedItmCod);

      if (Editing[ListingType])
        {
	 if (asprintf (&Title,Txt_Resources_of_X,Item->Title) < 0)
	    Err_NotEnoughMemoryExit ();
	 Box_BoxBegin ("100%",Title,
		       EditingResourcesOfThisItem ? PrgRsc_PutIconsViewResources :
						    PrgRsc_PutIconsEditResources,
		       &Item->Hierarchy.ItmCod,
		       Hlp_COURSE_Program,Box_NOT_CLOSABLE);
         free (Title);
        }
      else
	 Box_BoxBegin ("100%",NULL,
		       NULL,NULL,
		       NULL,Box_NOT_CLOSABLE);

      /***** Table *****/
      HTM_TABLE_BeginWidePadding (2);
	 HTM_TBODY_Begin (NULL);

	    /***** Write all item resources *****/
	    for (NumRsc = 0, The_ResetRowColor1 (1);
		 NumRsc < NumResources;
		 NumRsc++, The_ChangeRowColor1 (1))
	      {
	       /* Get data of this item resource */
	       PrgRsc_GetResourceDataFromRow (mysql_res,Item);

	       /* Show item */
	       if (EditingResourcesOfThisItem)
		  PrgRsc_WriteRowEditResource (NumRsc,NumResources,Item,
					       (ListingType == Prg_EDIT_RESOURCE_LINK &&
						Item->Resource.Hierarchy.RscCod == SelectedRscCod));	// Edit this link?
	       else
		  PrgRsc_WriteRowViewResource (NumRsc,Item);
	      }

	    /***** Form to create a new resource *****/
	    if (EditingResourcesOfThisItem)
	      {
	       Prg_ResetResource (Item);
	       PrgRsc_WriteRowNewResource (NumResources,Item,
					   (ListingType == Prg_EDIT_RESOURCE_LINK &&
					    Item->Resource.Hierarchy.RscCod == SelectedRscCod));	// Edit this link?
	      }

	 /***** End table *****/
	 HTM_TBODY_End ();
      HTM_TABLE_End ();

      /***** End box *****/
      Box_BoxEnd ();

      /***** End section *****/
      if (Item->Hierarchy.ItmCod == SelectedItmCod)
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
      if (*(long *) ItmCod > 0)
	 if (Prg_CheckIfICanEditProgram ())
	    Ico_PutContextualIconToView (ActFrmSeePrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParItmCod,ItmCod);
  }

static void PrgRsc_PutIconsEditResources (void *ItmCod)
  {
   /***** Put icon to create a new item resource *****/
   if (ItmCod)
      if (*(long *) ItmCod > 0)
	 if (Prg_CheckIfICanEditProgram ())
	    Ico_PutContextualIconToEdit (ActFrmEdiPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParItmCod,ItmCod);
  }

/*****************************************************************************/
/****************** Get item resource data using its code ********************/
/*****************************************************************************/

void PrgRsc_GetResourceDataByCod (struct Prg_Item *Item)
  {
   MYSQL_RES *mysql_res;

   if (Item->Resource.Hierarchy.RscCod > 0)
     {
      /***** Get data of item resource *****/
      if (Prg_DB_GetResourceDataByCod (&mysql_res,Item->Resource.Hierarchy.RscCod))
         PrgRsc_GetResourceDataFromRow (mysql_res,Item);
      else
         Prg_ResetItem (Item);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Clear all item resource data *****/
      Prg_ResetItem (Item);
  }

/*****************************************************************************/
/************************* Get item resource data ****************************/
/*****************************************************************************/

static void PrgRsc_GetResourceDataFromRow (MYSQL_RES *mysql_res,
                                           struct Prg_Item *Item)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   ItmCod	row[0]
   RscCod	row[1]
   RscInd	row[2]
   Hidden	row[3]
   Type		row[4]
   Cod		row[5]
   Title	row[6]
   */
   /***** Get code of the program item (row[0]) *****/
   Item->Hierarchy.ItmCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the item resource (row[1], row[2]) *****/
   Item->Resource.Hierarchy.RscCod = Str_ConvertStrCodToLongCod (row[1]);
   Item->Resource.Hierarchy.RscInd = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the program item is hidden (row(3)) *****/
   Item->Resource.Hierarchy.Hidden = (row[3][0] == 'Y');

   /***** Get link type and code (row[4], row[5]) *****/
   Item->Resource.Link.Type = Rsc_GetTypeFromString (row[4]);
   Item->Resource.Link.Cod  = Str_ConvertStrCodToLongCod (row[5]);

   /***** Get the title of the item resource (row[6]) *****/
   Str_Copy (Item->Resource.Title,row[6],sizeof (Item->Resource.Title) - 1);
  }

/*****************************************************************************/
/************************** Show one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         const struct Prg_Item *Item)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumRsc + 1);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT PRG_RSC_%s %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Txt (Item->Resource.Title);
	 HTM_BR ();
	 Rsc_WriteLinkName (&Item->Resource.Link,Frm_PUT_FORM_TO_GO);
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Edit one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowEditResource (unsigned NumRsc,unsigned NumResources,
                                         struct Prg_Item *Item,bool EditLink)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item resource *****/
      HTM_TD_Begin ("class=\"PRG_RSC_COL1 LT %s\"",The_GetColorRows1 (1));
         PrgRsc_PutFormsToRemEditOneResource (Item,NumRsc,NumResources);
      HTM_TD_End ();

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM LT PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumRsc + 1);
      HTM_TD_End ();

      /***** Title and link/clipboard *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT PRG_RSC_%s %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));

         /* Title */
	 Frm_BeginFormAnchor (ActRenPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
	    ParCod_PutPar (ParCod_Rsc,Item->Resource.Hierarchy.RscCod);
	    HTM_INPUT_TEXT ("Title",Rsc_MAX_CHARS_RESOURCE_TITLE,Item->Resource.Title,
			    HTM_SUBMIT_ON_CHANGE,
			    "class=\"PRG_RSC_INPUT INPUT_%s\"",
			    The_GetSuffix ());
	 Frm_EndForm ();

         HTM_BR ();

	 /* Edit link showing clipboard / Show current link */
	 if (EditLink)
	   {
	    /* Show clipboard to change resource link */
            Frm_BeginFormAnchor (ActChgLnkPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
               ParCod_PutPar (ParCod_Rsc,Item->Resource.Hierarchy.RscCod);
               Rsc_ShowClipboardToChangeLink (&Item->Resource.Link);
	    Frm_EndForm ();
           }
	 else
	    /* Show current link */
	    Rsc_WriteLinkName (&Item->Resource.Link,Frm_PUT_FORM_TO_GO);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Edit a new item resource **************************/
/*****************************************************************************/

static void PrgRsc_WriteRowNewResource (unsigned NumResources,
                                        struct Prg_Item *Item,bool EditLink)
  {
   extern const char *Txt_New_resource;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item resource *****/
      HTM_TD_Begin ("class=\"PRG_RSC_COL1 LT %s\"",The_GetColorRows1 (1));
	 PrgRsc_PutFormsToRemEditOneResource (Item,NumResources,NumResources);
      HTM_TD_End ();

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumResources + 1);
      HTM_TD_End ();

      /***** Title and link/clipboard *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT %s\"",The_GetColorRows1 (1));

         /* Title */
	 Frm_BeginFormAnchor (ActNewPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
	    ParCod_PutPar (ParCod_Itm,Item->Hierarchy.ItmCod);
	    HTM_INPUT_TEXT ("Title",Rsc_MAX_CHARS_RESOURCE_TITLE,"",
			    HTM_SUBMIT_ON_CHANGE,
			    "placeholder=\"%s\""
			    " class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_New_resource,
			    The_GetSuffix ());
	 Frm_EndForm ();

	 /* Edit link showing clipboard? */
	 if (EditLink)
	   {
            HTM_BR ();

	    /* Show clipboard to change resource link */
            Frm_BeginFormAnchor (ActChgLnkPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
               ParCod_PutPar (ParCod_Itm,Item->Hierarchy.ItmCod);
               Rsc_ShowClipboardToChangeLink (&Item->Resource.Link);
	    Frm_EndForm ();
	   }

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void PrgRsc_PutFormsToRemEditOneResource (struct Prg_Item *Item,
                                                 unsigned NumRsc,
                                                 unsigned NumResources)
  {
   static Act_Action_t ActionHideUnhide[2] =
     {
      [false] = ActHidPrgRsc,	// Visible ==> action to hide
      [true ] = ActUnhPrgRsc,	// Hidden ==> action to unhide
     };
   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_Visible;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Icon to remove item resource *****/
	 if (NumRsc < NumResources)
	    Ico_PutContextualIconToRemove (ActReqRemPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					   PrgRsc_PutParRscCod,&Item->Resource.Hierarchy.RscCod);
	 else
	    Ico_PutIconRemovalNotAllowed ();

	 /***** Icon to hide/unhide item resource *****/
	 if (NumRsc < NumResources)
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide,PrgRsc_RESOURCE_SECTION_ID,
					       PrgRsc_PutParRscCod,&Item->Resource.Hierarchy.RscCod,
					       Item->Resource.Hierarchy.Hidden);
	 else
	    Ico_PutIconOff ("eye.svg",Ico_GREEN,Txt_Visible);

	 /***** Put icon to edit the item resource *****/
	 if (NumRsc < NumResources)
	    Ico_PutContextualIconToEdit (ActFrmChgLnkPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 PrgRsc_PutParRscCod,&Item->Resource.Hierarchy.RscCod);
	 else
	    Ico_PutContextualIconToEdit (ActFrmChgLnkPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParItmCod,&Item->Hierarchy.ItmCod);

	 /***** Icon to move up the item resource *****/
	 if (NumRsc > 0 && NumRsc < NumResources)
	    Lay_PutContextualLinkOnlyIcon (ActUp_PrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                   PrgRsc_PutParRscCod,&Item->Resource.Hierarchy.RscCod,
	 			           "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the item resource *****/
	 if (NumRsc < NumResources - 1)
	    Lay_PutContextualLinkOnlyIcon (ActDwnPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                   PrgRsc_PutParRscCod,&Item->Resource.Hierarchy.RscCod,
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
/********************** Param used to edit a recource ************************/
/*****************************************************************************/

static void PrgRsc_PutParRscCod (void *RscCod)
  {
   if (RscCod)
      ParCod_PutPar (ParCod_Rsc,*((long *) RscCod));
  }

/*****************************************************************************/
/******************************** Rename resource ****************************/
/*****************************************************************************/

void PrgRsc_CreateResource (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get parameters *****/
   /* Get program item */
   Prg_GetPars (&Item);

   /* Get the new title for the new resource */
   Par_GetParText ("Title",Item.Resource.Title,Rsc_MAX_BYTES_RESOURCE_TITLE);

   /***** Create resource *****/
   Item.Resource.Hierarchy.RscCod = Prg_DB_CreateResource (&Item);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/******************************** Rename resource ****************************/
/*****************************************************************************/

void PrgRsc_RenameResource (void)
  {
   struct Prg_Item Item;
   char NewTitle[Rsc_MAX_BYTES_RESOURCE_TITLE + 1];

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item and resource *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Rename resource *****/
   /* Get the new title for the resource */
   Par_GetParText ("Title",NewTitle,Rsc_MAX_BYTES_RESOURCE_TITLE);

   /* Update database changing old title by new title */
   Prg_DB_UpdateResourceTitle (Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod,NewTitle);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/************ Ask for confirmation of removing an item resource **************/
/*****************************************************************************/

void PrgRsc_ReqRemResource (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_resource_X;
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item and resource *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Create alert to remove the item resource *****/
   Ale_CreateAlert (Ale_QUESTION,PrgRsc_RESOURCE_SECTION_ID,
                    Txt_Do_you_really_want_to_remove_the_resource_X,
                    Item.Resource.Title);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/**************************** Remove an item resource ************************/
/*****************************************************************************/

void PrgRsc_RemoveResource (void)
  {
   extern const char *Txt_Resource_X_removed;
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Remove resource *****/
   Prg_DB_RemoveResource (&Item);

   /***** Create alert to remove the item resource *****/
   Ale_CreateAlert (Ale_SUCCESS,PrgRsc_RESOURCE_SECTION_ID,
                    Txt_Resource_X_removed,Item.Resource.Title);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

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
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item and resource *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Hide/unhide item resource *****/
   Prg_DB_HideOrUnhideResource (Item.Resource.Hierarchy.RscCod,Hide);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

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
   struct Prg_Item Item;
   struct Prg_ResourceHierarchy Rsc2;
   bool Success = false;
   static unsigned (*GetOtherRscInd[PrgRsc_NUM_MOVEMENTS_UP_DOWN])(long ItmCod,unsigned RscInd) =
     {
      [PrgRsc_MOVE_UP  ] = Prg_DB_GetRscIndBefore,
      [PrgRsc_MOVE_DOWN] = Prg_DB_GetRscIndAfter,
     };

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item and resource *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Move up/down resource *****/
   if ((Rsc2.RscInd = GetOtherRscInd[UpDown] (Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscInd)))	// 0 ==> movement not allowed
     {
      /* Get the other resource code */
      Rsc2.RscCod = Prg_DB_GetRscCodFromRscInd (Item.Hierarchy.ItmCod,Rsc2.RscInd);

      /* Exchange subtrees */
      Success = PrgRsc_ExchangeResources (&Item.Resource.Hierarchy,&Rsc2);
     }
   if (!Success)
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCES,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/**** Exchange the order of two consecutive subtrees in a course program *****/
/*****************************************************************************/
// Return true if success

static bool PrgRsc_ExchangeResources (const struct Prg_ResourceHierarchy *Rsc1,
                                      const struct Prg_ResourceHierarchy *Rsc2)
  {
   if (Rsc1->RscInd > 0 &&	// Indexes should be in the range [1, 2,...]
       Rsc2->RscInd > 0)
     {
      /***** Lock table to make the move atomic *****/
      Prg_DB_LockTableResources ();

      /***** Exchange indexes of items *****/
      // This implementation works with non continuous indexes
      /*
      Example:
      Rsc1->Index =  5
      Rsc2->Index = 17
                                Step 1            Step 2            Step 3  (Equivalent to)
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      | RscInd| RscCod|   | RscInd| RscCod|   | RscInd| RscCod|   | RscInd| RscCod| | RscInd| RscCod|
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      |     5 |   218 |   |     5 |   218 |-->|--> 17 |   218 |   |    17 |   218 | |     5 |   240 |
      |    17 |   240 |-->|-->-17 |   240 |   |   -17 |   240 |-->|-->  5 |   240 | |    17 |   218 |
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      */
      /* Step 1: Change second index to negative,
		 necessary to preserve unique index (ItmCod,RscInd) */
      Prg_DB_UpdateRscInd (Rsc2->RscCod,-(int) Rsc2->RscInd);

      /* Step 2: Change first index */
      Prg_DB_UpdateRscInd (Rsc1->RscCod, (int) Rsc2->RscInd);

      /* Step 3: Change second index */
      Prg_DB_UpdateRscInd (Rsc2->RscCod, (int) Rsc1->RscInd);

      /***** Unlock table *****/
      DB_UnlockTables ();

      return true;	// Success
     }

   return false;	// No success
  }

/*****************************************************************************/
/******************** Show clipboard on top of program ***********************/
/*****************************************************************************/

void PrgRsc_ViewResourceClipboard (void)
  {
   /***** View resource clipboard *****/
   PrgRsc_ShowClipboard ();

   /***** Edit course program *****/
   Prg_EditCourseProgram ();
  }

/*****************************************************************************/
/************************* Show resources clipboard **************************/
/*****************************************************************************/

static void PrgRsc_ShowClipboard (void)
  {
   extern const char *Hlp_COURSE_Program_resource_clipboard;
   extern const char *Txt_Resource_clipboard;

   Box_BoxBegin (NULL,Txt_Resource_clipboard,
		 PrgRsc_PutIconsClipboard,NULL,
		 Hlp_COURSE_Program_resource_clipboard,Box_CLOSABLE);
      Rsc_ShowClipboard ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****** Put contextual icons when showing resource clipboard in program ******/
/*****************************************************************************/

static void PrgRsc_PutIconsClipboard (__attribute__((unused)) void *Args)
  {
   /***** Put icon to remove resource clipboard in program *****/
   if (Prg_CheckIfICanEditProgram ())
      if (Rsc_DB_GetNumResourcesInClipboard ())	// Only if there are resources
	 Ico_PutContextualIconToRemove (ActRemRscCli_InPrg,NULL,
					NULL,NULL);
  }

/*****************************************************************************/
/******************* Remove clipboard and show program ***********************/
/*****************************************************************************/

void PrgRsc_RemoveResourceClipboard (void)
  {
   extern const char *Txt_Resource_clipboard_removed;

   /***** Remove resource clipboard *****/
   Rsc_DB_RemoveClipboard ();
   Ale_ShowAlert (Ale_SUCCESS,Txt_Resource_clipboard_removed);

   /***** View resource clipboard again *****/
   PrgRsc_ViewResourceClipboard ();
  }

/*****************************************************************************/
/********** Edit program showing clipboard to change resource link ***********/
/*****************************************************************************/

void PrgRsc_EditProgramWithClipboard (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item and resource *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCE_LINK,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/***************** Show clipboard to change resource link ********************/
/*****************************************************************************/

void PrgRsc_ChangeLink (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get parameters *****/
   /* Get program item and resource */
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /* Get link type and code */
   if (Rsc_GetParLink (&Item.Resource.Link))
     {
      /***** Is it an existing resource? *****/
      if (Item.Resource.Hierarchy.RscCod <= 0)
	{
	 /* No resource selected, so it's a new resource at the end of the item */
	 /* Get the new title for the new resource from link title */
	 Rsc_GetResourceTitleFromLink (&Item.Resource.Link,
	                               Item.Resource.Title);

	 /***** Create resource *****/
	 Item.Resource.Hierarchy.RscCod = Prg_DB_CreateResource (&Item);
	}

      /***** Update resource link *****/
      Prg_DB_UpdateRscLink (&Item);

      /***** Remove link from clipboard *****/
      Rsc_DB_RemoveLinkFromClipboard (&Item.Resource.Link);
     }

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCE_LINK,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }
