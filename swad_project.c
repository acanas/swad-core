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
/************************* Public constants and types ************************/
/*****************************************************************************/

typedef enum
  {
   Prj_LIST_PROJECTS,
   Prj_PRINT_ONE_PROJECT,
   Prj_EDIT_ONE_PROJECT,
  } Prj_ProjectView_t;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Prj_ShowAllProjects (void);
static void Prj_PutHeadForSeeing (bool PrintView);
static bool Prj_CheckIfICanCreateProjects (void);
static void Prj_PutIconsListProjects (void);
static void Prj_PutIconToCreateNewPrj (void);
static void Prj_PutButtonToCreateNewPrj (void);
static void Prj_PutIconToShowAllData (void);
static void Prj_ShowOneProject (struct Project *Prj,Prj_ProjectView_t ProjectView);
static void Prj_ShowTableAllProjectsOneRow (struct Project *Prj);
static void Prj_ShowOneProjectDepartment (const struct Project *Prj,
                                          Prj_ProjectView_t ProjectView);
static void Prj_ShowTableAllProjectsDepartment (const struct Project *Prj);
static void Prj_ShowOneProjectTxtField (struct Project *Prj,
                                        Prj_ProjectView_t ProjectView,
                                        const char *Label,char *TxtField);
static void Prj_ShowTableAllProjectsTxtField (struct Project *Prj,
                                              char *TxtField);
static void Prj_ShowOneProjectUsrs (const struct Project *Prj,
                                    Prj_ProjectView_t ProjectView,
                                    const char *Label,Prj_RoleInProject_t RoleInProject);
static void Prj_ShowTableAllProjectsUsrs (const struct Project *Prj,
                                          Prj_RoleInProject_t RoleInProject);
static void Prj_WriteUsrs (long PrjCod,Prj_ProjectView_t ProjectView,
                           Prj_RoleInProject_t RoleInProject);

static void Prj_ReqAnotherUsrID (Prj_RoleInProject_t RoleInProject);
static void Prj_AddUsrToProject (Prj_RoleInProject_t RoleInProject);
static void Prj_ReqRemUsrFromPrj (Prj_RoleInProject_t RoleInProject);
static void Prj_RemUsrFromPrj (Prj_RoleInProject_t RoleInProject);
static bool Prj_CheckIfICanRemUsrFromPrj (void);

static void Prj_GetParamPrjOrder (void);

static void Prj_PutFormsToRemEditOnePrj (long PrjCod,bool Hidden);
static void Prj_PutParams (void);
static void Prj_GetDataOfProject (struct Project *Prj,const char *Query);
static void Prj_ResetProject (struct Project *Prj);
static void Prj_PutParamPrjCod (long PrjCod);

static void Prj_RequestCreatOrEditPrj (long PrjCod);
static void Prj_EditOneProjectTxtArea (const char *Id,
                                       const char *Label,char *TxtField,
                                       unsigned NumRows);

static void Prj_AllocMemProject (struct Project *Prj);
static void Prj_FreeMemProject (struct Project *Prj);

static void Prj_CreateProject (struct Project *Prj);
static void Prj_UpdateProject (struct Project *Prj);

/*****************************************************************************/
/***************************** List all projects *****************************/
/*****************************************************************************/

void Prj_SeeProjects (void)
  {
   /***** Get parameters *****/
   Prj_GetParamPrjOrder ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);

   /***** Show all the projects *****/
   Prj_ShowAllProjects ();
  }

/*****************************************************************************/
/************************ Show all projects in a table ***********************/
/*****************************************************************************/

