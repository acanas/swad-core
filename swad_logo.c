// swad_logo.c: logo of institution, center or degree

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_degree_database.h"
#include "swad_center_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_scope.h"
#include "swad_theme.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Lgo_PutIconToRemoveLogoIns (__attribute__((unused)) void *Args);
static void Lgo_PutIconToRemoveLogoCtr (__attribute__((unused)) void *Args);
static void Lgo_PutIconToRemoveLogoDeg (__attribute__((unused)) void *Args);
static void Lgo_PutIconToRemoveLogo (Act_Action_t ActionRem);

/*****************************************************************************/
/***************** Draw institution, center or degree logo *******************/
/*****************************************************************************/

void Lgo_DrawLogo (HieLvl_Level_t Level,long HieCod,const char *AltText,
                   unsigned Size,const char *Class)
  {
   static const char *HieIcon[HieLvl_NUM_LEVELS] =
     {
      [HieLvl_INS] = "university.svg",
      [HieLvl_CTR] = "building.svg",
      [HieLvl_DEG] = "graduation-cap.svg",
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
   if (HieIcon[Level])	// Scope is correct
     {
      if (HieCod > 0)	// Institution, center or degree exists
	{
	 /* Degree */
	 if (Level == HieLvl_DEG)
	   {
	    Folder = Cfg_FOLDER_DEG;
	    DegCod = HieCod;
	    snprintf (PathLogo,sizeof (PathLogo),"%s/%02u/%u/logo/%u.png",
		      Cfg_PATH_DEG_PUBLIC,
		      (unsigned) (DegCod % 100),
		      (unsigned)  DegCod,
		      (unsigned)  DegCod);
	    LogoFound = Fil_CheckIfPathExists (PathLogo);
	    if (LogoFound)
	       HieCod = DegCod;
	   }

	 /* Center */
	 if (!LogoFound && Level != HieLvl_INS)
	   {
	    Folder = Cfg_FOLDER_CTR;
	    if (Level == HieLvl_DEG)	// && !LogoFound
	       CtrCod = Deg_DB_GetCtrCodOfDegreeByCod (HieCod);
	    else
	       CtrCod = HieCod;
	    snprintf (PathLogo,sizeof (PathLogo),"%s/%02u/%u/logo/%u.png",
		      Cfg_PATH_CTR_PUBLIC,
		      (unsigned) (CtrCod % 100),
		      (unsigned)  CtrCod,
		      (unsigned)  CtrCod);
	    LogoFound = Fil_CheckIfPathExists (PathLogo);
	    if (LogoFound)
	       HieCod = CtrCod;
	   }

	 /* Institution */
	 if (!LogoFound)
	   {
	    Folder = Cfg_FOLDER_INS;
	    if (Level == HieLvl_DEG)		// && !LogoFound
	       InsCod = Deg_DB_GetInsCodOfDegreeByCod (HieCod);
	    else if (Level == HieLvl_CTR)	// && !LogoFound
	       InsCod = Ctr_DB_GetInsCodOfCenterByCod (HieCod);
	    else
	       InsCod = HieCod;
	    snprintf (PathLogo,sizeof (PathLogo),"%s/%02u/%u/logo/%u.png",
		      Cfg_PATH_INS_PUBLIC,
		      (unsigned) (InsCod % 100),
		      (unsigned)  InsCod,
		      (unsigned)  InsCod);
	    LogoFound = Fil_CheckIfPathExists (PathLogo);
	    if (LogoFound)
	       HieCod = InsCod;
	   }

	 /***** Draw logo *****/
	 ClassNotEmpty = false;
	 if (Class)
	    if (Class[0])
	       ClassNotEmpty = true;

	 if (LogoFound)
	   {
	    if (asprintf (&URL,"%s/%s/%02u/%u/logo",
			  Cfg_URL_SWAD_PUBLIC,Folder,
			  (unsigned) (HieCod % 100),
			  (unsigned) HieCod) < 0)
	       Err_NotEnoughMemoryExit ();
	    if (asprintf (&Icon,"%u.png",(unsigned) HieCod) < 0)
	       Err_NotEnoughMemoryExit ();

	    HTM_IMG (URL,Icon,AltText,
		     "class=\"ICO%ux%u"
			     "%s%s\"",
		     Size,Size,
		     ClassNotEmpty ? " " :
				     "",
		     ClassNotEmpty ? Class :
				     "");
	    free (Icon);
	    free (URL);
	   }
	 else
	    HTM_IMG (Cfg_URL_ICON_PUBLIC,HieIcon[Level],AltText,
		     "class=\"ICO%ux%u ICO_%s_%s"
			     "%s%s\"",
		     Size,Size,
		     Ico_GetPreffix (Ico_BLACK),The_GetSuffix (),
		     ClassNotEmpty ? " " :
				     "",
		     ClassNotEmpty ? Class :
				     "");
	}
     }
  }

/*****************************************************************************/
/************* Put an icon to go to the action used to request ***************/
/************* the logo of institution, center or degree       ***************/
/*****************************************************************************/

void Lgo_PutIconToChangeLogo (HieLvl_Level_t Level)
  {
   static Act_Action_t Action[HieLvl_NUM_LEVELS] =
     {
      [HieLvl_UNK] = ActUnk,		// Unknown
      [HieLvl_SYS] = ActUnk,		// System
      [HieLvl_CTY] = ActUnk,		// Country
      [HieLvl_INS] = ActReqInsLog,	// Institution
      [HieLvl_CTR] = ActReqCtrLog,	// Center
      [HieLvl_DEG] = ActReqDegLog,	// Degree
      [HieLvl_CRS] = ActUnk,		// Course
     };

   Lay_PutContextualLinkOnlyIcon (Action[Level],NULL,
                                  NULL,NULL,
				  "shield-alt.svg",Ico_BLACK);
  }

/*****************************************************************************/
/**** Show a form for sending a logo of the institution, center or degree ****/
/*****************************************************************************/

void Lgo_RequestLogo (HieLvl_Level_t Level)
  {
   extern const char *Txt_Logo;
   extern const char *Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y;
   extern const char *Txt_File_with_the_logo;
   long Cod;
   const char *Folder;
   Act_Action_t ActionRec;
   void (*FunctionToDrawContextualIcons) (void *Args);
   char PathLogo[PATH_MAX + 1];

   /***** Set action depending on scope *****/
   switch (Level)
     {
      case HieLvl_INS:
	 Cod = Gbl.Hierarchy.Node[HieLvl_INS].Cod;
	 Folder = Cfg_FOLDER_INS;
	 ActionRec = ActRecInsLog;
	 FunctionToDrawContextualIcons = Lgo_PutIconToRemoveLogoIns;
	 break;
      case HieLvl_CTR:
	 Cod = Gbl.Hierarchy.Node[HieLvl_CTR].Cod;
	 Folder = Cfg_FOLDER_CTR;
	 ActionRec = ActRecCtrLog;
	 FunctionToDrawContextualIcons = Lgo_PutIconToRemoveLogoCtr;
	 break;
      case HieLvl_DEG:
	 Cod = Gbl.Hierarchy.Node[HieLvl_DEG].Cod;
	 Folder = Cfg_FOLDER_DEG;
	 ActionRec = ActRecDegLog;
	 FunctionToDrawContextualIcons = Lgo_PutIconToRemoveLogoDeg;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Check if logo exists *****/
   snprintf (PathLogo,sizeof (PathLogo),"%s/%s/%02u/%u/logo/%u.png",
	     Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned)  Cod,
	     (unsigned)  Cod);
   if (!Fil_CheckIfPathExists (PathLogo))
      FunctionToDrawContextualIcons = NULL;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Logo,
                 FunctionToDrawContextualIcons,NULL,
                 NULL,Box_NOT_CLOSABLE);

      /***** Begin form to upload logo *****/
      Frm_BeginForm (ActionRec);

	 /***** Write help message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y,
			64,64);

	 /***** Upload logo *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
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
   Lay_PutContextualLinkOnlyIcon (ActionRem,NULL,
                                  NULL,NULL,
				  "trash.svg",Ico_RED);
  }

/*****************************************************************************/
/******* Receive the logo of the current institution, center or degree *******/
/*****************************************************************************/

void Lgo_ReceiveLogo (HieLvl_Level_t Level)
  {
   extern const char *Txt_The_file_is_not_X;
   long Cod;
   const char *Folder;
   char Path[PATH_MAX + 1];
   struct Par_Param *Par;
   char FileNameLogoSrc[PATH_MAX + 1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char FileNameLogo[PATH_MAX + 1];	// Full name (including path and .png) of the destination file
   bool WrongType = false;

   /***** Set variables depending on scope *****/
   switch (Level)
     {
      case HieLvl_INS:
	 Cod = Gbl.Hierarchy.Node[HieLvl_INS].Cod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case HieLvl_CTR:
	 Cod = Gbl.Hierarchy.Node[HieLvl_CTR].Cod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case HieLvl_DEG:
	 Cod = Gbl.Hierarchy.Node[HieLvl_DEG].Cod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Creates directories if not exist *****/
   snprintf (Path,sizeof (Path),"%s/%s",Cfg_PATH_SWAD_PUBLIC,Folder);
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),"%s/%s/%02u",Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100));
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),"%s/%s/%02u/%u",Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned)  Cod);
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),"%s/%s/%02u/%u/logo",Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned)  Cod);
   Fil_CreateDirIfNotExists (Path);

   /***** Copy in disk the file received *****/
   Par = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
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
      snprintf (FileNameLogo,sizeof (FileNameLogo),"%s/%s/%02u/%u/logo/%u.png",
	        Cfg_PATH_SWAD_PUBLIC,Folder,
	        (unsigned) (Cod % 100),
	        (unsigned)  Cod,
	        (unsigned)  Cod);
      if (!Fil_EndReceptionOfFile (FileNameLogo,Par))
	 Ale_ShowAlert (Ale_ERROR,"Error copying file.");
     }
  }

