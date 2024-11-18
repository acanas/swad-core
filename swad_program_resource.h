// swad_program_resource.h: course program (resources)

#ifndef _SWAD_PRG_RSC
#define _SWAD_PRG_RSC
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

void PrgRsc_ViewResourcesAfterEdit (void);
void PrgRsc_EditResources (void);

void PrgRsc_ListNodeResources (Tre_ListingType_t ListingType,
                               struct Tre_Node *Node,
                               long SelectedNodCod,
                               long SelectedRscCod);

void PrgRsc_GetResourceDataByCod (struct Tre_Node *Node);

void PrgRsc_CreateResource (void);

void PrgRsc_RenameResource (const struct Tre_Node *Node);

void PrgRsc_HideResource (void);
void PrgRsc_UnhideResource (void);

void PrgRsc_MoveUpResource (void);
void PrgRsc_MoveDownResource (void);

void PrgRsc_ViewResourceClipboard (void);
void PrgRsc_RemoveResourceClipboard (void);

void PrgRsc_ChangeLink (void);

#endif
