// swad_FAQ.h: Frequently Asked Questions

#ifndef _SWAD_FAQ
#define _SWAD_FAQ
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

#include "swad_FAQ_type.h"
#include "swad_tree.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void FAQ_ResetSpcFields (struct Tre_Node *Node);

void FAQ_ListNodeQaAs (Tre_ListingType_t ListingType,
                       struct Tre_Node *Node,
                       long SelectedNodCod,
                       long SelectedQaACod,
                       HidVis_HiddenOrVisible_t HiddenOrVisible);

void FAQ_GetQaADataByCod (struct Tre_Node *Node);

void FAQ_WriteCellViewQaA (struct Tre_Node *Node,
			   HidVis_HiddenOrVisible_t HiddenOrVisible);
void FAQ_WriteCellEditQaA (struct Tre_Node *Node,
                           Vie_ViewType_t ViewType,
			   HidVis_HiddenOrVisible_t HiddenOrVisible);
void FAQ_WriteCellNewQaA (void);

void FAQ_CreateQaA (struct Tre_Node *Node);
void FAQ_RenameQaA (const struct Tre_Node *Node);
void FAQ_ChangeQaA (struct Tre_Node *Node);

#endif
