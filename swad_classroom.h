// swad_classroom.h: classrooms, laboratories or other classrooms where classes are taught

#ifndef _SWAD_CLA
#define _SWAD_CLA
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#include "swad_string.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Cla_MAX_CHARS_CLASSROOM_SHRT_NAME	(32 - 1)	// 31
#define Cla_MAX_BYTES_CLASSROOM_SHRT_NAME	((Cla_MAX_CHARS_CLASSROOM_SHRT_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 511

#define Cla_MAX_CHARS_CLASSROOM_FULL_NAME	(128 - 1)	// 127
#define Cla_MAX_BYTES_CLASSROOM_FULL_NAME	((Cla_MAX_CHARS_CLASSROOM_FULL_NAME + 1) * Str_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Cla_MAX_CAPACITY_OF_A_CLASSROOM	10000	// If capacity of a classroom is greater than this, it is considered infinite
#define Cla_UNLIMITED_CAPACITY		INT_MAX	// This number can be stored in database as an integer...
						// ...and means that a classroom has no limited capacity

struct Classroom
  {
   long ClaCod;
   long InsCod;
   char ShrtName[Cla_MAX_BYTES_CLASSROOM_SHRT_NAME + 1];
   char FullName[Cla_MAX_BYTES_CLASSROOM_FULL_NAME + 1];
   unsigned Capacity;	// Seating capacity (maximum number of people that fit in the room)
  };

#define Cla_NUM_ORDERS 2
typedef enum
  {
   Cla_ORDER_BY_CLASSROOM = 0,
   Cla_ORDER_BY_CAPACITY  = 1,
  } Cla_Order_t;
#define Cla_ORDER_DEFAULT Cla_ORDER_BY_CLASSROOM

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Cla_SeeClassrooms (void);
void Cla_EditClassrooms (void);
void Cla_PutIconToViewClassrooms (void);
void Cla_GetListClassrooms (void);
void Cla_FreeListClassrooms (void);
void Cla_GetDataOfClassroomByCod (struct Classroom *Cla);
long Cla_GetParamClaCod (void);
void Cla_RemoveClassroom (void);
void Cla_RenameClassroomShort (void);
void Cla_RenameClassroomFull (void);
void Cla_ChangeMaxStudents (void);
void Cla_RecFormNewClassroom (void);

#endif
