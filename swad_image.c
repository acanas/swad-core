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
/*************************** Reset image fields ******************************/
/*****************************************************************************/
// Every struct Image must be initialized with this function Img_ImageConstructor after it is declared
// Every call to Img_ImageConstructor must have a call to Img_ImageDestructor

void Img_ImageConstructor (struct Image *Image)
  {
   Img_ResetImageExceptTitle (Image);
   Image->Title = NULL;
  }

/*****************************************************************************/
/********************* Reset image fields except title ***********************/
/*****************************************************************************/

void Img_ResetImageExceptTitle (struct Image *Image)
  {
   Image->Action = Img_ACTION_NO_IMAGE;
   Image->Status = Img_FILE_NONE;
   Image->Name[0] = '\0';
  }

/*****************************************************************************/
/******************************** Free image *********************************/
/*****************************************************************************/
// Every call to Img_ImageConstructor must have a call to Img_ImageDestructor

void Img_ImageDestructor (struct Image *Image)
  {
   Img_FreeImageTitle (Image);
  }

/*****************************************************************************/
/*************************** Reset image title *******************************/
/*****************************************************************************/

void Img_FreeImageTitle (struct Image *Image)
  {
   // Image->Title must be initialized to NULL after declaration
   if (Image->Title)
     {
      free ((void *) Image->Title);
      Image->Title = NULL;
     }
  }

/*****************************************************************************/
/****** Get image name and title from a query result and copy to struct ******/
/*****************************************************************************/

