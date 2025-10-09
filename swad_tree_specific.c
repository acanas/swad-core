// swad_tree_specific.c: list of specific items in tree nodes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_bibliography.h"
#include "swad_bibliography_database.h"
#include "swad_box.h"
#include "swad_error.h"
#include "swad_FAQ.h"
#include "swad_FAQ_database.h"
#include "swad_global.h"
#include "swad_info.h"
#include "swad_link.h"
#include "swad_link_database.h"
#include "swad_program_resource.h"
#include "swad_resource.h"
#include "swad_resource_database.h"
#include "swad_tree.h"
#include "swad_tree_database.h"
#include "swad_tree_specific.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static void (*TreSpc_GetItemDataFromRow[Inf_NUM_TYPES]) (MYSQL_RES *mysql_res,
							 struct Tre_Node *Node) =
  {
   [Inf_UNKNOWN_TYPE	] = NULL,
   [Inf_INFORMATION	] = NULL,
   [Inf_PROGRAM		] = PrgRsc_GetRscDataFromRow,
   [Inf_TEACH_GUIDE	] = NULL,
   [Inf_SYLLABUS_LEC	] = NULL,
   [Inf_SYLLABUS_PRA	] = NULL,
   [Inf_BIBLIOGRAPHY	] = Bib_GetRefDataFromRow,
   [Inf_FAQ		] = FAQ_GetQaADataFromRow,
   [Inf_LINKS		] = Lnk_GetLnkDataFromRow,
   [Inf_ASSESSMENT	] = NULL,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TreSpc_PutIconsViewItem (void *Node);
static void TreSpc_PutIconsEditItem (void *Node);

static void TreSpc_WriteRowViewItem (struct Tre_Node *Node,unsigned NumItem);
static void TreSpc_WriteRowEditItem (struct Tre_Node *Node,
				     unsigned NumItem,unsigned NumItems,
				     Vie_ViewType_t ViewType,
				     HidVis_HiddenOrVisible_t HiddenOrVisible);
static void TreSpc_WriteRowNewItem (struct Tre_Node *Node,unsigned NumItems);

static void TreSpc_PutFormsToEditItem (struct Tre_Node *Node,
				       unsigned NumItem,unsigned NumItems);

static Err_SuccessOrError_t TreSpc_ExchangeItems (const struct Tre_Node *Node,
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
      [Inf_BIBLIOGRAPHY	] = Bib_ResetSpcFields,
      [Inf_FAQ		] = FAQ_ResetSpcFields,
      [Inf_LINKS	] = Lnk_ResetSpcFields,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Reset common fields of specific item *****/
   Node->Item.Cod = -1L;
   Node->Item.Ind = 0;
   Node->Item.Hidden = HidVis_VISIBLE;

   /***** Reset specific fields of specific item *****/
   if (ResetSpcFields[Node->InfoType])
      ResetSpcFields[Node->InfoType] (Node);
  }

/*****************************************************************************/
/************************ Get item data using its code ***********************/
/*****************************************************************************/

void TreSpc_GetItemDataByCod (struct Tre_Node *Node)
  {
   static Exi_Exist_t (*GetItemDataByCod[Inf_NUM_TYPES]) (MYSQL_RES **mysql_res,
							  long ItmCod) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_DB_GetRscDataByCod,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = Bib_DB_GetRefDataByCod,
      [Inf_FAQ		] = FAQ_DB_GetQaADataByCod,
      [Inf_LINKS	] = Lnk_DB_GetLnkDataByCod,
      [Inf_ASSESSMENT	] = NULL,
     };
   MYSQL_RES *mysql_res;

   if (Node->Item.Cod > 0)
     {
      /***** Get data of resource *****/
      switch (GetItemDataByCod[Node->InfoType] (&mysql_res,Node->Item.Cod))
	{
	 case Exi_EXISTS:
	    TreSpc_GetItemDataFromRow[Node->InfoType] (mysql_res,Node);
	    break;
	 case Exi_DOES_NOT_EXIST:
	 default:
	    /* Clear all node data except type */
	    Tre_ResetNode (Node);
	    break;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Clear all node data except type *****/
      Tre_ResetNode (Node);
  }

/*****************************************************************************/
/****************************** List questions *******************************/
/*****************************************************************************/

void TreSpc_ListNodeItems (Tre_ListingType_t ListingType,
			   struct Tre_Node *Node,
			   long SelectedNodCod,
			   long SelectedItmCod,
			   HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *Hlp_COURSE_FAQ;
   extern const char *Txt_Resources;
   extern const char *Txt_Bibliographic_references;
   extern const char *Txt_Questions;
   extern const char *Txt_Links;
   MYSQL_RES *mysql_res;
   unsigned NumItem;
   unsigned NumItems;
   Vie_ViewType_t ViewingOrEditingListItemsOfThisNode;
   static unsigned (*GetListItems[Inf_NUM_TYPES]) (MYSQL_RES **mysql_res,long NodCod,
						   Lay_Show_t ShowHiddenItems) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Rsc_DB_GetListRscs,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = Bib_DB_GetListRefs,
      [Inf_FAQ		] = FAQ_DB_GetListQaAs,
      [Inf_LINKS	] = Lnk_DB_GetListLnks,
      [Inf_ASSESSMENT	] = NULL,
     };
   static Act_Action_t ActionsReqRemItem[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActReqRemPrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActReqRemBibRef,
      [Inf_FAQ		] = ActReqRemFAQQaA,
      [Inf_LINKS	] = ActReqRemCrsLnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };
   static Act_Action_t ActionsRemItem[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActRemPrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActRemBibRef,
      [Inf_FAQ		] = ActRemFAQQaA,
      [Inf_LINKS	] = ActRemCrsLnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };
   static const char **Title[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = &Txt_Resources,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = &Txt_Bibliographic_references,
      [Inf_FAQ		] = &Txt_Questions,
      [Inf_LINKS	] = &Txt_Links,
      [Inf_ASSESSMENT	] = NULL,
     };
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
   static Vie_ViewType_t ViewingOrEditingItem[Tre_NUM_LISTING_TYPES] =
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
   static void (*PutIconsListItems[Vie_NUM_VIEW_TYPES]) (void *Node) =
     {
      [Vie_VIEW] = TreSpc_PutIconsEditItem,
      [Vie_EDIT] = TreSpc_PutIconsViewItem,
     };

   /***** Trivial check: node should be valid *****/
   if (Node->Hierarchy.NodCod <= 0)
      return;

   /***** Trivial check: if this info type has no items ==> nothing to do *****/
   if (!GetListItems[Node->InfoType])
      return;

   /***** Get list of node questions & answers from database *****/
   NumItems = (*GetListItems[Node->InfoType]) (&mysql_res,Node->Hierarchy.NodCod,
					       ViewingOrEditingItem[ListingType] == Vie_EDIT ? Lay_SHOW :	// Show hidden items
											       Lay_DONT_SHOW);	// Don't show hidden items

   if (NumItems || ViewingOrEditing[ListingType] == Vie_EDIT)
     {
      if (Node->Hierarchy.NodCod == SelectedNodCod)
	{
	 /***** Begin section *****/
	 HTM_SECTION_Begin (TreSpc_LIST_ITEMS_SECTION_ID);

	    /***** Show possible alerts *****/
	    if (Gbl.Action.Act == ActionsReqRemItem[Node->InfoType])
	      {
	       /* Alert with button to remove question & answer */
	       Ale_ShowLastAlertAndButtonBegin ();
	       Ale_ShowAlertAndButtonEnd (ActionsRemItem[Node->InfoType],
					  TreSpc_LIST_ITEMS_SECTION_ID,NULL,
					  TreSpc_PutParItmCod,&SelectedItmCod,
					  Btn_REMOVE);
	      }
	    else
	       Ale_ShowAlerts (TreSpc_LIST_ITEMS_SECTION_ID);
	}

      /***** Begin box *****/
      ViewingOrEditingListItemsOfThisNode = ViewingOrEditingItem[ListingType] == Vie_EDIT &&
				            Node->Hierarchy.NodCod == SelectedNodCod ? Vie_EDIT :
										       Vie_VIEW;
      switch (ViewingOrEditing[ListingType])
        {
         case Vie_VIEW:
	    Box_BoxBegin (NULL,NULL,NULL,NULL,Box_NOT_CLOSABLE);
	    break;
         case Vie_EDIT:
	    Box_BoxBegin (*Title[Node->InfoType],
			  PutIconsListItems[ViewingOrEditingListItemsOfThisNode],Node,
			  Hlp_COURSE_FAQ,Box_NOT_CLOSABLE);
	    break;
         default:
            Err_WrongTypeExit ();
            break;
        }

	 /***** Table *****/
	 HTM_TABLE_BeginWidePadding (2);

	    /***** Write all items of this node *****/
	    for (NumItem = 0;
		 NumItem < NumItems;
		 NumItem++)
	      {
	       /* Get data of this item */
	       TreSpc_GetItemDataFromRow[Node->InfoType] (mysql_res,Node);

	       /* Show item */
	       switch (ViewingOrEditingListItemsOfThisNode)
		 {
		  case Vie_VIEW:
		     TreSpc_WriteRowViewItem (Node,NumItem);
		     break;
		  case Vie_EDIT:
		     TreSpc_WriteRowEditItem (Node,NumItem,NumItems,
					      ListingType == Tre_EDIT_SPC_ITEM &&
					      Node->Item.Cod == SelectedItmCod ? Vie_EDIT :
										 Vie_VIEW,
					      HiddenOrVisible);
		     break;
		  default:
		     Err_WrongTypeExit ();
		     break;
		 }
	      }

	    /***** Form to create a new question & answer *****/
	    if (ViewingOrEditingListItemsOfThisNode == Vie_EDIT)
	      {
	       TreSpc_ResetItem (Node);
	       TreSpc_WriteRowNewItem (Node,NumItems);
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

static void TreSpc_PutIconsViewItem (void *Node)
  {
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActFrmSeePrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActFrmSeeBibRef,
      [Inf_FAQ		] = ActFrmSeeFAQQaA,
      [Inf_LINKS	] = ActFrmSeeCrsLnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };

   if (Node)
      if (((struct Tre_Node *) Node)->Hierarchy.NodCod > 0)
	 if (Tre_CheckIfICanEditTree () == Usr_CAN)
	    Ico_PutContextualIconToView (Actions[((struct Tre_Node *) Node)->InfoType],
					 TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);
  }

static void TreSpc_PutIconsEditItem (void *Node)
  {
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActFrmEdiPrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActFrmEdiBibRef,
      [Inf_FAQ		] = ActFrmEdiFAQQaA,
      [Inf_LINKS	] = ActFrmEdiCrsLnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };

   if (Node)
      if (((struct Tre_Node *) Node)->Hierarchy.NodCod > 0)
	 if (Tre_CheckIfICanEditTree () == Usr_CAN)
	    Ico_PutContextualIconToEdit (Actions[((struct Tre_Node *) Node)->InfoType],
					 TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);
  }

/*****************************************************************************/
/*************************** Write row to view item **************************/
/*****************************************************************************/

static void TreSpc_WriteRowViewItem (struct Tre_Node *Node,unsigned NumItem)
  {
   static void (*WriteCell[Inf_NUM_TYPES]) (struct Tre_Node *Node) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_WriteCellViewRsc,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = Bib_WriteCellViewRef,
      [Inf_FAQ		] = FAQ_WriteCellViewQaA,
      [Inf_LINKS	] = Lnk_WriteCellViewLnk,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Question&Answer number *****/
      HTM_TD_Begin ("class=\"TRE_NUM ITM_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumItem + 1);
      HTM_TD_End ();

      /***** Question&Answer *****/
      HTM_TD_Begin ("class=\"TRE_MAIN ITM_%s\"",The_GetSuffix ());

	 /* Wite cell contents */
         if (WriteCell[Node->InfoType])
	    WriteCell[Node->InfoType] (Node);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Write row to edit item **************************/
/*****************************************************************************/

static void TreSpc_WriteRowEditItem (struct Tre_Node *Node,
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
      [Inf_PROGRAM	] = PrgRsc_WriteCellEditRsc,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = Bib_WriteCellEditRef,
      [Inf_FAQ		] = FAQ_WriteCellEditQaA,
      [Inf_LINKS	] = Lnk_WriteCellEditLnk,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this item *****/
      HTM_TD_Begin ("class=\"ITM_COL1 LT\"");
         TreSpc_PutFormsToEditItem (Node,NumItem,NumItems);
      HTM_TD_End ();

      /***** Item number *****/
      HTM_TD_Begin ("class=\"TRE_NUM ITM_%s\"",The_GetSuffix ());
	 HTM_Unsigned (NumItem + 1);
      HTM_TD_End ();

      /***** Item content *****/
      HTM_TD_Begin ("class=\"TRE_MAIN ITM_%s\"",The_GetSuffix ());

	 /* Write cell contents */
         if (WriteCell[Node->InfoType])
	    WriteCell[Node->InfoType] (Node,ViewType,HiddenOrVisible);

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/*************************** Write row for new item **************************/
/*****************************************************************************/

static void TreSpc_WriteRowNewItem (struct Tre_Node *Node,unsigned NumItems)
  {
   static Act_Action_t ActionsNewItem[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUnk,
      [Inf_PROGRAM	] = ActNewPrgRsc,
      [Inf_TEACH_GUIDE	] = ActUnk,
      [Inf_SYLLABUS_LEC	] = ActUnk,
      [Inf_SYLLABUS_PRA	] = ActUnk,
      [Inf_BIBLIOGRAPHY	] = ActNewBibRef,
      [Inf_FAQ		] = ActNewFAQQaA,
      [Inf_LINKS	] = ActNewCrsLnk,
      [Inf_ASSESSMENT	] = ActUnk,
     };
   static void (*WriteCell[Inf_NUM_TYPES]) (void) =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = PrgRsc_WriteCellNewRsc,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = Bib_WriteCellNewRef,
      [Inf_FAQ		] = FAQ_WriteCellNewQaA,
      [Inf_LINKS	] = Lnk_WriteCellNewLnk,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this question & answer *****/
      HTM_TD_Begin ("class=\"ITM_COL1 LT %s\"",The_GetColorRows1 (1));
	 TreSpc_PutFormsToEditItem (Node,NumItems,NumItems);
      HTM_TD_End ();

      /***** Item number *****/
      HTM_TD_Begin ("class=\"TRE_NUM ITM_%s %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumItems + 1);
      HTM_TD_End ();

      /***** Item content *****/
      HTM_TD_Begin ("class=\"TRE_MAIN %s\"",The_GetColorRows1 (1));

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

static void TreSpc_PutFormsToEditItem (struct Tre_Node *Node,
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
      [Inf_BIBLIOGRAPHY	] = ActReqRemBibRef,
      [Inf_FAQ		] = ActReqRemFAQQaA,
      [Inf_LINKS	] = ActReqRemCrsLnk,
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
      [Inf_BIBLIOGRAPHY	][HidVis_HIDDEN ] = ActUnhBibRef,	// Hidden ==> action to unhide
      [Inf_BIBLIOGRAPHY	][HidVis_VISIBLE] = ActHidBibRef,	// Visible ==> action to hide
      [Inf_FAQ		][HidVis_HIDDEN ] = ActUnhFAQQaA,	// Hidden ==> action to unhide
      [Inf_FAQ		][HidVis_VISIBLE] = ActHidFAQQaA,	// Visible ==> action to hide
      [Inf_LINKS	][HidVis_HIDDEN ] = ActUnhCrsLnk,	// Hidden ==> action to unhide
      [Inf_LINKS	][HidVis_VISIBLE] = ActHidCrsLnk,	// Visible ==> action to hide
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
      [Inf_BIBLIOGRAPHY	] = ActFrmChgBibRef,
      [Inf_FAQ		] = ActFrmChgFAQQaA,
      [Inf_LINKS	] = ActFrmChgCrsLnk,
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
      [Inf_BIBLIOGRAPHY	][TreSpc_UP  ] = ActUp_BibRef,
      [Inf_BIBLIOGRAPHY	][TreSpc_DOWN] = ActDwnBibRef,
      [Inf_FAQ		][TreSpc_UP  ] = ActUp_FAQQaA,
      [Inf_FAQ		][TreSpc_DOWN] = ActDwnFAQQaA,
      [Inf_LINKS	][TreSpc_UP  ] = ActUp_CrsLnk,
      [Inf_LINKS	][TreSpc_DOWN] = ActDwnCrsLnk,
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
					   TreSpc_PutParItmCod,&Node->Item.Cod);
	 else
	    Ico_PutIconRemovalNotAllowed ();

	 /***** Icon to hide/unhide question & answer *****/
	 if (NumItem < NumItems)
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide[Node->InfoType],
					       TreSpc_LIST_ITEMS_SECTION_ID,
					       TreSpc_PutParItmCod,&Node->Item.Cod,
					       Node->Item.Hidden);
	 else
	    Ico_PutIconOff ("eye.svg",Ico_GREEN,Txt_Visible);

	 /***** Put icon to edit the question & answer *****/
	 if (NumItem < NumItems)
	    Ico_PutContextualIconToEdit (ActionsFrmChg[Node->InfoType],
					 TreSpc_LIST_ITEMS_SECTION_ID,
					 TreSpc_PutParItmCod,&Node->Item.Cod);
	 else
	    Ico_PutContextualIconToEdit (ActionsFrmChg[Node->InfoType],
					 TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);

	 /***** Icon to move up the question & answer *****/
	 if (NumItem > 0 && NumItem < NumItems)
	    Lay_PutContextualLinkOnlyIcon (ActionUpDown[Node->InfoType][TreSpc_UP],
					   TreSpc_LIST_ITEMS_SECTION_ID,
	                                   TreSpc_PutParItmCod,&Node->Item.Cod,
	 			           "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the question & answer *****/
	 if (NumItem < NumItems - 1)
	    Lay_PutContextualLinkOnlyIcon (ActionUpDown[Node->InfoType][TreSpc_DOWN],
					   TreSpc_LIST_ITEMS_SECTION_ID,
	                                   TreSpc_PutParItmCod,&Node->Item.Cod,
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
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_END_EDIT_SPC_LIST_ITEMS,&Node);

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
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_SPC_LIST_ITEMS,&Node);

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
   Tre_ShowAllNodes (Tre_EDIT_SPC_ITEM,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********************** Create/update specific list item *********************/
/*****************************************************************************/

void TreSpc_ChangeItem (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;
   static struct
     {
      void (*GetPars) (struct Tre_Node *Node);
      long (*CreItem) (const struct Tre_Node *Node);
      void (*UpdItem) (const struct Tre_Node *Node);
     } Functions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = {NULL		,NULL			,NULL			},
      [Inf_INFORMATION	] = {NULL		,NULL			,NULL			},
      [Inf_PROGRAM	] = {PrgRsc_GetParsRsc	,Rsc_DB_CreateRsc	,Rsc_DB_UpdateRsc	},
      [Inf_TEACH_GUIDE	] = {NULL		,NULL			,NULL			},
      [Inf_SYLLABUS_LEC	] = {NULL		,NULL			,NULL			},
      [Inf_SYLLABUS_PRA	] = {NULL		,NULL			,NULL			},
      [Inf_BIBLIOGRAPHY	] = {Bib_GetParsRef	,Bib_DB_CreateRef	,Bib_DB_UpdateRef	},
      [Inf_FAQ		] = {FAQ_GetParsQaA	,FAQ_DB_CreateQaA	,FAQ_DB_UpdateQaA	},
      [Inf_LINKS	] = {Lnk_GetParsLnk	,Lnk_DB_CreateLnk	,Lnk_DB_UpdateLnk	},
      [Inf_ASSESSMENT	] = {NULL		,NULL			,NULL			}
     };

   /***** Check info type *****/
   if (!Functions[InfoType].GetPars ||
       !Functions[InfoType].CreItem ||
       !Functions[InfoType].UpdItem)
      Err_WrongTypeExit ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get parameters *****/
   /* Get tree node */
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /* Get parameters for specific list item */
   Functions[InfoType].GetPars (&Node);

   /***** Is it an existing item? *****/
   if (Node.Item.Cod > 0)
      Functions[InfoType].UpdItem (&Node);	// Update item
   else
      Functions[InfoType].CreItem (&Node);	// Create item

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Tre_EDIT_SPC_LIST_ITEMS,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********** Ask for confirmation of removing an specific list item ***********/
/*****************************************************************************/

void TreSpc_ReqRemItem (Inf_Type_t InfoType)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_item_X;
   static struct Tre_Node Node;
   static const char *ItemTitles[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = Node.Item.Rsc.Title,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = Node.Item.Bib.Fields[Bib_TITLE],
      [Inf_FAQ		] = Node.Item.QaA.Question,
      [Inf_LINKS	] = Node.Item.Lnk.Fields[Lnk_TITLE],
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and specific list item *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Create alert to remove the specific list item *****/
   if (ItemTitles[InfoType] == NULL)
      Err_WrongTypeExit ();
   Ale_CreateAlert (Ale_QUESTION,TreSpc_LIST_ITEMS_SECTION_ID,
		    Txt_Do_you_really_want_to_remove_the_item_X,
		    ItemTitles[InfoType]);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Tre_EDIT_SPC_LIST_ITEMS,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********************** Remove an specific list item ************************/
/*****************************************************************************/

void TreSpc_RemoveItem (Inf_Type_t InfoType)
  {
   extern const char *Txt_Resource_removed;
   extern const char *Txt_Bibliographic_reference_removed;
   extern const char *Txt_Question_removed;
   extern const char *Txt_Link_removed;
   struct Tre_Node Node;
   static const char **Txt[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = NULL,
      [Inf_INFORMATION	] = NULL,
      [Inf_PROGRAM	] = &Txt_Resource_removed,
      [Inf_TEACH_GUIDE	] = NULL,
      [Inf_SYLLABUS_LEC	] = NULL,
      [Inf_SYLLABUS_PRA	] = NULL,
      [Inf_BIBLIOGRAPHY	] = &Txt_Bibliographic_reference_removed,
      [Inf_FAQ		] = &Txt_Question_removed,
      [Inf_LINKS	] = &Txt_Link_removed,
      [Inf_ASSESSMENT	] = NULL,
     };

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get data of the specific list item from database *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Remove specific list item *****/
   Tre_DB_RemoveItem (&Node);

   /***** Create success alert *****/
   if (Txt[InfoType])
      Ale_CreateAlert (Ale_SUCCESS,TreSpc_LIST_ITEMS_SECTION_ID,*Txt[InfoType]);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Tre_EDIT_SPC_LIST_ITEMS,&Node);

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

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and item *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Hide/unhide specific list item *****/
   Tre_DB_HideOrUnhideItem (&Node,HiddenOrVisible);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Tre_EDIT_SPC_LIST_ITEMS,&Node);

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
   Err_SuccessOrError_t SuccessOrError = Err_ERROR;
   static unsigned (*GetOtherInd[TreSpc_NUM_UP_DOWN])(const struct Tre_Node *Node) =
     {
      [TreSpc_UP  ] = Tre_DB_GetItmIndBefore,
      [TreSpc_DOWN] = Tre_DB_GetItmIndAfter,
     };

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node and resource *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Move up/down list item *****/
   if ((Item2.Ind = GetOtherInd[UpDown] (&Node)))	// 0 ==> movement not allowed
     {
      /* Get the other list item code */
      Item2.Cod = Tre_DB_GetItmCodFromItmInd (&Node,Item2.Ind);

      /* Exchange items */
      SuccessOrError = TreSpc_ExchangeItems (&Node,&Item2);
     }
   if (SuccessOrError == Err_ERROR)
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Tre_EDIT_SPC_LIST_ITEMS,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********** Exchange the order of two consecutive questions & answers **********/
/*****************************************************************************/
// Return Err_SUCCESS if success

static Err_SuccessOrError_t TreSpc_ExchangeItems (const struct Tre_Node *Node,
						  const struct Tre_SpcItem *Item2)
  {
   const struct Tre_SpcItem *Item1 = &Node->Item;

   if (Item1->Ind > 0 &&	// Indexes should be in the range [1, 2,...]
       Item2->Ind > 0)
     {
      /***** Lock tables to make the move atomic *****/
      Tre_DB_LockTables (Node->InfoType);

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
		 necessary to preserve unique index (NodCod,ItmInd) */
      Tre_DB_UpdateItmInd (Node,Item2->Cod,-(int) Item2->Ind);

      /* Step 2: Change first index */
      Tre_DB_UpdateItmInd (Node,Item1->Cod, (int) Item2->Ind);

      /* Step 3: Change second index */
      Tre_DB_UpdateItmInd (Node,Item2->Cod, (int) Item1->Ind);

      /***** Unlock tables *****/
      DB_UnlockTables ();

      return Err_SUCCESS;	// Success
     }

   return Err_ERROR;		// No success
  }
