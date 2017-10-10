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

/***** Filters to list departments *****/
/* My projects / all projects */
#define Prj_NUM_WHOSE_PROJECTS 2
typedef enum
  {
   Prj_MY__PROJECTS,
   Prj_ALL_PROJECTS,
  } Prj_WhoseProjects_t;
#define Prj_FILTER_WHOSE_PROJECTS_DEFAULT	Prj_MY__PROJECTS

/* Preassigned projects / non-preassigned projects */
#define Prj_NUM_PREASSIGNED_NONPREASSIG 2
typedef enum
  {
   Prj_PREASSIGNED = 0,
   Prj_NONPREASSIG = 1,
  } Prj_PreassignedNonpreassig_t;
#define Prj_NEW_PRJ_PREASSIGNED_NONPREASSIG_DEFAULT Prj_NONPREASSIG
#define Prj_FILTER_PREASSIGNED_DEFAULT	(1 << Prj_PREASSIGNED)	// on
#define Prj_FILTER_NONPREASSIG_DEFAULT	(1 << Prj_NONPREASSIG)	// on

/* Hidden projects / visible projects */
#define Prj_NUM_HIDDEN_VISIBL 2
typedef enum
  {
   Prj_HIDDEN = 0,
   Prj_VISIBL = 1,
  } Prj_HiddenVisibl_t;
#define Prj_NEW_PRJ_HIDDEN_VISIBL_DEFAULT Prj_VISIBL
#define Prj_FILTER_HIDDEN_DEFAULT	(1 << Prj_HIDDEN)	// on
#define Prj_FILTER_VISIBL_DEFAULT	(1 << Prj_VISIBL)	// on

/* Project department */
#define Prj_FILTER_DPT_DEFAULT -1L	// Any department

/* Struct with all filters */
struct Prj_Filter
  {
   Prj_WhoseProjects_t My_All;	// Show my / all projects
   unsigned PreNon;		// Show preassigned / non preassigned projects
   unsigned HidVis;		// Show hidden / visible projects
   long DptCod;			// Show projects of this department
  };

/***** Order listing of projects by... *****/
#define Prj_NUM_ORDERS 4
typedef enum
  {
   Prj_ORDER_START_TIME = 0,
   Prj_ORDER_END_TIME   = 1,
   Prj_ORDER_TITLE      = 2,
   Prj_ORDER_DEPARTMENT = 3,
  } Prj_Order_t;
#define Prj_ORDER_DEFAULT Prj_ORDER_START_TIME

/***** Project title *****/
#define Prj_MAX_CHARS_PROJECT_TITLE	(128 - 1)	// 127
#define Prj_MAX_BYTES_PROJECT_TITLE	((Prj_MAX_CHARS_PROJECT_TITLE       + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

/***** Type of proposal ******/
#define Prj_NUM_PROPOSAL_TYPES 3
typedef enum
  {
   Prj_PROPOSAL_NEW,
   Prj_PROPOSAL_MODIFIED,
   Prj_PROPOSAL_UNMODIFIED,
  } Prj_Proposal_t;
#define Prj_PROPOSAL_DEFAULT Prj_PROPOSAL_NEW

/***** User roles in a project *****/
// Don't change these numbers! They are used in database
#define Prj_NUM_ROLES_IN_PROJECT 4
typedef enum
  {
   Prj_ROLE_UNK	= 0,	// Unknown
   Prj_ROLE_STD	= 1,	// Student
   Prj_ROLE_TUT	= 2,	// Tutor
   Prj_ROLE_EVA	= 3,	// Evaluator
  } Prj_RoleInProject_t;

/***** Struct to store a project *****/
struct Project
  {
   long PrjCod;
   long CrsCod;
   Prj_HiddenVisibl_t Hidden;
   Prj_PreassignedNonpreassig_t Preassigned;
   unsigned NumStds;
   Prj_Proposal_t Proposal;
   time_t CreatTime;
   time_t ModifTime;
   char Title[Prj_MAX_BYTES_PROJECT_TITLE + 1];
   long DptCod;
   char *Description;
   char *Knowledge;
   char *Materials;
   char URL[Cns_MAX_BYTES_WWW + 1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prj_SeeProjects (void);
void Prj_ShowTableAllProjects (void);

void Prj_PutParams (struct Prj_Filter *Filter,
                    Prj_Order_t Order,
                    unsigned NumPage,
                    long PrjCod);

void Prj_ShowOneUniqueProject (struct Project *Prj);

void Prj_PrintOneProject (void);

void Prj_FlushCacheMyRoleInProject (void);
Prj_RoleInProject_t Prj_GetMyRoleInProject (long PrjCod);

void Prj_ReqAddStd (void);
void Prj_ReqAddTut (void);
void Prj_ReqAddEva (void);

void Prj_AddStd (void);
void Prj_AddTut (void);
void Prj_AddEva (void);

void Prj_ReqRemStd (void);
void Prj_ReqRemTut (void);
void Prj_ReqRemEva (void);

void Prj_RemStd (void);
void Prj_RemTut (void);
void Prj_RemEva (void);

void Prj_PutHiddenParamPrjOrder (void);

bool Prj_CheckIfICanViewProjectFiles (Prj_RoleInProject_t MyRoleInProject);

void Prj_RequestCreatePrj (void);
void Prj_RequestEditPrj (void);

void Prj_AllocMemProject (struct Project *Prj);
void Prj_FreeMemProject (struct Project *Prj);

void Prj_GetListProjects (void);
void Prj_GetDataOfProjectByCod (struct Project *Prj);
long Prj_GetCourseOfProject (long PrjCod);
void Prj_FreeListProjects (void);

void Prj_PutParamPrjCod (long PrjCod);
long Prj_GetParamPrjCod (void);
void Prj_ReqRemProject (void);
void Prj_RemoveProject (void);
void Prj_HideProject (void);
void Prj_ShowProject (void);
void Prj_RecFormProject (void);

void Prj_RemoveCrsProjects (long CrsCod);
void Prj_RemoveUsrFromProjects (long UsrCod);

unsigned Prj_GetNumCoursesWithProjects (Sco_Scope_t Scope);
unsigned Prj_GetNumProjects (Sco_Scope_t Scope);

#endif
