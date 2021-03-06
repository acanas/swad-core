// swad_project.c: projects (final degree projects, thesis)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Ca�as Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_department.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_project.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

/***** Parameters used to filter listing of projects *****/
#define Prj_PARAM_PRE_NON_NAME	"PreNon"
#define Prj_PARAM_HID_VIS_NAME	"HidVis"
#define Prj_PARAM_FAULTIN_NAME	"Faulti"

/***** Type of view when writing one project *****/
typedef enum
  {
   Prj_LIST_PROJECTS,
   Prj_FILE_BROWSER_PROJECT,
   Prj_PRINT_ONE_PROJECT,
   Prj_EDIT_ONE_PROJECT,
  } Prj_ProjectView_t;

/***** User roles are shown in this order *****/
static const Prj_RoleInProject_t Prj_RolesToShow[] =
  {
   Prj_ROLE_TUT,	// Tutor
   Prj_ROLE_STD,	// Student
   Prj_ROLE_EVL,	// Evaluator
  };
static const unsigned Brw_NUM_ROLES_TO_SHOW = sizeof (Prj_RolesToShow) /
                                              sizeof (Prj_RolesToShow[0]);

/***** Enum field in database for types of proposal *****/
static const char *Prj_Proposal_DB[Prj_NUM_PROPOSAL_TYPES] =
  {
   [Prj_PROPOSAL_NEW       ] = "new",
   [Prj_PROPOSAL_MODIFIED  ] = "modified",
   [Prj_PROPOSAL_UNMODIFIED] = "unmodified",
  };

/***** Assigned/non-assigned project *****/
static const char *AssignedNonassigImage[Prj_NUM_ASSIGNED_NONASSIG] =
  {
   [Prj_ASSIGNED] = "user.svg",
   [Prj_NONASSIG] = "user-slash.svg",
  };

/***** Locked/unlocked project edition *****/
static const char *Prj_LockIcons[Prj_NUM_LOCKED_UNLOCKED] =
  {
   [Prj_LOCKED  ] = "lock.svg",
   [Prj_UNLOCKED] = "unlock.svg",
  };
static const Act_Action_t Prj_LockActions[Prj_NUM_LOCKED_UNLOCKED] =
  {
   [Prj_LOCKED  ] = ActUnlPrj,
   [Prj_UNLOCKED] = ActLckPrj,
  };

/***** List of users to select one or more members
       to be added to a project *****/
struct SelectedUsrs Prj_MembersToAdd =
  {
   .List =
     {
      NULL,	// Rol_UNK
      NULL,	// Rol_GST
      NULL,	// Rol_USR
      NULL,	// Rol_STD
      NULL,	// Rol_NET
      NULL,	// Rol_TCH
      NULL,	// Rol_DEG_ADM
      NULL,	// Rol_CTR_ADM
      NULL,	// Rol_INS_ADM
      NULL,	// Rol_SYS_ADM
     },
   .Filled      = false,
   .ParamSuffix = "Member",
   .Option      = Usr_OPTION_UNKNOWN,
  };

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Prj_Faults
  {
   bool WrongTitle;
   bool WrongDescription;
   bool WrongNumStds;
   bool WrongAssigned;
  };
/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static long Prj_PrjCod = -1L;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prj_ResetProjects (struct Prj_Projects *Projects);

static void Prj_ReqUsrsToSelect (void *Projects);
static void Prj_GetSelectedUsrsAndShowTheirPrjs (struct Prj_Projects *Projects);
static void Prj_ShowProjects (struct Prj_Projects *Projects);
static void Prj_ShowPrjsInCurrentPage (void *Projects);

static void Prj_ShowFormToFilterByMy_All (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByAssign (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByHidden (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByWarning (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByDpt (const struct Prj_Projects *Projects);

static void Prj_PutCurrentParams (void *Projects);
static void Prj_PutHiddenParamAssign (unsigned Assign);
static void Prj_PutHiddenParamHidden (unsigned Hidden);
static void Prj_PutHiddenParamFaulti (unsigned Faulti);
static void Prj_PutHiddenParamDptCod (long DptCod);
static void Prj_GetHiddenParamPreNon (struct Prj_Projects *Projects);
static Prj_HiddenVisibl_t Prj_GetHiddenParamHidVis (void);
static unsigned Prj_GetHiddenParamFaulti (void);
static long Prj_GetHiddenParamDptCod (void);
static void Prj_GetParams (struct Prj_Projects *Projects);
static Usr_Who_t Prj_GetParamWho (void);

static void Prj_ShowProjectsHead (struct Prj_Projects *Projects,
                                  Prj_ProjectView_t ProjectView);
static void Prj_ShowTableAllProjectsHead (void);
static bool Prj_CheckIfICanCreateProjects (void);
static void Prj_PutIconsListProjects (void *Projects);
static void Prj_PutIconToCreateNewPrj (struct Prj_Projects *Projects);
static void Prj_PutButtonToCreateNewPrj (struct Prj_Projects *Projects);
static void Prj_PutIconToShowAllData (struct Prj_Projects *Projects);

static void Prj_ShowOneProject (struct Prj_Projects *Projects,
				unsigned NumIndex,
                                struct Prj_Project *Prj,
                                Prj_ProjectView_t ProjectView);
static bool Prj_CheckIfPrjIsFaulty (long PrjCod,struct Prj_Faults *Faults);
static void Prj_PutWarningIcon (void);
static void Prj_PutIconToToggleProject (unsigned UniqueId,
                                        const char *Icon,const char *Text);
static void Prj_ShowTableAllProjectsOneRow (struct Prj_Project *Prj);
static void Prj_ShowOneProjectDepartment (const struct Prj_Project *Prj,
                                          Prj_ProjectView_t ProjectView);
static void Prj_ShowTableAllProjectsDepartment (const struct Prj_Project *Prj);
static void Prj_ShowOneProjectTxtField (struct Prj_Project *Prj,
                                        Prj_ProjectView_t ProjectView,
                                        const char *id,unsigned UniqueId,
                                        const char *Label,char *TxtField,
					bool Warning);
static void Prj_ShowTableAllProjectsTxtField (struct Prj_Project *Prj,
                                              char *TxtField);
static void Prj_ShowOneProjectURL (const struct Prj_Project *Prj,
                                   Prj_ProjectView_t ProjectView,
                                   const char *id,unsigned UniqueId);
static void Prj_ShowTableAllProjectsURL (const struct Prj_Project *Prj);
static void Prj_ShowOneProjectMembers (struct Prj_Projects *Projects,
                                       struct Prj_Project *Prj,
                                       Prj_ProjectView_t ProjectView);
static void Prj_ShowOneProjectMembersWithARole (struct Prj_Projects *Projects,
                                                const struct Prj_Project *Prj,
                                                Prj_ProjectView_t ProjectView,
                                                Prj_RoleInProject_t RoleInProject);
static void Prj_ShowTableAllProjectsMembersWithARole (const struct Prj_Project *Prj,
                                                      Prj_RoleInProject_t RoleInProject);

static unsigned Prj_GetNumUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject);
static unsigned Prj_GetUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject,
                                  MYSQL_RES **mysql_res);

static Prj_RoleInProject_t Prj_ConvertUnsignedStrToRoleInProject (const char *UnsignedStr);

static void Prj_FormToSelectStds (void *Projects);
static void Prj_FormToSelectTuts (void *Projects);
static void Prj_FormToSelectEvls (void *Projects);
static void Prj_FormToSelectUsrs (struct Prj_Projects *Projects,
                                  Prj_RoleInProject_t RoleInProject);
static void Prj_AddStds (__attribute__((unused)) void *Args);
static void Prj_AddTuts (__attribute__((unused)) void *Args);
static void Prj_AddEvls (__attribute__((unused)) void *Args);
static void Prj_AddUsrsToProject (Prj_RoleInProject_t RoleInProject);
static void Prj_ReqRemUsrFromPrj (struct Prj_Projects *Projects,
                                  Prj_RoleInProject_t RoleInProject);
static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInProject);

static Prj_Order_t Prj_GetParamPrjOrder (void);

static void Prj_PutFormsToRemEditOnePrj (struct Prj_Projects *Projects,
					 const struct Prj_Project *Prj,
                                         const char *Anchor,
                                         bool ICanViewProjectFiles);

static bool Prj_CheckIfICanEditProject (const struct Prj_Project *Prj);

static void Prj_GetListProjects (struct Prj_Projects *Projects);

static void Prj_ResetProject (struct Prj_Project *Prj);

static void Prj_RequestCreatOrEditPrj (struct Prj_Projects *Projects,
                                       struct Prj_Project *Prj);
static void Prj_PutFormProject (struct Prj_Projects *Projects,
                                struct Prj_Project *Prj,bool ItsANewProject);
static void Prj_EditOneProjectTxtArea (const char *Id,
                                       const char *Label,char *TxtField,
                                       unsigned NumRows,bool Required);

static void Prj_CreateProject (struct Prj_Project *Prj);
static void Prj_UpdateProject (struct Prj_Project *Prj);

static bool Prj_CheckIfICanConfigAllProjects (void);
static void Prj_GetConfigPrjFromDB (struct Prj_Projects *Projects);
static void Prj_GetConfigFromRow (struct Prj_Projects *Projects,MYSQL_ROW row);
static bool Prj_GetEditableFromForm (void);
static void Prj_PutIconsToLockUnlockAllProjects (struct Prj_Projects *Projects);

static void Prj_FormLockUnlock (const struct Prj_Project *Prj);
static void Prj_PutIconOffLockedUnlocked (const struct Prj_Project *Prj);

static void Prj_LockProjectEditionInDB (long PrjCod);
static void Prj_UnlockProjectEditionInDB (long PrjCod);


/*****************************************************************************/
/******* Set/get project code (used to pass parameter to file browser) *******/
/*****************************************************************************/

void Prj_SetPrjCod (long PrjCod)
  {
   Prj_PrjCod = PrjCod;
  }

long Prj_GetPrjCod (void)
  {
   return Prj_PrjCod;
  }

/*****************************************************************************/
/******************************* Reset projects ******************************/
/*****************************************************************************/

static void Prj_ResetProjects (struct Prj_Projects *Projects)
  {
   Projects->Config.Editable = Prj_EDITABLE_DEFAULT;
   Projects->Filter.Who      = Prj_FILTER_WHO_DEFAULT;
   Projects->Filter.Assign   = Prj_FILTER_ASSIGNED_DEFAULT |
	                       Prj_FILTER_NONASSIG_DEFAULT;
   Projects->Filter.Hidden   = Prj_FILTER_HIDDEN_DEFAULT |
	                       Prj_FILTER_VISIBL_DEFAULT;
   Projects->Filter.Faulti   = Prj_FILTER_FAULTY_DEFAULT |
	                       Prj_FILTER_FAULTLESS_DEFAULT;
   Projects->Filter.DptCod   = Prj_FILTER_DPT_DEFAULT;
   Projects->LstIsRead       = false;	// List is not read
   Projects->Num             = 0;
   Projects->LstPrjCods      = NULL;
   Projects->SelectedOrder   = Prj_ORDER_DEFAULT;
   Projects->CurrentPage     = 0;
   Projects->PrjCod          = -1L;
  }

/*****************************************************************************/
/**************************** List users to select ***************************/
/*****************************************************************************/

void Prj_ListUsrsToSelect (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);

   /***** Show list of users to select some of them *****/
   Prj_ReqUsrsToSelect (&Projects);
  }

static void Prj_ReqUsrsToSelect (void *Projects)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   extern const char *Txt_Projects;
   extern const char *Txt_View_projects;

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActSeePrj,
				     Prj_PutCurrentParams,Projects,
				     Txt_Projects,
				     Hlp_ASSESSMENT_Projects,
				     Txt_View_projects,
				     false);	// Do not put form with date range
  }

/*****************************************************************************/
/******************* Get parameters and show projects ************************/
/*****************************************************************************/

void Prj_SeeProjects (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);

   /***** Show projects *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/******************************* Show projects *******************************/
/*****************************************************************************/

static void Prj_ShowProjects (struct Prj_Projects *Projects)
  {
   switch (Projects->Filter.Who)
     {
      case Usr_WHO_ME:
      case Usr_WHO_ALL:
	 /* Show my projects / all projects */
         Prj_ShowPrjsInCurrentPage (Projects);
	 break;
      case Usr_WHO_SELECTED:
	 /* Get selected users and show their projects */
         Prj_GetSelectedUsrsAndShowTheirPrjs (Projects);
         break;
      default:
	 Err_WrongWhoExit ();
	 break;
     }
  }

/*****************************************************************************/
/****** Get and check list of selected users, and show users' projects *******/
/*****************************************************************************/

static void Prj_GetSelectedUsrsAndShowTheirPrjs (struct Prj_Projects *Projects)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  Prj_ShowPrjsInCurrentPage,Projects,	// when user(s) selected
                                  Prj_ReqUsrsToSelect,Projects);		// when no user selected
  }

/*****************************************************************************/
/********************** Show selected projects in a table ********************/
/*****************************************************************************/

void Prj_ShowTableSelectedPrjs (void)
  {
   extern const char *Txt_No_projects;
   struct Prj_Projects Projects;
   unsigned NumPrj;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);

   /***** Get list of projects *****/
   Prj_GetListProjects (&Projects);

   if (Projects.Num)
     {
      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Prj);

      /***** Table head *****/
      HTM_TABLE_BeginWidePadding (2);
      Prj_ShowTableAllProjectsHead ();

      /***** Write all the projects *****/
      for (NumPrj = 0;
	   NumPrj < Projects.Num;
	   NumPrj++)
	{
	 Prj.PrjCod = Projects.LstPrjCods[NumPrj];
	 Prj_ShowTableAllProjectsOneRow (&Prj);
	}

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Free memory of the project *****/
      Prj_FreeMemProject (&Prj);
     }
   else	// No projects created
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /***** Free list of projects *****/
   Prj_FreeListProjects (&Projects);
  }

/*****************************************************************************/
/****************** Show the projects in current page ************************/
/*****************************************************************************/

static void Prj_ShowPrjsInCurrentPage (void *Projects)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   extern const char *Txt_Projects;
   extern const char *Txt_No_projects;
   struct Pagination Pagination;
   unsigned NumPrj;
   unsigned NumIndex;
   struct Prj_Project Prj;

   if (Projects)
     {
      /***** Get list of projects *****/
      Prj_GetListProjects ((struct Prj_Projects *) Projects);

      /***** Compute variables related to pagination *****/
      Pagination.NumItems = ((struct Prj_Projects *) Projects)->Num;
      Pagination.CurrentPage = (int) ((struct Prj_Projects *) Projects)->CurrentPage;
      Pag_CalculatePagination (&Pagination);
      ((struct Prj_Projects *) Projects)->CurrentPage = (unsigned) Pagination.CurrentPage;

      /***** Begin box *****/
      Box_BoxBegin ("100%",Txt_Projects,
		    Prj_PutIconsListProjects,Projects,
		    Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

      /***** Put forms to choice which projects to show *****/
      /* 1st. row */
      Set_BeginSettingsHead ();
      Prj_ShowFormToFilterByMy_All ((struct Prj_Projects *) Projects);
      Prj_ShowFormToFilterByAssign ((struct Prj_Projects *) Projects);
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    Prj_ShowFormToFilterByHidden ((struct Prj_Projects *) Projects);
	    break;
	 default:	// Students will see only visible projects
	    break;
	}
      Prj_ShowFormToFilterByWarning ((struct Prj_Projects *) Projects);
      Set_EndSettingsHead ();

      /* 2nd. row */
      Prj_ShowFormToFilterByDpt ((struct Prj_Projects *) Projects);

      if (((struct Prj_Projects *) Projects)->Num)
	{
	 /***** Write links to pages *****/
	 Pag_WriteLinksToPagesCentered (Pag_PROJECTS,&Pagination,
					(struct Prj_Projects *) Projects,-1L);

	 /***** Allocate memory for the project *****/
	 Prj_AllocMemProject (&Prj);

	 /***** Table head *****/
	 HTM_TABLE_BeginWideMarginPadding (2);
	 Prj_ShowProjectsHead ((struct Prj_Projects *) Projects,Prj_LIST_PROJECTS);

	 /***** Write all the projects *****/
	 for (NumPrj = Pagination.FirstItemVisible;
	      NumPrj <= Pagination.LastItemVisible;
	      NumPrj++)
	   {
	    /* Get project data */
	    Prj.PrjCod = ((struct Prj_Projects *) Projects)->LstPrjCods[NumPrj - 1];
	    Prj_GetDataOfProjectByCod (&Prj);

	    /* Number of index */
	    switch (((struct Prj_Projects *) Projects)->SelectedOrder)
	      {
	       case Prj_ORDER_START_TIME:
	       case Prj_ORDER_END_TIME:
		  // NumPrj: 1, 2, 3 ==> NumIndex = 3, 2, 1
		  NumIndex = ((struct Prj_Projects *) Projects)->Num + 1 - NumPrj;
		  break;
	       default:
		  // NumPrj: 1, 2, 3 ==> NumIndex = 1, 2, 3
		  NumIndex = NumPrj;
		  break;
	      }

	    /* Show project */
	    Prj_ShowOneProject ((struct Prj_Projects *) Projects,
	                        NumIndex,&Prj,Prj_LIST_PROJECTS);
	   }

	 /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Free memory of the project *****/
	 Prj_FreeMemProject (&Prj);

	 /***** Write again links to pages *****/
	 Pag_WriteLinksToPagesCentered (Pag_PROJECTS,&Pagination,
					(struct Prj_Projects *) Projects,-1L);
	}
      else	// No projects created
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /***** Button to create a new project *****/
      if (Prj_CheckIfICanCreateProjects ())
	 Prj_PutButtonToCreateNewPrj ((struct Prj_Projects *) Projects);

      /***** End box *****/
      Box_BoxEnd ();

      /***** Free list of projects *****/
      Prj_FreeListProjects ((struct Prj_Projects *) Projects);
     }
  }

