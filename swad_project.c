// swad_project.c: projects (final degree projects, thesis)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_project.h"
#include "swad_role.h"
#include "swad_setting.h"
#include "swad_string.h"
#include "swad_table.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

/***** Parameters used to filter listing of projects *****/
#define Prj_PARAM_MY__ALL_NAME	"My_All"
#define Prj_PARAM_PRE_NON_NAME	"PreNon"
#define Prj_PARAM_HID_VIS_NAME	"HidVis"

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
   "new",		// Prj_PROPOSAL_NEW
   "modified",		// Prj_PROPOSAL_MODIFIED
   "unmodified",	// Prj_PROPOSAL_UNMODIFIED
  };

/***** Preassigned/non-preassigned project *****/
static const char *PreassignedNonpreassigImage[Prj_NUM_PREASSIGNED_NONPREASSIG] =
  {
   "user.svg",		// Prj_PREASSIGNED
   "user-slash.svg",	// Prj_NONPREASSIG
  };

/***** Locked/unlocked project edition *****/
static const char *Prj_LockIcons[Prj_NUM_LOCKED_UNLOCKED] =
  {
   "lock.svg",	// Prj_LOCKED
   "unlock.svg",	// Prj_UNLOCKED
  };
static const Act_Action_t Prj_LockActions[Prj_NUM_LOCKED_UNLOCKED] =
  {
   ActUnlPrj,	// Prj_LOCKED
   ActLckPrj,	// Prj_UNLOCKED
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prj_ShowProjectsInCurrentPage (void);

static void Prj_ShowFormToFilterByMy_All (void);
static void Prj_ShowFormToFilterByPreassignedNonPreassig (void);
static void Prj_ShowFormToFilterByHidden (void);
static void Prj_ShowFormToFilterByDpt (void);

static void Prj_PutCurrentParams (void);
static void Prj_PutHiddenParamMy_All (Prj_WhoseProjects_t My_All);
static void Prj_PutHiddenParamPreNon (unsigned PreNon);
static void Prj_PutHiddenParamHidVis (unsigned HidVis);
static void Prj_PutHiddenParamDptCod (long DptCod);
static void Prj_GetHiddenParamMy_All (void);
static void Prj_GetHiddenParamPreNon (void);
static void Prj_GetHiddenParamHidVis (void);
static void Prj_GetHiddenParamDptCod (void);
static void Prj_GetParams (void);

static void Prj_ShowProjectsHead (Prj_ProjectView_t ProjectView);
static void Prj_ShowTableAllProjectsHead (void);
static bool Prj_CheckIfICanCreateProjects (void);
static void Prj_PutIconsListProjects (void);
static void Prj_PutIconToCreateNewPrj (void);
static void Prj_PutButtonToCreateNewPrj (void);
static void Prj_PutIconToShowAllData (void);
static void Prj_PutIconsToLockUnlockAllProjects (void);

static void Prj_ShowOneProject (unsigned NumIndex,struct Project *Prj,
                                Prj_ProjectView_t ProjectView);
static void Prj_PutIconToToggleProject (unsigned UniqueId,
                                        const char *Icon,const char *Text);
static void Prj_ShowTableAllProjectsOneRow (struct Project *Prj);
static void Prj_ShowOneProjectDepartment (const struct Project *Prj,
                                          Prj_ProjectView_t ProjectView);
static void Prj_ShowTableAllProjectsDepartment (const struct Project *Prj);
static void Prj_ShowOneProjectTxtField (struct Project *Prj,
                                        Prj_ProjectView_t ProjectView,
                                        const char *id,unsigned UniqueId,
                                        const char *Label,char *TxtField);
static void Prj_ShowTableAllProjectsTxtField (struct Project *Prj,
                                              char *TxtField);
static void Prj_ShowOneProjectURL (const struct Project *Prj,
                                   Prj_ProjectView_t ProjectView,
                                   const char *id,unsigned UniqueId);
static void Prj_ShowTableAllProjectsURL (const struct Project *Prj);
static void Prj_ShowOneProjectMembers (struct Project *Prj,
                                       Prj_ProjectView_t ProjectView);
static void Prj_ShowOneProjectMembersWithARole (const struct Project *Prj,
                                                Prj_ProjectView_t ProjectView,
                                                Prj_RoleInProject_t RoleInProject);
static void Prj_ShowTableAllProjectsMembersWithARole (const struct Project *Prj,
                                                      Prj_RoleInProject_t RoleInProject);

static unsigned Prj_GetUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject,
                                  MYSQL_RES **mysql_res);

static Prj_RoleInProject_t Prj_ConvertUnsignedStrToRoleInProject (const char *UnsignedStr);

static void Prj_ReqAddUsrs (Prj_RoleInProject_t RoleInProject);
static void Prj_AddStds (void);
static void Prj_AddTuts (void);
static void Prj_AddEvls (void);
static void Prj_AddUsrsToProject (Prj_RoleInProject_t RoleInProject);
static void Prj_ReqRemUsrFromPrj (Prj_RoleInProject_t RoleInProject);
static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInProject);

static void Prj_GetParamPrjOrder (void);

static void Prj_PutFormsToRemEditOnePrj (const struct Project *Prj,
                                         const char *Anchor,
                                         bool ICanViewProjectFiles);

static bool Prj_CheckIfICanEditProject (const struct Project *Prj);
static bool Prj_CheckIfICanLockProjects (void);
static void Prj_FormLockUnlock (const struct Project *Prj);
static void Prj_PutIconOffLockedUnlocked (const struct Project *Prj);

static void Prj_ResetProject (struct Project *Prj);

static void Prj_LockProjectEditionInDB (long PrjCod);
static void Prj_UnlockProjectEditionInDB (long PrjCod);

static void Prj_RequestCreatOrEditPrj (long PrjCod);
static void Prj_PutFormProject (struct Project *Prj,bool ItsANewProject);
static void Prj_EditOneProjectTxtArea (const char *Id,
                                       const char *Label,char *TxtField,
                                       unsigned NumRows);

static void Prj_CreateProject (struct Project *Prj);
static void Prj_UpdateProject (struct Project *Prj);

/*****************************************************************************/
/***************************** List all projects *****************************/
/*****************************************************************************/

void Prj_SeeProjects (void)
  {
   /***** Get parameters *****/
   Prj_GetParams ();

   /***** Show all the projects *****/
   Prj_ShowProjectsInCurrentPage ();
  }

/*****************************************************************************/
/************************ Show all projects in a table ***********************/
/*****************************************************************************/

void Prj_ShowTableAllProjects (void)
  {
   extern const char *Txt_No_projects;
   unsigned NumPrj;
   struct Project Prj;

   /***** Get parameters *****/
   Prj_GetParams ();

   /***** Get list of projects *****/
   Prj_GetListProjects ();

   if (Gbl.Prjs.Num)
     {
      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Prj);

      /***** Table head *****/
      Tbl_TABLE_BeginWidePadding (2);
      Prj_ShowTableAllProjectsHead ();

      /***** Write all the projects *****/
      for (NumPrj = 0;
	   NumPrj < Gbl.Prjs.Num;
	   NumPrj++)
	{
	 Prj.PrjCod = Gbl.Prjs.LstPrjCods[NumPrj];
	 Prj_ShowTableAllProjectsOneRow (&Prj);
	}

      /***** End table *****/
      Tbl_TABLE_End ();

      /***** Free memory of the project *****/
      Prj_FreeMemProject (&Prj);
     }
   else	// No projects created
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /***** Free list of projects *****/
   Prj_FreeListProjects ();
  }

/*****************************************************************************/
/****************** Show the projects in current page ************************/
/*****************************************************************************/

static void Prj_ShowProjectsInCurrentPage (void)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   extern const char *Txt_Projects;
   extern const char *Txt_No_projects;
   struct Pagination Pagination;
   unsigned NumPrj;
   unsigned NumIndex;
   struct Project Prj;

   /***** Get list of projects *****/
   Prj_GetListProjects ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Prjs.Num;
   Pagination.CurrentPage = (int) Gbl.Prjs.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Prjs.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Start box *****/
   Box_StartBox ("100%",Txt_Projects,Prj_PutIconsListProjects,
                 Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

   /***** Put forms to choice which projects to show *****/
   /* 1st. row */
   Set_StartSettingsHead ();
   Prj_ShowFormToFilterByMy_All ();
   Prj_ShowFormToFilterByPreassignedNonPreassig ();
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
         Prj_ShowFormToFilterByHidden ();
	 break;
      default:	// Students will see only visible projects
         break;
     }
   Set_EndSettingsHead ();
   /* 2nd. row */
   Prj_ShowFormToFilterByDpt ();

   if (Gbl.Prjs.Num)
     {
      /***** Write links to pages *****/
      if (Pagination.MoreThanOnePage)
	 Pag_WriteLinksToPagesCentered (Pag_PROJECTS,
					0,
					&Pagination);

      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Prj);

      /***** Table head *****/
      Tbl_TABLE_BeginWideMarginPadding (2);
      Prj_ShowProjectsHead (Prj_LIST_PROJECTS);

      /***** Write all the projects *****/
      for (NumPrj = Pagination.FirstItemVisible;
	   NumPrj <= Pagination.LastItemVisible;
	   NumPrj++)
	{
         /* Get project data */
	 Prj.PrjCod = Gbl.Prjs.LstPrjCods[NumPrj - 1];
         Prj_GetDataOfProjectByCod (&Prj);

         /* Number of index */
	 switch (Gbl.Prjs.SelectedOrder)
	   {
	    case Prj_ORDER_START_TIME:
	    case Prj_ORDER_END_TIME:
	       // NumPrj: 1, 2, 3 ==> NumIndex = 3, 2, 1
	       NumIndex = Gbl.Prjs.Num + 1 - NumPrj;
	       break;
	    default:
	       // NumPrj: 1, 2, 3 ==> NumIndex = 1, 2, 3
	       NumIndex = NumPrj;
	       break;
	   }

         /* Show project */
	 Prj_ShowOneProject (NumIndex,&Prj,Prj_LIST_PROJECTS);
	}

      /***** End table *****/
      Tbl_TABLE_End ();

      /***** Free memory of the project *****/
      Prj_FreeMemProject (&Prj);

      /***** Write again links to pages *****/
      if (Pagination.MoreThanOnePage)
	 Pag_WriteLinksToPagesCentered (Pag_PROJECTS,
					0,
					&Pagination);
     }
   else	// No projects created
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /***** Button to create a new project *****/
   if (Prj_CheckIfICanCreateProjects ())
      Prj_PutButtonToCreateNewPrj ();

   /***** End box *****/
   Box_EndBox ();

   /***** Free list of projects *****/
   Prj_FreeListProjects ();
  }

/*****************************************************************************/
/*** Show form to choice whether to show only my projects or all projects ****/
/*****************************************************************************/

