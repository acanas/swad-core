// swad_program.c: course program

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

// #define _GNU_SOURCE 		// For asprintf
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For calloc
// #include <string.h>		// For string functions

// #include "swad_autolink.h"
// #include "swad_box.h"
// #include "swad_database.h"
#include "swad_error.h"
// #include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_pagination.h"
// #include "swad_parameter.h"
// #include "swad_photo.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_program_resource.h"
// #include "swad_role.h"
// #include "swad_setting.h"
// #include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/
/*
#define Prg_NUM_TYPES_FORMS 3
typedef enum
  {
   Prg_DONT_PUT_FORM_ITEM,
   Prg_PUT_FORM_CREATE_ITEM,
   Prg_PUT_FORM_CHANGE_ITEM,
  } Prg_CreateOrChangeItem_t;

#define Prg_NUM_MOVEMENTS_UP_DOWN 2
typedef enum
  {
   Prg_MOVE_UP,
   Prg_MOVE_DOWN,
  } Prg_MoveUpDown_t;

struct Level
  {
   unsigned Number;	// Numbers for each level from 1 to maximum level
   bool Hidden;		// If each level from 1 to maximum level is hidden
  };
*/
/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/
/*
static struct
  {
   struct
     {
      bool IsRead;		// Is the list already read from database...
			        // ...or it needs to be read?
      unsigned NumItems;	// Number of items
      struct Prg_ItemHierarchy *Items;	// List of items
     } List;
   unsigned MaxLevel;		// Maximum level of items
   struct Level *Levels;	// Numbers and hidden for each level from 1 to maximum level
  } Prg_Gbl =
  {
   .List =
     {
      .IsRead     = false,
      .NumItems   = 0,
      .Items      = NULL,
     },
   .MaxLevel      = 0,
   .Levels        = NULL
  };
*/

static long PrgSrc_RscCodToBeRemoved = -1L;

static const char *PrgRsc_RESOURCE_SECTION_ID = "rsc_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrgRsc_PutIconsListResources (__attribute__((unused)) void *Args);
static void PrgSrc_PutIconToCreateNewResource (long ItmCod);
static void PrgRsc_PutButtonToCreateNewResource (long ItmCod);

static void PrgRsc_GetDataOfResourceByCod (struct PrgRsc_Resource *Resource);
static void PrgRsc_GetDataOfResource (struct PrgRsc_Resource *Resource,
                                      MYSQL_RES **mysql_res);
static void PrgRsc_ResetResource (struct PrgRsc_Resource *Resource);
static void PrgRsc_WriteRowShowResource (unsigned NumRsc,
                                         struct PrgRsc_Resource *Resource);
static void PrgRsc_WriteRowEditResource (unsigned NumRsc,
                                         struct PrgRsc_Resource *Resource);
static void PrgRsc_PutFormsToRemEditOneResource (struct PrgRsc_Resource *Resource);
static void PrgRsc_PutParams (void *RscCod);
static void PrgRsc_PutParamRscCod (long RscCod);
static long PrgRsc_GetParamRscCod (void);

/*****************************************************************************/
/****************************** Show resources *******************************/
/*****************************************************************************/

void PrgRsc_ShowResources (long ItmCod)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRsc;
   unsigned NumResources;
   struct PrgRsc_Resource Resource;

   /***** Trivial check *****/
   if (ItmCod <= 0)
      return;

   /***** Get list of item resources from database *****/
   if ((NumResources = Prg_DB_GetListResources (&mysql_res,ItmCod))) // Resources found...
     {
      /***** Begin box *****/
      Box_BoxBegin ("100%",NULL,
		    NULL,NULL,
		    NULL,Box_NOT_CLOSABLE);

	 /***** Table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);
	    HTM_TBODY_Begin (NULL);		// 1st tbody start

	       /***** Write all item resources *****/
	       for (NumRsc  = 1;
		    NumRsc <= NumResources;
		    NumRsc++)
		 {
		  /* Get data of this item resource */
		  PrgRsc_GetDataOfResource (&Resource,&mysql_res);

		  /* Show item */
		  PrgRsc_WriteRowShowResource (NumRsc,&Resource);
		 }

	    /***** End table *****/
	    HTM_TBODY_End ();					// 3rd tbody end
	 HTM_TABLE_End ();

      /***** End box *****/
      Box_BoxEnd ();
     }
  }

/*****************************************************************************/
/****************************** Edit resources *******************************/
/*****************************************************************************/

