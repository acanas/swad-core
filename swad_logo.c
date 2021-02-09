// swad_logo.c: logo of institution, centre or degree

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_action.h"
#include "swad_box.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_scope.h"
#include "swad_theme.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Private types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Lgo_PutIconToRemoveLogoIns (__attribute__((unused)) void *Args);
static void Lgo_PutIconToRemoveLogoCtr (__attribute__((unused)) void *Args);
static void Lgo_PutIconToRemoveLogoDeg (__attribute__((unused)) void *Args);
static void Lgo_PutIconToRemoveLogo (Act_Action_t ActionRem);

/*****************************************************************************/
/****************************** Draw degree logo *****************************/
/*****************************************************************************/

void Lgo_DrawLogo (Hie_Level_t Scope,long Cod,const char *AltText,
                   unsigned Size,const char *Class,bool PutIconIfNotExists)
  {
   static const char *HieIcon[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = "sitemap.svg",	// not applicable here
      [Hie_SYS] = "sitemap.svg",	// not applicable here
      [Hie_CTY] = "sitemap.svg",	// not applicable here
      [Hie_INS] = "university.svg",
      [Hie_CTR] = "building.svg",
      [Hie_DEG] = "graduation-cap.svg",
      [Hie_CRS] = "sitemap.svg",	// not applicable here
     };
   const char *Folder = NULL;	// To avoid warning
   char PathLogo[PATH_MAX + 1];
   bool LogoFound = false;
   long InsCod;
   long CtrCod;
   long DegCod;
   char *URL;
   char *Icon;
   bool ClassNotEmpty;

   /***** Path to logo *****/
   if (HieIcon[Scope])	// Scope is correct
     {
      if (Cod > 0)	// Institution, centre or degree exists
	{
	 /* Degree */
	 if (Scope == Hie_DEG)
	   {
	    Folder = Cfg_FOLDER_DEG;
	    DegCod = Cod;
	    snprintf (PathLogo,sizeof (PathLogo),
		      "%s/%02u/%u/logo/%u.png",
		      Cfg_PATH_DEG_PUBLIC,
		      (unsigned) (DegCod % 100),
		      (unsigned) DegCod,
		      (unsigned) DegCod);
	    LogoFound = Fil_CheckIfPathExists (PathLogo);
	    if (LogoFound)
	       Cod = DegCod;
	   }

	 /* Centre */
	 if (!LogoFound && Scope != Hie_INS)
	   {
	    Folder = Cfg_FOLDER_CTR;
	    if (Scope == Hie_DEG)	// && !LogoFound
	       CtrCod = Deg_GetCtrCodOfDegreeByCod (Cod);
	    else
	       CtrCod = Cod;
	    snprintf (PathLogo,sizeof (PathLogo),
		      "%s/%02u/%u/logo/%u.png",
		      Cfg_PATH_CTR_PUBLIC,
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
	    if (Scope == Hie_DEG)		// && !LogoFound
	       InsCod = Deg_GetInsCodOfDegreeByCod (Cod);
	    else if (Scope == Hie_CTR)	// && !LogoFound
	       InsCod = Ctr_GetInsCodOfCentreByCod (Cod);
	    else
	       InsCod = Cod;
	    snprintf (PathLogo,sizeof (PathLogo),
		      "%s/%02u/%u/logo/%u.png",
		      Cfg_PATH_INS_PUBLIC,
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
	    if (LogoFound)
	      {
	       if (asprintf (&URL,"%s/%s/%02u/%u/logo",
			     Cfg_URL_SWAD_PUBLIC,Folder,
			     (unsigned) (Cod % 100),
			     (unsigned) Cod) < 0)
		  Lay_NotEnoughMemoryExit ();
	       if (asprintf (&Icon,"%u.png",(unsigned) Cod) < 0)
		  Lay_NotEnoughMemoryExit ();
	      }
	    else
	      {
	       if (asprintf (&URL,"%s",Cfg_URL_ICON_PUBLIC) < 0)
		  Lay_NotEnoughMemoryExit ();
	       if (asprintf (&Icon,"%s",HieIcon[Scope]) < 0)
		  Lay_NotEnoughMemoryExit ();
	      }
	    ClassNotEmpty = false;
	    if (Class)
	       if (Class[0])
		  ClassNotEmpty = true;
	    HTM_IMG (URL,Icon,AltText,
		     "class=\"ICO%ux%u%s%s\"",
		     Size,Size,
		     ClassNotEmpty ? " " :
			             "",
		     ClassNotEmpty ? Class :
			             "");
	    free (Icon);
            free (URL);
	   }
	}
     }
  }

/*****************************************************************************/
/************* Put an icon to go to the action used to request ***************/
/************* the logo of institution, centre or degree       ***************/
/*****************************************************************************/

void Lgo_PutIconToChangeLogo (Hie_Level_t Scope)
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
      case Hie_INS:
	 Action = ActReqInsLog;
	 Cod = Gbl.Hierarchy.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case Hie_CTR:
	 Action = ActReqCtrLog;
	 Cod = Gbl.Hierarchy.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case Hie_DEG:
	 Action = ActReqDegLog;
	 Cod = Gbl.Hierarchy.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Check if logo exists *****/
   snprintf (PathLogo,sizeof (PathLogo),
	     "%s/%s/%02u/%u/logo/%u.png",
	     Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned) Cod,
	     (unsigned) Cod);
   LogoExists = Fil_CheckIfPathExists (PathLogo);

   /***** Link for changing / uploading the logo *****/
   Lay_PutContextualLinkOnlyIcon (Action,NULL,
                                  NULL,NULL,
				  "shield-alt.svg",
				  LogoExists ? Txt_Change_logo :
					       Txt_Upload_logo);
  }

