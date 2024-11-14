// swad_FAQ.c: course FAQ

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
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_assignment_database.h"
#include "swad_attendance_database.h"
#include "swad_box.h"
// #include "swad_browser_resource.h"
#include "swad_button.h"
// #include "swad_call_for_exam_resource.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_database.h"
#include "swad_FAQ.h"
#include "swad_FAQ_database.h"
#include "swad_form.h"
#include "swad_forum_database.h"
// #include "swad_forum_resource.h"
#include "swad_game_database.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_project_database.h"
#include "swad_program.h"
#include "swad_role.h"
#include "swad_rubric_database.h"
#include "swad_survey_database.h"
#include "swad_tag_database.h"
#include "swad_theme.h"
#include "swad_timetable.h"
#include "swad_user_database.h"
#include "swad_view.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define FAQ_NUM_MOVEMENTS_UP_DOWN 2
typedef enum
  {
   FAQ_MOVE_UP,
   FAQ_MOVE_DOWN,
  } FAQ_MoveUpDown_t;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static const char *FAQ_QaA_SECTION_ID = "qaa_section";

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void FAQ_PutIconsViewRes (void *Node);
static void FAQ_PutIconsEditRes (void *Node);

static void FAQ_GetQaADataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node);
static void FAQ_WriteRowViewQaA (unsigned NumQaA,const struct Tre_Node *Node);
static void FAQ_WriteRowEditQaA (unsigned NumQaA,unsigned NumQaAs,
                                 struct Tre_Node *Node);
static void FAQ_WriteRowNewQaA (unsigned NumQaAs,struct Tre_Node *Node);
static void FAQ_PutFormsToRemEditOneQaA (struct Tre_Node *Node,
                                         unsigned NumQaA,unsigned NumQaAs);
static void FAQ_PutParQaACod (void *QaACod);
static void FAQ_HideOrUnhideQaA (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void FAQ_MoveUpDownQaA (FAQ_MoveUpDown_t UpDown);
static bool FAQ_ExchangeQaA (const struct Tre_Node *Node,
                             const struct FAQ_QaAHierarchy *QaA2);

/*****************************************************************************/
/*************************** Reset Question&Answer ***************************/
/*****************************************************************************/

void FAQ_ResetQaA (struct FAQ_QaA *QaA)
  {
   QaA->Hierarchy.QaACod = -1L;
   QaA->Hierarchy.QaAInd = 0;
   QaA->Hierarchy.HiddenOrVisible = HidVis_VISIBLE;
   QaA->Question[0] = '\0';
   QaA->Answer[0] = '\0';
  }

/*****************************************************************************/
/************************** View quetions&answers ****************************/
/*****************************************************************************/

void FAQ_ViewQaAsAfterEdit (void)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get tree node *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_END_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,-1L);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/************************** Edit questions&answers ***************************/
/*****************************************************************************/

void FAQ_EditQaAs (void)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get tree node *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,-1L);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/****************************** List questions *******************************/
/*****************************************************************************/