void PrgRsc_EditResources (long ItmCod)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Remove_resource;
   extern const char *Txt_Resources;
   MYSQL_RES *mysql_res;
   unsigned NumRsc;
   unsigned NumResources;
   struct PrgRsc_Resource Resource;

   /***** Trivial check *****/
   if (ItmCod <= 0)
      return;

   /***** Get list of item resources from database *****/
   NumResources = Prg_DB_GetListResources (&mysql_res,ItmCod); // Resources found...

   /***** Begin section *****/
   HTM_SECTION_Begin (PrgRsc_RESOURCE_SECTION_ID);

      /***** Show possible alerts *****/
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
		    PrgRsc_PutIconsListResources,&ItmCod,
		    Hlp_COURSE_Program,Box_NOT_CLOSABLE);

	 /***** Table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);
	    HTM_TBODY_Begin (NULL);

	       /***** Write all item resources *****/
	       for (NumRsc  = 1;
		    NumRsc <= NumResources;
		    NumRsc++)
		 {
		  /* Get data of this item resource */
		  PrgRsc_GetDataOfResource (&Resource,&mysql_res);

		  /* Show item */
		  PrgRsc_WriteRowEditResource (NumRsc,&Resource);

		  The_ChangeRowColor ();
		 }

	       /***** Create item at the end? *****/
	       /*
	       if (ItmCodBeforeForm <= 0 && CreateOrChangeItem == Prg_PUT_FORM_CREATE_ITEM)
		  Prg_WriteRowWithItemForm (Prg_PUT_FORM_CREATE_ITEM,-1L,1);
	       */

	    /***** End table *****/
	    HTM_TBODY_End ();
	 HTM_TABLE_End ();

	 /***** Button to create a new program item *****/
	 PrgRsc_PutButtonToCreateNewResource (ItmCod);

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/************** Put contextual icons in list of item resources ***************/
/*****************************************************************************/

static void PrgRsc_PutIconsListResources (void *ItmCod)
  {
   /***** Put icon to create a new item resource *****/
   if (ItmCod)
      if (*((long *) ItmCod) > 0)
	 if (Prg_CheckIfICanCreateItems ())
	    PrgSrc_PutIconToCreateNewResource (*((long *) ItmCod));
  }

/*****************************************************************************/
/****************** Put icon to create a new program item ********************/
/*****************************************************************************/

static void PrgSrc_PutIconToCreateNewResource (long ItmCod)
  {
   Ico_PutContextualIconToAdd (ActFrmNewPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
                               Prg_PutParams,&ItmCod);
  }

/*****************************************************************************/
/***************** Put button to create a new item resource ******************/
/*****************************************************************************/

