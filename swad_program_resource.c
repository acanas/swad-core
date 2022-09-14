// swad_program_resource.c: course program (resources)

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

#include <string.h>		// For string functions

#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_program_resource.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

const char *Prg_ResourceTypesLogos[PrgRsc_NUM_TYPES] =
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
   [PrgRsc_DOCUMENT        ] = "file.svg",
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

static void PrgRsc_PutIconsViewResources (void *ItmRscCodes);
static void PrgRsc_PutIconsEditResources (void *ItmRscCodes);

static void PrgRsc_GetDataOfResource (struct Prg_Item *Item,
                                      MYSQL_RES **mysql_res);
static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         const struct Prg_Item *Item);
static void PrgRsc_WriteRowEditResource (Prg_ListingType_t ListingType,
                                         unsigned NumRsc,unsigned NumResources,
                                         const struct Prg_Item *Item,
                                         struct Prg_ItmRscCodes *SelectedItmRscCodes);
static void PrgRsc_WriteRowNewResource (Prg_ListingType_t ListingType,
                                        unsigned NumResources,
                                        struct Prg_ItmRscCodes *SelectedItmRscCodes);
static void PrgRsc_PutFormsToRemEditOneResource (unsigned NumRsc,
                                                 unsigned NumResources,
                                                 struct Prg_ItmRscCodes *ItmRscCodes,
                                                 bool Hidden);
static void PrgRsc_HideOrUnhideResource (bool Hide);

static void PrgRsc_MoveUpDownResource (PrgRsc_MoveUpDown_t UpDown);
static bool PrgRsc_ExchangeResources (const struct Prg_ResourceHierarchy *Rsc1,
                                      const struct Prg_ResourceHierarchy *Rsc2);

static void PrgRsc_ShowClipboard (const struct Prg_Item *Item);
static void PrgRsc_WriteRowClipboard (unsigned NumLink,
                                      const struct PrgRsc_Link *Link);
