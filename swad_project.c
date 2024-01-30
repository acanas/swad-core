// swad_project.c: projects (final degree projects, thesis)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_database.h"
#include "swad_department.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_hidden_visible.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_program_resource.h"
#include "swad_project.h"
#include "swad_project_config.h"
#include "swad_project_database.h"
#include "swad_role.h"
#include "swad_rubric.h"
#include "swad_rubric_criteria.h"
#include "swad_rubric_database.h"
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
#define Prj_PARAM_FILTER_PRE_NON_NAME	"FilPreNon"
#define Prj_PARAM_FILTER_HID_VIS_NAME	"FilHidVis"
#define Prj_PARAM_FILTER_FAULTIN_NAME	"FilFaulti"
#define Prj_PARAM_FILTER_REVIEW_NAME	"FilReview"
#define Prj_PAR_FILTER_DPT_COD_NAME	"FilDptCod"

/***** User roles are shown in this order *****/
static const Prj_RoleInProject_t Prj_RolesToShow[] =
  {
   Prj_ROLE_TUT,	// Tutor
   Prj_ROLE_STD,	// Student
   Prj_ROLE_EVL,	// Evaluator
  };
static const unsigned Prj_NUM_ROLES_TO_SHOW = sizeof (Prj_RolesToShow) /
                                              sizeof (Prj_RolesToShow[0]);

/***** Assigned/non-assigned project *****/
static const char *AssignedNonassigIcon[Prj_NUM_ASSIGNED_NONASSIG] =
  {
   [Prj_ASSIGNED] = "user.svg",
   [Prj_NONASSIG] = "user-slash.svg",
  };

/***** Review status *****/
static struct
  {
   const char *Icon;
   Ico_Color_t Color;
  } ReviewIcon[Prj_NUM_REVIEW_STATUS] =
  {
   [Prj_UNREVIEWED] = {"file-circle-question.svg"   ,Ico_BLACK},
   [Prj_UNAPPROVED] = {"file-circle-exclamation.svg",Ico_RED  },
   [Prj_APPROVED  ] = {"file-circle-check.svg"      ,Ico_GREEN},
/*
   [Prj_UNREVIEWED] = {"filter-circle-xmark.svg",Ico_BLACK},
   [Prj_UNAPPROVED] = {"thumbs-down.svg"        ,Ico_RED  },
   [Prj_APPROVED  ] = {"thumbs-up.svg"          ,Ico_GREEN},
*/
  };

/***** Locked/unlocked project edition *****/
static const struct
  {
   const char *Icon;
   Ico_Color_t Color;
   Act_Action_t Action;
  } Prj_LockUnlock[Prj_NUM_LOCKED_UNLOCKED] =
  {
   [Prj_LOCKED  ] = {"lock.svg"  ,Ico_RED  ,ActUnlPrj},
   [Prj_UNLOCKED] = {"unlock.svg",Ico_GREEN,ActLckPrj},
  };

/***** List of users to select one or more members
       to be added to a project *****/
struct Usr_SelectedUsrs Prj_MembersToAdd =
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
   .ParSuffix = "Member",
   .Option      = Usr_OPTION_UNKNOWN,
  };

/***** Type of view when writing one project *****/
typedef enum
  {
   Prj_DONT_PUT_WARNING,
   Prj_PUT_WARNING,
  } Prj_Warning_t;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Prj_Faults
  {
   bool PrjIsFaulty;
   bool WrongTitle;
   bool WrongDescription;
   bool WrongNumStds;
   bool WrongAssigned;
   bool WrongReviewStatus;
   bool WrongModifTime;
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static long Prj_PrjCod = -1L;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prj_ReqUsrsToSelect (void *Projects);
static void Prj_GetSelectedUsrsAndShowTheirPrjs (struct Prj_Projects *Projects);
static void Prj_ShowPrjsInCurrentPage (void *Projects);

static void Prj_ShowFormToFilterByMy_All (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByAssign (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByHidden (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByWarning (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByReview (const struct Prj_Projects *Projects);
static void Prj_ShowFormToFilterByDpt (const struct Prj_Projects *Projects);

static bool Prj_CheckIfICanViewProjectFiles (long PrjCod);

static void Prj_PutParAssign (unsigned Assign);
static void Prj_PutParHidden (unsigned Hidden);
static void Prj_PutParFaulti (unsigned Faulti);
static void Prj_PutParReview (unsigned Review);
static void Prj_PutParFilterDptCod (long DptCod);
static void Prj_GetParPreNon (struct Prj_Projects *Projects);
static unsigned Prj_GetParHidVis (void);
static unsigned Prj_GetParFaulti (void);
static unsigned Prj_GetParReview (void);
static long Prj_GetParFilterDptCod (void);
static Usr_Who_t Prj_GetParWho (void);

static void Prj_ShowProjectsHead (struct Prj_Projects *Projects);
static void Prj_ShowTableAllProjectsHead (void);
static bool Prj_CheckIfICanCreateProjects (const struct Prj_Projects *Projects);
static void Prj_PutIconToCreateNewPrj (struct Prj_Projects *Projects);
static void Prj_PutIconToShowAllData (struct Prj_Projects *Projects);

static void Prj_PutIconsOnePrj (void *Projects);

//---------------------- Show one project in a row ----------------------------
static void Prj_ShowProjectRow (struct Prj_Projects *Projects);
static void Prj_ShowFirstRow (struct Prj_Projects *Projects,
                                     const char *ClassData,
                                     const struct Prj_Faults *Faults,
                                     unsigned UniqueId,
                                     const char *Anchor);
static void Prj_ShowProjectDepartment (const struct Prj_Projects *Projects,
                                       const char *ClassData);
static void Prj_ShowReviewStatus (struct Prj_Projects *Projects,
                                  const char *ClassLabel,
                                  const char *ClassData,
                                  const struct Prj_Faults *Faults,
                                  const char *Anchor);
static void Prj_PutSelectorReviewStatus (struct Prj_Projects *Projects);
static bool Prj_CheckIfICanReviewProjects (void);
static void Prj_ShowAssigned (const struct Prj_Projects *Projects,
                              const char *ClassLabel,
                              const char *ClassData,
                              const struct Prj_Faults *Faults);
static void Prj_ShowNumStds (const struct Prj_Projects *Projects,
                             const char *ClassLabel,
                             const char *ClassData,
                             const struct Prj_Faults *Faults);
static void Prj_ShowMembers (struct Prj_Projects *Projects);
static void Prj_ShowProjectMembersWithARole (struct Prj_Projects *Projects,
                                             Prj_RoleInProject_t RoleInPrj);
static void Prj_PutLinkToShowHiddenInfo (const struct Prj_Projects *Projects,
                                         unsigned UniqueId);
static void Prj_ShowProposal (const struct Prj_Projects *Projects,
                              const char *ClassLabel,
                              const char *ClassData,
                              unsigned UniqueId);
static void Prj_ShowTxtField (const struct Prj_Projects *Projects,
                              const char *ClassLabel,
                              const char *ClassData,
                              const char *id,unsigned UniqueId,
                              const char *Label,char *TxtField,
			      Prj_Warning_t Warning);
static void Prj_ShowURL (const struct Prj_Projects *Projects,
                         const char *ClassLabel,
                         const char *ClassData,
                         const char *id,unsigned UniqueId);

//------------------------------------------------ ----------------------------
static void Prj_CheckIfPrjIsFaulty (long PrjCod,struct Prj_Faults *Faults);
static void Prj_PutWarningIcon (void);
static void Prj_PutIconToToggleProject (unsigned UniqueId,
                                        const char *Icon,const char *Text);
static void Prj_ShowTableAllProjectsOneRow (struct Prj_Project *Prj);
static void Prj_ShowTableAllProjectsDepartment (const struct Prj_Project *Prj);
static void Prj_ShowTableAllProjectsTxtField (struct Prj_Project *Prj,
                                              char *TxtField);
static void Prj_ShowTableAllProjectsURL (const struct Prj_Project *Prj);
static void Prj_ShowTableAllProjectsMembersWithARole (const struct Prj_Project *Prj,
                                                      Prj_RoleInProject_t RoleInPrj);

static Prj_RoleInProject_t Prj_ConvertUnsignedStrToRoleInProject (const char *UnsignedStr);

static void Prj_FormToSelectStds (void *Projects);
static void Prj_FormToSelectTuts (void *Projects);
static void Prj_FormToSelectEvls (void *Projects);
static void Prj_FormToSelectUsrs (struct Prj_Projects *Projects,
                                  Prj_RoleInProject_t RoleInPrj);
static void Prj_AddStds (__attribute__((unused)) void *Args);
static void Prj_AddTuts (__attribute__((unused)) void *Args);
static void Prj_AddEvls (__attribute__((unused)) void *Args);
static void Prj_AddUsrsToProject (Prj_RoleInProject_t RoleInPrj);
static void Prj_ReqRemUsrFromPrj (struct Prj_Projects *Projects,
                                  Prj_RoleInProject_t RoleInPrj);
static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInPrj);

static Prj_Order_t Prj_GetParPrjOrder (void);

static void Prj_PutIconsToRemEditOnePrj (struct Prj_Projects *Projects,
                                         const char *Anchor);

static bool Prj_CheckIfICanEditProject (const struct Prj_Project *Prj);

static void Prj_GetListProjects (struct Prj_Projects *Projects);

static void Prj_ResetProject (struct Prj_Project *Prj);

static void Prj_HideUnhideProject (HidVis_HiddenOrVisible_t HiddenOrVisible);

static void Prj_ReqCreatOrEditPrj (struct Prj_Projects *Projects);
static void Prj_PutFormProject (struct Prj_Projects *Projects,bool ItsANewProject);
static void Prj_EditOneProjectTxtArea (const char *Id,
                                       const char *Label,char *TxtField,
                                       unsigned NumRows,bool Required);

static void Prj_CreateProject (struct Prj_Project *Prj);
static void Prj_UpdateProject (struct Prj_Project *Prj);

static void Prj_PutIconsToLockUnlockAllProjects (struct Prj_Projects *Projects);

static void Prj_FormLockUnlock (const struct Prj_Project *Prj);
static void Prj_PutIconOffLockedUnlocked (const struct Prj_Project *Prj);

//---------------------------- Review status ----------------------------------
static Prj_ReviewStatus_t Prj_GetParReviewStatus (void);

//------------------------------- Rubrics -------------------------------------
static void Prj_ShowRubrics (struct Prj_Projects *Projects);
static void Prj_ShowRubricsOfType (struct Prj_Projects *Projects,
                                   PrjCfg_RubricType_t RubricType);
static bool Prj_CheckIfICanViewRubric (long PrjCod,PrjCfg_RubricType_t WhichRubric);
static bool Prj_CheckIfICanFillRubric (long PrjCod,PrjCfg_RubricType_t WhichRubric);

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
/*********** Reset projects and read configuration from database *************/
/*****************************************************************************/

void Prj_ResetPrjsAndReadConfig (struct Prj_Projects *Projects)
  {
   /***** Filters *****/
   Projects->Filter.Who    = Prj_FILTER_WHO_DEFAULT;
   Projects->Filter.Assign = Prj_FILTER_ASSIGNED_DEFAULT |
	                     Prj_FILTER_NONASSIG_DEFAULT;
   Projects->Filter.Hidden = Prj_FILTER_HIDDEN_DEFAULT |
	                     Prj_FILTER_VISIBL_DEFAULT;
   Projects->Filter.Faulti = Prj_FILTER_FAULTY_DEFAULT |
	                     Prj_FILTER_FAULTLESS_DEFAULT;
   Projects->Filter.Review = Prj_FILTER_UNREVIEWED_DEFAULT |
                             Prj_FILTER_UNAPPROVED_DEFAULT |
                             Prj_FILTER_APPROVED_DEFAULT;
   Projects->Filter.DptCod = Prj_FILTER_DPT_DEFAULT;

   /***** Project list *****/
   Projects->LstIsRead     = false;	// List is not read
   Projects->Num           = 0;
   Projects->LstPrjCods    = NULL;
   Projects->SelectedOrder = Prj_ORDER_DEFAULT;
   Projects->CurrentPage   = 0;
   Projects->Prj.PrjCod    = -1L;

   /***** Read project configuration from database *****/
   PrjCfg_GetConfig (&Projects->Config);
  }

/*****************************************************************************/
/**************************** List users to select ***************************/
/*****************************************************************************/

void Prj_ListUsrsToSelect (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);

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
				     ActSeeAllPrj,
				     Prj_PutCurrentPars,Projects,
				     Txt_Projects,
				     Hlp_ASSESSMENT_Projects,
				     Txt_View_projects,
				     false);	// Do not put form with date range
  }

/*****************************************************************************/
/******* Get parameters and show all projects (depending on filters) *********/
/*****************************************************************************/

void Prj_SeeAllProjects (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);

   /***** Show projects *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/******************************* Show projects *******************************/
/*****************************************************************************/

void Prj_ShowProjects (struct Prj_Projects *Projects)
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
                                  Prj_ReqUsrsToSelect,Projects);	// when no user selected
  }

/*****************************************************************************/
/********************** Show selected projects in a table ********************/
/*****************************************************************************/

