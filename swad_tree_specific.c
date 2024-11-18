// swad_tree_specific.c: list of specific items in tree nodes

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

#include "swad_error.h"
#include "swad_FAQ.h"
#include "swad_FAQ_database.h"
#include "swad_info.h"
#include "swad_program_resource.h"
#include "swad_resource.h"
#include "swad_resource_database.h"
#include "swad_tree.h"
#include "swad_tree_specific.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

// extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TreSpc_HideOrUnhideListItem (HidVis_HiddenOrVisible_t HiddenOrVisible);
static void TreSpc_MoveUpDownListItem (TreSpc_MoveUpDown_t UpDown);

static bool TreSpc_ExchangeListItem (const struct Tre_Node *Node,
				     const struct Tre_ListItem *ListItem2);

/*****************************************************************************/
/************************ Reset specific list item ***************************/
/*****************************************************************************/

void TreSpc_ResetListItem (struct Tre_Node *Node)
  {
   static void (*ResetSpcFields[Inf_NUM_TYPES]) (struct Tre_Node *Node) =
     {
      [Inf_PROGRAM	] = Rsc_ResetSpcFields,
      [Inf_FAQ		] = FAQ_ResetSpcFields,
     };

   /***** Reset common fields of specific item *****/
   Node->ListItem.Cod = -1L;
   Node->ListItem.Ind = 0;
   Node->ListItem.HiddenOrVisible = HidVis_VISIBLE;

   /***** Reset specific fields of specific item *****/
   if (ResetSpcFields[Node->InfoType])
      ResetSpcFields[Node->InfoType] (Node);
  }

/*****************************************************************************/
/*********************** View specific list of items *************************/
/*****************************************************************************/

void TreSpc_ViewListItemsAfterEdit (void)
  {
   Inf_Type_t InfoType;
   struct Tre_Node Node;

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_END_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,-1L);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************ Edit specific list of items ************************/
/*****************************************************************************/

void TreSpc_EditListItems (void)
  {
   Inf_Type_t InfoType;
   struct Tre_Node Node;

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,-1L);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************* Edit tree with form to change specific list item **************/
/*****************************************************************************/