static void Prj_ShowFormToFilterByMy_All (void)
  {
   extern const char *Txt_PROJECT_MY_ALL_PROJECTS[Prj_NUM_WHOSE_PROJECTS];
   struct Prj_Filter Filter;
   Prj_WhoseProjects_t My_All;
   static const char *WhoseProjectsIcon[Prj_NUM_WHOSE_PROJECTS] =
     {
      "mysitemap.png",	// Prj_MY__PROJECTS
      "sitemap.svg",	// Prj_ALL_PROJECTS
     };

   Set_StartOneSettingSelector ();
   for (My_All =  (Prj_WhoseProjects_t) 0;
	My_All <= (Prj_WhoseProjects_t) (Prj_NUM_WHOSE_PROJECTS - 1);
	My_All++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
	       (Gbl.Prjs.Filter.My_All == My_All) ? "PREF_ON" :
					            "PREF_OFF");
      Frm_StartForm (ActSeePrj);
      Filter.My_All = My_All;
      Filter.PreNon = Gbl.Prjs.Filter.PreNon;
      Filter.HidVis = Gbl.Prjs.Filter.HidVis;
      Filter.DptCod = Gbl.Prjs.Filter.DptCod;
      Prj_PutParams (&Filter,
                     Gbl.Prjs.SelectedOrder,
                     Gbl.Prjs.CurrentPage,
                     -1L);
      Ico_PutSettingIconLink (WhoseProjectsIcon[My_All],
	                   Txt_PROJECT_MY_ALL_PROJECTS[My_All]);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</div>");
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/******** Show form to select preassigned / non-preassigned projects *********/
/*****************************************************************************/

static void Prj_ShowFormToFilterByPreassignedNonPreassig (void)
  {
   extern const char *Txt_PROJECT_PREASSIGNED_NONPREASSIGNED_PLURAL[Prj_NUM_PREASSIGNED_NONPREASSIG];
   struct Prj_Filter Filter;
   Prj_PreassignedNonpreassig_t PreNon;

   Set_StartOneSettingSelector ();
   for (PreNon =  (Prj_PreassignedNonpreassig_t) 0;
	PreNon <= (Prj_PreassignedNonpreassig_t) (Prj_NUM_PREASSIGNED_NONPREASSIG - 1);
	PreNon++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
	       (Gbl.Prjs.Filter.PreNon & (1 << PreNon)) ? "PREF_ON" :
						          "PREF_OFF");
      Frm_StartForm (ActSeePrj);
      Filter.My_All = Gbl.Prjs.Filter.My_All;
      Filter.PreNon = Gbl.Prjs.Filter.PreNon ^ (1 << PreNon);	// Toggle
      Filter.HidVis = Gbl.Prjs.Filter.HidVis;
      Filter.DptCod = Gbl.Prjs.Filter.DptCod;
      Prj_PutParams (&Filter,
                     Gbl.Prjs.SelectedOrder,
                     Gbl.Prjs.CurrentPage,
                     -1L);
      Ico_PutSettingIconLink (PreassignedNonpreassigImage[PreNon],
	                   Txt_PROJECT_PREASSIGNED_NONPREASSIGNED_PLURAL[PreNon]);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</div>");
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/************* Show form to select hidden / visible projects *****************/
/*****************************************************************************/

static void Prj_ShowFormToFilterByHidden (void)
  {
   extern const char *Txt_PROJECT_HIDDEN_VISIBL_PROJECTS[Prj_NUM_HIDDEN_VISIBL];
   struct Prj_Filter Filter;
   Prj_HiddenVisibl_t HidVis;
   static const char *HiddenVisiblIcon[Prj_NUM_HIDDEN_VISIBL] =
     {
      "eye-slash.svg",	// Prj_HIDDEN
      "eye.svg",	// Prj_VISIBL
     };

   Set_StartOneSettingSelector ();
   for (HidVis =  (Prj_HiddenVisibl_t) 0;
	HidVis <= (Prj_HiddenVisibl_t) (Prj_NUM_HIDDEN_VISIBL - 1);
	HidVis++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
	       (Gbl.Prjs.Filter.HidVis & (1 << HidVis)) ? "PREF_ON" :
						          "PREF_OFF");
      Frm_StartForm (ActSeePrj);
      Filter.My_All = Gbl.Prjs.Filter.My_All;
      Filter.PreNon = Gbl.Prjs.Filter.PreNon;
      Filter.HidVis = Gbl.Prjs.Filter.HidVis ^ (1 << HidVis);	// Toggle
      Filter.DptCod = Gbl.Prjs.Filter.DptCod;
      Prj_PutParams (&Filter,
                     Gbl.Prjs.SelectedOrder,
                     Gbl.Prjs.CurrentPage,
                     -1L);
      Ico_PutSettingIconLink (HiddenVisiblIcon[HidVis],
	                      Txt_PROJECT_HIDDEN_VISIBL_PROJECTS[HidVis]);
      Frm_EndForm ();
      fprintf (Gbl.F.Out,"</div>");
     }
   Set_EndOneSettingSelector ();
  }

/*****************************************************************************/
/*************** Show form to filter projects by department ******************/
/*****************************************************************************/

static void Prj_ShowFormToFilterByDpt (void)
  {
   extern const char *Txt_Any_department;
   struct Prj_Filter Filter;

   /***** Start form *****/
   fprintf (Gbl.F.Out,"<div>");
   Frm_StartForm (ActSeePrj);
   Filter.My_All = Gbl.Prjs.Filter.My_All;
   Filter.PreNon = Gbl.Prjs.Filter.PreNon;
   Filter.HidVis = Gbl.Prjs.Filter.HidVis;
   Filter.DptCod = Prj_FILTER_DPT_DEFAULT;	// Do not put department parameter here
   Prj_PutParams (&Filter,
		  Gbl.Prjs.SelectedOrder,
		  Gbl.Prjs.CurrentPage,
		  -1L);

   /***** Write selector with departments *****/
   Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Ins.InsCod,	// Departments in current insitution
                                Gbl.Prjs.Filter.DptCod,		// Selected department
                                "PRJ_INPUT",			// Selector class
                                -1L,				// First option
                                Txt_Any_department,		// Text when no department selected
                                true);				// Submit on change

   /***** End form *****/
   Frm_EndForm ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Put parameters used in projects **********************/
/*****************************************************************************/

static void Prj_PutCurrentParams (void)
  {
   Prj_PutParams (&Gbl.Prjs.Filter,
                  Gbl.Prjs.SelectedOrder,
		  Gbl.Prjs.CurrentPage,
                  Gbl.Prjs.PrjCod);
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
   if (Filter->My_All != Prj_FILTER_WHOSE_PROJECTS_DEFAULT)
      Prj_PutHiddenParamMy_All (Filter->My_All);

   if (Filter->PreNon != ((unsigned) Prj_FILTER_PREASSIGNED_DEFAULT |
	                  (unsigned) Prj_FILTER_NONPREASSIG_DEFAULT))
      Prj_PutHiddenParamPreNon (Filter->PreNon);

   if (Filter->HidVis != ((unsigned) Prj_FILTER_HIDDEN_DEFAULT |
	                  (unsigned) Prj_FILTER_VISIBL_DEFAULT))
      Prj_PutHiddenParamHidVis (Filter->HidVis);

   if (Filter->DptCod != Prj_FILTER_DPT_DEFAULT)
      Prj_PutHiddenParamDptCod (Filter->DptCod);

   /***** Put order field *****/
   if (Order != Prj_ORDER_DEFAULT)
      Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);

   /***** Put number of page *****/
   if (NumPage > 1)
      Pag_PutHiddenParamPagNum (Pag_PROJECTS,NumPage);

   /***** Put selected project code *****/
   if (PrjCod > 0)
      Prj_PutParamPrjCod (PrjCod);

   /***** Put another user's code *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted ();
  }

/*****************************************************************************/
/*********************** Put hidden params for projects **********************/
/*****************************************************************************/

static void Prj_PutHiddenParamMy_All (Prj_WhoseProjects_t My_All)
  {
   Par_PutHiddenParamUnsigned (Prj_PARAM_MY__ALL_NAME,(unsigned) My_All);
  }

static void Prj_PutHiddenParamPreNon (unsigned PreNon)
  {
   Par_PutHiddenParamUnsigned (Prj_PARAM_PRE_NON_NAME,PreNon);
  }

static void Prj_PutHiddenParamHidVis (unsigned HidVis)
  {
   Par_PutHiddenParamUnsigned (Prj_PARAM_HID_VIS_NAME,HidVis);
  }

static void Prj_PutHiddenParamDptCod (long DptCod)
  {
   Par_PutHiddenParamUnsigned (Dpt_PARAM_DPT_COD_NAME,DptCod);
  }

/*****************************************************************************/
/*********************** Get hidden params for projects **********************/
/*****************************************************************************/

static void Prj_GetHiddenParamMy_All (void)
  {
   Gbl.Prjs.Filter.My_All = (Prj_WhoseProjects_t) Par_GetParToUnsignedLong (Prj_PARAM_MY__ALL_NAME,
                                                                            0,
                                                                            Prj_NUM_WHOSE_PROJECTS - 1,
                                                                            Prj_FILTER_WHOSE_PROJECTS_DEFAULT);
  }

static void Prj_GetHiddenParamPreNon (void)
  {
   Gbl.Prjs.Filter.PreNon = (unsigned) Par_GetParToUnsignedLong (Prj_PARAM_PRE_NON_NAME,
                                                                 0,
                                                                 (1 << Prj_PREASSIGNED) |
                                                                 (1 << Prj_NONPREASSIG),
                                                                 (unsigned) Prj_FILTER_PREASSIGNED_DEFAULT |
                                                                 (unsigned) Prj_FILTER_NONPREASSIG_DEFAULT);
  }

static void Prj_GetHiddenParamHidVis (void)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:	// Students can view only visible projects
	 Gbl.Prjs.Filter.HidVis = (1 << Prj_VISIBL);	// Only visible projects
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 Gbl.Prjs.Filter.HidVis = (unsigned) Par_GetParToUnsignedLong (Prj_PARAM_HID_VIS_NAME,
								       0,
								       (1 << Prj_HIDDEN) |
								       (1 << Prj_VISIBL),
								       (unsigned) Prj_FILTER_HIDDEN_DEFAULT |
								       (unsigned) Prj_FILTER_VISIBL_DEFAULT);
	 break;
      default:
	 Rol_WrongRoleExit ();
         break;
     }
  }

static void Prj_GetHiddenParamDptCod (void)
  {
   Gbl.Prjs.Filter.DptCod = Par_GetParToLong (Dpt_PARAM_DPT_COD_NAME);
  }

/*****************************************************************************/
/***************** Get generic parameters to list projects *******************/
/*****************************************************************************/

static void Prj_GetParams (void)
  {
   /***** Get filter (which projects to show) *****/
   Prj_GetHiddenParamMy_All ();
   Prj_GetHiddenParamPreNon ();
   Prj_GetHiddenParamHidVis ();
   Prj_GetHiddenParamDptCod ();

   /***** Get order and page *****/
   Prj_GetParamPrjOrder ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
  }

/*****************************************************************************/
/******************* Write header with fields of a project *******************/
/*****************************************************************************/

static void Prj_ShowProjectsHead (Prj_ProjectView_t ProjectView)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_PROJECT_ORDER_HELP[Prj_NUM_ORDERS];
   extern const char *Txt_PROJECT_ORDER[Prj_NUM_ORDERS];
   Prj_Order_t Order;

   Tbl_TR_Begin (NULL);

   /***** Column for number of project *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 fprintf (Gbl.F.Out,"<th class=\"CENTER_MIDDLE\">%s</th>",Txt_No_INDEX);
	 break;
      default:
	 break;
     }

   /***** Column for contextual icons *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
	 fprintf (Gbl.F.Out,"<th class=\"CONTEXT_COL\"></th>");
	 break;
      default:
	 break;
     }

   /***** Rest of columns *****/
   for (Order = (Prj_Order_t) 0;
	Order <= (Prj_Order_t) (Prj_NUM_ORDERS - 1);
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"CENTER_MIDDLE\">");

      switch (ProjectView)
	{
	 case Prj_LIST_PROJECTS:
	 case Prj_FILE_BROWSER_PROJECT:
	    Frm_StartForm (ActSeePrj);
	    Prj_PutParams (&Gbl.Prjs.Filter,
			   Order,
			   Gbl.Prjs.CurrentPage,
			   -1L);
	    Frm_LinkFormSubmit (Txt_PROJECT_ORDER_HELP[Order],"TIT_TBL",NULL);
	    if (Order == Gbl.Prjs.SelectedOrder)
	       fprintf (Gbl.F.Out,"<u>");
            fprintf (Gbl.F.Out,"%s",Txt_PROJECT_ORDER[Order]);
	    if (Order == Gbl.Prjs.SelectedOrder)
	       fprintf (Gbl.F.Out,"</u>");
	    fprintf (Gbl.F.Out,"</a>");
	    Frm_EndForm ();
	    break;
	 default:
            fprintf (Gbl.F.Out,"%s",Txt_PROJECT_ORDER[Order]);
	    break;
	}

      fprintf (Gbl.F.Out,"</th>");
     }
  }

static void Prj_ShowTableAllProjectsHead (void)
  {
   extern const char *Txt_PROJECT_ORDER[Prj_NUM_ORDERS];
   extern const char *Txt_Preassigned_QUESTION;
   extern const char *Txt_Number_of_students;
   extern const char *Txt_PROJECT_ROLES_PLURAL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_Proposal;
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   extern const char *Txt_URL;
   Prj_Order_t Order;
   unsigned NumRoleToShow;

   Tbl_TR_Begin (NULL);

   for (Order = (Prj_Order_t) 0;
	Order <= (Prj_Order_t) (Prj_NUM_ORDERS - 1);
	Order++)
      fprintf (Gbl.F.Out,"<th class=\"LEFT_TOP DAT_N\">%s</th>",
               Txt_PROJECT_ORDER[Order]);

   fprintf (Gbl.F.Out,"<th class=\"LEFT_TOP DAT_N\">%s</th>"
	              "<th class=\"LEFT_TOP DAT_N\">%s</th>",
            Txt_Preassigned_QUESTION,
            Txt_Number_of_students);

   for (NumRoleToShow = 0;
	NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	NumRoleToShow++)
      fprintf (Gbl.F.Out,"<th class=\"LEFT_TOP DAT_N\">%s</th>",
	       Txt_PROJECT_ROLES_PLURAL_Abc[Prj_RolesToShow[NumRoleToShow]]);

   fprintf (Gbl.F.Out,"<th class=\"LEFT_TOP DAT_N\">%s</th>"
                      "<th class=\"LEFT_TOP DAT_N\">%s</th>"
                      "<th class=\"LEFT_TOP DAT_N\">%s</th>"
                      "<th class=\"LEFT_TOP DAT_N\">%s</th>"
                      "<th class=\"LEFT_TOP DAT_N\">%s</th>",
            Txt_Proposal,
            Txt_Description,
            Txt_Required_knowledge,
            Txt_Required_materials,
            Txt_URL);

   Tbl_TR_End ();
  }

