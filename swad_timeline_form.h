// swad_timeline_form.h: social timeline forms

#ifndef _SWAD_TML_FRM
#define _SWAD_TML_FRM
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
/******************************** Public types *******************************/
/*****************************************************************************/

#define TmlFrm_NUM_ACTIONS 16
typedef enum
  {
   TmlFrm_RECEIVE_POST,	// Receive post
   TmlFrm_RECEIVE_COMM,	// Receive comment

   TmlFrm_REQ_REM_NOTE,	// Request removal note
   TmlFrm_REQ_REM_COMM,	// Request removal comment

   TmlFrm_REM_NOTE,	// Remove note
   TmlFrm_REM_COMM,	// Remove comment

   TmlFrm_SHA_NOTE,	// Share a note
   TmlFrm_UNS_NOTE,	// Unshare a note

   TmlFrm_FAV_NOTE,     // Favourite a note
   TmlFrm_FAV_COMM,	// Favourite a comment

   TmlFrm_UNF_NOTE,	// Unfavourite a note
   TmlFrm_UNF_COMM,	// Unfavourite a comment

   TmlFrm_ALL_SHA_NOTE,	// Show all sharers of note

   TmlFrm_ALL_FAV_NOTE,	// Show all favouriters of note
   TmlFrm_ALL_FAV_COMM,	// Show all favouriters of comment

   TmlFrm_SHO_HID_COMM,	// Show / hide comments
  } TmlFrm_Action_t;

struct Tml_Form
  {
   TmlFrm_Action_t Action;
   const char *ParFormat;
   long ParCod;
   const char *Icon;
   Ico_Color_t Color;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TmlFrm_PutFormToFavUnfShaUns (TmlUsr_FavSha_t FavSha,long Cod);

void TmlFrm_BeginForm (const struct Tml_Timeline *Timeline,
                       TmlFrm_Action_t Action);
void TmlFrm_EndForm (void);

void TmlFrm_FormFavSha (const struct Tml_Form *Form);

void TmlFrm_FormToShowHiddenComms (long NotCod,
				   char IdComms[Frm_MAX_BYTES_ID + 1],
				   unsigned NumInitialComms);

void TmlFrm_BeginAlertRemove (const char *QuestionTxt);
void TmlFrm_EndAlertRemove (struct Tml_Timeline *Timeline,
                            TmlFrm_Action_t Action,
                            void (*FuncPars) (void *Args));

#endif
