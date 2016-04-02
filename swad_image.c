// swad_image.c: processing of image uploaded in a form

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <stdbool.h>		// For boolean type
#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <unistd.h>		// For unlink

#include "swad_config.h"
#include "swad_global.h"
#include "swad_file.h"
#include "swad_file_browser.h"
#include "swad_image.h"

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

static void Img_ProcessImage (const char *FileNameImgOriginal,
                              const char *FileNameImgProcessed,
                              unsigned Width,unsigned Height,unsigned Quality);

/*****************************************************************************/
/****************** Get image of a test question from form *******************/
/*****************************************************************************/
// Return true if image is created

void Img_GetImageFromForm (unsigned Width,unsigned Height,unsigned Quality)
  {
   struct Param *Param;
   char FileNameImgSrc[PATH_MAX+1];
   char *PtrExtension;
   size_t LengthExtension;
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE+1];
   char PathImgPriv[PATH_MAX+1];
   char FileNameImgOrig[PATH_MAX+1];	// Full name of original uploaded file
   char FileNameImgTmp[PATH_MAX+1];	// Full name of temporary processed file
   bool WrongType = false;

   /***** Reset image status *****/
   Gbl.Image.Status = Img_NONE;

   /***** Get filename and MIME type *****/
   Param = Fil_StartReceptionOfFile (FileNameImgSrc,MIMEType);
   if (!FileNameImgSrc[0])	// No file present
      return;

   /* Get filename extension */
   if ((PtrExtension = strrchr (FileNameImgSrc,(int) '.')) == NULL)
     {
      Lay_ShowAlert (Lay_WARNING,"El tipo de archivo no es correcto.");	// TODO: Need translation!!!!
      return;
     }   LengthExtension = strlen (PtrExtension);
   if (LengthExtension < Fil_MIN_LENGTH_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_LENGTH_FILE_EXTENSION)
     {
      Lay_ShowAlert (Lay_WARNING,"El tipo de archivo no es correcto.");	// TODO: Need translation!!!!
      return;
     }
   /* Check if the file type is image/ or application/octet-stream */
   if (strncmp (MIMEType,"image/",strlen ("image/")))
      if (strcmp (MIMEType,"application/octet-stream"))
	 if (strcmp (MIMEType,"application/octetstream"))
	    if (strcmp (MIMEType,"application/octet"))
	       WrongType = true;
   if (WrongType)
     {
      Lay_ShowAlert (Lay_WARNING,"El tipo de archivo no es correcto.");	// TODO: Need translation!!!!
      return;
     }

   /***** Assign a unique name for the image *****/
   strcpy (Gbl.Test.Image,Gbl.UniqueNameEncrypted);

   /***** Create private directories if not exist *****/
   /* Create private directory for images if it does not exist */
   sprintf (PathImgPriv,"%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /* Create temporary private directory for images if it does not exist */
   sprintf (PathImgPriv,"%s/%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /***** End the reception of original not processed image
          (it can be very big) into a temporary file *****/
   sprintf (FileNameImgOrig,"%s/%s/%s/%s_original.%s",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
            Gbl.Test.Image,PtrExtension);
   if (Fil_EndReceptionOfFile (FileNameImgOrig,Param))	// Success
     {
      Gbl.Image.Status = Img_FILE_RECEIVED;

      /***** Convert original image to temporary JPEG processed file *****/
      sprintf (FileNameImgTmp,"%s/%s/%s/%s.jpg",
	       Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
	       Gbl.Test.Image);

      /* Call to program that makes the conversion */
      Img_ProcessImage (FileNameImgOrig,FileNameImgTmp,Width,Height,Quality);
      Gbl.Image.Status = Img_FILE_PROCESSED;

      /***** Remove temporary file *****/
      unlink (FileNameImgOrig);
     }
  }

/*****************************************************************************/
/************ Process original image generating processed image **************/
/*****************************************************************************/

static void Img_ProcessImage (const char *FileNameImgOriginal,
                              const char *FileNameImgProcessed,
                              unsigned Width,unsigned Height,unsigned Quality)
  {
   char Command[1024+PATH_MAX*2];
   int ReturnCode;

   sprintf (Command,"convert %s -resize '%ux%u>' -quality %u %s",
            FileNameImgOriginal,
            Width,Height,Quality,
            FileNameImgProcessed);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running command to process image.");

   /***** Write message depending on return code *****/
   ReturnCode = WEXITSTATUS(ReturnCode);
   if (ReturnCode != 0)
     {
      sprintf (Gbl.Message,"Image could not be processed successfully.<br />"
			   "Error code returned by the program of processing: %d",
	       ReturnCode);
      Lay_ShowErrorAndExit (Gbl.Message);
     }
  }

/*****************************************************************************/
/**** Move temporary processed image file to definitive private directory ****/
/*****************************************************************************/

void Img_MoveImageToDefinitiveDirectory (void)
  {
   char PathImgPriv[PATH_MAX+1];
   char FileNameImgTmp[PATH_MAX+1];	// Full name of temporary processed file
   char FileNameImg[PATH_MAX+1];	// Full name of definitive processed file

   /***** Create subdirectory if it does not exist *****/
   sprintf (PathImgPriv,"%s/%s/%c%c",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	    Gbl.Test.Image[0],
	    Gbl.Test.Image[1]);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /***** Temporary processed file *****/
   sprintf (FileNameImgTmp,"%s/%s/%s/%s.jpg",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
	    Gbl.Test.Image);

   /***** Definitive processed file *****/
   sprintf (FileNameImg,"%s/%s/%c%c/%s.jpg",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	    Gbl.Test.Image[0],
	    Gbl.Test.Image[1],
	    Gbl.Test.Image);

   /***** Move file *****/
   if (rename (FileNameImgTmp,FileNameImg))	// Fail
      Lay_ShowAlert (Lay_ERROR,"Can not move file.");
   else						// Success
      Gbl.Image.Status = Img_FILE_MOVED;
  }

/*****************************************************************************/
/******************** Write the image of a test question *********************/
/*****************************************************************************/

void Img_ShowImage (const char *Image,const char *ClassImg)
  {
   char FileNameImgPriv[PATH_MAX+1];
   char FullPathImgPriv[PATH_MAX+1];
   char URL[PATH_MAX+1];

   /***** If no image to show ==> nothing to do *****/
   if (!Image)
      return;
   if (!Image[0])
      return;
   if (Gbl.Image.Status != Img_NAME_STORED_IN_DB)
      return;

   /***** Create a temporary public directory
	  used to download the zip file *****/
   Brw_CreateDirDownloadTmp ();

   /***** Build private path to image *****/
   sprintf (FileNameImgPriv,"%s.jpg",Image);
   sprintf (FullPathImgPriv,"%s/%s/%c%c/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	    Image[0],
	    Image[1],
	    FileNameImgPriv);

   /***** Create symbolic link from temporary public directory to private file
          in order to gain access to it for showing/downloading *****/
   Brw_CreateTmpPublicLinkToPrivateFile (FullPathImgPriv,FileNameImgPriv);

   /***** Create URL pointing to symbolic link *****/
   sprintf (URL,"%s/%s/%s/%s",
	    Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
	    Gbl.FileBrowser.TmpPubDir,
	    FileNameImgPriv);

   /***** Show image *****/
   fprintf (Gbl.F.Out,"<img src=\"%s\" alt=\"\" class=\"%s\"/>"
	              "<br />",
            URL,ClassImg);
  }