/*****************************************************************************/
/********************** Check if I can create projects ***********************/
/*****************************************************************************/

static bool Prj_CheckIfICanCreateProjects (void)
  {
   static const bool ICanCreateProjects[Rol_NUM_ROLES] =
     {
      false,	// Rol_UNK
      false,	// Rol_GST
      false,	// Rol_USR
      false,	// Rol_STD
      true,	// Rol_NET
      true,	// Rol_TCH
      true,	// Rol_DEG_ADM
      true,	// Rol_CTR_ADM
      true,	// Rol_INS_ADM
      true,	// Rol_SYS_ADM
     };

   return ICanCreateProjects[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/***************** Put contextual icons in list of projects ******************/
/*****************************************************************************/

static void Prj_PutIconsListProjects (void)
  {
   /***** Put icon to create a new project *****/
   if (Prj_CheckIfICanCreateProjects ())
      Prj_PutIconToCreateNewPrj ();

   if (Gbl.Prjs.Num)
     {
      /***** Put icon to show all data in a table *****/
      Prj_PutIconToShowAllData ();

      /***** Put icons to lock/unlock edition of all projects *****/
      if (Prj_CheckIfICanLockProjects ())
	 Prj_PutIconsToLockUnlockAllProjects ();
     }

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_PROJECTS;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************* Put icon to create a new project **********************/
/*****************************************************************************/

static void Prj_PutIconToCreateNewPrj (void)
  {
   extern const char *Txt_New_project;

   /***** Put form to create a new project *****/
   Gbl.Prjs.PrjCod = -1L;
   Ico_PutContextualIconToAdd (ActFrmNewPrj,NULL,Prj_PutCurrentParams,
			       Txt_New_project);
  }

/*****************************************************************************/
/******************** Put button to create a new project *********************/
/*****************************************************************************/

static void Prj_PutButtonToCreateNewPrj (void)
  {
   extern const char *Txt_New_project;

   Gbl.Prjs.PrjCod = -1L;
   Frm_StartForm (ActFrmNewPrj);
   Prj_PutCurrentParams ();
   Btn_PutConfirmButton (Txt_New_project);
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Put button to create a new project *********************/
/*****************************************************************************/

static void Prj_PutIconToShowAllData (void)
  {
   extern const char *Txt_Show_all_data_in_a_table;

   Lay_PutContextualLinkOnlyIcon (ActSeeTblAllPrj,NULL,Prj_PutCurrentParams,
			          "table.svg",
				  Txt_Show_all_data_in_a_table);
  }

/*****************************************************************************/
/****** Put icons to request locking/unlocking edition of all projects *******/
/*****************************************************************************/

static void Prj_PutIconsToLockUnlockAllProjects (void)
  {
   extern const char *Txt_Lock_editing;
   extern const char *Txt_Unlock_editing;

   /***** Put icon to lock all projects *****/
   Lay_PutContextualLinkOnlyIcon (ActReqLckAllPrj,NULL,Prj_PutCurrentParams,
			          "lock.svg",
				  Txt_Lock_editing);

   /***** Put icon to unlock all projects *****/
   Lay_PutContextualLinkOnlyIcon (ActReqUnlAllPrj,NULL,Prj_PutCurrentParams,
			          "unlock.svg",
				  Txt_Unlock_editing);
  }

/*****************************************************************************/
/***************** View / edit file browser of one project *******************/
/*****************************************************************************/

void Prj_ShowOneUniqueProject (struct Project *Prj)
  {
   /***** Start table *****/
   Tbl_TABLE_BeginWidePadding (2);

   /***** Write project head *****/
   Prj_ShowProjectsHead (Prj_FILE_BROWSER_PROJECT);

   /***** Show project *****/
   Prj_ShowOneProject (0,Prj,Prj_FILE_BROWSER_PROJECT);

   /***** End table *****/
   Tbl_TABLE_End ();
  }

/*****************************************************************************/
/********************** Show print view of one project ***********************/
/*****************************************************************************/

void Prj_PrintOneProject (void)
  {
   struct Project Prj;

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
   Tbl_TABLE_BeginWideMarginPadding (2);
   Prj_ShowProjectsHead (Prj_PRINT_ONE_PROJECT);

   /***** Write project *****/
   Prj_ShowOneProject (0,&Prj,Prj_PRINT_ONE_PROJECT);

   /***** End table *****/
   Tbl_TABLE_End ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/***************************** Show one project ******************************/
/*****************************************************************************/

static void Prj_ShowOneProject (unsigned NumIndex,struct Project *Prj,
                                Prj_ProjectView_t ProjectView)
  {
   extern const char *Txt_Today;
   extern const char *Txt_Project_files;
   extern const char *Txt_Preassigned_QUESTION;
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_PROJECT_PREASSIGNED_NONPREASSIGNED_SINGUL[Prj_NUM_PREASSIGNED_NONPREASSIG];
   extern const char *Txt_Number_of_students;
   extern const char *Txt_See_more;
   extern const char *Txt_See_less;
   extern const char *Txt_Proposal;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   char *Anchor = NULL;
   static unsigned UniqueId = 0;
   bool ICanViewProjectFiles = Prj_CheckIfICanViewProjectFiles (Prj_GetMyRolesInProject (Prj->PrjCod));

   /***** Set anchor string *****/
   Frm_SetAnchorStr (Prj->PrjCod,&Anchor);

   /***** Write first row of data of this project *****/
   Tbl_TR_Begin (NULL);

   /* Number of project */
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TD_Begin ("rowspan=\"3\" class=\"BIG_INDEX %s RIGHT_TOP COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "DATE_BLUE_LIGHT" :
						   "DATE_BLUE",
		       Gbl.RowEvenOdd);
	 fprintf (Gbl.F.Out,"%u",NumIndex);
	 Tbl_TD_End ();
	 break;
      default:
	 break;
     }

   /* Forms to remove/edit this project */
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         Tbl_TD_Begin ("rowspan=\"3\" class=\"CONTEXT_COL COLOR%u\"",Gbl.RowEvenOdd);
         Prj_PutFormsToRemEditOnePrj (Prj,Anchor,ICanViewProjectFiles);
         Tbl_TD_End ();
	 break;
      case Prj_FILE_BROWSER_PROJECT:
         Tbl_TD_Begin ("rowspan=\"3\" class=\"CONTEXT_COL\"");
         Prj_PutFormsToRemEditOnePrj (Prj,Anchor,ICanViewProjectFiles);
         Tbl_TD_End ();
	 break;
      default:
	 break;
     }

   /* Creation date/time */
   UniqueId++;
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TD_Begin ("id=\"prj_creat_%u\" class=\"%s LEFT_TOP COLOR%u\"",
		       UniqueId,
		       Prj->Hidden == Prj_HIDDEN ? "DATE_BLUE_LIGHT" :
						   "DATE_BLUE",
		       Gbl.RowEvenOdd);
	 break;
      default:
	 Tbl_TD_Begin ("id=\"prj_creat_%u\" class=\"%s LEFT_TOP\"",
		       UniqueId,
		       Prj->Hidden == Prj_HIDDEN ? "DATE_BLUE_LIGHT" :
						   "DATE_BLUE");
	 break;
     }
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('prj_creat_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>",
            UniqueId,Prj->CreatTime,
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
   Tbl_TD_End ();

   /* Modification date/time */
   UniqueId++;
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TD_Begin ("id=\"prj_modif_%u\" class=\"%s LEFT_TOP COLOR%u\"",
		       UniqueId,
		       Prj->Hidden == Prj_HIDDEN ? "DATE_BLUE_LIGHT" :
						   "DATE_BLUE",
		       Gbl.RowEvenOdd);
	 break;
      default:
	 Tbl_TD_Begin ("id=\"prj_modif_%u\" class=\"%s LEFT_TOP\"",
		       UniqueId,
		       Prj->Hidden == Prj_HIDDEN ? "DATE_BLUE_LIGHT" :
						   "DATE_BLUE");
	 break;
     }
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('prj_modif_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>",
            UniqueId,Prj->ModifTime,
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
   Tbl_TD_End ();

   /* Project title */
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TD_Begin ("class=\"%s LEFT_TOP COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_TITLE_LIGHT" :
						   "ASG_TITLE",
		       Gbl.RowEvenOdd);
	 break;
      default:
	 Tbl_TD_Begin ("class=\"%s LEFT_TOP\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_TITLE_LIGHT" :
						   "ASG_TITLE");
	 break;
     }
   Lay_StartArticle (Anchor);
   if (ICanViewProjectFiles)
     {
      Frm_StartForm (ActAdmDocPrj);
      Prj_PutCurrentParams ();
      Frm_LinkFormSubmit (Txt_Project_files,
                          Prj->Hidden == Prj_HIDDEN ? "ASG_TITLE_LIGHT" :
        	                                      "ASG_TITLE",
        	          NULL);
      fprintf (Gbl.F.Out,"%s</a>",Prj->Title);
      Frm_EndForm ();
     }
   else
      fprintf (Gbl.F.Out,"%s",Prj->Title);
   Lay_EndArticle ();
   Tbl_TD_End ();

   /* Department */
   Prj_ShowOneProjectDepartment (Prj,ProjectView);

   /***** Preassigned? *****/
   Tbl_TR_Begin (NULL);

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL",
		       Gbl.RowEvenOdd);
	 break;
      default:
         Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL");
         break;
     }
   fprintf (Gbl.F.Out,"%s:",Txt_Preassigned_QUESTION);
   Tbl_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT",
		       Gbl.RowEvenOdd);
         break;
      default:
         Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT");
         break;
     }
   fprintf (Gbl.F.Out,"%s&nbsp;",(Prj->Preassigned == Prj_PREASSIGNED) ? Txt_Yes :
        	                                                         Txt_No);
   Ico_PutIconOff (PreassignedNonpreassigImage[Prj->Preassigned],
		   Txt_PROJECT_PREASSIGNED_NONPREASSIGNED_SINGUL[Prj->Preassigned]);
   Tbl_TD_End ();

   Tbl_TR_End ();

   /***** Number of students *****/
   Tbl_TR_Begin (NULL);

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL",
		       Gbl.RowEvenOdd);
         break;
      default:
         Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL");
         break;
     }
   fprintf (Gbl.F.Out,"%s:",Txt_Number_of_students);
   Tbl_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT",
		       Gbl.RowEvenOdd);
         break;
      default:
         Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT");
         break;
     }
   fprintf (Gbl.F.Out,"%u",Prj->NumStds);
   Tbl_TD_End ();

   Tbl_TR_End ();

   /***** Project members *****/
   Prj_ShowOneProjectMembers (Prj,ProjectView);

   /***** Link to show hidden info *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TR_Begin ("id=\"prj_exp_%u\"",UniqueId);
	 Tbl_TD_Begin ("colspan=\"6\" class=\"CENTER_MIDDLE COLOR%u\"",Gbl.RowEvenOdd);
	 Prj_PutIconToToggleProject (UniqueId,"angle-down.svg",Txt_See_more);
	 Tbl_TD_End ();
	 Tbl_TR_End ();

	 Tbl_TR_Begin ("id=\"prj_con_%u\" style=\"display:none;\"",UniqueId);
	 Tbl_TD_Begin ("colspan=\"6\" class=\"CENTER_MIDDLE COLOR%u\"",Gbl.RowEvenOdd);
	 Prj_PutIconToToggleProject (UniqueId,"angle-up.svg",Txt_See_less);
	 Tbl_TD_End ();
	 Tbl_TR_End ();
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 Tbl_TR_Begin ("id=\"prj_exp_%u\"",UniqueId);
	 Tbl_TD_Begin ("colspan=\"5\" class=\"CENTER_MIDDLE\"");
	 Prj_PutIconToToggleProject (UniqueId,"angle-down.svg",Txt_See_more);
	 Tbl_TD_End ();
	 Tbl_TR_End ();

	 Tbl_TR_Begin ("id=\"prj_con_%u\" style=\"display:none;\"",UniqueId);
	 Tbl_TD_Begin ("colspan=\"5\" class=\"CENTER_MIDDLE\"");
	 Prj_PutIconToToggleProject (UniqueId,"angle-up.svg",Txt_See_less);
	 Tbl_TD_End ();
	 Tbl_TR_End ();
	 break;
      default:
	 break;
     }

   /***** Proposal *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TR_Begin ("id=\"prj_pro_%u\" style=\"display:none;\"",UniqueId);
	 Tbl_TD_Begin ("colspan=\"4\" class=\"RIGHT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL",
		       Gbl.RowEvenOdd);
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 Tbl_TR_Begin ("id=\"prj_pro_%u\" style=\"display:none;\"",UniqueId);
	 Tbl_TD_Begin ("colspan=\"3\" class=\"RIGHT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL");
	 break;
      default:
	 Tbl_TR_Begin (NULL);
	 Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL");
	 break;
     }
   fprintf (Gbl.F.Out,"%s:",Txt_Proposal);
   Tbl_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT",
		       Gbl.RowEvenOdd);
	 break;
      default:
	 Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT");
	 break;
     }
   fprintf (Gbl.F.Out,"%s",Txt_PROJECT_STATUS[Prj->Proposal]);
   Tbl_TD_End ();

   Tbl_TR_End ();

   /***** Write rows of data of this project *****/
   /* Description of the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,"prj_dsc_",UniqueId,
                               Txt_Description,Prj->Description);

   /* Required knowledge to carry out the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,"prj_knw_",UniqueId,
                               Txt_Required_knowledge,Prj->Knowledge);

   /* Required materials to carry out the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,"prj_mtr_",UniqueId,
                               Txt_Required_materials,Prj->Materials);

   /* Link to view more info about the project */
   Prj_ShowOneProjectURL (Prj,ProjectView,"prj_url_",UniqueId);

   /***** Free anchor string *****/
   Frm_FreeAnchorStr (Anchor);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off some fields of a project ************/
