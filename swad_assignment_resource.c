// swad_assignment_resource.c: links to assignments as program resources

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
#include "swad_assignment.h"
#include "swad_assignment_database.h"
#include "swad_assignment_resource.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_pagination.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/************************** Get link to assignment ***************************/
/*****************************************************************************/

void AsgRsc_GetLinkToAssignment (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Asg_Assignments Assignments;
   char Title[Asg_MAX_BYTES_ASSIGNMENT_TITLE + 1];

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Grp_GetParWhichGroups ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   Assignments.Asg.AsgCod = ParCod_GetPar (ParCod_Asg);

   /***** Get assignment title *****/
   AsgRsc_GetTitleFromAsgCod (Assignments.Asg.AsgCod,Title,sizeof (Title) - 1);

   /***** Copy link to assignment into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_ASSIGNMENT,Assignments.Asg.AsgCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Title);

   /***** Show selected assignment in a box *****/
   if (Assignments.Asg.AsgCod > 0)
      Asg_ShowOneAssignmentInBox (&Assignments);

   /***** Show current assignments, if any *****/
   Asg_ShowAllAssignments (&Assignments);
  }

/*****************************************************************************/
/************************ Write assignment as resource ***********************/
/*****************************************************************************/

void AsgRsc_WriteResourceAssignment (long AsgCod,Frm_PutFormToGo_t PutFormToGo,
                                     const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   Act_Action_t NextAction;
   char Title[Asg_MAX_BYTES_ASSIGNMENT_TITLE + 1];

   /***** Get assignment title *****/
   AsgRsc_GetTitleFromAsgCod (AsgCod,Title,sizeof (Title) - 1);

   /***** Begin form to go to assignment *****/
   if (PutFormToGo == Frm_PUT_FORM_TO_GO)
     {
      NextAction = (AsgCod > 0)	? ActSeeOneAsg :	// Assignment specified
				  ActSeeAsg;		// All assignments
      Frm_BeginForm (NextAction);
         ParCod_PutPar (ParCod_Asg,AsgCod);
         HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   switch (PutFormToGo)
     {
      case Frm_DONT_PUT_FORM_TO_GO:
         Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);
	 break;
      case Frm_PUT_FORM_TO_GO:
         Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
	 break;
     }

   /***** Write assignment title of exam *****/
   HTM_Txt (Title);

   /***** End form to download file *****/
   if (PutFormToGo == Frm_PUT_FORM_TO_GO)
     {
         HTM_BUTTON_End ();
      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/**************** Get assignment title from assignment code ******************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void AsgRsc_GetTitleFromAsgCod (long AsgCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Assignments;
   char TitleFromDB[Asg_MAX_BYTES_ASSIGNMENT_TITLE + 1];

   if (AsgCod > 0)
     {
      /***** Get assignment title *****/
      Asg_DB_GetAssignmentTitleByCod (AsgCod,TitleFromDB);
      Str_Copy (Title,TitleFromDB,TitleSize);
     }
   else
      /***** Generic title for all assignments *****/
      Str_Copy (Title,Txt_Assignments,TitleSize);
  }
