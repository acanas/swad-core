// swad_photo_shape.h: User photo shape

#ifndef _SWAD_PHO_SHA
#define _SWAD_PHO_SHA
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#define Pho_NUM_SHAPES 4
typedef enum
  {
   Pho_SHAPE_CIRCLE    = 0,
   Pho_SHAPE_ELLIPSE   = 1,
   Pho_SHAPE_OVAL      = 2,
   Pho_SHAPE_RECTANGLE = 3,
  } Pho_Shape_t;
#define Pho_SHAPE_DEFAULT Pho_SHAPE_CIRCLE

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Pho_PutIconsToSelectPhotoShape (void);
void Pho_ChangePhotoShape (void);
Pho_Shape_t Pho_GetParamPhotoShape (void);
Pho_Shape_t Pho_GetShapeFromStr (const char *Str);

#endif