/*****************************************************************************/

static void Prj_PutIconToToggleProject (unsigned UniqueId,
                                        const char *Icon,const char *Text)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];

   /***** Link to toggle on/off some fields of project *****/
   fprintf (Gbl.F.Out,"<a href=\"\" title=\"%s\" class=\"%s\""
                      " onclick=\"toggleProject('%u');return false;\" />",
            Text,The_ClassFormInBox[Gbl.Prefs.Theme],
            UniqueId);
   Ico_PutIconTextLink (Icon,Text);
   fprintf (Gbl.F.Out,"</a>");
  }

/*****************************************************************************/
/***************** Show one row of table with all projects *******************/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsOneRow (struct Project *Prj)
  {
   extern const char *Txt_Today;
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];
   unsigned NumRoleToShow;
   static unsigned UniqueId = 0;

   /***** Get data of this project *****/
   Prj_GetDataOfProjectByCod (Prj);

   /***** Start row *****/
   Tbl_TR_Begin (NULL);

   /***** Start date/time *****/
   UniqueId++;
   Tbl_TD_Begin ("id=\"prj_creat_%u\" class=\"LEFT_TOP %s COLOR%u\"",
		 UniqueId,
		 Prj->Hidden == Prj_HIDDEN ? "DATE_BLUE_LIGHT" :
					     "DATE_BLUE",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('prj_creat_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>",
            UniqueId,Prj->CreatTime,
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
   Tbl_TD_End ();

   /***** End date/time *****/
   UniqueId++;
   Tbl_TD_Begin ("id=\"prj_modif_%u\" class=\"LEFT_TOP %s COLOR%u\"",
		 UniqueId,
		 Prj->Hidden == Prj_HIDDEN ? "DATE_BLUE_LIGHT" :
					     "DATE_BLUE",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('prj_modif_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>",
            UniqueId,Prj->ModifTime,
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);
   Tbl_TD_End ();

   /***** Project title *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT_N",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"%s",Prj->Title);
   Tbl_TD_End ();

   /***** Department *****/
   Prj_ShowTableAllProjectsDepartment (Prj);

   /***** Preassigned? *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"%s",(Prj->Preassigned == Prj_PREASSIGNED) ? Txt_Yes :
        	                                                   Txt_No);
   Tbl_TD_End ();

   /***** Number of students *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"%u",Prj->NumStds);
   Tbl_TD_End ();

   /***** Project members *****/
   for (NumRoleToShow = 0;
	NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	NumRoleToShow++)
      Prj_ShowTableAllProjectsMembersWithARole (Prj,Prj_RolesToShow[NumRoleToShow]);

   /***** Proposal *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"%s",Txt_PROJECT_STATUS[Prj->Proposal]);
   Tbl_TD_End ();

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
   Tbl_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/****************** Show department associated to project ********************/
/*****************************************************************************/

static void Prj_ShowOneProjectDepartment (const struct Project *Prj,
                                          Prj_ProjectView_t ProjectView)
  {
   struct Department Dpt;
   bool PutLink;

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
	 Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT_N",
		       Gbl.RowEvenOdd);
	 break;
      default:
	 Tbl_TD_Begin ("class=\"LEFT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT_N");
	 break;
     }
   if (PutLink)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
			 " class=\"%s\">",
	       Dpt.WWW,
               Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
        	                           "DAT_N");
   fprintf (Gbl.F.Out,"%s",Dpt.FullName);
   if (PutLink)
      fprintf (Gbl.F.Out,"</a>");
   Tbl_TD_End ();
   Tbl_TR_End ();
  }

static void Prj_ShowTableAllProjectsDepartment (const struct Project *Prj)
  {
   struct Department Dpt;

   /***** Get data of department *****/
   Dpt.DptCod = Prj->DptCod;
   Dpt_GetDataOfDepartmentByCod (&Dpt);

   /***** Show department *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"%s",Dpt.FullName);
   Tbl_TD_End ();
  }

/*****************************************************************************/
/********************** Show text field about a project **********************/
/*****************************************************************************/

static void Prj_ShowOneProjectTxtField (struct Project *Prj,
                                        Prj_ProjectView_t ProjectView,
                                        const char *id,unsigned UniqueId,
                                        const char *Label,char *TxtField)
  {
   /***** Change format *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     TxtField,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
         Str_InsertLinks (TxtField,Cns_MAX_BYTES_TEXT,60);		// Insert links
	 break;
      default:
	 break;
     }

   /***** Write row with label and text *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 Tbl_TD_Begin ("colspan=\"4\" class=\"RIGHT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL",
		       Gbl.RowEvenOdd);
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 Tbl_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 Tbl_TD_Begin ("colspan=\"3\" class=\"RIGHT_TOP %s\"",
			    Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						        "ASG_LABEL");
	 break;
      case Prj_PRINT_ONE_PROJECT:
	 Tbl_TR_Begin (NULL);
	 Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL");
	 break;
      default:
	 // Not applicable
	 break;
     }
   fprintf (Gbl.F.Out,"%s:",Label);
   Tbl_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT",
		       Gbl.RowEvenOdd);
	 break;
      default:
	 Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT");
	 break;
     }
   fprintf (Gbl.F.Out,"%s",TxtField);
   Tbl_TD_End ();

   Tbl_TR_End ();
  }

static void Prj_ShowTableAllProjectsTxtField (struct Project *Prj,
                                              char *TxtField)
  {
   /***** Change format *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     TxtField,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML

   /***** Write text *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"%s",TxtField);
   Tbl_TD_End ();
  }

/*****************************************************************************/
/********************** Show URL associated to project ***********************/
/*****************************************************************************/

static void Prj_ShowOneProjectURL (const struct Project *Prj,
                                   Prj_ProjectView_t ProjectView,
                                   const char *id,unsigned UniqueId)
  {
   extern const char *Txt_URL;
   bool PutLink = (Prj->URL[0] &&
	           (ProjectView == Prj_LIST_PROJECTS ||
	            ProjectView == Prj_FILE_BROWSER_PROJECT));

   /***** Write row with label and text *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 Tbl_TD_Begin ("colspan=\"4\" class=\"RIGHT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL",
		       Gbl.RowEvenOdd);
	 break;
      case Prj_FILE_BROWSER_PROJECT:
	 Tbl_TR_Begin ("id=\"%s%u\" style=\"display:none;\"",id,UniqueId);
	 Tbl_TD_Begin ("colspan=\"3\" class=\"RIGHT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL");
	 break;
      case Prj_PRINT_ONE_PROJECT:
	 Tbl_TR_Begin (NULL);
	 Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						   "ASG_LABEL");
	 break;
      default:
	 // Not applicable
	 break;
     }
   fprintf (Gbl.F.Out,"%s:",Txt_URL);
   Tbl_TD_End ();

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s COLOR%u\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT",
		       Gbl.RowEvenOdd);
	 break;
      default:
	 Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s\"",
		       Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						   "DAT");
	 break;
     }
   if (PutLink)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",Prj->URL);
   fprintf (Gbl.F.Out,"%s",Prj->URL);
   if (PutLink)
      fprintf (Gbl.F.Out,"</a>");
   Tbl_TD_End ();

   Tbl_TR_End ();
  }

static void Prj_ShowTableAllProjectsURL (const struct Project *Prj)
  {
   /***** Show URL *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT",
		 Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"%s",Prj->URL);
   Tbl_TD_End ();
  }

/*****************************************************************************/
/************** Show projects members when showing one project ***************/
/*****************************************************************************/

static void Prj_ShowOneProjectMembers (struct Project *Prj,
                                       Prj_ProjectView_t ProjectView)
  {
   unsigned NumRoleToShow;

   for (NumRoleToShow = 0;
	NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	NumRoleToShow++)
      Prj_ShowOneProjectMembersWithARole (Prj,ProjectView,
                                          Prj_RolesToShow[NumRoleToShow]);
  }

/*****************************************************************************/
/************************* Show users row in a project ***********************/
/*****************************************************************************/

static void Prj_ShowOneProjectMembersWithARole (const struct Project *Prj,
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
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   static const Act_Action_t ActionReqRemUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActReqRemStdPrj,	// Prj_ROLE_STD, Student
      ActReqRemTutPrj,	// Prj_ROLE_TUT, Tutor
      ActReqRemEvlPrj,	// Prj_ROLE_EVL, Evaluator
     };
   static const Act_Action_t ActionReqAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActReqAddStdPrj,	// Prj_ROLE_STD, Student
      ActReqAddTutPrj,	// Prj_ROLE_TUT, Tutor
      ActReqAddEvlPrj,	// Prj_ROLE_EVL, Evaluator
     };

   /***** Get users in project from database *****/
   NumUsrs = Prj_GetUsrsInPrj (Prj->PrjCod,RoleInProject,&mysql_res);
   WriteRow = (NumUsrs != 0 ||
	       ProjectView == Prj_EDIT_ONE_PROJECT);

   if (WriteRow)
     {
      /***** Start row with label and listing of users *****/
      Tbl_TR_Begin (NULL);

      /* Column for label */
      switch (ProjectView)
	{
	 case Prj_LIST_PROJECTS:
	    Tbl_TD_Begin ("colspan=\"4\" class=\"RIGHT_TOP %s COLOR%u\"",
			  Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						      "ASG_LABEL",
			  Gbl.RowEvenOdd);
	    fprintf (Gbl.F.Out,"%s:",
		     NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInProject] :
		                    Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	    break;
	 case Prj_FILE_BROWSER_PROJECT:
	    Tbl_TD_Begin ("colspan=\"3\" class=\"RIGHT_TOP %s\"",
			  Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						      "ASG_LABEL");
	    fprintf (Gbl.F.Out,"%s:",
		     NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInProject] :
		                    Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	    break;
	 case Prj_PRINT_ONE_PROJECT:
	    Tbl_TD_Begin ("colspan=\"2\" class=\"RIGHT_TOP %s\"",
			  Prj->Hidden == Prj_HIDDEN ? "ASG_LABEL_LIGHT" :
						      "ASG_LABEL");
	    fprintf (Gbl.F.Out,"%s:",
		     NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInProject] :
		                    Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	    break;
	 case Prj_EDIT_ONE_PROJECT:
	    Tbl_TD_Begin ("class=\"RIGHT_TOP ASG_LABEL\"");
	    fprintf (Gbl.F.Out,"%s:",Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	    break;
	}
      Tbl_TD_End ();

      /* Start column with list of users */
      switch (ProjectView)
	{
	 case Prj_LIST_PROJECTS:
	    Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s COLOR%u\"",
			  Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						      "DAT",
			  Gbl.RowEvenOdd);
	    break;
	 case Prj_FILE_BROWSER_PROJECT:
	 case Prj_PRINT_ONE_PROJECT:
	    Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP %s\"",
			 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
						     "DAT");
	    break;
	 case Prj_EDIT_ONE_PROJECT:
	    Tbl_TD_Begin ("colspan=\"2\" class=\"LEFT_TOP DAT\"");
	    break;
	}

      /***** Start table with all members with this role *****/
      Tbl_TABLE_BeginPadding (2);

      /***** Write users *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 /* Get user's code */
	 row = mysql_fetch_row (mysql_res);
	 Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get user's data */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	   {
	    /* Start row for this user */
	    Tbl_TR_Begin (NULL);

	    /* Icon to remove user */
	    if (ProjectView == Prj_EDIT_ONE_PROJECT)
	      {
	       Tbl_TD_Begin ("class=\"PRJ_MEMBER_ICO\"");
	       Lay_PutContextualLinkOnlyIcon (ActionReqRemUsr[RoleInProject],NULL,
					      Prj_PutCurrentParams,
					      "trash.svg",
					      Txt_Remove);
	       Tbl_TD_End ();
	      }

	    /* Put user's photo */
	    Tbl_TD_Begin ("class=\"PRJ_MEMBER_PHO\"");
	    ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Other.UsrDat,PhotoURL);
	    Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,ShowPhoto ? PhotoURL :
								 NULL,
			      "PHOTO21x28",Pho_ZOOM,false);
	    Tbl_TD_End ();

	    /* Write user's name */
	    Tbl_TD_Begin ("class=\"PRJ_MEMBER_NAM\"");
	    fprintf (Gbl.F.Out,"%s",Gbl.Usrs.Other.UsrDat.FullName);
	    Tbl_TD_End ();

	    /* End row for this user */
	    Tbl_TR_End ();
	   }
	}

      /***** Row to add a new user *****/
      switch (ProjectView)
	{
	 case Prj_EDIT_ONE_PROJECT:
	    Tbl_TR_Begin (NULL);
	    Tbl_TD_Begin ("class=\"PRJ_MEMBER_ICO\"");
	    Gbl.Prjs.PrjCod = Prj->PrjCod;	// Used to pass project code as a parameter
	    snprintf (Gbl.Title,sizeof (Gbl.Title),
		      Txt_Add_USERS,
		      Txt_PROJECT_ROLES_PLURAL_abc[RoleInProject]);
	    Ico_PutContextualIconToAdd (ActionReqAddUsr[RoleInProject],NULL,
				        Prj_PutCurrentParams,
				        Gbl.Title);
	    Tbl_TD_End ();

	    Tbl_TD_Begin ("class=\"PRJ_MEMBER_PHO\"");	// Column for photo
	    Tbl_TD_End ();

	    Tbl_TD_Begin ("class=\"PRJ_MEMBER_NAM\"");	// Column for name
	    Tbl_TD_End ();

	    Tbl_TR_End ();
	    break;
	 default:
	    break;
	}

      /***** End table with all members with this role *****/
      Tbl_TABLE_End ();

      /***** End row with label and listing of users *****/
      Tbl_TD_End ();
      Tbl_TR_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