void Img_GetImageNameAndTitleFromRow (const char *Name,const char *Title,
                                      struct Image *Image)
  {
   size_t Length;

   /***** Copy image name to struct *****/
   strncpy (Image->Name,Name,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
   Image->Name[Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64] = '\0';

   /***** Set status of image file *****/
   Image->Status = Image->Name[0] ? Img_NAME_STORED_IN_DB :
	                            Img_FILE_NONE;

   /***** Copy image title to struct *****/
   // Image->Title can be empty or filled with a previous title
   // If filled  ==> free it
   Img_FreeImageTitle (Image);

   if (Title[0])
     {
      /* Get and limit length of the title */
      Length = strlen (Title);
      if (Length > Img_MAX_BYTES_TITLE)
	 Length = Img_MAX_BYTES_TITLE;

      if ((Image->Title = (char *) malloc (Length+1)) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for image title.");
      strncpy (Image->Title,Title,Length);
      Image->Title[Length] = '\0';
     }
  }

/*****************************************************************************/
/***************************** Get image from form ***************************/
/*****************************************************************************/

void Img_GetImageFromForm (unsigned NumOpt,struct Image *Image,
                           void (*GetImageFromDB) (unsigned NumOpt,struct Image *Image),
                           const char *ParamAction,const char *ParamFile,const char *ParamTitle,
                           unsigned Width,unsigned Height,unsigned Quality)
  {
   char Title[Img_MAX_BYTES_TITLE+1];
   size_t Length;

   /***** First, get action and initialize image
          (except title, that will be get after the image file) *****/
   Image->Action = Img_GetImageActionFromForm (ParamAction);
   Image->Status = Img_FILE_NONE;
   Image->Name[0] = '\0';

   /***** Secondly, get the image name and the file *****/
   switch (Image->Action)
     {
      case Img_ACTION_NEW_IMAGE:	// Upload new image
         /***** Get new image (if present ==> process and create temporary file) *****/
	 Img_GetAndProcessImageFileFromForm (Image,ParamFile,
	                                     Width,Height,Quality);
	 if (Image->Status != Img_FILE_PROCESSED)	// No new image received-processed successfully
	   {
	    /* Reset image name */
	    Image->Status = Img_FILE_NONE;
	    Image->Name[0] = '\0';
	   }
	 break;
      case Img_ACTION_KEEP_IMAGE:	// Keep current image unchanged
	 /***** Get image name *****/
	 if (GetImageFromDB != NULL)
	    GetImageFromDB (NumOpt,Image);
	 break;
      case Img_ACTION_CHANGE_IMAGE:	// Replace old image by new image
         /***** Get new image (if present ==> process and create temporary file) *****/
	 Img_GetAndProcessImageFileFromForm (Image,ParamFile,
	                                     Width,Height,Quality);
	 if (Image->Status != Img_FILE_PROCESSED &&	// No new image received-processed successfully
	     GetImageFromDB != NULL)
	    /* Get image name */
	    GetImageFromDB (NumOpt,Image);
	 break;
      case Img_ACTION_NO_IMAGE:		// Do not use image (remove current image if exists)
         break;
     }

   /***** By last, get image title from form *****/
   Par_GetParToText (ParamTitle,Title,Img_MAX_BYTES_TITLE);
   /* If the title coming from the form is empty, keep current image title unchanged
      If not empty, copy it to current image title */
   if ((Length = strlen (Title)) > 0)
     {
      /* Overwrite current title (empty or coming from database)
         with the title coming from the form */
      Img_FreeImageTitle (Image);
      if ((Image->Title = (char *) malloc (Length + 1)) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for image title.");
      strncpy (Image->Title,Title,Length);
      Image->Title[Length] = '\0';
     }
  }

/*****************************************************************************/
/************************* Get image action from form ************************/
/*****************************************************************************/

Img_Action_t Img_GetImageActionFromForm (const char *ParamAction)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /***** Get parameter with the action to perform on image *****/
   Par_GetParToText (ParamAction,UnsignedStr,10);
   if (UnsignedStr[0])
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
	 if (UnsignedNum < Img_NUM_ACTIONS)
	    return (Img_Action_t) UnsignedNum;

   /***** Default action if none supplied *****/
   return Img_ACTION_NO_IMAGE;
  }

/*****************************************************************************/
/**************************** Get image from form ****************************/
/*****************************************************************************/

void Img_GetAndProcessImageFileFromForm (struct Image *Image,const char *ParamFile,
                                         unsigned Width,unsigned Height,unsigned Quality)
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

   /***** Rest image file status *****/
   Image->Status = Img_FILE_NONE;

   /***** Get filename and MIME type *****/
   Param = Fil_StartReceptionOfFile (ParamFile,FileNameImgSrc,MIMEType);
   if (!FileNameImgSrc[0])	// No file present
      return;

   /* Get filename extension */
   if ((PtrExtension = strrchr (FileNameImgSrc,(int) '.')) == NULL)
      return;

   LengthExtension = strlen (PtrExtension);
   if (LengthExtension < Fil_MIN_LENGTH_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_LENGTH_FILE_EXTENSION)
      return;

   /* Check if the file type is image/ or application/octet-stream */
   if (strncmp (MIMEType,"image/",strlen ("image/")))
      if (strcmp (MIMEType,"application/octet-stream"))
	 if (strcmp (MIMEType,"application/octetstream"))
	    if (strcmp (MIMEType,"application/octet"))
	       WrongType = true;
   if (WrongType)
      return;

   /***** Assign a unique name for the image *****/
   Cry_CreateUniqueNameEncrypted (Image->Name);

   /***** Create private directories if not exist *****/
   /* Create private directory for images if it does not exist */
   sprintf (PathImgPriv,"%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /* Create temporary private directory for images if it does not exist */
   sprintf (PathImgPriv,"%s/%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /***** Remove old temporary private files *****/
   Fil_RemoveOldTmpFiles (PathImgPriv,Cfg_TIME_TO_DELETE_IMAGES_TMP_FILES,false);

   /***** End the reception of original not processed image
          (it can be very big) into a temporary file *****/
   Image->Status = Img_FILE_NONE;
   sprintf (FileNameImgOrig,"%s/%s/%s/%s_original.%s",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
            Image->Name,PtrExtension);
   if (Fil_EndReceptionOfFile (FileNameImgOrig,Param))	// Success
     {
      Image->Status = Img_FILE_RECEIVED;

      /***** Convert original image to temporary JPEG processed file
             by calling to program that makes the conversion *****/
      sprintf (FileNameImgTmp,"%s/%s/%s/%s.jpg",
	       Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
	       Image->Name);
      Img_ProcessImage (FileNameImgOrig,FileNameImgTmp,Width,Height,Quality);
      Image->Status = Img_FILE_PROCESSED;

      /***** Remove temporary original file *****/
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

void Img_MoveImageToDefinitiveDirectory (struct Image *Image)
  {
   char PathImgPriv[PATH_MAX+1];
   char FileNameImgTmp[PATH_MAX+1];	// Full name of temporary processed file
   char FileNameImg[PATH_MAX+1];	// Full name of definitive processed file

   /***** Create subdirectory if it does not exist *****/
   sprintf (PathImgPriv,"%s/%s/%c%c",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	    Image->Name[0],
	    Image->Name[1]);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /***** Temporary processed file *****/
   sprintf (FileNameImgTmp,"%s/%s/%s/%s.jpg",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
	    Image->Name);

   /***** Definitive processed file *****/
   sprintf (FileNameImg,"%s/%s/%c%c/%s.jpg",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	    Image->Name[0],
	    Image->Name[1],
	    Image->Name);

   /***** Move file *****/
   if (rename (FileNameImgTmp,FileNameImg))	// Fail
      Lay_ShowAlert (Lay_ERROR,"Can not move file.");
   else						// Success
      Image->Status = Img_FILE_MOVED;
  }

/*****************************************************************************/
/******************** Write the image of a test question *********************/
/*****************************************************************************/

void Img_ShowImage (struct Image *Image,const char *ClassImg)
  {
   extern const char *Txt_Image_not_found;
   char FileNameImgPriv[PATH_MAX+1];
   char FullPathImgPriv[PATH_MAX+1];
   char URL[PATH_MAX+1];

   /***** If no image to show ==> nothing to do *****/
   if (!Image->Name)
      return;
   if (!Image->Name[0])
      return;
   if (Image->Status != Img_NAME_STORED_IN_DB)
      return;

   /***** Create a temporary public directory used to show the image *****/
   Brw_CreateDirDownloadTmp ();

   /***** Build private path to image *****/
   sprintf (FileNameImgPriv,"%s.jpg",Image->Name);
   sprintf (FullPathImgPriv,"%s/%s/%c%c/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	    Image->Name[0],
	    Image->Name[1],
	    FileNameImgPriv);

   /***** Check if private image file exists *****/
   if (Fil_CheckIfPathExists (FullPathImgPriv))
     {
      /***** Create symbolic link from temporary public directory to private file
	     in order to gain access to it for showing/downloading *****/
      Brw_CreateTmpPublicLinkToPrivateFile (FullPathImgPriv,FileNameImgPriv);

      /***** Create URL pointing to symbolic link *****/
      sprintf (URL,"%s/%s/%s/%s",
	       Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
	       Gbl.FileBrowser.TmpPubDir,
	       FileNameImgPriv);

      /***** Show image *****/
      fprintf (Gbl.F.Out,"<div>"
			 "<img src=\"%s\" class=\"%s\" alt=\"\"",
	       URL,ClassImg);
      if (Image->Title)
         if (Image->Title[0])
	    fprintf (Gbl.F.Out," title=\"%s\"",Image->Title);
      fprintf (Gbl.F.Out," />"
			 "</div>");
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_Image_not_found);
  }

/*****************************************************************************/
/** Remove private file with an image, given the image name (without .jpg) ***/
/*****************************************************************************/

void Img_RemoveImageFile (const char *ImageName)
  {
   char FullPathImgPriv[PATH_MAX+1];

   if (ImageName[0])
     {
      /***** Build path to private file *****/
      sprintf (FullPathImgPriv,"%s/%s/%c%c/%s.jpg",
	       Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	       ImageName[0],
	       ImageName[1],
	       ImageName);

      /***** Remove private file *****/
      unlink (FullPathImgPriv);

      // Public links are removed automatically after a period
     }
  }