void Prj_ShowTableAllProjects (void)
  {
   extern const char *Txt_No_projects;
   unsigned NumPrj;
   struct Project Prj;

   /***** Get list of projects *****/
   Prj_GetListProjects ();

   if (Gbl.Prjs.Num)
     {
      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Prj);

      /***** Table head *****/
      Tbl_StartTableWide (2);

      /***** Write all the projects *****/
      for (NumPrj = 0;
	   NumPrj <= Gbl.Prjs.Num;
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
/***************************** Show the projects *****************************/
/*****************************************************************************/

static void Prj_ShowAllProjects (void)
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

   if (Gbl.Prjs.Num)
     {
      /***** Allocate memory for the project *****/
      Prj_AllocMemProject (&Prj);

      /***** Table head *****/
      Tbl_StartTableWideMargin (5);
      Prj_PutHeadForSeeing (false);	// Not print view

      /***** Write all the projects *****/
      for (NumPrj = Pagination.FirstItemVisible;
	   NumPrj <= Pagination.LastItemVisible;
	   NumPrj++)
	{
	 Prj.PrjCod = Gbl.Prjs.LstPrjCods[NumPrj - 1];
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
/******************* Write header with fields of a project *******************/
/*****************************************************************************/

static void Prj_PutHeadForSeeing (bool PrintView)
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

/*****************************************************************************/
/********************** Check if I can create projects ***********************/
/*****************************************************************************/

static bool Prj_CheckIfICanCreateProjects (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged == Rol_TCH ||
                  Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
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
/********************** Show print view of one project ***********************/
/*****************************************************************************/

void Prj_PrintOneProject (void)
  {
   struct Project Prj;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get the code of the project *****/
   Prj.PrjCod = Prj_GetParamPrjCod ();

   /***** Write header *****/
   Lay_WriteHeaderClassPhoto (true,false,
			      Gbl.CurrentIns.Ins.InsCod,
			      Gbl.CurrentDeg.Deg.DegCod,
			      Gbl.CurrentCrs.Crs.CrsCod);

   /***** Table head *****/
   Tbl_StartTableWideMargin (2);
   Prj_PutHeadForSeeing (true);	// Print view

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
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   extern const char *Txt_Preassigned_QUESTION;
   extern const char *Txt_PROJECT_ROLES_PLURAL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   Prj_RoleInProject_t RoleInProject;
   static unsigned UniqueId = 0;

   /***** Get data of this project *****/
   Prj_GetDataOfProjectByCod (Prj);

   /***** Write first row of data of this project *****/
   /* Forms to remove/edit this project */
   fprintf (Gbl.F.Out,"<tr>"
	              "<td rowspan=\"8\" class=\"CONTEXT_COL");
   if (ProjectView == Prj_LIST_PROJECTS)
     {
      fprintf (Gbl.F.Out," COLOR%u\">",Gbl.RowEvenOdd);
      Prj_PutFormsToRemEditOnePrj (Prj->PrjCod,Prj->Hidden);
     }
   else
      fprintf (Gbl.F.Out,"\">");
   fprintf (Gbl.F.Out,"</td>");

   /* Start date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"asg_date_start_%u\" class=\"%s LEFT_BOTTOM",
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
   fprintf (Gbl.F.Out,"<td id=\"asg_date_end_%u\" class=\"%s LEFT_BOTTOM",
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
                      "%u,'<br />','%s',false,true,0x7);"
                      "</script>"
	              "</td>",
            UniqueId,Prj->TimeUTC[Dat_END_TIME],
            (unsigned) Gbl.Prefs.DateFormat,Txt_Today);

   /* Project title */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP");
   if (ProjectView == Prj_LIST_PROJECTS)
      fprintf (Gbl.F.Out," COLOR%u",Gbl.RowEvenOdd);
   fprintf (Gbl.F.Out,"\">"
                      "<div class=\"%s\">%s</div>",
            Prj->Hidden ? "ASG_TITLE_LIGHT" :
        	          "ASG_TITLE",
            Prj->Title);
   fprintf (Gbl.F.Out,"</td>");

   /* Department */
   Prj_ShowOneProjectDepartment (Prj,ProjectView);

   /***** Write rows of data of this project *****/
   /* Description of the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,
                               Txt_Description,Prj->Description);

   /* Required knowledge to carry out the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,
                               Txt_Required_knowledge,Prj->Knowledge);

   /* Required materials to carry out the project */
   Prj_ShowOneProjectTxtField (Prj,ProjectView,
                               Txt_Required_materials,Prj->Materials);

   /* Preassigned? */
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

   /* Project members */
   for (RoleInProject = Prj_ROLE_STD;
	RoleInProject <= Prj_ROLE_EVA;
	RoleInProject++)
      Prj_ShowOneProjectUsrs (Prj,ProjectView,
                              Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject],RoleInProject);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/***************** Show one row of table with all projects *******************/
/*****************************************************************************/

static void Prj_ShowTableAllProjectsOneRow (struct Project *Prj)
  {
   extern const char *Txt_Today;
   extern const char *Txt_Yes;
   extern const char *Txt_No;
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   extern const char *Txt_Preassigned_QUESTION;
   extern const char *Txt_PROJECT_ROLES_PLURAL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   Prj_RoleInProject_t RoleInProject;
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
                      "%u,'<br />','%s',false,true,0x7);"
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

   /* Department */
   Prj_ShowTableAllProjectsDepartment (Prj);

   /***** Write rows of data of this project *****/
   /* Description of the project */
   Prj_ShowTableAllProjectsTxtField (Prj,Prj->Description);

   /* Required knowledge to carry out the project */
   Prj_ShowTableAllProjectsTxtField (Prj,Prj->Knowledge);

   /* Required materials to carry out the project */
   Prj_ShowTableAllProjectsTxtField (Prj,Prj->Materials);

   /* Preassigned? */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            (Prj->Preassigned == Prj_PREASSIGNED) ? Txt_Yes :
        	                                    Txt_No);

   /* Project members */
   for (RoleInProject = Prj_ROLE_STD;
	RoleInProject <= Prj_ROLE_EVA;
	RoleInProject++)
      Prj_ShowTableAllProjectsUsrs (Prj,RoleInProject);

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
                                        const char *Label,char *TxtField)
  {
   /***** Change format *****/
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     TxtField,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (TxtField,Cns_MAX_BYTES_TEXT,60);		// Insert links

   /***** Write row with label and text *****/
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
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"LEFT_TOP COLOR%u %s\">"
                      "%s"
                      "</td>",
            Gbl.RowEvenOdd,
            Prj->Hidden ? "DAT_LIGHT" :
        	          "DAT",
            TxtField);
  }

/*****************************************************************************/
/************************* Show users row in a project ***********************/
/*****************************************************************************/

static void Prj_ShowOneProjectUsrs (const struct Project *Prj,
                                    Prj_ProjectView_t ProjectView,
                                    const char *Label,Prj_RoleInProject_t RoleInProject)
  {
   /***** Row with label and listing of users *****/
   fprintf (Gbl.F.Out,"<tr>");
   switch (ProjectView)
     {
      case Prj_LIST_PROJECTS:
         fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"RIGHT_TOP COLOR%u %s\">%s:</td>"
                            "<td colspan=\"2\" class=\"LEFT_TOP COLOR%u %s\">",
	          Gbl.RowEvenOdd,
                  Prj->Hidden ? "ASG_LABEL_LIGHT" :
        	                "ASG_LABEL",
                  Label,
	          Gbl.RowEvenOdd,
        	  Prj->Hidden ? "DAT_LIGHT" :
        	                "DAT");
         break;
      case Prj_PRINT_ONE_PROJECT:
         fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"RIGHT_TOP %s\">%s:</td>"
                            "<td colspan=\"2\" class=\"LEFT_TOP %s\">",
                  Prj->Hidden ? "ASG_LABEL_LIGHT" :
        	                "ASG_LABEL",
                  Label,
        	  Prj->Hidden ? "DAT_LIGHT" :
        	                "DAT");
         break;
      case Prj_EDIT_ONE_PROJECT:
         fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP ASG_LABEL\">%s:</td>"
                            "<td colspan=\"2\" class=\"LEFT_TOP DAT\">",
                  Label);
         break;
     }
   Prj_WriteUsrs (Prj->PrjCod,ProjectView,RoleInProject);
   fprintf (Gbl.F.Out,"</td>"
                      "</tr>");
  }

