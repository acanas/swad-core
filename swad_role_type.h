// swad_role_type.h: user's role type

#ifndef _SWAD_ROL_T
#define _SWAD_ROL_T
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
/************************* Public constants and types ************************/
/*****************************************************************************/

// Related with user's roles
/*
   Don't change these numbers!
   They are used for users' permissions and for user's types in database

   Only Rol_STD, Rol_NET and Rol_TCH are allowed
   as user permanent roles in courses,
   but a user may be logged temporarily as other roles
*/
#define Rol_NUM_ROLES 10
typedef enum
  {
   // -------------------------------------------------------------------------
   Rol_UNK	= 0,	// User not logged in
   Rol_GST	= 1,	// User not belonging to any course
   Rol_USR	= 2,	// Student, non-editing teacher or teacher in some courses...
   	   	   	// ...but no course selected
			// ...or not belonging to the current course
   Rol_STD	= 3,	// Student in the current course
   // -------------------------------------------------------------------------
   Rol_NET	= 4,	// Non-editing teacher in the current course
   Rol_TCH	= 5,	// Teacher in the current course
   Rol_DEG_ADM	= 6,	// Degree administrator
   Rol_CTR_ADM	= 7,	// Center administrator
   // -------------------------------------------------------------------------
   Rol_INS_ADM	= 8,	// Institution administrator
   Rol_SYS_ADM	= 9,	// System administrator (superuser)
  } Rol_Role_t;

#endif
