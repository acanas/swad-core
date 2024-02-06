// swad_media.h: processing of image/video uploaded in a form

#ifndef _SWAD_MED
#define _SWAD_MED
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_cryptography.h"
#include "swad_string.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Med_BYTES_NAME	Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64

#define Med_MAX_CHARS_TITLE	(128 - 1)	// 127
#define Med_MAX_BYTES_TITLE	((Med_MAX_CHARS_TITLE + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Med_MAX_CHARS_ATTRIBUTION	(256 - 1)	// 255
#define Med_MAX_BYTES_ATTRIBUTION	((Med_MAX_CHARS_ATTRIBUTION + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 4095

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/***** Action to perform when editing a form with an image/video *****/
#define Med_NUM_ACTIONS	3
typedef enum
  {
   Med_ACTION_NO_MEDIA,		// Do not use media (remove current media if exists)
   Med_ACTION_KEEP_MEDIA,	// Keep current media unchanged
   Med_ACTION_NEW_MEDIA,	// Upload new media (if current media exists, remove and change by the new one)
  } Med_Action_t;
#define Med_ACTION_DEFAULT Med_ACTION_NO_MEDIA

/***** Status of an image/video file *****/
/*
No file   Original file               Temporary          Definitive         Name of the image/video
uploaded  uploaded by user            processed file     processed file     stored in database
--------- --------------------------- ------------------ ------------------ ---------------------
Med_STATUS_NONE                       Med_PROCESSED      Med_MOVED          Med_NAME_STORED_IN_DB
--------- --------------------------- ------------------ ------------------ ---------------------
     upload-file               process file         move file      insert in database
--------- --------------------------- ------------------ ------------------ ---------------------
file.ext        /                           /                  /               xx-unique-name
                |                           |                  |
               var                         var                var
                |                           |                  |
               www                         www                www
                |                           |                  |
               swad                        swad               swad
                |                           |                  |
               med                         med                med
                |                           |                  |
               tmp                         tmp                xx (2 first chars)
                |                           |                  |
          xx-unique-name_original.ext xx-unique-name.jpg xx-unique-name.jpg

xx-unique-name: a unique name encrypted starting by two random chars xx
*/
typedef enum
  {
   Med_STATUS_NONE,
   Med_PROCESSED,
   Med_MOVED,
   Med_STORED_IN_DB,
  } Med_Status_t;

#define Med_NUM_TYPES 8
typedef enum
  {
   Med_TYPE_NONE,
   Med_JPG,
   Med_GIF,
   Med_MP4,
   Med_WEBM,
   Med_OGG,
   Med_YOUTUBE,
   Med_EMBED,
  } Med_Type_t;

/***** Struct used to get images/videos from forms *****/
struct Med_Media
  {
   long MedCod;
   Med_Action_t Action;
   Med_Status_t Status;
   char Name[Med_BYTES_NAME + 1];
   Med_Type_t Type;
   bool URLIsAllocated;
   bool TitleIsAllocated;
   char *URL;	// URL, i.e. link to original big photo or video
		// (it must be initialized to NULL
		// in order to not trying to free it when no memory allocated)
   char *Title;	// Title/attribution (it must be initialized to NULL
		// in order to not trying to free it when no memory allocated)
   unsigned Width;
   unsigned Height;
   unsigned Quality;
  };

/***** Parameters used in a form to upload an image/video *****/
#define Med_MAX_BYTES_PARAM_UPLOAD_MEDIA (32 - 1)
struct Med_ParUpload
  {
   char Action  [Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
   char FormType[Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
   char File    [Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
   char Title   [Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
   char URL     [Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Med_MediaConstructor (struct Med_Media *Media);
void Med_MediaDestructor (struct Med_Media *Media);
void Med_ResetMedia (struct Med_Media *Media);

void Med_GetMediaDataByCod (struct Med_Media *Media);

void Med_PutMediaUploader (int NumMedia,const char *ClassInput);
void Med_GetMediaFromForm (long CrsCod,long QstCod,int NumMedia,struct Med_Media *Media,
                           void (*GetMediaFromDB) (long CrsCod,long QstCod,int NumMedia,struct Med_Media *Media),
			   const char *SectionForAlerts);
void Med_SetParsNames (struct Med_ParUpload *ParUpl,int NumMedia);

void Med_RemoveKeepOrStoreMedia (long CurrentMedCodInDB,struct Med_Media *Media);
void Med_MoveMediaToDefinitiveDir (struct Med_Media *Media);
void Med_StoreMediaInDB (struct Med_Media *Media);

void Med_ShowMedia (const struct Med_Media *Media,
                    const char *ClassContainer,const char *ClassMedia);

long Med_CloneMedia (const struct Med_Media *MediaSrc);

void Med_RemoveMediaFromAllRows (unsigned NumMedia,MYSQL_RES *mysql_res);
void Med_RemoveMedia (long MedCod);

#endif
