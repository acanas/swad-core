// swad_program.h: course program

#ifndef _SWAD_PRG
#define _SWAD_PRG
/*
    SWAD (Shared Workspace At a Distance),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_hierarchy_level.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prg_ShowCourseProgram (void);

void Prg_RequestCreateItem (void);
void Prg_RequestChangeItem (void);
void Prg_ReceiveFormNewItem (void);
void Prg_ReceiveFormChgItem (void);

void Prg_ReqRemItem (void);
void Prg_RemoveItem (void);
void Prg_HideItem (void);
void Prg_UnhideItem (void);

void Prg_MoveUpItem (void);
void Prg_MoveDownItem (void);
void Prg_MoveLeftItem (void);
void Prg_MoveRightItem (void);

void Prg_RemoveCrsItems (long CrsCod);

unsigned Prg_GetNumCoursesWithItems (Hie_Lvl_Level_t Scope);
unsigned Prg_GetNumItems (Hie_Lvl_Level_t Scope);

#endif
