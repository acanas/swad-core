// swad_program.c: course program

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_program.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_string.h"

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

typedef enum
  {
   Prg_DONT_PUT_FORM_ITEM,
   Prg_PUT_FORM_CREATE_ITEM,
   Prg_PUT_FORM_CHANGE_ITEM,
  } Prg_CreateOrChangeItem_t;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static long Prg_CurrentItmCod;		// Used as parameter in contextual links

static unsigned Prg_MaxLevel;		// Maximum level of items
static unsigned *Prg_NumItem = NULL;	// Numbers for each level from 1 to maximum level

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prg_ShowCourseProgramHighlightingItem (long HighlightItmCod);
static void Prg_ShowAllItems (Prg_CreateOrChangeItem_t CreateOrChangeItem,
			      long HighlightItmCod,
			      long ParentItmCod,long ItmCodBeforeForm,unsigned FormLevel);
static bool Prg_CheckIfICanCreateItems (void);
static void Prg_PutIconsListItems (void);
static void Prg_PutIconToCreateNewItem (void);
static void Prg_PutButtonToCreateNewItem (void);
static void Prg_ShowOneItem (unsigned NumItem,const struct ProgramItem *Item,
			     long HighlightItmCod,bool PrintView);
static void Prg_ShowItemForm (Prg_CreateOrChangeItem_t CreateOrChangeItem,
			      long ParamItmCod,unsigned FormLevel);

static void Prg_CreateNumbers (unsigned MaxLevel);
static void Prg_FreeNumbers (void);
static void Prg_IncreaseNumItem (unsigned Level);
static unsigned Prg_GetNumItem (unsigned Level);
static void Prg_WriteNumItem (unsigned Level);
static void Prg_WriteNumNewItem (unsigned Level);

static void Prg_PutFormsToRemEditOnePrgItem (unsigned NumItem,
					     const struct ProgramItem *Item,
					     const char *Anchor);
static bool Prg_CheckIfMoveUpIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveDownIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveLeftIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveRightIsAllowed (unsigned NumItem);

static void Prg_SetCurrentItmCod (long ItmCod);
static long Prg_GetCurrentItmCod (void);
static void Prg_PutParams (void);

static void Prg_GetListPrgItems (void);
static void Prg_GetDataOfItemByCod (struct ProgramItem *Item);
static void Prg_GetDataOfItem (struct ProgramItem *Item,
                               MYSQL_RES **mysql_res,
			       unsigned long NumRows);
static void Prg_ResetItem (struct ProgramItem *Item);
static void Prg_FreeListItems (void);
static void Prg_GetPrgItemTxtFromDB (long ItmCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);
static void Prg_PutParamItmCod (long ItmCod);
static long Prg_GetParamItmCod (void);

static unsigned Prg_GetNumItemFromItmCod (unsigned ItmCod);

static unsigned Prg_GetMaxItemLevel (void);
static int Prg_GetPrevBrother (int NumItem);
static int Prg_GetNextBrother (int NumItem);
static unsigned Prg_GetLastChild (int NumItem);

static void Prg_ExchangeItems (int NumItemTop,int NumItemBottom);
static void Prg_MoveItemAndChildrenLeft (unsigned NumItem);
static void Prg_MoveItemAndChildrenRight (unsigned NumItem);

static bool Prg_CheckIfSimilarItemExists (const char *Field,const char *Value,long ItmCod);
static void Prg_ShowFormToCreateItem (long ParentItmCod);
static void Prg_ShowFormToChangeItem (long ItmCod);
static void Prg_ShowFormItem (const struct ProgramItem *Item,
			      const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME],
			      const char *Txt);
static void Prg_InsertItem (const struct ProgramItem *ParentItem,
		            struct ProgramItem *Item,const char *Txt);
static long Prg_InsertItemIntoDB (struct ProgramItem *Item,const char *Txt);
static void Prg_UpdateItem (struct ProgramItem *Item,const char *Txt);

/*****************************************************************************/
/************************ List all the program items *************************/
/*****************************************************************************/

void Prg_ShowCourseProgram (void)
  {
   Prg_ShowCourseProgramHighlightingItem (-1L);
  }

static void Prg_ShowCourseProgramHighlightingItem (long HighlightItmCod)
  {
   /***** Show all the program items *****/
   Prg_ShowAllItems (Prg_DONT_PUT_FORM_ITEM,
		     HighlightItmCod,-1L,-1L,0);
  }

/*****************************************************************************/
/*********************** Show all the program items **************************/
/*****************************************************************************/

