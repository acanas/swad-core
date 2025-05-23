// swad_exam_session.h: exam sessions (each ocurrence of an exam)

#ifndef _SWAD_EXA_SES
#define _SWAD_EXA_SES
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_exam_type.h"
#include "swad_test.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define ExaSes_NEW_SESSION_SECTION_ID	"new_session"

#define ExaSes_AFTER_LAST_QUESTION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database to indicate that a session is finished

struct ExaSes_UsrAnswer
  {
   int NumOpt;	// < 0 ==> no answer selected
   int AnsInd;	// < 0 ==> no answer selected
  };

#define ExaSes_EXAM_SHEET_OPTIONS_SECTION_ID	"exam_sheet_options"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaSes_ResetSession (struct ExaSes_Session *Session);

void ExaSes_ListSessions (struct Exa_Exams *Exams,Frm_PutForm_t PutFormSession);

void ExaSes_ShowOneSession (void);

void ExaSes_ListUsersForSelection (struct Exa_Exams *Exams,
				   const struct ExaSes_Session *Session);
void ExaSes_ShowFormColumns (const struct ExaSes_Session *Session);
unsigned ExaSes_GetParNumCols (void);

void ExaSes_GetSessionDataByCod (struct ExaSes_Session *Session);

void ExaSes_ToggleVisResultsSesUsr (void);

void ExaSes_ReqRemSession (void);
void ExaSes_RemoveSession (void);

void ExaSes_HideSession (void);
void ExaSes_UnhideSession (void);

void ExaSes_GetAndCheckPars (struct Exa_Exams *Exams,
                             struct ExaSes_Session *Session);

void ExaSes_ReqCreatOrEditSes (void);
void ExaSes_ReceiveSession (void);

Usr_Can_t ExaSes_CheckIfICanAnswerThisSession (const struct Exa_Exam *Exam,
                                               const struct ExaSes_Session *Session);
Usr_Can_t ExaSes_CheckIfICanListThisSessionBasedOnGrps (long SesCod);

#endif
