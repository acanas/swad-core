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
#include "swad_group.h"
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

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static long Prg_CurrentItmCod;		// Used as parameter in contextual links

static unsigned Prg_MaxLevel;		// Maximum level of items
static unsigned *Prg_NumItem = NULL;	// Numbers for each level from 1 to maximum level

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prg_ShowAllItems (void);
static bool Prg_CheckIfICanCreateItems (void);
static void Prg_PutIconsListItems (void);
static void Prg_PutIconToCreateNewItem (void);
static void Prg_PutButtonToCreateNewItem (void);
static void Prg_ShowOneItem (unsigned NumItem,bool PrintView);

static void Prg_CreateNumbers (unsigned MaxLevel);
static void Prg_FreeNumbers (void);
static void Prg_IncreaseNumItem (unsigned Level);
static unsigned Prg_GetNumItem (unsigned Level);
static void Prg_WriteNumItem (unsigned Level);

static void Prg_PutFormsToRemEditOnePrgItem (unsigned NumItem,const char *Anchor);
static bool Prg_CheckIfMoveUpIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveDownIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveLeftIsAllowed (unsigned NumItem);
static bool Prg_CheckIfMoveRightIsAllowed (unsigned NumItem);

static void Prg_SetCurrentItmCod (long ItmCod);
static long Prg_GetCurrentItmCod (void);
static void Prg_PutParams (void);

static void Prg_GetListPrgItems (void);
static void Prg_GetDataOfItem (struct ProgramItem *Item,
                               MYSQL_RES **mysql_res,
			       unsigned long NumRows);
static void Prg_ResetItem (struct ProgramItem *Item);
static void Prg_GetPrgItemTxtFromDB (long ItmCod,char Txt[Cns_MAX_BYTES_TEXT + 1]);
static void Prg_PutParamItmCod (long ItmCod);
static long Prg_GetParamItmCod (void);

static unsigned Prg_GetNumItemFromItmCod (unsigned ItmCod);

static unsigned Prg_GetMaxItemLevel (void);
static unsigned Prg_GetMaxItemIndex (void);
static int Prg_GetPrevBrother (int NumItem);
static int Prg_GetNextBrother (int NumItem);
static int Prg_GetLastChild (int NumItem);

static void Prg_ExchangeItems (int NumItemTop,int NumItemBottom);
static void Prg_MoveItemAndChildrenLeft (unsigned NumItem);
static void Prg_MoveItemAndChildrenRight (unsigned NumItem);

static bool Prg_CheckIfSimilarPrgItemExists (const char *Field,const char *Value,long ItmCod);
static void Prg_ShowLstGrpsToEditPrgItem (long ItmCod);
static void Prg_CreatePrgItem (struct ProgramItem *Item,const char *Txt);
static void Prg_UpdatePrgItem (struct ProgramItem *Item,const char *Txt);
static bool Prg_CheckIfItemIsAssociatedToGrps (long ItmCod);
static void Prg_RemoveAllTheGrpsAssociatedToAnItem (long ItmCod);
static void Prg_CreateGrps (long ItmCod);
static void Prg_GetAndWriteNamesOfGrpsAssociatedToItem (struct ProgramItem *Item);
static bool Prg_CheckIfIBelongToCrsOrGrpsThisItem (long ItmCod);

/*****************************************************************************/
/************************ List all the program items *************************/
/*****************************************************************************/

void Prg_SeeCourseProgram (void)
  {
   /***** Get parameters *****/
   Grp_GetParamWhichGrps ();

   /***** Show all the program items *****/
   Prg_ShowAllItems ();
  }

/*****************************************************************************/
/*********************** Show all the program items **************************/
/*****************************************************************************/

