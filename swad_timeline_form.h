// swad_timeline_form.h: social timeline forms

#ifndef _SWAD_TL_FRM
#define _SWAD_TL_FRM
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
/********************************** Headers **********************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

#define TL_Frm_NUM_ACTIONS 16
typedef enum
  {
   TL_Frm_RECEIVE_POST,	// Receive post
   TL_Frm_RECEIVE_COMM,	// Receive comment

   TL_Frm_REQ_REM_NOTE,	// Request removal note
   TL_Frm_REQ_REM_COMM,	// Request removal comment

   TL_Frm_REM_NOTE,	// Remove note
   TL_Frm_REM_COMM,	// Remove comment

   TL_Frm_SHA_NOTE,	// Share a note
   TL_Frm_UNS_NOTE,	// Unshare a note

   TL_Frm_FAV_NOTE,     // Favourite a note
   TL_Frm_FAV_COMM,	// Favourite a comment

   TL_Frm_UNF_NOTE,	// Unfavourite a note
   TL_Frm_UNF_COMM,	// Unfavourite a comment

   TL_Frm_ALL_SHA_NOTE,	// Show all sharers of note

   TL_Frm_ALL_FAV_NOTE,	// Show all favouriters of note
   TL_Frm_ALL_FAV_COMM,	// Show all favouriters of comment

   TL_Frm_SHO_HID_COMM,	// Show / hide comments
  } TL_Frm_Action_t;

struct TL_Form
  {
   TL_Frm_Action_t Action;
   const char *ParamFormat;
   long ParamCod;
   const char *Icon;
   const char *Title;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void TL_Frm_FormStart (const struct TL_Timeline *Timeline,TL_Frm_Action_t Action);

void TL_Frm_PutFormToSeeAllFaversSharers (TL_Frm_Action_t Action,
		                          const char *ParamFormat,long ParamCod,
                                          TL_Usr_HowManyUsrs_t HowManyUsrs);
void TL_Frm_FormFavSha (const struct TL_Form *Form);

void TL_Frm_FormToShowHiddenComments (long NotCod,
				      char IdComments[Frm_MAX_BYTES_ID + 1],
				      unsigned NumInitialComments);

void TL_Frm_BeginAlertRemove (const char *QuestionTxt);
void TL_Frm_EndAlertRemove (struct TL_Timeline *Timeline,TL_Frm_Action_t Action,
                            void (*FuncParams) (void *Args));

#endif
