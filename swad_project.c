// swad_project.c: projects (final degree projects, thesis)

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_box.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_project.h"
#include "swad_string.h"
#include "swad_table.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

typedef enum
  {
   Prj_LIST_PROJECTS,
   Prj_FILE_BROWSER_PROJECT,
   Prj_PRINT_ONE_PROJECT,
   Prj_EDIT_ONE_PROJECT,
  } Prj_ProjectView_t;

#define Prj_INTERVAL_DEFAULT ((time_t) 365 * (time_t) 24 * (time_t) 60 * (time_t) 60)	// 1 year

const Prj_RoleInProject_t Prj_RolesToShow[] =
  {
   Prj_ROLE_TUT,	// Tutor
   Prj_ROLE_STD,	// Student
   Prj_ROLE_EVA,	// Evaluator
  };
const unsigned Brw_NUM_ROLES_TO_SHOW = sizeof (Prj_RolesToShow) / sizeof (Prj_RolesToShow[0]);

const char *Prj_Proposal_DB[Prj_NUM_PROPOSAL_TYPES] =
  {
   "new",		// Prj_PROPOSAL_NEW
   "modified",		// Prj_PROPOSAL_MODIFIED
   "unmodified",	// Prj_PROPOSAL_UNMODIFIED
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prj_ShowProjectsInCurrentPage (void);

static void Prj_PutFormToSelectWhichProjecsToShow (void);
static void Prj_ShowFormToSelWhichPrjs (Act_Action_t Action,void (*FuncParams) ());
static void Prj_GetParamWhichPrjs (void);

static void Prj_ShowProjectsHead (bool PrintView);
static void Prj_ShowTableAllProjectsHead (void);
static bool Prj_CheckIfICanCreateProjects (void);
static void Prj_PutIconsListProjects (void);
static void Prj_PutIconToCreateNewPrj (void);
static void Prj_PutButtonToCreateNewPrj (void);
static void Prj_PutIconToShowAllData (void);
static void Prj_ShowOneProject (struct Project *Prj,Prj_ProjectView_t ProjectView);
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

static void Prj_ReqAnotherUsrID (Prj_RoleInProject_t RoleInProject);
static void Prj_AddUsrToProject (Prj_RoleInProject_t RoleInProject);
static void Prj_ReqRemUsrFromPrj (Prj_RoleInProject_t RoleInProject);
static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInProject);

static void Prj_GetParamPrjOrder (void);

static void Prj_PutFormsToRemEditOnePrj (long PrjCod,bool Hidden,
                                         bool ICanAdminDocsProject);

static bool Prj_CheckIfICanEditProject (long PrjCod);
static bool Prj_GetIfIAmMemberOfProject (long PrjCod);
static bool Prj_GetIfIAmTutorInProject (long PrjCod);

static void Prj_PutParams (void);
static void Prj_GetDataOfProject (struct Project *Prj,const char *Query);
static void Prj_ResetProject (struct Project *Prj);

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
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);

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
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();

   /***** Get list of projects *****/
   Prj_GetListProjects ();

   if (Gbl.Prjs.Num)
     {
      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Prj);

      /***** Table head *****/
      Tbl_StartTableWide (2);
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
      Tbl_EndTable ();

      /***** Free memory of the project *****/
      Prj_FreeMemProject (&Prj);
     }
   else	// No projects created
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);
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
   struct Project Prj;

   /***** Get list of projects *****/
   Prj_GetListProjects ();

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Prjs.Num;
   Pagination.CurrentPage = (int) Gbl.Prjs.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Prjs.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_PROJECTS,
                                     0,
                                     &Pagination);

   /***** Start box *****/
   Box_StartBox ("100%",Txt_Projects,Prj_PutIconsListProjects,
                 Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

   /***** Select whether show only my projects or all projects *****/
   Prj_PutFormToSelectWhichProjecsToShow ();

   if (Gbl.Prjs.Num)
     {
      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Prj);

      /***** Table head *****/
      Tbl_StartTableWideMargin (2);
      Prj_ShowProjectsHead (false);	// Not print view

      /***** Write all the projects *****/
      for (NumPrj = Pagination.FirstItemVisible;
	   NumPrj <= Pagination.LastItemVisible;
	   NumPrj++)
	{
         /* Get project data */
	 Prj.PrjCod = Gbl.Prjs.LstPrjCods[NumPrj - 1];
         Prj_GetDataOfProjectByCod (&Prj);

         /* Show project */
	 Prj_ShowOneProject (&Prj,Prj_LIST_PROJECTS);
	}

      /***** End table *****/
      Tbl_EndTable ();

      /***** Free memory of the project *****/
      Prj_FreeMemProject (&Prj);
     }
   else	// No projects created
      Ale_ShowAlert (Ale_INFO,Txt_No_projects);

   /***** Button to create a new project *****/
   if (Prj_CheckIfICanCreateProjects ())
      Prj_PutButtonToCreateNewPrj ();

   /***** Put link to register students *****/
   Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs ();

   /***** End box *****/
   Box_EndBox ();

   /***** Write again links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (Pag_PROJECTS,
                                     0,
                                     &Pagination);

   /***** Free list of projects *****/
   Prj_FreeListProjects ();
  }

/*****************************************************************************/
/***************** Put form to select which groups to show *******************/
/*****************************************************************************/

