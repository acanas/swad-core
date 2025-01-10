// swad_box.c: layout of boxes

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
#include <stdbool.h>		// For boolean type
#include <stdlib.h>		// For malloc and free
#include <stdio.h>		// For asprintf

#include "swad_action.h"
#include "swad_box.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_help.h"
#include "swad_HTML.h"
#include "swad_icon.h"
#include "swad_theme.h"

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct
  {
   int Nested;			// Index of top open box
   char *Ids[Box_MAX_NESTED];	// 0 <= box index < Box_MAX_NESTED
  } Box_Boxes =
  {
   .Nested = -1,	// -1 means no box open
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Box_BoxInternalBegin (const char *Title,
				  void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
				  const char *HelpLink,Box_Closable_t Closable,
				  const char *ClassFrame);

/*****************************************************************************/
/******************* Begin and end a table with rounded box ******************/
/*****************************************************************************/
// CellPadding must be 0, 1, 2, 4 or 8

void Box_BoxTableBegin (const char *Title,
                        void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                        const char *HelpLink,Box_Closable_t Closable,
                        unsigned CellPadding)		// CellPadding must be 0, 1, 2, 5 or 10
  {
   Box_BoxBegin (Title,FunctionToDrawContextualIcons,Args,HelpLink,Closable);
      HTM_TABLE_BeginPadding (CellPadding);
  }

void Box_BoxTableShadowBegin (const char *Title,
                              void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                              const char *HelpLink,
                              unsigned CellPadding)	// CellPadding must be 0, 1, 2, 5 or 10
  {
   Box_BoxShadowBegin (Title,
                       FunctionToDrawContextualIcons,Args,
                       HelpLink);
      HTM_TABLE_BeginPadding (CellPadding);
  }

void Box_BoxBegin (const char *Title,
                   void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                   const char *HelpLink,Box_Closable_t Closable)
  {
   char *ClassFrame;

   /***** Begin box *****/
   if (asprintf (&ClassFrame,"FRAME FRAME_%s",The_GetSuffix ()) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxInternalBegin (Title,
			 FunctionToDrawContextualIcons,Args,
			 HelpLink,Closable,
			 ClassFrame);
   free (ClassFrame);
  }

void Box_BoxShadowBegin (const char *Title,
                         void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
                         const char *HelpLink)
  {
   char *ClassFrame;

   /***** Begin box *****/
   if (asprintf (&ClassFrame,"FRAME_SHADOW HIGHLIGHT_%s",The_GetSuffix ()) < 0)
      Err_NotEnoughMemoryExit ();
   Box_BoxInternalBegin (Title,
                         FunctionToDrawContextualIcons,Args,
			 HelpLink,Box_NOT_CLOSABLE,
			 ClassFrame);
   free (ClassFrame);
  }

// Return pointer to box id string
static void Box_BoxInternalBegin (const char *Title,
				  void (*FunctionToDrawContextualIcons) (void *Args),void *Args,
				  const char *HelpLink,Box_Closable_t Closable,
				  const char *ClassFrame)
  {
   extern const char *Txt_Help;
   extern const char *Txt_Close;

   /***** Check level of nesting *****/
   if (Box_Boxes.Nested >= Box_MAX_NESTED - 1)	// Can not nest a new box
      Err_ShowErrorAndExit ("Box nesting limit reached.");

   /***** Increase level of nesting *****/
   Box_Boxes.Nested++;

   /***** Create unique identifier for this box *****/
   if (Closable == Box_CLOSABLE)
     {
      if ((Box_Boxes.Ids[Box_Boxes.Nested] = malloc (Frm_MAX_BYTES_ID + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
     }
   else
      Box_Boxes.Ids[Box_Boxes.Nested] = NULL;

   /***** Begin box *****/
   if (Closable == Box_CLOSABLE)
     {
      /* Create unique id for alert */
      Frm_SetUniqueId (Box_Boxes.Ids[Box_Boxes.Nested]);

      HTM_DIV_Begin ("id=\"%s\" class=\"%s\"",
		     Box_Boxes.Ids[Box_Boxes.Nested],ClassFrame);
     }
   else
      HTM_DIV_Begin ("class=\"%s\"",ClassFrame);

   /***** Row for left and right icons *****/
   if (FunctionToDrawContextualIcons ||
       HelpLink ||
       Closable == Box_CLOSABLE)
     {
      /* Begin row for left and right icons */
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
				  "question.svg",Ico_BLACK,Txt_Help);
	       HTM_A_End ();
	      }

	    if (Closable == Box_CLOSABLE)	// Icon to close the box
	      {
	       HTM_A_Begin ("href=\"\" onclick=\"toggleDisplay('%s');return false;\"",
			    Box_Boxes.Ids[Box_Boxes.Nested]);
		  Ico_PutDivIcon ("CONTEXT_OPT HLP_HIGHLIGHT",
				  "times.svg",Ico_BLACK,Txt_Close);
	       HTM_A_End ();
	      }

	 HTM_DIV_End ();

      /* End row for left and right icons */
      HTM_DIV_End ();
     }

   /***** Frame title *****/
   if (Title)
     {
      HTM_DIV_Begin ("class=\"FRAME_TITLE %s FRAME_TITLE_%s\"",
	             Box_Boxes.Nested ? "FRAME_TITLE_SMALL" :
		                        "FRAME_TITLE_BIG",
		     The_GetSuffix ());
	 HTM_Txt (Title);
      HTM_DIV_End ();
     }
  }

void Box_BoxTableEnd (void)
  {
      HTM_TABLE_End ();
   Box_BoxEnd ();
  }

void Box_BoxTableWithButtonSaveCreateEnd (OldNew_OldNew_t OldNew)
  {
   extern const char *Txt_Create;
   extern const char *Txt_Save_changes;
   extern const char *Txt_View_results;
   extern const char *Txt_Go;
   extern const char *Txt_Remove;
   static const char **TxtButton[Btn_NUM_BUTTON_TYPES] =
     {
      [Btn_CREATE      ] = &Txt_Create,
      [Btn_CONFIRM     ] = &Txt_Save_changes,
      [Btn_VIEW_RESULTS] = &Txt_View_results,
      [Btn_GO          ] = &Txt_Go,
      [Btn_REMOVE      ] = &Txt_Remove,
     };
   static Btn_Button_t Buttons[OldNew_NUM_OLD_NEW] =
     {
      [OldNew_OLD] = Btn_CONFIRM,
      [OldNew_NEW] = Btn_CREATE ,
     };

   Box_BoxTableWithButtonEnd (Buttons[OldNew],*TxtButton[Buttons[OldNew]]);
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
      if (Box_Boxes.Nested < 0)
	 Err_ShowErrorAndExit ("Trying to end a box not open.");

      /***** Free memory allocated for box id string *****/
      if (Box_Boxes.Ids[Box_Boxes.Nested])
	 free (Box_Boxes.Ids[Box_Boxes.Nested]);

   /***** End box *****/
   HTM_DIV_End ();

   /***** Decrease level of nesting *****/
   Box_Boxes.Nested--;
  }
