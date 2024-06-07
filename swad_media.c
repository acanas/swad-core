// swad_media.c: processing of image/video uploaded in a form

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#define _GNU_SOURCE         	// For strcasestr, asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <sys/stat.h>		// For lstat
#include <sys/types.h>		// For lstat
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink, lstat

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_browser.h"
#include "swad_config.h"
#include "swad_cookie.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_file.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_media.h"
#include "swad_media_database.h"
#include "swad_parameter.h"

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Med_Extensions[Med_NUM_TYPES] =
  {
   [Med_TYPE_NONE] = "",
   [Med_JPG      ] = "jpg",
   [Med_GIF      ] = "gif",
   [Med_MP4      ] = "mp4",
   [Med_WEBM     ] = "webm",
   [Med_OGG      ] = "ogg",
   [Med_YOUTUBE  ] = "",
   [Med_EMBED    ] = "",
   };

#define Med_MAX_SIZE_GIF (5UL * 1024UL * 1024UL)	// 5 MiB
#define Med_MAX_SIZE_MP4 (5UL * 1024UL * 1024UL)	// 5 MiB

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Med_NUM_FORM_TYPES 4

typedef enum
  {
   Med_FORM_NONE    = 0,
   Med_FORM_FILE    = 1,
   Med_FORM_YOUTUBE = 2,
   Med_FORM_EMBED   = 3,
  } Med_FormType_t;