void Prj_ShowTableSelectedPrjs (void)
  {
   extern const char *Txt_No_projects;
   struct Prj_Projects Projects;
   unsigned NumPrj;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);

   /***** Get list of projects *****/
   Prj_GetListProjects (&Projects);

   if (Projects.Num)
     {
      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Projects.Prj);

      /***** Begin table *****/
      HTM_TABLE_BeginWidePadding (2);

         /***** Table head *****/
	 Prj_ShowTableAllProjectsHead ();

	 /***** Write all projects *****/
	 for (NumPrj = 0, The_ResetRowColor ();
	      NumPrj < Projects.Num;
	      NumPrj++, The_ChangeRowColor ())
	   {
	    Projects.Prj.PrjCod = Projects.LstPrjCods[NumPrj];
	    Prj_ShowTableAllProjectsOneRow (&Projects.Prj);
	   }

      /***** End table *****/
      HTM_TABLE_End ();

      /***** Free memory of the project *****/
      Prj_FreeMemProject (&Projects.Prj);
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
   struct Pag_Pagination Pagination;
   unsigned NumPrj;

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
      Box_BoxBegin (Txt_Projects,Prj_PutIconsListProjects,Projects,
		    Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

	 /***** Put filters to choice which projects to show *****/
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
	    Prj_ShowFormToFilterByReview ((struct Prj_Projects *) Projects);

	 Set_EndSettingsHead ();

	 /* 2nd. row */
	 Prj_ShowFormToFilterByDpt ((struct Prj_Projects *) Projects);

	 if (((struct Prj_Projects *) Projects)->Num)
	   {
	    /***** Write links to pages *****/
	    Pag_WriteLinksToPagesCentered (Pag_PROJECTS,&Pagination,
					   (struct Prj_Projects *) Projects,-1L);

	    /***** Allocate memory for the project *****/
	    Prj_AllocMemProject (&((struct Prj_Projects *) Projects)->Prj);

	    /***** Begin table *****/
	    HTM_TABLE_Begin ("TBL_SCROLL");

	       /***** Table head *****/
	       ((struct Prj_Projects *) Projects)->View = Prj_LIST_PROJECTS;
	       Prj_ShowProjectsHead ((struct Prj_Projects *) Projects);

	       /***** Write all projects *****/
	       for (NumPrj  = Pagination.FirstItemVisible, The_ResetRowColor ();
		    NumPrj <= Pagination.LastItemVisible;
		    NumPrj++, The_ChangeRowColor ())
		 {
		  /* Get project data */
		  ((struct Prj_Projects *) Projects)->Prj.PrjCod = ((struct Prj_Projects *) Projects)->LstPrjCods[NumPrj - 1];
		  Prj_GetProjectDataByCod (&((struct Prj_Projects *) Projects)->Prj);

		  /* Number of index */
		  switch (((struct Prj_Projects *) Projects)->SelectedOrder)
		    {
		     case Prj_ORDER_START_TIME:
		     case Prj_ORDER_END_TIME:
			// NumPrj: 1, 2, 3 ==> NumIndex = 3, 2, 1
			((struct Prj_Projects *) Projects)->NumIndex = ((struct Prj_Projects *) Projects)->Num + 1 - NumPrj;
			break;
		     default:
			// NumPrj: 1, 2, 3 ==> NumIndex = 1, 2, 3
			((struct Prj_Projects *) Projects)->NumIndex = NumPrj;
			break;
		    }

		  /* Show project */
		  ((struct Prj_Projects *) Projects)->View = Prj_LIST_PROJECTS;
		  Prj_ShowProjectRow ((struct Prj_Projects *) Projects);
		 }

	    /***** End table *****/
	    HTM_TABLE_End ();

	    /***** Free memory of the project *****/
	    Prj_FreeMemProject (&((struct Prj_Projects *) Projects)->Prj);

	    /***** Write again links to pages *****/
	    Pag_WriteLinksToPagesCentered (Pag_PROJECTS,&Pagination,
					   (struct Prj_Projects *) Projects,-1L);
	   }
	 else	// No projects created
	    Ale_ShowAlert (Ale_INFO,Txt_No_projects);

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
	 Set_BeginPref (Who == Projects->Filter.Who);
	    Frm_BeginForm (Who == Usr_WHO_SELECTED ? ActReqUsrPrj :
						     ActSeeAllPrj);
	       Filter.Who    = Who;
	       Filter.Assign = Projects->Filter.Assign;
	       Filter.Hidden = Projects->Filter.Hidden;
	       Filter.Faulti = Projects->Filter.Faulti;
	       Filter.Review = Projects->Filter.Review;
	       Filter.DptCod = Projects->Filter.DptCod;
	       Prj_PutPars (&Filter,
			    Projects->SelectedOrder,
			    Projects->CurrentPage,
			    -1L);
	       Usr_PutWhoIcon (Who);
	    Frm_EndForm ();
	 Set_EndPref ();
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
      Set_BeginPref ((Projects->Filter.Assign & (1 << Assign)));
	 Frm_BeginForm (ActSeeAllPrj);
	    Filter.Who    = Projects->Filter.Who;
	    Filter.Assign = Projects->Filter.Assign ^ (1 << Assign);	// Toggle
	    Filter.Hidden = Projects->Filter.Hidden;
	    Filter.Faulti = Projects->Filter.Faulti;
	    Filter.Review = Projects->Filter.Review;
	    Filter.DptCod = Projects->Filter.DptCod;
	    Prj_PutPars (&Filter,
			 Projects->SelectedOrder,
			 Projects->CurrentPage,
			 -1L);
	    Ico_PutSettingIconLink (AssignedNonassigIcon[Assign],Ico_BLACK,
				    Txt_PROJECT_ASSIGNED_NONASSIGNED_PLURAL[Assign]);
	 Frm_EndForm ();
      Set_EndPref ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************* Show form to select hidden / visible projects *****************/
/*****************************************************************************/

static void Prj_ShowFormToFilterByHidden (const struct Prj_Projects *Projects)
  {
   extern const char *Txt_PROJECT_HIDDEN_VISIBL_PROJECTS[HidVis_NUM_HIDDEN_VISIBLE];
   struct Prj_Filter Filter;
   HidVis_HiddenOrVisible_t HidVis;
   static const struct
     {
      const char *Icon;
      Ico_Color_t Color;
     } HiddenVisiblIcon[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = {"eye-slash.svg",Ico_RED  },
      [HidVis_VISIBLE] = {"eye.svg"      ,Ico_GREEN},
     };

   Set_BeginOneSettingSelector ();
   for (HidVis  = (HidVis_HiddenOrVisible_t) 0;
	HidVis <= (HidVis_HiddenOrVisible_t) (HidVis_NUM_HIDDEN_VISIBLE - 1);
	HidVis++)
     {
      Set_BeginPref ((Projects->Filter.Hidden & (1 << HidVis)));
	 Frm_BeginForm (ActSeeAllPrj);
	    Filter.Who    = Projects->Filter.Who;
	    Filter.Assign = Projects->Filter.Assign;
	    Filter.Hidden = Projects->Filter.Hidden ^ (1 << HidVis);	// Toggle
	    Filter.Faulti = Projects->Filter.Faulti;
	    Filter.Review = Projects->Filter.Review;
	    Filter.DptCod = Projects->Filter.DptCod;
	    Prj_PutPars (&Filter,
			 Projects->SelectedOrder,
			 Projects->CurrentPage,
			 -1L);
	    Ico_PutSettingIconLink (HiddenVisiblIcon[HidVis].Icon,
				    HiddenVisiblIcon[HidVis].Color,
				    Txt_PROJECT_HIDDEN_VISIBL_PROJECTS[HidVis]);
	 Frm_EndForm ();
      Set_EndPref ();
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
   struct
     {
      const char *Icon;
      Ico_Color_t Color;
     } FaultinessIcon[Prj_NUM_FAULTINESS] =
     {
      [Prj_FAULTY   ] = {"exclamation-triangle.svg",Ico_YELLOW},
      [Prj_FAULTLESS] = {"check-circle.svg"        ,Ico_GREEN },
     };

   Set_BeginOneSettingSelector ();
   for (Faultiness  = (Prj_Faultiness_t) 0;
	Faultiness <= (Prj_Faultiness_t) (Prj_NUM_FAULTINESS - 1);
	Faultiness++)
     {
      Set_BeginPref ((Projects->Filter.Faulti & (1 << Faultiness)));
	 Frm_BeginForm (ActSeeAllPrj);
	    Filter.Who    = Projects->Filter.Who;
	    Filter.Assign = Projects->Filter.Assign;
	    Filter.Hidden = Projects->Filter.Hidden;
	    Filter.Faulti = Projects->Filter.Faulti ^ (1 << Faultiness);	// Toggle
	    Filter.Review = Projects->Filter.Review;
	    Filter.DptCod = Projects->Filter.DptCod;
	    Prj_PutPars (&Filter,
			 Projects->SelectedOrder,
			 Projects->CurrentPage,
			 -1L);
	    Ico_PutSettingIconLink (FaultinessIcon[Faultiness].Icon,
				    FaultinessIcon[Faultiness].Color,
				    Txt_PROJECT_FAULTY_FAULTLESS_PROJECTS[Faultiness]);
	 Frm_EndForm ();
      Set_EndPref ();
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/********** Show form to select projects depending on review status **********/
/*****************************************************************************/

static void Prj_ShowFormToFilterByReview (const struct Prj_Projects *Projects)
  {
   extern const char *Txt_PROJECT_REVIEW_PLURAL[Prj_NUM_REVIEW_STATUS];
   struct Prj_Filter Filter;
   Prj_ReviewStatus_t ReviewStatus;

   Set_BeginOneSettingSelector ();
   for (ReviewStatus  = (Prj_ReviewStatus_t) 0;
	ReviewStatus <= (Prj_ReviewStatus_t) (Prj_NUM_REVIEW_STATUS - 1);
	ReviewStatus++)
     {
      Set_BeginPref ((Projects->Filter.Review & (1 << ReviewStatus)));
	 Frm_BeginForm (ActSeeAllPrj);
	    Filter.Who    = Projects->Filter.Who;
	    Filter.Assign = Projects->Filter.Assign;
	    Filter.Hidden = Projects->Filter.Hidden;
	    Filter.Faulti = Projects->Filter.Faulti;
	    Filter.Review = Projects->Filter.Review ^ (1 << ReviewStatus);	// Toggle
	    Filter.DptCod = Projects->Filter.DptCod;
	    Prj_PutPars (&Filter,
			 Projects->SelectedOrder,
			 Projects->CurrentPage,
			 -1L);
	    Ico_PutSettingIconLink (ReviewIcon[ReviewStatus].Icon,
				    ReviewIcon[ReviewStatus].Color,
				    Txt_PROJECT_REVIEW_PLURAL[ReviewStatus]);
	 Frm_EndForm ();
      Set_EndPref ();
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
   char *SelectClass;

   /***** Begin form *****/
   HTM_DIV_Begin (NULL);
      Frm_BeginForm (ActSeeAllPrj);
	 Filter.Who    = Projects->Filter.Who;
	 Filter.Assign = Projects->Filter.Assign;
	 Filter.Hidden = Projects->Filter.Hidden;
	 Filter.Faulti = Projects->Filter.Faulti;
	 Filter.Review = Projects->Filter.Review;
	 Filter.DptCod = Prj_FILTER_DPT_DEFAULT;	// Do not put department parameter here
	 Prj_PutPars (&Filter,
		      Projects->SelectedOrder,
		      Projects->CurrentPage,
		      -1L);

	 /***** Write selector with departments *****/
	 if (asprintf (&SelectClass,"Frm_C2_INPUT INPUT_%s",
	               The_GetSuffix ()) < 0)
	    Err_NotEnoughMemoryExit ();
	 Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Node[Hie_INS].HieCod,	// Departments in current institution
				      Projects->Filter.DptCod,			// Selected department
				      Prj_PAR_FILTER_DPT_COD_NAME,		// Parameter name
				      SelectClass,				// Selector class
				      -1L,					// First option
				      Txt_Any_department,			// Text when no department selected
				      HTM_SUBMIT_ON_CHANGE);
	 free (SelectClass);

      /***** End form *****/
      Frm_EndForm ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******************** Can I view files of a given project? *******************/
/*****************************************************************************/

static bool Prj_CheckIfICanViewProjectFiles (long PrjCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 return (Prj_GetMyRolesInProject (PrjCod) != 0);	// Am I a member?
      case Rol_TCH:	// Editing teachers in a course can access to all files
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/******** Check if I have permission to view project documents zone **********/
/*****************************************************************************/

bool Prj_CheckIfICanViewProjectDocuments (long PrjCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 return (Prj_GetMyRolesInProject (PrjCod) != 0);	// Am I a member?
      case Rol_TCH:	// Editing teachers in a course can access to all files
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/******** Check if I have permission to view project assessment zone *********/
/*****************************************************************************/

bool Prj_CheckIfICanViewProjectAssessment (long PrjCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 return ((Prj_GetMyRolesInProject (PrjCod) & (1 << Prj_ROLE_TUT |		// Tutor...
	                                              1 << Prj_ROLE_EVL)) != 0);	// ...or evaluator
      case Rol_TCH:	// Editing teachers in a course can access to all files
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

/*****************************************************************************/
/********************** Put parameters used in projects **********************/
/*****************************************************************************/

void Prj_PutCurrentPars (void *Projects)
  {
   if (Projects)
      Prj_PutPars (&((struct Prj_Projects *) Projects)->Filter,
		    ((struct Prj_Projects *) Projects)->SelectedOrder,
		    ((struct Prj_Projects *) Projects)->CurrentPage,
		    ((struct Prj_Projects *) Projects)->Prj.PrjCod);
  }

/* The following function is called
   when one or more parameters must be passed explicitely.
   Each parameter is passed only if its value is distinct to default. */

void Prj_PutPars (struct Prj_Filter *Filter,
                  Prj_Order_t Order,
                  unsigned NumPage,
                  long PrjCod)
  {
   /***** Put filter parameters (which projects to show) *****/
   if (Filter->Who != Prj_FILTER_WHO_DEFAULT)
      Usr_PutParWho (Filter->Who);

   if (Filter->Assign != ((unsigned) Prj_FILTER_ASSIGNED_DEFAULT |
	                  (unsigned) Prj_FILTER_NONASSIG_DEFAULT))
      Prj_PutParAssign (Filter->Assign);

   if (Filter->Hidden != ((unsigned) Prj_FILTER_HIDDEN_DEFAULT |
	                  (unsigned) Prj_FILTER_VISIBL_DEFAULT))
      Prj_PutParHidden (Filter->Hidden);

   if (Filter->Faulti != ((unsigned) Prj_FILTER_FAULTY_DEFAULT |
	                  (unsigned) Prj_FILTER_FAULTLESS_DEFAULT))
      Prj_PutParFaulti (Filter->Faulti);

   if (Filter->Review != ((unsigned) Prj_FILTER_UNREVIEWED_DEFAULT |
	                  (unsigned) Prj_FILTER_UNAPPROVED_DEFAULT |
	                  (unsigned) Prj_FILTER_APPROVED_DEFAULT))
      Prj_PutParReview (Filter->Review);

   if (Filter->DptCod != Prj_FILTER_DPT_DEFAULT)
      Prj_PutParFilterDptCod (Filter->DptCod);

   /***** Put order field *****/
   if (Order != Prj_ORDER_DEFAULT)
      Par_PutParUnsigned (NULL,"Order",(unsigned) Order);

   /***** Put number of page *****/
   if (NumPage > 1)
      Pag_PutParPagNum (Pag_PROJECTS,NumPage);

   /***** Put selected project code *****/
   ParCod_PutPar (ParCod_Prj,PrjCod);

   /***** Put another user's code *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);

   /***** Put selected users' codes *****/
   if (Filter->Who == Usr_WHO_SELECTED)
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/*********************** Put hidden params for projects **********************/
/*****************************************************************************/

static void Prj_PutParAssign (unsigned Assign)
  {
   Par_PutParUnsigned (NULL,Prj_PARAM_FILTER_PRE_NON_NAME,Assign);
  }

static void Prj_PutParHidden (unsigned Hidden)
  {
   Par_PutParUnsigned (NULL,Prj_PARAM_FILTER_HID_VIS_NAME,Hidden);
  }

static void Prj_PutParFaulti (unsigned Faulti)
  {
   Par_PutParUnsigned (NULL,Prj_PARAM_FILTER_FAULTIN_NAME,Faulti);
  }

static void Prj_PutParReview (unsigned Review)
  {
   Par_PutParUnsigned (NULL,Prj_PARAM_FILTER_REVIEW_NAME,Review);
  }

static void Prj_PutParFilterDptCod (long DptCod)
  {
   Par_PutParUnsigned (NULL,Prj_PAR_FILTER_DPT_COD_NAME,DptCod);
  }

/*****************************************************************************/
/*********************** Get hidden params for projects **********************/
/*****************************************************************************/

static void Prj_GetParPreNon (struct Prj_Projects *Projects)
  {
   Projects->Filter.Assign = (unsigned) Par_GetParUnsignedLong (Prj_PARAM_FILTER_PRE_NON_NAME,
                                                                0,
                                                                (1 << Prj_ASSIGNED) |
                                                                (1 << Prj_NONASSIG),
                                                                (unsigned) Prj_FILTER_ASSIGNED_DEFAULT |
                                                                (unsigned) Prj_FILTER_NONASSIG_DEFAULT);
  }

static unsigned Prj_GetParHidVis (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:	// Students can view only visible projects
	 return (unsigned) (1 << HidVis_VISIBLE);	// Only visible projects
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return (unsigned)
         Par_GetParUnsignedLong (Prj_PARAM_FILTER_HID_VIS_NAME,
				 0,
				 (1 << HidVis_HIDDEN) |
				 (1 << HidVis_VISIBLE),
				 (unsigned) Prj_FILTER_HIDDEN_DEFAULT |
				 (unsigned) Prj_FILTER_VISIBL_DEFAULT);
      default:
	 Err_WrongRoleExit ();
         return Prj_NEW_PRJ_HIDDEN_VISIBL_DEFAULT;	// Not reached
     }
  }

static unsigned Prj_GetParFaulti (void)
  {
   return (unsigned)
	  Par_GetParUnsignedLong (Prj_PARAM_FILTER_FAULTIN_NAME,
                                  0,
                                  (1 << Prj_FAULTY) |
                                  (1 << Prj_FAULTLESS),
                                  (unsigned) Prj_FILTER_FAULTY_DEFAULT |
                                  (unsigned) Prj_FILTER_FAULTLESS_DEFAULT);
  }

static unsigned Prj_GetParReview (void)
  {
   return (unsigned)
	  Par_GetParUnsignedLong (Prj_PARAM_FILTER_REVIEW_NAME,
                                  0,
                                  (1 << Prj_UNREVIEWED) |
                                  (1 << Prj_UNAPPROVED) |
                                  (1 << Prj_APPROVED),
                                  (unsigned) Prj_FILTER_UNREVIEWED_DEFAULT |
                                  (unsigned) Prj_FILTER_UNAPPROVED_DEFAULT |
                                  (unsigned) Prj_FILTER_APPROVED_DEFAULT);
  }

static long Prj_GetParFilterDptCod (void)
  {
   return Par_GetParLong (Prj_PAR_FILTER_DPT_COD_NAME);
  }

/*****************************************************************************/
/***************** Get generic parameters to list projects *******************/
/*****************************************************************************/

void Prj_GetPars (struct Prj_Projects *Projects)
  {
   /***** Get filter (which projects to show) *****/
   Projects->Filter.Who = Prj_GetParWho ();
   Prj_GetParPreNon (Projects);
   Projects->Filter.Hidden = Prj_GetParHidVis ();
   Projects->Filter.Faulti = Prj_GetParFaulti ();
   Projects->Filter.Review = Prj_GetParReview ();
   Projects->Filter.DptCod = Prj_GetParFilterDptCod ();

   /***** Get order and page *****/
   Projects->SelectedOrder = Prj_GetParPrjOrder ();
   Projects->CurrentPage = Pag_GetParPagNum (Pag_PROJECTS);

   /***** Get selected users *****/
   if (Projects->Filter.Who == Usr_WHO_SELECTED)
      Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
  }

/*****************************************************************************/
/************* Get parameter with whose users' projects to view **************/
/*****************************************************************************/

static Usr_Who_t Prj_GetParWho (void)
  {
   Usr_Who_t Who;

   /***** Get which users I want to see *****/
   Who = Usr_GetParWho ();

   /***** If parameter Who is unknown, set it to default *****/
   if (Who == Usr_WHO_UNKNOWN)
      Who = Prj_FILTER_WHO_DEFAULT;

   return Who;
  }

/*****************************************************************************/
/******************* Write header with fields of a project *******************/
/*****************************************************************************/

static void Prj_ShowProjectsHead (struct Prj_Projects *Projects)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_PROJECT_ORDER_HELP[Prj_NUM_ORDERS];
   extern const char *Txt_PROJECT_ORDER[Prj_NUM_ORDERS];
   Prj_Order_t Order;

   HTM_TR_Begin (NULL);

      /***** Column for number of project *****/
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TH (Txt_No_INDEX,HTM_HEAD_CENTER);
	    break;
	 default:
	    break;
	}

      /***** Column for contextual icons *****/
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
            HTM_TH_Span (NULL,HTM_HEAD_CENTER,1,1,"CONTEXT_COL");
	    break;
	 default:
	    break;
	}

      /***** Rest of columns *****/
      for (Order  = (Prj_Order_t) 0;
	   Order <= (Prj_Order_t) (Prj_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (HTM_HEAD_LEFT);

	    switch (Projects->View)
	      {
	       case Prj_LIST_PROJECTS:
	       case Prj_VIEW_ONE_PROJECT:
		  Frm_BeginForm (ActSeeAllPrj);
		     Prj_PutPars (&Projects->Filter,
				  Order,
				  Projects->CurrentPage,
				  -1L);
		     HTM_BUTTON_Submit_Begin (Txt_PROJECT_ORDER_HELP[Order],
		                              "class=\"BT_LINK\"");
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

   HTM_TR_End ();
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
	 HTM_TH (Txt_PROJECT_ORDER[Order],HTM_HEAD_LEFT);
      HTM_TH (Txt_Assigned_QUESTION ,HTM_HEAD_LEFT);
      HTM_TH (Txt_Number_of_students,HTM_HEAD_LEFT);
      for (NumRoleToShow = 0;
	   NumRoleToShow < Prj_NUM_ROLES_TO_SHOW;
	   NumRoleToShow++)
	 HTM_TH (Txt_PROJECT_ROLES_PLURAL_Abc[Prj_RolesToShow[NumRoleToShow]],HTM_HEAD_LEFT);
      HTM_TH (Txt_Proposal          ,HTM_HEAD_LEFT);
      HTM_TH (Txt_Description       ,HTM_HEAD_LEFT);
      HTM_TH (Txt_Required_knowledge,HTM_HEAD_LEFT);
      HTM_TH (Txt_Required_materials,HTM_HEAD_LEFT);
      HTM_TH (Txt_URL               ,HTM_HEAD_LEFT);
   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Check if I can create projects ***********************/
/*****************************************************************************/

static bool Prj_CheckIfICanCreateProjects (const struct Prj_Projects *Projects)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
         return Projects->Config.NETCanCreate;
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/***************** Put contextual icons in list of projects ******************/
/*****************************************************************************/

void Prj_PutIconsListProjects (void *Projects)
  {
   bool ICanConfigAllProjects;

   if (Projects)
     {
      ICanConfigAllProjects = PrjCfg_CheckIfICanConfig ();

      /***** Put icon to create a new project *****/
      if (Prj_CheckIfICanCreateProjects ((struct Prj_Projects *) Projects))
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
	 Ico_PutContextualIconToConfigure (ActCfgPrj,NULL,
	                                   NULL,NULL);

      /***** Link to get resource link *****/
      if (Rsc_CheckIfICanGetLink ())
	 Ico_PutContextualIconToGetLink (ActReqLnkPrj,NULL,
					 Prj_PutCurrentPars,Projects);

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_PROJECTS);
     }
  }

/*****************************************************************************/
/********************* Put icon to create a new project **********************/
/*****************************************************************************/

static void Prj_PutIconToCreateNewPrj (struct Prj_Projects *Projects)
  {
   Projects->Prj.PrjCod = -1L;
   Ico_PutContextualIconToAdd (ActFrmNewPrj,NULL,Prj_PutCurrentPars,Projects);
  }

/*****************************************************************************/
/******************** Put button to create a new project *********************/
/*****************************************************************************/

static void Prj_PutIconToShowAllData (struct Prj_Projects *Projects)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeeTblAllPrj,NULL,
                                  Prj_PutCurrentPars,Projects,
			          "table.svg",Ico_BLACK);
  }

/*****************************************************************************/
/************* Show a project followed by the list of projects ***************/
/*****************************************************************************/

void Prj_ShowOneProject (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get project data,
          then show project and (if possible) its file browser *****/
   Prj_AllocMemProject (&Projects.Prj);
   Prj_GetProjectDataByCod (&Projects.Prj);
   Prj_ShowBoxWithOneProject (&Projects);
   Prj_FreeMemProject (&Projects.Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/*** Show one project and (if possible) a file browser with its documents ****/
/*****************************************************************************/

void Prj_ShowBoxWithOneProject (struct Prj_Projects *Projects)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   extern const char *Txt_Project;

   /***** Begin box *****/
   Box_BoxBegin (Projects->Prj.Title[0] ? Projects->Prj.Title :
					  Txt_Project,
                 Prj_PutIconsOnePrj,Projects,
		 Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

      /***** Show project data *****/
      HTM_FIELDSET_Begin (NULL);
	 HTM_LEGEND (Txt_Project);
	 HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Table head and project *****/
	    Projects->View = Prj_VIEW_ONE_PROJECT;
	    Prj_ShowProjectsHead (Projects);
	    Prj_ShowProjectRow (Projects);

	 HTM_TABLE_End ();
      HTM_FIELDSET_End ();

      /***** Show project file browsers *****/
      if (Prj_CheckIfICanViewProjectFiles (Projects->Prj.PrjCod))
	 Brw_ShowFileBrowserProject (Projects->Prj.PrjCod);

      /***** Show project rubrics *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)	// TODO: Remove this rectriction when rubrics are finished
         Prj_ShowRubrics (Projects);

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****************** Put contextual icons in an assignment ********************/
/*****************************************************************************/

static void Prj_PutIconsOnePrj (void *Projects)
  {
   char *Anchor = NULL;

   if (Projects)
     {
      /***** Set anchor string *****/
      Frm_SetAnchorStr ((long) ((struct Prj_Projects *) Projects)->NumIndex,&Anchor);

      /***** Icons to remove/edit this project *****/
      Prj_PutIconsToRemEditOnePrj (Projects,Anchor);

      /***** Free anchor string *****/
      Frm_FreeAnchorStr (&Anchor);
     }
  }

/*****************************************************************************/
/********************** Show print view of one project ***********************/
/*****************************************************************************/

void Prj_PrintOneProject (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get project data *****/
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Write header *****/
   Lay_WriteHeaderClassPhoto (Vie_PRINT);

   /***** Begin table *****/
   HTM_TABLE_BeginWideMarginPadding (2);

      /***** Table head and project *****/
      Projects.View = Prj_PRINT_ONE_PROJECT;
      Prj_ShowProjectsHead (&Projects);
      Prj_ShowProjectRow (&Projects);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);
  }

/*****************************************************************************/
/*********** When listing projects, show one row with one project ************/
/*****************************************************************************/

static void Prj_ShowProjectRow (struct Prj_Projects *Projects)
  {
   extern const char *HidVis_LabelClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   struct Prj_Faults Faults;
   static unsigned UniqueId = 0;
   char *Anchor = NULL;

   /***** Set anchor string *****/
   Frm_SetAnchorStr ((long) Projects->NumIndex,&Anchor);

   /***** Check project faults *****/
   Prj_CheckIfPrjIsFaulty (Projects->Prj.PrjCod,&Faults);

   /***** First row with main data (dates, title...) *****/
   UniqueId++;
   Prj_ShowFirstRow (Projects,HidVis_DataClass[Projects->Prj.Hidden],
		     &Faults,UniqueId,Anchor);

   /***** Data always visible *****/
   Prj_ShowReviewStatus (Projects,
			 HidVis_LabelClass[Projects->Prj.Hidden],
			 HidVis_DataClass[Projects->Prj.Hidden],
			 &Faults,Anchor);
   Prj_ShowAssigned (Projects,
		     HidVis_LabelClass[Projects->Prj.Hidden],
		     HidVis_DataClass[Projects->Prj.Hidden],
		     &Faults);
   Prj_ShowNumStds (Projects,
		    HidVis_LabelClass[Projects->Prj.Hidden],
		    HidVis_DataClass[Projects->Prj.Hidden],
		    &Faults);
   Prj_ShowMembers (Projects);

   /***** Link to show hidden info *****/
   Prj_PutLinkToShowHiddenInfo (Projects,UniqueId);

   /***** Hiddeable data ******/
   Prj_ShowProposal (Projects,
		     HidVis_LabelClass[Projects->Prj.Hidden],
		     HidVis_DataClass[Projects->Prj.Hidden],UniqueId);
   Prj_ShowTxtField (Projects,
		     HidVis_LabelClass[Projects->Prj.Hidden],
		     HidVis_DataClass[Projects->Prj.Hidden],"prj_dsc_",UniqueId,
                     Txt_Description,		// Description of the project
                     Projects->Prj.Description,
                     Faults.WrongDescription ? Prj_PUT_WARNING :
                			       Prj_DONT_PUT_WARNING);
   Prj_ShowTxtField (Projects,
		     HidVis_LabelClass[Projects->Prj.Hidden],
		     HidVis_DataClass[Projects->Prj.Hidden],"prj_knw_",UniqueId,
                     Txt_Required_knowledge,	// Required knowledge
                     Projects->Prj.Knowledge,
		     Prj_DONT_PUT_WARNING);
   Prj_ShowTxtField (Projects,
		     HidVis_LabelClass[Projects->Prj.Hidden],
		     HidVis_DataClass[Projects->Prj.Hidden],"prj_mtr_",UniqueId,
                     Txt_Required_materials,	// Required materials
                     Projects->Prj.Materials,
		     Prj_DONT_PUT_WARNING);
   Prj_ShowURL (Projects,
	        HidVis_LabelClass[Projects->Prj.Hidden],
	        HidVis_DataClass[Projects->Prj.Hidden],"prj_url_",UniqueId);

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (&Anchor);
  }

/*****************************************************************************/
/** When listing a project, show first row with main data (dates, title...) **/
/*****************************************************************************/

static void Prj_ShowFirstRow (struct Prj_Projects *Projects,
                              const char *ClassData,
                              const struct Prj_Faults *Faults,
                              unsigned UniqueId,
                              const char *Anchor)
  {
   extern const char *HidVis_DateBlueClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_TitleClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   char *Id;
   Act_Action_t NextAction;

   /***** Write first row of data of this project *****/
   HTM_TR_Begin (NULL);

      /* Number of project */
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("rowspan=\"4\" class=\"RT BIG_INDEX_%s %s\"",
	                  The_GetSuffix (),
			  The_GetColorRows ());
	       HTM_Unsigned (Projects->NumIndex);
	       if (Faults->PrjIsFaulty)
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
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("rowspan=\"4\" class=\"CONTEXT_COL %s\"",
	                  The_GetColorRows ());
	       Prj_PutIconsToRemEditOnePrj (Projects,Anchor);
	    HTM_TD_End ();
	    break;
	 default:
	    break;
	}

      /* Creation date/time */
      if (asprintf (&Id,"prj_creat_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			  Id,
			  HidVis_DateBlueClass[Projects->Prj.Hidden],The_GetSuffix (),
			  The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
			  Id,
			  HidVis_DateBlueClass[Projects->Prj.Hidden],The_GetSuffix ());
	    break;
	}
	 Dat_WriteLocalDateHMSFromUTC (Id,Projects->Prj.CreatTime,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,true,0x7);
      HTM_TD_End ();
      free (Id);

      /* Modification date/time */
      UniqueId++;
      if (asprintf (&Id,"prj_modif_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s %s\"",
			  Id,
			  HidVis_DateBlueClass[Projects->Prj.Hidden],The_GetSuffix (),
			  The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("id=\"%s\" class=\"LT %s_%s\"",
			  Id,
			  HidVis_DateBlueClass[Projects->Prj.Hidden],The_GetSuffix ());
	    break;
	}
	 Dat_WriteLocalDateHMSFromUTC (Id,Projects->Prj.ModifTime,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,true,0x7);
      HTM_TD_End ();
      free (Id);

      /* Project title */
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("class=\"LT\"");
	    break;
	}
	 HTM_ARTICLE_Begin (Anchor);
	    if (Projects->Prj.Title[0])
	      {
	       NextAction = Prj_CheckIfICanViewProjectFiles (Projects->Prj.PrjCod) ? ActAdmDocPrj :
										     ActSeeOnePrj;
	       Frm_BeginForm (NextAction);
		  Prj_PutCurrentPars (Projects);
		  HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
					   "class=\"LT BT_LINK %s_%s\"",
					   HidVis_TitleClass[Projects->Prj.Hidden],
					   The_GetSuffix ());
		     HTM_Txt (Projects->Prj.Title);
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	      }
	    if (Faults->WrongTitle)
	       Prj_PutWarningIcon ();
	 HTM_ARTICLE_End ();
      HTM_TD_End ();

      /* Department */
      Prj_ShowProjectDepartment (Projects,ClassData);

   HTM_TR_End ();
  }

/*****************************************************************************/
/********** When listing a project, show first row with department ***********/
/*****************************************************************************/

static void Prj_ShowProjectDepartment (const struct Prj_Projects *Projects,
                                       const char *ClassData)
  {
   struct Dpt_Department Dpt;
   bool PutLinkToDpt;

   /***** Get data of department *****/
   Dpt.DptCod = Projects->Prj.DptCod;
   Dpt_GetDepartmentDataByCod (&Dpt);

   /***** Show department *****/
   PutLinkToDpt = (Dpt.WWW[0] &&
	           (Projects->View == Prj_LIST_PROJECTS ||
	            Projects->View == Prj_VIEW_ONE_PROJECT));

   switch (Projects->View)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TD_Begin ("class=\"LT %s_%s %s\"",
	               ClassData,The_GetSuffix (),The_GetColorRows ());
	 break;
      default:
	 HTM_TD_Begin ("class=\"LT %s_%s\"",
	               ClassData,The_GetSuffix ());
	 break;
     }

      if (PutLinkToDpt)
	 HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"%s_%s\"",
		      Dpt.WWW,ClassData,The_GetSuffix ());
      HTM_Txt (Dpt.FullName);
      if (PutLinkToDpt)
	 HTM_A_End ();

   HTM_TD_End ();
  }

/*****************************************************************************/
/********* When listing a project, show one row with review status ***********/
/*****************************************************************************/

static void Prj_ShowReviewStatus (struct Prj_Projects *Projects,
                                  const char *ClassLabel,
                                  const char *ClassData,
                                  const struct Prj_Faults *Faults,
                                  const char *Anchor)
  {
   extern const char *Txt_Review;
   extern const char *Txt_PROJECT_REVIEW_SINGUL[Prj_NUM_REVIEW_STATUS];
   extern const char *Txt_Comments;
   extern const char *Txt_Save_changes;
   Frm_PutForm_t PutForm;
   static unsigned UniqueId = 0;
   char *Id;

   /***** Check if put form to change review *****/
   switch (Projects->View)
     {
      case Prj_PRINT_ONE_PROJECT:
	 PutForm = Frm_DONT_PUT_FORM;
	 break;
      default:
	 PutForm = Prj_CheckIfICanReviewProjects () ? Frm_PUT_FORM :
						      Frm_DONT_PUT_FORM;
	 break;
     }

   HTM_TR_Begin (NULL);

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s %s\"",
			  ClassLabel,The_GetSuffix (),The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
			  ClassLabel,The_GetSuffix ());
	    break;
	}
	 HTM_TxtColon (Txt_Review);
      HTM_TD_End ();

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s %s\"",
			  ClassData,The_GetSuffix (),The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s\"",
			  ClassData,The_GetSuffix ());
	    break;
	}

      switch (PutForm)
	{
	 case Frm_DONT_PUT_FORM:
	    HTM_Txt (Txt_PROJECT_REVIEW_SINGUL[Projects->Prj.Review.Status]);
	    break;
	 case Frm_PUT_FORM:
	    /***** Begin form to change review status and text *****/
	    Frm_BeginFormAnchor (ActChgPrjRev,Anchor);
	       Prj_PutCurrentPars (Projects);

	       /***** Selector to change review status *****/
	       Prj_PutSelectorReviewStatus (Projects);
	    break;
	}

      HTM_NBSP ();
      Ico_PutIconOff (ReviewIcon[Projects->Prj.Review.Status].Icon,
		      ReviewIcon[Projects->Prj.Review.Status].Color,
		      Txt_PROJECT_REVIEW_SINGUL[Projects->Prj.Review.Status]);

      /***** Show warning icon depending on review status *****/
      if (Faults->WrongReviewStatus)
	 Prj_PutWarningIcon ();

      if (Projects->Prj.Review.Status != Prj_UNREVIEWED)
	{
         /***** Revision time *****/
         HTM_NBSP ();
	 UniqueId++;
	 if (asprintf (&Id,"prj_date_%u",UniqueId) < 0)
	    Err_NotEnoughMemoryExit ();
	 HTM_DIV_Begin ("id=\"%s\" class=\"PRJ_DATE\"",Id);
	    Dat_WriteLocalDateHMSFromUTC (Id,Projects->Prj.Review.Time,
					  Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
					  true,true,false,0x6);
	 HTM_DIV_End ();
	 free (Id);
	}

      /***** Show warning icon depending on modify time *****/
      if (Faults->WrongModifTime)
	 Prj_PutWarningIcon ();

      /***** Revision text *****/
      switch (PutForm)
	{
	 case Frm_DONT_PUT_FORM:
	    if (Projects->Prj.Review.Txt[0])
	      {
	       /* Change text format */
	       Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				 Projects->Prj.Review.Txt,Cns_MAX_BYTES_TEXT,
				 Str_DONT_REMOVE_SPACES);
	       switch (Projects->View)
		 {
		  case Prj_PRINT_ONE_PROJECT:
		     break;
		  default:
		     ALn_InsertLinks (Projects->Prj.Review.Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
		     break;
		 }

	       /* Show text */
	       HTM_BR ();
	       HTM_Txt (Projects->Prj.Review.Txt);
	      }
	    break;
	 case Frm_PUT_FORM:
	       /* Show text form */
	       HTM_BR ();
	       HTM_TEXTAREA_Begin ("name=\"ReviewTxt\" rows=\"2\""
				   " class=\"Frm_C2_INPUT INPUT_%s\""
				   " placeholder=\"%s&hellip;\""
				   " onchange=\"unhideElement('prj_rev_%ld');return false;\"",
				   The_GetSuffix (),Txt_Comments,
				   Projects->Prj.PrjCod);
		  HTM_Txt (Projects->Prj.Review.Txt);
	       HTM_TEXTAREA_End ();

	       /* Button to save changes.
		  Initially hidden, is shown when clicking on selector or text */
	       HTM_DIV_Begin ("id=\"prj_rev_%ld\" style=\"display:none;\"",
			      Projects->Prj.PrjCod);
		  Btn_PutConfirmButtonInline (Txt_Save_changes);
	       HTM_DIV_End ();

	    /* End form */
	    Frm_EndForm ();
	    break;
	}

      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Form to lock/unlock project edition ********************/
/*****************************************************************************/

static void Prj_PutSelectorReviewStatus (struct Prj_Projects *Projects)
  {
   extern const char *Txt_PROJECT_REVIEW_SINGUL[Prj_NUM_REVIEW_STATUS];
   Prj_ReviewStatus_t ReviewStatus;
   unsigned ReviewStatusUnsigned;
   char *FuncOnChange;

   /* Selector for review status */
   if (asprintf (&FuncOnChange,"unhideElement('prj_rev_%ld');",Projects->Prj.PrjCod) < 0)
      Err_NotEnoughMemoryExit ();
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,FuncOnChange,
		     "id=\"ReviewStatus\" name=\"ReviewStatus\""
		     " class=\"Frm_C2_INPUT INPUT_%s\"",
		     The_GetSuffix ());
   free (FuncOnChange);
      for (ReviewStatus  = (Prj_ReviewStatus_t) 0;
	   ReviewStatus <= (Prj_ReviewStatus_t) (Prj_NUM_REVIEW_STATUS - 1);
	   ReviewStatus++)
	{
	 ReviewStatusUnsigned = (unsigned) ReviewStatus;
	 HTM_OPTION (HTM_Type_UNSIGNED,&ReviewStatusUnsigned,
		     ReviewStatus == Projects->Prj.Review.Status ? HTM_OPTION_SELECTED :
								   HTM_OPTION_UNSELECTED,
		     HTM_OPTION_ENABLED,
		     "%s",Txt_PROJECT_REVIEW_SINGUL[ReviewStatus]);
	}
   HTM_SELECT_End ();
  }