static void Prj_PutFormToSelectWhichProjecsToShow (void)
  {
   fprintf (Gbl.F.Out,"<div style=\"display:table; margin:0 auto;\">");
   Prj_ShowFormToSelWhichPrjs (ActSeePrj,Prj_PutParams);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*** Show form to choice whether to show only my projects or all projects ****/
/*****************************************************************************/

static void Prj_ShowFormToSelWhichPrjs (Act_Action_t Action,void (*FuncParams) ())
  {
   extern const char *Txt_PROJECT_WHICH_PROJECTS[2];
   Prj_WhichProjects_t WhichPrjs;

   fprintf (Gbl.F.Out,"<div class=\"PREF_CONTAINER\">");
   for (WhichPrjs = Prj_ONLY_MY_PROJECTS;
	WhichPrjs <= Prj_ALL_PROJECTS;
	WhichPrjs++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\">",
	       WhichPrjs == Gbl.CurrentCrs.Prjs.WhichPrjs ? "PREF_ON" :
							    "PREF_OFF");
      Act_FormStart (Action);
      Par_PutHiddenParamUnsigned ("WhichPrjs",(unsigned) WhichPrjs);
      if (FuncParams)	// Extra parameters depending on the action
	 FuncParams ();
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s\""
			 " alt=\"%s\" title=\"%s\" class=\"ICO25x25\""
			 " style=\"margin:0 auto;\" />",
	       Gbl.Prefs.IconsURL,
	       WhichPrjs == Prj_ONLY_MY_PROJECTS ? "myhierarchy64x64.png" :
		                                   "hierarchy64x64.png",
	       Txt_PROJECT_WHICH_PROJECTS[WhichPrjs],
	       Txt_PROJECT_WHICH_PROJECTS[WhichPrjs]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Parameter to show only my projects or all projects ************/
/*****************************************************************************/

void Prj_PutParamWhichPrjs (void)
  {
   Prj_GetParamWhichPrjs ();

   Par_PutHiddenParamUnsigned ("WhichPrjs",(unsigned) Gbl.CurrentCrs.Prjs.WhichPrjs);
  }

/*****************************************************************************/
/*********** Get whether to show only my projects or all projects ************/
/*****************************************************************************/

static void Prj_GetParamWhichPrjs (void)
  {
   static bool AlreadyGot = false;

   if (!AlreadyGot)
     {
      /***** Get which projects (my projects or all projects) *****/
      Gbl.CurrentCrs.Prjs.WhichPrjs = (Prj_WhichProjects_t)
	                              Par_GetParToUnsignedLong ("WhichPrjs",
	                                                        0,
	                                                        Prj_NUM_WHICH_PROJECTS - 1,
	                                                        (unsigned long) Prj_WHICH_PROJECTS_DEFAULT);

      AlreadyGot = true;
     }
  }

/*****************************************************************************/
/******************* Write header with fields of a project *******************/
/*****************************************************************************/

static void Prj_ShowProjectsHead (bool PrintView)
  {
   extern const char *Txt_PROJECT_ORDER_HELP[Prj_NUM_ORDERS];
   extern const char *Txt_PROJECT_ORDER[Prj_NUM_ORDERS];
   Prj_Order_t Order;

   fprintf (Gbl.F.Out,"<tr>"
	              "<th class=\"CONTEXT_COL\"></th>");	// Column for contextual icons
   for (Order = (Prj_Order_t) 0;
	Order <= (Prj_Order_t) (Prj_NUM_ORDERS - 1);
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");

      if (!PrintView)
	{
	 Act_FormStart (ActSeePrj);
	 Prj_PutParamWhichPrjs ();
	 Pag_PutHiddenParamPagNum (Pag_PROJECTS,Gbl.Prjs.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_PROJECT_ORDER_HELP[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Prjs.SelectedOrder)
	    fprintf (Gbl.F.Out,"<u>");
	}
      fprintf (Gbl.F.Out,"%s",Txt_PROJECT_ORDER[Order]);
      if (!PrintView)
	{
	 if (Order == Gbl.Prjs.SelectedOrder)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
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

   fprintf (Gbl.F.Out,"<tr>");

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

   fprintf (Gbl.F.Out,"</tr>");
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

   /***** Put icon to show all data in a table *****/
   Prj_PutIconToShowAllData ();

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_PROJECTS;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/********************* Put icon to create a new project **********************/
/*****************************************************************************/

static void Prj_PutIconToCreateNewPrj (void)
  {
   extern const char *Txt_New_project;

   /***** Put form to create a new project *****/
   Gbl.Prjs.PrjCodToEdit = -1L;
   Lay_PutContextualLink (ActFrmNewPrj,NULL,Prj_PutParams,
                          "plus64x64.png",
                          Txt_New_project,NULL,
                          NULL);
  }

/*****************************************************************************/
/******************** Put button to create a new project *********************/
/*****************************************************************************/

static void Prj_PutButtonToCreateNewPrj (void)
  {
   extern const char *Txt_New_project;

   Gbl.Prjs.PrjCodToEdit = -1L;
   Act_FormStart (ActFrmNewPrj);
   Prj_PutParams ();
   Btn_PutConfirmButton (Txt_New_project);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Put button to create a new project *********************/
/*****************************************************************************/

static void Prj_PutIconToShowAllData (void)
  {
   extern const char *Txt_Show_all_data_in_a_table;

   Lay_PutContextualLink (ActSeeTblAllPrj,NULL,Prj_PutParams,
			  "table64x64.gif",
			  Txt_Show_all_data_in_a_table,NULL,
		          NULL);
  }

/*****************************************************************************/
/***************** View / edit file browser of one project *******************/
/*****************************************************************************/

void Prj_ShowOneUniqueProject (struct Project *Prj)
  {
   /***** Start table *****/
   Tbl_StartTableWide (2);

   /***** Write project head *****/
   Prj_ShowProjectsHead (true);	// Print view

   /***** Show project *****/
   Prj_ShowOneProject (Prj,Prj_FILE_BROWSER_PROJECT);

   /***** End table *****/
   Tbl_EndTable ();
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
			      Gbl.CurrentIns.Ins.InsCod,
			      Gbl.CurrentDeg.Deg.DegCod,
			      Gbl.CurrentCrs.Crs.CrsCod);

   /***** Table head *****/
   Tbl_StartTableWideMargin (2);
   Prj_ShowProjectsHead (true);	// Print view

   /***** Write project *****/
   Prj_ShowOneProject (&Prj,Prj_PRINT_ONE_PROJECT);

   /***** End table *****/
   Tbl_EndTable ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/***************************** Show one project ******************************/
/*****************************************************************************/

static void Prj_ShowOneProject (struct Project *Prj,Prj_ProjectView_t ProjectView)
  {
   extern const char *Txt_Today;
   extern const char *Txt_Project_documents;
   extern const char *Txt_Preassigned_QUESTION;
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_Number_of_students;
   extern const char *Txt_See_more;
   extern const char *Txt_See_less;
   extern const char *Txt_Proposal;
   extern const char *Txt_PROJECT_STATUS[Prj_NUM_PROPOSAL_TYPES];
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   static unsigned UniqueId = 0;
   bool ICanAdminDocsProject = Prj_CheckIfICanAdminDocsProject (Prj->PrjCod);

   /***** Write first row of data of this project *****/
   /* Forms to remove/edit this project */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td rowspan=\"3\" class=\"CONTEXT_COL");
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
	 fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	 // no break
      case Prj_FILE_BROWSER_PROJECT:
         fprintf (Gbl.F.Out,"\">");
         Prj_PutFormsToRemEditOnePrj (Prj->PrjCod,Prj->Hidden,
                                      ICanAdminDocsProject);
         break;
      default:
         fprintf (Gbl.F.Out,"\">");
	 break;
     }
   fprintf (Gbl.F.Out,"</td>");

   /* Start date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"asg_date_start_%u\" class=\"%s LEFT_TOP",
	    UniqueId,
            Prj->Hidden ? (Prj->Open ? "DATE_GREEN_LIGHT" :
        	                       "DATE_RED_LIGHT") :
                          (Prj->Open ? "DATE_GREEN" :
                                       "DATE_RED"));
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_start_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>"
	              "</td>",
            UniqueId,Prj->TimeUTC[Dat_START_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /* End date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"asg_date_end_%u\" class=\"%s LEFT_TOP",
	    UniqueId,
            Prj->Hidden ? (Prj->Open ? "DATE_GREEN_LIGHT" :
        	                       "DATE_RED_LIGHT") :
                          (Prj->Open ? "DATE_GREEN" :
                                       "DATE_RED"));
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_end_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>"
	              "</td>",
            UniqueId,Prj->TimeUTC[Dat_END_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /* Project title */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">");
   if (ICanAdminDocsProject)
     {
      Act_FormStart (ActAdmDocPrj);
      Prj_PutParams ();
      Act_LinkFormSubmit (Txt_Project_documents,
                          Prj->Hidden ? "ASG_TITLE_LIGHT" :
        	                        "ASG_TITLE",
        	          NULL);
      fprintf (Gbl.F.Out,"%s</a>",Prj->Title);
      Act_FormEnd ();
     }
   else
      fprintf (Gbl.F.Out,"<div class=\"%s\">%s</div>",
               Prj->Hidden ? "ASG_TITLE_LIGHT" :
        	             "ASG_TITLE",
	       Prj->Title);
   fprintf (Gbl.F.Out,"</td>");

   /* Department */
   Prj_ShowOneProjectDepartment (Prj,ProjectView);

   /***** Preassigned? *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"RIGHT_TOP");
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">"
                      "%s:"
	              "</td>"
                      "<td colspan=\"2\" class=\"LEFT_TOP",
            Prj->Hidden ? "ASG_LABEL_LIGHT" :
        	          "ASG_LABEL",
            Txt_Preassigned_QUESTION);
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">"
                      "%s"
                      "</td>"
                      "</tr>",
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            (Prj->Preassigned == Prj_PREASSIGNED) ? Txt_Yes :
        	                                    Txt_No);

   /***** Number of students *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"RIGHT_TOP");
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">"
                      "%s:"
	              "</td>"
                      "<td colspan=\"2\" class=\"LEFT_TOP",
            Prj->Hidden ? "ASG_LABEL_LIGHT" :
        	          "ASG_LABEL",
            Txt_Number_of_students);
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">"
                      "%u"
                      "</td>"
                      "</tr>",
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Prj->NumStds);

   /***** Project members *****/
   Prj_ShowOneProjectMembers (Prj,ProjectView);

   /***** Link to show hidden info *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
	 fprintf (Gbl.F.Out,"<tr id=\"prj_exp_%u\">"
			    "<td colspan=\"5\" class=\"CENTER_MIDDLE",
		  UniqueId);
	 if (ProjectView == Prj_LIST_PROJECTS)
	    fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	 fprintf (Gbl.F.Out,"\">");
	 Prj_PutIconToToggleProject (UniqueId,"more64x64.png",Txt_See_more);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");

	 fprintf (Gbl.F.Out,"<tr id=\"prj_con_%u\" style=\"display:none;\">"
			    "<td colspan=\"5\" class=\"CENTER_MIDDLE",
		  UniqueId);
	 if (ProjectView == Prj_LIST_PROJECTS)
	    fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	 fprintf (Gbl.F.Out,"\">");
	 Prj_PutIconToToggleProject (UniqueId,"less64x64.png",Txt_See_less);
	 fprintf (Gbl.F.Out,"</td>"
			    "</tr>");
	 break;
      default:
	 break;
     }

   /***** Proposal *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
	 fprintf (Gbl.F.Out,"<tr id=\"prj_pro_%u\" style=\"display:none;\">"
			    "<td colspan=\"3\" class=\"RIGHT_TOP",
		  UniqueId);
	 if (ProjectView == Prj_LIST_PROJECTS)
	    fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	 break;
      default:
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td colspan=\"3\" class=\"RIGHT_TOP");
     }
   fprintf (Gbl.F.Out," %s\">"
                      "%s:"
	              "</td>"
                      "<td colspan=\"2\" class=\"LEFT_TOP",
            Prj->Hidden ? "ASG_LABEL_LIGHT" :
        	          "ASG_LABEL",
            Txt_Proposal);
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">"
                      "%s"
                      "</td>"
                      "</tr>",
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Txt_PROJECT_STATUS[Prj->Proposal]);

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

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/********** Put an icon to toggle on/off some fields of a project ************/
/*****************************************************************************/

static void Prj_PutIconToToggleProject (unsigned UniqueId,
                                        const char *Icon,const char *Text)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];

   /***** Link to toggle on/off some fields of project *****/
   fprintf (Gbl.F.Out,"<a href=\"\" title=\"%s\" class=\"%s\""
                      " onclick=\"toggleProject('%u');return false;\" />",
            Text,The_ClassForm[Gbl.Prefs.Theme],
            UniqueId);
   Ico_PutIconWithText (Icon,Text,Text);
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
   fprintf (Gbl.F.Out,"<tr>");

   /***** Start date/time *****/
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"asg_date_start_%u\" class=\"LEFT_TOP COLOR%u %s\">"
	              "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_start_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>"
	              "</td>",
	    UniqueId,
            Gbl.RowEvenOdd,
            Prj->Hidden ? (Prj->Open ? "DATE_GREEN_LIGHT" :
        	                       "DATE_RED_LIGHT") :
                          (Prj->Open ? "DATE_GREEN" :
                                       "DATE_RED"),
            UniqueId,Prj->TimeUTC[Dat_START_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /***** End date/time *****/
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"asg_date_end_%u\" class=\"LEFT_TOP COLOR%u %s\">"
	              "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('asg_date_end_%u',%ld,"
                      "%u,'<br />','%s',true,true,0x7);"
                      "</script>"
	              "</td>",
	    UniqueId,
            Gbl.RowEvenOdd,
            Prj->Hidden ? (Prj->Open ? "DATE_GREEN_LIGHT" :
        	                       "DATE_RED_LIGHT") :
                          (Prj->Open ? "DATE_GREEN" :
                                       "DATE_RED"),
            UniqueId,Prj->TimeUTC[Dat_END_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /***** Project title *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT_N",
            Prj->Title);

   /***** Department *****/
   Prj_ShowTableAllProjectsDepartment (Prj);

   /***** Preassigned? *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            (Prj->Preassigned == Prj_PREASSIGNED) ? Txt_Yes :
        	                                    Txt_No);

   /***** Number of students *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%u"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Prj->NumStds);

   /***** Project members *****/
   for (NumRoleToShow = 0;
	NumRoleToShow < Brw_NUM_ROLES_TO_SHOW;
	NumRoleToShow++)
      Prj_ShowTableAllProjectsMembersWithARole (Prj,Prj_RolesToShow[NumRoleToShow]);

   /***** Proposal *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Txt_PROJECT_STATUS[Prj->Proposal]);

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
   fprintf (Gbl.F.Out,"</tr>");

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
   PutLink = (ProjectView == Prj_LIST_PROJECTS && Dpt.WWW[0]);

   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">",
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT_N");
   if (PutLink)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
			 " class=\"%s\">",
	       Dpt.WWW,
               Prj->Hidden ? "DAT_LIGHT" :
        	             "DAT_N");
   fprintf (Gbl.F.Out,"%s",Dpt.FullName);
   if (PutLink)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
  }

static void Prj_ShowTableAllProjectsDepartment (const struct Project *Prj)
  {
   struct Department Dpt;

   /***** Get data of department *****/
   Dpt.DptCod = Prj->DptCod;
   Dpt_GetDataOfDepartmentByCod (&Dpt);

   /***** Show department *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Dpt.FullName);
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
   if (ProjectView == Prj_LIST_PROJECTS)
      Str_InsertLinks (TxtField,Cns_MAX_BYTES_TEXT,60);		// Insert links

   /***** Write row with label and text *****/
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
	 fprintf (Gbl.F.Out,"<tr id=\"%s%u\" style=\"display:none;\">"
			    "<td colspan=\"3\" class=\"RIGHT_TOP",
		  id,UniqueId);
	 if (ProjectView == Prj_LIST_PROJECTS)
	    fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	 break;
      default:
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td colspan=\"3\" class=\"RIGHT_TOP");
	 break;
     }
   fprintf (Gbl.F.Out," %s\">"
                      "%s:"
	              "</td>"
                      "<td colspan=\"2\" class=\"LEFT_TOP",
            Prj->Hidden ? "ASG_LABEL_LIGHT" :
        	          "ASG_LABEL",
            Label);
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">"
                      "%s"
                      "</td>"
                      "</tr>",
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            TxtField);
  }

static void Prj_ShowTableAllProjectsTxtField (struct Project *Prj,
                                              char *TxtField)
  {
   /***** Change format *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     TxtField,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML

   /***** Write text *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            TxtField);
  }

/*****************************************************************************/
/********************** Show URL associated to project ***********************/
/*****************************************************************************/

static void Prj_ShowOneProjectURL (const struct Project *Prj,
                                   Prj_ProjectView_t ProjectView,
                                   const char *id,unsigned UniqueId)
  {
   extern const char *Txt_URL;
   bool PutLink;

   /***** Write row with label and text *****/
   PutLink = (ProjectView == Prj_LIST_PROJECTS && Prj->URL[0]);

   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
      case Prj_FILE_BROWSER_PROJECT:
	 fprintf (Gbl.F.Out,"<tr id=\"%s%u\" style=\"display:none;\">"
			    "<td colspan=\"3\" class=\"RIGHT_TOP",
	          id,UniqueId);
	 if (ProjectView == Prj_LIST_PROJECTS)
	    fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	 break;
      default:
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td colspan=\"3\" class=\"RIGHT_TOP");
	 break;
     }
   fprintf (Gbl.F.Out," %s\">"
		      "%s:"
		      "</td>"
		      "<td colspan=\"2\" class=\"LEFT_TOP",
	    Prj->Hidden ? "ASG_LABEL_LIGHT" :
			  "ASG_LABEL",
	    Txt_URL);
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out," %s\">",
	    Prj->Hidden ? "DAT_LIGHT" :
			  "DAT");
   if (PutLink)
      fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\""
			 " class=\"%s\">",
	       Prj->URL,
	       Prj->Hidden ? "DAT_LIGHT" :
			     "DAT");
   fprintf (Gbl.F.Out,"%s",Prj->URL);
   if (PutLink)
      fprintf (Gbl.F.Out,"</a>");
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
  }

