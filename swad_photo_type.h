// swad_photo.h: Definition of types for users' photos management

#ifndef _SWAD_PHO_TYP
#define _SWAD_PHO_TYP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
/************************* Public types and constants ************************/
/*****************************************************************************/

#define Pho_PHOTO_REAL_WIDTH		186
#define Pho_PHOTO_REAL_HEIGHT		248

/***** Zoom? *****/
typedef enum
  {
   Pho_ZOOM,
   Pho_NO_ZOOM,
  } Pho_Zoom_t;

/***** Show photos when listing students? *****/
#define Pho_NUM_PHOTOS 3
typedef enum
  {
   Pho_PHOTOS_UNKNOWN,
   Pho_PHOTOS_DONT_SHOW,
   Pho_PHOTOS_SHOW,
  } Pho_ShowPhotos_t;
#define Pho_PHOTOS_DEFAULT Pho_PHOTOS_SHOW

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
#define Pho_PHOTO_AVG_DEFAULT Pho_PHOTO_MEDIAN_ALL

#define Pho_NUM_HOW_COMPUTE_PHOTO_SIZES 4
typedef enum
  {
   Pho_PROPORTIONAL_TO_NUMBER_OF_STUDENTS = 0,
   Pho_PROPORTIONAL_TO_NUMBER_OF_PHOTOS   = 1,
   Pho_PROPORTIONAL_TO_PERCENT            = 2,
   Pho_UNIQUE_SIZE                        = 3,
  } Pho_HowComputePhotoSize_t;
#define Pho_PHOTO_SIZE_DEFAULT Pho_PROPORTIONAL_TO_NUMBER_OF_STUDENTS

#define Pho_NUM_HOW_ORDER_DEGREES 4
typedef enum
  {
   Pho_NUMBER_OF_STUDENTS = 0,
   Pho_NUMBER_OF_PHOTOS   = 1,
   Pho_PERCENT            = 2,
   Pho_DEGREE_NAME        = 3,
  } Pho_HowOrderDegrees_t;
#define Pho_ORDER_DEFAULT Pho_NUMBER_OF_STUDENTS

#define Pho_MAX_CLICKS_WITHOUT_PHOTO	 50

struct Pho_DegPhotos
  {
   Pho_AvgPhotoTypeOfAverage_t TypeOfAverage;
   Pho_HowComputePhotoSize_t HowComputePhotoSize;
   Pho_HowOrderDegrees_t HowOrderDegrees;
   int MaxStds;
   int MaxStdsWithPhoto;
   double MaxPercent;
  };

#endif
