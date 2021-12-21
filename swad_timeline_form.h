// swad_timeline_form.h: social timeline forms

#ifndef _SWAD_TML_FRM
#define _SWAD_TML_FRM
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/******************************** Public types *******************************/
/*****************************************************************************/

#define Tml_Frm_NUM_ACTIONS 16
typedef enum
  {
   Tml_Frm_RECEIVE_POST,	// Receive post
   Tml_Frm_RECEIVE_COMM,	// Receive comment

   Tml_Frm_REQ_REM_NOTE,	// Request removal note
   Tml_Frm_REQ_REM_COMM,	// Request removal comment

   Tml_Frm_REM_NOTE,	// Remove note
   Tml_Frm_REM_COMM,	// Remove comment

   Tml_Frm_SHA_NOTE,	// Share a note
   Tml_Frm_UNS_NOTE,	// Unshare a note

   Tml_Frm_FAV_NOTE,     // Favourite a note
   Tml_Frm_FAV_COMM,	// Favourite a comment

   Tml_Frm_UNF_NOTE,	// Unfavourite a note
   Tml_Frm_UNF_COMM,	// Unfavourite a comment

   Tml_Frm_ALL_SHA_NOTE,	// Show all sharers of note

   Tml_Frm_ALL_FAV_NOTE,	// Show all favouriters of note
   Tml_Frm_ALL_FAV_COMM,	// Show all favouriters of comment

   Tml_Frm_SHO_HID_COMM,	// Show / hide comments
  } Tml_Frm_Action_t;

struct Tml_Form
  {
   Tml_Frm_Action_t Action;
   const char *ParamFormat;
   long ParamCod;
   const char *Icon;
   Ico_Color_t Color;
   const char *Title;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Tml_Frm_PutFormToFavUnfShaUns (Tml_Usr_FavSha_t FavSha,long Cod);

void Tml_Frm_BeginForm (const struct Tml_Timeline *Timeline,
                        Tml_Frm_Action_t Action);
void Tml_Frm_EndForm (void);

void Tml_Frm_PutFormToSeeAllFaversSharers (Tml_Frm_Action_t Action,
		                           const char *ParamFormat,long ParamCod,
                                           Tml_Usr_HowManyUsrs_t HowManyUsrs);
void Tml_Frm_FormFavSha (const struct Tml_Form *Form);

void Tml_Frm_FormToShowHiddenComms (long NotCod,
				    char IdComms[Frm_MAX_BYTES_ID + 1],
				    unsigned NumInitialComms);

void Tml_Frm_BeginAlertRemove (const char *QuestionTxt);
void Tml_Frm_EndAlertRemove (struct Tml_Timeline *Timeline,
                             Tml_Frm_Action_t Action,
                             void (*FuncParams) (void *Args));

#endif
