// swad_image.h: processing of image uploaded in a form

#ifndef _SWAD_IMG
#define _SWAD_IMG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/
/*
No image  Original file               Temporary          Definitive         Name of the image
uploaded  uploaded by user            processed image    processed image    stored in database
--------- --------------------------- ------------------ ------------------ ---------------------
Img_NONE  Img_NONE                    Img_FILE_PROCESSED Img_FILE_MOVED     Img_NAME_STORED_IN_DB
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
          img                         img                img
           |                           |                  |
          tmp                         tmp                xx (2 first chars)
           |                           |                  |
          xx-unique-name_original.ext xx-unique-name.jpg xx-unique-name.jpg

xx-unique-name: a unique name encrypted starting by two random chars xx
*/
typedef enum
  {
   Img_NONE,
   Img_FILE_PROCESSED,
   Img_FILE_MOVED,
   Img_NAME_STORED_IN_DB,
  } Img_Status_t;

/***** Action to perform when editing a form with an image *****/
#define Img_NUM_ACTIONS	3
typedef enum
  {
   Img_ACTION_NONE,	// Do not use image (remove current image if exists)
   Img_ACTION_KEEP,	// Keep current image unchanged
   Img_ACTION_CHANGE,	// Upload new image (remove current image if exists)
  } Img_Action_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

Img_Action_t Img_GetImageActionFromForm (void);
void Img_GetImageFromForm (unsigned Width,unsigned Height,unsigned Quality);

void Img_MoveImageToDefinitiveDirectory (void);
void Img_ShowImage (const char *Image,const char *ClassImg);
void Img_RemoveImageFile (const char *ImageName);

#endif