static void Prj_ShowTableAllProjectsURL (const struct Project *Prj)
  {
   /***** Show URL *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            Prj->URL);
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
   extern const char *Txt_Add_USER;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT];
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
      ActReqRemEvaPrj,	// Prj_ROLE_EVA, Evaluator
     };
   static const Act_Action_t ActionReqAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActReqAddStdPrj,	// Prj_ROLE_STD, Student
      ActReqAddTutPrj,	// Prj_ROLE_TUT, Tutor
      ActReqAddEvaPrj,	// Prj_ROLE_EVA, Evaluator
     };

   /***** Get users in project from database *****/
   NumUsrs = Prj_GetUsrsInPrj (Prj->PrjCod,RoleInProject,&mysql_res);
   WriteRow = (NumUsrs != 0 ||
	       ProjectView == Prj_EDIT_ONE_PROJECT);

   if (WriteRow)
     {
      /***** Start row with label and listing of users *****/
      fprintf (Gbl.F.Out,"<tr>");

      switch (ProjectView)
	{
	 case Prj_LIST_PROJECTS:
	 case Prj_FILE_BROWSER_PROJECT:
	 case Prj_PRINT_ONE_PROJECT:
	    fprintf (Gbl.F.Out,"<td colspan=\"3\" class=\"RIGHT_TOP");
	    if (ProjectView == Prj_LIST_PROJECTS)
	       fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	    fprintf (Gbl.F.Out," %s\">%s:</td>"
			       "<td colspan=\"2\" class=\"LEFT_TOP",
		     Prj->Hidden ? "ASG_LABEL_LIGHT" :
				   "ASG_LABEL",
		     NumUsrs == 1 ? Txt_PROJECT_ROLES_SINGUL_Abc[RoleInProject] :
		                    Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	    if (ProjectView == Prj_LIST_PROJECTS)
	       fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
	    fprintf (Gbl.F.Out," %s\">",
		     Prj->Hidden ? "DAT_LIGHT" :
				   "DAT");
	    break;
	 case Prj_EDIT_ONE_PROJECT:
	    fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP ASG_LABEL\">%s:</td>"
			       "<td colspan=\"2\" class=\"LEFT_TOP DAT\">",
		     Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject]);
	    break;
	}

      /***** Start table with all members with this role *****/
      Tbl_StartTable (2);

      /***** Write users *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 /* Get user's code */
	 row = mysql_fetch_row (mysql_res);
	 Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get user's data */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
	   {
	    /* Start row for this user */
	    fprintf (Gbl.F.Out,"<tr>");

	    /* Icon to remove user */
	    if (ProjectView == Prj_EDIT_ONE_PROJECT)
	      {
	       fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\" style=\"width:30px;\">");
	       Lay_PutContextualLink (ActionReqRemUsr[RoleInProject],NULL,Prj_PutParams,
				      "remove-on64x64.png",
				      Txt_Remove,NULL,
				      NULL);
	       fprintf (Gbl.F.Out,"</td>");
	      }

	    /* Put user's photo */
	    fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\" style=\"width:30px;\">");
	    ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Other.UsrDat,PhotoURL);
	    Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,ShowPhoto ? PhotoURL :
								 NULL,
			      "PHOTO21x28",Pho_ZOOM,false);
	    fprintf (Gbl.F.Out,"</td>");

	    /* Write user's IDs */
	    if (RoleInProject == Prj_ROLE_STD)
	      {
	       fprintf (Gbl.F.Out,"<td class=\"AUTHOR_TXT LEFT_MIDDLE\">");
	       ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
	       fprintf (Gbl.F.Out,"</td>");
	      }

	    /* Write user's name */
	    fprintf (Gbl.F.Out,"<td class=\"AUTHOR_TXT LEFT_MIDDLE\">%s</td>",
		     Gbl.Usrs.Other.UsrDat.FullName);

	    /* End row for this user */
	    fprintf (Gbl.F.Out,"</tr>");
	   }
	}

      /***** Row to add a new user *****/
      if (ProjectView == Prj_EDIT_ONE_PROJECT)
	{
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"CENTER_TOP\" style=\"width:30px;\">");
	 Gbl.Prjs.PrjCodToEdit = Prj->PrjCod;	// Used to pass project code as a parameter
	 sprintf (Gbl.Title,Txt_Add_USER,Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject]);
	 Lay_PutContextualLink (ActionReqAddUsr[RoleInProject],NULL,Prj_PutParams,
				"plus64x64.png",
				Gbl.Title,NULL,
				NULL);
	 fprintf (Gbl.F.Out,"</td>"
			    "<td style=\"width:30px;\">"	// Column for photo
			    "</td>");
	 if (RoleInProject == Prj_ROLE_STD)
	    fprintf (Gbl.F.Out,"<td></td>");		// Column for user's IDs
	 fprintf (Gbl.F.Out,"<td></td>"			// Column for name
			    "</tr>");
	}

      /***** End table with all members with this role *****/
      Tbl_EndTable ();

      /***** End row with label and listing of users *****/
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
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
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">",
	    Gbl.RowEvenOdd,
	    Prj->Hidden ? "DAT_LIGHT" :
			  "DAT");

   if (NumUsrs)
     {
      /***** Write users *****/
      fprintf (Gbl.F.Out,"<ul>");

      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
	 /* Get user's code */
	 row = mysql_fetch_row (mysql_res);
	 Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get user's data */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
	   {
	    /* Start row for this user */
	    fprintf (Gbl.F.Out,"<li>");

	    /* Write user's IDs */
	    if (RoleInProject == Prj_ROLE_STD)
	      {
	       ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
	       fprintf (Gbl.F.Out,"<br />");
	      }

	    /* Write user's name */
	    fprintf (Gbl.F.Out,"%s",Gbl.Usrs.Other.UsrDat.FullName);

	    /* End row for this user */
	    fprintf (Gbl.F.Out,"</li>");
	   }
	}

      fprintf (Gbl.F.Out,"</ul>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End column with list of all members with this role *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/******************** Get number of users in a project ***********************/
/*****************************************************************************/

static unsigned Prj_GetUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject,
                                  MYSQL_RES **mysql_res)
  {
   char Query[1024];

   /***** Get users in project from database *****/
   sprintf (Query,"SELECT prj_usr.UsrCod,"
	          "usr_data.Surname1 AS S1,"
	          "usr_data.Surname2 AS S2,"
	          "usr_data.FirstName AS FN"
                  " FROM prj_usr,usr_data"
                  " WHERE prj_usr.PrjCod=%ld AND RoleInProject=%u"
                  " AND prj_usr.UsrCod=usr_data.UsrCod"
                  " ORDER BY S1,S2,FN",
            PrjCod,(unsigned) RoleInProject);
   return (unsigned) DB_QuerySELECT (Query,mysql_res,
                                     "can not get users in project");
  }