static void Prj_ShowTableAllProjectsMembersWithARole (const struct Project *Prj,
                                                      Prj_RoleInProject_t RoleInProject)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned NumUsrs;

   /***** Get users in project from database *****/
   NumUsrs = Prj_GetUsrsInPrj (Prj->PrjCod,RoleInProject,&mysql_res);

   /***** Start column with list of all members with this role *****/
   Tbl_TD_Begin ("class=\"LEFT_TOP %s COLOR%u\"",
		 Prj->Hidden == Prj_HIDDEN ? "DAT_LIGHT" :
					     "DAT",
		 Gbl.RowEvenOdd);

   if (NumUsrs)
     {
      /***** Write users *****/
      fprintf (Gbl.F.Out,"<ul class=\"PRJ_LST_USR\">");

      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 /* Get user's code */
	 row = mysql_fetch_row (mysql_res);
	 Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get user's data */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
	   {
	    /* Write user's name in "Surname1 Surname2, FirstName" format */
            fprintf (Gbl.F.Out,"<li>%s",Gbl.Usrs.Other.UsrDat.Surname1);
	    if (Gbl.Usrs.Other.UsrDat.Surname2[0])
               fprintf (Gbl.F.Out," %s",Gbl.Usrs.Other.UsrDat.Surname2);
            fprintf (Gbl.F.Out,", %s</li>",Gbl.Usrs.Other.UsrDat.FirstName);
	   }
	}

      fprintf (Gbl.F.Out,"</ul>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End column with list of all members with this role *****/
   Tbl_TD_End ();
  }

/*****************************************************************************/
/******************** Get number of users in a project ***********************/
/*****************************************************************************/

static unsigned Prj_GetUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject,
                                  MYSQL_RES **mysql_res)
  {
   /***** Get users in project from database *****/
   return (unsigned) DB_QuerySELECT (mysql_res,"can not get users in project",
				     "SELECT prj_usr.UsrCod,"
				     "usr_data.Surname1 AS S1,"
				     "usr_data.Surname2 AS S2,"
				     "usr_data.FirstName AS FN"
				     " FROM prj_usr,usr_data"
				     " WHERE prj_usr.PrjCod=%ld AND RoleInProject=%u"
				     " AND prj_usr.UsrCod=usr_data.UsrCod"
				     " ORDER BY S1,S2,FN",
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
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get my roles in project",
		                        "SELECT RoleInProject FROM prj_usr"
		                        " WHERE PrjCod=%ld AND UsrCod=%ld",
		                        PrjCod,Gbl.Usrs.Me.UsrDat.UsrCod);
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
   Prj_ReqAddUsrs (Prj_ROLE_STD);
  }

void Prj_ReqAddTuts (void)
  {
   Prj_ReqAddUsrs (Prj_ROLE_TUT);
  }

void Prj_ReqAddEvls (void)
  {
   Prj_ReqAddUsrs (Prj_ROLE_EVL);
  }

static void Prj_ReqAddUsrs (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Hlp_ASSESSMENT_Projects_add_user;
   extern const char *Txt_Add_USERS;
   extern const char *Txt_PROJECT_ROLES_PLURAL_abc[Prj_NUM_ROLES_IN_PROJECT];
   static Act_Action_t ActionAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActAddStdPrj,	// Prj_ROLE_STD, Student
      ActAddTutPrj,	// Prj_ROLE_TUT, Tutor
      ActAddEvlPrj,	// Prj_ROLE_EVL, Evaluator
     };
   char TxtButton[Lay_MAX_BYTES_TITLE + 1];

   /***** Get project code *****/
   if ((Gbl.Prjs.PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Put form to select users *****/
   snprintf (TxtButton,sizeof (TxtButton),
	     Txt_Add_USERS,
	     Txt_PROJECT_ROLES_PLURAL_abc[RoleInProject]);
   Usr_PutFormToSelectUsrsToGoToAct (ActionAddUsr[RoleInProject],Prj_PutCurrentParams,
				     TxtButton,
                                     Hlp_ASSESSMENT_Projects_add_user,
                                     TxtButton);

   /***** Put a form to create/edit project *****/
   Prj_RequestCreatOrEditPrj (Gbl.Prjs.PrjCod);
  }

/*****************************************************************************/
/******* Get and check list of selected users, and show users' works  ********/
/*****************************************************************************/

void Prj_GetSelectedUsrsAndAddStds (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (Prj_AddStds,		// when user(s) selected
                                  Prj_ReqAddStds);	// when no user selected
  }

void Prj_GetSelectedUsrsAndAddTuts (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (Prj_AddTuts,		// when user(s) selected
                                  Prj_ReqAddTuts);	// when no user selected
  }

void Prj_GetSelectedUsrsAndAddEvls (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (Prj_AddEvls,		// when user(s) selected
                                  Prj_ReqAddEvls);	// when no user selected
  }

/*****************************************************************************/
/**************************** Add users to project ***************************/
/*****************************************************************************/

static void Prj_AddStds (void)
  {
   Prj_AddUsrsToProject (Prj_ROLE_STD);
  }

static void Prj_AddTuts (void)
  {
   Prj_AddUsrsToProject (Prj_ROLE_TUT);
  }

static void Prj_AddEvls (void)
  {
   Prj_AddUsrsToProject (Prj_ROLE_EVL);
  }

static void Prj_AddUsrsToProject (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   long PrjCod;
   const char *Ptr;
   bool ItsMe;

   /***** Get project code *****/
   if ((PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Add the selected users to project *****/
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      /* Get next user */
      Par_GetNextStrUntilSeparParamMult (&Ptr,Gbl.Usrs.Other.UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&Gbl.Usrs.Other.UsrDat);

      /* Get user's data */
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))	// Get of the database the data of the user
        {
	 /* Add user to project */
	 DB_QueryREPLACE ("can not add user to project",
			  "REPLACE INTO prj_usr"
			  " (PrjCod,RoleInProject,UsrCod)"
			  " VALUES"
			  " (%ld,%u,%ld)",
			  PrjCod,(unsigned) RoleInProject,
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

   /***** Put form to edit project again *****/
   Prj_RequestCreatOrEditPrj (PrjCod);
  }

/*****************************************************************************/
/************ Request confirmation to remove user from project ***************/
/*****************************************************************************/

void Prj_ReqRemStd (void)
  {
   Prj_ReqRemUsrFromPrj (Prj_ROLE_STD);
  }

void Prj_ReqRemTut (void)
  {
   Prj_ReqRemUsrFromPrj (Prj_ROLE_TUT);
  }

void Prj_ReqRemEvl (void)
  {
   Prj_ReqRemUsrFromPrj (Prj_ROLE_EVL);
  }

static void Prj_ReqRemUsrFromPrj (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT][Usr_NUM_SEXS];
   extern const char *Txt_Remove_USER_from_this_project;
   static Act_Action_t ActionRemUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActRemStdPrj,	// Prj_ROLE_STD, Student
      ActRemTutPrj,	// Prj_ROLE_TUT, Tutor
      ActRemEvlPrj,	// Prj_ROLE_EVL, Evaluator
     };
   struct Project Prj;
   bool ItsMe;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (&Prj))
	{
	 ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);

	 /***** Show question and button to remove user as a role from project *****/
	 /* Start alert */
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,ItsMe ? Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y :
			                               Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y,
				  Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject][Gbl.Usrs.Other.UsrDat.Sex],
				  Prj.Title);

	 /* Show user's record */
	 Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	 /* Show form to request confirmation */
	 Frm_StartForm (ActionRemUsr[RoleInProject]);
	 Gbl.Prjs.PrjCod = Prj.PrjCod;
	 Prj_PutCurrentParams ();
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Remove_USER_from_this_project,
		   Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject][Gbl.Usrs.Other.UsrDat.Sex]);
	 Btn_PutRemoveButton (Gbl.Title);
	 Frm_EndForm ();

	 /* End alert */
	 Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
	}
      else
         Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Put form to edit project again *****/
   Prj_RequestCreatOrEditPrj (Prj.PrjCod);
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
   struct Project Prj;
   bool ItsMe;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (&Prj))
	{
	 /***** Remove user from the table of project-users *****/
	 DB_QueryDELETE ("can not remove a user from a project",
			 "DELETE FROM prj_usr"
			 " WHERE PrjCod=%ld AND RoleInProject=%u"
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
   Prj_RequestCreatOrEditPrj (Prj.PrjCod);
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of projects **********/
/*****************************************************************************/

static void Prj_GetParamPrjOrder (void)
  {
   Gbl.Prjs.SelectedOrder = (Prj_Order_t)
	                    Par_GetParToUnsignedLong ("Order",
                                                      0,
                                                      Prj_NUM_ORDERS - 1,
                                                      (unsigned long) Prj_ORDER_DEFAULT);
  }

/*****************************************************************************/
/***** Put a hidden parameter with the type of order in list of projects *****/
/*****************************************************************************/

void Prj_PutHiddenParamPrjOrder (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Prjs.SelectedOrder);
  }

/*****************************************************************************/
/****************** Put a link (form) to edit one project ********************/
/*****************************************************************************/

static void Prj_PutFormsToRemEditOnePrj (const struct Project *Prj,
                                         const char *Anchor,
                                         bool ICanViewProjectFiles)
  {
   Gbl.Prjs.PrjCod = Prj->PrjCod;	// Used as parameter in contextual links

   if (Prj_CheckIfICanEditProject (Prj))
     {
      /***** Put form to remove project *****/
      Ico_PutContextualIconToRemove (ActReqRemPrj,Prj_PutCurrentParams);

      /***** Put form to hide/show project *****/
      switch (Prj->Hidden)
        {
	 case Prj_HIDDEN:
	    Ico_PutContextualIconToUnhide (ActShoPrj,Anchor,Prj_PutCurrentParams);
	    break;
	 case Prj_VISIBL:
	    Ico_PutContextualIconToHide (ActHidPrj,Anchor,Prj_PutCurrentParams);
	    break;
        }

      /***** Put form to edit project *****/
      Ico_PutContextualIconToEdit (ActEdiOnePrj,Prj_PutCurrentParams);
     }

   /***** Put form to admin project documents *****/
   if (ICanViewProjectFiles)
      Ico_PutContextualIconToViewFiles (ActAdmDocPrj,Prj_PutCurrentParams);

   /***** Put form to print project *****/
   Ico_PutContextualIconToPrint (ActPrnOnePrj,Prj_PutCurrentParams);

   /***** Locked/unlocked project edition *****/
   if (Prj_CheckIfICanLockProjects ())
     {
      /* Put form to lock/unlock project edition */
      fprintf (Gbl.F.Out,"<div id=\"prj_lck_%ld\">",Prj->PrjCod);
      Prj_FormLockUnlock (Prj);
      fprintf (Gbl.F.Out,"</div>");
     }
   else
      /* Put icon toinform about locked/unlocked project edition */
      Prj_PutIconOffLockedUnlocked (Prj);
  }

/*****************************************************************************/
/******************** Can I view files of a given project? *******************/
/*****************************************************************************/

bool Prj_CheckIfICanViewProjectFiles (unsigned MyRolesInProject)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 return (MyRolesInProject != 0);	// Am I a member?
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************************ Can I edit a given project? ************************/
/*****************************************************************************/

static bool Prj_CheckIfICanEditProject (const struct Project *Prj)
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
/********************* Can I lock/unlock project edition? ********************/
/*****************************************************************************/

static bool Prj_CheckIfICanLockProjects (void)
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
/******************** Form to lock/unlock project edition ********************/
/*****************************************************************************/

static void Prj_FormLockUnlock (const struct Project *Prj)
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
      Lay_NotEnoughMemoryExit ();
   Frm_StartFormOnSubmit (ActUnk,OnSubmit);
   Ico_PutIconLink (Prj_LockIcons[Prj->Locked],Txt_LOCKED_UNLOCKED[Prj->Locked]);
   Frm_EndForm ();

   /* Free allocated memory for subquery */
   free ((void *) OnSubmit);
  }

