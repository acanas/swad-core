// swad_image.c: processing of image uploaded in a form

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_config.h"
#include "swad_global.h"
#include "swad_file.h"
#include "swad_file_browser.h"
#include "swad_form.h"
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

static void Img_ProcessImage (struct Image *Image,
                              const char *FileNameImgOriginal,
                              const char *FileNameImgProcessed);

/*****************************************************************************/
/*************************** Reset image fields ******************************/
/*****************************************************************************/
// Every struct Image must be initialized with this function Img_ImageConstructor after it is declared
// Every call to Img_ImageConstructor must have a call to Img_ImageDestructor

void Img_ImageConstructor (struct Image *Image)
  {
   Img_ResetImageExceptTitleAndURL (Image);
   Image->Title = NULL;
   Image->URL   = NULL;
  }

/*****************************************************************************/
/***************** Reset image fields except title and URL *******************/
/*****************************************************************************/

void Img_ResetImageExceptTitleAndURL (struct Image *Image)
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
   Img_FreeImageURL (Image);
  }

/*****************************************************************************/
/****************************** Free image title *****************************/
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
/******************************* Free image URL ******************************/
/*****************************************************************************/

void Img_FreeImageURL (struct Image *Image)
  {
   // Image->URL must be initialized to NULL after declaration
   if (Image->URL)
     {
      free ((void *) Image->URL);
      Image->URL = NULL;
     }
  }

/*****************************************************************************/
/**** Get image name, title and URL from a query result and copy to struct ***/
/*****************************************************************************/

