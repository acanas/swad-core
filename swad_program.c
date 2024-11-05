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

static void Prg_PutIconsListNodes (__attribute__((unused)) void *Args);
static void Prg_PutIconsEditNodes (__attribute__((unused)) void *Args);

/*****************************************************************************/
/******************************* Show program ********************************/
/*****************************************************************************/

void Prg_ShowProgram (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsListNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      (void) Tre_ShowTree ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************************* Edit program ********************************/
/*****************************************************************************/

void Prg_EditProgram (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_EditTree ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********* List tree nodes when click on view a node after edition ***********/
/*****************************************************************************/

void Prg_ViewNodeAfterEdit (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ViewNodeAfterEdit ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************ List tree nodes with a form to change a given node *************/
/*****************************************************************************/

void Prg_ReqChangeNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ReqChangeNode ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************* List tree nodes with a form to create a new node **************/
/*****************************************************************************/

void Prg_ReqCreateNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ReqCreateNode ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Receive form to change an existing tree node ****************/
/*****************************************************************************/

void Prg_ReceiveChgNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ReceiveChgNode ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************* Receive form to create a new tree node ******************/
/*****************************************************************************/

void Prg_ReceiveNewNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ReceiveNewNode ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Ask for confirmation of removing a tree node ****************/
/*****************************************************************************/

void Prg_ReqRemNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ReqRemNode ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************** Remove a tree node and its children ********************/
/*****************************************************************************/

void Prg_RemoveNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_RemoveNode ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************************** Hide/unhide a tree node *************************/
/*****************************************************************************/

void Prg_HideNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_HideOrUnhideNode (HidVis_HIDDEN);

   /***** End box *****/
   Box_BoxEnd ();
  }

void Prg_UnhideNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_HideOrUnhideNode (HidVis_VISIBLE);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*********** Move up/down position of a subtree in a course tree *************/
/*****************************************************************************/

void Prg_MoveUpNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_MoveUpDownNode (Tre_MOVE_UP);

   /***** End box *****/
   Box_BoxEnd ();
  }

void Prg_MoveDownNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_MoveUpDownNode (Tre_MOVE_DOWN);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Move a subtree to left/right in a course program *************/
/*****************************************************************************/

void Prg_MoveLeftNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_MoveLeftRightNode (Tre_MOVE_LEFT);

   /***** End box *****/
   Box_BoxEnd ();
  }

void Prg_MoveRightNode (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_MoveLeftRightNode (Tre_MOVE_RIGHT);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*************** Move a subtree to left/right in a course tree ***************/
/*****************************************************************************/

void Prg_ExpandNodeSeeing (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsListNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ExpandContractNode (Tre_EXPAND,Tre_VIEW);

   /***** End box *****/
   Box_BoxEnd ();
  }

void Prg_ContractNodeSeeing (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsListNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ExpandContractNode (Tre_CONTRACT,Tre_VIEW);

   /***** End box *****/
   Box_BoxEnd ();
  }

void Prg_ExpandNodeEditing (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ExpandContractNode (Tre_EXPAND,Tre_EDIT_NODES);

   /***** End box *****/
   Box_BoxEnd ();
  }

void Prg_ContractNodeEditing (void)
  {
   extern const char *Hlp_COURSE_Program;
   extern const char *Txt_COURSE_program;

   /***** Begin box *****/
   Box_BoxBegin (Txt_COURSE_program,Prg_PutIconsEditNodes,NULL,
                 Hlp_COURSE_Program,Box_NOT_CLOSABLE);

      Tre_ExpandContractNode (Tre_CONTRACT,Tre_EDIT_NODES);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************** Put contextual icons in list of tree nodes *****************/
/*****************************************************************************/

static void Prg_PutIconsListNodes (__attribute__((unused)) void *Args)
  {
   struct Tre_Node Node;

   Node.TreeType = Tre_PROGRAM;
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

static void Prg_PutIconsEditNodes (__attribute__((unused)) void *Args)
  {
   struct Tre_Node Node;

   Node.TreeType = Tre_PROGRAM;
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
   if ((NumNodes = Tre_DB_GetNumNodes (Tre_PROGRAM,Gbl.Scope.Current)))
      NumCoursesWithNodes = Tre_DB_GetNumCoursesWithNodes (Tre_PROGRAM,Gbl.Scope.Current);
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
