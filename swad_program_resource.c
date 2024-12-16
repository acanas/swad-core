// swad_program_resource.c: course program (resources)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_button.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_hidden_visible.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_program.h"
#include "swad_program_resource.h"
#include "swad_resource.h"
#include "swad_resource_database.h"
#include "swad_tree_specific.h"
#include "swad_view.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrgRsc_ShowClipboard (void);
static void PrgRsc_PutIconsClipboard (__attribute__((unused)) void *Args);

/*****************************************************************************/
/**************************** Get resource data ******************************/
/*****************************************************************************/

void PrgRsc_GetRscDataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NodCod	row[0]
   ItmCod	row[1]
   ItmInd	row[2]
   Hidden	row[3]
   Type		row[4]
   Cod		row[5]
   Title	row[6]
   */
   /***** Get code of the tree node (row[0]) *****/
   Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the resource (row[1], row[2]) *****/
   Node->Item.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Node->Item.Ind = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->Item.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

   /***** Get link type and code (row[4], row[5]) *****/
   Node->Item.Rsc.Link.Type = Rsc_GetTypeFromString (row[4]);
   Node->Item.Rsc.Link.Cod  = Str_ConvertStrCodToLongCod (row[5]);

   /***** Get the title of the resource (row[6]) *****/
   Str_Copy (Node->Item.Rsc.Title,row[6],sizeof (Node->Item.Rsc.Title) - 1);
  }

/*****************************************************************************/
/***************************** Show one resource *****************************/
/*****************************************************************************/

void PrgRsc_WriteCellViewRsc (struct Tre_Node *Node)
  {
   HTM_SPAN_Begin ("class=\"ITM_TIT\"");
      HTM_Txt (Node->Item.Rsc.Title);
   HTM_SPAN_End ();
   HTM_BR ();
   Rsc_WriteLinkName (&Node->Item.Rsc.Link,Frm_PUT_FORM);
  }

/*****************************************************************************/
/***************************** Edit one resource *****************************/
/*****************************************************************************/