/*****************************************************************************/
/*** Show form to choice whether to show only my projects or all projects ****/
/*****************************************************************************/

static void Prj_ShowFormToFilterByMy_All (const struct Prj_Projects *Projects)
  {
   struct Prj_Filter Filter;
   Usr_Who_t Who;
   unsigned Mask = 1 << Usr_WHO_ME   |
	           1 << Usr_WHO_SELECTED |
		   1 << Usr_WHO_ALL;

   Set_BeginOneSettingSelector ();
   for (Who  = (Usr_Who_t) 1;
	Who <= (Usr_Who_t) (Usr_NUM_WHO - 1);
	Who++)
      if (Mask & (1 << Who))
	{
	 HTM_DIV_Begin ("class=\"%s\"",
			(Projects->Filter.Who == Who) ? "PREF_ON" :
						        "PREF_OFF");
	 Frm_BeginForm (Who == Usr_WHO_SELECTED ? ActReqUsrPrj :
	                                          ActSeePrj);
	 Filter.Who    = Who;
	 Filter.Assign = Projects->Filter.Assign;
	 Filter.Hidden = Projects->Filter.Hidden;
	 Filter.Faulti = Projects->Filter.Faulti;
	 Filter.DptCod = Projects->Filter.DptCod;
	 Prj_PutParams (&Filter,
			Projects->SelectedOrder,
			Projects->CurrentPage,
			-1L);
	 Usr_PutWhoIcon (Who);
	 Frm_EndForm ();
	 HTM_DIV_End ();
	}
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/*********** Show form to select assigned / non-assigned projects ************/
/*****************************************************************************/

static void Prj_ShowFormToFilterByAssign (const struct Prj_Projects *Projects)
  {
   extern const char *Txt_PROJECT_ASSIGNED_NONASSIGNED_PLURAL[Prj_NUM_ASSIGNED_NONASSIG];
   struct Prj_Filter Filter;
   Prj_AssignedNonassig_t Assign;

   Set_BeginOneSettingSelector ();
   for (Assign  = (Prj_AssignedNonassig_t) 0;
	Assign <= (Prj_AssignedNonassig_t) (Prj_NUM_ASSIGNED_NONASSIG - 1);
	Assign++)
     {
      HTM_DIV_Begin ("class=\"%s\"",
		     (Projects->Filter.Assign & (1 << Assign)) ? "PREF_ON" :
								 "PREF_OFF");
      Frm_BeginForm (ActSeePrj);
      Filter.Who    = Projects->Filter.Who;
      Filter.Assign = Projects->Filter.Assign ^ (1 << Assign);	// Toggle
      Filter.Hidden = Projects->Filter.Hidden;
      Filter.Faulti = Projects->Filter.Faulti;
      Filter.DptCod = Projects->Filter.DptCod;
      Prj_PutParams (&Filter,
                     Projects->SelectedOrder,
                     Projects->CurrentPage,
                     -1L);
      Ico_PutSettingIconLink (AssignedNonassigImage[Assign],
	                      Txt_PROJECT_ASSIGNED_NONASSIGNED_PLURAL[Assign]);
      Frm_EndForm ();
      HTM_DIV_End ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************* Show form to select hidden / visible projects *****************/
/*****************************************************************************/

static void Prj_ShowFormToFilterByHidden (const struct Prj_Projects *Projects)
  {
   extern const char *Txt_PROJECT_HIDDEN_VISIBL_PROJECTS[Prj_NUM_HIDDEN_VISIBL];
   struct Prj_Filter Filter;
   Prj_HiddenVisibl_t HidVis;
   static const char *HiddenVisiblIcon[Prj_NUM_HIDDEN_VISIBL] =
     {
      [Prj_HIDDEN] = "eye-slash-red.svg",
      [Prj_VISIBL] = "eye-green.svg",
     };

   Set_BeginOneSettingSelector ();
   for (HidVis  = (Prj_HiddenVisibl_t) 0;
	HidVis <= (Prj_HiddenVisibl_t) (Prj_NUM_HIDDEN_VISIBL - 1);
	HidVis++)
     {
      HTM_DIV_Begin ("class=\"%s\"",
		     (Projects->Filter.Hidden & (1 << HidVis)) ? "PREF_ON" :
								 "PREF_OFF");
      Frm_BeginForm (ActSeePrj);
      Filter.Who    = Projects->Filter.Who;
      Filter.Assign = Projects->Filter.Assign;
      Filter.Hidden = Projects->Filter.Hidden ^ (1 << HidVis);	// Toggle
      Filter.Faulti = Projects->Filter.Faulti;
      Filter.DptCod = Projects->Filter.DptCod;
      Prj_PutParams (&Filter,
                     Projects->SelectedOrder,
                     Projects->CurrentPage,
                     -1L);
      Ico_PutSettingIconLink (HiddenVisiblIcon[HidVis],
	                      Txt_PROJECT_HIDDEN_VISIBL_PROJECTS[HidVis]);
      Frm_EndForm ();
      HTM_DIV_End ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************** Show form to select faulty/faultless projects ****************/
/*****************************************************************************/

static void Prj_ShowFormToFilterByWarning (const struct Prj_Projects *Projects)
  {
   extern const char *Txt_PROJECT_FAULTY_FAULTLESS_PROJECTS[Prj_NUM_FAULTINESS];
   struct Prj_Filter Filter;
   Prj_Faultiness_t Faultiness;
   static const char *FaultinessIcon[Prj_NUM_FAULTINESS] =
     {
      [Prj_FAULTY   ] = "exclamation-triangle.svg",
      [Prj_FAULTLESS] = "check-circle.svg",
     };

   Set_BeginOneSettingSelector ();
   for (Faultiness  = (Prj_Faultiness_t) 0;
	Faultiness <= (Prj_Faultiness_t) (Prj_NUM_FAULTINESS - 1);
	Faultiness++)
     {
      HTM_DIV_Begin ("class=\"%s\"",
		     (Projects->Filter.Faulti & (1 << Faultiness)) ? "PREF_ON" :
								     "PREF_OFF");
      Frm_BeginForm (ActSeePrj);
      Filter.Who    = Projects->Filter.Who;
      Filter.Assign = Projects->Filter.Assign;
      Filter.Hidden = Projects->Filter.Hidden;
      Filter.Faulti = Projects->Filter.Faulti ^ (1 << Faultiness);	// Toggle
      Filter.DptCod = Projects->Filter.DptCod;
      Prj_PutParams (&Filter,
                     Projects->SelectedOrder,
                     Projects->CurrentPage,
                     -1L);
      Ico_PutSettingIconLink (FaultinessIcon[Faultiness],
	                      Txt_PROJECT_FAULTY_FAULTLESS_PROJECTS[Faultiness]);
      Frm_EndForm ();
      HTM_DIV_End ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/*************** Show form to filter projects by department ******************/
/*****************************************************************************/

static void Prj_ShowFormToFilterByDpt (const struct Prj_Projects *Projects)
  {
   extern const char *Txt_Any_department;
   struct Prj_Filter Filter;

   /***** Begin form *****/
   HTM_DIV_Begin (NULL);
   Frm_BeginForm (ActSeePrj);
   Filter.Who    = Projects->Filter.Who;
   Filter.Assign = Projects->Filter.Assign;
   Filter.Hidden = Projects->Filter.Hidden;
   Filter.Faulti = Projects->Filter.Faulti;
   Filter.DptCod = Prj_FILTER_DPT_DEFAULT;	// Do not put department parameter here
   Prj_PutParams (&Filter,
		  Projects->SelectedOrder,
		  Projects->CurrentPage,
		  -1L);

   /***** Write selector with departments *****/
   Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Ins.InsCod,	// Departments in current insitution
                                Projects->Filter.DptCod,	// Selected department
                                "TITLE_DESCRIPTION_WIDTH",	// Selector class
                                -1L,				// First option
                                Txt_Any_department,		// Text when no department selected
                                true);				// Submit on change

   /***** End form *****/
   Frm_EndForm ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/********************** Put parameters used in projects **********************/
/*****************************************************************************/

static void Prj_PutCurrentParams (void *Projects)
  {
   if (Projects)
      Prj_PutParams (&((struct Prj_Projects *) Projects)->Filter,
		     ((struct Prj_Projects *) Projects)->SelectedOrder,
		     ((struct Prj_Projects *) Projects)->CurrentPage,
		     ((struct Prj_Projects *) Projects)->PrjCod);
  }

/* The following function is called
   when one or more parameters must be passed explicitely.
   Each parameter is passed only if its value is distinct to default. */

void Prj_PutParams (struct Prj_Filter *Filter,
                    Prj_Order_t Order,
                    unsigned NumPage,
                    long PrjCod)
  {
   /***** Put filter parameters (which projects to show) *****/
   if (Filter->Who != Prj_FILTER_WHO_DEFAULT)
      Usr_PutHiddenParamWho (Filter->Who);

   if (Filter->Assign != ((unsigned) Prj_FILTER_ASSIGNED_DEFAULT |
	                  (unsigned) Prj_FILTER_NONASSIG_DEFAULT))
      Prj_PutHiddenParamAssign (Filter->Assign);

   if (Filter->Hidden != ((unsigned) Prj_FILTER_HIDDEN_DEFAULT |
	                  (unsigned) Prj_FILTER_VISIBL_DEFAULT))
      Prj_PutHiddenParamHidden (Filter->Hidden);

   if (Filter->Faulti != ((unsigned) Prj_FILTER_FAULTY_DEFAULT |
	                  (unsigned) Prj_FILTER_FAULTLESS_DEFAULT))
      Prj_PutHiddenParamFaulti (Filter->Faulti);

   if (Filter->DptCod != Prj_FILTER_DPT_DEFAULT)
      Prj_PutHiddenParamDptCod (Filter->DptCod);

   /***** Put order field *****/
   if (Order != Prj_ORDER_DEFAULT)
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);

   /***** Put number of page *****/
   if (NumPage > 1)
      Pag_PutHiddenParamPagNum (Pag_PROJECTS,NumPage);

   /***** Put selected project code *****/
   if (PrjCod > 0)
      Prj_PutParamPrjCod (PrjCod);

   /***** Put another user's code *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);

   /***** Put selected users' codes *****/
   if (Filter->Who == Usr_WHO_SELECTED)
      Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/*********************** Put hidden params for projects **********************/
/*****************************************************************************/

static void Prj_PutHiddenParamAssign (unsigned Assign)
  {
   Par_PutHiddenParamUnsigned (NULL,Prj_PARAM_PRE_NON_NAME,Assign);
  }

static void Prj_PutHiddenParamHidden (unsigned Hidden)
  {
   Par_PutHiddenParamUnsigned (NULL,Prj_PARAM_HID_VIS_NAME,Hidden);
  }

static void Prj_PutHiddenParamFaulti (unsigned Faulti)
  {
   Par_PutHiddenParamUnsigned (NULL,Prj_PARAM_FAULTIN_NAME,Faulti);
  }

static void Prj_PutHiddenParamDptCod (long DptCod)
  {
   Par_PutHiddenParamUnsigned (NULL,Dpt_PARAM_DPT_COD_NAME,DptCod);
  }

/*****************************************************************************/
/*********************** Get hidden params for projects **********************/
/*****************************************************************************/

static void Prj_GetHiddenParamPreNon (struct Prj_Projects *Projects)
  {
   Projects->Filter.Assign = (unsigned) Par_GetParToUnsignedLong (Prj_PARAM_PRE_NON_NAME,
                                                                  0,
                                                                  (1 << Prj_ASSIGNED) |
                                                                  (1 << Prj_NONASSIG),
                                                                  (unsigned) Prj_FILTER_ASSIGNED_DEFAULT |
                                                                  (unsigned) Prj_FILTER_NONASSIG_DEFAULT);
  }

static Prj_HiddenVisibl_t Prj_GetHiddenParamHidVis (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:	// Students can view only visible projects
	 return (Prj_HiddenVisibl_t) (1 << Prj_VISIBL);	// Only visible projects
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return (Prj_HiddenVisibl_t)
		Par_GetParToUnsignedLong (Prj_PARAM_HID_VIS_NAME,
					  0,
					  (1 << Prj_HIDDEN) |
					  (1 << Prj_VISIBL),
					  (unsigned) Prj_FILTER_HIDDEN_DEFAULT |
					  (unsigned) Prj_FILTER_VISIBL_DEFAULT);
      default:
	 Err_WrongRoleExit ();
         return Prj_NEW_PRJ_HIDDEN_VISIBL_DEFAULT;	// Not reached
     }
  }

static unsigned Prj_GetHiddenParamFaulti (void)
  {
   return (unsigned)
	  Par_GetParToUnsignedLong (Prj_PARAM_FAULTIN_NAME,
                                    0,
                                    (1 << Prj_FAULTY) |
                                    (1 << Prj_FAULTLESS),
                                    (unsigned) Prj_FILTER_FAULTY_DEFAULT |
                                    (unsigned) Prj_FILTER_FAULTLESS_DEFAULT);
  }

static long Prj_GetHiddenParamDptCod (void)
  {
   return Par_GetParToLong (Dpt_PARAM_DPT_COD_NAME);
  }

/*****************************************************************************/
/***************** Get generic parameters to list projects *******************/
/*****************************************************************************/

static void Prj_GetParams (struct Prj_Projects *Projects)
  {
   /***** Get filter (which projects to show) *****/
   Projects->Filter.Who = Prj_GetParamWho ();
   Prj_GetHiddenParamPreNon (Projects);
   Projects->Filter.Hidden = Prj_GetHiddenParamHidVis ();
   Projects->Filter.Faulti = Prj_GetHiddenParamFaulti ();
   Projects->Filter.DptCod = Prj_GetHiddenParamDptCod ();

   /***** Get order and page *****/
   Projects->SelectedOrder = Prj_GetParamPrjOrder ();
   Projects->CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);

   /***** Get selected users *****/
   if (Projects->Filter.Who == Usr_WHO_SELECTED)
      Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/************* Get parameter with whose users' projects to view **************/
/*****************************************************************************/

static Usr_Who_t Prj_GetParamWho (void)
  {
   Usr_Who_t Who;

   /***** Get which users I want to see *****/
   Who = Usr_GetHiddenParamWho ();

   /***** If parameter Who is unknown, set it to default *****/
   if (Who == Usr_WHO_UNKNOWN)
      Who = Prj_FILTER_WHO_DEFAULT;

   return Who;
  }

/*****************************************************************************/
/******************* Write header with fields of a project *******************/
/*****************************************************************************/

static void Prj_ShowProjectsHead (struct Prj_Projects *Projects,
                                  Prj_ProjectView_t ProjectView)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_PROJECT_ORDER_HELP[Prj_NUM_ORDERS];
   extern const char *Txt_PROJECT_ORDER[Prj_NUM_ORDERS];
   Prj_Order_t Order;

   HTM_TR_Begin (NULL);

   /***** Column for number of project *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TH (1,1,"CM",Txt_No_INDEX);
	 break;
      default:
	 break;
     }

   /***** Column for contextual icons *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
	 HTM_TH (1,1,"CONTEXT_COL",NULL);
	 break;
      default:
	 break;
     }

   /***** Rest of columns *****/
   for (Order  = (Prj_Order_t) 0;
	Order <= (Prj_Order_t) (Prj_NUM_ORDERS - 1);
	Order++)
     {
      HTM_TH_Begin (1,1,"LM");

      switch (ProjectView)
	{
	 case Prj_LIST_PROJECTS:
	 case Prj_FILE_BROWSER_PROJECT:
	    Frm_BeginForm (ActSeePrj);
	    Prj_PutParams (&Projects->Filter,
			   Order,
			   Projects->CurrentPage,
			   -1L);
	    HTM_BUTTON_SUBMIT_Begin (Txt_PROJECT_ORDER_HELP[Order],"BT_LINK TIT_TBL",NULL);
	    if (Order == Projects->SelectedOrder)
	       HTM_U_Begin ();
            HTM_Txt (Txt_PROJECT_ORDER[Order]);
	    if (Order == Projects->SelectedOrder)
	       HTM_U_End ();
	    HTM_BUTTON_End ();
	    Frm_EndForm ();
	    break;
	 default:
            HTM_Txt (Txt_PROJECT_ORDER[Order]);
	    break;
	}

      HTM_TH_End ();
     }
  }

static void Prj_ShowTableAllProjectsHead (void)
  {
   extern const char *Txt_PROJECT_ORDER[Prj_NUM_ORDERS];
   extern const char *Txt_Assigned_QUESTION;
   extern const char *Txt_Number_of_students;
   extern const char *Txt_PROJECT_ROLES_PLURAL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_Proposal;
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   extern const char *Txt_URL;
   Prj_Order_t Order;
   unsigned NumRoleToShow;

   HTM_TR_Begin (NULL);

   for (Order  = (Prj_Order_t) 0;
	Order <= (Prj_Order_t) (Prj_NUM_ORDERS - 1);
	Order++)
      HTM_TH (1,1,"LT DAT_N",Txt_PROJECT_ORDER[Order]);
   HTM_TH (1,1,"LT DAT_N",Txt_Assigned_QUESTION);
   HTM_TH (1,1,"LT DAT_N",Txt_Number_of_students);
   for (NumRoleToShow = 0;
	NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	NumRoleToShow++)
      HTM_TH (1,1,"LT DAT_N",Txt_PROJECT_ROLES_PLURAL_Abc[Prj_RolesToShow[NumRoleToShow]]);
   HTM_TH (1,1,"LT DAT_N",Txt_Proposal);
   HTM_TH (1,1,"LT DAT_N",Txt_Description);
   HTM_TH (1,1,"LT DAT_N",Txt_Required_knowledge);
   HTM_TH (1,1,"LT DAT_N",Txt_Required_materials);
   HTM_TH (1,1,"LT DAT_N",Txt_URL);

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Check if I can create projects ***********************/
/*****************************************************************************/

static bool Prj_CheckIfICanCreateProjects (void)
  {
   static const bool ICanCreateProjects[Rol_NUM_ROLES] =
     {
      [Rol_UNK    ] = false,
      [Rol_GST    ] = false,
      [Rol_USR    ] = false,
      [Rol_STD    ] = false,
      [Rol_NET    ] = true,
      [Rol_TCH    ] = true,
      [Rol_DEG_ADM] = true,
      [Rol_CTR_ADM] = true,
      [Rol_INS_ADM] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanCreateProjects[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/***************** Put contextual icons in list of projects ******************/
/*****************************************************************************/

static void Prj_PutIconsListProjects (void *Projects)
  {
   bool ICanConfigAllProjects;

   if (Projects)
     {
      ICanConfigAllProjects = Prj_CheckIfICanConfigAllProjects ();

      /***** Put icon to create a new project *****/
      if (Prj_CheckIfICanCreateProjects ())
	 Prj_PutIconToCreateNewPrj ((struct Prj_Projects *) Projects);

      if (((struct Prj_Projects *) Projects)->Num)
	{
	 /***** Put icon to show all data in a table *****/
	 Prj_PutIconToShowAllData ((struct Prj_Projects *) Projects);

	 if (ICanConfigAllProjects)
	    /****** Put icons to request locking/unlocking edition
		    of all selected projects *******/
	    Prj_PutIconsToLockUnlockAllProjects ((struct Prj_Projects *) Projects);
	}

      /***** Put form to go to configuration of projects *****/
      if (ICanConfigAllProjects)
	 Ico_PutContextualIconToConfigure (ActCfgPrj,
	                                   NULL,NULL);

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_PROJECTS);
     }
  }

/*****************************************************************************/
/********************* Put icon to create a new project **********************/
/*****************************************************************************/

static void Prj_PutIconToCreateNewPrj (struct Prj_Projects *Projects)
  {
   extern const char *Txt_New_project;

   /***** Put form to create a new project *****/
   Projects->PrjCod = -1L;
   Ico_PutContextualIconToAdd (ActFrmNewPrj,NULL,
                               Prj_PutCurrentParams,Projects,
			       Txt_New_project);
  }

/*****************************************************************************/
/******************** Put button to create a new project *********************/
/*****************************************************************************/

static void Prj_PutButtonToCreateNewPrj (struct Prj_Projects *Projects)
  {
   extern const char *Txt_New_project;

   Projects->PrjCod = -1L;
   Frm_BeginForm (ActFrmNewPrj);
   Prj_PutCurrentParams (Projects);
   Btn_PutConfirmButton (Txt_New_project);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Put button to create a new project *********************/
/*****************************************************************************/

static void Prj_PutIconToShowAllData (struct Prj_Projects *Projects)
  {
   extern const char *Txt_Show_all_data_in_a_table;

   Lay_PutContextualLinkOnlyIcon (ActSeeTblAllPrj,NULL,
                                  Prj_PutCurrentParams,Projects,
			          "table.svg",
				  Txt_Show_all_data_in_a_table);
  }

/*****************************************************************************/
/***************** View / edit file browser of one project *******************/
/*****************************************************************************/

void Prj_ShowOneUniqueProject (struct Prj_Project *Prj)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

   /***** Write project head *****/
   Prj_ShowProjectsHead (&Projects,Prj_FILE_BROWSER_PROJECT);

   /***** Show project *****/
   Prj_ShowOneProject (&Projects,0,Prj,Prj_FILE_BROWSER_PROJECT);

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************** Show print view of one project ***********************/
/*****************************************************************************/

void Prj_PrintOneProject (void)
  {
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get project data *****/
   Prj.PrjCod = Prj_GetParamPrjCod ();
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Write header *****/
   Lay_WriteHeaderClassPhoto (true,false,
			      Gbl.Hierarchy.Ins.InsCod,
			      Gbl.Hierarchy.Deg.DegCod,
			      Gbl.Hierarchy.Crs.CrsCod);

   /***** Table head *****/
   HTM_TABLE_BeginWideMarginPadding (2);
   Prj_ShowProjectsHead (&Projects,Prj_PRINT_ONE_PROJECT);

   /***** Write project *****/
   Prj_ShowOneProject (&Projects,0,&Prj,Prj_PRINT_ONE_PROJECT);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/***************************** Show one project ******************************/
/*****************************************************************************/

static void Prj_ShowOneProject (struct Prj_Projects *Projects,
				unsigned NumIndex,
                                struct Prj_Project *Prj,
                                Prj_ProjectView_t ProjectView)
  {
   extern const char *Txt_Project_files;
   extern const char *Txt_Assigned_QUESTION;
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_PROJECT_ASSIGNED_NONASSIGNED_SINGUL[Prj_NUM_ASSIGNED_NONASSIG];
   extern const char *Txt_Number_of_students;
   extern const char *Txt_See_more;
   extern const char *Txt_See_less;
   extern const char *Txt_Proposal;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   char *Anchor = NULL;
   bool ICanViewProjectFiles = Brw_CheckIfICanViewProjectFiles (Prj->PrjCod);
   const char *ClassLabel;
   const char *ClassDate;
   const char *ClassTitle;
   const char *ClassLink;
   const char *ClassData;
   struct Prj_Faults Faults;
   bool PrjIsFaulty;
   static unsigned UniqueId = 0;
   char *Id;

   /***** Set CSS classes *****/
   ClassLabel = (Prj->Hidden == Prj_HIDDEN) ? "ASG_LABEL_LIGHT" :
					      "ASG_LABEL";
   ClassDate  = (Prj->Hidden == Prj_HIDDEN) ? "DATE_BLUE_LIGHT" :
					      "DATE_BLUE";
   ClassTitle = (Prj->Hidden == Prj_HIDDEN) ? "ASG_TITLE_LIGHT" :
					      "ASG_TITLE";
   ClassLink  = (Prj->Hidden == Prj_HIDDEN) ? "BT_LINK LT ASG_TITLE_LIGHT" :
					      "BT_LINK LT ASG_TITLE";
   ClassData  = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					      "DAT";

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Prj->PrjCod,&Anchor);

   /***** Check if project is faulty or faultless *****/
   PrjIsFaulty = Prj_CheckIfPrjIsFaulty (Prj->PrjCod,&Faults);

   /***** Write first row of data of this project *****/
   HTM_TR_Begin (NULL);

   /* Number of project */
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("rowspan=\"3\" class=\"BIG_INDEX RT COLOR%u\"",
		       Gbl.RowEvenOdd);
	 HTM_Unsigned (NumIndex);

	 if (PrjIsFaulty)
	   {
	    HTM_BR ();
	    Prj_PutWarningIcon ();
	   }

	 HTM_TD_End ();
	 break;
      default:
	 break;
     }

   /* Forms to remove/edit this project */
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         HTM_TD_Begin ("rowspan=\"3\" class=\"CONTEXT_COL COLOR%u\"",Gbl.RowEvenOdd);
         Prj_PutFormsToRemEditOnePrj (Projects,Prj,Anchor,ICanViewProjectFiles);
         HTM_TD_End ();
	 break;
      case Prj_FILE_BROWSER_PROJECT:
         HTM_TD_Begin ("rowspan=\"3\" class=\"CONTEXT_COL\"");
         Prj_PutFormsToRemEditOnePrj (Projects,Prj,Anchor,ICanViewProjectFiles);
         HTM_TD_End ();
	 break;
      default:
	 break;
     }

   /* Creation date/time */
   UniqueId++;
   if (asprintf (&Id,"prj_creat_%u",UniqueId) < 0)
      Err_NotEnoughMemoryExit ();
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		       Id,ClassDate,Gbl.RowEvenOdd);
	 break;
      default:
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT\"",
		       Id,ClassDate);
	 break;
     }
   Dat_WriteLocalDateHMSFromUTC (Id,Prj->CreatTime,
				 Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				 true,true,true,0x7);
   HTM_TD_End ();
   free (Id);

   /* Modification date/time */
   UniqueId++;
   if (asprintf (&Id,"prj_modif_%u",UniqueId) < 0)
      Err_NotEnoughMemoryExit ();
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT COLOR%u\"",
		       Id,ClassDate,Gbl.RowEvenOdd);
	 break;
      default:
	 HTM_TD_Begin ("id=\"%s\" class=\"%s LT\"",
		       Id,ClassDate);
	 break;
     }
   Dat_WriteLocalDateHMSFromUTC (Id,Prj->ModifTime,
				 Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				 true,true,true,0x7);
   HTM_TD_End ();
   free (Id);

   /* Project title */
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("class=\"%s LT COLOR%u\"",ClassTitle,Gbl.RowEvenOdd);
	 break;
      default:
	 HTM_TD_Begin ("class=\"%s LT\"",ClassTitle);
	 break;
     }
   HTM_ARTICLE_Begin (Anchor);
   if (Prj->Title[0])
     {
      if (ICanViewProjectFiles)
	{
	 Frm_BeginForm (ActAdmDocPrj);
	 Prj_PutCurrentParams (Projects);
	 HTM_BUTTON_SUBMIT_Begin (Txt_Project_files,ClassLink,NULL);
	 HTM_Txt (Prj->Title);
	 HTM_BUTTON_End ();
	 Frm_EndForm ();
	}
      else
	 HTM_Txt (Prj->Title);
     }
   if (Faults.WrongTitle)
      Prj_PutWarningIcon ();
   HTM_ARTICLE_End ();
   HTM_TD_End ();

   /* Department */
   Prj_ShowOneProjectDepartment (Prj,ProjectView);

   /***** Assigned? *****/
   HTM_TR_Begin (NULL);

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         HTM_TD_Begin ("colspan=\"2\" class=\"RT %s COLOR%u\"",
		       ClassLabel,Gbl.RowEvenOdd);
	 break;
      default:
         HTM_TD_Begin ("colspan=\"2\" class=\"RT %s\"",
		       ClassLabel);
         break;
     }
   HTM_TxtColon (Txt_Assigned_QUESTION);
   HTM_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         HTM_TD_Begin ("colspan=\"2\" class=\"LT %s COLOR%u\"",
		       ClassData,Gbl.RowEvenOdd);
         break;
      default:
         HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
		       ClassData);
         break;
     }
   HTM_TxtF ("%s&nbsp;",Prj->Assigned == Prj_ASSIGNED ? Txt_Yes :
        	                                        Txt_No);
   Ico_PutIconOff (AssignedNonassigImage[Prj->Assigned],
		   Txt_PROJECT_ASSIGNED_NONASSIGNED_SINGUL[Prj->Assigned]);

   if (Faults.WrongAssigned)
      Prj_PutWarningIcon ();

   HTM_TD_End ();

   HTM_TR_End ();

   /***** Number of students *****/
   HTM_TR_Begin (NULL);

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         HTM_TD_Begin ("colspan=\"2\" class=\"RT %s COLOR%u\"",
		       ClassLabel,Gbl.RowEvenOdd);
         break;
      default:
         HTM_TD_Begin ("colspan=\"2\" class=\"RT %s\"",
		       ClassLabel);
         break;
     }
   HTM_TxtColon (Txt_Number_of_students);
   HTM_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         HTM_TD_Begin ("colspan=\"2\" class=\"LT %s COLOR%u\"",
		       ClassData,Gbl.RowEvenOdd);
         break;
      default:
         HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
		       ClassData);
         break;
     }
   HTM_Unsigned (Prj->NumStds);
   if (Faults.WrongNumStds)
      Prj_PutWarningIcon ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Project members *****/
   Prj_ShowOneProjectMembers (Projects,Prj,ProjectView);

   /***** Link to show hidden info *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"prj_exp_%u\"",UniqueId);
	 HTM_TD_Begin ("colspan=\"6\" class=\"CM COLOR%u\"",Gbl.RowEvenOdd);
	 Prj_PutIconToToggleProject (UniqueId,"angle-down.svg",Txt_See_more);
	 HTM_TD_End ();
	 HTM_TR_End ();

	 HTM_TR_Begin ("id=\"prj_con_%u\" style=\"display:none;\"",UniqueId);
	 HTM_TD_Begin ("colspan=\"6\" class=\"CM COLOR%u\"",Gbl.RowEvenOdd);
	 Prj_PutIconToToggleProject (UniqueId,"angle-up.svg",Txt_See_less);
	 HTM_TD_End ();
	 HTM_TR_End ();
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 HTM_TR_Begin ("id=\"prj_exp_%u\"",UniqueId);
	 HTM_TD_Begin ("colspan=\"5\" class=\"CM\"");
	 Prj_PutIconToToggleProject (UniqueId,"angle-down.svg",Txt_See_more);
	 HTM_TD_End ();
	 HTM_TR_End ();

	 HTM_TR_Begin ("id=\"prj_con_%u\" style=\"display:none;\"",UniqueId);
	 HTM_TD_Begin ("colspan=\"5\" class=\"CM\"");
	 Prj_PutIconToToggleProject (UniqueId,"angle-up.svg",Txt_See_less);
	 HTM_TD_End ();
	 HTM_TR_End ();
	 break;
      default:
	 break;
     }

   /***** Proposal *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"prj_pro_%u\" style=\"display:none;\"",UniqueId);
	 HTM_TD_Begin ("colspan=\"4\" class=\"RT %s COLOR%u\"",
		       ClassLabel,Gbl.RowEvenOdd);
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 HTM_TR_Begin ("id=\"prj_pro_%u\" style=\"display:none;\"",UniqueId);
	 HTM_TD_Begin ("colspan=\"3\" class=\"RT %s\"",ClassLabel);
	 break;
      default:
	 HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"2\" class=\"RT %s\"",ClassLabel);
	 break;
     }
   HTM_TxtColon (Txt_Proposal);
   HTM_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s COLOR%u\"",
		       ClassData,
		       Gbl.RowEvenOdd);
	 break;
      default:
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
		       ClassData);
	 break;
     }
   HTM_Txt (Txt_PROJECT_STATUS[Prj->Proposal]);
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Write rows of data of this project *****/
   /* Description of the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,"prj_dsc_",UniqueId,
                               Txt_Description,Prj->Description,
			       Faults.WrongDescription);

   /* Required knowledge to carry out the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,"prj_knw_",UniqueId,
                               Txt_Required_knowledge,Prj->Knowledge,
			       false);	// No warning

   /* Required materials to carry out the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,"prj_mtr_",UniqueId,
                               Txt_Required_materials,Prj->Materials,
			       false);	// No warning

   /* Link to view more info about the project */
   Prj_ShowOneProjectURL (Prj,ProjectView,"prj_url_",UniqueId);

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********************** Check if a project has faults ************************/
/*****************************************************************************/