static void Prj_ShowTableAllProjectsUsrs (const struct Project *Prj,
                                          Prj_RoleInProject_t RoleInProject)
  {
   /***** Listing of users *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u %s\">",
	    Gbl.RowEvenOdd,
	    Prj->Hidden ? "DAT_LIGHT" :
			  "DAT");
   Prj_WriteUsrs (Prj->PrjCod,Prj_LIST_PROJECTS,RoleInProject);
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/*************** Write list of users with a role in a project ****************/
/*****************************************************************************/

static void Prj_WriteUsrs (long PrjCod,Prj_ProjectView_t ProjectView,
                           Prj_RoleInProject_t RoleInProject)
  {
   extern const char *Txt_Remove;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_ROLES_PLURAL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   extern const char *Txt_Add_USER;
   extern const char *Txt_PROJECT_ROLES_SINGUL_abc[Prj_NUM_ROLES_IN_PROJECT];
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr;
   unsigned NumUsrs;
   unsigned NumUsrsKnown;
   unsigned NumUsrsUnknown;
   unsigned NumUsrsToShow;
   bool UsrValid;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];
   static Act_Action_t ActionReqRemUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActReqRemStdPrj,	// Prj_ROLE_STD, Student
      ActReqRemTutPrj,	// Prj_ROLE_TUT, Tutor
      ActReqRemEvaPrj,	// Prj_ROLE_EVA, Evaluator
     };
   static Act_Action_t ActionReqAddUsr[Prj_NUM_ROLES_IN_PROJECT] =
     {
      ActUnk,		// Prj_ROLE_UNK, Unknown
      ActReqAddStdPrj,	// Prj_ROLE_STD, Student
      ActReqAddTutPrj,	// Prj_ROLE_TUT, Tutor
      ActReqAddEvaPrj,	// Prj_ROLE_EVA, Evaluator
     };

   /***** Get number of users in project from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM prj_usr"
	          " WHERE PrjCod=%ld AND RoleInProject=%u",
            PrjCod,(unsigned) RoleInProject);
   NumUsrs = (unsigned) DB_QueryCOUNT (Query,"can not get users in project");

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
   NumUsrsKnown = (unsigned) DB_QuerySELECT (Query,&mysql_res,
                                             "can not get users in project");

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table>");

   /***** Check number of users *****/
   if (NumUsrs)
     {
      /***** How many users will be shown? *****/
      NumUsrsToShow = NumUsrsKnown;

      /***** Write known users *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrsToShow;
	   NumUsr++)
        {
         /* Get user's code */
         row = mysql_fetch_row (mysql_res);
         Gbl.Usrs.Other.UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get user's data */
	 UsrValid = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat);

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
         ShowPhoto = (UsrValid ? Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Other.UsrDat,PhotoURL) :
                                 false);
         Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,ShowPhoto ? PhotoURL :
                        	                              NULL,
                           "PHOTO21x28",Pho_ZOOM,false);

         /* Write user's name */
         fprintf (Gbl.F.Out,"</td>"
                            "<td class=\"AUTHOR_TXT LEFT_MIDDLE\">");
         if (UsrValid)
            fprintf (Gbl.F.Out,"%s",Gbl.Usrs.Other.UsrDat.FullName);
         else
            fprintf (Gbl.F.Out,"[%s]",
                     Txt_ROLES_SINGUL_abc[Rol_UNK][Usr_SEX_UNKNOWN]);	// User not found, likely a user who has been removed
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      /***** If any users are unknown *****/
      if ((NumUsrsUnknown = NumUsrs - NumUsrsKnown))
         /***** Start form to show all the users *****/
         fprintf (Gbl.F.Out,"<tr>"
                            "<td colspan=\"3\" class=\"AUTHOR_TXT LEFT_MIDDLE\">"
                            "[%u %s]"
                            "</td>"
                            "</tr>",
                  NumUsrsUnknown,
                  (NumUsrsUnknown == 1) ?
                  Txt_ROLES_SINGUL_abc[Rol_UNK][Usr_SEX_UNKNOWN] :
                  Txt_ROLES_PLURAL_abc[Rol_UNK][Usr_SEX_UNKNOWN]);
     }

   /***** Row to add a new user *****/
   if (ProjectView == Prj_EDIT_ONE_PROJECT)
     {
      fprintf (Gbl.F.Out,"<tr>"
			 "<td colspan=\"3\" class=\"LEFT_MIDDLE\">");
      Gbl.Prjs.PrjCodToEdit = PrjCod;	// Used to pass project code as a parameter
      sprintf (Gbl.Title,Txt_Add_USER,Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject]);
      Lay_PutContextualLink (ActionReqAddUsr[RoleInProject],NULL,Prj_PutParams,
			     "plus64x64.png",
			     Gbl.Title,Gbl.Title,
			     NULL);
      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
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
   extern const char *Txt_THE_USER_X_already_exists_in_Y_but_is_not_yet_enroled_in_the_course_Z;
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

	 /* Check if this user belongs to the current course */
	 if (Usr_CheckIfUsrBelongsToCurrentCrs (&Gbl.Usrs.Other.UsrDat))
	   {
	    /***** Add user to project *****/
	    sprintf (Query,"REPLACE INTO prj_usr"
			   " (PrjCod,RoleInProject,UsrCod)"
			   " VALUES"
			   " (%ld,%u,%ld)",
		     PrjCod,(unsigned) RoleInProject,Gbl.Usrs.Other.UsrDat.UsrCod);
	    DB_QueryREPLACE (Query,"can not add user to project");

	    sprintf (Gbl.Alert.Txt,Txt_THE_USER_X_has_been_enroled_as_a_Y_in_the_project,
		     Gbl.Usrs.Other.UsrDat.FullName,
		     Txt_PROJECT_ROLES_SINGUL_abc[RoleInProject]);
	    Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
	   }
	 else        // User does not belong to the current course
	   {
	    sprintf (Gbl.Alert.Txt,Txt_THE_USER_X_already_exists_in_Y_but_is_not_yet_enroled_in_the_course_Z,
		     Gbl.Usrs.Other.UsrDat.FullName,Cfg_PLATFORM_SHORT_NAME,Gbl.CurrentCrs.Crs.FullName);
	    Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
	   }
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
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanRemUsrFromPrj ())
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
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Get user to be removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Prj_CheckIfICanRemUsrFromPrj ())
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
/*********** Check if I can remove another user in current course ************/
/*****************************************************************************/

