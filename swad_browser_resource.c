// swad_browser_resource.c: links to documents as program resources
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Cañas Vargas

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

#include "swad_action.h"
#include "swad_browser.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_program.h"
#include "swad_program_database.h"
#include "swad_program_resource.h"

/*****************************************************************************/
/******************** Global variables from other modules ********************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Get link to file *****************************/
/*****************************************************************************/

void BrwRsc_GetLinkToFile (void)
  {
   extern const char *Txt_Link_to_resource_X_copied_into_clipboard;
   struct Brw_FileMetadata FileMetadata;
   bool Found;
   PrgRsc_Type_t Type;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get file name *****/
   FileMetadata.FilCod = Brw_GetParamFilCod ();
   Brw_GetFileNameByCod (&FileMetadata);
   Found = Brw_GetFileTypeSizeAndDate (&FileMetadata);

   if (Found)
     {
      /***** Copy link to file into resource clipboard *****/
      switch (Gbl.Action.Act)
        {
	 case ActReqLnkSeeDocCrs:
	 case ActReqLnkAdmDocCrs:
	    Type = PrgRsc_DOCUMENT;
	    break;
	 case ActReqLnkSeeMrkCrs:
	 case ActReqLnkAdmMrkCrs:
	    Type = PrgRsc_MARKS;
	    break;
	 default:
	    Type = PrgRsc_NONE;	// Initialized to avoid warning
            Err_WrongTypeExit ();
            break;
        }
      Prg_DB_CopyToClipboard (Type,FileMetadata.FilCod);

      /***** Write sucess message *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_Link_to_resource_X_copied_into_clipboard,
		     FileMetadata.FilFolLnk.Name);
     }

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/******************** Write file name in course program **********************/
/*****************************************************************************/

void BrwRsc_WriteDocFileNameInCrsProgram (long FilCod,bool PutFormToGo,
                                          const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[Act_NUM_ACTIONS];
   Act_Action_t NextAction;
   struct Brw_FileMetadata FileMetadata;

   /***** Get file metadata *****/
   FileMetadata.FilCod = FilCod;
   Brw_GetFileMetadataByCod (&FileMetadata);

   /***** Begin form to go to file data *****/
   if (PutFormToGo)
     {
      NextAction = (FileMetadata.FilCod > 0) ? ActReqDatSeeDocCrs :	// Document specified
					       ActSeeAdmDocCrsGrp;	// All documents
      Frm_BeginForm (NextAction);
	 Brw_PutParamsFileBrowser (NULL,		// Not used
				   NULL,		// Not used
				   Brw_IS_UNKNOWN,	// Not used
				   FileMetadata.FilCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   switch (FileMetadata.FilFolLnk.Type)
     {
      case Brw_IS_FILE:
	 Brw_PutIconFile (FileMetadata.FilFolLnk.Name,
			  "CONTEXT_OPT ICO_HIGHLIGHT CONTEXT_ICO16x16",
			  PutFormToGo);	// Put link to view metadata
	 break;
      case Brw_IS_LINK:
	 if (PutFormToGo)
	    Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
	 else
	    Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);
	 break;
      default:
	 break;
     }

   /***** Write filename *****/
   HTM_Txt (FileMetadata.FilFolLnk.Name);

   /***** End form to download file *****/
   if (PutFormToGo)
     {
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/******************** Write file name in course program **********************/
/*****************************************************************************/

void BrwRsc_WriteMrkFileNameInCrsProgram (long FilCod,bool PutFormToGo,
                                          const char *Icon,const char *IconTitle)
  {
   extern const char *Txt_Actions[Act_NUM_ACTIONS];
   Act_Action_t NextAction;
   struct Brw_FileMetadata FileMetadata;

   /***** Get file metadata *****/
   FileMetadata.FilCod = FilCod;
   Brw_GetFileMetadataByCod (&FileMetadata);

   /***** Begin form to go to file data *****/
   if (PutFormToGo)
     {
      NextAction = (FileMetadata.FilCod > 0) ? ActReqDatSeeMrkCrs :	// Marks file specified
					       ActSeeAdmMrk;		// All marks files
      Frm_BeginForm (NextAction);
	 Brw_PutParamsFileBrowser (NULL,		// Not used
				   NULL,		// Not used
				   Brw_IS_UNKNOWN,	// Not used
				   FileMetadata.FilCod);
	 HTM_BUTTON_Submit_Begin (Txt_Actions[NextAction],
	                          "class=\"LM BT_LINK PRG_LNK_%s\"",
	                          The_GetSuffix ());
     }

   /***** Icon depending on type ******/
   if (PutFormToGo)
      Ico_PutIconLink (Icon,Ico_BLACK,NextAction);
   else
      Ico_PutIconOn (Icon,Ico_BLACK,IconTitle);

   /***** Write filename *****/
   HTM_Txt (FileMetadata.FilFolLnk.Name);

   /***** End form to download file *****/
   if (PutFormToGo)
     {
         HTM_BUTTON_End ();

      Frm_EndForm ();
     }
  }

/*****************************************************************************/
/********************** Get file name from file code *************************/
/*****************************************************************************/

void BrwRsc_GetFileNameFromFilCod (long FilCod,char *FileName,size_t FileNameSize)
  {
   struct Brw_FileMetadata FileMetadata;

   if (FilCod > 0)
     {
      /***** Return nothing on error *****/
      FileName[0] = '\0';	// Return nothing on error

      /***** Get file metadata *****/
      FileMetadata.FilCod = FilCod;
      Brw_GetFileMetadataByCod (&FileMetadata);

      /***** Copy file name into summary string *****/
      Str_Copy (FileName,FileMetadata.FilFolLnk.Name,FileNameSize);
     }
   else
      Str_Copy (FileName,"?",FileNameSize);
  }
