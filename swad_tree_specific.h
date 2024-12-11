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

#define TreSpc_NUM_UP_DOWN 2
typedef enum
  {
   TreSpc_UP,
   TreSpc_DOWN,
  } TreSpc_UpDown_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void TreSpc_ResetItem (struct Tre_Node *Node);

void TreSpc_GetItemDataByCod (struct Tre_Node *Node);

void TreSpc_ListNodeItems (Tre_ListingType_t ListingType,
			   struct Tre_Node *Node,
			   long SelectedNodCod,
			   long SelectedItmCod,
			   HidVis_HiddenOrVisible_t HiddenOrVisible);

void TreSpc_PutParItmCod (void *ItmCod);
void TreSpc_ViewListItemsAfterEdit (Inf_Type_t InfoType);
void TreSpc_EditListItems (Inf_Type_t InfoType);
void TreSpc_EditTreeWithFormItem (Inf_Type_t InfoType);

void TreSpc_ChangeItem (Inf_Type_t InfoType);

void TreSpc_ReqRemItem (Inf_Type_t InfoType);
void TreSpc_RemoveItem (Inf_Type_t InfoType);

void TreSpc_HideOrUnhideItem (Inf_Type_t InfoType,
			      HidVis_HiddenOrVisible_t HiddenOrVisible);

void TreSpc_MoveUpDownItem (Inf_Type_t InfoType,TreSpc_UpDown_t UpDown);

#endif
