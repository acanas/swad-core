// swad_button.h: buttons to submit forms

#ifndef _SWAD_BTN
#define _SWAD_BTN
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************* Public types ******************************/
/*****************************************************************************/

#define Btn_NUM_BUTTON_TYPES 27
typedef enum
  {
   Btn_NO_BUTTON,
   Btn_CHANGE,
   Btn_CHECK,
   Btn_CONFIRM,
   Btn_CONTINUE,
   Btn_CREATE,
   Btn_CREATE_ACCOUNT,
   Btn_DONE,
   Btn_EDIT,
   Btn_ENROL,
   Btn_ELIMINATE,
   Btn_FOLLOW,
   Btn_GO,
   Btn_LOCK_EDITING,
   Btn_PASTE,
   Btn_REJECT,
   Btn_REMOVE,
   Btn_RESET,
   Btn_SAVE_CHANGES,
   Btn_SEARCH,
   Btn_SEND,
   Btn_SHOW,
   Btn_SHOW_MORE_DETAILS,
   Btn_UNFOLLOW,
   Btn_UNLOCK_EDITING,
   Btn_UPLOAD_PHOTO,
   Btn_VIEW_RESULTS,
  } Btn_Button_t;

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Btn_PutButton (Btn_Button_t Button);
void Btn_PutButtonTxt (Btn_Button_t Button,const char *TxtButton);

void Btn_PutButtonInline (Btn_Button_t Button);
void Btn_PutButtonTxtInline (Btn_Button_t Button,const char *TxtButton);

#endif
