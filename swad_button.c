// swad_button.c: buttons to submit forms

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include <stdio.h>		// For fprintf

#include "swad_button.h"
#include "swad_global.h"
#include "swad_HTML.h"

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

/*****************************************************************************/
/********************** Put a button to submit a form ************************/
/*****************************************************************************/

void Btn_PutButton (Btn_Button_t Button,const char *TxtButton)
  {
   if (TxtButton)
      if (TxtButton[0])
	 switch (Button)
           {
	    case Btn_NO_BUTTON:
	       break;
	    case Btn_CREATE_BUTTON:
	       Btn_PutCreateButton (TxtButton);
	       break;
	    case Btn_CONFIRM_BUTTON:
	       Btn_PutConfirmButton (TxtButton);
	       break;
	    case Btn_REMOVE_BUTTON:
 	       Btn_PutRemoveButton (TxtButton);
	       break;
          }
  }

void Btn_PutButtonInline (Btn_Button_t Button,const char *TxtButton)
  {
   if (TxtButton)
      if (TxtButton[0])
	 switch (Button)
           {
	    case Btn_NO_BUTTON:
	       break;
	    case Btn_CREATE_BUTTON:
	       Btn_PutCreateButtonInline (TxtButton);
	       break;
	    case Btn_CONFIRM_BUTTON:
	       Btn_PutConfirmButtonInline (TxtButton);
	       break;
	    case Btn_REMOVE_BUTTON:
 	       Btn_PutRemoveButtonInline (TxtButton);
	       break;
          }
  }

void Btn_PutCreateButton (const char *TxtButton)
  {
   HTM_DIV_Begin ("class=\"CM\"");
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT BT_CREATE\">"
                      "%s"
                      "</button>",
            TxtButton);
   HTM_DIV_End ();
  }

void Btn_PutCreateButtonInline (const char *TxtButton)
  {
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_CREATE\">"
                      "%s"
                      "</button>",
            TxtButton);
  }

void Btn_PutConfirmButton (const char *TxtButton)
  {
   HTM_DIV_Begin ("class=\"CM\"");
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT BT_CONFIRM\">"
                      "%s"
                      "</button>",
            TxtButton);
   HTM_DIV_End ();
  }

void Btn_PutConfirmButtonInline (const char *TxtButton)
  {
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_CONFIRM\">"
                      "%s"
                      "</button>",
            TxtButton);
  }

void Btn_PutRemoveButton (const char *TxtButton)
  {
   HTM_DIV_Begin ("class=\"CM\"");
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT BT_REMOVE\">"
                      "%s"
                      "</button>",
            TxtButton);
   HTM_DIV_End ();
  }

void Btn_PutRemoveButtonInline (const char *TxtButton)
  {
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE BT_REMOVE\">"
                      "%s"
                      "</button>",
            TxtButton);
  }