void FAQ_ListNodeQaAs (Tre_ListingType_t ListingType,
                       struct Tre_Node *Node,
                       long SelectedNodCod,
                       long SelectedQaACod)
  {
   extern const char *Hlp_COURSE_FAQ;
   extern const char *Txt_Remove;
   extern const char *Txt_Questions;
   MYSQL_RES *mysql_res;
   unsigned NumQaA;
   unsigned NumQaAs;
   Vie_ViewType_t ViewingOrEditingQaAOfThisNode;
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
      [Tre_EDIT_PRG_RESOURCES		] = Vie_EDIT,
      [Tre_EDIT_PRG_RESOURCE_LINK	] = Vie_EDIT,
      [Tre_CHG_PRG_RESOURCE_LINK	] = Vie_EDIT,
      [Tre_END_EDIT_PRG_RESOURCES	] = Vie_EDIT,
     };
   static Vie_ViewType_t ViewingOrEditingQaA[Tre_NUM_LISTING_TYPES] =
     {
      [Tre_PRINT			] = Vie_VIEW,
      [Tre_VIEW				] = Vie_VIEW,
      [Tre_EDIT_NODES			] = Vie_VIEW,
      [Tre_FORM_NEW_END_NODE		] = Vie_VIEW,
      [Tre_FORM_NEW_CHILD_NODE		] = Vie_VIEW,
      [Tre_FORM_EDIT_NODE		] = Vie_VIEW,
      [Tre_END_EDIT_NODE		] = Vie_VIEW,
      [Tre_RECEIVE_NODE			] = Vie_VIEW,
      [Tre_EDIT_PRG_RESOURCES		] = Vie_EDIT,
      [Tre_EDIT_PRG_RESOURCE_LINK	] = Vie_EDIT,
      [Tre_CHG_PRG_RESOURCE_LINK	] = Vie_EDIT,
      [Tre_END_EDIT_PRG_RESOURCES	] = Vie_VIEW,
     };
   static void (*FAQ_PutIconsRes[Vie_NUM_VIEW_TYPES]) (void *Node) =
     {
      [Vie_VIEW		] = FAQ_PutIconsEditRes,
      [Vie_EDIT		] = FAQ_PutIconsViewRes,
      [Vie_CONFIG	] = NULL,
      [Vie_PRINT	] = NULL,
     };

   /***** Trivial check *****/
   if (Node->Hierarchy.NodCod <= 0)
      return;

   /***** Get list of node questions&answers from database *****/
   NumQaAs = FAQ_DB_GetListQaAs (&mysql_res,Node->Hierarchy.NodCod,
                                 ViewingOrEditingQaA[ListingType] == Vie_EDIT);

   if (NumQaAs || ViewingOrEditing[ListingType] == Vie_EDIT)
     {
      if (Node->Hierarchy.NodCod == SelectedNodCod)
	{
	 /***** Begin section *****/
	 HTM_SECTION_Begin (FAQ_QaA_SECTION_ID);

	    /***** Show possible alerts *****/
	    if (Gbl.Action.Act == ActReqRemFAQQaA)
	       /* Alert with button to remove question&answer */
	       Ale_ShowLastAlertAndButton (ActRemFAQQaA,FAQ_QaA_SECTION_ID,NULL,
					   FAQ_PutParQaACod,&SelectedQaACod,
					   Btn_REMOVE_BUTTON,Txt_Remove);
	    else
	       Ale_ShowAlerts (FAQ_QaA_SECTION_ID);
	}

      /***** Begin box *****/
      ViewingOrEditingQaAOfThisNode = (ViewingOrEditingQaA[ListingType] == Vie_EDIT &&
				       Node->Hierarchy.NodCod == SelectedNodCod) ? Vie_EDIT :
										   Vie_VIEW;
      switch (ViewingOrEditing[ListingType])
        {
         case Vie_VIEW:
	    Box_BoxBegin (NULL,NULL,NULL,NULL,Box_NOT_CLOSABLE);
	    break;
         case Vie_EDIT:
	    Box_BoxBegin (Txt_Questions,
			  FAQ_PutIconsRes[ViewingOrEditingQaAOfThisNode],Node,
			  Hlp_COURSE_FAQ,Box_NOT_CLOSABLE);
	    break;
         default:
            Err_WrongTypeExit ();
            break;
        }

	 /***** Table *****/
	 HTM_TABLE_BeginWidePadding (2);

	    /***** Write all questions of this node *****/
	    for (NumQaA = 0;
		 NumQaA < NumQaAs;
		 NumQaA++)
	      {
	       /* Get data of this question&answer */
	       FAQ_GetQaADataFromRow (mysql_res,Node);

	       /* Show question&answer */
	       switch (ViewingOrEditingQaAOfThisNode)
		 {
		  case Vie_VIEW:
		     FAQ_WriteRowViewQaA (NumQaA,Node);
		     break;
		  case Vie_EDIT:
		     FAQ_WriteRowEditQaA (NumQaA,NumQaAs,Node);
		     break;
		  default:
		     Err_WrongTypeExit ();
		     break;
		 }
	      }

	    /***** Form to create a new question&answer *****/
	    if (ViewingOrEditingQaAOfThisNode == Vie_EDIT)
	      {
	       FAQ_ResetQaA (&(Node->QaA));
	       FAQ_WriteRowNewQaA (NumQaAs,Node);
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
/********** Put contextual icons in list of node questions&answers ***********/
/*****************************************************************************/

static void FAQ_PutIconsViewRes (void *Node)
  {
   if (Node)
      if (((struct Tre_Node *) Node)->Hierarchy.NodCod > 0)
	 if (Tre_CheckIfICanEditTree () == Usr_CAN)
	    Ico_PutContextualIconToView (ActFrmSeeFAQQaA,FAQ_QaA_SECTION_ID,
					 Tre_PutPars,Node);
  }

static void FAQ_PutIconsEditRes (void *Node)
  {
   if (Node)
      if (((struct Tre_Node *) Node)->Hierarchy.NodCod > 0)
	 if (Tre_CheckIfICanEditTree () == Usr_CAN)
	    Ico_PutContextualIconToEdit (ActFrmEdiFAQQaA,FAQ_QaA_SECTION_ID,
					 Tre_PutPars,Node);
  }

/*****************************************************************************/
/************** Get node question&answer data using its code *****************/
/*****************************************************************************/

void FAQ_GetQaADataByCod (struct Tre_Node *Node)
  {
   MYSQL_RES *mysql_res;

   if (Node->QaA.Hierarchy.QaACod > 0)
     {
      /***** Get data of question&answer *****/
      if (FAQ_DB_GetQaADataByCod (&mysql_res,Node->QaA.Hierarchy.QaACod))
         FAQ_GetQaADataFromRow (mysql_res,Node);
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
/************************ Get Question&Answer data ***************************/
/*****************************************************************************/

static void FAQ_GetQaADataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NodCod	row[0]
   QaACod	row[1]
   QaAInd	row[2]
   Hidden	row[3]
   Question	row[4]
   Answer	row[5]
   */
   /***** Get code of the tree node (row[0]) *****/
   Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the question&answer (row[1], row[2]) *****/
   Node->QaA.Hierarchy.QaACod = Str_ConvertStrCodToLongCod (row[1]);
   Node->QaA.Hierarchy.QaAInd = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->QaA.Hierarchy.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

   /***** Get the question of the question&answer (row[4]) *****/
   Str_Copy (Node->QaA.Question,row[4],sizeof (Node->QaA.Question) - 1);
   Str_Copy (Node->QaA.Answer,row[5],sizeof (Node->QaA.Answer) - 1);
  }

/*****************************************************************************/
/*********************** Show one Question&Answer ****************************/
/*****************************************************************************/

static void FAQ_WriteRowViewQaA (unsigned NumQaA,const struct Tre_Node *Node)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Question&Answer number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT\"",The_GetSuffix ());
	 HTM_Unsigned (NumQaA + 1);
      HTM_TD_End ();

      /***** Question&Answer *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT PRG_RSC_%s\"",The_GetSuffix ());
	 HTM_Txt (Node->QaA.Question);
	 HTM_BR ();
      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Edit one question&answer **************************/
/*****************************************************************************/

static void FAQ_WriteRowEditQaA (unsigned NumQaA,unsigned NumQaAs,
                                 struct Tre_Node *Node)
  {
   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this question&answer *****/
      HTM_TD_Begin ("class=\"PRG_RSC_COL1 LT\"");
         FAQ_PutFormsToRemEditOneQaA (Node,NumQaA,NumQaAs);
      HTM_TD_End ();

      /***** Question&answer number *****/
      HTM_TD_Begin ("class=\"PRG_NUM LT PRG_RSC_%s RT\"",The_GetSuffix ());
	 HTM_Unsigned (NumQaA + 1);
      HTM_TD_End ();

      /***** Question and answer *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT PRG_RSC_%s\"",The_GetSuffix ());

         /* Question */
	 Frm_BeginFormAnchor (ActRenFAQQaA,FAQ_QaA_SECTION_ID);
	    ParCod_PutPar (ParCod_Rsc,Node->QaA.Hierarchy.QaACod);
	    HTM_INPUT_TEXT ("Title",FAQ_MAX_CHARS_QUESTION,Node->QaA.Question,
			    HTM_SUBMIT_ON_CHANGE,
			    "class=\"PRG_RSC_INPUT INPUT_%s\"",
			    The_GetSuffix ());
	 Frm_EndForm ();

         HTM_BR ();

         /* Answer */
         // TODO: Answer

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************ Edit a new question&answer *************************/
/*****************************************************************************/

static void FAQ_WriteRowNewQaA (unsigned NumQaAs,struct Tre_Node *Node)
  {
   extern const char *Txt_New_question;

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Forms to remove/edit this question&answer *****/
      HTM_TD_Begin ("class=\"PRG_RSC_COL1 LT %s\"",The_GetColorRows1 (1));
	 FAQ_PutFormsToRemEditOneQaA (Node,NumQaAs,NumQaAs);
      HTM_TD_End ();

      /***** Question&answer number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT %s\"",
                    The_GetSuffix (),The_GetColorRows1 (1));
	 HTM_Unsigned (NumQaAs + 1);
      HTM_TD_End ();

      /***** Title and link/clipboard *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT %s\"",The_GetColorRows1 (1));

         /* Question */
	 Frm_BeginFormAnchor (ActNewFAQQaA,FAQ_QaA_SECTION_ID);
	    ParCod_PutPar (ParCod_Nod,Node->Hierarchy.NodCod);
	    HTM_INPUT_TEXT ("Question",FAQ_MAX_CHARS_QUESTION,"",
			    HTM_SUBMIT_ON_CHANGE,
			    "placeholder=\"%s\""
			    " class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_New_question,The_GetSuffix ());
	 Frm_EndForm ();

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/***************** Put a link (form) to edit one tree node *******************/
/*****************************************************************************/

static void FAQ_PutFormsToRemEditOneQaA (struct Tre_Node *Node,
                                         unsigned NumQaA,unsigned NumQaAs)
  {
   static Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhFAQQaA,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidFAQQaA,	// Visible ==> action to hide
     };
   extern const char *Txt_Movement_not_allowed;
   extern const char *Txt_Visible;

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Icon to remove question&answer *****/
	 if (NumQaA < NumQaAs)
	    Ico_PutContextualIconToRemove (ActReqRemFAQQaA,FAQ_QaA_SECTION_ID,
					   FAQ_PutParQaACod,&Node->QaA.Hierarchy.QaACod);
	 else
	    Ico_PutIconRemovalNotAllowed ();

	 /***** Icon to hide/unhide question&answer *****/
	 if (NumQaA < NumQaAs)
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide,FAQ_QaA_SECTION_ID,
					       FAQ_PutParQaACod,&Node->QaA.Hierarchy.QaACod,
					       Node->QaA.Hierarchy.HiddenOrVisible);
	 else
	    Ico_PutIconOff ("eye.svg",Ico_GREEN,Txt_Visible);

	 /***** Put icon to edit the question&answer *****/
	 if (NumQaA < NumQaAs)
	    Ico_PutContextualIconToEdit (ActFrmChgFAQQaA,FAQ_QaA_SECTION_ID,
					 FAQ_PutParQaACod,&Node->QaA.Hierarchy.QaACod);
	 else
	    Ico_PutContextualIconToEdit (ActChgFAQQaA,FAQ_QaA_SECTION_ID,
					 Tre_PutPars,&Node);

	 /***** Icon to move up the question&answer *****/
	 if (NumQaA > 0 && NumQaA < NumQaAs)
	    Lay_PutContextualLinkOnlyIcon (ActUp_FAQQaA,FAQ_QaA_SECTION_ID,
	                                   FAQ_PutParQaACod,&Node->QaA.Hierarchy.QaACod,
	 			           "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the question&answer *****/
	 if (NumQaA < NumQaAs - 1)
	    Lay_PutContextualLinkOnlyIcon (ActDwnFAQQaA,FAQ_QaA_SECTION_ID,
	                                   FAQ_PutParQaACod,&Node->QaA.Hierarchy.QaACod,
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
/******************* Param used to edit a question&answer ********************/
/*****************************************************************************/

static void FAQ_PutParQaACod (void *QaACod)
  {
   if (QaACod)
      ParCod_PutPar (ParCod_QaA,*((long *) QaACod));
  }

/*****************************************************************************/
/************************** Create question&answer ***************************/
/*****************************************************************************/

void FAQ_CreateQaA (void)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get parameters *****/
   /* Get tree node */
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);

   /* Get the new question for the new question&answer */
   Par_GetParText ("Question",Node.QaA.Question,FAQ_MAX_BYTES_QUESTION);

   /***** Create question&answer *****/
   Node.QaA.Hierarchy.QaACod = FAQ_DB_CreateQaA (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*************************** Rename question&answer **************************/
/*****************************************************************************/

void FAQ_RenameQaA (void)
  {
   struct Tre_Node Node;
   char NewTitle[FAQ_MAX_BYTES_QUESTION + 1];

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get tree node and question&answer *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Rename question&answer *****/
   /* Get the new question for the question&answer */
   Par_GetParText ("Question",NewTitle,FAQ_MAX_BYTES_QUESTION);

   /* Update database changing old title by new title */
   FAQ_DB_UpdateQaAQuestion (Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod,NewTitle);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********** Ask for confirmation of removing a question&answer **************/
/*****************************************************************************/

void FAQ_ReqRemQaA (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_resource_X;
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get tree node and question&answer *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Create alert to remove the question&answer *****/
   Ale_CreateAlert (Ale_QUESTION,FAQ_QaA_SECTION_ID,
                    Txt_Do_you_really_want_to_remove_the_resource_X,
                    Node.QaA.Question);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*************************** Remove a question&answer ************************/
/*****************************************************************************/

void FAQ_RemoveQaA (void)
  {
   extern const char *Txt_Resource_X_removed;
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get data of the question&answer from database *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Remove question&answer *****/
   FAQ_DB_RemoveQaA (&Node);

   /***** Create alert to remove the question&answer *****/
   Ale_CreateAlert (Ale_SUCCESS,FAQ_QaA_SECTION_ID,
                    Txt_Resource_X_removed,Node.QaA.Question);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/***************************** Hide a tree node ***************************/
/*****************************************************************************/

void FAQ_HideQaA (void)
  {
   FAQ_HideOrUnhideQaA (HidVis_HIDDEN);
  }

void FAQ_UnhideQaA (void)
  {
   FAQ_HideOrUnhideQaA (HidVis_VISIBLE);
  }

static void FAQ_HideOrUnhideQaA (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get tree node and question&answer *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Hide/unhide question&answer *****/
   FAQ_DB_HideOrUnhideQaA (&Node,HiddenOrVisible);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*********************** Move up/down question&answer ************************/
/*****************************************************************************/

void FAQ_MoveUpQaA (void)
  {
   FAQ_MoveUpDownQaA (FAQ_MOVE_UP);
  }

void FAQ_MoveDownQaA (void)
  {
   FAQ_MoveUpDownQaA (FAQ_MOVE_DOWN);
  }

static void FAQ_MoveUpDownQaA (FAQ_MoveUpDown_t UpDown)
  {
   extern const char *Txt_Movement_not_allowed;
   struct Tre_Node Node;
   struct FAQ_QaAHierarchy FAQ2;
   bool Success = false;
   static unsigned (*GetOtherRscInd[FAQ_NUM_MOVEMENTS_UP_DOWN])(const struct Tre_Node *Node) =
     {
      [FAQ_MOVE_UP  ] = FAQ_DB_GetQaAIndBefore,
      [FAQ_MOVE_DOWN] = FAQ_DB_GetQaAIndAfter,
     };

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get tree node and question&answer *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Move up/down question&answer *****/
   if ((FAQ2.QaAInd = GetOtherRscInd[UpDown] (&Node)))	// 0 ==> movement not allowed
     {
      /* Get the other question&answer code */
      FAQ2.QaACod = FAQ_DB_GetQaACodFromQaAInd (Node.Hierarchy.NodCod,FAQ2.QaAInd);

      /* Exchange subtrees */
      Success = FAQ_ExchangeQaA (&Node,&FAQ2);
     }
   if (!Success)
      Ale_ShowAlert (Ale_WARNING,Txt_Movement_not_allowed);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCES,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/********** Exchange the order of two consecutive questions&answers **********/
/*****************************************************************************/
// Return true if success

static bool FAQ_ExchangeQaA (const struct Tre_Node *Node,
                             const struct FAQ_QaAHierarchy *QaA2)
  {
   const struct FAQ_QaAHierarchy *QaA1 = &Node->QaA.Hierarchy;

   if (QaA1->QaAInd > 0 &&	// Indexes should be in the range [1, 2,...]
       QaA2->QaAInd > 0)
     {
      /***** Lock tables to make the move atomic *****/
      FAQ_DB_LockTableQaAs ();

      /***** Exchange indexes of items *****/
      // This implementation works with non continuous indexes
      /*
      Example:
      Rsc1->Index =  5
      Rsc2->Index = 17
                                Step 1            Step 2            Step 3  (Equivalent to)
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      | QaAInd| QaACod|   | QaAInd| QaACod|   | QaAInd| QaACod|   | QaAInd| QaACod| | QaAInd| QaACod|
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      |     5 |   218 |   |     5 |   218 |-->|--> 17 |   218 |   |    17 |   218 | |     5 |   240 |
      |    17 |   240 |-->|-->-17 |   240 |   |   -17 |   240 |-->|-->  5 |   240 | |    17 |   218 |
      +-------+-------+   +-------+-------+   +-------+-------+   +-------+-------+ +-------+-------+
      */
      /* Step 1: Change second index to negative,
		 necessary to preserve unique index (NodCod,QaAInd) */
      FAQ_DB_UpdateQaAInd (Node,QaA2->QaACod,-(int) QaA2->QaAInd);

      /* Step 2: Change first index */
      FAQ_DB_UpdateQaAInd (Node,QaA1->QaACod, (int) QaA2->QaAInd);

      /* Step 3: Change second index */
      FAQ_DB_UpdateQaAInd (Node,QaA2->QaACod, (int) QaA1->QaAInd);

      /***** Unlock tables *****/
      DB_UnlockTables ();

      return true;	// Success
     }

   return false;	// No success
  }

/*****************************************************************************/
/*************** Edit FAQ with form to change question&answer ****************/
/*****************************************************************************/

void FAQ_EditFAQWithFormToChangeQaA (void)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get tree node and resource *****/
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCE_LINK,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/*************************** Change question&answer **************************/
/*****************************************************************************/

void FAQ_ChangeQaA (void)
  {
   struct Tre_Node Node;

   /***** Get list of tree nodes *****/
   Tre_GetListNodes (Inf_FAQ);

   /***** Get parameters *****/
   /* Get tree node */
   Node.InfoType = Inf_FAQ;
   Tre_GetPars (&Node);
   if (Node.Hierarchy.NodCod <= 0)
      Err_WrongItemExit ();

   /* Get the question and the answer */
   Par_GetParText ("Question",Node.QaA.Question,FAQ_MAX_BYTES_QUESTION);
   Par_GetParText ("Answer",Node.QaA.Answer,Cns_MAX_BYTES_TEXT);

   /***** Update question&answer *****/
   FAQ_DB_UpdateQaA (&Node);

   /***** Show current tree nodes, if any *****/
   Tre_ShowAllNodes (Inf_FAQ,Tre_EDIT_PRG_RESOURCE_LINK,
		     Node.Hierarchy.NodCod,Node.QaA.Hierarchy.QaACod);

   /***** Free list of tree nodes *****/
   Tre_FreeListNodes ();
  }

/*****************************************************************************/
/***************************** Get link parameter ****************************/
/*****************************************************************************/

bool FAQ_GetParQuestion (struct Rsc_Link *Link)
  {
   char TypeCod[3 + 1 + Cns_MAX_DIGITS_LONG + 1];
   char TypeStr[3 + 1];
   long Cod;

   /***** Get link type and code *****/
   Par_GetParText ("Question",TypeCod,sizeof (TypeCod) - 1);
   if (sscanf (TypeCod,"%3s_%ld",TypeStr,&Cod) == 2)
     {
      /* Correct link found */
      Link->Type = Rsc_GetTypeFromString (TypeStr);
      Link->Cod  = Cod;
      return true;
     }

   /* No link found */
   Link->Type = Rsc_NONE;
   Link->Cod  = -1L;
   return false;
  }
