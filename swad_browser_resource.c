// swad_browser_resource.c: links to documents as program resources
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

#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_browser.h"
#include "swad_browser_database.h"
#include "swad_browser_resource.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/****************************** Get link to file *****************************/
/*****************************************************************************/

void BrwRsc_GetLinkToDocFil (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Documents;
   struct Brw_FileBrowser FileBrowser;
   struct Brw_File File;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser (&FileBrowser);
   Brw_GetParFilCodAndGetFileMetadata (&FileBrowser,&File);	// Get file / folder / link

   /***** Copy link to file into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_DOCUMENT,File.FilCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
                  File.FilCod > 0 ? File.Path.Name :
                		             Txt_Documents);

   /***** Show again the file browser *****/
   Brw_ShowFileBrowserNormal (&FileBrowser);
  }

void BrwRsc_GetLinkToMrkFil (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Marks_area;
   struct Brw_FileBrowser FileBrowser;
   struct Brw_File File;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser (&FileBrowser);
   Brw_GetParFilCodAndGetFileMetadata (&FileBrowser,&File);	// Get file / folder / link

   /***** Copy link to file into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_MARKS,File.FilCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
                  File.FilCod > 0 ? File.Path.Name :
                			     Txt_Marks_area);

   /***** Show again the file browser *****/
   Brw_ShowFileBrowserNormal (&FileBrowser);
  }

/*****************************************************************************/
/********************** Get file name from file code *************************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void BrwRsc_GetFileTitle (long FilCod,char *Title,size_t TitleSize)
  {
   struct Brw_File File;

   /***** Get file name *****/
   File.FilCod = FilCod;
   Brw_DB_GetPathByCod (File.FilCod,
                        File.Path.InZone,
                        sizeof (File.Path.InZone) - 1);
   Brw_SplitFullPathIntoPathAndName (File.Path.InZone,
				     File.Path.InZoneWithoutName,
				     File.Path.Name);

   /***** Remove .url if it's a link *****/
   if (Str_FileIs (File.Path.Name,"url"))
      File.Path.Name[strlen (File.Path.Name) - 4] = '\0';

   /***** Copy file name into title *****/
   Str_Copy (Title,File.Path.Name,TitleSize);
  }
