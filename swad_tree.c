// swad_tree.c: course tree

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_action_list.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_contracted_expanded.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_FAQ.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_program.h"
#include "swad_program_resource.h"
#include "swad_resource.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_string.h"
#include "swad_tree.h"
#include "swad_tree_database.h"
#include "swad_tree_specific.h"
#include "swad_view.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Level
  {
   unsigned Number;	// Numbers for each level from 1 to maximum level
   ConExp_ContractedOrExpanded_t ContractedOrExpanded;	// If each level from 1 to maximum level is expanded
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
      bool IsRead;			// Is the list already read from database...
					// ...or it needs to be read?
      unsigned NumNodes;		// Number of nodes
      struct Tre_NodeHierarchy *Nodes;	// List of nodes
     } List;
   unsigned MaxLevel;		// Maximum level of nodes
   struct Level *Levels;	// Numbers and hidden for each level from 1 to maximum level
  } Tre_Gbl =
  {
   .List =
     {
      .IsRead     = false,
      .NumNodes   = 0,
      .Nodes      = NULL,
     },
   .MaxLevel      = 0,
   .Levels        = NULL
  };

static const char *Tre_NODE_SECTION_ID = "node_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Tre_WriteRowNode (Tre_ListingType_t ListingType,
                              unsigned NumNode,struct Tre_Node *Node,
                              ConExp_ContractedOrExpanded_t ContractedOrExpanded,
                              long SelectedNodCod,
                              long SelectedItmCod);	// Specific code (resource, question,...)
static void Tre_PutIconToContractOrExpandNode (struct Tre_Node *Node,
                                               ConExp_ContractedOrExpanded_t ContractedOrExpanded,
                                               Vie_ViewType_t ViewType);
static void Tre_WriteNodeText (const struct Tre_Node *Node,
			       HidVis_HiddenOrVisible_t HiddenOrVisible);
static void Tre_WriteRowToCreateNode (Inf_Type_t InfoType,
				      long ParentNodCod,unsigned FormLevel);
static void Tre_SetTitleClass (char **TitleClass,unsigned Level);
static void Tre_FreeTitleClass (char *TitleClass);

static void Tre_SetMaxNodeLevel (unsigned Level);
static unsigned Tre_GetMaxNodeLevel (void);
static unsigned Tre_CalculateMaxNodeLevel (void);
static Exi_Exist_t Tre_GetIfNodeHasChildren (unsigned NumNode);
static void Tre_CreateLevels (void);
static void Tre_FreeLevels (void);
static void Tre_IncreaseNumberInLevel (unsigned Level);
static unsigned Tre_GetCurrentNumberInLevel (unsigned Level);

static void Tre_SetExpandedLevel (unsigned Level,
				  ConExp_ContractedOrExpanded_t ContractedOrExpanded);
static void Tre_SetHiddenLevel (unsigned Level,
				HidVis_HiddenOrVisible_t HiddenOrVisible);
static ConExp_ContractedOrExpanded_t Tre_GetExpandedLevel (unsigned Level);
static HidVis_HiddenOrVisible_t Tre_GetHiddenLevel (unsigned Level);

static bool Tre_CheckIfAllHigherLevelsAreExpanded (unsigned CurrentLevel);
static HidVis_HiddenOrVisible_t Tre_CheckIfAnyHigherLevelIsHidden (unsigned CurrentLevel);

static void Tre_PutFormsToRemEditOneNode (Tre_ListingType_t ListingType,
                                          unsigned NumNode,
                                          struct Tre_Node *Node,
                                          bool HighlightNode);
static bool Tre_CheckIfMoveUpIsAllowed (unsigned NumNode);
static bool Tre_CheckIfMoveDownIsAllowed (unsigned NumNode);
static bool Tre_CheckIfMoveLeftIsAllowed (unsigned NumNode);
static bool Tre_CheckIfMoveRightIsAllowed (unsigned NumNode);

static void Tre_GetNodeDataByCod (struct Tre_Node *Node);
static void Tre_GetNodeDataFromRow (MYSQL_RES **mysql_res,
                                    struct Tre_Node *Node,
                                    Exi_Exist_t NodeExists);

static Err_SuccessOrError_t Tre_ExchangeNodeRanges (Inf_Type_t InfoType,
						    int NumNodeTop,int NumNodeBottom);
static int Tre_GetPrevBrother (int NumNode);
static int Tre_GetNextBrother (int NumNode);

static void Tre_SetNodeRangeWithAllChildren (unsigned NumNode,struct Tre_NodeRange *NodeRange);
static unsigned Tre_GetLastChild (int NumNode);

static void Tre_ShowFormToCreateNode (Inf_Type_t InfoType,long ParentNodCod);
static void Tre_ShowFormToChangeNode (struct Tre_Node *Node);
static void Tre_ShowFormNode (const struct Tre_Node *Node,
			      const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME],
			      const char *Txt);
static void Tre_GetNodeDataFromForm (struct Tre_Node *Node,
				     char Description[Cns_MAX_BYTES_TEXT + 1]);
static void Tre_InsertNode (const struct Tre_Node *ParentNode,
		            struct Tre_Node *NewNode,const char *Txt);

/*****************************************************************************/
/**************************** List all tree nodes ****************************/
/*****************************************************************************/

int Tre_WriteTreeIntoHTMLBuffer (Inf_Type_t InfoType)
  {
   char FileNameHTMLTmp[PATH_MAX + 1];
   FILE *FileHTMLTmp;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);
   if (Tre_GetNumNodes ())
     {
      /***** Create a unique name for the file *****/
      snprintf (FileNameHTMLTmp,sizeof (FileNameHTMLTmp),"%s/%s_tree.html",
	        Cfg_PATH_OUT_PRIVATE,Cry_GetUniqueNameEncrypted ());

      /***** Create a new temporary file for writing and reading *****/
      if ((FileHTMLTmp = fopen (FileNameHTMLTmp,"w+b")) == NULL)
	{
	 Tre_FreeListNodes ();
         return -1;
	}

      /***** Write syllabus in HTML into a temporary file *****/
      Tre_WriteTreeIntoHTMLTmpFile (InfoType,FileHTMLTmp);

      /***** Close and remove temporary file *****/
      fclose (FileHTMLTmp);
      // unlink (FileNameHTMLTmp);
     }

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();

   return 0;
  }

/*****************************************************************************/
/**************************** List all tree nodes ****************************/
/*****************************************************************************/
// Return number of nodes in tree

unsigned Tre_ShowTree (Inf_Type_t InfoType)
  {
   unsigned NumNodes;
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);
   NumNodes = Tre_Gbl.List.NumNodes;

   /***** Show course program without highlighting any node *****/
   Node.InfoType = InfoType;
   Node.Hierarchy.NodCod = -1L;
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_VIEW,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();

   return NumNodes;
  }

void Tre_EditTree (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Show course program without highlighting any node *****/
   Node.InfoType = InfoType;
   Node.Hierarchy.NodCod = -1L;
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_NODES,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*************************** Show all tree nodes *****************************/
/*****************************************************************************/

void Tre_ShowAllNodes (Tre_ListingType_t ListingType,
		       const struct Tre_Node *SelectedNode)
  {
   long ParentNodCod = -1L;	// Initialized to avoid warning
   unsigned NumNode;
   unsigned FormLevel = 0;	// Initialized to avoid warning
   struct Tre_Node Node;
   ConExp_ContractedOrExpanded_t ContractedOrExpanded;
   Inf_Type_t InfoType = SelectedNode->InfoType;
   long SelectedNodCod = SelectedNode->Hierarchy.NodCod;
   long SelectedItmCod = SelectedNode->Item.Cod;	// Specific code (resource, question,...)

   /***** Trivial check: tree type must be valid *****/
   if (InfoType < (Inf_Type_t) 1 ||
       InfoType > (Inf_Type_t) (Inf_NUM_TYPES - 1))
      Err_WrongTypeExit ();

   /***** Create levels *****/
   Tre_SetMaxNodeLevel (Tre_CalculateMaxNodeLevel ());
   Tre_CreateLevels ();

   /***** Compute form level *****/
   if (ListingType == Tre_FORM_NEW_CHILD_NODE)
     {
      ParentNodCod = SelectedNodCod;	// Node code here is parent of the node to create
      NumNode = Tre_GetNumNodeFromNodCod (SelectedNodCod);
      SelectedNodCod = Tre_GetNodCodFromNumNode (Tre_GetLastChild (NumNode));
      FormLevel = Tre_GetLevelFromNumNode (NumNode) + 1;
     }

   /***** Table *****/
   HTM_TABLE_Begin ("TBL_SCROLL CELLS_PAD_2");

      /***** Write all tree nodes *****/
      for (NumNode = 0, The_ResetRowColor ();
	   NumNode < Tre_Gbl.List.NumNodes;
	   NumNode++)
	{
	 /* Get data of this tree node */
	 Node.InfoType = InfoType;
	 Node.Hierarchy.NodCod = Tre_GetNodCodFromNumNode (NumNode);
	 Tre_GetNodeDataByCod (&Node);

	 /* Set if this level is expanded */
	 ContractedOrExpanded = Tre_DB_GetIfContractedOrExpandedNode (Node.Hierarchy.NodCod);
	 Tre_SetExpandedLevel (Node.Hierarchy.Level,ContractedOrExpanded);

	 /* Show this row only if all higher levels are expanded */
	 if (Tre_CheckIfAllHigherLevelsAreExpanded (Node.Hierarchy.Level))
	   {
	    /* Write row with this node */
	    Tre_WriteRowNode (ListingType,NumNode,&Node,ContractedOrExpanded,
			      SelectedNodCod,SelectedItmCod);
	    The_ChangeRowColor ();

	    /* Show form to create child node? */
	    if (ListingType == Tre_FORM_NEW_CHILD_NODE &&
		Node.Hierarchy.NodCod == SelectedNodCod)
	      {
	       Tre_WriteRowToCreateNode (InfoType,ParentNodCod,FormLevel);
	       The_ChangeRowColor ();
	      }
	   }
	}

      /***** Create node at the end? *****/
      if (ListingType == Tre_FORM_NEW_END_NODE)
	 Tre_WriteRowToCreateNode (InfoType,-1L,1);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free levels *****/
   Tre_FreeLevels ();
  }

/*****************************************************************************/
/******************* Check if I can create tree nodes *********************/
/*****************************************************************************/

Usr_Can_t Tre_CheckIfICanEditTree (void)
  {
   return (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
           Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM) ? Usr_CAN :
        					     Usr_CAN_NOT;
  }

/*****************************************************************************/
/************************** Put icon to edit tree ****************************/
/*****************************************************************************/

void Tre_PutIconToEditTree (struct Tre_Node *Node)
  {
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActEdiTreInf,
      [Inf_PROGRAM	] = ActEdiTrePrg,
      [Inf_TEACH_GUIDE	] = ActEdiTreGui,
      [Inf_SYLLABUS_LEC	] = ActEdiTreSyl,
      [Inf_SYLLABUS_PRA	] = ActEdiTreSyl,
      [Inf_BIBLIOGRAPHY	] = ActEdiTreBib,
      [Inf_FAQ		] = ActEdiTreFAQ,
      [Inf_LINKS	] = ActEdiTreLnk,
      [Inf_ASSESSMENT	] = ActEdiTreAss,
     };

   Ico_PutContextualIconToEdit (Actions[Node->InfoType],NULL,
				Tre_PutPars,Node);
  }

