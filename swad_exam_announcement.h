// swad_exam_announcement.h: exam announcements

#ifndef _SWAD_EXA_ANN
#define _SWAD_EXA_ANN
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#define ExaAnn_NUM_VIEWS 3
typedef enum
  {
   ExaAnn_NORMAL_VIEW,
   ExaAnn_PRINT_VIEW,
   ExaAnn_FORM_VIEW,
  } ExaAnn_TypeViewExamAnnouncement_t;

#define ExaAnn_NUM_STATUS 3
typedef enum
  {
   ExaAnn_VISIBLE_EXAM_ANNOUNCEMENT = 0,
   ExaAnn_HIDDEN_EXAM_ANNOUNCEMENT  = 1,
   ExaAnn_DELETED_EXAM_ANNOUNCEMENT = 2,
  } ExaAnn_Status_t;	// Don't change these numbers because they are used in database

#define ExaAnn_MAX_CHARS_SESSION	(128 - 1)	// 127
#define ExaAnn_MAX_BYTES_SESSION	((ExaAnn_MAX_CHARS_SESSION + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct ExaAnn_ExamAnnouncement
  {
   long CrsCod;
   ExaAnn_Status_t Status;
   char CrsFullName[Hie_MAX_BYTES_FULL_NAME + 1];
   unsigned Year; // Number of year (0 (N.A.), 1, 2, 3, 4, 5, 6) in the degree
   char Session[ExaAnn_MAX_BYTES_SESSION + 1];	// Exam session is june, september, etc.
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

struct ExaAnn_ExamCodeAndDate
  {
   long ExaCod;
   struct Date ExamDate;
  };

struct ExaAnn_ExamAnnouncements
  {
   unsigned NumExaAnns;	// Number of announcements of exam in the list
   struct ExaAnn_ExamCodeAndDate *Lst;	// List of exam announcements
   long NewExaCod;		// New exam announcement just created
   long HighlightExaCod;	// Exam announcement to be highlighted
   char HighlightDate[4 + 2 + 2 + 1];	// Date with exam announcements to be highlighted (in YYYYMMDD format)
   long ExaCod;		// Used to put contextual icons
   const char *Anchor;	// Used to put contextual icons
   struct ExaAnn_ExamAnnouncement ExamAnn;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void ExaAnn_ResetExamAnnouncements (struct ExaAnn_ExamAnnouncements *ExamAnns);

void ExaAnn_PutFrmEditAExamAnnouncement (void);
void ExaAnn_ReceiveExamAnnouncement1 (void);
void ExaAnn_ReceiveExamAnnouncement2 (void);
void ExaAnn_PrintExamAnnouncement (void);
void ExaAnn_ReqRemoveExamAnnouncement (void);
void ExaAnn_RemoveExamAnnouncement1 (void);
void ExaAnn_RemoveExamAnnouncement2 (void);
void ExaAnn_HideExamAnnouncement (void);
void ExaAnn_UnhideExamAnnouncement (void);

void ExaAnn_FreeListExamAnnouncements (struct ExaAnn_ExamAnnouncements *ExamAnns);
void ExaAnn_ListExamAnnouncementsSee (void);
void ExaAnn_ListExamAnnouncementsEdit (void);

void ExaAnn_ListExamAnnouncementsCod (void);
void ExaAnn_ListExamAnnouncementsDay (void);

void ExaAnn_CreateListExamAnnouncements (struct ExaAnn_ExamAnnouncements *ExamAnns);
void ExaAnn_PutHiddenParamExaCod (long ExaCod);

void ExaAnn_GetSummaryAndContentExamAnnouncement (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                                  char **ContentStr,
                                                  long ExaCod,bool GetContent);

#endif
