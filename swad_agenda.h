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

#define Loc_MAX_LENGTH_LOCATION	255
#define Loc_MAX_LENGTH_EVENT	255

#define Loc_MAX_LENGTH_FOLDER 32

#define Loc_NUM_TYPES_SEND_WORK 2
typedef enum
  {
   Loc_DO_NOT_SEND_WORK = 0,
   Loc_SEND_WORK        = 1,
  } Loc_SendWork_t;

#define Loc_NUM_DATES 2
typedef enum
  {
   Loc_START_TIME = 0,
   Loc_END_TIME   = 1,
  } Loc_StartOrEndTime_t;

struct Location
  {
   long LocCod;
   bool Hidden;
   long UsrCod;
   time_t TimeUTC[Loc_NUM_DATES];
   bool Open;
   char Location[Loc_MAX_LENGTH_LOCATION+1];
   char Event[Loc_MAX_LENGTH_EVENT+1];
  };

typedef enum
  {
   Loc_ORDER_BY_START_DATE = 0,
   Loc_ORDER_BY_END_DATE   = 1,
  } Loc_Order_t;

#define Loc_DEFAULT_ORDER_TYPE Loc_ORDER_BY_START_DATE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Loc_SeeLocations (void);
void Loc_PutHiddenParamLocOrderType (void);
void Loc_RequestCreatOrEditLoc (void);
void Loc_GetListLocations (void);
void Loc_GetDataOfLocationByCod (struct Location *Loc);
void Loc_FreeListLocations (void);

long Loc_GetParamLocCod (void);
void Loc_AskRemLocation (void);
void Loc_RemoveLocation (void);
void Loc_HideLocation (void);
void Loc_ShowLocation (void);
void Loc_RecFormLocation (void);
void Loc_RemoveUsrLocations (long UsrCod);
unsigned Loc_GetNumLocationsFromUsr (long UsrCod);

unsigned Loc_GetNumUsrsWithLocations (Sco_Scope_t Scope);
unsigned Loc_GetNumLocations (Sco_Scope_t Scope);

#endif
