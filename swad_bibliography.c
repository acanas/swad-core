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

static void Bib_WriteField (const char *Field,const char *Class);

/*****************************************************************************/
/************* Reset specific fields of bibliographic reference **************/
/*****************************************************************************/

void Bib_ResetSpcFields (struct Tre_Node *Node)
  {
   Node->Bib.Authors[0]   = '\0';
   Node->Bib.Title[0]     = '\0';
   Node->Bib.Source[0]    = '\0';
   Node->Bib.Publisher[0] = '\0';
   Node->Bib.Date[0]      = '\0';
   Node->Bib.Id[0]        = '\0';
   Node->Bib.URL[0]       = '\0';
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
   NodCod	row[ 0]
   BibCod	row[ 1]
   BibInd	row[ 2]
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
   Str_Copy (Node->Bib.Authors  ,row[ 4],sizeof (Node->Bib.Authors  ) - 1);
   Str_Copy (Node->Bib.Title    ,row[ 5],sizeof (Node->Bib.Title    ) - 1);
   Str_Copy (Node->Bib.Source   ,row[ 6],sizeof (Node->Bib.Source   ) - 1);
   Str_Copy (Node->Bib.Publisher,row[ 7],sizeof (Node->Bib.Publisher) - 1);
   Str_Copy (Node->Bib.Date     ,row[ 8],sizeof (Node->Bib.Date     ) - 1);
   Str_Copy (Node->Bib.Id       ,row[ 9],sizeof (Node->Bib.Id       ) - 1);
   Str_Copy (Node->Bib.URL      ,row[10],sizeof (Node->Bib.URL      ) - 1);
  }

/*****************************************************************************/
/******************* Show one bibliographic reference ************************/
/*****************************************************************************/