/*****************************************************************************/
/********* Put icon to inform about locked/unlocked project edition **********/
/*****************************************************************************/

static void Prj_PutIconOffLockedUnlocked (const struct Project *Prj)
  {
   extern const char *Txt_LOCKED_UNLOCKED[Prj_NUM_LOCKED_UNLOCKED];

   /***** Icon to inform about locked/unlocked project edition *****/
   Ico_PutIconOff (Prj_LockIcons[Prj->Locked],Txt_LOCKED_UNLOCKED[Prj->Locked]);
  }

/*****************************************************************************/
/************************** List all the projects ****************************/
/*****************************************************************************/

void Prj_GetListProjects (void)
  {
   char *PreNonSubQuery;
   char *HidVisSubQuery;
   char *DptCodSubQuery;
   static const char *OrderBySubQuery[Prj_NUM_ORDERS] =
     {
      "projects.CreatTime DESC,"	// Prj_ORDER_START_TIME
      "projects.ModifTime DESC,"
      "projects.Title",

      "projects.ModifTime DESC,"	// Prj_ORDER_END_TIME
      "projects.CreatTime DESC,"
      "projects.Title",

      "projects.Title,"			// Prj_ORDER_TITLE
      "projects.CreatTime DESC,"
      "projects.ModifTime DESC",

      "departments.FullName,"		// Prj_ORDER_DEPARTMENT
      "projects.CreatTime DESC,"
      "projects.ModifTime DESC,"
      "projects.Title",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   unsigned NumPrj;

   if (Gbl.Prjs.LstIsRead)
      Prj_FreeListProjects ();

   /***** Get list of projects from database *****/
   if (Gbl.Prjs.Filter.PreNon == 0 ||	// All selectors are off
       Gbl.Prjs.Filter.HidVis == 0)	// All selectors are off
      Gbl.Prjs.Num = 0;		// Nothing to get from database
   else
     {
      /* Preassigned subquery */
      switch (Gbl.Prjs.Filter.PreNon)
	{
	 case (1 << Prj_PREASSIGNED):	// Preassigned projects
	    if (asprintf (&PreNonSubQuery," AND projects.Preassigned='Y'") < 0)
	       Lay_NotEnoughMemoryExit ();
	    break;
	 case (1 << Prj_NONPREASSIG):	// Non-preassigned projects
	    if (asprintf (&PreNonSubQuery," AND projects.Preassigned='N'") < 0)
	       Lay_NotEnoughMemoryExit ();
	    break;
	 default:			// All projects
	    if (asprintf (&PreNonSubQuery,"%s","") < 0)
	       Lay_NotEnoughMemoryExit ();
	    break;
	}

      /* Hidden subquery */
      switch (Gbl.Usrs.Me.Role.Logged)
	{
	 case Rol_STD:	// Students can view only visible projects
	    if (asprintf (&HidVisSubQuery," AND projects.Hidden='N'") < 0)
	       Lay_NotEnoughMemoryExit ();
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	 case Rol_SYS_ADM:
	    switch (Gbl.Prjs.Filter.HidVis)
	      {
	       case (1 << Prj_HIDDEN):	// Hidden projects
		  if (asprintf (&HidVisSubQuery," AND projects.Hidden='Y'") < 0)
	             Lay_NotEnoughMemoryExit ();
		  break;
	       case (1 << Prj_VISIBL):	// Visible projects
		  if (asprintf (&HidVisSubQuery," AND projects.Hidden='N'") < 0)
	             Lay_NotEnoughMemoryExit ();
		  break;
	       default:			// All projects
		  if (asprintf (&HidVisSubQuery,"%s","") < 0)
	             Lay_NotEnoughMemoryExit ();
		  break;
	      }
	    break;
	 default:
	    Rol_WrongRoleExit ();
	    break;
	}

      /* Department subquery */
      if (Gbl.Prjs.Filter.DptCod >= 0)
        {
	 if (asprintf (&DptCodSubQuery," AND projects.DptCod=%ld",
	               Gbl.Prjs.Filter.DptCod) < 0)
	    Lay_NotEnoughMemoryExit ();
        }
      else	// Any department
	{
	 if (asprintf (&DptCodSubQuery,"%s","") < 0)
	    Lay_NotEnoughMemoryExit ();
	}

      /* Query */
      if (Gbl.Prjs.Filter.My_All == Prj_MY__PROJECTS)
	 switch (Gbl.Prjs.SelectedOrder)
	   {
	    case Prj_ORDER_START_TIME:
	    case Prj_ORDER_END_TIME:
	    case Prj_ORDER_TITLE:
	       NumRows = DB_QuerySELECT (&mysql_res,"can not get projects",
					 "SELECT projects.PrjCod"
					 " FROM projects,prj_usr"
					 " WHERE projects.CrsCod=%ld"
					 "%s%s%s"
					 " AND projects.PrjCod=prj_usr.PrjCod"
					 " AND prj_usr.UsrCod=%ld"
					 " ORDER BY %s",
					 Gbl.Hierarchy.Crs.CrsCod,
					 PreNonSubQuery,HidVisSubQuery,DptCodSubQuery,
					 Gbl.Usrs.Me.UsrDat.UsrCod,
					 OrderBySubQuery[Gbl.Prjs.SelectedOrder]);
	       break;
	    case Prj_ORDER_DEPARTMENT:
	       NumRows = DB_QuerySELECT (&mysql_res,"can not get projects",
					 "SELECT projects.PrjCod"
					 " FROM prj_usr,projects LEFT JOIN departments"
					 " ON projects.DptCod=departments.DptCod"
					 " WHERE projects.CrsCod=%ld"
					 "%s%s%s"
					 " AND projects.PrjCod=prj_usr.PrjCod"
					 " AND prj_usr.UsrCod=%ld"
					 " ORDER BY %s",
					 Gbl.Hierarchy.Crs.CrsCod,
					 PreNonSubQuery,HidVisSubQuery,DptCodSubQuery,
					 Gbl.Usrs.Me.UsrDat.UsrCod,
					 OrderBySubQuery[Gbl.Prjs.SelectedOrder]);
	       break;
	   }
      else	// Gbl.Prjs.My_All == Prj_ALL_PROJECTS
	 switch (Gbl.Prjs.SelectedOrder)
	   {
	    case Prj_ORDER_START_TIME:
	    case Prj_ORDER_END_TIME:
	    case Prj_ORDER_TITLE:
	       NumRows = DB_QuerySELECT (&mysql_res,"can not get projects",
					 "SELECT projects.PrjCod"
					 " FROM projects"
					 " WHERE projects.CrsCod=%ld"
					 "%s%s%s"
					 " ORDER BY %s",
					 Gbl.Hierarchy.Crs.CrsCod,
					 PreNonSubQuery,HidVisSubQuery,DptCodSubQuery,
					 OrderBySubQuery[Gbl.Prjs.SelectedOrder]);
	       break;
	    case Prj_ORDER_DEPARTMENT:
	       NumRows = DB_QuerySELECT (&mysql_res,"can not get projects",
					 "SELECT projects.PrjCod"
					 " FROM projects LEFT JOIN departments"
					 " ON projects.DptCod=departments.DptCod"
					 " WHERE projects.CrsCod=%ld"
					 "%s%s%s"
					 " ORDER BY %s",
					 Gbl.Hierarchy.Crs.CrsCod,
					 PreNonSubQuery,HidVisSubQuery,DptCodSubQuery,
					 OrderBySubQuery[Gbl.Prjs.SelectedOrder]);
	       break;
	   }

      /* Free allocated memory for subqueries */
      free ((void *) PreNonSubQuery);
      free ((void *) HidVisSubQuery);
      free ((void *) DptCodSubQuery);

      if (NumRows) // Projects found...
	{
	 Gbl.Prjs.Num = (unsigned) NumRows;

	 /***** Create list of projects *****/
	 if ((Gbl.Prjs.LstPrjCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
	    Lay_NotEnoughMemoryExit ();

	 /***** Get the projects codes *****/
	 for (NumPrj = 0;
	      NumPrj < Gbl.Prjs.Num;
	      NumPrj++)
	   {
	    /* Get next project code */
	    row = mysql_fetch_row (mysql_res);
	    if ((Gbl.Prjs.LstPrjCods[NumPrj] = Str_ConvertStrCodToLongCod (row[0])) < 0)
	       Lay_ShowErrorAndExit ("Error: wrong project code.");
	   }
	}
      else
	 Gbl.Prjs.Num = 0;

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   Gbl.Prjs.LstIsRead = true;
  }

/*****************************************************************************/
/****************** Check if a project exists in a course ********************/
/*****************************************************************************/

long Prj_GetCourseOfProject (long PrjCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long CrsCod = -1L;

   if (PrjCod > 0)
     {
      /***** Get course code from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get project course",
			  "SELECT CrsCod FROM projects WHERE PrjCod=%ld",
			  PrjCod)) // Project found...
	{
	 /* Get row */
	 row = mysql_fetch_row (mysql_res);

	 /* Get code of the course (row[0]) */
	 CrsCod = Str_ConvertStrCodToLongCod (row[0]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return CrsCod;
  }

/*****************************************************************************/
/********************* Get project data using its code ***********************/
/*****************************************************************************/

void Prj_GetDataOfProjectByCod (struct Project *Prj)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long LongNum;
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
				 "Preassigned,"			// row[ 5]
				 "NumStds,"			// row[ 6]
				 "Proposal,"			// row[ 7]
				 "UNIX_TIMESTAMP(CreatTime),"	// row[ 8]
				 "UNIX_TIMESTAMP(ModifTime),"	// row[ 9]
				 "Title,"			// row[10]
				 "Description,"			// row[11]
				 "Knowledge,"			// row[12]
				 "Materials,"			// row[13]
				 "URL"				// row[14]
			  " FROM projects"
			  " WHERE PrjCod=%ld AND CrsCod=%ld",
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

	 /* Get if project is preassigned or not (row[5]) */
	 Prj->Preassigned = (row[5][0] == 'Y') ? Prj_PREASSIGNED :
						 Prj_NONPREASSIG;

	 /* Get if project is preassigned or not (row[6]) */
	 LongNum = Str_ConvertStrCodToLongCod (row[6]);
	 if (LongNum >= 0)
	    Prj->NumStds = (unsigned) LongNum;
	 else
	    Prj->NumStds = 1;

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

	 /* Get the title of the project (row[10]) */
	 Str_Copy (Prj->Title,row[10],
		   Prj_MAX_BYTES_PROJECT_TITLE);

	 /* Get the description of the project (row[11]) */
	 Str_Copy (Prj->Description,row[11],
		   Cns_MAX_BYTES_TEXT);

	 /* Get the required knowledge for the project (row[12]) */
	 Str_Copy (Prj->Knowledge,row[12],
		   Cns_MAX_BYTES_TEXT);

	 /* Get the required materials for the project (row[13]) */
	 Str_Copy (Prj->Materials,row[13],
		   Cns_MAX_BYTES_TEXT);

	 /* Get the URL of the project (row[14]) */
	 Str_Copy (Prj->URL,row[14],
		   Cns_MAX_BYTES_WWW);
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

static void Prj_ResetProject (struct Project *Prj)
  {
   if (Prj->PrjCod <= 0)	// If > 0 ==> keep value
      Prj->PrjCod = -1L;
   Prj->CrsCod = -1L;
   Prj->Locked	    = Prj_UNLOCKED;
   Prj->Hidden      = Prj_NEW_PRJ_HIDDEN_VISIBL_DEFAULT;
   Prj->Preassigned = Prj_NEW_PRJ_PREASSIGNED_NONPREASSIG_DEFAULT;
   Prj->NumStds     = 1;
   Prj->Proposal    = Prj_PROPOSAL_DEFAULT;
   Prj->CreatTime =
   Prj->ModifTime = (time_t) 0;
   Prj->Title[0] = '\0';
   Prj->DptCod = -1L;	// Unknown department
   Prj->Description[0] = '\0';
   Prj->Knowledge[0]   = '\0';
   Prj->Materials[0]   = '\0';
   Prj->URL[0]         = '\0';
  }

/*****************************************************************************/
/*************************** Free list of projects ***************************/
/*****************************************************************************/

void Prj_FreeListProjects (void)
  {
   if (Gbl.Prjs.LstIsRead && Gbl.Prjs.LstPrjCods)
     {
      /***** Free memory used by the list of projects *****/
      free ((void *) Gbl.Prjs.LstPrjCods);
      Gbl.Prjs.LstPrjCods = NULL;
      Gbl.Prjs.Num = 0;
      Gbl.Prjs.LstIsRead = false;
     }
  }

/*****************************************************************************/
/******************* Write parameter with code of project ********************/
/*****************************************************************************/

void Prj_PutParamPrjCod (long PrjCod)
  {
   Par_PutHiddenParamLong ("PrjCod",PrjCod);
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
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
     {
      /***** Show question and button to remove the project *****/
      Gbl.Prjs.PrjCod = Prj.PrjCod;
      Ale_ShowAlertAndButton (ActRemPrj,NULL,NULL,Prj_PutCurrentParams,
			      Btn_REMOVE_BUTTON,Txt_Remove_project,
			      Ale_QUESTION,Txt_Do_you_really_want_to_remove_the_project_X,
	                      Prj.Title);
     }
   else
      Act_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

/*****************************************************************************/
/***************************** Remove a project ******************************/
/*****************************************************************************/

void Prj_RemoveProject (void)
  {
   extern const char *Txt_Project_X_removed;
   struct Project Prj;
   char PathRelPrj[PATH_MAX + 1];

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);	// Inside this function, the course is checked to be the current one

   if (Prj_CheckIfICanEditProject (&Prj))
     {
      /***** Remove users in project *****/
      DB_QueryDELETE ("can not remove project",
		      "DELETE FROM prj_usr USING projects,prj_usr"
		      " WHERE projects.PrjCod=%ld AND projects.CrsCod=%ld"
		      " AND projects.PrjCod=prj_usr.PrjCod",
	              Prj.PrjCod,Gbl.Hierarchy.Crs.CrsCod);

      /***** Flush cache *****/
      Prj_FlushCacheMyRolesInProject ();

      /***** Remove project *****/
      DB_QueryDELETE ("can not remove project",
		      "DELETE FROM projects"
		      " WHERE PrjCod=%ld AND CrsCod=%ld",
	              Prj.PrjCod,Gbl.Hierarchy.Crs.CrsCod);

      /***** Remove information related to files in project *****/
      Brw_RemovePrjFilesFromDB (Prj.PrjCod);

      /***** Remove directory of the project *****/
      snprintf (PathRelPrj,sizeof (PathRelPrj),
	        "%s/%ld/%s/%02u/%ld",
	        Cfg_PATH_CRS_PRIVATE,Prj.CrsCod,Cfg_FOLDER_PRJ,
	        (unsigned) (Prj.PrjCod % 100),Prj.PrjCod);
      Fil_RemoveTree (PathRelPrj);

      /***** Write message to show the change made *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Project_X_removed,
	             Prj.Title);
     }
   else
      Act_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

/*****************************************************************************/
/****************************** Hide a project *******************************/
/*****************************************************************************/

void Prj_HideProject (void)
  {
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
      /***** Hide project *****/
      DB_QueryUPDATE ("can not hide project",
		      "UPDATE projects SET Hidden='Y'"
		      " WHERE PrjCod=%ld AND CrsCod=%ld",
	              Prj.PrjCod,Gbl.Hierarchy.Crs.CrsCod);
   else
      Act_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

/*****************************************************************************/
/****************************** Show a project *******************************/
/*****************************************************************************/

void Prj_ShowProject (void)
  {
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (&Prj))
      /***** Show project *****/
      DB_QueryUPDATE ("can not show project",
		      "UPDATE projects SET Hidden='N'"
		      " WHERE PrjCod=%ld AND CrsCod=%ld",
	              Prj.PrjCod,Gbl.Hierarchy.Crs.CrsCod);
   else
      Act_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

/*****************************************************************************/
/************************** Lock edition of a project ************************/
/*****************************************************************************/

void Prj_LockProjectEdition (void)
  {
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

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
      Act_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

static void Prj_LockProjectEditionInDB (long PrjCod)
  {
   DB_QueryUPDATE ("can not lock project edition",
		   "UPDATE projects SET Locked='Y'"
		   " WHERE PrjCod=%ld AND CrsCod=%ld",
		   PrjCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************************* Unlock edition of a project ***********************/
/*****************************************************************************/

void Prj_UnlockProjectEdition (void)
  {
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

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
      Act_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

static void Prj_UnlockProjectEditionInDB (long PrjCod)
  {
   DB_QueryUPDATE ("can not lock project edition",
		   "UPDATE projects SET Locked='N'"
		   " WHERE PrjCod=%ld AND CrsCod=%ld",
		   PrjCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/************ Request locking/unlocking edition of all projects **************/
/*****************************************************************************/

void Prj_ReqLockAllProjectsEdition (void)
  {
   extern const char *Txt_Lock_editing;
   extern const char *Txt_Do_you_want_to_lock_the_editing_of_the_X_selected_projects;
   extern const char *Txt_No_projects;

   /***** Get parameters *****/
   Prj_GetParams ();

   /***** Show question and button to lock all selected projects *****/
   if (Prj_CheckIfICanLockProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects ();

      /* Show question and button */
      if (Gbl.Prjs.Num)
	 Ale_ShowAlertAndButton (ActLckAllPrj,NULL,NULL,Prj_PutCurrentParams,
				 Btn_REMOVE_BUTTON,Txt_Lock_editing,
				 Ale_QUESTION,Txt_Do_you_want_to_lock_the_editing_of_the_X_selected_projects,
				 Gbl.Prjs.Num);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects ();
     }
   else
      Act_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

void Prj_ReqUnlockAllProjectsEdition (void)
  {
   extern const char *Txt_Unlock_editing;
   extern const char *Txt_Do_you_want_to_unlock_the_editing_of_the_X_selected_projects;
   extern const char *Txt_No_projects;

   /***** Get parameters *****/
   Prj_GetParams ();

   /***** Show question and button to unlock all selected projects *****/
   if (Prj_CheckIfICanLockProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects ();

      /* Show question and button */
      if (Gbl.Prjs.Num)
	 Ale_ShowAlertAndButton (ActUnlAllPrj,NULL,NULL,Prj_PutCurrentParams,
				 Btn_CREATE_BUTTON,Txt_Unlock_editing,
				 Ale_QUESTION,Txt_Do_you_want_to_unlock_the_editing_of_the_X_selected_projects,
				 Gbl.Prjs.Num);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects ();
     }
   else
      Act_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

/*****************************************************************************/
/******************* Lock/unlock edition of all projects *********************/
/*****************************************************************************/

void Prj_LockAllProjectsEdition (void)
  {
   extern const char *Txt_No_projects;
   unsigned NumPrj;

   /***** Get parameters *****/
   Prj_GetParams ();

   /***** Lock all selected projects *****/
   if (Prj_CheckIfICanLockProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects ();

      /* Lock projects */
      if (Gbl.Prjs.Num)
	 for (NumPrj = 0;
	      NumPrj < Gbl.Prjs.Num;
	      NumPrj++)
            Prj_LockProjectEditionInDB (Gbl.Prjs.LstPrjCods[NumPrj]);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects ();
     }
   else
      Act_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

void Prj_UnlockAllProjectsEdition (void)
  {
   extern const char *Txt_No_projects;
   unsigned NumPrj;

   /***** Get parameters *****/
   Prj_GetParams ();

   /***** Unlock all selected projects *****/
   if (Prj_CheckIfICanLockProjects ())
     {
      /* Get list of projects */
      Prj_GetListProjects ();

      /* Unlock projects */
      if (Gbl.Prjs.Num)
	 for (NumPrj = 0;
	      NumPrj < Gbl.Prjs.Num;
	      NumPrj++)
            Prj_UnlockProjectEditionInDB (Gbl.Prjs.LstPrjCods[NumPrj]);
      else	// No projects found
	 Ale_ShowAlert (Ale_INFO,Txt_No_projects);

      /* Free list of projects */
      Prj_FreeListProjects ();
     }
   else
      Act_NoPermissionExit ();

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

/*****************************************************************************/
/********************* Put a form to create/edit project *********************/
/*****************************************************************************/

void Prj_RequestCreatePrj (void)
  {
   /***** Form to create project *****/
   Prj_RequestCreatOrEditPrj (-1L);	// It's a new, non existing, project
  }

void Prj_RequestEditPrj (void)
  {
   long PrjCod;

   /***** Get project code *****/
   if ((PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Form to edit project *****/
   Prj_RequestCreatOrEditPrj (PrjCod);
  }

static void Prj_RequestCreatOrEditPrj (long PrjCod)
  {
   struct Project Prj;
   bool ItsANewProject;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   ItsANewProject = ((Prj.PrjCod = PrjCod) < 0);

   /***** Get from the database the data of the project *****/
   if (ItsANewProject)
     {
      /* Initialize to empty project */
      Prj_ResetProject (&Prj);
      Prj.CreatTime = Gbl.StartExecutionTimeUTC;
      Prj.ModifTime = Gbl.StartExecutionTimeUTC;
      Prj.DptCod = Gbl.Usrs.Me.UsrDat.Tch.DptCod;	// Default: my department
     }
   else
      /* Get data of the project from database */
      Prj_GetDataOfProjectByCod (&Prj);

   /***** Put form to edit project *****/
   Prj_PutFormProject (&Prj,ItsANewProject);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_ShowProjectsInCurrentPage ();
  }

static void Prj_PutFormProject (struct Project *Prj,bool ItsANewProject)
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
   extern const char *Txt_Preassigned_QUESTION;
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
   unsigned NumRoleToShow;

   /***** Start project box *****/
   if (ItsANewProject)
     {
      Gbl.Prjs.PrjCod = -1L;
      Box_StartBox (NULL,Txt_New_project,NULL,
                    Hlp_ASSESSMENT_Projects_new_project,Box_NOT_CLOSABLE);
     }
   else
     {
      Gbl.Prjs.PrjCod = Prj->PrjCod;
      Box_StartBox (NULL,
                    Prj->Title[0] ? Prj->Title :
                	            Txt_Edit_project,
                    NULL,
                    Hlp_ASSESSMENT_Projects_edit_project,Box_NOT_CLOSABLE);
     }

   /***** 1. Project members *****/
   if (!ItsANewProject)	// Existing project
     {
      Box_StartBoxTable (NULL,Txt_Members,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      for (NumRoleToShow = 0;
	   NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	   NumRoleToShow++)
	 Prj_ShowOneProjectMembersWithARole (Prj,Prj_EDIT_ONE_PROJECT,
	                                     Prj_RolesToShow[NumRoleToShow]);
      Box_EndBoxTable ();
     }

   /***** 2. Project data *****/
   /* Start data form */
   Frm_StartForm (ItsANewProject ? ActNewPrj :
	                           ActChgPrj);
   Prj_PutCurrentParams ();

   /* Start box and table */
   Box_StartBoxTable (NULL,Txt_Data,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /* Project title */
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<label for=\"Title\" class=\"%s\">%s:</label>",
            The_ClassFormInBox[Gbl.Prefs.Theme],Txt_Title);
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" id=\"Title\" name=\"Title\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />",
            Prj_MAX_CHARS_PROJECT_TITLE,Prj->Title);
   Tbl_TD_End ();

   Tbl_TR_End ();

   /* Department */
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<label for=\"%s\" class=\"%s\">%s:</label>",
            Dpt_PARAM_DPT_COD_NAME,
            The_ClassFormInBox[Gbl.Prefs.Theme],Txt_Department);
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"LEFT_MIDDLE\"");
   Dpt_WriteSelectorDepartment (Gbl.Hierarchy.Ins.InsCod,	// Departments in current institution
                                Prj->DptCod,			// Selected department
                                "PRJ_INPUT",			// Selector class
                                0,				// First option
                                Txt_Another_department,		// Text when no department selected
                                false);				// Don't submit on change
   Tbl_TD_End ();

   Tbl_TR_End ();

   /* Preassigned? */
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"%s RIGHT_MIDDLE\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",Txt_Preassigned_QUESTION);
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<select name=\"Preassigned\">");

   fprintf (Gbl.F.Out,"<option value=\"Y\"");
   if (Prj->Preassigned == Prj_PREASSIGNED)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_Yes);

   fprintf (Gbl.F.Out,"<option value=\"N\"");
   if (Prj->Preassigned == Prj_NONPREASSIG)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_No);

   fprintf (Gbl.F.Out,"</select>");
   Tbl_TD_End ();

   Tbl_TR_End ();

   /* Number of students */
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"%s RIGHT_MIDDLE\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",Txt_Number_of_students);
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"number\" name=\"NumStds\""
                      " min=\"0\" value=\"%u\" />",
            Prj->NumStds);
   Tbl_TD_End ();

   Tbl_TR_End ();

   /* Proposal */
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"%s RIGHT_MIDDLE\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   fprintf (Gbl.F.Out,"%s:",Txt_Proposal);
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<select name=\"Proposal\">");
   for (Proposal  = (Prj_Proposal_t) 0;
	Proposal <= (Prj_Proposal_t) (Prj_NUM_PROPOSAL_TYPES - 1);
	Proposal++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",
               (unsigned) Proposal);
      if (Prj->Proposal == Proposal)
	 fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_PROJECT_STATUS[Proposal]);
     }
   fprintf (Gbl.F.Out,"</select>");
   Tbl_TD_End ();

   Tbl_TR_End ();

   /* Description of the project */
   Prj_EditOneProjectTxtArea ("Description",Txt_Description,
                              Prj->Description,12);

   /* Required knowledge to carry out the project */
   Prj_EditOneProjectTxtArea ("Knowledge",Txt_Required_knowledge,
                              Prj->Knowledge,4);

   /* Required materials to carry out the project */
   Prj_EditOneProjectTxtArea ("Materials",Txt_Required_materials,
                              Prj->Materials,4);

   /* URL for additional info */
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<label for=\"WWW\" class=\"%s\">%s:</label>",
	    The_ClassFormInBox[Gbl.Prefs.Theme],
	    Txt_URL);
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"DAT LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"url\" id=\"URL\" name=\"URL\""
		      " size=\"45\" maxlength=\"%u\" value=\"%s\" />",
	    Cns_MAX_CHARS_WWW,Prj->URL);
   Tbl_TD_End ();

   Tbl_TR_End ();

   /* End table, send button and end box */
   if (ItsANewProject)
      Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_project);
   else
      Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_Save_changes);

   /* End data form */
   Frm_EndForm ();

   /***** End project box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/************************ Show text row about a project **********************/
/*****************************************************************************/

static void Prj_EditOneProjectTxtArea (const char *Id,
                                       const char *Label,char *TxtField,
                                       unsigned NumRows)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];

   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"RIGHT_TOP\"");
   fprintf (Gbl.F.Out,"<label for=\"%s\" class=\"%s\">%s:</label>",
            Id,The_ClassFormInBox[Gbl.Prefs.Theme],Label);
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"LEFT_TOP\"");
   fprintf (Gbl.F.Out,"<textarea id=\"%s\" name=\"%s\" cols=\"60\" rows=\"%u\">"
                      "%s"
                      "</textarea>",
            Id,Id,
            NumRows,
            TxtField);
   Tbl_TD_End ();

   Tbl_TR_End ();
  }

/*****************************************************************************/
/*** Allocate memory for those parameters of a project with a lot of text ****/
/*****************************************************************************/

void Prj_AllocMemProject (struct Project *Prj)
  {
   if ((Prj->Description = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Prj->Knowledge   = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();

   if ((Prj->Materials   = (char *) malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/****** Free memory of those parameters of a project with a lot of text ******/
/*****************************************************************************/

void Prj_FreeMemProject (struct Project *Prj)
  {
   if (Prj->Description)
     {
      free ((void *) Prj->Description);
      Prj->Description = NULL;
     }
   if (Prj->Knowledge)
     {
      free ((void *) Prj->Knowledge);
      Prj->Knowledge = NULL;
     }
   if (Prj->Materials)
     {
      free ((void *) Prj->Materials);
      Prj->Materials = NULL;
     }
  }

/*****************************************************************************/
/******************** Receive form to create a new project *******************/
/*****************************************************************************/

void Prj_RecFormProject (void)
  {
   extern const char *Txt_You_must_specify_the_title_of_the_project;
   extern const char *Txt_Created_new_project_X;
   extern const char *Txt_The_project_has_been_modified;
   struct Project Prj;	// Project data received from form
   bool ItsANewProject;
   bool ICanEditProject;
   bool NewProjectIsCorrect = true;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParams ();
   ItsANewProject = ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0);

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

      /* Get whether the project is preassigned */
      Prj.Preassigned = (Par_GetParToBool ("Preassigned")) ? Prj_PREASSIGNED :
							     Prj_NONPREASSIG;

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
         Prj_PutFormProject (&Prj,ItsANewProject);

      /***** Show again form to edit project *****/
      Prj_RequestCreatOrEditPrj (Prj.PrjCod);
     }
   else
      Act_NoPermissionExit ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/************************** Create a new project *****************************/
/*****************************************************************************/

static void Prj_CreateProject (struct Project *Prj)
  {
   /***** Set dates to now *****/
   Prj->CreatTime =
   Prj->ModifTime = Gbl.StartExecutionTimeUTC;

   /***** Create a new project *****/
   Prj->PrjCod =
   DB_QueryINSERTandReturnCode ("can not create new project",
				"INSERT INTO projects"
				" (CrsCod,DptCod,Hidden,Preassigned,NumStds,Proposal,"
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
				Prj->Preassigned == Prj_PREASSIGNED ? 'Y' :
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
		   "INSERT INTO prj_usr"
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

static void Prj_UpdateProject (struct Project *Prj)
  {
   /***** Adjust date of last edition to now *****/
   Prj->ModifTime = Gbl.StartExecutionTimeUTC;

   /***** Update the data of the project *****/
   DB_QueryUPDATE ("can not update project",
		   "UPDATE projects"
		   " SET DptCod=%ld,"
		        "Hidden='%c',"
		        "Preassigned='%c',"
		        "NumStds=%u,"
		        "Proposal='%s',"
		        "ModifTime=FROM_UNIXTIME(%ld),"
		        "Title='%s',"
		        "Description='%s',"
		        "Knowledge='%s',"
		        "Materials='%s',"
		        "URL='%s'"
		   " WHERE PrjCod=%ld AND CrsCod=%ld",
	           Prj->DptCod,
	           Prj->Hidden == Prj_HIDDEN ? 'Y' :
					       'N',
	           Prj->Preassigned == Prj_PREASSIGNED ? 'Y' :
						         'N',
	           Prj->NumStds,
	           Prj_Proposal_DB[Prj->Proposal],
	           Prj->ModifTime,
	           Prj->Title,
	           Prj->Description,
	           Prj->Knowledge,
	           Prj->Materials,
	           Prj->URL,
	           Prj->PrjCod,Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/******************** Remove all the projects in a course ********************/
/*****************************************************************************/

void Prj_RemoveCrsProjects (long CrsCod)
  {
   /***** Remove users in projects of the course *****/
   DB_QueryDELETE ("can not remove all the projects of a course",
		   "DELETE FROM prj_usr USING projects,prj_usr"
		   " WHERE projects.CrsCod=%ld"
		   " AND projects.PrjCod=prj_usr.PrjCod",
                   CrsCod);

   /***** Flush cache *****/
   Prj_FlushCacheMyRolesInProject ();

   /***** Remove projects *****/
   DB_QueryDELETE ("can not remove all the projects of a course",
		   "DELETE FROM projects WHERE CrsCod=%ld",
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
		   "DELETE FROM prj_usr WHERE UsrCod=%ld",
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

unsigned Prj_GetNumCoursesWithProjects (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with projects from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with projects",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM projects"
			 " WHERE CrsCod>0");
         break;
       case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with projects",
			 "SELECT COUNT(DISTINCT projects.CrsCod)"
			 " FROM institutions,centres,degrees,courses,projects"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
       case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with projects",
			 "SELECT COUNT(DISTINCT projects.CrsCod)"
			 " FROM centres,degrees,courses,projects"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with projects",
			 "SELECT COUNT(DISTINCT projects.CrsCod)"
			 " FROM degrees,courses,projects"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with projects",
			 "SELECT COUNT(DISTINCT projects.CrsCod)"
			 " FROM courses,projects"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.Status=0"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of courses with projects",
			 "SELECT COUNT(DISTINCT CrsCod)"
			 " FROM projects"
			 " WHERE CrsCod=%ld",
			 Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of courses *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumCourses) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of courses with projects.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumCourses;
  }

/*****************************************************************************/
/************************** Get number of projects ***************************/
/*****************************************************************************/
// Returns the number of projects in this location

unsigned Prj_GetNumProjects (Hie_Level_t Scope)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumProjects;

   /***** Get number of projects from database *****/
   switch (Scope)
     {
      case Hie_SYS:
         DB_QuerySELECT (&mysql_res,"can not get number of projects",
			 "SELECT COUNT(*)"
                         " FROM projects"
                         " WHERE CrsCod>0");
         break;
      case Hie_CTY:
         DB_QuerySELECT (&mysql_res,"can not get number of projects",
			 "SELECT COUNT(*)"
			 " FROM institutions,centres,degrees,courses,projects"
			 " WHERE institutions.CtyCod=%ld"
			 " AND institutions.InsCod=centres.InsCod"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Cty.CtyCod);
         break;
      case Hie_INS:
         DB_QuerySELECT (&mysql_res,"can not get number of projects",
			 "SELECT COUNT(*)"
			 " FROM centres,degrees,courses,projects"
			 " WHERE centres.InsCod=%ld"
			 " AND centres.CtrCod=degrees.CtrCod"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Ins.InsCod);
         break;
      case Hie_CTR:
         DB_QuerySELECT (&mysql_res,"can not get number of projects",
			 "SELECT COUNT(*)"
			 " FROM degrees,courses,projects"
			 " WHERE degrees.CtrCod=%ld"
			 " AND degrees.DegCod=courses.DegCod"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Ctr.CtrCod);
         break;
      case Hie_DEG:
         DB_QuerySELECT (&mysql_res,"can not get number of projects",
			 "SELECT COUNT(*)"
			 " FROM courses,projects"
			 " WHERE courses.DegCod=%ld"
			 " AND courses.CrsCod=projects.CrsCod",
                         Gbl.Hierarchy.Deg.DegCod);
         break;
      case Hie_CRS:
         DB_QuerySELECT (&mysql_res,"can not get number of projects",
			 "SELECT COUNT(*)"
			 " FROM projects"
			 " WHERE CrsCod=%ld",
                         Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   /***** Get number of projects *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumProjects) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of projects.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumProjects;
  }