static bool Prj_CheckIfICanRemUsrFromPrj (void)
  {
   return true;	// TODO: Rewrite this function
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

static void Prj_PutFormsToRemEditOnePrj (long PrjCod,bool Hidden)
  {
   Gbl.Prjs.PrjCodToEdit = PrjCod;	// Used as parameter in contextual links

   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
      case Rol_SYS_ADM:
	 /***** Put form to remove project *****/
	 Ico_PutContextualIconToRemove (ActReqRemPrj,Prj_PutParams);

	 /***** Put form to hide/show project *****/
	 if (Hidden)
	    Ico_PutContextualIconToUnhide (ActShoPrj,Prj_PutParams);
	 else
	    Ico_PutContextualIconToHide (ActHidPrj,Prj_PutParams);

	 /***** Put form to edit project *****/
	 Ico_PutContextualIconToEdit (ActEdiOnePrj,Prj_PutParams);
	 // no break
      case Rol_STD:
      case Rol_NET:
	 /***** Put form to print project *****/
	 Ico_PutContextualIconToPrint (ActPrnOnePrj,Prj_PutParams);
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/********************** Params used to edit a project ************************/
/*****************************************************************************/

static void Prj_PutParams (void)
  {
   if (Gbl.Prjs.PrjCodToEdit > 0)
      Prj_PutParamPrjCod (Gbl.Prjs.PrjCodToEdit);
   Prj_PutHiddenParamPrjOrder ();
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
   switch (Gbl.Prjs.SelectedOrder)
     {
      case Prj_ORDER_START_TIME:
      case Prj_ORDER_END_TIME:
      case Prj_ORDER_TITLE:
	 /* Hidden subquery */
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:
	    case Rol_SYS_ADM:
	       HiddenSubQuery[0] = '\0';
	       break;
	    default:
	       sprintf (HiddenSubQuery," AND Hidden='N'");
	       break;
	   }

	 /* Order subquery */
	 switch (Gbl.Prjs.SelectedOrder)
	   {
	    case Prj_ORDER_START_TIME:
	       sprintf (OrderBySubQuery,"StartTime DESC,"
		                        "EndTime DESC,"
		                        "Title");
	       break;
	    case Prj_ORDER_END_TIME:
	       sprintf (OrderBySubQuery,"EndTime DESC,"
		                        "StartTime DESC,"
		                        "Title");
	       break;
	    case Prj_ORDER_TITLE:
	       sprintf (OrderBySubQuery,"Title,"
		                        "StartTime DESC,"
		                        "EndTime DESC");
	       break;
            case Prj_ORDER_DEPARTMENT:	// Not applicable
	       break;
	   }

	 /* Query */
	 sprintf (Query,"SELECT PrjCod"
	                " FROM projects"
	                " WHERE CrsCod=%ld%s"
	                " ORDER BY %s",
	          Gbl.CurrentCrs.Crs.CrsCod,HiddenSubQuery,
	          OrderBySubQuery);
         break;
      case Prj_ORDER_DEPARTMENT:
	 /* Hidden subquery */
	 switch (Gbl.Usrs.Me.Role.Logged)
	   {
	    case Rol_TCH:
	    case Rol_SYS_ADM:
	       HiddenSubQuery[0] = '\0';
	       break;
	    default:
	       sprintf (HiddenSubQuery," AND projects.Hidden='N'");
	       break;
	   }

	 /* Order subquery */
         sprintf (OrderBySubQuery,"departments.FullName,"
                                  "projects.StartTime DESC,"
                                  "projects.EndTime DESC,"
                                  "projects.Title");

	 /* Query */
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
/********************* Get project data using its code ***********************/
/*****************************************************************************/

void Prj_GetDataOfProjectByCod (struct Project *Prj)
  {
   char Query[1024];

   if (Prj->PrjCod > 0)
     {
      /***** Build query *****/
      sprintf (Query,"SELECT PrjCod,DptCod,Hidden,Preassigned,"
		     "UNIX_TIMESTAMP(StartTime),"
		     "UNIX_TIMESTAMP(EndTime),"
		     "NOW() BETWEEN StartTime AND EndTime,"
		     "Title,Description,Knowledge,Materials,URL"
		     " FROM projects"
		     " WHERE PrjCod=%ld AND CrsCod=%ld",
	       Prj->PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
      /*
      row[ 0]: PrjCod
      row[ 1]: DptCod
      row[ 2]: Hidden
      row[ 3]: Preassigned
      row[ 4]: UNIX_TIMESTAMP(StartTime)
      row[ 5]: UNIX_TIMESTAMP(EndTime)
      row[ 6]: NOW() BETWEEN StartTime AND EndTime
      row[ 7]: Title
      row[ 8]: Description
      row[ 9]: Knowledge
      row[10]: Materials
      row[11]: URL
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

   /***** Clear all project data *****/
   Prj_ResetProject (Prj);

   /***** Get data of project from database *****/
   if (DB_QuerySELECT (Query,&mysql_res,"can not get project data")) // Project found...
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);
      /*
      row[ 0]: PrjCod
      row[ 1]: DptCod
      row[ 2]: Hidden
      row[ 3]: Preassigned
      row[ 4]: UNIX_TIMESTAMP(StartTime)
      row[ 5]: UNIX_TIMESTAMP(EndTime)
      row[ 6]: NOW() BETWEEN StartTime AND EndTime
      row[ 7]: Title
      row[ 8]: Description
      row[ 9]: Knowledge
      row[10]: Materials
      row[11]: URL
      */

      /* Get code of the project (row[0]) */
      Prj->PrjCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get code of the department (row[1]) */
      Prj->DptCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get whether the project is hidden or not (row[2]) */
      Prj->Hidden = (row[2][0] == 'Y');

      /* Get the folder for the project files (row[3]) */
      Prj->Preassigned = (row[3][0] == 'Y') ? Prj_PREASSIGNED :
	                                      Prj_NOT_PREASSIGNED;

      /* Get start date (row[4] holds the start UTC time) */
      Prj->TimeUTC[Dat_START_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get end date   (row[5] holds the end   UTC time) */
      Prj->TimeUTC[Dat_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[5]);

      /* Get whether the project is open or closed (row[6]) */
      Prj->Open = (row[6][0] == '1');

      /* Get the title of the project (row[7]) */
      Str_Copy (Prj->Title,row[7],
                Prj_MAX_BYTES_PROJECT_TITLE);

      /* Get the description of the project (row[8]) */
      Str_Copy (Prj->Description,row[8],
                Cns_MAX_BYTES_TEXT);

      /* Get the required knowledge for the project (row[9]) */
      Str_Copy (Prj->Knowledge,row[9],
                Cns_MAX_BYTES_TEXT);

      /* Get the required materials for the project (row[10]) */
      Str_Copy (Prj->Materials,row[10],
                Cns_MAX_BYTES_TEXT);

      /* Get the URL of the project (row[11]) */
      Str_Copy (Prj->URL,row[11],
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
   Prj->Hidden = false;
   Prj->Preassigned = Prj_NOT_PREASSIGNED;
   Prj->TimeUTC[Dat_START_TIME] =
   Prj->TimeUTC[Dat_END_TIME  ] = (time_t) 0;
   Prj->Open = false;
   Prj->Title[0] = '\0';
   Prj->DptCod = -1L;	// Unknown department
   Prj->Description[0] = '\0';
   Prj->Knowledge[0] = '\0';
   Prj->Materials[0] = '\0';
   Prj->URL[0] = '\0';
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

static void Prj_PutParamPrjCod (long PrjCod)
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
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Show question and button to remove the project *****/
   Gbl.Prjs.PrjCodToEdit = Prj.PrjCod;
   sprintf (Gbl.Alert.Txt,Txt_Do_you_really_want_to_remove_the_project_X,
            Prj.Title);
   Ale_ShowAlertAndButton (Ale_QUESTION,Gbl.Alert.Txt,
                           ActRemPrj,NULL,NULL,Prj_PutParams,
                           Btn_REMOVE_BUTTON,Txt_Remove_project);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_SeeProjects ();
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
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);	// Inside this function, the course is checked to be the current one

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

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_SeeProjects ();
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

   /***** Get project code *****/
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Hide project *****/
   sprintf (Query,"UPDATE projects SET Hidden='Y'"
                  " WHERE PrjCod=%ld AND CrsCod=%ld",
            Prj.PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not hide project");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Project_X_is_now_hidden,
            Prj.Title);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_SeeProjects ();
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

   /***** Get project code *****/
   if ((Prj.PrjCod = Prj_GetParamPrjCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of project is missing.");

   /***** Get data of the project from database *****/
   Prj_GetDataOfProjectByCod (&Prj);

   /***** Hide project *****/
   sprintf (Query,"UPDATE projects SET Hidden='N'"
                  " WHERE PrjCod=%ld AND CrsCod=%ld",
            Prj.PrjCod,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not show project");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Project_X_is_now_visible,
            Prj.Title);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show projects again *****/
   Prj_SeeProjects ();
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
   extern const char *Hlp_ASSESSMENT_Projects_new_project;
   extern const char *Hlp_ASSESSMENT_Projects_edit_project;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_project;
   extern const char *Txt_Edit_project;
   extern const char *Txt_Project_data;
   extern const char *Txt_Title;
   extern const char *Txt_Department;
   extern const char *Txt_Description;
   extern const char *Txt_Required_knowledge;
   extern const char *Txt_Required_materials;
   extern const char *Txt_URL;
   extern const char *Txt_Preassigned_QUESTION;
   extern const char *Txt_No;
   extern const char *Txt_Yes;
   extern const char *Txt_Create_project;
   extern const char *Txt_Save;
   extern const char *Txt_Project_members;
   extern const char *Txt_PROJECT_ROLES_PLURAL_Abc[Prj_NUM_ROLES_IN_PROJECT];
   struct Project Prj;
   bool ItsANewProject;
   Prj_RoleInProject_t RoleInProject;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters *****/
   Prj_GetParamPrjOrder ();
   Gbl.Prjs.CurrentPage = Pag_GetParamPagNum (Pag_PROJECTS);

   /***** Get the code of the project *****/
   ItsANewProject = ((Prj.PrjCod = PrjCod) == -1L);

   /***** Get from the database the data of the project *****/
   if (ItsANewProject)
     {
      /* Initialize to empty project */
      Prj_ResetProject (&Prj);
      Prj.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
      Prj.TimeUTC[Dat_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
      Prj.Open = true;
     }
   else
      /* Get data of the project from database */
      Prj_GetDataOfProjectByCod (&Prj);

   /***** Start box and form *****/
   if (ItsANewProject)
     {
      Box_StartBox (NULL,Txt_New_project,NULL,
                    Hlp_ASSESSMENT_Projects_new_project,Box_NOT_CLOSABLE);
      Act_FormStart (ActNewPrj);
      Gbl.Prjs.PrjCodToEdit = -1L;
     }
   else
     {
      Box_StartBox (NULL,Txt_Edit_project,NULL,
                    Hlp_ASSESSMENT_Projects_edit_project,Box_NOT_CLOSABLE);
      Act_FormStart (ActChgPrj);
      Gbl.Prjs.PrjCodToEdit = Prj.PrjCod;
     }
   Prj_PutParams ();

   /***** 1. Project data *****/
   /* Start box and table */
   Box_StartBoxTable (NULL,Txt_Project_data,NULL,
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
            Prj_MAX_CHARS_PROJECT_TITLE,Prj.Title);

   /* Project start and end dates */
   Dat_PutFormStartEndClientLocalDateTimes (Prj.TimeUTC,Dat_FORM_SECONDS_ON);

   /* Department */
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"DptCod\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Department);
   Dpt_WriteSelectorDepartment (Gbl.CurrentIns.Ins.InsCod,
                                Prj.DptCod,	// Selected department
                                false);			// Don't submit on change
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   /* Description of the project */
   Prj_EditOneProjectTxtArea ("Description",Txt_Description,
                              Prj.Description,12);

   /* Required knowledge to carry out the project */
   Prj_EditOneProjectTxtArea ("Knowledge",Txt_Required_knowledge,
                              Prj.Knowledge,4);

   /* Required materials to carry out the project */
   Prj_EditOneProjectTxtArea ("Materials",Txt_Required_materials,
                              Prj.Materials,4);

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
	    Cns_MAX_CHARS_WWW,Prj.URL);

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
   if (Prj.Preassigned == Prj_NOT_PREASSIGNED)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_No);

   fprintf (Gbl.F.Out,"<option value=\"Y\"");
   if (Prj.Preassigned == Prj_PREASSIGNED)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_Yes);

   fprintf (Gbl.F.Out,"</select>"
	              "</td>"
                      "</tr>");

   /* End table, send button and end box */
   if (ItsANewProject)
      Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_project);
   else
      Box_EndBoxTableWithButton (Btn_CONFIRM_BUTTON,Txt_Save);

   /* End form */
   Act_FormEnd ();

   /***** Project members *****/
   if (!ItsANewProject)	// Existing project
     {
      Box_StartBoxTable (NULL,Txt_Project_members,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      for (RoleInProject = Prj_ROLE_STD;
	   RoleInProject <= Prj_ROLE_EVA;
	   RoleInProject++)
	 Prj_ShowOneProjectUsrs (&Prj,Prj_EDIT_ONE_PROJECT,
				 Txt_PROJECT_ROLES_PLURAL_Abc[RoleInProject],RoleInProject);
      Box_EndBoxTable ();
     }

   /***** End box *****/
   Box_EndBox ();

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);

   /***** Show current projects, if any *****/
   Prj_ShowAllProjects ();
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

static void Prj_AllocMemProject (struct Project *Prj)
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

static void Prj_FreeMemProject (struct Project *Prj)
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
   bool NewProjectIsCorrect = true;

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Prj);

   /***** Get parameters from form *****/
   /* Get the code of the project */
   Prj.PrjCod = Prj_GetParamPrjCod ();
   ItsANewProject = (Prj.PrjCod < 0);

   if (ItsANewProject)
      /* Reset project data */
      Prj_ResetProject (&Prj);
   else
      /* Get data of the project from database */
      Prj_GetDataOfProjectByCod (&Prj);

   /* Get start/end date-times */
   Prj.TimeUTC[Dat_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   Prj.TimeUTC[Dat_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /* Get project title */
   Par_GetParToText ("Title",Prj.Title,Prj_MAX_BYTES_PROJECT_TITLE);

   /* Get department */
   Prj.DptCod = Par_GetParToLong ("DptCod");

   /* Get project description, required knowledge and required materials */
   Par_GetParToHTML ("Description",Prj.Description,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
   Par_GetParToHTML ("Knowledge"  ,Prj.Knowledge  ,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)
   Par_GetParToHTML ("Materials"  ,Prj.Materials  ,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /* Get degree WWW */
   Par_GetParToText ("URL",Prj.URL,Cns_MAX_BYTES_WWW);

   /* Get whether the project is preassigned */
   Prj.Preassigned = (Par_GetParToBool ("Preassigned")) ? Prj_PREASSIGNED :
	                                                  Prj_NOT_PREASSIGNED;

   /***** Adjust dates *****/
   if (Prj.TimeUTC[Dat_START_TIME] == 0)
      Prj.TimeUTC[Dat_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (Prj.TimeUTC[Dat_END_TIME] == 0)
      Prj.TimeUTC[Dat_END_TIME] = Prj.TimeUTC[Dat_START_TIME] + 2 * 60 * 60;	// +2 hours

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

      /***** Show projects again *****/
      Prj_SeeProjects ();
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Prj_RequestCreatOrEditPrj (Prj.PrjCod);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Prj);
  }

/*****************************************************************************/
/************************** Create a new project *****************************/
/*****************************************************************************/

static void Prj_CreateProject (struct Project *Prj)
  {
   char Query[1024 +
              Prj_MAX_BYTES_PROJECT_TITLE +
              Cns_MAX_BYTES_TEXT*3 +
              Cns_MAX_BYTES_WWW];

   /***** Create a new project *****/
   sprintf (Query,"INSERT INTO projects"
	          " (CrsCod,DptCod,Hidden,Preassigned,StartTime,EndTime,"
	          "Title,Description,Knowledge,Materials,URL)"
                  " VALUES"
                  " (%ld,%ld,'%c','%c',FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
                  "'%s','%s','%s','%s','%s')",
            Gbl.CurrentCrs.Crs.CrsCod,
            Prj->PrjCod,
            Prj->Hidden ? 'Y' :
        	          'N',
            Prj->Preassigned == Prj_PREASSIGNED ? 'Y' :
        	                                  'N',
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
	          "DptCod=%ld,Hidden='%c',Preassigned='%c',"
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
/******************** Remove all the projects of a course ********************/
/*****************************************************************************/

void Prj_RemoveCrsProjects (long CrsCod)
  {
   char Query[512];

   /***** Remove projects *****/
   sprintf (Query,"DELETE FROM projects WHERE CrsCod=%ld",CrsCod);
   DB_QueryDELETE (Query,"can not remove all the projects of a course");
  }

/*****************************************************************************/
/******************** Get number of projects in a course *********************/
/*****************************************************************************/

unsigned Prj_GetNumProjectsInCrs (long CrsCod)
  {
   char Query[256];

   /***** Get number of projects in a course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM projects WHERE CrsCod=%ld",
            CrsCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of projects in course");
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
