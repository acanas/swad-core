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
#include "swad_global.h"
#include "swad_pagination.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Get link to assignment ***************************/
/*****************************************************************************/

void AsgRsc_GetLinkToAssignment (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Assignments;
   struct Asg_Assignments Asgs;

   /***** Reset assignments *****/
   Asg_ResetAssignments (&Asgs);

   /***** Get parameters *****/
   Asgs.SelectedOrder	= Asg_GetParAsgOrder ();
   Asgs.MyAllGrps	= Grp_GetParMyAllGrps ();
   Asgs.ThisPage	= Pag_GetParPagNum (Pag_ASSIGNMENTS);

   /***** Get assignment code *****/
   Asgs.Asg.AsgCod = ParCod_GetPar (ParCod_Asg);

   /***** Get data of this assignment *****/
   Asg_GetAssignmentDataByCod (&Asgs.Asg);

   /***** Copy link to assignment into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_ASSIGNMENT,Asgs.Asg.AsgCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Asgs.Asg.AsgCod > 0 ? Asgs.Asg.Title :
   					Txt_Assignments);

   /***** Show current assignments, if any *****/
   Asg_ShowAllAssignments (&Asgs);
  }