static bool Prj_CheckIfPrjIsFaulty (long PrjCod,struct Prj_Faults *Faults)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool IsAssigned;
   bool HasTitle;
   bool HasDescription;
   unsigned NumProposedStds;
   unsigned NumStdsRegisteredInPrj;

   /***** Reset faults *****/
   Faults->WrongTitle       =
   Faults->WrongDescription =
   Faults->WrongNumStds     =
   Faults->WrongAssigned    = false;

   /***** Get some project date and check faults ****/
   if (PrjCod > 0)
     {
      /***** Query database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get project data",
			  "SELECT Assigned='Y',"		// row[0] = 0 / 1
				 "NumStds,"			// row[1] =
				 "Title<>'',"			// row[2] = 0 / 1
				 "Description<>''"		// row[3] = 0 / 1
			   " FROM prj_projects"
			  " WHERE PrjCod=%ld",
			  PrjCod))	// Project found...
	{
         /***** Get some data of project *****/
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get if project is assigned or not (row[0]) */
	 IsAssigned = (row[0][0] != '0');

	 /* Get if project is assigned or not (row[1]) */
	 NumProposedStds = Str_ConvertStrToUnsigned (row[1]);

	 /* Get the title of the project (row[2]) */
	 HasTitle = (row[2][0] != '0');

	 /* Get the description of the project (row[3]) */
	 HasDescription = (row[3][0] != '0');

	 /***** Check faults *****/
	 /* 1. Check title */
	 Faults->WrongTitle       = !HasTitle;

	 /* 2. Check description */
	 Faults->WrongDescription = !HasDescription;

	 /* 3. Check number of students */
	 if (NumProposedStds == 0)
	    // The number of proposed students should be > 0
	    Faults->WrongNumStds = true;
	 else
	   {
	    NumStdsRegisteredInPrj = Prj_GetNumUsrsInPrj (PrjCod,Prj_ROLE_STD);
	    if (IsAssigned)		// Assigned
	       // In an assigned project the number of proposed students...
	       // ...should match the number of students registered in it
	       Faults->WrongNumStds = (NumProposedStds != NumStdsRegisteredInPrj);
	    else			// Not assigned
	       // A non assigned project should not have students registered in it
	       Faults->WrongAssigned = (NumStdsRegisteredInPrj != 0);
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return Faults->WrongTitle       ||
	  Faults->WrongDescription ||
	  Faults->WrongNumStds     ||
	  Faults->WrongAssigned;
  }

/*****************************************************************************/
/************** Put an icon to warn about a fault in a project ***************/
/*****************************************************************************/

static void Prj_PutWarningIcon (void)
  {
   Ico_PutIcon ("warning64x64.gif","","ICO16x16");
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off some fields of a project ************/
/*****************************************************************************/

static void Prj_PutIconToToggleProject (unsigned UniqueId,
                                        const char *Icon,const char *Text)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];

   /***** Link to toggle on/off some fields of project *****/
   HTM_A_Begin ("href=\"\" title=\"%s\" class=\"%s\""
                " onclick=\"toggleProject('%u');return false;\"",
               Text,The_ClassFormInBox[Gbl.Prefs.Theme],
               UniqueId);
   Ico_PutIconTextLink (Icon,Text);
   HTM_A_End ();
  }

