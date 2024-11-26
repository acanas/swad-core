// swad_bibliography.h: course bibliography

#ifndef _SWAD_BIB
#define _SWAD_BIB
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

void Bib_ResetSpcFields (struct Tre_Node *Node);

void Bib_GetBibRefDataFromRow (MYSQL_RES *mysql_res,struct Tre_Node *Node);

void Bib_WriteCellViewBibRef (struct Tre_Node *Node,
			      HidVis_HiddenOrVisible_t HiddenOrVisible);
void Bib_WriteCellEditBibRef (struct Tre_Node *Node,
                              Vie_ViewType_t ViewType,
			      HidVis_HiddenOrVisible_t HiddenOrVisible);
void Bib_WriteCellNewBibRef (void);

void Bib_CreateBibRef (struct Tre_Node *Node);
void Bib_ChangeBibRef (struct Tre_Node *Node);

#endif
