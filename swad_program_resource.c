// swad_program_resource.c: course program (resources)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_assignment_resource.h"
#include "swad_attendance.h"
#include "swad_browser_resource.h"
#include "swad_call_for_exam_resource.h"
#include "swad_error.h"
#include "swad_exam_resource.h"
#include "swad_form.h"
#include "swad_game_resource.h"
#include "swad_global.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_program_resource.h"
#include "swad_survey_resource.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

const char *Prg_ResourceTypesIcons[PrgRsc_NUM_TYPES] =
  {
   [PrgRsc_NONE            ] = "link-slash.svg",
   // gui TEACHING_GUIDE	// Link to teaching guide
   // bib BIBLIOGRAPHY		// Link to bibliography
   // faq FAQ			// Link to FAQ
   // lnk LINKS			// Link to links
   // tmt TIMETABLE		// Link to timetable
   [PrgRsc_ASSIGNMENT      ] = "edit.svg",
   // prj PROJECT		// A project is only for some students
   [PrgRsc_CALL_FOR_EXAM   ] = "bullhorn.svg",
   // tst TEST			// User selects tags, teacher should select
   [PrgRsc_EXAM            ] = "file-signature.svg",
   [PrgRsc_GAME            ] = "gamepad.svg",
   [PrgRsc_SURVEY          ] = "poll.svg",
   [PrgRsc_DOCUMENT        ] = "up-right-from-square.svg",	// for links because each file type has its own icon
   [PrgRsc_MARKS           ] = "list-alt.svg",
   // grp GROUPS		// ??? User select groups
   [PrgRsc_ATTENDANCE_EVENT] = "calendar-check.svg",
   [PrgRsc_FORUM_THREAD    ] = "comments.svg",
  };

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

static const char *PrgRsc_RESOURCE_SECTION_ID = "rsc_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrgRsc_PutIconsViewResources (void *ItmCod);
static void PrgRsc_PutIconsEditResources (void *ItmCod);

static void PrgRsc_GetDataOfResource (struct Prg_Item *Item,
                                      MYSQL_RES **mysql_res);
static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         const struct Prg_Item *Item);
static void PrgRsc_WriteRowEditResource (unsigned NumRsc,unsigned NumResources,
                                         struct Prg_Item *Item,bool EditLink);
static void PrgRsc_WriteRowNewResource (unsigned NumResources,
                                        struct Prg_Item *Item,bool EditLink);
static void PrgRsc_PutFormsToRemEditOneResource (struct Prg_Item *Item,
                                                 unsigned NumRsc,
                                                 unsigned NumResources);
static void PrgRsc_HideOrUnhideResource (bool Hide);

static void PrgRsc_MoveUpDownResource (PrgRsc_MoveUpDown_t UpDown);
static bool PrgRsc_ExchangeResources (const struct Prg_ResourceHierarchy *Rsc1,
                                      const struct Prg_ResourceHierarchy *Rsc2);

static void PrgRsc_ShowClipboard (struct Prg_Item *Item);
static void PrgRsc_WriteRowClipboard (bool SubmitOnClick,const struct Prg_Link *Link);
static void PrgRsc_WriteLinkName (const struct Prg_Link *Link,bool PutFormToGo,
                                  const char *Icon,const char *IconTitle);
static void PrgRsc_WriteEmptyLinkInCrsProgram (__attribute__((unused)) long Cod,
                                               __attribute__((unused)) bool PutFormToGo,
                                               const char *Icon,const char *IconTitle);
static void PrgRsc_GetDataOfLinkFromClipboard (struct Prg_Link *Link,
                                               MYSQL_RES **mysql_res);

/*****************************************************************************/
/****************************** View resources *******************************/
/*****************************************************************************/

void PrgRsc_ViewResourcesAfterEdit (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetParams (&Item);

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
   Prg_GetParams (&Item);

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
   extern const char *Txt_Remove_resource;
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
					Prg_PutParamRscCod,&SelectedRscCod,
					Btn_REMOVE_BUTTON,Txt_Remove_resource);
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
	       PrgRsc_GetDataOfResource (Item,&mysql_res);

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
					 Prg_PutParamItmCod,ItmCod);
  }