/*****************************************************************************/
/******* Remove the logo of the current institution, center or degree ********/
/*****************************************************************************/

void Lgo_RemoveLogo (HieLvl_Level_t Level)
  {
   long Cod;
   const char *Folder;
   char FileNameLogo[PATH_MAX + 1];	// Full name (including path and .png) of the destination file

   /***** Set variables depending on scope *****/
   switch (Level)
     {
      case HieLvl_INS:
	 Cod = Gbl.Hierarchy.Node[HieLvl_INS].Cod;
	 Folder = Cfg_FOLDER_INS;
	 break;
      case HieLvl_CTR:
	 Cod = Gbl.Hierarchy.Node[HieLvl_CTR].Cod;
	 Folder = Cfg_FOLDER_CTR;
	 break;
      case HieLvl_DEG:
	 Cod = Gbl.Hierarchy.Node[HieLvl_DEG].Cod;
	 Folder = Cfg_FOLDER_DEG;
	 break;
      default:
	 return;	// Nothing to do
     }

   /***** Remove logo *****/
   snprintf (FileNameLogo,sizeof (FileNameLogo),"%s/%s/%02u/%u/logo/%u.png",
	     Cfg_PATH_SWAD_PUBLIC,Folder,
	     (unsigned) (Cod % 100),
	     (unsigned)  Cod,
	     (unsigned)  Cod);
   Fil_RemoveTree (FileNameLogo);
  }
