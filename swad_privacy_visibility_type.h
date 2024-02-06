// swad_privacy_visibility_type.h: definition of privacy visibility type

#ifndef _SWAD_PRI_VIS_T
#define _SWAD_PRI_VIS_T
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
/************************* Public types and constants ************************/
/*****************************************************************************/

/***** Visibility (who can see user's photo / public profile) *****/
#define Pri_NUM_OPTIONS_PRIVACY 5
typedef enum
  {
   Pri_VISIBILITY_UNKNOWN = 0,	// Only visible by me and my teachers if I am a student or my students if I am a teacher
   Pri_VISIBILITY_USER    = 1,	// Only visible by me and my teachers if I am a student or my students if I am a teacher
   Pri_VISIBILITY_COURSE  = 2,	// Visible by users sharing courses with me
   Pri_VISIBILITY_SYSTEM  = 3,	// Visible by any user logged in platform
   Pri_VISIBILITY_WORLD   = 4,	// Public, visible to all people, even unlogged visitors
  } Pri_Visibility_t;

#endif
