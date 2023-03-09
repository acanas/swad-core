// swad_project_resource.c: links to projects as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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
#include "swad_form.h"
#include "swad_parameter.h"
#include "swad_program_database.h"
#include "swad_project.h"
#include "swad_project_database.h"
#include "swad_project_resource.h"

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
   Projects.Prj.PrjCod = Par_GetAndCheckParCode (Par_PrjCod);

   /***** Get project title *****/
   PrjRsc_GetTitleFromPrjCod (Projects.Prj.PrjCod,Title,sizeof (Title) - 1);

   /***** Copy link to PROJECT into resource clipboard *****/
   Prg_DB_CopyToClipboard (PrgRsc_PROJECT,Projects.Prj.PrjCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show project and (if possible) its file browser *****/
   Prj_ShowOneProjectWithFileBrowser (&Projects);

   /***** Show projects again *****/
   Prj_ShowProjects (&Projects);
  }

/*****************************************************************************/
/*********************** Write game in course program ************************/
/*****************************************************************************/

void PrjRsc_WriteProjectInCrsProgram (long PrjCod,bool PutFormToGo,
                                      const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   Act_Action_t NextAction;
   char Title[Prj_MAX_BYTES_TITLE + 1];

   /***** Get project title *****/
   PrjRsc_GetTitleFromPrjCod (PrjCod,Title,sizeof (Title) - 1);

   /***** Begin form to go to project *****/
   if (PutFormToGo)
     {
      NextAction = (PrjCod > 0)	? ActSeeOnePrj :	// Project specified
				  ActSeePrj;		// All projects
      Frm_BeginForm (NextAction);
	 Par_PutParCode (Par_PrjCod,PrjCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
				  "class=\"LM BT_LINK PRG_LNK_%s\"",
				  The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write project title of exam *****/
   HTM_Txt (Title);

   /***** End form to go to project *****/
   if (PutFormToGo)
     {
      /* End form */
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/******************* Get project title from project code *********************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void PrjRsc_GetTitleFromPrjCod (long PrjCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Projects;
   char TitleFromDB[Prj_MAX_BYTES_TITLE + 1];

   if (PrjCod > 0)
     {
      /***** Get project title *****/
      Prj_DB_GetProjectTitle (PrjCod,TitleFromDB);
      Str_Copy (Title,TitleFromDB,TitleSize);
     }
   else
      /***** Generic title for all projects *****/
      Str_Copy (Title,Txt_Projects,TitleSize);
  }