void Bib_WriteCellViewBibRef (struct Tre_Node *Node)
  {
   char *Fields[] =
     {
      Node->Bib.Authors,
      Node->Bib.Title,
      Node->Bib.Source,
      Node->Bib.Publisher,
      Node->Bib.Date,
      Node->Bib.Id,
     };
#define NUM_FIELDS (sizeof (Fields) / sizeof (Fields[0]))
   static const char *Class[] =
     {
      "",
      "TRE_TIT",
      "",
      "",
      "",
      "",
     };
   unsigned NumField;
   char EndingCh = '\0';
   char *Field;
   size_t Length;

   /***** Write fields *****/
   for (NumField = 0;
	NumField < NUM_FIELDS;
	NumField++)
     {
      Field = Fields[NumField];
      Length = strlen (Field);
      if (Length)
	{
	 if (EndingCh != '\0')
	   {
	    if (EndingCh != '.')
	       HTM_Txt (".");
	    HTM_Txt (" ");
	   }
	 Bib_WriteField (Field,Class[NumField]);
	 EndingCh = Field[Length - 1];
	}
     }
   if (EndingCh != '\0')
      if (EndingCh != '.')
	 HTM_Txt (".");

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
   extern const char *Txt_BIBLIOGRAPHY_Authors;
   extern const char *Txt_BIBLIOGRAPHY_Title;
   extern const char *Txt_BIBLIOGRAPHY_Source;
   extern const char *Txt_BIBLIOGRAPHY_Publisher;
   extern const char *Txt_BIBLIOGRAPHY_Date;
   extern const char *Txt_BIBLIOGRAPHY_Id;
   extern const char *Txt_BIBLIOGRAPHY_URL;
   extern const char *Txt_Save_changes;

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

	    /* Authors */
	    HTM_INPUT_TEXT ("Authors",Bib_MAX_CHARS_TITLE,Node->Bib.Authors,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_BIBLIOGRAPHY_Authors,The_GetSuffix ());

	    /* Title */
	    HTM_BR ();
	    HTM_INPUT_TEXT ("Title",Bib_MAX_CHARS_TITLE,Node->Bib.Title,
			    HTM_REQUIRED,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_BIBLIOGRAPHY_Title,The_GetSuffix ());

	    /* Source */
	    HTM_BR ();
	    HTM_INPUT_TEXT ("Source",Bib_MAX_CHARS_TITLE,Node->Bib.Source,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_BIBLIOGRAPHY_Source,The_GetSuffix ());

	    /* Publisher */
	    HTM_BR ();
	    HTM_INPUT_TEXT ("Publisher",Bib_MAX_CHARS_TITLE,Node->Bib.Publisher,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_BIBLIOGRAPHY_Publisher,The_GetSuffix ());

	    /* Date */
	    HTM_BR ();
	    HTM_INPUT_TEXT ("Date",Bib_MAX_CHARS_TITLE,Node->Bib.Date,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_BIBLIOGRAPHY_Date,The_GetSuffix ());

	    /* Id */
	    HTM_BR ();
	    HTM_INPUT_TEXT ("Id",Bib_MAX_CHARS_TITLE,Node->Bib.Id,
			    HTM_NO_ATTR,
			    "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
			    Txt_BIBLIOGRAPHY_Id,The_GetSuffix ());

	    /* URL */
	    HTM_BR ();
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
   extern const char *Txt_BIBLIOGRAPHY_Authors;
   extern const char *Txt_BIBLIOGRAPHY_Title;
   extern const char *Txt_BIBLIOGRAPHY_Source;
   extern const char *Txt_BIBLIOGRAPHY_Publisher;
   extern const char *Txt_BIBLIOGRAPHY_Date;
   extern const char *Txt_BIBLIOGRAPHY_Id;
   extern const char *Txt_BIBLIOGRAPHY_URL;
   extern const char *Txt_Save_changes;

   /***** Authors *****/
   HTM_INPUT_TEXT ("Authors",Bib_MAX_CHARS_TITLE,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_BIBLIOGRAPHY_Authors,The_GetSuffix ());

   /***** Title *****/
   HTM_BR ();
   HTM_INPUT_TEXT ("Title",Bib_MAX_CHARS_TITLE,"",
		   HTM_REQUIRED,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_BIBLIOGRAPHY_Title,The_GetSuffix ());

   /***** Source *****/
   HTM_BR ();
   HTM_INPUT_TEXT ("Source",Bib_MAX_CHARS_TITLE,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_BIBLIOGRAPHY_Source,The_GetSuffix ());

   /***** Publisher *****/
   HTM_BR ();
   HTM_INPUT_TEXT ("Publisher",Bib_MAX_CHARS_TITLE,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_BIBLIOGRAPHY_Publisher,The_GetSuffix ());

   /***** Date *****/
   HTM_BR ();
   HTM_INPUT_TEXT ("Date",Bib_MAX_CHARS_TITLE,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_BIBLIOGRAPHY_Date,The_GetSuffix ());

   /***** Id *****/
   HTM_BR ();
   HTM_INPUT_TEXT ("Id",Bib_MAX_CHARS_TITLE,"",
		   HTM_NO_ATTR,
		   "placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		   Txt_BIBLIOGRAPHY_Id,The_GetSuffix ());

   /***** URL *****/
   HTM_BR ();
   HTM_INPUT_URL ("URL","",
		  HTM_NO_ATTR,
		  " placeholder=\"%s\" class=\"PRG_RSC_INPUT INPUT_%s\"",
		  Txt_BIBLIOGRAPHY_URL,The_GetSuffix ());

   /***** Button to save changes *****/
   HTM_BR ();
   Btn_PutConfirmButtonInline (Txt_Save_changes);
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
/********************* Create bibliographic reference ************************/
/*****************************************************************************/

void Bib_CreateBibRef (struct Tre_Node *Node)
  {
   /***** Get fields for the new bibliographic reference *****/
   Par_GetParText ("Authors"    ,Node->Bib.Authors    ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Title"      ,Node->Bib.Title      ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Source"     ,Node->Bib.Source     ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Publisher"  ,Node->Bib.Publisher  ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Date"       ,Node->Bib.Date       ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Id"         ,Node->Bib.Id         ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("URL"        ,Node->Bib.URL        ,WWW_MAX_BYTES_WWW);

   /***** Create bibliographic reference *****/
   Node->SpcItem.Cod = Bib_DB_CreateBibRef (Node);
  }

/*****************************************************************************/
/*********************** Change bibliographic reference ***********************/
/*****************************************************************************/

void Bib_ChangeBibRef (struct Tre_Node *Node)
  {
   /***** Get fields for the bibliographic reference *****/
   Par_GetParText ("Authors"    ,Node->Bib.Authors    ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Title"      ,Node->Bib.Title      ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Source"     ,Node->Bib.Source     ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Publisher"  ,Node->Bib.Publisher  ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Date"       ,Node->Bib.Date       ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("Id"         ,Node->Bib.Id         ,Bib_MAX_BYTES_TITLE);
   Par_GetParText ("URL"        ,Node->Bib.URL        ,WWW_MAX_BYTES_WWW);

   /***** Update answer *****/
   Bib_DB_UpdateBibRef (Node);
  }
