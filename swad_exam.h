// swad_exam.h: exam announcements

#ifndef _SWAD_EXA
#define _SWAD_EXA
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include <stdbool.h>		// For boolean type

#include "swad_constant.h"
#include "swad_course.h"
#include "swad_date.h"
#include "swad_notification.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Exa_NUM_VIEWS 3
typedef enum
  {
   Exa_NORMAL_VIEW,
   Exa_PRINT_VIEW,
   Exa_FORM_VIEW,
  } Exa_TypeViewExamAnnouncement_t;

#define Exa_NUM_STATUS 3
typedef enum
  {
   Exa_VISIBLE_EXAM_ANNOUNCEMENT = 0,
   Exa_HIDDEN_EXAM_ANNOUNCEMENT  = 1,
   Exa_DELETED_EXAM_ANNOUNCEMENT = 2,
  } Exa_ExamAnnouncementStatus_t;	// Don't change these numbers because they are used in database

#define Exa_MAX_CHARS_SESSION	(128 - 1)	// 127
#define Exa_MAX_BYTES_SESSION	((Exa_MAX_CHARS_SESSION + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct ExamData
  {
   long ExaCod;
   long CrsCod;
   Exa_ExamAnnouncementStatus_t Status;
   char CrsFullName[Hie_MAX_BYTES_FULL_NAME + 1];
   unsigned Year; // Number of year (0 (N.A.), 1, 2, 3, 4, 5, 6) in the degree
   char Session[Exa_MAX_BYTES_SESSION + 1];	// Exam session is june, september, etc.
   struct Date CallDate;
   struct Date ExamDate;
   struct Hour StartTime;
   struct Hour Duration;
   char *Place;
   char *Mode;
   char *Structure;
   char *DocRequired;
   char *MatRequired;
   char *MatAllowed;
   char *OtherInfo;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Exa_PutFrmEditAExamAnnouncement (void);
void Exa_ReceiveExamAnnouncement1 (void);
void Exa_ReceiveExamAnnouncement2 (void);
void Exa_PrintExamAnnouncement (void);
void Exa_ReqRemoveExamAnnouncement (void);
void Exa_RemoveExamAnnouncement1 (void);
void Exa_RemoveExamAnnouncement2 (void);
void Exa_HideExamAnnouncement1 (void);
void Exa_HideExamAnnouncement2 (void);
void Exa_UnhideExamAnnouncement1 (void);
void Exa_UnhideExamAnnouncement2 (void);

void Exa_FreeMemExamAnnouncement (void);
void Exa_FreeListExamAnnouncements (void);
void Exa_ListExamAnnouncementsSee (void);

void Exa_GetExaCodToHighlight (void);
void Exa_GetDateToHighlight (void);

void Exa_CreateListDatesOfExamAnnouncements (void);
void Exa_PutHiddenParamExaCod (long ExaCod);
void Exa_GetSummaryAndContentExamAnnouncement (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                               char **ContentStr,
                                               long ExaCod,bool GetContent);

#endif