/*****************************************************************************/
/**************************** Can I review projects? *************************/
/*****************************************************************************/

static bool Prj_CheckIfICanReviewProjects (void)
  {
   static bool ICanReviewProjects[Rol_NUM_ROLES] =
     {
      [Rol_TCH    ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanReviewProjects[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/******** When listing a project, show one row with assigned status **********/
/*****************************************************************************/

static void Prj_ShowAssigned (const struct Prj_Projects *Projects,
                              const char *ClassLabel,
                              const char *ClassData,
                              const struct Prj_Faults *Faults)
  {
   extern const char *Txt_Assigned_QUESTION;
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_PROJECT_ASSIGNED_NONASSIGNED_SINGUL[Prj_NUM_ASSIGNED_NONASSIG];

   HTM_TR_Begin (NULL);

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s %s\"",
			  ClassLabel,The_GetSuffix (),The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
			  ClassLabel,The_GetSuffix ());
	    break;
	}
	 HTM_TxtColon (Txt_Assigned_QUESTION);
      HTM_TD_End ();

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s %s\"",
			  ClassData,The_GetSuffix (),The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s\"",
			  ClassData,The_GetSuffix ());
	    break;
	}
	 HTM_TxtF ("%s&nbsp;",Projects->Prj.Assigned == Prj_ASSIGNED ? Txt_Yes :
								       Txt_No);
	 Ico_PutIconOff (AssignedNonassigIcon[Projects->Prj.Assigned],Ico_BLACK,
			 Txt_PROJECT_ASSIGNED_NONASSIGNED_SINGUL[Projects->Prj.Assigned]);

	 if (Faults->WrongAssigned)
	    Prj_PutWarningIcon ();

      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/******* When listing a project, show one row with number of students ********/
/*****************************************************************************/

static void Prj_ShowNumStds (const struct Prj_Projects *Projects,
                             const char *ClassLabel,
                             const char *ClassData,
                             const struct Prj_Faults *Faults)
  {
   extern const char *Txt_Number_of_students;

   HTM_TR_Begin (NULL);

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s %s\"",
			  ClassLabel,The_GetSuffix (),The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
			  ClassLabel,The_GetSuffix ());
	    break;
	}
	 HTM_TxtColon (Txt_Number_of_students);
      HTM_TD_End ();

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s %s\"",
			  ClassData,The_GetSuffix (),The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s\"",
			  ClassData,The_GetSuffix ());
	    break;
	}
	 HTM_Unsigned (Projects->Prj.NumStds);
	 if (Faults->WrongNumStds)
	    Prj_PutWarningIcon ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/****** When listing a project, show several rows with projects members ******/
