// swad_photo.c: Users' photos management

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_file.h"
#include "swad_file_browser.h"
#include "swad_global.h"
#include "swad_logo.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_theme.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

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

static void Pho_PutLinkToRemoveUsrPhoto (const struct UsrData *UsrDat);
static void Pho_UpdatePhoto1 (struct UsrData *UsrDat);
static void Pho_UpdatePhoto2 (void);
static void Pho_ClearPhotoName (long UsrCod);

static void Pho_PutFormPublicPhoto (void);

static long Pho_GetDegWithAvgPhotoLeastRecentlyUpdated (void);
static long Pho_GetTimeAvgPhotoWasComputed (long DegCod);
static long Pho_GetTimeToComputeAvgPhoto (long DegCod);
static void Pho_ComputeAveragePhoto (long DegCod,Usr_Sex_t Sex,struct ListUsers *LstUsrs,
                                     Pho_AvgPhotoTypeOfAverage_t TypeOfAverage,const char *DirAvgPhotosRelPath,
                                     unsigned *NumStds,unsigned *NumStdsWithPhoto,long *TimeToComputeAvgPhotoInMicroseconds);
static void Pho_PutSelectorForTypeOfAvg (void);
static Pho_AvgPhotoTypeOfAverage_t Pho_GetPhotoAvgTypeFromForm (void);
static void Pho_PutSelectorForHowComputePhotoSize (void);
static Pho_HowComputePhotoSize_t Pho_GetHowComputePhotoSizeFromForm (void);
static void Pho_PutSelectorForHowOrderDegrees (void);
static Pho_HowOrderDegrees_t Pho_GetHowOrderDegreesFromForm (void);
static void Pho_PutLinkToPrintViewOfDegreeStats (void);
static void Pho_PutLinkToCalculateDegreeStats (void);
static void Pho_GetMaxStdsPerDegree (void);
static void Pho_ShowOrPrintClassPhotoDegrees (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint);
static void Pho_ShowOrPrintListDegrees (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint);
static void Pho_BuildQueryOfDegrees (char *Query);
static void Pho_GetNumStdsInDegree (long DegCod,Usr_Sex_t Sex,int *NumStds,int *NumStdsWithPhoto);
static void Pho_UpdateDegStats (long DegCod,Usr_Sex_t Sex,unsigned NumStds,unsigned NumStdsWithPhoto,long TimeToComputeAvgPhoto);
static void Pho_ShowDegreeStat (int NumStds,int NumStdsWithPhoto);
static void Pho_ShowDegreeAvgPhotoAndStat (struct Degree *Deg,Pho_AvgPhotoSeeOrPrint_t SeeOrPrint,Usr_Sex_t Sex,int NumStds,int NumStdsWithPhoto,struct Date *DateAvgPhoto);
static void Pho_ComputePhotoSize (int NumStds,int NumStdsWithPhoto,unsigned *PhotoWidth,unsigned *PhotoHeight);

/*****************************************************************************/
/************** Check if I can change the photo of another user **************/
/*****************************************************************************/

