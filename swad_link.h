// swad_link.h: course links

#ifndef _SWAD_LNK
#define _SWAD_LNK
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

#include "swad_link_type.h"
#include "swad_tree.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Lnk_ResetSpcFields (struct Tre_Node *Node);

void Lnk_GetCrsLinkDataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node);

void Lnk_WriteCellViewCrsLink (struct Tre_Node *Node);
void Lnk_WriteCellEditCrsLink (struct Tre_Node *Node,
                               Vie_ViewType_t ViewType,
			       HidVis_HiddenOrVisible_t HiddenOrVisible);
void Lnk_WriteCellNewCrsLink (void);

void Lnk_GetParsCrsLink (struct Tre_Node *Node);

#endif
