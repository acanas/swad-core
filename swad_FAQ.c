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
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_button.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_database.h"
#include "swad_FAQ.h"
#include "swad_FAQ_database.h"
#include "swad_form.h"
#include "swad_forum_database.h"
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
#include "swad_tree_specific.h"
#include "swad_user_database.h"
#include "swad_view.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void FAQ_PutIconsViewRes (void *Node);
static void FAQ_PutIconsEditRes (void *Node);

static void FAQ_GetQaADataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node);
static void FAQ_WriteRowViewQaA (unsigned NumQaA,struct Tre_Node *Node,
				 HidVis_HiddenOrVisible_t HiddenOrVisible);
static void FAQ_WriteRowEditQaA (unsigned NumQaA,unsigned NumQaAs,
                                 struct Tre_Node *Node,
                                 Vie_ViewType_t AnswerViewType);
static void FAQ_WriteRowNewQaA (unsigned NumQaAs,struct Tre_Node *Node);
static void FAQ_PutFormsToRemEditOneQaA (struct Tre_Node *Node,
                                         unsigned NumQaA,unsigned NumQaAs);
static void FAQ_PutParQaACod (void *QaACod);

/*****************************************************************************/
/***************** Reset specific fields of question&answer ******************/
/*****************************************************************************/

void FAQ_ResetSpcFields (struct Tre_Node *Node)
  {
   Node->QaA.Question[0] = '\0';
   Node->QaA.Answer[0] = '\0';
  }

/*****************************************************************************/
/****************************** List questions *******************************/
/*****************************************************************************/

void FAQ_ListNodeQaAs (Tre_ListingType_t ListingType,
                       struct Tre_Node *Node,
                       long SelectedNodCod,
                       long SelectedQaACod,
                       HidVis_HiddenOrVisible_t HiddenOrVisible)
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
      [Tre_EDIT_SPC_LIST_ITEMS		] = Vie_EDIT,
      [Tre_EDIT_SPC_ITEM		] = Vie_EDIT,
      [Tre_CHG_SPC_ITEM			] = Vie_EDIT,
      [Tre_END_EDIT_SPC_LIST_ITEMS	] = Vie_EDIT,
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
      [Tre_EDIT_SPC_LIST_ITEMS		] = Vie_EDIT,
      [Tre_EDIT_SPC_ITEM		] = Vie_EDIT,
      [Tre_CHG_SPC_ITEM			] = Vie_EDIT,
      [Tre_END_EDIT_SPC_LIST_ITEMS	] = Vie_VIEW,
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
	 HTM_SECTION_Begin (TreSpc_LIST_ITEMS_SECTION_ID);

	    /***** Show possible alerts *****/
	    if (Gbl.Action.Act == ActReqRemFAQQaA)
	       /* Alert with button to remove question&answer */
	       Ale_ShowLastAlertAndButton (ActRemFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,NULL,
					   FAQ_PutParQaACod,&SelectedQaACod,
					   Btn_REMOVE_BUTTON,Txt_Remove);
	    else
	       Ale_ShowAlerts (TreSpc_LIST_ITEMS_SECTION_ID);
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
		     FAQ_WriteRowViewQaA (NumQaA,Node,HiddenOrVisible);
		     break;
		  case Vie_EDIT:
		     FAQ_WriteRowEditQaA (NumQaA,NumQaAs,Node,
					  (ListingType == Tre_EDIT_SPC_ITEM &&
					   Node->ListItem.Cod == SelectedQaACod) ? Vie_EDIT :
										   Vie_VIEW);
		     break;
		  default:
		     Err_WrongTypeExit ();
		     break;
		 }
	      }

	    /***** Form to create a new question&answer *****/
	    if (ViewingOrEditingQaAOfThisNode == Vie_EDIT)
	      {
	       TreSpc_ResetListItem (Node);
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
	    Ico_PutContextualIconToView (ActFrmSeeFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);
  }

static void FAQ_PutIconsEditRes (void *Node)
  {
   if (Node)
      if (((struct Tre_Node *) Node)->Hierarchy.NodCod > 0)
	 if (Tre_CheckIfICanEditTree () == Usr_CAN)
	    Ico_PutContextualIconToEdit (ActFrmEdiFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,Node);
  }

