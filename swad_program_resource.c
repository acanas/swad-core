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
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_program.h"
#include "swad_resource.h"
#include "swad_resource_database.h"
#include "swad_tree_specific.h"
#include "swad_view.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrgRsc_PutIconsViewRes (void *Node);
static void PrgRsc_PutIconsEditRes (void *Node);

static void PrgRsc_GetResourceDataFromRow (MYSQL_RES *mysql_res,
                                           struct Tre_Node *Node);
static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         const struct Tre_Node *Node,
                                         HidVis_HiddenOrVisible_t HiddenOrVisible);
void PrgRsc_WriteCellEditResource (struct Tre_Node *Node,
				   Vie_ViewType_t ViewType,
				   __attribute__((unused)) HidVis_HiddenOrVisible_t HiddenOrVisible);
static void PrgRsc_WriteRowNewResource (unsigned NumResources,
                                        struct Tre_Node *Node,
                                        Vie_ViewType_t LinkViewType);

static void PrgRsc_ShowClipboard (void);
static void PrgRsc_PutIconsClipboard (__attribute__((unused)) void *Args);

/*****************************************************************************/
/****************************** List resources *******************************/
/*****************************************************************************/

void PrgRsc_ListNodeResources (Tre_ListingType_t ListingType,
                               struct Tre_Node *Node,
                               long SelectedNodCod,
                               long SelectedRscCod,
                               HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *Hlp_COURSE_Program_resources;
   extern const char *Txt_Remove;
   extern const char *Txt_Resources;
   MYSQL_RES *mysql_res;
   unsigned NumRsc;
   unsigned NumResources;
   Vie_ViewType_t ViewingOrEditingResOfThisNode;
   static Vie_ViewType_t ViewingOrEditing[Tre_NUM_LISTING_TYPES] =
     {
      [Tre_PRINT			] = Vie_VIEW,
      [Tre_VIEW				] = Vie_VIEW,
      [Tre_EDIT_NODES			] = Vie_EDIT,
      [Tre_FORM_NEW_END_NODE		] = Vie_EDIT,
      [Tre_FORM_NEW_CHILD_NODE		] = Vie_EDIT,
      [Tre_FORM_EDIT_NODE		] = Vie_EDIT,
      [Tre_END_EDIT_NODE		] = Vie_EDIT,
      [Tre_RECEIVE_NODE			] = Vie_EDIT,
      [Tre_EDIT_SPC_LIST_ITEMS		] = Vie_EDIT,
      [Tre_EDIT_SPC_ITEM		] = Vie_EDIT,
      [Tre_CHG_SPC_ITEM			] = Vie_EDIT,
      [Tre_END_EDIT_SPC_LIST_ITEMS	] = Vie_EDIT,
     };
   static Vie_ViewType_t ViewingOrEditingRes[Tre_NUM_LISTING_TYPES] =
     {
      [Tre_PRINT			] = Vie_VIEW,
      [Tre_VIEW				] = Vie_VIEW,
      [Tre_EDIT_NODES			] = Vie_VIEW,
      [Tre_FORM_NEW_END_NODE		] = Vie_VIEW,
      [Tre_FORM_NEW_CHILD_NODE		] = Vie_VIEW,
      [Tre_FORM_EDIT_NODE		] = Vie_VIEW,
      [Tre_END_EDIT_NODE		] = Vie_VIEW,
      [Tre_RECEIVE_NODE			] = Vie_VIEW,
      [Tre_EDIT_SPC_LIST_ITEMS		] = Vie_EDIT,
      [Tre_EDIT_SPC_ITEM		] = Vie_EDIT,
      [Tre_CHG_SPC_ITEM			] = Vie_EDIT,
      [Tre_END_EDIT_SPC_LIST_ITEMS	] = Vie_VIEW,
     };
   static void (*PrgRsc_PutIconsRes[Vie_NUM_VIEW_TYPES]) (void *Node) =
     {
      [Vie_VIEW		] = PrgRsc_PutIconsEditRes,
      [Vie_EDIT		] = PrgRsc_PutIconsViewRes,
      [Vie_CONFIG	] = NULL,
      [Vie_PRINT	] = NULL,
     };

   /***** Trivial check *****/
   if (Node->Hierarchy.NodCod <= 0)
      return;

   /***** Get list of node resources from database *****/
   NumResources = Rsc_DB_GetListResources (&mysql_res,Node->Hierarchy.NodCod,
                                           ViewingOrEditingRes[ListingType] == Vie_EDIT);

   if (NumResources || ViewingOrEditing[ListingType] == Vie_EDIT)
     {
      if (Node->Hierarchy.NodCod == SelectedNodCod)
	{
	 /***** Begin section *****/
	 HTM_SECTION_Begin (TreSpc_LIST_ITEMS_SECTION_ID);

	    /***** Show possible alerts *****/
	    if (Gbl.Action.Act == ActReqRemPrgRsc)
	       /* Alert with button to remove resource */
	       Ale_ShowLastAlertAndButton (ActRemPrgRsc,TreSpc_LIST_ITEMS_SECTION_ID,NULL,
					   TreSpc_PutParItmCod,&SelectedRscCod,
					   Btn_REMOVE_BUTTON,Txt_Remove);
	    else
	       Ale_ShowAlerts (TreSpc_LIST_ITEMS_SECTION_ID);
	}

      /***** Begin box *****/
      ViewingOrEditingResOfThisNode = (ViewingOrEditingRes[ListingType] == Vie_EDIT &&
				       Node->Hierarchy.NodCod == SelectedNodCod) ? Vie_EDIT :
										   Vie_VIEW;
      switch (ViewingOrEditing[ListingType])
        {
         case Vie_VIEW:
	    Box_BoxBegin (NULL,NULL,NULL,NULL,Box_NOT_CLOSABLE);
	    break;
         case Vie_EDIT:
	    Box_BoxBegin (Txt_Resources,
			  PrgRsc_PutIconsRes[ViewingOrEditingResOfThisNode],Node,
			  Hlp_COURSE_Program_resources,Box_NOT_CLOSABLE);
	    break;
         default:
            Err_WrongTypeExit ();
            break;
        }

	 /***** Table *****/
	 HTM_TABLE_BeginWidePadding (2);

	    /***** Write all resources of this node *****/
	    for (NumRsc = 0;
		 NumRsc < NumResources;
		 NumRsc++)
	      {
	       /* Get data of this resource */
	       PrgRsc_GetResourceDataFromRow (mysql_res,Node);

	       /* Show resource */
	       switch (ViewingOrEditingResOfThisNode)
		 {
		  case Vie_VIEW:
		     PrgRsc_WriteRowViewResource (NumRsc,Node,HiddenOrVisible);
		     break;
		  case Vie_EDIT:
		     TreSpc_WriteRowEditItem (Node,NumRsc,NumResources,
					      (ListingType == Tre_EDIT_SPC_ITEM &&
					       Node->SpcItem.Cod == SelectedRscCod) ? Vie_EDIT :
										      Vie_VIEW,
					      HiddenOrVisible);
		     break;
		  default:
		     Err_WrongTypeExit ();
		     break;
		 }
	      }

	    /***** Form to create a new resource *****/
	    if (ViewingOrEditingResOfThisNode == Vie_EDIT)
	      {
	       TreSpc_ResetItem (Node);
	       PrgRsc_WriteRowNewResource (NumResources,Node,
					   (ListingType == Tre_EDIT_SPC_ITEM &&
					    Node->SpcItem.Cod == SelectedRscCod) ? Vie_EDIT :
										   Vie_VIEW);
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

      /***** End box *****/
      Box_BoxEnd ();

      /***** End section *****/
      if (Node->Hierarchy.NodCod == SelectedNodCod)
	 HTM_SECTION_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************** Put contextual icons in list of node resources ***************/
/*****************************************************************************/

static void PrgRsc_PutIconsViewRes (void *Node)
  {
   if (Node)
      if (((struct Tre_Node *) Node)->Hierarchy.NodCod > 0)
	 if (Tre_CheckIfICanEditTree () == Usr_CAN)
	    Ico_PutContextualIconToView (ActFrmSeePrgRsc,TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);
  }

static void PrgRsc_PutIconsEditRes (void *Node)
  {
   if (Node)
      if (((struct Tre_Node *) Node)->Hierarchy.NodCod > 0)
	 if (Tre_CheckIfICanEditTree () == Usr_CAN)
	    Ico_PutContextualIconToEdit (ActFrmEdiPrgRsc,TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);
  }

/*****************************************************************************/
/****************** Get node resource data using its code ********************/
/*****************************************************************************/

void PrgRsc_GetResourceDataByCod (struct Tre_Node *Node)
  {
   MYSQL_RES *mysql_res;

   if (Node->SpcItem.Cod > 0)
     {
      /***** Get data of resource *****/
      if (Rsc_DB_GetResourceDataByCod (&mysql_res,Node->SpcItem.Cod))
         PrgRsc_GetResourceDataFromRow (mysql_res,Node);
      else
	 /* Clear all node data except type */
         Tre_ResetNode (Node);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Clear all node data except type *****/
      Tre_ResetNode (Node);
  }

/*****************************************************************************/
/**************************** Get resource data ******************************/
/*****************************************************************************/

static void PrgRsc_GetResourceDataFromRow (MYSQL_RES *mysql_res,
                                           struct Tre_Node *Node)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NodCod	row[0]
   RscCod	row[1]
   RscInd	row[2]
   Hidden	row[3]
   Type		row[4]
   Cod		row[5]
   Title	row[6]
   */
   /***** Get code of the tree node (row[0]) *****/
   Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the resource (row[1], row[2]) *****/
   Node->SpcItem.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Node->SpcItem.Ind = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->SpcItem.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

   /***** Get link type and code (row[4], row[5]) *****/
   Node->Resource.Link.Type = Rsc_GetTypeFromString (row[4]);
   Node->Resource.Link.Cod  = Str_ConvertStrCodToLongCod (row[5]);

   /***** Get the title of the resource (row[6]) *****/
   Str_Copy (Node->Resource.Title,row[6],sizeof (Node->Resource.Title) - 1);
  }

/*****************************************************************************/
/***************************** Show one resource *****************************/
/*****************************************************************************/

static void PrgRsc_WriteRowViewResource (unsigned NumRsc,
                                         const struct Tre_Node *Node,
                                         HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"TRE_NUM PRG_RSC_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumRsc + 1);
      HTM_TD_End ();

      /***** Title *****/
      HTM_TD_Begin ("class=\"PRG_MAIN PRG_RSC_%s\"",The_GetSuffix ());
	 HTM_SPAN_Begin ("class=\"TRE_TIT PRG_TXT_%s%s\"",
			 The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
	    HTM_Txt (Node->Resource.Title);
	 HTM_SPAN_End ();
	 HTM_BR ();
	 Rsc_WriteLinkName (&Node->Resource.Link,Frm_PUT_FORM);
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************************** Edit one resource *****************************/
/*****************************************************************************/

void PrgRsc_WriteCellEditResource (struct Tre_Node *Node,
				   Vie_ViewType_t ViewType,
				   __attribute__((unused)) HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   /***** Title *****/
   Frm_BeginFormAnchor (ActRenPrgRsc,TreSpc_LIST_ITEMS_SECTION_ID);
      TreSpc_PutParItmCod (&Node->SpcItem.Cod);
      HTM_INPUT_TEXT ("Title",Rsc_MAX_CHARS_RESOURCE_TITLE,Node->Resource.Title,
		      HTM_SUBMIT_ON_CHANGE,
		      "class=\"PRG_RSC_INPUT INPUT_%s\"",
		      The_GetSuffix ());
   Frm_EndForm ();

   HTM_BR ();

   /***** Show current link / Show clipboard to change resource link *****/
   switch (ViewType)
     {
      case Vie_VIEW:
	 /* Show current link */
	 Rsc_WriteLinkName (&Node->Resource.Link,Frm_PUT_FORM);
	 break;
      case Vie_EDIT:
	 /* Show clipboard to change resource link */
	 Frm_BeginFormAnchor (ActChgPrgRsc,TreSpc_LIST_ITEMS_SECTION_ID);
	    TreSpc_PutParItmCod (&Node->SpcItem.Cod);
	    Rsc_ShowClipboardToChangeLink (&Node->Resource.Link);
	 Frm_EndForm ();
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
     }
  }

/*****************************************************************************/
/**************************** Edit a new resource ****************************/
/*****************************************************************************/

static void PrgRsc_WriteRowNewResource (unsigned NumResources,
                                        struct Tre_Node *Node,
                                        Vie_ViewType_t LinkViewType)
  {
   extern const char *Txt_New_resource;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this resource *****/
      HTM_TD_Begin ("class=\"PRG_RSC_COL1 LT %s\"",The_GetColorRows1 (1));
	 TreSpc_PutFormsToEditItem (Node,NumResources,NumResources);
      HTM_TD_End ();

      /***** Resource number *****/
      HTM_TD_Begin ("class=\"TRE_NUM PRG_RSC_%s %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumResources + 1);
      HTM_TD_End ();

      /***** Title and link/clipboard *****/
      HTM_TD_Begin ("class=\"PRG_MAIN %s\"",The_GetColorRows1 (1));

         /* Title */
	 Frm_BeginFormAnchor (ActNewPrgRsc,TreSpc_LIST_ITEMS_SECTION_ID);
	    ParCod_PutPar (ParCod_Nod,Node->Hierarchy.NodCod);
	    HTM_INPUT_TEXT ("Title",Rsc_MAX_CHARS_RESOURCE_TITLE,"",
			    HTM_SUBMIT_ON_CHANGE,
			    "placeholder=\"%s\""
			    " class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_New_resource,The_GetSuffix ());
	 Frm_EndForm ();

	 /* Edit link showing clipboard? */
         if (LinkViewType == Vie_EDIT)
	   {
            HTM_BR ();

	    /* Show clipboard to change resource link */
            Frm_BeginFormAnchor (ActChgPrgRsc,TreSpc_LIST_ITEMS_SECTION_ID);
               ParCod_PutPar (ParCod_Nod,Node->Hierarchy.NodCod);
               Rsc_ShowClipboardToChangeLink (&Node->Resource.Link);
	    Frm_EndForm ();
	   }

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
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
      TreSpc_CreateItem (Inf_PROGRAM);
   Prg_AfterTree ();
  }

void PrgRsc_CreateResourceInternal (struct Tre_Node *Node)
  {
   /***** Get the new title for the new resource *****/
   Par_GetParText ("Title",Node->Resource.Title,Rsc_MAX_BYTES_RESOURCE_TITLE);

   /***** Create resource *****/
   Node->SpcItem.Cod = Rsc_DB_CreateResource (Node);
  }

/*****************************************************************************/
/************************** Rename resource in list **************************/
/*****************************************************************************/

void PrgRsc_RenameResource (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      TreSpc_RenameItem (Inf_PROGRAM);
   Prg_AfterTree ();
  }

void PrgRsc_RenameResourceInternal (const struct Tre_Node *Node)
  {
   char NewTitle[Rsc_MAX_BYTES_RESOURCE_TITLE + 1];

   /***** Get the new title for the resource *****/
   Par_GetParText ("Title",NewTitle,Rsc_MAX_BYTES_RESOURCE_TITLE);

   /***** Update database changing old title by new title *****/
   Rsc_DB_UpdateResourceTitle (Node->Hierarchy.NodCod,Node->SpcItem.Cod,
			       NewTitle);
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

void PrgRsc_ChangeResourceLinkInternal (struct Tre_Node *Node)
  {
   /***** Get link type and code *****/
   if (Rsc_GetParLink (&Node->Resource.Link))
     {
      /***** Is it an existing resource? *****/
      if (Node->SpcItem.Cod <= 0)
	{
	 /* No resource selected, so it's a new resource at the end of the node */
	 /* Get the new title for the new resource from link title */
	 Rsc_GetResourceTitleFromLink (&Node->Resource.Link,Node->Resource.Title);

	 /***** Create resource *****/
	 Node->SpcItem.Cod = Rsc_DB_CreateResource (Node);
	}

      /***** Update resource link *****/
      Rsc_DB_UpdateRscLink (Node);

      /***** Remove link from clipboard *****/
      Rsc_DB_RemoveLinkFromClipboard (&Node->Resource.Link);
     }
  }
