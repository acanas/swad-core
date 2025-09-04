// swad_API_database.h: SWAD web API provided to external plugins, operations with database

#ifndef _SWAD_API_DB
#define _SWAD_API_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include "swad_API.h"
#include "swad_exist.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void API_DB_CreateAPIKey (char APIKey[API_BYTES_KEY + 1],long UsrCod,long PlgCod);

Exi_Exist_t API_DB_CheckIfAPIKeyExists (char APIKey[API_BYTES_KEY + 1]);
Exi_Exist_t API_DB_GetDataFromAPIKey (MYSQL_RES **mysql_res,
				      char APIKey[API_BYTES_KEY + 1]);

void API_DB_RemoveOldAPIKeys (void);

#endif
