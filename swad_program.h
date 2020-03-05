// swad_program.h: course program

#ifndef _SWAD_PRG
#define _SWAD_PRG
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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

#include "swad_hierarchy.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prg_ShowCourseProgram (void);

void Prg_RequestCreatePrgItem (void);
void Prg_RequestChangePrgItem (void);
void Prg_RecFormNewPrgItem (void);
void Prg_RecFormChgPrgItem (void);

void Prg_ReqRemPrgItem (void);
void Prg_RemovePrgItem (void);
void Prg_HidePrgItem (void);
void Prg_UnhidePrgItem (void);

void Prg_MoveUpPrgItem (void);
void Prg_MoveDownPrgItem (void);
void Prg_MoveLeftPrgItem (void);
void Prg_MoveRightPrgItem (void);

void Prg_RecFormPrgItem (void);
void Prg_RemoveCrsItems (long CrsCod);
unsigned Prg_GetNumItemsInCrsProgram(long CrsCod);

unsigned Prg_GetNumCoursesWithItems (Hie_Level_t Scope);
unsigned Prg_GetNumItems (Hie_Level_t Scope);

#endif