static void Prg_ShowAllItems (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Course_program;
   extern const char *Txt_No_items;
   unsigned NumItem;

   /***** Get list of program items *****/
   Prg_GetListPrgItems ();
   if (Gbl.Prg.Num)
     {
      Prg_MaxLevel = Prg_GetMaxItemLevel ();
      Prg_CreateNumbers (Prg_MaxLevel);
     }
   else
      Prg_MaxLevel = 0;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Course_program,Prg_PutIconsListItems,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

   /***** Select whether show only my groups or all groups *****/
   if (Gbl.Crs.Grps.NumGrps)
     {
      Set_StartSettingsHead ();
      Grp_ShowFormToSelWhichGrps (ActSeePrg,NULL);
      Set_EndSettingsHead ();
     }

   if (Gbl.Prg.Num)
     {
      /***** Table head *****/
      HTM_TABLE_BeginWideMarginPadding (2);

      /***** Write all the program items *****/
      for (NumItem = 0;
	   NumItem < Gbl.Prg.Num;
	   NumItem++)
	 Prg_ShowOneItem (NumItem,
	                  false);	// Not print view

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
   if (Gbl.Prg.Num)
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

static void Prg_ShowOneItem (unsigned NumItem,bool PrintView)
  {
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   char *Id;
   struct ProgramItem Item;
   unsigned ColSpan;
   Dat_StartEndTime_t StartEndTime;
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   /***** Get data of this program item *****/
   Item.Hierarchy.ItmCod = Gbl.Prg.LstItems[NumItem].ItmCod;
   Prg_GetDataOfItemByCod (&Item);
   Prg_IncreaseNumItem (Item.Hierarchy.Level);

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Item.Hierarchy.ItmCod,&Anchor);

   /***** Start row *****/
   HTM_TR_Begin (NULL);

   /***** Forms to remove/edit this program item *****/
   if (!PrintView)
     {
      HTM_TD_Begin ("class=\"PRG_COL1 LT COLOR%u\"",Gbl.RowEvenOdd);
      Prg_PutFormsToRemEditOnePrgItem (NumItem,Anchor);
      HTM_TD_End ();
     }

   /***** Indent depending on the level *****/
   if (Item.Hierarchy.Level > 1)
     {
      HTM_TD_Begin ("colspan=\"%u\" class=\"COLOR%u\"",
		    Item.Hierarchy.Level - 1,Gbl.RowEvenOdd);
      HTM_TD_End ();
     }

   /***** Item number *****/
   HTM_TD_Begin ("class=\"%s RT COLOR%u\" style=\"width:%dpx;\"",
		 Item.Hidden ? "ASG_TITLE_LIGHT" :
			       "ASG_TITLE",
		 Gbl.RowEvenOdd,
		 Item.Hierarchy.Level * Prg_WIDTH_NUM_ITEM);
   Prg_WriteNumItem (Item.Hierarchy.Level);
   HTM_TD_End ();

   /***** Title, groups and text *****/
   /* Begin title, groups and text */
   ColSpan = Prg_MaxLevel - Item.Hierarchy.Level + 1;
   if (PrintView)
      HTM_TD_Begin ("colspan=\"%u\" class=\"LT\"",
		    ColSpan);
   else
      HTM_TD_Begin ("colspan=\"%u\" class=\"LT COLOR%u\"",
		    ColSpan,Gbl.RowEvenOdd);
   HTM_ARTICLE_Begin (Anchor);

   /* Title */
   HTM_DIV_Begin ("class=\"%s\"",
		  Item.Hidden ? "ASG_TITLE_LIGHT" :
				"ASG_TITLE");
   HTM_Txt (Item.Title);
   HTM_DIV_End ();

   /* Groups */
   if (Gbl.Crs.Grps.NumGrps)
      Prg_GetAndWriteNamesOfGrpsAssociatedToItem (&Item);

   /* Text */
   Prg_GetPrgItemTxtFromDB (Item.Hierarchy.ItmCod,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links

   HTM_DIV_Begin ("class=\"PAR %s\"",Item.Hidden ? "DAT_LIGHT" :
        	                                   "DAT");
   HTM_Txt (Txt);
   HTM_DIV_End ();

   /* End title, groups and text */
   HTM_ARTICLE_End ();
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
		       Item.Hidden ? (Item.Open ? "DATE_GREEN_LIGHT" :
					          "DATE_RED_LIGHT") :
				     (Item.Open ? "DATE_GREEN" :
					          "DATE_RED"));
      else
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		       Id,
		       Item.Hidden ? (Item.Open ? "DATE_GREEN_LIGHT" :
					          "DATE_RED_LIGHT") :
				     (Item.Open ? "DATE_GREEN" :
					          "DATE_RED"),
		       Gbl.RowEvenOdd);
      Dat_WriteLocalDateHMSFromUTC (Id,Item.TimeUTC[StartEndTime],
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    true,true,true,0x7);
      HTM_TD_End ();
      free (Id);
     }

   HTM_TR_End ();

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********************* Allocate memory for item numbers **********************/
/*****************************************************************************/

