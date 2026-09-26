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
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Get link to file *****************************/
/*****************************************************************************/

void BrwRsc_GetLinkToDocFil (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Documents;
   struct Brw_FileBrowser FileBrowser;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser (&FileBrowser);
   Brw_GetParsFilFolLnk (&FileBrowser);

   /***** Get file code *****/
   FileBrowser.FileMetadata.FilCod = ParCod_GetPar (ParCod_Fil);

   /***** Get file metadata *****/
   if (FileBrowser.FileMetadata.FilCod > 0)
      Brw_GetFileMetadataByCod (&FileBrowser.FileMetadata);

   /***** Copy link to file into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_DOCUMENT,FileBrowser.FileMetadata.FilCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
                  FileBrowser.FileMetadata.FilCod > 0 ? FileBrowser.FileMetadata.FilFolLnk.Name :
                					Txt_Documents);

   /***** Show again the file browser *****/
   Brw_ShowFileBrowserNormal (&FileBrowser);
  }

void BrwRsc_GetLinkToMrkFil (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   extern const char *Txt_Marks_area;
   struct Brw_FileBrowser FileBrowser;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser (&FileBrowser);
   Brw_GetParsFilFolLnk (&FileBrowser);

   /***** Get file code *****/
   FileBrowser.FileMetadata.FilCod = ParCod_GetPar (ParCod_Fil);

   /***** Get file metadata *****/
   if (FileBrowser.FileMetadata.FilCod > 0)
      Brw_GetFileMetadataByCod (&FileBrowser.FileMetadata);

   /***** Copy link to file into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_MARKS,FileBrowser.FileMetadata.FilCod);

   /***** Write success message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
                  FileBrowser.FileMetadata.FilCod > 0 ? FileBrowser.FileMetadata.FilFolLnk.Name :
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
   struct Brw_FileMetadata FileMetadata;

   /***** Get file name *****/
   FileMetadata.FilCod = FilCod;
   Brw_DB_GetPathByCod (FileMetadata.FilCod,
                        FileMetadata.FilFolLnk.Full,
                        sizeof (FileMetadata.FilFolLnk.Full) - 1);
   Str_SplitFullPathIntoPathAndFileName (FileMetadata.FilFolLnk.Full,
					 FileMetadata.FilFolLnk.Path,
					 FileMetadata.FilFolLnk.Name);

   /***** Remove .url if it's a link *****/
   if (Str_FileIs (FileMetadata.FilFolLnk.Name,"url"))
      FileMetadata.FilFolLnk.Name[strlen (FileMetadata.FilFolLnk.Name) - 4] = '\0';

   /***** Copy file name into title *****/
   Str_Copy (Title,FileMetadata.FilFolLnk.Name,TitleSize);
  }