/*****************************************************************************/
/***************** Show one row of table with all projects *******************/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsOneRow (struct Prj_Project *Prj)
  {
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];
   unsigned NumRoleToShow;
   const char *ClassDate;
   const char *ClassData;
   static unsigned UniqueId = 0;
   char *Id;

   /***** Get data of this project *****/
   Prj_GetDataOfProjectByCod (Prj);

   /***** Set CSS classes *****/
   ClassDate = (Prj->Hidden == Prj_HIDDEN) ? "DATE_BLUE_LIGHT" :
					     "DATE_BLUE";
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Start date/time *****/
      UniqueId++;
      if (asprintf (&Id,"prj_creat_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LT %s COLOR%u\"",
		    Id,ClassDate,Gbl.RowEvenOdd);
	 Dat_WriteLocalDateHMSFromUTC (Id,Prj->CreatTime,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,false,0x7);
      HTM_TD_End ();
      free (Id);

      /***** End date/time *****/
      UniqueId++;
      if (asprintf (&Id,"prj_modif_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LT %s COLOR%u\"",
		    Id,ClassDate,Gbl.RowEvenOdd);
	 Dat_WriteLocalDateHMSFromUTC (Id,Prj->ModifTime,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,false,0x7);
      HTM_TD_End ();
      free (Id);

      /***** Project title *****/
      HTM_TD_Begin ("class=\"LT %s COLOR%u\"",
		    ClassData,
		    Gbl.RowEvenOdd);
	 HTM_Txt (Prj->Title);
      HTM_TD_End ();

      /***** Department *****/
      Prj_ShowTableAllProjectsDepartment (Prj);

      /***** Assigned? *****/
      HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);
	 HTM_Txt ((Prj->Assigned == Prj_ASSIGNED) ? Txt_Yes :
						    Txt_No);
      HTM_TD_End ();

      /***** Number of students *****/
      HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);
	 HTM_Unsigned (Prj->NumStds);
      HTM_TD_End ();

      /***** Project members *****/
      for (NumRoleToShow = 0;
	   NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	   NumRoleToShow++)
	 Prj_ShowTableAllProjectsMembersWithARole (Prj,Prj_RolesToShow[NumRoleToShow]);

      /***** Proposal *****/
      HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);
	 HTM_Txt (Txt_PROJECT_STATUS[Prj->Proposal]);
      HTM_TD_End ();

      /***** Write rows of data of this project *****/
      /* Description of the project */
      Prj_ShowTableAllProjectsTxtField (Prj,Prj->Description);

      /* Required knowledge to carry out the project */
      Prj_ShowTableAllProjectsTxtField (Prj,Prj->Knowledge);

      /* Required materials to carry out the project */
      Prj_ShowTableAllProjectsTxtField (Prj,Prj->Materials);

      /* Link to view more info about the project */
      Prj_ShowTableAllProjectsURL (Prj);

   /***** End row *****/
   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/****************** Show department associated to project ********************/
/*****************************************************************************/

static void Prj_ShowOneProjectDepartment (const struct Prj_Project *Prj,
                                          Prj_ProjectView_t ProjectView)
  {
   struct Dpt_Department Dpt;
   bool PutLink;
   const char *ClassData;

   /***** Set CSS classes *****/
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Get data of department *****/
   Dpt.DptCod = Prj->DptCod;
   Dpt_GetDataOfDepartmentByCod (&Dpt);

   /***** Show department *****/
   PutLink = (Dpt.WWW[0] &&
	      (ProjectView == Prj_LIST_PROJECTS ||
	       ProjectView == Prj_FILE_BROWSER_PROJECT));

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);
	 break;
      default:
	 HTM_TD_Begin ("class=\"LT %s\"",ClassData);
	 break;
     }
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"%s\"",
	           Dpt.WWW,ClassData);
   HTM_Txt (Dpt.FullName);
   if (PutLink)
      HTM_A_End ();
   HTM_TD_End ();
   HTM_TR_End ();
  }

static void Prj_ShowTableAllProjectsDepartment (const struct Prj_Project *Prj)
  {
   struct Dpt_Department Dpt;
   const char *ClassData;

   /***** Set CSS classes *****/
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Get data of department *****/
   Dpt.DptCod = Prj->DptCod;
   Dpt_GetDataOfDepartmentByCod (&Dpt);

   /***** Show department *****/
   HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);
   HTM_Txt (Dpt.FullName);
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************** Show text field about a project **********************/
/*****************************************************************************/

static void Prj_ShowOneProjectTxtField (struct Prj_Project *Prj,
                                        Prj_ProjectView_t ProjectView,
                                        const char *id,unsigned UniqueId,
                                        const char *Label,char *TxtField,
					bool Warning)
  {
   const char *ClassLabel;
   const char *ClassData;

   /***** Set CSS classes *****/
   ClassLabel = (Prj->Hidden == Prj_HIDDEN) ? "ASG_LABEL_LIGHT" :
					      "ASG_LABEL";
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Label *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 HTM_TD_Begin ("colspan=\"4\" class=\"RT %s COLOR%u\"",ClassLabel,Gbl.RowEvenOdd);
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 HTM_TD_Begin ("colspan=\"3\" class=\"RT %s\"",ClassLabel);
	 break;
      case Prj_PRINT_ONE_PROJECT:
	 HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"2\" class=\"RT %s\"",ClassLabel);
	 break;
      default:
	 // Not applicable
	 break;
     }
   HTM_TxtColon (Label);
   HTM_TD_End ();

   /***** Change text format *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     TxtField,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
         Str_InsertLinks (TxtField,Cns_MAX_BYTES_TEXT,60);	// Insert links
	 break;
      default:
	 break;
     }

   /***** Text *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s COLOR%u\"",
		       ClassData,Gbl.RowEvenOdd);
	 break;
      default:
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
		       ClassData);
	 break;
     }
   HTM_Txt (TxtField);
   if (Warning)
      Prj_PutWarningIcon ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

static void Prj_ShowTableAllProjectsTxtField (struct Prj_Project *Prj,
                                              char *TxtField)
  {
   const char *ClassData;

   /***** Set CSS classes *****/
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Change format *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     TxtField,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML

   /***** Write text *****/
   HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);
   HTM_Txt (TxtField);
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************** Show URL associated to project ***********************/
/*****************************************************************************/

static void Prj_ShowOneProjectURL (const struct Prj_Project *Prj,
                                   Prj_ProjectView_t ProjectView,
                                   const char *id,unsigned UniqueId)
  {
   extern const char *Txt_URL;
   const char *ClassLabel;
   const char *ClassData;
   bool PutLink = (Prj->URL[0] &&
	           (ProjectView == Prj_LIST_PROJECTS ||
	            ProjectView == Prj_FILE_BROWSER_PROJECT));

   /***** Set CSS classes *****/
   ClassLabel = (Prj->Hidden == Prj_HIDDEN) ? "ASG_LABEL_LIGHT" :
					      "ASG_LABEL";
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Write row with label and text *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 HTM_TD_Begin ("colspan=\"4\" class=\"RT %s COLOR%u\"",ClassLabel,Gbl.RowEvenOdd);
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 HTM_TD_Begin ("colspan=\"3\" class=\"RT %s\"",ClassLabel);
	 break;
      case Prj_PRINT_ONE_PROJECT:
	 HTM_TR_Begin (NULL);
	 HTM_TD_Begin ("colspan=\"2\" class=\"RT %s\"",ClassLabel);
	 break;
      default:
	 // Not applicable
	 break;
     }
   HTM_TxtColon (Txt_URL);
   HTM_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s COLOR%u\"",
		       ClassData,Gbl.RowEvenOdd);
	 break;
      default:
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
		       ClassData);
	 break;
     }
   if (PutLink)
      HTM_A_Begin ("href=\"%s\" target=\"_blank\"",Prj->URL);
   HTM_Txt (Prj->URL);
   if (PutLink)
      HTM_A_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

static void Prj_ShowTableAllProjectsURL (const struct Prj_Project *Prj)
  {
   const char *ClassData;

   /***** Set CSS classes *****/
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Show URL *****/
   HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);
   HTM_Txt (Prj->URL);
   HTM_TD_End ();
  }

/*****************************************************************************/
/************** Show projects members when showing one project ***************/
/*****************************************************************************/

static void Prj_ShowOneProjectMembers (struct Prj_Projects *Projects,
                                       struct Prj_Project *Prj,
                                       Prj_ProjectView_t ProjectView)
  {
   unsigned NumRoleToShow;

   for (NumRoleToShow = 0;
	NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	NumRoleToShow++)
      Prj_ShowOneProjectMembersWithARole (Projects,
                                          Prj,ProjectView,
                                          Prj_RolesToShow[NumRoleToShow]);
  }

/*****************************************************************************/
/************************* Show users row in a project ***********************/
/*****************************************************************************/

static void Prj_ShowOneProjectMembersWithARole (struct Prj_Projects *Projects,
                                                const struct Prj_Project *Prj,
                                                Prj_ProjectView_t ProjectView,
                                                Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_PROJECT_ROLES_SINGUL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_PROJECT_ROLES_PLURAL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_Remove;
   extern const char *Txt_Add_USERS;
   extern const char *Txt_PROJECT_ROLES_PLURAL_abc[Prj_NUM_ROLES_IN_PROJECT];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool WriteRow;
   unsigned NumUsr;
   unsigned NumUsrs;
   const char *ClassLabel;
   const char *ClassData;
   static const Act_Action_t ActionReqRemUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      [Prj_ROLE_UNK] = ActUnk,		// Unknown
      [Prj_ROLE_STD] = ActReqRemStdPrj,	// Student
      [Prj_ROLE_TUT] = ActReqRemTutPrj,	// Tutor
      [Prj_ROLE_EVL] = ActReqRemEvlPrj,	// Evaluator
     };
   static const Act_Action_t ActionReqAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      [Prj_ROLE_UNK] = ActUnk,		// Unknown
      [Prj_ROLE_STD] = ActReqAddStdPrj,	// Student
      [Prj_ROLE_TUT] = ActReqAddTutPrj,	// Tutor
      [Prj_ROLE_EVL] = ActReqAddEvlPrj,	// Evaluator
     };

   /***** Set CSS classes *****/
   ClassLabel = (Prj->Hidden == Prj_HIDDEN) ? "ASG_LABEL_LIGHT" :
					      "ASG_LABEL";
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Get users in project from database *****/
   NumUsrs = Prj_GetUsrsInPrj (Prj->PrjCod,RoleInProject,&mysql_res);
   WriteRow = (NumUsrs != 0 ||
	       ProjectView == Prj_EDIT_ONE_PROJECT);

   if (WriteRow)
     {
      /***** Begin row with label and listing of users *****/
      HTM_TR_Begin (NULL);

	 /* Column for label */
	 switch (ProjectView)
	   {
	    case Prj_LIST_PROJECTS:
	       HTM_TD_Begin ("colspan=\"4\" class=\"RT %s COLOR%u\"",
			     ClassLabel,Gbl.RowEvenOdd);
		  HTM_TxtColon (NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInProject] :
					       Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	       break;
	    case Prj_FILE_BROWSER_PROJECT:
	       HTM_TD_Begin ("colspan=\"3\" class=\"RT %s\"",ClassLabel);
		  HTM_TxtColon (NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInProject] :
					       Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	       break;
	    case Prj_PRINT_ONE_PROJECT:
	       HTM_TD_Begin ("colspan=\"2\" class=\"RT %s\"",ClassLabel);
		  HTM_TxtColon (NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInProject] :
					       Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	       break;
	    case Prj_EDIT_ONE_PROJECT:
	       HTM_TD_Begin ("class=\"RT ASG_LABEL\"");
		  HTM_TxtColon (Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	       break;
	   }
	 HTM_TD_End ();

	 /* Begin column with list of users */
	 switch (ProjectView)
	   {
	    case Prj_LIST_PROJECTS:
	       HTM_TD_Begin ("colspan=\"2\" class=\"LT %s COLOR%u\"",
			     ClassData,Gbl.RowEvenOdd);
	       break;
	    case Prj_FILE_BROWSER_PROJECT:
	    case Prj_PRINT_ONE_PROJECT:
	       HTM_TD_Begin ("colspan=\"2\" class=\"LT %s\"",
			     ClassData);
	       break;
	    case Prj_EDIT_ONE_PROJECT:
	       HTM_TD_Begin ("colspan=\"2\" class=\"LT DAT\"");
	       break;
	   }

	 /***** Begin table with all members with this role *****/
	 HTM_TABLE_BeginPadding (2);

	    /***** Write users *****/
	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /* Get user's code */
	       row = mysql_fetch_row (mysql_res);
	       Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	       /* Get user's data */
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
		 {
		  /* Begin row for this user */
		  HTM_TR_Begin (NULL);

		     /* Icon to remove user */
		     if (ProjectView == Prj_EDIT_ONE_PROJECT)
		       {
			HTM_TD_Begin ("class=\"PRJ_MEMBER_ICO\"");
			   Lay_PutContextualLinkOnlyIcon (ActionReqRemUsr[RoleInProject],NULL,
							  Prj_PutCurrentParams,Projects,
							  "trash.svg",
							  Txt_Remove);
			HTM_TD_End ();
		       }

		     /* Put user's photo */
		     HTM_TD_Begin ("class=\"PRJ_MEMBER_PHO\"");
			Pho_ShowUsrPhotoIfAllowed (&Gbl.Usrs.Other.UsrDat,"PHOTO21x28",Pho_ZOOM,false);
		     HTM_TD_End ();

		     /* Write user's name */
		     HTM_TD_Begin ("class=\"PRJ_MEMBER_NAM\"");
			HTM_Txt (Gbl.Usrs.Other.UsrDat.FullName);
		     HTM_TD_End ();

		  /* End row for this user */
		  HTM_TR_End ();
		 }
	      }

	    /***** Row to add a new user *****/
	    switch (ProjectView)
	      {
	       case Prj_EDIT_ONE_PROJECT:
		  HTM_TR_Begin (NULL);
		  HTM_TD_Begin ("class=\"PRJ_MEMBER_ICO\"");
		  Projects->PrjCod = Prj->PrjCod;	// Used to pass project code as a parameter
		  Ico_PutContextualIconToAdd (ActionReqAddUsr[RoleInProject],NULL,
					      Prj_PutCurrentParams,Projects,
					      Str_BuildStringStr (Txt_Add_USERS,
								  Txt_PROJECT_ROLES_PLURAL_abc[RoleInProject]));
		  Str_FreeString ();
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"PRJ_MEMBER_PHO\"");	// Column for photo
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"PRJ_MEMBER_NAM\"");	// Column for name
		  HTM_TD_End ();

		  HTM_TR_End ();
		  break;
	       default:
		  break;
	      }

	 /***** End table with all members with this role *****/
	 HTM_TABLE_End ();

	 /***** End row with label and listing of users *****/
	 HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void Prj_ShowTableAllProjectsMembersWithARole (const struct Prj_Project *Prj,
                                                      Prj_RoleInProject_t RoleInProject)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned NumUsrs;
   const char *ClassData;

   /***** Set CSS classes *****/
   ClassData = (Prj->Hidden == Prj_HIDDEN) ? "DAT_LIGHT" :
					     "DAT";

   /***** Get users in project from database *****/
   NumUsrs = Prj_GetUsrsInPrj (Prj->PrjCod,RoleInProject,&mysql_res);

   /***** Begin column with list of all members with this role *****/
   HTM_TD_Begin ("class=\"LT %s COLOR%u\"",ClassData,Gbl.RowEvenOdd);

      if (NumUsrs)
	{
	 /***** Write users *****/
	 HTM_UL_Begin ("class=\"PRJ_LST_USR\"");

	    for (NumUsr = 0;
		 NumUsr < NumUsrs;
		 NumUsr++)
	      {
	       /* Get user's code */
	       row = mysql_fetch_row (mysql_res);
	       Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	       /* Get user's data */
	       if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
							    Usr_DONT_GET_PREFS,
							    Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
		 {
		  /* Write user's name in "Surname1 Surname2, FirstName" format */
		  HTM_LI_Begin (NULL);
		     HTM_Txt (Gbl.Usrs.Other.UsrDat.Surname1);
		     if (Gbl.Usrs.Other.UsrDat.Surname2[0])
			HTM_TxtF ("&nbsp;%s",Gbl.Usrs.Other.UsrDat.Surname2);
		     HTM_TxtF (", %s",Gbl.Usrs.Other.UsrDat.FrstName);
		  HTM_LI_End ();
		 }
	      }

	 HTM_UL_End ();
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);

   /***** End column with list of all members with this role *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/*************** Get number of users with a role in a project ****************/