/*****************************************************************************/

static void Prj_ShowMembers (struct Prj_Projects *Projects)
  {
   unsigned NumRoleToShow;

   for (NumRoleToShow = 0;
	NumRoleToShow < Prj_NUM_ROLES_TO_SHOW;
	NumRoleToShow++)
      Prj_ShowProjectMembersWithARole (Projects,Prj_RolesToShow[NumRoleToShow]);
  }

/*****************************************************************************/
/*** When listing a project, show one row with projects members with a role **/
/*****************************************************************************/

static void Prj_ShowProjectMembersWithARole (struct Prj_Projects *Projects,
                                             Prj_RoleInProject_t RoleInPrj)
  {
   extern const char *HidVis_LabelClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_MsgClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_PROJECT_ROLES_SINGUL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_PROJECT_ROLES_PLURAL_Abc[Prj_NUM_ROLES_IN_PROJECT];
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
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool WriteRow;
   unsigned NumUsr;
   unsigned NumUsrs;

   /***** Get users in project from database *****/
   NumUsrs = Prj_DB_GetUsrsInPrj (&mysql_res,Projects->Prj.PrjCod,RoleInPrj);
   WriteRow = (NumUsrs != 0 ||
	       Projects->View == Prj_EDIT_ONE_PROJECT);

   if (WriteRow)
     {
      /***** Begin row with label and listing of users *****/
      HTM_TR_Begin (NULL);

	 /* Column for label */
	 switch (Projects->View)
	   {
	    case Prj_LIST_PROJECTS:
	       HTM_TD_Begin ("colspan=\"4\" class=\"RT %s_%s %s\"",
			     HidVis_LabelClass[Projects->Prj.Hidden],
			     The_GetSuffix (),The_GetColorRows ());
		  HTM_TxtColon (NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInPrj] :
					       Txt_PROJECT_ROLES_PLURAL_Abc[RoleInPrj]);
	       break;
	    case Prj_VIEW_ONE_PROJECT:
	    case Prj_PRINT_ONE_PROJECT:
	       HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
			     HidVis_LabelClass[Projects->Prj.Hidden],
			     The_GetSuffix ());
		  HTM_TxtColon (NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInPrj] :
					       Txt_PROJECT_ROLES_PLURAL_Abc[RoleInPrj]);
	       break;
	    case Prj_EDIT_ONE_PROJECT:
	       HTM_TD_Begin ("class=\"RT %s_%s\"",
			     HidVis_LabelClass[Projects->Prj.Hidden],
			     The_GetSuffix ());
		  HTM_TxtColon (Txt_PROJECT_ROLES_PLURAL_Abc[RoleInPrj]);
	       break;
	   }
	 HTM_TD_End ();

	 /* Begin column with list of users */
	 switch (Projects->View)
	   {
	    case Prj_LIST_PROJECTS:
	       HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s %s\"",
			     HidVis_DataClass[Projects->Prj.Hidden],
			     The_GetSuffix (),
			     The_GetColorRows ());
	       break;
	    case Prj_VIEW_ONE_PROJECT:
	    case Prj_PRINT_ONE_PROJECT:
	       HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s\"",
			     HidVis_DataClass[Projects->Prj.Hidden],
			     The_GetSuffix ());
	       break;
	    case Prj_EDIT_ONE_PROJECT:
	       HTM_TD_Begin ("colspan=\"2\" class=\"LT DAT_%s\"",
	                     The_GetSuffix ());
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
							       Usr_DONT_GET_ROLE_IN_CRS))
		    {
		     /* Begin row for this user */
		     HTM_TR_Begin (NULL);

			/* Icon to remove user */
			if (Projects->View == Prj_EDIT_ONE_PROJECT)
			  {
			   HTM_TD_Begin ("class=\"PRJ_MEMBER_ICO\"");
			      Lay_PutContextualLinkOnlyIcon (ActionReqRemUsr[RoleInPrj],NULL,
							     Prj_PutCurrentPars,Projects,
							     "trash.svg",Ico_RED);
			   HTM_TD_End ();
			  }

			/* Put user's photo */
			HTM_TD_Begin ("class=\"PRJ_MEMBER_PHO\"");
			   Pho_ShowUsrPhotoIfAllowed (&Gbl.Usrs.Other.UsrDat,
			                              ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
			HTM_TD_End ();

			/* Write user's name */
			HTM_TD_Begin ("class=\"LM %s_%s\"",
				      HidVis_MsgClass[Projects->Prj.Hidden],
			              The_GetSuffix ());
			   HTM_Txt (Gbl.Usrs.Other.UsrDat.FullName);
			HTM_TD_End ();

		     /* End row for this user */
		     HTM_TR_End ();
		    }
		 }

	       /***** Row to add a new user *****/
	       switch (Projects->View)
		 {
		  case Prj_EDIT_ONE_PROJECT:
		     HTM_TR_Begin (NULL);
			HTM_TD_Begin ("class=\"PRJ_MEMBER_ICO\"");
			   Ico_PutContextualIconToAdd (ActionReqAddUsr[RoleInPrj],NULL,
						       Prj_PutCurrentPars,Projects);
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"PRJ_MEMBER_PHO\"");	// Column for photo
			HTM_TD_End ();

			HTM_TD_Begin ("class=\"LM MSG_AUT_%s\"",	// Column for name
			              The_GetSuffix ());
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

