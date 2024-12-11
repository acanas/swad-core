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

#include <mysql/mysql.h>	// To access MySQL databases
#include <stdbool.h>		// For boolean type
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
/************************* Private global variables **************************/
/*****************************************************************************/

extern const char *Txt_BIBLIOGRAPHY_Authors;
extern const char *Txt_BIBLIOGRAPHY_Title;
extern const char *Txt_BIBLIOGRAPHY_Source;
extern const char *Txt_BIBLIOGRAPHY_Publisher;
extern const char *Txt_BIBLIOGRAPHY_Date;
extern const char *Txt_BIBLIOGRAPHY_Id;

static const char *Bib_FormNames[Bib_NUM_FIELDS] =
  {
   [Bib_AUTHORS		] = "Authors",
   [Bib_TITLE		] = "Title",
   [Bib_SOURCE		] = "Source",
   [Bib_PUBLISHER	] = "Publisher",
   [Bib_DATE		] = "Date",
   [Bib_ID		] = "Id",
  };

static const char **Bib_Placeholders[Bib_NUM_FIELDS] =
  {
   [Bib_AUTHORS		] = &Txt_BIBLIOGRAPHY_Authors,
   [Bib_TITLE		] = &Txt_BIBLIOGRAPHY_Title,
   [Bib_SOURCE		] = &Txt_BIBLIOGRAPHY_Source,
   [Bib_PUBLISHER	] = &Txt_BIBLIOGRAPHY_Publisher,
   [Bib_DATE		] = &Txt_BIBLIOGRAPHY_Date,
   [Bib_ID		] = &Txt_BIBLIOGRAPHY_Id,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Bib_WriteField (const char *Field,const char *Class);

/*****************************************************************************/
/************* Reset specific fields of bibliographic reference **************/
/*****************************************************************************/

void Bib_ResetSpcFields (struct Tre_Node *Node)
  {
   unsigned NumField;

   for (NumField = 0;
	NumField < Bib_NUM_FIELDS;
	NumField++)
      Node->Bib.Fields[NumField][0] = '\0';
   Node->Bib.URL[0] = '\0';
  }

/*****************************************************************************/
/******************** Get bibliographic reference data ***********************/
/*****************************************************************************/

void Bib_GetBibRefDataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node)
  {
   MYSQL_ROW row;
   unsigned NumField;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   NodCod	row[ 0]
   ItmCod	row[ 1]
   ItmInd	row[ 2]
   Hidden	row[ 3]
   Authors	row[ 4]
   Title	row[ 5]
   Source	row[ 6]
   Publisher	row[ 7]
   Date		row[ 8]
   Id		row[ 9]
   URL		row[10]
   */
   /***** Get code of the tree node (row[0]) *****/
   Node->Hierarchy.NodCod = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get code and index of the bibliographic reference (row[1], row[2]) *****/
   Node->SpcItem.Cod = Str_ConvertStrCodToLongCod (row[1]);
   Node->SpcItem.Ind = Str_ConvertStrToUnsigned (row[2]);

   /***** Get whether the tree node is hidden (row(3)) *****/
   Node->SpcItem.HiddenOrVisible = HidVid_GetHiddenOrVisible (row[3][0]);

   /***** Get authors, title, source, publisher, date, id and URL
          of the bibliographic reference (row[4]...row[10]) *****/
   for (NumField = 0;
	NumField < Bib_NUM_FIELDS;
	NumField++)
      Str_Copy (Node->Bib.Fields[NumField],row[4 + NumField],Bib_MAX_BYTES_FIELD);
   Str_Copy (Node->Bib.URL,row[4 + Bib_NUM_FIELDS],sizeof (Node->Bib.URL) - 1);
  }

/*****************************************************************************/
/******************* Show one bibliographic reference ************************/
/*****************************************************************************/

void Bib_WriteCellViewBibRef (struct Tre_Node *Node)
  {
   static const char *Class[Bib_NUM_FIELDS] =
     {
      [Bib_TITLE] = "TRE_TIT",
     };
   unsigned NumField;
   char EndingCh;
   size_t Length;

   /***** Write fields *****/
   for (NumField = 0, EndingCh = '\0';
	NumField < Bib_NUM_FIELDS;
	NumField++)
      if (Node->Bib.Fields[NumField][0])
	{
         /* Put dot before field? */
	 if (EndingCh != '\0')
	   {
	    if (EndingCh != '.')
	       HTM_Dot ();
	    HTM_SP ();
	   }

	 Bib_WriteField (Node->Bib.Fields[NumField],Class[NumField]);
	 Length = strlen (Node->Bib.Fields[NumField]);
	 EndingCh = Node->Bib.Fields[NumField][Length - 1];
	}

   /* Put dot after last field? */
   if (EndingCh != '\0' && EndingCh != '.')
      HTM_Dot ();

   /***** Write URL *****/
   if (Node->Bib.URL[0])
     {
      HTM_BR ();
      HTM_A_Begin ("href=\"%s\" target=\"_blank\"",Node->Bib.URL);
         HTM_Txt (Node->Bib.URL);
      HTM_A_End ();
     }
  }