void TreSpc_EditTreeWithFormListItem (void)
  {
   Inf_Type_t InfoType;
   struct Tre_Node Node;

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and list item *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_PRG_RESOURCE_LINK,
		     Node.Hierarchy.NodCod,Node.ListItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************* Rename specific list item *************************/
/*****************************************************************************/

void TreSpc_RenameListItem (void)
  {
   Inf_Type_t InfoType;
   struct Tre_Node Node;
   static void (*RenameListItem[Inf_NUM_TYPES]) (const struct Tre_Node *Node) =
     {
      [Inf_PROGRAM	] = PrgRsc_RenameResource,
      [Inf_FAQ		] = FAQ_RenameQaA,
     };

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();
   if (!RenameListItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and question&answer *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Rename specific list item *****/
   RenameListItem[InfoType] (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.ListItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********** Ask for confirmation of removing an specific list item ***********/
/*****************************************************************************/

void TreSpc_ReqRemListItem (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_resource_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   Inf_Type_t InfoType;
   struct Tre_Node Node;

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and specific list item *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Create alert to remove the specific list item *****/
   switch (InfoType)
     {
      case Inf_PROGRAM:
	 Ale_CreateAlert (Ale_QUESTION,TreSpc_LIST_ITEMS_SECTION_ID,
			  Txt_Do_you_really_want_to_remove_the_resource_X,
			  Node.Resource.Title);
	 break;
      case Inf_FAQ:
	 Ale_CreateAlert (Ale_QUESTION,TreSpc_LIST_ITEMS_SECTION_ID,
			  Txt_Do_you_really_want_to_remove_the_question_X,
			  Node.QaA.Question);
	 break;
      default:
	 break;
     }

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.ListItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********************** Remove an specific list item ************************/
/*****************************************************************************/

void TreSpc_RemoveListItem (void)
  {
   extern const char *Txt_Resource_removed;
   extern const char *Txt_Question_removed;
   Inf_Type_t InfoType;
   struct Tre_Node Node;
   static void (*RemoveListItem[Inf_NUM_TYPES]) (const struct Tre_Node *Node) =
     {
      [Inf_PROGRAM	] = Rsc_DB_RemoveResource,
      [Inf_FAQ		] = FAQ_DB_RemoveQaA,
     };
   static const char **Txt[Inf_NUM_TYPES] =
     {
      [Inf_PROGRAM	] = &Txt_Resource_removed,
      [Inf_FAQ		] = &Txt_Question_removed,
     };

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();
   if (!RemoveListItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get data of the specific list item from database *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Remove specific list item *****/
   RemoveListItem[InfoType] (&Node);

   /***** Create success alert *****/
   if (Txt[InfoType])
      Ale_CreateAlert (Ale_SUCCESS,TreSpc_LIST_ITEMS_SECTION_ID,*Txt[InfoType]);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.ListItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********************** Hide/unhide specific list item ***********************/
/*****************************************************************************/

void TreSpc_HideListItem (void)
  {
   TreSpc_HideOrUnhideListItem (HidVis_HIDDEN);
  }

void TreSpc_UnhideListItem (void)
  {
   TreSpc_HideOrUnhideListItem (HidVis_VISIBLE);
  }

static void TreSpc_HideOrUnhideListItem (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   Inf_Type_t InfoType;
   struct Tre_Node Node;
   static void (*HideOrUnhideListItem[Inf_NUM_TYPES]) (const struct Tre_Node *Node,
						       HidVis_HiddenOrVisible_t HiddenOrVisible) =
     {
      [Inf_PROGRAM	] = Rsc_DB_HideOrUnhideResource,
      [Inf_FAQ		] = FAQ_DB_HideOrUnhideQaA,
     };

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();
   if (!HideOrUnhideListItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and question&answer *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Hide/unhide specific list item *****/
   HideOrUnhideListItem[InfoType] (&Node,HiddenOrVisible);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.ListItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********************** Move up/down specific list item *********************/
/*****************************************************************************/

void TreSpc_MoveUpListItem (void)
  {
   TreSpc_MoveUpDownListItem (TreSpc_MOVE_UP);
  }

void TreSpc_MoveDownListItem (void)
  {
   TreSpc_MoveUpDownListItem (TreSpc_MOVE_DOWN);
  }

static void TreSpc_MoveUpDownListItem (TreSpc_MoveUpDown_t UpDown)
  {
   extern const char *Txt_Movement_not_allowed;
   Inf_Type_t InfoType;
   struct Tre_Node Node;
   struct Tre_ListItem SpcItem2;
   bool Success = false;
   static unsigned (*GetOtherInd[Inf_NUM_TYPES][TreSpc_NUM_MOVEMENTS_UP_DOWN])(const struct Tre_Node *Node) =
     {
      [Inf_PROGRAM	][TreSpc_MOVE_UP  ] = Rsc_DB_GetRscIndBefore,
      [Inf_PROGRAM	][TreSpc_MOVE_DOWN] = Rsc_DB_GetRscIndAfter,
      [Inf_FAQ		][TreSpc_MOVE_UP  ] = FAQ_DB_GetQaAIndBefore,
      [Inf_FAQ		][TreSpc_MOVE_DOWN] = FAQ_DB_GetQaAIndAfter,
     };
   static long (*GetCodFromInd[Inf_NUM_TYPES]) (long NodCod,unsigned Ind) =
     {
      [Inf_PROGRAM	] = Rsc_DB_GetRscCodFromRscInd,
      [Inf_FAQ		] = FAQ_DB_GetQaACodFromQaAInd,
     };

   /***** Set info type *****/
   InfoType = Inf_AsignInfoType ();
   if (!GetCodFromInd[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and resource *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Move up/down list item *****/
   if ((SpcItem2.Ind = GetOtherInd[InfoType][UpDown] (&Node)))	// 0 ==> movement not allowed
     {
      /* Get the other list item code */
      SpcItem2.Cod = GetCodFromInd[InfoType] (Node.Hierarchy.NodCod,SpcItem2.Ind);

      /* Exchange list items */
      Success = TreSpc_ExchangeListItem (&Node,&SpcItem2);
     }
   if (!Success)
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.ListItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********** Exchange the order of two consecutive questions&answers **********/
/*****************************************************************************/
// Return true if success

static bool TreSpc_ExchangeListItem (const struct Tre_Node *Node,
				     const struct Tre_ListItem *ListItem2)
  {
   const struct Tre_ListItem *ListItem1 = &Node->ListItem;
   static void (*LockTable[Inf_NUM_TYPES]) (void) =
     {
      [Inf_PROGRAM	] = Rsc_DB_LockTableResources,
      [Inf_FAQ		] = FAQ_DB_LockTableQaAs,
     };
   static void (*UpdateInd[Inf_NUM_TYPES]) (const struct Tre_Node *Node,long Cod,int Ind) =
     {
      [Inf_PROGRAM	] = Rsc_DB_UpdateRscInd,
      [Inf_FAQ		] = FAQ_DB_UpdateQaAInd,
     };

   if (!UpdateInd[Node->InfoType])
      Err_WrongTypeExit ();

   if (ListItem1->Ind > 0 &&	// Indexes should be in the range [1, 2,...]
       ListItem2->Ind > 0)
     {
      /***** Lock tables to make the move atomic *****/
      LockTable[Node->InfoType] ();

      /***** Exchange indexes of list items *****/
      // This implementation works with non continuous indexes
      /*
      Example:
      ListItem1->Ind =  5
      ListItem2->Ind = 17
                                Step 1            Step 2            Step 3  (Equivalent to)
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      |   Ind |   Cod |   |   Ind |   Cod |   |   Ind |   Cod |   |   Ind |   Cod | |   Ind |   Cod |
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      |     5 |   218 |   |     5 |   218 |-->|--> 17 |   218 |   |    17 |   218 | |     5 |   240 |
      |    17 |   240 |-->|-->-17 |   240 |   |   -17 |   240 |-->|-->  5 |   240 | |    17 |   218 |
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      */
      /* Step 1: Change second index to negative,
		 necessary to preserve unique index (NodCod,QaAInd) */
      UpdateInd[Node->InfoType] (Node,ListItem2->Cod,-(int) ListItem2->Ind);

      /* Step 2: Change first index */
      UpdateInd[Node->InfoType] (Node,ListItem1->Cod, (int) ListItem2->Ind);

      /* Step 3: Change second index */
      UpdateInd[Node->InfoType] (Node,ListItem2->Cod, (int) ListItem1->Ind);

      /***** Unlock tables *****/
      DB_UnlockTables ();

      return true;	// Success
     }

   return false;	// No success
  }
