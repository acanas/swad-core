// swad_attendance.h: control of attendance

#ifndef _SWAD_ATT
#define _SWAD_ATT
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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

#define Att_MAX_CHARS_ATTENDANCE_EVENT_TITLE	(128 - 1)	// 127
#define Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE	((Att_MAX_CHARS_ATTENDANCE_EVENT_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Att_NUM_ORDERS_NEWEST_OLDEST 2
typedef enum
  {
   Att_NEWEST_FIRST,
   Att_OLDEST_FIRST,
  } Att_OrderNewestOldest_t;

#define Att_ORDER_DEFAULT Dat_STR_TIME

struct Att_Event
  {
   /* Fields stored in database */
   long AttCod;
   long CrsCod;
   bool Hidden;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   bool Open;
   char Title[Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE + 1];
   bool CommentTchVisible;

   /* Fields computed, not associated to the event in database */
   unsigned NumStdsTotal;	// Number total of students who have assisted to the event
   unsigned NumStdsFromList;	// Number of students (taken from a list) who has assisted to the event
   bool Selected;		// I have selected this attendance event
  };

struct Att_Events
  {
   bool LstIsRead;	// Is the list already read from database, or it needs to be read?
   unsigned Num;	// Number of attendance events
   struct Att_Event *Lst;	// List of attendance events
   Dat_StartEndTime_t SelectedOrder;
   long AttCod;
   bool ShowDetails;
   char *StrAttCodsSelected;
   unsigned CurrentPage;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Att_SeeAttEvents (void);
void Att_RequestCreatOrEditAttEvent (void);
bool Att_GetDataOfAttEventByCod (struct Att_Event *Event);

void Att_AskRemAttEvent (void);
void Att_GetAndRemAttEvent (void);
void Att_RemoveAttEventFromDB (long AttCod);

void Att_HideAttEvent (void);
void Att_UnhideAttEvent (void);
void Att_ReceiveFormAttEvent (void);
void Att_CreateAttEvent (struct Att_Event *Event,const char *Description);
void Att_UpdateAttEvent (struct Att_Event *Event,const char *Description);

void Att_RemoveCrsAttEvents (long CrsCod);

unsigned Att_GetNumAttEvents (HieLvl_Level_t Scope,unsigned *NumNotif);

void Att_SeeOneAttEvent (void);

void Att_RegisterMeAsStdInAttEvent (void);
void Att_RegisterStudentsInAttEvent (void);

void Att_ReqListUsrsAttendanceCrs (void);
void Att_ListMyAttendanceCrs (void);
void Att_PrintMyAttendanceCrs (void);
void Att_ListUsrsAttendanceCrs (void);
void Att_PrintUsrsAttendanceCrs (void);

//--------------------------- Program resources -------------------------------
void Att_GetLinkToEvent (void);
void AttRsc_WriteAttEventInCrsProgram (long AttCod,bool PutFormToGo,
                                    const char *Icon,const char *IconTitle);
void AttRsc_GetTitleFromAttCod (long AttCod,char *Title,size_t TitleSize);

#endif