/*****************************************************************************/

static unsigned Prj_GetNumUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject)
  {
   /***** Get users in project from database *****/
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users in project",
		  "SELECT COUNT(UsrCod)"
		   " FROM prj_users"
		  " WHERE PrjCod=%ld"
		    " AND RoleInProject=%u",
		  PrjCod,
		  (unsigned) RoleInProject);
  }

/*****************************************************************************/
/*************** Get number of users with a role in a project ****************/
/*****************************************************************************/

static unsigned Prj_GetUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject,
                                  MYSQL_RES **mysql_res)
  {
   /***** Get users in project from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users in project",
		   "SELECT prj_users.UsrCod,"		// row[0]
			  "usr_data.Surname1 AS S1,"	// row[1]
			  "usr_data.Surname2 AS S2,"	// row[2]
			  "usr_data.FirstName AS FN"	// row[3]
		    " FROM prj_users,"
			  "usr_data"
		   " WHERE prj_users.PrjCod=%ld"
		     " AND prj_users.RoleInProject=%u"
		     " AND prj_users.UsrCod=usr_data.UsrCod"
		   " ORDER BY S1,"
			     "S2,"
			     "FN",
		   PrjCod,(unsigned) RoleInProject);
  }

/*****************************************************************************/
/************************** Get my role in a project *************************/
/*****************************************************************************/

void Prj_FlushCacheMyRolesInProject (void)
  {
   Gbl.Cache.MyRolesInProject.PrjCod         = -1L;
   Gbl.Cache.MyRolesInProject.RolesInProject = 0;
  }

unsigned Prj_GetMyRolesInProject (long PrjCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   unsigned NumRow;
   Prj_RoleInProject_t RoleInProject;

   /***** 1. Fast check: trivial cases *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod <= 0 ||
       PrjCod <= 0)
      return 0;

   /***** 2. Fast check: Is my role in project already calculated *****/
   if (PrjCod == Gbl.Cache.MyRolesInProject.PrjCod)
      return Gbl.Cache.MyRolesInProject.RolesInProject;

   /***** 3. Slow check: Get my role in project from database.
			 The result of the query will have one row or none *****/
   Gbl.Cache.MyRolesInProject.PrjCod         = PrjCod;
   Gbl.Cache.MyRolesInProject.RolesInProject = 0;
   NumRows = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get my roles in project",
		   "SELECT RoleInProject"	// row[0]
		    " FROM prj_users"
		   " WHERE PrjCod=%ld"
		     " AND UsrCod=%ld",
		   PrjCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);
      RoleInProject = Prj_ConvertUnsignedStrToRoleInProject (row[0]);
      if (RoleInProject != Prj_ROLE_UNK)
	 Gbl.Cache.MyRolesInProject.RolesInProject |= (1 << RoleInProject);
     }
   DB_FreeMySQLResult (&mysql_res);

   return Gbl.Cache.MyRolesInProject.RolesInProject;
  }

/*****************************************************************************/
/********************** Get role from unsigned string ************************/
/*****************************************************************************/

static Prj_RoleInProject_t Prj_ConvertUnsignedStrToRoleInProject (const char *UnsignedStr)
  {
   unsigned UnsignedNum;

   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Prj_NUM_ROLES_IN_PROJECT)
         return (Prj_RoleInProject_t) UnsignedNum;

   return Prj_ROLE_UNK;
  }

/*****************************************************************************/
/******************* Request users to be added to project ********************/
/*****************************************************************************/

void Prj_ReqAddStds (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   Prj_FormToSelectStds (&Projects);
  }

void Prj_ReqAddTuts (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   Prj_FormToSelectTuts (&Projects);
  }

void Prj_ReqAddEvls (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   Prj_FormToSelectEvls (&Projects);
  }

static void Prj_FormToSelectStds (void *Projects)
  {
   if (Projects)
      Prj_FormToSelectUsrs ((struct Prj_Projects *) Projects,Prj_ROLE_STD);
  }

static void Prj_FormToSelectTuts (void *Projects)
  {
   if (Projects)
      Prj_FormToSelectUsrs ((struct Prj_Projects *) Projects,Prj_ROLE_TUT);
  }

static void Prj_FormToSelectEvls (void *Projects)
  {
   if (Projects)
      Prj_FormToSelectUsrs ((struct Prj_Projects *) Projects,Prj_ROLE_EVL);
  }

static void Prj_FormToSelectUsrs (struct Prj_Projects *Projects,
                                  Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Hlp_ASSESSMENT_Projects_add_user;
   extern const char *Txt_Add_USERS;
   extern const char *Txt_PROJECT_ROLES_PLURAL_abc[Prj_NUM_ROLES_IN_PROJECT];
   static Act_Action_t ActionAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      [Prj_ROLE_UNK] = ActUnk,		// Unknown
      [Prj_ROLE_STD] = ActAddStdPrj,	// Student
      [Prj_ROLE_TUT] = ActAddTutPrj,	// Tutor
      [Prj_ROLE_EVL] = ActAddEvlPrj,	// Evaluator
     };
   struct Prj_Project Prj;
   char *TxtButton;

   /***** Get parameters *****/
   Prj_GetParams (Projects);
   if ((Projects->PrjCod = Prj.PrjCod = Prj_GetParamPrjCod ()) <= 0)
      Err_WrongProjectExit ();

   /***** Put form to select users *****/
   if (asprintf (&TxtButton,Txt_Add_USERS,
	         Txt_PROJECT_ROLES_PLURAL_abc[RoleInProject]) < 0)
      Err_NotEnoughMemoryExit ();
   Usr_PutFormToSelectUsrsToGoToAct (&Prj_MembersToAdd,
				     ActionAddUsr[RoleInProject],
				     Prj_PutCurrentParams,Projects,
				     TxtButton,
                                     Hlp_ASSESSMENT_Projects_add_user,
                                     TxtButton,
				     false);	// Do not put form with date range
   free (TxtButton);

   /***** Put a form to create/edit project *****/
   Prj_RequestCreatOrEditPrj (Projects,&Prj);
  }

/*****************************************************************************/
/******* Get and check list of selected users, and show users' works  ********/
/*****************************************************************************/

void Prj_GetSelectedUsrsAndAddStds (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Prj_MembersToAdd,
				  Prj_AddStds,NULL,		// when user(s) selected
                                  Prj_FormToSelectStds,NULL);	// when no user selected
  }

void Prj_GetSelectedUsrsAndAddTuts (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Prj_MembersToAdd,
				  Prj_AddTuts,NULL,		// when user(s) selected
                                  Prj_FormToSelectTuts,NULL);	// when no user selected
  }

void Prj_GetSelectedUsrsAndAddEvls (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Prj_MembersToAdd,
				  Prj_AddEvls,NULL,		// when user(s) selected
                                  Prj_FormToSelectEvls,NULL);	// when no user selected
  }

/*****************************************************************************/
/**************************** Add users to project ***************************/
/*****************************************************************************/

static void Prj_AddStds (__attribute__((unused)) void *Args)
  {
   Prj_AddUsrsToProject (Prj_ROLE_STD);
  }

static void Prj_AddTuts (__attribute__((unused)) void *Args)
  {
   Prj_AddUsrsToProject (Prj_ROLE_TUT);
  }

static void Prj_AddEvls (__attribute__((unused)) void *Args)
  {
   Prj_AddUsrsToProject (Prj_ROLE_EVL);
  }

static void Prj_AddUsrsToProject (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   struct Prj_Projects Projects;
   struct Prj_Project Prj;
   const char *Ptr;
   bool ItsMe;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Projects.PrjCod = Prj.PrjCod = Prj_GetParamPrjCod ()) <= 0)
      Err_WrongProjectExit ();

   /***** Add the selected users to project *****/
   Ptr = Prj_MembersToAdd.List[Rol_UNK];
   while (*Ptr)
     {
      /* Get next user */
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);

      /* Get user's data */
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
        {
	 /* Add user to project */
	 DB_QueryREPLACE ("can not add user to project",
			  "REPLACE INTO prj_users"
			  " (PrjCod,RoleInProject,UsrCod)"
			  " VALUES"
			  " (%ld,%u,%ld)",
			  Projects.PrjCod,
			  (unsigned) RoleInProject,
			  Gbl.Usrs.Other.UsrDat.UsrCod);

	 /* Flush cache */
	 ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
	 if (ItsMe)
	    Prj_FlushCacheMyRolesInProject ();

	 /* Show success alert */
	 Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project,
			Gbl.Usrs.Other.UsrDat.FullName,
			Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject][Gbl.Usrs.Other.UsrDat.Sex]);
        }
     }

   /***** Free memory used by lists of selected encrypted users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Prj_MembersToAdd);

   /***** Put form to edit project again *****/
   Prj_RequestCreatOrEditPrj (&Projects,&Prj);
  }

/*****************************************************************************/
/************ Request confirmation to remove user from project ***************/
/*****************************************************************************/

void Prj_ReqRemStd (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   Prj_ReqRemUsrFromPrj (&Projects,Prj_ROLE_STD);
  }

void Prj_ReqRemTut (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   Prj_ReqRemUsrFromPrj (&Projects,Prj_ROLE_TUT);
  }

void Prj_ReqRemEvl (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   Prj_ReqRemUsrFromPrj (&Projects,Prj_ROLE_EVL);
  }

static void Prj_ReqRemUsrFromPrj (struct Prj_Projects *Projects,
                                  Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   extern const char *Txt_Remove_USER_from_this_project;
   static Act_Action_t ActionRemUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      [Prj_ROLE_UNK] = ActUnk,		// Unknown
      [Prj_ROLE_STD] = ActRemStdPrj,	// Student
      [Prj_ROLE_TUT] = ActRemTutPrj,	// Tutor
      [Prj_ROLE_EVL] = ActRemEvlPrj,	// Evaluator
     };
   struct Prj_Project Prj;
   bool ItsMe;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (Projects);
   if ((Projects->PrjCod = Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (&Prj))
	{
	 ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

	 /***** Show question and button to remove user as a role from project *****/
	 /* Begin alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,ItsMe ? Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y :
			                               Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y,
				  Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject][Gbl.Usrs.Other.UsrDat.Sex],
				  Prj.Title);

	 /* Show user's record */
	 Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	 /* Show form to request confirmation */
	 Frm_BeginForm (ActionRemUsr[RoleInProject]);
	 Projects->PrjCod = Prj.PrjCod;
	 Prj_PutCurrentParams (Projects);
	 Btn_PutRemoveButton (Str_BuildStringStr (Txt_Remove_USER_from_this_project,
					          Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject][Gbl.Usrs.Other.UsrDat.Sex]));
	 Str_FreeString ();
	 Frm_EndForm ();

	 /* End alert */
	 Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,
	                          NULL,NULL,
	                          Btn_NO_BUTTON,NULL);
	}
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Put form to edit project again *****/
   Prj_RequestCreatOrEditPrj (Projects,&Prj);
  }

/*****************************************************************************/
/************************ Remove user from project ***************************/
/*****************************************************************************/

void Prj_RemStd (void)
  {
   Prj_RemUsrFromPrj (Prj_ROLE_STD);
  }

void Prj_RemTut (void)
  {
   Prj_RemUsrFromPrj (Prj_ROLE_TUT);
  }

void Prj_RemEvl (void)
  {
   Prj_RemUsrFromPrj (Prj_ROLE_EVL);
  }

static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_as_a_Y_from_the_project_Z;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   struct Prj_Projects Projects;
   struct Prj_Project Prj;
   bool ItsMe;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Projects.PrjCod = Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (&Prj))
	{
	 /***** Remove user from the table of project-users *****/
	 DB_QueryDELETE ("can not remove a user from a project",
			 "DELETE FROM prj_users"
			 " WHERE PrjCod=%ld"
			   " AND RoleInProject=%u"
			   " AND UsrCod=%ld",
		         Prj.PrjCod,
		         (unsigned) RoleInProject,
		         Gbl.Usrs.Other.UsrDat.UsrCod);

	 /***** Flush cache *****/
	 ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
	 if (ItsMe)
	    Prj_FlushCacheMyRolesInProject ();

	 /***** Show success alert *****/
         Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_as_a_Y_from_the_project_Z,
		        Gbl.Usrs.Other.UsrDat.FullName,
		        Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject][Gbl.Usrs.Other.UsrDat.Sex],
		        Prj.Title);
	}
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Put form to edit project again *****/
   Prj_RequestCreatOrEditPrj (&Projects,&Prj);
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of projects **********/
/*****************************************************************************/

static Prj_Order_t Prj_GetParamPrjOrder (void)
  {
   return (Prj_Order_t)
	  Par_GetParToUnsignedLong ("Order",
				    0,
				    Prj_NUM_ORDERS - 1,
				    (unsigned long) Prj_ORDER_DEFAULT);
  }

/*****************************************************************************/
/****************** Put a link (form) to edit one project ********************/
/*****************************************************************************/

static void Prj_PutFormsToRemEditOnePrj (struct Prj_Projects *Projects,
					 const struct Prj_Project *Prj,
                                         const char *Anchor,
                                         bool ICanViewProjectFiles)
  {
   Projects->PrjCod = Prj->PrjCod;	// Used as parameter in contextual links

   if (Prj_CheckIfICanEditProject (Prj))
     {
      /***** Put form to remove project *****/
      Ico_PutContextualIconToRemove (ActReqRemPrj,NULL,
                                     Prj_PutCurrentParams,Projects);

      /***** Put form to hide/show project *****/
      switch (Prj->Hidden)
        {
	 case Prj_HIDDEN:
	    Ico_PutContextualIconToUnhide (ActShoPrj,Anchor,
	                                   Prj_PutCurrentParams,Projects);
	    break;
	 case Prj_VISIBL:
	    Ico_PutContextualIconToHide (ActHidPrj,Anchor,
	                                 Prj_PutCurrentParams,Projects);
	    break;
        }

      /***** Put form to edit project *****/
      Ico_PutContextualIconToEdit (ActEdiOnePrj,NULL,
                                   Prj_PutCurrentParams,Projects);
     }

   /***** Put form to admin project documents *****/
   if (ICanViewProjectFiles)
      Ico_PutContextualIconToViewFiles (ActAdmDocPrj,
                                        Prj_PutCurrentParams,Projects);

   /***** Put form to print project *****/
   Ico_PutContextualIconToPrint (ActPrnOnePrj,
                                 Prj_PutCurrentParams,Projects);

   /***** Locked/unlocked project edition *****/
   if (Prj_CheckIfICanConfigAllProjects ())
     {
      /* Put form to lock/unlock project edition */
      HTM_DIV_Begin ("id=\"prj_lck_%ld\"",Prj->PrjCod);
      Prj_FormLockUnlock (Prj);
      HTM_DIV_End ();
     }
   else
      /* Put icon toinform about locked/unlocked project edition */
      Prj_PutIconOffLockedUnlocked (Prj);
  }

