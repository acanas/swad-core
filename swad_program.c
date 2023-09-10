// swad_program.c: course program

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
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_program_resource.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Prg_NUM_MOVEMENTS_UP_DOWN 2
typedef enum
  {
   Prg_MOVE_UP,
   Prg_MOVE_DOWN,
  } Prg_MoveUpDown_t;

struct Level
  {
   unsigned Number;	// Numbers for each level from 1 to maximum level
   bool Expanded;	// If each level from 1 to maximum level is expanded
   HidVis_HiddenOrVisible_t HiddenOrVisible;	// If each level...
						// ...from 1 to maximum level...
						// ...is hidden or visible
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

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

static const char *Prg_ITEM_SECTION_ID = "item_section";
static const char *Prg_HIGHLIGHTED_SECTION_ID = "prg_highlighted";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prg_PutIconsListItems (__attribute__((unused)) void *Args);
static void Prg_PutIconsEditItems (__attribute__((unused)) void *Args);
static void Prg_PutIconToEditProgram (void);
static void Prg_PutIconToViewProgram (void);
static void Prg_PutIconToCreateNewItem (void);
static void Prg_PutIconToViewResourceClipboard (void);

static void Prg_WriteRowItem (Prg_ListingType_t ListingType,
                              unsigned NumItem,struct Prg_Item *Item,
                              bool Expanded,
                              long SelectedItmCod,
                              long SelectedRscCod);
static void Prg_PutIconToContractExpandItem (struct Prg_Item *Item,
                                             bool Expanded,bool Editing);
static void Prg_WriteItemText (long ItmCod,HidVis_HiddenOrVisible_t HiddenOrVisible);
static void Prg_WriteRowToCreateItem (long ParentItmCod,unsigned FormLevel);
static void Prg_SetTitleClass (char **TitleClass,unsigned Level);
static void Prg_FreeTitleClass (char *TitleClass);

static void Prg_SetMaxItemLevel (unsigned Level);
static unsigned Prg_GetMaxItemLevel (void);
static unsigned Prg_CalculateMaxItemLevel (void);
static void Prg_CreateLevels (void);
static void Prg_FreeLevels (void);
static void Prg_IncreaseNumberInLevel (unsigned Level);
static unsigned Prg_GetCurrentNumberInLevel (unsigned Level);
static void Prg_WriteNumItem (unsigned Level);
static void Prg_WriteNumNewItem (unsigned Level);

static void Prg_SetExpandedLevel (unsigned Level,bool Expanded);
static void Prg_SetHiddenLevel (unsigned Level,HidVis_HiddenOrVisible_t HiddenOrVisible);
static bool Prg_GetExpandedLevel (unsigned Level);
static HidVis_HiddenOrVisible_t Prg_GetHiddenLevel (unsigned Level);

static bool Prg_CheckIfAllHigherLevelsAreExpanded (unsigned CurrentLevel);
static HidVis_HiddenOrVisible_t Prg_CheckIfAnyHigherLevelIsHidden (unsigned CurrentLevel);

static void Prg_PutFormsToRemEditOneItem (Prg_ListingType_t ListingType,
                                          unsigned NumItem,
                                          struct Prg_Item *Item,
                                          bool HighlightItem);
static bool Prg_CheckIfMoveUpIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveDownIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveLeftIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveRightIsAllowed (unsigned NumItem);

static void Prg_GetItemDataByCod (struct Prg_Item *Item);
static void Prg_GetItemDataFromRow (MYSQL_RES **mysql_res,
                                    struct Prg_Item *Item,
                                    unsigned NumRows);

static void Prg_HideOrUnhideItem (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void Prg_MoveUpDownItem (Prg_MoveUpDown_t UpDown);
static bool Prg_ExchangeItemRanges (int NumItemTop,int NumItemBottom);
static int Prg_GetPrevBrother (int NumItem);
static int Prg_GetNextBrother (int NumItem);

static void Prg_MoveLeftRightItem (Prg_MoveLeftRight_t LeftRight);

static void Prg_ExpandContractItem (Prg_ExpandContract_t ExpandContract);

static void Prg_SetItemRangeOnlyItem (unsigned NumItem,struct Prg_ItemRange *ItemRange);
static void Prg_SetItemRangeWithAllChildren (unsigned NumItem,struct Prg_ItemRange *ItemRange);
static unsigned Prg_GetLastChild (int NumItem);

static void Prg_ShowFormToCreateItem (long ParentItmCod);
static void Prg_ShowFormToChangeItem (long ItmCod);
static void Prg_ParsFormItem (void *ItmCod);
static void Prg_ShowFormItem (const struct Prg_Item *Item,
			      const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME],
			      const char *Txt);

static void Prg_InsertItem (const struct Prg_Item *ParentItem,
		            struct Prg_Item *Item,const char *Txt);

/*****************************************************************************/
/************************** List all program items ***************************/
/*****************************************************************************/

void Prg_ShowCourseProgram (void)
  {
   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Show course program without highlighting any item *****/
   Prg_ShowAllItems (Prg_VIEW,-1L,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

void Prg_EditCourseProgram (void)
  {
   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Show course program without highlighting any item *****/
   Prg_ShowAllItems (Prg_EDIT_ITEMS,-1L,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/************************* Show all program items ****************************/
/*****************************************************************************/

void Prg_ShowAllItems (Prg_ListingType_t ListingType,
                       long SelectedItmCod,long SelectedRscCod)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;
   long ParentItmCod = -1L;	// Initialized to avoid warning
   unsigned NumItem;
   unsigned FormLevel = 0;	// Initialized to avoid warning
   struct Prg_Item Item;
   struct Prg_ItemRange ToHighlight;
   bool Expanded;
   char *Title;
   static bool FirstTBodyOpen = false;
   static void (*FunctionToDrawContextualIcons[Prg_NUM_LISTING_TYPES]) (void *Args) =
     {
      [Prg_PRINT               ] = NULL,
      [Prg_VIEW                ] = Prg_PutIconsListItems,
      [Prg_EDIT_ITEMS          ] = Prg_PutIconsEditItems,
      [Prg_FORM_NEW_END_ITEM   ] = Prg_PutIconsEditItems,
      [Prg_FORM_NEW_CHILD_ITEM ] = Prg_PutIconsEditItems,
      [Prg_FORM_EDIT_ITEM      ] = Prg_PutIconsEditItems,
      [Prg_END_EDIT_ITEM       ] = Prg_PutIconsEditItems,
      [Prg_RECEIVE_ITEM        ] = Prg_PutIconsEditItems,
      [Prg_EDIT_RESOURCES      ] = Prg_PutIconsEditItems,
      [Prg_EDIT_RESOURCE_LINK  ] = Prg_PutIconsEditItems,
      [Prg_CHANGE_RESOURCE_LINK] = Prg_PutIconsEditItems,
      [Prg_END_EDIT_RES        ] = Prg_PutIconsEditItems,
     };

   /***** Create numbers and hidden levels *****/
   Prg_SetMaxItemLevel (Prg_CalculateMaxItemLevel ());
   Prg_CreateLevels ();

   /***** Compute form level *****/
   ToHighlight.Begin =
   ToHighlight.End   = 0;
   switch (ListingType)
     {
      case Prg_VIEW:
      case Prg_EDIT_ITEMS:
	 if (SelectedItmCod > 0)
	    Prg_SetItemRangeWithAllChildren (Prg_GetNumItemFromItmCod (SelectedItmCod),
					     &ToHighlight);
	 break;
      case Prg_RECEIVE_ITEM:
      // case Prg_EDIT_RESOURCES:	// Uncomment to higlight item
      // case Prg_EDIT_RESOURCE_LINK:	// Uncomment to higlight item
      // case Prg_CHANGE_RESOURCE_LINK:	// Uncomment to higlight item
      // case Prg_END_EDIT_RES:		// Uncomment to higlight item
	 if (SelectedItmCod > 0)
            Prg_SetItemRangeOnlyItem (Prg_GetNumItemFromItmCod (SelectedItmCod),
                                      &ToHighlight);
         break;
      case Prg_FORM_NEW_CHILD_ITEM:
	 ParentItmCod = SelectedItmCod;		// Item code here is parent of the item to create
	 NumItem = Prg_GetNumItemFromItmCod (SelectedItmCod);
	 SelectedItmCod = Prg_GetItmCodFromNumItem (Prg_GetLastChild (NumItem));
	 FormLevel = Prg_GetLevelFromNumItem (NumItem) + 1;
	 break;
      default:
	 break;
     }

   /***** Begin box *****/
   if (asprintf (&Title,Txt_COURSE_program,Gbl.Hierarchy.Crs.ShrtName) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxBegin ("100%",Title,
                 FunctionToDrawContextualIcons[ListingType],NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);
   free (Title);

      /***** Table *****/
      HTM_TABLE_BeginWideMarginPadding (2);

	 /* In general, the table is divided into three bodys:
	 1. Rows before highlighted: <tbody></tbody>
	 2. Rows highlighted:        <tbody id="prg_highlighted"></tbody>
	 3. Rows after highlighted:  <tbody></tbody> */
	 HTM_TBODY_Begin (NULL);		// 1st tbody start
	 FirstTBodyOpen = true;

	 /***** Write all program items *****/
	 for (NumItem = 0, The_ResetRowColor ();
	      NumItem < Prg_Gbl.List.NumItems;
	      NumItem++)
	   {
	    /* Get data of this program item */
	    Item.Hierarchy.ItmCod = Prg_GetItmCodFromNumItem (NumItem);
	    Prg_GetItemDataByCod (&Item);

	    /* Begin range to highlight? */
	    if (Item.Hierarchy.ItmInd == ToHighlight.Begin)	// Begin of the highlighted range
	      {
	       if (FirstTBodyOpen)
		 {
		  HTM_TBODY_End ();				// 1st tbody end
		  FirstTBodyOpen = false;
		 }
	       HTM_TBODY_Begin ("id=\"%s\"",
	                        Prg_HIGHLIGHTED_SECTION_ID);	// Highlighted tbody start
	      }

	    /* Set if this level is expanded */
	    Expanded = Prg_DB_GetIfExpandedItem (Item.Hierarchy.ItmCod);
	    Prg_SetExpandedLevel (Item.Hierarchy.Level,Expanded);

	    /* Show this row only if all higher levels are expanded */
	    if (Prg_CheckIfAllHigherLevelsAreExpanded (Item.Hierarchy.Level))
	      {
	       /* Write row with this item */
	       Prg_WriteRowItem (ListingType,NumItem,&Item,Expanded,
				 SelectedItmCod,SelectedRscCod);
               The_ChangeRowColor ();

	       /* Show form to create child item? */
	       if (ListingType == Prg_FORM_NEW_CHILD_ITEM &&
		   Item.Hierarchy.ItmCod == SelectedItmCod)
		 {
		  Prg_WriteRowToCreateItem (ParentItmCod,FormLevel);
		  The_ChangeRowColor ();
		 }
	      }

	    /* End range to highlight? */
	    if (Item.Hierarchy.ItmInd == ToHighlight.End)	// End of the highlighted range
	      {
	       HTM_TBODY_End ();				// Highlighted tbody end
	       if (NumItem < Prg_Gbl.List.NumItems - 1)		// Not the last item
		  HTM_TBODY_Begin (NULL);			// 3rd tbody begin
	      }
	   }

	 /***** Create item at the end? *****/
	 if (ListingType == Prg_FORM_NEW_END_ITEM)
	    Prg_WriteRowToCreateItem (-1L,1);

	 /***** End table *****/
	 HTM_TBODY_End ();					// 3rd tbody end
      HTM_TABLE_End ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free hidden levels and numbers *****/
   Prg_FreeLevels ();
  }

/*****************************************************************************/
/******************* Check if I can create program items *********************/
/*****************************************************************************/

bool Prg_CheckIfICanEditProgram (void)
  {
   return Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
          Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM;
  }

/*****************************************************************************/
/************** Put contextual icons in list of program items ****************/
/*****************************************************************************/

static void Prg_PutIconsListItems (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit program *****/
   if (Prg_CheckIfICanEditProgram ())
      Prg_PutIconToEditProgram ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_COURSE_PROGRAMS);
  }

/*****************************************************************************/
/************** Put contextual icons in list of program items ****************/
/*****************************************************************************/

static void Prg_PutIconsEditItems (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view program *****/
   Prg_PutIconToViewProgram ();

   if (Prg_CheckIfICanEditProgram ())
     {
      /***** Put icon to create a new program item *****/
      Prg_PutIconToCreateNewItem ();

      /***** Put icon to view resource clipboard *****/
      Prg_PutIconToViewResourceClipboard ();
     }

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_COURSE_PROGRAMS);
  }

/*****************************************************************************/
/************************* Put icon to edit program **************************/
/*****************************************************************************/

static void Prg_PutIconToEditProgram (void)
  {
   Ico_PutContextualIconToEdit (ActEdiPrg,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/************************* Put icon to view program **************************/
/*****************************************************************************/

static void Prg_PutIconToViewProgram (void)
  {
   Ico_PutContextualIconToView (ActSeePrg,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/****************** Put icon to create a new program item ********************/
/*****************************************************************************/

static void Prg_PutIconToCreateNewItem (void)
  {
   long ItmCod = -1L;

   Ico_PutContextualIconToAdd (ActFrmNewPrgItm,Prg_ITEM_SECTION_ID,
                               Prg_PutParItmCod,&ItmCod);
  }

/*****************************************************************************/
/******************* Put icon to view resource clipboard *********************/
/*****************************************************************************/

static void Prg_PutIconToViewResourceClipboard (void)
  {
   Ico_PutContextualIconToViewClipboard (ActSeeRscCli_InPrg,NULL,
                                         NULL,NULL);
  }

/*****************************************************************************/
/************************** Show one program item ****************************/
/*****************************************************************************/

static void Prg_WriteRowItem (Prg_ListingType_t ListingType,
                              unsigned NumItem,struct Prg_Item *Item,
                              bool Expanded,
                              long SelectedItmCod,
                              long SelectedRscCod)
  {
   extern const char *HidVis_PrgClass[HidVis_NUM_HIDDEN_VISIBLE];
   static unsigned UniqueId = 0;
   static bool EditingProgram[Prg_NUM_LISTING_TYPES] =
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
   static const char *RowSpan[2] =
     {
      [false] = "",			// Not expanded
      [true ] = " rowspan=\"2\"",	// Expanded
     };
   HidVis_HiddenOrVisible_t HiddenOrVisible = HidVis_HIDDEN;	// Initialized to avoid warning
   char *Id;
   unsigned ColSpan;
   unsigned NumCol;
   char *TitleClass;
   Dat_StartEndTime_t StartEndTime;
   bool HighlightItem;

   /***** Check if this item should be shown as hidden *****/
   Prg_SetHiddenLevel (Item->Hierarchy.Level,Item->Hierarchy.HiddenOrVisible);
   switch (Item->Hierarchy.HiddenOrVisible)
     {
      case HidVis_HIDDEN:	// this item is marked as hidden
         HiddenOrVisible = HidVis_HIDDEN;
	 break;
      case HidVis_VISIBLE:	// this item is not marked as hidden
         HiddenOrVisible = Prg_CheckIfAnyHigherLevelIsHidden (Item->Hierarchy.Level);
	 break;
     }

   if (EditingProgram[ListingType] || HiddenOrVisible == HidVis_VISIBLE)
     {
      /***** Increase number in level *****/
      Prg_IncreaseNumberInLevel (Item->Hierarchy.Level);

      /***** Is this the item selected? *****/
      HighlightItem = Item->Hierarchy.ItmCod == SelectedItmCod &&
		      (ListingType == Prg_FORM_EDIT_ITEM ||
		       ListingType == Prg_END_EDIT_ITEM);

      /***** First row (title and dates) *****/
      HTM_TR_Begin (NULL);

	 /* Title CSS class */
	 Prg_SetTitleClass (&TitleClass,Item->Hierarchy.Level);

	 /* Indent depending on the level */
	 for (NumCol = 1;
	      NumCol < Item->Hierarchy.Level;
	      NumCol++)
	   {
	    HTM_TD_Begin ("class=\"%s\"%s",
	                  The_GetColorRows (),RowSpan[Expanded]);
	    HTM_TD_End ();
	   }

	 /* Expand/contract this program item */
	 HTM_TD_Begin ("class=\"LT %s\"%s",
	               The_GetColorRows (),RowSpan[Expanded]);
	    Prg_PutIconToContractExpandItem (Item,Expanded,EditingProgram[ListingType]);
	 HTM_TD_End ();

	 /* Forms to remove/edit this program item */
	 if (EditingProgram[ListingType])
	   {
	    HTM_TD_Begin ("class=\"PRG_COL1 LT %s\"%s",
			  The_GetColorRows (),RowSpan[Expanded]);
	       Prg_PutFormsToRemEditOneItem (ListingType,NumItem,Item,HighlightItem);
	    HTM_TD_End ();
	   }

	 /* Item number */
	 HTM_TD_Begin ("class=\"PRG_NUM %s\"%s",
	               The_GetColorRows (),RowSpan[Expanded]);
	    HTM_DIV_Begin ("class=\"RT %s%s\"",
			   TitleClass,
			   HidVis_PrgClass[HiddenOrVisible]);
	       Prg_WriteNumItem (Item->Hierarchy.Level);
	    HTM_DIV_End ();
	 HTM_TD_End ();

	 /* Title */
	 ColSpan = (Prg_GetMaxItemLevel () + 2) - Item->Hierarchy.Level;
	 switch (ListingType)
	   {
	    case Prg_PRINT:
	       HTM_TD_Begin ("colspan=\"%u\" class=\"PRG_MAIN\"",ColSpan);
	       break;
	    default:
	       HTM_TD_Begin ("colspan=\"%u\" class=\"PRG_MAIN %s\"",ColSpan,
			     The_GetColorRows ());

	       break;
	   }

	    if (HighlightItem)
	       HTM_ARTICLE_Begin (Prg_ITEM_SECTION_ID);
	    HTM_DIV_Begin ("class=\"LT %s%s\"",
			   TitleClass,
			   HidVis_PrgClass[HiddenOrVisible]);
	       HTM_Txt (Item->Title);
	    HTM_DIV_End ();
	    if (HighlightItem)
	       HTM_ARTICLE_End ();
	 HTM_TD_End ();

	 /* Start/end date/time */
	 switch (ListingType)
	   {
	    case Prg_PRINT:
	       HTM_TD_Begin ("class=\"PRG_DATE RT\"");
	       break;
	    default:
	       HTM_TD_Begin ("class=\"PRG_DATE RT %s\"",The_GetColorRows ());
	       break;
	   }
	 UniqueId++;
	 for (StartEndTime  = (Dat_StartEndTime_t) 0;
	      StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	      StartEndTime++)
	   {
	    if (asprintf (&Id,"prg_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	       Err_NotEnoughMemoryExit ();
	    HTM_DIV_Begin ("id=\"%s\" class=\"%s_%s%s\"",
			   Id,
			   Item->Open ? "DATE_GREEN" :
					"DATE_RED",
			   The_GetSuffix (),
			   HidVis_PrgClass[HiddenOrVisible]);
	       Dat_WriteLocalDateHMSFromUTC (Id,Item->TimeUTC[StartEndTime],
					     Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
					     true,true,false,0x6);
	    HTM_DIV_End ();
	    free (Id);
	   }
	 HTM_TD_End ();

	 /* Free title CSS class */
	 Prg_FreeTitleClass (TitleClass);

      HTM_TR_End ();

      /***** Second row (text and resources) *****/
      if (Expanded)
	{
	 HTM_TR_Begin (NULL);

	    /* Begin text and resources */
	    ColSpan++;
	    switch (ListingType)
	      {
	       case Prg_PRINT:
		  HTM_TD_Begin ("colspan=\"%u\" class=\"PRG_MAIN\"",ColSpan);
		  break;
	       default:
		  HTM_TD_Begin ("colspan=\"%u\" class=\"PRG_MAIN %s\"",
				ColSpan,The_GetColorRows ());
		  break;
	      }

	    /* Item text / form */
	    if (ListingType == Prg_FORM_EDIT_ITEM && HighlightItem)
	       /* Form to change item title, dates and text */
	       Prg_ShowFormToChangeItem (Item->Hierarchy.ItmCod);
	    else
	       /* Text */
	       Prg_WriteItemText (Item->Hierarchy.ItmCod,HiddenOrVisible);

	    /* List of resources */
	    PrgRsc_ListItemResources (ListingType,Item,SelectedItmCod,SelectedRscCod);

	    /* End text and resources */
	    HTM_TD_End ();

	 HTM_TR_End ();
	}
     }
  }

/*****************************************************************************/
/************************ Put icon to expand an item *************************/
/*****************************************************************************/

static void Prg_PutIconToContractExpandItem (struct Prg_Item *Item,
                                             bool Expanded,bool Editing)
  {
   static Act_Action_t NextAction[2][2] =
     {
      [false][false] = ActExpSeePrgItm,	// Contracted, Not editing ==> action to expand
      [false][true ] = ActExpEdiPrgItm,	// Contracted,     Editing ==> action to expand
      [true ][false] = ActConSeePrgItm,	// Expanded  , Not editing ==> action to contract
      [true ][true ] = ActConEdiPrgItm,	// Expanded  ,     Editing ==> action to contract
     };
   static void (*PutContextualIcon[2]) (const Act_Action_t NextAction,const char *Anchor,
                                        void (*FuncPars) (void *Args),void *Args) =
     {
      [false] = Ico_PutContextualIconToExpand,		// Contracted ==> function to expand
      [true ] = Ico_PutContextualIconToContract,	// Expanded   ==> function to contract
     };

   /***** Icon to hide/unhide program item *****/
   PutContextualIcon[Expanded] (NextAction[Expanded][Editing],Prg_HIGHLIGHTED_SECTION_ID,
			        Prg_PutParItmCod,&Item->Hierarchy.ItmCod);
  }

/*****************************************************************************/
/**************************** Show item text *********************************/
/*****************************************************************************/

static void Prg_WriteItemText (long ItmCod,HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_PrgClass[HidVis_NUM_HIDDEN_VISIBLE];
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /* Text */
   Prg_DB_GetItemTxt (ItmCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
		     Txt,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
   ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   HTM_DIV_Begin ("class=\"PAR PRG_TXT_%s%s\"",
		  The_GetSuffix (),
		  HidVis_PrgClass[HiddenOrVisible]);
      HTM_Txt (Txt);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Show item form *********************************/
/*****************************************************************************/

static void Prg_WriteRowToCreateItem (long ParentItmCod,unsigned FormLevel)
  {
   char *TitleClass;
   unsigned ColSpan;
   unsigned NumCol;

   /***** Title CSS class *****/
   Prg_SetTitleClass (&TitleClass,FormLevel);

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Column under expand/contract icon *****/
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
      HTM_TD_End ();

      /***** Column under icons *****/
      HTM_TD_Begin ("class=\"PRG_COL1 LT %s\"",The_GetColorRows ());
      HTM_TD_End ();

      /***** Indent depending on the level *****/
      for (NumCol = 1;
	   NumCol < FormLevel;
	   NumCol++)
	{
	 HTM_TD_Begin ("class=\"%s\"",The_GetColorRows ());
	 HTM_TD_End ();
	}

      /***** Item number *****/
      HTM_TD_Begin ("class=\"PRG_NUM %s RT %s\"",
                    TitleClass,The_GetColorRows ());
	 Prg_WriteNumNewItem (FormLevel);
      HTM_TD_End ();

      /***** Show form to create new item as child *****/
      ColSpan = (Prg_GetMaxItemLevel () + 4) - FormLevel;
      HTM_TD_Begin ("colspan=\"%u\" class=\"PRG_MAIN %s\"",
		    ColSpan,The_GetColorRows ());
         /* Form for item data */
	 HTM_ARTICLE_Begin (Prg_ITEM_SECTION_ID);
	    Prg_ShowFormToCreateItem (ParentItmCod);
	 HTM_ARTICLE_End ();
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();

   /***** Free title CSS class *****/
   Prg_FreeTitleClass (TitleClass);
  }

/*****************************************************************************/
/**************** Set / free title class depending on level ******************/
/*****************************************************************************/

static void Prg_SetTitleClass (char **TitleClass,unsigned Level)
  {
   if (asprintf (TitleClass,"PRG_TIT_%u_%s",
		 Level < 5 ? Level :
			     5,
		 The_GetSuffix ()) < 0)
      Err_NotEnoughMemoryExit ();
  }

static void Prg_FreeTitleClass (char *TitleClass)
  {
   free (TitleClass);
  }

/*****************************************************************************/
/************** Set and get maximum level in a course program ****************/
/*****************************************************************************/

static void Prg_SetMaxItemLevel (unsigned Level)
  {
   Prg_Gbl.MaxLevel = Level;
  }

static unsigned Prg_GetMaxItemLevel (void)
  {
   return Prg_Gbl.MaxLevel;
  }

/*****************************************************************************/
/******** Calculate maximum level of indentation in a course program *********/
/*****************************************************************************/

static unsigned Prg_CalculateMaxItemLevel (void)
  {
   unsigned NumItem;
   unsigned MaxLevel = 0;	// Return 0 if no items

   /***** Compute maximum level of all program items *****/
   for (NumItem = 0;
	NumItem < Prg_Gbl.List.NumItems;
	NumItem++)
      if (Prg_GetLevelFromNumItem (NumItem) > MaxLevel)
	 MaxLevel = Prg_GetLevelFromNumItem (NumItem);

   return MaxLevel;
  }

/*****************************************************************************/
/********************* Allocate memory for item numbers **********************/
/*****************************************************************************/

static void Prg_CreateLevels (void)
  {
   unsigned MaxLevel = Prg_GetMaxItemLevel ();

   if (MaxLevel)
     {
      /***** Allocate memory for item numbers and initialize to 0 *****/
      /*
      Example:  2.5.2.1
                MaxLevel = 4
      Level Number
      ----- ------
        0    N.A. <--- Root level
        1     2
        2     5
        3     2
        4     1
        5     0	  <--- Used to create a new item
      */
      if ((Prg_Gbl.Levels = calloc (1 + MaxLevel + 1,
                                    sizeof (*Prg_Gbl.Levels))) == NULL)
	 Err_NotEnoughMemoryExit ();
     }
   else
      Prg_Gbl.Levels = NULL;
  }

/*****************************************************************************/
/*********************** Free memory for item numbers ************************/
/*****************************************************************************/

static void Prg_FreeLevels (void)
  {
   if (Prg_GetMaxItemLevel () && Prg_Gbl.Levels)
     {
      /***** Free allocated memory for item numbers *****/
      free (Prg_Gbl.Levels);
      Prg_Gbl.Levels = NULL;
     }
  }

/*****************************************************************************/
/**************************** Increase number of item ************************/
/*****************************************************************************/

static void Prg_IncreaseNumberInLevel (unsigned Level)
  {
   /***** Increase number for this level *****/
   Prg_Gbl.Levels[Level    ].Number++;

   /***** Reset number for next level (children) *****/
   Prg_Gbl.Levels[Level + 1].Number = 0;
  }

/*****************************************************************************/
/****************** Get current number of item in a level ********************/
/*****************************************************************************/

static unsigned Prg_GetCurrentNumberInLevel (unsigned Level)
  {
   if (Prg_Gbl.Levels)
      return Prg_Gbl.Levels[Level].Number;

   return 0;
  }

/*****************************************************************************/
/******************** Write number of item in legal style ********************/
/*****************************************************************************/

static void Prg_WriteNumItem (unsigned Level)
  {
   HTM_Unsigned (Prg_GetCurrentNumberInLevel (Level));
  }

static void Prg_WriteNumNewItem (unsigned Level)
  {
   HTM_Unsigned (Prg_GetCurrentNumberInLevel (Level) + 1);
  }

/*****************************************************************************/
/********************** Set / Get if a level is hidden ***********************/
/*****************************************************************************/

static void Prg_SetExpandedLevel (unsigned Level,bool Expanded)
  {
   if (Prg_Gbl.Levels)
      Prg_Gbl.Levels[Level].Expanded = Expanded;
  }

static void Prg_SetHiddenLevel (unsigned Level,HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   if (Prg_Gbl.Levels)
      Prg_Gbl.Levels[Level].HiddenOrVisible = HiddenOrVisible;
  }

static bool Prg_GetExpandedLevel (unsigned Level)
  {
   /* Level 0 (root) is always expanded */
   if (Level == 0)
      return true;

   if (Prg_Gbl.Levels)
      return Prg_Gbl.Levels[Level].Expanded;

   return false;
  }

static HidVis_HiddenOrVisible_t Prg_GetHiddenLevel (unsigned Level)
  {
   /* Level 0 (root) is always visible */
   if (Level == 0)
      return HidVis_VISIBLE;

   if (Prg_Gbl.Levels)
      return Prg_Gbl.Levels[Level].HiddenOrVisible;

   return HidVis_VISIBLE;
  }

/*****************************************************************************/
/********* Check if any level higher than the current one is hidden **********/
/*****************************************************************************/

static bool Prg_CheckIfAllHigherLevelsAreExpanded (unsigned CurrentLevel)
  {
   unsigned Level;

   for (Level = 1;
	Level < CurrentLevel;
	Level++)
      if (!Prg_GetExpandedLevel (Level))	// Contracted?
         return false;	// A level is contracted. Not all are expanded

   return true;	// None is contracted. All are expanded
  }

static HidVis_HiddenOrVisible_t Prg_CheckIfAnyHigherLevelIsHidden (unsigned CurrentLevel)
  {
   unsigned Level;

   for (Level = 1;
	Level < CurrentLevel;
	Level++)
      if (Prg_GetHiddenLevel (Level) == HidVis_HIDDEN)
         return HidVis_HIDDEN;

   return HidVis_VISIBLE;	// None is hidden. All are visible.
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void Prg_PutFormsToRemEditOneItem (Prg_ListingType_t ListingType,
                                          unsigned NumItem,
                                          struct Prg_Item *Item,
                                          bool HighlightItem)
  {
   extern const char *Txt_Movement_not_allowed;
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhPrgItm,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidPrgItm,	// Visible ==> action to hide
     };
   char StrItemIndex[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   /***** Initialize item index string *****/
   snprintf (StrItemIndex,sizeof (StrItemIndex),"%u",Item->Hierarchy.ItmInd);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Icon to remove program item *****/
	 Ico_PutContextualIconToRemove (ActReqRemPrgItm,NULL,
	                                Prg_PutParItmCod,&Item->Hierarchy.ItmCod);

	 /***** Icon to hide/unhide program item *****/
	 Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Prg_HIGHLIGHTED_SECTION_ID,
					    Prg_PutParItmCod,&Item->Hierarchy.ItmCod,
					    Item->Hierarchy.HiddenOrVisible);

	 /***** Icon to edit program item *****/
	 if (ListingType == Prg_FORM_EDIT_ITEM && HighlightItem)
	    Ico_PutContextualIconToView (ActSeePrgItm,Prg_ITEM_SECTION_ID,
					 Prg_PutParItmCod,&Item->Hierarchy.ItmCod);
	 else
	    Ico_PutContextualIconToEdit (ActFrmChgPrgItm,Prg_ITEM_SECTION_ID,
					 Prg_PutParItmCod,&Item->Hierarchy.ItmCod);

	 /***** Icon to add a new child item inside this item *****/
	 Ico_PutContextualIconToAdd (ActFrmNewPrgItm,Prg_ITEM_SECTION_ID,
	                             Prg_PutParItmCod,&Item->Hierarchy.ItmCod);

	 HTM_BR ();

	 /***** Icon to move up the item *****/
	 if (Prg_CheckIfMoveUpIsAllowed (NumItem))
	    Lay_PutContextualLinkOnlyIcon (ActUp_PrgItm,Prg_HIGHLIGHTED_SECTION_ID,
	                                   Prg_PutParItmCod,&Item->Hierarchy.ItmCod,
					   "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Icon to move down the item *****/
	 if (Prg_CheckIfMoveDownIsAllowed (NumItem))
	    Lay_PutContextualLinkOnlyIcon (ActDwnPrgItm,Prg_HIGHLIGHTED_SECTION_ID,
	                                   Prg_PutParItmCod,&Item->Hierarchy.ItmCod,
					   "arrow-down.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Icon to move left item (increase level) *****/
	 if (Prg_CheckIfMoveLeftIsAllowed (NumItem))
	    Lay_PutContextualLinkOnlyIcon (ActLftPrgItm,Prg_HIGHLIGHTED_SECTION_ID,
	                                   Prg_PutParItmCod,&Item->Hierarchy.ItmCod,
					   "arrow-left.svg",Ico_BLACK);
	 else
            Ico_PutIconOff ("arrow-left.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Icon to move right item (indent, decrease level) *****/
	 if (Prg_CheckIfMoveRightIsAllowed (NumItem))
	    Lay_PutContextualLinkOnlyIcon (ActRgtPrgItm,Prg_HIGHLIGHTED_SECTION_ID,
	                                   Prg_PutParItmCod,&Item->Hierarchy.ItmCod,
					   "arrow-right.svg",Ico_BLACK);
	 else
            Ico_PutIconOff ("arrow-right.svg",Ico_BLACK,Txt_Movement_not_allowed);
	 break;
      case Rol_STD:
      case Rol_NET:
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/*********************** Check if item can be moved up ***********************/
/*****************************************************************************/

static bool Prg_CheckIfMoveUpIsAllowed (unsigned NumItem)
  {
   /***** Trivial check: if item is the first one, move up is not allowed *****/
   if (NumItem == 0)
      return false;

   /***** Move up is allowed if the item has brothers before it *****/
   // NumItem >= 1
   return Prg_GetLevelFromNumItem (NumItem - 1) >=
	  Prg_GetLevelFromNumItem (NumItem    );
  }

/*****************************************************************************/
/********************** Check if item can be moved down **********************/
/*****************************************************************************/

static bool Prg_CheckIfMoveDownIsAllowed (unsigned NumItem)
  {
   unsigned i;
   unsigned Level;

   /***** Trivial check: if item is the last one, move up is not allowed *****/
   if (NumItem >= Prg_Gbl.List.NumItems - 1)
      return false;

   /***** Move down is allowed if the item has brothers after it *****/
   // NumItem + 1 < Prg_Gbl.List.NumItems
   Level = Prg_GetLevelFromNumItem (NumItem);
   for (i = NumItem + 1;
	i < Prg_Gbl.List.NumItems;
	i++)
     {
      if (Prg_GetLevelFromNumItem (i) == Level)
	 return true;	// Next brother found
      if (Prg_GetLevelFromNumItem (i) < Level)
	 return false;	// Next lower level found ==> there are no more brothers
     }
   return false;	// End reached ==> there are no more brothers
  }

/*****************************************************************************/
/******************* Check if item can be moved to the left ******************/
/*****************************************************************************/

static bool Prg_CheckIfMoveLeftIsAllowed (unsigned NumItem)
  {
   /***** Move left is allowed if the item has parent *****/
   return Prg_GetLevelFromNumItem (NumItem) > 1;
  }

/*****************************************************************************/
/****************** Check if item can be moved to the right ******************/
/*****************************************************************************/

static bool Prg_CheckIfMoveRightIsAllowed (unsigned NumItem)
  {
   /***** If item is the first, move right is not allowed *****/
   if (NumItem == 0)
      return false;

   /***** Move right is allowed if the item has brothers before it *****/
   // NumItem >= 2
   return Prg_GetLevelFromNumItem (NumItem - 1) >=
	  Prg_GetLevelFromNumItem (NumItem    );
  }

/*****************************************************************************/
/******************** Params used to edit a program item *********************/
/*****************************************************************************/

void Prg_PutParItmCod (void *ItmCod)
  {
   if (ItmCod)
      ParCod_PutPar (ParCod_Itm,*((long *) ItmCod));
  }

void Prg_GetPars (struct Prg_Item *Item)
  {
   /***** Try to get item resource *****/
   Item->Resource.Hierarchy.RscCod = ParCod_GetPar (ParCod_Rsc);

   /***** Get data of the program item from database *****/
   PrgRsc_GetResourceDataByCod (Item);

   if (Item->Hierarchy.ItmCod <= 0)	// No resource specified
      /***** Try to get data of the program item from database *****/
      Item->Hierarchy.ItmCod = ParCod_GetPar (ParCod_Itm);

   /***** Get data of the program item from database *****/
   Prg_GetItemDataByCod (Item);
  }

/*****************************************************************************/
/************************** List all program items ***************************/
/*****************************************************************************/

void Prg_GetListItems (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumItem;

   if (Prg_Gbl.List.IsRead)
      Prg_FreeListItems ();

   /***** Get list of program items from database *****/
   if ((Prg_Gbl.List.NumItems = Prg_DB_GetListItems (&mysql_res))) // Items found...
     {
      /***** Create list of program items *****/
      if ((Prg_Gbl.List.Items = calloc (Prg_Gbl.List.NumItems,
				        sizeof (*Prg_Gbl.List.Items))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the program items codes *****/
      for (NumItem = 0;
	   NumItem < Prg_Gbl.List.NumItems;
	   NumItem++)
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get code of the program item (row[0]) */
         if ((Prg_Gbl.List.Items[NumItem].ItmCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongItemExit ();

         /* Get index of the program item (row[1])
            and level of the program item (row[2]) */
         Prg_Gbl.List.Items[NumItem].ItmInd = Str_ConvertStrToUnsigned (row[1]);
         Prg_Gbl.List.Items[NumItem].Level  = Str_ConvertStrToUnsigned (row[2]);

	 /* Get whether the program item is hidden or not (row[3]) */
	 Prg_Gbl.List.Items[NumItem].HiddenOrVisible = (row[3][0] == 'Y') ? HidVis_HIDDEN :
									    HidVis_VISIBLE;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Prg_Gbl.List.IsRead = true;
  }

/*****************************************************************************/
/****************** Get program item data using its code *********************/
/*****************************************************************************/

static void Prg_GetItemDataByCod (struct Prg_Item *Item)
  {
   MYSQL_RES *mysql_res;
   unsigned NumRows;

   if (Item->Hierarchy.ItmCod > 0)
     {
      /***** Build query *****/
      NumRows = Prg_DB_GetItemDataByCod (&mysql_res,Item->Hierarchy.ItmCod);

      /***** Get data of program item *****/
      Prg_GetItemDataFromRow (&mysql_res,Item,NumRows);
     }
   else
      /***** Clear all program item data *****/
      Prg_ResetItem (Item);
  }

/*****************************************************************************/
/************************* Get program item data *****************************/
/*****************************************************************************/

static void Prg_GetItemDataFromRow (MYSQL_RES **mysql_res,
                                    struct Prg_Item *Item,
                                    unsigned NumRows)
  {
   MYSQL_ROW row;

   /***** Get data of program item from database *****/
   if (NumRows) // Item found...
     {
      /* Get row */
      row = mysql_fetch_row (*mysql_res);
      /*
      ItmCod					row[0]
      ItmInd					row[1]
      Level					row[2]
      Hidden					row[3]
      UsrCod					row[4]
      UNIX_TIMESTAMP(StartTime)			row[5]
      UNIX_TIMESTAMP(EndTime)			row[6]
      NOW() BETWEEN StartTime AND EndTime	row[7]
      Title					row[8]
      */

      /* Get code of the program item (row[0]) */
      Item->Hierarchy.ItmCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get index of the program item (row[1])
         and level of the program item (row[2]) */
      Item->Hierarchy.ItmInd = Str_ConvertStrToUnsigned (row[1]);
      Item->Hierarchy.Level = Str_ConvertStrToUnsigned (row[2]);

      /* Get whether the program item is hidden or not (row[3]) */
      Item->Hierarchy.HiddenOrVisible = (row[3][0] == 'Y') ? HidVis_HIDDEN :
							     HidVis_VISIBLE;

      /* Get author of the program item (row[4]) */
      Item->UsrCod = Str_ConvertStrCodToLongCod (row[4]);

      /* Get start date (row[5] holds the start UTC time)
         and end date   (row[6] holds the end   UTC time) */
      Item->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[5]);
      Item->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[6]);

      /* Get whether the program item is open or closed (row(7)) */
      Item->Open = (row[7][0] == '1');

      /* Get the title of the program item (row[8]) */
      Str_Copy (Item->Title,row[8],sizeof (Item->Title) - 1);
     }
   else
      /***** Clear all program item data *****/
      Prg_ResetItem (Item);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Free list of program items *************************/
/*****************************************************************************/

void Prg_FreeListItems (void)
  {
   if (Prg_Gbl.List.IsRead && Prg_Gbl.List.Items)
     {
      /***** Free memory used by the list of program items *****/
      free (Prg_Gbl.List.Items);
      Prg_Gbl.List.Items = NULL;
      Prg_Gbl.List.NumItems = 0;
      Prg_Gbl.List.IsRead = false;
     }
  }

/*****************************************************************************/
/************************ Clear all program item data ************************/
/*****************************************************************************/

void Prg_ResetItem (struct Prg_Item *Item)
  {
   Item->Hierarchy.ItmCod = -1L;
   Item->Hierarchy.ItmInd = 0;
   Item->Hierarchy.Level  = 0;
   Item->Hierarchy.HiddenOrVisible = HidVis_VISIBLE;
   Item->UsrCod = -1L;
   Item->TimeUTC[Dat_STR_TIME] =
   Item->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Item->Open = false;
   Item->Title[0] = '\0';
   Prg_ResetResource (Item);
  }

void Prg_ResetResource (struct Prg_Item *Item)
  {
   Item->Resource.Hierarchy.RscCod = -1L;
   Item->Resource.Hierarchy.RscInd = 0;
   Item->Resource.Hierarchy.HiddenOrVisible = HidVis_VISIBLE;
   Item->Resource.Link.Type = Rsc_NONE;
   Item->Resource.Link.Cod  = -1L;
   Item->Resource.Title[0] = '\0';
   Item->Resource.Title[0] = '\0';
  }

/*****************************************************************************/
/**************** Get number of item in list from item code ******************/
/*****************************************************************************/

unsigned Prg_GetNumItemFromItmCod (long ItmCod)
  {
   unsigned NumItem;

   /***** List of items must be filled *****/
   if (!Prg_Gbl.List.IsRead || Prg_Gbl.List.Items == NULL)
      Err_WrongItemsListExit ();

   /***** Find item code in list *****/
   for (NumItem = 0;
	NumItem < Prg_Gbl.List.NumItems;
	NumItem++)
      if (Prg_GetItmCodFromNumItem (NumItem) == ItmCod)	// Found!
	 return NumItem;

   /***** Not found *****/
   Err_WrongItemExit ();
   return 0;	// Not reached
  }

/*****************************************************************************/
/******************** Get item code from number of item **********************/
/*****************************************************************************/

inline long Prg_GetItmCodFromNumItem (unsigned NumItem)
  {
   return Prg_Gbl.List.Items[NumItem].ItmCod;
  }

/*****************************************************************************/
/******************** Get item index from number of item *********************/
/*****************************************************************************/

inline unsigned Prg_GetItmIndFromNumItem (unsigned NumItem)
  {
   return Prg_Gbl.List.Items[NumItem].ItmInd;
  }

/*****************************************************************************/
/****************** Get level of item from number of item ********************/
/*****************************************************************************/

inline unsigned Prg_GetLevelFromNumItem (unsigned NumItem)
  {
   return Prg_Gbl.List.Items[NumItem].Level;
  }

/*****************************************************************************/
/************* Ask for confirmation of removing a program item ***************/
/*****************************************************************************/

void Prg_ReqRemItem (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_item_X;
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongItemExit ();

   /***** Show question and button to remove the program item *****/
   Ale_ShowAlertRemove (ActRemPrgItm,NULL,
                        Prg_PutParItmCod,&Item.Hierarchy.ItmCod,
			Txt_Do_you_really_want_to_remove_the_item_X,
                        Item.Title);

   /***** Show program items highlighting subtree *****/
   Prg_ShowAllItems (Prg_EDIT_ITEMS,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/******************* Remove a program item and its children ******************/
/*****************************************************************************/

void Prg_RemoveItem (void)
  {
   extern const char *Txt_Item_X_removed;
   struct Prg_Item Item;
   struct Prg_ItemRange ToRemove;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongItemExit ();

   /***** Indexes of items *****/
   Prg_SetItemRangeWithAllChildren (Prg_GetNumItemFromItmCod (Item.Hierarchy.ItmCod),
				    &ToRemove);

   /***** Remove program items *****/
   Prg_DB_RemoveItemRange (&ToRemove);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Item_X_removed,Item.Title);

   /***** Update list of program items *****/
   Prg_FreeListItems ();
   Prg_GetListItems ();

   /***** Show course program without highlighting any item *****/
   Prg_ShowAllItems (Prg_EDIT_ITEMS,-1L,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/***************************** Hide a program item ***************************/
/*****************************************************************************/

void Prg_HideItem (void)
  {
   Prg_HideOrUnhideItem (HidVis_HIDDEN);
  }

void Prg_UnhideItem (void)
  {
   Prg_HideOrUnhideItem (HidVis_VISIBLE);
  }

static void Prg_HideOrUnhideItem (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongItemExit ();

   /***** Hide/unhide program item *****/
   Prg_DB_HideOrUnhideItem (Item.Hierarchy.ItmCod,HiddenOrVisible);

   /***** Show program items highlighting subtree *****/
   Prg_ShowAllItems (Prg_EDIT_ITEMS,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/********** Move up/down position of a subtree in a course program ***********/
/*****************************************************************************/

void Prg_MoveUpItem (void)
  {
   Prg_MoveUpDownItem (Prg_MOVE_UP);
  }

void Prg_MoveDownItem (void)
  {
   Prg_MoveUpDownItem (Prg_MOVE_DOWN);
  }

static void Prg_MoveUpDownItem (Prg_MoveUpDown_t UpDown)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Prg_Item Item;
   unsigned NumItem;
   bool Success = false;
   static bool (*CheckIfAllowed[Prg_NUM_MOVEMENTS_UP_DOWN])(unsigned NumItem) =
     {
      [Prg_MOVE_UP  ] = Prg_CheckIfMoveUpIsAllowed,
      [Prg_MOVE_DOWN] = Prg_CheckIfMoveDownIsAllowed,
     };

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongItemExit ();

   /***** Move up/down item *****/
   NumItem = Prg_GetNumItemFromItmCod (Item.Hierarchy.ItmCod);
   if (CheckIfAllowed[UpDown] (NumItem))
     {
      /* Exchange subtrees */
      switch (UpDown)
        {
	 case Prg_MOVE_UP:
            Success = Prg_ExchangeItemRanges (Prg_GetPrevBrother (NumItem),NumItem);
            break;
	 case Prg_MOVE_DOWN:
            Success = Prg_ExchangeItemRanges (NumItem,Prg_GetNextBrother (NumItem));
            break;
        }
     }
   if (Success)
     {
      /* Update list of program items */
      Prg_FreeListItems ();
      Prg_GetListItems ();

      /* Show program items highlighting subtree */
      Prg_ShowAllItems (Prg_EDIT_ITEMS,Item.Hierarchy.ItmCod,-1L);
     }
   else
     {
      /* Show course program without highlighting any item */
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
      Prg_ShowAllItems (Prg_EDIT_ITEMS,-1L,-1L);
     }

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/**** Exchange the order of two consecutive subtrees in a course program *****/
/*****************************************************************************/
// Return true if success

static bool Prg_ExchangeItemRanges (int NumItemTop,int NumItemBottom)
  {
   struct Prg_ItemRange Top;
   struct Prg_ItemRange Bottom;
   unsigned DiffBegin;
   unsigned DiffEnd;

   if (NumItemTop    >= 0 &&
       NumItemBottom >= 0)
     {
      Prg_SetItemRangeWithAllChildren (NumItemTop   ,&Top   );
      Prg_SetItemRangeWithAllChildren (NumItemBottom,&Bottom);
      DiffBegin = Bottom.Begin - Top.Begin;
      DiffEnd   = Bottom.End   - Top.End;

      /***** Lock table to make the move atomic *****/
      Prg_DB_LockTableItems ();

      /***** Exchange indexes of items *****/
      // This implementation works with non continuous indexes
      /*
      Example:
      Top.Begin    =  5
		   = 10
      Top.End      = 17
      Bottom.Begin = 28
      Bottom.End   = 49

      DiffBegin = 28 -  5 = 23;
      DiffEnd   = 49 - 17 = 32;

                                Step 1            Step 2            Step 3          (Equivalent to)
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
              |ItmInd|ItmCod|   |ItmInd|ItmCod|   |ItmInd|ItmCod|   |ItmInd|ItmCod| |ItmInd|ItmCod|
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
Top.Begin:    |     5|   218|-->|--> -5|   218|-->|--> 37|   218|   |    37|   218| |     5|   221|
              |    10|   219|-->|-->-10|   219|-->|--> 42|   219|   |    42|   219| |    26|   222|
Top.End:      |    17|   220|-->|-->-17|   220|-->|--> 49|   220|   |    49|   220| |    37|   218|
Bottom.Begin: |    28|   221|-->|-->-28|   221|   |   -28|   221|-->|-->  5|   221| |    42|   219|
Bottom.End:   |    49|   222|-->|-->-49|   222|   |   -49|   222|-->|--> 26|   222| |    49|   220|
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
      */
      /* Step 1: Change all indexes involved to negative,
		 necessary to preserve unique index (CrsCod,ItmInd) */
      Prg_DB_UpdateIndexRange (  (long) 0            ,	// ItmInd=-ItmInd
                                 (long) Top.Begin    ,
                                 (long) Bottom.End   );	// All indexes in both parts

      /* Step 2: Increase top indexes */
      Prg_DB_UpdateIndexRange (  (long) DiffEnd      ,	// ItmInd=-ItmInd+DiffEnd
                               -((long) Top.End     ),
                               -((long) Top.Begin   ));	// All indexes in top part

      /* Step 3: Decrease bottom indexes */
      Prg_DB_UpdateIndexRange (-((long) DiffBegin   ),	// ItmInd=-ItmInd-DiffBegin
                               -((long) Bottom.End  ),
                               -((long) Bottom.Begin));	// All indexes in bottom part

      /***** Unlock table *****/
      DB_UnlockTables ();

      return true;	// Success
     }

   return false;	// No success
  }

/*****************************************************************************/
/******** Get previous brother item to a given item in current course ********/
/*****************************************************************************/
// Return -1 if no previous brother

static int Prg_GetPrevBrother (int NumItem)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if item is the first one, there is no previous brother *****/
   if (NumItem <= 0 ||
       NumItem >= (int) Prg_Gbl.List.NumItems)
      return -1;

   /***** Get previous brother before item *****/
   // 1 <= NumItem < Prg_Gbl.List.NumItems
   Level = Prg_GetLevelFromNumItem (NumItem);
   for (i  = NumItem - 1;
	i >= 0;
	i--)
     {
      if (Prg_GetLevelFromNumItem (i) == Level)
	 return i;	// Previous brother before item found
      if (Prg_GetLevelFromNumItem (i) < Level)
	 return -1;	// Previous lower level found ==> there are no brothers before item
     }
   return -1;	// Start reached ==> there are no brothers before item
  }

/*****************************************************************************/
/********** Get next brother item to a given item in current course **********/
/*****************************************************************************/
// Return -1 if no next brother

static int Prg_GetNextBrother (int NumItem)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if item is the last one, there is no next brother *****/
   if (NumItem < 0 ||
       NumItem >= (int) Prg_Gbl.List.NumItems - 1)
      return -1;

   /***** Get next brother after item *****/
   // 0 <= NumItem < Prg_Gbl.List.NumItems - 1
   Level = Prg_GetLevelFromNumItem (NumItem);
   for (i = NumItem + 1;
	i < (int) Prg_Gbl.List.NumItems;
	i++)
     {
      if (Prg_GetLevelFromNumItem (i) == Level)
	 return i;	// Next brother found
      if (Prg_GetLevelFromNumItem (i) < Level)
	 return -1;	// Next lower level found ==> there are no brothers after item
     }
   return -1;	// End reached ==> there are no brothers after item
  }

/*****************************************************************************/
/************** Move a subtree to left/right in a course program *************/
/*****************************************************************************/

void Prg_MoveLeftItem (void)
  {
   Prg_MoveLeftRightItem (Prg_MOVE_LEFT);
  }

void Prg_MoveRightItem (void)
  {
   Prg_MoveLeftRightItem (Prg_MOVE_RIGHT);
  }

static void Prg_MoveLeftRightItem (Prg_MoveLeftRight_t LeftRight)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Prg_Item Item;
   unsigned NumItem;
   struct Prg_ItemRange ToMove;
   static bool (*CheckIfAllowed[Prg_NUM_MOVEMENTS_LEFT_RIGHT])(unsigned NumItem) =
     {
      [Prg_MOVE_LEFT ] = Prg_CheckIfMoveLeftIsAllowed,
      [Prg_MOVE_RIGHT] = Prg_CheckIfMoveRightIsAllowed,
     };

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongItemExit ();

   /***** Move up/down item *****/
   NumItem = Prg_GetNumItemFromItmCod (Item.Hierarchy.ItmCod);
   if (CheckIfAllowed[LeftRight](NumItem))
     {
      /* Indexes of items */
      Prg_SetItemRangeWithAllChildren (NumItem,&ToMove);

      /* Move item and its children to left or right */
      Prg_DB_MoveLeftRightItemRange (&ToMove,LeftRight);

      /* Update list of program items */
      Prg_FreeListItems ();
      Prg_GetListItems ();

      /* Show program items highlighting subtree */
      Prg_ShowAllItems (Prg_EDIT_ITEMS,Item.Hierarchy.ItmCod,-1L);
     }
   else
     {
      /* Show course program without highlighting any item */
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
      Prg_ShowAllItems (Prg_EDIT_ITEMS,-1L,-1L);
     }

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/************** Move a subtree to left/right in a course program *************/
/*****************************************************************************/

void Prg_ExpandItem (void)
  {
   Prg_ExpandContractItem (Prg_EXPAND);
  }

void Prg_ContractItem (void)
  {
   Prg_ExpandContractItem (Prg_CONTRACT);
  }

static void Prg_ExpandContractItem (Prg_ExpandContract_t ExpandContract)
  {
   struct Prg_Item Item;
   Prg_ListingType_t ListingType;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongItemExit ();

   /***** Add/remove item to/from table of expanded items *****/
   switch (ExpandContract)
     {
      case Prg_EXPAND:
	 Prg_DB_InsertItemInExpandedItems (Item.Hierarchy.ItmCod);
	 break;
      case Prg_CONTRACT:
	 Prg_DB_RemoveItemFromExpandedItems (Item.Hierarchy.ItmCod);
	 break;
     }

   /***** Show program items highlighting subtree *****/
   switch (Gbl.Action.Act)
     {
      case ActExpEdiPrgItm:
      case ActConEdiPrgItm:
	 ListingType = Prg_EDIT_ITEMS;
	 break;
      case ActExpSeePrgItm:
      case ActConSeePrgItm:
      default:
	 ListingType = Prg_VIEW;
	 break;
     }
   Prg_ShowAllItems (ListingType,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/****** Set subtree begin and end from number of item in course program ******/
/*****************************************************************************/

static void Prg_SetItemRangeOnlyItem (unsigned NumItem,struct Prg_ItemRange *ItemRange)
  {
   /***** List of items must be filled *****/
   if (!Prg_Gbl.List.IsRead)
      Err_WrongItemsListExit ();

   /***** Number of item must be in the correct range *****/
   if (NumItem >= Prg_Gbl.List.NumItems)
      Err_WrongItemExit ();

   /***** Range includes only this item *****/
   ItemRange->Begin =
   ItemRange->End   = Prg_GetItmIndFromNumItem (NumItem);
  }

static void Prg_SetItemRangeWithAllChildren (unsigned NumItem,struct Prg_ItemRange *ItemRange)
  {
   /***** List of items must be filled *****/
   if (!Prg_Gbl.List.IsRead)
      Err_WrongItemsListExit ();

   /***** Number of item must be in the correct range *****/
   if (NumItem >= Prg_Gbl.List.NumItems)
      Err_WrongItemExit ();

   /***** Range includes this item and all its children *****/
   ItemRange->Begin = Prg_GetItmIndFromNumItem (NumItem);
   ItemRange->End   = Prg_GetItmIndFromNumItem (Prg_GetLastChild (NumItem));
  }

/*****************************************************************************/
/********************** Get last child in current course *********************/
/*****************************************************************************/

static unsigned Prg_GetLastChild (int NumItem)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if item is wrong, there are no children *****/
   if (NumItem < 0 ||
       NumItem >= (int) Prg_Gbl.List.NumItems)
      Err_WrongItemExit ();

   /***** Get next brother after item *****/
   // 0 <= NumItem < Prg_Gbl.List.NumItems
   Level = Prg_GetLevelFromNumItem (NumItem);
   for (i = NumItem + 1;
	i < (int) Prg_Gbl.List.NumItems;
	i++)
     {
      if (Prg_GetLevelFromNumItem (i) <= Level)
	 return i - 1;	// Last child found
     }
   return Prg_Gbl.List.NumItems - 1;	// End reached ==> all items after the given item are its children
  }

/*****************************************************************************/
/******** List program items when click on view an item after edition ********/
/*****************************************************************************/

void Prg_ViewItemAfterEdit (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_END_EDIT_ITEM,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/*********** List program items with a form to change a given item ***********/
/*****************************************************************************/

void Prg_ReqChangeItem (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_FORM_EDIT_ITEM,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/************ List program items with a form to create a new item ************/
/*****************************************************************************/

void Prg_ReqCreateItem (void)
  {
   struct Prg_Item Item;

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);

   /***** Add item to table of expanded items
          to ensure that child items are displayed *****/
   Prg_DB_InsertItemInExpandedItems (Item.Hierarchy.ItmCod);

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Item.Hierarchy.ItmCod > 0 ? Prg_FORM_NEW_CHILD_ITEM :
	                                         Prg_FORM_NEW_END_ITEM,
	             Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/***************** Put a form to create a new program item *******************/
/*****************************************************************************/

static void Prg_ShowFormToCreateItem (long ParentItmCod)
  {
   struct Prg_Item ParentItem;	// Parent item
   struct Prg_Item Item;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_TO_000000,
      [Dat_END_TIME] = Dat_HMS_TO_235959
     };

   /***** Get data of the parent program item from database *****/
   ParentItem.Hierarchy.ItmCod = ParentItmCod;
   Prg_GetItemDataByCod (&ParentItem);

   /***** Initialize to empty program item *****/
   Prg_ResetItem (&Item);
   Item.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   Item.TimeUTC[Dat_END_TIME] = Item.TimeUTC[Dat_STR_TIME] + (2 * 60 * 60);	// +2 hours
   Item.Open = true;

   /***** Show pending alerts */
   Ale_ShowAlerts (NULL);

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewPrgItm,NULL,
                       Prg_ParsFormItem,&ParentItem.Hierarchy.ItmCod);

      /***** Show form *****/
      Prg_ShowFormItem (&Item,SetHMS,NULL);

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
  }

static void Prg_ShowFormToChangeItem (long ItmCod)
  {
   struct Prg_Item Item;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };

   /***** Get data of the program item from database *****/
   Item.Hierarchy.ItmCod = ItmCod;
   Prg_GetItemDataByCod (&Item);
   Prg_DB_GetItemTxt (Item.Hierarchy.ItmCod,Txt);

   /***** Begin form to change *****/
   Frm_BeginFormTable (ActChgPrgItm,Prg_HIGHLIGHTED_SECTION_ID,
                       Prg_ParsFormItem,&Item.Hierarchy.ItmCod);

      /***** Show form *****/
      Prg_ShowFormItem (&Item,SetHMS,Txt);

   /***** End form to change *****/
   Frm_EndFormTable (Btn_CONFIRM_BUTTON);
  }

static void Prg_ParsFormItem (void *ItmCod)
  {
   ParCod_PutPar (ParCod_Itm,*(long *) ItmCod);
  }

/*****************************************************************************/
/***************** Put a form to create a new program item *******************/
/*****************************************************************************/

static void Prg_ShowFormItem (const struct Prg_Item *Item,
			      const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME],
		              const char *Txt)
  {
   extern const char *Txt_Title;
   extern const char *Txt_Description;

   /***** Item title *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RM","Title",Txt_Title);

      /* Data */
      HTM_TD_Begin ("class=\"LM\"");
	 HTM_INPUT_TEXT ("Title",Prg_MAX_CHARS_PROGRAM_ITEM_TITLE,Item->Title,
			 HTM_DONT_SUBMIT_ON_CHANGE,
			 "id=\"Title\""
			 " class=\"PRG_TITLE_DESCRIPTION_WIDTH INPUT_%s\""
			 " required=\"required\"",
			 The_GetSuffix ());
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Program item start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Item->TimeUTC,
					    Dat_FORM_SECONDS_ON,
					    SetHMS);

   /***** Program item text *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT","Txt",Txt_Description);

      /* Data */
      HTM_TD_Begin ("class=\"LT\"");
	 HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"10\""
			     " class=\"PRG_TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
			     The_GetSuffix ());
	    if (Txt)
	       if (Txt[0])
		  HTM_Txt (Txt);
	 HTM_TEXTAREA_End ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************* Receive form to change an existing program item ***************/
/*****************************************************************************/

void Prg_ReceiveFormChgItem (void)
  {
   struct Prg_Item Item;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   if (Item.Hierarchy.ItmCod <= 0)
      Err_WrongItemExit ();

   /***** Get start/end date-times *****/
   Item.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   Item.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /***** Get program item title *****/
   Par_GetParText ("Title",Item.Title,Prg_MAX_BYTES_PROGRAM_ITEM_TITLE);

   /***** Get program item text *****/
   Par_GetParHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (Item.TimeUTC[Dat_STR_TIME] == 0)
      Item.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   if (Item.TimeUTC[Dat_END_TIME] == 0)
      Item.TimeUTC[Dat_END_TIME] = Item.TimeUTC[Dat_STR_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Update existing item *****/
   Prg_DB_UpdateItem (&Item,Description);

   /***** Show program items highlighting the item just changed *****/
   Prg_ShowAllItems (Prg_RECEIVE_ITEM,Item.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/***************** Receive form to create a new program item *****************/
/*****************************************************************************/

void Prg_ReceiveFormNewItem (void)
  {
   struct Prg_Item Item;		// Parent item
   struct Prg_Item NewItem;		// Item data received from form
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get list of program items *****/
   Prg_GetListItems ();

   /***** Get program item *****/
   Prg_GetPars (&Item);
   // If item code <= 0 ==> this is the first item in the program

   /***** Set new item code *****/
   NewItem.Hierarchy.ItmCod = -1L;
   NewItem.Hierarchy.Level = Item.Hierarchy.Level + 1;	// Create as child

   /***** Get start/end date-times *****/
   NewItem.TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
   NewItem.TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

   /***** Get program item title *****/
   Par_GetParText ("Title",NewItem.Title,Prg_MAX_BYTES_PROGRAM_ITEM_TITLE);

   /***** Get program item text *****/
   Par_GetParHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewItem.TimeUTC[Dat_STR_TIME] == 0)
      NewItem.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   if (NewItem.TimeUTC[Dat_END_TIME] == 0)
      NewItem.TimeUTC[Dat_END_TIME] = NewItem.TimeUTC[Dat_STR_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Create a new program item *****/
   Prg_InsertItem (&Item,&NewItem,Description);

   /* Update list of program items */
   Prg_FreeListItems ();
   Prg_GetListItems ();

   /***** Show program items highlighting the item just created *****/
   Prg_ShowAllItems (Prg_EDIT_ITEMS,NewItem.Hierarchy.ItmCod,-1L);

   /***** Free list of program items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/*********** Insert a new program item as a child of a parent item ***********/
/*****************************************************************************/

static void Prg_InsertItem (const struct Prg_Item *ParentItem,
		            struct Prg_Item *Item,const char *Txt)
  {
   unsigned NumItemLastChild;

   /***** Lock table to create program item *****/
   Prg_DB_LockTableItems ();

   /***** Get list of program items *****/
   Prg_GetListItems ();
   if (Prg_Gbl.List.NumItems)	// There are items
     {
      if (ParentItem->Hierarchy.ItmCod > 0)	// Parent specified
	{
	 /***** Calculate where to insert *****/
	 NumItemLastChild = Prg_GetLastChild (Prg_GetNumItemFromItmCod (ParentItem->Hierarchy.ItmCod));
	 if (NumItemLastChild < Prg_Gbl.List.NumItems - 1)
	   {
	    /***** New program item will be inserted after last child of parent *****/
	    Item->Hierarchy.ItmInd = Prg_GetItmIndFromNumItem (NumItemLastChild + 1);

	    /***** Move down all indexes of after last child of parent *****/
	    Prg_DB_MoveDownItems (Item->Hierarchy.ItmInd);
	   }
	 else
	    /***** New program item will be inserted at the end *****/
	    Item->Hierarchy.ItmInd = Prg_GetItmIndFromNumItem (Prg_Gbl.List.NumItems - 1) + 1;

	 /***** Child ==> parent level + 1 *****/
         Item->Hierarchy.Level = ParentItem->Hierarchy.Level + 1;
	}
      else	// No parent specified
	{
	 /***** New program item will be inserted at the end *****/
	 Item->Hierarchy.ItmInd = Prg_GetItmIndFromNumItem (Prg_Gbl.List.NumItems - 1) + 1;

	 /***** First level *****/
         Item->Hierarchy.Level = 1;
	}
     }
   else		// There are no items
     {
      /***** New program item will be inserted as the first one *****/
      Item->Hierarchy.ItmInd = 1;

      /***** First level *****/
      Item->Hierarchy.Level = 1;
     }

   /***** Insert new program item *****/
   Item->Hierarchy.ItmCod = Prg_DB_InsertItem (Item,Txt);

   /***** Unlock table *****/
   DB_UnlockTables ();

   /***** Free list items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/********************** Show stats about schedule items **********************/
/*****************************************************************************/

void Prg_GetAndShowCourseProgramStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_course_programs;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_program_items;
   extern const char *Txt_Number_of_BR_courses_with_BR_program_items;
   extern const char *Txt_Average_number_BR_of_items_BR_per_course;
   unsigned NumItems;
   unsigned NumCoursesWithItems = 0;
   double NumItemsPerCourse = 0.0;

   /***** Get the number of program items from this location *****/
   if ((NumItems = Prg_DB_GetNumItems (Gbl.Scope.Current)))
      if ((NumCoursesWithItems = Prg_DB_GetNumCoursesWithItems (Gbl.Scope.Current)) != 0)
         NumItemsPerCourse = (double) NumItems /
	                     (double) NumCoursesWithItems;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_COURSE_PROGRAMS],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_course_programs,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_program_items                ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_program_items,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_items_BR_per_course  ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of assignments *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumItems);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (NumCoursesWithItems);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	    HTM_Double2Decimals (NumItemsPerCourse);
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