/*****************************************************************************/
/************** Get node question&answer data using its code *****************/
/*****************************************************************************/

void FAQ_GetQaADataByCod (struct Tre_Node *Node)
  {
   MYSQL_RES *mysql_res;

   if (Node->ListItem.Cod > 0)
     {
      /***** Get data of question&answer *****/
      if (FAQ_DB_GetQaADataByCod (&mysql_res,Node->ListItem.Cod))
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
   Node->ListItem.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Node->ListItem.Ind = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->ListItem.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

   /***** Get the questionand the answer of the question&answer (row[4], row[5]) *****/
   Str_Copy (Node->QaA.Question,row[4],sizeof (Node->QaA.Question) - 1);
   Str_Copy (Node->QaA.Answer  ,row[5],sizeof (Node->QaA.Answer  ) - 1);
  }

/*****************************************************************************/
/*********************** Show one Question&Answer ****************************/
/*****************************************************************************/

static void FAQ_WriteRowViewQaA (unsigned NumQaA,struct Tre_Node *Node,
				 HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_PrgClass[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Question&Answer number *****/
      HTM_TD_Begin ("class=\"PRG_NUM PRG_RSC_%s RT\"",The_GetSuffix ());
	 HTM_Unsigned (NumQaA + 1);
      HTM_TD_End ();

      /***** Question&Answer *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT PRG_RSC_%s\"",The_GetSuffix ());
	 /* Question */
	 HTM_Txt (Node->QaA.Question);

	 // HTM_BR ();

	 /* Answer */
	 Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			   Node->QaA.Answer,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
	 ALn_InsertLinks (Node->QaA.Answer,Cns_MAX_BYTES_TEXT,60);	// Insert links
	 HTM_DIV_Begin ("class=\"PAR PRG_TXT_%s%s\"",
			The_GetSuffix (),HidVis_PrgClass[HiddenOrVisible]);
	    HTM_Txt (Node->QaA.Answer);
	 HTM_DIV_End ();

      HTM_TD_End ();

   /***** End row *****/
   HTM_TR_End ();
  }

/*****************************************************************************/
/************************* Edit one question&answer **************************/
/*****************************************************************************/

static void FAQ_WriteRowEditQaA (unsigned NumQaA,unsigned NumQaAs,
                                 struct Tre_Node *Node,
                                 Vie_ViewType_t AnswerViewType)
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
	 Frm_BeginFormAnchor (ActRenFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID);
	    ParCod_PutPar (ParCod_QaA,Node->ListItem.Cod);
	    HTM_INPUT_TEXT ("Question",FAQ_MAX_CHARS_QUESTION,Node->QaA.Question,
			    HTM_SUBMIT_ON_CHANGE,
			    "class=\"PRG_RSC_INPUT INPUT_%s\"",
			    The_GetSuffix ());
	 Frm_EndForm ();

         HTM_BR ();

	 /* Show current answer / Show textarea to change answer */
         switch (AnswerViewType)
           {
            case Vie_VIEW:
	       /* Show current answer */
	       HTM_Txt (Node->QaA.Answer);
               break;
            case Vie_EDIT:
	       /* Show textarea to change answer */
	       Frm_BeginFormAnchor (ActChgFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID);
		  ParCod_PutPar (ParCod_QaA,Node->ListItem.Cod);
		  HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				      "name=\"Answer\" rows=\"4\""
				      " class=\"PRG_RSC_INPUT INPUT_%s\""
				      " onchange=\"this.form.submit();return false;\"",
				      The_GetSuffix ());
		     HTM_Txt (Node->QaA.Answer);
		  HTM_TEXTAREA_End ();
	       Frm_EndForm ();
               break;
	    default:
	       Err_WrongTypeExit ();
	       break;
           }

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

      /***** Question and answer *****/
      HTM_TD_Begin ("class=\"PRG_MAIN LT %s\"",The_GetColorRows1 (1));

         /* Question */
	 Frm_BeginFormAnchor (ActNewFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID);
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
	    Ico_PutContextualIconToRemove (ActReqRemFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,
					   FAQ_PutParQaACod,&Node->ListItem.Cod);
	 else
	    Ico_PutIconRemovalNotAllowed ();

	 /***** Icon to hide/unhide question&answer *****/
	 if (NumQaA < NumQaAs)
	    Ico_PutContextualIconToHideUnhide (ActionHideUnhide,TreSpc_LIST_ITEMS_SECTION_ID,
					       FAQ_PutParQaACod,&Node->ListItem.Cod,
					       Node->ListItem.HiddenOrVisible);
	 else
	    Ico_PutIconOff ("eye.svg",Ico_GREEN,Txt_Visible);

	 /***** Put icon to edit the question&answer *****/
	 if (NumQaA < NumQaAs)
	    Ico_PutContextualIconToEdit (ActFrmChgFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,
					 FAQ_PutParQaACod,&Node->ListItem.Cod);
	 else
	    Ico_PutContextualIconToEdit (ActChgFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,
					 Tre_PutPars,&Node);

	 /***** Icon to move up the question&answer *****/
	 if (NumQaA > 0 && NumQaA < NumQaAs)
	    Lay_PutContextualLinkOnlyIcon (ActUp_FAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,
	                                   FAQ_PutParQaACod,&Node->ListItem.Cod,
	 			           "arrow-up.svg",Ico_BLACK);
	 else
	    Ico_PutIconOff ("arrow-up.svg",Ico_BLACK,Txt_Movement_not_allowed);

	 /***** Put icon to move down the question&answer *****/
	 if (NumQaA < NumQaAs - 1)
	    Lay_PutContextualLinkOnlyIcon (ActDwnFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID,
	                                   FAQ_PutParQaACod,&Node->ListItem.Cod,
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

void FAQ_CreateQaA (struct Tre_Node *Node)
  {
   /***** Get the new question for the new question&answer *****/
   Par_GetParText ("Question",Node->QaA.Question,FAQ_MAX_BYTES_QUESTION);

   /***** Create question&answer *****/
   Node->ListItem.Cod = FAQ_DB_CreateQaA (Node);
  }

/*****************************************************************************/
/******************** Rename question in a question&answer *******************/
/*****************************************************************************/

void FAQ_RenameQaA (const struct Tre_Node *Node)
  {
   char NewQuestion[FAQ_MAX_BYTES_QUESTION + 1];

   /***** Get the new question for the question&answer *****/
   Par_GetParText ("Question",NewQuestion,FAQ_MAX_BYTES_QUESTION);

   /***** Update database changing old title by new title *****/
   FAQ_DB_UpdateQaAQuestion (Node->Hierarchy.NodCod,Node->ListItem.Cod,
			     NewQuestion);
  }

/*****************************************************************************/
/*************************** Change question&answer **************************/
/*****************************************************************************/

void FAQ_ChangeAnswer (struct Tre_Node *Node)
  {
   /***** Get answer *****/
   Par_GetParHTML ("Answer",Node->QaA.Answer,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Update answer *****/
   FAQ_DB_UpdateAnswer (Node);
  }

/*****************************************************************************/
/***************************** Get link parameter ****************************/
/*****************************************************************************/

bool FAQ_GetParQuestion (struct Rsc_Link *Link)	// TODO: cHANGE!!!!!!!!!!!!!!!!
  {
   char TypeCod[3 + 1 + Cns_MAX_DIGITS_LONG + 1];
   char TypeStr[3 + 1];
   long Cod;

   /***** Get link type and code *****/
   Par_GetParText ("Question",TypeCod,sizeof (TypeCod) - 1);
   if (sscanf (TypeCod,"%3s_%ld",TypeStr,&Cod) == 2)
     {
      /* Correct link found */
      // Link->Type = Rsc_GetTypeFromString (TypeStr);
      // Link->Cod  = Cod;
      return true;
     }

   /* No link found */
   Link->Type = Rsc_NONE;
   Link->Cod  = -1L;
   return false;
  }
