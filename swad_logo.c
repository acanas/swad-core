// swad_logo.c: logo of institution, center or degree

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include "swad_center_database.h"
#include "swad_course_database.h"
#include "swad_degree_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_scope.h"
#include "swad_theme.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

static const char *Lgo_Folder[Hie_NUM_LEVELS] =
  {
   [Hie_INS] = Cfg_FOLDER_INS,
   [Hie_CTR] = Cfg_FOLDER_CTR,
   [Hie_DEG] = Cfg_FOLDER_DEG,
  };

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

void Lgo_DrawLogo (Hie_Level_t HieLvl,const struct Hie_Node *Node,
                   const char *IconClass)
  {
   extern const char *Hie_Icons[Hie_NUM_LEVELS];
   const char *Folder = NULL;	// To avoid warning
   char PathLogo[PATH_MAX + 1];
   bool LogoFound = false;
   long HieCod;
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;
   char *URL;
   char *Icon;

   switch (HieLvl)
     {
      case Hie_SYS:
	 Ico_PutIcon (Hie_Icons[Hie_SYS],Ico_BLACK,Node->ShrtName,IconClass);
	 break;
      case Hie_CTY:
	 Cty_DrawCountryMap (Node,IconClass);
	 break;
      case Hie_INS:
      case Hie_CTR:
      case Hie_DEG:
      case Hie_CRS:
	 if (Node->HieCod > 0)	// Institution, center, degree or course exists
	   {
	    HieCod =
	    InsCod =
	    CtrCod =
	    DegCod =
	    CrsCod = Node->HieCod;

	    /* Course */

	    /* Degree */
	    if (!LogoFound && HieLvl >= Hie_DEG)
	      {
	       Folder = Cfg_FOLDER_DEG;
	       if (HieLvl >= Hie_CRS)
		  DegCod = Crs_DB_GetDegCodOfCourseByCod (CrsCod);
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
	    if (!LogoFound && HieLvl >= Hie_CTR)
	      {
	       Folder = Cfg_FOLDER_CTR;
	       if (HieLvl >= Hie_DEG)
		  CtrCod = Deg_DB_GetCtrCodOfDegreeByCod (DegCod);
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
	       if (HieLvl >= Hie_CTR)
		  InsCod = Ctr_DB_GetInsCodOfCenterByCod (CtrCod);
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
	    if (LogoFound)
	      {
	       if (asprintf (&URL,"%s/%s/%02u/%u/logo",
			     Cfg_URL_SWAD_PUBLIC,Folder,
			     (unsigned) (HieCod % 100),
			     (unsigned) HieCod) < 0)
		  Err_NotEnoughMemoryExit ();
	       if (asprintf (&Icon,"%u.png",(unsigned) HieCod) < 0)
		  Err_NotEnoughMemoryExit ();

	       HTM_IMG (URL,Icon,Node->FullName,
			"class=\"%s\"",IconClass);
	       free (Icon);
	       free (URL);
	      }
	    else
	       HTM_IMG (Cfg_URL_ICON_PUBLIC,Hie_Icons[HieLvl],Node->ShrtName,
			"class=\"%s ICO_%s_%s\"",
			IconClass,
			Ico_GetPreffix (Ico_BLACK),The_GetSuffix ());
	   }
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/************* Put an icon to go to the action used to request ***************/
/************* the logo of institution, center or degree       ***************/
/*****************************************************************************/

void Lgo_PutIconToChangeLogo (Hie_Level_t HieLvl)
  {
   static Act_Action_t Action[Hie_NUM_LEVELS] =
     {
      [Hie_UNK] = ActUnk,
      [Hie_SYS] = ActUnk,
      [Hie_CTY] = ActUnk,
      [Hie_INS] = ActReqInsLog,
      [Hie_CTR] = ActReqCtrLog,
      [Hie_DEG] = ActReqDegLog,
      [Hie_CRS] = ActUnk,
     };

   Lay_PutContextualLinkOnlyIcon (Action[HieLvl],NULL,
                                  NULL,NULL,
				  "shield-alt.svg",Ico_BLACK);
  }

/*****************************************************************************/
/**** Show a form for sending a logo of the institution, center or degree ****/
/*****************************************************************************/

void Lgo_RequestLogo (Hie_Level_t HieLvl)
  {
   extern const char *Txt_Logo;
   extern const char *Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y;
   extern const char *Txt_File_with_the_logo;
   static Act_Action_t ActionRec[Hie_NUM_LEVELS] =
     {
      [Hie_INS] = ActRecInsLog,
      [Hie_CTR] = ActRecCtrLog,
      [Hie_DEG] = ActRecDegLog,
     };
   static void (*FunctionToDrawContextualIcons[Hie_NUM_LEVELS]) (void *Args) =
     {
      [Hie_INS] = Lgo_PutIconToRemoveLogoIns,
      [Hie_CTR] = Lgo_PutIconToRemoveLogoCtr,
      [Hie_DEG] = Lgo_PutIconToRemoveLogoDeg,
     };
   long HieCod = Gbl.Hierarchy.Node[HieLvl].HieCod;
   char PathLogo[PATH_MAX + 1];

   /***** Check if logo exists *****/
   snprintf (PathLogo,sizeof (PathLogo),"%s/%s/%02u/%u/logo/%u.png",
	     Cfg_PATH_SWAD_PUBLIC,Lgo_Folder[HieLvl],
	     (unsigned) (HieCod % 100),
	     (unsigned)  HieCod,
	     (unsigned)  HieCod);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Logo,
	         Fil_CheckIfPathExists (PathLogo) ? FunctionToDrawContextualIcons[HieLvl] :
						    NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);

      /***** Begin form to upload logo *****/
      Frm_BeginForm (ActionRec[HieLvl]);

	 /***** Write help message *****/
	 Ale_ShowAlert (Ale_INFO,Txt_You_can_send_a_file_with_an_image_in_PNG_format_transparent_background_and_size_X_Y,
			64,64);

	 /***** Upload logo *****/
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_Txt (Txt_File_with_the_logo);
	    HTM_Colon ();
	    HTM_NBSP ();
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

void Lgo_ReceiveLogo (Hie_Level_t HieLvl)
  {
   extern const char *Txt_The_file_is_not_X;
   long HieCod = Gbl.Hierarchy.Node[HieLvl].HieCod;
   char Path[PATH_MAX + 1];
   struct Par_Param *Par;
   char FileNameLogoSrc[PATH_MAX + 1];
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char FileNameLogo[PATH_MAX + 1];	// Full name (including path and .png) of the destination file
   bool WrongType = false;

   /***** Creates directories if not exist *****/
   snprintf (Path,sizeof (Path),"%s/%s",
	     Cfg_PATH_SWAD_PUBLIC,Lgo_Folder[HieLvl]);
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),"%s/%s/%02u",
	     Cfg_PATH_SWAD_PUBLIC,Lgo_Folder[HieLvl],
	     (unsigned) (HieCod % 100));
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),"%s/%s/%02u/%u",
	     Cfg_PATH_SWAD_PUBLIC,Lgo_Folder[HieLvl],
	     (unsigned) (HieCod % 100),
	     (unsigned)  HieCod);
   Fil_CreateDirIfNotExists (Path);
   snprintf (Path,sizeof (Path),"%s/%s/%02u/%u/logo",
	     Cfg_PATH_SWAD_PUBLIC,Lgo_Folder[HieLvl],
	     (unsigned) (HieCod % 100),
	     (unsigned)  HieCod);
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
	        Cfg_PATH_SWAD_PUBLIC,Lgo_Folder[HieLvl],
	        (unsigned) (HieCod % 100),
	        (unsigned)  HieCod,
	        (unsigned)  HieCod);
      if (!Fil_EndReceptionOfFile (FileNameLogo,Par))
	 Ale_ShowAlert (Ale_ERROR,"Error copying file.");
     }
  }

/*****************************************************************************/
/******* Remove the logo of the current institution, center or degree ********/
/*****************************************************************************/

void Lgo_RemoveLogo (Hie_Level_t HieLvl)
  {
   long HieCod = Gbl.Hierarchy.Node[HieLvl].HieCod;
   char FileNameLogo[PATH_MAX + 1];	// Full name (including path and .png) of the destination file

   /***** Remove logo *****/
   snprintf (FileNameLogo,sizeof (FileNameLogo),"%s/%s/%02u/%u/logo/%u.png",
	     Cfg_PATH_SWAD_PUBLIC,Lgo_Folder[HieLvl],
	     (unsigned) (HieCod % 100),
	     (unsigned)  HieCod,
	     (unsigned)  HieCod);
   Fil_RemoveTree (FileNameLogo);
  }
