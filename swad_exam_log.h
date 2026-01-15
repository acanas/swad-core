// swad_exam_log.h: exam access log

#ifndef _SWAD_EXA_LOG
#define _SWAD_EXA_LOG
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_exam_print.h"

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

#define ExaLog_NUM_ACTIONS 5
// Don't change the numbers! If change ==> update them in table exa_log
typedef enum
  {
   ExaLog_UNKNOWN_ACTION  = 0,
   ExaLog_START_EXAM      = 1,
   ExaLog_RESUME_EXAM     = 2,
   ExaLog_ANSWER_QUESTION = 3,
   ExaLog_FINISH_EXAM     = 4,
  } ExaLog_Action_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaLog_SetPrnCod (long PrnCod);
long ExaLog_GetPrnCod (void);
void ExaLog_SetAction (ExaLog_Action_t Action);
ExaLog_Action_t ExaLog_GetAction (void);
void ExaLog_SetQstInd (unsigned QstInd);
int ExaLog_GetQstInd (void);
void ExaLog_SetIfCanAnswer (Usr_Can_t ICanAnswer);
Usr_Can_t ExaLog_GetIfCanAnswer (void);

void ExaLog_LogAccess (long LogCod);

void ExaLog_ShowExamLog (const struct ExaPrn_Print *Print);

#endif
