// swad_button.h: buttons to submit forms

#ifndef _SWAD_BTN
#define _SWAD_BTN
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************* Public types ******************************/
/*****************************************************************************/

#define Btn_NUM_BUTTON_TYPES 4
typedef enum
  {
   Btn_NO_BUTTON,
   Btn_CREATE_BUTTON,
   Btn_CONFIRM_BUTTON,
   Btn_REMOVE_BUTTON,
  } Btn_Button_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Btn_PutButton (Btn_Button_t Button,const char *TxtButton);
void Btn_PutButtonInline (Btn_Button_t Button,const char *TxtButton);
void Btn_PutCreateButton (const char *TxtButton);
void Btn_PutCreateButtonInline (const char *TxtButton);
void Btn_PutConfirmButton (const char *TxtButton);
void Btn_PutConfirmButtonInline (const char *TxtButton);
void Btn_PutRemoveButton (const char *TxtButton);
void Btn_PutRemoveButtonInline (const char *TxtButton);

#endif
