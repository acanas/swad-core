// swad_photo_visibility.h: Users' photo visibility

#ifndef _SWAD_PHO_VISIBILITY
#define _SWAD_PHO_VISIBILITY
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

/***** Photo visibility (who can see user's photo) *****/
#define Pho_NUM_VISIBILITIES 4

typedef enum
  {
   Pho_VISIBILITY_USER   = 0,	// Only visible by me and my teachers if I am a student or my students if I am a teacher
   Pho_VISIBILITY_COURSE = 1,	// Visible by users sharing courses with me
   Pho_VISIBILITY_SYSTEM = 2,	// Visible by any user logged in platform
   Pho_VISIBILITY_WORLD  = 3,	// Public, visible by all the people, even unlogged visitors
  } Pho_Visibility_t;

#define Pho_VISIBILITY_DEFAULT Pho_VISIBILITY_USER

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

#endif