/*****************************************************************************/
/***** When listing a project, show one row with link to show hidden info ****/
/*****************************************************************************/

static void Prj_PutLinkToShowHiddenInfo (const struct Prj_Projects *Projects,
                                         unsigned UniqueId)
  {
   extern const char *Txt_See_more;
   extern const char *Txt_See_less;

   switch (Projects->View)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"prj_exp_%u\"",UniqueId);
	    HTM_TD_Begin ("colspan=\"6\" class=\"CM %s\"",
	                  The_GetColorRows ());
	       Prj_PutIconToToggleProject (UniqueId,"angle-down.svg",Txt_See_more);
	    HTM_TD_End ();
	 HTM_TR_End ();

	 HTM_TR_Begin ("id=\"prj_con_%u\" style=\"display:none;\"",UniqueId);
	    HTM_TD_Begin ("colspan=\"6\" class=\"CM %s\"",
	                  The_GetColorRows ());
	       Prj_PutIconToToggleProject (UniqueId,"angle-up.svg",Txt_See_less);
	    HTM_TD_End ();
	 HTM_TR_End ();
	 break;
      case Prj_VIEW_ONE_PROJECT:
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
  }

/*****************************************************************************/
/********* When listing a project, show one row with type of proposal ********/
/*****************************************************************************/

static void Prj_ShowProposal (const struct Prj_Projects *Projects,
                              const char *ClassLabel,
                              const char *ClassData,
                              unsigned UniqueId)
  {
   extern const char *Txt_Proposal;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];

   switch (Projects->View)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"prj_pro_%u\" style=\"display:none;\"",UniqueId);
	    HTM_TD_Begin ("colspan=\"4\" class=\"RT %s_%s %s\"",
			  ClassLabel,The_GetSuffix (),The_GetColorRows ());
	 break;
      case Prj_VIEW_ONE_PROJECT:
	 HTM_TR_Begin ("id=\"prj_pro_%u\" style=\"display:none;\"",UniqueId);
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
	                  ClassLabel,The_GetSuffix ());
	 break;
      default:
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
	                  ClassLabel,The_GetSuffix ());
	 break;
     }
	 HTM_TxtColon (Txt_Proposal);
      HTM_TD_End ();

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s %s\"",
			  ClassData,The_GetSuffix (),
			  The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s\"",
			  ClassData,The_GetSuffix ());
	    break;
	}
	 HTM_Txt (Txt_PROJECT_STATUS[Projects->Prj.Proposal]);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********** When listing a project, show one row with a text field ***********/
/*****************************************************************************/

static void Prj_ShowTxtField (const struct Prj_Projects *Projects,
                              const char *ClassLabel,
                              const char *ClassData,
                              const char *id,unsigned UniqueId,
                              const char *Label,char *TxtField,
			      Prj_Warning_t Warning)
  {
   /***** Label *****/
   switch (Projects->View)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	    HTM_TD_Begin ("colspan=\"4\" class=\"RT %s_%s %s\"",
	                  ClassLabel,The_GetSuffix (),The_GetColorRows ());
	 break;
      case Prj_VIEW_ONE_PROJECT:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
	                  ClassLabel,The_GetSuffix ());
	 break;
      case Prj_PRINT_ONE_PROJECT:
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
	                  ClassLabel,The_GetSuffix ());
	 break;
      default:
	 // Not applicable
	 break;
     }
	 HTM_TxtColon (Label);
      HTM_TD_End ();

      /***** Change text format *****/
      Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			TxtField,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	 case Prj_VIEW_ONE_PROJECT:
	    ALn_InsertLinks (TxtField,Cns_MAX_BYTES_TEXT,60);	// Insert links
	    break;
	 default:
	    break;
	}

      /***** Text *****/
      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s %s\"",
			  ClassData,The_GetSuffix (),
			  The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s\"",
			  ClassData,The_GetSuffix ());
	    break;
	}
	 HTM_Txt (TxtField);
	 if (Warning == Prj_PUT_WARNING)
	    Prj_PutWarningIcon ();
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** When listing a project, show one row with URL ****************/
/*****************************************************************************/

static void Prj_ShowURL (const struct Prj_Projects *Projects,
                         const char *ClassLabel,
                         const char *ClassData,
                         const char *id,unsigned UniqueId)
  {
   extern const char *Txt_URL;
   bool PutLinkToURL = (Projects->Prj.URL[0] &&
	                (Projects->View == Prj_LIST_PROJECTS ||
	                 Projects->View == Prj_VIEW_ONE_PROJECT));

   /***** Write row with label and text *****/
   switch (Projects->View)
     {
      case Prj_LIST_PROJECTS:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	    HTM_TD_Begin ("colspan=\"4\" class=\"RT %s_%s %s\"",
	                  ClassLabel,The_GetSuffix (),The_GetColorRows ());
	 break;
      case Prj_VIEW_ONE_PROJECT:
	 HTM_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
	                  ClassLabel,The_GetSuffix ());
	 break;
      case Prj_PRINT_ONE_PROJECT:
	 HTM_TR_Begin (NULL);
	    HTM_TD_Begin ("colspan=\"2\" class=\"RT %s_%s\"",
	                  ClassLabel,The_GetSuffix ());
	 break;
      default:
	 // Not applicable
	 break;
     }
	 HTM_TxtColon (Txt_URL);
      HTM_TD_End ();

      switch (Projects->View)
	{
	 case Prj_LIST_PROJECTS:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s %s\"",
			  ClassData,The_GetSuffix (),
			  The_GetColorRows ());
	    break;
	 default:
	    HTM_TD_Begin ("colspan=\"2\" class=\"LT %s_%s\"",
			  ClassData,The_GetSuffix ());
	    break;
	}

	 if (PutLinkToURL)
	    HTM_A_Begin ("href=\"%s\" target=\"_blank\"",Projects->Prj.URL);
	 HTM_Txt (Projects->Prj.URL);
	 if (PutLinkToURL)
	    HTM_A_End ();

      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********************** Check if a project has faults ************************/
/*****************************************************************************/

static void Prj_CheckIfPrjIsFaulty (long PrjCod,struct Prj_Faults *Faults)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool IsAssigned;
   bool HasTitle;
   bool HasDescription;
   bool IsUnapproved;
   bool ModifiedAfterReview;
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
      if (Prj_DB_GetPrjDataToCheckFaults (&mysql_res,PrjCod))	// Project found...
	{
         /***** Get some data of project *****/
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get if project is assigned or not (row[0]) */
	 IsAssigned = (row[0][0] != '0');

	 /* Get number of proposed students (row[1]) */
	 NumProposedStds = Str_ConvertStrToUnsigned (row[1]);

	 /* Get if title is not empty (row[2])
	    and if description is not empty (row[3]) */
	 HasTitle       = (row[2][0] != '0');
	 HasDescription = (row[3][0] != '0');

	 /* Get if project is unnaproved (row[4]) */
	 IsUnapproved = (row[4][0] != '0');

	 /* Get if project has been modified after review (row[5]) */
	 ModifiedAfterReview = (row[5][0] != '0');

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
	    NumStdsRegisteredInPrj = Prj_DB_GetNumUsrsInPrj (PrjCod,Prj_ROLE_STD);
	    if (IsAssigned)		// Assigned
	       // In an assigned project the number of proposed students...
	       // ...should match the number of students registered in it
	       Faults->WrongNumStds = (NumProposedStds != NumStdsRegisteredInPrj);
	    else			// Not assigned
	       // A non assigned project should not have students registered in it
	       Faults->WrongAssigned = (NumStdsRegisteredInPrj != 0);
	   }

	 /* 4. Check review status */
	 Faults->WrongReviewStatus = IsUnapproved;
	 Faults->WrongModifTime    = ModifiedAfterReview;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   Faults->PrjIsFaulty = Faults->WrongTitle        ||
	                 Faults->WrongDescription  ||
	                 Faults->WrongNumStds      ||
	                 Faults->WrongAssigned     ||
	                 Faults->WrongReviewStatus ||
	                 Faults->WrongModifTime;
  }

/*****************************************************************************/
/************** Put an icon to warn about a fault in a project ***************/
/*****************************************************************************/

static void Prj_PutWarningIcon (void)
  {
   Ico_PutIcon ("warning64x64.gif",Ico_UNCHANGED,"","ICO16x16");
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off some fields of a project ************/
/*****************************************************************************/

static void Prj_PutIconToToggleProject (unsigned UniqueId,
                                        const char *Icon,const char *Text)
  {
   /***** Link to toggle on/off some fields of project *****/
   HTM_A_Begin ("href=\"\" title=\"%s\" class=\"FORM_IN_%s\""
                " onclick=\"toggleProject('%u');return false;\"",
               Text,The_GetSuffix (),
               UniqueId);
      Ico_PutIconTextLink (Icon,Ico_BLACK,Text);
   HTM_A_End ();
  }

/*****************************************************************************/
/***************** Show one row of table with all projects *******************/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsOneRow (struct Prj_Project *Prj)
  {
   extern const char *HidVis_DateBlueClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];
   unsigned NumRoleToShow;
   static unsigned UniqueId = 0;
   char *Id;

   /***** Get data of this project *****/
   Prj_GetProjectDataByCod (Prj);

   /***** Begin row *****/
   HTM_TR_Begin (NULL);

      /***** Start date/time *****/
      UniqueId++;
      if (asprintf (&Id,"prj_creat_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LT %s %s\"",
		    Id,HidVis_DateBlueClass[Prj->Hidden],The_GetColorRows ());
	 Dat_WriteLocalDateHMSFromUTC (Id,Prj->CreatTime,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,false,0x7);
      HTM_TD_End ();
      free (Id);

      /***** End date/time *****/
      UniqueId++;
      if (asprintf (&Id,"prj_modif_%u",UniqueId) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_TD_Begin ("id=\"%s\" class=\"LT %s %s\"",
		    Id,HidVis_DateBlueClass[Prj->Hidden],The_GetColorRows ());
	 Dat_WriteLocalDateHMSFromUTC (Id,Prj->ModifTime,
				       Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				       true,true,false,0x7);
      HTM_TD_End ();
      free (Id);

      /***** Project title *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    HidVis_DataClass[Prj->Hidden],The_GetSuffix (),The_GetColorRows ());
	 HTM_Txt (Prj->Title);
      HTM_TD_End ();

      /***** Department *****/
      Prj_ShowTableAllProjectsDepartment (Prj);

      /***** Assigned? *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    HidVis_DataClass[Prj->Hidden],The_GetSuffix (),
		    The_GetColorRows ());
	 HTM_Txt ((Prj->Assigned == Prj_ASSIGNED) ? Txt_Yes :
						    Txt_No);
      HTM_TD_End ();

      /***** Number of students *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    HidVis_DataClass[Prj->Hidden],The_GetSuffix (),
		    The_GetColorRows ());
	 HTM_Unsigned (Prj->NumStds);
      HTM_TD_End ();

      /***** Project members *****/
      for (NumRoleToShow = 0;
	   NumRoleToShow < Prj_NUM_ROLES_TO_SHOW;
	   NumRoleToShow++)
	 Prj_ShowTableAllProjectsMembersWithARole (Prj,Prj_RolesToShow[NumRoleToShow]);

      /***** Proposal *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		    HidVis_DataClass[Prj->Hidden],The_GetSuffix (),
		    The_GetColorRows ());
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
  }

/*****************************************************************************/
/****************** Show department associated to project ********************/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsDepartment (const struct Prj_Project *Prj)
  {
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   struct Dpt_Department Dpt;

   /***** Get data of department *****/
   Dpt.DptCod = Prj->DptCod;
   Dpt_GetDepartmentDataByCod (&Dpt);

   /***** Show department *****/
   HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		 HidVis_DataClass[Prj->Hidden],The_GetSuffix (),
		 The_GetColorRows ());
      HTM_Txt (Dpt.FullName);
   HTM_TD_End ();
  }

/*****************************************************************************/
/*********************** Show text field in a project ************************/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsTxtField (struct Prj_Project *Prj,
                                              char *TxtField)
  {
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Change format *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     TxtField,Cns_MAX_BYTES_TEXT,Str_DONT_REMOVE_SPACES);

   /***** Write text *****/
   HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		 HidVis_DataClass[Prj->Hidden],The_GetSuffix (),
		 The_GetColorRows ());
      HTM_Txt (TxtField);
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************** Show URL associated to project ***********************/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsURL (const struct Prj_Project *Prj)
  {
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];

   /***** Show URL *****/
   HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		 HidVis_DataClass[Prj->Hidden],The_GetSuffix (),
		 The_GetColorRows ());
      HTM_Txt (Prj->URL);
   HTM_TD_End ();
  }