static void PrgRsc_PutIconsEditResources (void *ItmCod)
  {
   /***** Put icon to create a new item resource *****/
   if (ItmCod)
      if (*(long *) ItmCod > 0)
	 if (Prg_CheckIfICanEditProgram ())
	    Ico_PutContextualIconToEdit (ActFrmEdiPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParamItmCod,ItmCod);
  }

/*****************************************************************************/
/****************** Get item resource data using its code ********************/
/*****************************************************************************/

void PrgRsc_GetDataOfResourceByCod (struct Prg_Item *Item)
  {
   MYSQL_RES *mysql_res;

   if (Item->Resource.Hierarchy.RscCod > 0)
     {
      /***** Get data of item resource *****/
      if (Prg_DB_GetDataOfResourceByCod (&mysql_res,Item->Resource.Hierarchy.RscCod))
         PrgRsc_GetDataOfResource (Item,&mysql_res);
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

static void PrgRsc_GetDataOfResource (struct Prg_Item *Item,
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
   Type		row[4]
   Cod		row[5]
   Title	row[6]
   */
   /* Get code of the program item (row[0]) */
   Item->Hierarchy.ItmCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Get code and index of the item resource (row[1], row[2]) */
   Item->Resource.Hierarchy.RscCod = Str_ConvertStrCodToLongCod (row[1]);
   Item->Resource.Hierarchy.RscInd = Str_ConvertStrToUnsigned (row[2]);

   /* Get whether the program item is hidden (row(3)) */
   Item->Resource.Hierarchy.Hidden = (row[3][0] == 'Y');

   /* Get link type and code (row[4], row[5]) */
   Item->Resource.Link.Type = PrgRsc_GetTypeFromString (row[4]);
   Item->Resource.Link.Cod  = Str_ConvertStrCodToLongCod (row[5]);

   /* Get the title of the item resource (row[6]) */
   Str_Copy (Item->Resource.Title,row[6],sizeof (Item->Resource.Title) - 1);
  }

/*****************************************************************************/
/************************** Show one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         const struct Prg_Item *Item)
  {
   extern const char *Txt_RESOURCE_TYPES[PrgRsc_NUM_TYPES];

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
	 PrgRsc_WriteLinkName (&Item->Resource.Link,
			       true,	// Put form
			       Prg_ResourceTypesIcons[Item->Resource.Link.Type],
			       Txt_RESOURCE_TYPES[Item->Resource.Link.Type]);
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
   extern const char *Txt_RESOURCE_TYPES[PrgRsc_NUM_TYPES];

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
	    Prg_PutParamRscCod (&Item->Resource.Hierarchy.RscCod);
	    HTM_INPUT_TEXT ("Title",PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE,Item->Resource.Title,
			    HTM_SUBMIT_ON_CHANGE,
			    "class=\"PRG_RSC_INPUT INPUT_%s\"",
			    The_GetSuffix ());
	 Frm_EndForm ();

         HTM_BR ();

	 /* Edit link showing clipboard / Show current link */
	 if (EditLink)
	    /* Show clipboard to change resource link */
	    PrgRsc_ShowClipboard (Item);
	 else
	    /* Show current link */
	    PrgRsc_WriteLinkName (&Item->Resource.Link,
	                          true,	// Put form
				  Prg_ResourceTypesIcons[Item->Resource.Link.Type],
	                          Txt_RESOURCE_TYPES[Item->Resource.Link.Type]);

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
	    Prg_PutParamItmCod (&Item->Hierarchy.ItmCod);
	    HTM_INPUT_TEXT ("Title",PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE,"",
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
	    PrgRsc_ShowClipboard (Item);
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
					   Prg_PutParamRscCod,&Item->Resource.Hierarchy.RscCod);
	 else
	    Ico_PutIconRemovalNotAllowed ();

	 /***** Icon to hide/unhide item resource *****/
	 if (NumRsc < NumResources)
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide,PrgRsc_RESOURCE_SECTION_ID,
					       Prg_PutParamRscCod,&Item->Resource.Hierarchy.RscCod,
					       Item->Resource.Hierarchy.Hidden);
	 else
	    Ico_PutIconOff ("eye.svg",Ico_GREEN,Txt_Visible);

	 /***** Put icon to edit the item resource *****/
	 if (NumRsc < NumResources)
	    Ico_PutContextualIconToEdit (ActSeeCliPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParamRscCod,&Item->Resource.Hierarchy.RscCod);
	 else
	    Ico_PutContextualIconToEdit (ActSeeCliPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParamItmCod,&Item->Hierarchy.ItmCod);

	 /***** Icon to move up the item resource *****/
	 if (NumRsc > 0 && NumRsc < NumResources)
	    Lay_PutContextualLinkOnlyIcon (ActUp_PrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                   Prg_PutParamRscCod,&Item->Resource.Hierarchy.RscCod,
	 			           "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the item resource *****/
	 if (NumRsc < NumResources - 1)
	    Lay_PutContextualLinkOnlyIcon (ActDwnPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                   Prg_PutParamRscCod,&Item->Resource.Hierarchy.RscCod,
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
/**************** Write parameter with code of program item ******************/
/*****************************************************************************/

void PrgRsc_PutParamRscCod (long RscCod)
  {
   Par_PutHiddenParamLong (NULL,"RscCod",RscCod);
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
   Prg_GetParams (&Item);

   /* Get the new title for the new resource */
   Par_GetParToText ("Title",Item.Resource.Title,PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE);

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
   char NewTitle[PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE + 1];

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item and resource *****/
   Prg_GetParams (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /***** Rename resource *****/
   /* Get the new title for the resource */
   Par_GetParToText ("Title",NewTitle,PrgRsc_MAX_BYTES_PROGRAM_RESOURCE_TITLE);

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
   Prg_GetParams (&Item);
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
   Prg_GetParams (&Item);
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
   Prg_GetParams (&Item);
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
   Prg_GetParams (&Item);
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
/***************** Show clipboard to change resource link ********************/
/*****************************************************************************/

void PrgRsc_ShowClipboardToChgLink (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item and resource *****/
   Prg_GetParams (&Item);
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

static void PrgRsc_ShowClipboard (struct Prg_Item *Item)
  {
   MYSQL_RES *mysql_res;
   unsigned NumLink;
   unsigned NumLinks;
   struct Prg_Link Link;
   static const struct Prg_Link EmptyLink =
     {
      .Type = PrgRsc_NONE,
      .Cod  = -1L,
     };

   /***** Begin form *****/
   Frm_BeginFormAnchor (ActChgLnkPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
      if (Item->Resource.Hierarchy.RscCod > 0)
         Prg_PutParamRscCod (&Item->Resource.Hierarchy.RscCod);
      else
	 /* No resource selected, so it's a new resource at the end of the item */
         Prg_PutParamItmCod (&Item->Hierarchy.ItmCod);

      /***** Begin list *****/
      HTM_UL_Begin ("class=\"PRG_CLIPBOARD\"");

	 /***** Current link (empty or not) *****/
	 PrgRsc_WriteRowClipboard (false,&Item->Resource.Link);

         /***** Row with empty link to remove the current link *****/
	 if (Item->Resource.Link.Type != PrgRsc_NONE)
	    PrgRsc_WriteRowClipboard (true,&EmptyLink);

	 /***** Get links in clipboard from database and write them *****/
	 NumLinks = Prg_DB_GetClipboard (&mysql_res);
	 for (NumLink  = 1;
	      NumLink <= NumLinks;
	      NumLink++)
	   {
	    PrgRsc_GetDataOfLinkFromClipboard (&Link,&mysql_res);
	    PrgRsc_WriteRowClipboard (true,&Link);
	   }
	 DB_FreeMySQLResult (&mysql_res);

      /***** End list *****/
      HTM_UL_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************ Show one link from clipboard ***********************/
/*****************************************************************************/

static void PrgRsc_WriteRowClipboard (bool SubmitOnClick,const struct Prg_Link *Link)
  {
   extern const char *Prg_ResourceTypesDB[PrgRsc_NUM_TYPES];
   extern const char *Txt_RESOURCE_TYPES[PrgRsc_NUM_TYPES];

   HTM_LI_Begin ("class=\"PRG_RSC_%s\"",The_GetSuffix ());
      HTM_LABEL_Begin (NULL);

         /***** Radio selector *****/
	 HTM_INPUT_RADIO ("Link",SubmitOnClick,
			  "value=\"%s_%ld\"%s",
			  Prg_ResourceTypesDB[Link->Type],Link->Cod,
			  SubmitOnClick ? "" :
					  " checked=\"checked\"");

	 /***** Name *****/
         PrgRsc_WriteLinkName (Link,
                               false,	// Don't put form
			       Prg_ResourceTypesIcons[Link->Type],
	                       Txt_RESOURCE_TYPES[Link->Type]);

      HTM_LABEL_End ();
   HTM_LI_End ();
  }

/*****************************************************************************/
/************* Write link name (filename, assignment title...) ***************/
/*****************************************************************************/

static void PrgRsc_WriteLinkName (const struct Prg_Link *Link,bool PutFormToGo,
                                  const char *Icon,const char *IconTitle)
  {
   static void (*WriteLinkName[PrgRsc_NUM_TYPES]) (long Cod,bool PutFormToGo,
						   const char *Icon,
						   const char *IconTitle) =
     {
      [PrgRsc_NONE            ] = PrgRsc_WriteEmptyLinkInCrsProgram,
      [PrgRsc_ASSIGNMENT      ] = AsgRsc_WriteAssignmentInCrsProgram,
      [PrgRsc_CALL_FOR_EXAM   ] = CfeRsc_WriteCallForExamInCrsProgram,
      [PrgRsc_EXAM            ] = ExaRsc_WriteExamInCrsProgram,
      [PrgRsc_GAME            ] = GamRsc_WriteGameInCrsProgram,
      [PrgRsc_SURVEY          ] = SvyRsc_WriteSurveyInCrsProgram,
      [PrgRsc_DOCUMENT        ] = BrwRsc_WriteDocFileNameInCrsProgram,
      [PrgRsc_MARKS           ] = BrwRsc_WriteMrkFileNameInCrsProgram,
      [PrgRsc_ATTENDANCE_EVENT] = AttRsc_WriteAttEventInCrsProgram,
      [PrgRsc_FORUM_THREAD    ] = ForRsc_WriteThreadInCrsProgram,
     };

   /***** Trivial check: code should be > 0 *****/
   if (Link->Type != PrgRsc_NONE &&
       Link->Cod <= 0)
      return;

   /***** Write link name *****/
   if (WriteLinkName[Link->Type])
      WriteLinkName[Link->Type] (Link->Cod,PutFormToGo,Icon,IconTitle);
   else
      Ale_ShowAlert (Ale_ERROR,"Not implemented!");
  }

/*****************************************************************************/
/********************** Write survey in course program ***********************/
/*****************************************************************************/

static void PrgRsc_WriteEmptyLinkInCrsProgram (__attribute__((unused)) long Cod,
                                               __attribute__((unused)) bool PutFormToGo,
                                               const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_RESOURCE_TYPES[PrgRsc_NUM_TYPES];

   /***** Icon depending on type ******/
   Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write Name of the course and date of exam *****/
   HTM_Txt (Txt_RESOURCE_TYPES[PrgRsc_NONE]);
  }

/*****************************************************************************/
/* Get the title for a new resource from link title */
/*****************************************************************************/

static void PrgRsc_GetResourceTitleFromLink (struct Prg_Item *Item)
  {
   static void (*GetTitle[PrgRsc_NUM_TYPES]) (long Cod,char *Title,size_t TitleSize) =
     {
      [PrgRsc_NONE            ] = NULL,
      [PrgRsc_ASSIGNMENT      ] = AsgRsc_GetTitleFromAsgCod,
      [PrgRsc_CALL_FOR_EXAM   ] = CfeRsc_GetTitleFromExaCod,
      [PrgRsc_EXAM            ] = ExaRsc_GetTitleFromExaCod,
      [PrgRsc_GAME            ] = GamRsc_GetTitleFromGamCod,
      [PrgRsc_SURVEY          ] = SvyRsc_GetTitleFromSvyCod,
      [PrgRsc_DOCUMENT        ] = BrwRsc_GetFileNameFromFilCod,
      [PrgRsc_MARKS           ] = BrwRsc_GetFileNameFromFilCod,
      [PrgRsc_ATTENDANCE_EVENT] = AttRsc_GetTitleFromAttCod,
      [PrgRsc_FORUM_THREAD    ] = ForRsc_GetTitleFromThrCod,
     };

   /***** Reset title *****/
   Item->Resource.Title[0] = '\0';

   /***** Trivial check: code should be > 0 *****/
   if (Item->Resource.Link.Cod <= 0)
      return;

   /***** Get title *****/
   if (GetTitle[Item->Resource.Link.Type])
      GetTitle[Item->Resource.Link.Type] (Item->Resource.Link.Cod,
					  Item->Resource.Title,
					  sizeof (Item->Resource.Title) - 1);
   else
      Ale_ShowAlert (Ale_ERROR,"Not implemented!");
  }

/*****************************************************************************/
/***************** Show clipboard to change resource link ********************/
/*****************************************************************************/

void PrgRsc_ChangeLink (void)
  {
   struct Prg_Item Item;
   char TypeCod[3 + 1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char TypeStr[3 + 1];
   long Cod;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get parameters *****/
   /* Get program item and resource */
   Prg_GetParams (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongResourceExit ();

   /* Get link type and code */
   Par_GetParToText ("Link",TypeCod,sizeof (TypeCod) - 1);
   if (sscanf (TypeCod,"%3s_%ld",TypeStr,&Cod) == 2)
     {
      Item.Resource.Link.Type = PrgRsc_GetTypeFromString (TypeStr);
      Item.Resource.Link.Cod  = Cod;

      /***** Is it an existing resource? *****/
      if (Item.Resource.Hierarchy.RscCod <= 0)
	{
	 /* No resource selected, so it's a new resource at the end of the item */
	 /* Get the new title for the new resource from link title */
	 PrgRsc_GetResourceTitleFromLink (&Item);

	 /***** Create resource *****/
	 Item.Resource.Hierarchy.RscCod = Prg_DB_CreateResource (&Item);
	}

      /***** Update resource link *****/
      Prg_DB_UpdateRscLink (&Item);

      /***** Remove link from clipboard *****/
      Prg_DB_RemoveLinkFromClipboard (&Item.Resource.Link);
     }

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_EDIT_RESOURCE_LINK,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/********************** Get resource data from clipboard *********************/
/*****************************************************************************/

static void PrgRsc_GetDataOfLinkFromClipboard (struct Prg_Link *Link,
                                               MYSQL_RES **mysql_res)
  {
   MYSQL_ROW row;

   /***** Get data of item resource from database *****/
   /* Get row */
   row = mysql_fetch_row (*mysql_res);
   /*
   Type	row[0]
   Cod	row[1]
   */
   /* Get type (row[0]) */
   Link->Type = PrgRsc_GetTypeFromString (row[0]);

   /* Get code (row[1]) */
   Link->Cod = Str_ConvertStrCodToLongCod (row[1]);
  }

/*****************************************************************************/
/********************** Convert from string to type **************************/
/*****************************************************************************/

PrgRsc_Type_t PrgRsc_GetTypeFromString (const char *Str)
  {
   extern const char *Prg_ResourceTypesDB[PrgRsc_NUM_TYPES];
   PrgRsc_Type_t Type;

   /***** Compare string with all string types *****/
   for (Type  = (PrgRsc_Type_t) 0;
	Type <= (PrgRsc_Type_t) (PrgRsc_NUM_TYPES - 1);
	Type++)
      if (!strcmp (Prg_ResourceTypesDB[Type],Str))
	 return Type;

   return PrgRsc_NONE;
  }
