// swad_link.c: course links

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
#include "swad_form.h"
#include "swad_forum_database.h"
#include "swad_game_database.h"
#include "swad_group_database.h"
#include "swad_hidden_visible.h"
#include "swad_HTML.h"
#include "swad_link.h"
#include "swad_link_database.h"
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
/************************* Private global variables **************************/
/*****************************************************************************/

extern const char *Txt_Title;
extern const char *Txt_Description;

static const char *Lnk_FormNames[Lnk_NUM_FIELDS] =
  {
   [Lnk_TITLE		] = "Title",
   [Lnk_DESCRIPTION	] = "Description",
  };

static const char **Lnk_Placeholders[Lnk_NUM_FIELDS] =
  {
   [Lnk_TITLE		] = &Txt_Title,
   [Lnk_DESCRIPTION	] = &Txt_Description,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Lnk_WriteField (const char *Field,const char *Class);

/*****************************************************************************/
/******************* Reset specific fields of course link ********************/
/*****************************************************************************/

void Lnk_ResetSpcFields (struct Tre_Node *Node)
  {
   unsigned NumField;

   for (NumField = 0;
	NumField < Lnk_NUM_FIELDS;
	NumField++)
      Node->Lnk.Fields[NumField][0] = '\0';
   Node->Lnk.WWW[0] = '\0';
  }

/*****************************************************************************/
/************************** Get course link data *****************************/
/*****************************************************************************/

void Lnk_GetCrsLinkDataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node)
  {
   MYSQL_ROW row;
   unsigned NumField;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NodCod	row[0]
   ItmCod	row[1]
   ItmInd	row[2]
   Hidden	row[3]
   Title	row[4]
   Description	row[5]
   WWW		row[6]
   */
   /***** Get code of the tree node (row[0]) *****/
   Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the course link (row[1], row[2]) *****/
   Node->SpcItem.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Node->SpcItem.Ind = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->SpcItem.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

   /***** Get title, description and URL
          of the course link (row[4], row[5], row[6]) *****/
   for (NumField = 0;
	NumField < Lnk_NUM_FIELDS;
	NumField++)
      Str_Copy (Node->Lnk.Fields[NumField],row[4 + NumField],Lnk_MAX_BYTES_FIELD);
   Str_Copy (Node->Lnk.WWW,row[4 + Lnk_NUM_FIELDS],sizeof (Node->Lnk.WWW) - 1);
  }

/*****************************************************************************/
/************************* Show one course link ******************************/
/*****************************************************************************/

void Lnk_WriteCellViewCrsLink (struct Tre_Node *Node)
  {
   static const char *Class[Lnk_NUM_FIELDS] =
     {
      [Lnk_TITLE] = "TRE_TIT",
     };
   unsigned NumField;

   for (NumField = 0;
	NumField < Lnk_NUM_FIELDS;
	NumField++)
      if (Node->Lnk.Fields[NumField][0])
	{
	 Lnk_WriteField (Node->Lnk.Fields[NumField],Class[NumField]);
	 HTM_BR ();
	}
   if (Node->Lnk.WWW[0])
     {
      HTM_A_Begin ("href=\"%s\" target=\"_blank\"",Node->Lnk.WWW);
	 HTM_Txt (Node->Lnk.WWW);
      HTM_A_End ();
     }
  }

/*****************************************************************************/
/************************ Edit one course link *************************/
/*****************************************************************************/