/*****************************************************************************/
/************************** Put icon to view tree ****************************/
/*****************************************************************************/

void Tre_PutIconToViewTree (struct Tre_Node *Node)
  {
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActSeeCrsInf,
      [Inf_PROGRAM	] = ActSeePrg,
      [Inf_TEACH_GUIDE	] = ActSeeTchGui,
      [Inf_SYLLABUS_LEC	] = ActSeeSyl,
      [Inf_SYLLABUS_PRA	] = ActSeeSyl,
      [Inf_BIBLIOGRAPHY	] = ActSeeBib,
      [Inf_FAQ		] = ActSeeFAQ,
      [Inf_LINKS	] = ActSeeCrsLnk,
      [Inf_ASSESSMENT	] = ActSeeAss,
     };

   Ico_PutContextualIconToView (Actions[Node->InfoType],NULL,
				Tre_PutPars,Node);
  }

/*****************************************************************************/
/******************** Put icon to create a new tree node *********************/
/*****************************************************************************/

void Tre_PutIconToCreateNewNode (struct Tre_Node *Node)
  {
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActFrmNewTreNodInf,
      [Inf_PROGRAM	] = ActFrmNewTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActFrmNewTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActFrmNewTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActFrmNewTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActFrmNewTreNodBib,
      [Inf_FAQ		] = ActFrmNewTreNodFAQ,
      [Inf_LINKS	] = ActFrmNewTreNodLnk,
      [Inf_ASSESSMENT	] = ActFrmNewTreNodAss,
     };

   Ico_PutContextualIconToAdd (Actions[Node->InfoType],Tre_NODE_SECTION_ID,
                               Tre_PutPars,Node);
  }

/*****************************************************************************/
/**************************** Show one tree node *****************************/
/*****************************************************************************/

