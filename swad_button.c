// swad_button.c: buttons to submit forms

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

#include "swad_button.h"
#include "swad_global.h"
#include "swad_HTML.h"

/*****************************************************************************/
/********************** Put a button to submit a form ************************/
/*****************************************************************************/

void Btn_PutButton (Btn_Button_t Button,const char *TxtButton)
  {
   static void (*Function[Btn_NUM_BUTTON_TYPES]) (const char *TxtButton) =
     {
      [Btn_CREATE_BUTTON ] = Btn_PutCreateButton,
      [Btn_CONFIRM_BUTTON] = Btn_PutConfirmButton,
      [Btn_REMOVE_BUTTON ] = Btn_PutRemoveButton,
     };

   if (Button != Btn_NO_BUTTON && TxtButton)
      Function[Button] (TxtButton);
  }

void Btn_PutButtonInline (Btn_Button_t Button,const char *TxtButton)
  {
   static void (*Function[Btn_NUM_BUTTON_TYPES]) (const char *TxtButton) =
     {
      [Btn_CREATE_BUTTON ] = Btn_PutCreateButtonInline,
      [Btn_CONFIRM_BUTTON] = Btn_PutConfirmButtonInline,
      [Btn_REMOVE_BUTTON ] = Btn_PutRemoveButtonInline,
     };

   if (Button != Btn_NO_BUTTON && TxtButton)
      Function[Button] (TxtButton);
  }

void Btn_PutCreateButton (const char *TxtButton)
  {
   HTM_DIV_Begin ("class=\"CM\"");
      HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT BT_CREATE\"");
	 HTM_Txt (TxtButton);
      HTM_BUTTON_End ();
   HTM_DIV_End ();
  }

void Btn_PutCreateButtonInline (const char *TxtButton)
  {
   HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT_INLINE BT_CREATE\"");
      HTM_Txt (TxtButton);
   HTM_BUTTON_End ();
  }

void Btn_PutConfirmButton (const char *TxtButton)
  {
   HTM_DIV_Begin ("class=\"CM\"");
      HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT BT_CONFIRM\"");
	 HTM_Txt (TxtButton);
      HTM_BUTTON_End ();
   HTM_DIV_End ();
  }

void Btn_PutConfirmButtonInline (const char *TxtButton)
  {
   HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT_INLINE BT_CONFIRM\"");
      HTM_Txt (TxtButton);
   HTM_BUTTON_End ();
  }

void Btn_PutRemoveButton (const char *TxtButton)
  {
   HTM_DIV_Begin ("class=\"CM\"");
      HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT BT_REMOVE\"");
	 HTM_Txt (TxtButton);
      HTM_BUTTON_End ();
   HTM_DIV_End ();
  }

void Btn_PutRemoveButtonInline (const char *TxtButton)
  {
   HTM_BUTTON_Submit_Begin (NULL,"class=\"BT_SUBMIT_INLINE BT_REMOVE\"");
      HTM_Txt (TxtButton);
   HTM_BUTTON_End ();
  }