static void Prg_ShowAllItems (Prg_CreateOrChangeItem_t CreateOrChangeItem,
			      long HighlightItmCod,
			      long ParentItmCod,long ItmCodBeforeForm,unsigned FormLevel)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Course_program;
   extern const char *Txt_No_items;
   unsigned NumItem;
   struct ProgramItem Item;

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   Prg_MaxLevel = Prg_GetMaxItemLevel ();
   Prg_CreateNumbers (Prg_MaxLevel);

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Course_program,Prg_PutIconsListItems,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

   if (Gbl.Prg.Num)
     {
      /***** Table head *****/
      HTM_TABLE_BeginWideMarginPadding (2);

      /***** Write all the program items *****/
      for (NumItem = 0;
	   NumItem < Gbl.Prg.Num;
	   NumItem++)
	{
	 /* Get data of this program item */
	 Item.Hierarchy.ItmCod = Gbl.Prg.LstItems[NumItem].ItmCod;
	 Prg_GetDataOfItemByCod (&Item);

	 /* Show item */
	 Prg_ShowOneItem (NumItem,&Item,
			  HighlightItmCod,
	                  false);	// Not print view

	 /* Show form to create/change item */
	 if (ItmCodBeforeForm == Item.Hierarchy.ItmCod)
	    switch (CreateOrChangeItem)
	      {
	       case Prg_DONT_PUT_FORM_ITEM:
		  break;
	       case Prg_PUT_FORM_CREATE_ITEM:
	          Prg_ShowItemForm (Prg_PUT_FORM_CREATE_ITEM,ParentItmCod,FormLevel);
		  break;
	       case Prg_PUT_FORM_CHANGE_ITEM:
	          Prg_ShowItemForm (Prg_PUT_FORM_CHANGE_ITEM,ItmCodBeforeForm,FormLevel);
	          break;
	      }

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
	}

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No program items created
      Ale_ShowAlert (Ale_INFO,Txt_No_items);

   /***** Button to create a new program item *****/
   if (Prg_CheckIfICanCreateItems ())
      Prg_PutButtonToCreateNewItem ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of program items *****/
   Prg_FreeNumbers ();
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/******************* Check if I can create program items *********************/
/*****************************************************************************/

static bool Prg_CheckIfICanCreateItems (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
                  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
  }

/*****************************************************************************/
/************** Put contextual icons in list of program items ****************/
/*****************************************************************************/

static void Prg_PutIconsListItems (void)
  {
   /***** Put icon to create a new program item *****/
   if (Prg_CheckIfICanCreateItems ())
      Prg_PutIconToCreateNewItem ();

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_COURSE_PROGRAMS;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/****************** Put icon to create a new program item ********************/
/*****************************************************************************/

static void Prg_PutIconToCreateNewItem (void)
  {
   extern const char *Txt_New_item;

   /***** Put form to create a new program item *****/
   Prg_SetCurrentItmCod (-1L);
   Ico_PutContextualIconToAdd (ActFrmNewPrgItm,NULL,Prg_PutParams,
			       Txt_New_item);
  }

/*****************************************************************************/
/***************** Put button to create a new program item *******************/
/*****************************************************************************/

static void Prg_PutButtonToCreateNewItem (void)
  {
   extern const char *Txt_New_item;

   Prg_SetCurrentItmCod (-1L);
   Frm_StartForm (ActFrmNewPrgItm);
   Prg_PutParams ();
   Btn_PutConfirmButton (Txt_New_item);
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************** Show one program item ****************************/
/*****************************************************************************/

#define Prg_WIDTH_NUM_ITEM 20

static void Prg_ShowOneItem (unsigned NumItem,const struct ProgramItem *Item,
			     long HighlightItmCod,bool PrintView)
  {
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   unsigned ColSpan;
   unsigned NumCol;
   Dat_StartEndTime_t StartEndTime;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Increase number of item *****/
   Prg_IncreaseNumItem (Item->Hierarchy.Level);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Item->Hierarchy.ItmCod,&Anchor);

   /***** Start row *****/
   if (Item->Hierarchy.ItmCod == HighlightItmCod)
      HTM_TR_Begin ("class=\"PRG_HIGHLIGHTED_ITEM\"");
   else
      HTM_TR_Begin (NULL);

   /***** Forms to remove/edit this program item *****/
   if (!PrintView)
     {
      HTM_TD_Begin ("class=\"PRG_COL1 LT COLOR%u\"",Gbl.RowEvenOdd);
      Prg_PutFormsToRemEditOnePrgItem (NumItem,Item,Anchor);
      HTM_TD_End ();
     }

   /***** Indent depending on the level *****/
   for (NumCol = 1;
	NumCol < Item->Hierarchy.Level;
	NumCol++)
     {
      HTM_TD_Begin ("class=\"COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();
     }

   /***** Item number *****/
   HTM_TD_Begin ("class=\"%s RT COLOR%u\" style=\"width:%dpx;\"",
		 Item->Hierarchy.Hidden ? "ASG_TITLE_LIGHT" :
			                  "ASG_TITLE",
		 Gbl.RowEvenOdd,
		 Item->Hierarchy.Level * Prg_WIDTH_NUM_ITEM);
   Prg_WriteNumItem (Item->Hierarchy.Level);
   HTM_TD_End ();

   /***** Title and text *****/
   /* Begin title and text */
   ColSpan = (Prg_MaxLevel + 2) - Item->Hierarchy.Level;
   if (PrintView)
      HTM_TD_Begin ("colspan=\"%u\" class=\"LT\"",
		    ColSpan);
   else
      HTM_TD_Begin ("colspan=\"%u\" class=\"LT COLOR%u\"",
		    ColSpan,Gbl.RowEvenOdd);
   if (Item->Hierarchy.ItmCod == HighlightItmCod)
      HTM_SECTION_Begin ("highlighted_item");
   HTM_ARTICLE_Begin (Anchor);

   /* Title */
   HTM_DIV_Begin ("class=\"%s\"",
		  Item->Hierarchy.Hidden ? "ASG_TITLE_LIGHT" :
				           "ASG_TITLE");
   HTM_Txt (Item->Title);
   HTM_DIV_End ();

   /* Text */
   Prg_GetPrgItemTxtFromDB (Item->Hierarchy.ItmCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   HTM_DIV_Begin ("class=\"PAR %s\"",Item->Hierarchy.Hidden ? "DAT_LIGHT" :
        	                                              "DAT");
   HTM_Txt (Txt);
   HTM_DIV_End ();

   /* End title and text */
   HTM_ARTICLE_End ();
   if (Item->Hierarchy.ItmCod == HighlightItmCod)
      HTM_SECTION_End ();
   HTM_TD_End ();

   /***** Start/end date/time *****/
   UniqueId++;

   for (StartEndTime  = (Dat_StartEndTime_t) 0;
	StartEndTime <= (Dat_StartEndTime_t) (Dat_NUM_START_END_TIME - 1);
	StartEndTime++)
     {
      if (asprintf (&Id,"scd_date_%u_%u",(unsigned) StartEndTime,UniqueId) < 0)
	 Lay_NotEnoughMemoryExit ();
      if (PrintView)
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT\"",
		       Id,
		       Item->Hierarchy.Hidden ? (Item->Open ? "DATE_GREEN_LIGHT" :
					                      "DATE_RED_LIGHT") :
				                (Item->Open ? "DATE_GREEN" :
					                      "DATE_RED"));
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		       Id,
		       Item->Hierarchy.Hidden ? (Item->Open ? "DATE_GREEN_LIGHT" :
					                      "DATE_RED_LIGHT") :
				                (Item->Open ? "DATE_GREEN" :
					                      "DATE_RED"),
		       Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,Item->TimeUTC[StartEndTime],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);
     }

   /***** End row *****/
   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);
  }

/*****************************************************************************/
/**************************** Show item form *********************************/
/*****************************************************************************/

static void Prg_ShowItemForm (Prg_CreateOrChangeItem_t CreateOrChangeItem,
			      long ParamItmCod,unsigned FormLevel)
  {
   unsigned ColSpan;
   unsigned NumCol;

   /***** Trivial check *****/
   if (CreateOrChangeItem == Prg_DONT_PUT_FORM_ITEM)
      return;

   /***** Change color row? *****/
   if (CreateOrChangeItem == Prg_PUT_FORM_CREATE_ITEM)
      Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Column under icons *****/
   HTM_TD_Begin ("class=\"PRG_COL1 LT COLOR%u\"",Gbl.RowEvenOdd);
   HTM_TD_End ();

   /***** Indent depending on the level *****/
   for (NumCol = 1;
	NumCol < FormLevel;
	NumCol++)
     {
      HTM_TD_Begin ("class=\"COLOR%u\"",Gbl.RowEvenOdd);
      HTM_TD_End ();
     }

   /***** Item number *****/
   HTM_TD_Begin ("class=\"ASG_TITLE RT COLOR%u\" style=\"width:%dpx;\"",
		 Gbl.RowEvenOdd,
		 FormLevel * Prg_WIDTH_NUM_ITEM);
   if (CreateOrChangeItem == Prg_PUT_FORM_CREATE_ITEM)
      Prg_WriteNumNewItem (FormLevel);
   else
      Prg_WriteNumItem (FormLevel);
   HTM_TD_End ();

   /***** Show form to create new item as child *****/
   ColSpan = (Prg_MaxLevel + 4) - FormLevel;
   HTM_TD_Begin ("colspan=\"%u\" class=\"LT COLOR%u\"",
		 ColSpan,Gbl.RowEvenOdd);
   HTM_ARTICLE_Begin ("item_form");
   if (CreateOrChangeItem == Prg_PUT_FORM_CREATE_ITEM)
      Prg_ShowFormToCreateItem (ParamItmCod);
   else
      Prg_ShowFormToChangeItem (ParamItmCod);
   HTM_ARTICLE_End ();
   HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/********************* Allocate memory for item numbers **********************/
/*****************************************************************************/

static void Prg_CreateNumbers (unsigned MaxLevel)
  {
   if (MaxLevel)
     {
      /***** Allocate memory for item numbers and initialize to 0 *****/
      /*
      Example:  2.5.2.1
                MaxLevel = 4
      Level Number
      ----- ------
        0         <--- Not used
        1     2
        2     5
        3     2
        4     1
        5     0	  <--- Used to create a new item
      */
      if ((Prg_NumItem = (unsigned *) calloc (1 + MaxLevel + 1,sizeof (unsigned))) == NULL)
	 Lay_NotEnoughMemoryExit ();
     }
   else
      Prg_NumItem = NULL;
  }

/*****************************************************************************/
/*********************** Free memory for item numbers ************************/
/*****************************************************************************/

static void Prg_FreeNumbers (void)
  {
   if (Prg_MaxLevel && Prg_NumItem)
     {
      /***** Free allocated memory for item numbers *****/
      free (Prg_NumItem);
      Prg_NumItem = NULL;
     }
  }

/*****************************************************************************/
/**************************** Increase number of item ************************/
/*****************************************************************************/

static void Prg_IncreaseNumItem (unsigned Level)
  {
   /***** Increase number for this level *****/
   Prg_NumItem[Level]++;

   /***** Reset number for next level (children) *****/
   Prg_NumItem[Level + 1] = 0;
  }

/*****************************************************************************/
/*********************** Get number of item in a level ***********************/
/*****************************************************************************/

static unsigned Prg_GetNumItem (unsigned Level)
  {
   if (Prg_NumItem)
      return Prg_NumItem[Level];

   return 0;
  }

/*****************************************************************************/
/******************** Write number of item in legal style ********************/
/*****************************************************************************/

static void Prg_WriteNumItem (unsigned Level)
  {
   unsigned i;

   /***** Write number in legal style *****/
   for (i  = 1;
	i <= Level;
	i++)
     {
      if (i > 1)
         HTM_Txt (".");
      HTM_Unsigned (Prg_GetNumItem (i));
     }
  }

static void Prg_WriteNumNewItem (unsigned Level)
  {
   unsigned i;

   /***** Write number in legal style *****/
   for (i  = 1;
	i <= Level;
	i++)
     {
      if (i > 1)
	 HTM_Txt (".");
      if (i < Level)
	 HTM_Unsigned (Prg_GetNumItem (i));
      else
	 HTM_Unsigned (Prg_GetNumItem (i) + 1);
     }
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void Prg_PutFormsToRemEditOnePrgItem (unsigned NumItem,
					     const struct ProgramItem *Item,
					     const char *Anchor)
  {
   extern const char *Txt_New_item;
   extern const char *Txt_Move_up_X;
   extern const char *Txt_Move_down_X;
   extern const char *Txt_Increase_level_of_X;
   extern const char *Txt_Decrease_level_of_X;
   extern const char *Txt_Movement_not_allowed;
   char StrItemIndex[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   Prg_SetCurrentItmCod (Item->Hierarchy.ItmCod);	// Used as parameter in contextual links

   /***** Initialize item index string *****/
   snprintf (StrItemIndex,sizeof (StrItemIndex),
	     "%u",
	     Item->Hierarchy.Index);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put form to remove program item *****/
	 Ico_PutContextualIconToRemove (ActReqRemPrgItm,Prg_PutParams);

	 /***** Put form to hide/show program item *****/
	 if (Item->Hierarchy.Hidden)
	    Ico_PutContextualIconToUnhide (ActShoPrgItm,"highlighted_item",Prg_PutParams);
	 else
	    Ico_PutContextualIconToHide (ActHidPrgItm,"highlighted_item",Prg_PutParams);

	 /***** Put form to edit program item *****/
	 Ico_PutContextualIconToEdit (ActFrmChgPrgItm,"item_form",Prg_PutParams);

	 /***** Put form to add a new child item inside this item *****/
	 Ico_PutContextualIconToAdd (ActFrmNewPrgItm,"item_form",Prg_PutParams,Txt_New_item);

	 HTM_BR ();

	 /***** Put icon to move up the item *****/
	 if (Prg_CheckIfMoveUpIsAllowed (NumItem))
	   {
	    Lay_PutContextualLinkOnlyIcon (ActUp_PrgItm,"highlighted_item",Prg_PutParams,
					   "arrow-up.svg",
					   Str_BuildStringStr (Txt_Move_up_X,
							       StrItemIndex));
	    Str_FreeString ();
	   }
	 else
	    Ico_PutIconOff ("arrow-up.svg",Txt_Movement_not_allowed);

	 /***** Put icon to move down the item *****/
	 if (Prg_CheckIfMoveDownIsAllowed (NumItem))
	   {
	    Lay_PutContextualLinkOnlyIcon (ActDwnPrgItm,"highlighted_item",Prg_PutParams,
					   "arrow-down.svg",
					   Str_BuildStringStr (Txt_Move_down_X,
							       StrItemIndex));
	    Str_FreeString ();
	   }
	 else
	    Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);

	 /***** Icon to move left item (increase level) *****/
	 if (Prg_CheckIfMoveLeftIsAllowed (NumItem))
	   {
	    Lay_PutContextualLinkOnlyIcon (ActLftPrgItm,"highlighted_item",Prg_PutParams,
					   "arrow-left.svg",
					   Str_BuildStringStr (Txt_Increase_level_of_X,
							       StrItemIndex));
	    Str_FreeString ();
	   }
	 else
            Ico_PutIconOff ("arrow-left.svg",Txt_Movement_not_allowed);

	 /***** Icon to move right item (indent, decrease level) *****/
	 if (Prg_CheckIfMoveRightIsAllowed (NumItem))
	   {
	    Lay_PutContextualLinkOnlyIcon (ActRgtPrgItm,"highlighted_item",Prg_PutParams,
					   "arrow-right.svg",
					   Str_BuildStringStr (Txt_Decrease_level_of_X,
							       StrItemIndex));
	    Str_FreeString ();
	   }
	 else
            Ico_PutIconOff ("arrow-right.svg",Txt_Movement_not_allowed);
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
   // NumItem >= 2
   return Gbl.Prg.LstItems[NumItem - 1].Level >=
	  Gbl.Prg.LstItems[NumItem    ].Level;
  }

/*****************************************************************************/
/********************** Check if item can be moved down **********************/
/*****************************************************************************/

static bool Prg_CheckIfMoveDownIsAllowed (unsigned NumItem)
  {
   unsigned i;
   unsigned Level;

   /***** Trivial check: if item is the last one, move up is not allowed *****/
   if (NumItem >= Gbl.Prg.Num - 1)
      return false;

   /***** Move down is allowed if the item has brothers after it *****/
   // NumItem + 1 < Gbl.Prg.Num
   Level = Gbl.Prg.LstItems[NumItem].Level;
   for (i = NumItem + 1;
	i < Gbl.Prg.Num;
	i++)
     {
      if (Gbl.Prg.LstItems[i].Level == Level)
	 return true;	// Next brother found
      if (Gbl.Prg.LstItems[i].Level < Level)
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
   return Gbl.Prg.LstItems[NumItem].Level > 1;
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
   return Gbl.Prg.LstItems[NumItem - 1].Level >=
	  Gbl.Prg.LstItems[NumItem    ].Level;
  }

/*****************************************************************************/
/**************** Access to variables used to pass parameter *****************/
/*****************************************************************************/

static void Prg_SetCurrentItmCod (long ItmCod)
  {
   Prg_CurrentItmCod = ItmCod;
  }

static long Prg_GetCurrentItmCod (void)
  {
   return Prg_CurrentItmCod;
  }

/*****************************************************************************/
/******************** Params used to edit a program item *********************/
/*****************************************************************************/

static void Prg_PutParams (void)
  {
   long CurrentItmCod = Prg_GetCurrentItmCod ();

   if (CurrentItmCod > 0)
      Prg_PutParamItmCod (CurrentItmCod);
  }

/*****************************************************************************/
/*********************** List all the program items **************************/
/*****************************************************************************/

static void Prg_GetListPrgItems (void)
  {
   static const char *HiddenSubQuery[Rol_NUM_ROLES] =
     {
      [Rol_UNK    ] = " AND Hidden='N'",
      [Rol_GST    ] = " AND Hidden='N'",
      [Rol_USR    ] = " AND Hidden='N'",
      [Rol_STD    ] = " AND Hidden='N'",
      [Rol_NET    ] = " AND Hidden='N'",
      [Rol_TCH    ] = "",
      [Rol_DEG_ADM] = " AND Hidden='N'",
      [Rol_CTR_ADM] = " AND Hidden='N'",
      [Rol_INS_ADM] = " AND Hidden='N'",
      [Rol_SYS_ADM] = "",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumItem;

   if (Gbl.Prg.LstIsRead)
      Prg_FreeListItems ();

   /***** Get list of program items from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get program items",
			     "SELECT ItmCod,"	// row[0]
				    "ItmInd,"	// row[1]
				    "Level,"		// row[2]
				    "Hidden"		// row[3]
			     " FROM prg_items"
			     " WHERE CrsCod=%ld%s"
			     " ORDER BY ItmInd",
			     Gbl.Hierarchy.Crs.CrsCod,
			     HiddenSubQuery[Gbl.Usrs.Me.Role.Logged]);

   if (NumRows) // Items found...
     {
      Gbl.Prg.Num = (unsigned) NumRows;

      /***** Create list of program items *****/
      if ((Gbl.Prg.LstItems =
	   (struct ProgramItemHierarchy *) calloc (NumRows,
						   sizeof (struct ProgramItemHierarchy))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the program items codes *****/
      for (NumItem = 0;
	   NumItem < Gbl.Prg.Num;
	   NumItem++)
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get code of the program item (row[0]) */
         if ((Gbl.Prg.LstItems[NumItem].ItmCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Error: wrong program item code.");

         /* Get index of the program item (row[1]) */
         Gbl.Prg.LstItems[NumItem].Index = Str_ConvertStrToUnsigned (row[1]);

         /* Get level of the program item (row[2]) */
         Gbl.Prg.LstItems[NumItem].Level = Str_ConvertStrToUnsigned (row[2]);

	 /* Get whether the program item is hidden or not (row[3]) */
	 Gbl.Prg.LstItems[NumItem].Hidden = (row[3][0] == 'Y');
        }
     }
   else
      Gbl.Prg.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.Prg.LstIsRead = true;
  }

/*****************************************************************************/
/****************** Get program item data using its code *********************/
/*****************************************************************************/

static void Prg_GetDataOfItemByCod (struct ProgramItem *Item)
  {
   MYSQL_RES *mysql_res;
   unsigned long NumRows;

   if (Item->Hierarchy.ItmCod > 0)
     {
      /***** Build query *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get program item data",
				"SELECT ItmCod,"				// row[0]
				       "ItmInd,"				// row[1]
				       "Level,"					// row[2]
				       "Hidden,"				// row[3]
				       "UsrCod,"				// row[4]
				       "UNIX_TIMESTAMP(StartTime),"		// row[5]
				       "UNIX_TIMESTAMP(EndTime),"		// row[6]
				       "NOW() BETWEEN StartTime AND EndTime,"	// row[7]
				       "Title"					// row[8]
				" FROM prg_items"
				" WHERE ItmCod=%ld"
				" AND CrsCod=%ld",	// Extra check
				Item->Hierarchy.ItmCod,Gbl.Hierarchy.Crs.CrsCod);

      /***** Get data of program item *****/
      Prg_GetDataOfItem (Item,&mysql_res,NumRows);
     }
   else
      /***** Clear all program item data *****/
      Prg_ResetItem (Item);
  }

/*****************************************************************************/
/************************* Get program item data *****************************/
/*****************************************************************************/

static void Prg_GetDataOfItem (struct ProgramItem *Item,
                               MYSQL_RES **mysql_res,
			       unsigned long NumRows)
  {
   MYSQL_ROW row;

   /***** Clear all program item data *****/
   Prg_ResetItem (Item);

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

      /* Get index of the program item (row[1]) */
      Item->Hierarchy.Index = Str_ConvertStrToUnsigned (row[1]);

      /* Get level of the program item (row[2]) */
      Item->Hierarchy.Level = Str_ConvertStrToUnsigned (row[2]);

      /* Get whether the program item is hidden or not (row[3]) */
      Item->Hierarchy.Hidden = (row[3][0] == 'Y');

      /* Get author of the program item (row[4]) */
      Item->UsrCod = Str_ConvertStrCodToLongCod (row[4]);

      /* Get start date (row[5] holds the start UTC time) */
      Item->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[5]);

      /* Get end date   (row[6] holds the end   UTC time) */
      Item->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[6]);

      /* Get whether the program item is open or closed (row(7)) */
      Item->Open = (row[7][0] == '1');

      /* Get the title of the program item (row[8]) */
      Str_Copy (Item->Title,row[8],
                Prg_MAX_BYTES_PROGRAM_ITEM_TITLE);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Clear all program item data ************************/
/*****************************************************************************/

static void Prg_ResetItem (struct ProgramItem *Item)
  {
   Item->Hierarchy.ItmCod = -1L;
   Item->Hierarchy.Index  = 0;
   Item->Hierarchy.Level  = 0;
   Item->Hierarchy.Hidden = false;
   Item->UsrCod = -1L;
   Item->TimeUTC[Dat_START_TIME] =
   Item->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Item->Open = false;
   Item->Title[0] = '\0';
  }

/*****************************************************************************/
/************************ Free list of program items *************************/
/*****************************************************************************/

static void Prg_FreeListItems (void)
  {
   if (Gbl.Prg.LstIsRead && Gbl.Prg.LstItems)
     {
      /***** Free memory used by the list of program items *****/
      free (Gbl.Prg.LstItems);
      Gbl.Prg.LstItems = NULL;
      Gbl.Prg.Num = 0;
      Gbl.Prg.LstIsRead = false;
     }
  }

/*****************************************************************************/
/******************* Get program item text from database *********************/
/*****************************************************************************/

static void Prg_GetPrgItemTxtFromDB (long ItmCod,char Txt[Cns_MAX_BYTES_TEXT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of program item from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get program item text",
	                     "SELECT Txt FROM prg_items"
			     " WHERE ItmCod=%ld"
			     " AND CrsCod=%ld",	// Extra check
			     ItmCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Txt,row[0],
                Cns_MAX_BYTES_TEXT);
     }
   else
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting program item text.");
  }

/*****************************************************************************/
/**************** Write parameter with code of program item ******************/
/*****************************************************************************/

static void Prg_PutParamItmCod (long ItmCod)
  {
   Par_PutHiddenParamLong (NULL,"ItmCod",ItmCod);
  }

/*****************************************************************************/
/***************** Get parameter with code of program item *******************/
/*****************************************************************************/

static long Prg_GetParamItmCod (void)
  {
   /***** Get code of program item *****/
   return Par_GetParToLong ("ItmCod");
  }

/*****************************************************************************/
/**************** Get number of item in list from item code ******************/
/*****************************************************************************/

static unsigned Prg_GetNumItemFromItmCod (unsigned ItmCod)
  {
   unsigned NumItem;

   for (NumItem = 0;
	NumItem < Gbl.Prg.Num;
	NumItem++)
      if (Gbl.Prg.LstItems[NumItem].ItmCod == ItmCod)
	 return NumItem;

   Lay_ShowErrorAndExit ("Wrong item code.");
   return 0;	// Not reached
  }

/*****************************************************************************/
/************* Ask for confirmation of removing a program item ***************/
/*****************************************************************************/

void Prg_ReqRemPrgItem (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_item_X;
   extern const char *Txt_Remove_item;
   struct ProgramItem Item;

   /***** Get program item code *****/
   if ((Item.Hierarchy.ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfItemByCod (&Item);

   /***** Show question and button to remove the program item *****/
   Prg_SetCurrentItmCod (Item.Hierarchy.ItmCod);
   Ale_ShowAlertAndButton (ActRemPrgItm,NULL,NULL,Prg_PutParams,
                           Btn_REMOVE_BUTTON,Txt_Remove_item,
			   Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_item_X,
                           Item.Title);

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (Item.Hierarchy.ItmCod);
  }

/*****************************************************************************/
/******************* Remove a program item and its children ******************/
/*****************************************************************************/

void Prg_RemovePrgItem (void)
  {
   extern const char *Txt_Item_X_removed;
   struct ProgramItem Item;
   unsigned NumItem;
   unsigned BeginIndex;
   unsigned EndIndex;

   /***** Get program item code *****/
   if ((Item.Hierarchy.ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfItemByCod (&Item);	// Inside this function, the course is checked to be the current one

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   NumItem = Prg_GetNumItemFromItmCod (Item.Hierarchy.ItmCod);

   /***** Indexes of items *****/
   BeginIndex = Gbl.Prg.LstItems[NumItem].Index;
   EndIndex   = Gbl.Prg.LstItems[Prg_GetLastChild (NumItem)].Index;

   /***** Remove program items *****/
   DB_QueryDELETE ("can not remove program item",
		   "DELETE FROM prg_items"
		   " WHERE CrsCod=%ld AND"
		   " ItmInd>=%u AND ItmInd<=%u",
                   Gbl.Hierarchy.Crs.CrsCod,
		   BeginIndex,EndIndex);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Item_X_removed,Item.Title);

   /***** Free list of program items *****/
   Prg_FreeListItems ();

   /***** Show program items again *****/
   Prg_ShowCourseProgram ();
  }

/*****************************************************************************/
/***************************** Hide a program item ***************************/
/*****************************************************************************/

void Prg_HidePrgItem (void)
  {
   struct ProgramItem Item;

   /***** Get program item code *****/
   if ((Item.Hierarchy.ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfItemByCod (&Item);

   /***** Hide program item *****/
   DB_QueryUPDATE ("can not hide program item",
		   "UPDATE prg_items SET Hidden='Y'"
		   " WHERE ItmCod=%ld"
		   " AND CrsCod=%ld",	// Extra check
                   Item.Hierarchy.ItmCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (Item.Hierarchy.ItmCod);
  }

/*****************************************************************************/
/***************************** Show a program item ***************************/
/*****************************************************************************/

void Prg_ShowPrgItem (void)
  {
   struct ProgramItem Item;

   /***** Get program item code *****/
   if ((Item.Hierarchy.ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfItemByCod (&Item);

   /***** Hide program item *****/
   DB_QueryUPDATE ("can not show program item",
		   "UPDATE prg_items SET Hidden='N'"
		   " WHERE ItmCod=%ld"
		   " AND CrsCod=%ld",	// Extra check
                   Item.Hierarchy.ItmCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (Item.Hierarchy.ItmCod);
  }

/*****************************************************************************/
/************** Move up position of an item in a course program **************/
/*****************************************************************************/

void Prg_MoveUpPrgItem (void)
  {
   extern const char *Txt_Movement_not_allowed;
   long ItmCod;
   unsigned NumItem;

   /***** Get program item code *****/
   if ((ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   NumItem = Prg_GetNumItemFromItmCod (ItmCod);

   if (Prg_CheckIfMoveUpIsAllowed (NumItem))
      /***** Move up item *****/
      /* Exchange subtrees */
      Prg_ExchangeItems (Prg_GetPrevBrother (NumItem),NumItem);
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Free list of program items *****/
   Prg_FreeListItems ();

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (ItmCod);
  }

/*****************************************************************************/
/************* Move down position of an item in a course program *************/
/*****************************************************************************/

void Prg_MoveDownPrgItem (void)
  {
   extern const char *Txt_Movement_not_allowed;
   long ItmCod;
   unsigned NumItem;

   /***** Get program item code *****/
   if ((ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   NumItem = Prg_GetNumItemFromItmCod (ItmCod);

   if (Prg_CheckIfMoveDownIsAllowed (NumItem))
      /***** Move down item *****/
      /* Exchange subtrees */
      Prg_ExchangeItems (NumItem,Prg_GetNextBrother (NumItem));
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Free list of program items *****/
   Prg_FreeListItems ();

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (ItmCod);
  }

/*****************************************************************************/
/**** Move left (decrease level) position of an item in a course program *****/
/*****************************************************************************/

void Prg_MoveLeftPrgItem (void)
  {
   long ItmCod;
   unsigned NumItem;

   /***** Get program item code *****/
   if ((ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   NumItem = Prg_GetNumItemFromItmCod (ItmCod);

   /***** Move left item (decrease level) *****/
   /* Move item and its children to left */
   Prg_MoveItemAndChildrenLeft (NumItem);

   /***** Free list of program items *****/
   Prg_FreeListItems ();

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (ItmCod);
  }

/*****************************************************************************/
/**** Move right (increase level) position of an item in a course program ****/
/*****************************************************************************/

void Prg_MoveRightPrgItem (void)
  {
   long ItmCod;
   unsigned NumItem;

   /***** Get program item code *****/
   if ((ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   NumItem = Prg_GetNumItemFromItmCod (ItmCod);

   /***** Move right item (increase level) *****/
   /* Move item and its children to right */
   Prg_MoveItemAndChildrenRight (NumItem);

   /***** Free list of program items *****/
   Prg_FreeListItems ();

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (ItmCod);
  }

/*****************************************************************************/
/******************* Get maximum level in a course program *******************/
/*****************************************************************************/
// Return 0 if no items

static unsigned Prg_GetMaxItemLevel (void)
  {
   unsigned NumItem;
   unsigned MaxLevel = 0;

   /***** Compute maximum level of all program items *****/
   for (NumItem = 0;
	NumItem < Gbl.Prg.Num;
	NumItem++)
      if (Gbl.Prg.LstItems[NumItem].Level > MaxLevel)
	 MaxLevel = Gbl.Prg.LstItems[NumItem].Level;

   return MaxLevel;
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
       NumItem >= (int) Gbl.Prg.Num)
      return -1;

   /***** Get previous brother before item *****/
   // 1 <= NumItem < Gbl.Prg.Num
   Level = Gbl.Prg.LstItems[NumItem].Level;
   for (i  = NumItem - 1;
	i >= 0;
	i--)
     {
      if (Gbl.Prg.LstItems[i].Level == Level)
	 return i;	// Previous brother before item found
      if (Gbl.Prg.LstItems[i].Level < Level)
	 return -1;		// Previous lower level found ==> there are no brothers before item
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
       NumItem >= (int) Gbl.Prg.Num - 1)
      return -1;

   /***** Get next brother after item *****/
   // 0 <= NumItem < Gbl.Prg.Num - 1
   Level = Gbl.Prg.LstItems[NumItem].Level;
   for (i = NumItem + 1;
	i < (int) Gbl.Prg.Num;
	i++)
     {
      if (Gbl.Prg.LstItems[i].Level == Level)
	 return i;	// Next brother found
      if (Gbl.Prg.LstItems[i].Level < Level)
	 return -1;	// Next lower level found ==> there are no brothers after item
     }
   return -1;	// End reached ==> there are no brothers after item
  }

/*****************************************************************************/
/********************** Get last child in current course *********************/
/*****************************************************************************/
// Return the same index if no children

static unsigned Prg_GetLastChild (int NumItem)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if item is wrong, there are no children *****/
   if (NumItem < 0 ||
       NumItem >= (int) Gbl.Prg.Num)
      Lay_ShowErrorAndExit ("Wrong number of item.");

   /***** Get next brother after item *****/
   // 0 <= NumItem < Gbl.Prg.Num
   Level = Gbl.Prg.LstItems[NumItem].Level;
   for (i = NumItem + 1;
	i < (int) Gbl.Prg.Num;
	i++)
     {
      if (Gbl.Prg.LstItems[i].Level <= Level)
	 return i - 1;	// Last child found
     }
   return Gbl.Prg.Num - 1;	// End reached ==> all items after the given item are its children
  }

/*****************************************************************************/
/**************** Move item and its children to left or right ****************/
/*****************************************************************************/

static void Prg_MoveItemAndChildrenLeft (unsigned NumItem)
  {
   extern const char *Txt_Movement_not_allowed;
   unsigned BeginIndex;
   unsigned EndIndex;

   if (Prg_CheckIfMoveLeftIsAllowed (NumItem))
     {
      /* Indexes of items */
      BeginIndex = Gbl.Prg.LstItems[NumItem].Index;
      EndIndex   = Gbl.Prg.LstItems[Prg_GetLastChild (NumItem)].Index;

      /* Move item and its children to left or right */
      DB_QueryUPDATE ("can not move items",
		      "UPDATE prg_items SET Level=Level-1"
		      " WHERE CrsCod=%ld"
		      " AND ItmInd>=%u AND ItmInd<=%u",
		      Gbl.Hierarchy.Crs.CrsCod,
		      BeginIndex,EndIndex);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
  }

/*****************************************************************************/
/**************** Move item and its children to left or right ****************/
/*****************************************************************************/

static void Prg_MoveItemAndChildrenRight (unsigned NumItem)
  {
   extern const char *Txt_Movement_not_allowed;
   unsigned BeginIndex;
   unsigned EndIndex;

   if (Prg_CheckIfMoveRightIsAllowed (NumItem))
     {
      /* Indexes of items */
      BeginIndex = Gbl.Prg.LstItems[NumItem].Index;
      EndIndex   = Gbl.Prg.LstItems[Prg_GetLastChild (NumItem)].Index;

      /* Move item and its children to left or right */
      DB_QueryUPDATE ("can not move items",
		      "UPDATE prg_items SET Level=Level+1"
		      " WHERE CrsCod=%ld"
		      " AND ItmInd>=%u AND ItmInd<=%u",
		      Gbl.Hierarchy.Crs.CrsCod,
		      BeginIndex,EndIndex);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
  }

/*****************************************************************************/
/****** Exchange the order of two consecutive items in a course program ******/
/*****************************************************************************/

static void Prg_ExchangeItems (int NumItemTop,int NumItemBottom)
  {
   extern const char *Txt_Movement_not_allowed;
   struct
     {
      unsigned Begin;
      unsigned End;
     } TopIndex, BottomIndex, Diff;

   if (NumItemTop    >= 0 &&
       NumItemBottom >= 0)
     {
      TopIndex.Begin    = Gbl.Prg.LstItems[NumItemTop                      ].Index;
      TopIndex.End      = Gbl.Prg.LstItems[Prg_GetLastChild (NumItemTop   )].Index;
      BottomIndex.Begin = Gbl.Prg.LstItems[NumItemBottom                   ].Index;
      BottomIndex.End   = Gbl.Prg.LstItems[Prg_GetLastChild (NumItemBottom)].Index;
      Diff.Begin        = BottomIndex.Begin - TopIndex.Begin;
      Diff.End          = BottomIndex.End   - TopIndex.End;

      Ale_ShowAlert (Ale_INFO,"TopIndex.Begin    = %u<br />"
			      "TopIndex.End      = %u<br />"
			      "BottomIndex.Begin = %u<br />"
			      "BottomIndex.End   = %u",
		     TopIndex.Begin,
		     TopIndex.End,
		     BottomIndex.Begin,
		     BottomIndex.End);


      /***** Lock table to make the move atomic *****/
      DB_Query ("can not lock tables to move program item",
		"LOCK TABLES prg_items WRITE");
      Gbl.DB.LockedTables = true;

      /***** Exchange indexes of items *****/
      // Although indexes are always continuous...
      // ...this implementation works even with non continuous indexes
      /*
      Example:
      TopIndex.Begin    =  5
		        = 10
      TopIndex.End      = 17
      BottomIndex.Begin = 28
      BottomIndex.End   = 49

      Diff.Begin = 28 -  5 = 23;
      Diff.End   = 49 - 17 = 32;

                                Step 1            Step 2            Step 3          (Equivalent to)
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
              |ItmInd|ItmCod|   |ItmInd|ItmCod|   |ItmInd|ItmCod|   |ItmInd|ItmCod| |ItmInd|ItmCod|
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
TopIndex.Begin:     5|   218|-->|--> -5|   218|-->|--> 37|   218|   |    37|   218| |     5|   221|
              |    10|   219|-->|-->-10|   219|-->|--> 42|   219|   |    42|   219| |    26|   222|
TopIndex.End: |    17|   220|-->|-->-17|   220|-->|--> 49|   220|   |    49|   220| |    37|   218|
BottomIndex.Begin: 28|   221|-->|-->-28|   221|   |   -28|   221|-->|-->  5|   221| |    42|   219|
BottomIndex.End:   49|   222|-->|-->-49|   222|   |   -49|   222|-->|--> 26|   222| |    49|   220|
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
      */
      /* Step 1: Change all indexes involved to negative,
		 necessary to preserve unique index (CrsCod,ItmInd) */
      DB_QueryUPDATE ("can not exchange indexes of items",
		      "UPDATE prg_items SET ItmInd=-ItmInd"
		      " WHERE CrsCod=%ld"
		      " AND ItmInd>=%u AND ItmInd<=%u",
		      Gbl.Hierarchy.Crs.CrsCod,
		      TopIndex.Begin,BottomIndex.End);

      /* Step 2: Increase top indexes */
      DB_QueryUPDATE ("can not exchange indexes of items",
		      "UPDATE prg_items SET ItmInd=-ItmInd+%u"
		      " WHERE CrsCod=%ld"
		      " AND ItmInd>=-%u AND ItmInd<=-%u",
		      Diff.End,
		      Gbl.Hierarchy.Crs.CrsCod,
		      TopIndex.End,TopIndex.Begin);		// All indexes in top part

      /* Step 3: Decrease bottom indexes */
      DB_QueryUPDATE ("can not exchange indexes of items",
		      "UPDATE prg_items SET ItmInd=-ItmInd-%u"
		      " WHERE CrsCod=%ld"
		      " AND ItmInd>=-%u AND ItmInd<=-%u",
		      Diff.Begin,
		      Gbl.Hierarchy.Crs.CrsCod,
		      BottomIndex.End,BottomIndex.Begin);	// All indexes in bottom part

      /***** Unlock table *****/
      Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				   // ...to not retry the unlock if error in unlocking
      DB_Query ("can not unlock tables after moving items",
		"UNLOCK TABLES");
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
  }

/*****************************************************************************/
/*************** Check if the title of a program item exists *****************/
/*****************************************************************************/

static bool Prg_CheckIfSimilarItemExists (const char *Field,const char *Value,long ItmCod)
  {
   /***** Get number of program items with a field value from database *****/
   return (DB_QueryCOUNT ("can not get similar program items",
			  "SELECT COUNT(*) FROM prg_items"
			  " WHERE CrsCod=%ld"
			  " AND %s='%s' AND ItmCod<>%ld",
			  Gbl.Hierarchy.Crs.CrsCod,
			  Field,Value,ItmCod) != 0);
  }

/*****************************************************************************/
/******* Put a form to create/edit program item and show current items *******/
/*****************************************************************************/

void Prg_RequestCreatePrgItem (void)
  {
   long ParentItmCod;
   unsigned NumItemParent;
   unsigned NumItemLastChild;
   long ItmCodBeforeForm;
   unsigned FormLevel;

   /***** Get the code of the parent program item *****/
   ParentItmCod = Prg_GetParamItmCod ();

   if (ParentItmCod > 0)
     {
      Prg_GetListPrgItems ();
      NumItemParent    = Prg_GetNumItemFromItmCod (ParentItmCod);
      NumItemLastChild = Prg_GetLastChild (NumItemParent);
      ItmCodBeforeForm = Gbl.Prg.LstItems[NumItemLastChild].ItmCod;
      FormLevel        = Gbl.Prg.LstItems[NumItemParent].Level + 1;
      Prg_FreeListItems ();
     }
   else
     {
      ItmCodBeforeForm = -1L;
      FormLevel        = 0;
     }

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_PUT_FORM_CREATE_ITEM,
		     -1L,ParentItmCod,ItmCodBeforeForm,FormLevel);
  }

void Prg_RequestChangePrgItem (void)
  {
   long ItmCodBeforeForm;
   unsigned NumItem;
   unsigned FormLevel;

   /***** Get the code of the program item *****/
   ItmCodBeforeForm = Prg_GetParamItmCod ();

   if (ItmCodBeforeForm > 0)
     {
      Prg_GetListPrgItems ();
      NumItem   = Prg_GetNumItemFromItmCod (ItmCodBeforeForm);
      FormLevel = Gbl.Prg.LstItems[NumItem].Level;
      Prg_FreeListItems ();
     }
   else
     {
      ItmCodBeforeForm  = -1L;
      FormLevel         = 0;
     }

   /***** Show current program items, if any *****/
   Prg_ShowAllItems (Prg_PUT_FORM_CHANGE_ITEM,
		     -1L,-1L,ItmCodBeforeForm,FormLevel);
  }

/*****************************************************************************/
/***************** Put a form to create a new program item *******************/
/*****************************************************************************/

static void Prg_ShowFormToCreateItem (long ParentItmCod)
  {
   extern const char *Hlp_COURSE_Program_new_item;
   extern const char *Txt_New_item;
   extern const char *Txt_Create_item;
   struct ProgramItem ParentItem;	// Parent item
   struct ProgramItem Item;
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_TO_000000,
      Dat_HMS_TO_235959
     };

   /***** Get data of the parent program item from database *****/
   ParentItem.Hierarchy.ItmCod = ParentItmCod;
   Prg_GetDataOfItemByCod (&ParentItem);

   /***** Initialize to empty program item *****/
   Prg_ResetItem (&Item);
   Item.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
   Item.TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
   Item.Open = true;

   /***** Begin form *****/
   Frm_StartFormAnchor (ActNewPrgItm,"highlighted_item");
   Prg_PutParamItmCod (ParentItem.Hierarchy.ItmCod);

   /***** Begin box and table *****/
   Box_BoxTableBegin ("100%",Txt_New_item,NULL,
		      Hlp_COURSE_Program_new_item,Box_NOT_CLOSABLE,2);

   /***** Show form *****/
   Prg_ShowFormItem (&Item,SetHMS,NULL);

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_item);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************** Put a form to create a new program item *******************/
/*****************************************************************************/

static void Prg_ShowFormToChangeItem (long ItmCod)
  {
   extern const char *Hlp_COURSE_Program_edit_item;
   extern const char *Txt_Edit_item;
   extern const char *Txt_Save_changes;
   struct ProgramItem Item;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_DO_NOT_SET,
      Dat_HMS_DO_NOT_SET
     };

   /***** Get data of the program item from database *****/
   Item.Hierarchy.ItmCod = ItmCod;
   Prg_GetDataOfItemByCod (&Item);
   Prg_GetPrgItemTxtFromDB (Item.Hierarchy.ItmCod,Txt);

   /***** Begin form *****/
   Frm_StartFormAnchor (ActChgPrgItm,"highlighted_item");
   Prg_PutParamItmCod (Item.Hierarchy.ItmCod);

   /***** Begin box and table *****/
   Box_BoxTableBegin ("100%",
		      Item.Title[0] ? Item.Title :
				      Txt_Edit_item,
		      NULL,
		      Hlp_COURSE_Program_edit_item,Box_NOT_CLOSABLE,2);

   /***** Show form *****/
   Prg_ShowFormItem (&Item,SetHMS,Txt);

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************** Put a form to create a new program item *******************/
/*****************************************************************************/

static void Prg_ShowFormItem (const struct ProgramItem *Item,
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
   HTM_INPUT_TEXT ("Title",Prg_MAX_CHARS_PROGRAM_ITEM_TITLE,Item->Title,false,
		   "id=\"Title\" required=\"required\""
		   " class=\"PRG_TITLE_DESCRIPTION_WIDTH\"");
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
	               " class=\"PRG_TITLE_DESCRIPTION_WIDTH\"");
   if (Txt)
      if (Txt[0])
         HTM_Txt (Txt);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Receive form to create a new program item *****************/
/*****************************************************************************/

void Prg_RecFormNewPrgItem (void)
  {
   extern const char *Txt_Already_existed_an_item_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_item;
   extern const char *Txt_Created_new_item_X;
   struct ProgramItem ParentItem;	// Parent item
   struct ProgramItem NewItem;		// Item data received from form
   bool NewItemIsCorrect = true;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the parent program item *****/
   ParentItem.Hierarchy.ItmCod = Prg_GetParamItmCod ();
   Prg_GetDataOfItemByCod (&ParentItem);

   /***** Set new item code *****/
   NewItem.Hierarchy.ItmCod = -1L;
   NewItem.Hierarchy.Level = ParentItem.Hierarchy.Level + 1;	// Create as child

   /***** Get start/end date-times *****/
   NewItem.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewItem.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get program item title *****/
   Par_GetParToText ("Title",NewItem.Title,Prg_MAX_BYTES_PROGRAM_ITEM_TITLE);

   /***** Get program item text *****/
   Par_GetParToHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewItem.TimeUTC[Dat_START_TIME] == 0)
      NewItem.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewItem.TimeUTC[Dat_END_TIME] == 0)
      NewItem.TimeUTC[Dat_END_TIME] = NewItem.TimeUTC[Dat_START_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Check if title is correct *****/
   if (NewItem.Title[0])	// If there's a program item title
     {
      /* If title of program item was in database... */
      if (Prg_CheckIfSimilarItemExists ("Title",NewItem.Title,NewItem.Hierarchy.ItmCod))
        {
         NewItemIsCorrect = false;

	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_item_with_the_title_X,
                        NewItem.Title);
        }
     }
   else	// If there is not a program item title
     {
      NewItemIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_item);
     }

   /***** Create a new program item or update an existing one *****/
   if (NewItemIsCorrect)
     {
      /* Add new program item to database */
      Prg_InsertItem (&ParentItem,&NewItem,Description);

      /* Write success message */
      Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_item_X,
		     NewItem.Title);
     }
   else
      /***** Show form to create item *****/
      // TODO: The form should be filled with partial data, now is always empty
      Prg_ShowFormToCreateItem (ParentItem.Hierarchy.ItmCod);

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (NewItem.Hierarchy.ItmCod);
  }

/*****************************************************************************/
/************* Receive form to change an existing program item ***************/
/*****************************************************************************/

void Prg_RecFormChgPrgItem (void)
  {
   extern const char *Txt_Already_existed_an_item_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_item;
   struct ProgramItem OldItem;	// Current program item data in database
   struct ProgramItem NewItem;	// Item data received from form
   bool NewItemIsCorrect = true;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the program item *****/
   NewItem.Hierarchy.ItmCod = Prg_GetParamItmCod ();

   /***** Get data of the old (current) program item from database *****/
   OldItem.Hierarchy.ItmCod = NewItem.Hierarchy.ItmCod;
   Prg_GetDataOfItemByCod (&OldItem);

   /***** Get start/end date-times *****/
   NewItem.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewItem.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get program item title *****/
   Par_GetParToText ("Title",NewItem.Title,Prg_MAX_BYTES_PROGRAM_ITEM_TITLE);

   /***** Get program item text *****/
   Par_GetParToHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewItem.TimeUTC[Dat_START_TIME] == 0)
      NewItem.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewItem.TimeUTC[Dat_END_TIME] == 0)
      NewItem.TimeUTC[Dat_END_TIME] = NewItem.TimeUTC[Dat_START_TIME] + 2 * 60 * 60;	// +2 hours

   /***** Check if title is correct *****/
   if (NewItem.Title[0])	// If there's a program item title
     {
      /* If title of program item was in database... */
      if (Prg_CheckIfSimilarItemExists ("Title",NewItem.Title,NewItem.Hierarchy.ItmCod))
        {
         NewItemIsCorrect = false;

	 Ale_ShowAlert (Ale_WARNING,Txt_Already_existed_an_item_with_the_title_X,
                        NewItem.Title);
        }
     }
   else	// If there is not a program item title
     {
      NewItemIsCorrect = false;
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_item);
     }

   /***** Update existing item *****/
   if (NewItemIsCorrect)
      /* Update program item */
      Prg_UpdateItem (&NewItem,Description);
   else
      /***** Show form to change item *****/
      // TODO: The form should be filled with partial data, now is always empty
      Prg_ShowFormToChangeItem (NewItem.Hierarchy.ItmCod);

   /***** Show program items again *****/
   Prg_ShowCourseProgramHighlightingItem (NewItem.Hierarchy.ItmCod);
  }

/*****************************************************************************/
/*********** Insert a new program item as a child of a parent item ***********/
/*****************************************************************************/

static void Prg_InsertItem (const struct ProgramItem *ParentItem,
		            struct ProgramItem *Item,const char *Txt)
  {
   unsigned NumItemParent;
   unsigned NumItemLastChild;

   /***** Lock table to create program item *****/
   DB_Query ("can not lock tables to create program item",
	     "LOCK TABLES prg_items WRITE");
   Gbl.DB.LockedTables = true;

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   if (Gbl.Prg.Num)	// There are items
     {
      if (ParentItem->Hierarchy.ItmCod > 0)	// Parent specified
	{
	 /***** Calculate where to insert *****/
	 NumItemParent    = Prg_GetNumItemFromItmCod (ParentItem->Hierarchy.ItmCod);
	 NumItemLastChild = Prg_GetLastChild (NumItemParent);
	 if (NumItemLastChild < Gbl.Prg.Num - 1)
	   {
	    /***** New program item will be inserted after last child of parent *****/
	    Item->Hierarchy.Index = Gbl.Prg.LstItems[NumItemLastChild + 1].Index;

	    /***** Move down all indexes of after last child of parent *****/
	    DB_QueryUPDATE ("can not move down items",
			    "UPDATE prg_items SET ItmInd=ItmInd+1"
			    " WHERE CrsCod=%ld"
			    " AND ItmInd>=%u"
			    " ORDER BY ItmInd DESC",	// Necessary to not create duplicate key (CrsCod,ItmInd)
			    Gbl.Hierarchy.Crs.CrsCod,
			    Item->Hierarchy.Index);
	   }
	 else
	    /***** New program item will be inserted at the end *****/
	    Item->Hierarchy.Index = Gbl.Prg.LstItems[Gbl.Prg.Num - 1].Index + 1;

	 /***** Child ==> parent level + 1 *****/
         Item->Hierarchy.Level = ParentItem->Hierarchy.Level + 1;
	}
      else	// No parent specified
	{
	 /***** New program item will be inserted at the end *****/
	 Item->Hierarchy.Index = Gbl.Prg.LstItems[Gbl.Prg.Num - 1].Index + 1;

	 /***** First level *****/
         Item->Hierarchy.Level = 1;
	}
     }
   else		// There are no items
     {
      /***** New program item will be inserted as the first one *****/
      Item->Hierarchy.Index = 1;

      /***** First level *****/
      Item->Hierarchy.Level = 1;
     }

   /***** Insert new program item *****/
   Item->Hierarchy.ItmCod = Prg_InsertItemIntoDB (Item,Txt);

   /***** Unlock table *****/
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after moving items",
	     "UNLOCK TABLES");

   /***** Free list items *****/
   Prg_FreeListItems ();
  }

/*****************************************************************************/
/***************** Create a new program item into database *******************/
/*****************************************************************************/

static long Prg_InsertItemIntoDB (struct ProgramItem *Item,const char *Txt)
  {
   return DB_QueryINSERTandReturnCode ("can not create new program item",
				       "INSERT INTO prg_items"
				       " (CrsCod,ItmInd,Level,UsrCod,StartTime,EndTime,Title,Txt)"
				       " VALUES"
				       " (%ld,%u,%u,%ld,FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				       "'%s','%s')",
				       Gbl.Hierarchy.Crs.CrsCod,
				       Item->Hierarchy.Index,
				       Item->Hierarchy.Level,
				       Gbl.Usrs.Me.UsrDat.UsrCod,
				       Item->TimeUTC[Dat_START_TIME],
				       Item->TimeUTC[Dat_END_TIME  ],
				       Item->Title,
				       Txt);
  }

/*****************************************************************************/
/******************** Update an existing program item ************************/
/*****************************************************************************/

static void Prg_UpdateItem (struct ProgramItem *Item,const char *Txt)
  {
   /***** Update the data of the program item *****/
   DB_QueryUPDATE ("can not update program item",
		   "UPDATE prg_items SET "
		   "StartTime=FROM_UNIXTIME(%ld),"
		   "EndTime=FROM_UNIXTIME(%ld),"
		   "Title='%s',Txt='%s'"
		   " WHERE ItmCod=%ld"
		   " AND CrsCod=%ld",	// Extra check
                   Item->TimeUTC[Dat_START_TIME],
                   Item->TimeUTC[Dat_END_TIME  ],
                   Item->Title,
                   Txt,
                   Item->Hierarchy.ItmCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/***************** Remove all the program items of a course ******************/
/*****************************************************************************/

void Prg_RemoveCrsItems (long CrsCod)
  {
   /***** Remove program items *****/
   DB_QueryDELETE ("can not remove all the program items of a course",
		   "DELETE FROM prg_items WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/***************** Get number of items in a course program *******************/
/*****************************************************************************/

unsigned Prg_GetNumItemsInCrsProgram (long CrsCod)
  {
   /***** Get number of items in a course program from database *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get number of items in course program",
			     "SELECT COUNT(*) FROM prg_items"
			     " WHERE CrsCod=%ld",
			     CrsCod);
  }

/*****************************************************************************/
/****************** Get number of courses with program items *****************/
/*****************************************************************************/
// Returns the number of courses with program items
// in this location (all the platform, current degree or current course)

unsigned Prg_GetNumCoursesWithItems (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with program items from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prg_items"
			 " WHERE CrsCod>0");
         break;
       case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM institutions,centres,degrees,courses,prg_items"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
       case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM centres,degrees,courses,prg_items"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM degrees,courses,prg_items"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT prg_items.CrsCod)"
			 " FROM courses,prg_items"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with program items",
                         "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prg_items"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with program items.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/************************ Get number of program items ************************/
/*****************************************************************************/
// Returns the number of program items in a hierarchy scope

unsigned Prg_GetNumItems (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumItems;

   /***** Get number of program items from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*)"
			 " FROM prg_items"
			 " WHERE CrsCod>0");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*)"
			 " FROM institutions,centres,degrees,courses,prg_items"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*)"
			 " FROM centres,degrees,courses,prg_items"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*)"
			 " FROM degrees,courses,prg_items"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*)"
			 " FROM courses,prg_items"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=prg_items.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of program items",
                         "SELECT COUNT(*)"
			 " FROM prg_items"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of program items *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumItems) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of program items.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumItems;
  }
