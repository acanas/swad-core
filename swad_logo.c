// swad_logo.c: logo of institution, centre or degree

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_box.h"
#include "swad_global.h"
#include "swad_scope.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal constants ****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Draw degree logo *****************************/
/*****************************************************************************/

void Log_DrawLogo (Sco_Scope_t Scope,long Cod,const char *AltText,
                   unsigned Size,const char *Class,bool PutIconIfNotExists)
  {
   static const char *Icon[Sco_NUM_SCOPES] =
     {
      NULL,		// Sco_SCOPE_UNK
      NULL,		// Sco_SCOPE_SYS
      NULL,		// Sco_SCOPE_CTY
      Cfg_FOLDER_INS,	// Sco_SCOPE_INS
      Cfg_FOLDER_CTR,	// Sco_SCOPE_CTR
      Cfg_FOLDER_DEG,	// Sco_SCOPE_DEG
      NULL,		// Sco_SCOPE_CRS
     };
   const char *Folder = NULL;	// To avoid warning
   char PathLogo[PATH_MAX + 1];
   bool LogoFound = false;
   long InsCod;
   long CtrCod;
   long DegCod;

   /***** Path to logo *****/
   if (Icon[Scope])	// Scope is correct
     {
      if (Cod > 0)	// Institution, centre or degree exists
	{
	 /* Degree */
	 if (Scope == Sco_SCOPE_DEG)
	   {
	    Folder = Cfg_FOLDER_DEG;
	    DegCod = Cod;
	    sprintf (PathLogo,"%s/%s/%02u/%u/logo/%u.png",
		     Cfg_PATH_SWAD_PUBLIC,Folder,
		     (unsigned) (DegCod % 100),
		     (unsigned) DegCod,
		     (unsigned) DegCod);
	    LogoFound = Fil_CheckIfPathExists (PathLogo);
	    if (LogoFound)
	       Cod = DegCod;
	   }

	 /* Centre */
	 if (!LogoFound && Scope != Sco_SCOPE_INS)
	   {
	    Folder = Cfg_FOLDER_CTR;
	    if (Scope == Sco_SCOPE_DEG)	// && !LogoFound
	       CtrCod = Deg_GetCtrCodOfDegreeByCod (Cod);
	    else
	       CtrCod = Cod;
	    sprintf (PathLogo,"%s/%s/%02u/%u/logo/%u.png",
		     Cfg_PATH_SWAD_PUBLIC,Folder,
		     (unsigned) (CtrCod % 100),
		     (unsigned) CtrCod,
		     (unsigned) CtrCod);
	    LogoFound = Fil_CheckIfPathExists (PathLogo);
	    if (LogoFound)
	       Cod = CtrCod;
	   }

	 /* Institution */
	 if (!LogoFound)
	   {
	    Folder = Cfg_FOLDER_INS;
	    if (Scope == Sco_SCOPE_DEG)		// && !LogoFound
	       InsCod = Deg_GetInsCodOfDegreeByCod (Cod);
	    else if (Scope == Sco_SCOPE_CTR)	// && !LogoFound
	       InsCod = Ctr_GetInsCodOfCentreByCod (Cod);
	    else
	       InsCod = Cod;
	    sprintf (PathLogo,"%s/%s/%02u/%u/logo/%u.png",
		     Cfg_PATH_SWAD_PUBLIC,Folder,
		     (unsigned) (InsCod % 100),
		     (unsigned) InsCod,
		     (unsigned) InsCod);
	    LogoFound = Fil_CheckIfPathExists (PathLogo);
	    if (LogoFound)
	       Cod = InsCod;
	   }

	 if (LogoFound || PutIconIfNotExists)
	   {
	    /***** Draw logo *****/
	    fprintf (Gbl.F.Out,"<img src=\"");
	    if (LogoFound)
	       fprintf (Gbl.F.Out,"%s/%s/%02u/%u/logo/%u.png",
			Cfg_URL_SWAD_PUBLIC,Folder,
			(unsigned) (Cod % 100),
			(unsigned) Cod,
			(unsigned) Cod);
	    else if (Icon[Scope])
	       fprintf (Gbl.F.Out,"%s/%s64x64.gif",
			Gbl.Prefs.IconsURL,Icon[Scope]);
	    fprintf (Gbl.F.Out,"\""
			       " alt=\"%s\" title=\"%s\""
			       " class=\"ICO%ux%u",
		     AltText,AltText,
		     Size,Size);
	    if (Class)
	       if (Class[0])
		  fprintf (Gbl.F.Out," %s",Class);
	    fprintf (Gbl.F.Out,"\" />");
	   }
	}
     }
  }

