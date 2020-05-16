// swad_exam_event.h: exam events (each ocurrence of an exam)

#ifndef _SWAD_EXA_EVE
#define _SWAD_EXA_EVE
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#define ExaEvt_NEW_EVENT_SECTION_ID	"new_event"

#define ExaEvt_AFTER_LAST_QUESTION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database to indicate that a event is finished

struct ExaEvt_UsrAnswer
  {
   int NumOpt;	// < 0 ==> no answer selected
   int AnsInd;	// < 0 ==> no answer selected
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaEvt_ResetEvent (struct ExaEvt_Event *Event);

void ExaEvt_ListEvents (struct Exa_Exams *Exams,
                        struct Exa_Exam *Exam,
		        struct ExaEvt_Event *Event,
                        bool PutFormEvent);
void ExaEvt_GetDataOfEventByCod (struct ExaEvt_Event *Event);
bool ExaEvt_CheckIfEventIsVisibleAndOpen (long EvtCod);

void ExaEvt_ToggleVisResultsEvtUsr (void);

void ExaEvt_RequestRemoveEvent (void);
void ExaEvt_RemoveEvent (void);

void ExaEvt_RemoveEventsInExamFromAllTables (long ExaCod);
void ExaEvt_RemoveEventInCourseFromAllTables (long CrsCod);
void ExaEvt_RemoveUsrFromEventTablesInCrs (long UsrCod,long CrsCod);

void ExaEvt_HideEvent (void);
void ExaEvt_UnhideEvent (void);

void ExaEvt_PutParamsEdit (void *Exams);
void ExaEvt_GetAndCheckParameters (struct Exa_Exams *Exams,
                                   struct Exa_Exam *Exam,
                                   struct ExaEvt_Event *Event);
long ExaEvt_GetParamEvtCod (void);

void ExaEvt_PutButtonNewEvent (struct Exa_Exams *Exams,long ExaCod);
void ExaEvt_RequestCreatOrEditEvent (void);
void ExaEvt_ReceiveFormEvent (void);

void ExaEvt_RemoveGroup (long GrpCod);
void ExaEvt_RemoveGroupsOfType (long GrpTypCod);

unsigned ExaEvt_GetNumEventsInExam (long ExaCod);
unsigned ExaEvt_GetNumOpenEventsInExam (long ExaCod);

bool ExaEvt_CheckIfICanAnswerThisEvent (const struct ExaEvt_Event *Event);
bool ExaEvt_CheckIfICanListThisEventBasedOnGrps (long EvtCod);

void ExaEvt_GetQstAnsFromDB (long EvtCod,long UsrCod,unsigned QstInd,
		             struct ExaEvt_UsrAnswer *UsrAnswer);

#endif
