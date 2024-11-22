// swad_tree_specific.c: list of specific items in tree nodes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Ca�as Vargas

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
#include "swad_global.h"
#include "swad_info.h"
#include "swad_program_resource.h"
#include "swad_resource.h"
#include "swad_resource_database.h"
#include "swad_tree.h"
#include "swad_tree_specific.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static bool TreSpc_ExchangeItems (const struct Tre_Node *Node,
				  const struct Tre_SpcItem *Item2);

/*****************************************************************************/
/************************ Reset specific list item ***************************/
/*****************************************************************************/

void TreSpc_ResetItem (struct Tre_Node *Node)
  {
   static void (*ResetSpcFields[Inf_NUM_TYPES]) (struct Tre_Node *Node) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_ResetSpcFields,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_ResetSpcFields,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Reset common fields of specific item *****/
   Node->SpcItem.Cod = -1L;
   Node->SpcItem.Ind = 0;
   Node->SpcItem.HiddenOrVisible = HidVis_VISIBLE;

   /***** Reset specific fields of specific item *****/
   if (ResetSpcFields[Node->InfoType])
      ResetSpcFields[Node->InfoType] (Node);
  }

/*****************************************************************************/
/*************************** Edit a specific item ****************************/
/*****************************************************************************/

void TreSpc_WriteRowViewItem (struct Tre_Node *Node,
			      unsigned NumItem,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   static void (*WriteCell[Inf_NUM_TYPES]) (struct Tre_Node *Node,
					    HidVis_HiddenOrVisible_t HiddenOrVisible) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_WriteCellViewResource,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_WriteCellViewQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Question&Answer number *****/
      HTM_TD_Begin ("class=\"TRE_NUM PRG_RSC_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumItem + 1);
      HTM_TD_End ();

      /***** Question&Answer *****/
      HTM_TD_Begin ("class=\"PRG_MAIN PRG_RSC_%s\"",The_GetSuffix ());

	 /* Wite cell contents */
         if (WriteCell[Node->InfoType])
	    WriteCell[Node->InfoType] (Node,HiddenOrVisible);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Edit a specific item ****************************/
/*****************************************************************************/

void TreSpc_WriteRowEditItem (struct Tre_Node *Node,
			      unsigned NumItem,unsigned NumItems,
			      Vie_ViewType_t ViewType,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   static void (*WriteCell[Inf_NUM_TYPES]) (struct Tre_Node *Node,
					    Vie_ViewType_t ViewType,
					    HidVis_HiddenOrVisible_t HiddenOrVisible) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_WriteCellEditResource,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_WriteCellEditQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item *****/
      HTM_TD_Begin ("class=\"PRG_RSC_COL1 LT\"");
         TreSpc_PutFormsToEditItem (Node,NumItem,NumItems);
      HTM_TD_End ();

      /***** Item number *****/
      HTM_TD_Begin ("class=\"TRE_NUM PRG_RSC_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumItem + 1);
      HTM_TD_End ();

      /***** Item content *****/
      HTM_TD_Begin ("class=\"PRG_MAIN PRG_RSC_%s\"",The_GetSuffix ());

	 /* Write cell contents */
         if (WriteCell[Node->InfoType])
	    WriteCell[Node->InfoType] (Node,ViewType,HiddenOrVisible);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Edit a specific item ****************************/
/*****************************************************************************/

void TreSpc_WriteRowNewItem (struct Tre_Node *Node,unsigned NumItems)
  {
   static Act_Action_t ActionsNewItem[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActNewPrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActUnk,
      [Inf_FAQ		] = ActNewFAQQaA,
      [Inf_LINKS	] = ActUnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };
   static void (*WriteCell[Inf_NUM_TYPES]) (void) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_WriteCellNewResource,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_WriteCellNewQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this question & answer *****/
      HTM_TD_Begin ("class=\"PRG_RSC_COL1 LT %s\"",The_GetColorRows1 (1));
	 TreSpc_PutFormsToEditItem (Node,NumItems,NumItems);
      HTM_TD_End ();

      /***** Item number *****/
      HTM_TD_Begin ("class=\"TRE_NUM PRG_RSC_%s %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumItems + 1);
      HTM_TD_End ();

      /***** Item content *****/
      HTM_TD_Begin ("class=\"PRG_MAIN %s\"",The_GetColorRows1 (1));

	 Frm_BeginFormAnchor (ActionsNewItem[Node->InfoType],
			      TreSpc_LIST_ITEMS_SECTION_ID);
	    ParCod_PutPar (ParCod_Nod,Node->Hierarchy.NodCod);

	    /* Write cell contents */
	    if (WriteCell[Node->InfoType])
	       WriteCell[Node->InfoType] ();

	 Frm_EndForm ();

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Put forms to edit a specific item **********************/
/*****************************************************************************/

void TreSpc_PutFormsToEditItem (struct Tre_Node *Node,
                                unsigned NumItem,unsigned NumItems)
  {
   static Act_Action_t ActionsReqRem[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActReqRemPrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActUnk,
      [Inf_FAQ		] = ActReqRemFAQQaA,
      [Inf_LINKS	] = ActUnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };
   static Act_Action_t ActionHideUnhide[Inf_NUM_TYPES][HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [Inf_UNKNOWN_TYPE	][HidVis_HIDDEN ] = ActUnk,
      [Inf_UNKNOWN_TYPE	][HidVis_VISIBLE] = ActUnk,
      [Inf_INFORMATION	][HidVis_HIDDEN ] = ActUnk,
      [Inf_INFORMATION	][HidVis_VISIBLE] = ActUnk,
      [Inf_PROGRAM	][HidVis_HIDDEN ] = ActUnhPrgRsc,	// Hidden ==> action to unhide
      [Inf_PROGRAM	][HidVis_VISIBLE] = ActHidPrgRsc,	// Visible ==> action to hide
      [Inf_TEACH_GUIDE	][HidVis_HIDDEN ] = ActUnk,
      [Inf_TEACH_GUIDE	][HidVis_VISIBLE] = ActUnk,
      [Inf_SYLLABUS_LEC	][HidVis_HIDDEN ] = ActUnk,
      [Inf_SYLLABUS_LEC	][HidVis_VISIBLE] = ActUnk,
      [Inf_SYLLABUS_PRA	][HidVis_HIDDEN ] = ActUnk,
      [Inf_SYLLABUS_PRA	][HidVis_VISIBLE] = ActUnk,
      [Inf_BIBLIOGRAPHY	][HidVis_HIDDEN ] = ActUnk,
      [Inf_BIBLIOGRAPHY	][HidVis_VISIBLE] = ActUnk,
      [Inf_FAQ		][HidVis_HIDDEN ] = ActUnhFAQQaA,	// Hidden ==> action to unhide
      [Inf_FAQ		][HidVis_VISIBLE] = ActHidFAQQaA,	// Visible ==> action to hide
      [Inf_LINKS	][HidVis_HIDDEN ] = ActUnk,
      [Inf_LINKS	][HidVis_VISIBLE] = ActUnk,
      [Inf_ASSESSMENT	][HidVis_HIDDEN ] = ActUnk,
      [Inf_ASSESSMENT	][HidVis_VISIBLE] = ActUnk,
     };
   static Act_Action_t ActionsFrmChg[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActFrmChgPrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActUnk,
      [Inf_FAQ		] = ActFrmChgFAQQaA,
      [Inf_LINKS	] = ActUnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };
   static Act_Action_t ActionUpDown[Inf_NUM_TYPES][HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [Inf_UNKNOWN_TYPE	][TreSpc_UP  ] = ActUnk,
      [Inf_UNKNOWN_TYPE	][TreSpc_DOWN] = ActUnk,
      [Inf_INFORMATION	][TreSpc_UP  ] = ActUnk,
      [Inf_INFORMATION	][TreSpc_DOWN] = ActUnk,
      [Inf_PROGRAM	][TreSpc_UP  ] = ActUp_PrgRsc,
      [Inf_PROGRAM	][TreSpc_DOWN] = ActDwnPrgRsc,
      [Inf_TEACH_GUIDE	][TreSpc_UP  ] = ActUnk,
      [Inf_TEACH_GUIDE	][TreSpc_DOWN] = ActUnk,
      [Inf_SYLLABUS_LEC	][TreSpc_UP  ] = ActUnk,
      [Inf_SYLLABUS_LEC	][TreSpc_DOWN] = ActUnk,
      [Inf_SYLLABUS_PRA	][TreSpc_UP  ] = ActUnk,
      [Inf_SYLLABUS_PRA	][TreSpc_DOWN] = ActUnk,
      [Inf_BIBLIOGRAPHY	][TreSpc_UP  ] = ActUnk,
      [Inf_BIBLIOGRAPHY	][TreSpc_DOWN] = ActUnk,
      [Inf_FAQ		][TreSpc_UP  ] = ActUp_FAQQaA,
      [Inf_FAQ		][TreSpc_DOWN] = ActDwnFAQQaA,
      [Inf_LINKS	][TreSpc_UP  ] = ActUnk,
      [Inf_LINKS	][TreSpc_DOWN] = ActUnk,
      [Inf_ASSESSMENT	][TreSpc_UP  ] = ActUnk,
      [Inf_ASSESSMENT	][TreSpc_DOWN] = ActUnk,
     };

   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_Visible;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Icon to remove question & answer *****/
	 if (NumItem < NumItems)
	    Ico_PutContextualIconToRemove (ActionsReqRem[Node->InfoType],TreSpc_LIST_ITEMS_SECTION_ID,
					   TreSpc_PutParItmCod,&Node->SpcItem.Cod);
	 else
	    Ico_PutIconRemovalNotAllowed ();

	 /***** Icon to hide/unhide question & answer *****/
	 if (NumItem < NumItems)
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide[Node->InfoType],
					       TreSpc_LIST_ITEMS_SECTION_ID,
					       TreSpc_PutParItmCod,&Node->SpcItem.Cod,
					       Node->SpcItem.HiddenOrVisible);
	 else
	    Ico_PutIconOff ("eye.svg",Ico_GREEN,Txt_Visible);

	 /***** Put icon to edit the question & answer *****/
	 if (NumItem < NumItems)
	    Ico_PutContextualIconToEdit (ActionsFrmChg[Node->InfoType],
					 TreSpc_LIST_ITEMS_SECTION_ID,
					 TreSpc_PutParItmCod,&Node->SpcItem.Cod);
	 else
	    Ico_PutContextualIconToEdit (ActionsFrmChg[Node->InfoType],
					 TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);

	 /***** Icon to move up the question & answer *****/
	 if (NumItem > 0 && NumItem < NumItems)
	    Lay_PutContextualLinkOnlyIcon (ActionUpDown[Node->InfoType][TreSpc_UP],
					   TreSpc_LIST_ITEMS_SECTION_ID,
	                                   TreSpc_PutParItmCod,&Node->SpcItem.Cod,
	 			           "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the question & answer *****/
	 if (NumItem < NumItems - 1)
	    Lay_PutContextualLinkOnlyIcon (ActionUpDown[Node->InfoType][TreSpc_UP],
					   TreSpc_LIST_ITEMS_SECTION_ID,
	                                   TreSpc_PutParItmCod,&Node->SpcItem.Cod,
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
/******************* Param used to edit a specific item **********************/
/*****************************************************************************/

void TreSpc_PutParItmCod (void *ItmCod)
  {
   if (ItmCod)
      ParCod_PutPar (ParCod_Itm,*((long *) ItmCod));
  }

/*****************************************************************************/
/************** View specific list of items after editing them ***************/
/*****************************************************************************/

void TreSpc_ViewListItemsAfterEdit (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_END_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,-1L);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************ Edit specific list of items ************************/
/*****************************************************************************/

void TreSpc_EditListItems (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;

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

void TreSpc_EditTreeWithFormItem (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and list item *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_ITEM,
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************ Create new specific list item **********************/
/*****************************************************************************/

void TreSpc_CreateItem (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;
   static void (*CreateItem[Inf_NUM_TYPES]) (struct Tre_Node *Node) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_CreateResourceInternal,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_CreateQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Check info type *****/
   if (!CreateItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get parameters *****/
   /* Get tree node */
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /***** Create specific list item *****/
   CreateItem[InfoType] (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_ITEM,
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************* Rename specific list item *************************/
/*****************************************************************************/

void TreSpc_RenameItem (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;
   static void (*RenameItem[Inf_NUM_TYPES]) (const struct Tre_Node *Node) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_RenameResourceInternal,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_RenameQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Check info type *****/
   if (!RenameItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Rename specific list item *****/
   RenameItem[InfoType] (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************ Change specific list item **************************/
/*****************************************************************************/

void TreSpc_ChangeItem (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;
   static void (*ChangeItem[Inf_NUM_TYPES]) (struct Tre_Node *Node) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_ChangeResourceLinkInternal,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_ChangeQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Check info type *****/
   if (!ChangeItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Change specific list item *****/
   ChangeItem[InfoType] (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********** Ask for confirmation of removing an specific list item ***********/
/*****************************************************************************/

void TreSpc_ReqRemItem (Inf_Type_t InfoType)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_resource_X;
   extern const char *Txt_Do_you_really_want_to_remove_the_question_X;
   struct Tre_Node Node;

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
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********************** Remove an specific list item ************************/
/*****************************************************************************/

void TreSpc_RemoveItem (Inf_Type_t InfoType)
  {
   extern const char *Txt_Resource_removed;
   extern const char *Txt_Question_removed;
   struct Tre_Node Node;
   static void (*RemoveItem[Inf_NUM_TYPES]) (const struct Tre_Node *Node) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_DB_RemoveResource,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_DB_RemoveQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };
   static const char **Txt[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = &Txt_Resource_removed,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = &Txt_Question_removed,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Check info type *****/
   if (!RemoveItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get data of the specific list item from database *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Remove specific list item *****/
   RemoveItem[InfoType] (&Node);

   /***** Create success alert *****/
   if (Txt[InfoType])
      Ale_CreateAlert (Ale_SUCCESS,TreSpc_LIST_ITEMS_SECTION_ID,*Txt[InfoType]);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********************** Hide/unhide specific list item ***********************/
/*****************************************************************************/

void TreSpc_HideOrUnhideItem (Inf_Type_t InfoType,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Tre_Node Node;
   static void (*HideOrUnhideItem[Inf_NUM_TYPES]) (const struct Tre_Node *Node,
						   HidVis_HiddenOrVisible_t HiddenOrVisible) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_DB_HideOrUnhideResource,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_DB_HideOrUnhideQaA,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Check info type *****/
   if (!HideOrUnhideItem[InfoType])
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and question & answer *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Hide/unhide specific list item *****/
   HideOrUnhideItem[InfoType] (&Node,HiddenOrVisible);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********************** Move up/down specific list item *********************/
/*****************************************************************************/

void TreSpc_MoveUpDownItem (Inf_Type_t InfoType,TreSpc_UpDown_t UpDown)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Tre_Node Node;
   struct Tre_SpcItem Item2;
   bool Success = false;
   static unsigned (*GetOtherInd[Inf_NUM_TYPES][TreSpc_NUM_UP_DOWN])(const struct Tre_Node *Node) =
     {
      [Inf_UNKNOWN_TYPE	][TreSpc_UP  ] = NULL,
      [Inf_UNKNOWN_TYPE	][TreSpc_DOWN] = NULL,
      [Inf_INFORMATION	][TreSpc_UP  ] = NULL,
      [Inf_INFORMATION	][TreSpc_DOWN] = NULL,
      [Inf_PROGRAM	][TreSpc_UP  ] = Rsc_DB_GetRscIndBefore,
      [Inf_PROGRAM	][TreSpc_DOWN] = Rsc_DB_GetRscIndAfter,
      [Inf_TEACH_GUIDE	][TreSpc_UP  ] = NULL,
      [Inf_TEACH_GUIDE	][TreSpc_DOWN] = NULL,
      [Inf_SYLLABUS_LEC	][TreSpc_UP  ] = NULL,
      [Inf_SYLLABUS_LEC	][TreSpc_DOWN] = NULL,
      [Inf_SYLLABUS_PRA	][TreSpc_UP  ] = NULL,
      [Inf_SYLLABUS_PRA	][TreSpc_DOWN] = NULL,
      [Inf_BIBLIOGRAPHY	][TreSpc_UP  ] = NULL,
      [Inf_BIBLIOGRAPHY	][TreSpc_DOWN] = NULL,
      [Inf_FAQ		][TreSpc_UP  ] = FAQ_DB_GetQaAIndBefore,
      [Inf_FAQ		][TreSpc_DOWN] = FAQ_DB_GetQaAIndAfter,
      [Inf_LINKS	][TreSpc_UP  ] = NULL,
      [Inf_LINKS	][TreSpc_DOWN] = NULL,
      [Inf_ASSESSMENT	][TreSpc_UP  ] = NULL,
      [Inf_ASSESSMENT	][TreSpc_DOWN] = NULL,
     };
   static long (*GetCodFromInd[Inf_NUM_TYPES]) (long NodCod,unsigned Ind) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_DB_GetRscCodFromRscInd,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_DB_GetQaACodFromQaAInd,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Check info type *****/
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
   if ((Item2.Ind = GetOtherInd[InfoType][UpDown] (&Node)))	// 0 ==> movement not allowed
     {
      /* Get the other list item code */
      Item2.Cod = GetCodFromInd[InfoType] (Node.Hierarchy.NodCod,Item2.Ind);

      /* Exchange items */
      Success = TreSpc_ExchangeItems (&Node,&Item2);
     }
   if (!Success)
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (InfoType,Tre_EDIT_SPC_LIST_ITEMS,
		     Node.Hierarchy.NodCod,Node.SpcItem.Cod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********** Exchange the order of two consecutive questions & answers **********/
/*****************************************************************************/
// Return true if success

static bool TreSpc_ExchangeItems (const struct Tre_Node *Node,
				  const struct Tre_SpcItem *Item2)
  {
   const struct Tre_SpcItem *Item1 = &Node->SpcItem;
   static void (*LockTable[Inf_NUM_TYPES]) (void) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_DB_LockTableResources,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_DB_LockTableQaAs,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };
   static void (*UpdateInd[Inf_NUM_TYPES]) (const struct Tre_Node *Node,long Cod,int Ind) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_DB_UpdateRscInd,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = NULL,
      [Inf_FAQ		] = FAQ_DB_UpdateQaAInd,
      [Inf_LINKS	] = NULL,
      [Inf_ASSESSMENT	] = NULL,
     };

   if (!UpdateInd[Node->InfoType])
      Err_WrongTypeExit ();

   if (Item1->Ind > 0 &&	// Indexes should be in the range [1, 2,...]
       Item2->Ind > 0)
     {
      /***** Lock tables to make the move atomic *****/
      LockTable[Node->InfoType] ();

      /***** Exchange indexes of list items *****/
      // This implementation works with non continuous indexes
      /*
      Example:
      Item1->Ind =  5
      Item2->Ind = 17
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
      UpdateInd[Node->InfoType] (Node,Item2->Cod,-(int) Item2->Ind);

      /* Step 2: Change first index */
      UpdateInd[Node->InfoType] (Node,Item1->Cod, (int) Item2->Ind);

      /* Step 3: Change second index */
      UpdateInd[Node->InfoType] (Node,Item2->Cod, (int) Item1->Ind);

      /***** Unlock tables *****/
      DB_UnlockTables ();

      return true;	// Success
     }

   return false;	// No success
  }
