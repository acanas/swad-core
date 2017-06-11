// swad_photo.c: Users' photos management

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <math.h>		// For log10, floor, ceil, modf, sqrt...
#include <stdlib.h>		// For system, getenv, etc.
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_action.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrolment.h"
#include "swad_file.h"
#include "swad_file_browser.h"
#include "swad_global.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_table.h"
#include "swad_theme.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

const char *Pho_StrAvgPhotoDirs[Pho_NUM_AVERAGE_PHOTO_TYPES] =
  {
   Cfg_FOLDER_DEGREE_PHOTO_MEDIAN,
   Cfg_FOLDER_DEGREE_PHOTO_AVERAGE,
  };
const char *Pho_StrAvgPhotoPrograms[Pho_NUM_AVERAGE_PHOTO_TYPES] =
  {
   Cfg_COMMAND_DEGREE_PHOTO_MEDIAN,
   Cfg_COMMAND_DEGREE_PHOTO_AVERAGE,
  };

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Pho_PutIconToRequestRemoveMyPhoto (void);
static void Pho_PutIconToRequestRemoveOtherUsrPhoto (void);
static void Pho_ReqMyPhoto (void);
static void Pho_ReqOtherUsrPhoto (void);

static void Pho_ReqPhoto (const struct UsrData *UsrDat,const char *PhotoURL);

static bool Pho_ReceivePhotoAndDetectFaces (bool ItsMe,const struct UsrData *UsrDat);

static void Pho_UpdatePhoto1 (struct UsrData *UsrDat);
static void Pho_UpdatePhoto2 (void);
static void Pho_ClearPhotoName (long UsrCod);

static long Pho_GetDegWithAvgPhotoLeastRecentlyUpdated (void);
static long Pho_GetTimeAvgPhotoWasComputed (long DegCod);
static long Pho_GetTimeToComputeAvgPhoto (long DegCod);
static void Pho_ComputeAveragePhoto (long DegCod,Usr_Sex_t Sex,Rol_Role_t Role,
                                     Pho_AvgPhotoTypeOfAverage_t TypeOfAverage,const char *DirAvgPhotosRelPath,
                                     unsigned *NumStds,unsigned *NumStdsWithPhoto,long *TimeToComputeAvgPhotoInMicroseconds);
static void Pho_PutSelectorForTypeOfAvg (void);
static Pho_AvgPhotoTypeOfAverage_t Pho_GetPhotoAvgTypeFromForm (void);
static void Pho_PutSelectorForHowComputePhotoSize (void);
static Pho_HowComputePhotoSize_t Pho_GetHowComputePhotoSizeFromForm (void);
static void Pho_PutSelectorForHowOrderDegrees (void);
static Pho_HowOrderDegrees_t Pho_GetHowOrderDegreesFromForm (void);

static void Pho_PutIconToPrintDegreeStats (void);
static void Pho_PutLinkToPrintViewOfDegreeStatsParams (void);

static void Pho_PutLinkToCalculateDegreeStats (void);
static void Pho_GetMaxStdsPerDegree (void);
static void Pho_ShowOrPrintClassPhotoDegrees (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint);
static void Pho_ShowOrPrintListDegrees (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint);
static void Pho_BuildQueryOfDegrees (char *Query);
static void Pho_GetNumStdsInDegree (long DegCod,Usr_Sex_t Sex,int *NumStds,int *NumStdsWithPhoto);
static void Pho_UpdateDegStats (long DegCod,Usr_Sex_t Sex,unsigned NumStds,unsigned NumStdsWithPhoto,long TimeToComputeAvgPhoto);
static void Pho_ShowDegreeStat (int NumStds,int NumStdsWithPhoto);
static void Pho_ShowDegreeAvgPhotoAndStat (struct Degree *Deg,
                                           Pho_AvgPhotoSeeOrPrint_t SeeOrPrint,
                                           Usr_Sex_t Sex,
                                           int NumStds,int NumStdsWithPhoto);
static void Pho_ComputePhotoSize (int NumStds,int NumStdsWithPhoto,unsigned *PhotoWidth,unsigned *PhotoHeight);

/*****************************************************************************/
/************** Check if I can change the photo of another user **************/
/*****************************************************************************/

bool Pho_ICanChangeOtherUsrPhoto (const struct UsrData *UsrDat)
  {
   if (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      return true;

   /* Check if I have permission to change user's photo */
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_TCH:
	 /* A teacher can change the photo of confirmed students */
         if (UsrDat->Roles.InCurrentCrs.Role == Rol_STD &&	// A student
	     UsrDat->Accepted)					// who accepted registration
            return true;

         return false;
      case Rol_DEG_ADM:
      case Rol_CTR_ADM:
      case Rol_INS_ADM:
      case Rol_SYS_ADM:
         return Usr_ICanEditOtherUsr (UsrDat);
      default:
	 return false;
     }
  }

/*****************************************************************************/
/********** Put a link to the action used to request user's photo ************/
/*****************************************************************************/

void Pho_PutLinkToChangeMyPhoto (void)
  {
   extern const char *Txt_Change_photo;
   extern const char *Txt_Upload_photo;
   const char *TitleText = Gbl.Usrs.Me.MyPhotoExists ? Txt_Change_photo :
		                                       Txt_Upload_photo;

   /***** Link for changing / uploading the photo *****/
   Lay_PutContextualLink (ActReqMyPho,NULL,NULL,
                          "photo64x64.gif",
                          TitleText,TitleText,
                          NULL);
  }

/*****************************************************************************/
/********** Put a link to the action used to request user's photo ************/
/*****************************************************************************/

void Pho_PutLinkToChangeOtherUsrPhoto (void)
  {
   extern const char *Txt_Change_photo;
   extern const char *Txt_Upload_photo;
   bool PhotoExists;
   char PhotoURL[PATH_MAX + 1];
   const char *TitleText;
   Act_Action_t NextAction;

   /***** Link for changing / uploading the photo *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      Pho_PutLinkToChangeMyPhoto ();
   else									// Not me
      if (Pho_ICanChangeOtherUsrPhoto (&Gbl.Usrs.Other.UsrDat))
	{
	 PhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Other.UsrDat,PhotoURL);
	 TitleText = PhotoExists ? Txt_Change_photo :
				   Txt_Upload_photo;
	 switch (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role)
	   {
	    case Rol_STD:
	       NextAction = ActReqStdPho;
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	       NextAction = ActReqTchPho;
	       break;
	    default:	// Guest, user or admin
	       NextAction = ActReqOthPho;
	       break;
	   }
	 Lay_PutContextualLink (NextAction,NULL,
	                        Usr_PutParamOtherUsrCodEncrypted,
	                        "photo64x64.gif",
				TitleText,TitleText,
				NULL);
	}
  }

/*****************************************************************************/
/************** Put a link to request the removal of my photo ****************/
/*****************************************************************************/

static void Pho_PutIconToRequestRemoveMyPhoto (void)
  {
   extern const char *Txt_Remove_photo;

   /***** Link to request the removal of my photo *****/
   if (Gbl.Usrs.Me.MyPhotoExists)
      Lay_PutContextualLink (ActReqRemMyPho,NULL,NULL,
                             "remove-on64x64.png",
			     Txt_Remove_photo,NULL,
		             NULL);
  }

/*****************************************************************************/
/********** Put a link to request the removal of a user's photo **************/
/*****************************************************************************/

static void Pho_PutIconToRequestRemoveOtherUsrPhoto (void)
  {
   extern const char *Txt_Remove_photo;
   char PhotoURL[PATH_MAX + 1];
   bool PhotoExists;
   Act_Action_t NextAction;

   /***** Link to request the removal of another user's photo *****/
   PhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Other.UsrDat,PhotoURL);
   if (PhotoExists)
     {
      switch (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role)
	{
	 case Rol_STD:
	    NextAction = ActReqRemStdPho;
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	    NextAction = ActReqRemTchPho;
	    break;
	 default:	// Guest, user or admin
	    NextAction = ActReqRemOthPho;
	    break;
	}
      Lay_PutContextualLink (NextAction,NULL,
                             Usr_PutParamOtherUsrCodEncrypted,
			     "remove-on64x64.png",
			     Txt_Remove_photo,NULL,
		             NULL);
     }
  }

/*****************************************************************************/
/************************ Form for sending my photo **************************/
/*****************************************************************************/

void Pho_ReqMyPhotoWithContextLinks (void)
  {
   /***** Contextual links to remove photo and change privacy *****/
   if (Gbl.Usrs.Me.MyPhotoExists)	// I have photo
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Pri_PutLinkToChangeMyPrivacy ();	// Put link (form) to change my privacy
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Show the form to send my photo *****/
   Pho_ReqMyPhoto ();
  }

static void Pho_ReqMyPhoto (void)
  {
   /***** Show the form for sending the photo *****/
   Pho_ReqPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL);
  }

/*****************************************************************************/
/******************* Form for sending other user's photo *********************/
/*****************************************************************************/

static void Pho_ReqOtherUsrPhoto (void)
  {
   char PhotoURL[PATH_MAX + 1];

   /***** Get photo URL *****/
   Pho_BuildLinkToPhoto (&Gbl.Usrs.Other.UsrDat,PhotoURL);

   /***** Show the form to send another user's photo *****/
   Pho_ReqPhoto (&Gbl.Usrs.Other.UsrDat,PhotoURL);
  }

/*****************************************************************************/
/****************** Show a form for sending an user's photo ******************/
/*****************************************************************************/

static void Pho_ReqPhoto (const struct UsrData *UsrDat,const char *PhotoURL)
  {
   extern const char *Hlp_PROFILE_Photo;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Photo;
   extern const char *Txt_You_can_send_a_file_with_an_image_in_JPEG_format_;
   extern const char *Txt_File_with_the_photo;
   extern const char *Txt_Upload_photo;
   bool ItsMe = (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);
   Act_Action_t NextAction;

   /***** Start frame *****/
   Box_StartBox (NULL,Txt_Photo,
                        ItsMe ? Pho_PutIconToRequestRemoveMyPhoto :
                                Pho_PutIconToRequestRemoveOtherUsrPhoto,
                        Hlp_PROFILE_Photo,
                        false);	// Not closable

   /***** Start form *****/
   if (ItsMe)
      Act_FormStart (ActDetMyPho);
   else
     {
      switch (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role)
	{
	 case Rol_STD:
	    NextAction = ActDetStdPho;
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	    NextAction = ActDetTchPho;
	    break;
	 default:	// Guest, user or admin
	    NextAction = ActDetOthPho;
	    break;
	}
      Act_FormStart (NextAction);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
     }

   /***** Show current photo and help message *****/
   fprintf (Gbl.F.Out,"<p>");
   Pho_ShowUsrPhoto (UsrDat,PhotoURL,
                     "PHOTO186x248",Pho_NO_ZOOM,false);
   fprintf (Gbl.F.Out,"</p>");

   Ale_ShowAlert (Ale_INFO,Txt_You_can_send_a_file_with_an_image_in_JPEG_format_);

   /***** Form to upload photo *****/
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
                      "%s:&nbsp;"
                      "<input type=\"file\" name=\"%s\" accept=\"image/*\""
                      " onchange=\"document.getElementById('%s').submit();\" />"
                      "</label>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_File_with_the_photo,
            Fil_NAME_OF_PARAM_FILENAME_ORG,
            Gbl.Form.Id);

   /***** End form *****/
   Act_FormEnd ();

   /***** End frame *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/*********************** Send another user's photograph **********************/
