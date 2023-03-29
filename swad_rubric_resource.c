// swad_rubric_resource.c: links to rubrics as resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Ca�as Vargas

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
#include "swad_box.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"
#include "swad_rubric.h"
#include "swad_rubric_database.h"
#include "swad_rubric_resource.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void RubRsc_ShowClipboard (void);

static void RubRsc_PutIconsClipboard (__attribute__((unused)) void *Args);

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
   Rub_GetPars (&Rubrics,false);

   /***** Get rubric title *****/
   RubRsc_GetTitleFromRubCod (Rubrics.Rubric.RubCod,Title,sizeof (Title) - 1);

   /***** Copy link to rubric into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_RUBRIC,Rubrics.Rubric.RubCod);

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

/*****************************************************************************/
/******************** Show clipboard on top of rubrics ***********************/
/*****************************************************************************/

void RubRsc_ViewResourceClipboard (void)
  {
   /***** View resource clipboard *****/
   RubRsc_ShowClipboard ();

   /***** Show all rubrics *****/
   Rub_SeeAllRubrics ();
  }

/*****************************************************************************/
/************************* Show resources clipboard **************************/
/*****************************************************************************/

static void RubRsc_ShowClipboard (void)
  {
   extern const char *Hlp_ASSESSMENT_Rubrics;
   extern const char *Txt_Resource_clipboard;

   Box_BoxBegin (NULL,Txt_Resource_clipboard,
		 RubRsc_PutIconsClipboard,NULL,
		 Hlp_ASSESSMENT_Rubrics,Box_CLOSABLE);
      Rsc_ShowClipboard ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****** Put contextual icons when showing resource clipboard in rubrics ******/
/*****************************************************************************/

static void RubRsc_PutIconsClipboard (__attribute__((unused)) void *Args)
  {
   /***** Put icon to remove resource clipboard in rubrics *****/
   if (Rub_CheckIfICanEditRubrics ())
      if (Rsc_DB_GetNumResourcesInClipboard ())	// Only if there are resources
	 Ico_PutContextualIconToRemove (ActRemRscCli_InRub,NULL,
					NULL,NULL);
  }

/*****************************************************************************/
/******************* Remove clipboard and show program ***********************/
/*****************************************************************************/

void RubRsc_RemoveResourceClipboard (void)
  {
   extern const char *Txt_Resource_clipboard_removed;

   /***** Remove resource clipboard *****/
   Rsc_DB_RemoveClipboard ();
   Ale_ShowAlert (Ale_SUCCESS,Txt_Resource_clipboard_removed);

   /***** View resource clipboard again *****/
   RubRsc_ViewResourceClipboard ();
  }