struct MediaUploader
  {
   Med_FormType_t FormType;
   const char *IconSuffix;
   const char *ParSuffix;
   const char *FunctionName;
   const char *Icon;
   const char *Title;
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Med_ResetMediaExceptURLAndTitle (struct Med_Media *Media);
static void Med_FreeMediaURL (struct Med_Media *Media);
static void Med_FreeMediaTitle (struct Med_Media *Media);

static void Med_PutIconMediaUploader (const char UniqueId[Frm_MAX_BYTES_ID + 1],
				      struct MediaUploader *MediaUploader);
static void Med_PutHiddenFormTypeMediaUploader (const char UniqueId[Frm_MAX_BYTES_ID + 1],
						struct MediaUploader *MediaUploader,
					        struct Med_ParUpload *ParUpload);

static Med_Action_t Med_GetMediaActionFromForm (const char *ParAction);
static Med_FormType_t Usr_GetFormTypeFromForm (struct Med_ParUpload *ParUpload);
static void Usr_GetURLFromForm (const char *ParName,struct Med_Media *Media);
static void Usr_GetTitleFromForm (const char *ParName,struct Med_Media *Media);
static void Med_GetAndProcessFileFromForm (const char *ParFile,
                                           struct Med_Media *Media);
static bool Med_DetectIfAnimated (struct Med_Media *Media,
			          const char PathFileOrg[PATH_MAX + 1]);

static void Med_ProcessJPG (struct Med_Media *Media,
			    const char PathFileOrg[PATH_MAX + 1]);
static void Med_ProcessGIF (struct Med_Media *Media,
			    const char PathFileOrg[PATH_MAX + 1]);
static void Med_ProcessVideo (struct Med_Media *Media,
			      const char PathFileOrg[PATH_MAX + 1]);

static int Med_ResizeImage (struct Med_Media *Media,
                            const char PathFileOriginal[PATH_MAX + 1],
                            const char PathFileProcessed[PATH_MAX + 1]);
static int Med_GetFirstFrame (const char PathFileOriginal[PATH_MAX + 1],
                              const char PathFileProcessed[PATH_MAX + 1]);

static void Med_GetAndProcessYouTubeFromForm (const char *ParURL,
                                              struct Med_Media *Media);
static void Med_GetAndProcessEmbedFromForm (const char *ParURL,
                                            struct Med_Media *Media);

static bool Med_MoveTmpFileToDefDir (struct Med_Media *Media,
				     const char PathMedPriv[PATH_MAX + 1],
				     const char *Extension);

static void Med_ShowMediaFile (const struct Med_Media *Media,const char *ClassMedia);
static void Med_ShowJPG (const struct Med_Media *Media,
			 const char PathMedPriv[PATH_MAX + 1],
			 const char *ClassMedia);
static void Med_ShowGIF (const struct Med_Media *Media,
			 const char PathMedPriv[PATH_MAX + 1],
			 const char *ClassMedia);
static void Med_ShowVideo (const struct Med_Media *Media,
			   const char PathMedPriv[PATH_MAX + 1],
			   const char *ClassMedia);
static void Med_ShowYoutube (const struct Med_Media *Media,const char *ClassMedia);
static void Med_ShowEmbed (const struct Med_Media *Media,const char *ClassMedia);
static void Med_AlertThirdPartyCookies (void);

static Med_Type_t Med_GetTypeFromExtAndMIME (const char *Extension,
                                             const char *MIMEType);

static void Med_ErrorProcessingMediaFile (void);

/*****************************************************************************/
/********************** Media (image/video) constructor **********************/
/*****************************************************************************/
// Every struct Med_Media must be initialized with this constructor function after it is declared
// Every call to constructor must have a call to destructor

void Med_MediaConstructor (struct Med_Media *Media)
  {
   Med_ResetMediaExceptURLAndTitle (Media);
   Media->URL   = NULL;
   Media->Title = NULL;
  }

/*****************************************************************************/
/********************** Media (image/video) destructor ***********************/
/*****************************************************************************/
// Every call to constructor must have a call to destructor

void Med_MediaDestructor (struct Med_Media *Media)
  {
   Med_ResetMedia (Media);
  }

/*****************************************************************************/
/********************* Reset media (image/video) fields **********************/
/*****************************************************************************/
// Media must be constructed before calling this function

void Med_ResetMedia (struct Med_Media *Media)
  {
   Med_ResetMediaExceptURLAndTitle (Media);
   Med_FreeMediaURL (Media);
   Med_FreeMediaTitle (Media);
  }

/*****************************************************************************/
/***************** Reset media fields except URL and title *******************/
/*****************************************************************************/

static void Med_ResetMediaExceptURLAndTitle (struct Med_Media *Media)
  {
   Media->MedCod  = -1L;
   Media->Action  = Med_ACTION_NO_MEDIA;
   Media->Status  = Med_STATUS_NONE;
   Media->Name[0] = '\0';
   Media->Type    = Med_TYPE_NONE;
  }

/*****************************************************************************/
/******************************* Free image URL ******************************/
/*****************************************************************************/

static void Med_FreeMediaURL (struct Med_Media *Media)
  {
   // Media->URL is initialized to NULL in constructor
   if (Media->URL)
     {
      free (Media->URL);
      Media->URL = NULL;
     }
  }

/*****************************************************************************/
/****************************** Free image title *****************************/
/*****************************************************************************/

static void Med_FreeMediaTitle (struct Med_Media *Media)
  {
   // Media->Title is initialized to NULL in constructor
   if (Media->Title)
     {
      free (Media->Title);
      Media->Title = NULL;
     }
  }

/*****************************************************************************/
/**** Get media name, title and URL from a query result and copy to struct ***/
/*****************************************************************************/

void Med_GetMediaDataByCod (struct Med_Media *Media)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRows;
   size_t Length;

   /***** Trivial check: media code should be > 0 *****/
   if (Media->MedCod <= 0)
     {
      Med_ResetMedia (Media);
      return;
     }

   /***** Get data of a media from database *****/
   NumRows = Med_DB_GetMediaDataByCod (&mysql_res,Media->MedCod);

   /***** Result should have a unique row *****/
   if (NumRows == 0)	// Media not found
      /***** Reset media data *****/
      Med_ResetMedia (Media);
   else if (NumRows == 1)
     {
      /***** Get row *****/
      row = mysql_fetch_row (mysql_res);

      /***** Convert type string (row[0]) to type *****/
      Media->Type = Med_DB_GetTypeFromStr (row[0]);

      /***** Set status of media file *****/
      Media->Status = (Media->Type != Med_TYPE_NONE) ? Med_STORED_IN_DB :
						       Med_STATUS_NONE;

      /***** Copy media name (row[1]) to struct *****/
      Str_Copy (Media->Name,row[1],sizeof (Media->Name) - 1);

      /***** Copy media URL (row[2]) to struct *****/
      // Media->URL can be empty or filled with previous value
      // If filled  ==> free it
      Med_FreeMediaURL (Media);
      if (row[2][0])
	{
	 /* Get and limit length of the URL */
	 Length = strlen (row[2]);
	 if (Length > WWW_MAX_BYTES_WWW)
	     Length = WWW_MAX_BYTES_WWW;

	 if ((Media->URL = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
	 Str_Copy (Media->URL,row[2],Length);
	}

      /***** Copy media title (row[3]) to struct *****/
      // Media->Title can be empty or filled with previous value
      // If filled  ==> free it
      Med_FreeMediaTitle (Media);
      if (row[3][0])
	{
	 /* Get and limit length of the title */
	 Length = strlen (row[3]);
	 if (Length > Med_MAX_BYTES_TITLE)
	     Length = Med_MAX_BYTES_TITLE;

	 if ((Media->Title = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
	 Str_Copy (Media->Title,row[3],Length);
	}
     }
   else
      Err_ShowErrorAndExit ("Internal error in database when getting media data.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Draw input fields to upload an image/video inside a form **********/
/*****************************************************************************/
/*
   _container_____________________________________________
  |                     _<id>_med_ico                     |
  |                    |____Clip_____|                    |
  |                                                       |
  |  _container <id>_med_upl_(initially hidden)_________  |
  | |  _box___________________________________________  | |
  | | |                                             ? | | |
  | | |                   Multimedia                  | | |
  | | |                                               | | |
  | | |      _prefs_container___________________      | | |
  | | |     | _pref_container_________________  |     | | |
  | | |     | |  _______   _______   _______  | |     | | |
  | | |     | | | Image/| |YouTube| | Embed | | |     | | |
  | | |     | | |_video_| |_______| |_______| | |     | | |
  | | |     | |_______________________________| |     | | |
  | | |     |___________________________________|     | | |
  | | |  _file_container____________________________  | | |
  | | | |  ___________                              | | | |
  | | | | |_Browse..._| No file selected.           | | | |
  | | | |___________________________________________| | | |
  | | |  _URL_container_____________________________  | | |
  | | | |  _______________________________________  | | | |
  | | | | |_Link__________________________________| | | | |
  | | | |___________________________________________| | | |
  | | |  _title_container___________________________  | | |
  | | | |  _______________________________________  | | | |
  | | | | |_Title/attribution_____________________| | | | |
  | | | |___________________________________________| | | |
  | | |_______________________________________________| | |
  | |___________________________________________________| |
  |_______________________________________________________|
*/

#define Med_NUM_MEDIA_UPLOADERS 3

void Med_PutMediaUploader (int NumMedia,const char *ClassInput)
  {
   extern const char *Hlp_Multimedia;
   extern const char *Txt_Multimedia;
   extern const char *Txt_Image_video;
   extern const char *Txt_Title_attribution;
   extern const char *Txt_Link;
   struct Med_ParUpload ParUpload;
   char Id[Frm_MAX_BYTES_ID + 1];
   size_t NumUploader;
   struct MediaUploader MediaUploader[Med_NUM_MEDIA_UPLOADERS] =
     {
	{// Upload
	 .FormType     = Med_FORM_FILE,
	 .IconSuffix   = "ico_upl",			// <id>_ico_upl
	 .ParSuffix  = "par_upl",			// <id>_par_upl
	 .FunctionName = "mediaClickOnActivateUpload",
	 .Icon         = "photo-video.svg",
	 .Title        = Txt_Image_video
	},
	{// YouTube
	 .FormType     = Med_FORM_YOUTUBE,
	 .IconSuffix   = "ico_you",			// <id>_ico_you
	 .ParSuffix  = "par_you",			// <id>_par_you
	 .FunctionName = "mediaClickOnActivateYoutube",
	 .Icon         = "youtube-brands.svg",
	 .Title        = "YouTube"
	},
	{// Embed
	 .FormType     = Med_FORM_EMBED,
	 .IconSuffix   = "ico_emb",			// <id>_ico_emb
	 .ParSuffix  = "par_emb",			// <id>_par_emb
	 .FunctionName = "mediaClickOnActivateEmbed",
	 .Icon         = "code.svg",
	 .Title        = "Embed"
	}
     };

   /***** Set names of parameters depending on number of media in form *****/
   Med_SetParsNames (&ParUpload,NumMedia);

   /***** Create unique id for this media uploader *****/
   Frm_SetUniqueId (Id);

   /***** Begin media uploader container *****/
   HTM_DIV_Begin ("class=\"MED_UPLOADER\"");				// container

      /***** Clip icon *****/
      /* Begin container */
      HTM_DIV_Begin ("id=\"%s_med_ico\"",Id);				// <id>_med_ico

         /* Icon 'clip' */
	 HTM_A_Begin ("href=\"\""
		      " onclick=\"mediaActivateMediaUploader('%s');return false;\"",
		      Id);
	    Ico_PutIcon ("paperclip.svg",Ico_BLACK,Txt_Multimedia,"ICO_HIGHLIGHT ICOx16");
	 HTM_A_End ();

      /* End container */
      HTM_DIV_End ();							// <id>_med_ico

      /***** Begin media uploader *****/
      HTM_DIV_Begin ("id=\"%s_med_upl\" style=\"display:none;\"",	// container <id>_med_upl
		     Id);

	 /***** Begin box *****/
	 Box_BoxBegin (Txt_Multimedia,NULL,NULL,
		       Hlp_Multimedia,Box_NOT_CLOSABLE);

	    /***** Action to perform on media *****/
	    Par_PutParUnsigned (NULL,ParUpload.Action,
					(unsigned) Med_ACTION_NEW_MEDIA);

	    /***** Icons *****/
	    /* Begin containers */
	    HTM_DIV_Begin ("class=\"PREF_CONTS\"");
	       HTM_DIV_Begin ("class=\"PREF_CONT\"");

		  /* Draw icons */
		  for (NumUploader = 0;
		       NumUploader < Med_NUM_MEDIA_UPLOADERS;
		       NumUploader++)
		     Med_PutIconMediaUploader (Id,&MediaUploader[NumUploader]);

	       /* End containers */
	       HTM_DIV_End ();
	    HTM_DIV_End ();

	    /***** Form types *****/
	    for (NumUploader = 0;
		 NumUploader < Med_NUM_MEDIA_UPLOADERS;
		 NumUploader++)
	       Med_PutHiddenFormTypeMediaUploader (Id,&MediaUploader[NumUploader],
						   &ParUpload);

	    /***** Media file *****/
	    /* Begin container */
	    HTM_DIV_Begin (NULL);

	       /* Media file */
	       HTM_INPUT_FILE (ParUpload.File,"image/,video/",
			       HTM_NO_ATTR,
			       "id=\"%s_fil\" class=\"%s\""		// <id>_fil
			       " disabled=\"disabled\" style=\"display:none;\"",
			       Id,ClassInput);

	    /* End container */
	    HTM_DIV_End ();

	    /***** Media URL *****/
	    /* Begin container */
	    HTM_DIV_Begin (NULL);

	       /* Media URL */
	       HTM_INPUT_URL (ParUpload.URL,"",
			      HTM_NO_ATTR,
			      "id=\"%s_url\" class=\"%s\""			// <id>_url
			      " placeholder=\"%s\" maxlength=\"%u\""
			      " disabled=\"disabled\" style=\"display:none;\"",
			      Id,ClassInput,Txt_Link,WWW_MAX_CHARS_WWW);

	    /* End container */
	    HTM_DIV_End ();

	    /***** Media title *****/
	    /* Begin container */
	    HTM_DIV_Begin (NULL);

	       /* Media title */
	       HTM_INPUT_TEXT (ParUpload.Title,Med_MAX_CHARS_TITLE,"",
			       HTM_NO_ATTR,
			       "id=\"%s_tit\" class=\"%s\""		// <id>_tit
			       " placeholder=\"%s\""
			       " disabled=\"disabled\" style=\"display:none;\"",
			       Id,ClassInput,Txt_Title_attribution);

	    /* End container */
	    HTM_DIV_End ();

	 /***** End box *****/
	 Box_BoxEnd ();

      /***** End media uploader *****/
      HTM_DIV_End ();							// container <id>_med_upl

   /***** End media uploader container *****/
   HTM_DIV_End ();							// container
  }

/*****************************************************************************/
/*********************** Put an icon in media uploader ***********************/
/*****************************************************************************/

static void Med_PutIconMediaUploader (const char UniqueId[Frm_MAX_BYTES_ID + 1],
				      struct MediaUploader *MediaUploader)
  {
   /***** Icon to activate form in media uploader *****/
   /* Begin container */
   HTM_DIV_Begin ("id=\"%s_%s\" class=\"PREF_OFF\"",	// <id>_IconSuffix
                  UniqueId,MediaUploader->IconSuffix);

      /* Icon to upload media */
      HTM_A_Begin ("href=\"\" onclick=\"%s('%s');return false;\"",
		   MediaUploader->FunctionName,UniqueId);
	 Ico_PutIcon (MediaUploader->Icon,Ico_BLACK,
	              MediaUploader->Title,
	              "ICO_HIGHLIGHT ICOx16");
      HTM_A_End ();

   /* End container */
   HTM_DIV_End ();					// <id>_IconSuffix
  }

/*****************************************************************************/
/******** Put a hidden input field with form type in media uploader **********/
/*****************************************************************************/

static void Med_PutHiddenFormTypeMediaUploader (const char UniqueId[Frm_MAX_BYTES_ID + 1],
						struct MediaUploader *MediaUploader,
					        struct Med_ParUpload *ParUpload)
  {
   char *Id;

   /***** Hidden field with form type *****/
   if (asprintf (&Id,"%s_%s",UniqueId,MediaUploader->ParSuffix) < 0)	// <id>_ParSuffix
      Err_NotEnoughMemoryExit ();
   Par_PutParUnsignedDisabled (Id,ParUpload->FormType,
			       (unsigned) MediaUploader->FormType);
   free (Id);
  }

/*****************************************************************************/
/******************** Get media (image/video) from form **********************/
/*****************************************************************************/
// Media constructor must be called before calling this function
// If NumMedia  < 0, params have no suffix
// If NumMedia >= 0, the number is a suffix of the params

void Med_GetMediaFromForm (long CrsCod,long QstCod,int NumMedia,struct Med_Media *Media,
                           void (*GetMediaFromDB) (long CrsCod,long QstCod,int NumMedia,struct Med_Media *Media),
			   const char *SectionForAlerts)
  {
   extern const char *Txt_Error_sending_or_processing_image_video;
   struct Med_ParUpload ParUploadMedia;
   Med_Action_t Action;
   Med_FormType_t FormType;

   /***** Set names of parameters depending on number of media in form *****/
   Med_SetParsNames (&ParUploadMedia,NumMedia);

   /***** Get action and initialize media (image/video)
          (except title, that will be get after the media file) *****/
   Action = Med_GetMediaActionFromForm (ParUploadMedia.Action);

   /***** Get the media (image/video) name and the file *****/
   switch (Action)
     {
      case Med_ACTION_NEW_MEDIA:	// Upload new image/video
	 /***** Get form type *****/
	 FormType = Usr_GetFormTypeFromForm (&ParUploadMedia);

         /***** Get new media *****/
	 switch (FormType)
	   {
	    case Med_FORM_FILE:
	       Media->Action = Med_ACTION_NEW_MEDIA;

	       /* Get image/video (if present ==>
	                           process and create temporary file) */
	       Med_GetAndProcessFileFromForm (ParUploadMedia.File,Media);

	       /* Check status of media after getting and processing it */
	       if (Media->Status == Med_PROCESSED)
	         {
		  Usr_GetURLFromForm (ParUploadMedia.URL,Media);
		  Usr_GetTitleFromForm (ParUploadMedia.Title,Media);
	         }
	       break;
	    case Med_FORM_YOUTUBE:
	       Media->Action = Med_ACTION_NEW_MEDIA;

	       /* Get and process embed YouTube video from form */
	       Med_GetAndProcessYouTubeFromForm (ParUploadMedia.URL,Media);
	       break;
	    case Med_FORM_EMBED:
	       Media->Action = Med_ACTION_NEW_MEDIA;

	       /* Get and process other embed media from form */
	       Med_GetAndProcessEmbedFromForm (ParUploadMedia.URL,Media);
	       break;
	    default:	// No media form selected
	       Media->Action = Med_ACTION_NO_MEDIA;
	       break;
	   }

	 /***** Check status of media *****/
	 if (FormType != Med_FORM_NONE &&	// A media form is selected
	     Media->Status != Med_PROCESSED)	// No media successfully processed
	   {
	    /* Create alert with warning */
	    Ale_CreateAlert (Ale_WARNING,SectionForAlerts,
			     Txt_Error_sending_or_processing_image_video);

	    /* Reset media (no media will be saved into database) */
	    Med_ResetMedia (Media);
	   }
	 break;
      case Med_ACTION_KEEP_MEDIA:	// Keep current image/video unchanged
	 Media->Action = Med_ACTION_KEEP_MEDIA;

	 /***** Get media name *****/
	 if (GetMediaFromDB != NULL)
	    GetMediaFromDB (CrsCod,QstCod,NumMedia,Media);
	 break;
      default:	// Unknown action
	 Media->Action = Med_ACTION_NO_MEDIA;
         break;
     }
  }

/*****************************************************************************/
/********* Set parameters names depending on number of media in form *********/
/*****************************************************************************/
// If NumMedia <  0, params have no suffix
// If NumMedia >= 0, the number is a suffix of the params

void Med_SetParsNames (struct Med_ParUpload *ParUpl,int NumMedia)
  {
   if (NumMedia < 0)	// One unique media in form ==> no suffix needed
     {
      Str_Copy (ParUpl->Action  ,"MedAct",sizeof (ParUpl->Action  ) - 1);
      Str_Copy (ParUpl->FormType,"MedFrm",sizeof (ParUpl->FormType) - 1);
      Str_Copy (ParUpl->File    ,"MedFil",sizeof (ParUpl->File    ) - 1);
      Str_Copy (ParUpl->Title   ,"MedTit",sizeof (ParUpl->Title   ) - 1);
      Str_Copy (ParUpl->URL     ,"MedURL",sizeof (ParUpl->URL     ) - 1);
     }
   else				// Several video/images in form ==> add suffix
     {
      snprintf (ParUpl->Action  ,sizeof (ParUpl->Action),"MedAct%u",NumMedia);
      snprintf (ParUpl->FormType,sizeof (ParUpl->Action),"MedFrm%u",NumMedia);
      snprintf (ParUpl->File    ,sizeof (ParUpl->File  ),"MedFil%u",NumMedia);
      snprintf (ParUpl->Title   ,sizeof (ParUpl->Title ),"MedTit%u",NumMedia);
      snprintf (ParUpl->URL     ,sizeof (ParUpl->URL   ),"MedURL%u",NumMedia);
     }
  }

/*****************************************************************************/
/************************* Get media action from form ************************/
/*****************************************************************************/

static Med_Action_t Med_GetMediaActionFromForm (const char *ParAction)
  {
   /***** Get parameter with the action to perform on media *****/
   return (Med_Action_t)
   Par_GetParUnsignedLong (ParAction,
			   0,
			   Med_NUM_ACTIONS - 1,
			   (unsigned long) Med_ACTION_DEFAULT);
  }

/*****************************************************************************/
/********************* Get from form the type of form ************************/
/*****************************************************************************/

static Med_FormType_t Usr_GetFormTypeFromForm (struct Med_ParUpload *ParUpload)
  {
   return (Med_FormType_t)
   Par_GetParUnsignedLong (ParUpload->FormType,
			   0,
			   Med_NUM_FORM_TYPES - 1,
			   (unsigned long) Med_FORM_NONE);
  }

/*****************************************************************************/
/********************* Get from form the type of form ************************/
/*****************************************************************************/

static void Usr_GetURLFromForm (const char *ParName,struct Med_Media *Media)
  {
   char URL[WWW_MAX_BYTES_WWW + 1];
   size_t Length;

   /***** Get media URL from form *****/
   Par_GetParText (ParName,URL,WWW_MAX_BYTES_WWW);
   /* If the URL coming from the form is empty, keep current media URL unchanged
      If not empty, copy it to current media URL */
   if ((Length = strlen (URL)) > 0)
     {
      /* Overwrite current URL (empty or coming from database)
         with the URL coming from the form */
      Med_FreeMediaURL (Media);
      if ((Media->URL = malloc (Length + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
      Str_Copy (Media->URL,URL,Length);
     }
  }

/*****************************************************************************/
/********************* Get from form the type of form ************************/
/*****************************************************************************/

static void Usr_GetTitleFromForm (const char *ParName,struct Med_Media *Media)
  {
   char Title[Med_MAX_BYTES_TITLE + 1];
   size_t Length;

   /***** Get image/video title from form *****/
   Par_GetParText (ParName,Title,Med_MAX_BYTES_TITLE);
   /* If the title coming from the form is empty, keep current media title unchanged
      If not empty, copy it to current media title */
   if ((Length = strlen (Title)) > 0)
     {
      /* Overwrite current title (empty or coming from database)
         with the title coming from the form */
      Med_FreeMediaTitle (Media);
      if ((Media->Title = malloc (Length + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
      Str_Copy (Media->Title,Title,Length);
     }
  }

/*****************************************************************************/
/**************************** Get media from form ****************************/
/*****************************************************************************/

static void Med_GetAndProcessFileFromForm (const char *ParFile,
                                           struct Med_Media *Media)
  {
   static void (*Process[Med_NUM_TYPES]) (struct Med_Media *Media,
			                  const char PathFileOrg[PATH_MAX + 1]) =
     {
      [Med_JPG ] = Med_ProcessJPG,
      [Med_GIF ] = Med_ProcessGIF,
      [Med_MP4 ] = Med_ProcessVideo,
      [Med_WEBM] = Med_ProcessVideo,
      [Med_OGG ] = Med_ProcessVideo,
     };
   struct Par_Param *Par;
   char FileNameImgSrc[PATH_MAX + 1];
   char *PtrExtension;
   size_t LengthExtension;
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   char PathFileOrg[PATH_MAX + 1];	// Full name of original uploaded file

   /***** Set media status *****/
   Media->Status = Med_STATUS_NONE;

   /***** Get filename and MIME type *****/
   Par = Fil_StartReceptionOfFile (ParFile,FileNameImgSrc,MIMEType);
   if (!FileNameImgSrc[0])	// No file present
      return;

   /* Get filename extension */
   if ((PtrExtension = strrchr (FileNameImgSrc,(int) '.')) == NULL)
      return;
   // PtrExtension now points to last '.' in file

   PtrExtension++;
   // PtrExtension now points to first char in extension

   LengthExtension = strlen (PtrExtension);
   if (LengthExtension < Fil_MIN_BYTES_FILE_EXTENSION ||
       LengthExtension > Fil_MAX_BYTES_FILE_EXTENSION)
      return;

   /* Check extension */
   Media->Type = Med_GetTypeFromExtAndMIME (PtrExtension,MIMEType);
   if (Media->Type == Med_TYPE_NONE)
      return;

   /***** Assign a unique name for the media *****/
   Cry_CreateUniqueNameEncrypted (Media->Name);

   /***** Create private directories if not exist *****/
   /* Create private directory for images/videos if it does not exist */
   Fil_CreateDirIfNotExists (Cfg_PATH_MEDIA_PRIVATE);

   /* Create temporary private directory for images/videos if it does not exist */
   Fil_CreateDirIfNotExists (Cfg_PATH_MEDIA_TMP_PRIVATE);

   /***** End the reception of original not processed media
          (it may be very big) into a temporary file *****/
   Media->Status = Med_STATUS_NONE;
   snprintf (PathFileOrg,sizeof (PathFileOrg),"%s/%s_original.%s",
	     Cfg_PATH_MEDIA_TMP_PRIVATE,Media->Name,PtrExtension);

   if (Fil_EndReceptionOfFile (PathFileOrg,Par))	// Success
     {
      /***** Detect if animated GIF *****/
      if (Media->Type == Med_GIF)
	 if (!Med_DetectIfAnimated (Media,PathFileOrg))
            Media->Type = Med_JPG;

      /***** Process media depending on the media file extension *****/
      if (Process[Media->Type])
         Process[Media->Type] (Media,PathFileOrg);
     }

   /***** Remove temporary original file *****/
   if (Fil_CheckIfPathExists (PathFileOrg))
      unlink (PathFileOrg);
  }

/*****************************************************************************/
/********************* Detect if a GIF image is animated *********************/
/*****************************************************************************/
// Return true if animated
// Return false if static or error

static bool Med_DetectIfAnimated (struct Med_Media *Media,
			          const char PathFileOrg[PATH_MAX + 1])
  {
   char PathFileTxtTmp[PATH_MAX + 1];
   char Command[128 + PATH_MAX * 2];
   int ReturnCode;
   FILE *FileTxtTmp;	// Temporary file with the output of the command
   int NumFrames = 0;

   /***** Build path to temporary text file *****/
   snprintf (PathFileTxtTmp,sizeof (PathFileTxtTmp),"%s/%s.txt",
             Cfg_PATH_MEDIA_TMP_PRIVATE,Media->Name);

   /***** Execute system command to get number of frames in GIF *****/
   snprintf (Command,sizeof (Command),
             "identify -format '%%n\n' %s | head -1 > %s",
             PathFileOrg,PathFileTxtTmp);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      return false;		// Error
   ReturnCode = WEXITSTATUS(ReturnCode);
   if (ReturnCode)
      return false;		// Error

   /***** Read temporary file *****/
   if ((FileTxtTmp = fopen (PathFileTxtTmp,"rb")) == NULL)
      return false;		// Error
   if (fscanf (FileTxtTmp,"%d",&NumFrames) != 1)
      return false;		// Error
   fclose (FileTxtTmp);

   /***** Remove temporary file *****/
   unlink (PathFileTxtTmp);

   return (NumFrames > 1);	// NumFrames > 1 ==> Animated
  }

/*****************************************************************************/
/************* Process original image generating processed JPG ***************/
/*****************************************************************************/

static void Med_ProcessJPG (struct Med_Media *Media,
			    const char PathFileOrg[PATH_MAX + 1])
  {
   char PathFileJPGTmp[PATH_MAX + 1];	// Full name of temporary processed file

   /***** Convert original media to temporary JPG processed file
	  by calling to program that makes the conversion *****/
   snprintf (PathFileJPGTmp,sizeof (PathFileJPGTmp),"%s/%s.%s",
	     Cfg_PATH_MEDIA_TMP_PRIVATE,Media->Name,Med_Extensions[Med_JPG]);
   if (Med_ResizeImage (Media,PathFileOrg,PathFileJPGTmp) == 0)	// On success ==> 0 is returned
      /* Success */
      Media->Status = Med_PROCESSED;
   else // Error processing media
     {
      /* Remove temporary destination media file */
      if (Fil_CheckIfPathExists (PathFileJPGTmp))
	 unlink (PathFileJPGTmp);

      /* Show error alert */
      Med_ErrorProcessingMediaFile ();
     }
  }

/*****************************************************************************/
/******* Process original GIF image generating processed PNG and GIF *********/
/*****************************************************************************/

static void Med_ProcessGIF (struct Med_Media *Media,
			    const char PathFileOrg[PATH_MAX + 1])
  {
   extern const char *Txt_The_size_of_the_file_exceeds_the_maximum_allowed_X;
   struct stat FileStatus;
   char PathFilePNGTmp[PATH_MAX + 1];	// Full name of temporary processed file
   char PathFileGIFTmp[PATH_MAX + 1];	// Full name of temporary processed file
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];

   /***** Check size of media file *****/
   if (lstat (PathFileOrg,&FileStatus) == 0)	// On success ==> 0 is returned
     {
      /* Success */
      if (FileStatus.st_size <= (__off_t) Med_MAX_SIZE_GIF)
	{
	 /* File size correct */
	 /***** Get first frame of orifinal GIF file
		and save it on temporary PNG file */
	 snprintf (PathFilePNGTmp,sizeof (PathFilePNGTmp),"%s/%s.png",
		   Cfg_PATH_MEDIA_TMP_PRIVATE,Media->Name);
	 if (Med_GetFirstFrame (PathFileOrg,PathFilePNGTmp) == 0)	// On success ==> 0 is returned
	   {
	    /* Success */
	    /***** Move original GIF file to temporary GIF file *****/
	    snprintf (PathFileGIFTmp,sizeof (PathFileGIFTmp),"%s/%s.%s",
		      Cfg_PATH_MEDIA_TMP_PRIVATE,
		      Media->Name,Med_Extensions[Med_GIF]);
	    if (rename (PathFileOrg,PathFileGIFTmp))	// Fail
	      {
	       /* Remove temporary PNG file */
	       if (Fil_CheckIfPathExists (PathFilePNGTmp))
		  unlink (PathFilePNGTmp);

	       /* Show error alert */
               Med_ErrorProcessingMediaFile ();
	      }
	    else					// Success
	       Media->Status = Med_PROCESSED;
	   }
	 else // Error getting first frame
	   {
	    /* Remove temporary PNG file */
	    if (Fil_CheckIfPathExists (PathFilePNGTmp))
	       unlink (PathFilePNGTmp);

	    /* Show error alert */
            Med_ErrorProcessingMediaFile ();
	   }
	}
      else	// Size exceeded
	{
	 /* Show warning alert */
	 Fil_WriteFileSizeBrief ((double) Med_MAX_SIZE_GIF,FileSizeStr);
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_size_of_the_file_exceeds_the_maximum_allowed_X,
			  FileSizeStr);
	}
     }
   else // Error getting file data
      /* Show error alert */
      Med_ErrorProcessingMediaFile ();
  }

/*****************************************************************************/
/*********** Process original MP4 video generating processed MP4 *************/
/*****************************************************************************/

static void Med_ProcessVideo (struct Med_Media *Media,
			      const char PathFileOrg[PATH_MAX + 1])
  {
   extern const char *Txt_The_size_of_the_file_exceeds_the_maximum_allowed_X;
   struct stat FileStatus;
   char PathFileTmp[PATH_MAX + 1];	// Full name of temporary processed file
   char FileSizeStr[Fil_MAX_BYTES_FILE_SIZE_STRING + 1];

   /***** Check size of media file *****/
   if (lstat (PathFileOrg,&FileStatus) == 0)	// On success ==> 0 is returned
     {
      /* Success */
      if (FileStatus.st_size <= (__off_t) Med_MAX_SIZE_MP4)
	{
	 /* File size correct */
	 /***** Move original video file to temporary MP4 file *****/
	 snprintf (PathFileTmp,sizeof (PathFileTmp),"%s/%s.%s",
		   Cfg_PATH_MEDIA_TMP_PRIVATE,
		   Media->Name,Med_Extensions[Media->Type]);
	 if (rename (PathFileOrg,PathFileTmp))	// Fail
	    /* Show error alert */
            Med_ErrorProcessingMediaFile ();
	 else						// Success
	    Media->Status = Med_PROCESSED;
	}
      else	// Size exceeded
	{
	 /* Show warning alert */
	 Fil_WriteFileSizeBrief ((double) Med_MAX_SIZE_MP4,FileSizeStr);
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_The_size_of_the_file_exceeds_the_maximum_allowed_X,
			  FileSizeStr);
	}
     }
   else // Error getting file data
      /* Show error alert */
      Med_ErrorProcessingMediaFile ();
  }

/*****************************************************************************/
/****************************** Resize image *********************************/
/*****************************************************************************/
// Return 0 on success
// Return != 0 on error

static int Med_ResizeImage (struct Med_Media *Media,
                            const char PathFileOriginal[PATH_MAX + 1],
                            const char PathFileProcessed[PATH_MAX + 1])
  {
   char Command[256 + PATH_MAX * 2];
   int ReturnCode;

   snprintf (Command,sizeof (Command),
             "convert %s -resize '%ux%u>' -quality %u %s",
             PathFileOriginal,
             Media->Width,
             Media->Height,
             Media->Quality,
             PathFileProcessed);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Err_ShowErrorAndExit ("Error when running command to process media.");

   ReturnCode = WEXITSTATUS(ReturnCode);
   return ReturnCode;
  }

/*****************************************************************************/
/************ Process original media generating processed media **************/
/*****************************************************************************/
// Return 0 on success
// Return != 0 on error

static int Med_GetFirstFrame (const char PathFileOriginal[PATH_MAX + 1],
                              const char PathFileProcessed[PATH_MAX + 1])
  {
   char Command[128 + PATH_MAX * 2];
   int ReturnCode;

   snprintf (Command,sizeof (Command),"convert '%s[0]' %s",
             PathFileOriginal,
             PathFileProcessed);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Err_ShowErrorAndExit ("Error when running command to process media.");

   ReturnCode = WEXITSTATUS(ReturnCode);

   return ReturnCode;
  }

/*****************************************************************************/
/************* Get link from form and transform to YouTube code **************/
/*****************************************************************************/

static void Med_GetAndProcessYouTubeFromForm (const char *ParURL,
                                              struct Med_Media *Media)
  {
   extern const char Str_BIN_TO_BASE64URL[64 + 1];
   char *PtrHost   = NULL;
   char *PtrPath   = NULL;
   char *PtrParams = NULL;
   char *PtrCode   = NULL;
   size_t CodeLength;
   enum
     {
      WRONG,	// Bad formed YouTube URL
      SHORT,	// youtu.be
      FULL,	// www.youtube.com/watch?
      EMBED,	// www.youtube.com/embed/
     } YouTube = WRONG;

   bool CodeFound = false;

   /***** Set media status *****/
   Media->Status = Med_STATUS_NONE;

   /***** Get embed URL from form *****/
   Usr_GetURLFromForm (ParURL,Media);

   /***** Process URL trying to convert it to a YouTube embed URL *****/
   if (Media->URL)
      if (Media->URL[0])	// URL given by user is not empty
	{
	 /* Examples of valid YouTube URLs:
	    https://www.youtube.com/watch?v=xu9IbeF9CBw
	    https://www.youtube.com/watch?v=xu9IbeF9CBw&t=10
	    https://youtu.be/xu9IbeF9CBw
	    https://youtu.be/xu9IbeF9CBw?t=10
	    https://www.youtube.com/embed/xu9IbeF9CBw
	    https://www.youtube.com/embed/xu9IbeF9CBw?start=10
	 */
	 /***** Step 1: Skip scheme *****/
	 if      (!strncasecmp (Media->URL,"https://",8))	// URL starts by https://
	    PtrHost = &Media->URL[8];
	 else if (!strncasecmp (Media->URL,"http://" ,7))	// URL starts by http://
	    PtrHost = &Media->URL[7];
	 else if (!strncasecmp (Media->URL,"//"      ,2))	// URL starts by //
	    PtrHost = &Media->URL[2];
	 else
	    PtrHost = &Media->URL[0];

	 if (PtrHost[0])
	   {
	    /***** Step 2: Skip host *****/
	    if      (!strncasecmp (PtrHost,"youtu.be/"       , 9))	// Host starts by youtu.be/
	      {
	       YouTube = SHORT;
	       PtrPath = &PtrHost[9];
	      }
	    else if (!strncasecmp (PtrHost,"www.youtube.com/",16))	// Host starts by www.youtube.com/
	      {
	       YouTube = FULL;
	       PtrPath = &PtrHost[16];
	      }
	    else if (!strncasecmp (PtrHost,"m.youtube.com/"  ,14))	// Host starts by m.youtube.com/
	      {
	       YouTube = FULL;
	       PtrPath = &PtrHost[14];
	      }
	    else if (!strncasecmp (PtrHost,"youtube.com/"    ,12))	// Host starts by youtube.com/
	      {
	       YouTube = FULL;
	       PtrPath = &PtrHost[12];
	      }

	    /* Check pointer to path */
	    if (PtrPath)
	      {
	       if (!PtrPath[0])
		  YouTube = WRONG;
	      }
	    else
	       YouTube = WRONG;

	    if (YouTube != WRONG)
	      {
	       /***** Step 3: Skip path *****/
	       if (YouTube == FULL)
		 {
		  if      (!strncasecmp (PtrPath,"watch?",6))	// Path starts by watch?
		     PtrParams = &PtrPath[6];
		  else if (!strncasecmp (PtrPath,"embed/",6))	// Path starts by embed/
		    {
		     YouTube = EMBED;
		     PtrParams = &PtrPath[6];
		    }
		  else
		     YouTube = WRONG;
		 }
	       else
		  PtrParams = &PtrPath[0];

	       /* Check pointer to params */
	       if (PtrParams)
		 {
		  if (!PtrParams[0])
		     YouTube = WRONG;
		 }
	       else
		  YouTube = WRONG;

	       if (YouTube != WRONG)
		 {
		  /***** Step 4: Search for video code *****/
		  switch (YouTube)
		    {
		     case SHORT:
			PtrCode = PtrParams;
			break;
		     case FULL:
			/* Search for v= */
			PtrCode = strcasestr (PtrPath,"v=");
			if (PtrCode)
			   PtrCode += 2;
			break;
		     case EMBED:
			PtrCode = PtrParams;
			break;
		     default:
			PtrCode = NULL;
			break;
		    }

		  /* Check pointer to code */
		  if (PtrCode)
		    {
		     if (!PtrCode[0])
			YouTube = WRONG;
		    }
		  else
		     YouTube = WRONG;

		  if (YouTube != WRONG)
		    {
		     /***** Step 5: Get video code *****/
		     CodeLength = strspn (PtrCode,Str_BIN_TO_BASE64URL);
		     if (CodeLength > 0 &&
			 CodeLength <= Med_BYTES_NAME)
			CodeFound = true;	// Success!
		    }
		 }
	      }
	   }
	}

   /***** Set or reset media *****/
   if (CodeFound)
     {
      /* Copy code */
      strncpy (Media->Name,PtrCode,CodeLength);
      Media->Name[CodeLength] = '\0';

      /* Set media type and status */
      Media->Type   = Med_YOUTUBE;
      Media->Status = Med_PROCESSED;
     }
   else
      /* Reset media */
      Med_ResetMedia (Media);
  }

/*****************************************************************************/
/************************ Get embed link from form ***************************/
/*****************************************************************************/

static void Med_GetAndProcessEmbedFromForm (const char *ParURL,
                                            struct Med_Media *Media)
  {
   extern const char Str_BIN_TO_BASE64URL[64 + 1];
   char *PtrHost = NULL;
   bool URLFound = false;

   /***** Set media status *****/
   Media->Status = Med_STATUS_NONE;

   /***** Get embed URL from form *****/
   Usr_GetURLFromForm (ParURL,Media);

   /***** Process URL trying to convert it to a YouTube embed URL *****/
   if (Media->URL)
      if (Media->URL[0])	// URL given by user is not empty
	{
	 /* Examples of valid embed URLs:
	    //www.slideshare.net/slideshow/embed_code/key/yngasD9sIZ7GQV
	 */
	 /***** Step 1: Skip scheme *****/
	 if      (!strncasecmp (Media->URL,"https://",8))	// URL starts by https://
	    PtrHost = &Media->URL[8];
	 else if (!strncasecmp (Media->URL,"http://" ,7))	// URL starts by http://
	    PtrHost = &Media->URL[7];
	 else if (!strncasecmp (Media->URL,"//"      ,2))	// URL starts by //
	    PtrHost = &Media->URL[2];

	 /***** Check if a URL is found *****/
	 if (PtrHost)
	    if (PtrHost[0])
	       URLFound = true;	// Success!
	}

   /***** Set or reset media *****/
   if (URLFound)
     {
      /* Set media type and status */
      Media->Type   = Med_EMBED;
      Media->Status = Med_PROCESSED;
     }
   else
      /* Reset media */
      Med_ResetMedia (Media);
  }

/*****************************************************************************/
/**** Remove media, keep media or store media, depending on media action *****/
/*****************************************************************************/

void Med_RemoveKeepOrStoreMedia (long CurrentMedCodInDB,struct Med_Media *Media)
  {
   switch (Media->Action)
     {
      case Med_ACTION_NO_MEDIA:
	 /* Remove possible current media */
	 Med_RemoveMedia (Media->MedCod);

	 /* Reset media data */
	 Med_ResetMedia (Media);
	 break;
      case Med_ACTION_KEEP_MEDIA:
	 /* Keep current media */
	 Media->MedCod = CurrentMedCodInDB;
         Med_GetMediaDataByCod (Media);
	 break;
      case Med_ACTION_NEW_MEDIA:
	 /* Remove possible current media */
	 Med_RemoveMedia (Media->MedCod);

	 /* New media received and processed sucessfully? */
	 if (Media->Status == Med_PROCESSED)		// The new media received has been processed
	   {
	    /* Move processed media to definitive directory */
	    Med_MoveMediaToDefinitiveDir (Media);

	    if (Media->Status == Med_MOVED)
	       /* Store media in database */
	       Med_StoreMediaInDB (Media);	// Set Media->MedCod
	    else
	       /* Reset media data */
	       Med_ResetMedia (Media);
	   }
	 else
	    /* Reset media data */
	    Med_ResetMedia (Media);
	 break;
     }
  }

/*****************************************************************************/
/**** Move temporary processed media file to definitive private directory ****/
/*****************************************************************************/

void Med_MoveMediaToDefinitiveDir (struct Med_Media *Media)
  {
   char PathMedPriv[PATH_MAX + 1];

   /***** Check trivial case *****/
   if (Media->Status == Med_PROCESSED)
     {
      switch (Media->Type)
        {
         case Med_JPG:
         case Med_GIF:
         case Med_MP4:
         case Med_WEBM:
         case Med_OGG:
	    /***** Create private subdirectory for media if it does not exist *****/
	    snprintf (PathMedPriv,sizeof (PathMedPriv),"%s/%c%c",
		      Cfg_PATH_MEDIA_PRIVATE,
		      Media->Name[0],
		      Media->Name[1]);
	    Fil_CreateDirIfNotExists (PathMedPriv);

	    /***** Move files *****/
	    switch (Media->Type)
	      {
	       case Med_JPG:
		  /* Move JPG */
		  if (Med_MoveTmpFileToDefDir (Media,PathMedPriv,
					       Med_Extensions[Med_JPG]))
		     Media->Status = Med_MOVED;	// Success
		  break;
	       case Med_GIF:
		  /* Move PNG */
		  if (Med_MoveTmpFileToDefDir (Media,PathMedPriv,
					       "png"))
		     /* Move GIF */
		     if (Med_MoveTmpFileToDefDir (Media,PathMedPriv,
						  Med_Extensions[Med_GIF]))
			Media->Status = Med_MOVED;	// Success
		  break;
	       case Med_MP4:
	       case Med_WEBM:
	       case Med_OGG:
		  /* Move MP4 or WEBM or OGG */
		  if (Med_MoveTmpFileToDefDir (Media,PathMedPriv,
					       Med_Extensions[Media->Type]))
		     Media->Status = Med_MOVED;	// Success
		  break;
	       default:
		  break;
	      }
            break;
         case Med_YOUTUBE:
         case Med_EMBED:
	    // Nothing to do with files ==> Processing successfully finished
	    Media->Status = Med_MOVED;	// Success
            break;
         default:
            break;
        }
     }

   /***** If fail ==> reset media *****/
   if (Media->Status != Med_MOVED)	// Fail
      Med_ResetMedia (Media);
  }

/*****************************************************************************/
/******* Move temporary processed file to definitive private directory *******/
/*****************************************************************************/
// Return true on success
// Return false on error

static bool Med_MoveTmpFileToDefDir (struct Med_Media *Media,
				     const char PathMedPriv[PATH_MAX + 1],
				     const char *Extension)
  {
   char PathFileTmp[PATH_MAX + 1];	// Full name of temporary processed file
   char PathFile[PATH_MAX + 1];		// Full name of definitive processed file

   /***** Temporary processed media file *****/
   snprintf (PathFileTmp,sizeof (PathFileTmp),"%s/%s.%s",
	     Cfg_PATH_MEDIA_TMP_PRIVATE,Media->Name,Extension);

   /***** Definitive processed media file *****/
   snprintf (PathFile,sizeof (PathFile),"%s/%s.%s",
	     PathMedPriv,Media->Name,Extension);

   /***** Move JPG file *****/
   if (rename (PathFileTmp,PathFile))	// Fail
     {
      Med_ErrorProcessingMediaFile ();
      return false;
     }

   return true;				// Success
  }

/*****************************************************************************/
/************************ Store media into database **************************/
/*****************************************************************************/

void Med_StoreMediaInDB (struct Med_Media *Media)
  {
   /***** Insert media into database *****/
   Media->MedCod = Med_DB_StoreMedia (Media);
   Media->Status = Med_STORED_IN_DB;
  }

/*****************************************************************************/
/****** Show a user uploaded media (in a test question, timeline, etc.) ******/
/*****************************************************************************/

void Med_ShowMedia (const struct Med_Media *Media,
                    const char *ClassContainer,const char *ClassMedia)
  {
   static void (*Show[Med_NUM_TYPES]) (const struct Med_Media *Media,const char *ClassMedia) =
     {
      [Med_JPG    ] = Med_ShowMediaFile,
      [Med_GIF    ] = Med_ShowMediaFile,
      [Med_MP4    ] = Med_ShowMediaFile,
      [Med_WEBM   ] = Med_ShowMediaFile,
      [Med_OGG    ] = Med_ShowMediaFile,
      [Med_YOUTUBE] = Med_ShowYoutube,
      [Med_EMBED  ] = Med_ShowEmbed,
     };

   /***** If no media to show ==> nothing to do *****/
   if (Media->MedCod <= 0 ||
       Media->Status != Med_STORED_IN_DB ||
       Media->Type   == Med_TYPE_NONE)
      return;

   /***** Begin media container *****/
   HTM_DIV_Begin ("class=\"%s\"",ClassContainer);

      /***** Show media *****/
      if (Show[Media->Type])
	 Show[Media->Type] (Media,ClassMedia);

   /***** End media container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*********************** Show an embed YouTube video *************************/
/*****************************************************************************/

static void Med_ShowMediaFile (const struct Med_Media *Media,const char *ClassMedia)
  {
   static void (*Show[Med_NUM_TYPES]) (const struct Med_Media *Media,
			               const char PathMedPriv[PATH_MAX + 1],
			               const char *ClassMedia) =
     {
      [Med_JPG ] = Med_ShowJPG,
      [Med_GIF ] = Med_ShowGIF,
      [Med_MP4 ] = Med_ShowVideo,
      [Med_WEBM] = Med_ShowVideo,
      [Med_OGG ] = Med_ShowVideo,
     };
   char PathMedPriv[PATH_MAX + 1];

   /***** If no media to show ==> nothing to do *****/
   if (!Media->Name)
      return;
   if (!Media->Name[0])
      return;

   /***** Begin optional link to external URL *****/
   if (Media->URL)
      if (Media->URL[0])
         HTM_A_Begin ("href=\"%s\" target=\"_blank\"",Media->URL);

   /***** Build path to private directory with the media *****/
   snprintf (PathMedPriv,sizeof (PathMedPriv),"%s/%c%c",
	     Cfg_PATH_MEDIA_PRIVATE,
	     Media->Name[0],
	     Media->Name[1]);

   /***** Show media *****/
   if (Show[Media->Type])
      Show[Media->Type] (Media,PathMedPriv,ClassMedia);

   /***** End optional link to external URL *****/
   if (Media->URL)
      if (Media->URL[0])
         HTM_A_End ();
  }

/*****************************************************************************/
/************************** Show a user uploaded JPG *************************/
/*****************************************************************************/

static void Med_ShowJPG (const struct Med_Media *Media,
			 const char PathMedPriv[PATH_MAX + 1],
			 const char *ClassMedia)
  {
   extern const char *Txt_File_not_found;
   char FileNameJPG[NAME_MAX + 1];
   char TmpPubDir[PATH_MAX + 1];
   char *FullPathJPGPriv;
   char *URL;
   bool Cached;

   /***** Build private path to JPG *****/
   snprintf (FileNameJPG,sizeof (FileNameJPG),"%s.%s",
	     Media->Name,Med_Extensions[Med_JPG]);
   if (asprintf (&FullPathJPGPriv,"%s/%s",
	         PathMedPriv,FileNameJPG) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Check if private media file exists *****/
   if (Fil_CheckIfPathExists (FullPathJPGPriv))
     {
      /***** Get cached public link to private file *****/
      Cached = Fil_GetPublicDirFromCache (FullPathJPGPriv,TmpPubDir);

      if (!Cached)
	{
	 /***** Create symbolic link from temporary public directory to private file
		in order to gain access to it for showing/downloading *****/
	 Brw_CreateDirDownloadTmp ();
	 Brw_CreateTmpPublicLinkToPrivateFile (FullPathJPGPriv,FileNameJPG);

	 snprintf (TmpPubDir,sizeof (TmpPubDir),"%s/%s",
	           Gbl.FileBrowser.TmpPubDir.L,Gbl.FileBrowser.TmpPubDir.R);
	 Fil_AddPublicDirToCache (FullPathJPGPriv,TmpPubDir);
	}

      /***** Show media *****/
      if (asprintf (&URL,"%s/%s",
		    Cfg_URL_FILE_BROWSER_TMP_PUBLIC,TmpPubDir) < 0)
	 Err_NotEnoughMemoryExit ();
      HTM_IMG (URL,FileNameJPG,Media->Title,
	       "class=\"%s\" lazyload=\"on\"",ClassMedia);	// Lazy load of the media
      free (URL);
     }
   else
      HTM_Txt (Txt_File_not_found);

   free (FullPathJPGPriv);
  }

/*****************************************************************************/
/************************** Show a user uploaded GIF *************************/
/*****************************************************************************/

static void Med_ShowGIF (const struct Med_Media *Media,
			 const char PathMedPriv[PATH_MAX + 1],
			 const char *ClassMedia)
  {
   extern const char *Txt_File_not_found;
   char FileNameGIF[NAME_MAX + 1];
   char FileNamePNG[NAME_MAX + 1];
   char TmpPubDir[PATH_MAX + 1];
   char *FullPathGIFPriv;
   char *FullPathPNGPriv;
   char *URL;
   char *URL_GIF;
   char *URL_PNG;
   bool Cached;

   /***** Build private path to animated GIF image *****/
   snprintf (FileNameGIF,sizeof (FileNameGIF),"%s.%s",
	     Media->Name,Med_Extensions[Med_GIF]);
   if (asprintf (&FullPathGIFPriv,"%s/%s",	// The animated GIF image
		 PathMedPriv,FileNameGIF) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Build private path to static PNG image *****/
   snprintf (FileNamePNG,sizeof (FileNamePNG),"%s.png",Media->Name);
   if (asprintf (&FullPathPNGPriv,"%s/%s",
		 PathMedPriv,FileNamePNG) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Check if private media file exists *****/
   if (Fil_CheckIfPathExists (FullPathGIFPriv))		// The animated GIF image
     {
      /***** Get cached public link to private file *****/
      Cached = Fil_GetPublicDirFromCache (FullPathGIFPriv,TmpPubDir);

      if (!Cached)
	{
	 /***** Create symbolic link from temporary public directory to private file
		in order to gain access to it for showing/downloading *****/
	 Brw_CreateDirDownloadTmp ();
	 Brw_CreateTmpPublicLinkToPrivateFile (FullPathGIFPriv,FileNameGIF);
	 Brw_CreateTmpPublicLinkToPrivateFile (FullPathPNGPriv,FileNamePNG);

	 snprintf (TmpPubDir,sizeof (TmpPubDir),"%s/%s",
	           Gbl.FileBrowser.TmpPubDir.L,Gbl.FileBrowser.TmpPubDir.R);
	 Fil_AddPublicDirToCache (FullPathGIFPriv,TmpPubDir);
	}

      /***** Create URLs pointing to symbolic links *****/
      if (asprintf (&URL,"%s/%s",
		    Cfg_URL_FILE_BROWSER_TMP_PUBLIC,TmpPubDir) < 0)
	 Err_NotEnoughMemoryExit ();
      if (asprintf (&URL_GIF,"%s/%s",URL,FileNameGIF) < 0)
	 Err_NotEnoughMemoryExit ();
      if (asprintf (&URL_PNG,"%s/%s",URL,FileNamePNG) < 0)	// The static PNG image
	 Err_NotEnoughMemoryExit ();

      /***** Check if private media file exists *****/
      if (Fil_CheckIfPathExists (FullPathPNGPriv))		// The static PNG image
	{
	 /***** Show static PNG and animated GIF *****/
	 /* Begin container */
	 HTM_DIV_Begin ("class=\"MED_PLAY\""
			" onmouseover=\"toggleOnGIF(this,'%s');\""
			" onmouseout=\"toggleOffGIF(this,'%s');\"",
			URL_GIF,
			URL_PNG);

	    /* Image */
	    HTM_IMG (URL,FileNamePNG,Media->Title,
		     "class=\"%s\" lazyload=\"on\"",ClassMedia);	// Lazy load of the media

	    /* Overlay with GIF label */
	    HTM_SPAN_Begin ("class=\"MED_PLAY_ICO\"");
	       HTM_Txt ("GIF");
	    HTM_SPAN_End ();

	 /* End container */
	 HTM_DIV_End ();
	}
      else
	 HTM_Txt (Txt_File_not_found);

      /***** Free URLs *****/
      free (URL_PNG);
      free (URL_GIF);
      free (URL);
     }
   else
      HTM_Txt (Txt_File_not_found);

   free (FullPathPNGPriv);
   free (FullPathGIFPriv);
  }

/*****************************************************************************/
/************************ Show a user uploaded video *************************/
/*****************************************************************************/

static void Med_ShowVideo (const struct Med_Media *Media,
			   const char PathMedPriv[PATH_MAX + 1],
			   const char *ClassMedia)
  {
   extern const char *Txt_File_not_found;
   char FileNameVideo[NAME_MAX + 1];
   char TmpPubDir[PATH_MAX + 1];
   char *FullPathVideoPriv;
   char *URL;
   bool Cached;

   /***** Build private path to video *****/
   snprintf (FileNameVideo,sizeof (FileNameVideo),"%s.%s",
	     Media->Name,Med_Extensions[Media->Type]);
   if (asprintf (&FullPathVideoPriv,"%s/%s",
	         PathMedPriv,FileNameVideo) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Check if private media file exists *****/
   if (Fil_CheckIfPathExists (FullPathVideoPriv))
     {
      /***** Get cached public link to private file *****/
      Cached = Fil_GetPublicDirFromCache (FullPathVideoPriv,TmpPubDir);

      if (!Cached)
	{
	 /***** Create symbolic link from temporary public directory to private file
		in order to gain access to it for showing/downloading *****/
	 Brw_CreateDirDownloadTmp ();
	 Brw_CreateTmpPublicLinkToPrivateFile (FullPathVideoPriv,FileNameVideo);

	 snprintf (TmpPubDir,sizeof (TmpPubDir),"%s/%s",
	           Gbl.FileBrowser.TmpPubDir.L,Gbl.FileBrowser.TmpPubDir.R);
	 Fil_AddPublicDirToCache (FullPathVideoPriv,TmpPubDir);
	}

      /***** Create URL pointing to symbolic link *****/
      if (asprintf (&URL,"%s/%s",
		    Cfg_URL_FILE_BROWSER_TMP_PUBLIC,TmpPubDir) < 0)
	 Err_NotEnoughMemoryExit ();

      /***** Show media *****/
      HTM_TxtF ("<video src=\"%s/%s\""
		" preload=\"metadata\" controls=\"controls\""
		" class=\"%s\"",
	        URL,FileNameVideo,ClassMedia);
      if (Media->Title)
	 if (Media->Title[0])
	    HTM_TxtF (" title=\"%s\"",Media->Title);
      HTM_Txt (" lazyload=\"on\">"	// Lazy load of the media
               "Your browser does not support HTML5 video."
	       "</video>");
      free (URL);
     }
   else
      HTM_Txt (Txt_File_not_found);

   free (FullPathVideoPriv);
  }

/*****************************************************************************/
/*********************** Show an embed YouTube video *************************/
/*****************************************************************************/

static void Med_ShowYoutube (const struct Med_Media *Media,const char *ClassMedia)
  {
   /***** Check if YouTube code exists *****/
   if (Media->Name[0])	// YouTube code
      switch (Gbl.Usrs.Me.UsrDat.Prefs.RefuseAcceptCookies)
        {
	 case Coo_REFUSE:
	    /***** Alert to inform about third party cookies *****/
	    Med_AlertThirdPartyCookies ();
	    break;
	 case Coo_ACCEPT:
	    /***** Show linked external media *****/
	    // Example of code given by YouTube:
	    // <iframe width="560" height="315"
	    // 	src="https://www.youtube.com/embed/xu9IbeF9CBw"
	    // 	frameborder="0"
	    // 	allow="accelerometer; autoplay; encrypted-media;
	    // 	gyroscope; picture-in-picture" allowfullscreen>
	    // </iframe>
	    /* Begin container */
	    HTM_DIV_Begin ("class=\"MED_VIDEO_CONT\"");

	       /* iframe for video */
	       HTM_TxtF ("<iframe src=\"https://www.youtube.com/embed/%s\""
			 " frameborder=\"0\""
			 " allow=\"accelerometer; autoplay; encrypted-media;"
			 " gyroscope; picture-in-picture\""
			 " allowfullscreen=\"allowfullscreen\""
			 " class=\"%s\"",
			 Media->Name,ClassMedia);
	       if (Media->Title)
		  if (Media->Title[0])
		     HTM_TxtF (" title=\"%s\"",Media->Title);
	       HTM_Txt (">"
			"</iframe>");

	    /* End container */
	    HTM_DIV_End ();
	    break;
        }
  }

/*****************************************************************************/
/*************************** Show an embed media *****************************/
/*****************************************************************************/

static void Med_ShowEmbed (const struct Med_Media *Media,const char *ClassMedia)
  {
   /***** Check if embed URL exists *****/
   if (Media->URL[0])	// Embed URL
      switch (Gbl.Usrs.Me.UsrDat.Prefs.RefuseAcceptCookies)
        {
	 case Coo_REFUSE:
	    /***** Alert to inform about third party cookies *****/
	    Med_AlertThirdPartyCookies ();
	    break;
	 case Coo_ACCEPT:
	    /***** Show linked external media *****/
	    // Example of code given by Slideshare:
	    // <iframe src="//www.slideshare.net/slideshow/embed_code/key/yngasD9sIZ7GQV"
	    // 	width="595" height="485" frameborder="0"
	    // 	marginwidth="0" marginheight="0" scrolling="no"
	    // 	style="border:1px solid #CCC; border-width:1px; margin-bottom:5px; max-width: 100%;"
	    // 	allowfullscreen>
	    // </iframe>
	    /* Begin container */
	    HTM_DIV_Begin ("class=\"MED_EMBED_CONT\"");

	       /* iframe for external media */
	       HTM_TxtF ("<iframe src=\"%s\""
			 " frameborder=\"0\""
			 " marginwidth=\"0\" marginheight=\"0\" scrolling=\"no\""
			 " allowfullscreen=\"allowfullscreen\""
			 " class=\"%s\"",
			 Media->URL,ClassMedia);
	       if (Media->Title)
		  if (Media->Title[0])
		     HTM_TxtF (" title=\"%s\"",Media->Title);
	       HTM_Txt (">"
			"</iframe>");

	    /* End container */
	    HTM_DIV_End ();
	    break;
        }
  }

/*****************************************************************************/
/********** Remove several media files and entries in database ***************/
/*****************************************************************************/

static void Med_AlertThirdPartyCookies (void)
  {
   extern const char *Txt_To_watch_multimedia_content_from_another_website_you_have_to_accept_third_party_cookies_in_your_personal_settings;
   extern const char *Txt_Settings;

   /***** Alert to inform about third party cookies *****/
   /* Begin alert */
   Ale_ShowAlertAndButtonBegin (Ale_INFO,Txt_To_watch_multimedia_content_from_another_website_you_have_to_accept_third_party_cookies_in_your_personal_settings);

   /* Put form to change cookies preferences */
   if (!Frm_CheckIfInside ())
      Lay_PutContextualLinkIconText (ActReqEdiSet,Coo_COOKIES_ID,
                                     NULL,NULL,
				     "cog.svg",Ico_BLACK,
				     Txt_Settings,NULL);

   /* End alert */
   Ale_ShowAlertAndButtonEnd (ActUnk,NULL,NULL,
                            NULL,NULL,
                            Btn_NO_BUTTON,NULL);
  }

/*****************************************************************************/
/************************* Create duplicate of media *************************/
/*****************************************************************************/

#define Med_NUM_MEDIA 2
#define Med_SRC 0
#define Med_DST 1

long Med_CloneMedia (const struct Med_Media *MediaSrc)
  {
   long MedCod = -1L;
   struct Med_Media MediaDst;
   struct
     {
      char Path[PATH_MAX + 1];
      char FullPath[PATH_MAX + 1 + NAME_MAX + 1];
     } MediaPriv[Med_NUM_MEDIA];
   size_t Length;

   /***** If no media ==> nothing to do *****/
   if (MediaSrc->Type == Med_TYPE_NONE)
      return MedCod;

   /***** If no media name ==> nothing to do *****/
   if (!MediaSrc->Name)
      return MedCod;
   if (!MediaSrc->Name[0])
      return MedCod;

   /***** Initialize media *****/
   Med_MediaConstructor (&MediaDst);

   /***** Copy type *****/
   MediaDst.Type = MediaSrc->Type;

   /***** Assign a unique name for the destination media *****/
   Cry_CreateUniqueNameEncrypted (MediaDst.Name);

   /***** Copy media URL *****/
   Med_FreeMediaURL (&MediaDst);
   if (MediaSrc->URL)
     {
      /* Get and limit length of the URL */
      Length = strlen (MediaSrc->URL);
      if (Length > WWW_MAX_BYTES_WWW)
	  Length = WWW_MAX_BYTES_WWW;
      if ((MediaDst.URL = malloc (Length + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
      Str_Copy (MediaDst.URL,MediaSrc->URL,Length);
     }

   /***** Copy media title *****/
   Med_FreeMediaTitle (&MediaDst);
   if (MediaSrc->Title)
     {
      /* Get and limit length of the title */
      Length = strlen (MediaSrc->Title);
      if (Length > WWW_MAX_BYTES_WWW)
	  Length = WWW_MAX_BYTES_WWW;
      if ((MediaDst.Title = malloc (Length + 1)) == NULL)
         Err_NotEnoughMemoryExit ();
      Str_Copy (MediaDst.Title,MediaSrc->Title,Length);
     }

   /***** Create duplicate of files *****/
   switch (MediaSrc->Type)
     {
      case Med_JPG:
      case Med_GIF:
      case Med_MP4:
      case Med_WEBM:
      case Med_OGG:
	 /***** Create private directories if not exist *****/
	 /* Create private directory for images/videos if it does not exist */
	 Fil_CreateDirIfNotExists (Cfg_PATH_MEDIA_PRIVATE);

	 /* Build paths to private directories */
	 snprintf (MediaPriv[Med_SRC].Path,sizeof (MediaPriv[Med_SRC].Path),
		   "%s/%c%c",
		   Cfg_PATH_MEDIA_PRIVATE,MediaSrc->Name[0],MediaSrc->Name[1]);
	 snprintf (MediaPriv[Med_DST].Path,sizeof (MediaPriv[Med_DST].Path),
		   "%s/%c%c",
		   Cfg_PATH_MEDIA_PRIVATE,MediaDst.Name[0],MediaDst.Name[1]);
	 Fil_CreateDirIfNotExists (MediaPriv[Med_DST].Path);

	 /* Build paths to private files */
	 snprintf (MediaPriv[Med_SRC].FullPath,
	           sizeof (MediaPriv[Med_SRC].FullPath),"%s/%s.%s",
		   MediaPriv[Med_SRC].Path,
		   MediaSrc->Name,Med_Extensions[MediaSrc->Type]);
	 snprintf (MediaPriv[Med_DST].FullPath,
	           sizeof (MediaPriv[Med_DST].FullPath),"%s/%s.%s",
		   MediaPriv[Med_DST].Path,
		   MediaDst.Name,Med_Extensions[MediaSrc->Type]);

	 /* Copy file */
	 Fil_FastCopyOfFiles (MediaPriv[Med_SRC].FullPath,
			      MediaPriv[Med_DST].FullPath);

	 if (MediaSrc->Type == Med_GIF)
	   {
	    /* Build private paths to PNG */
	    snprintf (MediaPriv[Med_SRC].FullPath,
	              sizeof (MediaPriv[Med_SRC].FullPath),"%s/%s.png",
		      MediaPriv[Med_SRC].Path,MediaSrc->Name);
	    snprintf (MediaPriv[Med_DST].FullPath,
	              sizeof (MediaPriv[Med_DST].FullPath),"%s/%s.png",
		      MediaPriv[Med_DST].Path,MediaDst.Name);

	    /* Copy PNG file */
	    Fil_FastCopyOfFiles (MediaPriv[Med_SRC].FullPath,
				 MediaPriv[Med_DST].FullPath);
	   }
	 break;
      default:
	 break;
     }

   /***** Code to return *****/
   Med_StoreMediaInDB (&MediaDst);
   MedCod = MediaDst.MedCod;

   /***** Free media *****/
   Med_MediaDestructor (&MediaDst);

   return MedCod;
  }

/*****************************************************************************/
/********** Remove several media files and entries in database ***************/
/*****************************************************************************/

void Med_RemoveMediaFromAllRows (unsigned NumMedia,MYSQL_RES *mysql_res)
  {
   unsigned NumMed;
   long MedCod;

   /***** Go over result removing media files *****/
   for (NumMed = 0;
	NumMed < NumMedia;
	NumMed++)
     {
      /***** Get media code *****/
      MedCod = DB_GetNextCode (mysql_res);

      /***** Remove media files *****/
      Med_RemoveMedia (MedCod);
     }
  }

/*****************************************************************************/
/********** Remove one media from filesystem and from database ***************/
/*****************************************************************************/

void Med_RemoveMedia (long MedCod)
  {
   char PathPriv[PATH_MAX + 1];
   char *FullPathPriv;
   struct Med_Media Media;

   /***** Trivial case *****/
   if (MedCod <= 0)
      return;

   /***** Initialize media *****/
   Med_MediaConstructor (&Media);

   /***** Get media data *****/
   Media.MedCod = MedCod;
   Med_GetMediaDataByCod (&Media);

   /***** Step 1. Remove media files from filesystem *****/
   switch (Media.Type)
     {
      case Med_JPG:
      case Med_GIF:
      case Med_MP4:
      case Med_WEBM:
      case Med_OGG:
	 if (Media.Name[0])
	   {
	    /***** Build path to private directory with the media *****/
	    snprintf (PathPriv,sizeof (PathPriv),"%s/%c%c",
		      Cfg_PATH_MEDIA_PRIVATE,
		      Media.Name[0],
		      Media.Name[1]);

	    /***** Remove files *****/
	    switch (Media.Type)
	      {
	       case Med_JPG:
		  /***** Remove private JPG file *****/
		  if (asprintf (&FullPathPriv,"%s/%s.%s",
			        PathPriv,Media.Name,Med_Extensions[Med_JPG]) < 0)
		     Err_NotEnoughMemoryExit ();
		  unlink (FullPathPriv);
                  free (FullPathPriv);
		  break;
	       case Med_GIF:
		  /***** Remove private GIF file *****/
		  if (asprintf (&FullPathPriv,"%s/%s.%s",
			        PathPriv,Media.Name,Med_Extensions[Med_GIF]) < 0)
		     Err_NotEnoughMemoryExit ();
		  unlink (FullPathPriv);
                  free (FullPathPriv);

		  /***** Remove private PNG file *****/
		  if (asprintf (&FullPathPriv,"%s/%s.png",
			        PathPriv,Media.Name) < 0)
		     Err_NotEnoughMemoryExit ();
		  unlink (FullPathPriv);
                  free (FullPathPriv);

		  break;
	       case Med_MP4:
	       case Med_WEBM:
	       case Med_OGG:
		  /***** Remove private video file *****/
		  if (asprintf (&FullPathPriv,"%s/%s.%s",
			        PathPriv,Media.Name,Med_Extensions[Media.Type]) < 0)
		     Err_NotEnoughMemoryExit ();
		  unlink (FullPathPriv);
		  free (FullPathPriv);

		  break;
	       default:
		  break;
	      }
	    // Public links are removed automatically after a period
	   }
	 break;
      default:
	 break;
     }

   /***** Step 2. Remove entry for this media from database *****/
   Med_DB_RemoveMedia (MedCod);

   /***** Free media *****/
   Med_MediaDestructor (&Media);
  }

/*****************************************************************************/
/************************ Get media type from extension **********************/
/*****************************************************************************/

static Med_Type_t Med_GetTypeFromExtAndMIME (const char *Extension,
                                             const char *MIMEType)
  {
   /***** Extensions and MIME types allowed to convert to JPG *****/
   if (!strcasecmp (Extension,"jpg" ) ||
       !strcasecmp (Extension,"jpeg") ||
       !strcasecmp (Extension,"png" ) ||
       !strcasecmp (Extension,"tif" ) ||
       !strcasecmp (Extension,"tiff") ||
       !strcasecmp (Extension,"bmp" ))
      if (!strcmp (MIMEType,"image/jpeg"              ) ||
          !strcmp (MIMEType,"image/pjpeg"             ) ||
          !strcmp (MIMEType,"image/png"               ) ||
          !strcmp (MIMEType,"image/x-png"             ) ||
	  !strcmp (MIMEType,"image/tiff"              ) ||
	  !strcmp (MIMEType,"image/bmp"               ) ||
          !strcmp (MIMEType,"application/octet-stream") ||
	  !strcmp (MIMEType,"application/octetstream" ) ||
	  !strcmp (MIMEType,"application/octet"       ))
	 return Med_JPG;

   /***** Extensions and MIME types allowed to convert to GIF *****/
   if (!strcasecmp (Extension,"gif"))
      if (!strcmp (MIMEType,"image/gif"               ) ||
          !strcmp (MIMEType,"application/octet-stream") ||
	  !strcmp (MIMEType,"application/octetstream" ) ||
	  !strcmp (MIMEType,"application/octet"       ))
	 return Med_GIF;

   /***** Extensions and MIME types allowed to convert to MP4 *****/
   if (!strcasecmp (Extension,"mp4"))
      if (!strcmp (MIMEType,"video/mp4"               ) ||
          !strcmp (MIMEType,"application/octet-stream") ||
	  !strcmp (MIMEType,"application/octetstream" ) ||
	  !strcmp (MIMEType,"application/octet"       ))
	 return Med_MP4;

   /***** Extensions and MIME types allowed to convert to WEBM *****/
   if (!strcasecmp (Extension,"webm"))
      if (!strcmp (MIMEType,"video/webm"              ) ||
          !strcmp (MIMEType,"application/octet-stream") ||
	  !strcmp (MIMEType,"application/octetstream" ) ||
	  !strcmp (MIMEType,"application/octet"       ))
	 return Med_WEBM;

   /***** Extensions and MIME types allowed to convert to OGG *****/
   if (!strcasecmp (Extension,"ogg"))
      if (!strcmp (MIMEType,"video/ogg"               ) ||
          !strcmp (MIMEType,"application/octet-stream") ||
	  !strcmp (MIMEType,"application/octetstream" ) ||
	  !strcmp (MIMEType,"application/octet"       ))
	 return Med_OGG;

   return Med_TYPE_NONE;
  }

/*****************************************************************************/
/******* Write error message when a media file could not be processed ********/
/*****************************************************************************/

static void Med_ErrorProcessingMediaFile (void)
  {
   extern const char *Txt_The_file_could_not_be_processed_successfully;

   Ale_ShowAlert (Ale_ERROR,Txt_The_file_could_not_be_processed_successfully);
  }