static void Prg_CreateNumbers (unsigned MaxLevel)
  {
   /***** Allocate memory for item numbers and initialize to 0 *****/
   if ((Prg_NumItem = (unsigned *) calloc (1 + MaxLevel,sizeof (unsigned))) == NULL)
      Lay_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/*********************** Free memory for item numbers ************************/
/*****************************************************************************/

static void Prg_FreeNumbers (void)
  {
   /***** Free allocated memory for item numbers *****/
   if (Prg_NumItem)
     {
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
   HTM_Unsigned (Prg_GetNumItem (1));
   for (i  = 2;
	i <= Level;
	i++)
     {
      HTM_Txt (".");
      HTM_Unsigned (Prg_GetNumItem (i));
     }
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one program item *****************/
/*****************************************************************************/

static void Prg_PutFormsToRemEditOnePrgItem (unsigned NumItem,const char *Anchor)
  {
   extern const char *Txt_Move_up_X;
   extern const char *Txt_Move_down_X;
   extern const char *Txt_Increase_level_of_X;
   extern const char *Txt_Decrease_level_of_X;
   extern const char *Txt_Movement_not_allowed;
   char StrItemIndex[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   Prg_SetCurrentItmCod (Gbl.Prg.LstItems[NumItem].ItmCod);	// Used as parameter in contextual links

   /***** Initialize item index string *****/
   snprintf (StrItemIndex,sizeof (StrItemIndex),
	     "%u",
	     Gbl.Prg.LstItems[NumItem].Index);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put form to remove program item *****/
	 Ico_PutContextualIconToRemove (ActReqRemPrgItm,Prg_PutParams);

	 /***** Put form to hide/show program item *****/
	 if (Gbl.Prg.LstItems[NumItem].Hidden)
	    Ico_PutContextualIconToUnhide (ActShoPrgItm,Anchor,Prg_PutParams);
	 else
	    Ico_PutContextualIconToHide (ActHidPrgItm,Anchor,Prg_PutParams);

	 /***** Put form to edit program item *****/
	 Ico_PutContextualIconToEdit (ActEdiOnePrgItm,Prg_PutParams);

	 HTM_BR ();

	 /***** Put icon to move up the item *****/
	 if (Prg_CheckIfMoveUpIsAllowed (NumItem))
	   {
	    Lay_PutContextualLinkOnlyIcon (ActUp_PrgItm,NULL,Prg_PutParams,
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
	    Lay_PutContextualLinkOnlyIcon (ActDwnPrgItm,NULL,Prg_PutParams,
					   "arrow-down.svg",
					   Str_BuildStringStr (Txt_Move_down_X,
							       StrItemIndex));
	    Str_FreeString ();
	   }
	 else
	    Ico_PutIconOff ("arrow-down.svg",Txt_Movement_not_allowed);

	 HTM_BR ();

	 /***** Icon to move left item (increase level) *****/
	 if (Prg_CheckIfMoveLeftIsAllowed (NumItem))
	   {
	    Lay_PutContextualLinkOnlyIcon (ActLftPrgItm,NULL,Prg_PutParams,
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
	    Lay_PutContextualLinkOnlyIcon (ActRgtPrgItm,NULL,Prg_PutParams,
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
   Grp_PutParamWhichGrps ();
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
   if (Gbl.Crs.Grps.WhichGrps == Grp_MY_GROUPS)
      NumRows = DB_QuerySELECT (&mysql_res,"can not get program items",
	                        "SELECT ItmCod,"	// row[0]
	                               "ItmInd,"	// row[1]
	                               "Level,"		// row[2]
	                               "Hidden"		// row[3]
				" FROM prg_items"
				" WHERE CrsCod=%ld%s"
				" AND "
				"(ItmCod NOT IN (SELECT ItmCod FROM prg_grp) OR"
				" ItmCod IN (SELECT prg_grp.ItmCod"
				            " FROM prg_grp,crs_grp_usr"
				            " WHERE crs_grp_usr.UsrCod=%ld"
				            " AND prg_grp.GrpCod=crs_grp_usr.GrpCod))"
				" ORDER BY ItmInd",
				Gbl.Hierarchy.Crs.CrsCod,
				HiddenSubQuery[Gbl.Usrs.Me.Role.Logged],
				Gbl.Usrs.Me.UsrDat.UsrCod);
   else	// Gbl.Crs.Grps.WhichGrps == Grp_ALL_GROUPS
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

void Prg_GetDataOfItemByCod (struct ProgramItem *Item)
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
   if (NumRows) // Schedule item found...
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
      Item->Hidden = (row[3][0] == 'Y');

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

      /* Can I do this program item? */
      Item->IBelongToCrsOrGrps = Prg_CheckIfIBelongToCrsOrGrpsThisItem (Item->Hierarchy.ItmCod);
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
   Item->Hierarchy.Level  = 1;
   Item->Hidden = false;
   Item->UsrCod = -1L;
   Item->TimeUTC[Dat_START_TIME] =
   Item->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Item->Open = false;
   Item->Title[0] = '\0';
   Item->IBelongToCrsOrGrps = false;
  }

/*****************************************************************************/
/************************ Free list of program items *************************/
/*****************************************************************************/

void Prg_FreeListItems (void)
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

   /***** Get parameters *****/
   Grp_GetParamWhichGrps ();

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
   Prg_SeeCourseProgram ();
  }

/*****************************************************************************/
/*************************** Remove a program item ***************************/
/*****************************************************************************/

void Prg_RemovePrgItem (void)
  {
   extern const char *Txt_Item_X_removed;
   struct ProgramItem Item;

   /***** Get program item code *****/
   if ((Item.Hierarchy.ItmCod = Prg_GetParamItmCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of program item is missing.");

   /***** Get data of the program item from database *****/
   Prg_GetDataOfItemByCod (&Item);	// Inside this function, the course is checked to be the current one

   /***** Remove all the groups of this program item *****/
   Prg_RemoveAllTheGrpsAssociatedToAnItem (Item.Hierarchy.ItmCod);

   /***** Remove program item *****/
   DB_QueryDELETE ("can not remove program item",
		   "DELETE FROM prg_items WHERE ItmCod=%ld AND CrsCod=%ld",
                   Item.Hierarchy.ItmCod,Gbl.Hierarchy.Crs.CrsCod);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Item_X_removed,Item.Title);

   /***** Show program items again *****/
   Prg_SeeCourseProgram ();
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
   Prg_SeeCourseProgram ();
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
   Prg_SeeCourseProgram ();
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
   Prg_SeeCourseProgram ();
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
   Prg_SeeCourseProgram ();
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
   Prg_SeeCourseProgram ();
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
   Prg_SeeCourseProgram ();
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
/**************** Get maximum item index in a course program *****************/
/*****************************************************************************/
// Question index can be 1, 2, 3...
// Return 0 if no items

static unsigned Prg_GetMaxItemIndex (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned MaxIndex = 0;

   /***** Get maximum item index in a course program from database *****/
   DB_QuerySELECT (&mysql_res,"can not get last item index",
		   "SELECT MAX(ItmInd)"
		   " FROM prg_items"
		   " WHERE CrsCod=%ld",
                   Gbl.Hierarchy.Crs.CrsCod);
   row = mysql_fetch_row (mysql_res);
   if (row[0])	// There are items
      if (sscanf (row[0],"%u",&MaxIndex) != 1)
         Lay_ShowErrorAndExit ("Error when getting last item index.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return MaxIndex;
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

static int Prg_GetLastChild (int NumItem)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if item is wrong, there are no children *****/
   if (NumItem < 0 ||
       NumItem >= (int) Gbl.Prg.Num)
      return -1;

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

static bool Prg_CheckIfSimilarPrgItemExists (const char *Field,const char *Value,long ItmCod)
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
/***************** Put a form to create a new program item *******************/
/*****************************************************************************/

void Prg_RequestCreatOrEditPrgItem (void)
  {
   extern const char *Hlp_COURSE_Program_new_item;
   extern const char *Hlp_COURSE_Program_edit_item;
   extern const char *Txt_New_item;
   extern const char *Txt_Edit_item;
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_Create_item;
   extern const char *Txt_Save_changes;
   struct ProgramItem Item;
   bool ItsANewItem;
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      Dat_HMS_TO_000000,
      Dat_HMS_TO_235959
     };

   /***** Get parameters *****/
   Grp_GetParamWhichGrps ();

   /***** Get the code of the program item *****/
   ItsANewItem = ((Item.Hierarchy.ItmCod = Prg_GetParamItmCod ()) == -1L);

   /***** Get from the database the data of the program item *****/
   if (ItsANewItem)
     {
      /* Initialize to empty program item */
      Prg_ResetItem (&Item);
      Item.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
      Item.TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
      Item.Open = true;
     }
   else
     {
      /* Get data of the program item from database */
      Prg_GetDataOfItemByCod (&Item);

      /* Get text of the program item from database */
      Prg_GetPrgItemTxtFromDB (Item.Hierarchy.ItmCod,Txt);
     }

   /***** Begin form *****/
   if (ItsANewItem)
     {
      Frm_StartForm (ActNewPrgItm);
      Prg_SetCurrentItmCod (-1L);
     }
   else
     {
      Frm_StartForm (ActChgPrgItm);
      Prg_SetCurrentItmCod (Item.Hierarchy.ItmCod);
     }
   Prg_PutParams ();

   /***** Begin box and table *****/
   if (ItsANewItem)
      Box_BoxTableBegin (NULL,Txt_New_item,NULL,
			 Hlp_COURSE_Program_new_item,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,
                         Item.Title[0] ? Item.Title :
                	                 Txt_Edit_item,
                         NULL,
			 Hlp_COURSE_Program_edit_item,Box_NOT_CLOSABLE,2);


   /***** Schedule item title *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RM","Title",Txt_Title);

   /* Data */
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("Title",Prg_MAX_CHARS_PROGRAM_ITEM_TITLE,Item.Title,false,
		   "id=\"Title\" required=\"required\""
		   " class=\"TITLE_DESCRIPTION_WIDTH\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Program item start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (Item.TimeUTC,
					    Dat_FORM_SECONDS_ON,
					    SetHMS);

   /***** Program item text *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Txt",Txt_Description);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"Txt\" name=\"Txt\" rows=\"10\""
	               " class=\"TITLE_DESCRIPTION_WIDTH\"");
   if (!ItsANewItem)
      HTM_Txt (Txt);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Groups *****/
   Prg_ShowLstGrpsToEditPrgItem (Item.Hierarchy.ItmCod);

   /***** End table, send button and end box *****/
   if (ItsANewItem)
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_item);
   else
      Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** Show current program items, if any *****/
   Prg_ShowAllItems ();
  }

/*****************************************************************************/
/*************** Show list of groups to edit and program item ****************/
/*****************************************************************************/

static void Prg_ShowLstGrpsToEditPrgItem (long ItmCod)
  {
   extern const char *Hlp_USERS_Groups;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Groups;
   extern const char *Txt_The_whole_course;
   unsigned NumGrpTyp;

   /***** Get list of groups types and groups in this course *****/
   Grp_GetListGrpTypesAndGrpsInThisCrs (Grp_ONLY_GROUP_TYPES_WITH_GROUPS);

   if (Gbl.Crs.Grps.GrpTypes.Num)
     {
      /***** Begin box and table *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_Groups);
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LT\"");
      Box_BoxTableBegin ("100%",NULL,NULL,
                         Hlp_USERS_Groups,Box_NOT_CLOSABLE,0);

      /***** First row: checkbox to select the whole course *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("colspan=\"7\" class=\"DAT LM\"");
      HTM_LABEL_Begin (NULL);
      HTM_INPUT_CHECKBOX ("WholeCrs",false,
		          "id=\"WholeCrs\" value=\"Y\"%s"
		          " onclick=\"uncheckChildren(this,'GrpCods')\"",
			  Prg_CheckIfItemIsAssociatedToGrps (ItmCod) ? "" :
				                                             " checked=\"checked\"");
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** List the groups for each group type *****/
      for (NumGrpTyp = 0;
	   NumGrpTyp < Gbl.Crs.Grps.GrpTypes.Num;
	   NumGrpTyp++)
         if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].NumGrps)
            Grp_ListGrpsToEditAsgAttSvyMch (&Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp],
                                            ItmCod,Grp_PROGRAM_ITEM);

      /***** End table and box *****/
      Box_BoxTableEnd ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** Free list of groups types and groups in this course *****/
   Grp_FreeListGrpTypesAndGrps ();
  }

/*****************************************************************************/
/***************** Receive form to create a new program item *****************/
/*****************************************************************************/

void Prg_RecFormPrgItem (void)
  {
   extern const char *Txt_Already_existed_an_item_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_item;
   extern const char *Txt_Created_new_item_X;
   extern const char *Txt_The_item_has_been_modified;
   struct ProgramItem OldItem;	// Current program item data in database
   struct ProgramItem NewItem;	// Schedule item data received from form
   bool ItsANewItem;
   bool NewItemIsCorrect = true;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get the code of the program item *****/
   NewItem.Hierarchy.ItmCod = Prg_GetParamItmCod ();
   ItsANewItem = (NewItem.Hierarchy.ItmCod < 0);

   if (ItsANewItem)
      /***** Reset old (current, not existing) program item data *****/
      Prg_ResetItem (&OldItem);
   else
     {
      /***** Get data of the old (current) program item from database *****/
      OldItem.Hierarchy.ItmCod = NewItem.Hierarchy.ItmCod;
      Prg_GetDataOfItemByCod (&OldItem);
     }

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
      if (Prg_CheckIfSimilarPrgItemExists ("Title",NewItem.Title,NewItem.Hierarchy.ItmCod))
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
      /* Get groups for this program items */
      Grp_GetParCodsSeveralGrps ();

      if (ItsANewItem)
	{
         Prg_CreatePrgItem (&NewItem,Description);	// Add new program item to database

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_item_X,
		        NewItem.Title);
	}
      else
        {
	 Prg_UpdatePrgItem (&NewItem,Description);

	 /***** Write success message *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_item_has_been_modified);
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Show program items again *****/
      Prg_SeeCourseProgram ();
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Prg_RequestCreatOrEditPrgItem ();
  }

/*****************************************************************************/
/************************ Create a new program item **************************/
/*****************************************************************************/

static void Prg_CreatePrgItem (struct ProgramItem *Item,const char *Txt)
  {
   unsigned MaxIndex;

   /***** Lock table to create program item *****/
   DB_Query ("can not lock tables to create program item",
	     "LOCK TABLES prg_items WRITE");
   Gbl.DB.LockedTables = true;

   /***** Get maximum item index *****/
   MaxIndex = Prg_GetMaxItemIndex ();

   /***** Create a new program item *****/
   Item->Hierarchy.Index = MaxIndex + 1;
   Item->Hierarchy.ItmCod =
   DB_QueryINSERTandReturnCode ("can not create new program item",
				"INSERT INTO prg_items"
				" (ItmInd,CrsCod,UsrCod,StartTime,EndTime,Title,Txt)"
				" VALUES"
				" (%u,%ld,%ld,FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				"'%s','%s')",
				Item->Hierarchy.Index,
				Gbl.Hierarchy.Crs.CrsCod,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Item->TimeUTC[Dat_START_TIME],
				Item->TimeUTC[Dat_END_TIME  ],
				Item->Title,
				Txt);

   /***** Unlock table *****/
   Gbl.DB.LockedTables = false;	// Set to false before the following unlock...
				// ...to not retry the unlock if error in unlocking
   DB_Query ("can not unlock tables after moving items",
	     "UNLOCK TABLES");

   /***** Create groups *****/
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Prg_CreateGrps (Item->Hierarchy.ItmCod);
  }

/*****************************************************************************/
/******************** Update an existing program item ************************/
/*****************************************************************************/

static void Prg_UpdatePrgItem (struct ProgramItem *Item,const char *Txt)
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

   /***** Update groups *****/
   /* Remove old groups */
   Prg_RemoveAllTheGrpsAssociatedToAnItem (Item->Hierarchy.ItmCod);

   /* Create new groups */
   if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)
      Prg_CreateGrps (Item->Hierarchy.ItmCod);
  }

/*****************************************************************************/
/*********** Check if a program item is associated to any group **************/
/*****************************************************************************/

static bool Prg_CheckIfItemIsAssociatedToGrps (long ItmCod)
  {
   /***** Get if a program item is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if a program item"
			  " is associated to groups",
			  "SELECT COUNT(*) FROM prg_grp WHERE ItmCod=%ld",
			  ItmCod) != 0);
  }

/*****************************************************************************/
/************ Check if a program item is associated to a group ***************/
/*****************************************************************************/

bool Prg_CheckIfItemIsAssociatedToGrp (long ItmCod,long GrpCod)
  {
   /***** Get if a program item is associated to a group from database *****/
   return (DB_QueryCOUNT ("can not check if a program item"
			  " is associated to a group",
			  "SELECT COUNT(*) FROM prg_grp"
			  " WHERE ItmCod=%ld AND GrpCod=%ld",
		  	  ItmCod,GrpCod) != 0);
  }

/*****************************************************************************/
/********************* Remove groups of a program item ***********************/
/*****************************************************************************/

static void Prg_RemoveAllTheGrpsAssociatedToAnItem (long ItmCod)
  {
   /***** Remove groups of the program item *****/
   DB_QueryDELETE ("can not remove the groups associated to a program item",
		   "DELETE FROM prg_grp WHERE ItmCod=%ld",
		   ItmCod);
  }

/*****************************************************************************/
/*************** Remove one group from all the program items *****************/
/*****************************************************************************/

void Prg_RemoveGroup (long GrpCod)
  {
   /***** Remove group from all the program items *****/
   DB_QueryDELETE ("can not remove group from the associations"
	           " between program items and groups",
		   "DELETE FROM prg_grp WHERE GrpCod=%ld",
		   GrpCod);
  }

/*****************************************************************************/
/********** Remove groups of one type from all the program items *************/
/*****************************************************************************/

void Prg_RemoveGroupsOfType (long GrpTypCod)
  {
   /***** Remove group from all the program items *****/
   DB_QueryDELETE ("can not remove groups of a type from the associations"
	           " between program items and groups",
		   "DELETE FROM prg_grp USING crs_grp,prg_grp"
		   " WHERE crs_grp.GrpTypCod=%ld"
		   " AND crs_grp.GrpCod=prg_grp.GrpCod",
                   GrpTypCod);
  }

/*****************************************************************************/
/********************* Create groups of a program item ***********************/
/*****************************************************************************/

static void Prg_CreateGrps (long ItmCod)
  {
   unsigned NumGrpSel;

   /***** Create groups of the program item *****/
   for (NumGrpSel = 0;
	NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
	NumGrpSel++)
      /* Create group */
      DB_QueryINSERT ("can not associate a group to a program item",
		      "INSERT INTO prg_grp"
		      " (ItmCod,GrpCod)"
		      " VALUES"
		      " (%ld,%ld)",
                      ItmCod,
		      Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]);
  }

/*****************************************************************************/
/********* Get and write the names of the groups of a program item ***********/
/*****************************************************************************/

static void Prg_GetAndWriteNamesOfGrpsAssociatedToItem (struct ProgramItem *Item)
  {
   extern const char *Txt_Group;
   extern const char *Txt_Groups;
   extern const char *Txt_and;
   extern const char *Txt_The_whole_course;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;

   /***** Get groups associated to a program item from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get groups of a program item",
	                     "SELECT crs_grp_types.GrpTypName,"	// row[0]
	                            "crs_grp.GrpName"		// row[1]
			     " FROM prg_grp,crs_grp,"
			           "crs_grp_types"
			     " WHERE prg_grp.ItmCod=%ld"
			       " AND prg_grp.GrpCod=crs_grp.GrpCod"
			       " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
			     " ORDER BY crs_grp_types.GrpTypName,"
			               "crs_grp.GrpName",
			     Item->Hierarchy.ItmCod);

   /***** Write heading *****/
   HTM_DIV_Begin ("class=\"%s\"",Item->Hidden ? "ASG_GRP_LIGHT" :
        	                                "ASG_GRP");
   HTM_TxtColonNBSP (NumRows == 1 ? Txt_Group  :
                                    Txt_Groups);

   /***** Write groups *****/
   if (NumRows) // Groups found...
     {
      /* Get and write the group types and names */
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
        {
         /* Get next group */
         row = mysql_fetch_row (mysql_res);

         /* Write group type name and group name */
         HTM_TxtF ("%s&nbsp;%s",row[0],row[1]);

         if (NumRows >= 2)
           {
            if (NumRow == NumRows-2)
               HTM_TxtF (" %s ",Txt_and);
            if (NumRows >= 3)
              if (NumRow < NumRows-2)
                  HTM_Txt (", ");
           }
        }
     }
   else
      HTM_TxtF ("%s&nbsp;%s",Txt_The_whole_course,Gbl.Hierarchy.Crs.ShrtName);

   HTM_DIV_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Remove all the program items of a course ******************/
/*****************************************************************************/

void Prg_RemoveCrsItems (long CrsCod)
  {
   /***** Remove groups *****/
   DB_QueryDELETE ("can not remove all the groups associated"
		   " to program items of a course",
		   "DELETE FROM prg_grp USING prg_items,prg_grp"
		   " WHERE prg_items.CrsCod=%ld"
		   " AND prg_items.ItmCod=prg_grp.ItmCod",
                   CrsCod);

   /***** Remove program items *****/
   DB_QueryDELETE ("can not remove all the program items of a course",
		   "DELETE FROM prg_items WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/******** Check if I belong to any of the groups of a program item ***********/
/*****************************************************************************/

static bool Prg_CheckIfIBelongToCrsOrGrpsThisItem (long ItmCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 // Students and teachers can do program items depending on groups
	 /***** Get if I can do a program item from database *****/
	 return (DB_QueryCOUNT ("can not check if I can do a program item",
			        "SELECT COUNT(*) FROM prg_items"
				" WHERE ItmCod=%ld"
				" AND "
				"("
				// Schedule item is for the whole course
				"ItmCod NOT IN (SELECT ItmCod FROM prg_grp)"
				" OR "
				// Schedule item is for specific groups
				"ItmCod IN"
				" (SELECT prg_grp.ItmCod FROM prg_grp,crs_grp_usr"
				" WHERE crs_grp_usr.UsrCod=%ld"
				" AND prg_grp.GrpCod=crs_grp_usr.GrpCod)"
				")",
				ItmCod,Gbl.Usrs.Me.UsrDat.UsrCod) != 0);
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
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
