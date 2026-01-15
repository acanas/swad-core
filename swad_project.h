// swad_project.h: projects (final degree projects, thesis)

#ifndef _SWAD_PRJ
#define _SWAD_PRJ
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_browser.h"
#include "swad_date.h"
#include "swad_notification.h"
#include "swad_project_config.h"
#include "swad_user.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/***** User roles in a project *****/
// Don't change these numbers! They are used in database
#define Prj_NUM_ROLES_IN_PROJECT 4
typedef enum
  {
   Prj_ROLE_UNK	= 0,	// Unknown
   Prj_ROLE_STD	= 1,	// Student
   Prj_ROLE_TUT	= 2,	// Tutor
   Prj_ROLE_EVL	= 3,	// Evaluator
  } Prj_RoleInProject_t;

/***** Filters to list projects *****/
/* Whose projects */
#define Prj_FILTER_WHO_DEFAULT	Usr_WHO_ME

/* Roles in projects */
#define Prj_FILTER_ROLE_UNK_DEFAULT	(1 << Prj_ROLE_UNK)	// on
#define Prj_FILTER_ROLE_STD_DEFAULT	(1 << Prj_ROLE_STD)	// on
#define Prj_FILTER_ROLE_TUT_DEFAULT	(1 << Prj_ROLE_TUT)	// on
#define Prj_FILTER_ROLE_EVL_DEFAULT	(1 << Prj_ROLE_EVL)	// on

/* Non-assigned/assigned projects */
#define Prj_NUM_ASSIGNED 2
typedef enum
  {
   Prj_NONASSIG = 0,
   Prj_ASSIGNED = 1,
  } Prj_Assigned_t;
#define Prj_NEW_PRJ_ASSIGNED_DEFAULT	Prj_NONASSIG
#define Prj_FILTER_NONASSIG_DEFAULT	(1 << Prj_NONASSIG)	// on
#define Prj_FILTER_ASSIGNED_DEFAULT	(1 << Prj_ASSIGNED)	// on

/* Locked/unlocked project */
#define Prj_NUM_LOCKED 2
typedef enum
  {
   Prj_LOCKED   = 0,
   Prj_UNLOCKED = 1,
  } Prj_Locked_t;

/* Hidden/visible project */
#define Prj_NEW_PRJ_HIDDEN_VISIBL_DEFAULT HidVis_VISIBLE
#define Prj_FILTER_HIDDEN_DEFAULT	(1 << HidVis_HIDDEN)	// on
#define Prj_FILTER_VISIBL_DEFAULT	(1 << HidVis_VISIBLE)	// on

/* Faulty/faultless project */
#define Prj_NUM_FAULTINESS 2
typedef enum
  {
   Prj_FAULTY    = 0,
   Prj_FAULTLESS = 1,
  } Prj_Faultiness_t;
#define Prj_FILTER_FAULTY_DEFAULT	(1 << Prj_FAULTY)	// on
#define Prj_FILTER_FAULTLESS_DEFAULT	(1 << Prj_FAULTLESS)	// on

/* Review status project */
#define Prj_NUM_REVIEW_STATUS 3
typedef enum
  {
   Prj_UNREVIEWED = 0,
   Prj_UNAPPROVED = 1,
   Prj_APPROVED   = 2,
  } Prj_ReviewStatus_t;
#define Prj_REVIEW_STATUS_DEFAULT Prj_UNREVIEWED
#define Prj_FILTER_UNREVIEWED_DEFAULT	(1 << Prj_UNREVIEWED)	// on
#define Prj_FILTER_UNAPPROVED_DEFAULT	(1 << Prj_UNAPPROVED)	// on
#define Prj_FILTER_APPROVED_DEFAULT	(1 << Prj_APPROVED)	// on

/* Project department */
#define Prj_FILTER_DPT_DEFAULT -1L	// Any department

