// swad_call_for_exam.h: calls for exams

#ifndef _SWAD_CFE
#define _SWAD_CFE
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#define Cfe_NUM_VIEWS 3
typedef enum
  {
   Cfe_NORMAL_VIEW,
   Cfe_PRINT_VIEW,
   Cfe_FORM_VIEW,
  } Cfe_TypeViewCallForExam_t;

#define Cfe_NUM_STATUS 3
typedef enum
  {
   Cfe_VISIBLE_CALL_FOR_EXAM = 0,
   Cfe_HIDDEN_CALL_FOR_EXAM  = 1,
   Cfe_DELETED_CALL_FOR_EXAM = 2,
  } Cfe_Status_t;	// Don't change these numbers because they are used in database
#define Cfe_STATUS_DEFAULT Cfe_VISIBLE_CALL_FOR_EXAM

#define Cfe_MAX_CHARS_SESSION	(128 - 1)	// 127
#define Cfe_MAX_BYTES_SESSION	((Cfe_MAX_CHARS_SESSION + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

struct Cfe_CallForExam
  {
   long CrsCod;
   Cfe_Status_t Status;
   char CrsFullName[Nam_MAX_BYTES_FULL_NAME + 1];
   unsigned Year; // Number of year (0 (N.A.), 1, 2, 3, 4, 5, 6) in the degree
   char Session[Cfe_MAX_BYTES_SESSION + 1];	// Exam session is june, september, etc.
   struct Dat_Date CallDate;
   struct Dat_Date ExamDate;
   struct Dat_Hour StartTime;
   struct Dat_Hour Duration;
   char *Place;
   char *Mode;
   char *Structure;
   char *DocRequired;
   char *MatRequired;
   char *MatAllowed;
   char *OtherInfo;
  };

struct Cfe_ExamCodeAndDate
  {
   long ExaCod;
   struct Dat_Date ExamDate;
  };

struct Cfe_CallsForExams
  {
   unsigned NumCallsForExams;		// Number of calls for exams in the list
   struct Cfe_ExamCodeAndDate *Lst;	// List of calls for exams
   long NewExaCod;			// New call for exam just created
   long HighlightExaCod;		// Call for exam to be highlighted
   char HighlightDate[4 + 2 + 2 + 1];	// Date with calls for exams to be highlighted (in YYYYMMDD format)
   long ExaCod;				// Used to put contextual icons
   const char *Anchor;			// Used to put contextual icons
   struct Cfe_CallForExam CallForExam;
  };

#define Cfe_MAX_BYTES_SESSION_AND_DATE (Cfe_MAX_BYTES_SESSION + (2 + Cns_MAX_BYTES_DATE + 7) + 1)

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

struct Cfe_CallsForExams *Cfe_GetGlobalCallsForExams (void);

void Cfe_ResetCallsForExams (struct Cfe_CallsForExams *CallsForExams);

void Cfe_PutFrmEditACallForExam (void);
void Cfe_AllocMemCallForExam (struct Cfe_CallsForExams *CallsForExams);
void Cfe_FreeMemCallForExam (struct Cfe_CallsForExams *CallsForExams);
void Cfe_ReceiveCallForExam1 (void);
void Cfe_ReceiveCallForExam2 (void);
void Cfe_PrintCallForExam (void);
void Cfe_ReqRemCallForExam (void);
void Cfe_RemoveCallForExam1 (void);
void Cfe_RemoveCallForExam2 (void);
void Cfe_HideCallForExam (void);
void Cfe_UnhideCallForExam (void);

void Cfe_FreeListCallsForExams (struct Cfe_CallsForExams *CallsForExams);
void Cfe_GetCallForExamDataByCod (struct Cfe_CallsForExams *CallsForExams,
                                  long ExaCod);
void Cfe_ListCallsForExamsSee (void);
void Cfe_ListCallsForExamsEdit (void);

void Cfe_ListCallsForExamsCod (void);
void Cfe_ListCallsForExamsDay (void);

void Cfe_CreateListCallsForExams (struct Cfe_CallsForExams *CallsForExams);

bool Cfe_CheckIfICanEditCallsForExams (void);

void Cfe_GetSummaryAndContentCallForExam (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                          char **ContentStr,
                                          long ExaCod,bool GetContent);

void Cfe_BuildSessionAndDate (const struct Cfe_CallsForExams *CallsForExams,
                              char SessionAndDate[Cfe_MAX_BYTES_SESSION_AND_DATE]);

#endif