void Img_GetImageNameTitleAndURLFromRow (const char *Name,
                                         const char *Title,
                                         const char *URL,
                                         struct Image *Image)
  {
   size_t Length;

   /***** Copy image name to struct *****/
   Str_Copy (Image->Name,Name,
             Img_BYTES_NAME);

   /***** Set status of image file *****/
   Image->Status = Image->Name[0] ? Img_NAME_STORED_IN_DB :
	                            Img_FILE_NONE;


   /***** Copy image title to struct *****/
   // Image->Title can be empty or filled with previous value
   // If filled  ==> free it
   Img_FreeImageTitle (Image);
   if (Title[0])
     {
      /* Get and limit length of the title */
      Length = strlen (Title);
      if (Length > Img_MAX_BYTES_TITLE)
	  Length = Img_MAX_BYTES_TITLE;

      if ((Image->Title = (char *) malloc (Length + 1)) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for image title.");
      Str_Copy (Image->Title,Title,
                Length);
     }

   /***** Copy image URL to struct *****/
   // Image->URL can be empty or filled with previous value
   // If filled  ==> free it
   Img_FreeImageURL (Image);
   if (URL[0])
     {
      /* Get and limit length of the URL */
      Length = strlen (URL);
      if (Length > Img_MAX_BYTES_TITLE)
	  Length = Img_MAX_BYTES_TITLE;

      if ((Image->URL = (char *) malloc (Length + 1)) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for image URL.");
      Str_Copy (Image->URL,URL,
                Length);
     }
  }

/*****************************************************************************/
/************ Draw input fields to upload an image inside a form *************/
/*****************************************************************************/

void Img_PutImageUploader (int NumImgInForm,const char *ClassImgTitURL)
  {
   extern const char *Txt_Image;
   extern const char *Txt_optional;
   extern const char *Txt_Image_title_attribution;
   extern const char *Txt_Link;
   struct ParamUploadImg ParamUploadImg;
   char Id[Frm_MAX_BYTES_ID + 1];

   /***** Set names of parameters depending on number of image in form *****/
   Img_SetParamNames (&ParamUploadImg,NumImgInForm);

   /***** Create unique id for this image uploader *****/
   Frm_SetUniqueId (Id);

   /***** Start container *****/
   fprintf (Gbl.F.Out,"<div class=\"IMG_UPLOAD_CONTAINER\">");

   /***** Action to perform on image *****/
   Par_PutHiddenParamUnsigned (ParamUploadImg.Action,(unsigned) Img_ACTION_NEW_IMAGE);

   /***** Image file *****/
   fprintf (Gbl.F.Out,"<label class=\"IMG_UPLOAD_BUTTON\">"
	              "<img src=\"%s/photo64x64.gif\""
	              " alt=\"%s\" title=\"%s (%s)\""
	              " class=\"IMG_UPLOAD_ICO\" />"
	              "<input type=\"file\" name=\"%s\" accept=\"image/*\""
	              " class=\"IMG_UPLOAD_FILE\""
	              " onchange=\"imageUploadOnSelectFile (this,'%s');\" />"
                      "<span id=\"%s_fil\" class=\"IMG_UPLOAD_FILENAME\" />"
                      "</span>"
	              "</label>",
            Gbl.Prefs.IconsURL,
            Txt_Image,Txt_Image,Txt_optional,
            ParamUploadImg.File,
            Id,Id);

   /***** Image title/attribution and URL *****/
   fprintf (Gbl.F.Out,"<div id=\"%s_tit_url\" style=\"display:none;\">",
            Id);
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"%s\""
                      " placeholder=\"%s (%s)\""
                      " class=\"%s\" maxlength=\"%u\" value=\"\" />",
            ParamUploadImg.Title,
            Txt_Image_title_attribution,Txt_optional,
            ClassImgTitURL,Img_MAX_CHARS_TITLE);
   fprintf (Gbl.F.Out,"<br />"
                      "<input type=\"url\" name=\"%s\""
                      " placeholder=\"%s (%s)\""
                      " class=\"%s\" maxlength=\"%u\" value=\"\" />",
            ParamUploadImg.URL,
            Txt_Link,Txt_optional,
            ClassImgTitURL,Cns_MAX_CHARS_WWW);
   fprintf (Gbl.F.Out,"</div>");

   /***** End container *****/
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/***************************** Get image from form ***************************/
/*****************************************************************************/
// If NumImgInForm < 0, params have no suffix
// If NumImgInForm >= 0, the number is a suffix of the params

void Img_GetImageFromForm (int NumImgInForm,struct Image *Image,
                           void (*GetImageFromDB) (int NumImgInForm,struct Image *Image))
  {
   struct ParamUploadImg ParamUploadImg;
   char Title[Img_MAX_BYTES_TITLE + 1];
   char URL[Cns_MAX_BYTES_WWW + 1];
   size_t Length;

   /***** Set names of parameters depending on number of image in form *****/
   Img_SetParamNames (&ParamUploadImg,NumImgInForm);

   /***** First, get action and initialize image
          (except title, that will be get after the image file) *****/
   Image->Action = Img_GetImageActionFromForm (ParamUploadImg.Action);
   Image->Status = Img_FILE_NONE;
   Image->Name[0] = '\0';

   /***** Secondly, get the image name and the file *****/
   switch (Image->Action)
     {
      case Img_ACTION_NEW_IMAGE:	// Upload new image
         /***** Get new image (if present ==> process and create temporary file) *****/
	 Img_GetAndProcessImageFileFromForm (Image,ParamUploadImg.File);
	 switch (Image->Status)
	   {
	    case Img_FILE_NONE:		// No new image received
	       Image->Action = Img_ACTION_NO_IMAGE;
               Image->Name[0] = '\0';
	       break;
	    case Img_FILE_RECEIVED:	// New image received, but not processed
	       Image->Status = Img_FILE_NONE;
	       Image->Name[0] = '\0';
	       break;
	    default:
	       break;
	   }
	 break;
      case Img_ACTION_KEEP_IMAGE:	// Keep current image unchanged
	 /***** Get image name *****/
	 if (GetImageFromDB != NULL)
	    GetImageFromDB (NumImgInForm,Image);
	 break;
      case Img_ACTION_CHANGE_IMAGE:	// Replace old image by new image
         /***** Get new image (if present ==> process and create temporary file) *****/
	 Img_GetAndProcessImageFileFromForm (Image,ParamUploadImg.File);
	 if (Image->Status != Img_FILE_PROCESSED &&	// No new image received-processed successfully
	     GetImageFromDB != NULL)
	    /* Get image name */
	    GetImageFromDB (NumImgInForm,Image);
	 break;
      case Img_ACTION_NO_IMAGE:		// Do not use image (remove current image if exists)
         break;
     }

   /***** Third, get image title from form *****/
   Par_GetParToText (ParamUploadImg.Title,Title,Img_MAX_BYTES_TITLE);
   /* If the title coming from the form is empty, keep current image title unchanged
      If not empty, copy it to current image title */
   if ((Length = strlen (Title)) > 0)
     {
      /* Overwrite current title (empty or coming from database)
         with the title coming from the form */
      Img_FreeImageTitle (Image);
      if ((Image->Title = (char *) malloc (Length + 1)) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for image title.");
      Str_Copy (Image->Title,Title,
                Length);
     }

   /***** By last, get image URL from form *****/
   Par_GetParToText (ParamUploadImg.URL,URL,Cns_MAX_BYTES_WWW);
   /* If the URL coming from the form is empty, keep current image URL unchanged
      If not empty, copy it to current image URL */
   if ((Length = strlen (URL)) > 0)
     {
      /* Overwrite current URL (empty or coming from database)
         with the URL coming from the form */
      Img_FreeImageURL (Image);
      if ((Image->URL = (char *) malloc (Length + 1)) == NULL)
	 Lay_ShowErrorAndExit ("Error allocating memory for image URL.");
      Str_Copy (Image->URL,URL,
                Length);
     }
  }

/*****************************************************************************/
/********* Set parameters names depending on number of image in form *********/
/*****************************************************************************/
// If NumImgInForm <  0, params have no suffix
// If NumImgInForm >= 0, the number is a suffix of the params

void Img_SetParamNames (struct ParamUploadImg *ParamUploadImg,int NumImgInForm)
  {
   if (NumImgInForm < 0)	// One unique image in form ==> no suffix needed
     {
      Str_Copy (ParamUploadImg->Action,"ImgAct",
                Img_MAX_BYTES_PARAM_UPLOAD_IMG);
      Str_Copy (ParamUploadImg->File  ,"ImgFil",
                Img_MAX_BYTES_PARAM_UPLOAD_IMG);
      Str_Copy (ParamUploadImg->Title ,"ImgTit",
                Img_MAX_BYTES_PARAM_UPLOAD_IMG);
      Str_Copy (ParamUploadImg->URL   ,"ImgURL",
                Img_MAX_BYTES_PARAM_UPLOAD_IMG);
     }
   else				// Several images in form ==> add suffix
     {
      snprintf (ParamUploadImg->Action,sizeof (ParamUploadImg->Action),
	        "ImgAct%u",
		NumImgInForm);
      snprintf (ParamUploadImg->File  ,sizeof (ParamUploadImg->File),
	        "ImgFil%u",
		NumImgInForm);
      snprintf (ParamUploadImg->Title ,sizeof (ParamUploadImg->Title),
	        "ImgTit%u",
		NumImgInForm);
      snprintf (ParamUploadImg->URL   ,sizeof (ParamUploadImg->URL),
	        "ImgURL%u",
		NumImgInForm);
     }
  }

/*****************************************************************************/
/************************* Get image action from form ************************/
/*****************************************************************************/

Img_Action_t Img_GetImageActionFromForm (const char *ParamAction)
  {
   /***** Get parameter with the action to perform on image *****/
   return (Img_Action_t)
	  Par_GetParToUnsignedLong (ParamAction,
                                    0,
                                    Img_NUM_ACTIONS - 1,
                                    (unsigned long) Img_ACTION_DEFAULT);
  }

/*****************************************************************************/
/**************************** Get image from form ****************************/
/*****************************************************************************/

void Img_GetAndProcessImageFileFromForm (struct Image *Image,const char *ParamFile)
  {
   struct Param *Param;
   char FileNameImgSrc[PATH_MAX + 1];
   char *PtrExtension;
   size_t LengthExtension;
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char PathImgPriv[PATH_MAX + 1];
   char FileNameImgOrig[PATH_MAX + 1];	// Full name of original uploaded file
   char FileNameImgTmp[PATH_MAX + 1];	// Full name of temporary processed file
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
   if (LengthExtension < Fil_MIN_BYTES_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_BYTES_FILE_EXTENSION)
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
   snprintf (PathImgPriv,sizeof (PathImgPriv),
	     "%s/%s",
	     Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /* Create temporary private directory for images if it does not exist */
   snprintf (PathImgPriv,sizeof (PathImgPriv),
	     "%s/%s/%s",
	     Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /***** Remove old temporary private files *****/
   Fil_RemoveOldTmpFiles (PathImgPriv,Cfg_TIME_TO_DELETE_IMAGES_TMP_FILES,false);

   /***** End the reception of original not processed image
          (it can be very big) into a temporary file *****/
   Image->Status = Img_FILE_NONE;
   snprintf (FileNameImgOrig,sizeof (FileNameImgOrig),
	     "%s/%s/%s/%s_original.%s",
             Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
             Image->Name,PtrExtension);
   if (Fil_EndReceptionOfFile (FileNameImgOrig,Param))	// Success
     {
      Image->Status = Img_FILE_RECEIVED;

      /***** Convert original image to temporary JPEG processed file
             by calling to program that makes the conversion *****/
      snprintf (FileNameImgTmp,sizeof (FileNameImgTmp),
	        "%s/%s/%s/%s.jpg",
	        Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
	        Image->Name);
      Img_ProcessImage (Image,FileNameImgOrig,FileNameImgTmp);
      Image->Status = Img_FILE_PROCESSED;

      /***** Remove temporary original file *****/
      unlink (FileNameImgOrig);
     }
  }

/*****************************************************************************/
/************ Process original image generating processed image **************/
/*****************************************************************************/

static void Img_ProcessImage (struct Image *Image,
                              const char *FileNameImgOriginal,
                              const char *FileNameImgProcessed)
  {
   char Command[1024 + PATH_MAX * 2];
   int ReturnCode;

   snprintf (Command,sizeof (Command),
	     "convert %s -resize '%ux%u>' -quality %u %s",
             FileNameImgOriginal,
             Image->Width,
             Image->Height,
             Image->Quality,
             FileNameImgProcessed);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running command to process image.");

   /***** Write message depending on return code *****/
   ReturnCode = WEXITSTATUS(ReturnCode);
   if (ReturnCode != 0)
     {
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        "Image could not be processed successfully.<br />"
		"Error code returned by the program of processing: %d",
	        ReturnCode);
      Lay_ShowErrorAndExit (Gbl.Alert.Txt);
     }
  }

/*****************************************************************************/
/**** Move temporary processed image file to definitive private directory ****/
/*****************************************************************************/

void Img_MoveImageToDefinitiveDirectory (struct Image *Image)
  {
   char PathImgPriv[PATH_MAX + 1];
   char FileNameImgTmp[PATH_MAX + 1];	// Full name of temporary processed file
   char FileNameImg[PATH_MAX + 1];	// Full name of definitive processed file

   /***** Create subdirectory if it does not exist *****/
   snprintf (PathImgPriv,sizeof (PathImgPriv),
	     "%s/%s/%c%c",
	     Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	     Image->Name[0],
	     Image->Name[1]);
   Fil_CreateDirIfNotExists (PathImgPriv);

   /***** Temporary processed file *****/
   snprintf (FileNameImgTmp,sizeof (FileNameImgTmp),
	     "%s/%s/%s/%s.jpg",
	     Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,Cfg_FOLDER_IMG_TMP,
	     Image->Name);

   /***** Definitive processed file *****/
   snprintf (FileNameImg,sizeof (FileNameImg),
	     "%s/%s/%c%c/%s.jpg",
	     Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	     Image->Name[0],
	     Image->Name[1],
	     Image->Name);

   /***** Move file *****/
   if (rename (FileNameImgTmp,FileNameImg))	// Fail
      Ale_ShowAlert (Ale_ERROR,"Can not move file.");
   else						// Success
      Image->Status = Img_FILE_MOVED;
  }

/*****************************************************************************/
/******************** Write the image of a test question *********************/
/*****************************************************************************/

void Img_ShowImage (struct Image *Image,
                    const char *ClassContainer,const char *ClassImg)
  {
   extern const char *Txt_Image_not_found;
   char FileNameImgPriv[PATH_MAX + 1];
   char FullPathImgPriv[PATH_MAX + 1];
   char URL[PATH_MAX + 1];
   bool PutLink;

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
   snprintf (FileNameImgPriv,sizeof (FileNameImgPriv),
	     "%s.jpg",
	     Image->Name);
   snprintf (FullPathImgPriv,sizeof (FullPathImgPriv),
	     "%s/%s/%c%c/%s",
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
      snprintf (URL,sizeof (URL),
	        "%s/%s/%s/%s",
	        Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
	        Gbl.FileBrowser.TmpPubDir,
	        FileNameImgPriv);

      /***** Show image *****/
      /* Check if optional link is present */
      PutLink = false;
      if (Image->URL)
         if (Image->URL[0])
            PutLink = true;

      /* Start image container */
      fprintf (Gbl.F.Out,"<div class=\"%s\">",ClassContainer);

      /* Start optional link to external URL */
      if (PutLink)
         fprintf (Gbl.F.Out,"<a href=\"%s\" target=\"_blank\">",Image->URL);

      /* Image */
      fprintf (Gbl.F.Out,"<img src=\"%s\" class=\"%s\" alt=\"\"",URL,ClassImg);
      if (Image->Title)
         if (Image->Title[0])
	    fprintf (Gbl.F.Out," title=\"%s\"",Image->Title);
      fprintf (Gbl.F.Out," />");

      /* End optional link to external URL */
      if (PutLink)
         fprintf (Gbl.F.Out,"</a>");

      /* End image container */
      fprintf (Gbl.F.Out,"</div>");
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Image_not_found);
  }

/*****************************************************************************/
/** Remove private file with an image, given the image name (without .jpg) ***/
/*****************************************************************************/

void Img_RemoveImageFile (const char *ImageName)
  {
   char FullPathImgPriv[PATH_MAX + 1];

   if (ImageName[0])
     {
      /***** Build path to private file *****/
      snprintf (FullPathImgPriv,sizeof (FullPathImgPriv),
	        "%s/%s/%c%c/%s.jpg",
	        Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_IMG,
	        ImageName[0],
	        ImageName[1],
	        ImageName);

      /***** Remove private file *****/
      unlink (FullPathImgPriv);

      // Public links are removed automatically after a period
     }
  }