/*****************************************************************************/
/************* Put an icon to go to the action used to request ***************/
/************* the logo of institution, centre or degree       ***************/
/*****************************************************************************/

void Log_PutIconToChangeLogo (Sco_Scope_t Scope)
  {
   extern const char *Txt_Change_logo;
   extern const char *Txt_Upload_logo;
   Act_Action_t Action;
   long Cod;
   const char *Folder;
   char PathLogo[PATH_MAX + 1];
   bool LogoExists;

   /***** Set variables depending on scope *****/
   switch (Scope)
     {
      case Sco_SCOPE_INS:
	 Action = ActReqInsLog;
	 Cod = Gbl.CurrentIns.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case Sco_SCOPE_CTR:
	 Action = ActReqCtrLog;
	 Cod = Gbl.CurrentCtr.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case Sco_SCOPE_DEG:
	 Action = ActReqDegLog;
	 Cod = Gbl.CurrentDeg.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Check if logo exists *****/
   sprintf (PathLogo,"%s/%s/%02u/%u/logo/%u.png",
	    Cfg_PATH_SWAD_PUBLIC,Folder,
	    (unsigned) (Cod % 100),
	    (unsigned) Cod,
	    (unsigned) Cod);
   LogoExists = Fil_CheckIfPathExists (PathLogo);

   /***** Link for changing / uploading the logo *****/
   Lay_PutContextualLink (Action,NULL,NULL,
                          "logo64x64.png",
                          LogoExists ? Txt_Change_logo :
		                       Txt_Upload_logo,NULL,
                          NULL);
  }

/*****************************************************************************/
/**** Show a form for sending a logo of the institution, centre or degree ****/
/*****************************************************************************/

void Log_RequestLogo (Sco_Scope_t Scope)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Remove_logo;
   extern const char *Txt_Logo;
   extern const char *Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y;
   extern const char *Txt_File_with_the_logo;
   long Cod;
   const char *Folder;
   Act_Action_t ActionRec;
   Act_Action_t ActionRem;
   char PathLogo[PATH_MAX + 1];

   /***** Set action depending on scope *****/
   switch (Scope)
     {
      case Sco_SCOPE_INS:
	 Cod = Gbl.CurrentIns.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 ActionRec = ActRecInsLog;
	 ActionRem = ActRemInsLog;
	 break;
      case Sco_SCOPE_CTR:
	 Cod = Gbl.CurrentCtr.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 ActionRec = ActRecCtrLog;
	 ActionRem = ActRemCtrLog;
	 break;
      case Sco_SCOPE_DEG:
	 Cod = Gbl.CurrentDeg.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 ActionRec = ActRecDegLog;
	 ActionRem = ActRemDegLog;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Check if logo exists *****/
   sprintf (PathLogo,"%s/%s/%02u/%u/logo/%u.png",
	    Cfg_PATH_SWAD_PUBLIC,Folder,
	    (unsigned) (Cod % 100),
	    (unsigned) Cod,
	    (unsigned) Cod);
   if (Fil_CheckIfPathExists (PathLogo))
     {
      /***** Form to remove photo *****/
      fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
      Lay_PutContextualLink (ActionRem,NULL,NULL,
                             "remove-on64x64.png",
                             Txt_Remove_logo,Txt_Remove_logo,
                             NULL);
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Start form to upload logo *****/
   Act_FormStart (ActionRec);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Logo,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Write help message *****/
   sprintf (Gbl.Alert.Txt,Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y,
	    64,64);
   Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);

   /***** Upload logo *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
	              "%s:&nbsp;"
                      "<input type=\"file\" name=\"%s\""
                      " onchange=\"document.getElementById('%s').submit();\" />"
	              "</label>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_File_with_the_logo,
            Fil_NAME_OF_PARAM_FILENAME_ORG,
            Gbl.Form.Id);

   /***** End box *****/
   Box_EndBox ();

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******* Receive the logo of the current institution, centre or degree *******/
/*****************************************************************************/

void Log_ReceiveLogo (Sco_Scope_t Scope)
  {
   extern const char *Txt_The_file_is_not_X;
   long Cod;
   const char *Folder;
   char Path[PATH_MAX + 1];
   struct Param *Param;
   char FileNameLogoSrc[PATH_MAX + 1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char FileNameLogo[PATH_MAX + 1];	// Full name (including path and .png) of the destination file
   bool WrongType = false;

   /***** Set variables depending on scope *****/
   switch (Scope)
     {
      case Sco_SCOPE_INS:
	 Cod = Gbl.CurrentIns.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case Sco_SCOPE_CTR:
	 Cod = Gbl.CurrentCtr.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case Sco_SCOPE_DEG:
	 Cod = Gbl.CurrentDeg.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Creates directories if not exist *****/
   sprintf (Path,"%s/%s",
	    Cfg_PATH_SWAD_PUBLIC,Folder);
   Fil_CreateDirIfNotExists (Path);
   sprintf (Path,"%s/%s/%02u",
	    Cfg_PATH_SWAD_PUBLIC,Folder,
	    (unsigned) (Cod % 100));
   Fil_CreateDirIfNotExists (Path);
   sprintf (Path,"%s/%s/%02u/%u",
	    Cfg_PATH_SWAD_PUBLIC,Folder,
	    (unsigned) (Cod % 100),
	    (unsigned) Cod);
   Fil_CreateDirIfNotExists (Path);
   sprintf (Path,"%s/%s/%02u/%u/logo",
	    Cfg_PATH_SWAD_PUBLIC,Folder,
	    (unsigned) (Cod % 100),
	    (unsigned) Cod);
   Fil_CreateDirIfNotExists (Path);

   /***** Copy in disk the file received from stdin (really from Gbl.F.Tmp) *****/
   Param = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                     FileNameLogoSrc,MIMEType);

   /* Check if the file type is image/jpeg or image/pjpeg or application/octet-stream */
   if (strcmp (MIMEType,"image/png"))
      if (strcmp (MIMEType,"image/x-png"))
         if (strcmp (MIMEType,"application/octet-stream"))
            if (strcmp (MIMEType,"application/octetstream"))
               if (strcmp (MIMEType,"application/octet"))
                  WrongType = true;
   if (WrongType)
     {
      sprintf (Gbl.Alert.Txt,Txt_The_file_is_not_X,"png");
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }
   else
     {
      /* End the reception of logo in a temporary file */
      sprintf (FileNameLogo,"%s/%s/%02u/%u/logo/%u.png",
	       Cfg_PATH_SWAD_PUBLIC,Folder,
	       (unsigned) (Cod % 100),
	       (unsigned) Cod,
	       (unsigned) Cod);
      if (!Fil_EndReceptionOfFile (FileNameLogo,Param))
	 Ale_ShowAlert (Ale_WARNING,"Error copying file.");
     }
  }

/*****************************************************************************/
/******* Remove the logo of the current institution, centre or degree ********/
/*****************************************************************************/

void Log_RemoveLogo (Sco_Scope_t Scope)
  {
   long Cod;
   const char *Folder;
   char FileNameLogo[PATH_MAX + 1];	// Full name (including path and .png) of the destination file

   /***** Set variables depending on scope *****/
   switch (Scope)
     {
      case Sco_SCOPE_INS:
	 Cod = Gbl.CurrentIns.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case Sco_SCOPE_CTR:
	 Cod = Gbl.CurrentCtr.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case Sco_SCOPE_DEG:
	 Cod = Gbl.CurrentDeg.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Remove logo *****/
   sprintf (FileNameLogo,"%s/%s/%02u/%u/logo/%u.png",
	    Cfg_PATH_SWAD_PUBLIC,Folder,
	    (unsigned) (Cod % 100),
	    (unsigned) Cod,
	    (unsigned) Cod);
   Fil_RemoveTree (FileNameLogo);
  }
