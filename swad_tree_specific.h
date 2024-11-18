// swad_tree_specific.h: list of specific items in tree nodes

#ifndef _SWAD_TRE_SPC
#define _SWAD_TRE_SPC
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
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define TreSpc_LIST_ITEMS_SECTION_ID	"spc_section"

#define TreSpc_NUM_MOVEMENTS_UP_DOWN 2
typedef enum
  {
   TreSpc_MOVE_UP,
   TreSpc_MOVE_DOWN,
  } TreSpc_MoveUpDown_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TreSpc_ResetListItem (struct Tre_Node *Node);
void TreSpc_ViewListItemsAfterEdit (void);
void TreSpc_EditListItems (void);
void TreSpc_EditTreeWithFormListItem (void);

void TreSpc_RenameListItem (void);

void TreSpc_ReqRemListItem (void);
void TreSpc_RemoveListItem (void);

void TreSpc_HideListItem (void);
void TreSpc_UnhideListItem (void);

void TreSpc_MoveUpListItem (void);
void TreSpc_MoveDownListItem (void);

#endif
