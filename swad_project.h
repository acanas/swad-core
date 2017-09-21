// swad_project.h: projects (final degree projects, thesis)

#ifndef _SWAD_PRJ
#define _SWAD_PRJ
/*
    SWAD (Shared Workspace At a Distance),
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
#include "swad_file_browser.h"
#include "swad_notification.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Prj_MAX_CHARS_PROJECT_TITLE		(128 - 1)	// 127
#define Prj_MAX_BYTES_PROJECT_TITLE		((Prj_MAX_CHARS_PROJECT_TITLE       + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Prj_NUM_TYPES_PREASSIGNED 2
typedef enum
  {
   Prj_NOT_PREASSIGNED = 0,
   Prj_PREASSIGNED     = 1,
  } Prj_Preassigned_t;

struct Project
  {
   long PrjCod;
   bool Hidden;
   Prj_Preassigned_t Preassigned;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   bool Open;
   char Title[Prj_MAX_BYTES_PROJECT_TITLE + 1];
   char *Description;
   char *Knowledge;
   char *Materials;
   char URL[Cns_MAX_BYTES_WWW + 1];
  };

#define Prj_ORDER_DEFAULT Dat_START_TIME

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prj_SeeProjects (void);
void Prj_PrintOneProject (void);

void Prj_ReqAddStd (void);
void Prj_ReqAddTut (void);
void Prj_ReqAddEva (void);

void Prj_AddStd (void);
void Prj_AddTut (void);
void Prj_AddEva (void);

void Prj_PutHiddenParamPrjOrder (void);

void Prj_RequestCreatePrj (void);
void Prj_RequestEditPrj (void);

void Prj_GetListProjects (void);
void Prj_GetDataOfProjectByCod (struct Project *Prj);
void Prj_FreeListProjects (void);

long Prj_GetParamPrjCod (void);
void Prj_ReqRemProject (void);
void Prj_RemoveProject (void);
void Prj_HideProject (void);
void Prj_ShowProject (void);
void Prj_RecFormProject (void);
void Prj_RemoveCrsProjects (long CrsCod);
unsigned Prj_GetNumProjectsInCrs(long CrsCod);

unsigned Prj_GetNumCoursesWithProjects (Sco_Scope_t Scope);
unsigned Prj_GetNumProjects (Sco_Scope_t Scope);

#endif
