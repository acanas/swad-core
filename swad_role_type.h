// swad_role_type.h: user's role type

#ifndef _SWAD_ROL_T
#define _SWAD_ROL_T
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
/********************************** Headers **********************************/
/*****************************************************************************/


/*****************************************************************************/
/************************** Public constant and types ************************/
/*****************************************************************************/

// Related with user's roles
/*
   Don't change these numbers!
   They are used for users' permissions and for user's types in database

   Only Rol_ROLE_STUDENT and Rol_ROLE_TEACHER are allowed
   as user permanent roles in courses,
   but a user may be logged temporarily as other roles
*/
// TODO: Teachers/students should be teachers/students only inside their courses
//       A user with courses (but not logged as admin) should be Rol_VISITOR outside his/her courses
//	 (Rol_VISITOR should be labeled as "User" instead "Visitor")
#define Rol_NUM_ROLES 9
typedef enum
  {
   Rol_UNKNOWN = 0,	// User not logged in
   Rol__GUEST_ = 1,	// User not belonging to any course
   Rol_VISITOR = 2,	// Student or teacher in other courses...
   	   	   	// ...but not belonging to the current course
   Rol_STUDENT = 3,	// Student in current course
   Rol_TEACHER = 4,	// Teacher in current course
   Rol_DEG_ADM = 5,	// Degree administrator
   Rol_CTR_ADM = 6,	// Centre administrator
   Rol_INS_ADM = 7,	// Institution administrator
   Rol_SYS_ADM = 8,	// System administrator (superuser)
  } Rol_Role_t;

#endif
