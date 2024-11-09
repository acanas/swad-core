// swad_program.c: course program

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

#include "swad_box.h"
#include "swad_figure.h"
#include "swad_global.h"
#include "swad_program.h"
#include "swad_tree.h"
#include "swad_tree_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prg_BeforeTree (Tre_ListingType_t ListingType);
static void Prg_AfterTree (void);

static void Prg_PutIconsWhenViewing (__attribute__((unused)) void *Args);
static void Prg_PutIconsWhenEditing (__attribute__((unused)) void *Args);

/*****************************************************************************/
/*************************** Before and after tree ***************************/
/*****************************************************************************/

static void Prg_BeforeTree (Tre_ListingType_t ListingType)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;
   static void (*FunctionToDrawContextualIcons[Tre_NUM_LISTING_TYPES]) (void *Args) =
     {
      [Tre_VIEW		] = Prg_PutIconsWhenViewing,
      [Tre_EDIT_NODES	] = Prg_PutIconsWhenEditing,
     };

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,
		 FunctionToDrawContextualIcons[ListingType],NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);
  }

static void Prg_AfterTree (void)
  {
   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************************* Show program ********************************/
/*****************************************************************************/

void Prg_ShowProgram (void)
  {
   Prg_BeforeTree (Tre_VIEW);
      (void) Tre_ShowTree ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************************* Edit program ********************************/
/*****************************************************************************/

void Prg_EditTree (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_EditTree ();
   Prg_AfterTree ();
  }

/*****************************************************************************/
/********* List tree nodes when click on view a node after edition ***********/
/*****************************************************************************/

void Prg_ViewNodeAfterEdit (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ViewNodeAfterEdit ();
   Prg_AfterTree ();
  }

/*****************************************************************************/
/*********** List tree nodes with a form to change/create a node *************/
/*****************************************************************************/

void Prg_ReqChangeNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ReqChangeNode ();
   Prg_AfterTree ();
  }

void Prg_ReqCreateNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ReqCreateNode ();
   Prg_AfterTree ();
  }

/*****************************************************************************/
/*************** Receive form to change/create a tree node *******************/
/*****************************************************************************/

void Prg_ReceiveChgNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ReceiveChgNode ();
   Prg_AfterTree ();
  }

void Prg_ReceiveNewNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ReceiveNewNode ();
   Prg_AfterTree ();
  }

/*****************************************************************************/
/******************** Remove a tree node and its children ********************/
/*****************************************************************************/

void Prg_ReqRemNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ReqRemNode ();
   Prg_AfterTree ();
  }

void Prg_RemoveNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_RemoveNode ();
   Prg_AfterTree ();
  }

/*****************************************************************************/
/*************************** Hide/unhide a tree node *************************/
/*****************************************************************************/

void Prg_HideNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_HideOrUnhideNode (HidVis_HIDDEN);
   Prg_AfterTree ();
  }

void Prg_UnhideNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_HideOrUnhideNode (HidVis_VISIBLE);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/*********** Move up/down position of a subtree in a course tree *************/
/*****************************************************************************/

void Prg_MoveUpNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_MoveUpDownNode (Tre_MOVE_UP);
   Prg_AfterTree ();
  }

void Prg_MoveDownNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_MoveUpDownNode (Tre_MOVE_DOWN);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/************** Move a subtree to left/right in a course program *************/
/*****************************************************************************/

void Prg_MoveLeftNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_MoveLeftRightNode (Tre_MOVE_LEFT);
   Prg_AfterTree ();
  }

void Prg_MoveRightNode (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_MoveLeftRightNode (Tre_MOVE_RIGHT);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/****************** Expand/contract a node in a course tree ******************/
/*****************************************************************************/

void Prg_ExpandNodeSeeing (void)
  {
   Prg_BeforeTree (Tre_VIEW);
      Tre_ExpandContractNode (Tre_EXPAND,Tre_VIEW);
   Prg_AfterTree ();
  }

void Prg_ContractNodeSeeing (void)
  {
   Prg_BeforeTree (Tre_VIEW);
      Tre_ExpandContractNode (Tre_CONTRACT,Tre_VIEW);
   Prg_AfterTree ();
  }

void Prg_ExpandNodeEditing (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ExpandContractNode (Tre_EXPAND,Tre_EDIT_NODES);
   Prg_AfterTree ();
  }

void Prg_ContractNodeEditing (void)
  {
   Prg_BeforeTree (Tre_EDIT_NODES);
      Tre_ExpandContractNode (Tre_CONTRACT,Tre_EDIT_NODES);
   Prg_AfterTree ();
  }

/*****************************************************************************/
/**************** Put contextual icons in list of tree nodes *****************/
/*****************************************************************************/

static void Prg_PutIconsWhenViewing (__attribute__((unused)) void *Args)
  {
   struct Tre_Node Node;

   Node.InfoType = Inf_PROGRAM;
   Tre_ResetNode (&Node);

   /***** Put icon to edit tree *****/
   if (Tre_CheckIfICanEditTree () == Usr_CAN)
      Tre_PutIconToEditTree (&Node);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_COURSE_PROGRAMS);
  }

/*****************************************************************************/
/*************** Put contextual icons in list of tree nodes ******************/
/*****************************************************************************/

static void Prg_PutIconsWhenEditing (__attribute__((unused)) void *Args)
  {
   struct Tre_Node Node;

   Node.InfoType = Inf_PROGRAM;
   Tre_ResetNode (&Node);

   /***** Put icon to view program *****/
   Tre_PutIconToViewTree (&Node);

   if (Tre_CheckIfICanEditTree () == Usr_CAN)
     {
      /***** Put icon to create a new tree node *****/
      Tre_PutIconToCreateNewNode (&Node);

      /***** Put icon to view resource clipboard *****/
      Rsc_PutIconToViewClipboard ();
     }

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_COURSE_PROGRAMS);
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
   unsigned NumNodes;
   unsigned NumCoursesWithNodes;

   /***** Get the number of tree nodes from this location *****/
   if ((NumNodes = Tre_DB_GetNumNodes (Inf_PROGRAM,Gbl.Scope.Current)))
      NumCoursesWithNodes = Tre_DB_GetNumCoursesWithNodes (Inf_PROGRAM,Gbl.Scope.Current);
   else
      NumCoursesWithNodes = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_COURSE_PROGRAMS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_course_programs,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_program_items                ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_program_items,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_items_BR_per_course  ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of nodes *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Unsigned (NumNodes);
	 HTM_TD_Unsigned (NumCoursesWithNodes);
	 HTM_TD_Ratio (NumNodes,NumCoursesWithNodes);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
