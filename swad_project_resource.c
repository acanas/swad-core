// swad_project_resource.c: links to projects as program resources

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_parameter_code.h"
#include "swad_project.h"
#include "swad_project_database.h"
#include "swad_project_resource.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/***************************** Get link to game ******************************/
/*****************************************************************************/

void PrjRsc_GetLinkToProject (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Projects;
   struct Prj_Projects Projects;

   /***** Reset projects *****/
   Prj_ResetPrjsAndReadConfig (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects,Usr_USE_LIST_SELECTED_USERS);
   Projects.Prj.PrjCod = ParCod_GetPar (ParCod_Prj);

   /***** Allocate memory for the project *****/
   Prj_AllocMemProject (&Projects.Prj);

   /***** Get project data *****/
   Prj_GetProjectDataByCod (&Projects.Prj);

   /***** Copy link to PROJECT into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_PROJECT,Projects.Prj.PrjCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Projects.Prj.PrjCod > 0 ? Projects.Prj.Title :
   					    Txt_Projects);

   /***** The link may point to all projects or to a given project.
          It it points to a given project, show it. *****/
   if (Projects.Prj.PrjCod > 0)
      Prj_ShowBoxWithOneProject (&Projects);

   /***** Free memory of the project *****/
   Prj_FreeMemProject (&Projects.Prj);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }
