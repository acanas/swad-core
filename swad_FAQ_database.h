// swad_FAQ_database.h: Frequently Asked Questions, operations with database

#ifndef _SWAD_FAQ_DB
#define _SWAD_FAQ_DB
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include "swad_tree.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

long FAQ_DB_CreateQaA (const struct Tre_Node *Node);
unsigned FAQ_DB_GetListQaAs (MYSQL_RES **mysql_res,long NodCod,
                             Lay_Show_t ShowHiddenQaAs);
Exi_Exist_t FAQ_DB_GetQaADataByCod (MYSQL_RES **mysql_res,long ItmCod);

void FAQ_DB_UpdateQaA (const struct Tre_Node *Node);

#endif
