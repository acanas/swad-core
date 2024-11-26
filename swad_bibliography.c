// swad_bibliography.c: bibliographic references

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
#include "swad_bibliography.h"
#include "swad_bibliography_database.h"
#include "swad_bibliography_type.h"
#include "swad_box.h"
#include "swad_button.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_exam_database.h"
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
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Bib_WriteTitle (const char Title[Bib_MAX_BYTES_TITLE + 1],
			    HidVis_HiddenOrVisible_t HiddenOrVisible);
static void Bib_WriteDescription (const char Description[Bib_MAX_BYTES_TITLE + 1],
			          HidVis_HiddenOrVisible_t HiddenOrVisible);
static void Bib_WriteWWW (const struct Tre_Node *Node,
			  HidVis_HiddenOrVisible_t HiddenOrVisible);

/*****************************************************************************/
/************* Reset specific fields of bibliographic reference **************/
/*****************************************************************************/

void Bib_ResetSpcFields (struct Tre_Node *Node)
  {
   Node->Bib.Title[0] = '\0';
   Node->Bib.Description[0] = '\0';
   Node->Bib.WWW[0] = '\0';
  }

/*****************************************************************************/
/******************** Get bibliographic reference data ***********************/
/*****************************************************************************/

void Bib_GetBibRefDataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NodCod	row[0]
   BibCod	row[1]
   BibInd	row[2]
   Hidden	row[3]
   Title	row[4]
   Description	row[5]
   WWW		row[6]
   */
   /***** Get code of the tree node (row[0]) *****/
   Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the bibliographic reference (row[1], row[2]) *****/
   Node->SpcItem.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Node->SpcItem.Ind = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->SpcItem.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

   /***** Get title, description and URL
          of the bibliographic reference (row[4], row[5], row[6]) *****/
   Str_Copy (Node->Bib.Title      ,row[4],sizeof (Node->Bib.Title      ) - 1);
   Str_Copy (Node->Bib.Description,row[5],sizeof (Node->Bib.Description) - 1);
   Str_Copy (Node->Bib.WWW        ,row[6],sizeof (Node->Bib.WWW        ) - 1);
  }

/*****************************************************************************/
/******************* Show one bibliographic reference ************************/
/*****************************************************************************/

void Bib_WriteCellViewBibRef (struct Tre_Node *Node,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   Bib_WriteTitle (Node->Bib.Title,HiddenOrVisible);
   HTM_BR ();
   Bib_WriteDescription (Node->Bib.Description,HiddenOrVisible);
   HTM_BR ();
   Bib_WriteWWW (Node,HiddenOrVisible);
  }

/*****************************************************************************/
/******************** Edit one bibliographic reference ***********************/
/*****************************************************************************/

void Bib_WriteCellEditBibRef (struct Tre_Node *Node,
                              Vie_ViewType_t ViewType,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_URL;
   extern const char *Txt_Save_changes;

   switch (ViewType)
     {
      case Vie_VIEW:
	 /***** Show current bibliographic reference *****/
	 Bib_WriteCellViewBibRef (Node,HiddenOrVisible);
	 break;
      case Vie_EDIT:
	 /***** Show form to change bibliographic reference *****/
	 Frm_BeginFormAnchor (ActChgBibRef,TreSpc_LIST_ITEMS_SECTION_ID);
	    TreSpc_PutParItmCod (&Node->SpcItem.Cod);

	    /* Title */
	    HTM_INPUT_TEXT ("Title",Bib_MAX_CHARS_TITLE,Node->Bib.Title,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_Title,The_GetSuffix ());

	    /* Description */
	    HTM_BR ();
	    HTM_INPUT_TEXT ("Description",Bib_MAX_CHARS_TITLE,Node->Bib.Description,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_Description,The_GetSuffix ());

	    /* URL */
	    HTM_BR ();
	    HTM_INPUT_URL ("WWW",Node->Bib.WWW,
			   HTM_REQUIRED,
			   " placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			   Txt_URL,The_GetSuffix ());

	    /* Button to save changes */
	    HTM_BR ();
	    Btn_PutConfirmButtonInline (Txt_Save_changes);

	 Frm_EndForm ();
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
     }
  }

/*****************************************************************************/
/********************* Edit a new bibliographic reference ********************/
/*****************************************************************************/

void Bib_WriteCellNewBibRef (void)
  {
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_URL;
   extern const char *Txt_Save_changes;

   /***** Title *****/
   HTM_INPUT_TEXT ("Title",Bib_MAX_CHARS_TITLE,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_Title,The_GetSuffix ());

   /***** Description *****/
   HTM_BR ();
   HTM_INPUT_TEXT ("Description",Bib_MAX_CHARS_TITLE,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_Description,The_GetSuffix ());

   /***** URL *****/
   HTM_BR ();
   HTM_INPUT_URL ("WWW","",
		  HTM_REQUIRED,
		  " placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		  Txt_URL,The_GetSuffix ());

   /***** Button to save changes *****/
   HTM_BR ();
   Btn_PutConfirmButtonInline (Txt_Save_changes);
  }

/*****************************************************************************/
/******************* Write title, description and URL ************************/
/*****************************************************************************/

static void Bib_WriteTitle (const char Title[Bib_MAX_BYTES_TITLE + 1],
			    HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];

   HTM_SPAN_Begin ("class=\"TRE_TIT PRG_TXT_%s%s\"",
		   The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
      HTM_Txt (Title);
   HTM_SPAN_End ();
  }

static void Bib_WriteDescription (const char Description[Bib_MAX_BYTES_TITLE + 1],
			          HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];

   HTM_SPAN_Begin ("class=\"PRG_TXT_%s%s\"",
		   The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
      HTM_Txt (Description);
   HTM_SPAN_End ();
  }

static void Bib_WriteWWW (const struct Tre_Node *Node,
			  HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];

   HTM_A_Begin ("href=\"%s\" title=\"%s\" target=\"_blank\""
	        " class=\"PAR PRG_TXT_%s%s\"",
		Node->Bib.WWW,Node->Bib.Title,
		The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
      HTM_Txt (Node->Bib.WWW);
   HTM_A_End ();
  }

/*****************************************************************************/
/********************* Create bibliographic reference ************************/
/*****************************************************************************/

void Bib_CreateBibRef (struct Tre_Node *Node)
  {
   /***** Get title, description and URL for the new bibliographic reference *****/
   Par_GetParText ("Title"      ,Node->Bib.Title      ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Description",Node->Bib.Description,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("WWW"        ,Node->Bib.WWW        ,WWW_MAX_BYTES_WWW);

   /***** Create bibliographic reference *****/
   Node->SpcItem.Cod = Bib_DB_CreateBibRef (Node);
  }

/*****************************************************************************/
/*********************** Change bibliographic reference ***********************/
/*****************************************************************************/

void Bib_ChangeBibRef (struct Tre_Node *Node)
  {
   /***** Get title, description and URL *****/
   Par_GetParText ("Title"      ,Node->Bib.Title      ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Description",Node->Bib.Description,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("WWW"        ,Node->Bib.WWW        ,WWW_MAX_BYTES_WWW);

   /***** Update answer *****/
   Bib_DB_UpdateBibRef (Node);
  }
