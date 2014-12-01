// swad_attendance.h: control of attendance

#ifndef _SWAD_ATT
#define _SWAD_ATT
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

#define Att_MAX_LENGTH_ATTENDANCE_EVENT_TITLE	255

#define Att_NUM_DATES 2
typedef enum
  {
   Att_START_TIME = 0,
   Att_END_TIME   = 1,
  } Att_StartOrEndTime_t;

struct AttendanceEvent
  {
   long AttCod;
   long CrsCod;
   bool Hidden;
   long UsrCod;
   struct DateTime DateTimes[Att_NUM_DATES];
   bool Open;
   char Title[Att_MAX_LENGTH_ATTENDANCE_EVENT_TITLE+1];
   bool CommentTchVisible;
   unsigned NumStdsTotal;	// Number total of students who have assisted to the event
   unsigned NumStdsFromList;	// Number of students (taken from a list) who has assisted to the event
  };

typedef enum
  {
   Att_ORDER_BY_START_DATE = 0,
   Att_ORDER_BY_END_DATE   = 1,
  } Att_EventsOrderType_t;

typedef enum
  {
   Att_NEWEST_FIRST,
   Att_OLDEST_FIRST,
  } Att_OrderTime_t;

#define Att_DEFAULT_ORDER_TYPE Att_ORDER_BY_START_DATE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Att_SeeAttEvents (void);
void Att_PutHiddenParamAttOrderType (void);
void Att_RequestCreatOrEditAttEvent (void);
bool Att_GetDataOfAttEventByCod (struct AttendanceEvent *Att);
void Att_FreeListAttEvents (void);

void Att_GetNotifAttEvent (char *SummaryStr,char **ContentStr,long AsgCod,unsigned MaxChars,bool GetContent);

void Att_PutParamAttCod (long AttCod);
long Att_GetParamAttCod (void);
void Att_AskRemAttEvent (void);
void Att_RemoveAttEvent (void);
void Att_HideAttEvent (void);
void Att_ShowAttEvent (void);
void Att_RecFormAttEvent (void);
void Att_CreateAttEvent (struct AttendanceEvent *Att,const char *Txt);
void Att_UpdateAttEvent (struct AttendanceEvent *Att,const char *Txt);
bool Att_CheckIfAttEventIsAssociatedToGrps (long AttCod);
bool Att_CheckIfAttEventIsAssociatedToGrp (long AsgCod,long GrpCod);
void Att_RemoveUsrFromAllAttEvents (long UsrCod);
void Att_RemoveUsrFromCrsAttEvents (long UsrCod,long CrsCod);
void Att_RemoveGroup (long GrpCod);
void Att_RemoveGroupsOfType (long GrpTypCod);
void Att_RemoveCrsAttEvents (long CrsCod);
unsigned Att_GetNumAttEventsInCrs(long CrsCod);

unsigned Att_GetNumCoursesWithAttEvents (Sco_Scope_t Scope);
unsigned Att_GetNumAttEvents (Sco_Scope_t Scope,unsigned *NumNotif);

void Att_SeeOneAttEvent (void);

void Att_RegisterMeAsStdInAttEvent (void);
void Att_RegisterStudentsInAttEvent (void);

void Att_RegUsrInAttEventNotChangingComments (long AttCod,long UsrCod);
void Att_RemoveUsrsAbsentWithoutCommentsFromAttEvent (long AttCod);

void Usr_ReqListAttendanceStdsCrs (void);
void Usr_ListAttendanceStdsCrs (void);

#endif
