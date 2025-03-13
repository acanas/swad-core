// swad_FAQ.c: course FAQ

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
/**************** Reset specific fields of question & answer *****************/
/*****************************************************************************/

void FAQ_ResetSpcFields (struct Tre_Node *Node)
  {
   Node->Item.QaA.Question[0] = '\0';
   Node->Item.QaA.Answer[0] = '\0';
  }

/*****************************************************************************/
/********************** Get question & answer data ***************************/
/*****************************************************************************/

void FAQ_GetQaADataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NodCod	row[0]
   ItmCod	row[1]
   ItmInd	row[2]
   Hidden	row[3]
   Question	row[4]
   Answer	row[5]
   */
   /***** Get code of the tree node (row[0]) *****/
   Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the question & answer (row[1], row[2]) *****/
   Node->Item.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Node->Item.Ind = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->Item.HiddenOrVisible = HidVis_GetHiddenOrVisibleFromYN (row[3][0]);

   /***** Get the questionand the answer of the question & answer (row[4], row[5]) *****/
   Str_Copy (Node->Item.QaA.Question,row[4],sizeof (Node->Item.QaA.Question) - 1);
   Str_Copy (Node->Item.QaA.Answer  ,row[5],sizeof (Node->Item.QaA.Answer  ) - 1);
  }

/*****************************************************************************/
/********************** Show one question & answer ***************************/
/*****************************************************************************/

void FAQ_WriteCellViewQaA (struct Tre_Node *Node)
  {
   /***** Question *****/
   HTM_SPAN_Begin ("class=\"ITM_TIT\"");
      HTM_Txt (Node->Item.QaA.Question);
   HTM_SPAN_End ();

   /***** Answer *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
		     Node->Item.QaA.Answer,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
   ALn_InsertLinks (Node->Item.QaA.Answer,Cns_MAX_BYTES_TEXT,60);	// Insert links

   HTM_DIV_Begin ("class=\"PAR\"");
      HTM_Txt (Node->Item.QaA.Answer);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************ Edit one question & answer *************************/
/*****************************************************************************/

void FAQ_WriteCellEditQaA (struct Tre_Node *Node,
                           Vie_ViewType_t ViewType,
			   HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Question;
   extern const char *Txt_Answer;

   switch (ViewType)
     {
      case Vie_VIEW:
	 /***** Show current question & answer *****/
	 HTM_DIV_Begin ("class=\"TRE_TXT_%s%s\"",
		        The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
	    FAQ_WriteCellViewQaA (Node);
	 HTM_DIV_End ();
	 break;
      case Vie_EDIT:
	 /***** Show form to change question & answer *****/
	 Frm_BeginFormAnchor (ActChgFAQQaA,TreSpc_LIST_ITEMS_SECTION_ID);
	    TreSpc_PutParItmCod (&Node->Item.Cod);

	    /* Question */
	    HTM_INPUT_TEXT ("Question",FAQ_MAX_CHARS_QUESTION,Node->Item.QaA.Question,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"ITM_INPUT INPUT_%s\"",
			    Txt_Question,The_GetSuffix ());

	    /* Answer */
	    HTM_BR ();
	    HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				"name=\"Answer\" rows=\"10\""
				" placeholder=\"%s\""
				" class=\"ITM_INPUT INPUT_%s\"",
				Txt_Answer,The_GetSuffix ());
	       HTM_Txt (Node->Item.QaA.Answer);
	    HTM_TEXTAREA_End ();

	    /* Button to save changes */
	    HTM_BR ();
	    Btn_PutButtonInline (Btn_SAVE_CHANGES);

	 Frm_EndForm ();
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
     }
  }

/*****************************************************************************/
/*********************** Edit a new question & answer ************************/
/*****************************************************************************/

void FAQ_WriteCellNewQaA (void)
  {
   extern const char *Txt_New_question;
   extern const char *Txt_Answer;

   /***** Question *****/
   HTM_INPUT_TEXT ("Question",FAQ_MAX_CHARS_QUESTION,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"ITM_INPUT INPUT_%s\"",
		   Txt_New_question,The_GetSuffix ());

   /***** Answer *****/
   HTM_BR ();
   HTM_TEXTAREA_Begin (HTM_NO_ATTR,
		       "name=\"Answer\" rows=\"10\""
		       " placeholder=\"%s\""
		       " class=\"ITM_INPUT INPUT_%s\"",
		       Txt_Answer,The_GetSuffix ());
   HTM_TEXTAREA_End ();

   /***** Button to save changes *****/
   HTM_BR ();
   Btn_PutButtonInline (Btn_CREATE);
  }

/*****************************************************************************/
/************* Get parameters to create/update question & answer *************/
/*****************************************************************************/

void FAQ_GetParsQaA (struct Tre_Node *Node)
  {
   Par_GetParText ("Question",Node->Item.QaA.Question,FAQ_MAX_BYTES_QUESTION);
   Par_GetParHTML ("Answer",Node->Item.QaA.Answer,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
  }