/*****************************************************************************/
/************************** Get my role in a project *************************/
/*****************************************************************************/

Prj_RoleInProject_t Prj_GetMyRoleInProject (long PrjCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned UnsignedNum;
   Prj_RoleInProject_t RoleInProject = Prj_ROLE_UNK;

   /***** Get my role in project from database *****/
   sprintf (Query,"SELECT RoleInProject FROM prj_usr WHERE PrjCod=%ld",PrjCod);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get my role in project"))
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Prj_NUM_ROLES_IN_PROJECT)
	    RoleInProject = (Prj_RoleInProject_t) UnsignedNum;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return RoleInProject;
  }

/*****************************************************************************/
/*** Request another user's ID, @nickname or email to add user to project ****/
/*****************************************************************************/

void Prj_ReqAddStd (void)
  {
   Prj_ReqAnotherUsrID (Prj_ROLE_STD);
  }

void Prj_ReqAddTut (void)
  {
   Prj_ReqAnotherUsrID (Prj_ROLE_TUT);
  }

void Prj_ReqAddEva (void)
  {
   Prj_ReqAnotherUsrID (Prj_ROLE_EVA);
  }

static void Prj_ReqAnotherUsrID (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Hlp_ASSESSMENT_Projects_add_user;
   extern const char *Txt_Add_USER;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT];
   static Act_Action_t ActionAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActAddStdPrj,	// Prj_ROLE_STD, Student
      ActAddTutPrj,	// Prj_ROLE_TUT, Tutor
      ActAddEvaPrj,	// Prj_ROLE_EVA, Evaluator
     };

   /***** Get project code *****/
   if ((Gbl.Prjs.PrjCodToEdit = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Start box *****/
   sprintf (Gbl.Title,Txt_Add_USER,Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject]);
   Box_StartBox (NULL,Gbl.Title,NULL,
                 Hlp_ASSESSMENT_Projects_add_user,Box_NOT_CLOSABLE);

   /***** Write form to request another user's ID *****/
   Enr_WriteFormToReqAnotherUsrID (ActionAddUsr[RoleInProject],Prj_PutParams);

   /***** End box *****/
   Box_EndBox ();

   /***** Put a form to create/edit project *****/
   Prj_RequestCreatOrEditPrj (Gbl.Prjs.PrjCodToEdit);
  }

