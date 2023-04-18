// swad_resource_database.h: resources, operations with database

#ifndef _SWAD_RSC_DB
#define _SWAD_RSC_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Ca�as Vargas

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

#include "swad_resource.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Rsc_DB_CopyToClipboard (Rsc_Type_t Type,long Cod);

unsigned Rsc_DB_GetNumResourcesInClipboard (void);
unsigned Rsc_DB_GetClipboard (MYSQL_RES **mysql_res);

void Rsc_DB_RemoveClipboard (void);
void Rsc_DB_RemoveLinkFromClipboard (struct Rsc_Link *Link);

#endif