/*****************************************************************************/
/***** Show table cell for members with a role in table with all projects ****/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsMembersWithARole (const struct Prj_Project *Prj,
                                                      Prj_RoleInProject_t RoleInPrj)
  {
   extern const char *HidVis_DataClass[HidVis_NUM_HIDDEN_VISIBLE];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned NumUsrs;

   /***** Get users in project from database *****/
   NumUsrs = Prj_DB_GetUsrsInPrj (&mysql_res,Prj->PrjCod,RoleInPrj);

   /***** Begin column with list of all members with this role *****/
   HTM_TD_Begin ("class=\"LT %s_%s %s\"",
		 HidVis_DataClass[Prj->Hidden],The_GetSuffix (),
		 The_GetColorRows ());

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
							    Usr_DONT_GET_ROLE_IN_CRS))
		 {
		  /* Write user's name in "Surname1 Surname2, FirstName" format */
		  HTM_LI_Begin (NULL);
		     HTM_Txt (Gbl.Usrs.Other.UsrDat.Surname1);
		     if (Gbl.Usrs.Other.UsrDat.Surname2[0])
			HTM_SPTxt (Gbl.Usrs.Other.UsrDat.Surname2);
		     HTM_Comma ();
		     HTM_SPTxt (Gbl.Usrs.Other.UsrDat.FrstName);
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
/************************** Get my role in a project *************************/
/*****************************************************************************/

void Prj_FlushCacheMyRolesInProject (void)
  {
   Gbl.Cache.MyRolesInProject.Valid = false;
  }

unsigned Prj_GetMyRolesInProject (long PrjCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRoles;
   unsigned NumRole;
   Prj_RoleInProject_t RoleInPrj;

   /***** 1. Fast check: trivial cases *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod <= 0 ||
       PrjCod <= 0)
      return 0;

   /***** 2. Fast check: Is my role in project already calculated *****/
   if (Gbl.Cache.MyRolesInProject.Valid &&
       PrjCod == Gbl.Cache.MyRolesInProject.PrjCod)
      return Gbl.Cache.MyRolesInProject.RolesInProject;

   /***** 3. Slow check: Get my role in project from database.
			 The result of the query should have one row or none *****/
   Gbl.Cache.MyRolesInProject.PrjCod         = PrjCod;
   Gbl.Cache.MyRolesInProject.RolesInProject = 0;
   NumRoles = Prj_DB_GetMyRolesInPrj (&mysql_res,PrjCod);
   for (NumRole = 0;
	NumRole < NumRoles;
	NumRole++)
     {
      row = mysql_fetch_row (mysql_res);
      RoleInPrj = Prj_ConvertUnsignedStrToRoleInProject (row[0]);
      if (RoleInPrj != Prj_ROLE_UNK)
	 Gbl.Cache.MyRolesInProject.RolesInProject |= (1 << RoleInPrj);
     }
   DB_FreeMySQLResult (&mysql_res);
   Gbl.Cache.MyRolesInProject.Valid = true;

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
   Prj_ResetPrjsAndReadConfig (&Projects);

   Prj_FormToSelectStds (&Projects);
  }

void Prj_ReqAddTuts (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   Prj_FormToSelectTuts (&Projects);
  }

void Prj_ReqAddEvls (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

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
                                  Prj_RoleInProject_t RoleInPrj)
  {
   extern const char *Hlp_ASSESSMENT_Projects_add_user;
   extern const char *Txt_Add_USERS;
   extern const char *Txt_PROJECT_ROLES_PLURAL_abc[Prj_NUM_ROLES_IN_PROJECT];
   static const Act_Action_t ActionAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      [Prj_ROLE_UNK] = ActUnk,		// Unknown
      [Prj_ROLE_STD] = ActAddStdPrj,	// Student
      [Prj_ROLE_TUT] = ActAddTutPrj,	// Tutor
      [Prj_ROLE_EVL] = ActAddEvlPrj,	// Evaluator
     };
   char *TxtButton;

   /***** Get parameters *****/
   Prj_GetPars (Projects);
   Projects->Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Put form to select users *****/
   if (asprintf (&TxtButton,Txt_Add_USERS,
	         Txt_PROJECT_ROLES_PLURAL_abc[RoleInPrj]) < 0)
      Err_NotEnoughMemoryExit ();
   Usr_PutFormToSelectUsrsToGoToAct (&Prj_MembersToAdd,
				     ActionAddUsr[RoleInPrj],
				     Prj_PutCurrentPars,Projects,
				     TxtButton,
                                     Hlp_ASSESSMENT_Projects_add_user,
                                     TxtButton,
				     false);	// Do not put form with date range
   free (TxtButton);

   /***** Put a form to create/edit project *****/
   Prj_ReqCreatOrEditPrj (Projects);
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

static void Prj_AddUsrsToProject (Prj_RoleInProject_t RoleInPrj)
  {
   extern const char *Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   struct Prj_Projects Projects;
   const char *Ptr;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Add the selected users to project *****/
   Ptr = Prj_MembersToAdd.List[Rol_UNK];
   while (*Ptr)
     {
      /* Get next user */
      Par_GetNextStrUntilSeparParMult (&Ptr,Gbl.Usrs.Other.UsrDat.EnUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);

      /* Get user's data */
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                   Usr_DONT_GET_PREFS,
                                                   Usr_DONT_GET_ROLE_IN_CRS))
        {
	 /* Add user to project */
	 Prj_DB_AddUsrToPrj (Projects.Prj.PrjCod,RoleInPrj,Gbl.Usrs.Other.UsrDat.UsrCod);

	 /* Flush cache */
	 if (Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_ME)
	    Prj_FlushCacheMyRolesInProject ();

	 /* Show success alert */
	 Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project,
			Gbl.Usrs.Other.UsrDat.FullName,
			Txt_PROJECT_ROLES_SINGUL_abc[RoleInPrj][Gbl.Usrs.Other.UsrDat.Sex]);
        }
     }

   /***** Free memory used by lists of selected encrypted users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Prj_MembersToAdd);

   /***** Put form to edit project again *****/
   Prj_ReqCreatOrEditPrj (&Projects);
  }

/*****************************************************************************/
/************ Request confirmation to remove user from project ***************/
/*****************************************************************************/

void Prj_ReqRemStd (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   Prj_ReqRemUsrFromPrj (&Projects,Prj_ROLE_STD);
  }

void Prj_ReqRemTut (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   Prj_ReqRemUsrFromPrj (&Projects,Prj_ROLE_TUT);
  }

void Prj_ReqRemEvl (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   Prj_ReqRemUsrFromPrj (&Projects,Prj_ROLE_EVL);
  }

static void Prj_ReqRemUsrFromPrj (struct Prj_Projects *Projects,
                                  Prj_RoleInProject_t RoleInPrj)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   extern const char *Txt_Remove_USER_from_this_project;
   static const Act_Action_t ActionRemUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      [Prj_ROLE_UNK] = ActUnk,		// Unknown
      [Prj_ROLE_STD] = ActRemStdPrj,	// Student
      [Prj_ROLE_TUT] = ActRemTutPrj,	// Tutor
      [Prj_ROLE_EVL] = ActRemEvlPrj,	// Evaluator
     };
   const char *Question[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y,
      [Usr_OTHER] = Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y
     };
   char *TxtButton;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects->Prj);

   /***** Get parameters *****/
   Prj_GetPars (Projects);
   Projects->Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects->Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (&Projects->Prj))
	{
	 /***** Show question and button to remove user as a role from project *****/
	 /* Begin alert */
	 Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Question[Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod)],
				  Txt_PROJECT_ROLES_SINGUL_abc[RoleInPrj][Gbl.Usrs.Other.UsrDat.Sex],
				  Projects->Prj.Title);

	    /* Show user's record */
	    Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	    /* Show form to request confirmation */
	    Frm_BeginForm (ActionRemUsr[RoleInPrj]);
	       Prj_PutCurrentPars (Projects);
	       if (asprintf (&TxtButton,Txt_Remove_USER_from_this_project,
	                     Txt_PROJECT_ROLES_SINGUL_abc[RoleInPrj][Gbl.Usrs.Other.UsrDat.Sex]) < 0)
		  Err_NotEnoughMemoryExit ();
	       Btn_PutRemoveButton (TxtButton);
	       free (TxtButton);
	    Frm_EndForm ();

	 /* End alert */
	 Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
	}
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects->Prj);

   /***** Put form to edit project again *****/
   Prj_ReqCreatOrEditPrj (Projects);
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

static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInPrj)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_as_a_Y_from_the_project_Z;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (&Projects.Prj))
	{
	 /***** Remove user from the table of project-users *****/
	 Prj_DB_RemoveUsrFromPrj (Projects.Prj.PrjCod,RoleInPrj,Gbl.Usrs.Other.UsrDat.UsrCod);

	 /***** Flush cache *****/
	 if (Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod) == Usr_ME)
	    Prj_FlushCacheMyRolesInProject ();

	 /***** Show success alert *****/
         Ale_ShowAlert (Ale_SUCCESS,Txt_THE_USER_X_has_been_removed_as_a_Y_from_the_project_Z,
		        Gbl.Usrs.Other.UsrDat.FullName,
		        Txt_PROJECT_ROLES_SINGUL_abc[RoleInPrj][Gbl.Usrs.Other.UsrDat.Sex],
		        Projects.Prj.Title);
	}
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);

   /***** Put form to edit project again *****/
   Prj_ReqCreatOrEditPrj (&Projects);
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of projects **********/
/*****************************************************************************/

static Prj_Order_t Prj_GetParPrjOrder (void)
  {
   return (Prj_Order_t)
	  Par_GetParUnsignedLong ("Order",
				  0,
				  Prj_NUM_ORDERS - 1,
				  (unsigned long) Prj_ORDER_DEFAULT);
  }

/*****************************************************************************/
/****************** Put a link (form) to edit one project ********************/
/*****************************************************************************/