static void PrgRsc_GetDataOfLinkFromClipboard (struct PrgRsc_Link *Link,
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
                               struct Prg_ItmRscCodes *SelectedItmRscCodes)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Remove_resource;
   extern const char *Txt_Resources;
   MYSQL_RES *mysql_res;
   unsigned NumRsc;
   unsigned NumResources;
   struct Prg_Item Item;
   static bool GetHiddenResources[Prg_NUM_LISTING_TYPES] =
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
      [Prg_SHOW_CLIPBOARD      ] = true,
      [Prg_CHANGE_RESOURCE_LINK] = true,
      [Prg_END_EDIT_RES        ] = false,
     };
   static bool ShowListWhenEmpty[Prg_NUM_LISTING_TYPES] =
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
      [Prg_SHOW_CLIPBOARD      ] = true,
      [Prg_CHANGE_RESOURCE_LINK] = true,
      [Prg_END_EDIT_RES        ] = true,
     };
   static bool FeaturedList[Prg_NUM_LISTING_TYPES] =
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
      [Prg_SHOW_CLIPBOARD      ] = true,
      [Prg_CHANGE_RESOURCE_LINK] = true,
      [Prg_END_EDIT_RES        ] = true,
     };
   static void (*FunctionToDrawContextualIcons[Prg_NUM_LISTING_TYPES]) (void *ItmRsc) =
     {
      [Prg_PRINT               ] = NULL,
      [Prg_VIEW                ] = NULL,
      [Prg_EDIT_ITEMS          ] = PrgRsc_PutIconsEditResources,
      [Prg_FORM_NEW_END_ITEM   ] = PrgRsc_PutIconsEditResources,
      [Prg_FORM_NEW_CHILD_ITEM ] = PrgRsc_PutIconsEditResources,
      [Prg_FORM_EDIT_ITEM      ] = PrgRsc_PutIconsEditResources,
      [Prg_END_EDIT_ITEM       ] = PrgRsc_PutIconsEditResources,
      [Prg_RECEIVE_ITEM        ] = PrgRsc_PutIconsEditResources,
      [Prg_EDIT_RESOURCES      ] = PrgRsc_PutIconsViewResources,
      [Prg_SHOW_CLIPBOARD      ] = PrgRsc_PutIconsViewResources,
      [Prg_CHANGE_RESOURCE_LINK] = PrgRsc_PutIconsViewResources,
      [Prg_END_EDIT_RES        ] = PrgRsc_PutIconsEditResources,
     };

   /***** Trivial check *****/
   if (SelectedItmRscCodes->ItmCod <= 0)
      return;

   /***** Get list of item resources from database *****/
   NumResources = Prg_DB_GetListResources (&mysql_res,SelectedItmRscCodes->ItmCod,
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
					   Prg_PutParams,SelectedItmRscCodes,
					   Btn_REMOVE_BUTTON,Txt_Remove_resource);
	       break;
	    default:
	       Ale_ShowAlerts (PrgRsc_RESOURCE_SECTION_ID);
	       break;
	   }

      /***** Begin box *****/
      Box_BoxBegin ("100%",Txt_Resources,
		    FunctionToDrawContextualIcons[ListingType],SelectedItmRscCodes,
		    Hlp_COURSE_Program,Box_NOT_CLOSABLE);

	 /***** Table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);
	    HTM_TBODY_Begin (NULL);

	       /***** Write all item resources *****/
	       for (NumRsc = 0, The_ResetRowColor1 (1);
		    NumRsc < NumResources;
		    NumRsc++, The_ChangeRowColor1 (1))
		 {
		  /* Get data of this item resource */
		  PrgRsc_GetDataOfResource (&Item,&mysql_res);

		  /* Show item */
		  switch (ListingType)
		    {
		     case Prg_EDIT_RESOURCES:
		     case Prg_SHOW_CLIPBOARD:
		     case Prg_CHANGE_RESOURCE_LINK:
			PrgRsc_WriteRowEditResource (ListingType,
			                             NumRsc,NumResources,
			                             &Item,SelectedItmRscCodes);
			break;
		     default:
			PrgRsc_WriteRowViewResource (NumRsc,&Item);
			break;
		    }
		 }

	       /***** Form to create a new resource *****/
	       switch (ListingType)
		 {
		  case Prg_EDIT_RESOURCES:
		  case Prg_SHOW_CLIPBOARD:
		  case Prg_CHANGE_RESOURCE_LINK:
		     PrgRsc_WriteRowNewResource (ListingType,NumResources,
		                                 SelectedItmRscCodes);
		     break;
		  default:
		     break;
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

static void PrgRsc_PutIconsViewResources (void *ItmRscCodes)
  {
   /***** Put icon to create a new item resource *****/
   if (ItmRscCodes)
      if (((struct Prg_ItmRscCodes *) ItmRscCodes)->ItmCod > 0)
	 if (Prg_CheckIfICanEditProgram ())
	    Ico_PutContextualIconToView (ActFrmSeePrgRsc,PrgRsc_RESOURCE_SECTION_ID,
				         Prg_PutParams,ItmRscCodes);
  }

static void PrgRsc_PutIconsEditResources (void *ItmRscCodes)
  {
   /***** Put icon to create a new item resource *****/
   if (ItmRscCodes)
      if (((struct Prg_ItmRscCodes *) ItmRscCodes)->ItmCod > 0)
	 if (Prg_CheckIfICanEditProgram ())
	    Ico_PutContextualIconToEdit (ActFrmEdiPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParams,ItmRscCodes);
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
   Item->Resource.Type = PrgRsc_GetTypeFromString (row[4]);
   Item->Resource.Cod  = Str_ConvertStrCodToLongCod (row[5]);

   /* Get the title of the item resource (row[6]) */
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
      HTM_TD_Begin ("class=\"PRG_MAIN PRG_RSC_%s %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Txt (Item->Resource.Title);
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************** Edit one item resource ***************************/
/*****************************************************************************/

static void PrgRsc_WriteRowEditResource (Prg_ListingType_t ListingType,
                                         unsigned NumRsc,unsigned NumResources,
                                         const struct Prg_Item *Item,
                                         struct Prg_ItmRscCodes *SelectedItmRscCodes)
  {
   struct Prg_ItmRscCodes ItmRscCodes;

   ItmRscCodes.ItmCod = Item->Hierarchy.ItmCod;
   ItmRscCodes.RscCod = Item->Resource.Hierarchy.RscCod;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item resource *****/
      HTM_TD_Begin ("class=\"PRG_COL1 LT %s\"",The_GetColorRows1 (1));
	 PrgRsc_PutFormsToRemEditOneResource (NumRsc,NumResources,
	                                      &ItmRscCodes,Item->Resource.Hierarchy.Hidden);
      HTM_TD_End ();

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumRsc + 1);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT %s\"",The_GetColorRows1 (1));
	 Frm_BeginFormAnchor (ActRenPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
	    Prg_PutParams (&ItmRscCodes);
	    HTM_INPUT_TEXT ("Title",PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE,Item->Resource.Title,
			    HTM_SUBMIT_ON_CHANGE,
			    "class=\"INPUT_FULL_NAME INPUT_%s\"",
			    The_GetSuffix ());
	 Frm_EndForm ();
      HTM_TD_End ();

      /***** Icon to edit link *****/
      HTM_TD_Begin ("class=\"PRG_MAIN RT %s\"",The_GetColorRows1 (1));
	 Ico_PutContextualIconToGetLink (ActSeeCliPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParams,&ItmRscCodes);
      HTM_TD_End ();

      /***** Link *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT %s\"",The_GetColorRows1 (1));

         /* Current link */
	 // Ale_ShowAlert (Ale_INFO,"Current link.");

	 /* Show clipboard to change resource link */
	 if (ListingType == Prg_SHOW_CLIPBOARD &&
	     Item->Resource.Hierarchy.RscCod == SelectedItmRscCodes->RscCod)
	    PrgRsc_ShowClipboard (Item);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Edit a new item resource **************************/
/*****************************************************************************/

static void PrgRsc_WriteRowNewResource (Prg_ListingType_t ListingType,
                                        unsigned NumResources,
                                        struct Prg_ItmRscCodes *SelectedItmRscCodes)
  {
   struct Prg_Item Item;
   struct Prg_ItmRscCodes ItmRscCodes;

   Prg_ResetItem (&Item);
   Item.Hierarchy.ItmCod = SelectedItmRscCodes->ItmCod;
   ItmRscCodes.ItmCod = Item.Hierarchy.ItmCod;
   ItmRscCodes.RscCod = Item.Resource.Hierarchy.RscCod;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item resource *****/
      HTM_TD_Begin ("class=\"PRG_COL1 LT %s\"",The_GetColorRows1 (1));
      HTM_TD_End ();

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumResources + 1);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT %s\"",The_GetColorRows1 (1));
	 Frm_BeginFormAnchor (ActNewPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
	    Prg_PutParams (&ItmRscCodes);
	    HTM_INPUT_TEXT ("Title",PrgRsc_MAX_CHARS_PROGRAM_RESOURCE_TITLE,"",
			    HTM_SUBMIT_ON_CHANGE,
			    "placeholder=\"%s\""
			    " class=\"INPUT_FULL_NAME INPUT_%s\"",
			    "Nuevo recurso",	// TODO: Need translation!!!!!!!!!!!!!!!!!!!
			    The_GetSuffix ());
	 Frm_EndForm ();
      HTM_TD_End ();

      /***** Icon to edit link *****/
      HTM_TD_Begin ("class=\"PRG_MAIN RT %s\"",The_GetColorRows1 (1));
	 Ico_PutContextualIconToGetLink (ActSeeCliPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
					 Prg_PutParams,&ItmRscCodes);
      HTM_TD_End ();

      /***** Link *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT %s\"",The_GetColorRows1 (1));

	 /* Show clipboard to change resource link */
	 if (ListingType == Prg_SHOW_CLIPBOARD &&
	     SelectedItmRscCodes->RscCod <= 0)	// No resource selected
	    PrgRsc_ShowClipboard (&Item);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void PrgRsc_PutFormsToRemEditOneResource (unsigned NumRsc,
                                                 unsigned NumResources,
                                                 struct Prg_ItmRscCodes *ItmRscCodes,
                                                 bool Hidden)
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
	                                Prg_PutParams,ItmRscCodes);

	 /***** Icon to hide/unhide item resource *****/
	 Ico_PutContextualIconToHideUnhide (ActionHideUnhide,PrgRsc_RESOURCE_SECTION_ID,
					    Prg_PutParams,ItmRscCodes,
					    Hidden);

	 /***** Icon to move up the item *****/
	 if (NumRsc > 0)
	    Lay_PutContextualLinkOnlyIcon (ActUp_PrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                   Prg_PutParams,ItmRscCodes,
	 			           "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the item *****/
	 if (NumRsc < NumResources - 1)
	    Lay_PutContextualLinkOnlyIcon (ActDwnPrgRsc,PrgRsc_RESOURCE_SECTION_ID,
	                                   Prg_PutParams,ItmRscCodes,
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
/***************** Get parameter with code of item resource ******************/
/*****************************************************************************/

long PrgRsc_GetParamRscCod (void)
  {
   return Par_GetParToLong ("RscCod");
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
   Prg_ShowAllItems (Prg_SHOW_CLIPBOARD,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/***************** Show clipboard to change resource link ********************/
/*****************************************************************************/

static void PrgRsc_ShowClipboard (const struct Prg_Item *Item)
  {
   MYSQL_RES *mysql_res;
   unsigned NumLink;
   unsigned NumLinks;
   struct PrgRsc_Link Link;
   struct Prg_ItmRscCodes ItmRscCodes;

   ItmRscCodes.ItmCod = Item->Hierarchy.ItmCod;
   ItmRscCodes.RscCod = Item->Resource.Hierarchy.RscCod;

   /***** Begin form *****/
   Frm_BeginFormAnchor (ActChgLnkPrgRsc,PrgRsc_RESOURCE_SECTION_ID);
      Prg_PutParams (&ItmRscCodes);

      /***** Begin list *****/
      HTM_UL_Begin ("class=\"PRG_CLIPBOARD\"");

	 /***** Current link *****/
	 Link.Type = Item->Resource.Type;
	 Link.Cod  = Item->Resource.Cod;
	 PrgRsc_WriteRowClipboard (0,&Link);

	 /***** Get list of item resources from database *****/
	 NumLinks = Prg_DB_GetClipboard (&mysql_res);

	 /***** Write all item resources *****/
	 for (NumLink  = 1;
	      NumLink <= NumLinks;
	      NumLink++)
	   {
	    /* Get data of this link */
	    PrgRsc_GetDataOfLinkFromClipboard (&Link,&mysql_res);

	    /* Show link */
	    PrgRsc_WriteRowClipboard (NumLink,&Link);
	   }

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);

      /***** End list *****/
      HTM_UL_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************ Show one link from clipboard ***********************/
/*****************************************************************************/

static void PrgRsc_WriteRowClipboard (unsigned NumLink,
                                      const struct PrgRsc_Link *Link)
  {
   extern const char *Prg_ResourceTypesDB[PrgRsc_NUM_TYPES];

   HTM_LI_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
      HTM_LABEL_Begin (NULL);
         if (NumLink)
	    HTM_INPUT_RADIO ("Link",true,
			     "value=\"%s_%ld\"",
			     Prg_ResourceTypesDB[Link->Type],Link->Cod);
         else
            HTM_INPUT_RADIO ("Link",false,
			     "value=\"%s_%ld\" checked=\"checked\"",
			     Prg_ResourceTypesDB[Link->Type],Link->Cod);

	 /***** Type *****/
         Ico_PutIconOn (Prg_ResourceTypesLogos[Link->Type],Ico_BLACK,Prg_ResourceTypesDB[Link->Type]);

	 /***** Code *****/
	 if (Link->Cod > 0)
	    HTM_Long (Link->Cod);
	 else
	    HTM_Txt ("sin enlace");	// TODO: Need translation!!!!!

      HTM_LABEL_End ();
   HTM_LI_End ();
  }

/*****************************************************************************/
/***************** Show clipboard to change resource link ********************/
/*****************************************************************************/

void PrgRsc_ChangeLink (void)
  {
   extern const char *Prg_ResourceTypesDB[PrgRsc_NUM_TYPES];
   struct Prg_Item Item;
   char TypeCod[3 + 1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1];
   char TypeStr[3 + 1];
   PrgRsc_Type_t Type;
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
      Type = PrgRsc_GetTypeFromString (TypeStr);

      /***** Update resource link *****/
      Prg_DB_UpdateRscLink (Item.Resource.Hierarchy.RscCod,Type,Cod);

      /***** Remove link from clipboard *****/
      Prg_DB_RemoveLinkFromClipboard (Type,Cod);
     }

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_SHOW_CLIPBOARD,Item.Hierarchy.ItmCod,Item.Resource.Hierarchy.RscCod);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/********************** Get resource data from clipboard *********************/
/*****************************************************************************/

static void PrgRsc_GetDataOfLinkFromClipboard (struct PrgRsc_Link *Link,
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
