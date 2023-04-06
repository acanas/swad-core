// swad_project_resource.c: links to projects as program resources

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

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_error.h"
#include "swad_parameter_code.h"
#include "swad_program_database.h"
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
   struct Prj_Projects Projects;
   char Title[Prj_MAX_BYTES_TITLE + 1];

   /***** Reset projects *****/
   Prj_ResetProjects (&Projects);

   /***** Get parameters *****/
   Prj_GetPars (&Projects);
   Projects.Prj.PrjCod = ParCod_GetPar (ParCod_Prj);

   /***** Get project title *****/
   PrjRsc_GetTitleFromPrjCod (Projects.Prj.PrjCod,Title,sizeof (Title) - 1);

   /***** Copy link to PROJECT into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_PROJECT,Projects.Prj.PrjCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show project and (if possible) its file browser *****/
   Prj_ShowOneProjectWithFileBrowser (&Projects);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/******************* Get project title from project code *********************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void PrjRsc_GetTitleFromPrjCod (long PrjCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Projects;

   if (PrjCod > 0)
      /***** Get project title from database *****/
      Prj_DB_GetProjectTitle (PrjCod,Title,TitleSize);
   else
      /***** Generic title for all projects *****/
      Str_Copy (Title,Txt_Projects,TitleSize);
  }
