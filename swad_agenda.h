// swad_agenda.h: user's agenda (personal organizer)

#ifndef _SWAD_AGD
#define _SWAD_AGD
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#define Agd_MAX_CHARS_LOCATION	(256 - 1)
#define Agd_MAX_BYTES_LOCATION	(Agd_MAX_CHARS_LOCATION * Str_MAX_BYTES_PER_CHAR)

#define Agd_MAX_CHARS_EVENT	(256 - 1)
#define Agd_MAX_BYTES_EVENT	(Agd_MAX_CHARS_EVENT * Str_MAX_BYTES_PER_CHAR)

#define Agd_NUM_DATES 2
typedef enum
  {
   Agd_START_TIME = 0,
   Agd_END_TIME   = 1,
  } Agd_StartOrEndTime_t;

struct AgendaEvent
  {
   long AgdCod;
   long UsrCod;
   bool Public;
   bool Hidden;
   time_t TimeUTC[Agd_NUM_DATES];
   Dat_TimeStatus_t TimeStatus;
   char Event[Agd_MAX_BYTES_EVENT + 1];
   char Location[Agd_MAX_BYTES_LOCATION + 1];
  };

typedef enum
  {
   Agd_ALL_EVENTS,
   Agd_ONLY_PUBLIC_EVENTS,
  } Agd_WhichEvents_t;

#define Agd_NUM_ORDERS 2
typedef enum
  {
   Agd_ORDER_BY_START_DATE = 0,
   Agd_ORDER_BY_END_DATE   = 1,
  } Agd_Order_t;
#define Agd_ORDER_DEFAULT Agd_ORDER_BY_START_DATE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Agd_PutFormLogInToShowUsrAgenda (void);
void Agd_PutParamAgd (void);

void Agd_ShowMyAgenda (void);
void Agd_ShowMyPublicAgenda (void);
void Agd_ShowUsrAgenda (void);
void Agd_ShowOtherAgendaAfterLogIn (void);

void Agd_PutHiddenParamEventsOrder (void);
void Agd_RequestCreatOrEditEvent (void);
void Agd_FreeListEvents (void);

long Agd_GetParamAgdCod (void);

void Agd_AskRemEvent (void);
void Agd_RemoveEvent (void);

void Agd_HideEvent (void);
void Agd_UnhideEvent (void);

void Agd_MakeEventPrivate (void);
void Agd_MakeEventPublic (void);

void Agd_RecFormEvent (void);

void Agd_RemoveUsrEvents (long UsrCod);
unsigned Agd_GetNumEventsFromUsr (long UsrCod);

unsigned Agd_GetNumUsrsWithEvents (Sco_Scope_t Scope);
unsigned Agd_GetNumEvents (Sco_Scope_t Scope);

void Agd_PrintAgdQRCode (void);

#endif