/*****************************************************************************/

void Pho_SendPhotoUsr (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user whose photo must be sent or removed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Pho_ICanChangeOtherUsrPhoto (&Gbl.Usrs.Other.UsrDat))	// If I have permission to change user's photo...
	{
	 Gbl.Usrs.Other.UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&Gbl.Usrs.Other.UsrDat);
         if (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
	    /***** Form to send my photo *****/
	    Pho_ReqMyPhotoWithContextLinks ();
	 else
	    /***** Form to send another user's photo *****/
	    Pho_ReqOtherUsrPhoto ();
	}
      else
         Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else		// User not found
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/**************** Receive my photo and detect faces on it ********************/
/*****************************************************************************/

void Pho_RecMyPhotoDetFaces (void)
  {
   /***** Receive my photo and detect faces on it *****/
   if (!Pho_ReceivePhotoAndDetectFaces (true,&Gbl.Usrs.Me.UsrDat))
      Pho_ReqMyPhoto ();	// Request my photograph again
  }

/*****************************************************************************/
/********** Receive another user's photo and detect faces on it **************/
/*****************************************************************************/

void Pho_RecOtherUsrPhotoDetFaces (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
     {
      /***** Receive photo *****/
      if (!Pho_ReceivePhotoAndDetectFaces (false,&Gbl.Usrs.Other.UsrDat))
	 Pho_ReqOtherUsrPhoto ();	// Request user's photograph again
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/********************** Request the removal of my photo **********************/
/*****************************************************************************/

void Pho_ReqRemoveMyPhoto (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_your_photo;
   extern const char *Txt_Remove_photo;
   extern const char *Txt_The_photo_no_longer_exists;

   /***** Show current photo and help message *****/
   if (Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL))
     {
      /***** Show question and button to remove my photo *****/
      /* Start alert */
      Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_remove_your_photo);

      /* Show current photo */
      Pho_ShowUsrPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL,
			"PHOTO186x248",Pho_NO_ZOOM,false);

      /* End alert */
      Ale_ShowAlertAndButton2 (ActRemMyPho,NULL,NULL,NULL,
                               Btn_REMOVE_BUTTON,Txt_Remove_photo);
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_The_photo_no_longer_exists);
  }

/*****************************************************************************/
/****************************** Remove my photo ******************************/
/*****************************************************************************/

void Pho_RemoveMyPhoto1 (void)
  {
   /***** Remove photo *****/
   Pho_RemovePhoto (&Gbl.Usrs.Me.UsrDat);

   /***** The link to my photo is not valid now, so build it again before writing the web page *****/
   Gbl.Usrs.Me.MyPhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL);
  }

void Pho_RemoveMyPhoto2 (void)
  {
   /***** Write success / warning message *****/
   Ale_ShowPendingAlert ();
  }

/*****************************************************************************/
/**************** Request the removal of another user's photo ****************/
/*****************************************************************************/