/*****************************************************************************/
/**************************** Add user to project ****************************/
/*****************************************************************************/

void Prj_AddStd (void)
  {
   Prj_AddUsrToProject (Prj_ROLE_STD);
  }

void Prj_AddTut (void)
  {
   Prj_AddUsrToProject (Prj_ROLE_TUT);
  }

void Prj_AddEva (void)
  {
   Prj_AddUsrToProject (Prj_ROLE_EVA);
  }

static void Prj_AddUsrToProject (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   long PrjCod;
   struct ListUsrCods ListUsrCods;
   unsigned NumUsr;
   char Query[512];

   /***** Get project code *****/
   if ((PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Use user's ID to identify the user(s) to be enroled /removed *****/
   Usr_GetParamOtherUsrIDNickOrEMailAndGetUsrCods (&ListUsrCods);

   if (ListUsrCods.NumUsrs)	// User(s) found with the ID
     {
      /***** For each user found... *****/
      for (NumUsr = 0;
	   NumUsr < ListUsrCods.NumUsrs;
	   NumUsr++)
	{
	 /* Get user's data */
         Gbl.Usrs.Other.UsrDat.UsrCod = ListUsrCods.Lst[NumUsr];
         Usr_GetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat);

	 /* Add user to project */
	 sprintf (Query,"REPLACE INTO prj_usr"
			" (PrjCod,RoleInProject,UsrCod)"
			" VALUES"
			" (%ld,%u,%ld)",
		  PrjCod,(unsigned) RoleInProject,Gbl.Usrs.Other.UsrDat.UsrCod);
	 DB_QueryREPLACE (Query,"can not add user to project");

	 /* Show success alert */
	 sprintf (Gbl.Alert.Txt,Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project,
		  Gbl.Usrs.Other.UsrDat.FullName,
		  Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject]);
	 Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	}

      /***** Free list of users' codes *****/
      Usr_FreeListUsrCods (&ListUsrCods);
     }
   else	// No users found
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

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

void Prj_ReqRemEva (void)
  {
   Prj_ReqRemUsrFromPrj (Prj_ROLE_EVA);
  }

static void Prj_ReqRemUsrFromPrj (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_Remove_me_from_this_project;
   extern const char *Txt_Remove_user_from_this_project;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   static Act_Action_t ActionRemUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActRemStdPrj,	// Prj_ROLE_STD, Student
      ActRemTutPrj,	// Prj_ROLE_TUT, Tutor
      ActRemEvaPrj,	// Prj_ROLE_EVA, Evaluator
     };
   struct Project Prj;
   bool ItsMe;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (Prj.PrjCod))
	{
	 ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);

	 /***** Show question and button to remove user as administrator *****/
	 /* Start alert */
	 sprintf (Gbl.Alert.Txt,
		  ItsMe ? Txt_Do_you_really_want_to_be_removed_as_a_X_from_the_project_Y :
			  Txt_Do_you_really_want_to_remove_the_following_user_as_a_X_from_the_project_Y,
		  Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject],
		  Prj.Title);
	 Ale_ShowAlertAndButton1 (Ale_QUESTION,Gbl.Alert.Txt);

	 /* Show user's record */
	 Rec_ShowSharedRecordUnmodifiable (&Gbl.Usrs.Other.UsrDat);

	 /* Show form to request confirmation */
	 Act_FormStart (ActionRemUsr[RoleInProject]);
	 Gbl.Prjs.PrjCodToEdit = Prj.PrjCod;
	 Prj_PutParams ();
	 Btn_PutRemoveButton (ItsMe ? Txt_Remove_me_from_this_project :
				      Txt_Remove_user_from_this_project);
	 Act_FormEnd ();

	 /* End alert */
	 Ale_ShowAlertAndButton2 (ActUnk,NULL,NULL,NULL,Btn_NO_BUTTON,NULL);
	}
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

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

void Prj_RemEva (void)
  {
   Prj_RemUsrFromPrj (Prj_ROLE_EVA);
  }

static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_THE_USER_X_has_been_removed_as_a_Y_from_the_project_Z;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT];
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Query[256];
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanEditProject (Prj.PrjCod))
	{
	 /***** Remove user from the table of project-users *****/
	 sprintf (Query,"DELETE FROM prj_usr"
	                " WHERE PrjCod=%ld AND RoleInProject=%u AND UsrCod=%ld",
		  Prj.PrjCod,
		  (unsigned) RoleInProject,
		  Gbl.Usrs.Other.UsrDat.UsrCod);
	 DB_QueryDELETE (Query,"can not remove a user from a project");

	 /***** Show success alert *****/
	 sprintf (Gbl.Alert.Txt,Txt_THE_USER_X_has_been_removed_as_a_Y_from_the_project_Z,
		  Gbl.Usrs.Other.UsrDat.FullName,
		  Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject],
		  Prj.Title);
         Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	}
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);

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

static void Prj_PutFormsToRemEditOnePrj (long PrjCod,bool Hidden,
                                         bool ICanAdminDocsProject)
  {
   Gbl.Prjs.PrjCodToEdit = PrjCod;	// Used as parameter in contextual links

   if (Prj_CheckIfICanEditProject (PrjCod))
     {
      /***** Put form to remove project *****/
      Ico_PutContextualIconToRemove (ActReqRemPrj,Prj_PutParams);

      /***** Put form to hide/show project *****/
      if (Hidden)
	 Ico_PutContextualIconToUnhide (ActShoPrj,Prj_PutParams);
      else
	 Ico_PutContextualIconToHide (ActHidPrj,Prj_PutParams);

      /***** Put form to edit project *****/
      Ico_PutContextualIconToEdit (ActEdiOnePrj,Prj_PutParams);
     }

   /***** Put form to admin project documents *****/
   if (ICanAdminDocsProject)
      Ico_PutContextualIconToViewFiles (ActAdmDocPrj,Prj_PutParams);

   /***** Put form to print project *****/
   Ico_PutContextualIconToPrint (ActPrnOnePrj,Prj_PutParams);
  }

/*****************************************************************************/
/***************** Can I admin documents of a given project? *****************/
/*****************************************************************************/

