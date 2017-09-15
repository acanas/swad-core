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

#define Prj_MAX_CHARS_ASSIGNMENT_TITLE	(128 - 1)	// 127
#define Prj_MAX_BYTES_ASSIGNMENT_TITLE	((Prj_MAX_CHARS_ASSIGNMENT_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Prj_NUM_TYPES_SEND_WORK 2
typedef enum
  {
   Prj_DO_NOT_SEND_WORK = 0,
   Prj_SEND_WORK        = 1,
  } Prj_SendWork_t;

struct Project
  {
   long PrjCod;
   bool Hidden;
   long UsrCod;
   time_t TimeUTC[Dat_NUM_START_END_TIME];
   bool Open;
   char Title[Prj_MAX_BYTES_ASSIGNMENT_TITLE + 1];
   Prj_SendWork_t SendWork;
   char Folder[Brw_MAX_BYTES_FOLDER + 1];
   bool IBelongToCrsOrGrps;	// I can do this assignment
				// (it is associated to no groups
				// or, if associated to groups,
				// I belong to any of the groups)
  };

#define Prj_ORDER_DEFAULT Dat_START_TIME

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prj_SeeProjects (void);
void Prj_PrintOneProject (void);

void Prj_PutHiddenParamPrjOrder (void);
void Prj_RequestCreatOrEditPrj (void);
void Prj_GetListProjects (void);
void Prj_GetDataOfProjectByCod (struct Project *Prj);
void Prj_GetDataOfProjectByFolder (struct Project *Prj);
void Prj_FreeListProjects (void);

void Prj_GetNotifProject (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                          char **ContentStr,
                          long PrjCod,bool GetContent);

long Prj_GetParamPrjCod (void);
void Prj_ReqRemProject (void);
void Prj_RemoveProject (void);
void Prj_HideProject (void);
void Prj_ShowProject (void);
void Prj_RecFormProject (void);
bool Prj_CheckIfPrjIsAssociatedToGrp (long PrjCod,long GrpCod);
void Prj_RemoveGroup (long GrpCod);
void Prj_RemoveGroupsOfType (long GrpTypCod);
void Prj_RemoveCrsProjects (long CrsCod);
unsigned Prj_GetNumProjectsInCrs(long CrsCod);

unsigned Prj_GetNumCoursesWithProjects (Sco_Scope_t Scope);
unsigned Prj_GetNumProjects (Sco_Scope_t Scope,unsigned *NumNotif);

#endif