static void PrgRsc_PutButtonToCreateNewResource (long ItmCod)
  {
   extern const char *Txt_New_resource;

   Frm_BeginFormAnchor (ActFrmNewPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
      Prg_PutParams (&ItmCod);
      Btn_PutConfirmButton (Txt_New_resource);
   Frm_EndForm ();
  }

/*****************************************************************************/
/*** Put a form to create/edit program resource and show current resources ***/
/*****************************************************************************/

void PrgRsc_RequestCreateResource (void)
  {
   // unsigned NumItem;
   // long ItmCodBeforeForm;
   // unsigned FormLevel;
   // struct Prg_Item Item;

   /***** Get list of program items *****/
   // Prg_GetListItems ();

   /***** Get data of the item from database *****/
   // Item.Hierarchy.ItmCod = Prg_GetParamItmCod ();
   // Prg_GetDataOfItemByCod (&Item);
   // if (Item.Hierarchy.ItmCod <= 0)
      // Err_WrongItemExit ();

   /***** Show current program items, if any *****/
   // Prg_SetItemRangeEmpty (&ToHighlight);
   // Prg_ShowAllItems (Prg_PUT_FORM_CHANGE_ITEM,
   //                   &ToHighlight,ParentItmCod,ItmCodBeforeForm,FormLevel);

   /***** Free list of program items *****/
   // Prg_FreeListItems ();
  }

/*****************************************************************************/
/****************** Get item resource data using its code ********************/
/*****************************************************************************/

static void PrgRsc_GetDataOfResourceByCod (struct PrgRsc_Resource *Resource)
  {
   MYSQL_RES *mysql_res;

   if (Resource->RscCod > 0)
     {
      /***** Get data of item resource *****/
      if (Prg_DB_GetDataOfResourceByCod (&mysql_res,Resource->RscCod))
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
   Hidden	row[2]
   Title	row[3]
   */
   /* Get code of the program item (row[0]) */
   Resource->ItmCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Get code of the item resource (row[1]) */
   Resource->RscCod = Str_ConvertStrCodToLongCod (row[1]);

   /* Get whether the program item is hidden (row(2)) */
   Resource->Hidden = (row[2][0] == 'Y');

   /* Get the title of the item resource (row[3]) */
   Str_Copy (Resource->Title,row[3],sizeof (Resource->Title) - 1);
  }

/*****************************************************************************/
/************************ Clear all item resource data ***********************/
/*****************************************************************************/

static void PrgRsc_ResetResource (struct PrgRsc_Resource *Resource)
  {
   Resource->ItmCod = -1L;
   Resource->RscCod = -1L;
   Resource->Hidden = false;
   Resource->Title[0] = '\0';
  }

/*****************************************************************************/
/************************** Show one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowShowResource (unsigned NumRsc,
                                         struct PrgRsc_Resource *Resource)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM RT %s\"",The_GetColorRows ());
	 HTM_Unsigned (NumRsc);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN %s\"",The_GetColorRows ());
	 HTM_Txt (Resource->Title);
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Edit one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowEditResource (unsigned NumRsc,
                                         struct PrgRsc_Resource *Resource)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item resource *****/
      HTM_TD_Begin ("class=\"PRG_COL1 LT %s\"",The_GetColorRows ());
	 PrgRsc_PutFormsToRemEditOneResource (Resource);
      HTM_TD_End ();

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM RT %s\"",The_GetColorRows ());
	 HTM_Unsigned (NumRsc);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN %s\"",The_GetColorRows ());
	 HTM_Txt (Resource->Title);
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void PrgRsc_PutFormsToRemEditOneResource (struct PrgRsc_Resource *Resource)
  {
   extern const char *Txt_Movement_not_allowed;
   // char StrItemIndex[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Initialize item index string *****/
   // snprintf (StrItemIndex,sizeof (StrItemIndex),"%u",Item->Hierarchy.Index);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put form to remove program item *****/
	 Ico_PutContextualIconToRemove (ActReqRemPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                PrgRsc_PutParams,&Resource->RscCod);

	 /***** Put form to hide/show program item *****/
	 //if (Item->Hierarchy.Hidden)
	 //   Ico_PutContextualIconToUnhide (ActShoPrgItm,"prg_highlighted",
	 //                                  Prg_PutParams,&Item->Hierarchy.ItmCod);
	 //else
	 //   Ico_PutContextualIconToHide (ActHidPrgItm,"prg_highlighted",
	 //                                Prg_PutParams,&Item->Hierarchy.ItmCod);

	 /***** Put form to edit program item *****/
	 // Ico_PutContextualIconToEdit (ActFrmChgPrgItm,"item_form",
	 //                             Prg_PutParams,&Item->Hierarchy.ItmCod);

	 /***** Put form to add a new child item inside this item *****/
	 // Ico_PutContextualIconToAdd (ActFrmNewPrgItm,"item_form",
	 //                            Prg_PutParams,&Item->Hierarchy.ItmCod);

	 HTM_BR ();

	 /***** Put icon to move up the item *****/
	 // if (Prg_CheckIfMoveUpIsAllowed (NumItem))
	 //   Lay_PutContextualLinkOnlyIcon (ActUp_PrgItm,"prg_highlighted",
	 //                                  Prg_PutParams,&Item->Hierarchy.ItmCod,
	 //				   "arrow-up.svg",Ico_BLACK);
	 // else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the item *****/
	 // if (Prg_CheckIfMoveDownIsAllowed (NumItem))
	 //   Lay_PutContextualLinkOnlyIcon (ActDwnPrgItm,"prg_highlighted",
	 //                                  Prg_PutParams,&Item->Hierarchy.ItmCod,
	 //				   "arrow-down.svg",Ico_BLACK);
	 //else
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
/************ Ask for confirmation of removing an item resource **************/
/*****************************************************************************/

void PrgRsc_ReqRemResource (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_resource_X;
   struct PrgRsc_Resource Resource;
   long ItmCodBeforeForm;
   unsigned FormLevel;
   struct Prg_ItemRange ToHighlight;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Resource.RscCod = PrgRsc_GetParamRscCod ();
   PrgRsc_GetDataOfResourceByCod (&Resource);
   if (Resource.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Create alert to remove the item resource *****/
   Ale_CreateAlert (Ale_QUESTION,PrgRsc_RESOURCE_SECTION_ID,
                    Txt_Do_you_really_want_to_remove_the_resource_X,
                    Resource.Title);
   PrgSrc_RscCodToBeRemoved = Resource.RscCod;

   /***** Get the code of the program item *****/
   ItmCodBeforeForm = Resource.ItmCod;
   FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (Resource.ItmCod));

   /***** Show current program items, if any *****/
   Prg_SetItemRangeEmpty (&ToHighlight);
   Prg_ShowAllItems (Prg_PUT_FORM_CHANGE_ITEM,
		     &ToHighlight,-1L,ItmCodBeforeForm,FormLevel);

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
   long ItmCodBeforeForm;
   unsigned FormLevel;
   struct Prg_ItemRange ToHighlight;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get data of the item resource from database *****/
   Resource.RscCod = PrgRsc_GetParamRscCod ();
   PrgRsc_GetDataOfResourceByCod (&Resource);
   if (Resource.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Remove resource *****/
   Prg_DB_RemoveResource (&Resource);

   /***** Create alert to remove the item resource *****/
   Ale_CreateAlert (Ale_SUCCESS,PrgRsc_RESOURCE_SECTION_ID,
                    Txt_Resource_X_removed,Resource.Title);

   /***** Get the code of the program item *****/
   ItmCodBeforeForm = Resource.ItmCod;
   FormLevel = Prg_GetLevelFromNumItem (Prg_GetNumItemFromItmCod (Resource.ItmCod));

   /***** Show current program items, if any *****/
   Prg_SetItemRangeEmpty (&ToHighlight);
   Prg_ShowAllItems (Prg_PUT_FORM_CHANGE_ITEM,
		     &ToHighlight,-1L,ItmCodBeforeForm,FormLevel);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }
