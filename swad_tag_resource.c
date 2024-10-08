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
#include "swad_tag.h"
#include "swad_tag_database.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/***************************** Get link to tag *******************************/
/*****************************************************************************/

void TagRsc_GetLinkToTag (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   long TagCod = ParCod_GetAndCheckPar (ParCod_Tag);
   char TagTxt[Tag_MAX_BYTES_TAG + 1];

   /***** Get tag title *****/
   Tag_DB_GetTagTitleByCod (TagCod,TagTxt,Tag_MAX_BYTES_TAG);

   /***** Copy link to tag into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_TEST,TagCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
   		  TagTxt);

   /***** Show tags again *****/
   Tag_ShowFormEditTags ();
  }