void Pho_ReqRemoveUsrPhoto (void)
  {
   extern const char *Txt_Photo;
   extern const char *Txt_Do_you_really_want_to_remove_the_photo_of_X;
   extern const char *Txt_Remove_photo;
   extern const char *Txt_The_photo_no_longer_exists;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char PhotoURL[PATH_MAX + 1];
   Act_Action_t NextAction;

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
     {
      if (Pho_ICanChangeOtherUsrPhoto (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** Show current photo and help message *****/
	 if (Pho_BuildLinkToPhoto (&Gbl.Usrs.Other.UsrDat,PhotoURL))
	   {
	    /***** Show question and button to remove user's photo *****/
	    /* Start alert */
	    sprintf (Gbl.Alert.Txt,Txt_Do_you_really_want_to_remove_the_photo_of_X,
	             Gbl.Usrs.Other.UsrDat.FullName);
	    Ale_ShowAlertAndButton1 (Ale_QUESTION,Gbl.Alert.Txt);

	    /* Show current photo */
	    Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,PhotoURL,
			      "PHOTO186x248",Pho_NO_ZOOM,false);

	    /* End alert */
	    switch (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role)
	      {
	       case Rol_STD:
		  NextAction = ActRemStdPho;
		  break;
	       case Rol_NET:
	       case Rol_TCH:
		  NextAction = ActRemTchPho;
		  break;
	       default:	// Guest, user or admin
		  NextAction = ActRemOthPho;
		  break;
	      }
	    Ale_ShowAlertAndButton2 (NextAction,NULL,NULL,
	                             Usr_PutParamOtherUsrCodEncrypted,
				     Btn_REMOVE_BUTTON,Txt_Remove_photo);
	   }
	 else
	    Ale_ShowAlert (Ale_INFO,Txt_The_photo_no_longer_exists);
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************ Remove another user's photo ************************/
/*****************************************************************************/

void Pho_RemoveUsrPhoto (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
     {
      /***** Remove photo *****/
      if (Pho_RemovePhoto (&Gbl.Usrs.Other.UsrDat))
         Ale_ShowPendingAlert ();
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/***************** Receive a photo and detect faces on it ********************/
/*****************************************************************************/
// Return false if no "green" faces detected

static bool Pho_ReceivePhotoAndDetectFaces (bool ItsMe,const struct UsrData *UsrDat)
  {
   extern const char *Txt_The_file_is_not_X;
   extern const char *Txt_Could_not_detect_any_face_in_front_position_;
   extern const char *Txt_A_face_marked_in_green_has_been_detected_;
   extern const char *Txt_A_face_marked_in_red_has_been_detected_;
   extern const char *Txt_X_faces_marked_in_green_have_been_detected_;
   extern const char *Txt_X_faces_marked_in_red_have_been_detected_;
   extern const char *Txt_X_faces_have_been_detected_in_front_position_1_Z_;
   extern const char *Txt_X_faces_have_been_detected_in_front_position_Y_Z_;
   extern const char *Txt_Faces_detected;
   char PathPhotosPriv[PATH_MAX + 1];
   char PathPhotosPubl[PATH_MAX + 1];
   struct Param *Param;
   char FileNamePhotoSrc[PATH_MAX + 1];
   char FileNamePhotoTmp[PATH_MAX + 1];	// Full name (including path and .jpg) of the destination temporary file
   char FileNamePhotoMap[PATH_MAX + 1];	// Full name (including path) of the temporary file with the original image with faces
   char FileNameTxtMap[PATH_MAX + 1];	// Full name (including path) of the temporary file with the text neccesary to make the image map
   char PathRelPhoto[PATH_MAX + 1];
   FILE *FileTxtMap = NULL;		// Temporary file with the text neccesary to make the image map. Initialized to avoid warning
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   bool WrongType = false;
   char Command[256 + PATH_MAX];	// Command to call the program of preprocessing of photos
   int ReturnCode;
   int NumLastForm = 0;	// Initialized to avoid warning
   char FormId[32];
   unsigned NumFacesTotal = 0;
   unsigned NumFacesGreen = 0;
   unsigned NumFacesRed = 0;
   unsigned NumFace;
   unsigned X;
   unsigned Y;
   unsigned Radius;
   unsigned BackgroundCode;
   char StrFileName[NAME_MAX + 1];
   Act_Action_t NextAction;

   /***** Creates directories if not exist *****/
   sprintf (PathPhotosPriv,"%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO);
   Fil_CreateDirIfNotExists (PathPhotosPriv);
   sprintf (PathPhotosPriv,"%s/%s/%02u",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
	    (unsigned) (UsrDat->UsrCod % 100));
   Fil_CreateDirIfNotExists (PathPhotosPriv);

   /***** Create directories if not exists
          and remove old temporary files *****/
   /* Create public directory for photos */
   sprintf (PathPhotosPubl,"%s/%s",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO);
   Fil_CreateDirIfNotExists (PathPhotosPubl);

   /* Create temporary directory for photos */
   sprintf (PathPhotosPubl,"%s/%s/%s",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP);
   Fil_CreateDirIfNotExists (PathPhotosPubl);

   /* Remove old temporary files */
   Fil_RemoveOldTmpFiles (PathPhotosPubl,Cfg_TIME_TO_DELETE_PHOTOS_TMP_FILES,false);

   /***** First of all, copy in disk the file received from stdin (really from Gbl.F.Tmp) *****/
   Param = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                     FileNamePhotoSrc,MIMEType);

   /* Check if the file type is image/jpeg or image/pjpeg or application/octet-stream */
   if (strcmp (MIMEType,"image/jpeg"))
      if (strcmp (MIMEType,"image/pjpeg"))
         if (strcmp (MIMEType,"application/octet-stream"))
            if (strcmp (MIMEType,"application/octetstream"))
               if (strcmp (MIMEType,"application/octet"))
                  WrongType = true;
   if (WrongType)
     {
      sprintf (Gbl.Alert.Txt,Txt_The_file_is_not_X,"jpg");
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
      return false;
     }

   /* End the reception of photo in a temporary file */
   sprintf (FileNamePhotoTmp,"%s/%s/%s/%s.jpg",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,
            Cfg_FOLDER_PHOTO_TMP,Gbl.UniqueNameEncrypted);
   if (!Fil_EndReceptionOfFile (FileNamePhotoTmp,Param))
     {
      Ale_ShowAlert (Ale_WARNING,"Error copying file.");
      return false;
     }

   /* Copy the original photo received to private directory.
      The purpose of this copy is only to have a backup used for researching better methods to detect faces in images */
   sprintf (PathRelPhoto,"%s/%s/%02u/%ld_original.jpg",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
            (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
   Fil_FastCopyOfFiles (FileNamePhotoTmp,PathRelPhoto);

   /***** Call to program that makes photo processing / face detection *****/
   sprintf (Command,Cfg_COMMAND_FACE_DETECTION,FileNamePhotoTmp);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running command to process photo and detect faces.");

   /***** Write message depending on return code *****/
   ReturnCode = WEXITSTATUS(ReturnCode);
   switch (ReturnCode)
     {
      case 0:        // Faces detected
         /***** Open text file with text for image map *****/
         sprintf (FileNameTxtMap,"%s/%s/%s/%s_map.txt",
                  Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,
                  Cfg_FOLDER_PHOTO_TMP,Gbl.UniqueNameEncrypted);
         if ((FileTxtMap = fopen (FileNameTxtMap,"rb")) == NULL)
            Lay_ShowErrorAndExit ("Can not read text file with coordinates of detected faces.");

         /***** Read file with coordinates for image map and compute the number of faces *****/
         NumLastForm = Gbl.Form.Num;
         while (!feof (FileTxtMap))
           {
            if (fscanf (FileTxtMap,"%u %u %u %u %s\n",&X,&Y,&Radius,&BackgroundCode,StrFileName) != 5)        // Example of StrFileName = "4924a838630e_016"

               break;
            if (BackgroundCode == 1)
              {
               NumFacesGreen++;
               if (ItsMe)
        	  Act_FormStart (ActUpdMyPho);
               else
        	 {
               	  switch (Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs.Role)
		    {
		     case Rol_STD:
			NextAction = ActUpdStdPho;
			break;
		     case Rol_NET:
		     case Rol_TCH:
			NextAction = ActUpdTchPho;
			break;
		     default:	// Guest, user or admin
			NextAction = ActUpdOthPho;
			break;
		    }
		  Act_FormStart (NextAction);
                  Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
        	 }
               Par_PutHiddenParamString ("FileName",StrFileName);
               Act_FormEnd ();
              }
            else
               NumFacesRed++;
           }
         NumFacesTotal = NumFacesGreen + NumFacesRed;
         break;
      case 1:        // No faces detected
         NumFacesTotal = NumFacesGreen = NumFacesRed = 0;
         break;
      default:        // Error
         sprintf (Gbl.Alert.Txt,"Photo could not be processed successfully.<br />"
                              "Error code returned by the program of processing: %d",
                  ReturnCode);
         Lay_ShowErrorAndExit (Gbl.Alert.Txt);
         break;
     }

   /***** Message to the user about the number of faces detected in the image*****/
   if (NumFacesTotal == 0)
      Ale_ShowAlert (Ale_WARNING,Txt_Could_not_detect_any_face_in_front_position_);
   else if (NumFacesTotal == 1)
     {
      if (NumFacesGreen == 1)
         Ale_ShowAlert (Ale_SUCCESS,Txt_A_face_marked_in_green_has_been_detected_);
      else
         Ale_ShowAlert (Ale_WARNING,Txt_A_face_marked_in_red_has_been_detected_);
     }
   else        // NumFacesTotal > 1
     {
      if (NumFacesRed == 0)
        {
         sprintf (Gbl.Alert.Txt,Txt_X_faces_marked_in_green_have_been_detected_,
                  NumFacesGreen);
         Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
        }
      else if (NumFacesGreen == 0)
        {
         sprintf (Gbl.Alert.Txt,Txt_X_faces_marked_in_red_have_been_detected_,
                  NumFacesRed);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
        }
      else        // NumFacesGreen > 0
        {
         if (NumFacesGreen == 1)
            sprintf (Gbl.Alert.Txt,Txt_X_faces_have_been_detected_in_front_position_1_Z_,
                     NumFacesTotal,NumFacesRed);
         else
            sprintf (Gbl.Alert.Txt,Txt_X_faces_have_been_detected_in_front_position_Y_Z_,
                     NumFacesTotal,NumFacesGreen,NumFacesRed);
         Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
        }
     }

   /***** Create map *****/
   fprintf (Gbl.F.Out,"<map name=\"faces_map\">\n");
   if (NumFacesTotal)
     {
      /***** Read again the file with coordinates and create area shapes *****/
      rewind (FileTxtMap);
      for (NumFace = 0; !feof (FileTxtMap);)
        {
         if (fscanf (FileTxtMap,"%u %u %u %u %s\n",&X,&Y,&Radius,&BackgroundCode,StrFileName) != 5)
            break;
         if (BackgroundCode == 1)
           {
            NumFace++;
            sprintf (FormId,"form_%d",NumLastForm + NumFace);
            fprintf (Gbl.F.Out,"<area shape=\"circle\""
                               " href=\"javascript:document.getElementById('%s').submit();\""
                               " coords=\"%u,%u,%u\">\n",
                     FormId,X,Y,Radius);
           }
        }
      /***** Close text file with text for image map *****/
      fclose (FileTxtMap);
     }
   fprintf (Gbl.F.Out,"</map>\n");

   /***** Show map photo *****/
   sprintf (FileNamePhotoMap,"%s/%s/%s/%s_map.jpg",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,Gbl.UniqueNameEncrypted);
   fprintf (Gbl.F.Out,"<div class=\"TIT CENTER_MIDDLE\">"
                      "<img src=\"%s/%s/%s/%s_map.jpg\""
                      " usemap=\"#faces_map\""
                      " alt=\"%s\" title=\"%s\" />"
                      "</div>",
            Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,
            Gbl.UniqueNameEncrypted,
            Txt_Faces_detected,Txt_Faces_detected);

   /***** Button to send another photo *****/
   return (NumFacesGreen != 0);
  }

/*****************************************************************************/
/***************************** Update my photo *******************************/
/*****************************************************************************/

void Pho_UpdateMyPhoto1 (void)
  {
   Pho_UpdatePhoto1 (&Gbl.Usrs.Me.UsrDat);

   /***** The link to my photo is not valid now, so build it again before writing the web page *****/
   Gbl.Usrs.Me.MyPhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL);
  }

void Pho_UpdateMyPhoto2 (void)
  {
   Pho_UpdatePhoto2 ();
  }

/*****************************************************************************/
/*********************** Update another user's photo *************************/
/*****************************************************************************/

void Pho_UpdateUsrPhoto1 (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user's code from form and user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
      Pho_UpdatePhoto1 (&Gbl.Usrs.Other.UsrDat);
   else
      Ale_ShowAlert (Ale_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

void Pho_UpdateUsrPhoto2 (void)
  {
   Pho_UpdatePhoto2 ();
  }

/*****************************************************************************/
/*************** Update a user's photo with a selected face ******************/
/*****************************************************************************/

static void Pho_UpdatePhoto1 (struct UsrData *UsrDat)
  {
   extern const char *Txt_Photo_has_been_updated;
   char PathPhotoTmp[PATH_MAX + 1];	// Full name (including path and .jpg) of the temporary file with the selected face
   char PathRelPhoto[PATH_MAX + 1];

   /***** Get the name of the file with the selected face *****/
   Par_GetParToText ("FileName",Gbl.Usrs.FileNamePhoto,NAME_MAX);        // Example of FileNamePhoto: "4924a838630e_016"

   /***** Convert the temporary photo resulting of the processing to the current photo of the user *****/
   sprintf (PathPhotoTmp,"%s/%s/%s/%s_paso3.jpg",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,Gbl.Usrs.FileNamePhoto);
   if (Fil_CheckIfPathExists (PathPhotoTmp))        // The file with the selected photo exists
     {
      /* Copy the temporary file of the third (last) step resulting of the processing to the directory of private photos */
      sprintf (PathRelPhoto,"%s/%s/%02u/%ld.jpg",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
               (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
      Fil_FastCopyOfFiles (PathPhotoTmp,PathRelPhoto);

      /* Update public photo name in database */
      Pho_UpdatePhotoName (UsrDat);

      /* Remove the user from the list of users without photo */
      Pho_RemoveUsrFromTableClicksWithoutPhoto (UsrDat->UsrCod);

      Gbl.Alert.Type = Ale_SUCCESS;
      sprintf (Gbl.Alert.Txt,"%s",Txt_Photo_has_been_updated);
     }
   else
     {
      Gbl.Alert.Type = Ale_ERROR;
      sprintf (Gbl.Alert.Txt,"%s","Error updating photo.");
     }
  }

static void Pho_UpdatePhoto2 (void)
  {
   extern const char *Txt_PHOTO_PROCESSING_CAPTIONS[3];
   unsigned NumPhoto;

   /***** Show the three images resulting of the processing *****/
   Tbl_StartTableWide (0);
   fprintf (Gbl.F.Out,"<tr>");
   for (NumPhoto = 0;
        NumPhoto < 3;
        NumPhoto++)
      fprintf (Gbl.F.Out,"<td class=\"DAT CENTER_TOP\" style=\"width:33%%;\">"
                         "<img src=\"%s/%s/%s/%s_paso%u.jpg\""
                         " alt=\"%s\" title=\"%s\""
                         " style=\"width:%upx; height:%upx;\" />"
                         "<br />%s"
                         "</td>",
               Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,
               Gbl.Usrs.FileNamePhoto,NumPhoto + 1,
               Txt_PHOTO_PROCESSING_CAPTIONS[NumPhoto],
               Txt_PHOTO_PROCESSING_CAPTIONS[NumPhoto],
               Pho_PHOTO_REAL_WIDTH,Pho_PHOTO_REAL_HEIGHT,
               Txt_PHOTO_PROCESSING_CAPTIONS[NumPhoto]);
   fprintf (Gbl.F.Out,"</tr>");
   Tbl_EndTable ();

   /***** Show message *****/
   Ale_ShowPendingAlert ();
  }

/*****************************************************************************/
/******************* Update number of clicks without photo *******************/
/*****************************************************************************/

unsigned Pho_UpdateMyClicksWithoutPhoto (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumClicks;

   /***** Get number of clicks without photo from database *****/
   sprintf (Query,"SELECT NumClicks FROM clicks_without_photo"
                  " WHERE UsrCod=%ld",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get number of clicks without photo");

   /***** Update the list of clicks without photo *****/
   if (NumRows)        // The user exists ==> update number of clicks without photo
     {
      /* Get current number of clicks */
      row = mysql_fetch_row (mysql_res);
      sscanf (row[0],"%u",&NumClicks);

      /* Update number of clicks */
      if (NumClicks <= Pho_MAX_CLICKS_WITHOUT_PHOTO)
        {
         sprintf (Query,"UPDATE clicks_without_photo SET NumClicks=NumClicks+1 WHERE UsrCod=%ld",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         DB_QueryUPDATE (Query,"can not update number of clicks without photo");
         NumClicks++;
        }
     }
   else                                        // The user does not exist ==> add him/her
     {
      /* Add the user, with one access */
      sprintf (Query,"INSERT INTO clicks_without_photo"
	             " (UsrCod,NumClicks)"
	             " VALUES"
	             " (%ld,1)",
               Gbl.Usrs.Me.UsrDat.UsrCod);
      DB_QueryINSERT (Query,"can not create number of clicks without photo");
      NumClicks = 1;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Return the number of rows of the result *****/
   return NumClicks;
  }

/*****************************************************************************/
/******** Remove user from table with number of clicks without photo *********/
/*****************************************************************************/

void Pho_RemoveUsrFromTableClicksWithoutPhoto (long UsrCod)
  {
   char Query[512];

   sprintf (Query,"DELETE FROM clicks_without_photo WHERE UsrCod=%ld",UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from the list of users without photo");
  }

/*****************************************************************************/
/********************* Check if user's photo can be shown ********************/
/*****************************************************************************/
// Returns true if the photo can be shown and false if not.
// Public photo means two different things depending on the user's type

bool Pho_ShowingUsrPhotoIsAllowed (struct UsrData *UsrDat,char *PhotoURL)
  {
   bool ICanSeePhoto;

   /***** Check if I can see the other's photo *****/
   ICanSeePhoto = Pri_ShowingIsAllowed (UsrDat->PhotoVisibility,UsrDat);

   /***** Photo is shown if I can see it, and it exists *****/
   return ICanSeePhoto ? Pho_BuildLinkToPhoto (UsrDat,PhotoURL) :
	                 false;
  }

/*****************************************************************************/
/***************** Create a public link to a private photo *******************/
/*****************************************************************************/
// Returns false if photo does not exist
// Returns true if link is created successfully

bool Pho_BuildLinkToPhoto (const struct UsrData *UsrDat,char *PhotoURL)
  {
   char PathPublPhoto[PATH_MAX + 1];
   char PathPrivPhoto[PATH_MAX + 1];

   if (UsrDat->Photo[0])
     {
      /***** Make path to public photo *****/
      sprintf (PathPublPhoto,"%s/%s/%s.jpg",
               Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,UsrDat->Photo);

      /***** Make path to private photo from public directory *****/
      sprintf (PathPrivPhoto,"%s/%s/%02u/%ld.jpg",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
               (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);

      /***** Create a symbolic link to the private photo, if not exists *****/
      if (!Fil_CheckIfPathExists (PathPublPhoto))
         if (symlink (PathPrivPhoto,PathPublPhoto) != 0)
            Lay_ShowErrorAndExit ("Can not create public link"
                                 " to access to user's private photo");

      /***** Create the public URL of the photo *****/
      sprintf (PhotoURL,"%s/%s/%s.jpg",
               Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,UsrDat->Photo);

      return true;
     }
   else
     {
      PhotoURL[0] = '\0';
      return false;
     }
  }

/*****************************************************************************/
/*************** Check if photo exists and return path to it *****************/
/*****************************************************************************/
// Returns false if photo does not exist
// Returns true if photo exists

bool Pho_CheckIfPrivPhotoExists (long UsrCod,char *PathPrivRelPhoto)
  {
   /***** Make path to private photo *****/
   sprintf (PathPrivRelPhoto,"%s/%s/%02u/%ld.jpg",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
            (unsigned) (UsrCod % 100),UsrCod);

   return Fil_CheckIfPathExists (PathPrivRelPhoto);
  }

/*****************************************************************************/
/************************** Remove a user's photo ****************************/
/*****************************************************************************/
// Returns true on success, false on error

bool Pho_RemovePhoto (struct UsrData *UsrDat)
  {
   extern const char *Txt_Photo_removed;
   char PathPrivRelPhoto[PATH_MAX + 1];
   char PathPublPhoto[PATH_MAX + 1];
   unsigned NumErrors = 0;

   if (UsrDat->Photo[0])
     {
      /***** Clear photo name in database *****/
      Pho_ClearPhotoName (UsrDat->UsrCod);

      /***** Remove public link *****/
      sprintf (PathPublPhoto,"%s/%s/%s.jpg",
               Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,UsrDat->Photo);
      if (Fil_CheckIfPathExists (PathPublPhoto))	// Public link exists
         if (unlink (PathPublPhoto))			// Remove public link
            NumErrors++;

      /***** Remove photo *****/
      sprintf (PathPrivRelPhoto,"%s/%s/%02u/%ld.jpg",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
               (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
      if (Fil_CheckIfPathExists (PathPrivRelPhoto))        // Photo exists
        {
         if (unlink (PathPrivRelPhoto))                        // Remove photo
            NumErrors++;
        }

      /***** Remove original photo *****/
      sprintf (PathPrivRelPhoto,"%s/%s/%02u/%ld_original.jpg",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
               (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
      if (Fil_CheckIfPathExists (PathPrivRelPhoto))		// Original photo exists
         if (unlink (PathPrivRelPhoto))				// Remove original photo
            NumErrors++;

      /***** Clear photo name in user's data *****/
      UsrDat->Photo[0] = '\0';
     }

   if (NumErrors)
     {
      Gbl.Alert.Type = Ale_ERROR;
      sprintf (Gbl.Alert.Txt,"%s","Error removing photo.");
      return false;
     }
   else
     {
      Gbl.Alert.Type = Ale_SUCCESS;
      sprintf (Gbl.Alert.Txt,"%s",Txt_Photo_removed);
      return true;
     }
  }

/*****************************************************************************/
/****************** Clear photo name of an user in database ******************/
/*****************************************************************************/

static void Pho_ClearPhotoName (long UsrCod)
  {
   char Query[128];

   /***** Clear photo name in user's data *****/
   sprintf (Query,"UPDATE usr_data SET Photo=''"
	          " WHERE UsrCod=%ld",
	    UsrCod);
   DB_QueryUPDATE (Query,"can not clear the name of a user's photo");
  }

/*****************************************************************************/
/***************** Update photo name of an user in database ******************/
/*****************************************************************************/

void Pho_UpdatePhotoName (struct UsrData *UsrDat)
  {
   char Query[512];
   char PathPublPhoto[PATH_MAX + 1];

   /***** Update photo name in database *****/
   sprintf (Query,"UPDATE usr_data SET Photo='%s'"
	          " WHERE UsrCod=%ld",
            Gbl.UniqueNameEncrypted,UsrDat->UsrCod);
   DB_QueryUPDATE (Query,"can not update the name of a user's photo");

   /***** Remove the old symbolic link to photo *****/
   sprintf (PathPublPhoto,"%s/%s/%s.jpg",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,UsrDat->Photo);
   unlink (PathPublPhoto);                // Remove public link

   /***** Update photo name in user's data *****/
   Str_Copy (UsrDat->Photo,Gbl.UniqueNameEncrypted,
             Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
  }

/*****************************************************************************/
/****************** Write code to show the photo of a user *******************/
/*****************************************************************************/

void Pho_ShowUsrPhoto (const struct UsrData *UsrDat,const char *PhotoURL,
                       const char *ClassPhoto,Pho_Zoom_t Zoom,
                       bool FormUnique)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   bool PhotoExists;
   bool PutLinkToPublicProfile = !Gbl.Form.Inside &&						// Only if not inside another form
                                 Act_Actions[Gbl.Action.Act].BrowserWindow == Act_THIS_WINDOW;	// Only in main window
   bool PutZoomCode = Zoom == Pho_ZOOM &&						// Make zoom
                      Act_Actions[Gbl.Action.Act].BrowserWindow == Act_THIS_WINDOW;	// Only in main window
   char IdCaption[Act_MAX_BYTES_ID];

   /***** Start form to go to public profile *****/
   if (PutLinkToPublicProfile)
     {
      if (FormUnique)
	{
	 Act_FormStartUnique (ActSeeOthPubPrf);
         Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
         Act_LinkFormSubmitUnique (NULL,NULL);
        }
      else
	{
	 Act_FormStart (ActSeeOthPubPrf);
         Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
         Act_LinkFormSubmit (NULL,NULL,NULL);
	}
     }

   /***** Hidden div to pass user's name to Javascript *****/
   if (PutZoomCode)
     {
      Act_SetUniqueId (IdCaption);
      fprintf (Gbl.F.Out,"<div id=\"%s\" class=\"NOT_SHOWN\">",
      	       IdCaption);

      /* First name and surnames */
      fprintf (Gbl.F.Out,"<div class=\"ZOOM_TXT_LINE DAT_N\">");	// Limited width
      Usr_WriteFirstNameBRSurnames (UsrDat);
      fprintf (Gbl.F.Out,"</div>");

      /* Institution  full name and institution country */
      if (UsrDat->InsCod > 0)
	{
	 fprintf (Gbl.F.Out,"<div class=\"ZOOM_TXT_LINE DAT_SMALL\">");
	 Ins_WriteInstitutionNameAndCty (UsrDat->InsCod);
	 fprintf (Gbl.F.Out,"</div>");
	}
      /* User's country */
      else if (UsrDat->CtyCod > 0)
	{
	 fprintf (Gbl.F.Out,"<div class=\"ZOOM_TXT_LINE DAT_SMALL\">");
	 Cty_WriteCountryName (UsrDat->CtyCod,
	                       NULL);	// Don't put link to country
	 fprintf (Gbl.F.Out,"</div>");
	}

      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Start image *****/
   fprintf (Gbl.F.Out,"<img src=\"");
   PhotoExists = false;
   if (PhotoURL)
      if (PhotoURL[0])
	 PhotoExists = true;
   if (PhotoExists)
      fprintf (Gbl.F.Out,"%s",PhotoURL);
   else
      fprintf (Gbl.F.Out,"%s/usr_bl.jpg",Gbl.Prefs.IconsURL);
   fprintf (Gbl.F.Out,"\" alt=\"%s\" title=\"%s\""
	              " class=\"%s\"",
            UsrDat->FullName,UsrDat->FullName,
	    ClassPhoto);

   /***** Image zoom *****/
   if (PutZoomCode)
     {
      fprintf (Gbl.F.Out," onmouseover=\"zoom(this,'");
      if (PhotoExists)
	 fprintf (Gbl.F.Out,"%s",PhotoURL);
      else
	 fprintf (Gbl.F.Out,"%s/usr_bl.jpg",Gbl.Prefs.IconsURL);
      fprintf (Gbl.F.Out,"','%s');\" onmouseout=\"noZoom();\"",
               IdCaption);
     }

   /***** End image *****/
   fprintf (Gbl.F.Out," />");

   /***** End form to go to public profile *****/
   if (PutLinkToPublicProfile)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
  }

/*****************************************************************************/
/************************** Change photo visibility **************************/
/*****************************************************************************/

void Pho_ChangePhotoVisibility (void)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];
   extern const char *Txt_The_visibility_of_your_photo_has_changed;
   char Query[128];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.PhotoVisibility = Pri_GetParamVisibility ("VisPho");

   /***** Store public/private photo in database *****/
   sprintf (Query,"UPDATE usr_data SET PhotoVisibility='%s'"
	          " WHERE UsrCod=%ld",
            Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.PhotoVisibility],
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your preference about photo visibility");

   /***** Show alert *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_The_visibility_of_your_photo_has_changed);

   /***** Show form again *****/
   Pri_EditMyPrivacy ();
  }

/*****************************************************************************/
/******** Calculate average photos of all students from each degree **********/
/*****************************************************************************/

void Pho_CalcPhotoDegree (void)
  {
   char PathPhotosPublic[PATH_MAX + 1];
   char PathPhotosTmpPriv[PATH_MAX + 1];
   Pho_AvgPhotoTypeOfAverage_t TypeOfAverage;
   long DegCod = -1L;
   char DirAvgPhotosRelPath[Pho_NUM_AVERAGE_PHOTO_TYPES][PATH_MAX + 1];
   unsigned NumStds,NumStdsWithPhoto;
   Usr_Sex_t Sex;
   long TotalTimeToComputeAvgPhotoInMicroseconds,PartialTimeToComputeAvgPhotoInMicroseconds;

   /***** Get type of average *****/
   Gbl.Stat.DegPhotos.TypeOfAverage = Pho_GetPhotoAvgTypeFromForm ();

   /***** Create public directories for average photos if not exist *****/
   sprintf (PathPhotosPublic,"%s/%s",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO);
   Fil_CreateDirIfNotExists (PathPhotosPublic);
   for (TypeOfAverage = (Pho_AvgPhotoTypeOfAverage_t) 0;
	TypeOfAverage < Pho_NUM_AVERAGE_PHOTO_TYPES;
	TypeOfAverage++)
     {
      sprintf (DirAvgPhotosRelPath[TypeOfAverage],"%s/%s/%s",
               Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Pho_StrAvgPhotoDirs[TypeOfAverage]);
      Fil_CreateDirIfNotExists (DirAvgPhotosRelPath[TypeOfAverage]);
     }

   /***** Creates private directory for lists of users' photos if not exists *****/
   sprintf (PathPhotosTmpPriv,"%s/%s/%s",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP);
   Fil_CreateDirIfNotExists (PathPhotosTmpPriv);

   /***** Remove old private files used for lists *****/
   Fil_RemoveOldTmpFiles (PathPhotosTmpPriv,Cfg_TIME_TO_DELETE_PHOTOS_TMP_FILES,false);

   /***** Get the degree which photo will be computed *****/
   DegCod = Deg_GetAndCheckParamOtherDegCod (1);

   /***** Prevent the computing of an average photo too recently updated *****/
   if (Pho_GetTimeAvgPhotoWasComputed (DegCod) >=
       Gbl.StartExecutionTimeUTC - Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO)
      Lay_ShowErrorAndExit ("Average photo has been computed recently.");

   /***** Get list of students in this degree *****/
   Usr_GetUnorderedStdsCodesInDeg (DegCod);

   for (Sex = (Usr_Sex_t) 0;
	Sex < Usr_NUM_SEXS;
	Sex++)
     {
      TotalTimeToComputeAvgPhotoInMicroseconds = 0;
      for (TypeOfAverage = (Pho_AvgPhotoTypeOfAverage_t) 0;
	   TypeOfAverage < Pho_NUM_AVERAGE_PHOTO_TYPES;
	   TypeOfAverage++)
        {
         /***** Compute average photos of students belonging this degree *****/
         Pho_ComputeAveragePhoto (DegCod,Sex,Rol_STD,
                                  TypeOfAverage,DirAvgPhotosRelPath[TypeOfAverage],
                                  &NumStds,&NumStdsWithPhoto,&PartialTimeToComputeAvgPhotoInMicroseconds);
         TotalTimeToComputeAvgPhotoInMicroseconds += PartialTimeToComputeAvgPhotoInMicroseconds;
        }

      /***** Store stats in database *****/
      Pho_UpdateDegStats (DegCod,Sex,NumStds,NumStdsWithPhoto,TotalTimeToComputeAvgPhotoInMicroseconds);
     }

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STD);

   /***** Show photos *****/
   Pho_ShowOrPrintPhotoDegree (Pho_DEGREES_SEE);
  }

/*****************************************************************************/
/******** Get degree code with average photo least recently updated **********/
/*****************************************************************************/
// Returns -1 when error or when all the degrees have been updated too recently

static long Pho_GetDegWithAvgPhotoLeastRecentlyUpdated (void)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows = 0;
   long DegCod = -1L;

   /***** Delete all the degrees in sta_degrees table not present in degrees table *****/
   Pho_RemoveObsoleteStatDegrees ();

   /***** 1. If a degree is not in table of computed degrees,
             choose it as least recently updated *****/
   /* Get one degree with students not yet computed */
   sprintf (Query,"SELECT DISTINCT degrees.DegCod"
	          " FROM degrees,courses,crs_usr"
                  " WHERE degrees.DegCod=courses.DegCod"
                  " AND courses.CrsCod=crs_usr.CrsCod"
                  " AND crs_usr.Role=%u"
	          " AND degrees.DegCod NOT IN"
	          " (SELECT DISTINCT DegCod FROM sta_degrees)"
	          " LIMIT 1",
            (unsigned) Rol_STD);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get degrees");

   /* If number of rows is 1, then get the degree code */
   if (NumRows == 1)
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get degree code (row[0]) */
      if ((DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of degree.");
     }
   else   // NumRows == 0
     {
      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);

      /***** 2. If all the degrees are in table,
                choose the least recently updated that has students *****/
      /* Get degrees from database */
      sprintf (Query,"SELECT sta_degrees.DegCod"
	             " FROM sta_degrees,courses,crs_usr"
                     " WHERE sta_degrees.TimeAvgPhoto<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')"
                     " AND sta_degrees.DegCod=courses.DegCod"
                     " AND courses.CrsCod=crs_usr.CrsCod"
                     " AND crs_usr.Role=%u"
	             " ORDER BY sta_degrees.TimeAvgPhoto LIMIT 1",
               Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO,
               (unsigned) Rol_STD);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get degrees");

      /* If number of rows is 1, then get the degree code */
      if (NumRows == 1)
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get degree code (row[0]) */
         if ((DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of degree.");
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return DegCod;
  }

/*****************************************************************************/
/* Delete all the degrees in sta_degrees table not present in degrees table **/
/*****************************************************************************/

void Pho_RemoveObsoleteStatDegrees (void)
  {
   char Query[512];

   sprintf (Query,"DELETE FROM sta_degrees"
                  " WHERE DegCod NOT IN (SELECT DegCod FROM degrees)");
   DB_QueryDELETE (Query,"can not remove old degrees from stats");
  }

/*****************************************************************************/
/***************** Get time when average photo was computed ******************/
/*****************************************************************************/

static long Pho_GetTimeAvgPhotoWasComputed (long DegCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long TimeAvgPhotoWasComputed = 0L;

   /***** Get last time an average photo was computed from database *****/
   sprintf (Query,"SELECT MIN(UNIX_TIMESTAMP(TimeAvgPhoto))"
                  " FROM sta_degrees WHERE DegCod=%ld",
            DegCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get last time an average photo was computed");

   if (NumRows == 1)
     {
      /***** Get row *****/
      row = mysql_fetch_row (mysql_res);

      /***** Get time when average photo was computed (row[0]) *****/
      if (row[0])
         sscanf (row[0],"%ld",&TimeAvgPhotoWasComputed);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return TimeAvgPhotoWasComputed;
  }

/*****************************************************************************/
/********************* Get time to compute average photo *********************/
/*****************************************************************************/

static long Pho_GetTimeToComputeAvgPhoto (long DegCod)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   Usr_Sex_t Sex;
   long PartialTimeToComputeAvgPhoto;
   long TotalTimeToComputeAvgPhoto = -1L;

   /***** Get time to compute average photo from database *****/
   sprintf (Query,"SELECT TimeToComputeAvgPhoto FROM sta_degrees"
                  " WHERE DegCod=%ld",DegCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get time to compute average photo");

   /***** Count number of rows in result *****/
   if (NumRows == Usr_NUM_SEXS)
     {
      TotalTimeToComputeAvgPhoto = 0;
      for (Sex = (Usr_Sex_t) 0;
	   Sex < Usr_NUM_SEXS;
	   Sex++)
        {
         /***** Get row *****/
         row = mysql_fetch_row (mysql_res);

         /***** Get time to compute average photo (row[0]) *****/
         PartialTimeToComputeAvgPhoto = -1L;
         if (row[0][0])
            if (sscanf (row[0],"%ld",&PartialTimeToComputeAvgPhoto) != 1)
               PartialTimeToComputeAvgPhoto = -1L;
         if (PartialTimeToComputeAvgPhoto < 0)
           {
            TotalTimeToComputeAvgPhoto = -1L;
            break;
           }
         TotalTimeToComputeAvgPhoto += PartialTimeToComputeAvgPhoto;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return TotalTimeToComputeAvgPhoto;
  }

/*****************************************************************************/
/********************** Compute the average photo of a list ******************/
/*****************************************************************************/
// Returns number of users in list with photo

static void Pho_ComputeAveragePhoto (long DegCod,Usr_Sex_t Sex,Rol_Role_t Role,
                                     Pho_AvgPhotoTypeOfAverage_t TypeOfAverage,const char *DirAvgPhotosRelPath,
                                     unsigned *NumStds,unsigned *NumStdsWithPhoto,long *TimeToComputeAvgPhotoInMicroseconds)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   unsigned NumUsr;
   char PathPrivRelPhoto[PATH_MAX + 1];	// Relative path to private photo, to calculate average face
   char PathRelAvgPhoto[PATH_MAX + 1];
   char FileNamePhotoNames[PATH_MAX + 1];
   FILE *FilePhotoNames = NULL;	// Initialized to avoid warning
   char StrCallToProgram[256];	// Call to photo processing program
   int ReturnCode;
   /* To compute execution time of this function */
   struct timeval tvStartComputingStat;
   struct timeval tvEndComputingStat;
   struct timezone tz;

   /***** Set start time to compute the stats of this degree *****/
   gettimeofday (&tvStartComputingStat,&tz);

   /***** Reset number of students in this degree *****/
   *NumStds = *NumStdsWithPhoto = 0;

   /***** Build name for file with average photo *****/
   sprintf (PathRelAvgPhoto,"%s/%ld_%s.jpg",
            DirAvgPhotosRelPath,DegCod,Usr_StringsSexDB[Sex]);

   /***** Remove old file if exists *****/
   if (Fil_CheckIfPathExists (PathRelAvgPhoto))  // If file exists
      unlink (PathRelAvgPhoto);

   /***** Build names for text file with photo paths *****/
   sprintf (FileNamePhotoNames,"%s/%s/%s/%ld.txt",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,DegCod);
   if ((FilePhotoNames = fopen (FileNamePhotoNames,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open file to compute average photo.");

   /***** Loop writing file names in text file *****/
   for (NumUsr = 0;
	NumUsr < Gbl.Usrs.LstUsrs[Role].NumUsrs;
	NumUsr++)
     {
      Gbl.Usrs.Other.UsrDat.Sex = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].Sex;
      if (Sex == Usr_SEX_ALL || Sex == Gbl.Usrs.Other.UsrDat.Sex)
	{
	 (*NumStds)++;

	 /***** Add photo to file for average face calculation *****/
	 Gbl.Usrs.Other.UsrDat.UsrCod = Gbl.Usrs.LstUsrs[Role].Lst[NumUsr].UsrCod;
	 if (Pho_CheckIfPrivPhotoExists (Gbl.Usrs.Other.UsrDat.UsrCod,PathPrivRelPhoto))
	   {
	    (*NumStdsWithPhoto)++;
	    fprintf (FilePhotoNames,"%s\n",PathPrivRelPhoto);
	   }
	}
     }
   fclose (FilePhotoNames);

   /***** Call to program to calculate average photo *****/
   if (*NumStdsWithPhoto)
     {
      sprintf (StrCallToProgram,"%s %s %s",
	       Pho_StrAvgPhotoPrograms[TypeOfAverage],
	       FileNamePhotoNames,PathRelAvgPhoto);
      ReturnCode = system (StrCallToProgram);
      if (ReturnCode == -1)
	 Lay_ShowErrorAndExit ("Error when running program that computes the average photo.");

      /* Write message depending on the return code */
      if (WEXITSTATUS(ReturnCode))
	 Lay_ShowErrorAndExit ("The average photo has not been computed successfully.");
     }

   /***** Time used to compute the stats of this degree *****/
   if (gettimeofday (&tvEndComputingStat, &tz))
      // Error in gettimeofday
      *TimeToComputeAvgPhotoInMicroseconds = -1L;
   else
     {
      if (tvEndComputingStat.tv_usec < tvStartComputingStat.tv_usec)
	{
	 tvEndComputingStat.tv_sec--;
	 tvEndComputingStat.tv_usec += 1000000L;
	}
      *TimeToComputeAvgPhotoInMicroseconds = (tvEndComputingStat.tv_sec -
	                                      tvStartComputingStat.tv_sec) * 1000000L +
	                                      tvEndComputingStat.tv_usec -
	                                      tvStartComputingStat.tv_usec;
     }
  }

/*****************************************************************************/
/*** Show class photo with average photos of all students from each degree ***/
/*****************************************************************************/

void Pho_ShowPhotoDegree (void)
  {
   /***** Get type of average *****/
   Gbl.Stat.DegPhotos.TypeOfAverage = Pho_GetPhotoAvgTypeFromForm ();

   Pho_ShowOrPrintPhotoDegree (Pho_DEGREES_SEE);
  }

/*****************************************************************************/
/** Print class photo with average photos of all students from each degree ***/
/*****************************************************************************/

void Pho_PrintPhotoDegree (void)
  {
   /***** Get type of average *****/
   Gbl.Stat.DegPhotos.TypeOfAverage = Pho_GetPhotoAvgTypeFromForm ();

   Pho_ShowOrPrintPhotoDegree (Pho_DEGREES_PRINT);
  }

/*****************************************************************************/
/*** Show class photo with average photos of all students from each degree ***/
/*****************************************************************************/

void Pho_ShowOrPrintPhotoDegree (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint)
  {
   extern const char *Hlp_STATS_Degrees;
   extern const char *Txt_Degrees;

   /***** Get photo size from form *****/
   Gbl.Stat.DegPhotos.HowComputePhotoSize = Pho_GetHowComputePhotoSizeFromForm ();

   /***** Get how to order degrees from form *****/
   Gbl.Stat.DegPhotos.HowOrderDegrees = Pho_GetHowOrderDegreesFromForm ();

   /***** Get and update type of list,
          number of columns in class photo
          and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   switch (SeeOrPrint)
     {
      case Pho_DEGREES_SEE:
	 /***** Link to computation of average photos *****/
	 Pho_PutLinkToCalculateDegreeStats ();

	 /***** Start frame *****/
	 Box_StartBox (NULL,Txt_Degrees,
			      Pho_PutIconToPrintDegreeStats,
			      Hlp_STATS_Degrees,
                              false);	// Not closable
	 Tbl_StartTableCenter (2);

	 /***** Put a selector for the type of average *****/
	 Pho_PutSelectorForTypeOfAvg ();

	 /***** Put a selector for the size of photos *****/
	 Pho_PutSelectorForHowComputePhotoSize ();

	 /***** Put a selector for the order of degrees *****/
	 Pho_PutSelectorForHowOrderDegrees ();

	 Tbl_EndTable ();
	 break;
      case Pho_DEGREES_PRINT:
	 /***** Start frame *****/
	 Box_StartBox (NULL,Txt_Degrees,NULL,
	                      NULL,
                              false);	// Not closable
	 break;
     }

   /***** Get maximum number of students
          and maximum number of students with photo
          in all degrees *****/
   Pho_GetMaxStdsPerDegree ();

   /***** Draw the classphoto/list *****/
   switch (Gbl.Usrs.Me.ListType)
     {
      case Usr_LIST_AS_CLASS_PHOTO:
         Pho_ShowOrPrintClassPhotoDegrees (SeeOrPrint);
         break;
      case Usr_LIST_AS_LISTING:
         Pho_ShowOrPrintListDegrees (SeeOrPrint);
         break;
      default:
	 break;
     }

   /***** End frame *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/******************* Put a selector for the type of average ******************/
/*****************************************************************************/

static void Pho_PutSelectorForTypeOfAvg (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Average_type;
   extern const char *Txt_AVERAGE_PHOTO_TYPES[Pho_NUM_AVERAGE_PHOTO_TYPES];
   Pho_AvgPhotoTypeOfAverage_t TypeOfAvg;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"AvgType\" class=\"%s\">%s:</label>"
	              "</td>"
	              "<td class=\"LEFT_MIDDLE\">",
	    The_ClassForm[Gbl.Prefs.Theme],Txt_Average_type);
   Act_FormStart (ActSeePhoDeg);
   Pho_PutHiddenParamPhotoSize ();
   Pho_PutHiddenParamOrderDegrees ();
   Usr_PutParamsPrefsAboutUsrList ();
   fprintf (Gbl.F.Out,"<select id=\"AvgType\" name=\"AvgType\""
                      " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);
   for (TypeOfAvg = (Pho_AvgPhotoTypeOfAverage_t) 0;
	TypeOfAvg < Pho_NUM_AVERAGE_PHOTO_TYPES;
	TypeOfAvg++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) TypeOfAvg);
      if (TypeOfAvg == Gbl.Stat.DegPhotos.TypeOfAverage)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_AVERAGE_PHOTO_TYPES[TypeOfAvg]);
     }
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/**************** Put hidden parameter for the type of average ***************/
/*****************************************************************************/

void Pho_PutHiddenParamTypeOfAvg (void)
  {
   Par_PutHiddenParamUnsigned ("AvgType",(unsigned) Gbl.Stat.DegPhotos.TypeOfAverage);
  }

/*****************************************************************************/
/*********************** Get type of average for photos **********************/
/*****************************************************************************/

static Pho_AvgPhotoTypeOfAverage_t Pho_GetPhotoAvgTypeFromForm (void)
  {
   return (Pho_AvgPhotoTypeOfAverage_t)
	  Par_GetParToUnsignedLong ("AvgType",
                                    0,
                                    Pho_NUM_AVERAGE_PHOTO_TYPES - 1,
                                    (unsigned long) Pho_PHOTO_AVG_DEFAULT);
  }

/*****************************************************************************/
/****************** Put a selector for the size of photos ********************/
/*****************************************************************************/

static void Pho_PutSelectorForHowComputePhotoSize (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Size_of_photos;
   extern const char *Txt_STAT_DEGREE_PHOTO_SIZE[Pho_NUM_HOW_COMPUTE_PHOTO_SIZES];
   Pho_HowComputePhotoSize_t PhoSi;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"PhotoSize\" class=\"%s\">%s:</label>"
	              "</td>"
	              "<td class=\"LEFT_MIDDLE\">",
	    The_ClassForm[Gbl.Prefs.Theme],Txt_Size_of_photos);
   Act_FormStart (ActSeePhoDeg);
   Pho_PutHiddenParamTypeOfAvg ();
   Pho_PutHiddenParamOrderDegrees ();
   Usr_PutParamsPrefsAboutUsrList ();
   fprintf (Gbl.F.Out,"<select id=\"PhotoSize\" name=\"PhotoSize\""
                      " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);
   for (PhoSi = (Pho_HowComputePhotoSize_t) 0;
	PhoSi < Pho_NUM_HOW_COMPUTE_PHOTO_SIZES;
	PhoSi++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) PhoSi);
      if (PhoSi == Gbl.Stat.DegPhotos.HowComputePhotoSize)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_STAT_DEGREE_PHOTO_SIZE[PhoSi]);
     }
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/**************** Put hidden parameter for the size of photos ****************/
/*****************************************************************************/

void Pho_PutHiddenParamPhotoSize (void)
  {
   Par_PutHiddenParamUnsigned ("PhotoSize",(unsigned) Gbl.Stat.DegPhotos.HowComputePhotoSize);
  }

/*****************************************************************************/
/********************* Get how to compute size of photos *********************/
/*****************************************************************************/

static Pho_HowComputePhotoSize_t Pho_GetHowComputePhotoSizeFromForm (void)
  {
   return (Pho_HowComputePhotoSize_t)
	  Par_GetParToUnsignedLong ("PhotoSize",
                                    0,
                                    Pho_NUM_HOW_COMPUTE_PHOTO_SIZES - 1,
                                    (unsigned long) Pho_PHOTO_SIZE_DEFAULT);
  }

/*****************************************************************************/
/****************** Put a selector for the order of degrees ******************/
/*****************************************************************************/

static void Pho_PutSelectorForHowOrderDegrees (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Sort_degrees_by;
   extern const char *Txt_STAT_DEGREE_PHOTO_ORDER[Pho_NUM_HOW_ORDER_DEGREES];
   Pho_HowOrderDegrees_t Order;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"RIGHT_MIDDLE\">"
	              "<label for=\"Order\" class=\"%s\">%s:</label>"
	              "</td>"
	              "<td class=\"LEFT_MIDDLE\">",
	    The_ClassForm[Gbl.Prefs.Theme],Txt_Sort_degrees_by);
   Act_FormStart (ActSeePhoDeg);
   Pho_PutHiddenParamTypeOfAvg ();
   Pho_PutHiddenParamPhotoSize ();
   Usr_PutParamsPrefsAboutUsrList ();
   fprintf (Gbl.F.Out,"<select id=\"Order\" name=\"Order\""
                      " onchange=\"document.getElementById('%s').submit();\">",
            Gbl.Form.Id);
   for (Order = (Pho_HowOrderDegrees_t) 0;
	Order < Pho_NUM_HOW_ORDER_DEGREES;
	Order++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Order);
      if (Order == Gbl.Stat.DegPhotos.HowOrderDegrees)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s</option>",Txt_STAT_DEGREE_PHOTO_ORDER[Order]);
     }
   fprintf (Gbl.F.Out,"</select>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/**************** Put hidden parameter for the order of degrees **************/
/*****************************************************************************/

void Pho_PutHiddenParamOrderDegrees (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Stat.DegPhotos.HowOrderDegrees);
  }

/*****************************************************************************/
/*************************** Get how to order degrees ************************/
/*****************************************************************************/

static Pho_HowOrderDegrees_t Pho_GetHowOrderDegreesFromForm (void)
  {
   return (Pho_HowOrderDegrees_t)
	  Par_GetParToUnsignedLong ("Order",
                                    0,
                                    Pho_NUM_HOW_ORDER_DEGREES - 1,
                                    (unsigned long) Pho_ORDER_DEFAULT);
  }

/*****************************************************************************/
/*************** Put icon to print view the stats of degrees ***************/
/*****************************************************************************/

static void Pho_PutIconToPrintDegreeStats (void)
  {
   Ico_PutContextualIconToPrint (ActPrnPhoDeg,Pho_PutLinkToPrintViewOfDegreeStatsParams);
  }

static void Pho_PutLinkToPrintViewOfDegreeStatsParams (void)
  {
   Pho_PutHiddenParamTypeOfAvg ();
   Pho_PutHiddenParamPhotoSize ();
   Pho_PutHiddenParamOrderDegrees ();
   Usr_PutParamsPrefsAboutUsrList ();
  }

/*****************************************************************************/
/*************** Put a link to calculate the stats of degrees ****************/
/*****************************************************************************/

#define Pho_MAX_BYTES_ESTIMATED_TIME (128 - 1)

static void Pho_PutLinkToCalculateDegreeStats (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Calculate_average_photo_of_a_degree;
   extern const char *Txt_Calculate_average_photo_of_THE_DEGREE_X;
   extern const char *Txt_unknown_TIME;
   extern const char *Txt_time;
   struct ListDegrees Degs;
   unsigned NumDeg;
   struct Degree Deg;
   long EstimatedTimeToComputeAvgPhotoInMicroseconds;
   char StrEstimatedTimeToComputeAvgPhoto[Pho_MAX_BYTES_ESTIMATED_TIME + 1];

   if ((Deg.DegCod = Pho_GetDegWithAvgPhotoLeastRecentlyUpdated ()) > 0)
     {
      /***** Get list of all the degrees *****/
      Deg_GetListAllDegsWithStds (&Degs);

      /***** Get data of the degree from database *****/
      Deg_GetDataOfDegreeByCod (&Deg);

      /***** Start div *****/
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

      /***** Start form *****/
      Act_FormStart (ActCalPhoDeg);
      Pho_PutHiddenParamTypeOfAvg ();
      Pho_PutHiddenParamPhotoSize ();
      Pho_PutHiddenParamOrderDegrees ();
      Usr_PutParamsPrefsAboutUsrList ();
      Act_LinkFormSubmitAnimated (Txt_Calculate_average_photo_of_a_degree,
                                  The_ClassFormBold[Gbl.Prefs.Theme],
                                  NULL);
      Ico_PutCalculateIconWithText (Txt_Calculate_average_photo_of_a_degree,
                                    Txt_Calculate_average_photo_of_THE_DEGREE_X);

      /***** Put selector with all the degrees with students *****/
      fprintf (Gbl.F.Out,"<select name=\"OthDegCod\">");
      for (NumDeg = 0;
	   NumDeg < Degs.Num;
	   NumDeg++)
        {
         /* Get time to compute average photo of this degree */
         EstimatedTimeToComputeAvgPhotoInMicroseconds = Pho_GetTimeToComputeAvgPhoto (Degs.Lst[NumDeg].DegCod);
         if (EstimatedTimeToComputeAvgPhotoInMicroseconds == -1L)
            Str_Copy (StrEstimatedTimeToComputeAvgPhoto,Txt_unknown_TIME,
                      Pho_MAX_BYTES_ESTIMATED_TIME);
         else
            Sta_WriteTime (StrEstimatedTimeToComputeAvgPhoto,
                           EstimatedTimeToComputeAvgPhotoInMicroseconds);

         fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s (%s: %s)</option>",
                  Degs.Lst[NumDeg].DegCod,
                  Degs.Lst[NumDeg].DegCod == Deg.DegCod ? " selected=\"selected\"" :
                                                          ((Pho_GetTimeAvgPhotoWasComputed (Degs.Lst[NumDeg].DegCod) >=
                                                          Gbl.StartExecutionTimeUTC - Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO) ? " disabled=\"disabled\"" :
                                                                	                                                     ""),
                  Degs.Lst[NumDeg].ShrtName,
                  Txt_time,
                  StrEstimatedTimeToComputeAvgPhoto);
        }

      /***** End selector, form and div *****/
      fprintf (Gbl.F.Out,"</select>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");

      /***** Free list of all the degrees with students *****/
      Deg_FreeListDegs (&Degs);
     }
  }

/*****************************************************************************/
/*** Get number of students and number of students with photo in a degree ****/
/*****************************************************************************/

static void Pho_GetMaxStdsPerDegree (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get maximum number of students in a degree from database *****/
   sprintf (Query,"SELECT MAX(NumStds),MAX(NumStdsWithPhoto),"
	          "MAX(NumStdsWithPhoto/NumStds)"
                  " FROM sta_degrees"
                  " WHERE Sex='all' AND NumStds>0");
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get maximum number of students in a degree");

   /***** Count number of rows in result *****/
   if (NumRows == 1)
     {
      row = mysql_fetch_row (mysql_res);

      if (row[0] == NULL)
	 Gbl.Stat.DegPhotos.MaxStds = -1;
      else if (sscanf (row[0],"%d",&Gbl.Stat.DegPhotos.MaxStds) != 1)
	 Gbl.Stat.DegPhotos.MaxStds = -1;

      if (row[1] == NULL)
	 Gbl.Stat.DegPhotos.MaxStdsWithPhoto = -1;
      else if (sscanf (row[1],"%d",&Gbl.Stat.DegPhotos.MaxStdsWithPhoto) != 1)
	 Gbl.Stat.DegPhotos.MaxStdsWithPhoto = -1;

      if (row[2] == NULL)
	 Gbl.Stat.DegPhotos.MaxPercent = -1.0;
      else if (sscanf (row[2],"%f",&Gbl.Stat.DegPhotos.MaxPercent) != 1)
	 Gbl.Stat.DegPhotos.MaxPercent = -1.0;

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
     {
      Gbl.Stat.DegPhotos.MaxStds = Gbl.Stat.DegPhotos.MaxStdsWithPhoto = -1;
      Gbl.Stat.DegPhotos.MaxPercent = -1.0;
     }
  }

/*****************************************************************************/
/************ Show or print the stats of degrees as class photo **************/
/*****************************************************************************/

static void Pho_ShowOrPrintClassPhotoDegrees (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   struct Degree Deg;
   unsigned NumDegsNotEmpty;
   int NumStds;
   int NumStdsWithPhoto;
   bool TRIsOpen = false;

   /***** Get degrees from database *****/
   Pho_BuildQueryOfDegrees (Query);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get degrees");

   if (NumRows)	// Degrees with students found
     {
      /***** Form to select type of list used to display degree photos *****/
      if (SeeOrPrint == Pho_DEGREES_SEE)
	 Usr_ShowFormsToSelectUsrListType (ActSeePhoDeg);
      Tbl_StartTableCenter (0);

      /***** Get and print degrees *****/
      for (NumRow = 0, NumDegsNotEmpty = 0;
	   NumRow < NumRows;
	   NumRow++)
	{
	 /***** Get next degree *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get degree code (row[0]) */
	 if ((Deg.DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of degree.");

	 /* Get data of degree */
	 Deg_GetDataOfDegreeByCod (&Deg);

	 /* Get number of students and number of students with photo in this degree */
	 Pho_GetNumStdsInDegree (Deg.DegCod,Usr_SEX_ALL,&NumStds,&NumStdsWithPhoto);

	 if (NumStds > 0)
	   {
	    if ((NumDegsNotEmpty % Gbl.Usrs.ClassPhoto.Cols) == 0)
	      {
	       fprintf (Gbl.F.Out,"<tr>");
	       TRIsOpen = true;
	      }

	    /***** Show average photo of students belonging to this degree *****/
	    fprintf (Gbl.F.Out,"<td class=\"CLASSPHOTO CENTER_MIDDLE\">");
	    Pho_ShowDegreeAvgPhotoAndStat (&Deg,SeeOrPrint,Usr_SEX_ALL,NumStds,NumStdsWithPhoto);
	    fprintf (Gbl.F.Out,"</td>");

	    if ((++NumDegsNotEmpty % Gbl.Usrs.ClassPhoto.Cols) == 0)
	      {
	       fprintf (Gbl.F.Out,"</tr>");
	       TRIsOpen = false;
	      }
	   }
	}
      if (TRIsOpen)
	 fprintf (Gbl.F.Out,"</tr>");

      Tbl_EndTable ();
     }
   else	// No degrees with students found
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/**************** Show or print the stats of degrees as list *****************/
/*****************************************************************************/

static void Pho_ShowOrPrintListDegrees (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Degree;
   extern const char *Txt_SEX_PLURAL_Abc[Usr_NUM_SEXS];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   unsigned NumDegsNotEmpty;
   int NumStds;
   int NumStdsWithPhoto;
   struct Degree Deg;
   Usr_Sex_t Sex;

   /***** Get degrees from database *****/
   Pho_BuildQueryOfDegrees (Query);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get degrees");

   if (NumRows)	// Degrees with students found
     {
      /***** Class photo start *****/
      if (SeeOrPrint == Pho_DEGREES_SEE)
	 /***** Form to select type of list used to display degree photos *****/
	 Usr_ShowFormsToSelectUsrListType (ActSeePhoDeg);

      /***** Write heading *****/
      Tbl_StartTableCenter (2);
      fprintf (Gbl.F.Out,"<tr>"
			 "<th class=\"RIGHT_TOP\">"
			 "%s"
			 "</th>"
			 "<th class=\"CENTER_TOP\">"
			 "%s"
			 "</th>",
	       Txt_No_INDEX,
	       Txt_Degree);
      for (Sex = (Usr_Sex_t) 0;
	   Sex < Usr_NUM_SEXS;
	   Sex++)
	 fprintf (Gbl.F.Out,"<th class=\"CENTER_TOP\">"
			    "%s"
			    "</th>",
		  Txt_SEX_PLURAL_Abc[Sex]);
      fprintf (Gbl.F.Out,"</tr>");

      /***** Get degrees *****/
      for (NumRow = 0, Gbl.RowEvenOdd = 0, NumDegsNotEmpty = 0;
	   NumRow < NumRows;
	   NumRow++, Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd)
	{
	 /***** Get next degree *****/
	 row = mysql_fetch_row (mysql_res);

	 /* Get degree code (row[0]) */
	 if ((Deg.DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	    Lay_ShowErrorAndExit ("Wrong code of degree.");

	 /* Get data of degree */
	 Deg_GetDataOfDegreeByCod (&Deg);

	 /***** Show logo and name of this degree *****/
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"DAT RIGHT_MIDDLE COLOR%u\">"
			    "%u"
			    "</td>",
		  Gbl.RowEvenOdd,++NumDegsNotEmpty);

	 /***** Show logo and name of this degree *****/
	 fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE COLOR%u\">",
		  Gbl.RowEvenOdd);
	 if (SeeOrPrint == Pho_DEGREES_SEE)
	    Deg_DrawDegreeLogoAndNameWithLink (&Deg,ActSeeDegInf,
					       "DAT","CENTER_TOP");
	 else	// Pho_DEGREES_PRINT
	   {
	    Log_DrawLogo (Sco_SCOPE_DEG,Deg.DegCod,Deg.ShrtName,20,"CENTER_TOP",true);
	    fprintf (Gbl.F.Out,"&nbsp;%s</a>",Deg.FullName);
	   }
	 fprintf (Gbl.F.Out,"</td>");

	 for (Sex = (Usr_Sex_t) 0;
	      Sex < Usr_NUM_SEXS;
	      Sex++)
	   {
	    /***** Show average photo of students belonging to this degree *****/
	    Pho_GetNumStdsInDegree (Deg.DegCod,Sex,&NumStds,&NumStdsWithPhoto);
	    fprintf (Gbl.F.Out,"<td class=\"CLASSPHOTO CENTER_MIDDLE COLOR%u\">",
		     Gbl.RowEvenOdd);
	    if (Gbl.Usrs.Listing.WithPhotos)
	       Pho_ShowDegreeAvgPhotoAndStat (&Deg,SeeOrPrint,Sex,NumStds,NumStdsWithPhoto);
	    else
	       Pho_ShowDegreeStat (NumStds,NumStdsWithPhoto);
	    fprintf (Gbl.F.Out,"</td>");
	   }
	 fprintf (Gbl.F.Out,"</tr>");
	}

      /***** Photos end *****/
      Tbl_EndTable ();
     }
   else	// No degrees with students found!
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****** Build a query to get the degrees ordered by different criteria *******/
/*****************************************************************************/

static void Pho_BuildQueryOfDegrees (char *Query)
  {
   switch (Gbl.Stat.DegPhotos.HowOrderDegrees)
     {
      case Pho_NUMBER_OF_STUDENTS:
         sprintf (Query,"SELECT degrees.DegCod"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all'"
                        " AND sta_degrees.NumStds>0"
                        " AND degrees.DegCod=sta_degrees.DegCod"
                        " ORDER BY sta_degrees.NumStds DESC,sta_degrees.NumStdsWithPhoto DESC,degrees.ShortName");
         break;
      case Pho_NUMBER_OF_PHOTOS:
         sprintf (Query,"SELECT degrees.DegCod"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all'"
                        " AND sta_degrees.NumStds>0"
                        " AND degrees.DegCod=sta_degrees.DegCod"
                        " ORDER BY sta_degrees.NumStdsWithPhoto DESC,sta_degrees.NumStds DESC,degrees.ShortName");
         break;
      case Pho_PERCENT:
         sprintf (Query,"SELECT degrees.DegCod"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all'"
                        " AND sta_degrees.NumStds>0"
                        " AND degrees.DegCod=sta_degrees.DegCod"
                        " ORDER BY sta_degrees.NumStdsWithPhoto/sta_degrees.NumStds DESC,degrees.ShortName");
         break;
      case Pho_DEGREE_NAME:
         sprintf (Query,"SELECT degrees.DegCod"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all'"
                        " AND sta_degrees.NumStds>0"
                        " AND degrees.DegCod=sta_degrees.DegCod"
                        " ORDER BY degrees.ShortName");
         break;
     }
  }

/*****************************************************************************/
/*** Get number of students and number of students with photo in a degree ****/
/*****************************************************************************/

static void Pho_GetNumStdsInDegree (long DegCod,Usr_Sex_t Sex,int *NumStds,int *NumStdsWithPhoto)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get the number of students in a degree from database *****/
   sprintf (Query,"SELECT NumStds,NumStdsWithPhoto FROM sta_degrees"
                  " WHERE DegCod=%ld AND Sex='%s'",
            DegCod,Usr_StringsSexDB[Sex]);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the number of students in a degree");

   if (NumRows == 0)
      *NumStds = *NumStdsWithPhoto = -1;
   else
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%d",NumStds) != 1)
	 *NumStds = -1;
      if (sscanf (row[1],"%d",NumStdsWithPhoto) != 1)
	 *NumStdsWithPhoto = -1;

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/*********************** Update statistics of a degree ***********************/
/*****************************************************************************/

static void Pho_UpdateDegStats (long DegCod,Usr_Sex_t Sex,unsigned NumStds,unsigned NumStdsWithPhoto,long TimeToComputeAvgPhotoInMicroseconds)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   char Query[1024];

   sprintf (Query,"REPLACE INTO sta_degrees"
                  " (DegCod,Sex,NumStds,NumStdsWithPhoto,TimeAvgPhoto,TimeToComputeAvgPhoto)"
                  " VALUES"
                  " (%ld,'%s',%u,%u,NOW(),%ld)",
	    DegCod,Usr_StringsSexDB[Sex],NumStds,NumStdsWithPhoto,TimeToComputeAvgPhotoInMicroseconds);
   DB_QueryREPLACE (Query,"can not save stats of a degree");
  }

/*****************************************************************************/
/******************* Show the average photo of a degree **********************/
/*****************************************************************************/

static void Pho_ShowDegreeStat (int NumStds,int NumStdsWithPhoto)
  {
   extern const char *Txt_photos;

   fprintf (Gbl.F.Out,"<span class=\"DAT\">%d&nbsp;</span>"
                      "<span class=\"DAT_SMALL\">(%d&nbsp;%s,&nbsp;%d%%)</span>",
            NumStds,NumStdsWithPhoto,Txt_photos,
            NumStds > 0 ? (int) (((NumStdsWithPhoto * 100.0) / NumStds) + 0.5) :
        	          0);
  }

/*****************************************************************************/
/******************* Show the average photo of a degree **********************/
/*****************************************************************************/

static void Pho_ShowDegreeAvgPhotoAndStat (struct Degree *Deg,
                                           Pho_AvgPhotoSeeOrPrint_t SeeOrPrint,
                                           Usr_Sex_t Sex,
                                           int NumStds,int NumStdsWithPhoto)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   extern const char *Txt_Go_to_X;
   extern const char *Txt_students_ABBREVIATION;
   extern const char *Txt_SEX_PLURAL_abc[Usr_NUM_SEXS];
   extern const char *Txt_photos;
   unsigned PhotoWidth;
   unsigned PhotoHeight;
   char PathRelAvgPhoto[PATH_MAX + 1];
   char PhotoURL[PATH_MAX + 1];
   char PhotoCaption[512];
   bool ShowDegPhoto;
   char IdCaption[Act_MAX_BYTES_ID];

   /***** Initializations *****/
   PhotoURL[0] = '\0';
   PhotoCaption[0] = '\0';

   /***** Compute photo width and height
          to be proportional to number of students *****/
   Pho_ComputePhotoSize (NumStds,NumStdsWithPhoto,&PhotoWidth,&PhotoHeight);

   /***** Put link to degree *****/
   if (SeeOrPrint == Pho_DEGREES_SEE)
     {
      Act_FormGoToStart (ActSeeDegInf);
      Deg_PutParamDegCod (Deg->DegCod);
      sprintf (Gbl.Title,Txt_Go_to_X,Deg->FullName);
      Act_LinkFormSubmit (Gbl.Title,NULL,NULL);
     }

   /***** Check if photo of degree can be shown *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      ShowDegPhoto = (NumStds > 0);
   else
      ShowDegPhoto = (NumStds > 0 &&
		      NumStdsWithPhoto >= Cfg_MIN_PHOTOS_TO_SHOW_AVERAGE);

   if (ShowDegPhoto)
     {
      sprintf (PathRelAvgPhoto,"%s/%s/%s/%ld_%s.jpg",
	       Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,
	       Pho_StrAvgPhotoDirs[Gbl.Stat.DegPhotos.TypeOfAverage],
	       Deg->DegCod,Usr_StringsSexDB[Sex]);
      if (Fil_CheckIfPathExists (PathRelAvgPhoto))
	{
	 sprintf (PhotoURL,"%s/%s/%s/%ld_%s.jpg",
		  Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,
		  Pho_StrAvgPhotoDirs[Gbl.Stat.DegPhotos.TypeOfAverage],
		  Deg->DegCod,Usr_StringsSexDB[Sex]);
		    if (SeeOrPrint == Pho_DEGREES_SEE)
         if (SeeOrPrint == Pho_DEGREES_SEE)
           {
            /***** Hidden div to pass user's name to Javascript *****/
	    sprintf (PhotoCaption,"%s<br />"
				  "%d&nbsp;%s&nbsp;(%s)<br />"
				  "%d&nbsp;%s&nbsp;(%d%%)",
		     Deg->ShrtName,
		     NumStds,Txt_students_ABBREVIATION,Txt_SEX_PLURAL_abc[Sex],
		     NumStdsWithPhoto,Txt_photos,
		     NumStds > 0 ? (int) (((NumStdsWithPhoto * 100.0) / NumStds) + 0.5) :
				   0);
	    Act_SetUniqueId (IdCaption);
	    fprintf (Gbl.F.Out,"<div id=\"%s\" class=\"NOT_SHOWN\">"
	                       "<div class=\"ZOOM_TXT_LINE DAT_N\">"
			       "%s"
	                       "</div>"
			       "</div>",
		     IdCaption,PhotoCaption);
	   }
	}
     }

   /***** Show photo *****/
   fprintf (Gbl.F.Out,"<img src=\"");
   if (PhotoURL[0])
     {
      fprintf (Gbl.F.Out,"%s\"",PhotoURL);
      if (PhotoCaption[0])
	 fprintf (Gbl.F.Out," onmouseover=\"zoom(this,'%s','%s');\""
	                    " onmouseout=\"noZoom();\"",
		  PhotoURL,IdCaption);
     }
   else
      fprintf (Gbl.F.Out,"%s/usr_bl.jpg\"",Gbl.Prefs.IconsURL);
   fprintf (Gbl.F.Out," alt=\"%s\""
	              " style=\"width:%upx; height:%upx;\" />",
            Deg->ShrtName,
            PhotoWidth,PhotoHeight);

   /***** Caption *****/
   fprintf (Gbl.F.Out,"<div class=\"CLASSPHOTO_CAPTION\">"
	              "%s<br />"
	              "%d&nbsp;%s<br />"
	              "%d&nbsp;%s<br />"
	              "(%d%%)"
	              "</div>",
            Deg->ShrtName,
            NumStds,Txt_students_ABBREVIATION,
            NumStdsWithPhoto,Txt_photos,
            NumStds > 0 ? (int) (((NumStdsWithPhoto * 100.0) / NumStds) + 0.5) :
        	          0);
   if (SeeOrPrint == Pho_DEGREES_SEE)
     {
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
     }
  }

/*****************************************************************************/
/******** Compute photo width and height for class photo of degrees **********/
/*****************************************************************************/
#define MIN_WIDTH_PHOTO		10
#define MIN_HEIGHT_PHOTO	15
#define MIN_PIXELS_PHOTO	(MIN_WIDTH_PHOTO * MIN_HEIGHT_PHOTO)
#define DEF_WIDTH_PHOTO		45
#define DEF_HEIGHT_PHOTO	60
#define DEF_PIXELS_PHOTO	(DEF_WIDTH_PHOTO * DEF_HEIGHT_PHOTO)
#define MAX_WIDTH_PHOTO		90
#define MAX_HEIGHT_PHOTO	120
#define MAX_PIXELS_PHOTO	(MAX_WIDTH_PHOTO * MAX_HEIGHT_PHOTO)

static void Pho_ComputePhotoSize (int NumStds,int NumStdsWithPhoto,unsigned *PhotoWidth,unsigned *PhotoHeight)
  {
   unsigned PhotoPixels = DEF_PIXELS_PHOTO;

   switch (Gbl.Stat.DegPhotos.HowComputePhotoSize)
     {
      case Pho_PROPORTIONAL_TO_NUMBER_OF_STUDENTS:
         if (Gbl.Stat.DegPhotos.MaxStds > 0)
            PhotoPixels = (unsigned) (((float) (MAX_PIXELS_PHOTO - MIN_PIXELS_PHOTO) /
        	                       Gbl.Stat.DegPhotos.MaxStds) * NumStds +
        	                      MIN_PIXELS_PHOTO + 0.5);
         break;
      case Pho_PROPORTIONAL_TO_NUMBER_OF_PHOTOS:
         if (Gbl.Stat.DegPhotos.MaxStdsWithPhoto > 0)
            PhotoPixels = (unsigned) (((float) (MAX_PIXELS_PHOTO - MIN_PIXELS_PHOTO) /
        	                       Gbl.Stat.DegPhotos.MaxStdsWithPhoto) * NumStdsWithPhoto +
        	                      MIN_PIXELS_PHOTO + 0.5);
         break;
      case Pho_PROPORTIONAL_TO_PERCENT:
         if (Gbl.Stat.DegPhotos.MaxPercent > 0.0)
           {
            if (NumStds)
               PhotoPixels = (unsigned) (((float) (MAX_PIXELS_PHOTO - MIN_PIXELS_PHOTO) /
        	                          Gbl.Stat.DegPhotos.MaxPercent) *
        	                         ((float) NumStdsWithPhoto / NumStds) +
        	                         MIN_PIXELS_PHOTO + 0.5);
            else
              {
               *PhotoWidth  = MIN_WIDTH_PHOTO;
               *PhotoHeight = MIN_HEIGHT_PHOTO;
               return;
              }
           }
         break;
      case Pho_UNIQUE_SIZE:
         *PhotoWidth  = DEF_WIDTH_PHOTO;
         *PhotoHeight = DEF_HEIGHT_PHOTO;
         return;
     }
   *PhotoWidth  = (unsigned) (sqrt (((double) Pho_PHOTO_REAL_WIDTH /
	                             (double) Pho_PHOTO_REAL_HEIGHT) *
	                            (double) PhotoPixels) + 0.5);
   *PhotoHeight = (unsigned) (((double) Pho_PHOTO_REAL_HEIGHT /
	                       (double) Pho_PHOTO_REAL_WIDTH) *
	                      (double) *PhotoWidth + 0.5);
  }
