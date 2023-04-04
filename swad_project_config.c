// swad_project_config.c: projects configuration
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

#include <stdbool.h>		// For boolean type

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_project.h"
#include "swad_project_config.h"
#include "swad_project_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void PrjCfg_ShowFormConfigEditableByNET (const struct Prj_Projects *Projects);

static void PrjCfg_GetCrsPrjsConfig (struct Prj_Projects *Projects);
static void PrjCfg_GetConfigDataFromRow (MYSQL_RES *mysql_res,
				         struct Prj_Projects *Projects);
static bool PrjCfg_GetEditableFromForm (void);

/*****************************************************************************/
/************************ Can I configure all projects? **********************/
/*****************************************************************************/

bool PrjCfg_CheckIfICanConfig (void)
  {
   static const bool ICanConfigAllProjects[Rol_NUM_ROLES] =
     {
      [Rol_TCH	  ] = true,
      [Rol_SYS_ADM] = true,
     };

   return ICanConfigAllProjects[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/********************** Configuration of all projects ************************/
/*****************************************************************************/

void PrjCfg_ShowFormConfig (void)
  {
   extern const char *Hlp_ASSESSMENT_Projects;
   extern const char *Txt_Configure_projects;
   extern const char *Txt_Save_changes;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Read projects configuration from database *****/
   PrjCfg_GetCrsPrjsConfig (&Projects);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Configure_projects,
                 Prj_PutIconsListProjects,&Projects,
                 Hlp_ASSESSMENT_Projects,Box_NOT_CLOSABLE);

      /***** Begin form *****/
      Frm_BeginForm (ActRcvCfgPrj);

	 HTM_TABLE_BeginCenterPadding (2);

	    /***** Projects are editable by non-editing teachers? *****/
	    PrjCfg_ShowFormConfigEditableByNET (&Projects);

	 HTM_TABLE_End ();

	 /***** Send button *****/
	 Btn_PutConfirmButton (Txt_Save_changes);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Projects are editable by non-editing teachers? ***************/
/*****************************************************************************/

static void PrjCfg_ShowFormConfigEditableByNET (const struct Prj_Projects *Projects)
  {
   extern const char *Txt_Editable;
   extern const char *Txt_Editable_by_non_editing_teachers;

   HTM_TR_Begin (NULL);

      /***** Label *****/
      Frm_LabelColumn ("RT","Editable",Txt_Editable);

      /***** Data *****/
      HTM_TD_Begin ("class=\"LT\"");
	 HTM_INPUT_CHECKBOX ("Editable",HTM_DONT_SUBMIT_ON_CHANGE,
			     "id=\"Editable\" value=\"Y\"%s",
			     Projects->Config.Editable ? " checked=\"checked\"" :
							 "");
	 HTM_Txt (Txt_Editable_by_non_editing_teachers);
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/************** Get configuration of projects for current course *************/
/*****************************************************************************/

static void PrjCfg_GetCrsPrjsConfig (struct Prj_Projects *Projects)
  {
   MYSQL_RES *mysql_res;

   /***** Get configuration of projects for current course from database *****/
   if (Prj_DB_GetCrsPrjsConfig (&mysql_res))
      PrjCfg_GetConfigDataFromRow (mysql_res,Projects);
   else
      Projects->Config.Editable = PrjCfg_EDITABLE_DEFAULT;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get configuration values from a database table row *************/
/*****************************************************************************/

static void PrjCfg_GetConfigDataFromRow (MYSQL_RES *mysql_res,
				         struct Prj_Projects *Projects)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get whether project are visible via plugins or not *****/
   Projects->Config.Editable = (row[0][0] == 'Y');
  }

/*****************************************************************************/
/************ Receive configuration of projects for current course ***********/
/*****************************************************************************/

void PrjCfg_ReceiveConfig (void)
  {
   extern const char *Txt_The_configuration_of_the_projects_has_been_updated;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get whether projects are editable by non-editing teachers or not *****/
   Projects.Config.Editable = PrjCfg_GetEditableFromForm ();

   /***** Update database *****/
   Prj_DB_UpdateCrsPrjsConfig (Projects.Config.Editable);

   /***** Show confirmation message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_configuration_of_the_projects_has_been_updated);

   /***** Show again the form to configure projects *****/
   PrjCfg_ShowFormConfig ();
  }

/*****************************************************************************/
/****** Get if projects are editable by non-editing teachers from form *******/
/*****************************************************************************/

static bool PrjCfg_GetEditableFromForm (void)
  {
   return Par_GetParBool ("Editable");
  }
