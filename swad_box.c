// swad_box.c: layout of boxes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For fprintf

#include "swad_action.h"
#include "swad_box.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_table.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Box_StartBoxInternal (const char *Width,const char *Title,
                                  void (*FunctionToDrawContextualIcons) (void),
                                  const char *HelpLink,Box_Closable_t Closable,
                                  const char *ClassFrame);

/*****************************************************************************/
/******************* Start and end a table with rounded box ******************/
/*****************************************************************************/
// CellPadding must be 0, 1, 2, 4 or 8

void Box_StartBoxTable (const char *Width,const char *Title,
                        void (*FunctionToDrawContextualIcons) (void),
                        const char *HelpLink,Box_Closable_t Closable,
                        unsigned CellPadding)		// CellPadding must be 0, 1, 2, 5 or 10
  {
   Box_StartBox (Width,Title,FunctionToDrawContextualIcons,
                 HelpLink,Closable);
   Tbl_StartTableWide (CellPadding);
  }

void Box_StartBoxTableShadow (const char *Width,const char *Title,
                              void (*FunctionToDrawContextualIcons) (void),
                              const char *HelpLink,
                              unsigned CellPadding)	// CellPadding must be 0, 1, 2, 5 or 10
  {
   Box_StartBoxShadow (Width,Title,
                       FunctionToDrawContextualIcons,
                       HelpLink);
   Tbl_StartTableWide (CellPadding);
  }

void Box_StartBox (const char *Width,const char *Title,
                   void (*FunctionToDrawContextualIcons) (void),
                   const char *HelpLink,Box_Closable_t Closable)
  {
   Box_StartBoxInternal (Width,Title,
			 FunctionToDrawContextualIcons,
			 HelpLink,Closable,
			 "FRAME");
  }

void Box_StartBoxShadow (const char *Width,const char *Title,
                         void (*FunctionToDrawContextualIcons) (void),
                         const char *HelpLink)
  {
   Box_StartBoxInternal (Width,Title,
                         FunctionToDrawContextualIcons,
			 HelpLink,Box_NOT_CLOSABLE,
			 "FRAME_SHADOW");
  }

static void Box_StartBoxInternal (const char *Width,const char *Title,
                                  void (*FunctionToDrawContextualIcons) (void),
                                  const char *HelpLink,Box_Closable_t Closable,
                                  const char *ClassFrame)
  {
   extern const char *Txt_Help;
   extern const char *Txt_Close;
   char IdFrame[Act_MAX_BYTES_ID];

   /***** Start box container *****/
   fprintf (Gbl.F.Out,"<div class=\"FRAME_CONTAINER\"");
   if (Closable == Box_CLOSABLE)
     {
      /* Create unique id for alert */
      Act_SetUniqueId (IdFrame);
      fprintf (Gbl.F.Out," id=\"%s\"",IdFrame);
     }
   fprintf (Gbl.F.Out,">");

   /***** Start box *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\"",ClassFrame);
   if (Width)
       fprintf (Gbl.F.Out," style=\"width:%s;\"",Width);
   fprintf (Gbl.F.Out,">");

   /***** Row for left and right icons *****/
   fprintf (Gbl.F.Out,"<div class=\"FRAME_ICO\">");

   /* Contextual icons at left */
   if (FunctionToDrawContextualIcons)
     {
      fprintf (Gbl.F.Out,"<div class=\"FRAME_ICO_LEFT\">");
      FunctionToDrawContextualIcons ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /* Icons at right: help and close */
   fprintf (Gbl.F.Out,"<div class=\"FRAME_ICO_RIGHT\">");

   if (HelpLink)	// Link to help
      fprintf (Gbl.F.Out,"<a href=\"%s%s\" target=\"_blank\">"
                         "<div class=\"CONTEXT_OPT HLP_HIGHLIGHT\">"
	                 "<img src=\"%s/help64x64.png\""
	                 " alt=\"%s\" title=\"%s\""
	                 " class=\"ICO20x20\" />"
                         "</div>"
                         "</a>",
	       Hlp_WIKI,HelpLink,
               Gbl.Prefs.IconsURL,
               Txt_Help,Txt_Help);

   if (Closable == Box_CLOSABLE)	// Icon to close the box
      fprintf (Gbl.F.Out,"<a href=\"\""
			 " onclick=\"toggleDisplay('%s');return false;\" />"
                         "<div class=\"CONTEXT_OPT HLP_HIGHLIGHT\">"
			 "<img src=\"%s/close64x64.png\""
			 " alt=\"%s\" title=\"%s\""
			 " class=\"ICO20x20\" />"
                         "</div>"
			 "</a>",
	       IdFrame,
	       Gbl.Prefs.IconsURL,
	       Txt_Close,Txt_Close);

   fprintf (Gbl.F.Out,"</div>");

   /***** End row for left and right icons *****/
   fprintf (Gbl.F.Out,"</div>");

   /***** Frame title *****/
   if (Title)
      fprintf (Gbl.F.Out,"<div class=\"FRAME_TITLE %s\">"
	                 "%s"
	                 "</div>",
	       Gbl.Layout.NestedBox ? "FRAME_TITLE_SMALL" :
		                        "FRAME_TITLE_BIG",
	       Title);

   Gbl.Layout.NestedBox++;
  }

void Box_EndBoxTable (void)
  {
   Tbl_EndTable ();
   Box_EndBox ();
  }

void Box_EndBoxTableWithButton (Btn_Button_t Button,const char *TxtButton)
  {
   Tbl_EndTable ();
   Box_EndBoxWithButton (Button,TxtButton);
  }

void Box_EndBoxWithButton (Btn_Button_t Button,const char *TxtButton)
  {
   Btn_PutButton (Button,TxtButton);
   Box_EndBox ();
  }

void Box_EndBox (void)
  {
   Gbl.Layout.NestedBox--;

   /***** End box and box container *****/
   fprintf (Gbl.F.Out,"</div>"
		      "</div>");
  }
