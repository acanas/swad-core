// swad_resource_database.h: resources, operations with database

#ifndef _SWAD_RSC_DB
#define _SWAD_RSC_DB
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

//----------------------------- Clipboard -------------------------------------
void Rsc_DB_CopyToClipboard (Rsc_Type_t Type,long ItmCod);

unsigned Rsc_DB_GetNumResourcesInClipboard (void);
unsigned Rsc_DB_GetClipboard (MYSQL_RES **mysql_res);

void Rsc_DB_RemoveClipboard (void);
void Rsc_DB_RemoveLinkFromClipboard (struct Rsc_Link *Link);

//------------------------ Resources in program -------------------------------
long Rsc_DB_CreateRsc (const struct Tre_Node *Node);

bool Rsc_DB_CheckListResources (long NodCod,bool ShowHiddenResources);
unsigned Rsc_DB_GetListResources (MYSQL_RES **mysql_res,long NodCod,
                                  bool ShowHiddenResources);
unsigned Rsc_DB_GetResourceDataByCod (MYSQL_RES **mysql_res,long ItmCod);

void Rsc_DB_UpdateRsc (const struct Tre_Node *Node);

#endif