bool Prj_CheckIfICanAdminDocsProject (long PrjCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
      case Rol_NET:
      case Rol_TCH:
	 return Prj_GetIfIAmMemberOfProject (PrjCod);
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/************************ Can I edit a given project? ************************/
/*****************************************************************************/

static bool Prj_CheckIfICanEditProject (long PrjCod)
  {
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
	 return Prj_GetIfIAmTutorInProject (PrjCod);
      case Rol_TCH:
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/*********************** Am I member of a given project? *********************/
/*****************************************************************************/

static bool Prj_GetIfIAmMemberOfProject (long PrjCod)
  {
   char Query[256];

   sprintf (Query,"SELECT COUNT(*) FROM prj_usr"
		  " WHERE PrjCod=%ld AND UsrCod=%ld",
	    PrjCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (bool) (DB_QueryCOUNT (Query,"can not check if I am a member of a project") != 0);
  }

/*****************************************************************************/
/*********************** Am I tutor in a given project? **********************/
/*****************************************************************************/

static bool Prj_GetIfIAmTutorInProject (long PrjCod)
  {
   char Query[256];

   sprintf (Query,"SELECT COUNT(*) FROM prj_usr"
		  " WHERE PrjCod=%ld AND RoleInProject=%u AND UsrCod=%ld",
	    PrjCod,Prj_ROLE_TUT,Gbl.Usrs.Me.UsrDat.UsrCod);
   return (bool) (DB_QueryCOUNT (Query,"can not check if I am a tutor in a project") != 0);
  }

/*****************************************************************************/
/********************** Params used to edit a project ************************/
/*****************************************************************************/

static void Prj_PutParams (void)
  {
   if (Gbl.Prjs.PrjCodToEdit > 0)
      Prj_PutParamPrjCod (Gbl.Prjs.PrjCodToEdit);
   Prj_PutHiddenParamPrjOrder ();
   Prj_PutParamWhichPrjs ();
   Pag_PutHiddenParamPagNum (Pag_PROJECTS,Gbl.Prjs.CurrentPage);
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted ();
  }

/*****************************************************************************/
/************************** List all the projects ****************************/
/*****************************************************************************/

void Prj_GetListProjects (void)
  {
   char HiddenSubQuery[256];
   char OrderBySubQuery[256];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumPrj;

   if (Gbl.Prjs.LstIsRead)
      Prj_FreeListProjects ();

   /***** Get list of projects from database *****/
   /* Hidden subquery */
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 HiddenSubQuery[0] = '\0';
	 break;
      default:
	 sprintf (HiddenSubQuery," AND projects.Hidden='N'");
	 break;
     }

   /* Order subquery */
   switch (Gbl.Prjs.SelectedOrder)
     {
      case Prj_ORDER_START_TIME:
	 sprintf (OrderBySubQuery,"projects.StartTime DESC,"
				  "projects.EndTime DESC,"
				  "projects.Title");
	 break;
      case Prj_ORDER_END_TIME:
	 sprintf (OrderBySubQuery,"projects.EndTime DESC,"
				  "projects.StartTime DESC,"
				  "projects.Title");
	 break;
      case Prj_ORDER_TITLE:
	 sprintf (OrderBySubQuery,"projects.Title,"
				  "projects.StartTime DESC,"
				  "projects.EndTime DESC");
	 break;
      case Prj_ORDER_DEPARTMENT:
	 sprintf (OrderBySubQuery,"departments.FullName,"
				  "projects.StartTime DESC,"
				  "projects.EndTime DESC,"
				  "projects.Title");
	 break;
     }

   /* Query */
   if (Gbl.CurrentCrs.Prjs.WhichPrjs == Prj_ONLY_MY_PROJECTS)
      switch (Gbl.Prjs.SelectedOrder)
	{
	 case Prj_ORDER_START_TIME:
	 case Prj_ORDER_END_TIME:
	 case Prj_ORDER_TITLE:
	    sprintf (Query,"SELECT projects.PrjCod"
			   " FROM projects,prj_usr"
			   " WHERE projects.CrsCod=%ld%s"
			   " AND projects.PrjCod=prj_usr.PrjCod"
			   " AND prj_usr.UsrCod=%ld"
			   " ORDER BY %s",
		     Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,
		     Gbl.Usrs.Me.UsrDat.UsrCod,
		     OrderBySubQuery);
	    break;
	 case Prj_ORDER_DEPARTMENT:
	    sprintf (Query,"SELECT projects.PrjCod"
			   " FROM prj_usr,projects LEFT JOIN departments"
			   " ON projects.DptCod=departments.DptCod"
			   " WHERE projects.CrsCod=%ld%s"
			   " AND projects.PrjCod=prj_usr.PrjCod"
			   " AND prj_usr.UsrCod=%ld"
			   " ORDER BY %s",
		     Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,
		     Gbl.Usrs.Me.UsrDat.UsrCod,
		     OrderBySubQuery);
	    break;
	}
   else	// Gbl.CurrentCrs.Prjs.WhichPrjs == Prj_ALL_PROJECTS
      switch (Gbl.Prjs.SelectedOrder)
	{
	 case Prj_ORDER_START_TIME:
	 case Prj_ORDER_END_TIME:
	 case Prj_ORDER_TITLE:
	    sprintf (Query,"SELECT projects.PrjCod"
			   " FROM projects"
			   " WHERE projects.CrsCod=%ld%s"
			   " ORDER BY %s",
		     Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,
		     OrderBySubQuery);
	    break;
	 case Prj_ORDER_DEPARTMENT:
	    sprintf (Query,"SELECT projects.PrjCod"
			   " FROM projects LEFT JOIN departments"
			   " ON projects.DptCod=departments.DptCod"
			   " WHERE projects.CrsCod=%ld%s"
			   " ORDER BY %s",
		     Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,
		     OrderBySubQuery);
	    break;
	}

   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get projects");

   if (NumRows) // Projects found...
     {
      Gbl.Prjs.Num = (unsigned) NumRows;

      /***** Create list of projects *****/
      if ((Gbl.Prjs.LstPrjCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store list of projects.");

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

   Gbl.Prjs.LstIsRead = true;
  }

/*****************************************************************************/
/****************** Check if a project exists in a course ********************/
/*****************************************************************************/

long Prj_GetCourseOfProject (long PrjCod)
  {
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long CrsCod = -1L;

   if (PrjCod > 0)
     {
      /***** Get course code from database *****/
      sprintf (Query,"SELECT CrsCod FROM projects WHERE PrjCod=%ld",PrjCod);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get project course")) // Project found...
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
   char Query[1024];

   if (Prj->PrjCod > 0)
     {
      /***** Build query *****/
      sprintf (Query,"SELECT PrjCod,CrsCod,DptCod,Hidden,Preassigned,NumStds,Proposal,"
		     "UNIX_TIMESTAMP(StartTime),"
		     "UNIX_TIMESTAMP(EndTime),"
		     "NOW() BETWEEN StartTime AND EndTime,"
		     "Title,Description,Knowledge,Materials,URL"
		     " FROM projects"
		     " WHERE PrjCod=%ld AND CrsCod=%ld",
	       Prj->PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
      /*
      row[ 0]: PrjCod
      row[ 1]: CrsCod
      row[ 2]: DptCod
      row[ 3]: Hidden
      row[ 4]: Preassigned
      row[ 5]: NumStds
      row[ 6]: Proposal
      row[ 7]: UNIX_TIMESTAMP(StartTime)
      row[ 8]: UNIX_TIMESTAMP(EndTime)
      row[ 9]: NOW() BETWEEN StartTime AND EndTime
      row[10]: Title
      row[11]: Description
      row[12]: Knowledge
      row[13]: Materials
      row[14]: URL
      */

      /***** Get data of project *****/
      Prj_GetDataOfProject (Prj,Query);
     }
   else
     {
      /***** Clear all project data *****/
      Prj->PrjCod = -1L;
      Prj_ResetProject (Prj);
     }
  }

/*****************************************************************************/
/**************************** Get project data *******************************/
/*****************************************************************************/

static void Prj_GetDataOfProject (struct Project *Prj,const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long NumLong;
   Prj_Proposal_t Proposal;

   /***** Clear all project data *****/
   Prj_ResetProject (Prj);

   /***** Get data of project from database *****/
   if (DB_QuerySELECT (Query,&mysql_res,"can not get project data")) // Project found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);
      /*
      row[ 0]: PrjCod
      row[ 1]: CrsCod
      row[ 2]: DptCod
      row[ 3]: Hidden
      row[ 4]: Preassigned
      row[ 5]: NumStds
      row[ 6]: Proposal
      row[ 7]: UNIX_TIMESTAMP(StartTime)
      row[ 8]: UNIX_TIMESTAMP(EndTime)
      row[ 9]: NOW() BETWEEN StartTime AND EndTime
      row[10]: Title
      row[11]: Description
      row[12]: Knowledge
      row[13]: Materials
      row[14]: URL
      */

      /* Get code of the project (row[0]) */
      Prj->PrjCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get code of the course (row[1]) */
      Prj->CrsCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get code of the department (row[2]) */
      Prj->DptCod = Str_ConvertStrCodToLongCod (row[2]);

      /* Get whether the project is hidden or not (row[3]) */
      Prj->Hidden = (row[3][0] == 'Y');

      /* Get if project is preassigned or not (row[4]) */
      Prj->Preassigned = (row[4][0] == 'Y') ? Prj_PREASSIGNED :
	                                      Prj_NOT_PREASSIGNED;

      /* Get if project is preassigned or not (row[5]) */
      NumLong = Str_ConvertStrCodToLongCod (row[5]);
      if (NumLong >= 0)
         Prj->NumStds = (unsigned) NumLong;
      else
	 Prj->NumStds = 1;

      /* Get project status (row[6]) */
      Prj->Proposal = Prj_PROPOSAL_DEFAULT;
      for (Proposal  = (Prj_Proposal_t) 0;
	   Proposal <= (Prj_Proposal_t) (Prj_NUM_PROPOSAL_TYPES - 1);
	   Proposal++)
	 if (!strcmp (Prj_Proposal_DB[Proposal],row[6]))
	   {
	    Prj->Proposal = Proposal;
	    break;
	   }

      /* Get start date (row[7] holds the start UTC time) */
      Prj->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[7]);

      /* Get end date   (row[8] holds the end   UTC time) */
      Prj->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[8]);

      /* Get whether the project is open or closed (row[9]) */
      Prj->Open = (row[9][0] == '1');

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

/*****************************************************************************/
/************************** Clear all project data ***************************/
/*****************************************************************************/

static void Prj_ResetProject (struct Project *Prj)
  {
   if (Prj->PrjCod <= 0)	// If > 0 ==> keep value
      Prj->PrjCod = -1L;
   Prj->CrsCod = -1L;
   Prj->Hidden = false;
   Prj->Preassigned = Prj_PREASSIGNED_DEFAULT;
   Prj->NumStds     = 1;
   Prj->Proposal    = Prj_PROPOSAL_DEFAULT;
   Prj->TimeUTC[Dat_START_TIME] =
   Prj->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Prj->Open = false;
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
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (Prj.PrjCod))
     {
      /***** Show question and button to remove the project *****/
      Gbl.Prjs.PrjCodToEdit = Prj.PrjCod;
      sprintf (Gbl.Alert.Txt,Txt_Do_you_really_want_to_remove_the_project_X,
	       Prj.Title);
      Ale_ShowAlertAndButton (Ale_QUESTION,Gbl.Alert.Txt,
			      ActRemPrj,NULL,NULL,Prj_PutParams,
			      Btn_REMOVE_BUTTON,Txt_Remove_project);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"You don't have permission to edit this project.");

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
   char Query[256];
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);	// Inside this function, the course is checked to be the current one

   if (Prj_CheckIfICanEditProject (Prj.PrjCod))
     {
      /***** Remove users in project *****/
      sprintf (Query,"DELETE FROM prj_usr USING projects,prj_usr"
		     " WHERE projects.PrjCod=%ld AND projects.CrsCod=%ld"
		     " AND projects.PrjCod=prj_usr.PrjCod",
	       Prj.PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
      DB_QueryDELETE (Query,"can not remove project");

      /***** Remove project *****/
      sprintf (Query,"DELETE FROM projects"
		     " WHERE PrjCod=%ld AND CrsCod=%ld",
	       Prj.PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
      DB_QueryDELETE (Query,"can not remove project");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_Project_X_removed,
	       Prj.Title);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"You don't have permission to edit this project.");

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
   extern const char *Txt_Project_X_is_now_hidden;
   char Query[512];
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (Prj.PrjCod))
     {
      /***** Hide project *****/
      sprintf (Query,"UPDATE projects SET Hidden='Y'"
		     " WHERE PrjCod=%ld AND CrsCod=%ld",
	       Prj.PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
      DB_QueryUPDATE (Query,"can not hide project");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_Project_X_is_now_hidden,
	       Prj.Title);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"You don't have permission to edit this project.");

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
   extern const char *Txt_Project_X_is_now_visible;
   char Query[512];
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) < 0)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   if (Prj_CheckIfICanEditProject (Prj.PrjCod))
     {
      /***** Show project *****/
      sprintf (Query,"UPDATE projects SET Hidden='N'"
		     " WHERE PrjCod=%ld AND CrsCod=%ld",
	       Prj.PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
      DB_QueryUPDATE (Query,"can not show project");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_Project_X_is_now_visible,
	       Prj.Title);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"You don't have permission to edit this project.");

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

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
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   ItsANewProject = ((Prj.PrjCod = PrjCod) < 0);

   /***** Get from the database the data of the project *****/
   if (ItsANewProject)
     {
      /* Initialize to empty project */
      Prj_ResetProject (&Prj);
      Prj.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
      Prj.TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC +
	                            Prj_INTERVAL_DEFAULT;
      Prj.Open = true;
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
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_project;
   extern const char *Txt_Edit_project;
   extern const char *Txt_Data;
   extern const char *Txt_Title;
   extern const char *Txt_Department;
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
   extern const char *Txt_Save;
   extern const char *Txt_Members;
   Prj_Proposal_t Proposal;
   unsigned NumRoleToShow;

   /***** Start project box *****/
   if (ItsANewProject)
     {
      Gbl.Prjs.PrjCodToEdit = -1L;
      Box_StartBox (NULL,Txt_New_project,NULL,
                    Hlp_ASSESSMENT_Projects_new_project,Box_NOT_CLOSABLE);
     }
   else
     {
      Gbl.Prjs.PrjCodToEdit = Prj->PrjCod;
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
   Act_FormStart (ItsANewProject ? ActNewPrj :
	                           ActChgPrj);
   Prj_PutParams ();

   /* Start box and table */
   Box_StartBoxTable (NULL,Txt_Data,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /* Project title */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Title\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" id=\"Title\" name=\"Title\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Title,
            Prj_MAX_CHARS_PROJECT_TITLE,Prj->Title);

   /* Project start and end dates */
   Dat_PutFormStartEndClientLocalDateTimes (Prj->TimeUTC,Dat_FORM_SECONDS_ON);

   /* Department */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"DptCod\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Department);
   Dpt_WriteSelectorDepartment (Gbl.CurrentIns.Ins.InsCod,
                                Prj->DptCod,	// Selected department
                                false);		// Don't submit on change
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Preassigned? */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<select name=\"Preassigned\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Preassigned_QUESTION);

   fprintf (Gbl.F.Out,"<option value=\"N\"");
   if (Prj->Preassigned == Prj_NOT_PREASSIGNED)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_No);

   fprintf (Gbl.F.Out,"<option value=\"Y\"");
   if (Prj->Preassigned == Prj_PREASSIGNED)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_Yes);

   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
                      "</tr>");

   /* Number of students */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"number\" name=\"NumStds\""
                      " min=\"0\" value=\"%u\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Number_of_students,
            Prj->NumStds);

   /* Proposal */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<select name=\"Proposal\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Proposal);
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
   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
                      "</tr>");

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
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"RIGHT_MIDDLE\">"
		      "<label for=\"WWW\" class=\"%s\">%s:</label>"
		      "</td>"
		      "<td class=\"DAT LEFT_MIDDLE\">"
                      "<input type=\"url\" id=\"URL\" name=\"URL\""
		      " size=\"45\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
		      "</tr>",
	    The_ClassForm[Gbl.Prefs.Theme],
	    Txt_URL,
	    Cns_MAX_CHARS_WWW,Prj->URL);

   /* End table, send button and end box */
   if (ItsANewProject)
      Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_project);
   else
      Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_Save);

   /* End data form */
   Act_FormEnd ();

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
   extern const char *The_ClassForm[The_NUM_THEMES];

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_TOP\">"
	              "<label for=\"%s\" class=\"%s\">%s:</label>"
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea id=\"%s\" name=\"%s\" cols=\"60\" rows=\"%u\">"
                      "%s"
                      "</textarea>"
                      "</td>"
                      "</tr>",
            Id,The_ClassForm[Gbl.Prefs.Theme],Label,
            Id,Id,
            NumRows,
            TxtField);
  }