void Lnk_WriteCellEditCrsLink (struct Tre_Node *Node,
                               Vie_ViewType_t ViewType,
			       HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_URL;
   extern const char *Txt_Save_changes;
   static HTM_Attributes_t Attributes[Lnk_NUM_FIELDS] =
     {
      [Lnk_TITLE	] = HTM_REQUIRED,
      [Lnk_DESCRIPTION	] = HTM_NO_ATTR,
     };
   unsigned NumField;

   switch (ViewType)
     {
      case Vie_VIEW:
	 /***** Show current course link *****/
	 HTM_DIV_Begin ("class=\"PRG_TXT_%s%s\"",
		        The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
	    Lnk_WriteCellViewCrsLink (Node);
	 HTM_DIV_End ();
	 break;
      case Vie_EDIT:
	 /***** Show form to change course link *****/
	 Frm_BeginFormAnchor (ActChgCrsLnk,TreSpc_LIST_ITEMS_SECTION_ID);
	    TreSpc_PutParItmCod (&Node->SpcItem.Cod);

	    /* Fields */
	    for (NumField = 0;
		 NumField < Lnk_NUM_FIELDS;
		 NumField++)
	      {
	       HTM_INPUT_TEXT (Lnk_FormNames[NumField],Lnk_MAX_CHARS_FIELD,Node->Lnk.Fields[NumField],
			       Attributes[NumField],
			       "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			       *Lnk_Placeholders[NumField],The_GetSuffix ());
	       HTM_BR ();
	      }

	    /* URL */
	    HTM_INPUT_URL ("WWW",Node->Lnk.WWW,
			   HTM_REQUIRED,
			   " placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			   Txt_URL,The_GetSuffix ());
	    HTM_BR ();

	    /* Button to save changes */
	    Btn_PutConfirmButtonInline (Txt_Save_changes);

	 Frm_EndForm ();
	 break;
      default:
	 Err_WrongTypeExit ();
	 break;
     }
  }

/*****************************************************************************/
/************************** Edit a new course link ***************************/
/*****************************************************************************/

void Lnk_WriteCellNewCrsLink (void)
  {
   extern const char *Txt_Title;
   extern const char *Txt_Description;
   extern const char *Txt_URL;
   extern const char *Txt_Save_changes;
   unsigned NumField;

   /***** Fields *****/
   for (NumField = 0;
	NumField < Lnk_NUM_FIELDS;
	NumField++)
     {
      HTM_INPUT_TEXT (Lnk_FormNames[NumField],Lnk_MAX_BYTES_FIELD,"",
		      HTM_NO_ATTR,
		      "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		      *Lnk_Placeholders[NumField],The_GetSuffix ());
      HTM_BR ();
     }

   /***** URL *****/
   HTM_INPUT_URL ("WWW","",
		  HTM_REQUIRED,
		  " placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		  Txt_URL,The_GetSuffix ());
   HTM_BR ();

   /***** Button to save changes *****/
   Btn_PutCreateButtonInline (Txt_Save_changes);
  }

/*****************************************************************************/
/******************* Write title, description and URL ************************/
/*****************************************************************************/

static void Lnk_WriteField (const char *Field,const char *Class)
  {
   if (Class)
      HTM_SPAN_Begin ("class=\"%s\"",Class);
   HTM_Txt (Field);
   if (Class)
      HTM_SPAN_End ();
  }

/*****************************************************************************/
/*************************** Create course link ******************************/
/*****************************************************************************/

void Lnk_CreateCrsLink (struct Tre_Node *Node)
  {
   unsigned NumField;

   /***** Get fields for the new course link *****/
   for (NumField = 0;
	NumField < Lnk_NUM_FIELDS;
	NumField++)
      Par_GetParText (Lnk_FormNames[NumField],Node->Lnk.Fields[NumField],Lnk_MAX_BYTES_FIELD);
   Par_GetParText ("WWW",Node->Lnk.WWW,WWW_MAX_BYTES_WWW);

   /***** Create course link *****/
   Node->SpcItem.Cod = Lnk_DB_CreateCrsLink (Node);
  }

/*****************************************************************************/
/***************************** Change course link ****************************/
/*****************************************************************************/

void Lnk_ChangeCrsLink (struct Tre_Node *Node)
  {
   unsigned NumField;

   /***** Get fields for the course link *****/
   for (NumField = 0;
	NumField < Lnk_NUM_FIELDS;
	NumField++)
      Par_GetParText (Lnk_FormNames[NumField],Node->Lnk.Fields[NumField],Lnk_MAX_BYTES_FIELD);
   Par_GetParText ("WWW",Node->Lnk.WWW,WWW_MAX_BYTES_WWW);

   /***** Update answer *****/
   Lnk_DB_UpdateCrsLink (Node);
  }
