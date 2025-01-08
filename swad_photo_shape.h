// swad_photo_shape.h: User photo shape

#ifndef _SWAD_PHO_SHA
#define _SWAD_PHO_SHA
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

// Related with user photo shape
#define PhoSha_NUM_SHAPES 4
typedef enum
  {
   PhoSha_SHAPE_CIRCLE    = 0,
   PhoSha_SHAPE_ELLIPSE   = 1,
   PhoSha_SHAPE_OVAL      = 2,
   PhoSha_SHAPE_RECTANGLE = 3,
  } PhoSha_Shape_t;
#define PhoSha_SHAPE_DEFAULT PhoSha_SHAPE_CIRCLE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void PhoSha_PutIconsToSelectPhotoShape (void);
void PhoSha_ChangePhotoShape (void);
PhoSha_Shape_t PhoSha_GetParPhotoShape (void);
PhoSha_Shape_t PhoSha_GetShapeFromStr (const char *Str);

void PhoSha_GetAndShowNumUsrsPerPhotoShape (void);

#endif
