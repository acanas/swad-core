// swad_browser_resource.c: links to documents as program resources
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

#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_browser.h"
#include "swad_browser_resource.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_parameter_code.h"
#include "swad_resource_database.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Get link to file *****************************/
/*****************************************************************************/

void BrwRsc_GetLinkToDocFil (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   long FilCod;
   char Title[NAME_MAX + 1];	// File or link name

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file code *****/
   FilCod = ParCod_GetPar (ParCod_Fil);

   /***** Get file title *****/
   BrwRsc_GetTitleFromDocFilCod (FilCod,Title,sizeof (Title) - 1);

   /***** Copy link to file into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_DOCUMENT,FilCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
		  Title);

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

void BrwRsc_GetLinkToMrkFil (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   long FilCod;
   char Title[NAME_MAX + 1];	// File or link name

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file code *****/
   FilCod = ParCod_GetPar (ParCod_Fil);

   /***** Get file title *****/
   BrwRsc_GetTitleFromMrkFilCod (FilCod,Title,sizeof (Title) - 1);

   /***** Copy link to file into resource clipboard *****/
   Rsc_DB_CopyToClipboard (Rsc_MARKS,FilCod);

   /***** Write sucess message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
		  Title);

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/********************** Get file name from file code *************************/
/*****************************************************************************/
// The trailing null character is not counted in TitleSize

void BrwRsc_GetTitleFromDocFilCod (long FilCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Documents;
   struct Brw_FileMetadata FileMetadata;

   if (FilCod > 0)
     {
      /***** Get file name *****/
      FileMetadata.FilCod = FilCod;
      Brw_GetPathByCod (&FileMetadata);

      /***** Remove .url if it's a link *****/
      if (Str_FileIs (FileMetadata.FilFolLnk.Name,"url"))
         FileMetadata.FilFolLnk.Name[strlen (FileMetadata.FilFolLnk.Name) - 4] = '\0';

      /***** Copy file name into title *****/
      Str_Copy (Title,FileMetadata.FilFolLnk.Name,TitleSize);
     }
   else
      /***** Generic title for all documents *****/
      Str_Copy (Title,Txt_Documents,TitleSize);
  }

void BrwRsc_GetTitleFromMrkFilCod (long FilCod,char *Title,size_t TitleSize)
  {
   extern const char *Txt_Marks_area;
   struct Brw_FileMetadata FileMetadata;

   if (FilCod > 0)
     {
      /***** Get file name *****/
      FileMetadata.FilCod = FilCod;
      Brw_GetPathByCod (&FileMetadata);

      /***** Copy file name into title *****/
      Str_Copy (Title,FileMetadata.FilFolLnk.Name,TitleSize);
     }
   else
      /***** Generic title for marks area *****/
      Str_Copy (Title,Txt_Marks_area,TitleSize);
  }