/* Struct with all filters */
struct Prj_Filter
  {
   Usr_Who_t Who;		// Show my / selected users' / all projects
   unsigned RolPrj;		// Show projects with selected users with a role in project
   unsigned Assign;		// Show assigned / non assigned projects
   unsigned Hidden;		// Show hidden / visible projects
   unsigned Faulti;		// Show faulty / faultless projects
   unsigned Review;		// Show projects depending on review status
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
#define Prj_MAX_CHARS_TITLE	(256 - 1)	// 255
#define Prj_MAX_BYTES_TITLE	((Prj_MAX_CHARS_TITLE + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 4095

/***** Type of proposal ******/
#define Prj_NUM_PROPOSAL_TYPES 3
typedef enum
  {
   Prj_PROPOSAL_NEW,
   Prj_PROPOSAL_MODIFIED,
   Prj_PROPOSAL_UNMODIFIED,
  } Prj_Proposal_t;
#define Prj_PROPOSAL_DEFAULT Prj_PROPOSAL_NEW

/***** Type of view when writing one project *****/
typedef enum
  {
   Prj_LIST_PROJECTS,
   Prj_VIEW_ONE_PROJECT,
   Prj_PRINT_ONE_PROJECT,
   Prj_EDIT_ONE_PROJECT,
  } Prj_ProjectView_t;

/***** Struct to store a project *****/
struct Prj_Project
  {
   long PrjCod;	// Project code
   long HieCod;	// Course code
   Prj_Locked_t Locked;
   HidVis_HiddenOrVisible_t Hidden;
   Prj_Assigned_t Assigned;
   unsigned NumStds;
   Prj_Proposal_t Proposal;
   time_t CreatTime;
   time_t ModifTime;
   char Title[Prj_MAX_BYTES_TITLE + 1];
   long DptCod;
   char *Description;
   char *Knowledge;
   char *Materials;
   char URL[WWW_MAX_BYTES_WWW + 1];
   struct
     {
      Prj_ReviewStatus_t Status;
      time_t Time;
      char *Txt;
     } Review;
  };

/***** Struct to store context/status of projects *****/
struct Prj_Projects
  {
   Prj_ProjectView_t View;
   struct PrjCfg_Config Config;
   struct Prj_Filter Filter;
   Cac_Status_t LstReadStatus;	// Is the list already read from database, or it needs to be read?
   unsigned Num;		// Number of projects
   long *LstPrjCods;		// List of project codes
   Prj_Order_t SelectedOrder;
   unsigned CurrentPage;
   unsigned NumIndex;		// Index shown in first column of projects list
   struct Prj_Project Prj;	// Selected/current project
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prj_SetPrjCod (long PrjCod);
long Prj_GetPrjCod (void);

void Prj_ResetPrjsAndReadConfig (struct Prj_Projects *Projects);

void Prj_ListUsrsToSelect (void);
void Prj_SeeAllProjects (void);
void Prj_ShowProjects (struct Prj_Projects *Projects);
void Prj_ShowTableSelectedPrjs (void);

Usr_Can_t Prj_CheckIfICanViewProjectDocuments (long PrjCod);
Usr_Can_t Prj_CheckIfICanViewProjectAssessment (long PrjCod);

void Prj_PutCurrentPars (void *Projects);
void Prj_PutPars (const struct Prj_Filter *Filter,
                  Prj_Order_t Order,
                  unsigned NumPage,
                  long PrjCod,
                  Usr_UseListSelectedUsrs UseListSelectedUsrs);
void Prj_GetPars (struct Prj_Projects *Projects,
		  Usr_UseListSelectedUsrs UseListSelectedUsrs);

void Prj_PutIconsListProjects (void *Projects);

void Prj_ShowOneProject (void);
void Prj_ShowBoxWithOneProject (struct Prj_Projects *Projects);

void Prj_PrintOneProject (void);

void Prj_FlushCacheMyRolesInProject (void);
unsigned Prj_GetMyRolesInProject (long PrjCod);

void Prj_ReqAddStds (void);
void Prj_ReqAddTuts (void);
void Prj_ReqAddEvls (void);

void Prj_GetSelectedUsrsAndAddStds (void);
void Prj_GetSelectedUsrsAndAddTuts (void);
void Prj_GetSelectedUsrsAndAddEvls (void);

void Prj_ReqRemStd (void);
void Prj_ReqRemTut (void);
void Prj_ReqRemEvl (void);

void Prj_RemStd (void);
void Prj_RemTut (void);
void Prj_RemEvl (void);

void Prj_ReqCreatePrj (void);
void Prj_ReqEditPrj (void);

void Prj_AllocMemProject (struct Prj_Project *Prj);
void Prj_FreeMemProject (struct Prj_Project *Prj);

void Prj_GetProjectDataByCod (struct Prj_Project *Prj);
void Prj_FreeListProjects (struct Prj_Projects *Projects);

void Prj_ReqRemProject (void);
void Prj_RemoveProject (void);
void Prj_HideProject (void);
void Prj_UnhideProject (void);

void Prj_ReceiveProject (void);

void Prj_ReqLockSelectedPrjsEdition (void);
void Prj_ReqUnloSelectedPrjsEdition (void);
void Prj_LockSelectedPrjsEdition (void);
void Prj_UnloSelectedPrjsEdition (void);
void Prj_LockProjectEdition (void);
void Prj_UnloProjectEdition (void);

void Prj_ChangeReviewStatus (void);

void Prj_ChangeCriterionScore (void);

void Prj_RemoveCrsProjects (long HieCod);
void Prj_RemoveUsrFromProjects (long UsrCod);

//-------------------------------- Figures ------------------------------------
void Prj_GetAndShowProjectsStats (Hie_Level_t HieLvl);

#endif
