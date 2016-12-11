// swad_exam.h: exam announcements

#ifndef _SWAD_EXA
#define _SWAD_EXA
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

#include <stdbool.h>		// For boolean type

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

typedef enum
  {
   Exa_NORMAL_VIEW,
   Exa_PRINT_VIEW,
   Exa_FORM_VIEW,
  } Exa_TypeViewExamAnnouncement_t;

typedef enum
  {
   Exa_ACTIVE_EXAM_ANNOUNCEMENT   = 0,
   Exa_OBSOLETE_EXAM_ANNOUNCEMENT = 1,
   Exa_DELETED_EXAM_ANNOUNCEMENT  = 2,
  } Exa_ExamAnnouncementStatus_t;	// Don't change these numbers because they are used in database

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Exa_PutFrmEditAExamAnnouncement (void);
void Exa_ReceiveExamAnnouncement (void);
void Exa_PrintExamAnnouncement (void);
void Exa_ReqRemoveExamAnnouncement (void);
void Exa_RemoveExamAnnouncement (void);
void Exa_FreeMemExamAnnouncement (void);
void Exa_FreeListExamAnnouncements (void);
void Exa_ListExamAnnouncementsSee (void);

void Exa_GetExaCodToHighlight (void);
void Exa_GetDateToHighlight (void);

void Exa_CreateListOfExamAnnouncements (void);
void Exa_PutHiddenParamExaCod (long ExaCod);
void Exa_GetSummaryAndContentExamAnnouncement (char *SummaryStr,char **ContentStr,
                                               long ExaCod,unsigned MaxChars,bool GetContent);

#endif