/*****************************************************************************/
/************************ Can I edit a given project? ************************/
/*****************************************************************************/

static bool Prj_CheckIfICanEditProject (const struct Prj_Project *Prj)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 if (Prj->Locked == Prj_LOCKED)			// Locked edition
	    return false;
	 return (Prj_GetMyRolesInProject (Prj->PrjCod) &
	         (1 << Prj_ROLE_TUT)) != 0;		// Am I a tutor?
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************************** List all the projects ****************************/
/*****************************************************************************/

static void Prj_GetListProjects (struct Prj_Projects *Projects)
  {
   char *PreNonSubQuery;
   char *HidVisSubQuery;
   char *DptCodSubQuery;
   static const char *OrderBySubQuery[Prj_NUM_ORDERS] =
     {
      [Prj_ORDER_START_TIME] = "prj_projects.CreatTime DESC,"
			       "prj_projects.ModifTime DESC,"
			       "prj_projects.Title",
      [Prj_ORDER_END_TIME  ] = "prj_projects.ModifTime DESC,"
			       "prj_projects.CreatTime DESC,"
			       "prj_projects.Title",
      [Prj_ORDER_TITLE     ] = "prj_projects.Title,"
			       "prj_projects.CreatTime DESC,"
			       "prj_projects.ModifTime DESC",
      [Prj_ORDER_DEPARTMENT] = "dpt_departments.FullName,"
			       "prj_projects.CreatTime DESC,"
			       "prj_projects.ModifTime DESC,"
			       "prj_projects.Title",
     };
   MYSQL_RES *mysql_res = NULL;	// Initialized to avoid freeing when not assigned
   unsigned NumUsrsInList;
   long *LstSelectedUsrCods;
   char *SubQueryUsrs;
   unsigned NumPrjsFromDB = 0;
   unsigned NumPrjsAfterFilter = 0;
   unsigned NumPrj;
   struct Prj_Faults Faults;
   long PrjCod;

   /***** Get list of projects from database *****/
   if (Projects->LstIsRead)
      Prj_FreeListProjects (Projects);

   if (Projects->Filter.Assign &&	// Any selector is on
       Projects->Filter.Hidden &&	// Any selector is on
       Projects->Filter.Faulti)		// Any selector is on
     {
      /* Assigned subquery */
      switch (Projects->Filter.Assign)
	{
	 case (1 << Prj_ASSIGNED):	// Assigned projects
	    if (asprintf (&PreNonSubQuery," AND prj_projects.Assigned='Y'") < 0)
	       Err_NotEnoughMemoryExit ();
	    break;
	 case (1 << Prj_NONASSIG):	// Non-assigned projects
	    if (asprintf (&PreNonSubQuery," AND prj_projects.Assigned='N'") < 0)
	       Err_NotEnoughMemoryExit ();
	    break;
	 default:			// All projects
	    if (asprintf (&PreNonSubQuery,"%s","") < 0)
	       Err_NotEnoughMemoryExit ();
	    break;
	}

      /* Hidden subquery */
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:	// Students can view only visible projects
	    if (asprintf (&HidVisSubQuery," AND prj_projects.Hidden='N'") < 0)
	       Err_NotEnoughMemoryExit ();
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    switch (Projects->Filter.Hidden)
	      {
	       case (1 << Prj_HIDDEN):	// Hidden projects
		  if (asprintf (&HidVisSubQuery," AND prj_projects.Hidden='Y'") < 0)
	             Err_NotEnoughMemoryExit ();
		  break;
	       case (1 << Prj_VISIBL):	// Visible projects
		  if (asprintf (&HidVisSubQuery," AND prj_projects.Hidden='N'") < 0)
	             Err_NotEnoughMemoryExit ();
		  break;
	       default:			// All projects
		  if (asprintf (&HidVisSubQuery,"%s","") < 0)
	             Err_NotEnoughMemoryExit ();
		  break;
	      }
	    break;
	 default:
	    Err_WrongRoleExit ();
	    break;
	}

      /* Department subquery */
      if (Projects->Filter.DptCod >= 0)
        {
	 if (asprintf (&DptCodSubQuery," AND prj_projects.DptCod=%ld",
	               Projects->Filter.DptCod) < 0)
	    Err_NotEnoughMemoryExit ();
        }
      else	// Any department
	{
	 if (asprintf (&DptCodSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	}

      /* Query */
      switch (Projects->Filter.Who)
        {
	 case Usr_WHO_ME:
	    /* Get list of projects */
	    switch (Projects->SelectedOrder)
	      {
	       case Prj_ORDER_START_TIME:
	       case Prj_ORDER_END_TIME:
	       case Prj_ORDER_TITLE:
		  NumPrjsFromDB = (unsigned)
		  DB_QuerySELECT (&mysql_res,"can not get projects",
				  "SELECT prj_projects.PrjCod"
				   " FROM prj_projects,"
				         "prj_users"
				  " WHERE prj_projects.CrsCod=%ld"
				    "%s"
				    "%s"
				    "%s"
				    " AND prj_projects.PrjCod=prj_users.PrjCod"
				    " AND prj_users.UsrCod=%ld"
				  " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
				  " ORDER BY %s",
				  Gbl.Hierarchy.Crs.CrsCod,
				  PreNonSubQuery,
				  HidVisSubQuery,
				  DptCodSubQuery,
				  Gbl.Usrs.Me.UsrDat.UsrCod,
				  OrderBySubQuery[Projects->SelectedOrder]);
		  break;
	       case Prj_ORDER_DEPARTMENT:
		  NumPrjsFromDB = (unsigned)
		  DB_QuerySELECT (&mysql_res,"can not get projects",
				  "SELECT prj_projects.PrjCod"
				   " FROM prj_projects LEFT JOIN dpt_departments,"
				         "prj_users"
				     " ON prj_projects.DptCod=dpt_departments.DptCod"
				  " WHERE prj_projects.CrsCod=%ld"
				    "%s"
				    "%s"
				    "%s"
				    " AND prj_projects.PrjCod=prj_users.PrjCod"
				    " AND prj_users.UsrCod=%ld"
				  " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
				  " ORDER BY %s",
				  Gbl.Hierarchy.Crs.CrsCod,
				  PreNonSubQuery,
				  HidVisSubQuery,
				  DptCodSubQuery,
				  Gbl.Usrs.Me.UsrDat.UsrCod,
				  OrderBySubQuery[Projects->SelectedOrder]);
		  break;
	      }
	    break;
         case Usr_WHO_SELECTED:
            /* Count number of valid users in list of encrypted user codes */
	    NumUsrsInList = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected);

	    if (NumUsrsInList)
	      {
	       /* Get list of users selected to show their projects */
	       Usr_GetListSelectedUsrCods (&Gbl.Usrs.Selected,NumUsrsInList,&LstSelectedUsrCods);

	       /* Create subquery string */
	       Usr_CreateSubqueryUsrCods (LstSelectedUsrCods,NumUsrsInList,
					  &SubQueryUsrs);

	       /* Get list of projects */
	       switch (Projects->SelectedOrder)
		 {
		  case Prj_ORDER_START_TIME:
		  case Prj_ORDER_END_TIME:
		  case Prj_ORDER_TITLE:
		     NumPrjsFromDB = (unsigned)
		     DB_QuerySELECT (&mysql_res,"can not get projects",
				     "SELECT prj_projects.PrjCod"
				      " FROM prj_projects,"
					    "prj_users"
				     " WHERE prj_projects.CrsCod=%ld"
				       "%s"
				       "%s"
				       "%s"
				       " AND prj_projects.PrjCod=prj_users.PrjCod"
				       " AND prj_users.UsrCod IN (%s)"
				     " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
				     " ORDER BY %s",
				     Gbl.Hierarchy.Crs.CrsCod,
				     PreNonSubQuery,
				     HidVisSubQuery,
				     DptCodSubQuery,
				     SubQueryUsrs,
				     OrderBySubQuery[Projects->SelectedOrder]);
		     break;
		  case Prj_ORDER_DEPARTMENT:
		     NumPrjsFromDB = (unsigned)
		     DB_QuerySELECT (&mysql_res,"can not get projects",
				     "SELECT prj_projects.PrjCod"
				      " FROM prj_projects LEFT JOIN dpt_departments,"
					    "prj_users"
				        " ON prj_projects.DptCod=dpt_departments.DptCod"
				     " WHERE prj_projects.CrsCod=%ld"
				       "%s"
				       "%s"
				       "%s"
				       " AND prj_projects.PrjCod=prj_users.PrjCod"
				       " AND prj_users.UsrCod IN (%s)"
				     " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
				     " ORDER BY %s",
				     Gbl.Hierarchy.Crs.CrsCod,
				     PreNonSubQuery,
				     HidVisSubQuery,
				     DptCodSubQuery,
				     SubQueryUsrs,
				     OrderBySubQuery[Projects->SelectedOrder]);
		     break;
		 }

	       /* Free memory for subquery string */
	       Usr_FreeSubqueryUsrCods (SubQueryUsrs);

	       /* Free list of user codes */
	       Usr_FreeListSelectedUsrCods (LstSelectedUsrCods);
	      }
	    break;
         case Usr_WHO_ALL:
	    /* Get list of projects */
	    switch (Projects->SelectedOrder)
	      {
	       case Prj_ORDER_START_TIME:
	       case Prj_ORDER_END_TIME:
	       case Prj_ORDER_TITLE:
		  NumPrjsFromDB = (unsigned)
		  DB_QuerySELECT (&mysql_res,"can not get projects",
				  "SELECT prj_projects.PrjCod"
				   " FROM prj_projects"
				  " WHERE prj_projects.CrsCod=%ld"
				    "%s"
				    "%s"
				    "%s"
				  " ORDER BY %s",
				  Gbl.Hierarchy.Crs.CrsCod,
				  PreNonSubQuery,
				  HidVisSubQuery,
				  DptCodSubQuery,
				  OrderBySubQuery[Projects->SelectedOrder]);
		  break;
	       case Prj_ORDER_DEPARTMENT:
		  NumPrjsFromDB = (unsigned)
		  DB_QuerySELECT (&mysql_res,"can not get projects",
				  "SELECT prj_projects.PrjCod"
				   " FROM prj_projects LEFT JOIN dpt_departments"
				     " ON prj_projects.DptCod=dpt_departments.DptCod"
				  " WHERE prj_projects.CrsCod=%ld"
				    "%s"
				    "%s"
				    "%s"
				  " ORDER BY %s",
				  Gbl.Hierarchy.Crs.CrsCod,
				  PreNonSubQuery,
				  HidVisSubQuery,
				  DptCodSubQuery,
				  OrderBySubQuery[Projects->SelectedOrder]);
		  break;
	      }
	    break;
	 default:
	    Err_WrongWhoExit ();
	    break;
        }

      /* Free allocated memory for subqueries */
      free (PreNonSubQuery);
      free (HidVisSubQuery);
      free (DptCodSubQuery);

      if (NumPrjsFromDB) // Projects found...
	{
	 /***** Create list of projects *****/
	 if ((Projects->LstPrjCods = calloc ((size_t) NumPrjsFromDB,
	                                     sizeof (*Projects->LstPrjCods))) == NULL)
	    Err_NotEnoughMemoryExit ();

	 /***** Get the projects codes *****/
	 for (NumPrj = 0;
	      NumPrj < NumPrjsFromDB;
	      NumPrj++)
	   {
	    /* Get next project code */
	    if ((PrjCod = DB_GetNextCode (mysql_res)) < 0)
               Err_WrongProjectExit ();

	    /* Filter projects depending on faultiness */
	    switch (Projects->Filter.Faulti)
	      {
	       case (1 << Prj_FAULTY):		// Faulty projects
		  if (Prj_CheckIfPrjIsFaulty (PrjCod,&Faults))
		     Projects->LstPrjCods[NumPrjsAfterFilter++] = PrjCod;
		  break;
	       case (1 << Prj_FAULTLESS):	// Faultless projects
		  if (!Prj_CheckIfPrjIsFaulty (PrjCod,&Faults))
		     Projects->LstPrjCods[NumPrjsAfterFilter++] = PrjCod;
		  break;
	       default:				// All projects
		  Projects->LstPrjCods[NumPrjsAfterFilter++] = PrjCod;
		  break;
	      }
	   }
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   Projects->Num = NumPrjsAfterFilter;
   Projects->LstIsRead = true;
  }

/*****************************************************************************/
/****************** Check if a project exists in a course ********************/
/*****************************************************************************/

long Prj_GetCourseOfProject (long PrjCod)
  {
   /***** Trivial check: project code should be > 0 *****/
   if (PrjCod <= 0)
      return -1L;

   /***** Get course code from database *****/
   return DB_QuerySELECTCode ("can not get project course",
			      "SELECT CrsCod"		// row[0]
			       " FROM prj_projects"
			      " WHERE PrjCod=%ld",
			      PrjCod); // Project found...
  }

/*****************************************************************************/
/********************* Get project data using its code ***********************/
/*****************************************************************************/

void Prj_GetDataOfProjectByCod (struct Prj_Project *Prj)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Prj_Proposal_t Proposal;

   if (Prj->PrjCod > 0)
     {
      /***** Clear all project data *****/
      Prj_ResetProject (Prj);

      /***** Get data of project *****/
      if (DB_QuerySELECT (&mysql_res,"can not get project data",
			  "SELECT PrjCod,"			// row[ 0]
				 "CrsCod,"			// row[ 1]
				 "DptCod,"			// row[ 2]
				 "Locked,"			// row[ 3]
				 "Hidden,"			// row[ 4]
				 "Assigned,"			// row[ 5]
				 "NumStds,"			// row[ 6]
				 "Proposal,"			// row[ 7]
				 "UNIX_TIMESTAMP(CreatTime),"	// row[ 8]
				 "UNIX_TIMESTAMP(ModifTime),"	// row[ 9]
				 "Title,"			// row[10]
				 "Description,"			// row[11]
				 "Knowledge,"			// row[12]
				 "Materials,"			// row[13]
				 "URL"				// row[14]
			   " FROM prj_projects"
			  " WHERE PrjCod=%ld"
			    " AND CrsCod=%ld",
			  Prj->PrjCod,
			  Gbl.Hierarchy.Crs.CrsCod))	// Project found...
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of the project (row[0]) */
	 Prj->PrjCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get code of the course (row[1]) */
	 Prj->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

	 /* Get code of the department (row[2]) */
	 Prj->DptCod = Str_ConvertStrCodToLongCod (row[2]);

	 /* Get whether the project is locked or not (row[3]) */
	 Prj->Locked = (row[3][0] == 'Y') ? Prj_LOCKED :
					    Prj_UNLOCKED;

	 /* Get whether the project is hidden or not (row[4]) */
	 Prj->Hidden = (row[4][0] == 'Y') ? Prj_HIDDEN :
					    Prj_VISIBL;

	 /* Get if project is assigned or not (row[5]) */
	 Prj->Assigned = (row[5][0] == 'Y') ? Prj_ASSIGNED :
					      Prj_NONASSIG;

	 /* Get number of students (row[6]) */
	 Prj->NumStds = Str_ConvertStrToUnsigned (row[6]);

	 /* Get project status (row[7]) */
	 Prj->Proposal = Prj_PROPOSAL_DEFAULT;
	 for (Proposal  = (Prj_Proposal_t) 0;
	      Proposal <= (Prj_Proposal_t) (Prj_NUM_PROPOSAL_TYPES - 1);
	      Proposal++)
	    if (!strcmp (Prj_Proposal_DB[Proposal],row[7]))
	      {
	       Prj->Proposal = Proposal;
	       break;
	      }

	 /* Get creation date/time (row[8] holds the creation UTC time) */
	 Prj->CreatTime = Dat_GetUNIXTimeFromStr (row[8]);

	 /* Get modification date/time (row[9] holds the modification UTC time) */
	 Prj->ModifTime = Dat_GetUNIXTimeFromStr (row[9]);

	 /* Get title (row[10]), description (row[11]), required knowledge (row[12]),
	    required materials (row[13]) and URL (row[14]) of the project */
	 Str_Copy (Prj->Title      ,row[10],sizeof (Prj->Title) - 1);
	 Str_Copy (Prj->Description,row[11],Cns_MAX_BYTES_TEXT);
	 Str_Copy (Prj->Knowledge  ,row[12],Cns_MAX_BYTES_TEXT);
	 Str_Copy (Prj->Materials  ,row[13],Cns_MAX_BYTES_TEXT);
	 Str_Copy (Prj->URL        ,row[14],sizeof (Prj->URL  ) - 1);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
     {
      /***** Clear all project data *****/
      Prj->PrjCod = -1L;
      Prj_ResetProject (Prj);
     }
  }

/*****************************************************************************/
/************************** Clear all project data ***************************/
/*****************************************************************************/

static void Prj_ResetProject (struct Prj_Project *Prj)
  {
   if (Prj->PrjCod <= 0)	// If > 0 ==> keep value
      Prj->PrjCod = -1L;
   Prj->CrsCod    = -1L;
   Prj->Locked	  = Prj_UNLOCKED;
   Prj->Hidden    = Prj_NEW_PRJ_HIDDEN_VISIBL_DEFAULT;
   Prj->Assigned  = Prj_NEW_PRJ_ASSIGNED_NONASSIG_DEFAULT;
   Prj->NumStds   = 1;
   Prj->Proposal  = Prj_PROPOSAL_DEFAULT;
   Prj->CreatTime =
   Prj->ModifTime = (time_t) 0;
   Prj->Title[0]  = '\0';
   Prj->DptCod    = -1L;	// Unknown department
   Prj->Description[0] = '\0';
   Prj->Knowledge[0]   = '\0';
   Prj->Materials[0]   = '\0';
   Prj->URL[0]         = '\0';
  }

/*****************************************************************************/
/*************************** Free list of projects ***************************/
/*****************************************************************************/

void Prj_FreeListProjects (struct Prj_Projects *Projects)
  {
   if (Projects->LstIsRead && Projects->LstPrjCods)
     {
      /***** Free memory used by the list of projects *****/
      free (Projects->LstPrjCods);
      Projects->LstPrjCods = NULL;
      Projects->Num = 0;
      Projects->LstIsRead = false;
     }
  }

/*****************************************************************************/
/******************* Write parameter with code of project ********************/
/*****************************************************************************/

void Prj_PutParamPrjCod (long PrjCod)
  {
   Par_PutHiddenParamLong (NULL,"PrjCod",PrjCod);
  }

/*****************************************************************************/
/******************** Get parameter with code of project *********************/
/*****************************************************************************/

long Prj_GetParamPrjCod (void)
  {
   /***** Get code of project *****/
   return Par_GetParToLong ("PrjCod");
  }

/*****************************************************************************/
/**************** Ask for confirmation of removing a project *****************/
/*****************************************************************************/

void Prj_ReqRemProject (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_project_X;
   extern const char *Txt_Remove_project;
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
     {
      /***** Show question and button to remove the project *****/
      Projects.PrjCod = Prj.PrjCod;
      Ale_ShowAlertAndButton (ActRemPrj,NULL,NULL,
                              Prj_PutCurrentParams,&Projects,
			      Btn_REMOVE_BUTTON,Txt_Remove_project,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_project_X,
	                      Prj.Title);
     }
   else
      Err_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/***************************** Remove a project ******************************/
/*****************************************************************************/

void Prj_RemoveProject (void)
  {
   extern const char *Txt_Project_X_removed;
   struct Prj_Projects Projects;
   struct Prj_Project Prj;
   char PathRelPrj[PATH_MAX + 1];

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);	// Inside this function, the course is checked to be the current one

   if (Prj_CheckIfICanEditProject (&Prj))
     {
      /***** Remove users in project *****/
      DB_QueryDELETE ("can not remove project",
		      "DELETE FROM prj_users"
		      " USING prj_projects,"
		             "prj_users"
		      " WHERE prj_projects.PrjCod=%ld"
		        " AND prj_projects.CrsCod=%ld"
		        " AND prj_projects.PrjCod=prj_users.PrjCod",
	              Prj.PrjCod,
	              Gbl.Hierarchy.Crs.CrsCod);

      /***** Flush cache *****/
      Prj_FlushCacheMyRolesInProject ();

      /***** Remove project *****/
      DB_QueryDELETE ("can not remove project",
		      "DELETE FROM prj_projects"
		      " WHERE PrjCod=%ld"
		        " AND CrsCod=%ld",
	              Prj.PrjCod,
	              Gbl.Hierarchy.Crs.CrsCod);

      /***** Remove information related to files in project *****/
      Brw_RemovePrjFilesFromDB (Prj.PrjCod);

      /***** Remove directory of the project *****/
      snprintf (PathRelPrj,sizeof (PathRelPrj),"%s/%ld/%s/%02u/%ld",
	        Cfg_PATH_CRS_PRIVATE,Prj.CrsCod,Cfg_FOLDER_PRJ,
	        (unsigned) (Prj.PrjCod % 100),Prj.PrjCod);
      Fil_RemoveTree (PathRelPrj);

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Project_X_removed,
	             Prj.Title);
     }
   else
      Err_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/****************************** Hide a project *******************************/
/*****************************************************************************/

void Prj_HideProject (void)
  {
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
      /***** Hide project *****/
      DB_QueryUPDATE ("can not hide project",
		      "UPDATE prj_projects"
		        " SET Hidden='Y'"
		      " WHERE PrjCod=%ld"
		        " AND CrsCod=%ld",
	              Prj.PrjCod,
	              Gbl.Hierarchy.Crs.CrsCod);
   else
      Err_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/****************************** Unhide a project *****************************/
/*****************************************************************************/

void Prj_UnhideProject (void)
  {
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
      /***** Show project *****/
      DB_QueryUPDATE ("can not show project",
		      "UPDATE prj_projects"
		        " SET Hidden='N'"
		      " WHERE PrjCod=%ld"
		        " AND CrsCod=%ld",
	              Prj.PrjCod,
	              Gbl.Hierarchy.Crs.CrsCod);
   else
      Err_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/********************* Put a form to create/edit project *********************/
/*****************************************************************************/

void Prj_RequestCreatePrj (void)
  {
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   Projects.PrjCod = Prj.PrjCod = -1L;	// It's a new, non existing, project

   /***** Form to create project *****/
   Prj_RequestCreatOrEditPrj (&Projects,&Prj);
  }

void Prj_RequestEditPrj (void)
  {
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Projects.PrjCod = Prj.PrjCod = Prj_GetParamPrjCod ()) <= 0)
      Err_WrongProjectExit ();

   /***** Form to edit project *****/
   Prj_RequestCreatOrEditPrj (&Projects,&Prj);
  }

static void Prj_RequestCreatOrEditPrj (struct Prj_Projects *Projects,
                                       struct Prj_Project *Prj)
  {
   bool ItsANewProject = (Prj->PrjCod < 0);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (Prj);

   /***** Get from the database the data of the project *****/
   if (ItsANewProject)
     {
      /* Initialize to empty project */
      Prj_ResetProject (Prj);
      Prj->CreatTime = Gbl.StartExecutionTimeUTC;
      Prj->ModifTime = Gbl.StartExecutionTimeUTC;
      Prj->DptCod = Gbl.Usrs.Me.UsrDat.Tch.DptCod;	// Default: my department
     }
   else
      /* Get data of the project from database */
      Prj_GetDataOfProjectByCod (Prj);

   /***** Put form to edit project *****/
   Prj_PutFormProject (Projects,Prj,ItsANewProject);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (Projects);
  }

static void Prj_PutFormProject (struct Prj_Projects *Projects,
                                struct Prj_Project *Prj,bool ItsANewProject)
  {
   extern const char *Hlp_ASSESSMENT_Projects_new_project;
   extern const char *Hlp_ASSESSMENT_Projects_edit_project;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_New_project;
   extern const char *Txt_Edit_project;
   extern const char *Txt_Data;
   extern const char *Txt_Title;
   extern const char *Txt_Department;
   extern const char *Txt_Another_department;
   extern const char *Txt_Assigned_QUESTION;
   extern const char *Txt_Number_of_students;
   extern const char *Txt_Proposal;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   extern const char *Txt_URL;
   extern const char *Txt_No;
   extern const char *Txt_Yes;
   extern const char *Txt_Create_project;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Members;
   Prj_Proposal_t Proposal;
   unsigned ProposalUnsigned;
   unsigned NumRoleToShow;

   /***** Begin project box *****/
   if (ItsANewProject)
     {
      Projects->PrjCod = -1L;
      Box_BoxBegin (NULL,Txt_New_project,
                    NULL,NULL,
                    Hlp_ASSESSMENT_Projects_new_project,Box_NOT_CLOSABLE);
     }
   else
     {
      Projects->PrjCod = Prj->PrjCod;
      Box_BoxBegin (NULL,Prj->Title[0] ? Prj->Title :
                	                 Txt_Edit_project,
                    NULL,NULL,
                    Hlp_ASSESSMENT_Projects_edit_project,Box_NOT_CLOSABLE);
     }

   /***** 1. Project members *****/
   if (!ItsANewProject)	// Existing project
     {
      Box_BoxTableBegin (NULL,Txt_Members,
                         NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      for (NumRoleToShow = 0;
	   NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	   NumRoleToShow++)
	 Prj_ShowOneProjectMembersWithARole (Projects,Prj,Prj_EDIT_ONE_PROJECT,
	                                     Prj_RolesToShow[NumRoleToShow]);
      Box_BoxTableEnd ();
     }

   /***** 2. Project data *****/
   /* Begin data form */
   Frm_BeginForm (ItsANewProject ? ActNewPrj :
	                           ActChgPrj);
   Prj_PutCurrentParams (Projects);

      /* Begin box and table */
      Box_BoxTableBegin (NULL,Txt_Data,
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);

	 /* Project title */
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","Title",Txt_Title);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_INPUT_TEXT ("Title",Prj_MAX_CHARS_PROJECT_TITLE,Prj->Title,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "id=\"Title\" required=\"required\""
			       " class=\"TITLE_DESCRIPTION_WIDTH\"");
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /* Department */
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT",Dpt_PARAM_DPT_COD_NAME,Txt_Department);

	    /* Data */
	    HTM_TD_Begin ("class=\"LT\"");
	       Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Ins.InsCod,	// Departments in current institution
					    Prj->DptCod,		// Selected department
					    "TITLE_DESCRIPTION_WIDTH",	// Selector class
					    0,				// First option
					    Txt_Another_department,	// Text when no department selected
					    false);			// Don't submit on change
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /* Assigned? */
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	       HTM_TxtColon (Txt_Assigned_QUESTION);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
				 "name=\"Assigned\"");
		  HTM_OPTION (HTM_Type_STRING,"Y",Prj->Assigned == Prj_ASSIGNED,false,
			      "%s",Txt_Yes);
		  HTM_OPTION (HTM_Type_STRING,"N",Prj->Assigned == Prj_NONASSIG,false,
			      "%s",Txt_No);
	       HTM_SELECT_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /* Number of students */
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	       HTM_TxtColon (Txt_Number_of_students);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_INPUT_LONG ("NumStds",(long) 0,(long) UINT_MAX,(long) Prj->NumStds,
			       HTM_DONT_SUBMIT_ON_CHANGE,false,
			       NULL);
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /* Proposal */
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
	       HTM_TxtColon (Txt_Proposal);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LM\"");
	       HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
				 "name=\"Proposal\" class=\"TITLE_DESCRIPTION_WIDTH\"");
		  for (Proposal  = (Prj_Proposal_t) 0;
		       Proposal <= (Prj_Proposal_t) (Prj_NUM_PROPOSAL_TYPES - 1);
		       Proposal++)
		    {
		     ProposalUnsigned = (unsigned) Proposal;
		     HTM_OPTION (HTM_Type_UNSIGNED,&ProposalUnsigned,
				 Prj->Proposal == Proposal,false,
				 "%s",Txt_PROJECT_STATUS[Proposal]);
		    }
	       HTM_SELECT_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /* Description of the project */
	 Prj_EditOneProjectTxtArea ("Description",Txt_Description,
				    Prj->Description,12,
				    true);	// Required

	 /* Required knowledge to carry out the project */
	 Prj_EditOneProjectTxtArea ("Knowledge",Txt_Required_knowledge,
				    Prj->Knowledge,4,
				    false);	// Not required

	 /* Required materials to carry out the project */
	 Prj_EditOneProjectTxtArea ("Materials",Txt_Required_materials,
				    Prj->Materials,4,
				    false);	// Not required

	 /* URL for additional info */
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("RT","WWW",Txt_URL);

	    /* Data */
	    HTM_TD_Begin ("class=\"DAT LT\"");
	       HTM_INPUT_URL ("URL",Prj->URL,HTM_DONT_SUBMIT_ON_CHANGE,
			      "class=\"TITLE_DESCRIPTION_WIDTH\"");
	    HTM_TD_End ();

	 HTM_TR_End ();

      /* End table, send button and end box */
      if (ItsANewProject)
	 Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_project);
      else
	 Box_BoxTableWithButtonEnd (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /* End data form */
   Frm_EndForm ();

   /***** End project box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************************ Show text row about a project **********************/
/*****************************************************************************/

static void Prj_EditOneProjectTxtArea (const char *Id,
                                       const char *Label,char *TxtField,
                                       unsigned NumRows,bool Required)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];

   /***** Description *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT",Id,Label);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_TEXTAREA_Begin ("id=\"%s\" name=\"%s\" rows=\"%u\"%s"
	               " class=\"TITLE_DESCRIPTION_WIDTH\"",
                       Id,Id,NumRows,Required ? " required=\"required\"" :
                	                        "");
   HTM_Txt (TxtField);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*** Allocate memory for those parameters of a project with a lot of text ****/
