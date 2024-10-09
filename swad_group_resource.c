// swad_tag_resource.c: links to test tags as program resources

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_alert.h"
#include "swad_group.h"
#include "swad_group_database.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/**************************** Get link to groups *****************************/
/*****************************************************************************/

void GrpRsc_GetLinkToGroups (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Groups;

   /***** Copy link to tag into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_GROUPS,-1L);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  Txt_Groups);

   /***** Show my groups again *****/
   Grp_ShowLstGrpsToChgMyGrps ();
  }

/*****************************************************************************/
/************************** Get link to group type ***************************/
/*****************************************************************************/

void GrpRsc_GetLinkToGrpTyp (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   long GrpTypCod = ParCod_GetAndCheckPar (ParCod_GrpTyp);
   char GrpTypName[Grp_MAX_BYTES_GROUP_TYPE_NAME + 1];	// Name of type of group

   /***** Get tag title *****/
   Grp_DB_GetGrpTypTitle (GrpTypCod,GrpTypName,Grp_MAX_BYTES_GROUP_TYPE_NAME);

   /***** Copy link to tag into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_GROUPS,GrpTypCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  GrpTypName);

   /***** Show edition of groups again *****/
   Grp_ReqEditGroups ();
  }