void PrgRsc_WriteCellEditRsc (struct Tre_Node *Node,
				   Vie_ViewType_t ViewType,
				   __attribute__((unused)) HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Save_changes;

   /***** Show current link / Show clipboard to change resource link *****/
   switch (ViewType)
     {
      case Vie_VIEW:
	 /* Show current link */
	 HTM_DIV_Begin ("class=\"TRE_TXT_%s%s\"",
		        The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
	    PrgRsc_WriteCellViewRsc (Node);
	 HTM_DIV_End ();
	 break;
      case Vie_EDIT:
	 Frm_BeginFormAnchor (ActChgPrgRsc,TreSpc_LIST_ITEMS_SECTION_ID);
	    TreSpc_PutParItmCod (&Node->Item.Cod);

	    /* Title */
	    HTM_INPUT_TEXT ("Title",Rsc_MAX_CHARS_RESOURCE_TITLE,Node->Item.Rsc.Title,
			    HTM_REQUIRED,
			    "class=\"ITM_INPUT INPUT_%s\"",
			    The_GetSuffix ());

	    /* Clipboard with resource links */
	    HTM_BR ();
	    Rsc_ShowClipboardToChangeLink (&Node->Item.Rsc.Link,HTM_NO_ATTR);

	    /* Button to save changes */
	    Btn_PutConfirmButtonInline (Txt_Save_changes);

	 Frm_EndForm ();
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
     }
  }

void PrgRsc_WriteCellNewRsc (void)
  {
   extern const char *Txt_New_resource;
   extern const char *Txt_Save_changes;

   /***** Title *****/
   HTM_INPUT_TEXT ("Title",Rsc_MAX_CHARS_RESOURCE_TITLE,"",
		   HTM_REQUIRED,
		   "placeholder=\"%s\""
		   " class=\"ITM_INPUT INPUT_%s\"",
		   Txt_New_resource,The_GetSuffix ());

   /***** Clipboard with resource links *****/
   HTM_BR ();
   Rsc_ShowClipboardToChangeLink (NULL,HTM_NO_ATTR);

   /***** Button to save changes *****/
   Btn_PutCreateButtonInline (Txt_Save_changes);
  }

/*****************************************************************************/
/******************** Show clipboard on top of program ***********************/
/*****************************************************************************/

void PrgRsc_ViewResourceClipboard (void)
  {
   /***** View resource clipboard *****/
   PrgRsc_ShowClipboard ();

   /***** Edit course program *****/
   Prg_EditTree ();
  }

/*****************************************************************************/
/************************* Show resources clipboard **************************/
/*****************************************************************************/

static void PrgRsc_ShowClipboard (void)
  {
   extern const char *Hlp_COURSE_Program_resource_clipboard;
   extern const char *Txt_Clipboard;

   Box_BoxBegin (Txt_Clipboard,PrgRsc_PutIconsClipboard,NULL,
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
   if (Tre_CheckIfICanEditTree () == Usr_CAN)
      if (Rsc_DB_GetNumResourcesInClipboard ())	// Only if there are resources
	 Ico_PutContextualIconToRemove (ActRemRscCliPrg,NULL,
					NULL,NULL);
  }

/*****************************************************************************/
/******************* Remove clipboard and show program ***********************/
/*****************************************************************************/

void PrgRsc_RemoveResourceClipboard (void)
  {
   extern const char *Txt_Clipboard_removed;

   /***** Remove resource clipboard *****/
   Rsc_DB_RemoveClipboard ();
   Ale_ShowAlert (Ale_SUCCESS,Txt_Clipboard_removed);

   /***** View resource clipboard again *****/
   PrgRsc_ViewResourceClipboard ();
  }

/*****************************************************************************/
/**************** View list of resources after editing them ******************/
/*****************************************************************************/

void PrgRsc_ViewResourcesAfterEdit (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_ViewListItemsAfterEdit (Inf_PROGRAM);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/*************************** Edit list of resources **************************/
/*****************************************************************************/

void PrgRsc_EditListResources (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_EditListItems (Inf_PROGRAM);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/************************* Create new resource in list ***********************/
/*****************************************************************************/

void PrgRsc_CreateResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_ChangeItem (Inf_PROGRAM);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/********** Ask for confirmation of removing a resource from list ************/
/*****************************************************************************/

void PrgRsc_ReqRemResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_ReqRemItem (Inf_PROGRAM);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/*********************** Remove a resource from list *************************/
/*****************************************************************************/

void PrgRsc_RemoveResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_RemoveItem (Inf_PROGRAM);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/*********************** Hide/unhide resource in list ************************/
/*****************************************************************************/

void PrgRsc_HideResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_HideOrUnhideItem (Inf_PROGRAM,HidVis_HIDDEN);
   Prg_AfterTree ();
  }

void PrgRsc_UnhideResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_HideOrUnhideItem (Inf_PROGRAM,HidVis_VISIBLE);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/************************ Move up/down resource in list **********************/
/*****************************************************************************/

void PrgRsc_MoveUpResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_MoveUpDownItem (Inf_PROGRAM,TreSpc_UP);
   Prg_AfterTree ();
  }

void PrgRsc_MoveDownResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_MoveUpDownItem (Inf_PROGRAM,TreSpc_DOWN);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/************** Edit tree with form to change resource in list ***************/
/*****************************************************************************/

void PrgRsc_EditTreeWithFormResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_EditTreeWithFormItem (Inf_PROGRAM);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/************************ Change resource link in list ***********************/
/*****************************************************************************/

void PrgRsc_ChangeResourceLink (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_ChangeItem (Inf_PROGRAM);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/***************** Get parameters to create/update resource ******************/
/*****************************************************************************/

void PrgRsc_GetParsRsc (struct Tre_Node *Node)
  {
   Par_GetParText ("Title",Node->Item.Rsc.Title,Rsc_MAX_BYTES_RESOURCE_TITLE);
   if (Rsc_GetParLink (&Node->Item.Rsc.Link))
      /* Remove link from clipboard */
      Rsc_DB_RemoveLinkFromClipboard (&Node->Item.Rsc.Link);
  }