static void Prj_PutIconsToRemEditOnePrj (struct Prj_Projects *Projects,
                                         const char *Anchor)
  {
   static const Act_Action_t ActionHideUnhide[HidVis_NUM_HIDDEN_VISIBLE] =
     {
      [HidVis_HIDDEN ] = ActUnhPrj,	// Hidden ==> action to unhide
      [HidVis_VISIBLE] = ActHidPrj,	// Visible ==> action to hide
     };

   if (Prj_CheckIfICanEditProject (&Projects->Prj))
     {
      /***** Icon to remove project *****/
      Ico_PutContextualIconToRemove (ActReqRemPrj,NULL,
                                     Prj_PutCurrentPars,Projects);

      /***** Icon to hide/unhide project *****/
      Ico_PutContextualIconToHideUnhide (ActionHideUnhide,Anchor,
					 Prj_PutCurrentPars,Projects,
					 Projects->Prj.Hidden);

      /***** Icon to edit project *****/
      Ico_PutContextualIconToEdit (ActEdiOnePrj,NULL,
                                   Prj_PutCurrentPars,Projects);
     }

   /***** Icon to admin project documents *****/
   if (Prj_CheckIfICanViewProjectFiles (Projects->Prj.PrjCod))
      Ico_PutContextualIconToViewFiles (ActAdmDocPrj,
                                        Prj_PutCurrentPars,Projects);

   /***** Icon to print project *****/
   Ico_PutContextualIconToPrint (ActPrnOnePrj,
                                 Prj_PutCurrentPars,Projects);

   /***** Locked/unlocked project edition *****/
   if (PrjCfg_CheckIfICanConfig ())
     {
      /* Icon to lock/unlock project edition */
      HTM_DIV_Begin ("id=\"prj_lck_%ld\" class=\"PRJ_LOCK\"",
                     Projects->Prj.PrjCod);
	 Prj_FormLockUnlock (&Projects->Prj);
      HTM_DIV_End ();
     }
   else
      /* Icon to inform about locked/unlocked project edition */
      Prj_PutIconOffLockedUnlocked (&Projects->Prj);

   /***** Link to get resource link *****/
   if (Rsc_CheckIfICanGetLink ())
      Ico_PutContextualIconToGetLink (ActReqLnkPrj,NULL,
				      Prj_PutCurrentPars,Projects);
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
   MYSQL_RES *mysql_res = NULL;	// Initialized to avoid freeing when not assigned
   unsigned NumUsrsInList = 0;
   long *LstSelectedUsrCods = NULL;
   char *UsrsSubQuery = NULL;
   unsigned NumPrjsFromDB;
   unsigned NumPrjsAfterFilter = 0;
   unsigned NumPrj;
   struct Prj_Faults Faults;
   long PrjCod;

   /***** Get list of projects from database *****/
   if (Projects->LstIsRead)
      Prj_FreeListProjects (Projects);

   if (Projects->Filter.Assign &&	// Any selector is on
       Projects->Filter.Hidden &&	// Any selector is on
       Projects->Filter.Faulti &&	// Any selector is on
       Projects->Filter.Review)		// Any selector is on
     {
      /****** Get users selected *****/
      if (Projects->Filter.Who == Usr_WHO_SELECTED)
	{
	 /* Count number of valid users in list of encrypted user codes */
	 NumUsrsInList = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected);

	 if (NumUsrsInList)
	   {
	    /* Get list of users selected to show their projects */
	    Usr_GetListSelectedUsrCods (&Gbl.Usrs.Selected,NumUsrsInList,&LstSelectedUsrCods);

	    /* Create subquery string */
	    Usr_CreateSubqueryUsrCods (LstSelectedUsrCods,NumUsrsInList,
				       &UsrsSubQuery);
	   }
	}

      /***** Query database *****/
      NumPrjsFromDB = Prj_DB_GetListProjects (&mysql_res,Projects,
                                              UsrsSubQuery);

      /****** Free users selected *****/
      if (Projects->Filter.Who == Usr_WHO_SELECTED)
	 if (NumUsrsInList)
           {
	    /* Free memory for subquery string */
	    Usr_FreeSubqueryUsrCods (UsrsSubQuery);

	    /* Free list of user codes */
	    Usr_FreeListSelectedUsrCods (LstSelectedUsrCods);
	   }

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
		  Prj_CheckIfPrjIsFaulty (PrjCod,&Faults);
		  if (Faults.PrjIsFaulty)
		     Projects->LstPrjCods[NumPrjsAfterFilter++] = PrjCod;
		  break;
	       case (1 << Prj_FAULTLESS):	// Faultless projects
		  Prj_CheckIfPrjIsFaulty (PrjCod,&Faults);
		  if (!Faults.PrjIsFaulty)
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
/********************* Get project data using its code ***********************/
/*****************************************************************************/

void Prj_GetProjectDataByCod (struct Prj_Project *Prj)
  {
   extern const char *Prj_DB_Proposal[Prj_NUM_PROPOSAL_TYPES];
   extern const char *Prj_DB_ReviewStatus[Prj_NUM_REVIEW_STATUS];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Prj_Proposal_t Proposal;
   Prj_ReviewStatus_t ReviewStatus;

   if (Prj->PrjCod > 0)
     {
      /***** Clear all project data *****/
      Prj_ResetProject (Prj);

      /***** Get data of project *****/
      if (Prj_DB_GetProjectDataByCod (&mysql_res,Prj->PrjCod))	// Project found...
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of the project (row[0]),
	        code of the course (row[1])
	    and code of the department (row[2]) */
	 Prj->PrjCod = Str_ConvertStrCodToLongCod (row[0]);
	 Prj->CrsCod = Str_ConvertStrCodToLongCod (row[1]);
	 Prj->DptCod = Str_ConvertStrCodToLongCod (row[2]);

	 /* Get whether the project is locked or not (row[3]),
	        whether the project is hidden or not (row[4])
	    and whether the project is assigned or not (row[5]) */
	 Prj->Locked   = (row[3][0] == 'Y') ? Prj_LOCKED :
					      Prj_UNLOCKED;
	 Prj->Hidden   = HidVid_GetHiddenOrVisible (row[4][0]);
	 Prj->Assigned = (row[5][0] == 'Y') ? Prj_ASSIGNED :
					      Prj_NONASSIG;

	 /* Get number of students (row[6]) */
	 Prj->NumStds = Str_ConvertStrToUnsigned (row[6]);

	 /* Get project status (row[7]) */
	 Prj->Proposal = Prj_PROPOSAL_DEFAULT;
	 for (Proposal  = (Prj_Proposal_t) 0;
	      Proposal <= (Prj_Proposal_t) (Prj_NUM_PROPOSAL_TYPES - 1);
	      Proposal++)
	    if (!strcmp (Prj_DB_Proposal[Proposal],row[7]))
	      {
	       Prj->Proposal = Proposal;
	       break;
	      }

	 /* Get creation date/time (row[8] holds the creation UTC time)
	    and modification date/time (row[9] holds the modification UTC time) */
	 Prj->CreatTime = Dat_GetUNIXTimeFromStr (row[8]);
	 Prj->ModifTime = Dat_GetUNIXTimeFromStr (row[9]);

	 /* Get title (row[10]), description (row[11]), required knowledge (row[12]),
	    required materials (row[13]) and URL (row[14]) of the project */
	 Str_Copy (Prj->Title      ,row[10],sizeof (Prj->Title) - 1);
	 Str_Copy (Prj->Description,row[11],Cns_MAX_BYTES_TEXT);
	 Str_Copy (Prj->Knowledge  ,row[12],Cns_MAX_BYTES_TEXT);
	 Str_Copy (Prj->Materials  ,row[13],Cns_MAX_BYTES_TEXT);
	 Str_Copy (Prj->URL        ,row[14],sizeof (Prj->URL  ) - 1);

	 /* Get review status (row[15]), review time (row[16])
	    and review text (row[17]) */
	 Prj->Review.Status = Prj_REVIEW_STATUS_DEFAULT;
	 for (ReviewStatus  = (Prj_ReviewStatus_t) 0;
	      ReviewStatus <= (Prj_ReviewStatus_t) (Prj_NUM_REVIEW_STATUS - 1);
	      ReviewStatus++)
	    if (!strcmp (Prj_DB_ReviewStatus[ReviewStatus],row[15]))
	      {
	       Prj->Review.Status = ReviewStatus;
	       break;
	      }
	 Prj->Review.Time = Dat_GetUNIXTimeFromStr (row[16]);
	 Str_Copy (Prj->Review.Txt  ,row[17],Cns_MAX_BYTES_TEXT);
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

   Prj->Review.Status = Prj_REVIEW_STATUS_DEFAULT;
   Prj->Review.Time   = (time_t) 0;
   Prj->Review.Txt[0] = '\0';
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
/**************** Ask for confirmation of removing a project *****************/
/*****************************************************************************/

void Prj_ReqRemProject (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_project_X;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Check if I can edit this project *****/
   if (!Prj_CheckIfICanEditProject (&Projects.Prj))
      Err_NoPermissionExit ();

   /***** Show question and button to remove the project *****/
   Ale_ShowAlertRemove (ActRemPrj,NULL,
			Prj_PutCurrentPars,&Projects,
			Txt_Do_you_really_want_to_remove_the_project_X,
			Projects.Prj.Title);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);

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
   char PathRelPrj[PATH_MAX + 1];

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects.Prj);	// Inside this function, the course is checked to be the current one

   /***** Check if I can edit this project *****/
   if (!Prj_CheckIfICanEditProject (&Projects.Prj))
      Err_NoPermissionExit ();

   /***** Remove users in project *****/
   Prj_DB_RemoveUsrsFromPrj (Projects.Prj.PrjCod);

   /***** Flush cache *****/
   Prj_FlushCacheMyRolesInProject ();

   /***** Remove project *****/
   Prj_DB_RemovePrj (Projects.Prj.PrjCod);

   /***** Remove information related to files in project *****/
   Brw_DB_RemovePrjFiles (Projects.Prj.PrjCod);

   /***** Remove directory of the project *****/
   snprintf (PathRelPrj,sizeof (PathRelPrj),"%s/%ld/%s/%02u/%ld",
	     Cfg_PATH_CRS_PRIVATE,Projects.Prj.CrsCod,Cfg_FOLDER_PRJ,
	     (unsigned) (Projects.Prj.PrjCod % 100),Projects.Prj.PrjCod);
   Fil_RemoveTree (PathRelPrj);

   /***** Write message to show the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Project_X_removed,
		  Projects.Prj.Title);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/*************************** Hide/unhide a project ***************************/
/*****************************************************************************/

void Prj_HideProject (void)
  {
   Prj_HideUnhideProject (HidVis_HIDDEN);
  }

void Prj_UnhideProject (void)
  {
   Prj_HideUnhideProject (HidVis_VISIBLE);
  }

static void Prj_HideUnhideProject (HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Check if I can edit this project *****/
   if (!Prj_CheckIfICanEditProject (&Projects.Prj))
      Err_NoPermissionExit ();

   /***** Hide/unhide project *****/
   Prj_DB_HideOrUnhideProject (Projects.Prj.PrjCod,HiddenOrVisible);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/********************* Put a form to create/edit project *********************/
/*****************************************************************************/

void Prj_ReqCreatePrj (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can create new projects *****/
   if (!Prj_CheckIfICanCreateProjects (&Projects))
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = -1L;	// It's a new, non existing, project

   /***** Form to create project *****/
   Prj_ReqCreatOrEditPrj (&Projects);
  }

void Prj_ReqEditPrj (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Form to edit project *****/
   Prj_ReqCreatOrEditPrj (&Projects);
  }

static void Prj_ReqCreatOrEditPrj (struct Prj_Projects *Projects)
  {
   bool ItsANewProject = (Projects->Prj.PrjCod < 0);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects->Prj);

   /***** Get from the database the data of the project *****/
   if (ItsANewProject)
     {
      /* Initialize to empty project */
      Prj_ResetProject (&Projects->Prj);
      Projects->Prj.CreatTime =
      Projects->Prj.ModifTime = Dat_GetStartExecutionTimeUTC ();
      Projects->Prj.DptCod = Gbl.Usrs.Me.UsrDat.Tch.DptCod;	// Default: my department
     }
   else
      /* Get data of the project from database */
      Prj_GetProjectDataByCod (&Projects->Prj);

   /***** Put form to edit project *****/
   Prj_PutFormProject (Projects,ItsANewProject);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects->Prj);

   /***** Show projects again *****/
   HTM_BR ();
   Prj_ShowProjects (Projects);
  }

static void Prj_PutFormProject (struct Prj_Projects *Projects,
                                bool ItsANewProject)
  {
   extern const char *Hlp_ASSESSMENT_Projects_edit_project;
   extern const char *Par_CodeStr[];
   extern const char *Txt_Project;
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
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
   extern const char *Txt_Create;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Members;
   Prj_Proposal_t Proposal;
   unsigned ProposalUnsigned;
   unsigned NumRoleToShow;
   char *SelectClass;
   Act_Action_t NextAction = ItsANewProject ? ActNewPrj :
					      ActChgPrj;

   /***** Begin project box *****/
   Box_BoxBegin (Projects->Prj.Title[0] ? Projects->Prj.Title :
					  Txt_Project,
		 NULL,NULL,
		 Hlp_ASSESSMENT_Projects_edit_project,Box_NOT_CLOSABLE);

      /***** 1. Project members *****/
      if (!ItsANewProject)	// Existing project
	{
	 HTM_FIELDSET_Begin (NULL);
	    HTM_LEGEND (Txt_Members);
	    HTM_TABLE_BeginWidePadding (2);
	       Projects->View = Prj_EDIT_ONE_PROJECT;
	       for (NumRoleToShow = 0;
		    NumRoleToShow < Prj_NUM_ROLES_TO_SHOW;
		    NumRoleToShow++)
		  Prj_ShowProjectMembersWithARole (Projects,Prj_RolesToShow[NumRoleToShow]);
	    HTM_TABLE_End ();
	 HTM_FIELDSET_End ();
	}

      /***** 2. Project data *****/
      HTM_FIELDSET_Begin (NULL);
	 HTM_LEGEND (Txt_Actions[NextAction]);

	 /* Begin data form */
	 Frm_BeginForm (NextAction);
	    Prj_PutCurrentPars (Projects);

	    /* Begin table */
	    HTM_TABLE_BeginWidePadding (2);

	       /* Project title */
	       HTM_TR_Begin (NULL);

		  /* Label */
		  Frm_LabelColumn ("RT","Title",Txt_Title);

		  /* Data */
		  HTM_TD_Begin ("class=\"LT\"");
		     HTM_INPUT_TEXT ("Title",Prj_MAX_CHARS_TITLE,Projects->Prj.Title,
				     HTM_DONT_SUBMIT_ON_CHANGE,
				     "id=\"Title\""
				     " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\""
				     " required=\"required\"",
				     The_GetSuffix ());
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /* Department */
	       HTM_TR_Begin (NULL);

		  /* Label */
		  Frm_LabelColumn ("RT",Par_CodeStr[ParCod_Dpt],Txt_Department);

		  /* Data */
		  HTM_TD_Begin ("class=\"LT\"");
		     if (asprintf (&SelectClass,"TITLE_DESCRIPTION_WIDTH INPUT_%s",
				   The_GetSuffix ()) < 0)
			Err_NotEnoughMemoryExit ();
		     Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Node[Hie_INS].HieCod,	// Departments in current institution
						  Projects->Prj.DptCod,			// Selected department
						  Par_CodeStr[ParCod_Dpt],		// Parameter name
						  SelectClass,				// Selector class
						  0,					// First option
						  Txt_Another_department,		// Text when no department selected
						  HTM_DONT_SUBMIT_ON_CHANGE);
		     free (SelectClass);
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /* Assigned? */
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("class=\"RM FORM_IN_%s\"",The_GetSuffix ());
		     HTM_TxtColon (Txt_Assigned_QUESTION);
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"LM\"");
		     HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				       "name=\"Assigned\" class=\"INPUT_%s\"",
				       The_GetSuffix ());
			HTM_OPTION (HTM_Type_STRING,"Y",
				    Projects->Prj.Assigned == Prj_ASSIGNED ? HTM_OPTION_SELECTED :
									     HTM_OPTION_SELECTED,
				    HTM_OPTION_ENABLED,
				    "%s",Txt_Yes);
			HTM_OPTION (HTM_Type_STRING,"N",
				    Projects->Prj.Assigned == Prj_NONASSIG ? HTM_OPTION_SELECTED :
									     HTM_OPTION_UNSELECTED,
				    HTM_OPTION_ENABLED,
				    "%s",Txt_No);
		     HTM_SELECT_End ();
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /* Number of students */
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("class=\"RM FORM_IN_%s\"",The_GetSuffix ());
		     HTM_TxtColon (Txt_Number_of_students);
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"LM\"");
		     HTM_INPUT_LONG ("NumStds",(long) 0,(long) UINT_MAX,(long) Projects->Prj.NumStds,
				     HTM_DONT_SUBMIT_ON_CHANGE,false,
				     "class=\"INPUT_%s\"",
				     The_GetSuffix ());
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /* Proposal */
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("class=\"RM FORM_IN_%s\"",The_GetSuffix ());
		     HTM_TxtColon (Txt_Proposal);
		  HTM_TD_End ();

		  HTM_TD_Begin ("class=\"LM\"");
		     HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,NULL,
				       "name=\"Proposal\""
				       " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
				       The_GetSuffix ());
			for (Proposal  = (Prj_Proposal_t) 0;
			     Proposal <= (Prj_Proposal_t) (Prj_NUM_PROPOSAL_TYPES - 1);
			     Proposal++)
			  {
			   ProposalUnsigned = (unsigned) Proposal;
			   HTM_OPTION (HTM_Type_UNSIGNED,&ProposalUnsigned,
				       Projects->Prj.Proposal == Proposal ? HTM_OPTION_SELECTED :
									    HTM_OPTION_UNSELECTED,
				       HTM_OPTION_ENABLED,
				       "%s",Txt_PROJECT_STATUS[Proposal]);
			  }
		     HTM_SELECT_End ();
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /* Description of the project */
	       Prj_EditOneProjectTxtArea ("Description",Txt_Description,
					  Projects->Prj.Description,12,
					  true);	// Required

	       /* Required knowledge to carry out the project */
	       Prj_EditOneProjectTxtArea ("Knowledge",Txt_Required_knowledge,
					  Projects->Prj.Knowledge,4,
					  false);	// Not required

	       /* Required materials to carry out the project */
	       Prj_EditOneProjectTxtArea ("Materials",Txt_Required_materials,
					  Projects->Prj.Materials,4,
					  false);	// Not required

	       /* URL for additional info */
	       HTM_TR_Begin (NULL);

		  /* Label */
		  Frm_LabelColumn ("RT","WWW",Txt_URL);

		  /* Data */
		  HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
		     HTM_INPUT_URL ("URL",Projects->Prj.URL,HTM_DONT_SUBMIT_ON_CHANGE,
				    "class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\"",
				    The_GetSuffix ());
		  HTM_TD_End ();

	       HTM_TR_End ();

	    /* End table */
	    HTM_TABLE_End ();

	    /* Send button */
	    if (ItsANewProject)
	       Btn_PutButton (Btn_CREATE_BUTTON,Txt_Create);
	    else
	       Btn_PutButton (Btn_CONFIRM_BUTTON,Txt_Save_changes);

	 /* End data form */
	 Frm_EndForm ();

      HTM_FIELDSET_End ();

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
   /***** Description *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT",Id,Label);

      /* Data */
      HTM_TD_Begin ("class=\"LT\"");
	 HTM_TEXTAREA_Begin ("id=\"%s\" name=\"%s\" rows=\"%u\""
			     " class=\"TITLE_DESCRIPTION_WIDTH INPUT_%s\"%s",
			     Id,Id,NumRows,
			     The_GetSuffix (),
			     Required ? " required=\"required\"" :
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

   if ((Prj->Review.Txt  = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
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
   if (Prj->Review.Txt)
     {
      free (Prj->Review.Txt);
      Prj->Review.Txt = NULL;
     }
  }

/*****************************************************************************/
/******************** Receive form to create a new project *******************/
/*****************************************************************************/

void Prj_ReceiveFormProject (void)
  {
   extern const char *Txt_Created_new_project_X;
   extern const char *Txt_The_project_has_been_modified;
   struct Prj_Projects Projects;
   bool ItsANewProject;
   bool ICanEditProject;
   bool NewProjectIsCorrect = true;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   ItsANewProject = ((Projects.Prj.PrjCod = ParCod_GetPar (ParCod_Prj)) <= 0);

   if (ItsANewProject)
     {
      /* Reset project data */
      Prj_ResetProject (&Projects.Prj);

      ICanEditProject = Prj_CheckIfICanCreateProjects (&Projects);
     }
   else
     {
      /* Get data of the project from database */
      Prj_GetProjectDataByCod (&Projects.Prj);

      ICanEditProject = Prj_CheckIfICanEditProject (&Projects.Prj);
     }

   /* Check if I can create/edit project */
   if (!ICanEditProject)
      Err_NoPermissionExit ();

   /* Get project title */
   Par_GetParText ("Title",Projects.Prj.Title,Prj_MAX_BYTES_TITLE);

   /* Get department */
   Projects.Prj.DptCod = ParCod_GetPar (ParCod_Dpt);

   /* Get whether the project is assigned */
   Projects.Prj.Assigned = (Par_GetParBool ("Assigned")) ? Prj_ASSIGNED :
							   Prj_NONASSIG;

   /* Get number of students */
   Projects.Prj.NumStds = (unsigned)
			  Par_GetParUnsignedLong ("NumStds",
						  0,
						  UINT_MAX,
						  1);

   /* Get status */
   Projects.Prj.Proposal = (Prj_Proposal_t)
			   Par_GetParUnsignedLong ("Proposal",
						   0,
						   Prj_NUM_PROPOSAL_TYPES - 1,
						   (unsigned long) Prj_PROPOSAL_DEFAULT);

   /* Get project description, required knowledge and required materials */
   Par_GetParHTML ("Description",Projects.Prj.Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
   Par_GetParHTML ("Knowledge"  ,Projects.Prj.Knowledge  ,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
   Par_GetParHTML ("Materials"  ,Projects.Prj.Materials  ,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /* Get degree WWW */
   Par_GetParText ("URL",Projects.Prj.URL,Cns_MAX_BYTES_WWW);

   /***** Check if title is correct *****/
   if (!Projects.Prj.Title[0])	// If there is not a project title
     {
      NewProjectIsCorrect = false;
      Ale_CreateAlertYouMustSpecifyTheTitle ();
     }

   /***** Create a new project or update an existing one *****/
   if (NewProjectIsCorrect)
     {
      if (ItsANewProject)
	{
	 /* Create project */
	 Prj_CreateProject (&Projects.Prj);	// Add new project to database

	 /* Write success message */
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_project_X,
			Projects.Prj.Title);
	}
      else if (NewProjectIsCorrect)
	{
	 /* Update project */
	 Prj_UpdateProject (&Projects.Prj);

	 /* Write success message */
	 Ale_ShowAlert (Ale_SUCCESS,Txt_The_project_has_been_modified);
	}
     }
   else
      Prj_PutFormProject (&Projects,ItsANewProject);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);

   /***** Show again form to edit project *****/
   Prj_ReqCreatOrEditPrj (&Projects);
  }

/*****************************************************************************/
/************************** Create a new project *****************************/
/*****************************************************************************/

static void Prj_CreateProject (struct Prj_Project *Prj)
  {
   /***** Set dates to now *****/
   Prj->CreatTime =
   Prj->ModifTime = Dat_GetStartExecutionTimeUTC ();

   /***** Create a new project *****/
   Prj->PrjCod = Prj_DB_CreateProject (Prj);

   /***** Insert creator as first tutor *****/
   Prj_DB_AddUsrToPrj (Prj->PrjCod,Prj_ROLE_TUT,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Flush cache *****/
   Prj_FlushCacheMyRolesInProject ();
  }

/*****************************************************************************/
/*********************** Update an existing project **************************/
/*****************************************************************************/

static void Prj_UpdateProject (struct Prj_Project *Prj)
  {
   /***** Adjust date of last edition to now *****/
   Prj->ModifTime = Dat_GetStartExecutionTimeUTC ();

   /***** Update the data of the project *****/
   Prj_DB_UpdateProject (Prj);
  }

/*****************************************************************************/
/****** Put icons to request locking/unlocking edition of all projects *******/
/*****************************************************************************/

static void Prj_PutIconsToLockUnlockAllProjects (struct Prj_Projects *Projects)
  {
   /***** Put icon to lock all projects *****/
   Lay_PutContextualLinkOnlyIcon (ActReqLckAllPrj,NULL,
                                  Prj_PutCurrentPars,Projects,
			          "lock.svg",Ico_RED);

   /***** Put icon to unlock all projects *****/
   Lay_PutContextualLinkOnlyIcon (ActReqUnlAllPrj,NULL,
                                  Prj_PutCurrentPars,Projects,
			          "unlock.svg",Ico_GREEN);
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
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can configure projects *****/
   if (!PrjCfg_CheckIfICanConfig ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   Prj_GetPars (&Projects);

   /***** Show question and button to lock all selected projects *****/
   /* Get list of projects */
   Prj_GetListProjects (&Projects);

   /* Show question and button */
   if (Projects.Num)
      Ale_ShowAlertAndButton (ActLckAllPrj,NULL,NULL,
			      Prj_PutCurrentPars,&Projects,
			      Btn_REMOVE_BUTTON,Txt_Lock_editing,
			      Ale_QUESTION,Txt_Do_you_want_to_lock_the_editing_of_the_X_selected_projects,
			      Projects.Num);
   else	// No projects found
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /* Free list of projects */
   Prj_FreeListProjects (&Projects);

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
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can configure projects *****/
   if (!PrjCfg_CheckIfICanConfig ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   Prj_GetPars (&Projects);

   /***** Show question and button to unlock all selected projects *****/
   /* Get list of projects */
   Prj_GetListProjects (&Projects);

   /* Show question and button */
   if (Projects.Num)
      Ale_ShowAlertAndButton (ActUnlAllPrj,NULL,NULL,
			      Prj_PutCurrentPars,&Projects,
			      Btn_CREATE_BUTTON,Txt_Unlock_editing,
			      Ale_QUESTION,Txt_Do_you_want_to_unlock_the_editing_of_the_X_selected_projects,
			      Projects.Num);
   else	// No projects found
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /* Free list of projects */
   Prj_FreeListProjects (&Projects);

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
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can configure projects *****/
   if (!PrjCfg_CheckIfICanConfig ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   Prj_GetPars (&Projects);

   /***** Lock all selected projects *****/
   /* Get list of projects */
   Prj_GetListProjects (&Projects);

   /* Lock projects */
   if (Projects.Num)
      for (NumPrj = 0;
	   NumPrj < Projects.Num;
	   NumPrj++)
	 Prj_DB_LockProjectEdition (Projects.LstPrjCods[NumPrj]);
   else	// No projects found
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /* Free list of projects */
   Prj_FreeListProjects (&Projects);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

void Prj_UnloSelectedPrjsEdition (void)
  {
   extern const char *Txt_No_projects;
   struct Prj_Projects Projects;
   unsigned NumPrj;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can configure projects *****/
   if (!PrjCfg_CheckIfICanConfig ())
      Err_NoPermissionExit ();

   /***** Get parameters *****/
   Prj_GetPars (&Projects);

   /***** Unlock all selected projects *****/
   /* Get list of projects */
   Prj_GetListProjects (&Projects);

   /* Unlock projects */
   if (Projects.Num)
      for (NumPrj = 0;
	   NumPrj < Projects.Num;
	   NumPrj++)
	 Prj_DB_UnlockProjectEdition (Projects.LstPrjCods[NumPrj]);
   else	// No projects found
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /* Free list of projects */
   Prj_FreeListProjects (&Projects);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/******************** Form to lock/unlock project edition ********************/
/*****************************************************************************/

static void Prj_FormLockUnlock (const struct Prj_Project *Prj)
  {
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
		 Act_GetActCod (Prj_LockUnlock[Prj->Locked].Action),
		 Gbl.Session.Id,
		 Prj->PrjCod) < 0)
      Err_NotEnoughMemoryExit ();
   Frm_BeginFormOnSubmit (ActUnk,OnSubmit);
      Ico_PutIconLink (Prj_LockUnlock[Prj->Locked].Icon,
                       Prj_LockUnlock[Prj->Locked].Color,
                       Prj_LockUnlock[Prj->Locked].Action);
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
   Ico_PutIconOff (Prj_LockUnlock[Prj->Locked].Icon,
                   Prj_LockUnlock[Prj->Locked].Color,
                   Txt_LOCKED_UNLOCKED[Prj->Locked]);
  }

/*****************************************************************************/
/************************** Lock edition of a project ************************/
/*****************************************************************************/

void Prj_LockProjectEdition (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can configure projects *****/
   if (!PrjCfg_CheckIfICanConfig ())
      Err_NoPermissionExit ();

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Lock project edition *****/
   Prj_DB_LockProjectEdition (Projects.Prj.PrjCod);
   Projects.Prj.Locked = Prj_LOCKED;

   /***** Show updated form and icon *****/
   Prj_FormLockUnlock (&Projects.Prj);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);
  }

/*****************************************************************************/
/************************* Unlock edition of a project ***********************/
/*****************************************************************************/

void Prj_UnloProjectEdition (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can configure projects *****/
   if (!PrjCfg_CheckIfICanConfig ())
      Err_NoPermissionExit ();

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Unlock project edition *****/
   Prj_DB_UnlockProjectEdition (Projects.Prj.PrjCod);
   Projects.Prj.Locked = Prj_UNLOCKED;

   /***** Show updated form and icon *****/
   Prj_FormLockUnlock (&Projects.Prj);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);
  }

/*****************************************************************************/
/********************** Change review status of a project ********************/
/*****************************************************************************/

void Prj_ChangeReviewStatus (void)
  {
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Check if I can review projects *****/
   if (!Prj_CheckIfICanReviewProjects ())
      Err_NoPermissionExit ();

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Update review *****/
   Projects.Prj.Review.Status = Prj_GetParReviewStatus ();
   Par_GetParHTML ("ReviewTxt",Projects.Prj.Review.Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
   Prj_DB_UpdateReview (&Projects.Prj);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/********************** Get parameter with review status *********************/
/*****************************************************************************/

static Prj_ReviewStatus_t Prj_GetParReviewStatus (void)
  {
   return (Prj_ReviewStatus_t)
	  Par_GetParUnsignedLong ("ReviewStatus",
				  0,
				  Prj_NUM_REVIEW_STATUS - 1,
				  (unsigned long) Prj_REVIEW_STATUS_DEFAULT);
  }

/*****************************************************************************/
/************************ Show rubrics in a project **************************/
/*****************************************************************************/

static void Prj_ShowRubrics (struct Prj_Projects *Projects)
  {
   extern const char *Txt_Assessment;
   PrjCfg_RubricType_t RubricType;

   /***** Begin fieldset *****/
   HTM_FIELDSET_Begin (NULL);
      HTM_LEGEND (Txt_Assessment);

      /***** Begin table *****/
      HTM_TABLE_BeginWideMarginPadding (5);

	 /***** Show rubrics of each type ready to fill them *****/

	 for (RubricType  = (PrjCfg_RubricType_t) 1;
	      RubricType <= (PrjCfg_RubricType_t) (PrjCfg_NUM_RUBRIC_TYPES - 1);
	      RubricType++)
	    if (Prj_CheckIfICanViewRubric (Projects->Prj.PrjCod,RubricType))
	       Prj_ShowRubricsOfType (Projects,RubricType);

      /***** End table *****/
      HTM_TABLE_End ();

   /***** End fieldset *****/
   HTM_FIELDSET_End ();
  }

/*****************************************************************************/
/************************ Show one rubric in a project ***********************/
/*****************************************************************************/

static void Prj_ShowRubricsOfType (struct Prj_Projects *Projects,
                                   PrjCfg_RubricType_t RubricType)
  {
   extern const char *Txt_PROJECT_RUBRIC[PrjCfg_NUM_RUBRIC_TYPES];
   MYSQL_RES *mysql_res;
   unsigned NumRubricsThisType;
   unsigned NumRubThisType;
   struct Rub_Rubric Rubric;
   bool ICanFill = Prj_CheckIfICanFillRubric (Projects->Prj.PrjCod,RubricType);

   /***** Get project rubrics for current course from database *****/
   NumRubricsThisType = Prj_DB_GetRubricsOfType (&mysql_res,RubricType);

   /***** Show each rubric *****/
   for (NumRubThisType = 0;
	NumRubThisType < NumRubricsThisType;
	NumRubThisType++)
     {
      /***** Get rubric data *****/
      Rub_RubricConstructor (&Rubric);
      Rubric.RubCod = DB_GetNextCode (mysql_res);
      Rub_GetRubricDataByCod (&Rubric);

      /***** Show rubric ready to fill them *****/
      /* Begin first row of this rubric */
      HTM_TR_Begin (NULL);

	 /* Rubric title */
	 HTM_TD_Begin ("colspan=\"8\" class=\"LT ASG_TITLE_%s %s\"",
		       The_GetSuffix (),The_GetColorRows ());
	    HTM_TxtColonNBSP (Txt_PROJECT_RUBRIC[RubricType]);
	    HTM_Txt (Rubric.Title);
	 HTM_TD_End ();

      /* End 1st row of this rubric */
      HTM_TR_End ();

      /* Begin 2nd row of this rubric */
      HTM_TR_Begin (NULL);

	 /* Text of the rubric */
	 HTM_TD_Begin ("colspan=\"8\" class=\"LT PAR DAT_%s %s\"",
		       The_GetSuffix (),The_GetColorRows ());
	    Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
			      Rubric.Txt,Cns_MAX_BYTES_TEXT,
			      Str_DONT_REMOVE_SPACES);
	    ALn_InsertLinks (Rubric.Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
	    HTM_Txt (Rubric.Txt);
	 HTM_TD_End ();

      /* End 2nd row of this rubric */
      HTM_TR_End ();

      /* Change color for rubric criteria */
      The_ChangeRowColor ();

      /* Write criteria of this rubric */
      RubCri_ListCriteriaInProject (Projects,Rubric.RubCod,ICanFill);

      /* Change color for next rubric */
      The_ChangeRowColor ();

      /***** Free memory used for rubric *****/
      Rub_RubricDestructor (&Rubric);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************* Who can view/fill rubrics *************************/
/*****************************************************************************/

static bool Prj_CheckIfICanViewRubric (long PrjCod,PrjCfg_RubricType_t WhichRubric)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 switch (WhichRubric)
	   {
	    case PrjCfg_RUBRIC_ERR:
	       return false;
	    case PrjCfg_RUBRIC_TUT:
	    case PrjCfg_RUBRIC_EVL:
	       return ((Prj_GetMyRolesInProject (PrjCod) & (1 << Prj_ROLE_TUT |		// I am a tutor
		                                            1 << Prj_ROLE_EVL)) != 0);	// or an evaluator
	    case PrjCfg_RUBRIC_GBL:
	       return (Prj_GetMyRolesInProject (PrjCod) != 0);	// I am a member
	   }
         return false;
      case Rol_TCH:	// Editing teachers in a course can view all rubrics
      case Rol_SYS_ADM:
         return true;
      default:
         return false;
     }
   return false;
  }

static bool Prj_CheckIfICanFillRubric (long PrjCod,PrjCfg_RubricType_t WhichRubric)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
	 switch (WhichRubric)
	   {
	    case PrjCfg_RUBRIC_ERR:
	       return false;
	    case PrjCfg_RUBRIC_TUT:
	       return ((Prj_GetMyRolesInProject (PrjCod) & (1 << Prj_ROLE_TUT)) != 0);	// I am a tutor
	    case PrjCfg_RUBRIC_EVL:
	       return ((Prj_GetMyRolesInProject (PrjCod) & (1 << Prj_ROLE_EVL)) != 0);	// Am I an evaluator
	    case PrjCfg_RUBRIC_GBL:
	       return false;
	   }
         return false;
      case Rol_TCH:	// Editing teachers in a course can fill all rubrics
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************* Change the score of a criterion in a project ******************/
/*****************************************************************************/

void Prj_ChangeCriterionScore (void)
  {
   struct Prj_Project Prj;
   long CriCod;
   double Score;
   long RubCod;
   PrjCfg_RubricType_t WhichRubric;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj.PrjCod = ParCod_GetAndCheckPar (ParCod_Prj);
   CriCod = ParCod_GetAndCheckPar (ParCod_Cri);
   Score = RubCri_GetParScore ();

   /***** Get data of the project from database *****/
   Prj_GetProjectDataByCod (&Prj);

   /***** Get which rubric match this criterion *****/
   if ((RubCod = Rub_DB_GetRubCodFromCriCod (CriCod)) <= 0)
      Err_WrongRubricExit ();
   if ((WhichRubric = Prj_DB_GetWichRubricFromRubCod (RubCod)) == PrjCfg_RUBRIC_ERR)
      Err_WrongRubricExit ();

   /***** Update review *****/
   if (Prj_CheckIfICanFillRubric (Prj.PrjCod,WhichRubric))
      Rub_DB_UpdateScore (Rsc_PROJECT,Prj.PrjCod,-1L,CriCod,Score);
   else
      Err_NoPermission ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Get parameters related to file browser
          and show again project including file browser *****/
   Brw_GetParAndInitFileBrowser ();
   Prj_ShowOneProject ();
  }

/*****************************************************************************/
/********************** Remove all projects in a course **********************/
/*****************************************************************************/

void Prj_RemoveCrsProjects (long CrsCod)
  {
   /***** Remove users in projects of the course *****/
   Prj_DB_RemoveUsrsFromCrsPrjs (CrsCod);

   /***** Flush cache *****/
   Prj_FlushCacheMyRolesInProject ();

   /***** Remove configuration of projects in the course *****/
   Prj_DB_RemoveConfigOfCrsPrjs (CrsCod);

   /***** Remove associations of rubrics to projects in the course *****/
   Prj_DB_RemoveRubricsOfCrsPrjs (CrsCod);

   /***** Remove projects *****/
   Prj_DB_RemoveCrsPrjs (CrsCod);
  }

/*****************************************************************************/
/******************* Remove user from all his/her projects *******************/
/*****************************************************************************/

void Prj_RemoveUsrFromProjects (long UsrCod)
  {
   /***** Remove user from projects *****/
   Prj_DB_RemoveUsrFromProjects (UsrCod);

   /***** Flush cache *****/
   if (Usr_ItsMe (UsrCod) == Usr_ME)
      Prj_FlushCacheMyRolesInProject ();
  }

/*****************************************************************************/
/************************ Show figures about projects ************************/
/*****************************************************************************/

void Prj_GetAndShowProjectsStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_projects;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_projects;
   extern const char *Txt_Number_of_BR_courses_with_BR_projects;
   extern const char *Txt_Average_number_BR_of_projects_BR_per_course;
   unsigned NumProjects;
   unsigned NumCoursesWithProjects;

   /***** Get the number of projects from this location *****/
   if ((NumProjects = Prj_DB_GetNumProjects (Gbl.Scope.Current)))
      NumCoursesWithProjects = Prj_DB_GetNumCoursesWithProjects (Gbl.Scope.Current);
   else
      NumCoursesWithProjects = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_PROJECTS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_projects,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_projects                      ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_projects      ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_projects_BR_per_course,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of projects *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Unsigned (NumProjects);
	 HTM_TD_Unsigned (NumCoursesWithProjects);
	 HTM_TD_Ratio (NumProjects,NumCoursesWithProjects);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