/*****************************************************************************/
/**** Show a form for sending a logo of the institution, centre or degree ****/
/*****************************************************************************/

void Lgo_RequestLogo (Hie_Level_t Scope)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Logo;
   extern const char *Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y;
   extern const char *Txt_File_with_the_logo;
   long Cod;
   const char *Folder;
   Act_Action_t ActionRec;
   void (*FunctionToDrawContextualIcons) (void *Args);
   char PathLogo[PATH_MAX + 1];

   /***** Set action depending on scope *****/
   switch (Scope)
     {
      case Hie_INS:
	 Cod = Gbl.Hierarchy.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 ActionRec = ActRecInsLog;
	 FunctionToDrawContextualIcons = Lgo_PutIconToRemoveLogoIns;
	 break;
      case Hie_CTR:
	 Cod = Gbl.Hierarchy.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 ActionRec = ActRecCtrLog;
	 FunctionToDrawContextualIcons = Lgo_PutIconToRemoveLogoCtr;
	 break;
      case Hie_DEG:
	 Cod = Gbl.Hierarchy.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 ActionRec = ActRecDegLog;
	 FunctionToDrawContextualIcons = Lgo_PutIconToRemoveLogoDeg;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Check if logo exists *****/
   snprintf (PathLogo,sizeof (PathLogo),
	     "%s/%s/%02u/%u/logo/%u.png",
	     Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned) Cod,
	     (unsigned) Cod);
   if (!Fil_CheckIfPathExists (PathLogo))
      FunctionToDrawContextualIcons = NULL;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Logo,
                 FunctionToDrawContextualIcons,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Begin form to upload logo *****/
   Frm_StartForm (ActionRec);

   /***** Write help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y,
	          64,64);

   /***** Upload logo *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtColonNBSP (Txt_File_with_the_logo);
   HTM_INPUT_FILE (Fil_NAME_OF_PARAM_FILENAME_ORG,"image/png",
                   HTM_SUBMIT_ON_CHANGE,
                   NULL);
   HTM_LABEL_End ();

   /***** End form *****/
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Put a link to request the removal of the logo ****************/
/*****************************************************************************/

static void Lgo_PutIconToRemoveLogoIns (__attribute__((unused)) void *Args)
  {
   Lgo_PutIconToRemoveLogo (ActRemInsLog);
  }

static void Lgo_PutIconToRemoveLogoCtr (__attribute__((unused)) void *Args)
  {
   Lgo_PutIconToRemoveLogo (ActRemCtrLog);
  }

static void Lgo_PutIconToRemoveLogoDeg (__attribute__((unused)) void *Args)
  {
   Lgo_PutIconToRemoveLogo (ActRemDegLog);
  }

static void Lgo_PutIconToRemoveLogo (Act_Action_t ActionRem)
  {
   extern const char *Txt_Remove_logo;

   /***** Link to request the removal of the logo *****/
   Lay_PutContextualLinkOnlyIcon (ActionRem,NULL,
                                  NULL,NULL,
				  "trash.svg",
				  Txt_Remove_logo);
  }

/*****************************************************************************/
/******* Receive the logo of the current institution, centre or degree *******/
/*****************************************************************************/

void Lgo_ReceiveLogo (Hie_Level_t Scope)
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
      case Hie_INS:
	 Cod = Gbl.Hierarchy.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case Hie_CTR:
	 Cod = Gbl.Hierarchy.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case Hie_DEG:
	 Cod = Gbl.Hierarchy.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Creates directories if not exist *****/
   snprintf (Path,sizeof (Path),
	     "%s/%s",
	     Cfg_PATH_SWAD_PUBLIC,Folder);
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),
	     "%s/%s/%02u",
	     Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100));
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),
	     "%s/%s/%02u/%u",
	     Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned) Cod);
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),
	     "%s/%s/%02u/%u/logo",
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
      Ale_ShowAlert (Ale_WARNING,Txt_The_file_is_not_X,
		     "png");
   else
     {
      /* End the reception of logo in a temporary file */
      snprintf (FileNameLogo,sizeof (FileNameLogo),
	        "%s/%s/%02u/%u/logo/%u.png",
	        Cfg_PATH_SWAD_PUBLIC,Folder,
	        (unsigned) (Cod % 100),
	        (unsigned) Cod,
	        (unsigned) Cod);
      if (!Fil_EndReceptionOfFile (FileNameLogo,Param))
	 Ale_ShowAlert (Ale_ERROR,"Error copying file.");
     }
  }

/*****************************************************************************/
/******* Remove the logo of the current institution, centre or degree ********/
/*****************************************************************************/

void Lgo_RemoveLogo (Hie_Level_t Scope)
  {
   long Cod;
   const char *Folder;
   char FileNameLogo[PATH_MAX + 1];	// Full name (including path and .png) of the destination file

   /***** Set variables depending on scope *****/
   switch (Scope)
     {
      case Hie_INS:
	 Cod = Gbl.Hierarchy.Ins.InsCod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case Hie_CTR:
	 Cod = Gbl.Hierarchy.Ctr.CtrCod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case Hie_DEG:
	 Cod = Gbl.Hierarchy.Deg.DegCod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Remove logo *****/
   snprintf (FileNameLogo,sizeof (FileNameLogo),
	     "%s/%s/%02u/%u/logo/%u.png",
	     Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned) Cod,
	     (unsigned) Cod);
   Fil_RemoveTree (FileNameLogo);
  }
