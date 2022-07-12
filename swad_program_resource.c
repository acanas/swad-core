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
// #include "swad_error.h"
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
static void PrgRsc_WriteRowResource (unsigned NumRsc,struct PrgRsc_Resource *Resource,
			             bool PrintView);

/*****************************************************************************/
/************************* Show all program items ****************************/
/*****************************************************************************/

void PrgRsc_ShowAllResources (long ItmCod)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_Resources;
   MYSQL_RES *mysql_res;
   unsigned NumRsc;
   unsigned NumResources;
   struct PrgRsc_Resource Resource;

   /***** Trivial check *****/
   if (ItmCod <= 0)
      return;

   /***** Begin box *****/
   Box_BoxBegin ("100%",Txt_Resources,
                 PrgRsc_PutIconsListResources,&ItmCod,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      /***** Get list of item resources from database *****/
      NumResources = Prg_DB_GetListResources (&mysql_res,ItmCod); // Resources found...

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
               PrgRsc_WriteRowResource (NumRsc,&Resource,false);	// Not print view

	       /* Show form to create/change item */
	       /*
	       if (Item.Hierarchy.ItmCod == ItmCodBeforeForm)
		  switch (CreateOrChangeItem)
		    {
		     case Prg_DONT_PUT_FORM_ITEM:
			break;
		     case Prg_PUT_FORM_CREATE_ITEM:
			Prg_WriteRowWithItemForm (Prg_PUT_FORM_CREATE_ITEM,
						  ParentItmCod,FormLevel);
			break;
		     case Prg_PUT_FORM_CHANGE_ITEM:
			Prg_WriteRowWithItemForm (Prg_PUT_FORM_CHANGE_ITEM,
						  ItmCodBeforeForm,FormLevel);
			break;
		    }
	       */

	       The_ChangeRowColor ();
	      }

	    /***** Create item at the end? *****/
	    /*
	    if (ItmCodBeforeForm <= 0 && CreateOrChangeItem == Prg_PUT_FORM_CREATE_ITEM)
	       Prg_WriteRowWithItemForm (Prg_PUT_FORM_CREATE_ITEM,-1L,1);
	    */

	 /***** End table *****/
	 HTM_TBODY_End ();					// 3rd tbody end
      HTM_TABLE_End ();

      /***** Button to create a new program item *****/
      if (Prg_CheckIfICanCreateItems ())
	 PrgRsc_PutButtonToCreateNewResource (ItmCod);

   /***** End box *****/
   Box_BoxEnd ();
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
   Ico_PutContextualIconToAdd (ActFrmNewPrgRsc,"rsc_form",
                               Prg_PutParams,&ItmCod);
  }

/*****************************************************************************/
/***************** Put button to create a new item resource ******************/
/*****************************************************************************/

static void PrgRsc_PutButtonToCreateNewResource (long ItmCod)
  {
   extern const char *Txt_New_resource;

   Frm_BeginFormAnchor (ActFrmNewPrgRsc,"rsc_form");
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

   /***** Clear all item resource data *****/
   PrgRsc_ResetResource (Resource);

   if (Resource->RscCod > 0)
     {
      /***** Get data of item resource *****/
      if (Prg_DB_GetDataOfResourceByCod (&mysql_res,
                                         Resource->ItmCod,
                                         Resource->RscCod))
         PrgRsc_GetDataOfResource (Resource,&mysql_res);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
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
   Resource->Hidden = false;
   Resource->Title[0] = '\0';
  }

/*****************************************************************************/
/************************** Show one program item ****************************/
/*****************************************************************************/

static void PrgRsc_WriteRowResource (unsigned NumRsc,struct PrgRsc_Resource *Resource,
			             bool PrintView)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item resource *****/
      if (!PrintView)
	{
	 HTM_TD_Begin ("class=\"PRG_COL1 LT %s\"",The_GetColorRows ());
	    // Prg_PutFormsToRemEditOneItem (NumItem,Item);
	 HTM_TD_End ();
	}

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"PRG_NUM RT %s\"",The_GetColorRows ());
	 HTM_Unsigned (NumRsc);
      HTM_TD_End ();

      /***** Title and text *****/
      /* Begin title and text */
      if (PrintView)
	 HTM_TD_Begin ("class=\"PRG_MAIN\"");
      else
	 HTM_TD_Begin ("class=\"PRG_MAIN %s\"",The_GetColorRows ());

      /* Title */
      HTM_DIV_Begin ("class=\"DAT\"");
	 HTM_Txt (Resource->Title);
      HTM_DIV_End ();

      /* End title and text */
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }
