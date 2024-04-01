// swad_agenda.h: user's agenda (personal organizer)

#ifndef _SWAD_AGD
#define _SWAD_AGD
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_date.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Agd_MAX_CHARS_EVENT	(128 - 1)	// 127
#define Agd_MAX_BYTES_EVENT	((Agd_MAX_CHARS_EVENT + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Agd_MAX_CHARS_LOCATION	(128 - 1)	// 127
#define Agd_MAX_BYTES_LOCATION	((Agd_MAX_CHARS_LOCATION + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Agd_Event
  {
   long AgdCod;
   long UsrCod;
   bool Public;
   HidVis_HiddenOrVisible_t HiddenOrVisible;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   Dat_TimeStatus_t TimeStatus;
   char Title[Agd_MAX_BYTES_EVENT + 1];
   char Location[Agd_MAX_BYTES_LOCATION + 1];
  };

#define Agd_NUM_PAST_FUTURE_EVENTS 2
typedef enum
  {
   Agd_PAST___EVENTS = 0,	// Events until yesterday (included)
   Agd_FUTURE_EVENTS = 1,	// Events from today (included) onwards
  } Agd_Past__FutureEvents_t;
#define Agd_DEFAULT_PAST___EVENTS	(0 << Agd_PAST___EVENTS)	// off
#define Agd_DEFAULT_FUTURE_EVENTS	(1 << Agd_FUTURE_EVENTS)	// on

#define Agd_NUM_PRIVAT_PUBLIC_EVENTS 2
typedef enum
  {
   Agd_PRIVAT_EVENTS = 0,
   Agd_PUBLIC_EVENTS = 1,
  } Agd_PrivatPublicEvents_t;
#define Agd_DEFAULT_PRIVAT_EVENTS	(1 << Agd_PRIVAT_EVENTS)	// on
#define Agd_DEFAULT_PUBLIC_EVENTS	(1 << Agd_PUBLIC_EVENTS)	// on

#define Agd_DEFAULT_HIDDEN_EVENTS	(0 << HidVis_HIDDEN)	// off
#define Agd_DEFAULT_VISIBL_EVENTS	(1 << HidVis_VISIBLE)	// on

#define Agd_ORDER_DEFAULT Dat_STR_TIME

struct Agd_Agenda
  {
   bool LstIsRead;	// Is the list already read from database, or it needs to be read?
   unsigned Num;	// Number of events
   long *LstAgdCods;	// List of agenda codes
   unsigned Past__FutureEvents;
   unsigned PrivatPublicEvents;
   unsigned HiddenVisiblEvents;
   Dat_StartEndTime_t SelectedOrder;
   long AgdCodToEdit;	// Used as parameter in contextual links
   unsigned CurrentPage;
  };

#define Agd_NUM_AGENDA_TYPES 4
typedef enum
  {
   Agd_MY_AGENDA_TODAY,
   Agd_MY_AGENDA,
   Agd_ANOTHER_AGENDA_TODAY,
   Agd_ANOTHER_AGENDA,
  } Agd_AgendaType_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Usr_ICan_t Agd_CheckIfICanViewUsrAgenda (struct Usr_Data *UsrDat);

void Agd_PutFormLogInToShowUsrAgenda (void);
void Agd_PutParAgd (void);

void Agd_GetParsAndShowMyAgenda (void);
void Agd_ShowUsrAgenda (void);
void Agd_ShowOtherAgendaAfterLogIn (void);

void Agd_ReqCreatOrEditEvent (void);

void Agd_PutParsMyAgenda (unsigned Past__FutureEvents,
                            unsigned PrivatPublicEvents,
                            unsigned HiddenVisiblEvents,
			    Dat_StartEndTime_t Order,
                            unsigned NumPage,
                            long AgdCodToEdit);
void Agd_PutParEventsOrder (Dat_StartEndTime_t SelectedOrder);

void Agd_AskRemEvent (void);
void Agd_RemoveEvent (void);

void Agd_HideEvent (void);
void Agd_UnhideEvent (void);

void Agd_MakeEventPrivate (void);
void Agd_MakeEventPublic (void);

void Agd_ReceiveEvent (void);

void Agd_PrintAgdQRCode (void);

//-------------------------------- Figures ------------------------------------
void Agd_GetAndShowAgendasStats (void);

#endif
