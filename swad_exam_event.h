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

#include "swad_scope.h"
#include "swad_test.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define ExaEvt_NEW_EVENT_SECTION_ID	"new_event"

#define ExaEvt_AFTER_LAST_QUESTION	((unsigned)((1UL << 31) - 1))	// 2^31 - 1, don't change this number because it is used in database to indicate that a event is finished

#define ExaEvt_NUM_SHOWING 5
typedef enum
  {
   ExaEvt_START,	// Start: don't show anything
   ExaEvt_STEM,	// Showing only the question stem
   ExaEvt_ANSWERS,	// Showing the question stem and the answers
   ExaEvt_RESULTS,	// Showing the results
   ExaEvt_END,	// End: don't show anything
  } ExaEvt_Showing_t;
#define ExaEvt_SHOWING_DEFAULT ExaEvt_START

struct ExaEvt_Event
  {
   long EvtCod;
   long ExaCod;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   char Title[Exa_MAX_BYTES_TITLE + 1];
   struct
     {
      unsigned QstInd;	// 0 means that the exam has not started. First question has index 1.
      long QstCod;
      time_t QstStartTimeUTC;
      ExaEvt_Showing_t Showing;	// What is shown on teacher's screen
      long Countdown;		// > 0 ==> countdown in progress
				// = 0 ==> countdown over ==> go to next step
				// < 0 ==> no countdown at this time
      unsigned NumCols;		// Number of columns for answers on teacher's screen
      bool ShowQstResults;	// Show global results of current question while playing
      bool ShowUsrResults;	// Show exam with results of all questions for the student
      bool Happening;		// Is being played now?
      unsigned NumParticipants;
     } Status;			// Status related to event playing
  };

struct ExaEvt_UsrAnswer
  {
   int NumOpt;	// < 0 ==> no answer selected
   int AnsInd;	// < 0 ==> no answer selected
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long ExaEvt_GetEvtCodBeingPlayed (void);

void ExaEvt_ListEvents (struct Exa_Exams *Exams,
                        struct Exa_Exam *Exam,
                        bool PutFormNewEvent);
void ExaEvt_GetDataOfEventByCod (struct ExaEvt_Event *Event);

void ExaEvt_ToggleVisibilResultsEvtUsr (void);

void ExaEvt_RequestRemoveEvent (void);
void ExaEvt_RemoveEvent (void);

void ExaEvt_RemoveEventsInExamFromAllTables (long ExaCod);
void ExaEvt_RemoveEventInCourseFromAllTables (long CrsCod);
void ExaEvt_RemoveUsrFromEventTablesInCrs (long UsrCod,long CrsCod);

void ExaEvt_PutParamsEdit (void *Exams);
void ExaEvt_GetAndCheckParameters (struct Exa_Exams *Exams,
                                   struct Exa_Exam *Exam,
                                   struct ExaEvt_Event *Event);
long ExaEvt_GetParamEvtCod (void);

void ExaEvt_CreateNewEventTch (void);
void ExaEvt_ResumeEvent (void);
void ExaEvt_GetIndexes (long EvtCod,unsigned QstInd,
		        unsigned Indexes[Tst_MAX_OPTIONS_PER_QUESTION]);

void ExaEvt_RemoveGroup (long GrpCod);
void ExaEvt_RemoveGroupsOfType (long GrpTypCod);

void ExaEvt_PlayPauseEvent (void);
void ExaEvt_ChangeNumColsEvt (void);
void ExaEvt_ToggleVisibilResultsEvtQst (void);
void ExaEvt_BackEvent (void);
void ExaEvt_ForwardEvent (void);

unsigned ExaEvt_GetNumEventsInExam (long ExaCod);
unsigned ExaEvt_GetNumUnfinishedEventsInExam (long ExaCod);

bool ExaEvt_CheckIfICanPlayThisEventBasedOnGrps (const struct ExaEvt_Event *Event);

void ExaEvt_WriteChoiceAnsViewEvent (const struct ExaEvt_Event *Event,
                                     const struct Tst_Question *Question,
                                     const char *Class,bool ShowResult);

bool ExaEvt_RegisterMeAsParticipantInEvent (struct ExaEvt_Event *Event);

void ExaEvt_GetEventBeingPlayed (void);
void ExaEvt_JoinEventAsStd (void);
void ExaEvt_RemoveMyQuestionAnswer (void);

void ExaEvt_StartCountdown (void);
void ExaEvt_RefreshEventTch (void);
void ExaEvt_RefreshEventStd (void);

void ExaEvt_GetQstAnsFromDB (long EvtCod,long UsrCod,unsigned QstInd,
		             struct ExaEvt_UsrAnswer *UsrAnswer);
void ExaEvt_ReceiveQuestionAnswer (void);

unsigned ExaEvt_GetNumUsrsWhoAnsweredQst (long EvtCod,unsigned QstInd);
unsigned ExaEvt_GetNumUsrsWhoHaveChosenAns (long EvtCod,unsigned QstInd,unsigned AnsInd);
void ExaEvt_DrawBarNumUsrs (unsigned NumRespondersAns,unsigned NumRespondersQst,bool Correct);

#endif
