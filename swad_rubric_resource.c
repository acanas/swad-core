// swad_rubric_resource.c: links to rubrics as resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
#include "swad_parameter_code.h"
#include "swad_program_database.h"
#include "swad_rubric.h"
#include "swad_rubric_database.h"
#include "swad_rubric_resource.h"

/*****************************************************************************/
/**************************** Get link to rubric *****************************/
/*****************************************************************************/

void RubRsc_GetLinkToRubric (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Rub_Rubrics Rubrics;
   char Title[Rub_MAX_BYTES_TITLE + 1];

   /***** Reset rubrics context *****/
   Rub_ResetRubrics (&Rubrics);

   /***** Get parameters *****/
   Rub_GetPars (&Rubrics,true);

   /***** Get rubric title *****/
   RubRsc_GetTitleFromRubCod (Rubrics.Rubric.RubCod,Title,sizeof (Title) - 1);

   /***** Copy link to rubric into resource clipboard *****/
   Prg_DB_CopyToClipboard (Rsc_RUBRIC,Rubrics.Rubric.RubCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show rubrics again *****/
   Rub_ListAllRubrics (&Rubrics);
  }

/*****************************************************************************/
/*************************** Write rubric as resource ************************/
/*****************************************************************************/

void RubRsc_WriteResourceRubric (long RubCod,bool PutFormToGo,
                                 const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   Act_Action_t NextAction;
   char Title[Rub_MAX_BYTES_TITLE + 1];

   /***** Get rubric title *****/
   RubRsc_GetTitleFromRubCod (RubCod,Title,sizeof (Title) - 1);

   /***** Begin form to go to rubric *****/
   if (PutFormToGo)
     {
      NextAction = (RubCod > 0)	? ActSeeRub :	// Rubric specified
				  ActSeeAllRub;	// All rubrics
      Frm_BeginForm (NextAction);
         ParCod_PutPar (ParCod_Rub,RubCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write title of rubric *****/
   HTM_Txt (Title);

   /***** End form to download file *****/
   if (PutFormToGo)
     {
         HTM_BUTTON_End ();
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/********************* Get rubric title from rubric code *********************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void RubRsc_GetTitleFromRubCod (long RubCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Rubrics;
   char TitleFromDB[Rub_MAX_BYTES_TITLE + 1];

   if (RubCod > 0)
     {
      /***** Get rubric title *****/
      Rub_DB_GetRubricTitle (RubCod,TitleFromDB);
      Str_Copy (Title,TitleFromDB,TitleSize);
     }
   else
      /***** Generic title for all rubrics *****/
      Str_Copy (Title,Txt_Rubrics,TitleSize);
  }
