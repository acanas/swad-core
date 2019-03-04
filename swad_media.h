// swad_media.h: processing of image/video uploaded in a form

#ifndef _SWAD_IMG
#define _SWAD_IMG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#define Med_NUM_ACTIONS	4
typedef enum
  {
   Med_ACTION_NEW_MEDIA,	// Upload new image/video
   Med_ACTION_KEEP_MEDIA,	// Keep current image/video unchanged
   Med_ACTION_CHANGE_MEDIA,	// Change existing image/video by a new one
   Med_ACTION_NO_MEDIA,		// Do not use image/video (remove current image/video if exists)
  } Med_Action_t;
#define Med_ACTION_DEFAULT Med_ACTION_NO_MEDIA

/***** Status of an image/video file *****/
/*
No file   Original file               Temporary          Definitive         Name of the image/video
uploaded  uploaded by user            processed file     processed file     stored in database
--------- --------------------------- ------------------ ------------------ ---------------------
Med_NONE  Med_FILE_RECEIVED           Med_FILE_PROCESSED Med_FILE_MOVED     Med_NAME_STORED_IN_DB
--------- --------------------------- ------------------ ------------------ ---------------------
-> upload-file ->          -> process-file ->  b -> move-file ->  -> insert-name ->
--------- --------------------------- ------------------ ------------------ ---------------------
file.ext   /                           /                  /                 xx-unique-name
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
   Med_FILE_NONE,
   Med_FILE_RECEIVED,
   Med_FILE_PROCESSED,
   Med_FILE_MOVED,
   Med_NAME_STORED_IN_DB,
  } Med_FileStatus_t;

#define Med_NUM_TYPES 3
typedef enum
  {
   Med_NONE,
   Med_JPG,
   Med_GIF,
  } Med_Type_t;

/***** Struct used to get images/videos from forms *****/
struct Media
  {
   Med_Action_t Action;
   Med_FileStatus_t Status;
   char Name[Med_BYTES_NAME + 1];
   Med_Type_t Type;
   char *Title;	// Title/attribution (it must be initialized to NULL
		// in order to not trying to free it when no memory allocated)
   char *URL;	// URL, i.e. link to original big photo or video
		// (it must be initialized to NULL
		// in order to not trying to free it when no memory allocated)
   unsigned Width;
   unsigned Height;
   unsigned Quality;
  };

/***** Parameters used in a form to upload an image/video *****/
#define Med_MAX_BYTES_PARAM_UPLOAD_MEDIA (16 - 1)
struct ParamUploadMedia
  {
   char Action[Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
   char File  [Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
   char Title [Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
   char URL   [Med_MAX_BYTES_PARAM_UPLOAD_MEDIA + 1];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Med_MediaConstructor (struct Media *Media);
void Med_ResetMediaExceptTitleAndURL (struct Media *Media);
void Med_MediaDestructor (struct Media *Media);
void Med_FreeMediaTitle (struct Media *Media);
void Med_FreeMediaURL (struct Media *Media);

void Med_GetMediaDataFromRow (const char *Name,
			      const char *TypeStr,
                              const char *Title,
                              const char *URL,
                              struct Media *Media);

void Med_PutMediaUploader (int NumMediaInForm,const char *ClassMediaTitURL);
void Med_GetMediaFromForm (int NumMediaInForm,struct Media *Media,
                           void (*GetMediaFromDB) (int NumMediaInForm,struct Media *Media));
void Med_SetParamNames (struct ParamUploadMedia *ParamUploadMedia,int NumMediaInForm);

void Med_MoveMediaToDefinitiveDir (struct Media *Media);
void Med_ShowMedia (struct Media *Media,
                    const char *ClassContainer,const char *ClassMedia);

void Med_RemoveMediaFilesFromAllRows (unsigned NumMedia,MYSQL_RES *mysql_res);
void Med_RemoveMediaFilesFromRow (MYSQL_RES *mysql_res);
void Med_RemoveMediaFiles (const char *Name,Med_Type_t Type);

Med_Type_t Med_GetTypeFromStrInDB (const char *Str);
const char *Med_GetStringTypeForDB (Med_Type_t Type);

#endif