bool Pho_CheckIfICanChangeOtherUsrPhoto (long UsrCod)
  {
   if (UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      return true;

   /* Check if I have permission to change user's photo */
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_ROLE_TEACHER:
	 return Usr_CheckIfUsrBelongsToCrs (UsrCod,Gbl.CurrentCrs.Crs.CrsCod);
      case Rol_ROLE_DEG_ADMIN:
	 /* If I am an administrator of current degree,
	    I only can change the photo of users from current degree */
	 return Usr_CheckIfUsrBelongsToDeg (UsrCod,Gbl.CurrentDeg.Deg.DegCod);
      case Rol_ROLE_SUPERUSER:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/********** Put a link to the action used to request user's photo ************/
/*****************************************************************************/

void Pho_PutLinkToChangeUsrPhoto (const struct UsrData *UsrDat)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Change_photo;
   extern const char *Txt_Upload_photo;
   bool PhotoExists;
   char PhotoURL[PATH_MAX+1];
   const char *Msg;

   /***** Link for changing / uploading the photo *****/
   if (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
     {
      PhotoExists = Gbl.Usrs.Me.MyPhotoExists;
      Act_FormStart (ActReqMyPho);
     }
   else							// Not me
     {
      PhotoExists = Pho_BuildLinkToPhoto (UsrDat,PhotoURL,true);
      Act_FormStart (ActReqUsrPho);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
     }
   Msg = PhotoExists ? Txt_Change_photo :
		       Txt_Upload_photo;
   Act_LinkFormSubmit (Msg,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("photo",Msg,Msg);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/********************* Put a link to remove user's photo *********************/
/*****************************************************************************/

static void Pho_PutLinkToRemoveUsrPhoto (const struct UsrData *UsrDat)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Remove_photo;

   /***** Link for changing / uploading the photo *****/
   if (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      Act_FormStart (ActRemMyPho);
   else							// Not me
     {
      Act_FormStart (ActRemUsrPho);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
     }

   Act_LinkFormSubmit (Txt_Remove_photo,The_ClassFormul[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("delon",Txt_Remove_photo,Txt_Remove_photo);
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/************************ Form for sending my photo **************************/
/*****************************************************************************/

void Pho_ReqMyPhoto (void)
  {
   /* Show the form for sending the photo */
   Pho_ReqPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.MyPhotoExists,Gbl.Usrs.Me.PhotoURL);
  }

/*****************************************************************************/
/******************* Form for sending other user's photo *********************/
/*****************************************************************************/

void Pho_ReqUsrPhoto (struct UsrData *UsrDat)
  {
   bool PhotoExists;
   char PhotoURL[PATH_MAX+1];

   /* Check if user's photo exists and create a link to it */
   PhotoExists = Pho_BuildLinkToPhoto (UsrDat,PhotoURL,true);

   /* Show the form for sending the photo */
   Pho_ReqPhoto (UsrDat,PhotoExists,PhotoURL);
  }

/*****************************************************************************/
/****************** Show a form for sending an user's photo ******************/
/*****************************************************************************/

void Pho_ReqPhoto (const struct UsrData *UsrDat,bool PhotoExists,const char *PhotoURL)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_You_can_send_a_file_with_an_image_in_jpg_format_;
   extern const char *Txt_File_with_the_photo;
   extern const char *Txt_Upload_photo;
   bool ItsMe = (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod);

   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

   /***** Write message about photo presence or ausence *****/
   if (PhotoExists)	// User has photo
     {
      /***** Forms to remove photo and make it public *****/
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
      Pho_PutLinkToRemoveUsrPhoto (UsrDat);
      Pho_PutFormPublicPhoto ();
      fprintf (Gbl.F.Out,"</div>");

      /* Show photo */
      Pho_ShowUsrPhoto (UsrDat,PhotoURL,"PHOTO150x200",true);
     }
   Lay_ShowAlert (Lay_INFO,Txt_You_can_send_a_file_with_an_image_in_jpg_format_);

   /***** Form to send photo *****/
   if (ItsMe)
      Act_FormStart (ActDetMyPho);
   else
     {
      Act_FormStart (ActDetUsrPho);
      Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
     }
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto;\">"
                      "<tr>"
                      "<td class=\"%s\" style=\"text-align:right;\">"
                      "%s:"
                      "</td>"
                      "<td style=\"text-align:left;\">"
                      "<input type=\"file\" name=\"%s\" size=\"40\" maxlength=\"100\" value=\"%ld.jpg\" />"
                      "</td>"
                      "</tr>"
                      "<tr>"
                      "<td colspan=\"2\" style=\"text-align:center;\">"
                      "<input type=\"submit\" value=\"%s\" accept=\"image/jpeg\" />"
                      "</td>"
                      "</tr>"
                      "</table>"
                      "</form>"
                      "</div>",
            The_ClassFormul[Gbl.Prefs.Theme],
            Txt_File_with_the_photo,
            Fil_NAME_OF_PARAM_FILENAME_ORG,
            UsrDat->UsrCod,
            Txt_Upload_photo);
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
      if (Pho_CheckIfICanChangeOtherUsrPhoto (Gbl.Usrs.Other.UsrDat.UsrCod))	// If I have permission to change user's photo...
	{
	 /* Check if uploading the other user's photo is allowed */
	 if (Gbl.Usrs.Me.LoggedRole > Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB)
	   {
	    Gbl.Usrs.Other.UsrDat.Accepted = Usr_GetIfUserHasAcceptedEnrollmentInCurrentCrs (Gbl.Usrs.Other.UsrDat.UsrCod);
	    Pho_ReqUsrPhoto (&Gbl.Usrs.Other.UsrDat);        // Request user's photograph
	   }
	 else
	    Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
	}
      else
         Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else		// User not found
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/**************** Receive my photo and detect faces on it ********************/
/*****************************************************************************/

void Pho_RecMyPhotoDetFaces (void)
  {
   /***** Receive my photo and detect faces on it *****/
   Pho_ReceivePhotoAndDetectFaces (true,&Gbl.Usrs.Me.UsrDat);
  }

/*****************************************************************************/
/********** Receive another user's photo and detect faces on it **************/
/*****************************************************************************/

void Pho_RecUsrPhotoDetFaces (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get password, user type and user's data from database *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
      /***** Receive photo *****/
      Pho_ReceivePhotoAndDetectFaces (false,&Gbl.Usrs.Other.UsrDat);
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/****************************** Remove my photo ******************************/
/*****************************************************************************/

void Pho_RemoveMyPhoto1 (void)
  {
   /***** Remove photo *****/
   Gbl.Error = !Pho_RemovePhoto (&Gbl.Usrs.Me.UsrDat);

   /***** The link to my photo is not valid now, so build it again before writing the web page *****/
   Gbl.Usrs.Me.MyPhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL,true);
  }

void Pho_RemoveMyPhoto2 (void)
  {
   extern const char *Txt_Photo_removed;

   if (!Gbl.Error)
      Lay_ShowAlert (Lay_SUCCESS,Txt_Photo_removed);
  }

/*****************************************************************************/
/************************ Remove another user's photo ************************/
/*****************************************************************************/

void Pho_RemoveUsrPhoto (void)
  {
   extern const char *Txt_Photo_removed;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncrypted ();

   /***** Get password, user type and user's data from database *****/
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
     {
      /***** Remove photo *****/
      if (Pho_RemovePhoto (&Gbl.Usrs.Other.UsrDat))
         Lay_ShowAlert (Lay_SUCCESS,Txt_Photo_removed);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/***************** Receive a photo and detect faces on it ********************/
/*****************************************************************************/

void Pho_ReceivePhotoAndDetectFaces (bool ItsMe,const struct UsrData *UsrDat)
  {
   extern const char *Txt_The_file_is_not_X;
   extern const char *Txt_Could_not_detect_any_face_in_front_position_;
   extern const char *Txt_A_face_marked_in_green_has_been_detected_;
   extern const char *Txt_A_face_marked_in_red_has_been_detected_;
   extern const char *Txt_Try_sending_another_image_where_the_face_appears_in_front_position_;
   extern const char *Txt_X_faces_marked_in_green_have_been_detected_;
   extern const char *Txt_X_faces_marked_in_red_have_been_detected_;
   extern const char *Txt_X_faces_have_been_detected_in_front_position_1_Z_;
   extern const char *Txt_X_faces_have_been_detected_in_front_position_Y_Z_;
   char PathPhotosPriv[PATH_MAX+1];
   char PathPhotosPubl[PATH_MAX+1];
   char PathPhotosTmpPubl[PATH_MAX+1];
   char FileNamePhotoSrc[PATH_MAX+1];
   char FileNamePhotoTmp[PATH_MAX+1];        // Full name (including path and .jpg) of the destination temporary file
   char FileNamePhotoMap[PATH_MAX+1];        // Full name (including path) of the temporary file with the original image with faces
   char FileNameTxtMap[PATH_MAX+1];        // Full name (including path) of the temporary file with the text neccesary to make the image map
   char PathRelPhoto[PATH_MAX+1];
   FILE *FileTxtMap = NULL;        // Temporary file with the text neccesary to make the image map. Initialized to avoid warning
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE+1];
   bool WrongType = false;
   char Command[256+PATH_MAX]; // Command to call the program of preprocessing of photos
   int ReturnCode;
   int NumLastForm = 0;        // Initialized to avoid warning
   char FormId[32];
   unsigned NumFacesTotal = 0;
   unsigned NumFacesGreen = 0;
   unsigned NumFacesRed = 0;
   unsigned NumFace;
   unsigned X;
   unsigned Y;
   unsigned Radius;
   unsigned BackgroundCode;
   char StrFileName[NAME_MAX+1];

   /***** Creates directories if not exist *****/
   sprintf (PathPhotosPriv,"%s/%s",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO);
   Fil_CreateDirIfNotExists (PathPhotosPriv);
   sprintf (PathPhotosPriv,"%s/%s/%02u",
	    Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
	    (unsigned) (UsrDat->UsrCod % 100));
   Fil_CreateDirIfNotExists (PathPhotosPriv);

   sprintf (PathPhotosPubl,"%s/%s",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO);
   Fil_CreateDirIfNotExists (PathPhotosPubl);
   sprintf (PathPhotosTmpPubl,"%s/%s/%s",
	    Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP);
   Fil_CreateDirIfNotExists (PathPhotosTmpPubl);

   /***** Remove old files *****/
   Fil_RemoveOldTmpFiles (PathPhotosTmpPubl,Cfg_TIME_TO_DELETE_PHOTOS_TMP_FILES,false);

   /***** First of all, copy in disk the file received from stdin (really from Gbl.F.Tmp) *****/
   Fil_StartReceptionOfFile (FileNamePhotoSrc,MIMEType);

   /* Check if the file type is image/jpeg or image/pjpeg or application/octet-stream */
   if (strcmp (MIMEType,"image/jpeg"))
      if (strcmp (MIMEType,"image/pjpeg"))
         if (strcmp (MIMEType,"application/octet-stream"))
            if (strcmp (MIMEType,"application/octetstream"))
               if (strcmp (MIMEType,"application/octet"))
                  WrongType = true;
   if (WrongType)
     {
      sprintf (Gbl.Message,Txt_The_file_is_not_X,"jpg");
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
      return;
     }

   /* End the reception of photo in a temporary file */
   sprintf (FileNamePhotoTmp,"%s/%s/%s/%s.jpg",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,
            Cfg_FOLDER_PHOTO_TMP,Gbl.UniqueNameEncrypted);
   if (!Fil_EndReceptionOfFile (FileNamePhotoTmp))
     {
      Lay_ShowAlert (Lay_WARNING,"Error uploading file.");
      return;
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
         NumLastForm = Gbl.NumForm;
         while (!feof (FileTxtMap))
           {
            if (fscanf (FileTxtMap,"%u %u %u %u %s\n",&X,&Y,&Radius,&BackgroundCode,StrFileName) != 5)        // Example of StrFileName = "4924a838630e_016"

               break;
            if (BackgroundCode == 1)
              {
               NumFacesGreen++;
               Act_FormStart (ItsMe ? ActUpdMyPho :
        	                      ActUpdUsrPho);
               if (!ItsMe)
                  Usr_PutParamOtherUsrCodEncrypted (UsrDat->EncryptedUsrCod);
               Par_PutHiddenParamString ("FileName",StrFileName);
               fprintf (Gbl.F.Out,"</form>");
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
         sprintf (Gbl.Message,"Photo could not be processed successfully.<br />"
                              "Error code returned by the program of processing: %d getuid() = %u geteuid() = %u",
                  ReturnCode,(unsigned) getuid(), (unsigned) geteuid());
         Lay_ShowErrorAndExit (Gbl.Message);
         break;
     }

   /***** Message to the user about the number of faces detected in the image*****/
   if (NumFacesTotal == 0)
     {
      Lay_ShowAlert (Lay_WARNING,Txt_Could_not_detect_any_face_in_front_position_);
      Lay_ShowAlert (Lay_INFO,Txt_Try_sending_another_image_where_the_face_appears_in_front_position_);
     }
   else if (NumFacesTotal == 1)
     {
      if (NumFacesGreen == 1)
         Lay_ShowAlert (Lay_SUCCESS,Txt_A_face_marked_in_green_has_been_detected_);
      else
        {
         Lay_ShowAlert (Lay_WARNING,Txt_A_face_marked_in_red_has_been_detected_);
         Lay_ShowAlert (Lay_INFO,Txt_Try_sending_another_image_where_the_face_appears_in_front_position_);
        }
     }
   else        // NumFacesTotal > 1
     {
      if (NumFacesRed == 0)
        {
         sprintf (Gbl.Message,Txt_X_faces_marked_in_green_have_been_detected_,
                  NumFacesGreen);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }
      else if (NumFacesGreen == 0)
        {
         sprintf (Gbl.Message,Txt_X_faces_marked_in_red_have_been_detected_,
                  NumFacesRed);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
         Lay_ShowAlert (Lay_INFO,Txt_Try_sending_another_image_where_the_face_appears_in_front_position_);
        }
      else        // NumFacesGreen > 0
        {
         if (NumFacesGreen == 1)
            sprintf (Gbl.Message,Txt_X_faces_have_been_detected_in_front_position_1_Z_,
                     NumFacesTotal,NumFacesRed);
         else
            sprintf (Gbl.Message,Txt_X_faces_have_been_detected_in_front_position_Y_Z_,
                     NumFacesTotal,NumFacesGreen,NumFacesRed);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
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
   fprintf (Gbl.F.Out,"<div class=\"TIT\" style=\"text-align:center;\">"
                      "<img src=\"%s/%s/%s/%s_map.jpg\" usemap=\"#faces_map\" />"
                      "</div>",
            Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,Gbl.UniqueNameEncrypted);
  }

/*****************************************************************************/
/***************************** Update my photo *******************************/
/*****************************************************************************/

void Pho_UpdateMyPhoto1 (void)
  {
   Pho_UpdatePhoto1 (&Gbl.Usrs.Me.UsrDat);

   /***** The link to my photo is not valid now, so build it again before writing the web page *****/
   Gbl.Usrs.Me.MyPhotoExists = Pho_BuildLinkToPhoto (&Gbl.Usrs.Me.UsrDat,Gbl.Usrs.Me.PhotoURL,true);
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
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
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
   char PathPhotoTmp[PATH_MAX+1];        // Full name (including path and .jpg) of the temporary file with the selected face
   char PathRelPhoto[PATH_MAX+1];

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

      Gbl.Error = false;
     }
   else
      Gbl.Error = true;
  }

static void Pho_UpdatePhoto2 (void)
  {
   extern const char *Txt_PHOTO_PROCESSING_CAPTIONS[3];
   extern const char *Txt_Photo_has_been_updated;
   unsigned NumPhoto;

   /***** Show the three images resulting of the processing *****/
   fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                      "<tr>");
   for (NumPhoto = 0;
        NumPhoto < 3;
        NumPhoto++)
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"width:33%%;"
	                 " text-align:center; vertical-align:top;\">"
                         "<img src=\"%s/%s/%s/%s_paso%u.jpg\""
                         " style=\"width:%upx; height:%upx;\" />"
                         "<br />%s"
                         "</td>",
               Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,
               Gbl.Usrs.FileNamePhoto,NumPhoto + 1,
               Pho_PHOTO_REAL_WIDTH,Pho_PHOTO_REAL_HEIGHT,
               Txt_PHOTO_PROCESSING_CAPTIONS[NumPhoto]);
   fprintf (Gbl.F.Out,"</tr>"
	              "</table>");

   /***** Show message *****/
   if (Gbl.Error)        // The file with the selected photo does not exist!
      Lay_ShowErrorAndExit ("Selected photo does not exist.");

   Lay_ShowAlert (Lay_SUCCESS,Txt_Photo_has_been_updated);
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
                  " WHERE UsrCod='%ld'",
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
         sprintf (Query,"UPDATE clicks_without_photo SET NumClicks=NumClicks+1 WHERE UsrCod='%ld'",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         DB_QueryUPDATE (Query,"can not update number of clicks without photo");
         NumClicks++;
        }
     }
   else                                        // The user does not exist ==> add him/her
     {
      /* Add the user, with one access */
      sprintf (Query,"INSERT INTO clicks_without_photo (UsrCod,NumClicks) VALUES ('%ld',1)",
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

   sprintf (Query,"DELETE FROM clicks_without_photo WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove a user from the list of users without photo");
  }

/*****************************************************************************/
/********************* Check if user's photo can be shown ********************/
/*****************************************************************************/
// Returns true if the photo can be shown and false if not.
// Public photo means two different things depending on the user's type

bool Pho_ShowUsrPhotoIsAllowed (struct UsrData *UsrDat,char *PhotoURL)
  {
   bool ICanSeePhoto = false;

   /***** Check if I can see the other's photo *****/
   if (Gbl.Usrs.Me.UsrDat.UsrCod == UsrDat->UsrCod ||	// It's me
       Gbl.Usrs.Me.LoggedRole == Rol_ROLE_SUPERUSER)
      // I always can see my photo
      // A superuser always can see any user's photo
      ICanSeePhoto = true;
   else if (Gbl.Usrs.Me.MaxRole == Rol_ROLE_STUDENT ||
            Gbl.Usrs.Me.MaxRole == Rol_ROLE_TEACHER ||
            Gbl.Usrs.Me.LoggedRole >= Rol_ROLE_DEG_ADMIN)
     {
      // If the maximum role of both users is student
      if (Gbl.Usrs.Me.MaxRole == Rol_ROLE_STUDENT &&	// My maximum role is student
	   (UsrDat->Roles & (1 << Rol_ROLE_STUDENT)) &&	// He/she is a student in some courses...
          !(UsrDat->Roles & (1 << Rol_ROLE_TEACHER)))	// ...but he/she is not a teacher in any course
	 // A student only can see the photo of another student if both photos are public
	 ICanSeePhoto = (Gbl.Usrs.Me.MyPhotoExists &&		// I have photo...
			 Gbl.Usrs.Me.UsrDat.PublicPhoto &&	// ...and my photo is public...
			 UsrDat->PublicPhoto);			// ...and the other student' photo is also public
      else if (UsrDat->PublicPhoto)	// The photo of the other user is public
	 ICanSeePhoto = true;
      else
	 ICanSeePhoto = Usr_CheckIfUsrSharesAnyOfMyCrs (UsrDat->UsrCod);	// Both users share the same course
     }

   /***** Photo is shown if I can see it, and it exists *****/
   return ICanSeePhoto ? Pho_BuildLinkToPhoto (UsrDat,PhotoURL,true) :
	                 false;
  }

/*****************************************************************************/
/***************** Create a public link to a private photo *******************/
/*****************************************************************************/
// Returns false if photo does not exist
// Returns true if link is created successfully

bool Pho_BuildLinkToPhoto (const struct UsrData *UsrDat,char *PhotoURL,bool HTTPS)
  {
   char PathPublPhoto[PATH_MAX+1];
   char PathPrivPhoto[PATH_MAX+1];

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
           {
            sprintf (Gbl.Message,"symlink (%s,%s)",
                     PathPrivPhoto,PathPublPhoto);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
            Lay_ShowErrorAndExit ("Can not create public link"
                                 " to access to user's private photo");
           }

      /***** Create the public URL of the photo *****/
      sprintf (PhotoURL,"%s/%s/%s.jpg",
               HTTPS ? Cfg_HTTPS_URL_SWAD_PUBLIC :
                       Cfg_HTTP_URL_SWAD_PUBLIC,
               Cfg_FOLDER_PHOTO,UsrDat->Photo);

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
   char PathPrivRelPhoto[PATH_MAX+1];
   char PathPublPhoto[PATH_MAX+1];
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
           {
            Lay_ShowAlert (Lay_ERROR,"Error removing public link to photo.");
            NumErrors++;
           }

      /***** Remove photo *****/
      sprintf (PathPrivRelPhoto,"%s/%s/%02u/%ld.jpg",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
               (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
      if (Fil_CheckIfPathExists (PathPrivRelPhoto))        // Photo exists
        {
         if (unlink (PathPrivRelPhoto))                        // Remove photo
           {
            Lay_ShowAlert (Lay_ERROR,"Error removing photo.");
            NumErrors++;
           }
        }

      /***** Remove original photo *****/
      sprintf (PathPrivRelPhoto,"%s/%s/%02u/%ld_original.jpg",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,
               (unsigned) (UsrDat->UsrCod % 100),UsrDat->UsrCod);
      if (Fil_CheckIfPathExists (PathPrivRelPhoto))		// Original photo exists
         if (unlink (PathPrivRelPhoto))				// Remove original photo
           {
            Lay_ShowAlert (Lay_ERROR,"Error removing the original photo.");
            NumErrors++;
           }

      /***** Clear photo name in user's data *****/
      UsrDat->Photo[0] = '\0';
     }

   return (NumErrors == 0);
  }

/*****************************************************************************/
/****************** Clear photo name of an user in database ******************/
/*****************************************************************************/

static void Pho_ClearPhotoName (long UsrCod)
  {
   char Query[128];

   /***** Clear photo name in user's data *****/
   sprintf (Query,"UPDATE usr_data SET Photo='' WHERE UsrCod='%ld'",UsrCod);
   DB_QueryUPDATE (Query,"can not clear the name of a user's photo");
  }

/*****************************************************************************/
/***************** Update photo name of an user in database ******************/
/*****************************************************************************/

void Pho_UpdatePhotoName (struct UsrData *UsrDat)
  {
   char Query[512];
   char PathPublPhoto[PATH_MAX+1];

   /***** Update photo name in database *****/
   sprintf (Query,"UPDATE usr_data SET Photo='%s' WHERE UsrCod='%ld'",
            Gbl.UniqueNameEncrypted,UsrDat->UsrCod);
   DB_QueryUPDATE (Query,"can not update the name of a user's photo");

   /***** Remove the old symbolic link to photo *****/
   sprintf (PathPublPhoto,"%s/%s/%s.jpg",
            Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,UsrDat->Photo);
   unlink (PathPublPhoto);                // Remove public link

   /***** Update photo name in user's data *****/
   strcpy (UsrDat->Photo,Gbl.UniqueNameEncrypted);
  }

/*****************************************************************************/
/****************** Write code to show the photo of a user *******************/
/*****************************************************************************/

void Pho_ShowUsrPhoto (const struct UsrData *UsrDat,const char *PhotoURL,
                       const char *ClassPhoto,bool Zoom)
  {
   char SpecialFullName [3*(Usr_MAX_BYTES_NAME_SPEC_CHAR+1)+1];
   char SpecialShortName[3*(Usr_MAX_BYTES_NAME_SPEC_CHAR+1)+6];
   char SpecialSurnames [2*(Usr_MAX_BYTES_NAME_SPEC_CHAR+1)+1];

   /* Replace tildes, ñ, etc. in full name by codes,
      because some browsers (i.e., IE5) don't show correctly tildes with AJAX */
   strcpy (SpecialFullName,UsrDat->FullName);
   Str_ReplaceSpecialCharByCodes (SpecialFullName,sizeof (SpecialFullName)-1);

   fprintf (Gbl.F.Out,"<img src=\"");
   if (PhotoURL)
      fprintf (Gbl.F.Out,"%s",PhotoURL);
   else
      fprintf (Gbl.F.Out,"%s/usr_bl.jpg",Gbl.Prefs.IconsURL);
   fprintf (Gbl.F.Out,"\" class=\"%s\"",ClassPhoto);
   if (SpecialFullName[0])
      fprintf (Gbl.F.Out," title=\"%s\"",SpecialFullName);
   if (Zoom && PhotoURL)
     {
      strcpy (SpecialShortName,UsrDat->FirstName);
      Str_LimitLengthHTMLStr (SpecialShortName,23);
      Str_ReplaceSpecialCharByCodes (SpecialShortName,Usr_MAX_BYTES_NAME_SPEC_CHAR);
      SpecialSurnames[0] = '\0';
      if (UsrDat->Surname1[0])
         strcpy (SpecialSurnames,UsrDat->Surname1);
      if (UsrDat->Surname2[0])
        {
         strcat (SpecialSurnames," ");
         strcat (SpecialSurnames,UsrDat->Surname2);
        }
      Str_LimitLengthHTMLStr (SpecialSurnames,23);
      Str_ReplaceSpecialCharByCodes (SpecialSurnames,2*Usr_MAX_BYTES_NAME_SPEC_CHAR+1);
      strcat (SpecialShortName,"<br />");
      strcat (SpecialShortName,SpecialSurnames);
      fprintf (Gbl.F.Out," onmouseover=\"zoom(this,'%s','%s')\" onmouseout=\"noZoom(this);\"",
               PhotoURL,SpecialShortName);
     }
   fprintf (Gbl.F.Out," />");
  }

/*****************************************************************************/
/*********************** Select public / private photo ***********************/
/*****************************************************************************/

static void Pho_PutFormPublicPhoto (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Public_photo;

   /***** Start form *****/
   Act_FormStart (ActChgPubPho);

   /***** Checkbox to select between public or private photo *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"PublicPhoto\" value=\"Y\"");
   if (Gbl.Usrs.Me.UsrDat.PublicPhoto)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," onchange=\"javascript:document.getElementById('%s').submit();\" />"
                      "<span class=\"%s\">%s</span>",
            Gbl.FormId,
            The_ClassFormul[Gbl.Prefs.Theme],Txt_Public_photo);

   /***** End form *****/
   fprintf (Gbl.F.Out,"</form>");
  }

/*****************************************************************************/
/********** Get parameter with public / private photo from form **************/
/*****************************************************************************/

bool Pho_GetParamPublicPhoto (void)
  {
   char YN[1+1];

   Par_GetParToText ("PublicPhoto",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/*********************** Change public / private photo ***********************/
/*****************************************************************************/

void Pho_ChangePublicPhoto (void)
  {
   char Query[512];

   /***** Get param with public/private photo *****/
   Gbl.Usrs.Me.UsrDat.PublicPhoto = Pho_GetParamPublicPhoto ();

   /***** Store public/private photo in database *****/
   sprintf (Query,"UPDATE usr_data SET PublicPhoto='%c' WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.PublicPhoto ? 'Y' :
        	                             'N',
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update your preference about public photo");
  }

/*****************************************************************************/
/******** Calculate average photos of all students from each degree **********/
/*****************************************************************************/

void Pho_CalcPhotoDegree (void)
  {
   char PathPhotosPublic[PATH_MAX+1];
   char PathPhotosTmpPriv[PATH_MAX+1];
   Pho_AvgPhotoTypeOfAverage_t TypeOfAverage;
   long DegCod = -1L;
   char DirAvgPhotosRelPath[Pho_NUM_AVERAGE_PHOTO_TYPES][PATH_MAX+1];
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
   if ((DegCod = Deg_GetParamOtherDegCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of degree is missing.");

   /***** Prevent the computing of an average photo too recently updated *****/
   if (Pho_GetTimeAvgPhotoWasComputed (DegCod) >=
       Gbl.TimeStartExecution - Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO)
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
         Pho_ComputeAveragePhoto (DegCod,Sex,&Gbl.Usrs.LstStds,
                                  TypeOfAverage,DirAvgPhotosRelPath[TypeOfAverage],
                                  &NumStds,&NumStdsWithPhoto,&PartialTimeToComputeAvgPhotoInMicroseconds);
         TotalTimeToComputeAvgPhotoInMicroseconds += PartialTimeToComputeAvgPhotoInMicroseconds;
        }

      /***** Store stats in database *****/
      Pho_UpdateDegStats (DegCod,Sex,NumStds,NumStdsWithPhoto,TotalTimeToComputeAvgPhotoInMicroseconds);
     }

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (&Gbl.Usrs.LstStds);

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

   /***** 1. If any degree is not in table, choose it as least recently updated *****/
   /* Get degrees from database */
   sprintf (Query,"SELECT DegCod FROM degrees"
	          " WHERE DegCod NOT IN (SELECT DISTINCT DegCod FROM sta_degrees)"
	          " LIMIT 1");
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

      /***** 2. If all the degrees are in table, choose the least recently updated *****/
      /* Get degrees from database */
      sprintf (Query,"SELECT DegCod FROM sta_degrees"
	             " WHERE UNIX_TIMESTAMP(TimeAvgPhoto)<UNIX_TIMESTAMP()-%ld"
	             " ORDER BY TimeAvgPhoto LIMIT 1",
               Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO);
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
                  " FROM sta_degrees WHERE DegCod='%ld'",DegCod);
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
                  " WHERE DegCod='%ld'",DegCod);
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

static void Pho_ComputeAveragePhoto (long DegCod,Usr_Sex_t Sex,struct ListUsers *LstUsrs,
                                     Pho_AvgPhotoTypeOfAverage_t TypeOfAverage,const char *DirAvgPhotosRelPath,
                                     unsigned *NumStds,unsigned *NumStdsWithPhoto,long *TimeToComputeAvgPhotoInMicroseconds)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   unsigned NumUsr;
   char PathPrivRelPhoto[PATH_MAX+1];	// Relative path to private photo, to calculate average face
   char PathRelAvgPhoto[PATH_MAX+1];
   char FileNamePhotoNames[PATH_MAX+1];
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

   if (LstUsrs->NumUsrs >= Cfg_MIN_PHOTOS_TO_COMPUTE_AVERAGE)
     {
      /***** Build names for text file with photo paths *****/
      sprintf (FileNamePhotoNames,"%s/%s/%s/%ld.txt",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_PHOTO,Cfg_FOLDER_PHOTO_TMP,DegCod);
      if ((FilePhotoNames = fopen (FileNamePhotoNames,"wb")) == NULL)
         Lay_ShowErrorAndExit ("Can not open file to compute average photo.");

      /***** Loop writing file names in text file *****/
      for (NumUsr = 0;
	   NumUsr < LstUsrs->NumUsrs;
	   NumUsr++)
        {
         Gbl.Usrs.Other.UsrDat.Sex = LstUsrs->Lst[NumUsr].Sex;
         if (Sex == Usr_SEX_ALL || Sex == Gbl.Usrs.Other.UsrDat.Sex)
           {
            (*NumStds)++;

            /***** Add photo to file for average face calculation *****/
            Gbl.Usrs.Other.UsrDat.UsrCod = LstUsrs->Lst[NumUsr].UsrCod;
            if (Pho_CheckIfPrivPhotoExists (Gbl.Usrs.Other.UsrDat.UsrCod,PathPrivRelPhoto))
              {
               (*NumStdsWithPhoto)++;
               fprintf (FilePhotoNames,"%s\n",PathPrivRelPhoto);
              }
           }
        }
      fclose (FilePhotoNames);

      /***** Call to program to calculate average photo *****/
      if (*NumStdsWithPhoto >= Cfg_MIN_PHOTOS_TO_COMPUTE_AVERAGE)
        {
         sprintf (StrCallToProgram,"%s %s %s",Pho_StrAvgPhotoPrograms[TypeOfAverage],FileNamePhotoNames,PathRelAvgPhoto);
         ReturnCode = system (StrCallToProgram);
         if (ReturnCode == -1)
            Lay_ShowErrorAndExit ("Error when running program that computes the average photo.");

         /* Write message depending on the return code */
         if (WEXITSTATUS(ReturnCode))
            Lay_ShowErrorAndExit ("The average photo has not been computed successfully.");
        }
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
      *TimeToComputeAvgPhotoInMicroseconds = (tvEndComputingStat.tv_sec - tvStartComputingStat.tv_sec) * 1000000L + tvEndComputingStat.tv_usec - tvStartComputingStat.tv_usec;
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
   /***** Get photo size from form *****/
   Gbl.Stat.DegPhotos.HowComputePhotoSize = Pho_GetHowComputePhotoSizeFromForm ();

   /***** Get how to order degrees from form *****/
   Gbl.Stat.DegPhotos.HowOrderDegrees = Pho_GetHowOrderDegreesFromForm ();

   /***** Get and update type of list, number of columns in class photo and preference about view photos *****/
   Usr_GetAndUpdatePrefsAboutUsrList ();

   if (SeeOrPrint == Pho_DEGREES_SEE)
     {
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\" style=\"margin:0 auto;\">");

      /***** Put a selector for the type of average *****/
      Pho_PutSelectorForTypeOfAvg ();

      /***** Put a selector for the size of photos *****/
      Pho_PutSelectorForHowComputePhotoSize ();

      /***** Put a selector for the order of degrees *****/
      Pho_PutSelectorForHowOrderDegrees ();

      fprintf (Gbl.F.Out,"</table>");

      /***** Form to select type of list used to display degree photos *****/
      Usr_ShowFormsToSelectUsrListType (ActSeePhoDeg);

      /***** Link to print view *****/
      Pho_PutLinkToPrintViewOfDegreeStats ();

      /***** Link to computation of average photos *****/
      Pho_PutLinkToCalculateDegreeStats ();
     }

   /***** Get maximum number of students and maximum number of students with photo in all degrees *****/
   Pho_GetMaxStdsPerDegree ();

   /***** Draw the classphoto/list *****/
   switch (Gbl.Usrs.Me.ListType)
     {
      case Usr_CLASS_PHOTO:
         Pho_ShowOrPrintClassPhotoDegrees (SeeOrPrint);
         break;
      case Usr_LIST:
         Pho_ShowOrPrintListDegrees (SeeOrPrint);
         break;
     }
  }

/*****************************************************************************/
/******************* Put a selector for the type of average ******************/
/*****************************************************************************/

static void Pho_PutSelectorForTypeOfAvg (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Average_type;
   extern const char *Txt_AVERAGE_PHOTO_TYPES[Pho_NUM_AVERAGE_PHOTO_TYPES];
   Pho_AvgPhotoTypeOfAverage_t TypeOfAvg;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle;\">"
	              "%s:"
	              "</td>"
	              "<td style=\"text-align:left; vertical-align:middle;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],Txt_Average_type);
   Act_FormStart (ActSeePhoDeg);
   Pho_PutHiddenParamPhotoSize ();
   Pho_PutHiddenParamOrderDegrees ();
   Usr_PutParamColsClassPhoto ();
   fprintf (Gbl.F.Out,"<select name=\"AvgType\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\">",
            Gbl.FormId);
   for (TypeOfAvg = (Pho_AvgPhotoTypeOfAverage_t) 0;
	TypeOfAvg < Pho_NUM_AVERAGE_PHOTO_TYPES;
	TypeOfAvg++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) TypeOfAvg);
      if (TypeOfAvg == Gbl.Stat.DegPhotos.TypeOfAverage)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s",Txt_AVERAGE_PHOTO_TYPES[TypeOfAvg]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</form>"
	              "</td>"
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
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("AvgType",UnsignedStr,10);
   if (UnsignedStr[0])
     {
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
         Lay_ShowErrorAndExit ("Type of average is missing.");
      if (UnsignedNum >= Pho_NUM_AVERAGE_PHOTO_TYPES)
         Lay_ShowErrorAndExit ("Type of average is missing.");
      return (Pho_AvgPhotoTypeOfAverage_t) UnsignedNum;
     }
   return (Pho_AvgPhotoTypeOfAverage_t) 0;
  }

/*****************************************************************************/
/****************** Put a selector for the size of photos ********************/
/*****************************************************************************/

static void Pho_PutSelectorForHowComputePhotoSize (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Size_of_photos;
   extern const char *Txt_STAT_DEGREE_PHOTO_SIZE[Pho_NUM_HOW_COMPUTE_PHOTO_SIZES];
   Pho_HowComputePhotoSize_t PhoSi;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle;\">"
	              "%s:"
	              "</td>"
	              "<td style=\"text-align:left; vertical-align:middle;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],Txt_Size_of_photos);
   Act_FormStart (ActSeePhoDeg);
   Pho_PutHiddenParamTypeOfAvg ();
   Pho_PutHiddenParamOrderDegrees ();
   Usr_PutParamColsClassPhoto ();
   fprintf (Gbl.F.Out,"<select name=\"PhotoSize\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\">",
            Gbl.FormId);
   for (PhoSi = (Pho_HowComputePhotoSize_t) 0;
	PhoSi < Pho_NUM_HOW_COMPUTE_PHOTO_SIZES;
	PhoSi++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) PhoSi);
      if (PhoSi == Gbl.Stat.DegPhotos.HowComputePhotoSize)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s",Txt_STAT_DEGREE_PHOTO_SIZE[PhoSi]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</form>"
	              "</td>"
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
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("PhotoSize",UnsignedStr,10);
   if (UnsignedStr[0])
     {
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
         Lay_ShowErrorAndExit ("Size of photo is missing.");
      if (UnsignedNum >= Pho_NUM_HOW_COMPUTE_PHOTO_SIZES)
         Lay_ShowErrorAndExit ("Size of photo is missing.");
      return (Pho_HowComputePhotoSize_t) UnsignedNum;
     }
   return (Pho_HowComputePhotoSize_t) 0;
  }

/*****************************************************************************/
/****************** Put a selector for the order of degrees ******************/
/*****************************************************************************/

static void Pho_PutSelectorForHowOrderDegrees (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Sort_degrees_by;
   extern const char *Txt_STAT_DEGREE_PHOTO_ORDER[Pho_NUM_HOW_ORDER_DEGREES];
   Pho_HowOrderDegrees_t Order;

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s\" style=\"text-align:right;"
	              " vertical-align:middle;\">"
	              "%s:"
	              "</td>"
	              "<td style=\"text-align:left; vertical-align:middle;\">",
	    The_ClassFormul[Gbl.Prefs.Theme],Txt_Sort_degrees_by);
   Act_FormStart (ActSeePhoDeg);
   Pho_PutHiddenParamTypeOfAvg ();
   Pho_PutHiddenParamPhotoSize ();
   Usr_PutParamColsClassPhoto ();
   fprintf (Gbl.F.Out,"<select name=\"OrdDeg\""
                      " onchange=\"javascript:document.getElementById('%s').submit();\">",
            Gbl.FormId);
   for (Order = (Pho_HowOrderDegrees_t) 0;
	Order < Pho_NUM_HOW_ORDER_DEGREES;
	Order++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%u\"",(unsigned) Order);
      if (Order == Gbl.Stat.DegPhotos.HowOrderDegrees)
         fprintf (Gbl.F.Out," selected=\"selected\"");
      fprintf (Gbl.F.Out,">%s",Txt_STAT_DEGREE_PHOTO_ORDER[Order]);
     }
   fprintf (Gbl.F.Out,"</select>"
	              "</form>"
	              "</td>"
	              "</tr>");
  }

/*****************************************************************************/
/**************** Put hidden parameter for the order of degrees **************/
/*****************************************************************************/

void Pho_PutHiddenParamOrderDegrees (void)
  {
   Par_PutHiddenParamUnsigned ("OrdDeg",(unsigned) Gbl.Stat.DegPhotos.HowOrderDegrees);
  }

/*****************************************************************************/
/*************************** Get how to order degrees ************************/
/*****************************************************************************/

static Pho_HowOrderDegrees_t Pho_GetHowOrderDegreesFromForm (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("OrdDeg",UnsignedStr,10);
   if (UnsignedStr[0])
     {
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
         Lay_ShowErrorAndExit ("Sort order is missing.");
      if (UnsignedNum >= Pho_NUM_HOW_ORDER_DEGREES)
         Lay_ShowErrorAndExit ("Sort order is missing.");
      return (Pho_HowOrderDegrees_t) UnsignedNum;
     }
   return (Pho_HowOrderDegrees_t) 0;
  }

/*****************************************************************************/
/*************** Put a link to print view the stats of degrees ***************/
/*****************************************************************************/

static void Pho_PutLinkToPrintViewOfDegreeStats (void)
  {
   fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");
   Lay_PutLinkToPrintView1 (ActPrnPhoDeg);
   Pho_PutHiddenParamTypeOfAvg ();
   Pho_PutHiddenParamPhotoSize ();
   Pho_PutHiddenParamOrderDegrees ();
   Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
   Usr_PutParamColsClassPhoto ();
   Usr_PutParamListWithPhotos ();
   Lay_PutLinkToPrintView2 ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*************** Put a link to calculate the stats of degrees ****************/
/*****************************************************************************/

static void Pho_PutLinkToCalculateDegreeStats (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Calculate_average_photo_of_a_degree;
   extern const char *Txt_Calculate_average_photo_of_THE_DEGREE_X;
   extern const char *Txt_unknown_TIME;
   extern const char *Txt_time;
   unsigned NumDeg;
   struct Degree Deg;
   long EstimatedTimeToComputeAvgPhotoInMicroseconds;
   char StrEstimatedTimeToComputeAvgPhoto[64];

   if ((Deg.DegCod = Pho_GetDegWithAvgPhotoLeastRecentlyUpdated ()) > 0)
     {
      /***** Get list of all the degrees *****/
      Deg_GetListAllDegs ();

      /***** Get data of the degree from database *****/
      Deg_GetDataOfDegreeByCod (&Deg);

      /***** Start div and table *****/
      fprintf (Gbl.F.Out,"<div style=\"text-align:center;\">");

      /***** Start form *****/
      Act_FormStart (ActCalPhoDeg);
      Pho_PutHiddenParamTypeOfAvg ();
      Pho_PutHiddenParamPhotoSize ();
      Pho_PutHiddenParamOrderDegrees ();
      Usr_PutParamUsrListType (Gbl.Usrs.Me.ListType);
      Usr_PutParamColsClassPhoto ();
      Usr_PutParamListWithPhotos ();
      Act_LinkFormSubmit (Txt_Calculate_average_photo_of_a_degree,The_ClassFormul[Gbl.Prefs.Theme]);
      Lay_PutSendIcon ("recycle",Txt_Calculate_average_photo_of_a_degree,Txt_Calculate_average_photo_of_THE_DEGREE_X);

      /***** Put selector with all the degrees *****/
      fprintf (Gbl.F.Out,"<select name=\"OthDegCod\">");
      for (NumDeg = 0;
	   NumDeg < Gbl.Degs.AllDegs.Num;
	   NumDeg++)
        {
         /* Get time to compute average photo of this degree */
         EstimatedTimeToComputeAvgPhotoInMicroseconds = Pho_GetTimeToComputeAvgPhoto (Gbl.Degs.AllDegs.Lst[NumDeg].DegCod);
         if (EstimatedTimeToComputeAvgPhotoInMicroseconds == -1L)
            strcpy (StrEstimatedTimeToComputeAvgPhoto,Txt_unknown_TIME);
         else
            Sta_WriteTime (StrEstimatedTimeToComputeAvgPhoto,EstimatedTimeToComputeAvgPhotoInMicroseconds);

         fprintf (Gbl.F.Out,"<option value=\"%ld\"%s>%s (%s: %s)</option>",
                  Gbl.Degs.AllDegs.Lst[NumDeg].DegCod,
                  Gbl.Degs.AllDegs.Lst[NumDeg].DegCod == Deg.DegCod ? " selected=\"selected\"" :
                                                                      ((Pho_GetTimeAvgPhotoWasComputed (Gbl.Degs.AllDegs.Lst[NumDeg].DegCod) >=
                                                                	Gbl.TimeStartExecution - Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO) ? " disabled=\"disabled\"" :
                                                                	                                                                ""),
                  Gbl.Degs.AllDegs.Lst[NumDeg].ShortName,
                  Txt_time,
                  StrEstimatedTimeToComputeAvgPhoto);
        }

      /***** End selector, form, table and div *****/
      fprintf (Gbl.F.Out,"</select>"
	                 "</form>"
	                 "</div>");

      /***** Free list of all the degrees *****/
      Deg_FreeListAllDegs ();
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
   sprintf (Query,"SELECT MAX(NumStds),MAX(NumStdsWithPhoto),MAX(NumStdsWithPhoto/NumStds)"
                  " FROM sta_degrees WHERE Sex='all' AND NumStds>'0'");
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
   extern const char *Txt_Degrees;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   struct Degree Deg;
   unsigned NumDegsNotEmpty;
   int NumStds,NumStdsWithPhoto;
   struct Date DateAvgPhoto;
   bool TRIsOpen = false;

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,0,Txt_Degrees);

   /***** Get degrees from database *****/
   Pho_BuildQueryOfDegrees (Query);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get degrees");

   /***** Get degrees *****/
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
         /* Get year, month and day (row[1] holds the date in YYYYMMDD format) */
         if (!(Dat_GetDateFromYYYYMMDD (&DateAvgPhoto,row[1])))
            Lay_ShowErrorAndExit ("Wrong date.");

         if ((NumDegsNotEmpty % Gbl.Usrs.ClassPhoto.Cols) == 0)
           {
            fprintf (Gbl.F.Out,"<tr>");
            TRIsOpen = true;
           }

         /***** Show average photo of students belonging to this degree *****/
         fprintf (Gbl.F.Out,"<td class=\"CLASSPHOTO\""
                            " style=\"text-align:center;\">");
         Pho_ShowDegreeAvgPhotoAndStat (&Deg,SeeOrPrint,Usr_SEX_ALL,NumStds,NumStdsWithPhoto,&DateAvgPhoto);
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

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** End frame *****/
   Lay_EndRoundFrameTable10 ();
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
   unsigned long NumRow,NumRows;
   unsigned NumDegsNotEmpty;
   int NumStds,NumStdsWithPhoto;
   struct Date DateAvgPhoto;
   struct Degree Deg;
   Usr_Sex_t Sex;

   /***** Class photo start *****/
   if (SeeOrPrint == Pho_DEGREES_SEE)
      Lay_StartRoundFrameTable10 (NULL,0,NULL);
   else
      fprintf (Gbl.F.Out,"<table>");

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"TIT_TBL\" style=\"text-align:right;"
                      " vertical-align:top;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:center;"
                      " vertical-align:top;\">"
                      "%s&nbsp;"
                      "</td>",
            Txt_No_INDEX,
            Txt_Degree);
   for (Sex = (Usr_Sex_t) 0;
	Sex < Usr_NUM_SEXS;
	Sex++)
      fprintf (Gbl.F.Out,"<td class=\"TIT_TBL\" style=\"text-align:center; vertical-align:top;\">"
	                 "%s&nbsp;"
	                 "</td>",
               Txt_SEX_PLURAL_Abc[Sex]);
   fprintf (Gbl.F.Out,"</tr>");

   /***** Get degrees from database *****/
   Pho_BuildQueryOfDegrees (Query);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get degrees");

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

      /* Get year, month and day (row[1] holds the date in YYYYMMDD format) */
      if (!(Dat_GetDateFromYYYYMMDD (&DateAvgPhoto,row[1])))
         Lay_ShowErrorAndExit ("Wrong date.");

      /* Get data of degree */
      Deg_GetDataOfDegreeByCod (&Deg);

      /***** Show logo and name of this degree *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT\" style=\"text-align:right;"
                         " background-color:%s;\">"
                         "%u&nbsp;"
                         "</td>",
               Gbl.ColorRows[Gbl.RowEvenOdd],++NumDegsNotEmpty);

      /***** Show logo and name of this degree *****/
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:left;"
                         " background-color:%s;\">",
               Gbl.ColorRows[Gbl.RowEvenOdd]);
      if (SeeOrPrint == Pho_DEGREES_SEE)
         fprintf (Gbl.F.Out,"<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">",
                  Deg.WWW,Deg.FullName);
      Log_DrawLogo (Sco_SCOPE_DEGREE,Deg.DegCod,Deg.ShortName,
                    16,"vertical-align:top;",true);
      fprintf (Gbl.F.Out,"&nbsp;%s&nbsp;",
               Deg.ShortName);
      if (SeeOrPrint == Pho_DEGREES_SEE)
         fprintf (Gbl.F.Out,"</a>");
      fprintf (Gbl.F.Out,"</td>");

      for (Sex = (Usr_Sex_t) 0;
	   Sex < Usr_NUM_SEXS;
	   Sex++)
        {
         /***** Show average photo of students belonging to this degree *****/
         Pho_GetNumStdsInDegree (Deg.DegCod,Sex,&NumStds,&NumStdsWithPhoto);
         fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
                            " background-color:%s;\">",
                  Gbl.ColorRows[Gbl.RowEvenOdd]);
         if (Gbl.Usrs.Listing.WithPhotos)
            Pho_ShowDegreeAvgPhotoAndStat (&Deg,SeeOrPrint,Sex,NumStds,NumStdsWithPhoto,&DateAvgPhoto);
         else
            Pho_ShowDegreeStat (NumStds,NumStdsWithPhoto);
         fprintf (Gbl.F.Out,"</td>");
        }
      fprintf (Gbl.F.Out,"</tr>");
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Photos end *****/
   if (SeeOrPrint == Pho_DEGREES_SEE)
      Lay_EndRoundFrameTable10 ();
   else
      fprintf (Gbl.F.Out,"</table>");
  }

/*****************************************************************************/
/****** Build a query to get the degrees ordered by different criteria *******/
/*****************************************************************************/

static void Pho_BuildQueryOfDegrees (char *Query)
  {
   switch (Gbl.Stat.DegPhotos.HowOrderDegrees)
     {
      case Pho_NUMBER_OF_STUDENTS:
         sprintf (Query,"SELECT degrees.DegCod,DATE_FORMAT(sta_degrees.TimeAvgPhoto,'%%Y%%m%%d')"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all' AND sta_degrees.NumStds>'0' AND degrees.DegCod=sta_degrees.DegCod"
                        " ORDER BY sta_degrees.NumStds DESC,sta_degrees.NumStdsWithPhoto DESC,degrees.ShortName");
         break;
      case Pho_NUMBER_OF_PHOTOS:
         sprintf (Query,"SELECT degrees.DegCod,DATE_FORMAT(sta_degrees.TimeAvgPhoto,'%%Y%%m%%d')"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all' AND sta_degrees.NumStds>'0' AND degrees.DegCod=sta_degrees.DegCod"
                        " ORDER BY sta_degrees.NumStdsWithPhoto DESC,sta_degrees.NumStds DESC,degrees.ShortName");
         break;
      case Pho_PERCENT:
         sprintf (Query,"SELECT degrees.DegCod,DATE_FORMAT(sta_degrees.TimeAvgPhoto,'%%Y%%m%%d')"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all' AND sta_degrees.NumStds>'0' AND degrees.DegCod=sta_degrees.DegCod"
                        " ORDER BY sta_degrees.NumStdsWithPhoto/sta_degrees.NumStds DESC,degrees.ShortName");
         break;
      case Pho_DEGREE_NAME:
         sprintf (Query,"SELECT degrees.DegCod,DATE_FORMAT(sta_degrees.TimeAvgPhoto,'%%Y%%m%%d')"
                        " FROM degrees,sta_degrees"
                        " WHERE sta_degrees.Sex='all' AND sta_degrees.NumStds>'0' AND degrees.DegCod=sta_degrees.DegCod"
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
                  " WHERE DegCod='%ld' AND Sex='%s'",
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
                  " VALUES ('%ld','%s','%u','%u',NOW(),'%ld')",
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

static void Pho_ShowDegreeAvgPhotoAndStat (struct Degree *Deg,Pho_AvgPhotoSeeOrPrint_t SeeOrPrint,Usr_Sex_t Sex,int NumStds,int NumStdsWithPhoto,struct Date *DateAvgPhoto)
  {
   extern const char *Usr_StringsSexDB[Usr_NUM_SEXS];
   extern const char *Txt_students_ABBREVIATION;
   extern const char *Txt_SEX_PLURAL_abc[Usr_NUM_SEXS];
   extern const char *Txt_photos;
   unsigned PhotoWidth,PhotoHeight;
   char PathRelAvgPhoto[PATH_MAX+1];
   char PhotoURL[PATH_MAX+1];
   char CopyOfDegShortName[Deg_MAX_LENGTH_DEGREE_SHORT_NAME+1];	// Short name of degree
   char PhotoCaption[512];

   /***** Compute photo width and height to be proportional to number of students *****/
   Pho_ComputePhotoSize (NumStds,NumStdsWithPhoto,&PhotoWidth,&PhotoHeight);

   /***** Make a copy of the degree short name *****/
   strncpy (CopyOfDegShortName,Deg->ShortName,Deg_MAX_LENGTH_DEGREE_SHORT_NAME);
   CopyOfDegShortName[Deg_MAX_LENGTH_DEGREE_SHORT_NAME] = '\0';
   Str_LimitLengthHTMLStr (CopyOfDegShortName,
                           SeeOrPrint == Pho_DEGREES_SEE ? 10 :
                        	                           15);

   if (SeeOrPrint == Pho_DEGREES_SEE)
      fprintf (Gbl.F.Out,"<a href=\"%s\" title=\"%s\" class=\"CLASSPHOTO\" target=\"_blank\">",
               Deg->WWW,Deg->FullName);
   fprintf (Gbl.F.Out,"<img src=\"");

   if (NumStds && NumStdsWithPhoto)
     {
      sprintf (PathRelAvgPhoto,"%s/%s/%s/%ld_%s.jpg",
               Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,
               Pho_StrAvgPhotoDirs[Gbl.Stat.DegPhotos.TypeOfAverage],
               Deg->DegCod,Usr_StringsSexDB[Sex]);

      if (Fil_CheckIfPathExists (PathRelAvgPhoto))
        {
         sprintf (PhotoURL,"%s/%s/%s/%ld_%s.jpg",
                  Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_PHOTO,
                  Pho_StrAvgPhotoDirs[Gbl.Stat.DegPhotos.TypeOfAverage],
                  Deg->DegCod,Usr_StringsSexDB[Sex]);
         fprintf (Gbl.F.Out,"%s\" style=\"width:%upx; height:%upx;\"",
                  PhotoURL,PhotoWidth,PhotoHeight);
         if (SeeOrPrint == Pho_DEGREES_SEE)
           {
            sprintf (PhotoCaption,"%s<br />"
        	                  "%d&nbsp;%s&nbsp;(%s)<br />"
        	                  "%d&nbsp;%s&nbsp;(%d%%)<br />"
        	                  "%02u/%02u/%04u",
                     Deg->ShortName,
                     NumStds,Txt_students_ABBREVIATION,Txt_SEX_PLURAL_abc[Sex],
                     NumStdsWithPhoto,Txt_photos,
                     NumStds > 0 ? (int) (((NumStdsWithPhoto * 100.0) / NumStds) + 0.5) :
                	           0,
                     DateAvgPhoto->Day,DateAvgPhoto->Month,DateAvgPhoto->Year);
            fprintf (Gbl.F.Out," onmouseover=\"zoom(this,'%s','%s')\" onmouseout=\"noZoom(this);\"",
                     PhotoURL,PhotoCaption);
           }
        }
      else
         fprintf (Gbl.F.Out,"%s/usr_bl.jpg\""
                            " style=\"width:%upx; height:%upx;\"",
                  Gbl.Prefs.IconsURL,PhotoWidth,PhotoHeight);
     }
   else
      fprintf (Gbl.F.Out,"%s/usr_bl.jpg\""
	                 " style=\"width:%upx; height:%upx;\"",
	       Gbl.Prefs.IconsURL,PhotoWidth,PhotoHeight);
   fprintf (Gbl.F.Out," alt=\"%s\" />",Deg->FullName);
   if (SeeOrPrint == Pho_DEGREES_PRINT)
      fprintf (Gbl.F.Out,"<span class=\"CLASSPHOTO\">");
   fprintf (Gbl.F.Out,"<br />%s<br />%d&nbsp;%s<br />%d&nbsp;%s<br />(%d%%)",
            CopyOfDegShortName,
            NumStds,Txt_students_ABBREVIATION,
            NumStdsWithPhoto,Txt_photos,
            NumStds > 0 ? (int) (((NumStdsWithPhoto * 100.0) / NumStds) + 0.5) :
        	          0);
   if (SeeOrPrint == Pho_DEGREES_SEE)
      fprintf (Gbl.F.Out,"</a>");
   else
      fprintf (Gbl.F.Out,"</span>");
  }

/*****************************************************************************/
/******** Compute photo width and height for class photo of degrees **********/
/*****************************************************************************/
#define MIN_WIDTH_PHOTO 9
#define MIN_HEIGHT_PHOTO 12
#define MIN_PIXELS_PHOTO (MIN_WIDTH_PHOTO * MIN_HEIGHT_PHOTO)
#define DEF_WIDTH_PHOTO 36
#define DEF_HEIGHT_PHOTO 48
#define DEF_PIXELS_PHOTO (DEF_WIDTH_PHOTO * DEF_HEIGHT_PHOTO)
#define MAX_WIDTH_PHOTO 72
#define MAX_HEIGHT_PHOTO 96
#define MAX_PIXELS_PHOTO (MAX_WIDTH_PHOTO * MAX_HEIGHT_PHOTO)

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
