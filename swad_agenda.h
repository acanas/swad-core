// swad_agenda.h: user's agenda (personal organizer)

#ifndef _SWAD_AGD
#define _SWAD_AGD
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#define Agd_MAX_LENGTH_LOCATION	255
#define Agd_MAX_LENGTH_EVENT	255

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
   char Event[Agd_MAX_LENGTH_EVENT+1];
   char Location[Agd_MAX_LENGTH_LOCATION+1];
  };

typedef enum
  {
   Agd_ALL_EVENTS,
   Agd_ONLY_PUBLIC_EVENTS,
  } Agd_WhichEvents_t;

typedef enum
  {
   Agd_ORDER_BY_START_DATE = 0,
   Agd_ORDER_BY_END_DATE   = 1,
  } Agd_Order_t;

#define Agd_DEFAULT_ORDER_TYPE Agd_ORDER_BY_END_DATE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Agd_PutFormLogInToShowUsrAgenda (void);
void Agd_PutParamAgd (void);

void Agd_ShowUsrAgendaAfterLogIn (void);
void Agd_ShowUsrAgenda (void);
void Agd_ShowMyAgenda (void);

void Agd_PutHiddenParamEventsOrderType (void);
void Agd_RequestCreatOrEditEvent (void);
void Agd_FreeListEvents (void);

long Agd_GetParamAgdCod (void);

void Agd_AskRemEvent (void);
void Agd_RemoveEvent (void);

void Agd_HideEvent (void);
void Agd_ShowEvent (void);

void Agd_MakeEventPrivate (void);
void Agd_MakeEventPublic (void);

void Agd_RecFormEvent (void);

void Agd_RemoveUsrEvents (long UsrCod);
unsigned Agd_GetNumEventsFromUsr (long UsrCod);

unsigned Agd_GetNumUsrsWithEvents (Sco_Scope_t Scope);
unsigned Agd_GetNumEvents (Sco_Scope_t Scope);

#endif
