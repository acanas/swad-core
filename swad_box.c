// swad_box.c: layout of boxes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include <stdlib.h>		// For malloc and free

#include "swad_action.h"
#include "swad_box.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_HTML.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Box_BoxInternalBegin (const char *Width,const char *Title,
				  void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
				  const char *HelpLink,Box_Closable_t Closable,
				  const char *ClassFrame);

/*****************************************************************************/
/******************* Begin and end a table with rounded box ******************/
/*****************************************************************************/
// CellPadding must be 0, 1, 2, 4 or 8

void Box_BoxTableBegin (const char *Width,const char *Title,
                        void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                        const char *HelpLink,Box_Closable_t Closable,
                        unsigned CellPadding)		// CellPadding must be 0, 1, 2, 5 or 10
  {
   Box_BoxBegin (Width,Title,
                 FunctionToDrawContextualIcons,Args,
                 HelpLink,Closable);
   HTM_TABLE_BeginWidePadding (CellPadding);
  }

void Box_BoxTableShadowBegin (const char *Width,const char *Title,
                              void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                              const char *HelpLink,
                              unsigned CellPadding)	// CellPadding must be 0, 1, 2, 5 or 10
  {
   Box_BoxShadowBegin (Width,Title,
                       FunctionToDrawContextualIcons,Args,
                       HelpLink);
   HTM_TABLE_BeginWidePadding (CellPadding);
  }

void Box_BoxBegin (const char *Width,const char *Title,
                   void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                   const char *HelpLink,Box_Closable_t Closable)
  {
   Box_BoxInternalBegin (Width,Title,
			 FunctionToDrawContextualIcons,Args,
			 HelpLink,Closable,
			 "FRAME");
  }

void Box_BoxShadowBegin (const char *Width,const char *Title,
                         void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                         const char *HelpLink)
  {
   Box_BoxInternalBegin (Width,Title,
                         FunctionToDrawContextualIcons,Args,
			 HelpLink,Box_NOT_CLOSABLE,
			 "FRAME_SHADOW");
  }

// Return pointer to box id string
static void Box_BoxInternalBegin (const char *Width,const char *Title,
				  void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
				  const char *HelpLink,Box_Closable_t Closable,
				  const char *ClassFrame)
  {
   extern const char *Txt_Help;
   extern const char *Txt_Close;

   /***** Check level of nesting *****/
   if (Gbl.Box.Nested >= Box_MAX_NESTED - 1)	// Can not nest a new box
      Err_ShowErrorAndExit ("Box nesting limit reached.");

   /***** Increase level of nesting *****/
   Gbl.Box.Nested++;

   /***** Create unique identifier for this box *****/
   if (Closable == Box_CLOSABLE)
     {
      if ((Gbl.Box.Ids[Gbl.Box.Nested] = malloc (Frm_MAX_BYTES_ID + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
     }
   else
      Gbl.Box.Ids[Gbl.Box.Nested] = NULL;

   /***** Begin box container *****/
   if (Closable == Box_CLOSABLE)
     {
      /* Create unique id for alert */
      Frm_SetUniqueId (Gbl.Box.Ids[Gbl.Box.Nested]);

      HTM_DIV_Begin ("class=\"FRAME_CONT\" id=\"%s\"",Gbl.Box.Ids[Gbl.Box.Nested]);
     }
   else
      HTM_DIV_Begin ("class=\"FRAME_CONT\"");

   /***** Begin box *****/
   if (Width)
      HTM_DIV_Begin ("class=\"%s\" style=\"width:%s;\"",ClassFrame,Width);
   else
      HTM_DIV_Begin ("class=\"%s\"",ClassFrame);

   /***** Row for left and right icons *****/
   HTM_DIV_Begin ("class=\"FRAME_ICO\"");

   /* Contextual icons at left */
   if (FunctionToDrawContextualIcons)
     {
      HTM_DIV_Begin ("class=\"FRAME_ICO_LEFT\"");
      FunctionToDrawContextualIcons (Args);
      HTM_DIV_End ();
     }

   /* Icons at right: help and close */
   HTM_DIV_Begin ("class=\"FRAME_ICO_RIGHT\"");

   if (HelpLink)	// Link to help
     {
      HTM_A_Begin ("href=\"%s%s\" target=\"_blank\"",Hlp_WIKI,HelpLink);
      Ico_PutDivIcon ("CONTEXT_OPT HLP_HIGHLIGHT",
		      "question.svg",Txt_Help);
      HTM_A_End ();
     }

   if (Closable == Box_CLOSABLE)	// Icon to close the box
     {
      HTM_A_Begin ("href=\"\" onclick=\"toggleDisplay('%s');return false;\"",
	           Gbl.Box.Ids[Gbl.Box.Nested]);
      Ico_PutDivIcon ("CONTEXT_OPT HLP_HIGHLIGHT",
		      "times.svg",Txt_Close);
      HTM_A_End ();
     }

   HTM_DIV_End ();

   /***** End row for left and right icons *****/
   HTM_DIV_End ();

   /***** Frame title *****/
   if (Title)
     {
      HTM_DIV_Begin ("class=\"FRAME_TITLE %s\"",
	             Gbl.Box.Nested ? "FRAME_TITLE_SMALL" :
		                      "FRAME_TITLE_BIG");
      HTM_Txt (Title);
      HTM_DIV_End ();
     }
  }

void Box_BoxTableEnd (void)
  {
   HTM_TABLE_End ();
   Box_BoxEnd ();
  }

void Box_BoxTableWithButtonEnd (Btn_Button_t Button,const char *TxtButton)
  {
   HTM_TABLE_End ();
   Box_BoxWithButtonEnd (Button,TxtButton);
  }

void Box_BoxWithButtonEnd (Btn_Button_t Button,const char *TxtButton)
  {
   Btn_PutButton (Button,TxtButton);
   Box_BoxEnd ();
  }

void Box_BoxEnd (void)
  {
   /***** Check level of nesting *****/
   if (Gbl.Box.Nested < 0)
      Err_ShowErrorAndExit ("Trying to end a box not open.");

   /***** Free memory allocated for box id string *****/
   if (Gbl.Box.Ids[Gbl.Box.Nested])
      free (Gbl.Box.Ids[Gbl.Box.Nested]);

   /***** End box and box container *****/
   HTM_DIV_End ();
   HTM_DIV_End ();

   /***** Decrease level of nesting *****/
   Gbl.Box.Nested--;
  }
