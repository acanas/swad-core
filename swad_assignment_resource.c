// swad_assignment_resource.c: links to assignments as program resources

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
   extern const char *Txt_Assignments;
   struct Asg_Assignments Assignments;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Assignments);

   /***** Get parameters *****/
   Assignments.SelectedOrder = Asg_GetParAsgOrder ();
   Grp_GetParMyAllGrps ();
   Assignments.CurrentPage = Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   Assignments.Asg.AsgCod = ParCod_GetPar (ParCod_Asg);

   /***** Get data of this assignment *****/
   if (Assignments.Asg.AsgCod > 0)
      Asg_GetAssignmentDataByCod (&Assignments.Asg);

   /***** Copy link to assignment into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_ASSIGNMENT,Assignments.Asg.AsgCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Assignments.Asg.AsgCod > 0 ? Assignments.Asg.Title :
   					       Txt_Assignments);

   /***** Show selected assignment in a box *****/
   if (Assignments.Asg.AsgCod > 0)
      Asg_ShowOneAssignmentInBox (&Assignments);

   /***** Show current assignments, if any *****/
   Asg_ShowAllAssignments (&Assignments);
  }
