// swad_location.h: teacher's location

#ifndef _SWAD_LOC
#define _SWAD_LOC
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

#define Loc_MAX_LENGTH_ASSIGNMENT_TITLE	255

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
   char Title[Loc_MAX_LENGTH_ASSIGNMENT_TITLE+1];
   Loc_SendWork_t SendWork;
   char Folder[Loc_MAX_LENGTH_FOLDER+1];
   bool IBelongToCrsOrGrps;	// I can do this location
				// (it is associated to no groups
				// or, if associated to groups,
				// I belong to any of the groups)
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

void Loc_GetNotifLocation (char *SummaryStr,char **ContentStr,long LocCod,unsigned MaxChars,bool GetContent);

long Loc_GetParamLocCod (void);
void Loc_AskRemLocation (void);
void Loc_RemoveLocation (void);
void Loc_HideLocation (void);
void Loc_ShowLocation (void);
void Loc_RecFormLocation (void);
void Loc_RemoveCrsLocations (long CrsCod);
unsigned Loc_GetNumLocationsInCrs(long CrsCod);

unsigned Loc_GetNumCoursesWithLocations (Sco_Scope_t Scope);
unsigned Loc_GetNumLocations (Sco_Scope_t Scope,unsigned *NumNotif);

#endif
