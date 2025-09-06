// swad_link_database.h: course links, operations with database

#ifndef _SWAD_LNK_DB
#define _SWAD_LNK_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_link_type.h"
#include "swad_hidden_visible.h"
#include "swad_tree.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long Lnk_DB_CreateLnk (const struct Tre_Node *Node);
unsigned Lnk_DB_GetListLnks (MYSQL_RES **mysql_res,long NodCod,
                                 bool ShowHiddenCrsLinks);
Exi_Exist_t Lnk_DB_GetLnkDataByCod (MYSQL_RES **mysql_res,long ItmCod);

void Lnk_DB_UpdateLnk (const struct Tre_Node *Node);

#endif
