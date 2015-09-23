// swad_photo.h: Users' photos management

#ifndef _SWAD_PHO
#define _SWAD_PHO
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

/*****************************************************************************/
/************************* Public types and constants ************************/
/*****************************************************************************/

#define Pho_PHOTO_REAL_WIDTH		225
#define Pho_PHOTO_REAL_HEIGHT		300

/***** Zoom? *****/
typedef enum
  {
   Pho_ZOOM,
   Pho_NO_ZOOM,
  } Pho_Zoom_t;

/***** Average photos of students in degrees ******/

typedef enum
  {
   Pho_DEGREES_SEE,
   Pho_DEGREES_PRINT,
  } Pho_AvgPhotoSeeOrPrint_t;

#define Pho_NUM_AVERAGE_PHOTO_TYPES 2
typedef enum
  {
   Pho_PHOTO_MEDIAN_ALL  = 0,
   Pho_PHOTO_AVERAGE_ALL = 1,
  } Pho_AvgPhotoTypeOfAverage_t;

#define Pho_NUM_HOW_COMPUTE_PHOTO_SIZES 4
typedef enum
  {
   Pho_PROPORTIONAL_TO_NUMBER_OF_STUDENTS = 0,
   Pho_PROPORTIONAL_TO_NUMBER_OF_PHOTOS   = 1,
   Pho_PROPORTIONAL_TO_PERCENT            = 2,
   Pho_UNIQUE_SIZE                        = 3,
  } Pho_HowComputePhotoSize_t;

#define Pho_NUM_HOW_ORDER_DEGREES 4
typedef enum
  {
   Pho_NUMBER_OF_STUDENTS = 0,
   Pho_NUMBER_OF_PHOTOS   = 1,
   Pho_PERCENT            = 2,
   Pho_DEGREE_NAME        = 3,
  } Pho_HowOrderDegrees_t;

#define Pho_MAX_CLICKS_WITHOUT_PHOTO	 50

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

bool Pho_CheckIfICanChangeOtherUsrPhoto (const struct UsrData *UsrDat);
void Pho_PutLinkToChangeMyPhoto (void);
void Pho_PutLinkToChangeOtherUsrPhoto (void);
void Pho_ReqMyPhoto (void);
void Pho_ReqUsrPhoto (struct UsrData *UsrDat);
void Pho_ReqPhoto (const struct UsrData *UsrDat,bool PhotoExists,const char *PhotoURL);
void Pho_SendPhotoUsr (void);
void Pho_RecMyPhotoDetFaces (void);
void Pho_RecUsrPhotoDetFaces (void);
void Pho_RemoveMyPhoto1 (void);
void Pho_RemoveMyPhoto2 (void);
void Pho_RemoveUsrPhoto (void);
void Pho_ReceivePhotoAndDetectFaces (bool ItsMe,const struct UsrData *UsrDat);
void Pho_UpdateMyPhoto1 (void);
void Pho_UpdateMyPhoto2 (void);
void Pho_UpdateUsrPhoto1 (void);
void Pho_UpdateUsrPhoto2 (void);

unsigned Pho_UpdateMyClicksWithoutPhoto (void);
void Pho_RemoveUsrFromTableClicksWithoutPhoto (long UsrCod);

bool Pho_ShowUsrPhotoIsAllowed (const struct UsrData *UsrDat,char *PhotoURL);
bool Pho_BuildLinkToPhoto (const struct UsrData *UsrDat,char *PhotoURL,bool HTTPS);
bool Pho_CheckIfPrivPhotoExists (long UsrCod,char *PathPrivRelPhoto);
bool Pho_RemovePhoto (struct UsrData *UsrDat);
void Pho_UpdatePhotoName (struct UsrData *UsrDat);
void Pho_ShowUsrPhoto (const struct UsrData *UsrDat,const char *PhotoURL,
                       const char *ClassPhoto,Pho_Zoom_t Zoom);

void Pho_ChangePhotoVisibility (void);

void Pho_CalcPhotoDegree (void);
void Pho_RemoveObsoleteStatDegrees (void);
void Pho_ShowPhotoDegree (void);
void Pho_PrintPhotoDegree (void);
void Pho_ShowOrPrintPhotoDegree (Pho_AvgPhotoSeeOrPrint_t SeeOrPrint);
void Pho_PutHiddenParamTypeOfAvg (void);
void Pho_PutHiddenParamPhotoSize (void);
void Pho_PutHiddenParamOrderDegrees (void);

#endif