/*****************************************************************************/

void Prj_AllocMemProject (struct Prj_Project *Prj)
  {
   if ((Prj->Description = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((Prj->Knowledge   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   if ((Prj->Materials   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/****** Free memory of those parameters of a project with a lot of text ******/
/*****************************************************************************/

void Prj_FreeMemProject (struct Prj_Project *Prj)
  {
   if (Prj->Description)
     {
      free (Prj->Description);
      Prj->Description = NULL;
     }
   if (Prj->Knowledge)
     {
      free (Prj->Knowledge);
      Prj->Knowledge = NULL;
     }
   if (Prj->Materials)
     {
      free (Prj->Materials);
      Prj->Materials = NULL;
     }
  }

/*****************************************************************************/
/******************** Receive form to create a new project *******************/
/*****************************************************************************/

void Prj_ReceiveFormProject (void)
  {
   extern const char *Txt_You_must_specify_the_title_of_the_project;
   extern const char *Txt_Created_new_project_X;
   extern const char *Txt_The_project_has_been_modified;
   struct Prj_Projects Projects;
   struct Prj_Project Prj;	// Project data received from form
   bool ItsANewProject;
   bool ICanEditProject;
   bool NewProjectIsCorrect = true;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   ItsANewProject = ((Projects.PrjCod = Prj.PrjCod = Prj_GetParamPrjCod ()) < 0);

   if (ItsANewProject)
     {
      /* Reset project data */
      Prj_ResetProject (&Prj);

      ICanEditProject = true;
     }
   else
     {
      /* Get data of the project from database */
      Prj_GetDataOfProjectByCod (&Prj);

      ICanEditProject = Prj_CheckIfICanEditProject (&Prj);
     }

   if (ICanEditProject)
     {
      /* Get project title */
      Par_GetParToText ("Title",Prj.Title,Prj_MAX_BYTES_PROJECT_TITLE);

      /* Get department */
      Prj.DptCod = Par_GetParToLong (Dpt_PARAM_DPT_COD_NAME);

      /* Get whether the project is assigned */
      Prj.Assigned = (Par_GetParToBool ("Assigned")) ? Prj_ASSIGNED :
					 	       Prj_NONASSIG;

      /* Get number of students */
      Prj.NumStds = (unsigned)
	            Par_GetParToUnsignedLong ("NumStds",
	                                      0,
	                                      UINT_MAX,
	                                      1);

      /* Get status */
      Prj.Proposal = (Prj_Proposal_t)
	             Par_GetParToUnsignedLong ("Proposal",
	                                       0,
	                                       Prj_NUM_PROPOSAL_TYPES - 1,
                                               (unsigned long) Prj_PROPOSAL_DEFAULT);

      /* Get project description, required knowledge and required materials */
      Par_GetParToHTML ("Description",Prj.Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
      Par_GetParToHTML ("Knowledge"  ,Prj.Knowledge  ,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
      Par_GetParToHTML ("Materials"  ,Prj.Materials  ,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

      /* Get degree WWW */
      Par_GetParToText ("URL",Prj.URL,Cns_MAX_BYTES_WWW);

      /***** Check if title is correct *****/
      if (!Prj.Title[0])	// If there is not a project title
	{
	 NewProjectIsCorrect = false;
	 Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_title_of_the_project);
	}

      /***** Create a new project or update an existing one *****/
      if (NewProjectIsCorrect)
	{
	 if (ItsANewProject)
	   {
	    /* Create project */
	    Prj_CreateProject (&Prj);	// Add new project to database

	    /* Write success message */
	    Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_project_X,
		           Prj.Title);
	   }
	 else if (NewProjectIsCorrect)
	   {
	    /* Update project */
	    Prj_UpdateProject (&Prj);

	    /* Write success message */
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_project_has_been_modified);
	   }
	}
      else
         Prj_PutFormProject (&Projects,&Prj,ItsANewProject);

      /***** Show again form to edit project *****/
      Prj_RequestCreatOrEditPrj (&Projects,&Prj);
     }
   else
      Err_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/************************** Create a new project *****************************/
/*****************************************************************************/

static void Prj_CreateProject (struct Prj_Project *Prj)
  {
   /***** Set dates to now *****/
   Prj->CreatTime =
   Prj->ModifTime = Gbl.StartExecutionTimeUTC;

   /***** Create a new project *****/
   Prj->PrjCod =
   DB_QueryINSERTandReturnCode ("can not create new project",
				"INSERT INTO prj_projects"
				" (CrsCod,DptCod,Hidden,Assigned,NumStds,Proposal,"
				  "CreatTime,ModifTime,"
				  "Title,Description,Knowledge,Materials,URL)"
				" VALUES"
				" (%ld,%ld,'%c','%c',%u,'%s',"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%s','%s','%s','%s','%s')",
				Gbl.Hierarchy.Crs.CrsCod,
				Prj->DptCod,
				Prj->Hidden == Prj_HIDDEN ? 'Y' :
							    'N',
				Prj->Assigned == Prj_ASSIGNED ? 'Y' :
								'N',
				Prj->NumStds,
				Prj_Proposal_DB[Prj->Proposal],
				Prj->CreatTime,
				Prj->ModifTime,
				Prj->Title,
				Prj->Description,
				Prj->Knowledge,
				Prj->Materials,
				Prj->URL);

   /***** Insert creator as first tutor *****/
   DB_QueryINSERT ("can not add tutor",
		   "INSERT INTO prj_users"
		   " (PrjCod,RoleInProject,UsrCod)"
		   " VALUES"
		   " (%ld,%u,%ld)",
	           Prj->PrjCod,
	           (unsigned) Prj_ROLE_TUT,
	           Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Flush cache *****/
   Prj_FlushCacheMyRolesInProject ();
  }

/*****************************************************************************/
/*********************** Update an existing project **************************/
/*****************************************************************************/

static void Prj_UpdateProject (struct Prj_Project *Prj)
  {
   /***** Adjust date of last edition to now *****/
   Prj->ModifTime = Gbl.StartExecutionTimeUTC;

   /***** Update the data of the project *****/
   DB_QueryUPDATE ("can not update project",
		   "UPDATE prj_projects"
		     " SET DptCod=%ld,"
		          "Hidden='%c',"
		          "Assigned='%c',"
		          "NumStds=%u,"
		          "Proposal='%s',"
		          "ModifTime=FROM_UNIXTIME(%ld),"
		          "Title='%s',"
		          "Description='%s',"
		          "Knowledge='%s',"
		          "Materials='%s',"
		          "URL='%s'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",
	           Prj->DptCod,
	           Prj->Hidden == Prj_HIDDEN ? 'Y' :
					       'N',
	           Prj->Assigned == Prj_ASSIGNED ? 'Y' :
						   'N',
	           Prj->NumStds,
	           Prj_Proposal_DB[Prj->Proposal],
	           Prj->ModifTime,
	           Prj->Title,
	           Prj->Description,
	           Prj->Knowledge,
	           Prj->Materials,
	           Prj->URL,
	           Prj->PrjCod,
	           Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************************ Can I configure all projects? **********************/
/*****************************************************************************/

static bool Prj_CheckIfICanConfigAllProjects (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/********************** Configuration of all projects ************************/
/*****************************************************************************/

void Prj_ShowFormConfig (void)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Configure_projects;
   extern const char *Txt_Editable;
   extern const char *Txt_Editable_by_non_editing_teachers;
   extern const char *Txt_Save_changes;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Read projects configuration from database *****/
   Prj_GetConfigPrjFromDB (&Projects);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Configure_projects,
                 Prj_PutIconsListProjects,&Projects,
                 Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

   /***** Begin form *****/
   Frm_BeginForm (ActRcvCfgPrj);

   /***** Projects are editable by non-editing teachers? *****/
   HTM_TABLE_BeginCenterPadding (2);
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","Editable",Txt_Editable);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   HTM_INPUT_CHECKBOX ("Editable",HTM_DONT_SUBMIT_ON_CHANGE,
		       "id=\"Editable\" value=\"Y\"%s",
		       Projects.Config.Editable ? " checked=\"checked\"" :
			                          "");
   HTM_Txt (Txt_Editable_by_non_editing_teachers);
   HTM_TD_End ();

   HTM_TR_End ();

   HTM_TABLE_End ();

   /***** Send button *****/
   Btn_PutConfirmButton (Txt_Save_changes);

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Get configuration of projects for current course *************/
/*****************************************************************************/

static void Prj_GetConfigPrjFromDB (struct Prj_Projects *Projects)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get configuration of projects for current course from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get configuration of test",
		       "SELECT Editable"		// row[0]
			" FROM prj_config"
		       " WHERE CrsCod=%ld",
		       Gbl.Hierarchy.Crs.CrsCod) == 0)
      Projects->Config.Editable = Prj_EDITABLE_DEFAULT;
   else // NumRows == 1
     {
      /***** Get configuration *****/
      row = mysql_fetch_row (mysql_res);
      Prj_GetConfigFromRow (Projects,row);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get configuration values from a database table row *************/
/*****************************************************************************/

static void Prj_GetConfigFromRow (struct Prj_Projects *Projects,MYSQL_ROW row)
  {
   /***** Get whether project are visible via plugins or not *****/
   Projects->Config.Editable = (row[0][0] == 'Y');
  }

/*****************************************************************************/
/************ Receive configuration of projects for current course ***********/
/*****************************************************************************/

void Prj_ReceiveConfigPrj (void)
  {
   extern const char *Txt_The_configuration_of_the_projects_has_been_updated;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get whether projects are editable by non-editing teachers or not *****/
   Projects.Config.Editable = Prj_GetEditableFromForm ();

   /***** Update database *****/
   DB_QueryREPLACE ("can not save configuration of projects",
		    "REPLACE INTO prj_config"
	            " (CrsCod,Editable)"
                    " VALUES"
                    " (%ld,'%c')",
		    Gbl.Hierarchy.Crs.CrsCod,
		    Projects.Config.Editable ? 'Y' :
			                       'N');

   /***** Show confirmation message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_configuration_of_the_projects_has_been_updated);

   /***** Show again the form to configure projects *****/
   Prj_ShowFormConfig ();
  }

/*****************************************************************************/
/****** Get if projects are editable by non-editing teachers from form *******/
/*****************************************************************************/

static bool Prj_GetEditableFromForm (void)
  {
   return Par_GetParToBool ("Editable");
  }

/*****************************************************************************/
/****** Put icons to request locking/unlocking edition of all projects *******/
/*****************************************************************************/

static void Prj_PutIconsToLockUnlockAllProjects (struct Prj_Projects *Projects)
  {
   extern const char *Txt_Lock_editing;
   extern const char *Txt_Unlock_editing;

   /***** Put icon to lock all projects *****/
   Lay_PutContextualLinkOnlyIcon (ActReqLckAllPrj,NULL,
                                  Prj_PutCurrentParams,Projects,
			          "lock.svg",
				  Txt_Lock_editing);

   /***** Put icon to unlock all projects *****/
   Lay_PutContextualLinkOnlyIcon (ActReqUnlAllPrj,NULL,
                                  Prj_PutCurrentParams,Projects,
			          "unlock.svg",
				  Txt_Unlock_editing);
  }

/*****************************************************************************/
/********** Request locking/unlocking edition of selected projects ***********/
/*****************************************************************************/

void Prj_ReqLockSelectedPrjsEdition (void)
  {
   extern const char *Txt_Lock_editing;
   extern const char *Txt_Do_you_want_to_lock_the_editing_of_the_X_selected_projects;
   extern const char *Txt_No_projects;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);

   /***** Show question and button to lock all selected projects *****/
   if (Prj_CheckIfICanConfigAllProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects (&Projects);

      /* Show question and button */
      if (Projects.Num)
	 Ale_ShowAlertAndButton (ActLckAllPrj,NULL,NULL,
	                         Prj_PutCurrentParams,&Projects,
				 Btn_REMOVE_BUTTON,Txt_Lock_editing,
				 Ale_QUESTION,Txt_Do_you_want_to_lock_the_editing_of_the_X_selected_projects,
				 Projects.Num);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects (&Projects);
     }
   else
      Err_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

void Prj_ReqUnloSelectedPrjsEdition (void)
  {
   extern const char *Txt_Unlock_editing;
   extern const char *Txt_Do_you_want_to_unlock_the_editing_of_the_X_selected_projects;
   extern const char *Txt_No_projects;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);

   /***** Show question and button to unlock all selected projects *****/
   if (Prj_CheckIfICanConfigAllProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects (&Projects);

      /* Show question and button */
      if (Projects.Num)
	 Ale_ShowAlertAndButton (ActUnlAllPrj,NULL,NULL,
	                         Prj_PutCurrentParams,&Projects,
				 Btn_CREATE_BUTTON,Txt_Unlock_editing,
				 Ale_QUESTION,Txt_Do_you_want_to_unlock_the_editing_of_the_X_selected_projects,
				 Projects.Num);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects (&Projects);
     }
   else
      Err_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/***************** Lock/unlock edition of selected projects ******************/
/*****************************************************************************/

void Prj_LockSelectedPrjsEdition (void)
  {
   extern const char *Txt_No_projects;
   struct Prj_Projects Projects;
   unsigned NumPrj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);

   /***** Lock all selected projects *****/
   if (Prj_CheckIfICanConfigAllProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects (&Projects);

      /* Lock projects */
      if (Projects.Num)
	 for (NumPrj = 0;
	      NumPrj < Projects.Num;
	      NumPrj++)
            Prj_LockProjectEditionInDB (Projects.LstPrjCods[NumPrj]);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects (&Projects);
     }
   else
      Err_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

void Prj_UnloSelectedPrjsEdition (void)
  {
   extern const char *Txt_No_projects;
   struct Prj_Projects Projects;
   unsigned NumPrj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);

   /***** Unlock all selected projects *****/
   if (Prj_CheckIfICanConfigAllProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects (&Projects);

      /* Unlock projects */
      if (Projects.Num)
	 for (NumPrj = 0;
	      NumPrj < Projects.Num;
	      NumPrj++)
            Prj_UnlockProjectEditionInDB (Projects.LstPrjCods[NumPrj]);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects (&Projects);
     }
   else
      Err_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/******************** Form to lock/unlock project edition ********************/
/*****************************************************************************/

static void Prj_FormLockUnlock (const struct Prj_Project *Prj)
  {
   extern const char *Txt_LOCKED_UNLOCKED[Prj_NUM_LOCKED_UNLOCKED];
   char *OnSubmit;
   /*
   +---------------------+
   | div (parent of form)|
   | +-----------------+ |
   | |    this form    | |
   | | +-------------+ | |
   | | |     icon    | | |
   | | +-------------+ | |
   | +-----------------+ |
   +---------------------+
   */

   /***** Form and icon to lock/unlock project edition *****/
   /* Form with icon */
   if (asprintf (&OnSubmit,"updateDivLockUnlockProject(this,"
			   "'act=%ld&ses=%s&PrjCod=%ld');"
			   " return false;",	// return false is necessary to not submit form
		 Act_GetActCod (Prj_LockActions[Prj->Locked]),
		 Gbl.Session.Id,
		 Prj->PrjCod) < 0)
      Err_NotEnoughMemoryExit ();
   Frm_BeginFormOnSubmit (ActUnk,OnSubmit);
   Ico_PutIconLink (Prj_LockIcons[Prj->Locked],Txt_LOCKED_UNLOCKED[Prj->Locked]);
   Frm_EndForm ();

   /* Free allocated memory for subquery */
   free (OnSubmit);
  }

/*****************************************************************************/
/********* Put icon to inform about locked/unlocked project edition **********/
/*****************************************************************************/

static void Prj_PutIconOffLockedUnlocked (const struct Prj_Project *Prj)
  {
   extern const char *Txt_LOCKED_UNLOCKED[Prj_NUM_LOCKED_UNLOCKED];

   /***** Icon to inform about locked/unlocked project edition *****/
   Ico_PutIconOff (Prj_LockIcons[Prj->Locked],Txt_LOCKED_UNLOCKED[Prj->Locked]);
  }

/*****************************************************************************/
/************************** Lock edition of a project ************************/
/*****************************************************************************/

void Prj_LockProjectEdition (void)
  {
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
     {
      /***** Lock project edition *****/
      Prj_LockProjectEditionInDB (Prj.PrjCod);
      Prj.Locked = Prj_LOCKED;

      /***** Show updated form and icon *****/
      Prj_FormLockUnlock (&Prj);
     }
   else
      Err_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

static void Prj_LockProjectEditionInDB (long PrjCod)
  {
   DB_QueryUPDATE ("can not lock project edition",
		   "UPDATE prj_projects"
		     " SET Locked='Y'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",
		   PrjCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************************* Unlock edition of a project ***********************/
/*****************************************************************************/

void Prj_UnloProjectEdition (void)
  {
   struct Prj_Projects Projects;
   struct Prj_Project Prj;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams (&Projects);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Err_WrongProjectExit ();

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
     {
      /***** Unlock project edition *****/
      Prj_UnlockProjectEditionInDB (Prj.PrjCod);
      Prj.Locked = Prj_UNLOCKED;

      /***** Show updated form and icon *****/
      Prj_FormLockUnlock (&Prj);
     }
   else
      Err_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

static void Prj_UnlockProjectEditionInDB (long PrjCod)
  {
   DB_QueryUPDATE ("can not lock project edition",
		   "UPDATE prj_projects"
		     " SET Locked='N'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",
		   PrjCod,
		   Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******************** Remove all the projects in a course ********************/
/*****************************************************************************/

void Prj_RemoveCrsProjects (long CrsCod)
  {
   /***** Remove users in projects of the course *****/
   DB_QueryDELETE ("can not remove all the projects of a course",
		   "DELETE FROM prj_users"
		   " USING prj_projects,"
		          "prj_users"
		   " WHERE prj_projects.CrsCod=%ld"
		     " AND prj_projects.PrjCod=prj_users.PrjCod",
                   CrsCod);

   /***** Flush cache *****/
   Prj_FlushCacheMyRolesInProject ();

   /***** Remove configuration of projects in the course *****/
   DB_QueryDELETE ("can not remove configuration of projects of a course",
		   "DELETE FROM prj_config"
		   " WHERE CrsCod=%ld",
		   CrsCod);

   /***** Remove projects *****/
   DB_QueryDELETE ("can not remove all the projects of a course",
		   "DELETE FROM prj_projects"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/******************* Remove user from all his/her projects *******************/
/*****************************************************************************/

void Prj_RemoveUsrFromProjects (long UsrCod)
  {
   bool ItsMe;

   /***** Remove user from projects *****/
   DB_QueryDELETE ("can not remove user from projects",
		   "DELETE FROM prj_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);

   /***** Flush cache *****/
   ItsMe = Usr_ItsMe (UsrCod);
   if (ItsMe)
      Prj_FlushCacheMyRolesInProject ();
  }

/*****************************************************************************/
/******************** Get number of courses with projects ********************/
/*****************************************************************************/
// Returns the number of courses with projects
// in this location (all the platform, current degree or current course)

unsigned Prj_GetNumCoursesWithProjects (HieLvl_Level_t Scope)
  {
   /***** Get number of courses with projects from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prj_projects"
			" WHERE CrsCod>0");
      case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Cty.CtyCod);
      case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Ins.InsCod);
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM deg_degrees,"
			       "crs_courses,"
			      "prj_projects"
			" WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Ctr.CtrCod);
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM crs_courses,"
			       "prj_projects"
			" WHERE crs_courses.DegCod=%ld"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Deg.DegCod);
         break;
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prj_projects"
			" WHERE CrsCod=%ld",
			Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 return 0;
     }
  }

/*****************************************************************************/
/************************** Get number of projects ***************************/
/*****************************************************************************/
// Returns the number of projects in this location

unsigned Prj_GetNumProjects (HieLvl_Level_t Scope)
  {
   /***** Get number of projects from database *****/
   switch (Scope)
     {
      case HieLvl_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
                         " FROM prj_projects"
                        " WHERE CrsCod>0");
         break;
      case HieLvl_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Cty.CtyCod);
         break;
      case HieLvl_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Ins.InsCod);
         break;
      case HieLvl_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case HieLvl_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM crs_courses,"
			       "prj_projects"
			" WHERE crs_courses.DegCod=%ld"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Deg.DegCod);
         break;
      case HieLvl_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM prj_projects"
			" WHERE CrsCod=%ld",
                        Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 return 0;
     }
  }
