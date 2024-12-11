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

void PrgRsc_GetRscDataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node);

void PrgRsc_WriteCellViewRsc (struct Tre_Node *Node);
void PrgRsc_WriteCellEditRsc (struct Tre_Node *Node,
				   Vie_ViewType_t ViewType,
				   __attribute__((unused)) HidVis_HiddenOrVisible_t HiddenOrVisible);
void PrgRsc_WriteCellNewRsc (void);

void PrgRsc_ViewResourceClipboard (void);
void PrgRsc_RemoveResourceClipboard (void);

//------------------------ Specific list of items -----------------------------
void PrgRsc_ViewResourcesAfterEdit (void);
void PrgRsc_EditListResources (void);
void PrgRsc_CreateResource (void);
void PrgRsc_ReqRemResource (void);
void PrgRsc_RemoveResource (void);
void PrgRsc_HideResource (void);
void PrgRsc_UnhideResource (void);
void PrgRsc_MoveUpResource (void);
void PrgRsc_MoveDownResource (void);
void PrgRsc_EditTreeWithFormResource (void);
void PrgRsc_ChangeResourceLink (void);
void PrgRsc_GetParsRsc (struct Tre_Node *Node);

#endif
