// swad_program.h: course program

#ifndef _SWAD_PRG
#define _SWAD_PRG
/*
    SWAD (Shared Workspace At a Distance),
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_tree.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Prg_BeforeTree (Tre_ListingType_t ListingType);
void Prg_AfterTree (void);

void Prg_ShowProgram (void);
void Prg_EditTree (void);
void Prg_ViewNodeAfterEdit (void);
void Prg_ReqChangeNode (void);
void Prg_ReqCreateNode (void);
void Prg_ReceiveChgNode (void);
void Prg_ReceiveNewNode (void);
void Prg_ReqRemNode (void);
void Prg_RemoveNode (void);
void Prg_HideNode (void);
void Prg_UnhideNode (void);
void Prg_MoveUpNode (void);
void Prg_MoveDownNode (void);
void Prg_MoveLeftNode (void);
void Prg_MoveRightNode (void);
void Prg_ExpandNodeSeeing (void);
void Prg_ContractNodeSeeing (void);
void Prg_ExpandNodeEditing (void);
void Prg_ContractNodeEditing (void);

//-------------------------------- Figures ------------------------------------
void Prg_GetAndShowCourseProgramStats (void); // TODO: Change function from assignments to schedule

#endif