/*****************************************************************************/
/******************** Edit one bibliographic reference ***********************/
/*****************************************************************************/

void Bib_WriteCellEditBibRef (struct Tre_Node *Node,
                              Vie_ViewType_t ViewType,
			      HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char *HidVis_TreeClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_BIBLIOGRAPHY_URL;
   extern const char *Txt_Save_changes;
   static HTM_Attributes_t Attributes[Bib_NUM_FIELDS] =
     {
      [Bib_AUTHORS	] = HTM_NO_ATTR,
      [Bib_TITLE	] = HTM_REQUIRED,
      [Bib_SOURCE	] = HTM_NO_ATTR,
      [Bib_PUBLISHER	] = HTM_NO_ATTR,
      [Bib_DATE		] = HTM_NO_ATTR,
      [Bib_ID		] = HTM_NO_ATTR,
     };
   unsigned NumField;

   switch (ViewType)
     {
      case Vie_VIEW:
	 /***** Show current bibliographic reference *****/
	 HTM_DIV_Begin ("class=\"PRG_TXT_%s%s\"",
		        The_GetSuffix (),HidVis_TreeClass[HiddenOrVisible]);
	    Bib_WriteCellViewBibRef (Node);
	 HTM_DIV_End ();
	 break;
      case Vie_EDIT:
	 /***** Show form to change bibliographic reference *****/
	 Frm_BeginFormAnchor (ActChgBibRef,TreSpc_LIST_ITEMS_SECTION_ID);
	    TreSpc_PutParItmCod (&Node->SpcItem.Cod);

	    for (NumField = 0;
		 NumField < Bib_NUM_FIELDS;
		 NumField++)
	      {
	       HTM_INPUT_TEXT (Bib_FormNames[NumField],Bib_MAX_CHARS_FIELD,Node->Bib.Fields[NumField],
			       Attributes[NumField],
			       "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			       *Bib_Placeholders[NumField],The_GetSuffix ());
	       HTM_BR ();
	      }

	    /* URL */
	    HTM_INPUT_URL ("URL",Node->Bib.URL,
			   HTM_NO_ATTR,
			   " placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			   Txt_BIBLIOGRAPHY_URL,The_GetSuffix ());

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
   extern const char *Txt_BIBLIOGRAPHY_URL;
   extern const char *Txt_Save_changes;
   unsigned NumField;

   /***** Fields *****/
   for (NumField = 0;
	NumField < Bib_NUM_FIELDS;
	NumField++)
     {
      HTM_INPUT_TEXT (Bib_FormNames[NumField],Bib_MAX_CHARS_FIELD,"",
		      HTM_NO_ATTR,
		      "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		      *Bib_Placeholders[NumField],The_GetSuffix ());
      HTM_BR ();
     }

   /***** URL *****/
   HTM_INPUT_URL ("URL","",
		  HTM_NO_ATTR,
		  " placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		  Txt_BIBLIOGRAPHY_URL,The_GetSuffix ());
   HTM_BR ();

   /***** Button to save changes *****/
   Btn_PutCreateButtonInline (Txt_Save_changes);
  }

/*****************************************************************************/
/************************* Write authors, title,... **************************/
/*****************************************************************************/

static void Bib_WriteField (const char *Field,const char *Class)
  {
   if (Class)
      HTM_SPAN_Begin ("class=\"%s\"",Class);
   HTM_Txt (Field);
   if (Class)
      HTM_SPAN_End ();
  }

/*****************************************************************************/
/*********************** Change bibliographic reference ***********************/
/*****************************************************************************/

void Bib_ChangeBibRef (struct Tre_Node *Node)
  {
   unsigned NumField;

   /***** Get fields for the new bibliographic reference *****/
   for (NumField = 0;
	NumField < Bib_NUM_FIELDS;
	NumField++)
      Par_GetParText (Bib_FormNames[NumField],Node->Bib.Fields[NumField],Bib_MAX_BYTES_FIELD);
   Par_GetParText ("URL",Node->Bib.URL,WWW_MAX_BYTES_WWW);

   /***** Is it an existing item? *****/
   if (Node->SpcItem.Cod >  0)
      /* Update item */
      Bib_DB_UpdateBibRef (Node);
   else
      /* Create item */
      Node->SpcItem.Cod = Bib_DB_CreateBibRef (Node);
  }