static void Tre_WriteRowNode (Tre_ListingType_t ListingType,
                              unsigned NumNode,struct Tre_Node *Node,
                              ConExp_ContractedOrExpanded_t ContractedOrExpanded,
                              long SelectedNodCod,
                              long SelectedItmCod)	// Specific code (resource, question,...)
  {
   extern const char *CloOpe_Class[CloOpe_NUM_CLOSED_OPEN][HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];
   static unsigned UniqueId = 0;
   static Vie_ViewType_t ViewingOrEditingProgram[Tre_NUM_LISTING_TYPES] =
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
   bool PutIconExpandContract;
   HidVis_HiddenOrVisible_t HiddenOrVisible;
   char *Id;
   unsigned ColSpan;
   unsigned NumCol;
   char *TitleClass;
   Dat_StartEndTime_t StartEndTime;
   bool HighlightNode;

   /***** Check if icon expand/contract is necessary *****/
   PutIconExpandContract = (Tre_GetIfNodeHasChildren (NumNode) == Exi_EXISTS);
   if (!PutIconExpandContract)
     {
      PutIconExpandContract = (Tre_DB_CheckIfNodeHasTxt (Node) == Exi_EXISTS);
      if (!PutIconExpandContract)
	 PutIconExpandContract = (Tre_DB_CheckListItems (Node,
							 ViewingOrEditingProgram[ListingType] == Vie_EDIT) == Exi_EXISTS);
     }

   /***** Check if this node should be shown as hidden *****/
   Tre_SetHiddenLevel (Node->Hierarchy.Level,Node->Hierarchy.Hidden);
   switch (Node->Hierarchy.Hidden)
     {
      case HidVis_VISIBLE:	// this node is not marked as hidden
         HiddenOrVisible = Tre_CheckIfAnyHigherLevelIsHidden (Node->Hierarchy.Level);
	 break;
      case HidVis_HIDDEN:	// this node is marked as hidden
      default:
         HiddenOrVisible = HidVis_HIDDEN;
	 break;
     }

   if (ViewingOrEditingProgram[ListingType] == Vie_EDIT ||
       HiddenOrVisible == HidVis_VISIBLE)
     {
      /***** Increase number in level *****/
      Tre_IncreaseNumberInLevel (Node->Hierarchy.Level);

      /***** Is this the node selected? *****/
      HighlightNode = (Node->Hierarchy.NodCod == SelectedNodCod);

      /***** First row (title and dates) *****/
      HTM_TR_Begin (NULL);

	 /* Title CSS class */
	 Tre_SetTitleClass (&TitleClass,Node->Hierarchy.Level);

	 /* Indent depending on the level */
	 for (NumCol = 1;
	      NumCol < Node->Hierarchy.Level;
	      NumCol++)
	   {
	    HTM_TD_Begin ("class=\"TRE_COL1 %s\" rowspan=\"2\"",The_GetColorRows ());
	    HTM_TD_End ();
	   }

	 /* Expand/contract this tree node */
	 HTM_TD_Begin ("class=\"TRE_COL1 RT %s\" rowspan=\"2\"",The_GetColorRows ());
	    if (PutIconExpandContract)
	       Tre_PutIconToContractOrExpandNode (Node,ContractedOrExpanded,
						  ViewingOrEditingProgram[ListingType]);
	 HTM_TD_End ();

	 /* Forms to remove/edit this tree node */
	 if (ViewingOrEditingProgram[ListingType] == Vie_EDIT)
	   {
	    HTM_TD_Begin ("class=\"TRE_COL1 LT %s\" rowspan=\"2\"",The_GetColorRows ());
	       Tre_PutFormsToRemEditOneNode (ListingType,NumNode,Node,HighlightNode);
	    HTM_TD_End ();
	   }

	 /* Node number */
	 HTM_TD_Begin ("class=\"TRE_NUM %s\" rowspan=\"2\"",The_GetColorRows ());
	    HTM_DIV_Begin ("class=\"%s%s\"",
			   TitleClass,HidVis_TreeClass[HiddenOrVisible]);
	       HTM_Unsigned (Tre_GetCurrentNumberInLevel (Node->Hierarchy.Level));
	    HTM_DIV_End ();
	 HTM_TD_End ();

	 /* Title */
	 /*___________________________________________________________
	  |   |   |                                                   |
	  | v | 1 | Lesson 1                 (colspan = 3+2-1 = 4)    |
	  |___|___|___________________________________________________|
          |   |   |   |                                               |
	  |   | v | 1 | Lesson 1.1           (colspan = 3+2-2 = 3)    |
	  |___|___|___|_______________________________________________|
          |   |   |   |   |                                           |
	  |   |   | v | 1 | Lesson 1.1.1     (colspan = 3+2-3 = 2)    |
	  |___|___|___|___|___________________________________________|
          |   |   |   |   |   |                                       |
	  |   |   |   |   | 1 |  New node    (colspan = 3+2-4 = 1)    |
	  |___|___|___|___|___|_______________________________________|
          |   |   |   |                                               |
	  |   | v | 2 | Lesson 1.2           (colspan = 3+2-2 = 3)    |
	  |___|___|___|_______________________________________________|
	  */
	 ColSpan = Tre_GetMaxNodeLevel () + 2 - Node->Hierarchy.Level;
	 switch (ListingType)
	   {
	    case Tre_PRINT:
	       HTM_TD_Begin ("colspan=\"%u\" class=\"TRE_MAIN\"",ColSpan);
	       break;
	    default:
	       HTM_TD_Begin ("colspan=\"%u\" class=\"TRE_MAIN %s\"",ColSpan,
			     The_GetColorRows ());

	       break;
	   }

	    if (HighlightNode)
	       HTM_ARTICLE_Begin (Tre_NODE_SECTION_ID);
	    HTM_DIV_Begin ("class=\"LT %s%s\"",
			   TitleClass,HidVis_TreeClass[HiddenOrVisible]);
	       HTM_Txt (Node->Title);
	    HTM_DIV_End ();
	    if (HighlightNode)
	       HTM_ARTICLE_End ();
	 HTM_TD_End ();

	 /* Free title CSS class */
	 Tre_FreeTitleClass (TitleClass);

	 if (Node->InfoType == Inf_PROGRAM)
	   {
	    /* Start/end date/time */
	    switch (ListingType)
	      {
	       case Tre_PRINT:
		  HTM_TD_Begin ("class=\"TRE_DATE RT\"");
		  break;
	       default:
		  HTM_TD_Begin ("class=\"TRE_DATE RT %s\"",The_GetColorRows ());
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
			      CloOpe_Class[Node->ClosedOrOpen][HidVis_VISIBLE],The_GetSuffix (),
			      HidVis_TreeClass[HiddenOrVisible]);
		  Dat_WriteLocalDateHMSFromUTC (Id,Node->TimeUTC[StartEndTime],
						Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
						Dat_WRITE_TODAY |
						Dat_WRITE_DATE_ON_SAME_DAY |
						Dat_WRITE_HOUR |
						Dat_WRITE_MINUTE);
	       HTM_DIV_End ();
	       free (Id);
	      }

	    HTM_TD_End ();
	   }

      HTM_TR_End ();

      /***** Second row (text and specific content) *****/
      HTM_TR_Begin (NULL);

	 /* Begin text and specific content */
	 if (Node->InfoType == Inf_PROGRAM)
	    ColSpan++;
	 switch (ListingType)
	   {
	    case Tre_PRINT:
	       HTM_TD_Begin ("colspan=\"%u\" class=\"TRE_MAIN\"",ColSpan);
	       break;
	    default:
	       HTM_TD_Begin ("colspan=\"%u\" class=\"TRE_MAIN %s\"",ColSpan,
			     The_GetColorRows ());
	       break;
	   }

	 if (ContractedOrExpanded == ConExp_EXPANDED)
	   {
	    /* Item text / form */
	    if (ListingType == Tre_FORM_EDIT_NODE && HighlightNode)
	       /* Form to change node title, dates and text */
	       Tre_ShowFormToChangeNode (Node);
	    else
	       /* Text */
	       Tre_WriteNodeText (Node,HiddenOrVisible);

	    /* List of items of this tree node (specific content depending on the tree type) */
	    TreSpc_ListNodeItems (ListingType,Node,SelectedNodCod,SelectedItmCod,
				  HiddenOrVisible);
	   }

	 /* End text and specific content */
	 HTM_TD_End ();

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/************************ Put icon to expand a node **************************/
/*****************************************************************************/

static void Tre_PutIconToContractOrExpandNode (struct Tre_Node *Node,
                                               ConExp_ContractedOrExpanded_t ContractedOrExpanded,
                                               Vie_ViewType_t ViewType)
  {
   static Act_Action_t Actions[Inf_NUM_TYPES][ConExp_NUM_CONTRACTED_EXPANDED][Vie_NUM_VIEW_TYPES] =
     {
      [Inf_INFORMATION	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodInf,	// Contracted, Not editing ==> action to expand
      [Inf_INFORMATION	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodInf,	// Contracted,     Editing ==> action to expand
      [Inf_INFORMATION	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodInf,	// Expanded  , Not editing ==> action to contract
      [Inf_INFORMATION	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodInf,	// Expanded  ,     Editing ==> action to contract

      [Inf_PROGRAM	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodPrg,	// Contracted, Not editing ==> action to expand
      [Inf_PROGRAM	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodPrg,	// Contracted,     Editing ==> action to expand
      [Inf_PROGRAM	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodPrg,	// Expanded  , Not editing ==> action to contract
      [Inf_PROGRAM	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodPrg,	// Expanded  ,     Editing ==> action to contract

      [Inf_TEACH_GUIDE	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodGui,	// Contracted, Not editing ==> action to expand
      [Inf_TEACH_GUIDE	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodGui,	// Contracted,     Editing ==> action to expand
      [Inf_TEACH_GUIDE	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodGui,	// Expanded  , Not editing ==> action to contract
      [Inf_TEACH_GUIDE	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodGui,	// Expanded  ,     Editing ==> action to contract

      [Inf_SYLLABUS_LEC	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodSyl,	// Contracted, Not editing ==> action to expand
      [Inf_SYLLABUS_LEC	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodSyl,	// Contracted,     Editing ==> action to expand
      [Inf_SYLLABUS_LEC	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodSyl,	// Expanded  , Not editing ==> action to contract
      [Inf_SYLLABUS_LEC	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodSyl,	// Expanded  ,     Editing ==> action to contract

      [Inf_SYLLABUS_PRA	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodSyl,	// Contracted, Not editing ==> action to expand
      [Inf_SYLLABUS_PRA	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodSyl,	// Contracted,     Editing ==> action to expand
      [Inf_SYLLABUS_PRA	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodSyl,	// Expanded  , Not editing ==> action to contract
      [Inf_SYLLABUS_PRA	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodSyl,	// Expanded  ,     Editing ==> action to contract

      [Inf_BIBLIOGRAPHY	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodBib,	// Contracted, Not editing ==> action to expand
      [Inf_BIBLIOGRAPHY	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodBib,	// Contracted,     Editing ==> action to expand
      [Inf_BIBLIOGRAPHY	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodBib,	// Expanded  , Not editing ==> action to contract
      [Inf_BIBLIOGRAPHY	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodBib,	// Expanded  ,     Editing ==> action to contract

      [Inf_FAQ		][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodFAQ,	// Contracted, Not editing ==> action to expand
      [Inf_FAQ		][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodFAQ,	// Contracted,     Editing ==> action to expand
      [Inf_FAQ		][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodFAQ,	// Expanded  , Not editing ==> action to contract
      [Inf_FAQ		][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodFAQ,	// Expanded  ,     Editing ==> action to contract

      [Inf_LINKS	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodLnk,	// Contracted, Not editing ==> action to expand
      [Inf_LINKS	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodLnk,	// Contracted,     Editing ==> action to expand
      [Inf_LINKS	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodLnk,	// Expanded  , Not editing ==> action to contract
      [Inf_LINKS	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodLnk,	// Expanded  ,     Editing ==> action to contract

      [Inf_ASSESSMENT	][ConExp_CONTRACTED][Vie_VIEW	] = ActExpSeeTreNodAss,	// Contracted, Not editing ==> action to expand
      [Inf_ASSESSMENT	][ConExp_CONTRACTED][Vie_EDIT	] = ActExpEdiTreNodAss,	// Contracted,     Editing ==> action to expand
      [Inf_ASSESSMENT	][ConExp_EXPANDED  ][Vie_VIEW	] = ActConSeeTreNodAss,	// Expanded  , Not editing ==> action to contract
      [Inf_ASSESSMENT	][ConExp_EXPANDED  ][Vie_EDIT	] = ActConEdiTreNodAss,	// Expanded  ,     Editing ==> action to contract
     };
   static void (*PutContextualIcon[ConExp_NUM_CONTRACTED_EXPANDED]) (Act_Action_t Actions,const char *Anchor,
								     void (*FuncPars) (void *Args),void *Args) =
     {
      [ConExp_CONTRACTED] = Ico_PutContextualIconToExpand,	// Contracted ==> function to expand
      [ConExp_EXPANDED  ] = Ico_PutContextualIconToContract,	// Expanded   ==> function to contract
     };

   /***** Icon to hide/unhide tree node *****/
   PutContextualIcon[ContractedOrExpanded] (Actions[Node->InfoType][ContractedOrExpanded][ViewType],
					    Tre_NODE_SECTION_ID,
					    Tre_PutPars,Node);
  }

/*****************************************************************************/
/**************************** Show node text *********************************/
/*****************************************************************************/

static void Tre_WriteNodeText (const struct Tre_Node *Node,
			       HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];
   char Txt[Cns_MAX_BYTES_TEXT + 1];

   Tre_DB_GetNodeTxt (Node,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
		     Txt,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
   if (Txt[0])
     {
      ALn_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
      HTM_DIV_Begin ("class=\"PAR TRE_TXT_%s%s\"",
		     The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
	 HTM_Txt (Txt);
      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/**************************** Show node form *********************************/
/*****************************************************************************/

static void Tre_WriteRowToCreateNode (Inf_Type_t InfoType,
				      long ParentNodCod,unsigned FormLevel)
  {
   char *TitleClass;
   unsigned ColSpan;
   unsigned NumCol;

   /***** Title CSS class *****/
   Tre_SetTitleClass (&TitleClass,FormLevel);

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Column under expand/contract icon *****/
      HTM_TD_Begin ("class=\"TRE_COL1 LT %s\"",The_GetColorRows ());
      HTM_TD_End ();

      /***** Column under icons *****/
      HTM_TD_Begin ("class=\"TRE_COL1 LT %s\"",The_GetColorRows ());
      HTM_TD_End ();

      /***** Indent depending on the level *****/
      for (NumCol = 1;
	   NumCol < FormLevel;
	   NumCol++)
	{
	 HTM_TD_Begin ("class=\"TRE_COL1 %s\"",The_GetColorRows ());
	 HTM_TD_End ();
	}

      /***** Node number *****/
      HTM_TD_Begin ("class=\"TRE_NUM %s %s\"",TitleClass,The_GetColorRows ());
	 HTM_Unsigned (Tre_GetCurrentNumberInLevel (FormLevel) + 1);
      HTM_TD_End ();

      /***** Show form to create new node as child *****/
      ColSpan = Tre_GetMaxNodeLevel () + 2 - FormLevel;
      if (InfoType == Inf_PROGRAM)
	 ColSpan++;
      HTM_TD_Begin ("colspan=\"%u\" class=\"TRE_MAIN %s\"",
		    ColSpan,The_GetColorRows ());
         /* Form for node data */
	 HTM_ARTICLE_Begin (Tre_NODE_SECTION_ID);
	    Tre_ShowFormToCreateNode (InfoType,ParentNodCod);
	 HTM_ARTICLE_End ();
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();

   /***** Free title CSS class *****/
   Tre_FreeTitleClass (TitleClass);
  }

/*****************************************************************************/
/**************** Set / free title class depending on level ******************/
/*****************************************************************************/

static void Tre_SetTitleClass (char **TitleClass,unsigned Level)
  {
   if (asprintf (TitleClass,"TRE_TIT_%u_%s",
		 Level < 5 ? Level :
			     5,
		 The_GetSuffix ()) < 0)
      Err_NotEnoughMemoryExit ();
  }

static void Tre_FreeTitleClass (char *TitleClass)
  {
   free (TitleClass);
  }

/*****************************************************************************/
/*************** Set and get maximum level in a course tree ******************/
/*****************************************************************************/

static void Tre_SetMaxNodeLevel (unsigned Level)
  {
   Tre_Gbl.MaxLevel = Level;
  }

static unsigned Tre_GetMaxNodeLevel (void)
  {
   return Tre_Gbl.MaxLevel;
  }

/*****************************************************************************/
/********* Calculate maximum level of indentation in a course tree ***********/
/*****************************************************************************/

static unsigned Tre_CalculateMaxNodeLevel (void)
  {
   unsigned NumNode;
   unsigned MaxLevel = 0;	// Return 0 if no nodes

   /***** Compute maximum level of all tree nodes *****/
   for (NumNode = 0;
	NumNode < Tre_Gbl.List.NumNodes;
	NumNode++)
      if (Tre_GetLevelFromNumNode (NumNode) > MaxLevel)
	 MaxLevel = Tre_GetLevelFromNumNode (NumNode);

   return MaxLevel;
  }

/*****************************************************************************/
/************************* Get if a node has children ************************/
/*****************************************************************************/

static Exi_Exist_t Tre_GetIfNodeHasChildren (unsigned NumNode)
  {
   /***** Check 1. If node is the last in list ==> node has no children *****/
   if (NumNode == Tre_Gbl.List.NumNodes - 1)
      return Exi_DOES_NOT_EXIST;

   /***** Check 2. If next node is in next level ==> node has children *****/
   return (Tre_GetLevelFromNumNode (NumNode + 1) > Tre_GetLevelFromNumNode (NumNode)) ? Exi_EXISTS :
										        Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/********************* Allocate memory for node numbers **********************/
/*****************************************************************************/

static void Tre_CreateLevels (void)
  {
   unsigned MaxLevel = Tre_GetMaxNodeLevel ();

   if (MaxLevel)
     {
      /***** Allocate memory for node numbers and initialize to 0 *****/
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
        5     0	  <--- Used to create a new node
      */
      if ((Tre_Gbl.Levels = calloc (1 + MaxLevel + 1,
                                    sizeof (*Tre_Gbl.Levels))) == NULL)
	 Err_NotEnoughMemoryExit ();
     }
   else
      Tre_Gbl.Levels = NULL;
  }

/*****************************************************************************/
/*********************** Free memory for node numbers ************************/
/*****************************************************************************/

static void Tre_FreeLevels (void)
  {
   if (Tre_GetMaxNodeLevel () && Tre_Gbl.Levels)
     {
      /***** Free allocated memory for node numbers *****/
      free (Tre_Gbl.Levels);
      Tre_Gbl.Levels = NULL;
     }
  }

/*****************************************************************************/
/**************************** Increase number of node ************************/
/*****************************************************************************/

static void Tre_IncreaseNumberInLevel (unsigned Level)
  {
   /***** Increase number for this level *****/
   Tre_Gbl.Levels[Level    ].Number++;

   /***** Reset number for next level (children) *****/
   Tre_Gbl.Levels[Level + 1].Number = 0;
  }

/*****************************************************************************/
/****************** Get current number of node in a level ********************/
/*****************************************************************************/

static unsigned Tre_GetCurrentNumberInLevel (unsigned Level)
  {
   if (Tre_Gbl.Levels)
      return Tre_Gbl.Levels[Level].Number;

   return 0;
  }

/*****************************************************************************/
/********************** Set / Get if a level is hidden ***********************/
/*****************************************************************************/

static void Tre_SetExpandedLevel (unsigned Level,
				  ConExp_ContractedOrExpanded_t ContractedOrExpanded)
  {
   if (Tre_Gbl.Levels)
      Tre_Gbl.Levels[Level].ContractedOrExpanded = ContractedOrExpanded;
  }

static void Tre_SetHiddenLevel (unsigned Level,
				HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   if (Tre_Gbl.Levels)
      Tre_Gbl.Levels[Level].HiddenOrVisible = HiddenOrVisible;
  }

static ConExp_ContractedOrExpanded_t Tre_GetExpandedLevel (unsigned Level)
  {
   /* Level 0 (root) is always expanded */
   if (Level == 0)
      return ConExp_EXPANDED;

   if (Tre_Gbl.Levels)
      return Tre_Gbl.Levels[Level].ContractedOrExpanded;

   return ConExp_CONTRACTED;
  }

static HidVis_HiddenOrVisible_t Tre_GetHiddenLevel (unsigned Level)
  {
   /* Level 0 (root) is always visible */
   if (Level == 0)
      return HidVis_VISIBLE;

   if (Tre_Gbl.Levels)
      return Tre_Gbl.Levels[Level].HiddenOrVisible;

   return HidVis_VISIBLE;
  }

/*****************************************************************************/
/********* Check if any level higher than the current one is hidden **********/
/*****************************************************************************/

static bool Tre_CheckIfAllHigherLevelsAreExpanded (unsigned CurrentLevel)
  {
   unsigned Level;

   for (Level = 1;
	Level < CurrentLevel;
	Level++)
      if (Tre_GetExpandedLevel (Level) == ConExp_CONTRACTED)	// Contracted?
         return false;	// A level is contracted. Not all are expanded

   return true;	// None is contracted. All are expanded
  }

static HidVis_HiddenOrVisible_t Tre_CheckIfAnyHigherLevelIsHidden (unsigned CurrentLevel)
  {
   unsigned Level;

   for (Level = 1;
	Level < CurrentLevel;
	Level++)
      if (Tre_GetHiddenLevel (Level) == HidVis_HIDDEN)
         return HidVis_HIDDEN;

   return HidVis_VISIBLE;	// None is hidden. All are visible.
  }

/*****************************************************************************/
/**************** Put a link (form) to edit one tree node *****************/
/*****************************************************************************/

static void Tre_PutFormsToRemEditOneNode (Tre_ListingType_t ListingType,
                                          unsigned NumNode,
                                          struct Tre_Node *Node,
                                          bool HighlightNode)
  {
   extern const char *Txt_Movement_not_allowed;
   static Act_Action_t ActionsReqRem[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActReqRemTreNodInf,
      [Inf_PROGRAM	] = ActReqRemTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActReqRemTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActReqRemTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActReqRemTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActReqRemTreNodBib,
      [Inf_FAQ		] = ActReqRemTreNodFAQ,
      [Inf_LINKS	] = ActReqRemTreNodLnk,
      [Inf_ASSESSMENT	] = ActReqRemTreNodAss,
     };
   static Act_Action_t ActionsHideUnhide[Inf_NUM_TYPES][HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [Inf_UNKNOWN_TYPE	][HidVis_HIDDEN ] = ActUnk,
      [Inf_UNKNOWN_TYPE	][HidVis_VISIBLE] = ActUnk,
      [Inf_INFORMATION	][HidVis_HIDDEN ] = ActUnhTreNodInf,	// Hidden ==> action to unhide
      [Inf_INFORMATION	][HidVis_VISIBLE] = ActHidTreNodInf,	// Visible ==> action to hide
      [Inf_PROGRAM	][HidVis_HIDDEN ] = ActUnhTreNodPrg,	// Hidden ==> action to unhide
      [Inf_PROGRAM	][HidVis_VISIBLE] = ActHidTreNodPrg,	// Visible ==> action to hide
      [Inf_TEACH_GUIDE	][HidVis_HIDDEN ] = ActUnhTreNodGui,	// Hidden ==> action to unhide
      [Inf_TEACH_GUIDE	][HidVis_VISIBLE] = ActHidTreNodGui,	// Visible ==> action to hide
      [Inf_SYLLABUS_LEC	][HidVis_HIDDEN ] = ActUnhTreNodSyl,	// Hidden ==> action to unhide
      [Inf_SYLLABUS_LEC	][HidVis_VISIBLE] = ActHidTreNodSyl,	// Visible ==> action to hide
      [Inf_SYLLABUS_PRA	][HidVis_HIDDEN ] = ActUnhTreNodSyl,	// Hidden ==> action to unhide
      [Inf_SYLLABUS_PRA	][HidVis_VISIBLE] = ActHidTreNodSyl,	// Visible ==> action to hide
      [Inf_BIBLIOGRAPHY	][HidVis_HIDDEN ] = ActUnhTreNodBib,	// Hidden ==> action to unhide
      [Inf_BIBLIOGRAPHY	][HidVis_VISIBLE] = ActHidTreNodBib,	// Visible ==> action to hide
      [Inf_FAQ		][HidVis_HIDDEN ] = ActUnhTreNodFAQ,	// Hidden ==> action to unhide
      [Inf_FAQ		][HidVis_VISIBLE] = ActHidTreNodFAQ,	// Visible ==> action to hide
      [Inf_LINKS	][HidVis_HIDDEN ] = ActUnhTreNodLnk,	// Hidden ==> action to unhide
      [Inf_LINKS	][HidVis_VISIBLE] = ActHidTreNodLnk,	// Visible ==> action to hide
      [Inf_ASSESSMENT	][HidVis_HIDDEN ] = ActUnhTreNodAss,	// Hidden ==> action to unhide
      [Inf_ASSESSMENT	][HidVis_VISIBLE] = ActHidTreNodAss,	// Visible ==> action to hide
     };
   static Act_Action_t ActionsSee[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActSeeTreNodInf,
      [Inf_PROGRAM	] = ActSeeTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActSeeTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActSeeTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActSeeTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActSeeTreNodBib,
      [Inf_FAQ		] = ActSeeTreNodFAQ,
      [Inf_LINKS	] = ActSeeTreNodLnk,
      [Inf_ASSESSMENT	] = ActSeeTreNodAss,
     };
   static Act_Action_t ActionsFrmChg[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActFrmChgTreNodInf,
      [Inf_PROGRAM	] = ActFrmChgTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActFrmChgTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActFrmChgTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActFrmChgTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActFrmChgTreNodBib,
      [Inf_FAQ		] = ActFrmChgTreNodFAQ,
      [Inf_LINKS	] = ActFrmChgTreNodLnk,
      [Inf_ASSESSMENT	] = ActFrmChgTreNodAss,
     };
   static Act_Action_t ActionsFrmNew[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActFrmNewTreNodInf,
      [Inf_PROGRAM	] = ActFrmNewTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActFrmNewTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActFrmNewTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActFrmNewTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActFrmNewTreNodBib,
      [Inf_FAQ		] = ActFrmNewTreNodFAQ,
      [Inf_LINKS	] = ActFrmNewTreNodLnk,
      [Inf_ASSESSMENT	] = ActFrmNewTreNodAss,
     };
   static Act_Action_t ActionsUp_[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActUp_TreNodInf,
      [Inf_PROGRAM	] = ActUp_TreNodPrg,
      [Inf_TEACH_GUIDE	] = ActUp_TreNodGui,
      [Inf_SYLLABUS_LEC	] = ActUp_TreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActUp_TreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActUp_TreNodBib,
      [Inf_FAQ		] = ActUp_TreNodFAQ,
      [Inf_LINKS	] = ActUp_TreNodLnk,
      [Inf_ASSESSMENT	] = ActUp_TreNodAss,
     };
   static Act_Action_t ActionsDwn[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActDwnTreNodInf,
      [Inf_PROGRAM	] = ActDwnTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActDwnTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActDwnTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActDwnTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActDwnTreNodBib,
      [Inf_FAQ		] = ActDwnTreNodFAQ,
      [Inf_LINKS	] = ActDwnTreNodLnk,
      [Inf_ASSESSMENT	] = ActDwnTreNodAss,
     };
   static Act_Action_t ActionsLft[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActLftTreNodInf,
      [Inf_PROGRAM	] = ActLftTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActLftTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActLftTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActLftTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActLftTreNodBib,
      [Inf_FAQ		] = ActLftTreNodFAQ,
      [Inf_LINKS	] = ActLftTreNodLnk,
      [Inf_ASSESSMENT	] = ActLftTreNodAss,
     };
   static Act_Action_t ActionsRgt[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActRgtTreNodInf,
      [Inf_PROGRAM	] = ActRgtTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActRgtTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActRgtTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActRgtTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActRgtTreNodBib,
      [Inf_FAQ		] = ActRgtTreNodFAQ,
      [Inf_LINKS	] = ActRgtTreNodLnk,
      [Inf_ASSESSMENT	] = ActRgtTreNodAss,
     };
   char StrItemIndex[Cns_MAX_DIGITS_UINT + 1];

   /***** Initialize node index string *****/
   snprintf (StrItemIndex,sizeof (StrItemIndex),"%u",Node->Hierarchy.NodInd);

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Icon to remove tree node *****/
	 Ico_PutContextualIconToRemove (ActionsReqRem[Node->InfoType],NULL,
	                                Tre_PutPars,Node);

	 /***** Icon to hide/unhide tree node *****/
	 Ico_PutContextualIconToHideUnhide (ActionsHideUnhide[Node->InfoType],Tre_NODE_SECTION_ID,
					    Tre_PutPars,Node,
					    Node->Hierarchy.Hidden);

	 /***** Icon to edit tree node *****/
	 if (ListingType == Tre_FORM_EDIT_NODE && HighlightNode)
	    Ico_PutContextualIconToView (ActionsSee[Node->InfoType],Tre_NODE_SECTION_ID,
					 Tre_PutPars,Node);
	 else
	    Ico_PutContextualIconToEdit (ActionsFrmChg[Node->InfoType],Tre_NODE_SECTION_ID,
					 Tre_PutPars,Node);

	 /***** Icon to add a new child node inside this node *****/
	 Ico_PutContextualIconToAdd (ActionsFrmNew[Node->InfoType],Tre_NODE_SECTION_ID,
	                             Tre_PutPars,Node);

	 HTM_BR ();

	 /***** Icon to move up the node *****/
	 if (Tre_CheckIfMoveUpIsAllowed (NumNode))
	    Lay_PutContextualLinkOnlyIcon (ActionsUp_[Node->InfoType],Tre_NODE_SECTION_ID,
	                                   Tre_PutPars,Node,
					   "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Icon to move down the node *****/
	 if (Tre_CheckIfMoveDownIsAllowed (NumNode))
	    Lay_PutContextualLinkOnlyIcon (ActionsDwn[Node->InfoType],Tre_NODE_SECTION_ID,
	                                   Tre_PutPars,Node,
					   "arrow-down.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-down.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Icon to move left node (increase level) *****/
	 if (Tre_CheckIfMoveLeftIsAllowed (NumNode))
	    Lay_PutContextualLinkOnlyIcon (ActionsLft[Node->InfoType],Tre_NODE_SECTION_ID,
	                                   Tre_PutPars,Node,
					   "arrow-left.svg",Ico_BLACK);
	 else
            Ico_PutIconOff ("arrow-left.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Icon to move right node (indent, decrease level) *****/
	 if (Tre_CheckIfMoveRightIsAllowed (NumNode))
	    Lay_PutContextualLinkOnlyIcon (ActionsRgt[Node->InfoType],Tre_NODE_SECTION_ID,
	                                   Tre_PutPars,Node,
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
/*********************** Check if node can be moved up ***********************/
/*****************************************************************************/

static bool Tre_CheckIfMoveUpIsAllowed (unsigned NumNode)
  {
   /***** Trivial check: if node is the first one, move up is not allowed *****/
   if (NumNode == 0)
      return false;

   /***** Move up is allowed if the node has brothers before it *****/
   // NumItem >= 1
   return Tre_GetLevelFromNumNode (NumNode - 1) >=
	  Tre_GetLevelFromNumNode (NumNode    );
  }

/*****************************************************************************/
/********************** Check if node can be moved down **********************/
/*****************************************************************************/

static bool Tre_CheckIfMoveDownIsAllowed (unsigned NumNode)
  {
   unsigned i;
   unsigned Level;

   /***** Trivial check: if node is the last one, move up is not allowed *****/
   if (NumNode >= Tre_Gbl.List.NumNodes - 1)
      return false;

   /***** Move down is allowed if the node has brothers after it *****/
   // NumItem + 1 < Prg_Gbl.List.NumItems
   Level = Tre_GetLevelFromNumNode (NumNode);
   for (i = NumNode + 1;
	i < Tre_Gbl.List.NumNodes;
	i++)
     {
      if (Tre_GetLevelFromNumNode (i) == Level)
	 return true;	// Next brother found
      if (Tre_GetLevelFromNumNode (i) < Level)
	 return false;	// Next lower level found ==> there are no more brothers
     }
   return false;	// End reached ==> there are no more brothers
  }

/*****************************************************************************/
/******************* Check if node can be moved to the left ******************/
/*****************************************************************************/

static bool Tre_CheckIfMoveLeftIsAllowed (unsigned NumNode)
  {
   /***** Move left is allowed if the node has parent *****/
   return Tre_GetLevelFromNumNode (NumNode) > 1;
  }

/*****************************************************************************/
/****************** Check if node can be moved to the right ******************/
/*****************************************************************************/

static bool Tre_CheckIfMoveRightIsAllowed (unsigned NumNode)
  {
   /***** If node is the first, move right is not allowed *****/
   if (NumNode == 0)
      return false;

   /***** Move right is allowed if the node has brothers before it *****/
   // NumItem >= 2
   return Tre_GetLevelFromNumNode (NumNode - 1) >=
	  Tre_GetLevelFromNumNode (NumNode    );
  }

/*****************************************************************************/
/********************** Params used to edit a tree node **********************/
/*****************************************************************************/

void Tre_PutPars (void *Node)
  {
   static Inf_Type_t InfoType;

   if (Node)
     {
      ParCod_PutPar (ParCod_Nod,((struct Tre_Node *) Node)->Hierarchy.NodCod);
      switch (((struct Tre_Node *) Node)->InfoType)
	{
	 case Inf_SYLLABUS_LEC:
	    InfoType = Inf_SYLLABUS_LEC;
	    Inf_PutParInfoType (&InfoType);
	    break;
	 case Inf_SYLLABUS_PRA:
	    InfoType = Inf_SYLLABUS_PRA;
	    Inf_PutParInfoType (&InfoType);
	    break;
	 default:
	    break;
	}
     }
  }

// Node type must be set before calling this function
void Tre_GetPars (struct Tre_Node *Node)
  {
   /***** Clear all node data except type *****/
   Tre_ResetNode (Node);

   /****** Parameters specific for each type of tree *****/
   Node->Item.Cod = ParCod_GetPar (ParCod_Itm);
   TreSpc_GetItemDataByCod (Node);

   if (Node->Hierarchy.NodCod <= 0)	// No node specified
      /***** Try to get data of the tree node *****/
      Node->Hierarchy.NodCod = ParCod_GetPar (ParCod_Nod);

   /***** Get data of the tree node from database *****/
   Tre_GetNodeDataByCod (Node);
  }

/*****************************************************************************/
/*************************** List all tree nodes *****************************/
/*****************************************************************************/

void Tre_GetListNodes (Inf_Type_t InfoType)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumItem;

   if (Tre_Gbl.List.IsRead)
      Tre_FreeListNodes ();

   /***** Get list of tree nodes from database *****/
   if ((Tre_Gbl.List.NumNodes = Tre_DB_GetListNodes (InfoType,&mysql_res))) // Nodes found...
     {
      /***** Create list of tree nodes *****/
      if ((Tre_Gbl.List.Nodes = calloc (Tre_Gbl.List.NumNodes,
				        sizeof (*Tre_Gbl.List.Nodes))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the tree nodes codes *****/
      for (NumItem = 0;
	   NumItem < Tre_Gbl.List.NumNodes;
	   NumItem++)
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get code of the tree node (row[0]) */
         if ((Tre_Gbl.List.Nodes[NumItem].NodCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongItemExit ();

         /* Get index of the tree node (row[1])
            and level of the tree node (row[2]) */
         Tre_Gbl.List.Nodes[NumItem].NodInd = Str_ConvertStrToUnsigned (row[1]);
         Tre_Gbl.List.Nodes[NumItem].Level  = Str_ConvertStrToUnsigned (row[2]);

	 /* Get whether the tree node is hidden or not (row[3]) */
	 Tre_Gbl.List.Nodes[NumItem].Hidden = HidVis_GetHiddenFromYN (row[3][0]);
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Tre_Gbl.List.IsRead = true;
  }

/*****************************************************************************/
/************************ Get number of nodes in tree ************************/
/*****************************************************************************/

unsigned Tre_GetNumNodes (void)
  {
   return Tre_Gbl.List.NumNodes;
  }

/*****************************************************************************/
/******************** Get tree node data using its code **********************/
/*****************************************************************************/
// Node type must be set before calling this function

static void Tre_GetNodeDataByCod (struct Tre_Node *Node)
  {
   MYSQL_RES *mysql_res;
   Exi_Exist_t NodeExists;

   if (Node->Hierarchy.NodCod > 0)
     {
      /***** Build query *****/
      NodeExists = Tre_DB_GetNodeDataByCod (Node,&mysql_res);

      /***** Get data of tree node *****/
      Tre_GetNodeDataFromRow (&mysql_res,Node,NodeExists);
     }
   else
      /***** Clear all node data except type *****/
      Tre_ResetNode (Node);
  }

/*****************************************************************************/
/*************************** Get tree node data ******************************/
/*****************************************************************************/
// Node type must be set before calling this function

static void Tre_GetNodeDataFromRow (MYSQL_RES **mysql_res,
                                    struct Tre_Node *Node,
                                    Exi_Exist_t NodeExists)
  {
   MYSQL_ROW row;

   /***** Get data of tree node from database *****/
   switch (NodeExists) // Item found...
     {
      case Exi_EXISTS:
	 /* Get row */
	 row = mysql_fetch_row (*mysql_res);
	 /*
	 NodCod					row[0]
	 NodInd					row[1]
	 Level					row[2]
	 Hidden					row[3]
	 UsrCod					row[4]
	 UNIX_TIMESTAMP(StartTime)		row[5]
	 UNIX_TIMESTAMP(EndTime)		row[6]
	 NOW() BETWEEN StartTime AND EndTime	row[7]
	 Title					row[8]
	 */

	 /* Get code of the tree node (row[0]) */
	 Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get index of the tree node (row[1])
	    and level of the tree node (row[2]) */
	 Node->Hierarchy.NodInd = Str_ConvertStrToUnsigned (row[1]);
	 Node->Hierarchy.Level = Str_ConvertStrToUnsigned (row[2]);

	 /* Get whether the tree node is hidden or not (row[3]) */
	 Node->Hierarchy.Hidden = HidVis_GetHiddenFromYN (row[3][0]);

	 /* Get author of the tree node (row[4]) */
	 Node->UsrCod = Str_ConvertStrCodToLongCod (row[4]);

	 /* Get start date (row[5] holds the start UTC time)
	    and end date   (row[6] holds the end   UTC time) */
	 Node->TimeUTC[Dat_STR_TIME] = Dat_GetUNIXTimeFromStr (row[5]);
	 Node->TimeUTC[Dat_END_TIME] = Dat_GetUNIXTimeFromStr (row[6]);

	 /* Get whether the tree node is open or closed (row(7)) */
	 Node->ClosedOrOpen = CloOpe_GetClosedOrOpenFrom01 (row[7][0]);

	 /* Get the title of the tree node (row[8]) */
	 Str_Copy (Node->Title,row[8],sizeof (Node->Title) - 1);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 /***** Clear all node data except type *****/
	 Tre_ResetNode (Node);
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************* Free list of tree nodes ***************************/
/*****************************************************************************/

void Tre_FreeListNodes (void)
  {
   if (Tre_Gbl.List.IsRead && Tre_Gbl.List.Nodes)
     {
      /***** Free memory used by the list of tree nodes *****/
      free (Tre_Gbl.List.Nodes);
      Tre_Gbl.List.Nodes = NULL;
      Tre_Gbl.List.NumNodes = 0;
      Tre_Gbl.List.IsRead = false;
     }
  }

/*****************************************************************************/
/************************ Clear all tree node data ************************/
/*****************************************************************************/

void Tre_ResetNode (struct Tre_Node *Node)
  {
   // Node->InfoType is not reset
   Node->Hierarchy.NodCod = -1L;
   Node->Hierarchy.NodInd = 0;
   Node->Hierarchy.Level  = 0;
   Node->Hierarchy.Hidden = HidVis_VISIBLE;
   Node->UsrCod = -1L;
   Node->TimeUTC[Dat_STR_TIME] =
   Node->TimeUTC[Dat_END_TIME] = (time_t) 0;
   Node->ClosedOrOpen = CloOpe_CLOSED;
   Node->Title[0] = '\0';
   TreSpc_ResetItem (Node);
  }

/*****************************************************************************/
/************* Get number of item inlist from node code ****************/
/*****************************************************************************/

unsigned Tre_GetNumNodeFromNodCod (long NodCod)
  {
   unsigned NumNode;

   /***** List of nodes must be filled *****/
   if (!Tre_Gbl.List.IsRead || Tre_Gbl.List.Nodes == NULL)
      Err_WrongItemsListExit ();

   /***** Find node code in list *****/
   for (NumNode = 0;
	NumNode < Tre_Gbl.List.NumNodes;
	NumNode++)
      if (Tre_GetNodCodFromNumNode (NumNode) == NodCod)	// Found!
	 return NumNode;

   /***** Not found *****/
   Err_WrongItemExit ();
   return 0;	// Not reached
  }

/*****************************************************************************/
/******************** Get node code from number of node **********************/
/*****************************************************************************/

inline long Tre_GetNodCodFromNumNode (unsigned NumNode)
  {
   return Tre_Gbl.List.Nodes[NumNode].NodCod;
  }

/*****************************************************************************/
/******************** Get node index from number of node *********************/
/*****************************************************************************/

inline unsigned Tre_GetNodIndFromNumNode (unsigned NumNode)
  {
   return Tre_Gbl.List.Nodes[NumNode].NodInd;
  }

/*****************************************************************************/
/****************** Get level of node from number of node ********************/
/*****************************************************************************/

inline unsigned Tre_GetLevelFromNumNode (unsigned NumNode)
  {
   return Tre_Gbl.List.Nodes[NumNode].Level;
  }

/*****************************************************************************/
/********* List tree nodes when click on view a node after edition ***********/
/*****************************************************************************/

void Tre_ViewNodeAfterEdit (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /***** Show current tree nodes, if any *****/
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_END_EDIT_NODE,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************ List tree nodes with a form to change a given node *************/
/*****************************************************************************/

void Tre_ReqChangeNode (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /***** If node is contracted ==> expand it *****/
   if (Tre_DB_GetIfContractedOrExpandedNode (Node.Hierarchy.NodCod) == ConExp_CONTRACTED)	// If contracted...
      Tre_DB_InsertNodeInExpandedNodes (Node.Hierarchy.NodCod);					// ...expand it

   /***** Show current tree nodes, if any *****/
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_FORM_EDIT_NODE,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************* List tree nodes with a form to create a new node **************/
/*****************************************************************************/

void Tre_ReqCreateNode (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);

   /***** Add node to table of expanded nodes
          to ensure that child items are displayed *****/
   if (Tre_DB_GetIfContractedOrExpandedNode (Node.Hierarchy.NodCod) == ConExp_CONTRACTED)	// If contracted...
      Tre_DB_InsertNodeInExpandedNodes (Node.Hierarchy.NodCod);					// ...expand it

   /***** Show current tree nodes, if any *****/
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Node.Hierarchy.NodCod > 0 ? Tre_FORM_NEW_CHILD_NODE :
	                                         Tre_FORM_NEW_END_NODE,
	             &Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/***************** Put a form to create a new tree node *******************/
/*****************************************************************************/

static void Tre_ShowFormToCreateNode (Inf_Type_t InfoType,long ParentNodCod)
  {
   struct Tre_Node ParentNode;	// Parent node
   struct Tre_Node Node;
   static Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_TO_000000,
      [Dat_END_TIME] = Dat_HMS_TO_235959
     };
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActNewTreNodInf,
      [Inf_PROGRAM	] = ActNewTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActNewTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActNewTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActNewTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActNewTreNodBib,
      [Inf_FAQ		] = ActNewTreNodFAQ,
      [Inf_LINKS	] = ActNewTreNodLnk,
      [Inf_ASSESSMENT	] = ActNewTreNodAss,
     };

   /***** Get data of the parent tree node from database *****/
   ParentNode.InfoType = InfoType;
   ParentNode.Hierarchy.NodCod = ParentNodCod;
   Tre_GetNodeDataByCod (&ParentNode);

   /***** Initialize to empty node *****/
   Node.InfoType = ParentNode.InfoType;
   Tre_ResetNode (&Node);	// Clear all node data except type
   Node.TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
   Node.TimeUTC[Dat_END_TIME] = Node.TimeUTC[Dat_STR_TIME] + (2 * 60 * 60);	// +2 hours
   Node.ClosedOrOpen = CloOpe_OPEN;

   /***** Show pending alerts */
   Ale_ShowAlerts (NULL);

   /***** Begin form to create *****/
   Frm_BeginFormTable (Actions[InfoType],NULL,
                       Tre_PutPars,&ParentNode,"TBL_WIDE");

      /***** Show form *****/
      Tre_ShowFormNode (&Node,SetHMS,NULL);

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
  }

static void Tre_ShowFormToChangeNode (struct Tre_Node *Node)
  {
   char Txt[Cns_MAX_BYTES_TEXT + 1];
   static Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME] =
     {
      [Dat_STR_TIME] = Dat_HMS_DO_NOT_SET,
      [Dat_END_TIME] = Dat_HMS_DO_NOT_SET
     };
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActChgTreNodInf,
      [Inf_PROGRAM	] = ActChgTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActChgTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActChgTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActChgTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActChgTreNodBib,
      [Inf_FAQ		] = ActChgTreNodFAQ,
      [Inf_LINKS	] = ActChgTreNodLnk,
      [Inf_ASSESSMENT	] = ActChgTreNodAss,
     };

   /***** Get data of the tree node from database *****/
   Tre_GetNodeDataByCod (Node);
   Tre_DB_GetNodeTxt (Node,Txt);

   /***** Begin form to change *****/
   Frm_BeginFormTable (Actions[Node->InfoType],Tre_NODE_SECTION_ID,
                       Tre_PutPars,Node,"TBL_WIDE");

      /***** Show form *****/
      Tre_ShowFormNode (Node,SetHMS,Txt);

   /***** End form to change *****/
   Frm_EndFormTable (Btn_CONFIRM);
  }

/*****************************************************************************/
/***************** Put a form to create a new tree node *******************/
/*****************************************************************************/

static void Tre_ShowFormNode (const struct Tre_Node *Node,
			      const Dat_SetHMS SetHMS[Dat_NUM_START_END_TIME],
		              const char *Txt)
  {
   extern const char *Txt_Title;
   extern const char *Txt_Description;

   /***** Node title *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RM","Title",Txt_Title);

      /* Data */
      HTM_TD_Begin ("class=\"LM\"");
	 HTM_INPUT_TEXT ("Title",Tre_MAX_CHARS_NODE_TITLE,Node->Title,
			 HTM_REQUIRED,
			 "id=\"Title\""
			 " class=\"TRE_TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
			 The_GetSuffix ());
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Node start and end dates *****/
   if (Node->InfoType == Inf_PROGRAM)
      Dat_PutFormStartEndClientLocalDateTimes (Node->TimeUTC,
					       Dat_FORM_SECONDS_ON,
					       SetHMS);

   /***** Node text *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT","Txt",Txt_Description);

      /* Data */
      HTM_TD_Begin ("class=\"LT\"");
	 HTM_TEXTAREA_Begin (HTM_NO_ATTR,
			     "id=\"Txt\" name=\"Txt\" rows=\"10\""
			     " class=\"TRE_TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
			     The_GetSuffix ());
	    if (Txt)
	       if (Txt[0])
		  HTM_Txt (Txt);
	 HTM_TEXTAREA_End ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*************** Receive form to change an existing tree node ****************/
/*****************************************************************************/

void Tre_ReceiveChgNode (Inf_Type_t InfoType)
  {
   struct Tre_Node Node;
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Get node data from form *****/
   Tre_GetNodeDataFromForm (&Node,Description);

   /***** Update existing node *****/
   Tre_DB_UpdateNode (&Node,Description);

   /***** Show items highlighting the node just changed *****/
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_RECEIVE_NODE,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/******************* Receive form to create a new tree node ******************/
/*****************************************************************************/

void Tre_ReceiveNewNode (Inf_Type_t InfoType)
  {
   struct Tre_Node ParentNode;		// Parent node
   struct Tre_Node NewNode;		// Node data received from form
   char Description[Cns_MAX_BYTES_TEXT + 1];

   /***** Get tree node *****/
   ParentNode.InfoType = InfoType;
   Tre_GetPars (&ParentNode);
   // If node code <= 0 ==> this is the first node in the program

   /***** Set new node code *****/
   NewNode.InfoType         = ParentNode.InfoType;
   NewNode.Hierarchy.NodCod = -1L;
   NewNode.Hierarchy.Level  = ParentNode.Hierarchy.Level + 1;	// Create as child

   /***** Get node data from form *****/
   Tre_GetNodeDataFromForm (&NewNode,Description);

   /***** Create a new tree node *****/
   Tre_InsertNode (&ParentNode,&NewNode,Description);

   /***** Add node to table of expanded nodes *****/
   Tre_DB_InsertNodeInExpandedNodes (NewNode.Hierarchy.NodCod);

   /***** Update list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Show items highlighting the node just created *****/
   NewNode.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_NODES,&NewNode);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/**************************** Get node data from form ************************/
/*****************************************************************************/

static void Tre_GetNodeDataFromForm (struct Tre_Node *Node,
				     char Description[Cns_MAX_BYTES_TEXT + 1])
  {
   /***** Get start/end date-times *****/
   if (Node->InfoType == Inf_PROGRAM)
     {
      Node->TimeUTC[Dat_STR_TIME] = Dat_GetTimeUTCFromForm (Dat_STR_TIME);
      Node->TimeUTC[Dat_END_TIME] = Dat_GetTimeUTCFromForm (Dat_END_TIME);

      /* Adjust dates */
      if (Node->TimeUTC[Dat_STR_TIME] == 0)
	 Node->TimeUTC[Dat_STR_TIME] = Dat_GetStartExecutionTimeUTC ();
      if (Node->TimeUTC[Dat_END_TIME] == 0)
	 Node->TimeUTC[Dat_END_TIME] = Node->TimeUTC[Dat_STR_TIME] + 2 * 60 * 60;	// +2 hours
     }
   else
      Node->TimeUTC[Dat_STR_TIME] =
      Node->TimeUTC[Dat_END_TIME] = (time_t) 0;

   /***** Get node title *****/
   Par_GetParText ("Title",Node->Title,Tre_MAX_BYTES_NODE_TITLE);

   /***** Get node text *****/
   Par_GetParHTML ("Txt",Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
  }

/*****************************************************************************/
/*************** Insert a new node as a child of a parent node ***************/
/*****************************************************************************/

static void Tre_InsertNode (const struct Tre_Node *ParentNode,
		            struct Tre_Node *NewNode,const char *Txt)
  {
   unsigned NumNodeLastChild;

   /***** Lock table to create tree node *****/
   Tre_DB_LockTableNodes ();

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (ParentNode->InfoType);
   if (Tre_Gbl.List.NumNodes)	// There are nodes
     {
      if (ParentNode->Hierarchy.NodCod > 0)	// Parent specified
	{
	 /***** Calculate where to insert *****/
	 NumNodeLastChild = Tre_GetLastChild (Tre_GetNumNodeFromNodCod (ParentNode->Hierarchy.NodCod));
	 if (NumNodeLastChild < Tre_Gbl.List.NumNodes - 1)
	   {
	    /***** New node will be inserted after last child of parent *****/
	    NewNode->Hierarchy.NodInd = Tre_GetNodIndFromNumNode (NumNodeLastChild + 1);

	    /***** Move down all indexes of after last child of parent *****/
	    Tre_DB_MoveDownNodes (ParentNode->InfoType,NewNode->Hierarchy.NodInd);
	   }
	 else
	    /***** New node will be inserted at the end *****/
	    NewNode->Hierarchy.NodInd = Tre_GetNodIndFromNumNode (Tre_Gbl.List.NumNodes - 1) + 1;

	 /***** Child ==> parent level + 1 *****/
         NewNode->Hierarchy.Level = ParentNode->Hierarchy.Level + 1;
	}
      else	// No parent specified
	{
	 /***** New tree node will be inserted at the end *****/
	 NewNode->Hierarchy.NodInd = Tre_GetNodIndFromNumNode (Tre_Gbl.List.NumNodes - 1) + 1;

	 /***** First level *****/
         NewNode->Hierarchy.Level = 1;
	}
     }
   else		// There are no nodes
     {
      /***** New tree node will be inserted as the first one *****/
      NewNode->Hierarchy.NodInd = 1;

      /***** First level *****/
      NewNode->Hierarchy.Level = 1;
     }

   /***** Insert new tree node *****/
   NewNode->Hierarchy.NodCod = Tre_DB_InsertNode (NewNode,Txt);

   /***** Unlock table *****/
   DB_UnlockTables ();

   /***** Free list items *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing a tree node ****************/
/*****************************************************************************/

void Tre_ReqRemNode (Inf_Type_t InfoType)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_item_X;
   static Act_Action_t Actions[Inf_NUM_TYPES] =
     {
      [Inf_UNKNOWN_TYPE	] = ActUnk,
      [Inf_INFORMATION	] = ActRemTreNodInf,
      [Inf_PROGRAM	] = ActRemTreNodPrg,
      [Inf_TEACH_GUIDE	] = ActRemTreNodGui,
      [Inf_SYLLABUS_LEC	] = ActRemTreNodSyl,
      [Inf_SYLLABUS_PRA	] = ActRemTreNodSyl,
      [Inf_BIBLIOGRAPHY	] = ActRemTreNodBib,
      [Inf_FAQ		] = ActRemTreNodFAQ,
      [Inf_LINKS	] = ActRemTreNodLnk,
      [Inf_ASSESSMENT	] = ActRemTreNodAss,
     };
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Show question and button to remove the tree node *****/
   Ale_ShowAlertRemove (Actions[InfoType],NULL,
                        Tre_PutPars,&Node,
			Txt_Do_you_really_want_to_remove_the_item_X,
                        Node.Title);

   /***** Show item highlighting subtree *****/
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_NODES,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/******************** Remove a tree node and its children ********************/
/*****************************************************************************/

void Tre_RemoveNode (Inf_Type_t InfoType)
  {
   extern const char *Txt_Item_X_removed;
   struct Tre_Node Node;
   struct Tre_NodeRange ToRemove;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Indexes of items *****/
   Tre_SetNodeRangeWithAllChildren (Tre_GetNumNodeFromNodCod (Node.Hierarchy.NodCod),
				    &ToRemove);

   /***** Remove items *****/
   Tre_DB_RemoveNodeRange (InfoType,&ToRemove);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Item_X_removed,Node.Title);

   /***** Update list of tree nodes *****/
   Tre_FreeListNodes ();
   Tre_GetListNodes (InfoType);

   /***** Show course program without highlighting any node *****/
   Node.Hierarchy.NodCod = -1L;
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_NODES,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************** Hide/unhide a tree node **************************/
/*****************************************************************************/

void Tre_HideOrUnhideNode (Inf_Type_t InfoType,
			   HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Hide/unhide tree node *****/
   Tre_DB_HideOrUnhideNode (&Node,HiddenOrVisible);

   /***** Show items highlighting subtree *****/
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_NODES,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********** Move up/down position of a subtree in a course tree *************/
/*****************************************************************************/

void Tre_MoveUpDownNode (Inf_Type_t InfoType,Tre_MoveUpDown_t UpDown)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Tre_Node Node;
   unsigned NumNode;
   Err_SuccessOrError_t SuccessOrError = Err_ERROR;
   static bool (*CheckIfAllowed[Tre_NUM_MOVEMENTS_UP_DOWN])(unsigned NumNode) =
     {
      [Tre_MOVE_UP  ] = Tre_CheckIfMoveUpIsAllowed,
      [Tre_MOVE_DOWN] = Tre_CheckIfMoveDownIsAllowed,
     };

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Move up/down node *****/
   NumNode = Tre_GetNumNodeFromNodCod (Node.Hierarchy.NodCod);
   if (CheckIfAllowed[UpDown] (NumNode))
     {
      /* Exchange subtrees */
      switch (UpDown)
        {
	 case Tre_MOVE_UP:
            SuccessOrError = Tre_ExchangeNodeRanges (InfoType,Tre_GetPrevBrother (NumNode),NumNode);
            break;
	 case Tre_MOVE_DOWN:
            SuccessOrError = Tre_ExchangeNodeRanges (InfoType,NumNode,Tre_GetNextBrother (NumNode));
            break;
        }
     }
   switch (SuccessOrError)
     {
      case Err_SUCCESS:
	 /* Update list of tree nodes */
	 Tre_FreeListNodes ();
	 Tre_GetListNodes (InfoType);
	 break;
      case Err_ERROR:
      default:
	 Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
	 Node.Hierarchy.NodCod = -1L;
	 break;
     }
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_NODES,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/**** Exchange the order of two consecutive subtrees in a course program *****/
/*****************************************************************************/

static Err_SuccessOrError_t Tre_ExchangeNodeRanges (Inf_Type_t InfoType,
						    int NumNodeTop,int NumNodeBottom)
  {
   struct Tre_NodeRange Top;
   struct Tre_NodeRange Bottom;
   unsigned DiffBegin;
   unsigned DiffEnd;

   if (NumNodeTop    >= 0 &&
       NumNodeBottom >= 0)
     {
      Tre_SetNodeRangeWithAllChildren (NumNodeTop   ,&Top   );
      Tre_SetNodeRangeWithAllChildren (NumNodeBottom,&Bottom);
      DiffBegin = Bottom.Begin - Top.Begin;
      DiffEnd   = Bottom.End   - Top.End;

      /***** Lock table to make the move atomic *****/
      Tre_DB_LockTableNodes ();

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
              |NodInd|NodCod|   |NodInd|NodCod|   |NodInd|NodCod|   |NodInd|NodCod| |NodInd|NodCod|
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
Top.Begin:    |     5|   218|-->|--> -5|   218|-->|--> 37|   218|   |    37|   218| |     5|   221|
              |    10|   219|-->|-->-10|   219|-->|--> 42|   219|   |    42|   219| |    26|   222|
Top.End:      |    17|   220|-->|-->-17|   220|-->|--> 49|   220|   |    49|   220| |    37|   218|
Bottom.Begin: |    28|   221|-->|-->-28|   221|   |   -28|   221|-->|-->  5|   221| |    42|   219|
Bottom.End:   |    49|   222|-->|-->-49|   222|   |   -49|   222|-->|--> 26|   222| |    49|   220|
              +------+------+   +------+------+   +------+------+   +------+------+ +------+------+
      */
      /* Step 1: Change all indexes involved to negative,
		 necessary to preserve unique index (CrsCod,NodInd) */
      Tre_DB_UpdateIndexRange (InfoType,
				 (long) 0            ,	// NodInd=-NodInd
                                 (long) Top.Begin    ,
                                 (long) Bottom.End   );	// All indexes in both parts

      /* Step 2: Increase top indexes */
      Tre_DB_UpdateIndexRange (InfoType,
				 (long) DiffEnd      ,	// NodInd=-NodInd+DiffEnd
                               -((long) Top.End     ),
                               -((long) Top.Begin   ));	// All indexes in top part

      /* Step 3: Decrease bottom indexes */
      Tre_DB_UpdateIndexRange (InfoType,
			       -((long) DiffBegin   ),	// NodInd=-NodInd-DiffBegin
                               -((long) Bottom.End  ),
                               -((long) Bottom.Begin));	// All indexes in bottom part

      /***** Unlock table *****/
      DB_UnlockTables ();

      return Err_SUCCESS;
     }

   return Err_ERROR;
  }

/*****************************************************************************/
/******** Get previous brother node to a given node in current course ********/
/*****************************************************************************/
// Return -1 if no previous brother

static int Tre_GetPrevBrother (int NumNode)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if node is the first one, there is no previous brother *****/
   if (NumNode <= 0 ||
       NumNode >= (int) Tre_Gbl.List.NumNodes)
      return -1;

   /***** Get previous brother before node *****/
   // 1 <= NumItem < Prg_Gbl.List.NumItems
   Level = Tre_GetLevelFromNumNode (NumNode);
   for (i  = NumNode - 1;
	i >= 0;
	i--)
     {
      if (Tre_GetLevelFromNumNode (i) == Level)
	 return i;	// Previous brother before node found
      if (Tre_GetLevelFromNumNode (i) < Level)
	 return -1;	// Previous lower level found ==> there are no brothers before node
     }
   return -1;	// Start reached ==> there are no brothers before node
  }

/*****************************************************************************/
/********** Get next brother node to a given node in current course **********/
/*****************************************************************************/
// Return -1 if no next brother

static int Tre_GetNextBrother (int NumNode)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if node is the last one, there is no next brother *****/
   if (NumNode < 0 ||
       NumNode >= (int) Tre_Gbl.List.NumNodes - 1)
      return -1;

   /***** Get next brother after node *****/
   // 0 <= NumItem < Prg_Gbl.List.NumItems - 1
   Level = Tre_GetLevelFromNumNode (NumNode);
   for (i = NumNode + 1;
	i < (int) Tre_Gbl.List.NumNodes;
	i++)
     {
      if (Tre_GetLevelFromNumNode (i) == Level)
	 return i;	// Next brother found
      if (Tre_GetLevelFromNumNode (i) < Level)
	 return -1;	// Next lower level found ==> there are no brothers after node
     }
   return -1;	// End reached ==> there are no brothers after node
  }

/*****************************************************************************/
/************** Move a subtree to left/right in a course program *************/
/*****************************************************************************/

void Tre_MoveLeftRightNode (Inf_Type_t InfoType,Tre_MoveLeftRight_t LeftRight)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Tre_Node Node;
   unsigned NumNode;
   struct Tre_NodeRange ToMove;
   static bool (*CheckIfAllowed[Tre_NUM_MOVEMENTS_LEFT_RIGHT])(unsigned NumNode) =
     {
      [Tre_MOVE_LEFT ] = Tre_CheckIfMoveLeftIsAllowed,
      [Tre_MOVE_RIGHT] = Tre_CheckIfMoveRightIsAllowed,
     };

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Move up/down node *****/
   NumNode = Tre_GetNumNodeFromNodCod (Node.Hierarchy.NodCod);
   if (CheckIfAllowed[LeftRight](NumNode))
     {
      /* Indexes of items */
      Tre_SetNodeRangeWithAllChildren (NumNode,&ToMove);

      /* Move node and its children to left or right */
      Tre_DB_MoveLeftRightNodeRange (InfoType,&ToMove,LeftRight);

      /* Update list of tree nodes */
      Tre_FreeListNodes ();
      Tre_GetListNodes (InfoType);
     }
   else
     {
      /* Show course program without highlighting any node */
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);
      Node.Hierarchy.NodCod = -1L;
     }
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (Tre_EDIT_NODES,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*************** Move a subtree to left/right in a course tree ***************/
/*****************************************************************************/

void Tre_ExpandContractNode (Inf_Type_t InfoType,
			     Tre_ExpandContract_t ExpandContract,
			     Tre_ListingType_t ListingType)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (InfoType);

   /***** Get tree node *****/
   Node.InfoType = InfoType;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Add/remove node to/from table of expanded nodes *****/
   switch (ExpandContract)
     {
      case Tre_EXPAND:
	 Tre_DB_InsertNodeInExpandedNodes (Node.Hierarchy.NodCod);
	 break;
      case Tre_CONTRACT:
	 Tre_DB_RemoveNodeFromExpandedNodes (Node.Hierarchy.NodCod);
	 break;
     }

   /***** Show items highlighting subtree *****/
   Node.Item.Cod = -1L;
   Tre_ShowAllNodes (ListingType,&Node);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

static void Tre_SetNodeRangeWithAllChildren (unsigned NumNode,struct Tre_NodeRange *NodeRange)
  {
   /***** List of nodes must be filled *****/
   if (!Tre_Gbl.List.IsRead)
      Err_WrongItemsListExit ();

   /***** Number of node must be in the correct range *****/
   if (NumNode >= Tre_Gbl.List.NumNodes)
      Err_WrongItemExit ();

   /***** Range includes this node and all its children *****/
   NodeRange->Begin = Tre_GetNodIndFromNumNode (NumNode);
   NodeRange->End   = Tre_GetNodIndFromNumNode (Tre_GetLastChild (NumNode));
  }

/*****************************************************************************/
/********************** Get last child in current course *********************/
/*****************************************************************************/

static unsigned Tre_GetLastChild (int NumNode)
  {
   unsigned Level;
   int i;

   /***** Trivial check: if node is wrong, there are no children *****/
   if (NumNode < 0 ||
       NumNode >= (int) Tre_Gbl.List.NumNodes)
      Err_WrongItemExit ();

   /***** Get next brother after node *****/
   // 0 <= NumItem < Prg_Gbl.List.NumItems
   Level = Tre_GetLevelFromNumNode (NumNode);
   for (i = NumNode + 1;
	i < (int) Tre_Gbl.List.NumNodes;
	i++)
     {
      if (Tre_GetLevelFromNumNode (i) <= Level)
	 return i - 1;	// Last child found
     }
   return Tre_Gbl.List.NumNodes - 1;	// End reached ==> all items after the given node are its children
  }

/*****************************************************************************/
/***************** Write a tree into a temporary HTML file *******************/
/*****************************************************************************/

void Tre_WriteTreeIntoHTMLTmpFile (Inf_Type_t InfoType,FILE *FileHTMLTmp)
  {
   extern const char *Txt_INFO_TITLE[Inf_NUM_TYPES];
   unsigned NumNode;
   struct Tre_Node Node;

   /***** Create levels *****/
   Tre_SetMaxNodeLevel (Tre_CalculateMaxNodeLevel ());
   Tre_CreateLevels ();

   /***** Write start of HTML code *****/
   Lay_BeginHTMLFile (FileHTMLTmp,Txt_INFO_TITLE[InfoType]);
   fprintf (FileHTMLTmp,"<body>\n"
                        "<table>\n");

   /***** Write all items of the current syllabus into text buffer *****/
   for (NumNode = 0;
	NumNode < Tre_Gbl.List.NumNodes;
	NumNode++)
     {
      Node.InfoType = InfoType;
      Node.Hierarchy.NodCod = Tre_GetNodCodFromNumNode (NumNode);
      Tre_GetNodeDataByCod (&Node);

      /***** Begin the row *****/
      fprintf (FileHTMLTmp,"<tr>");

      /***** Indent depending on the level *****/
      if (Tre_Gbl.List.Nodes[NumNode].Level > 1)
       	 fprintf (FileHTMLTmp,"<td colspan=\"%u\">"
      		              "</td>",
      		  Tre_Gbl.List.Nodes[NumNode].Level - 1);

      /***** Code of the node *****/
      fprintf (FileHTMLTmp,"<td class=\"RT\">");
      Tre_IncreaseNumberInLevel (Tre_Gbl.List.Nodes[NumNode].Level);
      fprintf (FileHTMLTmp,"%u",Tre_GetCurrentNumberInLevel (Tre_Gbl.List.Nodes[NumNode].Level));
      fprintf (FileHTMLTmp,"</td>");

      /***** Text of the item *****/
      fprintf (FileHTMLTmp,"<td colspan=\"%u\" class=\"LT\">"
      			   "%s"
      			   "</td>",
      	       Tre_Gbl.MaxLevel - Tre_Gbl.List.Nodes[NumNode].Level + 1,
      	       Node.Title);

      /***** End of the row *****/
      fprintf (FileHTMLTmp,"</tr>\n");
     }

   fprintf (FileHTMLTmp,"</table>\n"
			"</html>\n"
			"</body>\n");

   /***** Free levels *****/
   Tre_FreeLevels ();
  }