/*****************************************************************************/
/*** Allocate memory for those parameters of a project with a lot of text ****/
/*****************************************************************************/

void Prj_AllocMemProject (struct Project *Prj)
  {
   if ((Prj->Description = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store project.");

   if ((Prj->Knowledge   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store project.");

   if ((Prj->Materials   = malloc (Cns_MAX_BYTES_TEXT + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store project.");
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
   Prj_GetParamPrjOrder ();
   Prj_GetParamWhichPrjs ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
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

      ICanEditProject = Prj_CheckIfICanEditProject (Prj.PrjCod);
     }

   if (ICanEditProject)
     {
      /* Get start/end date-times */
      Prj.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
      Prj.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

      /* Get project title */
      Par_GetParToText ("Title",Prj.Title,Prj_MAX_BYTES_PROJECT_TITLE);

      /* Get department */
      Prj.DptCod = Par_GetParToLong ("DptCod");

      /* Get whether the project is preassigned */
      Prj.Preassigned = (Par_GetParToBool ("Preassigned")) ? Prj_PREASSIGNED :
							     Prj_NOT_PREASSIGNED;

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

      /***** Adjust dates *****/
      if (Prj.TimeUTC[Dat_START_TIME] == 0)
	 Prj.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
      if (Prj.TimeUTC[Dat_END_TIME] == 0)
	 Prj.TimeUTC[Dat_END_TIME] = Prj.TimeUTC[Dat_START_TIME] +
				     Prj_INTERVAL_DEFAULT;

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
	    Prj_CreateProject (&Prj);	// Add new project to database

	    /***** Write success message *****/
	    sprintf (Gbl.Alert.Txt,Txt_Created_new_project_X,Prj.Title);
	    Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	   }
	 else if (NewProjectIsCorrect)
	   {
	    Prj_UpdateProject (&Prj);

	    /***** Write success message *****/
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_project_has_been_modified);
	   }
	}
      else
         Prj_PutFormProject (&Prj,ItsANewProject);

      /***** Show again form to edit project *****/
      Prj_RequestCreatOrEditPrj (Prj.PrjCod);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"You don't have permission to edit this project.");

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/************************** Create a new project *****************************/
/*****************************************************************************/

static void Prj_CreateProject (struct Project *Prj)
  {
   char Query[2048 +
              Prj_MAX_BYTES_PROJECT_TITLE +
              Cns_MAX_BYTES_TEXT * 3 +
              Cns_MAX_BYTES_WWW];

   /***** Create a new project *****/
   sprintf (Query,"INSERT INTO projects"
	          " (CrsCod,DptCod,Hidden,Preassigned,NumStds,Proposal,"
	          "StartTime,EndTime,"
	          "Title,Description,Knowledge,Materials,URL)"
                  " VALUES"
                  " (%ld,%ld,'%c','%c',%u,'%s',"
                  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
                  "'%s','%s','%s','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,
            Prj->DptCod,
            Prj->Hidden ? 'Y' :
        	          'N',
            Prj->Preassigned == Prj_PREASSIGNED ? 'Y' :
        	                                  'N',
            Prj->NumStds,
            Prj_Proposal_DB[Prj->Proposal],
            Prj->TimeUTC[Dat_START_TIME],
            Prj->TimeUTC[Dat_END_TIME  ],
            Prj->Title,
            Prj->Description,
            Prj->Knowledge,
            Prj->Materials,
            Prj->URL);
   Prj->PrjCod = DB_QueryINSERTandReturnCode (Query,"can not create new project");

   /***** Insert creator as first tutor *****/
   sprintf (Query,"INSERT INTO prj_usr"
	          " (PrjCod,RoleInProject,UsrCod)"
                  " VALUES"
                  " (%ld,%u,%ld)",
            Prj->PrjCod,
            (unsigned) Prj_ROLE_TUT,
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryINSERT (Query,"can not add tutor");
  }

/*****************************************************************************/
/*********************** Update an existing project **************************/
/*****************************************************************************/

static void Prj_UpdateProject (struct Project *Prj)
  {
   char Query[1024 +
              Prj_MAX_BYTES_PROJECT_TITLE +
              Cns_MAX_BYTES_TEXT*3 +
              Cns_MAX_BYTES_WWW];

   /***** Update the data of the project *****/
   sprintf (Query,"UPDATE projects SET "
	          "DptCod=%ld,Hidden='%c',Preassigned='%c',NumStds=%u,Proposal='%s',"
	          "StartTime=FROM_UNIXTIME(%ld),"
	          "EndTime=FROM_UNIXTIME(%ld),"
                  "Title='%s',"
                  "Description='%s',Knowledge='%s',Materials='%s',URL='%s'"
                  " WHERE PrjCod=%ld AND CrsCod=%ld",
            Prj->DptCod,
            Prj->Hidden ? 'Y' :
        	          'N',
            Prj->Preassigned == Prj_PREASSIGNED ? 'Y' :
        	                                  'N',
            Prj->NumStds,
            Prj_Proposal_DB[Prj->Proposal],
            Prj->TimeUTC[Dat_START_TIME],
            Prj->TimeUTC[Dat_END_TIME  ],
            Prj->Title,
            Prj->Description,
            Prj->Knowledge,
            Prj->Materials,
            Prj->URL,
            Prj->PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not update project");
  }

/*****************************************************************************/
/******************** Remove all the projects in a course ********************/
/*****************************************************************************/

void Prj_RemoveCrsProjects (long CrsCod)
  {
   char Query[256];

   /***** Remove users in projects of the course *****/
   sprintf (Query,"DELETE FROM prj_usr USING projects,prj_usr"
                  " WHERE projects.CrsCod=%ld"
                  " AND projects.PrjCod=prj_usr.PrjCod",
            CrsCod);
   DB_QueryDELETE (Query,"can not remove all the projects of a course");

   /***** Remove projects *****/
   sprintf (Query,"DELETE FROM projects WHERE CrsCod=%ld",CrsCod);
   DB_QueryDELETE (Query,"can not remove all the projects of a course");
  }

/*****************************************************************************/
/******************* Remove user from all his/her projects *******************/
/*****************************************************************************/

void Prj_RemoveUsrFromProjects (long UsrCod)
  {
   char Query[128];

   /***** Remove user from projects *****/
   sprintf (Query,"DELETE FROM prj_usr WHERE UsrCod=%ld",UsrCod);
   DB_QueryDELETE (Query,"can not remove user from projects");
  }

/*****************************************************************************/
/******************** Get number of courses with projects ********************/
/*****************************************************************************/
// Returns the number of courses with projects
// in this location (all the platform, current degree or current course)

unsigned Prj_GetNumCoursesWithProjects (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCourses;

   /***** Get number of courses with projects from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(DISTINCT CrsCod)"
                        " FROM projects"
                        " WHERE CrsCod>0");
         break;
       case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT projects.CrsCod)"
                        " FROM institutions,centres,degrees,courses,projects"
                        " WHERE institutions.CtyCod=%ld"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
       case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT projects.CrsCod)"
                        " FROM centres,degrees,courses,projects"
                        " WHERE centres.InsCod=%ld"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT projects.CrsCod)"
                        " FROM degrees,courses,projects"
                        " WHERE degrees.CtrCod=%ld"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT projects.CrsCod)"
                        " FROM courses,projects"
                        " WHERE courses.DegCod=%ld"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT CrsCod)"
                        " FROM projects"
                        " WHERE CrsCod=%ld",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of courses with projects");

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

unsigned Prj_GetNumProjects (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumProjects;

   /***** Get number of projects from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM projects"
                        " WHERE CrsCod>0");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM institutions,centres,degrees,courses,projects"
                        " WHERE institutions.CtyCod=%ld"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM centres,degrees,courses,projects"
                        " WHERE centres.InsCod=%ld"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM degrees,courses,projects"
                        " WHERE degrees.CtrCod=%ld"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM courses,projects"
                        " WHERE courses.DegCod=%ld"
                        " AND courses.CrsCod=projects.CrsCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM projects"
                        " WHERE CrsCod=%ld",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of projects");

   /***** Get number of projects *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumProjects) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of projects.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumProjects;
  }
